/*--------------------------------------------------------------------------
 Shroud module.

 These functions are used to read from and write to "shrouded" files.  A
 shrouded file is an encoded version of some other file.  A shrouded file and
 its corresponding unencoded file are related in the following way:  Each byte
 in the unencoded file is transformed in value depending on its location in the
 file.  Hence, a shrouded file is identical in size to its corresponding
 non-shrouded file.

 A shrouded file is unreadable to any non-superhuman.  One can use a shrouded
 file in place of a file in which information must be hidden (and unalterable
 in a controlled sort of manner) from an average user.  For example, a
 shrouded localized strings file will be unreadable and so the strings will
 not be vulnerable to alteration.  As another example, a shrouded image file
 will be unreadable to any standard image viewing/manipulation program.

 Since shrouding a file alters each byte rather than rearranging bytes, one
 can use the functions in this file to read/write shrouded files and decode
 them one byte at a time.  For example, if you only want to read in the
 BITMAPINFOHEADER from a shrouded BMP file, you need only to use shroud_seek()
 to seek past the BITMAPFILEHEADER and shroud_read() to read and decode the
 next sizeof(BITMAPINFOHEADER) bytes of the shrouded BMP file. Similarly,
 shroud_write() will encode bytes as they are written to a shrouded file.

 Futhurmore, these functions correspond to commonly used stdio.h functions.
 So, when dealing with a shrouded file, one can make intuitive substitutions
 in their code:

	FILE *file							---> shroud_t *sfile
	fopen( filename, "rb" )				---> shroud_readopen( filename )
	fopen( filename, "wb" )				---> shroud_writeopen( filename )
	fopen( filename, "ab" )				---> shroud_appendopen( filename )
	fopen( filename, "r+b" )			---> shroud_updateopen( filename )
	fread( buffer, 1, length, file )	---> shroud_read( sfile, buffer, length )
	fwrite( buffer, 1, length, file )	---> shroud_write( sfile, buffer, length )
	fseek( file, offset, origin )		---> shroud_seek( sfile, offset, origin )
	fclose( file )						---> shroud_close( sfile )

 So we can deal with shrouded files in nearly exactly the same way in which
 we deal with any other file.  The encoding and decoding is handled internally.

 See the .h file for function declarations and descriptions of data types.
--------------------------------------------------------------------------*/

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "shroud.h"


/*--------------------------------------------------------------------------
 Open a shrouded file for reading.
 Uses fopen().
 Returns NULL on error.
--------------------------------------------------------------------------*/
shroud_t *shroud_readopen( char *filename )
{
	/* Allocate memory for the new shroud_t. */
	shroud_t *sfile = (shroud_t *) malloc( sizeof( shroud_t ) );
	if ( !sfile ) return NULL;

	/* Open the shrouded file for reading. */
	sfile->file = fopen( filename, "rb" );
	if ( !sfile->file )
	{
		free( sfile );
		return NULL;
	}

	/* Initialize 'curpos' and 'mode.' */
	sfile->curpos = 0;
	sfile->mode = shroud_READ;

	return sfile;
}


/*--------------------------------------------------------------------------
 Open a shrouded file for writing.
 Uses fopen().
 Returns NULL on error.
--------------------------------------------------------------------------*/
shroud_t *shroud_writeopen( char *filename )
{
	/* Allocate memory for the new shroud_t. */
	shroud_t *sfile = (shroud_t *) malloc( sizeof( shroud_t ) );
	if ( !sfile ) return NULL;

	/* Open the shrouded file for writing. */
	sfile->file = fopen( filename, "wb" );
	if ( !sfile->file )
	{
		free( sfile );
		return NULL;
	}

	/* Initialize 'curpos' and 'mode.' */
	sfile->curpos = 0;
	sfile->mode = shroud_WRITE;

	return sfile;
}


