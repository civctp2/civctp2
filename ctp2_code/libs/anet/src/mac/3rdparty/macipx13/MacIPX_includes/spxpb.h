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
 * spxpb.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved
 */

#ifndef __SPXPB_INCLUDED__
#define __SPXPB_INCLUDED__

#ifndef __SPXCALLS_INCLUDED__
#include "spxcalls.h"
#endif

#ifndef __IPXPB_INCLUDED__
#include "ipxpb.h"
#endif

#include "types.h"

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct SPXCancelEvt {
	SPX_ECB					*ecb;
} SPXCancelEvt;

typedef struct SPXCheckSkt {
	uint16			socketNumber;
} SPXCheckSkt;

typedef struct SPXCloseSkt {
	uint16			socketNumber;
} SPXCloseSkt;

typedef struct SPXEstabConn {
	uint16			socketNumber;
	SPX_ECB					*ecb;
	uint8			retryCount;
	uint8			flags;
	uint16			*connection;
} SPXEstabConn;

typedef struct SPXEstabConn2 {
	uint16			socketNumber;
	SPX_ECB					*ecb;
	SPX_ECB					*listen_ecb;
	uint8			retryCount;
	uint8			flags;
	uint16			*connection;
} SPXEstabConn2;

typedef struct SPXGetConfig {
	uint16			*maxConn;
	uint16			*availConn;
} SPXGetConfig;

typedef struct SPXGetConnStatus {
	uint16			connection;
	SPX_SESSION				*buffer;
} SPXGetConnStatus;

typedef struct SPXGetStats {
	uint8			*buffer;
	uint32			*buflen;
} SPXGetStats;

typedef struct SPXGetVersion {
	uint8			*majorVersion;
	uint8			*minorVersion;
	uint16			*revision;
} SPXGetVersion;

typedef struct SPXListenConn {
	uint16			socketNumber;
	SPX_ECB					*ecb;
	uint8			retryCount;
	uint8			flags;
	uint16			*connection;
} SPXListenConn;

typedef struct SPXListenConn2 {
	uint16			socketNumber;
	SPX_ECB					*ecb;
	SPX_ECB					*listen_ecb;
	uint8			retryCount;
	uint8			flags;
	uint16			*connection;
} SPXListenConn2;

typedef struct SPXListenSeqPkt {
	uint16			connection;
	SPX_ECB					*ecb;
} SPXListenSeqPkt;

typedef struct SPXOpenSkt {
	uint16			*socketNumber;
	uint8			flags;
} SPXOpenSkt;

typedef struct SPXSendSeqPkt {
	uint16			connection;
	SPX_ECB					*ecb;
} SPXSendSeqPkt;

typedef struct SPXTermConn {
	uint16			connection;
	SPX_ECB					*ecb;
} SPXTermConn;

typedef struct SPXpb {
	char					fill12[12];
	IOCompletionUPP			ioCompletion;
	sint16					ioResult;
	char					*ioNamePtr;
	sint16					ioVRefNum;
	sint16					ioCRefNum;
	sint16					csCode;
	union {
		SPXTermConn			abort;
		SPXCancelEvt		cancel;
		SPXCheckSkt			check;
		SPXCloseSkt			close;
		SPXEstabConn		establish;
		SPXEstabConn2		establish2;
		SPXGetConfig		getconfig;
		SPXGetConnStatus	getconnstatus;
		SPXGetStats			getstats;
		SPXGetVersion		version;
		SPXListenConn		listenconn;
		SPXListenConn2		listenconn2;
		SPXOpenSkt			open;
		SPXListenSeqPkt		recv;
		SPXSendSeqPkt		send;
		SPXTermConn			terminate;
	} csParams;
} SPXpb;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * SPX csCodes
 */
#define SPXABORTCONN				200
#define	SPXCANCELEVENT				201
#define	SPXCHECKSOCKET				202
#define SPXCLOSESOCKET				203
#define SPXESTABLISHCONN			204
#define SPXGETCONFIG				205
#define SPXGETCONNSTATUS			206
#define	SPXGETVERSION				207
#define SPXLISTENCONN				208
#define SPXLISTENSEQPKT				209
#define	SPXOPENSOCKET				210
#define	SPXSENDSEQPKT				211
#define SPXTERMINATECONN			212
#define SPXGETSTATISTICS			213
#define SPXESTABLISHCONN2			215
#define SPXLISTENCONN2				216

/*
 * Prototypes
 */
#ifndef COMPONENT_BUILD
extern sint16	call_ipx(IPXpb *pb, sint32 async);
#else
extern sint16	call_ipx(IPXpb *pb, sint32 async, sint16 drvrRefNum);
#endif

#endif /* __SPXPB_INCLUDED__ */
