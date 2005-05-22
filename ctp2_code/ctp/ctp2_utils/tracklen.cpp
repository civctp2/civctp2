//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - added linux specific code
//
//----------------------------------------------------------------------------
#include "c3.h"

#include <stdio.h>

#if defined(WIN32)
#include "windows.h"
#include "mbstring.h"
#include "mmsystem.h"
#elif defined(LINUX)
#include <sys/types.h>
#include <linux/cdrom.h>
#include <linux/iso_fs.h>
#include <sys/ioctl.h>
#endif
#if defined(USE_SDL)
#include <SDL_cdrom.h>
#endif
#include "cheatkey.h"

#include "tracklen.h"

#ifdef tracklen_LOGGING

FILE *tracklen_fp = NULL;

char tracklen_buf[512];


char *tracklen_cryptAscii(char *s)
{
	static char *keyy = "Checking aspiration formants.\n";

	int i = 0;
	char *p = s;
	static char buf[256];
	char *q = buf;

	while (*p) {
		int k = keyy[i++] & 31;
		if (*p >= 32)
			*q++ = *p++ ^ k;
		else
			*q++ = *p++;
		if (!keyy[i]) i = 0;
	}
	*q++ = 0;
	return buf;
}

#ifdef tracklen_CRYPTLOG
#define tracklen_DPRINT(ss) fprintf(tracklen_fp, "%s:%d ", "tar", 1000+__LINE__); sprintf ss; fprintf(tracklen_fp, "%s", tracklen_cryptAscii(tracklen_buf))
#else
#define tracklen_DPRINT(ss) fprintf(tracklen_fp, "%s:%d ", "tar", 1000+__LINE__); sprintf ss; fprintf(tracklen_fp, "%s", tracklen_buf)
#endif

static void DMCIError(int error)
{
	char szScratch[ MAX_PATH ];
	mciGetErrorString( error, szScratch, MAX_PATH );
	tracklen_DPRINT((tracklen_buf,  "last MCI error = %s\n", szScratch ));
}


#else
#define tracklen_DPRINT(ss) 
#define DMCIError(error)
#endif 





void tracklen_cryptBinary(char *data, size_t len)
{
	BYTE *p = cheatkey_get();

	for (size_t index = 0; index < len; index++ )
		data[ index ] ^= p[ index % 400 ];
}


static int tracklen_GetTrackLengthsViaHandle( DWORD *trackLenBuf, unsigned int wDeviceID )
{
#if defined(WIN32) && !defined(USE_SDL)
	MCI_STATUS_PARMS msp;
	MCI_SET_PARMS mp;
	long totalLen_ms = 0;
#endif
	DWORD i;
	BOOL bAllTracksOK = TRUE;
	uint64 totalLen_ms = 0;
	int iRet;

#if defined(WIN32) && !defined(USE_SDL)
	memset(&mp, 0, sizeof(mp));
	mp.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	iRet = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)&mp);
	if( iRet ) {
		tracklen_DPRINT((tracklen_buf, "GetTrackLengths: failed to set time format.\n" ));
		return __LINE__;
	}
	tracklen_DPRINT((tracklen_buf, "GetTrackLengths: set time format to milliseconds.\n"));

	
	memset(&msp, 0, sizeof(msp));
	msp.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if( iRet = mciSendCommand( wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&msp ) ) {
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: failed to read # of tracks\n"));
		DMCIError( iRet );
		return __LINE__;
	}
	trackLenBuf[0] = msp.dwReturn;
#elif defined(USE_SDL)
	SDL_CD *cdrom = SDL_CDOpen(wDeviceID);
	if (!cdrom)
		return __LINE__;
	if (!CD_INDRIVE(SDL_CDStatus(cdrom))) {
		return __LINE__;
	}
	trackLenBuf[0] = cdrom->numtracks;
#else
	return __LINE__;
