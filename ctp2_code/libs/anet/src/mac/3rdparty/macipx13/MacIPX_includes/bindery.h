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
#define ESUCCESS						(short)0x0000
#define ERR_SERVER_OUT_OF_MEMORY		(short)0x0096
#define ERR_NO_SUCH_SEGMENT				(short)0x00EC
#define ERR_INVALID_NAME				(short)0x00EF
#define ERR_WILDCARD_NOT_ALLOWED		(short)0x00F0
#define ERR_INVALID_BINDERY_SECURITY	(short)0x00F1
#define ERR_NO_PROPERTY_READ_PRIVILEGE	(short)0x00F9
#define ERR_NO_SUCH_PROPERTY			(short)0x00FB
#define ERR_NO_SUCH_OBJECT				(short)0x00FC
#define ERR_SERVER_BINDERY_LOCKED		(short)0x00FE
#define ERR_BINDERY_FAILURE				(short)0x00FF
#define ERR_INVALID_ARG					(short)0xFFFF

/* Bindery object type definitions */
#define OT_WILD                        (unsigned short)0xFFFF  /* Matches any type            */
#define OT_UNKNOWN                     (unsigned short)0x0000  /* Unknown object type         */
#define OT_USER                        (unsigned short)0x0001  /* The object is a "user"      */
#define OT_USER_GROUP                  (unsigned short)0x0002  /* A group of users            */
#define OT_GROUP                       (unsigned short)0x0002
#define OT_PRINT_QUEUE                 (unsigned short)0x0003  /* Services print queues       */
#define OT_FILE_SERVER                 (unsigned short)0x0004  /* The object serves files     */
#define OT_JOB_SERVER                  (unsigned short)0x0005
#define OT_GATEWAY                     (unsigned short)0x0006
#define OT_PRINT_SERVER                (unsigned short)0x0007
#define OT_ARCHIVE_QUEUE               (unsigned short)0x0008
#define OT_ARCHIVE_SERVER              (unsigned short)0x0009  /* Services backup jobs        */
#define OT_JOB_QUEUE                   (unsigned short)0x000A
#define OT_ADMINISTRATION              (unsigned short)0x000B
#define OT_NAS_SNA_GATEWAY             (unsigned short)0x0021
#define OT_REMOTE_BRIDGE_SERVER        (unsigned short)0x0024
#define OT_TCPIP_GATEWAY               (unsigned short)0x0027
#define OT_TIME_SYNCHRONIZATION_SERVER (unsigned short)0x002D
#define OT_ARCHIVE_SERVER_DYNAMIC_SAP  (unsigned short)0x002E
#define OT_ADVERTISING_PRINT_SERVER    (unsigned short)0x0047
#define OT_BTRIEVE_VAP                 (unsigned short)0x004B
#define OT_NW$SQL_VAP                  (unsigned short)0x004C
#define OT_PRINT_QUEUE_USER            (unsigned short)0x0053

/* Maximum lengths of object, properties, (includes terminating null) */
#define BL_OBJECT              			48
#define BL_PROPERTY            			16
#define BL_PASSWORD            			128

/*
 * Prototypes
 */
extern pascal short	MacIPXCreateConnectionWithServer(
											unsigned char	*serverAddress);

extern pascal short	MacIPXDestroyConnectionWithServer(void);

extern pascal short	MacIPXReadPropertyValue(
											char 			*objectName,
											unsigned short 	objectType,
											char 			*propertyName,
											unsigned char 	segmentNumber,
											unsigned char 	*propertyValue,
											unsigned char 	*moreSegments,
											unsigned char 	*propertyFlags);

extern pascal short	MacIPXScanBinderyObject(
											char 			*searchObjectName,
											unsigned short 	searchObjectType,
											long 			*objectID,
											char 			*objectName,
											unsigned short 	*objectType,
											char 			*objectHasProperties,
											char 			*objectFlag,
											char 			*objectSecurity);
												
#endif  /* __BINDERY_H_INCLUDED__ */
