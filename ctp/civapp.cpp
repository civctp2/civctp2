

#include "c3.h"
#include "civ3_main.h"


#include "c3cpu.h"
#include "splash.h"
#include "DebugMemory.h"


#include "civpaths.h"
#include "Unit.h"
#include "Message.h"
#include "gameinit.h"
#include "randgen.h"
#include "keymap.h"
#include "keypress.h"
#include "turncnt.h"
#include "DB.h"
#include "StrDB.h"
#include "AdvanceRecord.h"
#include "GWDB.h"
#include "UVDB.h"
#include "BuildingRecord.h"
#include "DiffDB.h"
#include "SpriteStateDB.h"
#include "ThroneDB.h"
#include "ConceptDB.h"

#include "WonderRecord.h"
#include "WonderMovieRecord.h"
#include "CivilisationDB.h"
#include "ProfileDB.h"
#include "CivArchive.h"
#include "GameFile.h"
#include "constdb.h"
#include "SlicEngine.h"
#include "Player.h"
#include "Advances.h"
#include "MessagePool.h"
#include "PlayListDB.h"

#include "CivScenarios.h"


#include "roboinit.h"
#include "ctpai.h"


#include "progresswindow.h"
#include "netshell.h"
#include "netshell_game.h"
#include "c3ui.h"
#include "aui_blitter.h"
#include "c3window.h"
#include "c3_static.h"
#include "c3_checkbox.h"
#include "background.h"
#include "GrabItem.h"
#include "c3_dropdown.h"
#include "c3_listbox.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "bevellesswindow.h"
#include "spnewgamewindow.h"
#include "spnewgametribescreen.h"
#include "c3windows.h"
#include "statswindow.h"
#include "backgroundwin.h"
#include "helptile.h"
#include "messagewin.h"
#include "controlpanelwindow.h"
#include "MainControlPanel.h"

#include "statuswindow.h"
#include "radarwindow.h"
#include "ancientwindows.h"
#include "greatlibrarywindow.h"
#include "scenariowindow.h"
#include "spwindow.h"
#include "initialplaywindow.h"
#include "optionswindow.h"
#include "optionwarningscreen.h"
#include "gameplayoptions.h"
#include "graphicsscreen.h"
#include "soundscreen.h"
#include "radarmap.h"
#include "TiledMap.h"
#include "workwin.h"
#include "CursorManager.h"
#include "gamesounds.h"
#include "SoundManager.h"
#include "civapp.h"
#include "ChatBox.h"
#include "BattleView.h"
#include "screenutils.h"
#include "sci_advancescreen.h"
#include "infowin.h"
#include "sciencewin.h"
#include "trademanager.h"
#include "intelligencewindow.h"
#include "NationalManagementDialog.h"
#include "ScienceManagementDialog.h"



#include "DomesticManagementDialog.h"
#include "dipwizard.h"
#include "DiplomacyDetails.h"
#include "loadsavewindow.h"




#include "SpriteEditor.h"

#include "network.h"
#include "ProfileDB.h"
#include "RiskDB.h"
#include "MovieDB.h"
#include "EndGameDB.h"
#include "FilenameDB.h"
#include "PollutionDB.H"
#include "Exclusions.h"
#include "MapDB.h"
#include "tutorialwin.h"
#include "km_screen.h"

#include "ctp_finger.h"
#include "AttractWindow.h"
#include "intromoviewin.h"
#include "endgamewindow.h"

#include "appstrings.h"
#include "victorywin.h"

#include "GameEventManager.h"

#include "Events.h"


#ifndef _NO_GAME_WATCH


#include "GameWatch.h"
#include "GWCiv.h"

int g_gameWatchID = -1;
#endif 

#include "DBLexer.h"
#include "GovernmentRecord.h"
#include "SpriteRecord.h"
#include "SoundRecord.h"
#include "SpecialEffectRecord.h"
#include "SpecialAttackInfoRecord.h"
#include "IconRecord.h"
#include "AdvanceBranchRecord.h"
#include "AgeRecord.h"
#include "IconRecord.h"
#include "TerrainRecord.h"
#include "TerrainImprovementRecord.h"
#include "UnitRecord.h"
#include "OrderRecord.h"
#include "CitySizeRecord.h"
#include "PopRecord.h"
#include "FeatRecord.h"
#include "EndGameObjectRecord.h"
#include "CityStyleRecord.h"
#include "AgeCityStyleRecord.h"


#include "GoalRecord.h"
#include "UnitBuildListRecord.h"
#include "WonderBuildListRecord.h"
#include "BuildingBuildListRecord.h"
#include "ImprovementListRecord.h"
#include "StrategyRecord.h"
#include "BuildListSequenceRecord.h"
#include "DiplomacyRecord.h"
#include "DiplomacyProposalRecord.h"
#include "DiplomacyThreatRecord.h"
#include "AdvanceListRecord.h"
#include "PersonalityRecord.h"

#include "UnitDynArr.h"

#include "unitutil.h"
#include "advanceutil.h"

#include "netconsole.h"

#include "ResourceRecord.h"

#include "InfoBar.h"

#include "display.h"


#include "gfx_options.h"

#include "diplomacyutil.h"

#include "ScenarioEditor.h"
#include "DiplomacyWindow.h"
#include "ArmyManagerWindow.h"
#include "CityWindow.h"
#include "terrainutil.h"
#include "buildingutil.h"

#include "UnitManager.h"
#include "EditQueue.h"
#include "ProfileEdit.h"

#include "ScreenManager.h"
extern ScreenManager *g_screenManager;

#define RELDBG(x) { FILE *f = fopen("reldbg.txt", "a"); fprintf x; fclose(f); }


extern Splash				*g_splash;
extern sint32				g_splash_cur; 
extern sint32				g_splash_old; 
extern char					g_splash_buf[100]; 
extern sint32				g_fog_toggle;
	extern sint32 g_god;



extern TurnCount				*g_turn;
extern RandomGenerator			*g_rand;


extern ConceptDB					*g_theConceptDB;
extern CivilisationDatabase			*g_theCivilisationDB;
extern GlobalWarmingDatabase		*g_theGWDB ;
extern OzoneDatabase				*g_theUVDB ;
extern PollutionDatabase			*g_thePollutionDB ;
extern ConstDB						*g_theConstDB; 
extern ThroneDB						*g_theThroneDB;
extern DifficultyDB					*g_theDifficultyDB; 

extern SpriteStateDB				*g_theSpriteStateDB;
extern SpriteStateDB				*g_theGoodsSpriteStateDB;
extern SpriteStateDB				*g_theCitySpriteStateDB;
extern ProfileDB					*g_theProfileDB;
extern RiskDatabase                 *g_theRiskDB;
extern MovieDB						*g_theVictoryMovieDB;
extern FilenameDB					*g_theMessageIconFileDB;
extern FilenameDB					*g_theGoodsIconDB;
extern PlayListDB					*g_thePlayListDB;

extern MessagePool	*g_theMessagePool;

extern MBCHAR g_improve_filename[_MAX_PATH];
extern MBCHAR g_pollution_filename[_MAX_PATH];
extern MBCHAR g_global_warming_filename[_MAX_PATH];
extern MBCHAR g_ozone_filename[_MAX_PATH];
extern MBCHAR g_terrain_filename[_MAX_PATH];
extern MBCHAR g_installation_filename[_MAX_PATH];
extern MBCHAR g_government_filename[_MAX_PATH];
extern MBCHAR g_governmenticondb_filename[_MAX_PATH];
extern MBCHAR g_wonder_filename[_MAX_PATH];
extern MBCHAR g_constdb_filename[_MAX_PATH]; 
extern MBCHAR g_pop_filename[_MAX_PATH];
extern MBCHAR g_civilisation_filename[_MAX_PATH] ;
extern MBCHAR g_agedb_filename[_MAX_PATH];
extern MBCHAR g_thronedb_filename[_MAX_PATH];
extern MBCHAR g_conceptdb_filename[_MAX_PATH];
extern MBCHAR g_terrainicondb_filename[_MAX_PATH];
extern MBCHAR g_advanceicondb_filename[_MAX_PATH];
extern MBCHAR g_advancedb_filename[_MAX_PATH];
extern MBCHAR g_concepticondb_filename[_MAX_PATH];
extern MBCHAR g_tileimprovementdb_filename[_MAX_PATH];
extern MBCHAR g_spritestatedb_filename[_MAX_PATH];
extern MBCHAR g_specialeffectdb_filename[_MAX_PATH];	
extern MBCHAR g_specialattackinfodb_filename[_MAX_PATH];





extern MBCHAR g_goodsspritestatedb_filename[_MAX_PATH];
extern MBCHAR g_cityspritestatedb_filename[_MAX_PATH];
extern MBCHAR g_uniticondb_filename[_MAX_PATH];
extern MBCHAR g_unitsdb_filename[_MAX_PATH];
extern MBCHAR g_wondericondb_filename[_MAX_PATH];
extern MBCHAR g_improveicondb_filename[_MAX_PATH];
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
extern MBCHAR g_endgame_filename[_MAX_PATH];
extern MBCHAR g_messageiconfdb_filename[_MAX_PATH];
extern MBCHAR g_goodsicondb_filename[_MAX_PATH];
extern MBCHAR g_orderdb_filename[_MAX_PATH];
extern MBCHAR g_mapdb_filename[_MAX_PATH];
extern MBCHAR g_playlistdb_filename[_MAX_PATH];

extern MBCHAR g_branchdb_filename[_MAX_PATH];	
extern MBCHAR g_endgameicondb_filename[_MAX_PATH];
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

extern Player	**g_player;


extern C3UI					*g_c3ui;
extern Background			*g_background;
extern StatsWindow			*g_statsWindow;
extern StatusWindow			*g_statusWindow;
extern ControlPanelWindow	*g_controlPanel;





extern SpriteEditWindow		*g_spriteEditWindow;


extern RadarMap				*g_radarMap;

extern aui_Surface			*g_sharedSurface;

extern TiledMap				*g_tiledMap;

extern sint32				g_modalWindow;

extern CivApp				*g_civApp;

extern SoundManager			*g_soundManager;
extern ChatBox				*g_chatBox;
extern SlicEngine			*g_slicEngine;
extern BOOL                 g_no_exit_action;



extern sint32 g_scenarioUsePlayerNumber;

