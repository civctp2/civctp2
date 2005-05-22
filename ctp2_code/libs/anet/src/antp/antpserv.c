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
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <errno.h>
#include "aneterr.h"
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

#define BUFLEN		1024
#define URL_MAXLEN	256
#define TIMEOUT		90		/* connection timeout in seconds */

#define STATE_READREQUEST	1
#define STATE_SENDRESPONSE	2
#define STATE_SENDDATA		3

#define CMD_NONE			0
#define CMD_GET				1
#define CMD_FIRST			2
#define CMD_CLOSE			3

/*--------------------------------------------------------------------------
 DPRINT() remaining system resources.
--------------------------------------------------------------------------*/
#ifdef _DEBUG
#include <sys/stat.h>
#include <fcntl.h>

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

/*--------------------------------------------------------------------------
 Returns a pointer to the start of the next word in str, or NULL if the end
 of str is encountered.
 Words are defined as a series of non-whitespace characters (not ' ', '\t')
 separated by sequences of whitespace characters (' ' or '\t').
--------------------------------------------------------------------------*/
static const char *antp_nextword(const char *str)
{
	const char *p = str;

	while ((*p != ' ') && (*p != '\t')) {
		if (*p == '\0')
			return NULL;
		p++;
	}
	while ((*p == ' ') || (*p == '\t'))
		p++;
	if (*p == '\0')
		return NULL;
	return p;
}

/*--------------------------------------------------------------------------
 Checks addr to see if it is an allowed client.
 Returns TRUE if allowed,
 		 FALSE if not.
--------------------------------------------------------------------------*/
static int antp_validateAddr(struct sockaddr_in addr)
{
	if (!strncmp(inet_ntoa(addr.sin_addr), "206.17.", 7))
		return TRUE;
	return FALSE;
}

