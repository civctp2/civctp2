#include "c3.h"

#include "aui.h"
#include "aui_ui.h"
#include "aui_action.h"
#include "aui_window.h"
#include "aui_ldl.h"

#include "c3ui.h"
#include "c3_checkbox.h"
#include "patternbase.h"
#include "pattern.h"
#include "primitives.h"
#include "colorset.h"

#include "SlicEngine.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;

extern ColorSet		*g_colorSet;

c3_CheckBox::c3_CheckBox(
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


c3_CheckBox::c3_CheckBox(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	const MBCHAR *text,
	const MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie,
	sint32 state,
	sint32 numStates )
	:
	aui_ImageBase( numStates ),
	aui_TextBase( text ),
	aui_Switch( retval, id, x, y, width, height, ActionFunc, cookie, state, numStates ),
	PatternBase(pattern)
{

	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon(k_C3_CHECKBOX_DEFAULT_BEVELWIDTH);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE c3_CheckBox::InitCommonLdl(const MBCHAR *ldlBlock)
{
	sint32		bevelWidth=k_C3_CHECKBOX_DEFAULT_BEVELWIDTH;
	aui_Ldl		*theLdl = g_c3ui->GetLdl();

	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );

	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	if (block->GetAttributeType( k_C3_CHECKBOX_LDL_BEVELWIDTH) == ATTRIBUTE_TYPE_INT) {
		bevelWidth = block->GetInt( k_C3_CHECKBOX_LDL_BEVELWIDTH );
	}

	return InitCommon(bevelWidth);
}


AUI_ERRCODE c3_CheckBox::InitCommon( sint32 bevelWidth )
{
	m_bevelWidth = bevelWidth;

	SetNumStates(2);
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE c3_CheckBox::DrawThis(
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

	if (m_bevelWidth > 0)
		primitives_BevelRect16( surface, &rect, m_bevelWidth, IsOn()?1:0, 16, 16 );

	uint32 bordWidth = m_bevelWidth ? m_bevelWidth*2 : 2;
	RECT checkrect = { bordWidth,bordWidth, m_height - bordWidth, m_height - bordWidth };
	OffsetRect(&checkrect, m_x+x, m_y+y);
	ToWindow(&checkrect);
	primitives_BevelRect16(surface, &checkrect, m_bevelWidth, 1, 16,16);

	if(GetState()) {
		uint32 off = 5;
		primitives_DrawAALine16(surface, checkrect.left+off,checkrect.top+off,
										 checkrect.right-off,checkrect.bottom-off,g_colorSet->GetColor(COLOR_RED));
		primitives_DrawAALine16(surface, checkrect.left+off,checkrect.bottom-off,
										 checkrect.right-off,checkrect.top+off, g_colorSet->GetColor(COLOR_RED));
	}

	RECT textrect = { m_height+bordWidth,0, m_width-bordWidth-m_height, m_height };
	OffsetRect(&textrect, m_x+x, m_y+y);
	ToWindow(&textrect);
	DrawThisText(
		surface,
		&textrect );


	if ( IsDisabled() && rect.left < rect.right && rect.top < rect.bottom )
	{

		uint16 *pixel;

		AUI_ERRCODE errcode = surface->Lock( &rect, (void **)&pixel, 0 );
		Assert( AUI_SUCCESS(errcode) );
		if ( AUI_SUCCESS(errcode) )
		{

			uint16 *origPixel = pixel;

			const sint32 pitch = surface->Pitch() / 2;
			const sint32 width = rect.right - rect.left;
			const sint32 diff = pitch - width;

			uint16 *stopHorizontal = pixel + width;
			const uint16 *stopVertical = pixel +
				pitch * ( rect.bottom - rect.top );

			do
			{
				do
				{
					*pixel = pixelutils_BlendFast(
						*pixel,
						0x0000,
						24);

				} while ( ++pixel != stopHorizontal );

				stopHorizontal += pitch;
			} while ( (pixel += diff) != stopVertical );

			errcode = surface->Unlock( origPixel );
			Assert( AUI_SUCCESS(errcode) );
		}
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
