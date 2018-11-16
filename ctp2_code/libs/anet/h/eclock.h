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

/*------------------------------------------------------------------------
 eclock() is a function that simulates clock() as it is implemented on PC's;
 it returns elasped time in ECLOCKS_PER_SEC units.

 $Log: eclock.h $
 Revision 1.2  1997/08/30 22:00:36  dkegel
 eclock() now returns milliseconds since midnight GMT under Win32,
 which might help compare log files between different systems.
 Revision 1.1  1997/05/20 19:25:44  dkegel
 Initial revision
------------------------------------------------------------------------*/
#ifndef eclock_h  /* Protect against double inclusion */
#define eclock_h

#ifdef UNIX

#ifdef SPARC

/****************************** Solaris 2.x **************************/
#include <sys/times.h>
#include <limits.h>
/* units for Solaris only; not correct for Sun 4.x */
#define ECLOCKS_PER_SEC CLK_TCK

clock_t eclock_sparc(void);
#define eclock() eclock_sparc()

#else /* Not SPARC */

/********************************* Linux ******************************/
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#define ECLOCKS_PER_SEC ((int)sysconf(_SC_CLK_TCK)) /*CLK_TCK */

#define eclock() times(NULL)

#endif /* Not SPARC */

/****************************** Macintosh *****************************/
#elif defined(__MWERKS__) /* Not UNIX, Macintosh */

#include <OSUtils.h>
#include <time.h>
#define ECLOCKS_PER_SEC 1000 /* eclock in milliseconds */

#else /* Not UNIX, Not Macintosh */

/************************* MS-DOS, MS-Windows *************************/
#if defined(_WIN32)
#include <time.h>
#define ECLOCKS_PER_SEC 1000 /* eclock_win32 in milliseconds */
clock_t eclock_win32(void);
#define eclock() eclock_win32()
#else
#include <time.h>
#define ECLOCKS_PER_SEC CLOCKS_PER_SEC
#define eclock() clock()
#endif  /* WIN32 */

#endif

#endif  /* Protect against double inclusion */
