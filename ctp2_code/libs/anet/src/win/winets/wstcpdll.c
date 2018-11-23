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
 Comm API Shims for Multiport Winsock TCP Protocol
 Copyright (c) 1997 By Activision Inc.  All rights reserved.

 Convention:
	Free-form addresses are stored in a six-byte buffer.  Bytes 0-3 are
	the IP address in network order (e.g., an address of 1.2.3.4 would be
	stored with a value of 1 in byte 0, and a value of 4 in byte 3).
	Bytes 4 and 5 are the port number in network order.

 (Original Winsock module by M. Livesay)
 $Log: wstcpdll.c $
 Revision 1.13  1997/10/01 00:48:17  anitalee
 changed Internet-S to Internet
 Revision 1.12  1997/09/19 21:33:25  lbennett
 Write actual port number to commInitReq.
 Revision 1.11  1997/09/04 22:54:50  lbennett
 Implemented commSayBye.
 Revision 1.10  1997/09/04 01:55:59  lbennett
 Fixed ping-response to empty packets.
 Revision 1.9  1997/09/02 18:29:07  dkegel
 1. commScanAddr now picks the standard default port number if
 you don't specify one; this is right default because we use this
 normally to specify server addresses.
 2. commInit now chooses a random port number by default; this is
 the right default for a client.
 Revision 1.8  1997/09/01 05:10:03  lbennett
 Added ping packet handling.
 Revision 1.7  1997/09/01 03:57:11  lbennett
 For received packet, save address only if from an unknown handle.
 Revision 1.6  1997/08/31 03:04:37  lbennett
 Added support for receive's adr field.
 Revision 1.5  1997/08/29 01:19:37  lbennett
 Removed MSVC-specific pragmas from non-WIN32 builds.
 Revision 1.4  1997/08/27 23:54:01  lbennett
 Changed comments.
 Revision 1.3  1997/08/27 23:34:15  lbennett
 Converted to use dictset for handles, changed TCPPEER.
 Revision 1.1  1997/08/22 22:35:24  lbennett
 Initial revision
*****************************************************************************/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <assert.h>
#include <stdio.h>
#include <windows.h>

#ifdef TCP_LAN
#include "adialup.h"
#endif

#include "commapi.h"
#include "wstcp.h"

/* fancy dance to allow including dp.h */
#define DEBUG_MODULE 0
#define dp_dprintf __bogus_dprintf

#include "dp.h"		/* for the ping packet stuff; includes ddprint.h */

/* now the real dprint definitions */
#undef dp_dprintf
#undef DPRINT
#undef DEBUG_MODULE
#define DEBUG_MODULE 1		/* Set to 1 to enable debug prints */
#include "ddprint.h"

/* For snoopy logging of packets. */
#include "logpkt.h"

#ifdef LOGPKT_ENABLED
FILE *logpkt_fp = NULL;
#endif

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows linked code has six of these ) */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/**
* Constants
*/

/*#define TCP_LAN  Makefile defines this for LAN, leaves alone for WAN */

/* The maximum number of nodes we can communicate with */
#define TCP_MAX_PEERS	100

/**
* Global Data
*/

static TCPINSTANCE *pTcp;

/**
* Methods
*/

/*****************************************************************************
 Convert a TCPHANDLE to the corresponding playerHdl_t.
*****************************************************************************/
static playerHdl_t		/* corresponding player handle */
tcp2commHdl(
	TCPINSTANCE* tcp,	/* (input) TCP instance holding the handle */
	TCPHANDLE h)		/* (input) TCP handle to convert */
{
	DPRINT(("tcp2commHdl : Handle %d : ", h));

	if (TCP_HDL_NONE == h) {
		DPRINT(("TCP_HDL_NONE => PLAYER_NONE\n"));
		return PLAYER_NONE;
	} else if (tcp->myHandle == h) {
		DPRINT(("tcp->myHandle => PLAYER_ME\n"));
		return PLAYER_ME;
	} else if (tcp->broadcastHandle == h) {
		DPRINT(("tcp->broadcastHandle => PLAYER_BROADCAST\n"));
		return PLAYER_BROADCAST;
	}

	DPRINT(("\n"));
/*	DPRINT(("WARNING: converting between dissimilar int types.")); */
	return ((playerHdl_t) h);
}

