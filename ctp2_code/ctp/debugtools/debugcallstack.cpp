//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Stack class structure implementation for debugging stack.
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
// _DEBUG
// - Generate debug version
//
// _BFR_
// - ??
//
// REQUIRE_CORRECT_LOG
// - ??
//
// GENERATE_ADDRESS_LOG
// - ??
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fix unreferenced warnings in regular compilation by placing intialization
//   of buff and err inside #ifndef statements.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added alternative show leaks function. (Sep 15th 2005 Martin Gühmann)
//
//////////////////////////////////////////////////////////////////////////////

#include "c3.h"
#include "debugcallstack.h"

#include "log.h"
#include "c3debug.h"
#ifdef WIN32
#include <windows.h>
#else
#include "windows.h"
#endif
#include <string.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#ifdef WIN32
#include <imagehlp.h>
#endif

#ifndef _DEBUG
#include "log_off.h"
#endif

#ifdef __linux__
#include <execinfo.h>
#include <dlfcn.h>
#include "civ3_main.h"
#include "../cifm.h"
#define fopen(a, b) ci_fopen(a, b)
#endif

static bool debug_dump_whole_stack = false;

void Debug_FunctionNameFindAddresses(FILE *fp);
int Debug_FunctionNameOpen (char *map_file_name);
void Debug_FunctionNameClose (void);
const char *Debug_FunctionNameGet (size_t address);
void Debug_MakeRoom(size_t capacity);
void Debug_SortFunctions();

#ifdef WIN32
#define k_MAP_FILE "ctp2.map"
#else // WIN32
#define k_MAP_FILE "ctp2linux.map"
#endif // WIN32

#if defined(WIN32) && defined(_X86_)
#define DEBUG_CODE_LIMIT 0x80000000
#else
#define DEBUG_CODE_LIMIT 0x8000000000000000
#endif
#define BUFFER_SIZE      (2000)

#define k_STACK_TRACE_LEN 32768

const int k_CALL_STACK_SIZE = 29;

static MBCHAR s_stackTraceString[k_STACK_TRACE_LEN];

void DebugCallStack_Open (void)
{
#if defined(WIN32)
	TCHAR exepath[MAX_PATH + 1];
	if (GetModuleFileName(0, exepath, MAX_PATH + 1) > 0)
	{
		char* name_map = strdup(exepath);
		char *p = strrchr(name_map, '.');
		if (p)
		{
			strcpy(p, ".map"); // Change to map file
			Debug_FunctionNameOpen(name_map);
		}
		else
			Debug_FunctionNameOpen(k_MAP_FILE);
	}
	else
		Debug_FunctionNameOpen(k_MAP_FILE);

#else
	Debug_FunctionNameOpen(k_MAP_FILE);
#endif
}

void DebugCallStack_Close (void)
{
	Debug_FunctionNameClose();
}

#if defined(WIN32)

static LONG _cdecl MemoryAccessExceptionFilter (LPEXCEPTION_POINTERS ep)
{
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		return (EXCEPTION_EXECUTE_HANDLER);

	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
		return (EXCEPTION_EXECUTE_HANDLER);

	return (EXCEPTION_CONTINUE_SEARCH);
}

#endif

static const char *unknown = "(unknown)";
static const char *kernel  = "(kernel)";
static int function_name_open = 0;

typedef struct tagFUNCTION_ADDRESS
{
	size_t address;
	char *name;
} FUNCTION_ADDRESS;

static std::vector<FUNCTION_ADDRESS> functionAdresses;

// Comparator function
bool funcComparator(FUNCTION_ADDRESS a, FUNCTION_ADDRESS b) {
	return a.address > b.address;
}

void Debug_MakeRoom(size_t capacity)
{
	functionAdresses.reserve(capacity);
}

void Debug_SortFunctions()
{
	std::stable_sort(functionAdresses.begin(), functionAdresses.end(), funcComparator);
}

