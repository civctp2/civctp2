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

 $Log: unistd.h $
 Revision 1.2  1997/08/14 21:48:40  dkegel
 Pick a signal that isn't usually used
 Revision 1.1  1997/08/14 21:38:06  dkegel
 Initial revision
-------------------------------------------------------------------------*/

#ifndef _unistd_h_
#define _unistd_h_

#ifndef SIGTIMER
#define SIGTIMER SIGTERM	/* sorry - can't define new signals :-( */
#endif

/*-------------------------------------------------------------------------
 After sec seconds, raise the SIGTIMER exception.
-------------------------------------------------------------------------*/
unsigned alarm(unsigned sec);

/*-------------------------------------------------------------------------
 Sleep for sec seconds.
-------------------------------------------------------------------------*/
int sleep(unsigned sec);

#endif
