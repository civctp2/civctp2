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

/*-------------------------------------------------------------------------
 Multiplayer game layer: group management.

 Copyright (C) 1997, Activision.

 $Log: dpgroup.c $
 Revision 1.18  1997/05/31 22:49:22  dkegel
 Moved pragma pack into dp*pack*.h for portability.
 Revision 1.17  1997/05/29 19:38:55  dkegel
 Fixed minor type mismatches caught by Codewarrior.
 Revision 1.16  1997/05/28 17:31:34  anitalee
 Made portable to big-endian machines - calls SwapBtyes2 now when reading
 or wirting 16-bit integers to or from the net.  See comments in dpio.c.
 Revision 1.15  1997/03/12 22:25:12  dkegel
 Don't validate player id's when handling addPlayerToGroup packets,
 since the player database may lag behind the group database!
 See gtest\regroup.in for a regression test.
 Revision 1.14  1997/03/12 04:08:00  dkegel
 Get arguments to deletePlayerLocal right.  Sheesh.
 Revision 1.13  1997/03/10 03:30:33  dkegel
 1. Added dpGroup_DeletePlayerFromAllGroups for when players die.
 2. Fixed horrible bug in addplayertogroup/deleteplayerfromgroup - 
 had args to memmove backwards.
 Revision 1.12  1997/03/09 03:52:37  dkegel
 Added a little more dprint info on group operations.
 Revision 1.11  1997/03/09 03:25:23  dkegel
 Validate arg to addplayertogroup.
 Revision 1.10  1997/03/05 02:39:15  dkegel
 ini.h and dprint.h now integrated into anet.h;
 to compile or use DP as a DLL, define DP_DLL;
 win\build.bat now creates debugging and nondebugging versions
 of both the static dp.lib and the dynamic anetdll.dll.
 Revision 1.9  1997/03/01 18:21:45  dkegel
 Fixed compiler warning.
 Revision 1.8  1997/03/01 08:54:57  dkegel
 Better dprints.
 Revision 1.7  1997/02/24 01:34:16  dkegel
 Made it clear that s must be NULL for dpEnumGroup*, and why.
 Revision 1.6  1997/02/16 04:05:30  dkegel
 1. group add packets now carry entire group contents, so
 informNewNode works now.
 2. use pragma pack() around structures that need to be portable
 3. Reduced max group size to dp_MAXREALPLAYERS.
 Revision 1.5  1997/02/13 00:29:16  dkegel
 Groups now saved by freeze/thaw.
 Revision 1.4  1997/02/12 03:46:05  dkegel
 Tie in to player variable system - delete group variables when group
 deleted.
 Revision 1.3  1997/02/12 03:02:36  dkegel
 Finished converting to use playerHdl_t instead of dpid_t for sending
 group info.
 Added more null-pointer checks.
 Revision 1.2  1997/02/10 07:10:12  dkegel
 1. Added handleAddGroupPacket, handleDelGroupPacket.
 2. Fixed bug in sendAddGroupPacket.
 Revision 1.1  1997/02/07 00:08:14  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#include "dp.h"
#include "dpprivy.h"
#include "dpgroup.h"

/*#undef	DPRINT*/
/*#define DPRINT(s)*/

/********************** Internal Functions ******************************/

/* Function to dump a block of memory in hex */
#if  1
static void dumpBuf(char *buf, int len)
{
	int i;
	for (i=0; i<len; i++) {
		DPRINT(("%02x ", buf[i]));
	}
	DPRINT(("\n"));
}
#endif

/* Function to dump the group table in ascii. */
static void dpGroup_dump(dp_t *dp)
{
	int i;
	if (!dp->groups) {
		DPRINT(("dpGroup_dump: no group table\n"));
		return;
	}
	DPRINT(("dpGroup_dump: n_used %d\n", dp->groups->n_used));
	for (i=0; i<dp->groups->n_used; i++) {
		assoctab_item_t *pe;
		dp_group_t *pg;
		int j;
		pe = assoctab_getkey(dp->groups, i);
		if (!pe) {
			 DPRINT(("dpGroup_dump: no group entry for index %d\n", i));
			 continue;
		}
		pg = (dp_group_t *) &pe->value;
		DPRINT(("dpGroup_dump: group id %d, karma %x, name '%s', n %d [",
			pg->id, pg->karma, pg->name, pg->n));
		for (j=0; j<pg->n; j++)
			DPRINT(("%d,", pg->members[j]));
		DPRINT(("]\n"));
	}
}

