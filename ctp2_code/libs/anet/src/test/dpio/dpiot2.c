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

/* #define VERBOSE */
/*-----------------------------------------------------------------------
 Unit Test for dpio
-----------------------------------------------------------------------*/

#include <stdio.h>

#include "dp2.h"
#include "../../dp/dpprivy.h"
#include "eclock.h"
#include "dpio.h"

int nextid = 0;

#undef assert
#define assert(expr)							      \
  ((void) ((expr) ? 0 :							      \
	   (assert_fail (__STRING(expr),				      \
			   __FILE__, __LINE__), 0)))

/* This prints an "Assertion failed" message and aborts.  */
static void assert_fail(__const char *__assertion,
				__const char *__file,
				unsigned int __line)
{
	printf("Test failed at line %d of file %s: %s\n",
		__line, __file, __assertion);
	exit(1);
}

/*--------------------------------------------------------------------------
 Structure holding the state of a dpio test object.
--------------------------------------------------------------------------*/
typedef struct {
	/* The dpio being tested, and its support objects */
	dpio_t *dt;
	clock_t dpio_now;
	dpio_t *dpio;
	FILE *logfp;

	/* Which test object this is */
	int id;

	/* id of peer, handle to peer */
	int idPeer;
	playerHdl_t hPeer;

	/* Test duration */
	clock_t t_start;
	clock_t t_end;

	/* Number of packets in test */
	int pkts_desired;
	/* Size of packets for test */
	int pktsize;

	/* Number of packets xferd so far */
	int pkts_rx;
	int pkts_tx;
} dpiot2_t;

/*--------------------------------------------------------------------------
 Given a process id and packet number, fill a buffer with test data of
 length pktsize.
 Test pattern data depends on position in data stream (npkt*pktsize).
--------------------------------------------------------------------------*/
static void testramp_fill(char *buf, int pktsize, int npkt)
{
	int i;

	/* Fill with pattern */
	buf[0] = 'T';
	buf[1] = '2';
	for (i = 2; i < pktsize; i++)
		buf[i] = (char)((npkt*pktsize + i) & 0xff);
}

/*--------------------------------------------------------------------------
 Given a packet number, compare a buffer with the 
 expected test pattern.

 Aborts with given error message if comparison fails.
--------------------------------------------------------------------------*/
static void testramp_compare(char *buf, int pktsize, int npkt, char *msg)
{
	char refbuf[dpio_MAXLEN_RELIABLE];

	if (!buf) {
		printf("%s: null buf\n", msg);
		exit(1);
	}
	testramp_fill(refbuf, pktsize, npkt);
	if (memcmp(buf, refbuf, pktsize)) {
		printf("%s: Bad data\n", msg);
		exit(1);
	}
}

/*--------------------------------------------------------------------------
 Create a dpio test object.
--------------------------------------------------------------------------*/
dpiot2_t *dpiot2_create(int npkts, int pktsize, int appWillFlush)
{
	dpiot2_t *ptest;

	commInitReq_t commInitReq;
	dp_result_t err;
	dp_transport_t dll;
	char logfname[128];

	ptest = malloc(sizeof(dpiot2_t));
	if (!ptest)
		return NULL;
	memset(ptest, 0, sizeof(dpiot2_t));

	ptest->pkts_desired = npkts;
	ptest->pktsize = pktsize;
	ptest->t_start = 0;
	ptest->t_end = 0;

	ptest->id = nextid++;

	sprintf(logfname, "dp%d.log", ptest->id);
	dp_setLogFP(NULL);
	dp_setLogFname(logfname);
	DPRINT(("dpiot2_create: logging started.\n"));
	ptest->logfp = dp_getLogFP();

	/* Create a dpio using the loopback driver */
	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.reqLen = sizeof(commInitReq_t);
	commInitReq.sessionId = ptest->id;	/* my own address */
	commInitReq.portnum = 0;
	ptest->dpio_now = eclock();
	memset(&dll, 0, sizeof(dll));
	strcpy(dll.fname, "wloop2d.dll");
	err = dpio_create(&ptest->dpio, &dll, &commInitReq, &ptest->dpio_now, NULL);
	assert(err == dp_RES_OK);

	/* Set driver to nonblocking mode */
	{
		commSetParamReq_t   req;
		commSetParamResp_t  resp; /* This is a dummy variable */

		memset(&req, 0, sizeof(commSetParamReq_t));
		req.param_num = dp_PARAMID_RXWAIT;
		req.reqLen = sizeof(commSetParamReq_t);
		commSetParam(&req, &resp, ptest->dpio->commPtr);
		assert(resp.status == 0);
	}
	ptest->dpio->appWillFlush = appWillFlush;

	return ptest;
}

