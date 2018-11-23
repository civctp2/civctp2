#include <stddef.h>
#include "commapi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include "modm.h"
#include "commapi.h"
#define DEBUG_MODULE TRUE   // enable DPRINT in this file
#include "dprint.h"

#ifndef DYNALINK
#define DYNALINK 1		// set to zero to test with static linking
#endif

#if DYNALINK
#include "DLL.h"		// DLL loader
#include "CDLLHdr.h"	// Header of network MW2 communications DLLs
#include "xpexe.h"

extern exports_exe_t	exports;
extern exports_cl_t		clExports;
commDLLHeader_t *	commDLL = NULL;
commDLLHeader_t *	commDLLOrig = NULL;
exports_commDLL_t *	commImports;
#endif

#if 1

/*----------------------------------------------------------------------
 Called internally by dpCreate to load a DLL.
----------------------------------------------------------------------*/
int dpLoadDLL(char *transportName)
{
#if DYNALINK
	int i;

	// Valid segment alignment masks
	static int	alignMask[] = {1-1, 2-1, 4-1, 16-1, 256-1, 4096-1};

	DPRINT(("dpLoadDLL: transport %s\n", transportName));

	if (commDLL)
		return 1;		// Don't allow double loading.

	if ((commDLL = DLL_load(transportName, DLLMEM_ALLOC, NULL)) == NULL) {
		//printf("Cannot load DLL.\n");
		return 2;
	}
	DPRINT(("DLL_load(ed). searching for signature\n"));
	commDLLOrig = commDLL;
	for ( ; ; ) {
		for (i = 0; ; ) {
			if (commDLL->magic == COMMDLL_MAGIC) break;
			if (++i == orderof(alignMask)) return 3;
			commDLL =
				(void *)
					((unsigned long) ((char *) commDLL + alignMask[i])
						& ~alignMask[i]);
		}

		if (commDLL->version > COMMDLL_FORMAT) return 4;

		if (commDLL->sizeIf16Bit == 0) break;

		commDLL = (void *) ((char *) commDLL + commDLL->sizeIf16Bit);
	}
	DPRINT(("dpLoadDLL: Found signature, version is %x\n", commDLL->version));

	if (commDLL->end != NULL) return 5;

	commImports = commDLL->exports;		// Exe imports what DLL exports
	commDLL->imports = &exports;		// DLL imports what exe exports
	commDLL->clImports = &clExports;	// DLL imports what exe's C lib exports
	
#endif
	return 0;
}
quitAll(void)
{
	printf("Game Over!\n");
	fflush(stdout);
	commTerm(NULL,NULL);
	exit(1);
}
#endif

test1(commInitReq_t *init_req, commTermReq_t *term_req)
{
	clock_t rope;
	int j;
	char buf[512];
	int nGot = 0;
	commRxPktReq_t rxReq;
	commRxPktResp_t rxResp;
	commTxPktReq_t txReq;
	commTxPktResp_t txResp;
	commInitResp_t	init_resp;

	if (!commInit(init_req,&init_resp)) {
		printf("Test Failed--Couldn't init: status %d\n", init_resp.status);
		exit(0);
	}
#if 1
	// Call ipx2_get, even though we don't expect any packets, to start
	// listening.
	rxReq.size = sizeof(buf);
	rxReq.buffer = buf;
	if (commRxPkt(&rxReq,&rxResp)) {
		printf("Test Failed--Buffer not empty at start\n");
		quitAll();
	}
#endif
#if 1
	printf("test1: loop beginning now!\n");
	for (j=0; j<3; j++) {

		txReq.dest = PLAYER_BROADCAST;
		txReq.buffer = "Hello World\n";
		txReq.length = 12;
		txReq.flags = 0;
		if (!commTxPkt(&txReq, &txResp)) {
			printf("Test Failed--Couldn't send packet\n");
			printf("txResp->status = %d\n", txResp.status);
			quitAll();
		}
		txReq.dest = PLAYER_BROADCAST;
		txReq.buffer = "Goodbye World\n";
		txReq.length = 15;
		txReq.flags = 0;
		// r = ipx2_put(, "Goodbye World\n", 15, ipx2_HDL_BROADCAST);
		if (!commTxPkt(&txReq, &txResp)) {
			printf("Test Failed--Couldn't send second packet\n");
			quitAll();
		}
		for (rope=clock()+CLOCKS_PER_SEC; rope>clock(); ) {
	
			if (kbhit()) break;

			rxReq.size = sizeof(buf);
			rxReq.buffer = buf;
			commRxPkt(&rxReq, &rxResp);
			if (rxResp.status == 2) {
				printf("Test Failed--Error receiving packet\n");
				quitAll();
			}
			if (rxResp.status != 0) continue;
			nGot++;
			if (rxResp.length != 12 && rxResp.length != 15) {
				printf("Test Failed-- j %d, Length %d received not as sent\n", j, rxResp.length);
				quitAll();
			}
			if (memcmp(buf, "Hello World\n", 12) != 0 
			&& memcmp(buf, "Goodbye World\n", 15) != 0) {
				printf("Test Failed--Bad data received\n");
				quitAll();
			}
			fwrite(buf, 1, rxResp.length, stdout);
		}
	}

	commTerm(term_req,NULL);
	if (!nGot) {
		printf("Warning: no packet received!\n");
		return;
	}
#endif
	printf(".");
}

