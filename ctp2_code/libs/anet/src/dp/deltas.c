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

#include "dp2.h"
#include "dpprivy.h"
#include "contract.h"

/*----------------------------------------------------------------------
 Callback for remote player tables.  Only used if application requested
 object deltas for this session's players.
----------------------------------------------------------------------*/
int dp_PASCAL dp_rplayers_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dp_playerId_t player;
	dp_result_t err;
	dpid_t id;
	size_t len_used;

	/* current table handled specially in dp_player_notify */
	if (table == dp->players)
		return 0;

	memset(&player, 0, sizeof(player));
	id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
	len_used = dp_unpack_playerId(id, buf, &player);
	if (total != len_used) {
		DPRINT(("dp_rplayers_cb: err: can't unpack player %d (%d != %d).\n", id, total, len_used));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	DPRINT(("dp_rplayers_cb: status %d, name %s\n", status, player.name));

	/* Inform caller via local message, if desired */
	err = dpSendObjectDelta(dp, status, (dp_object_t *) &player, table, subkey, subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dp_rplayers_cb: can't send object delta, err:%d\n", err));
		return err;
	}

	return 0;
}

/* gee, we could use the existing dp_sessionContext_t, but it's easier to
 * build this.
 */
typedef struct {
	dp_t *dp;
	dptab_table_t *tab;
} dp_rplayers_enumEx_context_t;

/*----------------------------------------------------------------------
 Callback for remote enum players.  Only used if application requested
 object deltas for this session's players.
----------------------------------------------------------------------*/
void dp_PASCAL dp_rplayers_enumEx_cb(dpid_t id, char *name, long flags, void *context, dp_playerId_t *player)
{
	dp_rplayers_enumEx_context_t *r = (dp_rplayers_enumEx_context_t *)context;
	dp_result_t err;
	char subkey[2];
	int subkeylen;

	if (!r || !r->dp || !r->tab)
		return;
	if (player == NULL)
		return;

	/* Inform caller via local message, if desired */
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(player->id);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(player->id);
	err = dpSendObjectDelta(r->dp, dp_RES_CREATED, (dp_object_t *) player, r->tab, subkey, subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dp_rplayers_enumEx_cb: can't send object delta, err:%d\n", err));
		return;
	}

	return;
}

