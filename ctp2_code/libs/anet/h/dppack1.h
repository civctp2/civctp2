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
 Whatever pragma is needed on this compiler to turn on full structure
 packing.  Use this to avoid padding in structures that will be sent on
 the network.
 Use dpunpack.h to revert to previous packing state.

 $Log: dppack1.h $
 Revision 1.4  1997/09/21 04:34:25  lbennett
 Removed MSVC compiler warning.
 Revision 1.3  1997/06/10 23:57:16  dkegel
 Define PACK if not yet defined; used to be done in anet.h.
 Revision 1.2  1997/06/01 00:00:28  dkegel
 Fixed typo in Mac pragma
 Revision 1.1  1997/05/31 22:40:54  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#if defined(__WATCOMC__)    /* Watcom C */

#pragma pack(1)

#elif defined(__MWERKS__)   /* Codewarrior */

#pragma options align=packed

#elif defined(_M_IX86)      /* Visual C */

#pragma warning( disable : 4103 )
#pragma pack(push, 1)

#endif

/* Structure members that need to be byte-alligned have PACK after them
 * for benefit of gcc.  Makefile will define PACK in that case.
 */
#ifndef PACK
#define PACK
#endif
