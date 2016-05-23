#include "c3.h"

#include "aui.h"
#include "aui_window.h"

#include "pattern.h"
#include "controlsheet.h"

#include "primitives.h"

ControlSheet::ControlSheet(AUI_ERRCODE *retval,
					   uint32 id,
					   const MBCHAR *ldlBlock,
					   ControlActionCallback *ActionFunc,
					   void *cookie)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	aui_Control(retval, id, ldlBlock, ActionFunc, cookie),
	PatternBase(ldlBlock, NULL)
{
	m_border = TRUE;

	SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );
}

ControlSheet::ControlSheet(AUI_ERRCODE *retval,
					   uint32 id,
					   sint32 x,
					   sint32 y,
					   sint32 width,
					   sint32 height,
					   const MBCHAR *pattern,
					   ControlActionCallback *ActionFunc,
					   void *cookie)
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase(NULL),
	aui_Control(retval, id, x, y, width, height, ActionFunc, cookie),
	PatternBase(pattern)
{
	m_border = TRUE;

	SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );
}

ControlSheet::~ControlSheet()
{

}

AUI_ERRCODE ControlSheet::DrawThis(aui_Surface *surface, sint32 x, sint32 y)
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) {
		if ( m_srcWidthPix || m_srcHeightPix ) {
			RECT srcRect = { m_srcX, m_srcY, m_srcX + m_srcWidthPix, m_srcY + m_srcHeightPix };
			m_pattern->Draw( surface, &rect, &srcRect );
		}
		else {
			m_pattern->Draw( surface, &rect );
		}
	}

	if ( m_border )
		primitives_BevelRect16( surface, &rect, 2, 1, 16, 16 );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
