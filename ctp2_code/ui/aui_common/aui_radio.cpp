










#include "c3.h"
#include "aui_window.h"
#include "aui_action.h"

#include "aui_radio.h"



aui_Radio::aui_Radio(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Switch(
		retval,
		id,
		ldlBlock,
		ActionFunc,
		cookie )
{
}



aui_Radio::aui_Radio(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie,
	sint32 state,
	sint32 numStates )
	:
	aui_ImageBase( numStates ),
	aui_TextBase( NULL ),
	aui_Switch(
		retval,
		id,
		x,
		y,
		width,
		height,
		ActionFunc,
		cookie,
		state,
		numStates )
{
}



void aui_Radio::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == this )
		{
			
			ReleaseMouseOwnership();

			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			
			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

			
			
			if ( !HandleGameSpecificLeftClick( this ) )
			if ( 0 < m_state && m_state < m_numStates - 1 )
				SetState( Mod(m_state+1,m_numStates) );
			else
				SetState( 1 );

			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;

			
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		}
		else
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


void aui_Radio::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		HandleGameSpecificRightClick((void *)this);
		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRDropOutside( mouseData );
	}
}

