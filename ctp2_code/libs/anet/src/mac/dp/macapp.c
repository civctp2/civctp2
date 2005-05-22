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
 $Log: macapp.c $
 Revision 1.1  1997/08/15 19:05:14  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef __POWERPC__

#include <stdio.h>
#include <Processes.h>
#include <Strings.h>
#include <string.h>
#include "dp.h"
#include "macapp.h"

/*--------------------------------------------------------------------------
Takes a long int and returns a point; used to convert eventRecord.where for 
 high-level applications.
--------------------------------------------------------------------------*/
Point longtopt(long n) {
	Point pt;
	pt.v = (short)(n >> 16);
	pt.h = (short)n;
	return pt;
}
/*--------------------------------------------------------------------------
Determines if there's a stub and if there is, returns its serial number stubPSN.
Requires stub to be called anetstub.
--------------------------------------------------------------------------*/
dp_result_t getstub(ProcessSerialNumber *pstubPSN)
{
	static int havestub = -1;
	OSErr err;
	ProcessInfoRec proc;
	ProcessSerialNumber myPSN;
	static ProcessSerialNumber stubPSN;
	char processName[FILENAME_MAX];
	FSSpec launchFSSpec;
	char buf[FILENAME_MAX];		// p2cstr alters its arg so use buffer
	
	/* check if already have determined if stub exists and if so return it */
	switch (havestub) {
		case -2:
			return dp_RES_BAD;
			break;
		case -1:
			break;
		case 0:
			return dp_RES_EMPTY;
			break;
		case 1:
			*pstubPSN = stubPSN;
			return dp_RES_OK;
			break;
		default:
			DPRINT(("macapp: error got havestub = %d\n", havestub));
			return dp_RES_BUG;
	}			
	
	/* find launcher of current process */
	proc.processInfoLength = sizeof(ProcessInfoRec);
	proc.processName = nil;
	proc.processAppSpec = nil;
	err = GetCurrentProcess(&myPSN);
	if (err != noErr) {
		DPRINT(("macapp: get current process number error %d\n", err));
		havestub = -2;
		return dp_RES_BAD;
	}
	err = GetProcessInformation(&myPSN, &proc);
	if (err != noErr) {
		DPRINT(("macapp: get current process info error %d\n", err));
		havestub = -2;
		return dp_RES_BAD;
	}
	proc.processName = &processName[0];
	proc.processAppSpec = &launchFSSpec;
	stubPSN = proc.processLauncher;
	/* check if launcher of current process was anetstub */
	err = GetProcessInformation(&stubPSN, &proc);
	if (err != noErr) {
		DPRINT(("macapp: get launcher process info error %d\n", err));
		havestub = -2;
		return dp_RES_BAD;
	}
	memcpy(buf, proc.processName, *proc.processName + 1);
	p2cstr(buf);
	DPRINT(("launcher was %s\n", buf));
	if (strcmp(buf, STUB_NAME)) {
		DPRINT(("macapp: found no stub\n", err));
		havestub = 0;
		return dp_RES_EMPTY;
	}
	*pstubPSN = stubPSN;
	havestub = 1;
	return dp_RES_OK;
}

#endif
