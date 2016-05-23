#include "c3.h"
#include "aui_ui.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_rectangle.h"

#include "aui_win.h"


BOOL aui_Win::m_registered = FALSE;
MBCHAR *aui_Win::m_windowClass = "aui_Win";
sint32 aui_Win::m_winRefCount = 0;
tech_WLList<aui_Win *> *aui_Win::m_winList = 0;
aui_Win *g_winFocus = 0;

aui_Win::aui_Win(
	AUI_ERRCODE *retval,
	uint32 id,
	MBCHAR *ldlBlock,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( ldlBlock ),
	aui_TextBase( ldlBlock, (const MBCHAR *)NULL ),
	aui_Control( retval, id, ldlBlock, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl( ldlBlock );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


aui_Win::aui_Win(
	AUI_ERRCODE *retval,
	uint32 id,
	sint32 x,
	sint32 y,
	sint32 width,
	sint32 height,
	ControlActionCallback *ActionFunc,
	void *cookie )
	:
	aui_ImageBase( (sint32)0 ),
	aui_TextBase( NULL ),
	aui_Control( retval, id, x, y, width, height, ActionFunc, cookie )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}


AUI_ERRCODE aui_Win::InitCommonLdl( MBCHAR *ldlBlock )
{
	return InitCommon();
}


AUI_ERRCODE aui_Win::InitCommon( void )
{
	m_hwnd = NULL;
	m_memdc = NULL;
	m_hbitmap = NULL;
	m_hbitmapOld = NULL;
	memset( &m_offscreen, 0, sizeof( m_offscreen ) );

	if ( !m_registered )
	{
#ifdef __AUI_USE_DIRECTX__
		WNDCLASSEX wcex;
		memset( &wcex, 0, sizeof( wcex ) );

		wcex.cbSize =			sizeof( wcex );
		wcex.style =			CS_DBLCLKS | CS_OWNDC;
		wcex.lpfnWndProc =		DefWindowProc;
		wcex.hInstance =		g_ui->TheHINSTANCE();
		wcex.lpszClassName =	m_windowClass;

		m_registered = RegisterClassEx( &wcex );
		Assert( m_registered );
#else
		m_registered = TRUE;
#endif // __AUI_USE_DIRECTX
	}
#ifdef __AUI_USE_DIRECTX
	HDC hdc = GetDC( g_ui->TheHWND() );

	m_memdc = CreateCompatibleDC( hdc );
	Assert( m_memdc != NULL );
	if ( !m_memdc ) return AUI_ERRCODE_MEMALLOCFAILED;

	m_hbitmap = CreateCompatibleBitmap( hdc, m_width, m_height );
	Assert( m_hbitmap != NULL );
	if ( !m_hbitmap ) return AUI_ERRCODE_MEMALLOCFAILED;

	ReleaseDC( g_ui->TheHWND(), hdc );

	m_hbitmapOld = (HBITMAP)SelectObject( m_memdc, m_hbitmap );

	RECT rect = { 0, 0, m_width, m_height };
	FillRect( m_memdc, &rect, GetSysColorBrush( COLOR_WINDOW ) );
#endif // __AUI_USE_DIRECTX__

	m_offscreen.x = g_ui->Width() + 1;
	m_offscreen.y = 0;

	RECT playground;
	if ( !m_winRefCount++ )
	{
		SetRect( &playground, 0, 0, g_ui->Width(), g_ui->Height() );

		m_winList = new tech_WLList<aui_Win *>;
		Assert( m_winList != NULL );
		if ( !m_winList ) return AUI_ERRCODE_MEMALLOCFAILED;
	}
#ifdef __AUI_USE_DIRECTX__
	else
		GetClipCursor( &playground );
#endif // __AUI_USE_DIRECTX__

	m_winList->AddTail( this );

	RECT morePlayground =
	{
		m_offscreen.x,
		m_offscreen.y,
		m_offscreen.x + m_width,
		m_offscreen.y + m_height
	};

	Rectangle_Consolidate( &playground, &playground, &morePlayground );

#ifdef __AUI_USE_DIRECTX__
	BOOL clipped = ClipCursor( &playground );
	Assert( clipped );

	MoveWindow(
		g_ui->TheHWND(),
		playground.left, playground.top, playground.right, playground.bottom,
		FALSE );
#endif // __AUI_USE_DIRECTX__
	return AUI_ERRCODE_OK;
}


aui_Win::~aui_Win()
{
#ifdef __AUI_USE_DIRECTX__
	if ( m_memdc )
	{
		if ( m_hbitmap )
		{
			SelectObject( m_memdc, m_hbitmapOld );
			DeleteObject( m_hbitmap );
		}

		DeleteDC( m_memdc );
	}

	if ( m_hwnd )
	{
		g_ui->RemoveWin( m_hwnd );
		DestroyWindow( m_hwnd );
		m_hwnd = NULL;
	}
#endif // __AUI_USE_DIRECTX__

	ListPos position = m_winList->Find( GetWinFromHWND(m_hwnd) );
	if ( position )
		m_winList->DeleteAt( position );

	if ( !--m_winRefCount )
	{
#ifdef __AUI_USE_DIRECTX__
		ClipCursor( NULL );

		if ( m_registered )
			UnregisterClass( m_windowClass, g_ui->TheHINSTANCE() );
#endif

		if ( m_winList )
		{
			delete m_winList;
			m_winList = NULL;
		}
	}
}


aui_Control *aui_Win::SetKeyboardFocus( void )
{
	if ( !IsDisabled() )
#ifdef __AUI_USE_DIRECTX__
		SetFocus( m_hwnd );
#else // __AUI_USE_DIRECTX__
		g_winFocus = this;
#endif // __AUI_USE_DIRECTX__

	return aui_Control::SetKeyboardFocus();
}


aui_Win *aui_Win::GetWinFromHWND( HWND hwnd )
{
	aui_Win *win = NULL;

	ListPos position = m_winList->GetHeadPosition();
	for ( sint32 i = m_winList->L(); i; i-- )
	{
		win = m_winList->GetNext( position );
		if ( win->TheHWND() == hwnd )
			break;
	}

	return win;
}


AUI_ERRCODE aui_Win::DrawThis( aui_Surface *surface, sint32 x, sint32 y )
{

	if ( IsHidden() ) return AUI_ERRCODE_OK;

	if ( !surface ) surface = m_window->TheSurface();

	RECT rect = { 0, 0, m_width, m_height };
	OffsetRect( &rect, m_x + x, m_y + y );
	ToWindow( &rect );

	if ( m_hwnd && m_memdc )
	{
#ifdef __AUI_USE_DIRECTX__
		InvalidateRect( m_hwnd, NULL, FALSE );
		SendMessage( m_hwnd, WM_PAINT, (WPARAM)m_memdc, 0 );

		HDC destDC = NULL;
		AUI_ERRCODE errcode = surface->GetDC( &destDC );
		Assert( errcode == AUI_ERRCODE_OK );

		if ( destDC )
		{

			BitBlt(
				destDC,
				rect.left,
				rect.top,
				rect.right - rect.left,
				rect.bottom - rect.top,
				m_memdc,
				0,
				0,
				SRCCOPY );

			errcode = surface->ReleaseDC( destDC );
			Assert( errcode == AUI_ERRCODE_OK );
		}
#endif // __AUI_USE_DIRECTX__
	}

	if ( surface == m_window->TheSurface() )
		m_window->AddDirtyRect( &rect );

	return AUI_ERRCODE_OK;
}








void aui_Win::MouseMoveInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( GetWhichSeesMouse() && GetWhichSeesMouse() != this )
		MouseMoveAway( mouseData );
	else if ( !IsActive() )
		MouseMoveOver( mouseData );
	else
		WinMouseMove( mouseData );
}


