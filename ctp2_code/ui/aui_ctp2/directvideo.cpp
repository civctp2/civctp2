//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : DirectMedia video handling
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Allow movies to be displayed on a system without a sound card.
//
//----------------------------------------------------------------------------

#include "c3.h"

#ifdef __AUI_USE_DIRECTX__


#include "directvideo.h"

#include "aui.h"
#include "aui_directui.h"
#include "aui_window.h"
#include "aui_directsurface.h"
#include "c3errors.h"
#include "errors.h"
#include "RefTime.h"


DirectVideo::DirectVideo()
:
	m_isPlaying                 (false),
	m_isOpen                    (false),
	m_isFinished                (false),
	m_isPaused                  (false),
	m_isModal                   (false),
	m_mmStream                  (NULL),
	m_dd                        (NULL),
	m_primaryVidStream          (NULL),
	m_ddStream                  (NULL),
	m_primarySurface            (NULL),
	m_streamSurface             (NULL),
	m_streamSample              (NULL),
	m_flags                     (0),
	m_window                    (NULL),
    m_timePerFrame              (0),
	m_lastFrameTime             (0)
{
	RECT emptyRect  = {0,0,0,0};

	m_surfaceRect   = emptyRect;
	m_destRect      = emptyRect;
}

DirectVideo::~DirectVideo()
{
}


HRESULT DirectVideo::Initialize(aui_DirectUI *ui)
{
	Assert(ui != NULL);
	if (ui == NULL) return -1;

	LPDIRECTDRAW			dd = ui->DD();
	Assert(dd != NULL);
	if (dd == NULL) return -1;
	m_dd = dd;

	LPDIRECTDRAWSURFACE		primarySurface = ((aui_DirectSurface *)ui->Primary())->DDS();

	Assert(primarySurface != NULL);
	if (primarySurface == NULL) return -1;
	m_primarySurface = primarySurface;

	m_flags = DVFLAGS_FULLSCREEN;

	return 0;
}

HRESULT DirectVideo::Initialize(aui_DirectUI *ui, aui_Window *window, BOOL modal)
{
	Assert(ui != NULL);
	if (ui == NULL) return -1;

	LPDIRECTDRAW	dd = ui->DD();
	Assert(dd != NULL);
	if (dd == NULL) return -1;
	m_dd = dd;


	m_window = window;

	m_flags = DVFLAGS_INWINDOW;

	m_isModal = modal;

	LPDIRECTDRAWSURFACE		primarySurface;

	if (m_isModal) {
		
		primarySurface = ((aui_DirectSurface *)ui->Primary())->DDS();
	} else {
		
		primarySurface = ((aui_DirectSurface *)window->TheSurface())->DDS();
	}

	Assert(primarySurface != NULL);
	if (primarySurface == NULL) return -1;
	m_primarySurface = primarySurface;

	return 0;
}

HRESULT	DirectVideo::OpenStream(MBCHAR *name)
{

    IAMMultiMediaStream *pAMStream;
    HRESULT hr;
	TCHAR	errorText[_MAX_PATH];

    hr = CoCreateInstance(CLSID_AMMultiMediaStream, NULL, CLSCTX_INPROC_SERVER,
				 IID_IAMMultiMediaStream, (void **)&pAMStream);
	if (FAILED(hr)) {
		AMGetErrorText(hr, errorText, _MAX_PATH);
		c3errors_ErrorDialog("Video", "Could not create AMMultiMediaStream.  Error#%d.", hr);
		goto Exit;
	}

    hr = pAMStream->Initialize(STREAMTYPE_READ, 0, NULL);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Could not initialize IAMMultiMediaStream.  Error#%d.", hr);
		goto Exit;
	}

    hr = pAMStream->AddMediaStream(m_dd, &MSPID_PrimaryVideo, 0, NULL);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Could not add primary video stream to AMStream.  Error#%d.", hr);
		goto Exit;
	}

	hr = pAMStream->AddMediaStream(NULL, &MSPID_PrimaryAudio, AMMSF_ADDDEFAULTRENDERER, NULL);
	(void) hr;  // Ignore failures: display video without sound. 
	
    WCHAR       wPath[_MAX_PATH];
	MultiByteToWideChar(CP_ACP, 0, name, -1, wPath, sizeof(wPath)/sizeof(wPath[0]));

    hr = pAMStream->OpenFile(wPath, 0);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Unable to open the file.  Error#%d.", hr);
		goto Exit;
	}

    m_mmStream = pAMStream;

	
	

    hr = m_mmStream->GetMediaStream(MSPID_PrimaryVideo, &m_primaryVidStream);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Couldn't access the primary video stream.  Error#%d.", hr);
		goto Exit;
	}

	
    hr = m_primaryVidStream->QueryInterface(IID_IDirectDrawMediaStream, (void **)&m_ddStream);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Couldn't access the DirectDraw media stream.  Error#%d.", hr);
		goto Exit;
	}

	
	STREAM_TIME		streamTime;

	hr = m_ddStream->GetTimePerFrame(&streamTime);
	m_timePerFrame = (uint32)(streamTime / (UNITS / MILLISECONDS));

	
    hr = m_ddStream->CreateSample(NULL, NULL, 0, &m_streamSample);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Couldn't access the DirectDraw stream sample.  Error#%d.", hr);
		goto Exit;
	}

	
    hr = m_streamSample->GetSurface(&m_streamSurface, &m_surfaceRect);
	if (FAILED(hr)) {
		c3errors_ErrorDialog("Video", "Couldn't access the DirectDraw sample surface.  Error#%d.", hr);
		goto Exit;
	}

	m_isFinished = FALSE;
	m_isOpen = TRUE;

	return hr;


