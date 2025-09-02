//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : File paths
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
// USE_SDL
// - Use SDL for sound and cdrom (originally, now disabled)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added linux specific code.
// - Added option to use multiple data directories.
// - Memory leak/crash fix
// - FindFile can ignore files in scenario paths. (9-Apr-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3files.h"
#include "CivPaths.h"
#include "profileDB.h"        // g_theProfileDB
#include "LanguageRecord.h"
#include <clocale>
#if defined(__AUI_USE_SDL__)
#include "SDL_locale.h"
#endif

#ifdef WIN32
#include <shlobj.h>
#else
#define mkdir(a, b) mkdir(CI_FixName(a), b)
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

CivPaths *g_civPaths = NULL;

#include "prjfile.h"
extern ProjectFile *g_ImageMapPF;

#define FILE_CIVPATHS_TXT "civpaths.txt"

#if defined(__AUI_USE_DIRECTX__)
#include "muiload.h"
#endif

void CivPaths_InitCivPaths()
{
	AUI_ERRCODE err = AUI_ERRCODE_OK;

	// Now let's get the paths, so that we know were the language database is.
	delete g_civPaths;
	g_civPaths = new CivPaths(err);

	if(AUI_ERRCODE_OK != err)
	{
		CivPaths_CleanupCivPaths();

		if(AUI_ERRCODE_LOADFAILED == err)
		{
			c3errors_FatalDialog("CivPaths", "Unable to open '%s'.",
			                     FILE_CIVPATHS_TXT);
		}
		else
		{
			c3errors_FatalDialog("CivPaths", "Unable to setup CTP2 paths.");
		}
	}

	// Get the Language DB up
	g_theLanguageDB = new CTPDatabase<LanguageRecord>;
	if (!g_theLanguageDB->Parse(C3DIR_GAMEDATA, "Language.txt"))
	{
		c3errors_FatalDialog("CivApp", "Unable to init the LanguageDB.");
	}

	// Get the language directory from the user profile, or if it is not
	// saved there, get it from the operating system. Or use the default entry
	// in the langauage database. If this does not exsist, just stay with the 
	// value from civpath.txt.
	g_civPaths->FindAndSetLocalizedPath();

	g_civPaths->SetLocaleFromLanguage();
}

void CivPaths_CleanupCivPaths()
{
	allocated::clear(g_civPaths);
	allocated::clear(g_theLanguageDB);
}

CivPaths::CivPaths(AUI_ERRCODE &errcode)
	:
	m_hdPath(new MBCHAR[_MAX_PATH]),
#if !defined(USE_SDL)
	m_cdPath(new MBCHAR[_MAX_PATH]),
