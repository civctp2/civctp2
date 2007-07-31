









#include "c3.h"

#include "aui.h"
#include "aui_action.h"
#include "aui_ldl.h"
#include "aui_window.h"
#include "aui_blitter.h"

#include "aui_Button.h"
#include "messageiconbutton.h"

#include "c3ui.h"

extern C3UI			*g_c3ui;


MessageIconButton *MessageIconButton::m_currentButton = NULL;


MessageIconButton::MessageIconButton(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_Button( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_ImageBase( ldlBlock )
{
}


void MessageIconButton::SetCurrentIconButton( MessageIconButton *button )
{
	if ( m_currentButton )
		m_currentButton->ShouldDraw( TRUE );

	m_currentButton = button;
}


AUI_ERRCODE MessageIconButton::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( this == m_currentButton ) {
		DrawImage( surface, &rect, 1,
					AUI_IMAGEBASE_SUBSTATE_STATE );
	} else {
		DrawImage( surface, &rect, 0,
					AUI_IMAGEBASE_SUBSTATE_STATE );
	}


	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

MessageIconButton::~MessageIconButton( void )
{
	ReleaseMouseOwnership();
}














































void MessageIconButton::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		HideTipWindow();

		
		SetMouseOwnership();
		SetKeyboardFocus();

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		
		m_attributes |= k_CONTROL_ATTRIBUTE_DOWN;
		m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;

		
		m_isRepeating = TRUE;
		m_repeatCount = 0;
		m_startWaitTime = mouseData->time;
		if ( m_action )
			m_action->Execute( this, AUI_BUTTON_ACTION_PRESS, 0 );
	}
	else
		MouseRGrabOutside( mouseData );
}



void MessageIconButton::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == this )
		{
			
			ReleaseMouseOwnership();

			
			m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

			
			m_attributes &= ~k_CONTROL_ATTRIBUTE_DOWN;

			
			
			
			m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;

			
			m_isRepeating = FALSE;
			
			if ( m_action )
				m_action->Execute( this, AUI_BUTTON_ACTION_EXECUTE, 1 );
		}
		else
		{

			
			if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
				m_mouseCode = AUI_ERRCODE_HANDLED;

		}
	}
	else
		MouseRDropOutside( mouseData );
}



void MessageIconButton::MouseRDropOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	
	if ( GetMouseOwnership() == this )
	{
		
		ReleaseMouseOwnership();

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

	}
}
