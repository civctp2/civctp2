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
 Test of table propagation

 Usage: dptabt driver n
 or:    dptabt driver n hostnum nextAddress

 This test program measures the performance of the dptab module.
 It measures the time it takes various things to propagate around a ring
 of n hosts.  The things sent around the ring include:
 * a single small (four-byte) table entry
 * a single table entry deletion
 * a block of 10 small (four-byte) table entries
 * a single large (10kbyte) table entry
 The results are reported in seconds per hop for the small entries,
 and bytes per second for the large entries.

 The first form spawns n processes using the second form, and waits for
 them all to finish.  I.e. for i=0..n, it executes
    dptabt driver i ((i+1 mod n) + 1).0.0.0
 For example, if driver is wloop.dll, and N == 2, it executes:
	dptabt wloop.dll 2 0 2.0.0.0
	dptabt wloop.dll 2 1 1.0.0.0
 If N == 4, it executes:
	dptabt wloop.dll 2 0 2.0.0.0
	dptabt wloop.dll 2 1 3.0.0.0
	dptabt wloop.dll 2 2 4.0.0.0
	dptabt wloop.dll 2 3 1.0.0.0

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
 run_one_test() for details.

 $Log: dptabt.c $
 Revision 1.6  1997/09/23 02:50:17  lbennett
 Moved freeze/thaw to exercise xfer code.
 Revision 1.5  1997/09/22 02:33:40  lbennett
 Added freeze/thaw test, revision log.
--------------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if (defined WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include "eclock.h"
#include "..\..\..\demo\utils\unistd.h"
#include "dp2.h"
#include "dptab.h"

#if 0
#define USE_MY_ASSERT 1
#define assert(x) myassert((int)(x), __LINE__)
#else
#include <assert.h>
#endif

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows *linked* code has six of these ) */
#if (defined WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/**
* Constants
*/

#define RECEIVE_FLAGS	0
#define BUFSIZE			40000
#define TIMEOUT_DELAY	90000
#define PORT_OFFSET		15904

#define LOGNAME "dptab%d.log"
#define FREEZENAME "dptabt%d.frz"

#define SUBKEY_SINGLE_SMALL   10
#define SUBKEY_SINGLE_LARGE   11
#define SUBKEY_MULTIPLE_SMALL 12

/* Packet types */
#define PKT_SU dppt_MAKE('S', 'U')	/* Startup */
#define PKT_IT dppt_MAKE('I', 'T')	/* Iteration */
#define PKT_QQ dppt_MAKE('Q', 'Q')	/* Quit */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

/**
* Types
*/

/* Structure to hold the results of the test */
typedef struct tabtResult_s {
	/* When test item is sent out from the master */
	clock_t sentSingleSmallTime;
	clock_t sentDeleteTime;
	clock_t sentMultipleSmallTime;
	clock_t sentSingleLargeTime;
	/* When test item comes back around the ring to the master */
	clock_t gotSingleSmallTime;
	clock_t gotDeleteTime;
	clock_t gotMultipleSmallTime;
	clock_t gotSingleLargeTime;

	/* Final computed values */
	float singleSmallTime;
	float deleteTime;

	int thisHost;		/* 0..n_hosts-1; if 0, this host is master */

	int n_hosts;
	int loops;

	char *driver;
	char *exe;

	int packetLoss;
} tabtResult_t;

/**
* Global variables
*/

tabtResult_t results;	// Must be global for signal handler
FILE* logFile;			// Must be global for signal handler
char buf[BUFSIZE];
char usage[1024];
playerHdl_t qq_src;		/* Who sent us a QQ packet */
playerHdl_t su_src;
playerHdl_t it_src;
playerHdl_t it_num;
clock_t dpio_now;

/**
* Methods
*/

