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

/*--------------------------------------------------------------------------
 Private functions exported from dp.c to other dp modules.

 $Log: dpprivy.h $
 Revision 1.9  1997/08/19 22:03:57  dkegel
 Added one more swap function temporarily as a define...
 Revision 1.8  1997/06/01 00:52:53  dkegel
 Use right ifdef for power pc.
 Revision 1.7  1997/05/28 17:37:35  anitalee
 Added compatibility with big-endian machines.
 Revision 1.6  1997/05/15 22:05:35  anitalee
 Fixed clock() to work under Sun's Solaris 2.x.
 Revision 1.5  1997/04/25 18:20:12  dkegel
 Moved dpHigherLevelBlanketAck to dpprivy.h from anet.h.
 Revision 1.4  1997/03/09 03:26:21  dkegel
 Added access to dpid2commHdl.
 Revision 1.3  1997/03/07 22:58:13  dkegel
 Added dp_broadcast_unreliable to avoid forward reference.
 Revision 1.2  1997/02/23 05:35:41  dkegel
 Export dp_getBroadcastHdls for benefit of dppv.
 Revision 1.1  1997/02/12 03:06:31  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef dpprivy_h
#define dpprivy_h

/*------------------------------------------------------------------------
 Returns temporary directory for the local machine.
------------------------------------------------------------------------*/
int getTempDir(char *name, int namelen);

/*------------------------------------------------------------------------
 Write/read size bytes to/from dat from/to *bp, incrementing *bp by size.
 Byteswaps if Sparc and PowerMac and do nothing otherwise.
------------------------------------------------------------------------*/
void writeSwap(void **bp, const void *dat, unsigned int size);
void readSwap(const void **bp, void *dat, unsigned int size);

#ifndef NO_NETWORK	/* what the hell is this? */
/*-----------------------------------------------------------------------
 Swaps bytes on sent and received packets for Sparc and PowerMac and
 do nothing otherwise.
 Some functions have a second integer argument giving the correct array
 size, since the value from the structure may or may not be byte-swapped
 and the function cannot tell which.
-----------------------------------------------------------------------*/
#if defined(dp_MUSTSWAP)  /*#if defined(SPARC) || defined(__POWERPC__) */
#ifndef dp_ANET2
void dpSwapEnumSessions(dp_enumSessions_packet_t *buffer);
#endif /* dp_ANET2 */
void dpSwapErrorPacket(dp_error_packet_t *buffer);
void dpSwapSessionPacket(dp_session_packet_t *buffer);
#ifndef dp_ANET2
void dpSwapEnumPlayers(dp_enumPlayers_packet_t *buffer);
void dpSwapPlayerList(dp_playerList_packet_t *buffer, int nPlayers);
void dpSwapPlayerRoster(dp_playerRoster_packet_t *buffer, int nPlayers);
void dpSwapAddPlayer(dp_addPlayer_packet_t *buffer);
void dpSwapDelPlayer(dp_delPlayer_packet_t *buffer);
void dpSwapUserAddPlayer(dp_user_addPlayer_packet_t *buffer);
#endif /* dp_ANET2 */
void dpSwapPingPacket(dp_ping_packet_t *buffer);
#ifndef dp_ANET2
void dpSwapUserAddGroup(dp_group_t *buffer, int n);
void dpSwapUserDelGroup(dp_user_delGroup_packet_t *buffer);
void dpSwapPlayerToFromGroup(dp_addPlayerToGroup_packet_t *buffer);
void dpSwapUserHost(dp_packetType_t *buffer);
#endif /* dp_ANET2 */
void dpSwapEnv(dp_envelope_t *buffer);
#ifndef dp_ANET2
void dpSwapPvUpdateInitial(pv_playerData_initial_packet_t *buffer);
void dpSwapPvUpdate(pv_playerData_body_packet_t *buffer);
#endif /* dp_ANET2 */
#else /* not dp_MUSTSWAP */
#ifndef dp_ANET2
#define dpSwapEnumSessions(buffer)
#endif /* dp_ANET2 */
#define dpSwapErrorPacket(buffer)
#define dpSwapSessionPacket(buffer)
#ifndef dp_ANET2
#define dpSwapEnumPlayers(buffer)
#define dpSwapPlayerList(buffer, nPlayers)
#define dpSwapPlayerRoster(buffer, nPlayers)
#define dpSwapAddPlayer(buffer)
#define dpSwapDelPlayer(buffer)
#define dpSwapUserAddPlayer(buffer)
#endif /* dp_ANET2 */
#define dpSwapPingPacket(buffer)
#ifndef dp_ANET2
#define dpSwapUserAddGroup(buffer, n)
#define dpSwapUserDelGroup(buffer)
#define dpSwapPlayerToFromGroup(buffer)
#define dpSwapUserHost(buffer)
#endif /* dp_ANET2 */
#define dpSwapEnv(buffer)
#ifndef dp_ANET2
#define dpSwapPvUpdateInitial(buffer)
#define dpSwapPvUpdate(buffer)
#endif /* dp_ANET2 */
#endif /* if/else, dp_MUSTSWAP */
#define dpSwapPlayerIdT(buf) dpSwapDelPlayer(buf)

