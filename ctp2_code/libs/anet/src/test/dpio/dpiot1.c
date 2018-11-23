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

/*-----------------------------------------------------------------------
 Unit Test for dpio
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

#include "dp2.h"
#include "../../dp/dpprivy.h"
#include "eclock.h"
#include "dpio.h"

int nextid = 0;

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

	dp_result_t dpio_status;

	/* Number of packets in test */
	int pkts_desired;

	/* Number of packets xferd so far */
	int pkts_rx;
	int pkts_tx;
} dpiot1_t;

/*--------------------------------------------------------------------------
 Given a process id and packet number, fill a buffer with the test data.
 Returns length of buffer.

 Test patterns length depends on npkt.
 Test pattern data depends on id and npkt.
--------------------------------------------------------------------------*/
static int testramp_fill(int id, int npkt, char *buf)
{
	int len;
	int i;
	char xor;
	len = 2 + (npkt % (dpio_MAXLEN_RELIABLE-1));

	/* Fill with pattern */
	xor = (char)(id << 7);
	buf[0] = 't';
	buf[1] = npkt | 0x20;
	for (i=2; i<len; i++)
		buf[i] = (char)((i & 255) ^ xor);

	return len;
}

/*--------------------------------------------------------------------------
 Given a process id and packet number, compare a buffer with the
 expected test pattern.

 Aborts with given error message if comparison fails.
--------------------------------------------------------------------------*/
static void testramp_compare(int id, int npkt, char *buf, int buflen, char *msg)
{
	char refbuf[dpio_MAXLEN_RELIABLE];
	int reflen;

	if (!buf) {
		printf("%s: null buf\n", msg);
		exit(1);
	}
	reflen = testramp_fill(id, npkt, refbuf);
	if (reflen != buflen) {
		DPRINT(("%s: id %d, npkt %d; Bad length %d, expected %d\n", msg, id, npkt, buflen, reflen));
		printf("%s: id %d, npkt %d; Bad length %d, expected %d\n", msg, id, npkt, buflen, reflen);
		assert(reflen == buflen);
		exit(1);
	}
	if (memcmp(buf, refbuf, buflen)) {
		printf("%s: Bad data\n", msg);
		exit(1);
	}
}

/*--------------------------------------------------------------------------
 Create a dpio test object.
--------------------------------------------------------------------------*/
dpiot1_t *dpiot1_create()
{
	dpiot1_t *ptest;

	commInitReq_t commInitReq;
	commScanAddrReq_t		scanReq;
	commScanAddrResp_t		scanResp;
	dp_result_t err;
	dp_transport_t dll;
	int i;
	char logfname[128];

	char peeradr_printable[128];
	unsigned char peeradr[dp_MAX_ADR_LEN];

	ptest = malloc(sizeof(dpiot1_t));
	if (!ptest)
		return NULL;
	memset(ptest, 0, sizeof(dpiot1_t));

	ptest->pkts_desired = 973;

	ptest->id = nextid++;

	sprintf(logfname, "dp%d.log", ptest->id);
	dp_setLogFP(NULL);
	dp_setLogFname(logfname);
	DPRINT(("dpiot1_create: logging started.\n"));
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

	return ptest;
}

/*--------------------------------------------------------------------------
 Connect a dpio test object to its peer.
--------------------------------------------------------------------------*/
void dpiot1_peer_connect(dpiot1_t *ptest, char *peerAdr)
{
	char key[10];
	dp_result_t err;

	dp_setLogFP(ptest->logfp);

	/* Open a comm handle to the peer's address */
	ptest->hPeer = dpio_openHdl(ptest->dpio, peerAdr, NULL, NULL);
	if (ptest->hPeer == PLAYER_NONE) {
		printf("Unable to connect to peer!\n");
		assert(FALSE);
	}
	printf("Opening handle to peer returns h:%x\n", ptest->hPeer);
}

