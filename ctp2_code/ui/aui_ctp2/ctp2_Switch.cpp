

#include "c3.h"
#include "ctp2_Switch.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_action.h"
#include "aui_window.h"
#include "aui_ldl.h"
#include "c3ui.h"
#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"

#include "ldl_data.hpp"

extern C3UI			*g_c3ui;


ctp2_Switch::ctp2_Switch(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR const *)NULL ),
	aui_Switch( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase(ldlBlock, NULL)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
}



ctp2_Switch::ctp2_Switch(
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
	aui_Switch( retval, id, x, y, width, height, ActionFunc, cookie, state, numStates ),
	PatternBase(pattern)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(k_CTP2_SWITCH_DEFAULT_BEVELWIDTH);
	Assert( AUI_SUCCESS(*retval) );
}



AUI_ERRCODE ctp2_Switch::InitCommonLdl( MBCHAR *ldlBlock )
{
    ldl_datablock * block = aui_Ldl::FindDataBlock(ldlBlock);
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;
	
	sint32		bevelWidth = k_CTP2_SWITCH_DEFAULT_BEVELWIDTH;
	if (block->GetAttributeType( k_CTP2_SWITCH_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT) {
		bevelWidth = block->GetInt( k_CTP2_SWITCH_LDL_BEVELWIDTH );
	}

	return InitCommon(bevelWidth);
}



AUI_ERRCODE ctp2_Switch::InitCommon( sint32 bevelWidth )
{
	m_bevelWidth = bevelWidth;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE ctp2_Switch::DrawThis(
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

	sint32 imIndex = m_state ? m_state : (IsOn() ? 1 : 0);
	aui_Image *im = GetImage(imIndex);
	RECT imRect = rect;
	if(im && im->TheSurface()) {
		sint32 imWidth = im->TheSurface()->Width();
		if(m_width > imWidth) {
			imRect.left += (m_width - imWidth) / 2;
		}

		sint32 imHeight = im->TheSurface()->Height();
		if(m_height > imHeight) {
			imRect.top += (m_height - imHeight) / 2;
		}
	}

	DrawThisStateImage(
		imIndex,
		surface,
		&imRect );

	if (m_bevelWidth > 0)
		primitives_BevelRect16( surface, &rect, m_bevelWidth, (IsOn() ? 1 : 0), 16, 16 );

	
	RECT down = rect;
	down.left += 2;
	down.top += 2;
	DrawThisText(
		surface,
		IsOn() ? &down : &rect );


	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

void ctp2_Switch::MouseLDoubleClickInside(aui_MouseEvent *mouseData)
{
	if(IsDisabled()) return;
	if(!GetWhichSeesMouse() || GetWhichSeesMouse() == this)
	{
		if(!GetWhichSeesMouse())
			SetWhichSeesMouse(this);

		if(m_ActionFunc)
			m_ActionFunc(this,
						 AUI_SWITCH_ACTION_DOUBLECLICK,
						 (uint32)m_state,
						 m_cookie);
		else if(m_action)
			m_action->Execute(this, AUI_SWITCH_ACTION_DOUBLECLICK,
							  (uint32)m_state);
	}
}
