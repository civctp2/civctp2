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

/* Example game server */
#include "dp.h"
#include "eclock.h"
#include "../../demo/utils/raw.h"

#include <stdio.h>
#include <assert.h>

char *masterServerHostname = NULL;	/* NULL = we are a master server ourself */

/*-------------------------------------------------------*/
/* Convert a key to ASCII for debug printing */
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
/* Convert a key to ASCII for debug printing */
static char key2a_buf[256];
static char key2a_buf2[256];
/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)
/*-------------------------------------------------------*/

/* Print out incoming variable adds or deletes. */
int dp_PASCAL print_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t err)
{
	dptab_table_t *sessions = (dptab_table_t *)context;

	printf("print_cb: src h:%x, dest h:%x, table %s, subkey %s, err %d\n", 
			src, dest, 
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen), 
			err);

	return TRUE;
}

/* When a new client comes online, 
 * send it our SESSIONS table, 
 * and when it sends us its MYSESSIONS table, put it in our MYSESSIONS table.
 * (If we're the master gameserver, dump it into the sessions table
 * instead.)
 */
void addClient(dp_t *dp, playerHdl_t src)
{
	dp_result_t err;
	dptab_table_t *target;

	target = masterServerHostname ? dp->mysessions: dp->sessions;

	DPRINT(("addClient: h:%x; sucking his MYSESSIONS into my table %s\n",
			src, key2a(target->key, target->keylen)));

	err = dptab_addPeer(dp->dt, src);
	if (err != dp_RES_OK) {
		DPRINT(("addClient: can't addPeer, err:%d\n", err));
		return;
	}

	/* Send our session table to the new client. */
	err = dptab_addSubscriber(dp->dt, dp->sessions, src);
	if (err != dp_RES_OK) {
		DPRINT(("addClient: can't addSubscriber, err:%d\n", err));
		return;
	}

	/* Accept the new client's mysessions table, dump it in to
	 * the 'target' table (different for master and slave servers)
	 */
	err = dptab_addPublisher(dp->dt, target,
			dp->mysessions->key, dp->mysessions->keylen, src);
	if (err != dp_RES_OK) {
		DPRINT(("addClient: can't addPublisher, err:%d\n", err));
		return;
	}
}

main(int argc, char **argv)
{
	dp_t *mydp;
	dp_transport_t theTransport;
	dp_result_t err;
	clock_t started;
	char *logfname;
	char *drivername;

	if (argc == 4) {
		masterServerHostname = argv[2];
		printf("We are a slave server.\n");
	} else if (argc == 3) {
		masterServerHostname = NULL;
		printf("We are a master server.\n");
	} else {
		printf("Usage: dp2t2 driver logfname [masterservername]\n");
		exit(1);
	}
	drivername = argv[1];
	logfname = argv[2];
	dp_setLogFname(logfname);

	memset(&theTransport, 0, sizeof(theTransport));
	strcpy(theTransport.fname, drivername);
	err = dpCreate(&mydp, &theTransport, NULL, NULL);
	if (err != dp_RES_OK) {
		printf("Can't create dp, err:%d\n", err);
		exit(1);
	}

	/* If we're not the master game server, log our server in as a client
	 * of it.
	 */
	if (masterServerHostname) {
		printf("Logging in to master game server %s\n", masterServerHostname);
		err = dpSetGameServer(mydp, masterServerHostname);
		assert(err == dp_RES_OK);
	}

	/* Whenever a variable comes in, print it. */
	dptab_setTableCallback(mydp->mysessions, print_cb, NULL);
	dptab_setTableCallback(mydp->sessions, print_cb, NULL);

	/* Wait for login requests.  If the request is ok, 
	 * add a subscription for the sessions table.
	 */
	started = eclock();
	while ((long)(eclock() - started) < (40 * ECLOCKS_PER_SEC)) {
		dpid_t idTo;
		char pkt[512];
		size_t size;
		playerHdl_t src;

		/* Set flags so sender's address is a playerHdl_t, not a dpid_t */
		size = sizeof(pkt);
		err = dpReceive(mydp, (dpid_t *)&src, &idTo, 1, &pkt, &size);
		assert ((err == dp_RES_EMPTY) || (err == dp_RES_OK));
		if (err == dp_RES_OK) {
			dp_packetType_t id = *(short *)pkt;
			switch (id) {
			case dppt_MAKE('e','1'):
				printf("server: Got add request from h:%x.\n", src);
				addClient(mydp, src);
				/* Note: dp will call deletePeer automatically if hdl closes */
				break;
			default:
				;
			}
		}
		if (raw_kbhit()) {
			raw_getc();
			printf("Disconnecting from upper server at user request\n");
			DPRINT(("Disconnecting from upper server at user request\n"));
			/* disconnect from the game server abruptly. */
			dpSetGameServer(mydp, NULL);
		}
	}

	err = dpDestroy(mydp, 0);
	assert(err == dp_RES_OK);

	printf("No test failed.\n");

	return 0;
}
