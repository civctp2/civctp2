//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : World utilities
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Created file (24-Mar-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __WORLDUTILS__h__
#define __WORLDUTILS__h__ 1

#include "MapRecord.h"

const MapRecord *worldutils_FindBestMapSizeMatch(sint32 w, sint32 h);
double *worldutils_CreateSettings(const MapRecord *mapRec, sint32 level, sint32 &numSettings);
void worldutils_DeleteSettings(double* settings);

#endif
