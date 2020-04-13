//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectX user interface handling
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
// __AUI_USE_DIRECTX__
// - When not set, this file does not do anything at all.
//
// __AUI_USE_DIRECTMEDIA__
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented crashes
// - Added graphics DirectX built in double buffering and extended it
//   to manual tripple buffering. (1-Jan-2010 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef __AUI_USE_DIRECTX__

#include "civ3_main.h"

#include "aui_mouse.h"
#include "aui_keyboard.h"
#include "aui_joystick.h"
#include "aui_directsurface.h"
#include "aui_directmouse.h"
#include "aui_directmoviemanager.h"

#include "aui_directui.h"

extern BOOL			g_exclusiveMode;

#include "civapp.h"
extern CivApp		*g_civApp;

#include "display.h"

extern BOOL					g_createDirectDrawOnSecondary;
extern sint32				g_ScreenWidth;
extern sint32				g_ScreenHeight;
extern DisplayDevice		g_displayDevice;

aui_DirectUI::aui_DirectUI
(
	AUI_ERRCODE *retval,
	HINSTANCE hinst,
	HWND hwnd,
	sint32 width,
	sint32 height,
	sint32 bpp,
	const MBCHAR *ldlFilename,
	BOOL useExclusiveMode
)
:
    aui_UI              (retval, hinst, hwnd, width, height, bpp, ldlFilename),
    aui_DirectX         (),
    m_lpdds             (NULL),
    m_back              (NULL),
    m_isCoinitialized   (false)
{
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateDirectScreen( useExclusiveMode );
	Assert( AUI_SUCCESS(*retval) );
}