void Debug_AddFunction (const char *name, size_t address)
{
	FUNCTION_ADDRESS new_function;

	new_function.address = address;

	if (name[0] == '?')
	{
#if defined(WIN32) && !defined(_BFR_)
		char buff[BUFFER_SIZE];
		if (UnDecorateSymbolName(name, buff, BUFFER_SIZE-1, 0))
		{
			new_function.name = strdup(buff);
		}
		else
#endif
		{
			new_function.name = strdup(name);
		}
	}
	else if (name[0] == '_')
	{
		new_function.name = strdup (name + 1);
	}
	else
	{
		new_function.name = strdup (name);
	}

	functionAdresses.push_back(new_function);
}

void Debug_FunctionNameFindAddresses(FILE *fp)
{
#if defined(WIN32)
	char buffer[BUFFER_SIZE];
	char name[BUFFER_SIZE];

	int done = 0;
	while (!done)
	{
		fgets(buffer, BUFFER_SIZE, fp);

		if (feof(fp))
			done = 1;

		if (sscanf(buffer, "%s", name) == 1)
		{
			if (strcmp(name, "Address") == 0)
				done = 1;
		}
	}

	fgets(buffer, BUFFER_SIZE, fp);
#endif
}

int Debug_FunctionNameOpen (char *map_file_name)
{
	FILE *fp;
	char buffer[BUFFER_SIZE];
	int done;
#if defined(WIN32)
	int crap;
#else
	char symbol[BUFFER_SIZE];
#endif
	char name[BUFFER_SIZE];
	size_t address;
#ifdef REQUIRE_CORRECT_LOG
	int log_correct;
#endif

	fp = fopen (map_file_name, "rt");
	if (!fp)
	{
#ifdef REQUIRE_CORRECT_LOG
		LOG ((LOG_FATAL, "I need the correct map file to run."));
#else
		return (0);
#endif
	}

	Debug_FunctionNameFindAddresses(fp);

	// First count how many etries we have
	// So that we can allocate memory in one go
	done = 0;
	size_t numEntries = 0;
	while (!done)
	{
		fgets(buffer, BUFFER_SIZE, fp);

		if (feof(fp))
			done = 1;

		numEntries++;
	}
	fclose(fp);

	// Preallocate the memory to store the functions
	// This way it is faster
	Debug_MakeRoom(numEntries);

	fp = fopen(map_file_name, "rt");
	Debug_FunctionNameFindAddresses(fp);

	done = 0;
	while (!done)
	{
		fgets (buffer, BUFFER_SIZE, fp);

#if defined(WIN32)
		if ((buffer[0] == ' ') &&
		    (buffer[1] == '0') &&
		    (buffer[5] == ':') &&
		    (sscanf (buffer, "%x:%x %s %zx", &crap, &crap, name, &address) == 4))
#else
		if(sscanf (buffer, "%zx %s %[^\n]", &address, symbol, name) == 3)
#endif
		{
			Debug_AddFunction (name, address);
		}

		if (feof (fp))
			done = 1;
	}

	fclose (fp);

	Debug_SortFunctions();
	function_name_open = 1;
	return (1);

	#undef BUFFER_SIZE
}

static HANDLE	hProc,
				hThread;

void Debug_FunctionNameClose (void)
{
#ifdef GENERATE_ADDRESS_LOG
	FILE *fh = fopen("address.log","w");
#endif

	function_name_open = 0;

	for (auto func : functionAdresses)
	{
#ifdef GENERATE_ADDRESS_LOG
		fprintf(fh, "%08X %s\n", func.address, func->name);
#endif
		free(func.name);
	}

	functionAdresses.clear();
	functionAdresses.shrink_to_fit();

#ifdef GENERATE_ADDRESS_LOG
	fclose (fh);
#endif
}

const char *Debug_FunctionNameGet (size_t address)
{
#ifdef __linux__
	Dl_info info;
	if(dladdr((void*)address, &info) != 0)
	{
		address -= (size_t)info.dli_fbase;
	}
#endif

	if (address > DEBUG_CODE_LIMIT)
	{
		return (kernel);
	}

	if (!function_name_open)
	{
		return (unknown);
	}
	else
	{
		for (auto func : functionAdresses)
		{
			if (address >= func.address)
				return (func.name);
		}

		return (unknown);
	}
}

