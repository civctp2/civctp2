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
 $Log: Ipxq.c $
 Revision 1.1  1997/05/30 18:27:14  dkegel
 Initial revision
 Revision 1.2  1996/02/13 20:16:31  dkegel
 No longer a queue; IPX treats all submitted ECB's without regard
 to the order of submission, so we have to search all the ECB's
 instead of looking at the oldest.
 Revision 1.1  1996/02/11 22:54:59  dkegel
 Initial revision

 A simple pool for IPX ECB's.
-----------------------------------------------------------------------*/

#include "ipxq.h"
#include <stdlib.h>

#include	"IpxTransport.h"
#include	"ipxpb.h"

#define DPMI_INT 0x31
#define GET_BASE_ADDR_FUNC		0x0006
#define ALLOC_DOS_MEM_FUNC		0x0100
#define FREE_DOS_MEM_FUNC		0x0101

/*-----------------------------------------------------------------------
 Create a pool.  Returns NULL on failure.
 Actually allocates an ECB plus ipxq_MAX_USERDATALEN bytes of
 DOS memory for each of ipxq_MAXELS packets.
 The parameter, socket, is used to initialize the socket field of the
 ECB's.  Presumably, all traffic will be via that socket number.

 Socket MUST be passed in network byte order.
 pool is created FULL of DONE packets.
-----------------------------------------------------------------------*/
ipxq_t *ipxq_create(short socket, nw_adr_t *defaultSrc, IOCompletionUPP esr) {
	ipxq_t *pq;
	size_t len;
	//union REGS      regs;
	int i;

	pq = (ipxq_t *)malloc(sizeof(ipxq_t));
	if (!pq) {
		// Abort!   Out of flat memory!
		return NULL;
	}
	memset(pq, 0, sizeof(ipxq_t));

	// Used to Use DPMI to alloc some DOS memory to hold all the IPX data.
	// Note: The DPMI service expects the number of paragraphs not bytes.
	len = ipxq_MAXELS * (sizeof(nw_ecb_t) + sizeof(nw_hdr_t) + ipxq_MAX_USERDATALEN)
		+ ipxq_SCRATCHLEN;
	pq->pkt_base.flat.p = malloc(len);
	if (pq->pkt_base.flat.p == nil) {
		// Abort!   Out of flat memory!
		free(pq);
		return NULL;
	}
	memset(pq->pkt_base.flat.p, 0, len);

	// Set up scratch area
	pq->scratch.flat.p = pq->pkt_base.flat.p;
	(char *)pq->scratch.flat.p += len - ipxq_SCRATCHLEN;

	// Fill pool with empty ECB's, with pointers all filled in already.
	// Let's allocate them all from the block as follows:
	// ECB0, ECB1, ... ECB15, HDR0 BUF0, HDR1 BUF1, ... HDR15 BUF15 SCRATCH
	for (i=0; i<ipxq_MAXELS; i++) {
		// Let's see... we need a pointer to the pool entry,
		ipxq_pkt_t *pp = pq->pkts + i;

		// a pointer to its ECB in protected memory,
		pp->ecb.flat.e		= pq->pkt_base.flat.e
								+ i;

		// a pointer to its header/user packet in protected memory,
		pp->hdr_n_buf.flat.p = ((char *)(pq->pkt_base.flat.e + ipxq_MAXELS))
								+ i*(sizeof(nw_hdr_t) + ipxq_MAX_USERDATALEN);

		// the ECB's first fragment points to the packet header,
		pp->ecb.flat.e->fragList[0].fragAddress = pp->hdr_n_buf.flat.p;
		pp->ecb.flat.e->fragList[0].fragSize = sizeof(nw_hdr_t);

		// its second fragment points to the user packet.
		pp->ecb.flat.e->fragList[1].fragAddress = ((char*) pp->hdr_n_buf.flat.p) + sizeof(nw_hdr_t);
		//pp->ecb.flat.e->FragmentDescriptor[1].Length = // fill in later

		// And, by golly, these ECB's are DONE.

		pp->ecb.flat.e->ESRAddress = esr; 		// No callback; we poll.
		pp->ecb.flat.e->UserWorkspace[0] = 0x00;		// ECB not in use
		//pp->ecb.flat.e->CompletionCode = 0xFC;	// Failure; the ECB was canceled
		//pp->ecb.flat.e->SocketNumber = socket;
		pp->ecb.flat.e->fragCount = nw_MAX_FRAGMENT_COUNT;
		// Um, let's set up the headers, too.
		pp->hdr_n_buf.flat.h->destSocket = socket;
		pp->hdr_n_buf.flat.h->sourceSocket = socket;
		memcpy(&(pp->hdr_n_buf.flat.h->sourceNet), defaultSrc, sizeof(nw_adr_t));
		pp->hdr_n_buf.flat.h->packetType = 4;		// IPX
	}

	// Second sanity check - protected mode address of last packet
	{
		size_t len2 =
			((char *)pq->pkts[ipxq_MAXELS-1].hdr_n_buf.flat.p
			- (char *)pq->pkt_base.flat.p)
			+ sizeof(nw_hdr_t)
			+ ipxq_MAX_USERDATALEN
			+ ipxq_SCRATCHLEN;
		if (len2 != len) {
			// Abort- internal screwup.
			return NULL;
		}
	}

	// Success!
	return pq;
}

/*-----------------------------------------------------------------------
 Destroy a pool.
 Frees all the messages that had been in the pool.
 Frees DOS memory.
 Warning: You must cancel all IPX activity on these packets, e.g. by closing
 the socket, before calling this function.
-----------------------------------------------------------------------*/
void ipxq_destroy(ipxq_t *pq)
{

	if (pq != nil) {
		if (pq->pkt_base.flat.p != nil) {
			free(pq->pkt_base.flat.p);
		}
		free(pq);
	}

}

/*-----------------------------------------------------------------------
 Get next packet out of a pool if it is DONE.
 Returns NULL if next packet in pool is not DONE.
-----------------------------------------------------------------------*/
ipxq_pkt_t *ipxq_get(ipxq_t *pq)
{
	int i;
	ipxq_pkt_t *pp;

	for (i=0, pp=pq->pkts; i<ipxq_MAXELS; i++, pp++)
		if (pp->ecb.flat.e->UserWorkspace[0] == 0x00) return pp;
	return NULL;
}

/*-----------------------------------------------------------------------
 Put a packet in a pool.
 Only place to get packets is ipxq_get, so the thing that's put on the
 pool is the value from the last call to ipxq_get.

 Returns NULL on failure.
 Returns the address of the packet in the pool on success.
-----------------------------------------------------------------------*/
ipxq_pkt_t *ipxq_put(ipxq_t *pq)
{
	return (ipxq_pkt_t *)1;
}
