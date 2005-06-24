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
 Unit Test for dptab
-----------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

#include "dp2.h"
#include "../../dp/dpprivy.h"
#include "eclock.h"
#include "dptab.h"

#define BIGDATALEN 800

int nextid = 1;

/* Set this FALSE to disable local callback value check on variables,
 * e.g. when setting up illegal value which will be overridden
 * before it finishes sending
 */
int checklocal = TRUE;

/* Use -v on command line to activate verbose printing */
int Verbose = FALSE;

/*--------------------------------------------------------------------------
 Structure holding the state of a dptab test object.
--------------------------------------------------------------------------*/
typedef struct {
	/* The dptab being tested, and its support objects */
	dptab_t *dt;
	clock_t dpio_now;
	dpio_t *dpio;
	FILE *logfp;

	/* Which test object this is */
	int id;

	/* id of peer, handle to peer */
	int idPeer;
	playerHdl_t hPeer;

	/* Pointers to tables inside dptab being tested */
	dptab_table_t *peerTable1, *peerTable2;
	dptab_table_t *table1, *table2;

	/* Test state */
	clock_t started;
	dp_result_t dptab_status;
	int nvars_rx;
	int nvars_tx;
	int nvars_rx_delete;
	int nvars_tx_delete;
	int pkts_rx;
	int pkts_rx_at_bogus_set;
} dptabt1_t;

/*--------------------------------------------------------------------------
 Given a process id, key and subkey, allocate and fill a buffer with the 
 test data.
 Returns length of buffer.

 Test patterns length is as follows:
   odd tables: 
   	length=4
   even tables:
    odd subkeys: 
		length=max allowed
	even subkeys:
		length=BIGDATALEN
 Test pattern data depends on id, key, and subkey.
--------------------------------------------------------------------------*/
static int testramp_fill(int id, int key, int subkey, char **bufp)
{
	int len;
	int i;
	char xor;
	char *buf;

	/* Get length */
	if (key & 1) {
		/* It's a fixed size table.  Always use 4 bytes. */
		len = 4;
	} else {
		/* It's a variable size table. */
		if (subkey & 1)
			len = dp_PLAYERDATA_LEN_MAX;
		else
			len = BIGDATALEN;
	}

	/* Fill with pattern */
	xor = (char)((id << 6) | (key << 4) | subkey);
	buf = malloc(len);
	if (!buf) 
		return 0;
	for (i=0; i<len; i++)
		buf[i] = (char)((i & 255) ^ xor);

	*bufp = buf;
	if (Verbose)
		printf("testramp_fill: id %d, key %d, subkey %d; length %d\n", id, key, subkey, len);
	return len;
}

/*--------------------------------------------------------------------------
 Given a process id, key and subkey, compare a buffer with the 
 expected test pattern.

 Aborts with given error message if comparison fails.
--------------------------------------------------------------------------*/
static void testramp_compare(int id, int key, int subkey, char *buf, int buflen, char *msg)
{
	char *refbuf;
	int reflen;

	if (!buf) {
		printf("%s: null buf\n", msg);
		exit(1);
	}
	reflen = testramp_fill(id, key, subkey, &refbuf);
	if (!reflen) {
		printf("%s: Out of memory\n", msg);
		exit(1);
	}
	if (reflen != buflen) {
		DPRINT(("%s: id %d, key %d, subkey %d; Bad length %d, expected %d\n", msg, id, key, subkey, buflen, reflen));
		if (Verbose)
			printf("%s: id %d, key %d, subkey %d; Bad length %d, expected %d\n", msg, id, key, subkey, buflen, reflen);
		assert(reflen == buflen);
		exit(1);
	}
	if (memcmp(buf, refbuf, buflen)) {
		printf("%s: Bad data\n", msg);
		exit(1);
	}
}

