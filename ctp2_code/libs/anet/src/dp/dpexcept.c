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

#ifdef _WIN32
#define LOG_CRASHES_TO_NET
#endif

#include <stdio.h>
#include "dp2.h"
#include "portable.h"
#include "eclock.h"
#include "dpprivy.h"
#include "dpexcept.h"
#ifdef _WIN32
#include "detect.h"
#include "getdisp.h"
#include <direct.h>
#elif defined(UNIX)
#include <unistd.h>
#endif

static int getAppParam(aeh_appParam_t *aehapp)
{
	dp_result_t err;
	char cwd[aeh_MAX_PATH];
	dp_appParam_t dpapp;
	char appname[aeh_MAX_PATH];
	char apppath[aeh_MAX_PATH];
	char appargs[aeh_MAX_PATH];
	char appcwd[aeh_MAX_PATH];
	char appshellOpts[aeh_MAX_PATH];

	aeh_SetCurrent(__LINE__, __FILE__);
	dpapp.name = &appname[0];
	dpapp.path = &apppath[0];
	dpapp.args = &appargs[0];
	dpapp.cwd = &appcwd[0];
	dpapp.shellOpts = &appshellOpts[0];
	if (!aehapp) return 1;
	memset(aehapp, 0, sizeof(aeh_appParam_t));
	if (!getcwd(cwd, sizeof(cwd)))
		return 1;
	aeh_SetCurrent(__LINE__, __FILE__);
	err = dpReadAnetInf(cwd, &dpapp);
	if (err != dp_RES_OK) return 1;
	aehapp->sessionType = dpapp.sessionType;
	aehapp->platform = dpapp.platform;
	aehapp->language = dpapp.language;
	aehapp->major_version = dpapp.current.major;
	aehapp->minor_version = dpapp.current.minor;
	aeh_SetCurrent(__LINE__, __FILE__);
	return 0;
}