/*****************************************************************************
 Convert a playerHdl_t to the corresponding TCPHANDLE.
*****************************************************************************/
static TCPHANDLE		/* corresponding TCP handle */
commHdl2tcp(
	TCPINSTANCE* tcp,	/* (input) TCP instance holding the handles */
	playerHdl_t h)		/* (input) player handle to convert */
{
	DPRINT(("commHdl2tcp : Handle %d : ", h));
	if (PLAYER_NONE == h) {
		DPRINT(("PLAYER_NONE => TCP_HDL_NONE\n"));
		return TCP_HDL_NONE;
	} else if (PLAYER_ME == h) {
		DPRINT(("PLAYER_ME => tcp->myHandle\n"));
		return tcp->myHandle;
	} else if (PLAYER_BROADCAST == h) {
		DPRINT(("PLAYER_BROADCAST => tcp->broadcastHandle\n"));
		return tcp->broadcastHandle;
	}
	/* DPRINT(("WARNING: converting between dissimilar int types.")); */
	DPRINT(("\n"));
	return ((TCPHANDLE) h);
}

/*****************************************************************************
 Do nothing; return TRUE.  If resp is not null, copy TCP_RES_OK
 into resp->status
*****************************************************************************/
DLLEXPORT int cdecl			/* success boolean */
commNoOp(
	commNoOpReq_t *req,		/* (input) ignored */
	commNoOpResp_t *resp)	/* (output) status */
{
	DPRINT(("@TCP commNoOp(): "));

	(void) req;
	if(resp != NULL)
		resp->status = TCP_RES_OK;

	DPRINT(("@\n"));
	return (TRUE);
}

/*-------------------------------------------------------------------------
 List the available comm ports.
 Implemented only for serial and modem drivers.
-------------------------------------------------------------------------*/
DLLEXPORT int cdecl
commEnumPorts(
	commEnumPortsReq_t *	req,		/* Request (or NULL) */
	commEnumPortsResp_t *	resp)		/* Response (or NULL) */
{
	(void) req;
	if (resp)
		resp->status = comm_STATUS_BUG;
	return FALSE;
}

/*****************************************************************************
 Initialize the communications driver.

 WARNING: Call this function only once.

 The driver is initialized, creating a connection as specified in the
 req parameter.  Only the portnum field in req is used; the connection
 is set up as a datagram connection on the local IP address, using the
 port specified in portnum.  The following special constants may be
 used in portnum:
	0				Let the OS choose a port number.

 Note: the port number parameter is ignored if TCP_LAN is defined,
 since everyone has to use the same port number for broadcast to work.

 Return TRUE if successful, FALSE otherwise.  On successful return,
 the port number in req is set to the actual port number used.
*****************************************************************************/
DLLEXPORT int cdecl			/* success boolean */
commInit(
	commInitReq_t *req,		/* (modified) type of connection */
	commInitResp_t *resp)	/* (output) status */
{
	int status;
	u_short port;
	commInitResp_t respDummy;

	DPRINT(("@TCP commInit(): "));

#ifdef LOGPKT_ENABLED
	logpkt_fp = logPkt_open();
#endif

	/* Protect against invalid parameters */
	if (NULL == resp)
	{
		resp = &respDummy;
	}

#ifdef TCP_LAN
	port = TCP_SOCKET_DEFAULT;
#else
	if ((NULL == req) || (0L == req->portnum)) {
		/*
		 * If the flags indicate that we are auto-opening connections
		 * then we should use the default port
		 */
		if (req->flags & comm_INIT_FLAGS_CONN_ADDR)
		{
			port = TCP_SOCKET_DEFAULT;
		}
		else
		{
			port = TCP_SOCKET_ANY;
		}
	}
	else
	{
		port = (u_short)req->portnum;
	}
#endif

#ifdef TCP_LAN
	{
	int autodialenabled = adialup_autodial_enabled();
	if (autodialenabled)
		adialup_autodial_set(FALSE);
#endif

	/* Create the TCP instance */
	pTcp = TCPWIN_Create(TCP_MAX_PEERS, &port, &status);

#ifdef TCP_LAN
	if (autodialenabled)
		adialup_autodial_set(autodialenabled);
	}
#endif

	if (pTcp == NULL) {
		DPRINT(("@TCP commInit: TCP is not present, status %d\n", status));
		resp->status = TCP_RES_NETWORK;
		return FALSE;
	}
	req->portnum = port;

	/* Successful */
	resp->status = comm_STATUS_OK;
/*	DPRINT(("@%u\n", resp->status)); */
	return TRUE;
}

