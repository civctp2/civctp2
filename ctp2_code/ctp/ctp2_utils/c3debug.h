//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
// _DEBUG
// - Generate debug version when set.
//
// SHOW_ASSERTS
// - Shows asserts even if _DEBUG is not set.
//
// USE_LOGGING
// - Enable logging facilities - even when not using the debug build.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Marked MS specific pragma
// - Corrected include dependency.
// - Report for !WIN32 on stderr
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __C3DEBUG_H__
#define __C3DEBUG_H__

#include "ctp2_config.h"
#include "c3.h"
#undef Assert

#if defined(_DEBUG) || defined(SHOW_ASSERTS)

#ifndef _AIDLL
	#define Assert(x)				{ if (!(x)) c3debug_Assert(#x, __FILE__, __LINE__); }
#else
	#include "c3ai.h"
	#define Assert(x)				{ if (!(x)) c3ai_Assert(#x, __FILE__, __LINE__); }
#endif

#else
#define Assert(x)
#endif

#if defined(WIN32)
#define Report(x)				{_RPT0(_CRT_WARN, x);}
#else
#define Report(x) { fprintf(stderr, "%s\n", x); }
#endif

typedef void (* CivExceptionFunction) (void);

#define k_DBG_ALL               0xffffffff
#define k_DBG_NONE              0x00000000
#define k_DBG_INFO              0x00000001
#define k_DBG_AI                0x00000002
#define k_DBG_NET               0x00000004
#define k_DBG_GRAPHICS          0x00000008
#define k_DBG_DATABASE          0x00000010
#define k_DBG_FILE              0x00000020
#define k_DBG_GAMESTATE         0x00000040
#define k_DBG_UI                0x00000080
#define k_DBG_FIX               0x00000100
#define k_DBG_SLIC              0x00000200
#define k_DBG_SCHEDULER         0x00000400
#define k_DBG_SCHEDULER_DETAIL  0x00000c00
#define k_DBG_SCHEDULER_ALL     0x00001c00
#define k_DBG_DIPLOMACY         0x00002000
#define k_DBG_MAPANALYSIS       0x00004000
#define k_DBG_ASTAR             0x00008000
#define k_DBG_GOVERNOR          0x00010000

#define k_DEBUG_OWNER_NONE          0
#define k_DEBUG_OWNER_CRC           1
#define k_DEBUG_OWNER_COMMANDLINE   2
#define k_DEBUG_OWNER_NETWORK_CHAT  3
#define k_DEBUG_OWNER_ADVANCES      4
#define k_DEBUG_OWNER_SEEWWR        5
#define k_DEBUG_OWNER_FRAME_RATE    6
#define k_DEBUG_OWNER_ZBS           7

void	c3debug_InitDebugLog();
void	c3debug_CloseDebugLog();
void	c3debug_dprintf(char const * format, ...);
void	c3debug_dprintfPrefix(sint32 mask, char const * file, sint32 line);
void	c3debug_SetDebugMask(sint32 mask, sint32 set);
#if defined(WIN32)
char	*c3debug_StackTrace(void);
char	*c3debug_ExceptionStackTrace(LPEXCEPTION_POINTERS exception);
static LONG _cdecl c3debug_CivExceptionHandler (LPEXCEPTION_POINTERS exception_pointers);
#endif
void	c3debug_ExceptionExecute(CivExceptionFunction function);

void	c3debug_Assert(char const * s, char const * file, int line);

#if defined(_DEBUG) || defined(USE_LOGGING)
	#define DPRINTF(mask, x) { c3debug_dprintfPrefix(mask, __FILE__, __LINE__); c3debug_dprintf x;}
#else
	#define DPRINTF(mask, x);
#endif

#endif
