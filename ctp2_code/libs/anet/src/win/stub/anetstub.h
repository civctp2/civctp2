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

/*----------------------------------------------------------------------
 The stub communicates with processes through environment variables listed
 below.  Processes use these environment variables to share common info
 (inherited serial handles, session id's) as well as communicate back
 to the stub (through pipe).  Things processes need to notify the stub
 about are newly launched processes and newly created serial handles.
----------------------------------------------------------------------*/

#ifndef anetstub_h
#define anetstub_h

#define MAX_HSTR 2 * sizeof(HANDLE) + 1

/*----------------------------------------------------------------------
 Environment variables set by stub.
 The first three are set by stub before it launches any processes and
 are inherited by all processes.
 DPSTUBMODEM is set when stub receives a value from the application.
 all subsequent processes inherit this handle; howevern the originating
 application is responsible for setting this environment variable itself.
----------------------------------------------------------------------*/
#define DPSTUB "thisDPSTUB"            /* handle to stub */
#define DPSTUBID "thisDPSTUBID"        /* session id picked by stub */
#define DPSTUBWPIPE "thisDPSTUBWPIPE"  /* handle to write end of pipe to stub */
#define DPSTUBMODEM "thisDPSTUBMODEM"  /* handle to serial port saved by stub */

/*----------------------------------------------------------------------
 Tag passed through pipe by applications along with a handle value;
 used by stub to identify type associated with the handle.

 DPSTUB_TAG_PROCHDL identifies new process handles launched by applications.
 DPSTUB_TAG_SERHDL identifies either new serial handle created by application
 or if handle value is set to INVALID_HANDLE_VALUE, tells stub to delete
 its copy of the serial handle.
----------------------------------------------------------------------*/
#define DPSTUB_TAG_PROCHDL   0x01  /* tag for process handle */
#define DPSTUB_TAG_SERHDL    0x02  /* tag for serial handle */

#endif
