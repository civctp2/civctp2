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

#ifndef dp_h
#define dp_h
/*----------------------------------------------------------------------
 Copyright (C) 1996, Activision.

 Multiplayer game layer. Uses dpio layer for basic networking,
 and dptab layer for shared table services.

 $Log: dp2.h $
 Revision 1.17  1997/10/08 03:24:55  lbennett
 Adding extra info to dpEnumApps.
 Revision 1.16  1997/09/28 03:56:38  dkegel
 Support new enumServer method
 Revision 1.15  1997/09/25 03:49:45  lbennett
 Freeze/thaw modifications.
 Revision 1.14  1997/09/19 03:56:50  anitalee
 made changes to dp_t, etc for groups
 Revision 1.13  1997/09/04 06:45:55  dkegel
 1. Support dptab_requestSubscription().
 2. Use it to transfer a table of game servers from the server we're
 connected to, if any.  Crude, but it's better than nothing.
 Revision 1.12  1997/09/02 23:01:45  dkegel
 Enable user-level handle callbacks so server can set sockopts.
 Revision 1.11  1997/09/02 21:38:43  dkegel
 Allow server addresses to be any length
 Revision 1.10  1997/09/01 00:36:49  dkegel
 Hosts which time out now have their player and host records 
 removed again.
 Revision 1.9  1997/08/31 04:45:39  dkegel
 Big warning on dp_PLAYERS_PER_HOST; must be power of two.
 Revision 1.8  1997/08/25 01:08:21  dkegel
 Added beacon_number so dpPoll() can schedule tasks for every other
 heartbeat, every fourth heartbeat, etc.
 Revision 1.7  1997/08/23 00:42:15  dkegel
 Added back the array that maps dpid's to commHdl's.
 Revision 1.6  1997/08/22 21:12:20  dkegel
 1. Avoid calling dpClose in callbacks by setting new private flag
 pleaseClose
 2. Avoid timing out game server
 3. Sent "session lost" message to user code if master goes away
 Revision 1.5  1997/08/22 16:51:21  dkegel
 Support dpPingUser and dpEnumServers.
 Revision 1.4  1997/08/20 22:36:24  dkegel
 1. Added enumServers and enumSessions state variables.
 2. Deleted unused ping and session structs (they might come back later).
 Revision 1.3  1997/08/19 22:04:34  dkegel
 Implemented player system.
 Revision 1.2  1997/08/18 01:47:51  dkegel
 Session joining starting to come alive...
 Revision 1.1  1997/08/11 01:29:29  dkegel
 Initial revision
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
#include "hkeytab.h"
#include "q.h"

#define dp_NO_FUNCTIONS
#include "anet.h"
#undef dp_NO_FUNCTIONS

#include "dppkt.h"

#include "dpio.h"
#include "dptab.h"

#include "../src/tca/tserv.h"
#include "../src/score/scorerep.h"

/* The structures in this file are not meant to be sent over the net
 * nor written to disk.  We need to protect ourselves from programs
 * that set the default packing to strange values, so always set it to
 * an arbitrary value here.  Let's pick 4.
 */
#include "dppackn.h"

/* Names (or pieces of names) of standard tables used internally */
#if 0  /* moved to anet.h */
#define dp_KEY_SESSIONS			1	/* known sessions */
#define dp_KEY_PLAYERS			3
#define dp_KEY_SERVERPINGS		10	/* servers: ip adr, name, ping results */
#define dp_KEY_SCORES			14	/* everyone's player score records */
#define dp_KEY_USERS			30	/* user records */
#endif

#define dp_KEY_MYSESSIONS		2	/* sessions on this machine */
#define dp_KEY_MYPLAYERS		4
#define dp_KEY_HOSTS			5	/* known machines */
#define dp_KEY_SERVERS			6	/* Table containing ip adrs of servers */
#define dp_KEY_GROUPS			7
#define dp_KEY_GROUP_PLAYERS	8
#define dp_KEY_GROUP_MYPLAYERS	9
#define dp_KEY_APPLICATIONS		11	/* server-supplied application info */
#define dp_KEY_PLAYER_VARIABLES	12	/* player variables */
#define dp_KEY_EXCEPTIONS		13	/* crash exception records */
#define dp_KEY_MYSCORES			15	/* player score records for this host */
#define dp_KEY_UID2SESSIONID	20	/* what session user last tried to join */

/* To make creating players easier, we preallocate a block of 4 id's for
 * each host that joins a session.  This limits us to
 * 2^14 or 16384 hosts per session.  
 * THIS MUST BE A POWER OF TWO, so it's easy to map back to firstId from id
 * with  firstId = id & ~(dp_PLAYERS_PER_HOST-1);
 */
#define dp_PLAYERS_PER_HOST 4

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

/* Used for holding game server replies from dpEnumServers. */
#define dp_SERVER_NPINGS 5			/* how much history to keep */

struct dp_s;

#define dp_MAX_GROUPS 4				/* > number of groups in a session */

