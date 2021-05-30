//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Main application initialisation, processing, and cleanup.
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
// _DEBUG
// - Generates debug information when set.
//
// _DEBUG_MEMORY
// _MEMORYLOGGING
// - Generates extra memory debug information when both set, and _DEBUG set.
//
// USE_SDL
// - Use SDL as replacement for DirectX.
//
// HAVE_UNISTD_H
// WIN32
// LINUX
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Keep the user's leader name when the data is consistent.
// - Skip begin turn handling when loading from a file.
// - Fixed a repetitive memory leak in the Great Libary caused by
//   scenario loading, by Martin Gühmann.
// - Removed some redundant code, because it is already done
//   somewhere else, by Martin Gühmann.
//
//----------------------------------------------------------------------------
//
// - Implemented GovernmentsModified subclass (allowing cdb files including
//   a GovernmentsModified record to produce parsers capable of reading and
//   storing subrecords for Government types.)
//   See http://apolyton.net/forums/showthread.php?s=&threadid=107916 for
//   more details  _____ by MrBaggins Jan-04
//
//   * Reordered parsing of CTPDatabase templated classes to ensure that
//     parsing Advances and Governments would occur before other DBs using
//     GovernmentsModified, to ensure that Governments would be able to be
//     inspected by those other database classes.
//
//----------------------------------------------------------------------------
//
// - When quitting to New Game, go to main menu rather than SP screen
// - Removed cleanup code for SP screen (JJB)
// - Removed some of Martin's library cleanup code, after correcting the
//   problem at the root in GreatLibrary.cpp.
// - Used the new ColorSet option to select civilisation colors.
// - Memory leak repaired: clean up the turn counter override information.
// - Hot seat handling improved.
// - Static member of StatusBar is now deleted correctly, by Martin Gühmann.
// - Cleaned up music screen.
// - The civilisation index from the profile is now reset if it is too high.
//   This prevents the game from crashing. - April 12th 2005 Martin Gühmann
// - Added crash prevention during game loading.
// - Added another civilisation index check.
// - Option added to include multiple data directories.
// - Added Slic segment cleanup.
// - Replaced old civilisation database by new one. (Aug 22nd 2005 Martin Gühmann)
// - Made progress bar more fluently. (Aug 22nd 2005 Martin Gühmann)
// - Removed the old endgame and installation databases. (Aug 29th 2005 Martin Gühmann)
// - Removed old sprite state databases, removed olf good's icon
//   database (unused), replaced old risk database by new one. (Aug 29th 2005 Martin Gühmann)
// - The right color set is now selected afterwards the ProfileDB is available. (Aug 29th 2005 Martin Gühmann)
// - Added cleanup of gaia controller and info window. (Sep 13th 2005 Martin Gühmann)
// - Added ArmyData and Network cleanup. (Sep 25th 2005 Martin Gühmann)
// - Added graphicsresscreen_Cleanup. (Sep 25th 2005 Martin Gühmann)
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
// - Replaced old pollution database by new one. (July 15th 2006 Martin Gühmann)
// - Replaced old global warming database by new one. (July 15th 2006 Martin Gühmann)
// - Added new map icon database. (3-Mar-2007 Martin Gühmann)
// - Replaced old map database by new one. (27-Mar-2007 Martin Gühmann)
// - Replaced old concept database by new one. (31-Mar-2007 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Fixed PBEM BeginTurn event execution. (27-Oct-2007 Martin Gühmann)
// - Games can now be saved if the visible player is a robot. (30-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "civapp.h"

#include "AdvanceBranchRecord.h"
#include "AdvanceListRecord.h"
#include "AdvanceRecord.h"
#include "Advances.h"
#include "advanceutil.h"
#include "AgeCityStyleRecord.h"
#include "AgeRecord.h"
#include <algorithm>                    // std::find
#include "ancientwindows.h"
#include "appstrings.h"
#include "ArmyData.h"                   // ArmyData::Cleanup
#include "armymanagerwindow.h"
#include "AttractWindow.h"
#include "aui_blitter.h"
#include "background.h"
#include "backgroundwin.h"
#include "battleview.h"
#include "bevellesswindow.h"
#include "BuildingBuildListRecord.h"
#include "BuildingRecord.h"
#include "buildingutil.h"
#include "BuildListSequenceRecord.h"
#include "c3_button.h"
#include "c3_checkbox.h"
#include "c3_dropdown.h"
#include "c3_listbox.h"
#include "c3_listitem.h"
#include "c3_static.h"
#include "c3ui.h"
#include "c3window.h"
#include "c3windows.h"
#include "chatbox.h"
#include "CitySizeRecord.h"
#include "CityStyleRecord.h"
#include "citywindow.h"
#include "civ3_main.h"
#include "civarchive.h"
#include "CivilisationRecord.h"
#include "CivPaths.h"
#include "civscenarios.h"
#include "ConceptRecord.h"
#include "ConstRecord.h"                    // g_theConstDB
#include "controlpanelwindow.h"
#include "ctpai.h"
#include "ctp_finger.h"
#include "cursormanager.h"
#include "DB.h"
#include "DBLexer.h"
#include "debugmemory.h"
#include "DifficultyRecord.h"
#include "DiplomacyDetails.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyRecord.h"
#include "DiplomacyThreatRecord.h"
#include "diplomacyutil.h"
#include "diplomacywindow.h"
#include "dipwizard.h"
#include "director.h"                   // g_director
#include "display.h"
#include "DomesticManagementDialog.h"
#include "EditQueue.h"
#include "EndGameObjectRecord.h"
#include "EndgameWindow.h"
#include "Events.h"
#include "Exclusions.h"
#include "FeatRecord.h"
#include "filenamedb.h"
#include "GameEventManager.h"
#include "gamefile.h"                   // SAVE_LEADER_NAME_SIZE
#include "gameinit.h"
#include "gameplayoptions.h"
#include "GameSettings.h"
#include "gamesounds.h"
#include "gfx_options.h"
#include "Globals.h"                    // allocated::clear, allocated::reassign
#include "GlobalWarmingRecord.h"
#include "GoalRecord.h"
#include "GovernmentRecord.h"
#include "grabitem.h"
#include "graphicsresscreen.h"          // graphicsresscreen_Cleanup
#include "graphicsscreen.h"
#include "greatlibrarywindow.h"
#include "helptile.h"
#include "IconRecord.h"
#include "ImprovementListRecord.h"
#include "InfoBar.h"
#include "infowin.h"
#include "infowindow.h"                 // Info Window cleanup
#include "initialplaywindow.h"
#include "intelligencewindow.h"
#include "IntroMovieWin.h"
#include "keymap.h"
#include "keypress.h"
#include "km_screen.h"
#include "loadsavewindow.h"
#include "MainControlPanel.h"
#include "MapIconRecord.h"
#include "MapRecord.h"
#include "message.h"
#include "MessagePool.h"                // g_theMessagePool
#include "messagewin.h"
#include "moviedb.h"
#include "musicscreen.h"
#include "NationalManagementDialog.h"
#include "netconsole.h"
#include "netshell.h"
#include "netshell_game.h"
#include "network.h"
#include "optionswindow.h"
#include "optionwarningscreen.h"
#include "OrderRecord.h"
#include "PersonalityRecord.h"
#include "player.h"                     // g_player
#include "PlayListDB.h"
#include "PollutionRecord.h"
#include "PopRecord.h"
#include "prjfile.h"
#include "profileDB.h"                  // g_theProfileDB
#include "ProfileEdit.h"
#include "progresswindow.h"
#include "radarmap.h"                   // g_radarMap
#include "radarwindow.h"
#include "RandGen.h"                    // g_rand
#include "ResourceRecord.h"
#include "RiskRecord.h"
#include "RoboInit.h"
#include "scenariowindow.h"
#include "sci_advancescreen.h"
#include "ScienceManagementDialog.h"
#include "sciencevictorydialog.h"       // Gaia controller window cleanup
#include "sciencewin.h"
#include "screenmanager.h"
#include "screenutils.h"
#include "SelItem.h"                    // g_selected_item
#include "SlicEngine.h"
#include "SlicSegment.h"                // SlicSegment::Cleanup
#include "soundmanager.h"               // g_soundManager
#include "SoundRecord.h"
#include "soundscreen.h"
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "scenarioeditor.h"
#include "splash.h"						// g_splash_old
#include "spnewgametribescreen.h"
#include "spnewgamewindow.h"
#include "spriteeditor.h"
#include "SpriteRecord.h"
#include "statswindow.h"
#include "statuswindow.h"
#include "StrategyRecord.h"
#include "StrDB.h"
#include <string>                       // std::string
#include "thronedb.h"                   // g_theThroneDB
#include "TerrainImprovementRecord.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "tiledmap.h"
#include "trademanager.h"
#include "TurnCnt.h"                    // g_turn
#include "tutorialwin.h"
#include "Unit.h"
#include "UnitBuildListRecord.h"
#include "UnitDynArr.h"
#include "unitmanager.h"
#include "UnitRecord.h"
#include "unitutil.h"
#include "UVDB.h"
#include "victorywin.h"
#include "WonderBuildListRecord.h"
#include "WonderRecord.h"
#include "WonderMovieRecord.h"
#include "workwin.h"

#include <thread>
#include <chrono>

extern ScreenManager *          g_screenManager;
extern OzoneDatabase            *g_theUVDB;
extern MovieDB                  *g_theVictoryMovieDB;
extern FilenameDB               *g_theMessageIconFileDB;
extern PlayListDB               *g_thePlayListDB;
extern C3UI                 *g_c3ui;
extern Background           *g_background;
extern StatsWindow          *g_statsWindow;
extern StatusWindow         *g_statusWindow;
extern ControlPanelWindow   *g_controlPanel;
extern SpriteEditWindow     *g_spriteEditWindow;
extern aui_Surface          *g_sharedSurface;
extern TiledMap             *g_tiledMap;
extern sint32               g_modalWindow;
extern CivApp               *g_civApp;
extern ChatBox              *g_chatBox;
extern SlicEngine           *g_slicEngine;
extern GameSettings         *g_theGameSettings;
extern TutorialWin          *g_tutorialWin;
extern SaveInfo *           g_savedGameRequest;

// User options
extern sint32               g_fog_toggle;
extern sint32               g_god;
extern sint32               g_isCheatModeOn;
extern BOOL                 g_launchIntoCheatMode;
extern BOOL                 g_launchScenario;
extern BOOL                 g_no_exit_action;
extern BOOL                 g_no_shell;
extern BOOL                 g_no_timeslice;
extern BOOL                 g_runInBackground;
extern sint32               g_scenarioUsePlayerNumber;
extern BOOL                 g_use_profile_process;
extern BOOL                 g_useIntroMovie;

