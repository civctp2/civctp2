




























#ifndef dpcompat_h
#define dpcompat_h


#ifndef DP_ANET
#include <dplay.h>


#define DP_Enumerate(cb,context)           DirectPlayEnumerate(cb,context)
#define DP_Create(pp,a,b,c)                DirectPlayCreate(pp,a,b)
#define DP_FAILED(h)                       FAILED(h)


#define DP_EnumSessions(p,a,b,c,d,e)       IDirectPlay_EnumSessions(p,a,b,c,d,e)
#define DP_GetCaps(p,a)                    IDirectPlay_GetCaps(p,a)
#define DP_Release(p)                      IDirectPlay_Release(p)
#define DP_QueryInterface(p,a,b)           IDirectPlay_QueryInterface(p,a,b)


#define DP2_AddPlayerToGroup(p,a,b)        IDirectPlay2_AddPlayerToGroup(p,a,b)
#define DP2_AddRef(p)                      IDirectPlay2_AddRef(p)                      
#define DP2_Close(p)                       IDirectPlay2_Close(p)                       
#define DP2_CreateGroup(p,a,b,c,d,e)       IDirectPlay2_CreateGroup(p,a,b,c,d,e)       
#define DP2_CreatePlayer(p,a,b,c,d,e,f)    IDirectPlay2_CreatePlayer(p,a,b,c,d,e,f)    
#define DP2_DeletePlayerFromGroup(p,a,b)   IDirectPlay2_DeletePlayerFromGroup(p,a,b)   
#define DP2_DestroyGroup(p,a)              IDirectPlay2_DestroyGroup(p,a)              
#define DP2_DestroyPlayer(p,a)             IDirectPlay2_DestroyPlayer(p,a)             
#define DP2_EnableNewPlayers(p,a)          IDirectPlay2_EnableNewPlayers(p,a)          
#define DP2_EnumGroupPlayers(p,a,b,c,d,e)  IDirectPlay2_EnumGroupPlayers(p,a,b,c,d,e)  
#define DP2_EnumGroups(p,a,b,c,d)          IDirectPlay2_EnumGroups(p,a,b,c,d)          
#define DP2_EnumPlayers(p,a,b,c,d)         IDirectPlay2_EnumPlayers(p,a,b,c,d)         
#define DP2_EnumSessions(p,a,b,c,d,e)      IDirectPlay2_EnumSessions(p,a,b,c,d,e)
#define DP2_GetCaps(p,a,b)                 IDirectPlay2_GetCaps(p,a,b)                 
#define DP2_GetGroupData(p,a,b,c,d)        IDirectPlay2_GetGroupData(p,a,b,c,d)        
#define DP2_GetGroupName(p,a,b,c)          IDirectPlay2_GetGroupName(p,a,b,c)          
#define DP2_GetMessageCount(p,a,b)         IDirectPlay2_GetMessageCount(p,a,b)         
#define DP2_GetPlayerAddress(p,a,b,c)      IDirectPlay2_GetPlayerAddress(p,a,b,c)      
#define DP2_GetPlayerCaps(p,a,b,c)         IDirectPlay2_GetPlayerCaps(p,a,b,c)         
#define DP2_GetPlayerData(p,a,b,c,d)       IDirectPlay2_GetPlayerData(p,a,b,c,d)       
#define DP2_GetPlayerName(p,a,b,c)         IDirectPlay2_GetPlayerName(p,a,b,c)         
#define DP2_GetSessionDesc(p,a,b)          IDirectPlay2_GetSessionDesc(p,a,b)          
#define DP2_Initialize(p,a)                IDirectPlay2_Initialize(p,a)                
#define DP2_Open(p,a,b)                    IDirectPlay2_Open(p,a,b)                    
#define DP2_QueryInterface(p,a,b)          IDirectPlay2_QueryInterface(p,a,b)          
#define DP2_Receive(p,a,b,c,d,e)           IDirectPlay2_Receive(p,a,b,c,d,e)           
#define DP2_Release(p)                     IDirectPlay2_Release(p)                     
#define DP2_Send(p,a,b,c,d,e)              IDirectPlay2_Send(p,a,b,c,d,e)              
#define DP2_SetGroupData(p,a,b,c,d)        IDirectPlay2_SetGroupData(p,a,b,c,d)        
#define DP2_SetGroupName(p,a,b,c)          IDirectPlay2_SetGroupName(p,a,b,c)          
#define DP2_SetPlayerData(p,a,b,c,d)       IDirectPlay2_SetPlayerData(p,a,b,c,d)       
#define DP2_SetPlayerName(p,a,b,c)         IDirectPlay2_SetPlayerName(p,a,b,c)         
#define DP2_SetSessionDesc(p,a,b)          IDirectPlay2_SetSessionDesc(p,a,b)          


