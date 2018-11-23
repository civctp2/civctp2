/*-----------------------------------------------------------------------
 $Log: IPXQ.H $
 Revision 1.1  1997/05/30 18:27:13  dkegel
 Initial revision
 Revision 1.2  1996/02/13 20:17:27  dkegel
 No longer a queue; IPX treats all submitted ECB's without regard
 to the order of submission, so we have to search all the ECB's 
 instead of looking at the oldest.
 Revision 1.1  1996/02/11 22:54:54  dkegel
 Initial revision

 A simple pool of IPX packets.
 Used to handle either incoming or outgoing packet stream.
 Two of these needed.
-----------------------------------------------------------------------*/

#ifndef ipxq_h
#define ipxq_h 

#pragma pack(1)

#include <string.h>
#include <stddef.h>
//#include <i86.h>
#include "ipxcalls.h"					//	#include "nw.h"
#include "IpxTransport.h"
#include	"ipxpb.h"

// type used to hold both real mode and protected mode pointers to
// buffers used to communicated between the two modes.
typedef struct {

	// flat protected mode naive pointer, relative to default DS:
	union {
		nw_ecb_t *e;	// Either a pointer to a Netware ECB,
		nw_hdr_t *h;	// an ipx header,
		void *p;		// or a pointer to user data, or whatever.
	} flat;
	
} ipxq_ptr_t;

typedef struct {
	// Request block for packet, used by Netware API
	ipxq_ptr_t	ecb;

	// User data area for header + packet.  The two are stored contiguously.
	ipxq_ptr_t	hdr_n_buf;
} ipxq_pkt_t;

// How deep a pool is.  32 packets ought to hold the little bastards.
// When using unicast (e.g. 8 peers or below), this gives us four
// pending packets per peer in each direction, which should be p-l-e-n-t-y,
// assuming sim code throttles itself nicely.
// When going above 8 players, the higher layer should switch to broadcast,
// in which case the transmit pool can stay small, but the recieve pool
// has to grow.  For now, we'll fix it at 32.  If we try to do more than
// 8 players, should make this dynamic.
#define ipxq_MAXELS 32

// How many bytes of user data we allocate per packet.
#define ipxq_MAX_USERDATALEN 512
// How many bytes to allocate for kludge scratch area used by callers.
#define ipxq_SCRATCHLEN 64

typedef struct {
	ipxq_pkt_t	pkts[ipxq_MAXELS];

	ipxq_ptr_t	pkt_base;	// Pointer to start of packet buffer area.
							// Used only by destructor.
	ipxq_ptr_t	scratch;	// Pointer to a small scratch area.
} ipxq_t;

/*-----------------------------------------------------------------------
 Create a pool.  Returns NULL on failure.
 Actually allocates an ECB plus ipxq_MAX_USERDATALEN bytes of
 DOS memory for each of ipxq_MAXELS packets.
 The parameter, socket, is used to initialize the socket field of the
 ECB's.  Presumably, all traffic will be via that socket number.

 pool is created FULL of DONE packets.
-----------------------------------------------------------------------*/
ipxq_t *ipxq_create(short socket, nw_adr_t *defaultSrc, IOCompletionUPP esr);
/*-----------------------------------------------------------------------
 Destroy a pool.
 Frees all the messages that had been in the pool.
 Frees DOS memory.
-----------------------------------------------------------------------*/
void ipxq_destroy(ipxq_t *pq);


/*-----------------------------------------------------------------------
 Get next packet out of a pool if it is DONE.
 Returns NULL if next packet in pool is not DONE.
-----------------------------------------------------------------------*/
ipxq_pkt_t *ipxq_get(ipxq_t *pq);

/*-----------------------------------------------------------------------
 Put a packet in a pool.
 Only place to get packets is ipxq_get, so the thing that's put on the
 pool is the value from the last call to ipxq_get.

 Returns NULL on failure.
 Returns the address of the packet in the pool on success.
-----------------------------------------------------------------------*/
ipxq_pkt_t *ipxq_put(ipxq_t *pq);


#endif
