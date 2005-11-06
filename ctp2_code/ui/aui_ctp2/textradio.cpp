












#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "pattern.h"
#include "aui_window.h"

#include "primitives.h"
#include "colorset.h"           // g_colorSet

#include "textradio.h"





TextRadio::TextRadio(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ), 
	aui_Radio( retval, id, ldlBlock, ActionFunc, cookie),
	PatternBase( ldlBlock, (MBCHAR *)NULL )
{
}



TextRadio::TextRadio(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	MBCHAR *pattern,
	MBCHAR *text,
	ControlActionCallback *ActionFunc,
	void *cookie,
	BOOL selected )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( text ),
	aui_Radio( retval, id, x, y, width, height, ActionFunc, cookie, selected ),
	PatternBase( pattern )
{
}



AUI_ERRCODE TextRadio::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	
	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	RECT buttonRect = { 0, m_height / 2 - 10, 0, 0 };
	buttonRect.right = buttonRect.left + 20;
	buttonRect.bottom = buttonRect.top + 20;
	OffsetRect( &buttonRect, rect.left, rect.top );

	
	if ( m_pattern )
		m_pattern->Draw( surface, &buttonRect );

	if ( IsOn() )
	{
		
		primitives_BevelRect16( surface, &buttonRect, 1, 1, 16, 16 );
	}
	else
	{
		
		primitives_BevelRect16( surface, &buttonRect, 2, 0, 16, 16 );
	}

	if ( IsActive() )
	{
		
		if ( IsOn() )
		{
			
			primitives_BevelRect16( surface, &buttonRect, 1, 1, 16, 16 );
		}
		else
		{
			
			primitives_BevelRect16( surface, &buttonRect, 2, 0, 16, 16 );
		}
	}
	else
	{
		
	}

	if ( IsActive() )
		SetTextColor(g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_HILITE));

	else
		SetTextColor(g_colorSet->GetColorRef(COLOR_BUTTON_TEXT_PLAIN));


	
	DrawThisText(surface, &rect);

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
