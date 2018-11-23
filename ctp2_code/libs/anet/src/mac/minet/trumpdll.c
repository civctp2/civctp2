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
 Adapter layer to convert OpenTransport UDP into a shared library suitable
 for use with Activenet.
 (Sources originally copied from Trumpet driver for DOS Netmech.)

 $Log: trumpdll.c $
 Revision 1.2  1997/08/22 18:20:05  anitalee
 changes made by Danny Swarzman.

 1997/08/05 dswarzman
 Fixed system includes.
 Initial revision
 Revision 1.1  1997/05/31 22:56:11  dkegel
 Initial revision
 Revision 1.1  1997/05/30 18:12:45  dkegel
 Initial revision
---------------------------------------------------------------------------*/

#include <OpenTransport.h>
#include <OpenTptInternet.h>
#include <Strings.h>
#include <TextUtils.h>
#include "ddprint.h"

#include "commapi.h"
#include "assoctab.h"
//#include <tcpabi/tcpabi.h>
//#include <tcpabi/tcpabix1.h>
#include <stdio.h>		// for sprintf and sscanf
//#include <i86.h>
#define  NO_EXT_KEYS
#include <string.h>

#include "commapi.h"
#include "trumpdll.h"
#include "trumpudp.h"
#include "otudp.h"
#include "otutils.h"

#include "HostDialog.h"

#define DPMI_INT 		0x31
#define MAX_RAW_PKTLEN	512

#ifndef TRUE
    #define TRUE        (1)
    #define FALSE       (0)
#endif

#pragma export on

comm_driverInfo_t gCommDriverInfo =
{
	comm_DRIVER_SIGNATURE,
	sizeof(comm_driverInfo_t),
	"OT Internet",
	0x0100,
	(comm_DRIVER_IS_VISIBLE
	| comm_DRIVER_ALLOWS_GAMELIST),
	(comm_INIT_NEEDS_UI
	/*	| comm_INIT_NEEDS_SESSION_NAME	*/	)
};

#pragma export off

EndpointRef			gUDPEndpoint;
InetInterfaceInfo	gInetInfo;
InetSvcRef			gInetService;

static trump_hdl_t hdl_rx = trump_HDL_NONE;	// Listen for incoming pkts on this hdl
static assoctab_t *peertab = NULL;			// Maps hdls to adrs and back
static void* scratch_flat;

static playerHdl_t trump2commHdl( trump_hdl_t h )
{
	switch ( h ) {
	case trump_HDL_NONE:		return PLAYER_NONE;
	case trump_HDL_ME:			return PLAYER_ME;
	default:
		;
	}

	return ((playerHdl_t) h);
}

static trump_hdl_t commHdl2trump (playerHdl_t h )
{
	switch ( h ) {
	case PLAYER_NONE:		return trump_HDL_NONE;
	case PLAYER_ME:			return trump_HDL_ME;
	case PLAYER_BROADCAST:	return trump_HDL_NONE;
	case PLAYER_UNKNOWN:	return trump_HDL_NONE;
	default:
		;
	}

	return ((trump_hdl_t) h);
}

void printAdr(ip_adr_t adr)
{
	DPRINT(("%d.%d.%d.%d",
		((char *)&adr)[0],
		((char *)&adr)[1],
		((char *)&adr)[2],
		((char *)&adr)[3]));
	(void) adr;
}

