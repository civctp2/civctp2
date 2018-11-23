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
 Debug message service.
 Controlled by .ini file; can write DPRINT messages to screen or file.

 $Log: dprint.c $
 Revision 1.14  1997/09/05 17:04:23  lbennett
 Removed L1-4 warnings from dprint.c
 Revision 1.13  1997/08/31 04:29:39  dkegel
 Include dummy version of dp_setLogFname for non-debugging dll.
 Revision 1.12  1997/08/20 22:28:06  dkegel
 Oops - got rid of possible log file thrash introduced by last change
 Revision 1.11  1997/08/20 00:38:08  anitalee
 added ifdef so windows will compile.
 Revision 1.10  1997/08/19 23:46:19  anitalee
 added include file stricmp.h
 Revision 1.9  1997/08/18 01:49:12  dkegel
 If they change the log file name after it's opened,
 dprint a warning, close the file, and let dprint reinitialize itself.
 Revision 1.8  1997/08/13 20:36:23  dkegel
 Added new call dp_flushLog() for when atomic logging is too slow.
 Revision 1.7  1997/08/11 01:26:30  dkegel
 Added dp_setLogFname() so we don't need a zillion .ini files
 when doing loopback testing.
 Revision 1.6  1997/04/29 21:40:23  dkegel
 Old names for dpini_readParameter, dpini_findSection, and dp_enableDebugPrint
 no longer supported, so switch to new names.
 Revision 1.5  1997/03/05 02:39:25  dkegel
 ini.h and dprint.h now integrated into anet.h;
 to compile or use DP as a DLL, define DP_DLL;
 win\build.bat now creates debugging and nondebugging versions
 of both the static dp.lib and the dynamic anetdll.dll.
 Revision 1.4  1997/01/31 03:51:21  dkegel
 Added Windows support.
--------------------------------------------------------------------------*/
/* Disable MSVC warnings as follows; the include files generate these when
MSVC's warning level is set to 4.
4201: nonstandard extension used : nameless struct/union
4214: nonstandard extension used : bit field types other than int
4115: named type definition in parentheses */
#if defined(WIN32)
#pragma warning( disable : 4201 4214 4115 )
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "dp.h"
#ifdef __MWERKS__
#include "stricmp.h"
#endif

#if defined(DPRNT) || defined(DEBUG) || defined(_DEBUG)

#ifdef WIN32
#include <windows.h>
#endif

/* Re-enable MSVC warnings (#@$&%#^& MS) */
/* Disable MSVC warning "unreferenced inline function has been removed" */
#if defined(WIN32)
#pragma warning( default : 4201 4214 4115 )
#pragma warning( disable : 4514 )
#endif

/*
 *	Enable and disable debug printing.
 */

#ifdef __WATCOMC__
int ylen = 8;
int ystart = 15;
int yline = 0;
int ynext = 1;
#endif

static char debugPrintingEnabled = 0;		/* Is debug printing enabled? */
static char	logfile[2 + 64 + 1] = "dp.log";
static FILE *fp = NULL;

static struct {
	unsigned int	initialized		:1;
	unsigned int	all				:1;
	unsigned int	warning			:1;
	unsigned int	onceWarning		:1;
	unsigned int	error			:1;
	unsigned int	onceError		:1;
	unsigned int	at				:1;
	unsigned int	dumpBuffer		:1;
	unsigned int	ifEnabled		:1;
	unsigned int	toNull			:1;
	unsigned int	toStdout		:1;
	unsigned int	toMono			:1;
	unsigned int	toLog			:1;
	unsigned int	toALog			:1;
	unsigned int	toALogScr		:1;
}				dprint;

/*--------------------------------------------------------------------------
 Call to flush log file.  Useful when about to crash :-)
--------------------------------------------------------------------------*/
DP_API void dp_flushLog(void)
{
	if (fp && (fp != dp_LOG_FILE_NONE))
		fflush(fp);
}

/*--------------------------------------------------------------------------
 Call this to retrieve the file pointer which dp is using for logging.
 Most programs won't need this.
 The only legal thing to do with the return value is to pass it to
 dp_setLogFP().  This is handy for context switching when using
 multiple dp_t's in the same program, as in our test bench.
--------------------------------------------------------------------------*/
DP_API FILE * DP_APIX dp_getLogFP(void)
{
	return fp;
}

/*--------------------------------------------------------------------------
 Call this to set the file pointer which dp should use for logging.
 Most programs won't need this.
 This will override any filename specified by the .ini file.
 (Note that dp_setLogFname closes the currently open log file, regardless
 of whether it was passed in via this function, or opened by dp.)
 WARNING: unless you're very sure your program is using the same C runtime
 library instance as does this code, only use the return value of
 dp_getLogFP().
--------------------------------------------------------------------------*/
DP_API void DP_APIX dp_setLogFP(FILE *_fp)
{
	fp = _fp;
	if (!_fp) {
		/* force file open on next dprint */
		dprint.initialized = 0;
	}
}

