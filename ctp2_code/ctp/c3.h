//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : General declarations
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
// _MSC_VER		
// - Use Microsoft C++ extensions when set.
//
// _TEST
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - #pragmas commented out
//
//----------------------------------------------------------------------------

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef __C3_H__
#define __C3_H__

#if defined(_MSC_VER)

#if defined(_TEST) || !defined(_DEBUG)
#pragma warning( disable : 4056 )
#endif
#if !defined(ACTIVISION_ORIGINAL)
#pragma warning(disable:4786)  // '255' characters in the debug information
#endif
#pragma warning( error : 4700 )
#endif	// _MSC_VER

#include <limits.h>
#if !defined(__GNUC__)
#include <crtdbg.h>
#endif

#if defined(ACTIVISION_ORIGINAL)
#include <windows.h>
#else
// Do not define the min and max *macros* in <windows.h>.
#define NOMINMAX	
#include <windows.h>

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
#endif	// ACTIVISION_ORIGINAL

#include <tchar.h>

#define DIRECTINPUT_VERSION 0x0700

#include "ddraw.h"
#include "dinput.h"


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
#endif

#include "c3types.h"
#include "c3debug.h"
#include "c3errors.h"
#include "aui.h"

#endif
