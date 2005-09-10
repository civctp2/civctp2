//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface header
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
#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_listbox.h"
#include "aui_switch.h"
#include "aui_ldl.h"

#include "aui_header.h"



aui_Header::aui_Header(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_SwitchGroup( retval, id, ldlBlock ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateSwitches( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Header::aui_Header(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_SwitchGroup( retval, id, x, y, width, height ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateSwitches();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Header::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}



AUI_ERRCODE aui_Header::InitCommon( void )
{
	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Header::CreateSwitches( MBCHAR *ldlBlock )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	
	aui_Ldl *theLdl = g_ui->GetLdl();
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( ldlBlock )
	{
		sint32 i = 0;
		do
		{
			sprintf( block, "%s.%s%d", ldlBlock, k_AUI_HEADER_LDL_SWITCH, i );

			if ( !theLdl->GetLdl()->FindDataBlock( block ) )
				break;

			aui_Switch *theSwitch = new aui_Switch(
				&errcode,
				aui_UniqueId(),
				block );
			Assert( AUI_NEWOK(theSwitch,errcode) );
			if ( !AUI_NEWOK(theSwitch,errcode) )
				return AUI_ERRCODE_MEMALLOCFAILED;

			aui_Action *action = new aui_HeaderSwitchAction( i );
			Assert( action != NULL );
			if ( !action ) return AUI_ERRCODE_MEMALLOCFAILED;

			theSwitch->SetAction( action );

			AddChild( theSwitch );

			i++;

		} while ( 1 );
	}

	return AUI_ERRCODE_OK;
}



aui_Header::~aui_Header()
{
	
	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Switch *theSwitch = (aui_Switch *)m_childList->GetNext( position );
		aui_Action *action = theSwitch->GetAction();
		if ( action )
			delete action;
		delete theSwitch;
	}
}



AUI_ERRCODE aui_Header::AddChild( aui_Region *child )
{
	AUI_ERRCODE errcode = aui_SwitchGroup::AddChild( child );

	CalculateDimensions();
	RepositionSwitches();

	return errcode;
}



AUI_ERRCODE aui_Header::RemoveChild( uint32 itemId )
{
	AUI_ERRCODE errcode = aui_SwitchGroup::RemoveChild( itemId );

	CalculateDimensions();
	RepositionSwitches();

	return errcode;
}



AUI_ERRCODE aui_Header::CalculateDimensions( void )
{
	
	m_width = 0;
	m_height = 0;

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Switch *theSwitch = (aui_Switch *)m_childList->GetNext( position );

		if ( theSwitch->Height() > m_height )
			m_height = theSwitch->Height();
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Header::RepositionSwitches( void )
{
	sint32 x = 0;

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Switch *theSwitch = (aui_Switch *)m_childList->GetNext( position );

		
		theSwitch->Move( x, m_height - theSwitch->Height() );

		x += theSwitch->Width();
	}

	return AUI_ERRCODE_OK;
}








void aui_HeaderSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	
	
	
	aui_ListBox *listbox = (aui_ListBox *)control->GetParent()->GetParent();
	if ( !listbox ) return;

	
	switch ( ((aui_Switch *)control)->ExtractState( data ) )
	{
	
	default:
	case 1:
		
		listbox->SortByColumn( m_column, TRUE );
		break;

	
	
	case 2:
		
		listbox->SortByColumn( m_column, FALSE );
		break;
	}
}
