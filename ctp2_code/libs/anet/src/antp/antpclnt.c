/* 
Copyright (C) 1995-2001 Activision, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "aneterr.h"
#include "antpget.h"
#include "antp.h"
#include "wmqfile.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#undef DPRINT
#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
#define DPRINT(a) printf a
/* fake functions so we don't have to include dp2.lib */
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)			/* printf-style arguments on stack (if any) */
{
#include <stdarg.h>
	va_list argptr = NULL;
	int len = 0;

	if (__format) {
		va_start(argptr, __format);
		len = vprintf(__format, argptr);
		va_end(argptr);
	}
	return len;
}
#else
#define DPRINT(a)
#endif
/* fake functions so we don't have to include dp2.lib */
dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt)
{
	printf("dpReportAssertionFailure: %s, %d: %s\n", file, lineno, linetxt);
	return dp_RES_OK;
}

#define BUFLEN		1024	/* size of data buffer */
#define MAX_URLS	16		/* number of URLs to monitor */
#define MAX_RETRIES	5		/* connect retries before giving up (0=inf) */
#define RETRYTIME_CONNECT	60		/* seconds between connection retries */
#define RETRYTIME_GET	30	/* seconds between retries for new data.
							 * Note that this should be less than the
							 * connection timeout time of antpserv or
							 * connections will not be persistent.
							 */

#define STATE_DISCONNECTED	0
#define STATE_WAIT			1
#define STATE_SENDGET		2
#define STATE_READHEAD		3
#define STATE_READDATA		4

/*--------------------------------------------------------------------------
 DPRINT() remaining system resources.
--------------------------------------------------------------------------*/
#ifdef _DEBUG
#define MAX_MEM 512
#define MAX_FD 256
#endif

static void checksys()
{
#ifdef _DEBUG
	char *memptr[MAX_MEM];
	int fd[MAX_FD];
	const char file[] = "/dev/null";
	int i;

	for (i = 0; i < MAX_MEM; i++) {
		memptr[i] = (char *)malloc(1024*1024);
		if (memptr[i] == NULL)
			break;
	}
	if (i >= MAX_MEM) {
		DPRINT(("checksys: memory: > %d MB\n", MAX_MEM));
	} else {
		DPRINT(("checksys: memory: %d MB\n", i));
	}
	for (i--; i >= 0; i--) {
		free(memptr[i]);
	}

	for (i = 0; i < MAX_FD; i++) {
		fd[i] = open(file, O_RDONLY);
		if (fd[i] < 0)
			break;
	}
	if (i >= MAX_FD) {
		DPRINT(("checksys: file descriptors: > %d\n", MAX_FD));
	} else {
		DPRINT(("checksys: file descriptors: %d\n", i));
	}
	for (i--; i >= 0; i--) {
		close(fd[i]);
	}
#endif
	return;
}

