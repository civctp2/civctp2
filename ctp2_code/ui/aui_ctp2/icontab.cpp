











#include "c3.h"

#include "aui.h"
#include "pattern.h"
#include "icon.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_tabgroup.h"

#include "primitives.h"

#include "icontab.h"


IconTab::IconTab(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 paneWidth,
	sint32 paneHeight,
	MBCHAR *pattern,
	Icon *icon,
	uint16 color,
	ControlActionCallback *ActionFunc,
	void *cookie,
	BOOL selected )
:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Tab( retval, id, x, y, width, height, paneWidth, paneHeight, ActionFunc, cookie, selected ),
	PatternBase( pattern ),
	m_icon( icon ),
	m_color ( color )
{
}


AUI_ERRCODE IconTab::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	RECT center;
	center.left = (m_width - m_icon->TheSurface()->Width()) / 2;
	center.top = (m_height - m_icon->TheSurface()->Height()) / 2;
	center.right = center.left + m_icon->TheSurface()->Width();
	center.bottom = center.top + m_icon->TheSurface()->Height();
	OffsetRect( &center, rect.left, rect.top );

	
	if ( m_pattern )
		m_pattern->Draw( surface, &rect );

	
	AUI_TABGROUP_ALIGNMENT a = ((aui_TabGroup *)m_parent)->
		GetTabGroupAlignment();

	if ( IsOn() )
	{
		
		RECT offset = { center.left+1, center.top+1, center.right+1, center.bottom+1 };
		m_icon->Draw( surface, &offset, m_color );

		primitives_BevelTabSelected16( surface, &rect, 2, 0, 16, 16, a );
	}
	else
	{
		m_icon->Draw( surface, &center, m_color );

		primitives_BevelTabDeselected16( surface, &rect, 2, 0, 16, 16, a );
	}

	if ( IsActive() )
	{
		
		if ( IsOn() )
			primitives_BevelTabSelected16( surface, &rect, 2, 0, 16, 16, a );
		else
			primitives_BevelTabDeselected16( surface, &rect, 2, 0, 16, 16, a );
	}
	else
	{
		
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
