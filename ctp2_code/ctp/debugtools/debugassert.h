#ifdef _DEBUG










#ifndef DEBUG_ASSERT_H
#define DEBUG_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
















typedef void (* DebugAssertClientFunction) (void);

void DebugAssert_Open (
	DebugAssertClientFunction function_enter,
	DebugAssertClientFunction function_leave);














void DebugAssert_Close (void);















#undef ASSERT
#define ASSERT(condition)													\
	{if (! ((int) (condition))) {											\
		DebugAssert_Assert ( __FILE__, __LINE__, LOG_ASSERT, #condition);	\
	}}
















#define ASSERT_CLASS(log_class, condition)								\
	{if (! ((int) (condition))) {										\
		DebugAssert_Assert (__FILE__, __LINE__, log_class, #condition);	\
	}}













#define ASSERT_INDIRECT(module_name, module_line, condition)					\
	{if (! ((int) (condition))) {												\
		DebugAssert_Assert (module_name, module_line, LOG_ASSERT, #condition);	\
	}}












void DebugAssert_Assert (const char *module_name, int module_line, const char *log_class, const char *condition_string);




#ifdef __cplusplus
}
#endif

#endif

#endif
