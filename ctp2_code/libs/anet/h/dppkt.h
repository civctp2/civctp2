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
 Private packet definitions for the dp multiplayer networking layer.
 $Log: dppkt.h $
 Revision 1.31  1997/08/23 20:47:01  dkegel
 1. Added dp_LEAVE_PACKET_ID 
 2. Added dpSendLeaveSession() and dpHandleLeaveSession()
 3. Added code in dpClose() to more thoroughly undo dpOpen()
 Revision 1.30  1997/08/19 22:04:57  dkegel
 Player system now preallocates a block of id's for each host.
 Revision 1.29  1997/08/18 01:48:49  dkegel
 Added in a couple dp2 packets.
 Revision 1.28  1997/06/13 22:57:57  geychaner
 Added FIN_PACKET_ID for active close FIN packet
 Revision 1.27  1997/05/31 22:42:21  dkegel
 Moved 'pragma pack' to dp*pack*.h for portability.
 Revision 1.26  1997/04/25 18:24:48  dkegel
 Added dpTurn stuff.
 Revision 1.25  1997/03/01 02:57:11  dkegel
 Added dp_SERVER_LOGOUT_PACKET_ID as kludge until active close implemented.
 Revision 1.24  1997/02/27 01:51:25  dkegel
 THE BIG REARRANGEMENT: split public API into anet.h; dp.h and dppkt.h
 remain for those who need the full API including the real dp_t;
 care must be taken to avoid circular includes and to avoid activating
 too much of anet.h when using the full API.
 Revision 1.23  1997/02/26 23:05:18  dkegel
 Moved pv_user_playerData_packet_t to dppkt.h as dp_user_ etc.
 Revision 1.22  1997/02/17 07:57:02  dkegel
 New size limit defines: dp[io]_MAXLEN_[UN]RELIABLE
 Revision 1.21  1997/02/17 06:09:25  dkegel
 Corrected packet length upper bounds to match real limits.
 Fragile; size of headers & envelopes is hardcoded to avoid circular
 references.
 Revision 1.20  1997/02/16 03:59:16  dkegel
 1. Added sessionKarma to dp_group_t because I'm sending it
 instead of dp_groupId_t for addgroup packet.
 2. Reduced max group members to dp_MAXREALPLAYERS for same reason.
 Revision 1.19  1997/02/16 01:41:32  dkegel
 Added one more alias for flags field of dp_session_t.
 Revision 1.18  1997/02/12 21:37:53  dkegel
 Added host migration message.
 Revision 1.17  1997/02/12 03:07:02  dkegel
 1. Added sessionKarma to group packets as insurance against bogus packets.
 2. Added SYN packet for starting reliable session.
 Revision 1.16  1997/02/10 07:46:17  dkegel
 1. Moved some definitions (dp_FAR, etc) in from dp.h for benefit of dpdll.
 2. Renamed rdp_ stuff to dpio_.
 Revision 1.15  1997/02/05 02:21:46  dkegel
 Added an alias in dp_playerId_t for the benefit of the other library's
 createplayer message.
 Revision 1.14  1997/02/04 04:18:44  dkegel
 1. Use the DP_ALIAS macro to declare anonymous unions; less cluttered,
 lets them be disabled when compiling with Gnu C, which doesn't support
 them.
 2. Added a few more aliases to support porting from the other library.
 Revision 1.13  1997/01/31 07:47:41  dkegel
 Settled the structure packing question by using 
 #pragma pack(x) / #pragma pack() pairs at top and bottom of
 all .h files that declare structures.  Was careful to put 
 #pragma pack(x) after last #include (to avoid wierd nesting effects
 due to lack of #pragma pack(pop).
 Revision 1.12  1997/01/30 01:58:25  dkegel
 1. Partially implemented player groups.
 2. Use anonymous unions (yech) to allow old programs to access
 elements of structures by the names used in the other library.
 Revision 1.11  1997/01/08 21:56:55  dkegel
 Player handles now long!
 Revision 1.10  1996/12/18 18:20:27  administrator
 Change to c style comments for benefit of gcc.
 Revision 1.9  1996/12/18 18:05:41  administrator
 1. Put PACK after every member of a structure that gets sent over the wire;
 see dp.h.
 2. Moved hMaster out of dp_session_t, as it changed sizes, and didn't
 really need to be sent; see dp.h.
 Revision 1.8  1996/10/10 23:12:00  dkegel
 Added support for dpPingUser().
 Revision 1.7  1996/10/10 18:45:38  dkegel
 Added offsets of more packet elements for benefit of game server.
 Revision 1.6  1996/09/25 22:48:16  dkegel
 1. Support the notion of unreachable hosts.  In particular, dpEnumSessions()
 will return dp_RES_HOST_NOT_RESPONDING if the game server is unreachable.
 2. Export dpSetGameServer(), and extend it so that one may clear the game
 server by passing it NULL.
 Revision 1.5  1996/09/13 01:16:00  dkegel
 Implemented PLOSSCTR define to dump packet loss summary at dpDestroy
 time.  Will get common.mif to obey PLOSSCTR= switch on cmdline.
 Revision 1.4  1996/09/08 23:05:31  dkegel
 Made dp_species_t unsigned, for the benefit of game server code running on
 unix machines.
 Revision 1.3  1996/08/26 21:31:05  dkegel
 dp->s.maxPlayers is now updated continuously.
 Revision 1.2  1996/07/23 22:08:37  dkegel
 Commented out #define SPARC because most programs compiled with this are on
 pc.
 Revision 1.1  1996/07/23 00:33:06  dkegel
 Initial revision
