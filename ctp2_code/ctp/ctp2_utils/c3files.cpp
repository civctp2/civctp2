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
// WIN32
// - Generates windows specific code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added linux specific code.
// - Added some casts. (Aug 7th 2005 Martin Gühmann)
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
// - Improved CTP2 disk detection.
// - c3files_fopen can now ignore scenario paths. (9-Apr-2007 Martin Gühmann)
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

#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef __linux__
#include <features.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <fstab.h>
// MS_* dupes with sys/mount.h
#include <linux/fs.h>
#include <linux/iso_fs.h>
#include <errno.h> //for extern int errno when mounting
#include <libgen.h>
#include <mntent.h>
#include <dirent.h>
#include <paths.h>
#include <pwd.h>
#include "cifm.h"
#define fopen(a, b) ci_fopen(a, b)
#endif

extern C3UI *       g_c3ui;

namespace
{
#ifdef _WIN32 // #ifndef USE_SDL
	DriveIdType const   DRIVE_FIRST         = 'A';
	DriveIdType const   DRIVE_LAST          = 'Z';
	DriveIdType const   DRIVE_UNDETERMINED  = '\0';
	size_t const        DRIVE_COUNT         = 1 + (DRIVE_LAST - DRIVE_FIRST);
#else
	DriveIdType const   DRIVE_FIRST         = 0;
	DriveIdType         DRIVE_LAST          = -1;
	DriveIdType const   DRIVE_UNDETERMINED  = -1;
	size_t              DRIVE_COUNT         = 0;
#endif

	size_t const        VOLUME_NAME_SIZE    = 32;

	bool                g_hasCD             = false;
#ifdef _WIN32 // #ifndef USE_SDL
	bool                IsCD[DRIVE_COUNT];
#endif
	MBCHAR              VolumeName[VOLUME_NAME_SIZE];
	DriveIdType         WhichCD             = DRIVE_UNDETERMINED;

	void                c3files_GetCDDrives(void);
	MBCHAR *            c3files_GetVolumeName(DriveIdType id);
	bool                c3files_FindCDByName(MBCHAR const * name);
}

#define k_CTP_CD_VOLUME_NAME		"CTP2"

FILE* c3files_fopen(C3DIR dirID, MBCHAR const * s1, MBCHAR const * s2, bool checkScenario)
{
	MBCHAR  s[_MAX_PATH];

	return g_civPaths->FindFile(dirID, s1, s, false, true, checkScenario) ? fopen(s, s2) : NULL;
}

FILE* c3files_freopen(const MBCHAR *s1, const MBCHAR *s2, FILE *file)
{
#ifdef __linux__
	return freopen(CI_FixName(s1), s2, file);
#else
	return freopen(s1, s2, file);
#endif
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
		delete [] bits;
		c3files_fclose(f);
		return NULL;
	}

	c3files_fclose(f);

	if ( size ) *size = filesize;

	return bits;
}

bool c3files_PathIsValid(const MBCHAR *path)
{
	int rc;
#if defined(WIN32)
	struct _stat tmpstat;
	rc = _stat(path, &tmpstat);
#else
	struct stat  tmpstat;
	rc = stat(CI_FixName(path), &tmpstat);
#endif

	return (rc == 0);
}

bool c3files_CreateDirectory(const MBCHAR *path)
{
#if defined(WIN32)
	return CreateDirectory(path, NULL) != FALSE; // BOOL to bool conversion
#else
	mode_t mask = 0777;
	return mkdir(CI_FixName(path), mask) == 0;
#endif
}

void c3files_StripSpaces(MBCHAR * s)
{
	size_t  len     = strlen(s);
	size_t  copied  = 0;

	for (size_t i = 0; i < len; ++i)
	{
		if (s[i] == ' ')
		{
			// No action: skip this one
		}
		else
		{
			s[copied++] = s[i];
		}
	}

	if (0 == copied)
	{
		strcpy(s, "-");
	}
	else
	{
		s[copied] = 0;
	}
}

bool c3files_getfilelist(C3SAVEDIR dirID, MBCHAR *ext, PointerList<MBCHAR> *list)
{
#if defined(WIN32)
	MBCHAR strbuf[256];
#endif
	MBCHAR path[_MAX_PATH];

	g_civPaths->GetSavePath(dirID, path);

#if defined(WIN32)
	if (ext) sprintf(strbuf,"*.%s",ext);
	else strcpy(strbuf, "*.*");

	strcat(path,strbuf);

	WIN32_FIND_DATA fileData;
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
#elif defined(LINUX)
	DIR *dir = opendir(CI_FixName(path));
	if (!dir)
		return FALSE;
	struct dirent *dent = NULL;

	while ((dent = readdir(dir)))
	{
		char *p = strrchr(dent->d_name, '.');
		if (NULL == p)
		{
			continue;
		}
		if (1 == strlen(p))
		{
			continue;
		}
		p++;
		if (ext != NULL && 0 != strcasecmp(p, ext))
		{
			continue;
		}
		MBCHAR *lpFileName = new char[NAME_MAX];
		strcpy(lpFileName, dent->d_name);
		list->AddTail(lpFileName);
	}

	closedir(dir);
#endif

	return true;
}

