/*--------------------------------------------------------------------------
 Verify DLL

 Username and password verification utilities.
--------------------------------------------------------------------------*/

#include <stdio.h>
#include <windows.h>

#include "resource.h"
#include "shroud.h"
#include "verify.h"



#define MAX_USERNAME	12
#define MAX_PASSWORD	12


/*--------------------------------------------------------------------------
 user_t data type.

 'szUsername' contains a username.
 'szPassword' contains the corresponding password.
--------------------------------------------------------------------------*/
typedef struct
{
	char	szUsername[ MAX_USERNAME + 1 ];
	char	szPassword[ MAX_PASSWORD + 1 ];
} user_t;


static user_t verify_user;







/*--------------------------------------------------------------------------
 Compares the values in 'szUsername' and 'szPassword' against the list in
 'szListFile.'
 Returns TRUE if a match is found.
 Returnf FALSE otherwise.
--------------------------------------------------------------------------*/
BOOL IsValidUser( char *szListFile )
{
	BOOL bFoundUser = FALSE;	// Return value.
	FILE *file;					// Used to get the BITMAPFILEHEADER.
	BITMAPFILEHEADER bfh;		// Used to find the offset of the user list.
	shroud_t *sfile;			// Used to read the shrouded user list.
	user_t ValidUser;			// A user from the shrouded user list.

	if ( (file = fopen( szListFile, "rb" )) )
	{
		/* Get the BITMAPINFOHEADER. */
		fread( &bfh, sizeof( bfh ), 1, file );
		fclose( file );

		if ( (sfile = shroud_readopen( szListFile )) )
		{
			/* Seek to where the user list is. */
			shroud_seek( sfile, bfh.bfSize, SEEK_SET );

			/* Loop until we find one or reach the end. */
			while( !bFoundUser && shroud_read( sfile, (char *)&ValidUser, sizeof(user_t) ) == sizeof(user_t) )
			if ( strcmp( verify_user.szUsername, ValidUser.szUsername ) == 0
			&&   strcmp( verify_user.szPassword, ValidUser.szPassword ) == 0 )
				bFoundUser = TRUE;

			shroud_close( sfile );
		}
	}

	return bFoundUser;
}


/*--------------------------------------------------------------------------
 Retreives 'szUsername' and 'szPassword' from 'szFingerprintFile.'
 Returns TRUE on success.
 Returns FALSE otherwise.
--------------------------------------------------------------------------*/
BOOL GetInfoFromFingerprint( char *szFingerprintFile )
{
	BOOL bGotFingerprint = FALSE;	// Return value.
	shroud_t *sfile;				// Used to read the hidden and shrouded data.
	size_t offset;					// Each byte will be in a non-adjacent place.
	int i;							// Loop index.

	if ( (sfile = shroud_readopen( szFingerprintFile )) )
	{
		/* Assume we'll have success. */
		bGotFingerprint = TRUE;

		/* Start somewhere in the middle of the file. */
		offset = 200;

		/* Get the hidden shrouded username. */
		for ( i = 0; bGotFingerprint && i <= MAX_USERNAME; i++ )
		{
			if ( shroud_seek( sfile, offset, SEEK_CUR ) != 0
			||   shroud_read( sfile, verify_user.szUsername + i, 1 ) != 1 )
				bGotFingerprint = FALSE;
			else
				offset = (size_t)verify_user.szUsername[ i ];
		}

		/* Get the hidden shrouded password. */
		for ( i = 0; bGotFingerprint && i <= MAX_PASSWORD; i++ )
		{
			if ( shroud_seek( sfile, offset, SEEK_CUR ) != 0
			||   shroud_read( sfile, verify_user.szPassword + i, 1 ) != 1 )
				bGotFingerprint = FALSE;
			else
				offset = (size_t)verify_user.szPassword[ i ];
		}

		shroud_close( sfile );
	}

	return bGotFingerprint;
}
