//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface tab
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
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "aui_action.h"
#include "aui_static.h"

#include "aui_tab.h"



aui_Tab::aui_Tab(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Radio( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Tab::aui_Tab(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 paneWidth,
	sint32 paneHeight,
	ControlActionCallback *ActionFunc,
	void *cookie,
	sint32 state,
	sint32 numStates )
	:
	aui_ImageBase( numStates ),
	aui_TextBase( NULL ),
	aui_Radio( retval, id, x, y, width, height, ActionFunc, cookie, state, numStates )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( paneWidth, paneHeight );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

	

AUI_ERRCODE aui_Tab::InitCommon( MBCHAR *ldlBlock )
{
	
	static MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.%s", ldlBlock, "pane" );

	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_pane = new aui_Static( &errcode, aui_UniqueId(), block );
	Assert( AUI_NEWOK(m_pane,errcode) );
	if ( !AUI_NEWOK(m_pane,errcode) ) return AUI_ERRCODE_OK; // Returnin OK?

	m_pane->SetBlindness( TRUE );

	
	m_pane->Move( 0, m_height );

	
	AddChild( m_pane );

	return AUI_ERRCODE_OK; // Why not errcode?
}



AUI_ERRCODE aui_Tab::InitCommon( sint32 paneWidth, sint32 paneHeight )
{
	
	Assert( paneWidth >= m_width );
	if ( paneWidth < m_width ) paneWidth = m_width;

	
	
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	m_pane = new aui_Static(
		&errcode,
		aui_UniqueId(),
		0,
		m_height,
		paneWidth,
		paneHeight );
	Assert( AUI_NEWOK(m_pane,errcode) );
	if ( !AUI_NEWOK(m_pane,errcode) ) return AUI_ERRCODE_OK;

	m_pane->SetBlindness( TRUE );

	
	AddChild( m_pane );

	return AUI_ERRCODE_OK;
}



aui_Tab::~aui_Tab()
{
	if ( m_pane )
	{
		delete m_pane;
		m_pane = NULL;
	}
}



sint32 aui_Tab::SetState( sint32 state )
{
	sint32 prevState = m_state;

	
	if ((m_state = state))
	{
		
		
		if ( m_numStates == 2 ) m_state = 1;

		m_attributes |= k_CONTROL_ATTRIBUTE_ON;
		if ( m_pane->IsHidden() ) m_pane->Show();
	}
	else
	{
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ON;
		if ( !m_pane->IsHidden() ) m_pane->Hide();
	}

	
	m_state = Mod(m_state,m_numStates);

	if ( m_ActionFunc )
		m_ActionFunc(
			this,
			m_state ? AUI_SWITCH_ACTION_ON : AUI_SWITCH_ACTION_OFF,
			(uint32)m_state,
			m_cookie );
	else if ( m_action )
		m_action->Execute(
			this,
			m_state ? AUI_SWITCH_ACTION_ON : AUI_SWITCH_ACTION_OFF,
			(uint32)m_state );

	
	if ( prevState != m_state )
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return prevState;
}



AUI_ERRCODE aui_Tab::AddPaneControl( aui_Control *control )
{
	m_pane->AddChild( control );

	
	m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Tab::RemovePaneControl( uint32 controlId )
{
	AUI_ERRCODE errcode = m_pane->RemoveChild( controlId );

	
	if ( errcode == AUI_ERRCODE_OK )
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_UPDATE;

	return errcode;
}



AUI_ERRCODE aui_Tab::Show( void )
{
	aui_Switch::Show();

	

	if ( !m_state ) m_pane->Hide();

	return AUI_ERRCODE_OK;
}








void aui_Tab::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == NULL )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGOVER;

			if ( m_ActionFunc )
				m_ActionFunc( this, AUI_SWITCH_ACTION_NULL, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, AUI_SWITCH_ACTION_NULL, 0 );
		}
	}
	else
		MouseLDragOutside( mouseData );
}



void aui_Tab::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	
	if ( GetMouseOwnership() == NULL )
	{
		PlaySound( AUI_SOUNDBASE_SOUND_DEACTIVATE );

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGAWAY;

		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_SWITCH_ACTION_NULL, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_SWITCH_ACTION_NULL, 0 );
	}
}



void aui_Tab::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

		
		HideTipWindow();

		
		SetMouseOwnership();
		ReleaseMouseOwnership();
		SetKeyboardFocus();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		
		if ( !HandleGameSpecificLeftClick( this ) )
		if ( 0 < m_state && m_state < m_numStates - 1 )
			SetState( Mod(m_state+1,m_numStates) );
		else
			SetState( 1 );

		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;

		
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;
	}
	else
		MouseLGrabOutside( mouseData );
}


void aui_Tab::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		HandleGameSpecificRightClick((void *)this);
		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRGrabOutside( mouseData );
	}
}



void aui_Tab::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() != NULL )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;

			if ( m_ActionFunc )
				m_ActionFunc( this, AUI_SWITCH_ACTION_NULL, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, AUI_SWITCH_ACTION_NULL, 0 );
		}
	}
	else
		MouseLDropOutside( mouseData );
}
