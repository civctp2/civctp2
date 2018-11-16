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

/*-----------------------------------------------------------------------
 $Log: ipx2.c $
 Revision 1.2  1997/06/02 01:27:51  dkegel
 Fix the ol' "your receive buffer is too big" usability bug.
 Revision 1.1  1997/05/30 18:27:13  dkegel
 Initial revision
 Revision 1.4  1996/02/16 18:12:18  dkegel
 demonstrate in the self test that one may do multiple puts in a row.
 Revision 1.3  1996/02/13 20:15:38  dkegel
 Added way to report error cause in ipx2_create.
 It's a kludge, but it helps.
 Revision 1.2  1996/02/12 19:08:29  dkegel
 Improved method descriptions.
 Revision 1.1  1996/02/11 22:54:57  dkegel
 Initial revision

 A simple IPX driver.  Really, a library that makes calling the
 real mode IPX driver easy from protected mode.
-----------------------------------------------------------------------*/
#include <stdlib.h>
//#include <stdio.h>
#include "ipx2.h"
#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 0

#include "ipx2.h"

#include "ipxcalls.h"
#include "ipxerror.h"
#include "ipxpb.h"
#include "rip.h"
#include "IpxTransport.h"

#define DEBUG_MODULE 0

#if 0
// IPX function numbers, parameters, and results, kinda.
#define IPX_SPX_OpenSocket			0x00
#define IPX_SPX_CloseSocket			0x01
#define IPX_GetLocalTarget			0x02
#define IPX_SendPacket				0x03
#define IPX_ListenForPacket			0x04
#define IPX_GetInternetworkAddress	0x09
#define IPX_RelinquishControl		0x0A

#define LONG_LIVED_SOCKET			0xFF
#define SHORT_LIVED_SOCKET			0x00
#define SELECT_A_SOCKET				0x0000	// Let IPX select a socket number

// DOS function numbers, parameters, and results, kinda.
#define MULTIPLEX_INT				0x2F
#define  IPX_MULTIPLEX_FUNCCODE			0x7A
#define   IPX_INSTALLED						0xFF
#define   IPX_NOT_INSTALLED					0x00

#endif

#if 0	// Meaning of the ECB in-use flag
  0x00 : ECB not in use
  0xE0 : AES temporary indicator
  0xF8 : IPX was a critical process so it queued the send ECB
  0xFA : IPX is processing the ECB
  0xFB : Send/Receive occurred; ECB awaits processing
  0xFC : AES waiting
  0xFD : Event scheduled; IPX awaits time expiration interval
  0xFE : IPX is listening for incoming packets on the socket
  0xFF : IPX is using this ECB to send a packet
#endif

#if 0	// Meaning of the listening ECB completion code
  0x00 : Success; a packet was received
  0xFC : Failure; the ECB was canceled
  0xFD : Failure; a packet overflow occurred
  0xFF : Failure; the ECB-specified socket is not open
#endif

#if 0	// Meaning of the sending ECB completion code
  0x00 : Successfully sent; not necessarily received
  0xFC : Failure; the ECB was canceled
  0xFD : Failure; invalid packet fragmentation or size
  0xFE : Failure; the packet is not deliverable
  0xFF : Failure; network or hardware problems
#endif

#if 0		//	HMS - don't need PC regs on Mac
typedef	struct {		// Lots of system registers in one convenient place
	union REGS		r;
	struct SREGS	s;
} regs_t;

typedef struct rminfo {

 long EDI;
 long ESI;
 long EBP;
 long reserved_by_system;
 long EBX;
 long EDX;
 long ECX;
 long EAX;
 short flags;
 short ES,DS,FS,GS,IP,CS,SP,SS;
} rmregs_t;

#endif

#if 1
	#define printRMI(a, b)
	#define printNode(pn)
	#define printAdr(pa)
	#define printHdr(ph)
	#define printECB(p)
#endif

IOCompletionUPP	ipx2_send_callback_UPP = nil;
IOCompletionUPP	ipx2_recieve_callback_UPP = nil;

pascal void ipx2_recieve_callback(IPX_ECB *ecb) {
	ecb->UserWorkspace[0] = 0x00;				//	ECB has valid data in it
}

pascal void ipx2_send_callback(IPX_ECB *ecb) {
	ecb->UserWorkspace[0] = 0x00;				//	ECB is no longer in use
}

