//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : General declarations
// Id           : $Id$
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
// Should be the first thing in the program that is created and 
// the last thing that is destroyed, so that we catch all leaks
// and don't catch anything that is freed after the leak report,
// as it was the case with the old way leaks were reported.
// Since this is supposed to be the very last thing that goes, 
// you might already back at the desktop while CTP2 is running 
// is the background writing the leak report.
static AutoDebug autoDebug;

AutoDebug::AutoDebug(){ Debug_Open(); }
AutoDebug::~AutoDebug(){ Debug_Close(); }
#endif

#ifdef _MSC_VER
#pragma hdrstop
#endif
