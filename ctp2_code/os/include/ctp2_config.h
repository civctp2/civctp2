#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __os_include_ctp2_config_h__
#define __os_include_ctp2_config_h__ 1

#if defined(HAVE_CONFIG_H)
#include "config.h"
#elif defined(_MSC_VER)
#include "config_win32.h"
#else 
#error "Unsupported build environment."
#endif

#endif

