











#include "c3.h"

#include "aui.h"
#include "picture.h"
#include "aui_window.h"

#include "picturetab.h"


PictureTab::PictureTab(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 paneWidth,
	sint32 paneHeight,
	Picture *pictureOn,
	Picture *pictureOff,
	Picture *pictureActiveOn,
	Picture *pictureActiveOff,
	ControlActionCallback *ActionFunc,
	void *cookie,
	BOOL selected )
:
	aui_Tab( retval, id, x, y, width, height, paneWidth, paneHeight, ActionFunc, cookie, selected ),
	m_pictureOn( pictureOn ),
	m_pictureOff( pictureOff ),
	m_pictureActiveOn( pictureActiveOn ),
	m_pictureActiveOff( pictureActiveOff ),
	aui_TextBase(NULL),
	aui_ImageBase( (sint32)0 )
{
}


AUI_ERRCODE PictureTab::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( IsOn() )
		m_pictureOn->Draw( surface, &rect );
	else
		m_pictureOff->Draw( surface, &rect );

	if ( IsActive() )
	{
		if ( IsOn() )
			m_pictureActiveOn->Draw( surface, &rect );
		else
			m_pictureActiveOff->Draw( surface, &rect );
	}
	else
	{
		
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
