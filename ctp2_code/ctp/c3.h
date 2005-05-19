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

#if defined(_TEST) || !defined(_DEBUG)
#pragma warning( disable : 4056 )
#endif

#pragma warning(disable:4786)	// identifier length over 255 (with templates)
#pragma warning(disable:4800)	// BOOL to bool conversion

#pragma warning( error : 4700 )
#endif	// _MSC_VER

#include <limits.h>

#if defined(WIN32)
#include <crtdbg.h>

// Do not define the min and max *macros* in <windows.h>.
#define NOMINMAX	
#include <windows.h>
#else
#include "windows.h"
#endif // WIN32

#if defined(_MSC_VER) && (_MSC_VER < 1300)
	// MSVC 6.0 does not even have std::min and std::max.
	namespace std
	{
		template <typename T>
		inline T const & min(T const & a, T const & b)
		{
			return (a < b) ? a : b;
		};

		template <typename T>
		inline T const & max(T const & a, T const & b)
		{
			return (a < b) ? b : a;
		};
	};	// namespace std

#else
	#include <algorithm>
#endif	// _MSC_VER < 1300

	// Allow usage of global min and max to reduce the number of code changes.
	using std::min;
	using std::max;

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
