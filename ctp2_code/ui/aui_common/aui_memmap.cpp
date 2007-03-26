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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Modified ReleaseFileFormat to fix some memory leaks, by Martin Gühmann.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "aui_memmap.h"

#include "aui_image.h"
#include "aui_sound.h"
#include "aui_movie.h"

/// Create an aui_FileFormat object
/// \param  filename The file to use  
/// \return The created object (or NULL, when the file format is not supported).
/// \remarks The format is derived from the extension of the file.
/// \see C3MemMap::GetFileFormat for more supported extensions.
aui_FileFormat * aui_MemMap::GetFileFormat(MBCHAR const * filename)
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

/// Delete an aui_FileFormat object
/// \param  format  The object to delete
/// \remarks Assumption: format is NULL, or has been created with GetFileFormat.
///          Would have liked to make format an aui_FileFormat * & , so it could
///          have been NULLed here. But this does not work with derived classes,
///          and using templates might cause problems with MSVC6. So, the caller
///          is now responsible for NULLing format.
void aui_MemMap::ReleaseFileFormat(aui_FileFormat * format)
{
	delete format;
}
