#ifndef DEBUG_EXCEPTION_H
#define DEBUG_EXCEPTION_H

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifdef _DEBUG

#ifdef __cplusplus
extern "C" {
#endif

typedef void (* DebugExceptionClientFunction) (void);

#ifdef WIN32
void DebugException_Open (DebugExceptionClientFunction function_enter);

void DebugException_Close (void);

void DebugException_Execute (DebugExceptionClientFunction function_monitored);
#endif

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _DEBUG

#endif // DEBUG_EXCEPTION_H
