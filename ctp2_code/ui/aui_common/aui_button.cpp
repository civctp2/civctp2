










#include "c3.h"
#include "aui_ui.h"
#include "aui_action.h"
#include "aui_window.h"

#include "aui_button.h"

#include "gamesounds.h"
#include "soundmanager.h"

extern SoundManager		*g_soundManager;


aui_Button::aui_Button(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Button::aui_Button(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie ),
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Button::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}



AUI_ERRCODE aui_Button::InitCommon( void )
{
	m_isRepeating = FALSE;
	m_repeatCount = 0;
	m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;
	m_keyboardAction = AUI_BUTTON_ACTION_EXECUTE;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Button::ResetThis( void )
{
	m_isRepeating = FALSE;
	m_repeatCount = 0;

	return aui_Control::ResetThis();
}



AUI_ERRCODE aui_Button::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawThisStateImage(
		IsDown() ? 1 : 0,
		surface,
		&rect );

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}








void aui_Button::KeyboardCallback( aui_KeyboardEvent *keyboardData )
{





	
	
	if ( !GetMouseOwnership() && ( keyboardData->key == AUI_KEYBOARD_KEY_RETURN)) {
		
		if ( keyboardData->down && !m_keyboardEvent.down )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

			
			HideTipWindow();

			
			m_attributes |= k_CONTROL_ATTRIBUTE_DOWN;
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_KEYSTATECHANGE;

			
			m_isRepeating = TRUE;
			m_repeatCount = 0;
			m_startWaitTime = keyboardData->time;
			if ( m_ActionFunc )
				m_ActionFunc( this, AUI_BUTTON_ACTION_PRESS, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, AUI_BUTTON_ACTION_PRESS, 0 );
		}

		
		else if ( !keyboardData->down && m_keyboardEvent.down )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			
			m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;
			m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;

			
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_KEYSTATECHANGE;

			if(GetAttracting()) {
				SetAttract(false, 0);
			}

			
			m_isRepeating = FALSE;
			if ( m_ActionFunc )
				m_ActionFunc( this, AUI_BUTTON_ACTION_EXECUTE, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, AUI_BUTTON_ACTION_EXECUTE, 0 );
		}
	}
}



void aui_Button::PostChildrenCallback( aui_MouseEvent *mouseData )
{
	if ( m_isRepeating && mouseData->time - m_startWaitTime > m_timeOut )
	{
		if ( mouseData->time - m_lastRepeatTime > m_repeatTime )
		{
			m_lastRepeatTime = mouseData->time;

			if ( m_ActionFunc )
				m_ActionFunc(
					this,
					AUI_BUTTON_ACTION_PRESS,
					(uint32)++m_repeatCount,
					m_cookie );
			else if ( m_action )
				m_action->Execute(
					this,
					AUI_BUTTON_ACTION_PRESS,
					(uint32)++m_repeatCount );
		}
	}
}



void aui_Button::MouseLDragOver( aui_MouseEvent *mouseData )
{
	
	aui_Control::MouseLDragOver(mouseData);

	if (IsDisabled()) return;

	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == this )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			m_attributes |= k_CONTROL_ATTRIBUTE_DOWN;
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGOVER;

			
			m_isRepeating = TRUE;
		}
	}
	else
		MouseLDragOutside( mouseData );
}



void aui_Button::MouseLDragAway( aui_MouseEvent *mouseData )
{
	
	aui_Control::MouseLDragAway(mouseData);

	if (IsDisabled()) return;

	
	if ( GetMouseOwnership() == this )
	{
		PlaySound( AUI_SOUNDBASE_SOUND_DISENGAGE );

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGAWAY;

		
		m_isRepeating = FALSE;
		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_BUTTON_ACTION_RELEASE, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_BUTTON_ACTION_RELEASE, 0 );
	}
}



void aui_Button::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) {
		if (g_soundManager) {
			g_soundManager->AddSound(SOUNDTYPE_SFX, 0, 
					gamesounds_GetGameSoundID(GAMESOUNDS_GENERALFAIL), 0, 0);
		}
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

		
		m_attributes |= k_CONTROL_ATTRIBUTE_DOWN;
		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;

		
		m_isRepeating = TRUE;
		m_repeatCount = 0;
		m_startWaitTime = mouseData->time;
		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_BUTTON_ACTION_PRESS, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_BUTTON_ACTION_PRESS, 0 );
	}
	else
		MouseLGrabOutside( mouseData );
}



void aui_Button::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) {
		g_soundManager->AddGameSound(GAMESOUNDS_DISABLED);
		return;
	}
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == this )
		{
			
			ReleaseMouseOwnership();

			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );
				
			
			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

			
			m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;

			
			
			
			m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;

			
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;

			
			m_isRepeating = FALSE;

			if(GetAttracting()) {
				SetAttract(false, 0);
			}

			if ( !HandleGameSpecificLeftClick( this ) )
			if ( m_ActionFunc )
				m_ActionFunc( this, AUI_BUTTON_ACTION_EXECUTE, 0, m_cookie );
			else if ( m_action )
				m_action->Execute( this, AUI_BUTTON_ACTION_EXECUTE, 0 );
		}
		else
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

			
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		}
	}
	else
		MouseLDropOutside( mouseData );
}



void aui_Button::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if (IsDisabled()) return;

	
	if ( GetMouseOwnership() == this )
	{
		
		ReleaseMouseOwnership();

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;
	}
}


void aui_Button::MouseRDropInside( aui_MouseEvent *mouseData )
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

