
#pragma once
#ifndef __GAMEFILE_H__
#define __GAMEFILE_H__

#include "pixeltypes.h"
#include "pointerlist.h"
#include "ns_gamesetup.h"
#include "MapPoint.h"
#include "StartingPosition.h"
#include "civscenarios.h"
#include "CivilisationPool.h"

enum GAMEFILE_ERR
	{
	GAMEFILE_ERR_LOAD_OK,
	GAMEFILE_ERR_LOAD_FAILED,
	GAMEFILE_ERR_INCORRECT_VERSION,
	GAMEFILE_ERR_STORE_OK,
	GAMEFILE_ERR_STORE_FAILED,
	GAMEFILE_ERR_MAX
	} ;

struct CivGuid {
	sint32 civIndex;
	GUID guid;
};

enum SAVEINFOLOAD {
	SAVEINFOLOAD_NONE = -1,

	SAVEINFOLOAD_BASIC,
	SAVEINFOLOAD_EXTENDED,

	SAVEINFOLOAD_MAX
};

class SaveInfo;

class SaveInfo {
public:
	SaveInfo();
	SaveInfo(SaveInfo *copyMe);
	~SaveInfo();

	MBCHAR					gameName[_MAX_PATH];		
	MBCHAR					fileName[_MAX_PATH];		
	MBCHAR					pathName[_MAX_PATH];		
	MBCHAR					leaderName[k_MAX_NAME_LEN];	
	MBCHAR					civName[k_MAX_NAME_LEN];	
	MBCHAR					note[_MAX_PATH];			
	sint32					radarMapWidth;				
	sint32					radarMapHeight;
	Pixel16					*radarMapData;
	sint32					powerGraphWidth;			
	sint32					powerGraphHeight;
	Pixel16					*powerGraphData;
	sint32					numCivs;					
	MBCHAR					civList[k_MAX_PLAYERS][k_MAX_NAME_LEN]; 
	CivGuid                 networkGUID[k_MAX_PLAYERS]; 

	
	nf_GameSetup gameSetup; 

	struct OptionScreenSettings
	{
		
		BOOL	tutorialadvice;
		BOOL	leftrightclickmove;
		BOOL	autocycleturn;
		BOOL	autocycleunits;
		BOOL	battleview;
		BOOL	monument;

		
		BOOL	walk;
		BOOL	goods;
		BOOL	attack;
		BOOL	trade;
		BOOL	wonder;
		BOOL	library;
		BOOL	message;
		BOOL	water;
		BOOL	movie;
		BOOL	grid;

		
		sint32	sfxVolume;
		sint32	musicVolume;
		sint32	voiceVolume;

		
		BOOL	autoRepeat;
		BOOL	randomOrder;
		BOOL	musicOn;
	} options;

	
	BOOL isScenario;
	STARTINFOTYPE startInfoType;
	sint32 numPositions;
	StartingPosition positions[k_MAX_START_POINTS];

	SAVEINFOLOAD	loadType;

	
	
	
	MBCHAR			*scenarioName;

	
	
	CIV_INDEX		playerCivIndexList[k_MAX_PLAYERS];

	

	
	BOOL	showLabels;
	sint32	startingPlayer;		
};

class GameInfo {
public:
	GameInfo();
	~GameInfo();
	MBCHAR					name[_MAX_PATH];			
	MBCHAR					path[_MAX_PATH];			
	PointerList<SaveInfo>	*files;						
};


class CivArchive;

class GameFile
	{
	private:
		FILE	*m_ds ;												

	public:

		GameFile() ;

		uint32 SaveDB(CivArchive &archive);
		uint32 Save(const MBCHAR *filepath, SaveInfo *info) ;
		uint32 Restore(const MBCHAR *filepath) ;

		static BOOL LoadExtendedGameInfo(FILE *saveFile, SaveInfo *info);
		static BOOL LoadBasicGameInfo(FILE *saveFile, SaveInfo *info);
		static void SaveExtendedGameInfo(FILE *saveFile, SaveInfo *info);

		static void SetProfileFromExtendedInfo(SaveInfo *info);
		static void GetExtendedInfoFromProfile(SaveInfo *info);

		static void RestoreGame(const MBCHAR *filename) ;
		static void SaveGame(const MBCHAR *filename, SaveInfo *info) ;
		static void RestoreScenarioGame(MBCHAR *name);

		static BOOL ValidateGameFile(MBCHAR *path, SaveInfo *info);
		static BOOL FetchExtendedSaveInfo(MBCHAR *path, SaveInfo *info);
		static PointerList<GameInfo> *BuildSaveList(C3SAVEDIR dir);
	} ;




class SaveMapInfo {
public:
	SaveMapInfo();

	MBCHAR					gameMapName[_MAX_PATH];
	MBCHAR					fileName[_MAX_PATH];
	MBCHAR					pathName[_MAX_PATH];
	MBCHAR					note[_MAX_PATH];
	sint32					radarMapWidth;
	sint32					radarMapHeight;
	Pixel16					*radarMapData;
};

class GameMapInfo {
public:
	GameMapInfo();
	MBCHAR					name[_MAX_PATH];
	MBCHAR					path[_MAX_PATH];
	PointerList<SaveMapInfo>	*files;
};

class GameMapFile
	{
	private:
		FILE	*m_ds ;

	public:

		GameMapFile() ;

		uint32 Save(const MBCHAR *filepath, SaveMapInfo *info) ;
		uint32 Restore(const MBCHAR *filepath) ;

		static BOOL LoadExtendedGameMapInfo(FILE *saveFile, SaveMapInfo *info);
		static void SaveExtendedGameMapInfo(FILE *saveFile, SaveMapInfo *info);

		static void SetProfileFromExtendedInfo(SaveMapInfo *info);
		static void GetExtendedInfoFromProfile(SaveMapInfo *info);

		static void RestoreGameMap(const MBCHAR *filename) ;
		static void SaveGameMap(const MBCHAR *filename, SaveMapInfo *info) ;

		static BOOL ValidateGameMapFile(MBCHAR *path, SaveMapInfo *info);
		static PointerList<GameMapInfo> *BuildSaveMapList(C3SAVEDIR dir);
	} ;

extern sint32 g_saveFileVersion;
extern sint32 g_startInfoType;
extern sint32 g_isScenario;
extern sint32 g_useScenarioCivs;
extern MBCHAR g_scenarioName[k_SCENARIO_NAME_MAX];


extern sint32 g_showUnitLabels;
extern sint32 g_startingPlayer;

sint32 gamefile_CurrentVersion();

#endif
