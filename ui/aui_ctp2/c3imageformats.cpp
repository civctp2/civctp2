












#include "c3.h"

#include <io.h>

#include "aui.h"
#include "aui_image.h"
#include "aui_surface.h"

#include "pixelutils.h"

#include "tiffutils.h"
#include "targautils.h"
#include "rimutils.h"

#include "c3imageformats.h"

#include "prjfile.h"
extern ProjectFile *g_ImageMapPF;






AUI_ERRCODE TiffImageFormat::Load( MBCHAR *filename, aui_Image *image )
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;
	uint16	width, height;
	uint16  *buffer;

	TIFGetMetrics( filename, &width, &height);

	errcode = image->LoadEmpty( width, height, 16 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) {
		c3errors_ErrorDialog("TIF Load", "Unable to find the file '%s'", filename);
		return AUI_ERRCODE_LOADFAILED;
	}

	aui_Surface *surface = image->TheSurface();

	if (!surface) return AUI_ERRCODE_LOADFAILED;

	errcode = surface->Lock( NULL, (LPVOID *)&buffer, 0 );

	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode == AUI_ERRCODE_OK )
	{
		BOOL		is565 = (surface->PixelFormat() == AUI_SURFACE_PIXELFORMAT_565);

		TIFLoadIntoBuffer16( filename, &width, &height, (uint16)surface->Pitch(), buffer, is565);

		errcode = surface->Unlock( buffer );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
			retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return retcode;
}





AUI_ERRCODE TargaImageFormat::Load(MBCHAR *filename, aui_Image *image)
{
	AUI_ERRCODE retcode = AUI_ERRCODE_OK;
	AUI_ERRCODE errcode;
	
	uint16  *buffer;

	int		width, height;
	int		bpp;

    
    if (_access(filename, 0) != 0) {
		return LoadRIM(filename, image);
    }        

	if (!Get_TGA_Dimension(filename, width, height, bpp)) {
		return AUI_ERRCODE_LOADFAILED;
    }

	errcode = image->LoadEmpty( width, height, 16 );
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) {
		MBCHAR	s[200];
		sprintf(s, "Unable to load the file '%s' (w:%d, h:%d)", filename, width, height);
		DPRINTF(k_DBG_FIX, (s));
		c3errors_ErrorDialog("Targa Load", s, filename);
		return AUI_ERRCODE_LOADFAILED;
	}

	aui_Surface *surface = image->TheSurface();

	if (!surface) return AUI_ERRCODE_LOADFAILED;

	errcode = surface->Lock( NULL, (LPVOID *)&buffer, 0 );

	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode == AUI_ERRCODE_OK )
	{
		Load_TGA_File(filename, (uint8 *)buffer, (int)surface->Pitch(), width, height, NULL, TRUE);

		errcode = surface->Unlock( buffer );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK )
			retcode = AUI_ERRCODE_SURFACEUNLOCKFAILED;
	}

	return retcode;
}




AUI_ERRCODE TargaImageFormat::LoadRIM(MBCHAR *filename, aui_Image *image)
{
    extern sint32 g_is565Format;

	AUI_ERRCODE errcode;
    uint8 *image_data;
    RIMHeader *rhead;

    void *buffer;

	int		width, height, pitch;
	int		record_is_565;
    long     size;

    char *basename;
    char rname[256];
    int  rlen;

    
    if (((basename = strrchr(filename, '\\')) == NULL) &&
        ((basename = strrchr(filename, '/')) == NULL)) {
        basename = filename;
    } else {
        basename++;
    }
    
    
    strcpy(rname, basename);
    rlen = strlen(rname);
    if (rlen < 3) {
		c3errors_ErrorDialog("Targa Load", "Invalid filename '%s'", filename);
        return AUI_ERRCODE_LOADFAILED;
    }
    rname[rlen - 3] = 'r';
    rname[rlen - 2] = 'i';
    rname[rlen - 1] = 'm';

    
    buffer = g_ImageMapPF->getData(rname, &size);
    if (buffer == NULL) {
		c3errors_ErrorDialog("Targa Load", "Unable to find the file '%s'", filename);
        return AUI_ERRCODE_LOADFAILED;
    }
    
    image_data = (uint8 *)buffer + sizeof(RIMHeader);

    
    rhead = (RIMHeader *)buffer;
    if ((rhead->tag[0] != 'R') ||
        (rhead->tag[1] != 'I') ||
        (rhead->tag[2] != 'M') ||
        (rhead->tag[3] != 'F') ||
        (rhead->version != 1)) {
		c3errors_ErrorDialog("Targa Load", "Invalid RIM file format '%s'", rname);
        return AUI_ERRCODE_LOADFAILED;
    }

    record_is_565 = ((rhead->flags & RFLAG_BIT_ENCODING) != 0);
    if (g_is565Format != record_is_565) {
		c3errors_ErrorDialog("Targa Load", "Invalid RIM file format '%s'", rname);
        return AUI_ERRCODE_LOADFAILED;
    }

    width = rhead->width;
    height = rhead->height;
    pitch = rhead->pitch;

	errcode = image->LoadFileMapped(width, height, 16, pitch, image_data);
	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) {
		c3errors_ErrorDialog("Targa Load", "Unable map the file '%s'", rname);
		return AUI_ERRCODE_LOADFAILED;
	}

	return errcode;
}
