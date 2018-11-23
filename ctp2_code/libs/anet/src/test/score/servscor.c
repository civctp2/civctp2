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

/* Score handling stuff. */

#include "servscor.h"
#include "dpprivy.h"
#include "wmq.h"

wmq_t *wmq;
dptab_table_t *myscoretab;

/*----------------------------------------------------------------------
 Callback to handle score reports from client.
----------------------------------------------------------------------*/
static int dp_PASCAL scores_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dp_result_t err;

	scorerep_buf_t sbuf;
	scorerep_t *srep;
	dp_uid_t uid;
	char sessid[dptab_KEY_MAXLEN];
	int sessidlen;
	dp_species_t sessType;

	if (!dp || !buf || (status != dp_RES_CREATED))
		return 0;
	DPRINT(("scores_cb: new score report; size %d\n", total));

	/* Look up uid of handle reporting score */
	uid = tserv_hdl2uid(dp->tserv, src);
	if (uid == dp_UID_NONE) {
		DPRINT(("scores_cb: can't map h:%x to uid\n", src));
		return 0;
	}
	/* Get id and type of session that uid most recently tried to join */
	err = dp_uid2sessid(dp, uid, sessid, &sessidlen, &sessType);
	if (err != dp_RES_OK) {
		DPRINT(("scores_cb: can't map uid:%d to session\n", uid));
		return 0;
	}

	/* Write out incoming score record to web message queue,
	 * preceded by header:
	 *	char sessid[8];		// inetadr + port + karma
	 *	dp_karma_t sessType;
	 *	unsigned short bloblen;
	 */
	{
		char buf[1024];
		int off;

		memcpy(buf, sessid, sessidlen);
		off = sessidlen;
		buf[off++] = dpGETSHORT_FIRSTBYTE(sessType);
		buf[off++] = dpGETSHORT_SECONDBYTE(sessType);
		buf[off++] = dpGETSHORT_FIRSTBYTE(total);
		buf[off++] = dpGETSHORT_SECONDBYTE(total);
		assert(off + total < sizeof(buf));
		memcpy(buf+off, buf, total);
		off += total;

		err = wmq_put(wmq, time(NULL), buf, off);
		assert(err == dp_RES_OK);
	}

	/* Delete the raw score report!  Causes another callback here, ignored. */
	dptab_delete(dptab, table, subkey, subkeylen);

	return 0;
}

/*--------------------------------------------------------------------------
 Call this once when the server is starting up.
--------------------------------------------------------------------------*/
void servscor_init(dp_t *dp)
{
	dp_result_t err;
	char key[dptab_KEY_MAXLEN];
	assoctab_t *types;
	int i;

	/* Create table to receive reports from clients. */
	key[0] = dp_KEY_MYSCORES;
	err = dptab_createTable(dp->dt, &myscoretab, key, 1, 0, NULL, NULL, scores_cb, dp);
	if (err != dp_RES_OK && err != dp_RES_ALREADY) {
		printf("servscor_init: Can't init myscoretab.\n");
		exit(1);
	}

	wmq = wmq_create("wmq", 1);
	if (!wmq) {
		printf("servscor_init: Can't init wmq.\n");
		exit(1);
	}
}

/*--------------------------------------------------------------------------
 Call once a minute or so to update all cumulative scores.
--------------------------------------------------------------------------*/
void servscor_poll(dp_t *dp)
{
	wmq_flush(wmq);
}

/*--------------------------------------------------------------------------
 Call when a new client connects to the server.
--------------------------------------------------------------------------*/
void servscor_addClient(dp_t *dp, playerHdl_t h)
{
	dp_result_t err;
	char key[dptab_KEY_MAXLEN];

	/* Let the user report scores to us */
	err = dptab_addPeer(dp->dt, h);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		DPRINT(("servscor_addClient: can't addPeer, err:%d\n", err));
		return;
	}
	if (!myscoretab) {
		DPRINT(("servscor_addClient: no score table?\n"));
		return;
	}
	key[0] = dp_KEY_MYSCORES;
	err = dptab_addPublisher(dp->dt, myscoretab, key, 1, h);
	if (err != dp_RES_OK) {
		DPRINT(("servscor_addClient: can't add h:%x as publisher?, err:%d\n", h, err));
		return;
	}
}
