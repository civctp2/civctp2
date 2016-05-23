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

/*
 * mstcp.c
 $Log: trumpudp.c $
 Revision 1.2  1997/08/22 18:21:39  anitalee
 changes made by Danny Swarzman.
 Revision 1.1 1997/08/05 dswarzman
 Fixed system includes.
*/

#include 	<stdlib.h>
#include	<string.h>
#include 	<assert.h>

#include	"dpmem.h"
#include 	"ddprint.h"
#include	"mstcp.h"
#include 	"mstcplow.h"


/******************************************************************************
 Create a TCP driver.

 Note that only one of these may exist at a time per port number, since
 multiple ones would compete for the same source of rx packets.  Choosing
 TCP_SOCKET_ANY for the port is guaranteed to give an unused port.
******************************************************************************/
TCPINSTANCE *			/* TCP driver, or NULL for failure */
TCPWIN_Create(
	u_short maxHandles,	/* (input) Maximum number of allowed handles */
	u_short* port,		/* (modified) Port (socket) number, or TCP_SOCKET_ANY */
	int *status)		/* (output) status */
{
	TCPINSTANCE* pTcp;
	TCPPEER peer;

	DPRINT(("TCPWIN_Create() Called..\n"));

	assert(status != NULL);

	/* allocate and initialize an instance structure */
	pTcp = (TCPINSTANCE *) dp_MALLOC(sizeof(TCPINSTANCE));
	if (pTcp == NULL) {
		*status = comm_STATUS_MEMORY;
		return NULL;
	}
	memset(pTcp, 0, sizeof(TCPINSTANCE));
	pTcp->handles = dcstCreate(sizeof(TCPPEER),
						maxHandles, maxHandles/3);
	if (pTcp->handles == NULL) {
		dp_FREE(pTcp);
		*status = comm_STATUS_MEMORY;
		return NULL;
	}
	pTcp->myHandle = TCP_HDL_NONE;
	pTcp->broadcastHandle = TCP_HDL_NONE;

	/* Kluge to force broadcastHandle to have value 0 */
	pTcp->broadcastHandle = dcstReplace(pTcp->handles,
							pTcp->broadcastHandle, &peer);

	*status = comm_STATUS_OK;
	TCPLow_Create(pTcp,port,&peer,status);
	if (*status != TCP_RES_OK)
		pTcp = NULL;
	return(pTcp);
}

/*****************************************************************************
 Destroy a TCP driver.
*****************************************************************************/
void
TCPWIN_Destroy(
	TCPINSTANCE *pTcp)	/* (modified) Current TCP instance */
{
	assert(pTcp != NULL);
	DPRINT(("TCPWIN_Destroy() Called..\n"));
	TCPLow_Destroy(pTcp);
	dp_FREE(pTcp);
}

/*****************************************************************************
 Given a TCP peer address, return a handle that can be used to send a
 message to that address.

 Handles are integers starting at 0.  If a handle has been assigned to that
 address, return it.  If no handle has been assigned to the address, fewer
 than the maximum number of handles have been assigned, and insert is non-zero,
 assign one; otherwise return TCP_HDL_NONE.

 WARNING: Potential resource lock.  Original TCP driver had this comment
 about the handle allocation scheme:
	There is no way provided to close handles, and no particular
	need, since no one should be assigning the maximim number,
	and they only take up 16 bytes each.
 The maximum number of handles is determined by TCP_MAX_PEERS, in wstcp.h

 Returns TCP_HDL_NONE on failure, and a valid TCPHANDLE otherwise.
*****************************************************************************/
TCPHANDLE				/* Corresponding handle */
TCPWIN_Address2Handle(
	TCPINSTANCE *tcp,	/* (modified) Current TCP instance */
	TCPPEER *pAddr,		/* (input) Address to search for */
	BOOL insert)		/* (input) Whether or not to assign a new handle */
{
	TCPHANDLE h;

	assert(tcp != NULL);
	assert(pAddr != NULL);
	assert((insert == TRUE) || (insert == FALSE));

	/* Search handle set and return if found */
	h = dcstFindKey(tcp->handles, pAddr);
	if(insert && (h == TCP_HDL_NONE)) {
		/* handle not found: add it to set, if desired */
		h = dcstAdd(tcp->handles, pAddr);
	}

	return h;
}

