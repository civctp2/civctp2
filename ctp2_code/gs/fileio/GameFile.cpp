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
// - Replaced old civilisation database by new one. (Aug 20th 2005 Martin Gühmann)
// - Made progress bar more fluently. (Aug 22nd 2005 Martin Gühmann)
// - Removed old sprite state databases. (Aug 29th 2005 Martin Gühmann)
// - Removed old difficulty database. (April 29th 2006 Martin Gühmann)
// - Removed old pollution database. (July 15th 2006 Martin Gühmann)
// - Removed old gobal warming database. (July 15th 2006 Martin Gühmann)
// - Removed old concept database. (31-Mar-2007 Martin Gühmann)
// - Removed old const database. (5-Aug-2007 Martin Gühmann)
// - Replaced CIV_INDEX by sint32. (2-Jan-2008 Martin Gühmann)
// - Make the Linux version loading and producing Windows compatible
//   savegames. (16-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "gamefile.h"

#include "AchievementTracker.h"
#include "AdvanceRecord.h"
#include "Advances.h"
#include "AgeRecord.h"
#include "AgreementPool.h"         // g_theAgreementPool
#include <algorithm>
#include "ArmyPool.h"
#include "BldQue.h"
#include "BuildingRecord.h"
#include "c3errors.h"
#include "c3files.h"
#include "Cell.h"
#include "citydata.h"
#include "civ3_main.h"
#include "civapp.h"
#include "civarchive.h"
#include "CivilisationPool.h"       // g_theCivilisationPool
#include "CivilisationRecord.h"
#include "CivPaths.h"               // g_civPaths
#include "civscenarios.h"
#include "ctpai.h"
#include "Diffcly.h"
#include "DiplomaticRequestPool.h"  // g_theDiplomaticRequestPool
#include "EventTracker.h"
#include "Exclusions.h"
#include "FeatTracker.h"
#include "gameinit.h"
#include "GameSettings.h"
#include "Gold.h"
#include "installation.h"
#include "installationpool.h"       // g_theInstallationPool
#include "MessagePool.h"            // g_theMessagePool
#include "netshell_game.h"
#include "pixelutils.h"
#include "player.h"                 // g_player
#include "pollution.h"
#include "profileDB.h"              // g_theProfileDB
#include "progresswindow.h"
#include "RandGen.h"                // g_rand
#include "RoboInit.h"
#include "Sci.h"
#include "SelItem.h"                // g_selected_item
#include "SlicEngine.h"
#include "soundmanager.h"           // g_soundManager
#include "StrDB.h"                  // g_theStringDB
#include "TaxRate.h"
#include "TerrainRecord.h"
#include "TerrImprovePool.h"        // g_theTerrainImprovementPool
#include "thronedb.h"               // g_theThroneDB
#include "TopTen.h"
#include "TradeBids.h"
#include "TradeOfferPool.h"
#include "TradePool.h"
#include "TurnCnt.h"                // g_turn
#include "UnitData.h"
#include "UnitPool.h"               // g_theUnitPool
#include "UVDB.h"
#include "WonderRecord.h"
#include "WonderTracker.h"
#include "World.h"                  // g_theWorld
#include "Wormhole.h"
#include <zlib.h>

#ifndef WIN32
#include <sys/types.h>
#include <dirent.h>
#endif

extern  CivApp                      *g_civApp;
extern  OzoneDatabase               *g_theUVDB;
extern  Pollution                   *g_thePollution;
extern  TopTen                      *g_theTopTen;
extern  nf_GameSetup                g_gamesetup;
extern sint32                       g_is565Format;
extern sint32                       g_isGridOn;
extern ProgressWindow *             g_theProgressWindow;
extern PointerList<Player> *        g_deadPlayer;

#if USE_FORMAT_67
#define k_GAME_MAGIC_VALUE		"CTP0067"
#else
#define k_GAME_MAGIC_VALUE		"CTP0066"
#endif

struct MagicValue
{
	char *string;
	sint32 version;
};

#if USE_FORMAT_67
#define k_NUM_MAGIC_VALUES 19
#else
#define k_NUM_MAGIC_VALUES 18
#endif
MagicValue s_magicValue[k_NUM_MAGIC_VALUES] =
{
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
	{ "CTP0063", 63},   // Activision Alexander the Great scenario
	{ "CTP0064", 64},
	{ "CTP0065", 65},   // Activision CTP2 unpatched
#if USE_FORMAT_67
	{ "CTP0066", 66},   // Activision CTP2 patched
	{ "CTP0067", 67},   // Apolyton
#else
	{ "CTP0066", 66}
#endif
};

sint32 gamefile_CurrentVersion()
{
	return s_magicValue[k_NUM_MAGIC_VALUES - 1].version;
}

sint32 g_saveFileVersion = -1;
sint32 g_startInfoType = STARTINFOTYPE_NONE;
sint32 g_isScenario = FALSE;
sint32 g_useScenarioCivs = 2;
sint32 g_showUnitLabels = FALSE;
sint32 g_startingPlayer = -1;

MBCHAR g_scenarioName[k_SCENARIO_NAME_MAX];

void GameFile::RestoreGame(MBCHAR const * name)
{
	GameFile().Restore(name);
}

void GameFile::RestoreScenarioGame(MBCHAR const * name)
{
	GameFile().Restore(name);
}

void GameFile::SaveGame(const MBCHAR *filename, SaveInfo *info)
{
	GameFile().Save(filename, info);
}

GameFile::GameFile(void)
{
}

static uint32 CompressData(uint8 *inbuf, size_t insize,
                           uint8 **outbuf, size_t *outsize)
{
	uLong tsize = (uLong)(((double)insize * 1.01) + 12.5);

	*outbuf = new uint8[tsize];

	int err = compress2(*outbuf, &tsize, inbuf, insize, Z_DEFAULT_COMPRESSION);
	*outsize = tsize;

	return (err == Z_OK);
}

