#ifdef _DEBUG


#include "DebugAssert.h"
#include "DebugCallstack.h"
#include "Breakpoint.h"
#include "log.h"







struct DebugAssert
{
	
	
	
	DebugAssertClientFunction DebugAssert_Enter;
	DebugAssertClientFunction DebugAssert_Leave;
};

static DebugAssert debug_assert = {0, 0};














void DebugAssert_Open (
	DebugAssertClientFunction function_enter,
	DebugAssertClientFunction function_leave)
{
	debug_assert.DebugAssert_Enter = function_enter;
	debug_assert.DebugAssert_Leave = function_leave;
}














void DebugAssert_Close (void)
{
	
}



















	
void DebugAssert_Assert (const char *module_name, int module_line, const char *log_class, const char *condition_string)
{
	
	Log_Enable (log_class);

	
	LOG_INDIRECT (module_name, module_line, (log_class, "ASSERT (%s)", condition_string));

	
	DebugCallStack_Dump (log_class);

	
	if (debug_assert.DebugAssert_Enter)
	{
		debug_assert.DebugAssert_Enter();
	}

	
	Breakpoint();	

	
	
	
	if (debug_assert.DebugAssert_Leave)
	{
		debug_assert.DebugAssert_Leave();
	}
}

#endif 