/*****************************************************************************
 Given a handle, return the corresponding TCPPEER address.

 Returns TCP_RES_OK on success, TCP_RES_EMPTY on failure.
*****************************************************************************/
int						/* status */
TCPWIN_Handle2Address(
	TCPINSTANCE *tcp,	/* (input) Current TCP instance */
	TCPHANDLE handle,	/* (input) Handle to translate */
	TCPPEER *pAddr)		/* (output) Corresponding address */
{
	TCPPEER* p;

	assert(tcp != NULL);
	assert(pAddr != NULL);
	assert(handle != TCP_HDL_NONE);

	p = ((TCPPEER*)dcstFindValue(tcp->handles, handle));
	if(NULL == p)
		return(TCP_RES_EMPTY);
	*pAddr = *p;
	return(TCP_RES_OK);
}

/*****************************************************************************
 Send a packet to the given destination.

 Copies len bytes from buf into internal packet queue.
 Must have previously gotten handle for destination by calling TCP_adr2hdl.
*****************************************************************************/
int						/* status */
TCPWIN_PutPacket(
	TCPINSTANCE *tcp,	/* (input) current TCP instance */
	void *bufptr,		/* (input) packet to send */
	ULONG len,			/* (input) length in bytes of packet to send */
	TCPHANDLE hdest)	/* (input) destination for packet */
{
	TCPPEER* pPeer;

	assert(tcp != NULL);
	assert(bufptr != NULL);
	assert(len < TCP_MAX_USERDATALEN);
	assert(hdest != TCP_HDL_NONE);

	/* do sanity checks */
	if ((len < 1) || (TCP_HDL_NONE == hdest) ||
				(len > TCP_MAX_USERDATALEN) || (NULL == bufptr))
		return TCP_RES_BAD;

    /* get ptr to peer address from peer handle */
	pPeer = (TCPPEER*)dcstFindValue(tcp->handles, hdest);
	if(NULL == pPeer)
		return TCP_RES_BAD;	/* Sending to unused handle */

	return TCPLow_PutPacket(tcp,pPeer,bufptr,len);
}

/*****************************************************************************
 Receive a packet.
 Caller must put size of buffer in len before calling.
 Packets larger than buffer will be silently ignored.
 On failure, returns
  TCP_RES_BAD if *len looked bogus, or buf or srcHdl were NULL,
         or packet was too big to fit in buffer, or 0 size.
  TCP_RES_EMPTY if no good packet was availible.
 On success,
  returns TCP_RES_OK,
  sets *len to the packet length,
  sets *hsrc to the handle of the sender (or TCP_HDL_NONE if one
    hasn't been assigned yet with TCPWIN_Address2Handle), and
  sets *srcAdr to the netware address of the sender if srcAdr is not NULL.

 To reply if source was TCP_HDL_NONE, you must first call
 TCPWIN_Address2Handle to turn the source's address into a handle.
 (TCP_get could do this for you, but then the peer table might fill up
 with all sorts of garbage addresses from hosts who sent you junk mail.)
*****************************************************************************/
int						/* status */
TCPWIN_GetPacket(
	TCPINSTANCE *tcp,	/* (input) Current TCP instance */
	void *bufptr,		/* (output) Contents of packet */
	ULONG *pLen,		/* (modified) size of bufptr/number of bytes in packet*/
	TCPHANDLE *hsrc,	/* (output) Handle of sender */
	TCPPEER *srcaddr)	/* (output) Address of sender */
{
	TCPPEER addr;
	int status;

	/* do sanity checks */
	assert(tcp != NULL);
	assert(bufptr != NULL);
	assert(pLen != NULL);
	assert(*pLen < 10*TCP_MAX_USERDATALEN);
	assert(hsrc != NULL);
	if ((*pLen < 1) || (*pLen > 10*TCP_MAX_USERDATALEN) ||
				(bufptr == NULL) || (hsrc == NULL)) {
		DPRINT(("TCPWIN_GetPacket(): Sanity Check Failed!\n"));
		return(TCP_RES_BAD);
	}

	/* read packet */
	status = TCPLow_GetPacket(tcp,bufptr,pLen,&addr);
	if (status != TCP_RES_OK)
		return status;
	*hsrc = TCPWIN_Address2Handle(tcp, &addr, FALSE);
	if (srcaddr != NULL)
		*srcaddr = addr;

	return(TCP_RES_OK);
}
