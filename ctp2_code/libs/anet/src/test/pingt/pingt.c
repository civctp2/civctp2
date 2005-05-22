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
 Unit Test for dpio ping and keepalive features.
-----------------------------------------------------------------------*/

#include <stdio.h>

#include "dp2.h"
#include "../../dp/dpprivy.h"
#include "eclock.h"
#include "dpio.h"

#define pingt_STATUS_CONTINUE	1
#define pingt_STATUS_SUCCESS	2
#define pingt_STATUS_FAILURE	3

#define pingt_PIGGYBACK_INTERVAL	500		/* ms */
#define pingt_FORCED_INTERVAL		2000	/* ms */
#define pingt_KEEPALIVE_INTERVAL	(dp_PLAYER_TIMEOUT_SECS*1000/8)	/* ms */

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

	/* Which test object this is connected to */
	int idPeer;
	playerHdl_t hPeer;

	/* time to send the next packet to give pings something to
	 * piggyback onto
	 */
	clock_t t_next_send;

	/* Number of ping responses so far */
	int nReplies;
} pingt_t;

/* Struct containing a test pair of connected pingt's */
typedef struct {
	pingt_t *pingt[2];

	int status;			/* one of pingt_STATUS_* */
	clock_t ping_interval;	/* expected interval between pings for this pair */
	clock_t t_start;	/* time connection was started */
	clock_t t_killed;	/* time when we killed pingt[1] */
	clock_t t_timeout;	/* time connection timed out, or 0 if it didn't */
	clock_t t_timeout_expected;	/* time connection should have timed out or 0 */
} pingt_pair_t;

