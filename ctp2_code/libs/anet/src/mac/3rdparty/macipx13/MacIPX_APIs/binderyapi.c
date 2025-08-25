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
 * binderyapi.c
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */

/*
 * Include files
 */
#include <Devices.h>
#include "bindery.h"
#include "binderypb.h"

/*
 * Defines
 */
#ifndef TRUE
#define TRUE					1
#endif
#ifndef FALSE
#define	FALSE					0
#endif
#define strlen					bindery_strlen

#ifdef strlen
//typedef uint32			size_t; // This should be defined
#endif /* strlen */

/*
 * Prototypes
 */
sint16		call_bindery(Binderypb *pb);
#ifdef strlen
size_t		bindery_strlen(char *s);
#else
#include <String.h>
#endif /* strlen */

pascal sint16
MacIPXCreateConnectionWithServer(uint8 *serverAddress)
{
	Binderypb	bindery_pb;

	bindery_pb.csCode = BINDERY_ATTACH;
	bindery_pb.csParams.CreateConn.serverAddress = serverAddress;
	return (call_bindery(&bindery_pb));
}

pascal sint16
MacIPXDestroyConnectionWithServer()
{
	Binderypb	bindery_pb;

	bindery_pb.csCode = BINDERY_DETACH;
	return (call_bindery(&bindery_pb));
}

pascal sint16
MacIPXReadPropertyValue(char *objectName, uint16 objectType, char *propertyName,
					uint8 segmentNumber, uint8 *propertyValue,
					uint8 *moreSegments, uint8 *propertyFlags)
{
	Binderypb	bindery_pb;

	if ((strlen(objectName) > 47) || (strlen(propertyName) > 15)) {
		return (ERR_INVALID_NAME);
	}
	if (propertyValue == (uint8 *)0) {
		return (ERR_INVALID_ARG);
	}
	bindery_pb.csCode = BINDERY_READ_PROP_VALUE;
	bindery_pb.csParams.ReadPropValue.objectName = objectName;
	bindery_pb.csParams.ReadPropValue.objectType = objectType;
	bindery_pb.csParams.ReadPropValue.propertyName = propertyName;
	bindery_pb.csParams.ReadPropValue.segmentNumber = segmentNumber;
	bindery_pb.csParams.ReadPropValue.propertyValue = propertyValue;
	bindery_pb.csParams.ReadPropValue.moreSegments = moreSegments;
	bindery_pb.csParams.ReadPropValue.propertyFlags = propertyFlags;
	return (call_bindery(&bindery_pb));
}

pascal sint16
MacIPXScanBinderyObject(char *searchObjectName, uint16 searchObjectType, sint32 *objectID,
					char *objectName, uint16 *objectType, char *objectHasProperties,
					char *objectFlag, char *objectSecurity)
{
	Binderypb	bindery_pb;

	if (strlen(searchObjectName) > 47) {
		return (ERR_INVALID_NAME);
	}
	if (objectID == (sint32 *)0) {
		return (ERR_INVALID_ARG);
	}
	bindery_pb.csCode = BINDERY_SCAN_BINDERY_OBJECT;
	bindery_pb.csParams.ScanBinderyObject.searchObjectName = searchObjectName;
	bindery_pb.csParams.ScanBinderyObject.searchObjectType = searchObjectType;
	bindery_pb.csParams.ScanBinderyObject.objectID = objectID;
	bindery_pb.csParams.ScanBinderyObject.objectName = objectName;
	bindery_pb.csParams.ScanBinderyObject.objectType = objectType;
	bindery_pb.csParams.ScanBinderyObject.objectHasProperties = objectHasProperties;
	bindery_pb.csParams.ScanBinderyObject.objectFlag = objectFlag;
	bindery_pb.csParams.ScanBinderyObject.objectSecurity = objectSecurity;
	return (call_bindery(&bindery_pb));
}

sint16
call_bindery(Binderypb *pb)
{
	sint16			status;
	Binderypb		bindery_pb;
	char			binderyDrvrName[11];

	binderyDrvrName[0] = 0x0A;
	binderyDrvrName[1] = '.';
	*(sint32 *)&binderyDrvrName[2] = 'NVL_';
	*(sint32 *)&binderyDrvrName[6] = 'BNDR';
	binderyDrvrName[10] = 'Y';
	bindery_pb.ioNamePtr = binderyDrvrName;
	bindery_pb.csCode = 0; /* We're actually setting the read/write permission here */
	status = PBOpen((ParmBlkPtr)&bindery_pb, FALSE);
	if (status) {
		goto Exit0;
	}

	pb->ioCRefNum = bindery_pb.ioCRefNum;
	(void)PBControl((ParmBlkPtr)pb, FALSE);
	if (pb->ioResult) {
		status = pb->ioResult;
	} else {
		status = pb->NCPCompletionCode & 0x00FF;
	}

Exit0:
	return (status);
}

#ifdef strlen
size_t
bindery_strlen(char *s)
{
	size_t	string_len;
	char	*string;

	string_len = 0;
	string = s;
	while (*string) {
		++string_len;
		++string;
	}
	return (string_len);
}
#endif /* strlen */
