


#include "c3.h"
#include "c3ai.h"

#ifdef _DEBUG

#include <imagehlp.h>
#include <winerror.h>

#define  sizeof_Name		128
#define  sizeof_CONTEXT		sizeof(CONTEXT)+96
#define  sizeof_STACKFRAME	sizeof(STACKFRAME)+16
#define  sizeof_symbol		sizeof(IMAGEHLP_SYMBOL)+sizeof_Name


#define k_STACK_TRACE_LEN 32768
static MBCHAR	s_stackTraceString[k_STACK_TRACE_LEN];
static MBCHAR	s_str[k_STACK_TRACE_LEN];




#ifdef _TEST
	#define k_EXE_NAME		"CivCTP_test.exe"
	#define k_DLL_NAME		"TestRobotCOM.dll"
	#define k_EXE_PATH		"Test"
	#define k_DLL_PATH		"dll\\ai"
#else
	#define k_EXE_NAME		"CivCTP_dbg.exe"
	#define k_DLL_NAME		"DBGRobotCOM.dll"
	#define k_EXE_PATH		"Debug"
	#define k_DLL_PATH		"dll\\ai"
#endif _TEST

char * c3ai_StackTrace(void)
{
	HANDLE			hProc, hThread;
	CONTEXT			*cxt;
	IMAGEHLP_SYMBOL	*sym;
	STACKFRAME		*frm;
	DWORD			machType, symDisp, lastErr, filepathlen;
	BOOL			stat;
	int				i;
	char			filepath[MAX_PATH], *lastdir, *pPath;
	MBCHAR			fileName[_MAX_PATH];
	MBCHAR			tempStr[_MAX_PATH];
	
	
	
	
	
	
	s_stackTraceString[0] = 0;

	filepathlen = GetModuleFileName ( NULL, filepath, sizeof(filepath));
	if (filepathlen == 0)
		sprintf(s_str, "StackTrace: Failed to get pathname for program\n");
		c3ai_Log(s_str);
	if (strlen(filepath) > 0)
		strcpy(fileName, filepath);

	
	
	lastdir = strrchr (filepath, '/');
	if (lastdir == NULL)
		lastdir = strrchr (filepath, '\\');
	if (lastdir != NULL)
		lastdir[0] = '\0';
	
	
	pPath = filepath;
	if (strlen (filepath) == 0)
		pPath = NULL;
	
	hProc   = GetCurrentProcess ();
	hThread = GetCurrentThread ();

	int err;

	HANDLE	image;
	MBCHAR	imagePath[_MAX_PATH];

	MBCHAR	searchPath[_MAX_PATH];

	
	sprintf(searchPath, "%s;%s\\%s;%s\\%s", filepath,
			filepath, k_DLL_PATH, 
			filepath, k_EXE_PATH);

	if (!SymInitialize(hProc, searchPath, FALSE)) {
		err = GetLastError();
	    sprintf(s_str, "SymInitialize failed with error %d", err);
		c3ai_Log(s_str);
	} else {
		

		
		image = FindExecutableImage(k_EXE_NAME, searchPath, imagePath);

		if (!SymLoadModule(hProc, image, imagePath, NULL, NULL, NULL)) {
			err = GetLastError();
			sprintf(s_str, "SymLoadModule failed with error %d", err);
			c3ai_Log(s_str);
		}

		
		image = FindExecutableImage(k_DLL_NAME, searchPath, imagePath);

		if (!SymLoadModule(hProc, image, imagePath, NULL, NULL, NULL)) {
			err = GetLastError();
			sprintf(s_str, "SymLoadModule failed with error %d", err);
			c3ai_Log(s_str);
		}
	}

	
	frm = (STACKFRAME *)malloc (sizeof_STACKFRAME);
	memset (frm, 0, sizeof(STACKFRAME));
	
	sym = (IMAGEHLP_SYMBOL *)malloc (sizeof_symbol);
	memset (sym, 0, sizeof_symbol);
	sym->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
	sym->MaxNameLength = sizeof_Name-1;

	
    machType = IMAGE_FILE_MACHINE_I386; 
	
	
	cxt = NULL;
	
	
    frm->AddrPC.Mode         = AddrModeFlat; 
    frm->AddrStack.Mode      = AddrModeFlat; 
    frm->AddrFrame.Mode      = AddrModeFlat;
	
	
	
	
	
	
	
	
	_asm mov  i, esp					
		frm->AddrStack.Offset    = i; 
	
	_asm mov  i, ebp					
		frm->AddrFrame.Offset    = i;
	
	
	
	
	
	
	
	
	frm->AddrPC.Offset       = ((DWORD) &c3ai_StackTrace) + 0x08c;	
	
	
	
	
	
	
	if ( !StackWalk( machType, hProc, hThread, frm, cxt, NULL, 
					SymFunctionTableAccess, SymGetModuleBase, NULL ) ) {
		printf ("StackTrace: Failed to walk current stack call\n");
	}
	
	
	
	if ( !SymGetSymFromAddr ( hProc, frm->AddrPC.Offset, &symDisp, sym ) ) {
		sprintf(tempStr, "(0x%08x[?]) /\n", frm->AddrPC.Offset);
	} else {
		sprintf(tempStr, "(0x%08x[%s+%d]) /\n", frm->AddrPC.Offset, sym->Name, symDisp);
	}

	
	
	
	for (i=0; i<100; i++) {
		
		stat = StackWalk( machType, hProc, hThread, frm, cxt, NULL, 
							SymFunctionTableAccess, SymGetModuleBase, NULL );
		if ( !stat ) {
			lastErr = GetLastError ();
			if (lastErr != ERROR_NOACCESS && lastErr != ERROR_INVALID_ADDRESS) {
				sprintf(s_str, "<stack walk terminated with error %d>\n", lastErr);
				c3ai_Log(s_str);
			}
			break;
		}
		
		
		if ( frm->AddrPC.Offset != 0 ) {
		
			
			if ( SymGetSymFromAddr ( hProc, frm->AddrPC.Offset, &symDisp, sym ) ) {
				sprintf(tempStr, "(0x%08x[%s+%d]) /\n", frm->AddrPC.Offset, sym->Name, symDisp);
				if(strlen(s_stackTraceString) + strlen(tempStr) < k_STACK_TRACE_LEN - 1 )
					strcat(s_stackTraceString, tempStr);
			}
			else {
				lastErr = GetLastError ();
				if (lastErr == ERROR_INVALID_ADDRESS) {		
					sprintf(tempStr, "(0x%08x[?]) /\n", frm->AddrPC.Offset);
					if(strlen(s_stackTraceString) + strlen(tempStr) < k_STACK_TRACE_LEN - 1 )
						strcat(s_stackTraceString, tempStr);
				} else {
					sprintf(s_str,"0x%08x   <no symbol available - error %d>\n", frm->AddrPC.Offset, lastErr);
					c3ai_Log(s_str);
				}
			}
		}
		
	}
	
	if (i >= 100) {
		sprintf(s_str, "StackTrace: <traceback terminated after 100 routines>\n");
		c3ai_Log(s_str);
	}

	if ( !SymCleanup (hProc) ){
		sprintf(s_str, "StackTrace: failed to cleanup symbols\n");
		c3ai_Log(s_str);
	}

	free (cxt);		
	free (frm);
	free (sym);	

	return s_stackTraceString;
}

