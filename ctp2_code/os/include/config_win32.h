#ifndef __os_include_config_win32_h__
#define __os_include_config_win32_h__ 1

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#ifndef   DIRECTDRAW_VERSION
#define   DIRECTDRAW_VERSION 0x0700
#endif // DIRECTDRAW_VERSION

#ifndef   DIRECTINPUT_VERSION
#define   DIRECTINPUT_VERSION 0x0700
#endif // DIRECTINPUT_VERSION

#ifndef   DIRECTSOUND_VERSION
#define   DIRECTSOUND_VERSION 0x0700
#endif // DIRECTSOUND_VERSION

#if defined(USE_SDL)
#define __AUI_USE_SDL__ 1
#endif

#endif

