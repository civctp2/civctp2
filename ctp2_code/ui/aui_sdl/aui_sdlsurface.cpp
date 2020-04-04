#include "ctp2_config.h"
#include "c3.h"

#ifdef __AUI_USE_SDL__

#include "aui_ui.h"
#include "aui_uniqueid.h"
#include "aui_sdlsurface.h"
#include <SDL2/SDL_thread.h>

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

	if ( !(m_lpdds = lpdds) )
	{
		m_lpdds = CreateSDLSurface(width, height, bpp);
		if ( m_lpdds == NULL )
		{
			*retval = AUI_ERRCODE_MEMALLOCFAILED;
			return;
		}
		m_allocated = TRUE;
		// clear it with black
		if (SDL_FillRect(m_lpdds, NULL, 0) < 0) {
			fprintf(stderr, "aui_Surface: Failed to erase new surface: %s\n", SDL_GetError());
		}
	}
	else {
	    m_allocated = takeOwnership;
	}

	m_buffer = (uint8*) m_lpdds->pixels;
	m_pitch = m_lpdds->pitch;
	m_size = m_pitch * m_height;

	//SetChromaKey( m_chromaKey = 0x00000000 );
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

uint32 aui_SDLSurface::TransformSurfacePixelFormatToSDL(const AUI_SURFACE_PIXELFORMAT pixelFormat) {
    switch (pixelFormat) {
        case AUI_SURFACE_PIXELFORMAT_332:
            return SDL_PIXELFORMAT_RGB332;
        case AUI_SURFACE_PIXELFORMAT_565:
            return SDL_PIXELFORMAT_RGB565;
        case AUI_SURFACE_PIXELFORMAT_555:
            return SDL_PIXELFORMAT_RGB555;
        case AUI_SURFACE_PIXELFORMAT_888:
            return SDL_PIXELFORMAT_RGB888;
        default:
            return SDL_PIXELFORMAT_UNKNOWN;
    }
}

SDL_Surface* aui_SDLSurface::CreateSDLSurface(const int width, const int height, const int bpp)
{
    const AUI_SURFACE_PIXELFORMAT pixelFormat = TransformBppToSurfacePixelFormat(bpp);

    int sdlBpp;
    uint32 rmask;
    uint32 gmask;
    uint32 bmask;
    uint32 amask;
    SDL_PixelFormatEnumToMasks(TransformSurfacePixelFormatToSDL(pixelFormat), &sdlBpp, &rmask, &gmask, &bmask, &amask);
    return SDL_CreateRGBSurface(0, width, height, sdlBpp, rmask, gmask, bmask, amask);
}

uint32 aui_SDLSurface::SetChromaKey( uint32 color ) {
    int hr = SDL_SetColorKey(m_lpdds, SDL_TRUE, /*SDL_MapRGB(m_lpdds->format, color>>16, (color>>8)&0xff, color&0xff)*/color); //|SDL_RLEACCEL ?
    //hr == 0 if succeded!
    //fprintf(stderr, "%s L%d: SDL_SRCCOLORKEY set to %#X\n", __FILE__, __LINE__, color);

    if ( hr == 0 )
        return aui_Surface::SetChromaKey( color ); //sets aui_Surface.m_chromaKey and returns last value!

    //return AUI_ERRCODE_OK;  //this is not sensible, should retrun last color key!?!
    //fprintf(stderr, "%s L%d: SDL_SRCCOLORKEY setting failed!\n", __FILE__, __LINE__);
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
    //fprintf(stderr, "%s L%d: Locking mutex!\n", __FILE__, __LINE__);
    if (SDL_MUSTLOCK(m_lpdds)) {
//        fprintf(stderr, "%s L%d: Locking surface! Check this!\n", __FILE__, __LINE__);
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

AUI_ERRCODE aui_SDLSurface::Blank(const uint32 &color)
{
	int errcode = SDL_FillRect(m_lpdds, NULL, color);
	if (errcode == 0)
		return AUI_ERRCODE_OK;

	return AUI_ERRCODE_BLTFAILED;
}

void aui_SDLSurface::Flip()
{
#if 0
	if(m_isPrimary)
		SDL_Flip(m_lpdds);
	// else?
#endif
}

#endif
