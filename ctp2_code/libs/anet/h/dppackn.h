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

/*--------------------------------------------------------------------------
 Whatever pragma is needed on this compiler to force default structure
 packing.  Use this to protect a .h file from any packing pragmas that
 might be in force on entry.
 Use dpunpack.h to revert to previous packing state.

 $Log: dppackn.h $
 Revision 1.3  1997/06/10 23:58:01  dkegel
 Define PACK if not yet defined; used to be done in anet.h.
 Revision 1.2  1997/06/01 00:00:31  dkegel
 Fixed typo in Mac pragma
 Revision 1.1  1997/05/31 22:40:56  dkegel
 Initial revision
--------------------------------------------------------------------------*/
#if defined(__WATCOMC__)    /* Watcom C */

#pragma pack()

#elif defined(__MWERKS__)   /* Codewarrior */

#pragma options align=native

#elif defined(_M_IX86)      /* Visual C */

#pragma pack(push)

#endif

/* Structure members that need to be byte-alligned have PACK after them
 * for benefit of gcc.  Makefile will define PACK in that case.
 */
#ifndef PACK
#define PACK
#endif
