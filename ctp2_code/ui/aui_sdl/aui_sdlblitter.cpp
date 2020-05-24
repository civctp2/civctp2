#include "c3.h"

#ifdef __AUI_USE_SDL__
#include "aui_sdlsurface.h"

#include "aui_sdlblitter.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#ifndef DD_OK
#define DD_OK 0
#endif
#ifndef DDERR_SURFACEBUSY
#define DDERR_SURFACEBUSY 3
#endif
#ifndef DDERR_WASSTILLDRAWING
#define DDERR_WASSTILLDRAWING 4
#endif

AUI_ERRCODE aui_SDLBlitter::Blt16To16(
    aui_Surface *destSurf,
    RECT *destRect,
    aui_Surface *srcSurf,
    RECT *srcRect,
    uint32 flags ) {

    AUI_ERRCODE retcode = AUI_ERRCODE_OK;

        if ( !destSurf->Buffer()
         &&   !srcSurf->Buffer()
         &&   destSurf->IsThisA( aui_SDLSurface::m_SDLSurfaceClassId )
         &&   srcSurf->IsThisA( aui_SDLSurface::m_SDLSurfaceClassId )
         &&   (!flags || (flags & k_AUI_BLITTER_FLAG_COPY) || (flags & k_AUI_BLITTER_FLAG_CHROMAKEY)) )

	{
        AUI_ERRCODE err = AUI_ERRCODE_OK;

        aui_SDLSurface* sdlDest = static_cast<aui_SDLSurface*>(destSurf);
        aui_SDLSurface* sdlSrc  = static_cast<aui_SDLSurface*>(srcSurf);
        // lock both surfaces
        // note that there is a chance of a deadlock if a different
        // thread locks the two surfaces in the opposite order
        /*SDL_LockMutex(sdlDest->m_bltMutex); //http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fBlitSurface says don't lock surface! mutex lock OK but seems not needed
        SDL_LockMutex(sdlSrc->m_bltMutex);*/
        SDL_Rect ssrc = { srcRect->left, srcRect->top, srcRect->right-srcRect->left, srcRect->bottom-srcRect->top };
        SDL_Rect sdst = { destRect->left, destRect->top, 0, 0 };

/*        if ( flags & k_AUI_BLITTER_FLAG_CHROMAKEY ) { //is k_AUI_BLITTER_FLAG_CHROMAKEY in flags?
            //ChromaKey and SDL_SRCCOLORKEY  should be set with aui_SDLSurface::SetChromaKey on
            //the SDL-surface already but if SDL_SRCCOLORKEY is missing we set it here:
            if (SDL_SetColorKey(sdlSrc->DDS(), SDL_SRCCOLORKEY, sdlSrc->GetChromaKey())) //sdlSrc->DDS()->format->colorkey))
                fprintf(stderr, "%s L%d: SDL_SRCCOLORKEY setting failed! key %#X\n", __FILE__, __LINE__, sdlSrc->DDS()->format->colorkey);
            //else fprintf(stderr, "%s L%d: SDL_SRCCOLORKEY setting succeded!\n", __FILE__, __LINE__);
            }
*/  //there seems no need for all this
        if (SDL_BlitSurface(sdlSrc->DDS(), &ssrc, sdlDest->DDS(), &sdst) < 0) {
                fprintf(stderr, "%s L%d: Blit failed: %s\n", __FILE__, __LINE__, SDL_GetError());
                retcode = AUI_ERRCODE_BLTFAILED;
            }

        // unlock in the opposite order
        /*SDL_UnlockMutex(sdlSrc->m_bltMutex);
        SDL_UnlockMutex(sdlDest->m_bltMutex);*/

        if(err != AUI_ERRCODE_OK) {
            DPRINTF(k_DBG_UI, ("%s:%d: err = %d\n", __FILE__, __LINE__, err));
            }
        if ( err != AUI_ERRCODE_OK )
            retcode = AUI_ERRCODE_BLTFAILED;
	}
    else {
        return aui_Blitter::Blt16To16(  //perhaps SDL can handle more flags on its own?
                                        //gets called when not SDL_surface
            destSurf,
            destRect,
            srcSurf,
            srcRect,
            flags );
        }
    return retcode;
    }


