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

/*
 * IPX_ECB status field busy (in-process) codes
 */
#define ST_AESQ				(short)0x1002
#define ST_ECBQ				(short)0x1004
#define ST_SENDPACKET		(short)0x2001

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
   unsigned short		fragSize;
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
   unsigned char           APIWorkspace[42];    		/* A */
   struct IPX_ECBStruct   *prev;                		/* A */
   short                   status;              		/* q */
   unsigned long           flags;               		/* q, s (IpxSend only) */
#ifdef REGISTER_A0_COMPATIBILITY
   pascal void             (*ESRAddress)(void);			/* sr */
#else
   ESRUPP                  ESRAddress;                  /* sr */
#endif
   unsigned char           immediateAddress[6]; 		/* s (IpxSend only) */
   unsigned char           DrvrWorkspace[30];   		/* R */
   unsigned char           UserWorkspace[4];    		/* u */
   QHdrPtr                 MacOSQueue;          		/* sr */
   unsigned long           savedA5;             		/* R */
   unsigned short          dataLen;             		/* q */
   unsigned short          fragCount;           		/* sr */
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
} IPX_HEADER;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#define IPX_HDR_SIZE		sizeof(IPX_HEADER)

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct {
	unsigned long	ipxInReceives;
	unsigned long	ipxInRcvBufOverflows;
	unsigned long	ipxInHdrErrors;
	unsigned long	ipxInAddrErrors;
	unsigned long	ipxInNoRcvBuffs;
	unsigned long	ipxInLapErrors;
	unsigned long	ipxInUnknownSocket;
	unsigned long	ipxInDelivers;
	unsigned long	ipxInRequests;
	unsigned long	ipxInBadChecksum;
	unsigned long	ipxOutRequests;
	unsigned long	ipxOutMalformedRequests;
	unsigned long	ipxOutNoRoutes;
	unsigned long	ipxOutLapErrors;
	unsigned long	ipxOutNoMemoryFails;
	unsigned long	ipxMaxOpenSockets;
	unsigned long	ipxOpenSocketFails;
	unsigned long	ipxScheduleEvents;
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
	unsigned long		supported_features;
	unsigned long		retry_interval;
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
	unsigned short	LookAheadLength;
	char			Reserved1[16];
} LookAheadStructure;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

typedef pascal IPX_ECB * ((*RecvLookAheadHandler)(unsigned short socket,
									LookAheadStructure *LookAheadStruct,
									long userData));

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
extern pascal short IpxCancelEvent(unsigned short socket, IPX_ECB  *ecb);
extern pascal short IpxCheckSocket(unsigned short socket);
extern pascal short IpxCloseSocket(unsigned short socket);
extern pascal short	IpxGetInfo(IPX_CONFIG_INFO *buffer, unsigned long *buflen);
extern pascal short IpxGetInternetworkAddress(unsigned char *address);
extern pascal short IpxGetLocalTarget(unsigned char *target, IPX_ECB *ecb, unsigned long *timeToNet);
extern pascal short IpxGetLocalTargetAsynch(unsigned char *target, IPX_ECB *ecb1, IPX_ECB *ecb2, unsigned long *timeToNet);
extern pascal short IpxGetMaxPacketSize(unsigned long *maxPktSize);
extern pascal short IpxGetStatistics(IPX_STATS *buffer, unsigned long *buflen);
extern pascal short IpxGetVersion(unsigned char *majorVersion, unsigned char *minorVersion, unsigned short *revision);
extern pascal short IpxInitialize(void);
extern pascal short IpxOpenLookAheadSocket(unsigned short *socket, RecvLookAheadHandlerUPP handler, unsigned short numBytes, long userData, unsigned char flags);
extern pascal short IpxOpenSocket(unsigned short *socket, unsigned char flags);
extern pascal short IpxReceive(unsigned short socket, IPX_ECB *ecb);
extern pascal short IpxRelinquishControl(void);
extern pascal short IpxScheduleIpxEvent(unsigned short socket, unsigned long timeTicks, IPX_ECB *ecb);
extern pascal short IpxSend(unsigned short socket,IPX_ECB *ecb);
extern pascal short IpxSend2(unsigned short socket, IPX_ECB *ecb, unsigned char flags);
#else
extern pascal short IpxCancelEvent(unsigned short socket, IPX_ECB *ecb, short drvrRefNum);
extern pascal short IpxCheckSocket(unsigned short socket, short drvrRefNum);
extern pascal short IpxCloseSocket(unsigned short socket, short drvrRefNum);
extern pascal short	IpxGetInfo(IPX_CONFIG_INFO *buffer, unsigned long *buflen, short drvrRefNum);
extern pascal short IpxGetInternetworkAddress(unsigned char *address, short drvrRefNum);
extern pascal short IpxGetLocalTarget(unsigned char *target, IPX_ECB *ecb, unsigned long *timeToNet, short drvrRefNum);
extern pascal short IpxGetLocalTargetAsynch(unsigned char *target, IPX_ECB *ecb1, IPX_ECB *ecb2, unsigned long *timeToNet, short drvrRefNum);
extern pascal short IpxGetMaxPacketSize(unsigned long *maxPktSize, short drvrRefNum);
extern pascal short IpxGetStatistics(IPX_STATS *buffer, unsigned long *buflen, short drvrRefNum);
extern pascal short IpxGetVersion(unsigned char *majorVersion, unsigned char *minorVersion, unsigned short *revision, short drvrRefNum);
extern pascal short IpxInitialize(short *drvrRefNumPtr);
extern pascal short IpxOpenLookAheadSocket(unsigned short *socket, RecvLookAheadHandlerUPP handler, unsigned short numBytes, long userData, unsigned char flags, short drvrRefNum);
extern pascal short IpxOpenSocket(unsigned short *socket, unsigned char flags, short drvrRefNum);
extern pascal short IpxReceive(unsigned short socket, IPX_ECB *ecb, short drvrRefNum);
extern pascal short IpxRelinquishControl(short drvrRefNum);
extern pascal short IpxScheduleIpxEvent(unsigned short socket, unsigned long timeTicks, IPX_ECB *ecb, short drvrRefNum);
extern pascal short IpxSend(unsigned short socket,IPX_ECB *ecb, short drvrRefNum);
extern pascal short IpxSend2(unsigned short socket, IPX_ECB *ecb, unsigned char flags, short drvrRefNum);
#endif

#endif  /* __IPXCALLS_INCLUDED__ */
