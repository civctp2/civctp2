//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Video player
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Changed string handling to avoid using deprecated functions.
//
//----------------------------------------------------------------------------

#include "c3.h"

#define STRSAFE_NO_DEPRECATE

#include "streams.h"

#include "mpconfig.h"
#include "VidPlay.h"





CBaseVideoPlayer::CBaseVideoPlayer()
:   m_achFileName    ()
{
    m_eState = Uninitialized ;
    m_pGraph = NULL ;
    m_pMC = NULL ;
    m_pME = NULL ;
    
    m_dwColorKey = 253 ;  
}





CBaseVideoPlayer::~CBaseVideoPlayer()
{
    ReleaseInterfaces() ;
}






HRESULT CBaseVideoPlayer::GetInterfaces(HWND hWndApp)
{
    HRESULT  hr ;
    hr = m_pGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMC) ;
    ASSERT(SUCCEEDED(hr) && m_pMC) ;
    
    hr = m_pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *)&m_pME) ;
    ASSERT(SUCCEEDED(hr) && m_pME) ;
    
    
    
    
    
    hr = m_pME->SetNotifyWindow((OAHWND) hWndApp, WM_PLAY_EVENT, 
        (ULONG)(LPVOID)m_pME) ;
    ASSERT(SUCCEEDED(hr)) ;
    
    m_eState = Stopped ;  
    
    return S_OK ;
}





void CBaseVideoPlayer::ReleaseInterfaces(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CBaseVideoPlayer::ReleaseInterfaces() entered"))) ;

    if (m_pMC) {
        m_pMC->Release() ;
        m_pMC = NULL ;
    }
    if (m_pME) {
        
        m_pME->SetNotifyWindow(NULL, WM_PLAY_EVENT, (ULONG)(LPVOID)m_pME) ;
        m_pME->Release() ;
        m_pME = NULL ;
    }
    
    m_eState = Uninitialized ;  
}





void CBaseVideoPlayer::WaitForState(FILTER_STATE State)
{
    
    LONG   lfs ;
    do
    {
        m_pMC->GetState(10, &lfs) ;
    } while (State != lfs) ;
}





BOOL CBaseVideoPlayer::Play(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CBaseVideoPlayer::Play() entered"))) ;
    
    if (! IsGraphReady() )
    {
        DbgLog((LOG_TRACE, 1, TEXT("DVD-Video playback graph hasn't been built yet"))) ;
        return FALSE ;
    }
    HRESULT  hr = m_pMC->Run() ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IMediaControl::Run() failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }
    WaitForState(State_Running) ;
    
    
    m_eState = Playing ;
    
    return TRUE ;  
}





BOOL CBaseVideoPlayer::Pause(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CBaseVideoPlayer::Pause() entered"))) ;
    
    if (! IsGraphReady() )
    {
        DbgLog((LOG_TRACE, 1, TEXT("DVD-Video playback graph hasn't been built yet"))) ;
        return FALSE ;
    }
    HRESULT  hr = m_pMC->Pause() ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IMediaControl::Pause() failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }
    WaitForState(State_Paused) ;
    
    
    m_eState = Paused ;
    
    return TRUE ;  
}





BOOL CBaseVideoPlayer::Stop(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CBaseVideoPlayer::Stop() entered"))) ;
    
    if (! IsGraphReady() )
    {
        DbgLog((LOG_TRACE, 1, TEXT("DVD-Video playback graph hasn't been built yet"))) ;
        return FALSE ;
    }
    HRESULT  hr = m_pMC->Stop() ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: IMediaControl::Stop() failed (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }
    WaitForState(State_Stopped) ;
    
    
    m_eState = Stopped ;
    
    return TRUE ;  
}






HRESULT CBaseVideoPlayer::GetColorKey(DWORD *pdwColorKey)
{
    DbgLog((LOG_TRACE, 5, TEXT("CBaseVideoPlayer::GetColorKey() entered"))) ;
    
    if (! IsGraphReady() )
        return E_UNEXPECTED ;
    
    *pdwColorKey = m_dwColorKey ;  
    return S_OK ;
}






CDVDPlayer::CDVDPlayer()
:	CBaseVideoPlayer  ()
{
    DbgLog((LOG_TRACE, 3, TEXT("CDVDPlayer c-tor entered"))) ;
    
    m_pDvdGB = NULL ;
    m_pDvdC  = NULL ;
    m_pDvdI  = NULL ;
}





CDVDPlayer::~CDVDPlayer()
{
    DbgLog((LOG_TRACE, 3, TEXT("CDVDPlayer d-tor entered"))) ;
    
    ReleaseInterfaces() ;
    
    if (m_pDvdGB)
        m_pDvdGB->Release() ;
    
    DbgLog((LOG_TRACE, 3, TEXT("CDVDPlayer d-tor exiting..."))) ;
}






BOOL CDVDPlayer::Initialize(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::Initialize() entered"))) ;
    
    
    
    
    HRESULT hr = CoCreateInstance(CLSID_DvdGraphBuilder, NULL, CLSCTX_INPROC, 
        IID_IDvdGraphBuilder, (LPVOID *)&m_pDvdGB) ;
    if (FAILED(hr) || NULL == m_pDvdGB)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("ERROR: DirectShow DVD software not installed properly (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }
    
    return TRUE;
}







HRESULT CDVDPlayer::BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, LPDIRECTDRAWSURFACE pDDPrimary)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::BuildGraph() entered"))) ;

    HRESULT     hr ;
    
    
    ReleaseInterfaces() ;
    SetColorKey(253) ;  
    
    
    WCHAR    achwFileName[MAX_PATH] ;
    LPCWSTR  lpszwFileName = NULL ;  
    if (lstrlen(GetFileName()) > 0)  
    {
#ifdef UNICODE
        lstrcpy(achwFileName, GetFileName()) ;
#else
        MultiByteToWideChar(CP_ACP, 0, GetFileName(), -1, achwFileName, MAX_PATH) ;
#endif 
        
        lpszwFileName = achwFileName ;
    }
    DbgLog((LOG_TRACE, 5, TEXT("DVD file <%s> will be played"), GetFileName())) ;
    
    
    IDDrawExclModeVideo  *pDDXMV ;
    hr = m_pDvdGB->GetDvdInterface(IID_IDDrawExclModeVideo, (LPVOID *)&pDDXMV) ;
    if (FAILED(hr) || NULL == pDDXMV)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("ERROR: IDvdGB::GetDvdInterface(IDDrawExclModeVideo) failed (Error 0x%lx)"), hr)) ;
        return hr ;
    }
    hr = pDDXMV->SetDDrawObject(pDDObj) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("ERROR: IDDrawExclModeVideo::SetDDrawObject() failed (Error 0x%lx)"), hr)) ;
        pDDXMV->Release() ;  
        return hr ;
    }
    hr = pDDXMV->SetDDrawSurface(pDDPrimary) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("ERROR: IDDrawExclModeVideo::SetDDrawSurface() failed (Error 0x%lx)"), hr)) ;
        pDDXMV->SetDDrawObject(NULL) ;  
        pDDXMV->Release() ;  
        return hr ;
    }
    pDDXMV->Release() ;  
    
    
    AM_DVD_RENDERSTATUS   Status ;
    TCHAR                 achBuffer[1000] ;
    hr = m_pDvdGB->RenderDvdVideoVolume(lpszwFileName,
        AM_DVD_HWDEC_PREFER, &Status) ;
    if (FAILED(hr))
    {
        AMGetErrorText(hr, achBuffer, sizeof(achBuffer)) ;
        MessageBox(hWndApp, achBuffer, "Error", MB_OK) ;
        return hr ;
    }
    if (S_FALSE == hr)  
    {
        std::basic_string<TCHAR>   l_Text;
        if (GetStatusText(&Status, achBuffer, sizeof(achBuffer)))
	    {
            l_Text.assign(achBuffer);
        }
        else
        {
            l_Text.assign(TEXT("An unknown error has occurred"));
        }
        l_Text.append(TEXT("\n\nDo you still want to continue?"));
        if (IDNO == MessageBox(hWndApp, l_Text.c_str(), TEXT("Warning"), MB_YESNO))
        {
            return E_FAIL;
        }
    }
    
    GetInterfaces(hWndApp) ;
    
    hr = GetColorKeyInternal() ;
    ASSERT(SUCCEEDED(hr)) ;

    return S_OK ;
}





HRESULT CDVDPlayer::GetInterfaces(HWND hWndApp)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::GetInterfaces() entered"))) ;

    HRESULT  hr ;
    hr = m_pDvdGB->GetFiltergraph(&m_pGraph) ;
    ASSERT(SUCCEEDED(hr) && m_pGraph) ;
    
    
    hr = m_pDvdGB->GetDvdInterface(IID_IDvdControl, (LPVOID *)&m_pDvdC) ;
    ASSERT(SUCCEEDED(hr) && m_pDvdC) ;
    
    hr = m_pDvdGB->GetDvdInterface(IID_IDvdInfo, (LPVOID *)&m_pDvdI) ;
    ASSERT(SUCCEEDED(hr) && m_pDvdI) ;
    
    return CBaseVideoPlayer::GetInterfaces(hWndApp) ;  
}






HRESULT CDVDPlayer::ClearGraph()
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::ClearGraph() entered"))) ;
    
    ReleaseInterfaces() ;  
    if (m_pDvdGB)
    {
        m_pDvdGB->Release() ;
        m_pDvdGB = NULL ;
    }
    
    return S_OK ;
    
}






HRESULT CDVDPlayer::GetColorKeyInternal(IBaseFilter *pOvM )
{
    ASSERT(NULL == pOvM) ; 
    
    
    IMixerPinConfig2  *pMPC ;
    DWORD              dwColorKey ;
    HRESULT  hr = m_pDvdGB->GetDvdInterface(IID_IMixerPinConfig2, (LPVOID *) &pMPC) ;
    if (SUCCEEDED(hr))
    {
        hr = pMPC->GetColorKey(NULL, &dwColorKey) ;
        ASSERT(SUCCEEDED(hr)) ;
        SetColorKey(dwColorKey) ;

        
        
        pMPC->SetAspectRatioMode(AM_ARMODE_STRETCHED);
        pMPC->Release() ;
    }
    else
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("WARNING: IDvdGraphBuilder::GetDvdInterface(IID_IMixerPinConfig2) failed (Error 0x%lx)"), hr)) ;
        return hr ;
    }
    return S_OK ;
}






HRESULT CDVDPlayer::GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::GetNativeVideoData() entered"))) ;
    
    IDDrawExclModeVideo  *pDDXMV ;
    HRESULT hr = m_pDvdGB->GetDvdInterface(IID_IDDrawExclModeVideo, (LPVOID *)&pDDXMV) ;
    if (FAILED(hr) || NULL == pDDXMV)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("ERROR: IDvdGB::GetDvdInterface(IDDrawExclModeVideo) failed (Error 0x%lx)"), hr)) ;
        return hr ;
    }
    hr = pDDXMV->GetNativeVideoProps(pdwWidth, pdwHeight, pdwARX, pdwARY) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_TRACE, 1, 
            TEXT("WARNING: IDDrawExclModeVideo::GetNativeVideoProps() failed (Error 0x%lx)"), hr)) ;
    }
    
    pDDXMV->Release() ;  
    
    return hr ;
}






HRESULT CDVDPlayer::SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::SetVideoPosition() entered"))) ;
    
    IDDrawExclModeVideo  *pDDXMV ;
    HRESULT hr = m_pDvdGB->GetDvdInterface(IID_IDDrawExclModeVideo, (LPVOID *)&pDDXMV) ;
    if (FAILED(hr) || NULL == pDDXMV)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("ERROR: IDvdGB::GetDvdInterface(IDDrawExclModeVideo) failed (Error 0x%lx)"), hr)) ;
        return hr ;
    }
    
    RECT  rectDest ;
    SetRect(&rectDest, dwLeft, dwTop, dwLeft + dwWidth, dwTop + dwHeight) ;
    
    hr = pDDXMV->SetDrawParameters(NULL, &rectDest) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_TRACE, 1, 
            TEXT("WARNING: IDDrawExclModeVideo::SetDrawParameters() failed (Error 0x%lx)"), hr)) ;
    }
    
    pDDXMV->Release() ;  
    
    return hr ;
}






HRESULT CDVDPlayer::SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::SetOverlayCallback() entered"))) ;

    HRESULT  hr ;
    IDDrawExclModeVideo  *pDDXMV ;
    hr = m_pDvdGB->GetDvdInterface(IID_IDDrawExclModeVideo, (LPVOID *)&pDDXMV) ;
    if (FAILED(hr) || NULL == pDDXMV)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("ERROR: IDvdGB::GetDvdInterface(IDDrawExclModeVideo) failed (Error 0x%lx)"), hr)) ;
        return hr ;
    }
    hr = pDDXMV->SetCallbackInterface(pCallback, 0) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("ERROR: IDDrawExclModeVideo::SetCallbackInterface() failed (Error 0x%lx)"), hr)) ;
    }
    pDDXMV->Release() ;  

    return hr ;
}





void CDVDPlayer::ReleaseInterfaces(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::ReleaseInterfaces() entered"))) ;
    
    if (m_pDvdC) {
        m_pDvdC->Release() ;
        m_pDvdC = NULL ;
    }
    if (m_pDvdI) {
        m_pDvdI->Release() ;
        m_pDvdI = NULL ;
    }
    if (m_pGraph) {
        m_pGraph->Release() ;
        m_pGraph = NULL ;
    }
    CBaseVideoPlayer::ReleaseInterfaces() ;  
}





DWORD CDVDPlayer::GetStatusText(AM_DVD_RENDERSTATUS *pStatus,
                                LPTSTR lpszStatusText,
                                DWORD dwMaxText)
{
    DbgLog((LOG_TRACE, 5, TEXT("CDVDPlayer::GetStatusText() entered"))) ;
    
    TCHAR    achBuffer[1000] ;
    
    if (IsBadWritePtr(lpszStatusText, sizeof(*lpszStatusText) * dwMaxText))
    {
        DbgLog((LOG_ERROR, 0, TEXT("GetStatusText(): bad text buffer param"))) ;
        return 0 ;
    }
    
    int    iChars ;
    LPTSTR lpszBuff = achBuffer ;
    ZeroMemory(achBuffer, sizeof(TCHAR) * 1000) ;
    if (pStatus->iNumStreamsFailed > 0)
    {
        iChars = wsprintf(lpszBuff, 
            TEXT("* %d out of %d DVD-Video streams failed to render properly\n"), 
            pStatus->iNumStreamsFailed, pStatus->iNumStreams) ;
        lpszBuff += iChars ;
        
        if (pStatus->dwFailedStreamsFlag & AM_DVD_STREAM_VIDEO)
        {
            iChars = wsprintf(lpszBuff, TEXT("    - video stream\n")) ;
            lpszBuff += iChars ;
        }
        if (pStatus->dwFailedStreamsFlag & AM_DVD_STREAM_AUDIO)
        {
            iChars = wsprintf(lpszBuff, TEXT("    - audio stream\n")) ;
            lpszBuff += iChars ;
        }
        if (pStatus->dwFailedStreamsFlag & AM_DVD_STREAM_SUBPIC)
        {
            iChars = wsprintf(lpszBuff, TEXT("    - subpicture stream\n")) ;
            lpszBuff += iChars ;
        }
    }
    
    if (FAILED(pStatus->hrVPEStatus))
    {
        lstrcat(lpszBuff, "* ") ;
        lpszBuff += lstrlen("* ") ;
        iChars = AMGetErrorText(pStatus->hrVPEStatus, lpszBuff, 200) ;
        lpszBuff += iChars ;
        lstrcat(lpszBuff, "\n") ;
        lpszBuff += lstrlen("\n") ;
    }
    
    if (pStatus->bDvdVolInvalid)
    {
        iChars = wsprintf(lpszBuff, TEXT("* Specified DVD-Video volume was invalid\n")) ;
        lpszBuff += iChars ;
    }
    else if (pStatus->bDvdVolUnknown)
    {
        iChars = wsprintf(lpszBuff, TEXT("* No valid DVD-Video volume could be located\n")) ;
        lpszBuff += iChars ;
    }
    
    if (pStatus->bNoLine21In)
    {
        iChars = wsprintf(lpszBuff, TEXT("* The video decoder doesn't produce closed caption data\n")) ;
        lpszBuff += iChars ;
    }
    if (pStatus->bNoLine21Out)
    {
        iChars = wsprintf(lpszBuff, TEXT("* Decoded closed caption data not rendered properly\n")) ;
        lpszBuff += iChars ;
    }
    
    DWORD dwLength = (lpszBuff - achBuffer) * sizeof(*lpszBuff) ;
    dwLength = min(dwLength, dwMaxText) ;
    lstrcpyn(lpszStatusText, achBuffer, dwLength) ;
    
    return dwLength ;
}






CFilePlayer::CFilePlayer(void)
:   CBaseVideoPlayer   ()
{
    DbgLog((LOG_TRACE, 3, TEXT("CFilePlayer c-tor entered"))) ;
    
    m_pDDXM = NULL ;
}





CFilePlayer::~CFilePlayer(void)
{
    DbgLog((LOG_TRACE, 3, TEXT("CFilePlayer d-tor entered"))) ;
    
    ReleaseInterfaces() ;
    
    DbgLog((LOG_TRACE, 3, TEXT("CFilePlayer d-tor exiting..."))) ;
}





void CFilePlayer::ReleaseInterfaces(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::ReleaseInterfaces() entered"))) ;
    
    if (m_pDDXM) {
        m_pDDXM->Release() ;
        m_pDDXM = NULL ;
    }
    CBaseVideoPlayer::ReleaseInterfaces() ;
}





BOOL CFilePlayer::Initialize(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::Initialize() entered"))) ;
    
    
    
    
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, 
        IID_IGraphBuilder, (LPVOID *)&m_pGraph) ;
    if (FAILED(hr) || NULL == m_pGraph)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("ERROR: DirectShow is not installed properly (Error 0x%lx)"), hr)) ;
        return FALSE ;
    }
    
    return TRUE;
}






BOOL CFilePlayer::IsOvMConnected(IBaseFilter *pOvM)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::IsOvMConnected() entered"))) ;
    
    IEnumPins   *pEnumPins ;
    IPin        *pPin ;
    IPin        *pPin2 ;
    ULONG        ul ;
    HRESULT      hr ;
    BOOL         bConnected = FALSE ;

    pOvM->EnumPins(&pEnumPins) ;
    while (S_OK == pEnumPins->Next(1, &pPin, &ul) && 1 == ul)
    {
        hr = pPin->ConnectedTo(&pPin2) ;
        if (SUCCEEDED(hr) && pPin2)
        {
            DbgLog((LOG_TRACE, 3, TEXT("Found pin %s connected to pin %s"), 
                    (LPCTSTR)CDisp(pPin), (LPCTSTR)CDisp(pPin2))) ;
            bConnected = TRUE ;
            pPin2->Release() ;
        }
        pPin->Release() ;
    }
    pEnumPins->Release() ;
    
    return bConnected ;
}







