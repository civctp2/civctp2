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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
// WIN32
// - Generates windows specific code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added some casts. (Aug 7th 2005 Martin Gühmann)
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "CivPaths.h"
#include "c3files.h"
#include "c3errors.h"
#include "pointerlist.h"
#include "profileDB.h"


#include "c3ui.h"

#include "tracklen.h"

#include "appstrings.h"

#include "civ3_main.h"

#include <sys/stat.h>
#include <sys/types.h>

extern ProfileDB *g_theProfileDB;

extern CivPaths *g_civPaths;



FILE* c3files_fopen(C3DIR dirID, MBCHAR *s1, MBCHAR *s2)
{
	MBCHAR      s[_MAX_PATH];

	if (g_civPaths->FindFile(dirID, s1, s) != NULL) {
		return fopen(s, s2);
	} else {
		return NULL;
	}
}

FILE* c3files_freopen(const MBCHAR *s1, const MBCHAR *s2, FILE *file)
{
	return freopen(s1, s2, file);
}


sint32 c3files_fclose(FILE *file)
{
	return (sint32)fclose(file);
}

sint32 c3files_fscanf(FILE *file, const MBCHAR *s, ...)
{
	va_list		valist;
	sint32		val;

	va_start(valist, s);
	val = (sint32)fscanf(file, s, valist);
	va_end(valist);
	
	return val;
}

size_t c3files_fread(void *p, size_t i1, size_t i2, FILE *file)
{
	return fread(p, i1, i2, file);
}

sint32 c3files_fgetc(FILE *file)
{
	return (sint32)fgetc(file);
}

sint32 c3files_fgetpos(FILE *file, fpos_t *pos)
{
	return (sint32)fgetpos(file, pos);
}

MBCHAR* c3files_fgets(MBCHAR *s, sint32 i, FILE *file)
{
	return (MBCHAR *)fgets(s, i, file);
}



size_t c3files_fwrite(const void *p, size_t i1, size_t i2, FILE *file)
{
	return fwrite(p, i1, i2, file);
}



sint32 c3files_fprintf(FILE *file, const MBCHAR *s, ...)
{
	va_list     valist;
	sint32      val;

	va_start(valist, s);
	val = (sint32)vfprintf(file, s, valist);
	va_end(valist);
	
	return val;
}



sint32 c3files_fputc(sint32 i, FILE *file)
{
	return (sint32)fputc(i, file);
}



sint32 c3files_fputs(const MBCHAR *s, FILE *file)
{
	return (sint32)fputs(s, file);
}



sint32 c3files_fsetpos(FILE *file, const fpos_t *pos)
{
	return (sint32)fsetpos(file, pos);
}



sint32 c3files_fseek(FILE *file, sint32 i1, sint32 i2)
{
	return (sint32)fseek(file, i1, i2);
}



sint32 c3files_ftell(FILE *file)
{
	return (sint32)ftell(file);
}



sint32 c3files_feof(FILE *file)
{
	return (sint32)feof(file);
}



sint32 c3files_ferror(FILE *file)
{
	return (sint32)ferror(file);
}



void c3files_clearerr(FILE *file)
{
	clearerr(file);
}



sint32 c3files_fflush(FILE *file)
{
	return (sint32)fflush(file);
}



sint32 c3files_getfilesize(C3DIR dir, MBCHAR *filename)
{
	sint32 filesize = 0;

	FILE *f = c3files_fopen(dir, filename, "rb");

	if ( !f )
		return -1;
	
	if (c3files_fseek(f, 0, SEEK_END) == 0) {
		filesize = c3files_ftell(f);
	} else {
		c3files_fclose(f);
		return -1;
	}

	c3files_fclose(f);

	return filesize;
}



