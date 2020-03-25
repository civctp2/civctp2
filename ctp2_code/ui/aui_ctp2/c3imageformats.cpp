//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Loading of various image file formats
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
// - Return an error code when loading a TGA file fails.
//
//----------------------------------------------------------------------------

#include "c3.h"                     // Pre-compiled header
#include "c3imageformats.h"         // Own declarations: consistency check

#ifdef WIN32
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#define _stat stat
#endif// _MSC_VER

#endif

#if defined(__AUI_USE_SDL__)
#include <SDL.h>
#include <SDL_image.h>
#include "aui_sdlsurface.h"
#endif

#include "aui.h"
#include "aui_image.h"
#include "aui_surface.h"

#include "pixelutils.h"
#include "prjfile.h"
#include "rimutils.h"
#include "targautils.h"
#include "tiffutils.h"
#ifdef __linux__
#include "cifm.h"
#else
#define CI_FixName(a) a
#endif

extern ProjectFile *    g_ImageMapPF;
extern sint32           g_is565Format;

AUI_ERRCODE TiffImageFormat::Load(MBCHAR const * filename, aui_Image *image )
{
	uint16	width, height;
	TIFGetMetrics( CI_FixName(filename), &width, &height);

	AUI_ERRCODE errcode = image->LoadEmpty( width, height, 16 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) {
		c3errors_ErrorDialog("TIF Load", "Unable to find the file '%s'", CI_FixName(filename));
		return AUI_ERRCODE_LOADFAILED;
	}

	aui_Surface *surface = image->TheSurface();
	if (!surface) return AUI_ERRCODE_LOADFAILED;

	AUI_ERRCODE retcode = AUI_ERRCODE_OK;

	uint16  * buffer;
	errcode = surface->Lock( NULL, (LPVOID *)&buffer, 0 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode == AUI_ERRCODE_OK )
	{
		BOOL		is565 = (surface->PixelFormat() == AUI_SURFACE_PIXELFORMAT_565);
		TIFLoadIntoBuffer16( CI_FixName(filename), &width, &height, (uint16)surface->Pitch(), buffer, is565);

		errcode = surface->Unlock( buffer );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
			retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return retcode;
}





AUI_ERRCODE TargaImageFormat::Load(MBCHAR const * filename, aui_Image *image)
{
#ifdef WIN32
    if (_access(CI_FixName(filename), 0) != 0)
#else
    struct stat st;
    if (stat(CI_FixName(filename), &st) == -1)
#endif
    {
		return LoadRIM(CI_FixName(filename), image);
    }

	int		width;
    int     height;
	int		bpp;
	if (!Get_TGA_Dimension(CI_FixName(filename), width, height, bpp))
    {
		return AUI_ERRCODE_LOADFAILED;
    }

	AUI_ERRCODE errcode = image->LoadEmpty( width, height, 16 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) {
		MBCHAR	s[200];
		sprintf(s, "Unable to load the file '%s' (w:%d, h:%d)", CI_FixName(filename), width, height);
		c3errors_ErrorDialog("Targa Load", s, CI_FixName(filename));
		return AUI_ERRCODE_LOADFAILED;
	}

	aui_Surface * surface = image->TheSurface();
	if (!surface) return AUI_ERRCODE_LOADFAILED;

	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	uint16 *    buffer;
	errcode = surface->Lock( NULL, (LPVOID *)&buffer, 0 );

	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode == AUI_ERRCODE_OK )
	{
		if (Load_TGA_File(CI_FixName(filename), (uint8 *)buffer, (int)surface->Pitch(), width, height, NULL, TRUE))
        {
		    errcode = surface->Unlock( buffer );

		    Assert( errcode == AUI_ERRCODE_OK );
		    if ( errcode != AUI_ERRCODE_OK )
			    retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
        }
        else
        {
            (void) surface->Unlock(buffer);
            retcode = AUI_ERRCODE_LOADFAILED;
        }
	}

	return retcode;
}




AUI_ERRCODE TargaImageFormat::LoadRIM(MBCHAR const * filename, aui_Image *image)
{
    // Find the last DOS or Unix style file separator
    char const *    lastSep     = strrchr(filename, '\\');
    if (!lastSep)
    {
        lastSep = strrchr(filename, '/');
    }

    char const *    basename        = (lastSep) ? lastSep + 1 : filename;
    size_t const    BASE_LEN_MAX    = 255;
    size_t const    rlen            = strlen(basename);

    if ((rlen < 3) || (rlen >= BASE_LEN_MAX))
    {
		c3errors_ErrorDialog("Targa Load", "Invalid filename '%s'", filename);
        return AUI_ERRCODE_LOADFAILED;
    }

    char rname[1 + BASE_LEN_MAX];
    strncpy(rname, basename, rlen - 3);
    rname[rlen - 3] = 'r';
    rname[rlen - 2] = 'i';
    rname[rlen - 1] = 'm';
    rname[rlen]     = '\0';

    size_t  size    = 0;
    void *  buffer  = g_ImageMapPF ? g_ImageMapPF->getData(CI_FixName(rname), size) : NULL;

    if (buffer == NULL)
    {
        if (g_ImageMapPF && !g_ImageMapPF->IsReported(filename))
        {
		    c3errors_ErrorDialog("Targa Load", "Unable to find the file '%s'", filename);
            g_ImageMapPF->MarkReported(filename);
        }
        return AUI_ERRCODE_LOADFAILED;
    }

    uint8 *     image_data  = (uint8 *)buffer + sizeof(RIMHeader);
    RIMHeader * rhead       = (RIMHeader *)buffer;

    if ((rhead->tag[0] != 'R') ||
        (rhead->tag[1] != 'I') ||
        (rhead->tag[2] != 'M') ||
        (rhead->tag[3] != 'F') ||
        (rhead->version != 1)) {
		c3errors_ErrorDialog("Targa Load", "Invalid RIM file format '%s'", rname);
        return AUI_ERRCODE_LOADFAILED;
    }

    sint32 record_is_565 = ((rhead->flags & RFLAG_BIT_ENCODING) != 0);
    if (g_is565Format != record_is_565) {
		c3errors_ErrorDialog("Targa Load", "Invalid RIM file format '%s'", rname);
        return AUI_ERRCODE_LOADFAILED;
    }

    int width   = rhead->width;
    int height  = rhead->height;
    int pitch   = rhead->pitch;

	AUI_ERRCODE errcode = image->LoadFileMapped(width, height, 16, pitch, image_data);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) {
		c3errors_ErrorDialog("Targa Load", "Unable map the file '%s'", rname);
		return AUI_ERRCODE_LOADFAILED;
	}

	return errcode;
}
