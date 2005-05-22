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
 Shim to turn ipx2.h's interface into commapi.h's interface
 $Log: Ipx2dll.c $
 Revision 1.1  1997/05/30 18:27:13  dkegel
 Initial revision
 Revision 1.9  1996/06/30 19:40:33  dkegel
 Added NO_BROADCAST option for researching WAN behavior...
 Revision 1.8  1996/05/22 00:46:11  dkegel
 Removed word "Driver" from dll description.
 Revision 1.7  1996/05/21 21:09:07  dkegel
 Issue more appropriate error if can't access IPX.
 Revision 1.6  1996/05/15 02:12:28  dkegel
 Use standard comm_STATUS codes in commInit.
 Revision 1.5  1996/04/30 20:59:35  dkegel
 Added "comm_DRIVER_IS_FAST" to capabilities bit field.
 Revision 1.4  1996/03/05 19:47:31  dkegel
 Implemented new driver signature stuff.
 Revision 1.3  1996/02/13 20:19:11  dkegel
 Report cause of error(in a kludgy way) when commInit fails.
 Revision 1.2  1996/02/12 03:14:28  dkegel
 Ignore packets from self.
 Revision 1.1  1996/02/12 01:57:41  dkegel
 Initial revision
---------------------------------------------------------------------------*/

#include "ddprint.h"

#include "ipx2.h"
#include "commapi.h"

#include <stdio.h>		// for sprintf and sscanf

static ipx2_t *ipx;

#pragma export on

comm_driverInfo_t gCommDriverInfo =
	{
		comm_DRIVER_SIGNATURE,
		sizeof(comm_driverInfo_t),
		"IPX",
		0x0100,
		(comm_DRIVER_IS_VISIBLE
		| comm_DRIVER_IS_FAST
		| comm_DRIVER_ALLOWS_GAMELIST),
		0
	};

#pragma export off

#define SOCKET_MW2		(0x52A1+commapi_NET_PORTOFFSET)		// (('M'<<4 + 'W')<<4 + '1')

#ifndef TRUE
    #define TRUE        (1)
    #define FALSE       (0)
#endif


static playerHdl_t ipx2commHdl( ipx2_hdl_t h )
{
	switch ( h ) {
	case ipx2_HDL_NONE:			return PLAYER_NONE;
	case ipx2_HDL_ME:			return PLAYER_ME;
	case ipx2_HDL_BROADCAST:	return PLAYER_BROADCAST;
	default:
		;
	}

	return ((playerHdl_t) h);
}