#ifdef _WIN32
#define MAX_DISPLAYDEV 4
/*--------------------------------------------------------------------------
 Gets a string describing the system into systemDesc, which has length len.
 Prepends the string crshtxt.
--------------------------------------------------------------------------*/
static void getSystemInfo(char *systemDesc, char *crshtxt, unsigned int len)
{
	static int firstCall = TRUE;
	static char buf2d[aeh_BUF_MAXLEN] = "";
	static char buf3d[aeh_BUF_MAXLEN] = "";

	aeh_SetCurrent(__LINE__, __FILE__);
	systemDesc[0] = '\0';
	if (crshtxt) {
		if ((len > strlen(crshtxt)) && (strlen(crshtxt) <= 100))
			strcat(systemDesc, crshtxt);
		else if (len > strlen(crshtxt)) {
			strncat(systemDesc, crshtxt, 100);
			systemDesc[101] = '\0';
		} else {
			strncat(systemDesc, crshtxt, len - 1);
			systemDesc[len - 1] = '\0';
			return;
		}
	}
	/* Build system info strings if this is the first call to getSystemInfo,
	 * else use the results of our first call.
	 */
	if (firstCall) {
#if 0
		struct display_t DispEntry[MAX_DISPLAYDEV];
		int nDisp = MAX_DISPLAYDEV, nFound, i;
#endif

		aeh_SetCurrent(__LINE__, __FILE__);
		buf2d[0] = '\0';
#if 0
		/* GetDisplayProfileData is buggy, and crashes reliably with HG2 */
		nFound = GetDisplayProfileData(&DispEntry[0], nDisp);
		aeh_SetCurrent(__LINE__, __FILE__);
		for (i = 0; i < nFound; i++) {
			sprintf(&buf2d[strlen(buf2d)], "%s,%s,%x,%x|", DispEntry[i].szName, DispEntry[i].szDriver, DispEntry[i].dwFileVersionMS, DispEntry[i].dwFileVersionLS);
		}
		if (buf2d[0]) buf2d[strlen(buf2d) - 1] = '\0';
		aeh_SetCurrent(__LINE__, __FILE__);
#endif
		Get3DHardware(buf3d, aeh_BUF_MAXLEN);
		aeh_SetCurrent(__LINE__, __FILE__);
		firstCall = FALSE;
	}
	if (!strlen(buf2d) && !strlen(buf3d))
		return;
	aeh_SetCurrent(__LINE__, __FILE__);
	if (buf2d[0]) {
		int tmplen;
		if (crshtxt) strcat(systemDesc, "@@##");
		tmplen = strlen(systemDesc) + 1;
		if (len > tmplen + strlen(buf2d))
			strcat(systemDesc, buf2d);
		else {
			strncpy(systemDesc, buf2d, len - tmplen);
			systemDesc[len - 1] = '\0';
			return;
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	if (buf3d[0]) {
		int tmplen;
		if (buf2d[0])
			strcat(systemDesc, ";");
		else if (crshtxt)
			strcat(systemDesc, "@@##");
		tmplen = strlen(systemDesc) + 1;
		if (len > tmplen + strlen(buf3d))
			strcat(systemDesc, buf3d);
		else {
			strncat(systemDesc, buf3d, len - tmplen);
			systemDesc[len - 1] = '\0';
			return;
		}
	}
	aeh_SetCurrent(__LINE__, __FILE__);
}
#else
/* not implemented */
static void getSystemInfo(char *systemDesc, char *crshtxt, unsigned int len)
{
	systemDesc[0] = '\0';
	if (crshtxt) {
		if ((len > strlen(crshtxt)) && (strlen(crshtxt) <= 100))
			strcat(systemDesc, crshtxt);
		else if (len > strlen(crshtxt)) {
			strncat(systemDesc, crshtxt, 100);
			systemDesc[101] = '\0';
		} else {
			strncat(systemDesc, crshtxt, len - 1);
			systemDesc[len - 1] = '\0';
			return;
		}
	}
	return;
}
#endif

#ifndef NO_NETWORK

static dptab_table_t *getExceptionTable(dptab_t *dptab)
{
	dp_result_t err;
	dptab_table_t *tab;
	char key[dptab_KEY_MAXLEN];
	key[0] = dp_KEY_EXCEPTIONS;
	tab = dptab_getTable(dptab, key, 1);
	if (!tab) {
		err = dptab_createTable(dptab, &tab, key, 1, 0, NULL, NULL, NULL, NULL);
		if (err != dp_RES_OK && err != dp_RES_ALREADY)
			tab = NULL;
	}
	return tab;
}

/*** Functions for client ***/

static int dp_makeExceptionRecords(unsigned char *buf, unsigned int buflen, aehlog_t *aehlog)
{
	int err;
	unsigned int ninst;
	aeh_buf_t aehbuf;
	int aehlog_tag = aehlog_MAGIC;
	unsigned char *ptr = buf;
	while (((err = aehlog_readExceptionRecord(aehlog, &aehbuf, &ninst)) == aeh_RES_OK)&&
		  (ptr + (aehbuf.buflen + 2 * sizeof(unsigned int)) < buf + buflen)) {
		writeSwap((void**)&ptr, &aehlog_tag, sizeof(aehlog_tag));
		writeSwap((void**)&ptr, &ninst, sizeof(ninst));
		writeSwap((void**)&ptr, &aehbuf.buflen, sizeof(aehbuf.buflen));
		memcpy(ptr, aehbuf.buf, aehbuf.buflen);
		ptr += aehbuf.buflen;
	}
	if (err == aeh_RES_BUG)
		aehlog_delete(aehlog);
	return (ptr - buf);
}

/*--------------------------------------------------------------------------
 Send exception records from file to the game server.
 Exception file is deleted afterwards if exception records were succesfully
  queued for delivery to game server.
 Input: dptab
        h (set to game server handle)
        aehlog (if NULL, uses default exception file;
               otherwise, specifies exception file set using aehlog_Init())
 Return: dp_RES_BAD if exception table doesn't exist or can't be created
         dp_RES_EMPTY if there are no exception records found
         otherwise return status of dptab_addSubscriber(), dptab_set()
--------------------------------------------------------------------------*/
dp_result_t dp_publishExceptions(dptab_t *dptab, playerHdl_t h, aehlog_t *aehlog)
{
	unsigned char buf[aehlog_MAXSEND];
	unsigned int aehsize;
	dp_result_t err;
	aehlog_t aehlogtmp;
	aehlog_t *aehlogptr;

	{
		const char *s;
		dpini_findSection("Debug");
		s = dpini_readParameter("nouploadcrash", FALSE);
		if (s && *s && (atoi(s) == 1))
			return dp_RES_OK;
	}
	if (!aehlog) {
		aehlog_Create("", &aehlogtmp);
		aehlogptr = &aehlogtmp;
	} else
		aehlogptr = aehlog;
	aehsize = dp_makeExceptionRecords(buf, sizeof(buf), aehlogptr);
	aehlog_close(aehlogptr);
	if (aehsize) {
		short randnum;
		dptab_table_t *tab;
		char subkey[dptab_KEY_MAXLEN];
		int subkeylen = 0;
		if (!(tab = getExceptionTable(dptab))) return dp_RES_BAD;
		err = dptab_addSubscriber(dptab, tab, h);
		if (err != dp_RES_OK) {
			DPRINT(("dp_publishExceptions: can't add h:%x as subscriber?, err:%d\n", h, err));
			return err;
		}
		/* select a random subkey */
		randnum = (short) ((eclock() + rand()) & 0xffff);
		subkey[subkeylen++] = dpGETSHORT_FIRSTBYTE(randnum);
		subkey[subkeylen++] = dpGETSHORT_SECONDBYTE(randnum);
		err = dptab_set(dptab, tab, subkey, subkeylen, buf, aehsize, 1, PLAYER_ME);
		DPRINT(("dp_publishExceptions: set table 13 err %d\n", err));
		if (err == dp_RES_OK)
			aehlog_delete(aehlogptr);
	} else
		return dp_RES_EMPTY;
	return err;
}

/*** Functions for server ***/

/*--------------------------------------------------------------------------
 Set callback for the exception table. Used by the game server to process
  exception records when received by dptab.
 Input: dptab, cb, context
 Return: dp_RES_BAD if exception table doesn't exist or can't be created
         otherwise return status of dptab_setTableCallback()
--------------------------------------------------------------------------*/
dp_result_t dp_setExceptionCb(dptab_t *dptab, dptab_status_cb cb, void *context)
{
	dptab_table_t *tab;
	if (!(tab = getExceptionTable(dptab))) return dp_RES_BAD;
	return dptab_setTableCallback(tab, cb, context);
}

/*--------------------------------------------------------------------------
 Allow exception records from host.
 Input: dptab
        h (handle of host to allow records from)
 Return: dp_RES_BAD if exception table doesn't exist or can't be created
         otherwise return status of dptab_addPublisher()
--------------------------------------------------------------------------*/
dp_result_t dp_subscribeExceptions(dptab_t *dptab, playerHdl_t h)
{
	dp_result_t err;
	dptab_table_t *tab;
	char key[dptab_KEY_MAXLEN];
	key[0] = dp_KEY_EXCEPTIONS;
	if (!(tab = getExceptionTable(dptab))) return dp_RES_BAD;
	err = dptab_addPublisher(dptab, tab, key, 1, h);
	if (err != dp_RES_OK) {
		DPRINT(("dp_subscribeExceptions: can't add h:%x as publisher?, err:%d\n", h, err));
	}
	return err;
}

/*--------------------------------------------------------------------------
 Save an exception record to file.
 Input: buf (exception record packet)
        buflen (length of buf)
        fmaxsize (if file larger than fmaxsize, delete file contents before
                  writing packet)
        aehlog (specifies exception file set using aehlog_Init();cannot be NULL)
 Return: dp_RES_BAD if couldn't write buf to file
         otherwise, dp_RES_OK
--------------------------------------------------------------------------*/
dp_result_t dp_handleExceptionRecords(unsigned char *buf, unsigned int buflen, unsigned int fmaxsize, aehlog_t *aehlog)
{
#if 0
	aeh_buf_t aehbuf;
	int aehlog_tag;
#endif
	int err;
	unsigned char *ptr = buf;
	if (!aehlog || !buf) return dp_RES_BAD;
	err = aehlog_appendMultExceptionRecords(aehlog, buf, buflen, fmaxsize);
	if (err == aeh_RES_BUG)
		aehlog_delete(aehlog);
#if 0
	while (ptr < buf + buflen) {
		readSwap((void**)&ptr, &aehlog_tag, sizeof(aehlog_tag));
		readSwap((void**)&ptr, &ninst, sizeof(ninst));
		readSwap((void**)&ptr, &aehbuf.buflen, sizeof(aehbuf.buflen));
		if ((aehlog_tag != aehlog_MAGIC) || (aehlog.buflen > aeh_BUF_MAXLEN) ||
			(ptr + aehbuf.buflen) > (buf + buflen))
			return 1;
		memcpy(aehbuf.buf, ptr, aehbuf.buflen);
		ptr += aehbuf.buflen;
		if (aehlog_writeExceptionRecord(aehlog, &aehbuf, ninst, fmaxsize)) return 1;
	}
#endif
	return err;
}

#endif  /*ifdef NO_NETWORK */

#ifdef LOG_CRASHES_TO_NET
/*--------------------------------------------------------------------------
 Convert a binary buf of length len into a printable hexadecimal string
 (with no spaces).
 hexbuf must be at least 2*len + 1 characters in length.
 Returns hexbuf.
--------------------------------------------------------------------------*/
static char *buf2hex(const char *buf, int len, char *hexbuf)
{
	int i;
	for (i = 0; i < len; i++) {
		sprintf(hexbuf + 2*i, "%02x", (unsigned char)buf[i]);
	}
	hexbuf[2*len] = '\0';
	return hexbuf;
}

/*--------------------------------------------------------------------------
 Send a crash log to a listening server.
 Launches an outside app with a dialog box to do this, so non-fatal
 assertion failures will be lost.
 Returns aeh_RES_OK on successful launch (but app may still fail).
--------------------------------------------------------------------------*/
static int sendtoserver(aeh_t *aeh)
{
	int err;
	int len;
	aeh_buf_t aehbuf;
	char atvilog[aeh_BUF_MAXLEN+12];
	char buf[2];
	char cmd[256 + 5 + (aeh_BUF_MAXLEN + 12)*2];
	char sendcrsh_exe[] = "sendcrsh.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	if (NULL != GetModuleHandle(sendcrsh_exe)) {
		aehDPRINT(("sendtoserver: %s already loaded\n", sendcrsh_exe));
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	err = aeh_writeOutputStream(aeh, &aehbuf);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (err == aeh_RES_FULL) {
		aehDPRINT(("sendtoserver: info size larger than aeh_BUF_MAXLEN\n"));
	} else if (err != aeh_RES_OK) {
		return err;
	}
	len = aehlog_writetobuf(&aehbuf, 1, atvilog, aeh_BUF_MAXLEN+12);
	if (len == -1) {
		aehDPRINT(("sendtoserver: aehlog_writetobuf error\n"));
		return aeh_RES_BUG;
	}
	/* FIXME: should use gameserver's hostname:port */
#if 1
	sprintf(cmd, "%s %s %d ", sendcrsh_exe, "aeh.activision.com", 21157);
#else
	sprintf(cmd, "%s %s %d ", sendcrsh_exe, "iserv.activision.com", 21157);
#endif
	buf[0] = dpGETSHORT_FIRSTBYTE(len);
	buf[1] = dpGETSHORT_SECONDBYTE(len);
	buf2hex(buf, 2, cmd + strlen(cmd));
	buf2hex(atvilog, len, cmd + strlen(cmd));
	aehDPRINT(("sendtoserver: calling:%s\n", cmd));

	/* launch sendcrsh */
	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	if (!SetEnvironmentVariable("SENDCRSH", "Y")) {
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);
		aehDPRINT(("sendtoserver: setenv error:%s\n", lpMsgBuf));
		LocalFree( lpMsgBuf );
		return aeh_RES_BUG;
	}
	if (!CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW |
			CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi)) {
#ifdef DEBUG_PRNT
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
		);
		aehDPRINT(("sendtoserver: command failed with error:%s\n", lpMsgBuf));
		LocalFree( lpMsgBuf );
#endif
		return aeh_RES_BAD;
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	aeh_SetCurrent(__LINE__, __FILE__);
	return aeh_RES_OK;
}
#else
static int writetofile(aeh_t *aeh)
{
	int err;
	aeh_buf_t aehbuf;
	aehlog_t aehlog;
	aeh_SetCurrent(__LINE__, __FILE__);
	err = aeh_writeOutputStream(aeh, &aehbuf);
	if (err != aeh_RES_OK && err != aeh_RES_FULL) return err;
	if (err == aeh_RES_FULL) {
		aehDPRINT(("writetofile: info size larger than aef_BUF_MAXLEN\n"));
	}
	aeh_SetCurrent(__LINE__, __FILE__);
	err = aehlog_Create("", &aehlog);
	if (err != aeh_RES_OK) return err;
	err = aehlog_writeExceptionRecord(&aehlog, &aehbuf, 1, aehlog_MAXLEN);
	if (err == aeh_RES_BUG)
		aehlog_delete(&aehlog);
	aehlog_close(&aehlog);
	aeh_SetCurrent(__LINE__, __FILE__);
	return err;
}
#endif