void aui_Win::MouseMoveOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;

	aui_Control::MouseMoveOver( mouseData );
	if ( GetWhichSeesMouse() == this ) WinMouseMove( mouseData );
}


void aui_Win::MouseMoveAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	aui_Control::MouseMoveAway( mouseData );
	WinMouseMove( mouseData );
}


void aui_Win::MouseMoveOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;

}


void aui_Win::WinMouseMove( aui_MouseEvent *mouseData )
{
	POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
	POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
	SendMessage(
		m_hwnd,
		WM_NCHITTEST,
		0,
		screen.x + ( screen.y << 16 ) );
	SendMessage(
		m_hwnd,
		WM_MOUSEMOVE,
		0,
		local.x + ( local.y << 16 ) );
#endif // __AUI_USE_DIRECTX__

	if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
		m_mouseCode = AUI_ERRCODE_HANDLED;
}


void aui_Win::MouseLDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseLDrag( mouseData );
}


void aui_Win::MouseLDragAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseLDrag( mouseData );
}


void aui_Win::MouseLDragInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseLDrag( mouseData );
}


void aui_Win::MouseLDragOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseLDrag( mouseData );
}


void aui_Win::WinMouseLDrag( aui_MouseEvent *mouseData )
{
	if ( GetMouseOwnership() == this )
	{
		POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
		POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
		WPARAM wParam = MK_LBUTTON;
		if ( mouseData->rbutton ) wParam |= MK_RBUTTON;

		SendMessage(
			m_hwnd,
			WM_MOUSEMOVE,
			wParam,
			local.x + ( local.y << 16 ) );
#endif //__AUI_USE_DIRECTX__

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDRAGOVER;
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}


void aui_Win::MouseRDragOver( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseRDrag( mouseData );
}


void aui_Win::MouseRDragAway( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseRDrag( mouseData );
}


void aui_Win::MouseRDragInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseRDrag( mouseData );
}


