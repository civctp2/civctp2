//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
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
// - Generate debug version.
//
// _DEBUGTOOLS
// - Provides debug tools (leak reporter, asserts, ...)
//
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only).
//
// __GNUC__
// - Compiler version (for the GNU C++ compiler only).
//
// _TEST
// ?
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
// - Standardised min/max usage.
// - Prevented useless warnings.
// - Moved DirectX API-Version to use into "ctp2_config.h"
// - Added struct for automatic debug tool initialization and
//   cleanup so that it is the first respective the last thing
//   that is done in the program. (Oct 3rd 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __C3_H__
#define __C3_H__

#include "ctp2_config.h"
#include "c3types.h"

#if defined(_MSC_VER)

#pragma warning(disable:4100)   // (Level 4)   'identifier' : unreferenced formal parameter
#pragma warning(disable:4127)   // (Level 4)   conditional expression is constant
#pragma warning(disable:4786)   // (Level ?)   identifier length over 255 (with templates)
#pragma warning(   1   :4800)   // (Level 3)   'type' : forcing value to bool 'true' or 'false' (performance warning)

#pragma warning( error :4700)   // (Level 1&4) uninitialized local variable 'name' used
#pragma warning( error :4804)   // (Level 1)   'operation' : unsafe use of type 'bool' in operation

#if defined(_DEBUG)
#pragma warning(disable:4130)   // logical operation on address of string constant (used in Asserts)
#endif

#if (_MSC_VER >= 1400)
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
// Suppress some rediculous MS warnings about "deprecated" standard functions
#define _SECURE_SCL					0
#define _SECURE_SCL_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _HAS_ITERATOR_DEBUGGING		0	// this one may be more interesting
//#define _SECURE_SCL_THROWS 1
#endif
#endif	// _MSC_VER

#include <limits.h>
#include <string.h>

#if defined(WIN32)
#include <crtdbg.h>

// Do not define the min and max *macros* in <windows.h>.
#define NOMINMAX
#include <windows.h>
#else
#include "windows.h"
#endif // WIN32

#include "minmax.h"

#if defined(WIN32)
#include <tchar.h>

#include <ddraw.h>
#include <dinput.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>

#if defined(__GNUC__)
#define _MAX_PATH PATH_MAX
#endif // __GNUC__
#include "c3debug.h"
#include "c3errors.h"
#include "aui.h"

#endif // __C3_H__
