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
 Test of dpio handle open, data send, handle close

 Usage: dpiot -d=driver -h=n [options]
 or:    dpiot -d=driver -h=n -n=hostnum -a=nextAddress [options]

 This test program measures the performance of the dpio module.
 It measures the time it takes various things to propagate around a ring
 of n hosts.  The things sent around the ring include:
 * a single small (four-byte) packet
 The results are reported in seconds per hop for the small entries,
 and bytes per second for the large entries.

 The first form spawns n processes using the second form, and waits for
 them all to finish.  I.e. for i=0..n, it executes
    dpiot -d=driver -h=n -n=i -a=((i+1 mod n) + 1).0.0.0
 For example, if driver is wloop.dll, and N == 2, it executes:
	dpiot -d=wloop.dll -h=2 -n=0 -a=2.0.0.0
	dpiot -d=wloop.dll -h=2 -n=1 -a=1.0.0.0
 If N == 4, it executes:
	dpiot -d=wloop.dll -h=4 -n=0 -a=2.0.0.0
	dpiot -d=wloop.dll -h=4 -n=1 -a=3.0.0.0
	dpiot -d=wloop.dll -h=4 -n=2 -a=4.0.0.0
	dpiot -d=wloop.dll -h=4 -n=3 -a=1.0.0.0

 This assumes we're using one of Marshall's loopback drivers,
 which assign the address 1.0.0.0 to the first host, 2.0.0.0 to the second,
 etc.  The expression ((i+2) mod n + 1).0.0.0 is the address of the 'next'
 host to be opened, the one with i one higher than this host.
 We might later add the ability to specify a list of real network addresses
 rather than having the first form synthesize loopback driver addresses.

 The second form expects a driver filename, its host number, and the network
 address of the 'next' host in the ring.  It creates a ring of hosts
 forwarding a table to each other, and measures performance of the code
 that propagates table entries around the ring.  See the function
 run_one_node() for details.

Freeze and Thaw Test
 The freeze and thaw test uses the same structure, where the first form
 spawns n processes of the second form.  For this test, the second form
 runs in a loop.  At certain times in the loop, one of the tests will
 freeze and exit; the first form will expect this and start up a second
 form that will thaw and continue the looped test from where the
 predecessor left off.
--------------------------------------------------------------------------*/

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include "eclock.h"
#include "../../../demo/utils/unistd.h"
#include "dp.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/**
* Constants
*/

#define RECEIVE_FLAGS	0
#define BUFSIZE			40000
#define MAXPKTS			255		/* to increase, need to change min pkt size */

#define TIMEOUT_DELAY	90000

#define LOGNAME "dpio%d.log"
#define FREEZENAME "dpio%d.frz"

#define PKT_SU dppt_MAKE('S', 'U')		/* Status */
#define PKT_TS dppt_MAKE('T', 'S')		/* Test */
#define PKT_QQ dppt_MAKE('Q', 'Q')		/* Quit */

#define PORT_OFFSET	13231
#define INET_ADDR "207.82.188.84"

/**
* Types
*/

/* Structure to hold arguments to the test */
typedef struct dpiotArgs_s {
	char *adrbuf;
	int pktsize;
	long portnum;
	char *phonenum;
	char *freeze;
	boolean bFreeze;
} dpiotArgs_t;

/* Structure to hold the results of the test */
typedef struct dpiotResult_s {
	/* When test item is sent out from the master */
	clock_t sentSingleSmallTime;
	clock_t sentMultipleSmallTime;
	clock_t sentSingleLargeTime;
	/* When test item comes back around the ring to the master */
	clock_t gotSingleSmallTime;
	clock_t gotMultipleSmallTime;
	clock_t gotSingleLargeTime;

	/* Final computed values */
	float singleSmallTime;

	int thisHost;		/* 0..n_hosts-1; if 0, this host is master */

	int n_hosts;

	int loops;
	int pkts;
	int pktsize;

	char *driver;
	char *exe;

	int packetLoss;
} dpiotResult_t;

/**
* Global variables
*/

dpiotResult_t results;	/* Must be global for signal handler */
FILE* logFile;			/* Must be global for signal handler */
char buf[BUFSIZE];
char usage[1024];
playerHdl_t qq_src;		/* Who sent us a QQ packet */
playerHdl_t su_src;
int su_num;
int su_pktnum;
playerHdl_t hdl_opened;
playerHdl_t hdl_deleted;
clock_t dpio_now;

/**
* Methods
*/

