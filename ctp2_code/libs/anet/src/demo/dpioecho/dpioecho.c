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

/*----------------------------------------------------------------------------
  A simple echo client/server demo using the dpio level interface.
----------------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>
#include "dp2.h"
#include "raw.h"
#include "eclock.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/* globals for use by main, echo_server/echo_client */
dpio_t *dpio = NULL;
clock_t	now;

/* globals for use by main, ProcessCommandLine */
char commDLLName[256];
int server;
char servername[256];
commInitReq_t commInitReq; /*  Parameters needed by some comm drivers. */
char phonenum[256];
char modeministr[256];

/* Globals related to simulated network. */
#ifdef SIMNET
/* There are two addresses: 0 and 1.  All packets are from the other address. */
struct {
	int len;
	char buf[256];
} simnetbuf[2];			/* receive buffer */
int sim_my_rx = 0;		/* offset I receive into */

/* Get a packet, if there is one */
dp_result_t dpio_get(
	dpio_t   *dpio,
	playerHdl_t *psrc,
	void *buffer,
	size_t *psize,
	int *flags)
{
	(void) dpio; (void) flags;
	if (simnetbuf[sim_my_rx].len) {
		*psrc = 1 - sim_my_rx;
		memcpy(buffer, simnetbuf[sim_my_rx].buf, simnetbuf[sim_my_rx].len);
		*psize = simnetbuf[sim_my_rx].len;
		simnetbuf[sim_my_rx].len = 0;
		printf("sim dpio_get: %d got packet of size %d from %d\n", sim_my_rx, *psize, *psrc);
		return dp_RES_OK;
	}
	return dp_RES_EMPTY;
}

/* Send a packet */
dp_result_t dpio_put_reliable2(
	dpio_t  *dpio,
	const playerHdl_t *dests,/* Vector of destinations */
	int nDests,             /* Size of destination vector */
	void *buffer,
	size_t size,
	playerHdl_t *errDest,  /* If error occurs, dest with error indicated here */
	int flags)
{
	printf("sim dpio_put: %d sending packet of size %d to %d\n", sim_my_rx, size, (int)(*dests));
	if ((int)(*dests) != (1 - sim_my_rx)) {
		printf("dpio_put_reliable: bad dest\n");
		exit(1);
	}
	if (simnetbuf[1 - sim_my_rx].len) {
		printf("dpio_put_reliable: output buffer full\n");
		exit(1);
	}
	memcpy(simnetbuf[1 - sim_my_rx].buf, buffer, size);
	simnetbuf[1 - sim_my_rx].len = size;
	return dp_RES_OK;
}

#define dpio_openHdl2(dpio, adr, adr2) (0)
#define dpio_create(dpio, transport, commInitReq, now, c) dp_RES_OK
#define dpio_destroy(dpio, flags)

dp_result_t dpio_printAdr(dpio_t *dpio, const char *adrbuf, size_t adrlen, char *buf, size_t buflen)
{
	sprintf(buf, "%d", *adrbuf);
	return dp_RES_OK;
}
int dpio_scanAdr(dpio_t *dpio, char *hostname, char *adrbuf, size_t buflen)
{
	int adr;
	sscanf(hostname, "%d", &adr);
	*adrbuf = adr;
	return 1;
}
dp_result_t dpio_hdl2adr(dpio_t *dpio, playerHdl_t h, void *adr, int *len)
{
	if (h != PLAYER_ME) {
		printf("dpio_hdl2adr: bad\n");
		exit(1);
	}
	((char *)adr)[0] = sim_my_rx;
	*len = 1;
	return dp_RES_OK;
}

#define dpio_update(dpio) dp_RES_OK

#endif