/*-----------------------------------------------------------------------
 Routine called by dptab whenever any i/o happens.  
 We pass in the dptabt1_t in the context parameter, so we can
 know which test instance the callback is for.
-----------------------------------------------------------------------*/
int dp_PASCAL table_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t err)
{
	dptabt1_t *ptest = (dptabt1_t *)context;
	int idsrc;
	int skey;

	/* Note who it's from */
	if (src == PLAYER_ME) {
		idsrc = ptest->id;
		skey = table->key[0];
	} else {
		idsrc = ptest->idPeer;
		skey = table->key[1];
	}

	assert(ptest->nvars_rx > -1);
	assert(ptest->nvars_rx < 100);
	DPRINT(("dptabt1: table_cb: id %d, idsrc %d, status:%d\n", ptest->id, idsrc, err));

	if (err == dp_RES_CREATED) {
		DPRINT(("table_cb: got: src h:%x, dest h:%x, key %s, subkey %s, sent %d, total %d, nvars_tx %d\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen),
			sent, total, ptest->nvars_tx));
		if ((src != PLAYER_ME) && (dest == PLAYER_ME) && (sent == total)) {
			ptest->nvars_rx++;
			DPRINT(("id %d: Got %d'th variable from h:%x; table %s, subkey %s; len %d\n", 
				ptest->id,
				ptest->nvars_rx,
				src, 
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				total));
			printf("id %d: Got %d'th variable from h:%x; table %s, subkey %s; len %d\n", 
				ptest->id,
				ptest->nvars_rx,
				src, 
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				total);
		}
	} else if (err == dp_RES_DELETED) {
		DPRINT(("table_cb: delete: src h:%x, dest h:%x, key %s, subkey %s\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));
		if (dest == PLAYER_ME)
			ptest->nvars_rx_delete++;		/* deleted by someone else */
		printf("id %d: Got delete variable from h:%x; table %s, subkey %s\n", 
			ptest->id,
			src, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen));
	} else if (err == dp_RES_FINISHED) {
		DPRINT(("table_cb: sent: src h:%x, dest h:%x, key %s, subkey %s, sent %d, total %d, nvars_tx %d\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen),
			sent, total, ptest->nvars_tx));
		if ((src == PLAYER_ME) && (sent == total)) {
			ptest->nvars_tx++;
			DPRINT(("table_cb: nvars_tx incremented to %d\n", ptest->nvars_tx));
			printf("id %d: Sent %d'th variable to h:%x; table %s, subkey %s; len %d\n", 
				ptest->id, ptest->nvars_tx,
				dest, 
				key2a(table->key, table->keylen),
				key2a2(subkey, subkeylen),
				total);
			if (ptest->nvars_tx > 10) {
				/* shouldn't be greater than 5, really */
				DPRINT(("id %d: Too many sends\n", ptest->id));
				printf("id %d: Too many sends\n", ptest->id);
				exit(1);
			}
		}
	}
	if (checklocal) 
		testramp_compare(idsrc, skey, subkey[0], buf, total, "table_cb");
	return TRUE;
}

/*-------------------------------------------------------------------------
 Build tables 1 and 2.
 Table 1 is a table of integers with two elements: 
	1.0
	1.1
 Table 2 is a table of blobs with two elements: 
 	2.2
 	2.3
 Make sure those elements are accessible, and nonexistent elements aren't.
 Tables are left built for use in later tests.
-------------------------------------------------------------------------*/
void dptabt1_build_tables(dptabt1_t *ptest)
{
	dp_result_t err;
	char key[10];
	char badkey[10];
	char subkey[10];
	int dummy;
	int *pdummy;
	size_t len;
	int subkeylen;
	char bigdata[BIGDATALEN];
	char *pbigdata;
	int i;
	dptab_table_t *table1;
	dptab_table_t *table2;
	char *maxdata;
	char *buf;
	int skey;

	/* Create one fixed size table and one variable sized table. */
	key[0] = 1;
	err = dptab_createTable(ptest->dt, &table1, key, 1, sizeof(int), NULL, NULL, table_cb, ptest);
	assert(err == dp_RES_OK);

	key[0] = 2;
	err = dptab_createTable(ptest->dt, &table2, key, 1, 0, NULL, NULL, table_cb, ptest);
	assert(err == dp_RES_OK);

	/* Put two elements in the fixed table: 1.0 and 1.1 */
	for (skey=0; skey<2; skey++) {
		key[0] = skey;
		len = testramp_fill(ptest->id, table1->key[0], key[0], &buf);
		err = dptab_set(ptest->dt, table1, key, 1, buf, len, 0, PLAYER_ME);
		assert(err == dp_RES_OK);
		free(buf);
	}
	/* Put two elements in the variable table: 2.2 and 2.3 */
	for (skey=2; skey<4; skey++) {
		key[0] = skey;
		len = testramp_fill(ptest->id, table2->key[0], key[0], &buf);
		err = dptab_set(ptest->dt, table2, key, 1, buf, len, 0, PLAYER_ME);
		assert(err == dp_RES_OK);
		free(buf);
	}

	/* Make sure they can be retrieved by key. */
	for (skey=0; skey<2; skey++) {
		key[0] = skey;
		err = dptab_get_bykey(table1, key, 1, (void **)&buf, &len);
		assert(err == dp_RES_OK);
		testramp_compare(ptest->id, table1->key[0], key[0], buf, len, "first get_bykey");
	}
	for (skey=2; skey<4; skey++) {
		key[0] = skey;
		err = dptab_get_bykey(table2, key, 1, (void **)&buf, &len);
		assert(err == dp_RES_OK);
		testramp_compare(ptest->id, table2->key[0], key[0], buf, len, "first get_bykey");
	}
	/* Make sure they can be retrieved by index. */
	for (i=0; i<2; i++) {
		skey = i;
		key[0] = skey;
		err = dptab_get_byindex(table1, i, (void **)&buf, &len, subkey, &subkeylen);
		assert(err == dp_RES_OK);
		assert(subkeylen == 1);
		assert(subkey[0] == skey);
		testramp_compare(ptest->id, table1->key[0], key[0], buf, len, "first get_byindex");
	}
	for (i=0; i<2; i++) {
		skey = i + 2;
		key[0] = skey;
		err = dptab_get_byindex(table2, i, (void **)&buf, &len, subkey, &subkeylen);
		assert(err == dp_RES_OK);
		assert(subkeylen == 1);
		assert(subkey[0] == skey);
		testramp_compare(ptest->id, table2->key[0], key[0], buf, len, "first get_byindex");
	}

	/* Make sure other values can't be retrieved! */
	badkey[0] = 5;
	err = dptab_get_bykey(table1, badkey, 1, (void **)&pdummy, &len);
	assert(err == dp_RES_EMPTY);

	badkey[0] = 9;
	err = dptab_get_bykey(table2, badkey, 1, (void **)&pdummy, &len);
	if (err != dp_RES_EMPTY)
		printf("bug: got err %d\n", err);
	assert(err == dp_RES_EMPTY);

	ptest->table1 = table1;
	ptest->table2 = table2;
}

/*--------------------------------------------------------------------------
 Create a dptab test object.
--------------------------------------------------------------------------*/
dptabt1_t *dptabt1_create()
{
	dptabt1_t *ptest;

	commInitReq_t commInitReq;
	commScanAddrReq_t		scanReq;
	commScanAddrResp_t		scanResp;
	dp_result_t err;
	dp_transport_t dll;
	int i;
	char logfname[128];

	char peeradr_printable[128];
	unsigned char peeradr[dp_MAX_ADR_LEN];

	ptest = malloc(sizeof(dptabt1_t));
	if (!ptest)
		return NULL;
	memset(ptest, 0, sizeof(dptabt1_t));

	ptest->id = nextid++;

	sprintf(logfname, "dp%d.log", ptest->id);
	dp_setLogFP(NULL);
	dp_setLogFname(logfname);
	DPRINT(("dptabt1_create: logging started.\n"));
	ptest->logfp = dp_getLogFP();

	/* Create a dpio using the loopback driver */
	memset(&commInitReq, 0, sizeof(commInitReq));
	commInitReq.reqLen = sizeof(commInitReq_t);
	commInitReq.sessionId = ptest->id;	/* my own address */
	commInitReq.portnum = 20000+ptest->id;	/* my own address */
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

	/* Create the dptab */
	ptest->dt = dptab_create(ptest->dpio);
	assert(ptest->dt);

	/* Build dummy test tables */
	dptabt1_build_tables(ptest);

	ptest->dpio->appWillFlush = 1;	/* kludge, fixme */

	return ptest;
}

/*--------------------------------------------------------------------------
 Connect a dptab test object to its peer.
--------------------------------------------------------------------------*/
void dptabt1_peer_connect(dptabt1_t *ptest, char *peerAdr)
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

	/* Add the second machine to the party */
	err = dptab_addPeer(ptest->dt, ptest->hPeer);
	assert(err == dp_RES_OK);

	/* Create fixed table #9.1 and route peer's table #1 to it. */
	key[0] = 9;
	key[1] = 1;
	err = dptab_createTable(ptest->dt, &ptest->peerTable1, key, 2, sizeof(int), NULL, NULL, table_cb, ptest);
	assert(err == dp_RES_OK);
	key[0] = 1;
	err = dptab_addPublisher(ptest->dt, ptest->peerTable1, key, 1, ptest->hPeer);
	assert(err == dp_RES_OK);

	/* Create variable table #9.2 and route peer's table #2 to it. */
	key[0] = 9;
	key[1] = 2;
	err = dptab_createTable(ptest->dt, &ptest->peerTable2, key, 2, 0, NULL, NULL, table_cb, ptest);
	assert(err == dp_RES_OK);
	key[0] = 2;
	err = dptab_addPublisher(ptest->dt, ptest->peerTable2, key, 1, ptest->hPeer);
	assert(err == dp_RES_OK);

	/* Publish our table 2 to our peer.  There are no entries in it yet. */
	err = dptab_addSubscriber(ptest->dt, ptest->table2, ptest->hPeer);
	assert(err == dp_RES_OK);
}


