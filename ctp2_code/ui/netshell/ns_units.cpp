//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer units list
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "ns_units.h"

#include "aui_stringtable.h"
#include "StrDB.h"              // g_theStringDB
#include "UnitRecord.h"         // g_theUnitDB

ns_Units *  g_nsUnits = NULL;

ns_Units::ns_Units()
:
	m_noIndex       (NULL),
    m_stringtable   (NULL)
{
	Assert(g_theUnitDB->NumRecords() <= k_UNITS_MAX);
    sint32      numUnits    = 
        std::min<sint32>(k_UNITS_MAX, g_theUnitDB->NumRecords());
	m_noIndex = new sint32[numUnits];

	AUI_ERRCODE errcode     = AUI_ERRCODE_OK;
	m_stringtable = new aui_StringTable(&errcode, numUnits);
	Assert(AUI_NEWOK(m_stringtable, errcode));
	if (!AUI_NEWOK(m_stringtable,errcode)) return;

	for (sint32 i = 0; i < numUnits; i++)
	{
		StringId stringNum = g_theUnitDB->GetName(i);
		m_stringtable->SetString(g_theStringDB->GetNameStr(stringNum), i);

		m_noIndex[i] = g_theUnitDB->Get(i)->GetNoIndex();
	}
}

ns_Units::~ns_Units()
{
	delete m_stringtable;
	delete [] m_noIndex;
}
