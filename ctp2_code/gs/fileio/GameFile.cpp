//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Game file handling
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
// _NO_GAME_WATCH
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired multiple memory leaks.
// - Readded Activision patch new magic number 66.
// - Fixed autosave directory name for scenarios to match normal directory.
// - Fixed the scenario savegame bug (but not for autosave, that still needs to be done)
// - Replaced old civilisation database by new one. (Aug 20th 2005 Martin G�hmann)
// - Made progress bar more fluently. (Aug 22nd 2005 Martin G�hmann)
// - Removed old sprite state databases. (Aug 29th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3errors.h"
#include "c3files.h"

#include <zlib.h>

#include "BldQue.h"
#include "ConstDB.h"
#include "AdvanceRecord.h"
#include "BuildingRecord.h"
#include "Diffcly.h"
#include "DiffDB.h"
#include "TerrainRecord.h"
#include "CivPaths.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "gameinit.h"
#include "player.h"
#include "civarchive.h"
#include "gamefile.h"
#include "citydata.h"
#include "Diffcly.h"
#include "UnitData.h"
#include "RandGen.h"
#include "TradePool.h"
#include "TradeOfferPool.h"
#include "SelItem.h"
#include "gwdb.h"
#include "UVDB.h"
#include "CivilisationRecord.h"
#include "pollution.h"

#include "RoboInit.h"
#include "TurnCnt.h"
#include "installation.h"
#include "installationpool.h"

#include "TerrImprovePool.h"
#include "WonderRecord.h"
#include "Advances.h"
#include "Gold.h"
#include "Sci.h"
#include "TaxRate.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "Cell.h"
#include "DiplomaticRequestPool.h"
#include "CivilisationPool.h"
#include "MessagePool.h"
#include "AgreementPool.h"

#include "SlicEngine.h"
#include "profileDB.h"
#include "TopTen.h"
#include "AgeRecord.h"
#include "thronedb.h"
#include "conceptdb.h"
#include "civapp.h"
#include "StrDB.h"

#include "civscenarios.h"
#include "Wormhole.h"
#include "WonderTracker.h"
#include "AchievementTracker.h"
#include "PollutionDB.h"
#include "TradeBids.h"
#include "Exclusions.h"

#include "soundmanager.h"
#include "progresswindow.h"

#include "GameSettings.h"

#include "netshell_game.h"
#include "AgeRecord.h"

#include "pixelutils.h"

#include "ctpai.h"

#include "FeatTracker.h"

#include "EventTracker.h"

#include "civ3_main.h"

#ifndef _NO_GAME_WATCH

#include "GameWatch.h"

extern int g_gameWatchID;
#endif

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

sint32 g_showUnitLabels = FALSE;

sint32 g_startingPlayer = -1;

extern sint32 g_isGridOn;

extern ProgressWindow *g_theProgressWindow;
extern SoundManager *g_soundManager;
extern PointerList<Player> *g_deadPlayer;










#define k_GAME_MAGIC_VALUE		"CTP0066"







struct MagicValue {
	const char *string;
	sint32 version;
};

#define k_NUM_MAGIC_VALUES 18
static MagicValue s_magicValue[k_NUM_MAGIC_VALUES] = {
	{ "CTP0049", 49},
	{ "CTP0050", 50},
	{ "CTP0051", 51},
	{ "CTP0052", 52},
	{ "CTP0053", 53},
	{ "CTP0054", 54},
	{ "CTP0055", 55},
	{ "CTP0056", 56},
	{ "CTP0057", 57},
	{ "CTP0058", 58},
	{ "CTP0059", 59},
	{ "CTP0060", 60},
	{ "CTP0061", 61},
	{ "CTP0062", 62},
	{ "CTP0063", 63},
	{ "CTP0064", 64},
	{ "CTP0065", 65},
	{ "CTP0066", 66}
};

sint32 gamefile_CurrentVersion()
{
	return s_magicValue[k_NUM_MAGIC_VALUES - 1].version;
}

extern  CivPaths                    *g_civPaths;
extern  CivApp                      *g_civApp;

extern  World                       *g_theWorld;
extern  Player                      **g_player;
extern  UnitPool                    *g_theUnitPool;
extern  StringDB                    *g_theStringDB;
extern  ConceptDB                   *g_theConceptDB;
extern  ConstDB                     *g_theConstDB;
extern  ThroneDB                    *g_theThroneDB;
extern  DifficultyDB                *g_theDifficultyDB;
extern  RandomGenerator             *g_rand;
extern  TurnCount                   *g_turn;
extern  SelectedItem                *g_selected_item;
extern  PollutionDatabase           *g_thePollutionDB;
extern  GlobalWarmingDatabase       *g_theGWDB;
extern  OzoneDatabase               *g_theUVDB;
extern  Pollution                   *g_thePollution;
extern  TopTen                      *g_theTopTen;

extern  TerrainImprovementPool      *g_theTerrainImprovementPool;
extern  InstallationPool            *g_theInstallationPool;
extern  TurnCount                   *g_turn;
extern  DiplomaticRequestPool       *g_theDiplomaticRequestPool;
extern  CivilisationPool            *g_theCivilisationPool;
extern  AgreementPool               *g_theAgreementPool;
extern  MessagePool                 *g_theMessagePool;

extern  ProfileDB                   *g_theProfileDB;
extern  nf_GameSetup                g_gamesetup;

extern sint32                       g_is565Format;

sint32 g_saveFileVersion = -1;
sint32 g_startInfoType = STARTINFOTYPE_NONE;
sint32 g_isScenario = FALSE;
sint32 g_useScenarioCivs = 2;

MBCHAR g_scenarioName[k_SCENARIO_NAME_MAX];




void GameFile::RestoreGame(const MBCHAR *filename)
{
	GameFile *game;

	game = new GameFile;
	game->Restore(filename);

	delete game;
}

void GameFile::RestoreScenarioGame(MBCHAR *name)
{
	GameFile *game;

	game = new GameFile;
	game->Restore(name);
	delete game;
}

void GameFile::SaveGame(const MBCHAR *filename, SaveInfo *info)
{
	GameFile *game;

	game = new GameFile;
	game->Save(filename, info);

	delete game;
}


GameFile::GameFile(void)
{

}

static uint32 CompressData(uint8 *inbuf, size_t insize,
                           uint8 **outbuf, size_t *outsize)
{
	int err;
	uLong tsize = (uLong)(((double)insize * 1.01) + 12.5);

	*outbuf = new uint8[tsize];

	err = compress2(*outbuf, &tsize, inbuf, insize, Z_DEFAULT_COMPRESSION);
	*outsize = tsize;

	return(err == Z_OK);
}

uint32 GameFile::SaveDB(CivArchive &archive)
{
	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		330 );

	const char *s = g_theStringDB->GetNameStr("LOADING");

	g_theProgressWindow->StartCountingTo( 10, s );




	g_theProgressWindow->StartCountingTo( 20 );


	g_theProgressWindow->StartCountingTo( 30 );


	g_theProgressWindow->StartCountingTo( 40 );


	g_theProgressWindow->StartCountingTo( 50 );


	g_theProgressWindow->StartCountingTo( 60 );


	g_theProgressWindow->StartCountingTo( 70 );


	g_theProgressWindow->StartCountingTo( 80 );


	g_theProgressWindow->StartCountingTo( 90 );


	g_theProgressWindow->StartCountingTo( 100 );

	g_theProgressWindow->StartCountingTo( 110 );







	g_theProgressWindow->StartCountingTo( 120 );

	g_theProgressWindow->StartCountingTo( 130 );

	g_theProgressWindow->StartCountingTo( 140 );


	g_theProgressWindow->StartCountingTo( 150 );


	g_theProgressWindow->StartCountingTo( 160 );


	g_theProgressWindow->StartCountingTo( 170 );

	g_theDifficultyDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 180 );

	g_theConstDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 190 );


	g_theProgressWindow->StartCountingTo( 200 );

	g_theThroneDB->Serialize( archive );

	g_theProgressWindow->StartCountingTo( 210 );

	g_theConceptDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 220 );


	g_theProgressWindow->StartCountingTo( 230 );


	g_theProgressWindow->StartCountingTo( 240 );


	g_theProgressWindow->StartCountingTo( 250 );

	g_theProgressWindow->StartCountingTo( 260 );

	g_theProgressWindow->StartCountingTo( 270 );

	g_thePollutionDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 280 );

	g_theGWDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 290 );

	g_theUVDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 300 );

	// SaveDB is only called if CivApp::m_saveDBInGameFile is TRUE
	// fortunatly this is never the case in CTP2 and can be removed
	// without any problems
