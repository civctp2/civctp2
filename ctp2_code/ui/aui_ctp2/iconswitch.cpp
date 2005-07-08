











#include "c3.h"

#include "aui.h"
#include "pattern.h"
#include "icon.h"
#include "aui_window.h"
#include "aui_surface.h"

#include "primitives.h"

#include "iconswitch.h"


IconSwitch::IconSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	MBCHAR *pattern,
	Icon *icon,
	uint16 color,
	ControlActionCallback *ActionFunc,
	void *cookie,
	BOOL selected )
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_Switch( retval, id, x, y, icon->TheSurface()->Width(), icon->TheSurface()->Height(), ActionFunc, cookie, selected ),
	PatternBase( pattern ),
	m_icon( icon ),
	m_color( color )
{
}


AUI_ERRCODE IconSwitch::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern )
		m_pattern->Draw( surface, &rect );

	
	if ( IsOn() )
	{
		
		RECT offset = { rect.left+1, rect.top+1, rect.right+1, rect.bottom+1 };
		if ( m_icon )
			m_icon->Draw( surface, &offset, m_color );
			
	}
	else
	{
		if ( m_icon )
			m_icon->Draw( surface, &rect, m_color );
	}

	if ( IsOn() )
	{
		
		primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
	}
	else
	{
		
		primitives_BevelRect16( surface, &rect, 1, 0, 16, 16 );
	}

	if ( IsActive() )
	{
		
		if ( IsOn() )
		{
			
			primitives_BevelRect16( surface, &rect, 1, 1, 16, 16 );
		}
		else
		{
			
			primitives_BevelRect16( surface, &rect, 1, 0, 16, 16 );
		}
	}
	else
	{
		
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
