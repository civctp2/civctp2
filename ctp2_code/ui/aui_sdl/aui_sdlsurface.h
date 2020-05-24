#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __aui_sdl__aui_sdlsurface_h__
#define __aui_sdl__aui_sdlsurface_h__ 1

#include "ctp2_config.h"

#if defined(__AUI_USE_SDL__)

#include "aui_surface.h"
#include "aui_sdl.h"

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
    static uint32 TransformSurfacePixelFormatToSDL(const AUI_SURFACE_PIXELFORMAT pixelFormat);

	virtual BOOL IsThisA( uint32 classId ) {
            return ((classId == m_SDLSurfaceClassId)
                    || aui_Surface::IsThisA( classId )
                    || aui_SDL::IsThisA( classId ));
            }

	virtual uint32 SetChromaKey( uint32 color );

	virtual AUI_ERRCODE Lock( RECT *rect, LPVOID *buffer, DWORD flags );
	virtual AUI_ERRCODE Unlock( LPVOID buffer );

	SDL_Surface*    DDS( void ) const { return m_lpdds; }
	BOOL                            IsDCGot( void ) const { return m_dcIsGot
; }

        virtual BOOL IsOK( void ) const;
	virtual AUI_ERRCODE Blank(const uint32 &color);

	virtual void Flip();

	static uint32 m_SDLSurfaceClassId;

	SDL_mutex* m_bltMutex;

	SDL_Surface* GetSDLSurface() { return m_lpdds; }
protected:
	SDL_Surface* m_lpdds;

private:
    SDL_Surface* CreateSDLSurface(const int width, const int height, const int bpp);
};

typedef aui_SDLSurface aui_NativeSurface;

#endif // defined(__AUI_USE_SDL__)

#endif
