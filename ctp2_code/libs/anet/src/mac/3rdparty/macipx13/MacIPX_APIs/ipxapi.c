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
 * ipxapi.c
 *
 * Copyright (C) 1992-1995 Novell, Inc.
 * All rights reserved.
 *
 * Note 1: Once your application is debugged, you can improve performance of
 *         these APIs slightly by defining the constant NO_ECB_CHECKS.
 *
 * Note 2: This file can be compiled without using globals by defining the
 *         constant CANNOT_USE_GLOBALS.
 *
 * Note 3: MPW users will also have to assemble and link with ipxapi.a.
 *
 * Note 4: This file can be compiled for use with the Component Manager by
 *         defining the constant COMPONENT_BUILD.  When this constant is defined,
 *         all the IPX routines which make a call to "call_ipx" are modified
 *         to accept an extra argument for the driver reference number. This
 *         driver reference number is normally stored away for you automatically
 *         for performance reasons.  A Macintosh Component, however, may not have
 *         global data and its code may be moved, even across heap zones. These
 *         conditions dissallow our normal method of hiding the driver reference
 *         number for you. These conditions require you to save the driver
 *         reference number from the IPXInitialize API and you must pass it into
 *         all the other IPX APIs.
 */
 
/*
 * Include files
 */
#include <Devices.h>
#include <Memory.h>
#if 0
#include <Desk.h>
#else
/* Metrowerks CW Pro 1 says: don't use Desk.h */
#include <Menus.h>
#include <Events.h>
#endif

#include "ipxcalls.h"
#include "ipxerror.h"
#include "ipxpb.h"
#include "rip.h"

/*
 * Defines
 */
#ifndef TRUE
#define TRUE					1
#endif
#ifndef FALSE
#define	FALSE					0
#endif

#if !defined (CANNOT_USE_GLOBALS) && !defined (COMPONENT_BUILD)
/*
 * Globals
 */
static short			MacIPXDrvrRefNum;
#endif

/*
 * Prototypes
 */
short					find_ipx_drvr(short *drvrRefNum);
#ifdef CANNOT_USE_GLOBALS
extern void				Ipx_set_drvr_ref_num(long drvrRefNum);
extern long				Ipx_get_drvr_ref_num(void);
#endif
#if !defined(powerc) && !defined (__powerc)
extern unsigned long	IpxGetRegisterA5(void);
#endif