/*--------------------------------------------------------------------------
 Read an ANTP/1.0 request on <sockfd> and send an appropriate response back.
 Only files in <BaseDir> may be requested.
--------------------------------------------------------------------------*/
static void antp_handleRequest(int sockfd, const char *BaseDir)
{
	dp_result_t err;
	char buf[BUFLEN];
	int nbytes;
	char *pbuf;
	int bNewline;
	int RequestCmd;
	int state;
	char file[URL_MAXLEN];
	long offset;
	long length;
	long total;
	FILE *fp;
	time_t now;
	time_t timeout;
	time_t next_checksys;

	/* wait for a new request */
	now = time(NULL);
	timeout = now + TIMEOUT;
	next_checksys = now;
	pbuf = buf;
	bNewline = TRUE;
	RequestCmd = CMD_NONE;
	state = STATE_READREQUEST;
	while ((long)(now - timeout) < 0) {
		char *pline;
		char *c;

		now = time(NULL);
		if ((long)(now - next_checksys) >= 0) {
			DPRINT(("server: t:%d system resources:\n", now));
			checksys();
			next_checksys = now + 60;
		}
		switch (state) {
		case STATE_READREQUEST:
			/* blocking read; wait for data */
			nbytes = read(sockfd, pbuf, BUFLEN-(pbuf-buf));
			if (nbytes < 0) {
				DPRINT(("server: read error:%d\n", errno));
				return;  /* read error, give up */
			} else if (nbytes == 0)
				continue;  /* no data, wait */

			for (c = pbuf; c < pbuf + nbytes; c++) {
				if (c > buf + BUFLEN) {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_EBUG, "Server error");
					DPRINT(("server: header too long for buf\n"));
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				if (bNewline) {
					pline = c;  /* mark the beginning of this line */
					bNewline = FALSE;
				}
				if (!isprint(*c) && (*c != '\t') && (*c != '\n')
				&&  (*c != '\r')) {
					state = STATE_SENDRESPONSE;
					RequestCmd = CMD_NONE;
					DPRINT(("server: Invalid character %d in request\n", *c));
					break;
				}
				if (*c == '\r')
					*c = '\0';  /* \r ends a line but doesn't start a new one */
				if (*c != '\n')
					continue;

				/* newline triggers processing of a line */
				bNewline = TRUE;
				*c = '\0';
				/* blank line terminates request */
				if (*pline == '\0') {
					state = STATE_SENDRESPONSE;
					break;
				}

				/* first line is request line */
				if (pline == buf) {
					const char *p;
					if (!strncmp(pline, ANTP_TAG, strlen(ANTP_TAG))
					&&  (p = antp_nextword(pline + 8))) {
						if (!strncmp(p, "GET", 3)) {
							if ((p = antp_nextword(p))
							&&  (sscanf(p, "%s %ld %ld",
									file, &offset, &length) == 3)
							&&  isprint(file[0]) && (offset >= 0)
							&&  (length >= 0))
								RequestCmd = CMD_GET;
						} else if (!strncmp(p, "FIRST", 5)) {
							RequestCmd = CMD_FIRST;
						} else if (!strncmp(p, "CLOSE", 5)) {
							RequestCmd = CMD_CLOSE;
						} else {
							DPRINT(("server: Unknown command:%s\n", p));
						}
					} else {
						DPRINT(("server: non-%s request:%s\n", ANTP_TAG,pline));
					}
				} else {
					/* some other header line */
					/* currently ignored */
				}
			}
			pbuf = c;
			break;

		case STATE_SENDRESPONSE:
			switch (RequestCmd) {
				char *p;
				char path[URL_MAXLEN];

			case CMD_GET:
				/* Valid command received, reset timeout */
				timeout = now + TIMEOUT;

				/* limit gets to wmq filenames and to only one base
				 * directory to be safe.
				 */
				if (!wmqfile_iswmqfile(file)) {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_ENOFILE, "File not found");
					DPRINT(("server: Invalid filename:%s\n", file));
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				strcpy(path, BaseDir);
				strncat(path, file, URL_MAXLEN - strlen(BaseDir));
				if ((fp = fopen(path, "rb")) == NULL) {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_ENOFILE, "File not found");
					DPRINT(("server: file not found:%s\n", file));
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				if ((fseek(fp, 0, SEEK_END) != 0)
				||  ((total = ftell(fp)) == -1L)) {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_EBUG, "Server error");
					DPRINT(("server: could not fseek(end) of %s\n", file));
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				if (offset > total) {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_EBADOFFSET, "Bad offset");
					DPRINT(("server: offset %d > length %d of %s\n",
						offset, length, file));
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				if (offset == total) {
					char newfile[URL_MAXLEN];

					/* Success, but no new data here */
					/* Check for a new file */
					DPRINT(("server: no more data in file %s, checking for later file\n", file));
					err = wmqfile_next(BaseDir, file, newfile);
					if (err == dp_RES_OK) {
						fclose(fp);
						strcpy(path, BaseDir);
						strncat(path, newfile, URL_MAXLEN - strlen(BaseDir));
						DPRINT(("server: found later file %s\n", newfile));
						offset = 0;
						if ((fp = fopen(path, "rb")) == NULL) {
							sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
								ANTP_STATUS_EBUG, "Server error");
							DPRINT(("server: can't open new file %s\n",
								newfile));
							nbytes = write(sockfd, buf, strlen(buf));
							if (nbytes < strlen(buf))
								DPRINT(("server: write error:%d\n", errno));
							return;
						}
						if ((fseek(fp, 0, SEEK_END) != 0)
						||  ((total = ftell(fp)) == -1L)) {
							sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
								ANTP_STATUS_EBUG, "Server error");
							DPRINT(("server: can't fseek(end) of %s\n",
								newfile));
							nbytes = write(sockfd, buf, strlen(buf));
							if (nbytes < strlen(buf))
								DPRINT(("server: write error:%d\n", errno));
							return;
						}
						if (fseek(fp, 0, SEEK_SET) != 0) {
							sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
								ANTP_STATUS_EBUG, "Server error");
							DPRINT(("server: can't fseek(start) of %s\n",
								newfile));
							nbytes = write(sockfd, buf, strlen(buf));
							if (nbytes < strlen(buf))
								DPRINT(("server: write error:%d\n", errno));
							return;
						}
						if ((length == 0) || (length > total))
							length = total;
						sprintf(buf, "%s %03d %s %d %d\n\n", ANTP_TAG,
							ANTP_STATUS_NEWFILE, newfile, offset, length);
						DPRINT(("server: file changed to %s, sending 0,%d/%d\n",
							newfile, length, total));
						nbytes = write(sockfd, buf, strlen(buf));
						if (nbytes < strlen(buf)) {
							DPRINT(("server: write error:%d\n", errno));
							return;
						}
						if (length == 0) {
							/* reset and wait for a new request */
							pbuf = buf;
							bNewline = TRUE;
							RequestCmd = CMD_NONE;
							state = STATE_READREQUEST;
							fclose(fp);
							break;
						}
						state = STATE_SENDDATA;
						break;  /* Go send data from newfile */
					} else if (err == dp_RES_EMPTY) {
						sprintf(buf, "%s %03d %s %d %d\n\n", ANTP_TAG,
							ANTP_STATUS_EMPTY, file, offset, 0);
						DPRINT(("server: no new data in %s\n", file));
						nbytes = write(sockfd, buf, strlen(buf));
						if (nbytes < strlen(buf))
							DPRINT(("server: write error:%d\n", errno));
						/* reset and wait for a new request */
						pbuf = buf;
						bNewline = TRUE;
						RequestCmd = CMD_NONE;
						state = STATE_READREQUEST;
						fclose(fp);
						break;
					} else {
						sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
							ANTP_STATUS_EBUG, "Server error");
						DPRINT(("server: t:%d out of memory\n", now));
						checksys();
						nbytes = write(sockfd, buf, strlen(buf));
						if (nbytes < strlen(buf))
							DPRINT(("server: write error:%d\n", errno));
						return;
					}
				}
				if (fseek(fp, offset, SEEK_SET) != 0) {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_EBUG, "Server error");
					DPRINT(("server: fseek(start) failed for %s\n", file));
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				/* Success, send response header */
				if (length == 0)  /* length = 0 means to EOF */
					length = total - offset;
				else if (offset + length > total)
					length = total - offset;
				sprintf(buf, "%s %03d %s %d %d\n\n", ANTP_TAG,
					ANTP_STATUS_OK, file, offset, length);
				DPRINT(("server: sending %d,%d/%d from %s\n",
					offset, length, total, file));
				nbytes = write(sockfd, buf, strlen(buf));
				if (nbytes < strlen(buf)) {
					DPRINT(("server: write error:%d\n", errno));
					return;
				}
				state = STATE_SENDDATA;
				break;

			case CMD_FIRST:
				/* Valid command received, reset timeout */
				timeout = now + TIMEOUT;
				err = wmqfile_first(BaseDir, file);
				if (err == dp_RES_OK) {
					sprintf(buf, "%s %03d %s %d %d\n\n", ANTP_TAG,
						ANTP_STATUS_NEWFILE, file, 0, 0);
					DPRINT(("server: first file is %s\n", file));
				} else if (err == dp_RES_EMPTY) {
					sprintf(buf, "%s %03d %s %d %d\n\n", ANTP_TAG,
						ANTP_STATUS_EMPTY, "0", 0, 0);
					DPRINT(("server: no first file\n"));
				} else {
					sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
						ANTP_STATUS_EBUG, "Server error");
					DPRINT(("server: t:%d out of memory\n", now));
					checksys();
					nbytes = write(sockfd, buf, strlen(buf));
					if (nbytes < strlen(buf))
						DPRINT(("server: write error:%d\n", errno));
					return;
				}
				nbytes = write(sockfd, buf, strlen(buf));
				if (nbytes < strlen(buf)) {
					DPRINT(("server: write error:%d\n", errno));
					return;
				}
				/* reset and wait for a new request */
				pbuf = buf;
				bNewline = TRUE;
				RequestCmd = CMD_NONE;
				state = STATE_READREQUEST;
				break;

			case CMD_CLOSE:
				/* close immediately if the client is nice enough to ask */
				return;

			default:
				/* bad request */
				sprintf(buf, "%s %03d %s\n\n", ANTP_TAG,
					ANTP_STATUS_EBADREQ, "Bad Request");
				nbytes = write(sockfd, buf, strlen(buf));
				return;
			}
			break;

		case STATE_SENDDATA:
			{
				size_t sent = 0;
				while (sent < length) {
					size_t lentosend = BUFLEN;
					if (lentosend > length - sent)
						lentosend = length - sent;
					if (fread(buf, lentosend, 1, fp) != 1) {
						DPRINT(("server: fread error:%d\n", ferror(fp)));
						return;
					}
					if (write(sockfd, buf, lentosend) < lentosend) {
						DPRINT(("server: write error:%d\n", errno));
						return;
					}
					sent += lentosend;
				}
				DPRINT(("server: sent %d bytes of data\n", sent));
			}
			/* reset and wait for a new request */
			pbuf = buf;
			bNewline = TRUE;
			RequestCmd = CMD_NONE;
			state = STATE_READREQUEST;
			fclose(fp);
			break;

		default:
			DPRINT(("server: bad state:%d\n", state));
			return;
		}
	}
}

void main(int argc, char *argv[])
{
	int sockfd; 
	struct sockaddr_in my_addr; 
	int MyPort;
	char BaseDir[URL_MAXLEN];
	int pid;

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	if (argc != 3) {
		printf("\
Usage: %s <Port> <BaseDir>\n\
	Starts server which listens to port <Port> and handles %s requests\n\
	for files in the directory <BaseDir>\n", argv[0], ANTP_TAG);
		exit(0);
	}
	MyPort = atoi(argv[1]);
	if (strlen(argv[2]) > URL_MAXLEN - 11) {
		printf("\
Usage: %s <Port> <BaseDir>\n\
	<BaseDir> to long (%d characters max)\n", argv[0], URL_MAXLEN-11);
		exit(1);
	}
	strcpy(BaseDir, argv[2]);
	if (BaseDir[strlen(BaseDir) - 1] != '/')
		strcat(BaseDir, "/");
	if (MyPort < 0) {
		printf("\
Usage: %s <Port> <BaseDir>\n\
	Invalid <Port>\n", argv[0]);
		exit(1);
	}
	printf("server: starting %s %d %s\n", argv[0], MyPort, BaseDir);

	close(0);		/* close stdin */


	/* PARENT handles accept */
	DPRINT(("server: creating socket\n"));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		DPRINT(("server: socket error:%d\n", errno));
		exit(1);
	}

	DPRINT(("server: binding my local socket\n"));
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET; 
	my_addr.sin_port = htons(MyPort); 
	my_addr.sin_addr.s_addr = htons(INADDR_ANY); 
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		DPRINT(("server: bind error:%d\n", errno));
		exit(1);
	}

	DPRINT(("server: starting listen\n"));
	if (listen(sockfd, 5) < 0) {
		DPRINT(("server: listen error:%d\n", errno));
		exit(1);
	}

	while (1) {
		int newsock;
		struct sockaddr_in client_addr;
		int client_addrlen;

		/* ACCEPT A CONNECTION AND THEN CREATE A CHILD TO DO THE WORK */ 
		/* LOOP BACK AND WAIT FOR ANOTHER CONNECTION */ 
		DPRINT(("server: starting accept\n"));
		newsock = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
		if (newsock < 0) {
			DPRINT(("server: accept error:%d\n", errno));
			exit(1);
		}
		DPRINT(("server: connection requested by:%s on socket:%d\n", inet_ntoa(client_addr.sin_addr), newsock));
		if (antp_validateAddr(client_addr)) {
			switch (pid = fork()) {
			case -1:
				DPRINT(("server: fork error:%d\n", errno));
				exit(1);

			case 0:
				/* CHILD PROCESS */ 
				close(sockfd);	/* child only uses newsock */
#ifndef _DEBUG
				/* set our process group ID to 0, so init will handle cleanup */
				if (setpgid(0, 0) != 0) {
					DPRINT(("server: setpgid error:%d\n", errno));
					exit(1);
				}
				close(1);		/* close stdout */
				close(2);		/* close stderr */
				switch (pid = fork()) {
				case -1:
					exit(1);
				case 0:		/* ORPHANED GRANDCHILD */
					break;
				default:	/* CHILD */
					exit(0);  /* orphan the grandchild */
				}
				/* ORPHANED GRANDCHILD handles connection */
#else
				/* _DEBUG - only one at a time, CHILD handles connection */
#endif
				antp_handleRequest(newsock, BaseDir); 
				close(newsock); 
				exit(0);
				break;

			default:
				/* PARENT PROCESS */
				close(newsock);  /* only the child needs this socket now */
				if (wait(NULL) != pid) {
					DPRINT(("server: wait for child %d failed\n", pid));
					exit(1);
				}
				break;
			}
		} else {
			close(newsock);
			DPRINT(("server: rejected connection from %s\n", inet_ntoa(client_addr.sin_addr)));
		}
	}
} 