#endif
	m_defaultPath(new MBCHAR[_MAX_PATH]),
	m_localizedPath(new MBCHAR[_MAX_PATH]),
	m_dataPath(new MBCHAR[_MAX_PATH]),
	m_extraDataPaths(),
	m_scenariosPath(new MBCHAR[_MAX_PATH]),
	m_savePath(new MBCHAR[_MAX_PATH]),
	m_saveGamePath(new MBCHAR[_MAX_PATH]),
	m_saveQueuePath(new MBCHAR[_MAX_PATH]),
	m_saveMPPath(new MBCHAR[_MAX_PATH]),
	m_saveSCENPath(new MBCHAR[_MAX_PATH]),
	m_saveMapPath(new MBCHAR[_MAX_PATH]),
	m_saveClipsPath(new MBCHAR[_MAX_PATH]),
	m_curScenarioPath(NULL),
	m_curScenarioPackPath(NULL)
{
	std::fill(m_desktopPath, m_desktopPath + _MAX_PATH, 0);

	FILE *  fin = fopen(FILE_CIVPATHS_TXT, "r");
	if (!fin)
	{
		const char *ctphome = c3files_GetCTPHomeDir();
		if (ctphome)
		{
			char tempname[MAX_PATH] = { 0 };
			snprintf(tempname, MAX_PATH, "%s" FILE_SEP "%s",
				ctphome, FILE_CIVPATHS_TXT);
			fin = fopen(tempname, "r");
		}
	}
#ifdef LINUX
	if (!fin)
	{
		fin = fopen(PACKAGE_DATADIR FILE_SEP FILE_CIVPATHS_TXT, "r");
	}
	if (!fin)
	{
		fin = fopen(PACKAGE_SYSCONFDIR FILE_SEP FILE_CIVPATHS_TXT, "r");
	}
#endif
	Assert(fin);
	if (!fin)
	{
		errcode = AUI_ERRCODE_LOADFAILED;
		return;
	}

	fscanf(fin, "%s", m_hdPath);
#if defined(USE_SDL)
	MBCHAR dummy[_MAX_PATH];
	fscanf(fin, "%s", dummy);
#else // USE_SDL
	fscanf(fin, "%s", m_cdPath);
#endif // USE_SDL
	fscanf(fin, "%s", m_defaultPath);
	fscanf(fin, "%s", m_localizedPath);
	fscanf(fin, "%s", m_dataPath);
	fscanf(fin, "%s", m_scenariosPath);
	fscanf(fin, "%s", m_savePath);
	fscanf(fin, "%s", m_saveGamePath);
	fscanf(fin, "%s", m_saveQueuePath);
	fscanf(fin, "%s", m_saveMPPath);
	fscanf(fin, "%s", m_saveSCENPath);
	fscanf(fin, "%s", m_saveMapPath);
	fscanf(fin, "%s", m_saveClipsPath);

	ReplaceFileSeperator(m_hdPath);
#if !defined(USE_SDL)
	ReplaceFileSeperator(m_cdPath);
#endif // USE_SDL
	ReplaceFileSeperator(m_defaultPath);
	ReplaceFileSeperator(m_localizedPath);
	ReplaceFileSeperator(m_dataPath);
	ReplaceFileSeperator(m_scenariosPath);
	ReplaceFileSeperator(m_savePath);
	ReplaceFileSeperator(m_saveGamePath);
	ReplaceFileSeperator(m_saveQueuePath);
	ReplaceFileSeperator(m_saveMPPath);
	ReplaceFileSeperator(m_saveSCENPath);
	ReplaceFileSeperator(m_saveMapPath);
	ReplaceFileSeperator(m_saveClipsPath);

	m_assetPaths[C3DIR_BASE] = new MBCHAR[2];
	m_assetPaths[C3DIR_BASE][0] = '.';
	m_assetPaths[C3DIR_BASE][1] = '\0';

	for (size_t dir = 1; dir < C3DIR_MAX; ++dir)
	{
		m_assetPaths[dir] = new MBCHAR[_MAX_PATH];
		fscanf (fin, "%s", m_assetPaths[dir]);
		ReplaceFileSeperator(m_assetPaths[dir]);
	}

	fclose(fin);

	MBCHAR	tempPath[_MAX_PATH];
	MBCHAR	fullPath[_MAX_PATH];
	MBCHAR	*s;

	sprintf(tempPath, "%s%s%s", m_hdPath, FILE_SEP, m_savePath);
	s = _fullpath(fullPath, tempPath, _MAX_PATH);
	Assert(s != NULL);

	CreateSaveFolders(fullPath);

	errcode = AUI_ERRCODE_OK;
}

CivPaths::~CivPaths()
{
	ResetExtraDataPaths();
	std::vector<MBCHAR const *>().swap(m_extraDataPaths);

	delete [] m_curScenarioPath;
	delete [] m_curScenarioPackPath;
	delete [] m_hdPath;
#if !defined(USE_SDL)
	delete [] m_cdPath;
#endif // USE_SDL
	delete [] m_defaultPath;
	delete [] m_localizedPath;
	delete [] m_dataPath;
	delete [] m_scenariosPath;
	delete [] m_savePath;
	delete [] m_saveGamePath;
	delete [] m_saveQueuePath;
	delete [] m_saveMPPath;
	delete [] m_saveSCENPath;
	delete [] m_saveMapPath;
	delete [] m_saveClipsPath;

	for (size_t dir = 0; dir < C3DIR_MAX; ++dir)
	{
		delete [] m_assetPaths[dir];
	}
}

