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

/*------------------------------------------------------------------------
 Copyright (C) 1996, 1997, 1998 Activision.

 Multiplayer game layer.
------------------------------------------------------------------------*/

#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/************ Constants, types, and packet definitions *******************/
#include "anet_types.h"

#ifndef dp_ANET1		/* Make dp_ANET2 be the default */
#ifndef dp_ANET2
#define dp_ANET2
#endif
#endif

#ifdef dp_ANET2
#define dp_MULTISESSTABLE
#endif

#ifndef dp_NO_TYPES
#ifndef anet_types_h
#define anet_types_h

/* Windows compatibility */
#ifndef PASCAL
#define dp_PASCAL
#else
#define dp_PASCAL PASCAL
#endif
#ifndef FAR
#define dp_FAR
#else
#define dp_FAR FAR		/* Only for declaring function POINTERS. */
#endif
/* DP_API goes before the type of exported functions */
#ifndef DP_API
  #if defined(_WIN32) && defined(DP_DLL)
    #define DP_API __declspec( dllimport )
  #else
    #define DP_API
  #endif
#endif
/* DP_APIX goes between the type and the function name of exported functions */
#ifndef DP_APIX
  #if defined(_WIN32)
    #define DP_APIX cdecl
  #else
    #define DP_APIX
  #endif
#endif

/*--------------------------------------------------------------------------
 Byte-swap primitive datatypes between Little-Endian and Big-Endian.

 Big-Endian and Little-Endian are wry references to Gulliver's Travels,
 and are used in the computer industry to indicate which order the bytes
 of a multibyte integer are stored in memory.  Intel stores the low order
 byte at the low address, and is called Little-Endian.
 Motorola's PowerPC and Sun's Sparc store the low order byte at the high
 address, and are called Big-Endian.

 Use SwapBytes2(v) when reading or writing a 16 bit integer that will
 be sent over the network.  It will adjust the byte order of the integer
 to match standard Intel byte order, even if you're running on a Macintosh
 or a Sparc.  Likewise, use SwapBytes4 when reading or writing longwords.
--------------------------------------------------------------------------*/

/* When porting to a new big-endian platform, add whatever predefined
 * symbol the platform defines to the following list, so the
 * real byte-swapping code gets turned on.
 */
#if defined(SPARC) || defined(__POWERPC__)
#define SwapBytes2(v)		((unsigned short) ((v) << 8) \
								| (unsigned char) ((v) >> 8))
#define SwapBytes4(v)		((unsigned long) (SwapBytes2(v) << 16) \
								| SwapBytes2((v) >> 16))
#define dp_MUSTSWAP
#else
#define SwapBytes2(v)		(v)
#define SwapBytes4(v)		(v)
#undef dp_MUSTSWAP
#endif

/* Construct portable multibyte representation of short integers */
#define dpGETSHORT_FIRSTBYTE(s) (unsigned char)((s) & 0xff)
#define dpGETSHORT_SECONDBYTE(s) (unsigned char)(((s) >> 8) & 0xff)

/* Construct portable multibyte representation of long integers */
#define dpGETLONG_FIRSTBYTE(s)  (unsigned char)((s) & 0xff)
#define dpGETLONG_SECONDBYTE(s) (unsigned char)(((s) >> 8) & 0xff)
#define dpGETLONG_THIRDBYTE(s)  (unsigned char)(((s) >> 16) & 0xff)
#define dpGETLONG_FOURTHBYTE(s) (unsigned char)(((s) >> 24) & 0xff)

/* Retrieve short integers from portable multibyte representation */
#define dpMAKESHORT(first,second) ((unsigned char)(first) |\
								 (((unsigned short)((unsigned char)(second)))<<8))

/* Retrieve long integers from portable multibyte representation */
#define dpMAKELONG(first,second,third,fourth) (   \
		(unsigned long)dpMAKESHORT(first,second) |\
		((unsigned long)dpMAKESHORT(third, fourth)) << 16)

/* Structure members that need to be byte-aligned have PACK after them
 * for benefit of gcc.  Makefile will define PACK in that case.
 */
#ifndef PACK
#define PACK
#endif

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
 * and end of each .h file.  Maybe Watcom 11 will add pop support...
 */
/*include "dppack1.h"*/
#if defined(__WATCOMC__)    /* Watcom C */
#pragma pack(1)
#elif defined(__MWERKS__)   /* Codewarrior */
#pragma options align=packed
#elif defined(_M_IX86)      /* Visual C */
#pragma pack(push, 1)
#endif

/*  Things that should be variable, but are fixed for now. */
#define dp_BROADCAST_THRESHOLD	18	/*  use broadcast if threshold # of players */
#define dp_MAXSESSIONS	32			/*  kludge.  should be dynamic. */
#define dp_PLAYER_TIMEOUT_SECS 41	/*  How long before we decide a machine is dead. */
#define dp_MAXREALPLAYERS	30		/* to match old definition on server */

#define sizeof_dp_envelope_t 6

typedef dp_char_t char_t;	/* for the moment - will go away */

#define dp_ID_NAMESERVER	0
#define dp_ID_BROADCAST		0
#define dp_ID_LOCAL_NAMESERVER	1
#define dp_ID_FIRST_FREE	2

#include "aneterr.h"		/* error codes and dp_result_t */

/* Size limits. */
/* Limits for dpSend. */
#define dp_MAXLEN_UNRELIABLE (dpio_MAXLEN_UNRELIABLE - sizeof_dp_envelope_t)
#define dp_MAXLEN_RELIABLE   (dp_MAXLEN_UNRELIABLE - sizeof_dpio_reliable_header)


/*  High bit of karma is whether it's a pseudoplayer. */
#define dp_KARMA_MAKE(pseudo,k) (((pseudo) ? 0x8000:0) | ((k)&0x7fff))
#define dp_KARMA_IS_PSEUDO(karma) (((karma) & 0x8000) != 0)


#define dp_SNAMELEN 32
#define dp_USERFIELDLEN 10
#define dp_PASSWORDLEN 10
#define dp_MAX_ADR_LEN 10	/*  Size of ipx's fullNetworkAddr_t. */


/*  Values for dp_session_t->flags.  Combine with bitwise OR. */
#define dp_SESSION_FLAGS_OPENSESSION		0x00
#define dp_SESSION_FLAGS_CREATESESSION		0x01
#define dp_SESSION_FLAGS_USE_BROADCAST		0x02
#define dp_SESSION_FLAGS_ENABLE_NEWPLAYERS	0x04	/*  ignored by dpOpen */
#define dp_SESSION_FLAGS_ISSERVER			0x08	/*  (can only be set internally) == dp_t.isServerLobby */
#define dp_SESSION_FLAGS_ISLOBBY			0x10
#define dp_SESSION_FLAGS_MIGRATE_FORBIDDEN	0x20
#define dp_SESSION_FLAGS_ENABLE_PLAYERVARS	0x40
#define dp_SESSION_FLAGS_ENABLE_DUALADDRESS	0x80	/* internal use ONLY */

#if defined(__MWERKS__)  /* || defined(__GCC__) */
#define __NO_ANONYMOUS_UNIONS__
#endif

/* Use anonymous unions (a C++ feature!) to allow old programs to access
 * new structures by other library's member names.
 * (Ifdef because standard C doesn't allow anonymous unions.)
 */
#ifndef __NO_ANONYMOUS_UNIONS__
#define DP_ALIAS(type,name1,name2) union { type name1 PACK; type name2 PACK; }
#define DP_ALIAS3(type,name1,name2,name3) union { type name1 PACK; type name2 PACK; type name3 PACK; }
#else
#define DP_ALIAS(type,name1,name2) type name1
#define DP_ALIAS3(type,name1,name2,name3) type name1
#endif

