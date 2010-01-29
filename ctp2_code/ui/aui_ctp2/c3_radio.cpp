
#include "c3.h"
#include "c3_radio.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_action.h"
#include "aui_window.h"
#include "aui_ldl.h"

#include "c3ui.h"
#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"
#include "colorset.h"       // g_colorSet
#include "SlicEngine.h"     // g_slicEngine

#include "ldl_data.hpp"

extern C3UI			*g_c3ui;


c3_Radio::c3_Radio(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_Radio( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase(ldlBlock, NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



c3_Radio::c3_Radio(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie,
	sint32 state,
	sint32 numStates )
	:
	aui_ImageBase( numStates ),
	aui_TextBase( NULL ),
	aui_Radio( retval, id, x, y, width, height, ActionFunc, cookie, state, numStates ),
	PatternBase(pattern)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(k_C3_RADIO_DEFAULT_BEVELWIDTH);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE c3_Radio::InitCommonLdl( MBCHAR *ldlBlock )
{
	ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
	
	sint32  bevelWidth  = k_C3_RADIO_DEFAULT_BEVELWIDTH; 
	if (block->GetAttributeType( k_C3_RADIO_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT) {
		bevelWidth = block->GetInt( k_C3_RADIO_LDL_BEVELWIDTH );
	}
	if (block->GetAttributeType( k_C3_RADIO_LDL_BEVELTYPE) == ATTRIBUTE_TYPE_INT) {
		m_bevelType = block->GetInt( k_C3_RADIO_LDL_BEVELTYPE );
	}

	return InitCommon(bevelWidth);
}



AUI_ERRCODE c3_Radio::InitCommon( sint32 bevelWidth )
{
	m_bevelWidth = bevelWidth;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE c3_Radio::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
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

	DrawThisStateImage(
		m_state ? m_state : IsOn() ? 1 : 0,
		surface,
		&rect );

	if (m_bevelWidth > 0) {
		if ( m_bevelType == 2 ) {
			if ( IsOn() ) {
				primitives_FrameThickRect16( surface, &rect, g_colorSet->GetColor( COLOR_UI_BOX ), m_bevelWidth );
			}
		}
		else {
			if ( IsOn() ) {
				primitives_BevelRect16( surface, &rect, m_bevelWidth, (IsOn() ? 1 : 0), 16, 16 );
			}
		}
	}

	DrawThisText(
		surface,
		&rect );

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