-------------------------------------------------------------------------*/
#ifndef _dppkt_h_
#define _dppkt_h_

/* All structures in this file should be packed to byte boundaries,
 * because the only structures in this file are those that are sent over
 * the wire or are written to files.
 *
 * Some compilers support #pragma pack, so use that.
 * Some compilers support only a member-by-member pack directive, so
 * place a PACK after every structure member.
 *
 * Pragma pack must come after last #include in file, and must be
 * undone at end.  Too bad we can't use #pragma pack(pop); Watcom only
 * supports #pragma pack(), which resets to the default.  For this to
 * work, one must always intend to be at the default packing at the beginning
 * and end of each .h file.  Maybe Watcom 10.6 will add pop support...
 */
#include "dppack1.h"

#ifndef dppkt_NO_DP

/*  Packet suffix.  Appended to end of every user data packet. */
typedef struct {
	dpid_t src PACK;
	dpid_t dest PACK;
	dp_karma_t sessKarma PACK;		/*  Hoo boy, we're paranoid now. */
} dp_envelope_t;

/* --------------- External packet types.  Never seen by user code. --------- */
/*  Packet Id's and packets.
 *  Naming scheme:
 *  Packets seen by user code are dp_PACKET_INITIALBYTE, DIGIT
 *  Packets defined here and not seen by user code are dp_PACKET_INITIALBYTE, UPPERCASE
 *  Packets defined in dppv.h and not seen by user code are dp_PACKET_INITIALBYTE, LOWERCASE
 */

/* ping packet definitions */
/* used to measure round trip transmission time */
#define dp_PING_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'B')
typedef struct {
	dp_karma_t	karma;	/* identifies a group of ping packets */
	unsigned short	pktnum;			/* starts at 0, increments for each packet */
	unsigned char	len;
	unsigned char	data[dpio_MAXLEN_UNRELIABLE-5];	/* 0..len-1 */
} PACK dp_ping_packet_t;
#define dp_PING_RESP_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'C')
typedef dp_ping_packet_t dp_ping_resp_packet_t;

/*  If a request to the host fails, the host may send one of these */
/*  to announce the failure.  This is only done for ADDPLAYER at the */
/*  moment. */
#define dp_ERROR_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'Z')
typedef struct {
	dp_packetType_t	request;	/*  The type of the packet that caused this error. */
	dp_result_t		err;		/*  Description of error. */
	dp_karma_t		karma;		/*  The karma of the object in question, if any. */
} PACK dp_error_packet_t;

/*  A description of a game in progress.  Sent by master periodically */
/*  or in response to an ENUMSESSIONS_PACKET. */
#define dp_SESSION_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'S')
typedef dp_session_t dp_session_packet_t;
#define sizeof_dp_session_packet_t 83		/*  Kludge: for portability */

/*  A request to describe games.  Sent by client if third parameter to */
/*  dpEnumSessions is not NULL. */
#define dp_ENUMSESSIONS_PACKET_ID	dppt_MAKE(dp_PACKET_INITIALBYTE,'E')
typedef struct {
	short sessionType;	/*  Please respond if you are a session of this type */
	unsigned char	adr[dp_MAX_ADR_LEN];	/*  Respond to this address */
} PACK dp_enumSessions_packet_t;

