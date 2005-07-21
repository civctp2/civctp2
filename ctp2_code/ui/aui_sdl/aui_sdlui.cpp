//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : SDL user interface handling
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// __AUI_USE_SDL__
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented crashes
//
//----------------------------------------------------------------------------

#include "c3.h"


#ifdef __AUI_USE_SDL__

#include "civ3_main.h"


#include "aui_mouse.h"
#include "aui_keyboard.h"
#include "aui_joystick.h"
#include "aui_sdlsurface.h"
#include "aui_sdlmouse.h"

#include "aui_sdlui.h"

extern BOOL			g_exclusiveMode;

#include "civapp.h"
extern CivApp		*g_civApp;


#include "display.h"

extern BOOL					g_createDirectDrawOnSecondary;
extern sint32				g_ScreenWidth;
extern sint32				g_ScreenHeight;
extern DisplayDevice		g_displayDevice;


aui_SDLUI::aui_SDLUI
(
	AUI_ERRCODE *retval,
	HINSTANCE hinst,
	HWND hwnd,
	sint32 width,
	sint32 height,
	sint32 bpp,
	MBCHAR *ldlFilename,
	BOOL useExclusiveMode 
)
:   aui_UI              (),
    aui_SDL             (),
    m_X11Display        (0)
{
	
	*retval = aui_Region::InitCommon( 0, 0, 0, width, height );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	Assert( aui_Base::GetBaseRefCount() == 2 );
	g_ui = aui_Base::GetBaseRefCount() == 2 ? this : NULL;

	
	*retval = aui_UI::InitCommon( hinst, hwnd, bpp, ldlFilename );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateNativeScreen( useExclusiveMode );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

#if defined(HAVE_X11)
	char *dispname = getenv("DISPLAY");
	if (dispname) {
		m_X11Display = XOpenDisplay(dispname);
	} else {
		m_X11Display = XOpenDisplay(":0.0");
	}
	if (!m_X11Display) {
		*retval = AUI_ERRCODE_NOUI;
	}
#endif
}



AUI_ERRCODE aui_SDLUI::InitCommon()
{
	m_savedMouseAnimFirstIndex = 0;
	m_savedMouseAnimLastIndex = 0;
	m_savedMouseAnimCurIndex = 0;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_SDLUI::DestroyNativeScreen(void)
{
	if (m_primary)
	{
		delete m_primary;
		m_primary   = NULL;
		m_lpdds     = NULL;
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_SDLUI::CreateNativeScreen( BOOL useExclusiveMode )
{
	AUI_ERRCODE errcode = aui_SDL::InitCommon( useExclusiveMode );
	Assert( AUI_SUCCESS(errcode) );
	assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	m_lpdds = SDL_SetVideoMode(m_width, m_height, m_bpp, SDL_SWSURFACE);
	
	m_primary = new aui_SDLSurface(
		&errcode,
		m_width,
		m_height,
		m_bpp,
		m_lpdd,
		TRUE );
	Assert( AUI_NEWOK(m_primary,errcode) );
	assert( AUI_NEWOK(m_primary,errcode) );
	if ( !AUI_NEWOK(m_primary,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_pixelFormat = m_primary->PixelFormat();

	return AUI_ERRCODE_OK;
}

#ifdef HAVE_X11
Display *
aui_SDLUI::getDisplay()
{
	return m_X11Display;
}
#endif

aui_SDLUI::~aui_SDLUI( void )
{
	if ( m_lpdds ) {
		// m_lpdds is deleted by SDL_Quit()
		m_lpdds = NULL;
	}
#ifdef HAVE_X11
	if (m_X11Display) {
		XCloseDisplay(m_X11Display);
		m_X11Display = 0;
	}
#endif
}


AUI_ERRCODE aui_SDLUI::TearDownMouse(void)
{
	
	
	if (m_mouse) {
		m_mouse->GetAnimIndexes(&m_savedMouseAnimFirstIndex, &m_savedMouseAnimLastIndex);
		m_savedMouseAnimCurIndex = m_mouse->GetCurrentCursorIndex();
		m_savedMouseAnimDelay = (sint32)m_mouse->GetAnimDelay();

		if ( m_minimize || m_exclusiveMode )
		{
#if 0
			SetCursorPos( m_mouse->X(), m_mouse->Y() );
#endif
		}

		m_mouse->End();
		delete m_mouse;
		m_mouse = NULL;
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_SDLUI::RestoreMouse(void)
{
	AUI_ERRCODE		auiErr;
	BOOL			exclusive = TRUE;

	
	aui_SDLMouse *mouse = new aui_SDLMouse( &auiErr, "CivMouse", exclusive );
	Assert(mouse != NULL);
	if ( !mouse ) return AUI_ERRCODE_MEMALLOCFAILED;

	delete m_mouse;
	m_mouse = mouse;

	m_mouse->SetAnimIndexes(m_savedMouseAnimFirstIndex, m_savedMouseAnimLastIndex);
	m_mouse->SetCurrentCursor(m_savedMouseAnimCurIndex);
	m_mouse->SetAnimDelay((uint32)m_savedMouseAnimDelay);

	auiErr = m_mouse->Start();
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return auiErr;

	if ( m_minimize || m_exclusiveMode )
	{
		POINT point;
		//GetCursorPos( &point );
		m_mouse->SetPosition( &point );
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_SDLUI::AltTabOut( void )
{


	if(m_keyboard) m_keyboard->Unacquire();
	if ( m_joystick ) m_joystick->Unacquire();

	if (m_mouse) {
		if (g_exclusiveMode) {
			TearDownMouse();
		} else {
			main_RestoreTaskBar();

			if (!m_mouse->IsSuspended()) {
				m_mouse->Suspend(FALSE);
				m_mouse->Unacquire();
			}
		}
	}

	if ( m_minimize || m_exclusiveMode )
	{
		DestroyNativeScreen();
	}

#if 0
	while ( ShowCursor( TRUE ) < 0 )
		; 

	if ( m_minimize || m_exclusiveMode )
	{
		while ( !IsIconic( m_hwnd ) )
			::ShowWindow( m_hwnd, SW_MINIMIZE );
	}
#endif
	if (g_civApp)
	{
		g_civApp->SetInBackground(TRUE);
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_SDLUI::AltTabIn( void )
{


	if ( !m_primary ) CreateNativeScreen( m_exclusiveMode );

#if 0
	if ( m_minimize || m_exclusiveMode )
		while ( GetForegroundWindow() != m_hwnd )
			::ShowWindow( m_hwnd, SW_RESTORE );
	::ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);

	while ( ShowCursor( FALSE ) >= 0 )
		; 


	if (g_exclusiveMode) {
		RestoreMouse();
	} else {
		if ( m_minimize || m_exclusiveMode )
		{
			POINT point;
			GetCursorPos( &point );
			m_mouse->SetPosition( &point );
		}

		if (m_mouse) {
			m_mouse->Acquire();
			m_mouse->Resume();
		}

		main_HideTaskBar();

		
		RECT clipRect = { 0, 0, m_width, m_height };
		ClipCursor(&clipRect);
	}
#endif
	if ( m_joystick ) m_joystick->Acquire();
	if (m_keyboard) m_keyboard->Acquire();

	if (g_civApp)
    {
        g_civApp->SetInBackground(FALSE);
    }

	return FlushDirtyList();
}


#endif  // __AUI_USE_SDL__ 
