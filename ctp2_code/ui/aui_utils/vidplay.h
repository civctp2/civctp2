//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// _MSC_VER		
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Multiple include guard (VIDPLAY_H) added.
// - Changed string handling to avoid using deprecated functions.
//
//----------------------------------------------------------------------------

#ifndef VIDPLAY_H
#define VIDPLAY_H

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

#include <string>           // std::basic_string

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class	CBaseVideoPlayer;
class CDVDPlayer;
class CFilePlayer;

typedef enum {
    Uninitialized = 0, Stopped, Paused, Playing, Scanning
} PLAYER_STATE ;


#define WM_PLAY_EVENT     WM_USER + 100
#define WM_SIZE_CHANGE    WM_USER + 101





class CBaseVideoPlayer
{
public:   
    CBaseVideoPlayer(void) ;
    ~CBaseVideoPlayer(void) ;
    
    virtual BOOL    Initialize(void) = 0 ;
    virtual HRESULT BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, 
        LPDIRECTDRAWSURFACE pDDPrimary) = 0 ;
    virtual HRESULT ClearGraph(void) = 0 ;
    virtual HRESULT GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY) = 0 ;
    virtual HRESULT SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight) = 0 ;
    virtual HRESULT GetInterfaces(HWND hWndApp) ;
    virtual HRESULT SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback) = 0 ;
    HRESULT GetColorKey(DWORD *pdwColorKey) ;
    BOOL    Play(void) ;
    BOOL    Pause(void) ;
    BOOL    Stop(void) ;
    void    SetFileName(LPCTSTR lpszFileName)
    {
        m_achFileName.assign(lpszFileName);
    };
    inline  BOOL    IsGraphReady(void)                  { return (Uninitialized != m_eState) ; } ;
    inline  PLAYER_STATE GetState(void)                 { return m_eState ; } ;
    inline  void    SetColorKey(DWORD dwColorKey)       { m_dwColorKey = dwColorKey ; } ;
    LPCTSTR GetFileName(void) const	
    {
        return m_achFileName.c_str();
    };
    
protected:
    virtual void    ReleaseInterfaces(void) ;
    virtual HRESULT GetColorKeyInternal(IBaseFilter *pOvM = NULL) = 0 ;
    
private:
    void    WaitForState(FILTER_STATE State) ;
    
protected:  
    IGraphBuilder  *m_pGraph ;        
    
private:    
    PLAYER_STATE    m_eState ;    
    std::basic_string<TCHAR>	m_achFileName;
    IMediaControl  *m_pMC ;           
    IMediaEventEx  *m_pME ;           
    
    DWORD           m_dwColorKey ;    
} ;





class CDVDPlayer : public CBaseVideoPlayer
{
public:   
    CDVDPlayer(void) ;
    ~CDVDPlayer(void) ;
    
    BOOL    Initialize(void) ;
    
    HRESULT BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT ClearGraph(void) ;
    HRESULT GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY) ;
    HRESULT SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight) ;
    HRESULT GetInterfaces(HWND hWndApp) ;
    HRESULT SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback) ;
    
private:  
    void    ReleaseInterfaces(void) ;
    HRESULT GetColorKeyInternal(IBaseFilter *pOvM = NULL) ;
    DWORD   GetStatusText(AM_DVD_RENDERSTATUS *pStatus, 
        LPTSTR lpszStatusText,
        DWORD dwMaxText) ;
    
private:  
    IDvdGraphBuilder  *m_pDvdGB ;         
    IDvdInfo          *m_pDvdI ;          
    IDvdControl       *m_pDvdC ;          
} ;





class CFilePlayer : public CBaseVideoPlayer
{
public:   
    CFilePlayer(void) ;
    ~CFilePlayer(void) ;
    
    BOOL    Initialize(void) ;
    
    HRESULT BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT ClearGraph(void) ;
    HRESULT GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY) ;
    HRESULT SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight) ;
    HRESULT GetInterfaces(HWND hWndApp) ;
    HRESULT SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback) ;
    
private:  
    void    ReleaseInterfaces() ;
    HRESULT GetColorKeyInternal(IBaseFilter *pOvM = NULL) ;
    BOOL    IsOvMConnected(IBaseFilter *pOvM) ;
    HRESULT GetVideoRendererInterface(IBaseFilter **ppVR) ;
    HRESULT AddOvMToGraph(IBaseFilter **ppOvM, LPDIRECTDRAW pDDObj, 
        LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT SetDDrawParams(IBaseFilter *pOvM, LPDIRECTDRAW pDDObj, 
        LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT PutVideoThroughOvM(IBaseFilter *pOvM, IBaseFilter *pVR) ;
    
private:  
    IDDrawExclModeVideo *m_pDDXM ;       
} ;

#endif