/*-------------------------------------------------------------------------
 Convert a key to ASCII for debug printing
-------------------------------------------------------------------------*/
static char *key2buf(char *key, int keylen, char *buf)
{
	int i;

	if (keylen > hkeytab_MAXLEN)
		return "key too long";

	buf[0] = 0;
	for (i=0; i<keylen; i++) {
		//DPRINT(("key[%d] %d\n", i, key[i]));
		sprintf(buf + strlen(buf), "%d%s", 255 & key[i], (i == (keylen-1)) ? "" : ".");
	}
	return buf;
}

static char key2a_buf[256];
static char key2a_buf2[256];

/*-------------------------------------------------------------------------
 Convert a key to ASCII for debug printing
-------------------------------------------------------------------------*/
/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)

/*-------------------------------------------------------------------------
 Write the results to the log file
-------------------------------------------------------------------------*/
void
writeReport(
	FILE* log,
	tabtResult_t* res)
{
	printf("Node %d: Writing report; gotSingleSmallTime is %f\n",
			res->thisHost, results.singleSmallTime);
	if (res->thisHost < 1) {
		fprintf(log, "Table Ring Test\nhosts %d\nDriver %s\npktloss %d%%\n",
			res->n_hosts, res->driver, res->packetLoss);

		if (res->singleSmallTime == -1) {
			fprintf(log, "Failure: set did not propagate\n");
			return;
		}
#if 0
		if (res->deleteTime == -1) {
			fprintf(log, "Failure: delete did not propagate\n");
			return;
		}
#endif

		fprintf(log, "singleSmallTime %f seconds (%f seconds/hop)\n",
				res->singleSmallTime, res->singleSmallTime / res->n_hosts);

#if 0
		fprintf(log, "deleteTime %f seconds (%f seconds/hop)\n",
				res->deleteTime, res->deleteTime / res->n_hosts);
#endif
	}
}

/*-------------------------------------------------------------------------
 Read the master's results from the log file
-------------------------------------------------------------------------*/
void
readReport(
	FILE* log,
	tabtResult_t* res)
{
	char test[128];
	float val;
	while (fgets(buf, BUFSIZE, log) != NULL) {
		if (sscanf(buf, "%s %f", test, &val) == 2) {
			if (strcmpi(test, "singleSmallTime") == 0)
				res->singleSmallTime = val;
			if (strcmpi(test, "pktloss") == 0)
				res->packetLoss = (int)val;
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
	tabtResult_t* res)
{
	fflush(log);
	fputs(message, log);
	if(res != NULL)
		writeReport(log, res);
	fclose(log);
	exit(exitCode);
}

/*-------------------------------------------------------------------------
 Stop the test if something went wrong that we thought would never happen.
 Useful when assertions print and exit, instead of going to a debugger.
-------------------------------------------------------------------------*/
#if USE_MY_ASSERT
void myassert(int x, int lineno)
{
	if (!(x)) {
		char buf[256];
		sprintf(buf, "Node %d: Assertion failed, line %d\n", results.thisHost, lineno);
		printf("%s", buf);
		abortTest(logFile, 9, buf, NULL);
	}
}
#endif

/*-------------------------------------------------------------------------
 Timeout
-------------------------------------------------------------------------*/
int timer_handler(int sigtype)
{
	(void) sigtype;

	fflush(stdout);
	printf("\nNode %d: Timeout\n", results.thisHost);
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
	(void) sigId;
	abortTest(logFile, 4, "Test terminated at user request.\n", &results);
}

/*-------------------------------------------------------------------------
 This is called by the dptab_handlePacket when a new variable arrives or
 an old one is deleted.
 Note the time of the callback in the result block.
-------------------------------------------------------------------------*/
int dp_PASCAL
table_cb(
	dptab_t *dptab,			/* table owner */
	dptab_table_t *table,	/* table */
	playerHdl_t src,		/* */
	playerHdl_t dest,		/* */
	char *subkey,			/* key of variable */
	int subkeylen,			/* number of bytes in subkey */
	void *buf,				/* pointer to variable */
	size_t sent,
	size_t total,
	int seconds_left,
	void *context,			/* whatever we gave it */
	dp_result_t err)		/* operation completed upon variable */
{
	clock_t now = eclock();
	printf("Node %d: table_cb: type:%d  src h:%x  dest h:%x  table:%s  key:%s\n",
				results.thisHost, err, src, dest, key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen));
	if (err == dp_RES_CREATED || err == dp_RES_CHANGED) {
		/*printf("table_cb: CREATED/CHANGED: src h:%x  dest h:%x  table:%s  key:%s\n",
				src, dest, key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen));*/
		if ((dest == PLAYER_ME) && (sent == total)) {
			/* Got a variable */
			switch (subkey[0]) {
			case SUBKEY_SINGLE_SMALL:
				if(table->key[0] == 2) {
					results.gotSingleSmallTime = now;
					results.singleSmallTime = ((float)now - results.sentSingleSmallTime) / ECLOCKS_PER_SEC;
					/* Move this statement to right after last measurement taken */
					writeReport(logFile, &results);
				}
				break;
			case SUBKEY_SINGLE_LARGE:
				results.gotSingleLargeTime = now;
				break;
			case SUBKEY_MULTIPLE_SMALL:
				results.gotMultipleSmallTime = now;
				break;
			default:
				;
			}
			printf("Node %d: Got variable from h:%x; table %s, subkey %s; len %d\n",
				results.thisHost,
				src,
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				total);
		}
	} else if (err == dp_RES_CLOSED) {
		/*printf("table_cb: Deleted: src h:%x  dest h:%x\n", src, dest);*/
		if (dest == PLAYER_ME) {
			switch (subkey[0]) {
			case SUBKEY_SINGLE_SMALL:
				results.gotDeleteTime = now;
				results.deleteTime = ((float)now - results.sentDeleteTime) / ECLOCKS_PER_SEC;
				break;
			default:
				;
			}
			printf("Node %d: Got delete variable from h:%x; table %s, subkey %s\n",
				results.thisHost,
				src,
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen));
		}
	}

	(void) dptab;
	(void) buf;
	(void) seconds_left;
	(void) context;
	return TRUE;
}

