//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : File object handling
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modified ReleaseFileFormat to fix some memory leaks, by Martin Gühmann.
//
//----------------------------------------------------------------------------

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


#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
void aui_MemMap::ReleaseFileFormat( aui_FileFormat *format )
{
	if ( format )
	{
		delete format;
		format = NULL;
	}
}

#else
//Added by Martin Gühmann

//----------------------------------------------------------------------------
//
// Name       : aui_MemMap::ReleaseFileFormat
//
// Description: Deletes a aui_FileFormat object.
//
// Parameters : format - The aui_FileFormat object to delete.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The validy of format does not need to be checked.
//              It is just a waste of time.
//              Nulling format after deletion is unnecessary and
//              causes harm here for some reasons.
//
//----------------------------------------------------------------------------
void aui_MemMap::ReleaseFileFormat( aui_FileFormat *format )
{
		delete format;
}
#endif