#include "GameSettings.h"
extern GameSettings			*g_theGameSettings;




#include "SelItem.h"
extern SelectedItem *g_selected_item;

#include "TutorialWin.h"
extern TutorialWin	*g_tutorialWin;

#include "PrjFile.h"
ProjectFile *g_GreatLibPF = NULL;
ProjectFile *g_ImageMapPF = NULL;
ProjectFile *g_SoundPF = NULL;
int InitializeGreatLibrary();
int InitializeSoundPF();
int InitializeImageMaps();

#include "Director.h"
extern Director				*g_director;
extern BOOL					g_runInBackground;

sint32 g_logCrashes = 1; 


sint32 g_oldRandSeed = FALSE;

ProgressWindow *g_theProgressWindow = NULL;

extern sint32 g_scenarioUsePlayerNumber;


bool g_tempLeakCheck = false;
sint32 g_allocatedAtStart;

void check_leak()
{
#ifdef _DEBUG
	_CrtMemState new_state; 

	if(g_tempLeakCheck) {
		_CrtMemCheckpoint(&new_state);
		Assert(g_allocatedAtStart == new_state.lSizes[1]);
		g_allocatedAtStart = new_state.lSizes[1];
	}
#endif
}	



CivApp::CivApp()
{
	m_appLoaded = FALSE;
	m_dbLoaded = FALSE;
	m_gameLoaded = FALSE;
	m_saveDBInGameFile = FALSE;
	m_inBackground = FALSE;
	m_aiFinishedThisTurn = TRUE;

	m_keyboardScrollingKey = 0;
	m_isKeyboardScrolling = FALSE;





}

CivApp::~CivApp()
{
}



