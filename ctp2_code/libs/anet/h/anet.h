





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


#ifndef dp_ANET1		
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


#ifndef PASCAL
#define dp_PASCAL
#else
#define dp_PASCAL PASCAL
#endif
#ifndef FAR
#define dp_FAR
#else
#define dp_FAR FAR		
#endif

#ifndef DP_API
  #if defined(_WIN32) && defined(DP_DLL)
    #define DP_API __declspec( dllimport )
  #else
    #define DP_API
  #endif
#endif

#ifndef DP_APIX
  #if defined(_WIN32)
    #define DP_APIX cdecl
  #else
    #define DP_APIX
  #endif
#endif





















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


#define dpGETSHORT_FIRSTBYTE(s) (unsigned char)((s) & 0xff)
#define dpGETSHORT_SECONDBYTE(s) (unsigned char)(((s) >> 8) & 0xff)


#define dpGETLONG_FIRSTBYTE(s)  (unsigned char)((s) & 0xff)
#define dpGETLONG_SECONDBYTE(s) (unsigned char)(((s) >> 8) & 0xff)
#define dpGETLONG_THIRDBYTE(s)  (unsigned char)(((s) >> 16) & 0xff)
#define dpGETLONG_FOURTHBYTE(s) (unsigned char)(((s) >> 24) & 0xff)


#define dpMAKESHORT(first,second) ((unsigned char)(first) |\
								 (((unsigned short)((unsigned char)(second)))<<8))


#define dpMAKELONG(first,second,third,fourth) (   \
		(unsigned long)dpMAKESHORT(first,second) |\
		((unsigned long)dpMAKESHORT(third, fourth)) << 16)




#ifndef PACK
#define PACK
#endif
















#if defined(__WATCOMC__)    
#pragma pack(1)
#elif defined(__MWERKS__)   
#pragma options align=packed
#elif defined(_M_IX86)      
#pragma pack(push, 1)
#endif


#define dp_BROADCAST_THRESHOLD	18	
#define dp_MAXSESSIONS	32			
#define dp_PLAYER_TIMEOUT_SECS 41	
#define dp_MAXREALPLAYERS	30		

#define sizeof_dp_envelope_t 6


typedef char dp_char_t;		

typedef dp_char_t char_t;	

#define dp_ID_NAMESERVER	0
#define dp_ID_BROADCAST		0
#define dp_ID_LOCAL_NAMESERVER	1
#define dp_ID_FIRST_FREE	2

#include "aneterr.h"		



#define dp_MAXLEN_UNRELIABLE (dpio_MAXLEN_UNRELIABLE - sizeof_dp_envelope_t)
#define dp_MAXLEN_RELIABLE   (dp_MAXLEN_UNRELIABLE - sizeof_dpio_reliable_header)



#define dp_KARMA_MAKE(pseudo,k) (((pseudo) ? 0x8000:0) | ((k)&0x7fff))
#define dp_KARMA_IS_PSEUDO(karma) (((karma) & 0x8000) != 0)


#define dp_SNAMELEN 32
#define dp_USERFIELDLEN 10
#define dp_PASSWORDLEN 10
#define dp_MAX_ADR_LEN 10	



#define dp_SESSION_FLAGS_OPENSESSION		0x00
#define dp_SESSION_FLAGS_CREATESESSION		0x01
#define dp_SESSION_FLAGS_USE_BROADCAST		0x02
#define dp_SESSION_FLAGS_ENABLE_NEWPLAYERS	0x04	
#define dp_SESSION_FLAGS_ISSERVER			0x08	
#define dp_SESSION_FLAGS_ISLOBBY			0x10
#define dp_SESSION_FLAGS_MIGRATE_FORBIDDEN	0x20
#define dp_SESSION_FLAGS_ENABLE_PLAYERVARS	0x40
#define dp_SESSION_FLAGS_ENABLE_DUALADDRESS	0x80	

#if defined(__MWERKS__)  
#define __NO_ANONYMOUS_UNIONS__
#endif





#ifndef __NO_ANONYMOUS_UNIONS__
#define DP_ALIAS(type,name1,name2) union { type name1 PACK; type name2 PACK; }
#define DP_ALIAS3(type,name1,name2,name3) union { type name1 PACK; type name2 PACK; type name3 PACK; }
#else
#define DP_ALIAS(type,name1,name2) type name1 PACK
#define DP_ALIAS3(type,name1,name2,name3) type name1 PACK
#endif