void CivPaths::CreateSaveFolders(const MBCHAR *path)
{
	MBCHAR subFolderPath[_MAX_PATH] = {0};

	c3files_CreateDirectory(path);

	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveGamePath);
	c3files_CreateDirectory(subFolderPath);
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveQueuePath);
	c3files_CreateDirectory(subFolderPath);
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveMPPath);
	c3files_CreateDirectory(subFolderPath);
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveSCENPath);
	c3files_CreateDirectory(subFolderPath);
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveMapPath);
	c3files_CreateDirectory(subFolderPath);
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveClipsPath);
	c3files_CreateDirectory(subFolderPath);
}

#if !defined(USE_SDL)
void CivPaths::InitCDPath(void)
{
	MBCHAR tempPath[_MAX_PATH] = {0};
	
	DriveIdType ctpcd = c3files_GetCtpCdId();
	if(ctpcd < 0)
		return;

	sprintf(tempPath, "%c:%s%s", ctpcd, FILE_SEP, m_cdPath);
	strcpy(m_cdPath, tempPath);
}
#endif // USE_SDL

MBCHAR *CivPaths::MakeSavePath(MBCHAR *fullPath, MBCHAR *s1, MBCHAR *s2, MBCHAR *s3)
{
	MBCHAR			tempPath[_MAX_PATH];
	MBCHAR			*s;
	int			r;
#ifdef WIN32
	struct _stat		tmpstat;
#else
	struct stat		tmpstat;
#endif

	{
		sprintf(tempPath, "%s%s%s%s%s", s1, FILE_SEP, s2, FILE_SEP, s3);

		s = _fullpath(fullPath, tempPath, _MAX_PATH);
		Assert(s != NULL);

#ifdef WIN32
		r = _stat(s, &tmpstat);
#else
		r = stat(CI_FixName(s), &tmpstat);
#endif

		if (r == 0) {
			strcat(fullPath, FILE_SEP);
			return fullPath;
		}
		else return NULL;
	}
}

void CivPaths::ReplaceFileSeperator(MBCHAR* path)
{
#ifdef LINUX
	MBCHAR* oldChar;
	MBCHAR* newChar;
	MBCHAR newPath[_MAX_PATH];
 
	oldChar=path;
	newChar=newPath;

	while (*oldChar && newChar < newPath+_MAX_PATH-1)
	{
		if (*oldChar == '\\' || *oldChar == '/')
		{
			strncpy(newChar,FILE_SEP,newPath+_MAX_PATH-newChar);
			newChar+=strlen(FILE_SEP);
		}
		else
		{
			*newChar=*oldChar;
			newChar++;
		}
		oldChar++;
	}

	*newChar = '\0';

	strncpy(path,newPath,_MAX_PATH);
#endif
}

MBCHAR *CivPaths::GetSavePath(C3SAVEDIR dir, MBCHAR *path)
{
	MBCHAR		fullPath[_MAX_PATH];

	switch (dir) {
	case C3SAVEDIR_GAME:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveGamePath))
		{
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_QUEUES:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveQueuePath))
		{
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_MP:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveMPPath))
		{
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_SCEN:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveSCENPath))
		{
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_MAP:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveMapPath))
		{
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_CLIPS:
		if(MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveClipsPath))
		{
			strcpy(path, fullPath);
			return path;
		}
		break;
	default :
		Assert(FALSE);
	}

	return NULL;
}