sint32 CivApp::InitializeAppUI(void)
{
	AUI_ERRCODE errcode;

	
	NETFunc::GameType = GAMEID;
	
	NETFunc::DllPath = "dll\\net";

	
	extern BOOL g_useIntroMovie;
	extern BOOL g_no_shell;
	extern BOOL g_launchScenario;

	if (g_useIntroMovie && !g_no_shell) 
	{
		intromoviewin_Initialize();
		intromoviewin_DisplayIntroMovie();
	} else {

		if (g_soundManager) {
			g_soundManager->EnableMusic();
			g_soundManager->PickNextTrack();
			g_soundManager->StartMusic();
		}

		errcode = initialplayscreen_Initialize();
		Assert(errcode == AUI_ERRCODE_OK);

		if(!g_no_shell && !g_launchScenario)
			initialplayscreen_displayMyWindow();
	}
	return 0;
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






sint32 CivApp::InitializeAppDB(CivArchive &archive)
{
	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		430 );
	DBLexer *lex = NULL;

	
	g_theUnitDB = new CTPDatabase<UnitRecord>;
	g_theIconDB = new CTPDatabase<IconRecord>;
	g_theAdvanceDB = new CTPDatabase<AdvanceRecord>;
	g_theSpriteDB = new CTPDatabase<SpriteRecord>;
	g_theSoundDB = new CTPDatabase<SoundRecord>;
	g_theSpecialEffectDB = new CTPDatabase<SpecialEffectRecord>;
	g_theSpecialAttackInfoDB = new CTPDatabase<SpecialAttackInfoRecord>;
	g_theGovernmentDB = new CTPDatabase<GovernmentRecord>;
	g_theAdvanceBranchDB = new CTPDatabase<AdvanceBranchRecord>;
	g_theAgeDB = new CTPDatabase<AgeRecord>;
	g_theTerrainDB = new CTPDatabase<TerrainRecord>;
	g_theResourceDB = new CTPDatabase<ResourceRecord>;
	g_theTerrainImprovementDB = new CTPDatabase<TerrainImprovementRecord>;
	g_theOrderDB = new CTPDatabase<OrderRecord>;
	g_theCityStyleDB = new CTPDatabase<CityStyleRecord>;
	g_theAgeCityStyleDB = new CTPDatabase<AgeCityStyleRecord>;

	g_theGoalDB = new CTPDatabase<GoalRecord>;
	g_theUnitBuildListDB = new CTPDatabase<UnitBuildListRecord>;
	g_theWonderBuildListDB = new CTPDatabase<WonderBuildListRecord>;
	g_theBuildingBuildListDB = new CTPDatabase<BuildingBuildListRecord>;
	g_theImprovementListDB = new CTPDatabase<ImprovementListRecord>;
	g_theStrategyDB = new CTPDatabase<StrategyRecord>;
	g_theBuildListSequenceDB = new CTPDatabase<BuildListSequenceRecord>;
	g_theDiplomacyDB = new CTPDatabase<DiplomacyRecord>;
	g_theDiplomacyProposalDB = new CTPDatabase<DiplomacyProposalRecord>;
	g_theDiplomacyThreatDB = new CTPDatabase<DiplomacyThreatRecord>;
	g_theAdvanceListDB = new CTPDatabase<AdvanceListRecord>;

	g_theCitySizeDB = new CTPDatabase<CitySizeRecord>;
	g_thePopDB = new CTPDatabase<PopRecord>;
	g_theBuildingDB = new CTPDatabase<BuildingRecord>;
	g_theWonderDB = new CTPDatabase<WonderRecord>;
	g_theWonderMovieDB = new CTPDatabase<WonderMovieRecord>;
	g_thePersonalityDB = new CTPDatabase<PersonalityRecord>;

	g_theFeatDB = new CTPDatabase<FeatRecord>;
	g_theEndGameObjectDB = new CTPDatabase<EndGameObjectRecord>;

	
    g_theStringDB = new StringDB();
    Assert(g_theStringDB); 
	if (g_theStringDB) {
		if(!g_theStringDB->Parse(g_stringdb_filename))
			return FALSE;
	}
	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );
	g_theProgressWindow->StartCountingTo( 10, s );

	g_theProgressWindow->StartCountingTo( 20 );

	
	Assert(g_theSoundDB);
	if (g_theSoundDB) {
		lex = new DBLexer(C3DIR_GAMEDATA, g_sounddb_filename);
		if (!g_theSoundDB->Parse(lex))
			return FALSE;
		delete lex;
	}

	g_theProgressWindow->StartCountingTo( 30 );

	g_theProgressWindow->StartCountingTo( 40 );

	g_theProgressWindow->StartCountingTo( 50 );

	g_theProgressWindow->StartCountingTo( 60 );

	g_theProgressWindow->StartCountingTo( 70 );

	g_theProgressWindow->StartCountingTo( 80 );

	if (g_theIconDB) {
		if (!g_theIconDB->Parse(C3DIR_GAMEDATA, g_uniticondb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 90 );

	
	
    if (!ConstDB_Parse (g_constdb_filename, C3DIR_GAMEDATA)) {
        return FALSE; 
	}
    Assert(g_theConstDB); 

	g_theProgressWindow->StartCountingTo( 100 );

	g_theProgressWindow->StartCountingTo( 110 );

	
	if (g_theWonderMovieDB) {
		if (!g_theWonderMovieDB->Parse(C3DIR_GAMEDATA, g_wondermoviedb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 120 );

	
	if (&archive)
		g_theVictoryMovieDB = new MovieDB(archive);
	else
		g_theVictoryMovieDB = new MovieDB;
	Assert(g_theVictoryMovieDB);
	if (g_theVictoryMovieDB) {
		if (!g_theVictoryMovieDB->Parse(g_victorymoviedb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 130 );

	
	if (&archive)
		g_thePlayListDB = new PlayListDB(archive);
	else
		g_thePlayListDB = new PlayListDB();
	if (g_thePlayListDB) {
		if (!g_thePlayListDB->Parse(g_playlistdb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 140 );

	
    if (&archive)
        g_theSpriteStateDB = new SpriteStateDB(archive) ;
    else
        g_theSpriteStateDB = new SpriteStateDB;     
    Assert(g_theSpriteStateDB); 
    if (g_theSpriteStateDB) {
		if (!g_theSpriteStateDB->Parse(g_spritestatedb_filename))
			return FALSE;
	}

	if(g_theSpriteDB) {
		
		
		if(!g_theSpriteDB->Parse(C3DIR_GAMEDATA, "newsprite.txt"))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 150 );

    if (g_theSpecialEffectDB) {
		if (!g_theSpecialEffectDB->Parse(C3DIR_GAMEDATA, g_specialeffectdb_filename))
			return FALSE;
	}

	if(g_theSpecialAttackInfoDB) {
		if(!g_theSpecialAttackInfoDB->Parse(C3DIR_GAMEDATA, g_specialattackinfodb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 160 );

	if (&archive) 
		g_theGoodsSpriteStateDB = new SpriteStateDB(archive);
	else
		g_theGoodsSpriteStateDB = new SpriteStateDB;
	Assert(g_theGoodsSpriteStateDB);
    if (g_theGoodsSpriteStateDB) {
		if (!g_theGoodsSpriteStateDB->Parse(g_goodsspritestatedb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 170 );

	if (&archive)
		g_theCitySpriteStateDB = new SpriteStateDB(archive);
	else
		g_theCitySpriteStateDB = new SpriteStateDB;
	Assert(g_theCitySpriteStateDB);
    if (g_theCitySpriteStateDB) {
		if (!g_theCitySpriteStateDB->Parse(g_cityspritestatedb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 180 );

	
	if(g_theAdvanceBranchDB) {
		if(!g_theAdvanceBranchDB->Parse(C3DIR_GAMEDATA, g_branchdb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 190 );

	if (g_theAdvanceDB) {
		if(!g_theAdvanceDB->Parse(C3DIR_GAMEDATA, g_advancedb_filename))
			return FALSE; 
	}

	g_theProgressWindow->StartCountingTo( 200 );

	
	if (g_theUnitDB) {
		if (!g_theUnitDB->Parse(C3DIR_GAMEDATA, g_unitdb_filename)) 
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 210 );

	
    if (&archive)
        g_theDifficultyDB = new DifficultyDB(archive) ;
    else
        g_theDifficultyDB = new DifficultyDB;
    Assert(g_theDifficultyDB); 
    if (g_theDifficultyDB) {
		if (!g_theDifficultyDB->Parse(g_difficultydb_filename))
			return FALSE;
	}

	g_theProgressWindow->StartCountingTo( 220 );

    
	if(g_theAgeDB) {
		if (!g_theAgeDB->Parse(C3DIR_GAMEDATA, g_agedb_filename)) {
			ExitGame();
			return FALSE;
		}
	}

	g_theProgressWindow->StartCountingTo( 230 );

	
	if ( &archive )
		g_theThroneDB = new ThroneDB( archive );
	else {
		g_theThroneDB = new ThroneDB();
		g_theThroneDB->Init(g_thronedb_filename );
	}
	Assert( g_theThroneDB );

	g_theProgressWindow->StartCountingTo( 240 );

	
	if (&archive)
		g_theConceptDB = new ConceptDB(archive) ;
	else {
		g_theConceptDB = new ConceptDB();
		g_theConceptDB->Init(g_conceptdb_filename);
	}
	Assert(g_theConceptDB);

	g_theProgressWindow->StartCountingTo( 250 );

	
	if(g_theTerrainImprovementDB) {
		if(!g_theTerrainImprovementDB->Parse(C3DIR_GAMEDATA, g_tileimprovementdb_filename )) {
			ExitGame();
			return FALSE;
		}
	}
	Assert( g_theTerrainImprovementDB );

	g_theProgressWindow->StartCountingTo( 260 );

	if(!g_theGovernmentDB->Parse(C3DIR_GAMEDATA, g_government_filename))
		return FALSE;
    Assert(g_theGovernmentDB); 

	g_theProgressWindow->StartCountingTo( 270 );

	if(g_theResourceDB) {
		if(!g_theResourceDB->Parse(C3DIR_GAMEDATA, g_goods_filename)) {
			ExitGame();
			return FALSE;
		}
	}

	g_theProgressWindow->StartCountingTo( 280 );

	if(g_theTerrainDB) {
		if(!g_theTerrainDB->Parse(C3DIR_GAMEDATA, g_terrain_filename)) {
            ExitGame(); 
			return FALSE;
		}
	}

	g_theProgressWindow->StartCountingTo( 290 );

	if(g_theBuildingDB) {
		if(!g_theBuildingDB->Parse(C3DIR_GAMEDATA, g_improve_filename)) {
            return FALSE; 
        }
    }
      
	g_theProgressWindow->StartCountingTo( 300 );

	g_theProgressWindow->StartCountingTo( 310 );

    if (&archive)
        g_thePollutionDB = new PollutionDatabase(archive) ;
    else
		{
        g_thePollutionDB = new PollutionDatabase() ;
        if (!g_thePollutionDB->Initialise(g_pollution_filename, C3DIR_GAMEDATA))
			{
			ExitGame();
            return (FALSE) ;
			}
    
		}

    Assert(g_thePollutionDB) ;

	g_theProgressWindow->StartCountingTo( 320 );

    if (&archive)
        g_theGWDB = new GlobalWarmingDatabase(archive) ;
    else
		{
        g_theGWDB = new GlobalWarmingDatabase() ;
        if (!g_theGWDB->Initialise(g_global_warming_filename, C3DIR_GAMEDATA))
			{
			ExitGame();
            return (FALSE) ;
			}
    
		}

    Assert(g_theGWDB) ;
    
	g_theProgressWindow->StartCountingTo( 330 );

    if (&archive)
        g_theUVDB = new OzoneDatabase(archive) ;
    else
		{
        g_theUVDB = new OzoneDatabase() ;
        if (!g_theUVDB->Initialise(g_ozone_filename, C3DIR_GAMEDATA))
			{ 
            ExitGame(); 
            return (FALSE) ;
			}
    
		}

    Assert(g_theUVDB) ;

	g_theProgressWindow->StartCountingTo( 340 );

	if (&archive)
		g_theCivilisationDB = new CivilisationDatabase(archive) ;
	else
		{
		g_theCivilisationDB = new CivilisationDatabase() ;
		if (!g_theCivilisationDB->Initialise(g_civilisation_filename, C3DIR_GAMEDATA))
			{
			ExitGame() ;
			return (FALSE) ;
			}

		}

	Assert(g_theCivilisationDB) ;

	
	
	
	

	g_theProgressWindow->StartCountingTo( 350 );

	














	g_theProgressWindow->StartCountingTo( 360 );

	if(g_theWonderDB) {
		if(!g_theWonderDB->Parse(C3DIR_GAMEDATA, g_wonder_filename)) {
			return FALSE;
		}
	}

	g_theProgressWindow->StartCountingTo( 370 );

	if(&archive)
		g_theRiskDB = new RiskDatabase(archive);
	else {
		g_theRiskDB = new RiskDatabase;
		if(!g_theRiskDB->Initialize(g_risk_filename, C3DIR_GAMEDATA)) {
			ExitGame();
			return FALSE;
		}
	}
	Assert(g_theRiskDB);

	g_theProgressWindow->StartCountingTo( 380 );

	if(&archive)
		g_theEndGameDB = new EndGameDatabase(archive);
	else {
		g_theEndGameDB = new EndGameDatabase;
		if(!g_theEndGameDB->Initialize(g_endgame_filename, C3DIR_GAMEDATA)) {
			ExitGame();
			return FALSE;
		}
	}
	Assert(g_theEndGameDB);

	g_theProgressWindow->StartCountingTo( 390 );

	if (&archive)
		g_theMessageIconFileDB = new FilenameDB(archive);
	else {
		g_theMessageIconFileDB = new FilenameDB;
		if (!g_theMessageIconFileDB->Parse(g_messageiconfdb_filename)) {
			ExitGame();
			return FALSE;
		}
	}

	g_theProgressWindow->StartCountingTo( 400 );

	if (&archive)
		g_theGoodsIconDB = new FilenameDB(archive);
	else {
		g_theGoodsIconDB = new FilenameDB;
		if (!g_theGoodsIconDB->Parse(g_goodsicondb_filename)) {
			ExitGame();
			return FALSE;
		}
	}
	
	g_theProgressWindow->StartCountingTo( 410 );

	if(&archive)
		g_exclusions = new Exclusions(archive);
	else
		g_exclusions = new Exclusions();

	
	g_theProgressWindow->StartCountingTo( 420 );

	
	
	
	

	g_theProgressWindow->StartCountingTo( 430 );

	g_theMapDB = new MapDatabase;
	if(!g_theMapDB->Initialize(g_mapdb_filename, C3DIR_GAMEDATA)) {
		ExitGame();
		return FALSE;
	}

	
	if (g_theOrderDB) {
		if (!g_theOrderDB->Parse(C3DIR_GAMEDATA, g_orderdb_filename)) 
			return FALSE;
	}

	if(g_theFeatDB) {
		if(!g_theFeatDB->Parse(C3DIR_GAMEDATA, g_featdb_filename))
			return FALSE;
	}

	if(g_theEndGameObjectDB) {
		if(!g_theEndGameObjectDB->Parse(C3DIR_GAMEDATA, g_endgameobject_filename))
			return FALSE;
	}

	
	
	
	g_theProgressWindow->StartCountingTo( 440 );

	
	if (g_theGoalDB) {
		if (!g_theGoalDB->Parse(C3DIR_AIDATA, g_goal_db_filename)) 
			return FALSE;
	}

	
	if (g_thePersonalityDB) {
		if (!g_thePersonalityDB->Parse(C3DIR_AIDATA, g_personality_db_filename)) 
			return FALSE;
	}

	
	if (g_theUnitBuildListDB) {
		if (!g_theUnitBuildListDB->Parse(C3DIR_AIDATA, g_unit_buildlist_db_filename)) 
			return FALSE;
	}
	
	if (g_theWonderBuildListDB) {
		if (!g_theWonderBuildListDB->Parse(C3DIR_AIDATA, g_wonder_buildlist_db_filename)) 
			return FALSE;
	}
	
	if (g_theBuildingBuildListDB) {
		if (!g_theBuildingBuildListDB->Parse(C3DIR_AIDATA, g_building_buildlist_db_filename)) 
			return FALSE;
	}
	
	if (g_theImprovementListDB) {
		if (!g_theImprovementListDB->Parse(C3DIR_AIDATA, g_improvement_list_db_filename)) 
			return FALSE;
	}
	
	if (g_theStrategyDB) {
		if (!g_theStrategyDB->Parse(C3DIR_AIDATA, g_strategy_db_filename)) 
			return FALSE;
	}

	
	if (g_theBuildListSequenceDB) {
		if (!g_theBuildListSequenceDB->Parse(C3DIR_AIDATA, g_buildlist_sequence_db_filename)) 
			return FALSE;
	}

	
	if (g_theDiplomacyDB) {
		if (!g_theDiplomacyDB->Parse(C3DIR_AIDATA, g_diplomacy_db_filename)) 
			return FALSE;
	}

	if(g_theDiplomacyProposalDB) {
		if(!g_theDiplomacyProposalDB->Parse(C3DIR_AIDATA, g_diplomacy_proposal_filename))
			return FALSE;
	}

	if(g_theDiplomacyThreatDB) {
		if(!g_theDiplomacyThreatDB->Parse(C3DIR_AIDATA, g_diplomacy_threat_filename))
			return FALSE;
	}

	
	if (g_theAdvanceListDB) {
		if (!g_theAdvanceListDB->Parse(C3DIR_AIDATA, g_advance_list_db_filename)) 
			return FALSE;
	}


	if(g_theCityStyleDB) {
		if(!g_theCityStyleDB->Parse(C3DIR_GAMEDATA, g_city_style_db_filename))
			return FALSE;
	}

	if(g_theAgeCityStyleDB) {
		if(!g_theAgeCityStyleDB->Parse(C3DIR_GAMEDATA, g_age_city_style_db_filename))
			return FALSE;
	}

	if(g_theCitySizeDB) {
		
		char *lastdot = strrchr(g_citysize_filename, '.');
		Assert(lastdot);

		if(!lastdot)
			lastdot = g_citysize_filename + strlen(g_citysize_filename);

		sprintf(lastdot, "%d.txt", 0);

		if(!g_theCitySizeDB->Parse(C3DIR_GAMEDATA, g_citysize_filename))
			return FALSE;

		strcpy(lastdot, ".txt");
	}

	if(g_thePopDB) {
		if(!g_thePopDB->Parse(C3DIR_GAMEDATA, g_pop_filename))
			return FALSE;
	}

	
	if(!g_theUnitDB->ResolveReferences()) return FALSE;
	if(!g_theAdvanceDB->ResolveReferences()) return FALSE;
	if(!g_theIconDB->ResolveReferences()) return FALSE;
	if(!g_theSpriteDB->ResolveReferences()) return FALSE;
	if(!g_theSoundDB->ResolveReferences()) return FALSE;
	if(!g_theSpecialEffectDB->ResolveReferences()) return FALSE;
	if(!g_theSpecialAttackInfoDB->ResolveReferences()) return FALSE;
	if(!g_theGovernmentDB->ResolveReferences()) return FALSE;
	if(!g_theAdvanceBranchDB->ResolveReferences()) return FALSE;
	if(!g_theAgeDB->ResolveReferences()) return FALSE;
	if(!g_theTerrainDB->ResolveReferences()) return FALSE; 
	if(!g_theResourceDB->ResolveReferences()) return FALSE; 
	if(!g_theTerrainImprovementDB->ResolveReferences()) return FALSE; 
	if(!g_theOrderDB->ResolveReferences()) return FALSE;
	if(!g_theCitySizeDB->ResolveReferences()) return FALSE;
	if(!g_thePopDB->ResolveReferences()) return FALSE;
	if(!g_theBuildingDB->ResolveReferences()) return FALSE;
	if(!g_theCityStyleDB->ResolveReferences()) return FALSE;
	if(!g_theAgeCityStyleDB->ResolveReferences()) return FALSE;

	if(!g_theGoalDB->ResolveReferences()) return FALSE;
	if(!g_thePersonalityDB->ResolveReferences()) return FALSE;
	if(!g_theUnitBuildListDB->ResolveReferences()) return FALSE;
	if(!g_theWonderBuildListDB->ResolveReferences()) return FALSE;
	if(!g_theBuildingBuildListDB->ResolveReferences()) return FALSE;
	if(!g_theImprovementListDB->ResolveReferences()) return FALSE;
	if(!g_theStrategyDB->ResolveReferences()) return FALSE;
	if(!g_theBuildListSequenceDB->ResolveReferences()) return FALSE;
	if(!g_theDiplomacyDB->ResolveReferences()) return FALSE;
	if(!g_theDiplomacyProposalDB->ResolveReferences()) return FALSE;
	if(!g_theDiplomacyThreatDB->ResolveReferences()) return FALSE;
	if(!g_theAdvanceListDB->ResolveReferences()) return FALSE;

	if(!g_theWonderDB->ResolveReferences()) return FALSE;
	if(!g_theWonderMovieDB->ResolveReferences()) return FALSE;

	if(!g_theFeatDB->ResolveReferences()) return FALSE;
	if(!g_theEndGameObjectDB->ResolveReferences()) return FALSE;

	unitutil_Initialize();
	advanceutil_Initialize();
	diplomacyutil_Initialize();
	terrainutil_Initialize();
	buildingutil_Initialize();

	ProgressWindow::EndProgress( g_theProgressWindow );

#ifdef _DEBUG
	FILE *dipFile = fopen("dipcombo.txt", "w");
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
	m_dbLoaded = TRUE;

	return TRUE;
}








sint32 CivApp::InitializeApp(HINSTANCE hInstance, int iCmdShow)
{
	sint32		success;

	

	
    CoInitialize(NULL);

	
	Splash::Initialize();

	
	CivPaths_InitCivPaths();

	
	g_theProfileDB = new ProfileDB; 
	if (!g_theProfileDB->Init(FALSE)) {
		c3errors_FatalDialog("CivApp", "Unable to init the ProfileDB.");
		return -1; 
	}

	g_logCrashes = g_theProfileDB->GetEnableLogs();
	
	
	c3files_InitializeCD();

	
	g_civPaths->InitCDPath();

	
	GreatLibrary::Initialize_Great_Library_Data();

	
	
	
	
#ifndef _NO_GAME_WATCH
	
	
	
	MBCHAR		*gameWatchDir = g_theProfileDB->GetGameWatchDirectory();
	if (gameWatchDir[0] == '\0') {
		gameWatch.DeliverySystem("gwfile", "\\\\PROJECT_GREEN\\Raid2\\CIV3\\Programming\\GameWatch\\Records");
	} else {
		gameWatch.DeliverySystem("gwfile", gameWatchDir);
	}

	
	
	gameWatch.RecordingSystem("gwciv");
#endif 

	
	display_Initialize(hInstance, iCmdShow);

	


	
	init_keymap();

	
	if (ui_Initialize() != AUI_ERRCODE_OK) {
	}

	
	

	
	SoundManager::Initialize();

	
	if ( sharedsurface_Initialize() != AUI_ERRCODE_OK ) {
		c3errors_FatalDialog( "CivApp", "Unable to init shared surface." );
		return -1;
	}

	
	CursorManager::Initialize();


	

    
    success = InitializeImageMaps();
	if (!success) {
		c3errors_FatalDialog("CivApp", "Unable to Init the image maps project file.");
		return -1;
	}

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		320 );

	




	g_theProgressWindow->StartCountingTo( 10);


	
	

	
	gameinit_InitializeGameFiles();

	if (g_theProfileDB->IsAIOn()) {
		SPLASH_STRING("AI is ON");
	} else {
		SPLASH_STRING("AI is OFF");
	}

	g_theProgressWindow->StartCountingTo( 20 );

	
    g_theConstDB = new ConstDB();
    if (!ConstDB_Parse (g_constdb_filename, C3DIR_GAMEDATA)) {
        ExitGame(); 
		return -1;
	}
    Assert(g_theConstDB); 

	
	g_theProgressWindow->StartCountingTo( 300 );

	
	success = InitializeAppDB((*(CivArchive *)(NULL)));
	if (!success) {
		c3errors_FatalDialog("CivApp", "Unable to Init the Databases.");
		return -1;
	}

	g_theProgressWindow->StartCountingTo( 310 );

    
    success = InitializeGreatLibrary();
	if (!success) {
		c3errors_FatalDialog("CivApp", "Unable to Init the Great Library.");
		return -1;
	}

    
    success = InitializeSoundPF();
	if (!success) {
		c3errors_FatalDialog("CivApp", "Unable to Init the audio project file.");
		return -1;
	}

	g_theProgressWindow->StartCountingTo( 320 );

	

	ProgressWindow::EndProgress( g_theProgressWindow );


	
	InitializeAppUI();

	
	CivScenarios::Initialize();

	
	g_theProfileDB->DefaultSettings();

	
	StartMessageSystem();










	SPLASH_STRING("Initializing Messaging System...");
	AUI_ERRCODE errcode = messagewin_InitializeMessages();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	if (g_theProfileDB->IsUseFingerprinting()) 
		if (!ctpfinger_Check()) {
			
			c3errors_FatalDialog(appstrings_GetString(APPSTR_INITIALIZE), 
									appstrings_GetString(APPSTR_CANTFINDFILE));
		}

	
	if (g_c3ui->TheMouse()) {
		double sensitivity = 0.0;

		
		sensitivity = 0.25 * (1 + g_theProfileDB->GetMouseSpeed());

		g_c3ui->TheMouse()->Sensitivity() = sensitivity;
	}

	m_appLoaded = TRUE;

	return 0;
}






sint32 CivApp::QuickInit(HINSTANCE hInstance, int iCmdShow)
{
	InitializeApp(hInstance, iCmdShow);
	StartGame();

	return 0;
}


extern ColorSet	*g_colorSet;



sint32 CivApp::CleanupAppUI(void)
{
	
	NetShell::Leave( k_NS_FLAGS_DESTROY );

	
	
	greatlibrary_Cleanup();
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	spscreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();
	optionsscreen_Cleanup();
	optionwarningscreen_Cleanup();
	graphicsscreen_Cleanup();
	gameplayoptions_Cleanup();
	soundscreen_Cleanup();

	
	delete g_c3ui->TheMovieManager();
	delete g_c3ui->TheKeyboard();

	
	delete g_c3ui->TheMouse();

	
	
	delete g_c3ui->TheBlitter();

	aui_MemMap *theMap = g_c3ui->TheMemMap();

	delete theMap;

	delete g_c3ui;
	g_c3ui = NULL;

	
	delete g_GreatLibPF;
	delete g_ImageMapPF;
	delete g_SoundPF;
	g_GreatLibPF = NULL;
	g_ImageMapPF = NULL;
	g_SoundPF = NULL;

	
	delete g_colorSet;

	return 0;
}






sint32 CivApp::CleanupAppDB(void)
{
	if (g_theMapDB) {
		delete g_theMapDB;
		g_theMapDB = NULL;
	}

	if (g_exclusions) {
		delete g_exclusions;
		g_exclusions = NULL;
	}

	if (g_theGoodsIconDB) {
		delete g_theGoodsIconDB;
		g_theGoodsIconDB = NULL;
	}

	if (g_theMessageIconFileDB) {
		delete g_theMessageIconFileDB;
		g_theMessageIconFileDB = NULL;
	}

	if (g_theEndGameDB) {
		delete g_theEndGameDB;
		g_theEndGameDB = NULL;
	}

	if (g_theRiskDB) {
		delete g_theRiskDB;
		g_theRiskDB = NULL;
	}

    if (g_theWonderDB) { 
    	delete g_theWonderDB; 
	    g_theWonderDB = NULL; 
    }

	





	if (g_theCivilisationDB) {
		delete g_theCivilisationDB;
		g_theCivilisationDB = NULL;
	}

	if (g_thePollutionDB) {
		delete g_thePollutionDB;
		g_thePollutionDB = NULL;
	}

	if (g_theUVDB) {
		delete g_theUVDB;
		g_theUVDB = NULL;
	}

	if (g_theGWDB) {
		delete g_theGWDB;
		g_theGWDB = NULL;
	}
	
    if (g_theBuildingDB) { 
    	delete g_theBuildingDB; 
	    g_theBuildingDB = NULL; 
    }

	if (g_theTerrainDB) {
		delete g_theTerrainDB;
		g_theTerrainDB = NULL;
	}

	if (g_theResourceDB) {
		delete g_theResourceDB;
		g_theResourceDB = NULL;
	}
    
    if (g_theGovernmentDB) { 
	    delete g_theGovernmentDB; 
    	g_theGovernmentDB = NULL; 
    }

	if (g_theConceptDB) {
		delete g_theConceptDB;
		g_theConceptDB = NULL;
	}

	if ( g_theThroneDB ) {
		delete g_theThroneDB;
		g_theThroneDB = NULL;
	}

	if (g_theAgeDB) {
		delete g_theAgeDB;
		g_theAgeDB = NULL;
	}
	
	if(g_theCityStyleDB) {
		delete g_theCityStyleDB;
		g_theCityStyleDB = NULL;
	}

	if(g_theAgeCityStyleDB) {
		delete g_theAgeCityStyleDB;
		g_theAgeCityStyleDB = NULL;
	}

	if (g_theDifficultyDB) {
		delete g_theDifficultyDB;
		g_theDifficultyDB = NULL;
	}

    if (g_theUnitDB) { 
    	delete g_theUnitDB; 
	    g_theUnitDB = NULL;
    }

    if (g_theAdvanceDB) { 
    	delete g_theAdvanceDB; 
	    g_theAdvanceDB = NULL;
    }

	if(g_theAdvanceBranchDB) {
		delete g_theAdvanceBranchDB;
		g_theAdvanceBranchDB = NULL;
	}

	if (g_theCitySpriteStateDB) {
		delete g_theCitySpriteStateDB;
		g_theCitySpriteStateDB = NULL;
	}
	
	if (g_theGoodsSpriteStateDB) {
		delete g_theGoodsSpriteStateDB;
		g_theGoodsSpriteStateDB = NULL;
	}

	if (g_theSpecialEffectDB) {
		delete g_theSpecialEffectDB;
		g_theSpecialEffectDB = NULL;
	}

	if (g_theSpriteStateDB) {
		delete g_theSpriteStateDB;
		g_theSpriteStateDB = NULL;
	}

	if(g_theSpriteDB) {
		delete g_theSpriteDB;
		g_theSpriteDB = NULL;
	}

	if(g_theSpecialAttackInfoDB) {
		delete g_theSpecialAttackInfoDB;
		g_theSpecialAttackInfoDB = NULL;
	}

	if (g_thePlayListDB) {
		delete g_thePlayListDB;
		g_thePlayListDB = NULL;
	}

	if (g_theVictoryMovieDB) {
		delete g_theVictoryMovieDB;
		g_theVictoryMovieDB = NULL;
	}

	if (g_theWonderMovieDB) {
		delete g_theWonderMovieDB;
		g_theWonderMovieDB = NULL;
	}

	if (g_theIconDB) {
		delete g_theIconDB;
		g_theIconDB = NULL;
	}

	if (g_theSoundDB) {
		delete g_theSoundDB;
		g_theSoundDB = NULL;
	}

	if (g_theStringDB) { 
    	delete g_theStringDB; 
	    g_theStringDB = NULL;
    }

	if(g_theTerrainImprovementDB) {
		delete g_theTerrainImprovementDB;
		g_theTerrainImprovementDB = NULL;
	}

	if(g_theOrderDB) {
		delete g_theOrderDB;
		g_theOrderDB = NULL;
	}

	if(g_theCitySizeDB) {
		delete g_theCitySizeDB;
		g_theCitySizeDB = NULL;
	}

	if(g_thePopDB) {
		delete g_thePopDB;
		g_thePopDB = NULL;
	}

	if(g_theBuildingDB) {
		delete g_theBuildingDB;
		g_theBuildingDB = NULL;
	}

	if(g_theFeatDB) {
		delete g_theFeatDB;
		g_theFeatDB = NULL;
	}

	if(g_theEndGameObjectDB) {
		delete g_theEndGameObjectDB;
		g_theEndGameObjectDB = NULL;
	}

	if(g_theGoalDB) {
		delete g_theGoalDB;
		g_theGoalDB = NULL;
	}

	if(g_thePersonalityDB) {
		delete g_thePersonalityDB;
		g_thePersonalityDB = NULL;
	}

	if(g_theUnitBuildListDB) {
		delete g_theUnitBuildListDB;
		g_theUnitBuildListDB = NULL;
	}

	if(g_theWonderBuildListDB) {
		delete g_theWonderBuildListDB;
		g_theWonderBuildListDB = NULL;
	}
	if(g_theBuildingBuildListDB) {
		delete g_theBuildingBuildListDB;
		g_theBuildingBuildListDB = NULL;
	}
	if(g_theImprovementListDB) {
		delete g_theImprovementListDB;
		g_theImprovementListDB = NULL;
	}

	if(g_theStrategyDB) {
		delete g_theStrategyDB;
		g_theStrategyDB = NULL;
	}

	if(g_theBuildListSequenceDB) {
		delete g_theBuildListSequenceDB;
		g_theBuildListSequenceDB = NULL;
	}
	
	if(g_theDiplomacyDB) {
		delete g_theDiplomacyDB;
		g_theDiplomacyDB = NULL;
	}

	if(g_theAdvanceListDB) {
		delete g_theAdvanceListDB;
		g_theAdvanceListDB = NULL;
	}

	if(g_theDiplomacyProposalDB) {
		delete g_theDiplomacyProposalDB;
		g_theDiplomacyProposalDB = NULL;
	}

	if(g_theDiplomacyThreatDB) {
		delete g_theDiplomacyThreatDB;
		g_theDiplomacyThreatDB = NULL;
	}

	m_dbLoaded = FALSE;

	return 0;
}






sint32 CivApp::CleanupApp(void)
{
	
	GreatLibrary::Shutdown_Great_Library_Data();

	Splash::Cleanup();

	
	messagewin_Cleanup();

	
	if (g_slicEngine) {
		delete g_slicEngine;
		g_slicEngine = NULL;
	}

	if (g_theMessagePool) {
		delete g_theMessagePool;
		g_theMessagePool = NULL;
	}

	
	CivScenarios::Cleanup();

	
	SoundManager::Cleanup();

    if (g_theProfileDB) { 
        delete g_theProfileDB; 
        g_theProfileDB = NULL; 
    }

    if (g_theConstDB) { 
    	delete g_theConstDB; 
	    g_theConstDB = NULL; 
    }

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

	
	CoUninitialize();

	

	display_Cleanup();

	m_appLoaded = FALSE;

	return 0;
}







sint32 CivApp::InitializeGameUI(void)
{
	AUI_ERRCODE		auiErr;

	
	g_colorSet->Import(0);
	

	
	
	

	sint32 errcode;

	
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	spscreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();

	bool initializedProgress = false;
	if(!g_theProgressWindow) {
		ProgressWindow::BeginProgress(
			g_theProgressWindow,
			"InitProgressWindow",
			140 );
		initializedProgress = true;
	}

	
	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );

	g_theProgressWindow->StartCountingTo( 40, s );

	SPLASH_STRING("Creating Main Windows...");
    g_splash_old = GetTickCount(); 


	SPLASH_STRING("Creating Status Window...");
	errcode = c3windows_MakeStatusWindow(TRUE);
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 50 );

	SPLASH_STRING("Creating Game Window...");
	errcode = backgroundWin_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 60 );

	SPLASH_STRING("Creating Tile Help Window...");
	errcode = helptile_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 70 );

	g_theProgressWindow->StartCountingTo( 80 );

	
	SPLASH_STRING("Creating Debug Window...");
	errcode = c3windows_MakeDebugWindow(TRUE);

	g_theProgressWindow->StartCountingTo( 90 );

	AncientWindows_PreInitialize();

	g_theProgressWindow->StartCountingTo( 100 );



	g_theProgressWindow->StartCountingTo( 110 );


	SPLASH_STRING("Creating Radar Window...");
	errcode = radarwindow_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;


	SPLASH_STRING("Creating Info Bar...");
	InfoBar::Initialize();

	SPLASH_STRING("Creating Control Panel Window...");
	errcode = controlpanelwindow_Initialize();

	g_theProgressWindow->StartCountingTo( 120 );

	
	auiErr = g_c3ui->AddWindow( g_background );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;




	auiErr = g_c3ui->AddWindow( g_statusWindow );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;
   
   
   

	
	radarwindow_Display();











	g_statusWindow->Hide();

	
	errcode = AncientWindows_Initialize();

	if(initializedProgress) {
		ProgressWindow::EndProgress( g_theProgressWindow );
	}

	g_modalWindow = 0;


	AttractWindow::Initialize();

	return 0;
}






sint32 CivApp::InitializeGame(CivArchive &archive)
{


















#ifndef _NO_GAME_WATCH
	
	
	SPLASH_STRING("Initializing Game Watch...");
	g_gameWatchID = gameWatch.StartGame();
#endif _NO_GAME_WATCH

#ifdef _DEBUG_MEMORY

#endif

	
	
	
	NewTurnCount::SetStopPlayer(1);

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		260 );

	
	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );

	g_theProgressWindow->StartCountingTo( 10, s );

	
	init_keymap();			

	g_theProgressWindow->StartCountingTo( 20 );

	SPLASH_STRING("Initializing Sprite Engine...");

	
	sprite_Initialize();

	g_theProgressWindow->StartCountingTo( 30 );

	if (m_dbLoaded && g_theProfileDB->IsScenario()) {
		
		CleanupAppDB();

		
		InitializeAppDB((*(CivArchive *)(NULL)));

		
		greatlibrary_Cleanup();
		GreatLibrary::Initialize_Great_Library_Data();
	}
	
	
	InitializeGameUI();

	g_theProgressWindow->StartCountingTo( 140 );

	
	ChatBox::Initialize();

	g_theProgressWindow->StartCountingTo( 150 );

	
	GrabItem::Init();

	g_theProgressWindow->StartCountingTo( 160 );

	if (m_dbLoaded && g_theProfileDB->IsScenario()) {
		if(g_controlPanel)
			g_controlPanel->CreateTileImpBanks();
	}

	g_theProgressWindow->StartCountingTo( 170 );

	
	gameEventManager_Initialize();

	events_Initialize();
    
	g_theProgressWindow->StartCountingTo( 180 );

	
	g_fog_toggle = FALSE;
	g_god = FALSE;

	
    if (!gameinit_Initialize(-1, -1, archive)) { 
       return FALSE;
    }
		
	
	if(g_isScenario && (&archive != NULL &&
	   (g_startInfoType != STARTINFOTYPE_NONE ||
	    g_saveFileVersion < gamefile_CurrentVersion()))) {
		
		for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				g_player[i]->m_messages->Clear();
			}			
		}

		delete g_theMessagePool;
		g_theMessagePool = new MessagePool();

		delete g_slicEngine;
		g_slicEngine = new SlicEngine();
		if(g_slicEngine->Load(g_slic_filename, k_NORMAL_FILE)) {
			g_slicEngine->Link();
		}

		
		if(g_scenarioUsePlayerNumber > 0 && g_player[g_scenarioUsePlayerNumber] &&
		   g_player[g_scenarioUsePlayerNumber]->m_civilisation &&
		   g_theCivilisationDB && g_theProfileDB) {
			Player *p = g_player[g_scenarioUsePlayerNumber];
			StringId id = ((CivilisationRecord *)p->m_civilisation->GetDBRec())->GetLeaderName();
			const MBCHAR *name = g_theStringDB->GetNameStr(id);
			if(name) {
				p->m_civilisation->AccessData()->SetLeaderName(name);
				g_theProfileDB->SetLeaderName((char *)name);
			}
		}
	}

	if(g_isScenario && !g_oldRandSeed) {
		
		
		
		
		
		
		g_rand->Initialize(time(0));
	}

	if(g_isScenario) {
		
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				g_player[p]->m_civilisation->ResetCiv(g_player[p]->m_civilisation->GetCivilisation(),
													  g_player[p]->m_civilisation->GetGender());
			}
		}
	}





#ifdef _DEBUG
	g_gevManager->Dump();
#endif

	g_theProgressWindow->StartCountingTo( 190 );

	
	GraphicsOptions::Initialize();

    
	SPLASH_STRING("Initializing Tile Engine...");
    tile_Initialize(&archive != NULL);


	
	
	
	
	
	if (g_isScenario) {
		g_tiledMap->PostProcessMap();
	}

	g_theProgressWindow->StartCountingTo( 200 );

	
    radar_Initialize();

	g_theProgressWindow->StartCountingTo( 210 );

	
	Splash::Cleanup();

	g_theProgressWindow->StartCountingTo( 220 );

	
	m_gameLoaded = TRUE;

	g_theProgressWindow->StartCountingTo( 230 );









	g_theProgressWindow->StartCountingTo( 240 );

	
	g_director->CatchUp();

	if(!g_network.IsActive() && !g_network.IsNetworkLaunch()) {
		
		

		if(&archive == NULL ||
			(g_startInfoType != STARTINFOTYPE_NONE)) {
			
			g_gevManager->AddEvent(GEV_INSERT_Tail,
				GEV_BeginTurn,
				GEA_Player, g_selected_item->GetCurPlayer(),
				GEA_Int, g_player[g_selected_item->GetCurPlayer()]->m_current_round,
				GEA_End);
		}
	}

    if(!g_network.IsActive()) { 
        if (NULL == &archive ||
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

	
	
	g_director->ReloadAllSprites();

	if(g_turn->IsEmail() && NULL != &archive) {
		g_selected_item->KeyboardSelectFirstUnit();
		if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY &&
		   (g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() > 0)) {
			g_selected_item->SetSelectCity(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0));
			g_director->AddCenterMap(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0).RetPos());
		}
		g_director->AddCenterMap(g_selected_item->GetCurSelectPos());

		g_slicEngine->CheckPendingResearch();
	}

	
	MainControlPanel::UpdateCityList();

	
	g_scenarioUsePlayerNumber = 0;

	g_theProgressWindow->StartCountingTo( 250 );

		




	if(g_selected_item) {
		g_selected_item->Refresh();
		if(g_director)
			g_director->AddCenterMap(g_selected_item->GetCurSelectPos());
	}

	
	g_oldRandSeed = FALSE;

	MainControlPanel::UpdatePlayer(g_selected_item->GetCurPlayer());

	
	
	extern BOOL g_launchIntoCheatMode;
	if (g_launchIntoCheatMode) {
		ScenarioEditor::Display();







	}

	ProgressWindow::EndProgress( g_theProgressWindow );

	return 0;
}





sint32 InitializeSpriteEditorUI(void)
{
	AUI_ERRCODE		auiErr;
	sint32          errcode;
	
	g_colorSet->Import(0);
	
	
	

	
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	spscreen_Cleanup();
	initialplayscreen_Cleanup();
	scenarioscreen_Cleanup();

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		120 );

	
	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );

	g_theProgressWindow->StartCountingTo( 10, s );

	SPLASH_STRING("Creating Main Windows...");
    g_splash_old = GetTickCount(); 

	g_theProgressWindow->StartCountingTo( 20 );

	



	



	g_theProgressWindow->StartCountingTo( 30 );

	SPLASH_STRING("Creating Status Window...");
	errcode = c3windows_MakeStatusWindow(TRUE);
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 50 );

	SPLASH_STRING("Creating Game Window...");
	errcode = backgroundWin_Initialize(false);
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 60 );

	SPLASH_STRING("Creating Tile Help Window...");
	errcode = helptile_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	g_theProgressWindow->StartCountingTo( 70 );

	g_theProgressWindow->StartCountingTo( 80 );

	
	SPLASH_STRING("Creating Debug Window...");
	errcode = c3windows_MakeDebugWindow(TRUE);

	g_theProgressWindow->StartCountingTo( 90 );

	AncientWindows_PreInitialize();

	g_theProgressWindow->StartCountingTo( 100 );



	g_theProgressWindow->StartCountingTo( 110 );

	SPLASH_STRING("Creating Control Panel Window...");
   
	errcode = controlpanelwindow_Initialize();

	g_theProgressWindow->StartCountingTo( 120 );


	SPLASH_STRING("Creating Radar Window...");
	errcode = radarwindow_Initialize();
	Assert(errcode == 0);
	if (errcode != 0) return 7;

	
	auiErr = g_c3ui->AddWindow( g_background );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;







 















	
	

	SPLASH_STRING("Creating Info Bar...");
	InfoBar::Initialize();

	ProgressWindow::EndProgress( g_theProgressWindow );

	g_modalWindow = 0;

	
	AttractWindow::Initialize();

	void 	HideControlPanel();

	HideControlPanel();
	g_statusWindow->Hide();


	
	
	
	errcode = SpriteEditWindow_Initialize();
	auiErr = g_c3ui->AddWindow( g_spriteEditWindow );
	Assert(auiErr == AUI_ERRCODE_OK);
	if ( auiErr != AUI_ERRCODE_OK ) return 11;

	return 0;
}





