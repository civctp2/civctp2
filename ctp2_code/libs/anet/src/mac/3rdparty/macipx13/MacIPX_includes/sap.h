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
 * sap.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */

#ifndef __SAP_H_INCLUDED__
#define __SAP_H_INCLUDED__

#include "types.h"

#define SAP_SOCKET               		(uint16)0x0452

#define GENERAL_SERVICE_QUERY    		1
#define GENERAL_SERVICE_RESPONSE 		2
#define NEAREST_SERVICE_QUERY    		3
#define NEAREST_SERVICE_RESPONSE 		4
#define PERIODIC_ID_PACKET       		2

#define NOT_SUPPORTED            		1
#define INVALID_QUERY_TYPE       		2

#define SAP_SUCCESSFUL			 		(sint16)0x0000
#define SAP_QUERY_IN_PROGRESS			(sint16)0x0001
#define SAP_QUERY_ABORTED				(sint16)0xFFFA
#define SAP_QUERY_NOT_FOUND				(sint16)0xFFFB
#define SAP_SERVICE_ALREADY_ADVERTISED	(sint16)0xFFFC
#define SAP_INVALID_ARG			 		(sint16)0xFFFD
#define SAP_NO_MEM				 		(sint16)0xFFFE
#define SAP_NO_RESPONSES		 		(sint16)0xFFFF

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct
 {
    uint16   SAPPacketType;          /* 2 or 4 */
    uint16   serverType;             /* assigned by Novell */
    uint8    serverName[48];         /* service name */
	uint32    serverNet;              /* server internetwork address */
	uint8    serverNode[6];
	uint16   serverSocket;
    uint16   interveningNetworks;    /* # of networks packet must traverse */
 } T_SAP_ID_PACKET;

typedef struct
 {
   uint16    queryType;               /* 1 or 3 */
   uint16    serverType;              /* assigned by Novell */
 } SERVICE_QUERY_PACKET;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#define SAP_RESPONSES_PER_PACKET    8
#define QUERY_LIST_SIGNATURE        'SAPL'

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct serverInfo
 {
	uint16    serverType;          /* assigned by Novell */
	uint8     serverName[48];      /* service name */
	uint32     serverNet;           /* server internetwork address */
	uint8     serverNode[6];
	uint16    serverSocket;
	uint16    interveningNetworks; /* # of networks packet must traverse */
} serverInfo;

typedef struct SAPResponse
 {
    uint16      SAPPacketType;       /* 2 or 4 */
    serverInfo 			responses[SAP_RESPONSES_PER_PACKET];
    struct SAPResponse *next;
    uint32       signature;
    sint16             	count;
 } SAP_RESPONSE_LIST_ENTRY;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#ifdef REGISTER_A0_COMPATIBILITY
typedef pascal void (*QSCompProcPtr)(void);
#else
typedef pascal void (*QSCompProcPtr)(struct queryInfo *qi);

#if USESROUTINEDESCRIPTORS
enum  {
	uppQSCompProcInfo = kPascalStackBased
						| RESULT_SIZE(kNoByteCode)
						| STACK_ROUTINE_PARAMETER(1, kFourByteCode)
};

typedef UniversalProcPtr QSCompUPP;

#define CallQSCompProc(userRoutine, qi_ptr)  \
	CallUniversalProc((UniversalProcPtr)(userRoutine), uppQSCompProcInfo, (qi_ptr))

#define NewQSCompProc(userRoutine)  \
	(QSCompUPP) NewRoutineDescriptor((ProcPtr)(userRoutine),  \
	uppQSCompProcInfo, GetCurrentISA())

#else
typedef QSCompProcPtr QSCompUPP;

#define NewQSCompProc(userRoutine)  \
	(QSCompUPP)((userRoutine))

#endif
#endif

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct queryInfo
{
	uint16			queryType;
	uint16			serviceType;
	uint16			retryCount;
	uint32			retryInterval;
	uint16			num_recv_ecbs;
	SAP_RESPONSE_LIST_ENTRY	*sap_entries;
	sint16					status;
#ifdef REGISTER_A0_COMPATIBILITY
	QSCompProcPtr			completion_routine;
#else
	QSCompUPP				completion_routine;
#endif
	void					*user_data;
} queryInfo, *queryInfoPtr;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * SAP APIs
 */
extern pascal sint16 AbortQueryServices(
         queryInfoPtr    query_info );

extern pascal sint16  AdvertiseService(
         uint16  __serviceType,
         char            *__serviceName,
         uint16  __serviceSocket,
		 uint32	 *__advertisingHandle );

extern pascal sint16 FreeQueryServicesList(
         SAP_RESPONSE_LIST_ENTRY *__listP );

extern pascal sint16 QueryServices(
         queryInfoPtr    query_info,
		 char            async );

extern pascal sint16 SapRelinquishControl(void);

extern pascal sint16 ShutdownAdvertising(
         uint32   __advertisingHandle );

#endif  /* __SAP_H_INCLUDED__ */
