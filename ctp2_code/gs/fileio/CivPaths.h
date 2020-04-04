//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added option to use multiple data directories.
// - FindFile can ignore files in scenario paths. (9-Apr-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __CIVPATHS_H__
#define __CIVPATHS_H__ 1

#include "c3files.h"
#include <vector>	// list did not work: crashes on begin() for empty list

class CivPaths {
private:
	MBCHAR *m_hdPath;
	MBCHAR *m_cdPath;

	MBCHAR *m_defaultPath;
	MBCHAR *m_localizedPath;

	MBCHAR *m_dataPath;			                    // original data path (...\ctp2_data)
	std::vector<MBCHAR const *> m_extraDataPaths;   // searched before m_dataPath
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

	CivPaths(AUI_ERRCODE &errcode);

	virtual ~CivPaths();

	void CreateSaveFolders(const MBCHAR *path);

	void InitCDPath(void);

	MBCHAR *GetSavePath(C3SAVEDIR dir, MBCHAR *path);

	MBCHAR *FindFile(C3DIR dir, const MBCHAR *filename, MBCHAR *path,
	                 bool silent = false, bool check_prjfile = true, bool checkScenario = true, bool checkLocalizedPath = true);

	MBCHAR *GetSpecificPath(C3DIR dir, MBCHAR *path, BOOL local);

	MBCHAR *GetScenarioRootPath(MBCHAR *path);

	void	SetCurScenarioPath(const MBCHAR *path);

	MBCHAR *GetCurScenarioPath(void);

	void	ClearCurScenarioPath(void);

	void	SetCurScenarioPackPath(const MBCHAR *path);

	MBCHAR	* GetCurScenarioPackPath(void);

	void	ClearCurScenarioPackPath(void);

	bool        FindPath(C3DIR dir, int num, MBCHAR *path);

	MBCHAR *    GetSavePathString(void) const { return m_savePath; }

	MBCHAR *    GetDesktopPath(void);

	std::vector<MBCHAR const *> const &
	            GetExtraDataPaths(void) const;
	void        InsertExtraDataPath(MBCHAR const * path);
	void        ResetExtraDataPaths(void);
	void        ReplaceFileSeperator(MBCHAR* path);

protected:

	MBCHAR *    MakeAssetPath
	(
	    MBCHAR *        fullPath,
	    MBCHAR const *  s1,
	    MBCHAR const *  s2,
	    MBCHAR const *  s3,
	    MBCHAR const *  s4,
	    MBCHAR const *  s5
	) const;

	MBCHAR *    MakeSavePath(MBCHAR *fullPath, MBCHAR *s1, MBCHAR *s2, MBCHAR *s3);
};

void CivPaths_InitCivPaths();

void CivPaths_CleanupCivPaths();

extern CivPaths *g_civPaths;

#endif
