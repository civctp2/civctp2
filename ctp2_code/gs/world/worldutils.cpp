//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Moved some functionality from the old map database. (24-Mar-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"             // Precompiled header
#include "worldutils.h"     // Own declarations

const MapRecord *worldutils_FindBestMapSizeMatch(sint32 w, sint32 h)
{
	sint32 area, actualarea = w * h;
	sint32 closestArea = -1;
	sint32 closestRec = -1;

	for(sint32 i = 0; i < g_theMapDB->NumRecords(); i++) {
		area = g_theMapDB->Get(i)->GetWidth() * g_theMapDB->Get(i)->GetHeight();
		if(closestArea < 0 || abs(area - actualarea) < closestArea) {
			closestArea = abs(area - actualarea);
			closestRec = i;
		}
	}
	if(closestRec < 0)
		closestRec = 0;
	return g_theMapDB->Get(closestRec);
}

double *worldutils_CreateSettings(const MapRecord *mapRec, sint32 level, sint32 &numSettings)
{
	if(level < 0) {
		level = 0;
	} else if(level >= mapRec->GetNumSettings()) {
		level = mapRec->GetNumSettings();
	}

	#define k_NUM_MAP_VARS 14 // Number of tokens which go into the output array
	numSettings = k_NUM_MAP_VARS;

	double* settings = new double[k_NUM_MAP_VARS];

	const MapRecord::Settings* mapSetRec = mapRec->GetSettings(level);

	settings[ 0] = mapSetRec->GetNumContinents();
	settings[ 1] = mapSetRec->GetMinWidth();
	settings[ 2] = mapSetRec->GetMinHeight();
	settings[ 3] = mapSetRec->GetMaxWidth();
	settings[ 4] = mapSetRec->GetMaxHeight();
	settings[ 5] = mapSetRec->GetBumpSize();
	settings[ 6] = mapSetRec->GetMinSubcontinents();
	settings[ 7] = mapSetRec->GetMaxSubcontinents();
	settings[ 8] = mapSetRec->GetSubcontinentSize();
	settings[ 9] = mapSetRec->GetSubcontinentHeight();
	settings[10] = mapSetRec->GetBorderSize();
	settings[11] = mapSetRec->GetFaultLines();
	settings[12] = mapSetRec->GetFaultLength();
	settings[13] = mapSetRec->GetNSBorderSize();

	return settings;
}

void worldutils_DeleteSettings(double* settings){ delete[] settings;};