
#pragma once

#ifndef __C3AI_H__
#define __C3AI_H__

typedef void (* CivExceptionFunction) (void);

char * c3ai_StackTrace(void);
char * c3ai_ExceptionStackTrace(LPEXCEPTION_POINTERS exception);
LONG _cdecl c3ai_CivExceptionHandler (LPEXCEPTION_POINTERS exception_pointers);
void c3ai_ExceptionExecute(CivExceptionFunction function);
void c3ai_Assert(char *s, char *file, int line);
void c3ai_Log(char *);

#endif