/*----------------------------------------------------------------------
 Create the dp->groups object.  Return dp_RES_OK upon success.
 Called by dpOpen and its ilk.
----------------------------------------------------------------------*/
dp_result_t dpGroup_create(dp_t *dp)
{
	dp->groups = assoctab_create(sizeof(dp_group_t));
	if (!dp->groups) return dp_RES_NOMEM;
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Destroy the dp->groups object.  Don't destroy it if it's NULL.
 Call this when opening a session.
 Called by dpClose and its ilk.
----------------------------------------------------------------------*/
void dpGroup_destroy(dp_t *dp)
{
	if (dp->groups) {
		assoctab_destroy(dp->groups);
		dp->groups = NULL;
	}
}

/*-----------------------------------------------------------------------
 Save the state of the groups table to disk.
-----------------------------------------------------------------------*/
dp_result_t dpGroup_freeze(dp_t *dp, FILE *fp)
{
	DPRINT(("dpGroup_freeze: "));
	dpGroup_dump(dp);
	(void) assoctab_freeze(dp->groups, fp);
	return dp_RES_OK;
}

/*-----------------------------------------------------------------------
 Restore the state of the groups table from disk.
-----------------------------------------------------------------------*/
dp_result_t dpGroup_thaw(dp_t *dp, FILE *fp)
{
	if (!assoctab_thaw(dp->groups, fp))
		return dp_RES_BAD;
	DPRINT(("dpGroup_thaw: "));
	dpGroup_dump(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Handle an add group packet.  Creates the group locally.
 Group starts out fully formed, with all members.
 (No group variables yet, though.  Those come later.)
----------------------------------------------------------------------*/
dp_result_t dpGroup_HandleAddGroupPacket(
	dp_t	*dp,
	dp_group_t *body,
	size_t len)
{
	dp_group_t *pg;

	DPRINT(("dpGroup_HandleAddGroupPacket(dp, ...): id %d, name %s, n %d\n",
		body->id, body->name, body->n));

	if (!dp->groups) {
		DPRINT(("dpGroup_HandleAddGroupPacket: not in session\n"));
		return dp_RES_BUG;
	}
	if (body->sessionKarma != dp->s.karma) {
		DPRINT(("dpGroup_HandleAddGroupLocal: bad karma; got %d, wanted %d\n", body->sessionKarma, dp->s.karma));
		return dp_RES_BAD;
	}
	if (len != sizeof_dp_group_t(body->n)) {
		DPRINT(("dpGroup_HandleAddGroupLocal: bad len; was %d, wanted %d\n", len, sizeof_dp_group_t(body->n)));
		return dp_RES_BAD;
	}

	/* Allocate storage */
	pg = (dp_group_t *)assoctab_subscript_grow(dp->groups, body->id);
	if (!pg) {
		DPRINT(("dpGroup_HandleAddGroupLocal: bug\n"));
		return dp_RES_BUG;
	}
	/* Fill group */
	memcpy(pg, body, sizeof_dp_group_t(body->n));
	pg->name[sizeof(pg->name)-1] = 0;
	dpGroup_dump(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Handle an del group packet.  Deletes the group locally.
----------------------------------------------------------------------*/
dp_result_t dpGroup_HandleDelGroupPacket(
	dp_t	*dp,
	dp_user_delGroup_packet_t *body)
{
	dp_result_t err;
	DPRINT(("dpGroup_HandleDelGroupPacket(dp, %d, %s):\n", body->id, body->name));

	if (!dp->groups) {
		DPRINT(("dpGroup_HandleDelGroupPacket: not in session\n"));
		return dp_RES_BUG;
	}
	if (body->sessionKarma != dp->s.karma) {
		DPRINT(("dpGroup_HandleDelGroupLocal: bad karma\n"));
		return dp_RES_BAD;
	}

	/* Deallocate storage */
	if (assoctab_subscript_delete(dp->groups, body->id)) {
		DPRINT(("dpGroup_HandleDelGroupLocal: bug\n"));
		return dp_RES_BUG;
	}

	/* Delete the group's variables. */
	err = pv_deletePlayer(dp->pv, body->id);
	if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)) {
		DPRINT(("dpGroup_handleDelGroupPacket: can't del group %d's variables, err %d\n", body->id, err));
		return err;
	}

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Inform some machine about the given group.  Only called by host.
 Sends a dp_user_addGroup_packet_t or dp_user_delGroup_packet_t reliably.
 dest should be either dp_ID_BROADCAST (when creating/deleting group)
 or a particular machine's pseudoplayer (when adding machine).
----------------------------------------------------------------------*/
static dp_result_t dpGroup_SendAddGroupPacket(
	dp_t *dp,
	dp_group_t *pg,
	playerHdl_t dest,
	dp_packetType_t tag)
{
	dp_result_t err;
	playerHdl_t errHdl;
	size_t pktlen;
#include "dppack1.h"
	struct {
		dp_packetType_t tag;
		union {
			dp_group_t add PACK;
			dp_user_delGroup_packet_t del PACK;
		} b PACK;
		char pad[6] PACK;
	} pkt;
#include "dpunpack.h"

	if (!dp || !dp->groups || !pg) {
		DPRINT(("dpGroup_SendAddGroupPacket: null\n"));
		return dp_RES_BUG;
	}
	if (!dp->bMaster) {
		DPRINT(("dpGroup_SendAddGroupPacket: must be master\n"));
		return dp_RES_BAD;
	}
	pkt.tag = tag;

	/* If it's an addGroup packet, copy the whole thing.
	 * If it's a delGroup packet, just copy the beginning.
	 */
	if (tag == dp_USER_ADDGROUP_PACKET_ID)
		pktlen = sizeof_dp_group_t(pg->n);
	else
		pktlen = sizeof(dp_user_delGroup_packet_t);
	memcpy(&pkt.b, pg, pktlen);
	pktlen += sizeof(pkt.tag);
	pkt.b.add.sessionKarma = dp->s.karma;

	DPRINT(("dpGroup_SendAddGroupPacket(dp, pg->name %s, dest %d, tag %c%c; bytes %d):\n",
		pg->name, dest, ((char *)&tag)[0], ((char *)&tag)[1], pktlen));
	dumpBuf((char *)&pkt, pktlen);
	if (tag == dp_USER_ADDGROUP_PACKET_ID)
	{
		int i;
		for (i=0; i<pkt.b.add.n; i++) {
			DPRINT(("dpGroup_SendAddGroupPacket: player %d in group is %d = %d\n",
				 i, pkt.b.add.members[i], pg->members[i]));
		}
		/* call dpSwapUserAddGroup to byte swap pkt.b.add) */
		dpSwapUserAddGroup(&pkt.b.add, pkt.b.add.n);
	}
	else
	{
		/* call dpSwapUserDelGroup to byte swap pkt.b.del) */
		dpSwapUserDelGroup(&pkt.b.del);
	}
	
    if (dest == PLAYER_BROADCAST)
		err = dp_broadcast_reliable(dp, &pkt, pktlen, &errHdl);
	else
		err = dpio_put_reliable(dp->dpio, &dest, 1, &pkt, pktlen, &errHdl);

	if (err != dp_RES_OK)
		DPRINT(("dpGroup_SendAddGroupPacket: send failed, err %d, errHdl %d\n", err, errHdl));
	return err;
}

/*----------------------------------------------------------------------
 Inform some machine about the given new player in the given group.
 Sends a dp_addPlayerToGroup_packet_t or dp_DeletePlayerFromGroup_packet_t
 reliably.
 dest should be dp_ID_BROADCAST (when creating/deleting player to/from group)
 or a particular machine's pseudoplayer (when adding machine).
----------------------------------------------------------------------*/
static dp_result_t dpGroup_SendAddPlayerPacket(
	dp_t *dp,
	dpid_t	idGroup,
	dpid_t	idPlayer,
	playerHdl_t  dest,
	dp_packetType_t tag)
{
	dp_result_t err;
	playerHdl_t errHdl;
	size_t pktlen;
#include "dppack1.h"
	struct {
		dp_packetType_t tag;
		dp_addPlayerToGroup_packet_t body PACK;
		char pad[6] PACK;
	} pkt;
#include "dpunpack.h"

	if (!dp) {
		DPRINT(("dpGroup_SendAddPlayerPacket: null\n"));
		return dp_RES_BUG;
	}
	DPRINT(("dpGroup_SendAddPlayerPacket(dp, gr %d, pl %d, dest %d, tag %c%c):\n",
		idGroup, idPlayer, dest, tag & 0xff, tag >> 8));

	pkt.tag = tag;
	pkt.body.dpIdGroup = idGroup;
	pkt.body.dpIdPlayer = idPlayer;
	pkt.body.sessionKarma = dp->s.karma;	/* so destination can reject if not same session */

	pktlen = sizeof(pkt.tag)+sizeof(pkt.body);
	/* call dpSwapPlayerToFromGroup to byte swap pkt.body */
	dpSwapPlayerToFromGroup(&pkt.body);
    if (dest == PLAYER_BROADCAST)
		err = dp_broadcast_reliable(dp, &pkt, pktlen, &errHdl);
	else
		err = dpio_put_reliable(dp->dpio, &dest, 1, &pkt, pktlen, &errHdl);
	if (err != dp_RES_OK)
		DPRINT(("dpGroup_SendAddPlayerPacket: send failed, err %d\n", err));
	return err;
}

/*--------------------------------------------------------------------------
 Notify a new player about every existing player group.

 Called only by dpReceive on host.

 Should only be called if
 we're the host,
 we're creating a new player,
 the new player is remote,
 this is the first time we've received a request to create him,
 AND it's a pseudoplayer.
--------------------------------------------------------------------------*/
dp_result_t dpGroup_InformNewMachine(
	dp_t *dp,
	playerHdl_t dest)
{
	int i;
	for (i=0; i<dp->groups->n_used; i++) {
		assoctab_item_t *pe;
		dp_group_t *pg;
		pe = assoctab_getkey(dp->groups, i);
		if (!pe) {
			 DPRINT(("dpGroup_InformNewMachine: no group entry for index %d\n", i));
			 return dp_RES_BUG;
		}
		pg = (dp_group_t *) &pe->value;
		DPRINT(("dpGroup_InformNewMachine: telling dest %d about group id %d, karma %x, name %p = '%s'\n",
			dest, pg->id, pg->karma, pg->name, pg->name));
		/* Hope we don't send too many packets here! */
		(void) dpGroup_SendAddGroupPacket(dp, pg, dest, dp_USER_ADDGROUP_PACKET_ID);
	}
	dpGroup_dump(dp);

	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Add player idPlayer to the group idGroup, but don't broadcast notification.
 Called only by dpReceive and dpAddPlayerToGroup.
----------------------------------------------------------------------*/
dp_result_t dpGroup_AddPlayerLocal(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer)
{
	dp_group_t *pg;
	int i;

	DPRINT(("dpGroup_AddPlayerLocal(dp, group %d, player %d)\n", idGroup, idPlayer));
	if (!dp->groups) {
		DPRINT(("dpGroup_AddPlayerLocal: not in session\n"));
		return dp_RES_BUG;
	}
	pg = (dp_group_t *)assoctab_subscript(dp->groups, idGroup);
	if (!pg) {
		DPRINT(("dpGroup_AddPlayerLocal(%d, %d): no such group\n", idGroup, idPlayer));
		return dp_RES_BAD;
	}
	/* Verify that player exists */
	if (dpGetPlayerName(dp, idPlayer, NULL, 0) != dp_RES_OK) {
		DPRINT(("dpGroup_AddPlayerLocal(%d, %d): warning: no such player\n", idGroup, idPlayer));
		/* Can't be sure our player database is up to date yet.  Grr. */
		/*return dp_RES_BAD;*/
	}
	if (pg->n >= dp_MAXREALPLAYERS) {
		/* Unlikely */
		DPRINT(("dpGroup_AddPlayerLocal(%d, %d): group full\n", idGroup, idPlayer));
		return dp_RES_FULL;
	}

	/* Find proper point at which to insert player */
	for (i=0; i<pg->n; i++)
		if (idPlayer <= pg->members[i]) break;
	if ((i < pg->n) && (idPlayer == pg->members[i])) {
		DPRINT(("dpGroup_AddPlayerLocal(%d, %d): player already in group\n", idGroup, idPlayer));
		return dp_RES_ALREADY;
	}
	/* Move other players up */
	/* If no other players, then i is zero, n is zero, and no bytes are moved. */
	memmove(pg->members+i+1, pg->members+i, sizeof(pg->members[0]) * (pg->n - i));
	/* Add new player */
	pg->members[i] = idPlayer;
	pg->n++;

	dpGroup_dump(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Delete player idPlayer from idGroup, but don't broadcast notification.
 Called only by dpReceive and dpDeletePlayerFromGroup.
----------------------------------------------------------------------*/
dp_result_t dpGroup_DeletePlayerLocal(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer)
{
	dp_group_t *pg;
	int i;

	if (!dp->groups) {
		DPRINT(("dpGroup_DeletePlayerLocal: not in session\n"));
		return dp_RES_BUG;
	}
	pg = (dp_group_t *)assoctab_subscript(dp->groups, idGroup);
	if (!pg) {
		DPRINT(("dpGroup_DeletePlayerLocal(%d, %d): no such group\n", idGroup, idPlayer));
		return dp_RES_BAD;
	}
	/* Find player */
	for (i=0; i<pg->n; i++)
		if (idPlayer <= pg->members[i]) break;
	if ((i == pg->n) || (idPlayer != pg->members[i])) {
		DPRINT(("dpGroup_DeletePlayerLocal(%d, %d): player not in group\n", idGroup, idPlayer));
		return dp_RES_ALREADY;
	}
	/* Move other players down */
	pg->n--;
	/* If no other players, then i is zero, n is zero, and no bytes are moved. */
	memmove(pg->members+i, pg->members+i+1, sizeof(pg->members[0]) * (pg->n - i));

	dpGroup_dump(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Delete player idPlayer from all groups.
 Only called by dpNotifyDeletePlayer.
 Bug: should generate messages to user code, but doesn't.
----------------------------------------------------------------------*/
dp_result_t dpGroup_DeletePlayerFromAllGroups(
	dp_t	*dp,
	dpid_t	idPlayer)
{
	int i;

	if (!dp->groups) {
		DPRINT(("dpGroup_DeletePlayerFromAllGroups: not in session\n"));
		return dp_RES_BUG;
	}
	for (i=0; i<dp->groups->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(dp->groups, i);
		if (!pe) {
			 DPRINT(("dpGroup_DeletePlayerFromAllGroups: no group entry for index %d\n", i));
			 return dp_RES_BUG;
		}
		DPRINT(("dpGroup_DeletePlayerFromAllGroups: deleting id %d from group id %d\n",
			idPlayer, pe->key));
		if (dpGroup_DeletePlayerLocal(dp, pe->key, idPlayer) == dp_RES_OK) {
			/* send message to user code that player no longer in group */
			/* BUG */
			;
		}
	}

	return dp_RES_OK;
}

/********************** User-Callable Functions **************************/

/*----------------------------------------------------------------------
 Add a new group to the currently open session.
 Can only be called on game host!
----------------------------------------------------------------------*/
DP_API dp_result_t dpCreateGroup(
	dp_t	*dp,
	dpid_t *id,
	char_t	*name)
{
	dp_group_t *pg;
	dpid_t groupId;

	if (!dp || !dp->groups) {
		DPRINT(("dpCreateGroup: null\n"));
		return dp_RES_BUG;
	}
	if (!dp->bMaster) {
		DPRINT(("dpCreateGroup: must be master\n"));
		return dp_RES_BAD;
	}
	DPRINT(("dpCreateGroup(dp, %p, %s):\n", id, name));
	/* Allocate group id and storage */
	groupId = dp->nextId++;
	pg = (dp_group_t *)assoctab_subscript_grow(dp->groups, groupId);
	if (!pg) {
		DPRINT(("dpCreateGroup: bug\n"));
		return dp_RES_BUG;
	}
	/* Fill group */
	pg->id = groupId;
	do
		pg->karma = rand();
	while (pg->karma == dp_KARMA_NONE);
	strncpy(pg->name, name, sizeof(pg->name));
	pg->name[sizeof(pg->name)-1] = 0;
	DPRINT(("dpCreateGroup(%s): pg->name %s\n", name, pg->name));
	pg->n = 0;

	*id = groupId;

	/* Notify all existing systems about new group */
	return dpGroup_SendAddGroupPacket(dp, pg, PLAYER_BROADCAST, dp_USER_ADDGROUP_PACKET_ID);
}


/*----------------------------------------------------------------------
 Destroy the given group; removes the group from the game session.
 The dpID will not be reused during the current session.
 Can only be called on game host!
----------------------------------------------------------------------*/
DP_API dp_result_t dpDestroyGroup(
	dp_t   *dp,
	dpid_t id)
{
	dp_group_t *pg;
	dp_result_t err;
	int fail;

	if (!dp || !dp->groups) {
		DPRINT(("dpDestroyGroup: null\n"));
		return dp_RES_BUG;
	}
	if (!dp->bMaster) {
		DPRINT(("dpDestroyGroup: must be master\n"));
		return dp_RES_BAD;
	}
	pg = (dp_group_t *)assoctab_subscript(dp->groups, id);
	if (!pg) return dp_RES_EMPTY;

	/* Notify all existing systems about loss of group */
	err = dpGroup_SendAddGroupPacket(dp, pg, PLAYER_BROADCAST, dp_USER_DELGROUP_PACKET_ID);
	if (err != dp_RES_OK) return err;

	fail = assoctab_subscript_delete(dp->groups, id);
	if (fail) return dp_RES_EMPTY;
	return dp_RES_OK;

}

/*----------------------------------------------------------------------
 Add player idPlayer to the group idGroup.  Broadcast notification to others.
 Called by user code.
----------------------------------------------------------------------*/
DP_API dp_result_t dpAddPlayerToGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer)
{
	dp_result_t err;

	DPRINT(("dpAddPlayerToGroup(dp, group %d, player %d)\n", idGroup, idPlayer));
	if (dpid2commHdl(dp, idPlayer) == PLAYER_NONE) {
		DPRINT(("dpAddPlayerToGroup: player %d has no comm handle, can't add\n", idPlayer));
		return dp_RES_BAD;
	}
	err = dpGroup_AddPlayerLocal(dp, idGroup, idPlayer);
	if (err != dp_RES_OK)
		return err;

	/* Propagate this to all other machines in game */
	return dpGroup_SendAddPlayerPacket(dp, idGroup, idPlayer, PLAYER_BROADCAST,
		dp_USER_ADDPLAYERTOGROUP_PACKET_ID);
}

/*----------------------------------------------------------------------
 Delete player idPlayer from the group idGroup.
----------------------------------------------------------------------*/
DP_API dp_result_t dpDeletePlayerFromGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer)
{
	dp_result_t err;

	DPRINT(("dpDeletePlayerFromGroup(dp, group %d, player %d)\n", idGroup, idPlayer));
	err = dpGroup_DeletePlayerLocal(dp, idGroup, idPlayer);
	if (err != dp_RES_OK)
		return err;

	/* Propagate this to all other machines in game */
	return dpGroup_SendAddPlayerPacket(dp, idGroup, idPlayer, PLAYER_BROADCAST,
		dp_USER_DELPLAYERFROMGROUP_PACKET_ID);
}

/*----------------------------------------------------------------------
 Calls the given function once for each group in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the group in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.

 s must currently be NULL - that is, you can't yet enumerate the groups
 of sessions you haven't joined.
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnumGroups(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	)
{
	dp_group_t *pg;
	int i;

	(void) timeout;
	if (s) {
		DPRINT(("dpEnumGroups: can't enum other sessions' groups yet.\n"));
		return dp_RES_BUG;
	}

	if (!dp->groups) {
		DPRINT(("dpEnumGroups: not in session\n"));
		return dp_RES_BUG;
	}

	for (i=0; i<dp->groups->n_used; i++) {
		assoctab_item_t *pe;
		pe = assoctab_getkey(dp->groups, i);
		if (!pe) {
			 DPRINT(("dpEnumGroups: no entry for index %d\n", i));
			 return dp_RES_BUG;
		}
		pg = (dp_group_t *) &pe->value;
		if (pe->key != pg->id)
			DPRINT(("dpEnumGroups: bug\n"));
		DPRINT(("dpEnumGroups: id %d, karma %x, name %p = '%s'\n",
			pg->id, pg->karma, pg->name, pg->name));
		cb(pg->id, pg->name, 0, context);
	}
	cb(dp_ID_NONE, NULL, 0, context);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Calls the given function once for each player in the given group, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.

 s must currently be NULL - that is, you can't yet enumerate the group
 members of sessions you haven't joined.
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnumGroupPlayers(
	dp_t *dp,
	dpid_t idGroup,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	)
{
	dp_group_t *pg;
	int i;

	if (s) {
		DPRINT(("dpEnumGroupPlayers: can't enum other sessions' groups yet.\n"));
		return dp_RES_BAD;
	}
	if (!dp->groups) {
		DPRINT(("dpEnumGroupPlayers: not in session\n"));
		return dp_RES_BUG;
	}

	pg = (dp_group_t *)assoctab_subscript(dp->groups, idGroup);
	if (!pg) {
		DPRINT(("dpEnumGroupPlayers(%d): no such group\n", idGroup));
		return dp_RES_BAD;
	}

	/* Go through the players in the group; fetch name and status; call back. */
	for (i=0; i<pg->n; i++) {
		char name[256];
		if (dpGetPlayerName(dp, pg->members[i], name, sizeof(name)) != dp_RES_OK)
			continue;
		/* Should check whether player is local or remote here... */
		/* Should also check return value of callback. */
		cb(pg->members[i], name, 0, context);
	}

	(void) timeout;
	return dp_RES_OK;
}
