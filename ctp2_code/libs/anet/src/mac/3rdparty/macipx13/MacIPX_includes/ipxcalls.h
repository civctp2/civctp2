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
 * ipxcalls.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 *
 * This include file defines IPX constants, macros, and types.
 */

#ifndef __IPXCALLS_INCLUDED__
#define __IPXCALLS_INCLUDED__

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif

#include "types.h"

/*
 * IPX_ECB status field busy (in-process) codes
 */
#define ST_AESQ				(sint16)0x1002
#define ST_ECBQ				(sint16)0x1004
#define ST_SENDPACKET		(sint16)0x2001

/*
 * IPX_ECB flags field bit mask.  The comments have the following
 * meanings:
 *  q - This bit may be read by the application
 *  s - This bit must be set or cleared prior to a send
 *  R - This bit is reserved for Novell
 */
#define BROADCAST_FLAG		0x00000001	/* q - Packet was received as a data link broadcast */
#define CHECKSUM_FLAG		0x00000002	/* s - (IpxSend) Packet should be checksummed  */
										/* q - (IpxReceive) Packet contained a checksum */
#define NO_CHECKSUM_FLAG	0x00000004  /* s - (IpxSend) Packet should not be checksummed */

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
   uint16		fragSize;
   void					*fragAddress;
} IPXECBFrag;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#ifndef REGISTER_A0_COMPATIBILITY
typedef pascal void (*ESRProcPtr)(struct IPX_ECBStruct *ecb);

#if USESROUTINEDESCRIPTORS
enum  {
	uppESRProcInfo = kPascalStackBased
						| RESULT_SIZE(kNoByteCode)
						| STACK_ROUTINE_PARAMETER(1, kFourByteCode)
};

typedef UniversalProcPtr ESRUPP;

#define CallESRProc(userRoutine, ecb_ptr)  \
	CallUniversalProc((UniversalProcPtr)(userRoutine), uppESRProcInfo, (ecb_ptr))

#define NewESRProc(userRoutine)  \
	(ESRUPP) NewRoutineDescriptor((ProcPtr)(userRoutine),  \
	uppESRProcInfo, GetCurrentISA())

#else
typedef ESRProcPtr ESRUPP;

#define NewESRProc(userRoutine)  \
	(ESRUPP)((userRoutine))

#endif
#endif /* REGISTER_A0_COMPATIBILITY */

/*---------------------------------------------------------------------------*
 *   The comment characters in the IPX_ECB structure have the                *
 *   following meanings                                                      *
 *   s - this field must be filled in prior to a send                        *
 *   r - this field must be filled in prior to a receive                     *
 *   R - this field is reserved                                              *
 *   A - this field may be used when the ECB is not in use by IPX/SPX        *
 *   q - the application may read this field                                 *
 *   u - this field is reserved for the application, IPX/SPX will not use it *
 *---------------------------------------------------------------------------*/

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct IPX_ECBStruct {
   struct IPX_ECBStruct   *next;                		/* A */
   uint8           APIWorkspace[42];    		/* A */
   struct IPX_ECBStruct   *prev;                		/* A */
   sint16                   status;              		/* q */
   uint32           flags;               		/* q, s (IpxSend only) */
#ifdef REGISTER_A0_COMPATIBILITY
   pascal void             (*ESRAddress)(void);			/* sr */
#else
   ESRUPP                  ESRAddress;                  /* sr */
#endif
   uint8           immediateAddress[6]; 		/* s (IpxSend only) */
   uint8           DrvrWorkspace[30];   		/* R */
   uint8           UserWorkspace[4];    		/* u */
   QHdrPtr                 MacOSQueue;          		/* sr */
   uint32           savedA5;             		/* R */
   uint16          dataLen;             		/* q */
   uint16          fragCount;           		/* sr */
   IPXECBFrag              DrvrFrag1[2];				/* R */
   IPXECBFrag              fragList[2];         		/* sr */
   IPXECBFrag              DrvrFrag2;					/* R */
} IPX_ECB;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

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
} IPX_HEADER;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#define IPX_HDR_SIZE		sizeof(IPX_HEADER)

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
	uint32	ipxInReceives;
	uint32	ipxInRcvBufOverflows;
	uint32	ipxInHdrErrors;
	uint32	ipxInAddrErrors;
	uint32	ipxInNoRcvBuffs;
	uint32	ipxInLapErrors;
	uint32	ipxInUnknownSocket;
	uint32	ipxInDelivers;
	uint32	ipxInRequests;
	uint32	ipxInBadChecksum;
	uint32	ipxOutRequests;
	uint32	ipxOutMalformedRequests;
	uint32	ipxOutNoRoutes;
	uint32	ipxOutLapErrors;
	uint32	ipxOutNoMemoryFails;
	uint32	ipxMaxOpenSockets;
	uint32	ipxOpenSocketFails;
	uint32	ipxScheduleEvents;
} IPX_STATS;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * Novell IPX gestalt selector
 */