/*--------------------------------------------------------------------------
 Call to set default log filename.  (Default value is "dp.log".)
 This value may be overridden by setting a log filename in the .ini file.
 Note: no log file unless you are linked with the debugging version of
 the library, and a .ini file with (for example) All=1 and ToLog=1 lines
 in the [DEBUG] section exists (see dp.htm#inifile).
--------------------------------------------------------------------------*/
DP_API void dp_setLogFname(const char *fname)
{
	if (fp && (fp != dp_LOG_FILE_NONE)) {
		DPRINT(("dp_setLogFname: warning: log file already created, changing name to %s!\n", fname));
		dprint.initialized = 0;
		fclose(fp);
		fp = NULL;
	}
	strcpy(logfile, fname);
}

/*--------------------------------------------------------------------------
 Call with 1 to increase debug printing level (i.e. show more info);
 call with -1 to decrease debug printing level (i.e. show less info);
 call with 0 to disable all debug printing.
--------------------------------------------------------------------------*/
DP_API void dp_enableDebugPrint(int enable)
{
	switch (enable) {

	case TRUE:						/* MORE_DPRINT */
		++debugPrintingEnabled;
		break;

	case -TRUE:						/* LESS_DPRINT */
		if (debugPrintingEnabled != 0)
			--debugPrintingEnabled;
		break;

	case FALSE:						/* NO_DPRINT */
		debugPrintingEnabled = 0;
		break;

	default:
		break;
	}
}

/*
 *	Read a "[Debug]" section parameter from the .INI file.  Strip
 *	off any trailing comments.
 *
 *	Return TRUE if the parameter is true.
 */

static int
readDebugParam(
	char *	param)
{
	const char *s = dpini_readParameter(param, FALSE);

	return (   stricmp(s, "1") == 0
			|| stricmp(s, "Yes") == 0
			|| stricmp(s, "On") == 0
			|| stricmp(s, "True") == 0);
}

/*--------------------------------------------------------------------------
 Print a debug string to the destination set in the .ini file.
 See dp.htm#inifile for a detailed description of the .ini file format.
 Messages beginning with @ are only displayed if warnings are turned on
 in the .ini file.
 Returns the number of characters sent.
 Don't use dp_dprintf directly; use the DPRINT macro instead.
--------------------------------------------------------------------------*/