HRESULT CFilePlayer::GetVideoRendererInterface(IBaseFilter **ppVR)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::IsVideoStreamPresent() entered"))) ;
    
    
    return m_pGraph->FindFilterByName(L"Video Renderer", ppVR) ;
}








HRESULT CFilePlayer::AddOvMToGraph(IBaseFilter **ppOvM, LPDIRECTDRAW pDDObj, 
                                   LPDIRECTDRAWSURFACE pDDPrimary)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::AddOvMToGraph() entered"))) ;
    
    HRESULT  hr ;
    
    hr = CoCreateInstance(CLSID_OverlayMixer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID *)ppOvM) ;
    if (FAILED(hr) || NULL == *ppOvM)
    {
        DbgLog((LOG_ERROR, 0, TEXT("Can't instantiate Overlay Mixer (Error 0x%lx)"), hr)) ;
        return E_FAIL ;
    }
    hr = m_pGraph->AddFilter(*ppOvM, L"Overlay Mixer") ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("Can't add Overlay Mixer to the graph (Error 0x%lx)"), hr)) ;
        (*ppOvM)->Release() ;
        *ppOvM = NULL ;
        return E_FAIL ;
    }
    
    
    hr = SetDDrawParams(*ppOvM, pDDObj, pDDPrimary) ;
    if (FAILED(hr))
    {
        m_pGraph->RemoveFilter(*ppOvM) ;  
        (*ppOvM)->Release() ;
        *ppOvM = NULL ;
        return hr ;
    }
    
    return S_OK ;
}






HRESULT CFilePlayer::SetDDrawParams(IBaseFilter *pOvM, LPDIRECTDRAW pDDObj, 
                                    LPDIRECTDRAWSURFACE pDDPrimary)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::SetDDrawParams() entered"))) ;
    
    HRESULT                 hr ;
    IDDrawExclModeVideo    *pDDXM ;
    
    hr = pOvM->QueryInterface(IID_IDDrawExclModeVideo, (LPVOID *)&pDDXM) ;
    if (FAILED(hr) || NULL == pDDXM)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("Can't get IDDrawExclusiveMode interface from Overlay Mixer (Error 0x%lx)"), hr)) ;
        return E_FAIL ;
    }
    hr = pDDXM->SetDDrawObject(pDDObj) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("SetDDrawObject(0x%lx) failed (Error 0x%lx)"), pDDObj, hr)) ;
        pDDXM->Release() ;               
        return E_FAIL ;
    }
    hr = pDDXM->SetDDrawSurface(pDDPrimary) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("SetDDrawSurface(0x%lx) failed (Error 0x%lx)"), pDDPrimary, hr)) ;
        pDDXM->SetDDrawObject(NULL) ;    
        pDDXM->Release() ;               
        return E_FAIL ;
    }
    
    pDDXM->Release() ;               
    
    return S_OK ;
}