typedef struct {

	DP_ALIAS(char, dummy, dwSize);	/* used to be hMaster; now in dp_t */
	dp_uint8_t adrMaster[dp_MAX_ADR_LEN];	/*  Address needed to establish comm layer connection. */
	dpid_t		masterPseudoplayer;
	/*  Only allow connections to sessions with the same sessionType. */
	DP_ALIAS3(dp_species_t, sessionType, guidSession, guidApplication);
	/*  Random number chosen by library when creating a session. */
	DP_ALIAS(dp_karma_t, karma, dwSession);
	/* short	maxPlayers;*/
	DP_ALIAS(dp_int16_t,	maxPlayers, dwMaxPlayers);
	/* short	currentPlayers; */
	DP_ALIAS(dp_int16_t,	currentPlayers, dwCurrentPlayers);
	DP_ALIAS(dp_int16_t,	flags, dwFlags);
	/* char_t		sessionName[dp_SNAMELEN]; */
	DP_ALIAS3(dp_char_t, sessionName[dp_SNAMELEN], szSessionName[dp_SNAMELEN], lpszSessionNameA[dp_SNAMELEN]);
	dp_char_t	szUserField[dp_USERFIELDLEN];	/*  Availible for user data. */
	dp_char_t	szPassword[dp_PASSWORDLEN];		/*  Not supported yet. */
	dp_int32_t	dwReserved1;
	dp_int32_t	dwUser1;						/*  Availible for user data. */
#ifdef dp_ANET2
	dp_uchar_t	reserved2[dp_MAX_ADR_LEN+2];
#endif
} PACK dp_session_t;	/* 83 bytes + 2 byte header */

/* dp_session_t are used both internally and as packets on the wire. */

/*  Everything you need to know about a player to talk to him or decide if */
/*  he is you. */
#define dp_PNAMELEN 24

#ifdef dp_ANET2
#define dp_MAX_PLAYERBLOB_LEN 16
#endif

typedef struct {
	DP_ALIAS(dpid_t, id, dpId);
	dp_karma_t	karma;
	dp_uint8_t	adr[dp_MAX_ADR_LEN];
	char		name[dp_PNAMELEN];
#ifdef dp_ANET2
	dp_uint8_t bloblen;
	dp_uint8_t blob[dp_MAX_PLAYERBLOB_LEN];	/* game-specific info */
#endif
} PACK dp_playerId_t;

/* Structure to describe version info. */
typedef struct {
	dp_uint16_t major;
	dp_uint16_t minor;
} PACK dp_version_t;

/* Structure to describe an installed game.
   Get them from dpEnumApp() and use in demo/utils/launchapp.c's launchapp().
   This is for building shells that can launch any of the games installed
   on a machine.
*/
#define dp_PLAT_UNKNOWN	0
#define dp_LANG_UNKNOWN	0
#define dp_VERS_UNKNOWN	0xFF
typedef struct {
	char *name;					/* Human readable name */
	char *path;					/* Full path to game executable */
	char *args;					/* Command arguments needed to launch game */
	char *cwd;					/* Directory to be in when launching game */
	char *shellOpts;			/* Options for netshell */
	dp_species_t sessionType;	/* Session type of the game */
	dp_uint16_t platform;	/* OS, accelerator cards, etc. */
	unsigned char language;		/* e.g, English, French, Hindi, etc. */
	dp_version_t current;		/* installed version # */
	dp_version_t latest;		/* latest published version # */
} dp_appParam_t;

/* copied from aeh.h; to be used by exception handling code to detect if
   exception was thrown by dpReportAssertionFailure() */
#ifndef aeh_h
#define aeh_ASSERTION_CODE   0xE0000002 /* aeh defined exception code for case where assertion failure occurred */
#endif

/************************ System Messages ********************************/

/*  Build a packet id out of two ascii chars. */
#define dppt_MAKE(a,b) (dp_packetType_t) SwapBytes2(((a)&255) | ((b)<<8))

#define dp_PACKET_INITIALBYTE 'd'

/*  Notification that a player has joined the currently open game. */
/*  Returned by dpReceive whenever it notices a new player. */
/*  This message is not handled by dp at all; it is simply a courtesy to the */
/*  user program so it doesn't have to call dpEnumPlayers all the time. */
#define dp_USER_ADDPLAYER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'1')
typedef dp_playerId_t dp_user_addPlayer_packet_t;

/*  Notification that a player has left the currently open game. */
/*  Returned by dpReceive whenever it notices that a player has vanished. */
/*  This message is not handled by dp at all; it is simply a courtesy to the */
/*  user program so it doesn't have to call dpEnumPlayers all the time. */
#define dp_USER_DELPLAYER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'2')
typedef dp_playerId_t dp_user_delPlayer_packet_t;

/*  Notification that a group has been created. */
/*  Returned by dpReceive whenever it notices a new group. */
#define dp_USER_ADDGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'3')
typedef struct dp_groupId_s dp_user_addGroup_packet_t;

/*  Notification that a group has been deleted. */
/*  Returned by dpReceive whenever it notices a group disappear. */
#define dp_USER_DELGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'4')
typedef struct dp_groupId_s dp_user_delGroup_packet_t;

/*  Notification that a player has joined a group. */
/*  Returned by dpReceive whenever it notices a new player in a group. */
#define dp_USER_ADDPLAYERTOGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'5')
typedef struct {
	dpid_t		dpIdGroup;
	dpid_t		dpIdPlayer;
	dp_karma_t	sessionKarma;		/* group only valid within this session */
} PACK dp_addPlayerToGroup_packet_t;

/*  Notification that a player has left a group. */
/*  Returned by dpReceive whenever it notices that a player has vanished. */
#define dp_USER_DELPLAYERFROMGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'6')
typedef dp_addPlayerToGroup_packet_t dp_delPlayerFromGroup_packet_t;

/*  Notification that the local machine is now the session host. */
#define dp_USER_HOST_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'7')

typedef struct dp_groupId_s {
	dpid_t		id;			/* msgs to id reach all members */
	dp_karma_t	karma;			/* used to verify group identity */
	dp_char_t	name[dp_PNAMELEN]; 	/* name of group */
	dp_karma_t	sessionKarma;		/* group only valid within this session */
} PACK dp_groupId_t;

/*-------------------------------------------------------------------------
 Packet carrying a variable change.
-------------------------------------------------------------------------*/
#define dp_USER_PLAYERDATA_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'8')
typedef struct {
	size_t len;			/* let them be huge */
	dpid_t id;
	dp_uint16_t key;
	void *data;			/* only sent on local machine, ptrs ok */
} PACK dp_user_playerData_packet_t;

/*-------------------------------------------------------------------------
 Packet informing of the loss of a session.
-------------------------------------------------------------------------*/
#define dp_SESSIONLOST_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'9')
typedef dp_session_t dp_sessionLost_packet_t;

/*-------------------------------------------------------------------------
 Packet containing the result of a dpOpen attempt.  Local only.
-------------------------------------------------------------------------*/
#define dp_SESSIONRESULT_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'0')
typedef struct {
	dp_result_t reason;
	dp_session_t sess;
} PACK dp_sessionResult_packet_t;

/****************** Low-level comm definitions ***************************/

#ifndef _CommAPI_h_

#define comm_DRIVER_SIGLEN			8   /* must be multiple of four. */
#define comm_DRIVER_NAMELEN			32

typedef struct {
	char signature[comm_DRIVER_SIGLEN];
	size_t recordLen;				/* sizeof(comm_driverInfo_t) */
	char name[comm_DRIVER_NAMELEN];/* Name to present to user */
	dp_int16_t version;					/* Major, minor rev. in high, low byte */
	dp_int16_t capabilities;			/* What driver can do/wants to do */
	dp_int16_t needs;					/* What fields in commInitReq_t to fill in */
} PACK comm_driverInfo_t;

/* Special value for commInitReq_t.portnum */
#define comm_PORT_ANY	(0)			/* let NOS pick a port number */

