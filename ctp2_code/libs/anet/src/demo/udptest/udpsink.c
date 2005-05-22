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
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "eclock.h"

int n = 0;
int nrcvd = 0;
clock_t t_first;
clock_t t_last;

void printStats(int sig)
{
	float duration;

	if (sig != SIGALRM) {
		printf("got signal %d != SIGALRM\n", sig);
		exit(1);
	}
	duration = (float)((long)(t_last-t_first))/ECLOCKS_PER_SEC;
	if (duration < (float)1.0/ECLOCKS_PER_SEC)
		duration = (float)1.0/ECLOCKS_PER_SEC;
	printf("received %d/%d packets in %.2fs, %.2f pkts/sec %.2f%% loss\n",
		nrcvd, n, duration, (float)nrcvd/duration, (float)((n-nrcvd)*100.0)/n);
	exit(0);
}

void main(int argc, char *argv[])
{
	int sockfd;
	int port = 2000;
	struct protoent *pe;
	struct sockaddr_in my_addr;

	if (argc > 1) {
		printf("Usage: %s\n", argv[0]);
		printf("    Listens for UDP packets from udpspew on port %d\n", port);
		printf("    and prints statistics.\n");
		exit(1);
	}

	pe = getprotobyname("udp");
	if (!pe) {
		printf("unknown protocol - udp\n");
		exit(1);
	}
	memset((caddr_t)&my_addr, 0, sizeof(struct sockaddr_in));
	my_addr.sin_family = AF_INET; 
	my_addr.sin_port = htons(port); 
	my_addr.sin_addr.s_addr = htons(INADDR_ANY); 

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, pe->p_proto)) < 0) {
		printf("socket error:%d\n", errno);
		exit(1);
	}
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		printf("bind error:%d\n", errno);
		exit(1);
	}

	if (SIG_ERR == signal(SIGALRM, printStats)) {
		printf("can't set SIGALRM handler\n");
		exit(1);
	}

	printf("listening for udpspew packets on UDP port %d\n", port);
	while(1) {
		unsigned char msg[1500];
		int len;
		struct sockaddr_in from_addr;
		int fromlen = sizeof(struct sockaddr_in);
		int i;

		if ((len = recvfrom(sockfd, msg, 256, 0, &from_addr, &fromlen)) == -1) {
			printf("recvfrom error:%d\n", errno);
			exit(1);
		} else if (len < 4) {
			printf("received odd sized packet len:%d\n", len);
			exit(1);
		}
		if (!n) {
			t_first = eclock();
			alarm(3);  /* set 3s alarm after first packet */
			n = ((msg[3] << 8) + msg[2]);
			if (n < 1) {
				printf("received odd n:%d\n", n);
				exit(1);
			}
		}
		i = ((msg[1] << 8) + msg[0]);
		if ((i < 1) || (i > n)) {
			printf("received odd i:%d\n", i);
			exit(1);
		}
		t_last = eclock();
		nrcvd++;
	}
}