pascal short
#ifndef	COMPONENT_BUILD
IpxCancelEvent(unsigned short socketNetOrder, IPX_ECB *ecb)
#else
IpxCancelEvent(unsigned short socketNetOrder, IPX_ECB *ecb, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	/*
	 * If the ECB is no longer waiting to be used for receiving, or
	 * waiting for an AES, we cannot cancel it
	 */
#ifndef NO_ECB_CHECKS
	if (ecb == (IPX_ECB *)0) {
		return (IPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status != ST_ECBQ) && (ecb->status != ST_AESQ)) {
		return (IPX_EVENT_CANNOT_BE_CANCELLED);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb.csCode = IPXCANCELEVENT;
	ipx_pb.csParams.cancel.socketNumber = socketNetOrder;
	ipx_pb.csParams.cancel.ecb = ecb;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxCheckSocket(unsigned short socketNetOrder)
#else
IpxCheckSocket(unsigned short socketNetOrder, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	ipx_pb.csCode = IPXCHECKSOCKET;
	ipx_pb.csParams.check.socketNumber = socketNetOrder;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxCloseSocket(unsigned short socketNetOrder)
#else
IpxCloseSocket(unsigned short socketNetOrder, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	ipx_pb.csCode = IPXCLOSESOCKET;
	ipx_pb.csParams.close.socketNumber = socketNetOrder;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxGetInfo(IPX_CONFIG_INFO *buffer, unsigned long *buflen)
#else
IpxGetInfo(IPX_CONFIG_INFO *buffer, unsigned long *buflen, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	if ((buffer == (IPX_CONFIG_INFO *)0) || (buflen == (unsigned long *)0)) {
		return (IPX_BAD_PARAMETER);
	}
	ipx_pb.csCode = IPXGETINFO;
	ipx_pb.csParams.getInfo.buf = buffer;
	ipx_pb.csParams.getInfo.buflen = buflen;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxGetInternetworkAddress(unsigned char *networkAddress)
#else
IpxGetInternetworkAddress(unsigned char *networkAddress, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	if (networkAddress == (unsigned char *)0) {
		return (IPX_BAD_PARAMETER);
	}
	ipx_pb.csCode = IPXGETINTERNETWORKADDRESS;
	ipx_pb.csParams.getInetAddr.networkAddress = networkAddress;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxGetLocalTarget(unsigned char *networkAddress, IPX_ECB *ecb,
					unsigned long *transportTime)
#else
IpxGetLocalTarget(unsigned char *networkAddress, IPX_ECB *ecb,
					unsigned long *transportTime, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	if (networkAddress == (unsigned char *)0) {
		return (IPX_BAD_PARAMETER);
	}
	/*
	 * Note: ecb and transportTime can both be zero if the user doesn't
	 *       want that information.
	 */

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb.csCode = IPXGETLOCALTARGET;
	ipx_pb.csParams.getLocalTgt.networkAddress = networkAddress;
	ipx_pb.csParams.getLocalTgt.ecb = ecb;
	ipx_pb.csParams.getLocalTgt.transportTime = transportTime;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxGetLocalTargetAsynch(unsigned char *networkAddress, IPX_ECB *ecb,
						IPX_ECB *recv_ecb, unsigned long *transportTime)
#else
IpxGetLocalTargetAsynch(unsigned char *networkAddress, IPX_ECB *ecb,
						IPX_ECB *recv_ecb, unsigned long *transportTime, short drvRefNum)
#endif
{
	IPXpb		*ipx_pb;
	short		status;
	
	if (networkAddress == (unsigned char *)0) {
		return (IPX_BAD_PARAMETER);
	}
	/*
	 * Note: ecb and transportTime can both be zero if the user doesn't
	 *       want that information.
	 */
#ifndef NO_ECB_CHECKS
	if (recv_ecb) {
		if ((recv_ecb->fragCount != 1) || 
		    (recv_ecb->fragList[0].fragSize < sizeof(routeResponsePacket_t))) {
			recv_ecb->status = IPX_BAD_PACKET;
			return (IPX_BAD_PACKET);
		}
	} else {
		return (IPX_BAD_PARAMETER);
	}
#endif

	if ((recv_ecb->status == ST_ECBQ) || (recv_ecb->status == ST_SENDPACKET)) {
		return (IPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	recv_ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb = (IPXpb *)recv_ecb;
	ipx_pb->ioCompletion = 0;
	ipx_pb->csCode = IPXGETLOCALTARGETASYNCH;
	ipx_pb->csParams.getLocalTgtAsynch.networkAddress = networkAddress;
	ipx_pb->csParams.getLocalTgtAsynch.resp_ecb = ecb;
	ipx_pb->csParams.getLocalTgtAsynch.recv_ecb = recv_ecb;
	ipx_pb->csParams.getLocalTgtAsynch.transportTime = transportTime;
	recv_ecb->status = ST_ECBQ;
#ifndef	COMPONENT_BUILD
	status = call_ipx(ipx_pb, TRUE);
#else
	status = call_ipx(ipx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		recv_ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}


pascal short
#ifndef	COMPONENT_BUILD
IpxGetMaxPacketSize(unsigned long *maxPktSize)
#else
IpxGetMaxPacketSize(unsigned long *maxPktSize, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	if (maxPktSize == (unsigned long *)0) {
		return (IPX_BAD_PARAMETER);
	}
	ipx_pb.csCode = IPXGETMAXPKTSIZE;
	ipx_pb.csParams.getMaxPktSize.maxPktSize = maxPktSize;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxGetStatistics(IPX_STATS *buffer, unsigned long *buflen)
#else
IpxGetStatistics(IPX_STATS *buffer, unsigned long *buflen, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	if ((buffer == (IPX_STATS *)0) || (buflen == (unsigned long *)0)) {
		return (IPX_BAD_PARAMETER);
	}
	ipx_pb.csCode = IPXGETSTATISTICS;
	ipx_pb.csParams.getStats.buffer = (unsigned char *)buffer;
	ipx_pb.csParams.getStats.buflen = buflen;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxGetVersion(unsigned char *majorVersion, unsigned char *minorVersion,
				unsigned short *revision)
#else
IpxGetVersion(unsigned char *majorVersion, unsigned char *minorVersion,
				unsigned short *revision, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	/*
	 * Note: Any of the arguments to this function can be zero if the
	 *       user doesn't want that information.
	 */
	ipx_pb.csCode = IPXGETVERSION;
	ipx_pb.csParams.version.majorVersion = majorVersion;
	ipx_pb.csParams.version.minorVersion = minorVersion;
	ipx_pb.csParams.version.revision = revision;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxInitialize()
#else
IpxInitialize(short *drvRefNumPtr)
#endif
{
	short	drvrRefNum, status;
	
	status = find_ipx_drvr(&drvrRefNum);
	if (status == 0) {
#ifndef	COMPONENT_BUILD
#ifdef CANNOT_USE_GLOBALS	
		Ipx_set_drvr_ref_num((long)drvrRefNum);
#else
		MacIPXDrvrRefNum = drvrRefNum;
#endif
#else
		*drvRefNumPtr = drvrRefNum;
#endif
	}
	return (status);
}

pascal short
#ifndef	COMPONENT_BUILD
IpxOpenLookAheadSocket(unsigned short *socketNetOrder, RecvLookAheadHandlerUPP handler,
						unsigned short numBytes, long userData, unsigned char flags)
#else
IpxOpenLookAheadSocket(unsigned short *socketNetOrder, RecvLookAheadHandlerUPP handler,
						unsigned short numBytes, long userData, unsigned char flags,
						short drvrRefNum)
#endif
{
	IPXpb		ipx_pb;

	if ((handler == (RecvLookAheadHandlerUPP)0) ||
			(numBytes < IPX_HDR_SIZE) ||
			(numBytes > MAX_LOOKAHEAD_LEN)) {
		return (IPX_BAD_PARAMETER);
	}
	ipx_pb.csCode = IPXOPENLOOKAHEADSOCKET;
	ipx_pb.csParams.openLookAhead.socketNumber = socketNetOrder;
	ipx_pb.csParams.openLookAhead.handler = handler;
	ipx_pb.csParams.openLookAhead.numBytes = numBytes;
	ipx_pb.csParams.openLookAhead.userData = userData;
#if !defined(powerc) && !defined (__powerc)
	ipx_pb.csParams.openLookAhead.savedA5 = (void *)IpxGetRegisterA5();
#endif
	ipx_pb.csParams.openLookAhead.flags = flags;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvrRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxOpenSocket(unsigned short *socketNetOrder, unsigned char flags)
#else
IpxOpenSocket(unsigned short *socketNetOrder, unsigned char flags, short drvRefNum)
#endif
{
	IPXpb		ipx_pb;
	
	ipx_pb.csCode = IPXOPENSOCKET;
	ipx_pb.csParams.open.socketNumber = socketNetOrder;
	ipx_pb.csParams.open.flags = flags;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxReceive(unsigned short socketNetOrder, IPX_ECB *ecb)
#else
IpxReceive(unsigned short socketNetOrder, IPX_ECB *ecb, short drvRefNum)
#endif
{
	IPXpb		*ipx_pb;
	short		status;

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB.
	 */
	if (ecb) {
		if ((ecb->fragCount < 1) ||
				(ecb->fragList[0].fragSize < IPX_HDR_SIZE)) {
			ecb->status = IPX_BAD_PACKET;
			return (IPX_BAD_PACKET);
		}
	} else {
		return (IPX_BAD_PARAMETER);
	}
#endif
	if ((ecb->status == ST_ECBQ) || (ecb->status == ST_SENDPACKET)) {
		return (IPX_ECB_IN_USE);
	}
	
#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb = (IPXpb *)ecb;
	ipx_pb->ioCompletion = 0;
	ipx_pb->csCode = IPXRECEIVE;
	ipx_pb->csParams.recv.socketNumber = socketNetOrder;
	ipx_pb->csParams.recv.ecb = ecb;
	ecb->status = ST_ECBQ;
#ifndef	COMPONENT_BUILD
	status = call_ipx(ipx_pb, TRUE);
#else
	status = call_ipx(ipx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

pascal short
#ifndef	COMPONENT_BUILD
IpxRelinquishControl()
#else
IpxRelinquishControl(short drvRefNum)
#endif
{
	IPXpb	ipx_pb;
	
	ipx_pb.csCode = IPXRELINQUISHCONTROL;
#ifndef	COMPONENT_BUILD
	return (call_ipx(&ipx_pb, FALSE));
#else
	return (call_ipx(&ipx_pb, FALSE, drvRefNum));
#endif
}

pascal short
#ifndef	COMPONENT_BUILD
IpxScheduleIpxEvent(unsigned short socketNetOrder, unsigned long timeTicks,
					IPX_ECB *ecb)
#else
IpxScheduleIpxEvent(unsigned short socketNetOrder, unsigned long timeTicks,
					IPX_ECB *ecb, short drvRefNum)
#endif
{
	IPXpb		*ipx_pb;
	short		status;

#ifndef NO_ECB_CHECKS
	if (ecb == (IPX_ECB *)0) {
		return (IPX_BAD_PARAMETER);
	}
#endif

	/*
	 * Check whether the delay time is greater than 1 hour
	 */
	if (timeTicks > (unsigned long)0x34BC0) {
		ecb->status = IPX_BAD_PARAMETER;
		return (IPX_BAD_PARAMETER);
	}

	if (ecb->status == ST_AESQ) {
		return (IPX_ECB_IN_USE);
	}
	
#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb = (IPXpb *)ecb;
	ipx_pb->ioCompletion = 0;
	ipx_pb->csCode = IPXSCHEDULEEVENT;
	ipx_pb->csParams.schedule.socketNumber = socketNetOrder;
	ipx_pb->csParams.schedule.timeTicks = timeTicks;
	ipx_pb->csParams.schedule.ecb = ecb;
	ecb->status = ST_AESQ;
#ifndef	COMPONENT_BUILD
	status = call_ipx(ipx_pb, TRUE);
#else
	status = call_ipx(ipx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

pascal short
#ifndef	COMPONENT_BUILD
IpxSend(unsigned short socketNetOrder, IPX_ECB *ecb)
#else
IpxSend(unsigned short socketNetOrder, IPX_ECB *ecb, short drvRefNum)
#endif
{
	IPXpb		*ipx_pb;
	short		status;

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB.
	 */
	if (ecb) {
		if (ecb->fragCount < 1) {
			ecb->status = IPX_BAD_PACKET;
			return (IPX_BAD_PACKET);
		}
	} else {
		return (IPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (IPX_ECB_IN_USE);
	}
	
#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb = (IPXpb *)ecb;
	ipx_pb->ioCompletion = 0;
	ipx_pb->csCode = IPXSEND;
	ipx_pb->csParams.send.socketNumber = socketNetOrder;
	ipx_pb->csParams.send.ecb = ecb;
	ecb->status = ST_SENDPACKET;
#ifndef	COMPONENT_BUILD
	status = call_ipx(ipx_pb, TRUE);
#else
	status = call_ipx(ipx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

pascal short
#ifndef	COMPONENT_BUILD
IpxSend2(unsigned short socketNetOrder, IPX_ECB *ecb, unsigned char flags)
#else
IpxSend2(unsigned short socketNetOrder, IPX_ECB *ecb, unsigned char flags, short drvRefNum)
#endif
{
	IPXpb	*ipx_pb;
	short	status;

	/*
	 * This routine is identical to IpxSend, EXCEPT that it allows
	 * you to control whether the IPX packet is checksummed or not.
	 */

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB.
	 */
	if (ecb) {
		if (ecb->fragCount < 1) {
			ecb->status = IPX_BAD_PACKET;
			return (IPX_BAD_PACKET);
		}
	} else {
		return (IPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (IPX_ECB_IN_USE);
	}
	
	if (flags & DO_NOT_CHECKSUM_PACKET) {
		ecb->flags |= NO_CHECKSUM_FLAG;
		ecb->flags &= (~CHECKSUM_FLAG);
	} else {
		ecb->flags |= CHECKSUM_FLAG;
		ecb->flags &= (~NO_CHECKSUM_FLAG);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	ipx_pb = (IPXpb *)ecb;
	ipx_pb->ioCompletion = 0;
	ipx_pb->csCode = IPXSEND;
	ipx_pb->csParams.send.socketNumber = socketNetOrder;
	ipx_pb->csParams.send.ecb = ecb;
	ecb->status = ST_SENDPACKET;
#ifndef	COMPONENT_BUILD
	status = call_ipx(ipx_pb, TRUE);
#else
	status = call_ipx(ipx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

short
#ifndef	COMPONENT_BUILD
call_ipx(IPXpb *ipx_pb, int async)
#else
call_ipx(IPXpb *ipx_pb, int async, short drvRefNum)
#endif
{
	short	err;

#ifndef	COMPONENT_BUILD
#ifdef CANNOT_USE_GLOBALS	
	ipx_pb->ioCRefNum = (short)Ipx_get_drvr_ref_num();
#else
	ipx_pb->ioCRefNum = MacIPXDrvrRefNum;
#endif
#else
	ipx_pb->ioCRefNum = drvRefNum;
#endif

	/*
	 * These calls don't use the PBControl glue, so some execution
	 * time is saved.
	 */
	if (async) {
		err = PBControlAsync((ParmBlkPtr)ipx_pb);
		if (err) {
			if (err == (short)0xFFEB) {
				/* 0xFFEB == -21 driver not found */
				err = IPX_NOT_INSTALLED;
			} else if (err == (short)0xFFE4) {
				/* 0xFFE4 == -28 driver not open */
				err = IPX_NOT_INSTALLED;
			}
		}
		return (err);
	} else {
		(void)PBControlSync((ParmBlkPtr)ipx_pb);
		if (ipx_pb->ioResult) {
			if (ipx_pb->ioResult == (short)0xFFEB) {
				/* 0xFFEB == -21 driver not found */
				ipx_pb->ioResult = IPX_NOT_INSTALLED;
			} else if (ipx_pb->ioResult == (short)0xFFE4) {
				/* 0xFFE4 == -28 driver not open */
				ipx_pb->ioResult = IPX_NOT_INSTALLED;
			}
		}
		return (ipx_pb->ioResult);
	}
}

short
find_ipx_drvr(short *drvrRefNum)
{
	char		drvr_name[10];
	IPXpb		ipx_pb;

	drvr_name[0] = 0x08;
	drvr_name[1] = '.';
	*(long *)&drvr_name[2] = 'NVL_';
	*(long *)&drvr_name[6] = 'IPX ';
	ipx_pb.ioNamePtr = drvr_name;
	ipx_pb.csCode = 0; /* We're actually setting the read/write permission here */
	(void)PBOpen((ParmBlkPtr)&ipx_pb, FALSE);
	if (ipx_pb.ioResult == 0) {
		*drvrRefNum = ipx_pb.ioCRefNum;
	}
	return (ipx_pb.ioResult);
}

#ifdef THINK_C
/*
 * Think C support.
 */
#ifdef CANNOT_USE_GLOBALS
static void
Ipx_set_drvr_ref_num(long refNum)
{
	asm {
		lea		@private_storage, a0
		move.l	refNum, (a0)
		bra.s	@2
		
extern Ipx_get_drvr_ref_num:
		lea		@private_storage, a0
		move.l	(a0), d0
		rts
		
	@private_storage	dc.l 	0

	@2
	}
}
#endif /* CANNOT_USE_GLOBALS */

#if !defined(powerc) && !defined (__powerc)
unsigned long
IpxGetRegisterA5()
{
	asm {
		move.l	a5, d0
	}
}
#endif

#ifdef REGISTER_A0_COMPATIBILITY
pascal IPX_ECB *
IpxGetECBFromRegisterA0()
{
	IPX_ECB		*ecb;
	
	asm {
		move.l	a0, ecb
	}
	return (ecb);
}
#endif
#endif /* THINK_C */

#ifdef __MWERKS__
/*
 * Metrowerks support.
 */
#ifdef CANNOT_USE_GLOBALS
static asm void
Ipx_set_drvr_ref_num(long refNum)
{
	lea		private_storage, a0
	move.l	4(a7), (a0)
	bra.s	L1
		
ENTRY static Ipx_get_drvr_ref_num
	lea		private_storage, a0
	move.l	(a0), d0
	rts
			
private_storage:	dc.l 	0

L1:
	rts
}
#endif /* CANNOT_USE_GLOBALS */

#if !defined(powerc) && !defined (__powerc)
asm unsigned long
IpxGetRegisterA5()
{
	move.l	a5, d0
	rts
}
#endif

#ifdef REGISTER_A0_COMPATIBILITY
pascal asm IPX_ECB *
IpxGetECBFromRegisterA0()
{
	move.l	a0, 4(a7)
	movea.l	(a7)+, a0
	jmp		(a0)
}
#endif
#endif /* __MWERKS__ */
