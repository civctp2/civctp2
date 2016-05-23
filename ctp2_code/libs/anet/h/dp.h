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

#ifdef dp_ANET2
#include "dp2.h"	/* If compiling for version 2, use its .h file instead. */
#else
#ifndef dp_h
#define dp_h
/*----------------------------------------------------------------------
 Copyright (C) 1996, Activision.
 $Log: dp.h $
 Revision 1.73  1997/08/22 16:45:21  dkegel
 Become dp2.h if dp_ANET2 set.
 Revision 1.72  1997/07/27 22:57:49  dkegel
 No longer keep commInitReq in dp.  Moved dpdll.h so it
 can access types.
 Revision 1.71  1997/07/10 19:13:14  dkegel
 Use predefined _WIN32 symbol rather than special WIN32 symbol.
 Revision 1.70  1997/06/13 22:56:43  geychaner
 Change incoming connection callback to generic connection callback
 Revision 1.69  1997/06/07 22:16:13  dkegel
 Added support for calculating packet loss to server.  Not returned to user
 yet.
 Revision 1.68  1997/05/31 22:42:19  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.67  1997/05/23 18:31:35  dkegel
 Support timeout on dpEnumServers() now.
 Revision 1.66  1997/05/23 01:34:04  dkegel
 Added dpEnumServers.
 Revision 1.65  1997/04/25 18:24:31  dkegel
 Added dpTurn stuff.
 Revision 1.64  1997/04/10 02:30:29  geychaner
 Made a typedef for the session array structure in dp; added gamesServed
 and startTime variables to dp.
 Revision 1.63  1997/04/04 22:06:11  geychaner
 Added last_sess_sent for dribbling out of game sessions by server.
 Revision 1.62  1997/03/15 03:35:01  geychaner
 Created a callback mechanism for incoming connections.
 Revision 1.61  1997/03/11 22:13:10  geychaner
 Rolled back to r1.59; removed callback from setGameServer.
 Revision 1.59  1997/03/05 03:42:19  dkegel
 Added TRUE for benefit of legacy code
 Revision 1.58  1997/03/05 02:39:06  dkegel
 ini.h and dprint.h now integrated into anet.h;
 to compile or use DP as a DLL, define DP_DLL;
 win\build.bat now creates debugging and nondebugging versions
 of both the static dp.lib and the dynamic anetdll.dll.
 Revision 1.57  1997/03/01 02:56:19  dkegel
 Keep track of handle to node we're enuming players of, so we can close it
 when we get the last packet.  Packets must be sent reliably now to
 avoid reordering.
 Revision 1.56  1997/02/28 02:08:38  gceych
 Added GS_beacon and isNew to support dp.c session list improvements
 Revision 1.55  1997/02/27 01:51:21  dkegel
 THE BIG REARRANGEMENT: split public API into anet.h; dp.h and dppkt.h
 remain for those who need the full API including the real dp_t;
 care must be taken to avoid circular includes and to avoid activating
 too much of anet.h when using the full API.
 Revision 1.54  1997/02/25 05:02:18  gceych
 Added driver capabilities to dp structure
 Revision 1.53  1997/02/24 01:34:00  dkegel
 Made it clear that s must be NULL for dpEnumGroup*, and why.
 Revision 1.52  1997/02/13 02:18:34  dkegel
 No longer pay attention to DP_WIN_COMPAT; always give all four args
 for callbacks.
 Revision 1.51  1997/02/12 03:08:06  dkegel
 1. Added player data.
 2. Moved include's that had crept inbetween the pragma packs; this
 cured a crash bug.
 Revision 1.50  1997/02/10 07:45:20  dkegel
 1. Moved some defines into dppkt.h
 2. moved dpdll functions into dpdll.h
 3. moved reliable data stuff into dpio.h
 Revision 1.49  1997/02/07 04:21:18  dkegel
 EnumSessions' callback parameters now in right order.
 Revision 1.48  1997/02/06 23:17:00  dkegel
 Added missing group functions.
 Made dp_WIN_COMPAT mandatory.
 Revision 1.47  1997/02/05 02:20:49  dkegel
 1. Surround function prototypes with extern "C"...
 2. Get GUID from windows.h in dp.h.
 Revision 1.46  1997/02/04 04:16:13  dkegel
 Added new function dpEnumGroupPlayers(); not implemented yet.
 Revision 1.45  1997/01/31 07:47:40  dkegel
 Settled the structure packing question by using
 #pragma pack(x) / #pragma pack() pairs at top and bottom of
 all .h files that declare structures.  Was careful to put
 #pragma pack(x) after last #include (to avoid wierd nesting effects
 due to lack of #pragma pack(pop).
 Revision 1.44  1997/01/30 01:56:48  dkegel
 1. Partially implemented player groups.
 2. Rearranged functions in dp.h to be grouped more logically.
 3. Added DP_WIN_COMPAT flag to enable compatibility with other library;
 later, the compatibility will not be optional.  This is just so I
 don't screw up existing code for the moment.
 Revision 1.43  1997/01/03 00:45:17  dkegel
 Expose master/slave status
 Revision 1.42  1996/12/18 18:20:25  administrator
 Change to c style comments for benefit of gcc.
 Revision 1.41  1996/12/18 18:00:08  administrator
 1. Use PACK after every member of structures that get sent over the wire,
 to force gcc to not insert extra bytes.
 PACK is defined in makefile.g32.
 2. Added dpDeclareLobby() and dp.isServerLobby to support writing game
 servers.
 3. Moved hMaster out of dp_session_t because it changed size,
 and didn't really need to be sent anyway.  Replaced with dummy.
 4. Added lastSeen field to sessions array to support game servers.
 Revision 1.40  1996/12/06 22:13:51  dkegel
 Document dpDestroy's argument.
 Revision 1.39  1996/10/10 23:11:39  dkegel
 Added support for dpPingUser().
 Revision 1.38  1996/09/28 03:14:00  dkegel
 Throw HOST_NOT_RESPONDING error if game server does not return packets
 Revision 1.37  1996/09/25 22:48:14  dkegel
 1. Support the notion of unreachable hosts.  In particular, dpEnumSessions()
 will return dp_RES_HOST_NOT_RESPONDING if the game server is unreachable.
 2. Export dpSetGameServer(), and extend it so that one may clear the game
 server by passing it NULL.
 Revision 1.36  1996/09/08 19:36:14  dkegel
 Save dp->*GameServer across freeze/thaw cycles.
 Revision 1.35  1996/07/23 00:33:04  dkegel
 Initial support for internet game server.
 Revision 1.34  1996/06/29 16:12:55  dkegel
 Added dp_RES_NETWORK_NOT_RESPONDING.  Supported by null modem driver
 at the moment.
 Revision 1.33  1996/06/25 20:06:49  dkegel
 Added handler for comm_STATUS_BAD_VERSION in dp.
 Revision 1.32  1996/05/31 02:21:56  dkegel
 Added new function dpReportScore to pass score down to comm layer;
 not well implemented, just barely works for dwango, but that's all we need.
 Revision 1.31  1996/05/29 04:02:08  dkegel
 Actively reject add player requests by sending a dp_error_packet_t.
 Revision 1.30  1996/05/21 21:08:45  dkegel
 Translate new comm_STATUS_NETWORK_NOT_PRESENT error from commInit()
 into new dp_RES_NETWORK_NOT_PRESENT.
 Revision 1.29  1996/05/15 02:16:08  dkegel
 1. unLoadDLL in commInit when returning an error.
 2. translate comm_STATUS error codes to dp_RES error codes in dpCreate.
 3. Added new dp_RES error codes for modems.
 4. Increased size of player and session names.
 Revision 1.28  1996/05/09 02:02:11  dkegel
 1. Added hangup flag to dpDestroy.
 2. Set baud rate and portnum properly when thawing.
 3. Check for NULL dp in dpSend().
 Revision 1.27  1996/05/04 23:14:00  dkegel
 1. Converted a few ifdef DEBUG's into ifdef PLOSSCTR so we could
 intermix debugging and non-debugging .obj's.
 2. Fixed bug in updating of next_pktnum when receiving a reliable packet.
 Symptom was lockup if packets came in out of order!
 3. Made reliable packets stored previously in the window higher priority
 than fresh network packets.
 Revision 1.26  1996/05/04 18:41:37  dkegel
 1. Increased player timeout from 20 to 30 seconds -- sim takes 24 secs
 to launch on my 486.  (Ought to set this high during launch, back
 down to normal after launch finishes.)
 2. Added dpReadyToFreeze() call to see if dpFreeze would succeed.
 3. Even when frozen, still ack old packets.
 Revision 1.25  1996/04/23 21:27:55  dkegel
 1. The add player packet now includes sessionType and Karma
 to prevent players from accidently joining the wrong game.
 2. Fixed a number of places where we used the wrong packet
 structure.  (didn't use to matter)
 Revision 1.24  1996/04/18 00:48:16  dkegel
 Added reliable transport flag to dpSend.
 Rewrote dpReceive slightly to use early returns rather than
 setting a return value and falling through to end of function.
 Revision 1.23  1996/04/12 17:24:29  dkegel
 Added sessionType field to enumplayers packet to let us keep launched and
 unlaunched games from seeing each other.  (e.g. By incrementing
 myDP->s.sessionType immediately before calling dpFreeze, and decrementing
 it thereafter.)
 Also check sessionType and karma a little more carefully before
 responding to enumsession packets.
 Revision 1.22  1996/04/09 02:29:49  dkegel
 1. Added password and other user fields to dp_session_t.
 2. Added dp_MAXREALPLAYERS.
 3. Added new methods dpEnableNewPlayers() and dpGetPlayerName().
 Revision 1.21  1996/03/25 19:17:55  dkegel
 1. Raised timeout to allow shell displaying scores to still be in session.
 Not bad for WAN use, either, I bet.
 2. Set dp_MAXPACKETLEN more or less properly.
 Revision 1.20  1996/03/21 03:25:14  dkegel
 1. Added dp_PLAYERROSTER_PACKET.  This let us raise the max number of
 players from about 4 to about 32 with a underlying packet size of 256.
 It also reduces the number of bytes per second consumed by overhead.
 2. Improved the documentation of each of the packet types in dp.h
 3. No longer call dpDumpPlayerTable() etc.
 Revision 1.19  1996/03/14 20:23:48  dkegel
 1. New arguments and functionality for dpEnumPlayers.  Now you don't have
 to join a session to enumerate its players.
 2. Fixed bug that caused truncation of last player's callsign.
 Revision 1.18  1996/03/09 01:20:46  dkegel
 Added packet loss counter.
 Revision 1.17  1996/03/06 23:46:14  dkegel
 1. Support the notion of dp_KARMA_NONE.
 2. For the benefit of nosey user programs, give dp->s.hMaster a valid value
 even when we are the master.
 Revision 1.16  1996/03/05 19:52:18  dkegel
 1. dpThaw is now part of dpCreate.
 2. dpGetTransportInfo is now public.
 Revision 1.15  1996/02/29 01:43:11  dkegel
 1. Added new capabilities signature support in dpEnumTransports.
 2. Added new parameters for commInit.
 3. Slightly better DPRINTs, freetime reporting in test_p.c.
 Revision 1.14  1996/02/24 17:47:37  dkegel
 1. Added dpCommThaw() method for use with Dwango and its ilk.
 2. Fixed bug in address matching that affected create and destroy player;
 garbage bytes were being treated as significant, and causing the
 operation to not destroy a player or create more than one of a player.
 Only showed up with 'short' addresses.
 Revision 1.13  1996/02/22 23:04:25  dkegel
 1. dp_enumsess_packet_t now contains the link address of the
 client requesting the session description.  That way, the master
 can reply with a unicast response.
 2. Added masterHostName arg to dpEnumSessions, which uses it to
 send a dp_enumsess_packet_t directly to the master.
 3. Added -s flag to dp_test.c to set masterHostName.  Needd to
 test dwango driver in the absence of a dwango host.
 Revision 1.12  1996/02/20 05:46:47  dkegel
 dp_host_t was never used.
 Revision 1.11  1996/02/16 20:27:22  dkegel
 1. No limit on DPID's. dpid_t now unsigned.
 2. Trust outgoing packet queue if MULTICAST_BLAST defined.
 3. Local messages are delivered even if network is busy.
 4. Now send keepalives to master if user hasn't lately.
 5. Added session karma to user packet envelope.
 6. Don't let user see packets from strangers.
 7. Retry player creation if no reply.
 8. Kludge: doubled BROADCAST_THRESH, because it counts dpid's rather than
 destinations.  Should keep a count of real and pseudoplayers so THRESH can
 be on destination count rather than n_players.
 Revision 1.10  1996/02/12 03:17:38  dkegel
 Changes needed to run with new IPX driver.
 1. commSayHi now can be called multiple times on same adr without risk.
 2. packets from strangers don't get handles- we have to allocate
 handles for them with adr2hdl if we want to reply to them.
 3. Master now identifies himself by ethernet address in session packet.
 Revision 1.9  1996/02/03 05:53:09  dkegel
 Added dpNumPlayers().
 Revision 1.8  1996/02/02 21:14:13  dkegel
 Increased lurker limit.  Documented kludge nature of a few limits.
 Revision 1.7  1996/02/02 21:06:44  dkegel
 1. Don't call callbacks if null.  Duh.
 2. More regular names for session flags.  (Two collided!)
 Revision 1.6  1996/01/31 00:25:17  dkegel
 1. Got rid of state variable in dp.h
 2. Added message types used to notify user program of player additions
 and deletions.
 3. Many small fixes in how a new master is elected.  Master and player
 timeouts appear to work properly now.
 4. Added new constant dpid, dp_ID_LOCAL_NAMESERVER, for internal plumbing.
 Revision 1.5  1996/01/30 06:32:24  dkegel
 1. dpid_t now short.  Saves four bytes in packet.
 (Should it be a char, even?  Or converted to one?)
 2. Increased dp_MAXDPIDS to 24.  Check when allocating new DPID!
 Should remove this limit- nothing should care how large these get.
 3. Added local message queue.  Messages sent with dpSend to
 players on the local machine get put on a special local message queue.
 This is intended for use with player timeouts, or anything else that
 generates local system messages.
 Revision 1.4  1996/01/30 01:14:28  dkegel
 added player/machine timeouts.
 use new dynatab_t to store timeout table.
 Revision 1.3  1996/01/29 18:49:20  dkegel
 added dpSetClocksPerSec() for programs like mw2's sim which change the speed
 of the system clock.
 Revision 1.2  1996/01/27 02:16:13  dkegel
 minor fix to comments.
 Revision 1.1  1996/01/25 01:56:44  dkegel
 Initial revision

 Multiplayer game layer. Uses mw2 comm layer for basic networking.
----------------------------------------------------------------------*/

