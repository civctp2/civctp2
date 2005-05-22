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
   unsigned short          checksum;
   unsigned short          packetLen;
   unsigned char           transportCtl;
   unsigned char           packetType;
   unsigned long           destNet;
   unsigned char           destNode[6];
   unsigned short          destSocket;
   unsigned long           sourceNet;
   unsigned char           sourceNode[6];
   unsigned short          sourceSocket;

   unsigned char           connectionCtl;
   unsigned char           dataStreamType;
   unsigned short          sourceConnectID;
   unsigned short          destConnectID;
   unsigned short          sequenceNumber;
   unsigned short          ackNumber;
   unsigned short          allocNumber;
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
   unsigned char           sStatus;
   unsigned char           sFlags;
   unsigned short          sSourceConnectID;
   unsigned short          sDestConnectID;
   unsigned short          sSequenceNumber;
   unsigned short          sAckNumber;
   unsigned short          sAllocNumber;

   unsigned short          sRemoteAckNumber;
   unsigned short          sRemoteAllocNumber;

   unsigned short          sLocalSocket;
   unsigned char           sImmediateAddress[6];

   unsigned long           sRemoteNet;
   unsigned char           sRemoteNode[6];
   unsigned short          sRemoteSocket;

   unsigned char           sRetransmitCount;
   unsigned char           sRetransmitMax;
   unsigned short          sRoundTripTimer;
   unsigned short          sRetransmittedPackets;
   unsigned short          sSuppressedPackets;
} SPX_SESSION;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
	unsigned long		spxMaxUsedConnections;
	unsigned long		spxEstablishConnectionRequests;
	unsigned long		spxEstablishConnectionFails;
	unsigned long		spxListenConnectionRequests;
	unsigned long		spxListenConnectionFails;
	unsigned long		spxOutRequests;
	unsigned long		spxOutAckRequests;
	unsigned long		spxOutWindowClosed;
	unsigned long		spxOutMalformedPackets;
	unsigned long		spxOutIpxFailures;
	unsigned long		spxOutUnknownConnIDs;
	unsigned long		spxOutRetransmits;
	unsigned long		spxOutAckFailures;
	unsigned long		spxOutNoAckReceived;
	unsigned long		spxConnectionAborted;
	unsigned long		spxInRequests;
	unsigned long		spxInFails;
	unsigned long		spxInReceives;
	unsigned long		spxInUnknownConnIDs;
	unsigned long		spxInDelivers;
	unsigned long		spxInNoRcvBuffs;
	unsigned long		spxInWindowClosed;
	unsigned long		spxInHdrErrors;
	unsigned long		spxInBadChecksum;
	unsigned long		spxInUnknownSocket;
	unsigned long		spxSuppressedPackets;
	unsigned long		spxWatchdogDestroySessions;
	unsigned long		spxNoInternalECBs;
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
extern pascal short SpxAbortConnection(unsigned short connection, SPX_ECB *ecb);
extern pascal short SpxCancelEvent(SPX_ECB *ecb);
extern pascal short SpxCheckSocket(unsigned short socket);
extern pascal short SpxCloseSocket(unsigned short socket);
extern pascal short SpxEstablishConnection(unsigned short socket, SPX_ECB *ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection);
extern pascal short SpxEstablishConnection2(unsigned short socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection);
extern pascal short SpxGetConfiguration(unsigned short *maxConn, unsigned short *availConn);
extern pascal short SpxGetConnectionStatus(unsigned short connection, SPX_SESSION *buffer);
extern pascal short SpxGetStatistics(SPX_STATS *buffer, unsigned long *buflen);
extern pascal short SpxGetVersion(unsigned char *majorVersion, unsigned char *minorVersion, unsigned short *revision);
extern pascal short SpxListenForConnection(unsigned short socket, SPX_ECB *ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection);
extern pascal short SpxListenForConnection2(unsigned short socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection);
extern pascal short SpxListenForConnectionPacket(unsigned short connection, SPX_ECB *ecb);
extern pascal short SpxOpenSocket(unsigned short *socket, unsigned char flags);
extern pascal short SpxSendSequencedPacket(unsigned short connection, SPX_ECB *ecb);
extern pascal short SpxTerminateConnection(unsigned short connection, SPX_ECB *ecb);
#else
extern pascal short SpxAbortConnection(unsigned short connection, SPX_ECB *ecb, short drvrRefNum);
extern pascal short SpxCancelEvent(SPX_ECB *ecb, short drvrRefNum);
extern pascal short SpxCheckSocket(unsigned short socket, short drvrRefNum);
extern pascal short SpxCloseSocket(unsigned short socket, short drvrRefNum);
extern pascal short SpxEstablishConnection(unsigned short socket, SPX_ECB *ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection, short drvrRefNum);
extern pascal short SpxEstablishConnection2(unsigned short socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection, short drvrRefNum);
extern pascal short SpxGetConfiguration(unsigned short *maxConn, unsigned short *availConn, short drvrRefNum);
extern pascal short SpxGetConnectionStatus(unsigned short connection, SPX_SESSION *buffer, short drvrRefNum);
extern pascal short SpxGetStatistics(SPX_STATS *buffer, unsigned long *buflen, short drvrRefNum);
extern pascal short SpxGetVersion(unsigned char *majorVersion, unsigned char *minorVersion, unsigned short *revision, short drvrRefNum);
extern pascal short SpxListenForConnection(unsigned short socket, SPX_ECB *ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection, short drvrRefNum);
extern pascal short SpxListenForConnection2(unsigned short socket, SPX_ECB *ecb, SPX_ECB *recv_ecb, unsigned char retryCount, unsigned char flags, unsigned short *connection, short drvrRefNum);
extern pascal short SpxListenForConnectionPacket(unsigned short connection, SPX_ECB *ecb, short drvrRefNum);
extern pascal short SpxOpenSocket(unsigned short *socket, unsigned char flags, short drvrRefNum);
extern pascal short SpxSendSequencedPacket(unsigned short connection, SPX_ECB *ecb, short drvrRefNum);
extern pascal short SpxTerminateConnection(unsigned short connection, SPX_ECB *ecb, short drvrRefNum);
#endif

#endif  /* __SPXCALLS_INCLUDED__ */
