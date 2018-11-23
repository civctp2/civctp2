/*--------------------------------------------------------------------------
 encode and decode functions.

 These functions are used by shroud_read/write() in the shroud module but
 are presented here so that they may be used ouside of the module.

 See the shroud.h file for function declarations.
--------------------------------------------------------------------------*/

#include "shroud.h"

/*--------------------------------------------------------------------------
 Encode 'length' elements of 'bytes' using 'operand' as the encoding key.
 When used with shrouded files, 'operand' represents the offset of the the
 first character of 'bytes' in the file.  However, it is not mandatory that
 'operand' be used this way outside of shrouded files...

 In general, one can deduce the proper usage of encode() by accepting as an
 axiom that the following two snippets produce equivalent results:

	// this line of code...

	shroud_encode(    ptr     , len1 + len2 ,    opr     );

	// is equivalent to these two lines...

	shroud_encode(    ptr     ,    len1     ,    opr     );
	shroud_encode( ptr + len1 ,    len2     , opr + len1 );

--------------------------------------------------------------------------*/
void shroud_encode( unsigned char *bytes, size_t length, size_t operand )
{
	unsigned char uc_operand = (unsigned char)(41*operand);
	unsigned char uc_offset = (unsigned char)(operand/70 + 53);
	unsigned char uc_count = (unsigned char)(operand%70);

	while ( length-- )
	{
		/* If 'byte' is odd in value, adjust its value but still keep it odd
		so that the operation is invertible.*/
		if ( *bytes & 0x01 ) *bytes += 108;

		/* Bitwise-exclusive-or 'byte' with 41 times 'operand.'  'operand' is
		scaled so that identical adjacent bytes will be shrouded better. */
		*bytes ^= uc_operand;

		/* Every 70 characters will get a different value added to it.  This way,
		repeating strings in a given file will be endcoded differently depending on
		their location in the file. Add 53 so that the leading characters of a file
		will be altered. */
		*bytes += uc_offset;

		/* Get ready for next pass. */
		bytes++;
		uc_operand += 41;
		if ( ++uc_count == 70 )
		{
			uc_count = 0;
			uc_offset++;
		}
	}
}

/*--------------------------------------------------------------------------
 Decode 'length' elements of 'bytes' using 'operand' as the decoding key.
 When used with shrouded files, 'operand' represents the offset of the the
 first character of 'bytes' in the file.  However, it is not mandatory that
 'operand' be used this way outside of shrouded files...

 In general, one can deduce the proper usage of decode() by accepting as an
 axiom that the following two snippets produce equivalent results:

	// this line of code...

	shroud_decode(    ptr     , len1 + len2 ,    opr     );

	// is equivalent to these two lines...

	shroud_decode(    ptr     ,    len1     ,    opr     );
	shroud_decode( ptr + len1 ,    len2     , opr + len1 );

--------------------------------------------------------------------------*/
void shroud_decode( unsigned char *bytes, size_t length, size_t operand )
{
	unsigned char uc_operand = (unsigned char)(41*operand);
	unsigned char uc_offset = (unsigned char)(operand/70 + 53);
	unsigned char uc_count = (unsigned char)(operand%70);

	while ( length-- )
	{
		/* These operations undo what the operations in encode() did.
		See shroud_encode() for comments. */
		*bytes -= uc_offset;
		*bytes ^= uc_operand;
		if ( *bytes & 0x01 ) *bytes -= 108;

		/* Get ready for next pass. */
		bytes++;
		uc_operand += 41;
		if ( ++uc_count == 70 )
		{
			uc_count = 0;
			uc_offset++;
		}
	}
}

/*--------------------------------------------------------------------------
 Test main.
--------------------------------------------------------------------------*/
/*#define	endedode_TESTMAIN
*/
#ifdef	endedode_TESTMAIN

#include <assert.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#define LENGTH1 90
#define LENGTH2 110

int main( void )
{
	unsigned char bytes[ LENGTH1 + LENGTH2 ];		/* Some test bytes. */
	unsigned char ebytesA[ LENGTH1 + LENGTH2 ];		/* Used to encode 'bytes' all at once. */
	unsigned char ebytesB[ LENGTH1 + LENGTH2 ];		/* Used to encode 'bytes' in two chuncks. */
	size_t operand;									/* Used by encode() and decode(). */
	int i;											/* Loop index. */

	srand( (unsigned int)time( NULL ) );

	/* Fill 'bytes' with randomely generated data. */
	for ( i = 0; i < LENGTH1 + LENGTH2; i++ )
		bytes[ i ] = (unsigned char)rand();

	/* Pick some random number for the operand. */
	operand = (size_t)rand();

	/* Make sure there will be no rollover. */
	while ( operand + LENGTH1 + LENGTH2 < operand )
		operand = (size_t)rand();

	/* Get two copies of 'bytes.' */
	memcpy( ebytesA, bytes, LENGTH1 + LENGTH2 );
	memcpy( ebytesB, bytes, LENGTH1 + LENGTH2 );

	/*-----
	Encode.
	-----*/

	/* Encode all of 'bytes' at once. */
	shroud_encode( ebytesA, LENGTH1 + LENGTH2, operand );

	/* Encode 'bytes' in two chuncks. */
	shroud_encode( ebytesB, LENGTH1, operand );
	shroud_encode( ebytesB + LENGTH1, LENGTH2, operand + LENGTH1 );

	/* Make sure we got the same results. */
	assert( memcmp( ebytesA, ebytesB, LENGTH1 + LENGTH2 ) == 0 );

	/*-----
	Decode.
	-----*/

	/* Decode all at once. */
	shroud_decode( ebytesA, LENGTH1 + LENGTH2, operand );

	/* Decode two chuncks. */
	shroud_decode( ebytesB, LENGTH1, operand );
	shroud_decode( ebytesB + LENGTH1, LENGTH2, operand + LENGTH1 );

	/* Make sure we got the same results. */
	assert( memcmp( ebytesA, ebytesB, LENGTH1 + LENGTH2 ) == 0 );

	/* Make sure that decode() undid encode(). */
	assert( memcmp( bytes, ebytesA, LENGTH1 + LENGTH2 ) == 0 );

	/* At this point, all is well. */
	fprintf( stdout, "Succeess!\nEndecode module test complete.\n" );

	return 0;
}

#endif
