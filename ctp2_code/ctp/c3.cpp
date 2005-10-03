//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : General declarations
// Id           : $Id:$
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
// _DEBUGTOOLS
// - Provides debug tools (leak reporter, asserts, ...)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added struct for automatic debug tool initialization and
//   cleanup so that it is the first respective the last thing 
//   that is done in the program. (Oct 3rd 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef _DEBUGTOOLS
static int s_AutoDebugCounter = 0;
AutoDebug::AutoDebug(){ if(s_AutoDebugCounter == 0){ Debug_Open();} s_AutoDebugCounter++; }
AutoDebug::~AutoDebug(){ if(s_AutoDebugCounter == 1){ Debug_Close();} s_AutoDebugCounter--; }
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif
