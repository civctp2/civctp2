#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_surface.h"
#include "ui/aui_common/aui_window.h"
#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_common/aui_action.h"

#include "ui/aui_ctp2/c3ui.h"

#include "ui/aui_ctp2/pattern.h"
#include "gfx/gfx_utils/pixelutils.h"
#include "gfx/gfx_utils/colorset.h"

#include "ui/aui_utils/primitives.h"
#include "ui/aui_utils/textutils.h"

#include "ui/aui_ctp2/textbutton.h"

#include "gs/slic/SlicEngine.h"

extern C3UI			*g_c3ui;
extern SlicEngine	*g_slicEngine;

TextButton::TextButton(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	c3_Button( retval, id, ldlBlock, ActionFunc, cookie )
{
}


TextButton::TextButton(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text ),
	c3_Button( retval, id, x, y, width, height, pattern, ActionFunc, cookie )
{
}


AUI_ERRCODE TextButton::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{




	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );





	if ( m_pattern )
		m_pattern->Draw( surface, &rect );




	if ( IsDown() )
		primitives_BevelRect16( surface, &rect, 2, 1, 16, 16 );
	else
		primitives_BevelRect16( surface, &rect, 2, 0, 16, 16 );

	if ( IsActive() )
	{
		if (m_text) {
			textutils_CenteredDropString(surface, m_text, &rect, 9, COLOR_BUTTON_TEXT_HILITE, 0);
		}
	}
	else
	{
		if (m_text) {
			textutils_CenteredDropString(surface, m_text, &rect, 9, COLOR_BUTTON_TEXT_PLAIN, 0);
		}
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