/*--------------------------------------------------------------------------
 Open a shrouded file for appending.
 Uses fopen().
 Subsequent calls to shroud_write() will reposition the file pointer to the
 end of the shrouded file before writing just as fwrite() would do to a normal
 file opened with the "ab" mode.  Calls to shroud_seek() will work but will
 not be of much use.
 Returns NULL on error.
--------------------------------------------------------------------------*/
shroud_t *shroud_appendopen( char *filename )
{
	/* Allocate memory for the new shroud_t. */
	shroud_t *sfile = (shroud_t *) malloc( sizeof( shroud_t ) );
	if ( !sfile ) return NULL;

	/* Open the shrouded file for writing. */
	sfile->file = fopen( filename, "ab" );
	if ( !sfile->file )
	{
		free( sfile );
		return NULL;
	}

	/* Initialize 'curpos' and 'mode.' */
	sfile->curpos = 0;
	sfile->mode = shroud_APPEND;

	return sfile;
}


/*--------------------------------------------------------------------------
 Open a shrouded file for reading and writing.  'filename' must exist and a
 call to shroud_seek() is necessary between calls to shourd_read() and
 shroud_write().
 Uses fopen().
 Returns NULL on error.
--------------------------------------------------------------------------*/
shroud_t *shroud_updateopen( char *filename )
{
	/* Allocate memory for the new shroud_t. */
	shroud_t *sfile = (shroud_t *) malloc( sizeof( shroud_t ) );
	if ( !sfile ) return NULL;

	/* Open the shrouded file for reading and writing. */
	sfile->file = fopen( filename, "r+b" );
	if ( !sfile->file )
	{
		free( sfile );
		return NULL;
	}

	/* Initialize 'curpos' and 'mode.' */
	sfile->curpos = 0;
	sfile->mode = shroud_UPDATE;

	return sfile;
}


/*--------------------------------------------------------------------------
 Read 'length' bytes from a shrouded file and decode them into 'buffer.'
 Uses fread().
 Returns the number of bytes successfully read.
--------------------------------------------------------------------------*/
size_t shroud_read( shroud_t *sfile, char *buffer, size_t length )
{
	size_t size; /* Number of bytes successfully read. */

	/* Read encoded bytes into 'buffer.' */
	size = fread( buffer, 1, length, sfile->file );

	/* Decode the bytes in 'buffer.' */
	shroud_decode( (unsigned char*) buffer, size, sfile->curpos );

	/* Update 'curpos.' */
	sfile->curpos += size;

	return size;
}


/*--------------------------------------------------------------------------
 Write 'length' bytes from 'buffer' and encode them into a shrouded file.
 Uses fwrite().
 The elements of 'buffer' will remain unaltered.
 Returns the number of bytes successfully written.
--------------------------------------------------------------------------*/
size_t shroud_write( shroud_t *sfile, char *buffer, size_t length )
{
	size_t bytecount = 0;										/* Number of bytes successfully written. */
	unsigned char encodedbuffer[ shroud_BLOCKSIZE ];			/* temporary buffer to store encoded blocks of 'buffer.' */
	const size_t numblocks = length / shroud_BLOCKSIZE;			/* numblocks <= length / shroud_BLOCKSIZE < numblocks + 1 */
	const size_t bytesremaining = length % shroud_BLOCKSIZE;	/* length == numblocks * shroud_BLOCKSIZE + bytesremaining */
	size_t blockcount;											/* Counter for the for loop. */

	/* If the shrouded file is open for appending, set 'curpos' to the end of the file. */
	if ( sfile->mode == shroud_APPEND )
		if ( shroud_seek( sfile, 0, SEEK_END ) != 0 )
			return 0;

	/* For each block... */
	for ( blockcount = numblocks; blockcount; blockcount-- )
	{
		/* Copy a block of buffer so as not to alter its elements. */
		memcpy( encodedbuffer, buffer, shroud_BLOCKSIZE );

		/* Encode the block. */
		shroud_encode( encodedbuffer, shroud_BLOCKSIZE, sfile->curpos );

		/* Write the encoded bytes to the shrouded file. */
		if ( fwrite( encodedbuffer, 1, shroud_BLOCKSIZE, sfile->file ) != shroud_BLOCKSIZE )
			return bytecount;

		/* Get ready for next pass. */
		sfile->curpos += shroud_BLOCKSIZE;
		bytecount += shroud_BLOCKSIZE;
		buffer += shroud_BLOCKSIZE;
	}

	/* Take care of any remaining bytes that didn't fill an entire block. */
	if ( bytesremaining )
	{
		memcpy( encodedbuffer, buffer, bytesremaining );

		shroud_encode( encodedbuffer, bytesremaining, sfile->curpos );

		if ( fwrite( encodedbuffer, 1, bytesremaining, sfile->file ) != bytesremaining )
			return bytecount;

		sfile->curpos += bytesremaining;
		bytecount += bytesremaining;
	}

	return bytecount;
}


