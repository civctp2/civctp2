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
 Compatibility layer - use these macros if you want to be able to
 compile for both ActiveNet and Microsoft's library.

 $Log: dpcompat.h $
 Revision 1.7  1997/02/27 02:33:33  dkegel
 Replaced private dp.h with public anet.h.
 Revision 1.6  1997/02/18 00:18:50  dkegel
 Define a few more error codes.
 Revision 1.5  1997/02/05 02:36:08  dkegel
 Added macro to conver msg buffer pointer to particular type;
 all of our message structs lack the 'type' field, so need to add 2.
 Revision 1.4  1997/02/05 02:22:34  dkegel
 1. DP2_GetCaps now supported.
 2. The most vital system messages are now supported enough to compile,
 but they probably won't run right yet.
 Revision 1.3  1997/02/04 03:37:08  dkegel
 Rather more complete - still not quite done tweaking it to support
 a real game yet.
 Revision 1.2  1997/02/03 20:28:31  dkegel
 Added IDirectPlay2 etc.
 Revision 1.1  1997/01/30 02:45:10  dkegel
 Initial revision
--------------------------------------------------------------------------*/
/*#define DP_ANET			/* Define DP_ANET to use Activenet. */

#ifndef dpcompat_h
#define dpcompat_h

/* If (not activenet): */
#ifndef DP_ANET
#include <dplay.h>

/* DirectPlay functions */
#define DP_Enumerate(cb,context)           DirectPlayEnumerate(cb,context)
#define DP_Create(pp,a,b,c)                DirectPlayCreate(pp,a,b)
#define DP_FAILED(h)                       FAILED(h)

/* IDirectPlay methods */
#define DP_EnumSessions(p,a,b,c,d,e)       IDirectPlay_EnumSessions(p,a,b,c,d,e)
#define DP_GetCaps(p,a)                    IDirectPlay_GetCaps(p,a)
#define DP_Release(p)                      IDirectPlay_Release(p)
#define DP_QueryInterface(p,a,b)           IDirectPlay_QueryInterface(p,a,b)

/* IDirectPlay2 methods */
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

/* New structures needed to be able to compile for either library. */
typedef GUID     DP_TRANSPORT_GUID;	/* Use for GUID's that identify service providers. */
typedef LPGUID LPDP_TRANSPORT_GUID;
typedef GUID     DP_SESSION_GUID;	/* Use for GUID's that identify game sessions */
typedef LPGUID LPDP_SESSION_GUID;

/* Use DP_GUIDINSTANCE(pSdesc) in place of pSdesc->guidInstance */
#define DP_GUIDINSTANCE(pSdesc) ((pSdesc)->guidInstance)

/* Do this to cast the buffer pointer to a system message pointer. */
#define DP_CASTMSG(type, ptr) (type (ptr))

typedef HRESULT DP_HRESULT;

#else
/* If (activenet): */

#include <anet.h>

/* DirectPlay functions */
/* Note: DP_Create differs between ActiveNet and Microsoft. */
#define DP_Enumerate(cb,context)           dpEnumTransports("dll", cb, context)
#define DP_Create(pp,a,b,c)                dpCreate(a,pp,b,c)
#define DP_FAILED(h)                       (h != dp_RES_OK)

/* IDirectPlay methods */
#define DP_EnumSessions(p,a,b,c,d,e)       dpEnumSessions(p,a,NULL,b,c,d)	/* ignore flags for now... fixx */
#define DP_GetCaps(p,a)                    dpGetCaps(p,a,0)
#define DP_Receive(p,a,b,c,d,e)            dpReceive(p,a,b,c,d,e)
#define DP_Release(p)                      dpDestroy(p, 0)		/* Should check ref cnt */
// #define DP_QueryInterface(p,a,b)           ((dp_t **)b = *(dp_t *)p; 1)	/* return a non-FAILED value */
//QueryInterface reallocates, too different for a mere macro.

/* IDirectPlay2 methods */
#define DP2_AddPlayerToGroup(p,a,b)        dpAddPlayerToGroup(p,a,b)        
#define DP2_AddRef(p)                      dpUnsupported(p)
#define DP2_Close(p)                       dpClose(p)                       
#define DP2_CreateGroup(p,pid,pname,dat,siz,flags) dpCreateGroup(p,pid,pname)
//#define DP2_CreatePlayer(p,pid,pn,hev,len,buf,flags) dpCreatePlayer(p,cb,cont,pn)
//CreatePlayer uses a callback, too different for a mere macro.
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
//#define DP2_Open(p,sess,flags)             ((sess->flags |= b), dpOpen(p,a,b)
//Open uses a callback, too different for a mere macro.
#define DP2_Receive(p,a,b,c,d,e)           dpReceive(p,a,b,c,d,e)           
#define DP2_Release(p)                     dpDestroy(p, 0)
#define DP2_Send(p,a,b,c,d,e)              dpSend(p,a,b,c,d,e)              
#define DP2_SetGroupData(p,a,b,c,d)        dpSetGroupData(p,a,0,b,c,d)
#define DP2_SetGroupName(p,a,b,c)          dpSetGroupName(p,a,b,c)          
#define DP2_SetPlayerData(p,a,b,c,d)       dpSetPlayerData(p,a,0,b,c,d)
#define DP2_SetPlayerName(p,a,b,c)         dpSetPlayerName(p,a,b,c)
#define DP2_SetSessionDesc(p,a,b)          dpSetSessionDesc(p,a,b)