Exit:
	if (pAMStream != NULL)
		pAMStream->Release();

	m_mmStream = NULL;
	m_isFinished = FALSE;
	m_isOpen = FALSE;

	CloseStream();

    return hr;
}

HRESULT	DirectVideo::PlayAll(void)
{
	HRESULT hr = m_mmStream->SetState(STREAMSTATE_RUN);
	if (FAILED(hr)) return hr;

	
	m_isFinished = false;

	
    while (true) {
		if (m_streamSample->Update(0, NULL, NULL, 0) != S_OK) {
		    break;
		}
		hr = m_primarySurface->Blt(&m_destRect, m_streamSurface, &m_surfaceRect, DDBLT_WAIT, NULL);
    }

	return hr;
}

HRESULT	DirectVideo::PlayOne(void)
{
	HRESULT		hr = 0;

	if (m_isFinished) return hr;

	
	Assert(m_mmStream);

	if (!m_mmStream) {
		m_isPlaying = FALSE;

		return (HRESULT)-1;
	}

	hr = m_mmStream->SetState(STREAMSTATE_RUN);
	if (FAILED(hr)) return hr;

	m_isPlaying = true;

	if (m_lastFrameTime == 0) m_lastFrameTime = GetTickCount();
//	sint32 iterations = (GetTickCount() - m_lastFrameTime)/m_timePerFrame + 2;
	

	sint32	r = m_streamSample->Update(SSUPDATE_CONTINUOUS, NULL, NULL, 0);


	r = m_streamSample->CompletionStatus(COMPSTAT_WAIT, 0);
	if (r == MS_S_ENDOFSTREAM) {
		STREAM_TIME time = 0;

		m_mmStream->Seek(time);
	}

	hr = m_primarySurface->Blt(&m_destRect, m_streamSurface, &m_surfaceRect, DDBLT_WAIT, NULL);

	
	m_lastFrameTime = GetTickCount();

	
	if (m_flags == DVFLAGS_INWINDOW && !m_isModal) {
		Assert(m_window != NULL);
		if (m_window) {
			m_window->Invalidate(&m_destRect);
		}
	}

	return hr;
}

HRESULT	DirectVideo::CloseStream(void)
{
	HRESULT			hr = 0;

    if (m_primaryVidStream != NULL) {
		m_primaryVidStream->Release();
		m_primaryVidStream = NULL;
	}

    if (m_ddStream != NULL) {
		m_ddStream->Release();
		m_ddStream = NULL;
	}

    if (m_streamSample != NULL) {
		m_streamSample->Release();
		m_streamSample = NULL;	
	}

    if (m_streamSurface != NULL) {
		m_streamSurface->Release();
		m_streamSurface = NULL;
	}


	if (m_mmStream != NULL) {
		m_mmStream->Release();
		m_mmStream = NULL;
	}

	return hr;
}

HRESULT DirectVideo::Process(void)
{
	if (m_flags == DVFLAGS_INWINDOW) 
    {
        return (m_isModal) ? PlayAll() : PlayOne();
	}

	return 0;
}

void DirectVideo::Pause(void)
{
	if (m_isPlaying) 
    {
		m_isPaused = true;

		if (m_mmStream) 
        {
	        HRESULT hr = m_mmStream->SetState(STREAMSTATE_STOP);
			if (FAILED(hr)) return;
		}
	}
}

void DirectVideo::Resume(void)
{
	if (m_isPaused) 
    {
		if (m_mmStream) 
        {
			HRESULT hr = m_mmStream->SetState(STREAMSTATE_RUN);
			if (FAILED(hr)) return;
		}

		m_isPaused = false;
	}
}

#endif // __AUI_USE_DIRECTX__
