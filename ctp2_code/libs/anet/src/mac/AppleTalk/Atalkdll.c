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

/*---------------------------------------------------------------------------
 Applealk NetMech Code
---------------------------------------------------------------------------*/

#include <OpenTransport.h>
#include <OpenTptAppleTalk.h>

#include "ddprint.h"

#include "commapi.h"
#include "assoctab.h"
//#include <tcpabi/tcpabi.h>
//#include <tcpabi/tcpabix1.h>
#include <stdio.h>		// for sprintf and sscanf
//#include <i86.h>
#define  NO_EXT_KEYS
#include <string.h>

#include "atalkdll.h"
#include "AppleTalkDDP.h"
#include "otutils.h"
#include "otddp3.h"

#define DPMI_INT 		0x31
#define MAX_RAW_PKTLEN	512

#ifndef TRUE
    #define TRUE        (1)
    #define FALSE       (0)
#endif

#define	DEBUGSTR	//
//#define	DEBUGSTR	ColorDebugStr

#pragma export on

comm_driverInfo_t gCommDriverInfo =
{
	comm_DRIVER_SIGNATURE,
	sizeof(comm_driverInfo_t),
	"OT AppleTalk",
	0x0100,
	(comm_DRIVER_IS_VISIBLE
//	| comm_DRIVER_IS_FAST	//	slow like IP
	| comm_DRIVER_ALLOWS_GAMELIST),
	0					//	comm_INIT_NEEDS_SESSION_NAME no longer needed for AppleTalk
};

#pragma export off

EndpointRef			gDDPEndpoint;
MapperRef			gNBPMapper;
ATSvcRef			gAppleTalkRef;

static atalk_hdl_t hdl_rx = atalk_HDL_NONE;	// Listen for incoming pkts on this hdl
static assoctab_t *peertab = NULL;			// Maps hdls to adrs and back
static void* scratch_flat;

static playerHdl_t atalk2commHdl( atalk_hdl_t h )
{
	switch ( h ) {
	case atalk_HDL_NONE:		return PLAYER_NONE;
	case atalk_HDL_ME:			return PLAYER_ME;
	default:
		;
	}

	return ((playerHdl_t) h);
}

static atalk_hdl_t commHdl2atalk (playerHdl_t h )
{
	switch ( h ) {
	case PLAYER_NONE:		return atalk_HDL_NONE;
	case PLAYER_ME:			return atalk_HDL_ME;
	case PLAYER_BROADCAST:	return atalk_HDL_NONE;
	case PLAYER_UNKNOWN:	return atalk_HDL_NONE;
	default:
		;
	}

	return ((atalk_hdl_t) h);
}

/*-----------------------------------------------------------------------
 Given a local port number, a remote ip address, and a remote port number,
 return a handle that can be used to send an address to that address.  

 If 0 is given for the local port number, picks an unused one.  Not suitable
 for a handle that will be used to listen for incoming service requests on
 a standard port number.

 If 0 is given for the remote port number, return a handle that can be used
 for receiving packets sent from any port at the remote host.

 If 255.255.255.255 is given for the remote address, return a handle
 that can be used for sending broadcast packets, or for receiving without
 regard to source address.

 If no handle exists for that address,
	 If insert is non-zero,
	  allocate one.
	 else
          return atalk_HDL_NONE
 Multiple calls with the same arg will return the same result.
 There is no upper bound on handles.
 Returns atalk_HDL_NONE on failure.
-----------------------------------------------------------------------*/
atalk_hdl_t atalk_adr2hdl(DDPAddress* remoteadr, int insert)
{
	// Search peer tab.  If not found, append it.
	atalk_hdl_t h;
	DDPAddress *peer;
	int i;
	unsigned int lport;
	OSStatus	err;

	// Look for existing address/handle
	for (i=0; i<peertab->n_used; i++) {
		assoctab_item_t *pe = assoctab_getkey(peertab, i);
		if (!pe) 
			DEBUGSTR(("\patalk_adr2hdl: couldn't find entry\n"));
		else {
			peer = (DDPAddress *)pe->value;
			DPRINT(("atalk_adr2hdl: pe->value %p, *pe->value %x, remoteadr %x\n", pe->value, *peer, remoteadr));
			if (
				(peer->fNetwork == remoteadr->fNetwork) &&
				(peer->fNodeID == remoteadr->fNodeID) &&
				(peer->fSocket == remoteadr->fSocket)
			) {
				DPRINT(("atalk_adr2hdl: found match, hdl %d\n", pe->key));
				return pe->key;
			}
		}
	}
	if (!insert) {
		static noInsertion = 0;
		noInsertion++;
		return atalk_HDL_NONE;
	}

	// Open new handle for this address
	h = atalk_ddp_open(remoteadr, UDP_OPEN_NORMAL, &lport);
	if (h == atalk_HDL_NONE)
		return h;

	// Save address
	peer = (DDPAddress *)assoctab_subscript_grow(peertab, h);
	if (!peer) {
		// Abort! Out of memory!
		return atalk_HDL_NONE;
	}
	DPRINT(("atalk_adr2hdl: saving %x at %p\n", remoteadr, peer));
	*peer = *remoteadr;
	DPRINT(("atalk_adr2hdl: udp_open successful. localport %d\n", lport));
	return h;
}

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding IP address.
 Returns comm_STATUS_EMPTY on failure.