sint32 CivApp::InitializeSpriteEditor(CivArchive &archive)
{
	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		380 );

	
	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );

	g_theProgressWindow->StartCountingTo( 10, s );

	
	g_fog_toggle = TRUE;

	
	init_keymap();			

	g_theProgressWindow->StartCountingTo( 20 );

	SPLASH_STRING("Initializing Sprite Engine...");

	
	sprite_Initialize();

	g_theProgressWindow->StartCountingTo( 30 );

	
	InitializeSpriteEditorUI();

	g_theProgressWindow->StartCountingTo( 100 );

	
	ChatBox::Initialize();

	g_theProgressWindow->StartCountingTo( 110 );

	
	GrabItem::Init();

	g_theProgressWindow->StartCountingTo( 120 );

	if (m_dbLoaded && g_theProfileDB->IsScenario()) 
	{
		
		CleanupAppDB();

		
		InitializeAppDB((*(CivArchive *)(NULL)));
	}

	g_theProgressWindow->StartCountingTo( 300 );

	
	gameEventManager_Initialize();

	events_Initialize();

	
   
    if (!spriteEditor_Initialize(20,15)) 
		
		
		return FALSE;





	
	if(&archive != NULL &&
	   (g_startInfoType != STARTINFOTYPE_NONE ||
	    g_saveFileVersion < gamefile_CurrentVersion())) {
		
		delete g_slicEngine;
		g_slicEngine = new SlicEngine();
		if(g_slicEngine->Load(g_slic_filename, k_NORMAL_FILE)) {
			g_slicEngine->Link();
		}
	}
	g_slicEngine->RunTrigger(TRIGGER_LIST_GAME_LOADED, ST_END);