//	g_theCivilisationDB->Serialize(archive);

	g_theProgressWindow->StartCountingTo( 310 );


	g_theProgressWindow->StartCountingTo( 320 );

	g_theProgressWindow->StartCountingTo( 330 );

	g_theProgressWindow->StartCountingTo( 340 );


	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}

uint32 GameFile::Save(const MBCHAR *filepath, SaveInfo *info)
{

	FILE	*fpSave = NULL;

	size_t ulLen;
	uint32 n ;

	sint32	i;

	clock_t	start, finish;

	CivArchive	archive;

	MBCHAR	sHeader[_MAX_PATH];

	size_t compressedSize = 0;
	uint8	*compressedData = NULL;

	start = clock();

	archive.SetStore();

	Assert(k_MAX_PLAYERS!=0);
	if (k_MAX_PLAYERS <= 0) {
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_INCORRECT_NUM_PLAYERS");
		return (GAMEFILE_ERR_STORE_FAILED);
	}

	bool showProgress = !strstr(filepath, "AUTOSAVE");

#define PROGRESS(i) if(showProgress) { g_theProgressWindow->StartCountingTo(i); }

	if(showProgress) {
		ProgressWindow::BeginProgress(
			g_theProgressWindow,
			"InitProgressWindow",
			520 );
	}

	const char *s = g_theStringDB->GetNameStr("SAVING");
	if(showProgress)
		g_theProgressWindow->StartCountingTo( 100, s );

	if (g_civApp->SaveDBInGameFile())
		SaveDB(archive);


	archive<<World_World_GetVersion();
	archive<<Player_Player_GetVersion();
	archive<<SelectedItem_GetVersion();
	archive<<Advances_Advances_GetVersion();

	archive<<BldQue_BuildQueue_GetVersion();
	archive<<CityData_CityData_GetVersion();
	archive<<Diffcly_Difficulty_GetVersion();
	archive<<GameObj_GameObj_GetVersion();
	archive<<Gold_Gold_GetVersion();
	archive<<ID_ID_GetVersion();
	archive<<ObjPool_ObjPool_GetVersion();
	archive<<Sci_Science_GetVersion();
	archive<<TaxRate_TaxRate_GetVersion();
	archive<<Unit_Unit_GetVersion();
	archive<<UnitData_UnitData_GetVersion();
	archive<<UnitPool_UnitPool_GetVersion();
	archive<<Cell_CELL_GetVersion();
	archive<<MapPoint_MapPoint_GetVersion();


	g_rand->Serialize(archive);

	PROGRESS( 110 );

	g_theGameSettings->Serialize(archive);

	PROGRESS( 120 );

	BOOL saveEverything = TRUE;




	if (g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS) {
		saveEverything = FALSE;
	}

#if 0

	g_theWorld->ClearStartingPoints();
	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && g_player[i]->m_all_units->Num() > 0) {
			g_theWorld->AddStartingPoint(g_player[i]->m_all_units->Access(0).RetPos(),
										 g_player[i]->m_civilisation->GetCivilisation());
		}
	}
#endif

	g_theWorld->Serialize(archive);

	PROGRESS( 130 );

	if(saveEverything)
		g_turn->Serialize(archive);

	PROGRESS( 140 );

	if(saveEverything)
		g_selected_item->Serialize(archive);

	PROGRESS( 150 );

	if(saveEverything)
		g_theUnitPool->Serialize(archive);

	PROGRESS( 160 );

	if(saveEverything)
		g_theArmyPool->Serialize(archive);

	PROGRESS( 170 );

	PROGRESS( 180 );

	if(saveEverything)
		g_theTradePool->Serialize(archive);

	PROGRESS( 190 );





	PROGRESS( 200 );

	if(saveEverything)
		g_thePollution->Serialize(archive);

	PROGRESS( 210 );





	PROGRESS( 220 );

	g_slicEngine->Serialize(archive);

	PROGRESS( 230 );

	if(saveEverything)
		g_theTerrainImprovementPool->Serialize(archive);

	PROGRESS( 240 );





	PROGRESS( 250 );

	if(saveEverything)
		g_theCivilisationPool->Serialize(archive);

	PROGRESS( 260 );





	PROGRESS( 270 );

	if(saveEverything)
		g_theMessagePool->Serialize(archive);

	PROGRESS( 280 );

	if(saveEverything)
		g_theInstallationPool->Serialize(archive);

	PROGRESS( 290 );










	PROGRESS( 300 );

	if(saveEverything)
		g_theWonderTracker->Serialize(archive);

	PROGRESS( 310 );





	PROGRESS( 320 );


		g_exclusions->Serialize(archive);

	PROGRESS( 330 );

	if(saveEverything)
		g_featTracker->Serialize(archive);





	if(saveEverything)
		g_eventTracker->Serialize(archive);

	PROGRESS( 340 );


	for (i=0; i<k_MAX_PLAYERS; i++) {
		sint32 playerAlive = g_player[i] != NULL;

		if (g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS) {

			playerAlive = 0;
		}
		archive << playerAlive;
		if(playerAlive)
			g_player[i]->Serialize(archive);
	}

	if (g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS) {

		sint32 num = 0;
		archive << num;
	} else {
		sint32 num = g_deadPlayer->GetCount();
		archive << num;
		PointerList<Player>::Walker walk(g_deadPlayer);
		while(walk.IsValid()) {
			walk.GetObj()->Serialize(archive);
			walk.Next();
		}
	}

	PROGRESS( 350 );










	if(saveEverything)
		CtpAi::Save(archive);

	PROGRESS( 360 );

	fpSave=c3files_fopen(C3DIR_DIRECT, (MBCHAR *)filepath, "wb");
	if (fpSave == NULL)
	{
		c3errors_ErrorDialogFromDB("SAVE_ERROR", "SAVE_FAILED_TO_SAVE");

		if(showProgress) {
			ProgressWindow::EndProgress( g_theProgressWindow );
		}
		return (GAMEFILE_ERR_STORE_FAILED);
	}

	PROGRESS( 370 );

	strcpy(sHeader, k_GAME_MAGIC_VALUE);
	n = c3files_fwrite(sHeader, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), fpSave);
	if (n!=sizeof(k_GAME_MAGIC_VALUE))
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
	}












	BOOL createdInfo = FALSE;
	if (info == NULL) {
		createdInfo = TRUE;

		info = new SaveInfo();
		GetExtendedInfoFromProfile(info);
	}

	PROGRESS( 380 );

	SaveExtendedGameInfo(fpSave, info);

	PROGRESS( 390 );

	if (createdInfo)
		delete info;

	ulLen = archive.StreamLen();
	n = c3files_fwrite(&ulLen, sizeof(ulLen), 1, fpSave);
	if (n!=1)
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
	}

	PROGRESS( 500 );

	if (!CompressData(archive.GetStream(), ulLen,
					  &compressedData, &compressedSize))
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
	}

	PROGRESS( 510 );

	n = c3files_fwrite(&compressedSize, sizeof(compressedSize), 1, fpSave);
	if (n!=1)
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
		}

	PROGRESS( 520 );

	n = c3files_fwrite(compressedData, sizeof(uint8), compressedSize, fpSave);
	if (n != compressedSize)
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
		}

	delete compressedData;

	c3files_fclose(fpSave);