/*-------------------------------------------------------------------------
 Listen for packets and put them to the screen.
-------------------------------------------------------------------------*/
test2(commInitReq_t *init_req, commTermReq_t *term_req)
{
	char buf[512];
	int nGot = 0;
	commRxPktReq_t rxReq;
	commRxPktResp_t rxResp;
	commInitResp_t	init_resp;
	clock_t send_interval = CLOCKS_PER_SEC / 5;
	clock_t last_sent;

	if (!commInit(init_req,&init_resp)) {
		printf("Test Failed--Couldn't init: status %d\n", init_resp.status);
		exit(0);
	}
	last_sent = clock();
	while (!kbhit()) {

		if (clock()-send_interval < last_sent)
			continue;
		last_sent += send_interval;

		rxReq.size = sizeof(buf);
		rxReq.buffer = buf;
		while (commRxPkt(&rxReq, &rxResp)) {
			nGot++;
			printf("src %d, length %d, data %02x%02x%02x%02x nGot %d\n",
				rxResp.src, rxResp.length, buf[0], buf[1], buf[2],
				buf[3], nGot);
		}
		if (rxResp.status != 0) printf("status %d\n", rxResp.status);
	}

	commTerm(term_req,NULL);
	if (!nGot) {
		printf("Warning: no packet received!\n");
		return;
	}
	printf(".");
}

/*-------------------------------------------------------------------------
 Send a packet each time the user hits a key.
-------------------------------------------------------------------------*/
test3(commInitReq_t *init_req, commTermReq_t *term_req)
{
	char buf[512];
	int nGot = 0;
	int c;
	commTxPktReq_t txReq;
	commTxPktResp_t txResp;
	commInitResp_t	init_resp;

	if (!commInit(init_req,&init_resp)) {
		printf("Test Failed--Couldn't init: status %d\n", init_resp.status);
		exit(0);
	}
	while (((c = getch()) != EOF) && (c != 27)) {
		buf[0] = c;
		txReq.dest = PLAYER_BROADCAST;
		txReq.buffer = buf;
		txReq.length = 1;
		txReq.flags = 0;
		if (!commTxPkt(&txReq, &txResp)) {
			printf("Test Failed--Couldn't send packet\n");
			printf("txResp->status = %d\n", txResp.status);
			quitAll();
		}
		printf("test3: sent pkt; dest %d, length %d, contents %02x\n",
			txReq.dest, txReq.length, buf[0]);
	}

	commTerm(term_req,NULL);
	if (!nGot) {
		printf("Warning: no packet received!\n");
		return;
	}
	printf(".");
}

/*-------------------------------------------------------------------------
 Send a 200 byte packet every 1/5th of a second
-------------------------------------------------------------------------*/
test4(commInitReq_t *init_req, commTermReq_t *term_req)
{
	char buf[512];
	int nGot = 0;
	commTxPktReq_t txReq;
	commTxPktResp_t txResp;
	commInitResp_t	init_resp;
	clock_t send_interval = CLOCKS_PER_SEC / 5;
	clock_t last_sent;
	int i;

	int pktnum;
	for (i=0; i<200; i++)
		buf[i] = i;

	if (!commInit(init_req,&init_resp)) {
		printf("Test Failed--Couldn't init: status %d\n", init_resp.status);
		exit(0);
	}
	last_sent = clock();
	pktnum = 0;
	while (!kbhit()) {
		if (clock()-send_interval < last_sent)
			continue;
		buf[0] = pktnum++;
		last_sent += send_interval;
		txReq.dest = PLAYER_BROADCAST;
		txReq.buffer = buf;
		txReq.length = 200;
		txReq.flags = 0;
		if (!commTxPkt(&txReq, &txResp)) {
			printf("Test Failed--Couldn't send packet\n");
			printf("txResp->status = %d\n", txResp.status);
			quitAll();
		}
		printf(".");
	}

	commTerm(term_req,NULL);
	if (!nGot) {
		printf("Warning: no packet received!\n");
		return;
	}
}

