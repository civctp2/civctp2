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
 $Log: iotest.c $
 Revision 1.7  1997/03/07 01:57:08  geychaner
 Extract the differences between UNIX and WIN32 to the raw I/O library
 (in ..\demo\utils)
 Revision 1.6  1997/03/06 06:05:06  geychaner
 Added ifdefs for UNIX (Linux) platform to keyboard I/O
 Revision 1.4  1997/02/15 02:20:50  dkegel
 Adapted to change in dpio_openHdl.
 Revision 1.3  1997/02/10 05:04:02  dkegel
 1) Reads list of players from file
 2) Handles player timeout
 3) Allows drop percentage of packets
 4) Nicer user I/O
 Revision 1.2  1997/02/10 02:41:49  dkegel
 Adapted to changes in api.
 Revision 1.1  1997/02/08 01:02:30  dkegel
 Initial revision

 Test program for the underlying packet delivery services used by ActiveNet.
 Tests both reliable and unreliable host-to-host packet delivery.
 (Does not test *player-to-player* packet delivery.)
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "dpio.h"
#include "raw.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define DEBUG_MODULE TRUE   /*  enable DPRINT in this file */
#define MAX_PLAYERS	 8		/*  Maximum number of players in player file */

dpio_t *dpio = NULL;

static char		adrString[256];
static int		packetLen;
static int		dropPercent;
char        	commDLLName[256];
commInitReq_t	commInitReq;		/*  Parameters needed by some comm drivers. */
char			modeministr[256];
char			phonenum[256];

playerHdl_t		AllPlayers[MAX_PLAYERS];	/* Player array for multiplayer */
int 			N_Players=0;				/* Number of multiplayer players */

clock_t			Now;

int useReliable = FALSE;

/*  Get a list of player addresses from an external file */
/*  Create the array of playerHdl AllPlayers from this list */
/*  Set the number of players N_Players */
/*  Set the master player HMaster to the first player in the array */
void readPlayerAddresses (char *playerFname)
{

    commScanAddrReq_t		scanReq;
    commScanAddrResp_t		scanResp;
	char					printable[256];
	char					adrBuf[16];
	playerHdl_t				dest = PLAYER_BROADCAST;
	FILE					*playerFile = NULL;
	char 					buf[256];
	playerHdl_t		HMaster = PLAYER_ME;
	
	playerFile = fopen (playerFname, "r");
	if (!playerFile) {
		printf ("Unable to open player file %s\n", playerFname);
		return;
	}
	while (fgets (buf, 255, playerFile) && (N_Players < 8)) {
		sscanf (buf, "%s", printable);
		
		/*  Convert partner's address to binary for use with commSayHi */
		scanReq.printable = printable;
		scanReq.address = adrBuf;
		scanReq.size = sizeof(adrBuf);
		if (!commScanAddr(&scanReq, &scanResp)) {
			printf("Unable to scan player address, error: %d\n", scanResp.status);
			return;
		}
	
		/*  Open a comm handle to the partner's address */
		dest = dpio_openHdl(dpio, adrBuf, NULL, NULL);
		if (dest == PLAYER_NONE) {
			printf("Unable to connect to player %s\n", printable);
			return;
		}
		printf("Got player handle: %x for %s\n", dest, printable);
		AllPlayers[N_Players] = dest;
		N_Players++;
	}
	fclose (playerFile);
	HMaster = AllPlayers[0];
	if (HMaster == PLAYER_ME) {
		printf ("Found %d players, Master is ME\n", N_Players);
	} else {
		printf ("Found %d players, Master is %d\n", N_Players, AllPlayers[0]);
	}
}


