//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer building list
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
#include "ns_improvements.h"

#include "aui_stringtable.h"
#include "BuildingRecord.h"     // g_theBuildingDB
#include "StrDB.h"              // g_theStringDB

ns_Improvements * g_nsImprovements = NULL;

ns_Improvements::ns_Improvements()
:
    m_stringtable   (NULL)
{
	Assert(g_theBuildingDB->NumRecords() <= k_IMPROVEMENTS_MAX );
	sint32      numImprovements = 
        std::min<sint32>(k_IMPROVEMENTS_MAX, g_theBuildingDB->NumRecords());

	AUI_ERRCODE errcode         = AUI_ERRCODE_OK;
	m_stringtable = new aui_StringTable( &errcode, numImprovements );
	Assert( AUI_NEWOK(m_stringtable,errcode) );
	if ( !AUI_NEWOK(m_stringtable,errcode) ) return;

	for ( sint32 i = 0; i < numImprovements; i++ )
	{
		StringId stringNum = g_theBuildingDB->GetName(i);
		m_stringtable->SetString(g_theStringDB->GetNameStr(stringNum), i);
	}
}

ns_Improvements::~ns_Improvements()
{
	delete m_stringtable;
}
