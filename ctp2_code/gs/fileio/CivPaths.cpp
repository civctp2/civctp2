











#include "c3.h"
#include "c3files.h"
#include "CivPaths.h"


#include <shlobj.h>

CivPaths *g_civPaths; 

#include "prjfile.h"
extern ProjectFile *g_ImageMapPF;



void CivPaths_InitCivPaths() 
{ 
	g_civPaths = new CivPaths; 
} 



void CivPaths_CleanupCivPaths()
{ 
	Assert(g_civPaths != NULL);

    delete g_civPaths;
	g_civPaths = NULL;
} 



CivPaths::CivPaths ()
{ 
    FILE			*fin = NULL; 
    sint32			dir;

    memset(this, 0, sizeof(*this)); 

	m_curScenarioPath = NULL;

	m_curScenarioPackPath = NULL;

    fin = fopen ("civpaths.txt", "r");		
    Assert(fin); 

	m_hdPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_hdPath);

	m_cdPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_cdPath);

	m_defaultPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_defaultPath);

	m_localizedPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_localizedPath);

	m_dataPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_dataPath);

	m_scenariosPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_scenariosPath);

	m_savePath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_savePath);

	m_saveGamePath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_saveGamePath);

	m_saveQueuePath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_saveQueuePath);

	m_saveMPPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_saveMPPath);

	m_saveSCENPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_saveSCENPath);

	m_saveMapPath = new MBCHAR[_MAX_PATH];
	fscanf (fin, "%s", m_saveMapPath);

	m_saveClipsPath = new MBCHAR[_MAX_PATH];
	fscanf(fin, "%s", m_saveClipsPath);

	
	for (dir = C3DIR_GAMEDATA; dir < C3DIR_MAX; dir++) {
		m_assetPaths[dir] = new MBCHAR[_MAX_PATH];
		fscanf (fin, "%s", m_assetPaths[dir]);
	}

	fclose(fin); 

	

	MBCHAR	tempPath[_MAX_PATH];
	MBCHAR	fullPath[_MAX_PATH];
	MBCHAR	*s;

	sprintf(tempPath, "%s\\%s", m_hdPath, m_savePath);

	s = _fullpath(fullPath, tempPath, _MAX_PATH);
	Assert(s != NULL);

	CreateSaveFolders(fullPath);
}



CivPaths::~CivPaths()
{
    sint32			dir;

	if (m_curScenarioPath)
		delete[] m_curScenarioPath;

	
	if (m_curScenarioPackPath)
		delete[] m_curScenarioPackPath;

	delete[] m_hdPath;
	delete[] m_cdPath;
	delete[] m_defaultPath;
	delete[] m_localizedPath;
	delete[] m_dataPath;
	delete[] m_scenariosPath;
	delete[] m_savePath;
	delete[] m_saveGamePath;
	delete[] m_saveQueuePath;
	delete[] m_saveMPPath;
	delete[] m_saveSCENPath;
	delete[] m_saveMapPath;
	delete[] m_saveClipsPath;

	for (dir = C3DIR_GAMEDATA; dir < C3DIR_MAX; dir++) {
		delete[] m_assetPaths[dir];
	}

}

void CivPaths::CreateSaveFolders(MBCHAR *path) 
{
	SECURITY_ATTRIBUTES		sa;

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	
	CreateDirectory((LPCTSTR)path, &sa);

	
	MBCHAR subFolderPath[_MAX_PATH];

	sprintf(subFolderPath, "%s\\%s", path, m_saveGamePath);
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
	sprintf(subFolderPath, "%s\\%s", path, m_saveQueuePath);
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
	sprintf(subFolderPath, "%s\\%s", path, m_saveMPPath);
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
	sprintf(subFolderPath, "%s\\%s", path, m_saveSCENPath);
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
	sprintf(subFolderPath, "%s\\%s", path, m_saveMapPath);
	CreateDirectory((LPCTSTR)subFolderPath, &sa);
	sprintf(subFolderPath, "%s\\%s", path, m_saveClipsPath);
}

void CivPaths::InitCDPath(void)
{
	MBCHAR tempPath[_MAX_PATH];
	sprintf(tempPath, "%c:\\%s", c3files_GetCTPCDDriveLetter(), m_cdPath);
	strcpy(m_cdPath, tempPath);
}



