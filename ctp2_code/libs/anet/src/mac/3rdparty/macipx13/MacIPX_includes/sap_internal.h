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
 * sap_internal.h
 *
 * Copyright (C) 1992-1994 Novell, Inc.
 * All rights reserved.
 */

#ifndef __SAP_INTERNAL__
#define __SAP_INTERNAL__

#ifndef THINK_C
#ifndef __MEMORY__
#include <Memory.h>
#endif
#endif

#ifndef __SAP_H_INCLUDED__
#include "sap.h"
#endif

#ifndef __SAPPB_INCLUDED__
#include "sappb.h"
#endif

#define ADVERTISING_SIGNATURE	'SAPS'

#if defined(powerc) || defined (__powerc)
#pragma options align=mac68k
#endif
typedef struct AdvertisingStruct {
	struct AdvertisingStruct	*qLink;
	short						qType;	/* Not used */
	unsigned long				signature;
	T_SAP_ID_PACKET				ASServerIDpacket;
} T_AdvertisingStruct;
#if defined(powerc) || defined(__powerc)
#pragma options align=reset
#endif

#endif /* __SAP_INTERNAL__ */
