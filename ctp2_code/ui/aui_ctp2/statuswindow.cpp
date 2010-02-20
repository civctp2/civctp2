












#include "c3.h"

#include "aui.h"
#include "pattern.h"

#include "primitives.h"

#include "statuswindow.h"

#include "c3cmdline.h"


AUI_ERRCODE StatusWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	m_pattern->Draw( m_surface, &rect );

	
	primitives_BevelRect16( m_surface, &rect, 2, 0, 16, 16 );

	
	InflateRect(&rect, -2, -2);
	primitives_BevelRect16( m_surface, &rect, 1, 1, 16, 16 );

#ifdef _PLAYTEST
	g_commandLine.Draw();
#endif
	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}

#ifdef _PLAYTEST
AUI_ERRCODE StatusWindow::DrawCommand(char* str)
{
	if ( IsHidden() ) Show();
	RECT rect = { 0, 0, m_width, m_height };
	primitives_DropText(m_surface, 15, 5, str, 0x0000, 1);
	m_dirtyList->AddRect(&rect);
	return AUI_ERRCODE_OK;
}
#endif
