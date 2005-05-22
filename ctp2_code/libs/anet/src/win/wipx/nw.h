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
 $Log: nw.h $
 Revision 1.2  1997/02/01 00:42:01  dkegel
 Hew to 'pragma pack(1) after includes; pragma pack() at end'.
 Revision 1.1  1996/12/12 22:09:22  dkegel
 Initial revision
 Revision 1.1  1996/02/11 22:54:56  dkegel
 Initial revision

 Netware definitions needed for public .h files.
 That means anything that sets a structure size.
-----------------------------------------------------------------------*/

#ifndef nw_h
#define nw_h

#pragma pack(1)

typedef unsigned char u8;
typedef unsigned short u16;

typedef struct {
	// real mode pointer to area
	// WARNING: must be same as native x86 far pointer!
	short off, seg;
} nw_segptr_t;


// IPX network number
typedef	unsigned char nw_netAdr_t[4];

// IPX node address (link layer address)
typedef	unsigned char nw_nodeAdr_t[6];

// The two together form a full IPX internetwork address.
typedef	struct {	
	nw_netAdr_t		net;
	nw_nodeAdr_t	node;
} nw_adr_t;

#define nw_MAX_FRAGMENT_COUNT	2		// fragment count- hdr and body

// The control block used by the local Netware driver to control a packet
// on its way through the driver.
typedef	struct {			// ECB
	nw_segptr_t	LinkAddress;
	nw_segptr_t	EventServiceRoutine;
	u8			InUseFlag;
	u8			CompletionCode;
	short		SocketNumber;
	u8			WorkSpace[4];
	u8			DriverWorkSpace[12];
	nw_nodeAdr_t	ImmediateAddress;
	u16			FragmentCount;
	struct {
		nw_segptr_t	Ptr;
		u16			Length;
	}			FragmentDescriptor[nw_MAX_FRAGMENT_COUNT];
} nw_ecb_t;

// The header sent on the wire along with every user data packet.
typedef	struct {			// IPX header (Big-Endian)
	u16				CheckSum;
	u16				Length;				// Including header
	u8				TransportControl;
	u8				PacketType;
	nw_adr_t		dest;
	u16				DestinationSocket;
	nw_adr_t		src;
	u16				SourceSocket;
} nw_hdr_t;

// Return to default structure packing
#pragma pack()

#endif