/* Bits of comm_driverInfo_t.capabilities */
#define comm_DRIVER_IS_VISIBLE		1		/* Whether to show driver to user */
#define comm_DRIVER_KNOWS_PLAYERLST	2		/* Whether to call dpCommThaw */
#define comm_DRIVER_ALLOWS_GAMELIST	4		/* Whether to call dpEnumSessions */
#define comm_DRIVER_IS_FAST			8		/* Whether to set update rate down */
#define comm_DRIVER_PREFERS_BROADCAST	16	/* Whether to use broadcast for frequent data */
#define comm_DRIVER_NO_BROADCAST	32		/* Whether to avoid broadcast entirely */

/* Bits of comm_driverInfo_t.needs */
#define comm_INIT_NEEDS_SESSIONID	2
#define comm_INIT_NEEDS_PORTNUM		4
#define comm_INIT_NEEDS_BAUD		8
#define comm_INIT_NEEDS_BASEADR		0x10
#define comm_INIT_NEEDS_HWIRQ		0x20
#define comm_INIT_NEEDS_SWINT		0x40
#define comm_INIT_NEEDS_PHONENUM	0x80
#define comm_INIT_NEEDS_MODEMINISTR	0x100

/* Bits of commInitReq_t.flags */
#define comm_INIT_FLAGS_RESUME		1		/* modem connection already estab. */
#define comm_INIT_FLAGS_TEST		2		/* perform loopback test on init if not resuming */

/* Bits of commInitReq_t.dialing_method */
#define comm_INIT_DIALING_METHOD_PULSE 0
#define comm_INIT_DIALING_METHOD_TONE  1

typedef struct {			/* Request (filled in by caller) */
	size_t reqLen;			/* Sizeof(commInitReq_t) */
	long sessionId;		/* Random number chosen at initial startup */
	long portnum;
	long baud;
	long baseadr;          /* ignored by Windows */
	long hwirq;            /* ignored by Windows */
	long swint;            /* ignored by Windows */
	char *phonenum;
	char *modeministr;
	long flags;			/* controls whether to dial and/or test */
	long dialing_method;	/* parameter to HMSetDialingMethod */
} PACK commInitReq_t;

typedef struct {
	int portnum;			/* Value for commInitReq->portnum (e.g. 0) */
	char name[64];			/* Name to present to user (e.g. COM1) */
} commPortName_t;

#endif

/* A reference to a file.  May be written to disk and still remain valid,
 * as long as the volume containing the file is not unmounted and remounted.
 * If you happen to need to create one from scratch, you should memset()
 * the whole record to zero before filling in the field(s) of interest.
 */
typedef struct {
#if defined(__MWERKS__)   /* Codewarrior */
	char fname[120];
	long vRefNum;
	long dirID;
#else
	char fname[128];
#endif
} dp_transport_t;

#ifdef dp_ANET2
/* Structure to hold info about a server.  To be used by a future
 * dpEnumServersEx(), and by the Java interface.
 */
typedef struct dp_serverInfo_s {
	dp_int16_t len;						/* length of this structure */
	dp_int16_t rtt_ms_avg;				/* Average round trip time, millisec */
	dp_int16_t loss_percent;				/* Average packet loss */
	dp_int16_t cur_users;				/* people currently connected */
	dp_int16_t max_users;				/* max # allowed to connect */

	char hostname[64];				/* ASCII server name (often hostname) */
	dp_species_t sessType;			/* session type given in dpEnumServersEx()*/
	dp_int16_t cur_sessTypeUsers;		/* people currently in sessType sessions */
	dp_int16_t cur_games;				/* games currently running */
	dp_int16_t cur_sessTypeGames;		/* games of sessType currently running */
	char reserved[16];				/* for internal use */
} dp_serverInfo_t;

#define dp_USER_NAME_LEN 16
#define dp_USER_DESCRIPTION_LEN 256
#define dp_USER_URL_LEN 64

/* A record describing a user (users are more persistant than players) 
 * Strings of wchar_t are Unicode.  These strings are always nul-terminated.
 */
typedef struct {
	dp_uid_t uid;
	wchar_t uname[dp_USER_NAME_LEN];
	wchar_t url[dp_USER_URL_LEN];
	wchar_t description[dp_USER_DESCRIPTION_LEN];
} PACK dp_userInfo_t;

/* A record describing a user's cumulative scores in one kind of game.  
 * Can't be completely declared in C; some manual unpacking required.
 */
typedef struct {
	dp_int16_t nScoreTypes;
	short scoreIds[1 /* nScoreTypes */];	/* variable length */
	long scores[1 /* nScoreTypes */ ];		/* variable length */
} PACK dp_scoreInfo_t;

/* The kinds of objects that can be monitored with dpRequestObjectDeltas(). */
typedef union {
	dp_playerId_t   p;
	dp_session_t    sess;
	dp_serverInfo_t serv;
	dp_userInfo_t	user;
	dp_scoreInfo_t  score;
} dp_object_t;

#define dp_KEY_SESSIONS			1	/* known sessions */
#define dp_KEY_PLAYERS			3
#define dp_KEY_SERVERPINGS		10	/* servers: ip adr, name, ping results */
#define dp_KEY_SCORES			14	/* user score records */
#define dp_KEY_USERS			30	/* user records */
#define dp_KEY_PLAYER_LATENCIES 31  /* Only used by dpRequestObjectDeltas */

/*-------------------------------------------------------------------------
 Packet carrying object change info.
-------------------------------------------------------------------------*/
#define dp_OBJECTDELTA_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'+')
#define dp_KEY_MAXLEN 12    /* Keys must be 12 bytes in length or less. */
#define dp_OBJECTDELTA_FLAG_LOCAL 1	/* set if object created by this machine */
#define dp_OBJECTDELTA_FLAG_INOPENSESS 2	/* player in session hosted or joined by this machine */
#define dp_OBJECTDELTA_FLAG_ISHOST 4	/* player is master of session */
typedef struct {
	short pktloss;		/* player deltas include a loss in percent */
	short latency;     /* player deltas include a latency in ms */
	
	long flags;		/* one or more of dp_OBJECTDELTA_FLAG_* */

	/* If an object is being created, status = dp_RES_CREATED.
	 * If an object is being changed, status = dp_RES_CHANGED.
	 * If an object is being deleted, status = dp_RES_DELETED.
	 */
	dp_result_t status;

	/* For players,  key is {dp_KEY_PLAYERS, reserved bytes... }
	 * For sessions, key is {dp_KEY_SESSIONS, reserved bytes...}
	 * For servers,  key is {dp_KEY_SERVERPINGS, reserved bytes...}
	 */
	short keylen;
	char key[dp_KEY_MAXLEN];/* long key of context for following data */

	/* Subkey is reserved. */
	short subkeylen;
	char subkey[dp_KEY_MAXLEN];

	/* For players,  data is dp_playerId_t
	 * For sessions, data is dp_session_t
	 * For servers,  data is dp_serverInfo_t
	 * If status == dp_RES_CHANGED, data is after change.
	 */
	dp_object_t data;
	/* nothing may go here; data is variable length */
} PACK dp_objectDelta_packet_t;

/*-------------------------------------------------------------------------
 Packet sent by system to indicate success or failure of attempt to log
 in to the server.

 Reason is dp_RES_OK      if login was successful and account is activated.
 Reason is dp_RES_NOTYET  if login successful, but account not yet activated,
                          in which case dpAccountActivate() needs to be called

 Reason is dp_RES_ACCESS  if login unsuccessful, bad username or password
 Reason is dp_RES_ALREADY if login unsuccessful, username already logged in
-------------------------------------------------------------------------*/
#define dp_ACCOUNT_PACKET_ID dppt_MAKE(dp_PACKET_INITIALBYTE,'-')
typedef struct {
	dp_uid_t	uid;		/* dp_UID_NONE unless reason is dp_RES_OK */
	dp_result_t 	reason;	
	dp_uint32_t	reserved;	/* for future use */
} dp_account_packet_t;
#endif

/*************************************************************************/

/*------------------------------------------------------------------------
 Callback used to list servers and the round trip delay to them.
------------------------------------------------------------------------*/
typedef int (dp_FAR dp_PASCAL *dpEnumServersCallback_t) (const char *hostname, long roundtrip_ms,long *pTimeout,long flags,void *context);