/*****************************************************************************
 Tear down the communications driver.

 WARNING: Call this function only once, only after calling commInit().

 Return TRUE if successful, FALSE otherwise.  If resp is not NULL,
 TCP_RES_OK is copied into resp->status.
*****************************************************************************/
DLLEXPORT int cdecl			/* success boolean (always TRUE) */
commTerm(
	commTermReq_t *req,		/* (input) ignored */
	commTermResp_t *resp)	/* (output) status */
{
	DPRINT(("@TCP commTerm(): "));

	logPkt_close(logpkt_fp);

	/* Protect against invalid parameters */
	(void) req;

	/* Tear down the TCP instance */
	TCPWIN_Destroy(pTcp);

	/* Set the status and leave */
	if(NULL != resp)
		resp->status = TCP_RES_OK;
	DPRINT(("@\n"));
	return TRUE;
}

/*****************************************************************************
 Retrieve info about the communications driver.

 The driver info is copied into the appropriate field in resp.  The
 fields and values are:
	status (comm_status_t): Return status; always TCP_RES_OK.
	info (comm_driverInfo_t): Driver info (see below).

 WARNING: do not modify the driver info.

 Return TRUE if info was retrieved, FALSE if resp was NULL.

 We plan to make dpEnumTransports scan through the DLL file looking
 for this signature.  It will use the info to present a driver name
 to the user, and discover driver properties such as "expects to be
 called by external shell" (e.g. dwango).
*****************************************************************************/
DLLEXPORT int cdecl				/* success boolean */
commDriverInfo(
	commDriverInfoReq_t *req,	/* (input) ignored */
	commDriverInfoResp_t *resp)	/* (output) driver info */
{
	/* Driver info */
	/* const */ static comm_driverInfo_t commDriverInfo = {
		comm_DRIVER_SIGNATURE,
		sizeof(comm_driverInfo_t),
#ifdef _DEBUG
		"Debug "
#endif
#ifndef TCP_LAN
		"Internet",
#else
		"TCP/IP LAN",
#endif
		0x0100,
		(comm_DRIVER_IS_VISIBLE | comm_DRIVER_IS_FAST |
				comm_DRIVER_ALLOWS_GAMELIST
#ifndef TCP_LAN
				| comm_DRIVER_NO_BROADCAST
#endif
				| comm_DRIVER_JUMBO_PACKETS),
		0
	};

	/* Protect against invalid parameters */
	(void) req;
	assert(resp != NULL);
	if (NULL == resp)
		return FALSE;

	/* Copy data and return it */
	resp->info = &commDriverInfo;
	resp->status = TCP_RES_OK;
	return (TRUE);
}