MBCHAR *CivPaths::MakeAssetPath
(
    MBCHAR *        fullPath,
    MBCHAR const *  s1,
    MBCHAR const *  s2,
    MBCHAR const *  s3,
    MBCHAR const *  s4,
    MBCHAR const *  s5
) const
{
	MBCHAR			tempPath[_MAX_PATH];
	MBCHAR			*s;
	int			r;
#ifdef WIN32
	struct _stat		tmpstat;
#else
	struct stat		tmpstat;
#endif

	sprintf(tempPath, "%s%s%s%s%s%s%s%s%s",
	        s1, FILE_SEP, s2, FILE_SEP, s3, FILE_SEP, s4, FILE_SEP, s5);
	
	s = _fullpath(fullPath, tempPath, _MAX_PATH);
#if defined(WIN32)
	// The Linux version of _fullpath returns NULL
	// if the path does not exist. This is valid behavior.
	Assert(s != NULL);
#endif
	if(!s) return NULL;

#ifdef WIN32
	r = _stat(s, &tmpstat);
#else
	r = stat(CI_FixName(s), &tmpstat);
#endif

	if (r == 0) return fullPath;
	else return NULL;
}

MBCHAR *CivPaths::FindFile(C3DIR dir, const MBCHAR *filename, MBCHAR *path,
                           bool silent, bool check_prjfile, bool checkScenario, bool checkLocalizedPath)
{
	MBCHAR			fullPath[_MAX_PATH];

	Assert(path != NULL);

	Assert(dir < C3DIR_MAX);

	Assert(filename != NULL);

	if (dir == C3DIR_DIRECT)
	{
		strcpy(path, filename);

		return path;
	}

	if(checkScenario)
	{
		if (m_curScenarioPath)
		{
			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);
			if (checkLocalizedPath && c3files_PathIsValid(fullPath))
			{
				strcpy(path, fullPath);
				return path;
			}
			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);

			if (c3files_PathIsValid(fullPath))
			{
				strcpy(path, fullPath);
				return path;
			}
		}

		if (m_curScenarioPackPath)
		{
			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPackPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);
			if (checkLocalizedPath && c3files_PathIsValid(fullPath))
			{
				strcpy(path, fullPath);
				return path;
			}
			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPackPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);

			if (c3files_PathIsValid(fullPath))
			{
				strcpy(path, fullPath);
				return path;
			}
		}
	}

	// The extra data paths take priority over the regular one.
	for
	(
		std::vector<MBCHAR const *>::iterator	p	= m_extraDataPaths.begin();
		p != m_extraDataPaths.end();
		++p
	)
	{
		MBCHAR const *	l_dataPath	= *p;
		if (checkLocalizedPath && MakeAssetPath(fullPath, m_hdPath, l_dataPath, m_localizedPath, m_assetPaths[dir], filename) ||
		                          MakeAssetPath(fullPath, m_hdPath, l_dataPath, m_defaultPath,   m_assetPaths[dir], filename)
		   )
		{
			strcpy(path, fullPath);
			return path;
		}
	}

	// When not found in the new data, try the original directories
	if (checkLocalizedPath && MakeAssetPath(fullPath, m_hdPath, m_dataPath, m_localizedPath, m_assetPaths[dir], filename))
	{
		strcpy(path, fullPath);
		return path;
	}

	if (MakeAssetPath(fullPath, m_hdPath, m_dataPath, m_defaultPath, m_assetPaths[dir], filename))
	{
		strcpy(path, fullPath);
		return path;
	}

#if !defined(USE_SDL)
	// The CD will only have the original content
	if (checkLocalizedPath && MakeAssetPath(fullPath, m_cdPath, m_dataPath, m_localizedPath, m_assetPaths[dir], filename))
	{
		strcpy(path, fullPath);
		return path;
	}

	if (MakeAssetPath(fullPath, m_cdPath, m_dataPath, m_defaultPath, m_assetPaths[dir], filename))
	{
		strcpy(path, fullPath);
		return path;
	}
#endif // !USE_SDL

	if (check_prjfile &&
	    ((dir == C3DIR_PATTERNS) ||
	     (dir == C3DIR_PICTURES)))
	{
		size_t len = strlen(filename);

		if (len > 3)
		{
			strcpy(fullPath, filename);
			fullPath[len-3] = 'r';
			fullPath[len-2] = 'i';
			fullPath[len-1] = 'm';

			if (g_ImageMapPF && g_ImageMapPF->exists(fullPath))
			{
				strcpy(path, filename);
				return path;
			}
		}
	}

	if (!silent)
		c3errors_ErrorDialog("Paths", "'%s' not found in asset tree.", filename);

	return NULL;
}

