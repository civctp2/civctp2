//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer wonders list
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

#include "WonderRecord.h"
#include "StrDB.h"

#include "aui_stringtable.h"

#include "ns_wonders.h"

ns_Wonders *g_nsWonders = NULL;


ns_Wonders::ns_Wonders()
{
	Assert( g_nsWonders == NULL );
	if ( !g_nsWonders )
	{
		sint32 numWonders = g_theWonderDB->NumRecords();

		Assert( numWonders <= k_WONDERS_MAX );
		if ( numWonders > k_WONDERS_MAX )
			numWonders = k_WONDERS_MAX;

		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		m_stringtable = new aui_StringTable( &errcode, numWonders );
		Assert( AUI_NEWOK(m_stringtable,errcode) );
		if ( !AUI_NEWOK(m_stringtable,errcode) ) return;

		for ( sint32 i = 0; i < numWonders; i++ )
		{
			StringId stringNum = g_theWonderDB->Get(i)->m_name;
			const MBCHAR *name = g_theStringDB->GetNameStr( stringNum );

			m_stringtable->SetString( name, i );
		}

		g_nsWonders = this;
	}
}


ns_Wonders::~ns_Wonders()
{
	if ( m_stringtable )
	{
		delete m_stringtable;
		m_stringtable = NULL;
	}

	g_nsWonders = NULL;
}