#endif
	
	tracklen_DPRINT((tracklen_buf, "GetTrackLengths: %d tracks\n", trackLenBuf[0]));
	if (trackLenBuf[0] > tracklen_MAXTRACKS) {
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: too many tracks\n"));
		return __LINE__;
	}
	
	for (i = 1; i <= trackLenBuf[0]; i++) {
#if defined(WIN32) && !defined(USE_SDL)
		memset(&msp, 0, sizeof(msp));
		msp.dwItem = MCI_STATUS_LENGTH;
		msp.dwTrack = i;
		iRet = mciSendCommand( wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)&msp );
		if ( iRet ) {
			tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: can't get track %d len\n", i));
			DMCIError( iRet );
			trackLenBuf[i] = 0;
			continue;
		}
		trackLenBuf[i] = msp.dwReturn;
#elif defined(USE_SDL)
		uint64 ui = cdrom->track[i].length * 1000 / CD_FPS;
		trackLenBuf[i] = ui;
		totalLen_ms += cdrom->track[i].length * 1000;
#endif
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: track %d len %d\n", i, trackLenBuf[i]));
	}
#if defined(USE_SDL)
	totalLen_ms = totalLen_ms / CD_FPS;
#elif defined(WIN32)
	msp.dwItem = MCI_STATUS_LENGTH;
	iRet = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&msp);
	if( iRet ) {
		tracklen_DPRINT((tracklen_buf, "GetTrackLengths: Can't get disc len.\n" ));
		DMCIError( iRet );
		return __LINE__;
	}
	totalLen_ms = msp.dwReturn;
#endif
	tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: got disc len %d\n", totalLen_ms ));

	if (totalLen_ms < ((74 * 60 + 30) * 1000)) {
		tracklen_DPRINT((tracklen_buf, "GetTrackLengths: disc is too short\n"));
		return __LINE__;
	}
	if (totalLen_ms > ((80 * 60 + 30) * 1000)) {
		tracklen_DPRINT((tracklen_buf, "GetTrackLengths: disc is too long\n"));
		return __LINE__;
	}
	
	return 0;
}

int tracklen_GetTrackLengths(DWORD *trackLenBuf, int iDrive)
{
	int iRet = __LINE__;
#if defined(USE_SDL)
#elif defined(WIN32)
	char whichDrive = static_cast<char>(iDrive);
	MCI_OPEN_PARMS mop;
	char szDrive[16];

#ifdef tracklen_LOGGING
	if (!tracklen_fp) tracklen_fp = fopen(tracklen_LOGFILE, "w");
#endif

	memset(&mop, 0, sizeof(mop));
	mop.lpstrDeviceType = (LPCSTR)(MCI_DEVTYPE_CD_AUDIO);
	mop.lpstrAlias = NULL;
	szDrive[0] = whichDrive;
	szDrive[1] = ':';
	szDrive[2] = 0;
	mop.lpstrElementName = szDrive;
	mop.dwCallback = NULL;
	mop.wDeviceID = 0;
	if( iRet = mciSendCommand( 0, MCI_OPEN, MCI_WAIT | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD)&mop ) )
	{
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: failed to open MCI device.\n" ));
		DMCIError( iRet );
		return __LINE__;
	}
	tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: Opened MCI device\n" ));
	
	iRet = tracklen_GetTrackLengthsViaHandle( trackLenBuf, mop.wDeviceID);
	
	mciSendCommand( mop.wDeviceID, MCI_CLOSE, 0, NULL );

	tracklen_DPRINT((tracklen_buf, "GetTrackLengths: Returning %d\n", iRet ));
#else
	return __LINE__;
#endif
	return iRet;
}

