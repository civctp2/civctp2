/*----------------------------------------------------------------------------
 *
 * Project      : Call To Power 2
 * File type    : C source
 * Description  : Debug functions
 *
 *----------------------------------------------------------------------------
 *
 * Disclaimer
 *
 * THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
 *
 * This material has been developed at apolyton.net by the Apolyton CtP2 
 * Source Code Project. Contact the authors at ctp2source@apolyton.net.
 *
 *----------------------------------------------------------------------------
 *
 * Compiler flags
 *
 * _DEBUG
 * This code is only activated when this flag has been set.
 *
 * _MSC_VER		
 * - Compiler version (for the Microsoft C++ compiler only).
 *
 *----------------------------------------------------------------------------
 *
 * Modifications from the original Activision code:
 *
 * - Added function to display a thread name in the debugger.
 *
 *----------------------------------------------------------------------------
 */


#ifdef _DEBUG

#include "debug.h"  // Own declarations: consistency check

#include <stdio.h>
#ifdef LINUX
#include <linux/prctl.h>
#include <linux/version.h>
#endif

#include "debugassert.h"
#include "debugcallstack.h"
#include "debugexception.h"
#include "debugmemory.h"
#include "log.h"

typedef struct tagTHREADNAME_INFO
{
    DWORD           dwType;     // must be 0x1000
    LPCSTR          szName;     // pointer to name (in user addr space)
    DWORD           dwThreadID; // thread ID (-1=caller thread)
    DWORD           dwFlags;    // reserved for future use, must be zero
}   THREADNAME_INFO;

void Debug_SystemCleanup (void)
{
}

void Debug_SystemRestore (void)
{
}

//
// open all debugging systems.
//

void Debug_Open (void)
{
	DebugCallStack_Open();

	Log_Open ("CTP_debug.cfg", 0);

	DebugAssert_Open (Debug_SystemCleanup, Debug_SystemRestore);
	DebugException_Open (Debug_SystemCleanup);
	DebugMemory_Open();
}


//
// Only close memory system ... logs asserts etc may be used in static desctructors
//
void Debug_Close (void)
{
	DebugMemory_Close();
	Log_Close();
}

/*----------------------------------------------------------------------------
 *
 * Name       : Debug_SetThreadName
 *
 * Description: Add a name to a thread to display in the debugger.
 *
 * Parameters : szThreadName    : name of the thread
 *              dwThreadId      : thread ID
 *
 * Globals    : -
 *
 * Returns    : -
 *
 * Remark(s)  : When using -1 as thread ID, the current thread is used.
 *
 *----------------------------------------------------------------------------
 */
#ifdef WIN32
void Debug_SetThreadName(LPCSTR szThreadName, DWORD dwThreadID)
{
    THREADNAME_INFO info;
    info.dwType     = 0x1000;
    info.szName     = szThreadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags    = 0;

#if defined(_MSC_VER)
    __try
    {
        RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*) &info);
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {
    }
#endif
}
#endif // WIN32

/*----------------------------------------------------------------------------
 *
 * Name       : Debug_SetProcessName
 *
 * Description: Add a name to a process to display in the debugger.
 *
 * Parameters : szProcessName    : name of the process
 *
 * Globals    : -
 *
 * Returns    : -
 *
 *----------------------------------------------------------------------------
 */
#ifdef LINUX
void Debug_SetProcessName(char const * szProcessName)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 9)
	int rc = prctl(PR_SET_NAME, szProcessName);
#endif
}
#endif

#endif // def _DEBUG

