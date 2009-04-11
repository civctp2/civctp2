









#include "c3.h"

#include "tech_memmap.h"




bool tech_MemMap::GetFileExtension(char const * filename, char * extension)
{
	if (filename && extension)
	{
		char const * lastDot = strrchr(filename, '.');
		if (lastDot)
		{
			strcpy(extension, ++lastDot);
			return true;
		}
	}

	return false;
}



unsigned char *tech_MemMap::GetFileBits
(
	const char *    filename,
	size_t *        outfilesize
)
{

	if ( outfilesize ) *outfilesize = 0;

	size_t        filesize;
	FILE *f = fopen( filename, "rb" );

	if ( !f )
		return NULL;
	
	if (fseek(f, 0, SEEK_END) == 0) {
		filesize = ftell(f);
	} else {
		fclose(f);
		return NULL;
	}

	if (fseek(f, 0, SEEK_SET) != 0) {
		fclose(f);
		return NULL;
	}

	unsigned char * bits = new unsigned char[filesize];

	if (!bits) {
		fclose(f);
		return NULL;
	}

	if ( fread( bits, 1, filesize, f ) != filesize ) {
		delete[] bits;
		fclose(f);
		return NULL;
	}

	fclose(f);

	if ( outfilesize ) *outfilesize = filesize;

	return bits;
}



void tech_MemMap::ReleaseFileBits( unsigned char *&bits )
{
	if (bits) {
		delete[] bits;
		bits = NULL;
	}
}