/*****************************************************************************
 Retrieve info about a player, including ourselves.

 The player info is copied into the appropriate fields in resp.  The
 fields and values are:
	status (comm_status_t): TCP_RES_OK on success, TCP_RES_BAD for
				invalid parameters, a value returned by
				TCPWIN_Handle2Address() on other error.
	name (char[]): Player name; always the empty string (this layer
				doesn't know about names).
	address (void *): The player's address.  This is a six-byte buffer;
				the first four bytes are the IP address in network order,
				and the last two bytes are the port number in network order.
	address2 (void *): The player's second address. If the player does
				not have a second address then it is identical to the first
	addrLen (size_t): The number of bytes that address points to.
	nodeID (char[8]): Always {0,0,0,0,0,0,0,0}.

 WARNING: Potential memory leak, potential unexpected behavior.  The player
 address is copied into a static buffer, and the resp->address field is
 overwritten to point to the static buffer.  If the resp->address field
 points to allocated memory other than a previous static buffer, that
 pointer will be lost.  Additionally, it may be invalid to keep a reference
 to the static buffer; subsequent calls to commPlayerInfo() will change
 the value stored in that buffer.  The address should be used immediately
 and discarded, or copied into a permanent storage place if desired.
 Keeping a pointer to and subsequently modifying the buffer may cause
 subsequent calls to commPlayerInfo() to return invalid data.

 Return TRUE if the player is known and is not a group, FALSE otherwise.
*****************************************************************************/
DLLEXPORT int cdecl					/* success boolean */
commPlayerInfo(
	commPlayerInfoReq_t *req,		/* (input) player to get info about */
	commPlayerInfoResp_t *resp)		/* (output) info about player */
{
	int err;
	static TCPPEER addrBuf;
	static TCPPEER2 addrBuf2;

	DPRINT(("@TCP commPlayerInfo(): "));

	/* Protect against NULL req */
	assert(NULL != req);
	if (NULL == req) {
		if(NULL != resp)
			resp->status = TCP_RES_BAD;
		return FALSE;
	}

	/* Get corresponding TCP peer from player handle */
	err = TCPWIN_Handle2Address(pTcp, commHdl2tcp(pTcp, req->player), &addrBuf, &addrBuf2);
	if (TCP_RES_OK != err)
	{
		if (NULL != resp)
		{
			resp->status = err;
		}
		return FALSE;
	}

	/* Protect against NULL resp ??? should this be an assertion ??? */
	if (NULL != resp)
	{
		// fill in response struct from TCP peer info
		resp->name = "";		// This layer doesn't know about names
		resp->address = &addrBuf;
		resp->address2 = (addrBuf2.status == TCP_HDL2_NONE) ? &addrBuf : &(addrBuf2.peer);
		resp->addrLen = sizeof(addrBuf);
		memset(&resp->nodeID, 0, sizeof(resp->nodeID));
		resp->status = (addrBuf2.status == TCP_HDL2_NONE) ? TCP_RES_OK : TCP_RES_DUAL_ADDRESS;

		resp->flags = 0;
		resp->flags |= (addrBuf2.status & TCP_HDL2_LIVE) ? comm_PLAYERINFO_2NDLIVE : 0;
	}

	return(TRUE);
}

/*****************************************************************************
 Find out whether the transmit queue is full.

 Return TRUE if no more packets can be queued for transmission at this
 time, FALSE otherwise.

 Not yet implemented.
*****************************************************************************/
DLLEXPORT int cdecl				/* Transmit queue full boolean */
commTxFull(
	commTxFullReq_t * req,		/* (input) ignored */
	commTxFullResp_t * resp)	/* (output) status */
{
	commTxFullResp_t respDummy;

	DPRINT(("@TCP commTxFull(): "));

	/* Protect against invalid parameters */
	(void) req;
	if (NULL == resp)
		resp = &respDummy;

	resp->status = TCP_RES_UNIMPLEMENTED;
	return FALSE;
}