HRESULT CFilePlayer::PutVideoThroughOvM(IBaseFilter *pOvM, IBaseFilter *pVR)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::PutVideoThroughOvM() entered"))) ;
    
    IEnumPins     *pEnumPins ;
    IPin          *pPinVR ;
    IPin          *pPinConnectedTo ;
    IPin          *pPinOvM ;
    ULONG          ul ;
    PIN_DIRECTION  pd ;
    HRESULT        hr ;
    
    
    hr = pVR->EnumPins(&pEnumPins) ;
    ASSERT(SUCCEEDED(hr)) ;
    hr = pEnumPins->Next(1, &pPinVR, &ul) ;
    ASSERT(S_OK == hr  &&  1 == ul)  ;
    pPinVR->QueryDirection(&pd) ;
    ASSERT(PINDIR_INPUT == pd) ;
    pEnumPins->Release() ;
    
    
    hr = pPinVR->ConnectedTo(&pPinConnectedTo) ;
    ASSERT(SUCCEEDED(hr) && pPinConnectedTo) ;
    hr = m_pGraph->Disconnect(pPinVR) ;
    ASSERT(SUCCEEDED(hr)) ;
    hr = m_pGraph->Disconnect(pPinConnectedTo) ;
    ASSERT(SUCCEEDED(hr)) ;
    
    
    
    hr = pOvM->EnumPins(&pEnumPins) ;
    ASSERT(SUCCEEDED(hr)) ;
    int  iInConns  = 0 ;
    while (iInConns == 0  &&   
        S_OK == pEnumPins->Next(1, &pPinOvM, &ul) && 1 == ul)  
    {
        pPinOvM->QueryDirection(&pd) ;
        if (PINDIR_INPUT == pd)  
        {
            if (0 == iInConns) 
            {
                hr = m_pGraph->Connect(pPinConnectedTo, pPinOvM) ; 
                ASSERT(SUCCEEDED(hr)) ;
                if (FAILED(hr))
                    DbgLog((LOG_ERROR, 0, TEXT("ERROR: m_pGraph->Connect() failed (Error 0x%lx)"), hr)) ;
                else
                {
                    DbgLog((LOG_TRACE, 5, TEXT("Pin %s connected to pin %s"), 
                        (LPCTSTR) CDisp(pPinConnectedTo), (LPCTSTR) CDisp(pPinOvM))) ;
                    iInConns++ ;
                }
            }
            
        }
        else
            DbgLog((LOG_ERROR, 1, TEXT("OVMixer still has an out pin!!!"))) ;
        
        pPinOvM->Release() ;
    }
    pEnumPins->Release() ;
    
    pPinConnectedTo->Release() ;  
    pPinVR->Release() ; 
    m_pGraph->RemoveFilter(pVR) ;  

    
    if (0 == iInConns)  
    {
        DbgLog((LOG_ERROR, 0, TEXT("WARNING: Couldn't connect any out pin to OvMixer's 1st in pin"))) ;
        return E_FAIL ;    
    }
    
    return S_OK ;          
}






HRESULT CFilePlayer::BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, LPDIRECTDRAWSURFACE pDDPrimary)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::BuildGraph() entered"))) ;
    
    HRESULT       hr ;
    IBaseFilter  *pOvM ;
    WCHAR         achwFileName[MAX_PATH] ;
    LPWSTR        lpszwFileName = NULL ;
    IBaseFilter  *pVR ;
    
    
    ReleaseInterfaces() ;
    SetColorKey(253) ;  
    
    
    if (lstrlen(GetFileName()) > 0)  
    {
#ifdef UNICODE
        lstrcpy(achwFileName, GetFileName()) ;
#else
        MultiByteToWideChar(CP_ACP, 0, GetFileName(), -1, achwFileName, MAX_PATH) ;
#endif 
        
        lpszwFileName = achwFileName ;
    }
    else  
        return E_FAIL ;
    
    
    
    
    hr = AddOvMToGraph(&pOvM, pDDObj, pDDPrimary) ;
    if (FAILED(hr))
    {
        return E_FAIL ;
    }
    
    
    
    
    hr = m_pGraph->RenderFile(lpszwFileName, NULL) ;
    if (S_OK != hr)
    {
        DbgLog((LOG_ERROR, 0, 
            TEXT("Rendering the given file didn't succeed completely (Error 0x%lx)"), hr)) ;
        m_pGraph->RemoveFilter(pOvM) ;  
        pOvM->Release() ;               
        return E_FAIL ;
    }

    
    
    
    
    
    if (! IsOvMConnected(pOvM) )
    {
        DbgLog((LOG_TRACE, 1, TEXT("OverlayMixer is not used in the graph. Try again..."))) ;
    
        
        
        
        
        if (FAILED(GetVideoRendererInterface(&pVR)))
        {
            DbgLog((LOG_TRACE, 1, TEXT("Specified file doesn't have any video stream. Aborting graph building."))) ;
            m_pGraph->RemoveFilter(pOvM) ;  
            pOvM->Release() ;               
            return E_FAIL ;
        }

        
        
        
        hr = PutVideoThroughOvM(pOvM, pVR) ;
        if (FAILED(hr))
        {
            DbgLog((LOG_TRACE, 1, TEXT("Couldn't put video through the OverlayMixer."))) ;
            m_pGraph->RemoveFilter(pOvM) ;  
            pOvM->Release() ;               
            pVR->Release() ;                
            return E_FAIL ;
        }
        pVR->Release() ;    
    }

    
    
    
    GetInterfaces(hWndApp) ;
    
    
    hr = pOvM->QueryInterface(IID_IDDrawExclModeVideo, (LPVOID *)&m_pDDXM) ;
    ASSERT(SUCCEEDED(hr)) ;
    
    
    hr = GetColorKeyInternal(pOvM) ;
    ASSERT(SUCCEEDED(hr)) ;
    
    pOvM->Release() ;  
    
    return S_OK ;
}







