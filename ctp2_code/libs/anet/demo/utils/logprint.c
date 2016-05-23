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

#if defined(logprint_ENABLED)

#include <stdio.h>

static FILE *logprint_fp = NULL;

/*--------------------------------------------------------------------------
 Set the output log file.  If file is NULL, send output to stdout.
 Returns 0 on success,
        -1 on failure.
--------------------------------------------------------------------------*/
int logprint_setFile(const char *file)
{
	if (logprint_fp != NULL) {
		fclose(logprint_fp);
		logprint_fp = NULL;
	}
	if (!file)
		return 0;
	if ((logprint_fp = fopen(file, "w")) == NULL)
		return -1;
	return 0;
}

/*--------------------------------------------------------------------------
 Print to log file set by logprint_setFile or stdout if logprint_setFile
 has not yet been called.
--------------------------------------------------------------------------*/
void cdecl logprint_printf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
{
#include <stdarg.h>
	va_list argptr = NULL;

	if (__format == NULL)
		return;
	va_start(argptr, __format);
	if (logprint_fp == NULL) {
		vprintf(__format, argptr);
	} else {
		vfprintf(logprint_fp, __format, argptr);
		fflush(logprint_fp);
	}
	va_end(argptr);

	return;
}

#endif