// File names
extern MBCHAR g_improve_filename[_MAX_PATH];
extern MBCHAR g_pollution_filename[_MAX_PATH];
extern MBCHAR g_global_warming_filename[_MAX_PATH];
extern MBCHAR g_ozone_filename[_MAX_PATH];
extern MBCHAR g_terrain_filename[_MAX_PATH];
extern MBCHAR g_installation_filename[_MAX_PATH];
extern MBCHAR g_government_filename[_MAX_PATH];
extern MBCHAR g_governmenticondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_wonder_filename[_MAX_PATH];
extern MBCHAR g_constdb_filename[_MAX_PATH];
extern MBCHAR g_pop_filename[_MAX_PATH];
extern MBCHAR g_civilisation_filename[_MAX_PATH];
extern MBCHAR g_agedb_filename[_MAX_PATH];
extern MBCHAR g_thronedb_filename[_MAX_PATH];
extern MBCHAR g_conceptdb_filename[_MAX_PATH];
extern MBCHAR g_terrainicondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_advanceicondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_advancedb_filename[_MAX_PATH];
extern MBCHAR g_mapicondb_filename[_MAX_PATH]; // New map icon database
extern MBCHAR g_tileimprovementdb_filename[_MAX_PATH];
extern MBCHAR g_spritestatedb_filename[_MAX_PATH];
extern MBCHAR g_specialeffectdb_filename[_MAX_PATH];
extern MBCHAR g_specialattackinfodb_filename[_MAX_PATH];
extern MBCHAR g_goodsspritestatedb_filename[_MAX_PATH];
extern MBCHAR g_cityspritestatedb_filename[_MAX_PATH];
extern MBCHAR g_uniticondb_filename[_MAX_PATH]; // Used by the icon db
extern MBCHAR g_unitsdb_filename[_MAX_PATH];
extern MBCHAR g_wondericondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_improveicondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_difficultydb_filename[_MAX_PATH];
extern MBCHAR g_stringdb_filename[_MAX_PATH];
extern MBCHAR g_slic_filename[_MAX_PATH];
extern MBCHAR g_tutorial_filename[_MAX_PATH];
extern MBCHAR g_unitdb_filename[_MAX_PATH];
extern MBCHAR g_sounddb_filename[_MAX_PATH];
extern MBCHAR g_goods_filename[_MAX_PATH];
extern MBCHAR g_risk_filename[_MAX_PATH];
extern MBCHAR g_wondermoviedb_filename[_MAX_PATH];
extern MBCHAR g_victorymoviedb_filename[_MAX_PATH];
extern MBCHAR g_endgame_filename[_MAX_PATH]; // Free slot
extern MBCHAR g_messageiconfdb_filename[_MAX_PATH];
extern MBCHAR g_goodsicondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_orderdb_filename[_MAX_PATH];
extern MBCHAR g_mapdb_filename[_MAX_PATH];
extern MBCHAR g_playlistdb_filename[_MAX_PATH];
extern MBCHAR g_branchdb_filename[_MAX_PATH];
extern MBCHAR g_endgameicondb_filename[_MAX_PATH]; // Empty slot
extern MBCHAR g_citysize_filename[_MAX_PATH];
extern MBCHAR g_featdb_filename[_MAX_PATH];
extern MBCHAR g_endgameobject_filename[_MAX_PATH];
extern MBCHAR g_city_style_db_filename[_MAX_PATH];
extern MBCHAR g_age_city_style_db_filename[_MAX_PATH];
extern MBCHAR g_goal_db_filename[_MAX_PATH];
extern MBCHAR g_personality_db_filename[_MAX_PATH];
extern MBCHAR g_strategy_db_filename[_MAX_PATH];
extern MBCHAR g_buildlist_sequence_db_filename[_MAX_PATH];
extern MBCHAR g_unit_buildlist_db_filename[_MAX_PATH];
extern MBCHAR g_wonder_buildlist_db_filename[_MAX_PATH];
extern MBCHAR g_building_buildlist_db_filename[_MAX_PATH];
extern MBCHAR g_improvement_list_db_filename[_MAX_PATH];
extern MBCHAR g_diplomacy_db_filename[_MAX_PATH];
extern MBCHAR g_advance_list_db_filename[_MAX_PATH];
extern MBCHAR g_diplomacy_proposal_filename[_MAX_PATH];
extern MBCHAR g_diplomacy_threat_filename[_MAX_PATH];

ProjectFile *g_GreatLibPF = NULL;
ProjectFile *g_ImageMapPF = NULL;
ProjectFile *g_SoundPF = NULL;

sint32 g_logCrashes = 1;

sint32 g_oldRandSeed = FALSE;

ProgressWindow *g_theProgressWindow = NULL;

bool    g_tempLeakCheck = false;

void InitializeGreatLibrary();
void InitializeSoundPF();
void InitializeImageMaps();

namespace
{
#if defined(_DEBUG)
/// Allocated memory at the start
size_t  g_allocatedAtStart  = 0;
#endif

/// Add search directories to a project file
/// @param  a_ProjectFile   Project file to add to
/// @param  a_Type          Type of search items
/// @param  a_PackFileName  Name of a packed (.zfs) file to add to the lookup
/// @remarks Both the directories (for individual files) and the packed files in
///          the directories are added to the search items.
void AddSearchDirectories
(
    ProjectFile *   a_ProjectFile,
    C3DIR const &   a_Type,
    MBCHAR const *  a_PackFileName
)
{
    MBCHAR          path[_MAX_PATH];

    for (int i = 0; g_civPaths->FindPath(a_Type, i++, path); )
    {
        if (path[0])
        {
            // The directory, for looking up "loose" files
            a_ProjectFile->addPath(path);

            // A predefined packed file in the directory
            strcat(path, FILE_SEP);
            strcat(path, a_PackFileName);
            a_ProjectFile->addPath(path);
        }
    }
}

/// Add search "pack files" to a project file
/// @param  a_ProjectFile   Project file to add to
/// @param  a_Type          Type of search path
/// @param  a_PackFileName  Name of a packed (.zfs) file to add to the lookup
/// @remarks Only the packed files are added to the search items.
void AddSearchPacks
(
    ProjectFile *   a_ProjectFile,
    C3DIR const &   a_Type,
    MBCHAR const *  a_PackFileName
)
{
    MBCHAR          path[_MAX_PATH];

    for (int i = 0; g_civPaths->FindPath(a_Type, i++, path); )
    {
        if (path[0])
        {
            // A predefined packed file in the directory
            strcat(path, FILE_SEP);
            strcat(path, a_PackFileName);
            a_ProjectFile->addPath(path, TRUE);
        }
    }
}

//----------------------------------------------------------------------------
//
// Name       : InitDataIncludePath
//
// Description: Add an 'include'-style path to lookup data files.
//
// Parameters : -
//
// Globals    : g_theProfileDB:   user preferences (read)
//              g_civPaths:       (updated)
//
// Returns    : -
//
// Remark(s)  : The top directories (ctp2_data-style) are read from a
//              semicolon-separated string 'Rulesets' in userprofile.txt.
//
//----------------------------------------------------------------------------
void InitDataIncludePath(void)
{
	MBCHAR                  ruleSets[MAX_PATH];
	strcpy(ruleSets, g_theProfileDB->GetRuleSets());

	std::vector<MBCHAR *>   pathStarts;
	MBCHAR *                nextPath    = ruleSets;
    size_t const            ruleSetSize = strlen(ruleSets);

	for (size_t	toDo = ruleSetSize; toDo > 0; )
	{
		pathStarts.push_back(nextPath);
		nextPath = std::find(nextPath, nextPath + toDo, PATH_SEPC);

		if (nextPath < ruleSets + ruleSetSize)
		{
			*nextPath++	= 0;
			toDo		= strlen(nextPath);
		}
		else
		{
			toDo		= 0;
		}
	}

	for
	(
	    std::vector<MBCHAR *>::reverse_iterator p  = pathStarts.rbegin();
	    p != pathStarts.rend();
	    ++p
	)
	{
		g_civPaths->InsertExtraDataPath(*p);
	}
}

//----------------------------------------------------------------------------
//
// Name       : SelectColorSet
//
// Description: Select which color set (colors##.txt file) to use.
//
// Parameters : -
//
// Globals    : g_theProfileDB  : user preferences (read)
//
// Returns    : -
//
// Remark(s)  : - When the user preference is invalid, color set 0 is used.
//              - The existence of the file is not checked.
//
//----------------------------------------------------------------------------
void SelectColorSet(void)
{
	Assert(g_theProfileDB);
	ColorSet::Initialize(g_theProfileDB->GetValueByName("ColorSet"));
}

} // namespace

//----------------------------------------------------------------------------
//
// Name       : Os
//
// Description: Wrapper for some operating system specific functions
//
// Remark(s)  : TODO: move to a better location
//
//----------------------------------------------------------------------------
namespace Os
{
	uint32 GetTicks(void)
	{
#if defined(USE_SDL)
		return SDL_GetTicks();
#else
		return GetTickCount();
#endif
	}

	void Sleep(uint32 milliSeconds)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(milliSeconds));
	}
} // namespace Os

void check_leak()
{
#ifdef _DEBUG
	if (g_tempLeakCheck)
	{
#if defined(WIN32)		
		_CrtMemState new_state;
		_CrtMemCheckpoint(&new_state);
		Assert(g_allocatedAtStart == new_state.lSizes[1]);
		g_allocatedAtStart = new_state.lSizes[1];
#endif // WIN32	
	}
#endif // _DEBUG
}

CivApp::CivApp()
:
    m_appLoaded             (false),
    m_dbLoaded              (false),
    m_gameLoaded            (false),
    m_aiFinishedThisTurn    (true),
    m_inBackground          (false),
    m_isKeyboardScrolling   (false),
    m_keyboardScrollingKey  (0)
{
}

void CivApp::InitializeAppUI(void)
{
	// Set CTP2 specific data for the Anet library (multiplayer only)
	NETFunc::GameType	= GAMEID;				// CTP2 game id for Anet
	NETFunc::DllPath	= "dll" FILE_SEP "net";	// Anet DLLs are in dll\net (relative to executable)

	if (g_useIntroMovie && !g_no_shell)
	{
		intromoviewin_Initialize();
		intromoviewin_DisplayIntroMovie();
	}
	else
	{
		if (g_soundManager)
		{
			g_soundManager->EnableMusic();
			g_soundManager->PickNextTrack();
			g_soundManager->StartMusic();
		}

		AUI_ERRCODE errcode = initialplayscreen_Initialize();
		Assert(errcode == AUI_ERRCODE_OK);

		if(!g_no_shell && !g_launchScenario)
			initialplayscreen_displayMyWindow();
	}
}