#ifdef _WIN32
/*--------------------------------------------------------------------------
 Always force exception handler to be executed when there is a problem
--------------------------------------------------------------------------*/
LONG _cdecl StupidExceptionFilter (LPEXCEPTION_POINTERS ep)
{
	return (EXCEPTION_EXECUTE_HANDLER);
}
#endif

/*--------------------------------------------------------------------------
 Records crash info to logfile. Designed to be called from an exception handler.
 For Win32, pException is the LPEXCEPTION_POINTERS structure returned by
  GetExceptionInformation().
 crshtxt is included in the crash record; crshtxt is truncated if longer than
  100 bytes or if the crash record is already 512 bytes.
 Returns dp_RES_ALREADY if dpReportCrash(Ex) was called previously; in this
  case, does not write a new crash record to log unless the crash was caused
  during dpReportCrash(Ex).
 See aeh.htm for more info.
--------------------------------------------------------------------------*/
#ifdef _WIN32
DP_API dp_result_t DP_APIX dpReportCrash(LPEXCEPTION_POINTERS pException)
{
	return dpReportCrashEx(pException, NULL);
}

DP_API dp_result_t DP_APIX dpReportCrashEx(LPEXCEPTION_POINTERS pException, char *crshtxt)
{
	static short bPrevExc = 0;
	static short bPrevCall = 0;
	int err;
	aeh_t aeh;
	aeh_appParam_t aehapp;
	char systemDesc[aeh_BUF_MAXLEN];

	/* Catch crashes in dpReportCrashEx and just return */
	__try {
		/* see if there were any previous exceptions thrown before this one; if so,
		 * the handler is generating an exception (possibly here). If this is first
		 * time, try to log this in a simple fashion; otherwise, just return
		 */
		if (bPrevExc) {
#ifndef LOG_CRASHES_TO_NET
			aeh_buf_t aehbuf;
			aehlog_t aehlog;
#endif
			if (bPrevExc > 1)
				return dp_RES_ALREADY;
			bPrevExc++;
			memset(&aeh, 0, sizeof(aeh_t));
			aeh.retcode = aeh_EXCEPTION_CODE;

#ifdef DEBUG_AEHEXCP
			/* No aeh_SetCurrent's can preceed this line in dpReportCrashEx -
			 * the actual line number of the crash in aeh would be overwritten
			 */
			aeh_GetCurrent(&aeh.assertln, &aeh.assertfile);
#endif
#ifdef LOG_CRASHES_TO_NET
			/* Don't write anything to disk; might be dangerous during a crash;
			 * just send over the net.
			 */
			err = sendtoserver(&aeh);
#else
			/* append to file, because writetofile might have crashed */
			if ((err = aeh_writeOutputStream(&aeh, &aehbuf)) ||
			  	(err = aehlog_Create("", &aehlog)) ||
			  	(err = aehlog_appendExceptionRecord(&aehlog, &aehbuf, 1))) {
			  	aehlog_close(&aehlog);
			 	return err;
			}
			aehlog_close(&aehlog);
#endif

#ifdef DEBUG_AEHEXCP
			if (aeh.assertfile)
				free(aeh.assertfile);
#endif
			return dp_RES_ALREADY;
		} else if (bPrevCall)
			return dp_RES_ALREADY;
		aeh_SetCurrent(__LINE__, __FILE__);
		bPrevCall++;
		aeh_SetCurrent(__LINE__, __FILE__);
		bPrevExc++;
		aeh_SetCurrent(__LINE__, __FILE__);
		getAppParam(&aehapp);
		aeh_SetCurrent(__LINE__, __FILE__);
		getSystemInfo(systemDesc, crshtxt, aeh_BUF_MAXLEN);
		aeh_SetCurrent(__LINE__, __FILE__);
		/* check if exception was a software-generated exception thrown by
		 * dpReportAssertionFailure; if so, get extra info on line number, etc.
		 */
		if (pException->ExceptionRecord->ExceptionCode == aeh_ASSERTION_CODE &&
			pException->ExceptionRecord->NumberParameters == 3) {
			const DWORD *array = pException->ExceptionRecord->ExceptionInformation;
			err = aeh_Create(&aeh, pException, &aehapp, systemDesc, (const int *)array[0], (const char*)array[1], (const char*)array[2]);
			bPrevCall--;
		} else
			err = aeh_Create(&aeh, pException, &aehapp, systemDesc, NULL, NULL, NULL);
		aeh_SetCurrent(__LINE__, __FILE__);
		if (err != aeh_RES_OK) {
			bPrevExc--;
			return err;
		}
		aeh_SetCurrent(__LINE__, __FILE__);
#ifdef LOG_CRASHES_TO_NET
		/* Don't write anything to disk; might be dangerous during a crash;
		 * just send over the net.
		 */
		err = sendtoserver(&aeh);
#else
		err = writetofile(&aeh);
#endif
		aeh_SetCurrent(__LINE__, __FILE__);
		aeh_Destroy(&aeh);
		bPrevExc--;
		aeh_SetCurrent(__LINE__, __FILE__);
	} __except (StupidExceptionFilter(GetExceptionInformation())) {
		return dp_RES_BUG;
	}

	return err;
}
#endif /* _WIN32 */