/* Same thing, but returns dp_serverInfo_t, too */
typedef int (dp_FAR dp_PASCAL *dpEnumServersExCallback_t)(const char *hostname, long roundtrip_ms,dp_serverInfo_t *server,long *pTimeout,long flags,
void *context);

/*------------------------------------------------------------------------
 Callback used for measuring round trip delay.
------------------------------------------------------------------------*/
typedef void (dp_FAR dp_PASCAL *dp_ping_callback_t)(dp_karma_t karma, long avg_ms, int loss_pct);

/*------------------------------------------------------------------------
 Function type called for each session enumerated or created.
------------------------------------------------------------------------*/
typedef int (dp_FAR dp_PASCAL *dpEnumSessionsCallback_t)(dp_session_t *sDesc,long *pTimeout,long flags,void *context);

/*----------------------------------------------------------------------
 Function type called when a connection is opened or closed.
----------------------------------------------------------------------*/
typedef void (dp_FAR dp_PASCAL *dpOpenConnCallback_t)(void *addr, int len,
	int n_conns, dp_result_t state, char *user, char *pass, void *context);

/*------------------------------------------------------------------------
 Function type called for each player enumerated or created.
 The flags argument is used to indicate whether the player is on this
 machine or not.
------------------------------------------------------------------------*/
#define dp_EPC_FLAGS_LOCAL 1
#define dp_EPC_FLAGS_REMOTE 2
typedef void (dp_FAR dp_PASCAL *dpEnumPlayersCallback_t)(dpid_t id, dp_char_t *name, long flags, void *context);

/* Same thing, but includes blob info, too */
typedef void (dp_FAR dp_PASCAL *dpEnumPlayersExCallback_t)(dpid_t id, dp_char_t *name, long flags, void *context, dp_playerId_t *player);

/*----------------------------------------------------------------------
 Function type called to announce near-fatal problems.
----------------------------------------------------------------------*/
typedef void (dp_FAR dp_PASCAL *dpExceptionCallback_t)(void *context, dp_result_t err, char *msg);

/*----------------------------------------------------------------------
 Function type called for each transport enumerated.
 Note: filenames are volatile, and must be copied into a local buffer!
----------------------------------------------------------------------*/
typedef void (dp_FAR dp_PASCAL *dpEnumTransportCallback_t)( const dp_transport_t *fname, const comm_driverInfo_t *description, void *context);

/*----------------------------------------------------------------------
 Function type called for each application enumerated by dpEnumApp().
----------------------------------------------------------------------*/
typedef void (dp_FAR dp_PASCAL *dpEnumAppCallback_t)(dp_appParam_t *appParam, void *context);

/*-------------------------------------------------------------------------
 callback function type called periodically by dpCommThaw.
 Argument is how many connection steps have been completed so far.
 (this starts at 0 and slowly increases to somewhere between 2 and 10.)
 Return value should be 0 to abort, non-zero to continue.
-------------------------------------------------------------------------*/
typedef int (dp_FAR dp_PASCAL *dpCommThawCallback_t)(int status, void *context);

/* For dpGetCaps() */

#define dp_CAPS_FLAGS_ISHOST 1	/* set if calling program is the host */
typedef struct {
    long dwSize;
    long dwFlags;
    long dwHeaderLength;
    long dwHundredBaud;
    long dwLatency;
    long dwMaxBufferSize;
    long dwMaxLocalPlayers;
    long dwMaxPlayers;
    long dwMaxQueueSize;
    long dwPktLoss;
} dp_caps_t;

#ifdef dp_ANET2
#define dp_STATS
/*-------------------------------------------------------------------------
 Structure to hold network statistics.
 You can hang one of these at any buffer where objects might
 loiter while being processed.
 For instance, you could outfit a water tank with one of these.
 In that case, in = liters of water piped into the tank since time 0,
 out = liters of water delivered throught output pipe of tank since time 0,
 dropped = liters of water spilled when tank overflows since time 0,
 waiting = liters of water in tank at the moment,
 sum_waiting = sum of measurements of number of liters of water in tank,
 num_waiting = number of measurements in sum_waiting.

 To use the values in this structure to find the average statistics
 covering a period of time, make a copy of structure at the beginning of
 the period of time, and another at the end of the period of time.
 Subtract the two.
 To get the average number of units sent per second, divide out by the
 length of the period of time.  Same for average number of units queued
 or dropped per second.
 To get the average number of units waiting, divide sum_waiting by num_waiting.
-------------------------------------------------------------------------*/
typedef struct {
	long in,
		 out,
		 dropped,
		 waiting,
		 sum_waiting,
		 num_waiting;
} dp_stat_t;
/* How to tell dpGetStats() which statistics to access. */
#define dp_STAT_DPIO_RX_REL_BYTES   0
#define dp_STAT_DPIO_RX_REL_PKTS    1
#define dp_STAT_DPIO_RX_UNREL_BYTES 2
#define dp_STAT_DPIO_RX_UNREL_PKTS  3
#define dp_STAT_DPIO_TX_REL_BYTES   4
#define dp_STAT_DPIO_TX_REL_PKTS    5
#define dp_STAT_DPIO_TX_UNREL_BYTES 6
#define dp_STAT_DPIO_TX_UNREL_PKTS  7
#define dp_STAT_DPIO_CONNECTING     8
#define dp_STAT_DPIO_CONNECTED      9
#define dp_STAT_DPTAB_RX_BYTES     10
#define dp_STAT_DPTAB_RX_RECORDS   11
#define dp_STAT_DPTAB_TX_BYTES     12
#define dp_STAT_DPTAB_TX_RECORDS   13
/* Highest value accepted by dpGetStats() */
#define dp_STAT_MAX  13
#endif

/* Structure with information about a process started by dpLaunchApp */
typedef struct {
#ifdef _WIN32
	HANDLE hProcess;
#else
	char dummy;
#endif
} dpLaunchApp_result_t;

/* Undo the previous pragma. */
/*include "dpunpack.h"*/
#if defined(__WATCOMC__)    /* Watcom C */
#pragma pack()
#elif defined(__MWERKS__)   /* Codewarrior */
#pragma options align=reset
#elif defined(_M_IX86)      /* Visual C */
#pragma pack(pop)
#endif

#endif
#endif

/***************************** Functions *********************************/

#if defined(__MWERKS__)
#pragma export on
#endif

#ifndef dp_NO_FUNCTIONS
#ifndef anet_functions_h
#define anet_functions_h

#ifndef dp_t_defined
#define dp_t_defined
typedef struct dp_s {
	char opaque[4096];
} dp_t;
#endif

/*************************** Initialization ******************************/

/*----------------------------------------------------------------------
 Calls the callback function (cb) once with a description,
 including the server's hostname and delay,
 of each game server that this object could connect to.
 Servers may be reported multiple times.
 Finishes by calling the callback with a null server pointer.

 Since our functions must be non-blocking, returns immediately,
 before any calls to the callback. The callback is called by dpReceive,
 which must be called frequently.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumServers(
	dp_t *dp,
	long timeout,				/* How long in milliseconds to wait. */
	dpEnumServersCallback_t cb,
	void *context);

/*----------------------------------------------------------------------
 Similar to dpEnumServers() but returns more server information during callback.

 dpEnumServersEx() takes an additional parameter for session type and returns
 additional server information in a dp_serverInfo_t structure during callback.
 This info includes latency, packet loss, number of users of given session
 type, total number of users, and maximum allowed users.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumServersEx(
	dp_t *dp,
	long timeout,				/* How long in milliseconds to wait. */
	dp_species_t sessType,
	dpEnumServersExCallback_t cb,
	void *context);