/*  Start up comm system, chat with otherPlayer. */
/*  If otherPlayer is NULL or empty, prompt user for other player's address. */
/*  If broadcast is TRUE, ignore otherPlayer, as we don't need to know other */
/*  address to broadcast. */
void chatTest(char *otherPlayer, int broadcast)
{
    commDriverInfoResp_t 	infoResp;
    commPlayerInfoReq_t  	playerReq;
    commPlayerInfoResp_t 	playerResp;
    commPrintAddrReq_t		printReq;
    commPrintAddrResp_t		printResp;
    commScanAddrReq_t		scanReq;
    commScanAddrResp_t		scanResp;
	char					printable[50];
	char					adrBuf[16];
	playerHdl_t				src;
	playerHdl_t				dest = PLAYER_BROADCAST;
	playerHdl_t				timedOutPlayer;
	dp_result_t				err;

	printf("chatTest(otherPlayer: %s)\n", otherPlayer);

	if (broadcast && useReliable) {
		printf("Error: can't use broadcast and reliable together.\n");
		return;
	}

    /*  find out info about this driver */
    if (commDriverInfo(NULL, &infoResp))
		if (!infoResp.status)
			printf("%s, Version %x\n", infoResp.info->name, infoResp.info->version);

    /*  Find out about this player */
    playerReq.player = PLAYER_ME;
    if (!commPlayerInfo(&playerReq, &playerResp)) {
		printf("Unable to get player %d info, error: %d\n", PLAYER_ME, playerResp.status);
		return;
    }
    /*  print out player info */
	if (playerResp.name != NULL)
		printf("Player name: %s\n", playerResp.name);
	printf("Player address: %p -> %x\n", playerResp.address, *(long *)playerResp.address);

	/*  Print out our address (normally used only during debugging, */
	/*   but in this chat test, you need to see it so you can yell it */
	/*   to your chat partner before continuing). */
	printReq.length = playerResp.addrLen;
	printReq.address = playerResp.address;
	printReq.printable = printable;
	printReq.size = sizeof(printable);
    if (!commPrintAddr(&printReq, &printResp)) {
		printf("Unable to print player address, error: %d\n", printResp.status);
		return;
    }
	printf("My address is %s\n", printable);

	if (!broadcast) {
		if (!otherPlayer || !otherPlayer[0]) {
			/*  Prompt user for address of chat partner */
			printf("Enter chat partner's address or filename:");
			gets(printable);
		} else {
			strcpy(printable, otherPlayer);
		}
		/* First, try to read from a file */
		readPlayerAddresses (printable);
		/* If unsuccessful, try the input as an address */
		if (!N_Players) {
			/*  Convert partner's address to binary for use with commSayHi */
			scanReq.printable = printable;
			scanReq.address = adrBuf;
			scanReq.size = sizeof(adrBuf);
			if (!commScanAddr(&scanReq, &scanResp)) {
				printf("Unable to scan player address, error: %d\n", scanResp.status);
				return;
			}
	
			/*  Open a comm handle to the partner's address */
			dest = dpio_openHdl(dpio, adrBuf, NULL, NULL);
			if (dest == PLAYER_NONE) {
				printf("Unable to connect to player\n");
				return;
			}
			printf("Got player handle: %x\n", dest);
			AllPlayers[0] = dest;
			N_Players = 1;
		}
	}
	
	raw_init();
    for (;;) {
        char ch;
		char buf[512];
		size_t len;

		/*  Check for any received packets */
		/*  Note: API breaks if receive buffer sorter than a few bytes */
		len = sizeof(buf);
		err = dpio_get(dpio, &src, buf, &len, NULL);
		if (err == dp_RES_OK) {
			/*  Check the packet length */
			if (len == 3) {
				printf("Packet from: %x ", src);
				/*  make sure this is not a broadcast message from myself. */
				if (src != PLAYER_ME) {
					ch = buf[2];
					printf ("%c", ch);
	 				if (buf[0] != 'Q' || buf[1] != 'Q') {
						printf (" - Bad packet header %c%c",buf[0],buf[1]);
					}
					/*  Do a linefeed when we need to */
					if (ch == 13) putch(10);
					/* sleep(3); */
				}
				printf ("\n");
			}
		} else if (err != dp_RES_EMPTY && err != dp_RES_AGAIN)
				printf("Error %d receiving packet\n", err);
		
		/*  see about sending data */
		if (raw_kbhit()) {
		    ch = raw_getc();
			putch(ch);
			/*  Do a linefeed when we need to */
			if (ch == 13) putch(10);
	
			/*  see if user wants to esc */
			if (ch == 0x1b) break;
	
			buf[0] = buf[1] = 'Q';
			buf[2] = ch;
			buf[3] = 0;
			
   			if (useReliable)
				err = dpio_put_reliable(dpio, AllPlayers, N_Players, buf, strlen(buf), NULL);
			else
				err = dpio_put_unreliable(dpio, AllPlayers, N_Players, buf, strlen(buf), NULL);
			if (err != dp_RES_OK)
				printf("Error %d sending packet\n", err);
			buf[0] = 0;
		}

		/* If reliable, poll for retransmissions. */
		Now = clock();
		dpio_update(dpio);
		timedOutPlayer = dpio_findTimedOutHost(dpio);
		if (timedOutPlayer != PLAYER_NONE) {
			printf("Player %d timed out...deleting\n", timedOutPlayer);
			err = dpio_closeHdl (dpio, timedOutPlayer);
			if (err != dp_RES_OK) {
				printf("Error %d deleting player\n", err);
			}
		}
       }
       raw_end();
}

