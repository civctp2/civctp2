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

#if !defined(USE_SDL)

#include <stdio.h>

#include "windows.h"
#include "mbstring.h"
#include "mmsystem.h"

#if defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif
#include "cheatkey.h"

#include "tracklen.h"

#if defined(tracklen_LOGGING)

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
	MCI_STATUS_PARMS msp;
	MCI_SET_PARMS mp;
	long totalLen_ms = 0;
	DWORD i;

	memset(&mp, 0, sizeof(mp));
	mp.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
	if (mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD_PTR)&mp))
	{
		tracklen_DPRINT((tracklen_buf, "GetTrackLengths: failed to set time format.\n" ));
		return __LINE__;
	}
	tracklen_DPRINT((tracklen_buf, "GetTrackLengths: set time format to milliseconds.\n"));

	memset(&msp, 0, sizeof(msp));
	msp.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if (mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&msp))
	{
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: failed to read # of tracks\n"));
		DMCIError( iRet );
		return __LINE__;
	}
	trackLenBuf[0] = msp.dwReturn;
	tracklen_DPRINT((tracklen_buf, "GetTrackLengths: %d tracks\n", trackLenBuf[0]));
	if (trackLenBuf[0] > tracklen_MAXTRACKS) {
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: too many tracks\n"));
		return __LINE__;
	}

	for (i = 1; i <= trackLenBuf[0]; i++) {
		memset(&msp, 0, sizeof(msp));
		msp.dwItem = MCI_STATUS_LENGTH;
		msp.dwTrack = i;
		if (mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR)&msp))
		{
			tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: can't get track %d len\n", i));
			DMCIError( iRet );
			trackLenBuf[i] = 0;
			continue;
		}
		trackLenBuf[i] = msp.dwReturn;
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: track %d len %d\n", i, trackLenBuf[i]));
	}
	msp.dwItem = MCI_STATUS_LENGTH;
	if(mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&msp))
	{
		tracklen_DPRINT((tracklen_buf, "GetTrackLengths: Can't get disc len.\n" ));
		DMCIError( iRet );
		return __LINE__;
	}
	totalLen_ms = msp.dwReturn;
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

int tracklen_GetTrackLengths(DWORD *trackLenBuf, char whichDrive)
{
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

	int iRet = mciSendCommand(0, MCI_OPEN, MCI_WAIT | MCI_OPEN_ELEMENT | MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID | MCI_OPEN_SHAREABLE, (DWORD_PTR)&mop);
	if (iRet)
	{
		tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: failed to open cdrom device.\n" ));
		DMCIError( iRet );
		return __LINE__;
	}
	tracklen_DPRINT((tracklen_buf,  "GetTrackLengths: Opened cdrom device\n" ));

	iRet = tracklen_GetTrackLengthsViaHandle( trackLenBuf, mop.wDeviceID);

	mciSendCommand( mop.wDeviceID, MCI_CLOSE, 0, NULL );

	tracklen_DPRINT((tracklen_buf, "GetTrackLengths: Returning %d\n", iRet ));
	return iRet;
}

static int tracklen_CheckTrackLengths2( DWORD *trackLenBuf)
{
	if( !trackLenBuf )
		return __LINE__;

	tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: begin\n" ));

	int i = 0;
	HWND hCD;
	for (i = 0; (hCD=FindWindow( "SJE_CdPlayerClass",NULL)) && i < 100; i++)
	{
		tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: Shutting down CD player\n" ));
		SendMessage( hCD, WM_CLOSE, 0, 0 );
	}

	int iDrive;
	for (iDrive = 0; iDrive < 26; iDrive++) {
		char szDrive[ MAX_PATH ];
		int iDriveType;

		szDrive[0] = static_cast<char>(iDrive + 'A');
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
		int iRet = tracklen_GetTrackLengths(newbuf, szDrive[0]);
		if (iRet)
		{
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: drive %s fails check %d\n", szDrive, iRet ));
			continue;
		}
		if (newbuf[0] != trackLenBuf[0])
		{
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: drive %s has %d tracks, wanted %d\n", szDrive, newbuf[0], trackLenBuf[0] ));
			continue;
		}

		for (i = 2; i < (int)(newbuf[0]); i++)
		{
			if (newbuf[i] != trackLenBuf[i])
			{
				tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: drive %s track %d has len %d, wanted %d\n", szDrive, i, newbuf[i], trackLenBuf[i]));
				break;
			}
		}

		if (i == ((int) newbuf[0]))
		{
			tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: success!\n"));
			return 0;
		}
	}

	tracklen_DPRINT((tracklen_buf,  "CheckTrackLengths2: failure!\n"));
	return __LINE__;
}

DWORD *tracklen_LoadEncryptedKey( DWORD *trackLenBuf, const char *szFile )
{
	char szTemp[MAX_PATH] = {0};
	if( !strchr(szFile, ':' ) && strncmp( szFile, FILE_SEP FILE_SEP, 2 ) )
	{
		GetModuleFileName( NULL, szTemp, MAX_PATH );
		char *pos = (char*)strrchr(szTemp, FILE_SEPC);
		if (pos) {
			*pos = '\0';
			strcat(szTemp, FILE_SEP);
			strcat(szTemp, szFile);
		} else {
			strcpy(szTemp, szFile);
		}
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
}

void tracklen_AutoPlay_Restore()
{
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
}

#endif // USE_SDL
