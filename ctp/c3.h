











#pragma once
#ifndef __C3_H__
#define __C3_H__

#if defined(_TEST) || !defined(_DEBUG)
#pragma warning( disable : 4056 )
#endif

#pragma warning( error : 4700 )


#include <limits.h>
#include <crtdbg.h>
#include <windows.h>
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


#include "c3types.h"
#include "c3debug.h"
#include "c3errors.h"
#include "aui.h"

#endif