typedef struct {

	DP_ALIAS(char, dummy, dwSize);	
	unsigned char adrMaster[dp_MAX_ADR_LEN] PACK;	
	dpid_t		masterPseudoplayer PACK;
	
	DP_ALIAS3(dp_species_t, sessionType, guidSession, guidApplication);
	
	DP_ALIAS(dp_karma_t, karma, dwSession);
	
	DP_ALIAS(short,	maxPlayers, dwMaxPlayers);
	
	DP_ALIAS(short,	currentPlayers, dwCurrentPlayers);
	DP_ALIAS(short,	flags, dwFlags);
	
	DP_ALIAS3(dp_char_t, sessionName[dp_SNAMELEN], szSessionName[dp_SNAMELEN], lpszSessionNameA[dp_SNAMELEN]);
	char		szUserField[dp_USERFIELDLEN] PACK;	
	char		szPassword[dp_PASSWORDLEN] PACK;		
	long		dwReserved1 PACK;
	long		dwUser1 PACK;						
#ifdef dp_ANET2
	unsigned char	reserved2[dp_MAX_ADR_LEN+2] PACK;
#endif
} dp_session_t;	

#define sizeof_dp_session_packet_t 83		





#define dp_PNAMELEN 24

#ifdef dp_ANET2
#define dp_MAX_PLAYERBLOB_LEN 16
#endif

typedef struct {
	DP_ALIAS(dpid_t, id, dpId) PACK;
	dp_karma_t	karma PACK;
	unsigned char	adr[dp_MAX_ADR_LEN] PACK;
	char			name[dp_PNAMELEN] PACK;
#ifdef dp_ANET2
	unsigned char bloblen;
	unsigned char blob[dp_MAX_PLAYERBLOB_LEN];	
#endif
} dp_playerId_t;


typedef struct {
	unsigned short major PACK;
	unsigned short minor PACK;
} dp_version_t;






#define dp_PLAT_UNKNOWN	0
#define dp_LANG_UNKNOWN	0
#define dp_VERS_UNKNOWN	0xFF
typedef struct {
	char *name;					
	char *path;					
	char *args;					
	char *cwd;					
	char *shellOpts;			
	dp_species_t sessionType;	
	unsigned short platform;	
	unsigned char language;		
	dp_version_t current;		
	dp_version_t latest;		
} dp_appParam_t;



#ifndef aeh_h
#define aeh_ASSERTION_CODE   0xE0000002 
#endif




#define dppt_MAKE(a,b) (dp_packetType_t) SwapBytes2(((a)&255) | ((b)<<8))

#define dp_PACKET_INITIALBYTE 'd'





#define dp_USER_ADDPLAYER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'1')
typedef dp_playerId_t dp_user_addPlayer_packet_t;





#define dp_USER_DELPLAYER_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'2')
typedef dp_playerId_t dp_user_delPlayer_packet_t;



#define dp_USER_ADDGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'3')
typedef struct dp_groupId_s dp_user_addGroup_packet_t;



#define dp_USER_DELGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'4')
typedef struct dp_groupId_s dp_user_delGroup_packet_t;



#define dp_USER_ADDPLAYERTOGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'5')
typedef struct {
	dpid_t		dpIdGroup PACK;
	dpid_t		dpIdPlayer PACK;
	dp_karma_t	sessionKarma PACK;		
} dp_addPlayerToGroup_packet_t;



#define dp_USER_DELPLAYERFROMGROUP_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'6')
typedef dp_addPlayerToGroup_packet_t dp_delPlayerFromGroup_packet_t;


#define dp_USER_HOST_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'7')

typedef struct dp_groupId_s {
	dpid_t		id PACK;				
	dp_karma_t	karma PACK;				
	dp_char_t		name[dp_PNAMELEN] PACK;	
	dp_karma_t	sessionKarma PACK;		
} dp_groupId_t;




#define dp_USER_PLAYERDATA_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'8')
typedef struct {
	size_t len PACK;			
	dpid_t id PACK;
	unsigned short key PACK;
	void *data PACK;			
} dp_user_playerData_packet_t;