char * c3ai_ExceptionStackTrace(LPEXCEPTION_POINTERS exception)
{
	HANDLE			hProc, hThread;
	CONTEXT			*cxt;
	IMAGEHLP_SYMBOL	*sym;
	STACKFRAME		*frm;
	DWORD			machType, symDisp, lastErr, filepathlen;
	BOOL			stat;
	int				i;
	char			filepath[MAX_PATH], *lastdir, *pPath;
	MBCHAR			fileName[_MAX_PATH];
	MBCHAR			tempStr[_MAX_PATH];
	
	
	
	
	
	
	s_stackTraceString[0] = 0;

	filepathlen = GetModuleFileName ( NULL, filepath, sizeof(filepath));
	if (filepathlen == 0) {
		sprintf(s_str, "StackTrace: Failed to get pathname for program\n");
		c3ai_Log(s_str);
	}

	if (strlen(filepath) > 0)
		strcpy(fileName, filepath);

	
	
	lastdir = strrchr (filepath, '/');
	if (lastdir == NULL)
		lastdir = strrchr (filepath, '\\');
	if (lastdir != NULL)
		lastdir[0] = '\0';
	
	
	pPath = filepath;
	if (strlen (filepath) == 0)
		pPath = NULL;
	
	hProc   = GetCurrentProcess ();
	hThread = GetCurrentThread ();

	int err;

	HANDLE	image;
	MBCHAR	imagePath[_MAX_PATH];

	MBCHAR	searchPath[_MAX_PATH];

	
	sprintf(searchPath, "%s;%s\\%s;%s\\%s", filepath,
			filepath, k_DLL_PATH, 
			filepath, k_EXE_PATH);

	if (!SymInitialize(hProc, searchPath, FALSE)) {
		err = GetLastError();
	    sprintf(s_str, "SymInitialize failed with error %d", err);
		c3ai_Log(s_str);
	} else {
		

		
		image = FindExecutableImage(k_EXE_NAME, searchPath, imagePath);

		if (!SymLoadModule(hProc, image, imagePath, NULL, NULL, NULL)) {
			err = GetLastError();
			sprintf(s_str, "SymLoadModule failed with error %d", err);
			c3ai_Log(s_str);
		}

		
		image = FindExecutableImage(k_DLL_NAME, searchPath, imagePath);

		if (!SymLoadModule(hProc, image, imagePath, NULL, NULL, NULL)) {
			err = GetLastError();
			sprintf(s_str, "SymLoadModule failed with error %d", err);
			c3ai_Log(s_str);
		}
	}


	
	frm = (STACKFRAME *)malloc (sizeof_STACKFRAME);
	memset (frm, 0, sizeof(STACKFRAME));
	
	sym = (IMAGEHLP_SYMBOL *)malloc (sizeof_symbol);
	memset (sym, 0, sizeof_symbol);
	sym->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
	sym->MaxNameLength = sizeof_Name-1;

	
    machType = IMAGE_FILE_MACHINE_I386; 
	
	
	cxt = NULL;
	
	
    frm->AddrPC.Mode         = AddrModeFlat; 
    frm->AddrStack.Mode      = AddrModeFlat; 
    frm->AddrFrame.Mode      = AddrModeFlat;
	
	
	
	
	
	
	
	frm->AddrStack.Offset = exception->ContextRecord->Esp;
	frm->AddrFrame.Offset = exception->ContextRecord->Ebp;
	frm->AddrPC.Offset = exception->ContextRecord->Eip;

	
	
	
	
	
	if ( !StackWalk( machType, hProc, hThread, frm, cxt,
		NULL, SymFunctionTableAccess, SymGetModuleBase, NULL ) ) {
		printf ("StackTrace: Failed to walk current stack call\n");
	}
	
	
	if ( !SymGetSymFromAddr ( hProc, frm->AddrPC.Offset, &symDisp, sym ) ) {
		sprintf(tempStr, "(0x%08x[?]) /\n", frm->AddrPC.Offset);
		strcat(s_stackTraceString, tempStr);
	} else {
		sprintf(tempStr, "(0x%08x[%s+%d]) /\n", frm->AddrPC.Offset, sym->Name, symDisp);
		strcat(s_stackTraceString, tempStr);
	}

	
	
	
	for (i=0; i<100; i++) {
		
		stat = StackWalk( machType, hProc, hThread, frm, cxt, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL );
		if ( !stat ) {
			lastErr = GetLastError ();
			if (lastErr != ERROR_NOACCESS && lastErr != ERROR_INVALID_ADDRESS) {
				sprintf(s_str, "<stack walk terminated with error %d>\n", lastErr);
				c3ai_Log(s_str);
			}
			break;
		}
		
		
		if ( frm->AddrPC.Offset != 0 ) {
		
			
			if ( SymGetSymFromAddr ( hProc, frm->AddrPC.Offset, &symDisp, sym ) ) {
				sprintf(tempStr, "(0x%08x[%s+%d]) /\n", frm->AddrPC.Offset, sym->Name, symDisp);
				if(strlen(s_stackTraceString) + strlen(tempStr) < k_STACK_TRACE_LEN - 1 )
					strcat(s_stackTraceString, tempStr);
			}
			else {
				lastErr = GetLastError ();
				if (lastErr == ERROR_INVALID_ADDRESS) {		
					sprintf(tempStr, "(0x%08x[?]) /\n", frm->AddrPC.Offset);
					if(strlen(s_stackTraceString) + strlen(tempStr) < k_STACK_TRACE_LEN - 1 )
						strcat(s_stackTraceString, tempStr);
				} else {
					sprintf(s_str, "0x%08x   <no symbol available - error %d>\n", frm->AddrPC.Offset, lastErr);
					c3ai_Log(s_str);
				}
			}
		}
	}
	
	if (i >= 100) {
		sprintf(s_str, "StackTrace: <traceback terminated after 100 routines>\n");
		c3ai_Log(s_str);
	}

	if ( !SymCleanup (hProc) ){
		sprintf(s_str, "StackTrace: failed to cleanup symbols\n");
		c3ai_Log(s_str);
	}

	free (cxt);		
	free (frm);
	free (sym);	

	return s_stackTraceString;
}

