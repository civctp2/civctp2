#include "c3.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_action.h"
#include "aui_window.h"
#include "aui_ldl.h"

#include "c3ui.h"
#include "pixelutils.h"
#include "colorset.h"
#include "c3_coloredswitch.h"
#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"

#include "SlicEngine.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;
extern ColorSet		*g_colorSet;

c3_ColoredSwitch::c3_ColoredSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	const MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Switch( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase(ldlBlock, NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


c3_ColoredSwitch::c3_ColoredSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie,
	sint32 state,
	sint32 numStates )
	:
	aui_ImageBase( numStates ),
	aui_TextBase( NULL ),
	aui_Switch( retval, id, x, y, width, height, ActionFunc, cookie, state, numStates ),
	PatternBase(pattern)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(k_C3_COLOREDSWITCH_DEFAULT_BEVELWIDTH);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_ColoredSwitch::InitCommonLdl(const MBCHAR *ldlBlock)
{
	sint32		bevelWidth=k_C3_COLOREDSWITCH_DEFAULT_BEVELWIDTH;
	aui_Ldl		*theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	if (block->GetAttributeType( k_C3_COLOREDSWITCH_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT) {
		bevelWidth = block->GetInt( k_C3_COLOREDSWITCH_LDL_BEVELWIDTH );
	}

	return InitCommon(bevelWidth);
}


AUI_ERRCODE c3_ColoredSwitch::InitCommon( sint32 bevelWidth )
{
	m_bevelWidth = bevelWidth;

	m_color = COLOR_BLACK;

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_ColoredSwitch::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_pattern ) m_pattern->Draw( surface, &rect );

	DrawThisStateImage(
		m_state ? m_state : IsOn() ? 1 : 0,
		surface,
		&rect );

	if (m_color != COLOR_MAX) {
		primitives_PaintRect16(surface, &rect, g_colorSet->GetColor(m_color));
	}

	if (m_bevelWidth > 0)
		primitives_BevelRect16( surface, &rect, m_bevelWidth, (IsOn() ? 1 : 0), 16, 16 );

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