#define dp_SESSIONLOST_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'9')
typedef dp_session_t dp_sessionLost_packet_t;




#define dp_SESSIONRESULT_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'0')
typedef struct {
	dp_result_t reason PACK;
	dp_session_t sess PACK;
} dp_sessionResult_packet_t;



#ifndef _CommAPI_h_

#define comm_DRIVER_SIGLEN			8   
#define comm_DRIVER_NAMELEN			32

typedef struct {
	char signature[comm_DRIVER_SIGLEN] PACK;
	size_t recordLen PACK;				
	char name[comm_DRIVER_NAMELEN] PACK;
	short version PACK;					
	short capabilities PACK;			
	short needs PACK;					
} comm_driverInfo_t;


#define comm_PORT_ANY	(0)			


#define comm_DRIVER_IS_VISIBLE		1		
#define comm_DRIVER_KNOWS_PLAYERLST	2		
#define comm_DRIVER_ALLOWS_GAMELIST	4		
#define comm_DRIVER_IS_FAST			8		
#define comm_DRIVER_PREFERS_BROADCAST	16	
#define comm_DRIVER_NO_BROADCAST	32		


#define comm_INIT_NEEDS_SESSIONID	2
#define comm_INIT_NEEDS_PORTNUM		4
#define comm_INIT_NEEDS_BAUD		8
#define comm_INIT_NEEDS_BASEADR		0x10
#define comm_INIT_NEEDS_HWIRQ		0x20
#define comm_INIT_NEEDS_SWINT		0x40
#define comm_INIT_NEEDS_PHONENUM	0x80
#define comm_INIT_NEEDS_MODEMINISTR	0x100


#define comm_INIT_FLAGS_RESUME		1		
#define comm_INIT_FLAGS_TEST		2		


#define comm_INIT_DIALING_METHOD_PULSE 0
#define comm_INIT_DIALING_METHOD_TONE  1

typedef struct {			
	size_t reqLen PACK;			
	long sessionId PACK;		
	long portnum PACK;
	long baud PACK;
	long baseadr PACK;          
	long hwirq PACK;            
	long swint PACK;            
	char *phonenum PACK;
	char *modeministr PACK;
	long flags PACK;			
	long dialing_method PACK;	
} commInitReq_t;

typedef struct {
	int portnum;			
	char name[64];			
} commPortName_t;

#endif






typedef struct {
#if defined(__MWERKS__)   
	char fname[120];
	long vRefNum;
	long dirID;
#else
	char fname[128];
#endif
} dp_transport_t;

#ifdef dp_ANET2



typedef struct dp_serverInfo_s {
	short len;						
	short rtt_ms_avg;				
	short loss_percent;				
	short cur_users;				
	short max_users;				

	char hostname[64];				
	dp_species_t sessType;			
	short cur_sessTypeUsers;		
	short cur_games;				
	short cur_sessTypeGames;		
	char reserved[16];				
} dp_serverInfo_t;

#define dp_USER_NAME_LEN 16
#define dp_USER_DESCRIPTION_LEN 256
#define dp_USER_URL_LEN 64




typedef struct {
	dp_uid_t uid PACK;
	wchar_t uname[dp_USER_NAME_LEN] PACK;
	wchar_t url[dp_USER_URL_LEN] PACK;
	wchar_t description[dp_USER_DESCRIPTION_LEN] PACK;
} dp_userInfo_t;




typedef struct {
	short nScoreTypes PACK;
	short scoreIds[1 ] PACK;	
	long scores[1  ] PACK;		
} dp_scoreInfo_t;


typedef union {
	dp_playerId_t   p;
	dp_session_t    sess;
	dp_serverInfo_t serv;
	dp_userInfo_t	user;
	dp_scoreInfo_t  score;
} dp_object_t;

#define dp_KEY_SESSIONS			1	
#define dp_KEY_PLAYERS			3
#define dp_KEY_SERVERPINGS		10	
#define dp_KEY_SCORES			14	
#define dp_KEY_USERS			30	
#define dp_KEY_PLAYER_LATENCIES 31  