/*--------------------------------------------------------------------------
 Generate a specific stream of dptab traffic, designed to test several
 assumptions about how dptab works.
--------------------------------------------------------------------------*/
void dptabt1_send(dptabt1_t *ptest)
{
	char key[10];
	dp_result_t err;
	int j;

	dp_setLogFP(ptest->logfp);

	ptest->nvars_rx = 0;
	ptest->nvars_tx = 0;
	ptest->nvars_rx_delete = 0;
	ptest->dptab_status = dp_RES_OK;
	ptest->started = eclock();

	/* Send same four variables zillions of times before calling
	 * dptab_update; we should end up with no duplicate sends.
	 * Note: we don't have any direct way of detecting whether it
	 * does or not!  Have to examine the stats, and see if a large number
	 * of packets get sent...
	 *
	 * Use a mix of two different large and small variables.
	 */

	for (j=0; j<1000; j++) {
		key[0] = 0;
		err = dptab_send(ptest->dt, ptest->table1, key, 1, ptest->hPeer, 1);
		assert(err == dp_RES_OK);

		key[0] = 2;
		err = dptab_send(ptest->dt, ptest->table2, key, 1, ptest->hPeer, 1);
		assert(err == dp_RES_OK);

		key[0] = 1;
		err = dptab_send(ptest->dt, ptest->table1, key, 1, ptest->hPeer, 1);
		assert(err == dp_RES_OK);

		key[0] = 3;
		err = dptab_send(ptest->dt, ptest->table2, key, 1, ptest->hPeer, 1);
		assert(err == dp_RES_OK);
	}
}

