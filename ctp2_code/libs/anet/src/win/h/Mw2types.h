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

/* ***********************************************************************
 *                               RCS HEADER
 * $Locker: $
 * $Log: Mw2types.h $
 * Revision 1.1  1996/12/13 23:25:04  jgraebner
 * Initial revision
 * Revision 1.2  1995/10/27 11:27:47  dzobel
 * Added magic numbers and orderof().
 * Revision 1.1  1995/09/14 16:05:08  dzobel
 * Initial revision
 *
 * *********************************************************************** */

/*
 *	MW2Types.h
 *
 *	MechWarrior 2 standard datatypes.
 */

#ifndef _WINDOWS
  #pragma once			// Avoid multiple inclusions
#endif

#ifndef _MW2Types_h_
#define _MW2Types_h_

#pragma	pack(1)			// Don't pad or align structures

/* ======================================================================== *
								Type Definitions
 * ======================================================================== */
// In order to get the numeric types situation under control, all MW2 modules
// will use the typedefs defined herein.  This is the ONLY module that should
// declare any numeric types.  These types are consistent with those defined
// in the windef.h module int the Bcc C Windows library.
#define TRUE  1
#define FALSE 0

typedef char            CHAR;  // Note: Under WATCOM char is unsigned.
typedef signed char    SCHAR;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;
typedef	unsigned char	u8;
typedef	signed   char	s8;

typedef short          SHORT;
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef	unsigned short	u16;
typedef	signed   short	s16;

typedef           int  INT;
typedef unsigned int   UINT;
typedef           int  BOOL;

typedef          long  LONG;
typedef unsigned long  DWORD;
typedef unsigned long  ULONG;
typedef	unsigned long	u32;
typedef	signed   long	s32;

typedef          float FLOAT;
#ifndef _WINDOWS
//  typedef long           FIXED; ///////////////////////////////////////////////////////////
#endif
typedef signed   long  FIXED16;   // 16:16 fixed-point type [-32K,+32K]
typedef signed   long  FIXED29;   // 3:29 used for trig functions.
typedef signed   long  FIXED30;   // 2:30 fixed-point type [-1.999,+1.999]

						// A four-character magic number
typedef	unsigned long	magic_t;
#define magicNum(a,b,c,d) \
		(				(unsigned char) (a) \
			| (			(unsigned char) (b) \
			| (			(unsigned char) (c) \
			| (magic_t) (unsigned char) (d) \
				<<8)<<8)<<8)

						// Number of items in an array (analogous to sizeof)
#define orderof(a)		(sizeof(a) / sizeof(*(a)))

#endif // _MW2Types_h_
