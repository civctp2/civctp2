//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Retrieve CPU information through cpuinf32.dll.
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Stubbed out all functionality to prevent crashes on some machines.
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "c3cpu.h"


// Stub versions of the above

MBCHAR g_cpuString[] = "";

void c3cpu_Initialize(void)
{
}

void c3cpu_Examine(void)
{
}

MBCHAR * c3cpu_GetString(void)
{
	return g_cpuString;
}

