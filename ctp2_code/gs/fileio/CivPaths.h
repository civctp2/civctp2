











#pragma once
#ifndef __CIVPATHS_H__
#define __CIVPATHS_H__ 1

#include "c3files.h"











class CivPaths {
private:
	MBCHAR *m_hdPath;			
	MBCHAR *m_cdPath;			

	MBCHAR *m_defaultPath;		
	MBCHAR *m_localizedPath;	

	MBCHAR *m_dataPath;			
	MBCHAR *m_scenariosPath;	

	MBCHAR *m_savePath;			
	MBCHAR *m_saveGamePath;		
	MBCHAR *m_saveQueuePath;	
	MBCHAR *m_saveMPPath;		
	MBCHAR *m_saveSCENPath;		
	MBCHAR *m_saveMapPath;		
	MBCHAR *m_saveClipsPath;    

	MBCHAR *m_assetPaths[C3DIR_MAX]; 

	MBCHAR *m_curScenarioPath;	
	
	
	MBCHAR *m_curScenarioPackPath;
	
	
	MBCHAR m_desktopPath[_MAX_PATH];


public:
	
	CivPaths ();
	
	~CivPaths();

	
	void CreateSaveFolders(MBCHAR *path);

	
	void InitCDPath(void);

	
	MBCHAR *GetSavePath(C3SAVEDIR dir, MBCHAR *path);

	
	
	
	MBCHAR *FindFile(C3DIR dir, MBCHAR *filename, MBCHAR *path,
                     BOOL silent=0, BOOL check_prjfile=1);
	
	
	MBCHAR *GetSpecificPath(C3DIR dir, MBCHAR *path, BOOL local);

	
	MBCHAR *GetScenarioRootPath(MBCHAR *path);

	
	void	SetCurScenarioPath(MBCHAR *path);
	
	MBCHAR *GetCurScenarioPath(void);
	
	void	ClearCurScenarioPath(void);


	
	
	void	SetCurScenarioPackPath(MBCHAR *path);
	
	MBCHAR	*GetCurScenarioPackPath(void);
	
	void	ClearCurScenarioPackPath(void);

	
	
	
	
	
	
	
	
	
	
    BOOL FindPath(C3DIR dir, int num, MBCHAR *path);

	
	MBCHAR	*GetSavePathString(void) const { return m_savePath; }

	
	MBCHAR *GetDesktopPath(void);

protected:
	
	MBCHAR *MakeAssetPath(MBCHAR *fullPath, MBCHAR *s1, MBCHAR *s2, MBCHAR *s3, MBCHAR *s4, MBCHAR *s5);

	
	MBCHAR *MakeSavePath(MBCHAR *fullPath, MBCHAR *s1, MBCHAR *s2, MBCHAR *s3);

};







void CivPaths_InitCivPaths();



void CivPaths_CleanupCivPaths();






extern CivPaths *g_civPaths;

#endif