/*--------------------------------------------------------------------------
 Connect a dpio test object to its peer.
--------------------------------------------------------------------------*/
void dpiot2_peer_connect(dpiot2_t *ptest, char *peerAdr)
{
	dp_setLogFP(ptest->logfp);

	/* Open a comm handle to the peer's address */
	ptest->hPeer = dpio_openHdl(ptest->dpio, peerAdr, NULL, NULL);
	if (ptest->hPeer == PLAYER_NONE) {
		printf("Unable to connect to peer!\n");
		assert(FALSE);
	}
#ifdef VERBOSE
	printf("Opening handle to peer returns h:%x\n", ptest->hPeer);
#endif
}

/*--------------------------------------------------------------------------
 Set the packet loss (on reception) for this dpiot2.
--------------------------------------------------------------------------*/
void dpiot2_setPktLoss(dpiot2_t *ptest, int percent)
{
	if (!ptest || !ptest->dpio)
		return;
	if (percent < 0)
		dpio_forceRxDropPercent(ptest->dpio, 0);
	else if (percent > 99)
		dpio_forceRxDropPercent(ptest->dpio, 99);
	else
		dpio_forceRxDropPercent(ptest->dpio, percent);
}

/*--------------------------------------------------------------------------
 Call this repeatedly.
 Simulates the network processing routine of a game.
 Keeps sending test packets to peer, and checking packets from peer.
 If send is non-zero, transmit packets, else expect to receive packets.
 Only send if there are more than npktsfree open spots in the send queue.
 Returns FALSE if done, TRUE if it still needs to be called.
--------------------------------------------------------------------------*/
int dpiot2_poll(dpiot2_t *ptest, int send, int npktsfree)
{
	dp_result_t err;
	playerHdl_t src;
	char pkt[512];
	size_t size;

	dp_setLogFP(ptest->logfp);

	ptest->dpio_now = eclock();
	if (send && !ptest->t_start)
		ptest->t_start = ptest->dpio_now;

	/* Get a burst of packets */
	do {
		src = (dpid_t) PLAYER_NONE;	/* kludge */
		size = sizeof(pkt);
		err = dpio_get(ptest->dpio, &src, &pkt, &size, NULL);
		DPRINT(("id %d: dpio_get returns err %d size %d from handle %x\n", ptest->id, err, size, src));
		if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
			printf("dpio_get reports error %d\n", err);
			assert(FALSE);
		}
		if (err == dp_RES_OK) {
			if (!send && !ptest->t_start)
				ptest->t_start = ptest->dpio_now;
#ifdef VERBOSE
			printf("id %d: Got packet %d size %d from handle %x\n", ptest->id, ptest->pkts_rx, size, src);
#endif
			if (size != ptest->pktsize) {
				printf("received packet of unexpected length %d != %d\n", size,
					ptest->pktsize);
				assert(size == ptest->pktsize);
				exit(1);
			}
			testramp_compare(pkt, size, ptest->pkts_rx, "rx");
			ptest->pkts_rx++;
		}
	} while ((err == dp_RES_OK) || (err == dp_RES_AGAIN));

	/* Send a burst of packets, if there's room */
	if (send
	&&  (ptest->pkts_tx < ptest->pkts_desired)
	&&  (dpio_getHdlTxPktsFree(ptest->dpio, ptest->hPeer) > npktsfree)) {
		do {
			testramp_fill(pkt, ptest->pktsize, ptest->pkts_tx);
			err = dpio_put_reliable2(ptest->dpio, &ptest->hPeer, 1, pkt, ptest->pktsize, NULL, dp_SEND_RESERVE_NONE);
			if (err == dp_RES_OK) {
				DPRINT(("id %d: sent packet %d len %d to handle %x\n", ptest->id, ptest->pkts_tx, ptest->pktsize, ptest->hPeer));
#ifdef VERBOSE
				printf("id %d: sent packet %d len %d to handle %x\n", ptest->id, ptest->pkts_tx, ptest->pktsize, ptest->hPeer);
#endif
				ptest->pkts_tx++;
			} else if (err != dp_RES_FULL) {
				printf("id %d: err %d sending to handle %x\n", ptest->id, err, ptest->hPeer);
				exit(1);
			}
		} while ((err == dp_RES_OK) && (ptest->pkts_tx < ptest->pkts_desired));
	}

	/* After the burst of activity is over, send all outstanding buffers */
	err = dpio_update(ptest->dpio);
	if (err != dp_RES_OK && err != dp_RES_EMPTY) {
		printf("dpio_update reports error %d\n", err);
		assert(FALSE);
	}
	dpio_flush(ptest->dpio);

	if (send) {
		if ((ptest->pkts_tx >= ptest->pkts_desired) 
		&&  (dp_RES_BUSY != dpio_ReadyToFreeze(ptest->dpio, NULL))) {
			ptest->t_end = ptest->dpio_now;
			return FALSE;
		}
	} else {
		if ((ptest->pkts_rx >= ptest->pkts_desired) 
		&&  (dp_RES_BUSY != dpio_ReadyToFreeze(ptest->dpio, NULL))) {
			ptest->t_end = ptest->dpio_now;
			return FALSE;
		}
	}
	return TRUE;
}

