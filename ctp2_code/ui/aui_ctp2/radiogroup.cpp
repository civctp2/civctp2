#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "pattern.h"

#include "primitives.h"

#include "radiogroup.h"


RadioGroup::RadioGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_SwitchGroup( retval, id, ldlBlock ),
	PatternBase( ldlBlock, (MBCHAR *)NULL )
{
}


RadioGroup::RadioGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	const MBCHAR *text )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text ),
	aui_SwitchGroup( retval, id, x, y, width, height ),
	PatternBase( pattern )
{
}


AUI_ERRCODE RadioGroup::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern )
		m_pattern->Draw( surface, &rect );

























	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