//----------------------------------------------------------------------------
//
// Name       : CivPaths::FindPath
//
// Description: Get the next possible lookup path
//
// Parameters : dir		type of path to lookup (not really used)
//              num     "index" of path to lookup
//
// Returns    : bool    there may be more paths to lookup
//              path    filled with a found path (set to "" when not found)
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool CivPaths::FindPath(C3DIR dir, int num, MBCHAR * path)
{
	Assert(path != NULL);
	Assert(dir < C3DIR_MAX);

	path[0] = 0;

	if (dir == C3DIR_DIRECT)
	{
		return false;
	}

	MBCHAR          tempPath[_MAX_PATH];
	tempPath[0] = 0;

	switch (num)
	{
	case 0:
		if (m_curScenarioPath)
		{
			sprintf(path, "%s%s%s%s%s", m_curScenarioPath, FILE_SEP,
				m_localizedPath, FILE_SEP, m_assetPaths[dir]);
		}
		break;

	case 1:
		if (m_curScenarioPath)
		{
			sprintf(path, "%s%s%s%s%s", m_curScenarioPath, FILE_SEP,
				m_defaultPath, FILE_SEP, m_assetPaths[dir]);
		}
		break;

	case 2:
		if (m_curScenarioPath)
		{
			sprintf(path, "%s%s%s%s%s", m_curScenarioPackPath, FILE_SEP,
				m_localizedPath, FILE_SEP, m_assetPaths[dir]);
		}
		break;

	case 3:
		if (m_curScenarioPath)
		{
			sprintf(path, "%s%s%s%s%s", m_curScenarioPackPath, FILE_SEP,
				m_defaultPath, FILE_SEP, m_assetPaths[dir]);
		}
		break;

	default:
		{
			size_t const    i   = (num - 4) / 2;

			if (i < m_extraDataPaths.size())
			{
				if (num & 1)    // even: language dependent, odd: default
				{
					sprintf(tempPath, "%s%s%s%s%s%s%s", m_hdPath, FILE_SEP,
					        m_extraDataPaths[i], FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir]);
				}
				else
				{
					sprintf(tempPath, "%s%s%s%s%s%s%s", m_hdPath, FILE_SEP,
					        m_extraDataPaths[i], FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir]);
				}
			}
			else
			{
				switch (num - 2 * m_extraDataPaths.size())
				{
				case 4:
					sprintf(tempPath, "%s%s%s%s%s%s%s", m_hdPath, FILE_SEP,
					        m_dataPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir]);
					break;
				case 5:
					sprintf(tempPath, "%s%s%s%s%s%s%s", m_hdPath, FILE_SEP,
					        m_dataPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir]);
					break;
#if !defined(USE_SDL)
				case 6:
					if (m_cdPath && m_cdPath[0])
					{
						sprintf(tempPath, "%s%s%s%s%s%s%s", m_cdPath, FILE_SEP,
						    m_dataPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir]);
					}
					break;
				case 7:
					if (m_cdPath && m_cdPath[0])
					{
						sprintf(tempPath, "%s%s%s%s%s%s%s", m_cdPath, FILE_SEP,
						    m_dataPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir]);
					}
					break;
#endif // USE_SDL
				default:
					return false;
				} // switch
			}

			if (_fullpath(path, tempPath, _MAX_PATH) == NULL)
			{
				path[0] = 0;
			}
		} // scope default
	} // switch

	return true;
}

MBCHAR *CivPaths::GetSpecificPath(C3DIR dir, MBCHAR *path, BOOL local)
{
	Assert(path != NULL);
	if (path == NULL) return NULL;
	Assert(dir < C3DIR_MAX);
	if (dir >= C3DIR_MAX) return NULL;

	MBCHAR			tempPath[_MAX_PATH];
	if (local)
	{
		sprintf(tempPath, "%s%s%s%s%s%s%s", m_hdPath, FILE_SEP, m_dataPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir]);
	}
	else
	{
		sprintf(tempPath, "%s%s%s%s%s%s%s", m_hdPath, FILE_SEP, m_dataPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir]);
	}

	MBCHAR          fullPath[_MAX_PATH];
	MBCHAR const *  s = _fullpath(fullPath, tempPath, _MAX_PATH);
	Assert(s);
	if (s)
	{
		strcpy(path, fullPath);
	}
	return path;
}

