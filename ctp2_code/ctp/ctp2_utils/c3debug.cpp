//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Debugging
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// _DEBUG           (automatically set when choosing the Debug configuration)
// USE_LOGGING      Enable logging for the release/final version.
//                  The debug version has logging enabled always.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - The log files are now only opened and closed once, this speeds up
//   debugging significantly. (09-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3debug.h"

#if defined(_DEBUG) || defined(USE_LOGGING)

#include <sys/types.h>
#if !defined(WIN32)
#include <dirent.h>
#include <unistd.h>
#include <csignal>
#endif

#include "aui.h"
#include "debugwindow.h"
#include "civ3_main.h"
#include "netconsole.h"
#include "c3files.h"

extern BOOL g_noAssertDialogs;
uint32 g_debug_mask = k_DBG_NONE;
static int g_useMask;

char g_last_debug_text[4096];

#ifndef _AIDLL
extern DebugWindow *g_debugWindow;
#endif

#define k_FILENAME            "logs" FILE_SEP "civ3log%#.3d.txt"
#define k_MAX_LOG_FILE_LINES  40000

MBCHAR	s_logFileName[20];
sint32	s_logFileNumber=0;
sint32	s_logLinesThisFile=0;

FILE* g_theLogFile = NULL;

#if 0
MBCHAR const * c3debug_GetLogFileName(void)
{
	return s_logFileName;
}

sint32 *c3debug_GetLogFileNumber(void)
{
	return &s_logFileNumber;
}

sint32 *c3debug_GetLogLinesThisFile(void)
{
	return &s_logLinesThisFile;
}
#endif

void c3debug_InitDebugLog()
{
	c3files_CreateDirectory("logs");

#if defined(WIN32)
	WIN32_FIND_DATA fileData;
	MBCHAR path[_MAX_PATH];

	strcpy(path, "logs" FILE_SEP "*.*");

	HANDLE lpFileList = FindFirstFile(path, &fileData);

	if(lpFileList != INVALID_HANDLE_VALUE)
	{
		MBCHAR fileName[256];
		do
		{
			sprintf(fileName, "logs%s%s", FILE_SEP, fileData.cFileName);
			DeleteFile(fileName);
		} while(FindNextFile(lpFileList,&fileData));

		FindClose(lpFileList);
	}
#else
	DIR *dir = opendir("logs");
	Assert(dir);
	struct dirent *dent = NULL;
	MBCHAR fileName[256];

	while((dent = readdir(dir)))
	{
		int unlinkRetVal;
		if(strcmp(dent->d_name, "." ) != 0
		&& strstr(dent->d_name, ".map") == NULL // do not delete already created k_MAP_FILE
		&& strcmp(dent->d_name, "..") != 0)
		{
			sprintf(fileName, "logs%s%s", FILE_SEP, dent->d_name);
			unlinkRetVal = unlink(fileName);
			Assert(unlinkRetVal == 0);
		}
	}

	closedir(dir);
#endif

	s_logFileNumber = 0;
	s_logLinesThisFile = 0;

	sprintf(s_logFileName, k_FILENAME, s_logFileNumber);

	g_theLogFile = fopen(s_logFileName, "w");
	Assert(g_theLogFile);
}

void c3debug_CloseDebugLog()
{
	fclose(g_theLogFile);
}

void c3debug_dprintfPrefix
(
    sint32      mask,
    char const* file,
    sint32      line
)
{
	g_useMask = mask;

	if (mask & g_debug_mask)
	{
		if(g_theLogFile)
		{
			if (s_logLinesThisFile >=  k_MAX_LOG_FILE_LINES)
			{
				s_logFileNumber++;
				s_logLinesThisFile = 0;
				sprintf(s_logFileName, k_FILENAME, s_logFileNumber);
				fclose(g_theLogFile);
				g_theLogFile = fopen(s_logFileName, "a");
				fprintf(g_theLogFile, "[Continued from Part %#.3d]\n\n", s_logFileNumber-1);
			}

			char const * filename = strrchr(file, FILE_SEPC);
			if(filename)
			{
				filename++;
			}
			else
			{
				filename = file;
			}

			fprintf(g_theLogFile, "%15.15s@%-4d: ", filename, line);

			s_logLinesThisFile++;

			g_last_debug_text[0] = 0;
		}
	}
}