void fillRamp(char *buffer, int len)
{
	int	i;

	for (i=1; i<=len; i++) buffer[i-1]=(i&15)+'A';
	/* buffer[len-1]=13; */
}

#if 0

int badRamp(char *buffer, int len)
{
	int 	status = 0;
	int		i;

	for (i=1; i<=len; i++)
		if (buffer[i-1]!=((i&15)+'A')) {
			printf("%02x!=%02x\n", buffer[i-1], i);
			status = 1;
		}

	return status;
}

#define INTERVAL	(CLOCKS_PER_SEC/5)
#define NPACKETS	50

void packetTest(int packetLen)
{
	int			sent = 0;
	int			sendfailed = 0;
	int			got = 0;
	int			gotok = 0;
	clock_t		first = 0;
	clock_t		next = 0;
	clock_t		last = 0;
	clock_t		freetime = 0;
	float		duration = 0.0;
	char		buf[1000];
	char		ch;
	/*  rough guess at size of commbuf in packets */
	int			prestuff=1024/(packetLen+20);	


	commTxPktReq_t	txReq;
	commTxPktResp_t	txResp;
	commRxPktReq_t	rxReq;
	commRxPktResp_t	rxResp;

	sent=got=gotok=0;
	last=next=clock()+INTERVAL;
	while ((sent < NPACKETS) || (got < NPACKETS)) {
		rxReq.buffer = buf;
		rxReq.size = packetLen;

		if (commRxPkt(&rxReq, &rxResp) == TRUE) {
			if (!got) first=clock();
			got++;
			if (!badRamp(buf,packetLen)) {
				gotok++;
				putch('.');
			}
			last=clock();
		}

		/*  got+n limits the number of rx failures that can occur before */
		/*  this transmit section begins to fail */
		if ((sent < (got+prestuff)) && (got > 0) && (NPACKETS > sent) && (clock() > next)) {
			fillRamp(buf, packetLen);
			txReq.dest = 0;
			txReq.buffer = buf;
			txReq.length = packetLen;
			next += INTERVAL;
			freetime += clock();
			if (commTxPkt(&txReq, &txResp)) {
				putch('*');
				sent++;
			}
			else
				sendfailed++;
			freetime -= clock();
		}

		if (kbhit())
		{
			ch = getch();
			/*  see if user wants to esc */
			if (ch == 0x1b) break;

			/*  send a packet so the other side assigns a playerHdl_t */
			/*  and starts sending packets */
			fillRamp(buf, packetLen);
			txReq.dest = PLAYER_BROADCAST;
			txReq.buffer = buf;
			txReq.length = packetLen;
			if (!commTxPkt(&txReq, &txResp)) {
				printf("packetTest: broadcast failed.  status %d.\n", txResp.status);
			}
			else {
				sent++;
				next += INTERVAL;
			}
		}

		if ((last+INTERVAL*20) < clock()) break;
	}

	printf("\n");
	duration = clock()-first;
	freetime += duration;
	printf("duration %f, percent free %f\n", duration, (100.0*freetime)/duration);
	duration /= CLOCKS_PER_SEC;
	printf("duration %f\n", duration);
	printf("sent %d, failed to send %d, got %d, gotok %d, packets/sec %f\n",
		sent, sendfailed,
		got, gotok,
		gotok / duration);
}

