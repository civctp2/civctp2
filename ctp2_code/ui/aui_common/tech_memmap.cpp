









#include "c3.h"

#include "tech_memmap.h"




int tech_MemMap::GetFileExtension( char *filename, char *extension )
{
	if ( !filename ) return 0;

	if ( !extension ) return 0;

	char *lastDot = strrchr( filename, '.' );
	if ( !lastDot ) return 0;

	
	strcpy( extension, ++lastDot );

	return 1;
}



unsigned char *tech_MemMap::GetFileBits(
	char *filename,
	size_t *outfilesize )
{
	if ( outfilesize ) *outfilesize = 0;

	unsigned long filesize;
	unsigned char *bits;
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

	bits = new unsigned char[filesize];

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