/* Structure that holds quick-access pointers for a session.
 * Used as context pointer for that session's player and hosts table callback.
 * This is the *real* dp_session_t; dpOpen and dpClose should operate on
 * these, but we haven't made that leap yet.
 */
typedef struct {
	struct dp_s *dp;
	dptab_table_t *hosts;
	dptab_table_t *players;
	dptab_table_t *myplayers;
	dptab_table_t *groups;
	dptab_table_t *grplayers[dp_MAX_GROUPS];
	dptab_table_t *grmyplayers[dp_MAX_GROUPS];
	dptab_table_t *playervars;
	char sess_subkey[dptab_KEY_MAXLEN];
	int sess_subkeylen;
	int old_numplayers;  /* currentPlayers from dp_session_t at last dpPoll */
	dp_karma_t sessionKarma;
	int hostid;			/* Next hostid (dpid/dp_PLAYERS_PER_HOST) to assign */
	dp_species_t sessionType;
} dp_sessionContext_t;

/********************************************************************* */
/* The class structure itself */


#define dp_MAGIC	4531
#define dp_MAX_SERVERS 256
#define dp_MAX_SERVERNAMELEN 64


typedef struct dp_s {

	clock_t now;

	dpio_t *dpio;

	dptab_t *dt;

	/* Our address as seen by an outside authority, like 
	 * the first game server we connect to.
	 * Default value is our internal address; overwritten
	 * each time we connect to a game server or host
	 * by an address it sends in its host record for us.  (Kludge!)
	 */
	char my_outside_adr[dp_MAX_ADR_LEN];

	/* Special purpose tables within dt */
	dptab_table_t *sessions;	/* dp_session_t; Sessions known in general */
	dptab_table_t *mysessions;	/* dp_session_t; Sessions created locally */

	dptab_table_t *players;		/* dp_playerId_t; Players in this session */
	dptab_table_t *myplayers;	/* dp_playerId_t; Players created locally */

	dptab_table_t *hosts;		/* dp_host_t; Hosts known in this session */
	dptab_table_t *groups;		/* dpid_t; Groups in this session */

	/* Info about current game server. */
	playerHdl_t hGameServer;	/* Handle to current game server */
	char adrGameServer[dp_MAX_ADR_LEN];	/* address of same */
	char nameGameServer[dp_MAX_SERVERNAMELEN];	/* DNS name of same */

	/* Periodic housekeeping. */
	clock_t      next_beacon;		/* whether it's time to do chores */
	clock_t      next_beacon2;		/* whether it's time to do harder chores */
	clock_t      next_beacon4;		/* whether it's time to do hardest chores */
	clock_t      beacon_interval;	/* how often to do periodic stuff. */
	int 		 check_timeout; 	/* look for timed out handles early */
	int dpReceive_prevcall;		/* boolean, has dpReceive been called before? */

	dp_sessionContext_t *pSess;	/* The current session */

	/* All sessions currently open or cast off. */
	dynatab_t *sessionContexts;	/* dp_sessionContext_t[] */

	/* Info about which session each client is in. */
	assoctab_t *hdl2sess;		/* dp_sessionContext_t[playerHdl_t] */

	/* Info about current session.  (Much of this should be replaced with
	 * references to pSess.)
	 */
	playerHdl_t hMaster;		/* Handle to current session master */
	char sess_subkey[dptab_KEY_MAXLEN];	/* Subkey in sessions (and mysessions, if master) */
	int sess_subkeylen;
	dp_karma_t sess_karma;		/* for quick access by dpRecieve :-( */
	dp_karma_t joinKarma;		/* so we can delete imposters */
	int			pleaseJoinAny;	/* if set, join a server lobby on next dpPoll */
	short		pleaseJoinAnyMask;	/* session flags mask to use with above */
	dp_species_t	pleaseJoinAnySessType;
	int			enableNewPlayers;

	dpid_t		 firstId;		/* first id to hand out when creating players */
	dpid_t		 nextId;		/* next id to hand out when creating players */
	dpid_t		 firstGId;		/* first id to hand out when creating groups */
	dpid_t		 nextGId;		/* next id to hand out when creating groups */
	int			pleaseClose;	/* set this to close on next dpPoll */
	int			closing;		/* set during dpClose; read by dp_sessions_cb */

	/* How to map from dpid's to comm layer addresses. */
	/* Indexed by dpid.  Update whenever nPlayers is incremented. */
	/*playerHdl_t	 commHdl[dp_MAXDPIDS]; */
	assoctab_t	*dpid2commHdl;

	/* Handle flags to use for connections to players in our session */
	int peerHandleMode;			/* dpio_OPTION_* */

	/* Callback called when connections are opened */
	dpOpenConnCallback_t openConn_callback;
	void *openConn_context;

	/* Called when dpOpen completes. */
	dpEnumSessionsCallback_t dpOpen_cb;
	void *dpOpen_context;

	/* Called when dpCreatePlayer completes. */
	dpEnumPlayersCallback_t dpCreatePlayer_cb;
	void *dpCreatePlayer_context;

	/* Following fields support dpEnumApps */
	dptab_table_t *apps;		/* list from server */

	/* Following fields support dpEnumServers */
	dptab_table_t *servertab;		/* from server */
	dptab_table_t *serverpings;		/* server + bootstrap + ping results */
	dpEnumServersCallback_t oldenumServers_callback; /*Called for each server*/
	void *oldenumServers_context;
	clock_t enumServers_deadline;	/* when to give up on replies. */

	/* Following fields support enumSessions. */
	void		*enumSessions_context;
	dpEnumSessionsCallback_t enumSessions_callback;
	clock_t		 enumSessions_deadline;	/* when to give up on replies. */
	dp_species_t enumSessions_sessType;

	/* Following fields support dpPingUser(). */
	playerHdl_t	ping_dest;	/* who we're pinging, or PLAYER_NONE */
	dp_ping_t	ping;		/* details of current ping session */

	/* Following fields support elections aka. host migration. */
	clock_t election_yield_deadline;	/* when people who can't see majority candidate die */
	clock_t election_deadline;	/* when election is over */
	int election_size;			/* number of votes needed to win election, or 0 if no election */
	assoctab_t *election_votes;	/* indexed by handle; holds dp_election_vote_t; cleared on dpClose */
	dp_session_t election_old_session;	/* Used by winner of election */
	
	/* Following fields support session record compression */
	dp_species_t defaultSessionType;

	/* Following fields support master-selected sessions */
	int select_keylen;
	char select_key[dptab_KEY_MAXLEN];

	/* Following fields support dpEnumServersEx */
	dpEnumServersExCallback_t enumServersEx_callback; /*Called for each server*/
	void *enumServersEx_context;

	/* Following fields support thread safety */
#ifdef _WIN32
	unsigned long threadId;		/* id of thread that called dpCreate() */
#endif
	
	/* Following fields support score reporting */
	scorerep_t *scorerep;		/* dpScoreReport creates, dpClose deletes */
	dptab_table_t *myscoretab;	/* local score reports */
	dptab_table_t *uid2sessionid;	/* tracks what session user is in */

	/* Following fields support dpRequestObjectDeltas() */
	int monitor_object_players;	/* TRUE if monitoring player table */
	int monitor_player_latencies; /* TRUE if monitoring player latencies */
	int monitor_object_sessions;/* TRUE if monitoring session table */
	int monitor_object_servers;	/* TRUE if monitoring serverping table */
	dp_species_t monitor_object_servers_sessType; /* for player count */
	/* Monitoring remote player lists controlled by callback on
	 * the particular remote player table.
	 */
	/* Following fields used for dpEnumServersPoll() */
	clock_t      next_serverping;			/* when to send next ping burst */
	clock_t      serverping_interval;		/* 1..10 sec; adjusted on the fly */
	int 		 serverping_rx_count; 		/* number of pings received */
	int 		 serverping_rx_count_old;	/* ditto at last dpEnumServersPoll */

	/* Following fields support user login */
	tserv_t *tserv;
	tca_t *tca;

	/* Following fields used by dpShutdown for nice connection closing */
	int quitState;
	clock_t quitDeadline;

 	/* Following fields support packet buffering */
	clock_t appLastFlushed;		/* time the app last called dpFlush */
} dp_t;

