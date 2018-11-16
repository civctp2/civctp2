#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "ctp2_config.h"

#ifdef _DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define Breakpoint() __asm { int 03h }
#else
#define Breakpoint() assert(0)
#endif

#ifdef __cplusplus
}
#endif

#endif // _DEBUG

#endif // BREAKPOINT_H
