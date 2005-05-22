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
 DDPrint.h
 
 Debug printing for ActiveNet Windows drivers, where it's not clear how to
 give them access to the full services in dprint.h.  For now, only log to
 the Windows debug system.
 Later, if we figure it out, we can have this include the real dprint.h.

 Define the symbol
   DEBUG_MODULE
 to TRUE or FALSE before including dprint.h.  For instance,
   #define DEBUG_MODULE 1
    include "ddprint.h"

 Then, to print a debugging message, use
   DPRINT(("message %s", str));
 and compile the driver for debugging (which turns on _DEBUG).

--------------------------------------------------------------------------*/

#ifndef _DDPrint_h_
#define _DDPrint_h_

#include <windows.h>

/*------------------------------------------------------------------------
 Callback used to list servers and the round trip delay to them.
------------------------------------------------------------------------*/
typedef int (FAR PASCAL *dp_dprintf_t) (
	const char *    __format,   /* printf-style format (or NULL) */
	...);                       /* printf-style arguments on stack (if any) */

dp_dprintf_t dp_dprintf;

/*-------------------------------------------------------------------------
 Call this to specify a dp_dprintf compatible function to call instead of 
 the default dp_dprintf.
-------------------------------------------------------------------------*/
#define dp_dprintf_set(fn) { dp_dprintf = fn; }

/*--------------------------------------------------------------------------
 Print a debug string to the debugging console. 
 Returns the number of characters sent.
 Don't use dp_dprintf directly; use the DPRINT macro instead for lower overhead.
--------------------------------------------------------------------------*/
int ddprintf(
	const char *    __format,   /* printf-style format (or NULL) */
	...);                       /* printf-style arguments on stack (if any) */

/*--------------------------------------------------------------------------
 Print a debug string to the debugging console, or, if dp_dprintf_set has been
 called, printed to the function specified by dp_dprintf_set.
 Don't use dp_dprintf directly; use the DPRINT macro instead for lower overhead.
--------------------------------------------------------------------------*/
#if defined(_DEBUG) && defined(DEBUG_MODULE) && DEBUG_MODULE
    #define DPRINT(p)  ((dp_dprintf ? dp_dprintf: ddprintf) p)
#else
	#define DPRINT(p)  ;
#endif

#endif // _DDPrint_h_
