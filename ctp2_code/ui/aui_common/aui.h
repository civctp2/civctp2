//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifndef __AUI_H__
#define __AUI_H__

#if defined(WIN32)
#include <windows.h>
#include <mmsystem.h>
#include <vfw.h>
#include <tchar.h>
#include <mbstring.h>
#else
#include "os/nowin32/windows.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>

#include "ui/ldl/ldl.h"
#include "ui/freetype/freetype.h"

#include "ui/aui_common/auicfg.h"
#include "ui/aui_common/auitypes.h"
#include "ui/aui_common/auidebug.h"


#if defined(WIN32)
#if defined(__AUI_USE_DIRECTMEDIA__) && !defined(__AUI_USE_DIRECTX__)
#define __AUI_USE_DIRECTX__
#endif
#endif // WIN32

#if defined(__AUI_USE_DIRECTX__)
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>
#endif // __AUI_USE_DIRECTX__

#ifdef __AUI_USE_DIRECTMEDIA__
#include <amstream.h>
#include <ddstream.h>
#include <mmstream.h>
//#include <reftime.h>
#endif // __AUI_USE_DIRECTMEDIA__

#endif
