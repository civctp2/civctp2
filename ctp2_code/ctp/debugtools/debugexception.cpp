#ifdef _DEBUG

#include "debugexception.h"
#include "debugassert.h"
#include "debugcallstack.h"
#include "breakpoint.h"
#include "log.h"
#include <windows.h>








struct DebugException
{


	DebugAssertClientFunction DebugException_Enter;
};

static DebugException debug_exception = {0};



















void DebugException_Open (DebugExceptionClientFunction function_enter)
{
	debug_exception.DebugException_Enter = function_enter;
}














void DebugException_Close (void)
{

}












inline static void DebugExceptionFilter_LogExceptionType (LPEXCEPTION_POINTERS ep)
{




	switch(ep->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    ACCESS VIOLATION"));
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    ARRAY BOUND EXCEEDED"));
		break;

	case EXCEPTION_BREAKPOINT:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    BREAKPOINT"));
		break;

	case EXCEPTION_DATATYPE_MISALIGNMENT:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    DATATYPE MISALIGNMENT"));
		break;

	case EXCEPTION_FLT_DENORMAL_OPERAND:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT DENORMAL OPERAND"));
		break;

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT DIVIDE BY ZERO"));
		break;

	case EXCEPTION_FLT_INEXACT_RESULT:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT INEXACT RESULT"));
		break;

	case EXCEPTION_FLT_INVALID_OPERATION:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT INVALID OPERATION"));
		break;

	case EXCEPTION_FLT_OVERFLOW:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT_OVERFLOW"));
		break;

	case EXCEPTION_FLT_STACK_CHECK:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT STACK CHECK"));
		break;

	case EXCEPTION_FLT_UNDERFLOW:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    FLOATING POINT UNDERFLOW"));
		break;

	case EXCEPTION_ILLEGAL_INSTRUCTION:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    ILLEGAL INSTRUCTION"));
		break;

	case EXCEPTION_IN_PAGE_ERROR:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    IN PAGE ERROR"));
		break;

	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    INTEGER DIVIDE BY ZERO"));
		break;

	case EXCEPTION_INT_OVERFLOW:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    INTEGER OVERFLOW"));
		break;

	case EXCEPTION_INVALID_DISPOSITION:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    INVALID DISPOSTION"));
		break;

	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    NONCONTINUABLE EXCEPTION"));
		break;

	case EXCEPTION_PRIV_INSTRUCTION:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    PRIV INSTRUCTION"));
		break;

	case EXCEPTION_SINGLE_STEP:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    SINGLE STEP"));
		break;

	case EXCEPTION_STACK_OVERFLOW:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    STACK OVERFLOW"));
		break;

	default:
		LOG ((LOG_EXCEPTION, " EXCEPTION:    UNKNOWN EXCEPTION - %08xh", ep->ExceptionRecord->ExceptionCode));
		break;
	}
}

inline static void DebugExceptionFilter_LogRegisterState (LPEXCEPTION_POINTERS ep)
{




	LOG ((LOG_EXCEPTION, "  EAX: %08xh      ESI: %08xh", (DWORD) ep->ContextRecord->Eax, (DWORD) ep->ContextRecord->Esi));
	LOG ((LOG_EXCEPTION, "  EBX: %08xh      EDI: %08xh", (DWORD) ep->ContextRecord->Ebx, (DWORD) ep->ContextRecord->Edi));
	LOG ((LOG_EXCEPTION, "  ECX: %08xh", (DWORD) ep->ContextRecord->Ecx));
	LOG ((LOG_EXCEPTION, "  EDX: %08xh", (DWORD) ep->ContextRecord->Edx));
	LOG ((LOG_EXCEPTION, "   SS: %08xh       CS: %08xh", (DWORD) ep->ContextRecord->SegSs, (DWORD) ep->ContextRecord->SegCs));
	LOG ((LOG_EXCEPTION, "  EBP: %08xh      EIP: %08xh", (DWORD) ep->ContextRecord->Ebp, (DWORD) ep->ContextRecord->Eip));
	LOG ((LOG_EXCEPTION, "  ESP: %08xh", (DWORD) ep->ContextRecord->Esp));
	LOG ((LOG_EXCEPTION, "   DS: %08xh       FS: %08xh", (DWORD) ep->ContextRecord->SegDs, (DWORD) ep->ContextRecord->SegFs));
	LOG ((LOG_EXCEPTION, "   ES: %08xh       GS: %08xh", (DWORD) ep->ContextRecord->SegEs, (DWORD) ep->ContextRecord->SegGs));
	LOG ((LOG_EXCEPTION, "Flags: %08xh       CF: %d PF:%d AF:%d ZF:%d SF:%d OF:%d",
		(DWORD) ep->ContextRecord->EFlags,
		(int) ep->ContextRecord->EFlags & 0x0001 ? 1 : 0,
		(int) ep->ContextRecord->EFlags & 0x0004 ? 1 : 0,
		(int) ep->ContextRecord->EFlags & 0x0010 ? 1 : 0,
		(int) ep->ContextRecord->EFlags & 0x0040 ? 1 : 0,
		(int) ep->ContextRecord->EFlags & 0x0080 ? 1 : 0,
		(int) ep->ContextRecord->EFlags & 0x0100 ? 1 : 0,
		(int) ep->ContextRecord->EFlags & 0x1000 ? 1 : 0));
}

static LONG _cdecl DebugException_Filter (LPEXCEPTION_POINTERS exception_pointers)
{

	DebugExceptionFilter_LogExceptionType (exception_pointers);
	DebugExceptionFilter_LogRegisterState (exception_pointers);

	DebugCallStack_DumpAddress (LOG_EXCEPTION, exception_pointers->ContextRecord->Eip);
	DebugCallStack_DumpFrom (LOG_EXCEPTION, exception_pointers->ContextRecord->Ebp);

	if (debug_exception.DebugException_Enter)
	{
		debug_exception.DebugException_Enter();
	}

	return (EXCEPTION_CONTINUE_SEARCH);
}













void DebugException_Execute (DebugExceptionClientFunction function_monitored)
{

	__try
	{
		function_monitored();
	}

	__except (DebugException_Filter (GetExceptionInformation()))
	{
	}
}


#endif
