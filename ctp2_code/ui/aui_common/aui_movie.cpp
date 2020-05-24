//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Activision User Interface movie window
// Id           : $Id$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_ui.h"
#include "aui_surface.h"
#include "aui_rectangle.h"
#include "aui_mouse.h"

#include "aui_movie.h"

WNDPROC aui_Movie::m_windowProc = NULL;
aui_Movie *aui_Movie::m_onScreenMovie = NULL;

aui_Movie::aui_Movie() : aui_Base() {
	InitCommon();
}

aui_Movie::aui_Movie(
	AUI_ERRCODE *retval,
	const MBCHAR * filename )
	:
	aui_Base()
{
	*retval = InitCommon( filename );
	Assert( AUI_SUCCESS(*retval) );
}

void aui_Movie::InitCommon() {
	m_format = NULL;
	m_surface = NULL;
	m_isOpen = FALSE;
	m_isPlaying = FALSE;
	m_isFinished = FALSE;
	m_isPaused = FALSE;
	m_flags = 0;
	m_timePerFrame = 0;
	m_lastFrameTime = 0;

	m_windowSurface = NULL;
	memset( &m_rect, 0, sizeof( m_rect ) );
	memset(&m_windowRect, 0, sizeof(m_windowRect));

#ifdef __AUI_USE_DIRECTX__
	m_aviFile = NULL;
	m_aviStream = NULL;
	memset( &m_aviFileInfo, 0, sizeof( m_aviFileInfo ) );
	memset( &m_aviStreamInfo, 0, sizeof( m_aviStreamInfo ) );
	m_getFrame = NULL;
#endif
	m_curFrame = 0;
}

AUI_ERRCODE aui_Movie::InitCommon( const MBCHAR * filename )
{
	InitCommon();

	AUI_ERRCODE errcode = SetFilename( filename );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

#ifdef __AUI_USE_DIRECTX__
	AVIFileInit();
#endif

	return AUI_ERRCODE_OK;
}

aui_Movie::~aui_Movie()
{
	Unload();

#ifdef __AUI_USE_DIRECTX__
	AVIFileExit();
#endif
}

