#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "pattern.h"
#include "picture.h"

#include "primitives.h"

#include "pictureswitch.h"

PictureSwitch::PictureSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	Picture *pictureOn,
	Picture *pictureOff,
	Picture *pictureActiveOn,
	Picture *pictureActiveOff,
	ControlActionCallback *ActionFunc,
	void *cookie,
	BOOL selected )
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_Switch( retval, id, x, y, width, height, ActionFunc, cookie, selected ),
	m_pictureOn( pictureOn ),
	m_pictureOff( pictureOff ),
	m_pictureActiveOn( pictureActiveOn ),
	m_pictureActiveOff( pictureActiveOff )
{
}

AUI_ERRCODE PictureSwitch::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( IsOn() )
	{

		m_pictureOn->Draw( surface, &rect );
	}
	else
	{

		m_pictureOff->Draw( surface, &rect );
	}

	if ( IsActive() )
	{
		if ( IsOn() )
		{

			m_pictureActiveOn->Draw( surface, &rect );
		}
		else
		{

			m_pictureActiveOff->Draw( surface, &rect );
		}
	}
	else
	{

	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