#if defined(WIN32)
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
#endif // _WIN32

const MBCHAR *c3files_GetCTPHomeDir()
{
	static MBCHAR ctphome[MAX_PATH] = {0};
	static BOOL init = FALSE;

	if(!init)
	{
		init = TRUE;
#if defined(LINUX)
		MBCHAR tmp[MAX_PATH] = {0};
		uid_t uid = getuid();
		struct passwd *pwent = getpwuid(uid);
		if(!pwent)
			return NULL;

		if(!pwent->pw_dir)
			return NULL;

		if(!*pwent->pw_dir)
			return NULL;

		size_t s = snprintf(tmp, MAX_PATH, "%s" FILE_SEP "%s",
							pwent->pw_dir, ".civctp2");
		if(s > MAX_PATH)
		{
			return NULL;
		}
		struct stat st = {0};
		if(stat(tmp, &st) == 0)
		{
			if(S_ISDIR(st.st_mode))
			{
				strncpy(ctphome, tmp, MAX_PATH);
				return ctphome;
			}
			else
			{
				return NULL;
			}
		}
		mode_t mode = 0777;
		if(mkdir(tmp, mode) == 0)
		{
			strncpy(ctphome, tmp, MAX_PATH);
			return ctphome;
		}

		return NULL;
#endif
	}

	if(ctphome[0])
		return ctphome;
	else
		return NULL;
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
#if defined(WIN32)
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
#else
				c3errors_ErrorDialog(appstrings_GetString(APPSTR_CDROM),
				                     appstrings_GetString(APPSTR_NEEDCDROM)
				                    );
#endif

				// Do not annoy the user with more messages after cancelling
				exit(-1);
#if defined(WIN32)
			}
#endif

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
#if defined(__linux__)
	int rc = SDL_Init(SDL_INIT_CDROM);
	if(0 != rc)
	{
		fprintf(stderr, "Could not initialize CDROM:\n%s\n", SDL_GetError());
		return;
	}
#endif
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

