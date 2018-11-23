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
File to hold various techniques to ensure that the code is portable
across various machines, including big-endian/little-endian conflicts.

$Log: portable.h $
Revision 1.1  1997/10/08 01:20:56  lbennett
Initial revision
-----------------------------------------------------------------------*/

#if !defined(_DP_PORTABLE_H)
#define _DP_PORTABLE_H 1

/* Construct portable multibyte representation of short integers */
#define dpGETSHORT_FIRSTBYTE(s) (unsigned char)((s) & 0xff)
#define dpGETSHORT_SECONDBYTE(s) (unsigned char)(((s) >> 8) & 0xff)

/* Construct portable multibyte representation of long integers */
#define dpGETLONG_FIRSTBYTE(s)  (unsigned char)((s) & 0xff)
#define dpGETLONG_SECONDBYTE(s) (unsigned char)(((s) >> 8) & 0xff)
#define dpGETLONG_THIRDBYTE(s)  (unsigned char)(((s) >> 16) & 0xff)
#define dpGETLONG_FOURTHBYTE(s) (unsigned char)(((s) >> 24) & 0xff)

/* Retrieve short integers from portable multibyte representation */
#define dpMAKESHORT(first,second) ((unsigned char)(first) |\
								 (((unsigned short)((unsigned char)(second)))<<8))

/* Retrieve long integers from portable multibyte representation */
#define dpMAKELONG(first,second,third,fourth) (   \
		(unsigned long)dpMAKESHORT(first,second) |\
		((unsigned long)dpMAKESHORT(third, fourth)) << 16)

#endif /* _DP_PORTABLE_H */
