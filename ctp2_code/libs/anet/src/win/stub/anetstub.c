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
 Stub process for NetShell.
 $Log: anetstub.c $
 Revision 1.5  1997/10/07 02:13:27  anitalee
 stub now passes sessionId to dpio in environment.
 if dpio finds sessionId in environment, it ignores sessionId
 in commInitReq.
 This lets NetShell keep the same address every time it opens
 the modem or null modem drivers.
 Revision 1.4  1997/08/16 18:41:54  lbennett
 fixed bug on NT.
 Revision 1.3  1997/08/13 21:12:59  anitalee
 took out redefinition of MAXIMUM_WAIT_OBJECTS
 Revision 1.2  1997/08/06 22:06:50  anitalee
 Added relaunch flag; append -r to command line on second and following launches of NetShell
 Revision 1.1  1997/07/25 19:51:55  anitalee
 Initial revision
--------------------------------------------------------------------------*/
#ifdef WIN32

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <direct.h>
#include <winreg.h>
#include <winbase.h>
#include <winerror.h>
#include <winuser.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "anetstub.h"
#include "resource.h"

#define hKeyRoot HKEY_LOCAL_MACHINE
#define SubKey "SOFTWARE\\JavaSoft\\Java Runtime Environment\\1.1"

static HANDLE hpipeReadDup, hreadEvent, hcontEvent;
static HANDLE hSpawn = 0;
static HANDLE hSerial = 0;

/* Maximum size for dialog box title and message */
#define TITLELEN	64
#define MSGLEN		2048

#ifdef DEBUG
#ifdef PRINT_STDOUT
#define DPRINT(s) printf s
#else
#define DPRINT(s) write_log s
FILE *flog = NULL;
static void write_log(char *fmt, ...) {
	if (flog) {
		va_list args;
		va_start(args, fmt);
		vfprintf(flog, fmt, args);
		va_end(args);
		fflush(flog);
	}
}
#endif
#else
#define DPRINT(s)
#endif