MBCHAR *CivPaths::GetScenarioRootPath(MBCHAR *path)
{
	MBCHAR          temp[_MAX_PATH];
	MBCHAR const *  s  = _fullpath(temp, m_scenariosPath, _MAX_PATH);
	Assert(s);
	if (s)
	{
		strcpy(path, temp);
	}

	return path;
}

void CivPaths::SetCurScenarioPath(const MBCHAR *path)
{
	delete [] m_curScenarioPath;
	m_curScenarioPath = new MBCHAR[_MAX_PATH];
	memset(m_curScenarioPath, 0, _MAX_PATH);
	strcpy(m_curScenarioPath, path);
}

MBCHAR *CivPaths::GetCurScenarioPath(void)
{
	return m_curScenarioPath;
}

void CivPaths::ClearCurScenarioPath(void)
{
	delete[] m_curScenarioPath;
	m_curScenarioPath = NULL;
}

void CivPaths::SetCurScenarioPackPath(const MBCHAR *path)
{
	delete[] m_curScenarioPackPath;
	m_curScenarioPackPath = new MBCHAR[_MAX_PATH];
	memset(m_curScenarioPackPath, 0, _MAX_PATH);
	strcpy(m_curScenarioPackPath, path);
}

MBCHAR *CivPaths::GetCurScenarioPackPath(void)
{
	return m_curScenarioPackPath;
}

void CivPaths::ClearCurScenarioPackPath(void)
{
	delete[] m_curScenarioPackPath;
	m_curScenarioPackPath = NULL;
}

MBCHAR *CivPaths::GetDesktopPath(void)
{
#ifdef WIN32
	MBCHAR		    tempStr[_MAX_PATH] = { 0 };
	ITEMIDLIST *    idList;
	HRESULT		    hr = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &idList);

	Assert(hr == S_OK);
	if (    (hr != S_OK)
	    ||  !SHGetPathFromIDList(idList, tempStr)
	   )
	{
		MBCHAR * s = _fullpath(tempStr, FILE_SEP, _MAX_PATH);
		Assert(s);
		if (!s) return NULL;
	}

	strcpy(m_desktopPath, tempStr);
	return m_desktopPath;
#else
	return NULL;
#endif
}

//----------------------------------------------------------------------------
//
// Name       : CivPaths::GetExtraDataPaths
//
// Description: Inspect the data include directory lookup paths
//
// Parameters : -
//
// Globals    : -
//
// Returns    : std::vector<MBCHAR const *> const & : the extra lookup paths
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
std::vector<MBCHAR const *> const & CivPaths::GetExtraDataPaths(void) const
{
	return m_extraDataPaths;
}

//----------------------------------------------------------------------------
//
// Name       : CivPaths::InsertExtraDataPath
//
// Description: Insert a data include directory to the lookup path
//
// Parameters : path	: ctp2_data-style directory tree
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : For lookup, the most recent path has the highest priority,
//              and any inserted paths have priority above the original
//              "..\..\ctp2_data" path.
//
//----------------------------------------------------------------------------
void CivPaths::InsertExtraDataPath(MBCHAR const * path)
{
	MBCHAR *	newPath	= new MBCHAR[1 + strlen(path)];
	strcpy(newPath, path);
	m_extraDataPaths.insert(m_extraDataPaths.begin(), newPath);
}

//----------------------------------------------------------------------------
//
// Name       : CivPaths::ResetExtraDataPaths
//
// Description: Clear the entire lookup path
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The original "..\..\ctp2_data" path will remain - it is not
//              stored in m_extraDataPaths, but in m_dataPath.
//
//----------------------------------------------------------------------------
void CivPaths::ResetExtraDataPaths(void)
{
	for
	(
		std::vector<MBCHAR const *>::iterator	p = m_extraDataPaths.begin();
		p != m_extraDataPaths.end();
		++p
	)
	{
		delete [] const_cast<MBCHAR *>(*p);
	}
	m_extraDataPaths.clear();

}