/*-----------------------------------------------------------------------
 Dump the player table to the log file.
-----------------------------------------------------------------------*/
static void dprint_peertab(assoctab_t *p)
{
#if defined(DPRNT) || defined(DEBUG)
	int i;
	for (i=0; i < p->n_used; i++) {
		assoctab_item_t	*ip			 = assoctab_getkey(p, i);
		trump_hdl_t		h			 = (trump_hdl_t) ip->key;
		ip_adr_t		*pAdr		 = (ip_adr_t *)   ip->value;
		DPRINT(("p[%d] = ", h));
		printAdr(*pAdr);
		DPRINT((", at %p\n", pAdr));
	}
#else
	(void)p;
#endif
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
          return trump_HDL_NONE
 Multiple calls with the same arg will return the same result.
 There is no upper bound on handles.
 Returns trump_HDL_NONE on failure.
-----------------------------------------------------------------------*/
trump_hdl_t trump_adr2hdl(ip_adr_t remoteadr, short localport, short remoteport, int insert)
{
	// Search peer tab.  If not found, append it.
	trump_hdl_t h;
	ip_adr_t *peer;
	int i;
	unsigned int lport;
	OSStatus	err;

#if 0
	DPRINT(("trump_adr2hdl: adr "));
	printAdr(remoteadr);
	DPRINT((", dest_port %d, localport %d, insert %d\n", remoteport, localport, insert));
#endif

	// Look for existing address/handle
	for (i=0; i<peertab->n_used; i++) {
		assoctab_item_t *pe = assoctab_getkey(peertab, i);
		if (!pe)
			DPRINT(("trump_adr2hdl: couldn't find entry\n"));
		else {
			peer = (ip_adr_t *)pe->value;
			//DPRINT(("trump_adr2hdl: pe->value %p, *pe->value %x, remoteadr %x\n",
				//pe->value, *peer, remoteadr));
			if (memcmp(peer, &remoteadr, sizeof(ip_adr_t)) == 0) {
				//DPRINT(("trump_adr2hdl: found match, hdl %d\n", pe->key));
				return pe->key;
			}
		}
	}
	if (!insert) return trump_HDL_NONE;

	// Open new handle for this address
	h = tcpabi_udp_open(remoteadr, remoteport, localport, UDP_OPEN_NORMAL, &lport);
	if (h == trump_HDL_NONE)
		return h;

	// Save address
	peer = (ip_adr_t *)assoctab_subscript_grow(peertab, h);
	if (!peer) {
		// Abort! Out of memory!
		return trump_HDL_NONE;
	}
	dprint_peertab(peertab);
	//DPRINT(("trump_adr2hdl: saving %x at %p\n", remoteadr, peer));
	*peer = remoteadr;
	//dprint_peertab(peertab);
	//DPRINT(("trump_adr2hdl: udp_open successful. localport %d\n", lport));
	return h;
}

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding IP address.
 Returns comm_STATUS_EMPTY on failure.
-----------------------------------------------------------------------*/
comm_status_t trump_hdl2adr(trump_hdl_t hdl, ip_adr_t *adr)
{
	ip_adr_t *peer;

	peer = (ip_adr_t *)assoctab_subscript(peertab, hdl);
	if (!peer) {
		DPRINT(("trump_hdl2adr: couldn't find peer %d\n", hdl));
		dprint_peertab(peertab);
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

	DPRINT(("@TRUMP commNoOp(): "));

	if (req == NULL)
		req = (commNoOpReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	resp->status = comm_STATUS_OK;

	DPRINT(("@\n"));

	return (TRUE);
}

Boolean commNeedsUI(commInitReq_t* req) {

	//	if this is a resume, we don't need the UI

	if (req->flags & comm_INIT_FLAGS_RESUME) {
		return false;
	} else {
		return true;
	}

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
	ip_adr_t adr_broadcast;
	ip_adr_t *padr;
	OSStatus			err;
	extern Str255	gSavedSelection;
//	DebugStr ( "\pcommInit()" );
	DPRINT(("@TRUMP commInit(): "));

	if (req == NULL)
		req = (commInitReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	scratch_flat = malloc(MAX_RAW_PKTLEN);
	//scratch_flat = my_dos_malloc(MAX_RAW_PKTLEN, &scratch_seg, &scratch_off, &scratch_selector);
	if (!scratch_flat) {
		DPRINT(("commInit: couldn't allocate DOS memory!\n"));
		resp->status = comm_STATUS_BAD;
		return FALSE;
	}

	//	check for the existanct of OpenTransport
	if(!OpenTransportExists() || !OpenTransportInetExists()) {
		//DebugStr("\pOpen Transport Does Not Exist");
		DPRINT(("commInit: UDP ABI not found\n"));
		resp->status = comm_STATUS_NETWORK_NOT_PRESENT;
		free(scratch_flat);
		return FALSE;
	}

	//	Initialize OpenTransport
	err = InitOpenTransport();
	if (err != noErr) {
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		return FALSE;
	}

	//	initialize Internet Services

	gInetService = OTOpenInternetServices(kDefaultInternetServicesPath, 0, &err);
	if (err != noErr) {
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		return FALSE;
	}

	//	open an endpoint for sending and recieving data

	err = CreateAndConfigUDP(&gUDPEndpoint);
	if (err != noErr) {
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);

		ShutDownUDP();

		return FALSE;
	}

	//	create the peer table

	peertab = assoctab_create(sizeof(ip_adr_t));
	if (!peertab) {
		// ABORT! Out of Memory
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);

		ShutDownUDP();

		return FALSE;
	}

	//	Get information about the Internet

	err = OTInetGetInterfaceInfo(&gInetInfo, kDefaultInetInterface);
	if (err != noErr) {
		//DebugStr("\pCannot Get Information About Default Interface");
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);

		ShutDownUDP();

		return FALSE;
	}

	// Store our address in the peer table under the bogus ME handle
	padr = (ip_adr_t *)assoctab_subscript_grow(peertab, trump_HDL_ME);
	if (!padr) {
		DPRINT(("commInit: couldn't grow peer table\n"));
		resp->status = comm_STATUS_BAD;
		free(scratch_flat);
		assoctab_destroy(peertab);

		ShutDownUDP();

		return FALSE;
	}
	dprint_peertab(peertab);
	DPRINT(("commInit: saving handle %d adr %x at %p\n", trump_HDL_ME, gInetInfo.fAddress, padr));
	memcpy(padr, &gInetInfo.fAddress, sizeof(ip_adr_t));
	dprint_peertab(peertab);

	// Open a handle good for receiving packets on the standard port
	// SRC = (*,*)
	// DEST = (localhost, pt->port)
	adr_broadcast = 0xffffffff;
	hdl_rx = trump_adr2hdl(adr_broadcast, SOCKET_MW2, SOCKET_MW2, TRUE);
	if (hdl_rx == trump_HDL_NONE) {
		DPRINT(("commInit: couldn't open handle for listening\n"));
		resp->status = comm_STATUS_BUSY;
		free(scratch_flat);
		assoctab_destroy(peertab);

		ShutDownUDP();

		return FALSE;
	}
	/*
	if ((req->flags & comm_INIT_FLAGS_RESUME) == 0) {
		if (!DoHostListDialog()) {
			resp->status = comm_STATUS_EMPTY;
			free(scratch_flat);
			assoctab_destroy(peertab);

			ShutDownUDP();

			return FALSE;
		}
	} else {

		//	we need to load the last string selected when we are resuming

		OpenPrefsFile();
		p2cstr(gSavedSelection);		//	the apps like C-strings
		ClosePrefsFile();

	}
	*/
	//	initialize our address list to nothing

	InitAddressList();

	//	add our own address to the beginning of the list (it MUST Be the first
	//	address in our list - we broadcast to all _other_ addresses in our list)

	AddAddressToList(gInetInfo.fAddress);

	//	add the address from the dialog to our broadcast list if the user chose one

	if (gSavedSelection[0] != 0) {
		InetHostInfo	theHostInfo;

		OTSetSynchronous(gInetService);
		err = OTInetStringToAddress(gInetService, gSavedSelection, &theHostInfo);
		OTSetAsynchronous(gInetService);

		if (err == noErr) {
			AddAddressToList(theHostInfo.addrs[0]);
		}
	}

	resp->status = comm_STATUS_OK;
	DPRINT(("commInit: success\n"));
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

	DPRINT(("@TRUMP commTerm(): "));

	if (req == NULL)
		req = (commTermReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	if (peertab) {
		int i;
		for (i=0; i<peertab->n_used; i++) {
			assoctab_item_t *pe = assoctab_getkey(peertab, i);
			if (!pe)
				DPRINT(("commTerm: couldn't find entry\n"));
			else if (pe->key != trump_HDL_ME) {
				DPRINT(("commTerm: closing handle %d\n", pe->key));
				tcpabi_udp_close(pe->key, UDP_CLOSE_NORMAL);
			} else {
				DPRINT(("commTerm: not closing handle %d\n", pe->key));
			}
		}
		assoctab_destroy(peertab);
	}
	free(scratch_flat);
	resp->status = comm_STATUS_OK;
	DPRINT(("commTerm: success\n"));

	//	unbind our endpoint and close open transport

	ShutDownUDP();

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
	static ip_adr_t 		adr;
	comm_status_t			err;

	if (req == NULL)
		req = (commPlayerInfoReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	DPRINT(("commPlayerInfo: player %d\n", req->player));

	// This comm layer doesn't know about user names.
	resp->name = "";

	err = trump_hdl2adr(commHdl2trump(req->player), &adr );
	if (err != comm_STATUS_OK) {
		DPRINT(("commPlayerInfo: trump_hdl2adr returns %d\n", err));
		resp->status = err;
		return FALSE;
	}

	resp->address = (void*) &adr;
	resp->addrLen = sizeof(ip_adr_t);
	memset(&resp->nodeID, 0, sizeof(resp->nodeID)); // node id is not supported
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

	DPRINT(("@TRUMP commTxFull(): "));

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
	trump_hdl_t		h;

	if (req == NULL)
		req = (commTxPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

/*
#define NO_BROADCAST		// Uncomment this line to simulate WAN lacking broadcast
#ifdef NO_BROADCAST
	if (req->dest == PLAYER_BROADCAST) {
		//	only used for enumSessions packets
		resp->status = comm_STATUS_OK;
		return TRUE;
	}
#endif
*/

	if (req->dest == PLAYER_BROADCAST) {
		err = tcpabi_udp_broadcast(req->buffer, req->length);
	} else {
		h = commHdl2trump(req->dest);
		err = tcpabi_udp_send(h, req->buffer, req->length, TTLTOS_NORMAL, 0, UDP_SEND_NORMAL);
	}

	if (err == -1) {
		DPRINT(("commTxPkt error sending to trump hdl %d, _tcpabi_errno %d\n", h, err));
		resp->status = comm_STATUS_FULL;
	}
	else
		resp->status = comm_STATUS_OK;

	return (resp->status == comm_STATUS_OK);
}
/*
 * Dummy routine. This isn't really used.
 *
*/
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

	DPRINT(("@TRUMP commPeekPkt(): "));

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
	tcpabi_session_info_t *info;
	int len;

	//DPRINT(("@TRUMP commRxPkt(): "));

	if (req == NULL)
		req = (commRxPktReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	len = tcpabi_udp_recv(hdl_rx, req->buffer, req->size, 0, UDP_RECV_NORMAL, TTLTOS_NORMAL, NULL);
	if (len == -1) {
		resp->status = comm_STATUS_EMPTY;
		return FALSE;
	}

	resp->length = len;
	tcpabi_udp_status(hdl_rx, 0, NULL, &info);
	resp->src = trump2commHdl(trump_adr2hdl(info->ip_dst, 0, 0, FALSE));
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
	ip_adr_t* 			adr;
	OSStatus			err;
	InetHostInfo		theHost;

	if (req == NULL)
		req = (commScanAddrReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	DPRINT(("@TRUMP commScanAddr(%s):\n", req->printable));

	if (req->size < sizeof(ip_adr_t)) {
		resp->status = comm_STATUS_FULL;
		return FALSE;
	}

	adr = (ip_adr_t*) req->address;

	//	convert the dotted decimal address or host name to an IP address (we do
	//	this sync.)

	OTSetSynchronous(gInetService);
	err = OTInetStringToAddress(gInetService, req->printable, &theHost);
	OTSetAsynchronous(gInetService);
	if (err != noErr) {

		//{
		//	Str255	theString;
		//
		//	NumToString(err, theString);
		//	DebugStr(theString);
		//}

		DPRINT(("@TRUMP commScanAddr: resolve failed!\n"));
		resp->status = comm_STATUS_BAD;
	    return FALSE;
	}

	*adr = theHost.addrs[0];		//	we only want the first address

	//	return the length of the address

	resp->length = sizeof(ip_adr_t);
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

	DPRINT(("@TRUMP commPrintAddr(): "));

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

	DPRINT(("@TRUMP commGroupAlloc(): "));

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

	DPRINT(("@TRUMP commGroupFree(): "));

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

	DPRINT(("@TRUMP commGroupAdd(): "));

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

	DPRINT(("@TRUMP commGroupSubtract(): "));

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
	ip_adr_t			*params;
	trump_hdl_t			h;

	DPRINT(("@TRUMP commSayHi(): "));

	if (req == NULL)
		req = (commSayHiReq_t *)memset(&reqDummy, 0, sizeof(*req));
	if (resp == NULL)
		resp = &respDummy;

	params = (ip_adr_t *)req->address;

	if (req->length != sizeof(*params)) {
		resp->status = comm_STATUS_BAD;
		DPRINT(("commSayHi: bad request length!\n"));
		return FALSE;
	}

	h = trump_adr2hdl(*params, SOCKET_MW2, SOCKET_MW2, TRUE);

	resp->player = trump2commHdl(h);

	if (h == trump_HDL_NONE) {
		resp->status = comm_STATUS_BAD;
		DPRINT(("commSayHi: trump_adr2hdl fails\n"));
		return FALSE;
	}

	resp->status = comm_STATUS_OK;
	DPRINT(("commSayHi: trump_adr2hdl returns %d; trump2commHdl returns %d\n",
		h, resp->player));
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
	trump_hdl_t h;
	comm_status_t err;
	int err0;

	DPRINT(("@TRUMP commSayBye(): "));

    if (resp == NULL) resp = &respDummy;
    if (req == NULL) {
		resp->status = 1;
		return FALSE;
	}
	h = commHdl2trump(req->player);

	err0 = assoctab_subscript_delete(peertab, h);

	err = tcpabi_udp_close(h, UDP_CLOSE_NORMAL);
	DPRINT(("commSayBye: player %d, hdl %d, delete returns %d, tcpabi_udp_close returns %d\n", req->player, h, err0, err));
	if (err != TCPABI_NO_ERROR) {
		DPRINT(("commSayBye: tcpabi_udp_close returns %d\n", err));
		resp->status = 2;
		return FALSE;
	}

    resp->status = 0;
    return (TRUE);
}

#pragma export off