#ifdef _DEBUG
	g_gevManager->Dump();
#endif

	g_theProgressWindow->StartCountingTo( 310 );

	


	g_theProgressWindow->StartCountingTo( 320 );

    
	SPLASH_STRING("Initializing AI...");
    roboinit_Initalize(archive); 
	CtpAi::Initialize();
    
	g_theProgressWindow->StartCountingTo( 330 );

    
	SPLASH_STRING("Initializing Tile Engine...");
    tile_Initialize(&archive != NULL);











	g_theProgressWindow->StartCountingTo( 340 );

	
    radar_Initialize();

	g_theProgressWindow->StartCountingTo( 350 );

	
	Splash::Cleanup();

	g_theProgressWindow->StartCountingTo( 360 );

	
	m_gameLoaded = TRUE;

	g_theProgressWindow->StartCountingTo( 370 );









	g_theProgressWindow->StartCountingTo( 380 );

	
	g_director->CatchUp();

	
    g_turn->BeginNewTurn(FALSE);  

	
	extern sint32 g_scenarioUsePlayerNumber;
    if(!g_network.IsActive()) { 
        if (NULL == &archive ||
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

	if(g_turn->IsEmail() && NULL != &archive) {
		g_selected_item->KeyboardSelectFirstUnit();
		if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY &&
		   (g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Num() > 0)) {
			g_selected_item->SetSelectCity(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0));
			g_director->AddCenterMap(g_player[g_selected_item->GetVisiblePlayer()]->m_all_cities->Access(0).RetPos());
		}
		g_director->AddCenterMap(g_selected_item->GetCurSelectPos());

		g_slicEngine->CheckPendingResearch();
	}

	
	g_scenarioUsePlayerNumber = 0;

	ProgressWindow::EndProgress( g_theProgressWindow );

		





	if(g_selected_item) {
		g_selected_item->Refresh();
		if(g_director)
			g_director->AddCenterMap(g_selected_item->GetCurSelectPos());
	}

	
	g_oldRandSeed = FALSE;

	
	
	extern BOOL g_launchIntoCheatMode;
	if (g_launchIntoCheatMode) {
		ScenarioEditor::Display();







	}

	return 0;
}