/*----------------------------------------------------------------------------
  Processes the command line arguments.  Some arguments are handled
  directly by this routine.  Others simply result in an update to
  the global switches data structure.
----------------------------------------------------------------------------*/
void ProcessCommandLine(int argc, char **argv)
{
	char *chptr;
	int   i;

	servername[0] = '\0';
	commDLLName[0] = '\0';

	for (i = 1; i < argc; ++i) {
		if (argv[i][0] == '/' || argv[i][0] == '-') {
			/* deal with args that start with - or / */
			if (argv[i][1] == 0) {
				printf("Expected letter after -\n");
				exit(1);
			}
			if (argv[i][2] != '=' && argv[i][2] != ':') {
				printf("Expected = after -%c\n", argv[i][1]);
				exit(1);
			}
			switch(toupper(argv[i][1])) {
			case 'C':
				server = FALSE;
				strncpy(servername, (argv[i]+3), 256);
				servername[255] = '\0';
				printf("Read servername: %s\n", servername);
				break;
			case 'D':
				/*  Turn on some sort of debugging mode */
				break;
			case 'I':   /*  Set modem init string */
				strncpy(modeministr, argv[i]+3, sizeof(modeministr));
				modeministr[sizeof(modeministr)-1] = '\0';
				break;
			case 'K':	/*  Comm parameters; Kport, Kirq, Kpulse */
				if (stricmp(argv[i], "-Kpulse") == 0) {
					/*  No need to set default case  */
					/*  TOUCH_TONE == 0 */
					/*  PULSE == 1 */
					commInitReq.dialing_method = 1;
					break;
				}
				if (stricmp(argv[i], "-Ktest") == 0) {
					/* Do loopback test. Other side must have -Ktest, too. */
					commInitReq.flags |= comm_INIT_FLAGS_TEST;
					break;
				}
				if ((chptr = strchr(argv[i], '=')) == NULL) {
					printf("-K flag requires -Kport=hexnum, -Kirq=intnum, -Kpulse, or -Ktest");
					exit(1);
				}
				*chptr = 0;
				if (stricmp(argv[i], "-Kport") == 0) {
					sscanf(chptr+1, "%lx", &commInitReq.baseadr);
				} else if (stricmp(argv[i], "-Kirq") == 0) {
					sscanf(chptr+1, "%ld", &commInitReq.hwirq);
				} else {
					*chptr = '=';
					printf("unknown parameter %s", argv[i]);
					exit(1);
				}
				break;
			case 'N':   /*  Communications DLL name */
				if ((chptr = strchr(argv[i], '=')) == NULL)
					/* No comm DLL name specified. */
					commDLLName[0] = '\0';
				else {
					chptr++;
					strncpy(commDLLName, chptr, sizeof(commDLLName));
					commDLLName[sizeof(commDLLName)-1] = '\0';
				}
				break;
			case 'W':   /*  Set phone number */
				strncpy(phonenum, argv[i]+3, sizeof(phonenum));
				phonenum[sizeof(phonenum)-1] = '\0';
				break;
			case 'Y':   /*  Set comm port */
				sscanf(argv[i]+3, "%ld", &commInitReq.portnum);
				break;
			case 'Z':   /*  Set baud rate */
				sscanf(argv[i]+3, "%ld", &commInitReq.baud);
				break;
			default:
				printf("\
Usage: %s -N=comm.dll [-C=serveraddr]\n\
    [-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n\
    [-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n\
Unknown arg %s\n", argv[0], argv[i]);
				printf("\
A simple client server test using dpio.  The server simply echoes anything\n\
the client types, in upper case.  If -C=serveraddr is specified, act as\n\
a client to a server at serveraddr.  If not, act as a server and print\n\
out the network address for clients to connect to.\n");
				exit(1);
			} /*  end switch */
		}
	}
}

/*----------------------------------------------------------------------------
 A simple client packet send and receive poll function.
 Sends out what the user types to the server, prints out whatever returns.
 On first entry,
    init = TRUE
	all other values ignored
 On following entries,
	init = FALSE
	charFromUser = 0 or an ASCII keystroke
	serverhdl = where to send chat packets to
----------------------------------------------------------------------------*/
void echo_client(playerHdl_t serverhdl, int init, int charFromUser)
{
	dp_result_t err;
	playerHdl_t phdl;
	int flags;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;

	static char kbbuf[1024];
	static char *c;

	if (init) {
		memset(kbbuf, 0, sizeof(kbbuf));
		c = kbbuf;
		return;
	}

	now = eclock();
	err = dpio_update(dpio);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpio_get(dpio, &phdl, pktbuf, &pktlen, &flags);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (err == dp_RES_OK) {
		printf("Received: %s\n", pktbuf);
	}

	if (charFromUser) {
		if ((charFromUser == 13) /* ^M or Return */
		||  (c-kbbuf > sizeof(kbbuf)-2)) {
			playerHdl_t errdest;

			printf("\n");
			*c++ = '\0';
			err = dpio_put_reliable(dpio, &serverhdl, 1, kbbuf, c-kbbuf, &errdest);
			assert(err != dp_RES_BAD);
			assert(err != dp_RES_BUG);
			c = kbbuf;
		} else if (isprint(charFromUser)) {
			printf("%c", charFromUser);
			*c++ = charFromUser;
		}
	}
}