const char *Debug_FunctionNameAndOffsetGet (size_t address, size_t *offset)
{
	*offset = 0;

#ifdef __linux__
	Dl_info info;
	if(dladdr((void*)address, &info) != 0)
	{
		address -= (size_t)info.dli_fbase;
	}
#endif

	if (address > DEBUG_CODE_LIMIT)
	{
		return (kernel);
	}

	if (!function_name_open)
	{
		return (unknown);
	}
	else
	{
		for (auto func : functionAdresses)
		{
			if (address >= func.address)
			{
				*offset = address - func.address;
				return (func.name);
			}
		}

		return (unknown);
	}
}

unsigned char Debug_NumToChar (unsigned char byte)
{
	if ((byte >= 32) && (byte <= 127))
		return (byte);
	else
		return ('.');
}

void DebugCallStack_DumpAddress (LogClass log_class, size_t address)
{
	const char	*caller_name;
	size_t		offset;

	caller_name = Debug_FunctionNameAndOffsetGet (address, &offset);

	LOG_INDIRECT (NULL, 0, (log_class, "  0x%08x  [%s + 0x%x]\n", address, caller_name, offset));
}

void DebugCallStack_Dump (LogClass log_class)
{
// Disabled for anything else than WIN32, because the assembler code does not seem to be save
#if defined(_MSC_VER) && defined(_X86_)
	size_t base_pointer;
#if defined(_MSC_VER) && defined(_X86_)
	__asm mov base_pointer, ebp;
#elif !defined(_MSC_VER) // if GCC 64 bit
	__asm("\t movq %%rbp,%0" : "=r"(base_pointer));
#endif // WIN32

	DebugCallStack_DumpFrom (log_class, base_pointer);
#endif
}

void DebugCallStack_DumpFrom (LogClass log_class, size_t base_pointer)
{
// Disabled for anything than WIN32, because the code segfaults, which can only be catched on Windows.
// Beside that, this is not a good design anyway.
#if defined(_MSC_VER) && defined(_X86_)
	size_t frame_limit;
	size_t frame_pointer;
	size_t caller;
	int finished;
	int registers_printed;

	int debug_dump_whole_stack = 0;

	size_t regs_eax;
	size_t regs_ebx;
	size_t regs_ecx;
	size_t regs_edx;
	size_t regs_esi;
	size_t regs_edi;
	size_t regs_esp;
	size_t regs_ebp;

#ifdef WIN32
	__asm mov regs_eax, eax;
	__asm mov regs_ebx, ebx;
	__asm mov regs_ecx, ecx;
	__asm mov regs_edx, edx;
	__asm mov regs_esi, esi;
	__asm mov regs_edi, edi;
	__asm mov regs_esp, esp;
	__asm mov regs_ebp, ebp;
#else // if GCC 64 bit
	__asm("\t movq %%rax,%0" : "=r"(regs_eax));
	__asm("\t movq %%rbx,%0" : "=r"(regs_ebx));
	__asm("\t movq %%rcx,%0" : "=r"(regs_ecx));
	__asm("\t movq %%rdx,%0" : "=r"(regs_edx));
	__asm("\t movq %%rsi,%0" : "=r"(regs_esi));
	__asm("\t movq %%rdi,%0" : "=r"(regs_edi));
	__asm("\t movq %%rsp,%0" : "=r"(regs_esp));
	__asm("\t movq %%rbp,%0" : "=r"(regs_ebp));
#endif

	finished = 0;
	registers_printed = 0;
	while (!finished)
	{
// This is code that segfaults, both on Windows and Linux, that's why we have the __try in the Windows code
#ifdef WIN32
		__try
		{
			caller = *((size_t *) (base_pointer + 4));
		}
		__except (MemoryAccessExceptionFilter (GetExceptionInformation()))
		{
			finished = 1;
		}
#else
		try
		{
			caller = *((size_t *) (base_pointer + 8)); // or 8 for 64 bit
		}
		catch(...)
		{
			finished = 1;
		}
#endif // WIN32

		if (caller >= DEBUG_CODE_LIMIT)
		{
			finished = 1;
		}

		if (!finished)
		{
			DebugCallStack_DumpAddress (log_class, caller);

			if ((debug_dump_whole_stack) && (!registers_printed))
			{
				LOG_INDIRECT (NULL, 0, (log_class, "    EAX %08x  EBX %08x  ECX %08x  EDX %08x\n",
				              regs_eax,
				              regs_ebx,
				              regs_ecx,
				              regs_edx));

				LOG_INDIRECT (NULL, 0, (log_class, "    ESI %08x  EDI %08x  ESP %08x  EBP %08x\n",
				              regs_esi,
				              regs_edi,
				              regs_esp,
				              regs_ebp));

				registers_printed = 1;
			}

			frame_pointer = base_pointer + 8;
			frame_limit = * ((size_t *) base_pointer);
			if (debug_dump_whole_stack)
			{
				while (frame_pointer < frame_limit)
				{
					LOG_INDIRECT(NULL, 0, (log_class, "    %02x %02x %02x %02x    '%c%c%c%c'\n",
					             (size_t) (* ((unsigned char *) (frame_pointer + 0))),
					             (size_t) (* ((unsigned char *) (frame_pointer + 1))),
					             (size_t) (* ((unsigned char *) (frame_pointer + 2))),
					             (size_t) (* ((unsigned char *) (frame_pointer + 3))),
					             Debug_NumToChar (* ((unsigned char *) (frame_pointer + 0))),
					             Debug_NumToChar (* ((unsigned char *) (frame_pointer + 1))),
					             Debug_NumToChar (* ((unsigned char *) (frame_pointer + 2))),
					             Debug_NumToChar (* ((unsigned char *) (frame_pointer + 3)))
					             ));
					frame_pointer += 4;
				}
			}

			base_pointer = frame_limit;
		}
	}
#endif
}

