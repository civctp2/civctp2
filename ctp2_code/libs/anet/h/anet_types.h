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

/*-----------------------------------------------------------------------
 Type Library

A collection of useful names for basic types.

Copyright (c) 1997 by Activision Inc.  All rights reserved.
Copyright protection claimed includes all forms and matters of
copyrightable material and information now allowed by statutory or
judicial law or hereinafter granted, including without limitation,
material generated from the software programs which are displayed
on the screen such as icons, screen display looks, etc.

$Log: types.h $
Revision 1.1  1997/08/26 01:41:27  lbennett
Initial revision
-----------------------------------------------------------------------*/

/* Protect against double-inclusion */
#if !defined(_ANET_TYPES_H_)
#define _ANET_TYPES_H_ 1

#ifdef HAVE_STDINT_H

#include <stdint.h>

typedef char           dp_char_t;
typedef unsigned char  dp_uchar_t;
typedef int8_t         dp_int8_t;
typedef uint8_t        dp_uint8_t;
typedef int16_t        dp_int16_t;
typedef uint16_t       dp_uint16_t;
typedef int32_t        dp_int32_t;
typedef uint32_t       dp_uint32_t;

#elif defined(__x86_64) || defined(__amd64) || defined(__LP64__)

typedef char           dp_char_t;
typedef unsigned char  dp_uchar_t;
typedef char           dp_int8_t;
typedef unsigned char  dp_uint8_t;
typedef short          dp_int16_t;
typedef unsigned short dp_uint16_t;
typedef int            dp_int32_t;
typedef unsigned int   dp_uint32_t;

#else

typedef char           dp_char_t;
typedef unsigned char  dp_uchar_t;
typedef char           dp_int8_t;
typedef unsigned char  dp_uint8_t;
typedef short          dp_int16_t;
typedef unsigned short dp_uint16_t;
typedef long           dp_int32_t;
typedef unsigned long  dp_uint32_t;

#endif

#endif /* Protect against double-inclusion */

