#include "c3.h"

#include "aui.h"
#include "aui_control.h"

#include "pattern.h"
#include "primitives.h"

#include "tiledmap.h"

#include "workwindow.h"

extern TiledMap *g_tiledMap;


WorkWindow::WorkWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	C3Window(retval, id, ldlBlock, bpp, type)
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


WorkWindow::WorkWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type)
	:
	C3Window( retval, id, x, y, width, height, bpp, pattern, type )
{
	*retval = InitCommon();
	Assert(AUI_SUCCESS(*retval));
	if (!AUI_SUCCESS(*retval)) return;
}


AUI_ERRCODE WorkWindow::InitCommon(void)
{
	return C3Window::InitCommon();
}


AUI_ERRCODE WorkWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	m_pattern->Draw( m_surface, &rect );





	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}

void WorkWindow::MouseMoveInside(aui_MouseEvent *data)

{
	if ( IsDisabled() ) return;
	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

    Assert(data);

	g_tiledMap->DrawHilite( FALSE );

}

void WorkWindow::MouseMoveAway(aui_MouseEvent *data)

{
	if ( IsDisabled() ) return;
	if (GetWhichSeesMouse() && GetWhichSeesMouse() != this) return;
	SetWhichSeesMouse(this);

    Assert(data);

}
