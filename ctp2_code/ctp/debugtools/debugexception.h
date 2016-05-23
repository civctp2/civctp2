#ifdef _DEBUG










#ifndef DEBUG_EXCEPTION_H
#define DEBUG_EXCEPTION_H

#ifdef __cplusplus
extern "C" {
#endif

















typedef void (* DebugExceptionClientFunction) (void);

void DebugException_Open (DebugExceptionClientFunction function_enter);














void DebugException_Close (void);













void DebugException_Execute (DebugExceptionClientFunction function_monitored);




#ifdef __cplusplus
}
#endif

#endif

#endif