LONG _cdecl c3ai_CivExceptionHandler (LPEXCEPTION_POINTERS exception_pointers)
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

	sprintf(s_str, "Exception: '%s' thrown.\n", s);
	c3ai_Log(s_str);

	
	s = c3ai_ExceptionStackTrace(exception_pointers);

	sprintf(s_str, "Exception Stack Trace:\n%s\n", s);
	c3ai_Log(s_str);

	return EXCEPTION_CONTINUE_SEARCH;
}

void c3ai_ExceptionExecute(CivExceptionFunction function)
{
	
	__try
	{
		function();
	}

	
	__except (c3ai_CivExceptionHandler(GetExceptionInformation()))
	{
	}
}

#define k_FILENAME				"logs\\civ3log%#.2d.txt"
#define k_MAX_LOG_FILE_LINES	10000		

MBCHAR	*s_logFileName;
sint32	*s_logFileNumber;
sint32	*s_logLinesThisFile;

void c3aidebug_SetCiv3LogName(MBCHAR *civ3logName)
{
	s_logFileName = civ3logName;
}

void c3aidebug_SetCiv3LogNumber(sint32 *civ3logNumber)
{
	s_logFileNumber = civ3logNumber;
}

void c3aidebug_SetCiv3LogLinesThisFile(sint32 *civ3logLinesThisFile)
{
	s_logLinesThisFile = civ3logLinesThisFile;
}

