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
 * ipxerror.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */

#ifndef __IPXERROR_INCLUDED__
#define __IPXERROR_INCLUDED__

#define IPX_SUCCESSFUL					(short)0x0000

#define IPX_NOT_INSTALLED				(short)0x90F0
#define IPX_OUT_OF_RESOURCE				(short)0x90F7
#define IPX_EVENT_CANNOT_BE_SCHEDULED	(short)0x90F8
#define IPX_EVENT_CANNOT_BE_CANCELLED	(short)0x90F9
#define IPX_NO_ROUTE					(short)0x90FA
#define IPX_EVENT_CANCELLED				(short)0x90FC
#define IPX_BAD_PACKET					(short)0x90FD
#define IPX_SOCKET_TABLE_FULL			(short)0x90FE
#define IPX_SOCKET_IN_USE				(short)0x90FF

#define IPX_BAD_CHECKSUM				(short)0x91D1
#define IPX_LAP_FAILS					(short)0x91D2
#define IPX_PACKET_OVERFLOW				(short)0x91FD
#define IPX_BAD_PARAMETER				(short)0x91FE
#define IPX_SOCKET_NOT_OPEN				(short)0x91FF

#define IPX_ECB_IN_USE					(short)0x92FF

#endif /* __IPXERROR_INCLUDED__ */