/*------------------------------------------------------------------------
 If your program changes the speed of the system clock, pass in the new
 value of the ANSI C constant CLOCKS_PER_SEC here.
 This is used to along with the clock() function to set internal timeouts.
 This is not needed under Windows 95.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetClocksPerSec(
	dp_t *dp,
	int cps);

/*----------------------------------------------------------------------
 Sets a callback to be called for opening and closing of connections.
 This is done via a callback translation of the dpio_setIncomingCallback
 mechanism, and passing the callback translation to dpio_openHdl.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetConnCallback(
	dp_t *dp, dpOpenConnCallback_t cb, void *context);

#define dpSetActiveThread _daoot
/*----------------------------------------------------------------------
 Sets which thread is allowed to make dp calls.
 Avoid using this call.  It's dangerous.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetActiveThread(dp_t *dp);

/*------------------------------------------------------------------------
 Create a dp_t.
 This is the very first step in using the dp layer.
 Loads and initializes the given comm driver.
 If thawFilename is NULL, use the params argument to initialize communications.
 (Consult the comm_driverInfo_t.needs field returned by dpEnumTransports
 to determine which fields of params must be filled in.)

 If thawFilename is not NULL, restore from disk the session previously saved
 by dpFreeze or anetdrop.exe, and ignore the params argument.  Note: this
 does not restore any callbacks or enumerations that were in progress
 when dpFreeze was called.  Also, some transports (e.g. modem and Heat)
 do not allow restoring frozen sessions.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCreate(
	dp_t **pdp,
	dp_transport_t *transport,
	commInitReq_t *params,
	char *thawFilename);

/*------------------------------------------------------------------------
 Destroy a dp_t.
 Shuts down and unloads the comm driver (!).
 If the flags argument is non-zero, does not hang up the phone line (if
 any).
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpDestroy(
	dp_t *dp,
	int flags);

/*--------------------------------------------------------------------------
 Convert an ASCII hostname into a binary address.
--------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpResolveHostname(
	dp_t *dp,
	char *hostname,
	char adrbuf[dp_MAX_ADR_LEN]);

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
	dp_species_t sessionType);

/*--------------------------------------------------------------------------
 Retrieve the current game server name and default session type.
 Returns dp_RES_FULL if buffer too small,
 dp_RES_NOTYET if no server has been set.
--------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetGameServerEx(
	dp_t *dp,
	char *masterHostNameBuf,
	size_t masterHostNameBufLen,
	dp_species_t *psessionType);

/*------------------------------------------------------------------------
 Same as dpSetGameServerEx, but gets sessionType by calling dpReadAnetInf().
 Obsolete.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetGameServer(
	dp_t *dp,
	char *masterHostName);	/* server's name, or NULL to clear */

/*------------------------------------------------------------------------
 Retrieve the current session description.
 *pbuflen must be filled with the size of the buffer before calling;
 it will be set to sizeof(dp_session_t).
 If pbuflen is NULL, it is assumed that the buffer is the right size.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetSessionDesc(
	dp_t *dp,
	dp_session_t *buf,
	size_t *pbuflen);

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
	long flags);

/*------------------------------------------------------------------------
 Retrieve the capabilities of the currently loaded transport,
 plus some info about the current session, especially whether this machine
 is the host of the current session.
 Flags is currently unused.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetCaps(
	dp_t *dp,
	dp_caps_t *info,
	long flags);

/*----------------------------------------------------------------------
 Retrieve the capabilities of the currently loaded transport,
 plus some info about the given player, especially the current latency and
 packet loss to that player.
 If flags is dp_SEND_RELIABLE, retrieves info about reliable packet services
 instead of unreliable services.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetPlayerCaps(
	dp_t *dp,
	dpid_t id,
	dp_caps_t *info,
	long flags);

/*************************** Security ******************************/

#define dpAccountLoginW _dalw
#define dpAccountLoginA _dala

/*------------------------------------------------------------------------
 After using dpSetGameServerEx(),
 call this function to log in to an existing account on the server.
 dpReceive() will later return a packet of type dp_ACCOUNT_PACKET_ID
 to indicate success or failure, or to tell you to call dpAccountActivate().

 'username' is the user name specified by the user in dpAccountCreate().
 'password' is the password set by the user in dpAccountCreate().
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpAccountLoginW(
	dp_t *dp,
	const wchar_t *username,
	const wchar_t *password);

DP_API dp_result_t DP_APIX dpAccountLoginA(
	dp_t *dp,
	const char *username,
	const char *password);

#ifdef UNICODE
#define dpAccountLogin dpAccountLoginW
#else
#define dpAccountLogin dpAccountLoginA
#endif

#define dpAccountActivateW _daaw
#define dpAccountActivateA _daaa
/*------------------------------------------------------------------------
 Call this function after receiving a packet telling you to activate
 your account in response to dpAccountLogin() or dpAccountCreate()
 to activate your account.

 'secretcode' is the secret code emailed to you by the server when the
              account was created.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpAccountActivateW(
	dp_t *dp,
	const wchar_t *secretcode);

DP_API dp_result_t DP_APIX dpAccountActivateA(
	dp_t *dp,
	const char *secretcode);

#ifdef UNICODE
#define dpAccountActivate dpAccountActivateW
#else
#define dpAccountActivate dpAccountActivateA
#endif

/*------------------------------------------------------------------------
 Call this function after receiving a packet telling you to activate
 your account in response to dpAccountLogin() or dpAccountCreate()
 to request that your secret code be emailed again.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpRequestEmail(dp_t *dp);

/******************** Sending/Receiving Datagrams ************************/

/*------------------------------------------------------------------------
 Send a packet from one player to another.
 First two bytes of the message must indicate the message type.
 Users must not use any message type starting with dp_PACKET_INITIALBYTE.
 Message types are created with the dppt_MAKE macro.

 If the flags argument is dp_SEND_RELIABLE, dp will continue trying
 to send the packet until it is acknowledged.  Otherwise it will just
 send it once.

 NOTE:  It is best to always use a send buffer of
 size dpio_MAXLEN_UNRELIABLE (not dp_MAXLEN_RELIABLE, which is less).
 This is because an extra sizeof_dp_envelope_t bytes at end of
 buffer are sometimes used for dpid's.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSend(
	dp_t  *dp,
	dpid_t idFrom,
	dpid_t idTo,
	int flags,
	void *buffer,
	size_t size);

/*------------------------------------------------------------------------
 Send any unsent packets.  Call this after a bunch of dpSend's.
 Once this is called the first time, dpSend no longer flushes its buffer
 automatically; it waits for you to call this.
------------------------------------------------------------------------*/
DP_API dp_result_t dpFlush(dp_t *dp);

/*----------------------------------------------------------------------
 Receive a packet.
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

 NOTE:  It is best to always use a receive buffer of
 size dpio_MAXLEN_UNRELIABLE (not dp_MAXLEN_RELIABLE, which is less).
 This is because an extra sizeof_dp_envelope_t bytes at end of
 buffer are sometimes used for dpid's.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReceive(
	dp_t   *dp,
	dpid_t *idFrom,
	dpid_t *idTo,
	int    flags,
	void *buffer,
	size_t *size);

/******************** Session Management *********************************/

/*------------------------------------------------------------------------
 Start a new session, or join an existing one.
 If starting a new session, include dp_SESSION_FLAGS_CREATESESSION in
  flags argument for s.
 Otherwise if joining a specific session, pass that session in s.
 Or if joining any session, either pass in NULL for s if joining
  a lobby on the game server (dpSetGameServer must have been called)
 or pass in address including port number (in format returned by
  dpResolveHostname()) as well as the session type and
  a flags argument (indicating lobby or game) in s.
 Returns immediately.
 dp_receive will call the callback when operation completes, and will
 indicate success with a non-zero session description pointer.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpOpen(
	dp_t *dp,
	dp_session_t *s,
	dpEnumSessionsCallback_t cb,
	void *context);

/*------------------------------------------------------------------------
 Close an open session.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpClose(
	dp_t *dp);

#ifdef dp_ANET2
/*------------------------------------------------------------------------
 Cast off an open session - it stays open, but is no longer the
 "current session".
 After this, you can call dpOpen() to create a new session.

 Caveats:
 Intended for use only on game servers.
 Can only be called on the master.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCastoffSession(dp_t *dp);
#endif

/*------------------------------------------------------------------------
 Calls the callback function once with a description of
 each game session that this object could connect to,
 finishes by calling the callback with a null session pointer.

 Since our functions must be non-blocking, returns immediately,
 before any calls to the callback. The callback is called by dpReceive.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumSessions(
	dp_t *dp,
	dp_session_t *sDesc,
	char *mhn,					/* must be NULL */
	long timeout,				/* How long in milliseconds to wait. */
	dpEnumSessionsCallback_t cb,
	void *context);

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
    char key[dp_KEY_MAXLEN+1];
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

 Note: keylen will be dp_KEY_MAXLEN+1 when requesting player deltas
 in IPX sessions!
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpRequestObjectDeltas(
	dp_t *dp,
	int monitor,		/* TRUE to start, FALSE to stop */
	const char *key,
	int keylen);

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
DP_API dp_result_t DP_APIX dpSetPingIntervals(
	dp_t *dp,
	int piggybackPingIntervalMS,
	int forcedPingIntervalMS);

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
	int *pidlen);				/* length of resulting id stored here */

