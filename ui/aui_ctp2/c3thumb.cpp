












#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "aui_thumb.h"
#include "pattern.h"

#include "primitives.h"

#include "c3thumb.h"



C3Thumb::C3Thumb(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	ControlActionCallback *ActionFunc,
	void *cookie )
:
	aui_Thumb( retval, id, x, y, width, height, ActionFunc, cookie ),
	PatternBase( pattern ),
	aui_TextBase(NULL),
	aui_ImageBase( (sint32)0 )
{
}


C3Thumb::C3Thumb(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
:
	aui_Thumb( retval, id, ldlBlock, ActionFunc, cookie ),
	PatternBase( ldlBlock, (MBCHAR *)NULL ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ),
	aui_ImageBase( ldlBlock )
{
}


AUI_ERRCODE C3Thumb::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	DrawThisStateImage(0, surface, &rect);

	
	if ( m_pattern ) {
		m_pattern->Draw( surface, &rect );
		primitives_BevelRect16( surface, &rect, 2, 0, 16, 16 );
	}

	










	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE	C3Thumb::PlaySound(AUI_SOUNDBASE_SOUND sound)
{
	return AUI_ERRCODE_OK;
}

