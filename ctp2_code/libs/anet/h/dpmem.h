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
 Memory allocation functions used by DP.

 Include in any dp*.c file that wants to call malloc, e.g.
   assoctab.c dp.c dpio.c dppv.c dynatab.c q.c

 This lets us override the allocator in different environments if
 malloc won't do.

 $Log: dpmem.h $
 Revision 1.4  1997/07/10 19:13:15  dkegel
 Use predefined _WIN32 symbol rather than special WIN32 symbol.
 Revision 1.3  1997/05/29 19:42:12  dkegel
 Switch to stdlib.h to get malloc(); more portable.
 Revision 1.2  1997/03/09 03:04:19  dkegel
 Tell Windows it can move the block when reallocating if it needs to.
 Revision 1.1  1997/03/07 19:35:29  dkegel
 Initial revision
--------------------------------------------------------------------------*/

#ifndef dpmem_h
#define dpmem_h

#if defined(_WIN32)
//if defined(_WIN32) && defined(DP_DLL)
#include <windows.h>
/* Don't want memory to go away when game setup shell frees the heap! */
#define dp_MALLOC(s)     GlobalAlloc(GMEM_FIXED, s)
#define dp_REALLOC(p, s) GlobalReAlloc(p, s, GMEM_MOVEABLE)
#define dp_FREE(p)       GlobalFree(p)
#else
#include <stdlib.h>
#define dp_MALLOC(s)     malloc(s)
#define dp_REALLOC(p, s) realloc(p, s)
#define dp_FREE(p)       free(p)
#endif

#endif
