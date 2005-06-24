#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdlsurface_h__
#define __aui_sdl__aui_sdlsurface_h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_surface.h"
#include "aui_sdl.h"
#include <SDL.h>

class aui_SDLSurface : public aui_Surface, public aui_SDL {
public:
	aui_SDLSurface(
	               AUI_ERRCODE *retval,
	               sint32 width,
	               sint32 height,
	               sint32 bpp,
	               SDL_Surface* lpdds = NULL,
	               BOOL isPrimary = FALSE,
	               BOOL useVideoMemory = FALSE,
	               BOOL takeOwnership = FALSE );
        virtual ~aui_SDLSurface();

protected:
        aui_SDLSurface() : aui_Surface() {}
        AUI_ERRCODE InitCommon( void );

public:
	virtual BOOL IsThisA( uint32 classId )
	{
		return classId == m_SDLSurfaceClassId
		                  || aui_Surface::IsThisA( classId )
		                  || aui_SDL::IsThisA( classId );
        }

	virtual uint32 SetChromaKey( uint32 color );


	virtual AUI_ERRCODE Lock( RECT *rect, LPVOID *buffer, DWORD flags );
	virtual AUI_ERRCODE Unlock( LPVOID buffer );

	SDL_Surface*    DDS( void ) const { return m_lpdds; }
	BOOL                            IsDCGot( void ) const { return m_dcIsGot
; }


        virtual BOOL IsOK( void ) const;

	static uint32 m_SDLSurfaceClassId;

	SDL_mutex* m_bltMutex;

protected:
	SDL_Surface* m_lpdds;
};


#endif // defined(__AUI_USE_SDL__)

#endif