AUI_ERRCODE aui_DirectUI::InitCommon()
{
	m_savedMouseAnimFirstIndex = 0;
	m_savedMouseAnimLastIndex = 0;
	m_savedMouseAnimCurIndex = 0;

#ifdef __AUI_USE_DIRECTMEDIA__
	if (!m_isCoinitialized)
	{
		HRESULT const   hr  = CoInitialize(NULL);
		m_isCoinitialized   = (S_OK == hr) || (S_FALSE == hr);
	}
#endif

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectUI::DestroyDirectScreen(void)
{
	if (m_primary)
	{
		((aui_DirectSurface *)m_primary)->DDS()->Release();
		delete m_primary;
		m_primary   = NULL;
		m_lpdds     = NULL;
	}

	if(m_secondary)
	{
		delete m_secondary;
		m_secondary = NULL;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectUI::CreateDirectScreen( BOOL useExclusiveMode )
{
	AUI_ERRCODE errcode = aui_DirectX::InitCommon( useExclusiveMode );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	HRESULT hr;

	uint32 coopFlags = (m_exclusiveMode) ? (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT) : DDSCL_NORMAL;

	if (g_createDirectDrawOnSecondary)
	{
		coopFlags = DDSCL_SETFOCUSWINDOW | DDSCL_CREATEDEVICEWINDOW |
					DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;
	}

	hr = m_lpdd->SetCooperativeLevel( m_hwnd, coopFlags );
	Assert( hr == DD_OK );
	if ( hr != DD_OK ) return AUI_ERRCODE_SETCOOPLEVELFAILED;

	// With nonexclusive windows color depth must be 16 bit otherwise the game will crash.
	hr = m_lpdd->SetDisplayMode( m_width, m_height, m_bpp );
	Assert( hr == DD_OK );
	if ( hr != DD_OK ) return AUI_ERRCODE_SETDISPLAYFAILED;

	MoveWindow(
		m_hwnd,
		0, 0,

		g_ScreenWidth,
		g_ScreenHeight,
		TRUE );

	LPDIRECTDRAWSURFACE lpdds;
	DDSURFACEDESC ddsd;
	memset( &ddsd, 0, sizeof( ddsd ) );
	ddsd.dwSize = sizeof( ddsd );
	if(m_exclusiveMode)
	{
		ddsd.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = 1;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
	}
	else
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}

	hr = m_lpdd->CreateSurface( &ddsd, &lpdds, NULL );
	Assert( hr == DD_OK || hr == DDERR_PRIMARYSURFACEALREADYEXISTS );

	if ( hr != DD_OK )
	{
		if ( hr != DDERR_PRIMARYSURFACEALREADYEXISTS )
			return AUI_ERRCODE_CREATESURFACEFAILED;

		hr = m_lpdds->Restore();
		Assert( hr == DD_OK );
		if ( hr != DD_OK ) return AUI_ERRCODE_CREATESURFACEFAILED;
	}
	else
	{
		m_lpdds = lpdds;
	}

	if(m_exclusiveMode)
	{
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;

		hr = m_lpdds->GetAttachedSurface(&ddsd.ddsCaps, &m_back);
		Assert( hr == DD_OK );
		if ( hr != DD_OK ) return AUI_ERRCODE_CREATESURFACEFAILED;
	}
	else
	{
		m_lpdds->Restore();
	}

	delete m_primary;
	m_primary = new aui_DirectSurface(
		&errcode,
		m_width,
		m_height,
		m_bpp,
		m_lpdd,
		m_lpdds,
		TRUE,
		TRUE,
		m_back);
	Assert( AUI_NEWOK(m_primary,errcode) );
	if ( !AUI_NEWOK(m_primary,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	delete m_secondary;
	m_secondary = new aui_DirectSurface( &errcode, m_width, m_height, m_bpp, m_lpdd, 0, FALSE, TRUE );
	Assert( AUI_NEWOK(m_secondary,errcode) );
	if ( !AUI_NEWOK(m_secondary,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_pixelFormat = m_primary->PixelFormat();

	return AUI_ERRCODE_OK;
}

aui_DirectUI::~aui_DirectUI( void )
{
	// First end mouse, so that the mouse thread does not interfere anymore
	m_mouse->End();
	if (m_lpdds)
	{
		m_lpdds->Release();
		m_lpdds = NULL; // Set to be zero, since mouse thread may be still active when the program is shut down, well that was before mouse end
	}

	if (m_lpdd)
	{
		m_lpdd->RestoreDisplayMode();
	}

#ifdef __AUI_USE_DIRECTMEDIA__
	if (m_isCoinitialized)
	{
		CoUninitialize();
	}
#endif
}

AUI_ERRCODE aui_DirectUI::TearDownMouse(void)
{
	if (m_mouse)
	{
		m_mouse->GetAnimIndexes(&m_savedMouseAnimFirstIndex, &m_savedMouseAnimLastIndex);
		m_savedMouseAnimCurIndex = m_mouse->GetCurrentCursorIndex();
		m_savedMouseAnimDelay = (sint32)m_mouse->GetAnimDelay();

		if ( m_minimize || m_exclusiveMode )
		{
			SetCursorPos( m_mouse->X(), m_mouse->Y() );
		}

		m_mouse->End();
		delete m_mouse;
		m_mouse = NULL;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectUI::RestoreMouse(void)
{
	AUI_ERRCODE		auiErr;
	BOOL			exclusive = TRUE;

	aui_DirectMouse *mouse = new aui_DirectMouse( &auiErr, "CivMouse", exclusive );
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
		GetCursorPos( &point );
		m_mouse->SetPosition( &point );
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectUI::AltTabOut( void )
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
		DestroyDirectScreen();
	}

	while ( ShowCursor( TRUE ) < 0 )
		;

	if ( m_minimize || m_exclusiveMode )
	{
		while ( !IsIconic( m_hwnd ) )
			::ShowWindow( m_hwnd, SW_MINIMIZE );
	}

	if (g_civApp)
	{
		g_civApp->SetInBackground(TRUE);
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_DirectUI::AltTabIn( void )
{
	if ( !m_primary ) CreateDirectScreen( m_exclusiveMode );

	if ( m_minimize || m_exclusiveMode )
		while ( GetForegroundWindow() != m_hwnd )
			::ShowWindow( m_hwnd, SW_RESTORE );
	::ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);

	while ( ShowCursor( FALSE ) >= 0 )
		;

	if (g_exclusiveMode)
	{
		RestoreMouse();
	}
	else
	{
		if ( m_minimize || m_exclusiveMode )
		{
			POINT point;
			GetCursorPos( &point );
			m_mouse->SetPosition( &point );
		}

		if (m_mouse)
		{
			m_mouse->Acquire();
			m_mouse->Resume();
		}

		main_HideTaskBar();

		RECT clipRect = { 0, 0, m_width, m_height };
		ClipCursor(&clipRect);
	}

	if ( m_joystick ) m_joystick->Acquire();
	if (m_keyboard) m_keyboard->Acquire();

	if (g_civApp)
	{
		g_civApp->SetInBackground(FALSE);
	}

	return FlushDirtyList();
}

aui_MovieManager* aui_DirectUI::CreateMovieManager( void ) {
	return new aui_DirectMovieManager();
}

#endif  // __AUI_USE_DIRECTX__