uint32 GameFile::Save(const MBCHAR *filepath, SaveInfo *info)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	clock_t start = clock();
#endif

	CivArchive	archive;
	archive.SetStore();

	Assert(k_MAX_PLAYERS > 0);
	if (k_MAX_PLAYERS <= 0) {
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_INCORRECT_NUM_PLAYERS");
		return GAMEFILE_ERR_STORE_FAILED;
	}

	bool showProgress = !strstr(filepath, "AUTOSAVE");

#define PROGRESS(i) if(showProgress) { g_theProgressWindow->StartCountingTo(i); }

	if (showProgress)
	{
		ProgressWindow::BeginProgress(g_theProgressWindow, "InitProgressWindow", 520);
		g_theProgressWindow->StartCountingTo(100, g_theStringDB->GetNameStr("SAVING"));
	}

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

	bool const  saveEverything =
	    !g_isScenario || (g_startInfoType == STARTINFOTYPE_NOLOCS);

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

	if (g_exclusions)
	{
		g_exclusions->Serialize(archive);
	}
	else
	{
		Exclusions().Serialize(archive);
	}

	PROGRESS( 330 );

	if(saveEverything)
		g_featTracker->Serialize(archive);

	if(saveEverything)
		g_eventTracker->Serialize(archive);

	PROGRESS( 340 );

	for (sint32 i=0; i<k_MAX_PLAYERS; i++)
	{
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

	FILE *  fpSave = c3files_fopen(C3DIR_DIRECT, filepath, "wb");
	if (!fpSave)
	{
		c3errors_ErrorDialogFromDB("SAVE_ERROR", "SAVE_FAILED_TO_SAVE");

		if(showProgress) {
			ProgressWindow::EndProgress( g_theProgressWindow );
		}
		return GAMEFILE_ERR_STORE_FAILED;
	}

	PROGRESS( 370 );

	MBCHAR	sHeader[_MAX_PATH];
	strcpy(sHeader, k_GAME_MAGIC_VALUE);
	uint32 n = c3files_fwrite(sHeader, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), fpSave);
	if (n!=sizeof(k_GAME_MAGIC_VALUE))
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	bool createInfo = (info == NULL);
	if (createInfo)
	{
		info = new SaveInfo();
		GetExtendedInfoFromProfile(info);
	}

	PROGRESS( 380 );

	SaveExtendedGameInfo(fpSave, info);

	PROGRESS( 390 );

	if (createInfo)
		delete info;

	size_t       realSize = archive.StreamLen();
	Assert(realSize <= 0xffffffffu);
	/// \todo Generate error when file is too large
	uint32       ulLen    = static_cast<uint32>(realSize);
	n = c3files_fwrite(&ulLen, sizeof(ulLen), 1, fpSave);
	if (n!=1)
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	PROGRESS( 500 );

	size_t      compressedSize = 0;
	uint8	*   compressedData = NULL;

	if (!CompressData(archive.GetStream(), ulLen,
					  &compressedData, &compressedSize))
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	PROGRESS( 510 );

	uint32      saveSize = static_cast<uint32>(compressedSize);
	n = c3files_fwrite(&saveSize, sizeof(saveSize), 1, fpSave);
	if (n!=1)
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
		}

	PROGRESS( 520 );

	n = c3files_fwrite(compressedData, sizeof(uint8), compressedSize, fpSave);
	if (n != compressedSize)
		{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
		}

	delete compressedData;

	c3files_fclose(fpSave);

	DPRINTF(k_DBG_FILE,
	        ("Time to save game data = %4.2f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC)
	       );

	if (showProgress)
	{
		ProgressWindow::EndProgress(g_theProgressWindow);
	}

	return GAMEFILE_ERR_STORE_OK;
}