void DebugCallStack_Save  (size_t *call_stack, int number, size_t Ebp)
{
#if defined(_MSC_VER) && defined(_X86_)
	size_t base_pointer;
	size_t caller;
	int finished;
	int index;

	if(Ebp == 0)
	{
#ifdef WIN32
		__asm mov base_pointer, ebp;
#else // if GCC 64 bit
		__asm("\t movq %%rbp,%0" : "=r"(base_pointer));
#endif
	}
	else
	{
		base_pointer = Ebp;
	}

	finished = 0;
	caller = 0;
	index = 0;

	while ((!finished) && ( index < number))
	{

		if ((caller >= DEBUG_CODE_LIMIT) || (base_pointer == 0))
		{
			finished = 1;
		}
		else
		{
#ifdef WIN32
			__try
			{
				caller = *((size_t *) (base_pointer + 4));
			}
			__except (MemoryAccessExceptionFilter (GetExceptionInformation()))
			{
				finished = 1;
			}
#else
			// This does not catch segfaults like the Windows counterpart, only exceptions.
			// Of course, writing such code is bad style.
			// Fortunately, all this can be replaced by backtrace.
			try
			{
				caller = *((size_t *) (base_pointer + 8)); // 8 bytes for 64 bit
			}
			catch(...)
			{
				finished = 1;
			}
#endif // WIN32
		}

		if (!finished)
		{
			call_stack[index] = caller;

			base_pointer = * ((size_t *) base_pointer);

			if ((index != 0) && (call_stack[index - 1] == call_stack[index]))
			{
			}
			else
			{
				index ++;
			}
		}
	}
	while (index < number)
	{
		call_stack[index] = 0;

		index ++;
	}
#elif defined(_MSC_VER)
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);

	size_t cap = CaptureStackBackTrace(0, number, (void**)call_stack, NULL);
	for (size_t i = cap; i < number; i++)
	{
		call_stack[i] = 0;
	}
#else
	backtrace((void**)call_stack, number);
#endif
}

void DebugCallStack_Show  (LogClass log_class, size_t *call_stack, int number)
{
	size_t caller;
	const char *caller_name;

	int index = 0;
	while ((index < number) && (call_stack[index] != 0))
	{
		caller = call_stack[index];

		size_t offset;

		caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

		LOG_INDIRECT (NULL, 0, (log_class, "  0x%08x  [%s + 0x%x]\n", caller, caller_name, offset));

		index ++;
	}
}