/*--------------------------------------------------------------------------
 Create a dpio test object.
--------------------------------------------------------------------------*/
pingt_t *pingt_create(int appWillFlush)
{
	pingt_t *ptest;

	commInitReq_t commInitReq;
	dp_result_t err;
	dp_transport_t dll;
	char logfname[128];

	ptest = malloc(sizeof(pingt_t));
	if (!ptest)
		return NULL;
	memset(ptest, 0, sizeof(pingt_t));

	ptest->nReplies = 0;

	ptest->id = nextid++;
	ptest->hPeer = PLAYER_NONE;

	sprintf(logfname, "dp%d.log", ptest->id);
	dp_setLogFP(NULL);
	dp_setLogFname(logfname);
	DPRINT(("pingt_create: logging started.\n"));
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

	ptest->t_next_send = ptest->dpio_now;

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
 Connect a dpio test object to its peer with the given handleOptions
 (from dpio_OPTION_*).
--------------------------------------------------------------------------*/
void pingt_peer_connect(pingt_t *ptest, char *peerAdr, int handleOptions)
{
	dp_setLogFP(ptest->logfp);

	/* Open a comm handle to the peer's address */
	ptest->hPeer = dpio_openHdl(ptest->dpio, peerAdr, NULL, NULL);
	if (ptest->hPeer == PLAYER_NONE) {
		printf("Unable to connect to peer!\n");
		assert(FALSE);
		exit(1);
	}
	if (dp_RES_OK != dpio_setHandleOptions(ptest->dpio, ptest->hPeer, handleOptions)) {
		printf("Unable to set handleOptions:%x on h:%x\n", handleOptions, ptest->hPeer);
		assert(FALSE);
		exit(1);
	}
#ifdef VERBOSE
	printf("Opening handle to peer returns h:%x\n", ptest->hPeer);
#endif
}

/*--------------------------------------------------------------------------
 Create and connect a pair of pingt_t's.
--------------------------------------------------------------------------*/
pingt_pair_t *pingt_pair_create(int appWillFlush, int handleOptions)
{
	pingt_pair_t *pair;

	pair = (pingt_pair_t *)malloc(sizeof(pingt_pair_t));
	if (!pair) {
		printf("create pair failed\n");
		exit(1);
	}
	memset(pair, 0, sizeof(pingt_pair_t));
	pair->pingt[0] = pingt_create(appWillFlush);
	pair->pingt[1] = pingt_create(appWillFlush);
	if (!pair->pingt[0] || !pair->pingt[1]) {
		printf("create pingts failed\n");
		exit(1);
	}
	pingt_peer_connect(pair->pingt[0], pair->pingt[1]->dpio->myAdr, handleOptions);
	pingt_peer_connect(pair->pingt[1], pair->pingt[0]->dpio->myAdr, handleOptions);
	pair->pingt[0]->idPeer = pair->pingt[1]->id;
	pair->pingt[1]->idPeer = pair->pingt[0]->id;
	pair->t_start = eclock();
	pair->status = pingt_STATUS_CONTINUE;
	if (handleOptions & dpio_OPTION_PIGGYBACK_PING)
		pair->ping_interval = pingt_PIGGYBACK_INTERVAL;
	else if (handleOptions & dpio_OPTION_FORCED_PING)
		pair->ping_interval = pingt_FORCED_INTERVAL;
	else if (handleOptions & dpio_OPTION_KEEPALIVE)
		pair->ping_interval = pingt_KEEPALIVE_INTERVAL;
	else {
		printf("Invalid handle options:%x\n", handleOptions);
		exit(1);
	}
	printf("id %d,%d: ping_interval = %d\n", pair->pingt[0]->id, pair->pingt[1]->id, pair->ping_interval);
	return pair;
}

/*--------------------------------------------------------------------------
 Destroy a dpio test object.  Does a few final checks.
--------------------------------------------------------------------------*/
void pingt_destroy(pingt_t *ptest)
{
	assert(ptest);
#ifdef VERBOSE
	printf("pingt_destroy: entry\n");
#endif
	dp_setLogFP(ptest->logfp);
	dpio_destroy(ptest->dpio, 0);
#ifdef VERBOSE
	printf("pingt_destroy: done\n");
#endif
}

/*--------------------------------------------------------------------------
 Destroy pingt test pair. 
--------------------------------------------------------------------------*/
void pingt_pair_destroy(pingt_pair_t *pair)
{
	assert(pair);
	pingt_destroy(pair->pingt[0]);
	pingt_destroy(pair->pingt[1]);
	free(pair);
}

/*--------------------------------------------------------------------------
 Set the packet loss (on reception) for this pingt.
--------------------------------------------------------------------------*/
void pingt_setPktloss(pingt_t *ptest, int percent)
{
	if (!ptest || !ptest->dpio)
		return;

	dp_setLogFP(ptest->logfp);

	if (percent < 0)
		dpio_forceRxDropPercent(ptest->dpio, 0);
	else if (percent > 99)
		dpio_forceRxDropPercent(ptest->dpio, 99);
	else
		dpio_forceRxDropPercent(ptest->dpio, percent);
#ifdef VERBOSE
	DPRINT("Set packetloss to %d%%\n", ptest->dpio->rxDropPercent));
#endif
}

/*--------------------------------------------------------------------------
 Set the ping intervals for this pingt.
--------------------------------------------------------------------------*/
void pingt_setPingIntervals(pingt_t *ptest, clock_t piggybackInterval, clock_t forcedInterval)
{
	if (!ptest || !ptest->dpio)
		return;

	dp_setLogFP(ptest->logfp);

	if (dp_RES_OK != dpio_setPingIntervals(ptest->dpio, piggybackInterval, forcedInterval)) {
		printf("id %d: setPingIntervals(%d, %d) failed!\n", ptest->id, piggybackInterval, forcedInterval);
		exit(1);
	}
}