DP_API int cdecl
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
{
#include <stdarg.h>
    /* #ifdef	_M_ALPHA */
	va_list argptr = { NULL , 0 };
    /* #else */
    /* 	va_list argptr = NULL; */
    /* #endif */
	int		atomic = FALSE;
	int		len = 0;
	int		writeMe;					/* Write this message? */
	int		writeOnNewLine = FALSE;		/* Emit a CRLF first? */

	static int		writeAll = 0;		/* Write all messages? */
	const char *fname;

	if (__format == NULL)
		return (0);

	if (!dprint.initialized) {

		dpini_findSection("Debug");
		dprint.all = readDebugParam("All");
		dprint.warning = readDebugParam("Warning");
		dprint.onceWarning = readDebugParam("OnceWarning");
		dprint.error = readDebugParam("Error");
		dprint.onceError = readDebugParam("OnceError");
		dprint.at = readDebugParam("At");
		dprint.dumpBuffer = readDebugParam("DumpBuffer");
		dprint.ifEnabled = readDebugParam("IfEnabled");
		dprint.toNull = readDebugParam("ToNull");
		dprint.toStdout = readDebugParam("ToStdout");
		dprint.toMono = readDebugParam("ToMono");
#if 0
		if (dprint.toMono) {
		  if (monothere()) {
			mcursoff();
			mclears();
		  } else
		  	dprint.toMono = FALSE;
		}
#endif
		dprint.toLog = readDebugParam("ToLog");
		dprint.toALog = readDebugParam("ToALog");
		dprint.toALogScr = readDebugParam("ToALogScr");

		fname = dpini_readParameter("Logfile", FALSE);
		if (fname && *fname)
			dp_setLogFname(fname);

		if (dprint.toALog) {
			fp = fopen(logfile, "wt");	/* wipe out disk file on startup */
		}
		dprint.initialized = TRUE;
	}

	writeMe = writeAll;

	if (dprint.all)			/* Write all messages */
		++writeMe;

	if (dprint.warning) {	/* Write messages that start with "WARNING: " */
		if (strnicmp(__format, "WARNING: ", sizeof("WARNING: ") - 1) == 0) {
			++writeMe;
			writeOnNewLine = TRUE;
		}
	}

							/* Write all messages after the first "WARNING: " */
	if (dprint.onceWarning) {
		if (strnicmp(__format, "WARNING: ", sizeof("WARNING: ") - 1) == 0) {
			++writeMe;
			++writeAll;
			writeOnNewLine = TRUE;
		}
	}

	if (dprint.error) {		/* Write messages that start with "ERROR: " */
		if (strnicmp(__format, "ERROR: ", sizeof("ERROR: ") - 1) == 0) {
			++writeMe;
			writeOnNewLine = TRUE;
		}
	}

	if (dprint.onceError) {	/* Write all messages after the first "ERROR: " */
		if (strnicmp(__format, "ERROR: ", sizeof("ERROR: ") - 1) == 0) {
			++writeMe;
			++writeAll;
			writeOnNewLine = TRUE;
		}
	}

	switch (*__format++) {

	case '@':
		if (dprint.at)	/* Write messages that start with the '@' character */
			++writeMe;
		break;

	case '!':
						/* Write all DUMP_BUFFER and DUMP_COM_PACKET calls */
		if (dprint.dumpBuffer)
			writeMe = ++writeAll;
		break;

	case '\\':
						/* Write all DUMP_BUFFER and DUMP_COM_PACKET calls */
		if (dprint.dumpBuffer)
			writeMe = writeAll--;
		break;

	default:
		--__format;
		break;
	}

						/* Write messages while debugPrintingEnabled is true */
	if (dprint.ifEnabled && debugPrintingEnabled)
		++writeMe;

	if (!writeMe)
		return (0);

	if (dprint.toNull) {	/* Do nothing */
		;
	}

							/* Write to standard output (usually the screen) */
	else if (dprint.toStdout) {
		if (!fp)
			fp = stdout;
	}

							/* Write to the monochrome monitor, if there is one */
	else if (dprint.toMono) {
		char	monobuf[256];

		va_start(argptr, __format);
		len = vsprintf(monobuf, __format, argptr);
		va_end(argptr);
#ifdef __WATCOMC__
#if 0
		mputs("                                                    ",
		  0, yline+ystart, WHITE_TEXT | BLACK_BACK);
		mputs(monobuf, 0, yline+ystart, WHITE_TEXT | BLACK_BACK);
		mputs("__                                                  ",
		 0, ynext+ystart, WHITE_TEXT | BLACK_BACK);
#endif
		yline = (yline + 1) % ylen;
		ynext = (yline + 1) % ylen;
#endif
#ifdef WIN32
		OutputDebugString(monobuf);
#endif
	}

							/* Write to log file (may lose data if we crash) */
	else if (dprint.toLog) {
		if (fp == NULL)
			fp = fopen(logfile, "wt");
	}

							/* Atomic write to log file (no data loss if crash) */
	else if (dprint.toALog) {
						/* MAKE SURE DISK CACHING IS DISABLED ON THAT DRIVE */
		if (!fp)
			fp = fopen(logfile, "at");
		atomic = TRUE;
	}

							/* Atomic write file (crash-proof); errs to screen */
	else if (dprint.toALogScr) {
						/* MAKE SURE DISK CACHING IS DISABLED ON THAT DRIVE */
		if (!fp)
			fp = fopen(logfile, (fp == NULL) ? "wt" : "at");
		atomic = TRUE;

		if (strnicmp(__format, "ERROR: ", sizeof("ERROR: ") - 1) == 0)
			vprintf(__format, argptr);
	}

	if ((fp != NULL) && (fp != dp_LOG_FILE_NONE)) {
		va_start(argptr, __format);
		if (writeOnNewLine)
			(void) fprintf(fp, "\n");
		len = vfprintf(fp, __format, argptr);

		if (atomic) {
			fclose(fp);
			fp = NULL;
		} else
			fflush(fp);

		va_end(argptr);
	}

	return (len);
}

#else

/* Needed to export to DLL's in shipping version */
DP_API void dp_flushLog(void)
{
	;
}

DP_API void dp_enableDebugPrint(int	enable)
{
    (void) enable;
}

DP_API void dp_setLogFname(const char *fname)
{
	(void) fname;
}

DP_API int cdecl
dp_dprintf(
	const char *	__format,	/* printf-style format (or NULL) */
	...)						/* printf-style arguments on stack (if any) */
{
    return 0;
}

DP_API FILE * DP_APIX dp_getLogFP(void)
{
	return NULL;
}

DP_API void DP_APIX dp_setLogFP(FILE *_fp)
{
	return;
}

#endif