void c3ai_Log(char *s)
{
	FILE* f = fopen(s_logFileName, "a");
	
	
	if (!f) return;

	if ((*s_logLinesThisFile) >=  k_MAX_LOG_FILE_LINES) {
		(*s_logFileNumber)++;
		(*s_logLinesThisFile) = 0;
		sprintf(s_logFileName, k_FILENAME, *s_logFileNumber);
		fclose(f);
		f = fopen(s_logFileName, "w");
		fprintf(f, "[Continued from Part %#.2d]\n\n", (*s_logFileNumber)-1);
	}

	fprintf(f, "%s", s);
	
	(*s_logLinesThisFile)++;

	fclose(f);
}

void c3ai_Assert(char *s, char *file, int line)
{
#ifdef _DEBUG
	sprintf(s_str, "AI: Assertion (%s) Failed in File:%s, Line:%ld\n", s, file, line);
	c3ai_Log(s_str);

	MBCHAR *traceStr = c3ai_StackTrace();
	sprintf(s_str, "AI: Stack Trace: '%s'\n", traceStr);
	c3ai_Log(s_str);
    do { 
		if (_CrtDbgReport(_CRT_ASSERT, file, line, NULL, s) == 1) _CrtDbgBreak(); 
	} while (0);
#endif
}

#endif 