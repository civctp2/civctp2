












#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_window.h"
#include "pattern.h"
#include "aui_ldl.h"
#include "aui_action.h"

#include "c3ui.h"

#include "pixelutils.h"
#include "colorset.h"

#include "primitives.h"
#include "textutils.h"

#include "textswitch.h"

#include "SlicEngine.h"

extern ColorSet		*g_colorSet;
extern SlicEngine	*g_slicEngine;
extern C3UI			*g_c3ui;


TextSwitch::TextSwitch(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie)
	:
	aui_Switch( retval, id, ldlBlock, ActionFunc, cookie ),
	aui_TextBase( ldlBlock, (MBCHAR *)NULL ), 
	PatternBase( ldlBlock, (MBCHAR *)NULL ),
	aui_ImageBase( ldlBlock )
{
}



TextSwitch::TextSwitch(
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
	aui_Switch( retval, id, x, y, width, height, ActionFunc, cookie, selected ),
	aui_TextBase( text ),
	PatternBase( pattern ),
	aui_ImageBase( (sint32)0 )
{
}



AUI_ERRCODE TextSwitch::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{
	
	

	
	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	
	if ( m_pattern )
		m_pattern->Draw( surface, &rect );

	primitives_FrameRect16(surface, &rect, g_colorSet->GetColor(COLOR_BLACK));

	











	if ( IsOn() )
		primitives_BevelRect16( surface, &rect, 2, 1, 16, 16 );
	else
		primitives_BevelRect16( surface, &rect, 2, 0, 16, 16 );

	if ( IsActive() )
	{
		
		if ( IsOn() )
		{
			
		}
		else
		{
			
		}

		
		if (m_text) {




			textutils_CenteredDropString((aui_DirectSurface *)surface, m_text, &rect, 9, COLOR_BUTTON_TEXT_HILITE, 0);
		}
	}
	else
	{
		
		if (m_text) {




			textutils_CenteredDropString((aui_DirectSurface *)surface, m_text, &rect, 9, COLOR_BUTTON_TEXT_PLAIN, 0);

		}
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}
