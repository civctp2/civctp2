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
 * spxcalls.h
 *
 * Copyright (C) 1992-1995 Novell, Inc.
 * All rights reserved.
 *
 * This include file defines SPX constants, macros, and types.
 */

#ifndef __SPXCALLS_INCLUDED__
#define __SPXCALLS_INCLUDED__

#ifndef __IPXCALLS_INCLUDED__
#include "ipxcalls.h"
#endif

#include "types.h"
/*
 * The SPX ECB is the same as the IPX ECB.
 */
#define SPX_ECB				IPX_ECB

/*
 * "Establish" and "Listen For" connection flags
 */
#define WATCHDOG_CONNECTION				0x02
#define DO_CHECKSUM_CONNECTION_PKTS		0x04
#define DO_NOT_CHECKSUM_CONNECTION_PKTS	0x08

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
   uint16          checksum;
   uint16          packetLen;
   uint8           transportCtl;
   uint8           packetType;
   uint32           destNet;
   uint8           destNode[6];
   uint16          destSocket;
   uint32           sourceNet;
   uint8           sourceNode[6];
   uint16          sourceSocket;

   uint8           connectionCtl;
   uint8           dataStreamType;
   uint16          sourceConnectID;
   uint16          destConnectID;
   uint16          sequenceNumber;
   uint16          ackNumber;
   uint16          allocNumber;
} SPX_HEADER;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#define SPX_HDR_SIZE          sizeof(SPX_HEADER)
#define SPX_END_OF_MESSAGE    0x10
#define SPX_ATTENTION         0x20

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct SPX_ConnStruct {
   uint8           sStatus;
   uint8           sFlags;
   uint16          sSourceConnectID;
   uint16          sDestConnectID;
   uint16          sSequenceNumber;
   uint16          sAckNumber;
   uint16          sAllocNumber;

   uint16          sRemoteAckNumber;
   uint16          sRemoteAllocNumber;

   uint16          sLocalSocket;
   uint8           sImmediateAddress[6];

   uint32           sRemoteNet;
   uint8           sRemoteNode[6];
   uint16          sRemoteSocket;

   uint8           sRetransmitCount;
   uint8           sRetransmitMax;
   uint16          sRoundTripTimer;
   uint16          sRetransmittedPackets;
   uint16          sSuppressedPackets;
} SPX_SESSION;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
	uint32		spxMaxUsedConnections;
	uint32		spxEstablishConnectionRequests;
	uint32		spxEstablishConnectionFails;
	uint32		spxListenConnectionRequests;
	uint32		spxListenConnectionFails;
	uint32		spxOutRequests;
	uint32		spxOutAckRequests;
	uint32		spxOutWindowClosed;
	uint32		spxOutMalformedPackets;
	uint32		spxOutIpxFailures;
	uint32		spxOutUnknownConnIDs;
	uint32		spxOutRetransmits;
	uint32		spxOutAckFailures;
	uint32		spxOutNoAckReceived;
	uint32		spxConnectionAborted;
	uint32		spxInRequests;
	uint32		spxInFails;
	uint32		spxInReceives;
	uint32		spxInUnknownConnIDs;
	uint32		spxInDelivers;
	uint32		spxInNoRcvBuffs;
	uint32		spxInWindowClosed;
	uint32		spxInHdrErrors;
	uint32		spxInBadChecksum;
	uint32		spxInUnknownSocket;
	uint32		spxSuppressedPackets;
	uint32		spxWatchdogDestroySessions;
	uint32		spxNoInternalECBs;
} SPX_STATS;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * SPX_SESSION sStatus field codes:
 */
#define SPX_SSTATUS_ABORTED      		0x00
#define SPX_SSTATUS_WAITING      		0x01
#define SPX_SSTATUS_STARTING     		0x02
#define SPX_SSTATUS_ESTABLISHED  		0x03
#define SPX_SSTATUS_TERMINATING  		0x04
#define SPX_SSTATUS_ROUTE_LOOKUP		0x05
#define SPX_SSTATUS_CONNECT_NO_ROUTE	0x06

