










#include "c3.h"

#include "BuildingRecord.h"
#include "StrDB.h"

#include "aui_stringtable.h"

#include "ns_improvements.h"


ns_Improvements *g_nsImprovements = NULL;


extern StringDB *g_theStringDB;



ns_Improvements::ns_Improvements()
{
	Assert( g_nsImprovements == NULL );
	if ( !g_nsImprovements )
	{
		sint32 numImprovements = g_theBuildingDB->NumRecords();

		
		Assert( numImprovements <= k_IMPROVEMENTS_MAX );
		if ( numImprovements > k_IMPROVEMENTS_MAX )
			numImprovements = k_IMPROVEMENTS_MAX;

		AUI_ERRCODE errcode;
		m_stringtable = new aui_StringTable( &errcode, numImprovements );
		Assert( AUI_NEWOK(m_stringtable,errcode) );
		if ( !AUI_NEWOK(m_stringtable,errcode) ) return;

		for ( sint32 i = 0; i < numImprovements; i++ )
		{
			
			StringId stringNum = g_theBuildingDB->GetName(i);
			const MBCHAR *name = g_theStringDB->GetNameStr( stringNum );

			m_stringtable->SetString( name, i );
		}

		g_nsImprovements = this;
	}
}



ns_Improvements::~ns_Improvements()
{
	if ( m_stringtable )
	{
		delete m_stringtable;
		m_stringtable = NULL;
	}

	g_nsImprovements = NULL;
}
