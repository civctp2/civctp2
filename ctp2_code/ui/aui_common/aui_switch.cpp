#include "c3.h"
#include "aui_switch.h"

#include "aui_ui.h"
#include "aui_window.h"
#include "aui_action.h"
#include "aui_blitter.h"
#include "aui_ldl.h"

#include "soundmanager.h"
#include "gamesounds.h"

#include "ldl_data.hpp"

aui_Switch::aui_Switch(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie )
{
	m_drawFunc			= NULL;
	m_drawCookie		= NULL;
	m_drawCallbackExclusive	= true;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


aui_Switch::aui_Switch(
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
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie )
{
	m_drawFunc			= NULL;
	m_drawCookie		= NULL;
	m_drawCallbackExclusive	= true;

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon( state, numStates );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE aui_Switch::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	sint32 state = block->GetInt( k_AUI_SWITCH_LDL_STATE );
	sint32 numStates = block->GetInt( k_AUI_SWITCH_LDL_NUMSTATES );

	AUI_ERRCODE errcode = InitCommon( state, numStates );
	Assert( AUI_SUCCESS(errcode) );
	return errcode;
}


AUI_ERRCODE aui_Switch::InitCommon( sint32 state, sint32 numStates )
{
	SetNumStates( numStates ? numStates : k_AUI_SWITCH_DEFAULTNUMSTATES );
	SetState( state );

	return AUI_ERRCODE_OK;
}


sint32 aui_Switch::SetState( sint32 state )
{
	sint32 prevState = m_state;
	m_state = state;

	if (m_state)
	{

		if ( m_numStates == 2 ) m_state = 1;

		m_attributes |= k_CONTROL_ATTRIBUTE_ON;
	}
	else
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ON;

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


sint32 aui_Switch::SetNumStates( sint32 numStates )
{
	Assert( numStates >= 2 );
	if ( numStates < 2 ) numStates = 2;

	sint32 prevNumStates = m_numStates;
	m_numStates = numStates;
	return prevNumStates;
}


AUI_ERRCODE aui_Switch::ResetThis( void )
{
	if ( GetState() )
		m_attributes |= k_CONTROL_ATTRIBUTE_ON;
	else
		m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;

	return aui_Control::ResetThis();
}


AUI_ERRCODE aui_Switch::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if((m_drawFunc)&&(m_drawCallbackExclusive)) {
		if (m_window)
			if ( surface == m_window->TheSurface() )
				m_window->AddDirtyRect( &rect );
		return m_drawFunc(this, surface, rect, m_drawCookie);
	}

	if((m_drawFunc)&&(!m_drawCallbackExclusive))
		m_drawFunc(this, surface, rect, m_drawCookie);

	DrawThisStateImage(
		m_state ? m_state : IsOn() ? 1 : 0,
		surface,
		&rect );






	RECT down = rect;
	down.left += 2;
	down.top += 2;

	DrawThisText(
		surface,
		IsOn() ? &down : &rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}






void aui_Switch::SetDrawCallbackAndCookie(SwitchDrawCallback *func, void *cookie,bool exclusive)
{
	m_drawFunc		= func;
	m_drawCookie	= cookie;
	m_drawCallbackExclusive	= exclusive;
}


void aui_Switch::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		if ( GetMouseOwnership() == this )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			if ( !m_state ) m_attributes |= k_CONTROL_ATTRIBUTE_ON;
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGOVER;

			if ( m_ActionFunc )
				m_ActionFunc(
					this,
					AUI_SWITCH_ACTION_PRESS,
					(uint32)m_state,
					m_cookie );
			else if ( m_action )
				m_action->Execute(
					this,
					AUI_SWITCH_ACTION_PRESS,
					(uint32)m_state );
		}
	}
	else
		MouseLDragOutside( mouseData );
}


void aui_Switch::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;

	if ( GetMouseOwnership() == this )
	{
		PlaySound( AUI_SOUNDBASE_SOUND_DISENGAGE );

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		if ( !m_state ) m_attributes &= ~k_CONTROL_ATTRIBUTE_ON;
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGAWAY;

		if ( m_ActionFunc )
			m_ActionFunc(
				this,
				AUI_SWITCH_ACTION_RELEASE,
				(uint32)m_state,
				m_cookie );
		else if ( m_action )
			m_action->Execute(
				this,
				AUI_SWITCH_ACTION_RELEASE,
				(uint32)m_state );
	}
}


void aui_Switch::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) {
		g_soundManager->AddGameSound(GAMESOUNDS_DISABLED);
		return;
	}
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		HideTipWindow();

		SetMouseOwnership();
		SetKeyboardFocus();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		m_attributes |= k_CONTROL_ATTRIBUTE_ON;
		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;

		if ( m_ActionFunc )
			m_ActionFunc(
				this,
				AUI_SWITCH_ACTION_PRESS,
				(uint32)m_state,
				m_cookie );
		else if ( m_action )
			m_action->Execute(
				this,
				AUI_SWITCH_ACTION_PRESS,
				(uint32)m_state );
	}
}


void aui_Switch::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		if ( GetMouseOwnership() == this )
		{

			ReleaseMouseOwnership();

			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

			if ( !HandleGameSpecificLeftClick( this ) )
			SetState( Mod(m_state+1,m_numStates) );
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
				m_ActionFunc(
					this,
					AUI_SWITCH_ACTION_NULL,
					(uint32)m_state,
					m_cookie );
			else if ( m_action )
				m_action->Execute(
					this,
					AUI_SWITCH_ACTION_NULL,
					(uint32)m_state );
		}
	}
	else
		MouseLDropOutside( mouseData );
}


void aui_Switch::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;

	if ( GetMouseOwnership() == this )
	{

		ReleaseMouseOwnership();

		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;

		if ( m_ActionFunc )
			m_ActionFunc(
				this,
				AUI_SWITCH_ACTION_NULL,
				(uint32)m_state,
				m_cookie );
		else if ( m_action )
			m_action->Execute(
				this,
				AUI_SWITCH_ACTION_NULL,
				(uint32)m_state );
	}
}

void aui_Switch::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		HandleGameSpecificRightClick((void *)this);
		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRDropOutside( mouseData );
	}
}
