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
 * lap_if.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */
 
#ifndef __LAP_IF__
#define __LAP_IF__

#ifndef __FILES__
#include <Files.h>
#endif

#ifndef __WDS_INCLUDED__
#include <wds.h>
#endif


/* 
 * Well known codes for lap_main call 
 */
 
#define		DO_INIT						0L
#define 	DO_RESTORE_STATE			1L

/* Control Information */
#define 	GET_ADDR					100L
#define 	GET_NAME					102L
#define 	GET_MTU						103L
#define 	GET_FILE_ICON				104L
#define		GET_HW_ADDR					105L

/* To acquire lap functions */
#define 	GET_DIALOG					201L
#define 	GET_SLOT					202L
#define		GET_SEND					203L
#define 	GET_CLOSE					204L
#define 	GET_SYSTEM					205L
#define		GET_ADD_SOCKET				206L
#define		GET_REMOVE_SOCKET			207L

/* To attach IPX handler */
#define 	PUT_HANDLER					300L

/* Auto config selectors */
#define 	PUT_AUTOCONFIG_HANDLER		400L
#define 	PUT_AUTOCONFIG_DATA			401L

/* Multicast addr selector */
#define		PUT_MCAST_ADDR				500L
#define		REMOVE_MCAST_ADDR			501L

/* Miscellaneous */
#define		CAN_CHECKSUM_PKTS			600L
#define		IS_APPLETALK_CLIENT			601L
#define		GET_SAP_PERIODIC_TICKS		602L
#define		GET_RIP_PERIODIC_TICKS		603L
#define		PUT_MACIPX_VERSION			604L

#ifndef _TYPES_
/* 
 * some unsigned types 
 */
typedef  unsigned char 	u_char;
typedef  unsigned int  	u_int;
typedef  unsigned long 	u_long;
typedef  unsigned short u_short;
#endif


/* 
 * pointer to function types 
 */
typedef  OSErr 		(*CloseProcPtr) 	(void);
typedef  void  		(*SendCompProcPtr) 	(ParmBlkPtr paramBlock, 
                                         WDS *wds_p, 
										 long status);
typedef  OSErr 		(*SendProcPtr) 		(ParmBlkPtr paramBlock, 
                                         WDS *wds_p, 
										 char *dest_addr, 
										 SendCompProcPtr send_completion);
typedef  void  		(*PHProcPtr)		(void);
typedef  OSErr 		(*LapMainProcPtr) 	(long code, 
										 void *ptr, 
										 u_long *len);
typedef  Boolean  	(*DialogProcPtr) 	(long slot_num);
typedef  OSErr  	(*SlotProcPtr) 		(long *slot_num);
typedef	 OSErr		(*SocketProcPtr)	(char *socket_addr);			
typedef  void		(*SysTimeProcPtr)	(void);


/* 
 * IPX protocol type for Ethernet, Token Ring, and what else???
 */
#ifndef IPX_PROTOCOL_TYPE 
#define	IPX_PROTOCOL_TYPE		(unsigned short)0x8137
#endif

/*
 * Novell SAP for 802.2 headers
 */
#ifndef NOVELL_SAP
#define NOVELL_SAP				(unsigned char)0xE0
#endif

#endif /* __LAP_IF__ */