HRESULT CFilePlayer::GetInterfaces(HWND hWndApp)
{
    return CBaseVideoPlayer::GetInterfaces(hWndApp) ;  
}





HRESULT CFilePlayer::GetColorKeyInternal(IBaseFilter *pOvM)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::GetColorKeyInternal() entered"))) ;
    
    if (NULL == pOvM)
        return E_INVALIDARG ;
    
    IEnumPins  *pEnumPins ;
    IPin       *pPin ;
    ULONG       ul ;
    PIN_DIRECTION  pd ;
    DWORD       dwColorKey ;
    IMixerPinConfig  *pMPC ;
    HRESULT  hr = pOvM->EnumPins(&pEnumPins) ;
    ASSERT(pEnumPins) ;
    while (S_OK == pEnumPins->Next(1, &pPin, &ul)  &&  1 == ul)  
    {
        pPin->QueryDirection(&pd) ;
        if (PINDIR_INPUT == pd)  
        {
            hr = pPin->QueryInterface(IID_IMixerPinConfig, (LPVOID *) &pMPC) ;
            ASSERT(SUCCEEDED(hr) && pMPC) ;
            hr = pMPC->GetColorKey(NULL, &dwColorKey) ;  
            SetColorKey(dwColorKey) ;
            
            
            pMPC->SetAspectRatioMode(AM_ARMODE_STRETCHED);
            ASSERT(SUCCEEDED(hr)) ;
            pMPC->Release() ;
            pPin->Release() ; 
            break ;   
        }
        pPin->Release() ;
    }
    pEnumPins->Release() ;  
    
    return S_OK ;
}






HRESULT CFilePlayer::ClearGraph(void)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::ClearGraph() entered"))) ;
    
    ReleaseInterfaces() ;  
    if (m_pGraph)
    {
        m_pGraph->Release() ;  
        m_pGraph = NULL ;
    }
    
    return S_OK ;
}





HRESULT CFilePlayer::SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::SetVideoPosition() entered"))) ;
    
    RECT  rectDest ;
    SetRect(&rectDest, dwLeft, dwTop, dwLeft + dwWidth, dwTop + dwHeight) ;
    
    
    return m_pDDXM->SetDrawParameters(NULL, &rectDest) ;
}





HRESULT CFilePlayer::GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::GetNativeVideoData() entered"))) ;
    
    return m_pDDXM->GetNativeVideoProps(pdwWidth, pdwHeight, pdwARX, pdwARY) ;
}






HRESULT CFilePlayer::SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback)
{
    DbgLog((LOG_TRACE, 5, TEXT("CFilePlayer::SetOverlayCallback() entered"))) ;

    HRESULT  hr ;
    hr = m_pDDXM->SetCallbackInterface(pCallback, 0) ;
    if (FAILED(hr))
    {
        DbgLog((LOG_ERROR, 0, TEXT("ERROR: IDDrawExclModeVideo::SetCallbackInterface() failed (Error 0x%lx)"), hr)) ;
    }

    return hr ;
}