-----------------------------------------------------------------------*/
comm_status_t atalk_hdl2adr(atalk_hdl_t hdl, DDPAddress *adr)
{
	DDPAddress *peer;

	peer = (DDPAddress *)assoctab_subscript(peertab, hdl);
	if (!peer) {
		DPRINT(("atalk_hdl2adr: couldn't find peer %d\n", hdl));
		return comm_STATUS_EMPTY;
	}
	*adr = *peer;
	return comm_STATUS_OK;
}

#pragma export on

/*
 *	Spin your wheels -- and get paid for it!
 *
 *	Return FALSE on error.
 */

int
commNoOp(
	commNoOpReq_t *		req,		// Request (or NULL)
	commNoOpResp_t *	resp)		// Response (or NULL)
{
	commNoOpReq_t	reqDummy;
	commNoOpResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commNoOp(): "));

	if (req == NULL)
		req = (commNoOpReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = comm_STATUS_OK;

	DEBUGSTR(("\p@\n"));

	return (TRUE);
}


/*
 *	Initialize the communications driver.
 *  Call this function only once.
 *
 *	Return FALSE on error.
 */

int
commInit(
	commInitReq_t *		req,		// Request (or NULL)
	commInitResp_t *	resp)		// Response (or NULL)
{
	commInitReq_t		reqDummy;
	commInitResp_t		respDummy;
	DDPAddress*			padr;
	OSStatus			err;
	AppleTalkInfo		atalkInfo;
	TNetbuf				theNetBuffer;
	extern DDPAddress	gDDPAddress;
	
	DEBUGSTR(("\p@ATALK commInit(): "));

	if (req == NULL)
		req = (commInitReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;
		
	scratch_flat = malloc(MAX_RAW_PKTLEN);
	if (!scratch_flat) {
		DEBUGSTR(("\pcommInit: couldn't allocate DOS memory!\n"));
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}
	
	//	check for the existanct of OpenTransport
	if(!OpenTransportExists() || !OpenTransportAppleTalkExists()) {
		DEBUGSTR(("\pcommInit: Open Transport not found\n"));
		resp->status = comm_STATUS_NETWORK_NOT_PRESENT;
		free(scratch_flat);
		return FALSE;
	}
	
	//	Initialize OpenTransport
	err = InitOpenTransport();
	if (err != noErr) {
		DEBUGSTR(("\pcommInit: Cannot Init Open Transport\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		return FALSE;
	}
	
	//	open appletalk services
	
	err = StartupAppleTalk();
	//gAppleTalkRef = OTOpenAppleTalkServices(kDefaultAppleTalkServicesPath, 0, &err);
	if (err != noErr) {
		DEBUGSTR(("\pcommInit: Cannot Open AppleTalk Services\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		return FALSE;
	}
	
	//	open an endpoint for sending and recieving data (if we are resuming, we start
	//	with the same address as the last time)
	
	err = CreateAndConfigDDP(&gDDPEndpoint, req->flags & comm_INIT_FLAGS_RESUME);
	if (err != noErr) {
		DEBUGSTR(("\pcommInit: Cannot Configure Endpoint\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		ShutDownAppleTalk();
		return FALSE;
	}
	
	//	open the mapper and register our name
	
	err = CreateAndConfigNBP(&gNBPMapper);
	if (err != noErr) {
		DEBUGSTR(("\pcommInit: Cannot Configure Mapper\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		ShutDownAppleTalk();
		return FALSE;
	}
	
	if (!req->flags & comm_INIT_FLAGS_RESUME) {
		OTRegisterMyName();
	}
	
	//	create the peer table
	
	peertab = assoctab_create(sizeof(DDPAddress));
	if (!peertab) {
		// ABORT! Out of Memory
		DEBUGSTR(("\pcommInit: Cannot Allocate assoc table\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		ShutDownAppleTalk();
		return FALSE;
	}
	
	// Store our address in the peer table under the bogus ME handle
	padr = (DDPAddress *)assoctab_subscript_grow(peertab, atalk_HDL_ME);
	if (!padr) {
		DEBUGSTR(("\pcommInit: couldn't grow peer table\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		assoctab_destroy(peertab);
		ShutDownAppleTalk();
		return FALSE;
	}

	#if 1
	
		//	get our address from the global created when the endpoint was bound
		
		memcpy(padr, &gDDPAddress, sizeof(DDPAddress));
	
	#else
		
		//	Get information about the AppleTalk network
		
		theNetBuffer.maxlen = sizeof(AppleTalkInfo);
		theNetBuffer.len = sizeof(AppleTalkInfo);	//	size of data to get
		theNetBuffer.buf = (UInt8*) &atalkInfo;		//	buffer to write data to
		
		err = OTATalkGetInfo(gAppleTalkRef, &theNetBuffer);
		if (err != noErr) {
			DEBUGSTR(("\pcommInit: Cannot Get AppleTalk Info\n"));
			resp->status = comm_STATUS_BAD;
			free(scratch_flat);
			ShutDownAppleTalk();
			return FALSE;
		}
		
		atalkInfo.fOurAddress.fSocket = SOCKET_MW2;
		memcpy(padr, &atalkInfo.fOurAddress, sizeof(DDPAddress));
	#endif

	resp->status = comm_STATUS_OK;
	DEBUGSTR(("\pcommInit: success\n"));
	return TRUE;
}


/*
 *	Tear down the communications driver.  
 *
 *	Return FALSE on error.
 */

int
commTerm(
	commTermReq_t *		req,		// Request (or NULL)
	commTermResp_t *	resp)		// Response (or NULL)
{
	commTermReq_t	reqDummy;
	commTermResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commTerm(): "));

	if (req == NULL)
		req = (commTermReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	if (peertab) {
		int i;
		for (i=0; i<peertab->n_used; i++) {
			assoctab_item_t *pe = assoctab_getkey(peertab, i);
			if (!pe) 
				DEBUGSTR(("\pcommTerm: couldn't find entry\n"));
			else if (pe->key != atalk_HDL_ME) {
				DPRINT(("commTerm: closing handle %d\n", pe->key));
				atalk_ddp_close(pe->key, UDP_CLOSE_NORMAL);
			} else {
				DPRINT(("commTerm: not closing handle %d\n", pe->key));
			}
		}
		assoctab_destroy(peertab);
	}
	free(scratch_flat);
	resp->status = comm_STATUS_OK;
	DEBUGSTR(("\pcommTerm: success\n"));
	
	//	unbind our endpoint and close open transport
	
	ShutDownAppleTalk();

	return TRUE;
}


/*
 *	Retrieve info about the communications driver.
 *
 *	Return TRUE if info was retrieved.
 */

int commDriverInfo(commDriverInfoReq_t *req, commDriverInfoResp_t *resp)
{
	static commDriverInfoResp_t   commDriverInfoResp =
	{
		0,
		&gCommDriverInfo
	};

    if (resp == NULL) return FALSE;
	*resp = commDriverInfoResp;

    resp->status = 0;
    return (TRUE);
}

/*
 *	Retrieve info about a player, including ourselves.
 *
 *	Return TRUE if the player is known and is not a group.
 *
 *  Pointers returned in the response point to static buffers
 *  which will be overwritten by the next call.
 */

int
commPlayerInfo(
	commPlayerInfoReq_t *	req,	// Request
	commPlayerInfoResp_t *	resp)	// Response (or NULL)
{
	commPlayerInfoReq_t		reqDummy;
	commPlayerInfoResp_t	respDummy;
	static DDPAddress 		adr;
	comm_status_t			err;


	if (req == NULL)
		req = (commPlayerInfoReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	DPRINT(("commPlayerInfo: player %d\n", req->player));

	// This comm layer doesn't know about user names.
	resp->name = "";

	err = atalk_hdl2adr(commHdl2atalk(req->player), &adr );
	if (err != comm_STATUS_OK) {
		DPRINT(("commPlayerInfo: atalk_hdl2adr returns %d\n", err));
		resp->status = err;
		return FALSE;
	}

	resp->address = (void*) &adr;
	resp->addrLen = sizeof(DDPAddress);
	memset(resp->nodeID, 0, sizeof(resp->nodeID));		// node id is not supported
	resp->status = comm_STATUS_OK;

	return TRUE;
}


/*
 *	Find out whether the transmit queue is full.
 *
 *	Return TRUE if no more packets can be queued for transmission at this
 *	time.
 */

int
commTxFull(
	commTxFullReq_t *	req,	// Request (or NULL)
	commTxFullResp_t *	resp)	// Response (or NULL)
{
	commTxFullReq_t		reqDummy;
	commTxFullResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commTxFull(): "));

	if (req == NULL)
		req = (commTxFullReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = comm_STATUS_OK;

	return FALSE;
}


/*
 *	Send a packet.  Upon return, the buffer can be discarded, although the
 *	packet may not be sent until later.
 *
 *	Return FALSE on error.  Note that a TRUE return value does not guarantee
 *	that the packet has been (or ever will be) sent.
 */

int
commTxPkt(
	commTxPktReq_t *	req,	// Request
	commTxPktResp_t *	resp)	// Response (or NULL)
{
	commTxPktReq_t	reqDummy;
	commTxPktResp_t	respDummy;
	comm_status_t	err;
	atalk_hdl_t		h;

	if (req == NULL)
		req = (commTxPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;
	
	if (req->dest == PLAYER_BROADCAST) {
	
		//	if we are broadcasting, we need to register our name now
		
		OTRegisterMyName();

		//	broadcast is only used for enumSessions packets
		//	if this is an enumSessions packet, start a name lookup (this does nothing
		//	if one is already running). We do the broadcast because the lookup starts
		//	by clearing the array of DDP addresses we have accumulated so far)
	
		err = atalk_ddp_broadcast(req->buffer, req->length);
		DoOTNameLookup();
		CheckZoneList();
	} else {
	
		//	this is a normal packet, just send it to who it should be sent to

		h = commHdl2atalk(req->dest);
		err = atalk_ddp_send(h, req->buffer, req->length, TTLTOS_NORMAL, 0, UDP_SEND_NORMAL);
	
	}

	if (err == -1) {
		resp->status = comm_STATUS_FULL;
	} else {
		resp->status = comm_STATUS_OK;
	}
	
	return (resp->status == comm_STATUS_OK);
}


/*
 *	Get information about a pending incoming packet.
 *
 *	Return TRUE if a packet was retrieved.
 */

int
commPeekPkt(
	commPeekPktReq_t *	req,	// Request (or NULL)
	commPeekPktResp_t *	resp)	// Response (or NULL)
{
	commPeekPktReq_t	reqDummy;
	commPeekPktResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commPeekPkt(): "));

	if (req == NULL)
		req = (commPeekPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	// Not implemented
	resp->status = comm_STATUS_BAD;

	return FALSE;
}


/*
 *	Retrieve a pending incoming packet.
 *
 *	Return TRUE if a packet was retrieved.
 *  Status is zero on success, nonzero on error!
 */

int
commRxPkt(
	commRxPktReq_t *	req,	// Request (or NULL)
	commRxPktResp_t *	resp)	// Response (or NULL)
{
	commRxPktReq_t	reqDummy;
	commRxPktResp_t	respDummy;
	atalk_session_info_t *info;
	int len;

	DEBUGSTR(("\p@ATALK commRxPkt(): "));

	if (req == NULL)
		req = (commRxPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	len = atalk_ddp_recv(hdl_rx, req->buffer, req->size, 0, UDP_RECV_NORMAL, TTLTOS_NORMAL, NULL);
	if (len == -1) {
		resp->status = comm_STATUS_EMPTY;
		return FALSE;
	}
	
	resp->length = len;
	atalk_ddp_status(hdl_rx, 0, NULL, &info);
	resp->src = atalk2commHdl(atalk_adr2hdl(&info->ip_dst, FALSE));
	resp->status = comm_STATUS_OK;
	DPRINT(("commRxPkt: got packet, length %d, source hdl %d\n", len, resp->src));

	// ignore packets from myself
	if (resp->src == PLAYER_ME)
		resp->status = comm_STATUS_EMPTY;

	return (resp->status == comm_STATUS_OK);
}


/*
 *	Attempt to parse a NUL-terminated address string into a free-format
 *	buffer.
 *
 *	Return TRUE if the string was parsed successfully.
 */

int
commScanAddr(
	commScanAddrReq_t *		req,	// Request
	commScanAddrResp_t *	resp)	// Response (or NULL)
{
	commScanAddrReq_t	reqDummy;
	commScanAddrResp_t	respDummy;
	DDPAddress* 		adr;
	OSStatus			err;
	long				nodeID;
	long				netID;
	long				socketID;
	
	//	this is only called if there is a masterhostname string. There should
	//	not be for the AppleTalk transport. This code was used for the NBP code
	//	was written in order to test the transport without it

	if (req == NULL)
		req = (commScanAddrReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	DPRINT(("@ATALK commScanAddr(%s):\n", req->printable));

	if (req->size < sizeof(DDPAddress)) {
		resp->status = comm_STATUS_FULL;
		return FALSE;
	}
	
	//	copy the address into the supplied buffer

	adr = (DDPAddress*) req->address;
	sscanf(req->printable, "%ld.%ld.%d", &netID, &nodeID, &socketID);
	OTInitDDPAddress(adr, netID, nodeID, socketID, 0);
	
	//	return the length of the address

	resp->length = sizeof(DDPAddress);
	resp->status = comm_STATUS_OK;

	return (TRUE);
}


/*
 *	Attempt to format a free-format address buffer into a NUL-terminated
 *	string.
 *
 *	Return TRUE if the buffer was formatted successfully.
 */

int
commPrintAddr(
	commPrintAddrReq_t *	req,	// Request
	commPrintAddrResp_t *	resp)	// Response (or NULL)
{
	commPrintAddrReq_t	reqDummy;
	commPrintAddrResp_t	respDummy;
	unsigned char 	*params;
	char				printable[80];

	DEBUGSTR(("\p@ATALK commPrintAddr(): "));

	if (req == NULL)
		req = (commPrintAddrReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = comm_STATUS_OK;

	if (req->length != sizeof(*params))
		return FALSE;

	params = (unsigned char *)req->address;

	sprintf(printable, "%u.%u.%u.%u", params[0]&0xff, params[1]&0xff, params[2]&0xff, params[3]&0xff);

	if (req->printable != NULL) {
		if (req->size < strlen(printable) + 1) {
			resp->status = comm_STATUS_FULL;
			return FALSE;
		}
		strcpy(req->printable, printable);
	}

	return TRUE;
}


/*
 *	Generate a pseudo-player handle referring to a group of players.  Handy
 *	for multicasting.  A group can have zero players.
 *
 *	Return TRUE if the pseudo-player handle was generated.
 */

int
commGroupAlloc(
	commGroupAllocReq_t *	req,	// Request
	commGroupAllocResp_t *	resp)	// Response
{
	commGroupAllocReq_t		reqDummy;
	commGroupAllocResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commGroupAlloc(): "));

	if (req == NULL)
		req = (commGroupAllocReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}


/*
 *	Invalidate a pseudo-player handle referring to a group of players.
 *
 *	Return TRUE if the pseudo-player handle was invalidated.
 */

int
commGroupFree(
	commGroupFreeReq_t *	req,	// Request
	commGroupFreeResp_t *	resp)	// Response
{
	commGroupFreeReq_t	reqDummy;
	commGroupFreeResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commGroupFree(): "));

	if (req == NULL)
		req = (commGroupFreeReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}


/*
 *	Add one or more players to a group.
 *
 *	Return TRUE if the players were all added.
 */

int
commGroupAdd(
	commGroupAddReq_t *	req,	// Request
	commGroupAddResp_t *	resp)	// Response
{
	commGroupAddReq_t	reqDummy;
	commGroupAddResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commGroupAdd(): "));

	if (req == NULL)
		req = (commGroupAddReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}


/*
 *	Subtract one or more players from a group.  Do not delete the group,
 *	even if it is left empty.
 *
 *	Return TRUE unless there was a problem subtracting one or more players.
 */

#if 0
int
commGroupSubtract(
	commGroupSubtractReq_t *	req,	// Request
	commGroupSubtractResp_t *	resp)	// Response
{
	commGroupSubtractReq_t	reqDummy;
	commGroupSubtractResp_t	respDummy;

	DEBUGSTR(("\p@ATALK commGroupSubtract(): "));

	if (req == NULL)
		req = (commGroupSubtractReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}

#endif

/*
 *	Establish a data link to a player and shake hands with him.  This does
 *	not actually establish a connection in the IP sense of the word.
 *
 *	Return TRUE if the link was established and we shook hands.
 */

int
commSayHi(
	commSayHiReq_t *	req,	// Request
	commSayHiResp_t *	resp)	// Response
{
	commSayHiReq_t		reqDummy;
	commSayHiResp_t		respDummy;
	DDPAddress*			params;
	atalk_hdl_t			h;

	DEBUGSTR(("\p@ATALK commSayHi(): "));

	if (req == NULL)
		req = (commSayHiReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	params = (DDPAddress *)req->address;

	if (req->length != sizeof(*params)) {
		resp->status = comm_STATUS_BAD;
		DEBUGSTR(("\pcommSayHi: bad request length!\n"));
		return FALSE;
	}

	h = atalk_adr2hdl(params, TRUE);

	resp->player = atalk2commHdl(h);

	if (h == atalk_HDL_NONE) {
		resp->status = comm_STATUS_BAD;
		DEBUGSTR(("\pcommSayHi: atalk_adr2hdl fails\n"));
		return FALSE;
	}

	resp->status = comm_STATUS_OK;
	DPRINT(("commSayHi: atalk_adr2hdl returns %d; atalk2commHdl returns %d\n", h, resp->player));
	return TRUE;
}


/*
 *	Tear down a data link to a player.  The link or the player may already be
 *	down, so don't shake hands.
 *
 *	Return TRUE if the link was successfully broken.
 */

int
commSayBye(
	commSayByeReq_t *	req,	// Request
	commSayByeResp_t *	resp)	// Response (or NULL)
{
	commSayByeResp_t		respDummy;
	atalk_hdl_t h;
	comm_status_t err;
	int err0;

	DEBUGSTR(("\p@ATALK commSayBye(): "));

    if (resp == NULL) resp = &respDummy;
    if (req == NULL) {
		resp->status = 1;
		return FALSE;
	}
	h = commHdl2atalk(req->player);

	err0 = assoctab_subscript_delete(peertab, h);

	err = atalk_ddp_close(h, UDP_CLOSE_NORMAL);
	DPRINT(("commSayBye: player %d, hdl %d, delete returns %d, atalk_ddp_close returns %d\n", req->player, h, err0, err));
	if (err != TCPABI_NO_ERROR) {
		DPRINT(("commSayBye: atalk_ddp_close returns %d\n", err));
		resp->status = 2;
		return FALSE;
	}

    resp->status = 0;
    return (TRUE);
}

#pragma export off
