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
 Client-side code to deal with score reporting.
 (See server/servscor.c for server-side code, and score/score*.c for
 shared code.)

 If there is a connection to an internet game server 
 (hGameServer != PLAYER_NONE), the scores are sent there
 whenever players leave.
 Otherwise, the scores are reported to the comm driver immediately
 (in case it's a third-party game network with its own score hooks).
 The routines dpScoreReportStart() and dpScoreReportEnd() might go away
 once we get things figured out better; they're there for Heat's benefit
 right now.

 Scores are identified by the session id of the game, and the user id
 of each player.  
 Scores are not reported for players that don't have user id's.
-----------------------------------------------------------------------*/

#ifdef WIN32
#include <crtdbg.h>
#define ASSERTMEM() assert(_CrtCheckMemory())
#else
#define ASSERTMEM()
#endif

#include "dp2.h"
#include "dptab.h"
#include "dpprivy.h"
#include "../score/scorerep.h"
#include "contract.h"
#include "eclock.h"

/*----------------------------------------------------------------------
 Debugging routines. 
----------------------------------------------------------------------*/
#ifdef _DEBUG
static void dumpBuf(const char *buf, int len)
{
	int i;
	for (i=0; i<len; i++) {
		DPRINT(("%02x ", 255 & buf[i]));
		if ((i % 24) == 23) {
			DPRINT(("\n"));
		}
	}
	DPRINT(("\n"));
}
#else
#define dumpBuf(buf, len)
#endif

/*------------------ Startup / Shutdown -----------------------------------*/

/*--------------------------------------------------------------------------
 Call in dpSetGameServerEx to prepare client to report scores to server later.
 Creates myscore table.
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_init(dp_t *dp)
{
	dp_result_t err;
	dptab_table_t *tab;
	char key[dptab_KEY_MAXLEN];

	ASSERTMEM();	
	DPRINT(("dpscore_client_init: creating myscores table\n"));
	/* Create outgoing scores table. */
	key[0] = dp_KEY_MYSCORES;
	err = dptab_createTable(dp->dt, &tab, key, 1, 0, NULL, NULL, NULL, NULL);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("dpscore_client_init: can't create myscore table, err:%d\n", err));
		return err;
	}
	dp->myscoretab = tab;

	/* Publish it to the game server */
	err = dptab_addSubscriber(dp->dt, tab, dp->hGameServer);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("dpscore_client_init: can't add server h:%x as subscriber?, err:%d\n", dp->hGameServer, err));
		return err;
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Call in dpDestroy *before dpFreeze* (and in dpCloseGameServer()) to clean up.
 Don't want to save old scores to disk... oughtta be a way to mark
 tables as 'volatile' in dptab_createTable().
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_cleanup(dp_t *dp)
{
	char key[dptab_KEY_MAXLEN];

	DPRINT(("dpscore_client_cleanup: deleting myscores and scores tables\n"));
	/* Delete outgoing scores */
	key[0] = dp_KEY_MYSCORES;
	dptab_deleteTable(dp->dt, key, 1);
	dp->myscoretab = NULL;  /* clear our quick access pointer */
	ASSERTMEM();
	
	/* Delete incoming scores.  KLUDGE; only does one session type... */
	/* Watch out- the entry in the publishers table contains a pointer
	 * to this table.  Possible crash bug if a record comes in
	 * from the game server after table is deleted.  Not sure if deleteTable
	 * wipes out the publishers table entry...
	 */
	key[0] = dp_KEY_SCORES;
	key[1] = dpGETSHORT_FIRSTBYTE(dp->defaultSessionType);
	key[2] = dpGETSHORT_SECONDBYTE(dp->defaultSessionType);
	dptab_deleteTable(dp->dt, key, 3);
	ASSERTMEM();

	return dp_RES_OK;
}

/*------------------ Score Reporting -----------------------------------*/

