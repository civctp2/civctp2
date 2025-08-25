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
Revision 1.2  2025/07/05 15:25:35  Martin Gühmann
Add the typedefs from CTP2, so that we have predictable
data length across different compilers
Revision 1.1  1997/08/26 01:41:27  lbennett
Initial revision
-----------------------------------------------------------------------*/

/* Protect against double-inclusion */
#if !defined(_TYPES_H_)
#define _TYPES_H_

//typedef unsigned char u_char;
//typedef unsigned short int u_short;
//typedef unsigned int u_int;
//typedef unsigned long int u_long;

#if defined(_MSC_VER)

typedef          char        schar;
typedef unsigned char        uchar;

typedef          char        sint8;
typedef unsigned char        uint8;
typedef          short int   sint16;
typedef unsigned short int   uint16;
typedef          int         sint32;
typedef unsigned int         uint32;
typedef          __int64     sint64;
typedef unsigned __int64     uint64;

#else

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#endif

#include <stdint.h>


typedef signed char          schar;
typedef unsigned char        uchar;

typedef int8_t               sint8;
typedef uint8_t              uint8;
typedef int16_t              sint16;
typedef uint16_t             uint16;
typedef int32_t              sint32;
typedef uint32_t             uint32;
typedef int64_t              sint64;
typedef uint64_t             uint64;

#endif // !defined(_MSC_VER)

#endif /* Protect against double-inclusion */