/*****************************************************************************
  Send a packet.

  On entry, req->dest contains the player handle of the recipient or
  PLAYER_BROADCAST, req->buffer contains the packet to be sent, and
  req->length contains the number of bytes in the packet.

  On exit, the buffer may be discarded, although the packet may not be
  sent until later; resp->status contains the completion status.

  Return FALSE on error, TRUE otherwise.  Note that a TRUE return value does
  not guarantee that the packet has been (or ever will be) sent.

  WARNING: ignores req->flags.
*****************************************************************************/
DLLEXPORT int cdecl				/* success boolean */
commTxPkt(
	commTxPktReq_t * req,		/* input: destination, packet */
	commTxPktResp_t * resp)		/* output: status */
{
	commTxPktResp_t respDummy;
	TCPHANDLE h;

	DPRINT(("@TCP commTxPkt(): "));

	/* Protect against invalid parameters */
	assert(req != NULL);
	assert(req->buffer != NULL);
	assert(req->length > 0);
	if (NULL == resp)
		resp = &respDummy;
	if ((NULL == req) || (NULL == req->buffer) || (req->length <= 0)) {
		resp->status = TCP_RES_BAD;
		return FALSE;
	}
	if (!pTcp) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Simulate non-broadcast */
	#ifndef TCP_LAN
		if (PLAYER_BROADCAST == req->dest) {
			resp->status = comm_STATUS_BAD;
			return FALSE;
		}
	#endif

	logPkt(logpkt_fp, req->buffer, req->length, req->dest, "tx");

	/* Send the packet */
	h = commHdl2tcp(pTcp, req->dest);
	resp->status = TCPWIN_PutPacket(pTcp, req->buffer, req->length, h);
	return (TCP_RES_OK == resp->status);
}

/*****************************************************************************
  Get information about a pending incoming packet.
  Return TRUE if a packet is available, FALSE otherwise.

  Not yet implemented.
*****************************************************************************/
DLLEXPORT int cdecl
commPeekPkt(
	commPeekPktReq_t *req,
	commPeekPktResp_t *resp)
{
	commPeekPktReq_t  reqDummy;
	commPeekPktResp_t respDummy;

	DPRINT(("@TCP commPeekPkt(): "));

	/* Protect against invalid parameters */
	if (NULL == req)
		req = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	// Not implemented
	resp->status = TCP_RES_UNIMPLEMENTED;
	return FALSE;
}

/*****************************************************************************
 Retrieve a pending incoming packet.

 On entry, req->buffer must point to a block of memory; req->size must be
 set to the number of bytes in req->buffer.  The packet is placed into
 req->buffer, and the number of bytes actually received is placed in
 resp->length.  Also, resp->src is set to the handle of the player sending
 the packet.

 Returns TRUE if a packet was retrieved, FALSE otherwise; resp->status is
 TCP_RES_OK on success, other values on error.
*****************************************************************************/
DLLEXPORT int cdecl			/* success boolean */
commRxPkt(
	commRxPktReq_t *req,	/* Non-NULL; input bufsize/output buffer */
	commRxPktResp_t *resp)	/* output (discarded if NULL) */
{
	commRxPktResp_t respDummy;
	int err;
	TCPHANDLE hTcp;
	TCPPEER addr;

/*	DPRINT(("@TCP commRxPkt(): ")); */

	/* Protect against invalid parameters */
	assert(req != NULL);
	assert(req->buffer != NULL);
	assert(req->size > 0);
	if (NULL == resp)
		resp = &respDummy;
	if ((NULL == req) || (NULL == req->buffer) || (req->size <= 0)) {
		resp->status = TCP_RES_BAD;
		return FALSE;
	}
	if (!pTcp) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Retrieve packet */
	resp->length = req->size;
	err = TCPWIN_GetPacket(pTcp, req->buffer, (ULONG *) &resp->length, &hTcp, &addr);
	if(TCP_RES_OK != err) {
		resp->status = err;
		return FALSE;
	}

	resp->src = tcp2commHdl(pTcp, hTcp);
	logPkt(logpkt_fp, req->buffer, resp->length, resp->src, "rx");

	/* Handle ping packets immediately */
	if ((dp_PING_PACKET_ID == *(dp_packetType_t*)req->buffer) &&
			(TCP_HDL_NONE != hTcp) && (pTcp->myHandle != hTcp))
	{
		*(dp_packetType_t*)req->buffer = dp_PING_RESP_PACKET_ID;
		logPkt(logpkt_fp, req->buffer, resp->length, resp->src, "tx");
		TCPWIN_PutPacket(pTcp, req->buffer, resp->length, hTcp);
		err = TCP_RES_EMPTY;
	}

	/* Ignore packets from myself, save address of unknown senders */
	if (PLAYER_ME == resp->src)
	{
		err = TCP_RES_EMPTY;
	}
	else if (TCP_HDL_NONE == hTcp)
	{
		assert(sizeof(TCPPEER) <= comm_MAX_ADR_LEN);
		memcpy(resp->adr, &addr, sizeof(TCPPEER));
	}

	/* Save status and return */
	resp->status = err;
	return (TCP_RES_OK == err);
}