/*--------------------------------------------------------------------------
 Records assertion failure to logfile.
--------------------------------------------------------------------------*/
DP_API dp_result_t dpReportAssertionFailure(int lineno, char *file, char *linetxt) {
	int err = aeh_RES_OK;
#ifdef _WIN32
#ifndef LOG_CRASHES_TO_NET
	DWORD array[3];
	array[0] = (DWORD)&lineno;
	array[1] = (DWORD)file;
	array[2] = (DWORD)linetxt;
	RaiseException(aeh_ASSERTION_CODE, 0, 3, array);
#endif
#else /* _WIN32 */
	aeh_t aeh;
	aeh_appParam_t aehapp;
	char systemDesc[aeh_BUF_MAXLEN];

	aeh_SetCurrent(__LINE__, __FILE__);
	getAppParam(&aehapp);
	aeh_SetCurrent(__LINE__, __FILE__);
	getSystemInfo(systemDesc, NULL, aeh_BUF_MAXLEN);
	aeh_SetCurrent(__LINE__, __FILE__);
	err = aeh_Create(&aeh, NULL, &aehapp, systemDesc, &lineno, file, linetxt);
	aeh_SetCurrent(__LINE__, __FILE__);
	if (err != aeh_RES_OK) return err;
	aeh_SetCurrent(__LINE__, __FILE__);
	err = writetofile(&aeh);
	aeh_SetCurrent(__LINE__, __FILE__);
	aeh_Destroy(&aeh);
	aeh_SetCurrent(__LINE__, __FILE__);
#endif /* !_WIN32 */

	return err;
}