#define gestaltNovellIPXVersion			'NIPX'

/*
 * Novell NetWare/IP gestalt selector
 */
#define gestaltNovellNetWareIPVersion	'NWIP'

/*
 * Features supported by the protocol stack
 */
#define gestaltNovellIPXStackAttr		'NiPx'
#define gestaltAsyncGetLocalTarget		1L
#define gestaltIpxLookAheadSocket		2L
#define gestaltECBOnStackForCallbacks	4L

/*
 * IpxOpenSocket and IpxOpenLookAheadSocket flags
 */
#define WANT_BROADCASTS				0x01
#define USE_EXTERNAL_SPX			0x02

/*
 * IpxGetInfo "supported_features" bitmask result
 */
#define CHECKSUMS_ARE_OK			2L
#define IPX_DIAGNOSTICS_ACTIVE		4L
#define ESR_ECB_CAN_BE_ON_STACK		8L

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
	uint32		supported_features;
	uint32		retry_interval;
} IPX_CONFIG_INFO;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * IpxSend2 flags bitmask
 */
#define DO_NOT_CHECKSUM_PACKET		0x01

/*
 * IPX Receive Look Ahead Handler information
 */
#define MAX_LOOKAHEAD_LEN			128

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
	char			Reserved0[4];
	void			*LookAheadPtr;
	uint16	LookAheadLength;
	char			Reserved1[16];
} LookAheadStructure;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

typedef pascal IPX_ECB * ((*RecvLookAheadHandler)(uint16 socket,
									LookAheadStructure *LookAheadStruct,
									sint32 userData));

#if USESROUTINEDESCRIPTORS
enum  {
	uppRecvLookAheadHandlerProcInfo = kPascalStackBased
						| RESULT_SIZE(kFourByteCode)
						| STACK_ROUTINE_PARAMETER(1, kTwoByteCode)
						| STACK_ROUTINE_PARAMETER(2, kFourByteCode)
						| STACK_ROUTINE_PARAMETER(3, kFourByteCode)
};

typedef UniversalProcPtr RecvLookAheadHandlerUPP;

#define CallRecvLookAheadHandlerProc(userRoutine, socket, LookAheadStruct, userData)  \
	CallUniversalProc((UniversalProcPtr)(userRoutine), uppRecvLookAheadHandlerProcInfo, (socket), (LookAheadStruct), (userData))

#define NewRecvLookAheadHandlerProc(userRoutine)  \
	(RecvLookAheadHandlerUPP) NewRoutineDescriptor((ProcPtr)(userRoutine),  \
	uppRecvLookAheadHandlerProcInfo, GetCurrentISA())

#else
typedef RecvLookAheadHandler RecvLookAheadHandlerUPP;

#define NewRecvLookAheadHandlerProc(userRoutine)  \
	(RecvLookAheadHandlerUPP)((userRoutine))

#endif

/*
 * IPX APIs
 */
#ifdef REGISTER_A0_COMPATIBILITY
extern pascal IPX_ECB *IpxGetECBFromRegisterA0(void);
#endif