/*--------------------------------------------------------------------------
 Moves the shrouded file pointer to a specified location.
 Essentially fseek().
 Returns 0 on success, nonzero on error.
--------------------------------------------------------------------------*/
int shroud_seek( shroud_t *sfile, long offset, int origin )
{
	int errorcode = fseek( sfile->file, offset, origin );
	if ( errorcode != 0 ) return errorcode;

	sfile->curpos = ftell( sfile->file );

	return 0;
}


/*--------------------------------------------------------------------------
 Close a shrouded file.
 Returns 0 on success, nonzero on error (e.g. disk full).
--------------------------------------------------------------------------*/
int shroud_close( shroud_t *sfile )
{
	int errorcode = fclose( sfile->file );
	if ( errorcode != 0 ) return errorcode;

	/* Free up memory allocated in shroud_*open(). */
	free( sfile );

	return 0;
}


/*--------------------------------------------------------------------------
 Test main.
--------------------------------------------------------------------------*/
/*#define	shroud_TESTMAIN
*/
#ifdef	shroud_TESTMAIN


#include <assert.h>


int main( void )
{
	shroud_t *sfile = NULL;			/* For reading/writing shrouded files. */
	FILE *file = NULL;				/* For reading/writing normal files. */
	size_t testsize = 256			/* We will test things using the entire ASCII set... */
		+ shroud_BLOCKSIZE			/* ...and use arrays larger than shroud_BLOCKSIZE... */
		+ (shroud_BLOCKSIZE>>2);	/* ...but not a multiple of shroud_BLOCKSIZE.  (see PROOF @ EOF.) */
	unsigned char *bytes;			/* The testing array. */
	unsigned char *morebytes;		/* Used to compare with 'bytes.' */
	int i;							/* Loop index. */
	int errorcode;					/* Used to assert(). */
	size_t size;					/* Used to assert(). */

	if ( !(bytes = malloc( testsize )) || !(morebytes = malloc( testsize )) )
	{
		perror( "malloc failed." );
		exit( 1 );
	}

	/* Create an input file with a repeating pattern of all possible characters. */
	file = fopen( "input.txt", "wb" );
	for ( i = 0; i < (int)testsize; i++ )
		fwrite( &(unsigned char)i, 1, 1, file );
	fclose( file );


	/*--------------------------
	Write a whole shrouded file.
	--------------------------*/

	/* Read in the input file. */
	file = fopen( "input.txt", "rb" );
	size = fread( bytes, 1, testsize, file );
	fclose( file );

	sfile = shroud_writeopen( "shrouded.txt" );
	assert( sfile != NULL );

	size = shroud_write( sfile, bytes, testsize );
	assert( size == testsize );

	errorcode = shroud_close( sfile );
	assert( errorcode == 0 );


	/*-------------------------
	Read a whole shrouded file.
	-------------------------*/

	sfile = shroud_readopen( "shrouded.txt" );
	assert( sfile != NULL );

	size = shroud_read( sfile, bytes, testsize );
	assert( size == testsize );

	errorcode = shroud_close( sfile );
	assert( errorcode == 0 );

	/* Write to an output file. */
	file = fopen( "output.txt", "wb" );
	size = fwrite( bytes, 1, testsize, file );
	fclose( file );

	/* Get 'morebytes' to compare with 'bytes.' */
	file = fopen( "input.txt", "rb" );
	size = fread( morebytes, 1, testsize, file );
	fclose( file );

	/* Make sure the encoding and decoding works. */
	assert( memcmp( bytes, morebytes, testsize ) == 0 );


	/*------------------------
	Append to a shrouded file.
	------------------------*/

	sfile = shroud_appendopen( "shrouded.txt" );
	assert( sfile != NULL );

	/* Do a meaningless seek to try and fool shroud_write(). */
	errorcode = shroud_seek( sfile, 10, SEEK_SET );
	assert( errorcode == 0 );

	/* Should write at end of file reguardless of current file position. */
	size = shroud_write( sfile, "abcde", 5 );
	assert( size == 5 );

	errorcode = shroud_close( sfile );
	assert( errorcode == 0 );

	sfile = shroud_readopen( "shrouded.txt" );
	assert( sfile != NULL );

	/* Seek to where the file previously ended. */
	errorcode = shroud_seek( sfile, testsize, SEEK_SET );
	assert( errorcode == 0 );

	size = shroud_read( sfile, bytes, 5 );
	assert( size == 5 );

	errorcode = shroud_close( sfile );
	assert( errorcode == 0 );

	assert( memcmp( bytes, "abcde", 5 ) == 0 );


	/*---------------------
	Update a shrouded file.
	---------------------*/

	sfile = shroud_updateopen( "shrouded.txt" );
	assert( sfile != NULL );

	/* Test SEEK_SET. */
	errorcode = shroud_seek( sfile, 10, SEEK_SET );
	assert( errorcode == 0 );

	/* Test SEEK_CUR. */
	errorcode = shroud_seek( sfile, 20, SEEK_CUR );
	assert( errorcode == 0 );

	/* Test writing in a file open for update. */
	size = shroud_write( sfile, "ABCDE", 5 );
	assert( size == 5 );

	/* Test SEEK_END. */
	errorcode = shroud_seek( sfile, -5, SEEK_END );
	assert( errorcode == 0 );

	/* Test reading in a file open for update. */
	size = shroud_read( sfile, bytes, 5 );
	assert( size == 5 );

	errorcode = shroud_close( sfile );
	assert( errorcode == 0 );

	/* Left over from the append test above. */
	assert( memcmp( bytes, "abcde", 5 ) == 0 );

	sfile = shroud_readopen( "shrouded.txt" );
	assert( sfile != NULL );

	/* Test SEEK_CUR from the beginning of the file. */
	errorcode = shroud_seek( sfile, 30, SEEK_CUR );
	assert( errorcode == 0 );

	size = shroud_read( sfile, bytes, 5 );
	assert( size == 5 );

	errorcode = shroud_close( sfile );
	assert( errorcode == 0 );

	assert( memcmp( bytes, "ABCDE", 5 ) == 0 );


	/* Clean up. */
	free( bytes );
	free( morebytes );

	/* If we're here, all was successful. */
	fprintf( stdout, "Succeess!\nShroud module test complete.\n" );

	return 0;
}


#endif


/*--------------------------------------------------------------------------
CLAIM: 'testsize' is not a multiple of 'shroud_BLOCKSIZE.'
PROOF: Assume it was.  Then testsize = shroud_BLOCKSIZE * c for some positive
integer c.  In other words, 256 + (5/4) * shroud_BLOCKSIZE = shroud_BLOCKSIZE * c.
So, 1024 = (4*c - 5)*shroud_BLOCKSIZE.  But 4*c - 5 is odd.  This means that
1024 has an odd factor which is impossible unless 4*c - 5 = 1.  But this can't
be either since it implies that c (which is an integer) = 3/2.
(4*c - 5 could also be -1 but this would imply that shroud_BLOCKSIZE be negative.)
So, by contradiction, we're done.
--------------------------------------------------------------------------*/
