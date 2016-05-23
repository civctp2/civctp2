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

/*----------------------------------------------------------------------------
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been modified by the Apolyton CtP2 Source Code Project.
 * Contact the authors at ctp2source@apolyton.net.
 *
 * Modifications from the Activision Anet 0.10 code:
 *
 * - 2005/03/11: Added return statements to prevent compiler complaints.
 *
 *----------------------------------------------------------------------------
 */

#define SEMIRELIABLE_SESSIONS
/* #define DONT_RETAIN_SESSFLAGS */
/* #define USE_dp_enableNewPlayers */
/* #define IGNORE_CLOSEDSESS */
/*------------------------------------------------------------------------
 Copyright (C) 1996, 1997, Activision.

 Multiplayer game layer.
 The central object is the 'session'.
 A session is a group of machines, each of which can have zero or more
 players in the session.  One machine is the session master.
 Messages can be sent from one player to another player, to a group of
 players, or broadcast to all players in the session.

 Machines can't be operated on directly.
 Sessions can be enumerated, created, joined, and left.  They can't be deleted!
 Groups can be enumerated, created, deleted, joined, left, and sent to.
 Players can be enumerated, created, deleted, and sent to.
 A group exists within a session, and can't cross session boundaries.

 $Log: dp2.c $
 Revision 1.64  1997/10/08 03:24:01  lbennett
 Adding extra info to dpEnumApps; in progress.
 Revision 1.63  1997/10/07 23:30:56  lbennett
 Removed compiler warnings.
 Revision 1.62  1997/10/07 02:41:57  anitalee
 temporary workaround until netshell can handle unreliable sessionlists
 Revision 1.61  1997/10/05 05:48:45  dkegel
 Fixed crash bug in dpClose caused by overeager debug prints
 Revision 1.60  1997/10/04 01:29:54  lbennett
 Better logging
 Revision 1.59  1997/10/01 21:49:02  lbennett
 Added better logging.
 Revision 1.58  1997/10/01 00:12:38  lbennett
 Fixed hdl2sess creation in freeze/thaw
 Fixed openHdl_cb bug (table deleted in wrong case)
 Fixed karma-not-set bug in dpClose's group table code.
 Revision 1.57  1997/09/30 18:32:58  dkegel
 work in progress: try to restore hdl2sess properly
 Revision 1.56  1997/09/30 06:38:19  lbennett
 Work in progress, fixing freeze/thaw.
 Revision 1.55  1997/09/28 03:54:26  dkegel
 Save game server list to disk on exit
 Revision 1.54  1997/09/27 05:37:15  lbennett
 Close frozen file's handle, restore dpid2commHdl correctly.
 Revision 1.53  1997/09/27 02:01:31  lbennett
 Better error checking and logging.
 Revision 1.52  1997/09/26 19:02:53  anitalee
 added terminating callback to heavy gear bandaids
 Revision 1.51  1997/09/26 18:43:23  lbennett
 Added assertValid calls to dpReceive.
 Revision 1.50  1997/09/25 22:23:14  lbennett
 Fixed obscure bug for double-deletion of peers.
 Revision 1.49  1997/09/25 21:19:42  lbennett
 Fixed sessionContext table bug.
 Added debug-only dump routines for sessionContexts.
 Corrected a few mislabeled DPRINTs.
 Revision 1.48  1997/09/25 18:34:53  lbennett
 Fixed sessionContext crash bug.
 Revision 1.47  1997/09/25 17:36:46  dkegel
 Now binds properly to heavy gear's sim
 Revision 1.46  1997/09/25 03:48:50  lbennett
 Work in progress; freeze/thaw.
 Revision 1.45  1997/09/24 03:15:06  lbennett
 Work in progress; adding freeze/thaw.
 Revision 1.44  1997/09/23 19:05:39  anitalee
 added delete tables created by dpRemoteEnumPlayers and removed band-aids
 Revision 1.43  1997/09/20 04:48:33  anitalee
 added remote enum players and various bandaids for heavy gear
 Revision 1.42  1997/09/19 03:56:29  anitalee
 added group functions
 Revision 1.41  1997/09/16 18:20:40  lbennett
 Changed dptab ->context references to use dptab_getTableContext() instead.
 Revision 1.40  1997/09/14 01:56:57  lbennett
 Converted to new table callbacks.
 Revision 1.39  1997/09/13 21:57:36  lbennett
 Modified dpSetGameServer() to handle existing server correctly,
 either ignoring the request for an already-open server, or by
 refusing for a server in the process of being closed.
 Revision 1.38  1997/09/13 19:23:05  dkegel
 Kludge to let server lobbies fill all the way up.
 Revision 1.37  1997/09/04 21:05:47  dkegel
 Make some attempt to enforce maxplayers
 Revision 1.36  1997/09/04 06:47:04  dkegel
 1. Support dptab_requestSubscription().
 2. Use it to transfer a table of game servers from the server we're
 connected to, if any.  Crude, but it's better than nothing.
 Revision 1.35  1997/09/04 02:20:44  dkegel
 Wrote but did not enable session record compression routine.
 Revision 1.34  1997/09/03 22:54:26  dkegel
 Implemented dpDeclareLobby - needed to set handle limit high
 enough for server.
 Revision 1.33  1997/09/03 20:05:02  dkegel
 Got rid of spurious bug message - hosts table is supposed to lose
 its context after session is cast off
 Revision 1.32  1997/09/02 23:51:08  dkegel
 KLUDGE: pass back handle in password parameter of openHdl callback!
 Revision 1.31  1997/09/02 23:01:18  dkegel
 Enable user-level handle callbacks so server can set socketopts.
 Revision 1.30  1997/09/01 19:15:47  dkegel
 1. Open handle to new hosts as soon as possible.
 2. Close handles by looking at hdl2sess rather than dpid2hdl;
 this catches even hosts that haven't created players yet.
 Revision 1.29  1997/09/01 00:37:38  dkegel
 Hosts which time out now have their player and host records
 removed again.
 Revision 1.28  1997/08/31 22:14:21  dkegel
 Compiles with gcc.  (Had to cast foo ** to void **... yech.)
 Revision 1.27  1997/08/31 04:39:53  dkegel
 1. Use standard macros for turning shorts into keys and vice versa.
 2. Compress host and player records.  This reduces modem traffic from
 server by 50%.  Currently we use one malloc per entry, which is pretty
 awful; we need to make dptab a little better at doing its own allocation.
 3. Store host records with a key of firstId, partly because that saves
 network traffic, partly because that's how we really want to look 'em up.
 4. dpClose now uses getBroadcastHandles to get handles to close.
 5. dpNumPlayers now just a call to dptab_tableSize.
 Revision 1.26  1997/08/28 18:06:27  dkegel
 Added dpCastoffSession, which is used in the game server to spawn
 multiple lobbies.  Need to break dpOpen up into dpCreateSession and
 leave the 'default session' variable-setting in dpOpen.
 Revision 1.25  1997/08/28 15:23:28  dkegel
 Protect two more functions against crashing if called when no session.
 Revision 1.24  1997/08/27 15:58:28  dkegel
 dpHandleLeavePacket() now does less locally to avoid
 triggering a SESSION_LOST message when executing a normal dpClose()
 of a session you hosted.
 Revision 1.23  1997/08/26 23:21:49  dkegel
 Don't let user join self.
 Revision 1.22  1997/08/26 21:07:36  dkegel
 1. Send a dp_ERROR_PACKET if somebody tries to join a nonexistant session.
 2. Handle dp_ERROR_PACKETs and abort the join if it's the current one.
 3. Perform the right magic incantation (#include dppack1.h) to get
 the nasty embedded structures to be byte alligned.
 (Why did it work before?)
 4. Ripped out the trailing padding bytes on the embedded structures;
 not needed with dpio_put_reliable.
 Revision 1.21  1997/08/26 00:12:58  dkegel
 1. Send sessionlost packet when our host record is deleted.
 This only happens when the host times out or kicks us out.
 Lets the user code know we're out of the session.
 2. dpReceive now correctly passes through sessionlost packets.
 Revision 1.20  1997/08/25 21:35:49  dkegel
 Fixed crash bug- can't dpSend if no session.
 Revision 1.19  1997/08/25 02:02:13  dkegel
 dpClose now forces a dptab_update() so Matt's method of
 checking dpReadyToFreeze() after dpClose() is likely to work.
 Revision 1.18  1997/08/25 01:25:29  dkegel
 Switch to new timeout control method.
 Revision 1.17  1997/08/25 00:33:38  dkegel
 Improved handling of error conditions; terminating callbacks
 now issued properly in more cases.
 Revision 1.16  1997/08/23 23:34:26  dkegel
 1. Fixed current player count.
 2. Renamed a parameter in the callbacks to avoid ambiguity;
 probably prevented a future bug.
 Revision 1.15  1997/08/23 21:24:19  dkegel
 Master now updates session record when players enter or leave.
 Revision 1.14  1997/08/23 20:48:12  dkegel
 1. Added dp_LEAVE_PACKET_ID
 2. Added dpSendLeaveSession() and dpHandleLeaveSession()
 3. Added code in dpClose() to more thoroughly undo dpOpen()
 Revision 1.13  1997/08/23 00:54:22  dkegel
 Fixed swap routine.
 Revision 1.12  1997/08/23 00:41:31  dkegel
 1. Maintain array to map dpid's to playerhdl's.
 2. Implement dpClose more fully.
 Revision 1.11  1997/08/22 21:13:19  dkegel
 1. Avoid calling dpClose in callbacks by setting new private flag
 pleaseClose
 2. Avoid timing out game server
 3. Sent "session lost" message to user code if master goes away
 Revision 1.10  1997/08/22 16:53:05  dkegel
 1. Support dpPingUser and dpDestroyPlayer.
 2. Send keepalives to game server.
 Revision 1.9  1997/08/20 22:35:35  dkegel
 1. Implemented dpGetPlayerCaps and dpGetPlayerName.
 2. Gave dpEnumServers() most of its timeout semantics back.
 3. Implemented stubs for all functions used by gtest.
 Revision 1.8  1997/08/19 21:58:42  dkegel
 Implemented player system.
 Revision 1.7  1997/08/18 18:44:21  dkegel
 Support joining sessions on LAN's.
 Now uses dptab to broadcast our session info.  It even sends
 out the delete record - no more ghost rooms!
 Revision 1.6  1997/08/18 01:50:20  dkegel
 Session joining starting to come alive...
 Revision 1.5  1997/08/15 02:05:18  dkegel
 Session records now deleted when host who sent them disconnects!
 Revision 1.4  1997/08/11 23:48:37  dkegel
 dpClose() now deletes session record from mysessions.
 Revision 1.3  1997/08/11 01:25:37  dkegel
 Can now propagate stuff across three or four hops.
 Moved unit test code into dp2t1.c (client) and dp2t2.c (server).
 Revision 1.2  1997/08/09 01:42:52  dkegel
 Added almost enough to try publishing sessions on a LAN.
 Revision 1.1  1997/08/08 21:04:23  dkegel
 Initial revision
------------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(_WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef UNIX
/* for dpYield() (select) */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <crtdbg.h>
#define ASSERTMEM() /* assert(_CrtCheckMemory())   Use only in dire circumstances, it takes 200ms sometimes */
#else
#define ASSERTMEM()
#endif

#include "dp.h"
#include "dpmem.h"
#include "dptab.h"
#include "dpprivy.h"
#include "enumserv.h"
#include "enumapps.h"
#include "portable.h"
#include "contract.h"
#include "eclock.h"
#include "dpexcept.h"
#include "launchparams.h"

#include "ptimer.h"
/* PTIMER_* must be less than NPMAX */
#define PTIMER_RECEIVE 0
#define PTIMER_RECEIVE_FINE 1
#define PTIMER_ECLOCK 2
#define PTIMER_DPIO_GET 3
#define PTIMER_POLLPING 4
#define PTIMER_POLL 5
/* Note: have #define PTIMER_DPIO_NETWORK_GET 6 in dpio.c */

/* Many structures are declared in this file that get
 * sent over the wire.  Let the default structure packing
 * for every structure declared from here on down be byte-alligned.
 * Must come after last #include!
 */
#include "dppack1.h"

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if defined(_WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

#define DEFER_SESSION_UPDATE
#define DP_BEACON_INT_MSEC 1000		/* How often to do frequent chores */

#define MY_MAX_HOSTS 100			/* > number of hosts in a session */

/*--------------------------------------------------------------------------
 Element of dp->election_votes.
--------------------------------------------------------------------------*/
typedef struct {
	clock_t when;		/* Used to time out old votes. */
	dpid_t id;			/* Who this vote is for. */
} dp_election_vote_t;

/*----------------- Private Utility Functions -----------------------------*/

#if defined(_WIN32)
    #define dpYield() Sleep(10)
#else
#if defined(UNIX)
    #define dpYield()	{	struct timeval tv; \
							tv.tv_sec = 0; \
							tv.tv_usec = 10000; /* 10ms */ \
							select(1, NULL, NULL, NULL, &tv); \
						}
#else
	#define dpYield() /* FIXME: what this should be for Mac? */
#endif
#endif

#if !defined(NDEBUG)

int dp_PASCAL dp_hosts_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status);

#define dp_assertValidSessionContext(s) {\
	assert((s) != NULL);\
	assert((s)->sess_subkeylen <= dptab_KEY_MAXLEN);\
	dptab_assertValidTable((s)->hosts);\
	dptab_assertValidTable((s)->players);\
	dptab_assertValidTable((s)->groups);\
}

#else
#define dp_assertValidSessionContext(s)
#endif

/* standalone master if no game server set and on internet (can be changed to
 * include ipx later if dp->hGameServer is set to PLAYER_BROADCAST for ipx)
 * Note dp->select_keylen is set when connecting to standalone master. */
#define bIAmStandaloneMaster(dp)  \
	(((dp)->dpio->driverinfo->capabilities&comm_DRIVER_NO_BROADCAST) && \
	 ((dp)->hGameServer == PLAYER_NONE) && \
	 ((dp)->hMaster == PLAYER_ME))
#define bIAmConnectedToStandaloneMaster(dp) \
	(((dp)->hMaster != PLAYER_ME) && \
	 (dp)->select_keylen && \
	 ((dp)->hGameServer == PLAYER_NONE))
#define bIAmGameServer(dp)  \
	(((dp)->dpio->driverinfo->capabilities&comm_DRIVER_NO_BROADCAST) && \
	 ((dp)->hGameServer == PLAYER_NONE))

/* Forward declarations (ugh) */
static dp_result_t initOpenSession(dp_t *dp, dp_session_t *s, dp_session_t *sess);
static dp_result_t dp_setSessionTablePeer(dp_t *dp, playerHdl_t h, dp_species_t sessionType);
static dp_result_t dp_setSessionDesc( dp_t *dp, dp_session_t *buf, long flags);
static dpid_t dp_findLowestHost(dp_t *dp, clock_t maxAge);
static void dp_election_start(dp_t *dp, int hostDeleted);
static void dp_election_end(dp_t *dp);
static void dp_election_poll(dp_t *dp);
static void dp_election_peer_deleted(dp_t *dp, playerHdl_t h);
static dp_result_t dpHandleVote(dp_t *dp, playerHdl_t h, char *req, size_t reqlen);
static dp_result_t dpSendVote(dp_t *dp, dpid_t candidate, playerHdl_t dest);
static void dpElectMaster(dp_t *dp);
static dp_result_t dpSendVictory(dp_t *dp);
static int dp_getBroadcastHdls(dp_t *dp, playerHdl_t hdls[], int flags);
static int dp_getHdls(dp_t *dp, dpid_t idTo, playerHdl_t *dests, int flags);

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

#ifdef _DEBUG
static void printAdr(
	int adrLen,
	const unsigned char adr[dp_MAX_ADR_LEN])
{
	char buf[256];
	int i;

	buf[0] = 0;
	for (i=0; i<adrLen-1; i++)
		sprintf(buf + 3*i, "%02x:", ((unsigned char *)adr)[i]);
	sprintf(buf + 3*i, "%02x", ((unsigned char *)adr)[i]);
	DPRINT(("%s [", buf));

	buf[0] = 0;
	for (i=0; i<adrLen-1; i++)
		sprintf(buf + strlen(buf), "%d.", ((unsigned char *)adr)[i]);
	sprintf(buf + strlen(buf), "%d]", ((unsigned char *)adr)[i]);
	DPRINT(("%s", buf));
}
#else
#define printAdr(adrLen, adr)
#endif

#ifdef _DEBUG
static void dprint_sess(dp_t *dp, const dp_session_t *sess, char *msg)
{
	DPRINT(("%s: sessType %d, name %s, k%d u%x f%x, table %s, key %s, u ",
			msg,
			sess->sessionType, sess->sessionName,
			sess->karma, sess->dwUser1, sess->flags,
			key2a(dp->sessions->key, dp->sessions->keylen),
			key2a2(sess->reserved2, dp->dpio->myAdrLen+2)
			));
	printAdr(sizeof(sess->szUserField), sess->szUserField);
	DPRINT((", adr "));
	printAdr(dp->dpio->myAdrLen, sess->adrMaster);
	DPRINT(("\n"));

}

#else
#define dprint_sess(dp, sess, msg)
#endif

/*-------------------------------------------------------------------------
Debugging dump routines
-------------------------------------------------------------------------*/
#if !defined(NDEBUG)

/* Check a dp_t's session contexts for validity */
void dp_assertValidSessionContexts(dp_t* d)
{
	int i;
	int count = 0;

	/* All session contexts should be internally valid */
	/* One session context should be the current one */
	for(i = 0; i < d->sessionContexts->n_used; i++) {
		dp_sessionContext_t **ppS = (dp_sessionContext_t **)dynatab_subscript(
					d->sessionContexts, i);
		assert(ppS != NULL);
		dp_assertValidSessionContext(*ppS);
		if((*ppS)->players == d->players) {		/* if current */
			assert((*ppS)->hosts == d->hosts);
			assert((*ppS)->groups == d->groups);
			count++;
		}
	}
	assert((d->players == NULL) || (count == 1));	/* only one current session */

	/* Each handle's session should be in sessionContexts */
	for(i = 0; i < d->hdl2sess->n_used; i++) {
		int found = FALSE;
		dp_sessionContext_t **ppS = NULL;
		assoctab_item_t *pe = assoctab_getkey(d->hdl2sess, i);
		playerHdl_t h;
		int j;
		assert(pe != NULL);
		h = pe->key;
		ppS = (dp_sessionContext_t **) &pe->value;
		assert(ppS != NULL);
		for(j = 0; j < d->sessionContexts->n_used; j++) {
			dp_sessionContext_t **ppS2 = (dp_sessionContext_t **)dynatab_subscript(
						d->sessionContexts, j);
			assert(ppS2 != NULL);
			if(*ppS == *ppS2) {
				found = TRUE;
				break;
			}
		}
		assert(found);	/* Check if it was found */
	}
}

/* Check a dp_t for validity */
void dp_assertValid(dp_t* d)
{
	assert(d != NULL);
	dptab_assertValid(d->dt);
	dpio_assertValid(d->dpio);
	dp_assertValidSessionContexts(d);
	assert(d->quitState >= 0 && d->quitState < 10);

	/* Protect against access by multiple threads. */
#ifdef _WIN32
	assert(d->threadId == GetCurrentThreadId());
#endif
	ASSERTMEM();
}

#else
#define dp_assertValid(d)
#endif

/*-------------------------------------------------------------------------
Debugging dump routines
-------------------------------------------------------------------------*/
#if !defined(NDEBUG)

void
dumpAdrTable(
	dp_t *dp)
{
	int i;

	assert(dp != NULL);
	assert(dp->dpid2commHdl != NULL);

	DPRINT(("Address table of %d items:\n", dp->dpid2commHdl->n_used));

	for(i = 0; i < dp->dpid2commHdl->n_used; i++) {
		assoctab_item_t * item = assoctab_getkey(dp->dpid2commHdl, i);
		DPRINT(("   %2d:  dpid:%d = h:%x\n", i, item->key, *((playerHdl_t *)(item->value))));
	}
}

void dumpSessContext(dp_sessionContext_t *sess, int indent)
{
	char buf[120] = "";

	while(indent-- > 0)
		strcat(buf, "  ");

	if(sess == NULL) {
		DPRINT(("%scontext NULL\n", buf));
	} else {
		DPRINT(("%scontext %s in dp %p:\n", buf,
			key2a(sess->sess_subkey, sess->sess_subkeylen), sess->dp));
		DPRINT(("%s  Hosts:%s  Players:%s  Groups:%s\n", buf,
			(sess->hosts != NULL)?key2a(sess->hosts->key, sess->hosts->keylen):"NULL",
			(sess->players != NULL)?key2a2(sess->players->key, sess->players->keylen):"NULL",
			(sess->groups != NULL)?key2a3(sess->groups->key, sess->groups->keylen):"NULL"));
	}
}

void dumpSessionContexts(dp_t *dp)
{
	int i;
	DPRINT(("%d Session Contexts in dp %p:\n", dp->sessionContexts->n_used, dp));
	for(i = 0; i < dp->sessionContexts->n_used; i++) {
		dumpSessContext(*((dp_sessionContext_t **)dynatab_subscript(dp->sessionContexts, i)),1);
	}
}
#else
#define dumpSessionContexts(dp)
#define dumpSessContext(sess, indent)
#define dumpAdrTable(dp)
#endif

/*-------------------------------------------------------------------------
 Byte-swapping routines.
 This library runs on both bigendian and little-endian machines, so
 every object stuffed into a dptab table should have one of these
 so the bytes sent over the network always have a standard byte ordering.

 In fact, objects in dptab might always need to be in network byte order.
 Ack!
-------------------------------------------------------------------------*/

#ifdef dp_MUSTSWAP
static void dpSwapJoinPacketT(dp_join_packet_t *p, int len)
{
	p->joinKarma = SwapBytes2(p->joinKarma);
	p->sessionKarma = SwapBytes2(p->sessionKarma);
	if (len >= dp_V3_JOIN_PACKET_LEN) {
		p->sessionType = SwapBytes2(p->sessionType);
		p->flags = SwapBytes2(p->flags);
		p->mask = SwapBytes2(p->mask);
	}
}
static void dpSwapSelectSessionPacket(dp_select_sess_packet_t *p)
{
	dpSwapSessionPacket(p->sess);
	SwapBytes2(p->keylen);
}
#else
#define dpSwapJoinPacketT(host, len)
#define dpSwapSelectSessionPacket(p)
#endif
#define dpSwapLeavePacketT(p, len) dpSwapJoinPacketT(p, len)

/*----------------------------------------------------------------------
 Convert a dpid into a playerHdl.

 Works only for the default session.
 Looks up the comm handle associated with the block of dpid's containing
 the specified id.
----------------------------------------------------------------------*/
playerHdl_t dpid2commHdl(
	dp_t *dp,
	dpid_t id)
{
	playerHdl_t *pH;
	dpid_t firstId;

	dp_assertValid(dp);

	if (!dp->dpid2commHdl) {
		DPRINT(("dpid2commHdl: no table!\n"));
		dp_assertValid(dp);
		return PLAYER_NONE;
	}
	firstId = (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1));	/* Skip back to firstId */
	pH = (playerHdl_t *)assoctab_subscript(dp->dpid2commHdl, firstId);
	if (!pH) {
		/*DPRINT(("dpid2commHdl: no entry for %d!\n", id)); */
		dp_assertValid(dp);
		return PLAYER_NONE;
	}
	DPRINT(("dpid2commHdl(%d) returns %d\n", id, *pH));
	dp_assertValid(dp);
	return *pH;
}

/*----------------------------------------------------------------------
 Look up the last dpid that is associated with the given comm handle.
 On failure, return dp_ID_NONE.
----------------------------------------------------------------------*/
dpid_t dp_commHdl2dpid(
	dp_t *dp,
	playerHdl_t h)
{
	int i;

	if (!dp->dpid2commHdl)
		return dp_ID_NONE;

	for (i=dp->dpid2commHdl->n_used-1; i >= 0; i--) {
		playerHdl_t *pH;
		assoctab_item_t *pe;
		pe = assoctab_getkey(dp->dpid2commHdl, i);
		if (!pe)
			break;
		pH = (playerHdl_t *) &pe->value;
		if (*pH == h) {
			DPRINT(("dp_commHdl2dpid(%d): found id %d\n", h, pe->key));
			return (dpid_t) pe->key;
		}
	}
	DPRINT(("dp_commHdl2dpid(%d): didn't find id\n", h));
	return dp_ID_NONE;
}

/*----------------------------------------------------------------------
 Remember what commHdl goes with the block of dpid's containing
 the specified id.
 To cancel an assignment, call with h = PLAYER_NONE.
----------------------------------------------------------------------*/
void commHdl4dpid(
	dp_t *dp,
	dpid_t id,
	playerHdl_t h)
{
	playerHdl_t *pH;
	dpid_t firstId;

	dp_assertValid(dp);

	if (!dp->dpid2commHdl) {
		DPRINT(("commHdl4dpid: no table!\n"));
		dp_assertValid(dp);
		return;
	}
	firstId = (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1));	/* Skip back to firstId */
	if (h == PLAYER_NONE) {
		int err = assoctab_subscript_delete(dp->dpid2commHdl, firstId);
		DPRINT(("commHdl4dpid: deleting commHdl[%d..%d]; fail:%d\n",
			firstId, firstId+dp_PLAYERS_PER_HOST-1, err));
		dp_assertValid(dp);
		return;
	}

	pH = (playerHdl_t *)assoctab_subscript_grow(dp->dpid2commHdl, firstId);
	if (!pH) {
		DPRINT(("commHdl4dpid: bug\n"));
		dp_assertValid(dp);
		return;
	}
	DPRINT(("commHdl4dpid: assigning commHdl[%d..%d] = %d\n",
		firstId, firstId+dp_PLAYERS_PER_HOST-1, h));

	/* Quick sanity check.
	 * I don't think zero is an officially reserved value
	 * for playerHdl_t, but the drivers we have do seem to avoid using
	 * handle 0.
	 */
	if (*pH && (*pH != PLAYER_NONE) && (*pH != h)) {
		DPRINT(("commHdl4dpid: commHdl[%d..%d] was already %d: bug!\n",
				firstId, firstId+dp_PLAYERS_PER_HOST-1, *pH, h));
	}
	*pH = h;
	dp_assertValid(dp);
}

/*----------------------------------------------------------------------
 Somebody requested something they couldn't have.  Send an error packet.
----------------------------------------------------------------------*/
static dp_result_t dpSendErrorPacket(
	dp_t *dp,
	playerHdl_t dest,
	dp_packetType_t failed_request,
	dp_karma_t karma,
	dp_result_t failure_reason)
{
	struct {
		dp_packetType_t   tag;
 		dp_error_packet_t body;
	} PACK pkt;

	dp_assertValid(dp);
	pkt.tag = dp_ERROR_PACKET_ID;
	pkt.body.request = failed_request;
	pkt.body.karma = karma;
	pkt.body.err = failure_reason;

	DPRINT(("dpSendErrorPacket: dest %d, failed_request %x, reason e:%d\n",
		dest, failed_request, failure_reason));
	/* call dpSwapErrorPacket to byte swap pkt.body */
	dpSwapErrorPacket(&pkt.body);
	dp_assertValid(dp);
	return dpio_put_reliable(dp->dpio, &dest, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
}

/*----------------------------------------------------------------------
 Send the local user code a message telling it the session is no more.
*----------------------------------------------------------------------*/
static void dpSendSessionLostPacket(dp_t *dp)
{
	/* Send our client a message */
	playerHdl_t h = PLAYER_ME;
	struct {
		dp_packetType_t			tag;
		dp_sessionLost_packet_t	body;
	} PACK pkt;

	dp_assertValid(dp);
	memset(&pkt.body, 0, sizeof(pkt.body));	/* in case get fails */
	pkt.tag = dp_SESSIONLOST_PACKET_ID;
	dpGetSessionDesc(dp, &pkt.body, 0);
	DPRINT(("dpSendSessionLostPacket: Session lost\n"));
	/* Do not swap when sending or receiving local message */
	dpio_put_reliable(dp->dpio, &h, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
	dp_assertValid(dp);
}

/*----------------------------------------------------------------------
 Notify the caller of the success or failure of his attempt to join a
 session using the callback passed to dpOpen (if any) and a local packet
 indicating success or failure and information about the session joined.

 If there are no internal errors, dp_RES_OK is returned.
----------------------------------------------------------------------*/
static dp_result_t dpOpenLocalResult(dp_t *dp, dp_result_t reason, dpid_t id)
{
	dp_result_t err;
	dp_result_t dpioerr;
	dpEnumSessionsCallback_t cb;
	static dp_session_t sess;  /* static in case of bad habits */
	dp_session_t *pSess;
	playerHdl_t me = PLAYER_ME;
	struct {
		dp_packetType_t tag;
		dp_sessionResult_packet_t body;
	} pkt;

	assert(dp->dpOpen_cb != NULL);
	cb = dp->dpOpen_cb;
	dp->dpOpen_cb = NULL;  /* flag: our open is no longer in progress */

	err = dp_RES_OK;
	pSess = &sess;
	pkt.tag = dp_SESSIONRESULT_PACKET_ID;
	memset(&pkt.body, 0, sizeof(dp_sessionResult_packet_t));
	pkt.body.reason = reason;
	if (reason == dp_RES_OK) {
		DPRINT(("dpOpenLocalResult: t:%d session joined\n", *(dp->dpio->now)));
		err = dpGetSessionDesc(dp, &sess, 0);
		if (err == dp_RES_OK) {
			memcpy(&pkt.body.sess, &sess, sizeof(dp_session_t));
		} else {
			DPRINT(("dpOpenLocalResult: bug: can't get session record\n"));
			pSess = NULL;
			dp->pleaseClose = TRUE;	/* close on next dpPoll */
			pkt.body.reason = dp_RES_BUG;
			err = dp_RES_BUG;
		}
	} else {
		DPRINT(("dpOpenLocalResult: t:%d session join failed, reason:%d\n",
				*(dp->dpio->now), reason));
		pSess = NULL;
		dp->pleaseClose = TRUE;
	}

	DPRINT(("dpOpenLocalResult: calling callback %p\n", cb));
	cb(pSess, NULL, 0, dp->dpOpen_context);
	dpioerr = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
	if (dpioerr != dp_RES_OK) {
		DPRINT(("dpOpenLocalResult: can't send local msg, err:%d\n", dpioerr));
		return dpioerr;
	}
	return err;
}

/*----------------------------------------------------------------------
 Callback for opening dpio handles which insures that necessary cleanup
 happens on remote close
----------------------------------------------------------------------*/
static void dp_openHdl_cb(
	playerHdl_t h, int n_hdls, dp_result_t status, void *context)
{
	dp_t *dp = (dp_t *) context;
	DPRINT(("dp_openHdl_cb: h:%x status:%d t:%d\n", h, status, *(dp->dpio->now)));

	/* Figure out which session, if any, the handle was involved in. */
	if (dp) {
		dp_assertValid(dp);
		if ((status == dp_RES_PEER_CLOSED) || (status == dp_RES_CLOSED)) {
			int i;
			dptab_peer_t *peer;
			dp_sessionContext_t **spp;

			/*----------- start of stuff moved from dpPoll -------------*/
			dpid_t firstId;

			/* Disqualify current master from election.
			 * Also don't wait for dead machines to vote.
			 */
			firstId = dp_commHdl2dpid(dp, h);
			DPRINT(("dp_openHdl_cb: handle h:%x closing; hmaster h:%x; first_id:%d\n", h, dp->hMaster, firstId));
			if (dp->hosts) {
				char subkey[2];
				DPRINT(("dp_openHdl_cb: deleting host record for h:%x; first_id:%d\n", h, firstId));
				subkey[0] = (char) dpGETSHORT_FIRSTBYTE(firstId);
				subkey[1] = (char) dpGETSHORT_SECONDBYTE(firstId);
				dptab_delete(dp->dt, dp->hosts, subkey, 2);
			}
			if (h == dp->hMaster) {
				dp_session_t sess;
				dpGetSessionDesc(dp, &sess, 0);
				DPRINT(("dp_openHdl_cb: master h:%x closed; flags %x\n", h, sess.flags));
				/* report dpOpen failure if master goes away */
				if (dp->dpOpen_cb) {
					dpOpenLocalResult(dp, dp_RES_HOST_NOT_RESPONDING, dp_ID_NONE);
				} else if (sess.flags & (dp_SESSION_FLAGS_ISSERVER | dp_SESSION_FLAGS_MIGRATE_FORBIDDEN)) {
					DPRINT(("dp_openHdl_cb: session not allowed to migrate, closing\n"));
					dp->pleaseClose = TRUE;

				} else if (!dp->closing
					   &&  (firstId != dp_ID_NONE)) {
					/* If session was already joined, find new master. */
					/* Must elect new master before finishing closing old
					 * master.
					 */
					dp_election_start(dp, TRUE);
					dpElectMaster(dp);
				}
			}
			/*----------- end of stuff moved from dpPoll -------------*/

			/* Somebody closed a connection to us.  Make sure anything they
			 * were doing is cleaned up.  Especially make sure that
			 * any session records they pushed us get deleted!
			 * Player variable tables get deleted during hosts callback.
			 */
			dptab_delete_bySrc(dp->dt, dp->mysessions, h);
			dptab_delete_bySrc(dp->dt, dp->sessions, h);

			spp = (dp_sessionContext_t **) assoctab_subscript(dp->hdl2sess, h);
			DPRINT(("dp_openHdl_cb: hdl2sess[h:%x] = *%p= %p\n", h, spp, spp?*spp:NULL));
			if (spp && *spp) {
				if (h != dp->hMaster) {
					/* Don't delete any tables we migrate */
					DPRINT(("dp_openHdl_cb: sp %p; h:%x was in a session; deleting\n", *spp, h));
					dptab_delete_bySrc(dp->dt, (*spp)->players, h);
					dptab_delete_bySrc(dp->dt, (*spp)->myplayers, h);
					dptab_delete_bySrc(dp->dt, (*spp)->hosts, h);

					for (i = 0; i < dp_MAX_GROUPS; i++) {
						dptab_delete_bySrc(dp->dt, (*spp)->grplayers[i], h);
					}

					dptab_delete_bySrc(dp->dt, (*spp)->groups, h);
				}
				assoctab_subscript_delete(dp->hdl2sess, h);
			}

			/* Delete the peer and any tables that won't be cleared up
			 * by dpPoll() when it notices the handle's being closed.
			 */
			peer = (dptab_peer_t *)assoctab_subscript(dp->dt->peers, h);
			if(peer != NULL) {
				dptab_assertValidPeer(peer);
				/* Clean up after dp_RemoteEnumPlayers() by deleting any player
				 * table published from this handle that isn't just
				 * his myplayers table & that isn't in use by the
				 * current session
				 */
				for (i=0; i<peer->pubs->n_used; i++) {
					hkeytab_item_t *pe;
					dptab_table_t **sub;
					pe = hkeytab_getkey(peer->pubs, i);
					condition(pe != NULL, "corrupted pubs table");
					sub = (dptab_table_t **)pe->value;
					/*DPRINT(("dp_openHdl_cb: table %s, remote
						key %s, h:%x\n",
						key2a((*sub)->key, (*sub)->keylen),
						key2a2(pe->key, pe->keylen), h)); */
					if ((((*sub)->key)[0] == dp_KEY_PLAYERS)
					&& ((pe->key)[0] == dp_KEY_PLAYERS)
					&&	(*sub != dp->players)) {
						DPRINT(("dp_openHdl_cb: deleting remote player table\n"));
						dptab_deleteTable(dp->dt, (*sub)->key, (*sub)->keylen);
					}
				}
				DPRINT(("dp_openHdl_cb: h:%x, hMaster:%x; deletePeer if not same\n", h, dp->hMaster));
				if (h != dp->hMaster)
					dptab_deletePeer(dp->dt, h);
			}

			/* Finally, take the peer out of any ongoing election. */
			dp_election_peer_deleted(dp, h);
		}
		/* Check for a user callback */
		if (dp->openConn_callback) {
			int len = dp_MAX_ADR_LEN;
			static unsigned char adr[dp_MAX_ADR_LEN];
			dp_result_t err = dp_RES_OK;

			memset(adr, 0, sizeof(adr));
			/*
			err = hdl2adr(h, adr, &len);
			if (err != dp_RES_OK)
			*/
				len = 0;
			DPRINT(("dp_openHdl_cb: calling user callback\n"));
			/* Pass handle as password field for now.  Yech. */
			dp->openConn_callback((err != dp_RES_OK) ? NULL : adr,
				len, n_hdls, status, NULL, (char *)h, dp->openConn_context);
		}
		dp_assertValid(dp);
	}
}

#if 1

/*----------------------------------------------------------------------
 Pack a dp_session_t into a compact, uniform byte order form for transmission.
 Doesn't need to send external address or karma, but
 will need to send internal address if it's not the same as external.
 If any field is too long, it is truncated.
 Returns length used.
----------------------------------------------------------------------*/
DP_API int dp_pack_session(dp_t *dp, dp_species_t defaultSessionType, const dp_session_t *p, char *buf)
{
	char *q = buf;
	const char *s;
	char *lenp;
	int len;
	int subkeylen = dp->dpio->myAdrLen + sizeof(dp_karma_t);

	DPRINT(("dp_pack_session: deftype %d, ", dp->defaultSessionType));
	dprint_sess(dp, p, "");
	if (defaultSessionType != p->sessionType) {
		size_t len;

		len = sizeof(*p) - sizeof(p->reserved2);
		memcpy(buf, p, len);
		dpSwapSessionPacket((dp_session_t*)buf);
		return len;
	}
	*q++ = (char)p->maxPlayers;
	*q++ = (char)p->currentPlayers;
	*q++ = (char)(p->flags);

	lenp = q++;	/* skip len */
	for (len=0, s=p->sessionName; (len < dp_SNAMELEN-1) && *s; len++)
		*q++ = *s++;
	*lenp = len;

	lenp = q++;	/* skip len */
	for (len=0, s=p->szPassword; (len < dp_PASSWORDLEN-1) && *s; len++)
		*q++ = *s++;
	*lenp = len;

	/* concatenate user fields and strip trailing 0's */
	lenp = q++;	/* skip len */
	q[0] = dpGETLONG_FIRSTBYTE(p->dwUser1);
	q[1] = dpGETLONG_SECONDBYTE(p->dwUser1);
	q[2] = dpGETLONG_THIRDBYTE(p->dwUser1);
	q[3] = dpGETLONG_FOURTHBYTE(p->dwUser1);
	memcpy(q+4, p->szUserField, dp_USERFIELDLEN);
	for (len=sizeof(p->szUserField)+sizeof(p->dwUser1); len > 0; len--)
		if (q[len-1])
			break;
	*lenp = len;
	q += len;

	DPRINT(("dp_pack_session: subkey %s, adrMaster ",
			key2a2(p->reserved2, subkeylen)));
	printAdr(dp->dpio->myAdrLen, p->adrMaster);
	/*dumpBuf(buf, (q - buf));*/
	DPRINT(("\n"));
	if (memcmp(p->reserved2, p->adrMaster, dp->dpio->myAdrLen)) {
		/* session has migrated.  Send new master address. */
		memcpy(q, p->adrMaster, dp->dpio->myAdrLen);
		q += dp->dpio->myAdrLen;
		DPRINT(("dp_pack_session: session has migrated: new adr "));
		printAdr(dp->dpio->myAdrLen, p->adrMaster);
		DPRINT(("\n"));
	}
	len = (q - buf);
	if (len == sizeof_dp_session_packet_t) {
		/* uh-oh, don't want to confuse dp_unpack_session; use old method */
		len = dp_pack_session(dp, 0, p, buf);
		assert(len == sizeof_dp_session_packet_t);
	}
	return len;
}

/*----------------------------------------------------------------------
 Unpack the compact, byte-order-uniform version of a dp_session_t
 into the fluffy form we use internally.
 Returns number of bytes used, or -1 on error.
----------------------------------------------------------------------*/
DP_API int dp_unpack_session(dp_t *dp, const char *subkey, int subkeylen, const char *buf, size_t buflen, dp_session_t *p)
{
	const unsigned char *q = (const unsigned char *)buf;
	int len;
	unsigned char userbuf[sizeof(p->szUserField)+sizeof(p->dwUser1)];
	dp_assertValid(dp);

	/* Handle old uncompressed records */
	if (buflen == sizeof_dp_session_packet_t) {
		assert(subkeylen == (dp->dpio->myAdrLen + 2));
		len = sizeof(*p) - sizeof(p->reserved2);
		memcpy(p, buf, len);
		memcpy(p->reserved2, subkey, subkeylen);
		dpSwapSessionPacket(p);
		dprint_sess(dp, p, "dp_unpack_session (old)");
		return len;
	}

	/*DPRINT(("dp_unpack_session: "));
	dumpBuf(buf, (q - buf));*/

	memset(p, 0, sizeof(*p));
	p->karma = dpMAKESHORT(subkey[dp->dpio->myAdrLen],subkey[dp->dpio->myAdrLen+1]);
	memcpy(p->adrMaster, subkey, dp->dpio->myAdrLen);
	memcpy(p->reserved2, subkey, subkeylen);

	assert(dp->defaultSessionType != 0);
	p->sessionType = dp->defaultSessionType;
	p->maxPlayers = *(unsigned char *)q++;
	p->currentPlayers = *(unsigned char *)q++;
	p->flags = *q++ & 255;

	len = *q++;
	if (len > dp_SNAMELEN) {
		dp_assertValid(dp);
		return -1;
	}
	memcpy(p->sessionName, q, len);
	p->sessionName[len] = 0;
	q += len;

	len = *q++;
	if (len > dp_PASSWORDLEN) {
		dp_assertValid(dp);
		return -1;
	}
	memcpy(p->szPassword, q, len);
	p->szPassword[len] = 0;
	q += len;

	len = *q++;
	memset(userbuf, 0, sizeof(userbuf));
	memcpy(userbuf, q, len);
	q += len;
	p->dwUser1 = (long) dpMAKELONG(userbuf[0], userbuf[1], userbuf[2], userbuf[3]);
	/*DPRINT(("dp_unpack_session: dwuser1 %02x%02x%02x%02x = %08x\n", userbuf[0], userbuf[1], userbuf[2], userbuf[3], p->dwUser1));*/

	len -= 4;
	if (len > dp_USERFIELDLEN)
		len = dp_USERFIELDLEN;
	if (len > 0)
		memcpy(p->szUserField, userbuf+4, len);

	len = buflen - ((size_t)q - (size_t)buf);
	if (len > 0) {
		/* session has migrated.  Receive new master address. */
		assert(len >= dp->dpio->myAdrLen);
		memcpy(p->adrMaster, q, dp->dpio->myAdrLen);
		DPRINT(("dp_unpack_session: session has migrated: new adr "));
		printAdr(dp->dpio->myAdrLen, p->adrMaster);
		DPRINT(("\n"));
		q += dp->dpio->myAdrLen;
	}

	dprint_sess(dp, p, "dp_unpack_session (new)");
	dp_assertValid(dp);
	return ((size_t)q - (size_t)buf);
}
#else

/*----------------------------------------------------------------------
 Pack a dp_session_t into a compact, uniform byte order form for transmission.
 Don't send subkey.
 Returns length used.
----------------------------------------------------------------------*/
static int dp_pack_session(dp_t *dp, const dp_session_t *p, char *buf)
{
	size_t len;

	dprint_sess(dp, p, "dp_pack_session");
	len = sizeof(*p) - sizeof(p->reserved2);
	memcpy(buf, p, len);
	dpSwapSessionPacket((dp_session_t*)buf);
	return len;
}

/*----------------------------------------------------------------------
 Unpack the compact, byte-order-uniform version of a dp_session_t
 into the fluffy form we use internally.
 All this does right now is add back in the subkey.
 Returns number of bytes used, or -1 on error.
----------------------------------------------------------------------*/
DP_API int dp_unpack_session(dp_t *dp, const char *subkey, int subkeylen, const char *buf, dp_session_t *p)
{
	size_t len;

	assert(subkeylen == (dp->dpio->myAdrLen + 2));
	len = sizeof(*p) - sizeof(p->reserved2);
	memcpy(p, buf, len);
	memcpy(p->reserved2, subkey, subkeylen);
	dpSwapSessionPacket(p);
	dprint_sess(dp, p, "dp_unpack_session");
	return len;
}
#endif

/*----------------------------------------------------------------------
 Callback for addition/deletion of session entries.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_sessions_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp = (dp_t *)context;
	dp_session_t sess;

	if (!dp || !buf)
		return 0;
	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED))
		return 0;

	dp_assertValid(dp);

	/* Must unpack when retrieving records from dptab */
	dp_unpack_session(dp, subkey, subkeylen, buf, total, &sess);
	DPRINT(("dp_sessions_cb: status %d, buf %p, len %d, name %s, type %d\n", status, buf, total, sess.sessionName, sess.sessionType));
	dumpBuf(buf, total);

	/* If we are a game server, look up the user's uid and add them to the
	 * dp->uid2sessionid table.  In the future, the uid should come from the
	 * dptab data packet instead.
	 */
	if (bIAmGameServer(dp) && status == dp_RES_CREATED) {
		dp_result_t err;
		dp_uid_t uid;

		uid = tserv_hdl2uid(dp->tserv, src);
		DPRINT(("dp_sessions_cb: recording sessid4uid(%d)\n", uid));
		err = dp_sessid4uid(dp, uid, sess.reserved2, dp->dpio->myAdrLen+2, sess.sessionType);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessions_cb: can't record session for h:%x uid:%d, err:%d\n", src, uid, err));
		}
	}

	if (dp->monitor_object_sessions) {
		dp_result_t err;
		err = dpSendObjectDelta(dp, status, (dp_object_t *) &sess, table, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessions_cb: can't send object delta, err:%d\n", err));
		}
	}

	/* Save the current session's latest record for use during elections */
	if ((dp->hMaster != PLAYER_NONE) && !memcmp(subkey, dp->sess_subkey, dp->sess_subkeylen)) {
		if (status == dp_RES_CHANGED) {
			dprint_sess(dp, &sess, "dp_sessions_cb saving");
			dp->election_old_session = sess;
		} else if (status == dp_RES_DELETED) {
			DPRINT(("dp_sessions_cb: current session deleted.\n"));
			if ((dp->hGameServer != PLAYER_NONE)
			&&  (dp->hMaster == PLAYER_ME)
			&&  !dp->closing) {
				dp_result_t err;

#if 1
				/* Advertise the session to the world */
#ifdef DONT_RETAIN_SESSFLAGS
				dp->election_old_session.flags |= dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
#endif
				memcpy(dp->election_old_session.adrMaster, dp->dpio->myAdr, dp->dpio->myAdrLen);
				DPRINT(("dp_sessions_cb: new adrMaster "));
				printAdr(dp->dpio->myAdrLen, dp->election_old_session.adrMaster);
				DPRINT(("\n"));
				dp->election_old_session.currentPlayers = dpNumPlayers(dp);
				err = dp_setSessionDesc(dp, &dp->election_old_session, 0);
				if (err != dp_RES_OK) {
					DPRINT(("dp_sessions_cb: can't announce session\n"));
					return dp_RES_BUG;
				}
#endif
			}
		}
	}
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED)) {
		/* Not interested in session deletions etc */
		return 0;
	}

	/* If a session was added, and it matches the user's filter, call the
	 * user's callback.
	 */
	if (dp->enumSessions_callback) {
		long timeout_dummy;

		/* Report only sessions that allow new players, and have same type unless wildcard (0) specified */
		if (
#ifdef IGNORE_CLOSEDSESS
			!(sess.flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS) ||
#endif
			(dp->enumSessions_sessType && (dp->enumSessions_sessType != sess.sessionType))) {
			dp_assertValid(dp);
			return dp_RES_OK;
		}

		/* for compatibility with mercnet's netshell, tell caller
		 * whether this is a local session
		 */
		sess.dummy = (char) ((src == PLAYER_ME) ? PLAYER_ME : 0);

		dprint_sess(dp, &sess, "dp_sessions_cb calling");
		(dp->enumSessions_callback)(&sess, &timeout_dummy, 0, dp->enumSessions_context);
	}

	(void) seconds_left;
	(void) total;
	(void) sent;
	(void) subkeylen;
	(void) subkey;
	(void) dest;
	(void) table;
	(void) dptab;

	dp_assertValid(dp);
	return 0;
}

/*----------------------------------------------------------------------
 Unpack the compact, byte-order-uniform version of a dp_host_t
 into the fluffy form we use internally.
 Since the record's key is firstId, no need to send that.
 Reduces record size from 18 to 10 bytes if adrlen is 4, so we can get
 21 instead of 12 of these into one packet.
 Returns number of bytes used, or -1 on error.
----------------------------------------------------------------------*/
static int dp_unpack_host(dpid_t firstId, int myAdrLen, const char *buf, dp_host_t *p, unsigned int length)
{
	const unsigned char *q = (const unsigned char *)buf;

	/*
	 * Make sure that the package is of a valid size
	 */
	assert((length == (unsigned)(2 + myAdrLen)) || (length == (unsigned)(2 + myAdrLen + myAdrLen)));

	p->joinKarma = (dp_karma_t) dpMAKESHORT(q[0], q[1]);
	q += 2;

	/* Rip the first address out */
	memcpy(p->iadr, q, myAdrLen);
	q += myAdrLen;

	DPRINT(("dp_unpack_host: jk %d, fi %d, len %d iadr ", p->joinKarma, p->firstId, length));
	printAdr(myAdrLen, p->iadr);

	/* Depending upon the size of this host a second address could be embeded */
	if (length > ((size_t)q - (size_t)buf)) {
		memcpy(p->iadr2, q, myAdrLen);
		q += myAdrLen;

		DPRINT((", iadr2 "));
		printAdr(myAdrLen, p->iadr2);
	} else {
		memcpy(p->iadr2, p->iadr, myAdrLen);
	}

	p->firstId = firstId;
	DPRINT(("\n"));

	return ((size_t)q - (size_t)buf);
}

/*----------------------------------------------------------------------
 Pack a dp_host_t into a compact, uniform byte order form for transmission.
 Returns length used.
----------------------------------------------------------------------*/
static int dp_pack_host(int myAdrLen, const dp_host_t *p, char *buf)
{
	char *q = buf;

	DPRINT(("dp_pack_host: jk %d, fi %d, iadr ", p->joinKarma, p->firstId));
	printAdr(myAdrLen, p->iadr);

	*q++ = (char) dpGETSHORT_FIRSTBYTE(p->joinKarma);
	*q++ = (char) dpGETSHORT_SECONDBYTE(p->joinKarma);

	/* Could even avoid sending iadr if it's same as oadr. */
	memcpy(q, p->iadr, myAdrLen);
	q += myAdrLen;

	/* If the second address is different then attach it */
	if (memcmp(p->iadr, p->iadr2, myAdrLen)) {
		memcpy(q, p->iadr2, myAdrLen);
		q += myAdrLen;

		DPRINT((", iadr2 "));
		printAdr(myAdrLen, p->iadr2);
	}
	DPRINT(("\n"));
	return ((size_t)q - (size_t)buf);
}

/*----------------------------------------------------------------------
 Pack a dp_playerId_t into a compact, uniform byte order form for transmission.
 If any field is too long, it is truncated.
 Returns length used.
----------------------------------------------------------------------*/
static int dp_pack_playerId(const dp_playerId_t *p, char *buf)
{
	char *q = buf;
	const char *s;
	char *lenp;
	int len;

	*q++ = (char) dpGETSHORT_FIRSTBYTE(p->karma);
	*q++ = (char) dpGETSHORT_SECONDBYTE(p->karma);
	lenp = q++;	/* skip len */
	for (len=0, s=p->name; (len < dp_PNAMELEN-1) && *s; len++)
		*q++ = *s++;
	*lenp = (char) len;
	len = p->bloblen;
	if (len > dp_MAX_PLAYERBLOB_LEN)
		len = dp_MAX_PLAYERBLOB_LEN;
	*q++ = (char) len;
	memcpy(q, p->blob, len);
	q += len;

#if 0
	DPRINT(("dp_pack_playerId: len %d: karma %d bloblen %d name %s\n",
			(q - buf), p->karma, p->bloblen, p->name));
	DPRINT(("dp_pack_playerId: input:"));
	dumpBuf((char *)p, sizeof(*p));
	DPRINT(("dp_pack_playerId: output:"));
	dumpBuf(buf, q - buf);
#endif

	return (q - buf);
}

/*----------------------------------------------------------------------
 Unpack the compact, byte-order-uniform version of a dp_playerId_t
 into the fluffy form we use internally.
 Does not fill in address field; that has to be looked up in the hosts table.
 Returns number of bytes used, or -1 on error.
----------------------------------------------------------------------*/
int dp_unpack_playerId(dpid_t id, const char *buf, dp_playerId_t *p)
{
	const unsigned char *q = (const unsigned char *)buf;
	int len;

	p->karma = (dp_karma_t) dpMAKESHORT(q[0], q[1]);
	q += 2;

	len = *q++;
#if 0
	DPRINT(("dp_unpack_playerId(id %d): name len %d, expected max %d\n",
			id, len, dp_PNAMELEN));
	dumpBuf(buf, 20);
#endif
	if (len > dp_PNAMELEN) {
		DPRINT(("dp_unpack_playerId: name too long\n"));
		return -1;
	}
	memcpy(p->name, q, len);
	p->name[len] = 0;
	q += len;

	len = *q++;
	if (len > dp_MAX_PLAYERBLOB_LEN) {
		DPRINT(("dp_unpack_playerId: blob len %d, expected max of %d\n", len, dp_MAX_PLAYERBLOB_LEN));
		return -1;
	}
	p->bloblen = len;
	memcpy(p->blob, q, len);
	q += len;

	p->id = id;

#if 0
	DPRINT(("dp_unpack_playerId(id %d): name %s\n", id, p->name));
#endif

  return ((size_t)q - (size_t)buf);
}

/*--------------------------------------------------------------------------
 A player has entered or exited the game.
 Notify the user code.
 The player record is unpacked but its address field is empty.
 Fill it in for the benefit of that one application that depends on it.

 Called by the hosts and players tables callbacks only; the status parameter
 is the status parameter the callback was called with.
--------------------------------------------------------------------------*/

static void dp_player_notify(dp_t *dp, dp_playerId_t *pp, dp_host_t *phost, dp_result_t status)
{
	playerHdl_t me;
	dp_result_t err;

	dp_assertValid(dp);

	DPRINT(("dp_player_notify: id %d, name %s, status %d\n", pp->id, pp->name, status));
	if (status == dp_RES_CREATED) {
		if (dpid2commHdl(dp, phost->firstId) == PLAYER_NONE) {
			DPRINT(("dp_player_notify: bug: no handle for firstId %d yet\n", phost->firstId));
#if 0
		/* Open a comm handle to this fellow, and
		 * Remember the comm handle for this range of id's.
		 * FIXME: to work behind firewalls, might need to open oadr
		 * instead.  Probably need to pass both addresses to dpio and
		 * let it sort the issue out.
		 */
			playerHdl_t h;
			h = dpio_openHdl(dp->dpio, phost->iadr, dp_openHdl_cb, dp);
			commHdl4dpid(dp, phost->firstId, h);
#endif
		}
	}

	/* Should we use iadr or oadr?  Can't tell until we try to connect.
	 * For now, just guess iadr.  We won't have oadr until dpio
	 * knows how to get address of sender of packets from strangers.
	 * We'll have to deal with this before we work over firewalls.  FIXME
	 */
	memcpy(pp->adr, phost->iadr, dp->dpio->myAdrLen);
	/* This address should not be used, let's set it to zero to see what happens :)
	   Well, what happens is DR1.3 ignore feature goes to pot.
	memset(pp->adr, 0x00, dp->dpio->myAdrLen); */

	/* If a player was added, and it was us, call dpCreatePlayer's callback.
	 *
	 * It's not enough to match our internal address, because
	 * others might be behind different firewalls, and share the
	 * same internal address.
	 * Um, for the moment, just make sure it has the expected id.  FIXME
	 */
	if ((pp->id & ~(dp_PLAYERS_PER_HOST - 1)) == dp->firstId) {

		if (pp->id != dp->nextId-1) {
			DPRINT(("dp_player_notify: bug: id %d != nextId-1 %d\n", pp->id, dp->nextId-1));
		}
		if (status == dp_RES_CREATED) {
			DPRINT(("dp_player_notify: local player created, calling callback.  t:%d, id %d, name %s, status %d\n",
					*(dp->dpio->now), pp->id, pp->name, status));
			if (dp->dpCreatePlayer_cb)
				dp->dpCreatePlayer_cb(pp->id,
				 pp->name,
				 dp_EPC_FLAGS_LOCAL,
				 dp->dpCreatePlayer_context);
			/* clear callback so user can make next call to createPlayer. */
			dp->dpCreatePlayer_cb = NULL;
		} else if (status == dp_RES_DELETED) {
			/* Ulp - our player was deleted.
			 * This can only happen if the host kicked us out?
			 */
			DPRINT(("dp_player_notify: local player deleted\n"));
		}
	}

	/* Inform the user code of the addition/deletion with a local message. */
	if (dptab_getTableContext(dp->players, dp_rplayers_cb)) {
		/* they want a new style message instead of an old one */
		char subkey[2];
		int subkeylen;
		subkeylen = 0;
		subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(pp->id);
		subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(pp->id);
		err = dpSendObjectDelta(dp, status, (dp_object_t *) pp, dp->players, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dp_player_notify: can't send object delta, err:%d\n", err));
		}
		return;
	}
	if (status == dp_RES_CREATED) {
		struct {
			dp_packetType_t   tag;
			dp_user_addPlayer_packet_t body;
		} PACK pkt;

		/*DPRINT(("dp_player_notify: player added, sending message...\n"));*/

		pkt.tag = dp_USER_ADDPLAYER_PACKET_ID;
		pkt.body = *pp;

		me = PLAYER_ME;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_player_notify: can't send local msg, err:%d\n", err));
		}

	} else if (status == dp_RES_DELETED) {
		struct {
			dp_packetType_t   tag;
			dp_delPlayer_packet_t body;
		} PACK pkt;
		DPRINT(("dp_player_notify: player deleted, sending message...\n"));

		pkt.tag = dp_USER_DELPLAYER_PACKET_ID;
		pkt.body = *pp;

		me = PLAYER_ME;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_player_notify: can't send local msg, err:%d\n", err));
		}
	}
	dp_assertValid(dp);
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of player variable entries.

 Can handle multiple sessions.  Context pointer for this callback is a pointer
 to a structure holding pointers to this session's tables.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_playervars_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	dp_sessionContext_t *pSess = (dp_sessionContext_t *)context;
	dp_t *dp;
	dpid_t id;
	dp_result_t err;
	playerHdl_t me = PLAYER_ME;

	dptab_assertValid(dptab);

	DPRINT(("dp_playervars_cb: status:%d, src h:%x, dest h:%x, table %s, subkey %s\n",
			status, src, dest,
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));

	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED)) {
		return 0;
	}
	if (status == dp_RES_DELETED) {
		/* ???????? don't do anything ?????? */
		return dp_RES_OK;
	}

	if (!context || !buf) {
		DPRINT(("dp_playervars_cb: bug: NULL context=%p, buf=%p!\n",context,buf));
		return 0;
	}
	dp = pSess->dp;
	dp_assertValid(dp);

	/**** Notify the user code of the new player variable ****/

	/* Ignore sessions that are cast off.  They don't care about new player
	 * variables. */
	{	dptab_table_t *playervars;
		char key[dptab_KEY_MAXLEN];
		int keylen = 0;
		key[keylen++] = dp_KEY_PLAYER_VARIABLES;
		key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		if (table->keylen > keylen) { /* someone else's player variable table */
			key[keylen++] = table->key[3];
			key[keylen++] = table->key[4];
		}
		playervars = dptab_getTable(dp->dt, key, keylen);
		if (table != playervars) {
			DPRINT(("dp_playervars_cb: cast-off session, not doing callbacks.\n"));
			dp_assertValid(dp);
			return 0;
		}
	}

	if (table->keylen == 3) { /* one of our own player's variable */
		id = dp->firstId + (unsigned short) dpMAKESHORT(subkey[0], 0);
	} else {
		dpid_t firstid = (unsigned short) dpMAKESHORT(table->key[3], table->key[4]);
		id = firstid + (unsigned short) dpMAKESHORT(subkey[0], 0);
	}
	DPRINT(("dp_playervars_cb: subkey %s maps to id %d\n",
			key2a(subkey, 2), id));

	{	struct {
			dp_packetType_t tag;
			dp_user_playerData_packet_t body;
		} PACK pkt;
		pkt.tag = dp_USER_PLAYERDATA_PACKET_ID;
		pkt.body.len = sent;
		pkt.body.id = id;
		pkt.body.key = (unsigned short) dpMAKESHORT(subkey[1], subkey[2]);
		/* the following is currently invalid since buf is a local variable */
		pkt.body.data = buf;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_playervars_cb: can't send local msg, err:%d\n", err));
		}
	}

	(void) dptab;
	(void) total;
	(void) seconds_left;

	dp_assertValid(dp);
	return 0;
}

static short dp_getDesiredHandleOptions(dp_t *dp, playerHdl_t h)
{
	short opt;

	/* Check pings, if dpSetPingIntervals() was called.
	 * Not sure this is quite right for a server, but it's not unreasonable.
	 */
	opt = dp->peerHandleMode;

	/* Game servers/masters, or handles to them, always have keepalives. */
	if ((h == dp->hMaster)
	||  (PLAYER_ME == dp->hMaster)
	||  (h == dp->hGameServer)
	||  bIAmGameServer(dp))
		opt |= dpio_OPTION_KEEPALIVE;
	return opt;
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of host entries in a session hosts table.

 Only the default session needs a callback, because the callback is only
 used to handle success or failure in opening a session (or keeping it open).
 Only one session at a time can be open.
 This callback refers to the default session's variables in dp for convenience.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_hosts_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	dp_t *dp;
	dp_sessionContext_t *pSess = (dp_sessionContext_t *)context;
	dp_host_t host;
	dpid_t id;
	dp_result_t err;
	playerHdl_t h = dp_ID_NONE;
	int itsMe;

	dptab_assertValid(dptab);
	dptab_assertValidTable(table);

	if (!context || !buf)
		return 0;
	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED))
		return 0;

	dp = pSess->dp;

	dp_assertValid(dp);

	id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
	dp_unpack_host(id, dp->dpio->myAdrLen, buf, &host, total);

	/* To tell whether a host record refers to us, it is sufficient to check
	 * the global address.
	 */
	itsMe = !memcmp(dp->dpio->myAdr, host.iadr, dp->dpio->myAdrLen);

	if ((status == dp_RES_CREATED) || (status == dp_RES_CHANGED)) {
#if 1
		/* Open a comm handle to this fellow, and
		 * remember the comm handle for this range of id's.
		 *
		 * Supports NAT and machines with multiple ip interfaces
		 * as long as the game server only sees one address.
		 * If the host has two DIFFERENT addresses then pass them
		 * both to dpio, as long as the host's internal address
		 * can't be confused with ours.
		 */
		DPRINT(("dp_hosts_cb: id:%d itsMe:%d; Opening ", id, itsMe));
		printAdr(dp->dpio->myAdrLen, host.iadr);

		if (!memcmp(host.iadr, host.iadr2, dp->dpio->myAdrLen)
		||  (!memcmp(dp->dpio->myAdr2, host.iadr2, dp->dpio->myAdrLen)
			 && !itsMe)) {
			DPRINT(("\n"));
			h = dpio_openHdl2(dp->dpio, host.iadr, NULL);
		} else {
			DPRINT((", "));
			printAdr(dp->dpio->myAdrLen, host.iadr2);
			DPRINT(("\n"));
			h = dpio_openHdl2(dp->dpio, host.iadr, host.iadr2);
		}
		commHdl4dpid(dp, host.firstId, h);
		err = dpio_setHandleOptions(dp->dpio, h, dp_getDesiredHandleOptions(dp, h));
		if (err != dp_RES_OK) {
			DPRINT(("dp_hosts_cb: dpio_setHandleOptions(h:%x) err:%d\n", h));
		}
#endif
#if 1
		/* Nonhosts need to track what session player is in, too.
		 * (Hosts do it at join time.)
		 */
		if (dp->hMaster != PLAYER_ME) {
			dp_sessionContext_t **spp;
			/* Remember what session the fellow is in.
			 * We happen to know that player table context is a dp_sessionContext_t *
			 */
			spp = (dp_sessionContext_t **) assoctab_subscript_grow(dp->hdl2sess, h);
			if (!spp) {
				DPRINT(("dp_hosts_cb: can't save handle's session\n"));
				dp_assertValid(dp);
				return dp_RES_BUG;
			}
			*spp = pSess;
		}
#endif
		;
	} else if (status == dp_RES_DELETED) {
		/* Close and delete the comm handle for this range of id's. */
		h = dpid2commHdl(dp, host.firstId);
		if ((h != PLAYER_ME) && (h != PLAYER_NONE)) {
			DPRINT(("dp_hosts_cb: host firstId %d deleted, closing h:%x\n", host.firstId, h));
			commHdl4dpid(dp, host.firstId, PLAYER_NONE);
			dpio_closeHdl(dp->dpio, h);
		}
	}

	/* If a host was added, and it was us, call dpOpen's callback. */
	if (itsMe) {
		if (host.joinKarma != dp->joinKarma) {
			DPRINT(("dp_hosts_cb: host.joinKarma:%d != dp->joinKarma:%d, ignoring\n", host.joinKarma, dp->joinKarma));
			return 0;
		}
		if (status == dp_RES_CREATED) {
			dp->firstId = host.firstId;
			dp->nextId = host.firstId;
			if (dp->dpOpen_cb) {
				err = dpOpenLocalResult(dp, dp_RES_OK, host.firstId);
				if (err != dp_RES_OK)
					return 0;
			}
		} else if (status == dp_RES_DELETED) {
			/* Ulp - our host record was deleted.
			 * This can only happen if the host kicked us out?
			 * We should send the local user a message... FIXME
			 */
			DPRINT(("dp_hosts_cb: hey: we got kicked out of the session??\n"));
			/*dpClose(dp);*/
			dp->pleaseClose = TRUE;	/* close on next dpPoll */
		}
	} else {
		if ((status == dp_RES_CREATED) || (status == dp_RES_CHANGED)) {
			/* someone else added */

			dptab_table_t *playervars;
			char key[dptab_KEY_MAXLEN];
			int keylen;
			dp_session_t sess;
			err = dpGetSessionDescById(dp, pSess->sess_subkey, &sess, 0);
			if (err != dp_RES_OK) {
				DPRINT(("dp_hosts_cb: bug: can't get session record\n"));
				dp->pleaseClose = TRUE;	/* FIXME - not right for server */
				return dp_RES_BUG;
			}
			if (sess.flags & dp_SESSION_FLAGS_ENABLE_PLAYERVARS) {
				err = dptab_addPeer(dp->dt, h);
				if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
					DPRINT(("dp_hosts_cb: can't add h:%x to peer table, err:%d\n", h, err));
					dp->pleaseClose = TRUE;	/* FIXME - not right for server */
					dp_assertValid(dp);
					return err;
				}
#if 0
				/* Give the host a subscription to our player variable table. */
				if (pSess->playervars) {
					err = dptab_addSubscriber(dp->dt, pSess->playervars, h);
					if (err != dp_RES_OK) {
						DPRINT(("dp_hosts_cb: can't add subscriber h:%x, err:%d\n", h, err));
						dp->pleaseClose = TRUE;	/* FIXME - not right for server */
						dp_assertValid(dp);
						return dp_RES_BUG;
					}
				} else {
					DPRINT(("dp_hosts_cb: own playervars is null\n"));
					dp->pleaseClose = TRUE;	/* FIXME - not right for server */
					dp_assertValid(dp);
					return dp_RES_BUG;
				}
#endif
				/* Create and request subscription from the host's player
				 * variable table */
				keylen = 0;
				key[keylen++] = dp_KEY_PLAYER_VARIABLES;
				key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
				key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
				key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(host.firstId);
				key[keylen++] = (char) dpGETSHORT_SECONDBYTE(host.firstId);
				err = dptab_createTable(dp->dt, &playervars, key, keylen, 0, NULL, NULL, dp_playervars_cb, pSess);
				if (err != dp_RES_OK) {
					DPRINT(("dp_hosts_cb: err %d creating player variable table for h:%x\n", err, h));
					dp->pleaseClose = TRUE;	/* FIXME - not right for server */
					dp_assertValid(dp);
					return dp_RES_BUG;
				}
				keylen = 3;
				err = dptab_addPublisher(dp->dt, playervars, key, keylen, h);
				if (err != dp_RES_OK) {
					DPRINT(("dp_hosts_cb: can't add publisher h:%x, err:%d\n", h, err));
					dp->pleaseClose = TRUE;	/* FIXME - not right for server */
					dp_assertValid(dp);
					return dp_RES_BUG;
				}
				err = dptab_requestSubscription(dp->dt, key, keylen, h, NULL, NULL);
				if (err != dp_RES_OK) {
					DPRINT(("dp_hosts_cb: can't request subscription from h:%x, err:%d\n", h, err));
					dp->pleaseClose = TRUE;	/* FIXME - not right for server */
					dp_assertValid(dp);
					return dp_RES_BUG;
				}
			}
		} else if (dp_RES_DELETED == status) {	/* someone else deleted */
			/* Delete the host's subscription to our player variable table */
			char key[dptab_KEY_MAXLEN];
			int keylen;
			if (pSess->playervars)
				err = dptab_deleteSubscriber(dp->dt, pSess->playervars, h);
			/* Delete publisher and host's player variable table */
			keylen = 5;
			key[0] = dp_KEY_PLAYER_VARIABLES;
			key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
			key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
			key[3] = (char) dpGETSHORT_FIRSTBYTE(host.firstId);
			key[4] = (char) dpGETSHORT_SECONDBYTE(host.firstId);
			if (dptab_getTable(dp->dt, key, keylen)) {
				keylen = 3;
				dptab_deletePublisher(dp->dt, key, keylen, h);
				keylen = 5;
				dptab_deleteTable(dp->dt, key, keylen);
			}
		}
	}

	/* If it's a new host, check to see if there are already player
	 * records (due to dptab_update() delivering things out of order);
	 * if so, do the notification for each of those players.
	 */
	for (id=host.firstId; id < host.firstId + dp_PLAYERS_PER_HOST; id++) {
		char *pbuf;
		dp_playerId_t player;
		size_t len, len_used;
		char pkey[dptab_KEY_MAXLEN];

		pkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
		pkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
		err = dptab_get_bykey(pSess->players,pkey,2,(void **)&pbuf,&len);
		if (err != dp_RES_OK)
			continue;
		len_used = dp_unpack_playerId(id, pbuf, &player);
		if (len_used != len) {
			DPRINT(("dp_hosts_cb: can't unpack player %d.\n", id));
			dp_assertValid(dp);
			return 0;
		}
		/* If the host is being deleted, let the player table callback notify
		 * the user; otherwise, we have to notify the user here.
		 */
		if (status == dp_RES_DELETED) {
			err = dptab_delete(dp->dt, pSess->players, pkey, 2);
			if (err != dp_RES_OK) {
				DPRINT(("dp_hosts_cb: err:%d\n", err));
			}
		} else {
			dp_player_notify(dp, &player, &host, status);
		}
	}

	(void) seconds_left;
	(void) total;
	(void) sent;
	(void) subkeylen;
	(void) dest;
	(void) src;
	(void) table;
	(void) dptab;

	dp_assertValid(dp);
	return 0;
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of player entries in a session's myplayer table.

 Can handle multiple sessions.  Context pointer for this callback is a pointer
 to a structure holding pointers to this session's tables.

 Only does anything on the host.
 On the host, it copies records into the matching players table.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_myplayers_cb(
	dptab_t *dptab,
	dptab_table_t *table,
	playerHdl_t src,
	playerHdl_t dest,
	char *subkey,
	int subkeylen,
	void *buf,
	size_t sent,
	size_t total,
	int seconds_left,
	void *context,
	dp_result_t status)
{
	dp_sessionContext_t *pSess = (dp_sessionContext_t *)context;
	dp_t *dp;
	dp_result_t err;

	dptab_assertValid(dptab);
	DPRINT(("dp_myplayers_cb: status:%d, src h:%x, dest h:%x, table %s, subkey %s\n",
			status, src, dest,
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));

	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED)) {
		return 0;
	}

	if (!context || !buf) {
		DPRINT(("dp_myplayers_cb: bug: NULL context=%p, buf=%p!\n",context,buf));
		return 0;
	}
	dp = pSess->dp;
	dp_assertValid(dp);

	/* If we're the master, add it in to the players table, too.
	 * If we're master, the myplayers table should have no subscribers.
	 */
	if (pSess->myplayers->subscribers->n_used)
		return 0;

	if ((status == dp_RES_CREATED) || (status == dp_RES_CHANGED)) {
		dp_playerId_t player;
		dpid_t id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		size_t len_used = dp_unpack_playerId(id, buf, &player);
		dp_uid_t uid = dp_UID_NONE;
		int playerlen;
		char playerbuf[dpio_MAXLEN_RELIABLE];

		if (len_used != total) {
			DPRINT(("dp_myplayers_cb: len_used %d != total %d!\n", len_used, total));
		}
		if (dp->tserv)
			uid = tserv_hdl2uid(dp->tserv, src);
		player.karma = (dp_karma_t)(uid & 0xffff);	/* kludge... */
		DPRINT(("dp_myplayers_cb: id %d, karma %d (uid %d), bloblen %d, name %s\n",
				player.id,
				player.karma, uid,
				player.bloblen,
				player.name));
		playerlen = dp_pack_playerId(&player, playerbuf);

		DPRINT(("dp_myplayers_cb: setting player info in pSess->players\n"));
		err = dptab_set(dp->dt, pSess->players, subkey, subkeylen, playerbuf, total, 2, src);
		if (err != dp_RES_OK) {
			DPRINT(("dp_myplayers_cb: can't set player record, err:%d\n", err));
			dp_assertValid(dp);
			return err;
		}
	} else if (status == dp_RES_DELETED) {
		err = dptab_delete(dp->dt, pSess->players, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dp_myplayers_cb: dptab_delete returns err:%d\n", err));
		}
	}
	return 0;
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of player entries in a session's player table.

 Can handle multiple sessions.  Context pointer for this callback is a pointer
 to a structure holding pointers to this session's tables.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_players_cb(
	dptab_t *dptab,
	dptab_table_t *table,
	playerHdl_t src,
	playerHdl_t dest,
	char *subkey,
	int subkeylen,
	void *buf,
	size_t sent,
	size_t total,
	int seconds_left,
	void *context,
	dp_result_t status)
{
	dp_sessionContext_t *pSess = (dp_sessionContext_t *)context;
	dp_t *dp;
	dpid_t id, firstId;
	dp_playerId_t player;
	dp_result_t err;
	size_t len;
	char pkey[dptab_KEY_MAXLEN];
	dp_host_t host;
	char *hostbuf;
	size_t hostlen;

	dptab_assertValid(dptab);
	DPRINT(("dp_players_cb: status:%d, src h:%x, dest h:%x, table %s, subkey %s\n",
			status, src, dest,
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));

	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED)) {
		return 0;
	}

	if (!context || !buf) {
		DPRINT(("dp_players_cb: bug: NULL context=%p, buf=%p!\n",context,buf));
		return 0;
	}
	dp = pSess->dp;
	dp_assertValid(dp);

#ifndef DEFER_SESSION_UPDATE
	{
	size_t sesslen;
	dp_session_t sess;
	char *sessbuf;

	/* If we're the master, inform the world of the change in the
	 * number of players.  We can only tell if we're the master of
	 * a particular (possibly cast-off) session by seeing if it's
	 * in our mysessions table.  Can't check hMaster...
	 */
	err = dptab_get_bykey(dp->mysessions, pSess->sess_subkey, pSess->sess_subkeylen, (void **)&sessbuf, &sesslen);
	if (err == dp_RES_OK) {
		char rawbuf[sizeof(dp_session_t)];
		size_t rawbuflen;
		/* Must unpack when retrieving records from dptab */
		dp_unpack_session(dp, pSess->sess_subkey, pSess->sess_subkeylen, sessbuf, sesslen, &sess);
		sess.currentPlayers = (short) dptab_tableSize(table);
		/* Callback happens before record actually deleted */
		if (status == dp_RES_DELETED)
			sess.currentPlayers--;
		/* Must pack when writing to dptab */
		rawbuflen = dp_pack_session(dp, 0, &sess, rawbuf);
		err = dptab_set(dp->dt, dp->mysessions, pSess->sess_subkey, pSess->sess_subkeylen, rawbuf, rawbuflen, 4, PLAYER_ME);
		DPRINT(("dp_players_cb: currentPlayers now %d; dptab_set returns err:%d\n", sess.currentPlayers, err));

	}
	}
#endif

	id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
	firstId = (dpid_t) (id & ~(dp_PLAYERS_PER_HOST-1));	/* Skip back to firstId */

	/* Even if not master, delete player from group player, group myplayer,
	 * and player variable tables. */
	if (status == dp_RES_DELETED) {
		int i;
		char key[dptab_KEY_MAXLEN];
		int keylen = 5;

		if (!bIAmGameServer(dp)) {
			/* Tell the server about this guy's score, since he's leaving
			 * the game.
			 */
			err = dpscore_client_playerLeaving(dp, id);
			if (err != dp_RES_OK) {
				DPRINT(("dp_players_cb: dpscore_client_playerLeaving returns err:%d\n", err));
				/* ignore error? */
			}
		}

		key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		for (i = dp->firstGId; i < dp->firstGId + dp_MAX_GROUPS; i++) {
			dptab_table_t *grplayers;
			key[0] = (char) dp_KEY_GROUP_PLAYERS;
			key[3] = (char) dpGETSHORT_FIRSTBYTE(i);
			key[4] = (char) dpGETSHORT_SECONDBYTE(i);
			if ((grplayers = dptab_getTable(dp->dt, key, keylen)) != NULL)
				dptab_delete(dp->dt, grplayers, subkey, subkeylen);
			key[0] = (char) dp_KEY_GROUP_MYPLAYERS;
			if ((grplayers = dptab_getTable(dp->dt, key, keylen)) != NULL)
				dptab_delete(dp->dt, grplayers, subkey, subkeylen);
		}
		key[0] = dp_KEY_PLAYER_VARIABLES;
		if (firstId == dp->firstId) {	/* our own player */
			keylen = 3;
		} else {
			key[3] = dpGETSHORT_FIRSTBYTE(firstId);
			key[4] = dpGETSHORT_SECONDBYTE(firstId);
		}
		{	dptab_table_t *playervars;
			if ((playervars = dptab_getTable(dp->dt, key, keylen)) != NULL) {
				for (i=dptab_tableSize(playervars) - 1; i >= 0; i--) {
					char pvsubkey[dptab_KEY_MAXLEN];
					int pvsubkeylen;
					char* buf;
					size_t len;
					err = dptab_get_byindex(playervars, i, (void **)&buf, &len, pvsubkey, &pvsubkeylen);
					condition(err == dp_RES_OK, "playervars corrupted");
					if (id == (firstId + dpMAKESHORT(pvsubkey[0], 0)))
						dptab_delete(dp->dt, playervars, pvsubkey, pvsubkeylen);
				}
			}
		}
	}

	/**** Notify the user code of the new player ****/

	/* Ignore sessions that are cast off.  They don't care about new players.
	 * (They don't have host table callbacks anymore, so we don't always
	 * call dp_player_notify in that case anyway.)
	 */
	if (table != dp->players) {
		DPRINT(("dp_players_cb: cast-off session, not doing callbacks.\n"));
		dp_assertValid(dp);
		return 0;
	}

	DPRINT(("dp_players_cb: subkey %s maps to id %d\n",
			key2a(subkey, 2), id));

	len = dp_unpack_playerId(id, buf, &player);
	if (len != total) {
		DPRINT(("dp_players_cb: can't unpack player %d; len %d != total %d\n",
				id, len, total));
		dp_assertValid(dp);
		return 0;
	}

	/* get the player's network address from the hosts table.
	 * If we can't, it might be because the player record was delivered
	 * before the hosts record; to handle this, the hosts callback will call
	 * dp_player_notify on any players already in the table.
	 */
	if (!dp->hosts) {
		DPRINT(("dp_players_cb: no hosts\n"));
		dp_assertValid(dp);
		return 0;
	}
	pkey[0] = (char) dpGETSHORT_FIRSTBYTE(firstId);
	pkey[1] = (char) dpGETSHORT_SECONDBYTE(firstId);
	err = dptab_get_bykey(dp->hosts, pkey, 2, (void **)&hostbuf, &hostlen);
	if (err != dp_RES_OK) {
		DPRINT(("dp_players_cb: can't get host record; deferring\n"));
		dp_assertValid(dp);
		return 0;
	}
	dp_unpack_host(firstId, dp->dpio->myAdrLen, hostbuf, &host, hostlen);
	dp_player_notify(dp, &player, &host, status);

	(void) seconds_left;
	(void) sent;
	(void) dptab;

	dp_assertValid(dp);
	return 0;
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of group entries in a session's group table.

 Can handle multiple sessions.  Context pointer for this callback is a pointer
 to a structure holding pointers to this session's tables.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_groups_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	dp_sessionContext_t *pSess = (dp_sessionContext_t *)context;
	dp_t *dp;
	dpid_t id;
	dp_result_t err;
	dp_groupId_t gbuf;
	playerHdl_t me = PLAYER_ME;
	int namelen = sent < dp_PNAMELEN - 1 ? sent : dp_PNAMELEN - 1;

	dptab_assertValid(dptab);

	DPRINT(("dp_groups_cb: status:%d, src h:%x, dest h:%x, table %s, subkey %s\n",
			status, src, dest,
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));

	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED)) {
		return 0;
	}

	if (!context || !buf) {
		DPRINT(("dp_groups_cb: bug: NULL context=%p, buf=%p!\n",context,buf));
		return 0;
	}
	dp = pSess->dp;
	dp_assertValid(dp);

	/**** Notify the user code of the new group ****/

	/* Ignore sessions that are cast off.  They don't care about new groups.
	 */
	if (table != dp->groups) {
		DPRINT(("dp_groups_cb: cast-off session, not doing callbacks.\n"));
		return 0;
	}

	id = (unsigned short) dpMAKESHORT(subkey[0], subkey[1]);
	DPRINT(("dp_groups_cb: subkey %s maps to id %d\n",
			key2a(subkey, 2), id));

	gbuf.id = id;
	gbuf.karma = id;		/* kludge */
	memcpy(gbuf.name, buf, sent);
	gbuf.name[namelen] = '\0';
	gbuf.sessionKarma = dp->sess_karma;
	if (status == dp_RES_CREATED) {
		struct {
			dp_packetType_t tag;
			dp_user_addGroup_packet_t body;
		} PACK pkt;
		pkt.tag = dp_USER_ADDGROUP_PACKET_ID;
		pkt.body = gbuf;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_group_cb: can't send local msg, err:%d\n", err));
		}
	} else if (status == dp_RES_DELETED) {
		struct {
			dp_packetType_t tag;
			dp_user_delGroup_packet_t body;
		} PACK pkt;
		pkt.tag = dp_USER_DELGROUP_PACKET_ID;
		pkt.body = gbuf;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_group_cb: can't send local msg, err:%d\n", err));
		}
	}
#if 0
	else if (status == dp_RES_CHANGED) {
		/* do something? */
	}
#endif

	(void) dptab;
	(void) total;
	(void) seconds_left;

	dp_assertValid(dp);
	return 0;
}

/*----------------------------------------------------------------------
 Callback for addition/deletion of player entries in a session's group players
 table.

 Can handle multiple sessions.  Context pointer for this callback is a pointer
 to a structure holding pointers to this session's tables.
----------------------------------------------------------------------*/
int dp_PASCAL
dp_group_players_cb(
	dptab_t *dptab, dptab_table_t *table, playerHdl_t src, playerHdl_t dest,
	char *subkey, int subkeylen, void *buf, size_t sent, size_t total,
	int seconds_left, void *context, dp_result_t status)
{
	dp_sessionContext_t *pSess = (dp_sessionContext_t *)context;
	dp_t *dp;
	dpid_t id;
	dp_result_t err;
	dp_addPlayerToGroup_packet_t gbuf;
	playerHdl_t me = PLAYER_ME;

	dptab_assertValid(dptab);

	DPRINT(("dp_group_players_cb: status:%d, src h:%x, dest h:%x, table %s, subkey %s\n",
			status, src, dest,
			key2a(table->key, table->keylen),
			key2a2(subkey, subkeylen)));

	/* Don't care about send events */
	if ((status != dp_RES_CREATED) && (status != dp_RES_CHANGED) && (status != dp_RES_DELETED)) {
		return 0;
	}

	if (!context || !buf) {
		DPRINT(("dp_group_players_cb: bug: NULL context=%p, buf=%p!\n",context,buf));
		return 0;
	}
	dp = pSess->dp;
	dp_assertValid(dp);

	/**** Notify the user code of the new group ****/

	/* Ignore sessions that are cast off.  They don't care about new groups.
	 */
	{	dptab_table_t *grplayers;
		char key[dptab_KEY_MAXLEN];
		int keylen = 0;
		key[keylen++] = dp_KEY_GROUP_PLAYERS;
		key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		key[keylen++] = table->key[3];
		key[keylen++] = table->key[4];
		grplayers = dptab_getTable(dp->dt, key, keylen);
		if (table != grplayers) {
			DPRINT(("dp_group_players_cb: cast-off session, not doing callbacks.\n"));
			dp_assertValid(dp);
			return 0;
		}
	}

	id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
	DPRINT(("dp_group_players_cb: subkey %s maps to id %d\n",
			key2a(subkey, 2), id));

	gbuf.dpIdGroup = (unsigned short) dpMAKESHORT(table->key[3], table->key[4]);
	gbuf.dpIdPlayer = id;
	gbuf.sessionKarma = dp->sess_karma;
	if (status == dp_RES_CREATED) {
		struct {
			dp_packetType_t tag;
			dp_addPlayerToGroup_packet_t body;
		} PACK pkt;
		pkt.tag = dp_USER_ADDPLAYERTOGROUP_PACKET_ID;
		pkt.body = gbuf;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_group_cb: can't send local msg, err:%d\n", err));
		}
	} else if (status == dp_RES_DELETED) {
		struct {
			dp_packetType_t tag;
			dp_delPlayerFromGroup_packet_t body;
		} PACK pkt;
		pkt.tag = dp_USER_DELPLAYERFROMGROUP_PACKET_ID;
		pkt.body = gbuf;
		err = dpio_put_reliable(dp->dpio, &me, 1, &pkt, sizeof(pkt.tag)+sizeof(pkt.body), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dp_group_cb: can't send local msg, err:%d\n", err));
		}
	}
#if 0
	else if (status == dp_RES_CHANGED) {
		/* do something? */
	}
#endif

	(void) dptab;
	(void) sent;
	(void) total;
	(void) seconds_left;

	dp_assertValid(dp);
	return 0;
}

/* Used to mark freeze files (for Garfield fans) */
#define dp_SIGNATURE	(0x0D1E)

/*----------------------------------------------------------------------
Freeze the contents of a dp_sessionContext_t.
----------------------------------------------------------------------*/
dp_result_t
dp_sessionContext_freeze(
	dp_t *dp,
	dp_sessionContext_t* pSess,
	FILE* fp)
{
	dp_result_t err;
	int i;
	char state = 0;

	precondition(pSess != NULL);
	precondition(fp != NULL);

	assert(pSess->dp == dp);
	dp_assertValid(pSess->dp);

	/* Handle null pointers */
	if(pSess->hosts != NULL)
		state |= 0x01;
	if(pSess->players != NULL)
		state |= 0x02;
	if(pSess->groups != NULL)
		state |= 0x04;
	if(pSess->playervars != NULL)
		state |= 0x16;
	/* Remember if this is the default session */
	if (pSess == dp->pSess)
		state |= 0x08;

	/* Save simple stuff */
	if ((fwrite(&state, sizeof(char), 1, fp) != 1) ||
		(fwrite(&(pSess->sessionType), sizeof(dp_species_t), 1, fp) != 1) ||
		(fwrite(&(pSess->sess_subkeylen), sizeof(int), 1, fp) != 1) ||
		(fwrite(pSess->sess_subkey, sizeof(char), pSess->sess_subkeylen, fp) !=
										(size_t)(pSess->sess_subkeylen)))
		return dp_RES_FULL;

	/* Save possibly null pointers */
	if(state & 0x01) {
		err = dptab_tableRef_freeze(pSess->hosts, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_freeze: freezing hosts, err:%d\n", err));
			return err;
		}
	}
	if(state & 0x02) {
		err = dptab_tableRef_freeze(pSess->players, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_freeze: freezing players, err:%d\n", err));
			return err;
		}
	}
	if(state & 0x04) {
		err = dptab_tableRef_freeze(pSess->groups, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_freeze: freezing groups, err:%d\n", err));
			return err;
		}
	}
	if(state & 0x16) {
		err = dptab_tableRef_freeze(pSess->playervars, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_freeze: freezing player variables, err:%d\n", err));
			return err;
		}
	}
	assert(pSess->myplayers);
	err = dptab_tableRef_freeze(pSess->myplayers, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dp_sessionContext_freeze: freezing myplayers, err:%d\n", err));
		return err;
	}
	for (i=0; i<dp_MAX_GROUPS; i++) {
		assert(pSess->grplayers[i]);
		err = dptab_tableRef_freeze(pSess->grplayers[i], fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_freeze: freezing grplayers[%d], err:%d\n", i, err));
			return err;
		}
	}
	for (i=0; i<dp_MAX_GROUPS; i++) {
		assert(pSess->grmyplayers[i]);
		err = dptab_tableRef_freeze(pSess->grmyplayers[i], fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_freeze: freezing grmyplayers[%d], err:%d\n", i, err));
			return err;
		}
	}

	DPRINT(("dp_sessionContext_freeze: context frozen, state:%x\n", state));
	dp_assertValid(pSess->dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
Thaw the contents of a dp_sessionContext_t.
Assumes that dp contains a dptab that has already been thawed.
----------------------------------------------------------------------*/
dp_result_t
dp_sessionContext_thaw(
	dp_t* dp,
	dp_sessionContext_t* pSess,
	FILE* fp)
{
	dp_result_t err;
	char state = 0;
	int i;

	precondition(pSess != NULL);
	precondition(dp != NULL);
	precondition(fp != NULL);
	memset(pSess, 0, sizeof(*pSess));

	/* dp_assertValid(dp); */
	pSess->dp = dp;
	if ((fread(&state, sizeof(char), 1, fp) != 1) ||
		(fread(&(pSess->sessionType), sizeof(dp_species_t), 1, fp) != 1) ||
		(pSess->sess_subkeylen != dp->dpio->myAdrLen+2) ||
		(fread(&(pSess->sess_subkeylen), sizeof(int), 1, fp) != 1) ||
		(fread(pSess->sess_subkey, sizeof(char), pSess->sess_subkeylen, fp) !=
										(size_t)(pSess->sess_subkeylen))) {
		DPRINT(("dp_sessionContext_thaw: error reading key\n"));
		return dp_RES_EMPTY;
	}
	if(state & 0x01) {
		err = dptab_tableRef_thaw(&(pSess->hosts), dp->dt, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_thaw: thawing hosts, err:%d\n", err));
			return err;
		}
	} else
		pSess->hosts = NULL;
	if(state & 0x02) {
		err = dptab_tableRef_thaw(&(pSess->players), dp->dt, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_thaw: thawing players, err:%d\n", err));
			return err;
		}
	} else
		pSess->players = NULL;
	if(state & 0x04) {
		err = dptab_tableRef_thaw(&(pSess->groups), dp->dt, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_thaw: thawing groups, err:%d\n", err));
			return err;
		}
	} else
		pSess->groups = NULL;
	if(state & 0x16) {
		err = dptab_tableRef_thaw(&(pSess->playervars), dp->dt, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_thaw: thawing player variables, err:%d\n", err));
			return err;
		}
	} else
		pSess->playervars = NULL;

	err = dptab_tableRef_thaw(&(pSess->myplayers), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dp_sessionContext_thaw: thawing myplayers, err:%d\n", err));
		return err;
	}
	for (i=0; i<dp_MAX_GROUPS; i++) {
		err = dptab_tableRef_thaw(&(pSess->grplayers[i]), dp->dt, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_thaw: thawing grplayers[%d], err:%d\n", i, err));
			return err;
		}
	}
	for (i=0; i<dp_MAX_GROUPS; i++) {
		err = dptab_tableRef_thaw(&(pSess->grmyplayers[i]), dp->dt, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_sessionContext_thaw: thawing grmyplayers[%d], err:%d\n", i, err));
			return err;
		}
	}
	if (state & 0x08)
		dp->pSess = pSess;

	DPRINT(("dp_sessionContext_thaw: context thawed, state:%x\n", state));
	/* dp_assertValid(pSess->dp); */
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
Save the current running game to a file (for use during a future dpCreate).

Note: dpio_freeze is called first, so programs that don't need the
overhead of dp can just use dpio_create rather than dpCreate to restore.

Returns dp_RES_OK if successful, and various other values if not.  A value
of dp_RES_FULL indicates a failure to write to file (perhaps the disk
is full?); a value of dp_RES_BUSY indicates that there are incomplete
reliable transmissions (try again later); a value of dp_RES_ALREADY
indicates that the file could not be opened for writing.
----------------------------------------------------------------------*/
DP_API dp_result_t	/* status */
dpFreeze(
	dp_t* dp,		/* (input) dp_t to freeze */
	char* fname)	/* (input) Name of file to freeze to */
{
	int sig = dp_SIGNATURE;
	FILE* fp;
	int i;
	dp_result_t err;

	precondition(dp != NULL);
	precondition(fname != NULL);
	dp_assertValid(dp);

#if 0
	/* The caller should be responsible for waiting for any important
	 * communications to finish (using dpReadyToFreeze), if this is desired.
	 * dpFreeze will happily freeze and thaw all in transit communications,
	 * which is handy for doing a quick server restart to change log files.
	 */
	if(dpReadyToFreeze(dp) != dp_RES_OK) {
		DPRINT(("dpFreeze: busy, not freezing.\n"));
		dp_assertValid(dp);
		return dp_RES_BUSY;
	}
#endif

	if((fp = fopen(fname, "wb")) == NULL) {
		DPRINT(("dpFreeze: Couldn't open file %s for writing.\n", fname));
		dp_assertValid(dp);
		return dp_RES_ALREADY;
	}

	/* Transport layer */
	/* Must call before signature, so that both dpCreate() and dpio_create()
	   can restore from this file. */
	err = dpio_freeze(dp->dpio, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: dpio_freeze failed, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}

	/* Signature */
	if (fwrite(&sig, sizeof(sig), 1, fp) != 1) {
		dp_assertValid(dp);
		fclose(fp);
		return dp_RES_FULL;
	}

	/* Master Table */
	err = dptab_freeze(dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: dptab_freeze failed, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}

	/* Details */
	/* ignore next_beacon; it's time-based and won't be valid when restored. */
	if ((fwrite(dp->my_outside_adr, sizeof(char), dp_MAX_ADR_LEN, fp) !=
													dp_MAX_ADR_LEN) ||
		(fwrite(dp->adrGameServer, sizeof(char), dp_MAX_ADR_LEN, fp) !=
													dp_MAX_ADR_LEN) ||
		(fwrite(&(dp->beacon_interval), sizeof(clock_t), 1, fp) != 1) ||
		(fwrite(dp->sess_subkey, sizeof(char), dptab_KEY_MAXLEN, fp) !=
													dptab_KEY_MAXLEN) ||
		(fwrite(&(dp->sess_subkeylen), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(dp->sess_karma), sizeof(dp_karma_t), 1, fp) != 1) ||
		(fwrite(&(dp->joinKarma), sizeof(dp_karma_t), 1, fp) != 1) ||
#ifdef USE_dp_enableNewPlayers
		(fwrite(&(dp->enableNewPlayers), sizeof(int), 1, fp) != 1) ||
#endif
		(fwrite(&(dp->firstId), sizeof(dpid_t), 1, fp) != 1) ||
		(fwrite(&(dp->nextId), sizeof(dpid_t), 1, fp) != 1) ||
		(fwrite(&(dp->firstGId), sizeof(dpid_t), 1, fp) != 1) ||
		(fwrite(&(dp->nextGId), sizeof(dpid_t), 1, fp) != 1) ||
		(fwrite(&(dp->defaultSessionType), sizeof(dp_species_t), 1, fp) != 1) ||
		(fwrite(&(dp->select_keylen), sizeof(int), 1, fp) != 1) ||
		(fwrite(&(dp->select_key), sizeof(char), dptab_KEY_MAXLEN, fp) !=
		 											dptab_KEY_MAXLEN) ||
		(fwrite(&(dp->pleaseClose), sizeof(int), 1, fp) != 1)) {
		DPRINT(("dpFreeze: error writing details\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return dp_RES_FULL;
	}
	err = dpio_freezeHdl(dp->dpio, dp->hGameServer, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing hGameServer, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	err = dpio_freezeHdl(dp->dpio, dp->hMaster, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing hMaster, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	DPRINT(("dpFreeze: hMaster h:%x\n", dp->hMaster));

	/* Table References */
	err = dptab_tableRef_freeze(dp->sessions, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing sessions, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	err = dptab_tableRef_freeze(dp->mysessions, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing mysessions, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	err = dptab_tableRef_freeze(dp->players, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing players, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	err = dptab_tableRef_freeze(dp->myplayers, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing myplayers, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	err = dptab_tableRef_freeze(dp->hosts, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing hosts, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
	err = dptab_tableRef_freeze(dp->groups, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing groups, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
#if defined(dp_ANET2)
	err = dptab_tableRef_freeze(dp->apps, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpFreeze: freezing apps, err:%d\n", err));
		dp_assertValid(dp);
		fclose(fp);
		return err;
	}
#endif

	/* DPIDs */
	if(dp->dpid2commHdl == NULL) {
		int nullTag = -1;
		DPRINT(("dpFreeze: freezing NULL dpid2commHdl\n"));
		if (fwrite(&nullTag, sizeof(int), 1, fp) != 1) {
			dp_assertValid(dp);
			fclose(fp);
			return dp_RES_FULL;
		}
	} else {
		DPRINT(("dpFreeze: freezing dpid2commHdl of %d items\n", dp->dpid2commHdl->n_used));
		condition(dp->dpid2commHdl->n_used >= 0, "bad null logic");
		if (fwrite(&(dp->dpid2commHdl->n_used), sizeof(int), 1, fp) != 1) {
			dp_assertValid(dp);
			fclose(fp);
			return dp_RES_FULL;
		}
		for (i = 0; i < dp->dpid2commHdl->n_used; i++) {
			assoctab_item_t* item = assoctab_getkey(dp->dpid2commHdl, i);
			condition(item != NULL, "not enough items in dpid2commHdl");
			if (fwrite(&(item->key), sizeof(int), 1, fp) != 1) {
				dp_assertValid(dp);
				fclose(fp);
				return dp_RES_FULL;
			}
			DPRINT(("dpFreeze: #%d dpid2commHdl[id:%d]=h:%x\n", i, item->key, *((playerHdl_t *)item->value)));
			err = dpio_freezeHdl(dp->dpio, *((playerHdl_t *)item->value), fp);
			if (err != dp_RES_OK) {
				DPRINT(("dpFreeze: freezing hGameServer, err:%d\n", err));
				dp_assertValid(dp);
				fclose(fp);
				return err;
			}
		}
	}

	/* Session contexts */
	if (fwrite(&(dp->sessionContexts->n_used), sizeof(int), 1, fp) != 1) {
		dp_assertValid(dp);
		fclose(fp);
		return dp_RES_FULL;
	}
	DPRINT(("dpFreeze: freezing sessionContexts of %d items\n", dp->sessionContexts->n_used));
	dumpSessionContexts(dp);
	for(i = 0; i < dp->sessionContexts->n_used; i++) {
		assoctab_item_t* item = NULL;
		/* Write out the session context */
		dp_sessionContext_t **sess = dynatab_subscript(dp->sessionContexts, i);
		condition(sess != NULL, "not enough items in sessionContexts");
		dp_sessionContext_freeze(dp, *sess, fp);
	}

	/* tserv login, database information */
	err = tserv_Freeze(dp->tserv, fp);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		fclose(fp);
		return dp_RES_FULL;
	}

	if(fclose(fp) != 0) {
		DPRINT(("dpFreeze: error closing.\n"));
		return dp_RES_BAD;
	}

	dumpSessionContexts(dp);

	/* Callbacks are set by the caller, or by dpThaw for dp's callbacks */
	/* Support for dpEnumSessions(), dpEnumServers(), and dpPingUser()
	   is automatically cleared by dpThaw() */

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Restore the current state from a stream.

 Most of the information is simply restored from the stream.
 Caller must have initialized dpio.
 All dpEnumPlayers, dpEnumSessions, and dpPingUser information is cleared.

 Returns dp_RES_OK if successful, and various other values if not.  A value
 of dp_RES_EMPTY indicates an attempt to read beyond end of file; a value
 of dp_RES_BAD indicates that the stream did not have the expected format
 (e.g., incorrect signature bytes, unreconcilable reference, etc.); a value
 of dp_RES_NOMEM indicates that the function could not allocate needed
 memory.
------------------------------------------------------------------------*/
static dp_result_t	/* status */
dpThaw(
	dp_t* dp,		/* (output) dp_t to thaw into */
	FILE* fp)		/* (modified) File to thaw from */
{
	int sig;
	int id2comm_used;
	int sessCont_used;
	int i;
	dp_result_t err;

	precondition(dp != NULL);
	precondition(fp != NULL);

	/* Signature */
	if (fread(&sig, sizeof(sig), 1, fp) != 1) {
		DPRINT(("dpThaw: can't read signature\n"));
		return dp_RES_EMPTY;	/* Invalid file */
	}
	if (sig != dp_SIGNATURE) {
		DPRINT(("dpThaw: bad sig\n"));
		return dp_RES_BAD;
	}

	/* Master Table */
	dp->dt = dptab_create(dp->dpio);
	if (!dp->dt) {
		DPRINT(("dpThaw: dptab_create failed\n"));
		return dp_RES_NOMEM;
	}
	err = dptab_thaw(dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: dptab_thaw failed, err:%d\n", err));
		return err;
	}

	/* Details */
	if ((fread(dp->my_outside_adr, sizeof(char), dp_MAX_ADR_LEN, fp) !=
													dp_MAX_ADR_LEN) ||
		(fread(dp->adrGameServer, sizeof(char), dp_MAX_ADR_LEN, fp) !=
													dp_MAX_ADR_LEN) ||
		(fread(&(dp->beacon_interval), sizeof(clock_t), 1, fp) != 1) ||
		(fread(dp->sess_subkey, sizeof(char), dptab_KEY_MAXLEN, fp) !=
													dptab_KEY_MAXLEN) ||
		(fread(&(dp->sess_subkeylen), sizeof(int), 1, fp) != 1) ||
		(fread(&(dp->sess_karma), sizeof(dp_karma_t), 1, fp) != 1) ||
		(fread(&(dp->joinKarma), sizeof(dp_karma_t), 1, fp) != 1) ||
#ifdef USE_dp_enableNewPlayers
		(fread(&(dp->enableNewPlayers), sizeof(int), 1, fp) != 1) ||
#endif
		(fread(&(dp->firstId), sizeof(dpid_t), 1, fp) != 1) ||
		(fread(&(dp->nextId), sizeof(dpid_t), 1, fp) != 1) ||
		(fread(&(dp->firstGId), sizeof(dpid_t), 1, fp) != 1) ||
		(fread(&(dp->nextGId), sizeof(dpid_t), 1, fp) != 1) ||
		(fread(&(dp->defaultSessionType), sizeof(dp_species_t), 1, fp) != 1) ||
		(fread(&(dp->select_keylen), sizeof(int), 1, fp) != 1) ||
		(fread(&(dp->select_key), sizeof(char), dptab_KEY_MAXLEN, fp) !=
													dptab_KEY_MAXLEN) ||
		(fread(&(dp->pleaseClose), sizeof(int), 1, fp) != 1)) {
		DPRINT(("dpThaw: error reading details\n", err));
		return dp_RES_EMPTY;
	}
	err = dpio_thawHdl(dp->dpio, &(dp->hGameServer), fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing hGameServer, err:%d\n", err));
		return err;
	}
	err = dpio_thawHdl(dp->dpio, &(dp->hMaster), fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing hMaster, err:%d\n", err));
		return err;
	}
	DPRINT(("dpThaw: hMaster h:%x\n", dp->hMaster));

	/* Table References */
	err = dptab_tableRef_thaw(&(dp->sessions), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing sessions, err:%d\n", err));
		return err;
	}
	err = dptab_tableRef_thaw(&(dp->mysessions), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing mysessions, err:%d\n", err));
		return err;
	}
	err = dptab_tableRef_thaw(&(dp->players), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing players, err:%d\n", err));
		return err;
	}
	err = dptab_tableRef_thaw(&(dp->myplayers), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing myplayers, err:%d\n", err));
		return err;
	}
	err = dptab_tableRef_thaw(&(dp->hosts), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing hosts, err:%d\n", err));
		return err;
	}
	err = dptab_tableRef_thaw(&(dp->groups), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing groups, err:%d\n", err));
		return err;
	}
#if defined(dp_ANET2)
	err = dptab_tableRef_thaw(&(dp->apps), dp->dt, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: thawing apps, err:%d\n", err));
		return err;
	}
#endif
	/* Restore the sessions table callback */
	err = dptab_setTableCallback(dp->sessions, dp_sessions_cb, dp);
	/* Restore the myplayers table callback */
	err = dptab_setTableCallback(dp->myplayers, dp_myplayers_cb, dp);

	/* DPIDs */
	if (fread(&(id2comm_used), sizeof(int), 1, fp) != 1) {
		return dp_RES_EMPTY;
	}
	DPRINT(("dpThaw: thawing dpid2commHdl of %d items\n", id2comm_used));
	if ((id2comm_used < 0)
	|| (id2comm_used > (dp_PLAYERS_PER_HOST * dp->dpio->max_playerHdls))) {
		DPRINT(("dpThaw: wrong length for dpid2commHdl:%d\n", id2comm_used));
		dp->dpid2commHdl = NULL;
	} else {
		dp->dpid2commHdl = assoctab_create(sizeof(playerHdl_t));
		if (dp->dpid2commHdl == NULL) {
			DPRINT(("dpThaw: couldn't create dpid2commHdl (%d)\n", id2comm_used));
			return err;
		}
		for (i = 0; i < id2comm_used; i++) {
			playerHdl_t* pH = NULL;
			int key;
			if (fread(&key, sizeof(int), 1, fp) != 1) {
				DPRINT(("dpThaw: unable to read dpid %d\n", i));
				return dp_RES_EMPTY;
			}
			pH = assoctab_subscript_grow(dp->dpid2commHdl, key);
			if(pH == NULL) {
				DPRINT(("dpThaw: error creating dpid %d\n", i));
				return dp_RES_NOMEM;
			}
			err = dpio_thawHdl(dp->dpio, pH, fp);
			if (err != dp_RES_OK) {
				DPRINT(("dpThaw: thawing dpid %d, err:%d\n", i, err));
				return err;
			}
			DPRINT(("dpThaw: #%d dpid2commHdl[id:%d]=h:%x\n", i, key, *pH));
			/*assert(dpid2commHdl(dp, (dpid_t) key) == *pH);*/
		}
	}

	/* Session contexts */
	dp->sessionContexts = dynatab_create(sizeof(dp_sessionContext_t *));
	dp->hdl2sess = assoctab_create(sizeof(dp_sessionContext_t *));
	if ((dp->sessionContexts == NULL) || (dp->hdl2sess == NULL)) {
		DPRINT(("dpThaw: error creating sessionContexts or hdl2sess\n"));
		return dp_RES_NOMEM;
	}
	if (fread(&(sessCont_used), sizeof(int), 1, fp) != 1) {
		DPRINT(("dpThaw: unable to read sessCont_used\n"));
		return dp_RES_EMPTY;
	}
	DPRINT(("dpThaw: thawing sessionContexts of %d items\n", sessCont_used));
	dumpSessionContexts(dp);
	for (i = 0; i < sessCont_used; i++) {
		int j;
		dp_sessionContext_t* pSess;
		dp_sessionContext_t** ppSess;
		/* Read in session context */
		pSess = dp_MALLOC(sizeof(dp_sessionContext_t));
		if(pSess == NULL) {
			DPRINT(("dpThaw: error creating sess %d\n", i));
			return dp_RES_NOMEM;
		}
		err = dp_sessionContext_thaw(dp, pSess, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dpThaw: thawing sess %d, err:%d\n", i, err));
			return err;
		}
		assert(pSess->players != NULL);
		err = dptab_setTableCallback(pSess->players, dp_players_cb, pSess);
		if (err != dp_RES_OK) {
			DPRINT(("dpThaw: setting players callback %d, err:%d\n", i, err));
			return err;
		}
		assert(pSess->myplayers != NULL);
		err = dptab_setTableCallback(pSess->myplayers, dp_myplayers_cb, pSess);
		if (err != dp_RES_OK) {
			DPRINT(("dpThaw: setting myplayers callback %d, err:%d\n", i, err));
			return err;
		}
		/* Add it to the sessionContexts set */
		ppSess = dynatab_subscript_grow(dp->sessionContexts, dynatab_size(dp->sessionContexts));
		if(ppSess == NULL) {
			DPRINT(("dpThaw: error creating sess entry %d\n", i));
			return dp_RES_NOMEM;
		}
		*ppSess = pSess;
		/* Restore hosts, groups, group players and player variable callbacks,
		 * but only if default session.  All others are as if dpCastoffSession
		 * were called.
		 */
		assert(pSess->groups != NULL);
		if (pSess->groups == dp->groups) {
			char key[dptab_KEY_MAXLEN];
			int keylen;

			assert(pSess->hosts != NULL);
			err = dptab_setTableCallback(pSess->hosts, dp_hosts_cb, pSess);
			if (err != dp_RES_OK) {
				DPRINT(("dpThaw: setting hosts callback	%d, err:%d\n", i, err));
				return err;
			}

			err = dptab_setTableCallback(pSess->groups, dp_groups_cb, pSess);
			if (err != dp_RES_OK) {
				DPRINT(("dpThaw: setting groups callback %d, err:%d\n", i, err));
				return err;
			}
			for (j = 0; j < dp_MAX_GROUPS; j++) {
				assert(pSess->grplayers[j] != NULL);
				err = dptab_setTableCallback(pSess->grplayers[j], dp_group_players_cb, pSess);
				if (err != dp_RES_OK) {
					DPRINT(("dpThaw: setting group players[%d] callback %d, err:%d\n", j, i, err));
					return err;
				}
			}
			if (pSess->playervars) {
				err = dptab_setTableCallback(pSess->playervars, dp_playervars_cb, pSess);
				if (err != dp_RES_OK) {
					DPRINT(("dpThaw: setting hosts callback	%d, err:%d\n", i, err));
					return err;
				}
				keylen = 5;
				key[0] = dp_KEY_PLAYER_VARIABLES;
				key[1] =  (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
				key[2] =  (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
				for (j=dptab_tableSize(pSess->hosts); j-- > 0; ) {
					char subkey[dptab_KEY_MAXLEN];
					int subkeylen;
					dptab_table_t *playervars;
					char* hostbuf;
					size_t len;
					err = dptab_get_byindex(pSess->hosts, j, (void **)&hostbuf, &len, subkey, &subkeylen);
					condition(err == dp_RES_OK, "pSess->hosts corrupted");
					if (((dpid_t)dpMAKESHORT(subkey[0], subkey[1])) == dp->firstId) continue; /* already did our own table */
					key[3] = subkey[0];
					key[4] = subkey[1];
					playervars = dptab_getTable(dp->dt, key, keylen);
					assert(playervars != NULL);
					err = dptab_setTableCallback(playervars, dp_playervars_cb, pSess);
					if (err != dp_RES_OK) {
						DPRINT(("dpThaw: setting player variables callback	%d, err:%d\n", i, err));
						return err;
					}
				}
			}
			/* While we're at it, restore the pointer to the default session.
			 * and session's description in case we are elected master. */
			dp->pSess = pSess;
			{	dp_session_t sess;
				dpGetSessionDesc(dp, &sess, 0);
				dp->election_old_session = sess;
			}
		}
		/* Store what session each of the hosts in this session is in */
		for (j=dptab_tableSize(pSess->hosts); j-- > 0; ) {
			playerHdl_t h;
			dp_host_t host;
			char* hostbuf;
			size_t len;
			char subkey[dptab_KEY_MAXLEN];
			int subkeylen;
			dp_sessionContext_t **spp;

			err = dptab_get_byindex(pSess->hosts, j, (void **)&hostbuf, &len, subkey, &subkeylen);
			condition(err == dp_RES_OK, "pSess->hosts corrupted");
			dp_unpack_host(dp_ID_NONE, dp->dpio->myAdrLen, hostbuf, &host, len);

			if (memcmp(host.iadr, host.iadr2, dp->dpio->myAdrLen)) {
				h = dpio_openHdl2(dp->dpio, host.iadr, host.iadr2);
			} else {
	  			h = dpio_openHdl2(dp->dpio, host.iadr, NULL);
			}

			if(h == PLAYER_NONE) {
				DPRINT(("dpThaw: ran out of handles; corrupted file?\n", i));
				return dp_RES_BAD;
			}
			/* Remember what session the fellow is in. */
			spp = (dp_sessionContext_t **) assoctab_subscript_grow(dp->hdl2sess, h);
			if (!spp) {
				DPRINT(("dpThaw: can't save handle's session\n"));
				return dp_RES_NOMEM;
			}
			*spp = pSess;
		}
	}

	/* load tserv login/database information */
	dp->tca = tca_create();
	if (!dp->tca) {
		DPRINT(("dpThaw: can't create tca.\n"));
		return dp_RES_NOMEM;
	}
	dp->tserv = tserv_create(dp->dpio, dp->tca);
	if (!dp->tserv) {
		DPRINT(("dpThaw: can't create tserv.\n"));
		return dp_RES_NOMEM;
	}
	err = tserv_Thaw(dp->tserv, fp);
	if (err != dp_RES_OK) {
		DPRINT(("dpThaw: tserv_Thaw returns err:%d\n", err));
		return err;
	}

	dumpSessionContexts(dp);
	assert((dp->players == NULL) ||
		(dptab_getTableContext(dp->players, dp_players_cb) != NULL));
	assert((dp->hosts == NULL) ||
		(dptab_getTableContext(dp->hosts, dp_hosts_cb) != NULL));
	assert((dp->groups == NULL) ||
		(dptab_getTableContext(dp->groups, dp_groups_cb) != NULL));

	/* Following stuff partly duplicated in dpCreate().  Yech.  FIXME */

	/* Handle stuff that wasn't in the file */
	dpio_setIncomingCallback(dp->dpio, dp_openHdl_cb, dp);
	dp->next_beacon = dp->now + dp->beacon_interval;
	dp->next_beacon2 = dp->now + 2*dp->beacon_interval;
	dp->next_beacon4 = dp->now + 4*dp->beacon_interval;
	/* rely on dpCreate's memset(dp, 0, sizeof(dp)) to clear all fields
	   associated with dpEnumServers, dpEnumSessions, and dpPingUser */

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Callback triggered by dpOpen when creating a session.
-------------------------------------------------------------------------*/
static int	dp_PASCAL open_sess_cb(dp_session_t *ps, long *pTimeout,long flags, void *context)
{
	dp_result_t *pOpenFinished = (int *)context;
	assert(pOpenFinished);

	if (ps) {
		DPRINT(("Opened session.\n"));
		*pOpenFinished = dp_RES_OK;
	} else {
		DPRINT(("Failed to Opened session.\n"));
		*pOpenFinished = dp_RES_BAD;
	}

	(void) context;
	return (ps != NULL);
}

typedef struct {
	const char * sessname;	/* set by caller to desired session name, if any */
	const char * sessId;	/* used if session ID is used instead of sessname */
	int sessIdLen;
	int finished;			/* set nonzero by callback when done */
	dp_session_t session;	/* set to found session by callback when done */
} dp_commThaw_sessfinder_t;

/*-------------------------------------------------------------------------
 Callback triggered by listing sessions.
-------------------------------------------------------------------------*/
static int dp_PASCAL listSessions_cb(dp_session_t *sDesc, long *pTimeout,long flags,void *context)
{
	dp_commThaw_sessfinder_t *pc = (dp_commThaw_sessfinder_t *)context;
	assert(pc);

	if (!sDesc) {
		pc->finished = TRUE;
		return FALSE;
	}
	DPRINT(("listSessions_cb: c%p, f%d\n", context, flags));
	/*dumpBuf((char *)sDesc, sizeof(dp_session_t)); */

	DPRINT(("(k %d; %d/%d plrs; spec %d; fl %x; U1 %x; pw '%s') name %s reserved2 %d.%d.%d.%d\n",
		sDesc->karma & 0xffff,
		sDesc->currentPlayers,
		sDesc->maxPlayers,
		sDesc->sessionType, sDesc->flags,
		sDesc->dwUser1, sDesc->szPassword,
		sDesc->sessionName,
		sDesc->reserved2[0] & 0xff,
		sDesc->reserved2[1] & 0xff,
		sDesc->reserved2[2] & 0xff,
		sDesc->reserved2[3] & 0xff
		));

	if (pc->sessId) {
		if (!memcmp(pc->sessId, sDesc->reserved2, pc->sessIdLen)) {
			pc->session = *sDesc;
			pc->finished = TRUE;
			return FALSE;
		}
	} else if (!pc->sessname || !pc->sessname[0] || !stricmp(pc->sessname, sDesc->sessionName)) {
		pc->session = *sDesc;
		pc->finished = TRUE;
		return FALSE;
	}

	return TRUE;
}

/*-------------------------------------------------------------------------
 Callback triggered by creating a player.
-------------------------------------------------------------------------*/
static void dp_PASCAL create_player_cb(dpid_t id, char_t *name, long flags, void *context)
{
	dpid_t *pMy_id = (dpid_t *)context;

	if (id != dp_ID_NONE) {
		DPRINT(("Created player id %d, name %s\n", id, name));
	} else {
		DPRINT(("Failed to create player\n"));
	}
	if (pMy_id) *pMy_id = id;

	(void) context;
	(void) flags;
}

/*--------------------------------------------------------------------------
 Remove the newline from end of string.
--------------------------------------------------------------------------*/
static void chopnl(char *string)
{
	int		len;

	len = strlen(string);
	if (len && (string[len - 1] == '\n'))
		string[len - 1] = 0;
}

/*--------------------------------------------------------------------------
 Return true if all len bytes of buf are zero.
--------------------------------------------------------------------------*/
static int allzeros(char *buf, int len)
{
	while(len--)
		if (*buf++)
			return FALSE;

	return TRUE;
}

#ifndef MAX_PATH
#define MAX_PATH 255
#endif
#if defined(_WIN32) || defined(UNIX)  /* No dpYield/dpReadAnetInf for Mac */

/*-------------------------------------------------------------------------
 Given a file pointer pointing to a dp_launchParams_t, initialize a
 dp_t and connect to or create a game session specified by the file.
 Do not return until the session has been established and a player has
 been created.
 During any delays, this function will call the given callback periodically
 to both inform the caller of its progress joining the session and to
 give the caller a chance to abort (by returning 0).
-------------------------------------------------------------------------*/
DP_API dp_result_t dpCommThaw(dp_t **pdp, FILE *thawfp, dpCommThawCallback_t cb, void *context)
{
	dp_result_t err;
	dp_appParam_t app;
	dp_session_t sess;
	dp_transport_t driver;
	dp_t *dp;
	char	 GameName[MAX_PATH]="";
	char	 GamePath[MAX_PATH]="";
	char	 GameArgs[MAX_PATH]="";
	char	 GameCwd[MAX_PATH]="";
	char	 GameShellOpts[MAX_PATH]="";
	int		 cancelStatus=0;
	dpid_t my_id;
	time_t deadline;

	/* KLUDGE: this has to be static, or 'finder.sessname' is corrupted
	 * when callback time comes around.  FIXME
	 * The way to fix this is to use session delta messages instead of
	 * dpEnumSessions, I think.
	 */
	static dp_launchParams_t params;	/* pointer to this gets left behind - shouldn't be referenced, but you never know */
	static dp_commThaw_sessfinder_t finder;

	char arg[256];

	*pdp = NULL;
	app.name = GameName;
	app.path = GamePath;
	app.args = GameArgs;
	app.cwd = GameCwd;
	app.shellOpts = GameShellOpts;

	if (!thawfp) {
		DPRINT(("dpCommThaw: null fp\n"));
		return dp_RES_BAD;
	}
	if (!fgets(arg, sizeof(arg), thawfp)) {
		DPRINT(("dpCommThaw: fread() failed to read from freeze.dat \n"));
		return dp_RES_BAD;
	}
	chopnl(arg);

	if (strcmp(arg, dp_LAUNCHPARAMS_T_SIG)) {
		DPRINT(("dpCommThaw: Error: freeze.dat signature mismatch!\n"));
		return dp_RES_BAD;
	}

	/* Read in the launchParams from the thaw file */
	while (fgets(arg, sizeof(arg), thawfp)) {
		chopnl(arg);
		if (!strcmp(arg, "!"))
			break;
		err = dp_launchParams_parse(&params, arg);
		if (dp_RES_OK != err) {
			DPRINT(("dpCommThaw: Bad argument '%s' in thaw file\n", arg));
		}
	}

	/* Load the app's anet.inf. */
	err = dpReadAnetInf(".", &app);
	if (err != dp_RES_OK) {
		DPRINT(("dpCommThaw: Can't read anet.inf.\n"));
		return dp_RES_BAD;
	}
	DPRINT(("dpCommThaw: After check: params.Driver=%s, params.Sessname=%s, params.Playname=%s, params.Adr=%s, params.Host=%d, params.Join=%d, params.Wait=%d, params.JoinAnyFlags=%d, params.Username=%s, params.Password=%s Cwd=%s, Exe=%s, Args=%s\n", params.Driver, params.Sessname, params.Playname, params.Adr, params.Host, params.Join, params.Wait, params.JoinAnyFlags, params.Username, params.Password, app.cwd, app.path, app.args));

	if (cb && !cb(cancelStatus, context))
		return dp_RES_BAD;
	params.commInitReq.phonenum = params.Phonenum;
	params.commInitReq.modeministr = params.Modeministr;

	/* This step MUST be done before initialisation */
	if (params.OpenAddresses[0]) {
		DPRINT(("dpCommThaw: Open Addresses %s\n", params.OpenAddresses));
		params.commInitReq.flags |= comm_INIT_FLAGS_CONN_ADDR;
		params.commInitReq.dialing_method = (int) params.OpenAddresses;
	}

	/* Load Activenet. */
	memset(&driver, 0, sizeof(driver));
	strcpy(driver.fname, params.Driver);
	err = dpCreate(&dp, &driver, &params.commInitReq, NULL);
	if (err == comm_STATUS_OK_AM_MASTER) {
		params.Host = TRUE;
	}
	else if (err != dp_RES_OK) {
		DPRINT(("dpCommThaw: Error %d creating!\n", err));
		return err;
	}

	if (params.GameServer[0]) {
		err = dpSetGameServerEx(dp, params.GameServer, app.sessionType);
		if (err != dp_RES_OK) {
			DPRINT(("dpCommThaw: Can't connect to game server %s, err %d\n", params.GameServer, err));
			return dp_RES_BAD;
		}

		/* Login using Username and Password (tserv login) */
		if (params.Username[0] != 0) {
			int got_result = FALSE;
			dp_uid_t dpuid = dp_UID_NONE;
			dp_uid_t tservuid = dp_UID_NONE;

			/* Send our username and password and wait for a response */
			err = dpAccountLoginA(dp, params.Username, params.Password);
			if (err != dp_RES_OK)
				return dp_RES_BAD;

			while (!got_result) {
				dpid_t idFrom;
				dpid_t idTo;
				char pkt[dpio_MAXLEN_UNRELIABLE];
				size_t size;
				size = sizeof(pkt);
				err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
				assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
				if ((err == dp_RES_OK)
				&&  (*((dp_packetType_t *)pkt) == dp_ACCOUNT_PACKET_ID)) {
					dp_account_packet_t *acctpkt = (dp_account_packet_t *)(pkt + sizeof(dp_packetType_t));
					dpuid = acctpkt->uid;
					got_result = TRUE;
					if (acctpkt->reason != dp_RES_OK) {
						DPRINT(("dpCommThaw: login rejected, reason:%d\n", acctpkt->reason));
						return dp_RES_BAD;
					}
				}
				dpYield();
				if (cb && !cb(cancelStatus, context))
					return dp_RES_BAD;
			}
			tservuid = tserv_hdl2uid(dp->tserv, PLAYER_ME);
			if (dpuid == dp_UID_NONE) {
				DPRINT(("dpCommThaw: acctpkt->uid == dp_UID_NONE!\n"));
				assert(FALSE);
			}
			if (tservuid == dp_UID_NONE) {
				DPRINT(("dpCommThaw: tserv_hdl2uid(PLAYER_ME) == dp_UID_NONE\n"));
				assert(FALSE);
			}
			if (dpuid != tservuid) {
				DPRINT(("dpCommThaw: tserv_hdl2uid(PLAYER_ME) != acctpkt->uid\n"));
				assert(FALSE);
			}
		}
	}
	/* Wait for game server login to succeed - otherwise we won't get
	 * our public address correct when we create the session.
	 * KLUDGE - we need a login result packet from the server to wait for!
	 */
	if (params.GameServer[0] && params.Host) {
		deadline = time(0) + 20;
		cancelStatus++;
		DPRINT(("dpCommThaw: t:%d, waiting for server login to complete\n", eclock()));
		while ((deadline - time(0)) >= 0) {
			dpid_t idFrom;
			dpid_t idTo;
			char pkt[dpio_MAXLEN_UNRELIABLE];
			size_t size;
			size = sizeof(pkt);
			err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
			assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
			dpYield();
			if (cb && !cb(cancelStatus, context))
				return dp_RES_BAD;
			if (dp_RES_BUSY != dpReadyToFreeze(dp))
				break;
		}
		/* Wait another few seconds just in case */
		deadline = time(0) + 2;
		DPRINT(("dpCommThaw: t:%d, extra wait for server login to complete\n", eclock()));
		cancelStatus++;
		while ((deadline - time(0)) >= 0) {
			dpid_t idFrom;
			dpid_t idTo;
			char pkt[dpio_MAXLEN_UNRELIABLE];
			size_t size;
			size = sizeof(pkt);
			err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
			assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
			dpYield();
			if (cb && !cb(cancelStatus, context))
				return dp_RES_BAD;
		}
		DPRINT(("dpCommThaw: t:%d, extra wait done\n", eclock()));
	}

	/* Start or wait for the session. */
	if (params.Host) {
		/* Create a room */
		dp_result_t openFinished;

		memset(&sess, 0, sizeof(sess));
		sess.sessionType = app.sessionType;
		sess.maxPlayers = params.Maxplayers;
		if (!params.Sessname[0])
			strncpy(sess.sessionName, "session", dp_SNAMELEN);
		else
			strncpy(sess.sessionName, params.Sessname, dp_SNAMELEN);
		sess.sessionName[dp_SNAMELEN-1] = 0;
		sess.flags = dp_SESSION_FLAGS_CREATESESSION;
		if (params.EnablePlayervars)
			sess.flags |= dp_SESSION_FLAGS_ENABLE_PLAYERVARS;
		openFinished = dp_RES_BUSY;
		err = dpOpen(dp, &sess, open_sess_cb, &openFinished);
		if (err != dp_RES_OK) {
			DPRINT(("dpCommThaw: Unable to create a session, error: %d\n", err));
			return dp_RES_BAD;
		} else {
			DPRINT(("dpCommThaw: Called dpOpen to create session; maxpl %d, flags %x, dwUser1 %x\n", sess.maxPlayers, sess.flags, sess.dwUser1));
		}
	} else {
		dp_result_t openFinished;
		if (params.Join & dp_LAUNCHPARAMS_JOIN_ANY) {
			/* do join any session during dpOpen() */
			openFinished = dp_RES_BUSY;
			if (params.Adr[0]) {
				memset(&sess, 0, sizeof(sess));
				err = dpResolveHostname(dp, params.Adr, (char*) sess.adrMaster);
				if (err != dp_RES_OK)
					return err;
				sess.sessionType = app.sessionType;
				sess.flags = params.JoinAnyFlags;
				err = dpOpen(dp, &sess, open_sess_cb, &openFinished);
			} else
				err = dpOpen(dp, NULL, open_sess_cb, &openFinished);
			if (err != dp_RES_OK) {
				DPRINT(("dpCommThaw: Unable to open the session, error: %d\n", err));
				return err;
			}
		} else {
			/* Sit around and wait for a room to appear, then join it. */

			memset(&finder, 0, sizeof(finder));
			deadline = time(0) + 60;	/* allow 1 minute to find room */
			do {
				/* List sessions */
				memset(&sess, 0, sizeof(sess));
				sess.sessionType = app.sessionType;
		 		finder.sessIdLen = dp->dpio->myAdrLen + sizeof(dp_karma_t);
				if (!allzeros(params.sessId, finder.sessIdLen)) {
					DPRINT(("dpCommThaw: Using session ID %s\n", key2a(params.sessId, finder.sessIdLen)));
					finder.sessId = params.sessId;
				} else if (params.Sessname[0])
				{
					DPRINT(("dpCommThaw: Using session name %s\n", params.Sessname));
					finder.sessname = params.Sessname;
				}
				finder.finished = FALSE;
				err = dpEnumSessions(dp, &sess, NULL, 100L, listSessions_cb, &finder);
				if (err != dp_RES_OK) {
					DPRINT(("dpCommThaw: error %d calling dpEnumSessions\n", err));
				} else {
					/* Lubricate network */
					cancelStatus++;
					while (!finder.finished) {
						dpid_t idFrom;
						dpid_t idTo;
						char pkt[dpio_MAXLEN_UNRELIABLE];
						size_t size;
						size = sizeof(pkt);
						err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
						assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
						if (cb && !cb(cancelStatus, context))
							return dp_RES_BAD;
					}
				}
			} while (!finder.session.karma && ((deadline - time(0)) >= 0));
			if (!finder.session.karma) {
				/* Note: if an enumsess is still active here, it still
				 * has a pointer to the callback function and its
				 * context pointer, which is why the context variable
				 * must be declared static.
				 * FIXME
				 */
				DPRINT(("dpCommThaw: Timeout.  Can't find session.\n"));
				return dp_RES_BAD;
			}

			openFinished = dp_RES_BUSY;
			err = dpOpen(dp, &finder.session, open_sess_cb, &openFinished);
			if (err != dp_RES_OK) {
				DPRINT(("dpCommThaw: Unable to open the session, error: %d\n", err));
				return dp_RES_BAD;
			}
		}

		/* Lubricate network */
		deadline = time(0) + 30;
		cancelStatus++;
		while ((openFinished == dp_RES_BUSY) && ((deadline - time(0)) >= 0)) {
			dpid_t idFrom;
			dpid_t idTo;
			char pkt[dpio_MAXLEN_UNRELIABLE];
			size_t size;
			size = sizeof(pkt);
			err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
			assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
			dpYield();
			if (cb && !cb(cancelStatus, context))
				return dp_RES_BAD;
		}
		if (openFinished != dp_RES_OK) {
			DPRINT(("dpCommThaw: Timeout.  Can't join session.\n"));
			return dp_RES_BAD;
		}
		DPRINT(("dpCommThaw: params.Joined session.\n"));
	}

	/* Create the user's player. */
	my_id = dp_ID_BROADCAST;
	if (dp->dpio->driverinfo->capabilities &  comm_DRIVER_KNOWS_PLAYERLST) {
		err = dpio_hdl2name(dp->dpio, PLAYER_ME, params.Playname, sizeof(params.Playname));
		if (err != dp_RES_OK) {
			DPRINT(("dpCommThaw: Can't get player name from transport\n"));
		}
	}
	err = dpCreatePlayer(dp, create_player_cb, &my_id, params.Playname);
	if (err != dp_RES_OK) {
		DPRINT(("dpCommThaw: Creating player \"%s\" returns err %d\n", params.Playname, err));
		return dp_RES_BAD;
	}

	/* Lubricate network */
	deadline = time(0) + 30;
	cancelStatus++;
	while ((my_id == dp_ID_BROADCAST) && ((deadline - time(0)) >= 0)) {
		dpid_t idFrom;
		dpid_t idTo;
		char pkt[dpio_MAXLEN_UNRELIABLE];
		size_t size;
		size = sizeof(pkt);
		err = dpReceive(dp, &idFrom, &idTo, 0, &pkt, &size);
		assert((err == dp_RES_OK) || (err == dp_RES_EMPTY));
		dpYield();
		if (cb && !cb(cancelStatus, context))
			return dp_RES_BAD;
	}
	if (my_id == dp_ID_NONE) {
		DPRINT(("dpCommThaw: Can't create player.\n"));
		return dp_RES_BAD;
	}
	if (my_id == dp_ID_BROADCAST) {
		DPRINT(("dpCommThaw: Timeout.  Can't create player.\n"));
		return dp_RES_BAD;
	}
	DPRINT(("dpCommThaw: Created player.\n"));
	*pdp = dp;
	return dp_RES_OK;
}
#endif

/*----------------------------------------------------------------------
 Sets which thread is allowed to make dp calls.
 Avoid using this call.  It's dangerous.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetActiveThread(dp_t *dp)
{
	precondition(dp);
#ifdef _WIN32
	dp->threadId = GetCurrentThreadId();
#endif
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Create a dp_t.
 Loads and initializes a transport DLL (aka comm driver).
 One of 'transport' or 'thawFilename' must not be NULL.
 If thawFilename is not NULL, igores the 'transport' and 'params' arguments,
 and restores a previous session from the given file.
 Otherwise, 'transport' controls which DLL to load, and 'params'
 is passed to the DLL's startup routine.
 (Consult the comm_driverInfo_t.needs field returned by dpEnumTransports
 to determine which fields of params must be filled in.)
------------------------------------------------------------------------*/
DP_API dp_result_t dpCreate(
	dp_t **pdp,
	dp_transport_t *transport,
	commInitReq_t *params,
	char *thawFilename)
{
	dp_t *dp;
	dp_result_t		err;
	dp_result_t		dpio_err;
	FILE *fp = NULL;

	precondition(pdp != NULL);
	precondition((transport != NULL) || (thawFilename != NULL));

	DPRINT(("dpCreate: enter(%p,", pdp));
	if(transport == NULL) {DPRINT(("NULL,"));} else {DPRINT(("%s,", transport));}
	if(params == NULL) {DPRINT(("NULL,"));} else {DPRINT(("%p,", params));}
	if(thawFilename == NULL) {DPRINT(("NULL)\n"));} else {DPRINT(("%s)\n", thawFilename));}

	/* Allocate and clear the object */
	dp = (dp_t *)dp_MALLOC(sizeof(dp_t));
	*pdp = NULL;
	if (!dp)
		return dp_RES_NOMEM;
	memset(dp, 0, sizeof(dp_t));

	dp->election_votes = assoctab_create(sizeof(dp_election_vote_t));

	/* Protect against access by multiple threads. */
	dpSetActiveThread(dp);

	dp->hMaster = PLAYER_NONE;
	dp->hGameServer = PLAYER_NONE;
	dp->ping_dest = PLAYER_NONE;

	/* Load state from disk, if filename specified. */
	if (thawFilename) {
		DPRINT(("dpCreate: Thawing from file %s\n", thawFilename));
		fp = fopen(thawFilename, "rb");
		if (!fp) return dp_RES_EMPTY;

#ifndef UNIX	/* dpCommThaw not in unix yet */
		err = dpCommThaw(pdp, fp, NULL, NULL);
		DPRINT(("dpCreate: dpCommThaw returns %d\n", err));
		if (err == dp_RES_OK) {
			fclose(fp);
			DPRINT(("dpCreate: Thaw successful.\n"));
			return dp_RES_OK;
		}
		rewind(fp);
#endif
		/* Init transport layer before calling dpThaw() */
		dp->now = eclock();
		err = dpio_create(&dp->dpio, NULL, NULL, &dp->now, fp);
		if (err != dp_RES_OK) {
			DPRINT(("dpCreate: thaw dpio_create fails, err:%d\n", err));
			fclose(fp);
			return err;
		}
		/* Restore upper layers */
		err = dpThaw(dp, fp);
		if (err == dp_RES_OK) {
			dp_assertValid(dp);
			*pdp = dp;
			DPRINT(("dpCreate: Thaw successful.\n"));
		} else {
			DPRINT(("dpCreate: Thaw failed, err:%d\n",err));
			/* Gotta shut down the networking, in case they try again
			 * without thawing.
			 */
			dpio_destroy(dp->dpio, 0);
		}

		fclose(fp);
		return err;
	}
	assert(!fp);
	DPRINT(("dpCreate: Creating new dp from scratch\n", thawFilename));

	/* Initialize the transport layer.
	 * If file pointer is not NULL, it will restore itself from that;
	 * otherwise, it will use the transport and params arguments
	 * to load and initialize a transport DLL.
	 * Note: because the DLL doesn't have methods to save or restore
	 * state from disk, params is used also to pass information *back* from
	 * the DLL that will be needed to restore the state of the DLL
	 * in case the caller calls dpCreate with a thaw filename next time.
	 */
	dp->now = eclock();
	dpio_err = dpio_create(&dp->dpio, transport, params, &dp->now, NULL);
	if ((dpio_err != comm_STATUS_OK_AM_MASTER) && (dpio_err != dp_RES_OK)) {
		DPRINT(("dpCreate: dpio_create fails, err:%d\n", dpio_err));
		return dpio_err;
	}
	/* reset dp->now in case dpio_create took a long time */
	dp->now = eclock();
	/* Bogus kludge: initialize external addr to same as internal addr. */
	memcpy(dp->my_outside_adr, dp->dpio->myAdr, dp->dpio->myAdrLen);

	/* Initialize the table transfer layer. */
	dp->dt = dptab_create(dp->dpio);
	if (!dp->dt)
		return dp_RES_NOMEM;

	{
		/* Initialize a fresh set of tables etc. */
		char key[dptab_KEY_MAXLEN];

		/* Create the set that holds all sessions. */
		DPRINT(("dpCreate: Creating session context list.\n"));
		dp->sessionContexts = dynatab_create(sizeof(dp_sessionContext_t *));
		if (!dp->sessionContexts)
			return dp_RES_NOMEM;
		dumpSessionContexts(dp);

		/* Create the table that maps handles to sessions. */
		dp->hdl2sess = assoctab_create(sizeof(dp_sessionContext_t *));
		if (!dp->hdl2sess)
			return dp_RES_NOMEM;

		/* Create the table of sessions hosted by this node. */
		key[0] = dp_KEY_MYSESSIONS;
		err = dptab_createTable(dp->dt, &dp->mysessions, key, 1, 0, NULL, NULL, NULL, NULL);
		if (err != dp_RES_OK)
			return err;

		/* Create the table of known sessions. */
		key[0] = dp_KEY_SESSIONS;
		err = dptab_createTable(dp->dt, &dp->sessions, key, 1, 0, NULL, NULL, dp_sessions_cb, dp);
		if (err != dp_RES_OK)
			return err;

		/* Zero out the player and group tables. */
		dp->players = NULL;
		dp->groups = NULL;
	}

	/* Following stuff partly duplicated in dpThaw().  Yech.  FIXME */

	/* We need to know when the transport layer connects
	 * to new hosts or disconnects from existing hosts.
	 * Must pass this same callback on all dpio_openHdl()'s, too.
	 */
	dpio_setIncomingCallback(dp->dpio, dp_openHdl_cb, dp);

	/* If broadcast is allowed, there's no gameserver, so set up
	 * to broadcast our mysession table.
	 */
	if (!(dp->dpio->driverinfo->capabilities& comm_DRIVER_NO_BROADCAST)) {
		err = dp_setSessionTablePeer(dp, PLAYER_BROADCAST, 0);
		if (err != dp_RES_OK) {
			DPRINT(("dpCreate: can't set PLAYER_BROADCAST (h:%x) as session peer, err:%d\n", dp->hGameServer, err));
			dpClose(dp);
			return err;
		}
	}

	/* Set up periodic stuff. */
	dp->beacon_interval = (dp->dpio->clocksPerSec * DP_BEACON_INT_MSEC)/1000;
	if (dp->beacon_interval == 0)
		dp->beacon_interval = 1;				/* don't let it be zero */
	dp->next_beacon = dp->now + dp->beacon_interval;
	dp->next_beacon2 = dp->now + 2*dp->beacon_interval;
	dp->next_beacon4 = dp->now + 4*dp->beacon_interval;

	/* Setup for dpShutdown() */
	dp->quitState = 0;

	/* Success. */
	*pdp = dp;
	dp_assertValid(dp);
	if (dpio_err == comm_STATUS_OK_AM_MASTER)
		return comm_STATUS_OK_AM_MASTER;
	ptimer_zeroAll();
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Destroy a dp_t.
 Shuts down and unloads the comm driver (!).
 Also saves the list of game servers to disk, if appropriate.
 If the flags argument is non-zero, does not hang up the phone line (if
 any).
------------------------------------------------------------------------*/
DP_API dp_result_t dpDestroy(
	dp_t *dp,
	int flags)
{
	precondition(dp != NULL);

	DPRINT(("dpDestroy: entering, flags:%d\n", flags));
	dp_saveServerList(dp);
    #if defined(dp_ANET2)
		if (dp->hGameServer != PLAYER_NONE)
			dp_freezeAppsList(dp);
	#endif

	/* destry score table created from dpReportScore2 */
	dpscore_client_cleanup(dp);

	if (dp->tserv) {
		tserv_destroy(dp->tserv);
		dp->tserv = NULL;
		tca_destroy(dp->tca);
		dp->tca = NULL;
	}

	dptab_destroy(dp->dt);
	dpio_destroy(dp->dpio, flags);
	assoctab_destroy(dp->hdl2sess);

	assoctab_destroy(dp->election_votes);

	memset(dp, 0, sizeof(dp_t));
	dp_FREE(dp);

	ptimer_dumpToFile(NULL);
	DPRINT(("dpDestroy: returning OK\n"));
	dp_flushLog();

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Disconnect from the current game server.
 Close its handle unconditionally.  Any session joined there is exited
 eventually.
--------------------------------------------------------------------------*/
DP_API dp_result_t dpCloseGameServer(
	dp_t *dp)
{
	playerHdl_t h;

	precondition(dp != NULL);
	dp_assertValid(dp);

	/* destry score table created from dpReportScore2 */
	dpscore_client_cleanup(dp);

	h = dp->hGameServer;

	/* Clear old list of sessions, and don't let him publish any more to us;
	 * a few may leak thru, but will go away when close handle callback fires.
	 * Note: deleting the table would cause a crash if any more packets
	 * come in and were routed there; this actually happened when running
	 * the load app with n=20 and telling the load app to repeatedly
	 * disconnect and reconnect.  So play it safe and don't delete the table.
	 */
	if (dp->sessions) {
		char key[dptab_KEY_MAXLEN];
		int n;

		dptab_delete_byAge(dp->dt, dp->sessions, 0);
		n = dptab_tableSize(dp->sessions);
		if (n != 0) {
			DPRINT(("dpCloseGameServer: bug: %d sessions left in table after clearing?\n", n));
		}

		key[0] = dp_KEY_SESSIONS;
		dptab_deletePublisher(dp->dt, key, 1, h);
		key[1] = dpGETSHORT_FIRSTBYTE(dp->defaultSessionType);
		key[2] = dpGETSHORT_SECONDBYTE(dp->defaultSessionType);
		dptab_deletePublisher(dp->dt, key, 3, h);
	}

	/* Close the handle to the old game server.  Our callback will
	 * delete him as a dptab peer, too, eventually, possibly after
	 * a few more packets from him arrive.
	 */
	dp->hGameServer = PLAYER_NONE;
	if (h != PLAYER_NONE) {
		dp_freezeAppsList(dp);	/* freeze 'em before the handle is closed! */

		DPRINT(("dpCloseGameServer: closing hGameServer h:%x\n", h));
		dpio_closeHdl(dp->dpio, h);
	}

	/* If we had any authentication going, nuke it. */
	if (dp->tserv) {
		tserv_destroy(dp->tserv);
		dp->tserv = NULL;
		tca_destroy(dp->tca);
		dp->tca = NULL;
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Convert an ASCII hostname into a binary address.
--------------------------------------------------------------------------*/
DP_API dp_result_t dpResolveHostname(
	dp_t *dp,
	char *hostname,
	char adrbuf[dp_MAX_ADR_LEN])
{
	precondition(dp != NULL);
	dp_assertValid(dp);

	if (dp->serverpings) {
		int i;
		dp_serverInfo_t *server;
		char subkey[dptab_KEY_MAXLEN];
		size_t serverlen;
		int subkeylen;
		for (i = 0; i < dptab_tableSize(dp->serverpings); i++) {
			if (dptab_get_byindex(dp->serverpings, i, (void **)&server,
				&serverlen, subkey, &subkeylen) == dp_RES_OK) {
				if (!stricmp(hostname, server->hostname)
				&&  (server->loss_percent < 100)) {
					memcpy(adrbuf, subkey, subkeylen);
					return dp_RES_OK;
				}
			}
		}
	}

	if (dpio_scanAdr(dp->dpio, hostname, adrbuf, dp_MAX_ADR_LEN) == 0)
		return dp_RES_BAD;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Call this with an open playerHdl_t to exchange session data with.
 On the Internet, this is a handle to the game server;
 on a LAN, it is PLAYER_BROADCAST.
--------------------------------------------------------------------------*/
static dp_result_t dp_setSessionTablePeer(
	dp_t *dp,
	playerHdl_t h,
	dp_species_t sessionType)
{
	char key[dptab_KEY_MAXLEN];
	int keylen;
	dp_result_t err;

	precondition(dp != NULL);
	dp_assertValid(dp);

	DPRINT(("dp_setSessionTablePeer: sessionType %d, h:%x\n", sessionType, h));
	/* Register the game server as a dptab peer. */
	err = dptab_addPeer(dp->dt, h);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		DPRINT(("dp_setSessionTablePeer: can't add h:%x to peer table, err:%d\n", h, err));
		dp_assertValid(dp);
		return err;
	}
	/* When the game server sends us his SESSIONS table, put it in ours. */
	/* When a peer sends us his MYSESSIONS table, put in our SESSIONS table.*/
	key[0] = (char) ((h == PLAYER_BROADCAST) ? dp_KEY_MYSESSIONS : dp_KEY_SESSIONS);
	keylen = 1;
#ifdef dp_MULTISESSTABLE
	/* If connecting to a game server, request the game specific session
	 * table rather than the generic session table.
	 */
	if ((h != PLAYER_BROADCAST) && (sessionType != 0)) {
		key[keylen++] = dpGETSHORT_FIRSTBYTE(sessionType);
		key[keylen++] = dpGETSHORT_SECONDBYTE(sessionType);
		/* Tell dp_unpack_session what the default session type is */
		dp->defaultSessionType = sessionType;
		DPRINT(("dp_setSessionTablePeer: setting defaultSessionType to %d\n", sessionType));
	}
#endif
	err = dptab_addPublisher(dp->dt, dp->sessions, key, keylen, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_setSessionTablePeer: can't add h:%x as publisher?, err:%d\n", h, err));
		dp_assertValid(dp);
		return err;
	}

	/* Send the game server our MYSESSIONS table and any changes to it. */
	err = dptab_addSubscriber(dp->dt, dp->mysessions, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_setSessionTablePeer: can't add h:%x as subscriber?, err:%d\n", h, err));
		dp_assertValid(dp);
		return err;
	}

	/* Subscribe to the applications table and send it records from our
	 * exception log, if it's a real server */
	if (h != PLAYER_BROADCAST) {
		err = dp_subscribeAppsList(dp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_setSessionTablePeer: can't subscribe to apps list, err:%d\n", err));
			dp_assertValid(dp);
			return err;
		}

		err = dp_publishExceptions(dp->dt, h, NULL);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			DPRINT(("dp_setSessionTablePeer: can't publish exceptions, err:%d\n", err));
			dp_assertValid(dp);
			return err;
		}
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Same as dpSetGameServerEx, but gets sessionType by calling dpReadAnetInf().
 Obsolete.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetGameServer(
	dp_t *dp,
	char *masterHostName)	/* server's name, or NULL to clear */
#ifdef dp_MULTISESSTABLE
{
	dp_result_t err;
	dp_appParam_t app;
	dp_species_t sessionType = 0;

	err = dpGetAppVersion(dp, &app);
	if ((err == dp_RES_EMPTY) || (err == dp_RES_OK)) {
		sessionType = app.sessionType;
	}
	return dpSetGameServerEx(dp, masterHostName, sessionType);
}

/*------------------------------------------------------------------------
 When using an Internet driver, call this function with the ascii name of
 the server, and explicit requests for sessions will be sent to that server.
 Closes any open sessions.

 Call with NULL to clear the game server.

 dpSetGameServerEx lets you specify the session type you will be calling
 dpEnumSessions() later with.  This tells the game server to send only
 information about that particular session type.
 To get information about all session types, specify 0 for sessionType.

 Returns:
 dp_RES_OK         Server set/cleared successfully
 dp_RES_BUG        Internal error.
 dp_RES_BAD        Called with invalid parameters, or internal error.
 dp_RES_HOST_NOT_RESPONDING  Could not resolve new host name.
 dp_RES_BUSY       Trying to open server that is being closed
 dp_RES_FULL       Transmission queue full.
 (Other values may also be returned.)
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetGameServerEx(
	dp_t *dp,
	char *masterHostName,	/* server's name, or NULL to clear */
	dp_species_t sessionType)
#endif
{
	dp_result_t err;
	unsigned char newHostAdr[dp_MAX_ADR_LEN];
	char pktbuf[10];
	size_t pktlen;

	precondition(dp != NULL);
	dp_assertValid(dp);

	DPRINT(("dpSetGameServerEx: masterHostName %s, sessionType %d\n", masterHostName, sessionType));
	/* Check condition of desired server. */
	if (masterHostName && *masterHostName) {
		playerHdl_t h;

		if (strlen(masterHostName) >= dp_MAX_SERVERNAMELEN) {
			DPRINT(("dpSetGameServerEx: masterHostName too long\n"));
			return dp_RES_BADSIZE;
		}

		/* Let them log in even if they didn't finish shutting down */
		dp->quitState = 0;

		err = dpResolveHostname(dp, masterHostName, (char*) newHostAdr);
		if (err != dp_RES_OK) {
			DPRINT(("Unable to get address, err:%d\n", err));
			/*dpCloseGameServer(dp);*/
			dp_assertValid(dp);
			return dp_RES_HOST_NOT_RESPONDING;
		}

		/* Get handle, but don't open connection */
		h = dpio_openHdlRaw(dp->dpio, newHostAdr);
		/* if connection already exists, check... */
		if (dpio_getHdlTxPktsFree(dp->dpio, h) != -1) {
			/* If we're in the process of closing the desired server. */
			short state = dpio_getHdlState(dp->dpio, h);
			if ((state & (dpio_STATE_FIN_SENT | dpio_STATE_FIN_RCVD))) {
				DPRINT(("dpSetGameServer: return busy; state:%x, t:%d, h:%x is closing.\n", state, dp->now, h));
				dp_assertValid(dp);
				return dp_RES_BUSY;
			}
			/* If we're reopening the open server, do nothing. */
			if (h == dp->hGameServer) {
				DPRINT(("dpSetGameServer: h:%x already open.\n", h));
				dp_assertValid(dp);
				return dp_RES_OK;
			}
		}
		strcpy(dp->nameGameServer, masterHostName);
	}

#if 0
	/* Clear out any open session.  This lets us correctly set dwReserved1
	 * in the session description to be the game server address.
	 * (That kludge lets game servers avoid getting their own sessions
	 * back from the master server, which might reduce traffic at the
	 * master game server by 10% if we have ten game servers.  Hmm.)
	 */
	if (dp->players)
		dpSendSessionLostPacket(dp);
	dpClose(dp);
#endif

	/* Clear out old sessions */
	dpCloseGameServer(dp);
	if (!masterHostName || !*masterHostName) {
		dp_assertValid(dp);
		return dp_RES_OK;
	}

	/* Open a handle to the game server and send a login request */
	memcpy(dp->adrGameServer,newHostAdr,dp->dpio->myAdrLen);
	dp->hGameServer = dpio_openHdl(dp->dpio, dp->adrGameServer, NULL, NULL);
	DPRINT(("dpSetGameServer: opened comm h:%x for server %s.  sessionType %d\n", dp->hGameServer, masterHostName, sessionType));
	if (dp->hGameServer == PLAYER_NONE) {
		DPRINT(("dpSetGameServer: bad handle to game server\n"));
		return dp_RES_BUG;
	}

	/* Tell the server what session type we need. */
	pktbuf[0] = 'e';
	pktbuf[1] = '1';
	pktlen = 2;
#ifdef dp_MULTISESSTABLE
	if (sessionType != 0) {
		pktbuf[pktlen++] = dpGETSHORT_FIRSTBYTE(sessionType);
		pktbuf[pktlen++] = dpGETSHORT_SECONDBYTE(sessionType);
	}
#endif
	err = dpio_put_reliable(dp->dpio, &dp->hGameServer, 1,pktbuf,pktlen, NULL);
	if (err != dp_RES_OK) {
		DPRINT(("dpSetGameServer: can't send login pkt to h:%x, err:%d\n", dp->hGameServer, err));
		dpCloseGameServer(dp);
		dp_assertValid(dp);
		return err;
	}

	/* Start the secure login process.  Next step is for user to call
	 * dpAccountLogin() or dpAccountCreate().
	 */
	if (!dp->tserv) {
		dp->tca = tca_create();
		if (!dp->tca) {
			DPRINT(("dpSetGameServer: can't tca_create!\n"));
			dpCloseGameServer(dp);
			dp_assertValid(dp);
			return dp_RES_NOMEM;
		}
		dp->tserv = tserv_create(dp->dpio, dp->tca);
		if (!dp->tserv) {
			DPRINT(("dpSetGameServer: can't tserv_create!\n"));
			dpCloseGameServer(dp);
			dp_assertValid(dp);
			return dp_RES_NOMEM;
		}
	}

	err = tserv_server_add(dp->tserv, dp->hGameServer);
	if (err != dp_RES_OK) {
		DPRINT(("dpSetGameServer: can't tserv_server_add(h:%x), err:%d\n", dp->hGameServer, err));
		dpCloseGameServer(dp);
		dp_assertValid(dp);
		return err;
	}

	err = dp_setSessionTablePeer(dp, dp->hGameServer, sessionType);
	if (err != dp_RES_OK) {
		DPRINT(("dpSetGameServer: can't set h:%x as session peer, err:%d\n", dp->hGameServer, err));
		dpCloseGameServer(dp);
		dp_assertValid(dp);
		return err;
	}
	DPRINT(("dpSetGameServer: called setSessionTablePeer. h:%x, host %s, sessionType %d, dp->defaultSessionType %d\n", dp->hGameServer, masterHostName, sessionType, dp->defaultSessionType));

	/* Arrange for an updated list of servers, now that we have a gameserver. */
	dp_initEnumServers(dp);

	/* Tell dpio to alert us if the game server goes down or becomes
	 * unreachable.
	 */
	err = dpio_setHandleOptions(dp->dpio, dp->hGameServer, dpio_OPTION_KEEPALIVE);
	if (err != dp_RES_OK) {
		DPRINT(("dpSetGameServer: can't set handle options, err %d\n", err));
		return dp_RES_BUG;
	}

	/* Get ready to exchange score data with the server. */
	err = dpscore_client_init(dp);
	if (err != dp_RES_OK) {
		DPRINT(("dpSetGameServer: can't init score xfer, err %d\n", err));
		return dp_RES_BUG;
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Retrieve the current game server name and default session type.
 Returns dp_RES_FULL if buffer too small,
 dp_RES_NOTYET if no server has been set.
--------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetGameServerEx(
	dp_t *dp,
	char *masterHostNameBuf,
	size_t masterHostNameBufLen,
	dp_species_t *psessionType)
{
	precondition(dp != NULL);
	precondition(masterHostNameBuf != NULL);

	if (!dp->nameGameServer[0])
		return dp_RES_NOTYET;

	if (strlen(dp->nameGameServer) >= masterHostNameBufLen)
		return dp_RES_FULL;

	strcpy(masterHostNameBuf, dp->nameGameServer);

	if (psessionType)
		*psessionType = dp->defaultSessionType;

	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Retrieve the current session description.
 *pbuflen must be filled with the size of the buffer before calling;
 it will be set to sizeof(dp_session_t).
 If pbuflen is NULL, it is assumed that the buffer is the right size.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetSessionDesc(
	dp_t *dp,
	dp_session_t *buf,
	size_t *pbuflen)
{
	char* rawbuf;
	size_t len;
	dp_result_t err;
	dptab_table_t *sesstab;

	precondition(dp != NULL);
	precondition(buf != NULL);
	dp_assertValid(dp);

	if (pbuflen && (*pbuflen < sizeof(dp_session_t)))
		return dp_RES_FULL;

	if (!dp->players) {
		DPRINT(("dpGetSessionDesc: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	if (pbuflen)
		*pbuflen = sizeof(dp_session_t);

	sesstab = dp->sessions;
	if (dp->hMaster == PLAYER_ME)
		sesstab = dp->mysessions;

	/* Get pointer to the current session entry in mysessions */
	err = dptab_get_bykey(sesstab, dp->sess_subkey, dp->sess_subkeylen, (void **)&rawbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
#if 0
		return err;
#else
		/* use most recently cached copy */

		*buf = dp->election_old_session;
#endif
	} else {
		/* Must unpack when retrieving records from dptab */
		dp_unpack_session(dp, dp->sess_subkey, dp->sess_subkeylen, rawbuf, len, buf);
	}
	dprint_sess(dp, buf, "dpGetSessionDesc");

	if (dp->sess_karma != buf->karma) {
		DPRINT(("dpGetSessionDesc: bug: dp->sess_karma %d != buf->karma %d\n",
			dp->sess_karma, buf->karma));
		return dp_RES_BUG;
	}
	if (dp->sess_karma == 0) {
		DPRINT(("dpGetSessionDesc: bug: dp->sess_karma == 0\n"));
		return dp_RES_BUG;
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

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
	size_t *pbuflen)
{
	int keylen;
	char* rawbuf;
	size_t len;
	dp_result_t err;

	precondition(dp != NULL);
	precondition(buf != NULL);
	dp_assertValid(dp);

	if (pbuflen && (*pbuflen < sizeof(dp_session_t)))
		return dp_RES_FULL;

	if (pbuflen)
		*pbuflen = sizeof(dp_session_t);

	keylen = dp->dpio->myAdrLen + sizeof(dp_karma_t);

	if (dp->players && !memcmp(dp->sess_subkey, id, keylen))
		return dpGetSessionDesc(dp, buf, pbuflen);

	/* Get pointer to the current session entry in mysessions */
	err = dptab_get_bykey(dp->sessions, id, keylen, (void **)&rawbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return err;
	} else {
		/* Must unpack when retrieving records from dptab */
		dp_unpack_session(dp, id, keylen, rawbuf, len, buf);
	}
	dprint_sess(dp, buf, "dpGetSessionDescById");
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Change the name or user data of the current session.
 May be called only by master.
 Flags is currently unused.
 For internal use only.
------------------------------------------------------------------------*/
static dp_result_t dp_setSessionDesc(
	dp_t *dp,
	dp_session_t *buf,
	long flags)
{
	dp_result_t err;
	dp_session_t desc;
	char rawbuf[sizeof(dp_session_t)];
	size_t rawbuflen;

	precondition(dp != NULL);
	precondition(buf != NULL);
	dp_assertValid(dp);

	if (dp->hMaster != PLAYER_ME) {
		DPRINT(("dpSetSessionDesc: must be master\n"));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	/* Force values of a few flags - FIXME */
	desc = *buf;
#ifdef USE_dp_enableNewPlayers
	desc.flags &= ~dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
	if (dp->enableNewPlayers)
		desc.flags |= dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
#endif

	/* Must pack when writing to dptab */
	rawbuflen = dp_pack_session(dp, 0, &desc, rawbuf);

	/* Set new value for current session's subkey.
	 * Allow four hops (client -> server -> masterserver -> server -> client)
	 */
	err = dptab_set(dp->dt, dp->mysessions, dp->sess_subkey, dp->sess_subkeylen, rawbuf, rawbuflen, 4, PLAYER_ME);

	if (!(dp->dpio->driverinfo->capabilities&comm_DRIVER_NO_BROADCAST)) {
		/* Some games rely on sessions showing up immediately in sessions
		 * table when not connected to an internet game server.  Oblige them.
		 */
		err = dptab_set(dp->dt, dp->sessions, dp->sess_subkey,
						dp->sess_subkeylen, rawbuf, rawbuflen, 1, PLAYER_ME);
	}

	(void) flags;
	dprint_sess(dp, buf, "dpSetSessionDesc");

	dp_assertValid(dp);
	return err;
}

/*------------------------------------------------------------------------
 Change the name or user data of the current session.
 May be called only by master.
 Flags is currently unused.
 The only fields of the dp_session_t that may be changed with this function
 are dwUser1, maxPlayers, szUserField, and sessionName.
 To set or clear the dp_SESSION_FLAGS_ENABLE_NEWPLAYERS flag bit, call
 dpEnableNewPlayers().
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetSessionDesc(
	dp_t *dp,
	const dp_session_t *buf,
	long flags)
{
	dp_session_t desc;

	precondition(dp != NULL);
	precondition(buf != NULL);
	dp_assertValid(dp);

	/* Force values of all but a few fields */
	dpGetSessionDesc(dp, &desc, 0);
	desc.dwUser1 = buf->dwUser1;
	desc.maxPlayers = buf->maxPlayers;
	memcpy(desc.szUserField, buf->szUserField, dp_USERFIELDLEN);
	memcpy(desc.sessionName, buf->sessionName, dp_SNAMELEN);

	return dp_setSessionDesc(dp, &desc, flags);
}

/*----------------------------------------------------------------------
 Retrieve the capabilities of the currently loaded transport,
 plus some info about the current session.
 If flags is dp_SEND_RELIABLE, retrieves info about reliable packet services
 instead of unreliable services.
----------------------------------------------------------------------*/
DP_API dp_result_t dpGetCaps(
	dp_t *dp,
	dp_caps_t *info,
	long flags)
{
	precondition(dp != NULL);
	precondition(dp->dpio != NULL);
	precondition(dp->dpio->driverinfo != NULL);
	precondition(info != NULL);
	dp_assertValid(dp);

	memset(info, 0, sizeof(*info));

	info->dwSize = sizeof(*info);

	if (flags & dp_SEND_RELIABLE)
		info->dwMaxBufferSize = dp_MAXLEN_RELIABLE;
	else
		info->dwMaxBufferSize = dp_MAXLEN_UNRELIABLE;

	info->dwMaxPlayers = dp_MAXREALPLAYERS;
	info->dwMaxLocalPlayers = 1;	/* not true, but haven't really tested higher */

	if (dp->hMaster == PLAYER_ME)
		info->dwFlags |= dp_CAPS_FLAGS_ISHOST;

	info->dwLatency = dpio_get_latency(dp->dpio, flags);

	DPRINT(("dpGetCaps: dwFlags:%x dwMaxPlayers:%d\n", info->dwFlags, info->dwMaxPlayers));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Retrieve the capabilities of the currently loaded transport,
 plus some info about the given player, especially the current latency and
 packet loss to that player.
 If flags is dp_SEND_RELIABLE, retrieves info about reliable packet services
 instead of unreliable services.
----------------------------------------------------------------------*/
DP_API dp_result_t dpGetPlayerCaps(
	dp_t *dp,
	dpid_t id,
	dp_caps_t *info,
	long flags)
{
	int	loss;
	precondition(dp != NULL);
	precondition(dp->dpio != NULL);
	precondition(dp->dpio->conns != NULL);
	precondition(dp->dpio->driverinfo != NULL);
	precondition(info != NULL);
	dp_assertValid(dp);

	dpGetCaps(dp, info, flags);

	info->dwFlags &= ~dp_CAPS_FLAGS_ISHOST;
	/* Don't want to go to trouble of determining who's host yet */
	/* if (id == dp->players[0].id)
		info->dwFlags |= dp_CAPS_FLAGS_ISHOST;
	 */

	info->dwLatency = dpio_get_player_latency(dp->dpio, dpid2commHdl(dp, id), flags, &loss);
	info->dwPktLoss = loss;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/******************** Session Management *********************************/

/*----------------------------------------------------------------------
 Start sending the given machine our player, etc. tables, and get ready
 to accept new player records, etc. from him.
----------------------------------------------------------------------*/
static dp_result_t dp_subscribe_client(dp_t *dp, playerHdl_t h, dp_sessionContext_t *sp)
{
	int i;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	dp_result_t err;

	if (h == PLAYER_ME) {
		return dp_RES_OK;
	}

	/* We are the master of an open session, or of a cast-off session. */
	assert((dp->hMaster == PLAYER_ME) || (dp->hMaster == PLAYER_NONE));

	dpio_setHandleOptions(dp->dpio, h, dp_getDesiredHandleOptions(dp, h));

	err = dptab_addPeer(dp->dt, h);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		DPRINT(("dp_subscribe_client: can't add h:%x to peer table, err:%d\n", h, err));
		dp_assertValid(dp);
		return err;
	}
#ifdef SEMIRELIABLE_SESSIONS
	/* Battlezone relies on session records being sent reliably, which is a
	 * no-no.  Attempt to simulate this in the case when dpOpen unsubscribes
	 * us from the game server's session table.
	 */
	DPRINT(("dp_subscribe_client: IAmGameServer:%d flags:%x\n", bIAmGameServer(dp), dp->election_old_session.flags));

	if ((dp->hGameServer != PLAYER_NONE)
	&&  !(dp->election_old_session.flags & dp_SESSION_FLAGS_ISLOBBY)) {
		err = dptab_addSubscriber(dp->dt, dp->mysessions, h);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribe_client: players: subscribe h:%x?, err:%d\n",
					h, err));
			dp_assertValid(dp);
			return err;
		}
	}
#endif
	err = dptab_addSubscriber(dp->dt, sp->players, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribe_client: players: subscribe h:%x?, err:%d\n",
				h, err));
		dp_assertValid(dp);
		return err;
	}
	err = dptab_addSubscriber(dp->dt, sp->hosts, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribe_client: hosts: subscribe h:%x?, err:%d\n",
				h, err));
		dp_assertValid(dp);
		return err;
	}
	err = dptab_addSubscriber(dp->dt, sp->groups, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribe_client: groups: subscribe h:%x?, err:%d\n",
				h, err));
		dp_assertValid(dp);
		return err;
	}
	for (i = 0; i < dp_MAX_GROUPS; i++) {
		err = dptab_addSubscriber(dp->dt, sp->grplayers[i], h);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribe_client: grplayers[%d]: subscribe h:%x?, err:%d\n",
					i, h, err));
			dp_assertValid(dp);
			return err;
		}
	}

	/* FIXME: To prevent race conditions, there should be 4 phases to this
	 * process:
	 * 1) When a player notices he will probably become host, he should start
	 * listening to all other players' myplayers tables.
	 * This is implemented by dp_subscribe_client(), but probably shouldn't be.
	 *
	 * 2) When a player notices that someone else will probably become host,
	 * he should start listening to that player's players table.
	 * This is implemented by dp_election_set_master().
	 *
	 * 3) When a player notices that he is definitely the new host, he should
	 * send a victory packet and start sending his players table to all other
	 * players.
	 * This is implemented by dp_subscribe_client().
	 *
	 * 4) When a player notices that someone else has definitely become the
	 * new host, he should start sending his myplayers table.
	 * This is implemented by dp_subscribe_host().
	 *
	 * Steps 1 and 2 can happen in either order.
	 * Currently, steps 1 and 3 are done at the same time, which seems wrong.
	 */

	/* The client's myplayers table goes into the host's myplayers table;
	 * a listener on the host's myplayers table copies the records into
	 * the players table- and adds the uid as it goes, if anyone happens
	 * to have one.
	 */
	keylen = 3;
	key[0] = dp_KEY_MYPLAYERS;
	key[1] = sp->players->key[1];
	key[2] = sp->players->key[2];
	err = dptab_addPublisher(dp->dt, sp->myplayers, key, keylen, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribe_client: myplayers: publisher h:%x, err:%d\n",
				h, err));
		dp_assertValid(dp);
		return err;
	}

	/* Let the client publish directly into our group players
	 * table. (Risque, but until we feel like writing a callback, easy.)
	 */
	keylen = 5;
	key[0] = dp_KEY_GROUP_MYPLAYERS;
	for (i = 0; i < dp_MAX_GROUPS; i++) {
		dpid_t gid = (dpid_t) (dp->firstGId + i);
		key[3] = (char) dpGETSHORT_FIRSTBYTE(gid);
		key[4] = (char) dpGETSHORT_SECONDBYTE(gid);
		err = dptab_addPublisher(dp->dt, sp->grplayers[i], key, keylen, h);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribe_client: grplayers[%d]: publisher h:%x?, err:%d\n",
					i, h, err));
			dp_assertValid(dp);
			return err;
		}
	}
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Start sending the given host our myplayer, etc. tables.
----------------------------------------------------------------------*/
static dp_result_t dp_subscribe_host(dp_t *dp, playerHdl_t h, dp_sessionContext_t *sp)
{
	dp_result_t err;
	int i;

	dpio_setHandleOptions(dp->dpio, h, dp_getDesiredHandleOptions(dp, h));
	err = dptab_addSubscriber(dp->dt, dp->myplayers, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribe_host: can't add subscriber h:%x for myplayer, err:%d\n",
				h, err));
		return err;
	}
	for (i = 0; i < dp_MAX_GROUPS; i++) {
		/* Send group myplayers tables to host */
		err = dptab_addSubscriber(dp->dt, sp->grmyplayers[i], h);
		if (err != dp_RES_OK) {
			DPRINT(("dp_subscribe_host: can't add subscriber h:%x for grmyplayer, err:%d\n",
					h, err));
			return err;
		}
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Only for game servers.
 When a new client comes online, send it our SESSIONS table,
 and when it sends us its MYSESSIONS table, put it in our MYSESSIONS table.
 (If we're the master gameserver, dump it into the sessions table
 instead.)
-------------------------------------------------------------------------*/
dp_result_t DP_APIX dp_addClient(dp_t *dp, playerHdl_t src,
									char *body, size_t len)
{
	dp_result_t err;
	dptab_table_t *target;

	target = dp->sessions; /* dp->sessions on slave srvr */

	DPRINT(("dp_addClient: h:%x; sucking his MYSESSIONS into my sessions table\n",
			src));

	err = dptab_addPeer(dp->dt, src);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		DPRINT(("dp_addClient: can't addPeer, err:%d\n", err));
		return err;
	}

	/* Send our session table to the new client. */
	if (len == 0) {
		/* Original case: subscribe to master session table */
		err = dptab_addSubscriber(dp->dt, dp->sessions, src);
		if (err != dp_RES_OK) {
			DPRINT(("dp_addClient: can't addSubscriber, err:%d\n", err));
			return err;
		}
	} else if (len == 2) {
		/* New case: subscribe to a particular session table */
		char key[3];
		dptab_table_t *tab;
		dp_species_t sessionType = dpMAKESHORT(body[0], body[1]);
		/* Locate (and possibly create) the session-specific table */
		key[0] = dp_KEY_SESSIONS;
		key[1] = dpGETSHORT_FIRSTBYTE(sessionType);
		key[2] = dpGETSHORT_SECONDBYTE(sessionType);
		err = dptab_createTable(dp->dt, &tab, key, 3, 0, NULL,NULL,NULL,NULL);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("Can't locate session table\n"));
			return err;
		}
		err = dptab_addSubscriber(dp->dt, tab, src);
		if (err != dp_RES_OK) {
			DPRINT(("dp_addClient: can't addSubscriber, err:%d\n", err));
			return err;
		}
	} else {
		DPRINT(("dp_addClient: bad login packet len %d\n", len));
	}
	/* Accept the new client's mysessions table, dump it in to
	 * the 'target' table (different for master and slave servers)
	 */
	err = dptab_addPublisher(dp->dt, target,
			dp->mysessions->key, dp->mysessions->keylen, src);
	if (err != dp_RES_OK) {
		DPRINT(("dp_addClient: can't addPublisher, err:%d\n", err));
		return err;
	}
	err = dp_subscribeExceptions(dp->dt, src);
	if (err != dp_RES_OK) {
		DPRINT(("dp_addClient: can't subscribeExceptions, err:%d\n", err));
		return err;
	}
	return err;
}

/*----------------------------------------------------------------------
 Select a session for host and tell them about it.
 On success, returns the selected session's karma in req->karma and notifies
  host of the selected session.
----------------------------------------------------------------------*/
static dp_result_t dp_selectSessionForHost
 (dp_t *dp, playerHdl_t h, dp_join_packet_t *req)
{
	int nRating;
	int nRatingSelect;
	dp_session_t sessSelect;
	char key[dptab_KEY_MAXLEN];
	int nsess;
	int keylen;
	int i;
	char pktbuf[dpio_MAXLEN_UNRELIABLE];
	struct {
		dp_packetType_t tag;
		dp_select_sess_packet_t body;
	} *pkt = (void *)pktbuf;
	sessSelect.currentPlayers = 0;
	sessSelect.karma = 0;

	if ((nsess = dptab_tableSize(dp->mysessions)) == 0)
		return dp_RES_EMPTY;
	if ((req->sessionType == 0) && (nsess > 1)) {
		DPRINT(("dp_selectSessionForHost: err: sessType 0 and nsess %d\n", nsess));
		return dp_RES_BAD;
	}

	DPRINT(("dp_selectSessionForHost: src h:%x, joinK %d, nsess %d\n", h,req->joinKarma, nsess));

	/* select the session with the highest rating */
	nRatingSelect = 0;
	for (i = 0; i < dp->sessionContexts->n_used; i++) {
		dp_result_t err;
		int nPlayers, nHosts;
		dp_session_t sess;
		dp_sessionContext_t *pSess;
		dp_sessionContext_t **ppS = (dp_sessionContext_t **)dynatab_subscript(
					dp->sessionContexts, i);
		assert(ppS);
		if (!ppS) break;
		pSess = *ppS;

		/* Check session type */
		if (req->sessionType && (pSess->sessionType != req->sessionType)) {
			/*DPRINT(("dp_selectSessionForHost: rejected due to different sessionType\n"));*/
			continue;
		}

		/* Check whether session full */
		nPlayers = dptab_tableSize(pSess->players);
		nHosts = dptab_tableSize(pSess->hosts);
		if (nHosts > nPlayers)
			nPlayers = nHosts;		/* Each host has a player eventually */

		/* Check session flags */
		err = dpGetSessionDescById(dp, pSess->sess_subkey, &sess, 0);
		if (err != dp_RES_OK) {
			DPRINT(("dp_selectSessionForHost: bug: can't get session record\n"));
			return dp_RES_BUG;
		}
		if (nPlayers >= sess.maxPlayers) {
			/*DPRINT(("dp_selectSessionForHost: rejected due to session being full\n"));*/
			continue;
		}
		if (((sess.flags ^ req->flags) & req->mask)) {
			/*DPRINT(("dp_selectSessionForHost: rejected; session flags %02xh, request flags %0x2h, request mask %02xh\n", sess.flags, req->flags, req->mask));*/
			continue;
		}
		/* check to make sure this session will not be rejected
		 * by dpHandleJoinSession()
		 */
		if (!(sess.flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS)) {
			/*DPRINT(("dp_selectSessionForHost: rejected due to disabled new players\n"));*/
			continue;
		}

		/* Rate the session for joinability:
		 * The rating is maxPlayers for an empty room, increases linearly to
		 * maxPlayers*2 for a half full room, then decreases linearly to
		 * zero for a full room.
		 * This favors rooms which are not too close to full or empty
		 * and favors large rooms over small ones.
		 */
		if (nPlayers * 2 < sess.maxPlayers)
			nRating = sess.maxPlayers + nPlayers * 2;
		else
			nRating = (sess.maxPlayers - nPlayers) * 4;
		if (nRating > nRatingSelect) {
			nRatingSelect = nRating;
			sessSelect = sess;
			sessSelect.currentPlayers = nPlayers;	/* odd - refresh current player count that will be sent to host */
		}
		DPRINT(("dp_selectSessionForHost: sess %s, rating %d, best rating %d\n", sess.sessionName, nRating, nRatingSelect));
	}
	if (nRatingSelect == 0) {
		DPRINT(("dp_selectSessionForHost: failed to pick session.\n"));
		return dp_RES_EMPTY;
	}
	DPRINT(("dp_selectSessionForHost: picked sess %s, karma %d\n", sessSelect.sessionName, sessSelect.karma));

	/* return selected session's karma in req->sessionKarma and notify host of
	 * the selected session */
	req->sessionKarma = sessSelect.karma;
	pkt->tag = dp_SELECTED_SESSION_PACKET_ID;
	if (bIAmStandaloneMaster(dp)) {    /* standalone master */
		key[0] = dp_KEY_MYSESSIONS;
		keylen = 1;
	} else  {                          /* game server */
		key[0] = dp_KEY_SESSIONS;
		if (req->sessionType == 0) {
			keylen = 1;
		} else {
			keylen = 3;
			key[1] = (char) dpGETSHORT_FIRSTBYTE(req->sessionType);
			key[2] = (char) dpGETSHORT_SECONDBYTE(req->sessionType);
		}
	}
	pkt->body.sess = sessSelect;
	pkt->body.keylen = keylen;
	memcpy(pkt->body.key, key, keylen);
	dpSwapSelectSessionPacket(pkt->body);
	return dpio_put_reliable(dp->dpio, &h, 1, pkt, sizeof(pkt->tag)+sizeof_dp_select_sess_packet_t(keylen), NULL);
}

/*-------------------------------------------------------------------------
 Someone (possibly ourselves) has requested that we let them in to
 one of our sessions so they can start creating players.
 If we don't feel like it, ignore the request.
 Otherwise, subscribe from their MYPLAYERS.sessionid onto our
 PLAYERS.sessionid and their groupid.sessionid's and publish them to them, then
 add a record for them into HOSTS.sessionid table and publish it to them.

 Request has already been byte-swapped to native (not network) byte order.
-------------------------------------------------------------------------*/
static dp_result_t
dpHandleJoinSession(
	dp_t *dp, playerHdl_t h, dp_join_packet_t *req, size_t reqlen)
{
	dp_host_t host;
	char key[dptab_KEY_MAXLEN];
	int keylen;
/*	char oadr[dptab_KEY_MAXLEN]; */
/*  int iadr2len; */
	dptab_table_t *players;
	dp_result_t err;
	char hostbuf[dpio_MAXLEN_RELIABLE];
	size_t hostlen;
	dp_sessionContext_t **spp;
	dp_sessionContext_t *sp = NULL;
	dp_session_t sess;
	int hops;

	precondition(dp != NULL);
	precondition(req != NULL);
	precondition((reqlen == dp_V1_JOIN_PACKET_LEN) ||
				(reqlen == dp_V2_JOIN_PACKET_LEN) ||
				(reqlen == dp_V3_JOIN_PACKET_LEN));

	DPRINT(("dpHandleJoinSession: got join request ... type %s\n",
		(reqlen == dp_V1_JOIN_PACKET_LEN) ? "V1" :
		(reqlen == dp_V2_JOIN_PACKET_LEN) ? "V2" :
		(reqlen == dp_V3_JOIN_PACKET_LEN) ? "V3" : "UNKNOWN!"));

	dp_assertValid(dp);
	assert((dp->hosts == NULL) ||
		(dptab_getTableContext(dp->hosts, dp_hosts_cb) != NULL));

	DPRINT(("dpHandleJoinSession: src h:%x, joinK %d, sessK %d, t:%d\n",
			h, req->joinKarma, req->sessionKarma, *(dp->dpio->now)));

	/* Check if sessionKarma is zero; if yes, select a session for host */
	if (!req->sessionKarma) {
		if ((reqlen == dp_V1_JOIN_PACKET_LEN) ||
			(dp_selectSessionForHost(dp, h, req) != dp_RES_OK) ||
			!req->sessionKarma /* note: updated by dp_selectSessionForHost */) {
			DPRINT(("dpHandleJoinSession: err: can't select session for host\n"));
			dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
								req->joinKarma, dp_RES_CLOSED);
			dp_assertValid(dp);
			return dp_RES_EMPTY;
		}

		/* if standalone master, subscribe host to our mysessions table;
		 * otherwise if game server, do as server would do */
		if (bIAmStandaloneMaster(dp)) { /* standalone */
			err = dptab_addPeer(dp->dt, h);
			if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
				DPRINT(("dpHandleJoinSession: can't addPeer, err:%d\n", err));
				dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
									req->joinKarma, dp_RES_CLOSED);
				dp_assertValid(dp);
				return err;
			}
			/* Send our mysessions table to the new client. */
			err = dptab_addSubscriber(dp->dt, dp->mysessions, h);
			if (err != dp_RES_OK) {
				DPRINT(("dpHandleJoinSession: can't addSubscriber, err:%d\n", err));
				dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
									req->joinKarma, dp_RES_CLOSED);
				dp_assertValid(dp);
				return err;
			}
		} else {        /* game server */
			/* kludge: check if we publish our sessions table to him;
			 * if not, then assume he hasn't logged on yet */
			char key[dptab_KEY_MAXLEN];
			err = dp_RES_OK;
			key[0] = dp_KEY_SESSIONS;
			if (req->sessionType == 0) {
				/* subscribe host to master session table */
				if (NULL == assoctab_subscript(dp->sessions->subscribers, h))
					err = dp_addClient(dp, h, key, 0);
			} else {
				/* subscribe host to session-specific table */
				dptab_table_t *tab;
				key[1] = dpGETSHORT_FIRSTBYTE(req->sessionType);
				key[2] = dpGETSHORT_SECONDBYTE(req->sessionType);
				if (((tab = dptab_getTable(dp->dt, key, 3)) == NULL) ||
					(NULL == assoctab_subscript(tab->subscribers, h)))
					err = dp_addClient(dp, h, &key[1], 2);
			}
			if (err != dp_RES_OK) {
				DPRINT(("dpHandleJoinSession: can't addClient, err:%d\n", err));
				dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
									req->joinKarma, dp_RES_CLOSED);
				dp_assertValid(dp);
				return err;
			}
		}
	}

	/* Locate the players table for the session in question. */
	keylen = 0;
	key[keylen++] = dp_KEY_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(req->sessionKarma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(req->sessionKarma);
	players = dptab_getTable(dp->dt, key, keylen);
	/*LBCONTEXT sp = (dp_sessionContext_t *)players->context;*/
	if (players) {
		sp = (dp_sessionContext_t *)dptab_getTableContext(players, dp_players_cb);
	}

	if (!players || !sp) {
		/* Actively reject the applicant - there's no session to join. */
		DPRINT(("dpHandleJoinSession: no such session (players %p, sp %p).  Sending nak.\n", players, sp));
		dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
				req->joinKarma, dp_RES_CLOSED);
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}

#if 1
	{
		char *sessbuf;
		size_t len;
		int nHappyLurkers;

		/* Verify that session is not full and allows new players.  Act as
		 * if lurkers take a slot, too. */
		err = dptab_get_bykey(dp->mysessions, sp->sess_subkey, sp->sess_subkeylen, (void **)&sessbuf, &len);
		if (err != dp_RES_OK)
		{
			DPRINT(("dpHandleJoinSession: bug: no session record?\n"));
			dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
					req->joinKarma, dp_RES_CLOSED);
			dp_assertValid(dp);
			return err;
		}
		/* Must unpack when retrieving records from dptab */
		dp_unpack_session(dp, sp->sess_subkey, sp->sess_subkeylen, sessbuf,len, &sess);
		if (!(sess.flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS)) {
			DPRINT(("dpHandleJoinSession: session not taking new players.  Sending nak.\n"));
			dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
					req->joinKarma, dp_RES_FULL);
			dp_assertValid(dp);
			return dp_RES_EMPTY;
		}
		nHappyLurkers = 0;
		if (sess.flags & dp_SESSION_FLAGS_ISSERVER)
			nHappyLurkers++;	/* server is in host table but not player table */
		if ((dptab_tableSize(players) >= sess.maxPlayers)   /* kosher check */
		|| (dptab_tableSize(sp->hosts) >= sess.maxPlayers+nHappyLurkers)) {	/* kludge check */
			/* Actively reject the applicant - there's no room in session. */
			DPRINT(("dpHandleJoinSession: session full (pl %d >= %d or ho %d >= %d).  Sending nak.\n",
				dptab_tableSize(players), sess.maxPlayers,
				dptab_tableSize(sp->hosts), sess.maxPlayers+nHappyLurkers));
			dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
					req->joinKarma, dp_RES_FULL);
			dp_assertValid(dp);
			return dp_RES_EMPTY;
		}
	}
#endif

	assert(sp->players == players);

	/* Subscribe host h to players, hosts, groups, and group players tables */
	if (h != PLAYER_ME) {
		if (bIAmGameServer(dp)) {
			/* Also add host h to the uid2sessid table */
			dp_uid_t uid = tserv_hdl2uid(dp->tserv, h);
			if (uid != dp_UID_NONE) {
				err = dp_sessid4uid(dp, uid, sess.reserved2, dp->dpio->myAdrLen+2, sess.sessionType);
				if (err != dp_RES_OK) {
					DPRINT(("dpHandleJoinSession: dp_sessid4uid(uid:%d, sesstype:%d) returns err:%d\n", uid, sess.sessionType, err));
				}
			} else {
				DPRINT(("dpHandleJoinSession: handle h:%x has no uid\n", h));
			}
		}

		err = dp_subscribe_client(dp, h, sp);
		if (err != dp_RES_OK) {
			DPRINT(("dpHandleJoinSession: bug: failed subscribing h:%x\n", h));
			dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
					req->joinKarma, dp_RES_CLOSED);
			dp_assertValid(dp);
			return err;
		}
	}

	/* Create a record for that host.  Set source handle of entry so
	 * table can be purged with dptab_delete_bySrc() when hosts disconnect.
	 * Note that record must be in network byte order at all times when
	 * in the table!
	 */
	memset(&host, 0, sizeof(host));
	/* Find first available non-reserved dpId for player */
	do {
		char subkey[dp_KEY_MAXLEN];
		size_t subkeylen = 2;
		char *buf;
		size_t buflen;

		host.firstId = (dpid_t) (dp_PLAYERS_PER_HOST * sp->hostid++);
		/* Wrap dpids around at dp_MAXDPIDS back to dp_FIRST_DPID
	 	 * We wouldn't want to hand out dp_ID_NONE or the reserved dpId block.
	 	 */
		if ((host.firstId + dp_PLAYERS_PER_HOST - 1 > dp_MAXDPIDS) ||
			(host.firstId < dp_FIRST_DPID)) {
			DPRINT(("dpHandleJoinSession: dpId wrapped around to %d\n", dp_FIRST_DPID));
			host.firstId = dp_FIRST_DPID;
			sp->hostid = dp_FIRST_DPID/dp_PLAYERS_PER_HOST;
			/* assert(!(dp_FIRST_DPID % dp_PLAYERS_PER_HOST)); */
		}
		subkey[0] = dpGETSHORT_FIRSTBYTE(host.firstId);
		subkey[1] = dpGETSHORT_SECONDBYTE(host.firstId);

		err = dptab_get_bykey(sp->hosts, subkey, subkeylen, (void **)&buf, &buflen);
		if (err != dp_RES_OK && err != dp_RES_EMPTY) {
			/* FIXME: Very bad, but we should still send an error packet. */
			DPRINT(("dpHandleJoinSession: bug: dptab_get_bykey(sp->hosts, id:%d) failed with err:%d\n", err));
			return dp_RES_BUG;
		}
	} while (err != dp_RES_EMPTY);
	host.joinKarma = req->joinKarma;
	memcpy(host.iadr, req->iadr, dp_MAX_ADR_LEN);

	DPRINT(("dpHandleJoinSession: iadr "));
	printAdr(dp->dpio->myAdrLen, host.iadr);

	/*
	 * If both addresses were supplied in the join request then use
	 * the two addresses as they were supplied
	 */
	if (reqlen >= dp_V3_JOIN_PACKET_LEN) {
		memcpy(host.iadr2, req->iadr2, dp_MAX_ADR_LEN);
	} else {
		/*
		 * Receiving a request from an old client
		 *
		 * Set the second address to be identical to the first
		 */
		memcpy(host.iadr2, host.iadr, dp_MAX_ADR_LEN);
	}

	DPRINT((", iadr2 "));
	printAdr(dp->dpio->myAdrLen, host.iadr2);
	DPRINT((" \n"));

	keylen = 0;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(host.firstId);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(host.firstId);
	hostlen = dp_pack_host(dp->dpio->myAdrLen, &host, hostbuf);

	/* Note: dptab_set will decrement hops if h != ME; that's fine
	 * for tables we subscribe to, but we're synthesizing the records
	 * here, and we want the hop counts the same!  This hops thing is
	 * more trouble than it's worth... it was meant to aid in doing a
	 * distributed server, but we haven't used it yet.
	 * Having hops disagree between elements of same table causes dptab
	 * to flush too often...
	 */
	hops = 4;
	if (h != PLAYER_ME)
		hops++;

	err = dptab_set(dp->dt, sp->hosts, key, keylen, hostbuf, hostlen, hops, h);
	if (err != dp_RES_OK) {
		DPRINT(("dpHandleJoinSession: bug: can't add h:%x to host table for session, err:%d\n", h, err));
		dpSendErrorPacket(dp, h, dp_JOIN_PACKET_ID,
				req->joinKarma, dp_RES_CLOSED);
		dp_assertValid(dp);
		return err;
	}
	DPRINT(("dpHandleJoinSession: h:%x added to host table for session.\n", h));

	/* Remember what session the fellow is in.
	 * We happen to know that player table context is a dp_sessionContext_t *
	 */
	spp = (dp_sessionContext_t **) assoctab_subscript_grow(dp->hdl2sess, h);
	if (!spp) {
		/* FIXME: Very bad, but we should still send an error packet. */
		DPRINT(("dpHandleJoinSession: can't save handle's session\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	*spp = (dp_sessionContext_t *)dptab_getTableContext(players,dp_players_cb);
	if (((*spp)->players != players)
	|| ((*spp)->dp != dp)) {
		DPRINT(("dpHandleJoinSession: bug: players table context bad?\n"));
	}
	/* Cast-off sessions don't have a hosts callback anymore */
	sp = (dp_sessionContext_t *)dptab_getTableContext(sp->hosts,dp_hosts_cb);
	if (sp && (sp != *spp)) {
		DPRINT(("dpHandleJoinSession: bug: hosts' context %p != players' context %p?\n", sp, *spp));
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Request that the game server tell the host of the given session to open a
 handle to us.
 The 3rd argument is TRUE to join the session, or FALSE to just
 open a connection to enumerate players.
--------------------------------------------------------------------------*/
static dp_result_t dpSendIndirectJoin(dp_t *dp, dp_session_t *session, int joining)
{
	struct {
		dp_packetType_t				tag;
		dp_indirect_join_packet_t	body;
	} PACK pkt;
	dp_result_t res;
	int	len;

	/*
	 * Compose packet
	 */
	memset(&pkt, 0, sizeof(pkt));
	pkt.tag = dp_INDIRECT_JOIN_PACKET_ID;
	pkt.body.chunk1 = joining ? dp_INDIRECT_JOIN_CHUNK_SESSIONID : dp_INDIRECT_JOIN_CHUNK_SESSIONID_OPENONLY;
	res = dpGetSessionId(dp, session, (char *) &pkt.body.sessionId, &len);

	if (res != dp_RES_OK) {
		DPRINT(("dpSendIndirectJoin: can't convert session into sessionId\n"));
		return res;
	}
	DPRINT(("dpSendIndirectJoin: sending INDIRECT_JOIN(%s) to server h:%x\n",
		key2a(pkt.body.sessionId, len), dp->hGameServer));

	/*
	 * Send to Game Server
	 */
	res = dpio_put_reliable(dp->dpio, &dp->hGameServer, 1, &pkt, sizeof(pkt), NULL);

	if (res != dp_RES_OK) {
		DPRINT(("dpSendIndirectJoin: can't send INDIRECT_JOIN to server\n"));
		return (res);
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Request that the given host let us into the given session so we can
 start creating players.
 The first karma identifies the act of joining this host to this session;
 the second karma identifies the session.
 If second karma is zero, select_sessionType, select_flags and select_mask
  specify arguments for the master so he can select a session for us.
 If the session supports dual-addresses then send them both of our addresses
 in the extended join session packet.  Otherwise rever to to old behaviour.
--------------------------------------------------------------------------*/
static dp_result_t dpSendJoinSession(dp_t *dp, dp_karma_t joinKarma, dp_session_t *session, dp_species_t select_sessionType, short select_flags, short select_mask, playerHdl_t h)
{
	struct {
		dp_packetType_t   tag;
		dp_join_packet_t body;
	} PACK pkt;
	int bodylen;

	precondition(dp != NULL);
	condition(dp->dpio != NULL, "corrupted dp");
	dp_assertValid(dp);

	DPRINT(("dpSendJoinSession: joinKarma %d, sel_ty %d, sel_fl %x, sel_mk %x, h:%x, ",
		joinKarma, select_sessionType, select_flags, select_mask, h));
	if (session) {
		dprint_sess(dp, session, "session");
	} else  {
		DPRINT(("session: NULL\n"));
	}
	memset(&pkt, 0, sizeof(pkt));
	pkt.tag = dp_JOIN_PACKET_ID;
	pkt.body.joinKarma = joinKarma;
	pkt.body.sessionKarma = session ? session->karma : 0;
	memcpy(pkt.body.iadr, dp->dpio->myAdr, dp->dpio->myAdrLen);

	/*
	 * If the host supports dual addresses then we send the new
	 * join request packet
	 */
	if (select_flags & dp_SESSION_FLAGS_ENABLE_DUALADDRESS) {
		DPRINT(("dpSendJoinSession: host supports DUAL ADDRESSES\n"));
		DPRINT(("myadr: "));
		printAdr(dp->dpio->myAdrLen, dp->dpio->myAdr);
		DPRINT((", myadr2: "));
		printAdr(dp->dpio->myAdrLen, dp->dpio->myAdr2);
		DPRINT(("\n"));

		pkt.body.sessionType = select_sessionType;
		pkt.body.flags = select_flags;
		pkt.body.mask = select_mask;
		memcpy(pkt.body.iadr2, dp->dpio->myAdr2, dp->dpio->myAdrLen);
		bodylen = sizeof(pkt.body);
	} else {
		DPRINT(("dpSendJoinSession: no support for DUAL ADDRESSES\n"));

		/*
		 * If the host doesn't support dual address then we send the
		 * old join request packet whose structure is determined by
		 * sessionKarma being equal to zero
		 */
		if (!session) {
			pkt.body.sessionType = select_sessionType;
			pkt.body.flags = select_flags;
			pkt.body.mask = select_mask;
			bodylen = dp_V2_JOIN_PACKET_LEN;
		} else {
  			bodylen = dp_V1_JOIN_PACKET_LEN;
		}
	}

	/* If local request, don't queue or swap it, execute it immediately */
	if (h == PLAYER_ME) {
		dp_assertValid(dp);
		return dpHandleJoinSession(dp, h, &pkt.body, bodylen);
	}

	dpSwapJoinPacketT(&pkt.body, bodylen);
	dp_assertValid(dp);

	/* If we are connected to a game server and this packet request is not
	 * going to a game server then send an INDIRECT_JOIN_PACKET to the game
	 * server.
	 *
	 * This is used for game server tracking and allows join hosts who are
	 * behind firewalls since the game server will request that the host open
	 * a connection for you.
	 *
	 * All of this only applies in the case where a specific session has been
	 * specified.
	 */
	if ((dp->hGameServer != PLAYER_NONE) && (dp->hGameServer != h)) {
		dp_result_t res = dpSendIndirectJoin(dp, session, TRUE);
		if (res != dp_RES_OK) {
			DPRINT(("dpSendJoinSession: Could not send INDIRECT_JOIN to game server\n"));
			return (res);
		}
	}

	/*
	 * Send join request to session host
	 */
	return dpio_put_reliable(dp->dpio, &h, 1, &pkt, sizeof(pkt.tag)+bodylen, NULL);
}

/*-------------------------------------------------------------------------
 Someone (possibly ourselves) has requested that we remove them and all
 their players from one of our sessions.

 Request has already been byte-swapped to native (not network) byte order.
-------------------------------------------------------------------------*/
static dp_result_t dpHandleLeaveSession(dp_t *dp, playerHdl_t h, dp_leave_packet_t *req, size_t reqlen)
{
	int i;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	dptab_table_t *hosts;
	dptab_table_t *players;
	dptab_table_t *groups;
	dptab_table_t *grplayers[dp_MAX_GROUPS];

	precondition(dp != NULL);
	dp_assertValid(dp);

	DPRINT(("dpHandleLeaveSession: src h:%x, joinK %d, sessK %d, t:%d\n",
			h, req->joinKarma, req->sessionKarma, *(dp->dpio->now)));
	if (reqlen != dp_LEAVE_PACKET_LEN) {
		DPRINT(("dpHandleLeaveSession: request %d bytes, expected %d\n",
				reqlen, dp_LEAVE_PACKET_LEN));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}

	/* Locate the players table for the session in question. */
	keylen = 0;
	key[keylen++] = dp_KEY_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(req->sessionKarma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(req->sessionKarma);
	dp_assertValid(dp);
	players = dptab_getTable(dp->dt, key, keylen);
	if (!players) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}

	/* Locate the host table for the session in question. */
	key[0] = dp_KEY_HOSTS;
	hosts = dptab_getTable(dp->dt, key, keylen);
	if (!hosts) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}

	/* Locate the groups table for the session in question. */
	key[0] = dp_KEY_GROUPS;
	groups = dptab_getTable(dp->dt, key, keylen);
	if (!groups) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}

	/* Locate the group player tables for the session in question. */
	keylen = 5;
	key[0] = dp_KEY_GROUP_PLAYERS;
	for (i = 0; i < dp_MAX_GROUPS; i++) {
		dpid_t gid = (dpid_t) (dp->firstGId + i);
		key[3] = (char) dpGETSHORT_FIRSTBYTE(gid);
		key[4] = (char) dpGETSHORT_SECONDBYTE(gid);
		grplayers[i] = dptab_getTable(dp->dt, key, keylen);
		if (!grplayers[i]) {
			dp_assertValid(dp);
			return dp_RES_EMPTY;
		}
	}

	/* Delete host h's right to publish myplayers table into players table
	 * and group myplayers table into players table */
	keylen = 3;
	key[0] = dp_KEY_MYPLAYERS;
	dptab_deletePublisher(dp->dt, key, keylen, h);
	keylen = 5;
	key[0] = dp_KEY_GROUP_MYPLAYERS;
	for (i = dp->firstGId; i < dp->firstGId + dp_MAX_GROUPS; i++) {
		key[3] = (char) dpGETSHORT_FIRSTBYTE(i);
		key[4] = (char) dpGETSHORT_SECONDBYTE(i);
		dptab_deletePublisher(dp->dt, key, keylen, h);
	}

	/* Delete host h's subscriptions to players, hosts, groups, and group
	 * players tables */
	dptab_deleteSubscriber(dp->dt, players, h);
	dptab_deleteSubscriber(dp->dt, hosts, h);
	dptab_deleteSubscriber(dp->dt, groups, h);
	for (i = 0; i < dp_MAX_GROUPS; i++) {
		dptab_deleteSubscriber(dp->dt, grplayers[i], h);
	}

	/* Delete host h from players, hosts, groups, and group players tables */
	if (h != PLAYER_ME) {	/* dpClose takes care of this if local */
		dptab_delete_bySrc(dp->dt, players, h);
		dptab_delete_bySrc(dp->dt, hosts, h);
		dptab_delete_bySrc(dp->dt, groups, h);
		for (i = 0; i < dp_MAX_GROUPS; i++) {
			dptab_delete_bySrc(dp->dt, grplayers[i], h);
		}
	}

	/* If we are a standalone master,
	 * delete host h's subscriptions to our mysessions table;
	 * if game server, deletePeer called when hdl closes */
	if (bIAmStandaloneMaster(dp) && (h != PLAYER_ME))
		dptab_deleteSubscriber(dp->dt, dp->mysessions, h);

	/* Hmm.  Leave keepalive on so we can delete him if he crashes out. */
	/*dpio_setHandleOptions(dp->dpio, h, 0);*/

	/* Should we delete him as peer?  Naah - wait until dpio_closeHdl's
	 * callback tells us he's really gone.  But there may be a bunch
	 * of xfers queued up for him still; guess dp_update() will send them
	 * needlessly...
	 */
	DPRINT(("dpHandleLeaveSession: h:%x deleted from host and player tables for session.\n", h));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Request that the given host remove this host and all its players from
 the given session.
 The first karma identifies the act of joining this host to this session;
 the second karma identifies the session.
--------------------------------------------------------------------------*/
static dp_result_t dpSendLeaveSession
 (dp_t *dp, dp_karma_t joinKarma, dp_karma_t sessionKarma, playerHdl_t h)
{
	struct {
		dp_packetType_t   tag;
		dp_join_packet_t body;
	} PACK pkt;

	dp_assertValid(dp);
	memset(&pkt, 0, sizeof(pkt));
	pkt.tag = dp_LEAVE_PACKET_ID;
	pkt.body.joinKarma = joinKarma;
	pkt.body.sessionKarma = sessionKarma;
	memcpy(pkt.body.iadr, dp->dpio->myAdr, dp->dpio->myAdrLen);

	/* If local request, don't queue or swap it, execute it immediately */
	if (h == PLAYER_ME) {
		dp_assertValid(dp);
		return dpHandleLeaveSession(dp, h, &pkt.body, dp_LEAVE_PACKET_LEN);
	}

	dpSwapLeavePacketT(&pkt.body, dp_LEAVE_PACKET_LEN);
	dp_assertValid(dp);
	return dpio_put_reliable(dp->dpio, &h, 1, &pkt, sizeof(pkt.tag)+dp_LEAVE_PACKET_LEN, NULL);
}

/*-------------------------------------------------------------------------
 Handle the session selected for us by the game server/master.
--------------------------------------------------------------------------*/
static dp_result_t dpHandleSelectedSession
 (dp_t *dp, playerHdl_t h, dp_select_sess_packet_t *s, size_t slen)
{
	int err;
	dp_session_t sess;
	precondition(s != NULL);

	/* Sanity checks */
	if (dp->players != NULL) {
		DPRINT(("dpHandleSelectedSession: can't open if session already open (players)\n"));
		dp_assertValid(dp);
		return dp_RES_BUSY;
	}
	if (dp->hosts != NULL) {
		DPRINT(("dpHandleSelectedSession: can't open if session already open (hosts)\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dp->groups != NULL) {
		DPRINT(("dpHandleSelectedSession: can't open if session already open (hosts)\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if ((s->sess.maxPlayers < 1) || (s->sess.maxPlayers > dp_MAXREALPLAYERS)) {
		DPRINT(("dpHandleSelectedSession: bad maxPlayers %d\n", s->sess.maxPlayers));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	dprint_sess(dp, &s->sess, "dpHandleSelectedSession");

	/* Add it to the dptab system if hGameServer is not set */
	if (dp->hGameServer == PLAYER_NONE) {
		/* Let him send us dptab stuff in general */
		err = dptab_addPeer(dp->dt, h);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("dpHandleSelectedSession: can't add h:%x to peer table, err:%d\n", h, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}
		/* In particular, let him send us the table that contained the
		 * session record, and route it to our sessions table.
		 */
		err = dptab_addPublisher(dp->dt, dp->sessions, s->key, s->keylen,h);
		if (err != dp_RES_OK) {
			DPRINT(("dpHandleSelectedSession: can't add h:%x as session table peer, err:%d\n", h, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}
		dp->select_keylen = s->keylen;
		memcpy(dp->select_key, s->key, s->keylen);
	}
	err = initOpenSession(dp, &s->sess, &sess);
	if (err != dp_RES_OK) {
		dpClose(dp);
		dp_assertValid(dp);
		return err;
	}
	dprint_sess(dp, &sess, "dpHandleSelectedSession saving");
	dp->election_old_session = sess;
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Check if a session with the given karma already exists.
-------------------------------------------------------------------------*/
static dp_sessionContext_t *findSessionWithKarma(dp_t *dp, dp_karma_t karma)
{
	int i;

	for (i = 0; i < dp->sessionContexts->n_used; i++) {
		dp_sessionContext_t *pSess;
		dp_sessionContext_t **ppS = (dp_sessionContext_t **)dynatab_subscript(
					dp->sessionContexts, i);
		assert(ppS);
		if (!ppS) break;
		pSess = *ppS;
		if (pSess->sessionKarma == karma) {
			DPRINT(("findSessionWithKarma: found session %p\n", pSess));
			return pSess;
		}
	}
	DPRINT(("findSessionWithKarma: no matching session found\n"));
	return NULL;
}

/*-------------------------------------------------------------------------
 Set up session record sess using info from enum'd or selected session s and
  create player, group, etc. tables for the session.
 Used whether creating or joining a specific session or joining any session.
-------------------------------------------------------------------------*/
static dp_result_t initOpenSession(dp_t *dp, dp_session_t *s, dp_session_t *sess)
{
	int i;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	int subkeylen;
	dp_result_t err;
	dp_sessionContext_t *pSess;	/* a handle to the session in memory */
	dp_sessionContext_t **ppSC;

	/* Set up session record 'sess'. */
	if (s->flags & dp_SESSION_FLAGS_CREATESESSION) {
		memset(sess, 0, sizeof(*sess));
		sess->sessionType = s->sessionType;
		sess->maxPlayers = s->maxPlayers;
		sess->dwUser1 = s->dwUser1;
		memcpy(sess->sessionName, s->sessionName, dp_SNAMELEN);
		memcpy(sess->szPassword, s->szPassword, dp_PASSWORDLEN);
		memcpy(sess->szUserField, s->szUserField, dp_USERFIELDLEN);
		/* Special for server-server session propagation:
		 * put the address of the current game server in the session packet!
		 * This is a bit odd - for this to work, you shouldn't be able
		 * to change game servers while a session is open.
		 */
		memcpy(&sess->dwReserved1, dp->adrGameServer, sizeof(sess->dwReserved1));

		memcpy(sess->adrMaster, dp->dpio->myAdr, dp->dpio->myAdrLen);
		do {
			sess->karma = (dp_karma_t) ((eclock() + rand()) & 0xffff);
		} while ((sess->karma == 0) || findSessionWithKarma(dp, sess->karma));

		/* Subkey is the original host address and karma, Intel byte order. */
		subkeylen = dp->dpio->myAdrLen;
		memcpy(sess->reserved2, sess->adrMaster, subkeylen);
		sess->reserved2[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(sess->karma);
		sess->reserved2[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(sess->karma);
	} else {
		*sess = *s;

		DPRINT(("initOpenSession: attempt to join remote session (curpl %d, maxpl %d)\n",
			sess->currentPlayers, sess->maxPlayers));
		if (!(sess->flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS)) {
			DPRINT(("initOpenSession: session not taking new players\n"));
			dp_assertValid(dp);
			return dp_RES_FULL;
		}
		/* Enforce maxplayers limit on client side */
		if (sess->currentPlayers >= sess->maxPlayers) {
			DPRINT(("initOpenSession: attempt to join full remote session (%d >= %d)\n",
				sess->currentPlayers, sess->maxPlayers));
			dp_assertValid(dp);
			return dp_RES_FULL;
		}
		/* Clear flags, except for user-modifiable flags */
		DPRINT(("initOpenSession: Pre-clear flags=%x\n", sess->flags));
		sess->flags = (short) (sess->flags &
			(dp_SESSION_FLAGS_USE_BROADCAST| dp_SESSION_FLAGS_MIGRATE_FORBIDDEN|
			dp_SESSION_FLAGS_ISLOBBY | dp_SESSION_FLAGS_ENABLE_NEWPLAYERS |
			dp_SESSION_FLAGS_ENABLE_PLAYERVARS));
		DPRINT(("initOpenSession: Post-clear flags=%x\n", sess->flags));
	}
	/* Done with 's' argument; refer to local variable 'sess' from here on,
	 * except when we need his original 'flags' value.
	 */
#if 0
	DPRINT(("initOpenSession: sessType %d, name %s, adr \n",
			sess->sessionType, sess->sessionName));
	printAdr(dp->dpio->myAdrLen, sess->adrMaster);
	DPRINT(("\n"));
#endif

	dp->dpid2commHdl = assoctab_create(sizeof(playerHdl_t));
	if (!dp->dpid2commHdl) {
		dp_assertValid(dp);
		return dp_RES_NOMEM;
	}

	commHdl4dpid(dp, dp_ID_BROADCAST, PLAYER_BROADCAST);

	/* Set up key to session in sessions table (and mysessions, if master) */
	/* Subkey is the original host address and karma, Intel byte order. */
	subkeylen = dp->dpio->myAdrLen+2;
	memcpy(dp->sess_subkey, sess->reserved2, subkeylen);
	dp->sess_subkeylen = subkeylen;
	dp->sess_karma = sess->karma;	/* for quick access :-( */

	dprint_sess(dp, sess, "initOpenSession");

	/* If master, save session record in 'mysessions'.
	 * If slave, contact master.
	 */
	if (s->flags & dp_SESSION_FLAGS_CREATESESSION) {
		dp->hMaster = PLAYER_ME;
#ifdef USE_dp_enableNewPlayers
		dpEnableNewPlayers(dp, TRUE);
#endif
		sess->flags |= dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
		if (s->flags & dp_SESSION_FLAGS_ENABLE_PLAYERVARS)
			sess->flags |= dp_SESSION_FLAGS_ENABLE_PLAYERVARS;
		if (s->flags & dp_SESSION_FLAGS_ISLOBBY)
			sess->flags |= dp_SESSION_FLAGS_ISLOBBY;

		/*
		 * All new versions suport dualaddress so hard code it
		 */
		sess->flags |= dp_SESSION_FLAGS_ENABLE_DUALADDRESS;

		/*
		 * Kludge! to ensure that DUAL address support is used when there
		 * is an attempt to create a player in the session they just created
		 */
		s->flags |= dp_SESSION_FLAGS_ENABLE_DUALADDRESS;

		err = dp_setSessionDesc(dp, sess, 0);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: can't create session record, err:%d\n", err));
			dp_assertValid(dp);
			return err;
		}

		/* If a game server is listening to our mysession table,
		 * or if we're broadcasting it periodically on a LAN,
		 * people will be able to find our game now.
		 */
	} else {

		/* Join an existing session.  This means subscribing to its
		 * player table.
		 * Open a comm handle to the host's address.  Hmm - these
		 * will need to be reference counted somehow...
		 */
		dp->hMaster = dpio_openHdl(dp->dpio, sess->adrMaster, NULL, NULL);
		if ((dp->hMaster == PLAYER_NONE)
		||  (dp->hMaster == PLAYER_ME)) {
			DPRINT(("initOpenSession: bad handle h:%x to host\n", dp->hMaster));
			dpClose(dp);
			return dp_RES_HOST_NOT_RESPONDING;
		}
		DPRINT(("initOpenSession: opened handle to host, h:%x\n", dp->hMaster));
		/* Add it to the dptab system */
		err = dptab_addPeer(dp->dt, dp->hMaster);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("initOpenSession: can't add h:%x to peer table, err:%d\n", dp->hMaster, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}
	}

	/* Calculate key for the session's player table.  Note:
	 * this key is the same on client and host, and is
	 * a constant byte PLAYERS followed by the
	 * karma assigned by the original master.  (Can't use the
	 * original host's address unless we somehow add it to the
	 * session record.)
	 */
	keylen = 0;
	key[keylen++] = dp_KEY_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(sess->karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(sess->karma);

	/* Create session context, for use as various tables' callback context */
	DPRINT(("initOpenSession: Creating new session context.\n"));
	dumpSessionContexts(dp);
	pSess = dp_MALLOC(sizeof(dp_sessionContext_t));
	memset(pSess, 0, sizeof(dp_sessionContext_t));
	dp->pSess = pSess;
	ppSC = dynatab_subscript_grow(dp->sessionContexts, dynatab_size(dp->sessionContexts));
	if ((pSess == NULL) || (ppSC == NULL)) {
		DPRINT(("initOpenSession: Could not allocate session context\n", err));
		dpClose(dp);
		dp_assertValid(dp);
		return dp_RES_NOMEM;
	}
	*ppSC = pSess;
	pSess->dp = dp;
	pSess->sess_subkeylen = dp->sess_subkeylen;
	memcpy(pSess->sess_subkey, dp->sess_subkey, dp->sess_subkeylen);
	pSess->sessionKarma = sess->karma;
	pSess->sessionType = sess->sessionType;
	/* Save ids less than dp_FIRST_DPID for internal use */
#if 1
	pSess->hostid = dp_FIRST_DPID/dp_PLAYERS_PER_HOST;
#else
	/* start them out 2 players from max to check wrap around behavior */
	pSess->hostid = dp_MAXDPIDS/dp_PLAYERS_PER_HOST - 2;
#endif

	/* Create player table.  Only difference between client and host
	 * is who's writing into it and who's reading from it, and how.
	 * Data inside it, and the key of the table, are always the same.
	 */
	err = dptab_createTable(dp->dt, &pSess->players, key, keylen, 0, NULL, NULL, dp_players_cb, pSess);
	if (err != dp_RES_OK && err != dp_RES_ALREADY) {
		DPRINT(("initOpenSession: err %d creating players table\n", err));
		dpClose(dp);
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (err == dp_RES_ALREADY) {
		DPRINT(("initOpenSession: players already existed...reset callback and context\n"));
		pSess->players = dptab_getTable(dp->dt, key, keylen);
		dptab_setTableCallback(pSess->players, dp_players_cb, pSess);
	}
	dp->players = pSess->players;

	/* Publicise this session (if master) or subscribe to it (if slave) */
	if (s->flags & dp_SESSION_FLAGS_CREATESESSION) {
		/* We can't add subscribers yet - we don't know who they are! */
		;
	} else {
		/* Allow host to send player table to us */
		err = dptab_addPublisher(dp->dt, dp->players, key, keylen, dp->hMaster);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: can't add publisher h:%x for player table, err:%d\n",
					dp->hMaster, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}
	}

	/* Create myplayer table.   Published to host from each client.
	 */
	key[0] = dp_KEY_MYPLAYERS;
	err = dptab_createTable(dp->dt, &dp->myplayers, key, keylen, 0, NULL, NULL, dp_myplayers_cb, pSess);
	if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
		DPRINT(("initOpenSession: err %d creating myplayers table\n", err));
		dpClose(dp);
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (err == dp_RES_ALREADY) {
		DPRINT(("initOpenSession: myplayers already existed...\n"));
		dp->myplayers = dptab_getTable(dp->dt, key, keylen);
		assert(dp->myplayers != NULL);
		err = dptab_setTableCallback(dp->myplayers, dp_myplayers_cb, pSess);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: setting myplayers callback, err:%d\n", err));
			return err;
		}
	}
	pSess->myplayers = dp->myplayers;

	DPRINT(("initOpenSession: myplayers has key %s\n",
			key2a(dp->myplayers->key, dp->myplayers->keylen)));

	/* Create the user table.  Like player table, but holds
	 * one record per machine in the session, no matter if
	 * the machine in question has 0 or 10 players.
	 */
	key[0] = dp_KEY_HOSTS;
	err = dptab_createTable(dp->dt, &pSess->hosts, key, keylen, 0, NULL, NULL, dp_hosts_cb, pSess);
	if (err != dp_RES_OK) {
		DPRINT(("initOpenSession: err %d creating hosts table\n", err));
		dpClose(dp);
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	dp->hosts = pSess->hosts;
	DPRINT(("initOpenSession: after hosts created, myplayers has key %s\n",
			key2a(dp->myplayers->key, dp->myplayers->keylen)));
	/* Publicise this session (if master) or subscribe to it (if slave) */
	if (s->flags & dp_SESSION_FLAGS_CREATESESSION) {
		/* We can't add subscribers yet - we don't know who they are! */
		;
	} else {
		/* Allow host to send player table to us */
		err = dptab_addPublisher(dp->dt, dp->hosts, key, keylen, dp->hMaster);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: can't add publisher h:%x for user table, err:%d\n",
					dp->hMaster, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}
	}

	/* Create player variable table.   Published to all in session from each
	 * client. Add subscribers during dp_hosts_cb. */
	if (s->flags & dp_SESSION_FLAGS_ENABLE_PLAYERVARS) {
		key[0] = dp_KEY_PLAYER_VARIABLES;
		err = dptab_createTable(dp->dt, &pSess->playervars, key, keylen, 0, NULL, NULL, dp_playervars_cb, pSess);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: err %d creating player variable table\n", err));
			dpClose(dp);
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
	} else {
		pSess->playervars = NULL;
	}

	/* Create group table.  Only difference between client and host
	 * is who's writing into it and who's reading from it, and how.
	 * Data inside it, and the key of the table, are always the same.
	 */
	key[0] = dp_KEY_GROUPS;
	err = dptab_createTable(dp->dt, &pSess->groups, key, keylen, 0, NULL, NULL, dp_groups_cb, pSess);
	if (err != dp_RES_OK) {
		DPRINT(("initOpenSession: err %d creating groups table\n", err));
		dpClose(dp);
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	dp->groups = pSess->groups;

	/* Publicise this session (if master) or subscribe to it (if slave) */
	if (s->flags & dp_SESSION_FLAGS_CREATESESSION) {
		/* We can't add subscribers yet - we don't know who they are! */
		;
	} else {
		/* Allow host to send group table to us */
		err = dptab_addPublisher(dp->dt, dp->groups, key, keylen, dp->hMaster);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: can't add publisher h:%x for group table, err:%d\n",
					dp->hMaster, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}
	}

	/* Create group player tables.  Only difference between client and host
	 * is who's writing into it and who's reading from it, and how.
	 * Data inside it, and the key of the table, are always the same.
	 */
	keylen = 5;
	key[0] = dp_KEY_GROUP_PLAYERS;
	for (i = dp->firstGId; i < dp->firstGId + dp_MAX_GROUPS; i++) {
		dptab_table_t *grplayers;
		key[3] = (char) dpGETSHORT_FIRSTBYTE(i);
		key[4] = (char) dpGETSHORT_SECONDBYTE(i);
		err = dptab_createTable(dp->dt, &grplayers, key, keylen, 0, NULL, NULL, dp_group_players_cb, pSess);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: err %d creating group players table\n", err));
			dpClose(dp);
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
		pSess->grplayers[i-dp->firstGId] = grplayers;

		/* Publicise this session (if master) or subscribe to it (if slave) */
		if (s->flags & dp_SESSION_FLAGS_CREATESESSION) {
			/* We can't add subscribers yet - we don't know who they are! */
			;
		} else {
			/* Allow host to send group players table to us */
			err = dptab_addPublisher(dp->dt, grplayers, key, keylen, dp->hMaster);
			if (err != dp_RES_OK) {
				DPRINT(("initOpenSession: can't add publisher h:%x for group %d players table, err:%d\n",
						dp->hMaster, i, err));
				dpClose(dp);
				dp_assertValid(dp);
				return err;
			}
		}
	}

	/* Create group myplayer tables.   Published to host from each client.
	 */
	key[0] = dp_KEY_GROUP_MYPLAYERS;
	for (i = dp->firstGId; i < dp->firstGId + dp_MAX_GROUPS; i++) {
		dptab_table_t *grplayers;
		key[3] = (char) dpGETSHORT_FIRSTBYTE(i);
		key[4] = (char) dpGETSHORT_SECONDBYTE(i);
		err = dptab_createTable(dp->dt, &grplayers, key, keylen, 0, NULL, NULL, NULL, NULL);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: err %d creating group myplayers table\n", err));
			dpClose(dp);
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
		pSess->grmyplayers[i-dp->firstGId] = grplayers;
	}
	/* If we're a slave, give the host a subscription to our myplayer tables */
	if (!(s->flags & dp_SESSION_FLAGS_CREATESESSION)) {
		err = dp_subscribe_host(dp, dp->hMaster, pSess);
		if (err != dp_RES_OK) {
			DPRINT(("initOpenSession: can't add subscriber h:%x, err:%d\n",
					dp->hMaster, err));
			dpClose(dp);
			dp_assertValid(dp);
			return err;
		}

#ifdef SEMIRELIABLE_SESSIONS
		/* Battlezone relies on session records being sent reliably, which is a
		 * no-no.  Attempt to simulate this in the case when dpOpen unsubscribes
		 * us from the game server's session table.
		 */
		if ((dp->hGameServer != PLAYER_NONE)
		&&  !(s->flags & dp_SESSION_FLAGS_ISLOBBY)) {
			key[0] = dp_KEY_MYSESSIONS;
			keylen = 1;
			err = dptab_addPublisher(dp->dt, dp->sessions, key, keylen, dp->hMaster);
			if (err != dp_RES_OK) {
				DPRINT(("initOpenSession: can't add h:%x as publisher?, err:%d\n", dp->hMaster, err));
				dp_assertValid(dp);
				return err;
			}
		}
#endif
	}

	DPRINT(("initOpenSession: session context created.\n"));
	dumpSessionContexts(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Since dpOpen_cb is used as a flag to check if an open is in progress and
 dpOpen does not require a callback, we need a non-null value to use if no
 callback is given.
------------------------------------------------------------------------*/
static int dp_PASCAL dpOpen_dummy_cb(dp_session_t *sDesc, long *timeout, long flags, void *context)
{
	/*
	(dp_session_t *)sDesc;
	(long *)timeout;
	(long)flags;
	(void *)context;
	*/
	return FALSE;
}

/*------------------------------------------------------------------------
 Start a new session, or join an existing one using one of three different ways.
 If starting a new session,
    include dp_SESSION_FLAGS_CREATESESSION in flags argument for s.
 If joining, do one of the following three:
 1. To join a specific session (gotten for instance using dpEnumSessions),
    pass that session in s.
 2. To join any lobby session on a gameserver set by a previous call to
 dpSetGameserver(),
 	set s to NULL.
 3. To join any session on a specific machine, set:
    s->adrMaster with the machine's address including port number (this
	 must be in the format returned by dpResolveHostname)
	s->sessionType with the game's session type
	s->flags with appropriate flags argument (currently, only the
	 dp_SESSION_FLAGS_ISLOBBY bit is used; other bits are masked off).
 Returns immediately.
 dpReceive will call the callback when operation completes, and will
 indicate success with a non-zero session description pointer.
------------------------------------------------------------------------*/
DP_API dp_result_t dpOpen(
	dp_t *dp,
	dp_session_t *s,
	dpEnumSessionsCallback_t cb,
	void *context)
{
	dp_result_t err;
	dp_session_t sess;
	int bJoinAny;

	precondition(dp);
	precondition(dp->dpio);
	precondition(dp->dpio->driverinfo);
	if (s) {
		precondition(
			(s->flags & dp_SESSION_FLAGS_CREATESESSION) ||
			(!(dp->dpio->driverinfo->capabilities&comm_DRIVER_NO_BROADCAST)) ||
			(*s->adrMaster && s->sessionType));
	} else {
		precondition(dp->hGameServer != PLAYER_NONE);
	}
	if (s &&
		(s->flags & dp_SESSION_FLAGS_CREATESESSION) &&
		(s->sessionType == 0)) {
		DPRINT(("dpOpen: can't create session with sess type 0\n"));
		return dp_RES_BAD;
	}
	dp_assertValid(dp);
	bJoinAny = ((s == NULL) ||
			((s->karma == 0) && !(s->flags & dp_SESSION_FLAGS_CREATESESSION)));
	if (!bJoinAny) {
		dprint_sess(dp, s, "dpOpen");
	}

	/* Sanity checks */
	if (dp->players != NULL) {
		DPRINT(("dpOpen: can't open if session already open (players)\n"));
		dp_assertValid(dp);
		return dp_RES_BUSY;
	}
	if (dp->hosts != NULL) {
		DPRINT(("dpOpen: can't open if session already open (hosts)\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dp->groups != NULL) {
		DPRINT(("dpOpen: can't open if session already open (hosts)\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (!bJoinAny && ((s->maxPlayers<1) || (s->maxPlayers>dp_MAXREALPLAYERS))) {
		DPRINT(("dpOpen: bad maxPlayers %d\n", s->maxPlayers));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	if ((s == NULL) && (dp->defaultSessionType == 0)) {
		DPRINT(("dpOpen: err: join any session on game server but don't know default session type\n"));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	if (s
	&& !bJoinAny
	&& !(s->flags & dp_SESSION_FLAGS_CREATESESSION)
	&& !(s->flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS)) {
		DPRINT(("dpOpen: err: session closed to new players\n"));
		dprint_sess(dp, s, "dpOpen error");
		dp_assertValid(dp);
		return dp_RES_FULL;
	}

	/* These will be initialized when the user record for this host is
	 * added to the USERS table for this session
	 */
	dp->firstId = dp_ID_NONE;
	dp->nextId = dp_ID_NONE;
	/* FIXME: These groupid's are bogus and should really be moved to
	 * dp_sessionContext_t.
	 */
	dp->firstGId = 2 * dp_PLAYERS_PER_HOST;
	dp->nextGId = dp->firstGId;

	if (!bJoinAny) {
		err = initOpenSession(dp, s, &sess);
		if (err != dp_RES_OK)
			return err;
	} else {
		if (s && s->adrMaster) {
			if (dp->hGameServer != PLAYER_NONE)
				dpCloseGameServer(dp);
			dp->hMaster = dpio_openHdl(dp->dpio, s->adrMaster, NULL, NULL);
		} else
			dp->hMaster = dp->hGameServer;
		if ((dp->hMaster == PLAYER_NONE)
		||  (dp->hMaster == PLAYER_ME)) {
			DPRINT(("dpOpen: bad handle h:%x to host\n", dp->hMaster));
			dpClose(dp);
			return dp_RES_HOST_NOT_RESPONDING;
		}
		DPRINT(("dpOpen: opened handle to host, h:%x\n", dp->hMaster));
	}

	dpio_setHandleOptions(dp->dpio, dp->hMaster, dp_getDesiredHandleOptions(dp, dp->hMaster));

	/* Pick a random number to identify this join request by */
	dp->joinKarma = (dp_karma_t) ((dp->now + rand()) & 0xffff);
#ifdef _WIN32
	dp->joinKarma = (dp_karma_t) (dp->joinKarma ^ (short) GetTickCount());
#endif
	DPRINT(("dpOpen: chose joinKarma %d.\n", 0xffff & dp->joinKarma));

	/* We get positive confirmation that we're in the session
	 * when our host record - with the right joinkarma - appears in the
	 * session's hosts table.  (Possibly during dpSendJoinSession.)
	 * We get negative confirmation if the handle to the game
	 * host is closed.
	 * At both of those times, call the callback.
	 */
	if (cb)
		dp->dpOpen_cb = cb;
	else
		dp->dpOpen_cb = dpOpen_dummy_cb;
	dp->dpOpen_context = context;

	/* Now that the session exists, join it.
	 * (Packet will loop back to this machine if it's the master.)
	 */
	if (!bJoinAny) {
		err = dpSendJoinSession(dp, dp->joinKarma, &sess, 0, s->flags, 0, dp->hMaster);
	} else {
		dp_species_t sessType;
		short mask = dp_SESSION_FLAGS_ISLOBBY;
		if (s && s->sessionType)
			sessType = s->sessionType;
		else
			sessType = dp->defaultSessionType;

		if (s) {
			DPRINT(("dpOpen: joining any session sessType %d, flags %d\n", sessType, s->flags));
			err = dpSendJoinSession(dp, dp->joinKarma, NULL, sessType, s->flags, mask, dp->hMaster);
		} else if (dp->dpio->publicAddressKnown) {
			short flags = mask | dp_SESSION_FLAGS_ENABLE_DUALADDRESS;
			/* joining game server; pick a lobby */
			/* kludge: when joining any session on a server, include support for DUAL addresses */
			DPRINT(("dpOpen: joining any session on game server sessType %d, mask %d, flags %d\n", sessType, mask, flags));
			err = dpSendJoinSession(dp, dp->joinKarma, NULL, sessType, flags, mask, dp->hMaster);
		} else {
			DPRINT(("dpOpen: don't know public address yet; setting pleaseJoinAny\n"));
			dp->pleaseJoinAny = TRUE;
			dp->pleaseJoinAnyMask = mask;
			dp->pleaseJoinAnySessType = sessType;
			err = dp_RES_OK;
		}

	}
	if (err != dp_RES_OK) {
		DPRINT(("dpOpen: join request failed, err:%d.\n", err));
		dpClose(dp);
		dp_assertValid(dp);
		return err;
	}

	/* If we are joining a lobby, make sure we have a subscription to the
	 * session table; otherwise, blow away our subscription.
	 */
	if (!s || (s->flags & dp_SESSION_FLAGS_ISLOBBY)) {
		if (dp->hGameServer != PLAYER_NONE) {
			char key[dp_KEY_MAXLEN];
			int keylen = 1;

			/* Note: probably should set defaultSessionType here if an explicit
			 * session was given?
			 */
			key[0] = dp_KEY_SESSIONS;
			if (dp->defaultSessionType != 0) {
				keylen = 3;
				key[1] = dpGETSHORT_FIRSTBYTE(dp->defaultSessionType);
				key[2] = dpGETSHORT_SECONDBYTE(dp->defaultSessionType);
			}
			err = dptab_addPublisher(dp->dt, dp->sessions, key, keylen, dp->hGameServer);
			if (err != dp_RES_OK) {
				DPRINT(("dpOpen: bug: can't add h:%x as publisher?, err:%d\n", dp->hGameServer, err));
			}
			err = dptab_requestSubscription(dp->dt, key, keylen, dp->hGameServer, NULL, NULL);
			if (err != dp_RES_OK) {
				DPRINT(("dpOpen: bug: can't request sub from h:%x, err:%d\n", dp->hGameServer, err));
			}
			/* Don't restart player subscriptions; shell programmers will have
			 * to call dpRequestObjectDeltas() in the unlikely event they want
			 * to bring them back.
			 */
		}
	} else {
		err = dpPruneSessionSubscription(dp);
		err = dpPrunePlayerSubscriptions(dp);
	}
	DPRINT(("dpOpen: join request sent to h:%x t:%d.\n", dp->hMaster, *(dp->dpio->now)));
	/* Remember this session's description in case we are elected master. */
	if (!bJoinAny) {
		dprint_sess(dp, &sess, "dpOpen saving");
		dp->election_old_session = sess;
	}
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Close an open session.
 May be called even when session already closed.
------------------------------------------------------------------------*/
DP_API dp_result_t dpClose(
	dp_t *dp)
{
	dp_sessionContext_t *pSC = NULL;
	dp_result_t err;
	int i;
	int iSC;

	dp_assertValid(dp);
	precondition(dp);
	if (dp->closing) {
		DPRINT(("dpClose: recursion; ignoring.\n"));
		return dp_RES_OK;
	}

	DPRINT(("dpClose: karma %d (%d.%d)\n",
		dp->sess_karma, dpGETSHORT_FIRSTBYTE(dp->sess_karma), dpGETSHORT_SECONDBYTE(dp->sess_karma)));
	dumpSessionContexts(dp);

	dp_election_end(dp);

	dp->closing = TRUE;		/* kludge flag for dp_sessions_cb */

#if 0
	if (dp->myplayers) {
		/* Iterate through myplayers, deleting them all.  Callbacks galore! */
		for (i = 0; i < dp->myplayers->vars->n_used; i++) {
			void *buf;
			size_t len;
			char subkey[dptab_KEY_MAXLEN];
			int subkeylen;

			err = dptab_get_byindex(dp->myplayers, i, &buf, &len, subkey, &subkeylen);
			if (err != dp_RES_OK)
				DPRINT(("dpClose: horrible err:%d can't get myplayers by index %d\n", err, i));
			else {
				dpid_t id;
				assert(subkeylen == 2);
				id = dpMAKESHORT(subkey[0], subkey[1]);
				err = dpDestroyPlayer(dp, id);
				if (err != dp_RES_OK)
					DPRINT(("dpClose: dpDestroyPlayer(%d) returns err:%d\n", id, err));
			}
		}
	}
#endif

	if (dp->hMaster != PLAYER_NONE) {
		char key[dptab_KEY_MAXLEN];
		int keylen;

		/* Delete master's right to publish players, hosts, groups and
		 * group players tables to us, or when he does, he will crash us,
		 * since the pointer in the publications table will soon be invalid.
		 */
		keylen = 0;
		key[keylen++] = dp_KEY_PLAYERS;
		key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		dptab_deletePublisher(dp->dt, key, keylen, dp->hMaster);

		key[0] = dp_KEY_HOSTS;
		dptab_deletePublisher(dp->dt, key, keylen, dp->hMaster);

		key[0] = dp_KEY_GROUPS;
		dptab_deletePublisher(dp->dt, key, keylen, dp->hMaster);

		keylen = 5;
		key[0] = dp_KEY_GROUP_PLAYERS;
		for (i = dp->firstGId; i < dp->firstGId + dp_MAX_GROUPS; i++) {
			key[3] = (char) dpGETSHORT_FIRSTBYTE(i);
			key[4] = (char) dpGETSHORT_SECONDBYTE(i);
			dptab_deletePublisher(dp->dt, key, keylen, dp->hMaster);
		}

		/* Delete the session record itself.
		 * This prevents the session from migrating to another host?  FIXME
		 */
		if (dp->hMaster == PLAYER_ME) {
			dptab_delete(dp->dt,dp->mysessions,
				dp->sess_subkey,dp->sess_subkeylen);
			/* this entry is now invalid - delete it asap */
			dptab_delete(dp->dt,dp->sessions,
				dp->sess_subkey,dp->sess_subkeylen);
		}

		/* Tell the game host to remove us from the session */
		dpSendLeaveSession(dp, dp->joinKarma, dp->sess_karma, dp->hMaster);

		/* Close the handle to the game host, if it's not the game server. */
		/* Note: this breaks joining multiple sessions on same host. */
		if ((dp->hMaster != PLAYER_ME) && (dp->hMaster != dp->hGameServer))
			dpio_closeHdl(dp->dpio, dp->hMaster);

		/* If connected to standalone master, delete him as publisher */
		if (bIAmConnectedToStandaloneMaster(dp)) {
			dptab_deletePublisher(dp->dt, dp->select_key, dp->select_keylen, dp->hMaster);
			dp->select_keylen = 0;
			memset(dp->select_key, 0, dptab_KEY_MAXLEN);
		}
	}

	/* Delete publisher and player variable tables for all hosts in the
	 * session.  Delete subscribers to our player variable table. */
	if (dp->hosts) {
		char key[dptab_KEY_MAXLEN];
		int keylen;
		key[0] = dp_KEY_PLAYER_VARIABLES;
		key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		for (i = dptab_tableSize(dp->hosts) - 1; i >= 0; i--) {
			dptab_table_t *playervars;
			char subkey[dptab_KEY_MAXLEN];
			int subkeylen;
			char *hbuf;
			size_t len;
			dpid_t id;
			playerHdl_t h;
			err = dptab_get_byindex(dp->hosts, i, (void **)&hbuf, &len, subkey, &subkeylen);
			id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
			if (id == dp->firstId) continue; /* own player variable table */
			h = dpid2commHdl(dp, id);
			key[3] = subkey[0];
			key[4] = subkey[1];
			keylen = 5;
			if (NULL != (playervars=dptab_getTable(dp->dt, key, keylen))) {
				keylen = 3;
				dptab_deletePublisher(dp->dt, key, keylen, h);
				keylen = 5;
				dptab_deleteTable(dp->dt, key, keylen);
			}
			keylen = 3;
			if (NULL != (playervars=dptab_getTable(dp->dt, key, keylen))) {
				dptab_deleteSubscriber(dp->dt, playervars, h);
			}
		}
	}

	/* Close comm handles for all hosts that subscribe to our player table
	 * from a dpRemoteEnumPlayers call */
	if (dp->players) {
		for (i=0; i<dp->players->subscribers->n_used; i++) {
			assoctab_item_t *pe;
			assoctab_item_t *mype;
			playerHdl_t h = 0;
			pe = assoctab_getkey(dp->players->subscribers, i);
			if (!pe) {
				DPRINT(("dpClose: bug: couldn't find all subs for h:%x\n", h));
				dp_assertValid(dp);
				break;	/* horrible error */
			}
			h = pe->key;
			mype = assoctab_subscript(dp->hdl2sess, h);
			if (!mype && (h != PLAYER_ME) && (h != dp->hMaster) && (h != dp->hGameServer))
				dpio_closeHdl(dp->dpio, h);
		}
	}

	DPRINT(("dpClose: closing handles... n_used %d\n", dp->hdl2sess->n_used));
	/* Close comm handles for all hosts in current session */
	for (i=dp->hdl2sess->n_used-1; i>=0; i--) {
		assoctab_item_t *pe;
		playerHdl_t h;
		dp_sessionContext_t **spp;

		pe = assoctab_getkey(dp->hdl2sess, i);
		if (!pe) continue;
		h = pe->key;
		spp = (dp_sessionContext_t **) &pe->value;
		if (!*spp) {
			DPRINT(("dpClose: bug: hdl2sess corrupted?\n"));
			dp_assertValid(dp);
			break;
		}

		if ((*spp)->players != dp->players)
			continue;		/* handle from some other session */

		/* We're about to blow away the session - don't leave
		 * the pointer to the session in the hdl2sess table!
		 */
		assoctab_subscript_delete(dp->hdl2sess, h);
		if ((h != PLAYER_ME) && (h != dp->hMaster) && (h != dp->hGameServer))
			dpio_closeHdl(dp->dpio, h);
	}

	/* Blow away hosts, myplayers, players, groups, group players and
	 * player variable tables */
	if (dp->hosts) {
		dptab_deleteTable(dp->dt, dp->hosts->key, dp->hosts->keylen);
		dp->hosts = NULL;
	}

	if (dp->myplayers) {
		dptab_deleteTable(dp->dt, dp->myplayers->key, dp->myplayers->keylen);
		dp->myplayers = NULL;
	}

	if (dp->players) {
		/* Save a pointer to the session context */
		pSC = (dp_sessionContext_t *)dptab_getTableContext(dp->players, dp_players_cb);
		condition(pSC != NULL, "no session context present");
		err = dptab_deleteTable(dp->dt, dp->players->key, dp->players->keylen);
		dp->players = NULL;
	}

	if (dp->groups) {
		err = dptab_deleteTable(dp->dt, dp->groups->key, dp->groups->keylen);
		dp->groups = NULL;
	}

	{	char key[dptab_KEY_MAXLEN];
		int keylen;
		keylen = 5;
		key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		for (i = 0; i < dp_MAX_GROUPS; i++) {
			dpid_t gid = (dpid_t) (dp->firstGId + i);
			key[0] = dp_KEY_GROUP_PLAYERS;
			key[3] = (char) dpGETSHORT_FIRSTBYTE(gid);
			key[4] = (char) dpGETSHORT_SECONDBYTE(gid);
			err = dptab_deleteTable(dp->dt, key, keylen);
			key[0] = dp_KEY_GROUP_MYPLAYERS;
			err = dptab_deleteTable(dp->dt, key, keylen);
		}

		keylen = 3;
		key[0] = dp_KEY_PLAYER_VARIABLES;
		err = dptab_deleteTable(dp->dt, key, keylen);
	}

	/* Destroy session context */
	DPRINT(("dpClose: Removing session context p:%p.\n", pSC));
	if(pSC != NULL) {
		iSC = dynatab_find(dp->sessionContexts, &pSC);
		DPRINT(("dpClose: Found session context, id=%d, to remove.\n", iSC));
		condition(iSC != dynatab_NONE, "session context not in set");
		dynatab_remove(dp->sessionContexts, iSC);
		dp_FREE(pSC);
	}
	dumpSessionContexts(dp);

	/* Well, don't know what to do, but we sure don't want any callbacks */
	/* after this point! */
	/* (hmm, maybe we should call them back one last time.) */
	if (dp->dpCreatePlayer_cb)
		dp->dpCreatePlayer_cb(dp_ID_NONE, "", 0L, dp->dpCreatePlayer_context);
	dp->dpCreatePlayer_cb = NULL;
	if (dp->dpOpen_cb)
		dpOpenLocalResult(dp, dp_RES_CLOSED, dp_ID_NONE);

	/* Clear status variables associated with the default session */
	dp->hMaster = PLAYER_NONE;
	memset(dp->sess_subkey, 0, sizeof(dp->sess_subkey));
	dp->sess_subkeylen = 0;
	dp->sess_karma = dp_KARMA_NONE;
	dp->joinKarma = dp_KARMA_NONE;
	dp->pSess = NULL;

	/* Forget all dpid's... */
	if (dp->dpid2commHdl) {
		assoctab_destroy(dp->dpid2commHdl);
		dp->dpid2commHdl = NULL;
	}
	/* Forget all votes... */
	assoctab_destroy(dp->election_votes);
	dp->election_votes = assoctab_create(sizeof(dp_election_vote_t));

	dp->closing = FALSE;

	/* Force a dptab send cycle.  Need to get the session deletion
	 * to propagate.  There ought to be a callback to indicate this
	 * has finished...
	 */
/*	dptab_update(dp->dt); */

	DPRINT(("dpClose: successful\n"));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Cast off an open session - it stays open, but is no longer the
 "current session".
 After this, you can call dpOpen() to create a new session.

 Caveats:
 Intended for use only on game servers.
 Can only be called on the master.

 I don't think you can call dpOpen() to join a session that you cast off
 yourself, but that could be arranged if needed.
 You can't currently close sessions once they've been cast off.
 Actually, this is all backwards; there ought to be a dpCreateSession
 and a dpDestroySession that do the real work of creating and destroying
 sessions, and should work without reference to the default session pointers.

 Any players created by the local system are stranded.
 (They probably ought to be deleted, but I haven't implemented that yet.)
------------------------------------------------------------------------*/
DP_API dp_result_t dpCastoffSession(
	dp_t *dp)
{
	DPRINT(("dpCastoffSession:\n"));

	dp_assertValid(dp);

	if (NULL == dp) {
		return dp_RES_BAD;
	}
	if (dp->hMaster != PLAYER_ME) {
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	if (!dp->myplayers) {
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	/* Hmm, should propagate this.  FIXME */
	/* Hmm: can't do this, it breaks dpFreeze */
	/*dptab_deleteTable(dp->dt, dp->myplayers->key, dp->myplayers->keylen);*/
	dp->myplayers = NULL;

	/* We still care if players come and go from that session,
	 * but we don't need the quick-access pointer anymore.
	 */
	dp->players = NULL;		/* The underlying table continues to exist */

	/* No longer care if hosts, groups, group players and player variables
	 * come and go from that session */
	{	int i;
		dptab_table_t *playervars;
		char key[dptab_KEY_MAXLEN];
		int keylen;
		keylen = 3;
		key[0] = dp_KEY_PLAYER_VARIABLES;
		key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		if ((playervars = dptab_getTable(dp->dt, key, keylen)) != NULL)
			dptab_clearTableCallback(playervars, dp_playervars_cb);
		keylen = 5;
		for (i = dptab_tableSize(dp->hosts) - 1; i >= 0; i--) {
			char* buf;
			size_t len;
			dp_result_t err;
			char subkey[dptab_KEY_MAXLEN];
			int subkeylen;
			err = dptab_get_byindex(dp->hosts, i, (void **)&buf, &len, subkey, &subkeylen);
			condition(err == dp_RES_OK, "dp->hosts corrupted");
			if (((dpid_t)dpMAKESHORT(subkey[0], subkey[1])) == dp->firstId) continue; /* our own player variable table */
			key[3] = subkey[0];
			key[4] = subkey[1];
			if ((playervars = dptab_getTable(dp->dt, key, keylen)) != NULL)
				dptab_clearTableCallback(playervars, dp_playervars_cb);
		}
	}
	dptab_clearTableCallback(dp->hosts, dp_hosts_cb);
	dp->hosts = NULL;		/* The underlying table continues to exist */
	dptab_clearTableCallback(dp->groups, dp_groups_cb);
	dp->groups = NULL;		/* The underlying table continues to exist */
	{	int i;
		char key[dptab_KEY_MAXLEN];
		int keylen;
		keylen = 5;
		key[0] = dp_KEY_GROUP_PLAYERS;
		key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		for (i = 0; i < dp_MAX_GROUPS; i++) {
			dptab_table_t *grplayers;
			dpid_t gid = (dpid_t) (dp->firstGId + i);
			key[3] = (char) dpGETSHORT_FIRSTBYTE(gid);
			key[4] = (char) dpGETSHORT_SECONDBYTE(gid);
			if ((grplayers = dptab_getTable(dp->dt, key, keylen)) != NULL)
				dptab_clearTableCallback(grplayers, dp_group_players_cb);
		}
	}

	/*------ copied from dpClose --------*/
	/* Well, don't know what to do, but we sure don't want any callbacks */
	/* after this point! */
	/* (hmm, maybe we should call them back one last time.) */
	if (dp->dpCreatePlayer_cb)
		dp->dpCreatePlayer_cb(dp_ID_NONE, "", 0L, dp->dpCreatePlayer_context);
	dp->dpCreatePlayer_cb = NULL;

	/* Clear status variables associated with the default session */
	dp->hMaster = PLAYER_NONE;
	memset(dp->sess_subkey, 0, sizeof(dp->sess_subkey));
	dp->sess_subkeylen = 0;
	dp->sess_karma = dp_KARMA_NONE;
	dp->joinKarma = dp_KARMA_NONE;

	/* Forget all dpid's... */
	if (dp->dpid2commHdl) {
		assoctab_destroy(dp->dpid2commHdl);
		dp->dpid2commHdl = NULL;
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Calls the callback function once with a description of
 each game session that this object could connect to,
 finishes by calling the callback with a null session pointer.
 The callbacks might come now or during dpReceive().
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnumSessions(
	dp_t *dp,
	dp_session_t *sDesc,
	char *mhn,					/* must be NULL */
	long timeout,				/* How long in milliseconds to wait. */
	dpEnumSessionsCallback_t cb,
	void *context)
{
	dp_result_t err;
	int i;
	long timeout_dummy;
	dptab_table_t *sesstab;

	precondition(dp != NULL);
	precondition(sDesc != NULL);
	precondition(cb != NULL);
	precondition(dp->dt != NULL);
	precondition(dp->sessions != NULL);
	precondition(dp->mysessions != NULL);
	dp_assertValid(dp);

	DPRINT(("dpEnumSessions(dp, sessType %d, timeout %d):\n",
			sDesc->sessionType, timeout));

	/* End enumSessions activity if already active. */
	if (dp->enumSessions_callback) {
		long timeout_dummy;
		DPRINT(("dpEnumSessions: terminating previous callback\n"));
		(dp->enumSessions_callback)(NULL, &timeout_dummy, 0, dp->enumSessions_context );
		dp->enumSessions_callback = NULL;
	}
	/* Save filter for later use.  Save timeout to terminate later use. */
	dp->enumSessions_sessType = sDesc->sessionType;
	dp->now = eclock();	/* can't afford for this to be stale here */
	dp->enumSessions_deadline = dp->now + (timeout*dp->dpio->clocksPerSec)/1000;
	dp->enumSessions_context = context;
	dp->enumSessions_callback = cb;

	/* List the matching entries in the sessions table. */

	/* Optimization: tell the game server the sessionType we're interested in.
	 * In the case of the shell, where the user flips back and forth between
	 * multiple game types, we will kill the session table and its
	 * subscription whenever dpEnumSessions is called with a new sessionType;
	 * it'll take a few seconds for the new session table to come down, but
	 * it's probably a savings overall.
	 * Won't do this yet.
	 */

	/* If we're a standalone master, enum the mysessions table */
	if (bIAmStandaloneMaster(dp))
		sesstab = dp->mysessions;
	else
		sesstab = dp->sessions;
	DPRINT(("dpEnumSessions: table size %d\n", dptab_tableSize(sesstab)));
	for (i=0; i<dptab_tableSize(sesstab); i++) {
		char *s;
		dp_session_t sess;
		size_t len;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;

		err = dptab_get_byindex(sesstab, i, (void **)&s, &len, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpEnumSessions: bug?\n"));
			dp_assertValid(dp);
			break;
		}
		/* Must unpack when retrieving records from dptab */
		dp_unpack_session(dp, subkey, subkeylen, s, len, &sess);

#if 1
		dprint_sess(dp, &sess, "dpEnumSessions cb");
#endif

		if (
#ifdef IGNORE_CLOSEDSESS
			(sess.flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS) &&
#endif
			((sDesc->sessionType==0) || (sDesc->sessionType==sess.sessionType))) {
			timeout_dummy = 0;
			cb(&sess, &timeout_dummy, 0, context);
		}
	}

	/* Don't terminate the callback now - do that in dpPoll() unless
	 * timeout is set to zero.
	 * Report sessions that appear in the meantime via the callback.
	 */
	if (timeout == 0) {
		/*DPRINT(("dpEnumSessions: Exiting.\n"));*/
		cb(NULL, &timeout_dummy, 0, context);
		dp->enumSessions_callback = NULL;
	}

	(void) mhn;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Declare that the current session is a lobby.
 May only be called by host.
 The parameter 'flags' should be set to 0 for a peer lobby,
 and to 1 for a server lobby.
----------------------------------------------------------------------*/
DP_API dp_result_t dpDeclareLobby(
	dp_t *dp,
	int flags)
{
	dp_session_t sess;
	dp_result_t err;

	dp_assertValid(dp);
	dpGetSessionDesc(dp, &sess, 0);
	if (flags) {
		DPRINT(("dpDeclareLobby: Setting flags from %x\n", sess.flags));
		sess.flags |=
			dp_SESSION_FLAGS_ISSERVER
			| dp_SESSION_FLAGS_MIGRATE_FORBIDDEN
			| dp_SESSION_FLAGS_ISLOBBY;
	} else {
		DPRINT(("dpDeclareLobby: Clearing flags from %x\n", sess.flags));
		dpio_setMaxPlayerHdls(dp->dpio, dp_MAXREALPLAYERS + 5);
		sess.flags &= ~(
			dp_SESSION_FLAGS_ISSERVER
			| dp_SESSION_FLAGS_MIGRATE_FORBIDDEN
#if 0
			| dp_SESSION_FLAGS_ISLOBBY
#endif
			);
	}
	DPRINT(("dpDeclareLobby: Final flags=%x\n", sess.flags));
	err = dp_setSessionDesc(dp, &sess, 0);
	dp_assertValid(dp);
	return err;
}

/*----------------------------------------------------------------------
 Clear out any unnecessary player table publishers (i.e. any but the
 host of the session we are in) and tell them to cancel our
 subscription.
 Call this when entering a game to really turn off (not just ignore)
 player deltas from outside sessions.
----------------------------------------------------------------------*/
dp_result_t dpPrunePlayerSubscriptions(dp_t *dp)
{
	dp_result_t err;
	int i;

	if (bIAmGameServer(dp))
		return dp_RES_OK;
	if (!dp->dt || !dp->dt->peers)
		return dp_RES_BAD;

	for (i = 0; i < dp->dt->peers->n_used; i++) {
		playerHdl_t h;
		assoctab_item_t *dpe;
		dptab_table_t *table;

		dpe = assoctab_getkey(dp->dt->peers, i);
		if (!dpe) {
			DPRINT(("dpPrunePlayerSubscriptions: bug: can't fetch peer %d; peers->n_used %d\n", i, dp->dt->peers->n_used));
			return dp_RES_BUG;
		}
		h = dpe->key;

		/* Blow away player subscriptions from anyone but the host */
		if (h == dp->hMaster)
			continue;
		if (h == PLAYER_NONE) {
			DPRINT(("dpPrunePlayerSubscriptions: PLAYER_NONE in peer table?\n"));
			assert(h != PLAYER_NONE);
			continue;
		}
		err = dptab_shutdownMatchingSubscription(dp->dt, dp_KEY_PLAYERS, h, &table);
		if (err != dp_RES_OK) {
			DPRINT(("dpPrunePlayerSubscriptions: failed to shutdown player table subscription to h:%x\n", h));
			return dp_RES_BUG;
		}
		/* Inform user code that this table is going away.  The closest thing
		 * we have to this right now is to tell him that all the elements are
		 * going away. This lets him clean up his user interface.
		 */
		err = dptab_delete_byAge(dp->dt, table, 0);
		if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)) {
			DPRINT(("dpPrunePlayerSubscriptions: bug: failed to delete players in players table, err:%d\n", err));
		}
		/* Assume that there's only one matching table and that it's ok to
		 * blow it away.
		 */
		err = dptab_deleteTable(dp->dt, table->key, table->keylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpPrunePlayerSubscriptions: bug: failed to delete players table\n"));
			return dp_RES_BUG;
		}
	}
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Clear out any unnecessary session table publishers (i.e. the server,
 if we are not in a lobby) and tell them to cancel our subscription.
 Call this only when entering a game (as opposed to a lobby) to really
 turn off (not just ignore) session deltas from outside sessions.
----------------------------------------------------------------------*/
dp_result_t dpPruneSessionSubscription(dp_t *dp)
{
	dp_result_t err;
	dptab_table_t *table;

	if (bIAmGameServer(dp) || (dp->hGameServer == PLAYER_NONE))
		return dp_RES_OK;

	err = dptab_shutdownMatchingSubscription(dp->dt, dp_KEY_SESSIONS, dp->hGameServer, &table);
	if (err != dp_RES_OK) {
		DPRINT(("dpPruneSessionSubscriptions: failed to shutdown session table subscriptions to hGameServer:%x\n", dp->hGameServer));
		return dp_RES_BUG;
	}
	/* Inform user code that this table is going away.  The closest thing
	 * we have to this right now is to tell him that all the elements are
	 * going away. This lets him avoid ghost sessions in his user interface.
	 */
	err = dptab_delete_byAge(dp->dt, table, 0);
	if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)) {
		DPRINT(("dpPruneSessionSubscription: bug: failed to delete sessions in sessions table, err:%d\n", err));
	}
	return dp_RES_OK;
}

/************************ Player Management ******************************/

/*----------------------------------------------------------------------
 Add a new player to the currently open session.
 On Success, returns dp_RES_OK, and calls callback with dpid either
 immediately or on later call to dpReceive.
 On failure, either returns !dp_RES_OK immediately, or returns dp_RES_OK
 now, but calls callback with dp_ID_NONE during later call to dpReceive.
----------------------------------------------------------------------*/
DP_API dp_result_t dpCreatePlayer(
	dp_t	*dp,
	dpEnumPlayersCallback_t cb,
	void	*context,
	char_t	*name)
{
	dp_playerId_t player;
	char playerbuf[dpio_MAXLEN_RELIABLE];
	int playerlen;
	dp_result_t err;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	dp_assertValid(dp);

	DPRINT(("dpCreatePlayer(name %s):\n", name));
	if (!dp)
		return dp_RES_BUG;
	if (dp->dpCreatePlayer_cb) {
		DPRINT(("dpCreatePlayer: err: busy\n"));
		dp_assertValid(dp);
		return dp_RES_BUSY;
	}
	if (!dp->players || (dp->nextId == dp_ID_NONE)) {
		DPRINT(("dpCreatePlayer: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	/* Make sure we're not out of id's */
	if (dp->nextId >= dp->firstId + dp_PLAYERS_PER_HOST) {
		DPRINT(("dpCreatePlayer: err: no more id's for me\n"));
		return dp_RES_FULL;
	}

	/* Make sure session is not full */
	{
		dp_session_t sess;

		err = dpGetSessionDesc(dp, &sess, 0);
		if (err == dp_RES_OK) {
			DPRINT(("dpCreatePlayers: numPl %d, maxPl %d\n", dpNumPlayers(dp), sess.maxPlayers));
			if (dpNumPlayers(dp) >= sess.maxPlayers) {
				DPRINT(("dpCreatePlayers: session full\n"));
				dp_assertValid(dp);
				return dp_RES_FULL;
			}
		} else if (dp->dpio->driverinfo->capabilities&comm_DRIVER_NO_BROADCAST){
			DPRINT(("dpCreatePlayers: can't get session description?\n"));
			/* Session description not there?  Bad if it's on the server... */
			return err;
		}
	}

	/* Create the player record. */
	memset(&player, 0, sizeof(player));
	player.id = dp->nextId++;
	memcpy(player.adr, dp->dpio->myAdr, dp->dpio->myAdrLen);
	strncpy(player.name, name, dp_PNAMELEN);
	do
		player.karma = (dp_karma_t) dp_KARMA_MAKE(FALSE, rand());
	while (player.karma == dp_KARMA_NONE);	/* Should also make unique FIXME */

	/* Prepare for the callback. */
	dp->dpCreatePlayer_cb = cb;
	dp->dpCreatePlayer_context = context;

	/* Player record's key is its two-byte player id. */
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(player.id);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(player.id);

	/* Write the player record; this triggers the callback on remote hosts. */
	playerlen = dp_pack_playerId(&player, playerbuf);
#if 0
	{
		dp_playerId_t foo;
		int len_used = dp_unpack_playerId(player.id, playerbuf, &foo);
		DPRINT(("dpCreatePlayer: id %d %d, karma %d %d bloblen %d %d, name %s %s\n",
				player.id, foo.id,
				player.karma, foo.karma,
				player.bloblen, foo.bloblen,
				player.name, foo.name));
	}
#endif
	err = dptab_set(dp->dt, dp->myplayers, subkey, subkeylen, playerbuf, playerlen, 2, PLAYER_ME);
	if (err != dp_RES_OK) {
		DPRINT(("dpCreatePlayer: can't set player record, err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}
	/* The myplayers listener on the host will loop the data back to the
	 * host's players table.
	 */
	dptab_update(dp->dt);	/* We're pretty darned impatient about players */

	DPRINT(("dpCreatePlayer: set player record.  Callback should have already"
			" happened locally.\n"));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Destroy the given player; removes the player from the game session.
 The dpID will not be reused during the current session.
 On success, returns dp_RES_OK.  If dp or group players table is null,
 returns dp_RES_BUG; returns dp_RES_CLOSED if session isn't fully initialized,
 or returns return value of dptab_delete().
------------------------------------------------------------------------*/
DP_API dp_result_t dpDestroyPlayer(
	dp_t   *dp,
	dpid_t id)
{
	dp_result_t err;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	dp_assertValid(dp);
	if (!dp)
		return dp_RES_BUG;
	if (!dp->players) {
		DPRINT(("dpDestroyPlayer: err: session not initialized"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	err = dptab_delete(dp->dt, dp->myplayers, subkey, subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dpDestroyPlayer: err:%d\n", err));
	}

#if 0
	/* When we delete from myplayers, dp_myplayers_cb handles deleting
	 * from players.
	 * Originally, the only way for players to disappear from the player
	 * table was if the machine that created the player deleted the player
	 * from his myplayers table; this deletion would propagate to the host's
	 * players table, thence to everybody's player table.
	 * On the host, the myplayers table didn't propagate to the players table,
	 * so we had to delete from the players table manually.
	 */
	if (dpIsMaster(dp)) {
		err = dptab_delete(dp->dt, dp->players, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpDestroyPlayer: err:%d\n", err));
		}
	}
#endif

	dp_assertValid(dp);
	return err;
}

/*------------------------------------------------------------------------
 Return the number of players in the current session.
------------------------------------------------------------------------*/
DP_API int dpNumPlayers(
	dp_t *dp)
{
	if (!dp || !dp->players)
		return 0;

	dp_assertValid(dp);
	return dptab_tableSize(dp->players);
}

/*------------------------------------------------------------------------
 Get the id of the current host for the open session, if he has any
 players.
 Returns dp_RES_OK on success,
 		 dp_RES_CLOSED if no session is open,
 		 dp_RES_EMPTY if the host has no players.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetCurrentHostId(
	dp_t *dp,
	dpid_t *phid)
{
	dp_assertValid(dp);
	if (!dp->players) {
		DPRINT(("dpGetCurrentHostId: no session\n"));
		return dp_RES_CLOSED;
	}
	*phid = dp_commHdl2dpid(dp, dp->hMaster);
	if (*phid == dp_ID_NONE) {
		DPRINT(("dpGetCurrentHostId: no id for master h:%x\n", dp->hMaster));
		return dp_RES_EMPTY;
	}
	DPRINT(("dpGetCurrentHostId: id for master h:%x is id:%d\n", dp->hMaster, *phid));
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Convert a dpid into a player name.
 On success return dp_RES_OK, and copy at most bufLen-1 bytes of player
 name into buf; always null terminate result.
 On failure, return dp_RES_EMPTY.
------------------------------------------------------------------------*/
DP_API dp_result_t dpGetPlayerName(
	dp_t *dp,
	dpid_t id,
	dp_char_t *buf,
	size_t bufLen)
{
	dp_result_t err;
	dp_playerId_t player;
	char *playerbuf;
	size_t len_used;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);
	if (!dp->players) return dp_RES_CLOSED;
	if (!dp->groups) return dp_RES_CLOSED;

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	if (id >= dp->firstGId && id < dp->firstGId + dp_MAX_GROUPS) {
		err = dptab_get_bykey(dp->groups, subkey, subkeylen, (void **)&playerbuf, &len);
		if (err != dp_RES_OK) {
			dp_assertValid(dp);
			return dp_RES_EMPTY;
		}

		/* drk: calculate number of bytes to copy minus nul assuming no nul
		 * in dptab.
		 */
		if (len >= bufLen)
			len = bufLen - 1;

		if (buf) {
			memcpy(buf, playerbuf, len);
			buf[len] = 0;
		}

		dp_assertValid(dp);
		return dp_RES_OK;
	}

	err = dptab_get_bykey(dp->players, subkey, subkeylen, (void **)&playerbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}
	len_used = dp_unpack_playerId(id, playerbuf, &player);
	if (len != len_used) {
		DPRINT(("dpGetPlayerName: can't unpack player %d.\n", id));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	if (buf) strncpy(buf, player.name, bufLen);
	if (buf && (bufLen > 0)) buf[bufLen-1] = 0;
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Set the name for the player indicated by the given dpid.
 If name is too long, it will be truncated.
 On success return dp_RES_OK
------------------------------------------------------------------------*/
DP_API dp_result_t dpSetPlayerName(
	dp_t *dp,
	dpid_t id,
	dp_char_t *name)
{
	dp_result_t err;
	dp_playerId_t player;
	char *playerbuf;
	char newplayerbuf[1024];
	size_t len_used;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);
	if (!name) return dp_RES_BAD;
	if (!dp->players) return dp_RES_CLOSED;
	if (!dp->groups) return dp_RES_CLOSED;

	DPRINT(("dpSetPlayerName(id:%d, name:%s):\n", id, name));

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	err = dptab_get_bykey(dp->myplayers, subkey, subkeylen, (void **)&playerbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}
	len_used = dp_unpack_playerId(id, playerbuf, &player);
	if (len != len_used) {
		DPRINT(("dpSetPlayerName: can't unpack player %d.\n", id));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	strncpy(player.name, name, sizeof(player.name));
	player.name[sizeof(player.name)-1] = 0;
	len = dp_pack_playerId(&player, newplayerbuf);
	err = dptab_set(dp->dt, dp->myplayers, subkey, subkeylen, newplayerbuf, len, 2, PLAYER_ME);
	if (dp->hMaster == PLAYER_ME) {
		err = dptab_set(dp->dt, dp->players, subkey, subkeylen, newplayerbuf, len, 1, PLAYER_ME);
	}
	dp_assertValid(dp);
	return err;
}

/*------------------------------------------------------------------------
 Set the uid for the player indicated by the given dpid.
 On success return dp_RES_OK
------------------------------------------------------------------------*/
static dp_result_t dpSetPlayerUid(
	dp_t *dp,
	dpid_t id,
	dp_uid_t uid)
{
	dp_result_t err;
	dp_playerId_t player;
	char *playerbuf;
	char newplayerbuf[1024];
	size_t len_used;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);
	if (!dp->players) return dp_RES_CLOSED;

	DPRINT(("dpSetPlayerUid(id:%d, uid:%d):\n", id, uid));

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	err = dptab_get_bykey(dp->myplayers, subkey, subkeylen, (void **)&playerbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}
	len_used = dp_unpack_playerId(id, playerbuf, &player);
	if (len != len_used) {
		DPRINT(("dpSetPlayerName: can't unpack player %d.\n", id));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	/* KLUDGE: Only have 16 bits for UID right now */
	player.karma = (dp_karma_t)(uid & 0xffff);	/* kludge... */
	len = dp_pack_playerId(&player, newplayerbuf);
	err = dptab_set(dp->dt, dp->myplayers, subkey, subkeylen, newplayerbuf, len, 2, PLAYER_ME);

	dp_assertValid(dp);
	return err;
}

/*------------------------------------------------------------------------
 Retrieve the blob associated with player having the given id.
 On entry, *pbuflen holds the length of buf.
 If *pbuflen is not big enough to hold the blob, the actual
 size of the blob is placed in *pbuflen, and dp_RES_FULL is
 returned.

 On exit, *pbuflen holds the actual length of the blob,
 and the blob is copied into buf.
 On success return dp_RES_OK
------------------------------------------------------------------------*/
DP_API dp_result_t dpGetPlayerBlob(
	dp_t *dp,
	dpid_t id,
	dp_char_t *buf,
	size_t *pbuflen)
{
	dp_result_t err;
	dp_playerId_t player;
	char *playerbuf;
	size_t len_used;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);
	if (!dp->players) return dp_RES_CLOSED;
	if (!dp->groups) return dp_RES_CLOSED;
	if (!pbuflen) return dp_RES_BAD;

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	err = dptab_get_bykey(dp->players, subkey, subkeylen, (void **)&playerbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}
	len_used = dp_unpack_playerId(id, playerbuf, &player);
	if (len != len_used) {
		DPRINT(("dpGetPlayerBlob: can't unpack player %d.\n", id));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	if (*pbuflen < player.bloblen) {
		*pbuflen = player.bloblen;
		return dp_RES_FULL;
	}
	*pbuflen = player.bloblen;
	if (buf) memcpy(buf, player.blob, player.bloblen);

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Set the blob for the player indicated by the given dpid.
 If blob is too long, dp_RES_FULL is returned.
 On success return dp_RES_OK
------------------------------------------------------------------------*/
DP_API dp_result_t dpSetPlayerBlob(
	dp_t *dp,
	dpid_t id,
	void *blob,
	size_t bloblen)
{
	dp_result_t err;
	dp_playerId_t player;
	char *playerbuf;
	char newplayerbuf[1024];
	size_t len_used;
	size_t len;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);
	if (!blob) return dp_RES_BAD;
	if (bloblen < 1) return dp_RES_BAD;
	if (bloblen > dp_MAX_PLAYERBLOB_LEN) return dp_RES_FULL;
	if (!dp->players) return dp_RES_CLOSED;
	if (!dp->groups) return dp_RES_CLOSED;

	DPRINT(("dpSetPlayerBlob(id:%d, bloblen:%d):\n", id, bloblen));

	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;

	err = dptab_get_bykey(dp->myplayers, subkey, subkeylen, (void **)&playerbuf, &len);
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return dp_RES_EMPTY;
	}
	len_used = dp_unpack_playerId(id, playerbuf, &player);
	if (len != len_used) {
		DPRINT(("dpSetPlayerBlob: can't unpack player %d.\n", id));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	memcpy(player.blob, blob, bloblen);
	player.bloblen = bloblen;
	len = dp_pack_playerId(&player, newplayerbuf);
	err = dptab_set(dp->dt, dp->myplayers, subkey, subkeylen, newplayerbuf, len, 2, PLAYER_ME);
	if (dp->hMaster == PLAYER_ME) {
		err = dptab_set(dp->dt, dp->players, subkey, subkeylen, newplayerbuf, len, 1, PLAYER_ME);
	}
	dp_assertValid(dp);
	return err;
}

/*----------------------------------------------------------------------
 Enumerate a player list from the given remote session if it exists.
 If not, request the  player list from the given remote session.
----------------------------------------------------------------------*/
static dp_result_t dpRemoteEnumPlayers(
	dp_t *dp,
	dp_session_t *sDesc,
	dpEnumPlayersCallback_t cb,
	void *context,
	dpid_t start)
{
	int i;
	dp_result_t err;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	dptab_table_t *rplayers;

	dp_assertValid(dp);

	keylen = 0;
	key[keylen++] = dp_KEY_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(sDesc->karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(sDesc->karma);
	if (!(rplayers = dptab_getTable(dp->dt, key, keylen))) {
		playerHdl_t dest = dpio_openHdl(dp->dpio, sDesc->adrMaster, NULL, NULL);
		if (dest == PLAYER_NONE) {
			DPRINT(("dpRemoteEnumPlayers: err: can't get handle\n"));
			dp_assertValid(dp);
			return dp_RES_FULL;
		}

		/* Have server tell session host to open a connection to us,
		 * just in case it's behind a firewall, or is behind same NAT as us
		 */
		err = dpSendIndirectJoin(dp, sDesc, FALSE);
		if (err != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't send indirect open\n"));
			dp_assertValid(dp);
			return err;
		}

		if ((err = dptab_createTable(dp->dt, &rplayers, key, keylen, 0, NULL, NULL, NULL, NULL)) != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't create table\n"));
			dp_assertValid(dp);
			return err;
		}
		err = dptab_addPeer(dp->dt, dest);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("dpRemoteEnumPlayers: err: can't add h:%x to peer table, err:%d\n", dest, err));
			dp_assertValid(dp);
			return err;
		}
		if ((err = dptab_addPublisher(dp->dt, rplayers, key, keylen, dest)) != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't addPublisher\n"));
			dp_assertValid(dp);
			return err;
		}
		if ((err = dptab_requestSubscription(dp->dt, key, keylen, dest, NULL, NULL)) != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err:%d can't request subscription from h:%x\n",err, dest));
			dp_assertValid(dp);
			return err;
		}
		if (cb) cb(dp_ID_NONE, NULL, 0, context);
		dp_assertValid(dp);
		return dp_RES_OK;
	}
	if (!cb) return dp_RES_EMPTY;
	for (i=dptab_tableSize(rplayers) - 1; i >= 0; i--) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dp_result_t err;
		dp_playerId_t player;
		char *playerbuf;
		size_t len_used;
		size_t len;
		dpid_t id;

		err = dptab_get_byindex(rplayers, i, (void **)&playerbuf, &len, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't get element %d on players table.\n", i));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
		id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		len_used = dp_unpack_playerId(id, playerbuf, &player);
		if (len != len_used) {
			DPRINT(("dpRemoteEnumPlayers: err: can't unpack player %d (%d != %d).\n", id, len, len_used));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}

		DPRINT(("dpRemoteEnumPlayers: cb(%d, %s, %x, %p)\n",
			player.id, player.name, dp_EPC_FLAGS_REMOTE, context));

		cb(player.id, player.name, dp_EPC_FLAGS_REMOTE, context);
	}
	DPRINT(("dpRemoteEnumPlayers: cb(id:NONE, NULL, 0, %p) for end of list.\n", context));
	cb(dp_ID_NONE, NULL, 0, context);

	(void) start;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Calls the given function once for each player in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.
------------------------------------------------------------------------*/
DP_API dp_result_t dpEnumPlayers(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	)
{
	int i;
	dp_result_t err;

	precondition(dp != NULL);
	precondition(cb != NULL);
	dp_assertValid(dp);

	DPRINT(("dpEnumPlayers: entry t:%d\n", eclock()));

	/* if he specified a session, and it's not the current session, go remote */
	if (s && (!dp->pSess || memcmp(s->reserved2, dp->pSess->sess_subkey, dp->pSess->sess_subkeylen))) {
		DPRINT(("dpEnumPlayers: remote check\n"));
		err = dpRemoteEnumPlayers(dp, s, cb, context, 0);
		dp_assertValid(dp);
		return err;
	}
	if (!dp->players) {
		DPRINT(("dpEnumPlayers: err: null players table\n"));
		cb(dp_ID_NONE, NULL, 0, context);
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	DPRINT(("dpEnumPlayers: %d players in table\n", dptab_tableSize(dp->players)));
	for (i=dptab_tableSize(dp->players) - 1; i >= 0; i--) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dp_playerId_t player;
		char *playerbuf;
		size_t len_used;
		size_t len;
		long flag;
		dpid_t id;

		err = dptab_get_byindex(dp->players, i, (void **)&playerbuf, &len, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpEnumPlayers: err: can't get element %d on players table.\n", i));
			break;
		}
		id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		len_used = dp_unpack_playerId(id, playerbuf, &player);
		if (len != len_used) {
			DPRINT(("dpEnumPlayers: err: can't unpack player %d (%d != %d).\n", id, len, len_used));
			dp_assertValid(dp);
			return dp_RES_BAD;
		}

		flag = dp_EPC_FLAGS_LOCAL;
		if ((id & ~(dp_PLAYERS_PER_HOST - 1)) != dp->firstId)
			flag = dp_EPC_FLAGS_REMOTE;
		DPRINT(("dpEnumPlayers: cb(id:%d, %s, %x, %p) for h:%x\n",
			player.id, player.name, flag, context, dpid2commHdl(dp, player.id)));
		cb(player.id, player.name, flag, context);
	}
    DPRINT(("dpEnumPlayers: cb(id:NONE, NULL, 0, %p) for end of list.\n", context));
	cb(dp_ID_NONE, NULL, 0, context);

	(void) timeout;

	dp_assertValid(dp);
	DPRINT(("dpEnumPlayers: exit\n"));

	return dp_RES_OK;
}

#if 1
/* KLUDGE : following are exact copies of above, but with different
 * callback parameters
 */

/*----------------------------------------------------------------------
 Enumerate a player list from the given remote session if it exists.
 If not, request the  player list from the given remote session.
----------------------------------------------------------------------*/
static dp_result_t dpRemoteEnumPlayersEx(
	dp_t *dp,
	dp_session_t *sDesc,
	dpEnumPlayersExCallback_t cb,
	void *context,
	dpid_t start)
{
	int i;
	dp_result_t err;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	dptab_table_t *rplayers;

	dp_assertValid(dp);

	keylen = 0;
	key[keylen++] = dp_KEY_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(sDesc->karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(sDesc->karma);
	if (!(rplayers = dptab_getTable(dp->dt, key, keylen))) {
		playerHdl_t dest = dpio_openHdl(dp->dpio, sDesc->adrMaster, NULL, NULL);
		if (dest == PLAYER_NONE) {
			DPRINT(("dpRemoteEnumPlayers: err: can't get handle\n"));
			dp_assertValid(dp);
			return dp_RES_FULL;
		}

		/* Have server tell session host to open a connection to us,
		 * just in case it's behind a firewall, or is behind same NAT as us
		 */
		err = dpSendIndirectJoin(dp, sDesc, FALSE);
		if (err != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't send indirect open\n"));
			dp_assertValid(dp);
			return err;
		}

		if ((err = dptab_createTable(dp->dt, &rplayers, key, keylen, 0, NULL, NULL, NULL, NULL)) != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't create table\n"));
			dp_assertValid(dp);
			return err;
		}
		err = dptab_addPeer(dp->dt, dest);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("dpRemoteEnumPlayers: err: can't add h:%x to peer table, err:%d\n", dest, err));
			dp_assertValid(dp);
			return err;
		}
		if ((err = dptab_addPublisher(dp->dt, rplayers, key, keylen, dest)) != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't addPublisher\n"));
			dp_assertValid(dp);
			return err;
		}
		if ((err = dptab_requestSubscription(dp->dt, key, keylen, dest, NULL, NULL)) != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't request subscription\n"));
			dp_assertValid(dp);
			return err;
		}
		cb(dp_ID_NONE, NULL, 0, context, NULL);
		dp_assertValid(dp);
		return dp_RES_OK;
	}
	for (i=dptab_tableSize(rplayers) - 1; i >= 0; i--) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dp_result_t err;
		dp_playerId_t player;
		char *playerbuf;
		size_t len_used;
		size_t len;
		dpid_t id;

		err = dptab_get_byindex(rplayers, i, (void **)&playerbuf, &len, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpRemoteEnumPlayers: err: can't get element %d on players table.\n", i));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
		id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		len_used = dp_unpack_playerId(id, playerbuf, &player);
		if (len != len_used) {
			DPRINT(("dpRemoteEnumPlayers: err: can't unpack player %d (%d != %d).\n", id, len, len_used));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}

		/*DPRINT(("dpRemoteEnumPlayers: cb(%d, %s, %x, %p)\n",
			player.id, player.name, dp_EPC_FLAGS_REMOTE, context));*/
		cb(player.id, player.name, dp_EPC_FLAGS_REMOTE, context, &player);
	}
    /*DPRINT(("dpEnumPlayers: cb(id:NONE, NULL, 0, %p) for end of list.\n", context));*/
	cb(dp_ID_NONE, NULL, 0, context, NULL);

	(void) start;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Calls the given function once for each player in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.
------------------------------------------------------------------------*/
DP_API dp_result_t dpEnumPlayersEx(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersExCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	)
{
	int i;
	dp_result_t err;

	precondition(dp != NULL);
	precondition(cb != NULL);
	dp_assertValid(dp);

	DPRINT(("dpEnumPlayersEx: (s:%s, timeout:%d) entry t:%d\n", ((s) ? s->sessionName : "NULL"), timeout, eclock()));

	if (s) {
		DPRINT(("dpEnumPlayers: remote check\n"));
		err = dpRemoteEnumPlayersEx(dp, s, cb, context, 0);
		dp_assertValid(dp);
		return err;
	}
	if (!dp->players) {
		DPRINT(("dpEnumPlayers: err: null players table\n"));
		cb(dp_ID_NONE, NULL, 0, context, NULL);
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	DPRINT(("dpEnumPlayers: %d players in table\n", dptab_tableSize(dp->players)));
	for (i=dptab_tableSize(dp->players) - 1; i >= 0; i--) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dp_playerId_t player;
		char *playerbuf;
		size_t len_used;
		size_t len;
		long flag;
		dpid_t id;

		err = dptab_get_byindex(dp->players, i, (void **)&playerbuf, &len, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpEnumPlayers: err: can't get element %d on players table.\n", i));
			break;
		}
		id = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		len_used = dp_unpack_playerId(id, playerbuf, &player);
		if (len != len_used) {
			DPRINT(("dpEnumPlayers: err: can't unpack player %d (%d != %d).\n", id, len, len_used));
			dp_assertValid(dp);
			return dp_RES_BAD;
		}

		flag = dp_EPC_FLAGS_LOCAL;
		if ((id & ~(dp_PLAYERS_PER_HOST - 1)) != dp->firstId)
			flag = dp_EPC_FLAGS_REMOTE;
		DPRINT(("dpEnumPlayers: cb(id:%d, %s, %x, %p) for h:%x\n",
			player.id, player.name, flag, context, dpid2commHdl(dp, player.id)));
		cb(player.id, player.name, flag, context, &player);
	}
    DPRINT(("dpEnumPlayers: cb(id:NONE, NULL, 0, %p) for end of list.\n", context));
	cb(dp_ID_NONE, NULL, 0, context, NULL);

	(void) timeout;

	dp_assertValid(dp);
	DPRINT(("dpEnumPlayers: exit\n"));

	return dp_RES_OK;
}
#endif

/*----------------------------------------------------------------------
 Enable or disable new players from entering the game.
 May only be called by host.
 If called with FALSE, session packets will no longer be broadcast,
 and add player requests will be ignored.
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnableNewPlayers(
	dp_t *dp,
	int enable)
{
	dp_session_t sess;

	if (dp->hMaster != PLAYER_ME) return dp_RES_BAD;
	dpGetSessionDesc(dp, &sess, 0);
	if (enable)
		sess.flags |= dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
	else
		sess.flags &= ~dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
#ifdef USE_dp_enableNewPlayers
	/* dpSetSessionDesc sets session flag according to dp->enableNewPlayers */
	dp->enableNewPlayers = enable;
#endif
	dp_setSessionDesc(dp, &sess, 0);
	dp_assertValid(dp);
	return dp_RES_OK;
}

/******************** Sending/Receiving Datagrams ************************/

#define dp_ELECTIONLEN ((10 * dp->dpio->maxRxInterval)/8)

/*--------------------------------------------------------------------------
  If an election isn't in progress, start one.  Otherwise, do nothing.
  Set hostDeleted TRUE only when calling because you've just noticed the
  host's handle has closed; set it FALSE if somebody else started the
  election.
--------------------------------------------------------------------------*/
static void dp_election_start(dp_t *dp, int hostDeleted)
{
	int nhosts;

	assert(dp != NULL);
	dp_assertValid(dp);

	if (dp->election_size)
		return;

	assert(dp->election_votes);

	/* Election lasts as long as the expected skew in player timeouts
	 * PLUS a player timeout interval, to handle those pesky two-players-die
	 * scenarios.  (Perhaps we should keep timeouts going to guy who
	 * will be host, to speed those elections along?)
	 */
	dp->election_deadline = dp->now + dp_ELECTIONLEN;

	/* The compromise phase starts sooner */
	dp->election_yield_deadline = dp->now + dp->dpio->maxRxInterval;

	/* Remember how many machines were in session before the election */
	nhosts = dptab_tableSize(dp->hosts);
	if (hostDeleted)
		nhosts++;

	/* Set # of votes needed to win election.
	 * Could just be nhosts/2, but let's require a stronger majority.
	 * We want it to work if 2 out of seven hosts crap out, so let's
	 * set the bar at about 5/7.  Can't set it much lower, else
	 * someone may declare he won while somebody still thinks the old
	 * host is winning.  We may need to change the validation in
	 * dpHandleVictory to work in that case.
	 */
	dp->election_size = (2 * nhosts) / 3;

	/* ... but make sure 2/3 > 1/2, so 4 player games can't split */
	if ((dp->election_size * 2) == nhosts) {
		DPRINT(("dp_election_start: incrementing election size to avoid even split.\n"));
		dp->election_size++;
	}

	/* Final special case: in a two-player game, the above rule forbids
	 * migration.  Let's allow it iff the host left gracefully.
	 */
	if (hostDeleted && (nhosts==2) && (dpio_getHdlState(dp->dpio, dp->hMaster) & dpio_STATE_FIN_RCVD)) {
		DPRINT(("dp_election_start: host left nicely; allowing migration\n"));
		dp->election_size = 1;
	}

	DPRINT(("dp_election_start: t:%d, yield t:%d, end t:%d; %d hosts, toWin %d, hostDeleted %d\n",
		dp->now, dp->election_yield_deadline, dp->election_deadline,
		nhosts, dp->election_size, hostDeleted));

/*  Session description already gone; use one cached earlier
	err = dpGetSessionDesc(dp, &dp->election_old_session, 0);
	assert (err == dp_RES_OK);*/

	/* Turn on keepalive for all handles for duration of election */
	{
		int i, n;
		playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */

		n = dp_getHdls(dp, dp_ID_BROADCAST, dests, dp_SEND_RELIABLE);
		for (i=0; i<n; i++)
			dpio_setHandleOptions(dp->dpio, dests[i], dpio_OPTION_KEEPALIVE);
	}

	postcondition(dp->election_size > 0);
	postcondition(dp->election_votes);
}

/*--------------------------------------------------------------------------
 Deallocate resources used by an election if it is in progress.
--------------------------------------------------------------------------*/
static void dp_election_end(dp_t *dp)
{
	clock_t election_start;

	assert(dp != NULL);
	dp_assertValid(dp);

	if (!dp->election_size)
		return;

	election_start = dp->election_deadline - dp_ELECTIONLEN;
	DPRINT(("dp_election_end: t:%d; election lasted %d ticks\n", dp->now,
		dp->now - election_start));
	dp->election_deadline = 0;
	dp->election_yield_deadline = 0;
	dp->election_size = 0;

	/* Turn off keepalive for all handles but host */
	{
		int i, n;
		playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */

		n = dp_getHdls(dp, dp_ID_BROADCAST, dests, dp_SEND_RELIABLE);
		for (i=0; i<n; i++) {
			dpio_setHandleOptions(dp->dpio, dests[i], dp_getDesiredHandleOptions(dp, dests[i]));
		}
	}

	dp->election_size = 0;
}

#define DP_ELECTION_MAXAGE (dp->dpio->maxRxInterval / 2)

/*--------------------------------------------------------------------------
 Call periodically during an election.
--------------------------------------------------------------------------*/
static void dp_election_poll(dp_t *dp)
{
	clock_t age;

	if (!dp->election_size)
		return;

	if ((long)(dp->election_deadline - dp->now) < 0) {
		DPRINT(("dpPoll: election took too long.\n"));
		dp->pleaseClose = TRUE;
	}
	age = dpio_getHdlAge(dp->dpio, dp->hMaster);
	if (age > DP_ELECTION_MAXAGE) {
		DPRINT(("dp_election_poll: hMaster h:%x too old; age %d > maxage %d\n",
				dp->hMaster, age, DP_ELECTION_MAXAGE));
		dpElectMaster(dp);
	}
}

/*----------------------------------------------------------------------
 Switch from current master to indicated master.
----------------------------------------------------------------------*/
static dp_result_t dp_election_set_master(dp_t *dp, dpid_t newIdMaster)
{
	playerHdl_t newhMaster;

	newhMaster = dpid2commHdl(dp, newIdMaster);
	DPRINT(("dp_election_set_master: newIdMaster %d new hMaster h:%x\n", newIdMaster,newhMaster));
	if (newhMaster != dp->hMaster) {
		dp_result_t err = dptab_addPeer(dp->dt, newhMaster);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("dp_election_set_master: can't add h:%x to peer table, err:%d\n", newhMaster, err));
			return err;
		}
		/* before changing publishers, delete the host's player variable table
		 * so it doesn't migrate; delete table during host callback */
		{
			char key[dptab_KEY_MAXLEN];
			int keylen = 3;
			key[0] = dp_KEY_PLAYER_VARIABLES;
			key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
			key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
			dptab_deletePublisher(dp->dt, key, keylen, dp->hMaster);
		}
		err = dptab_changePublisher(dp->dt, dp->hMaster, newhMaster);
		if (err != dp_RES_OK) {
			DPRINT(("dp_election_set_master: can't change publisher, err:%d\n", err));
			return err;
		}

		dp->hMaster = newhMaster;

#ifdef SEMIRELIABLE_SESSIONS
		/* Battlezone relies on session records being sent reliably, which is a
		 * no-no.  Attempt to simulate this in the case when dpOpen unsubscribes
		 * us from the game server's session table.
		 */
		DPRINT(("dp_election_set_master: flags:%x\n", dp->election_old_session.flags));
		if ((dp->hGameServer != PLAYER_NONE)
		&&  !(dp->election_old_session.flags & dp_SESSION_FLAGS_ISLOBBY)) {
			char key[dptab_KEY_MAXLEN];
			int keylen;

			key[0] = dp_KEY_MYSESSIONS;
			keylen = 1;
			err = dptab_addPublisher(dp->dt, dp->sessions, key, keylen, newhMaster);
			if (err != dp_RES_OK) {
				DPRINT(("dp_election_set_master: can't add h:%x as publisher?, err:%d\n", newhMaster, err));
				dp_assertValid(dp);
				return err;
			}
		}
#endif
	}
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Register a vote.  Called by dpHandleVote.
--------------------------------------------------------------------------*/
static void dp_election_register_vote(dp_t *dp,
	dpid_t idVoter, playerHdl_t hVoter, dpid_t idVotedFor)
{
	dp_election_vote_t *pvote;

	pvote = assoctab_subscript_grow(dp->election_votes, hVoter);
	DPRINT(("dp_election_register_vote: id:%d h:%x votes for id %d; %d handles have voted\n", idVoter, hVoter, idVotedFor, dp->election_votes->n_used));
	assert(pvote);
	pvote->id = idVotedFor;
	pvote->when = dp->now;
}

/*--------------------------------------------------------------------------
 Count votes; see who won, and how many votes he had.
 Returns count; sets *pwinner_id to the winner's id.
--------------------------------------------------------------------------*/
static int dp_election_tally(dp_t *dp, dpid_t *pwinner_id)
{
	assoctab_t *tally;
	int winner_votes;
	dpid_t winner_id;
	int i;

	tally = assoctab_create(sizeof(int));
	assert(tally);
	winner_votes = 0;
	winner_id = dp_ID_NONE;

	/* Iterate downwards through votes 'cause we might delete 'em on the fly */
	for (i=dp->election_votes->n_used-1; i>=0; i--) {
		assoctab_item_t *pe;
		int* pint;
		dp_election_vote_t *pvote;
		dpid_t voter_id;
		playerHdl_t voter_h;

		pe = assoctab_getkey(dp->election_votes, i);
		assert(pe);
		pvote = (dp_election_vote_t *) pe->value;
		voter_h = (playerHdl_t)(pe->key);
		voter_id = dp_commHdl2dpid(dp, voter_h);

		if ((long)(dp->now - pvote->when) > dp_ELECTIONLEN) {
			/* Vote too old.  Delete it.  This is why we iterate downwards. */
			DPRINT(("dp_election_tally: id:%d h:%x vote too old, deleting\n", voter_id, voter_h));
			assoctab_subscript_delete(dp->election_votes, pe->key);
		} else if (voter_id == dp_ID_NONE) {
			DPRINT(("dp_election_tally: unknown h:%x voted, deleting\n", voter_h));
			assoctab_subscript_delete(dp->election_votes, voter_h);
		} else {
			pint = assoctab_subscript_grow(tally, pvote->id);
			assert(pint);
			(*pint)++;
			DPRINT(("dp_election_tally: id:%d h:%x votes for id %d; total %d\n",
				voter_id, voter_h, pvote->id, *pint));
			if (*pint > winner_votes) {
				winner_votes = *pint;
				winner_id = pvote->id;
			}
		}
	}
	assoctab_destroy(tally);

	*pwinner_id = winner_id;
	return winner_votes;
}

/*--------------------------------------------------------------------------
 When votes have been tallied, and a player on this machine has been
 unanimously elected master, this function is called to assume the throne.
--------------------------------------------------------------------------*/
static dp_result_t dp_election_become_master(dp_t *dp, dpid_t winner_id)
{
	dp_sessionContext_t *sp;
	dpid_t highest_id;
	playerHdl_t h;
	dp_packetType_t pkt;
	dp_result_t err;
	int j;

	DPRINT(("dp_election_become_master: We must have won the election.  Setting up to be master.\n"));

	/* If we lost connection to game server, try to reestablish it,
	 * so that the game will reappear on the game server
	 */
	if ((dp->hGameServer == PLAYER_NONE) && dp->adrGameServer[0] && dp->adrGameServer[1] && dp->adrGameServer[2]) {
		char hostName[64];
		err = dpio_printAdr(dp->dpio, dp->adrGameServer, dp->dpio->myAdrLen, hostName,sizeof(hostName));
		if (dp_RES_OK == err) {
			err = dpSetGameServerEx(dp, hostName, dp->defaultSessionType);
			DPRINT(("dp_election_become_master: Restoring connection to server %s returns err:%d\n", hostName, err));
		}
	}

	/* Notify local user code that it is the new master. */
	h = PLAYER_ME;
	pkt = dp_USER_HOST_PACKET_ID;
	dpio_put_reliable(dp->dpio, &h, 1, &pkt, sizeof(pkt), NULL);

	err = dp_election_set_master(dp, winner_id);
	if (err != dp_RES_OK) {
		DPRINT(("dp_election_become_master: Cannot set %d as master\n", winner_id));
		return err;
	}

	/* Build hdl2sess from hosts table */
	sp = (dp_sessionContext_t *)dptab_getTableContext(dp->players,dp_players_cb);
	if ((sp->players != dp->players)
	|| (sp->dp != dp)) {
		DPRINT(("dp_election_become_master: bug: players table context bad?\n"));
	}
	highest_id = 0;
	for (j=dptab_tableSize(dp->hosts); j-- > 0; ) {
		char* hostbuf;
		size_t len;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dpid_t firstId;
		dp_sessionContext_t **spp;

		err = dptab_get_byindex(dp->hosts, j, (void **)&hostbuf, &len, subkey, &subkeylen);
		condition(err == dp_RES_OK, "dp->hosts corrupted");
		firstId = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		if ((firstId != dp_ID_NONE) && (firstId > highest_id))
			highest_id = firstId;
		h = dpid2commHdl(dp, firstId);
		DPRINT(("dp_election_become_master: j %d, firstId %d, h:%x\n", j, firstId, h));
		if (h == PLAYER_NONE) {
			DPRINT(("dp_election_become_master: bug: j %d, firstId %d, h:%x\n", j, firstId, h));
		}
		assert(h != PLAYER_NONE);
		/* Remember what session the fellow is in.
		 * We happen to know that player table context is a dp_sessionContext_t *
		 */
		spp = (dp_sessionContext_t **) assoctab_subscript_grow(dp->hdl2sess, h);
		if (!spp) {
			DPRINT(("dp_election_become_master: can't save handle's session\n"));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
#if 1
		*spp = sp;
#else
		assert(*spp == sp);		/* already done in dp_openHdl_cb */
#endif
		err = dp_subscribe_client(dp, h, sp);
		if (err != dp_RES_OK) {
			DPRINT(("dp_election_become_master: can't subscribe handle\n"));
			return dp_RES_BUG;
		}

		/* if we're standalone master, publish mysessions table */
		if (bIAmStandaloneMaster(dp)) {
			err = dptab_addSubscriber(dp->dt, dp->mysessions, h);
			if (err != dp_RES_OK) {
				DPRINT(("dp_election_become_master: can't subscribe handle, err:%d\n", err));
				dp_assertValid(dp);
				return dp_RES_BUG;
			}
			dp->select_keylen = 0;
			memset(dp->select_key, 0, dptab_KEY_MAXLEN);
		}
	}

	/* If new hosts want to join, start them out at a nice
	 * safe spot in id space.  Same with groups.
	 */
	sp->hostid = (highest_id / dp_PLAYERS_PER_HOST) + 2;
	DPRINT(("dp_election_become_master: new players will start at id:%d\n", sp->hostid * dp_PLAYERS_PER_HOST));
	{	int i; size_t len; int subkeylen;
		char subkey[dptab_KEY_MAXLEN];
		char *buf;
		subkeylen = 2;
		dp->nextGId = dp->firstGId;
		for (i = dp->firstGId + dp_MAX_GROUPS; i >= dp->firstGId; i--) {
			subkey[0] = (char) dpGETSHORT_FIRSTBYTE(i);
			subkey[1] = (char) dpGETSHORT_SECONDBYTE(i);
			if (dptab_get_bykey(dp->groups, subkey, subkeylen,
				(void **)&buf, &len) == dp_RES_OK) {
				dp->nextGId = i++;
				break;
			}
		}
	}

	/* Advertise the session to the world here.
	 * It will be periodically rebroadcast by dpPoll(), and
	 * readvertized by the session table callback if it ever gets erased,
	 * which could happen because of a race condition.
	 */
#ifdef DONT_RETAIN_SESSFLAGS
	dp->election_old_session.flags |= dp_SESSION_FLAGS_ENABLE_NEWPLAYERS;
#endif
	memcpy(dp->election_old_session.adrMaster, dp->dpio->myAdr, dp->dpio->myAdrLen);
	DPRINT(("dp_election_become_master: new adrMaster "));
	printAdr(dp->dpio->myAdrLen, dp->election_old_session.adrMaster);
	DPRINT(("\n"));
	dp->election_old_session.currentPlayers = dpNumPlayers(dp);
	err = dp_setSessionDesc(dp, &dp->election_old_session, 0);
	if (err != dp_RES_OK) {
		DPRINT(("dp_election_become_master: can't announce session\n"));
		return dp_RES_BUG;
	}

	/* Tell all the other machines we have assumed the throne, and it
	 * is safe to send us requests.
	 */
	dpSendVictory(dp);

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 The election is over, for one reason or another, and dp_election_end has
 already been called, so the table of votes etc. is already cleared.
 Annoint the given player master.

 And *blow away any players with lower id's than the winner*, to prevent
 delayed votes for guys about to time out from triggering new election.
--------------------------------------------------------------------------*/
static dp_result_t dp_election_inaugerate_winner(dp_t *dp, dpid_t winner_id)
{
	playerHdl_t winner_h;
	dp_result_t err;

	DPRINT(("dp_election_inaugerate_winner: Winner = %d, election over\n", winner_id));
	err = dp_election_set_master(dp, winner_id);
	if (err != dp_RES_OK) {
		DPRINT(("dp_election_inaugerate_winner: Cannot set %d as master\n", winner_id));
		return err;
	}

	/* Check for special case winners */
	winner_h = dpid2commHdl(dp, winner_id);
	if (winner_h == PLAYER_NONE) {
		DPRINT(("dp_election_inaugerate_winner: bad winner id:%d; quitting\n", winner_id));
		dp->pleaseClose = TRUE;
		return dp_RES_OK;
	} else if (winner_h == PLAYER_ME) {
		err = dp_election_become_master(dp, winner_id);
		if (err != dp_RES_OK) {
			DPRINT(("dp_election_inaugerate_winner: can't become master; quitting session\n", winner_id));
			dp->pleaseClose = TRUE;
			return dp_RES_OK;
		}
	}

	/* Notify user code of change in master if player deltas requested. */
	if (dptab_getTableContext(dp->players, dp_rplayers_cb)) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		char *pbuf;
		dp_playerId_t player;
		size_t len;
		size_t len_used;

		subkey[0] = (char) dpGETSHORT_FIRSTBYTE(winner_id);
		subkey[1] = (char) dpGETSHORT_SECONDBYTE(winner_id);
		subkeylen = 2;
		err = dptab_get_bykey(dp->players, subkey, subkeylen, (void **)&pbuf, &len);
		if (err != dp_RES_OK) {
			DPRINT(("dp_election_inaugerate_winner: Winner %d has no player record? bug\n"));
			return err;
		}
		len_used = dp_unpack_playerId(winner_id, pbuf, &player);
		if (len_used != len) {
			DPRINT(("dp_election_inaugerate_winner: can't unpack player %d. len %d, len_used %d; bug\n", winner_id, len, len_used));
			return dp_RES_BUG;
		}
		err = dpSendObjectDelta(dp, dp_RES_CHANGED, (dp_object_t *) &player, dp->players, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dp_election_inaugerate_winner: can't send object delta, err:%d\n",err));
			return err;
		}
	}

	/*---- start fix to 'delayed vote for crashed player triggers election' -*/
	/* Blow away players with id's lower than the winner.
	 * Note that if an election is triggered, and the current master
	 * is not the lowest id, there must be a migration to the new
	 * lowest id for this to work.
	 */
	if (1) {
		int j;
		dptab_table_t *hosts;

		hosts = dp->hosts;
		for (j=dptab_tableSize(hosts); j-- > 0; ) {
			char* hostbuf;
			size_t len;
			char subkey[dptab_KEY_MAXLEN];
			int subkeylen;
			dpid_t firstId;

			err = dptab_get_byindex(hosts, j, (void **)&hostbuf, &len, subkey, &subkeylen);
			condition(err == dp_RES_OK, "pSess->hosts corrupted");
			firstId = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
			if (firstId < winner_id) {
				playerHdl_t h = dpid2commHdl(dp, firstId);
				DPRINT(("dpHandleVictory: peer id:%d h:%x has lower id than winner id:%d, so closing handle to him\n", firstId, h, winner_id));
				/* Note: this may delete the host.  That's why we iterate
				 * downwards.
				 */
				dpio_closeHdl(dp->dpio, h);
			}
		}
	}
	/*---- end fix to 'delayed vote for crashed player triggers election' -*/

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
In order to provide a more perfect "host migration" scheme, we the
programmers do hereby resolve that:

1. An election begins when the first machine loses contact with the host.

2. When a machine loses contact with the host, or notices that an
election has begun, it votes for who it thinks the new master should be.
The vote consists of the lowest dpid in the session (really, the lowest
key in the hosts table), and is sent to all remaining hosts reliably.

3. The current host is allowed to vote for itself.

4. When any machine notices that some machine has received a majority of
the votes, if that wasn't the machine it voted for, it changes its vote
and votes for the winner, if it thinks the winner exists.  If the winner
doesn't exist from the machine's point of view, the machine kills itself.

5. A majority is defined by the number of players in the session when
the election first began.  This number may be different on different
machines!!??

5a. Halfway through the election, any player who notices that he
has no connection to the candidate with more than half the needed votes
yields to the majority, and kills himself.

6. The election is over when ten seconds has passed or some machine has
received a unanimous victory.  If no machine has received a unanimous
victory, the session is considered lost.

7. Votes for the current host do not trigger an election.  This
prevents reverberating elections.

 If req is NULL, no vote happened, but other processing
 is done; use this e.g. from dp_openHdl_cb when any host is deleted.

 Request is in network byte order, and has had the tag stripped off already.
-------------------------------------------------------------------------*/
static dp_result_t
dpHandleVote(
	dp_t *dp, playerHdl_t hVoter, char *req, size_t reqlen)
{
	dp_election_vote_t *pvote;
	dpid_t winner_id;
	playerHdl_t winner_h;
	int winner_votes;
	dp_result_t err;
	dpid_t idVoter = dp_ID_NONE;

	/* If somebody really voted, register their vote in election_votes[]. */
	if (req) {
		dpid_t idVotedFor;
		dp_session_t sess;

		dpGetSessionDesc(dp, &sess, 0);
		if (sess.flags & (dp_SESSION_FLAGS_ISSERVER | dp_SESSION_FLAGS_MIGRATE_FORBIDDEN)) {
			DPRINT(("dpHandleVote: session not allowed to migrate, ignoring\n"));
			return dp_RES_OK;
		}
		/* Do nothing if bogus packet */
		if (reqlen != 2) {
			DPRINT(("dpHandleVote: Expected len 2, got len %d\n", reqlen));
			return dp_RES_OK;
		}

		/* Do nothing if that handle wasn't in the default session */
		idVoter = dp_commHdl2dpid(dp, hVoter);
		if (idVoter == dp_ID_NONE) {
			DPRINT(("dpHandleVote: h:%x not in default session?\n", hVoter));
			return dp_RES_OK;
		}

		idVotedFor = (unsigned short) dpMAKESHORT(req[0], req[1]);

		/* If no election is yet in progress...  */
		if (!dp->election_size) {
			playerHdl_t hVotedFor = dpid2commHdl(dp, idVotedFor);
			if (hVotedFor == dp->hMaster) {
				/* Register the vote, just in case... */
				dp_election_register_vote(dp, idVoter, hVoter, idVotedFor);
				if (hVotedFor == PLAYER_ME) {
					DPRINT(("dpHandleVote: No election; id:%d h:%x voted for current master (me); I must echo.\n", idVoter, hVoter));
					dpSendVote(dp, idVotedFor, hVoter);
				} else {
					DPRINT(("dpHandleVote: No election; id:%d h:%x voted for current master, ignoring.\n", idVoter, hVoter));
				}
				return dp_RES_OK;
			} else if (hVotedFor == PLAYER_NONE) {
				/* Tricky question: should we start an election when
				 * somebody votes for a player no longer in the session?
				 * This probably only happens when a vote is delayed in
				 * transit past the end of an election.
				 *
				 * FIXME Hidden bug: this won't catch votes for players
				 * which are really gone but haven't timed out yet :-(
				 */
				DPRINT(("dpHandleVote: No election; id:%d h:%x voted for bogus id:%d; ignoring.\n", idVoter, hVoter, idVotedFor));
				return dp_RES_OK;
			}

			/* otherwise his vote will start a new election. */
		}

		dp_election_start(dp, FALSE);

		/* Register the vote */
		dp_election_register_vote(dp, idVoter, hVoter, idVotedFor);
	}

	/* Find out who had the most votes and how many he had */
	winner_votes = dp_election_tally(dp, &winner_id);

	DPRINT(("dpHandleVote: t:%d winner_votes %d winner_id %d election_size %d hosts %d\n", dp->now, winner_votes, winner_id, dp->election_size, dptab_tableSize(dp->hosts)));

	/* Election abandoned if we no longer have a quorum */
	if (dptab_tableSize(dp->hosts) < dp->election_size) {
		DPRINT(("dpHandleVote: lost quorum, so election cancelled\n"));
		dp->pleaseClose = TRUE;
		return dp_RES_OK;
	}

	/* Election ends when we have enough votes to win,
	 * or when current host has enough votes to win,
	 * or when a victor sends us a victory packet (see dpHandleVictory).
	 * This keeps dead guys from winning the election.
	 */
	winner_h = dpid2commHdl(dp, winner_id);
	if ((winner_votes >= dp->election_size)
		  && ((winner_h == PLAYER_ME) || (winner_h == dp->hMaster))) {

		/* Terminate the election.  This invalidates dp->election_size etc. */
		dp_election_end(dp);

		/* Do whatever it is we do after an election */
		err = dp_election_inaugerate_winner(dp, winner_id);

		/* Election is over, so we're done. */
		return err;
	}

	/* Election still in progress. */

	/* If the candidate we voted for is winning, just wait for next vote. */
	pvote = assoctab_subscript(dp->election_votes, PLAYER_ME);
	if (pvote && (pvote->id == winner_id))
		return dp_RES_OK;

	/* If the candidate we voted for isn't winning, go with the winner. */
	if (winner_votes > dptab_tableSize(dp->hosts)/2) {
		playerHdl_t winner_h;
		char subkey[dptab_KEY_MAXLEN];
		char *pbuf;
		size_t len;
		dp_result_t err;
		clock_t age;

		/* If the winner doesn't exist, die */
		winner_h = dpid2commHdl(dp, winner_id);
		subkey[0] = (char) dpGETSHORT_FIRSTBYTE(winner_id);
		subkey[1] = (char) dpGETSHORT_SECONDBYTE(winner_id);
		err = dptab_get_bykey(dp->hosts, subkey, 2, (void **)&pbuf, &len);
		if ((winner_h == PLAYER_NONE) || (err != dp_RES_OK)) {
			long left;
			left = dp->election_yield_deadline - dp->now;
			if (left <= 0) {
				DPRINT(("dpHandleVote: I have no handle to the winner id:%d; yielding.\n", winner_id));
				dp->pleaseClose = TRUE;
			} else {
				DPRINT(("dpHandleVote: t:%d; I have no handle to the winner id:%d; will yield at t:%d, %d ticks from now\n", dp->now, winner_id, dp->election_yield_deadline, left));
			}
			return dp_RES_OK;
		}

		age = dpio_getHdlAge(dp->dpio, winner_h);
		if (age < DP_ELECTION_MAXAGE) {
			DPRINT(("dpHandleVote: Voting for Winner %d\n", winner_id));
			err = dp_election_set_master(dp, winner_id);
			if (err != dp_RES_OK) {
				DPRINT(("dpHandleVote: Cannot set %d as master\n", winner_id));
				dp->pleaseClose = TRUE;
				return dp_RES_OK;
			}
			dpSendVote(dp, winner_id, PLAYER_BROADCAST);
			return dp_RES_OK;
		}
	}

	/* Otherwise, if we weren't just listening to ourselves vote,
	 * pick the best candidate, and vote again.
	 * (We might vote for the same guy again, but that's ok.)
	 */
	if (hVoter != PLAYER_ME) {
		DPRINT(("dpHandleVote: Electing a master\n"));
		dpElectMaster(dp);
	}
	return dp_RES_OK;
}

/*-------------------------------------------------------------------------
 Either we have noticed start of an election, or we are changing our vote.
 Send a vote for the given candidate to the given destination.
 If destination is PLAYER_BROADCAST, send it to all hosts in the current
 session.
--------------------------------------------------------------------------*/
static dp_result_t dpSendVote(dp_t *dp, dpid_t candidate, playerHdl_t dest)
{
	struct {
		dp_packetType_t   tag;
		char body[2];
	} PACK pkt;

	dp_result_t		err = 0;
	playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */
	int ndests;
	int i;

	precondition(dp != NULL);
	precondition(dp->dpid2commHdl);
	dp_assertValid(dp);

	pkt.tag = dp_VOTE_PACKET_ID;
	pkt.body[0] = (char) dpGETSHORT_FIRSTBYTE(candidate);
	pkt.body[1] = (char) dpGETSHORT_SECONDBYTE(candidate);

	DPRINT(("dpSendVote: t:%d, Candidate=%d, dest = h:%x\n", dp->now, candidate, dest ));

	/* Generate list of destination handles */
	if (dest == PLAYER_BROADCAST)
		ndests = dp_getBroadcastHdls(dp, dests, dp_SEND_RELIABLE);
	else {
		ndests = 1;
		dests[0] = dest;
	}
	for (i=0; i<ndests; i++) {
		playerHdl_t h = dests[i];
		/* Have to make sure the clock is up to date here */
		dp->now = eclock();
		err = dpio_put_reliable(dp->dpio, &h, 1, &pkt, sizeof(pkt), NULL);
		if (err != dp_RES_OK) {
			DPRINT(("dpSendVote: can't send vote to h:%x!\n", h));
		}
	}

	/* Send it to self, too */
	if (dest == PLAYER_BROADCAST)
		dpHandleVote(dp, PLAYER_ME, &pkt.body[0], sizeof(pkt.body));

	dp_assertValid(dp);
	return err;
}

/*-------------------------------------------------------------------------
 A peer was deleted during an election.
 Delete any votes he cast.
-------------------------------------------------------------------------*/
static void dp_election_peer_deleted(dp_t *dp, playerHdl_t h)
{
	if (!dp->election_size)
		return;

	/* If he voted, delete his vote, and recount. */
	if (!assoctab_subscript_delete(dp->election_votes, h)) {
		DPRINT(("dp_election_peer_deleted: peer h:%x deleted during election, recount...\n", h));
		dpHandleVote(dp, PLAYER_NONE, NULL, 0);
	}
}

/*-------------------------------------------------------------------------
 We won! Gloat! This tells all the other players that it's ok to pester
 us with requests.
--------------------------------------------------------------------------*/
static dp_result_t dpSendVictory(dp_t *dp)
{
	struct {
		dp_packetType_t   tag;
	} PACK pkt;

	dp_result_t		err;
	playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */
	int ndests;

	precondition(dp != NULL);
	precondition(dp->dpid2commHdl);
	dp_assertValid(dp);

	pkt.tag = dp_VICTORY_PACKET_ID;

	DPRINT(("dpSendVictory: t:%d\n", dp->now));

	/* Generate list of destination handles */
	ndests = dp_getBroadcastHdls(dp, dests, dp_SEND_RELIABLE);
	if (ndests >= 1) {
		/* Have to make sure the clock is up to date here */
		dp->now = eclock();
		err = dpio_put_reliable(dp->dpio, dests, ndests, &pkt, sizeof(pkt), NULL);
	}

	dp_assertValid(dp);
	return err;
}

/*-------------------------------------------------------------------------
 The new master has assumed the throne.  Pester him with all sorts of
 requests.
--------------------------------------------------------------------------*/
static dp_result_t dpHandleVictory(dp_t *dp, playerHdl_t src, size_t reqlen)
{
	dp_result_t		err;

	precondition(dp != NULL);
	dp_assertValid(dp);

	/* Do nothing if bogus packet */
	if (reqlen != 0) {
		return dp_RES_OK;
	}
	DPRINT(("dpHandleVictory: t:%d, src h:%x\n", dp->now, src));

	if ((src != dp->hMaster) || !dp->pSess) {
		DPRINT(("dpHandleVictory: Bug: Bogus host\n"));
		return dp_RES_OK;
	}

	/* If an election is still in progress, end it one way or
	 * the other.  Accept the new host if he was winning.
	 */
	if (dp->election_size) {
		dpid_t winner_id;
		int winner_votes = dp_election_tally(dp, &winner_id);
		int election_size = dp->election_size;

		dp_election_end(dp);

		if (dpid2commHdl(dp, winner_id) == src) {
			DPRINT(("dpHandleVictory: believing winner id:%d\n", winner_id));
			/* Do whatever it is we do after an election */
			err = dp_election_inaugerate_winner(dp, winner_id);
			if (err != dp_RES_OK)
				return err;
		} else {
			DPRINT(("dpHandleVictory: error: winner_id:%d doesn't map to src h:%x\n", winner_id, src));
			dp->pleaseClose = TRUE;
			return dp_RES_OK;
		}
	}

	/* FIXME: this stuff might fit better in dp_election_inaugerate_winner,
	 * but it's ok here.
	 */

	/* Send him our player table */
	err = dp_subscribe_host(dp, dp->hMaster, dp->pSess);
	if (err != dp_RES_OK) {
		DPRINT(("dpHandleVictory: Cannot subscribe host\n"));
		return err;
	}

	/* If connected to standalone master, also send him our session table */
	if (bIAmConnectedToStandaloneMaster(dp)) {
		err = dptab_addPublisher(dp->dt, dp->sessions, dp->select_key,
									dp->select_keylen, dp->hMaster);
		if (err != dp_RES_OK) {
			DPRINT(("dpHandleVictory: Cannot subscribe host, err %d\n", err));
			return err;
		}
	}

	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Find the host with the lowest id.
 If maxAge is nonzero, only return hosts whose handle has age < maxAge.
 Return his host id.
--------------------------------------------------------------------------*/
static dpid_t dp_findLowestHost(dp_t *dp, clock_t maxAge)
{
	int j;
	dpid_t lowest;
	dptab_table_t *hosts;

	lowest = dp_ID_NONE;
	hosts = dp->hosts;
	for (j=dptab_tableSize(hosts); j-- > 0; ) {
		char* hostbuf;
		size_t len;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		dpid_t firstId;

		dp_result_t err = dptab_get_byindex(hosts, j, (void **)&hostbuf, &len, subkey, &subkeylen);
		condition(err == dp_RES_OK, "pSess->hosts corrupted");
		firstId = (dpid_t) dpMAKESHORT(subkey[0], subkey[1]);
		DPRINT (("dp_findLowestHost: lowest = %d, firstId = %d\n", lowest, firstId));
		if (lowest > firstId) {
#define BADSTATE (dpio_STATE_FIN_SENT | dpio_STATE_FIN_RCVD | dpio_STATE_FIN_RCVDACK | dpio_STATE_RST_RCVD | dpio_STATE_CLOSE_QFULL)
			/* Don't vote for a handle that is closing! */
			playerHdl_t h = dpid2commHdl(dp, firstId);
			short state = dpio_getHdlState(dp->dpio, h);
			DPRINT(("dp_findLowestHost: considering id %d h:%x; state %x; bad state %x\n", firstId, h, state, state&BADSTATE));
			if (state & BADSTATE)
				continue;

			if (maxAge) {
				/* Don't vote for a handle that is too old */
				clock_t age = dpio_getHdlAge(dp->dpio, h);
				DPRINT(("dp_findLowestHost: considering id %d h:%x; age %d <? maxAge %d\n", firstId, h, age, maxAge));
				if (age > maxAge)
					continue;
			}
			lowest = firstId;
		}
	}

	return lowest;
}

/*----------------------------------------------------------------------
 Elect a new master!
 Old master has died.
 (We know either because he timed out, or because he suddenly changed
  karma on us.)
 Choose next highest dpid to become the master (or kill the session if
 it's not allowed to migrate).
 Tricky... need to change our whole lifestyle.
 Election may already be in progress when this is called.
----------------------------------------------------------------------*/
static void dpElectMaster(
	dp_t *dp)
{
	dpid_t lowest;
	dp_session_t sess;
	dp_election_vote_t *pvote;

	DPRINT(("dpElectMaster: Old master h:%x\n", dp->hMaster));

	dp_assertValid(dp);

	/* If the session can't be migrated, kill it */
	dpGetSessionDesc(dp, &sess, 0);
	if (0 && sess.flags & dp_SESSION_FLAGS_MIGRATE_FORBIDDEN) {	/* FIXME!!! */
		dpSendSessionLostPacket(dp);

		/* Close the session */
		dpClose(dp);

		/* Nothing more to do here... */
		dp_assertValid(dp);
		return;
	}

	/* Find the host with the lowest id */
	lowest = dp_findLowestHost(dp, DP_ELECTION_MAXAGE);
	if (lowest == dp_ID_NONE) {
		DPRINT(("dpElectMaster: no hosts young enough!\n"));
		return;
	}

	pvote = NULL;
	if (dp->election_size)
		pvote = assoctab_subscript(dp->election_votes, PLAYER_ME);
	DPRINT(("dpElectMaster: old vote %d, new vote %d\n",
			(pvote ? pvote->id : -1), lowest));
	if (!pvote || (pvote->id != lowest)) {
		dp_election_set_master(dp, lowest);
		dpSendVote(dp, lowest, PLAYER_BROADCAST);
	}
}

/*----------------------------------------------------------------------
 Internal.
 Send system packets if needed.
 Time out unresponsive machines.
 Call frequently- e.g. on every dpReceive.
 Returns dp_RES_OK on success.
 Returns dp_RES_NETWORK_NOT_RESPONDING if transmit buffers become too full.
 Returns dp_RES_HOST_NOT_RESPONDING if game server handle resets.
----------------------------------------------------------------------*/
static dp_result_t dpPoll(
	dp_t   *dp,
	clock_t	now)
{
	playerHdl_t h;
	dp_result_t err;
	dp_result_t return_value = dp_RES_OK;
	int nhdls= -1;
	int beacon_expired;
	int beacon2_expired;
	int beacon4_expired;

	dp_assertValid(dp);

	/* Check the beacons and reset them */
	beacon_expired = ((long)(dp->next_beacon - now) < 0);
	if (beacon_expired)
		dp->next_beacon = now + dp->beacon_interval;
	beacon2_expired = ((long)(dp->next_beacon2 - now) < 0);
	if (beacon2_expired)
		dp->next_beacon2 = now + dp->beacon_interval*2;
	beacon4_expired = ((long)(dp->next_beacon4 - now) < 0);
	if (beacon4_expired)
		dp->next_beacon4 = now + dp->beacon_interval*4;

	/* Every beacon... */
	if (beacon_expired) {
		DPRINT(("dpPoll:  now %d, next_beacon %d\n", now, dp->next_beacon));
		/* Give player table system some airtime. */
		dptab_update(dp->dt);
		/* Give serverping system some airtime. */
		err = dpEnumServersPoll(dp);

		/* If this app isn't calling dpFlush(), switch it to the old behavior */
		if (dp->dpio->appWillFlush
		&&  ((long)(dp->now - dp->appLastFlushed) > ECLOCKS_PER_SEC/4)) {
			DPRINT(("dpPoll: t:%d - no dpFlush since t:%d, switching to old behavior\n", dp->now, dp->appLastFlushed));
			/* flush in case anything is in the gather buffer */
			dpio_flush(dp->dpio);
			dp->dpio->appWillFlush = FALSE;
		}
	}

	/* Every call */
	/* Retransmit reliable datagrams if necessary, and send keepalives. */
	dpio_update(dp->dpio);

	/* Every 2nd beacon... */

	/* Check for hosts that have actively reset their connection to us */
	if (dp->check_timeout || beacon2_expired) {
		h = dpio_findTimedOutHost(dp->dpio);
		DPRINT(("dpPoll: t:%d, findTimedOutHost returns h:%x\n", now, h));

		/* check again later if we find  one now */
		dp->check_timeout = (h != PLAYER_NONE);
		if (h != PLAYER_NONE) {

			/* If we found a dying or inactive handle, close it.
			 * Caution: This triggers a callback that might do dangerous
			 * stuff; see dp_openHdl_cb().
			 */
			dpio_closeHdlImmed(dp->dpio, h);

			/* Tell the user process if we timed out the game server */
			if (h == dp->hGameServer) {
				DPRINT(("dpPoll: Game server h:%x connection closed!\n", h));
				return_value = dp_RES_HOST_NOT_RESPONDING;
				dpSetGameServer(dp, NULL);
				/* Tell dp_unpack_session there is no default session type */
				dp->defaultSessionType = 0;
				/* Careful - don't clobber return_value, it must get back */
			}

		}
	}
	if (beacon2_expired) {
		/* End enumServers activity if deadline passes. */
		if (dp->enumServersEx_callback &&
			((long)(dp->enumServers_deadline - now) < 0))
			dp_endEnumServers(dp);

		/* End enumSessions activity if deadline passes. */
		DPRINT(("dpPoll: checking enumSessions timeout: cb %p, t:%d, deadline %d\n",
			dp->enumSessions_callback, dp->now, dp->enumSessions_deadline));
		if (dp->enumSessions_callback && (long)(dp->enumSessions_deadline - now) < 0) {
			dpEnumSessionsCallback_t cb = dp->enumSessions_callback;
			long timeout_dummy;
			/*DPRINT(("dpPoll: enumSessions timeout: deadline %d, now %d\n", dp->enumSessions_deadline, now));*/
			cb(NULL, &timeout_dummy, 0, dp->enumSessions_context);
			dp->enumSessions_callback = NULL;
		}
	}

	/* Every 4th beacon... */
	if (beacon4_expired) {
#ifdef DEFER_SESSION_UPDATE
		int i;
		/* If we're the master, inform the world of the change in the
		 * number of players.  We can only tell if we're the master of
		 * a particular (possibly cast-off) session by seeing if it's
		 * in our mysessions table.  Can't check hMaster...
		 */
		/*DPRINT(("dpPoll: checking numplayers of %d sessions\n", dp->sessionContexts->n_used));*/
		for(i = 0; i < dp->sessionContexts->n_used; i++) {
			char * sessbuf;
			size_t sesslen;
			dp_sessionContext_t *pSess;
			dp_sessionContext_t **ppS = (dp_sessionContext_t **)dynatab_subscript(
						dp->sessionContexts, i);
			assert(ppS);
			if (!ppS) break;
			pSess = *ppS;
			/*DPRINT(("dpPoll: old_numplayers %d, new numplayers %d, key %s\n",
				pSess->old_numplayers, dptab_tableSize(pSess->players),
				key2a(pSess->sess_subkey, pSess->sess_subkeylen)));*/
			if (pSess->old_numplayers == dptab_tableSize(pSess->players))
				continue;
			err = dptab_get_bykey(dp->mysessions, pSess->sess_subkey, pSess->sess_subkeylen, (void **)&sessbuf, &sesslen);
			if (err == dp_RES_OK) {
				char rawbuf[sizeof(dp_session_t)];
				size_t rawbuflen;
				dp_session_t sess;
				/* Must unpack when retrieving records from dptab */
				dp_unpack_session(dp, pSess->sess_subkey, pSess->sess_subkeylen, sessbuf, sesslen, &sess);
				sess.currentPlayers = (short) dptab_tableSize(pSess->players);
				pSess->old_numplayers = sess.currentPlayers;
				/* Must pack when writing to dptab */
				rawbuflen = dp_pack_session(dp, 0, &sess, rawbuf);
				err = dptab_set(dp->dt, dp->mysessions, pSess->sess_subkey, pSess->sess_subkeylen, rawbuf, rawbuflen, 4, PLAYER_ME);
				DPRINT(("dpPoll: currentPlayers now %d; dptab_set returns err:%d\n", sess.currentPlayers, err));
			}
		}
#endif
		if (dp->dpio->driverinfo->capabilities& comm_DRIVER_NO_BROADCAST) {
			/* Stuff that only happens if there is a gameserver */
			if (dp->hGameServer != PLAYER_NONE) {
				tserv_poll(dp->tserv);
			}
		} else {
			/* Stuff that only happens if there's no gameserver */
#ifdef IGNORE_CLOSEDSESS
#ifndef USE_dp_enableNewPlayers
			dp_session_t sess;
			dpGetSessionDesc(dp, &sess, 0);
#endif
#endif
			if ((dp->hMaster == PLAYER_ME)
#ifdef IGNORE_CLOSEDSESS
#ifdef USE_dp_enableNewPlayers
			&&  dp->enableNewPlayers
#else
			&& (sess.flags & dp_SESSION_FLAGS_ENABLE_NEWPLAYERS)
#endif
#endif
			) {
				char *sessbuf;
				size_t len;
#if 0
				dp_session_t s;
#endif

				/* Punch our session into the sessions table to keep it
				 * from timing out
				 */
				err = dptab_get_bykey(dp->mysessions, dp->sess_subkey, dp->sess_subkeylen, (void **)&sessbuf, &len);
				if (err == dp_RES_OK) {
#if 0
					s = *sessbuf;
					dpSwapSessionPacket(&s);	/* swap when reading from dptab! */
					/*DPRINT(("dpPoll: copying session record; buf %p, len %d, name %s, type %d\n", sessbuf, len, s.sessionName, s.sessionType));
					dumpBuf((char *)sessbuf, len);*/
#endif
					err = dptab_set(dp->dt, dp->sessions, dp->sess_subkey, dp->sess_subkeylen, sessbuf, len, 1, PLAYER_ME);
				}
				if (err != dp_RES_OK) {
					DPRINT(("dpPoll: bug: copying mysession to session\n"));
				}

				/* Broadcast our session on a LAN */
#if 0
				DPRINT(("dpPoll: Broadcasting session; name %s, type %d, caps %x\n",
						s.sessionName, s.sessionType,
						dp->dpio->driverinfo->capabilities));
#endif
				err = dptab_send(dp->dt, dp->mysessions, dp->sess_subkey, dp->sess_subkeylen, PLAYER_BROADCAST, 1);

			}

			/* Time out old sessions - ten seconds is the limit */
			/* Oops; Loren absolutely depends on session not dropping out of
			 * list and it can take 30 seconds to launch, so add extra 30 sec? */
			dptab_delete_byAge(dp->dt, dp->sessions, 10);
		}
	}

	if (dp->election_size)
		dp_election_poll(dp);

	dp_assertValid(dp);
	return return_value;
}

/*----------------------------------------------------------------------
 respond to the given ping packet
----------------------------------------------------------------------*/
static dp_result_t dpHandlePingPacket(
	dp_t *dp,
	unsigned char *buf,
	size_t len,
	playerHdl_t dest,
	dp_packetType_t tag)
{
	dp_assertValid(dp);

	if ((dest == PLAYER_NONE)
	|| (dest == PLAYER_ME)
	|| (dest == PLAYER_UNKNOWN)
	|| (dest == PLAYER_BROADCAST)) {
		DPRINT(("dpHandlePingPacket: unknown or bad source h:%x\n", dest));
		dp_assertValid(dp);
		return dp_RES_OK;
	}

	*(short *)buf = tag;

	DPRINT(("dpHandlePingPacket: set tag; sending to %d; pkt is ", dest));
	  dumpBuf(buf, len);
	dp_assertValid(dp);
	return dpio_put_unreliable(dp->dpio, &dest, 1, buf, len, NULL);
}

/*----------------------------------------------------------------------
 Call periodically to handle sending ping packets and report results
----------------------------------------------------------------------*/
static void dpPollPing(dp_t *dp)
{
	unsigned short len;

	/*DPRINT(("dpPollPing: now %d, next %d, dest h:%x\n",
		dp->now, dp->ping.next_send_time, dp->ping_dest));*/

	dp_assertValid(dp);

	if (dp->ping_dest == PLAYER_NONE) {
		dp_assertValid(dp);
		return;
	}

	if (dp->ping.n_sent >= dp->ping.n_total) {
		if ((long)(dp->now - dp->ping.deadline) > 0) {
			DPRINT(("dpPollPing: done listening pings to dest %d\n", dp->ping_dest));
			if (dp->ping.cb) {
				int loss_percent = 100*(dp->ping.n_total - dp->ping.n_got) / dp->ping.n_total;
				time_t average = 0;
				int i;

				DPRINT(("dpPollPing: for()\n"));
				for (i=0; i<dp_PING_MAX_NTOTAL; i++) {
					DPRINT(("i %d\n", i));
					if (dp->ping.got_at[i]) {
						DPRINT(("got_at %ld, sent_at %ld, dif %ld\n",
							dp->ping.got_at[i],  dp->ping.sent_at[i],
							dp->ping.got_at[i] - dp->ping.sent_at[i]));
						average += (dp->ping.got_at[i] - dp->ping.sent_at[i]);
						DPRINT(("average: %ld\n", average));
					}
				}
				DPRINT(("average: %ld\n", average));
				if (dp->ping.n_got != 0)
					average = ((average*1000) / dp->ping.n_got) / dp->dpio->clocksPerSec;
				else
					average = 0;
				DPRINT(("average: %ld\n", average));
				dp->ping.cb(dp->ping.karma, average, loss_percent);
			}
			dp->ping.cb = NULL;
			dp->ping_dest = PLAYER_NONE;
		}
	} else if ((long)(dp->now - dp->ping.next_send_time) > 0) {
		struct {
			dp_packetType_t tag;
			dp_ping_packet_t pingbuf;
		} PACK pkt;
		DPRINT(("dpPollPing: sending ping to dest %d\n", dp->ping_dest));
		pkt.pingbuf.karma = dp->ping.karma;
		dp->ping.sent_at[dp->ping.n_sent] = dp->now;
		pkt.pingbuf.pktnum = (unsigned short) dp->ping.n_sent++;
		pkt.pingbuf.len = 0;
		pkt.tag = dp_PING_PACKET_ID;
		len = sizeof(pkt.tag)+sizeof(pkt.pingbuf)-sizeof(pkt.pingbuf.data)+0;
		/* dumpBuf((char *)&pkt, len); */
		/* call dpSwapPingPacket to byte swap pkt.pingbuf */
		dpSwapPingPacket(&pkt.pingbuf);
		dpio_put_unreliable(dp->dpio, &dp->ping_dest, 1, &pkt, len, NULL);
		dp->ping.next_send_time = dp->now + dp->dpio->clocksPerSec / 5;
	}
	dp_assertValid(dp);
}

/*----------------------------------------------------------------------
 Look up the session type and id of the session the given uid most
 recently tried to join.
----------------------------------------------------------------------*/
dp_result_t dp_uid2sessid(dp_t *dp, dp_uid_t uid, char *sessidbuf, int *sessidlen, dp_species_t *sessType)
{
	char subkey[dptab_KEY_MAXLEN];
	char *pbuf;
	dp_result_t err;
	size_t len;

	subkey[0] = dpGETLONG_FIRSTBYTE(uid);
	subkey[1] = dpGETLONG_SECONDBYTE(uid);
	subkey[2] = dpGETLONG_THIRDBYTE(uid);
	subkey[3] = dpGETLONG_FOURTHBYTE(uid);

	if (!dp->uid2sessionid) {
		DPRINT(("dp_uid2sessid: no uid2sessionid tab\n"));
		return dp_RES_BUG;
	}

	err = dptab_get_bykey(dp->uid2sessionid, subkey, sizeof(uid), (void **)&pbuf, &len);
	if (err == dp_RES_OK) {
		memcpy(sessidbuf, pbuf, len - 2);
		*sessType = dpMAKESHORT(pbuf[len - 2], pbuf[len - 1]);
	}
	*sessidlen = len - 2;
	return err;
}

/*----------------------------------------------------------------------
 Remember the session type and id of the session the given uid most
 recently tried to join.
 Silently ignores calls with uid == dp_UID_NONE
----------------------------------------------------------------------*/
dp_result_t dp_sessid4uid(dp_t *dp, dp_uid_t uid, const unsigned char *sessid, int sessidlen, dp_species_t sessType)
{
	dp_result_t err;
	char subkey[dptab_KEY_MAXLEN];
	char sesstypeAndId[dptab_KEY_MAXLEN + sizeof(dp_species_t)];

	if (uid == dp_UID_NONE)
		return dp_RES_OK;

	subkey[0] = dpGETLONG_FIRSTBYTE(uid);
	subkey[1] = dpGETLONG_SECONDBYTE(uid);
	subkey[2] = dpGETLONG_THIRDBYTE(uid);
	subkey[3] = dpGETLONG_FOURTHBYTE(uid);

	if (!dp->uid2sessionid) {
		char key[dptab_KEY_MAXLEN];

		key[0] = dp_KEY_UID2SESSIONID;
		err = dptab_createTable(dp->dt, &dp->uid2sessionid, key, 1, sessidlen + sizeof(dp_species_t), NULL,NULL,NULL,NULL);
		if ((err != dp_RES_OK) && (err != dp_RES_ALREADY)) {
			DPRINT(("dp_sessid4uid: can't create uid2sessionid tab\n"));
			return err;
		}
	}

	memcpy(sesstypeAndId, (void *)sessid, sessidlen);
	sesstypeAndId[sessidlen] = (char) dpGETSHORT_FIRSTBYTE(sessType);
	sesstypeAndId[sessidlen+1] = (char) dpGETSHORT_SECONDBYTE(sessType);
	/* Record the session type and id last entered by this uid */
	err = dptab_set(dp->dt, dp->uid2sessionid, subkey, sizeof(uid), sesstypeAndId, sessidlen + sizeof(dp_species_t), 2, PLAYER_ME);
	if (err != dp_RES_OK) {
		DPRINT(("dp_sessid4uid: can't record session for uid %d\n", uid));
		return err;
	} else {
		DPRINT(("dp_sessid4uid: uid:%d attempting to join session %s\n",
				uid, key2a(sessid, sessidlen)));
	}
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Receive a packet.
 If the packet was sent with dp_SEND_RELIABLE, acknowledge it.
 Also triggers internal housekeeping.  Must be called frequently.
 Caller must initialize *size with the size of the buffer before calling.
 idFrom and idTo must be set to the addresses of uninitialized dpid_t's.
 On return, *size is set to the number of bytes received, and
 *idFrom and *idTo are set to the dpid's of the sender and recipient.
 Returns dp_RES_BUG if dp was invalid.
 Returns dp_RES_OK if a user packet was receieved.
 Returns dp_RES_EMPTY if NO user packet was receieved.
 Returns dp_RES_BAD if a user packet with a bad envelope was receieved.
 Returns dp_RES_NETWORK_NOT_RESPONDING if transmit buffers become too full.
 Returns dp_RES_HOST_NOT_RESPONDING if game server handle resets.
 Returns dp_RES_MODEM_NO_RESPONSE if modem connection lost is detected.

 If flags argument is nonzero, envelope on packet is ignored,
 *idFrom gets the playerHdl_t of the sender of the message,
 and *idTo is set to dp_ID_NONE.  This is meant for game server
 development until we figure our how to do it right.
----------------------------------------------------------------------*/
static dp_result_t dp_receive(dp_t *dp, dpid_t *idFrom, dpid_t *idTo, int flags, void *buffer, size_t *size);

DP_API dp_result_t dpReceive(
	dp_t   *dp,
	dpid_t *idFrom,
	dpid_t *idTo,
	int    flags,
	void *buffer,
	size_t *size)
{
	int i;
	dp_result_t err = dp_RES_AGAIN;
	clock_t			new_now;
	size_t orig_size;

	dp_assertValid(dp);
	assert((dp->hosts == NULL) ||
		(dptab_getTableContext(dp->hosts, dp_hosts_cb) != NULL));

	if (dp == NULL) {
		DPRINT(("bug: dp NULL\n"));
		return dp_RES_BUG;
	}
	ptimer_Enter(PTIMER_ECLOCK, "eclock timer");
	new_now = eclock();
	ptimer_Exit(PTIMER_ECLOCK, 0);
	if ((new_now - dp->now) > (dp->dpio->clocksPerSec / 5)) {
		DPRINT(("dpReceive: time elapsed since last call %d ms\n",
				(1000 * (new_now - dp->now)) / dp->dpio->clocksPerSec));
		if (dp->dpReceive_prevcall) {
#if 0
			/* Abort if too much time elapsed. */
			if ((new_now - dp->now) > (dp->dpio->clocksPerSec))
				return dp_RES_BAD;
#endif
		} else
			dp->dpReceive_prevcall = 1;
	}
	dp->now = new_now;
	orig_size = *size;
	ptimer_Enter(PTIMER_RECEIVE, "dpReceive timer");
	for (i = 0; (i < 25) && (err == dp_RES_AGAIN); i++) {
		*size = orig_size;
		ptimer_Enter(PTIMER_RECEIVE_FINE, "dp_receive timer");
		err = dp_receive(dp, idFrom, idTo, flags, buffer, size);
		ptimer_Exit(PTIMER_RECEIVE_FINE, 0);
	}
	ptimer_Exit(PTIMER_RECEIVE, i-1);
	if (err == dp_RES_AGAIN)
		return dp_RES_EMPTY;
	if (err == dp_RES_OK) {
		DPRINT(("dpReceive: got user packet after %d loop\n", i));
	}
	return err;
}

/*----------------------------------------------------------------------
 Send a player delta containing the current latency.
----------------------------------------------------------------------*/
static dp_result_t dp_sendLatencyDelta(dp_t *dp, playerHdl_t h)
{
	dpid_t id;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	char *pbuf;
	dp_playerId_t player;
	size_t len;
	size_t len_used;
	dp_result_t err;

	id = dp_commHdl2dpid(dp, h);
	if (id == dp_ID_NONE) {
		DPRINT(("dpReceive: h:%x has no id for latency delta\n", h));
		return dp_RES_EMPTY;
	}
	subkey[0] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[1] = (char) dpGETSHORT_SECONDBYTE(id);
	subkeylen = 2;
	err = dptab_get_bykey(dp->players, subkey, subkeylen, (void **)&pbuf, &len);
	if (err != dp_RES_OK) {
		DPRINT(("dpReceive: bug: id %d has no player record?\n", id));
		return dp_RES_BUG;
	}
	len_used = dp_unpack_playerId(id, pbuf, &player);
	if (len_used != len) {
		DPRINT(("dpReceive: bug: can't unpack player %d. len %d, len_used %d\n", id, len, len_used));
		return dp_RES_BUG;
	}
	err = dpSendObjectDelta(dp, dp_RES_CHANGED, (dp_object_t *) &player, dp->players, subkey, subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dpReceive: dpSendObjectDelta returns err:%d\n",err));
	}
	return err;
}

/*----------------------------------------------------------------------
Same description as for dpReceive() except that it returns dp_RES_AGAIN
  if housekeeping/system packet was received.  Note that  dp_RES_AGAIN
  is a private return code hidden from user code by the wrapper function.
----------------------------------------------------------------------*/
static dp_result_t dp_receive(
	dp_t   *dp,
	dpid_t *idFrom,
	dpid_t *idTo,
	int    flags,
	void *buffer,
	size_t *size)
{
	dp_envelope_t	*env;
	dp_result_t		err;

	playerHdl_t		pktsrc;
	size_t			pktlen;
	int				get_flags;
	struct dpReceivePkt_s {
		dp_packetType_t tag;
		union {
			dp_session_packet_t body1;
			dp_join_packet_t join;
			dp_leave_packet_t leave;
			dp_select_sess_packet_t selectsess;
			dp_ping_packet_t ping;
			dp_ping_resp_packet_t pingresp;
			dptab_subscribe_packet_t subscribe;
			dptab_unsubscribe_packet_t unsubscribe;
			dp_error_packet_t error;
			dp_user_playerData_packet_t pData;
			dp_request_open_packet_t requestOpen;
			dp_indirect_join_packet_t indirectJoin;
			tserv_packet_t tservpkt;
			dp_account_packet_t	account;
		} body;
	} PACK *pkt = (struct dpReceivePkt_s *)buffer;

	/* If our host record was deleted, or if the master's handle
	 * timed out or was reset, close the session.
	 */
	if (dp->pleaseClose) {
		DPRINT(("dpReceive: closed session\n"));
		dp->pleaseClose = FALSE;
		dpClose(dp);
		dpSendSessionLostPacket(dp);
		return dp_RES_AGAIN;
	}

	/* If user tried to open a lobby before we'd established our
	 * connection to the game server, the sendJoin was deferred until
	 * now (because we don't find out our real address until the SYN
	 * comes back).
	 */
	if (dp->pleaseJoinAny && dp->dpio->publicAddressKnown) {
		dp->pleaseJoinAny = FALSE;
		if (dp->dpOpen_cb) {
			short flags = dp->pleaseJoinAnyMask | dp_SESSION_FLAGS_ENABLE_DUALADDRESS;

			DPRINT(("dpReceive: joining any session on game server sessType %d, mask %d, flags %d\n", dp->pleaseJoinAnySessType, dp->pleaseJoinAnyMask, flags));
			err = dpSendJoinSession(dp, dp->joinKarma, NULL, dp->pleaseJoinAnySessType, flags, dp->pleaseJoinAnyMask, dp->hMaster);
			if (err != dp_RES_OK) {
				DPRINT(("dpReceive: send join failed.\n"));
				dp->pleaseClose = TRUE;
			}
		}
	}

	/* Receive a packet, check its type, possibly eat it here.  Yum! */
	pktlen = *size;
	ptimer_Enter(PTIMER_DPIO_GET, "dpio_get timer");
	err = dpio_get(dp->dpio, &pktsrc, buffer, &pktlen, &get_flags);
	ptimer_Exit(PTIMER_DPIO_GET, err);
	/* If dpio_get() is telling us that this handle may have timed out,
	 * make sure it is checked at the next opportunity rather than waiting
	 * 2 dpPoll() beacons.
	 */
	if (get_flags & dpio_GET_CHECK_TIMEOUT) {
		dp_result_t pollerr;

		dp->check_timeout = TRUE;
		pollerr = dpPoll(dp, dp->now);
		if ((pollerr == dp_RES_NETWORK_NOT_RESPONDING)
			|| (pollerr == dp_RES_HOST_NOT_RESPONDING))
			return pollerr;
	}
	/* If we've requested player deltas and dpio_get() has a new latency
	 * measurement for this player and there isn't too much local traffic,
	 * generate a player delta containing the latency.
	 */
	if (dp->monitor_player_latencies &&
		(get_flags & dpio_GET_LATENCY_MEASUREMENT) &&
		(dpio_localQ_npkts(dp->dpio) < 16)) {
		dp_result_t deltaerr = dp_sendLatencyDelta(dp, pktsrc);
		if (deltaerr != dp_RES_OK) {
			DPRINT(("dpReceive: can't send player latency delta, err:%d\n", deltaerr));
		}
	}
	dp_assertValid(dp);

	/* No packet in real network? Do housekeeping. */
	if (err == dp_RES_AGAIN)
		return dp_RES_AGAIN;
	if (err == dp_RES_EMPTY) {
		/* Handle ping transmissions first, they must be fast. */
		ptimer_Enter(PTIMER_POLLPING, "dpPollPing timer");
		dpPollPing(dp);
		ptimer_Exit(PTIMER_POLLPING, 0);
		dp_assertValid(dp);

		/* Handle outgoing housekeeping. */
		ptimer_Enter(PTIMER_POLL, "dpPoll timer");
		err = dpPoll(dp, dp->now);
		ptimer_Exit(PTIMER_POLL, 0);

		dp_assertValid(dp);
		if ((err == dp_RES_NETWORK_NOT_RESPONDING)
		|| (err == dp_RES_HOST_NOT_RESPONDING))
			return err;
		return dp_RES_EMPTY;
	}
	if (err != dp_RES_OK) {
		dp_assertValid(dp);
		return err;
	}

	DPRINT(("dpReceive: Packet %c%c from h:%x, length %d\n",
		((char *)buffer)[0], ((char *)buffer)[1],
		pktsrc, pktlen));

	/* Handle system packets. */
	if (*(char *)buffer == dp_PACKET_INITIALBYTE) switch (pkt->tag) {

	case dp_JOIN_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dpSwapJoinPacketT(&pkt->body.join, pktlen - sizeof(dp_packetType_t));
		err = dpHandleJoinSession(dp, pktsrc, &pkt->body.join, pktlen - sizeof(dp_packetType_t));
		if (err == dp_RES_BAD)
			return dp_RES_BUG;		/* in case of internal corruption */
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	case dp_LEAVE_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dpSwapLeavePacketT(&pkt->body.leave, pktlen - sizeof(dp_packetType_t));
		dpHandleLeaveSession(dp, pktsrc, &pkt->body.leave, pktlen - sizeof(dp_packetType_t));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	case dp_VOTE_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dpHandleVote(dp, pktsrc, ((char*)buffer)+2, pktlen-2);
		return dp_RES_AGAIN;
		break;

	case dp_VICTORY_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dpHandleVictory(dp, pktsrc, pktlen-2);
		return dp_RES_AGAIN;
		break;

	case dp_SELECTED_SESSION_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dpSwapSelectSessionPacket(&pkt->body.selectsess);
		dpHandleSelectedSession(dp, pktsrc, &pkt->body.selectsess, pktlen - 2);
		return dp_RES_AGAIN;
		break;

	case dptab_SUBSCRIBE_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dptab_handleSubscriptionRequest(dp->dt, pktsrc, &pkt->body.subscribe, pktlen - sizeof(dp_packetType_t));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	case dptab_UNSUBSCRIBE_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		dptab_handleUnsubscriptionRequest(dp->dt, pktsrc, &pkt->body.unsubscribe, pktlen - sizeof(dp_packetType_t));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	case dp_ERROR_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		/* A request we made to a server failed. */
		dpSwapErrorPacket(&pkt->body.error);
		DPRINT(("dpReceive: got failure packet %2.2s k%d from h:%x\n",
				&pkt->body.error.request,
				pkt->body.error.karma,
				pkt->body.error.err,
				pktsrc));
		if ((pkt->body.error.request == dp_JOIN_PACKET_ID)
		&&  (pkt->body.error.karma == dp->joinKarma)
		&&  (pktsrc == dp->hMaster)) {
			DPRINT(("dpReceive: got join fail packet from master h:%x\n", pktsrc));
			/* sniff, they won't let us in their game! */
			dp->pleaseClose = TRUE;
		}
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	case dp_PING_PACKET_ID:
		/* These are sometimes handled directly by the driver, because
		 * we have to be able to handle pings from hosts that we don't
		 * have open handles to!
		 * See winets\*.c, uudps.c
		 */
		dpSwapPingPacket(&pkt->body.ping);
		DPRINT(("ping: karma %d, pktnum %d\n", pkt->body.ping.karma, pkt->body.ping.pktnum));
		dpHandlePingPacket(dp, (unsigned char *)buffer, pktlen, pktsrc, dp_PING_RESP_PACKET_ID);
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	case dp_PING_RESP_PACKET_ID:
		dpSwapPingPacket(&pkt->body.pingresp);
		DPRINT(("ping_resp: karma %d, want karma %d, pktnum %d, len %d\n",
				pkt->body.pingresp.karma,
				dp->ping.karma,
				pkt->body.pingresp.pktnum,
				pktlen));
		if (pkt->body.pingresp.pktnum & 0x8000) {
			/* kludge for now */
			extern dp_result_t dpHandleServerPingResponsePacket( dp_t *dp, unsigned char *buf, size_t len, playerHdl_t src);
			dpHandleServerPingResponsePacket(dp, (unsigned char *)buffer, pktlen, pktsrc);
			dp_assertValid(dp);
			return dp_RES_AGAIN;
		}
		if ((pkt->body.pingresp.karma != dp->ping.karma)
		|| (pkt->body.pingresp.pktnum >= dp_PING_MAX_NTOTAL)) {
			dp_assertValid(dp);
			return dp_RES_AGAIN;
		}
		dp->ping.got_at[pkt->body.pingresp.pktnum] = dp->now;
		dp->ping.n_got++;
		dp_assertValid(dp);
		return dp_RES_AGAIN;
		break;

	/* need all this since packet doesn't contain pointer to table value */
	case dp_USER_PLAYERDATA_PACKET_ID: {
		dptab_table_t *playervars;
		char key[dptab_KEY_MAXLEN];
		int keylen;
		dpid_t firstId = (dpid_t) (pkt->body.pData.id & ~(dp_PLAYERS_PER_HOST-1));
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		key[0] = dp_KEY_PLAYER_VARIABLES;
		key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
		key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
		if (dp->firstId == firstId) {	/* Our own player */
			keylen = 3;
		} else {
			keylen = 5;
			key[3] = (char) dpGETSHORT_FIRSTBYTE(firstId);
			key[4] = (char) dpGETSHORT_SECONDBYTE(firstId);
		}
		if ((playervars = dptab_getTable(dp->dt, key, keylen)) != NULL) {
			char subkey[dptab_KEY_MAXLEN];
			int subkeylen = 0;
			char *buf;
			size_t buflen;
			int idoffset = (dpid_t)pkt->body.pData.id - firstId;
			subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idoffset);
			subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(pkt->body.pData.key);
			subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(pkt->body.pData.key);
			err = dptab_get_bykey(playervars, subkey, subkeylen, (void **)&buf, &buflen);
			if (err != dp_RES_OK) {
				DPRINT(("dpReceive: error: couldn't get player variable referenced by packet.\n"));
				return dp_RES_BUG;
			}
			pkt->body.pData.data = buf;
			if (pktsrc != PLAYER_ME)
				return dp_RES_AGAIN;
			*size = pktlen;
			*idFrom = dp_ID_NAMESERVER;		/* Simulate envelope */
			*idTo = dp_ID_BROADCAST;
		} else {
			/* could happen if player variable table was recently deleted and
			 * the owner's handle not yet closed */
			/* DPRINT(("dpReceive: couldn't get player variable table referenced by packet.\n")); */
		}
		dp_assertValid(dp);
		return dp_RES_OK;
		break;
	}

	case dp_TSERV_PACKET_ID:
	{
		tserv_event_t tevent;
		err = tserv_handle_packet(dp->tserv, pktsrc, get_flags, &pkt->body.tservpkt, &tevent);
		if (err == dp_RES_EMPTY)
			return dp_RES_AGAIN;
		if (err == dp_RES_OK) {
			DPRINT(("dpReceive: tserv event: h:%x, reason %d, uid %d\n", tevent.h, tevent.reason, tevent.uid));
			if ((dp->hMaster == PLAYER_ME) && (tevent.reason == dp_RES_OK)) {
				/* This is a credentials message.  I am the host, so
				 * set this uid for this player in myplayers table.
				 */
				dpid_t id;
				id = dp_commHdl2dpid(dp, tevent.h);
				if (id == dp_ID_NONE) {
					DPRINT(("dp_receive: no id for tevent.h:%x yet, waiting.\n", tevent.h));
				} else {
					dpid_t firstId = (dpid_t)(id & ~(dp_PLAYERS_PER_HOST-1));
					for (id = firstId; id < firstId+dp_PLAYERS_PER_HOST; id++) {
						err = dpSetPlayerUid(dp, id, tevent.uid);
						if ((err != dp_RES_OK) && (err != dp_RES_EMPTY)
						&&  (err != dp_RES_CLOSED)) {
							DPRINT(("dp_receive: dpSetPlayerUid(id:%d, uid:%d) returns err:%d\n", id, tevent.uid, err));
							return dp_RES_BUG;
						}
					}
				}
			}
			if ((tevent.h == PLAYER_ME) || bIAmGameServer(dp)) {
				/* This is a local message, either to a client or to the
				 * game server concerning some client.  Forward it as a
				 * local ACCOUNT_PACKET.
				 */
				pkt->tag = dp_ACCOUNT_PACKET_ID;
				pkt->body.account.reason = tevent.reason;
				pkt->body.account.uid = tevent.uid;
				pkt->body.account.reserved = pktsrc;

				/* Simulate a packet */
				*size = sizeof(pkt->tag) + sizeof(pkt->body.account);
				*idFrom = dp_ID_NAMESERVER;		/* Simulate envelope */
				*idTo = dp_ID_BROADCAST;
				return dp_RES_OK;
			}
		}
		break;
	}

	case dp_REQUEST_OPEN_PACKET_ID:
	{
		/*
		 * Request Open Packets are requests from a game server to open a connection to
		 * a designated address.  We must insure that this packet has come from a game
		 * server and then we open a handle to the player in the normal way.
		 */
		DPRINT(("dpReceive: Got REQUEST_OPEN packet\n"));
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}

		if (pktsrc != dp->hGameServer) {
			DPRINT(("dpReceive: REMOTE_SYN received from someone who is NOT a game server!"));
			return dp_RES_AGAIN;
		}

		DPRINT(("dpReceive: Opening Connection to "));
		printAdr(dp->dpio->myAdrLen, pkt->body.requestOpen.adr);
		DPRINT((", "));
		printAdr(dp->dpio->myAdrLen, pkt->body.requestOpen.adr2);
		DPRINT(("\n"));

		if (memcmp(pkt->body.requestOpen.adr, pkt->body.requestOpen.adr2, dp->dpio->myAdrLen)) {
			/*
			 * Addresses are different
			 */
			dpio_openHdl2(dp->dpio, pkt->body.requestOpen.adr, pkt->body.requestOpen.adr2);
		} else {
			/*
			 * Addresses are identical, only pass one in
			 */
			dpio_openHdl2(dp->dpio, pkt->body.requestOpen.adr, NULL);
		}
		return (dp_RES_AGAIN);
		break;
	}

	case dp_INDIRECT_JOIN_PACKET_ID:
	{
		dp_session_t	sess;
		playerHdl_t		host;
		size_t			len;

		DPRINT(("dpReceive: got INDIRECT_JOIN packet\n"));
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}

		/*
		 * Find the session in the session table
		 */
		len = sizeof (dp_session_t);
		if (dpGetSessionDescById(dp, (char*) pkt->body.indirectJoin.sessionId, &sess, &len) != dp_RES_OK) {
			DPRINT(("dpReceive: did not find session !\n"));
			dp_assertValid(dp);
			return err;
		}

		/*
		 * Lets find out what was in the session
		 */
		DPRINT(("dpReceive: session is being hosted by "));
		printAdr(dp->dpio->myAdrLen, sess.adrMaster);
		DPRINT(("\n"));

		/*
		 * Get handle to host of the session and make sure that
		 *
		 * - We did get a handle
		 * - The handle is not to me
		 *
		 */
		host = dpio_openHdl(dp->dpio, sess.adrMaster, NULL, NULL);

		if (host == PLAYER_NONE) {
			DPRINT(("dpReceive: No handle for host\n"));
			return dp_RES_AGAIN;
		}

		/*
		 * Track which session this user last tried to join.
		 */
		if (pkt->body.indirectJoin.chunk1 == dp_INDIRECT_JOIN_CHUNK_SESSIONID) {
			dp_uid_t uid;

			uid = tserv_hdl2uid(dp->tserv, pktsrc);
			err = dp_sessid4uid(dp, uid, pkt->body.indirectJoin.sessionId, dp->dpio->myAdrLen+2, sess.sessionType);
			if (err != dp_RES_OK) {
				DPRINT(("dpReceive: can't record session for h:%x uid:%d, err:%d\n", pktsrc, uid, err));
			}
		}

		if (host == PLAYER_ME) {
			DPRINT(("dpReceive: I'm the host ???\n"));
			return dp_RES_AGAIN;
		}

#if 1
		/* Try to send credentials for the joiner to the host.
		 * If that fails, at least tell the host to open a handle
		 * to the joiner.
		 */
		err = tserv_send_credentials(dp->tserv, pktsrc, host);
		if (err != dp_RES_OK)
#endif
		{

			/*
			 * If can't authorize, send a remote open request instead.
			 *
			 * - Get both known addresses of the handle
			 * - Compose REQUEST_OPEN_PACKET
			 * - Fire it off to the session master
			 *
			 */
			struct {
				dp_packetType_t				tag;
				dp_request_open_packet_t	body;
			} PACK pkt;
			int len;

			DPRINT(("dpReceive: Could not send credentials from h:%x to h:%x, err:%d\n", pktsrc, host, err));

			pkt.tag = dp_REQUEST_OPEN_PACKET_ID;
			len = dp_MAX_ADR_LEN;

			err = dpio_hdl2adr2(dp->dpio, pktsrc, pkt.body.adr, pkt.body.adr2, &len);

			DPRINT(("dpReceive: Sending REQUEST OPEN for "));
			printAdr(dp->dpio->myAdrLen, pkt.body.adr);
			DPRINT((", "));
			printAdr(dp->dpio->myAdrLen, pkt.body.adr2);
			DPRINT((" to "));
			printAdr(dp->dpio->myAdrLen, sess.adrMaster);
			DPRINT(("\n"));

			if (err != dp_RES_OK) {
				DPRINT(("dpReceive: Could not translate host handle to address(es)\n"));
				return (err);
			}

			err = dpio_put_reliable(dp->dpio, &host, 1, &pkt, sizeof (pkt), NULL);
			if (err != dp_RES_OK) {
				DPRINT(("dpReceive: Could not send indirect join to host h:%x\n", host));
				return (err);
			}
		}
		return (dp_RES_AGAIN);
		break;
	}

	case dp_USER_HOST_PACKET_ID:
	case dp_SESSIONLOST_PACKET_ID:
	case dp_USER_ADDPLAYER_PACKET_ID:
	case dp_USER_DELPLAYER_PACKET_ID:
	case dp_USER_ADDGROUP_PACKET_ID:
	case dp_USER_DELGROUP_PACKET_ID:
	case dp_USER_ADDPLAYERTOGROUP_PACKET_ID:
	case dp_USER_DELPLAYERFROMGROUP_PACKET_ID:
	case dp_OBJECTDELTA_PACKET_ID:
	case dp_SESSIONRESULT_PACKET_ID:
		/* make sure packet was sent reliably */
		if ((get_flags & dpio_GET_RELIABLE) == 0) {
			DPRINT(("dpReceive: got reliable packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		if (pktsrc != PLAYER_ME)
			return dp_RES_AGAIN;
		*size = pktlen;
		*idFrom = dp_ID_NAMESERVER;		/* Simulate envelope */
		*idTo = dp_ID_BROADCAST;
		dp_assertValid(dp);
		return dp_RES_OK;
		break;

	case dptab_DELETE_PACKET_ID:
	case dptab_INITIAL_PACKET_ID:
	case dptab_BODY_PACKET_ID:
	case dptab_SMALL_PACKET_ID:
		/* if internet, check that all dptab packets were sent reliably */
		if (((dp->dpio->driverinfo->capabilities & comm_DRIVER_NO_BROADCAST)) &&
			((get_flags & dpio_GET_RELIABLE) == 0)) {
			DPRINT(("dpReceive: got internet dptab packet not sent reliably\n"));
			return dp_RES_AGAIN;
		}
		DPRINT(("dpReceive: Offering packet from h:%x to dptab\n", pktsrc));
		err = dptab_handlePacket(dp->dt, pktsrc, pktlen, buffer);
		if (err != dp_RES_OK) {
			DPRINT(("dpReceive: dptab failed to handle packet from h:%x, err:%d \n", pktsrc, err));
		}
		dp_assertValid(dp);
		return dp_RES_AGAIN;

		break;

	default: ;
		DPRINT(("dpReceive: Unknown system packet %2.2s len %d; dptab_handlePacket returns err:%d\n", buffer, pktlen, err));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
	}

	/* Utter kludge: if flags is set, ignore envelope, and return player
	 * handle as source instead of player id.
	 * Meant for use by game server during development.
	 */
	if (flags) {
		*size = pktlen;
		*(playerHdl_t *)idFrom = pktsrc;	/* WARNING: pointer size not type safe! */
		*idTo = dp_ID_NONE;
		dp_assertValid(dp);
		return dp_RES_OK;
	}

	/* System messages all handled above.  No user messages may start
	 * with the same initial byte of the dp system messages.
	 */
	if (pktlen <= sizeof(dp_envelope_t)) {
		DPRINT(("dpReceive: user message too short, ignoring.\n"));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
	}

	/* Not a system packet - thus it must have an envelope. */
	*size = pktlen - sizeof(dp_envelope_t);
	env = (dp_envelope_t *)(((char *)buffer) + *size);
	dp_assertValid(dp);

	/* call dpSwapEnv to byte swap env*/
	dpSwapEnv(env);
	DPRINT(("dpReceive: t:%d tag:%2.2s src:%d dest:%d len:%d; karma:%d\n",
		dp->now, buffer, env->src, env->dest, *size, env->sessKarma));
	/* Validate the source address and the envelope. */
	if (pktsrc == PLAYER_NONE
	|| pktsrc == PLAYER_UNKNOWN
	|| env->sessKarma != dp->sess_karma) {
		DPRINT(("dpReceive: unknown source h:%x or bad karma %d (wanted %d)\n", pktsrc, env->sessKarma, dp->sess_karma));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
	}
	if (env->src >= dp_MAXDPIDS
	||  env->dest >= dp_MAXDPIDS) {
		DPRINT(("bogus envelope!\n"));
		dp_assertValid(dp);
		return dp_RES_AGAIN;
	}
	if (!dp->sess_karma || !dp->dpid2commHdl) {
		DPRINT(("dpReceieve: env->src id %d, but no session active\n", env->src));
		return dp_RES_AGAIN;
	}

	if (pktsrc != dpid2commHdl(dp, env->src)
	&& pktsrc != PLAYER_ME) {
		DPRINT(("Forged source adr!  src h:%x, env->src id %d, corresponding h:%x\n",
			pktsrc, env->src, dpid2commHdl(dp, env->src)));
		dumpAdrTable(dp);
		/* Forged source address. Probably from an old game, */
		/* or possibly from a new server that can't hear us... */
		/* KLUDGE?  Allow self to forge anything.  Probably safe. */
		dp_assertValid(dp);
		return dp_RES_AGAIN;
	}

	/* Enforce restriction that you can't send to yourself */
	if ((env->src != dp_ID_NAMESERVER)
	&&  (env->src == env->dest)) {
		dp_assertValid(dp);
		return dp_RES_AGAIN;
	}

	/* It is defintely a user packet.  Pass it on to the caller. */
	*idFrom = env->src;
	*idTo = env->dest;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Get list of player handles corresponding to the broadcast group.
 Return number of handles, or -1 upon error.
 Note: will not return PLAYER_ME...

 If flags is dp_SEND_RELIABLE, never yields PLAYER_BROADCAST.
 If flags is dp_SEND_UNRELIABLE, yields PLAYER_BROADCAST only if
 idTo is dp_ID_BROADCAST and the driver prefers broadcast.
--------------------------------------------------------------------------*/
static int dp_getBroadcastHdls(dp_t *dp, playerHdl_t hdls[], int flags)
{
	int i;
	int nhdls;

	dp_assertValid(dp);

	if ((flags == dp_SEND_UNRELIABLE)
	&& (dp->dpio->driverinfo->capabilities&comm_DRIVER_PREFERS_BROADCAST)) {
		DPRINT(("dp_getBroadcastHdls: hardware broadcast!\n"));
		hdls[0] = PLAYER_BROADCAST;
		return 1;
	}

	if (!dp->dpid2commHdl) {
		DPRINT(("dp_getBroadcastHdls: no table!\n"));
		return 0;
	}

	/* Construct destination list on the fly for now. */
	for (nhdls=0, i=dp->dpid2commHdl->n_used-1; i>=0; i--) {
		int k;
		assoctab_item_t *pe;
		playerHdl_t *pH;
		playerHdl_t h;
		dpid_t id;

		pe = assoctab_getkey(dp->dpid2commHdl, i);
		if (!pe)
			continue;
		id = (dpid_t) pe->key;

		if (id == dp_ID_BROADCAST)
			continue;

		pH = (playerHdl_t *) &pe->value;
		h = *pH;

		/* Don't broadcast to myself if there's only one player here.
		 * Don't broadcast to the Game Server.
		 * Note: This means we can't really create player on server.
		 */
		if ((h == PLAYER_ME) || (h == dp->hGameServer)/*&& (dp->my_nPlayers < 3)*/)
			continue;

		DPRINT(("dp_getBroadcastHdls: id %d has h:%x\n", id, h));

		/* Set to PLAYER_NONE if duplicate */
		if (h != PLAYER_NONE) {
			for (k=0; k<nhdls; k++) {
				if (hdls[k] == h) {
					h = PLAYER_NONE;
					break;
				}
			}
		}

		/* Add to hdls if not duplicate */
		if ((h != PLAYER_NONE) && (h != PLAYER_UNKNOWN))
			hdls[nhdls++] = h;
	}
	DPRINT(("dp_getBroadcastHdls: %d handles\n", nhdls));
	dp_assertValid(dp);
	return nhdls;
}

/*--------------------------------------------------------------------------
 Get list of player handles corresponding to the given group.
 Return number of handles, or -1 upon error.
 BUG: doesn't return -1... should fix.
 Note: will not return PLAYER_ME.
--------------------------------------------------------------------------*/
static int dp_getGroupHdls(dp_t *dp, dpid_t id, playerHdl_t hdls[])
{
	int i;
	int nhdls;
	dptab_table_t *grplayers;
	char key[dptab_KEY_MAXLEN];
	int keylen;

	dp_assertValid(dp);

	if (!dp)
		return -1;

	/* Check if id is within group id limits */
	if (id < dp->firstGId || id >= dp->firstGId + dp_MAX_GROUPS) {
		dp_assertValid(dp);
		return -1;
	}

	/* Locate the group player table specified by the given id. */
	keylen = 0;
	key[keylen++] = dp_KEY_GROUP_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(id);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(id);
	if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
		dp_assertValid(dp);
		return -1;
	}

	/* Construct destination list on the fly for now. */
	for (i = dptab_tableSize(grplayers) - 1, nhdls = 0; i >= 0; i--) {
		int k;
		playerHdl_t h;
		dp_result_t err;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		void *pbuf;
		size_t plen;
		dpid_t pid;

		err = dptab_get_byindex(grplayers, i, (void **)&pbuf, &plen, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			dp_assertValid(dp);
			return -1;
		}
		pid = (unsigned short) dpMAKESHORT(subkey[0], subkey[1]);
		h = dpid2commHdl(dp, pid);

		/* Set to PLAYER_NONE if duplicate */
		for (k=0; k<nhdls; k++) {
			if (hdls[k] == h) {
				h = PLAYER_NONE;
				break;
			}
		}
		/* Don't send to myself */
		if ((h == PLAYER_ME) /*&& (dp->my_nPlayers < 3)*/)
			h = PLAYER_NONE;

		/* Add to hdls if not duplicate */
		if ((h != PLAYER_NONE) && (h != PLAYER_UNKNOWN))
			hdls[nhdls++] = h;
	}
	DPRINT(("dp_getGroupHdls: group %d has %d handles\n", id, nhdls));
	dp_assertValid(dp);
	return nhdls;
}

/*--------------------------------------------------------------------------
 Convert a destination dpid into an array of playerHdl_t for use with
 the dpio layer.
 If flags is dp_SEND_RELIABLE, never yields PLAYER_BROADCAST.
 If flags is dp_SEND_UNRELIABLE, yields PLAYER_BROADCAST only if
 idTo is dp_ID_BROADCAST and the driver prefers broadcast.

 Returns number of handles, or -1 (or zero :( ) on error.
--------------------------------------------------------------------------*/
static int dp_getHdls(dp_t *dp, dpid_t idTo, playerHdl_t *dests, int flags)
{
	int ndests;

	dp_assertValid(dp);

	/* Generate list of destination handles */
	if (idTo == dp_ID_BROADCAST) { /* Is it a reliable broadcast id? */
		ndests = dp_getBroadcastHdls(dp, dests, flags);
	} else {
		/* Is it a normal id? */
		dests[0] = dpid2commHdl(dp, idTo);
		ndests = 1;
		if (dests[0] == PLAYER_NONE)
			ndests = dp_getGroupHdls(dp, idTo, dests);
	}
	dp_assertValid(dp);
	return ndests;
}

/*----------------------------------------------------------------------
 Send a packet from one player to another.
 First two bytes of the message must indicate the message type.
 Users must not use any message type starting with 'd'.
 Message types are created with the dppt_MAKE macro.

 If the flags argument is dp_SEND_RELIABLE, dp will continue trying
 to send the packet until it is acknowledged.  Otherwise it will just
 send it once.

 Warning!  User must allocate extra six bytes at end of buffer for dpid's.
 DO NOT report these extra bytes in the size parameter!
----------------------------------------------------------------------*/
DP_API dp_result_t dpSend(
	dp_t  *dp,
	dpid_t idFrom,
	dpid_t idTo,
	int flags,
	void *buffer,
	size_t size)
{
	dp_envelope_t	*env;
	dp_result_t		err;
	playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */
	int ndests;

	precondition(dp != NULL);
	precondition((idFrom != dp_ID_NONE) && (idTo != dp_ID_NONE));
	dp_assertValid(dp);

	if (!dp->dpid2commHdl) {
		DPRINT(("dpSend: Can't send when not in session (no id tbl)\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
	if (!dp->sess_karma) {
		DPRINT(("dpSend: Can't send when not in session (no sess karma)\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
    /* Warning!  User must allocate extra six bytes at end of buffer */
	/* to leave room for env. */
	env = (dp_envelope_t *)(((char *)buffer) + size);
	env->src = idFrom;
	env->dest = idTo;
	env->sessKarma = dp->sess_karma;

	DPRINT(("dpSend: From=%d To=%d Tag=%c%c Len=%d time:%d flags=%x\n",
                idFrom, idTo,
                (((char *)buffer)[0]), (((char *)buffer)[1]), size,
                dp->now, flags
				));

	/* Generate list of destination handles */
	ndests = dp_getHdls(dp, idTo, dests, flags);
	if (ndests < 0) {
		/* Why, it isn't a valid id at all.  (don't assume this is caught!) */
		DPRINT(("dpSend: unknown dest id:%d\n", idTo));
		dp_assertValid(dp);
		return dp_RES_PEER_CLOSED;
	}
	if (ndests == 0) {
		DPRINT(("dpSend: dest id %d maps to no handle?\n", idTo));
		dp_assertValid(dp);
		return dp_RES_OK;
	}

	/* Have to make sure the clock is up to date here */
	dp->now = eclock();

	/* call dpSwapEnv to byte swap env*/
	dpSwapEnv(env);
	if ((flags & 1) == dp_SEND_UNRELIABLE)
		err = dpio_put_unreliable(dp->dpio, dests, ndests, buffer, size + sizeof(dp_envelope_t), NULL);
	else if ((flags & 1) == dp_SEND_RELIABLE)
		err = dpio_put_reliable2(dp->dpio, dests, ndests, buffer, size + sizeof(dp_envelope_t), NULL, flags);
	else
		err = dp_RES_BUG;

	/* Flush the packet, if the user code hasn't volunteered to do
	 * that itself by calling dpFlush().
	 */
	if (!dp->dpio->appWillFlush) {
		dpio_flush(dp->dpio);
	}

	dp_assertValid(dp);
	return err;
}

/*------------------------------------------------------------------------
 Send any unsent packets.  Call this after a bunch of dpSend's.
 Once this is called the first time, dpSend no longer flushes its buffer
 automatically; it waits for you to call this.
------------------------------------------------------------------------*/
DP_API dp_result_t dpFlush(
	dp_t *dp)
{
	dpio_flush(dp->dpio);
	dp->appLastFlushed = dp->now;
	dp->dpio->appWillFlush = TRUE;
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Set the intervals used by the latency measurement system.
 If both intervals are 0, only ACKs of non-retransmitted reliable packets
	will generate new measurements.  No extra bandwidth is used, but
	measurements may be infrequent if there are few reliable packets or
	there is heavy packet loss.
 If piggbackPingInterval is set, additional small ping packets and
	responses will be piggybacked onto gathered packets every
	piggybackPingInterval ms when possible.  No extra physical packets
	will be generated, and any network traffic at all will generate
	latency and packet loss measurements.
 If forcedPingInterval is set and no piggybacked pings have been sent in
	the past forcedPingInterval ms, an additional ping packet will be
	sent to get a latency measurement.

 Pings will only be sent to players in the current session, and will
 never be sent more than once per round trip time, regardless of the
 intervals requested.

 Returns dp_RES_OK on success, or dp_RES_BAD on bad parameters.
------------------------------------------------------------------------*/
DP_API dp_result_t dpSetPingIntervals(
	dp_t *dp,
	int piggybackPingIntervalMS,
	int forcedPingIntervalMS)
{
	dp_result_t err;
	int i;
	playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */
	int ndests;

	precondition(dp && dp->dpio);
	err = dpio_setPingIntervals(dp->dpio, piggybackPingIntervalMS,
		forcedPingIntervalMS);
	if (err != dp_RES_OK) {
		DPRINT(("dpSetPingIntervals: err:%d\n", err));
		return err;
	}
	dp->peerHandleMode =
		(piggybackPingIntervalMS ? dpio_OPTION_PIGGYBACK_PING : 0) |
		(forcedPingIntervalMS ? dpio_OPTION_FORCED_PING : 0);

	ndests = dp_getBroadcastHdls(dp, dests, dp_SEND_RELIABLE);
	for (i = 0; i < ndests; i++) {
		playerHdl_t h = dests[i];
		short opt = dp_getDesiredHandleOptions(dp, h);

		err = dpio_setHandleOptions(dp->dpio, h, opt);
		if (err != dp_RES_OK) {
			DPRINT(("dpSetPingIntervals: dpio_setHandleOptions(h:%x, mode:%x) err:%d\n", h, opt));
		}
	}
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Measure round-trip transmission time to a player or the gamehost.
 To ping the gamehost, use dest = 0.
 Result is returned after a couple seconds via callback.
 Call with a unique karma (say, 1 for first call, 2 for 2nd call, etc.)
------------------------------------------------------------------------*/
DP_API dp_result_t dpPingUser(
	dp_t *dp,
	dpid_t dest,
	short karma,
	dp_ping_callback_t cb)
{
	DPRINT(("dpPingUser(,%d,):\n", dest));

	dp_assertValid(dp);

	/* Set up the ping structure, then let dp_poll do the rest. */
	if (dp->ping_dest != PLAYER_NONE) {
		DPRINT(("dpPingUser: busy; ping_dest already %d\n", dp->ping_dest));
		dp_assertValid(dp);
		return dp_RES_BUSY;
	}
	dp->ping_dest = dpid2commHdl(dp, dest);
	if (dp->ping_dest == PLAYER_NONE) {
		DPRINT(("dpPingUser: odd, can't get commHdl for dpid %d\n", dp->ping_dest));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	dp->ping.karma = (dp_karma_t)karma;
	/* Send 2 seconds worth of packets */
	dp->ping.n_total = 10;
	dp->ping.n_sent = 0;
	dp->ping.n_got = 0;
	memset(dp->ping.sent_at, 0, sizeof(time_t)*10);
	memset(dp->ping.got_at, 0, sizeof(time_t)*10);
	dp->ping.next_send_time = dp->now + dp->dpio->clocksPerSec / 5;
	/* Wait 3 seconds for all packets to come back. */
	dp->ping.deadline = dp->now + (dp->dpio->clocksPerSec * 30) / 10;
	dp->ping.cb = cb;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/********************** Group Management **************************/

/*----------------------------------------------------------------------
 Add a new group to the currently open session. Input is dp and name of group.
 Can only be called on game host!
 On success, dp_RES_OK is returned and newly assigned group id is put into *id.
 On failure, dp_RES_BUG is returned if inputs were null; dp_RES_BAD if
 game host is not caller; dp_RES_CLOSED if session not fully initialized;
 dp_RES_FULL if maximum groups allowed to be created has been reached;
 otherwise get return value of dptab_set().
----------------------------------------------------------------------*/
DP_API dp_result_t dpCreateGroup(
	dp_t	*dp,
	dpid_t *id,
	char_t	*name)
{
	dp_result_t err;
	dpid_t groupId;
	struct {
		char_t name[dp_PNAMELEN];
	} PACK gbuf;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	size_t glen;

	precondition(dp != NULL);
	precondition(name != NULL);
	dp_assertValid(dp);

	DPRINT(("dpCreateGroup:\n"));
	if (dp->hMaster != PLAYER_ME) {
		DPRINT(("dpCreateGroup: err: must be master\n"));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	if (!dp->groups) {
		DPRINT(("dpCreateGroup: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	/* Make sure we're not out of id's */
	if (dp->nextGId >= dp->firstGId + dp_MAX_GROUPS) {
		DPRINT(("dpCreateGroup: err: no more group id's for me\n"));
		dp_assertValid(dp);
		return dp_RES_FULL;
	}
	DPRINT(("dpCreateGroup(dp, %p, %s):\n", id, name));

	/* Allocate group id and storage */
	groupId = dp->nextGId++;
	*id = groupId;

	/* Group record's key is its group id. */
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(groupId);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(groupId);

	/* Create the group record. */
	strncpy(gbuf.name, name, dp_PNAMELEN);
	glen = strlen(name);
	if (glen > dp_PNAMELEN - 1)
		glen = dp_PNAMELEN - 1;

	/* Write the group record; this triggers the callback on remote hosts. */
	err = dptab_set(dp->dt, dp->groups, subkey, subkeylen, (void *)&gbuf, glen, 2, PLAYER_ME);
	if (err != dp_RES_OK) {
		DPRINT(("dpCreateGroup: err: can't set group record, err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}

	DPRINT(("dpCreateGroup: set group record.  Callback should have already"
			" happened locally.\n"));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Destroy the given group; removes the group from the game session.
 The dpID will not be reused during the current session.
 Can only be called on game host!
 On success, dp_RES_OK is returned.
 On failure, dp_RES_BUG is returned if inputs were null; dp_RES_BAD if
 game host is not caller; dp_RES_CLOSED if session not fully initialized;
 otherwise get return value of dptab_delete().
----------------------------------------------------------------------*/
DP_API dp_result_t dpDestroyGroup(
	dp_t   *dp,
	dpid_t id)
{
	int i;
	int n;
	dp_result_t err;
	dptab_table_t *grplayers;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);

	DPRINT(("dpDestroyGroup:\n"));
	if (dp->hMaster != PLAYER_ME) {
		DPRINT(("dpDestroyGroup: err: must be master\n"));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	if (!dp->groups) {
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	keylen = 5;
	key[1] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	key[2] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	key[3] = (char) dpGETSHORT_FIRSTBYTE(id);
	key[4] = (char) dpGETSHORT_SECONDBYTE(id);
	for (n = 0; n < 2; n++) {
		switch (n) {
			case 0:		key[0] = dp_KEY_GROUP_PLAYERS; break;
			case 1:		key[0] = dp_KEY_GROUP_MYPLAYERS; break;
		}
		if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
			DPRINT(("dpDestroyGroup: err: closed\n"));
			dp_assertValid(dp);
			return dp_RES_CLOSED;
		}
		for (i = dptab_tableSize(grplayers) - 1; i >= 0; i--) {
			void *pbuf;
			size_t plen;

			err = dptab_get_byindex(grplayers, i, (void **)&pbuf, &plen, subkey, &subkeylen);
			if (err != dp_RES_OK) {
				DPRINT(("dpDestroyGroup: err: couldn't get %d player from table\n", i));
				dp_assertValid(dp);
				return err;
			}
			err = dptab_delete(dp->dt, grplayers, subkey, subkeylen);
			if (err != dp_RES_OK) {
				DPRINT(("dpDestroyGroup: err: couldn't delete %d player from table\n", i));
				dp_assertValid(dp);
				return err;
			}
		}
	}

	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(id);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(id);
	err = dptab_delete(dp->dt, dp->groups, subkey, subkeylen);
	if (err != dp_RES_OK) {
		DPRINT(("dpDestroyGroup: err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}

	DPRINT(("dpDestroyGroup: destroyed group record.  Callback should have already happened locally.\n"));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Add player idPlayer to the group idGroup.  Broadcast notification to others.
 Called by user code.
 On success, dp_RES_OK is returned.
 On failure, dp_RES_BUG is returned if dp is null or idGroup/idPlayer is
 not a valid id/doesn't exist or if caller is not host and idPlayer is not
 caller's player; dp_RES_CLOSED if session not fully initialized;
 dp_RES_ALREADY if player is already in group;
 otherwise get return value of dptab_set().
----------------------------------------------------------------------*/
DP_API dp_result_t dpAddPlayerToGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer)
{
	dp_result_t err;
	void *pbuf;
	size_t plen;
	dptab_table_t *grplayers;
	playerHdl_t src;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	char name[dp_PNAMELEN];

	DPRINT(("dpAddPlayerToGroup:\n"));
	if (!dp) {
		DPRINT(("dpAddPlayerToGroup: err: null\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (!dp->groups) {
		DPRINT(("dpAddPlayerToGroup: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
	keylen = 0;
	key[keylen++] = dp_KEY_GROUP_MYPLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(idGroup);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(idGroup);
	if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
		DPRINT(("dpAddPlayerToGroup: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idGroup);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(idGroup);
	err = dptab_get_bykey(dp->groups, subkey, subkeylen, (void **)&pbuf, &plen);
	if (err != dp_RES_OK) {
		DPRINT(("dpAddPlayerToGroup: err: group %d is not in group table\n", idGroup));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if ((src = dpid2commHdl(dp, idPlayer)) == PLAYER_NONE) {
		DPRINT(("dpAddPlayerToGroup: err: player %d has no comm handle, can't add\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dpGetPlayerName(dp, idPlayer, name, dp_PNAMELEN) != dp_RES_OK) {
		DPRINT(("dpAddPlayerToGroup: err: no player name for id %d\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dp->hMaster != PLAYER_ME) {	/* if not master, check if it's my player */
		subkeylen = 0;
		subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idPlayer);
		subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(idPlayer);
		err = dptab_get_bykey(dp->myplayers, subkey, subkeylen, (void **)&pbuf, &plen);
		if (err != dp_RES_OK) {
			DPRINT(("dpAddPlayerToGroup: err: player %d is not my player\n", idPlayer));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
	}

	/* Set the group myplayer record */
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idPlayer);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(idPlayer);
	{	dptab_table_t *gplayers;
		key[0] = dp_KEY_GROUP_PLAYERS;
		if (!(gplayers = dptab_getTable(dp->dt, key, keylen))) {
			DPRINT(("dpAddPlayerToGroup: err: closed\n"));
			dp_assertValid(dp);
			return dp_RES_CLOSED;
		}
		if (dptab_get_bykey(gplayers, subkey, subkeylen, (void **)&pbuf, &plen) == dp_RES_OK) {
			DPRINT(("dpAddPlayerToGroup: err: player %d already in group\n", idPlayer));
			dp_assertValid(dp);
			return dp_RES_ALREADY;
		}
	}
	pbuf = &key[0];		/* kludge */
	plen = 0;

	/* Write the group myplayer record; this triggers the callback on remote hosts. */
	err = dptab_set(dp->dt, grplayers, subkey, subkeylen, (void *)pbuf, plen, 2, src);
	if (err != dp_RES_OK) {
		DPRINT(("dpAddPlayerToGroup: err: can't set group player record, err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}

	/* If we're the master, add it in to the group players table, too
	 * (dptab doesn't do local subscriptions, I think)
	 */
	if (dp->hMaster == PLAYER_ME) {
		key[0] = dp_KEY_GROUP_PLAYERS;
		if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
			DPRINT(("dpAddPlayerToGroup: err: closed\n"));
			dp_assertValid(dp);
			return dp_RES_CLOSED;
		}
		err = dptab_set(dp->dt, grplayers, subkey, subkeylen, (void *)pbuf, plen, 2, src);
		if (err != dp_RES_OK) {
			DPRINT(("dpAddPlayerToGroup: err: can't set group player record, err:%d\n", err));
			dp_assertValid(dp);
			return err;
		}
	}

	DPRINT(("dpAddPlayerToGroup: set group player record.  Callback should have already happened locally.\n"));
	dp_assertValid(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Delete player idPlayer from the group idGroup.
 On success, dp_RES_OK is returned.
 On failure, dp_RES_BUG is returned if dp is null or idGroup/idPlayer is
 not a valid id/doesn't exist or if caller is not host and idPlayer is not
 caller's player; dp_RES_CLOSED if session not fully initialized;
 otherwise get return value of dptab_delete().
----------------------------------------------------------------------*/
DP_API dp_result_t dpDeletePlayerFromGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer)
{
	dp_result_t err;
	dptab_table_t *grplayers;
	char key[dptab_KEY_MAXLEN];
	int keylen;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;

	precondition(dp != NULL);
	dp_assertValid(dp);

	DPRINT(("dpDeletePlayerFromGroup:\n"));
	if (!dp->groups) {
		DPRINT(("dpDeletePlayerFromGroup: err: closed\n"));
		return dp_RES_CLOSED;
	}
	keylen = 0;
	key[keylen++] = dp_KEY_GROUP_MYPLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(idGroup);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(idGroup);
	if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
		DPRINT(("dpAddPlayerToGroup: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
	if ((idGroup < dp->firstGId) || (idGroup >= dp->firstGId + dp_MAX_GROUPS)) {
		dp_assertValid(dp);
		return dp_RES_BUG;
	}

	/* Delete the myplayer record. */
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idPlayer);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(idPlayer);
	err = dptab_delete(dp->dt, grplayers, subkey, subkeylen);
	if (err != dp_RES_OK && dp->hMaster != PLAYER_ME) {
		DPRINT(("dpDeletePlayerFromGroup: err: can't delete group player record, err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}

	/* If we're the master, delete it from the group players table, too
	 * (dptab doesn't do local subscriptions, I think)
	 */
	if (dp->hMaster == PLAYER_ME) {
		key[0] = dp_KEY_GROUP_PLAYERS;
		if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
			DPRINT(("dpAddPlayerToGroup: err: closed\n"));
			dp_assertValid(dp);
			return dp_RES_CLOSED;
		}
		err = dptab_delete(dp->dt, grplayers, subkey, subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpDeletePlayerFromGroup: err:%d\n", err));
		}
	}

	DPRINT(("dpDeletePlayerFromGroup: deleted group player record.  Callback should have already happened locally.\n"));
	dp_assertValid(dp);
	return err;
}

/*----------------------------------------------------------------------
 Calls the given function once for each group in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the group in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.

 s must currently be NULL - that is, you can't yet enumerate the groups
 of sessions you haven't joined.

 On success, dp_RES_OK is returned.
 On failure, dp_RES_BUG is returned if inputs are null; dp_RES_CLOSED if
 session not fully initialized; dp_RES_BAD if session is not NULL.
----------------------------------------------------------------------*/
DP_API dp_result_t dpEnumGroups(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	)
{
	int i;
	dp_result_t err;

	DPRINT(("dpEnumGroups:\n"));
	if (!dp || !cb) {
		DPRINT(("dpEnumGroups: err: null dp or callback\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (!dp->groups) {
		DPRINT(("dpEnumGroups: err: session not initialized\n"));
		cb(dp_ID_NONE, NULL, 0, context);
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
	if (s) {
		DPRINT(("dpEnumGroups: err: can't enum remote sessions yet\n"));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}

	for (i = dptab_tableSize(dp->groups) - 1; i >= 0; i--) {
		int namelen;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		char *grec;
		size_t glen;
		dpid_t gid;
		char_t name[dp_PNAMELEN];

		err = dptab_get_byindex(dp->groups, i, (void **)&grec, &glen, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpEnumGroups: err: error getting %d element from group table\n, i"));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
		gid = (unsigned short) dpMAKESHORT(subkey[0], subkey[1]);
		memcpy(name, grec, glen);
		namelen = glen < dp_PNAMELEN - 1 ? glen : dp_PNAMELEN - 1;
		name[namelen] = '\0';

		/* DPRINT(("dpEnumGroups: cb(%d, %s, %p)\n",
			gid, name, context)); */
		cb(gid, name, 0, context);
	}
	cb(dp_ID_NONE, NULL, 0, context);

	(void) timeout;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Calls the given function once for each player in the given group, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.

 s must currently be NULL - that is, you can't yet enumerate the group
 members of sessions you haven't joined.

 On success, dp_RES_OK is returned.
 On failure, dp_RES_BUG is returned if inputs are null or idGroup is
 not a valid id/doesn't exist; dp_RES_CLOSED if session not fully initialized;
 dp_RES_BAD if session is not NULL.
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
	int i;
	dp_result_t err;
	dptab_table_t *grplayers;
	char key[dptab_KEY_MAXLEN];
	int keylen;

	precondition(dp != NULL);
	precondition(cb != NULL);
	if (!dp->groups) {
		DPRINT(("dpEnumGroupPlayers: err: session not initialized\n"));
		cb(dp_ID_NONE, NULL, 0, context);
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}
	if (s) {
		DPRINT(("dpEnumGroupPlayers: err: can't enum remote sessions yet\n"));
		return dp_RES_BAD;
	}
	precondition((idGroup >= dp->firstGId) && (idGroup < dp->firstGId + dp_MAX_GROUPS));
	dp_assertValid(dp);
	DPRINT(("dpEnumGroupPlayers:\n"));

	{
		dp_result_t err;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		void *pbuf;
		size_t plen;
		subkeylen = 0;
		subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idGroup);
		subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(idGroup);
		err = dptab_get_bykey(dp->groups, subkey, subkeylen, (void **)&pbuf, &plen);
		if (err != dp_RES_OK) {
			DPRINT(("dpEnumGroupPlayers: err: group %d is not in group table\n", idGroup));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
	}
	keylen = 0;
	key[keylen++] = dp_KEY_GROUP_PLAYERS;
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	key[keylen++] = (char) dpGETSHORT_FIRSTBYTE(idGroup);
	key[keylen++] = (char) dpGETSHORT_SECONDBYTE(idGroup);
	if (!(grplayers = dptab_getTable(dp->dt, key, keylen))) {
		DPRINT(("dpEnumGroupPlayers: err: closed\n"));
		dp_assertValid(dp);
		return dp_RES_CLOSED;
	}

	DPRINT(("dpEnumGroupPlayers: tablesize %d\n", dptab_tableSize(grplayers)));
	for (i = dptab_tableSize(grplayers) - 1; i >= 0; i--) {
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen;
		void *pbuf;
		size_t plen;
		dpid_t pid;
		char_t name[dp_PNAMELEN];

		err = dptab_get_byindex(grplayers, i, (void **)&pbuf, &plen, subkey, &subkeylen);
		if (err != dp_RES_OK) {
			DPRINT(("dpEnumGroupPlayers: err %d getting %d element from group players table\n", err, i));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}
		pid = (unsigned short) dpMAKESHORT(subkey[0], subkey[1]);
		if ((err = dpGetPlayerName(dp, pid, name, dp_PNAMELEN)) != dp_RES_OK) {
			DPRINT(("dpEnumGroupPlayers: err: no player name for id %d\n", pid));
			dp_assertValid(dp);
			return dp_RES_BUG;
		}

		DPRINT(("dpEnumGroupPlayers: cb(%d, %s, %p) for %d\n",
			pid, name, context, i));
		cb(pid, name, 0, context);
	}
	cb(dp_ID_NONE, NULL, 0, context);

	(void) timeout;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/******************** Player Variable Management *************************/

/*------------------------------------------------------------------------
 Set variable 'key' for a player.
------------------------------------------------------------------------*/
DP_API dp_result_t dpSetPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	void   *buf,
    size_t buflen,
	long   flags)
{
	dptab_table_t *playervars;
	playerHdl_t src;
	dp_result_t err;
	dpid_t firstId;
	char tabkey[dptab_KEY_MAXLEN];
	int tabkeylen;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	char_t name[dp_PNAMELEN];
	int idoffset;
	precondition(dp != NULL);
	if ((src = dpid2commHdl(dp, idPlayer)) == PLAYER_NONE) {
		DPRINT(("dpSetPlayerData: err: player %d has no comm handle, can't add\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dpGetPlayerName(dp, idPlayer, name, dp_PNAMELEN) != dp_RES_OK) {
		DPRINT(("dpSetPlayerData: err: no player name for id %d\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	firstId = (dpid_t) (idPlayer & ~(dp_PLAYERS_PER_HOST-1));	/* Skip back to firstId */
	if (firstId != dp->firstId) {   /* not own player */
		DPRINT(("dpSetPlayerData: err: player %d is not my player\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	tabkeylen = 0;
	tabkey[tabkeylen++] = dp_KEY_PLAYER_VARIABLES;
	tabkey[tabkeylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	tabkey[tabkeylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	if (NULL == (playervars = dptab_getTable(dp->dt, tabkey, tabkeylen))) {
		DPRINT(("dpSetPlayerData: err: no player variables table\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if ((idoffset = idPlayer - dp->firstId) > dp_PLAYERS_PER_HOST) {
		DPRINT(("dpSetPlayerData: err: incorrect idPlayer %d for firstId %d\n", idPlayer, dp->firstId));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dpGETSHORT_SECONDBYTE(idoffset)) {
		DPRINT(("dpSetPlayerData: err: idoffset %d too big\n", idoffset));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idoffset);
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(key);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(key);
	if (flags & dp_PLAYERDATA_FLAG_NOFLOOD) {
		err = dptab_set(dp->dt, playervars, subkey, subkeylen, buf, buflen, 0, PLAYER_ME);
	} else {
		err = dptab_set(dp->dt, playervars, subkey, subkeylen, buf, buflen, 2, PLAYER_ME);
	}
	if (err != dp_RES_OK) {
		DPRINT(("dpSetPlayerData: err: can't set player data record, err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}

	dp_assertValid(dp);
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Send a given player's variable 'key' to a player (or to everybody,
 if idTo == dp_ID_BROADCAST).
 Only used for variables that were not sent when they were set, i.e.
 which were set with dpSendPlayerData(..., dp_PLAYERDATA_FLAG_NOFLOOD)
------------------------------------------------------------------------*/
DP_API dp_result_t dpSendPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	dpid_t idTo)
{
	playerHdl_t dests[MY_MAX_HOSTS];		/* FIXME */
	dptab_table_t *playervars;
	playerHdl_t src;
	dp_result_t err;
	dpid_t firstId;
	char tabkey[dptab_KEY_MAXLEN];
	int tabkeylen;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	char_t name[dp_PNAMELEN];
	int idoffset;
	int ndests;
	int i;

	precondition(dp != NULL);

	/* Generate list of destination handles */
	ndests = dp_getHdls(dp, idTo, dests, dp_SEND_RELIABLE);
	if (ndests < 0) {
		/* Why, it isn't a valid id at all.  (don't assume this is caught!) */
		DPRINT(("dpSendPlayerData: unknown dest id:%d\n", idTo));
		dp_assertValid(dp);
		return dp_RES_BAD;
	}
	if (ndests == 0) {
		DPRINT(("dpSendPlayerData: dest id %d maps to no handle?\n", idTo));
		dp_assertValid(dp);
		return dp_RES_OK;
	}

	if ((src = dpid2commHdl(dp, idPlayer)) == PLAYER_NONE) {
		DPRINT(("dpSendPlayerData: err: player %d has no comm handle, can't add\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dpGetPlayerName(dp, idPlayer, name, dp_PNAMELEN) != dp_RES_OK) {
		DPRINT(("dpSendPlayerData: err: no player name for id %d\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	firstId = (dpid_t) (idPlayer & ~(dp_PLAYERS_PER_HOST-1));	/* Skip back to firstId */
	if (firstId != dp->firstId) {   /* not own player */
		DPRINT(("dpSendPlayerData: err: player %d is not my player\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	tabkeylen = 0;
	tabkey[tabkeylen++] = dp_KEY_PLAYER_VARIABLES;
	tabkey[tabkeylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	tabkey[tabkeylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	if (NULL == (playervars = dptab_getTable(dp->dt, tabkey, tabkeylen))) {
		DPRINT(("dpSendPlayerData: err: no player variables table\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if ((idoffset = idPlayer - dp->firstId) > dp_PLAYERS_PER_HOST) {
		DPRINT(("dpSendPlayerData: err: incorrect idPlayer %d for firstId %d\n", idPlayer, dp->firstId));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dpGETSHORT_SECONDBYTE(idoffset)) {
		DPRINT(("dpSendPlayerData: err: idoffset %d too big\n", idoffset));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idoffset);
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(key);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(key);

	for (i=0; i < ndests; i++) {
		err = dptab_send(dp->dt, playervars, subkey, subkeylen, dests[i], 1);
		if (err != dp_RES_OK) {
			DPRINT(("dpSendPlayerData: err: in sending player variable %d\n", err));
			return err;
		}
	}
	return dp_RES_OK;
}

/*------------------------------------------------------------------------
 Get a variable for a player.
 Caller must set *buflen before calling to the size of buf.

 If the player or variable does not exist, dp_RES_EMPTY is returned.
 If *buflen is not big enough, *buflen is set to the current size,
 and dp_res_FULL is returned.
------------------------------------------------------------------------*/
DP_API dp_result_t dpGetPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	void   *buf,
	size_t *pbuflen,
	long   flags)
{
	dptab_table_t *playervars;
	dp_result_t err;
	dpid_t firstId;
	char tabkey[dptab_KEY_MAXLEN];
	int tabkeylen;
	char subkey[dptab_KEY_MAXLEN];
	int subkeylen;
	char *tabbuf;
	size_t tabbuflen;
	int idoffset;
	precondition(dp != NULL);

#if !defined(NDEBUG)
	{
	char_t name[dp_PNAMELEN];
	playerHdl_t src;
	if ((src = dpid2commHdl(dp, idPlayer)) == PLAYER_NONE) {
		DPRINT(("dpGetPlayerData: err: player %d has no comm handle, can't add\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if ((err = dpGetPlayerName(dp, idPlayer, name, dp_PNAMELEN)) != dp_RES_OK) {
		DPRINT(("dpGetPlayerData: err: no player name for id %d\n", idPlayer));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	}
#endif
	tabkeylen = 0;
	tabkey[tabkeylen++] = dp_KEY_PLAYER_VARIABLES;
	tabkey[tabkeylen++] = (char) dpGETSHORT_FIRSTBYTE(dp->sess_karma);
	tabkey[tabkeylen++] = (char) dpGETSHORT_SECONDBYTE(dp->sess_karma);
	firstId = (dpid_t) (idPlayer & ~(dp_PLAYERS_PER_HOST-1));	/* Skip back to firstId */
	if (firstId != dp->firstId) {   /* not own player */
		tabkey[tabkeylen++] = (char) dpGETSHORT_FIRSTBYTE(firstId);
		tabkey[tabkeylen++] = (char) dpGETSHORT_SECONDBYTE(firstId);
		if (NULL == (playervars = dptab_getTable(dp->dt, tabkey, tabkeylen))) {
			DPRINT(("dpGetPlayerData: err: haven't gotten host record yet for id:%d\n", idPlayer));
			dp_assertValid(dp);
			return dp_RES_BUSY;
		}
	} else if (NULL == (playervars=dptab_getTable(dp->dt, tabkey, tabkeylen))) {
		DPRINT(("dpGetPlayerData: err: can't find player variables table\n"));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if ((idoffset = idPlayer - firstId) > dp_PLAYERS_PER_HOST) {
		DPRINT(("dpGetPlayerData: err: incorrect idPlayer %d for firstId %d\n", idPlayer, firstId));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	if (dpGETSHORT_SECONDBYTE(idoffset)) {
		DPRINT(("dpGetPlayerData: err: idoffset %d too big\n",  idoffset));
		dp_assertValid(dp);
		return dp_RES_BUG;
	}
	subkeylen = 0;
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(idoffset);
	subkey[subkeylen++] = (char) dpGETSHORT_FIRSTBYTE(key);
	subkey[subkeylen++] = (char) dpGETSHORT_SECONDBYTE(key);
	err = dptab_get_bykey(playervars, subkey, subkeylen, (void **)&tabbuf, &tabbuflen);
	if (err != dp_RES_OK) {
		DPRINT(("dpGetPlayerData: err: can't get player data record, err:%d\n", err));
		dp_assertValid(dp);
		return err;
	}
	if (tabbuflen > *pbuflen) {
		*pbuflen = tabbuflen;
		return dp_RES_FULL;
	} else {
		*pbuflen = tabbuflen;
		memcpy(buf, tabbuf, tabbuflen);
	}

	(void) flags;

	dp_assertValid(dp);
	return dp_RES_OK;
}

/******************** Miscellaneous *********************/

/*----------------------------------------------------------------------
 Returns dp_RES_BUSY if dp can't freeze due to ongoing reliable transmission.
----------------------------------------------------------------------*/
DP_API dp_result_t dpReadyToFreeze(
	dp_t   *dp)
{
	int i;
	playerHdl_t errHdl;	/* Should return this to caller! */

	dp_assertValid(dp);
	if (dp->dt && dp->dt->peers) {
		for (i = 0; i < dp->dt->peers->n_used; i++) {
			assoctab_item_t *dpe;
			dptab_peer_t *peer;

			dpe = assoctab_getkey(dp->dt->peers, i);
			if (!dpe) {
				DPRINT(("dpReadyToFreeze: bug: can't fetch peer %d; peers->n_used %d\n", i, dp->dt->peers->n_used));
				break;	/* horrible error */
			}
			peer = (dptab_peer_t *)dpe->value;
			errHdl = dpe->key;

			if (peer->tx->n_used) {
				DPRINT(("dpReadyToFreeze: Outstanding table transfers to h:%x.\n", errHdl));
				return dp_RES_BUSY;
			}
		}
	}
	return dpio_ReadyToFreeze(dp->dpio, &errHdl);
}

/*----------------------------------------------------------------------
 Sets a callback to be called for incoming connections.  This is done
 via a callback translation of the dpio_setIncomingCallback mechanism.
----------------------------------------------------------------------*/
DP_API dp_result_t dpSetConnCallback(
	dp_t *dp, dpOpenConnCallback_t cb, void *context)
{
	precondition(dp != NULL);
	dp_assertValid(dp);

	if (dp) {
		dp->openConn_callback = cb;
		dp->openConn_context = context;
		DPRINT(("dpSetConnCallback: callback set\n"));
		dp_assertValid(dp);
		return dp_RES_OK;
	}
	DPRINT(("dpSetConnCallback: no dp!\n"));
	dp_assertValid(dp);
	return dp_RES_BUG;
}

/*-------------------------------------------------------------------------
 Retrieve a parameter set by an external game launcher, if any.
 On success, places the value of the parameter into *pparamVal.
 Returns dp_RES_OK on success.
 Returns dp_RES_UNIMPLEMENTED if that parameter is not supported by this
 driver.
 May return other error codes as well; depends on the driver being used.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetParameterLong(dp_t *dp, int paramId, long *pparamVal)
{
	commSetParamReq_t   req;
	commSetParamResp_t  resp; /* This is a dummy variable */
	int ok;

	precondition(dp);
	precondition(pparamVal);

	memset(&req, 0, sizeof(commSetParamReq_t));
	req.reqLen = sizeof(commSetParamReq_t);
	memset(&resp, 0, sizeof(commSetParamResp_t));
	resp.respLen = sizeof(commSetParamResp_t);

	req.param_num = paramId;
	req.param_value = comm_PARAM_BOGUSVALUE;
	req.param_value2 = comm_PARAM_BOGUSVALUE;
	ok = commSetParam(&req, &resp, dp->dpio->commPtr);
	if (!ok) {
		DPRINT(("dpGetParameterLong: can't get pparamVal; status:%d\n", resp.status));
		return resp.status;
	}
	*pparamVal = resp.param_value;
	DPRINT(("dpGetParameterLong: paramId %d new value %d\n", paramId, *pparamVal));
	return dp_RES_OK;
}

/*----------------------------------------------------------------------
 Get path and info about of currently loaded transport.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetCurrentTransportInfo(
	dp_t *dp,
	dp_transport_t *path,
	comm_driverInfo_t *info)
{
	precondition(dp);
	precondition(path);
	precondition(info);

	*path = dp->dpio->transportDLLname;
	*info = *dp->dpio->driverinfo;
	DPRINT(("dpGetCurrentTransportInfo: path %s, caps %x\n", path->fname, info->capabilities));

	return dp_RES_OK;
}

/************************* Turn Management *******************************/

/* Note: the turn-based stuff is currently only suitable for up to 8
 * players
 */
/*-------------------------------------------------------------------------
 Call to initialize turn-taking mode.
 Must have already called dpOpen and created a player.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnInit(dp_t *dp)
{
	(void) dp;
	DPRINT(("turn not supported yet\n"));
	dp_assertValid(dp);
	return dp_RES_BAD;
}

/*-------------------------------------------------------------------------
 Call to shut down dpturn.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnFinish(dp_t *dp)
{
	(void) dp;
	DPRINT(("turn not supported yet\n"));
	dp_assertValid(dp);
	return dp_RES_BAD;
}

/*-------------------------------------------------------------------------
 Call to send a move to all players.
 Move is queued for transmission by dpTurnPoll.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnPutMove(
	dp_t  *dp,
	char *buffer,
	size_t size)
{
	(void) dp;
	(void) buffer;
	(void) size;
	DPRINT(("turn not supported yet\n"));
	dp_assertValid(dp);
	return dp_RES_BAD;
}

/*-------------------------------------------------------------------------
 Check to see if a move has been received from the given player during this
 round.  If so, put its length in *data_len.
 If *idFrom is dp_ID_NONE, then any player's move is retrieved, and
 the player's id is placed in *idFrom.

 If it will fit in maxlen bytes, copy it to buf, and return TRUE.
 Otherwise, return FALSE.

 Caller may wish to wait until dpturn_poll() returns TRUE before calling
 this.  In that case, he should then call this function for all players
 and call dpturn_sendMove() before calling dpturn_poll() again to avoid
 losing a player's move.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnGetMove(
	dp_t   *dp,
	dpid_t *idFrom,
	void *buffer,
	size_t *size)
{
	(void) dp;
	(void) idFrom;
	(void) buffer;
	(void) size;
	DPRINT(("turn not supported yet\n"));
	dp_assertValid(dp);
	return dp_RES_BAD;
}

/*-------------------------------------------------------------------------
 Send any packets appropriate at this point in the round.
 If not in dpturn mode, or if some player's move has not yet been
 received, or our move has not yet finished sending, returns
 dp_RES_EMPTY.
 If the round just finished, and packets are ready to picked up via
 dpTurnGetMove, returns dp_RES_OK.
 If dp is NULL, returns dp_RES_BUG.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpTurnPoll(dp_t *dp)
{
	(void) dp;
	DPRINT(("turn not supported yet\n"));
	dp_assertValid(dp);
	return dp_RES_BAD;
}

#ifdef dp_STATS
/************************** Statistics ********************************/
/*-------------------------------------------------------------------------
 Retrieve statistics from dp about network traffic.
 On entry,
 statkind is one of dp_STAT_*; for example, if you want to know how many
 reliable packets have been received by the underlying reliable transport
 since dpCreate, use dp_STAT_DPIO_RX_REL_PKTS.
 buf should be a pointer to a dp_stat_t.
 buflen should be sizeof(dp_stat_t).
 Return value is dp_RES_OK on success,
 and dp_RES_BAD if dp is null or id is invalid.
-------------------------------------------------------------------------*/
DP_API dp_result_t dpGetStats(
	dp_t *dp,
	int statkind,
	dp_stat_t *buf,
	size_t buflen)
{
	dp_stat_t *p;
	if (!dp || !buf)
		return dp_RES_BAD;

	switch (statkind) {
	case dp_STAT_DPIO_RX_REL_BYTES   :
	case dp_STAT_DPIO_RX_REL_PKTS    :
	case dp_STAT_DPIO_RX_UNREL_BYTES :
	case dp_STAT_DPIO_RX_UNREL_PKTS  :
	case dp_STAT_DPIO_TX_REL_BYTES   :
	case dp_STAT_DPIO_TX_REL_PKTS    :
	case dp_STAT_DPIO_TX_UNREL_BYTES :
	case dp_STAT_DPIO_TX_UNREL_PKTS  :
	case dp_STAT_DPIO_CONNECTING  :
	case dp_STAT_DPIO_CONNECTED   :
		p = &dp->dpio->stats[statkind]  ; break;
	case dp_STAT_DPTAB_RX_BYTES     :
	case dp_STAT_DPTAB_RX_RECORDS   :
	case dp_STAT_DPTAB_TX_BYTES     :
	case dp_STAT_DPTAB_TX_RECORDS   :
		p = &dp->dt->stats[statkind - dp_STAT_DPTAB_RX_BYTES]  ; break;
	default:
		return dp_RES_BAD;
	}
	memcpy(buf, p, buflen);
	return dp_RES_OK;
}
#endif