/*  A request to describe players.  Sent by client either as a result */
/*  of dpEnumPlayers with a non-NULL second parameter, or after a roster has */
/*  been received that has new players. */
#define dp_ENUMPLAYERS_PACKET_ID	dppt_MAKE(dp_PACKET_INITIALBYTE,'P')
typedef struct {
	short sessionType;	/*  Please respond if you are a session of this type */
	short karma;	/*  Please respond if you are still master of this session. */
	dpid_t start;	/*  I want players whose dpid is this or greater */
	unsigned char	adr[dp_MAX_ADR_LEN];	/*  Respond to this address */
} PACK dp_enumPlayers_packet_t;

/*  A request to add a player to the game.  Sent by client as a result of */
/*  dpOpen (which creates a single pseudoplayer) or dpCreatePlayer. */
/*  Kind of like a playerlist packet with only one player. */
#define dp_ADDPLAYER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'A')
typedef struct {
	dp_playerId_t newPlayer;
	dp_species_t sessionType;
	dp_karma_t sessionKarma;
	char		szPassword[dp_PASSWORDLEN];		/*  Not supported yet. */
} PACK dp_addPlayer_packet_t;

/*  A request to delete a player from the game.  Sent by client as a result */
/*  of dpDestroyPlayer. */
#define dp_DELPLAYER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'D')
typedef dp_playerId_t dp_delPlayer_packet_t;

/*  An empty packet to remind master that we're still here.  Sent by client. */
#define dp_KEEPALIVE_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'K')

/*  An empty packet to tell gamehost master to close our handle.  Kludge. */
#define dp_SERVER_LOGOUT_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'O')

/*  The player list packet has a variable number of players in it. */
/*  It is sent by master in response to a dp_ENUMPLAYERS_PACKET, or */
/*  after the master adds a player but before it sends out the roster. */
/*  It doesn't contain info about all players, just about the ones */
/*  with dpids equal to or greater than dp_ENUMPLAYERS_PACKET.start, */
/*  or about the new players. */
#define dp_PLAYERLIST_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'L')
typedef struct {
	dp_species_t	sessionType;
	dp_karma_t		karma;
	dpid_t			highest;		/*  Maximum dpid in use in game. */
	short			nPlayers;		/*  Number of players in packet, not game. */
	dp_playerId_t	players[1 /* n_players */ ];	/*  VARIABLE LENGTH ARRAY */
} PACK dp_playerList_packet_t;
#define sizeof_dp_playerList_packet_t(n_players) 				\
	(sizeof(dp_playerList_packet_t) +									\
	(n_players)*sizeof(dp_playerId_t)							\
	-sizeof(dp_playerId_t))		/* players[] */
/*  Assume that all transport layers allow packets of 250 bytes. */
#define dp_MAX_PLAYERLIST_N (\
	(dpio_MAXLEN_UNRELIABLE-sizeof_dp_playerList_packet_t(0)-sizeof(dp_packetType_t)) \
	/sizeof(dp_playerId_t))


/*
 * This packet gets is from a game server to a session host requesting
 * that a connection be opened to that specified address pair
 *
 * This packet should only be accepted by clients if it comes from a 
 * game server and should be discarded otherwise.
 *
 * The reason for this packets existence is allow people behind firewalls
 * to host sessions...
 *
 * The two addresses adr, adr2 are the two address of the player who
 * wishes to join the session.  They may be identical, in which case the
 * player has only one known addresses, or they may be different in which
 * case the player is multihomed or is behind a renumbering firewall.
 *
 */
#define dp_REQUEST_OPEN_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE, 'X')

typedef struct 
{
	unsigned char	adr[dp_MAX_ADR_LEN];
	unsigned char	adr2[dp_MAX_ADR_LEN];
} PACK dp_request_open_packet_t;


/*
 * This packets is sent from a player to a game server when that player
 * wishes to join a session.  This is done so that the game server can
 * then send the REQUEST_OPEN packet to the host so that if the host is
 * behind a Firewall then they will initiate the connection allowing 
 * communication to commence.
 *
 * Note: The size of sessionId should be [dptab_KEY_MAXLEN] but dependencies
 * make that impossible.  Regardless, the sessionId is currently guaranteed
 * never to exceed the length of an address + the length of a karma.
 *
 */