sint32 CivApp::CleanupGameUI(void)
{

AttractWindow::Cleanup();

	
	GrabItem::Cleanup();

	
	
	

	
	greatlibrary_Cleanup();
	spnewgamescreen_Cleanup();
	spnewgametribescreen_Cleanup();
	spscreen_Cleanup();
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

	
	
	sci_advancescreen_Cleanup();
	infowin_Cleanup();
	ScienceManagementDialog::Cleanup();

	NationalManagementDialog::Cleanup();
	tutorialwin_Cleanup();

	
	km_screen_Cleanup();
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
	loadsavescreen_Cleanup();

	return 0;
}






sint32 CivApp::CleanupGame(bool keepScenInfo)
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

	
	roboinit_Cleanup();

	
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

		
		InitializeAppDB((*(CivArchive *)(NULL)));
	}

#ifdef _DEBUG
#ifdef _MEMORYLOGGING
#ifdef _DEBUG_MEMORY
	
	DebugMemory_LeaksShow(8675209);
#endif 
#endif 
#endif 

#ifndef _NO_GAME_WATCH
	
	
    if (!g_no_exit_action) { 
	    char userName[256];
	    DWORD size = 256;
	    userName[0] = '\0';		
	    GetUserName(userName, &size);

	    
	    char computerName[256];
	    size = 256;
	    computerName[0] = '\0';		
	    GetComputerName(computerName, &size);

	    
	    SYSTEMTIME localTime;
	    memset(&localTime, 0, sizeof(localTime));	
	    GetLocalTime(&localTime);

	    
	    char stamp[1024];
	    sprintf(stamp, "Civilization III CTP - %s on %s at %d/%d/%d %d:%d:%d", userName, computerName,
		    localTime.wMonth, localTime.wDay, localTime.wYear, localTime.wHour,
		    localTime.wMinute, localTime.wSecond);

	    
	    gameWatch.EndGame(g_gameWatchID, stamp);
    }
