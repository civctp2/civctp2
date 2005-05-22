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
 $Log: rawtest.c $
 Revision 1.10  1997/06/02 01:29:43  dkegel
 1. Exit if ^C (or cmd-.) is detected.
 2. Flush stdout after every putch (for mac).
 Revision 1.9  1997/05/30 18:53:59  dkegel
 1. Switched to raw.h from conio.h.
 2. Print error if can't load DLL.
 3. Added Mac support.
 4. Syntax cleanup.
 Revision 1.8  1997/01/31 03:15:15  dkegel
 Added dprint at top of main.  Works in Windows!
 Revision 1.7  1997/01/31 02:35:55  dkegel
 Turn on debug log.
 Revision 1.6  1997/01/30 02:00:33  dkegel
 Slightly improved debug msgs.
 Revision 1.5  1996/12/16 03:36:14  dkegel
 Don't really need io.h
 Revision 1.4  1996/12/16 00:12:26  dkegel
 Use C comments, not C++ comments.
 Revision 1.3  1996/12/14 04:09:22  dkegel
 Don't use 'BOOL' type...
 Revision 1.2  1996/12/13 21:00:38  dkegel
 Now uses dprint.h instead of dprint_.h
 Revision 1.1  1996/12/10 20:41:14  dkegel
 Initial revision

 Test program for the underlying unreliable packet delivery services
 used by ActiveNet.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "dp.h"
#define DEBUG_MODULE TRUE   /*  enable DPRINT in this file */
#include "dprint.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static char		adrString[256];
static int		packetLen;
char        	commDLLName[256];
commInitReq_t	commInitReq;		/*  Parameters needed by some comm drivers. */
char			modeministr[256];
char			phonenum[256];

#ifdef __MWERKS__
/* Mac replacements for some "standard" DOS stuff. */
#include "stricmp.h"
#define putch(c) putchar(c)
#include <console.h>  /* for commandline support */
#endif

#include "raw.h"

/*  Start up comm system, chat with otherPlayer. */
/*  If otherPlayer is NULL or empty, prompt user for other player's address. */
/*  If broadcast is TRUE, ignore otherPlayer, as we don't need to know other */
/*  address to broadcast. */
static void chatTest(char *otherPlayer, int broadcast)
{
    commDriverInfoResp_t 	infoResp;
    commPlayerInfoReq_t  	playerReq;
    commPlayerInfoResp_t 	playerResp;
    commPrintAddrReq_t		printReq;
    commPrintAddrResp_t		printResp;
    commScanAddrReq_t		scanReq;
    commScanAddrResp_t		scanResp;
	commSayHiReq_t			cshReq;
	commSayHiResp_t			cshResp;
	char					printable[50];
	char					adrBuf[16];
	
	printf("chatTest(otherPlayer: %s)\n", otherPlayer);

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
			raw_end();
			printf("Enter chat partner's address:");
			fflush(stdout);
			gets(printable);
			raw_init();
		} else {
			strcpy(printable, otherPlayer);
		}
	
		/*  Convert partner's address to binary for use with commSayHi */
		scanReq.printable = printable;
		scanReq.address = adrBuf;
		scanReq.size = sizeof(adrBuf);
		if (!commScanAddr(&scanReq, &scanResp)) {
			printf("Unable to scan player address, error: %d\n", scanResp.status);
			return;
		}
	
		/*  Open a comm handle to the partner's address */
		cshReq.address = adrBuf;
		cshReq.length = scanResp.length;
		if (!commSayHi(&cshReq, &cshResp)) {
			printf("Unable to connect to player, error: %d\n", cshResp.status);
			return;
		}
		printf("Got player handle: %x\n", cshResp.player);
	}


    for (;;)
    {
        char ch;
        commRxPktReq_t		rxPktReq;
        commRxPktResp_t		rxPktResp;

        /*  see about receiving data */
		rxPktReq.buffer = &ch;
		rxPktReq.size = sizeof(ch);
		
		/*  Check for any received packets */
		if (commRxPkt(&rxPktReq, &rxPktResp)) {
			/*  Make sure this is not a broadcast message from myself. */
			printf("Packet from: %x\n",rxPktResp.src);
			if (rxPktResp.src != PLAYER_ME) {
				putch(ch);						
				/*  Do a linefeed when we need to */
				if (ch == 13) putch(10);
				fflush(stdout);
				/* sleep(3); */
			}
		}
		
		/*  see about sending data */
		if (raw_kbhit()) {
			commTxPktReq_t	txPktReq;
	
			ch = raw_getc();
			putch(ch);
			/*  Do a linefeed when we need to */
			if (ch == 13) putch(10);
			fflush(stdout);
	
			/*  see if user wants to esc */
			if ((ch == 3) || (ch == 0x1b)) break;
	
			if (broadcast) txPktReq.dest = PLAYER_BROADCAST;
			else txPktReq.dest = cshResp.player;
			txPktReq.buffer = &ch;
			txPktReq.length = 1;
			txPktReq.flags  = 0;
						
			commTxPkt(&txPktReq,NULL);
			/* sleep(3); */
		}
		/* sleep(1); */
    }
}