/*------------------------------------------------------------------------
 Start or stop monitoring the given object table for changes.
 Initially, and on any change to the given table, a dp_objectDelta_packet_t
 is generated and placed in a queue for retrieval with dpReceive.
 See anet.h for the definition of dp_objectDelta_packet_t.

 When called with monitor=TRUE, a burst of messages are generated giving 
 the initial contents of the table.
 When called with monitor=FALSE, no more messages of that sort will
 be generated, although there may still be some in the queue; you can
 call dpReceive until it returns empty to flush these out if desired.

 The key argument is a variable-length binary string that indicates
 what objects to start or stop monitoring.

 To start or stop monitoring sessions, use 
	keylen=1, key[0] = dp_KEY_SESSIONS
 To stop monitoring servers, use 
	keylen=1, key[0] = dp_KEY_SERVERPINGS,
 To start monitoring servers, use 
 	keylen=3;
	key[0] = dp_KEY_SERVERPINGS,
	key[1] = (char) dpGETSHORT_FIRSTBYTE(sessiontype);
	key[2] = (char) dpGETSHORT_SECONDBYTE(sessiontype);
 To monitor players, use
    char key[dp_MAX_KEYLEN+1];
	key[0] = dp_KEY_PLAYERS;
	dpGetSessionId(dp, &sess, &key[1], &keylen);
	keylen++;
 To request that latencies be included in player deltas for the current 
	session, use
	keylen = 1, key[0] = dp_KEY_PLAYER_LATENCIES;
	The latency in milliseconds will be placed in the latency field of
	the object delta packet.
	The frequency of latency measurements is influenced by the intervals
	set with dpSetPingIntervals.
	Deltas are synthesized locally and do not load the network.

 Note: keylen will be dp_MAX_KEYLEN+1 when requesting player deltas
 in IPX sessions!
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpRequestObjectDeltas(
	dp_t *dp,
	int monitor,		/* TRUE to start, FALSE to stop */
	const char *key,
	int keylen)
{
	dp_result_t err;
	precondition(dp);
	precondition(key);
	precondition((keylen > 0) && (keylen <= dp_KEY_MAXLEN+1));

	DPRINT(("dpRequestObjectDeltas(,%d,%s)\n",
			monitor, key2a(key, keylen)));

	switch (key[0]) {
	case dp_KEY_SESSIONS:
		if (keylen != 1) {
			DPRINT(("dpRequestObjectDeltas: sessions: can't specify session type yet\n"));
			return dp_RES_BAD;
		}
		if (dp->monitor_object_sessions && monitor) 
			return dp_RES_ALREADY;
		/* Cause dp_sessions_cb to generate new messages */
		dp->monitor_object_sessions = monitor;
		if (monitor) {
			dptab_table_t *sesstab;
			int i;

			/* enumerate the existing sessions */
			/* If we're a standalone master, enum the mysessions table */
			if (bIAmStandaloneMaster(dp))
				sesstab = dp->mysessions;
			else
				sesstab = dp->sessions;
			DPRINT(("dpRequestObjectDeltas: table size %d\n", dptab_tableSize(sesstab)));
			for (i=0; i<dptab_tableSize(sesstab); i++) {
				char *s;
				dp_session_t sess;
				size_t len;
				char subkey[dptab_KEY_MAXLEN];
				int subkeylen;

				err = dptab_get_byindex(sesstab, i, (void **)&s, &len, subkey, &subkeylen);
				if (err != dp_RES_OK) {
					DPRINT(("dpRequestObjectDeltas: bug?\n"));
					dp_assertValid(dp);
					break;
				}
				/* Must unpack when retrieving records from dptab */
				dp_unpack_session(dp, subkey, subkeylen, s, len, &sess);
				err = dpSendObjectDelta(dp, dp_RES_CREATED, (dp_object_t *) &sess, sesstab, subkey, subkeylen);
				if (err != dp_RES_OK) {
					DPRINT(("dpRequestObjectDeltas: can't send object delta, err:%d\n", err));
				}
			}
		}
		return dp_RES_OK;
		break;

	case dp_KEY_SERVERPINGS:
		if (dp->monitor_object_servers && monitor) return dp_RES_ALREADY;
		/* Cause dpHandleServerPingResponsePacket to generate new messages */
		dp->monitor_object_servers = monitor;
		dp->serverping_interval = dp->dpio->clocksPerSec;
		dp->next_serverping = dp->now;
		dp->serverping_rx_count = 0;
		dp->serverping_rx_count_old = 0;
		if (monitor) {
			int i;

			if (keylen != 3) {
				DPRINT(("dpRequestObjectDeltas: must specify session type.\n"));
				return dp_RES_BAD;
			}
			dp->monitor_object_servers_sessType = dpMAKESHORT(key[1], key[2]);
			DPRINT(("dpRequestObjectDeltas: servers_sessType %d\n",dp->monitor_object_servers_sessType));
			
			/* enumerate the existing servers */
			for (i=0; i<dptab_tableSize(dp->serverpings); i++) {
				dp_serverInfo_t *server;
				size_t len;
				char subkey[dptab_KEY_MAXLEN];
				int subkeylen;

				err = dptab_get_byindex(dp->serverpings, i, (void **)&server, &len, subkey, &subkeylen);
				if (err != dp_RES_OK) {
					DPRINT(("dpRequestObjectDeltas: bug?\n"));
					dp_assertValid(dp);
					break;
				}
				err = dpSendObjectDelta(dp, dp_RES_CREATED, (dp_object_t *) server, dp->serverpings, subkey, subkeylen);
				if (err != dp_RES_OK) {
					DPRINT(("dpRequestObjectDeltas: can't send object delta, err:%d\n", err));
				}
			}
		}
		return dp_RES_OK;
		break;

	case dp_KEY_PLAYERS: 
		{
		dp_session_t sDesc;
		void *rplayers_context;
		dp_rplayers_enumEx_context_t rpecontext;
		size_t slen;
		dptab_table_t *rplayers;
		char pkey[3];
		int pkeylen;

		precondition(keylen == dp->dpio->myAdrLen + 3);
		pkeylen = 0;
		pkey[pkeylen++] = dp_KEY_PLAYERS;
		pkey[pkeylen++] = key[dp->dpio->myAdrLen+1];
		pkey[pkeylen++] = key[dp->dpio->myAdrLen+2];
		rplayers_context = NULL;
		if ((rplayers = dptab_getTable(dp->dt, pkey, pkeylen)))
			rplayers_context = dptab_getTableContext(rplayers, dp_rplayers_cb);
		if (!monitor) {
			/* caller no longer wants object delta messages for this table */
			if (!rplayers_context)
				return dp_RES_ALREADY;
			dptab_clearTableCallback(rplayers, dp_rplayers_cb);
			/* bug: should cancel subscription if remote */
			return dp_RES_OK;
		}
		/* caller wants to start getting object delta messages for this table */
		if (rplayers_context)
			return dp_RES_ALREADY;
		slen = sizeof(sDesc);
		err = dpGetSessionDescById(dp, key+1, &sDesc, &slen);
		if (err != dp_RES_OK) {
			DPRINT(("dpRequestObjectDeltas: dpGetSessionById returns %d\n", err));
			return err;
		}
		rpecontext.dp = dp;
		rpecontext.tab = rplayers;
		err = dpEnumPlayersEx(dp, &sDesc, dp_rplayers_enumEx_cb, &rpecontext, 0);
		if (err != dp_RES_OK) {
			DPRINT(("dpRequestObjectDeltas: dpEnumPlayers returns %d\n", err));
			return err;
		}
		rplayers = dptab_getTable(dp->dt, pkey, pkeylen);
		assert(rplayers);
		err = dptab_setTableCallback(rplayers, dp_rplayers_cb, dp);
		if (err != dp_RES_OK) {
			DPRINT(("dpRequestObjectDeltas: dptab_setTableCallback returns %d\n", err));
			return err;
		}
		return dp_RES_OK;
		}
		break;

	case dp_KEY_PLAYER_LATENCIES:
		if (keylen != 1) {
			DPRINT(("dpRequestObjectDeltas: player_latencies: bad keylen:%d != 1\n", keylen));
			return dp_RES_BAD;
		}
		dp->monitor_player_latencies = (monitor) ? TRUE : FALSE;
		return dp_RES_OK;
		break;
		
	case dp_KEY_SCORES:
		/* For the moment, only allow the default session type */
		if (keylen != 1) {
			DPRINT(("dpRequestObjectDeltas: scores: can't specify session type yet\n"));
			return dp_RES_BAD;
		}
		err = dpscore_client_subscribe(dp, dp->defaultSessionType);
		if (err != dp_RES_OK) {
			DPRINT(("dpRequestObjectDeltas: dpscore_client_subscribe returns err:%d\n", err));
			return dp_RES_BUG;
		}
		return dp_RES_OK;
		break; 
	default:
		;
	}

	DPRINT(("dpRequestObjectDeltas: unknown request\n"));
	return dp_RES_BAD;
}