/*****************************************************************************
 Attempt to parse a NUL-terminated address string into a 6-byte address
 buffer.

 Address strings are of the form <address>[:<port>], where <port> is a
 two-byte decimal number, and <address> is either four individual one-byte
 decimal numbers separated by '.'s (e.g., 255.255.255.255 or 1.1.1.1),
 or a string that can be resolved by a name lookup.  If there is no port
 specified in the string, the address buffer is given a port of
 TCP_SOCKET_DEFAULT.

 Return TRUE if the buffer was formatted successfully, FALSE otherwise.
*****************************************************************************/
DLLEXPORT int cdecl				/* success boolean */
commScanAddr(
	commScanAddrReq_t *req,		/* (modified) string, buffer size/address */
	commScanAddrResp_t *resp)	/* (output) address size, status */
{
	commScanAddrResp_t respDummy;
	u_long adr;
	u_short port = TCP_SOCKET_DEFAULT;
	char * pc;
	char hostname[128];

	/* Protect against invalid parameters */
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = TCP_RES_BAD;
		return FALSE;
	}

	DPRINT(("@TCP commScanAddr(%s)\n", req->printable));

	/* Make sure there's enough size in the buffer */
	if (req->size < sizeof(TCPPEER)) {
		resp->status = TCP_RES_FULL;
	    return FALSE;
	}
	resp->status = TCP_RES_OK;

	strcpy(hostname, req->printable);
	/* Strip port from req's string */
	for(pc = hostname; *pc != '\0'; pc++) {
		if(':' == *pc) {
			*pc = '\0';
			*pc++;
			if(*pc != '\0')
				port = (unsigned short) atoi(pc);
			break;
		}
	}

	/* Convert address if it's in numeric format (e.g., 255.255.255.255) */
	adr = inet_addr(hostname);

	/* Get address from name if not numeric */
	/* WARNING: gethostbyname is not SOCKS5 compliant */
	if (adr == INADDR_NONE) {
		struct hostent *he;
		he = gethostbyname(hostname);
		if (!he) {
			resp->status = TCP_RES_BAD;
			return FALSE;
		}
		memcpy(&adr, he->h_addr, sizeof(u_long));
	}

	/* Save and return */
	/* Note: yes, it really expects the returned address in a field
	   in the req parameter, and the length of that address in a field
	   in the resp parameter. */
	if (NULL != req->address) {
		((TCPPEER *)req->address)->addr = adr;
		((TCPPEER *)req->address)->port = htons(port);
	}
	resp->length = sizeof(TCPPEER);
	return (TRUE);
}