#if defined(_DEBUG) || defined(DPRNT)
extern char key2a_buf[];
extern char key2a_buf2[];
extern char key2a_buf3[];
extern char key2a_buf4[];
extern char *key2buf(const char *key, int keylen, char *buf);
/* Don't use this twice in one printf! */
#define key2a(key, keylen) key2buf(key, keylen, key2a_buf)
/* You can use this one for the second key in a printf */
#define key2a2(key, keylen) key2buf(key, keylen, key2a_buf2)
/* You can use this one for the third key in a printf */
#define key2a3(key, keylen) key2buf(key, keylen, key2a_buf3)
/* etc */
#define key2a4(key, keylen) key2buf(key, keylen, key2a_buf4)
#else
#define key2a(key, keylen) "ndbug"
#define key2a2(key, keylen) "ndbug"
#define key2a3(key, keylen) "ndbug"
#define key2a4(key, keylen) "ndbug"
#endif

/* dp2.c */

/* Check a dp_t for validity */
#if !defined(NDEBUG)
void dp_assertValid(dp_t *d);
#else
#define dp_assertValid(d)
#endif

#define bIAmStandaloneMaster(dp)  \
	(((dp)->dpio->driverinfo->capabilities&comm_DRIVER_NO_BROADCAST) && \
	 ((dp)->hGameServer == PLAYER_NONE) && \
	 ((dp)->hMaster == PLAYER_ME))

/*----------------------------------------------------------------------
 Unpack the compact, byte-order-uniform version of a dp_playerId_t
 into the fluffy form we use internally.
 Does not fill in address field; that has to be looked up in the hosts table.
 Returns number of bytes used, or -1 on error.
----------------------------------------------------------------------*/
int dp_unpack_playerId(dpid_t id, const char *buf, dp_playerId_t *p);

/*----------------------------------------------------------------------
 Convert a dpid into a playerHdl.
----------------------------------------------------------------------*/
playerHdl_t dpid2commHdl(
	dp_t *dp,
	dpid_t id);

/*----------------------------------------------------------------------
 Look up the last dpid that is associated with the given comm handle.
 On failure, return dp_ID_NONE.
----------------------------------------------------------------------*/
dpid_t dp_commHdl2dpid(
	dp_t *dp,
	playerHdl_t h);

/*------------------------------------------------------------------------
 Send a dpio-style unreliable packet to everybody.
 Kludge...
--------------------------------------------------------------------------*/
dp_result_t dp_broadcast_unreliable(dp_t *dp, void *buf, size_t size, playerHdl_t *errHdl);

/*------------------------------------------------------------------------
 Send a dpio-style reliable packet to everybody.
 Kludge...
--------------------------------------------------------------------------*/
dp_result_t dp_broadcast_reliable(dp_t *dp, void *buf, size_t size, playerHdl_t *errHdl);

#if 0
/*--------------------------------------------------------------------------
 Get list of player handles corresponding to the broadcast group.
 Return number of handles, or -1 upon error.
 Note: will not return PLAYER_ME if there's only one player on this machine.

 If flags is DP_SEND_RELIABLE, never yields PLAYER_BROADCAST.
 If flags is DP_SEND_UNRELIABLE, yields PLAYER_BROADCAST only if
 idTo is dp_ID_BROADCAST and the driver prefers broadcast.
--------------------------------------------------------------------------*/
int dp_getBroadcastHdls(dp_t *dp, playerHdl_t hdls[]
#ifdef dp_ANET2
		, int flags
#endif
	);
#endif