/*  be the reciever: direction = 0 */
/*  be the sender:   direction = 1 */
void onewayTest(int packetLen, int direction)
{
	int			started = 0;
	int			txrx = 0;
	int			txrxok = 0;
	float		duration = 0.0;
	char		buf[1000];
	char		ch;
	clock_t		first;
	clock_t		last;
	commTxPktReq_t	txReq;
	commTxPktResp_t	txResp;
	commRxPktReq_t	rxReq;
	commRxPktResp_t	rxResp;

	while (txrx < NPACKETS) {
		rxReq.buffer = buf;
		rxReq.size = packetLen;

		switch (direction) {
		case 0:
			if (commRxPkt(&rxReq, &rxResp)) {
				if (!txrx) first=clock();
				txrx++;
				if (!badRamp(buf,packetLen)) txrxok++;
				putch('.');
				last = clock();
			}
			break;

		case 1:
			if (started) {
				if (!txrx) first=clock();
				fillRamp(buf, packetLen);
				txReq.dest = 0;
				txReq.buffer = buf;
				txReq.length = packetLen;
				txrx++;
				if (commTxPkt(&txReq, &txResp)) {
					putch('*');
					txrxok++;
					last = clock();
				}
			}
			break;

		default:
			/*  the caller of this function should be smarter that this! */
			break;
		}

		if (kbhit())
		{
			ch = getch();
			/*  see if user wants to esc */
			if (ch == 0x1b) break;

			/*  send a packet so the other side assigns a playerHdl_t */
			/*  and starts sending packets */
			started = 1;
		}

		if ((last+INTERVAL*20) < clock()) break;
	}

	duration = clock()-first;
	duration /= CLOCKS_PER_SEC;
	printf("\ntxrx %d, txrxok %d, packets/sec %f\n",
		txrx, txrxok,
		txrxok / duration);

}
#endif