/*-------------------------------------------------------------------------
 Write the results to the log file
-------------------------------------------------------------------------*/
void
writeReport(
	FILE* log,
	dpiotResult_t* res)
{
	printf("Writing report for node %d\n", res->thisHost);
	if (res->thisHost < 1) {
		fprintf(log, "Table Ring Test\nhosts %d, loops %d, npkts %d, pktsize %d\nDriver %s\npktloss %d%%\n",
			res->n_hosts, res->loops, res->pkts, res->pktsize,
			res->driver, res->packetLoss);

		if (res->singleSmallTime == -1) {
			fprintf(log, "Failure: set did not propagate\n");
			return;
		}

		if (res->pkts < 2) {
			fprintf(log, "singleSmallTime %f seconds (%f seconds/hop)\n",
				res->singleSmallTime, res->singleSmallTime / res->n_hosts);
		} else {
			fprintf(log, "singleSmallTime %f seconds (%f bytes/sec)\n",
				res->singleSmallTime, res->pktsize * (res->pkts - 1) / res->singleSmallTime);
		}
	}
}

/*-------------------------------------------------------------------------
 Read the master's results from the log file
-------------------------------------------------------------------------*/
void
readReportToStdout(
	FILE* log,
	dpiotResult_t* res)
{
	char test[128];
	float val;
	int write = 0;
	while (fgets(buf, BUFSIZE, log) != NULL) {
		if (sscanf(buf, "%s %f", test, &val) == 2) {
			if (strcmpi(test, "singleSmallTime") == 0) {
				res->singleSmallTime = val;
				write++;
			}
			if (strcmpi(test, "pktloss") == 0) {
				res->packetLoss = (int)val;
				write++;
			}
		}
		if (write == 2) {
			writeReport(stdout, res);
			write = 0;
		}
	}
}

/*-------------------------------------------------------------------------
 Stop the test
-------------------------------------------------------------------------*/
void
abortTest(
	FILE* log,
	int exitCode,
	char* message,
	dpiotResult_t* res)
{
	fflush(log);
	fputs(message, log);
	if(res != NULL)
		writeReport(log, res);
	fclose(log);
	fprintf(stdout, "%s", message);
	exit(exitCode);
}

/*-------------------------------------------------------------------------
 Stop the test if something went wrong that we thought would never happen
-------------------------------------------------------------------------*/
#define assert(x) myassert((int)(x), __LINE__)
void myassert(int x, int lineno)
{
	if (!(x)) {
		char buf[256];
		sprintf(buf, "Node %d: Assertion failed, line %d\n", results.thisHost, lineno);
		printf("%s", buf);
		abortTest(logFile, 9, buf, NULL);
	}
}

/*-------------------------------------------------------------------------
 Timeout
-------------------------------------------------------------------------*/
int timer_handler(int sigtype)
{
	fflush(stdout);
	printf("\nTimeout\n");
	abortTest(logFile, 3, "\nTimeout\n", NULL);
	return 0;
}

/*-------------------------------------------------------------------------
 Handle a signal (^C or similar)
-------------------------------------------------------------------------*/
void
signalHandler(
	int sigId)
{
	abortTest(logFile, 4, "Test terminated at user request.\n", &results);
}

/*----------------------------------------------------------------------
 Callback for opening dpio handles which insures that necessary cleanup
 happens on remote close
----------------------------------------------------------------------*/
static void dp_PASCAL dp_openHdl_cb(
	playerHdl_t hdl, int n_hdls, dp_result_t status, void *context)
{
	DPRINT(("dp_openHdl_cb: h:%x status:%d\n", hdl, status));

	if (status == dp_RES_CLOSED) {
		printf("Node %d: Handle h:%x closed remotely\n", results.thisHost, hdl);
		hdl_deleted = hdl;
	}
	if (status == dp_RES_OPENED) {
		printf("Node %d: Handle h:%x opened remotely\n", results.thisHost, hdl);
		hdl_opened = hdl;
	}
}

