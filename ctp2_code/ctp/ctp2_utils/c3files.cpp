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
// - Improved CTP2 disk detection.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3files.h"

#include "appstrings.h"
#include "civ3_main.h"
#include "CivPaths.h"
#include "c3errors.h"
#include "c3ui.h"
#include "pointerlist.h"
#include "profileDB.h"
#include "soundmanager.h"
#include "tracklen.h"
#include <sys/stat.h>
#include <sys/types.h>

extern C3UI *       g_c3ui;

namespace
{
    DriveIdType const   DRIVE_FIRST         = 'A';
    DriveIdType const   DRIVE_LAST          = 'Z';
    DriveIdType const   DRIVE_UNDETERMINED  = '\0';
    size_t const        DRIVE_COUNT         = 1 + (DRIVE_LAST - DRIVE_FIRST);
    
    size_t const        VOLUME_NAME_SIZE    = 32;

    bool                g_hasCD             = false;
    bool                IsCD[DRIVE_COUNT];
    MBCHAR              VolumeName[VOLUME_NAME_SIZE];
    DriveIdType         WhichCD             = DRIVE_UNDETERMINED;

    void		        c3files_GetCDDrives(void);
    MBCHAR const *      c3files_GetVolumeName(DriveIdType id);
    bool                c3files_FindCDByName(MBCHAR const * name);
}

#define k_CTP_CD_VOLUME_NAME		"CTP2"

FILE* c3files_fopen(C3DIR dirID, MBCHAR const * s1, MBCHAR const * s2)
{
	MBCHAR  s[_MAX_PATH];

	return g_civPaths->FindFile(dirID, s1, s) ? fopen(s, s2) : NULL;
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



sint32 c3files_getfilesize(C3DIR dir, MBCHAR const *filename)
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



uint8 *c3files_loadbinaryfile(C3DIR dir, MBCHAR const * filename, sint32 *size)
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
		delete [] bits;
		c3files_fclose(f);
		return NULL;
	}

	c3files_fclose(f);

	if ( size ) *size = filesize;

	return bits;
}

bool c3files_PathIsValid(MBCHAR *path)
{
#if defined(WIN32)
	struct _stat tmpstat;
	return !_stat(path, &tmpstat);
#else
	struct stat  tmpstat;
	return !stat(path, &tmpstat);
#endif
}

