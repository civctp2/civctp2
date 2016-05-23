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

/**
* Copyright (c) 1997, 1998 Activision Inc.  All rights reserved.
*/

#include "eclock.h"

/****************************** Solaris 2.x **************************/
#ifdef SPARC

/**
* returns elapsed time in CLK_TCK units.
*/
clock_t eclock_sparc(void)
{
	static struct tms time_buffer;

	return times(&time_buffer);
}
#endif

/****************************** Macintosh *****************************/
#if defined(__MWERKS__)

/**
* Return the number of milliseconds elapsed since system startup
*/
clock_t eclock(void)
{
    static UnsignedWide t;
    Microseconds(&t);
    return ((clock_t)(t.lo/1000));
}
#endif


/**************************** Windows NT/95 ***************************/
#if defined(_WIN32)

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if (defined _WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <windows.h>

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if (defined _WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/* Return number of milliseconds since midnight GMT */
clock_t eclock_win32(void)
{
	static clock_t clk_offset = 0;

	if (!clk_offset) {
		SYSTEMTIME time_buffer;
		GetSystemTime(&time_buffer);
		clk_offset = GetTickCount() -
			(time_buffer.wMilliseconds +
			 1000L * ( time_buffer.wSecond +
			          60L * ( time_buffer.wMinute +
			                  60L * time_buffer.wHour)));
	}
	return GetTickCount() - clk_offset;
}

#endif
