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
 $Log: ipx2.h $
 Revision 1.2  1997/06/02 01:27:34  dkegel
 Use commapi.h's error codes!
 Revision 1.1  1997/05/30 18:27:13  dkegel
 Initial revision
 Revision 1.3  1996/02/13 20:16:08  dkegel
 Added way to report error cause in ipx2_create.
 It's a kludge, but it helps.
 Revision 1.2  1996/02/12 19:09:02  dkegel
 Improved method descriptions.
 Revision 1.1  1996/02/11 22:54:52  dkegel
 Initial revision

 A simple IPX driver.  Really, a library that makes calling the
 real mode IPX driver easy from protected mode.
-----------------------------------------------------------------------*/

#ifndef ipx2_h
#define ipx2_h

#pragma pack(1)

#include <string.h>
#include <stddef.h>

#include "dynatab.h"
#include "ipxq.h"
#include "commapi.h"			// For caller-friendly error codes.

// A peer.  Somebody we send packets to, or get packets from, often.
typedef struct {
	nw_adr_t		dest;			// Ultimate destination of peer.
	nw_nodeAdr_t	immedAddress;	// Address of router to peer on LAN.  From Netware.
} ipx2_peer_t;

// A peer handle.  Almost same as an ipx2_peer_t *, but doesn't
// change if peer table is resized.  Plus, you can print them out
// as integers and even understand them, which is nice for debugging.
typedef int ipx2_hdl_t;			// Handles used in sending packets.
#define ipx2_HDL_NONE -1
#define ipx2_HDL_BROADCAST	0	// Right, the first entry in ipx2->peers is for broadcast.
#define ipx2_HDL_ME			1	// The 2nd entry is for my own address.

//--------------------------------------------------------------------------
// A driver.  A thing which lets us send packets to and from other computers.
// The central type of this module.
//--------------------------------------------------------------------------
typedef struct {
	dynatab_t *peers;			// Table of ipx2_peer_t's.
								// Managed by ipx2_adr2hdl. Indexed by ipx2_hdl_t
	ipxq_t	*rx;				// Incoming packet queue.
	ipxq_t	*tx;				// Outgoing packet queue.

	nw_segptr_t ipxEntryPoint;	// Real mode far call entry point to Hades.

	short socket;				// What socket to close to clean up.
} ipx2_t;

// A result.  Lets you know if a call failed, and why.
typedef int ipx2_result_t;		// Error/success status type.
// IPXWIN Function Result Codes.  same as interface our caller inherits from...
#define ipx2_RES_OK		  comm_STATUS_OK
#define ipx2_RES_FULL	  comm_STATUS_FULL
#define ipx2_RES_EMPTY	  comm_STATUS_EMPTY
#define ipx2_RES_BAD	  comm_STATUS_BAD

/*-----------------------------------------------------------------------
 Create an IPX driver.
 Returns NULL on failure.
 Parameter is socket number (analogous to UDP/IP port number) to
 use for all traffic.
 Only one of these may exist at a time, since multiple ones would
 compete for the same source of rx packets.
-----------------------------------------------------------------------*/
ipx2_t *ipx2_create(int socket, int *status);

/*-----------------------------------------------------------------------
 Destroy an IPX driver.
-----------------------------------------------------------------------*/
void ipx2_destroy(ipx2_t *ipx);

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
ipx2_hdl_t ipx2_adr2hdl(ipx2_t *ipx, nw_adr_t *adr, int insert);

/*-----------------------------------------------------------------------
 Given a handle, return the corresponding IPX address.
 Returns ipx2_RES_EMPTY on failure.
-----------------------------------------------------------------------*/
ipx2_result_t ipx2_hdl2adr(ipx2_t *ipx, ipx2_hdl_t hdl, nw_adr_t *adr);

/*-----------------------------------------------------------------------
 Send a packet via IPX to the given destination.
 Copies len bytes from buf into internal packet queue.
 Must have previously gotten handle for destination by calling ipx2_adr2hdl.
-----------------------------------------------------------------------*/
ipx2_result_t ipx2_put(ipx2_t *ipx, void *buf, size_t len, ipx2_hdl_t dest);

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
ipx2_result_t ipx2_get(ipx2_t *ipx, void *buf, size_t *len, ipx2_hdl_t *srcHdl, nw_adr_t *srcAdr);

//	other prototypes

pascal void ipx2_recieve_callback(IPX_ECB *ecb);
pascal void ipx2_send_callback(IPX_ECB *ecb);

#endif
