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

/*-------------------------------------------------------------------------
 Win32 implementations of functions normally in <unistd.h>
 alarm()
 sleep()

 $Log: unistd.c $
 Revision 1.4  1997/09/22 02:35:04  lbennett
 Removed MSVC L4 compiler warnings.
 Revision 1.3  1997/08/14 23:49:47  dkegel
 No compiler warnings...
 Revision 1.2  1997/08/14 22:09:55  dkegel
 Correctly implement 'cancel previous timer' behavior, I think.
 Revision 1.1  1997/08/14 21:38:05  dkegel
 Initial revision
-------------------------------------------------------------------------*/

/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if (defined _WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <windows.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include "unistd.h"

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed"
   (Windows *linked* code has six of these ) */
#if (defined _WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/*-------------------------------------------------------------------------
 After sec seconds, raise the SIGTIMER exception.
 Cancels any previous alarm.
 Call with sec=0 to cancel previous alarm without setting new one.
 Should return number of seconds remaining from previous run, but don't yet.
-------------------------------------------------------------------------*/
unsigned alarm(unsigned sec);

/*-------------------------------------------------------------------------
 Sleep for sec seconds.
 Return value is undefined!
-------------------------------------------------------------------------*/
int sleep(unsigned sec);
