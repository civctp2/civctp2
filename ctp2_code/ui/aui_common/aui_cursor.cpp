










#include "c3.h"

#ifdef __AUI_USE_DIRECTX__
#include "aui_directui.h"
#include "aui_directsurface.h"
#else
#include "aui_ui.h"
#include "aui_surface.h"
#endif 

#include "aui_cursor.h"



aui_Cursor::aui_Cursor(
	AUI_ERRCODE *retval,
	MBCHAR *filename )
	:
	aui_Image( retval, filename )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_Cursor::InitCommon( void )
{
	memset( &m_hotspot, 0, sizeof( m_hotspot ) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Cursor::GetHotspot( sint32 *x, sint32 *y )
{
	if ( x ) *x = m_hotspot.x;
	if ( y ) *y = m_hotspot.y;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Cursor::SetHotspot( sint32 x, sint32 y )
{
	m_hotspot.x = x;
	m_hotspot.y = y;

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_Cursor::LoadEmpty( sint32 width, sint32 height, sint32 bpp )
{
	AUI_ERRCODE retcode;

#ifdef __AUI_USE_DIRECTX__
	m_surface = new aui_DirectSurface(
		&retcode,
		width,
		height,
		bpp,
		((aui_DirectUI *)g_ui)->DD(),
		NULL,
		FALSE,
		FALSE );
#else
	m_surface = new aui_Surface(
		&retcode,
		width,
		height,
		bpp );
#endif 

	Assert( AUI_NEWOK(m_surface,retcode) );
	return retcode;
}