/*----------------------------------------------------------------------
 Use with caution.
 Pretend an ACK has been received for all outstanding reliable
 packets with the given tag sent to the given destination.
 (The tag is the first taglen bytes of the packet.)

 This is useful only for higher-level protocols which somehow know
 that packets have gotten through.  For instance, in a turn-based game,
 when the token makes it all the way back around, you know that all the
 packets you sent out have been received, so there's no sense in waiting
 for explicit ACKs anymore.  This can improve performance under conditions
 of heavy packet loss (when the ACKs are likely to be lost).

 Returns dp_RES_OK if one or more packets were affected;
         dp_RES_EMPTY if no packets were affected;
		 dp_RES_BAD if passed invalid arguments;
		 dp_RES_BUG if passed null dp, or internal error.
-------------------------------------------------------------------------*/
dp_result_t dpHigherLevelBlanketAck(
	dp_t  *dp,
	dpid_t idTo,
	char *tag,
	size_t taglen);

/*------------------------------------------------------------------------
 Retrieve the session description for the session with the given id.
 id is a pointer to an array of length dpio->myAdrLen+sizeof(dp_karma_t),
 as returned by dpGetSessionId.
 *buflen must be filled with the size of the buffer before calling;
 it will be set to sizeof(dp_session_t).
 If buflen is NULL, it is assumed that the buffer is the right size.
------------------------------------------------------------------------*/
dp_result_t dpGetSessionDescById(
	dp_t *dp,
	const char *id,
	dp_session_t *buf,
	size_t *pbuflen);

/*----------------------------------------------------------------------
 Clear out any unnecessary player table publishers (i.e. any but the
 host of the session we are in) and tell them to cancel our
 subscription.
 Call this when entering a game to really turn off (not just ignore)
 player deltas from outside sessions.
----------------------------------------------------------------------*/
dp_result_t dpPrunePlayerSubscriptions(dp_t *dp);

/*----------------------------------------------------------------------
 Clear out any unnecessary session table publishers (i.e. the server,
 if we are not in a lobby) and tell them to cancel our subscription.
 Call this only when entering a game (as opposed to a lobby) to really
 turn off (not just ignore) session deltas from outside sessions.
----------------------------------------------------------------------*/
dp_result_t dpPruneSessionSubscription(dp_t *dp);

/* dpscore.c */

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

/*--------------------------------------------------------------------------
 Request that the server send us score data for the given session type.
 Call from dpRequestObjectDeltas().
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_subscribe(dp_t *dp, dp_species_t sessType);

/*--------------------------------------------------------------------------
 Call this (from the player table callback dp_players_cb())
 whenever a player leaves the session.
 Sends a score record for us and the given player to the game server.
 If the player is ourselves, sends scores for all players.

 Return: dp_RES_BAD if score table doesn't exist or can't be created
         dp_RES_EMPTY if there are no score records found
         otherwise return status of dptab_addSubscriber(), dptab_set()
--------------------------------------------------------------------------*/
dp_result_t dpscore_client_playerLeaving(dp_t *dp, dpid_t id);

/*--------------------------------------------------------------------------
 Retrieve a player's dpid.
--------------------------------------------------------------------------*/
dpid_t dpGetMyId(dp_t *dp);

/* deltas.c */

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
	int subkeylen);

/*----------------------------------------------------------------------
 Callback for remote player tables.  Only used if application requested
 object deltas for this session's players.
----------------------------------------------------------------------*/
int dp_PASCAL dp_rplayers_cb(dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest, char *subkey, int subkeylen, void *buf, size_t sent, size_t total, int seconds_left, void *context, dp_result_t status);

/* enumsrv2.c */

/*----------------------------------------------------------------------
 Call this periodically from dpPoll().
 If the user has called dpRequestObjectDeltas(...dp_KEY_SERVERPINGS...),
 this routine will ping the servers for him.
----------------------------------------------------------------------*/
dp_result_t dpEnumServersPoll(dp_t *dp);

/*----------------------------------------------------------------------
 Look up the session type and id of the session the given uid most
 recently tried to join.
----------------------------------------------------------------------*/
dp_result_t dp_uid2sessid(dp_t *dp, dp_uid_t uid, char *sessidbuf, int *sessidlen, dp_species_t *sessType);

/*----------------------------------------------------------------------
 Remember the session type and id of the session the given uid most
 recently tried to join.
 Silently ignores calls with uid == dp_UID_NONE
----------------------------------------------------------------------*/
dp_result_t dp_sessid4uid(dp_t *dp, dp_uid_t uid, const char *sessid, int sessidlen, dp_species_t sessType);

#endif	/* NO_NETWORK */

#endif
