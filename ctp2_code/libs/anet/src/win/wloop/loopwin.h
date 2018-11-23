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

/*****************************************************************************
 Header File for Loopback Testing Protocol Driver Functions For Win32
 Marshall Robin
 (c) 1996 By Activision
******************************************************************************/

#ifndef LOOPWIN_H
#define LOOPWIN_H 1

#include <windows.h>
#include <windowsx.h>
#include "commapi.h"			// For caller-friendly error codes

#define LOOP_MAX_USERDATALEN	512

// Peer literals
#define MAX_PEERS			100

typedef int					LOOPHANDLE;  // Peer handle
typedef DWORD				LOOPADDR;    // Winsock's address of peer
typedef struct {
	LOOPADDR	addr;
	HANDLE		mutex;
	HANDLE		filemap;
	void *		pbuf;
} LOOPPEER;

#define	INVALID_LOOPADDR			0	// Invalid as a return value
#define	BROADCAST_LOOPADDR			0

// Predefined Peer Handles
#define loop_HDL_NONE				-1
#define loop_HDL_BROADCAST			0	    /* The first entry in LOOPINSTANCE.peers is for broadcast  */
#define loop_HDL_ME					1	    /* The 2nd entry is for my own host address               */

typedef struct _LOOPINSTANCE       // Loopback Instance Struct  (Gee, in C++, this would be an object)
{
  LOOPADDR		addr;			  // My "address"

  int           nexthandle;       // next available peer handle (next unused peers[] element)
  LOOPPEER		peers[MAX_PEERS]; // Peer info (indexed with peer handle)
}
LOOPINSTANCE;

// substitution literals for DOS types
#define loop_hdl_t		LOOPHANDLE
#define loop_result_t	int
#define loop_t			LOOPINSTANCE

// LOOPWIN Function Result Codes.  same as interface our caller inherits from...
#define loop_RES_OK	comm_STATUS_OK
#define loop_RES_FULL	comm_STATUS_FULL
#define loop_RES_EMPTY	comm_STATUS_EMPTY
#define loop_RES_BAD	comm_STATUS_BAD

// Other result codes

#define	LOOP_ERROR		-1

// LOOPWIN Functions

LOOPINSTANCE *	LOOPWIN_Create(int *status, long sessionId);

void			LOOPWIN_Destroy(LOOPINSTANCE *loop);

LOOPHANDLE		LOOPWIN_Address2Handle(LOOPINSTANCE *loop, LOOPADDR addr, int insert);

int				LOOPWIN_Handle2Address(LOOPINSTANCE *loop, LOOPHANDLE handle, LOOPADDR *addr);

int				LOOPWIN_PutPacket(LOOPINSTANCE *loop, void *bufptr, ULONG len, LOOPHANDLE hdest);

int				LOOPWIN_GetPacket(LOOPINSTANCE *loop, void *bufptr, ULONG *len, LOOPHANDLE *hsrc, LOOPADDR *srcaddr);

#define loop_create(stat, sessionId)             LOOPWIN_Create(stat, sessionId)

#define loop_destroy(loop)                       LOOPWIN_Destroy(loop)

#define loop_adr2hdl(loop, adr, insert)          LOOPWIN_Address2Handle(loop, adr, insert)

#define loop_hdl2adr(loop, hdl, adr)             LOOPWIN_Handle2Address(loop, hdl, adr)

#define loop_put(loop, buf, len, dest)           LOOPWIN_PutPacket(loop, buf, len, dest)

#define loop_get(loop, buf, len, srcHdl, srcAdr) LOOPWIN_GetPacket(loop, buf, len, srcHdl, srcAdr)

#endif