/*--------------------------------------------------------------------------
 Call this repeatedly.
 Watches for ping responses.
 If send is non-zero, send an occasional packet to give outgoing pings
 something to piggyback onto.
 Returns TRUE while the connection to peer is open,
         FALSE if the connection is lost.
--------------------------------------------------------------------------*/
dp_result_t pingt_poll(pingt_t *ptest, int send)
{
	dp_result_t err;
	playerHdl_t src;
	playerHdl_t h;
	char pkt[512];
	size_t size;
	int flags;

	dp_setLogFP(ptest->logfp);

	if (PLAYER_NONE == ptest->hPeer)
		return FALSE;

	ptest->dpio_now = eclock();
	/* Get any incoming packets */
	do {
		src = (dpid_t) PLAYER_NONE;	/* kludge */
		size = sizeof(pkt);
		err = dpio_get(ptest->dpio, &src, &pkt, &size, &flags);
		if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
			printf("dpio_get reports error %d\n", err);
			assert(FALSE);
		}
		if (flags & dpio_GET_LATENCY_MEASUREMENT) {
			int RTT = dpio_get_player_latency(ptest->dpio, ptest->hPeer, 0);
			ptest->nReplies++;
			if (src == ptest->hPeer) {
				printf("id %d: t:%d, got ping response %d from id:%d, new RTT:%d\n", ptest->id, ptest->dpio_now, ptest->nReplies, ptest->idPeer, RTT);
			} else {
				assert(src == ptest->hPeer);
			}
		}
		if (err == dp_RES_OK) {
			if (src == ptest->hPeer) {
#ifdef VERBOSE
				printf("id %d: got packet tag:%c%c size:%d from id:%d\n", ptest->id, pkt[0], pkt[1], size, ptest->idPeer);
#endif
				if (*((dp_packetType_t *)pkt) != dppt_MAKE('P', 'T')) {
					printf("id %d: got unexpected packet tag:%c%c size:%d from id:%d\n", ptest->id, pkt[0], pkt[1], size, ptest->idPeer);
					assert(*((dp_packetType_t *)pkt) == dppt_MAKE('P', 'T'));
				}
			} else {
				assert(src == ptest->hPeer);
			}
		}
	} while ((err == dp_RES_OK) || (err == dp_RES_AGAIN));

	if (send && ((long)(ptest->dpio_now - ptest->t_next_send) > 0)) {
		*((dp_packetType_t *)pkt) = dppt_MAKE('P', 'T');
		/* rest of packet is random junk we don't care about */
		err = dpio_put_unreliable(ptest->dpio, &ptest->hPeer, 1, pkt, 100, &src);
		if (err != dp_RES_OK) {
			printf("id %d: send failed to id:%d with err:%d\n", ptest->id, ptest->idPeer, err);
		}
		ptest->t_next_send = ptest->dpio_now + ECLOCKS_PER_SEC/4;
	}

	err = dpio_update(ptest->dpio);
	if (err != dp_RES_OK && err != dp_RES_EMPTY) {
		printf("dpio_update reports error %d\n", err);
		assert(FALSE);
	}
	dpio_flush(ptest->dpio);
	h = dpio_findTimedOutHost(ptest->dpio);
	if (h != PLAYER_NONE) {
		DPRINT(("id %d: t:%d, h:%x timed out\n", ptest->id, ptest->dpio_now, h));
		dpio_closeHdlImmed(ptest->dpio, h);
		if (h == ptest->hPeer) {
			printf("id %d: t:%d, id:%x timed out\n", ptest->id, ptest->dpio_now, ptest->idPeer);
			ptest->hPeer = PLAYER_NONE;
			return FALSE;
		}
		assert(h == ptest->hPeer);
	}
	return TRUE;
}