const MBCHAR *c3files_GetCDDriveMount(MBCHAR *buf, size_t size,
                                      DriveIdType cdIndex)
{
	if(buf == NULL)
		return NULL;

	MBCHAR *cdDriveName = c3files_GetVolumeName(cdIndex);

	if(cdDriveName == NULL)
		return NULL;

#if defined(WIN32)
	if(strlen(cdDriveName) >= size)
		return NULL;

	strcpy(buf, cdDriveName);
	return buf;
#elif defined(LINUX)
	MBCHAR tempPath[_MAX_PATH] = {0};
	const size_t mntInfoCnt = 2;
	// Do not change order of mntInfo
	const MBCHAR *mntInfo[mntInfoCnt] = {_PATH_MOUNTED, _PATH_MNTTAB };
	const size_t mntOptsCnt = 23;
	int lerrno = errno;
	typedef struct
	{
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
	MBCHAR devlink[_MAX_PATH] = {0};
	char *fulllink = NULL;
	struct stat st = {0};

	if(lstat(cdDriveName, &st) == -1)
	{
		for(sint32 i = 0; i < strlen(cdDriveName); ++i)
		{
			cdDriveName[i] = tolower(cdDriveName[i]);
		}
		
		if(lstat(cdDriveName, &st) == -1)
		{
			return NULL;
		}
	}

	if(S_ISLNK(st.st_mode))
	{
		int len = readlink(cdDriveName, devlink, _MAX_PATH);
		if((len > 0) && (len < _MAX_PATH))
		{

			cdrLink = TRUE;
			devlink[len] = '\0';
			strcpy(tempPath, cdDriveName);
			char *dn = dirname(tempPath);
			if(dn)
				strcpy(tempPath, dn);

			strcat(tempPath, FILE_SEP);
			strcat(tempPath, devlink);
			fulllink = _fullpath(NULL, tempPath, 0);
		}
	}

	for(size_t u = 0; u < mntInfoCnt; u++)
	{
		FILE *mounts = setmntent(mntInfo[u], "r");
		
		if(mounts)
		{
			struct mntent *mntent;
			while(mntent = getmntent(mounts))
			{
				if((mntent->mnt_fsname == NULL) ||
					(mntent->mnt_dir == NULL) ||
				   (mntent->mnt_type == NULL))
					continue;

				if(strcmp(mntent->mnt_type, MNTTYPE_IGNORE) == 0)
				if(mntent->mnt_type == MNTTYPE_IGNORE)
					continue;

				if(    strcasestr(mntent->mnt_dir, cdDriveName) != NULL
				   ||  strcasecmp(cdDriveName, mntent->mnt_fsname) == 0
				   || (cdrLink && (!strcasecmp(devlink, mntent->mnt_fsname)))
				   || ((fulllink != NULL) && (!strcasecmp(fulllink, mntent->mnt_fsname)))
				  )
				{
					const MBCHAR *ret = NULL;
					// Mounted?
					if(u == 0)
					{
						strncpy(buf, mntent->mnt_dir, size);
						ret = buf;
						// Not mounted fstab entry
					}
					else if(u == 1)
					{
						unsigned long flags = MS_MGC_VAL;
						if(hasmntopt(mntent, "defaults"))
						{
							flags |= (MS_NOUSER);
						}
						for(size_t f = 0; f < mntOptsCnt; f++)
						{
							if(hasmntopt(mntent, mntOpts[f].name))
								if(mntOpts[f].clear)
									flags &= ~mntOpts[f].flag;
								else
									flags |= mntOpts[f].flag;
						}
						errno = 0;
						int rc = mount(mntent->mnt_fsname,
									   mntent->mnt_dir,
									   mntent->mnt_type,
									   flags, 0);
						lerrno = errno; //preserve errno value of mount over other library calls!
						if(0 == rc)
						{
							strncpy(buf, mntent->mnt_dir, size);
							ret = buf;
						}
						else
						{
							fprintf(stderr, "%s L%d: CD mount error occured: ", __FILE__, __LINE__);
							perror(strerror(lerrno));
						}
					}

					endmntent(mounts);
					if(fulllink)
					{
						free(fulllink);
						fulllink = 0;
					}
					
					return ret;
				}
			}
			endmntent(mounts);
		}
	}
	if(fulllink)
	{
		free(fulllink);
		fulllink = 0;
	}

	return NULL;
#else
/* Seriously? A game mounting a filesystem? */
strlcpy(buf, cdDriveName, size);
return buf;
#endif
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
#if defined(WIN32) // #ifndef USE_SDL
	MBCHAR          drivepath[4];   // letter + : + dir seperator + zero
	strcpy(drivepath, " :" FILE_SEP);

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
#else
	int CDCnt = SDL_CDNumDrives();
	Assert(CDCnt >= 0);
	if (CDCnt < 0)
	{
		DRIVE_COUNT = 0;
	}
	else
	{
		DRIVE_COUNT = CDCnt;
	}
	DRIVE_LAST = DRIVE_COUNT - 1;
#endif
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
MBCHAR * c3files_GetVolumeName(DriveIdType id)
{
#if defined(WIN32)
	MBCHAR          drivepath[4];   // letter + : + dir seperator + zero
	strcpy(drivepath, " :" FILE_SEP);
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

#elif defined(LINUX)
	/// \todo Add code to determine beginsector of iso_primary_sector
	/// On a German ctp2 cd, it starts on sector 16 (byte 16 << 11 = 0x8000)
	const MBCHAR *cd_dev = SDL_CDName(id);
	FILE *cd = fopen(cd_dev, "rb");
	int lerrno = errno;
	if (cd == NULL)
	{
		fprintf(stderr, "%s\n", strerror(lerrno));
		return NULL;
	}
	
	int rc = fseek(cd, 0x8000, SEEK_SET);
	lerrno = errno;
	if (rc != 0)
	{
		fprintf(stderr, "%s\n", strerror(lerrno));
		fclose(cd);
		return NULL;
	}

	struct iso_primary_descriptor ipd;
	memset(&ipd, 0, sizeof(ipd));
	size_t s = fread(&ipd, 1, sizeof(ipd), cd);
	lerrno = errno;
	if (s != sizeof(ipd))
	{
		fprintf(stderr, "Failed reading %d bytes: %s\n", sizeof(ipd),
		        strerror(lerrno));
		fclose(cd);
		return NULL;
	}
	unsigned int i = 0;
	s = std::min(sizeof(ipd.volume_id), sizeof(VolumeName));
	assert(i < s);

	while (i < s)
	{
		if ('\0' == ipd.volume_id[i])
		{
			VolumeName[i] = '\0';
			return VolumeName;
		}
		if ((!isalnum(ipd.volume_id[i])) && (!isspace(ipd.volume_id[i])))
		{
			return NULL;
		}
		VolumeName[i] = ipd.volume_id[i];
		i++;
	}
	i--;

	// Win32 Compat: GetVolumeInformation does not return ending spaces
	while ((i > 0) && isspace(VolumeName[i]))
	{
		i--;
	}
	if (isspace(VolumeName[i+1]))
		VolumeName[i+1] = '\0';

	return VolumeName;
#endif
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

	for (DriveIdType i = DRIVE_FIRST; (i <= DRIVE_LAST) && !g_hasCD; ++i)
	{
#if defined(WIN32) // #ifndef USE_SDL
		if (IsCD[static_cast<int>(i - DRIVE_FIRST)])
		{
#endif
			MBCHAR const *  cdName  = c3files_GetVolumeName(i);

			if (cdName)
			{
				size_t const checkLength = strlen(cdName);

				if ((checkLength == strlen(name))
				&&  !strnicmp(cdName, name, checkLength)
				){
					WhichCD = i;
					g_hasCD = true;
				}
			}
#if defined(WIN32) // #ifndef USE_SDL
		}
#endif
	}

	return g_hasCD || !g_theProfileDB->IsRequireCD();
}

} // local namespace