/*****************************************************************************
  Attempt to format a 6-byte address buffer into a NUL-terminated
  string.  The string will be of the form 255.255.255.255:65535, where
  the first four numbers are the address and the final number is the port
  (socket) number.

  Return TRUE if the buffer was formatted successfully, FALSE otherwise.
*****************************************************************************/
DLLEXPORT int cdecl				/* success boolean */
commPrintAddr(
	commPrintAddrReq_t *req,	/* (modified) Address to format, buffer */
	commPrintAddrResp_t *resp)	/* (output) status */
{
	commPrintAddrResp_t respDummy;
	char * strAddr;
	char strPort[128];
	struct in_addr inetAddr;

	DPRINT(("@TCP commPrintAddr(): "));

	/* Protect against invalid parameters */
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = TCP_RES_BAD;
		return FALSE;
	}

	/* Make sure the address is the correct size */
	if (req->length != sizeof(TCPPEER)) {
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}
	/* Get printable version */
	sprintf(strPort, ":%d", ntohs(((TCPPEER *)(req->address))->port));
	inetAddr.s_addr = ((TCPPEER *)(req->address))->addr;
	strAddr = inet_ntoa(inetAddr);
	if (!strAddr)  {
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}

	/* Copy printable version to response */
	if (req->printable != NULL) {
		if (req->size < strlen(strAddr) + strlen(strPort) + 1) {
			resp->status = TCP_RES_FULL;
			return FALSE;
		}
		strcpy(req->printable, strAddr);
		strcat(req->printable, strPort);
	}

	/* Save status and return */
	resp->status = TCP_RES_OK;
	return TRUE;
}

/*****************************************************************************
  Generate a pseudo-player handle referring to a group of players.  Handy
  for multicasting.  A group can have zero players.
  Return TRUE if the pseudo-player handle was generated.

  Not yet implemented.
*****************************************************************************/
DLLEXPORT int cdecl
commGroupAlloc(
	commGroupAllocReq_t *req,
	commGroupAllocResp_t *resp)
{
	commGroupAllocResp_t respDummy;

	DPRINT(("@TCP commGroupAlloc(): "));

	/* Protect against invalid parameters */
	(void) req;
	if (NULL == resp)
		resp = &respDummy;

	resp->status = TCP_RES_UNIMPLEMENTED;
	return FALSE;
}

/*****************************************************************************
  Invalidate a pseudo-player handle referring to a group of players.
  Return TRUE if the pseudo-player handle was invalidated.

  Not yet implemented.
*****************************************************************************/
DLLEXPORT int cdecl
commGroupFree(
	commGroupFreeReq_t *req,
	commGroupFreeResp_t *resp)
{
	commGroupFreeResp_t respDummy;

	DPRINT(("@TCP commGroupFree(): "));

	/* Protect against invalid parameters */
	(void)req;
	if (NULL == resp)
		resp = &respDummy;

	resp->status = TCP_RES_UNIMPLEMENTED;
	return FALSE;
}

/*****************************************************************************
  Add one or more players to a group.
  Return TRUE if the players were all added.

  Not yet implemented.
*****************************************************************************/
DLLEXPORT int cdecl
commGroupAdd(
	commGroupAddReq_t *req,
	commGroupAddResp_t *resp)
{
	commGroupAddResp_t  respDummy;

	DPRINT(("@TCP commGroupAdd(): "));

	/* Protect against invalid parameters */
	(void) req;
	if (NULL == resp)
		resp = &respDummy;

	resp->status = TCP_RES_UNIMPLEMENTED;
	return FALSE;
}

/*****************************************************************************
  Set driver parameters.
*****************************************************************************/
DLLEXPORT int cdecl				/* success boolean */
commSetParam(
	commSetParamReq_t *req,		/* */
	commSetParamResp_t *resp)	/* */
{
	commSetParamReq_t   reqDummy;
	commSetParamResp_t  respDummy;

	DPRINT(("@TCP commSetParam(): "));

	/* Protect against invalid parameters */
	if (req  == NULL)  req  = memset(&reqDummy, 0, sizeof(*req));
	if (NULL == resp)
		resp = &respDummy;

	switch (req->param_num) {
	case comm_PARAM_DPRINTF:
		dp_dprintf_set((dp_dprintf_t) req->param_value);
		break;
	default:
		resp->status = TCP_RES_UNIMPLEMENTED;
		return FALSE;
	}
	resp->status = TCP_RES_OK;
	return TRUE;
}

