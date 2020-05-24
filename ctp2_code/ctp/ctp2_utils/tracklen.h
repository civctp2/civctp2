//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  :
// Id           : $Id$
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
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef tracklen_h
#define tracklen_h

#if !defined(USE_SDL)

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VERSIONINFO			"V01.000 Overburn"
#define CHRVERSIONINFOLEN	30
#define DWVERSIONINFOLEN	CHRVERSIONINFOLEN / sizeof( DWORD ) + 1

enum{ _TRACKLEN_OK, _TRACKLEN_VERSIONMISMATCH, _TRACKLEN_LOADFAILED, _TRACKLEN_CHECKFAILED };

#define tracklen_MAXTRACKS 128

#define tracklen_DATFILE "texture.dat"

#ifdef tracklen_LOGGING
#define tracklen_LOGFILE "cd.log"

extern FILE *tracklen_fp;
extern char tracklen_buf[];

char *tracklen_cryptAscii(char *s);

#endif

void tracklen_cryptBinary(char *data, size_t len);
DWORD *tracklen_LoadEncryptedKey( DWORD *trackLenBuf, const char *szFile );
int tracklen_GetTrackLengths(DWORD *trackLenBuf, char whichDrive);
char *GetVersionInfo( DWORD *trackLenBuffer );
BYTE tracklen_CheckTrackLengths( char *szVersionInfoBuffer = NULL );
void tracklen_AutoPlay_Disable();
void tracklen_AutoPlay_Restore();

#ifdef __cplusplus
}
#endif

#endif // USE_SDL

#endif // tracklen_h