/*--------------------------------------------------------------------------
 Destroy a dpio test object.  Does a few final checks.
--------------------------------------------------------------------------*/
void dpiot2_destroy(dpiot2_t *ptest)
{
#ifdef VERBOSE
	printf("dpiot2_destroy: entry\n");
#endif
	dp_setLogFP(ptest->logfp);
	dpio_destroy(ptest->dpio, 0);
#ifdef VERBOSE
	printf("dpiot2_destroy: done\n");
#endif
}

/*-----------------------------------------------------------------------
 Unit test main loop.  Creates two test objects, hooks them together,
 and polls them until they cry uncle.  Any failure causes the 
 subroutines to terminate the program with an error message.
-----------------------------------------------------------------------*/
main(int argc, char **argv)
{
	int i;
	int running1;
	int running2;
	int pktsize;
	int total;
	int npkts;
	int npktsfree;
	int appWillFlush;
	long tx_time[5];
	long rx_time[5];
	float txpkts_per_sec[5];
	float rxpkts_per_sec[5];

	if (argc <= 3) {
		printf("Usage: %s <pktsize> <total> <sendThreshold> [appWillFlush]\n\
Exchanges <total> bytes worth of packets of size <pktsize> reliably\n\
between two dpio's at several packet loss levels and reports statistics.\n\
Sender will send when <sendThreshold> open slots are available in the \n\
outgoing queue.\n\
If <appWillFlush> is present and 0, test will not use packet gathering.\n",
			argv[0]);
		exit(1);
	}
	pktsize = atoi(argv[1]);
	total = atoi(argv[2]);
	npktsfree = atoi(argv[3]);
	if (argc > 4) {
		appWillFlush = atoi(argv[3]);
		if (appWillFlush)
			appWillFlush = TRUE;
		else
			appWillFlush = FALSE;
	} else
		appWillFlush = TRUE;
	if ((pktsize < 1) || (pktsize > dpio_MAXLEN_RELIABLE)) {
		printf("Usage: %s <pktsize> <total>\n\
	<pktsize> must be between 1 and %d\n",
			argv[0], dpio_MAXLEN_RELIABLE);
		exit(1);
	}
	if (total < 1) {
		printf("Usage: %s <pktsize> <total>\n\
	<total> must be 1 and %d\n",
			argv[0], INT_MAX);
		exit(1);
	}
	npkts = (int)(total + pktsize - 1)/pktsize;

	for (i = 0; i < 5; i++) {
		dpiot2_t *t1, *t2;

		printf("Run %d: %d%% packet loss.\n", i, i*5);

		t1 = dpiot2_create(npkts, pktsize, appWillFlush);
		t2 = dpiot2_create(npkts, pktsize, appWillFlush);
		dpiot2_setPktLoss(t1, i*5);
		dpiot2_setPktLoss(t2, i*5);
		assert(t1 && t2);
		dpiot2_peer_connect(t1, t2->dpio->myAdr);
		t1->idPeer = t2->id;
		dpiot2_peer_connect(t2, t1->dpio->myAdr);
		t2->idPeer = t1->id;

		do {
			running1 = dpiot2_poll(t1, 1, npktsfree);	/* send */
			running2 = dpiot2_poll(t2, 0, 0);			/* recv */
		} while (running1 || running2);

		tx_time[i] = t1->t_end - t1->t_start;
		if (tx_time[i] <= 0)
			txpkts_per_sec[i] = (float)99999.99;
		else
			txpkts_per_sec[i] = (float)npkts * ECLOCKS_PER_SEC / tx_time[i];
		rx_time[i] = t2->t_end - t2->t_start;
		if (rx_time[i] <= 0)
			rxpkts_per_sec[i] = (float)99999.99;
		else
			rxpkts_per_sec[i] = (float)npkts * ECLOCKS_PER_SEC / rx_time[i];
		dpiot2_destroy(t1);
		dpiot2_destroy(t2);
	}
	printf("No test failed.\n");
	printf("Sent %d bytes as %d packets of %d bytes (gathering %s)\n",
		npkts*pktsize, npkts, pktsize,
		(appWillFlush ? "on" : "off"));
	printf("loss (RT) txtime txrate              rxtime rxrate\n");
	for (i = 0; i < 5; i++) {
		int pktloss = i*5;
		int rt_pktloss = 100 - (100 - pktloss)*(100 - pktloss)/100;
		printf(" %02d%% %02d%% %5.2fs %7.2fp/s %5dB/s %5.2fs %7.2fp/s %5dB/s\n",
			pktloss, rt_pktloss, (float)tx_time[i]/ECLOCKS_PER_SEC,
			txpkts_per_sec[i], (int)(txpkts_per_sec[i]*pktsize),
			(float)rx_time[i]/ECLOCKS_PER_SEC, rxpkts_per_sec[i],
			(int)(rxpkts_per_sec[i]*pktsize));
	}
	return 0;
}
