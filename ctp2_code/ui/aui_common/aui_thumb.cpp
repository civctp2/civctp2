










#include "c3.h"
#include "aui_ui.h"
#include "aui_blitter.h"
#include "aui_window.h"
#include "aui_action.h"

#include "aui_thumb.h"



aui_Thumb::aui_Thumb(
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

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Thumb::aui_Thumb(
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



AUI_ERRCODE aui_Thumb::InitCommon( void )
{
	m_grabPoint.x = m_grabPoint.y = 0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Thumb::Reposition( sint32 x, sint32 y )
{
	
	Assert( m_parent->Width() >= m_width );
	Assert( m_parent->Height() >= m_height );
	if ( m_parent->Width() < m_width || m_parent->Height() < m_height )
		return AUI_ERRCODE_INVALIDDIMENSION;

	
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;

	sint32 limit = m_parent->Width() - m_width;
	if ( x > limit ) x = limit;
	limit = m_parent->Height() - m_height;
	if ( y > limit ) y = limit;

	
	Move( x, y );

	return AUI_ERRCODE_OK;
}








void aui_Thumb::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	
	if ( GetMouseOwnership() == this )
	{
		sint32 x = mouseData->position.x - m_grabPoint.x;
		sint32 y = mouseData->position.y - m_grabPoint.y;

		Reposition( x, y );

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGAWAY;

		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_THUMB_ACTION_DRAG, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_THUMB_ACTION_DRAG, 0 );
	}
}



void aui_Thumb::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLDragAway( mouseData );
}



void aui_Thumb::MouseLDragInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLDragAway( mouseData );
}



void aui_Thumb::MouseLDragOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLDragAway( mouseData );
}



void aui_Thumb::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		
		HideTipWindow();

		
		SetMouseOwnership();
		SetKeyboardFocus();

		m_grabPoint.x = mouseData->position.x - m_x;
		m_grabPoint.y = mouseData->position.y - m_y;

		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;

		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_THUMB_ACTION_GRAB, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_THUMB_ACTION_GRAB, 0 );
	}
	else
		MouseLGrabOutside( mouseData );
}



void aui_Thumb::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		
		if ( GetMouseOwnership() == this )
		{
			PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

			
			ReleaseMouseOwnership();
			m_grabPoint.x = m_grabPoint.y = 0;
		}
		else
		{
			PlaySound( AUI_SOUNDBASE_SOUND_ACTIVATE );

			
			m_attributes |= k_CONTROL_ATTRIBUTE_ACTIVE;
			m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		}

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		if ( !HandleGameSpecificLeftClick( this ) )
		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_THUMB_ACTION_DROP, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_THUMB_ACTION_DROP, 0 );
	}
	else
		MouseLDropOutside( mouseData );
}


void aui_Thumb::MouseRDropInside( aui_MouseEvent *mouseData )
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



void aui_Thumb::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	
	if ( GetMouseOwnership() == this )
	{
		
		ReleaseMouseOwnership();
		m_grabPoint.x = m_grabPoint.y = 0;

		
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		
		m_attributes &= ~k_CONTROL_ATTRIBUTE_ACTIVE;
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPOUTSIDE;

		
		if ( m_parent != (aui_Region *)m_window )
			((aui_Control*)m_parent)->SetMouseOwnership();

		if ( m_ActionFunc )
			m_ActionFunc( this, AUI_THUMB_ACTION_DROP, 0, m_cookie );
		else if ( m_action )
			m_action->Execute( this, AUI_THUMB_ACTION_DROP, 0 );
	}
}
