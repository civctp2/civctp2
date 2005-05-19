












#include "c3.h"

#include "aui.h"

#include "c3ui.h"

#include "aui_keyboard.h"
#include "aui_surface.h"


C3UI::C3UI(
	AUI_ERRCODE *retval,
	HINSTANCE hinst,
	HWND hwnd,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *ldlFilename,
	BOOL useExclusiveMode )
	:
#if defined(__AUI_USE_SDL__)
	aui_SDLUI( retval, hinst, hwnd, width, height, bpp, ldlFilename, useExclusiveMode )
#else
	aui_DirectUI( retval, hinst, hwnd, width, height, bpp, ldlFilename, useExclusiveMode )
#endif
{
	m_patternResource = new aui_Resource<Pattern>;
	m_iconResource = new aui_Resource<Icon>;
	m_pictureResource = new aui_Resource<Picture>;
}



C3UI::~C3UI()
{
	if ( m_patternResource )
	{
		delete m_patternResource;
		m_patternResource = NULL;
	}

	if ( m_iconResource )
	{
		delete m_iconResource;
		m_iconResource = NULL;
	}

	if ( m_pictureResource )
	{
		delete m_pictureResource;
		m_pictureResource = NULL;
	}
}

BOOL C3UI::TopWindowIsNonBackground(void) 
{
	if (TopWindow()) { 
		return TopWindow()->Type() != AUI_WINDOW_TYPE_BACKGROUND &&
				TopWindow()->Type() != AUI_WINDOW_TYPE_SINKING; 
	} else 
		return FALSE; 
}