int pingt_pair_poll_timeout(pingt_pair_t *pair, clock_t now, clock_t end)
{
	int connected;

	if (!pair)
		return pingt_STATUS_FAILURE;
	if (pair->status != pingt_STATUS_CONTINUE)
		return pair->status;

	if ((long)(now - end) > 0) {
		if (pair->t_timeout) {
			printf("id %d (timeout): noticed id:%d's death after %.2fs, expected ~%.2fs\n",
				pair->pingt[0]->id, pair->pingt[0]->idPeer,
				(float)(pair->t_timeout - pair->t_killed)/ECLOCKS_PER_SEC,
				(float)(pair->t_timeout_expected - pair->t_killed)/ECLOCKS_PER_SEC);
			if (pair->t_timeout_expected
			&&  ((long)(pair->t_timeout - pair->t_timeout_expected) > (-3 - pair->ping_interval/1000)*ECLOCKS_PER_SEC)
			&&  ((long)(pair->t_timeout - pair->t_timeout_expected) < 2*ECLOCKS_PER_SEC)) {
				printf("id %d,%d (timeout): test passed\n", pair->pingt[0]->id, pair->pingt[0]->idPeer);
				pair->status = pingt_STATUS_SUCCESS;
			} else {
				printf("id %d,%d (timeout): test failed\n", pair->pingt[0]->id, pair->pingt[0]->idPeer);
				pair->status = pingt_STATUS_FAILURE;
			}
		} else {
			printf("id %d (timeout): t:%d, id:%d did not timeout in %.2fs, expected ~%.2fs\n",
				pair->pingt[0]->id, now, pair->pingt[0]->idPeer,
				(float)(now - pair->t_killed)/ECLOCKS_PER_SEC,
				(float)(pair->t_timeout_expected - pair->t_killed)/ECLOCKS_PER_SEC);
			printf("id %d,%d (timeout): test failed\n", pair->pingt[0]->id, pair->pingt[1]->id);
			pair->status = pingt_STATUS_FAILURE;
			pingt_destroy(pair->pingt[1]);
			pair->pingt[1] = NULL;
		}
		pingt_destroy(pair->pingt[0]);
		pair->pingt[0] = NULL;
		return pair->status;
	}
	if (pair->t_timeout)
		return pingt_STATUS_CONTINUE;

	/* kill pingt[1] after 20 seconds */
	if ((long)(now - pair->t_start) < 20 * ECLOCKS_PER_SEC) {
		connected = pingt_poll(pair->pingt[1], 0);
		if (!connected) {
			printf("id %d (timeout): t:%d lost connection to id:%d\n",
				pair->pingt[1]->id, now, pair->pingt[1]->idPeer);
			printf("id %d,%d (timeout): test failed\n", pair->pingt[0]->id, pair->pingt[1]->id);
			pair->status = pingt_STATUS_FAILURE;
			pingt_destroy(pair->pingt[1]);
			pair->pingt[1] = NULL;
		}
		connected = pingt_poll(pair->pingt[0], 0);
		if (!connected) {
			printf("id %d (timeout): t:%d lost connection to id:%d\n",
				pair->pingt[0]->id, now, pair->pingt[0]->idPeer);
			printf("id %d,%d (timeout): test failed\n", pair->pingt[0]->id, pair->pingt[1]->id);
			pair->status = pingt_STATUS_FAILURE;
			pingt_destroy(pair->pingt[0]);
			pair->pingt[0] = NULL;
		}
	} else {
		if (pair->pingt[1]) {
			pair->t_killed = now;
			pair->t_timeout_expected = now + (dp_PLAYER_TIMEOUT_SECS * ECLOCKS_PER_SEC);
			printf("id %d (timeout): t:%d, disconnecting abruptly\n",
				pair->pingt[1]->id, now);
			pingt_destroy(pair->pingt[1]);
			pair->pingt[1] = NULL;
		}
		/* wait for pingt[0] to notice pingt[1] is dead */
		if (pair->pingt[0]) {
			connected = pingt_poll(pair->pingt[0], 0);
			if (!connected) {
				pair->t_timeout = now;
				printf("id %d (timeout): t:%d, noticed id:%d's death after %.2fs, expected ~%.2fs\n",
					pair->pingt[0]->id, now, pair->pingt[0]->idPeer,
					(float)(now - pair->t_killed)/ECLOCKS_PER_SEC,
					(float)(pair->t_timeout_expected - pair->t_killed)/ECLOCKS_PER_SEC);
			}
		}
	}
	return pair->status;
}

