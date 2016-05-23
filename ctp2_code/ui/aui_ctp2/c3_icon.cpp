#include "c3.h"
#include "aui.h"
#include "aui_ldl.h"
#include "aui_ui.h"
#include "aui_window.h"

#include "tiledmap.h"
#include "tileset.h"

#include "pixelutils.h"
#include "colorset.h"
#include "c3_icon.h"
#include "patternbase.h"
#include "pattern.h"

#include "primitives.h"

extern aui_UI		*g_ui;
extern ColorSet		*g_colorSet;
extern TiledMap		*g_tiledMap;
extern TileSet		*g_tileSet;

c3_Icon::c3_Icon(
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


c3_Icon::c3_Icon(
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


AUI_ERRCODE c3_Icon::InitCommonLdl(const MBCHAR *ldlBlock)
{
	return InitCommon();
}


AUI_ERRCODE c3_Icon::InitCommon()
{
	m_color = COLOR_BLACK;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE c3_Icon::DrawThis(
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

	if (m_mapIcon != MAPICON_MAX) {
		if (m_color != COLOR_MAX) {
			POINT where;
			TileSet		*tileSet = g_tiledMap->GetTileSet();
			POINT iconDim = tileSet->GetMapIconDimensions( m_mapIcon );

			where.x = ( m_width - iconDim.x ) / 2;
			if (where.x < 0) where.x = 0;
			where.y = ( m_height - iconDim.y ) / 2;
			if (where.y < 0) where.y = 0;

			where.x += rect.left;
			where.y += rect.top;

			Pixel16 *icon;

			icon = tileSet->GetMapIconData( m_mapIcon );
			Assert(icon);
			if (!icon) return AUI_ERRCODE_OK;

			g_tiledMap->DrawColorizedOverlay( icon, surface, where.x, where.y, g_colorSet->GetColor(m_color) );
		}
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
