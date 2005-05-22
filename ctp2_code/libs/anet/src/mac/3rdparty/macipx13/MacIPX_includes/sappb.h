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
 * sappb.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */
 
#ifndef __SAPPB_INCLUDED__
#define __SAPPB_INCLUDED__

#ifndef __TYPES__
#include <Types.h>
#endif

#ifndef __MEMORY__
#include <Memory.h>
#endif

#ifndef __SAP_H_INCLUDED__
#include "sap.h"
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
typedef struct {
	Handle					h;
} AdvertiseSrvc;

typedef struct {
	queryInfoPtr			query_info;
	char					async;
	THz						appZone;
	unsigned long			appA5;
} QuerySrvc;

typedef struct SAPpb {
	char					fill12[12];
	IOCompletionUPP			ioCompletion;
	short					ioResult;
	char					*ioNamePtr;
	short					ioVRefNum;
	short					ioCRefNum;
	short					csCode;
	union {
		AdvertiseSrvc	adv_service;
		QuerySrvc		query_service;
	} csParams;
} SAPpb;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif


/*
 * SAP csCodes
 */
#define	ADVERTISESERVICE			200
#define SHUTDOWNADVERTISING			201
#define QUERYSERVICES				202
#define TERMINATEQUERYSERVICES		203
#define SAPRELINQUISHCONTROL		204

#endif /* __SAPPB_INCLUDED__ */
