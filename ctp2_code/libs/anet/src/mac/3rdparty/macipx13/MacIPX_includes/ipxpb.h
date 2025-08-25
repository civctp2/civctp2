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
 * ipxpb.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved
 */

#ifndef __IPXPB_INCLUDED__
#define __IPXPB_INCLUDED__

#ifndef __IPXCALLS_INCLUDED__
#include "ipxcalls.h"
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef NewIOCompletionProc
/*
 * Not using Apple's Universal Includes, so
 * define IOCompletionUPP here.
 */
typedef	ProcPtr	IOCompletionUPP;
#endif

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct IPXCancelEvt {
	uint16			socketNumber;
	IPX_ECB					*ecb;
} IPXCancelEvt;

typedef struct IPXCheckSkt {
	uint16			socketNumber;
} IPXCheckSkt;

typedef struct IPXCloseSkt {
	uint16			socketNumber;
} IPXCloseSkt;

typedef struct IPXGetInetAddr {
	uint8			*networkAddress;
} IPXGetInetAddr;

typedef struct IPXGetInfo {
	IPX_CONFIG_INFO			*buf;
	uint32			*buflen;
} IPXGetInfo;

typedef struct IPXGetLocalTgt {
	uint8			*networkAddress;
	IPX_ECB					*ecb;
	uint32			*transportTime;
} IPXGetLocalTgt;

typedef struct IPXGetLocalTgtAsynch {
	uint8			*networkAddress;
	IPX_ECB					*resp_ecb;
	IPX_ECB					*recv_ecb;
	uint32			*transportTime;
} IPXGetLocalTgtAsynch;

typedef struct IPXGetMaxPktSze {
	uint32			*maxPktSize;
} IPXGetMaxPktSze;

typedef struct IPXGetStats {
	uint8			*buffer;
	uint32			*buflen;
} IPXGetStats;

typedef struct IPXGetVersion {
	uint8			*majorVersion;
	uint8			*minorVersion;
	uint16			*revision;
} IPXGetVersion;

typedef struct IPXOpenLookAheadSkt {
	uint16			*socketNumber;
	RecvLookAheadHandlerUPP	handler;
	uint16			numBytes;
	sint32					userData;
	void					*savedA5;
	uint8			flags;
} IPXOpenLookAheadSkt;

typedef struct IPXOpenSkt {
	uint16			*socketNumber;
	uint8			flags;
} IPXOpenSkt;

typedef struct IPXRcv {
	uint16			socketNumber;
	IPX_ECB					*ecb;
} IPXRcv;

typedef struct IPXsched {
	uint16			socketNumber;
	uint32			timeTicks;
	IPX_ECB					*ecb;
} IPXSched;

typedef struct IPXSnd {
	uint16			socketNumber;
	IPX_ECB					*ecb;
} IPXSnd;

typedef struct IPXpb {
	char					fill12[12];
	IOCompletionUPP			ioCompletion;
	sint16					ioResult;
	char					*ioNamePtr;
	sint16					ioVRefNum;
	sint16					ioCRefNum;
	sint16					csCode;
	union {
		IPXCancelEvt			cancel;
		IPXCheckSkt				check;
		IPXCloseSkt				close;
		IPXGetInetAddr			getInetAddr;
		IPXGetInfo				getInfo;
		IPXGetLocalTgt			getLocalTgt;
		IPXGetLocalTgtAsynch	getLocalTgtAsynch;
		IPXGetMaxPktSze 		getMaxPktSize;
		IPXGetStats				getStats;
		IPXGetVersion			version;
		IPXOpenSkt				open;
		IPXOpenLookAheadSkt		openLookAhead;
		IPXRcv					recv;
		IPXSched				schedule;
		IPXSnd					send;
	} csParams;
} IPXpb;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * IPX csCodes
 */
#define	IPXCANCELEVENT				100
#define	IPXCHECKSOCKET				101
#define	IPXCLOSESOCKET				102
#define IPXGETANDCLEARQ				103
#define	IPXGETINTERNETWORKADDRESS	104
#define	IPXGETLOCALTARGET			105
#define	IPXGETSTATISTICS			106
#define	IPXGETVERSION				107
#define	IPXOPENSOCKET				108
#define	IPXRECEIVE					109
#define IPXRELINQUISHCONTROL		110
#define IPXSCHEDULEEVENT			112
#define	IPXSEND						113
#define IPXGETMAXPKTSIZE			114
#define IPXGETINFO					115
#define	IPXGETLOCALTARGETASYNCH		116
#define IPXOPENLOOKAHEADSOCKET		117

/*
 * Prototypes
 */
#ifndef COMPONENT_BUILD
extern sint16	call_ipx(IPXpb *pb, sint32 async);
#else
extern sint16	call_ipx(IPXpb *pb, sint32 async, sint16 drvrRefNum);
#endif

#endif /* __IPXPB_INCLUDED__ */