/*----------------------------------------------------------------------
 Get info about JRE from registry.
----------------------------------------------------------------------*/
static int findJreInfoFromRegistry(char *path, char *args) {
	INT	   i;
	HKEY   hKey;				/* Key handle of NetShell */
	CHAR   ValueName[MAX_PATH]; /* Name of value. */
	DWORD  cbValueName;			/* Size of value name. */
	DWORD  dwType;     			/* Type of data. */
	CHAR   bData[MAX_PATH];   	/* Data buffer. */
	DWORD  cbData;  		  	/* Size of data buffer. */
	DWORD  retCode;
	CHAR   *Param;
	BOOL   found = FALSE;		/* We have found info in the registry */

	/* Open the key for jre and get info about it. */
	retCode = RegOpenKeyEx(hKeyRoot, SubKey, 0, KEY_EXECUTE, &hKey);
	if (retCode != ERROR_SUCCESS) {
		DPRINT(("stub: RegOpenKeyEx error for hKey = %d\n",
				retCode));
		return FALSE;
	}
	for (i = 0; retCode == ERROR_SUCCESS; i++) {
		bData[0] = '\0';
		ValueName[0] = '\0';
		cbData = MAX_PATH;
		cbValueName = MAX_PATH;
		/* Enumerate the key values. */
		retCode = RegEnumValue(hKey, i, ValueName, &cbValueName,
								NULL, &dwType, bData, &cbData);
		if (retCode != ERROR_SUCCESS) {
			if (dwType < REG_FULL_RESOURCE_DESCRIPTOR &&
								retCode != ERROR_NO_MORE_ITEMS) {
				DPRINT(("stub: ERROR, RegEnumValue = %d cbData = %d line %d\n"
					   , retCode, cbData, __LINE__));
 				return FALSE;
			}
		}
		DPRINT(("stub.findJreInfo...: i=%d, ValueName=%s, cbValueName=%d, dwType=%d,\n\t bData=%s, cbData=%d; retCode=%d\n", i, ValueName, cbValueName, dwType, bData, cbData, retCode));
		/* Process the value accordingly. */
		Param = _strlwr(ValueName);
		if(!strcmp("javahome", Param)) {
			DPRINT(("stub.findJreInfo...: Setting path and args to %s\n",bData));
			strcat(path, bData);
			strcat(args, bData);
			found = TRUE;
		}
		if(!strcmp("microversion", Param)) {
			DPRINT(("stub.findJreInfo...: found JRE 1.1.%s\n",bData));
			if(atoi(bData) < 6) {
				CHAR title[TITLELEN];
				CHAR msg[MSGLEN];
				LoadString(NULL, IDS_OLD_JRE_TITLE, title, TITLELEN);
				LoadString(NULL, IDS_OLD_JRE_MSG, msg, MSGLEN);
				MessageBox(NULL, msg, title,
					MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
				DPRINT(("stub: version of jre is too old"));
				return FALSE;
			}
		}
	}
	DPRINT(("stub.findJreInfo...: done, found=%d",found));
	return found;
}

/*----------------------------------------------------------------------
 Get info about JRE and use it to fill the character
 arrays that were passed as arguments.  Returns non-zero if error occurred.
----------------------------------------------------------------------*/
static int getinfo(char *path, char *args, char *cwd)
{
	CHAR   *Param;
	BOOL   pathFound = FALSE;
	struct _stat statbuf;
	char buf[MAX_PATH];

    strcpy(args, "-classpath \"");		// args for jre
	GetModuleFileName(NULL, cwd, MAX_PATH);		// directory stub is in
	if (cwd[strlen(cwd) - 1] == '\\')
		cwd[strlen(cwd) - 1] == '\0';
	if (Param = strrchr(cwd, '\\')) {
		*Param = '\0';
	} else {
		DPRINT(("stub.getinfo: no backslash in GetModuleFileName %s\n", cwd));
		return 1;
	}
	/* First check the jre\bin subdirectory of stub directory, then
	 * in the registry for jre */
	path[0] = '\0';
	strcpy(buf, cwd);
	strcat(buf, "\\jre\\bin\\jrew.exe");
	if (_stat(buf, &statbuf) == 0) {
		strcpy(buf, cwd);
		strcat(buf, "\\jre");
		strcat(path, buf);
		strcat(args, buf);
		DPRINT(("stub.getinfo: Local JRE at [%s]\n",path));
		pathFound = TRUE;
	} else
		pathFound = findJreInfoFromRegistry(path, args);
	if (!pathFound) {
		DPRINT(("stub.getinfo: Didn't find jre directory\n"));
		return 1;
	}
	strcat(path, "\\bin\\jrew.exe");
	strcat(args, "\\lib\\rt.jar\";jar\\atvi.jar;jar\\netshell.jar;jar\\JConfig.zip NetShell");
	
	if (!*path || !*cwd || !*args) {
		DPRINT(("stub.getinfo: Error getting NetShell parameters\n"));
		DPRINT((" got path, cwd, args = %s, %s, %s\n", path, cwd, args));
		return 1;
	}

	// Check that the JRE exists at the expected location
	if(fopen(path, "r") == NULL) {
		DPRINT(("stub.getinfo: JRE (jrew.exe) not found at [%s].\n",path));
		return 1;
	}

	DPRINT(("stub.getinfo: found JRE\n"));
	return 0;
}

/*--------------------------------------------------------------------------
 Thread to read from a pipe about newly created processes.  When process is
 read from pipe, sets global variable hSpawn equal to it and signals the
 main thread to add the new process to its list.  Terminates when stub exits.
--------------------------------------------------------------------------*/
DWORD WINAPI readThread(LPVOID lpvThreadParm) {
	int nread;
	unsigned long tag;
	HANDLE hbuf;

	while (1) {
		if (!ReadFile(hpipeReadDup, (char *)&tag, sizeof(tag), &nread, NULL) ||
			!nread ||
			!ReadFile(hpipeReadDup, (char *)&hbuf, sizeof(tag), &nread, NULL) ||
			!nread) {
			DPRINT(("stub.readThread: read err %x\n", GetLastError()));
			exit(1);
		}
		switch (tag) {
			case DPSTUB_TAG_PROCHDL:
				hSpawn = hbuf;
				break;
			case DPSTUB_TAG_SERHDL:
				hSerial = hbuf;
				break;
			default:
				DPRINT(("stub.readThread: unknown tag %x\n", tag));
				exit(1);
				break;
		}
		if (!SetEvent(hreadEvent)) {
			DPRINT(("stub.readThread: setting read event error %x.\n", GetLastError()));
			exit(1);
		}
		WaitForSingleObject(hcontEvent, INFINITE);
//		DPRINT(("stub: readThread: read %d bytes\n", nread));
//		DPRINT(("stub: readThread: got handle %x\n", hSpawn));
	}
	return 0;
}

/*--------------------------------------------------------------------------
 Launches NetShell, waits for Netshell and all of NetShell's launched
   processes to die and then relaunches NetShell if nothing exited with
   non-zero exit status.
--------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hinstExe, HINSTANCE hinstExePrev, LPSTR lpszCmdLine, int nCmdShow)
{
	int firsttime = 1;
	char cmd[2*MAX_PATH];
	char args[MAX_PATH];
	char cwd[MAX_PATH];
	char strhandle[MAX_HSTR];
	DWORD nhandles = 0;
	DWORD exitCode = 0;
	DWORD dwThreadId, iSignal;
	HANDLE hpipeRead, hpipeWrite, hSelf;
	HANDLE *hArray, *hStartArr;
	SECURITY_ATTRIBUTES saAttr;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

#ifdef DEBUG
#ifndef PRINT_STDOUT
	{	char logfile[MAX_PATH];
		sprintf(logfile, "%x", time(NULL));
		strcat(logfile, ".log");
		flog = fopen(logfile, "w");
	}
#endif
#endif
	/* Pick random session id for benefit of serial and modem drivers */
	sprintf(strhandle, "%x", GetTickCount());
	if (!SetEnvironmentVariable(DPSTUBID, strhandle)) {
		DPRINT(("stub: set environment variable error %x.\n", GetLastError()));
		exit(1);
	}
	hArray = (HANDLE *)malloc(MAXIMUM_WAIT_OBJECTS * sizeof(HANDLE));
	hStartArr = hArray;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	if (!DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &hSelf,  0, TRUE, DUPLICATE_SAME_ACCESS)) {
		DPRINT(("stub: error duplicating own handle\n"));
		exit(1);
	}
	sprintf(strhandle, "%x", hSelf);
	if (!SetEnvironmentVariable(DPSTUB, strhandle)) {
		DPRINT(("stub: set environment variable error %x.\n", GetLastError()));
		exit(1);
	}
