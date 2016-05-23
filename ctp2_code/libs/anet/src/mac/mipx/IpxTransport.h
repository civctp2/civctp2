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

#ifndef _IpxTransport_
#define	_IpxTransport_

//	remap some structures to the MacIPX header files

#define	nw_ecb_t	IPX_ECB			//	Macintosh ECB structure
#define	nw_hdr_t	IPX_HEADER		//	Macintosh header structure
#define	nw_segptr_t	void*			//	complex Macintosh memory address

//	steal some structures from NW.H

// IPX network number
typedef	unsigned char nw_netAdr_t[4];

// IPX node address (link layer address)
typedef	unsigned char nw_nodeAdr_t[6];

// The two together form a full IPX internetwork address.
typedef	struct {
	nw_netAdr_t		net;
	nw_nodeAdr_t	node;
} nw_adr_t;

typedef struct {
	nw_adr_t		netNode;
	short			socket;
} IpxNetAddress;

#define nw_MAX_FRAGMENT_COUNT	2		// fragment count- hdr and body

#endif