/*-----------------------------------------------------------------------
 Create an IPX driver.
 Returns NULL on failure.
 Parameter is socket number (analogous to UDP/IP port number) to
 use for all traffic.
 Only one of these may exist at a time, since multiple ones would
 compete for the same source of rx packets.
-----------------------------------------------------------------------*/
ipx2_t *ipx2_create(int socket, int *status)
{
	ipx2_t	*ipx;
	ipx2_hdl_t bh;
	nw_adr_t ba;
	int i;
	//regs_t	regs;
	//nw_segptr_t spOut;
	char *fpOut;
	ipx2_result_t stat;
	short		localSocket = socket;

	//	set up the UPPs for the callbacks

	ipx2_send_callback_UPP = NewIOCompletionProc(ipx2_send_callback);
	if (ipx2_send_callback_UPP == nil) {
		return nil;
	}

	ipx2_recieve_callback_UPP = NewIOCompletionProc(ipx2_recieve_callback);
	if (ipx2_recieve_callback_UPP == nil) {
		return nil;
	}

	ipx = (ipx2_t *)malloc(sizeof(ipx2_t));
	if (!ipx) {
		*status = 100;
		// ABORT! Out of Memory
		return NULL;
	}
	memset(ipx, 0, sizeof(ipx2_t));
	// Init dynamic things to NULL early for benefit of ipx2_destroy
	ipx->peers = NULL;
	ipx->rx = ipx->tx = NULL;

	stat = IpxOpenSocket((unsigned short *)&localSocket, (unsigned char) WANT_BROADCASTS);
	if (stat != ipx2_RES_OK) {
		//printf("create: couldn't call IPX\n");
		free(ipx);
		*status = 120;
		return NULL;
	}

	ipx->socket = localSocket;

	ipx->peers = dynatab_create(sizeof(ipx2_peer_t));
	if (!ipx->peers) {
		// ABORT! Out of Memory
		ipx2_destroy(ipx);
		*status = 140;
		return NULL;
	}
	//printf("allocating broadcast handle:\n");
	// Allocate the broadcast 'peer' handle
	for (i=0; i<sizeof(ba.net); i++)
		ba.net[i] = 0;
	for (i=0; i<sizeof(ba.node); i++)
		ba.node[i] = 0xff;
	ipx->rx = ipxq_create(localSocket, &ba, ipx2_recieve_callback_UPP);

	bh = ipx2_adr2hdl(ipx, &ba, 1);

	// Make sure it's what we expect
	if (bh != ipx2_HDL_BROADCAST) {
		// ABORT! Bug in allocating broadcast handle.
		ipx2_destroy(ipx);
		*status = 150;
		return NULL;
	}

	//printf("allocating the me handle:\n");
	// Allocate the me handle.
	fpOut = (char *)ipx->rx->scratch.flat.p;

	IpxGetInternetworkAddress((unsigned char *)fpOut);
	bh = ipx2_adr2hdl(ipx, (nw_adr_t *)fpOut, 1);
	//printf("My address is: ");
	//printAdr((nw_adr_t *)fpOut);
	//printf(", my handle is %d\n",bh);

	// Make sure it's what we expect
	if (bh != ipx2_HDL_ME) {
		// ABORT! Bug in allocating my handle.
		ipx2_destroy(ipx);
		*status = 160;
		return NULL;
	}

	// Allocate IPX ECB/header/buffer queues.  Default src is this host
	// (for benefit of tx queue).
	ipx->tx = ipxq_create(localSocket, (nw_adr_t *)fpOut, ipx2_send_callback_UPP);

	if (!ipx->rx || !ipx->tx) {
		// ABORT! Out of Memory
		ipx2_destroy(ipx);
		*status = 170;
		return NULL;
	}
	*status = 0;

	return ipx;
}

/*-----------------------------------------------------------------------
 Destroy an IPX driver.
-----------------------------------------------------------------------*/
void ipx2_destroy(ipx2_t *ipx)
{
	// First, close socket- else you'll crash so fast it'll make your disk spin
	//printf("ipx2_destroy...\n");
	IpxCloseSocket(ipx->socket);

	if (ipx->peers) dynatab_destroy(ipx->peers);
	if (ipx->rx) ipxq_destroy(ipx->rx);
	if (ipx->tx) ipxq_destroy(ipx->tx);
	free(ipx);
}

