#include "c3.h"

#include "aui.h"
#include "c3ui.h"
#include "aui_surface.h"
#include "pattern.h"

#include "primitives.h"
#include "textutils.h"
#include "colorset.h"

#include "tipwindow.h"

extern C3UI *g_c3ui;

TipWindow::TipWindow(
		AUI_ERRCODE *retval,
		uint32 id,
		MBCHAR *ldlBlock,
		MBCHAR *text,
		sint32 bpp)
	:
	C3Window(retval, id, ldlBlock, bpp, AUI_WINDOW_TYPE_TIP),
	aui_TextBase(ldlBlock, text)
{
	FitWindowToText();
}

TipWindow::TipWindow(AUI_ERRCODE *retval, uint32 id, sint32 x, sint32 y, sint32 width, sint32 height, sint32 bpp,
					 MBCHAR *pattern, MBCHAR *text)
		:
		C3Window( retval, id, x, y, width, height, bpp, pattern, AUI_WINDOW_TYPE_TIP ),
		aui_TextBase( text )
{
	FitWindowToText();
}




AUI_ERRCODE TipWindow::FitWindowToText( void )
{
	if ( m_text && m_surface)
	{
		RECT rect = textutils_GetBounds(m_surface, m_text );
		rect.right += 10;
		rect.bottom += 10;

		if ( rect.right > m_surface->Width() ) rect.right = m_surface->Width();
		if ( rect.bottom > m_surface->Height() ) rect.bottom = m_surface->Height();

		Resize( rect.right, rect.bottom );
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE TipWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if (surface == NULL) surface = m_surface;

	RECT rect = { 0, 0, m_width, m_height };

	m_pattern->Draw( surface, &rect );

	primitives_BevelRect16( surface, &rect, 1, 0, 16, 16 );

	if ( m_text )
	{

		textutils_CenteredDropString(surface, m_text, &rect, 9, COLOR_BUTTON_TEXT_PLAIN, 0);
	}

	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}