/*-------------------------------------------------------------------------
 Check for network packets and handle them.
 Sets globals qq_src and su_src if/when qq or su packet received.
-------------------------------------------------------------------------*/
void poll_test(dpio_t *dpio, dptab_t *tab)
{
	dp_result_t err;
	playerHdl_t src;
	size_t size;
	char pkt[dpio_MAXLEN_UNRELIABLE];

	err = dptab_update(tab);
	assert(err == dp_RES_OK || err == dp_RES_EMPTY);

	dpio_now = eclock();
	err = dpio_update(dpio);
	if (err != dp_RES_OK && err != dp_RES_EMPTY) {
		printf("dpio_update reports error %d\n", err);
		assert(FALSE);
	}
	/*printf("%1d", results.thisHost);*/
	//src = (dpid_t) PLAYER_NONE;	/* kludge */
	size = sizeof(pkt);
	err = dpio_get(dpio, &src, &pkt, &size, NULL);
	if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
		printf("dpio_get reports error %d\n", err);
		assert(FALSE);
	}
	if (err == dp_RES_OK) {
		/*printf("Got packet from handle %x\n", src);*/
		switch (*(dp_packetType_t *)pkt) {
			case PKT_IT:
				it_src = src;
				it_num = pkt[sizeof(dp_packetType_t)];
				printf("Node %d: Got it=%d packet from source h:%x\n", results.thisHost, it_num, src);
				break;
			case PKT_SU:
				su_src = src;
				printf("Node %d: Got su packet from source h:%x\n", results.thisHost, src);
				break;
			case PKT_QQ:
				qq_src = src;
				printf("Got qq packet from source h:%x\n", src);
				break;
			default:
				err = dptab_handlePacket(tab, src, size, pkt);
				if (err != dp_RES_OK && err != dp_RES_EMPTY) {
					printf("dptab_handlePacket reports error %d\n", err);
					fflush(stdout);
					assert(FALSE);
				}
				break;
		}
	}
}