int pingt_pair_poll_ping(pingt_pair_t *pair, clock_t now, clock_t end)
{
	int connected;
	int i;

	if (!pair)
		return pingt_STATUS_FAILURE;
	if (pair->status != pingt_STATUS_CONTINUE)
		return pair->status;

	if ((long)(now - end) > 0) {
		int replies_expected;

		replies_expected = ((long)(now - pair->t_start))*1000/pair->ping_interval/ECLOCKS_PER_SEC;
		pair->status = pingt_STATUS_SUCCESS;
		for (i = 0; i < 2; i++) {
			printf("id %d (ping): received %d pings, expected ~%d\n",
				pair->pingt[i]->id, pair->pingt[i]->nReplies, replies_expected);
			if ((pair->pingt[i]->nReplies < replies_expected*2/3)
			||  (pair->pingt[i]->nReplies > replies_expected*4/3)) {
				pair->status = pingt_STATUS_FAILURE;
			}
		}
		if (pair->status == pingt_STATUS_SUCCESS)
			printf("id %d,%d (ping): test passed\n", pair->pingt[0]->id, pair->pingt[1]->id);
		else
			printf("id %d,%d (ping): test failed\n", pair->pingt[0]->id, pair->pingt[1]->id);
		pingt_destroy(pair->pingt[0]);
		pair->pingt[0] = NULL;
		pingt_destroy(pair->pingt[1]);
		pair->pingt[1] = NULL;
		return pair->status;
	}
	
	/* just make sure they don't disconnect. */
	for (i = 0; i < 2; i++) {
		connected = pingt_poll(pair->pingt[i], 1);
		if (!connected) {
			printf("id %d (ping): t:%d lost connection to id:%d after %.2fs\n",
				pair->pingt[i]->id, now, pair->pingt[i]->idPeer,
				(float)((long)(now - pair->t_start))/ECLOCKS_PER_SEC);
			printf("id %d,%d (ping): test failed\n", pair->pingt[0]->id, pair->pingt[1]->id);
			pair->status = pingt_STATUS_FAILURE;
			pingt_destroy(pair->pingt[0]);
			pair->pingt[0] = NULL;
			pingt_destroy(pair->pingt[1]);
			pair->pingt[1] = NULL;
			break;
		}
	}
	return pair->status;
}

int pingt_pair_poll_keepalive(pingt_pair_t *pair, clock_t now, clock_t end)
{
	int connected;
	int i;

	if (!pair)
		return pingt_STATUS_FAILURE;
	if (pair->status != pingt_STATUS_CONTINUE)
		return pair->status;

	if ((long)(now - end) > 0) {
		printf("id %d,%d (keepalive): test passed\n", pair->pingt[0]->id, pair->pingt[1]->id);
		pair->status = pingt_STATUS_SUCCESS;
		pingt_destroy(pair->pingt[0]);
		pair->pingt[0] = NULL;
		pingt_destroy(pair->pingt[1]);
		pair->pingt[1] = NULL;
		return pair->status;
	}

	/* just make sure they don't disconnect. */
	for (i = 0; i < 2; i++) {
		connected = pingt_poll(pair->pingt[i], 0);
		if (!connected) {
			printf("id %d (keepalive): t:%d lost connection to id:%d after %.2fs\n",
				pair->pingt[i]->id, now, pair->pingt[i]->idPeer,
				(float)((long)(now - pair->t_start))/ECLOCKS_PER_SEC);
			printf("id %d,%d (keepalive): test failed\n", pair->pingt[0]->id, pair->pingt[1]->id);
			pair->status = pingt_STATUS_FAILURE;
			pingt_destroy(pair->pingt[0]);
			pair->pingt[0] = NULL;
			pingt_destroy(pair->pingt[1]);
			pair->pingt[1] = NULL;
			break;
		}
	}
	return pair->status;
}