static int tracklen_CheckTrackLengths2( DWORD *trackLenBuf)
{
#if defined(WIN32)
	if( !trackLenBuf )
		return __LINE__;

	tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: begin\n" ));

	
	int i;
	HWND hCD;
	for (i=0; (hCD=FindWindow( "SJE_CdPlayerClass",NULL)) && i<100; i++) {
		tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: Shutting down CD player\n" ));
		SendMessage( hCD, WM_CLOSE, 0, 0 );
	}

	
	int iDrive;
	for (iDrive=0; iDrive<26; iDrive++) {
		char szDrive[ MAX_PATH ];
		int iDriveType;
		int iRet;
		
		szDrive[0] = iDrive + 'A';
		szDrive[1] = ':';
		szDrive[2] = '\\';
		szDrive[3] = 0;
		iDriveType = GetDriveType( szDrive );
		if( iDriveType != DRIVE_CDROM ) {
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: %s type %d, not cd.\n", szDrive, iDriveType ));
			continue;
		}
		tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: device %s is cd.\n", szDrive ));

		
		DWORD newbuf[tracklen_MAXTRACKS];
		iRet = tracklen_GetTrackLengths(newbuf, szDrive[0]);
		if (iRet) {
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: drive %s fails check %d\n", szDrive, iRet ));
			continue;
		}
		if (newbuf[0] != trackLenBuf[0]) {
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: drive %s has %d tracks, wanted %d\n", szDrive, newbuf[0], trackLenBuf[0] ));
			continue;
		}

		for (i=2; i< (int)(newbuf[0]); i++) {
			if (newbuf[i] != trackLenBuf[i]) {
				tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: drive %s track %d has len %d, wanted %d\n", szDrive, i, newbuf[i], trackLenBuf[i]));
				break;
			}
		}
		
		if (i == ((int) newbuf[0])) {
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: success!\n"));
			return 0;
		}
	}

	tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: failure!\n"));
#endif
	return __LINE__;
}

DWORD *tracklen_LoadEncryptedKey( DWORD *trackLenBuf, const char *szFile )
{
#if defined(WIN32)
	char szTemp[ MAX_PATH ];
	if( !strchr(szFile, ':' ) && strncmp( szFile, FILE_SEP FILE_SEP, 2 ) )
	{
		GetModuleFileName( NULL, szTemp, MAX_PATH );
		*(char*)strrchr(szTemp, FILE_SEPC) = 0;
		strcat( szTemp, FILE_SEP );
		strcat( szTemp, szFile );
	}
	else
		strcpy( szTemp, szFile );

	HANDLE hFile = CreateFile( szTemp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hFile == INVALID_HANDLE_VALUE ) {
		tracklen_DPRINT((tracklen_buf, "tracklen_LoadEncryptedKey: can't open %s\n", szTemp));
		return NULL;
	}

	DWORD dwSize = GetFileSize( hFile, NULL );
	if ((dwSize < 4) || (dwSize % 4)) {
		tracklen_DPRINT((tracklen_buf, "tracklen_LoadEncryptedKey: %s bad size\n", szTemp));
		CloseHandle( hFile );
		return NULL;
	}


	DWORD ignore;
	ReadFile( hFile, trackLenBuf, dwSize, &ignore, NULL );
	CloseHandle( hFile );
	if( ignore < dwSize ) {
		tracklen_DPRINT((tracklen_buf, "tracklen_LoadEncryptedKey: file %s bad size 2\n", szTemp));
		return NULL;
	}

	*trackLenBuf = dwSize/4;

	tracklen_cryptBinary((char *)(trackLenBuf+1), dwSize-4);

	tracklen_DPRINT((tracklen_buf, "tracklen_LoadEncryptedKey: success\n"));
	return trackLenBuf;
#else
	return NULL;
#endif
}






char *GetVersionInfo( DWORD *trackLenBuffer )
{
	
	char *szVersionPtr = (char*)&trackLenBuffer[1];
	if( szVersionPtr[0] != 'V' || szVersionPtr[3] != '.' )
	{
#ifdef tracklen_LOGGING
		tracklen_DPRINT((tracklen_buf, "GetVersionInfo: outdated tracklen file: does not contain version information. Version information has been implemented as of 08/21/00\n"));
#endif	
		return( NULL );
	}

	return( szVersionPtr );
}

