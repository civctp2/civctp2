












#include "c3.h"

#include "aui.h"
#include "aui_control.h"
#include "aui_ldl.h"
#include "pattern.h"

#include "primitives.h"

#include "c3window.h"



C3Window::C3Window(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	sint32 bpp,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	aui_Window( retval, id, ldlBlock, bpp, type ),
	PatternBase( ldlBlock, NULL )
{
	m_bevel = bevel;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



C3Window::C3Window(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *pattern,
	AUI_WINDOW_TYPE type,
	bool bevel)
	:
	aui_Window( retval, id, x, y, width, height, bpp, type ),
	PatternBase( pattern )
{
	m_bevel = bevel;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

	

AUI_ERRCODE C3Window::InitCommon( void )
{
	GrabRegion()->Move( 0, 0 );
	GrabRegion()->Resize( m_width, 20 );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE C3Window::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	RECT rect = { 0, 0, m_width, m_height };

	if (m_pattern)
		m_pattern->Draw( m_surface, &rect );

	
	if(m_bevel)
		primitives_BevelRect16( m_surface, &rect, 3, 0, 16, 16 );

	



	
	m_dirtyList->AddRect( &rect );

	return AUI_ERRCODE_OK;
}
