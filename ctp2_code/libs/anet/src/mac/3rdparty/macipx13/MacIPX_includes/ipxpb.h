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
	unsigned short			socketNumber;
	IPX_ECB					*ecb;
} IPXCancelEvt;

typedef struct IPXCheckSkt {
	unsigned short			socketNumber;
} IPXCheckSkt;

typedef struct IPXCloseSkt {
	unsigned short			socketNumber;
} IPXCloseSkt;

typedef struct IPXGetInetAddr {
	unsigned char			*networkAddress;
} IPXGetInetAddr;

typedef struct IPXGetInfo {
	IPX_CONFIG_INFO			*buf;
	unsigned long			*buflen;
} IPXGetInfo;

typedef struct IPXGetLocalTgt {
	unsigned char			*networkAddress;
	IPX_ECB					*ecb;
	unsigned long			*transportTime;
} IPXGetLocalTgt;

typedef struct IPXGetLocalTgtAsynch {
	unsigned char			*networkAddress;
	IPX_ECB					*resp_ecb;
	IPX_ECB					*recv_ecb;
	unsigned long			*transportTime;
} IPXGetLocalTgtAsynch;

typedef struct IPXGetMaxPktSze {
	unsigned long			*maxPktSize;
} IPXGetMaxPktSze;

typedef struct IPXGetStats {
	unsigned char			*buffer;
	unsigned long			*buflen;
} IPXGetStats;

typedef struct IPXGetVersion {
	unsigned char			*majorVersion;
	unsigned char			*minorVersion;
	unsigned short			*revision;
} IPXGetVersion;

typedef struct IPXOpenLookAheadSkt {
	unsigned short			*socketNumber;
	RecvLookAheadHandlerUPP	handler;
	unsigned short			numBytes;
	long					userData;
	void					*savedA5;
	unsigned char			flags;
} IPXOpenLookAheadSkt;

typedef struct IPXOpenSkt {
	unsigned short			*socketNumber;
	unsigned char			flags;
} IPXOpenSkt;

typedef struct IPXRcv {
	unsigned short			socketNumber;
	IPX_ECB					*ecb;
} IPXRcv;

typedef struct IPXsched {
	unsigned short			socketNumber;
	unsigned long			timeTicks;
	IPX_ECB					*ecb;
} IPXSched;

typedef struct IPXSnd {
	unsigned short			socketNumber;
	IPX_ECB					*ecb;
} IPXSnd;

typedef struct IPXpb {
	char					fill12[12];
	IOCompletionUPP			ioCompletion;
	short					ioResult;
	char					*ioNamePtr;
	short					ioVRefNum;
	short					ioCRefNum;
	short					csCode;
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
extern short	call_ipx(IPXpb *pb, int async);
#else
extern short	call_ipx(IPXpb *pb, int async, short drvrRefNum);
#endif

#endif /* __IPXPB_INCLUDED__ */