/*-------------------------------------------------------------------------
 Check for network packets and handle them.
 Sets globals qq_src and su_src if/when qq or su packet received.
-------------------------------------------------------------------------*/
void poll_test(dpio_t *dpio)
{
	dp_result_t err;
	playerHdl_t src;
	size_t size;
	char pkt[dpio_MAXLEN_UNRELIABLE];

	dpio_now = eclock();
	err = dpio_update(dpio);
	if (err != dp_RES_OK && err != dp_RES_EMPTY) {
		printf("dpio_update reports error %d\n", err);
		assert(FALSE);
	}
	/*printf("%1d", results.thisHost);*/
	src = (dpid_t) PLAYER_NONE;	/* kludge */
	su_src = PLAYER_NONE; /* kludge */
	size = sizeof(pkt);
	err = dpio_get(dpio, &src, &pkt, &size, NULL);
	if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
		printf("dpio_get reports error %d\n", err);
		assert(FALSE);
	}
	if (err == dp_RES_OK) {
		/*printf("Got packet from handle %x\n", src);*/
		switch (*(dp_packetType_t *)pkt) {
		case PKT_SU:
			su_src = src;
			su_num = pkt[results.pktsize + 1];
			su_pktnum = (unsigned char)pkt[results.pktsize];
			printf("Node %d: Got su packet from source h:%x\n", results.thisHost, src);
			results.gotSingleSmallTime = dpio_now;
			results.singleSmallTime = ((float)dpio_now - results.sentSingleSmallTime) / ECLOCKS_PER_SEC;
			/* Move this statement to right after last measurement taken */
			if (results.pkts < 2)
				writeReport(logFile, &results);

			break;
		case PKT_TS:
			printf("Node %d: Got TS packet from source h:%x\n", results.thisHost, src);
			break;
		case PKT_QQ:
			qq_src = src;
			printf("Node %d: Got qq packet from source h:%x\n", results.thisHost, src);
			break;
		default:
			;
		}
	}
}

/*-------------------------------------------------------------------------
 Handle command-line arguments
-------------------------------------------------------------------------*/
int						/* status */
handleArguments(
	dpiotArgs_t *arg,	/* (output) values */
	int argc,			/* (input) number of arguments */
	char *argv[] )		/* (input) value of arguments */
{
	char buf[BUFSIZE];
	char usage[BUFSIZE];
	int i;

	sprintf(usage,
"Usage: %s -h=host_count(max=200) -d=driver [-nofreeze]\n\
 [-l=loop_count(default=1:max=127)] [-p=packet_count(default=1:max=%d)]\n\
 [-s=packet_size(default=%d:max=%d] [-n=my_host_number]\n\
 [-a=next_host_address] [-y=port_number] [-w=phone_number]\n\
or: (internal use) %s -f=freeze_file -n=my_host_number\n\
 where freeze_file specifies an existing freeze file;\n",
argv[0], MAXPKTS, sizeof(dp_packetType_t), dpio_MAXLEN_UNRELIABLE-2, argv[0]);

	/* Handle each command-line argument */
	for(i = 0; i < argc; i++) {
		if((argv[i][0] == '-') || (argv[i][0] == '/')) {
			if (!strcmp(argv[i],"-nofreeze") || !strcmp(argv[i],"/nofreeze")) {
				arg->bFreeze = FALSE;
				continue;
			}
			if ((argv[i][1] == '\0') || ((argv[i][2] != '=') &&
					(argv[i][2] != ':') && (argv[i][2] != '\0'))) {
				sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
				abortTest(logFile, 3, buf, NULL);
			} else if((argv[i][2] != '\0') && (argv[i][3] == '\0')) {
				sprintf(buf, "Value expected: %s\n%s", argv[i], usage);
				abortTest(logFile, 3, buf, NULL);
			}
			switch(argv[i][1]) {
				case 'a': case 'A':	/* host address of next machine in ring */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					arg->adrbuf = argv[i]+3;
					break;
				case 'd': case 'D':	/* driver filename */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					results.driver = argv[i]+3;
					break;
				case 'f': case 'F': /* freeze file name */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					arg->freeze = argv[i]+3;
					break;
				case 'h': case 'H':	/* number of hosts in ring */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.n_hosts))!=1) {
						sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					if (results.n_hosts < 2 || results.n_hosts > 200) {
						sprintf(buf, "Invalid number of hosts (%s)\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					break;
				case 'l': case 'L':	/* number of times to go around ring */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.loops)) != 1) {
						sprintf(buf, "Error opening log file %s\n%s", argv[i]+3, usage);
						abortTest(stdout, 3, buf, NULL);
					}
					if (results.loops < 1 || results.loops > 127) {
						sprintf(buf, "Invalid number of loops (%s)\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					break;
				case 'n': case 'N':	/* player (me) */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.thisHost))!=1) {
						sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					break;
				case 'p': case 'P':	/* number of packets to propagate */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.pkts)) != 1) {
						sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
						abortTest(stdout, 3, buf, NULL);
					}
					if (results.pkts < 1 || results.pkts > MAXPKTS) {
						sprintf(buf, "Invalid number of pkts (%s)\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					break;
				case 's': case 'S':	/* packet size */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%d", &(results.pktsize)) != 1) {
						sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
						abortTest(stdout, 3, buf, NULL);
					}
					if (results.pktsize < sizeof(dp_packetType_t) || results.pktsize > dpio_MAXLEN_UNRELIABLE - 2) {
						sprintf(buf, "Invalid packet size (%s)\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					break;
				case 'w': case 'W':	/* phone number */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					arg->phonenum = argv[i]+3;
					break;
				case 'y': case 'Y':	/* port number */
					if(argv[i][2] == '\0') {
						sprintf(buf, "Value required: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					} else if(sscanf(argv[i]+3, "%ld", &(arg->portnum)) != 1) {
						sprintf(buf, "Malformed switch: %s\n%s", argv[i], usage);
						abortTest(logFile, 3, buf, NULL);
					}
					break;
				default:
					sprintf(buf,"Illegal switch: %s\n%s", argv[i], usage);
					abortTest(logFile, 3, buf, NULL);
			}
		}
	}

	if(*arg->freeze) {
		if(results.thisHost < 0)
			abortTest(logFile, 3, "Must use nonnegative my_host_number", NULL);
		return 0;
	}
	if(results.n_hosts < 2)
		abortTest(logFile, 3, "Must have at least two hosts.", NULL);
	if(!*(results.driver))
		abortTest(logFile, 3, "Must specify transport driver.", NULL);
	if((results.thisHost < 0 && *arg->adrbuf) || (results.thisHost >= 0 && !*(arg->adrbuf))) {
		sprintf(buf, "Must have nonnegative my_host_number %d and non-empty next_host_address %s.\n", results.thisHost, arg->adrbuf);
		abortTest(logFile, 3, buf, NULL);
	}
	if(results.thisHost >= results.n_hosts) {
		sprintf(buf, "my_host_number %d must smaller than host count %d.\n", results.thisHost, results.n_hosts);
		abortTest(logFile, 3, buf, NULL);
	}
/*	printf("n(hosts:%d,loops:%d,pkts:%d,pktsize:%d),thisHost:%d,driver:%s,adrbuf:%s\n",results.n_hosts,results.loops,results.pkts,results.pktsize,results.thisHost,results.driver,arg->adrbuf); */

	return 0;
}

