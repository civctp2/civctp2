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
 * spxerror.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */

#ifndef __SPXERROR_INCLUDED__
#define __SPXERROR_INCLUDED__

#include "types.h"

#define SPX_SUCCESSFUL				(sint16)0x3000

#define SPX_OUT_OF_RESOURCE			(sint16)0xA002
#define SPX_ECB_IN_USE				(sint16)0xA0FF

#define SPX_CANCEL_FAILED			(sint16)0xA1F9
#define SPX_BAD_PARAMETER			(sint16)0xA1FD
#define SPX_ECB_NOT_FOUND			(sint16)0xA1FF

#define SPX_CONNECT_FAILED			(sint16)0xA2ED
#define SPX_CONNECT_NOT_FOUND		(sint16)0xA2EE
#define SPX_CONNECT_NO_ROUTE		(sint16)0xA2FA
#define SPX_ECB_CANCELLED			(sint16)0xA2FC
#define SPX_BAD_PACKET				(sint16)0xA2FD
#define SPX_SOCKET_NOT_FOUND		(sint16)0xA2FF

#define SPX_CONNECT_TERMINATED		(sint16)0xA3EC
#define SPX_CONNECT_ABORTED			(sint16)0xA3ED
#define SPX_CONNECT_TABLE_FULL		(sint16)0xA3EF
#define SPX_PACKET_OVERFLOW			(sint16)0xA3FD
#define SPX_SOCKET_TABLE_FULL		(sint16)0xA3FE

#define SPX_BAD_CHECKSUM			(sint16)0xAAFF

#endif /* __SPXERROR_INCLUDED__ */
