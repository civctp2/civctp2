#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "aui_window.h"

#include "pixelutils.h"
#include "colorset.h"           // g_colorSet
#include "c3_coloredstatic.h"
#include "patternbase.h"
#include "pattern.h"

#include "primitives.h"

extern aui_UI		*g_ui;

c3_ColoredStatic::c3_ColoredStatic(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	c3_Static( retval, id, ldlBlock )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_ColoredStatic::c3_ColoredStatic(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	uint32 maxLength,
	uint32 bevelWidth,
	uint32 bevelType)
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text, maxLength ),
	c3_Static( retval, id, x, y, width, height, pattern, text, maxLength, bevelWidth, bevelType)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_ColoredStatic::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}


AUI_ERRCODE c3_ColoredStatic::InitCommon()
{
	m_color = COLOR_BLACK;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE c3_ColoredStatic::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern )
		m_pattern->Draw( surface, &rect );

	DrawThisStateImage(
		0,
		surface,
		&rect );

	if (m_color != COLOR_MAX) {
		primitives_PaintRect16(surface, &rect, g_colorSet->GetColor(m_color));
	}

	if (m_bevelWidth > 0) {
		primitives_BevelRect16( surface, &rect, m_bevelWidth, m_bevelType, 16, 16 );
	}

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
