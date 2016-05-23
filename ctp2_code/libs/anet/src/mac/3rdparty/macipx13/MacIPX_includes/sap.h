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

#define SAP_SOCKET               		(unsigned short)0x0452

#define GENERAL_SERVICE_QUERY    		1
#define GENERAL_SERVICE_RESPONSE 		2
#define NEAREST_SERVICE_QUERY    		3
#define NEAREST_SERVICE_RESPONSE 		4
#define PERIODIC_ID_PACKET       		2

#define NOT_SUPPORTED            		1
#define INVALID_QUERY_TYPE       		2

#define SAP_SUCCESSFUL			 		(short)0x0000
#define SAP_QUERY_IN_PROGRESS			(short)0x0001
#define SAP_QUERY_ABORTED				(short)0xFFFA
#define SAP_QUERY_NOT_FOUND				(short)0xFFFB
#define SAP_SERVICE_ALREADY_ADVERTISED	(short)0xFFFC
#define SAP_INVALID_ARG			 		(short)0xFFFD
#define SAP_NO_MEM				 		(short)0xFFFE
#define SAP_NO_RESPONSES		 		(short)0xFFFF

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct
 {
    unsigned short   SAPPacketType;          /* 2 or 4 */
    unsigned short   serverType;             /* assigned by Novell */
    unsigned char    serverName[48];         /* service name */
	unsigned long    serverNet;              /* server internetwork address */
	unsigned char    serverNode[6];
	unsigned short   serverSocket;
    unsigned short   interveningNetworks;    /* # of networks packet must traverse */
 } T_SAP_ID_PACKET;

typedef struct
 {
   unsigned short    queryType;               /* 1 or 3 */
   unsigned short    serverType;              /* assigned by Novell */
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
	unsigned short    serverType;          /* assigned by Novell */
	unsigned char     serverName[48];      /* service name */
	unsigned long     serverNet;           /* server internetwork address */
	unsigned char     serverNode[6];
	unsigned short    serverSocket;
	unsigned short    interveningNetworks; /* # of networks packet must traverse */
} serverInfo;

typedef struct SAPResponse
 {
    unsigned short      SAPPacketType;       /* 2 or 4 */
    serverInfo 			responses[SAP_RESPONSES_PER_PACKET];
    struct SAPResponse *next;
    unsigned long       signature;
    short             	count;
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
	unsigned short			queryType;
	unsigned short			serviceType;
	unsigned short			retryCount;
	unsigned long			retryInterval;
	unsigned short			num_recv_ecbs;
	SAP_RESPONSE_LIST_ENTRY	*sap_entries;
	short					status;
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
extern pascal short AbortQueryServices(
         queryInfoPtr    query_info );

extern pascal short  AdvertiseService(
         unsigned short  __serviceType,
         char            *__serviceName,
         unsigned short  __serviceSocket,
		 unsigned long	 *__advertisingHandle );

extern pascal short FreeQueryServicesList(
         SAP_RESPONSE_LIST_ENTRY *__listP );

extern pascal short QueryServices(
         queryInfoPtr    query_info,
		 char            async );

extern pascal short SapRelinquishControl(void);

extern pascal short ShutdownAdvertising(
         unsigned long   __advertisingHandle );

#endif  /* __SAP_H_INCLUDED__ */