static ipx2_hdl_t commHdl2ipx( playerHdl_t h )
{
	switch ( h ) {
	case PLAYER_NONE:		return ipx2_HDL_NONE;
	case PLAYER_ME:			return ipx2_HDL_ME;
	case PLAYER_BROADCAST:	return ipx2_HDL_BROADCAST;
	case PLAYER_UNKNOWN:	return ipx2_HDL_NONE;
	default:
		;
	}

	return ((ipx2_hdl_t) h);
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

	DPRINT(("@IPX commNoOp(): "));

	if (req == NULL)
		req = (commNoOpReq_t *) memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = ipx2_RES_OK;

	DPRINT(("@\n"));

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
	int status;

//  printf("commInit starting...\n");
	DPRINT(("@IPX commInit(): "));

	if (req == NULL)
		req = (commInitReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;
		
	//	initialize Ipx
	
	IpxInitialize();

	ipx = ipx2_create(SOCKET_MW2, &status);

	if (!ipx) {
		DPRINT(("IPX is not present, status %d\n", status));
	    resp->status = comm_STATUS_NETWORK_NOT_PRESENT;   // ipx2_RES_BAD;
	    return FALSE;
	}

	resp->status = comm_STATUS_OK;

	DPRINT(("@%u\n", resp->status));
	
	#ifdef MACDEBUG
		{
			IPX_STATS		buffer;
			unsigned long		length = sizeof(IPX_STATS);
			
			IpxGetStatistics(&buffer, &length);
		}
	#endif

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

	#ifdef MACDEBUG
		{
			IPX_STATS		buffer;
			unsigned long		length = sizeof(IPX_STATS);
			
			IpxGetStatistics(&buffer, &length);
		}
	#endif

	DPRINT(("@IPX commTerm(): "));

	if (req == NULL)
		req = (commTermReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	ipx2_destroy(ipx);

	resp->status = ipx2_RES_OK;

	DPRINT(("@\n"));

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
//	(void) req;
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
	static nw_adr_t adr;
	ipx2_result_t	err;

	DPRINT(("@IPX commPlayerInfo(): "));

	if (req == NULL)
		req = (commPlayerInfoReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	// This comm layer doesn't know about user names.
	resp->name = "";

	err = ipx2_hdl2adr( ipx, commHdl2ipx(req->player), &adr );
	if (err != ipx2_RES_OK) {
		resp->status = err;
		return FALSE;
	}

	resp->address = &adr;
	resp->addrLen = sizeof(nw_adr_t);
	memset(resp->nodeID, 0, sizeof(resp->nodeID));  /* node id is not supported */
	resp->status = ipx2_RES_OK;

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

	DPRINT(("@IPX commTxFull(): "));

	if (req == NULL)
		req = (commTxFullReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = ipx2_RES_OK;

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
	ipx2_result_t	err;
	ipx2_hdl_t		h;

	DPRINT(("@IPX commTxPkt(): "));

	if (req == NULL)
		req = (commTxPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

//#define NO_BROADCAST		// Uncomment this line to simulate WAN lacking broadcast
#ifdef NO_BROADCAST
	if (req->dest == PLAYER_BROADCAST) {
		resp->status = comm_STATUS_BAD;
		return 0;
	}
#endif
	h = commHdl2ipx(req->dest);
	err = ipx2_put( ipx, req->buffer, req->length, h );
	resp->status = err;

	return (err == ipx2_RES_OK);
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

	DPRINT(("@IPX commPeekPkt(): "));

	if (req == NULL)
		req = (commPeekPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	// Not implemented
	resp->status = ipx2_RES_BAD;

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
	ipx2_result_t	err;
	ipx2_hdl_t		hdl;

	DPRINT(("@IPX commRxPkt(): "));

	if (req == NULL)
		req = (commRxPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->length = req->size;

	err = ipx2_get( ipx, req->buffer, &resp->length, &hdl, NULL );
	resp->src = ipx2commHdl(hdl);
	// ignore packets from myself
	if (resp->src == PLAYER_ME) {
		err = ipx2_RES_EMPTY;
	}
	resp->status = err;

	return (err == ipx2_RES_OK);
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
	int					tmpInt[sizeof(nw_adr_t)];
	int					i;

	if (req == NULL)
		req = (commScanAddrReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	DPRINT(("@IPX commScanAddr(%s): ", req->printable));

	if (req->size < sizeof(nw_adr_t)) {
		resp->status = ipx2_RES_FULL;
		return FALSE;
	}

	resp->status = ipx2_RES_OK;

	if (sscanf(req->printable, "%u.%u.%u.%u,%X.%X.%X.%X.%X.%X",
				&tmpInt[0], &tmpInt[1], &tmpInt[2], &tmpInt[3],
				&tmpInt[4], &tmpInt[5], &tmpInt[6], &tmpInt[7],
				&tmpInt[8], &tmpInt[9])
			!= 10)
		return FALSE;

	for (i = sizeof(nw_netAdr_t); i--; )
		((nw_adr_t *) req->address)->net[i] = tmpInt[i];
	for (i = sizeof(nw_nodeAdr_t); i--; )
		((nw_adr_t *) req->address)->node[i] = tmpInt[i+sizeof(nw_netAdr_t)];

	resp->length = sizeof(nw_adr_t);

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
	nw_adr_t			*params;
	char				printable[80];

	DPRINT(("@IPX commPrintAddr(): "));

	if (req == NULL)
		req = (commPrintAddrReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = ipx2_RES_OK;

	if (req->length != sizeof(*params))
		return FALSE;

	params = (nw_adr_t *)req->address;

	sprintf(printable, "%u.%u.%u.%u,%X.%X.%X.%X.%X.%X",
		params->net[0]&0xff,
		params->net[1]&0xff,
		params->net[2]&0xff,
		params->net[3]&0xff,
		params->node[0]&0xff,
		params->node[1]&0xff,
		params->node[2]&0xff,
		params->node[3]&0xff,
		params->node[4]&0xff,
		params->node[5]&0xff);

	if (req->printable != NULL) {
		if (req->size < strlen(printable) + 1) {
			resp->status = ipx2_RES_FULL;
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

	DPRINT(("@IPX commGroupAlloc(): "));

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

	DPRINT(("@IPX commGroupFree(): "));

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

	DPRINT(("@IPX commGroupAdd(): "));

	if (req == NULL)
		req = (commGroupAddReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}

#if 0
/*
 *	Subtract one or more players from a group.  Do not delete the group,
 *	even if it is left empty.
 *
 *	Return TRUE unless there was a problem subtracting one or more players.
 */

int
commGroupSubtract(
	commGroupSubtractReq_t *	req,	// Request
	commGroupSubtractResp_t *	resp)	// Response
{
	commGroupSubtractReq_t	reqDummy;
	commGroupSubtractResp_t	respDummy;

	DPRINT(("@IPX commGroupSubtract(): "));

	if (req == NULL)
		req = (commGroupSubtractReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return FALSE;
}
#else
int
commSetParam(
	commSetParamReq_t *	req,	// Request
	commSetParamResp_t *	resp)	// Response
{
	commSetParamResp_t	respDummy;
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
	nw_adr_t			*params;
	ipx2_hdl_t			h;

	DPRINT(("@IPX commSayHi(): "));

	if (req == NULL)
		req = (commSayHiReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	params = (nw_adr_t *)req->address;

	if (req->length != sizeof(*params)) {
		resp->status = ipx2_RES_BAD;
		return FALSE;
	}

	h = ipx2_adr2hdl( ipx, params, TRUE );

	resp->player = ipx2commHdl(h);

	if (h == ipx2_HDL_NONE) {
		resp->status = ipx2_RES_BAD;
		return FALSE;
	}

	resp->status = ipx2_RES_OK;
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
	commSayByeReq_t			reqDummy;
	commSayByeResp_t		respDummy;

	DPRINT(("@IPX commSayBye(): "));

	if (req == NULL)
		req = (commSayByeReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = 0;

	return TRUE;
}

#pragma export off
