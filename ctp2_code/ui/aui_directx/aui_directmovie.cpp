//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectMedia movie handling
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
// __AUI_USE_DIRECTMEDIA__	
// should be set to have any meaningful code at all
//
// ACTIVISION_ORIGINAL		
// when set, generates the original Activision code
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Allow movies to be displayed on a system without a sound card.
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "c3ui.h"

#ifdef __AUI_USE_DIRECTMEDIA__


#include "aui_directui.h"
#include "aui_directsurface.h"

#include "aui_directmovie.h"

#include "CivPaths.h"
#include "SoundManager.h"



extern CivPaths *g_civPaths;
extern C3UI		*g_c3ui;
extern SoundManager *g_soundManager;

aui_DirectMovie::aui_DirectMovie(
	AUI_ERRCODE *retval,
	MBCHAR *filename )
	:
	aui_Movie( retval, filename )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



AUI_ERRCODE aui_DirectMovie::InitCommon( void )
{
	m_mmStream = NULL;
	m_primaryVidStream = NULL;
	m_ddStream = NULL;
	m_streamSample = NULL;
	m_streamSurface = NULL;
	m_endOfMovieTickCount = 0;

	memset( &m_streamRect, 0, sizeof( m_streamRect ) );

	return AUI_ERRCODE_OK;
}



aui_DirectMovie::~aui_DirectMovie()
{
	Close();
}