/*--------------------------------------------------------------------------
 Call this repeatedly.
 Simulates the network processing routine of a game.
 Keeps sending test packets to peer, and checking packets from peer.
 Returns FALSE if done, TRUE if it still needs to be called.
--------------------------------------------------------------------------*/
int dpiot1_poll(dpiot1_t *ptest)
{
	char key[10];
	playerHdl_t src;
	char pkt[512];
	size_t size;
	dp_result_t err;

	dp_setLogFP(ptest->logfp);

	ptest->dpio_now = eclock();

	/* Get a burst of packets */
	do {
		assert(ptest->dpio_status == dp_RES_OK || ptest->dpio_status == dp_RES_EMPTY);
		src = (dpid_t) PLAYER_NONE;	/* kludge */
		size = sizeof(pkt);
		err = dpio_get(ptest->dpio, &src, &pkt, &size, NULL);
		DPRINT(("id %d: dpio_get returns err %d size %d from handle %x\n", ptest->id, err, size, src));
		if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
			printf("dpio_get reports error %d\n", err);
			assert(FALSE);
		}
		if (err == dp_RES_OK) {
			printf("id %d: Got packet %d size %d from handle %x\n", ptest->id, ptest->pkts_rx, size, src);
			testramp_compare(ptest->idPeer, ptest->pkts_rx, pkt, size, "rx");
			ptest->pkts_rx++;
		}
	} while ((err == dp_RES_OK) || (err == dp_RES_AGAIN));

	/* Send a burst of packets, if there's room */
	if ((ptest->pkts_tx < ptest->pkts_desired)
	&& (dpio_getHdlTxPktsFree(ptest->dpio, ptest->hPeer) > 10)) {
		do {
			size = testramp_fill(ptest->id, ptest->pkts_tx, pkt);
			err = dpio_put_reliable2(ptest->dpio, &ptest->hPeer, 1, pkt, size, NULL, dp_SEND_RESERVE_NONE);
			if (err == dp_RES_OK) {
				DPRINT(("id %d: sent packet %d len %d to handle %x\n", ptest->id, ptest->pkts_tx, size, ptest->hPeer));
				printf("id %d: sent packet %d len %d to handle %x\n", ptest->id, ptest->pkts_tx, size, ptest->hPeer);
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
	ptest->dpio_status = err;
	dpio_flush(ptest->dpio);

	if ((ptest->pkts_rx >= ptest->pkts_desired)
	&&  (ptest->pkts_tx >= ptest->pkts_desired)
	&&  (ptest->dpio_status == dp_RES_EMPTY)
	&&  (dp_RES_BUSY != dpio_ReadyToFreeze(ptest->dpio, NULL))) {
		return FALSE;
	}

	return TRUE;
}

/*--------------------------------------------------------------------------
 Destroy a dpio test object.  Does a few final checks.
--------------------------------------------------------------------------*/
void dpiot1_destroy(dpiot1_t *ptest)
{
	dp_result_t err;
	char key[10];

	printf("dpiot1_destroy: entry\n");

	dp_setLogFP(ptest->logfp);

	dpio_destroy(ptest->dpio, 0);

	printf("dpiot1_destroy: done\n");
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

	for (i=0; i<2; i++) {
		dpiot1_t *t1, *t2;

		printf("Run %d.\n", i);

		t1 = dpiot1_create();
		t2 = dpiot1_create();
		assert(t1 && t2);
		dpiot1_peer_connect(t1, t2->dpio->myAdr);
		t1->idPeer = t2->id;
		dpiot1_peer_connect(t2, t1->dpio->myAdr);
		t2->idPeer = t1->id;

		do {
			running1 = dpiot1_poll(t1);
			running2 = dpiot1_poll(t2);
		} while (running1 || running2);

		dpiot1_destroy(t1);
		dpiot1_destroy(t2);
	}
	printf("No test failed.\n");

	return 0;
}