#ifndef _NO_GAME_WATCH

	char gameWatchFilename[1024];
	sprintf(gameWatchFilename, "%s.gw", filepath);


	gameWatch.SaveGame(g_gameWatchID, gameWatchFilename);
#endif

	finish = clock();
	DPRINTF(k_DBG_FILE, ("Time to save game data = %4.2f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC));

	if(showProgress) {
		ProgressWindow::EndProgress( g_theProgressWindow );
	}

	return (GAMEFILE_ERR_STORE_OK);
}














uint32 GameFile::Restore(const MBCHAR *filepath)
	{
	FILE	*fpLoad;

	uint32	ver, n;
	uLong ulLen;

	clock_t	start, finish;

	MBCHAR sHeader[_MAX_PATH];

	CivArchive	archive;

	uLong	tlen = 0;
	uLong	compressedSize = 0;
	uint8	*compressedData = NULL;

	start = clock();

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		1090 );

	const char *s = g_theStringDB->GetNameStr("LOADING");

	g_theProgressWindow->StartCountingTo( 10, s );

	fpLoad=c3files_fopen(C3DIR_DIRECT, (MBCHAR *)filepath, "rb");
	if (fpLoad == NULL) {
		c3errors_ErrorDialog("LOAD_ERROR", "LOAD_FAILED_TO_LOAD_GAME");
		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 20 );

	n = c3files_fread(sHeader, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), fpLoad);
	Assert(n==sizeof(k_GAME_MAGIC_VALUE));
	if (n!=sizeof(k_GAME_MAGIC_VALUE))
		{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_UNABLE_TO_READ_GAME");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}

	g_theProgressWindow->StartCountingTo( 30 );

	g_saveFileVersion = -1;
	sint32 i;
	for(i = 0; i < k_NUM_MAGIC_VALUES; i++) {
		if(strcmp(sHeader, s_magicValue[i].string) == 0) {
			g_saveFileVersion = s_magicValue[i].version;
			break;
		}
	}

	g_theProgressWindow->StartCountingTo( 40 );

	if(g_saveFileVersion < 0) {
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_NOT_A_SAVEGAME_FILE");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 50 );

	SaveInfo *info = new SaveInfo();

	LoadExtendedGameInfo(fpLoad, info);

	g_theProgressWindow->StartCountingTo( 60 );

	SetProfileFromExtendedInfo(info);

	delete info;

	g_theProgressWindow->StartCountingTo( 70 );

	n = c3files_fread(&ulLen, sizeof(ulLen), 1, fpLoad);
	Assert(n==1);
	if (n!=1)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 80 );

	archive.SetSize(ulLen);
	archive.SetLoad();

	g_theProgressWindow->StartCountingTo( 90 );

	n = c3files_fread(&compressedSize, sizeof(compressedSize), 1, fpLoad);
	Assert(n==1);
	if (n!=1)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 100 );

	compressedData = new uint8[compressedSize];
	n = c3files_fread(compressedData, sizeof(uint8), compressedSize, fpLoad);
	if (n!=compressedSize)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 110 );

	c3files_fclose(fpLoad);

	g_theProgressWindow->StartCountingTo( 120 );

	tlen = ulLen;
	int q;
	if (((q = uncompress(archive.GetStream(), &tlen, compressedData, compressedSize)) != Z_OK) ||
		(tlen != ulLen)) {
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");
		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	delete compressedData;

	g_theProgressWindow->StartCountingTo( 130 );

	archive>>ver;
	if (ver!=World_World_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 140 );

	archive>>ver;
	if (ver!=Player_Player_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 150 );

	archive>>ver;
	if (ver != SelectedItem_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 160 );

	archive>>ver;
	if (ver!=Advances_Advances_GetVersion())
		{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}

	g_theProgressWindow->StartCountingTo( 170 );
#if 0

	archive>>ver;
	if (ver!=ArmyList_ArmyList_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}
#endif

	g_theProgressWindow->StartCountingTo( 180 );

	archive>>ver;
	if (ver!=BldQue_BuildQueue_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 190 );

	archive>>ver;
	if (ver!=CityData_CityData_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 200 );

	archive>>ver;
	if (ver!=Diffcly_Difficulty_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 210 );

	archive>>ver;
	if (ver!=GameObj_GameObj_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 220 );

	archive>>ver;
	if (ver!=Gold_Gold_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 230 );

	archive>>ver;
	if (ver!=ID_ID_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 240 );

	archive>>ver;
	if (ver!=ObjPool_ObjPool_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 250 );

	archive>>ver;
	if (ver!=Sci_Science_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 260 );

	archive>>ver;
	if (ver!=TaxRate_TaxRate_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 270 );

	archive>>ver;
	if (ver!=Unit_Unit_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 280 );

	archive>>ver;
	if (ver!=UnitData_UnitData_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 290 );

	archive>>ver;
	if (ver!=UnitPool_UnitPool_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 300 );

	archive>>ver;
	if (ver!=Cell_CELL_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 310 );

	archive>>ver;
	if (ver!=MapPoint_MapPoint_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
	}

	g_theProgressWindow->StartCountingTo( 1080 );

	g_civApp->InitializeGame(archive);

	g_theProgressWindow->StartCountingTo( 1090 );

#ifndef _NO_GAME_WATCH

	char gameWatchFilename[1024];
	sprintf(gameWatchFilename, "%s.gw", filepath);

	g_gameWatchID = gameWatch.LoadGame(gameWatchFilename);
#endif

	finish = clock();
	DPRINTF(k_DBG_FILE, ("Time to load game data = %4.2f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC));

	ProgressWindow::EndProgress( g_theProgressWindow );

	return (GAMEFILE_ERR_LOAD_OK);
}






BOOL GameFile::LoadExtendedGameInfo(FILE *saveFile, SaveInfo *info)
{
	sint32		n;

	n = c3files_fread(info->gameName, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->leaderName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->civName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->note, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	info->radarMapWidth = 0;
	info->radarMapHeight = 0;
	if (info->radarMapData) {
		delete[] info->radarMapData;
		info->radarMapData = NULL;
	}

	n = c3files_fread(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth)) {
		c3files_fclose(saveFile);
		return FALSE;
	}
	n = c3files_fread(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight)) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	if (info->radarMapHeight > 0 && info->radarMapWidth > 0) {
		info->radarMapData = new Pixel16[info->radarMapWidth * info->radarMapHeight];
		n = c3files_fread(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapWidth * info->radarMapHeight, saveFile);
		if (n != (sint32)(info->radarMapWidth * info->radarMapHeight * sizeof(Pixel16))) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		if (!g_is565Format) {
			for (sint32 i=0; i<info->radarMapWidth * info->radarMapHeight; i++) {
				info->radarMapData[i] = pixelutils_Convert565to555(info->radarMapData[i]);
			}
		}
	}

	info->powerGraphWidth = 0;
	info->powerGraphHeight = 0;
	if (info->powerGraphData) {
		delete[] info->powerGraphData;
		info->powerGraphData = NULL;
	}

	n = c3files_fread(&info->powerGraphWidth, sizeof(uint8), sizeof(info->powerGraphWidth), saveFile);
	if (n != sizeof(info->powerGraphWidth)) {
		c3files_fclose(saveFile);
		return FALSE;
	}
	n = c3files_fread(&info->powerGraphHeight, sizeof(uint8), sizeof(info->powerGraphHeight), saveFile);
	if (n != sizeof(info->powerGraphHeight)) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	if (info->powerGraphHeight > 0 && info->powerGraphWidth > 0) {
		info->powerGraphData = new Pixel16[info->powerGraphWidth * info->powerGraphHeight];
		n = c3files_fread(info->powerGraphData, sizeof(uint8),
							sizeof(Pixel16) * info->powerGraphWidth * info->powerGraphHeight, saveFile);
		if (n != (sint32)(info->powerGraphWidth * info->powerGraphHeight * sizeof(Pixel16))) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		if (!g_is565Format) {
			for (sint32 i=0; i<info->powerGraphWidth * info->powerGraphHeight; i++) {
				info->powerGraphData[i] = pixelutils_Convert565to555(info->powerGraphData[i]);
			}
		}
	}

	sint32 numPlayers;
	n = c3files_fread(&numPlayers, sizeof(uint8), sizeof(sint32), saveFile);
	if (n != sizeof(sint32)) {
		c3files_fclose(saveFile);
		return FALSE;
	}
	info->numCivs = numPlayers;

    sint32 has_robot;
	for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
        c3files_fread(&has_robot, sizeof(uint8), sizeof(sint32), saveFile);

        if (has_robot) {
		    n = c3files_fread(info->civList[i], sizeof(uint8), k_MAX_NAME_LEN, saveFile);
		    if (n != k_MAX_NAME_LEN) {
			    c3files_fclose(saveFile);
			    return FALSE;
		    }
        } else {
            info->civList[i][0] = '\0';
        }

		sint32 civindex;
		GUID guid;
		n = c3files_fread(&civindex, 1, sizeof(sint32), saveFile);
		if(n != sizeof(sint32)) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		n = c3files_fread(&guid, 1, sizeof(guid), saveFile);
		if(n != sizeof(guid)) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		info->networkGUID[i].civIndex = civindex;
		info->networkGUID[i].guid = guid;
	}

	n = c3files_fread(&info->gameSetup, sizeof(nf_GameSetup), 1, saveFile);
	if (n != 1) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	NETFunc::Session *s = (NETFunc::Session *)&info->gameSetup;
	dp_session_t *sess =
		(dp_session_t *)((uint8*)s + sizeof(NETFunc::Key));
	sess->sessionType = GAMEID;

	n = c3files_fread(&info->options, sizeof(SaveInfo::OptionScreenSettings), 1, saveFile);
	if (n != 1) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	info->loadType = SAVEINFOLOAD_EXTENDED;


	n = c3files_fread(&info->isScenario, sizeof(info->isScenario), 1, saveFile);
	if(n != 1) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(&info->startInfoType, sizeof(info->startInfoType), 1, saveFile);
	if(n != 1) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(&info->numPositions, sizeof(info->numPositions), 1, saveFile);
	if(n != 1) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(&info->positions, sizeof(StartingPosition), k_MAX_START_POINTS, saveFile);
	if(n != k_MAX_START_POINTS) {
		c3files_fclose(saveFile);
		return FALSE;
	}




	MBCHAR name[k_SCENARIO_NAME_MAX];
	n = c3files_fread(name, sizeof(MBCHAR), k_SCENARIO_NAME_MAX, saveFile);
	if(n != k_SCENARIO_NAME_MAX) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	if (info->scenarioName) {
		delete[] info->scenarioName;
		info->scenarioName = NULL;
	}


	if (strlen(name) > 0) {
		info->scenarioName = new MBCHAR[strlen(name)+1];
		strcpy(info->scenarioName, name);
	}


	if (g_saveFileVersion >= 47) {


		n = c3files_fread(info->playerCivIndexList, sizeof(CIV_INDEX), k_MAX_PLAYERS, saveFile);
		if(n != k_MAX_PLAYERS) {
			return FALSE;
		}
	}

	if (g_saveFileVersion >= 50) {
		n = c3files_fread(&info->showLabels, sizeof(info->showLabels), 1, saveFile);
		if(n != 1) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		n = c3files_fread(&info->startingPlayer, sizeof(info->startingPlayer), 1, saveFile);
		if(n != 1) {
			c3files_fclose(saveFile);
			return FALSE;
		}
	} else {
		info->showLabels = FALSE;
		info->startingPlayer = -1;
	}

	return TRUE;
}







