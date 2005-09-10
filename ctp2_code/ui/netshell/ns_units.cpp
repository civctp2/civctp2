//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer units list
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

#include "StrDB.h"

#include "aui_stringtable.h"

#include "ns_units.h"
#include "UnitRecord.h"


ns_Units *g_nsUnits = NULL;


extern StringDB *g_theStringDB;



ns_Units::ns_Units()
{
	Assert( g_nsUnits == NULL );
	if ( !g_nsUnits )
	{
		sint32 numUnits = g_theUnitDB->NumRecords();

		
		Assert( numUnits <= k_UNITS_MAX );
		if ( numUnits > k_UNITS_MAX )
			numUnits = k_UNITS_MAX;

		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_noIndex = new sint32[ numUnits ];
		m_stringtable = new aui_StringTable( &errcode, numUnits );
		Assert( AUI_NEWOK(m_stringtable,errcode) );
		if ( !AUI_NEWOK(m_stringtable,errcode) ) return;

		for ( sint32 i = 0; i < numUnits; i++ )
		{
			StringId stringNum = g_theUnitDB->GetName( i );
			const MBCHAR *name = g_theStringDB->GetNameStr( stringNum );

			m_stringtable->SetString( name, i );

			m_noIndex[ i ] = g_theUnitDB->Get( i )->GetNoIndex();
		}

		g_nsUnits = this;
	}
}



ns_Units::~ns_Units()
{
	if ( m_stringtable )
	{
		delete m_stringtable;
		m_stringtable = NULL;
	}

	if ( m_noIndex )
	{
		delete m_noIndex;
		m_noIndex = NULL;
	}

	g_nsUnits = NULL;
}
