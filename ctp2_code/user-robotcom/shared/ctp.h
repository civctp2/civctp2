#pragma once
#ifndef __C3_H__
#define __C3_H__ 1

#if defined(_TEST) || !defined(_DEBUG)
#pragma warning( disable : 4056 )
#endif

#pragma warning( error : 4700 )

#include <limits.h>
#include <crtdbg.h>
#include <windows.h>
#include <tchar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>

#include "ctp-types.h"

#include <crtdbg.h>
#define Assert(x) _ASSERTE(x)
#endif __C3_H__