uint32 GameFile::Restore(const MBCHAR *filepath)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	clock_t start = clock();
#endif

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		1090 );

	g_theProgressWindow->StartCountingTo(10, g_theStringDB->GetNameStr("LOADING"));

	FILE *  fpLoad = c3files_fopen(C3DIR_DIRECT, filepath, "rb");
	if (!fpLoad)
	{
		c3errors_ErrorDialog("LOAD_ERROR", "LOAD_FAILED_TO_LOAD_GAME");
		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 20 );

	MBCHAR sHeader[_MAX_PATH];
	uint32 n = c3files_fread(sHeader, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), fpLoad);
	Assert(n==sizeof(k_GAME_MAGIC_VALUE));
	if (n!=sizeof(k_GAME_MAGIC_VALUE))
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_UNABLE_TO_READ_GAME");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 30 );

	g_saveFileVersion = -1;
	sint32 i;
	for(i = 0; i < k_NUM_MAGIC_VALUES; i++)
	{
		if(strcmp(sHeader, s_magicValue[i].string) == 0) {
			g_saveFileVersion = s_magicValue[i].version;
			break;
		}
	}

	g_theProgressWindow->StartCountingTo( 40 );

	if(g_saveFileVersion < 0)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_NOT_A_SAVEGAME_FILE");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 50 );

	{
		SaveInfo info;
		if(!LoadExtendedGameInfo(fpLoad, &info))
		{
			c3files_fclose(fpLoad);
			c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_FAILED_TO_LOAD_GAME");

			return GAMEFILE_ERR_LOAD_FAILED;
		}

		g_theProgressWindow->StartCountingTo(60);

		SetProfileFromExtendedInfo(&info);
	}

	g_theProgressWindow->StartCountingTo( 70 );

	uint32 ulLen = 0;
	n = c3files_fread(&ulLen, sizeof(ulLen), 1, fpLoad);
	Assert(n==1);
	if (n!=1)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 80 );

	CivArchive	archive;
	archive.SetSize(ulLen);
	archive.SetLoad();

	g_theProgressWindow->StartCountingTo( 90 );

	uint32	compressedSize = 0;
	n = c3files_fread(&compressedSize, sizeof(compressedSize), 1, fpLoad);
	Assert(n==1);
	if (n!=1)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 100 );

	uint8 * compressedData = new uint8[compressedSize];
	if (compressedSize != c3files_fread(compressedData, sizeof(uint8), compressedSize, fpLoad))
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");
		delete [] compressedData;

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 110 );

	c3files_fclose(fpLoad);

	g_theProgressWindow->StartCountingTo( 120 );

	unsigned long tlen = ulLen;
	if (    (Z_OK != uncompress(archive.GetStream(), &tlen, compressedData, compressedSize))
	     ||	(tlen != ulLen)
	   )
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");
		delete [] compressedData;
		return GAMEFILE_ERR_LOAD_FAILED;
	}

	delete [] compressedData;

	g_theProgressWindow->StartCountingTo( 130 );

	uint32	ver;
	archive >> ver;
	if (ver!=World_World_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 140 );

	archive>>ver;
	if (ver!=Player_Player_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 150 );

	archive>>ver;
	if (ver != SelectedItem_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 160 );

	archive>>ver;
	if (ver!=Advances_Advances_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
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

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 190 );

	archive>>ver;
	if (ver!=CityData_CityData_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 200 );

	archive>>ver;
	if (ver!=Diffcly_Difficulty_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 210 );

	archive>>ver;
	if (ver!=GameObj_GameObj_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 220 );

	archive>>ver;
	if (ver!=Gold_Gold_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 230 );

	archive>>ver;
	if (ver!=ID_ID_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 240 );

	archive>>ver;
	if (ver!=ObjPool_ObjPool_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 250 );

	archive>>ver;
	if (ver!=Sci_Science_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 260 );

	archive>>ver;
	if (ver!=TaxRate_TaxRate_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 270 );

	archive>>ver;
	if (ver!=Unit_Unit_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 280 );

	archive>>ver;
	if (ver!=UnitData_UnitData_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 290 );

	archive>>ver;
	if (ver!=UnitPool_UnitPool_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 300 );

	archive>>ver;
	if (ver!=Cell_CELL_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 310 );

	archive>>ver;
	if (ver!=MapPoint_MapPoint_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	g_theProgressWindow->StartCountingTo( 1080 );

	g_civApp->InitializeGame(&archive);

	g_theProgressWindow->StartCountingTo( 1090 );

	DPRINTF(k_DBG_FILE,
	        ("Time to load game data = %4.2f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC)
	       );

	ProgressWindow::EndProgress(g_theProgressWindow);

	return GAMEFILE_ERR_LOAD_OK;
}

bool GameFile::LoadExtendedGameInfo(FILE *saveFile, SaveInfo *info)
{
	sint32		n;

	n = c3files_fread(info->gameName, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		return false;
	}

	n = c3files_fread(info->leaderName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN)
	{
		return false;
	}

	n = c3files_fread(info->civName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN)
	{
		return false;
	}

	n = c3files_fread(info->note, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		return false;
	}

	info->radarMapWidth = 0;
	info->radarMapHeight = 0;
	delete [] info->radarMapData;
	info->radarMapData = NULL;

	n = c3files_fread(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth))
	{
		return false;
	}
	n = c3files_fread(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight))
	{
		return false;
	}

	if (info->radarMapHeight > 0 && info->radarMapWidth > 0)
	{
		info->radarMapData = new Pixel16[info->radarMapWidth * info->radarMapHeight];
		n = c3files_fread(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapWidth * info->radarMapHeight, saveFile);
		if (n != (sint32)(info->radarMapWidth * info->radarMapHeight * sizeof(Pixel16)))
		{
			return false;
		}

		if (!g_is565Format)
		{
			for (sint32 i=0; i<info->radarMapWidth * info->radarMapHeight; i++)
			{
				info->radarMapData[i] = pixelutils_Convert565to555(info->radarMapData[i]);
			}
		}
	}

	info->powerGraphWidth = 0;
	info->powerGraphHeight = 0;
	delete [] info->powerGraphData;
	info->powerGraphData = NULL;

	n = c3files_fread(&info->powerGraphWidth, sizeof(uint8), sizeof(info->powerGraphWidth), saveFile);
	if (n != sizeof(info->powerGraphWidth))
	{
		return false;
	}

	n = c3files_fread(&info->powerGraphHeight, sizeof(uint8), sizeof(info->powerGraphHeight), saveFile);
	if (n != sizeof(info->powerGraphHeight))
	{
		return false;
	}

	if (info->powerGraphHeight > 0 && info->powerGraphWidth > 0)
	{
		info->powerGraphData = new Pixel16[info->powerGraphWidth * info->powerGraphHeight];
		n = c3files_fread(info->powerGraphData, sizeof(uint8),
							sizeof(Pixel16) * info->powerGraphWidth * info->powerGraphHeight, saveFile);
		if (n != (sint32)(info->powerGraphWidth * info->powerGraphHeight * sizeof(Pixel16)))
		{
			return false;
		}

		if (!g_is565Format)
		{
			std::transform(info->powerGraphData,
			               info->powerGraphData + (info->powerGraphWidth * info->powerGraphHeight),
			               info->powerGraphData,
			               pixelutils_Convert565to555
			              );
		}
	}

	sint32 numPlayers;
	n = c3files_fread(&numPlayers, sizeof(uint8), sizeof(sint32), saveFile);
	if (n != sizeof(sint32))
	{
		return false;
	}
	info->numCivs = numPlayers;

	sint32 has_robot;
	for (sint32 i=0; i<k_MAX_PLAYERS; i++)
	{
		c3files_fread(&has_robot, sizeof(uint8), sizeof(sint32), saveFile);

		if (has_robot)
		{
			n = c3files_fread(info->civList[i], sizeof(uint8), k_MAX_NAME_LEN, saveFile);
			if (n != k_MAX_NAME_LEN)
			{
				return false;
			}
		}
		else
		{
			info->civList[i][0] = '\0';
		}

		sint32 civindex;
		GUID guid;
		n = c3files_fread(&civindex, 1, sizeof(sint32), saveFile);
		if(n != sizeof(sint32))
		{
			return false;
		}

		n = c3files_fread(&guid, 1, sizeof(guid), saveFile);
		if(n != sizeof(guid))
		{
			return false;
		}

		info->networkGUID[i].civIndex = civindex;
		info->networkGUID[i].guid = guid;
	}

	info->gameSetup.ReadFromFile(saveFile);

	NETFunc::Session *  s       = (NETFunc::Session *) &info->gameSetup;
	dp_session_t *      sess    =
	    (dp_session_t *)((uint8*)s + sizeof(NETFunc::Key)); // Problem
	sess->sessionType = GAMEID;

	n = c3files_fread(&info->options, sizeof(SaveInfo::OptionScreenSettings), 1, saveFile); // No problem, unless BOOL is not 4 bytes
	if (n != 1)
	{
		return false;
	}

	info->loadType = SAVEINFOLOAD_EXTENDED;

	n = c3files_fread(&info->isScenario, sizeof(info->isScenario), 1, saveFile);
	if(n != 1)
	{
		return false;
	}

	n = c3files_fread(&info->startInfoType, sizeof(info->startInfoType), 1, saveFile);
	if(n != 1)
	{
		return false;
	}

	n = c3files_fread(&info->numPositions, sizeof(info->numPositions), 1, saveFile);
	if(n != 1)
	{
		return false;
	}

	n = c3files_fread(&info->positions, sizeof(StartingPosition), k_MAX_START_POINTS, saveFile); // Problem, but should be OK
	if(n != k_MAX_START_POINTS)
	{
		return false;
	}

	MBCHAR name[k_SCENARIO_NAME_MAX];
	n = c3files_fread(name, sizeof(MBCHAR), k_SCENARIO_NAME_MAX, saveFile);
	if(n != k_SCENARIO_NAME_MAX)
	{
		return false;
	}

	delete [] info->scenarioName;
	info->scenarioName = NULL;

	if (strlen(name) > 0)
	{
		info->scenarioName = new MBCHAR[strlen(name)+1];
		strcpy(info->scenarioName, name);
	}

	if (g_saveFileVersion >= 47) 
	{
		n = c3files_fread(info->playerCivIndexList, sizeof(sint32), k_MAX_PLAYERS, saveFile);
		if(n != k_MAX_PLAYERS)
		{
			return false;
		}
	}

	if (g_saveFileVersion >= 50)
	{
		n = c3files_fread(&info->showLabels, sizeof(info->showLabels), 1, saveFile);
		if(n != 1)
		{
			return false;
		}

		n = c3files_fread(&info->startingPlayer, sizeof(info->startingPlayer), 1, saveFile);
		if(n != 1)
		{
			return false;
		}
	}
	else
	{
		info->showLabels = FALSE;
		info->startingPlayer = -1;
	}

	return true;
}

bool GameFile::LoadBasicGameInfo(FILE *saveFile, SaveInfo *info)
{

	sint32 n = c3files_fread(info->gameName, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		return false;
	}

	n = c3files_fread(info->leaderName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN)
	{
		return false;
	}

	n = c3files_fread(info->civName, sizeof(uint8), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN)
	{
		return false;
	}

	n = c3files_fread(info->note, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		return false;
	}

	info->loadType = SAVEINFOLOAD_BASIC;

	if (g_saveFileVersion >= 42)
	{
		n = c3files_fread(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
		if (n != sizeof(info->radarMapWidth))
		{
			return false;
		}

		n = c3files_fread(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
		if (n != sizeof(info->radarMapHeight))
		{
			return false;
		}

		if (info->radarMapHeight > 0 && info->radarMapWidth > 0)
		{
			c3files_fseek(saveFile,
						sizeof(Pixel16) * info->radarMapWidth * info->radarMapHeight,
						SEEK_CUR);
		}

		n = c3files_fread(&info->powerGraphWidth, sizeof(uint8), sizeof(info->powerGraphWidth), saveFile);
		if (n != sizeof(info->powerGraphWidth))
		{
			return false;
		}
		n = c3files_fread(&info->powerGraphHeight, sizeof(uint8), sizeof(info->powerGraphHeight), saveFile);
		if (n != sizeof(info->powerGraphHeight))
		{
			return false;
		}

		if (info->powerGraphHeight > 0 && info->powerGraphWidth > 0)
		{
			c3files_fseek(saveFile,
						sizeof(Pixel16) * info->powerGraphWidth * info->powerGraphHeight,
						SEEK_CUR);
		}

		sint32 numPlayers;
		n = c3files_fread(&numPlayers, sizeof(uint8), sizeof(sint32), saveFile);
		if (n != sizeof(sint32))
		{
			return false;
		}
		info->numCivs = numPlayers;

		sint32 has_robot;
		for (sint32 i=0; i<k_MAX_PLAYERS; i++)
		{
			c3files_fread(&has_robot, sizeof(uint8), sizeof(sint32), saveFile);

			if (has_robot)
			{
				n = c3files_fread(info->civList[i], sizeof(uint8), k_MAX_NAME_LEN, saveFile);
				if (n != k_MAX_NAME_LEN)
				{
					return false;
				}
			}
			else
			{
				info->civList[i][0] = '\0';
			}

			sint32 civindex;
			GUID guid;
			n = c3files_fread(&civindex, 1, sizeof(sint32), saveFile);
			if(n != sizeof(sint32))
			{
				return false;
			}

			n = c3files_fread(&guid, 1, sizeof(guid), saveFile);
			if(n != sizeof(guid))
			{
				return false;
			}

			info->networkGUID[i].civIndex = civindex;
			info->networkGUID[i].guid = guid;
		}

		info->gameSetup.ReadFromFile(saveFile); // 720 bytes

		n = c3files_fread(&info->options, sizeof(SaveInfo::OptionScreenSettings), 1, saveFile); // Problem, seems to be OK unless BOOL is not 4 bytes
		if (n != 1)
		{
			return false;
		}

		n = c3files_fread(&info->isScenario, sizeof(info->isScenario), 1, saveFile);
		if(n != 1)
		{
			return false;
		}

		n = c3files_fread(&info->startInfoType, sizeof(info->startInfoType), 1, saveFile);
		if(n != 1)
		{
			return false;
		}
	}

	if (g_saveFileVersion >= 46)
	{
		MBCHAR name[k_SCENARIO_NAME_MAX];
		memset(name, 0, k_SCENARIO_NAME_MAX);
		n = c3files_fread(name, sizeof(MBCHAR), k_SCENARIO_NAME_MAX, saveFile);
		if(n != k_SCENARIO_NAME_MAX)
		{
			return false;
		}

		delete [] info->scenarioName;
		info->scenarioName = NULL;

		if (strlen(name) > 0 && isalpha(name[0])) // Non alpha-numeric characters may have been saved
		{
			info->scenarioName = new MBCHAR[strlen(name)+1];
			strcpy(info->scenarioName, name);
		}
	}

	if (g_saveFileVersion >= 47)
	{
		n = c3files_fread(info->playerCivIndexList, sizeof(sint32), k_MAX_PLAYERS, saveFile);
		if(n != k_MAX_PLAYERS)
		{
			return false;
		}
	}

	return true;
}

void GameFile::SaveExtendedGameInfo(FILE *saveFile, SaveInfo *info)
{
	MBCHAR		*functionName = "GameFile::SaveExtendedGameInfo";
	MBCHAR		*errorString = "Unable to write save file.";

	sint32		n;

	n = c3files_fwrite(info->gameName, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->leaderName, sizeof(MBCHAR), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->civName, sizeof(MBCHAR), k_MAX_NAME_LEN, saveFile);
	if (n != k_MAX_NAME_LEN)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->note, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	n = c3files_fwrite(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	if (info->radarMapWidth > 0 && info->radarMapHeight > 0)
	{
		if (!g_is565Format)
		{
			for (sint32 i=0; i<info->radarMapWidth * info->radarMapHeight; i++)
			{
				Pixel16		pixel = info->radarMapData[i];
				info->radarMapData[i] = ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);
			}
		}

		n = c3files_fwrite(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth,
							saveFile);
		if (n != (sint32)(sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth))
		{
			c3errors_FatalDialog(functionName, errorString);
			return;
		}
	}

	n = c3files_fwrite(&info->powerGraphWidth, sizeof(uint8), sizeof(info->powerGraphWidth), saveFile);
	if (n != sizeof(info->powerGraphWidth))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	n = c3files_fwrite(&info->powerGraphHeight, sizeof(uint8), sizeof(info->powerGraphHeight), saveFile);
	if (n != sizeof(info->powerGraphHeight))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	if (info->powerGraphHeight > 0 && info->powerGraphWidth > 0)
	{
		if (!g_is565Format)
		{
			for (sint32 i=0; i<info->powerGraphWidth * info->powerGraphHeight; i++)
			{
				Pixel16		pixel = info->powerGraphData[i];
				info->powerGraphData[i] = ((pixel & 0x7FE0) << 1) | (pixel & 0x001F);
			}
		}

		n = c3files_fwrite(info->powerGraphData, sizeof(uint8),
							sizeof(Pixel16) * info->powerGraphHeight * info->powerGraphWidth,
							saveFile);
		if (n != (sint32)(sizeof(Pixel16) * info->powerGraphHeight * info->powerGraphWidth))
		{
			c3errors_FatalDialog(functionName, errorString);
			return;
		}
	}

	sint32		numPlayers = k_MAX_PLAYERS;

	n = c3files_fwrite(&numPlayers, sizeof(uint8), sizeof(sint32), saveFile);
	if (n != sizeof(sint32))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	for (sint32 i=0; i<k_MAX_PLAYERS; i++)
	{
		MBCHAR civName[k_MAX_NAME_LEN];

		sint32 has_player;
		GUID guid;
		sint32 civindex;
		if (g_player[i])
		{
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
		}
		else
		{
			has_player = 0;
			c3files_fwrite(&has_player, sizeof(uint8), sizeof(sint32), saveFile);
			memset(&guid, 0, sizeof(guid));
			civindex = -1;
		}
		c3files_fwrite(&civindex, 1, sizeof(sint32), saveFile);
		c3files_fwrite(&guid, 1, sizeof(GUID), saveFile); // Check Linux implementation, should have 16 bytes
	}

	info->gameSetup.WriteToFile(saveFile);

	n = c3files_fwrite(&info->options, sizeof(SaveInfo::OptionScreenSettings), 1, saveFile);
	if (n != 1)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->isScenario, sizeof(info->isScenario), 1, saveFile);
	if(n != 1)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->startInfoType, sizeof(info->startInfoType), 1, saveFile);
	if(n != 1)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->numPositions, sizeof(info->numPositions), 1, saveFile);
	if(n != 1)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->positions, sizeof(StartingPosition), k_MAX_START_POINTS, saveFile);
	if(n != k_MAX_START_POINTS)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	MBCHAR name[k_SCENARIO_NAME_MAX];
	memset(name, 0, k_SCENARIO_NAME_MAX);

	if(strlen(g_scenarioName) > 0) // Problem in Multiplayer
	{
		strcpy(name, g_scenarioName);
	}

	n = c3files_fwrite(name, sizeof(MBCHAR), k_SCENARIO_NAME_MAX, saveFile);
	if(n != k_SCENARIO_NAME_MAX)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->playerCivIndexList, sizeof(sint32), k_MAX_PLAYERS, saveFile);
	if(n != k_MAX_PLAYERS)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->showLabels, sizeof(info->showLabels), 1, saveFile);
	if(n != 1)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->startingPlayer, sizeof(info->startingPlayer), 1, saveFile);
	if(n != 1)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
}