BOOL GameFile::LoadBasicGameInfo(FILE *saveFile, SaveInfo *info)
{
	sint32		n;

	n = c3files_fread(info->gameName, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->leaderName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->civName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->note, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	info->loadType = SAVEINFOLOAD_BASIC;




	if (g_saveFileVersion >= 42) {


		n = c3files_fread(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
		if (n != sizeof(info->radarMapWidth)) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		n = c3files_fread(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
		if (n != sizeof(info->radarMapHeight)) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		if (info->radarMapHeight > 0 && info->radarMapWidth > 0) {
			c3files_fseek(saveFile,
						sizeof(Pixel16) * info->radarMapWidth * info->radarMapHeight,
						SEEK_CUR);
		}

		n = c3files_fread(&info->powerGraphWidth, sizeof(uint8), sizeof(info->powerGraphWidth), saveFile);
		if (n != sizeof(info->powerGraphWidth)) {
			c3files_fclose(saveFile);
			return FALSE;
		}
		n = c3files_fread(&info->powerGraphHeight, sizeof(uint8), sizeof(info->powerGraphHeight), saveFile);
		if (n != sizeof(info->powerGraphHeight)) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		if (info->powerGraphHeight > 0 && info->powerGraphWidth > 0) {
			c3files_fseek(saveFile,
						sizeof(Pixel16) * info->powerGraphWidth * info->powerGraphHeight,
						SEEK_CUR);
		}

		sint32 numPlayers;
		n = c3files_fread(&numPlayers, sizeof(uint8), sizeof(sint32), saveFile);
		if (n != sizeof(sint32)) {
			c3files_fclose(saveFile);
			return FALSE;
		}
		info->numCivs = numPlayers;

		sint32 has_robot;
		for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
			c3files_fread(&has_robot, sizeof(uint8), sizeof(sint32), saveFile);

			if (has_robot) {
				n = c3files_fread(info->civList[i], sizeof(uint8), k_MAX_NAME_LEN, saveFile);
				if (n != k_MAX_NAME_LEN) {
					c3files_fclose(saveFile);
					return FALSE;
				}
			} else {
				info->civList[i][0] = '\0';
			}

			sint32 civindex;
			GUID guid;
			n = c3files_fread(&civindex, 1, sizeof(sint32), saveFile);
			if(n != sizeof(sint32)) {
				c3files_fclose(saveFile);
				return FALSE;
			}

			n = c3files_fread(&guid, 1, sizeof(guid), saveFile);
			if(n != sizeof(guid)) {
				c3files_fclose(saveFile);
				return FALSE;
			}

			info->networkGUID[i].civIndex = civindex;
			info->networkGUID[i].guid = guid;
		}

		n = c3files_fread(&info->gameSetup, sizeof(nf_GameSetup), 1, saveFile);
		if (n != 1) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		n = c3files_fread(&info->options, sizeof(SaveInfo::OptionScreenSettings), 1, saveFile);
		if (n != 1) {
			c3files_fclose(saveFile);
			return FALSE;
		}




		n = c3files_fread(&info->isScenario, sizeof(info->isScenario), 1, saveFile);
		if(n != 1) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		n = c3files_fread(&info->startInfoType, sizeof(info->startInfoType), 1, saveFile);
		if(n != 1) {
			c3files_fclose(saveFile);
			return FALSE;
		}
	}




	if (g_saveFileVersion >= 46) {

		MBCHAR name[k_SCENARIO_NAME_MAX];
		n = c3files_fread(name, sizeof(MBCHAR), k_SCENARIO_NAME_MAX, saveFile);
		if(n != k_SCENARIO_NAME_MAX) {
			c3files_fclose(saveFile);
			return FALSE;
		}

		if (info->scenarioName) {
			delete[] info->scenarioName;
			info->scenarioName = NULL;
		}


		if (strlen(name) > 0) {
			info->scenarioName = new MBCHAR[strlen(name)+1];
			strcpy(info->scenarioName, name);
		}
	}


	if (g_saveFileVersion >= 47) {


		n = c3files_fread(info->playerCivIndexList, sizeof(CIV_INDEX), k_MAX_PLAYERS, saveFile);
		if(n != k_MAX_PLAYERS) {
                    c3files_fclose(saveFile);
                    return FALSE;
		}
	}

	return TRUE;
}







void GameFile::SaveExtendedGameInfo(FILE *saveFile, SaveInfo *info)
{
	const char *functionName = "GameFile::SaveExtendedGameInfo";
	const char *errorString = "Unable to write save file.";

	sint32		n;




	n = c3files_fwrite(info->gameName, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->leaderName, sizeof(MBCHAR), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->civName, sizeof(MBCHAR), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->note, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	n = c3files_fwrite(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	if (info->radarMapWidth > 0 && info->radarMapHeight > 0) {

		if (!g_is565Format) {
			for (sint32 i=0; i<info->radarMapWidth * info->radarMapHeight; i++) {
				Pixel16		pixel = info->radarMapData[i];
				info->radarMapData[i] = ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);

			}
		}

		n = c3files_fwrite(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth,
							saveFile);
		if (n != (sint32)(sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth)) {
			c3errors_FatalDialog(functionName, errorString);
			return;
		}
	}

	n = c3files_fwrite(&info->powerGraphWidth, sizeof(uint8), sizeof(info->powerGraphWidth), saveFile);
	if (n != sizeof(info->powerGraphWidth)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	n = c3files_fwrite(&info->powerGraphHeight, sizeof(uint8), sizeof(info->powerGraphHeight), saveFile);
	if (n != sizeof(info->powerGraphHeight)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	if (info->powerGraphHeight > 0 && info->powerGraphWidth > 0) {

		if (!g_is565Format) {
			for (sint32 i=0; i<info->powerGraphWidth * info->powerGraphHeight; i++) {
				Pixel16		pixel = info->powerGraphData[i];
				info->powerGraphData[i] = ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);
			}
		}

		n = c3files_fwrite(info->powerGraphData, sizeof(uint8),
							sizeof(Pixel16) * info->powerGraphHeight * info->powerGraphWidth,
							saveFile);
		if (n != (sint32)(sizeof(Pixel16) * info->powerGraphHeight * info->powerGraphWidth)) {
			c3errors_FatalDialog(functionName, errorString);
			return;
		}
	}

	sint32		numPlayers = k_MAX_PLAYERS;

	n = c3files_fwrite(&numPlayers, sizeof(uint8), sizeof(sint32), saveFile);
	if (n != sizeof(sint32)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
		MBCHAR civName[k_MAX_NAME_LEN];

	    sint32 has_player;
		GUID guid;
		sint32 civindex;
		if (g_player[i]) {
            has_player = 1;
            c3files_fwrite(&has_player, sizeof(uint8), sizeof(sint32), saveFile);

		    g_player[i]->GetPluralCivName(civName);

		    n = c3files_fwrite(civName, sizeof(MBCHAR), k_MAX_NAME_LEN, saveFile);
		    if (n != k_MAX_NAME_LEN) {
			    c3errors_FatalDialog(functionName, errorString);
			    return;
		    }
			guid = g_player[i]->m_networkGuid;
			civindex = g_player[i]->m_civilisation->GetCivilisation();
        } else {
            has_player = 0;
            c3files_fwrite(&has_player, sizeof(uint8), sizeof(sint32), saveFile);
			memset(&guid, 0, sizeof(guid));
			civindex = -1;
        }
		c3files_fwrite(&civindex, 1, sizeof(sint32), saveFile);
		c3files_fwrite(&guid, 1, sizeof(GUID), saveFile);
	}

	n = c3files_fwrite(&info->gameSetup, sizeof(nf_GameSetup), 1, saveFile);
	if (n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->options, sizeof(SaveInfo::OptionScreenSettings), 1, saveFile);
	if (n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}




	n = c3files_fwrite(&info->isScenario, sizeof(info->isScenario), 1, saveFile);
	if(n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->startInfoType, sizeof(info->startInfoType), 1, saveFile);
	if(n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->numPositions, sizeof(info->numPositions), 1, saveFile);
	if(n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->positions, sizeof(StartingPosition), k_MAX_START_POINTS, saveFile);
	if(n != k_MAX_START_POINTS) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}




	MBCHAR name[k_SCENARIO_NAME_MAX];
	memset(name, 0, k_SCENARIO_NAME_MAX);

	if (strlen(g_scenarioName) > 0) {
		strcpy(name, g_scenarioName);
	}

	n = c3files_fwrite(name, sizeof(MBCHAR), k_SCENARIO_NAME_MAX, saveFile);
	if(n != k_SCENARIO_NAME_MAX) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}




	n = c3files_fwrite(info->playerCivIndexList, sizeof(CIV_INDEX), k_MAX_PLAYERS, saveFile);
	if(n != k_MAX_PLAYERS) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}





	n = c3files_fwrite(&info->showLabels, sizeof(info->showLabels), 1, saveFile);
	if(n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->startingPlayer, sizeof(info->startingPlayer), 1, saveFile);
	if(n != 1) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
}

void GameFile::SetProfileFromExtendedInfo(SaveInfo *info)
{
	Assert(g_theProfileDB);
	if (!g_theProfileDB) return;

	Assert(info);
	if (!info) return;

	if(!g_isScenario) {
		g_theProfileDB->SetGameName(info->gameName);
	} else {
		size_t const	SAVE_LEADER_NAME_SIZE	= 6;
		MBCHAR	name[SAVE_LEADER_NAME_SIZE + 1];
		strncpy(name, g_theProfileDB->GetLeaderName(), SAVE_LEADER_NAME_SIZE);
		name[SAVE_LEADER_NAME_SIZE] = 0;
		// TODO: check if this is OK for japanese.
		g_theProfileDB->SetGameName(name);
	}


	if (info->startInfoType != STARTINFOTYPE_CIVS &&
		info->startInfoType != STARTINFOTYPE_POSITIONSFIXED) {
		g_theProfileDB->SetLeaderName(info->leaderName);
		g_theProfileDB->SetCivName(info->civName);
		g_theProfileDB->SetSaveNote(info->note);
	}

	nf_GameSetup temp = g_gamesetup;
	g_gamesetup = info->gameSetup;

	memcpy(
		g_gamesetup.GetTribeSlots(),
		temp.GetTribeSlots(),
		8  * sizeof( TribeSlot ) );

	g_gamesetup.SetLaunched(true);
	g_gamesetup.Pack();

	g_theProfileDB->SetTutorialAdvice(info->options.tutorialadvice);
















	g_isGridOn = info->options.grid;





	Assert( g_soundManager != NULL );
	if ( !g_soundManager ) return;

	g_soundManager->SetAutoRepeat(info->options.autoRepeat);
	if ( info->options.randomOrder )
		g_soundManager->SetMusicStyle(MUSICSTYLE_RANDOM);
	else
		g_soundManager->SetMusicStyle(MUSICSTYLE_NONE);
	if ( info->options.musicOn )
		g_soundManager->EnableMusic();
	else
		g_soundManager->DisableMusic();

	if(g_saveFileVersion >= 42) {
        if(!info->isScenario){// exclude starting new scenarios
            if (info->scenarioName != NULL && strlen(info->scenarioName) > 0) {//same as in beginloadprocess
		        strcpy(g_scenarioName,info->scenarioName);
			}
		}
		g_isScenario = info->isScenario;
		g_startInfoType = info->startInfoType;
	} else {
		g_isScenario = FALSE;
		g_startInfoType = STARTINFOTYPE_NONE;
	}

	if (g_saveFileVersion >= 50)
	{
		g_showUnitLabels = info->showLabels;
		g_startingPlayer = info->startingPlayer;
	} else {
		g_showUnitLabels = FALSE;
		g_startingPlayer = -1;
	}
}

void GameFile::GetExtendedInfoFromProfile(SaveInfo *info)
{
	Assert(g_theProfileDB);
	if (!g_theProfileDB) return;

	Assert(info);
	if (!info) return;

	strcpy(info->gameName, g_theProfileDB->GetGameName());
	strcpy(info->leaderName, g_theProfileDB->GetLeaderName());
	strcpy(info->civName, g_theProfileDB->GetCivName());
	strcpy(info->note, g_theProfileDB->GetSaveNote());

	info->gameSetup = g_gamesetup;

	memset(
		info->gameSetup.GetTribeSlots(),
		0,
		8  * sizeof( TribeSlot ) );

	if(g_isScenario) {
		memset(info->gameSetup.GetSavedTribeSlots(),
			   0,
			   8 * sizeof(TribeSlot));
	}

	info->options.tutorialadvice = g_theProfileDB->IsTutorialAdvice();
	info->options.leftrightclickmove = g_theProfileDB->IsUseLeftClick();
	info->options.autocycleturn = g_theProfileDB->IsAutoTurnCycle();
	info->options.autocycleunits = g_theProfileDB->IsAutoSelectFirstUnit();
	info->options.battleview = g_theProfileDB->IsZoomedCombatAlways();
	info->options.monument = g_theProfileDB->IsThroneRoom();


	info->options.walk = g_theProfileDB->IsUnitAnim();
	info->options.goods = g_theProfileDB->IsGoodAnim();

	info->options.trade = g_theProfileDB->IsTradeAnim();
	info->options.wonder = g_theProfileDB->IsWonderMovies();
	info->options.library = g_theProfileDB->IsLibraryAnim();
	info->options.message = g_theProfileDB->IsBounceMessage();

	info->options.movie = g_theProfileDB->IsFullScreenMovies();

	info->options.grid = g_isGridOn;

	info->options.sfxVolume = g_theProfileDB->GetSFXVolume();
	info->options.musicVolume = g_theProfileDB->GetMusicVolume();
	info->options.voiceVolume = g_theProfileDB->GetVoiceVolume();

	Assert( g_soundManager != NULL );
	if ( !g_soundManager ) return;

	info->options.autoRepeat = g_soundManager->IsAutoRepeat();
	info->options.randomOrder = g_soundManager->GetMusicStyle() == MUSICSTYLE_RANDOM;
	info->options.musicOn = g_soundManager->IsMusicEnabled();






	info->isScenario = g_isScenario;
	info->startInfoType = (STARTINFOTYPE)g_startInfoType;

	memset(info->positions, 0, sizeof(info->positions));

	if(g_theWorld) {
		info->numPositions = g_theWorld->GetNumStartingPositions();
		sint32 i;
		for(i = 0; i < info->numPositions; i++) {
			info->positions[i].point = g_theWorld->GetStartingPoint(i);
			info->positions[i].civIndex = g_theWorld->GetStartingPointCiv(i);
		}
	}

	info->showLabels = g_showUnitLabels;
	info->startingPlayer = g_startingPlayer;
}









BOOL GameFile::ValidateGameFile(MBCHAR *path, SaveInfo *info)
{
	FILE		*saveFile = NULL;
	MBCHAR		filepath[_MAX_PATH];
	MBCHAR		header[_MAX_PATH];
	sint32		n;

	sprintf(filepath, "%s%s%s", path, FILE_SEP, info->fileName);
        //printf("%s %dL: filepath %s", __FILE__, __LINE__, filepath);
	saveFile = c3files_fopen(C3DIR_DIRECT, (MBCHAR *)filepath, "rb");
	if (saveFile == NULL)
		return FALSE;

	n = c3files_fread(header, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), saveFile);
	if (n!=sizeof(k_GAME_MAGIC_VALUE)) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	g_saveFileVersion = -1;

	sint32 i;
	for(i = 0; i < k_NUM_MAGIC_VALUES; i++) {
		if(strcmp(header, s_magicValue[i].string) == 0) {
			g_saveFileVersion = s_magicValue[i].version;
			break;
		}
	}

	if(g_saveFileVersion < 0) {
		c3files_fclose(saveFile);
		return FALSE;
	}







	BOOL success = LoadBasicGameInfo(saveFile, info);
        if (success)
            c3files_fclose(saveFile);

	return success;

}






