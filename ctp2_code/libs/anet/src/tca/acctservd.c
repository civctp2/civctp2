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

/*--------------------------------------------------------------------------
 An interface to the password database for use with a CGI script or similar
 form of access.
--------------------------------------------------------------------------*/

#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include "anet.h"
#include "acctserv.h"
#include "antpserv.h"
#include "mywcs.h"

#define acctservd_SOCKETNAME	"/tmp/acctserv.sock"
#define acctservd_BUFLEN		256
#define acctservd_MAX_ARGS		5
#define acctservd_TIME_SAVE		600	/* secs between saves to disk */

#define acctservd_STATE_READREQUEST		1
#define acctservd_STATE_PROCESSREQUEST	2
#define acctservd_STATE_SENDERROR			3
#define acctservd_STATE_SENDHEADER		4

#if defined(_DEBUG) || defined(DEBUG) || defined(DPRNT)
int cdecl
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
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
#endif

dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt) 
{
	printf("dpReportAssertionFailure(%d, %s, %s)\n",
			lineno, file, linetxt);
	exit(1);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Checks inet addr to see if it is an allowed client.
 Returns 1 if allowed,
 		 0 if not.
--------------------------------------------------------------------------*/
static int acctservd_validateAddr(struct sockaddr_in addr)
{
	if (!strncmp(inet_ntoa(addr.sin_addr), "206.17.", 7))
		return 1;
	return 0;
}

/*--------------------------------------------------------------------------
 Handle an incoming account change request on <sockfd>. 
--------------------------------------------------------------------------*/
static void acctservd_handleRequest(int sockfd, acctserv_t *acctserv)
{
	dp_result_t err;
	char buf[acctservd_BUFLEN];
	char *pbuf;
	int buflen;
	int nbytes;
	char *argv[acctservd_MAX_ARGS];
	int argc = 0;
	int state;
	int bNewArg = 1;
	time_t now;

	/* wait for a new request */
	pbuf = buf;
	state = acctservd_STATE_READREQUEST;
	DPRINT(("acctservd: handling request at t:%d\n", time(NULL)));
	while (1) {
		char *c;

		now = time(NULL);
		switch (state) {
		case acctservd_STATE_READREQUEST:
			/* blocking read; wait for data */
			nbytes = read(sockfd, pbuf, acctservd_BUFLEN-(pbuf-buf));
			if (nbytes < 0) {
				DPRINT(("acctservd: read error:%d\n", errno));
				return;  /* read error, give up */
			} else if (nbytes == 0)
				continue;  /* no data, wait */

			DPRINT(("acctservd: read:"));
			for (c = pbuf; c < pbuf + nbytes; c++) {
				if (c > buf + acctservd_BUFLEN) {
					DPRINT(("\nacctservd: request too long for buf\n"));
					sprintf(buf, "%d %s\n", 4, "Buffer Overflow"); 
					c = buf;  /* pbuf = c;  as we exit for loop */
					buflen = strlen(buf);
					state = acctservd_STATE_SENDERROR;
					break;
				}
				if (*c == '\n') {
					/* newline triggers processing of a line */
					*c++ = '\0';
					state = acctservd_STATE_PROCESSREQUEST;
					break;
				} else if (isspace(*c)) {
					*c = '\0';
					if (!bNewArg) { DPRINT((" ")); }
					bNewArg = 1;
				} else if (!isprint(*c)) {
					DPRINT(("\nacctservd: bad char:%d in request\n", *c));
					sprintf(buf, "%d %s\n", 3, "Bad Character in Request"); 
					c = buf;  /* pbuf = c;  as we exit for loop */
					buflen = strlen(buf);
					state = acctservd_STATE_SENDERROR;
					break;
				} else if (bNewArg) {
					/* Non-space following whitespace starts an arg */
					if (argc < acctservd_MAX_ARGS)
						argv[argc++] = c;
					bNewArg = 0;
					DPRINT(("%c", *c));
				} else {
					DPRINT(("%c", *c));
				}
			}
			DPRINT(("\n"));
			pbuf = c;
			break;

		case acctservd_STATE_PROCESSREQUEST:
			switch (buf[0]) {
			case 'N':	/* N(ew-account) username password email */
				if (argc != 4) {
					sprintf(buf, "%d %s\n", 3, "Invalid Number of Arguments");
					break;
				}
				err = acctserv_account_create(acctserv, argv[1], argv[2],
					argv[3]);
				if (err == dp_RES_OK)
					sprintf(buf, "%d %s\n", 1, "Account Created, Please Activate");
				else if (err == dp_RES_ALREADY)
					sprintf(buf, "%d %s\n", 2, "Username In Use");
				else if (err == dp_RES_BADSIZE)
					sprintf(buf, "%d %s\n", 3, "Invalid Argument");
				else
					sprintf(buf, "%d %s\n", 4, "Server Error");
				break;
			case 'A':	/* A(ctivate-account) username password code */
				if (argc != 4) {
					sprintf(buf, "%d %s\n", 3, "Invalid Number of Arguments");
					break;
				}
				err = acctserv_account_activate(acctserv, argv[1], argv[2],
					argv[3]);
				if (err == dp_RES_OK || err == dp_RES_ALREADY)
					sprintf(buf, "%d %s\n", 0, "Account Activated");
				else if (err == dp_RES_ACCESS)
					sprintf(buf, "%d %s\n", 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					sprintf(buf, "%d %s\n", 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					sprintf(buf, "%d %s\n", 3, "Invalid Argument");
				else
					sprintf(buf, "%d %s\n", 4, "Server Error");
				break;
			case 'D':	/* D(elete-account) username password */
				if (argc != 3) {
					sprintf(buf, "%d %s\n", 3, "Invalid Number of Arguments");
					break;
				}
				err = acctserv_account_delete(acctserv, argv[1], argv[2]);
				if (err == dp_RES_OK)
					sprintf(buf, "%d %s\n", 0, "Account Deleted");
				else if (err == dp_RES_ACCESS)
					sprintf(buf, "%d %s\n", 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					sprintf(buf, "%d %s\n", 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					sprintf(buf, "%d %s\n", 3, "Invalid Argument");
				else
					sprintf(buf, "%d %s\n", 4, "Server Error");
				break;
			case 'P':	/* P(assword-change) username password newpassword */
				if (argc != 4) {
					sprintf(buf, "%d %s\n", 3, "Invalid Number of Arguments");
					break;
				}
				err = acctserv_account_change(acctserv, argv[1], argv[2],
					argv[3], NULL);
				if (err == dp_RES_OK)
					sprintf(buf, "%d %s\n", 0, "Password Changed");
				else if (err == dp_RES_ACCESS)
					sprintf(buf, "%d %s\n", 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					sprintf(buf, "%d %s\n", 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					sprintf(buf, "%d %s\n", 3, "Invalid Argument");
				else
					sprintf(buf, "%d %s\n", 4, "Server Error");
				break;
			case 'E':	/* E(mail-change) username password newemail */
				if (argc != 4) {
					sprintf(buf, "%d %s\n", 3, "Invalid Number of Arguments");
					break;
				}
				err = acctserv_account_change(acctserv, argv[1], argv[2],
					NULL, argv[3]);
				if (err == dp_RES_OK)
					sprintf(buf, "%d %s\n", 1, "Email Changed, Please Activate");
				else if (err == dp_RES_ACCESS)
					sprintf(buf, "%d %s\n", 2, "Incorrect Password");
				else if (err == dp_RES_EMPTY)
					sprintf(buf, "%d %s\n", 2, "No Such User");
				else if (err == dp_RES_BADSIZE)
					sprintf(buf, "%d %s\n", 3, "Invalid Argument");
				else
					sprintf(buf, "%d %s\n", 4, "Server Error");
				break;
			default:
				if (isprint(buf[0])) {
					DPRINT(("acctservd: bad cmd:%c in request\n", buf[0]));
				} else {
					DPRINT(("acctservd: bad cmd:0x%2x in request\n", buf[0]));
				}
				sprintf(buf, "%d %s\n", 3, "Bad Command");
				break;
			}
			pbuf = buf;
			buflen = strlen(buf);
			state = acctservd_STATE_SENDERROR;
			break;

		case acctservd_STATE_SENDERROR:
			nbytes = write(sockfd, pbuf, buflen);
			if ((nbytes == -1) && (errno != EAGAIN)) {
				DPRINT(("acctservd: write error:%d\n", errno));
				return;
			} else if (nbytes == buflen) {
				DPRINT(("acctservd: sent error:%s\n", buf));
				/* Finished sending error, now exit */
				return;
			} else if (nbytes > 0) {
				pbuf += nbytes;
				buflen -= nbytes;
			}
			break;

		default:
			DPRINT(("acctservd: bad state:%d\n", state));
			sprintf(buf, "%d %s\n", 4, "Server Error");
			return;
		}
	}
	return;
}

static dp_result_t acctservd_save(acctserv_t *acctserv)
{
	dp_result_t err;
	char oldfile[260];

	wmq_flush(acctserv->pwq->wmq);
	/* Rename the current database.
	 * Any processes currently reading will keep reading from this version.
	 */
	strcpy(oldfile, acctserv->tcapw->filename);
	strcat(oldfile, ".old");
	if (rename(acctserv->tcapw->filename, oldfile) == -1) {
		printf("acctservd: rename(%s,%s) error:%d\n", acctserv->tcapw->filename,
			oldfile, errno);
		return dp_RES_BUG;
	}
	/* Save the new db to disk as acctserv->tcapw->filename.
	 * This doesn't change tcapw.
	 * Future requests for the db will get this new version.
	 */
	err = tcapw_close(acctserv->tcapw);
	if (err != dp_RES_OK) {
		printf("acctservd: tcapw_close err:%d\n", err);
		return err;
	}
}

static acctserv_t *acctservd_restore(const char *dir, const char *dbfile)
{
	dp_result_t err;
	tcapw_t *tcapw;
	wmq_t *wmq;
	pwq_t *pwq;
	char PWFile[256];
	int len;
	time_t timestamp;
	long offset;

	len = strlen(dir);
	if (len > 240)
		return NULL;
	strcpy(PWFile, dir);
	if (dir[len-1] != '/') {
		PWFile[len++] = '/';
	}
	strcpy(PWFile + len, dbfile);

	tcapw = tcapw_create();
	if (!tcapw)
		return NULL;
	err = tcapw_open(tcapw, PWFile);
	if (err == dp_RES_EMPTY) {
		printf("acctservd: pwfile:%s not found\n", PWFile);
		return NULL;
	} else if (err != dp_RES_OK) {
		printf("acctservd: tcapw_open err:%d\n", err);
		return NULL;
	}

	pwq = pwq_create(tcapw, dir, 0, pwq_SERVERTAG_MASTER);
	if (!pwq)
		return NULL;
	err = pwq_tell(pwq, &timestamp, &offset);
	if (err == dp_RES_OK) {
		while (1) {
			pwq_message_t msg;

			err = pwq_get(pwq, &msg);
			if (err == dp_RES_EMPTY)
				break;
			else if (err == dp_RES_AGAIN)
				continue;
			else if (err != dp_RES_OK)
				return NULL;

			switch (msg.type) {
			case pwq_MESSAGETYPE_CREATE:
				err = tcapw_stuff_entry(tcapw, &msg.u.acctCreate);
				if (err != dp_RES_OK) {
					printf("acctservd: entry_stuff(%d) returns err:%d\n", msg.u.acctCreate.uid, err);
					return NULL;
				}
				break;
			case pwq_MESSAGETYPE_CHANGE:
				err = tcapw_entry_change(tcapw, msg.u.acctChange.uid, &msg.u.acctChange.hpw, msg.u.acctChange.flags, msg.u.acctChange.email);
				if (err != dp_RES_OK) {
					printf("servpw_poll: entry_change(%d) returns err:%d\n", msg.u.acctChange.uid, err);
					return NULL;
				}
				break;
			case pwq_MESSAGETYPE_DELETE:
				err = tcapw_entry_delete(tcapw, msg.u.acctDelete.uid);
				if (err != dp_RES_OK) {
					printf("acctservd: entry_delete(%d) returns err:%d\n", msg.u.acctDelete.uid, err);
					return NULL;
				}
				break;
			default:
				printf("acctservd: unknown message type:%c\n", msg.type);
				return NULL;
			}

			/* database was modified, so update its wmq pointer */
			err = pwq_tell(pwq, &timestamp, &offset);
			if (err != dp_RES_OK) {
				printf("acctservd: bug: pwq_tell returns err:%d\n", err);
				return NULL;
			}
			err = tcapw_setWmqPosition(tcapw, timestamp, offset);
			if (err != dp_RES_OK) {
				printf("servpw_poll: bug: setWmqPosition returns err:%d\n", err);
				return NULL;
			}
		}
	} else if (err != dp_RES_EMPTY) {
		printf("acctservd: bug: pwq_tell returns err:%d\n", err);
		return NULL;
	}

	pwq_destroy(pwq);  /* This incidentally saves tcapw to disk. */

	pwq = pwq_create(tcapw, dir, 1, pwq_SERVERTAG_MASTER);
	if (!pwq)
		return NULL;
	return acctserv_create(pwq);
}

void main(int argc, char *argv[])
{
	dp_result_t err;
	acctserv_t *acctserv;
	int sockunix, sockinet; 
	struct sockaddr_un addrunix;
	struct sockaddr_in addrinet; 
	time_t now, next_save;
	int MyPort;
	char MyDir[antpserv_MAXPATH];
	int len;
	int pidunix = 0;
	int pidinet = 0;
	int quit = 0;
	char DBFile[] = "prefs.dat";

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	if (argc < 3) {
		printf("\
Usage: %s <Dir> <Port>\n\
	Starts account server which handles local account change requests\n\
	and internet TCP file requests on <Port> for output files.\n\
	<Dir> is used as the working directory, and should contain a password\n\
	database file %s and any wmq files associated with it.\n", argv[0], DBFile);
		exit(0);
	}
	strncpy(MyDir, argv[1], antpserv_MAXPATH);
	MyDir[antpserv_MAXPATH - 1] = '\0';
	len = strlen(MyDir);
	if ((len > 1) && (MyDir[len-1] == '/'))
		MyDir[len-1] = '\0';
	MyPort = atoi(argv[2]);
	if (MyPort < 1024) {
		printf("acctserv: Invalid Port:%s\n", argv[2]);
		exit(1);
	}
	printf("acctservd: starting %s %s %d\n", argv[0], MyPort);

	close(0);		/* close stdin */

	acctserv = acctservd_restore(MyDir, DBFile);
	if (!acctserv) {
		printf("acctservd: error restoring from dir:%s\n", MyDir);
		exit(1);
	}

	printf("acctservd: creating unix domain socket\n");
	if ((sockunix = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
		printf("acctservd: socket error:%d\n", errno);
		exit(1);
	}
	DPRINT(("acctservd: creating inet socket\n"));
	if ((sockinet = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		DPRINT(("acctservd: socket error:%d\n", errno));
		exit(1);
	}

	printf("acctservd: binding unix domain socket\n");
	memset(&addrunix, 0, sizeof(addrunix));
	addrunix.sun_family = AF_LOCAL; 
	strcpy(addrunix.sun_path, acctservd_SOCKETNAME);
	unlink(addrunix.sun_path);
	if (bind(sockunix, (struct sockaddr *)&addrunix, sizeof(addrunix)) < 0) {
		printf("acctservd: bind error:%d\n", errno);
		exit(1);
	}
	if (chmod(addrunix.sun_path, 0777) == -1) {
		printf("acctservd: chmod error:%d\n", errno);
		exit(1);
	}
	DPRINT(("acctservd: binding inet socket\n"));
	memset(&addrinet, 0, sizeof(addrinet));
	addrinet.sin_family = AF_INET; 
	addrinet.sin_port = htons(MyPort); 
	addrinet.sin_addr.s_addr = htons(INADDR_ANY); 
	if (bind(sockinet, (struct sockaddr *)&addrinet, sizeof(addrinet)) < 0) {
		DPRINT(("acctservd: bind error:%d\n", errno));
		exit(1);
	}

	printf("acctservd: starting unix domain listen\n");
	if (listen(sockunix, 10) < 0) {
		printf("acctservd: listen error:%d\n", errno);
		exit(1);
	}
	DPRINT(("acctservd: starting inet listen\n"));
	if (listen(sockinet, 5) < 0) {
		DPRINT(("acctservd: listen error:%d\n", errno));
		exit(1);
	}

	next_save = time(NULL) + acctservd_TIME_SAVE;
	while (!quit) {
		struct timeval tv;
		int sockmax;
		int nsocks;
		fd_set rfds;
		int pid;

		if (pidunix) {
			pid = waitpid(pidunix, NULL, WNOHANG);
			if (pid == -1) {
				printf("acctservd: waitpid(%d) error:%d\n", pidunix, errno);
				exit(1);
			} else if (pid == pidunix) {
				pidunix = 0;
				printf("acctservd: unix domain acctservd process %d finished\n", pid);
			}
		}
		FD_ZERO(&rfds);
		if (!pidunix)	/* only accept one unix domain request at a time */
			FD_SET(sockunix, &rfds);
		FD_SET(sockinet, &rfds);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		sockmax = (sockinet > sockunix) ? sockinet : sockunix;
		nsocks = select(sockmax + 1, &rfds, NULL, NULL, &tv);
		if (nsocks < 0) {
			printf("acctservd: select error:%d\n", errno);
			break;
		}

		if (nsocks == 0) {
			if (!pidunix) {
				now = time(NULL);
				if ((long)(now - next_save) > 0) {
					err = acctservd_save(acctserv);
					if (err != dp_RES_OK) {
						printf("acctservd: save err:%d\n", err);
						exit(1);
					}
					printf("acctservd: database saved\n");
					next_save = now + acctservd_TIME_SAVE;
				}
			}
			continue;
		}

		if (FD_ISSET(sockunix, &rfds)) {
			int newsock;
			struct sockaddr_un client_addr;
			int client_addrlen;

			newsock = accept(sockunix, (struct sockaddr *)&client_addr,
				&client_addrlen);
			if (newsock < 0) {
				printf("acctservd: accept error:%d\n", errno);
				break;
			}
			printf("acctservd: unix domain connection requested on socket:%d\n",
				newsock);
			/* fork to handle the connection */
			switch (pidunix = fork()) {
			case -1:
				printf("acctservd: fork error:%d\n", errno);
				quit = 1;
				break;

			case 0:
				/* CHILD PROCESS */
				/* child only uses newsock */ 
				close(sockunix);
				close(sockinet);
				close(1);		/* close stdout */
				close(2);		/* close stderr */
				acctservd_handleRequest(newsock, acctserv); 
				close(newsock);
				exit(0);
				break;

			default:
				/* PARENT PROCESS */
				close(newsock);  /* only the child needs this socket now */
				break;
			}
		}
		if (FD_ISSET(sockinet, &rfds) && !quit) {
			int newsock;
			struct sockaddr_in client_addr;
			int client_addrlen;

			newsock = accept(sockinet, (struct sockaddr *)&client_addr,
				&client_addrlen);
			if (newsock < 0) {
				DPRINT(("acctservd: accept error:%d\n", errno));
				break;
			}
			DPRINT(("acctservd: connection requested by:%s on socket:%d\n", inet_ntoa(client_addr.sin_addr), newsock));
			if (acctservd_validateAddr(client_addr)) {
				switch (pid = fork()) {
				case -1:
					DPRINT(("acctservd: fork error:%d\n", errno));
					quit = 1;
					break;

				case 0:
					/* CHILD PROCESS */ 
					/* child only uses newsock */ 
					close(sockunix);
					close(sockinet);
					close(1);		/* close stdout */
					close(2);		/* close stderr */
					/* set our process group ID to 0, so init will handle cleanup */
					if (setpgid(0, 0) != 0) {
						DPRINT(("acctservd: setpgid error:%d\n", errno));
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
						close(newsock); 
						exit(0);  /* orphan the grandchild */
					}
					/* ORPHANED GRANDCHILD handles connection */
					antpserv_handleRequest(newsock, MyDir, DBFile); 
					close(newsock); 
					exit(0);
					break;

				default:
					/* PARENT PROCESS */
					close(newsock);  /* only the child needs this socket now */
					/* Wait for the child, which should take no time */
					if (waitpid(pidinet, NULL, 0) != pidinet) {
						DPRINT(("acctservd: inet child waitpid(%d) error:%d\n",
							pidinet, errno));
						quit = 1;
					}
					break;
				}
			} else {
				close(newsock);
				DPRINT(("acctservd: rejected inet connection from %s\n",
					inet_ntoa(client_addr.sin_addr)));
			}
		}
	}

	if (!pidunix) {
		err = acctservd_save(acctserv);
		if (err != dp_RES_OK) {
			printf("acctservd: save err:%d\n", err);
			exit(1);
		}
		printf("acctservd: database saved\n");
	}
	pwq_destroy(acctserv->pwq);
	tcapw_destroy(acctserv->tcapw);
	acctserv_destroy(acctserv);
	exit(0);
}