bool c3files_CreateDirectory(MBCHAR *path)
{
#if defined(WIN32)
	return CreateDirectory(path, NULL) != FALSE; // BOOL to bool conversion
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




bool c3files_getfilelist(C3SAVEDIR dirID, MBCHAR *ext, PointerList<MBCHAR> *list)
{
	MBCHAR strbuf[256];
	MBCHAR path[_MAX_PATH];

	g_civPaths->GetSavePath(dirID, path);
	
	if (ext) sprintf(strbuf,"*.%s",ext);
	else strcpy(strbuf, "*.*");
		
	strcat(path,strbuf);

	WIN32_FIND_DATA	fileData;
	HANDLE          lpFileList = FindFirstFile(path, &fileData);
	
	if (lpFileList ==  INVALID_HANDLE_VALUE) return false;
	
	MBCHAR *lpFileName = new MBCHAR[256];
	strcpy(lpFileName,fileData.cFileName);
	list->AddTail(lpFileName);
	
	while (FindNextFile(lpFileList,&fileData))
	{
		lpFileName = new MBCHAR[256];
		strcpy(lpFileName,fileData.cFileName);
		list->AddTail(lpFileName);
	}

	FindClose(lpFileList);

	return true;
}



bool c3files_getfilelist_ex(C3SAVEDIR dirID, MBCHAR *ext, PointerList<WIN32_FIND_DATA> *list)
{
	MBCHAR strbuf[256];
	MBCHAR path[_MAX_PATH];
	
	g_civPaths->GetSavePath(dirID, path);
	
	if (ext) sprintf(strbuf,"*.%s",ext);
	else strcpy(strbuf, "*.*");
		
	strcat(path,strbuf);

	
	WIN32_FIND_DATA *   lpFileData  = new WIN32_FIND_DATA;
	HANDLE              lpFileList  = FindFirstFile(path, lpFileData);

	if (lpFileList == INVALID_HANDLE_VALUE) 
    {
        delete lpFileData;
        return false;
    }
    
	list->AddTail(lpFileData);

	lpFileData = new WIN32_FIND_DATA;
	while (FindNextFile(lpFileList,lpFileData))
	{
		list->AddTail(lpFileData);
		lpFileData = new WIN32_FIND_DATA;
	}
    delete lpFileData;

	FindClose(lpFileList);

	return true;
}

bool c3files_HasLegalCD()
{
	bool success = false;

	if (g_soundManager) 
    {
		g_soundManager->CleanupRedbook();
	}

	while (!success) 
    {
		success = c3files_FindCDByName(k_CTP_CD_VOLUME_NAME);

		if (success && g_theProfileDB->IsProtected()) 
        {
			success = (_TRACKLEN_OK == tracklen_CheckTrackLengths());
		}

		if (!success) 
        {
			int const rval = MessageBox
                                (g_c3ui ? g_c3ui->TheHWND() : NULL,
			                     appstrings_GetString(APPSTR_INSERTCDROM),
			                     appstrings_GetString(APPSTR_CDROM),
			                     MB_RETRYCANCEL         | 
                                    MB_ICONEXCLAMATION  | 
                                    MB_DEFBUTTON1       | 
                                    MB_SYSTEMMODAL      | 
                                    MB_SETFOREGROUND
                                );

			if (IDCANCEL == rval) 
            {
                // Do not annoy the user with more messages after cancelling
				exit(-1);
			}

            if (g_c3ui)
            {
			    g_c3ui->AltTabIn();
            }
		}
	}

	if (g_soundManager) 
    {
		g_soundManager->InitRedbook();
	}

	return success;
}

void c3files_InitializeCD(void)
{
	c3files_GetCDDrives();
	(void) c3files_FindCDByName(k_CTP_CD_VOLUME_NAME);
}

bool c3files_HasCD(void)
{
	return g_hasCD;
}

DriveIdType c3files_GetCtpCdId(void)
{
    return WhichCD;
}

namespace
{

//----------------------------------------------------------------------------
//
// Name       : c3files_GetCDDrives
//
// Description: Determine the CD drives of the PC
//
// Parameters : -
//
// Globals    : IsCD    : filled
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void c3files_GetCDDrives(void)
{
	MBCHAR          drivepath[4];   // letter + : + dir seperator + zero
	strcpy(drivepath, " :\\");

	uint32 const    all_drives = GetLogicalDrives();

	for (uint32 i = 0; i < DRIVE_COUNT; ++i)
	{
		if (all_drives & (1 << i))
		{
			drivepath[0]    = static_cast<MBCHAR>(i + DRIVE_FIRST);
			IsCD[i]         = (DRIVE_CDROM == GetDriveType(drivepath));
		}
        else
        {
            IsCD[i]         = false;
        }
	}
}

//----------------------------------------------------------------------------
//
// Name       : c3files_GetVolumeName
//
// Description: Determine the name of a CD in a drive
//
// Parameters : id              : the drive id
//
// Globals    : VolumeName      : filled when found
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
MBCHAR const * c3files_GetVolumeName(DriveIdType id)
{
	MBCHAR          drivepath[4];   // letter + : + dir seperator + zero
	strcpy(drivepath, " :\\");
	drivepath[0] = id;

	MBCHAR  FSName[32];
	DWORD   SerialNumber;
	DWORD   MaxComponentLen;
	DWORD   FSFlags;

	if (GetVolumeInformation
            (drivepath, 
             VolumeName, 
             VOLUME_NAME_SIZE, 
             &SerialNumber, 
             &MaxComponentLen, 
             &FSFlags, 
             FSName, sizeof(FSName)
            )
       ) 
    {
		return VolumeName;
	}

	return NULL;
}

//----------------------------------------------------------------------------
//
// Name       : c3files_FindCDByName
//
// Description: Determine whether a named disk is in some CD drive.
//
// Parameters : name            : the name of the disk to look for
//
// Globals    : g_theProfileDB  : CD is required 
//              g_hasCD         : updated
//              WhichCD         : updated
//
// Returns    : bool            : CD has been found (or is not required yet)
//
// Remark(s)  : Assumption      : name is non-NULL, name is zero-terminated
//
//----------------------------------------------------------------------------
bool c3files_FindCDByName(MBCHAR const * name)
{
	WhichCD = DRIVE_UNDETERMINED;
	g_hasCD = false;
      Assert(name);

	for (size_t i = 0; (i < DRIVE_COUNT) && !g_hasCD; ++i)
    {
		if (IsCD[i])
		{
			MBCHAR const *  cdName  = c3files_GetVolumeName
                                        (static_cast<MBCHAR>(i + DRIVE_FIRST));

			if (cdName)
            {
                size_t const checkLength = strlen(cdName);

                if ((checkLength == strlen(name)) &&
                    !strnicmp(cdName, name, checkLength)
                   )
			    {
				    WhichCD = static_cast<MBCHAR>(i + DRIVE_FIRST);
				    g_hasCD = true;
                }
			}
		}
    }

	return g_hasCD || !g_theProfileDB->IsRequireCD();
}

} // local namespace