/*------------------------------------------------------------------------
 Convert a session description to its unique id (aka key).
 key must put to a buffer of length dp_KEY_MAXLEN;
 the session's id will be copied to this buffer.
 *pidlen will be filled with the number of bytes of id used.

 Session ids are unique, and do not change during the duration of
 a game session.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetSessionId(
	dp_t *dp,
	const dp_session_t *sess,	/* session to convert */
	char *id,					/* resulting id stored here */
	int *pidlen)				/* length of resulting id stored here */
{
	precondition(dp);
	precondition(dp->dpio);
	precondition(sess);
	precondition(id);
	precondition(pidlen);

	/* Id is original master's address plus the session karma,
	 * and is stored in reserved2.
	 */
	*pidlen = dp->dpio->myAdrLen + sizeof(dp_karma_t);
	memcpy(id, sess->reserved2, *pidlen);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 A change happened in one of the tables the user was monitoring.
 Inform them.
----------------------------------------------------------------------*/
dp_result_t dpSendObjectDelta(
	dp_t *dp,
	dp_result_t status,
	dp_object_t *data,
	dptab_table_t *tab,
	char *subkey,
	int subkeylen)
{
	playerHdl_t dest;
	size_t pktlen;
	size_t datalen;
	dp_result_t err;
	struct {
		dp_packetType_t   tag PACK;
 		dp_objectDelta_packet_t body PACK;
	} pkt;

	precondition(dp);
	precondition(data);
	precondition(tab);
	precondition(subkeylen);

	dp_assertValid(dp);

	memset(&pkt, 0, sizeof(pkt));

	/* DPRINT(("dpSendObjectDeltaPacket: status e:%d, key %s, subkey %s, name ",status, key2a(tab->key, tab->keylen), key2a2(subkey, subkeylen))); */
	DPRINT(("dpSendObjectDeltaPacket: status e:%d, ",status));
	switch(tab->key[0]) {
	case dp_KEY_PLAYERS: {
		if (tab == dp->players) {
			dpid_t id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
			if (dp->monitor_player_latencies) {
				playerHdl_t h = dpid2commHdl(dp, id);

				if (h != PLAYER_NONE) {
					/* Stuff latency into delta for players in our session */
					int		loss;
					int		latency;

					latency = dpio_get_player_latency(dp->dpio, h, 0, &loss);
					if (latency > 32767)
						latency = 32767;

					pkt.body.latency = latency;
					pkt.body.pktloss = loss;
				}
			}

			pkt.body.flags |= dp_OBJECTDELTA_FLAG_INOPENSESS;
			if (id >= dp->firstId && id < dp->firstId + dp_PLAYERS_PER_HOST)
				pkt.body.flags |= dp_OBJECTDELTA_FLAG_LOCAL;
			if (dp_commHdl2dpid(dp, dp->hMaster) == id) 
				pkt.body.flags |= dp_OBJECTDELTA_FLAG_ISHOST;
		}
		datalen = sizeof(dp_playerId_t);
		DPRINT(("playername:%s ping:%d loss:%d", data->p.name, pkt.body.latency, pkt.body.pktloss));
		break; 
	}
	case dp_KEY_SESSIONS:
		if (dp->players && !memcmp(data->sess.reserved2, dp->sess_subkey, dp->sess_subkeylen))
			pkt.body.flags |= dp_OBJECTDELTA_FLAG_INOPENSESS;
		if (!memcmp(data->sess.adrMaster, dp->dpio->myAdr, dp->dpio->myAdrLen))
			pkt.body.flags |= dp_OBJECTDELTA_FLAG_LOCAL;
		datalen = sizeof(dp_session_t); 
		DPRINT(("sessname %s, flags %x", data->sess.sessionName, data->sess.flags));
		break; 
	case dp_KEY_SERVERPINGS:
		datalen = sizeof(dp_serverInfo_t);
		DPRINT(("hostname %s", data->serv.hostname));
		break; 
	case dp_KEY_SCORES:
		datalen = sizeof(data->score.nScoreTypes) 
			+ data->score.nScoreTypes * ( sizeof(data->score.scoreIds[0]) + 
										  sizeof(data->score.scores[0]) );
		DPRINT(("uid %d", dpMAKELONG(subkey[0], subkey[1], subkey[2], subkey[3])));
		break;
	default:
		return dp_RES_BAD;
	}
	DPRINT(("\n"));

	pkt.tag = dp_OBJECTDELTA_PACKET_ID;
	pkt.body.status = status;
	pkt.body.keylen = tab->keylen;
	memcpy(pkt.body.key, tab->key, tab->keylen);
	pkt.body.subkeylen = subkeylen;
	memcpy(pkt.body.subkey, subkey, subkeylen);
	memcpy(&pkt.body.data, data, datalen);

	dest = PLAYER_ME;
	pktlen = sizeof(pkt) - sizeof(pkt.body.data) + datalen;

	err = dpio_put_reliable(dp->dpio, &dest, 1, &pkt, pktlen, NULL);
	dp_assertValid(dp);
	return err;
}