#endif 

	m_gameLoaded = FALSE;

	
	
	
	extern BOOL g_launchIntoCheatMode;

	g_launchIntoCheatMode = FALSE;

	g_god = FALSE;

	extern sint32 g_isCheatModeOn;

	g_isCheatModeOn = FALSE;


	
	if(g_c3ui && g_c3ui->Primary() && g_c3ui->TheBlitter()) {
		RECT rect = {0, 0, g_c3ui->Primary()->Width(), g_c3ui->Primary()->Height()};
		g_c3ui->TheBlitter()->ColorBlt(g_c3ui->Primary(), &rect, RGB(0,0,0), 0);
	}

	return 0;
}

sint32 CivApp::StartMessageSystem()
{
	

	
	
	if(!m_dbLoaded)
		InitializeAppDB((*(CivArchive *)(NULL)));


	
	g_slicEngine = new SlicEngine();
	if(g_slicEngine->Load(g_slic_filename, k_NORMAL_FILE))
		g_slicEngine->Link();
	Assert(g_slicEngine) ;

	
	g_theMessagePool = new MessagePool();
	Assert(g_theMessagePool);

	return 0;
}

void CivApp::BeginKeyboardScrolling(sint32 key)
{
	m_keyboardScrollingKey = key;
	m_isKeyboardScrolling = TRUE;
}

void CivApp::StopKeyboardScrolling(sint32 key)
{


	m_keyboardScrollingKey = 0;
	m_isKeyboardScrolling = FALSE;
}








static BOOL s_inCallback = FALSE;
void CivApp::ProcessGraphicsCallback(void)
{
	if (s_inCallback) return;

	if (!g_tiledMap) return;
	if (!g_background) return;
	if (!g_director) return;
	if (!g_background) return;
	if (!g_c3ui) return;

	s_inCallback = TRUE;

	if(g_tiledMap)
		g_tiledMap->RestoreMixFromMap(g_background->TheSurface());

	if(g_background)
		g_background->Draw();
				
	if(g_c3ui)
		g_c3ui->Process();

	if(g_director && (!g_network.IsActive() || (g_network.ReadyToStart())))
		g_director->Process();

	s_inCallback = FALSE;
}






sint32 CivApp::ProcessUI(const uint32 target_milliseconds, uint32 &used_milliseconds)
{
    uint32 start_time_ms = GetTickCount(); 
	uint32			curTicks = GetTickCount();
	static uint32	lastTicks = curTicks;

	if (g_c3ui->TheMouse()) {
		if (g_c3ui->TheMouse()->IsSuspended() ) {
	        used_milliseconds = GetTickCount() - start_time_ms; 

			if (g_runInBackground) {
				if (m_gameLoaded) {
					
					
					if (g_director) {
						g_director->GarbageCollectItems();
						g_director->Process();
					}
				}
			}
			return 0;
		}

		BOOL netGameLoading = FALSE;
		if ((g_network.IsNetworkLaunch() || g_network.IsActive()) 
			&& !g_network.ReadyToStart()) {
			netGameLoading = TRUE;
		}


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

				
				lastTicks = curTicks;
			} else {
				
				
				if(g_tiledMap) {
					g_tiledMap->RestoreMixFromMap(g_background->TheSurface());
				}
				if(g_background)
					g_background->Draw();
				
				
				lastTicks = curTicks;
			}
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

    uint32 start_time_ms = GetTickCount();

	if (g_c3ui->TheMouse()) {
		if (g_c3ui->TheMouse()->IsSuspended() && !g_runInBackground) {
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

    uint32 start_time_ms = GetTickCount();

	if (g_c3ui->TheMouse()) {
		if (g_c3ui->TheMouse()->IsSuspended() && !g_runInBackground) {
	        used_milliseconds = GetTickCount() - start_time_ms; 
			return 0;
		}
	}

    
    
    
	
	

    
    return 0; 
} 


sint32 CivApp::ProcessNet(const uint32 target_milliseconds, uint32 &used_milliseconds)
{
    uint32 start_time_ms = GetTickCount();

	if(m_gameLoaded) {
		g_network.Process();
	}

	return 0;
}

sint32 CivApp::ProcessSLIC(void) 
{
	static sint32   lastRanSlicTimers = 0;

	if(!g_slicEngine)
		return 0;

#ifdef _WAS_ABOUT_TEST_WHEN_DAN_GOT_ME_REPRO_STEPS
	sint32 now = time(0);
	static sint32 last = -1;
	if(now > last) {
		if(g_player && g_player[1]){
			SlicObject *so = new SlicObject("foobar");
			so->AddRecipient(1);
			g_slicEngine->Execute(so);
		}
		last = now;
	}
#endif

	g_slicEngine->ProcessUITriggers();

	if(time(0) > lastRanSlicTimers + g_slicEngine->GetTimerGranularity()) {
		g_slicEngine->RunTimerTriggers();
	}

	if(g_slicEngine->WaitingForLoad()) {
		char buf[1024];
		strncpy(buf, g_slicEngine->GetLoadName(), 1023);
		main_RestoreGame(buf);
	}
	return 0;
}


sint32 frame = 0;

extern BOOL g_no_timeslice;
extern BOOL g_use_profile_process;


sint32 CivApp::ProcessProfile(void)
{
    uint32 target_milliseconds;

    if (g_no_timeslice) { 
        target_milliseconds=10000000; 
    } else { 
        target_milliseconds=30;
    }

    uint32 used_milliseconds;

	ProcessNet(target_milliseconds, used_milliseconds);

	ProcessUI(target_milliseconds, used_milliseconds);

	
	
	ProcessSLIC();

    ProcessRobot(target_milliseconds, used_milliseconds);

	if (g_soundManager)
		g_soundManager->Process(target_milliseconds, used_milliseconds);

frame++;

	exit(0);

	return 0;
}

sint32 CivApp::Process(void)
{
	
#ifdef _DEBUG
	if(g_tempLeakCheck) {
		_CrtMemState new_state; 
		_CrtMemCheckpoint(&new_state);

		g_allocatedAtStart = new_state.lSizes[1];
	}
#endif

	
	
	if(g_netConsole) {
		static uint32 last_tick = 0;
		if(GetTickCount() > last_tick + 250) {
			g_netConsole->Idle();
			last_tick = GetTickCount();
		}
	}

	
	
	if (m_inBackground) {
		
		Sleep(50);
		return 0;
	}

	if (g_use_profile_process) {
		ProcessProfile();
		return 0;
	}

    uint32 target_milliseconds;

    if (g_no_timeslice) { 
        target_milliseconds=10000000; 
    } else { 
        target_milliseconds=30;
    }

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

	
	
	if (m_gameLoaded) {
		if (g_selected_item) {
			if (g_player[g_selected_item->GetCurPlayer()]) {
				if (g_player[g_selected_item->GetCurPlayer()]->m_playerType 
						!= PLAYER_TYPE_ROBOT) {
					extern SaveInfo *g_savedGameRequest;

					if (g_savedGameRequest) {
						
						
						if (g_director)
							g_director->CatchUp();

						
						
						
						
						extern BOOL g_launchIntoCheatMode;

						if (g_launchIntoCheatMode)
							g_theGameSettings->SetKeepScore(TRUE);

						GameFile::SaveGame(g_savedGameRequest->pathName, g_savedGameRequest);
					
						
						
						
						
						if (g_launchIntoCheatMode)
							g_theGameSettings->SetKeepScore(FALSE);
	
						delete g_savedGameRequest;
						g_savedGameRequest = NULL;
					}
				}
			}
		}
	}

frame++;
	return 0;
}


sint32 CivApp::StartGame(void)
{
	InitializeGame((*(CivArchive *)(NULL)));

	return 0;
}


sint32 CivApp::StartSpriteEditor(void)
{
	InitializeSpriteEditor((*(CivArchive *)(NULL)));

	return 0;
}







sint32 CivApp::EndGame(void)
{
	if (m_gameLoaded) {
		CleanupGame(false); 
		StartMessageSystem();
	}
	AUI_ERRCODE errcode;

	extern bool g_e3Demo;
	if(g_e3Demo)
		ExitGame();

	errcode = initialplayscreen_Initialize();
	Assert(errcode == AUI_ERRCODE_OK);

	initialplayscreen_displayMyWindow();

	return 0;
}


sint32 CivApp::LoadSavedGame(MBCHAR *name)
{

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		1000 );

	
	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );

	g_theProgressWindow->StartCountingTo( 1000, s );

	
	
    char filepath[_MAX_PATH]={0};
    FILE *fin=NULL; 
	strcpy(filepath, name);

    fin = fopen(filepath, "r"); 
    if (fin == NULL) { 
        c3errors_ErrorDialog("Load save game", "Could not open %s", name);
        return 0; 
    } 
    fclose (fin); 

	if (m_gameLoaded) {
		CleanupGame(true); 

		
		

		
		CleanupAppDB();

		
		InitializeAppDB((*(CivArchive *)(NULL)));

		
		greatlibrary_Cleanup();
		GreatLibrary::Initialize_Great_Library_Data();
		

		StartMessageSystem();
	}	

	GameFile::RestoreGame(name);

	ProgressWindow::EndProgress( g_theProgressWindow );

	g_tiledMap->InvalidateMap();

	g_selected_item->NextUnmovedUnit(TRUE, TRUE);

	return 0;
}