static void fillRamp(char *buffer, int len)
{
	int	i;

	for (i=1; i<=len; i++) buffer[i-1]=(i&15)+'A';
	/* buffer[len-1]=13; */
}

static int badRamp(char *buffer, int len)
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

static void packetTest(int packetLen)
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

		if (raw_kbhit())
		{
			ch = raw_getc();
			/*  see if user wants to esc */
			if ((ch == 3) || (ch == 0x1b)) break;

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
static void onewayTest(int packetLen, int direction)
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

		if (raw_kbhit())
		{
			ch = raw_getc();
			/*  see if user wants to esc */
			if ((ch == 3) || (ch == 0x1b)) break;

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

static void quickTest(int packetLen)
{
	char		buf[1000];
	commTxPktReq_t		txReq;
	commTxPktResp_t		txResp;
	/* commRxPktReq_t		rxReq; */
	/* commRxPktResp_t		rxResp; */


	printf("entering quickTest.\n");
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


/* ======================================================================== *
   Function Name: ProcessCommandLine
   Description  : Processes the command line arguments.  Some arguments are
   handled directly by this routine.  Others simply result in an update to
   the global switches data structure
 * ======================================================================== */
static void ProcessCommandLine(int argc, char **argv) {
  char *chptr;
  int   i;

  commDLLName[0] = 0;
  adrString[0] = 0;
  packetLen=0;

  for (i = 1; i < argc; ++i) {
	if (argv[i][0] == '/' || argv[i][0] == '-') /*  deal with args that start with - or / */
	{
	  if (argv[i][1] == 0) {
		printf("Expected letter after -\n");
		exit(1);
	  }
	  if (argv[i][2] != '=') {
		printf("Expected = after -%c\n", argv[i][1]);
		exit(1);
	  }
	  switch(toupper(argv[i][1])) {
		case 'L':
			printf("Setting packetLen to %s\n", argv[i]+3);
			packetLen = atoi(argv[i]+3);
			printf("PacketLen = %d\n", packetLen);
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
			printf("Got DLL name %s\n", commDLLName);
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

		default:
			printf("Usage: %s cmd [-N=comm.dll][-P=adrString]\n"
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
    commInitResp_t  commInitResp;
	commTermReq_t   termReq;
    dp_result_t err;
    
	#ifdef __MWERKS__
		argc = ccommand(&argv);
	#endif

	if (argc == 1) {
		printf("Usage: %s cmd [-N=comm.dll][-P=adrString]\n"
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
    err = dpLoadDLL(NULL, commDLLName);
    if (err == dp_RES_OK)
	    printf("DLL Loaded.\n");
	else {
		printf("Unable to load DLL!  Error %d.\n", err);
		exit(1);
	}
	
    /*  init comm system */
    if (!commInit(&commInitReq, &commInitResp)) {
		printf("Unable to init comm system, error: %d\n", commInitResp.status);
		dpUnloadDLL(NULL);
		exit(1);
    }
    printf("Inited comm system.\n");

	cmd = argv[1][0];
	if (isupper(cmd)) cmd = tolower(cmd);
	raw_init();

	switch (cmd) {
	case 'q':
		quickTest(packetLen);
		break;

	case 'p':
		packetTest(packetLen);
		break;

	case 't':
		onewayTest(packetLen,1);
		break;

	case 'g':
		onewayTest(packetLen,0);
		break;

	case 'c':
		/*  Chat via (transport DLL argv[2]) with broadcast */
		chatTest(NULL, TRUE);
		break;

	case 'i':
		/*  Chat via (transport DLL argv[2]) with (user at address argv[3]) */
		chatTest(adrString, FALSE);
		break;
		
	default:
		raw_end();
		printf("bogus argument %c\n",cmd);
		exit(1);
	}
	raw_end();

	printf("Unloading comm drivers....\n");
	DPRINT(("unloadComm: commTerm\n"));
	termReq.flags = 0;			/*  hang up phone... */
	commTerm(&termReq, NULL);	/*  This line must come before you unload the DLL - KA 1-29-95 */

	DPRINT(("unloadComm: dpUnloadDLL\n"));
    dpUnloadDLL(NULL);
	printf("Exiting.\n");

    return(0);
}