#define dp_OBJECTDELTA_PACKET_ID		dppt_MAKE(dp_PACKET_INITIALBYTE,'+')
#define dp_KEY_MAXLEN 12    
#define dp_OBJECTDELTA_FLAG_LOCAL 1	
#define dp_OBJECTDELTA_FLAG_INOPENSESS 2	
#define dp_OBJECTDELTA_FLAG_ISHOST 4	
typedef struct {
	short pktloss PACK;		
	short latency PACK;     
	
	long flags PACK;		

	



	dp_result_t status PACK;

	



	short keylen PACK;
	char key[dp_KEY_MAXLEN] PACK;

	
	short subkeylen PACK;
	char subkey[dp_KEY_MAXLEN] PACK;

	




	dp_object_t data;
	
} dp_objectDelta_packet_t;












#define dp_ACCOUNT_PACKET_ID dppt_MAKE(dp_PACKET_INITIALBYTE,'-')
typedef struct {
	dp_uid_t		uid;		
	dp_result_t 	reason;	
	unsigned long	reserved;	
} dp_account_packet_t;
#endif






typedef int (dp_FAR dp_PASCAL *dpEnumServersCallback_t) (const char *hostname, long roundtrip_ms,long *pTimeout,long flags,void *context);


typedef int (dp_FAR dp_PASCAL *dpEnumServersExCallback_t)(const char *hostname, long roundtrip_ms,dp_serverInfo_t *server,long *pTimeout,long flags,
void *context);




typedef void (dp_FAR dp_PASCAL *dp_ping_callback_t)(dp_karma_t karma, long avg_ms, int loss_pct);




typedef int (dp_FAR dp_PASCAL *dpEnumSessionsCallback_t)(dp_session_t *sDesc,long *pTimeout,long flags,void *context);




typedef void (dp_FAR dp_PASCAL *dpOpenConnCallback_t)(void *addr, int len,
	int n_conns, dp_result_t state, char *user, char *pass, void *context);






#define dp_EPC_FLAGS_LOCAL 1
#define dp_EPC_FLAGS_REMOTE 2
typedef void (dp_FAR dp_PASCAL *dpEnumPlayersCallback_t)(dpid_t id, dp_char_t *name, long flags, void *context);


typedef void (dp_FAR dp_PASCAL *dpEnumPlayersExCallback_t)(dpid_t id, dp_char_t *name, long flags, void *context, dp_playerId_t *player);




typedef void (dp_FAR dp_PASCAL *dpExceptionCallback_t)(void *context, dp_result_t err, char *msg);





typedef void (dp_FAR dp_PASCAL *dpEnumTransportCallback_t)( const dp_transport_t *fname, const comm_driverInfo_t *description, void *context);




typedef void (dp_FAR dp_PASCAL *dpEnumAppCallback_t)(dp_appParam_t *appParam, void *context);







typedef int (dp_FAR dp_PASCAL *dpCommThawCallback_t)(int status, void *context);



#define dp_CAPS_FLAGS_ISHOST 1	
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





















typedef struct {
	long in,
		 out,
		 dropped,
		 waiting,
		 sum_waiting,
		 num_waiting;
} dp_stat_t;

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

#define dp_STAT_MAX  13
#endif


typedef struct {
#ifdef _WIN32
	HANDLE hProcess;
#else
	char dummy;
#endif
} dpLaunchApp_result_t;



#if defined(__WATCOMC__)    
#pragma pack()
#elif defined(__MWERKS__)   
#pragma options align=reset
#elif defined(_M_IX86)      
#pragma pack(pop)
#endif

#endif
#endif



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














DP_API dp_result_t DP_APIX dpEnumServers(
	dp_t *dp,
	long timeout,				
	dpEnumServersCallback_t cb,
	void *context);









DP_API dp_result_t DP_APIX dpEnumServersEx(
	dp_t *dp,
	long timeout,				
	dp_species_t sessType,
	dpEnumServersExCallback_t cb,
	void *context);







DP_API dp_result_t DP_APIX dpSetClocksPerSec(
	dp_t *dp,
	int cps);






DP_API dp_result_t DP_APIX dpSetConnCallback(
	dp_t *dp, dpOpenConnCallback_t cb, void *context);

#define dpSetActiveThread _daoot




DP_API dp_result_t DP_APIX dpSetActiveThread(dp_t *dp);















DP_API dp_result_t DP_APIX dpCreate(
	dp_t **pdp,
	dp_transport_t *transport,
	commInitReq_t *params,
	char *thawFilename);