#define dp_t_defined

/* How to tell if your machine is currently the master / host */
#define dpIsMaster(dp) (dp->hMaster == PLAYER_ME)

/* Flag for dpReceive, but only for callers who know about dpio, so
 * hide it here in dp2.h
 */
#define dpReceive_FLAGS_DPIO 4

/* Launch parameters written by anetdrop and read by dpCreate */
typedef struct {
	int Host;
	int Join;
	int Wait;
	int Maxplayers;
	int EnablePlayervars;
	int JoinAnyFlags;
	char Driver[100];
	char Sessname[100];
	char Playname[100];
	char Adr[100];
	char GameServer[100];
	commInitReq_t commInitReq;
	char Phonenum[100];
	char Modeministr[100];
	char OpenAddresses[1024];
	char Username[17];  /* tcapw_LEN_USERNAME + 1 */
	char Password[17];  /* tcapw_LEN_PW + 1 */
	char sessId[dp_MAX_ADR_LEN+2];
} dp_launchParams_t;

#define dp_LAUNCHPARAMS_T_SIG "launchParams"
#define dp_LAUNCHPARAMS_JOIN_ANY 0x02

/* Return to default packing. */
#include "dpunpack.h"

/*-------------------------------------------------------------------------
 Only for game servers.
 When a new client comes online, send it our SESSIONS table, 
 and when it sends us its MYSESSIONS table, put it in our MYSESSIONS table.
 (If we're the master gameserver, dump it into the sessions table
 instead.)
-------------------------------------------------------------------------*/
#define dp_addClient	_daC
dp_result_t DP_APIX dp_addClient(dp_t *dp, playerHdl_t src,
									char *body, size_t len);

#ifdef __cplusplus
}
#endif

/* Finally, get function prototypes */
#define dp_NO_TYPES
#include "anet.h"
#include "dpdll.h"
#undef dp_NO_TYPES

#endif
