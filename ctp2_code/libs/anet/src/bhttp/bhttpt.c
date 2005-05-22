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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>

#include "bhttp.h"

#if defined(_DEBUG) || defined(DPRNT)
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

int pleaseQuit;

static void signalQuit(int sig)
{
	printf("Test timed out\n");
	pleaseQuit = 1;
}

int url2buf(char *context, const char *url, char *buf, int buflen, bhttp_url2buf_result_t *urlResult)
{
	urlResult->httpResultCode = 200;
	strcpy(urlResult->mimeType, "text/plain");

	return sprintf(buf, "The URL is : %s", url);
}

void main (int argc, char *argv[])
{
	bhttp_t *bhttp;
	unsigned short port;
	int ncomplete;
	int flags;
	int i;
	int timeout;

	if (argc <= 2) {
		printf("Usage: %s <port> <timeout>\n", argv[0]);
		printf("    Tests bhttp module by listening for <timeout> seconds\n");
		printf("	on <port> \n");
		exit(1);
	}
	port = atoi(argv[1]);
	timeout = atoi(argv[2]);

	bhttp = bhttp_create(port, url2buf, NULL);
	if (!bhttp) {
		printf("failed to create bhttp with port:%d\n", port);
		exit(1);
	}

	ncomplete = 0;
	pleaseQuit = 0;
	if (SIG_ERR == signal(SIGALRM, signalQuit)) {
		printf("can't set alarm signal handler\n");
		exit(1);
	}
	alarm(timeout);  /* test fails if not done by alarm time */
	while (!pleaseQuit) {
		fd_set rfds;
		fd_set wfds;
		struct timeval tv;
		int nsocks;
		int sockmax;
		int sock;
		int len;

		tv.tv_sec = 10;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		sockmax = bhttp_getfds(bhttp, &rfds, &wfds);
		nsocks = select(sockmax + 1, &rfds, &wfds, NULL, &tv);		
		if (nsocks < 0) {
			printf("select error:%d\n", errno);
			exit(1);
		} else if (nsocks == 0) {
			printf("select timed out\n");
		}
		printf("select returns %d/%d socks", nsocks, (1 + bhttp->conns->n_used)); 
		for (sock = 0; sock <= sockmax; sock++) {
			if (FD_ISSET(sock, &rfds))
				printf(":%d", sock);
			if (FD_ISSET(sock, &wfds))
				printf(":w%d", sock);
		}
		printf("\n");

		len = bhttp_poll(bhttp, &rfds, &wfds, nsocks);
		if (-1 == len) {
			printf("bhttp_poll error!\n");
			pleaseQuit = 1;
			break;
		}
	}
	bhttp_destroy(bhttp);

	exit(0);
}