DP_API dp_result_t DP_APIX dpDestroy(
	dp_t *dp,
	int flags);




DP_API dp_result_t DP_APIX dpResolveHostname(
	dp_t *dp,
	char *hostname,
	char adrbuf[dp_MAX_ADR_LEN]);






















DP_API dp_result_t DP_APIX dpSetGameServerEx(
	dp_t *dp,
	char *masterHostName,	
	dp_species_t sessionType);






DP_API dp_result_t DP_APIX dpGetGameServerEx(
	dp_t *dp,
	char *masterHostNameBuf,
	size_t masterHostNameBufLen,
	dp_species_t *psessionType);





DP_API dp_result_t DP_APIX dpSetGameServer(
	dp_t *dp,
	char *masterHostName);	







DP_API dp_result_t DP_APIX dpGetSessionDesc(
	dp_t *dp,
	dp_session_t *buf,
	size_t *pbuflen);










DP_API dp_result_t DP_APIX dpSetSessionDesc(
	dp_t *dp,
	const dp_session_t *buf,
	long flags);







DP_API dp_result_t DP_APIX dpGetCaps(
	dp_t *dp,
	dp_caps_t *info,
	long flags);








DP_API dp_result_t DP_APIX dpGetPlayerCaps(
	dp_t *dp,
	dpid_t id,
	dp_caps_t *info,
	long flags);



#define dpAccountLoginW _dalw
#define dpAccountLoginA _dala










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






DP_API dp_result_t DP_APIX dpRequestEmail(dp_t *dp);


















DP_API dp_result_t DP_APIX dpSend(
	dp_t  *dp,
	dpid_t idFrom,
	dpid_t idTo,
	int flags,
	void *buffer,
	size_t size);






DP_API dp_result_t dpFlush(dp_t *dp);





















DP_API dp_result_t DP_APIX dpReceive(
	dp_t   *dp,
	dpid_t *idFrom,
	dpid_t *idTo,
	int    flags,
	void *buffer,
	size_t *size);

















DP_API dp_result_t DP_APIX dpOpen(
	dp_t *dp,
	dp_session_t *s,
	dpEnumSessionsCallback_t cb,
	void *context);




DP_API dp_result_t DP_APIX dpClose(
	dp_t *dp);

#ifdef dp_ANET2









DP_API dp_result_t DP_APIX dpCastoffSession(dp_t *dp);
#endif









DP_API dp_result_t DP_APIX dpEnumSessions(
	dp_t *dp,
	dp_session_t *sDesc,
	char *mhn,					
	long timeout,				
	dpEnumSessionsCallback_t cb,
	void *context);










































DP_API dp_result_t DP_APIX dpRequestObjectDeltas(
	dp_t *dp,
	int monitor,		
	const char *key,
	int keylen);






















DP_API dp_result_t DP_APIX dpSetPingIntervals(
	dp_t *dp,
	int piggybackPingIntervalMS,
	int forcedPingIntervalMS);










DP_API dp_result_t DP_APIX dpGetSessionId(
	dp_t *dp,
	const dp_session_t *sess,	
	char *id,					
	int *pidlen);				










DP_API dp_result_t DP_APIX dpDeclareLobby(
	dp_t *dp,
	int flags);









DP_API dp_result_t DP_APIX dpCreatePlayer(
	dp_t	*dp,
	dpEnumPlayersCallback_t cb,
	void	*context,
	dp_char_t	*name);





DP_API dp_result_t DP_APIX dpDestroyPlayer(
	dp_t   *dp,
	dpid_t id);




DP_API int DP_APIX dpNumPlayers(
	dp_t *dp);








DP_API dp_result_t DP_APIX dpGetCurrentHostId(
	dp_t *dp,
	dpid_t *phid);







DP_API dp_result_t DP_APIX dpGetPlayerName(
	dp_t *dp,
	dpid_t id,
	dp_char_t *buf,
	size_t bufLen);







DP_API dp_result_t DP_APIX dpSetPlayerName(
	dp_t *dp,
	dpid_t id,
	dp_char_t *name);














DP_API dp_result_t DP_APIX dpGetPlayerBlob(
	dp_t *dp,
	dpid_t id,
	dp_char_t *buf,
	size_t *pbuflen);






