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
 General functions needed by dpLaunchApp() and dpExitFromApp().
 $Log: macapp.h $
 Revision 1.2  1997/08/19 23:33:33  anitalee
 took out redefinition of DPRINT
 Revision 1.1  1997/08/15 19:05:19  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef __POWERPC__

#ifndef macapp_h
#define macapp_h

#define STUB_NAME "anetstub"
#define MAX_PATH 255
#define STUB_EVENTID 'STUB'		// defines events used by the stub
#define STUB_LAUNCLASS 'LAUN'	// launch request for stub
#define STUB_OBITCLASS 'OBIT'	// app die requests relaunch of netshell
#define pttolong(s) ((long) ((s).v << 16 | (s).h ))

Point longtopt(long n);

#ifdef dp_h
dp_result_t getstub(ProcessSerialNumber *stubPSN);
#endif

#endif
#endif