/*-------------------------------------------------------------------------
 Run a single copy of the test.

 Return value is 0 on successful end and negative for error.  A positive
 nonzero return code indicates that the copy froze during the listed
 iteration of the loop (e.g., return 1 indicates that the freeze occurred
 at the beginning of the i==1 iteration of the loop), and expects that a
 new version will be started by thawing and running at that iteration
 of the loop.  (Note that this means none of the tests may freeze during
 the first iteration, since a return value of 0 indicates successful
 completion).

 Does the following steps:
 1. Initialize dpio
 2. If childnum is > 0, this child is a slave:
    Open a handle to address
 3. If childnum == 0, this child is the master:
	Wait for someone to open a handle to us
    Open a handle to address
    Send a packet to address
 4. If childnum > 0:
    Wait until packet recieved, then send it to address.

 N must be 2 or greater.

-------------------------------------------------------------------------*/
int
run_one_node(
	int childNum,
	char *sNextAdr,
	int pkts,
	int pktsize,
	long portnum,
	char *phonenum,
	int loopTotal,
	int endLoopAt)
{
	dp_result_t err;
	dpio_t *dpio;
	playerHdl_t dest;
	unsigned char adrBuf[dp_MAX_ADR_LEN];
	commInitReq_t commInitReq;
	commScanAddrReq_t		scanReq;
	commScanAddrResp_t		scanResp;
	dp_transport_t dll;
	char nbuf[dpio_MAXLEN_UNRELIABLE];
	char dplogname[200];

	char fname[256];
	int i;
	int startLoopAt = 0;

	/* Check parameters */
	assert(childNum >= 0);
	assert(sNextAdr != NULL);
	assert(loopTotal < 128);
	assert(endLoopAt > 0);

	/* Set a timeout of 20 seconds; 200 seconds if running serial driver
	 * or multiple packets */
	signal(SIGTIMER, timer_handler);
	if (portnum < 0 && pkts < 2) {
		alarm(20);
	} else {
		alarm(200);
	}

	results.thisHost = childNum;

	sprintf(fname, LOGNAME, childNum);
	if (loopTotal < 1) {	/* thawing */
		assert ((logFile = fopen(fname, "a")) != NULL);
		fprintf(logFile, "--Thawing--\n");
	} else
		assert ((logFile = fopen(fname, "w")) != NULL);

	hdl_opened = PLAYER_NONE;
	su_src = PLAYER_NONE;
	su_num = -1;
	su_pktnum = -1;

	#ifdef WIN32
		/* Randomize so dpio's starting packet sequence number is random */
		srand(GetTickCount());
	#endif

	/* 1. Initialize dpio */
	printf("Node %d step 1\n", childNum);
	dpio_now = eclock();

	if (loopTotal < 1) {	/* thawing */
		FILE* thawFile;
		sprintf(fname, FREEZENAME, childNum);
		printf("Node %d: Thawing from file %s.\n", childNum, fname);
		assert ((thawFile = fopen(fname, "r")) != NULL);

		fread(&startLoopAt,sizeof(int),1,thawFile);
		fread(&loopTotal,sizeof(int),1,thawFile);
		fread(&su_num,sizeof(int),1,thawFile);
		fread(&pkts,sizeof(int),1,thawFile);
		fread(&pktsize,sizeof(int),1,thawFile);
		fread(&(results.n_hosts),sizeof(int),1,thawFile);
		results.pkts = pkts;
		results.pktsize = pktsize;

		sprintf(dplogname, "dp%d.%d.log", childNum,startLoopAt);
		dp_setLogFname(dplogname);

		err = dpio_create(&dpio, NULL, NULL, &dpio_now, thawFile);
		assert(err == dp_RES_OK);
		err = dpio_thawHdl(dpio, &dest, thawFile);
		assert(err == dp_RES_OK);
		fclose(thawFile);

		/* Re-set any callbacks */
		dpio_setIncomingCallback(dpio, dp_openHdl_cb, NULL);
		results.packetLoss = dpio->rxDropPercent;

		/* Send a test packet */
		printf("Node %d: Sending test packet to address %s on h:%x\n",
			childNum, scanReq.printable, dest);
		(*(dp_packetType_t *)nbuf) = PKT_TS;
		nbuf[sizeof(dp_packetType_t)] = i;
		err = dpio_put_reliable(dpio, &dest, 1, nbuf, sizeof(dp_packetType_t)+2, NULL);
		assert(err == dp_RES_OK);
	} else {	/* not thawing */
		sprintf(dplogname, "dp%d.%d.log", childNum,startLoopAt);
		dp_setLogFname(dplogname);

		memset(&dll, 0, sizeof(dll));
		strcpy(dll.fname, results.driver);
		memset(&commInitReq, 0, sizeof(commInitReq));
		commInitReq.sessionId = childNum + 1;			/* claim our address */
		if (portnum > 0) {
			commInitReq.portnum = portnum;
			if (phonenum) {
				commInitReq.phonenum = phonenum;
				commInitReq.dialing_method = 1;		/* tone dialing */
			}
		} else {
			commInitReq.portnum = childNum + PORT_OFFSET;	/* claim our port number */
		}
		commInitReq.reqLen = sizeof(commInitReq_t);
		err = dpio_create(&dpio, &dll, &commInitReq, &dpio_now, NULL);
		assert(err == dp_RES_OK);
		/* We need to know when the transport layer connects
		 * to new hosts or disconnects from existing hosts.
		 */
		dpio_setIncomingCallback(dpio, dp_openHdl_cb, NULL);

		scanReq.printable = sNextAdr;
		scanReq.address = adrBuf;
		scanReq.size = sizeof(adrBuf);
		if (!commScanAddr(&scanReq, &scanResp)) {
			printf("Unable to scan next host address %s, err: %d",
					scanReq.printable, scanResp.status);
			assert(FALSE);
		}

		results.packetLoss = dpio->rxDropPercent;

		/* 2. If childnum is > 0, this child is a slave:
		 *    Open a handle to address
		 */
		printf("Node %d step 2\n", childNum);
		if (childNum > 0) {
			/*  Open a comm handle to the partner's address */
			dest = dpio_openHdl(dpio, adrBuf, NULL, NULL);
			if (dest == PLAYER_NONE) {
				printf("Unable to connect to address %s", scanReq.printable);
				assert(FALSE);
			}
			printf("Node %d: Opening address %s returns handle %d/%x\n", childNum, scanReq.printable, dest, dest);
			if (dest == PLAYER_ME) {
				printf("Executed out of order - got handle to myself\n");
				assert(FALSE);
			}
		}

		/* 3a. If childnum == 0, this child is the master:
		 *     Wait for someone to open a handle to us
		 *     Open a handle to address
		 */
		printf("Node %d step 3a\n", childNum);
		if (childNum == 0) {
			do {
				poll_test(dpio);
			} while (hdl_opened == PLAYER_NONE);

			/*  Open a comm handle to the partner's address */
			dest = dpio_openHdl(dpio, adrBuf, NULL, NULL);
			if (dest == PLAYER_NONE) {
				printf("Unable to connect to address %s", scanReq.printable);
				assert(FALSE);
			}
			printf("Node %d: Opening address %s returns handle %d/%x\n",
				childNum, scanReq.printable, dest, dest);
			if (dest == PLAYER_ME) {
				printf("Executed out of order - got handle to myself\n");
				assert(FALSE);
			}
		}
	}	/* not thawing */

	for(i = startLoopAt; i < loopTotal; i++) {
		/* If it's time to freeze, do so */
		if(i == endLoopAt) {
			FILE* freezeFile;
			int result;
			int proc;
			char argbuf1[BUFSIZE];
			char argbuf2[BUFSIZE];
			sprintf(fname, FREEZENAME, childNum);
			printf("Node %d: Freezing to file %s.\n", childNum, fname);
			assert ((freezeFile = fopen(fname, "w")) != NULL);

			/* save needed parameters into freezefile */
			fwrite(&i,sizeof(int),1,freezeFile);	/* start loop at */
			fwrite(&loopTotal,sizeof(int),1,freezeFile);
			fwrite(&su_num,sizeof(int),1,freezeFile);
			fwrite(&pkts,sizeof(int),1,freezeFile);
			fwrite(&pktsize,sizeof(int),1,freezeFile);
			fwrite(&(results.n_hosts),sizeof(int),1,freezeFile);

			/* save dpio into freezefile */
			dpio_freeze(dpio, freezeFile);
			err = dpio_freezeHdl(dpio, dest, freezeFile);
			assert(err == dp_RES_OK);
			fclose(freezeFile);
			fclose(logFile);
			dpio_destroy(dpio, 1);	/* 1=don't hang up */

			/* spawn, and return child's return value */
			sprintf(argbuf1, "-n=%d", childNum);
			sprintf(argbuf2, "-f=%s", fname);
			proc = _spawnl(_P_NOWAIT, results.exe, results.exe, argbuf1, argbuf2, NULL);
			_cwait(&result, proc, 0);
			printf("Node %d.%d returned %d\n", childNum, i, result);
			return result;
		}

		/* 3b. If childnum == 0, this child is the master:
		 *     Send a packet to address then wait till get all packets back
		 */
		printf("Node %d step 3b\n", childNum);
		if (childNum == 0) {
			int j, chksum, pktgot;
			boolean first;
			printf("Node %d: Sending %d packets(siz:%d) to addr:%s(h:%x)\n",
				childNum, pkts, pktsize, scanReq.printable, dest);

			for (j = 0, chksum = 0, pktgot = 0, first = TRUE; j < pkts || pktgot < pkts; ) {
				if (pktgot < pkts) {
					do {
						poll_test(dpio);
					} while (su_num < (i-1));
					if (su_num == i && su_src != PLAYER_NONE) {
						printf("Node %d: time %ld: pktgot=%d;pkts=%d;su_num=%d, su_src:%x,pktnum:%d\n",childNum,eclock(),pktgot,pkts,su_num,su_src,su_pktnum);
						chksum += su_pktnum;
						if (pktgot == 0) results.sentSingleSmallTime = eclock();
						pktgot++;
					}
				}

				if (first) {	/* kludgy */
					results.sentSingleSmallTime = eclock();
					first = FALSE;
				}
				if (j >= pkts) continue;  /*done sending packets; just receive*/
				(*(dp_packetType_t *)nbuf) = PKT_SU;
				nbuf[pktsize + 1] = i;
				nbuf[pktsize] = j;
				err = dpio_put_reliable(dpio, &dest, 1, nbuf, pktsize+2, NULL);
				assert(err == dp_RES_OK || err == dp_RES_FULL);
				if (err == dp_RES_OK) j++;
			}
			/* printf("Node %d: got chksum %d for npkts %d\n", childNum,  chksum, pkts); */
			if (chksum != (pkts - 1) * pkts / 2) {
				sprintf(buf, "Node %d: err: got chksum %d for npkts %d\n", childNum,  chksum, pkts);
				abortTest(logFile, 3, buf, NULL);
			} else if (pkts > 1) {
				results.gotSingleSmallTime = dpio_now;
				results.singleSmallTime = ((float)dpio_now - results.sentSingleSmallTime) / ECLOCKS_PER_SEC;
				writeReport(logFile, &results);
			}
		}

		/* 4. If childnum > 0:
		 *      Wait until packet recieved, then send it to address.
		 *      If send buffer is full, save it to send later.
		 */
		/*define TICKLEFINBUG*/	/* see dpio.c if you are interested in ancient bugs */
		#ifdef TICKLEFINBUG
			printf("Node %d: closing handle h:%x; if we are TICKLING the FIN ACK bug, this should free peer up to ack our data\n", childNum, dest);
			dpio_closeHdl(dpio, dest);
		#else
			printf("Node %d step 4\n", childNum);
			if (childNum > 0) {
				int jrx, chksum, jtx;
				for (jrx = 0, chksum = 0, jtx = 0; jtx < pkts; jrx++) {
					if (jrx < pkts) {
						do {
							poll_test(dpio);
						} while ((su_src == PLAYER_NONE) || (su_num < i));
						if (su_num == i && su_src != PLAYER_NONE) {
							printf("Node %d: time %ld: pktgot=%d;pkts=%d;su_num=%d, su_src:%x\n",childNum,eclock(),jrx,pkts,su_num,su_src);
							if (jrx != su_pktnum) {
								sprintf(buf, "Node %d: err: got su_pktnum %d; should be %d\n", su_pktnum, jrx);
								abortTest(logFile, 3, buf, NULL);
							}
							chksum += su_pktnum;
						} else {
							sprintf(buf, "Node %d: err: su_num %d != loop %d or src is PLAYER_NONE\n", childNum,  su_num, i);
							abortTest(logFile, 3, buf, NULL);
						}
					} else		/* received all packets */
						poll_test(dpio);

					printf("Node %d: time %ld: sending packet %d(tot:%d,siz:%d) to addr:%s(h:%x)\n",
						childNum, eclock(), jtx, pkts, pktsize, scanReq.printable, dest);
					(*(dp_packetType_t *)nbuf) = PKT_SU;
					nbuf[pktsize + 1] = i;
					nbuf[pktsize] = jtx;
					err = dpio_put_reliable(dpio, &dest, 1, nbuf, pktsize+2, NULL);
					assert(err == dp_RES_OK || err == dp_RES_FULL);
					if (err == dp_RES_OK)
						jtx++;
				}
				if (chksum != (pkts - 1) * pkts / 2) {
					sprintf(buf, "Node %d: err: got chksum %d\n", childNum, chksum);
					abortTest(logFile, 3, buf, NULL);
				}
			}
		#endif

		/* Wait until all of our packets have been ack'd, then continue. */
		printf("Node %d step 6\n", childNum);
		do {
			poll_test(dpio);
		} while (dpio_ReadyToFreeze(dpio, NULL) == dp_RES_BUSY);

		/* Clear stuff for next iteration */
		su_src = PLAYER_NONE;
	}

	printf("Node %d step 7\n", childNum);
	/* Ack packets for four more seconds just in case */
	{
		clock_t start = eclock();
		while ((long)(eclock() - start) < 4 * ECLOCKS_PER_SEC) {
			poll_test(dpio);
		}
	}
	printf("Node %d exiting\n", childNum);

	return 0;
}