DP_API dp_result_t DP_APIX dpSetPlayerBlob(
	dp_t *dp,
	dpid_t id,
	void *blob,
	size_t bloblen);





DP_API dp_uid_t DP_APIX dpGetPlayerUid(dp_t *dp, dpid_t id);







DP_API dp_result_t DP_APIX dpEnumPlayers(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				
	);







DP_API dp_result_t DP_APIX dpEnumPlayersEx(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersExCallback_t cb,
	void *context,
	long timeout				
	);







DP_API dp_result_t DP_APIX dpEnableNewPlayers(
	dp_t *dp,
	int enable);







DP_API dp_result_t DP_APIX dpPingUser(
	dp_t *dp,
	dpid_t dest,
	short karma,
	dp_ping_callback_t cb);







DP_API dp_result_t DP_APIX dpCreateGroup(
	dp_t	*dp,
	dpid_t *id,
	dp_char_t	*name);






DP_API dp_result_t DP_APIX dpDestroyGroup(
	dp_t   *dp,
	dpid_t id);










DP_API dp_result_t DP_APIX dpEnumGroups(
	dp_t *dp,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				
	);




DP_API dp_result_t DP_APIX dpAddPlayerToGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer);




DP_API dp_result_t DP_APIX dpDeletePlayerFromGroup(
	dp_t	*dp,
	dpid_t	idGroup,
	dpid_t	idPlayer);










DP_API dp_result_t DP_APIX dpEnumGroupPlayers(
	dp_t *dp,
	dpid_t groupId,
	dp_session_t *s,
	dpEnumPlayersCallback_t cb,
	void *context,
	long timeout				
	);




#define dp_PLAYERDATA_FLAG_NOFLOOD 0x01	
#define dp_PLAYERDATA_KEY_USERMAX 0xff00
#define dp_PLAYERDATA_LEN_MAX 0x40000	




DP_API dp_result_t DP_APIX dpSetPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	void   *buf,
	size_t buflen,
	long   flags);







DP_API dp_result_t DP_APIX dpSendPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	dpid_t idTo);









DP_API dp_result_t DP_APIX dpGetPlayerData(
	dp_t   *dp,
	dpid_t idPlayer,
	int    key,
	void   *buf,
	size_t *pbuflen,
	long   flags);






DP_API dp_result_t DP_APIX dpReadyToFreeze(
	dp_t   *dp);






DP_API dp_result_t DP_APIX dpFreeze(
	dp_t   *dp,
	char *fName);










DP_API dp_result_t DP_APIX dpCommThaw(dp_t **pdp, FILE *thawfp, dpCommThawCallback_t cb, void *context);



















DP_API dp_result_t DP_APIX dpShutdown(dp_t *dp, clock_t timeout, clock_t wait_after, int flags);

#define dpReportScoreStart _dax7
#define dpReportScore2 _dax8
#define dpReportScoreEnd _dax9










DP_API dp_result_t DP_APIX dpReportScoreStart(dp_t *dp, int flag);

#define dp_SCOREID_SCORE	0x1101	
#define dp_SCOREID_RANK 	0x1103 	
#define dp_SCOREID_KILLS 	0x1104	
#define dp_SCOREID_DEATHS	0x1105	
#define dp_SCOREID_VICTORY	0x1106	




DP_API dp_result_t DP_APIX dpReportScore2(dp_t *dp, dpid_t dpId, int scoreId, long scoreVal);




















DP_API dp_result_t DP_APIX dpReportScoreBuf(dp_t *dp, dpid_t dpId, int scoreId, const char *scorebuf, int scorelen);




DP_API dp_result_t DP_APIX dpReportScoreEnd(dp_t *dp);

#define dp_PARAMID_SYNCJOIN 0x3001	
#define dp_PARAMID_RXWAIT 	0x4100	
#define dp_PARAMID_FILEDESC	0x4200	









DP_API dp_result_t DP_APIX dpGetParameterLong(dp_t *dp, int paramId, long *pparamVal);




DP_API dp_result_t DP_APIX dpGetTransportInfo(
	dp_transport_t *path,
	comm_driverInfo_t *info);




DP_API dp_result_t DP_APIX dpGetCurrentTransportInfo(
	dp_t *dp,
	dp_transport_t *path,
	comm_driverInfo_t *info);

