//	DPRINT(("stub: current process handle is %s\n", strhandle));

	/* create event so read thread can signal when it's read a new process */
	if (!(hreadEvent = CreateEvent(NULL, FALSE, FALSE, "ReadEvent"))) {
		DPRINT(("stub: create read event error %x.\n", GetLastError()));
		exit(1);
	}
	*hArray++ = hreadEvent;
	nhandles++;
//	DPRINT(("stub: read event handle is %x\n", *(hArray - 1)));

	/* create event so read thread knows main thread has finished processing
	   hSpawn and can continue to get new processes */
	if (!(hcontEvent = CreateEvent(NULL, FALSE, FALSE, "ContinueReadEvent"))) {
		DPRINT(("stub: create continue read event error %x.\n", GetLastError()));
		exit(1);
	}
//	DPRINT(("stub: continue read event handle is %x\n", hcontEvent));

	/* create a pipe with read end uninheritable and write end inheritable*/
	if(!CreatePipe(&hpipeRead, &hpipeWrite, &saAttr, sizeof(HANDLE))) {
		DPRINT(("stub: create pipe error %x.\n", GetLastError()));
		exit(1);
	}
	sprintf(strhandle, "%x", hpipeWrite);
	if (!SetEnvironmentVariable(DPSTUBWPIPE, strhandle)) {
		DPRINT(("stub: set environment variable error %x.\n", GetLastError()));
		exit(1);
	}
	if (!DuplicateHandle(GetCurrentProcess(), hpipeRead, GetCurrentProcess(), &hpipeReadDup,  0, FALSE, DUPLICATE_SAME_ACCESS)) {
		DPRINT(("stub: error duplicating uninheritable handle\n"));
		exit(1);
	}
	CloseHandle(hpipeRead);
