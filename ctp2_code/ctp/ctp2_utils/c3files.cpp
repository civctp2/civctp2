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
// - added linux specific code
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

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifndef WIN32
#include <dirent.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef LINUX
#include <features.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
// MS_* dupes with sys/mount.h
#include <linux/fs.h>
#include <linux/iso_fs.h>
#include <errno.h>
#include <libgen.h>
#include <mntent.h>
#include <paths.h>
#include <pwd.h>
#endif

extern ProfileDB *g_theProfileDB;

extern CivPaths *g_civPaths;



FILE* c3files_fopen(C3DIR dirID, const MBCHAR *s1, const MBCHAR *s2)
{
	MBCHAR s[_MAX_PATH] = { 0 };

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



sint32 c3files_getfilesize(C3DIR dir, const MBCHAR *filename)
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



uint8 *c3files_loadbinaryfile(C3DIR dir, const MBCHAR *filename, sint32 *size)
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
		c3files_fclose(f);
		return NULL;
	}

	if (c3files_fseek(f, 0, SEEK_SET) != 0) {
		c3files_fclose(f);
		return NULL;
	}

	bits = new uint8[filesize];

	Assert(bits != NULL);
	if (!bits) {
		c3files_fclose(f);
		return NULL;
	}

	if (c3files_fread( bits, 1, filesize, f ) != filesize) {
		delete[] bits;
		
		c3files_fclose(f);

		return NULL;
	}

	c3files_fclose(f);

	if ( size ) *size = filesize;

	return bits;
}

BOOL c3files_PathIsValid(const MBCHAR *path)
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

