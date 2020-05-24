//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//
//----------------------------------------------------------------------------
#ifndef __AUI_MOVIE_H__
#define __AUI_MOVIE_H__

class aui_Movie;
class aui_MovieFormat;
class aui_AviMovieFormat;

#include "aui_base.h"
#include "aui_memmap.h" // aui_FileFormat

class aui_Surface;

#define k_AUI_MOVIE_PLAYFLAG_LOOP			0x00000001
#define k_AUI_MOVIE_PLAYFLAG_ONSCREEN		0x00000002
#define k_AUI_MOVIE_PLAYFLAG_PLAYANDHOLD	0x00000004


class aui_MovieFormat : public aui_FileFormat
{
public:

	aui_MovieFormat() {}
	virtual ~aui_MovieFormat() {}

	virtual AUI_ERRCODE	Load( const MBCHAR *filename, aui_Movie *movie )
		{ return AUI_ERRCODE_OK; }
};


class aui_Movie : public aui_Base
{
public:
	aui_Movie( void );
	aui_Movie(
		AUI_ERRCODE *retval,
		const MBCHAR * filename = NULL );
	virtual ~aui_Movie();

protected:
	void InitCommon( void );
	AUI_ERRCODE InitCommon( const MBCHAR * filename );

public:

	virtual AUI_ERRCODE Load( void );
	virtual AUI_ERRCODE Unload( void );

	AUI_ERRCODE   SetFilename( const MBCHAR * filename );
	const MBCHAR *GetFilename( void ) const { return m_filename; }

	aui_Surface	*SetDestSurface( aui_Surface *surface );
	aui_Surface	*GetDestSurface( void ) const { return m_surface; }

	void SetDestRect( RECT *rect );
	void SetDestRect(
		sint32 x,
		sint32 y,
		sint32 w,
		sint32 h );
	RECT *GetDestRect( void ) { return &m_rect; }
	void GetDestRect(
		sint32 *x,
		sint32 *y,
		sint32 *w,
		sint32 *h ) const;

	uint32 SetTimePerFrame( uint32 timePerFrame );
	uint32 GetTimePerFrame( void ) const { return m_timePerFrame; }

	virtual AUI_ERRCODE Open(
		uint32 flags = 0,
		aui_Surface *surface = NULL,
		RECT *rect = NULL );
	virtual AUI_ERRCODE Close( void );

	virtual AUI_ERRCODE Play( void );
	virtual AUI_ERRCODE Stop( void );

	virtual AUI_ERRCODE Pause( void );
	virtual AUI_ERRCODE Resume( void );

	virtual AUI_ERRCODE Process( void );

	virtual BOOL IsOpen( void ) const { return m_isOpen; }
	virtual BOOL IsPlaying( void ) const { return m_isPlaying; }
	virtual BOOL IsFinished( void ) const { return m_isFinished; }
	virtual BOOL IsPaused( void ) const { return m_isPaused; }

	static WNDPROC		m_windowProc;
	static aui_Movie	*m_onScreenMovie;

	uint32 GetFlags(void) { return m_flags; }

	aui_Surface *GetWindowSurface(void) { return m_windowSurface; }
	void SetWindowSurface(aui_Surface *surf) { m_windowSurface = surf; }
	RECT *GetWindowRect(void) { return &m_windowRect; }
	void SetWindowRect(RECT *rect) { m_windowRect = *rect; }

protected:

	AUI_ERRCODE PlayOnScreenMovie( void );

	MBCHAR m_filename[ MAX_PATH + 1 ];
	aui_MovieFormat *m_format;

	aui_Surface	*m_surface;
	RECT		m_rect;

	aui_Surface *m_windowSurface;
	RECT		m_windowRect;

	BOOL		m_isOpen;
	BOOL		m_isPlaying;
	BOOL		m_isFinished;
	BOOL		m_isPaused;

	uint32		m_flags;

	uint32		m_timePerFrame;
	uint32		m_lastFrameTime;

#if defined(__AUI_USE_DIRECTX__)
	PAVIFILE			m_aviFile;
	PAVISTREAM			m_aviStream;
	AVIFILEINFO			m_aviFileInfo;
	AVISTREAMINFO		m_aviStreamInfo;
	PGETFRAME			m_getFrame;
#endif
	uint32				m_curFrame;
};

class aui_AviMovieFormat : public aui_MovieFormat
{
public:

	aui_AviMovieFormat() {}
	virtual ~aui_AviMovieFormat() {}

	virtual AUI_ERRCODE	Load( const MBCHAR * filename, aui_Movie *movie )
	{ return AUI_ERRCODE_OK; }
};

LRESULT CALLBACK OnScreenMovieWindowProc(
	HWND hwnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam );

#endif
