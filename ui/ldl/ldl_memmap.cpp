#include "c3.h"

#include "ldl_memmap.h"



unsigned char *ldl_MemMap::GetFileBits( char *filename, unsigned long *junk )
{
	unsigned int filesize;
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
		delete[filesize] bits;
		fclose(f);
		return NULL;
	}

	fclose(f);

	return bits;

}



void ldl_MemMap::ReleaseFileBits( unsigned char *&bits )
{
	if ( bits ) {
		delete[] bits;
		bits = NULL;
	}
}