BOOL GameFile::FetchExtendedSaveInfo(MBCHAR *fullPath, SaveInfo *info)
{
	FILE		*saveFile = NULL;
	MBCHAR		header[_MAX_PATH];
	sint32		n;

	saveFile = c3files_fopen(C3DIR_DIRECT, (MBCHAR *)fullPath, "rb");
	if (saveFile == NULL)
		return FALSE;

	n = c3files_fread(header, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), saveFile);
	if (n!=sizeof(k_GAME_MAGIC_VALUE)) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	g_saveFileVersion = -1;

	sint32 i;
	for(i = 0; i < k_NUM_MAGIC_VALUES; i++) {
		if(strcmp(header, s_magicValue[i].string) == 0) {
			g_saveFileVersion = s_magicValue[i].version;
			break;
		}
	}

	if(g_saveFileVersion < 0) {
		c3files_fclose(saveFile);
		return FALSE;
	}


	BOOL success = LoadExtendedGameInfo(saveFile, info);

	c3files_fclose(saveFile);

	return success;
}

PointerList<GameInfo> *GameFile::BuildSaveList(C3SAVEDIR dir)
{
	PointerList<GameInfo> *list;

	list = new PointerList<GameInfo>;

#ifdef WIN32
	WIN32_FIND_DATA		fileData;
	HANDLE				lpDirList;
#else
	DIR			*d;
	struct stat		tmpstat;
	struct dirent		*dent = 0;
#endif
	MBCHAR				dirPath[_MAX_PATH],
					path[_MAX_PATH];

	if (!g_civPaths->GetSavePath(dir, dirPath)) return list;

#ifdef WIN32
	sprintf(path, "%s%s*.*", dirPath);
	lpDirList = FindFirstFile(path, &fileData);

	if (lpDirList == INVALID_HANDLE_VALUE) return list;
#else
	d = opendir(dirPath);
	if (!d) return list;
#endif

	GameInfo			*gameInfo;

	do {
#ifndef WIN32
		dent = readdir(d);
		if (!dent) continue;

		snprintf(path, sizeof(path), "%s%s%s", dirPath, FILE_SEP, dent->d_name);
		if (stat(path, &tmpstat))
			continue;

		if (S_ISDIR(tmpstat.st_mode)) {
			MBCHAR *name = dent->d_name;
#else
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			MBCHAR *name = fileData.cFileName;
#endif
			if (!strcmp(name, ".")) continue;
			if (!strcmp(name, "..")) continue;

			gameInfo = new GameInfo();

			strcpy(gameInfo->name, name);

			sprintf(gameInfo->path, "%s%s%s", dirPath, FILE_SEP, name);

			gameInfo->files = new PointerList<SaveInfo>;

			list->AddTail(gameInfo);

#ifdef WIN32
			WIN32_FIND_DATA		fileData2;
			HANDLE				lpFileList;

			sprintf(path, "%s%s*.*", gameInfo->path, FILE_SEP);

			lpFileList = FindFirstFile(path, &fileData2);
			if (lpFileList == INVALID_HANDLE_VALUE) continue;
#else
			DIR *dir2 = opendir(path);
			struct dirent *dent2 = 0;

			if (!dir2) continue;
#endif
			do {
#ifndef WIN32
				dent2 = readdir(dir2);
				if (!dent2) continue;

				snprintf(path, sizeof(path), "%s%s%s", gameInfo->path, FILE_SEP, dent2->d_name);
				if (stat(path, &tmpstat)) continue;

				if (!S_ISDIR(tmpstat.st_mode)) {
					name = dent2->d_name;
#else
				if (!(fileData2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					name = fileData2.cFileName;
#endif

					SaveInfo		*saveInfo = new SaveInfo();

					strcpy(saveInfo->fileName, name);

					sprintf(saveInfo->pathName, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);

					if (!ValidateGameFile(gameInfo->path, saveInfo)) {
						delete saveInfo;
						continue;
					}

					gameInfo->files->AddTail(saveInfo);
				}
#ifdef WIN32
			} while (FindNextFile(lpFileList, &fileData2));
			FindClose(lpFileList);
		}
	} while(FindNextFile(lpDirList,&fileData));
	FindClose(lpDirList);
#else
			} while (dent2);
			closedir(dir2);
		}
	} while(dent);
	closedir(d);
