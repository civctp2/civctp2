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
 * lap_error.h
 *
 * The lap errors defined in this file are in addition to the OSErr defined by
 * Mac OS.  This file does not necessarily contain all the error codes required
 * by any alternate lap.  Lap developers are encouraged to add any error code(s)
 * that might be required.
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */


#ifndef __LAP_ERROR_INCLUDED__
#define __LAP_ERROR_INCLUDED__

#include "types.h"

#define LAP_NO_ERROR					(sint16)0x0000

#define LAP_GENERAL_FAILURE				(sint16)0xC000

#define LAP_NOT_INSTALLED				(sint16)0xC001
#define LAP_OUT_OF_RESOURCE				(sint16)0xC002
#define LAP_BAD_DRVR_DATA				(sint16)0xC003
#define LAP_CORRUPTED_DATA				(sint16)0xC004
#define LAP_UNSUPPORTED_HARDWARE		(sint16)0xC005
#define LAP_LINK_UNAVAILABLE			(sint16)0xC006
#define LAP_USING_ATALK_PHASE1			(sint16)0xC007

#define LAP_SELECTOR_NOT_SUPPORTED		(sint16)0xC008
#define LAP_NO_ATALK_GATEWAY			(sint16)0xC009
#define	LAP_WRONG_MACIPX_VERSION		(sint16)0xC00A

#endif /* __LAP_ERROR_INCLUDED__ */