MBCHAR *CivPaths::MakeSavePath(MBCHAR *fullPath, MBCHAR *s1, MBCHAR *s2, MBCHAR *s3)
{
	MBCHAR			tempPath[_MAX_PATH];
	MBCHAR			*s;
	sint32			r;
    struct _stat	tmpstat;














	
	{
		sprintf(tempPath, "%s\\%s\\%s", s1, s2, s3);
		
		s = _fullpath(fullPath, tempPath, _MAX_PATH);
		Assert(s != NULL);

		r = _stat(fullPath, &tmpstat);
		
		if (!r) {
			strcat(fullPath, "\\");
			return fullPath;
		}
		else return NULL;
	}
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



MBCHAR *CivPaths::MakeAssetPath(MBCHAR *fullPath, MBCHAR *s1, MBCHAR *s2, MBCHAR *s3, MBCHAR *s4, MBCHAR *s5)
{
	MBCHAR			tempPath[_MAX_PATH];
	MBCHAR			*s;
	sint32			r;
    struct _stat	tmpstat;

	sprintf(tempPath, "%s\\%s\\%s\\%s\\%s", s1, s2, s3, s4, s5);

	s = _fullpath(fullPath, tempPath, _MAX_PATH);
	Assert(s != NULL);

	r = _stat(fullPath, &tmpstat);

	if (!r) return fullPath;
	else return NULL;
}




MBCHAR *CivPaths::FindFile(C3DIR dir, MBCHAR *filename, MBCHAR *path,
                           BOOL silent, BOOL check_prjfile)
{
	MBCHAR			fullPath[_MAX_PATH];	

	
	Assert(path != NULL);

	
	Assert(dir < C3DIR_MAX);

	
	Assert(filename != NULL);

	
	if (dir == C3DIR_DIRECT) {
		strcpy(path, filename);

		
		return path;
	}

	
	if (m_curScenarioPath) {
		
		sprintf(fullPath, "%s\\%s\\%s\\%s", m_curScenarioPath, m_localizedPath, m_assetPaths[dir], filename);
		if (c3files_PathIsValid(fullPath)) {
			
			strcpy(path, fullPath);
			return path;
		}
		sprintf(fullPath, "%s\\%s\\%s\\%s", m_curScenarioPath, m_defaultPath, m_assetPaths[dir], filename);
		
		if (c3files_PathIsValid(fullPath)) {
			
			strcpy(path, fullPath);
			return path;
		}
	}

	
	
	if (m_curScenarioPackPath) {
		
		sprintf(fullPath, "%s\\%s\\%s\\%s", m_curScenarioPackPath, m_localizedPath, m_assetPaths[dir], filename);
		if (c3files_PathIsValid(fullPath)) {
			
			strcpy(path, fullPath);
			return path;
		}
		sprintf(fullPath, "%s\\%s\\%s\\%s", m_curScenarioPackPath, m_defaultPath, m_assetPaths[dir], filename);
		
		if (c3files_PathIsValid(fullPath)) {
			
			strcpy(path, fullPath);
			return path;
		}
	}

	

	
	if (MakeAssetPath(fullPath, m_hdPath, m_dataPath, m_localizedPath, m_assetPaths[dir], filename)) {
		
		strcpy(path, fullPath);
		return path;
	}

	
	if (MakeAssetPath(fullPath, m_hdPath, m_dataPath, m_defaultPath, m_assetPaths[dir], filename)) {
		
		strcpy(path, fullPath);
		return path;
	}

	

	
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
        
		int len = strlen(filename);

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



BOOL CivPaths::FindPath(C3DIR dir, int num, MBCHAR *path)
{
	MBCHAR          tempPath[_MAX_PATH];
	
	Assert(path != NULL);
	Assert(dir < C3DIR_MAX);
	
	path[0] = 0;
	
	if (dir == C3DIR_DIRECT) {
		return(FALSE);
	}
	
	switch (num) {
	case 0: {
		if (m_curScenarioPath) {
			sprintf(path, "%s\\%s\\%s\\%s", m_curScenarioPath,
				m_localizedPath, m_assetPaths[dir], "");
		}
		return(TRUE);
			}
	case 1: {
		if (m_curScenarioPath) {
			sprintf(path, "%s\\%s\\%s\\%s", m_curScenarioPath,
				m_defaultPath, m_assetPaths[dir], "");
		}
		return(TRUE);
			}
		
	case 2: {
		if (m_curScenarioPath) {
			sprintf(path, "%s\\%s\\%s\\%s", m_curScenarioPackPath,
				m_localizedPath, m_assetPaths[dir], "");
		}
		return(TRUE);
			}
	case 3: {
		if (m_curScenarioPath) {
			sprintf(path, "%s\\%s\\%s\\%s", m_curScenarioPackPath,
				m_defaultPath, m_assetPaths[dir], "");
		}
		return(TRUE);
			}
		
	case 4: {
		sprintf(tempPath, "%s\\%s\\%s\\%s", m_hdPath,
			m_dataPath, m_localizedPath, m_assetPaths[dir]);
		if (_fullpath(path, tempPath, _MAX_PATH) == NULL) {
			path[0] = 0;
		}
		return(TRUE);
			}          
	case 5: {
		sprintf(tempPath, "%s\\%s\\%s\\%s", m_hdPath,
			m_dataPath, m_defaultPath, m_assetPaths[dir]);
		if (_fullpath(path, tempPath, _MAX_PATH) == NULL) {
			path[0] = 0;
		}
		return(TRUE);
			}
	case 6: {
		sprintf(tempPath, "%s\\%s\\%s\\%s", m_cdPath,
			m_dataPath, m_localizedPath, m_assetPaths[dir]);
		if (_fullpath(path, tempPath, _MAX_PATH) == NULL) {
			path[0] = 0;
		}
		return(TRUE);
			}
	case 7: {
		sprintf(tempPath, "%s\\%s\\%s\\%s", m_cdPath,
			m_dataPath, m_defaultPath, m_assetPaths[dir]);
		if (_fullpath(path, tempPath, _MAX_PATH) == NULL) {
			path[0] = 0;
		}
		return(TRUE);
			}
	}
	
	return(FALSE);
}



MBCHAR *CivPaths::GetSpecificPath(C3DIR dir, MBCHAR *path, BOOL local)
{
	Assert(path != NULL);
	if (path == NULL) return NULL;
	Assert(dir < C3DIR_MAX);
	if (dir >= C3DIR_MAX) return NULL;

	MBCHAR			fullPath[_MAX_PATH];
	MBCHAR			tempPath[_MAX_PATH];
	MBCHAR			*s;

	if (local) {
		sprintf(tempPath, "%s\\%s\\%s\\%s", m_hdPath, m_dataPath, m_localizedPath, m_assetPaths[dir]);
		s = _fullpath(fullPath, tempPath, _MAX_PATH);
		Assert(s);
		if (s) {
			strcpy(path, fullPath);
		}
		return path;
	} else {
		sprintf(tempPath, "%s\\%s\\%s\\%s", m_hdPath, m_dataPath, m_defaultPath, m_assetPaths[dir]);
		s = _fullpath(fullPath, tempPath, _MAX_PATH);
		Assert(s);
		if (s) {
			strcpy(path, fullPath);
		}
		return path;
	}

	return NULL;
}

MBCHAR *CivPaths::GetScenarioRootPath(MBCHAR *path)
{
	MBCHAR	temp[_MAX_PATH];
	MBCHAR	*s;

	s = _fullpath(temp, m_scenariosPath, _MAX_PATH);
	Assert(s);
	if (s) {
		strcpy(path, temp);
	}

	return path;
}


void CivPaths::SetCurScenarioPath(MBCHAR *path)
{
	if (m_curScenarioPath)
		delete[] m_curScenarioPath;
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
	if (m_curScenarioPath) {
		delete[] m_curScenarioPath;
		m_curScenarioPath = NULL;
	}
}



void CivPaths::SetCurScenarioPackPath(MBCHAR *path)
{
	if (m_curScenarioPackPath)
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
	if (m_curScenarioPackPath) {
		delete[] m_curScenarioPackPath;
		m_curScenarioPackPath = NULL;
	}
}





MBCHAR *CivPaths::GetDesktopPath(void)
{
	MBCHAR		tempStr[_MAX_PATH];
	HRESULT		hr;
	ITEMIDLIST	*idList;

	hr = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &idList); 
	Assert(hr == S_OK);

	if (hr != S_OK) 
		goto rootpath;

	if (!SHGetPathFromIDList(idList, tempStr))
		goto rootpath;

	
	goto finished;

rootpath:
	
	MBCHAR *s;
	s = _fullpath(tempStr, "\\", _MAX_PATH);
	Assert(s);
	if (!s) return NULL;

finished:
	strcpy(m_desktopPath, tempStr);
	return m_desktopPath;
}
