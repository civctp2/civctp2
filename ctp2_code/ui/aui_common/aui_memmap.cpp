










#include "c3.h"

#include "aui_image.h"
#include "aui_sound.h"
#include "aui_movie.h"

#include "aui_memmap.h"



aui_FileFormat *aui_MemMap::GetFileFormat( MBCHAR *filename )
{
	MBCHAR extension[ 8 ];
	if ( GetFileExtension( filename, extension ) )
	{
		if ( !strnicmp( extension, "bmp", 3 ) )
			return new aui_BmpImageFormat;
		if ( !strnicmp( extension, "wav", 3 ) )
			return new aui_WavSoundFormat;
		if ( !strnicmp( extension, "avi", 3 ) )
			return new aui_AviMovieFormat;
	}

	return NULL;
}



void aui_MemMap::ReleaseFileFormat( aui_FileFormat *format )
{
	if ( format )
	{
		delete format;
		format = NULL;
	}
}
