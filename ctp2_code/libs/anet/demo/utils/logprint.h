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
 Minimal module for logging to file.
 To use, include this file and call logprint_setFile() once before any
 calls to DPRINT().
 Similar to dp_setLogFname() and DPRINT() in anet.h.
--------------------------------------------------------------------------*/

#ifndef logprint_H
#define logprint_H

#include <stdio.h>

#if defined(logprint_ENABLED)
#define DPRINT(a) logprint_printf a
#else
#define DPRINT(a)
#endif

/*--------------------------------------------------------------------------
 Set the output log file.  If file is NULL, send output to stdout.
 Returns 0 on success,
        -1 on failure.
--------------------------------------------------------------------------*/
#if defined(logprint_ENABLED)
int logprint_setFile(const char *file);
#else
#define logprint_setFile(f) (-1)
#endif

/*--------------------------------------------------------------------------
 Print to log file set by logprint_setFile or stdout if logprint_setFile
 has not yet been called.
--------------------------------------------------------------------------*/
#if defined(logprint_ENABLED)
void cdecl logprint_printf(
	const char *	__format,	/* printf-style format (or NULL) */
	...);						/* printf-style arguments on stack (if any) */
#endif

#endif