uint8 *c3files_loadbinaryfile(C3DIR dir, MBCHAR *filename, sint32 *size)
{
	if ( size ) *size = 0;

	uint32 filesize;
	uint8 *bits;

	FILE *f = c3files_fopen(dir, filename, "rb");

	if ( !f )
		return NULL;

	if (c3files_fseek(f, 0, SEEK_END) == 0) {
		filesize = c3files_ftell(f);
	} else {
		fclose(f);
		return NULL;
	}

	if (c3files_fseek(f, 0, SEEK_SET) != 0) {
		fclose(f);
		return NULL;
	}

	bits = new uint8[filesize];

	Assert(bits != NULL);
	if (!bits) {
		c3files_fclose(f);
		return NULL;
	}

	if (c3files_fread( bits, 1, filesize, f ) != filesize) {
#if defined(_MSC_VER)
		delete[filesize] bits;
#else
		delete[] bits;
#endif
		c3files_fclose(f);

		return NULL;
	}

	c3files_fclose(f);

	if ( size ) *size = filesize;

	return bits;
}

BOOL c3files_PathIsValid(MBCHAR *path)
{
#if defined(WIN32)
	struct _stat tmpstat;
#else
	struct stat  tmpstat;
#endif
	sint32       r;

#if defined(WIN32)
   r = _stat(path, &tmpstat);
#else
   r = stat(path, &tmpstat);
#endif

	if (!r) return TRUE;
	else return FALSE;
}

BOOL c3files_CreateDirectory(MBCHAR *path)
{
#if defined(WIN32)
	return CreateDirectory(path, NULL);
#else
	mode_t mask = 0777;
	return mkdir(path, mask);
#endif
}

void c3files_StripSpaces(MBCHAR *s)
{
	sint32      i;
	sint32      j;
	sint32      len = strlen(s);

	for (i=len-1; i>=0; i--) {
		if (s[i] == ' ') {
			for (j=i; j<len; j++) {
				s[j] = s[j+1];
			}
		}
	}

	if (strlen(s) <= 0) {
		strcpy(s, "-");
	}
}




sint32 c3files_getfilelist(C3SAVEDIR dirID, MBCHAR *ext, PointerList<MBCHAR> *list)
{
	MBCHAR strbuf[256];
	MBCHAR *lpFileName = NULL;
	MBCHAR path[_MAX_PATH];

	WIN32_FIND_DATA	fileData;
	HANDLE lpFileList;

	g_civPaths->GetSavePath(dirID, path);
	
	if (ext) sprintf(strbuf,"*.%s",ext);
	else strcpy(strbuf, "*.*");
		
	strcat(path,strbuf);

	
	lpFileList = FindFirstFile(path,&fileData);
	
	if (lpFileList ==  INVALID_HANDLE_VALUE) return FALSE;
	
	lpFileName = new MBCHAR[256];
	strcpy(lpFileName,fileData.cFileName);
	list->AddTail(lpFileName);
	
	while(FindNextFile(lpFileList,&fileData))
	{
		lpFileName = new MBCHAR[256];
		strcpy(lpFileName,fileData.cFileName);
		list->AddTail(lpFileName);
	}

	FindClose(lpFileList);

	return TRUE;
}



sint32 c3files_getfilelist_ex(C3SAVEDIR dirID, MBCHAR *ext, PointerList<WIN32_FIND_DATA> *list)
{
	MBCHAR strbuf[256];
	MBCHAR path[_MAX_PATH];

	WIN32_FIND_DATA *lpFileData;
	HANDLE lpFileList;

	
	g_civPaths->GetSavePath(dirID, path);

	
	if (ext) sprintf(strbuf,"*.%s",ext);
	else strcpy(strbuf, "*.*");
		
	strcat(path,strbuf);

	
	lpFileData = new WIN32_FIND_DATA;
	lpFileList = FindFirstFile(path,lpFileData);

	
	if (lpFileList ==  INVALID_HANDLE_VALUE) return FALSE;

	
	list->AddTail(lpFileData);
	
	lpFileData = new WIN32_FIND_DATA;
	while(FindNextFile(lpFileList,lpFileData))
	{
		list->AddTail(lpFileData);
		lpFileData = new WIN32_FIND_DATA;
	}

	FindClose(lpFileList);

	return TRUE;
}



#define k_CTP_CD_VOLUME_NAME		"CTP2"

MBCHAR  VolumeName[32];
MBCHAR  WhichCD;
MBCHAR  IsCD[26];
sint32  CDIndex = 0;
BOOL    g_hasCD = FALSE;