AUI_ERRCODE aui_Movie::SetFilename( const MBCHAR *filename )
{
	Unload();

	memset( m_filename, '\0', sizeof( m_filename ) );

	if ( !filename ) return AUI_ERRCODE_INVALIDPARAM;

	strncpy( m_filename, filename, MAX_PATH );

	m_format = (aui_MovieFormat *)
		g_ui->TheMemMap()->GetFileFormat( m_filename );
	Assert( m_format != NULL );
	if ( !m_format ) return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::Load( void )
{
	Assert(m_format != NULL);
	if (m_format == NULL) return AUI_ERRCODE_INVALIDPARAM;

	return m_format->Load( m_filename, this );
}

AUI_ERRCODE aui_Movie::Unload( void )
{
	Close();

	g_ui->TheMemMap()->ReleaseFileFormat(m_format);
	m_format = NULL;

	return AUI_ERRCODE_OK;
}

aui_Surface *aui_Movie::SetDestSurface( aui_Surface *surface )
{
	aui_Surface *   prevSurface = m_surface;
	m_surface   = surface;

	if (m_surface)
	{
		RECT surfRect =
		{
			0,
			0,
			m_surface->Width(),
			m_surface->Height()
		};

		Rectangle_Clip( &m_rect, &surfRect );
	}

	return prevSurface;
}

void aui_Movie::SetDestRect( RECT *rect )
{
	if ( rect )
		SetDestRect( rect->left, rect->top, rect->right, rect->bottom );
	else
		SetDestRect( 0, 0, 0, 0 );
}

void aui_Movie::SetDestRect(
	sint32 left,
	sint32 top,
	sint32 right,
	sint32 bottom )
{
	m_rect.left = left;
	m_rect.top = top;
	m_rect.right = right;
	m_rect.bottom = bottom;

	if ( m_surface )
	{
		RECT surfRect =
		{
			0,
			0,
			m_surface->Width(),
			m_surface->Height()
		};

		Rectangle_Clip( &m_rect, &surfRect );
	}
}

void aui_Movie::GetDestRect(
	sint32 *left,
	sint32 *top,
	sint32 *right,
	sint32 *bottom ) const
{
	*left = m_rect.left;
	*top = m_rect.top;
	*right = m_rect.right;
	*bottom = m_rect.bottom;
}

uint32 aui_Movie::SetTimePerFrame( uint32 timePerFrame )
{
	uint32 prevTimePerFrame = m_timePerFrame;
	m_timePerFrame = timePerFrame;
	return prevTimePerFrame;
}

AUI_ERRCODE aui_Movie::Open(
	uint32 flags,
	aui_Surface *surface,
	RECT *rect )
{
	if ( !m_isOpen )
	{
		m_flags = flags;

		if ( surface )
			SetDestSurface( surface );
		if ( rect )
			SetDestRect( rect );

#ifdef __AUI_USE_DIRECTX__
		uint32 err = AVIFileOpen
		             (
		              &m_aviFile,
		              m_filename,
		              OF_READ,
		              NULL
		             );

		Assert( err == 0 );
		if ( err ) return AUI_ERRCODE_HACK;

		err = AVIFileInfo(
			m_aviFile,
			&m_aviFileInfo,
			sizeof( m_aviFileInfo ) );
		Assert( err == 0 );
		if ( err ) return AUI_ERRCODE_HACK;

		err = AVIFileGetStream(
			m_aviFile,
			&m_aviStream,
			streamtypeVIDEO,
			0 );
		Assert( err == 0 );
		if ( err ) return AUI_ERRCODE_HACK;

		err = AVIStreamInfo(
			m_aviStream,
			&m_aviStreamInfo,
			sizeof( m_aviStreamInfo ) );
		Assert( err == 0 );
		if ( err ) return AUI_ERRCODE_HACK;

		BITMAPINFOHEADER bih;
		memset( &bih, 0, sizeof( bih ) );
		bih.biSize = sizeof( bih );
		bih.biWidth = m_aviFileInfo.dwWidth;
		bih.biHeight = m_aviFileInfo.dwHeight;
		bih.biPlanes = 1;
		bih.biBitCount = (uint16)g_ui->BitsPerPixel();
		bih.biCompression = BI_RGB;

		m_getFrame = AVIStreamGetFrameOpen(
			m_aviStream,
			&bih );
		Assert( m_getFrame != NULL );
		if ( !m_getFrame ) return AUI_ERRCODE_HACK;

		m_timePerFrame =
			m_aviStreamInfo.dwScale * 1000 / m_aviStreamInfo.dwRate;

		m_curFrame = m_aviStreamInfo.dwStart;

		m_rect.right = m_rect.left + m_aviStreamInfo.rcFrame.right;
		m_rect.bottom = m_rect.top + m_aviStreamInfo.rcFrame.bottom;
#endif
		m_isOpen = TRUE; //even if open failed, loops otherwise
		m_isPlaying = FALSE;
		m_isPaused = FALSE;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::Close( void )
{
	if ( m_isOpen )
	{

		Stop();

#ifdef __AUI_USE_DIRECTX__
		if ( m_getFrame )
		{
			AVIStreamGetFrameClose( m_getFrame );
			m_getFrame = NULL;
		}

		if ( m_aviStream )
		{
			AVIStreamRelease( m_aviStream );
			m_aviStream = NULL;
		}

		if ( m_aviFile )
		{
			AVIFileRelease( m_aviFile );
			m_aviFile = NULL;
		}
#endif

		m_isOpen = FALSE;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::Play( void )
{
	if ( !m_isPlaying )
	{
		Open();

#ifdef __AUI_USE_DIRECTX__
		uint32 err = AVIStreamBeginStreaming(
			m_aviStream,
			0,
			m_aviStreamInfo.dwLength,
			1000 );
		Assert( err == 0 );
		if ( err ) return AUI_ERRCODE_HACK;
#endif

		if ( m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN )
			PlayOnScreenMovie();
	}

	m_isPlaying = TRUE; // Regardless if open failed, loops otherwise
	m_isPaused = FALSE;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::PlayOnScreenMovie( void )
{
#ifdef __AUI_USE_DIRECTX__
	aui_Mouse *mouse = g_ui->TheMouse();
	sint32 numEvents;
	static aui_MouseEvent mouseEvents[ k_MOUSE_MAXINPUT ];
	aui_MouseEvent *mouseState = NULL;

	if (mouse) {

		numEvents = mouse->ManipulateInputs( mouseEvents, FALSE );

		mouseState = numEvents ?
			mouseEvents + numEvents - 1 :
			mouse->GetLatestMouseEvent();

		mouse->Hide();
	}

	MSG msg;
	m_windowProc = (WNDPROC)GetWindowLong( g_ui->TheHWND(), GWL_WNDPROC );
	SetWindowLong( g_ui->TheHWND(), GWL_WNDPROC, (LONG)OnScreenMovieWindowProc );

	m_onScreenMovie = this;

	while ( !m_isFinished && m_isPlaying )
	{
		while ( ShowCursor( FALSE ) >= 0 )
			;

		Process();

		if ( PeekMessage( &msg, g_ui->TheHWND(), 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if (mouse) {
			numEvents = mouse->ManipulateInputs( mouseEvents, FALSE );
			aui_MouseEvent *curEvent = mouseEvents;
			for ( sint32 k = numEvents; k; k--, curEvent++ )
			{
				if ( curEvent->lbutton != mouseState->lbutton
				||   curEvent->rbutton != mouseState->rbutton )
				{
					Stop();
					break;
				}
			}
		}
	}

	m_onScreenMovie = NULL;

	SetWindowLong( g_ui->TheHWND(), GWL_WNDPROC, (LONG)m_windowProc );
	m_windowProc = NULL;

	if (mouse)
		mouse->Show();

	g_ui->AddDirtyRect( &m_rect );  // Does this set the rect that needs to be repainted later on?
#endif

	return Stop();
}

AUI_ERRCODE aui_Movie::Stop( void )
{
	if(m_isPlaying)
	{
#ifdef __AUI_USE_DIRECTX__
		uint32 err = AVIStreamEndStreaming(m_aviStream);
		Assert(err == 0);
		if(err) return AUI_ERRCODE_HACK;
#endif
	}

	m_isPlaying = FALSE;
	m_isPaused  = FALSE;

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::Pause(void)
{
	if (m_isPlaying && !m_isPaused)
	{
		m_isPaused = TRUE;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::Resume(void)
{
	if (m_isPlaying && m_isPaused)
	{
		m_isPaused = FALSE;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_Movie::Process( void )
{
	AUI_ERRCODE retval = AUI_ERRCODE_UNHANDLED;

#ifdef __AUI_USE_DIRECTX__
	if(m_isPlaying && !m_isPaused)
	{
		uint32 time = GetTickCount();
		if(time - m_lastFrameTime > m_timePerFrame)
		{
			aui_Surface *surface =
				(m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN) ?
				g_ui->Secondary() :
				m_surface;
			uint8 *frame = (uint8 *)AVIStreamGetFrame(m_getFrame, m_curFrame);
			Assert(frame != NULL);
			if(!frame) return AUI_ERRCODE_HACK;

			BITMAPINFOHEADER *bih = (BITMAPINFOHEADER *)frame;

			sint32 bmpHeight = bih->biHeight;
			sint32 bmpByteWidth = bih->biWidth * (bih->biBitCount >> 3);
			sint32 bmpPitch = bmpByteWidth + (uint32(-bmpByteWidth) & 0x3);

			frame += sizeof(BITMAPINFOHEADER);

			uint8 *buffer = NULL;
			AUI_ERRCODE errcode = surface->Lock(&m_rect, (LPVOID *)&buffer, 0);
			Assert(AUI_SUCCESS(errcode));
			if(!AUI_SUCCESS(errcode)) return AUI_ERRCODE_SURFACELOCKFAILED;

			const uint8 *origBuffer = buffer;
			const sint32 pitch = surface->Pitch();

			if(bmpHeight > 0)
			{
				frame += bmpPitch * (bmpHeight - 1);

				for(sint32 i = bmpHeight; i; i--)
				{
					memcpy(buffer, frame, bmpByteWidth);
					buffer += pitch;
					frame -= bmpPitch;
				}
			}
			else
			{
				bmpHeight = -bmpHeight;

				for(sint32 i = bmpHeight; i; i--)
				{
					memcpy(buffer, frame, bmpByteWidth);
					buffer += pitch;
					frame += bmpPitch;
				}
			}

			errcode = surface->Unlock((LPVOID)origBuffer);
			Assert(AUI_SUCCESS(errcode));
			if(!AUI_SUCCESS(errcode)) return AUI_ERRCODE_SURFACEUNLOCKFAILED;

			if(++m_curFrame == m_aviStreamInfo.dwLength)
			{
				m_curFrame = 0;
				if(!(m_flags & k_AUI_MOVIE_PLAYFLAG_LOOP))
					m_isFinished = TRUE;
			}

			m_lastFrameTime = time;

			retval = AUI_ERRCODE_HANDLED;
		}
	}
#endif

	return retval;
}

#ifdef __AUI_USE_DIRECTX__
LRESULT CALLBACK OnScreenMovieWindowProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam )
{
	if ( aui_Movie::m_onScreenMovie )
	{
		g_ui->HandleWindowsMessage( hwnd, message, wParam, lParam );

		switch ( message )
		{
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
			aui_Movie::m_onScreenMovie->Stop();
			break;

		case WM_CLOSE:
			if ( hwnd != g_ui->TheHWND() ) break;

			aui_Movie::m_onScreenMovie->Close();

			PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );

			return 0;
		}
	}

	return CallWindowProc(
	        (long(__stdcall *)(HWND, unsigned int, unsigned int, long))aui_Movie::m_windowProc,
	         hwnd, message, wParam, lParam );
}
#endif