void CivPaths::FindAndSetLocalizedPath()
{
	Assert(g_theProfileDB);

	// If we have a value in userprofile.txt
	const MBCHAR* LanguageDir = g_theProfileDB ? g_theProfileDB->GetLanguageDirectory() : "";
	if(LanguageDir[0] != '\0')
	{
		SetLocalizedPath(LanguageDir);
		return;
	}

	const LanguageRecord* lanRec = FindLanguage();

	if(lanRec != NULL)
		SetLocalizedPath(lanRec->GetDirectory());
}

void CivPaths::SetLocaleFromLanguage()
{
	for(sint32 i = 0; i < g_theLanguageDB->NumRecords(); i++)
	{
		const LanguageRecord* lanRec = g_theLanguageDB->Get(i);
		if(strcmp(m_localizedPath, lanRec->GetDirectory()) == 0)
		{
			std::setlocale(LC_COLLATE, lanRec->GetIsoCode());
			return;
		}
	}
}

const LanguageRecord* CivPaths::FindLanguage()
{
#if defined(__AUI_USE_SDL__)
	SDL_Locale* locales = SDL_GetPreferredLocales();

	if(locales != NULL)
	{
		// Get the language that matches the IsoCode
		for(size_t j = 0; locales[j].language != NULL; j++)
		{
			for(sint32 i = 0; i < g_theLanguageDB->NumRecords(); i++)
			{
				const LanguageRecord* lanRec = g_theLanguageDB->Get(i);

				if(lanRec->GetDisabled())
					continue;

				if(lanRec->GetHidden())
					continue;

				if(strcmp(lanRec->GetIsoCode(), locales[j].language) == 0)
				{
					SDL_free(locales);
					return lanRec;
				}
			}
		}
	}

	SDL_free(locales);

#elif defined(__AUI_USE_DIRECTX__)
	ULONG size = 0;

	GetUILanguageFallbackList(NULL, 0, &size);

	wchar_t* buffer = new wchar_t[size];
	GetUILanguageFallbackList(buffer, size, NULL);

	// Get the language that matches the IsoCode
	while(buffer[0] != L'\0')
	{
		for(sint32 i = 0; i < g_theLanguageDB->NumRecords(); i++)
		{
			const LanguageRecord* lanRec = g_theLanguageDB->Get(i);

			if(lanRec->GetDisabled())
				continue;

			if(lanRec->GetHidden())
				continue;

			if(CompareLocals(lanRec->GetIsoCode(), buffer))
			{
				return lanRec;
			}
		}

		while(true)
		{
			buffer++;
			if(buffer[0] == L'\0')
			{
				buffer++;
				break;
			}
		}
	}
#else
#error No locale detection defined for non DX / non SDL builds
#endif

	// Get default Language from database
	for(sint32 i = 0; i < g_theLanguageDB->NumRecords(); i++)
	{
		const LanguageRecord* lanRec = g_theLanguageDB->Get(i);

		if(lanRec->GetDisabled())
			continue;

		if(lanRec->GetHidden())
			continue;

		if(lanRec->GetDefault())
		{
			return lanRec;
		}
	}

	return NULL;
}

void CivPaths::SetLocalizedPath(const MBCHAR *path)
{
	sprintf(m_localizedPath, "%s", path);
	ReplaceFileSeperator(m_localizedPath);
	if(g_theProfileDB)
		g_theProfileDB->SetLanguageDirectory(path);
}

bool CivPaths::CompareLocals(const MBCHAR *locale1, const wchar_t* locale2) const
{
	for(size_t i = 0; locale1[i] != '\0'; ++i)
	{
		if(locale1[i] != static_cast<MBCHAR>(locale2[i]))
		{
			return false;
		}
	}

	return true;
}
