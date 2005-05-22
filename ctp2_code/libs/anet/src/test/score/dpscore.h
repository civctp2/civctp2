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

#include "dp2.h"
#include "dptab.h"

/*----------------------------------------------------------------------
 Debugging routines. 
----------------------------------------------------------------------*/
#ifdef _DEBUG
static void dumpBuf(const char *buf, int len);
#else
#define dumpBuf(buf, len)
#endif

/*------------------ Startup / Shutdown -----------------------------------*/

/*--------------------------------------------------------------------------
 Call in dpSetGameServerEx to prepare client to report scores to server later.
 Creates myscore table.
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_init(dp_t *dp);

/*--------------------------------------------------------------------------
 Call in dpDestroy *before dpFreeze* (and in dpCloseGameServer()) to clean up.
 Don't want to save old scores to disk... oughtta be a way to mark
 tables as 'volatile' in dptab_createTable().
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_cleanup(dp_t *dp);

/*------------------ Score Reporting -----------------------------------*/

/*--------------------------------------------------------------------------
 Call this (from the player table callback dp_players_cb())
 whenever a player leaves the session.
 Sends a score record for the given player to the game server.
 If the player is ourselves, also sends scores for all other players.

 Return: dp_RES_BAD if score table doesn't exist or can't be created
         dp_RES_EMPTY if there are no score records found
         otherwise return status of dptab_addSubscriber(), dptab_set()
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_playerLeaving(dp_t *dp, dpid_t id);

/*-------------------------------------------------------------------------
 Report the score for dpid id to the comm layer, if it cares.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpReportScore(dp_t *dp, dpid_t id, long score);

/*-------------------------------------------------------------------------
 Begin a score report.  
 Flag must be zero.
 This should only be called at the end of the game (but before dpClose).
 
 Call dpReportScoreStart before calling dpReportScore2, then
 call dpReportScore2 to report as many scores as you like, then finally
 call dpReportScoreEnd to finish sending the block of scores.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreStart(dp_t *dp, int flag);

/*-------------------------------------------------------------------------
 Report that player dpId achieved a score of scoreVal in category scoreId.
 e.g. dpReportScore2(dp, dpId, dp_SCOREID_KILLS, 5) means player dpId
 scored 5 kills.

 Returns dp_RES_UNIMPLEMENTED if that score type is not supported by this
 driver; this is not really an error.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScore2(dp_t *dp, dpid_t dpId, int scoreId, long scoreVal);

/*-------------------------------------------------------------------------
 End a score report.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreEnd(dp_t *dp);

/*------------------ Score Retrieval -----------------------------------*/

/*----------------------------------------------------------------------
 Callback for incoming score tables.  Only used if application requested
 object deltas for this session's scores.
----------------------------------------------------------------------*/
int dp_PASCAL dpscores_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status);

/*--------------------------------------------------------------------------
 Request that the server send us score data for the given session type.
 Call from dpRequestObjectDeltas().
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_subscribe(dp_t *dp, dp_species_t sessType);
