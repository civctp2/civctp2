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

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
/* Convert a binary buffer to hex notation.  Don't use twice in one DPRINT! */
static const char *hexstring(const unsigned char *binstr, int len)
{
	static char buf[768];
	int i;
	if (len < 1) return "";
	for (i = 0; i < len && i < 256; i++)
		sprintf(buf + 3*i, "%02x ", binstr[i]);
	buf[3*i-1] = '\0';
	return buf;
}
#endif

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
		char wmqbuf[1024];
		int wmqbuflen;

		memcpy(wmqbuf, sessid, sessidlen);
		wmqbuflen = sessidlen;
		wmqbuf[wmqbuflen++] = dpGETSHORT_FIRSTBYTE(sessType);
		wmqbuf[wmqbuflen++] = dpGETSHORT_SECONDBYTE(sessType);
		wmqbuf[wmqbuflen++] = dpGETSHORT_FIRSTBYTE(total);
		wmqbuf[wmqbuflen++] = dpGETSHORT_SECONDBYTE(total);
		assert(wmqbuflen + total < sizeof(wmqbuf));
		memcpy(wmqbuf + wmqbuflen, buf, total);
		wmqbuflen += total;

		err = wmq_put(wmq, time(NULL), wmq_RECORDTAG_SCORE, wmqbuf, wmqbuflen);
		if (err != dp_RES_OK)
			DPRINT(("scores_cb: wmq_put returns err:%d\n", err));
		assert(err == dp_RES_OK);
	}

	/* Delete the raw score report!  Causes another callback here, ignored. */
	dptab_delete(dptab, table, subkey, subkeylen);

	return 0;
}

/*--------------------------------------------------------------------------
 Call this once when the server is starting up.
--------------------------------------------------------------------------*/
void servscor_init(dp_t *dp, const char *wmqDirectory)
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

	wmq = wmq_create(wmqDirectory, 1);
	if (!wmq) {
		printf("servscor_init: Can't init wmq in dir:%s.\n", wmqDirectory);
		exit(1);
	}
	err = wmq_seek(wmq, time(NULL), 0);
	if (err != dp_RES_OK) {
		printf("servscor_init: wmq_seek returns err:%d.\n", err);
		exit(1);
	}
	/* set serverTag to our IP addr, including port */
	err = wmq_setServerTag(wmq, dp->dpio->myAdr, dp->dpio->myAdrLen);
	if (err != dp_RES_OK) {
		printf("servscor_init: wmq_setServerTag returns err:%d.\n", err);
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
