#ifndef __os_include_ctp2_config_h__
#define __os_include_ctp2_config_h__ 1

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#if defined(HAVE_CONFIG_H)
#include "config.h"
#elif defined(_MSC_VER)
#include "config_win32.h"
#else
#error "Unsupported build environment."
#endif

/// \def UNREACHABLE_RETURN(a_Value)
/// Mark a return statement as unreachable.
/// Compiler workaround for MSVC7, which has no knowledge about functions
/// that do not return, and will report missing return statements otherwise.
#if defined(_MSC_VER) && (_MSC_VER < 1400)
#define UNREACHABLE_RETURN(a_Value)         return a_Value
#else
#define UNREACHABLE_RETURN(a_Value)
#endif

/// \def VOID_PARAMETER_FOR_TEMPLATE(a_Type)
/// Mark a template function as expecting no arguments when called.
/// Compiler workaround for MSVC6, which incorrectly requires template functions
/// to use the template argument (type) in function arguments or return value.
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#define VOID_PARAMETER_FOR_TEMPLATE(a_Type) a_Type * unused = 0
#else
#define VOID_PARAMETER_FOR_TEMPLATE(a_Type) void
#endif

/// \def CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(a_Type)
/// Mark a template function as expecting no arguments when called.
/// Compiler workaround for MSVC6, which incorrectly requires template functions
/// to use the template argument (type) in function arguments or return value.
#if defined(_MSC_VER) && (_MSC_VER < 1300)
#define CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(a_Type) (static_cast<a_Type*>(0))
#else
#define CALL_TEMPLATE_FUNCTION_WITHOUT_ARGUMENT(a_Type) <a_Type>()
#endif

#endif // __os_include_ctp2_config_h__