void aui_Win::MouseRDragOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	WinMouseRDrag( mouseData );
}


void aui_Win::WinMouseRDrag( aui_MouseEvent *mouseData )
{
	if ( GetMouseOwnership() == this )
	{
		POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
		POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
		WPARAM wParam = MK_RBUTTON;
		if ( mouseData->lbutton ) wParam |= MK_LBUTTON;

		SendMessage(
			m_hwnd,
			WM_MOUSEMOVE,
			wParam,
			local.x + ( local.y << 16 ) );
#endif //__AUI_USE_DIRECTX__
		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSERDRAGOVER;
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;
	}
}


void aui_Win::MouseLGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		SetMouseOwnership();

		SetKeyboardFocus();

		POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
		POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
		BOOL set = SetCursorPos( screen.x, screen.y );
		Assert( set );

		WPARAM wParam = MK_LBUTTON;
		if ( mouseData->rbutton ) wParam |= MK_RBUTTON;

		SendMessage(
			m_hwnd,
			WM_NCHITTEST,
			0,
			screen.x + ( screen.y << 16 ) );

		SendMessage(
			m_hwnd,
			WM_MOUSEACTIVATE,
			(WPARAM)g_ui->TheHWND(),
			HTCLIENT + ( WM_LBUTTONDOWN << 16 ) );

		SendMessage(
			m_hwnd,
			WM_LBUTTONDOWN,
			wParam,
			local.x + ( local.y << 16 ) );

		SendMessage(
			m_hwnd,
			WM_NCHITTEST,
			0,
			screen.x + ( screen.y << 16 ) );

		SendMessage(
			m_hwnd,
			WM_GETDLGCODE,
			0,
			0 );

		SendMessage(
			m_hwnd,
			WM_MOUSEMOVE,
			wParam,
			local.x + ( local.y << 16 ) );
#endif // __AUI_USE_DIRECTX__

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELGRABINSIDE;
		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
	else
		MouseLGrabOutside( mouseData );
}


void aui_Win::MouseLGrabOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;

	if ( GetKeyboardFocus() == this ) ReleaseKeyboardFocus();
}


