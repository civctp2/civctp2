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
 Multiport Winsock TCP Protocol Driver Functions For Win32

 Copyright (c) 1997 By Activision Inc.  All rights reserved.
 Copyright protection claimed includes all forms and matters of
 copyrightable material and information now allowed by statutory or
 judicial law or hereinafter granted, including without limitation,
 material generated from the software programs which are displayed
 on the screen such as icons, screen display looks, etc.

 (Original module by M. Livesay)
  $Log: wstcp.h $
  Revision 1.5  1997/09/19 21:33:23  lbennett
  Write actual port number to commInitReq.
  Revision 1.4  1997/09/02 18:26:25  dkegel
  Use a port number one higher than uudp, so we can coexist
  at least for the moment.
  Revision 1.3  1997/08/27 23:55:58  lbennett
  Removed unused items.
  Revision 1.2  1997/08/27 23:33:20  lbennett
  Converted to use dictset for handles, reduced size of TCPPEER.
  Revision 1.1  1997/08/22 22:35:09  lbennett
  Initial revision

*****************************************************************************/

#ifndef _WSTCP_H_
#define _WSTCP_H_ 1

#include <windows.h>
#include <winsock.h>

#include "dictset.h"

/**
* Constants
*/

/* Magic values for port numbers */
#define TCP_SOCKET_ANY		(0)		/* Let OS choose the port number */
#define TCP_SOCKET_DEFAULT	(0x52A1+commapi_NET_PORTOFFSET+1) /* +1 for uudps */

/* Longest allowed packet */
#define TCP_MAX_USERDATALEN	520		/* This must match dpio_MAXLEN_GATHER! */

/* Predefined TCPHANDLEs */
#define TCP_HDL_NONE	dcst_INVALID_KEY

/* Result Codes */
#include "commerr.h"
#define TCP_RES_OK				comm_STATUS_OK
#define TCP_RES_FULL			comm_STATUS_FULL
#define TCP_RES_EMPTY			comm_STATUS_EMPTY
#define TCP_RES_BAD				comm_STATUS_BAD
#define TCP_RES_NETWORK			comm_STATUS_NETWORK_NOT_PRESENT
#define TCP_RES_UNIMPLEMENTED	comm_STATUS_UNIMPLEMENTED
#define TCP_RES_DUAL_ADDRESS	comm_STATUS_DUAL_ADDRESS

/* Secondary address status */
#define TCP_HDL2_NONE     0x01
#define TCP_HDL2_LIVE     0x02

/**
* Types
*/

/* Address of a peer (used to specify destination/source info) */
typedef struct TCPPEER_s {
	u_long addr;	/* in network byte order */
	u_short port;	/* in network byte order */
} TCPPEER;

/* Second address of a peer (used to specify destination/source info) */
typedef struct TCPEER2_s {
  u_char status; /* status of the second address */
  TCPPEER peer;  /* address */
} TCPPEER2;

/* Handle for a peer (used to identify a node in the package) */
typedef dcst_key_t TCPHANDLE;

/* TCP Instance Struct (like C++ object's data) */
typedef struct tcpInstance_s
{
  SOCKET socket;				/* WinSock socket */
  dcst_t handles;				/* TCPHANDLE<->TCPPEER dictionary set */
  dcst_t secondary;     /* TCPHANDLE<->TCPPEER secondary disctionary set */
  TCPHANDLE myHandle;			/* Handle for this node's address */
  TCPHANDLE broadcastHandle;	/* Handle for broadcast address */
}
TCPINSTANCE;

/**
* Methods
*/

/* Constructor: Start a run of TCPWIN, communicating using
   the specified socket.  If socket is TCP_SOCKET_ANY, have
   OS choose an unused socket.  TCPWIN will support a maximum
   of maxHandles connections at a time. */
TCPINSTANCE * TCPWIN_Create(unsigned short maxHandles, unsigned short* socket, int *status);

/* Destructor: End a run of TCPWIN. */
void TCPWIN_Destroy(TCPINSTANCE *TCP);

/*
 * Converts a PAIR of TCPPEERs into a corresponding TCPHANDLE.
 *
 * The insert flag is used to indicate that if there is no handle
 * found for the supplied address(es) then one can be created.
 *
 * The live flag is used to indicate that the second address should
 * have packets sent to it until it has been determined which address
 * to use.  This is determined by the first packet received from
 * either address.
 *
 * If BOTH addresses are supplied then the following occurs:
 *   If either the first or second address is matched in the primary handle
 *   list then that handle is returned.  If neither existed in the primaries
 *   then a new pair of handles is created (if the insert flag is set to TRUE)
 *   and the second address is marked as LIVE (if the live flag is set to TRUE).
 *   In the event that one of the address does match then the other supplied
 *   address is tested agains that other stored address (if there is one at all),
 *   and the stored address is replaced.  If the LIVE flag is set then this
 *   second address is marked as LIVE.
 *
 * If a only the first address is supplied then the following occurs:
 *   If the address appears in the primary handle list then a check is done
 *   to ensure that if the handle appears in the secondary handle list then
 *   it is marked as NON-LIVE.  If the address appears in the secondary
 *   handle list AND the secondary address is marked as LIVE then the
 *   primary and secondary addresses are EXCHANGED and the secondary is
 *   marked as NON-LIVE.
 */
TCPHANDLE TCPWIN_Address2Handle(TCPINSTANCE *TCP, TCPPEER *addr, TCPPEER *addr2, BOOL insert, BOOL live);

/* Convert a TCPHANDLE to its corresponding TCPPEER.  Return
   a status code to indicate success or failure.
   If a second address exists then it is returned also via addr2
   A second address is only seached for if the primary existed
*/
int TCPWIN_Handle2Address(TCPINSTANCE *TCP, TCPHANDLE handle, TCPPEER *addr, TCPPEER2 *addr2);

/*
 * Send a packet to a node identified by a TCPHANDLE.
 *
 * TCPHANDLE may turn out to have more than one address associated with it
 * In the event that more than one address is associated with the handle, the
 * second address will only be sent to if it is marked as LIVE.
 *
 */
int TCPWIN_PutPacket(TCPINSTANCE *TCP, void *bufptr, ULONG len, TCPHANDLE hdest);

/*
 * Receive a packet.  The packet will be copied into bufptr, and
 * the source handle will be copied into phSrc.  If pSrcAddr is
 * not NULL, the source address will be copied into it.
 *
 * By virtue of the fact that incoming packet's source addresses
 * are translated into player handles, receiving packets may cause
 * changes to the addresses associated with a handle.  In the case
 * where a handle has two addresses and the second address is LIVE,
 * after resolving that handle from one of the addresses only one
 * of those addresses will remain LIVE.  The next time a packet is
 * sent to that handle it will sent only to a single address.
 *
 * It is still valid to receive packets from the other address
 * but since it is not marked as LIVE it will not causes a change
 * in the address to associate with a handle for sending packets.
 *
 */
int TCPWIN_GetPacket(TCPINSTANCE *TCP, void *bufptr, ULONG *len, TCPHANDLE *phSrc, TCPPEER *pSrcAddr);

#endif
