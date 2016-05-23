#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_ldl.h"
#include "pattern.h"

#include "primitives.h"

#include "bevellesswindow.h"


BevelLessWindow::BevelLessWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type )
	:
	aui_Window( retval, id, ldlBlock, bpp, type ),
	PatternBase( ldlBlock, NULL )
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


BevelLessWindow::BevelLessWindow(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type )
	:
	aui_Window( retval, id, x, y, width, height, bpp, type ),
	PatternBase( pattern )
{
	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE BevelLessWindow::InitCommon( void )
{
	GrabRegion()->Move( 0, 0 );
	GrabRegion()->Resize( m_width, 20 );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE BevelLessWindow::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	if ( m_pattern ) {
		m_pattern->Draw( m_surface, &rect );
	}









	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}