AUI_ERRCODE aui_SDLBlitter::ColorBlt16(
    aui_Surface *destSurf,
    RECT *destRect,
    uint32 color,
    uint32 flags )
    {
    AUI_ERRCODE retcode = AUI_ERRCODE_OK;


    if ( !destSurf->Buffer()
         &&   destSurf->IsThisA( aui_SDLSurface::m_SDLSurfaceClassId ) )
	{
        aui_SDLSurface* sdlDest = static_cast<aui_SDLSurface*>(destSurf);
        /*SDL_LockMutex(sdlDest->m_bltMutex);*/

        SDL_Rect sdst = { destRect->left, destRect->top,
                          destRect->right-destRect->left, destRect->bottom-destRect->top
	    };
        if (SDL_FillRect(sdlDest->DDS(), &sdst, color) < 0)
	    {
            fprintf(stderr, "FillRect failed: %s\n", SDL_GetError());
            retcode = AUI_ERRCODE_BLTFAILED;
	    }
        /*SDL_UnlockMutex(sdlDest->m_bltMutex);*/
	}
    else
        return aui_Blitter::ColorBlt16(
            destSurf,
            destRect,
            color,
            flags );

    return retcode;
    }

AUI_ERRCODE aui_SDLBlitter::ColorStencilBlt16(
    aui_Surface *destSurf,
    RECT *destRect,
    aui_Surface *stencilSurf,
    RECT *stencilRect,
    uint32 color,
    uint32 flags )
    {
    return AUI_ERRCODE_OK;
    }

// stretch the source horizontally and vertically
static AUI_ERRCODE SimpleHVStretch(SDL_Surface* src, SDL_Rect* rsrc,
				   SDL_Surface* dst, SDL_Rect* rdst)
    {
//    printf("StretchBlt %dx%d -> %dx%d\n", rsrc->w,rsrc->h, rdst->w,rdst->h);
    if (rdst->h <= 1 && rdst->w <= 1) {
	// copy only (at most) the top-left pixel
	if (SDL_BlitSurface(src, rdst, dst, rdst) < 0) {
	    fprintf(stderr, "Blt failed: %s\n", SDL_GetError());
	    return AUI_ERRCODE_BLTFAILED;
            }
	return AUI_ERRCODE_OK;
        }

    if (SDL_MUSTLOCK(src)) {
	if (SDL_LockSurface(src) < 0) {
	    fprintf(stderr, "StretchBlt: lock src failed: %s\n", SDL_GetError());
	    return AUI_ERRCODE_BLTFAILED;
            }
        }
    if (SDL_MUSTLOCK(dst)) {
	if (SDL_LockSurface(dst) < 0) {
	    fprintf(stderr, "StretchBlt: lock dst failed: %s\n", SDL_GetError());
	    SDL_UnlockSurface(src);
	    return AUI_ERRCODE_BLTFAILED;
            }
        }

    AUI_ERRCODE retcode = AUI_ERRCODE_OK;

    // use a Bresenham-like algorithm for both axes
    Uint8* f8 = (Uint8*)src->pixels;
    Uint8* t8 = (Uint8*)dst->pixels;

    int ly = rdst->h/2;			// Bresenham level for y
    int dy = rdst->y;
    int edy = rdst->y + rdst->h;	// end destination y
    int sy = rsrc->y;
    int esy = rsrc->y + rsrc->h;	// end source y
    while (dy < edy && sy < esy)
        {
	// copy pixels into line dy while the level is non-negative
	while (ly >= 0 && dy < edy && sy < esy)
            {
	    // write source line sy
	    int lx = rdst->w/2;			// Bresenham level for x
	    int dx = rdst->x;
	    int edx = rdst->x + rdst->w;	// end destination x
	    int sx = rsrc->x;
	    int esx = rsrc->x + rsrc->w;	// end source x
	    Uint16* dptr = (Uint16*)(t8 + dy*dst->pitch);
	    Uint16* sptr = (Uint16*)(f8 + sy*src->pitch);
	    while (dx < edx && sx < esx)
                {
		// put a pixel in column dx while the level is non-negative
		while (lx >= 0 && dx < edx && sx < esx) {
		    // write source pixel sx
		    assert(src->format->BytesPerPixel == 2);
		    assert(dst->format->BytesPerPixel == 2);
		    dptr[dx] = sptr[sx];
		    lx -= rsrc->w-1;
		    ++dx;
                    }
		lx += rdst->w-1;
		++sx;
                }
	    ly -= rsrc->h-1;
	    ++dy;
            }
	ly += rdst->h-1;
	++sy;
        }

    if (SDL_MUSTLOCK(dst)) {
	SDL_UnlockSurface(dst);
        }
    if (SDL_MUSTLOCK(src)) {
	SDL_UnlockSurface(src);
        }
    return retcode;
    }