void c3debug_dprintf(char const * format, ...)
{
	va_list list;
	if(g_debug_mask & g_useMask)
	{
		if(g_theLogFile)
		{
			if (s_logLinesThisFile >=  k_MAX_LOG_FILE_LINES)
			{
				s_logFileNumber++;
				s_logLinesThisFile = 0;
				sprintf(s_logFileName, k_FILENAME, s_logFileNumber);
				fclose(g_theLogFile);
				g_theLogFile = fopen(s_logFileName, "a");
				fprintf(g_theLogFile, "[Continued from Part %#.3d]\n\n", s_logFileNumber-1);
			}

			va_start(list, format);

			if(g_netConsole)
			{
				g_netConsole->Print(format, list);
			}

			va_end(list);

			va_start(list, format);
			vfprintf(g_theLogFile, format, list);
			va_end(list);

			s_logLinesThisFile++;

			fflush(g_theLogFile);
		}

		va_start(list, format);
		vsprintf(g_last_debug_text + strlen(g_last_debug_text), format, list);
		va_end(list);
#ifndef _AIDLL

		if (g_debugWindow)
		{
			g_debugWindow->AddText(g_last_debug_text);
		}
#endif
	}
}

void c3debug_SetDebugMask(int mask, int set)
{
	if(set)
	{
		g_debug_mask |= mask;
	}
	else
	{
		g_debug_mask &= ~(mask);
	}
}

#ifdef WIN32
static LONG _cdecl c3debug_CivExceptionHandler (LPEXCEPTION_POINTERS exception_pointers)
{
	MBCHAR *s;

	switch (exception_pointers->ExceptionRecord->ExceptionCode) {
	case EXCEPTION_ACCESS_VIOLATION:		s = "Access Violation";		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:	s = "Array Bounds Exceeded"; break;
	case EXCEPTION_BREAKPOINT:				s = "Breakpoint"; break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:	s = "Datatype Misalignment"; break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:	s = "Floating Point Denormal Operand"; break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:		s = "Floating Point Divide by Zero"; break;
	case EXCEPTION_FLT_INEXACT_RESULT:		s = "Floating Point Inexact Result"; break;
	case EXCEPTION_FLT_INVALID_OPERATION:	s = "Floating Point Invalid Operation"; break;
	case EXCEPTION_FLT_OVERFLOW:			s = "Floating Point Overflow"; break;
	case EXCEPTION_FLT_STACK_CHECK:			s = "Floating Point Stack Check"; break;
	case EXCEPTION_FLT_UNDERFLOW:			s = "Floating Point Underflow"; break;
	case EXCEPTION_GUARD_PAGE:				s = "Guard Page"; break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:		s = "Illegal Instruction"; break;
	case EXCEPTION_IN_PAGE_ERROR:			s = "In-page Error"; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:		s = "Integer Divide By Zero"; break;
	case EXCEPTION_INT_OVERFLOW:			s = "Integer Overflow"; break;
	case EXCEPTION_INVALID_DISPOSITION:		s = "Invalid Disposition"; break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:s = "Non-Continuable Exception"; break;
	case EXCEPTION_PRIV_INSTRUCTION:		s = "Privileged Instruction"; break;
	case EXCEPTION_SINGLE_STEP:				s = "Single Step"; break;
	case EXCEPTION_STACK_OVERFLOW:			s = "Stack Overflow"; break;
	default:
		s = "Unknown";
	}

	DPRINTF(k_DBG_FIX, ("Exception: '%s' thrown.\n", s));

	s = c3debug_ExceptionStackTrace(exception_pointers);

	DPRINTF(k_DBG_FIX, ("Exception Stack Trace:\n%s\n", s));

	return EXCEPTION_CONTINUE_SEARCH;
}

void c3debug_ExceptionExecute(CivExceptionFunction function)
{
	__try
	{
		function();
	}
	__except (c3debug_CivExceptionHandler(GetExceptionInformation()))
	{
		DoFinalCleanup();
	}
}
#endif // WIN32

void c3debug_Assert(char const *s, char const * file, int line)
{
	DPRINTF(k_DBG_FIX, ("Assertion (%s) Failed in File:%s, Line:%ld\n", s, file, line));
	DPRINTF(k_DBG_FIX, ("Stack Trace: '%s'\n", c3debug_StackTrace()));

#if defined(_DEBUG)
#if defined(WIN32)
	do
	{
		if (_CrtDbgReport(_CRT_ASSERT, file, line, NULL, s) == 1)
		{
			_CrtDbgBreak();
		}
	}
	while (0);
#else
	MBCHAR str[1024];
	sprintf(str, "Assertion (%s) Failed in File:%s, Line:%ld\n", s, file, line);
	fprintf(stderr, str);
	sint32 result = g_noAssertDialogs ? IDIGNORE
		: MessageBox(NULL, str, "Assert", MB_ABORTRETRYIGNORE | MB_ICONEXCLAMATION);
	
	if(result == IDRETRY)
	{
		std::raise(SIGINT);
	}
	else if(result == IDABORT)
	{
		exit(-1);
	}
#endif
#endif
}

#endif
