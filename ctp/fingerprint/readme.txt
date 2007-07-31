this is the source code necessary to implement fingerprinting.  this code can be divided into two
parts: 

1)  placing the fingerprint data into a file
2)  and verifying the fingerprint data in a file.

this readme is only concerned with part 2).


the function to verify fingerprint data requires the following two arguments:


- filename of fingerprint file.

- filename of list file


obtain these from the people making the installer for your product, as it is usually the function of
the installer to create the fingerprint file and use the list file.

the functions to call to perform this verification are:

verify.c:

	BOOL GetInfoFromFingerprint( char *szFingerprintFile );
	BOOL IsValidUser( char *szListFile );
	
to test fingerprint validity, just do:

#include "verify.h"

...

if( GetInfoFromFingerprint( szFingerprintFile ) && IsValidUser( szListFile ) )
	success...
else
	failure...
...

	
the files you'll need to include in your project are:

verify.c
shroud.c
endecode.c
shroud.h
verify.h



the list file and fingerprint file are ordinary game assets into which you secretly insert username
and password information.  the list file contains a listing of all the valid usernames and passwords,
and should be constructed by your team using our tools:

the list file must be a bitmap file.  The list data is appended to the end of the bitmap file, and list file reader code 
relies on the bfSize field in the BITMAPINFOHEADER struct to know where the list file data begins in the file.

to construct/modify/view the list file, use listmod.exe.  this is a console app which functions like a shell.  it displays its
own help.  when inventing usernames and passwords, remember that both are limited to 12 characters (not including null terminator),
and both are case-sensitive.



the fingerprint will have data inserted into it at offset 200 thru offset 247.  Bytes in the file in this range will be replaced,
so the file should be indifferent to changes in this area, and be at least 247 bytes in size.  wav files make good candidates for
fingerprint files, especially short ones with a sudden attack.


the username and password data is shrouded is the list and fingerprint files. 
however, it is still wise to keep the identities of the list and fingerprint files secret.