main(int argc, char **argv)
{
	dp_result_t err;
	time_t now;
	antpget_t *antpget[MAX_URLS];
	antpget_url_t url[MAX_URLS];
	long offset[MAX_URLS];
	int sock[MAX_URLS];
	int state[MAX_URLS];
	int retries[MAX_URLS];
	time_t next_retry[MAX_URLS];
	char localdir[MAX_URLS][80];
	FILE *localfp[MAX_URLS];
	int nurls;
	char buf[BUFLEN];
	int nread;
	FILE *antpfile;
	char temppath[antpget_URL_MAXLEN];
	int quit = FALSE;
	time_t next_checksys;

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	if (argc != 2) {
		printf("\
Usage: %s <filelist>\n\
	Mirror up to %d remote wmq dirs listed in <filelist> via antpserv.\n\
	If local files already exist, restart with a partial get at the end of\n\
	the last file, else start at the beginning of the first remote file.\n\
	Each line in <filelist> has the format:\n\
		antp://<remotehost>:<port> <localdir>\n\
	ex. antp://us1.alink.activision.com:11380 us1/\n",
			argv[0], MAX_URLS);
		exit(1);
	}
	if ((antpfile = fopen(argv[1], "r")) == NULL) {
		printf("%s: file %s not found\n", argv[0], argv[1]);
		exit(1);
	}
	nurls = 0;
	now = time(NULL);
	while ((fgets(buf, BUFLEN, antpfile) != NULL) && (nurls < MAX_URLS)
		&& (sscanf(buf, "%s %s", temppath, localdir[nurls]))) {
		struct stat st;

		/* check localdir */
		if (localdir[nurls][strlen(localdir[nurls]) - 1] == '/')
			localdir[nurls][strlen(localdir[nurls]) - 1] = '\0';
		if (stat(localdir[nurls], &st) == -1) {
			if (errno != ENOENT) {
				printf("stat %s error:%d\n", localdir[nurls], errno);
				exit(1);
			} else {
				/* no directory, try to create */
				if (mkdir(localdir[nurls], 0755) != 0) {
					printf("mkdir %s error:%d\n", localdir[nurls], errno);
					exit(1);
				}
				if (stat(localdir[nurls], &st) == -1) {
					printf("stat %s error:%d\n", localdir[nurls], errno);
					exit(1);
				}
			}
		}
		if (!S_ISDIR(st.st_mode)) {
			printf("%s exists but is not a directory\n", localdir[nurls]);
			exit(1);
		}
		if ((st.st_mode & S_IRWXU) != S_IRWXU) {
			printf("%s exists but permissions are wrong (mode:%o !&= %o)\n", localdir[nurls], st.st_mode, S_IRWXU);
			exit(1);
		}

		/* Check url */
		/* HACK: dp_RES_BAD indicates no filename */
		err = antpget_parseURL(temppath, &url[nurls]);
		if (err == dp_RES_BAD) {
			err = wmqfile_last(localdir[nurls], url[nurls].path);
			if (err == dp_RES_OK) {
				/* We will do a partial GET at the end of the last file */
				sprintf(temppath, "%s/%s", localdir[nurls], url[nurls].path);
				if (((localfp[nurls] = fopen(temppath, "ab")) == NULL)
				||  ((offset[nurls] = ftell(localfp[nurls])) == -1L)) {
					if (localfp[nurls]) {
						printf("antp_get: ftell(%s, end) failed\n", temppath);
						fclose(localfp[nurls]);
						localfp[nurls] = NULL;
					} else {
						printf("antp_get: fopen(%s) failed\n", temppath);
					}
					offset[nurls] = 0;
					exit(1);
				}
			} else if (err == dp_RES_EMPTY) {
				/* We will send a FIRST command */
				url[nurls].path[0] = '\0';
				localfp[nurls] = NULL;
				offset[nurls] = 0;
			} else {
				printf("antp_get: wmqfile_last out of memory\n");
				exit(1);
			}
		} else if (err != dp_RES_OK)
			exit(1);

		antpget[nurls] = antpget_create();
		if (!antpget[nurls]) {
			printf("%s: out of memory for url %d!\n", argv[0], nurls + 1);
			exit(1);
		}
		sock[nurls] = antpget_setHost(antpget[nurls], url[nurls].host, url[nurls].port);
		if (sock[nurls] < 0) {
			printf("%s: can't bind socket for url %d.\n", argv[0], nurls + 1);
			exit(1);
		}
		retries[nurls] = 0;
		next_retry[nurls] = now;
		state[nurls] = STATE_DISCONNECTED;
		nurls++;
	}
	printf("%s: connecting to %d servers now\n", argv[0], nurls);

	next_checksys = now;
	while (!quit) {
		fd_set rfds;
		fd_set wfds;
		struct timeval tv;
		int i;
		int nsocks;
		int ndisc = 0;
		int sockmax;

		tv.tv_sec = 5;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		sockmax = -1;
		quit = TRUE;
		for (i = 0; i < nurls; i++) {
			if (sock[i] >= 0) {
				quit = FALSE;
				switch (state[i]) {
				case STATE_WAIT:
					/* Wait for retry to check for more data */
					if ((long)(now - next_retry[i]) < 0)
						continue;
					/* Done waiting, send a get */
					state[i] = STATE_SENDGET;
					FD_SET(sock[i], &wfds);
					break;
				case STATE_SENDGET:
					FD_SET(sock[i], &wfds);
					break;
				case STATE_READHEAD:
				case STATE_READDATA:
					FD_SET(sock[i], &rfds);
					break;
				case STATE_DISCONNECTED:
					ndisc++;
					continue;
				default:
					continue;
				}
				if (sock[i] > sockmax)
					sockmax = sock[i];
			}
		}
		nsocks = select(sockmax + 1, &rfds, &wfds, NULL, &tv);		
		if (nsocks < 0) {
			DPRINT(("select error!\n"));
			break;
		}

		now = time(NULL);
		if ((long)(now - next_checksys) >= 0) {
			DPRINT(("server: t:%d system resources:\n", now));
			checksys();
			next_checksys = now + 60;
		}
		/* DPRINT(("t:%d - checking sockets, %d to r/w, %d to reconnect\n", now, nsocks, ndisc)); */
		if ((nsocks == 0) && (ndisc == 0))
			continue;
		for (i = 0; i < nurls; i++) {
		/* DPRINT(("%s:%d/%s - state:%d (internal:%d)\n", url[i].host, url[i].port, url[i].path, state[i], antpget[i]->respState)); */
			switch(state[i]) {
			case STATE_DISCONNECTED:
				/* Reconnect if we're ready */
				if (((long)(now - next_retry[i]) >= 0) && (sock[i] >= 0)) {
					err = antpget_connect(antpget[i]);
					if (err == dp_RES_HOST_NOT_RESPONDING) {
						retries[i]++;
						next_retry[i] = now + RETRYTIME_CONNECT;
						DPRINT(("%s:%d t:%d - connect retry %d failed, next retry at t:%d\n",
								url[i].host, url[i].port, now, retries[i], next_retry[i]));
						if (MAX_RETRIES && (retries[i] >= MAX_RETRIES))
							sock[i] = -1;  /* stop trying this url */
						continue;
					} else if (err != dp_RES_OK) {
						DPRINT(("%s:%d - connect error:%d\n",
								url[i].host, url[i].port, err));
						sock[i] = -1;  /* bad error, stop trying this url */
					}
					retries[i] = 0;
					DPRINT(("%s:%d - sending GET request now\n",
						url[i].host, url[i].port));
					state[i] = STATE_SENDGET;
				}
				break;

			case STATE_SENDGET:
				if (FD_ISSET(sock[i], &wfds)) {
					if (url[i].path[0])
						err = antpget_requestGet(antpget[i], url[i].path, offset[i]);
					else 
						err = antpget_requestFirst(antpget[i]);
					if (err == dp_RES_OK) {
						if (url[i].path[0])
							DPRINT(("%s:%d/%s - sent GET %d EOF\n", url[i].host,
								url[i].port, url[i].path, offset[i]));
						else
							DPRINT(("%s:%d - sent FIRST\n", url[i].host,
								url[i].port));
						state[i] = STATE_READHEAD;
					} else if (err == dp_RES_HOST_NOT_RESPONDING) {
						state[i] = STATE_DISCONNECTED;
					} else {
						if (url[i].path[0])
							DPRINT(("%s:%d/%s - requestGet error:%d\n",
								url[i].host, url[i].port, url[i].path, err));
						else
							DPRINT(("%s:%d - requestFirst error:%d\n",
								url[i].host, url[i].port, err));
						state[i] = STATE_DISCONNECTED;
						close(sock[i]);
						sock[i] = -1;  /* bad error, stop trying this url */
					}
				}
				break;

			case STATE_READHEAD:
				if (FD_ISSET(sock[i], &rfds)) {
					antpget_header_t *header;
					err = antpget_readHeader(antpget[i], &header);
					if ((err == dp_RES_OK) || (err == dp_RES_AGAIN)) {
						continue;
					} else if (err == dp_RES_EMPTY) {
						size_t newLength;
						int j;

						for (j = 0; j < header->nlines; j++)
							DPRINT(("%s:%d/%s> %s\n", url[i].host, url[i].port,
									url[i].path, header->line[j]));
						switch (header->status) {
						case ANTP_STATUS_OK:
							state[i] = STATE_READDATA;
							break;
						case ANTP_STATUS_NEWFILE:
							DPRINT(("%s:%d/%s> new file %s\n", url[i].host,
								url[i].port, url[i].path, header->path));
							strcpy(url[i].path, header->path);
							offset[i] = header->offset;
							if (localfp[i])
								fclose(localfp[i]);
							sprintf(temppath,"%s/%s", localdir[i], url[i].path);
							if ((localfp[i] = fopen(temppath, "wb")) == NULL) {
								DPRINT(("%s:%d/%s - Can't open new file!\n",
									url[i].host, url[i].port, url[i].path));
								close(sock[i]);
								sock[i] = -1;
								state[i] = STATE_DISCONNECTED;
							}
							if (header->length > 0)
								state[i] = STATE_READDATA;
							else	/* probably a response to a FIRST */
								state[i] = STATE_SENDGET;
							break;
						case ANTP_STATUS_EMPTY:
							next_retry[i] = now + RETRYTIME_GET;
							DPRINT(("%s:%d - sending next GET at t:%d\n",
								url[i].host, url[i].port, next_retry[i]));
							state[i] = STATE_WAIT;
							break;
						default:
							DPRINT(("%s:%d - unknown success status %03d\n",
								url[i].host, url[i].port, header->status));
							state[i] = STATE_DISCONNECTED;
							close(sock[i]);
							sock[i] = -1;  /* bad error, stop trying this url */
							break;
						}
					} else if (err == dp_RES_HOST_NOT_RESPONDING) {
						state[i] = STATE_DISCONNECTED;
					} else {
						if (header->line[0][0])
							DPRINT(("%s:%d/%s> %s\n", url[i].host, url[i].port,
								url[i].path, header->line[0]));
						DPRINT(("%s:%d/%s - readHeader error:%d\n",
							url[i].host, url[i].port, url[i].path, err));
						state[i] = STATE_DISCONNECTED;
						close(sock[i]);
						sock[i] = -1;  /* bad error, stop trying this url */
					}
				}
				if ((err != dp_RES_EMPTY) || (state[i] != STATE_READDATA))
					break;
				/* else fall through to READDATA while sock[i] is still set 
				 * to be read, in order to read the rest of the current buffer
				 * as data.
				 * If we don't do this and the entirety of the data fit in
				 * the header read buffer, select won't give us another chance
				 * to get at it.
				 */

			case STATE_READDATA:
				if (FD_ISSET(sock[i], &rfds)) {
					int total = 0;

					do {
						/* keep reading until we get a partial buffer */
						err = antpget_readData(antpget[i], buf, BUFLEN, &nread);
						if (((err == dp_RES_OK) || (err == dp_RES_EMPTY))
						&&  (nread > 0)) {
							int j;

							offset[i] += nread;
							total += nread;
							/* DPRINT(("%s:%d/%s> ", url[i].host, url[i].port,
									url[i].path));
							for (j = 0; j < nread; j++) {
								if (isprint(buf[j]))
									DPRINT(("%c", buf[j]));
								else if (buf[j] == '\n')
									DPRINT(("\n%s:%d/%s> ", url[i].host,
											url[i].port, url[i].path));
								else
									DPRINT(("\\%02x", (unsigned char)buf[j]));
							}
							DPRINT(("\n")); */
							if (fwrite(buf, nread, 1, localfp[i]) != 1) {
								DPRINT(("%s:%d/%s - fwrite error:%d\n",
									url[i].host, url[i].port, url[i].path,
									ferror(localfp[i])));
								state[i] = STATE_DISCONNECTED;
								close(sock[i]);
								sock[i] = -1; /* bad, stop trying this url */
							}
						}
					} while ((nread == BUFLEN) && (err == dp_RES_OK));
					if ((err == dp_RES_OK) || (err == dp_RES_AGAIN)) {
						DPRINT(("%s:%d/%s - read %d bytes total\n", url[i].host,
							url[i].port, url[i].path, total));
						if (total > 0)
							fflush(localfp[i]);
					} else if (err == dp_RES_EMPTY) {
						DPRINT(("%s:%d/%s - read %d bytes total\n",
							url[i].host, url[i].port, url[i].path, total));
						if (total > 0)
							fflush(localfp[i]);
						next_retry[i] = now;
						state[i] = STATE_SENDGET;
					} else if (err == dp_RES_HOST_NOT_RESPONDING) {
						state[i] = STATE_DISCONNECTED;
					} else {
						DPRINT(("%s:%d/%s - readData error:%d\n",
								url[i].host, url[i].port, url[i].path, err));
						state[i] = STATE_DISCONNECTED;
						close(sock[i]);
						sock[i] = -1;  /* bad error, stop trying this url */
					}
				}
				break;

			case STATE_WAIT:
				/* whenever multiple urls are being accessed, any that
				 * are in STATE_WAIT will slip through to here.
				 */
				break;

			default:
				DPRINT(("%s:%d/%s - bad state:%d\n",
						url[i].host, url[i].port, url[i].path, state[i]));
				state[i] = STATE_DISCONNECTED;
				close(sock[i]);
				sock[i] = -1;  /* bad error, stop trying this url */
				break;
			}
		}
	}
}