/*-----------------------------------------------------------------------
 Given an IPX address, return a handle that can be used to send an
 address to that address.  Handles are integers starting at 0.
 If no handle exists for that address,
	 If insert is non-zero,
	  allocate one.
	 else
	  return ipx2_HDL_NONE
 Multiple calls with the same arg will return the same result.
 There is no upper bound on handles.
 (There is no way provided to close handles, and no particular need,
 since nobody will be creating more than 100 or so, and they only take up
 16 bytes each.)
 Returns ipx2_HDL_NONE on failure.
-----------------------------------------------------------------------*/
ipx2_hdl_t ipx2_adr2hdl(ipx2_t *ipx, nw_adr_t *adr, int insert) {
	// Search peer tab.  If not found, append it.
	int n;
	ipx2_hdl_t h;
	ipx2_peer_t *peer;
	//nw_segptr_t spIn;
	//nw_segptr_t spOut;
	char *fpIn;
	char *fpOut;
	//regs_t					regs;
	unsigned long		targetTime;
	IPX_ECB						temp_ECB;
	IpxNetAddress			targetAddress;

	//printf("ipx2_adr2hdl...\n");
	peer = (ipx2_peer_t *)dynatab_subscript(ipx->peers, 0);
	n = ipx->peers->n_used;
	for (h=0; h<n; h++, peer++)
		if (memcmp(&peer->dest, adr, sizeof(nw_adr_t)) == 0) return h;
	h = n;
	if (!insert) return ipx2_HDL_NONE;
	// Save address
	peer = (ipx2_peer_t *)dynatab_subscript_grow(ipx->peers, h);
	if (!peer) {
		// Abort! Out of memory!
		return ipx2_HDL_NONE;
	}
	memcpy(&peer->dest, adr, sizeof(nw_adr_t));

	memcpy(&targetAddress.netNode, adr, sizeof(nw_adr_t));
	targetAddress.socket = ipx->socket;

	IpxGetLocalTarget((unsigned char*) &targetAddress, &temp_ECB, &targetTime);
	memcpy(peer->immedAddress, temp_ECB.immediateAddress, sizeof(nw_nodeAdr_t));
	return h;
}

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding IPX address.
 Returns ipx2_RES_EMPTY on failure.
-----------------------------------------------------------------------*/
ipx2_result_t ipx2_hdl2adr(ipx2_t *ipx, ipx2_hdl_t hdl, nw_adr_t *adr)
{
	ipx2_peer_t *peer;
	//printf("ipx2_hdl2adr:\n");
	peer = (ipx2_peer_t *)dynatab_subscript(ipx->peers, hdl);
	if (!peer) {
		return ipx2_RES_EMPTY;
	}
	memcpy(adr, &peer->dest, sizeof(nw_adr_t));
	return ipx2_RES_OK;
}

/*-----------------------------------------------------------------------
 Send a packet via IPX to the given destination.
 Copies len bytes from buf into internal packet queue.
 Must have previously gotten handle for destination by calling ipx2_adr2hdl.
-----------------------------------------------------------------------*/
ipx2_result_t ipx2_put(ipx2_t *ipx, void *buf, size_t len, ipx2_hdl_t dest)
{
	ipxq_pkt_t *pp;
	ipx2_peer_t *peer = (ipx2_peer_t *)dynatab_subscript(ipx->peers, dest);
	//regs_t	regs;

	//printf("ipx2_put:\n");
	if (len < 1 || len > ipxq_MAX_USERDATALEN || !buf || !peer) {
		return ipx2_RES_BAD;
	}
	pp = ipxq_get(ipx->tx);
	if (!pp) {
		return ipx2_RES_FULL;
	}
	memcpy(pp->ecb.flat.e->immediateAddress, peer->immedAddress, sizeof(nw_nodeAdr_t));
	pp->ecb.flat.e->fragList[1].fragSize = len;

	memcpy(pp->hdr_n_buf.flat.h + 1, buf, len);
	//pp->hdr_n_buf.flat.h->DestinationSocket =  NativeToBigEndian(16, ipx->socket);
	memcpy(&(pp->hdr_n_buf.flat.h->destNet), &peer->dest, sizeof(nw_adr_t));		//	net, node
	//memcpy(&pp->hdr_n_buf.flat.h->src, &ipx->myAdr, sizeof(nw_adr_t));
	//pp->hdr_n_buf.flat.h->PacketType = 4; // ??
	//pp->hdr_n_buf.flat.h->Length = NativeToBigEndian(len, 16);
	//memset(&pp->ecb.flat.e->LinkAddress, 0, sizeof(nw_segptr_t));

	// Set CC to a bogus value so we can see if IPX is ever setting it.
	//pp->ecb.flat.e->CompletionCode = 0x77;
	pp->ecb.flat.e->UserWorkspace[0] = 0x77;					//	in use for send

	//printf("SendPacket: Before ipx2_call:\n");
	//printECB(pp);
	//printHdr(pp->hdr_n_buf.flat.h);
	IpxSend(ipx->socket, (IPX_ECB *)pp->ecb.flat.p);
	//printf("After ipx2_call:\n");
	//printECB(pp);

	ipxq_put(ipx->tx);
	//if (pp->ecb.flat.e->CompletionCode != 0x00) {
		//printf("ipx2_put: managed to get nonzero completion code!\n");
		;
	//}
	//printf("\n");
	return ipx2_RES_OK;
}

