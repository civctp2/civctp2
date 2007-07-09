//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : File handling
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
// _WIN32
// - Microsoft Windows version
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

#ifndef __C3FILES_H__
#define __C3FILES_H__

template <class T> class PointerList;

enum C3DIR {
	C3DIR_DIRECT	= -1,	

	C3DIR_GAMEDATA,			
  	C3DIR_GL,
	C3DIR_AIDATA,
    C3DIR_UIDATA,			
	C3DIR_LAYOUT,
	C3DIR_FONTS,
	C3DIR_GRAPHICS,
	C3DIR_SPRITES,
	C3DIR_TILES,
	C3DIR_PATTERNS,
	C3DIR_PICTURES,
	C3DIR_ICONS,
	C3DIR_CURSORS,
    C3DIR_SOUNDS, 
	C3DIR_VIDEOS,

	C3DIR_MAX
};

enum C3SAVEDIR {
	C3SAVEDIR_GAME,
	C3SAVEDIR_QUEUES,
	C3SAVEDIR_MP,
	C3SAVEDIR_SCEN,
	C3SAVEDIR_MAP,
	C3SAVEDIR_CLIPS,

	C3SAVEDIR_MAX
};

FILE*		c3files_fopen(C3DIR dirID, const MBCHAR *, const MBCHAR *);
FILE*		c3files_freopen(const MBCHAR *, const MBCHAR *, FILE *);
sint32		c3files_fclose(FILE *);

sint32		c3files_fscanf(FILE *, const MBCHAR *, ...);
size_t		c3files_fread(void *, size_t, size_t, FILE *);
sint32		c3files_fgetc(FILE *);
sint32		c3files_fgetpos(FILE *, fpos_t *);
MBCHAR*		c3files_fgets(MBCHAR *, sint32, FILE *);

size_t		c3files_fwrite(const void *, size_t, size_t, FILE *);
sint32		c3files_fprintf(FILE *, const MBCHAR *, ...);
sint32		c3files_fputc(sint32, FILE *);
sint32		c3files_fputs(const MBCHAR *, FILE *);

sint32		c3files_fsetpos(FILE *, const fpos_t *);
sint32		c3files_fseek(FILE *, sint32, sint32);
sint32		c3files_ftell(FILE *);
sint32		c3files_feof(FILE *);

sint32		c3files_ferror(FILE *);
void		c3files_clearerr(FILE *);
sint32		c3files_fflush(FILE *);

sint32		c3files_getfilesize(C3DIR dir, MBCHAR const * filename);
uint8 *      c3files_loadbinaryfile(C3DIR dir, MBCHAR const *filename, sint32 *size);

bool		c3files_PathIsValid(MBCHAR *path);
bool		c3files_CreateDirectory(MBCHAR const *path);

void		c3files_StripSpaces(MBCHAR *s);


bool		c3files_getfilelist(C3SAVEDIR dirID, MBCHAR *ext, PointerList<MBCHAR> *plist);
#if defined WIN32
bool		c3files_getfilelist_ex(C3SAVEDIR dirID, MBCHAR *ext, PointerList<WIN32_FIND_DATA> *plist);
#endif

/** Returns the users CTP2 directory within his/her home.
 * On windows, NULL is returned. On linux, $HOME/.civctp2 will be returned.
 *
 * If the directory does not exist, it'll be created. If the creation
 * fails, NULL is returned instead.
 * 
 * @returns Path to existing CTP2 directory within users' home
 */
const MBCHAR * c3files_GetCTPHomeDir();


bool		c3files_HasLegalCD(void);
void		c3files_InitializeCD(void);
/** Returns the count of CD drives installed on the system.
 * On error, -1 is returned.
 *
 * @returns CD drive count
 */
int		c3files_GetCDDriveCount(void);
/** Returns the number of the CD drive, which contains the CTP2 CD.
 * 
 * If no CD drive exists or no drive contains the CTP2 CD, -1 is returned.
 * 
 * @returns CDDriveNum
 */
int		c3files_GetCTPCDDriveNum(void);
/**
 * Returns the system-dependent identifier for the CD drive number cdIndex.
 *
 * If cdIndex is negative or greater than the number of cd drives available,
 * NULL is returned.
 *
 * @param cdIndex Index # of CD
 * @returns CDDriveName
 */
const MBCHAR *	c3files_GetCDDriveName(int cdIndex);
/** Returns the system-dependent mount point of the CD drive number cdIndex,
 * i.e. the path to the root directory of the cd.
 *
 * If cdIndex is negative or greater than the number of cd drives available,
 * NULL is returned.
 * If the cd drive has no cd mounted, NULL is returned.
 *
 * @param buf Buffer for mount point retrieval
 * @param size Size of that buffer
 * @param cdIndex Index # of CD
 * @returns CD Mount
 */
const MBCHAR *c3files_GetCDDriveMount(MBCHAR *buf, size_t size, int cdIndex);
BOOL		c3files_HasCD(void);
void		c3files_GetCDDrives(void);
const MBCHAR		*c3files_GetVolumeName(int cdIndex);
bool		c3files_FindCDByName(MBCHAR const * name);

#endif
