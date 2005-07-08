












#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "pattern.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "aui_action.h"
#include "aui_bitmapfont.h"
#include "aui_tabgroup.h"

#include "c3ui.h"

#include "primitives.h"

#include "colorset.h"
#include "textutils.h"

#include "texttab.h"

#include "SlicEngine.h"

extern ColorSet		*g_colorSet;
extern SlicEngine	*g_slicEngine;
extern C3UI			*g_c3ui;


TextTab::TextTab(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ), 
	aui_Tab( retval, id, ldlBlock, ActionFunc, cookie),
	PatternBase( ldlBlock, (MBCHAR *)NULL )
{
	
	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE
		| k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER
		| k_AUI_REGION_DRAWFLAG_MOUSEMOVEAWAY
		| k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;
}



TextTab::TextTab(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 paneWidth,
	sint32 paneHeight,
	MBCHAR *pattern,
	MBCHAR *text,
	ControlActionCallback *ActionFunc,
	void *cookie,
	BOOL selected )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text ),
	aui_Tab( retval, id, x, y, width, height, paneWidth, paneHeight, ActionFunc, cookie, selected ),
	PatternBase( pattern )
{
	
	m_drawMask = k_AUI_REGION_DRAWFLAG_UPDATE
		| k_AUI_REGION_DRAWFLAG_MOUSEMOVEOVER
		| k_AUI_REGION_DRAWFLAG_MOUSEMOVEAWAY
		| k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;
}



AUI_ERRCODE TextTab::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	
	
	if ( !m_draw )
		m_draw = k_AUI_REGION_DRAWFLAG_UPDATE;
	else
	{
		
		
		if ( IsOn()
		&&   (m_draw & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE)
		&&   !(m_draw & k_AUI_REGION_DRAWFLAG_UPDATE) )
			return AUI_ERRCODE_DONTDRAWCHILDREN;
	}

	
	
	BOOL drawPane = m_draw & (
		k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE |
		k_AUI_REGION_DRAWFLAG_UPDATE );

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	RECT paneRect =
	{
		m_pane->X(),
		m_pane->Y(),
		m_pane->X() + m_pane->Width(),
		m_pane->Y() + m_pane->Height()
	};
	
	OffsetRect( &paneRect, rect.left, rect.top );

	
	if ( m_pattern )
	{
		m_pattern->Draw( surface, &rect );

		if ( IsOn() && drawPane )
			m_pattern->Draw( surface, &paneRect );
	}

	DrawThisStateImage(
		m_state ? m_state : IsOn() ? 1 : 0,
		surface,
		&rect );

	
	AUI_TABGROUP_ALIGNMENT a = ((aui_TabGroup *)m_parent)->
		GetTabGroupAlignment();

	if ( IsOn() )
	{
		if ( drawPane )
		{
			primitives_BevelTabSelected16( surface, &rect, 2, 0, 16, 16, a );
			primitives_BevelPane16( surface, &paneRect, 2, 0, 16, 16, a );
			primitives_BevelLeftPiece16( surface, &rect, &paneRect,
										 2, 0, 16, 16, a );
			primitives_BevelRightPiece16( surface, &rect, &paneRect,
										  2, 0, 16, 16, a );
		}
		else
			primitives_BevelTabDeselected16( surface, &rect, 2, 0, 16, 16, a );
	}
	else
		primitives_BevelTabDeselected16( surface, &rect, 2, 0, 16, 16, a );




	if ( IsActive() )
		SetTextColor(g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_HILITE));

	else
		SetTextColor(g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_PLAIN));







	
	DrawThisText(surface, &rect);

	if ( surface == m_window->TheSurface() )
	{
		m_window->AddDirtyRect( &rect );

		if ( IsOn() && drawPane )
			m_window->AddDirtyRect( &paneRect );
	}

	return drawPane ? AUI_ERRCODE_OK : AUI_ERRCODE_DONTDRAWCHILDREN;
}