AUI_ERRCODE aui_DirectMovie::Open(
	uint32 flags,
	aui_Surface *surface,
	RECT *rect )
{
	if ( !m_isOpen )
	{
		m_flags = flags;

		g_soundManager->ReleaseSoundDriver();

		if ( surface )
			SetDestSurface( surface );
		if ( rect )
			SetDestRect( rect );

		HRESULT hr;
		IAMMultiMediaStream *temp = NULL;

		hr = CoCreateInstance(
			CLSID_AMMultiMediaStream,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IAMMultiMediaStream,
			(void **)&temp );

		
		m_mmStream = temp;

		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		hr = temp->Initialize(
			STREAMTYPE_READ,
			0,
			NULL );
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		hr = temp->AddMediaStream(
			((aui_DirectUI *)g_ui)->DD(),
			&MSPID_PrimaryVideo,
			0,
			NULL );
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		hr = temp->AddMediaStream(
			NULL,
			&MSPID_PrimaryAudio,
			AMMSF_ADDDEFAULTRENDERER,
			NULL );
#if defined(ACTIVISION_ORIGINAL)
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}
#else
        (void) hr;  // Ignore audio failures: display movie without sound.
#endif
		
		char fullPath[_MAX_PATH];
		strcpy(fullPath, m_filename);






		WCHAR wPath[ MAX_PATH + 1 ];
		MultiByteToWideChar(
			CP_ACP,
			0,
			fullPath,
			-1,
			wPath,
			sizeof(wPath)/sizeof(wPath[0]) );

		hr = temp->OpenFile(
			wPath,
			0 );



		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		

		hr = m_mmStream->GetMediaStream(
			MSPID_PrimaryVideo,
			&m_primaryVidStream );
		Assert( !FAILED(hr) );
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		
		hr = m_primaryVidStream->QueryInterface(
			IID_IDirectDrawMediaStream,
			(void **)&m_ddStream );
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		
		
		STREAM_TIME streamTime = 0;
		hr = m_ddStream->GetTimePerFrame( &streamTime );
		m_timePerFrame = uint32(streamTime / ( UNITS / MILLISECONDS ));

		
		hr = m_ddStream->CreateSample(
			NULL,
			NULL,
			0,
			&m_streamSample );
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		
		hr = m_streamSample->GetSurface(
			&m_streamSurface,
			&m_streamRect );
		if ( FAILED(hr) )
		{
			Close();
			return AUI_ERRCODE_LOADFAILED;
		}

		DDSURFACEDESC		sd;
		sd.dwSize = sizeof(sd);
		m_streamSurface->GetSurfaceDesc(&sd);

		m_isOpen = TRUE;
		m_isPlaying = FALSE;
		m_isPaused = FALSE;
	}

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE aui_DirectMovie::TargetSurface(aui_DirectSurface *surface)
{

	m_streamSample->Release();
	m_streamSample = NULL;
	m_streamSurface = NULL;

	HRESULT hr;

	
	hr = m_ddStream->CreateSample(
		surface->DDS(),
		NULL,
		0,
		&m_streamSample );
	Assert( !FAILED(hr) );
	if ( FAILED(hr) )
	{
		return AUI_ERRCODE_LOADFAILED;
	}

	
	hr = m_streamSample->GetSurface(
		&m_streamSurface,
		&m_streamRect );
	Assert( !FAILED(hr) );
	if ( FAILED(hr) )
	{
		return AUI_ERRCODE_LOADFAILED;
	}

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_DirectMovie::Close( void )
{
	
	Stop();

	if ( m_primaryVidStream )
	{
		m_primaryVidStream->Release();
		m_primaryVidStream = NULL;
	}

	if ( m_ddStream )
	{
		m_ddStream->Release();
		m_ddStream = NULL;
	}

	if ( m_streamSample )
	{
		m_streamSample->Release();
		m_streamSample = NULL;
	}

	if ( m_streamSurface )
	{
		m_streamSurface->Release();
		m_streamSurface = NULL;
	}

	if ( m_mmStream )
	{
		m_mmStream->Release();
		m_mmStream = NULL;
	}

	m_isOpen = FALSE;

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectMovie::Play( void )
{
	if ( !m_isPlaying )
	{
		
		Open();

		m_isPlaying = TRUE;
		m_isPaused = FALSE;

		
		
		if ( m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN ) {

			sint32		savedWidth = g_c3ui->Width(), 
						savedHeight = g_c3ui->Height();

			
			g_c3ui->TearDownMouse();

			
			g_c3ui->DestroyDirectScreen();
			g_c3ui->SetWidth(640);
			g_c3ui->SetHeight(480);
			g_c3ui->CreateDirectScreen(g_c3ui->GetExclusiveMode());

			HRESULT hr = m_mmStream->SetState( STREAMSTATE_RUN );
			Assert( !FAILED(hr) );
			if ( FAILED(hr) )
				return AUI_ERRCODE_HACK;

			
			while ( ShowCursor( FALSE ) >= 0 )
				;

			
			
			
			if (m_mmStream) {
				STREAM_TIME st = 0;
				HRESULT hr = m_mmStream->GetDuration(&st);
				Assert(!FAILED(hr));
				if (!FAILED(hr)) {
					uint32 ms = uint32((double)st / ( (double)UNITS / (double)MILLISECONDS ));
					m_endOfMovieTickCount = GetTickCount() + ms;
				}
			}

			PlayOnScreenMovie();

			
			g_c3ui->DestroyDirectScreen();
			g_c3ui->SetWidth(savedWidth);
			g_c3ui->SetHeight(savedHeight);
			g_c3ui->CreateDirectScreen(g_c3ui->GetExclusiveMode());

			g_c3ui->RestoreMouse();
		} else {
		
			HRESULT hr = m_mmStream->SetState( STREAMSTATE_RUN );
			Assert( !FAILED(hr) );
			if ( FAILED(hr) )
				return AUI_ERRCODE_HACK;

			
			
			
			if (m_mmStream) {
				STREAM_TIME st = 0;
				HRESULT hr = m_mmStream->GetDuration(&st);
				Assert(!FAILED(hr));
				if (!FAILED(hr)) {
					uint32 ms = uint32((double)st / ( (double)UNITS / (double)MILLISECONDS ));
					m_endOfMovieTickCount = GetTickCount() + ms;
				}
			}

			PlayOnScreenMovie();
		}

	}

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectMovie::Stop( void )
{
	if ( m_isPlaying )
	{
		


		
		
		
		
		if (m_flags & k_AUI_MOVIE_PLAYFLAG_PLAYANDHOLD) {
			if (m_windowSurface) {

				
				
				
				
				STREAM_TIME		st;

				HRESULT hr = m_mmStream->GetDuration(&st);
				Assert(!FAILED(hr));
				
				hr = m_mmStream->Seek(st);
				Assert(!FAILED(hr));

				hr = m_streamSample->Update(
					SSUPDATE_CONTINUOUS,
					NULL,
					NULL,
					0 );
				Assert(!FAILED(hr));
	
				hr = m_streamSample->CompletionStatus( COMPSTAT_WAIT, 0 );
				Assert(!FAILED(hr));

				LPDIRECTDRAWSURFACE lpdds =
					((aui_DirectSurface *)m_windowSurface)->DDS();

				hr = lpdds->Blt(
					&m_windowRect,
					m_streamSurface,
					&m_streamRect,
					DDBLT_WAIT,
					NULL );
				Assert( !FAILED(hr) );
			}
		}

		HRESULT hr = m_mmStream->SetState( STREAMSTATE_STOP );
		Assert( !FAILED(hr) );
		if ( FAILED(hr) )
			return AUI_ERRCODE_SUSPENDFAILED;

		m_isPlaying = FALSE;
		m_isPaused = FALSE;
		m_isFinished = TRUE;

	}

	
	g_soundManager->ReacquireSoundDriver();

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectMovie::Pause( void )
{
	if ( m_isPlaying && !m_isPaused )
	{
		HRESULT hr = m_mmStream->SetState( STREAMSTATE_STOP );
		Assert( !FAILED(hr) );
		if ( FAILED(hr) )
			return AUI_ERRCODE_SUSPENDFAILED;

		m_isPaused = TRUE;
	}

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectMovie::Resume( void )
{
	if ( m_isPlaying && m_isPaused )
	{
		HRESULT hr = m_mmStream->SetState( STREAMSTATE_RUN );
		Assert( !FAILED(hr) );
		if ( FAILED(hr) )
			return AUI_ERRCODE_RESUMEFAILED;

		m_isPaused = FALSE;
	}

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE aui_DirectMovie::Process( void )
{
	AUI_ERRCODE retval = AUI_ERRCODE_UNHANDLED;

	if ( m_isPlaying && !m_isPaused && !m_isFinished )
	{

		sint32	r;

		
		if (!m_streamSample) return AUI_ERRCODE_UNHANDLED;

		r = m_streamSample->Update(
			SSUPDATE_CONTINUOUS,
			NULL,
			NULL,
			0 );

		r = m_streamSample->CompletionStatus( COMPSTAT_WAIT, 0 );
		if ( r == MS_S_ENDOFSTREAM )
		{
			if (GetTickCount() >= m_endOfMovieTickCount) {
				if (m_flags & k_AUI_MOVIE_PLAYFLAG_LOOP) {
					
					if (m_mmStream)
						m_mmStream->Seek( 0 );
				} else {
					Stop();
				}
			}
		}

		
		if (m_flags & k_AUI_MOVIE_PLAYFLAG_ONSCREEN) {
			
			
			if (!g_ui) return AUI_ERRCODE_UNHANDLED;
			if (!g_ui->Primary()) return AUI_ERRCODE_UNHANDLED;

			LPDIRECTDRAWSURFACE lpdds =
					((aui_DirectSurface *)g_ui->Primary())->DDS();

			RECT surfRect = {0, 0, g_ui->Primary()->Width(), g_ui->Primary()->Height()};
			RECT destRect = {0, 0, m_streamRect.right * 2, m_streamRect.bottom * 2};

			OffsetRect(&destRect, 
						(surfRect.right - surfRect.left)/2 - (destRect.right - destRect.left)/2,
						(surfRect.bottom - surfRect.top)/2 - (destRect.bottom - destRect.top)/2);
			if(destRect.left < 0) destRect.left = 0;
			if(destRect.right > 640) destRect.right = 640;
			if(destRect.top < 0) destRect.top = 0;
			if(destRect.bottom > 480) destRect.bottom = 480;

			HRESULT hr = lpdds->Blt(
				&destRect,
				m_streamSurface,
				&m_streamRect,
				DDBLT_WAIT,
				NULL );
			if(FAILED(hr)) {

			}
			Assert( !FAILED(hr) );
		} else {
			
			
			
			LPDIRECTDRAWSURFACE lpdds =
					((aui_DirectSurface *)m_surface)->DDS();

			HRESULT hr = lpdds->Blt(
				&m_rect,
				m_streamSurface,
				&m_streamRect,
				DDBLT_WAIT,
				NULL );
			Assert( !FAILED(hr) );
		}

		
		m_lastFrameTime = GetTickCount();

		retval = AUI_ERRCODE_HANDLED;
	}

	return retval;
}


#endif 
