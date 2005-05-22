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
#include "anet.h"
#include "antpserv.h"

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
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
#endif

/* fake functions so we don't have to include dp2.lib */
dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt)
{
	printf("dpReportAssertionFailure: %s, %d: %s\n", file, lineno, linetxt);
	return dp_RES_OK;
}

extern char DBFile[antpserv_MAXPATH];

/*--------------------------------------------------------------------------
 Checks addr to see if it is an allowed client.
 Returns 1 if allowed,
 		 0 if not.
--------------------------------------------------------------------------*/
static int antpservd_validateAddr(struct sockaddr_in addr)
{
	if (!strncmp(inet_ntoa(addr.sin_addr), "206.17.", 7))
		return 1;
	return 0;
}

void main(int argc, char *argv[])
{
	int sockfd; 
	struct sockaddr_in my_addr; 
	int MyPort;
	char BaseDir[antpserv_MAXPATH];
	int pid;

	setlinebuf(stdout);  /* line buffer if we are redirecting */

	if (argc < 3) {
		printf("\
Usage: %s <Port> <BaseDir> [dbfile]\n\
	Starts server which listens to port <Port> and handles %s requests\n\
	for files in the directory <BaseDir>.\n\
	If <dbfile> is specified, FIRST requests will <dbfile> first, followed\n\
	by the wmqfile pointed to by <dbfile>\n", argv[0], ANTP_TAG);
		exit(0);
	}
	MyPort = atoi(argv[1]);
	if (strlen(argv[2]) > antpserv_MAXPATH - 11) {
		printf("\
Usage: %s <Port> <BaseDir> [dbfile]\n\
	<BaseDir> to long (%d characters max)\n", argv[0], antpserv_MAXPATH-11);
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
	if (argc > 3) {
		strncpy(DBFile, argv[3], antpserv_MAXPATH);
		DBFile[antpserv_MAXPATH-1] = '\0';
	} else
		DBFile[0] = '\0';
	printf("antpservd: starting %s %d %s %s\n", argv[0], MyPort, BaseDir, DBFile);

	close(0);		/* close stdin */


	/* PARENT handles accept */
	DPRINT(("antpservd: creating socket\n"));
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		DPRINT(("antpservd: socket error:%d\n", errno));
		exit(1);
	}

	DPRINT(("antpservd: binding my local socket\n"));
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET; 
	my_addr.sin_port = htons(MyPort); 
	my_addr.sin_addr.s_addr = htons(INADDR_ANY); 
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		DPRINT(("antpservd: bind error:%d\n", errno));
		exit(1);
	}

	DPRINT(("antpservd: starting listen\n"));
	if (listen(sockfd, 5) < 0) {
		DPRINT(("antpservd: listen error:%d\n", errno));
		exit(1);
	}

	while (1) {
		int newsock;
		struct sockaddr_in client_addr;
		int client_addrlen;

		/* ACCEPT A CONNECTION AND THEN CREATE A CHILD TO DO THE WORK */ 
		/* LOOP BACK AND WAIT FOR ANOTHER CONNECTION */ 
		DPRINT(("antpservd: starting accept\n"));
		newsock = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrlen);
		if (newsock < 0) {
			DPRINT(("antpservd: accept error:%d\n", errno));
			exit(1);
		}
		DPRINT(("antpservd: connection requested by:%s on socket:%d\n", inet_ntoa(client_addr.sin_addr), newsock));
		if (antpservd_validateAddr(client_addr)) {
			switch (pid = fork()) {
			case -1:
				DPRINT(("antpservd: fork error:%d\n", errno));
				exit(1);

			case 0:
				/* CHILD PROCESS */ 
				close(sockfd);	/* child only uses newsock */
#ifndef _DEBUG
				/* set our process group ID to 0, so init will handle cleanup */
				if (setpgid(0, 0) != 0) {
					DPRINT(("antpservd: setpgid error:%d\n", errno));
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
				antpserv_handleRequest(newsock, BaseDir, NULL); 
				close(newsock); 
				exit(0);
				break;

			default:
				/* PARENT PROCESS */
				close(newsock);  /* only the child needs this socket now */
				if (wait(NULL) != pid) {
					DPRINT(("antpservd: wait for child %d failed\n", pid));
					exit(1);
				}
				break;
			}
		} else {
			close(newsock);
			DPRINT(("antpservd: rejected connection from %s\n", inet_ntoa(client_addr.sin_addr)));
		}
	}
}
