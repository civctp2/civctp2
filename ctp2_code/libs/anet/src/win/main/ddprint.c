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
 Debug printing for ActiveNet Windows drivers.
 For now, only log to the Windows debug system.
 See ddprint.h for more info.
--------------------------------------------------------------------------*/

#if defined(_DEBUG) && defined(DEBUG_MODULE) && DEBUG_MODULE

#include <windows.h>
#include <stdarg.h>
#include <stdio.h>

int cdecl ddprintf(const char *	__format,  ...)
{
    int     len;
    char    buf[512];
  	va_list	argptr;

    va_start(argptr, __format);

  	len = vsprintf(buf, __format, argptr);

  	va_end(argptr);

    OutputDebugString(buf);

  	return(len);
}
#else
  void ddprintf(const char *	__format,  ...) {(void)__format;}
#endif