#include "soundmanager.h"
extern SoundManager		*g_soundManager;

extern C3UI *g_c3ui;

BOOL c3files_HasLegalCD()
{
	BOOL success = FALSE;

	if (g_soundManager) {
		g_soundManager->CleanupRedbook();
	}

	while (!success) {

		success = c3files_FindCDByName(k_CTP_CD_VOLUME_NAME, TRUE);

		
		if (success && g_theProfileDB->IsProtected()) {
			BOOL        valid;

			valid = tracklen_CheckTrackLengths();
			

			if (!valid) 
				success = TRUE;
			else
				success = FALSE;
			
			
			
		}

		if (!success) {
			int rval = MessageBox(g_c3ui->TheHWND(),
			                      appstrings_GetString(APPSTR_INSERTCDROM),
			                      appstrings_GetString(APPSTR_CDROM),
			                      MB_RETRYCANCEL |
			                      MB_ICONEXCLAMATION |
			                      MB_DEFBUTTON1 |
			                      MB_SYSTEMMODAL |
			                      MB_SETFOREGROUND);

			if (rval == IDCANCEL) {
				c3errors_ErrorDialog(appstrings_GetString(APPSTR_CDROM),
				                     appstrings_GetString(APPSTR_NEEDCDROM));
				
				exit(-1);
			}

			
			
			if (g_c3ui) {
				g_c3ui->AltTabIn();
			}
		}
	}

	if (g_soundManager) {
		g_soundManager->InitRedbook();
	}

	return success;
}

void c3files_InitializeCD(void)
{
	c3files_GetCDDrives();
	c3files_FindCDByName(k_CTP_CD_VOLUME_NAME, TRUE);
}

MBCHAR c3files_GetCTPCDDriveLetter(void)
{
	return WhichCD;
}

BOOL c3files_HasCD(void)
{
	return g_hasCD;
}


void c3files_GetCDDrives(void)
{
	uint32      all_drives;
	MBCHAR      i;
	MBCHAR      drivepath[16];
	
	WhichCD = '\0';

	strcpy(drivepath, " :\\");
	all_drives = GetLogicalDrives();

	for (i = 0; i < 26; i++)
	{
		if (all_drives & (1 << i))
		{
			drivepath[0] = i + 'A';
			if (GetDriveType(drivepath) == DRIVE_CDROM)
				IsCD[i] = 1;
		}
	}
}

MBCHAR *c3files_GetVolumeName(MBCHAR name)
{
	MBCHAR drivepath[32];
	MBCHAR FSName[32];
	uint32 SerialNumber;
	uint32 MaxComponentLen;
	uint32 FSFlags;
	
	strcpy(drivepath, " :\\");
	drivepath[0] = name;
	if (GetVolumeInformation(drivepath, VolumeName, 32, &SerialNumber,
	                         &MaxComponentLen, &FSFlags, FSName, 32)) {
		return(VolumeName);
	}
	return(NULL);
}

BOOL c3files_FindCDByName(CHAR *name, BOOL findDriveLetter)
{
	sint32  i;
	BOOL    found = FALSE;
	MBCHAR  *cdName;

	CDIndex = 0;

	if (findDriveLetter)
		WhichCD = '\0';

	if (!g_theProfileDB->IsRequireCD()) {
		for (i=0; i<26; i++) {
			if (IsCD[i]) {
				if (findDriveLetter)
					WhichCD = static_cast<MBCHAR>(i + 'A');
				g_hasCD = TRUE;
			}
		}
		return TRUE;
	} else {
		for (i = 0; (i < 26) && !found; i++)
			if (IsCD[i])
			{
				CDIndex++;
				cdName = c3files_GetVolumeName((MBCHAR)(i + 'A'));
				if (cdName && !strnicmp(cdName, name, strlen(name)))
				{
					if (findDriveLetter)
						WhichCD = static_cast<MBCHAR>(i + 'A');

					found = TRUE;
					g_hasCD = TRUE;
				}
			}

		if (!found && findDriveLetter)
			found = (WhichCD != '\0');

		return (found);
	}
}