BYTE tracklen_CheckTrackLengths( char *szVersionInfoBuffer )
{
	if( szVersionInfoBuffer )
		*szVersionInfoBuffer = 0;

	DWORD trackLenBuf[ DWVERSIONINFOLEN + tracklen_MAXTRACKS ];

#ifdef tracklen_LOGGING
	if (!tracklen_fp) tracklen_fp = fopen(tracklen_LOGFILE, "w");
#endif

	if (!tracklen_LoadEncryptedKey( trackLenBuf, tracklen_DATFILE))
		return( _TRACKLEN_LOADFAILED );

	BYTE byRet = _TRACKLEN_OK;

	
	char *szVersionPtr = GetVersionInfo( trackLenBuf );
#ifdef tracklen_LOGGING
	if( !szVersionPtr )
	{
                tracklen_DPRINT((tracklen_buf, "tracklen_CheckTrackLengths: Warning: tracklen file does not contain any version information and was most likely created using a previous version of this SDK\n"));
		byRet = _TRACKLEN_VERSIONMISMATCH;
	}
#endif	

	if( szVersionPtr && szVersionInfoBuffer )
		strncpy( szVersionInfoBuffer, szVersionPtr, CHRVERSIONINFOLEN );

#ifdef tracklen_LOGGING
	
	if( szVersionPtr )
	{
		if( strcmp( szVersionPtr, VERSIONINFO ) )
		{
			tracklen_DPRINT((tracklen_buf, "tracklen_CheckTrackLengths: Warning: tracklen file's version information does not match the version info of this SDK. tracklen file's version info: %s - expected version info: %s\n", szVersionPtr, VERSIONINFO ));
			byRet = _TRACKLEN_VERSIONMISMATCH;
		}
		else
		{
			tracklen_DPRINT((tracklen_buf, "tracklen_CheckTrackLengths: Verified version info: %s\n", szVersionPtr ));
		}
	}
#endif	

	if( szVersionPtr )
	{
		trackLenBuf[ DWVERSIONINFOLEN ] = trackLenBuf[0] - (DWORD)(DWVERSIONINFOLEN);
		if( tracklen_CheckTrackLengths2( trackLenBuf + DWVERSIONINFOLEN ) )
			return( _TRACKLEN_CHECKFAILED );
		
		return( byRet );
	}
	else if( tracklen_CheckTrackLengths2( trackLenBuf ) )
		return( _TRACKLEN_CHECKFAILED );
	
	return( byRet );
}


#define AUTORUN_UNKNOWN 0		
#define AUTORUN_DEFAULT 0x95
#define AUTORUN_DISABLE 0xff


static ULONG s_oldAutoRunValue = AUTORUN_UNKNOWN;


void tracklen_AutoPlay_Disable()
{
#if defined(WIN32)
	int res;
	HKEY hkey;

	hkey = NULL;
	res = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", 0, KEY_ALL_ACCESS, &hkey);
	if (res == ERROR_SUCCESS) {	
		if (s_oldAutoRunValue == AUTORUN_UNKNOWN) {
			unsigned long ulDataSize = sizeof(s_oldAutoRunValue);
			res = RegQueryValueEx(hkey, "NoDriveTypeAutoRun", 0, NULL, (unsigned char*)&s_oldAutoRunValue, &ulDataSize);
		}
		
		if (res == ERROR_SUCCESS) {
			unsigned long ulDisableAutoRun = AUTORUN_DISABLE;
			RegSetValueEx(hkey, "NoDriveTypeAutoRun", 0, REG_BINARY, (const unsigned char*)&ulDisableAutoRun, 4);
		}
		RegFlushKey(hkey);
		RegCloseKey(hkey);
	 }
#endif
}

void tracklen_AutoPlay_Restore()
{
#if defined(WIN32)
	int res;
	HKEY hkey = NULL;

	
	if (s_oldAutoRunValue == AUTORUN_UNKNOWN) {
		return;
	}

	res = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", 0, KEY_ALL_ACCESS, &hkey);
	if (res == ERROR_SUCCESS) {	
		RegSetValueEx(hkey, "NoDriveTypeAutoRun", 0, REG_BINARY, (const unsigned char*)&s_oldAutoRunValue, 4);
		RegFlushKey(hkey);
		RegCloseKey(hkey);
	 }
#endif
}