/*--------------------------------------------------------------------------
 Call this repeatedly.
 Checks for dptab traffic from the peer's dptab_send() to be received, verify 
 that it meets our expectations.
 Returns FALSE if done, TRUE if it still needs to be called.
--------------------------------------------------------------------------*/
int dptabt1_poll(dptabt1_t *ptest)
{
	char key[10];
	playerHdl_t src;
	char pkt[512];
	size_t size;
	dp_result_t err;

	dp_setLogFP(ptest->logfp);

	ptest->dpio_now = eclock();

	/* Returns EMPTY when nothing waiting to go out */
	ptest->dptab_status = dptab_update(ptest->dt);
	assert(ptest->dptab_status == dp_RES_OK || ptest->dptab_status == dp_RES_EMPTY);

	if ((ptest->nvars_rx >= 4) && (ptest->nvars_tx_delete == 0)) {
		int len;
		char *buf;

		ptest->nvars_tx_delete++;
		/* Make sure we can delete one remotely, too! */
		key[0] = 2;
		err = dptab_send_delete(ptest->dt, ptest->table2, key, 1, ptest->hPeer, 1);
		assert(err == dp_RES_OK);

		/* And start sending a maximally long bogus value. */
		key[0] = 4;
		len = testramp_fill(ptest->id, ptest->table2->key[0], 199+key[0], &buf);
		checklocal = FALSE;		/* avoid assertion failure on local callback */
		DPRINT(("Setting bogus value; pkts_rx = %d\n", ptest->pkts_rx));
		if (Verbose)
			printf("Setting bogus value; pkts_rx = %d\n", ptest->pkts_rx);
		err = dptab_set(ptest->dt, ptest->table2, key, 1, buf, len, 1, PLAYER_ME);
		ptest->pkts_rx_at_bogus_set = ptest->pkts_rx;
		checklocal = TRUE;
		assert(err == dp_RES_OK);
		free(buf);
	}

	/* Yank rug out from under bogus value - send real, shorter, one. */
	/* Timing is fragile here; I'm guessing that the long, bogus value
	 * will have started to arrive by the time ten packets are received.
	 * Want to see if receiving an override works ok.
	 */
	if (ptest->pkts_rx_at_bogus_set && (ptest->pkts_rx_at_bogus_set + 4 <= ptest->pkts_rx)) {
		int len;
		char *buf;

		DPRINT(("Setting final good value\n"));
		if (Verbose)
			printf("Setting final good value\n");
		key[0] = 4;
		len = testramp_fill(ptest->id, ptest->table2->key[0], key[0], &buf);
		err = dptab_set(ptest->dt, ptest->table2, key, 1, buf, len, 1, PLAYER_ME);
		ptest->pkts_rx_at_bogus_set=0;
	}

	err = dpio_update(ptest->dpio);
	if (err != dp_RES_OK && err != dp_RES_EMPTY) {
		printf("dpio_update reports error %d\n", err);
		assert(FALSE);
	}
	src = (dpid_t) PLAYER_NONE;	/* kludge */
	size = sizeof(pkt);
	err = dpio_get(ptest->dpio, &src, &pkt, &size, NULL);
	if (err != dp_RES_OK && err != dp_RES_EMPTY && err != dp_RES_AGAIN) {
		printf("dpio_get reports error %d\n", err);
		assert(FALSE);
	}
	if (err == dp_RES_OK) {
		ptest->pkts_rx++;

		/*printf("id %d: Got packet from handle %x\n", ptest->id, src);*/
		err = dptab_handlePacket(ptest->dt, src, size, pkt);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			printf("dptab_handlePacket reports error %d\n", err);
			fflush(stdout);
			assert(FALSE);
		}
		DPRINT(("id %d: rx %d tx %d del %d stat %d ready %d pkts_rx %d\n", ptest->id, ptest->nvars_rx, ptest->nvars_tx, ptest->nvars_rx_delete, ptest->dptab_status, dpio_ReadyToFreeze(ptest->dpio, NULL), ptest->pkts_rx));
		if (Verbose)
			printf("id %d: rx %d tx %d del %d stat %d ready %d pkts_rx %d\n", ptest->id, ptest->nvars_rx, ptest->nvars_tx, ptest->nvars_rx_delete, ptest->dptab_status, dpio_ReadyToFreeze(ptest->dpio, NULL), ptest->pkts_rx);
	} else if (err == dp_RES_EMPTY) {
		playerHdl_t h_timedout;
		dpio_flush(ptest->dpio);
		h_timedout = dpio_findTimedOutHost(ptest->dpio);
		if (h_timedout != PLAYER_NONE) {
			/* FIXME: add a fail function */
			printf("id %d: h:%x timed out!\n", ptest->id, h_timedout);
			assert(h_timedout == ptest->hPeer);
			exit(1);
		}
	}
	if ((ptest->nvars_rx >= 5) 
	&&  (ptest->nvars_tx >= 5) 
	&&  (ptest->nvars_rx_delete >= 1) 
	&&  (ptest->dptab_status == dp_RES_EMPTY) 
	&&  (dp_RES_BUSY != dpio_ReadyToFreeze(ptest->dpio, NULL))) {
		return FALSE;
	}

	return TRUE;
}