// stretch the source horizontally, heights are equal
static AUI_ERRCODE SimpleHStretch(SDL_Surface* src, SDL_Rect* rsrc,
				  SDL_Surface* dst, SDL_Rect* rdst)
    {
//    printf("StretchBlt H %dx%d -> %dx%d\n", rsrc->w,rsrc->h, rdst->w,rdst->h);
    if (rdst->w <= 1) {
	// copy only (at most) the first line
	if (SDL_BlitSurface(src, rdst, dst, rdst) < 0) {
	    fprintf(stderr, "Blt failed: %s\n", SDL_GetError());
	    return AUI_ERRCODE_BLTFAILED;
            }
	return AUI_ERRCODE_OK;
        }
    // TODO: optimize this
    return SimpleHVStretch(src, rsrc, dst, rdst);
    }

// stretch the source vertically, widths are equal
static AUI_ERRCODE SimpleVStretch(SDL_Surface* src, SDL_Rect* rsrc,
				  SDL_Surface* dst, SDL_Rect* rdst)
    {
//    printf("StretchBlt V %dx%d -> %dx%d\n", rsrc->w,rsrc->h, rdst->w,rdst->h);
    if (rdst->h <= 1) {
	// copy only (at most) the first column
	if (SDL_BlitSurface(src, rdst, dst, rdst) < 0) {
	    fprintf(stderr, "Blt failed: %s\n", SDL_GetError());
	    return AUI_ERRCODE_BLTFAILED;
            }
	return AUI_ERRCODE_OK;
        }
    // TODO: optimize this
    return SimpleHVStretch(src, rsrc, dst, rdst);
    }

AUI_ERRCODE aui_SDLBlitter::StretchBlt16To16(
    aui_Surface *destSurf,
    RECT *destRect,
    aui_Surface *srcSurf,
    RECT *srcRect,
    uint32 flags )
    {
    AUI_ERRCODE retcode = AUI_ERRCODE_OK;




    if ( !destSurf->Buffer()
         &&   !srcSurf->Buffer()
         &&   destSurf->IsThisA( aui_SDLSurface::m_SDLSurfaceClassId )
         &&   srcSurf->IsThisA( aui_SDLSurface::m_SDLSurfaceClassId )
         &&   (!flags || (flags &
                          (k_AUI_BLITTER_FLAG_COPY | k_AUI_BLITTER_FLAG_CHROMAKEY))) )
	{
        AUI_ERRCODE err = AUI_ERRCODE_OK;

        aui_SDLSurface* sdlDest = static_cast<aui_SDLSurface*>(destSurf);
        aui_SDLSurface* sdlSrc  = static_cast<aui_SDLSurface*>(srcSurf);
        // lock both surfaces
        // note that there is a chance of a deadlock if a different
        // thread locks the two surfaces in the opposite order
        /*SDL_LockMutex(sdlDest->m_bltMutex); //http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fBlitSurface says don't lock surface! mutex lock OK but seems not needed
        SDL_LockMutex(sdlSrc->m_bltMutex);*/

        SDL_Rect ssrc = { srcRect->left, srcRect->top, srcRect->right-srcRect->left, srcRect->bottom-srcRect->top };
        SDL_Rect sdst = { destRect->left, destRect->top, destRect->right-destRect->left, destRect->bottom-destRect->top };

        if (ssrc.w == sdst.w) {
            if (ssrc.h == sdst.h) {
                //fprintf(stderr, "%s L%d: Using normal blit!\n", __FILE__, __LINE__);
                if (SDL_BlitSurface(sdlSrc->DDS(), &ssrc, sdlDest->DDS(), &sdst) < 0) {
                    fprintf(stderr, "StrechBlt failed: %s\n", SDL_GetError());
                    retcode = AUI_ERRCODE_BLTFAILED;
                    }
                }
            else {
                //fprintf(stderr, "%s L%d: Using V blit!\n", __FILE__, __LINE__);
                retcode = SimpleVStretch(sdlSrc->DDS(), &ssrc, sdlDest->DDS(), &sdst);
                }
            }
        else if (ssrc.h == sdst.h) {
            //fprintf(stderr, "%s L%d: Using H blit!\n", __FILE__, __LINE__);
            retcode = SimpleHStretch(sdlSrc->DDS(), &ssrc, sdlDest->DDS(), &sdst);
            }
        else {
            //fprintf(stderr, "%s L%d: Using HV blit!\n", __FILE__, __LINE__);
            retcode = SimpleHVStretch(sdlSrc->DDS(), &ssrc, sdlDest->DDS(), &sdst);
            }

        // unlock in the opposite order
        /*SDL_UnlockMutex(sdlSrc->m_bltMutex);
        SDL_UnlockMutex(sdlDest->m_bltMutex);*/

        Assert( err == DD_OK );
        if ( err != DD_OK ) retcode = AUI_ERRCODE_BLTFAILED;
	}
    else {
        return aui_Blitter::StretchBlt16To16(
            destSurf,
            destRect,
            srcSurf,
            srcRect,
            flags );
        }
    return retcode;
    }

#endif // __AUI_USE_SDL__