/*--------------------------------------------------------------------------
 Call this (from the player table callback dp_players_cb())
 whenever a player leaves the session.
 Sends a score record for us and the given player to the game server.
 If the player is ourselves, sends scores for all players.

 Return: dp_RES_BAD if score table doesn't exist or can't be created
         dp_RES_EMPTY if there are no score records found
         otherwise return status of dptab_addSubscriber(), dptab_set()
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_playerLeaving(dp_t *dp, dpid_t id)
{
	dp_result_t err = dp_RES_OK;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen = 0;
	int flags;
	dp_uid_t uid;
	dpid_t firstId;
	scorerep_buf_t sbuf;

	precondition(dp);
	uid = dpGetPlayerUid(dp, id);
	if (uid == dp_UID_NONE) {
		DPRINT(("dpscore_client_playerLeaving: no uid for id:%d\n", id));
		return dp_RES_BUG;
	}
	DPRINT(("dpscore_client_playerLeaving: id:%d uid:%d\n", id, uid));
	ASSERTMEM();

	if (!dp->myscoretab) {
		DPRINT(("dpscore_client_playerLeaving: no myscorestab\n"));
		return dp_RES_BUG;
	}

	if (dp->scorerep) {
		/* If (local player), set SELFEXIT to show we're leaving */
		/* We care because when local player leaves, we report on everyone */
		/* Use same code as dpEnumPlayers to detect local player */
		firstId = (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1));	
		flags = 0;
		if (firstId == dp->firstId)
			flags = scorerep_FLAGS_SELFEXIT;

		/* Hmm, what happens if someone creates > 1 players during a game? */
		err = scorerep_setLeaver(dp->scorerep, id, uid);
		if (err != dp_RES_OK) {
			DPRINT(("dpscore_client_playerLeaving: scorerep_setLeaver(id:%d, uid:%d) returns err:%d\n", id, uid, err));
			return dp_RES_BUG;
		}

		/* Grab the appropriate score data and put it into a buffer */
		err = scorerep_toBuf(dp->scorerep, flags, id, &sbuf);
		ASSERTMEM();
		if (err != dp_RES_OK) {
			DPRINT(("dpscore_client_playerLeaving: scorerep_toBuf returns err:%d\n", err));
			return dp_RES_BUG;
		}

		/* The score report key is the session id plus this player's dpid plus
		 * the id of the player who is leaving.
		 */
		assert(dp->sess_subkeylen + 4 <= dptab_KEY_MAXLEN);
		memcpy(subkey, dp->sess_subkey, dp->sess_subkeylen);
		ASSERTMEM();
		subkeylen = dp->sess_subkeylen;
		subkey[subkeylen++] = dpGETSHORT_FIRSTBYTE(dp->firstId);  /* KLUDGE */
		subkey[subkeylen++] = dpGETSHORT_SECONDBYTE(dp->firstId);
		subkey[subkeylen++] = dpGETSHORT_FIRSTBYTE(id);
		subkey[subkeylen++] = dpGETSHORT_SECONDBYTE(id);
		assert(subkeylen <= dptab_KEY_MAXLEN);
		err = dptab_set(dp->dt, dp->myscoretab, subkey, subkeylen, sbuf.buf, sbuf.len, 1, PLAYER_ME);
		ASSERTMEM();
		if (err != dp_RES_OK) {
			DPRINT(("dpscore_client_playerLeaving: dptab_set(MYSCORES.%s) returns err:%d\n", key2a(subkey, subkeylen), err));
			return err;
		}
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Report the score for dpid id to the comm layer, if it cares.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpReportScore(dp_t *dp, dpid_t id, long score)
{
#if 0
	commGroupAddReq_t req;

	dp_assertValid(dp);

	DPRINT(("dpReportScore: id %d, score %d\n", id, score));
	/* horrible kludge: abuse commGroupAdd function to report score. */
	/* if driver cares about score, and group is PLAYER_NONE, */
	/* then count is id's score. */
	req.group = PLAYER_NONE;
	req.players = (playerHdl_t *)&id;
	req.count = score;
	commGroupAdd(&req, NULL);
	(void) id;
	(void) dp;
	(void) score;
	dp_assertValid(dp);
#endif
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Begin a score report.  
 Flag must be zero.
 This should only be called at the end of the game (but before dpClose).
 
 Call dpReportScoreStart before calling dpReportScore2, then
 call dpReportScore2 to report as many scores as you like, then finally
 call dpReportScoreEnd to finish sending the block of scores.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreStart(dp_t *dp, int flag)
{
	precondition(dp);
	/* dp->score_flag = flag; */		/* no longer used? */

#if 0
	if (dp->hGameServer == PLAYER_NONE) {
		commSetParamReq_t   req;
		commSetParamResp_t  resp; /* This is a dummy variable */

		memset(&req, 0, sizeof(commSetParamReq_t));
		req.param_num = comm_PARAM_FINALREPORT;
		req.param_value = TRUE;	/* To begin score reporting */
		req.param_value2 = (long) flag;
		req.reqLen = sizeof(commSetParamReq_t);
		commSetParam(&req, &resp);
		return resp.status;
	}
#endif
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Old score reporting function, dummied out.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScore2(dp_t *dp, dpid_t dpId, int scoreId, long scoreVal)
{
	precondition(dp);
	precondition(dp->dpio);

	/* Needed to pass through comm settings like RXWAIT */
	if (dp->hGameServer == PLAYER_NONE) {
		commSetParamReq_t   req;
		commSetParamResp_t  resp; /* This is a dummy variable */

		memset(&req, 0, sizeof(commSetParamReq_t));
		req.param_num = scoreId;
		req.param_value = scoreVal;
		/* set a playerHdl to param_value2 base on the given dpId */
		req.param_value2 = (long)dpid2commHdl(dp, dpId); 
		req.reqLen = sizeof(commSetParamReq_t);
		commSetParam(&req, &resp, dp->dpio->commPtr);
		/* Driver usually returns status dp_RES_OK or dp_RES_UNIMPLEMENTED */
		DPRINT(("dpReportScore2: driver reports err:%d for scoreId %d\n",
				resp.status, scoreId));
		return resp.status;  
	}
	return dp_RES_OK;
}


/*-------------------------------------------------------------------------
 Report the score(s) for player dpId.  

 The idea is to pack all the score info of interest about a particular player
 into a compact buffer, then call this function.  This should be done
 periodically, e.g. after every major event.  

 You should use the first two bytes for a rough 'score', most significant
 byte first, and the third byte should be 0 normally, and 1 if this player
 has 'won'.
 e.g.
	buf[0] = dpGETSHORT_FIRSTBYTE(score);
	buf[1] = dpGETSHORT_SECONDBYTE(score);
	buf[2] = 0;
 Additional details can be appended.  You should write up a spec on how
 to interpret these bytes, and provide it to the game server administrators.

 scoreId should be zero.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreBuf(dp_t *dp, dpid_t dpId, int scoreId, const char *scorebuf, int scorelen)
{
	dp_uid_t uid;
	dp_result_t err;

	precondition(dp);
	ASSERTMEM();

#if 0
	if (dp->hGameServer == PLAYER_NONE) {
		commSetParamReq_t   req;
		commSetParamResp_t  resp; /* This is a dummy variable */

		memset(&req, 0, sizeof(commSetParamReq_t));
		req.param_num = scoreId;
		req.param_value = scoreVal;
		/* set a playerHdl to param_value2 base on the given dpId */
		req.param_value2 = (long)dpid2commHdl(dp, dpId); 
		req.reqLen = sizeof(commSetParamReq_t);
		commSetParam(&req, &resp);
		/* Driver usually returns status dp_RES_OK or dp_RES_UNIMPLEMENTED */
		DPRINT(("dpReportScoreBuf: driver reports err:%d for scoreId %d\n",
				resp.status, scoreId));
		return resp.status;  
	}
#endif

	uid = dpGetPlayerUid(dp, dpId);
	ASSERTMEM();
	if (uid == dp_UID_NONE) {
		DPRINT(("dpReportScoreBuf: player %d had no uid.  Not reporting.\n", dpId));
		return dp_RES_OK;
	}
	
	/* Create the score matrix for this session, if needed, and register our
	 * player and uid
	 */
	if (!dp->scorerep) {
		dpid_t mydpid;
		dp_uid_t myuid;
		dp_session_t sess;
		
		dp->scorerep = scorerep_create();
		if (!dp->scorerep)
			return dp_RES_NOMEM;

		/* Identify us as any player created on this machine */
		err = dpGetSessionDesc(dp, &sess, NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dpReportScoreBuf: can't get session desc, err:%d\n", err));
			return err;
		}
		dp->scorerep->sessType = sess.sessionType;
		mydpid = dpGetMyId(dp);
		ASSERTMEM();
		myuid = tserv_hdl2uid(dp->tserv, PLAYER_ME);
		ASSERTMEM();
		DPRINT(("dpReportScoreBuf: first report; I am id:%d, uid:%d\n", mydpid, myuid));
		scorerep_setSelf(dp->scorerep, mydpid, myuid);
		ASSERTMEM();
	}

	/* Place this score into the matrix */
	DPRINT(("dpReportScoreBuf: Reporting score for id:%d uid:%d\n", dpId, uid));
	err = scorerep_set(dp->scorerep, dpId, uid, scoreId, scorebuf, scorelen);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("dpReportScoreBuf: scorerep_set returns err:%d\n", err));
		return err;
	}
	
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 End a score report.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreEnd(dp_t *dp)
{
	precondition(dp);

#if 0
	if (dp->hGameServer == PLAYER_NONE) {
		commSetParamReq_t   req;
		commSetParamResp_t  resp; /* This is a dummy variable */

		memset(&req, 0, sizeof(commSetParamReq_t));
		req.param_num = comm_PARAM_FINALREPORT;
		req.param_value = FALSE;	/* To end score reporting */
		req.reqLen = sizeof(commSetParamReq_t);
		commSetParam(&req, &resp);
		return resp.status;  
	}
#endif
	return dp_RES_OK;
}