/*----------------------------------------------------------------------
 Declare that the current session is a lobby.
 May only be called by host.
 The parameter 'flags' should be set to 0 for a peer lobby,
 and to 1 for a server lobby.
 Bug: This function is incorrect.  Please talk to Dan about how
 to indicate that a session is a lobby.  The basic idea is to use the
 dp_SESSION_FLAGS_ISLOBBY flag when creating the session.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpDeclareLobby(
	dp_t *dp,
	int flags);

/************************ Player Management ******************************/

/*------------------------------------------------------------------------
 Add a new player to the currently open session.
 Calls given function when user is created and its dpid becomes available.
 On failure, either returns dp_RES_FULL immediately, or calls callback
 with dp_ID_NONE during later call to dpReceive.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCreatePlayer(
	dp_t	*dp,
	dpEnumPlayersCallback_t cb,
	void	*context,
	dp_char_t	*name);

/*------------------------------------------------------------------------
 Destroy the given player; removes the player from the game session.
 The dpID will not be reused during the current session.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpDestroyPlayer(
	dp_t   *dp,
	dpid_t id);

/*------------------------------------------------------------------------
 Return the number of players in the current session.
------------------------------------------------------------------------*/
DP_API int DP_APIX dpNumPlayers(
	dp_t *dp);

/*------------------------------------------------------------------------
 Get the id of the current host for the open session, if he has any
 players.
 Returns dp_RES_OK on success,
 		 dp_RES_CLOSED if no session is open,
 		 dp_RES_EMPTY if the host has no players.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetCurrentHostId(
	dp_t *dp,
	dpid_t *phid);

/*------------------------------------------------------------------------
 Convert a dpid into a player name.
 On success return dp_RES_OK, and copy at most bufLen-1 bytes of player
 name into buf; always null terminate result.
 On failure, return dp_RES_EMPTY.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetPlayerName(
	dp_t *dp,
	dpid_t id,
	dp_char_t *buf,
	size_t bufLen);

/*------------------------------------------------------------------------
 Set the name for the player indicated by the given dpid.
 If name is too long, it will be truncated.
 On success return dp_RES_OK
 On failure, return dp_RES_EMPTY.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetPlayerName(
	dp_t *dp,
	dpid_t id,
	dp_char_t *name);

/*------------------------------------------------------------------------
 Retrieve the blob associated with player having the given id.
 The blob is just a block of data that is carried with the player.

 On entry, *pbuflen holds the length of buf.
 If *pbuflen is not big enough to hold the blob, the actual
 size of the blob is placed in *pbuflen, and dp_RES_FULL is
 returned.

 On exit, *pbuflen holds the actual length of the blob,
 and the blob is copied into buf.
 On success return dp_RES_OK
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetPlayerBlob(
	dp_t *dp,
	dpid_t id,
	dp_char_t *buf,
	size_t *pbuflen);

/*------------------------------------------------------------------------
 Set the blob for the player indicated by the given dpid.
 If blob is too long, dp_RES_FULL is returned.
 On success return dp_RES_OK
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetPlayerBlob(
	dp_t *dp,
	dpid_t id,
	void *blob,
	size_t bloblen);

/*------------------------------------------------------------------------
 Convert a player id (dpid_t) to a user id (dp_uid_t).
 Returns dp_UID_NONE on any error.
------------------------------------------------------------------------*/
DP_API dp_uid_t DP_APIX dpGetPlayerUid(dp_t *dp, dpid_t id);

/*------------------------------------------------------------------------
 Calls the given function once for each player in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumPlayers(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	);

/*------------------------------------------------------------------------
 Calls the given function once for each player in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumPlayersEx(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersExCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	);

/*------------------------------------------------------------------------
 Enable or disable new players from entering the game.
 May only be called by host.
 If called with FALSE, session packets will no longer be broadcast,
 and add player requests will be ignored.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnableNewPlayers(
	dp_t *dp,
	int enable);

/*------------------------------------------------------------------------
 Measure round-trip transmission time to a player or the gamehost.
 To ping the gamehost, use dest = 0.
 Result is returned after a couple seconds via callback.
 Call with a unique karma (say, 1 for first call, 2 for 2nd call, etc.)
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpPingUser(
	dp_t *dp,
	dpid_t dest,
	short karma,
	dp_ping_callback_t cb);

/************************ Group Management *******************************/

/*------------------------------------------------------------------------
 Add a new group to the currently open session.
 Can only be called on game host!
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCreateGroup(
	dp_t	*dp,
	dpid_t *id,
	dp_char_t	*name);

/*------------------------------------------------------------------------
 Destroy the given group; removes the group from the game session.
 The dpID will not be reused during the current session.
 Can only be called on game host!
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpDestroyGroup(
	dp_t   *dp,
	dpid_t id);

/*------------------------------------------------------------------------
 Calls the given function once for each group in the given session, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the group in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.

 s must currently be NULL - that is, you can't yet enumerate the groups
 of sessions you haven't joined.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumGroups(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	);

/*------------------------------------------------------------------------
 Add player idPlayer to the group idGroup.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpAddPlayerToGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer);

/*------------------------------------------------------------------------
 Delete player idPlayer from the group idGroup.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpDeletePlayerFromGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer);

/*------------------------------------------------------------------------
 Calls the given function once for each player in the given group, then
 calls the given function once with dp_ID_NONE to indicate end of list.
 If s is NULL, lists the players in the current session.
 If s is not NULL, it must be a value from dpEnumSessions.

 s must currently be NULL - that is, you can't yet enumerate the group
 members of sessions you haven't joined.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumGroupPlayers(
	dp_t *dp,
	dpid_t groupId,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				/* How long in milliseconds to wait. */
	);

/******************** Player Variable Management *************************/

/* Values for flags parameter of dpSetPlayerData */
#define dp_PLAYERDATA_FLAG_NOFLOOD 0x01	/* Not sent except on dpSendPlayerData */
#define dp_PLAYERDATA_KEY_USERMAX 0xff00/* max user key; other vars reserved */
#define dp_PLAYERDATA_LEN_MAX 0x40000	/* max size of variable is 256K */

/*------------------------------------------------------------------------
 Set variable 'key' for a player.  See dp.htm for more info.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSetPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	void   *buf,
	size_t buflen,
	long   flags);

/*------------------------------------------------------------------------
 Send a given player's variable 'key' to a player (or to everybody,
 if idTo == dp_ID_BROADCAST).
 Only used for variables that were not sent when they were set, i.e.
 which were set with dpSendPlayerData(..., dp_PLAYERDATA_FLAG_NOFLOOD)
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpSendPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	dpid_t idTo);

/*------------------------------------------------------------------------
 Get a variable for a player.
 Caller must set *buflen before calling to the size of buf.

 If the player or variable does not exist, dp_RES_EMPTY is returned.
 If *buflen is not big enough, *buflen is set to the current size,
 and dp_res_FULL is returned.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	void   *buf,
	size_t *pbuflen,
	long   flags);

/************************** Miscellaneous ********************************/

