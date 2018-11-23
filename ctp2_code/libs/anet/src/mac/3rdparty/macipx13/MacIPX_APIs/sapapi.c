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
 * sapapi.c
 *
 * Copyright (C) 1992-1995 Novell, Inc.
 * All rights reserved.
 */

/*
 * Includes
 */
#include <Memory.h>
#include <Devices.h>
#include "ipxcalls.h"
#include "sappb.h"
#include "sap_internal.h"

/*
 * Defines
 */
#define IntSwap
#ifndef TRUE
#define TRUE					1
#endif
#ifndef FALSE
#define FALSE					0
#endif
#define strncpy					sap_strncpy

#ifdef strncpy
typedef unsigned long			size_t;
#endif /* strncpy */

/*
 * Prototypes
 */
extern short			call_sap(SAPpb *sap_pb);
#if !defined(powerc) && !defined (__powerc)
extern unsigned long	IpxGetRegisterA5(void);
#endif
#ifdef strncpy
char 					*sap_strncpy(char *dst, char *src, size_t n);
#else
#include <String.h>
#endif /* strncpy */

pascal short
AdvertiseService(unsigned short serviceType, char *serviceName,
                       unsigned short serviceSocket, unsigned long *adv_handle)
{
   	T_AdvertisingStruct *ASp;
	Handle				h;
	SAPpb				pb;
	short				status;
	unsigned int		i;
	char				*tmp;

	*adv_handle = 0L;

	if ((h = NewHandleClear(sizeof(T_AdvertisingStruct))) == (Handle)0) {
		return (SAP_NO_MEM);
	}
	MoveHHi(h);
	HLock(h);

	ASp = (T_AdvertisingStruct *)*h;
	ASp->signature = ADVERTISING_SIGNATURE;
   	ASp->ASServerIDpacket.SAPPacketType = IntSwap( PERIODIC_ID_PACKET );
   	ASp->ASServerIDpacket.serverType = IntSwap( serviceType );
	/*
	 * Make sure the serviceName isn't longer than 47 characters.
	 */
	for(i=0, tmp=serviceName; i<sizeof(ASp->ASServerIDpacket.serverName); i++, tmp++) {
		if (*tmp == 0) {
			break;
		}
	}
	if (i >= sizeof(ASp->ASServerIDpacket.serverName)) {
		serviceName[sizeof(ASp->ASServerIDpacket.serverName)-1] = 0;
	}
   	strncpy((char *)ASp->ASServerIDpacket.serverName, serviceName, sizeof(ASp->ASServerIDpacket.serverName));
   	ASp->ASServerIDpacket.serverSocket = serviceSocket;
   	ASp->ASServerIDpacket.interveningNetworks = IntSwap( 1 );
	pb.csCode = ADVERTISESERVICE;
	pb.csParams.adv_service.h = h;
	status = call_sap(&pb);
	if (status) {
		DisposeHandle(h);
		return (status);
	}
	*adv_handle = (unsigned long)h;
	return (SAP_SUCCESSFUL);
}

pascal short
ShutdownAdvertising(unsigned long adv_handle)
{
	T_AdvertisingStruct		*ASp;
	SAPpb					pb;
	short					status;
	Handle					h;

	if (adv_handle == 0L) {
		return (SAP_SUCCESSFUL);
	}

	h = (Handle)adv_handle;
	ASp = (T_AdvertisingStruct *)*h;
	if (ASp->signature != ADVERTISING_SIGNATURE) {
		return (SAP_INVALID_ARG);
	}

   	ASp->ASServerIDpacket.interveningNetworks = IntSwap( 16 );
	pb.csCode = SHUTDOWNADVERTISING;
	pb.csParams.adv_service.h = h;

	/*
	 * Assuming that the adv_handle is valid, if the driver is no
	 * longer open, we can simply dispose of the memory and return
	 * success. (i.e. the driver must have been open for adv_handle
	 * to have been allocated.)
	 */
	status = call_sap(&pb);
	if ((status == 0) || (status == (short)-28)) {
		DisposeHandle(h);
		status = SAP_SUCCESSFUL;
	}
	return (status);
}

