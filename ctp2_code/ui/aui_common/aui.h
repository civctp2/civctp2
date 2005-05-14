










#ifndef __AUI_H__
#define __AUI_H__

#if defined(WIN32)
#include <windows.h>
#include <mmsystem.h>
#include <vfw.h>
#include <tchar.h>
#include <mbstring.h>
#endif


#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>


#include "ldl.h"
#include "freetype.h"


#include "auicfg.h"
#include "auitypes.h"
#include "auidebug.h"



#if defined(WIN32)
#if defined(__AUI_USE_DIRECTMEDIA__) && !defined(__AUI_USE_DIRECTX__)
#define __AUI_USE_DIRECTX__
#endif

#if defined(__AUI_USE_DIRECTX__)
#include <ddraw.h>
#include <dinput.h>
#include <dsound.h>
#endif 

#ifdef __AUI_USE_DIRECTMEDIA__
#include <amstream.h>
#include <ddstream.h>
#include <mmstream.h>
#include <reftime.h>
#endif 
#endif

#endif 