#define dp_INDIRECT_JOIN_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE, 'I')
#define dp_INDIRECT_JOIN_CHUNK_SESSIONID	1	/* chunk ID for session ID */
#define dp_INDIRECT_JOIN_CHUNK_SESSIONID_OPENONLY	2	/* same, but not joining, only opening connection, e.g. for remote enumplayers */

typedef struct
{
	unsigned char	chunk1;
	unsigned char	sessionId[dp_MAX_ADR_LEN + sizeof (dp_karma_t)];  
} PACK dp_indirect_join_packet_t;
#endif

/*-------- dpio packets ------------------------------------------*/

/*
 * A data packet with no data, and whose pktnum is the initial pktnum
 * for this stream (TCP/IP Illustrated, Volume 1, pg. 231)
 */
#define dpio_SYN_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'Y')


/*
 * A packet used to actively close a connection
 */
#define dpio_FIN_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'F')

#define dpio_DATA_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'T')
typedef struct {
	unsigned short	pktnum;		/* starts at 0, increments for each packet */
	unsigned char	len;			/* (dpio_MAXLEN_RELIABLE depends on size of these fields) */
	unsigned char	data[dpio_MAXLEN_RELIABLE];	/* 0..len-1 */
} PACK dpio_data_packet_t;

/* Acknowledgement of receipt of a packet. */
#define dpio_ACK_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'U')
typedef struct {
	unsigned short	pktnum;		/* starts at 0, increments for each packet */
#ifndef OLD_ACK
	char gotAllUpTo_offset;		/* 0 if all earlier packets received */
#endif
} PACK dpio_ack_packet_t;
#define sizeof_dpio_ack_packet_t (sizeof(unsigned short)+sizeof(char))

/* A data packet wrapped the way dp likes it. */
typedef struct {
	dp_packetType_t tag;
	dpio_data_packet_t body;
} PACK dpio_wrapped_data_packet_t;
#define sizeof_dpio_wdp(len) (sizeof(dp_packetType_t)+sizeof(dpio_data_packet_t)+len-dpio_MAXLEN_RELIABLE)

/* A bunch of unreliable packets, gathered together. */
#define dpio_GATHER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'G')
/* Format is len,data,len,data... where len is 1 byte. */

/*-------------------------------------------------------*/

/*  The minimum amount of detail needed to detect whether or not a player */
/*  is in a game. */
typedef struct {
	dpid_t		id;
	dp_karma_t	karma;
} PACK dp_playerIdShort_t;

#ifndef dppkt_NO_DP
/*----------------------------------------------------------------------
 Object used to represent a group of players.
 'members' is a variable-length array; use macro sizeof_dp_group_t()
 to get real size of structure, but sizeof(dp_group_t) will be big enough.
----------------------------------------------------------------------*/
typedef struct {
	dpid_t		id;				/* msgs to id reach all members */
	dp_karma_t	karma;				/* used to verify group identity */
	char		name[dp_PNAMELEN];	/* name of group */
	dp_karma_t	sessionKarma;		/* group only valid within this session */
	short		n;					/* number of members in group */
	dpid_t		members[dp_MAXREALPLAYERS];	/* dpid's of members */
} PACK dp_group_t;
#define sizeof_dp_group_t(n_members) 				\
	(sizeof(dpid_t)+sizeof(dp_karma_t)+sizeof(char)*dp_PNAMELEN \
	+sizeof(dp_karma_t) + sizeof(short) + sizeof(dpid_t)*n_members)

/* dp_groupId_t in anet.h is an abbreviated version of above.  */

/* Stuff from dpturn.h */
#define dpturn_ROUND_PKT_ID dppt_MAKE(dp_PACKET_INITIALBYTE,'o')

/* Stuff for dp2 */

/* Represents a user participating in a session; used only for
 * opening comm handles,
 * enumerating hosts when performing broadcasts,
 * and electing new session masters.
 * Has nothing to do with players.
 *
 * There are two network addresses that might apply:
 * 1) the source address of the join request as reported by dpio
 * 2) the sender's address included in the join request
 * We anticipate both being important later when we really deal
 * with firewalls.
 *
 * These are stored in a dptab table, with dpid as the key
 *
 * iadr is our estimate of the external address for the host.
 *
 * iadr and iadr2 are initially set to what the driver thinks its local address is.
 * iadr can be changed to the externally observed address if a game server
 * detects that the this is different from what the driver reported.
 * This can occur in both the Dual IP case (modem/lan) and the Firewall case.
 *
 */