/*****************************************************************************
  Establish a data link to a player and shake hands with him.  This does
  not actually establish a connection in the IP sense of the word.
  Return TRUE if the link was established and we shook hands.
*****************************************************************************/
DLLEXPORT int cdecl			/* success boolean */
commSayHi(
	commSayHiReq_t *req,	/* (input) address */
	commSayHiResp_t *resp)	/* (output) player handle, status */
{
	commSayHiResp_t respDummy;
	TCPHANDLE hTcp;

  DPRINT(("@TCP commSayHi():\n"));

	/* Protect against invalid parameters */
	assert(req != NULL);
	if (NULL == resp)
		resp = &respDummy;
	if (NULL == req) {
		resp->status = TCP_RES_BAD;
		return FALSE;
	}
	if (!pTcp) {
		resp->status = comm_STATUS_BUG;
		return FALSE;
	}

	/* Check length */
	if (req->length != (sizeof(u_long) + sizeof(u_short))) {
		DPRINT(("\nBad Address Length in commSayHi()!!"));
		resp->status = TCP_RES_BAD;
		return FALSE;
	}

	/* Copy req's player handle into resp */
	hTcp = TCPWIN_Address2Handle(pTcp, req->address, req->address2, TRUE, req->flags & comm_SAYHI_2NDLIVE);
	resp->player = tcp2commHdl(pTcp, hTcp);
	if (hTcp == TCP_HDL_NONE)
	{
		DPRINT(("Returned TCP_HDL_NONE in commSayHi()!!"));
		resp->status = TCP_RES_BAD;
		return FALSE;
	}

	/* Return success */
	resp->status = TCP_RES_OK;
	return TRUE;
}

/*****************************************************************************
  Tear down a data link to a player.  The link or the player may already be
  down, so don't shake hands.

  Return TRUE if the link was successfully broken.
*****************************************************************************/
DLLEXPORT int cdecl				/* success boolean */
commSayBye(
	commSayByeReq_t *req,		/* (input) address */
	commSayByeResp_t *resp)		/* (output) status */
{
	commSayByeResp_t respDummy;
	TCPHANDLE h;

	DPRINT(("@TCP commSayBye(): "));

	/* Protect against invalid parameters */
	assert(req != NULL);
	assert(req->player != PLAYER_NONE);
	if (NULL == resp)
		resp = &respDummy;
	if ((NULL == req) || (PLAYER_NONE == req->player)) {
		resp->status = TCP_RES_BAD;
		return FALSE;
	}

	/* Remove player from our list; don't allow removing one we don't have */
	h = commHdl2tcp(pTcp, req->player);
	if(NULL != dcstFindValue(pTcp->handles, h))
		dcstDeleteKey(pTcp->handles, h);

	/* Now check the secondary handles to ensure that there is no residual handle */
	if(NULL != dcstFindValue(pTcp->secondary, h))
		dcstDeleteKey(pTcp->secondary, h);

	/* Return status */
	resp->status = TCP_RES_OK;
	return TRUE;
}

#if defined(_WINDOWS)
/*****************************************************************************
  Windows-only DLL entry point.
*****************************************************************************/
BOOL WINAPI
DllMain (
	HANDLE hModule,
	DWORD fdwReason,
	LPVOID lpReserved)
{
/*	WSADATA wsaData; */

	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
			/* start Windows Sockets */
/*			WSAStartup(MAKEWORD(1, 1), &wsaData); */
			return(TRUE);

		case DLL_PROCESS_DETACH:
			/* end Windows Sockets */
/*			WSACleanup(); */
			return(TRUE);

		case DLL_THREAD_ATTACH:
			return(TRUE);

		case DLL_THREAD_DETACH:
			return(TRUE);
	}

	(void) lpReserved;
	(void) hModule;
	return(TRUE);
}
#endif /* defined(_WINDOWS) */