pascal short
QueryServices(queryInfoPtr query_info, char async)
{
	SAPpb		pb;

	/*
	 * Valid query?
	 */
	if (query_info == (queryInfoPtr)0) {
		return (SAP_INVALID_ARG);
	}
	if ((query_info->queryType != GENERAL_SERVICE_QUERY) && (query_info->queryType != NEAREST_SERVICE_QUERY)) {
		return (SAP_INVALID_ARG);
	}

	if ((query_info->serviceType == 0xFFFF) && (query_info->queryType != GENERAL_SERVICE_QUERY)) {
		return (SAP_INVALID_ARG);
	}

	query_info->sap_entries = (SAP_RESPONSE_LIST_ENTRY *)0;

	pb.csCode = QUERYSERVICES;
	pb.csParams.query_service.query_info = query_info;
	pb.csParams.query_service.async = async;
	pb.csParams.query_service.appZone = GetZone();
#if !defined(powerc) && !defined (__powerc)
	pb.csParams.query_service.appA5 = IpxGetRegisterA5();
#endif

	/*
	 * Make a synchronous call to the driver.  An async QueryServices is
	 * handled by the driver rather than by the Mac OS.
	 */
	return (call_sap(&pb));
}

pascal short
AbortQueryServices(queryInfoPtr query_info)
{
	SAPpb		pb;

	/*
	 * Valid query?
	 */
	if (query_info == (queryInfoPtr)0) {
		return (SAP_INVALID_ARG);
	}

	pb.csCode = TERMINATEQUERYSERVICES;
	pb.csParams.query_service.query_info = query_info;
	return (call_sap(&pb));
}

pascal short
FreeQueryServicesList(SAP_RESPONSE_LIST_ENTRY *listP)
{
   	SAP_RESPONSE_LIST_ENTRY *nextListP;

	if (listP == (SAP_RESPONSE_LIST_ENTRY *)0) {
		return (SAP_SUCCESSFUL);
	}

	if (listP->signature != QUERY_LIST_SIGNATURE) {
		return (SAP_INVALID_ARG);
	}

   	for ( ; listP; listP=nextListP) {
      	nextListP = listP->next;
      	DisposePtr((Ptr)listP);
	}
   	return (SAP_SUCCESSFUL);
}

pascal short
SapRelinquishControl()
{
	SAPpb		pb;

	pb.csCode = SAPRELINQUISHCONTROL;
	return (call_sap(&pb));
}

short
call_sap(SAPpb *pb)
{
	short	status;
	SAPpb	sap_pb;
	char	sapDrvrName[10];

	sapDrvrName[0] = 0x08;
	sapDrvrName[1] = '.';
	*(long *)&sapDrvrName[2] = 'NVL_';
	*(long *)&sapDrvrName[6] = 'SAP ';
	sap_pb.ioNamePtr = sapDrvrName;
	sap_pb.csCode = 0; /* We're actually setting the read/write permission here */
	status = PBOpen((ParmBlkPtr)&sap_pb, FALSE);
	if (status) {
		goto Exit0;
	}

	pb->ioCRefNum = sap_pb.ioCRefNum;
	(void)PBControl((ParmBlkPtr)pb, FALSE);
	status = pb->ioResult;

Exit0:
	return (status);
}

/*
 * These routines are here so that you don't need to link to a C library to
 * use the SAP routines.  If you prefer to use the C library routines, simply
 * remove the #defines for strncpy at the beginning of this file.
 */
#ifdef strncpy
static char *
sap_strncpy(char *dst, char *src, size_t n)
{
	char	*p;

	p = dst;
	while (n--) {
		*p++ = *src;
		if (*src) {
			++src;
		}
	}
	return (dst);
}
#endif /* strncpy */