void onewayTestTx(int packetLen)
{
	int			txrx = 0;
	int			txrxok = 0;
	float		duration = 0.0;
	char		buf[1000];
	clock_t		first;
	clock_t		last;
	clock_t		now;
	commTxPktReq_t	txReq;
	commTxPktResp_t	txResp;

	first=clock();
	fillRamp(buf, packetLen);
	txReq.dest = 0;
	txReq.buffer = buf;
	txReq.length = packetLen;
	last = first;
	while (txrxok < 100) {
		clock_t before_send = clock();

		txrx++;
		if (commTxPkt(&txReq, &txResp)) {
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
#if 1

void quickTest(int packetLen)
{
	char		buf[1000];
	commTxPktReq_t		txReq;
	commTxPktResp_t		txResp;
	/* commRxPktReq_t		rxReq; */
	/* commRxPktResp_t		rxResp; */


	/*  send a packet so the other side assigns a playerHdl_t */
	/*  and starts sending packets */
	txReq.dest = PLAYER_BROADCAST;
	txReq.buffer = buf;
	txReq.length = packetLen;
	memset(&txResp, 0, sizeof(txResp));
	printf("About to transmit a %d byte packet\n",packetLen);
	if (!commTxPkt(&txReq, &txResp)) {
		printf("packetTest: broadcast failed.  status %d.\n", txResp.status);
	}

	printf("quicktest done.\n");
}

#endif

/* ======================================================================== *
   Function Name: ProcessCommandLine
   Description  : Processes the command line arguments.  Some arguments are
   handled directly by this routine.  Others simply result in an update to
   the global switches data structure
 * ======================================================================== */
void ProcessCommandLine(int argc, char **argv) {
  char *chptr;
  int   i;

  commDLLName[0] = 0;
  adrString[0] = 0;
  packetLen=0;
  dropPercent=0;

  for (i = 1; i < argc; ++i) {
	if (argv[i][0] == '/' || argv[i][0] == '-') /*  deal with args that start with - or / */
	{
	  if (argv[i][1] == 0) {
		printf("Expected letter after -\n");
		exit(1);
	  }
	  if (argv[i][2] != '=' && argv[i][2] != ':') {
		printf("Expected = after -%c\n", argv[i][1]);
		exit(1);
	  }
	  switch(toupper(argv[i][1])) {
		case '%':
			printf("Setting drop percentage to %s\n", argv[i]+3);
			dropPercent = atoi(argv[i]+3);
			break;
		case 'L':
			printf("Setting packetLen to %s\n", argv[i]+3);
			packetLen = atoi(argv[i]+3);
			break;
		case 'D':
			/*  Turn on some sort of debugging mode */
			break;

		case 'P':   /*  Communications Parameters */
			if ((chptr = strchr(argv[i], '=')) == NULL) /*  No comm params specified. */
                          adrString[0] = '\0';
			else {
			  chptr++;
                          strncpy(adrString, chptr, sizeof(adrString));
                          adrString[sizeof(adrString)-1] = '\0';
			}
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
				/*  Do loopback test.  Other side must have -Ktest, too. */
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
				/* printf("setting comm port base adr to 0x%x, effect depends on driver.\n", commInitReq.baseadr); */
				/* sleep(1); */
			} else if (stricmp(argv[i], "-Kirq") == 0) {
				sscanf(chptr+1, "%ld", &commInitReq.hwirq);
				/* printf("setting comm port irq to %d, effect depends on driver.\n", commInitReq.hwirq); */
				/* sleep(1); */
			} else {
				*chptr = '=';
				printf("unknown parameter %s", argv[i]);
				exit(1);
			}
			break;

		case 'N':   /*  Communications DLL name */
			if ((chptr = strchr(argv[i], '=')) == NULL) /*  No comm DLL name specified. */
				commDLLName[0] = '\0';
			else {
				chptr++;
				strncpy(commDLLName, chptr, sizeof(commDLLName));
				commDLLName[sizeof(commDLLName)-1] = '\0';
			}
			break;

		case 'I':   /*  Set modem init string */
			strncpy(modeministr, argv[i]+3, sizeof(modeministr));
			modeministr[sizeof(modeministr)-1] = '\0';
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

		case 'R':
			useReliable = TRUE;
			printf("setting useReliable TRUE\n");
			break;

		default:
			printf("Usage: %s cmd [-N=comm.dll][-P=adrString][-%%=dropPercent]\n"
				"[-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n"
				"[-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n"
				"Unknown arg %s\n", argv[0], argv[i]);
		    exit(1);
	  } /*  end switch */
	}
  }
}

/*  end of ProcessCommandLine() */

int main( int argc, char *argv[] )
{
	char			cmd;
	dp_result_t err;
	dp_transport_t transport;

	if (argc == 1) {
		printf("Usage: %s cmd [-N=comm.dll][-P=adrString][-%%=dropPercent]\n"
			"[-W=phonenum][-Y=commportnum(0 = com1)][-Z=baudrate][-I=modeministr]\n"
			"[-Kirq=irqnum][-Kport=hexportnum][-Kpulse][-Ktest]\n", argv[0]);
		printf("Examples: \n"
"     : test Q -L=packetLen (can we even send one packet)\n"
"   or: test C              (broadcast chat)\n"
"   or: test I [-P=adr]     (chat)\n"
"   or: test P -L=packetLen (packet loss test)\n"
"   or: test T -L=packetLen (one way test TX)\n"
"   or: test G -L=packetLen (one way test RX)\n");
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

	ProcessCommandLine(argc, argv);
    dp_enableDebugPrint(1);
	DPRINT(("Start of rawtest.c\n"));

	if (!commDLLName[0]) {
		printf("No DLL was specified.\n");
		exit(1);
	}

	memset(&transport, 0, sizeof(transport));
	strcpy(transport.fname, commDLLName);
    err = dpio_create(&dpio, &transport, &commInitReq, &Now, NULL);
	if (err == dp_RES_OK) {
		printf("DLL Loaded.\n");
	} else {
		printf("Unable to init io system, error: %d\n", err);
		exit(1);
	}
	dpio_forceRxDropPercent (dpio, dropPercent);

	cmd = argv[1][0];
	if (isupper(cmd)) cmd = tolower(cmd);

	switch (cmd) {
	case 'q':
		quickTest(packetLen);
		break;

	case 't':
		onewayTestTx(packetLen);
		break;

#if 0
	case 'p':
		packetTest(packetLen);
		break;

	case 'g':
		onewayTest(packetLen,0);
		break;
#endif

	case 'c':
		/*  Chat via (transport DLL argv[2]) with broadcast */
		chatTest(NULL, TRUE);
		break;

	case 'i':
		/*  Chat via (transport DLL argv[2]) with (user at address argv[3]) */
		chatTest(adrString, FALSE);
		break;
		
	default:
		printf("bogus argument %c\n",cmd);
		exit(1);
	}

	printf("Unloading comm drivers....\n");
	dpio_destroy(dpio, 0);
	printf("Unloaded.\n");

    return(0);
}


