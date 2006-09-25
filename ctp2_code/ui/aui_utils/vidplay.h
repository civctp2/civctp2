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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Multiple include guard (VIDPLAY_H) added.
// - Changed string handling to avoid using deprecated functions.
//
//----------------------------------------------------------------------------
//------------------------------------------------------------------------------
// File: VidPlay.h
//
// Desc: DirectShow sample code - video (DVD and file) playback 
//       class header file.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef VIDPLAY_H
#define VIDPLAY_H

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

#include <ddraw.h>          // LPDIRECTDRAW
#include <string>           // std::basic_string
#include <windows.h>        // Lots of definitions

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class CBaseVideoPlayer;
class CDVDPlayer;
class CFilePlayer;

// Player state
typedef enum {
    Uninitialized = 0, Stopped, Paused, Playing, Scanning
} PLAYER_STATE ;

// Define a special WM message for playback related events from DirectShow filtergraph
#define WM_PLAY_EVENT     WM_USER + 100
#define WM_SIZE_CHANGE    WM_USER + 101


//
// Video Playback base class
//
class CBaseVideoPlayer
{
public:   // public methods for Windows structure to call
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
    
protected:  // semi-internal state info (to be shared with derived classes)
    IGraphBuilder  *m_pGraph ;  // IGraphBuilder interface      
    
private:    // internal state info
    PLAYER_STATE    m_eState ;    // player state (run/pause/stop/...)
    std::basic_string<TCHAR>	m_achFileName;  // current file name
    IMediaControl  *m_pMC ;           // IMediaControl interface
    IMediaEventEx  *m_pME ;           // IMediaEventEx interface
    
    DWORD           m_dwColorKey ;    // color key to be used for video
} ;


//
// DVD Playback class
//
class CDVDPlayer : public CBaseVideoPlayer
{
public:   // public methods for Windows structure to call
    CDVDPlayer(void) ;
    ~CDVDPlayer(void) ;
    
    BOOL    Initialize(void) ;
    
    HRESULT BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT ClearGraph(void) ;
    HRESULT GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY) ;
    HRESULT SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight) ;
    HRESULT GetInterfaces(HWND hWndApp) ;
    HRESULT SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback) ;
    
private:  // private helper methods for the class' own use
    void    ReleaseInterfaces(void) ;
    HRESULT GetColorKeyInternal(IBaseFilter *pOvM = NULL) ;
    DWORD   GetStatusText(AM_DVD_RENDERSTATUS *pStatus, 
        LPTSTR lpszStatusText,
        DWORD dwMaxText) ;
    
private:  // internal state info
    IDvdGraphBuilder  *m_pDvdGB ;         // IDvdGraphBuilder interface
    IDvdInfo          *m_pDvdI ;          // IDvdInfo interface
    IDvdControl       *m_pDvdC ;          // IDvdControl interface
} ;


//
// File Playback class
//
class CFilePlayer : public CBaseVideoPlayer
{
public:   // public methods for Windows structure to call
    CFilePlayer(void) ;
    ~CFilePlayer(void) ;
    
    BOOL    Initialize(void) ;
    
    HRESULT BuildGraph(HWND hWndApp, LPDIRECTDRAW pDDObj, LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT ClearGraph(void) ;
    HRESULT GetNativeVideoData(DWORD *pdwWidth, DWORD *pdwHeight, DWORD *pdwARX, DWORD *pdwARY) ;
    HRESULT SetVideoPosition(DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight) ;
    HRESULT GetInterfaces(HWND hWndApp) ;
    HRESULT SetOverlayCallback(IDDrawExclModeVideoCallback *pCallback) ;
    
private:  // private helper methods for the class' own use
    void    ReleaseInterfaces() ;
    HRESULT GetColorKeyInternal(IBaseFilter *pOvM = NULL) ;
    BOOL    IsOvMConnected(IBaseFilter *pOvM) ;
    HRESULT GetVideoRendererInterface(IBaseFilter **ppVR) ;
    HRESULT AddOvMToGraph(IBaseFilter **ppOvM, LPDIRECTDRAW pDDObj, 
        LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT SetDDrawParams(IBaseFilter *pOvM, LPDIRECTDRAW pDDObj, 
        LPDIRECTDRAWSURFACE pDDPrimary) ;
    HRESULT PutVideoThroughOvM(IBaseFilter *pOvM, IBaseFilter *pVR) ;
    
private:  // internal state info
    IDDrawExclModeVideo *m_pDDXM ;       // IDDrawExclModeVideo interface
} ;

#endif
