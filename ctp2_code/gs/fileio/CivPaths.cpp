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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added option to use multiple data directories.
// - Memory leak/crash fix
// - FindFile can ignore files in scenario paths. (9-Apr-2007 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3files.h"
#include "CivPaths.h"

#ifdef WIN32
#include <shlobj.h>
#else
#define mkdir(a, b) mkdir(CI_FixName(a), b)
#endif

CivPaths *g_civPaths;

#include "prjfile.h"
extern ProjectFile *g_ImageMapPF;


void CivPaths_InitCivPaths()
{
    delete g_civPaths;
	g_civPaths = new CivPaths;
}


void CivPaths_CleanupCivPaths()
{
    delete g_civPaths;
	g_civPaths = NULL;
}


CivPaths::CivPaths ()
:
    m_hdPath                (new MBCHAR[_MAX_PATH]),
	m_cdPath                (new MBCHAR[_MAX_PATH]),
    m_defaultPath           (new MBCHAR[_MAX_PATH]),
	m_localizedPath         (new MBCHAR[_MAX_PATH]),
	m_dataPath              (new MBCHAR[_MAX_PATH]),
    m_extraDataPaths        (),
	m_scenariosPath         (new MBCHAR[_MAX_PATH]),
    m_savePath              (new MBCHAR[_MAX_PATH]),
	m_saveGamePath          (new MBCHAR[_MAX_PATH]),
	m_saveQueuePath         (new MBCHAR[_MAX_PATH]),
	m_saveMPPath            (new MBCHAR[_MAX_PATH]),
	m_saveSCENPath          (new MBCHAR[_MAX_PATH]),
	m_saveMapPath           (new MBCHAR[_MAX_PATH]),
	m_saveClipsPath         (new MBCHAR[_MAX_PATH]),
    m_curScenarioPath       (NULL),
	m_curScenarioPackPath   (NULL)
{
    std::fill(m_desktopPath, m_desktopPath + _MAX_PATH, 0);

    FILE *  fin = fopen("civpaths.txt", "r");
    Assert(fin);

	fscanf(fin, "%s", m_hdPath);
	fscanf(fin, "%s", m_cdPath);
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
	ReplaceFileSeperator(m_cdPath);
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

	for (size_t dir = 0; dir < C3DIR_MAX; ++dir)
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
}


CivPaths::~CivPaths()
{
	ResetExtraDataPaths();
    std::vector<MBCHAR const *>().swap(m_extraDataPaths);

	delete [] m_curScenarioPath;
	delete [] m_curScenarioPackPath;
	delete [] m_hdPath;
	delete [] m_cdPath;
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
#ifdef WIN32
	SECURITY_ATTRIBUTES		sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	CreateDirectory((LPCTSTR)path, &sa);
#else
	mode_t mode = 0777;
	mkdir(path, mode);
#endif

	MBCHAR subFolderPath[_MAX_PATH];

	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveGamePath);
#ifdef WIN32
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
#else
	mkdir(subFolderPath, mode);
#endif
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveQueuePath);
#ifdef WIN32
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
#else
	mkdir(subFolderPath, mode);
#endif
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveMPPath);
#ifdef WIN32
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
#else
	mkdir(subFolderPath, mode);
#endif
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveSCENPath);
#ifdef WIN32
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
#else
	mkdir(subFolderPath, mode);
#endif
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveMapPath);
#ifdef WIN32
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
#else
	mkdir(subFolderPath, mode);
#endif
	sprintf(subFolderPath, "%s%s%s", path, FILE_SEP, m_saveClipsPath);
#ifdef WIN32
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
#else
	mkdir(subFolderPath, mode);
#endif
}

void CivPaths::InitCDPath(void)
{
	MBCHAR tempPath[_MAX_PATH];
	sprintf(tempPath, "%c:%s%s", c3files_GetCtpCdId(), FILE_SEP, m_cdPath);
	strcpy(m_cdPath, tempPath);
}


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

		if (!r) {
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
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveGamePath)) {
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_QUEUES:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveQueuePath)) {
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_MP:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveMPPath)) {
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_SCEN:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveSCENPath)) {
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_MAP:
		if (MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveMapPath)) {
			strcpy(path, fullPath);
			return path;
		}
		break;
	case C3SAVEDIR_CLIPS:
		if(MakeSavePath(fullPath, m_hdPath, m_savePath, m_saveClipsPath)) {
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
	Assert(s != NULL);
	if(!s) return NULL;

#ifdef WIN32
	r = _stat(s, &tmpstat);
#else
	r = stat(CI_FixName(s), &tmpstat);
#endif

	if (!r) return fullPath;
	else return NULL;
}




MBCHAR *CivPaths::FindFile(C3DIR dir, const MBCHAR *filename, MBCHAR *path,
                           bool silent, bool check_prjfile, bool checkScenario)
{
	MBCHAR			fullPath[_MAX_PATH];

	Assert(path != NULL);

	Assert(dir < C3DIR_MAX);

	Assert(filename != NULL);

	if (dir == C3DIR_DIRECT) {
		strcpy(path, filename);

		return path;
	}

	if(checkScenario){
		if (m_curScenarioPath) {

			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);
			if (c3files_PathIsValid(fullPath)) {

				strcpy(path, fullPath);
				return path;
			}
			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);

			if (c3files_PathIsValid(fullPath)) {

				strcpy(path, fullPath);
				return path;
			}
		}

		if (m_curScenarioPackPath) {

			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPackPath, FILE_SEP, m_localizedPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);
			if (c3files_PathIsValid(fullPath)) {

				strcpy(path, fullPath);
				return path;
			}
			sprintf(fullPath, "%s%s%s%s%s%s%s", m_curScenarioPackPath, FILE_SEP, m_defaultPath, FILE_SEP, m_assetPaths[dir], FILE_SEP, filename);

			if (c3files_PathIsValid(fullPath)) {

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
		if (MakeAssetPath(fullPath, m_hdPath, l_dataPath, m_localizedPath, m_assetPaths[dir], filename) ||
			MakeAssetPath(fullPath, m_hdPath, l_dataPath, m_defaultPath,   m_assetPaths[dir], filename)
		   )
		{
			strcpy(path, fullPath);
			return path;
		}
	}

	// When not found in the new data, try the original directories
	if (MakeAssetPath(fullPath, m_hdPath, m_dataPath, m_localizedPath, m_assetPaths[dir], filename)) {

		strcpy(path, fullPath);
		return path;
	}

	if (MakeAssetPath(fullPath, m_hdPath, m_dataPath, m_defaultPath, m_assetPaths[dir], filename)) {

		strcpy(path, fullPath);
		return path;
	}


	// The CD will only have the original content
	if (MakeAssetPath(fullPath, m_cdPath, m_dataPath, m_localizedPath, m_assetPaths[dir], filename)) {

		strcpy(path, fullPath);
		return path;
	}

	if (MakeAssetPath(fullPath, m_cdPath, m_dataPath, m_defaultPath, m_assetPaths[dir], filename)) {

		strcpy(path, fullPath);
		return path;
	}

    if (check_prjfile &&
        ((dir == C3DIR_PATTERNS) ||
			(dir == C3DIR_PICTURES))) {

		uint32 len = strlen(filename);

        if (len > 3) {

            strcpy(fullPath, filename);
            fullPath[len-3] = 'r';
            fullPath[len-2] = 'i';
            fullPath[len-1] = 'm';

            if (g_ImageMapPF && g_ImageMapPF->exists(fullPath)) {

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
	MBCHAR	        temp[_MAX_PATH];
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