#endif

	return list;
}

//----------------------------------------------------------------------------
//
// Name       : SaveInfo::SaveInfo
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
SaveInfo::SaveInfo()
{
	gameName[0] = '\0';
	fileName[0] = '\0';
	pathName[0] = '\0';
	leaderName[0] = '\0';
	civName[0] = '\0';
	note[0] = '\0';

	radarMapWidth = 0;
	radarMapHeight = 0;
	radarMapData = NULL;

	powerGraphWidth = 0;
	powerGraphHeight = 0;
	powerGraphData = NULL;

	numCivs = 0;
	for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
		civList[i][0] = '\0';
		memset(&networkGUID[i], 0, sizeof(CivGuid));

		playerCivIndexList[i] = (CIV_INDEX)0;
	}









	isScenario = FALSE;
	startInfoType = STARTINFOTYPE_NONE;
	numPositions = 0;

	loadType = SAVEINFOLOAD_NONE;


	scenarioName = NULL;

}

//----------------------------------------------------------------------------
//
// Name       : SaveInfo::SaveInfo
//
// Description: Copy constructor
//
// Parameters : copyMe			: pointer to object to copy
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : * The pointer data in copyMe that has been allocated on the
//                heap is not shared, but freshly allocated. This enables
//                both objects to be deallocated independently.
//
//----------------------------------------------------------------------------
SaveInfo::SaveInfo(SaveInfo *copyMe)
{
	memcpy(this, copyMe, sizeof(SaveInfo));

	sint32 numPixels, numBytes;

	if (copyMe->radarMapWidth > 0 &&
		copyMe->radarMapHeight > 0 &&
		copyMe->radarMapData != NULL) {

		numPixels = copyMe->radarMapWidth * copyMe->radarMapHeight;
		numBytes = numPixels * sizeof(Pixel16);

		radarMapData = new Pixel16[numPixels];
		memcpy(radarMapData, copyMe->radarMapData, numBytes);
	}

	if (copyMe->powerGraphWidth > 0 &&
		copyMe->powerGraphHeight > 0 &&
		copyMe->powerGraphData != NULL) {

		numPixels = copyMe->powerGraphWidth * copyMe->powerGraphHeight;
		numBytes = numPixels * sizeof(Pixel16);

		powerGraphData = new Pixel16[numPixels];
		memcpy(powerGraphData, copyMe->powerGraphData, numBytes);
	}

	if (copyMe->scenarioName)
	{
		size_t const	sizeHeap	= strlen(copyMe->scenarioName) + 1;
		scenarioName				= new MBCHAR[sizeHeap];
		memcpy(scenarioName, copyMe->scenarioName, sizeHeap);
	}
}