/*-----------------------------------------------------------------------
 Receive a packet via IPX.
 Caller must put size of buffer in len before calling.
 Packets larger than buffer will be silently ignored.
 On failure, returns
 	ipx2_RES_BAD if *len looked bogus, or buf or srcHdl were NULL,
				 or packet was too big to fit in buffer, or 0 size.
	ipx2_RES_EMPTY if no good packet was availible.
 On success,
 	returns ipx2_RES_OK,
	sets *len to the packet length,
	sets *srcHdl to the handle of the sender (or ipx2_HDL_NONE if one
		hasn't been assigned yet with ipx2_adr2hdl), and
	sets *srcAdr to the netware address of the sender if srcAdr is not NULL.

 To reply if source was ipx2_HDL_NONE, you must first call ipx2_adr2hdl
 to turn the source's address into a handle.
 (ipx2_get could do this for you, but then the peer table might fill up
 with all sorts of garbage addresses from hosts who sent you junk mail.)
-----------------------------------------------------------------------*/
ipx2_result_t ipx2_get(ipx2_t *ipx, void *buf, size_t *plen, ipx2_hdl_t *srcHdl, nw_adr_t *srcAdr)
{
	ipx2_result_t stat;
	ipxq_pkt_t *pp;
	size_t len;
	//regs_t	regs;
	int maxloops = ipxq_MAXELS;

	//printf("ipx2_get:\n");
	if (*plen < 1 || *plen > (10 * ipxq_MAX_USERDATALEN) || !buf || !srcHdl) {
		return ipx2_RES_BAD;
	}
	IpxRelinquishControl();
	stat = ipx2_RES_EMPTY;
	// Get packets until we find one with data in it.
	// Throw all the packets back on the queue after telling IPX
	// to listen on them.
	// Note: ipxq_get won't hand us packets that aren't done.
	while (maxloops-- && (pp = ipxq_get(ipx->rx)) != NULL) {
		//printf("ipx2_get: Got packet, InUse %d, cc %x\n", pp->ecb.flat.e->InUseFlag, pp->ecb.flat.e->CompletionCode);
		if (pp->ecb.flat.e->status == 0) {
			// A packet with data in it!
			//len = pp->ecb.flat.e->FragmentDescriptor[1].Length; wrong
			//len = pp->hdr_n_buf.flat.h->Length; wrong
			//len = NativeToBigEndian(16, pp->hdr_n_buf.flat.h->Length); wrong
			len = pp->hdr_n_buf.flat.h->packetLen - sizeof(nw_hdr_t);

			//printf("get: len %d, src= ", len);
			//printAdr(&pp->hdr_n_buf.flat.h->src);
			//printf("\n");
			if (len < 1 || len > ipxq_MAX_USERDATALEN || len > *plen) {
				stat = ipx2_RES_BAD;
			} else {
				memcpy(buf, pp->hdr_n_buf.flat.h + 1, len);
				*plen = len;
				*srcHdl = ipx2_adr2hdl(ipx, (nw_adr_t*) &pp->hdr_n_buf.flat.h->sourceNet, 0);
				if (srcAdr)
					memcpy(srcAdr, &pp->hdr_n_buf.flat.h->sourceNet, sizeof(nw_adr_t));
				stat = ipx2_RES_OK;
			}
		}
		// Set CC to a bogus value so we can see if IPX is ever setting it.
		//pp->ecb.flat.e->CompletionCode = 0x88;
		pp->ecb.flat.e->UserWorkspace[0] = 0x88;					//	in use for recieve

		pp->ecb.flat.e->fragList[1].fragSize = ipxq_MAX_USERDATALEN;
		//printf("ListenForPacket: Before ipx2_call:\n");
		//printECB(pp);
		//printHdr(pp->hdr_n_buf.flat.h);
		stat = IpxReceive(ipx->socket, pp->ecb.flat.e);
		// AL		Completion code
		//  0x00	 Success
		//  0xFF	 Nonexistent listening socket
		if (ipxq_put(ipx->rx) == NULL) {
			// ABORT! Somehow, the packet wouldn't fit back into the queue
			return ipx2_RES_BAD;
		}
		//printf("After ipx2_call:\n");
		//printECB(pp);

		// Only get one packet at a time.
		if (stat == ipx2_RES_OK) break;
	}
	//printf("\n");
	return stat;
}
