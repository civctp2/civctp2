#ifndef dxver_h
#define dxver_h

#include <windows.h>


#define VER_FIRSTISLATER		1
#define VER_SECONDISLATER		2
#define VER_SAMEVERSION			4
#define VER_INVALIDINPUT		0

#define MAX_VERSION				256












#define DX_SOFTWARE				1


#define DX_HARDWARE				2



#define DX_NOINFO				3










int VersionCompare( LPSTR version1, LPSTR version2 );










void GetTheVersion( LPSTR szVersion, LPSTR szFile );






BOOL DirectXInstalled_3( void );






BOOL DirectXInstalled_3a( void );





BOOL DirectXInstalled_5( void );




























#ifdef __cplusplus
extern "C" {
#endif

BOOL WINAPI MicrosoftDirectXInstalled( DWORD *pVersion );

#ifdef __cplusplus
}
#endif


#endif