#ifdef _DEBUG
void print_dip_details(FILE *dipFile, const DiplomacyProposalRecord::StrengthStrings *strings1,
					   const DiplomacyProposalRecord *rec2)
{
	const DiplomacyProposalRecord::StrengthStrings *strings2;
	StringId str1, str2;

	if(strings1->GetDetailsEven(str1)) {
		if(rec2->GetDetailsEx0(strings2)) {
			if(strings2->GetDetailsEven(str2)) {
				fprintf(dipFile, "%s %s (%s/%s)\n\n",
						g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
						g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
			}
		}
		if(rec2->GetDetailsEx1(strings2)) {
			if(strings2->GetDetailsEven(str2)) {
				fprintf(dipFile, "%s %s (%s/%s)\n\n",
						g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
						g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
			}
		}
		if(rec2->GetDetailsEx2(strings2)) {
			if(strings2->GetDetailsEven(str2)) {
				fprintf(dipFile, "%s %s (%s/%s)\n\n",
						g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
						g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
			}
		}
		if(rec2->GetDetailsEx3(strings2)) {
			if(strings2->GetDetailsEven(str2)) {
				fprintf(dipFile, "%s %s (%s/%s)\n\n",
						g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
						g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
			}
		}
		if(rec2->GetDetailsEx4(strings2)) {
			if(strings2->GetDetailsEven(str2)) {
				fprintf(dipFile, "%s %s (%s/%s)\n\n",
						g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
						g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
			}
		}
	}
}
#endif

// Old style interface - parameter not used any more
sint32 CivApp::InitializeAppDB(CivArchive & /* archive */)
{
	return InitializeAppDB() ? TRUE : FALSE;
}

/// Initialize databases
bool CivApp::InitializeAppDB(void)
{
	ProgressWindow::BeginProgress
		(g_theProgressWindow, "InitProgressWindow", 520);

	// Create a set of empty databases
	g_theAdvanceDB              = new CTPDatabase<AdvanceRecord>;
	g_theAdvanceBranchDB        = new CTPDatabase<AdvanceBranchRecord>;
	g_theAdvanceListDB          = new CTPDatabase<AdvanceListRecord>;
	g_theAgeDB                  = new CTPDatabase<AgeRecord>;
	g_theAgeCityStyleDB         = new CTPDatabase<AgeCityStyleRecord>;
	g_theBuildingDB             = new CTPDatabase<BuildingRecord>;
	g_theBuildingBuildListDB    = new CTPDatabase<BuildingBuildListRecord>;
	g_theBuildListSequenceDB    = new CTPDatabase<BuildListSequenceRecord>;
	g_theCitySizeDB             = new CTPDatabase<CitySizeRecord>;
	g_theCityStyleDB            = new CTPDatabase<CityStyleRecord>;
	g_theCivilisationDB         = new CTPDatabase<CivilisationRecord>;
	g_theConceptDB              = new CTPDatabase<ConceptRecord>;
	g_theConstDB                = new CTPDatabase<ConstRecord>;
	g_theDifficultyDB           = new CTPDatabase<DifficultyRecord>;
	g_theDiplomacyDB            = new CTPDatabase<DiplomacyRecord>;
	g_theDiplomacyProposalDB    = new CTPDatabase<DiplomacyProposalRecord>;
	g_theDiplomacyThreatDB      = new CTPDatabase<DiplomacyThreatRecord>;
	g_theEndGameObjectDB        = new CTPDatabase<EndGameObjectRecord>;
	g_theFeatDB                 = new CTPDatabase<FeatRecord>;
	g_theGlobalWarmingDB        = new CTPDatabase<GlobalWarmingRecord>;
	g_theGoalDB                 = new CTPDatabase<GoalRecord>;
	g_theGovernmentDB           = new CTPDatabase<GovernmentRecord>;
	g_theIconDB                 = new CTPDatabase<IconRecord>;
	g_theImprovementListDB      = new CTPDatabase<ImprovementListRecord>;
	g_theMapDB                  = new CTPDatabase<MapRecord>;
	g_theMapIconDB              = new CTPDatabase<MapIconRecord>;
	g_theMessageIconFileDB      = new FilenameDB();
	g_theOrderDB                = new CTPDatabase<OrderRecord>;
	g_thePersonalityDB          = new CTPDatabase<PersonalityRecord>;
	g_thePlayListDB             = new PlayListDB();
	g_thePollutionDB            = new CTPDatabase<PollutionRecord>;
	g_thePopDB                  = new CTPDatabase<PopRecord>;
	g_theResourceDB             = new CTPDatabase<ResourceRecord>;
	g_theRiskDB                 = new CTPDatabase<RiskRecord>;
	g_theSoundDB                = new CTPDatabase<SoundRecord>;
	g_theSpecialAttackInfoDB    = new CTPDatabase<SpecialAttackInfoRecord>;
	g_theSpecialEffectDB        = new CTPDatabase<SpecialEffectRecord>;
	g_theSpriteDB               = new CTPDatabase<SpriteRecord>;
	g_theStrategyDB             = new CTPDatabase<StrategyRecord>;
	g_theStringDB               = new StringDB();
	g_theTerrainDB              = new CTPDatabase<TerrainRecord>;
	g_theTerrainImprovementDB   = new CTPDatabase<TerrainImprovementRecord>;
	g_theThroneDB               = new ThroneDB();
	g_theUVDB                   = new OzoneDatabase();
	g_theUnitBuildListDB        = new CTPDatabase<UnitBuildListRecord>;
	g_theUnitDB                 = new CTPDatabase<UnitRecord>;
	g_theVictoryMovieDB         = new MovieDB();
	g_theWonderDB               = new CTPDatabase<WonderRecord>;
	g_theWonderBuildListDB      = new CTPDatabase<WonderBuildListRecord>;
	g_theWonderMovieDB          = new CTPDatabase<WonderMovieRecord>;

	// Firstly get the string database up and running - so we can display texts
	if (!g_theStringDB->Parse(g_stringdb_filename))
	{
		return false;
	}

	// Has to be done after the initialization of the string database
	g_theProgressWindow->StartCountingTo(10, g_theStringDB->GetNameStr("LOADING"));

	// Fill the databases from file

	if (!g_theSoundDB->Parse(C3DIR_GAMEDATA, g_sounddb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 20 );

	if (strcmp(g_mapicondb_filename, ""))   // May not exist for mods
	{
		if (!g_theMapIconDB->Parse(C3DIR_GAMEDATA, g_mapicondb_filename))
		{
			return false;
		}
	}

	g_theProgressWindow->StartCountingTo( 30 );

	if (!g_theIconDB->Parse(C3DIR_GAMEDATA, g_uniticondb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 40 );

	if (c3files_getfilesize(C3DIR_DIRECT, g_constdb_filename) > 0 && // includes FindFile
	    g_theConstDB->Parse(C3DIR_DIRECT, g_constdb_filename))
	{
		fprintf(stderr, "%s L%d: Using Const.txt from user space!\n", __FILE__, __LINE__);
	}
	else if (!g_theConstDB->Parse(C3DIR_GAMEDATA, g_constdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 50 );

	if (!g_theWonderMovieDB->Parse(C3DIR_GAMEDATA, g_wondermoviedb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 60 );

	if (!g_theVictoryMovieDB->Parse(g_victorymoviedb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 70 );

	if (!g_thePlayListDB->Parse(g_playlistdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 80 );

	if (!g_theSpriteDB->Parse(C3DIR_GAMEDATA, "newsprite.txt"))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 90 );

	if (!g_theSpecialEffectDB->Parse(C3DIR_GAMEDATA, g_specialeffectdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 100 );

	if (!g_theSpecialAttackInfoDB->Parse(C3DIR_GAMEDATA, g_specialattackinfodb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 110 );

	if (!g_theAdvanceBranchDB->Parse(C3DIR_GAMEDATA, g_branchdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 120 );

	if (!g_theAdvanceDB->Parse(C3DIR_GAMEDATA, g_advancedb_filename))
    {
		return false;
	}

	g_theProgressWindow->StartCountingTo( 130 );

	if (!g_theGovernmentDB->Parse(C3DIR_GAMEDATA, g_government_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 140 );

	if (!g_theUnitDB->Parse(C3DIR_GAMEDATA, g_unitdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 150 );

	if (!g_theDifficultyDB->Parse(C3DIR_GAMEDATA, g_difficultydb_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 160 );

	if (!g_theAgeDB->Parse(C3DIR_GAMEDATA, g_agedb_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 170 );

	g_theThroneDB->Init(g_thronedb_filename );

	g_theProgressWindow->StartCountingTo( 180 );

	if (!g_theConceptDB->Parse(C3DIR_GAMEDATA, g_conceptdb_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 190 );

	if (!g_theTerrainImprovementDB->Parse(C3DIR_GAMEDATA, g_tileimprovementdb_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 200 );

	if (!g_theResourceDB->Parse(C3DIR_GAMEDATA, g_goods_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 210 );

	if (!g_theTerrainDB->Parse(C3DIR_GAMEDATA, g_terrain_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 220 );

	if (!g_theBuildingDB->Parse(C3DIR_GAMEDATA, g_improve_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 230 );

	if (!g_thePollutionDB->Parse(C3DIR_GAMEDATA, g_pollution_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 240 );

	if (!g_theGlobalWarmingDB->Parse(C3DIR_GAMEDATA, g_global_warming_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 250 );

	if (!g_theUVDB->Initialise(g_ozone_filename, C3DIR_GAMEDATA))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 260 );

	if (!g_theCivilisationDB->Parse(C3DIR_GAMEDATA, g_civilisation_filename))
	{
		return false;
	}

	// Fix the player index if it is out of range:
	if(g_theProfileDB->GetCivIndex() >= g_theCivilisationDB->NumRecords())
		g_theProfileDB->SetCivIndex(1); // Set to first non-Barbarian civ

	g_theProgressWindow->StartCountingTo( 270 );

	if (!g_theWonderDB->Parse(C3DIR_GAMEDATA, g_wonder_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 280 );

	if (!g_theRiskDB->Parse(C3DIR_GAMEDATA, g_risk_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 290 );

	if (!g_theMessageIconFileDB->Parse(g_messageiconfdb_filename))
	{
		ExitGame();
		return false;
	}

	g_theProgressWindow->StartCountingTo( 300 );

	if (!g_theMapDB->Parse(C3DIR_GAMEDATA, g_mapdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 310 );

	if (!g_theOrderDB->Parse(C3DIR_GAMEDATA, g_orderdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 320 );

	if (!g_theFeatDB->Parse(C3DIR_GAMEDATA, g_featdb_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 330 );

	if (!g_theEndGameObjectDB->Parse(C3DIR_GAMEDATA, g_endgameobject_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 340 );

	if (!g_theGoalDB->Parse(C3DIR_AIDATA, g_goal_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 350 );

	if (!g_thePersonalityDB->Parse(C3DIR_AIDATA, g_personality_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 360 );

	if (!g_theUnitBuildListDB->Parse(C3DIR_AIDATA, g_unit_buildlist_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 370 );

	if (!g_theWonderBuildListDB->Parse(C3DIR_AIDATA, g_wonder_buildlist_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 380 );

	if (!g_theBuildingBuildListDB->Parse(C3DIR_AIDATA, g_building_buildlist_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 390 );

	if (!g_theImprovementListDB->Parse(C3DIR_AIDATA, g_improvement_list_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 400 );

	if (!g_theStrategyDB->Parse(C3DIR_AIDATA, g_strategy_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 410 );

	if (!g_theBuildListSequenceDB->Parse(C3DIR_AIDATA, g_buildlist_sequence_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 420 );

	if (!g_theDiplomacyDB->Parse(C3DIR_AIDATA, g_diplomacy_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 430 );

	if (!g_theDiplomacyProposalDB->Parse(C3DIR_AIDATA, g_diplomacy_proposal_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 440 );

	if (!g_theDiplomacyThreatDB->Parse(C3DIR_AIDATA, g_diplomacy_threat_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 450 );

	if (!g_theAdvanceListDB->Parse(C3DIR_AIDATA, g_advance_list_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 460 );

	if (!g_theCityStyleDB->Parse(C3DIR_GAMEDATA, g_city_style_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 470 );

	if (!g_theAgeCityStyleDB->Parse(C3DIR_GAMEDATA, g_age_city_style_db_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 480 );

	{
		char *lastdot = strrchr(g_citysize_filename, '.');
		Assert(lastdot);

		if(!lastdot)
			lastdot = g_citysize_filename + strlen(g_citysize_filename);

		sprintf(lastdot, "%d.txt", 0);

		if (!g_theCitySizeDB->Parse(C3DIR_GAMEDATA, g_citysize_filename))
			return false;

		strcpy(lastdot, ".txt");
	}

	g_theProgressWindow->StartCountingTo( 490 );

	if (!g_thePopDB->Parse(C3DIR_GAMEDATA, g_pop_filename))
	{
		return false;
	}

	g_theProgressWindow->StartCountingTo( 500 );

	g_exclusions = new Exclusions();

	if(!g_theUnitDB->ResolveReferences())               return false;
	if(!g_theAdvanceDB->ResolveReferences())            return false;
	if(!g_theIconDB->ResolveReferences())               return false;
	if(!g_theConstDB->ResolveReferences())              return false;
	if(!g_theSpriteDB->ResolveReferences())             return false;
	if(!g_theSoundDB->ResolveReferences())              return false;
	if(!g_theSpecialEffectDB->ResolveReferences())      return false;
	if(!g_theSpecialAttackInfoDB->ResolveReferences())  return false;
	if(!g_theGovernmentDB->ResolveReferences())         return false;
	if(!g_theAdvanceBranchDB->ResolveReferences())      return false;
	if(!g_theAgeDB->ResolveReferences())                return false;
	if(!g_theTerrainDB->ResolveReferences())            return false;
	if(!g_theResourceDB->ResolveReferences())           return false;
	if(!g_theTerrainImprovementDB->ResolveReferences()) return false;
	if(!g_theMapDB->ResolveReferences())                return false;
	if(!g_theOrderDB->ResolveReferences())              return false;
	if(!g_theMapIconDB->ResolveReferences())            return false;
	if(!g_theCitySizeDB->ResolveReferences())           return false;
	if(!g_thePopDB->ResolveReferences())                return false;
	if(!g_theBuildingDB->ResolveReferences())           return false;
	if(!g_thePollutionDB->ResolveReferences())          return false;
	if(!g_theCityStyleDB->ResolveReferences())          return false;
	if(!g_theAgeCityStyleDB->ResolveReferences())       return false;
	if(!g_theGoalDB->ResolveReferences())               return false;
	if(!g_thePersonalityDB->ResolveReferences())        return false;
	if(!g_theUnitBuildListDB->ResolveReferences())      return false;
	if(!g_theWonderBuildListDB->ResolveReferences())    return false;
	if(!g_theBuildingBuildListDB->ResolveReferences())  return false;
	if(!g_theConceptDB->ResolveReferences())            return false;
	if(!g_theImprovementListDB->ResolveReferences())    return false;
	if(!g_theStrategyDB->ResolveReferences())           return false;
	if(!g_theBuildListSequenceDB->ResolveReferences())  return false;
	if(!g_theDiplomacyDB->ResolveReferences())          return false;
	if(!g_theDiplomacyProposalDB->ResolveReferences())  return false;
	if(!g_theDiplomacyThreatDB->ResolveReferences())    return false;
	if(!g_theAdvanceListDB->ResolveReferences())        return false;
	if(!g_theCivilisationDB->ResolveReferences())       return false;
	if(!g_theWonderDB->ResolveReferences())             return false;
	if(!g_theWonderMovieDB->ResolveReferences())        return false;
	if(!g_theFeatDB->ResolveReferences())               return false;
	if(!g_theEndGameObjectDB->ResolveReferences())      return false;
	if(!g_theRiskDB->ResolveReferences())               return false;
	if(!g_theDifficultyDB->ResolveReferences())         return false;
	if(!g_theGlobalWarmingDB->ResolveReferences())      return false;

	g_theProgressWindow->StartCountingTo( 510 );

	unitutil_Initialize();
	advanceutil_Initialize();
	diplomacyutil_Initialize();
	terrainutil_Initialize();
	buildingutil_Initialize();

	g_theProgressWindow->StartCountingTo( 520 );

#ifdef _DEBUG
	FILE *dipFile = fopen("logs" FILE_SEP "dipcombo.txt", "w");
	if(dipFile) {
		sint32 i,j;

		for(i = 1; i < g_theDiplomacyProposalDB->NumRecords(); i++) {
			const DiplomacyProposalRecord *rec1 = g_theDiplomacyProposalDB->Get(i);
			for(j = 1; j < g_theDiplomacyProposalDB->NumRecords(); j++) {
				if(j == i) continue;

				const DiplomacyProposalRecord *rec2 = g_theDiplomacyProposalDB->Get(j);
				if(rec1->GetExcludes() & rec2->GetClass()) continue;

				StringId str1, str2;
				const DiplomacyProposalRecord::StrengthStrings *strings1, *strings2;

				if(rec1->GetDetails0(strings1)) {
					if(strings1->GetDetailsEven(str1)) {
						if(rec2->GetDetailsEx0(strings2)) {
							if(strings2->GetDetailsEven(str2)) {
								fprintf(dipFile, "%s %s (%s/%s)\n\n",
										g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
										g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
							}
						}
					}
				}
				if(rec1->GetDetails1(strings1)) {
					if(strings1->GetDetailsEven(str1)) {
						if(rec2->GetDetailsEx1(strings2)) {
							if(strings2->GetDetailsEven(str2)) {
								fprintf(dipFile, "%s %s (%s/%s)\n\n",
										g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
										g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
							}
						}
					}
				}
				if(rec1->GetDetails2(strings1)) {
					if(strings1->GetDetailsEven(str1)) {
						if(rec2->GetDetailsEx2(strings2)) {
							if(strings2->GetDetailsEven(str2)) {
								fprintf(dipFile, "%s %s (%s/%s)\n\n",
										g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
										g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
							}
						}
					}
				}
				if(rec1->GetDetails3(strings1)) {
					if(strings1->GetDetailsEven(str1)) {
						if(rec2->GetDetailsEx3(strings2)) {
							if(strings2->GetDetailsEven(str2)) {
								fprintf(dipFile, "%s %s (%s/%s)\n\n",
										g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
										g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
							}
						}
					}
				}
				if(rec1->GetDetails4(strings1)) {
					if(strings1->GetDetailsEven(str1)) {
						if(rec2->GetDetailsEx4(strings2)) {
							if(strings2->GetDetailsEven(str2)) {
								fprintf(dipFile, "%s %s (%s/%s)\n\n",
										g_theStringDB->GetNameStr(str1), g_theStringDB->GetNameStr(str2),
										g_theStringDB->GetIdStr(str1), g_theStringDB->GetIdStr(str2));
							}
						}
					}
				}
			}
		}
		fclose(dipFile);
	}
#endif
	ProgressWindow::EndProgress( g_theProgressWindow );

	m_dbLoaded = true;

	return true;
}

sint32 CivApp::InitializeApp(HINSTANCE hInstance, int iCmdShow)
{
#ifdef WIN32
    // COM needed for DirectX/Movies
	CoInitialize(NULL);
#endif
#ifdef __AUI_USE_SDL__
        Uint32 flags = (SDL_INIT_EVERYTHING | SDL_INIT_EVENTS) & ~SDL_INIT_AUDIO; //why no audio?
#if defined(_DEBUG) || defined(DEBUG)
        flags |= SDL_INIT_NOPARACHUTE;
#endif// _DEBUG || DEBUG
        int rc = SDL_Init(flags);
        if (rc != 0) {
                fprintf(stderr, "Could not initialize SDL:\n%s\n", SDL_GetError());
                return -1;
        }
#endif

	Splash::Initialize();

	CivPaths_InitCivPaths();

	g_theProfileDB = new ProfileDB;
	if (!g_theProfileDB->Init(FALSE)) {
		c3errors_FatalDialog("CivApp", "Unable to init the ProfileDB.");
		return -1;
	}

	g_logCrashes = g_theProfileDB->GetEnableLogs();

	InitDataIncludePath();
#if !defined(USE_SDL)
	c3files_InitializeCD();
	g_civPaths->InitCDPath();
#endif // USE_SDL
	GreatLibrary::Initialize_Great_Library_Data();

	display_Initialize(hInstance, iCmdShow);
	init_keymap();
	(void) ui_Initialize();
	SoundManager::Initialize();

	if ( sharedsurface_Initialize() != AUI_ERRCODE_OK ) {
		c3errors_FatalDialog( "CivApp", "Unable to init shared surface." );
		return -1;
	}

	CursorManager::Initialize();

	InitializeImageMaps();

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		620 );

	g_theProgressWindow->StartCountingTo( 10 );

	if (!gameinit_InitializeGameFiles())
	{
		ExitGame();
		return -1;
	}

	SPLASH_STRING(g_theProfileDB->IsAIOn() ? "AI is ON" : "AI is OFF");

	g_theProgressWindow->StartCountingTo( 20 );

	g_theProgressWindow->StartCountingTo( 540 );

	if (!InitializeAppDB())
	{
		c3errors_FatalDialog("CivApp", "Unable to Init the Databases.");
		return -1;
	}

	g_theProgressWindow->StartCountingTo( 550 );

	InitializeGreatLibrary();

	g_theProgressWindow->StartCountingTo( 560 );

	InitializeSoundPF();

	g_theProgressWindow->StartCountingTo( 570 );

	InitializeAppUI();

	g_theProgressWindow->StartCountingTo( 580 );

	CivScenarios::Initialize();

	{
		// Maintain consistency between the CivIndex and CivName entries.
		// When inconsistent, the CivIndex is leading.

		sint32 const userCivIndex = g_theProfileDB->GetCivIndex();

		if (static_cast<int>(userCivIndex) < g_theCivilisationDB->NumRecords())
		{
			MBCHAR const * const    dbCivName =
			    g_theStringDB->GetNameStr
			        (g_theCivilisationDB->Get(userCivIndex)->GetPluralCivName());

			if (0 == strcmp(dbCivName, g_theProfileDB->GetCivName()))
			{
				// No action: keep the leader name of the user.
			}
			else
			{
				// Restore civilisation default country and leader names.
				g_theProfileDB->DefaultSettings();
			}
		}
		else
		{
			// Possible after using a mod with less civilisations
			g_theProfileDB->SetCivIndex(1);
			g_theProfileDB->DefaultSettings();
		}
	}

	g_theProgressWindow->StartCountingTo( 590 );

	StartMessageSystem();

	g_theProgressWindow->StartCountingTo( 600 );

	SPLASH_STRING("Initializing Messaging System...");
	AUI_ERRCODE errcode = messagewin_InitializeMessages();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 610 );

	if (g_theProfileDB->IsUseFingerprinting())
		if (!ctpfinger_Check()) {

			c3errors_FatalDialog(appstrings_GetString(APPSTR_INITIALIZE),
									appstrings_GetString(APPSTR_CANTFINDFILE));
		}

	g_theProgressWindow->StartCountingTo( 620 );

	if (g_c3ui->TheMouse())
	{
		double const sensitivity = 0.25 * (1 + g_theProfileDB->GetMouseSpeed());
		g_c3ui->TheMouse()->Sensitivity() = sensitivity;
	}

	SelectColorSet(); // Select the right color set.

	ProgressWindow::EndProgress( g_theProgressWindow );

	m_appLoaded = true;

	return 0;
}

sint32 CivApp::QuickInit(HINSTANCE hInstance, int iCmdShow)
{
	InitializeApp(hInstance, iCmdShow);
	StartGame();

	return 0;
}

void CivApp::CleanupAppUI(void)
{
	NetShell::Leave( k_NS_FLAGS_DESTROY );

	// Clean up any opened screens
	greatlibrary_Cleanup();
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();
	optionsscreen_Cleanup();
	optionwarningscreen_Cleanup();
	graphicsscreen_Cleanup();
	gameplayoptions_Cleanup();
	soundscreen_Cleanup();
	musicscreen_Cleanup();
	StatusBar::CleanUp();
	loadsavescreen_Cleanup();
	graphicsresscreen_Cleanup();
	km_screen_Cleanup();

	allocated::clear(g_c3ui);

#if defined(_DEBUG) && !defined(__AUI_USE_SDL__)
	sint32 const cleanBaseRefCount = aui_Base::GetBaseRefCount();
	Assert(0 == cleanBaseRefCount);
#endif

	allocated::clear(g_GreatLibPF);
	allocated::clear(g_ImageMapPF);
	allocated::clear(g_SoundPF);
}

void CivApp::CleanupAppDB(void)
{
    allocated::clear(g_theMapDB);
    allocated::clear(g_exclusions);
    allocated::clear(g_theMessageIconFileDB);
    allocated::clear(g_theRiskDB);
    allocated::clear(g_theWonderDB);
    allocated::clear(g_theCivilisationDB);
    allocated::clear(g_thePollutionDB);
    allocated::clear(g_theUVDB);
    allocated::clear(g_theGlobalWarmingDB);
    allocated::clear(g_theBuildingDB);
    allocated::clear(g_theTerrainDB);
    allocated::clear(g_theResourceDB);
    allocated::clear(g_theGovernmentDB);
    allocated::clear(g_theConceptDB);
    allocated::clear(g_theThroneDB);
    allocated::clear(g_theAgeDB);
    allocated::clear(g_theCityStyleDB);
    allocated::clear(g_theAgeCityStyleDB);
    allocated::clear(g_theDifficultyDB);
    allocated::clear(g_theUnitDB);
    allocated::clear(g_theAdvanceDB);
    allocated::clear(g_theAdvanceBranchDB);
    allocated::clear(g_theSpecialEffectDB);
    allocated::clear(g_theSpriteDB);
    allocated::clear(g_theSpecialAttackInfoDB);
    allocated::clear(g_thePlayListDB);
    allocated::clear(g_theVictoryMovieDB);
    allocated::clear(g_theWonderMovieDB);
    allocated::clear(g_theIconDB);
    allocated::clear(g_theSoundDB);
    allocated::clear(g_theStringDB);
    allocated::clear(g_theTerrainImprovementDB);
    allocated::clear(g_theOrderDB);
    allocated::clear(g_theCitySizeDB);
    allocated::clear(g_thePopDB);
    allocated::clear(g_theBuildingDB);
    allocated::clear(g_theFeatDB);
    allocated::clear(g_theEndGameObjectDB);
    allocated::clear(g_theGoalDB);
    allocated::clear(g_thePersonalityDB);
    allocated::clear(g_theUnitBuildListDB);
    allocated::clear(g_theWonderBuildListDB);
    allocated::clear(g_theBuildingBuildListDB);
    allocated::clear(g_theImprovementListDB);
    allocated::clear(g_theStrategyDB);
    allocated::clear(g_theBuildListSequenceDB);
    allocated::clear(g_theDiplomacyDB);
    allocated::clear(g_theAdvanceListDB);
    allocated::clear(g_theDiplomacyProposalDB);
    allocated::clear(g_theDiplomacyThreatDB);
    allocated::clear(g_theMapIconDB);
    allocated::clear(g_theConstDB);

    m_dbLoaded = false;
}

void CivApp::CleanupApp(void)
{
	if (m_appLoaded)
	{
		g_network.Cleanup();
		GreatLibrary::Shutdown_Great_Library_Data();
		Splash::Cleanup();
		messagewin_Cleanup();

		allocated::clear(g_slicEngine);
		allocated::clear(g_theMessagePool);

		CivScenarios::Cleanup();
		SoundManager::Cleanup();

		allocated::clear(g_theProfileDB);

		gameinit_Cleanup();
		events_Cleanup();
		gameEventManager_Cleanup();
		g_network.Cleanup();
		CursorManager::Cleanup();
		sharedsurface_Cleanup();
		CleanupAppUI();
		cleanup_keymap();
		CleanupAppDB();
		CivPaths_CleanupCivPaths();
		SlicSegment::Cleanup();

#ifdef WIN32
		// COM needed for DirectX Moviestuff
		CoUninitialize();
#endif

		display_Cleanup();
	}

	m_appLoaded = false;
}

sint32 CivApp::InitializeGameUI(void)
{
	SelectColorSet();
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		130 );

	g_theProgressWindow->StartCountingTo(10, g_theStringDB->GetNameStr("LOADING"));

	SPLASH_STRING("Creating Main Windows...");
#if defined(_DEBUG)
	g_splash_old = Os::GetTicks();
#endif

	SPLASH_STRING("Creating Status Window...");
	sint32 errcode = c3windows_MakeStatusWindow(TRUE);
	Assert(errcode == 0);
	if (errcode != 0) return 7;
	g_statusWindow->Hide(); // Maybe should be removed entirely

	g_theProgressWindow->StartCountingTo( 20 );

	SPLASH_STRING("Creating Game Window...");
	errcode = backgroundWin_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 30 );

	SPLASH_STRING("Creating Tile Help Window...");
	errcode = helptile_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 40 );

	SPLASH_STRING("Creating Debug Window...");
	errcode = c3windows_MakeDebugWindow(TRUE);

	g_theProgressWindow->StartCountingTo( 50 );

	AncientWindows_PreInitialize();

	g_theProgressWindow->StartCountingTo( 60 );

	SPLASH_STRING("Creating Radar Window...");
	errcode = radarwindow_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 70 );

	SPLASH_STRING("Creating Info Bar...");
	InfoBar::Initialize();

	g_theProgressWindow->StartCountingTo( 80 );

	SPLASH_STRING("Creating Control Panel Window...");
	errcode = controlpanelwindow_Initialize();

	g_theProgressWindow->StartCountingTo( 90 );

	AUI_ERRCODE auiErr = g_c3ui->AddWindow( g_background );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;

	g_theProgressWindow->StartCountingTo( 100 );

	auiErr = g_c3ui->AddWindow( g_statusWindow );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;

	g_theProgressWindow->StartCountingTo( 110 );

	radarwindow_Display();

	g_theProgressWindow->StartCountingTo( 120 );

	errcode = AncientWindows_Initialize();

	g_modalWindow = 0;

	g_theProgressWindow->StartCountingTo( 130 );

	AttractWindow::Initialize();

	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}

sint32 CivApp::InitializeGame(CivArchive *archive)
{
	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		770 );

	g_theProgressWindow->StartCountingTo
	    (10, g_theStringDB->GetNameStr("LOADING"));

	init_keymap();

	g_theProgressWindow->StartCountingTo( 20 );

	SPLASH_STRING("Initializing Sprite Engine...");

	sprite_Initialize();

	g_theProgressWindow->StartCountingTo( 540 );

	if (m_dbLoaded && g_theProfileDB->IsScenario())
	{
		CleanupAppDB();
		InitializeAppDB();

		// The cached patterns of the allocated windows have to be unloaded to
		// be able to reinitialise the image maps safely.
		// InitializeGameUI will take care of this by closing most windows, but
		// the progress window stays open. It has to be handled manually to
		// prevent possible corruption of its background and border patterns.
		g_theProgressWindow->PatternInfoSave();

		InitializeImageMaps();
		InitializeSoundPF();

		// Restore the progress window background
		g_theProgressWindow->PatternInfoRestore();

		InitializeGameUI();
		greatlibrary_Cleanup();
		InitializeGreatLibrary();
		GreatLibrary::Initialize_Great_Library_Data();
	}
	else
	{
		InitializeGameUI();
	}

	g_theProgressWindow->StartCountingTo( 560 );

	ChatBox::Initialize();

	g_theProgressWindow->StartCountingTo( 570 );

	GrabItem::Init();

	g_theProgressWindow->StartCountingTo( 580 );

	if (m_dbLoaded && g_theProfileDB->IsScenario()) {
		if(g_controlPanel)
			g_controlPanel->CreateTileImpBanks();
	}

	g_theProgressWindow->StartCountingTo( 590 );

	gameEventManager_Initialize();

	// Prevent the event handler corrupting the (diplomacy) data in the
	// middle of a file restore operation.
	g_gevManager->Pause();

	events_Initialize();

	g_theProgressWindow->StartCountingTo( 600 );

	g_fog_toggle = FALSE;
	g_god = FALSE;

	if (!gameinit_Initialize(-1, -1, archive)) {
		g_gevManager->Resume();
		return FALSE;
	}

	if (archive) { // Ensure that SlicEngine's strings have correct numbering
		SlicEngine::Reload(g_slic_filename);
	}

	g_theProgressWindow->StartCountingTo( 610 );

	if(g_isScenario && archive &&
	   (g_startInfoType != STARTINFOTYPE_NONE ||
		g_saveFileVersion < gamefile_CurrentVersion())) {

		for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				g_player[i]->m_messages->Clear();
			}
		}

		delete g_theMessagePool;
		g_theMessagePool = new MessagePool();

		SlicEngine::Reload(g_slic_filename);

		if(g_scenarioUsePlayerNumber > 0 && g_player[g_scenarioUsePlayerNumber] &&
		   g_player[g_scenarioUsePlayerNumber]->m_civilisation &&
		   g_theCivilisationDB && g_theProfileDB) {
			Player *        p       = g_player[g_scenarioUsePlayerNumber];
			StringId        id      =
			    (p->m_civilisation->GetDBRec())->GetLeaderNameMale();
			const MBCHAR *name = g_theStringDB->GetNameStr(id);
			if(name) {
				p->m_civilisation->AccessData()->SetLeaderName(name);
				g_theProfileDB->SetLeaderName(name);
			}
		}
	}

	g_theProgressWindow->StartCountingTo( 620 );

	if(g_isScenario && !g_oldRandSeed) {

		g_rand->Initialize(static_cast<sint32>(time(0)));
	}

	g_theProgressWindow->StartCountingTo( 630 );

	if (g_isScenario)
	{
		for (size_t p = 0; p < k_MAX_PLAYERS; ++p)
		{
			if (g_player[p])
			{
				g_player[p]->m_civilisation->ResetCiv
					(g_player[p]->m_civilisation->GetCivilisation(),
					 g_player[p]->m_civilisation->GetGender()
					);
			}
		}
	}

	g_theProgressWindow->StartCountingTo( 640 );

#ifdef _DEBUG
	g_gevManager->Dump();
#endif

	g_theProgressWindow->StartCountingTo( 650 );

	GraphicsOptions::Initialize();

	SPLASH_STRING("Initializing Tile Engine...");
	tile_Initialize(archive != NULL);

	g_theProgressWindow->StartCountingTo( 660 );

	if (g_isScenario) {
		g_tiledMap->PostProcessMap();
	}

	g_theProgressWindow->StartCountingTo( 670 );

	radar_Initialize();

	g_theProgressWindow->StartCountingTo( 680 );

	Splash::Cleanup();

	g_theProgressWindow->StartCountingTo( 690 );

	m_gameLoaded = TRUE;
	g_gevManager->Resume();
	g_gevManager->Process();

	g_theProgressWindow->StartCountingTo( 700 );

	g_director->CatchUp();

	g_theProgressWindow->StartCountingTo( 710 );

	if(!g_network.IsActive() && !g_network.IsNetworkLaunch())
	{
		if ((archive == NULL) ||										// launch button
			((g_startInfoType != STARTINFOTYPE_NONE) && g_isScenario)	// scenario start
		   )
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail,
				GEV_BeginTurn,
				GEA_Player, g_selected_item->GetCurPlayer(),
				GEA_Int, g_player[g_selected_item->GetCurPlayer()]->m_current_round,
				GEA_End);
		}
	}

	g_theProgressWindow->StartCountingTo( 720 );

	if(!g_network.IsActive()) {
		if (NULL == archive ||
			(g_saveFileVersion >= 42 &&
			(g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS)))
		{
			if (g_director)
				g_director->AddCopyVision();
		}
	}

	g_theProgressWindow->StartCountingTo( 730 );

	Director::ReloadAllSprites();

	g_theProgressWindow->StartCountingTo( 740 );

	if(g_turn->IsEmail() && archive) {
		g_selected_item->KeyboardSelectFirstUnit();
		if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY &&
		   (g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() > 0)) {
			g_selected_item->SetSelectCity(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0));
			g_director->AddCenterMap(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0).RetPos());
		}
		g_director->AddCenterMap(g_selected_item->GetCurSelectPos());

		g_slicEngine->CheckPendingResearch();
	}

	g_theProgressWindow->StartCountingTo( 750 );

	if (!g_turn->IsHotSeat())
	{
		MainControlPanel::UpdateCityList();
	}

	g_scenarioUsePlayerNumber = 0;

	g_theProgressWindow->StartCountingTo( 760 );

	if (archive && g_turn->IsHotSeat())
	{
		// Indicate the resuming player when loading a saved hotseat game
		g_turn->SendNextPlayerMessage();
	}
	else if (g_selected_item)
	{
		if (!archive)
		{
			g_selected_item->Refresh();
		}

		if (g_director)
			g_director->AddCenterMap(g_selected_item->GetCurSelectPos());
	}

	g_theProgressWindow->StartCountingTo( 770 );

	g_oldRandSeed = FALSE;

	if (!g_turn->IsHotSeat())
	{
		MainControlPanel::UpdatePlayer(g_selected_item->GetCurPlayer());
	}

	if (g_launchIntoCheatMode)
	{
		ScenarioEditor::Display();
	}

	if (    g_turn->IsEmail()
	     && g_player[g_selected_item->GetCurPlayer()]->IsTurnOver()
	){
		g_gevManager->AddEvent(GEV_INSERT_Tail,
		                       GEV_BeginTurn,
		                       GEA_Player, g_selected_item->GetCurPlayer(),
		                       GEA_Int,    g_player[g_selected_item->GetCurPlayer()]->GetCurRound() + 1,
		                       GEA_End);
	}

	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}

sint32 InitializeSpriteEditorUI(void)
{
	SelectColorSet();

	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		120 );

	g_theProgressWindow->StartCountingTo(10, g_theStringDB->GetNameStr("LOADING"));

	SPLASH_STRING("Creating Main Windows...");
#if defined(_DEBUG)
	g_splash_old = Os::GetTicks();
#endif

	g_theProgressWindow->StartCountingTo( 20 );

	SPLASH_STRING("Creating Status Window...");
	sint32          errcode;

	errcode = c3windows_MakeStatusWindow(TRUE);
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 30 );

	SPLASH_STRING("Creating Game Window...");
	errcode = backgroundWin_Initialize(false);
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 40 );

	SPLASH_STRING("Creating Tile Help Window...");
	errcode = helptile_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 50 );
	SPLASH_STRING("Creating Debug Window...");
	errcode = c3windows_MakeDebugWindow(TRUE);

	g_theProgressWindow->StartCountingTo( 60 );

	AncientWindows_PreInitialize();

	g_theProgressWindow->StartCountingTo( 70 );

	SPLASH_STRING("Creating Control Panel Window...");

	errcode = controlpanelwindow_Initialize();

	g_theProgressWindow->StartCountingTo( 80 );

	SPLASH_STRING("Creating Radar Window...");
	errcode = radarwindow_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 90 );

	AUI_ERRCODE	auiErr = g_c3ui->AddWindow(g_background);
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;

	SPLASH_STRING("Creating Info Bar...");
	InfoBar::Initialize();

	g_theProgressWindow->StartCountingTo( 100 );

	g_modalWindow = 0;
	AttractWindow::Initialize();

	g_theProgressWindow->StartCountingTo( 110 );

	HideControlPanel();
	g_statusWindow->Hide();
	radarwindow_Hide();

	g_theProgressWindow->StartCountingTo( 120 );

	errcode = SpriteEditWindow_Initialize();
	auiErr = g_c3ui->AddWindow( g_spriteEditWindow );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;

	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}

sint32 CivApp::InitializeSpriteEditor(CivArchive *archive)
{
	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		860 );

	g_theProgressWindow->StartCountingTo(10, g_theStringDB->GetNameStr("LOADING"));

	g_fog_toggle = TRUE;

	init_keymap();

	g_theProgressWindow->StartCountingTo( 20 );

	SPLASH_STRING("Initializing Sprite Engine...");

	sprite_Initialize();

	g_theProgressWindow->StartCountingTo( 140 );

	InitializeSpriteEditorUI();

	g_theProgressWindow->StartCountingTo( 150 );

	ChatBox::Initialize();

	g_theProgressWindow->StartCountingTo( 160 );

	GrabItem::Init();

	g_theProgressWindow->StartCountingTo( 680 );

	if (m_dbLoaded && g_theProfileDB->IsScenario())
	{
		CleanupAppDB();
		InitializeAppDB();
	}

	g_theProgressWindow->StartCountingTo( 690 );

	gameEventManager_Initialize();

	g_theProgressWindow->StartCountingTo( 700 );

	events_Initialize();

	g_theProgressWindow->StartCountingTo( 710 );

	if (!spriteEditor_Initialize(20,15))
		return FALSE;

	g_theProgressWindow->StartCountingTo( 720 );

	if (archive
	     && (g_startInfoType != STARTINFOTYPE_NONE ||
	         g_saveFileVersion < gamefile_CurrentVersion()
	        )
	   )
	{
		SlicEngine::Reload(g_slic_filename);
	}
	g_slicEngine->RunTrigger(TRIGGER_LIST_GAME_LOADED, ST_END);

	g_theProgressWindow->StartCountingTo( 730 );

#ifdef _DEBUG
	g_gevManager->Dump();
#endif

	g_theProgressWindow->StartCountingTo( 740 );

	SPLASH_STRING("Initializing AI...");
	roboinit_Initalize(archive);
	CtpAi::Initialize();

	g_theProgressWindow->StartCountingTo( 750 );

	SPLASH_STRING("Initializing Tile Engine...");
	tile_Initialize(archive != NULL);

	g_theProgressWindow->StartCountingTo( 760 );

	radar_Initialize();

	g_theProgressWindow->StartCountingTo( 770 );

	Splash::Cleanup();

	g_theProgressWindow->StartCountingTo( 780 );

	m_gameLoaded = TRUE;

	g_director->CatchUp();

	g_theProgressWindow->StartCountingTo( 790 );

	g_turn->BeginNewTurn(FALSE);

	if(!g_network.IsActive()) {
		if (NULL == archive ||
			(g_saveFileVersion >= 42 &&
			(g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS))) {

			if(g_scenarioUsePlayerNumber == 0 && !g_turn->IsHotSeat() &&
				!g_turn->IsEmail()) {
				g_selected_item->SetPlayerOnScreen(1);
			}
			if (g_director)
				g_director->AddCopyVision();
		}
	}

	g_theProgressWindow->StartCountingTo( 800 );

	if(g_turn->IsEmail() && archive) {
		g_selected_item->KeyboardSelectFirstUnit();
		if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY &&
		   (g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() > 0)) {
			g_selected_item->SetSelectCity(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0));
			g_director->AddCenterMap(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0).RetPos());
		}
		g_director->AddCenterMap(g_selected_item->GetCurSelectPos());

		g_slicEngine->CheckPendingResearch();
	}

	g_theProgressWindow->StartCountingTo( 810 );

	g_scenarioUsePlayerNumber = 0;

	if(g_selected_item) {
		g_selected_item->Refresh();
		if(g_director)
			g_director->AddCenterMap(g_selected_item->GetCurSelectPos());
	}

	g_oldRandSeed = FALSE;


	if (g_launchIntoCheatMode) {
		ScenarioEditor::Display();
	}

	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}

void CivApp::CleanupGameUI(void)
{
	AttractWindow::Cleanup();
	GrabItem::Cleanup();
	greatlibrary_Cleanup();
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	musicscreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();
	optionsscreen_Cleanup();
	optionwarningscreen_Cleanup();
	graphicsscreen_Cleanup();
	gameplayoptions_Cleanup();
	soundscreen_Cleanup();
	AncientWindows_Cleanup();
	controlpanelwindow_Cleanup();

	c3windows_MakeDebugWindow(FALSE);

	messagewin_PurgeMessages();

	helptile_Cleanup();
	backgroundWin_Cleanup();
	radarwindow_Cleanup();
	c3windows_MakeStatusWindow(FALSE);

	InfoBar::Cleanup();
	InfoWindow::Cleanup();
	ScienceVictoryDialog::Cleanup();

	sci_advancescreen_Cleanup();
	infowin_Cleanup();
	infowin_Cleanup_Controls();
	ScienceManagementDialog::Cleanup();

	NationalManagementDialog::Cleanup();
	tutorialwin_Cleanup();

	workwin_Cleanup();

	ScenarioEditor::Cleanup();
	CityWindow::Cleanup();
	DiplomacyWindow::Cleanup();
	ArmyManagerWindow::Cleanup();
	TradeManager::Cleanup();
	IntelligenceWindow::Cleanup();
	UnitManager::Cleanup();
	EditQueue::Cleanup();
	ProfileEdit::Cleanup();

	DipWizard::Cleanup();
	DomesticManagementDialog::Cleanup();
	initialplayscreen_Cleanup();

	DiplomacyDetails::Cleanup();
}

void CivApp::CleanupGame(bool keepScenInfo)
{
	gameinit_CleanupMessages();
	uint32 target_milliseconds = 100000;
	uint32 used_milliseconds;
	ProcessUI(target_milliseconds, used_milliseconds);

	g_network.Cleanup();

	radarwindow_Cleanup();

	GraphicsOptions::Cleanup();

	tile_Cleanup();

	ChatBox::Cleanup();

	CleanupGameUI();

	ProcessUI(target_milliseconds, used_milliseconds);

	gameinit_Cleanup();

	events_Cleanup();

	gameEventManager_Cleanup();

	sprite_Cleanup();

	if (g_soundManager)
		g_soundManager->DumpAllSounds();

	if(!keepScenInfo) {

		g_theProfileDB->SetIsScenario(FALSE);
		g_civPaths->ClearCurScenarioPath();
		g_civPaths->ClearCurScenarioPackPath();
		memset(g_scenarioName, '\0', k_SCENARIO_NAME_MAX);
		CleanupAppDB();
		InitializeAppDB();
	}

#if defined(_DEBUG) && defined(_MEMORYLOGGING) && defined(_DEBUG_MEMORY)
	DebugMemory_LeaksShow(8675209);
#endif

	m_gameLoaded            = false;

	g_launchIntoCheatMode   = FALSE;
	g_god                   = FALSE;
	g_isCheatModeOn         = FALSE;

	g_c3ui->BlackScreen();
}

void CivApp::StartMessageSystem()
{
	if (!m_dbLoaded)
	{
		InitializeAppDB();
	}

	delete g_theMessagePool;
	g_theMessagePool = new MessagePool();
	SlicEngine::Reload(g_slic_filename);
}

void CivApp::BeginKeyboardScrolling(sint32 key)
{
	m_keyboardScrollingKey  = key;
	m_isKeyboardScrolling   = true;
}

void CivApp::StopKeyboardScrolling(sint32 key)
{
	m_keyboardScrollingKey  = 0;
	m_isKeyboardScrolling   = false;
}

void CivApp::ProcessGraphicsCallback(void)
{
	static bool s_inCallback = false;

	if (s_inCallback)   return;

	if (!g_tiledMap)    return;
	if (!g_background)  return;
	if (!g_director)    return;
	if (!g_background)  return;
	if (!g_c3ui)        return;

	s_inCallback = true;

	g_tiledMap->RestoreMixFromMap(g_background->TheSurface());
	g_background->Draw();
	g_c3ui->Process();

	if (!g_network.IsActive() || g_network.ReadyToStart())
	{
		g_director->Process();
	}

	s_inCallback = false;
}

sint32 CivApp::ProcessUI(const uint32 target_milliseconds, uint32 &used_milliseconds)
{
	uint32          start_time_ms   = Os::GetTicks();
	uint32          curTicks        = Os::GetTicks();
	static uint32	lastTicks       = curTicks;

	if (g_c3ui->TheMouse()) {
		if (g_c3ui->TheMouse()->IsSuspended() )
		{
			used_milliseconds = Os::GetTicks() - start_time_ms;

			if(g_runInBackground
			|| g_theProfileDB->GetValueByName("RunInBackground")
			){
				if (m_gameLoaded)
				{
					if (g_director)
					{
						g_director->Process();
					}
				}
			}
			return 0;
		}

		bool netGameLoading = (g_network.IsNetworkLaunch() || g_network.IsActive())
			                  && !g_network.ReadyToStart();

		if (m_gameLoaded && !g_modalWindow) {
			if (!netGameLoading && ui_CheckForScroll()) {
				do {

					g_tiledMap->CopyMixDirtyRects(g_background->GetDirtyList());

					g_c3ui->Process();

					uint32 target_milliseconds=30;
					uint32 used_milliseconds;

					ProcessNet(target_milliseconds, used_milliseconds);
				} while (ui_CheckForScroll());

				g_tiledMap->RetargetTileSurface(NULL);
				g_tiledMap->Refresh();
				g_tiledMap->InvalidateMap();
				g_tiledMap->ValidateMix();
			}
			else
			{
				if(g_tiledMap) {
					g_tiledMap->RestoreMixFromMap(g_background->TheSurface());
				}
				if(g_background)
					g_background->Draw();

			}

			lastTicks = curTicks;
		}
	}

	if (m_appLoaded) {
		g_c3ui->Process();
	}

	if (m_gameLoaded && !g_modalWindow && g_tiledMap) {

		if (g_director)
			g_director->Process();
	}

	return 0;
}

sint32 CivApp::ProcessAI()
{
	if((g_network.IsActive() || g_network.IsNetworkLaunch()) &&
		!g_network.ReadyToStart())
		return 0;

	if(victorywin_IsOnScreen())
		return 0;

	if (g_c3ui->TheMouse()) {
		if( g_c3ui->TheMouse()->IsSuspended()
		&& !g_runInBackground
		&& !g_theProfileDB->GetValueByName("RunInBackground")
		){
			return 0;
		}
	}
	return 1;
}

sint32 CivApp::ProcessRobot(const uint32 target_milliseconds, uint32 &used_milliseconds)
{
	if((g_network.IsActive() || g_network.IsNetworkLaunch()) &&
		!g_network.ReadyToStart())
		return 0;

	if(victorywin_IsOnScreen())
		return 0;

	uint32 const    start_time_ms = Os::GetTicks();

	if (g_c3ui->TheMouse())
	{
		if (g_c3ui->TheMouse()->IsSuspended() && !g_runInBackground && !g_theProfileDB->GetValueByName("RunInBackground"))
		{
			// Probably there was something here.
		}
	}

	used_milliseconds = Os::GetTicks() - start_time_ms;
	return 0;
}

sint32 CivApp::ProcessNet(const uint32 target_milliseconds, uint32 &used_milliseconds)
{
	uint32 const        start_time = Os::GetTicks();

	if (m_gameLoaded)
	{
		g_network.Process();
	}

	used_milliseconds   = Os::GetTicks() - start_time;
	return 0;
}

sint32 CivApp::ProcessSLIC(void)
{
	if (!g_slicEngine)
		return 0;

	g_slicEngine->ProcessUITriggers();

	static time_t   lastRanSlicTimers   = 0;
	time_t          now                 = time(0);
	if (now > lastRanSlicTimers + g_slicEngine->GetTimerGranularity())
	{
		g_slicEngine->RunTimerTriggers();
		/// @todo Check lastRanSlicTimers = now;
	}

	if (g_slicEngine->WaitingForLoad())
	{
		main_RestoreGame(g_slicEngine->GetLoadName());
	}

	return 0;
}

sint32 CivApp::ProcessProfile(void)
{
    uint32 target_milliseconds = (g_no_timeslice) ? 10000000 : 30;
	uint32 used_milliseconds;

	ProcessNet(target_milliseconds, used_milliseconds);

	ProcessUI(target_milliseconds, used_milliseconds);


	ProcessSLIC();

	ProcessRobot(target_milliseconds, used_milliseconds);

	if (g_soundManager)
		g_soundManager->Process(target_milliseconds, used_milliseconds);

	return 0;
}

sint32 CivApp::Process(void)
{
#ifdef _DEBUG
	if(g_tempLeakCheck) {
#if defined(WIN32)	
		_CrtMemState new_state;
		_CrtMemCheckpoint(&new_state);

		g_allocatedAtStart = new_state.lSizes[1];
#endif // WIN32
	}
#endif // _DEBUG

	if(g_netConsole)
	{
		static uint32 last_tick = 0;

		if (Os::GetTicks() > last_tick + 250)
		{
			g_netConsole->Idle();
			last_tick = Os::GetTicks();
		}
	}

	// Sleep problem if the game is supposed to run in background
	if (m_inBackground)
	{
		Os::Sleep(50);
		return 0;
	}

	if (g_use_profile_process)
	{
		ProcessProfile();
		return 0;
	}

	uint32 target_milliseconds = (g_no_timeslice) ? 10000000 : 30;
	uint32 used_milliseconds;

	ProcessNet(target_milliseconds, used_milliseconds);

	ProcessUI(target_milliseconds, used_milliseconds);

	if(g_attractWindow)
		g_attractWindow->AppIdle();

	ProcessSLIC();

	ProcessRobot(target_milliseconds, used_milliseconds);

	if (g_soundManager)
		g_soundManager->Process(target_milliseconds, used_milliseconds);

	if(g_gevManager)
		g_gevManager->Process();

	if (m_gameLoaded && g_savedGameRequest && g_selected_item)
	{
		Player *    p = g_player[g_selected_item->GetCurPlayer()];
		if(p && !p->IsRobot()
		|| g_selected_item->GetVisiblePlayer() == g_selected_item->GetCurPlayer())
		{
			if (g_director)
				g_director->CatchUp();

			if (g_launchIntoCheatMode)
				g_theGameSettings->SetKeepScore(TRUE);

			GameFile::SaveGame(g_savedGameRequest->pathName, g_savedGameRequest);

			if (g_launchIntoCheatMode)
				g_theGameSettings->SetKeepScore(FALSE);

			allocated::clear(g_savedGameRequest);
		}
	}

	return 0;
}

sint32 CivApp::StartGame(void)
{
	return InitializeGame(NULL);
}

sint32 CivApp::StartSpriteEditor(void)
{
	return InitializeSpriteEditor(NULL);
}

sint32 CivApp::EndGame(void)
{
	if (m_gameLoaded) {
		CleanupGame(false);
		StartMessageSystem();
	}

	AUI_ERRCODE errcode = initialplayscreen_Initialize();
	Assert(errcode == AUI_ERRCODE_OK);

	return initialplayscreen_displayMyWindow();
}

sint32 CivApp::LoadSavedGame(MBCHAR const * name)
{
	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		1300 );

	g_theProgressWindow->StartCountingTo( 10, g_theStringDB->GetNameStr("LOADING") );

	FILE * fin = fopen(name, "r");
	if (fin == NULL) {
		c3errors_ErrorDialog("Load save game", "Could not open %s", name);
		return 0;
	}
	fclose(fin);

	g_theProgressWindow->StartCountingTo( 20 );

	if (m_gameLoaded) {
		CleanupGame(true);

		g_theProgressWindow->StartCountingTo( 30 );

		CleanupAppDB();

		g_theProgressWindow->StartCountingTo( 550 );

		InitializeAppDB();

		g_theProgressWindow->StartCountingTo( 560 );

		greatlibrary_Cleanup();
		GreatLibrary::Initialize_Great_Library_Data();

		g_theProgressWindow->StartCountingTo( 570 );

		StartMessageSystem();
	}

	g_theProgressWindow->StartCountingTo( 1280 );

	GameFile::RestoreGame(name);

	g_theProgressWindow->StartCountingTo( 1290 );

	g_tiledMap->ReloadGoodActors(); // reloads goods according to g_theProfileDB->IsGoodAnim()
	g_tiledMap->InvalidateMap();

	g_theProgressWindow->StartCountingTo( 1300 );

	if (!g_turn->IsHotSeat())
	{
		g_selected_item->NextUnmovedUnit(TRUE, TRUE);
	}

	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}

sint32 CivApp::LoadSavedGameMap(MBCHAR const * name)
{
	FILE * fin = fopen(name, "r");

	if (fin)
	{
		fclose(fin);
		GameMapFile::RestoreGameMap(name);
		g_tiledMap->InvalidateMap();
		g_selected_item->NextUnmovedUnit(TRUE, FALSE);
	}
	else
	{
		c3errors_ErrorDialog("Load save game map", "Could not open %s", name);
	}

	return 0;
}

sint32 CivApp::LoadScenarioGame(MBCHAR const * file)
{
	if (m_gameLoaded) {
		CleanupGame(true);
		StartMessageSystem();
	}

	GameFile::RestoreScenarioGame(file);

	if (g_tiledMap)
		g_tiledMap->InvalidateMap();

	g_selected_item->NextUnmovedUnit(TRUE, FALSE);

	return 0;
}

sint32 CivApp::RestartGame(void)
{
	if (m_gameLoaded) {
		CleanupGame(true);
		StartMessageSystem();
	}

	return StartGame();
}

sint32 CivApp::RestartGameSameMap(void)
{
	Assert(g_rand);
	g_oldRandSeed = g_rand ? g_rand->GetSeed() : 0;

	if (m_gameLoaded)
	{
		CleanupGame(true);
		StartMessageSystem();
	}

	if (g_theProfileDB->IsScenario())
	{
		spnewgamescreen_scenarioExitCallback(NULL, AUI_BUTTON_ACTION_EXECUTE, 0, NULL);
		return 0;
	}
	else
	{
		return StartGame();
	}
}

sint32 CivApp::QuitToSPShell(void)
{
	if (m_gameLoaded)
	{
		CleanupGame(false);
		StartMessageSystem();
	}

	// We've removed that so go to main menu instead
	// (Change by JJB)
	return initialplayscreen_displayMyWindow();
}

sint32 CivApp::QuitToLobby(void)
{
	if (m_gameLoaded) {
		CleanupGame(false);
		StartMessageSystem();
	}

	return NetShell::Enter( k_NS_FLAGS_RETURN );
}

void CivApp::QuitGame(void)
{
	if (m_gameLoaded)
		CleanupGame(true);

	CleanupApp();
}

void CivApp::AutoSave(sint32 player, bool isQuickSave)
{
	if ((g_network.IsActive() && !g_network.IsHost()) || g_network.IsNetworkLaunch())
		return;

	MBCHAR const *  autosaveItem    = (isQuickSave) ? "QUICKSAVE_NAME" : "AUTOSAVE_NAME";
	MBCHAR const *  autosaveName    = g_theStringDB->GetNameStr(autosaveItem);

	MBCHAR			leaderName[k_MAX_NAME_LEN];
	strcpy(leaderName, g_theProfileDB->GetLeaderName());
	leaderName[SAVE_LEADER_NAME_SIZE] = '\0';
	c3files_StripSpaces(leaderName);

	MBCHAR			filename[_MAX_PATH];
	sprintf(filename, "%s-%s", autosaveName, leaderName);

	C3SAVEDIR       dir = (g_network.IsActive()) ? C3SAVEDIR_MP : C3SAVEDIR_GAME;

	MBCHAR			path[_MAX_PATH];
	g_civPaths->GetSavePath(dir, path);

	MBCHAR			fullpath[_MAX_PATH];
	sprintf(fullpath, "%s%s%s", path, FILE_SEP, leaderName);

	if (c3files_PathIsValid(fullpath) || c3files_CreateDirectory(fullpath))
	{
		strcat(fullpath, FILE_SEP);
		strcat(fullpath, filename);

		g_isScenario = FALSE;
		GameFile().Save(fullpath, NULL);
	}
	else
	{
		Assert(false);
	}
}

void CivApp::RestoreAutoSave(sint32 player)
{
	MBCHAR		filename[_MAX_PATH];
	sprintf(filename, "auto%d.sav", player);

	g_c3ui->AddAction(new LoadSaveGameAction(filename));
}


void CivApp::PostStartGameAction(void)
{
	g_c3ui->AddAction(new StartGameAction());
}

void CivApp::PostSpriteTestAction(void)
{
	g_c3ui->AddAction(new SpriteTestAction());
}

void CivApp::PostLoadSaveGameAction(MBCHAR const * name)
{
	g_c3ui->AddAction(new LoadSaveGameAction(name));
}

void CivApp::PostLoadQuickSaveAction(sint32 player)
{
	/// @todo Check g_network.IsHost? See AutoSave. Otherwise, the dir assignment
	///       becomes constant.
	if (g_network.IsActive())
	{
		return;
	}

	MBCHAR			leaderName[SAVE_LEADER_NAME_SIZE + 1];
	strncpy(leaderName, g_theProfileDB->GetLeaderName(), SAVE_LEADER_NAME_SIZE);
	leaderName[SAVE_LEADER_NAME_SIZE] = '\0';
	c3files_StripSpaces(leaderName);

	MBCHAR			filename[_MAX_PATH];
	sprintf(filename, "%s-%s", g_theStringDB->GetNameStr("QUICKSAVE_NAME"), leaderName);

	C3SAVEDIR       dir = (g_network.IsActive()) ? C3SAVEDIR_MP : C3SAVEDIR_GAME;

	MBCHAR			path[_MAX_PATH];
	g_civPaths->GetSavePath(dir, path);

	MBCHAR			fullpath[_MAX_PATH];
	sprintf(fullpath, "%s%s%s", path, FILE_SEP, leaderName);

	if (c3files_PathIsValid(fullpath) || c3files_CreateDirectory(fullpath))
	{
		strcat(fullpath, FILE_SEP);
		strcat(fullpath, filename);

		FILE * f = fopen(fullpath, "r");
		if (f)
		{
			fclose(f);
			PostLoadSaveGameAction(fullpath);
		}
	}
	else
	{
		Assert(FALSE);
	}
}

#if 0   // never used
void CivApp::PostLoadSaveGameMapAction(MBCHAR const * name)
{
	g_c3ui->AddAction(new LoadSaveGameMapAction(name));
}
#endif

void CivApp::PostRestartGameAction(void)
{
	g_c3ui->AddAction(new RestartGameAction());
}

void CivApp::PostRestartGameSameMapAction(void)
{
	Player * p = g_player[g_selected_item->GetVisiblePlayer()];

	if (p && g_theProfileDB)
	{
		g_theProfileDB->SetLeaderName(p->GetLeaderName());
		g_theProfileDB->SetCivIndex(p->GetCivilisation()->GetCivilisation());
	}

	g_c3ui->AddAction(new RestartGameSameMapAction());
}

void CivApp::PostQuitToSPShellAction(void)
{
	g_c3ui->AddAction(new QuitToSPShellAction());
}

void CivApp::PostQuitToLobbyAction(void)
{
	g_c3ui->AddAction(new QuitToLobbyAction());
}

void CivApp::PostEndGameAction(void)
{
	g_c3ui->AddAction(new EndGameAction());
}

void CivApp::PostLoadScenarioGameAction(MBCHAR const * filename)
{
	g_c3ui->AddAction(new LoadScenarioGameAction(filename));
}

void StartGameAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->StartGame();
}

void SpriteTestAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->StartSpriteEditor();
}

void LoadSaveGameAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->LoadSavedGame(m_filename);
}

#if 0   // never used
void LoadSaveGameMapAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->LoadSavedGameMap(m_filename);
}
#endif
void RestartGameAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->RestartGame();
}
void RestartGameSameMapAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->RestartGameSameMap();
}
void QuitToSPShellAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->QuitToSPShell();
}
void QuitToLobbyAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->QuitToLobby();
}
void EndGameAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	g_civApp->EndGame();
}
void LoadScenarioGameAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	g_civApp->LoadScenarioGame(m_filename);
}

void InitializeGreatLibrary()
{
	delete g_GreatLibPF;
	g_GreatLibPF = new ProjectFile();
	AddSearchDirectories(g_GreatLibPF, C3DIR_GL, "gl.zfs");
}

void InitializeSoundPF()
{
	delete g_SoundPF;
	g_SoundPF = new ProjectFile();
	AddSearchDirectories(g_SoundPF, C3DIR_SOUNDS, "sound.zfs");
}

void InitializeImageMaps()
{
	delete g_ImageMapPF;
	g_ImageMapPF = new ProjectFile();

	if (g_c3ui->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555)
	{
		AddSearchPacks(g_ImageMapPF, C3DIR_PATTERNS, "pat555.zfs");
		AddSearchPacks(g_ImageMapPF, C3DIR_PICTURES, "pic555.zfs");
	}
	else
	{
		AddSearchPacks(g_ImageMapPF, C3DIR_PATTERNS, "pat565.zfs");
		AddSearchPacks(g_ImageMapPF, C3DIR_PICTURES, "pic565.zfs");
	}
}