//----------------------------------------------------------------------------
//
// Name       : SaveInfo::~SaveInfo
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : * powerGraphData, radarMapData, and scenarioName are allocated
//                with new [] in GameFile::LoadBasicGameInfo and
//                GameFile::LoadBasicGameInfo.
//
//----------------------------------------------------------------------------
SaveInfo::~SaveInfo()
{
	delete [] powerGraphData;
	delete [] radarMapData;
	delete [] scenarioName;
}

//----------------------------------------------------------------------------

GameInfo::GameInfo()
{
	name[0] = '\0';
	path[0] = '\0';
	files = NULL;
}

GameInfo::~GameInfo()
{
	if (files) {
		files->DeleteAll();
	}
	delete files;
}





SaveMapInfo::SaveMapInfo()
{
	gameMapName[0] = '\0';
	fileName[0] = '\0';
	pathName[0] = '\0';
	note[0] = '\0';

	radarMapWidth = 0;
	radarMapHeight = 0;
	radarMapData = NULL;
}

GameMapInfo::GameMapInfo()
{
	name[0] = '\0';
	path[0] = '\0';
	files = NULL;
}


#define k_GAMEMAP_MAGIC_VALUE		"CTPMAP__"

void GameMapFile::RestoreGameMap(const MBCHAR *filename)
	{
	GameMapFile	*gameMap;

    gameMap = new GameMapFile;

	gameMap->Restore(filename);

	delete gameMap;
	}

void GameMapFile::SaveGameMap(const MBCHAR *filename, SaveMapInfo *info)
	{
	GameMapFile	*gameMap;

	gameMap = new GameMapFile;
	gameMap->Save(filename, info);
    delete gameMap;
	}

GameMapFile::GameMapFile(void)
	{

	}