sint32 CivApp::LoadSavedGameMap(MBCHAR *name)
{

	
	
    char filepath[_MAX_PATH]={0};
    FILE *fin=NULL; 
	strcpy(filepath, name);

    fin = fopen(filepath, "r"); 
    if (fin == NULL) { 
        c3errors_ErrorDialog("Load save game map", "Could not open %s", name);
        return 0; 
    } 
    fclose (fin); 
	
	GameMapFile::RestoreGameMap(name);

	g_tiledMap->InvalidateMap();

	g_selected_item->NextUnmovedUnit(TRUE, FALSE);

	return 0;
}



sint32 CivApp::LoadScenarioGame(MBCHAR *file)
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

	StartGame();

	return 0;
}


sint32 CivApp::RestartGameSameMap(void)
{
	Assert( g_rand != NULL );
	
	g_oldRandSeed = g_rand ? g_rand->GetSeed() : 0;

	if (m_gameLoaded) {
		CleanupGame(true);
		StartMessageSystem();
	}

	StartGame();

	return 0;
}


sint32 CivApp::QuitToSPShell(void)
{
	if (m_gameLoaded) {
		CleanupGame(false);


#ifdef _DEBUG_MEMORY


#endif
		StartMessageSystem();
	}

	spscreen_displayMyWindow();


	return 0;
}


sint32 CivApp::QuitToLobby(void)
{
	if (m_gameLoaded) {
		CleanupGame(false);
		StartMessageSystem();
	}

	NetShell::Enter( k_NS_FLAGS_RETURN );

	return 0;
}


sint32 CivApp::QuitGame(void)
{
	if (m_gameLoaded)
		CleanupGame(true); 

	if (m_appLoaded)
		CleanupApp();

	return 0;
}



void CivApp::AutoSave(PLAYER_INDEX player, bool isQuickSave)
{
	
	
	
	if ((g_network.IsActive() && !g_network.IsHost()) || g_network.IsNetworkLaunch())
		return;

	MBCHAR			filename[_MAX_PATH];
	MBCHAR			path[_MAX_PATH];
	MBCHAR			fullpath[_MAX_PATH];
	
	MBCHAR			leaderName[k_MAX_NAME_LEN];
	const MBCHAR	*autosaveName;

	C3SAVEDIR		dir;

	
	
	if(!isQuickSave) {
		autosaveName = g_theStringDB->GetNameStr("AUTOSAVE_NAME");
	} else {
		autosaveName = g_theStringDB->GetNameStr("QUICKSAVE_NAME");
	}

	
	strcpy(leaderName, g_theProfileDB->GetLeaderName());
	leaderName[6] = '\0';
	c3files_StripSpaces(leaderName);

	
	sprintf(filename, "%s-%s", autosaveName, leaderName);

	
	if (g_network.IsActive()) {
		dir = C3SAVEDIR_MP;
	} else {
		dir = C3SAVEDIR_GAME;
	}

	
	g_civPaths->GetSavePath(dir, path);

	
	sprintf(fullpath, "%s\\%s", path, leaderName);

	
	if (!c3files_PathIsValid(fullpath)) {
		if (!c3files_CreateDirectory(fullpath)) {
			Assert(FALSE);
			return;
		}
	}

	
	strcat(fullpath, "\\");
	strcat(fullpath, filename);

	GameFile	*file = new GameFile();
	
	
	
	g_isScenario = FALSE;

	file->Save(fullpath, NULL);

	delete file;
}

void CivApp::RestoreAutoSave(PLAYER_INDEX player)
{
	MBCHAR		filename[_MAX_PATH];
	sprintf(filename, "auto%d.sav", player);

	g_c3ui->AddAction(new LoadSaveGameAction(filename));
}



void CivApp::PostStartGameAction(void)
{
	StartGameAction		*startGameAction = new StartGameAction();

	if (startGameAction)
		g_c3ui->AddAction(startGameAction);
}


void CivApp::PostSpriteTestAction(void)
{
	SpriteTestAction		*spriteTestAction = new SpriteTestAction();

	if (spriteTestAction)
		g_c3ui->AddAction(spriteTestAction);
}


void CivApp::PostLoadSaveGameAction(MBCHAR *name)
{
	LoadSaveGameAction		*loadsaveGameAction = new LoadSaveGameAction(name);

	if (loadsaveGameAction)
		g_c3ui->AddAction(loadsaveGameAction);
}

void CivApp::PostLoadQuickSaveAction(PLAYER_INDEX player)
{
	if(g_network.IsActive()) {
		
		return;
	}

	MBCHAR			filename[_MAX_PATH];
	MBCHAR			path[_MAX_PATH];
	MBCHAR			fullpath[_MAX_PATH];
	
	MBCHAR			leaderName[k_MAX_NAME_LEN];
	const MBCHAR	*autosaveName;

	C3SAVEDIR		dir;

	
	
	autosaveName = g_theStringDB->GetNameStr("QUICKSAVE_NAME");

	
	strcpy(leaderName, g_theProfileDB->GetLeaderName());
	leaderName[6] = '\0';
	c3files_StripSpaces(leaderName);

	
	sprintf(filename, "%s-%s", autosaveName, leaderName);

	
	if (g_network.IsActive()) {
		dir = C3SAVEDIR_MP;
	} else {
		dir = C3SAVEDIR_GAME;
	}

	
	g_civPaths->GetSavePath(dir, path);

	
	sprintf(fullpath, "%s\\%s", path, leaderName);

	
	if (!c3files_PathIsValid(fullpath)) {
		if (!c3files_CreateDirectory(fullpath)) {
			Assert(FALSE);
			return;
		}
	}

	
	strcat(fullpath, "\\");
	strcat(fullpath, filename);

	FILE *f = fopen(fullpath, "r");
	if(!f)
		return;
	fclose(f);

	PostLoadSaveGameAction(fullpath);
}


void CivApp::PostLoadSaveGameMapAction(MBCHAR *name)
{
	LoadSaveGameMapAction		*loadsaveGameMapAction = new LoadSaveGameMapAction(name);

	if (loadsaveGameMapAction)
		g_c3ui->AddAction(loadsaveGameMapAction);
}

void CivApp::PostRestartGameAction(void)
{
	RestartGameAction		*restartGameAction = new RestartGameAction();

	if (restartGameAction)
		g_c3ui->AddAction(restartGameAction);
}

void CivApp::PostRestartGameSameMapAction(void)
{
	
	
	

	Player *p = g_player[g_selected_item->GetVisiblePlayer()];

	if(p && g_theProfileDB) {
		g_theProfileDB->SetLeaderName(p->GetLeaderName());
		g_theProfileDB->SetCivIndex(p->m_civilisation->GetCivilisation());
	}

	RestartGameSameMapAction *restartGameSameMapAction =
		new RestartGameSameMapAction();

	if (restartGameSameMapAction)
		g_c3ui->AddAction(restartGameSameMapAction);
}

void CivApp::PostQuitToSPShellAction(void)
{
	QuitToSPShellAction *quitToSPShellAction =
		new QuitToSPShellAction();

	if (quitToSPShellAction)
		g_c3ui->AddAction(quitToSPShellAction);
}

void CivApp::PostQuitToLobbyAction(void)
{
	QuitToLobbyAction *quitToLobbyAction =
		new QuitToLobbyAction();

	if (quitToLobbyAction)
		g_c3ui->AddAction(quitToLobbyAction);
}

void CivApp::PostEndGameAction(void)
{
	EndGameAction *endGameAction = new EndGameAction();
	if (endGameAction)
		g_c3ui->AddAction(endGameAction);
}

void CivApp::PostLoadScenarioGameAction(MBCHAR *filename)
{
	LoadScenarioGameAction *loadScenarioGameAction = new LoadScenarioGameAction(filename);
	if (loadScenarioGameAction)
		g_c3ui->AddAction(loadScenarioGameAction);
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
void LoadSaveGameMapAction::Execute(aui_Control *control, uint32 action, uint32 data )
{
	g_civApp->LoadSavedGameMap(m_filename);
}
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




int InitializeGreatLibrary()
{
    MBCHAR path[_MAX_PATH];
    int i=0;

    g_GreatLibPF = new ProjectFile();

    while(g_civPaths->FindPath(C3DIR_GL, i++, path)) {
        if (path[0]) {
            g_GreatLibPF->addPath(path);
            strcat(path, "\\gl.zfs");
            g_GreatLibPF->addPath(path);
        }
    }

    return(TRUE);
}

int InitializeSoundPF()
{
    MBCHAR path[_MAX_PATH];
    int i=0;

    g_SoundPF = new ProjectFile();

    while(g_civPaths->FindPath(C3DIR_SOUNDS, i++, path)) {
        if (path[0]) {
            g_SoundPF->addPath(path);
            strcat(path, "\\sound.zfs");
            g_SoundPF->addPath(path);
        }
    }

    return(TRUE);
}


int InitializeImageMaps()
{
    MBCHAR path[_MAX_PATH];
    int i=0;
    char *patname;
    char *picname;

	if (g_c3ui->Primary()->PixelFormat() == AUI_SURFACE_PIXELFORMAT_555) {
        patname = "\\pat555.zfs";
        picname = "\\pic555.zfs";
    } else {
        patname = "\\pat565.zfs";
        picname = "\\pic565.zfs";
    }

    g_ImageMapPF = new ProjectFile();

    while(g_civPaths->FindPath(C3DIR_PATTERNS, i++, path)) {
        if (path[0]) {
            strcat(path, patname);
            g_ImageMapPF->addPath(path, TRUE);
        }
    }

	i=0;
    while(g_civPaths->FindPath(C3DIR_PICTURES, i++, path)) {
        if (path[0]) {
            strcat(path, picname);
            g_ImageMapPF->addPath(path, TRUE);
        }
    }

    return(TRUE);
}
