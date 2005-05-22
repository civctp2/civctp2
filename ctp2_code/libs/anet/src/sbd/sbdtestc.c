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
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <assert.h>

#include "sbdclnt.h"
#include "eclock.h"

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

void main (int argc, char *argv[])
{
	char *host;
	unsigned short port;
	int npkts;
	size_t pktsize;
	char buf[sbd_MAXLEN];
	int i;

	if (argc <= 3) {
		printf("Usage: %s <host> <port> <npkts> [pktsize]\n", argv[0]);
		printf("    Tests sbd modules by sending known data to a listening\n");
		printf("    sbdtests at inet address <host>:<port> <npkts> times.\n");
		exit(1);
	}
	host = argv[1];
	port = atoi(argv[2]);
	npkts = atoi(argv[3]);
	if (argc > 4)
		pktsize = (size_t)atoi(argv[4]);
	else 
		pktsize = sbd_MAXLEN;
		
	for (i = 0; i < pktsize; i++)
		buf[i] = (i % ('Z' - 'A') + 'A');

	printf("Sending buffer of length %d to %s:%d %d times\n",
		pktsize, host, port, npkts);
	for (i = 0; i < npkts; i++) {
		clock_t timeout;
		sbdclnt_t *sbdclnt;

		timeout = eclock() + ECLOCKS_PER_SEC * 30;
		sbdclnt = sbdclnt_create(buf, pktsize, host, port);
		if (!sbdclnt) {
			printf("sbdclnt_create failed\n");
			exit(1);
		}

		while (1) {
			fd_set wfds;
			struct timeval tv;
			int nsocks;

			if ((long)(eclock() - timeout) > 0) {
				printf("\nsend timed out\n");
				exit(1);
			}
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			FD_ZERO(&wfds);
			FD_SET(sbdclnt->sock, &wfds);
			nsocks = select(sbdclnt->sock + 1, NULL, &wfds, NULL, &tv);
			if (nsocks < 0) {
				printf("\nselect error:%d\n", errno);
				exit(1);
			} else if (nsocks == 0) {
				continue;
			}

			if (FD_ISSET(sbdclnt->sock, &wfds)) {
				int len = sbdclnt_poll(sbdclnt);
				if (len < 0) {
					printf("\nsbdclnt_poll error\n");
					exit(1);
				}
				if (len == 0)
					continue;
				assert(len == pktsize);
				printf(".");
				break;
			}
		}

		sbdclnt_destroy(sbdclnt);
		if (!((i + 1) % 50) || (i == npkts-1))
			printf("\n");
	}
	printf("done\n");
	exit(0);
}