#ifndef COMPONENT_BUILD
extern pascal sint16 IpxCancelEvent(uint16 socket, IPX_ECB  *ecb);
extern pascal sint16 IpxCheckSocket(uint16 socket);
extern pascal sint16 IpxCloseSocket(uint16 socket);
extern pascal sint16	IpxGetInfo(IPX_CONFIG_INFO *buffer, uint32 *buflen);
extern pascal sint16 IpxGetInternetworkAddress(uint8 *address);
extern pascal sint16 IpxGetLocalTarget(uint8 *target, IPX_ECB *ecb, uint32 *timeToNet);
extern pascal sint16 IpxGetLocalTargetAsynch(uint8 *target, IPX_ECB *ecb1, IPX_ECB *ecb2, uint32 *timeToNet);
extern pascal sint16 IpxGetMaxPacketSize(uint32 *maxPktSize);
extern pascal sint16 IpxGetStatistics(IPX_STATS *buffer, uint32 *buflen);
extern pascal sint16 IpxGetVersion(uint8 *majorVersion, uint8 *minorVersion, uint16 *revision);
extern pascal sint16 IpxInitialize(void);
extern pascal sint16 IpxOpenLookAheadSocket(uint16 *socket, RecvLookAheadHandlerUPP handler, uint16 numBytes, sint32 userData, uint8 flags);
extern pascal sint16 IpxOpenSocket(uint16 *socket, uint8 flags);
extern pascal sint16 IpxReceive(uint16 socket, IPX_ECB *ecb);
extern pascal sint16 IpxRelinquishControl(void);
extern pascal sint16 IpxScheduleIpxEvent(uint16 socket, uint32 timeTicks, IPX_ECB *ecb);
extern pascal sint16 IpxSend(uint16 socket,IPX_ECB *ecb);
extern pascal sint16 IpxSend2(uint16 socket, IPX_ECB *ecb, uint8 flags);
#else
extern pascal sint16 IpxCancelEvent(uint16 socket, IPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 IpxCheckSocket(uint16 socket, sint16 drvrRefNum);
extern pascal sint16 IpxCloseSocket(uint16 socket, sint16 drvrRefNum);
extern pascal sint16	IpxGetInfo(IPX_CONFIG_INFO *buffer, uint32 *buflen, sint16 drvrRefNum);
extern pascal sint16 IpxGetInternetworkAddress(uint8 *address, sint16 drvrRefNum);
extern pascal sint16 IpxGetLocalTarget(uint8 *target, IPX_ECB *ecb, uint32 *timeToNet, sint16 drvrRefNum);
extern pascal sint16 IpxGetLocalTargetAsynch(uint8 *target, IPX_ECB *ecb1, IPX_ECB *ecb2, uint32 *timeToNet, sint16 drvrRefNum);
extern pascal sint16 IpxGetMaxPacketSize(uint32 *maxPktSize, sint16 drvrRefNum);
extern pascal sint16 IpxGetStatistics(IPX_STATS *buffer, uint32 *buflen, sint16 drvrRefNum);
extern pascal sint16 IpxGetVersion(uint8 *majorVersion, uint8 *minorVersion, uint16 *revision, sint16 drvrRefNum);
extern pascal sint16 IpxInitialize(sint16 *drvrRefNumPtr);
extern pascal sint16 IpxOpenLookAheadSocket(uint16 *socket, RecvLookAheadHandlerUPP handler, uint16 numBytes, sint32 userData, uint8 flags, sint16 drvrRefNum);
extern pascal sint16 IpxOpenSocket(uint16 *socket, uint8 flags, sint16 drvrRefNum);
extern pascal sint16 IpxReceive(uint16 socket, IPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 IpxRelinquishControl(sint16 drvrRefNum);
extern pascal sint16 IpxScheduleIpxEvent(uint16 socket, uint32 timeTicks, IPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 IpxSend(uint16 socket,IPX_ECB *ecb, sint16 drvrRefNum);
extern pascal sint16 IpxSend2(uint16 socket, IPX_ECB *ecb, uint8 flags, sint16 drvrRefNum);
#endif

#endif  /* __IPXCALLS_INCLUDED__ */