#include <stddef.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef DP_API
#if defined(_WIN32) && defined(DP_DLL)
#define DP_API __declspec( dllexport )
#else
#define DP_API
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "commapi.h"
#include "assoctab.h"
#include "q.h"

/* Temporary flag to enable compatibility changes.  These will be permanent soon.
 */
#define DP_WIN_COMPAT

#define dp_NO_FUNCTIONS
#include "anet.h"
#undef dp_NO_FUNCTIONS

#include "dppkt.h"

#include "dpio.h"
#include "dppv.h"
#include "dpturn.h"

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.  Let's pick 4.
 */
#include "dppackn.h"

/*----------------------------------------------------------------------
 Object used for measuring round trip delay.
----------------------------------------------------------------------*/
#define dp_PING_MAX_NTOTAL 10
typedef struct {
	dp_karma_t karma;	/* send packets with this, ignore responses that differ. */
	int n_total; /* how many to send. */
	int n_sent;  /* how many sent so far */
	int n_got;   /* how many we got so far */
	time_t sent_at[dp_PING_MAX_NTOTAL];
	time_t got_at[dp_PING_MAX_NTOTAL];
	time_t next_send_time;
	time_t deadline;		/* when we give up and call the callback */
	dp_ping_callback_t cb;
} dp_ping_t;

