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
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "eclock.h"

void main (int argc, char *argv[])
{
	int sockfd;
	char *host;
	int port = 2000;
	int n;
	struct hostent *he;
	struct protoent *pe;
	struct sockaddr_in sockaddr;
	int i;
	int len;
	char msg[1500];
	clock_t t_start;
	float duration;

	if (argc < 3) {
		printf("Usage: %s <host> <n> [size]\n", argv[0]);
		printf("    Spew n UDP packets to host on port %d.\n", port);
		printf("	If size is not given, uses minimum (4).\n");
		printf("    Use udpsink to receive them and print stats\n");
		exit(1);
	}
	host = argv[1];
	n = (short)atoi(argv[2]);
	if (n < 1)
		n = 1;
	if (argc > 3) {
		len = atoi(argv[3]);
		if (len < 4)
			len = 4;
		else if (len > 1500)
			len = 1500;
	} else
		len = 4;

	pe = getprotobyname("UDP");
	if (!pe) {
		printf("unknown protocol - UDP\n");
		exit(1);
	}
	he = gethostbyname(host);
	if (!he) {
		printf("unknown host - %s\n", host);
		exit(1);
	}
	memset((caddr_t)&sockaddr, 0, sizeof(struct sockaddr_in));
	sockaddr.sin_family = he->h_addrtype;
	memcpy(&sockaddr.sin_addr, he->h_addr, he->h_length);
	sockaddr.sin_port = htons(port);

	sockfd = socket(sockaddr.sin_family, SOCK_DGRAM, pe->p_proto);
	if (sockfd < 0) {
		printf("socket error:%d\n", errno);
		exit(1);
	}

	memset(msg, 0, 256);
	msg[2] = (char)(n & 0xff);
	msg[3] = (char)((n >> 8) & 0xff);
	printf("sending %d packets of length %d to %s:%d\n", n, len, host, port);
	t_start = eclock();
	for (i = 1;  i <= n;  i++) {
		msg[0] = (char)(i & 0xff);
		msg[1] = (char)((i >> 8) & 0xff);
		if (-1 == sendto(sockfd, msg, len, 0,
		                 &sockaddr, sizeof(struct sockaddr_in)))
			printf("sendto error:%d on packet %d/%d\n", errno, i, n);
	}
	duration = (float)((long)(eclock()-t_start))/ECLOCKS_PER_SEC;
	if (duration < (float)1.0/ECLOCKS_PER_SEC)
		duration = (float)1.0/ECLOCKS_PER_SEC;
	printf("finished in %.2fs, %.2f pkts/sec\n", duration, (float)n/duration);
}