/*------------------------------------------------------------------------
 Returns dp_RES_BUSY if dp can't freeze due to ongoing reliable transmission.
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReadyToFreeze(
	dp_t   *dp);

/*------------------------------------------------------------------------
 Freeze the current running game, and save it on disk for use during a
 future dpCreate.  Works even if packets are in transit.
 If you want to wait until the network is quiet, check dpReadyToFreeze().
------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpFreeze(
	dp_t   *dp,
	char *fName);

/*-------------------------------------------------------------------------
 Given a file pointer pointing to a dp_launchParams_t, initialize a
 dp_t and connect to or create a game session specified by the file.
 Do not return until the session has been established and a player has
 been created.
 During any delays, this function will call the given callback periodically
 to both inform the caller of its progress joining the session and to
 give the caller a chance to abort (by returning 0).
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCommThaw(dp_t **pdp, FILE *thawfp, dpCommThawCallback_t cb, void *context);

/*----------------------------------------------------------------------
 When you want to quit and terminate the game, you should poll this
 routine in a loop with dpReceive.  This will close the connection to
 the server and delete all sessions and players associated with it.
 Flags should normally be zero.

 dpShutdown will wait an additional wait_after ms after closing the
 connection to handle packet retries.
 If flags is 1, the connection to the game server will not be closed.
 
 Steps which are dependant on network events will time out in
 timeout ms, if timeout is non-zero.  Since there are three such
 states, the maximum total time for shut down is about
 (timeout * 3 + wait_after) ms.

 Returns dp_RES_OK when finished,
         dp_RES_BUSY while still shutting down.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpShutdown(dp_t *dp, clock_t timeout, clock_t wait_after, int flags);

#define dpReportScoreStart _dax7
#define dpReportScore2 _dax8
#define dpReportScoreEnd _dax9

/*-------------------------------------------------------------------------
 Begin a score report.
 Flag must be zero.
 This should only be called at the end of the game (but before dpClose).

 Call dpReportScoreStart before calling dpReportScore2, then
 call dpReportScore2 to report as many scores as you like, then finally
 call dpReportScoreEnd to finish sending the block of scores.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreStart(dp_t *dp, int flag);

#define dp_SCOREID_SCORE	0x1101	/* weighted score */
#define dp_SCOREID_RANK 	0x1103 	/* rank of player by weighted score */
#define dp_SCOREID_KILLS 	0x1104	/* # of enemies killed */
#define dp_SCOREID_DEATHS	0x1105	/* # of lives consumed */
#define dp_SCOREID_VICTORY	0x1106	/* whether this player won this game */

/*-------------------------------------------------------------------------
 Old score reporting function, dummied out.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScore2(dp_t *dp, dpid_t dpId, int scoreId, long scoreVal);

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
DP_API dp_result_t DP_APIX dpReportScoreBuf(dp_t *dp, dpid_t dpId, int scoreId, const char *scorebuf, int scorelen);

/*-------------------------------------------------------------------------
 End a score report.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportScoreEnd(dp_t *dp);

#define dp_PARAMID_SYNCJOIN 0x3001	/* 0 if dynamic, 1 if sync launch */
#define dp_PARAMID_RXWAIT 	0x4100	/* Change received mode of driver (linux only) */
#define dp_PARAMID_FILEDESC	0x4200	/* File Descriptor of Driver (linux only) */

/*-------------------------------------------------------------------------
 Retrieve a parameter set by an external game launcher, if any.
 On success, places the value of the parameter into *pparamVal.
 Returns dp_RES_OK on success.
 Returns dp_RES_UNIMPLEMENTED if that parameter is not supported by this
 driver.
 May return other error codes as well; depends on the driver being used.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetParameterLong(dp_t *dp, int paramId, long *pparamVal);

/*----------------------------------------------------------------------
 Read description out of the DLL file named by 'path'.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetTransportInfo(
	dp_transport_t *path,
	comm_driverInfo_t *info);

/*----------------------------------------------------------------------
 Get path and info about of currently loaded transport.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetCurrentTransportInfo(
	dp_t *dp,
	dp_transport_t *path,
	comm_driverInfo_t *info);

/*-------------------------------------------------------------------------
 Call this during or after dpEnumTransports, but before dpCreate,
 and only on transports which have the comm_INIT_NEEDS_PORTNUM bit set
 in comm_driverInfo_t.needs.
 Present the returned list of port names to the user, then use the
 matching portnum as the value for commInitReq_t.portnum in dpCreate.

 On entry,
 transport is the name (from dpEnumTransports) of the transport to query.
 ports is an array to be filled with portnames.
 maxports is the size of the ports array.

 On exit,
 ports is filled with descriptions of the available ports.
 *pnPorts is set to the number of portnames placed in the ports array.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumPorts(const dp_transport_t *transport, commPortName_t *ports, int maxports, int *pnPorts);

/*----------------------------------------------------------------------
 Calls the given function once for each transport available.
 All calls are made before dpEnumTransports returns.
 Path argument is to directory to search for DLL's.
 Eventually, will read description out of the DLL files.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumTransports(
	dp_transport_t *path,
	dpEnumTransportCallback_t cb,
	void *context);

/*----------------------------------------------------------------------
 Enumerate applications installed on this machine.
 Callback is called once for each application found.
 Callback ends when dpEnumApp() exits.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpEnumApp(dp_t *dp, dpEnumAppCallback_t cb, void *context);

#ifdef dp_ANET2
/*----------------------------------------------------------------------
 Determine the current application's version, and whether it needs to be
 updated.
 Returns info about the current app in *app.  Strings are allocated with
 strdup.

 Returns dp_RES_OK if update needed,
 dp_RES_EMPTY if no update needed,
 other values on failure.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpGetAppVersion(dp_t* dp, dp_appParam_t *app);

/*----------------------------------------------------------------------
 Gets appParam parameters from an anet.inf file.
 path is name of directory containing anet.inf.
 Not needed by most user programs.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReadAnetInf(const char *path, dp_appParam_t *pAppParam);

/*----------------------------------------------------------------------
 Determine whether application *app needs to be updated.
 Returns latest app version from game server in *app.
 Not needed by most user programs.

 Returns dp_RES_OK if update needed,
 dp_RES_EMPTY if no update needed.
----------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpCheckAppVersion(dp_t *dp, dp_appParam_t *app);

/*-------------------------------------------------------------------------
 Call to download the patch corresponding to the given product,
 but only if dpGetAppVersion says you need to.
 app is the structure filled in by dpGetAppVersion.
 If it returns dp_RES_OK, you should exit your application immediately!
 See also updater.htm.

 Returns dp_RES_OK if patch is downloaded and running;
 dp_RES_EMPTY if no patch needed,
 dp_RES_BAD if can't find updater dll (anet/dkupddll.dll).
 dp_RES_USER_CANCELLED if user offered patch, but declined it.
-------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpDownloadUpdate(dp_t *dp, const dp_appParam_t *app);

#endif

/*--------------------------------------------------------------------------
 Launches an application from another application and notifies stub
  of the newly launched application, if stub exists.
 If /NEWCONSOLE is found in appParam->shellOpts, a new console is created.
 If /OLDCONSOLE is found in appParam->shellOpts, the old console is used.

 result may be NULL.  If result is not NULL, it is filled with system
 dependant information about the newly launched app.  On Win32 systems,
 if result is NULL, the handle to the newly created process is closed,
 otherwise it is left open for the caller.
--------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpLaunchApp(dp_appParam_t *appParam);
DP_API dp_result_t DP_APIX dpLaunchApp2(dp_appParam_t *appParam, dpLaunchApp_result_t *result);

#ifdef __POWERPC__
/*--------------------------------------------------------------------------
 Searches for stub and if it exists, notifies it of pending exit and if it
  wants NetShell relaunched (yes if status is zero) then exits application.
--------------------------------------------------------------------------*/
DP_API void DP_APIX dpExitFromApp(int status);
#else
#define dpExitFromApp(s) exit(s)
#endif