//----------------------------------------------------------------------------
//
// Name       : DebugCallStack_ShowToFile
//
// Description: Writes the leaks report to a file in up-down wise.
//
// Parameters : LogClass log_class:   Unused
//              size_t *call_stack:   The call stack with unfreed memory.
//              int number:           The number of calls from the stack to report
//              FILE *file:           File to write the leak report
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void DebugCallStack_ShowToFile  (LogClass log_class, size_t *call_stack, int number, FILE *file)
{
	size_t caller;
	const char *caller_name;

	char allocator_name[1024];

	int index = 0;
	while ((index < number) && (call_stack[index] != 0))
	{
		caller = call_stack[index];

		size_t offset;

		caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

		if (index == 3)
		{
			strcpy(allocator_name, caller_name);
		}

		fprintf(file, "0x%08zx [%s+0x%zx] / ", caller, caller_name, offset);

		index ++;
	}

	fprintf(file, "\t%s\n", allocator_name);
}

//----------------------------------------------------------------------------
//
// Name       : DebugCallStack_ShowToAltFile
//
// Description: Writes the leaks report to a file in bottom-up wise.
//
// Parameters : LogClass log_class:   Unused
//              size_t *call_stack:   The call stack with unfreed memory.
//              int number:           The number of calls from the stack to report
//              FILE *file:           File to write the leak report
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void DebugCallStack_ShowToAltFile  (LogClass log_class, size_t *call_stack, int number, FILE *file)
{
	size_t caller;
	const char *caller_name;
	int index = 3;

	char buff[8196];
	char cpyBuff[8196];

	caller = call_stack[index];
	size_t offset;
	caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);
	sprintf(buff, "[%s]", caller_name);
	index++;

	while ((index < number) && (call_stack[index] != 0))
	{
		caller = call_stack[index];
		size_t offset;
		caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

		strcpy(cpyBuff, buff);
		sprintf(buff, "[%s] %s", caller_name, cpyBuff);

		index++;
	}

	fprintf(file, "%s", buff);
	fprintf(file, "\n");
}

char * c3debug_StackTrace(void)
{
	size_t caller;
	const char *caller_name;
	int index;
	MBCHAR function_name[16384];

	size_t callstack_function[k_CALL_STACK_SIZE];

	DebugCallStack_Save(callstack_function, k_CALL_STACK_SIZE, 0);

	s_stackTraceString[0] = 0;

	index = 0;
	while ((index < k_CALL_STACK_SIZE) && (callstack_function[index] != 0))
	{
		caller = callstack_function[index];

		size_t offset;

		caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

		sprintf(function_name, "  0x%08zx  [%s + 0x%zx]\n", caller, caller_name, offset);

		if (strlen(s_stackTraceString) + strlen(function_name) < k_STACK_TRACE_LEN - 1 )
			strcat(s_stackTraceString, function_name);

		index ++;
	}

	return s_stackTraceString;
}

#if defined(_MSC_VER)
char * c3debug_ExceptionStackTrace(LPEXCEPTION_POINTERS exception)
{
	if(!function_name_open)
	{
		DebugCallStack_Open();
	}

	size_t caller;
	const char *caller_name;
	int index;
	MBCHAR function_name[_MAX_PATH];

	size_t callstack_function[k_CALL_STACK_SIZE];

#if defined(_X86_)
	callstack_function[0] = exception->ContextRecord->Eip;

	DebugCallStack_Save(&callstack_function[1], k_CALL_STACK_SIZE - 1, exception->ContextRecord->Ebp);
#else
	callstack_function[0] = NULL;

	DebugCallStack_Save(&callstack_function[1], k_CALL_STACK_SIZE - 1, 0);
#endif

	s_stackTraceString[0] = 0;

	index = 0;
	while ((index < k_CALL_STACK_SIZE) && (callstack_function[index] != 0))
	{
		caller = callstack_function[index];

		if(function_name_open)
		{
			size_t offset;
			caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);

			sprintf(function_name, "  0x%08zx  [%s + 0x%zx]\n", caller, caller_name, offset);
		}
		else
		{
			sprintf(function_name, "  0x%08zx\n", caller);
		}

		if (strlen(s_stackTraceString) + strlen(function_name) < k_STACK_TRACE_LEN - 1 )
			strcat(s_stackTraceString, function_name);

		index ++;
	}

	return s_stackTraceString;
}
#endif // WIN32