/*--------------------------------------------------------------------------
 Destroy a dptab test object.  Does a few final checks.
--------------------------------------------------------------------------*/
void dptabt1_destroy(dptabt1_t *ptest)
{
	dp_result_t err;
	char key[10];

	printf("dptabt1_destroy: entry\n");

	dp_setLogFP(ptest->logfp);

	/* Make sure that we can delete the items created by the other host
	 * by referring to it by handle.
	 */
	assert(ptest->peerTable1->vars->n_used != 0);
	err = dptab_delete_bySrc(ptest->dt, ptest->peerTable1, ptest->hPeer);
	assert(err == dp_RES_OK);
	assert(ptest->peerTable1->vars->n_used == 0);

	/* Make sure delete_bySrc doesn't delete anything if there were
	 * no variables from that source.
	 */
	err = dptab_delete_bySrc(ptest->dt, ptest->table1, ptest->hPeer);
	assert(err == dp_RES_EMPTY);

	dpio_destroy(ptest->dpio, 0);

	/* Clean up. */
	key[0] = 1;
	err = dptab_deleteTable(ptest->dt, key, 1);
	assert(err == dp_RES_OK);

	key[0] = 1;
	err = dptab_deleteTable(ptest->dt, key, 1);
	assert(err == dp_RES_EMPTY);

	key[0] = 2;
	/* Verify that old table pointer still valid! */
	assert((ptest->table2->keylen == 1) && !memcmp(key, ptest->table2->key, 1));
	err = dptab_deleteTable(ptest->dt, key, 1);
	assert(err == dp_RES_OK);

	key[0] = 9;
	key[1] = 1;
	err = dptab_deleteTable(ptest->dt, key, 2);
	assert(err == dp_RES_OK);

	key[0] = 9;
	key[1] = 2;
	err = dptab_deleteTable(ptest->dt, key, 2);
	assert(err == dp_RES_OK);

	printf("dptabt1_destroy: done\n");
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

	if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v')
		Verbose = TRUE;

	for (i=0; i<2; i++) {
		dptabt1_t *t1, *t2;

		printf("Run %d.\n", i);

		t1 = dptabt1_create();
		t2 = dptabt1_create();
		assert(t1 && t2);
		dptabt1_peer_connect(t1, t2->dpio->myAdr);
		t1->idPeer = t2->id;
		dptabt1_peer_connect(t2, t1->dpio->myAdr);
		t2->idPeer = t1->id;
		dptabt1_send(t1);
		dptabt1_send(t2);

		do {
			running1 = dptabt1_poll(t1);
			running2 = dptabt1_poll(t2);
		} while (running1 || running2);

		dptabt1_destroy(t1);
		dptabt1_destroy(t2);
	}
	printf("No test failed.\n");

	return 0;
}
