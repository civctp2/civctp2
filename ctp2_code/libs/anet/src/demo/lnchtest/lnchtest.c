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
 $Log: lnchtest.c $
 Revision 1.1  1997/10/03 01:12:31  anitalee
 Initial revision

 Test program for launching.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <winbase.h>

#include "dp.h"
#define DEBUG_MODULE TRUE   /*  enable DPRINT in this file */
#include "dprint.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

static char		adrString[256];
char        	commDLLName[256];
commInitReq_t	commInitReq;		/*  Parameters needed by some comm drivers. */
char			modeministr[256];
char			phonenum[256];
long			sessionid = 0;
long			baseadr = 0;

static void quickTest(char *buf, int packetLen)
{
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

static void ProcessCommandLine(int argc, char **argv) {
  char *chptr;
  int   i;

  commDLLName[0] = 0;
  adrString[0] = 0;

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
	  	case 'B':	/*  Set port handle */
			baseadr =  atol(argv[i]+3);
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

		case 'S':	/* Set session id */
			sessionid = atol(argv[i]+3);
			break;
			
		case 'W':   /*  Set phone number */
			strncpy(phonenum, argv[i]+3, sizeof(phonenum));
			phonenum[sizeof(phonenum)-1] = '\0';
			break;

		case 'Y':   /*  Set comm port */
			sscanf(argv[i]+3, "%ld", &commInitReq.portnum);
			break;

		default:
			printf("Usage: %s [-N=comm.dll][-W=phonenum][-Y=commportnum(0 = com1)]\nUnknown arg %s\n", argv[0], argv[i]);
		    exit(1);
	  } /*  end switch */
	}
  }
}

int main( int argc, char *argv[] )
{
	int i;
	int packetLen;
	char cmd[256];
	char args[256];
	char buf[256];
    commInitResp_t  commInitResp;
	commTermReq_t   termReq;
    dp_result_t err;
    
	if (argc == 1) {
		printf("Usage: %s [-N=comm.dll][-W=phonenum][-Y=commportnum(0 = com1)]\n", argv[0]);
		printf("need to specify comm.dll\n");
		return(1);
	}
	strcpy(cmd, argv[0]);
	strcpy(args, argv[1]);
	for (i = 2; i < argc; i++) {
		strcat(args, " ");
		strcat(args, argv[i]);
	}

	modeministr[0] = 0;
	phonenum[0] = 0;
	ProcessCommandLine(argc, argv);

	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.modeministr = modeministr;
	commInitReq.phonenum = phonenum;
	commInitReq.baud = 19200;
	commInitReq.hwirq = 0;
	commInitReq.portnum = 1;
	commInitReq.dialing_method = 1;
	if (baseadr && sessionid) {
		commInitReq.sessionId = sessionid;
		commInitReq.baseadr = baseadr;
		commInitReq.flags = comm_INIT_FLAGS_RESUME;
		printf("got sessionId %ld, baseadr %ld, portnum %d\n", commInitReq.sessionId, commInitReq.baseadr, commInitReq.portnum);
	} else {
		commInitReq.sessionId = rand() | (rand() << 16);	
	}
	commInitReq.reqLen = sizeof(commInitReq_t);

    dp_enableDebugPrint(1);
	printf(("Start of rawtest.c\n"));

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

	if (baseadr && sessionid) {
		strcpy(buf, "Sent from second sess");
		packetLen = strlen(buf) + 1;
		printf("Press enter when ready\n");
		getc(stdin);
	} else {
		strcpy(buf, "Sent from first sess");
		packetLen = strlen(buf) + 1;
	}

	quickTest(buf, packetLen);

	if (!baseadr && !sessionid) {
		STARTUPINFO StartupInfo;
		PROCESS_INFORMATION ProcessInformation;
		memset(&StartupInfo, 0, sizeof(STARTUPINFO));
		StartupInfo.cb = sizeof(StartupInfo);
		sprintf(buf, " -n=%s -b=%ld -s=%ld -y=%d", commDLLName, commInitReq.baseadr, commInitReq.sessionId);
		strcat(args, buf);
		printf("cmd: %s %s\n", cmd, args);
		CreateProcess(cmd, args, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInformation);
	}

	printf("Unloading comm drivers....\n");
	printf(("unloadComm: commTerm\n"));
	termReq.flags = 1;			/*  don't hang up phone... */
//	termReq.flags = 0;			/*  hang up phone... */
	commTerm(&termReq, NULL);	/*  This line must come before you unload the DLL - KA 1-29-95 */

	DPRINT(("unloadComm: dpUnloadDLL\n"));
    dpUnloadDLL(NULL);
	printf("Exiting.\n");

    return(0);
}