DP_API dp_result_t DP_APIX dpEnumPorts(const dp_transport_t *transport, commPortName_t *ports, int maxports, int *pnPorts);







DP_API dp_result_t DP_APIX dpEnumTransports(
	dp_transport_t *path,
	dpEnumTransportCallback_t cb,
	void *context);






DP_API dp_result_t DP_APIX dpEnumApp(dp_t *dp, dpEnumAppCallback_t cb, void *context);

#ifdef dp_ANET2










DP_API dp_result_t DP_APIX dpGetAppVersion(dp_t* dp, dp_appParam_t *app);






DP_API dp_result_t DP_APIX dpReadAnetInf(const char *path, dp_appParam_t *pAppParam);









DP_API dp_result_t DP_APIX dpCheckAppVersion(dp_t *dp, dp_appParam_t *app);













DP_API dp_result_t DP_APIX dpDownloadUpdate(dp_t *dp, const dp_appParam_t *app);

#endif












DP_API dp_result_t DP_APIX dpLaunchApp(dp_appParam_t *appParam);
DP_API dp_result_t DP_APIX dpLaunchApp2(dp_appParam_t *appParam, dpLaunchApp_result_t *result);

#ifdef __POWERPC__




DP_API void DP_APIX dpExitFromApp(int status);
#else
#define dpExitFromApp(s) exit(s)
#endif

#ifdef dp_ANET2
#ifdef _WIN32











DP_API dp_result_t DP_APIX dpReportCrash(LPEXCEPTION_POINTERS pException);
DP_API dp_result_t DP_APIX dpReportCrashEx(LPEXCEPTION_POINTERS pException, char *crshtxt);
#endif




DP_API dp_result_t DP_APIX dpReportAssertionFailure(int lineno, char *file, char *linetxt);

#include <assert.h>
#ifdef _WIN32
#define DP_ASSERT(exp) (void)((exp) || dpReportAssertionFailure(__LINE__, __FILE__, #exp), assert(exp), exit(1), 0)

#elif defined(UNIX)
#define DP_ASSERT(exp) (void)((exp) || dpReportAssertionFailure(__LINE__, __FILE__, __STRING(exp)), assert(exp), exit(1), 0)

#else 
#define DP_ASSERT(exp) assert(exp)
#endif

#endif

#ifdef dp_STATS












DP_API dp_result_t DP_APIX dpGetStats(
	dp_t *dp,
	int statkind,
	dp_stat_t *buf,
	size_t buflen);
#endif

#ifdef dp_ANET2









DP_API int DP_APIX dp_pack_session(dp_t *dp, dp_species_t defaultSessionType, const dp_session_t *p, char *buf);






DP_API int DP_APIX dp_unpack_session(dp_t *dp, const char *subkey, int subkeylen, const char *buf, size_t buflen, dp_session_t *p);

#endif

#endif 
#endif



#ifndef anet_ini_h
#define anet_ini_h
#define dpini_DEFAULT_FILENAME "dp.ini"




DP_API void DP_APIX dpini_SetFile(char *iniFileName);






DP_API const char * DP_APIX dpini_GetFile(void);








DP_API int DP_APIX dpini_findSection(
	char *sectionWant);	















DP_API const char * DP_APIX dpini_readParameter(
	char *paramWant,	
	int	  verbatim);	



























DP_API void DP_APIX dp_flushLog(void);








DP_API void DP_APIX dp_enableDebugPrint(int enable);

#define dp_LOG_FILE_NONE ((FILE *)0xffffffff)








DP_API FILE * DP_APIX dp_getLogFP(void);












DP_API void DP_APIX dp_setLogFP(FILE *_fp);








DP_API void DP_APIX dp_setLogFname(const char *fname);










DP_API int DP_APIX dp_dprintf(
	const char *	__format,	
	...);						

#if defined(__MWERKS__)
#pragma export off
#endif

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)
#define DPRINT(p) (dp_dprintf p )
#else
#define DPRINT(p)
#endif





#define dp_CKPT		DPRINT(("@" __FILE__ " %u:\n", __LINE__))








DP_API dp_result_t DP_APIX dpPkt2a(
	const char *pkt, 
	int pktlen, 
	char *buf, 
	int buflen);
	
#endif



#ifdef __cplusplus
}
#endif