#ifdef dp_ANET2
#ifdef _WIN32
/*--------------------------------------------------------------------------
 Records crash info to logfile. Designed to be called from an exception handler.
 For Win32, pException is the LPEXCEPTION_POINTERS structure returned
 by GetExceptionInformation().
 crshtxt is included in the crash record; crshtxt is truncated if longer than
  100 bytes or if the crash record is already 512 bytes.
 Returns dp_RES_ALREADY if dpReportCrash(Ex) was called previously; in this
  case, does not write a new crash record to log unless the crash was caused
  during dpReportCrash(Ex).
 See aeh.htm for more info.
--------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportCrash(LPEXCEPTION_POINTERS pException);
DP_API dp_result_t DP_APIX dpReportCrashEx(LPEXCEPTION_POINTERS pException, char *crshtxt);
#endif

/*--------------------------------------------------------------------------
 Records assertion failure to logfile.
--------------------------------------------------------------------------*/
DP_API dp_result_t DP_APIX dpReportAssertionFailure(int lineno, char *file, char *linetxt);

#include <assert.h>
#ifdef _WIN32
#define DP_ASSERT(exp) (void)((exp) || dpReportAssertionFailure(__LINE__, __FILE__, #exp), assert(exp), exit(1), 0)

#elif defined(UNIX)
#define DP_ASSERT(exp) (void)((exp) || dpReportAssertionFailure(__LINE__, __FILE__, __STRING(exp)), assert(exp), exit(1), 0)

#else /* not implemented yet */
#define DP_ASSERT(exp) assert(exp)
#endif

#endif

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
DP_API dp_result_t DP_APIX dpGetStats(
	dp_t *dp,
	int statkind,
	dp_stat_t *buf,
	size_t buflen);
#endif

#ifdef dp_ANET2
/************************** Advanced API ********************************/

/*----------------------------------------------------------------------
 Pack a dp_session_t into a compact, uniform byte order form for transmission.
 Doesn't need to send external address or karma, but
 will need to send internal address if it's not the same as external.
 If any field is too long, it is truncated.
 Returns length used.
----------------------------------------------------------------------*/
DP_API int DP_APIX dp_pack_session(dp_t *dp, dp_species_t defaultSessionType, const dp_session_t *p, char *buf);

/*----------------------------------------------------------------------
 Unpack the compact, byte-order-uniform version of a dp_session_t
 into the fluffy form we use internally.
 Returns number of bytes used, or -1 on error.
----------------------------------------------------------------------*/
DP_API int DP_APIX dp_unpack_session(dp_t *dp, const char *subkey, int subkeylen, const char *buf, size_t buflen, dp_session_t *p);

#endif

#endif /* ifndef dp_NO_FUNCTIONS */
#endif

/************************** Ini File Access ********************************/

#ifndef anet_ini_h
#define anet_ini_h
#define dpini_DEFAULT_FILENAME "dp.ini"

/*--------------------------------------------------------------------------
 Change the .INI file accessed by future calls.
--------------------------------------------------------------------------*/
DP_API void DP_APIX dpini_SetFile(char *iniFileName);

/*--------------------------------------------------------------------------
 Return the name of the current .INI file.
 Value is a pointer to a static string; the caller should copy it
 to a buffer immediately.
--------------------------------------------------------------------------*/
DP_API const char * DP_APIX dpini_GetFile(void);

/*--------------------------------------------------------------------------
 Search the current .INI file for a section header.
 Section headers are case-insensitive.

 Return non-zero on any error.
--------------------------------------------------------------------------*/

DP_API int DP_APIX dpini_findSection(
	char *sectionWant);	/* Section we want, minus "[" and "]"; case-blind */

/*--------------------------------------------------------------------------
 Search the current section of the current .INI file for the parameter.
 Parameter names are case-insensitive.
 If dpini_findSection() has not yet been called, returns the empty string.

 If the {verbatim} parameter is true, returns a pointer to the entire rest
 of the line following the "=" sign in the .INI file.
 If {verbatim} is false, any comment beginning with a semicolon is stripped
 out, and white space surrounding the parameter value is deleted.
 In either case, the returned string ends with a NUL but no CR or LF.

 The next call to readINIParameter() will overwrite the buffer.
--------------------------------------------------------------------------*/

DP_API const char * DP_APIX dpini_readParameter(
	char *paramWant,	/* Parameter we want, minus "="; case-insensitive */
	int	  verbatim);	/* Return everything after the "=" (else clean)? */

/************************** Debug Log File ********************************/

/*--------------------------------------------------------------------------
 Debug message service.
 Controlled by .ini file; can write DPRINT messages to screen or file.

 To print a debugging message, use
   DPRINT(("message %s", str)),
 compile with the DPRNT or DEBUG preprocessor flag turned on,
 and link in the debugging version of this library.

 At the start of main, call
   dp_enableDebugPrint(TRUE);
 to initialize the log file.

 A shorthand way to stick the current file and line number into the log
 file is
	dp_CKPT;
 This only has effect if "warnings" are turned on in the .ini file.
--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 Not normally needed.

 Call to flush log file.
--------------------------------------------------------------------------*/
DP_API void DP_APIX dp_flushLog(void);

/*--------------------------------------------------------------------------
 Not normally needed.

 Call with 1 to increase debug printing level (i.e. show more info);
 call with -1 to decrease debug printing level (i.e. show less info);
 call with 0 to disable all debug printing.
--------------------------------------------------------------------------*/
DP_API void DP_APIX dp_enableDebugPrint(int enable);

#define dp_LOG_FILE_NONE ((FILE *)0xffffffff)

/*--------------------------------------------------------------------------
 Call this to retrieve the file pointer which dp is using for logging.
 Most programs won't need this.
 The only legal thing to do with the return value is to pass it to
 dp_setLogFP().  This is handy for context switching when using
 multiple dp_t's in the same program, as in our test bench.
--------------------------------------------------------------------------*/
DP_API FILE * DP_APIX dp_getLogFP(void);

/*--------------------------------------------------------------------------
 Call this to set the file pointer which dp should use for logging.
 Most programs won't need this.
 If _fp is dp_LOG_FILE_NONE, this disables logging entirely.
 This will override any filename specified by the .ini file.
 (Note that dp_setLogFname closes the currently open log file, regardless
 of whether it was passed in via this function, or opened by dp.)
 WARNING: unless you're very sure your program is using the same C runtime
 library instance as does this code, only use the return value of 
 dp_getLogFP().
--------------------------------------------------------------------------*/
DP_API void DP_APIX dp_setLogFP(FILE *_fp);

/*--------------------------------------------------------------------------
 Call to set default log filename.  (Default value is "dp.log".)
 This value may be overridden by setting a log filename in the .ini file.
 Note: no log file unless you are linked with the debugging version of
 the library, and a .ini file with (for example) All=1 and ToLog=1 lines
 in the [DEBUG] section exists (see dp.htm#inifile).
--------------------------------------------------------------------------*/
DP_API void DP_APIX dp_setLogFname(const char *fname);

/*--------------------------------------------------------------------------
 Print a debug string to the destination set in the .ini file.
 See dp.htm#inifile for a detailed description of the .ini file format.
 Messages beginning with @ are only displayed if warnings are turned on
 in the .ini file.
 Returns the number of characters sent.
 Don't use dp_dprintf directly; use the DPRINT macro instead.
--------------------------------------------------------------------------*/

DP_API int DP_APIX dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...);						/* printf-style arguments on stack (if any) */

#if defined(__MWERKS__)
#pragma export off
#endif

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
#define DPRINT(p) (dp_dprintf p )
#else
#define DPRINT(p)
#endif

/* Checkpoint - just toss these in like candy throughout areas
 * you suspect of crashing.  The @ symbol in front is a flag to DPRINT
 * to only display them if "warnings" are turned on in the .ini file.
 */
#define dp_CKPT		DPRINT(("@" __FILE__ " %u:\n", __LINE__))

#endif

/*************************************************************************/

#ifdef __cplusplus
}
#endif
