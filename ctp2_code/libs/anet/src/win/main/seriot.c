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

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include "serio.h"

void fillRamp(char *buffer, int len)
{
	int	i;

	for (i=1; i<=len; i++) buffer[i-1]=(i&15)+'A';
	/* buffer[len-1]=13; */
}

void oneWayTestTx(serio_t *serio, int packetLen, int packetCnt)
{
	int			txrx = 0;
	int			txrxok = 0;
	double		duration = 0.0;
	char		buf[1000];
	clock_t		first;
	clock_t		last;
	clock_t		now;

	first=clock();
	fillRamp(buf, packetLen);
	last = first;
	while (txrxok < packetCnt) {
		clock_t before_send = clock();

		txrx++;
		if (serio_write(serio, buf, packetLen) == serio_RES_OK) {
			now = clock();
			txrxok++;
			/* print out how long since last send, and how long
			 * this commTxPkt() took
			 */
			printf("txrxok %d txrx %d interval %d duration %d\n",txrxok, txrx, now - last, now - before_send);
			last = now;
		}
	}

	duration = clock()-first;
	duration /= CLOCKS_PER_SEC;
	printf("\ntxrx %d, txrxok %d, packets/sec %f\n",
		txrx, txrxok,
		txrxok / duration);

}

void oneWayTestRx(serio_t *serio, int packetLen, int packetCnt)
{
	int			txrx = 0;
	int			txrxok = 0;
	double		duration = 0.0;
	char		buf[1000];
	clock_t		first;
	clock_t		last;
	clock_t		now;
	size_t		n_received;

	first=clock();
	last = first;
	while (txrxok < packetCnt) {
		size_t got;

		txrx++;
		got = 0;
		while ((int)got < packetLen) {
			clock_t before_read = clock();
			size_t get = packetLen - got;
			n_received = 6666666;
			if (serio_read(serio, buf+got, get, &n_received) == serio_RES_OK) {
				got += n_received;
				now = clock();
				printf("n_received %d duration %d\n",n_received, now - before_read);
			}
		}
		txrxok++;
		/* print out how long since last read, and how long
		 * it took to read the packet
		 */
		printf("txrxok %d txrx %d interval %d\n",txrxok, txrx, now - last);
		fwrite(buf, packetLen, 1, stdout);
		printf("\n");
		last = now;
	}

	duration = clock()-first;
	duration /= CLOCKS_PER_SEC;
	printf("\ntxrx %d, txrxok %d, packets/sec %f\n",
		txrx, txrxok,
		txrxok / duration);

}

void main(int argc, char **argv)
{
	int   i;
	int packetCnt;
	size_t packetLen;
	long baud;
	char portname[_MAX_PATH];
	char *command = "t";
	serio_res_t err;
	serio_t serio;

	packetLen = 1;
	packetCnt = 100;
	baud = 19200;
	strcpy(portname, "COM1");

	for (i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			command = argv[i];
			continue;
		}
		if (argv[i][1] == 0) {
			printf("Expected letter after -\n");
			exit(1);
		}
		if (argv[i][2] != '=' && argv[i][2] != ':') {
			printf("Expected = after -%c\n", argv[i][1]);
			exit(1);
		}

		switch (toupper(argv[i][1])) {

		case 'L':
			printf("Setting packetLen to %s\n", argv[i]+3);
			packetLen = atoi(argv[i]+3);
			break;

		case 'M':
			printf("Setting number of packet to %d\n", argv[i]+3);
			packetCnt = atoi(argv[i]+3);
			break;

		case 'Y':   /*  Set comm port */
			sscanf(argv[i]+3, "%s", portname);
			break;

		case 'Z':   /*  Set baud rate */
			sscanf(argv[i]+3, "%ld", &baud);
			break;

		default:
			printf("Usage: %s cmd [-Y=commport][-Z=baudrate][-L=packetlen][-M=packetCnt]\n"
				"Unknown arg %s\n", argv[0], argv[i]);
			exit(1);
		} /*  end switch */
	}

	err = serio_open(&serio, baud, portname);
	if (err != serio_RES_OK) {
		printf("seriot: error initializing\n");
		exit(1);
	}

	switch (toupper(command[0])) {
	case 'T':
		oneWayTestTx(&serio, packetLen, packetCnt);
		break;
	case 'R':
		oneWayTestRx(&serio, packetLen, packetCnt);
		break;
	default :
		printf("seriot: error: unknown command %s\n", command);
		exit(1);
	}

	err = serio_close(&serio);
	if (err != serio_RES_OK) {
		printf("seriot: error closing port\n");
		exit(1);
	}

	exit(0);
}