/*------------------ Score Retrieval -----------------------------------*/

/*----------------------------------------------------------------------
 Callback for incoming score tables.  Only used if application requested
 object deltas for this session's scores.
----------------------------------------------------------------------*/
int dp_PASCAL dpscores_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dp_result_t err;

	/* Poor user has to unpack our representation himself */

	/* Inform caller via local message, if desired */
	err = dpSendObjectDelta(dp, status, (dp_object_t *) buf, table, subkey, subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dpscores_cb: can't send object delta, err:%d\n", err));
		return err;
	}
	return 0;
}


/*--------------------------------------------------------------------------
 Request that the server send us score data for the given session type.
 Call from dpRequestObjectDeltas().
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_subscribe(dp_t *dp, dp_species_t sessType)
{
	dp_result_t err;
	dptab_table_t *tab;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	ASSERTMEM();
	
	/* Create incoming scores table. */
	key[0] = dp_KEY_SCORES;
	key[1] = dpGETSHORT_FIRSTBYTE(sessType);
	key[2] = dpGETSHORT_SECONDBYTE(sessType);
	keylen = 3;
	err = dptab_createTable(dp->dt, &tab, key, keylen, 0, NULL, NULL, dpscores_cb, dp);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("dpscore_client_subscribe: can't create scores table, err:%d\n", err));
		return err;
	}
	/* Allow the game server to send us data on this table. */
	err = dptab_addPublisher(dp->dt, tab, key, keylen, dp->hGameServer);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("dpscore_client_subscribe: can't add hGameServer h:%x as publisher?, err:%d\n", dp->hGameServer, err));
		return err;
	}

	/* Request the game server to send us data on this table. 
	 * For now, ask for the whole table; later, we'll ask for just
	 * part of it (otherwise we'll drown in data).
	 */
	err = dptab_requestSubscription(dp->dt, key, keylen, dp->hGameServer, NULL, NULL);
	ASSERTMEM();
	if (err != dp_RES_OK) {
		DPRINT(("dpscore_client_subscribe: can't request scores from hGameServer h:%x, err:%d\n", dp->hGameServer, err));
		return err;
	}
	ASSERTMEM();

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Retrieve a player's dpid.
--------------------------------------------------------------------------*/
dpid_t dpGetMyId(dp_t *dp)
{
	dp_result_t err;
	void *buf;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	
	err = dptab_get_byindex(dp->myplayers, 0, &buf, &len, subkey, &subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dpGetMyId: no player created yet?\n"));
		return dp_ID_NONE;
	}
	return (dpid_t)dpMAKESHORT(subkey[0], subkey[1]);
}