BOOL c3files_CreateDirectory(const MBCHAR *path)
{
#if defined(WIN32)
	return CreateDirectory((const LPCSTR *) path, NULL);
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




sint32 c3files_getfilelist(C3SAVEDIR dirID, const MBCHAR *ext, PointerList<MBCHAR> *list)
{
#ifdef WIN32
	MBCHAR strbuf[256];
#endif
	MBCHAR *lpFileName = NULL;
	MBCHAR path[_MAX_PATH];

#ifdef WIN32
	WIN32_FIND_DATA	fileData;
	HANDLE lpFileList;
#endif

	g_civPaths->GetSavePath(dirID, path);

#ifdef WIN32
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
#else
        DIR *dir = opendir(path);
	if (!dir)
		return FALSE;
	struct dirent *dent = NULL;
	
	while ((dent = readdir(dir)))
	{
                char *p = strrchr(dent->d_name, '.');
		if (NULL == p) {
			continue;
		}
		if (1 == strlen(p)) {
			continue;
		}
                p++;
                if (ext != NULL && 0 != strcasecmp(p, ext)) {
			continue;
		}
		lpFileName = new char[NAME_MAX];
		strcpy(lpFileName, dent->d_name);
		list->AddTail(lpFileName);
	}

	closedir(dir);
#endif

	return TRUE;
}

#if defined(WIN32)
sint32 c3files_getfilelist_ex(C3SAVEDIR dirID, const MBCHAR *ext, PointerList<WIN32_FIND_DATA> *list)
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
#endif

#define k_CTP_CD_VOLUME_NAME		"CTP2"

MBCHAR	VolumeName[33] = { 0 };
int 	WhichCD = -1;
int	CDDrivesCnt = 0;
const int kMAX_NUM_CDDRIVES = 26;
sint32	CDIndex = 0;
BOOL	g_hasCD = FALSE;

#if defined(WIN32) && !defined(USE_SDL)
typedef MBCHAR win32_drive_name[3];
win32_drive_name CDDrivesNames[kMAX_NUM_CDDRIVES] = { 0 };
#endif

#include "soundmanager.h"
extern SoundManager		*g_soundManager;

extern C3UI *g_c3ui;

const MBCHAR *c3files_GetCTPHomeDir()
{
	static MBCHAR ctphome[MAX_PATH] = { 0 };
	static BOOL init = FALSE;

	if (!init) {
		init = TRUE;
#ifdef LINUX
		MBCHAR tmp[MAX_PATH] = { 0 };
		uid_t uid = getuid();
		struct passwd *pwent = getpwuid(uid);
		if (!pwent)
			return NULL;

		if (!pwent->pw_dir)
			return NULL;

		if (!*pwent->pw_dir)
			return NULL;

		size_t s = snprintf(tmp, MAX_PATH, "%s" FILE_SEP "%s",
		                    pwent->pw_dir, ".civctp2");
		if (s > MAX_PATH) {
			return NULL;
		}
		struct stat st = { 0 };
		if (stat(tmp, &st) == 0) {
			if (S_ISDIR(st.st_mode)) {
				strncpy(ctphome, tmp, MAX_PATH);
				return ctphome;
			} else {
				return NULL;
			}
		}
		mode_t mode = 0777;
		if (mkdir(tmp, mode) == 0) {
			strncpy(ctphome, tmp, MAX_PATH);
			return ctphome;
		}

		return NULL;
#endif
	}

	if (ctphome[0])
		return ctphome;
	else
		return NULL;
}

BOOL c3files_HasLegalCD()
{
	BOOL success = FALSE;

	if (g_soundManager) {
		g_soundManager->CleanupRedbook();
	}

	while (!success) {

		success = c3files_FindCDByName(k_CTP_CD_VOLUME_NAME, TRUE);

		
		if (success && g_theProfileDB->IsProtected()) {		
			BOOL		valid;

			valid = tracklen_CheckTrackLengths();

			if (!valid) 
				success = TRUE;
			else
				success = FALSE;
		}

		if (!success) {
#ifdef WIN32			
			int rval = MessageBox(g_c3ui->TheHWND(),
			                      appstrings_GetString(APPSTR_INSERTCDROM),
			                      appstrings_GetString(APPSTR_CDROM),
			                      MB_RETRYCANCEL |
			                      MB_ICONEXCLAMATION |
			                      MB_DEFBUTTON1 |
			                      MB_SYSTEMMODAL |
			                      MB_SETFOREGROUND);

			if (rval == IDCANCEL) {
#endif // WIN32
				c3errors_ErrorDialog(appstrings_GetString(APPSTR_CDROM),
				                     appstrings_GetString(APPSTR_NEEDCDROM));
				
				exit(-1);
#ifdef WIN32
			}
#endif // WIN32

			
			
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
#if 0
	int rc = SDL_Init(SDL_INIT_CDROM);
	if (0 != rc) {
		fprintf(stderr, "Could not initialize CDROM:\n%s\n", SDL_GetError());
		return;
	}
#endif
	c3files_GetCDDrives();
	c3files_FindCDByName(k_CTP_CD_VOLUME_NAME, TRUE);
}

int c3files_GetCDDriveCount(void)
{
	return CDDrivesCnt;
}

int c3files_GetCTPCDDriveNum(void)
{
	return WhichCD;
}

BOOL c3files_HasCD(void)
{
	return g_hasCD;
}


void c3files_GetCDDrives(void)
{
#if defined(USE_SDL)
	CDDrivesCnt = SDL_CDNumDrives();
	if (-1 == CDDrivesCnt) {
		CDDrivesCnt = 0;
		// Forgot init...
		Assert(0);
		return;
	}
#elif defined(WIN32)
	uint32		all_drives;
	MBCHAR		i;
	MBCHAR		drivepath[16];
    
	WhichCD = -1;
	memset(CDDrivesNames, 0, sizeof(CDDrivesNames));
	CDDrivesCnt = 0;

	strcpy(drivepath, " :\\");
	all_drives = GetLogicalDrives();

	for (i = 0; i < kMAX_NUM_CDDRIVES; i++)
	{
		if (all_drives & (1 << i))
		{
			drivepath[0] = i + 'A';
			if (GetDriveType(drivepath) == DRIVE_CDROM) {
				CDDrivesNames[CDDrivesCnt][0] = drivepath[0];
				CDDrivesNames[CDDrivesCnt][1] = ':';
				CDDrivesCnt++;
			}
		}
	}
#endif
}

const MBCHAR *c3files_GetCDDriveMount(MBCHAR *buf, size_t size,
                                      int cdIndex)
{
	if (!buf)
		return NULL;

	const MBCHAR *cdDriveName = c3files_GetCDDriveName(cdIndex);

	if (!cdDriveName)
		return NULL;
#ifdef WIN32
	if (strlen(cdDriveName) >= size)
		return NULL;

	strcpy(buf, cdDriveName);
	return buf;
#elif defined(LINUX)
	MBCHAR tempPath[_MAX_PATH] = { 0 };
	const size_t mntInfoCnt = 2;
	// Do not change order of mntInfo 
	const MBCHAR *mntInfo[mntInfoCnt] = { _PATH_MOUNTED, _PATH_MNTTAB };
	const size_t mntOptsCnt = 23;
	typedef struct {
		const char *name;
		unsigned long flag;
		bool clear;
	} mount_opt_t;
	const mount_opt_t mntOpts[mntOptsCnt] = {
		{ MNTOPT_RO,      MS_RDONLY, false },
		{ MNTOPT_RW,      MS_RDONLY, true },
		{ MNTOPT_NOSUID,  MS_NOSUID, false },
		{ MNTOPT_SUID,    MS_NOSUID, true },
		{ "nodev",        MS_NODEV, false },
		{ "dev",          MS_NODEV, true },
		{ "noexec",       MS_NOEXEC, false },
		{ "exec",         MS_NOEXEC, true },
		{ "sync",         MS_SYNCHRONOUS, false },
		{ "async",        MS_SYNCHRONOUS, true },
		{ "remount",      MS_REMOUNT, false },
		{ "lock",         MS_MANDLOCK, false },
		{ "nolock",       MS_MANDLOCK, true },
		{ "dirsync",      MS_DIRSYNC, false },
		{ "noatime",      MS_NOATIME, false },
		{ "atime",        MS_NOATIME, true },
		{ "nodiratime",   MS_NODIRATIME, false },/// \todo: check string
		{ "bind",         MS_BIND, false },
//		{ MS_MOVE },			// for /bin/mount
//		{ MS_REC },
		{ "verbose",      MS_VERBOSE, false },
		{ "acl",          MS_POSIXACL, false },
		{ "noacl",        MS_POSIXACL, true },
//		{ MS_ACTIVE },
		{ "nouser",       MS_NOUSER, false },
		{ "user",         MS_NOUSER, true }
	};
	BOOL cdrLink = FALSE;
	MBCHAR devlink[_MAX_PATH] = { 0 };
	char *fulllink = 0;
	struct stat st = { 0 };
	if (lstat(cdDriveName, &st) != 0) {
		return NULL;
	}
	if (S_ISLNK(st.st_mode)) {
		int len = readlink(cdDriveName, devlink, _MAX_PATH);
		if ((len > 0) && (len < _MAX_PATH)) {
			cdrLink = TRUE;
			devlink[len] = '\0';
			strcpy(tempPath, cdDriveName);
			char *dn = dirname(tempPath);
			if (dn)
				strcpy(tempPath, dn);
			
			strcat(tempPath, FILE_SEP);
			strcat(tempPath, devlink);
			fulllink = _fullpath(NULL, tempPath, 0);
		}
	}

	for (size_t u = 0; u < mntInfoCnt; u++) {
		FILE *mounts = setmntent(mntInfo[u], "r");
		if (mounts) {
			struct mntent *mntent;
			while ((mntent = getmntent(mounts)) != NULL) {
				if ((mntent->mnt_fsname == NULL) || 
				    (mntent->mnt_dir == NULL) ||
				    (mntent->mnt_type == NULL))
					continue;
			
				if (mntent->mnt_type == MNTTYPE_IGNORE)
					continue;
			
				if ((!strcasecmp(cdDriveName, mntent->mnt_fsname))
				    || (cdrLink && (!strcasecmp(devlink, mntent->mnt_fsname)))
				    || ((fulllink != NULL) && (!strcasecmp(fulllink, mntent->mnt_fsname)))
				   ) {
					const MBCHAR *ret = NULL;
					// Mounted?
					if (u == 0) {
						strncpy(buf, mntent->mnt_dir, size);
						ret = buf;
					// Not mounted fstab entry
					} else if (u == 1) {
						unsigned long flags = MS_MGC_VAL;
						if (hasmntopt(mntent, "defaults")) {
							flags |= (MS_NOUSER);
						}
						for (size_t f = 0; f < mntOptsCnt; f++) {
							if (hasmntopt(mntent, mntOpts[f].name))
								if (mntOpts[f].clear)
									flags &= ~mntOpts[f].flag;
								else
									flags |= mntOpts[f].flag;
						}
						errno = 0;
						int rc = mount(mntent->mnt_fsname,
						               mntent->mnt_dir,
						               mntent->mnt_type,
						               flags, 0);
						if (0 == rc) {
							strncpy(buf, mntent->mnt_dir, size);
							ret = buf;
						} else {
							perror(strerror(errno));
						}
					}
					
					endmntent(mounts);
					if (fulllink) {
						free(fulllink);
						fulllink = 0;
					}

					return ret;
				}
			}
			endmntent(mounts);
		}
	}
	if (fulllink) {
		free(fulllink);
		fulllink = 0;
	}
	return NULL;
#endif
}

const MBCHAR *c3files_GetCDDriveName(int cdIndex)
{
	if (cdIndex < 0)
		return NULL;

	if (cdIndex > CDDrivesCnt)
		return NULL;

#if defined(USE_SDL)
	return SDL_CDName(cdIndex);
#else
	return CDDrivesNames[cdIndex];
#endif
}

const MBCHAR *c3files_GetVolumeName(int cdIndex)
{
#if defined(WIN32)
	if (cdIndex < 0)
		return NULL;

	if (cdIndex >= k_MAX_NUM_CDDRIVES)
		return NULL;

	MBCHAR drivepath[32];
	MBCHAR FSName[32];
	uint32 SerialNumber;
	uint32 MaxComponentLen;
	uint32 FSFlags;
	MBCHAR name = (MBCHAR) cdIndex+'A';
    
	strcpy(drivepath, " :\\");
	drivepath[0] = name;
	if (GetVolumeInformation(drivepath, VolumeName, 32, &SerialNumber,
	                         &MaxComponentLen, &FSFlags, FSName, 32)) {
		return(VolumeName);
	}
	return(NULL);
#elif defined(LINUX)
	/// \todo Add code to determine beginsector of iso_primary_sector
	/// On german ctp2 cd, it starts on sector 16 (byte 16 << 11 = 0x8000)
	const char *cd_dev = SDL_CDName(cdIndex);
	FILE *cd = fopen(cd_dev, "rb");
	if (cd == NULL) {
		fprintf(stderr, "%s\n", strerror(errno));
		return NULL;
	}
	int rc = fseek(cd, 0x8000, SEEK_SET);
	if (rc != 0) {
		fprintf(stderr, "%s\n", strerror(errno));
		fclose(cd);
		return NULL;
	}
	struct iso_primary_descriptor ipd = { 0 };
	size_t s = fread(&ipd, 1, sizeof(ipd), cd);
	if (s != sizeof(ipd)) {
		fprintf(stderr, "Failed reading %d bytes: %s\n", sizeof(ipd),
		        strerror(errno));
		fclose(cd);
		return NULL;
	}
	unsigned int i = 0;
	s = std::min(sizeof(ipd.volume_id), sizeof(VolumeName));
	assert(i < s);
	while (i < s) {
		if ('\0' == ipd.volume_id[i]) {
			VolumeName[i] = '\0';
			return VolumeName;
		}
		if ((!isalnum(ipd.volume_id[i])) && (!isspace(ipd.volume_id[i]))) {
			return NULL;
		}
		VolumeName[i] = ipd.volume_id[i];
		i++;
	}
	i--;
	// Win32 Compat: GetVolumeInformation does not return ending spaces
	while ((i > 0) && isspace(VolumeName[i])) {
		i--;
	}
	if (isspace(VolumeName[i+1]))
		VolumeName[i+1] = '\0';

	return VolumeName;
#endif
}

BOOL c3files_FindCDByName(const CHAR *name, BOOL findDriveNum)
{
	sint32	i;
	BOOL	found = FALSE;
	const MBCHAR *cdName;

	CDIndex = 0;

	if (findDriveNum)
		WhichCD = -1;

	if (!g_theProfileDB->IsRequireCD()) {
		for (i=0; i < CDDrivesCnt; i++) {
			if (findDriveNum) {
				cdName = c3files_GetVolumeName(i);
				if (cdName && !strnicmp(cdName, name, strlen(name)))
					WhichCD = i;
			}
			g_hasCD = TRUE;
		}
		return TRUE;
	} else {
		for (i = 0; (i < CDDrivesCnt) && !found; i++)
		{
			cdName = c3files_GetVolumeName(i);
			if (cdName && !strnicmp(cdName, name, strlen(name)))
			{
				if (findDriveNum)
					WhichCD = i;
				
				found = TRUE;
				g_hasCD = TRUE;
			}
		}

		if (!found && findDriveNum)
			found = (WhichCD >= 0);

		return (found);
	}
}

