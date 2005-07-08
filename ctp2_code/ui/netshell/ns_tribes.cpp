










#include "c3.h"

#include "CivilisationDB.h"
#include "StrDB.h"

#include "aui_uniqueid.h"

#include "ns_item.h"

#include "ns_tribes.h"


ns_Tribes *g_nsTribes = NULL;


extern CivilisationDatabase *g_theCivilisationDB;
extern StringDB *g_theStringDB;



ns_Tribes::ns_Tribes()
{
	Assert( g_nsTribes == NULL );
	if ( !g_nsTribes )
	{
		sint32 numCivs = g_theCivilisationDB->GetCivilisations();

		
		Assert( numCivs <= k_TRIBES_MAX - 1 );
		if ( numCivs > k_TRIBES_MAX - 1 )
			numCivs = k_TRIBES_MAX - 1;

		AUI_ERRCODE errcode;
		m_stringtable = new aui_StringTable( &errcode, numCivs + 1 );
		Assert( AUI_NEWOK(m_stringtable,errcode) );
		if ( !AUI_NEWOK(m_stringtable,errcode) ) return;

		
		
		m_stringtable->SetString( (char *)g_theStringDB->GetNameStr("str_tbl_ldl_None"), 0 );

		for ( sint32 i = 0; i < numCivs; i++ )
		{
			StringId stringNum =
				g_theCivilisationDB->GetSingularCivName( (CIV_INDEX)i );
			const MBCHAR *name = g_theStringDB->GetNameStr( stringNum );

			m_stringtable->SetString( name, i + 1 );
		}

		g_nsTribes = this;
	}
}



ns_Tribes::~ns_Tribes()
{
	if ( m_stringtable )
	{
		delete m_stringtable;
		m_stringtable = NULL;
	}

	g_nsTribes = NULL;
}



ns_TribesDropDown::ns_TribesDropDown(
	AUI_ERRCODE *retval,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	c3_DropDown( retval, aui_UniqueId(), ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	
	Assert( g_nsTribes );
	if ( !g_nsTribes )
	{
		*retval = AUI_ERRCODE_HACK;
		return;
	}

	sint32 numCivs = g_nsTribes->GetStrings()->GetNumStrings();
	for ( sint32 i = 0; i < numCivs; i++ )
	{
		MBCHAR *name = g_nsTribes->GetStrings()->GetString( i );

		ns_ListItem *item = new ns_ListItem(
			retval,
			name,
			"listitems.tribeitem" );
		Assert( AUI_NEWOK(item,*retval) );
		if ( !AUI_NEWOK(item,*retval) ) return;

		AddItem( (aui_Item *)item );
	}
}



ns_TribesDropDown::~ns_TribesDropDown()
{
	
	m_listBox->RemoveItems( TRUE );
}
