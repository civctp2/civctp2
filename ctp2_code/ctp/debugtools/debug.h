#ifdef _DEBUG

#ifndef __DEBUG_H
#define __DEBUG_H





#include "DebugAssert.h"
#include "DebugException.h"
#include "DebugMemory.h"
#include "Log.h"
#include "Breakpoint.h"

#ifdef __cplusplus
extern "C" {
#endif



void Debug_Open (void);
void Debug_Close (void);



#ifdef __cplusplus
}
#endif

#endif	

#endif 