/*----------------------------------------------------------------------------
  A simple server packet send and receive polling function.
  Processes and sends any packets received back to the sender.
----------------------------------------------------------------------------*/
void echo_server()
{
	dp_result_t err;
	playerHdl_t phdl;
	int flags;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	size_t pktlen;

	now = eclock();
	err = dpio_update(dpio);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);

	pktlen = dpio_MAXLEN_UNRELIABLE;
	err = dpio_get(dpio, &phdl, pktbuf, &pktlen, &flags);
	assert(err != dp_RES_BAD);
	assert(err != dp_RES_BUG);
	if (err == dp_RES_OK) {
		playerHdl_t errdest;
		char *p;

		/* process incoming packets here (just uppercase them) */
		for (p = pktbuf; (size_t)(p - pktbuf) < pktlen; p++)
			*p = toupper(*p);
		/* echo the pktbuf back to the sender */
		err = dpio_put_reliable(dpio, &phdl, 1, pktbuf, pktlen, &errdest);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
	}

}

int main(int argc, char *argv[])
{
	dp_result_t err;
	dp_transport_t transport;
	playerHdl_t serverhdl;

	if (argc < 2) {
		printf("\
Usage: %s -N=comm.dll [-C=serveraddr]\n\
    [-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n\
    [-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n", argv[0]);
		printf("\
A simple client server test using dpio.  The server simply echoes anything\n\
the client types, in upper case.  If -C=serveraddr is specified, act as\n\
a client to a server at serveraddr.  If not, act as a server and print\n\
out the network address for clients to connect to.\n");
		exit(0);
	}

	modeministr[0] = 0;
	phonenum[0] = 0;
	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.modeministr = modeministr;
	commInitReq.phonenum = phonenum;
	commInitReq.baud = 19200;
	commInitReq.hwirq = 0;
	commInitReq.portnum = 1;
	commInitReq.sessionId = rand() | (rand() << 16);
	commInitReq.reqLen = sizeof(commInitReq_t);
	server = TRUE;  /* I am server unless -C is specified */

	ProcessCommandLine(argc, argv);

	if (!commDLLName[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}

	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, commDLLName);
	now = eclock();
    err = dpio_create(&dpio, &transport, &commInitReq, &now, NULL);
	if (err == dp_RES_OK) {
		printf("dpio_create loaded dll.\n");
	} else {
		printf("dpio_create returns fatal error: %d\n", err);
		exit(1);
	}

#ifndef SIMNET
	if (!server)
#endif
	{
		char serveraddr[dp_MAX_ADR_LEN];
		int addrlen;

		addrlen = dpio_scanAdr(dpio, servername, serveraddr, dp_MAX_ADR_LEN);
		if (addrlen == 0) {
			printf("dpio_scanAdr(%s) failed\n", argv[1]);
			exit(1);
		}
		serverhdl = dpio_openHdl2(dpio, serveraddr, NULL);
		if (serverhdl == PLAYER_NONE) {
			printf("dpio_openHdl2 failed\n");
			exit(1);
		}
		echo_client(serverhdl, 1, 0);  /* initialize the keyboard buffer */
	}
#ifndef SIMNET
	else
#endif
	{
		char serveraddr[dp_MAX_ADR_LEN];
		int addrlen = dp_MAX_ADR_LEN;
		char printable[50];

		/* print my address if I am server, so clients can connect */
		err = dpio_hdl2adr(dpio, PLAYER_ME, serveraddr, &addrlen);
		assert(err != dp_RES_BAD);
		assert(err != dp_RES_BUG);
		if (dpio_printAdr(dpio, serveraddr, addrlen, printable, 50)) {
			printf("I am server, unable to print my address\n");
			exit(1);
		}
		printf("I am server, my address is %s\n", printable);
	}

	raw_init();
	while (1) {
		int charFromUser = 0;
		if (raw_kbhit()) {
			charFromUser = raw_getc();
			if (charFromUser == 27 /* Esc */) {
				printf("Escape hit.  Terminating.\n");
				break;
			}
		}
#ifdef SIMNET
		sim_my_rx = 0;
		echo_server();
		sim_my_rx = 1;
		echo_client(serverhdl, 0, charFromUser);
#else
		if (server) {
			echo_server();
		} else {
			echo_client(serverhdl, 0, charFromUser);
		}
#endif
	}
	raw_end();

	printf("dpio_destroy unloading comm drivers...");
	dpio_destroy(dpio, 0);
	printf(" done.\n");

    return(0);
}