/*-----------------------------------------------------------------------
 Unit test main loop.
 Creates a few sets of two connected dpio's and checks their ping
 and keepalive behavior for sanity in various test scenarios.
-----------------------------------------------------------------------*/
main(int argc, char **argv)
{
	int i;
	int appWillFlush;
	pingt_pair_t *timeout;
	pingt_pair_t *ping;
	pingt_pair_t *fping;
	pingt_pair_t *keepalive;
	clock_t now;
	clock_t t_end;

	if (argc <= 1) {
		printf("Usage: %s <appWillFlush>\n\
If <appWillFlush> is 0, test will not use packet gathering.\n",
			argv[0]);
		exit(1);
	}
	appWillFlush = atoi(argv[1]);
	if (appWillFlush)
		appWillFlush = TRUE;
	else
		appWillFlush = FALSE;

	/* Create two dpios to test timeout on disconnect */
	timeout = pingt_pair_create(appWillFlush, dpio_OPTION_KEEPALIVE);
	printf("Created timeout pair, ids:%d and %d\n", timeout->pingt[0]->id,
		timeout->pingt[1]->id);

	/* Create two dpios to test ping behavior */
	ping = pingt_pair_create(appWillFlush, dpio_OPTION_PIGGYBACK_PING
		| dpio_OPTION_FORCED_PING | dpio_OPTION_KEEPALIVE);
	pingt_setPingIntervals(ping->pingt[0], pingt_PIGGYBACK_INTERVAL, pingt_FORCED_INTERVAL);
	pingt_setPingIntervals(ping->pingt[1], pingt_PIGGYBACK_INTERVAL, pingt_FORCED_INTERVAL);
	printf("Created ping pair, ids:%d and %d\n", ping->pingt[0]->id,
		ping->pingt[1]->id);

	/* Create two dpios to test forced ping behavior */
	fping = pingt_pair_create(appWillFlush, dpio_OPTION_FORCED_PING
		| dpio_OPTION_KEEPALIVE);
	pingt_setPingIntervals(fping->pingt[0], pingt_PIGGYBACK_INTERVAL, pingt_FORCED_INTERVAL);
	pingt_setPingIntervals(fping->pingt[1], pingt_PIGGYBACK_INTERVAL, pingt_FORCED_INTERVAL);
	printf("Created fping pair, ids:%d and %d\n", fping->pingt[0]->id,
		fping->pingt[1]->id);

	/* Create two dpios to test keepalive with packet loss */
	keepalive = pingt_pair_create(appWillFlush, dpio_OPTION_KEEPALIVE);
	pingt_setPktloss(keepalive->pingt[0], 10);
	pingt_setPktloss(keepalive->pingt[1], 10);
	printf("Created keepalive pair, ids:%d and %d\n", keepalive->pingt[0]->id,
		keepalive->pingt[1]->id);

	t_end = eclock() + 120 * ECLOCKS_PER_SEC;
	do {
		now = eclock();

		pingt_pair_poll_timeout(timeout, now, t_end);
		pingt_pair_poll_ping(ping, now, t_end);
		pingt_pair_poll_ping(fping, now, t_end);
		pingt_pair_poll_keepalive(keepalive, now, t_end);
	} while ((timeout->status == pingt_STATUS_CONTINUE)
	      || (ping->status == pingt_STATUS_CONTINUE)
	      || (fping->status == pingt_STATUS_CONTINUE)
	      || (keepalive->status == pingt_STATUS_CONTINUE));

	if ((timeout->status != pingt_STATUS_SUCCESS)
	||  (ping->status != pingt_STATUS_SUCCESS)
	||  (fping->status != pingt_STATUS_SUCCESS)
	||  (keepalive->status != pingt_STATUS_SUCCESS)) {
		printf("Test failed\n");
		exit(1);
	}
	printf("No test failed\n");
	return 0;
}
