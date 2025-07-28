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
 * spxapi.c
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 *
 * Note 1: Once your application is debugged, you can improve performance of
 *         these APIs slightly by defining the constant NO_ECB_CHECKS.
 *
 * Note 2: This file can be compiled for use with the Component Manager by
 *         defining the constant COMPONENT_BUILD.  When this constant is defined,
 *         all the SPX routines which make a call to "call_ipx" are modified
 *         to accept an extra argument for the driver reference number. This
 *         driver reference number is normally stored away for you automatically
 *         for performance reasons.  A Macintosh Component, however, may not have
 *         global data and its code may be moved, even across heap zones. These
 *         conditions dissallow our normal method of hiding the driver reference
 *         number for you. These conditions require you to save the driver
 *         reference number from the SPXInitialize API and you must pass it into
 *         all the other SPX APIs.
 */

/*
 * Include files
 */
#include <Devices.h>
#include <Memory.h>
#include "spxcalls.h"
#include "spxerror.h"
#include "ipxerror.h"
#include "spxpb.h"
#include <string.h>

/*
 * Defines
 */
#ifndef TRUE
#define TRUE					1
#endif
#ifndef FALSE
#define	FALSE					0
#endif

/*
 * Prototypes
 */
#if !defined(powerc) && !defined (__powerc)
extern uint32	IpxGetRegisterA5(void);
#endif
static sint16			check_establish_ecb(SPX_ECB *ecb);