void aui_Win::MouseLDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( GetMouseOwnership() == this )
	{
		ReleaseMouseOwnership();

		PlaySound( AUI_SOUNDBASE_SOUND_EXECUTE );

		POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
		POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
		BOOL set = SetCursorPos( screen.x, screen.y );
		Assert( set );

		WPARAM wParam = 0;
		if ( mouseData->rbutton ) wParam |= MK_RBUTTON;

		SendMessage(
			m_hwnd,
			WM_LBUTTONUP,
			wParam,
			local.x + ( local.y << 16 ) );

		SendMessage(
			m_hwnd,
			WM_NCHITTEST,
			0,
			screen.x + ( screen.y << 16 ) );
#endif

		WinMouseMove( mouseData );

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDROPINSIDE;
		if ( m_mouseCode == AUI_ERRCODE_UNHANDLED )
			m_mouseCode = AUI_ERRCODE_HANDLED;

		HandleGameSpecificLeftClick( this );
	}
}

void aui_Win::MouseLDropOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	MouseLDropInside( mouseData );
}


void aui_Win::MouseRGrabInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
}


void aui_Win::MouseRGrabOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
}


void aui_Win::MouseRDropInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this ) {
		SetWhichSeesMouse( this );

		HandleGameSpecificRightClick((void *)this);
		m_mouseCode = AUI_ERRCODE_HANDLED;
	}
	else {
		MouseRDropOutside( mouseData );
	}
}


void aui_Win::MouseRDropOutside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
}


void aui_Win::MouseLDoubleClickInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		SetMouseOwnership();

		POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
		POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
		BOOL set = SetCursorPos( screen.x, screen.y );
		Assert( set );

		WPARAM wParam = MK_LBUTTON;
		if ( mouseData->rbutton ) wParam |= MK_RBUTTON;

		SendMessage(
			m_hwnd,
			WM_NCHITTEST,
			0,
			screen.x + ( screen.y << 16 ) );

		SendMessage(
			m_hwnd,
			WM_MOUSEACTIVATE,
			(WPARAM)g_ui->TheHWND(),
			HTCLIENT + ( WM_LBUTTONDOWN << 16 ) );

		SendMessage(
			m_hwnd,
			WM_LBUTTONDBLCLK,
			wParam,
			local.x + ( local.y << 16 ) );
#endif // __AUI_USE_DIRECTX__

		WinMouseMove( mouseData );

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSELDOUBLECLICKINSIDE;
		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
}


void aui_Win::MouseRDoubleClickInside( aui_MouseEvent *mouseData )
{
	if ( IsDisabled() ) return;
	if ( !GetWhichSeesMouse() || GetWhichSeesMouse() == this )
	{
		SetWhichSeesMouse( this );

		PlaySound( AUI_SOUNDBASE_SOUND_ENGAGE );

		SetMouseOwnership();

		POINT local =
		{ mouseData->position.x - m_x, mouseData->position.y - m_y };
		POINT screen =
		{ local.x + m_offscreen.x, local.x + m_offscreen.y };

#ifdef __AUI_USE_DIRECTX__
		BOOL set = SetCursorPos( screen.x, screen.y );
		Assert( set );

		WPARAM wParam = MK_RBUTTON;
		if ( mouseData->lbutton ) wParam |= MK_LBUTTON;

		SendMessage(
			m_hwnd,
			WM_NCHITTEST,
			0,
			screen.x + ( screen.y << 16 ) );

		SendMessage(
			m_hwnd,
			WM_MOUSEACTIVATE,
			(WPARAM)g_ui->TheHWND(),
			HTCLIENT + ( WM_LBUTTONDOWN << 16 ) );

		SendMessage(
			m_hwnd,
			WM_RBUTTONDBLCLK,
			wParam,
			local.x + ( local.y << 16 ) );
#endif // __AUI_USE_DIRECTX__

		WinMouseMove( mouseData );

		m_draw |= m_drawMask & k_AUI_REGION_DRAWFLAG_MOUSERDOUBLECLICKINSIDE;
		m_mouseCode = AUI_ERRCODE_HANDLEDEXCLUSIVE;
	}
}