typedef struct dp_host_s {
	unsigned char	iadr[dp_MAX_ADR_LEN];	/* primary interface/external address */
	dp_karma_t	joinKarma;				/* Changes each time host joins a session */
	dpid_t		firstId;				/* Host can use dpid's firstId..firstId+dp_PLAYERS_PER_HOST-1 */
	unsigned char	iadr2[dp_MAX_ADR_LEN];	/* secondary interface/internal address, NOT present in old clients */
} PACK dp_host_t;

/*
 * Request to join a particular session 
 *
 * The first karma identifies the act of joining this host to this session;
 * the second karma identifies the session.
 * If second karma is zero, the next three fields are parameters for
 * master to select a session for this host.
 *
 * The contents of iadr and iadr2 depend upon whether the session supports
 * firewalls (as indicated by the capabilities bit in the session information).
 *
 * When sending this packet, the iadr2 field is only sent if the session
 * supports firewalls.
 * When receiving this packet, the size of the packet is used to determine
 * which flavor of join we are receiving... (from OLD clients or NEW clients)
 *
 * iadr always contains the address we know about externally (from game server)
 * iadr2 contains the address the driver detects (local address)
 *
 * iadr and iadr2 will only be different when the machine is multi-homed or
 * behind a renumbering firewall.
 *
 */
#define dp_JOIN_PACKET_ID			dppt_MAKE(dp_PACKET_INITIALBYTE,'J')

/* Sizes of various versions of this packet */
#define dp_V1_JOIN_PACKET_LEN		2 * sizeof(dp_karma_t) + dp_MAX_ADR_LEN
#define dp_V2_JOIN_PACKET_LEN		2 * sizeof(dp_karma_t) + dp_MAX_ADR_LEN + \
 									sizeof (dp_species_t) + 2 * sizeof(short)
#define dp_V3_JOIN_PACKET_LEN		sizeof(dp_join_packet_t)

typedef struct {
	/* V1: Original Fields */
	dp_karma_t		joinKarma;
	dp_karma_t		sessionKarma;
	unsigned char	iadr[dp_MAX_ADR_LEN];

	/* V2: Following fields added to support letting server pick session. */
	dp_species_t	sessionType;
	short			flags;
	short			mask;

	/* V3: Following fields added to support renumbering firewalls. */
	unsigned char	iadr2[dp_MAX_ADR_LEN]; /* NOT present in old clients */
} PACK dp_join_packet_t;


/* Request to leave a particular session 
 * joinKarma and iadr are ignored. This is a rather silly packet.
 */
#define dp_LEAVE_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'Q')
#define dp_LEAVE_PACKET_LEN 	dp_V1_JOIN_PACKET_LEN
typedef dp_join_packet_t dp_leave_packet_t;

/* Info sent by host to client about the session it selected for it.
 */
#define dp_SELECTED_SESSION_PACKET_ID	dppt_MAKE(dp_PACKET_INITIALBYTE,'s')
typedef struct{
	dp_session_t sess;
	unsigned char keylen;
	char key[1];			/* key; dummy size. */
} PACK dp_select_sess_packet_t;
#define sizeof_dp_select_sess_packet_t(keylen) 				\
	(sizeof(dp_session_t)+sizeof(unsigned char)+keylen)

/* Our vote for who is to become master.  Packet defined in dp2.c.
 */
#define dp_VOTE_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'V')

/* Victory packet sent out by winner of election.  Packet defined in dp2.c.
 */
#define dp_VICTORY_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'W')

/* Request to log in to a server.  
 * Note: does not start with dp_PACKET_INITIALBYTE!  
 * The first version of this packet had no body, and caused the client
 * to automatically be subscribed to the global session table.
 * The second version has a body which specifies which sessionType to listen
 * to.
 * This lets the application subscribe to just the session table of interest.
 */
#define dp_LOGIN_PACKET_ID		dppt_MAKE('e','1')
typedef struct {
	dp_species_t sessionType;
} dp_login_packet_t;
#endif

/* Undo the previous pragma. */
#include "dpunpack.h"
#endif
