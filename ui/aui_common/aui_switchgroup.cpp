










#include "c3.h"
#include "aui_switch.h"
#include "aui_window.h"

#include "aui_switchgroup.h"



aui_SwitchGroup::aui_SwitchGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_Control( retval, id, ldlBlock ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_SwitchGroup::aui_SwitchGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height )
	:
	aui_Control( retval, id, x, y, width, height ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_SwitchGroup::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}



AUI_ERRCODE aui_SwitchGroup::InitCommon( void )
{
	m_whichWasSelected = 0,
	m_whichIsSelected = 0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_SwitchGroup::AddChild( aui_Region *child )
{
	AUI_ERRCODE errcode = aui_Control::AddChild( child );
	if ( errcode == AUI_ERRCODE_OK )
		MakeSureOnlyOneIsSelected();

	return errcode;
}



AUI_ERRCODE aui_SwitchGroup::RemoveChild( uint32 switchId )
{
	if ( m_whichIsSelected == switchId ) m_whichIsSelected = 0;

	return aui_Control::RemoveChild( switchId );
}


sint32 aui_SwitchGroup::WhichIndexIsSelected( void )
{
	ListPos position = m_childList->GetHeadPosition();

	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Region *region = m_childList->GetNext( position );
		if ( region->Id() == m_whichIsSelected ) return (m_childList->L()-i);
	}

	return -1;
}


BOOL aui_SwitchGroup::MakeSureOnlyOneIsSelected( void )
{
	BOOL foundSelectedSwitch = FALSE;

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Switch *theSwitch = (aui_Switch *)m_childList->GetNext( position );

		if (theSwitch) {
			if ( theSwitch->IsSelected() )
			{
				
				if ( foundSelectedSwitch )
					
					theSwitch->SetState( 0 );
				else
				{
					foundSelectedSwitch = TRUE;
					m_whichIsSelected = theSwitch->Id();

					
					theSwitch->SetState( theSwitch->GetState() );
				}
			}
			else
				
				theSwitch->SetState( 0 );
		}
	}

	return foundSelectedSwitch;
}



void aui_SwitchGroup::PreChildrenCallback( aui_MouseEvent *input )
{
	m_whichWasSelected = m_whichIsSelected;
}



void aui_SwitchGroup::PostChildrenCallback( aui_MouseEvent *input )
{
	BOOL foundSelectedSwitch = FALSE;

	ListPos position = m_childList->GetHeadPosition();
	for ( sint32 i = m_childList->L(); i; i-- )
	{
		aui_Switch *theSwitch = (aui_Switch *)m_childList->GetNext( position );

		if ( theSwitch->IsSelected() )
		{
			foundSelectedSwitch = TRUE;

			if ( theSwitch->Id() != m_whichWasSelected )
			{
				
				aui_Switch *otherSwitch = m_whichWasSelected ?
					(aui_Switch *)GetChild( m_whichWasSelected ) :
					NULL;

				if ( otherSwitch )
					
					otherSwitch->SetState( 0 );

				m_whichIsSelected = theSwitch->Id();

				
				m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

				m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
				break;
			}
		}
	}

	if ( !foundSelectedSwitch ) m_whichIsSelected = 0;
}