/*-------------------------------------------------------------------------
 Send a 200 byte packet every 1/5th of a second
-------------------------------------------------------------------------*/
test5(commInitReq_t *init_req, commTermReq_t *term_req, int reInitOften)
{
	char buf[512];
	int nGot = 0;
	commTxPktReq_t txReq;
	commTxPktResp_t txResp;
	commInitResp_t	init_resp;
	commRxPktReq_t rxReq;
	commRxPktResp_t rxResp;
	clock_t send_interval = CLOCKS_PER_SEC / 5;
	clock_t last_sent;
	int i;

	int pktnum;
	for (i=0; i<200; i++)
		buf[i] = i;

	if (!commInit(init_req,&init_resp)) {
		printf("Test Failed--Couldn't init: status %d\n", init_resp.status);
		exit(0);
	}
	last_sent = clock();
	pktnum = 0;
	while (!kbhit()) {
		if (clock()-send_interval < last_sent)
			continue;
		buf[0] = pktnum++;
		last_sent += send_interval;
		txReq.dest = PLAYER_BROADCAST;
		txReq.buffer = buf;
		txReq.length = 200;
		txReq.flags = 0;
		if (!commTxPkt(&txReq, &txResp)) {
			printf("Test Failed--Couldn't send packet\n");
			printf("txResp->status = %d\n", txResp.status);
			quitAll();
		}
		printf(".");
		rxReq.size = sizeof(buf);
		rxReq.buffer = buf;
		while (commRxPkt(&rxReq, &rxResp)) {
			nGot++;
			printf("src %d, length %d, data %02x%02x%02x%02x nGot %d\n",
				rxResp.src, rxResp.length, buf[0], buf[1], buf[2],
				buf[3], nGot);
		}
		if (rxResp.status != 0) printf("status %d\n", rxResp.status);
		if (reInitOften) {
			commTermReq_t termReq;
			init_req->flags = 0;
			termReq.flags = 0;
			if (!commTerm(&termReq,NULL)) {
				printf("commTerm failed!\n");
				exit(1);
			}
			sleep(1);
			printf("test5: about to call commInit()\n");
			if (!commInit(init_req,NULL)) {
				printf("comm re-init failed!\n");
				exit(1);
			}
		}
	}

	commTerm(term_req,NULL);
	if (!nGot) {
		printf("Warning: no packet received!\n");
		return;
	}
}

main(int argc, char **argv)
{
	commInitReq_t req;
	commTermReq_t termreq;

	if (argc == 1) {
		printf("Usage: modmtest 1/rx/tx/stx/rxtx/rxtx2 [phonenum] [comminit commterm] [modeministr]");
		exit(1);
	}
	dpLoadDLL("modem.dll");
	req.modeministr = NULL;
	req.phonenum = NULL;
	req.flags = 0;
	termreq.flags = 0;
	if (argc > 2)
		req.phonenum = argv[2];
	if (argc >= 5) {
		req.flags = atoi(argv[3]);
		termreq.flags = atoi(argv[4]);
	}
	if (argc == 6)
		req.modeministr = argv[5];
	req.baud = 19200;
	req.portnum = 1;
	req.reqLen = sizeof(req);
	printf("phone number: %s\n", req.phonenum);
	printf("baud: %d\n", req.baud);
	printf("portnum: %d\n", req.portnum);
	if (!stricmp(argv[1], "1")) {
		test1(&req, &termreq);
	} else if (!stricmp(argv[1], "rx")) {
		test2(&req, &termreq);
	} else if (!stricmp(argv[1], "tx")) {
		test3(&req, &termreq);
	} else if (!stricmp(argv[1], "stx")) {
		test4(&req, &termreq);
	} else if (!stricmp(argv[1], "rxtx")) {
		test5(&req, &termreq, 0);
	} else if (!stricmp(argv[1], "rxtx2")) {
		test5(&req, &termreq, 1);
	} else {
		printf("unknown argument: %s\n", argv[1]);
		exit(1);
	}
	exit(0);
}