/*-------------------------------------------------------------------------
 Run a single copy of the test.
 Return value is 0 on success.

 If childnum is > 0, this is a slave; otherwise, this is the master.

 Does the following steps:
 1. Initialize dpio and dptab
 2. Create a table
 3. Establish connections.
	a. Open a handle to address and subscribe the table from that peer.
	   Master subscribes as table2, all else as standard table.
	b. Send an "SU" packet to address requesting peer to publish the table to us
 4. Wait for a "SU" packet.
 5. publish the table to whoever sent "SU" to us.
 Loop appropriate number of times, doing:
	 6. Master sends an "IT" packet to next. On receipt of an "IT" packet,
		slaves send an "IT" packet to next. Master waits for "IT" packet.
	    Watch for iteration number in the IT packet.  If this is the time,
		freeze the dpio and dptab, spawn a replacement, and exit using the
		replacement's exit code.
	 7. If childnum == 0, this child is the master:
		c. Set a value in the table.  Use a max hop count of N.
		d. Wait until value appears in table2 (watch the callback).
		e. Print out how long it took to get there.
 Once the correct number of loops is done,
 8. If childnum == 0: Send a QQ packet to next, wait for QQ packet.
    If childnum > 0: Wait until QQ packet recieved, send QQ to next.
 9. Exit.

 N must be 2 or greater.

 For example:

 If N == 2, the launcher executes:
	dptabt wloop.dll 0 2.0.0.0          (host 1 = master)
	dptabt wloop.dll 1 1.0.0.0          (host 2 = slave)
 the sequence of events is
    host step action
    h1   #4  waits for SU
	h2   #3  subscribes to 1 from h1, sends "SU" to h1, waits for "sU"
	h1   #5  Gets "SU", publishes table 1 to h2
	h1   #6  subscribe's h2's table 1 onto table 2, sends "SU" to h2
	h2   #5  Gets "SU", publishes table 1 to h1
 and the variable's trip around the ring looks like this:
    master -> slave -> master again
 The only reason it doesn't continue on to the slave is that the
 max hop count was set to 2.
 (Interestingly, if another host connects to the master's table,
  whether or not they get a copy of the variable depends on whether
  they connect before the variable comes around again (yes), or after (no).
  This is a good reason to never use rings like this in real systems!)

 If N == 4, the launcher executes:
	dptabt wloop.dll 0 2.0.0.0          (host 1 = master)
	dptabt wloop.dll 1 3.0.0.0          (host 2 = slave)
	dptabt wloop.dll 2 4.0.0.0          (host 3 = slave)
	dptabt wloop.dll 3 1.0.0.0          (host 4 = slave)
 the sequence of events is
    host step action
    h1   #4  waits for SU
	h2   #3  subscribes to 1 from next, sends "SU" to next, waits for "SU"
	h3   #3  subscribes to 1 from next, sends "SU" to next, waits for "SU"
	h4   #3  subscribes to 1 from next, sends "SU" to next, waits for "SU"
	h1   #5  Gets "SU" from prev, publishes table 1 to prev
	h1   #6  subscribe's next's table 1 onto table 2, sends "SU" to next
	h2   #5  Gets "SU" from prev, publishes table 1 to prev
	h3   #5  Gets "SU" from prev, publishes table 1 to prev
	h4   #5  Gets "SU" from prev, publishes table 1 to prev
 and the variable's trip around the ring looks like this:
    h1 -> h2 -> h3 -> h4 -> h1 again
-------------------------------------------------------------------------*/
int
run_one_node(
	int childNum,
	char *sNextAdr,
	int loopTotal,
	int endLoopAt)
{
	dptab_t *tab;
	dp_result_t err;
	char key[10];
	char subkey[10];
	dptab_table_t *table;
	dptab_table_t *table2;
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
	int startLoopAt = 0;
	int i;

	/* Set a timeout of 30 seconds */
	signal(SIGTIMER, timer_handler);
	alarm(30);

	results.thisHost = childNum;

	sprintf(fname, LOGNAME, childNum);
	if(loopTotal < 1) {	/* thawing */
		assert ((logFile = fopen(fname, "a")) != NULL);
		fprintf(logFile, "--Thawing--\n");
	} else
		assert ((logFile = fopen(fname, "w")) != NULL);
	#ifdef WIN32
		srand(GetTickCount());
	#endif

	/* 1. Initialize dpio and dptab */
	dpio_now = eclock();
	if(loopTotal < 1) {	/* thawing */
		/* Find our file */
		FILE* thawFile;
		sprintf(fname, FREEZENAME, childNum);
		printf("Node %d: Thawing from file %s.\n", childNum, fname);
		thawFile = fopen(fname, "r");
		assert (thawFile != NULL);

		/* Read everything from our file */
		fread(&startLoopAt,sizeof(int),1,thawFile);
		fread(&loopTotal,sizeof(int),1,thawFile);
		fread(&it_num,sizeof(int),1,thawFile);
		fread(&(results.n_hosts),sizeof(int),1,thawFile);
		sprintf(dplogname, "dpt%d.%d.log", childNum, startLoopAt);
		dp_setLogFname(dplogname);

		err = dpio_create(&dpio, NULL, NULL, &dpio_now, thawFile);
		assert(err == dp_RES_OK);
		err = dpio_thawHdl(dpio, &dest, thawFile);
		assert(err == dp_RES_OK);

		tab = dptab_create(dpio);
		assert(tab);
		err = dptab_thaw(tab, thawFile);
		assert(err == dp_RES_OK);
		fclose(thawFile);

		/* Re-set any callbacks */
		key[0] = 1;
		table = dptab_getTable(tab, key, 1);
		assert(table != NULL);
		err = dptab_setTableCallback(table, table_cb, NULL);
		assert(err == dp_RES_OK);
		if(childNum == 0) {
			key[0] = 2;
			table2 = dptab_getTable(tab, key, 1);
			assert(table != NULL);
			err = dptab_setTableCallback(table2, table_cb, NULL);
			assert(err == dp_RES_OK);
		}
	} else {	/* not thawing */
		sprintf(dplogname, "dpt%d.%d.log", childNum,startLoopAt);
		dp_setLogFname(dplogname);
		printf("Node %d step 1\n", childNum);

		/* Create our dpio */
		memset(&dll, 0, sizeof(dll));
		strcpy(dll.fname, results.driver);
		memset(&commInitReq, 0, sizeof(commInitReq));
		commInitReq.sessionId = childNum + 1;			/* claim our address */
		commInitReq.portnum = childNum + PORT_OFFSET;	/* claim our port */
		commInitReq.reqLen = sizeof(commInitReq_t);
		err = dpio_create(&dpio, &dll, &commInitReq, &dpio_now, NULL);
		assert(err == dp_RES_OK);

		/* Find our next-in-ring */
		scanReq.printable = sNextAdr;
		scanReq.address = adrBuf;
		scanReq.size = sizeof(adrBuf);
		if (!commScanAddr(&scanReq, &scanResp)) {
			printf("Unable to scan next host address %s, err: %d",
					scanReq.printable, scanResp.status);
			assert(FALSE);
		}
		results.packetLoss = dpio->rxDropPercent;

		/* Create our table collection (dptab) */
		tab = dptab_create(dpio);
		assert(tab);

		/* 2. Create tables */
		printf("Node %d step 2\n", childNum);
		key[0] = 1;
		err = dptab_createTable(tab, &table, key, 1, sizeof(int), NULL, NULL, table_cb, NULL);
		assert(err == dp_RES_OK);
		if(childNum == 0) {
			key[0] = 2;
			err = dptab_createTable(tab, &table2, key, 1, sizeof(int), NULL, NULL, table_cb, NULL);
			assert(err == dp_RES_OK);
		}

		/* 3. Establish connections */
		printf("Node %d step 3\n", childNum);
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

		/* Add the next machine to the party */
		err = dptab_addPeer(tab, dest);
		/* only master should have received packet from unknown source. */
		if ((childNum == 0) && (su_src == dest))
			assert(err == dp_RES_ALREADY);
		else
			assert(err == dp_RES_OK);

		/* Let the next machine publish on top of our table */
		key[0] = 1;
		if(childNum == 0)
			err = dptab_addPublisher(tab, table2, key, 1, dest);
		else
			err = dptab_addPublisher(tab, table, key, 1, dest);
		assert(err == dp_RES_OK);

		/* Request the next machine publish table to us */
		(*(dp_packetType_t *)nbuf) = PKT_SU;
		err = dpio_put_reliable(dpio, &dest, 1, nbuf, sizeof(dp_packetType_t)+2, NULL);
		assert(err == dp_RES_OK);
		printf("Node %d sent SU to h:%x\n", childNum, dest);

		/* 4. Wait for an "SU" packet. */
		printf("Node %d step 4: waiting for su.\n", childNum);
		su_src = PLAYER_NONE;
		do {
			poll_test(dpio, tab);
		} while (su_src == PLAYER_NONE);

		/* 5. publish the table to whoever sent "SU" to us. */
		printf("Node %d step 5\n", childNum);
		err = dptab_addPeer(tab, su_src);
		if (su_src == dest)
			assert(err == dp_RES_ALREADY);
		else
			assert(err == dp_RES_OK);
		err = dptab_addSubscriber(tab, table, su_src);
		assert(err == dp_RES_OK);
	}	/* not thawing */

	for(i = startLoopAt; i < loopTotal; i++) {
		/* 6. Send an IT packet around the ring. */
		printf("Node %d step 6.%d\n", childNum, i);
		if(childNum == 0) {	/* Master sends first IT. */
			(*(dp_packetType_t *)nbuf) = PKT_IT;
			nbuf[sizeof(dp_packetType_t)] = (char) (i & 0x7F);
			err = dpio_put_reliable(dpio, &dest, 1, nbuf,
						sizeof(dp_packetType_t)+sizeof(char), NULL);
			assert(err == dp_RES_OK);
		}
		printf("Node %d waiting for IT(%d).\n", childNum, i);
		do {				/* Wait for IT */
			poll_test(dpio, tab);
		} while (((int) it_num) < (i & 0x7F));
		if(childNum != 0) {	/* Slaves send IT after getting one. */
			(*(dp_packetType_t *)nbuf) = PKT_IT;
			nbuf[sizeof(dp_packetType_t)] = (char) i;
			err = dpio_put_reliable(dpio, &dest, 1, nbuf,
						sizeof(dp_packetType_t)+sizeof(char), NULL);
			assert(err == dp_RES_OK);
		}

		/* 7. If master, set a value and wait for it to come back around. */
		if(childNum == 0) {
			printf("Node %d step 7\n", childNum);
			subkey[0] = SUBKEY_SINGLE_SMALL;
			sprintf(nbuf, "a%03d", i);
			printf("Node %d: setting variable to %s\n", childNum, nbuf);
			results.sentSingleSmallTime = dpio_now;
			err = dptab_set(tab, table, subkey, 1, nbuf, strlen(nbuf), results.n_hosts, PLAYER_ME);
			if (err != dp_RES_OK) {
				printf("Node %d: dptab_set returns err %d!\n", childNum, err);
				assert(err == dp_RES_OK);
			}
		}

		/* Freeze and spawn if it's the right iteration. */
		/* Do it here to force an xfer to be saved and restored. */
		if(i == endLoopAt) {
			FILE* freezeFile;
			int result;
			int proc;

			/* Find our file */
			sprintf(fname, FREEZENAME, childNum);
			printf("Node %d: Iteration %d, Freezing to file %s.\n", childNum, i, fname);
			freezeFile = fopen(fname, "w");
			assert (freezeFile != NULL);

			/* Write everything to our file */
			fwrite(&i,sizeof(int),1,freezeFile);
			fwrite(&loopTotal,sizeof(int),1,freezeFile);
			fwrite(&it_num,sizeof(int),1,freezeFile);
			fwrite(&(results.n_hosts),sizeof(int),1,freezeFile);

			err = dpio_freeze(dpio, freezeFile);
			assert(err == dp_RES_OK);
			err = dpio_freezeHdl(dpio, dest, freezeFile);
			assert(err == dp_RES_OK);

			err = dptab_freeze(tab, freezeFile);
			assert(err == dp_RES_OK);
			fclose(freezeFile);
			dptab_destroy(tab);
			dpio_destroy(dpio, 1);

			/* Spawn and return child's return code */
			sprintf(buf, "%d", childNum);
			proc = _spawnl(_P_NOWAIT, results.exe, results.exe, buf, fname, NULL);
			_cwait(&result, proc, 0);
			printf("Node %d.%d returned %d.\n", childNum, i, result);
			return result;
		}

		if(childNum == 0) {
			/* d. Wait until value appears in table2 (callback). */
			printf("Node %d waiting for value to appear.\n", childNum);
			results.gotSingleSmallTime = -1;
			do {
				poll_test(dpio, tab);
			} while (results.gotSingleSmallTime == -1);
		}
	}

	/* 8. Send a QQ packet around the ring. */
	printf("Node %d step 8\n", childNum);
	qq_src = PLAYER_NONE;
	if(childNum == 0) {	/* Master sends first QQ */
		(*(dp_packetType_t *)nbuf) = PKT_QQ;
		err = dpio_put_reliable(dpio, &dest, 1, nbuf, sizeof(dp_packetType_t), NULL);
		assert(err == dp_RES_OK);
	}
	/* Wait for QQ */
	printf("Node %d waiting for QQ.\n", childNum);
	do {
		poll_test(dpio, tab);
	} while (qq_src == PLAYER_NONE);
	if(childNum != 0) {	/* Slaves wait for a QQ and then send QQ */
		(*(dp_packetType_t *)nbuf) = PKT_QQ;
		err = dpio_put_reliable(dpio, &dest, 1, nbuf, sizeof(dp_packetType_t), NULL);
		assert(err == dp_RES_OK);
	}

	/* 9. Exit. */
	printf("Node %d step 9\n", childNum);
	/* Ack packets for four more seconds just in case */
	{
		clock_t start = eclock();
		while ((long)(eclock() - start) < 4 * ECLOCKS_PER_SEC) {
			dpio_now = eclock();
			dpio_update(dpio);
		}
	}
	printf("Node %d exiting\n", childNum);
	return 0;
}