//	DPRINT(("stub: uninheritable pipe handle is %x.\n", hpipeReadDup));

	/* create a thread to read from pipe about new processes*/
	if (CreateThread(NULL, 0, readThread, 0, 0, &dwThreadId) < 0) {
		DPRINT(("stub: create thread error %x.\n", GetLastError()));
		exit(1);
	}

	/* set command and current working directory for CreateProcess() */
	{	char *buf;
		char path[2*MAX_PATH];
		char cmdline[1024];
		if (getinfo(path, args, cwd) != 0) {
			CHAR title[TITLELEN];
			CHAR msg[MSGLEN];
			LoadString(NULL, IDS_NO_JRE_TITLE, title, TITLELEN);
			LoadString(NULL, IDS_NO_JRE_MSG, msg, MSGLEN);
			MessageBox(NULL, msg, title,
				MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
			exit(1);
		}
		strcat(cmd, "\"");
		strcat(cmd, path);
		strcat(cmd, "\" ");
		strcat(cmd, args);

		buf = GetCommandLine();
		if (!buf) {
			DPRINT(("stub: error getting commandline args; quitting\n"));
			exit(1);
		}
		strcpy(cmdline, buf);
#ifdef DEFAULTARGS
		/* If no switches given, append default args */
		if (!strstr(cmdline, " -")) {
			strcat(cmdline, " ");
			strcat(cmdline, DEFAULTARGS);
		}
#endif
		buf = cmdline;
		DPRINT(("args: %s\n", buf));
		{
			OSVERSIONINFO VersionInfo;
			VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if (!GetVersionEx(&VersionInfo)) {
				DPRINT(("stub: cannot determine OS; quitting\n"));
				exit(1); 
			}
			if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT || VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
				while (' ' == *buf || '\t' == *buf) buf++;
				if (*buf == '\"') {
					buf++;
					if (!(buf = strchr(buf, '\"'))) {
						DPRINT(("stub: cannot get commandline args; quitting\n"));
						exit(1);
					}
					buf++;
				} else {
					if (buf = strchr(buf, ' ')) {
						buf++;
					} else {
						buf = "\0";
					}
				}
			} else {
				CHAR title[TITLELEN];
				CHAR msg[MSGLEN];
				LoadString(NULL, IDS_UNSP_OS_TITLE, title, TITLELEN);
				LoadString(NULL, IDS_UNSP_OS_MSG, msg, MSGLEN);
				MessageBox(NULL, msg, title,
					MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
				DPRINT(("OS not supported; quitting\n"));
				exit(1);
			}
			while (' ' == *buf || '\t' == *buf) buf++;
			if (2 * MAX_PATH < strlen(cmd) + strlen(buf) + 2) {
				DPRINT(("stub: cmd buffer not large enough to hold args; quitting\n"));
				exit(1);
			}
			strcat(cmd, " ");
			strcat(cmd, buf);
		}
	}
	DPRINT(("cmd: %s\n", cmd));

	/* launch NetShell then wait for NetShell and all its launched processes
	   to die, then if all exit status are zero, relaunch NetShell */
	do {
		memset(&pi, 0, sizeof(pi));
		memset(&si, 0, sizeof(si));
		si.cb = sizeof(si);
		/* launch NetShell */
    	DPRINT(("stub: Launching %s from %s.\n", cmd, cwd));
		if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, cwd, &si, &pi)) {
			DPRINT(("stub: Error launching %s from %s; quitting\n", cmd, cwd));
			DPRINT(("stub: GetLastError returns %x", GetLastError()));
			exit(1);
		}
		if (firsttime) {
			char *buf = " -r";
			if (2 * MAX_PATH < strlen(cmd) + strlen(buf) + 1) {
				DPRINT(("stub: cmd buffer not large enough to hold args\n"));
				exit(1);
			}
			strcat(cmd, buf);
			firsttime = 0;
		}
		CloseHandle(pi.hThread);
		*hArray++ = pi.hProcess;
		nhandles++;
		DPRINT(("stub: Launch complete; shell handle %x.\n", *(hArray-1)));

		/* Wait for NetShell and all its launched processes to exit */
		do {
			iSignal = WaitForMultipleObjects(nhandles, hStartArr, FALSE, INFINITE);
			if (WAIT_FAILED == iSignal) {
				DPRINT(("stub: wait error %x.\n", GetLastError()));
				exit(1);
			}
			iSignal -= WAIT_OBJECT_0;
			if (iSignal >= 0 && iSignal < nhandles) {
				if (*(hStartArr + iSignal) != hreadEvent) {
					if (!GetExitCodeProcess(*(hStartArr + iSignal), &exitCode)) {
						DPRINT(("stub: Error reading spawned process\n"));
						exit(1);
					}
					DPRINT(("stub: deleting handle %x, exit code %d.\n", *(hStartArr + iSignal), exitCode));
					if (exitCode) break;
					CloseHandle(*(hStartArr + iSignal));
					if (nhandles && iSignal < nhandles - 1) {
						*(hStartArr + iSignal) = *(hStartArr + nhandles - 1);
					}
					nhandles--;
					hArray = hStartArr + nhandles;
				} else {
					// DPRINT(("stub: got new handle %x\n", hSpawn));
					if (hSpawn != 0) {
						if (hSpawn < 0) {
							DPRINT(("stub: Error reading spawned process; hSpawn %x\n", hSpawn));
							exit(1);
						}
						DPRINT(("stub: adding handle %x.\n", hSpawn));
						*hArray++ = hSpawn;
						nhandles++;
						hSpawn = 0;
					} else if (hSerial != 0) {
						char *pstrhandle;
						if (hSerial < 0 && hSerial != INVALID_HANDLE_VALUE) {
							DPRINT(("stub: Error reading serial handle; hSerial %x\n", hSerial));
							exit(1);
						}
						if (hSerial == INVALID_HANDLE_VALUE) {
							if (GetEnvironmentVariable(DPSTUBMODEM, strhandle, 256)) {
								HANDLE baseadr;
								sscanf(strhandle, "%x", &baseadr);
								CloseHandle(baseadr);
								DPRINT(("stub: deleted serial h:%x\n",baseadr));
								pstrhandle = NULL;
							}
						} else {
							DPRINT(("stub: setting serial h:%x.\n", hSerial));
							sprintf(strhandle, "%x", hSerial);
							pstrhandle = strhandle;
						}
						if (!SetEnvironmentVariable(DPSTUBMODEM, pstrhandle)) {
							DPRINT(("stub: set environment variable error %x.\n", GetLastError()));
							exit(1);
						}
						hSerial = 0;
					} else {
						DPRINT(("stub: Error no handle set\n"));
						exit(1);
					}
					if (!SetEvent(hcontEvent)) {
						DPRINT(("stub: set continue read event error %x.\n", GetLastError()));
						exit(1);
					}
				}
			} else {
				DPRINT(("stub: error wait returned invalid handle %d\n", iSignal));
				exit(1);
			}
//			DPRINT(("stub: nhandles = %d\n", nhandles));
		} while (nhandles > 1);
//		DPRINT(("stub: exited inside loop\n"));
#ifdef NOLOOP		
		break;
#endif
	} while (!exitCode);
	return 0;
}

#endif
