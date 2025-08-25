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
 * binderypb.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved
 */

#ifndef __BINDERYPB_INCLUDED__
#define __BINDERYPB_INCLUDED__

#ifndef __IPXPB_INCLUDED__
#include "ipxpb.h"
#endif

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct CreateConnection {
	uint8			*serverAddress;
} CreateConnection;

typedef struct ReadPropertyValue {
	uint16			objectType;
	char					*objectName;
	char					segmentNumber;
	char					*propertyName;
	uint8			*propertyValue;
	uint8			*moreSegments;
	uint8			*propertyFlags;
} ReadPropertyValue;

typedef struct ScanBinderyObject {
	char					*searchObjectName;
	uint16			searchObjectType;
	sint32					*objectID;
	char					*objectName;
	uint16			*objectType;
	char					*objectHasProperties;
	char					*objectFlag;
	char					*objectSecurity;
} ScanBinderyObject;

typedef struct Binderypb {
	char					fill12[12];
#ifdef REGISTER_A0_COMPATIBILITY
	ProcPtr					ioCompletion;
#else
	IOCompletionUPP			ioCompletion;
#endif
	sint16					ioResult;
	char					*ioNamePtr;
	sint16					ioVRefNum;
	sint16					ioCRefNum;
	sint16					csCode;
	sint16					NCPCompletionCode;
	union {
		CreateConnection		CreateConn;
		ReadPropertyValue		ReadPropValue;
		ScanBinderyObject		ScanBinderyObject;
	} csParams;
} Binderypb;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

/*
 * Bindery csCodes
 */
#define BINDERY_ATTACH					400
#define BINDERY_READ_PROP_VALUE			401
#define BINDERY_SCAN_BINDERY_OBJECT		402
#define BINDERY_DETACH					403

#endif /* __BINDERYPB_INCLUDED__ */
