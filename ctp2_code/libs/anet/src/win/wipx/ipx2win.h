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

/*************************************************************************************

                 Header File for IPX Protocol Driver Functions For Win32

                               M. Livesay  20-OCT-96

                              (c) 1996 By Activision

**************************************************************************************/

#ifndef IPX2WIN_H
#define IPX2WIN_H 1

#include <windows.h> 
#include <windowsx.h>
#include <wsipx.h>   
#include "commapi.h"			// For caller-friendly error codes

#define IPX2_MAX_USERDATALEN 512

// Peer literals
#define MAX_PEERS          100

typedef int           IPXHANDLE;  // Peer handle
typedef SOCKADDR_IPX  IPXPEER;    // Winsock's address of peer

// Predefined Peer Handles
#define ipx2_HDL_NONE      -1
#define ipx2_HDL_BROADCAST	0	    /* The first entry in IPXINSTANCE.peers is for broadcast  */
#define ipx2_HDL_ME			    1	    /* The 2nd entry is for my own host address               */


typedef struct _IPXINSTANCE       // IPX Instance Struct  (Gee, in C++, this would be an object)
{
  SOCKET        socket;           // WinSock socket

  int           nexthandle;       // next available peer handle (next unused peers[] element)
  IPXPEER       peers[MAX_PEERS]; // Peer info (indexed with peer handle)
} 
IPXINSTANCE;


// substitution literals for DOS IPX types
#define ipx2_hdl_t    IPXHANDLE
#define ipx2_result_t int
#define ipx2_t        IPXINSTANCE


// IPXWIN Function Result Codes.  same as interface our caller inherits from...
#define ipx2_RES_OK		  comm_STATUS_OK
#define ipx2_RES_FULL	  comm_STATUS_FULL
#define ipx2_RES_EMPTY	  comm_STATUS_EMPTY
#define ipx2_RES_BAD	  comm_STATUS_BAD


// IPXWIN Functions

IPXINSTANCE * IPXWIN_Create(int socket, int *status);

void          IPXWIN_Destroy(IPXINSTANCE *ipx);

IPXHANDLE     IPXWIN_Address2Handle(IPXINSTANCE *ipx, IPXPEER *addr, int insert);

int           IPXWIN_Handle2Address(IPXINSTANCE *ipx, IPXHANDLE handle, IPXPEER *addr);

int           IPXWIN_PutPacket(IPXINSTANCE *ipx, void *bufptr, ULONG len, IPXHANDLE hdest);

int           IPXWIN_GetPacket(IPXINSTANCE *ipx, void *bufptr, ULONG *len, IPXHANDLE *hsrc, IPXPEER *srcaddr);


#define ipx2_create(socket, stat)               IPXWIN_Create(socket, stat)

#define ipx2_destroy(ipx)                       IPXWIN_Destroy(ipx)

#define ipx2_adr2hdl(ipx, adr, insert)          IPXWIN_Address2Handle(ipx, adr, insert)

#define ipx2_hdl2adr(ipx, hdl, adr)             IPXWIN_Handle2Address(ipx, hdl, adr)

#define ipx2_put(ipx, buf, len, dest)           IPXWIN_PutPacket(ipx, buf, len, dest)

#define ipx2_get(ipx, buf, len, srcHdl, srcAdr) IPXWIN_GetPacket(ipx, buf, len, srcHdl, srcAdr)


#endif