typedef GUID     DP_TRANSPORT_GUID;	
typedef LPGUID LPDP_TRANSPORT_GUID;
typedef GUID     DP_SESSION_GUID;	
typedef LPGUID LPDP_SESSION_GUID;


#define DP_GUIDINSTANCE(pSdesc) ((pSdesc)->guidInstance)


#define DP_CASTMSG(type, ptr) (type (ptr))

typedef HRESULT DP_HRESULT;

#else


#include <anet.h>



#define DP_Enumerate(cb,context)           dpEnumTransports("dll", cb, context)
#define DP_Create(pp,a,b,c)                dpCreate(a,pp,b,c)
#define DP_FAILED(h)                       (h != dp_RES_OK)


#define DP_EnumSessions(p,a,b,c,d,e)       dpEnumSessions(p,a,NULL,b,c,d)	
#define DP_GetCaps(p,a)                    dpGetCaps(p,a,0)
#define DP_Receive(p,a,b,c,d,e)            dpReceive(p,a,b,c,d,e)
#define DP_Release(p)                      dpDestroy(p, 0)		




#define DP2_AddPlayerToGroup(p,a,b)        dpAddPlayerToGroup(p,a,b)        
#define DP2_AddRef(p)                      dpUnsupported(p)
#define DP2_Close(p)                       dpClose(p)                       
#define DP2_CreateGroup(p,pid,pname,dat,siz,flags) dpCreateGroup(p,pid,pname)


#define DP2_DeletePlayerFromGroup(p,a,b)   dpDeletePlayerFromGroup(p,a,b)   
#define DP2_DestroyGroup(p,a)              dpDestroyGroup(p,a)              
#define DP2_DestroyPlayer(p,a)             dpDestroyPlayer(p,a)             
#define DP2_EnableNewPlayers(p,a)          dpEnableNewPlayers(p,a)          
#define DP2_EnumGroupPlayers(p,id,sessguid,cb,cont,flags) dpEnumGroupPlayers(p,id,sessguid,cb,cont,1000)
#define DP2_EnumGroups(p,a,b,c,d)          dpEnumGroups(p,a,b,c,d)          
#define DP2_EnumPlayers(p,a,b,c,d)         dpEnumPlayers(p,a,b,c,d)         
#define DP2_EnumSessions(p,desc,timeout,cb,cont,flags)  dpEnumSessions(p,desc,NULL,timeout,cb,cont)
#define DP2_GetCaps(p,a,b)                 dpGetCaps(p,a,b)
#define DP2_GetGroupData(p,a,b,c,d)        dpGetGroupData(p,a,b,c,d)        
#define DP2_GetGroupName(p,a,b,c)          dpGetGroupName(p,a,b,c)          
#define DP2_GetMessageCount(p,a,b)         dpUnsupported(p,a,b)
#define DP2_GetPlayerAddress(p,a,b,c)      dpUnsupported(p,a,b,c)
#define DP2_GetPlayerCaps(p,a,b,c)         dpUnsupported(p,a,b,c)
#define DP2_GetPlayerData(p,a,b,c,d)       dpGetPlayerData(p,a,b,c,d)       
#define DP2_GetPlayerName(p,a,b,c)         dpGetPlayerName(p,a,b,c)         
#define DP2_GetSessionDesc(p,a,b)          dpGetSessionDesc(p,a,b)          
#define DP2_Initialize(p,a)                dpUnsupported(p,a)


#define DP2_Receive(p,a,b,c,d,e)           dpReceive(p,a,b,c,d,e)           
#define DP2_Release(p)                     dpDestroy(p, 0)
#define DP2_Send(p,a,b,c,d,e)              dpSend(p,a,b,c,d,e)              
#define DP2_SetGroupData(p,a,b,c,d)        dpSetGroupData(p,a,0,b,c,d)
#define DP2_SetGroupName(p,a,b,c)          dpSetGroupName(p,a,b,c)          
#define DP2_SetPlayerData(p,a,b,c,d)       dpSetPlayerData(p,a,0,b,c,d)
#define DP2_SetPlayerName(p,a,b,c)         dpSetPlayerName(p,a,b,c)
#define DP2_SetSessionDesc(p,a,b)          dpSetSessionDesc(p,a,b)