/* Used for holding game server replies from dpEnumServers.
 * Each call to dpEnumServers triggers at most one ping.
 * Results of the pings are tallied in this structure.
 */
#define dp_SERVER_NPINGS 5			/* how much history to keep */
typedef struct dp_enumserver_s {
	const char *hostname;			/* Printable address of server */

	short rtt_ms_avg;				/* Average measurement */
	short loss_percent;				/* Average packet loss */

	short i_rtt;					/* next entry in rtt_ms to overwrite */
	short rtt_ms[dp_SERVER_NPINGS];	/* roundtrip time, msec; -1 if empty */

	byte adr[dp_MAX_ADR_LEN];		/* lowest level transport address */
} dp_enumserver_t;

/* Used for holding sessions received by a game server */
typedef struct dp_enumsession_s {
	dp_session_t s;			/* The packet received from the host. */
	clock_t lastSeen;		/* When it was received. */
	unsigned isNew : 1;		/* Newly added session */
} dp_enumsession_t;			/* Used to avoid duplicate reports */

/********************************************************************* */
/* The class structure itself */

#define dp_MAGIC	4531
#define dp_MAX_SERVERS 256

typedef struct dp_s {
	int			 magic;			/* sentinel to make sure structure valid */
	dpExceptionCallback_t	exception_cb;	// set this to catch fatal errors */
	dp_karma_t	 myKarma;		/* random number; used as sessionKarma if leader */

	int 		 bMaster;		/* whether I'm a session master */
	dpid_t		 nextId;		/* next id to hand out when creating players */
	dp_session_t s;				/* session I'm connected to or hosting, if any. */
	playerHdl_t  hMaster;		/* used to be in dp_session_t */
	byte		 myAdr[dp_MAX_ADR_LEN];		/* lowest level transport address */
	int			 myAdrLen;
	int			 pollState;	/* what housekeeping packet to send this time. */
	int			 clocksPerSec;	/* call dpSetClocks with multiplier to indicate relative speed. */
	int			 enableNewPlayers;
	int			 isServerLobby;
	int			 sentToMaster;	/* whether we have sent a packet to the master lately. */

	/* Our local list of players on this machine. No remote players included. */
	int          my_nPlayers;
	dp_playerId_t	my_players[dp_MAXPLAYERS];

	/* Copy of last player list from master. */
	int          nPlayers;
	dp_playerId_t	players[dp_MAXPLAYERS];

	/* How to map from dpid's to comm layer addresses. */
	/* Indexed by dpid.  Update whenever nPlayers is incremented. */
	/*playerHdl_t	 commHdl[dp_MAXDPIDS]; */
	assoctab_t	*dpid2commHdl;

	/* Capabilities of communications (transport) driver. */
	long drivercaps;

	/* Following fields support dpEnumServers */
	dp_enumserver_t servers[dp_MAX_SERVERS];
	int n_servers;
	dpEnumServersCallback_t enumServers_callback; /* Called for each server */
	void *enumServers_context;
	clock_t enumServers_deadline;	/* when to give up on replies. */

	/* Callback called when connections are opened */
	dpOpenConnCallback_t openConn_callback;
	void *openConn_context;

	/* Called when createPlayer finishes. */
	dpEnumPlayersCallback_t createPlayer_callback;
	void *createPlayer_context;
	clock_t		 createPlayer_deadline;	/* when to grovel. */
	int createPlayer_retries;	/* how many times we grovel before giving up. */

	/* Called when open completes. */
	dpEnumSessionsCallback_t openSession_callback;
	void *openSession_context;

	/* Following fields support enumSessions. */
	dp_enumsession_t sessions[dp_MAXSESSIONS];
	int			 n_sessions;
	int			 last_sess_sent;	/* The last session sent to clients */
	clock_t		 next_GS_beacon;	/* Whether it's time to send server */
	clock_t		 GS_beacon_interval;	/* How often to broadcast server */
	clock_t      next_beacon;		/* whether it's time to broadcast */
	clock_t      beacon_interval;	/* how often to broadcast session name. */
	void		*enumSessions_context;
	dpEnumSessionsCallback_t enumSessions_callback;
	clock_t		 enumSessions_deadline;	/* when to give up on replies. */
	dp_species_t enumSessions_sessType;
	int			 syncAfter;
	int			 replyAfter;
	playerHdl_t	 replyTo;
	playerHdl_t  hGameServer;
	byte		 adrGameServer[dp_MAX_ADR_LEN];
	int			 gameServerReqCt;	/* How many enumSession packets we've sent */
	int			 gameServerRespCt;	/* How many Session packets we've gotten */
	int			 gamesServed;		/* How many games have been played */
	time_t		 startTime;			/* The time at which this server was last reset */

	/* Following fields support enumPlayers. */
	dpEnumPlayersCallback_t enumPlayers_callback;
	void *enumPlayers_context;
	clock_t		 enumPlayers_deadline;	/* when to grovel. */
	dp_karma_t	 enumPlayers_karma;		/* karma of session we're interested in. */
	playerHdl_t	 enumPlayers_handle;	/* handle to master we're interested in. */

#if 0
	/* Following fields support multi-cast simulation by dpSend. */
	byte		 mc_buf[dp_MAXPACKETLEN];	/* Holding pen while multicasting */
	int 		 mc_len;	/* Length of user data. */
	int			 mc_next;	/* Index into players[].  Next guy to send it to, or -1 if done. */
	int		 mc_sysMessage;	/* whether system message */
#endif

	/* Following field supports reliable data transmission. */
	dpio_t		*dpio;

	/* Round-trip transit time measurement. */
	playerHdl_t	ping_dest;	/* who we're pinging, or PLAYER_NONE */
	dp_ping_t	ping;		/* details of current ping session */

	clock_t			now;	/* Set at top of dpReceive, used to avoid */
							/* multiple calls to clock(). */

	/******* Group Management *******/
	assoctab_t	 *groups;	/* table of dp_group_t */

	/******* Player Variables *******/
	pv_t		*pv;

	/******* Turn Management ********/
	dpturn_t     dpt;

} dp_t;
#define dp_t_defined

// How to tell if your machine is currently the master / host
//#define dpIsMaster(dp) (dp->s.hMaster == PLAYER_ME)
#define dpIsMaster(dp) (dp->bMaster)

/* Return to default packing. */
#include "dpunpack.h"

#ifdef __cplusplus
}
#endif

/* Finally, get function prototypes */
#define dp_NO_TYPES
#include "anet.h"
#include "dpdll.h"
#undef dp_NO_TYPES

#endif
#endif