pascal sint16
#ifndef	COMPONENT_BUILD
SpxAbortConnection(uint16 connection, SPX_ECB *ecb)
#else
SpxAbortConnection(uint16 connection, SPX_ECB *ecb, sint16 drvRefNum)
#endif
{
	SPXpb		*spx_pb;
	sint16		status;

#ifndef NO_ECB_CHECKS
	if (ecb == (SPX_ECB *)0) {
		return (SPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXABORTCONN;
	spx_pb->csParams.abort.connection = connection;
	spx_pb->csParams.abort.ecb = ecb;
	ecb->status = ST_SENDPACKET;
#ifndef	COMPONENT_BUILD
	status = call_ipx((IPXpb *)spx_pb, TRUE);
#else
	status = call_ipx((IPXpb *)spx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxCancelEvent(SPX_ECB *ecb)
#else
SpxCancelEvent(SPX_ECB *ecb, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

#ifndef NO_ECB_CHECKS
	if (ecb == (SPX_ECB *)0) {
		return (SPX_BAD_PARAMETER);
	}
#endif

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb.csCode = SPXCANCELEVENT;
	spx_pb.csParams.cancel.ecb = ecb;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxCheckSocket(uint16 socketNetOrder)
#else
SpxCheckSocket(uint16 socketNetOrder, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	spx_pb.csCode = SPXCHECKSOCKET;
	spx_pb.csParams.check.socketNumber = socketNetOrder;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxCloseSocket(uint16 socketNetOrder)
#else
SpxCloseSocket(uint16 socketNetOrder, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	spx_pb.csCode = SPXCLOSESOCKET;
	spx_pb.csParams.close.socketNumber = socketNetOrder;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxEstablishConnection(uint16 socketNetOrder, SPX_ECB *ecb, uint8 retryCount,
			uint8 flags, uint16 *connection)
#else
SpxEstablishConnection(uint16 socketNetOrder, SPX_ECB *ecb, uint8 retryCount,
			uint8 flags, uint16 *connection, sint16 drvRefNum)
#endif
{
	SPXpb	*spx_pb;
	sint16	err_code;

	if (connection == (uint16 *)0) {
		return (SPX_BAD_PARAMETER);
	}

	err_code = check_establish_ecb(ecb);
	if (err_code) {
		return (err_code);
	}

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXESTABLISHCONN;
	spx_pb->csParams.establish.socketNumber = socketNetOrder;
	spx_pb->csParams.establish.ecb = ecb;
	spx_pb->csParams.establish.retryCount = retryCount;
	spx_pb->csParams.establish.flags = flags;
	spx_pb->csParams.establish.connection = connection;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxEstablishConnection2(uint16 socketNetOrder, SPX_ECB *ecb, SPX_ECB *listen_ecb,
			 uint8 retryCount, uint8 flags, uint16 *connection)
#else
SpxEstablishConnection2(uint16 socketNetOrder, SPX_ECB *ecb, SPX_ECB *listen_ecb,
			 uint8 retryCount, uint8 flags, uint16 *connection, sint16 drvRefNum)
#endif
{
	SPXpb	*spx_pb;
	sint16	err_code;

	if (connection == (uint16 *)0) {
		return (SPX_BAD_PARAMETER);
	}

	err_code = check_establish_ecb(ecb);
	if (err_code) {
		return (err_code);
	}

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif

#ifndef NO_ECB_CHECKS
	/*
	 * Check validity of the listen ECB
	 */
	if (listen_ecb) {
		if ((listen_ecb->fragCount < 1) || (listen_ecb->fragList[0].fragSize < SPX_HDR_SIZE)) {
			return (SPX_BAD_PACKET);
		}
		if (listen_ecb->status == ST_ECBQ) {
			return (SPX_ECB_IN_USE);
		}
#if !defined(powerc) && !defined (__powerc)
		listen_ecb->savedA5 = ecb->savedA5;
#endif
	}
#else
	if (listen_ecb) {
		if ((listen_ecb->status == ST_ECBQ) || (listen_ecb->status == ST_SENDPACKET)) {
			return (SPX_ECB_IN_USE);
		}
#if !defined(powerc) && !defined (__powerc)
		listen_ecb->savedA5 = ecb->savedA5;
#endif
	}
#endif

	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXESTABLISHCONN2;
	spx_pb->csParams.establish2.socketNumber = socketNetOrder;
	spx_pb->csParams.establish2.ecb = ecb;
	spx_pb->csParams.establish2.listen_ecb = listen_ecb;
	spx_pb->csParams.establish2.retryCount = retryCount;
	spx_pb->csParams.establish2.flags = flags;
	spx_pb->csParams.establish2.connection = connection;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)spx_pb, FALSE, drvRefNum));
#endif
}

static sint16
check_establish_ecb(SPX_ECB *ecb)
{
	SPX_HEADER	*spxhdr;

#ifndef NO_ECB_CHECKS
	/*
	 * Check validity of the ECB
	 */
	if (ecb) {
		if ((ecb->fragCount != 1) || (ecb->fragList[0].fragSize != SPX_HDR_SIZE)) {
			return (SPX_BAD_PACKET);
		}
	} else {
		return (SPX_BAD_PARAMETER);
	}
#endif

	/*
	 * Is this packet being sent to a broadcast address (which is illegal)?
	 */
	spxhdr = (SPX_HEADER *)ecb->fragList[0].fragAddress;
	if ((*(uint32 *)spxhdr->destNode == 0xFFFFFFFF) &&
			(*(uint16 *)&spxhdr->destNode[4] == 0xFFFF)) {
		return (SPX_BAD_PACKET);
	}
	return (0);
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxGetConfiguration(uint16 *maxConn, uint16 *availConn)
#else
SpxGetConfiguration(uint16 *maxConn, uint16 *availConn, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	/*
	 * Note: Any of the arguments to this function can be zero if the
	 *       user doesn't want that information.
	 */
	spx_pb.csCode = SPXGETCONFIG;
	spx_pb.csParams.getconfig.maxConn = maxConn;
	spx_pb.csParams.getconfig.availConn = availConn;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxGetConnectionStatus(uint16 connection, SPX_SESSION *buffer)
#else
SpxGetConnectionStatus(uint16 connection, SPX_SESSION *buffer, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	if (buffer == (SPX_SESSION *)0) {
		return (SPX_BAD_PARAMETER);
	}
	spx_pb.csCode = SPXGETCONNSTATUS;
	spx_pb.csParams.getconnstatus.connection = connection;
	spx_pb.csParams.getconnstatus.buffer = buffer;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxGetStatistics(SPX_STATS *buffer, uint32 *buflen)
#else
SpxGetStatistics(SPX_STATS *buffer, uint32 *buflen, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	if ((buffer == (SPX_STATS *)0) || (buflen == (uint32 *)0)) {
		return (SPX_BAD_PARAMETER);
	}
	spx_pb.csCode = SPXGETSTATISTICS;
	spx_pb.csParams.getstats.buffer = (uint8 *)buffer;
	spx_pb.csParams.getstats.buflen = buflen;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxGetVersion(uint8 *majorVersion, uint8 *minorVersion, uint16 *revision)
#else
SpxGetVersion(uint8 *majorVersion, uint8 *minorVersion, uint16 *revision, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	/*
	 * Note: Any of the arguments to this function can be zero if the
	 *       user doesn't want that information.
	 */
	spx_pb.csCode = SPXGETVERSION;
	spx_pb.csParams.version.majorVersion = majorVersion;
	spx_pb.csParams.version.minorVersion = minorVersion;
	spx_pb.csParams.version.revision = revision;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxListenForConnection(uint16 socketNetOrder, SPX_ECB *ecb, uint8 retryCount,
			uint8 flags, uint16 *connection)
#else
SpxListenForConnection(uint16 socketNetOrder, SPX_ECB *ecb, uint8 retryCount,
			uint8 flags, uint16 *connection, sint16 drvRefNum)
#endif
{
	SPXpb		*spx_pb;

	if (connection == (uint16 *)0) {
		return (SPX_BAD_PARAMETER);
	}

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB
	 */
	if (ecb) {
		if ((ecb->fragCount < 1) || (ecb->fragList[0].fragSize < SPX_HDR_SIZE)) {
			return (SPX_BAD_PACKET);
		}
	} else {
		return (SPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_ECBQ) || (ecb->status == ST_SENDPACKET)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXLISTENCONN;
	spx_pb->csParams.listenconn.socketNumber = socketNetOrder;
	spx_pb->csParams.listenconn.ecb = ecb;
	spx_pb->csParams.listenconn.retryCount = retryCount;
	spx_pb->csParams.listenconn.flags = flags;
	spx_pb->csParams.listenconn.connection = connection;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxListenForConnection2(uint16 socketNetOrder, SPX_ECB *ecb, SPX_ECB *listen_ecb,
			uint8 retryCount, uint8 flags, uint16 *connection)
#else
SpxListenForConnection2(uint16 socketNetOrder, SPX_ECB *ecb, SPX_ECB *listen_ecb,
			uint8 retryCount, uint8 flags, uint16 *connection, sint16 drvRefNum)
#endif
{
	SPXpb		*spx_pb;

	if (connection == (uint16 *)0) {
		return (SPX_BAD_PARAMETER);
	}

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB(s)
	 */
	if (ecb) {
		if ((ecb->fragCount < 1) || (ecb->fragList[0].fragSize < SPX_HDR_SIZE)) {
			return (SPX_BAD_PACKET);
		}
	} else {
		return (SPX_BAD_PARAMETER);
	}
	if ((ecb->status == ST_ECBQ) || (ecb->status == ST_SENDPACKET)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif

	if (listen_ecb) {
		if ((listen_ecb->fragCount < 1) || (listen_ecb->fragList[0].fragSize < SPX_HDR_SIZE)) {
			return (SPX_BAD_PACKET);
		}
		if ((listen_ecb->status == ST_ECBQ) || (listen_ecb->status == ST_SENDPACKET)) {
			return (SPX_ECB_IN_USE);
		}
#if !defined(powerc) && !defined (__powerc)
		listen_ecb->savedA5 = ecb->savedA5;
#endif
	}
#else
	if ((ecb->status == ST_ECBQ) || (ecb->status == ST_SENDPACKET)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	if (listen_ecb) {
		if ((listen_ecb->status == ST_ECBQ) || (listen_ecb->status == ST_SENDPACKET)) {
			return (SPX_ECB_IN_USE);
		}
#if !defined(powerc) && !defined (__powerc)
		listen_ecb->savedA5 = ecb->savedA5;
#endif
	}
#endif

	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXLISTENCONN2;
	spx_pb->csParams.listenconn2.socketNumber = socketNetOrder;
	spx_pb->csParams.listenconn2.ecb = ecb;
	spx_pb->csParams.listenconn2.listen_ecb = listen_ecb;
	spx_pb->csParams.listenconn2.retryCount = retryCount;
	spx_pb->csParams.listenconn2.flags = flags;
	spx_pb->csParams.listenconn2.connection = connection;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxListenForConnectionPacket(uint16 connection, SPX_ECB *ecb)
#else
SpxListenForConnectionPacket(uint16 connection, SPX_ECB *ecb, sint16 drvRefNum)
#endif
{
	SPXpb		*spx_pb;
	sint16		status;

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB
	 */
	if (ecb) {
		if ((ecb->fragCount < 1) || (ecb->fragList[0].fragSize < SPX_HDR_SIZE)) {
			ecb->status = SPX_BAD_PACKET;
			return (SPX_BAD_PACKET);
		}
	} else {
		return (SPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_ECBQ) || (ecb->status == ST_SENDPACKET)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXLISTENSEQPKT;
	spx_pb->csParams.recv.connection = connection;
	spx_pb->csParams.recv.ecb = ecb;
	ecb->status = ST_ECBQ;
#ifndef	COMPONENT_BUILD
	status = call_ipx((IPXpb *)spx_pb, TRUE);
#else
	status = call_ipx((IPXpb *)spx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxOpenSocket(uint16 *socketNetOrder, uint8 flags)
#else
SpxOpenSocket(uint16 *socketNetOrder, uint8 flags, sint16 drvRefNum)
#endif
{
	SPXpb		spx_pb;

	if (flags & WANT_BROADCASTS) {
		/*
		 * Sorry, SPX cannot receive broadcasts.
		 */
		return (SPX_BAD_PARAMETER);
	}

	spx_pb.csCode = SPXOPENSOCKET;
	spx_pb.csParams.open.socketNumber = socketNetOrder;
	spx_pb.csParams.open.flags = flags;
#ifndef	COMPONENT_BUILD
	return (call_ipx((IPXpb *)&spx_pb, FALSE));
#else
	return (call_ipx((IPXpb *)&spx_pb, FALSE, drvRefNum));
#endif
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxSendSequencedPacket(uint16 connection, SPX_ECB *ecb)
#else
SpxSendSequencedPacket(uint16 connection, SPX_ECB *ecb, sint16 drvRefNum)
#endif
{
	SPXpb		*spx_pb;
	sint16		status;

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB
	 */
	if (ecb) {
		if ((ecb->fragCount < 1) ||
						(ecb->fragList[0].fragSize < SPX_HDR_SIZE)) {
			ecb->status = SPX_BAD_PACKET;
			return (SPX_BAD_PACKET);
		}
	} else {
		return (SPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXSENDSEQPKT;
	spx_pb->csParams.send.connection = connection;
	spx_pb->csParams.send.ecb = ecb;
	ecb->status = ST_SENDPACKET;
#ifndef	COMPONENT_BUILD
	status = call_ipx((IPXpb *)spx_pb, TRUE);
#else
	status = call_ipx((IPXpb *)spx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}

pascal sint16
#ifndef	COMPONENT_BUILD
SpxTerminateConnection(uint16 connection, SPX_ECB *ecb)
#else
SpxTerminateConnection(uint16 connection, SPX_ECB *ecb, sint16 drvRefNum)
#endif
{
	SPXpb	*spx_pb;
	sint16	status;

#ifndef NO_ECB_CHECKS
	/*
	 * Check the validity of the ECB
	 */
	if (ecb) {
		if ((ecb->fragCount != 1) || (ecb->fragList[0].fragSize != SPX_HDR_SIZE)) {
			ecb->status = SPX_BAD_PACKET;
			return (SPX_BAD_PACKET);
		}
	} else {
		return (SPX_BAD_PARAMETER);
	}
#endif

	if ((ecb->status == ST_SENDPACKET) || (ecb->status == ST_ECBQ)) {
		return (SPX_ECB_IN_USE);
	}

#if !defined(powerc) && !defined (__powerc)
	ecb->savedA5 = IpxGetRegisterA5();
#endif
	spx_pb = (SPXpb *)ecb;
	spx_pb->ioCompletion = 0;
	spx_pb->csCode = SPXTERMINATECONN;
	spx_pb->csParams.send.connection = connection;
	spx_pb->csParams.send.ecb = ecb;
	ecb->status = ST_SENDPACKET;
#ifndef	COMPONENT_BUILD
	status = call_ipx((IPXpb *)spx_pb, TRUE);
#else
	status = call_ipx((IPXpb *)spx_pb, TRUE, drvRefNum);
#endif
	if (status) {
		ecb->status = IPX_NOT_INSTALLED;
	}
	return (status);
}