#define DP_OK                             dp_RES_OK
#define DPERR_BUFFERTOOLARGE              dp_RES_BADSIZE
#define DPERR_BUFFERTOOSMALL              dp_RES_BADSIZE
#define DPERR_INVALIDFLAGS                dp_RES_BAD
#define DPERR_INVALIDOBJECT               dp_RES_BUG
#define DPERR_INVALIDPARAMS               dp_RES_BAD
#define DPERR_NOMESSAGES                  dp_RES_EMPTY
#define DPERR_OUTOFMEMORY                 dp_RES_NOMEM
#define DPERR_SENDTOOBIG                  dp_RES_BADSIZE










typedef dp_t          IDirectPlay;
typedef IDirectPlay *LPDIRECTPLAY;
typedef dp_caps_t        DPCAPS;
typedef dp_session_t     DPSESSIONDESC;
typedef DPSESSIONDESC *LPDPSESSIONDESC;
typedef dpid_t           DPID, *LPDPID;


typedef dp_t           IDirectPlay2;
typedef IDirectPlay2   IDirectPlay2A;
typedef IDirectPlay2 *LPDIRECTPLAY2A;
typedef dp_session_t      DPSESSIONDESC2;
typedef DPSESSIONDESC2 *LPDPSESSIONDESC2;
typedef struct
{
	char lpszShortNameA[dp_PNAMELEN];
	
} DPNAME, FAR *LPDPNAME;
typedef const DPNAME FAR *LPCDPNAME;

#define DPSHORTNAMELEN dp_PNAMELEN


typedef dpEnumSessionsCallback_t LPDPENUMSESSIONSCALLBACK2;
typedef dpEnumPlayersCallback_t  LPDPENUMPLAYERSCALLBACK2;


typedef dp_transport_t    DP_TRANSPORT_GUID; 
typedef dp_transport_t *LPDP_TRANSPORT_GUID;
typedef dp_session_t      DP_SESSION_GUID; 
typedef dp_session_t   *LPDP_SESSION_GUID;

#define DP_GUIDINSTANCE(pSdesc) (*(pSdesc))

typedef dp_result_t DP_HRESULT;


#define DPENUMPLAYERS_ALL       (1<<0)
#define DPENUMPLAYERS_LOCAL     (1<<1)
#define DPENUMPLAYERS_REMOTE    (1<<2)
#define DPENUMPLAYERS_GROUP     (1<<3)
#define DPENUMPLAYERS_SESSION   (1<<4)


#define DPSESSION_NEWPLAYERSDISABLED 0
#define DPSESSION_MIGRATEHOST        0  
#define DPSESSION_NOMESSAGEID        0
#define DPSESSION_JOINDISABLED       0
#define DPSESSION_KEEPALIVE          0
#define DPSESSION_NODATAMESSAGES     0


#define DPRECEIVE_ALL                0	


#define DPID_SYSMSG     dp_ID_NAMESERVER
#define DPID_ALLPLAYERS dp_ID_BROADCAST	


typedef struct {
	dp_packetType_t dwType;
} DPMSG_GENERIC;





#define DP_CASTMSG(type, ptr) (type (((char *)(ptr)) + sizeof(dp_packetType_t)))







typedef dp_user_addPlayer_packet_t   DPMSG_CREATEPLAYERORGROUP;
#define DPSYS_CREATEPLAYERORGROUP   dp_USER_ADDPLAYER_PACKET_ID

typedef dp_user_delPlayer_packet_t   DPMSG_DESTROYPLAYERORGROUP;
#define DPSYS_DESTROYPLAYERORGROUP   dp_USER_DELPLAYER_PACKET_ID


typedef dp_addPlayerToGroup_packet_t DPMSG_ADDPLAYERTOGROUP;
#define DPSYS_ADDPLAYERTOGROUP       dp_USER_ADDPLAYERTOGROUP_PACKET_ID

typedef dp_delPlayerFromGroup_packet_t DPMSG_DELETEPLAYERFROMGROUP;
#define DPSYS_DELPLAYERFROMGROUP     dp_USER_DELPLAYERFROMGROUP_PACKET_ID

#endif

#endif
