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
 Launches application specified by appParam.
 $Log: launchap.c $
 Revision 1.2  1997/08/13 20:43:13  dkegel
 Print out name of command being launched, and flush log, before launching.
 Revision 1.1  1997/07/25 18:21:19  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef WIN32

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "dp.h"
#include "anetstub.h"

#define SLEEP_DEFAULT 0

/*--------------------------------------------------------------------------
 Takes the handle of the process created using CreateProcess() and notifies
  stub of the new process, if stub exists.
 If stub exists, it must have set environment variables DPSTUB (handle to stub)
   and DPSTUBWPIPE (handle to write end of pipe to stub).
--------------------------------------------------------------------------*/
static dp_result_t notifystub(HANDLE hprocess)
{
	char strhandle[MAX_HSTR];
	int nwrite;
	unsigned long tag = DPSTUB_TAG_PROCHDL;
	HANDLE hstub, hspawn, hpipe;

	if (!GetEnvironmentVariable(DPSTUB, strhandle, MAX_HSTR)) {
		DPRINT(("launchap: could not find stub\n"));
		return dp_RES_EMPTY;
	}
	sscanf(strhandle, "%x", &hstub);
/*	DPRINT(("launchap: Got stub handle %x\n", hstub)); */
	if (!DuplicateHandle(GetCurrentProcess(), hprocess, hstub, &hspawn, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
		DPRINT(("launchap: duplicating handle error %x.\n", GetLastError()));
		return dp_RES_BAD;
	}
/*	DPRINT(("launchap: duped process %x\n", hspawn)); */
	if (!GetEnvironmentVariable(DPSTUBWPIPE, strhandle, MAX_HSTR)) {
		DPRINT(("launchap: could not find stub pipe write handle\n"));
		return dp_RES_BUG;
	}
	sscanf(strhandle, "%x", &hpipe);
	if (!WriteFile(hpipe, (char *)&tag, sizeof(tag), &nwrite, NULL) ||
		!WriteFile(hpipe, (char *)&hspawn, sizeof(hspawn), &nwrite, NULL)) {
		DPRINT(("launchap: writing error %x.\n", GetLastError()));
		return dp_RES_BAD;
	}
	/* Sleep() is used to let the stub run so it can read the pipe
	    before the function calling dpLaunchApp exits. */
	Sleep(SLEEP_DEFAULT);
	return dp_RES_OK;
}

/*--------------------------------------------------------------------------
 Launches an application from another application and notifies stub
  of the newly launched application, if stub exists.
 If /NEWCONSOLE is found in appParam->shellOpts, a new console is created.
 If /OLDCONSOLE is found in appParam->shellOpts, the old console is used.

 result may be NULL.  If result is not NULL, it is filled with system
 dependant information about the newly launched app.  On Win32 systems,
 if result is NULL, the handle to the newly created process is closed,
 otherwise it is left open for the caller.
--------------------------------------------------------------------------*/
DP_API dp_result_t dpLaunchApp(dp_appParam_t *appParam)
{
	return dpLaunchApp2(appParam, NULL);
}

DP_API dp_result_t dpLaunchApp2(dp_appParam_t *appParam, dpLaunchApp_result_t *result)
{
	int status;
	char cmd[MAX_PATH*2];
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	DPRINT(("dpLaunchApp:\n"));
	if (!appParam) {
		return dp_RES_BUG;
	}
	strcpy(cmd, appParam->path);
	if (*appParam->args) {
		strcat(cmd, " ");
		strcat(cmd, appParam->args);
	}
	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	DPRINT(("dpLaunchApp: cmd %s\n", cmd));
	dp_flushLog();
	if (strstr(appParam->shellOpts, "/NEWCONSOLE")) {
		if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NEW_CONSOLE |
					CREATE_NEW_PROCESS_GROUP, NULL, appParam->cwd, &si, &pi)) {
			return dp_RES_BAD;
		}
		DPRINT(("launched app w/ new console.\n"));
	} else if (strstr(appParam->shellOpts, "/OLDCONSOLE")) {
		if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE,
					CREATE_NEW_PROCESS_GROUP, NULL, appParam->cwd, &si, &pi)) {
			return dp_RES_BAD;
		}
		DPRINT(("launched app w/ old console.\n"));
	} else {
		if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW |
					CREATE_NEW_PROCESS_GROUP, NULL, appParam->cwd, &si, &pi)) {
			return dp_RES_BAD;
		}
		DPRINT(("launched app w/ no console.\n"));
	}
	CloseHandle(pi.hThread);
	status = notifystub(pi.hProcess);
	if (result) {
		result->hProcess = pi.hProcess;
	} else {
		CloseHandle(pi.hProcess);
	}
	return status;
}

#endif