uint32 GameMapFile::Save(const MBCHAR *filepath, SaveMapInfo *info)
	{

	FILE	*fpSave = NULL;

	uint32	ulLen,
			n;

	clock_t	start, finish;

	CivArchive	archive;

	MBCHAR	sHeader[_MAX_PATH];

	start = clock();

	archive.SetStore();

	archive<<World_World_GetVersion();




	g_theWorld->SerializeJustMap(archive);


	fpSave=c3files_fopen(C3DIR_DIRECT, (MBCHAR *)filepath, "wb");
	if (fpSave == NULL)
		{
		c3errors_ErrorDialogFromDB("SAVE_ERROR", "SAVE_FAILED_TO_SAVE");

		return (GAMEFILE_ERR_STORE_FAILED);
		}

	strcpy(sHeader, k_GAMEMAP_MAGIC_VALUE);
	n = c3files_fwrite(sHeader, sizeof(uint8), sizeof(k_GAMEMAP_MAGIC_VALUE), fpSave);
	if (n!=sizeof(k_GAMEMAP_MAGIC_VALUE))
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
		}












	BOOL createdInfo = FALSE;
	if (info == NULL) {
		createdInfo = TRUE;

		info = new SaveMapInfo();
		GetExtendedInfoFromProfile(info);
	}

	SaveExtendedGameMapInfo(fpSave, info);

	if (createdInfo)
		delete info;

	ulLen = archive.StreamLen();
	n = c3files_fwrite(&ulLen, sizeof(ulLen), 1, fpSave);
	if (n!=1)
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
		}

	n = c3files_fwrite(archive.GetStream(), sizeof(uint8), ulLen, fpSave);
	if (n != ulLen)
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return (GAMEFILE_ERR_STORE_FAILED);
		}

	c3files_fclose(fpSave);

	finish = clock();
	DPRINTF(k_DBG_FILE, ("Time to save gamemap data = %4.2f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC));

	return (GAMEFILE_ERR_STORE_OK);
	}

uint32 GameMapFile::Restore(const MBCHAR *filepath)
	{
	FILE	*fpLoad;

	uint32	ver,
			ulLen,
			n;

	clock_t	start, finish;

	MBCHAR sHeader[_MAX_PATH];

	CivArchive	archive;

	start = clock();

	fpLoad=c3files_fopen(C3DIR_DIRECT, (MBCHAR *)filepath, "rb");
	if (fpLoad == NULL) {
		c3errors_ErrorDialog("LOAD_ERROR", "LOAD_FAILED_TO_LOAD_GAME");
		return (GAMEFILE_ERR_LOAD_FAILED);
	}


	n = c3files_fread(sHeader, sizeof(uint8), sizeof(k_GAMEMAP_MAGIC_VALUE), fpLoad);
	Assert(n==sizeof(k_GAMEMAP_MAGIC_VALUE));
	if (n!=sizeof(k_GAMEMAP_MAGIC_VALUE))
		{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_UNABLE_TO_READ_GAME");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}

	if (strcmp(sHeader, k_GAMEMAP_MAGIC_VALUE))
		{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_NOT_A_SAVEGAME_FILE");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}


	SaveMapInfo *info = new SaveMapInfo();

	LoadExtendedGameMapInfo(fpLoad, info);

	SetProfileFromExtendedInfo(info);

	delete info;

	n = c3files_fread(&ulLen, sizeof(ulLen), 1, fpLoad);
	Assert(n==1);
	if (n!=1)
		{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}

	archive.SetSize(ulLen);
	archive.SetLoad();

	n = c3files_fread(archive.GetStream(), sizeof(uint8), ulLen, fpLoad);
	if (n!=ulLen)
		{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}

	c3files_fclose(fpLoad);

	archive>>ver;
	if (ver!=World_World_GetVersion())
		{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return (GAMEFILE_ERR_LOAD_FAILED);
		}


	if ( g_theWorld ) delete g_theWorld;
	g_theWorld = new World( archive, TRUE );
	Assert( g_theWorld != NULL );
	if ( !g_theWorld ) return FALSE;

	finish = clock();
	DPRINTF(k_DBG_FILE, ("Time to load gamemap data = %4.2f seconds\n", (double)(finish - start) / CLOCKS_PER_SEC));

	return (GAMEFILE_ERR_LOAD_OK);
}

BOOL GameMapFile::LoadExtendedGameMapInfo(FILE *saveFile, SaveMapInfo *info)
{
	sint32		n;

	n = c3files_fread(info->gameMapName, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(info->note, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	n = c3files_fread(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth)) {
		c3files_fclose(saveFile);
		return FALSE;
	}
	n = c3files_fread(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight)) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	if (info->radarMapHeight > 0 && info->radarMapWidth > 0) {
		info->radarMapData = new Pixel16[info->radarMapWidth * info->radarMapHeight];
		n = c3files_fread(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapWidth * info->radarMapHeight, saveFile);
		if (n != (sint32)(info->radarMapWidth * info->radarMapHeight * sizeof(Pixel16))) {
			c3files_fclose(saveFile);
			return FALSE;
		}
	}

	return TRUE;
}

void GameMapFile::SaveExtendedGameMapInfo(FILE *saveFile, SaveMapInfo *info)
{
	const char	*functionName = "GameMapFile::SaveExtendedGameMapInfo";
	const char	*errorString = "Unable to write savemap file.";

	sint32		n;




	n = c3files_fwrite(info->gameMapName, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->note, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	n = c3files_fwrite(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight)) {
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	if (info->radarMapWidth > 0 && info->radarMapHeight > 0) {
		n = c3files_fwrite(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth,
							saveFile);
		if (n != (sint32)(sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth)) {
			c3errors_FatalDialog(functionName, errorString);
			return;
		}
	}
}

void GameMapFile::SetProfileFromExtendedInfo(SaveMapInfo *info)
{
	Assert(g_theProfileDB);
	if (!g_theProfileDB) return;

	Assert(info);
	if (!info) return;




}

void GameMapFile::GetExtendedInfoFromProfile(SaveMapInfo *info)
{
	Assert(g_theProfileDB);
	if (!g_theProfileDB) return;

	Assert(info);
	if (!info) return;









}


BOOL GameMapFile::ValidateGameMapFile(MBCHAR *path, SaveMapInfo *info)
{
	FILE		*saveFile = NULL;
	MBCHAR		filepath[_MAX_PATH];
	MBCHAR		header[_MAX_PATH];
	sint32		n;

	sprintf(filepath, "%s%s%s", path, FILE_SEP, info->fileName);

	saveFile = c3files_fopen(C3DIR_DIRECT, (MBCHAR *)filepath, "rb");
	if (saveFile == NULL)
		return FALSE;

	n = c3files_fread(header, sizeof(uint8), sizeof(k_GAMEMAP_MAGIC_VALUE), saveFile);
	if (n!=sizeof(k_GAMEMAP_MAGIC_VALUE)) {
		c3files_fclose(saveFile);
		return FALSE;
	}

	if (strcmp(header, k_GAMEMAP_MAGIC_VALUE)) {
		c3files_fclose(saveFile);
		return FALSE;
	}


	BOOL success = LoadExtendedGameMapInfo(saveFile, info);

	c3files_fclose(saveFile);

	return success;

}

PointerList<GameMapInfo> *GameMapFile::BuildSaveMapList(C3SAVEDIR dir)
{
	PointerList<GameMapInfo> *list;

	list = new PointerList<GameMapInfo>;

#ifdef WIN32
	WIN32_FIND_DATA		fileData;
	HANDLE				lpDirList;
#else
	DIR			*d;
	struct stat		tmpstat;
	struct dirent		*dent = 0;
#endif
	MBCHAR				dirPath[_MAX_PATH],
						path[_MAX_PATH];

	if (!g_civPaths->GetSavePath(dir, dirPath)) return list;

#ifdef WIN32
	sprintf(path, "%s%s*.*", dirPath, FILE_SEP);

	lpDirList = FindFirstFile(path, &fileData);

	if (lpDirList == INVALID_HANDLE_VALUE) return list;
#else
	d = opendir(dirPath);
	if (!dir) return list;
#endif

	GameMapInfo			*gameInfo;

	do {
#ifndef WIN32
		dent = readdir(d);
		if (!dent) continue;

		snprintf(path, sizeof(path), "%s%s%s", dirPath, FILE_SEP, dent->d_name);
		if (!stat(path, &tmpstat))
			continue;

		if (S_ISDIR(tmpstat.st_mode)) {
			MBCHAR *name = dent->d_name;
#else
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			MBCHAR *name = fileData.cFileName;
#endif
			if (!strcmp(name, ".")) continue;
			if (!strcmp(name, "..")) continue;

			gameInfo = new GameMapInfo();
			strcpy(gameInfo->name, name);
			sprintf(gameInfo->path, "%s%s%s", dirPath, FILE_SEP, name);
			gameInfo->files = new PointerList<SaveMapInfo>;

			list->AddTail(gameInfo);

#ifdef WIN32
			WIN32_FIND_DATA		fileData2;
			HANDLE				lpFileList;

			sprintf(path, "%s%s*.*", gameInfo->path, FILE_SEP);

			lpFileList = FindFirstFile(path, &fileData2);
			if (lpFileList == INVALID_HANDLE_VALUE) continue;
#else
			DIR *dir2 = opendir(path);
			struct dirent *dent2 = 0;

			if (!dir2) continue;
#endif
			do {
#ifndef WIN32
				dent2 = readdir(dir2);
				if (!dent2) continue;

				snprintf(path, sizeof(path), "%s%s%s", gameInfo->path, FILE_SEP, dent2->d_name);
				if (!stat(path, &tmpstat)) continue;

				if (!S_ISDIR(tmpstat.st_mode)) {
					name = dent2->d_name;
#else
				if (!(fileData2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					name = fileData2.cFileName;
#endif
					SaveMapInfo		*saveInfo = new SaveMapInfo();

					strcpy(saveInfo->fileName, name);

					sprintf(saveInfo->pathName, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);

					if (!ValidateGameMapFile(gameInfo->path, saveInfo)) {
						delete saveInfo;
						continue;
					}

					gameInfo->files->AddTail(saveInfo);
				}
#ifdef WIN32
			} while (FindNextFile(lpFileList, &fileData2));
			FindClose(lpFileList);
		}
	} while(FindNextFile(lpDirList,&fileData));
	FindClose(lpDirList);
#else
			} while (dent2);
			closedir(dir2);
		}
	} while(dent);
	closedir(d);
#endif

	return list;
}