/*-------------------------------------------------------------------------
 Run n copies of the test.
 Return value is 0 on success.
-------------------------------------------------------------------------*/
int					/* */
run_n_nodes(
	char *exe,		/* */
	int loops)		/* */
{
	char *childArgs[7] = {NULL, NULL, "nhosts", "nloops", "childnum", "nextadr.possibly.internet.address:maybeWithPortNumber", NULL};
	int* procs = NULL;
	int* result = NULL;
	int i;

	childArgs[0] = exe;
	childArgs[1] = results.driver;

	if ((procs = (int *) malloc(results.n_hosts * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate process handle storage.\n", NULL);
	if((result = (int *) malloc(results.n_hosts * sizeof(int))) == NULL)
		abortTest(logFile, 3, "Unable to allocate result storage.\n", NULL);

	/* Start tests */
	for (i=0; i < results.n_hosts; i++) {
		sprintf(childArgs[2], "%d", results.n_hosts);
		sprintf(childArgs[3], "%d", loops);
		sprintf(childArgs[4], "%d", i);
		#if LOOPBACK_ADDRESS
			sprintf(childArgs[5], "%d.0.0.0", ((i+1) % results.n_hosts) + 1);
		#elif INTERNET_ADDRESS
			sprintf(childArgs[5], "207.82.188.88:%d", ((i+1) % results.n_hosts) + PORT_OFFSET);
		#else
			Warning: Must choose one of the above defines.
		#endif
		printf("Launching %s %s %s %s %s %s\n",
				childArgs[0],
				childArgs[1],
				childArgs[2],
				childArgs[3],
				childArgs[4],
				childArgs[5]);
		procs[i] = _spawnv(_P_NOWAIT, exe, childArgs);
		if(procs[i] == -1) {
			sprintf(buf, "Unable to start process %d: %s\n", i, strerror(errno));
			abortTest(logFile, 4, buf, NULL);
		}
	}

	/* Wait for tests to finish */
	for (i=0; i < results.n_hosts; i++) {
		_cwait(&(result[i]), procs[i], 0);
		printf("Node %d returned %d\n", i, result[i]);
	}

	/* Get test results */
	{
		FILE* fp;
		char fname[256];

		sprintf(fname, LOGNAME, 0);
		if ((fp = fopen(fname, "r")) != NULL) {
			readReport(fp, &results);
		} else
			fprintf(logFile, "Can't read client %d's logfile %s\n", 0, fname);
	}
	writeReport(logFile, &results);
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
	logFile = stdout;
	sprintf(usage, "Usage: %s driver nhosts nloops\n\
or: %s driver nhosts nloops hostnum nextadr \n\
or: %s hostnum filename \n\
First form launches n copies of second form;\n\
second form is the part of the test that runs on each computer;\n\
third form is a second form restoring from disk.\n\
Hostnum is 0..nhosts-1\n",
		argv[0], argv[0], argv[0]);

	/* Initialize results to no-values-found */
	results.sentSingleSmallTime = -1;
	results.sentSingleLargeTime = -1;
	results.sentMultipleSmallTime = -1;
	results.sentDeleteTime = -1;
	results.gotSingleSmallTime = -1;
	results.gotSingleLargeTime = -1;
	results.gotMultipleSmallTime = -1;
	results.gotDeleteTime = -1;

	results.singleSmallTime = -1.0f;
	results.deleteTime = -1.0f;

	results.thisHost = -1;
	results.n_hosts = -1;
	results.packetLoss = -100;
	results.exe = argv[0];

	/* Handle arguments */
	if ((argc != 3) && (argc != 4) && (argc != 6)) {
		sprintf(buf, "Invalid number of arguments (%d)\n%s", argc, usage);
		abortTest(logFile, 3, buf, NULL);
		return 1;
	}

	if(argc != 3) {
		results.driver = argv[1];

		results.n_hosts = atoi(argv[2]);
		if (results.n_hosts < 2 || results.n_hosts > 200) {
			sprintf(buf, "Invalid number of hosts (%s)\n%s", argv[2], usage);
			abortTest(logFile, 3, buf, NULL);
		}

		results.loops = atoi(argv[3]);
		if (results.loops < 1 || results.loops > 127) {
			sprintf(buf, "Invalid number of hosts (%s)\n%s", argv[3], usage);
			abortTest(logFile, 3, buf, NULL);
		}
	}

	if (argc == 3) {
		results.thisHost = atoi(argv[1]);
		if (results.thisHost < 0 || results.thisHost > 200) {
			sprintf(buf, "Invalid childnum (%s)\n%s", argv[1], usage);
			abortTest(logFile, 3, buf, NULL);
		}
		return run_one_node(results.thisHost, argv[2], -1, 1000);
	} else if (argc == 4) {
		printf("Deleting old log files:\n");
		fflush(stdout);
		system("del dptab*.log");
		printf("Done deleting old log files:\n");
		fflush(stdout);

		return run_n_nodes(argv[0], results.loops);
	} else {
		int stop = 0;
		results.thisHost = atoi(argv[4]);
		if (results.thisHost < 0 || results.thisHost > 200) {
			sprintf(buf, "Invalid childnum (%s)\n%s", argv[4], usage);
			abortTest(logFile, 3, buf, NULL);
		}
		stop = 2 + 2*results.thisHost;
		return run_one_node(results.thisHost, argv[5], results.loops, stop);
	}
}
