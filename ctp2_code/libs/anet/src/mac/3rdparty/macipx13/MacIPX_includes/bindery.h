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
 * bindery.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 *
 * Bindery support routines specifically for MacIPX.
 */

#ifndef __BINDERY_H_INCLUDED__
#define __BINDERY_H_INCLUDED__

/* Error codes */
#define ESUCCESS						(sint16)0x0000
#define ERR_SERVER_OUT_OF_MEMORY		(sint16)0x0096
#define ERR_NO_SUCH_SEGMENT				(sint16)0x00EC
#define ERR_INVALID_NAME				(sint16)0x00EF
#define ERR_WILDCARD_NOT_ALLOWED		(sint16)0x00F0
#define ERR_INVALID_BINDERY_SECURITY	(sint16)0x00F1
#define ERR_NO_PROPERTY_READ_PRIVILEGE	(sint16)0x00F9
#define ERR_NO_SUCH_PROPERTY			(sint16)0x00FB
#define ERR_NO_SUCH_OBJECT				(sint16)0x00FC
#define ERR_SERVER_BINDERY_LOCKED		(sint16)0x00FE
#define ERR_BINDERY_FAILURE				(sint16)0x00FF
#define ERR_INVALID_ARG					(sint16)0xFFFF

/* Bindery object type definitions */
#define OT_WILD                        (uint16)0xFFFF  /* Matches any type            */
#define OT_UNKNOWN                     (uint16)0x0000  /* Unknown object type         */
#define OT_USER                        (uint16)0x0001  /* The object is a "user"      */
#define OT_USER_GROUP                  (uint16)0x0002  /* A group of users            */
#define OT_GROUP                       (uint16)0x0002
#define OT_PRINT_QUEUE                 (uint16)0x0003  /* Services print queues       */
#define OT_FILE_SERVER                 (uint16)0x0004  /* The object serves files     */
#define OT_JOB_SERVER                  (uint16)0x0005
#define OT_GATEWAY                     (uint16)0x0006
#define OT_PRINT_SERVER                (uint16)0x0007
#define OT_ARCHIVE_QUEUE               (uint16)0x0008
#define OT_ARCHIVE_SERVER              (uint16)0x0009  /* Services backup jobs        */
#define OT_JOB_QUEUE                   (uint16)0x000A
#define OT_ADMINISTRATION              (uint16)0x000B
#define OT_NAS_SNA_GATEWAY             (uint16)0x0021
#define OT_REMOTE_BRIDGE_SERVER        (uint16)0x0024
#define OT_TCPIP_GATEWAY               (uint16)0x0027
#define OT_TIME_SYNCHRONIZATION_SERVER (uint16)0x002D
#define OT_ARCHIVE_SERVER_DYNAMIC_SAP  (uint16)0x002E
#define OT_ADVERTISING_PRINT_SERVER    (uint16)0x0047
#define OT_BTRIEVE_VAP                 (uint16)0x004B
#define OT_NW$SQL_VAP                  (uint16)0x004C
#define OT_PRINT_QUEUE_USER            (uint16)0x0053

/* Maximum lengths of object, properties, (includes terminating null) */
#define BL_OBJECT              			48
#define BL_PROPERTY            			16
#define BL_PASSWORD            			128

/*
 * Prototypes
 */
extern pascal sint16	MacIPXCreateConnectionWithServer(
											uint8	*serverAddress);

extern pascal sint16	MacIPXDestroyConnectionWithServer(void);

extern pascal sint16	MacIPXReadPropertyValue(
											char 			*objectName,
											uint16 	objectType,
											char 			*propertyName,
											uint8 	segmentNumber,
											uint8 	*propertyValue,
											uint8 	*moreSegments,
											uint8 	*propertyFlags);

extern pascal sint16	MacIPXScanBinderyObject(
											char 			*searchObjectName,
											uint16 	searchObjectType,
											sint32 			*objectID,
											char 			*objectName,
											uint16 	*objectType,
											char 			*objectHasProperties,
											char 			*objectFlag,
											char 			*objectSecurity);

#endif  /* __BINDERY_H_INCLUDED__ */
