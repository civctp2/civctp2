#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_surface.h"
#include "ui/aui_common/aui_window.h"
#include "ui/aui_ctp2/pattern.h"

#include "ui/aui_utils/primitives.h"

#include "ui/aui_ctp2/radiogroup.h"


RadioGroup::RadioGroup(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock )
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
	MBCHAR *pattern,
	MBCHAR *text )
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