void GameFile::SetProfileFromExtendedInfo(SaveInfo *info)
{
	Assert(info && g_theProfileDB);
	if (!info || !g_theProfileDB) return;

	if (g_isScenario)
	{
		MBCHAR	name[SAVE_LEADER_NAME_SIZE + 1];
		strncpy(name, g_theProfileDB->GetLeaderName(), SAVE_LEADER_NAME_SIZE);
		name[SAVE_LEADER_NAME_SIZE] = 0;
		// TODO: check if this is OK for japanese.
		g_theProfileDB->SetGameName(name);
	}
	else
	{
		g_theProfileDB->SetGameName(info->gameName);
	}


	if (info->startInfoType != STARTINFOTYPE_CIVS &&
	    info->startInfoType != STARTINFOTYPE_POSITIONSFIXED)
	{
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

	if(g_saveFileVersion >= 42)
	{
		if(!info->isScenario)
		{// exclude starting new scenarios
			if (info->scenarioName != NULL && strlen(info->scenarioName) > 0)
			{//same as in beginloadprocess
				strcpy(g_scenarioName,info->scenarioName);
			}
		}
		g_isScenario = info->isScenario;
		g_startInfoType = info->startInfoType;
	} else
	{
		g_isScenario = FALSE;
		g_startInfoType = STARTINFOTYPE_NONE;
	}

	if (g_saveFileVersion >= 50)
	{
		g_showUnitLabels = info->showLabels;
		g_startingPlayer = info->startingPlayer;
	}
	else
	{
		g_showUnitLabels = FALSE;
		g_startingPlayer = -1;
	}
}

void GameFile::GetExtendedInfoFromProfile(SaveInfo *info)
{
	Assert(info && g_theProfileDB);
	if (!info || !g_theProfileDB) return;

	strcpy(info->gameName, g_theProfileDB->GetGameName());
	strcpy(info->leaderName, g_theProfileDB->GetLeaderName());
	strcpy(info->civName, g_theProfileDB->GetCivName());
	strcpy(info->note, g_theProfileDB->GetSaveNote());

	info->gameSetup = g_gamesetup;

	memset(
		info->gameSetup.GetTribeSlots(),
		0,
		8  * sizeof( TribeSlot ) );

	if(g_isScenario)
	{
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

	if(g_theWorld)
	{
		info->numPositions = g_theWorld->GetNumStartingPositions();
		sint32 i;
		for(i = 0; i < info->numPositions; i++)
		{
			info->positions[i].point = g_theWorld->GetStartingPoint(i);
			info->positions[i].civIndex = g_theWorld->GetStartingPointCiv(i);
		}
	}

	info->showLabels = g_showUnitLabels;
	info->startingPlayer = g_startingPlayer;
}

bool GameFile::ValidateGameFile(MBCHAR const * path, SaveInfo *info)
{
	MBCHAR		filepath[_MAX_PATH];
	sprintf(filepath, "%s%s%s", path, FILE_SEP, info->fileName);

	FILE *  saveFile = c3files_fopen(C3DIR_DIRECT, filepath, "rb");
	if (saveFile == NULL)
		return false;

	MBCHAR	header[_MAX_PATH];
	size_t	n = c3files_fread(header, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), saveFile);
	if (n!=sizeof(k_GAME_MAGIC_VALUE))
	{
		c3files_fclose(saveFile);
		return false;
	}

	g_saveFileVersion = -1;

	sint32 i;
	for(i = 0; i < k_NUM_MAGIC_VALUES; i++)
	{
		if(strcmp(header, s_magicValue[i].string) == 0)
		{
			g_saveFileVersion = s_magicValue[i].version;
			break;
		}
	}

	if(g_saveFileVersion < 0)
	{
		c3files_fclose(saveFile);
		return false;
	}

	bool success = LoadBasicGameInfo(saveFile, info);

	c3files_fclose(saveFile);

	return success;
}

bool GameFile::FetchExtendedSaveInfo(MBCHAR const * fullPath, SaveInfo *info)
{
	FILE * saveFile = c3files_fopen(C3DIR_DIRECT, fullPath, "rb");
	if(saveFile == NULL)
		return false;

	MBCHAR  header[_MAX_PATH];
	size_t 	n = c3files_fread(header, sizeof(uint8), sizeof(k_GAME_MAGIC_VALUE), saveFile);
	if( n != sizeof(k_GAME_MAGIC_VALUE))
	{
		c3files_fclose(saveFile);
		return false;
	}

	g_saveFileVersion = -1;

	sint32 i;
	for(i = 0; i < k_NUM_MAGIC_VALUES; i++)
	{
		if(strcmp(header, s_magicValue[i].string) == 0)
		{
			g_saveFileVersion = s_magicValue[i].version;
			break;
		}
	}

	if(g_saveFileVersion < 0)
	{
		c3files_fclose(saveFile);
		return false;
	}

	bool success = LoadExtendedGameInfo(saveFile, info);

	c3files_fclose(saveFile);

	return success;
}

PointerList<GameInfo> *GameFile::BuildSaveList(C3SAVEDIR dir)
{
	PointerList<GameInfo> * list = new PointerList<GameInfo>;

	MBCHAR  dirPath[_MAX_PATH];
	MBCHAR  path[_MAX_PATH];
	if (!g_civPaths->GetSavePath(dir, dirPath)) return list;

#ifdef WIN32
	sprintf(path, "%s%s*.*", dirPath, FILE_SEP);

	WIN32_FIND_DATA fileData;
	HANDLE lpDirList = FindFirstFile(path, &fileData);
	if (lpDirList == INVALID_HANDLE_VALUE) return list;
#else
	DIR * d = opendir(dirPath);
	if (!d) return list;

	struct stat     tmpstat;
	struct dirent * dent = 0;
#endif

	GameInfo			*gameInfo;

	do
	{
#ifndef WIN32
		dent = readdir(d);
		if (!dent) continue;

		snprintf(path, sizeof(path), "%s%s%s", dirPath, FILE_SEP, dent->d_name);
		if (stat(path, &tmpstat) == -1) continue;

		if (S_ISDIR(tmpstat.st_mode))
		{
			MBCHAR *name = dent->d_name;
#else
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
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
			do
			{
#ifndef WIN32
				dent2 = readdir(dir2);
				if (!dent2) continue;

				snprintf(path, sizeof(path), "%s%s%s", gameInfo->path, FILE_SEP, dent2->d_name);
				if (stat(path, &tmpstat) == -1) continue;

				if (!S_ISDIR(tmpstat.st_mode)) {
					name = dent2->d_name;
#else
				if (!(fileData2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					name = fileData2.cFileName;
#endif
					SaveInfo		*saveInfo = new SaveInfo();

					strcpy(saveInfo->fileName, name);

					sprintf(saveInfo->pathName, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);

					if (!ValidateGameFile(gameInfo->path, saveInfo))
					{
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
:
	radarMapWidth       (0),
	radarMapHeight      (0),
	radarMapData        (NULL),
	powerGraphWidth     (0),
	powerGraphHeight    (0),
	powerGraphData      (0),
	numCivs             (0),
// nf_GameSetup gameSetup;
// struct OptionScreenSettings options
	isScenario          (false),
	startInfoType       (STARTINFOTYPE_NONE),
	numPositions        (0),
	loadType            (SAVEINFOLOAD_NONE),
	scenarioName        (NULL),
	showLabels          (false),
	startingPlayer      (CIV_INDEX_VANDALS)
{
	gameName[0] = '\0';
	fileName[0] = '\0';
	pathName[0] = '\0';
	leaderName[0] = '\0';
	civName[0] = '\0';
	note[0] = '\0';

	for (sint32 i=0; i<k_MAX_PLAYERS; i++) {
		civList[i][0] = '\0';
		memset(&networkGUID[i], 0, sizeof(CivGuid));
	}

	std::fill_n(playerCivIndexList, k_MAX_PLAYERS, CIV_INDEX_VANDALS);
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
		copyMe->radarMapData != NULL)
	{
		numPixels = copyMe->radarMapWidth * copyMe->radarMapHeight;
		numBytes = numPixels * sizeof(Pixel16);

		radarMapData = new Pixel16[numPixels];
		memcpy(radarMapData, copyMe->radarMapData, numBytes);
	}

	if (copyMe->powerGraphWidth > 0 &&
		copyMe->powerGraphHeight > 0 &&
		copyMe->powerGraphData != NULL)
	{

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
:
	files   (NULL)
{
	name[0] = '\0';
	path[0] = '\0';
}

GameInfo::~GameInfo()
{
	if (files)
    {
		files->DeleteAll();
	}
	delete files;
}

SaveMapInfo::SaveMapInfo()
:
	radarMapWidth    (0),
	radarMapHeight   (0),
	radarMapData     (NULL)
{
	gameMapName[0] = '\0';
	fileName[0] = '\0';
	pathName[0] = '\0';
	note[0] = '\0';
}

GameMapInfo::GameMapInfo()
:
    files   (NULL)
{
	name[0] = '\0';
	path[0] = '\0';
}

#define k_GAMEMAP_MAGIC_VALUE		"CTPMAP__"

void GameMapFile::RestoreGameMap(const MBCHAR *filename)
{
	GameMapFile().Restore(filename);
}

void GameMapFile::SaveGameMap(const MBCHAR *filename, SaveMapInfo *info)
{
	GameMapFile().Save(filename, info);
}

GameMapFile::GameMapFile(void)
{
}

uint32 GameMapFile::Save(const MBCHAR *filepath, SaveMapInfo *info)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	clock_t start = clock();
#endif

	CivArchive	archive;
	archive.SetStore();

	archive<<World_World_GetVersion();

	g_theWorld->SerializeJustMap(archive);

	FILE * fpSave = c3files_fopen(C3DIR_DIRECT, filepath, "wb");
	if (fpSave == NULL)
	{
		c3errors_ErrorDialogFromDB("SAVE_ERROR", "SAVE_FAILED_TO_SAVE");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	size_t  n;
	MBCHAR	sHeader[_MAX_PATH];
	strcpy(sHeader, k_GAMEMAP_MAGIC_VALUE);
	n = c3files_fwrite(sHeader, sizeof(uint8), sizeof(k_GAMEMAP_MAGIC_VALUE), fpSave);
	if (n!=sizeof(k_GAMEMAP_MAGIC_VALUE))
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	bool createInfo = (info == NULL);
	if (createInfo)
	{
		info = new SaveMapInfo();
		GetExtendedInfoFromProfile(info);
	}

	SaveExtendedGameMapInfo(fpSave, info);

	if (createInfo)
		delete info;

	uint32 ulLen = archive.StreamLen();
	n = c3files_fwrite(&ulLen, sizeof(ulLen), 1, fpSave);
	if (n!=1)
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	n = c3files_fwrite(archive.GetStream(), sizeof(uint8), ulLen, fpSave);
	if (n != ulLen)
	{
		c3files_fclose(fpSave);
		c3errors_FatalDialogFromDB("SAVE_ERROR", "SAVE_UNABLE_TO_WRITE_SAVEGAME");

		return GAMEFILE_ERR_STORE_FAILED;
	}

	c3files_fclose(fpSave);

	DPRINTF(k_DBG_FILE,
	        ("Time to save gamemap data = %4.2f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC)
	       );

	return GAMEFILE_ERR_STORE_OK;
}

uint32 GameMapFile::Restore(const MBCHAR *filepath)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	clock_t start   = clock();
#endif
	FILE *  fpLoad  = c3files_fopen(C3DIR_DIRECT, filepath, "rb");
	if (fpLoad == NULL) {
		c3errors_ErrorDialog("LOAD_ERROR", "LOAD_FAILED_TO_LOAD_GAME");
		return GAMEFILE_ERR_LOAD_FAILED;
	}

	MBCHAR sHeader[_MAX_PATH];
	size_t n = c3files_fread(sHeader, sizeof(uint8), sizeof(k_GAMEMAP_MAGIC_VALUE), fpLoad);
	Assert(n==sizeof(k_GAMEMAP_MAGIC_VALUE));
	if (n!=sizeof(k_GAMEMAP_MAGIC_VALUE))
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_UNABLE_TO_READ_GAME");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	if (strcmp(sHeader, k_GAMEMAP_MAGIC_VALUE))
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_NOT_A_SAVEGAME_FILE");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	{
		SaveMapInfo * info = new SaveMapInfo();
		LoadExtendedGameMapInfo(fpLoad, info);
		SetProfileFromExtendedInfo(info);
		delete info;
	}

	uint32 ulLen;
	n = c3files_fread(&ulLen, sizeof(ulLen), 1, fpLoad);
	Assert(n==1);
	if (n!=1)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	CivArchive	archive;
	archive.SetSize(ulLen);
	archive.SetLoad();

	n = c3files_fread(archive.GetStream(), sizeof(uint8), ulLen, fpLoad);
	if (n!=ulLen)
	{
		c3files_fclose(fpLoad);
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_FILE_SIZE");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	c3files_fclose(fpLoad);

	uint32	ver;
	archive>>ver;
	if (ver!=World_World_GetVersion())
	{
		c3errors_FatalDialogFromDB("LOAD_ERROR", "LOAD_INCORRECT_VERSION_INFO");

		return GAMEFILE_ERR_LOAD_FAILED;
	}

	delete g_theWorld;
	g_theWorld = new World(archive, true);

	DPRINTF(k_DBG_FILE,
	        ("Time to load gamemap data = %4.2f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC)
	       );

	return GAMEFILE_ERR_LOAD_OK;
}

bool GameMapFile::LoadExtendedGameMapInfo(FILE *saveFile, SaveMapInfo *info)
{
	sint32		n;

	n = c3files_fread(info->gameMapName, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		c3files_fclose(saveFile);
		return false;
	}

	n = c3files_fread(info->note, sizeof(uint8), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		c3files_fclose(saveFile);
		return false;
	}

	n = c3files_fread(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth))
	{
		c3files_fclose(saveFile);
		return false;
	}
	n = c3files_fread(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight))
	{
		c3files_fclose(saveFile);
		return false;
	}

	if (info->radarMapHeight > 0 && info->radarMapWidth > 0)
	{
		info->radarMapData = new Pixel16[info->radarMapWidth * info->radarMapHeight];
		n = c3files_fread(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapWidth * info->radarMapHeight, saveFile);
		if (n != (sint32)(info->radarMapWidth * info->radarMapHeight * sizeof(Pixel16)))
		{
			c3files_fclose(saveFile);
			return false;
		}
	}

	return true;
}

void GameMapFile::SaveExtendedGameMapInfo(FILE *saveFile, SaveMapInfo *info)
{
	MBCHAR const functionName[] = "GameMapFile::SaveExtendedGameMapInfo";
	MBCHAR const errorString[]  = "Unable to write savemap file.";

	sint32		n;

	n = c3files_fwrite(info->gameMapName, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(info->note, sizeof(MBCHAR), _MAX_PATH, saveFile);
	if (n != _MAX_PATH)
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}

	n = c3files_fwrite(&info->radarMapWidth, sizeof(uint8), sizeof(info->radarMapWidth), saveFile);
	if (n != sizeof(info->radarMapWidth))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	n = c3files_fwrite(&info->radarMapHeight, sizeof(uint8), sizeof(info->radarMapHeight), saveFile);
	if (n != sizeof(info->radarMapHeight))
	{
		c3errors_FatalDialog(functionName, errorString);
		return;
	}
	if (info->radarMapWidth > 0 && info->radarMapHeight > 0)
	{
		n = c3files_fwrite(info->radarMapData, sizeof(uint8),
							sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth,
							saveFile);
		if (n != (sint32)(sizeof(Pixel16) * info->radarMapHeight * info->radarMapWidth))
		{
			c3errors_FatalDialog(functionName, errorString);
			return;
		}
	}
}

void GameMapFile::SetProfileFromExtendedInfo(SaveMapInfo *info)
{
	Assert(info && g_theProfileDB);
}

void GameMapFile::GetExtendedInfoFromProfile(SaveMapInfo *info)
{
	Assert(info && g_theProfileDB);
}

bool GameMapFile::ValidateGameMapFile(MBCHAR const * path, SaveMapInfo *info)
{
	MBCHAR		filepath[_MAX_PATH];
	sprintf(filepath, "%s%s%s", path, FILE_SEP, info->fileName);

	FILE *  saveFile = c3files_fopen(C3DIR_DIRECT, filepath, "rb");
	if (saveFile == NULL)
		return false;

	MBCHAR  header[_MAX_PATH];
	sint32	n = c3files_fread(header, sizeof(uint8), sizeof(k_GAMEMAP_MAGIC_VALUE), saveFile);
	if (n!=sizeof(k_GAMEMAP_MAGIC_VALUE)) {
		c3files_fclose(saveFile);
		return false;
	}

	if (strcmp(header, k_GAMEMAP_MAGIC_VALUE)) {
		c3files_fclose(saveFile);
		return false;
	}

	bool success = LoadExtendedGameMapInfo(saveFile, info);

	c3files_fclose(saveFile);

	return success;
}

PointerList<GameMapInfo> *GameMapFile::BuildSaveMapList(C3SAVEDIR dir)
{
	PointerList<GameMapInfo> * list = new PointerList<GameMapInfo>;
	MBCHAR dirPath[_MAX_PATH];
	MBCHAR path[_MAX_PATH];
	if (!g_civPaths->GetSavePath(dir, dirPath)) return list;

#ifdef WIN32
	sprintf(path, "%s%s*.*", dirPath, FILE_SEP);

	WIN32_FIND_DATA fileData;
	HANDLE          lpDirList = FindFirstFile(path, &fileData);
	if (lpDirList == INVALID_HANDLE_VALUE) return list;
#else
	DIR * d = opendir(dirPath);
	if (!dir) return list;

	struct stat     tmpstat;
	struct dirent * dent = 0;
#endif

	GameMapInfo			*gameInfo;

	do {
#ifndef WIN32
		dent = readdir(d);
		if (!dent) continue;

		snprintf(path, sizeof(path), "%s%s%s", dirPath, FILE_SEP, dent->d_name);
		if (stat(path, &tmpstat) == -1) continue;

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
			do
			{
#ifndef WIN32
				dent2 = readdir(dir2);
				if (!dent2) continue;

				snprintf(path, sizeof(path), "%s%s%s", gameInfo->path, FILE_SEP, dent2->d_name);
				if (stat(path, &tmpstat) == -1) continue;

				if (!S_ISDIR(tmpstat.st_mode))
				{
					name = dent2->d_name;
#else
				if (!(fileData2.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					name = fileData2.cFileName;
#endif
					SaveMapInfo		*saveInfo = new SaveMapInfo();

					strcpy(saveInfo->fileName, name);

					sprintf(saveInfo->pathName, "%s%s%s", gameInfo->path, FILE_SEP, saveInfo->fileName);

					if (!ValidateGameMapFile(gameInfo->path, saveInfo))
					{
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
