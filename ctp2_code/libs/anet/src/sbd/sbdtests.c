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

#include "sbdserv.h"

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

void main (int argc, char *argv[])
{
	sbdserv_t *sbdserv;
	unsigned short port;
	int npkts;
	int pktsize;
	char testbuf[sbd_MAXLEN];
	int ncomplete;
	int flags;
	int i;
	int timeout;

	if (argc <= 3) {
		printf("Usage: %s <port> <npkts> <timeout> [pktsize]\n", argv[0]);
		printf("    Tests sbd modules by listening for <timeout> seconds\n");
		printf("	on <port> for <npkts> packets sent by sbdtestc and\n");
		printf("    doing sanity checks.\n");
		exit(1);
	}
	port = atoi(argv[1]);
	npkts = atoi(argv[2]);
	timeout = atoi(argv[3]);
	if (argc > 4)
		pktsize = atoi(argv[4]);
	else
		pktsize = sbd_MAXLEN;

	/* expected data */
	for (i = 0; i < pktsize; i++)
		testbuf[i] = (i % ('Z' - 'A') + 'A');

	sbdserv = sbdserv_create(port);
	if (!sbdserv) {
		printf("failed to create sbdserv with port:%d\n", port);
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
		struct timeval tv;
		int nsocks;
		int sockmax;
		int sock;
		char buf[sbd_MAXLEN];
		int len;

		tv.tv_sec = 10;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		sockmax = sbdserv_getfds(sbdserv, &rfds);
		nsocks = select(sockmax + 1, &rfds, NULL, NULL, &tv);		
		if (nsocks < 0) {
			printf("select error:%d\n", errno);
			exit(1);
		} else if (nsocks == 0) {
			printf("select timed out\n");
		}
		printf("select returns %d/%d socks", nsocks, (1 + sbdserv->conns->n_used)); 
		for (sock = 0; sock <= sockmax; sock++)
			if (FD_ISSET(sock, &rfds))
				printf(":%d", sock);
		printf("\n");

		sbdserv_startRead(sbdserv, nsocks);
		while (1) {
			len = sbdserv_poll(sbdserv, &rfds, buf);
			if (-1 == len) {
				printf("sbdserv_poll error!\n");
				pleaseQuit = 1;
				break;
			} else if (0 == len) {
				break;
			}
			if (len != pktsize) {
				printf("received data of unexpected len:%d!\n", len);
				assert(len == pktsize);
				exit(1);
			} else if (memcmp(buf, testbuf, pktsize)) {
				printf("received garbled buffer!\n");
				assert(!memcmp(buf, testbuf, pktsize));
				exit(1);
			} 
			ncomplete++;
			if (ncomplete == npkts)
				pleaseQuit = 1;
		}
	}
	sbdserv_destroy(sbdserv);

	printf("%d/%d correct sends from sbdtestc.\n", ncomplete, npkts);
	if (ncomplete != npkts) {
		printf("Test failed\n");
		exit(1);
	}
	printf("Test passed\n");
	exit(0);
}
