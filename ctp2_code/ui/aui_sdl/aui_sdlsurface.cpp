#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_sdlsurface.h"
#include <SDL.h>
#include <SDL_thread.h>

uint32 aui_SDLSurface::m_SDLSurfaceClassId = aui_UniqueId();

aui_SDLSurface::aui_SDLSurface(
	AUI_ERRCODE *retval,
	sint32 width,
	sint32 height,
	sint32 bpp,
	SDL_Surface* lpdds,
	BOOL isPrimary,
	BOOL useVideoMemory,
	BOOL takeOwnership )
	:
	aui_Surface()
{
	m_bltMutex = SDL_CreateMutex();
	if (lpdds != 0 && takeOwnership) {
		width = lpdds->w;
		height = lpdds->h;
		bpp = lpdds->format->BitsPerPixel;
	}
	*retval = aui_Surface::InitCommon( width, height, bpp, isPrimary );
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

        SDL_PixelFormat* fmt = SDL_GetVideoSurface()->format;
	if ( !(m_lpdds = lpdds) )
	{
		m_lpdds = SDL_CreateRGBSurface(0, width, height, fmt->BitsPerPixel, fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);
		if ( m_lpdds == NULL )
		{
			*retval = AUI_ERRCODE_MEMALLOCFAILED;
			return;
		}
		m_allocated = TRUE;
		// clear it with black
		if (SDL_FillRect(m_lpdds, NULL, 0) < 0) {
			fprintf(stderr, "aui_Surface: Failed to erase new surface: %s\n",
			SDL_GetError());
		}
	}
	else
            {
	    m_allocated = takeOwnership;
            }

	//just checking (fmt->Gmask >> fmt->Gshift == 0x3F) should be enough
        //but save is save...
	if ((fmt->Rmask >> fmt->Rshift == 0x1F) && (fmt->Gmask >> fmt->Gshift == 0x3F) && (fmt->Bmask >> fmt->Bshift == 0x1F)) {
            m_pixelFormat = AUI_SURFACE_PIXELFORMAT_565;
            //printf("%s L%d: AUI_SURFACE_PIXELFORMAT_565\n", __FILE__, __LINE__);
            }
        if ((fmt->Rmask >> fmt->Rshift == 0x1F) && (fmt->Gmask >> fmt->Gshift == 0x1F) && (fmt->Bmask >> fmt->Bshift == 0x1F)) {
            m_pixelFormat = AUI_SURFACE_PIXELFORMAT_555;
            //printf("%s L%d: AUI_SURFACE_PIXELFORMAT_555\n", __FILE__, __LINE__);
            }

	m_pitch = m_lpdds->pitch;
	m_size = m_pitch * m_height;
}


AUI_ERRCODE aui_SDLSurface::InitCommon( void )
{
	m_lpdds = NULL;

	return AUI_ERRCODE_OK;
}


aui_SDLSurface::~aui_SDLSurface()
{
	if ( m_allocated && m_lpdds )
	{
		SDL_FreeSurface(m_lpdds);
		m_lpdds = NULL;
		m_allocated = FALSE;
	}
	SDL_DestroyMutex(m_bltMutex);
}


uint32 aui_SDLSurface::SetChromaKey( uint32 color ) {
    int hr = SDL_SetColorKey(m_lpdds, SDL_SRCCOLORKEY, color); //|SDL_RLEACCEL ?
    //hr == 0 if succeded!
    //printf("%s L%d: SDL_SRCCOLORKEY set to %#X\n", __FILE__, __LINE__, color);

    if ( hr == 0 )
        return aui_Surface::SetChromaKey( color ); //sets aui_Surface.m_chromaKey and returns last value!

    //return AUI_ERRCODE_OK;  //this is not sensible, should retrun last color key!?!
    printf("%s L%d: SDL_SRCCOLORKEY setting failed!\n", __FILE__, __LINE__);
    return (uint32)-1; //better?
    }


BOOL aui_SDLSurface::IsOK( void ) const
{
	return TRUE;
}




AUI_ERRCODE aui_SDLSurface::Lock( RECT *rect, LPVOID *buffer, DWORD flags ){

    AUI_ERRCODE errcode = AUI_ERRCODE_OK;

    // must lock the mutex first!
    SDL_LockMutex(m_bltMutex);
    //printf("%s L%d: Locking mutex!\n", __FILE__, __LINE__);
    if (SDL_MUSTLOCK(m_lpdds)) {
        printf("%s L%d: Locking surface! Check this!\n", __FILE__, __LINE__);
        if (SDL_LockSurface(m_lpdds) < 0) {
            fprintf(stderr, "Cannot lock surface: %s\n", SDL_GetError());
            return AUI_ERRCODE_SURFACELOCKFAILED;
            }
        }
    // return a buffer pointer that points to the rectangle
    *buffer = m_lpdds->pixels;
    m_saveBuffer = static_cast<uint8*>(*buffer);
    if (rect != NULL)
	{
        *buffer = static_cast<char*>(*buffer) +
            rect->top * m_lpdds->pitch +
            rect->left * m_Bpp;
	}
    return ManipulateLockList( rect, buffer, AUI_SURFACE_LOCKOP_ADD );
    }




AUI_ERRCODE aui_SDLSurface::Unlock( LPVOID buffer )
{
	AUI_ERRCODE errcode =
	ManipulateLockList( NULL, &buffer, AUI_SURFACE_LOCKOP_REMOVE );

	if ( errcode == AUI_ERRCODE_OK )
	{
		if (SDL_MUSTLOCK(m_lpdds)) {
			SDL_UnlockSurface(m_lpdds);
		}
	}
	m_saveBuffer = NULL;
	SDL_UnlockMutex(m_bltMutex);

	return errcode;
}

#endif
