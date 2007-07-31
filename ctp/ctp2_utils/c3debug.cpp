











#include "c3.h"

#ifdef _DEBUG

#include "aui.h"
#include "debugWindow.h"
#include "civ3_main.h"
#include "netconsole.h"


#ifndef _NO_GAME_WATCH

#include "GameWatch.h"


extern int g_gameWatchID;
#endif 

uint32 g_debug_mask = k_DBG_NONE;
static int g_useMask;

char g_last_debug_text[4096];

#ifndef _AIDLL
extern DebugWindow *g_debugWindow;
#endif

#define k_FILENAME				"logs\\civ3log%#.2d.txt"
#define k_MAX_LOG_FILE_LINES	10000		

MBCHAR	s_logFileName[20];
sint32	s_logFileNumber=0;
sint32	s_logLinesThisFile=0;

MBCHAR *c3debug_GetLogFileName(void)
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

int c3debug_InitDebugLog()
{
	
	SECURITY_ATTRIBUTES		sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	
	CreateDirectory((LPCTSTR)"logs", &sa);

	
	WIN32_FIND_DATA	fileData;
	HANDLE lpFileList;
	MBCHAR path[_MAX_PATH];

	strcpy(path, "logs\\*.*");
		
	
	lpFileList = FindFirstFile(path, &fileData);
	
	if (lpFileList != INVALID_HANDLE_VALUE) {
		
		MBCHAR fileName[256];
		do {
			sprintf(fileName, "logs\\%s", fileData.cFileName);
			DeleteFile(fileName);
		} while(FindNextFile(lpFileList,&fileData));

		FindClose(lpFileList);
	}











	s_logFileNumber = 0;
	s_logLinesThisFile = 0;

	sprintf(s_logFileName, k_FILENAME, s_logFileNumber);

	FILE* f = fopen(s_logFileName, "w");
	Assert(f);
	fclose(f);
	return 0;
}

int
c3debug_dprintfPrefix(int mask, 
			  char* file, 
			  int line) 
{
	g_useMask = mask;
	char *filename;

	if(mask & g_debug_mask) {
		FILE* f = fopen(s_logFileName, "a");
		
		
		if (!f) return 0;

		if (s_logLinesThisFile >=  k_MAX_LOG_FILE_LINES) {
			s_logFileNumber++;
			s_logLinesThisFile = 0;
			sprintf(s_logFileName, k_FILENAME, s_logFileNumber);
			fclose(f);
			f = fopen(s_logFileName, "w");
			fprintf(f, "[Continued from Part %#.2d]\n\n", s_logFileNumber-1);
		}

		filename = strrchr(file, '\\');
		if(!filename)
			filename = file;
		else
			filename++;
		
		fprintf(f, "%15.15s@%-4d: ", filename, line);
		
		s_logLinesThisFile++;

		fclose(f);
		
		g_last_debug_text[0] = 0;
	}
	return 0;
}

int
c3debug_dprintf(char* format, 
		...) 
{
	va_list list;
	if(g_debug_mask & g_useMask) {
		FILE* f = fopen(s_logFileName, "a");
		if(f) {
			if (s_logLinesThisFile >=  k_MAX_LOG_FILE_LINES) {
				s_logFileNumber++;
				s_logLinesThisFile = 0;
				sprintf(s_logFileName, k_FILENAME, s_logFileNumber);
				fclose(f);
				f = fopen(s_logFileName, "w");
				fprintf(f, "[Continued from Part %#.2d]\n\n", s_logFileNumber-1);
			}
			va_start(list, format);
			if(g_netConsole) {
				g_netConsole->Print(format, list);
			}
			va_end(list);

			va_start(list, format);
			vfprintf(f, format, list);
			va_end(list);

			s_logLinesThisFile++;

			fclose(f);
		}
		
		va_start(list, format);
		vsprintf(g_last_debug_text + strlen(g_last_debug_text), format, list);
		va_end(list);
#ifndef _AIDLL
		
		if (g_debugWindow) {
			g_debugWindow->AddText(g_last_debug_text);
		}
#endif
	}
	return 0;
}

void
c3debug_SetDebugMask(int mask, int set)
{
	if(set) {
		g_debug_mask |= mask;
	} else {
		g_debug_mask &= ~(mask);
	}
}


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

	
	
#ifndef _NO_GAME_WATCH
	
	
	char userName[256];
	DWORD size = 256;
	userName[0] = '\0';		
	GetUserName(userName, &size);

	
	char computerName[256];
	size = 256;
	computerName[0] = '\0';		
	GetComputerName(computerName, &size);

	
	SYSTEMTIME localTime;
	memset(&localTime, 0, sizeof(localTime));	
	GetLocalTime(&localTime);

	
	char stamp[1024];
	sprintf(stamp, "Civilization III CTP - %s on %s at %d/%d/%d %d:%d:%d", userName, computerName,
		localTime.wMonth, localTime.wDay, localTime.wYear, localTime.wHour,
		localTime.wMinute, localTime.wSecond);

	
	gameWatch.EndGame(g_gameWatchID, stamp);
#endif 

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

void c3debug_Assert(char *s, char *file, int line)
{
	DPRINTF(k_DBG_FIX, ("Assertion (%s) Failed in File:%s, Line:%ld\n", s, file, line)); 
 
	MBCHAR *traceStr = c3debug_StackTrace();
	DPRINTF(k_DBG_FIX, ("Stack Trace: '%s'\n", traceStr));

    do { 


 		if (_CrtDbgReport(_CRT_ASSERT, file, line, NULL, s) == 1) _CrtDbgBreak(); 

	} while (0);
}	

#endif 