/*
 * SPX APIs
 */
#define SpxCancelPacket			SpxCancelEvent
#ifdef REGISTER_A0_COMPATIBILITY
#define SpxGetECBFromRegisterA0 IpxGetECBFromRegisterA0
#endif
#define SpxInitialize			IpxInitialize

#ifndef COMPONENT_BUILD
extern pascal sint16 SpxAbortConnection(uint16 connection, SPX_ECB *ecb);
extern pascal sint16 SpxCancelEvent(SPX_ECB *ecb);
extern pascal sint16 SpxCheckSocket(uint16 socket);
extern pascal sint16 SpxCloseSocket(uint16 socket);
extern pascal sint16 SpxEstablishConnection(uint16 socket, SPX_ECB *ecb, uint8 retryCount, uint8 flags, uint16 *connection);
extern pascal sint16 SpxEstablishConnection2(uint16 socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, uint8 retryCount, uint8 flags, uint16 *connection);
extern pascal sint16 SpxGetConfiguration(uint16 *maxConn, uint16 *availConn);
extern pascal sint16 SpxGetConnectionStatus(uint16 connection, SPX_SESSION *buffer);
extern pascal sint16 SpxGetStatistics(SPX_STATS *buffer, uint32 *buflen);
extern pascal sint16 SpxGetVersion(uint8 *majorVersion, uint8 *minorVersion, uint16 *revision);
extern pascal sint16 SpxListenForConnection(uint16 socket, SPX_ECB *ecb, uint8 retryCount, uint8 flags, uint16 *connection);
extern pascal sint16 SpxListenForConnection2(uint16 socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, uint8 retryCount, uint8 flags, uint16 *connection);
extern pascal sint16 SpxListenForConnectionPacket(uint16 connection, SPX_ECB *ecb);
extern pascal sint16 SpxOpenSocket(uint16 *socket, uint8 flags);
extern pascal sint16 SpxSendSequencedPacket(uint16 connection, SPX_ECB *ecb);
extern pascal sint16 SpxTerminateConnection(uint16 connection, SPX_ECB *ecb);
#else
extern pascal sint16 SpxAbortConnection(uint16 connection, SPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 SpxCancelEvent(SPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 SpxCheckSocket(uint16 socket, sint16 drvrRefNum);
extern pascal sint16 SpxCloseSocket(uint16 socket, sint16 drvrRefNum);
extern pascal sint16 SpxEstablishConnection(uint16 socket, SPX_ECB *ecb, uint8 retryCount, uint8 flags, uint16 *connection, sint16 drvrRefNum);
extern pascal sint16 SpxEstablishConnection2(uint16 socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, uint8 retryCount, uint8 flags, uint16 *connection, sint16 drvrRefNum);
extern pascal sint16 SpxGetConfiguration(uint16 *maxConn, uint16 *availConn, sint16 drvrRefNum);
extern pascal sint16 SpxGetConnectionStatus(uint16 connection, SPX_SESSION *buffer, sint16 drvrRefNum);
extern pascal sint16 SpxGetStatistics(SPX_STATS *buffer, uint32 *buflen, sint16 drvrRefNum);
extern pascal sint16 SpxGetVersion(uint8 *majorVersion, uint8 *minorVersion, uint16 *revision, sint16 drvrRefNum);
extern pascal sint16 SpxListenForConnection(uint16 socket, SPX_ECB *ecb, uint8 retryCount, uint8 flags, uint16 *connection, sint16 drvrRefNum);
extern pascal sint16 SpxListenForConnection2(uint16 socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, uint8 retryCount, uint8 flags, uint16 *connection, sint16 drvrRefNum);
extern pascal sint16 SpxListenForConnectionPacket(uint16 connection, SPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 SpxOpenSocket(uint16 *socket, uint8 flags, sint16 drvrRefNum);
extern pascal sint16 SpxSendSequencedPacket(uint16 connection, SPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 SpxTerminateConnection(uint16 connection, SPX_ECB *ecb, sint16 drvrRefNum);
#endif

#endif  /* __SPXCALLS_INCLUDED__ */