/*-------------------------------------------------------------------------
 Run n copies of the test.
 Return value is 0 on success.
-------------------------------------------------------------------------*/
int run_n_nodes(
	char *exe,
	int loops,
	int pkts,
	int pktsize,
	boolean bFreeze)
{
	char *childArgs[10] = {exe, "driver.internet_or_ipx_or_modem_or_serial", "number_of_hosts", "number_of_loops", "childnum", "nextadr.internet.address.specification:maybeWithPortNumber", "number_of_pkts", "pktsize", "-nofreeze", NULL};
	int* procs = NULL;
	int* result = NULL;
	int i;

	if ((procs = (int *) malloc(results.n_hosts * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate process handle storage.\n", NULL);
	if((result = (int *) malloc(results.n_hosts * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate result storage.\n", NULL);

	/* Start tests */
	for (i=0; i < results.n_hosts; i++) {
		sprintf(childArgs[1], "-d=%s", results.driver);
		sprintf(childArgs[2], "-h=%d", results.n_hosts);
		sprintf(childArgs[3], "-l=%d", loops);
		sprintf(childArgs[4], "-n=%d", i);
#ifdef LOOP
		sprintf(childArgs[5], "-a=%d.0.0.0", ((i+1) % results.n_hosts) + 1);
#elif defined(INET)
		sprintf(childArgs[5], "-a=%s:%d", INET_ADDR, ((i+1) % results.n_hosts) + PORT_OFFSET);
#endif
		sprintf(childArgs[6], "-p=%d", pkts);
		sprintf(childArgs[7], "-s=%d", pktsize);
		if (TRUE == bFreeze)
			childArgs[8] = NULL;
		printf("Launching %s %s %s %s %s %s %s %s\n",
				childArgs[0],
				childArgs[1],
				childArgs[2],
				childArgs[3],
				childArgs[4],
				childArgs[5],
				childArgs[6],
				childArgs[7]);
		fflush(stdout);
		procs[i] = _spawnv(_P_NOWAIT, exe, childArgs);
		if(procs[i] == -1) {
			sprintf(buf, "Unable to start process %d: %s\n", i, strerror(errno));
			abortTest(logFile, 4, buf, NULL);
		}
	}

	/* Wait for tests to finish */
	for (i=0; i < results.n_hosts; i++) {
		int rVal = _cwait(&(result[i]), procs[i], 0);
		printf("Node %d returned %d\n", i, result[i]);
	}

	/* Get test results */
	{
		FILE* fp;
		char fname[256];

		sprintf(fname, LOGNAME, 0);
		if ((fp = fopen(fname, "r")) != NULL) {
			readReportToStdout(fp, &results);
		} else
			fprintf(logFile, "Can't read client %d's logfile %s\n", 0, fname);
	}
	return 0;
}

/*-------------------------------------------------------------------------
 Entry point
-------------------------------------------------------------------------*/
int						/* Success (0) or failure (nonzero) */
main(
	int argc,			/* number of arguments */
	char *argv[] )		/* value of arguments */
{
	dpiotArgs_t arg;
	logFile = stdout;

	/* Initialize results to no-values-found */
	results.sentSingleSmallTime = -1;
	results.sentSingleLargeTime = -1;
	results.sentMultipleSmallTime = -1;
	results.gotSingleSmallTime = -1;
	results.gotSingleLargeTime = -1;
	results.gotMultipleSmallTime = -1;

	results.singleSmallTime = -1.0f;

	results.thisHost = -1;
	results.n_hosts = -1;
	results.packetLoss = -100;
	results.exe = argv[0];

	/* Initialize arguments with defaults */
	results.loops = 1;
	results.pkts = 1;
	results.pktsize = sizeof(dp_packetType_t);
	arg.bFreeze = TRUE;
	arg.adrbuf = "";
	arg.freeze = "";
	arg.phonenum = "";
	arg.portnum = -1;

	/* Handle arguments */
	handleArguments(&arg, argc, argv);

	if (*arg.freeze) {
		return run_one_node(results.thisHost, arg.freeze, results.pkts, results.pktsize, arg.portnum, arg.phonenum, -1, 1000);
	} else if (results.thisHost < 0 && !(*arg.adrbuf)) {
		printf("Deleting old log and freeze files:\n");
		fflush(stdout);
		system("del dpio*.log");
		system("del dp*.*.log");
		system("del dpio*.frz");
		printf("Done deleting old log files:\n");
		fflush(stdout);

		return run_n_nodes(argv[0], results.loops, results.pkts, results.pktsize, arg.bFreeze);
	} else if (results.thisHost >=0 && (*arg.adrbuf)) {
		int stop = 0;
		if (FALSE == arg.bFreeze) {
			return run_one_node(results.thisHost, arg.adrbuf, results.pkts, results.pktsize, arg.portnum, arg.phonenum, results.loops, 1000);
		} else {
			stop = 3 - results.thisHost;/* + 2; */
			return run_one_node(results.thisHost, arg.adrbuf, results.pkts, results.pktsize, arg.portnum, arg.phonenum, results.loops, stop);
		}
	} else {
		abortTest(logFile, 3, "Invalid argument set not caught by handleArguments\n", NULL);
	}
}
