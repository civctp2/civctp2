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

/*
 * rip.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */

#ifndef __RIP_H_INCLUDED__
#define __RIP_H_INCLUDED__

#ifndef __IPXCALLS_INCLUDED__
#include "ipxcalls.h"
#endif

#define ROUTE_ENTRIES_PER_PACKET	50
#define ROUTER_SOCKET				(uint16)0x0453
#define MIN_ROUTE_PACKET			40
#define RIP_PACKET_TYPE				1
#define ROUTE_REQUEST				1
#define ROUTE_RESPONSE				2

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct routeEntry {
	uint32		targetNet;
	uint16		targetHops;
	uint16		targetTime;
} routeEntry_t;

typedef struct routePacket {
	uint16		checksum;		/* always 0xffff */
	uint16		packetLen;		/* length of data and IPX header */
	uint8		transportCtl;	/* transport control */
	uint8		packetType;		/* packet type */
	uint32		destNet;    	/* destination address */
	uint8		destNode[6];
	uint16		destSocket;
	uint32		sourceNet;  	/* source address */
	uint8		sourceNode[6];
	uint16		sourceSocket;
	uint16		operation;
	routeEntry_t		routeTable[ROUTE_ENTRIES_PER_PACKET];
} routePacket_t;

typedef struct routeRequestPacket {
	uint16		checksum;		/* always 0xffff */
	uint16		packetLen;		/* length of data and IPX header */
	uint8		transportCtl;	/* transport control */
	uint8		packetType;		/* packet type */
	uint32		destNet;    	/* destination address */
	uint8		destNode[6];
	uint16		destSocket;
	uint32		sourceNet;  	/* source address */
	uint8		sourceNode[6];
	uint16		sourceSocket;
	uint16		operation;
	routeEntry_t		routeRequest;
} routeRequestPacket_t;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

typedef routeRequestPacket_t routeResponsePacket_t;

#endif /* __RIP_H_INCLUDED__ */
