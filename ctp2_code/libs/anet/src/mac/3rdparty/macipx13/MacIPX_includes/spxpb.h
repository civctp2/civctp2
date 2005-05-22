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

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct SPXCancelEvt {
	SPX_ECB					*ecb;
} SPXCancelEvt;

typedef struct SPXCheckSkt {
	unsigned short			socketNumber;
} SPXCheckSkt;

typedef struct SPXCloseSkt {
	unsigned short			socketNumber;
} SPXCloseSkt;

typedef struct SPXEstabConn {
	unsigned short			socketNumber;
	SPX_ECB					*ecb;
	unsigned char			retryCount;
	unsigned char			flags;
	unsigned short			*connection;
} SPXEstabConn;

typedef struct SPXEstabConn2 {
	unsigned short			socketNumber;
	SPX_ECB					*ecb;
	SPX_ECB					*listen_ecb;
	unsigned char			retryCount;
	unsigned char			flags;
	unsigned short			*connection;
} SPXEstabConn2;

typedef struct SPXGetConfig {
	unsigned short			*maxConn;
	unsigned short			*availConn;
} SPXGetConfig;

typedef struct SPXGetConnStatus {
	unsigned short			connection;
	SPX_SESSION				*buffer;
} SPXGetConnStatus;

typedef struct SPXGetStats {
	unsigned char			*buffer;
	unsigned long			*buflen;
} SPXGetStats;

typedef struct SPXGetVersion {
	unsigned char			*majorVersion;
	unsigned char			*minorVersion;
	unsigned short			*revision;
} SPXGetVersion;

typedef struct SPXListenConn {
	unsigned short			socketNumber;
	SPX_ECB					*ecb;
	unsigned char			retryCount;
	unsigned char			flags;
	unsigned short			*connection;
} SPXListenConn;

typedef struct SPXListenConn2 {
	unsigned short			socketNumber;
	SPX_ECB					*ecb;
	SPX_ECB					*listen_ecb;
	unsigned char			retryCount;
	unsigned char			flags;
	unsigned short			*connection;
} SPXListenConn2;

typedef struct SPXListenSeqPkt {
	unsigned short			connection;
	SPX_ECB					*ecb;
} SPXListenSeqPkt;

typedef struct SPXOpenSkt {
	unsigned short			*socketNumber;
	unsigned char			flags;
} SPXOpenSkt;

typedef struct SPXSendSeqPkt {
	unsigned short			connection;
	SPX_ECB					*ecb;
} SPXSendSeqPkt;

typedef struct SPXTermConn {
	unsigned short			connection;
	SPX_ECB					*ecb;
} SPXTermConn;

typedef struct SPXpb {
	char					fill12[12];
	IOCompletionUPP			ioCompletion;
	short					ioResult;
	char					*ioNamePtr;
	short					ioVRefNum;
	short					ioCRefNum;
	short					csCode;
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
extern short	call_ipx(IPXpb *pb, int async);
#else
extern short	call_ipx(IPXpb *pb, int async, short drvrRefNum);
#endif

#endif /* __SPXPB_INCLUDED__ */