/* ActiveNet error codes don't map easily.  Here's an approximate mapping. */
#define DP_OK                             dp_RES_OK
#define DPERR_BUFFERTOOLARGE              dp_RES_BADSIZE
#define DPERR_BUFFERTOOSMALL              dp_RES_BADSIZE
#define DPERR_INVALIDFLAGS                dp_RES_BAD
#define DPERR_INVALIDOBJECT               dp_RES_BUG
#define DPERR_INVALIDPARAMS               dp_RES_BAD
#define DPERR_NOMESSAGES                  dp_RES_EMPTY
#define DPERR_OUTOFMEMORY                 dp_RES_NOMEM
#define DPERR_SENDTOOBIG                  dp_RES_BADSIZE

/* Why isn't this in windows.h? */
/*
#ifndef __LPGUID_DEFINED__
#define __LPGUID_DEFINED__
typedef GUID *LPGUID;
#endif
*/

/* DirectPlay structures. */
typedef dp_t          IDirectPlay;
typedef IDirectPlay *LPDIRECTPLAY;
typedef dp_caps_t        DPCAPS;
typedef dp_session_t     DPSESSIONDESC;
typedef DPSESSIONDESC *LPDPSESSIONDESC;
typedef dpid_t           DPID, *LPDPID;

/* IDirectPlay2 structures. */
typedef dp_t           IDirectPlay2;
typedef IDirectPlay2   IDirectPlay2A;
typedef IDirectPlay2 *LPDIRECTPLAY2A;
typedef dp_session_t      DPSESSIONDESC2;
typedef DPSESSIONDESC2 *LPDPSESSIONDESC2;
typedef struct
{
	char lpszShortNameA[dp_PNAMELEN];
	// Long name not available yet.
} DPNAME, FAR *LPDPNAME;
typedef const DPNAME FAR *LPCDPNAME;

#define DPSHORTNAMELEN dp_PNAMELEN

/* IDirectPlay2 callback types. */
typedef dpEnumSessionsCallback_t LPDPENUMSESSIONSCALLBACK2;
typedef dpEnumPlayersCallback_t  LPDPENUMPLAYERSCALLBACK2;

/* New structures needed to be able to compile for either library. */
typedef dp_transport_t    DP_TRANSPORT_GUID; /* Use for GUID's that id transports. */
typedef dp_transport_t *LPDP_TRANSPORT_GUID;
typedef dp_session_t      DP_SESSION_GUID; /* Use for GUID's that identify game sessions. */
typedef dp_session_t   *LPDP_SESSION_GUID;
/* Use DP_GUIDINSTANCE(pSdesc) in place of pSdesc->guidInstance */
#define DP_GUIDINSTANCE(pSdesc) (*(pSdesc))

typedef dp_result_t DP_HRESULT;

/* Flag parameter to dpEnumPlayers -- all ignored right now */
#define DPENUMPLAYERS_ALL       (1<<0)
#define DPENUMPLAYERS_LOCAL     (1<<1)
#define DPENUMPLAYERS_REMOTE    (1<<2)
#define DPENUMPLAYERS_GROUP     (1<<3)
#define DPENUMPLAYERS_SESSION   (1<<4)

/* Flag field of dp_session_t */
#define DPSESSION_NEWPLAYERSDISABLED 0
#define DPSESSION_MIGRATEHOST        0  /* inverse of dp_SESSION_FLAGS_ENABLE_NEWPLAYERS */
#define DPSESSION_NOMESSAGEID        0
#define DPSESSION_JOINDISABLED       0
#define DPSESSION_KEEPALIVE          0
#define DPSESSION_NODATAMESSAGES     0

/* Flag field of DP2_RECEIVE */
#define DPRECEIVE_ALL                0	/* only supported value */

/* Special dpid_t's */
#define DPID_SYSMSG     dp_ID_NAMESERVER
#define DPID_ALLPLAYERS dp_ID_BROADCAST	

/* System messages */
typedef struct {
	dp_packetType_t dwType;
} DPMSG_GENERIC;

/* Do this to cast the buffer pointer to a system message pointer.
 * Needed at the moment because packet type is included in their structs,
 * but not in ours.
 */
#define DP_CASTMSG(type, ptr) (type (((char *)(ptr)) + sizeof(dp_packetType_t)))

/* Add, destroy player.  MS uses one packet for both players and groups,
 * and distinguishes with a field.  If needed, we will later create an accessor macro for
 * that field.  We don't support player data in those messages, though,
 * and if they want to be notified of group destruction, they need to
 * check another packet type, dp_USER_DELGROUP_PACKET_ID.
 */
typedef dp_user_addPlayer_packet_t   DPMSG_CREATEPLAYERORGROUP;
#define DPSYS_CREATEPLAYERORGROUP   dp_USER_ADDPLAYER_PACKET_ID

typedef dp_user_delPlayer_packet_t   DPMSG_DESTROYPLAYERORGROUP;
#define DPSYS_DESTROYPLAYERORGROUP   dp_USER_DELPLAYER_PACKET_ID

/* Add, delete player to/from group.  These track closely. */
typedef dp_addPlayerToGroup_packet_t DPMSG_ADDPLAYERTOGROUP;
#define DPSYS_ADDPLAYERTOGROUP       dp_USER_ADDPLAYERTOGROUP_PACKET_ID

typedef dp_delPlayerFromGroup_packet_t DPMSG_DELETEPLAYERFROMGROUP;
#define DPSYS_DELPLAYERFROMGROUP     dp_USER_DELPLAYERFROMGROUP_PACKET_ID

#endif

#endif
