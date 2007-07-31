









#include "c3.h"

#include "aui.h"

#include "aui_image.h"
#include "aui_sound.h"
#include "aui_movie.h"

#include "c3imageformats.h"

#include "c3memmap.h"



aui_FileFormat *C3MemMap::GetFileFormat( MBCHAR *filename )
{
	MBCHAR extension[ 8 ];
	if ( GetFileExtension( filename, extension ) )
	{
		if ( !strnicmp( extension, "tga", 3 ) )
			return new TargaImageFormat;
		if ( !strnicmp( extension, "tif", 3 ) )
			return new TiffImageFormat;
		if ( !strnicmp( extension, "bmp", 3 ) )
			return new aui_BmpImageFormat;
		if ( !strnicmp( extension, "wav", 3 ) )
			return new aui_WavSoundFormat;
		if ( !strnicmp( extension, "avi", 3 ) )
			return new aui_AviMovieFormat;
	}

	return NULL;
}