char * c3debug_ExceptionStackTraceFromFile(FILE *f)
{
	if(!function_name_open)
	{
		DebugCallStack_Open();
	}

	size_t caller;
	char line[1024];
	const char *caller_name;

	size_t offset;

	MBCHAR function_name[_MAX_PATH];
	s_stackTraceString[0] = 0;
	while(!feof(f))
	{
		if(fgets(line, 1024, f)) {
			if(sscanf(line, "  0x%08zx", &caller) == 1)
			{
				caller_name = Debug_FunctionNameAndOffsetGet(caller, &offset);

				sprintf(function_name, "  0x%08zx  [%s + 0x%zx]\n", caller, caller_name, offset);

				if (strlen(s_stackTraceString) + strlen(function_name) < k_STACK_TRACE_LEN - 1 )
					strcat(s_stackTraceString, function_name);
			}
		}
	}
	return s_stackTraceString;
}

cDebugCallStackSet::cDebugCallStackSet(int depth)
{
	m_depth = depth;
	m_blockSize = m_depth + 1;
	m_numStacks = 0;

	m_maxNumStacks = 16;
	m_caller = 0;

	m_stacks = new size_t[m_blockSize*m_maxNumStacks];

	m_curStack = new size_t[m_depth+2];
}

cDebugCallStackSet::~cDebugCallStackSet()
{
	delete [] m_stacks;
	delete [] m_curStack;
}

static int qsortDebugCallStack(const void *a,const void *b)
{
	return (int)(*((size_t*)b)) - (int)(*((size_t*)a));
}

void cDebugCallStackSet::Add()
{
	DebugCallStack_Save(m_curStack,m_depth+2,0);

	int i;
	for (i=0;i<m_numStacks;++i)
	{
		if (memcmp(m_curStack+2,&m_stacks[m_blockSize*i+1],sizeof(size_t)*m_depth)==0)
		{
			m_stacks[m_blockSize*i]++;

			if (m_stacks[m_blockSize*i]%16 == 0)
			{
				qsort(m_stacks,m_numStacks,m_blockSize*sizeof(size_t),qsortDebugCallStack);
			}

			return;
		}
	}

	if (m_numStacks>=m_maxNumStacks)
	{
		int newMax = m_maxNumStacks*=2;
		size_t *pNewStack = new size_t[m_blockSize * newMax];

		memcpy(pNewStack,m_stacks,m_numStacks*m_blockSize*sizeof(size_t));

		delete [] m_stacks;
		m_stacks = pNewStack;
		m_maxNumStacks = newMax;
	}

	m_caller = m_curStack[1];
	memcpy(&m_stacks[m_blockSize*i+1],m_curStack+2,sizeof(size_t)*m_depth);
	m_stacks[m_blockSize*i] = 1;
	m_numStacks++;

	qsort(m_stacks,m_numStacks,m_blockSize*sizeof(size_t),qsortDebugCallStack);

	if (m_numStacks > 1)
	{
		Assert(m_stacks[0] >= m_stacks[m_blockSize]);
	}
}

void cDebugCallStackSet::Dump(const char *filename)
{
	const char *caller_name;
	size_t offset;

	qsort(m_stacks,m_numStacks,m_blockSize*sizeof(size_t),qsortDebugCallStack);

	FILE *fp = fopen(filename,"wt");

	caller_name = Debug_FunctionNameAndOffsetGet(m_caller,&offset);

	size_t totalCalls = 0;
	int i;
	for (i=0;i<m_numStacks;++i)
	{
		totalCalls += m_stacks[m_blockSize*i];
	}

	fprintf(fp,"callstack dump for function:\n%s\ncalled %zu times\n",caller_name,totalCalls);

	for (i=0;i<m_numStacks;++i)
	{
		size_t called = m_stacks[m_blockSize*i];
		float perCalled = (float)(100 * called)/(float)totalCalls;
		fprintf(fp,"\nnum times called: %zu, %.2f percent\n",called,perCalled);
		int j;
		for (j=0;j<m_depth;++j)
		{
			size_t caller = m_stacks[(m_blockSize*i)+1+j];
			caller_name = Debug_FunctionNameAndOffsetGet (caller, &offset);
			fprintf(fp,"%s\n",caller_name);
		}
	}
	fclose(fp);
}
