/*----------------------------------------------------------------------------
 *
 * Project      : Call To Power 2
 * File type    : C/C++ header
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
 *----------------------------------------------------------------------------
 *
 * Modifications from the original Activision code:
 *
 * - Added function to display a thread name in the debugger.
 *
 *----------------------------------------------------------------------------
 */

#ifdef _DEBUG

#ifndef __DEBUG_H
#define __DEBUG_H

#if defined(WIN32)
#include <windows.h>	/* LPCSTR, DWORD */
#endif

#ifdef __cplusplus
extern "C" {
#endif

void Debug_Open (void);
void Debug_Close (void);

#ifdef WIN32
void Debug_SetThreadName(LPCSTR szThreadName, DWORD dwThreadID);
#endif // WIN32
#ifdef LINUX
void Debug_SetProcessName(char const * szProcessName);
#endif

#ifdef __cplusplus
} // extern "C"

namespace Os
{
    inline void SetThreadName(char const * szThreadName)
    {
#if defined(WIN32)
        Debug_SetThreadName((LPCSTR) szThreadName, (DWORD) -1);
#elif defined(LINUX)
        Debug_SetProcessName(szThreadName);
#endif
    };
} // namespace Os

#endif /* __cplusplus */

#endif /* Multiple include guard */

#endif /* _DEBUG */
