//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : New game (SP, MP, scenario) initialisation
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
// USE_TEST_MP_AS_SP
// - When defined, will convert saved MP games to SP games for testing.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added player selection for new single player game and scenarios of
//   all types, by Martin Gühmann.
// - Fixed scenarios that allow players other than player 1 to be played,
//   by Martin Gühmann
// - Added multiplayer to single player game conversion for testing.
// - Prevent assigning the same civilisation index twice, while keeping the 
//   human player selection.
// - TradePool is fixed on reload if the number of goods in the 
//   savegame differs from the number of goods in the database. 
//   - June 4th 2005 Martin Gühmann
// - Allowed for nPlayers to be 2 or 3 - JJB 2005/06/28
// - Removed auto-tutorial on low difficulty - JJB 2005/06/28
//
//----------------------------------------------------------------------------

#include "c3.h" 
#include "c3debug.h"
#include <ios>
#include <iostream>

#include "aui.h"



#include "DB.h"
#include "StrDB.h"
#include "AdvanceRecord.h"

#include "TerrainRecord.h"
#include "gwdb.h"
#include "UVDB.h"

#include "BuildingRecord.h"
#include "DiffDB.h"

#include "moviedb.h"


#include "ConstDB.h"

#include "XY_Coordinates.h"
#include "A_Star_Heuristic_Cost.h"
#include "World.h"
#include "player.h"

#include "DataCheck.h"

#include "RandGen.h"
#include "SpriteStateDB.h"

#include "TradePool.h"
#include "gameinit.h"
#include "SelItem.h"
#include "TradeOfferPool.h"

#include "newturncount.h"
#include "TurnCnt.h"

#include "QuadTree.h"

#include "TerrImprovePool.h"
#include "installationpool.h"
#include "installationtree.h"

#include "WonderRecord.h"
#include "CivilisationDB.h"
#include "filenamedb.h"

#include "pollution.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "DiplomaticRequestPool.h"
#include "CivilisationPool.h"
#include "MessagePool.h"
#include "AgreementPool.h"

#include "civ3_main.h"



#include "profileDB.h"
#include "c3errors.h"

#include "AICause.h"
#include "Advances.h"

#include "SlicEngine.h"

#include "AgeRecord.h"
#include "thronedb.h"
#include "conceptdb.h"

#include "splash.h"
#include "TopTen.h"
#include "SoundRecord.h"

#include "RiskDB.h"

#include "PlayListDB.h"

#include "Wormhole.h"
#include "WonderTracker.h"
#include "AchievementTracker.h"
#include "Exclusions.h"

#include "network.h"
#include "EndGameDB.h"
#include "PollutionDB.h"

#include "debugmemory.h"
#include "TradeBids.h"

#include "Order.h"
#include "UnseenCell.h"
#include "pool.h"

#include "c3slider.h"
#include "custommapscreen.h"

#include "GameSettings.h"
#include "pointerlist.h"

#include "controlpanelwindow.h"

#include "MaterialPool.h"

#include "gamefile.h"
#include "Cell.h"

#include "director.h"
extern Director *g_director;

#include "IconRecord.h"



#include "infowin.h"

#include "MoveFlags.h"

#include "wonderutil.h"


#include "ctpai.h"
#include "RoboInit.h"

#include "FeatTracker.h"


#include "EventTracker.h"

#include "CriticalMessagesPrefs.h"

#include "UnitData.h"
#include "UnitActor.h"
#include "SpriteState.h"
#include "SpriteRecord.h"
#include "ResourceRecord.h"


StringDB                    *g_theStringDB=NULL; 
ConceptDB                   *g_theConceptDB = NULL;
CivilisationDatabase        *g_theCivilisationDB=NULL;
PollutionDatabase           *g_thePollutionDB=NULL ;
GlobalWarmingDatabase       *g_theGWDB=NULL ;
OzoneDatabase               *g_theUVDB=NULL ;
Database <GovernmentRecord> *g_theGovernmentDB=NULL; 
ConstDB                     *g_theConstDB=NULL; 
ThroneDB                    *g_theThroneDB = NULL;
DifficultyDB                *g_theDifficultyDB = NULL; 

PlayListDB                  *g_thePlayListDB = NULL;

World                       *g_theWorld=NULL; 
UnitPool                    *g_theUnitPool=NULL; 
ArmyPool                    *g_theArmyPool=NULL;
Player                      **g_player=NULL;
PointerList<Player>         *g_deadPlayer = NULL;
RandomGenerator             *g_rand=NULL; 
TradePool                   *g_theTradePool = NULL;
TradeOfferPool              *g_theTradeOfferPool = NULL;
QuadTree<Unit>              *g_theUnitTree = NULL;
Pollution                   *g_thePollution=NULL ;
DiplomaticRequestPool       *g_theDiplomaticRequestPool=NULL ;
MessagePool                 *g_theMessagePool=NULL ;
CivilisationPool            *g_theCivilisationPool=NULL ;
AgreementPool               *g_theAgreementPool=NULL ;
TerrainImprovementPool      *g_theTerrainImprovementPool = NULL;
InstallationPool            *g_theInstallationPool = NULL;
InstallationQuadTree        *g_theInstallationTree = NULL;

TopTen                      *g_theTopTen = NULL ; 
TurnCount                   *g_turn = NULL; 
ProfileDB                   *g_theProfileDB = NULL; 
RiskDatabase                *g_theRiskDB = NULL;
MovieDB                     *g_theWonderMovieDB = NULL;
MovieDB                     *g_theVictoryMovieDB = NULL;
FilenameDB                  *g_theMessageIconFileDB = NULL;
FilenameDB                  *g_theGoodsIconDB = NULL;
Pool<Order>                 *g_theOrderPond = NULL;
Pool<UnseenCell>            *g_theUnseenPond = NULL;

#include "Diplomacy_Log.h"
Diplomacy_Log               *g_theDiplomacyLog=NULL;


EventTracker                *g_eventTracker;

CriticalMessagesPrefs *g_theCriticalMessagesPrefs=NULL;

MapPoint g_player_start_list[k_MAX_PLAYERS];
sint32 g_player_start_score[k_MAX_PLAYERS];

sint32 g_abort_parse  = FALSE;

sint32 g_splash_cur;
sint32 g_splash_old;
MBCHAR g_splash_buf[100];

MBCHAR g_improve_filename[_MAX_PATH];
MBCHAR g_pollution_filename[_MAX_PATH];
MBCHAR g_global_warming_filename[_MAX_PATH];
MBCHAR g_ozone_filename[_MAX_PATH];
MBCHAR g_terrain_filename[_MAX_PATH];
MBCHAR g_installation_filename[_MAX_PATH];
MBCHAR g_government_filename[_MAX_PATH];
MBCHAR g_governmenticondb_filename[_MAX_PATH];
MBCHAR g_wonder_filename[_MAX_PATH];
MBCHAR g_constdb_filename[_MAX_PATH]; 
MBCHAR g_pop_filename[_MAX_PATH];
MBCHAR g_civilisation_filename[_MAX_PATH] ;
MBCHAR g_agedb_filename[_MAX_PATH];
MBCHAR g_thronedb_filename[_MAX_PATH];
MBCHAR g_conceptdb_filename[_MAX_PATH];
MBCHAR g_terrainicondb_filename[_MAX_PATH];
MBCHAR g_advanceicondb_filename[_MAX_PATH];
MBCHAR g_advancedb_filename[_MAX_PATH];
MBCHAR g_concepticondb_filename[_MAX_PATH];
MBCHAR g_tileimprovementdb_filename[_MAX_PATH];
MBCHAR g_tileimprovementicondb_filename[_MAX_PATH];
MBCHAR g_spritestatedb_filename[_MAX_PATH];
MBCHAR g_specialeffectdb_filename[_MAX_PATH];
MBCHAR g_specialattackinfodb_filename[_MAX_PATH];
MBCHAR g_city_style_db_filename[_MAX_PATH];
MBCHAR g_age_city_style_db_filename[_MAX_PATH];





MBCHAR g_goodsspritestatedb_filename[_MAX_PATH];
MBCHAR g_cityspritestatedb_filename[_MAX_PATH];
MBCHAR g_uniticondb_filename[_MAX_PATH];
MBCHAR g_wondericondb_filename[_MAX_PATH];
MBCHAR g_improveicondb_filename[_MAX_PATH];
MBCHAR g_difficultydb_filename[_MAX_PATH];
MBCHAR g_stringdb_filename[_MAX_PATH];
MBCHAR g_unitdb_filename[_MAX_PATH];
MBCHAR g_sounddb_filename[_MAX_PATH];
MBCHAR g_goods_filename[_MAX_PATH];
MBCHAR g_risk_filename[_MAX_PATH];
MBCHAR g_wondermoviedb_filename[_MAX_PATH];
MBCHAR g_victorymoviedb_filename[_MAX_PATH];
MBCHAR g_endgame_filename[_MAX_PATH];
MBCHAR g_messageiconfdb_filename[_MAX_PATH];
MBCHAR g_goodsicondb_filename[_MAX_PATH];
MBCHAR g_orderdb_filename[_MAX_PATH];
MBCHAR g_mapdb_filename[_MAX_PATH];
MBCHAR g_playlistdb_filename[_MAX_PATH];
MBCHAR g_branchdb_filename[_MAX_PATH];
MBCHAR g_endgameicondb_filename[_MAX_PATH];
MBCHAR g_citysize_filename[_MAX_PATH];
MBCHAR g_featdb_filename[_MAX_PATH];
MBCHAR g_endgameobject_filename[_MAX_PATH];


MBCHAR g_goal_db_filename[_MAX_PATH];
MBCHAR g_personality_db_filename[_MAX_PATH];
MBCHAR g_squad_class_db_filename[_MAX_PATH];
MBCHAR g_unit_buildlist_db_filename[_MAX_PATH];
MBCHAR g_building_buildlist_db_filename[_MAX_PATH];
MBCHAR g_wonder_buildlist_db_filename[_MAX_PATH];
MBCHAR g_buildlist_sequence_db_filename[_MAX_PATH];
MBCHAR g_improvement_list_db_filename[_MAX_PATH];
MBCHAR g_strategy_db_filename[_MAX_PATH];
MBCHAR g_diplomacy_db_filename[_MAX_PATH];
MBCHAR g_advance_list_db_filename[_MAX_PATH];
MBCHAR g_diplomacy_proposal_filename[_MAX_PATH];
MBCHAR g_diplomacy_threat_filename[_MAX_PATH];

extern MBCHAR g_slic_filename[_MAX_PATH];
extern MBCHAR g_tutorial_filename[_MAX_PATH];

extern DataCheck *g_dataCheck;

extern void Astar_Init();
extern void Astar_Cleanup();
CIV_INDEX gameinit_GetCivForSlot(sint32 slot);






extern SpriteStateDB      *g_theSpriteStateDB;
extern SpriteStateDB      *g_theSpecialEffectDB;
extern SpriteStateDB      *g_theGoodsSpriteStateDB;
extern SpriteStateDB      *g_theCitySpriteStateDB;

extern HWND               gHwnd; 
extern void               verifyYwrap();
extern Splash             *g_splash;

extern ControlPanelWindow *g_controlPanel;


extern BOOL               g_aPlayerIsDead;

extern sint32             g_numGoods; // To fix games with altered ressource database
extern sint32             *g_newGoods;


sint32 g_scenarioUsePlayerNumber = 0;


BOOL                      g_setDifficultyUponLaunch = FALSE;
sint32                    g_difficultyToSetUponLaunch = 0;
BOOL                      g_setBarbarianRiskUponLaunch = FALSE;
sint32                    g_barbarianRiskUponLaunch = 0;


#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#include "aui_window.h"
#include "debugwindow.h"

#include "UnitRecord.h"
#include "UnitDynArr.h"

extern DebugWindow *g_debugWindow; 

SelectedItem *g_selected_item; 

extern sint32 g_abort_parse;

extern sint32 g_oldRandSeed;

static sint32 s_networkSettlers[k_MAX_PLAYERS];
extern sint32 g_cheat_age;

BOOL g_startEmailGame = FALSE, g_startHotseatGame = FALSE;
HotseatPlayerSetup g_hsPlayerSetup[k_MAX_PLAYERS];

//----------------------------------------------------------------------------


namespace
{

//----------------------------------------------------------------------------
//
// Name       : CreateBarbarians
//
// Description: Create a barbarian player.
//
// Parameters : diff                : difficulty level of the game
//
// Globals    : g_theProfileDB      : AI available?
//              g_player            : updated
//              s_networkSettlers   : updated
//
// Returns    : -
//
// Remark(s)  : Will create a human player when the AI is unavailble.
//
//----------------------------------------------------------------------------
void CreateBarbarians(sint32 const diff)
{
	if (g_theProfileDB->IsAIOn()) 
	{
		g_player[PLAYER_INDEX_VANDALS]      = 
		    new Player(PLAYER_INDEX_VANDALS, diff, PLAYER_TYPE_ROBOT,
		               CIV_INDEX_VANDALS, GENDER_MALE
		              );
	}
	else 
	{
		g_player[PLAYER_INDEX_VANDALS]      = 
		    new Player(PLAYER_INDEX_VANDALS, diff, PLAYER_TYPE_HUMAN,
		               CIV_INDEX_RANDOM, GENDER_RANDOM
		              );
	}

	s_networkSettlers[PLAYER_INDEX_VANDALS] = 0;
}

//----------------------------------------------------------------------------
//
// Name       : CreateInitialHuman
//
// Description: Create the main human player.
//
// Parameters : diff      		: difficulty level of the game
//              index           : index to use in player list
//              requestedCiv    : selected civilisation
//
//
// Globals    : g_theProfileDB  : player settings
//              g_player        : updated
//              g_selected_item : updated
//              NewTurnCount    : updated
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CreateInitialHuman
(
	sint32 const	diff,
	sint32 const	index,
	CIV_INDEX const	requestedCiv

)
{
	CIV_INDEX const civ = (CIV_INDEX_RANDOM == requestedCiv)
	                      ? g_theProfileDB->GetCivIndex()
	                      : requestedCiv;

	g_player[index]     = new Player(index, diff, PLAYER_TYPE_HUMAN, civ, 
	                                 g_theProfileDB->GetGender()
	                                );

	// Set the selected player so that the game starts with the first turn 
	// and the correct player.
	g_selected_item->SetPlayerOnScreen(index);
	g_selected_item->SetCurPlayer(index);

	// Make sure that the current player is kept by turning it into the stop 
	// player.
	NewTurnCount::SetStopPlayer(index);
}

}; // namespace


//----------------------------------------------------------------------------

sint32 gameinit_PlaceInitalUnits(sint32 nPlayers, MapPoint player_start_list[k_MAX_PLAYERS])
{
	sint32 i, j;
	sint32 settler = -1;

	sint32 n = g_theUnitDB->NumRecords();
	for (i=0; i<n; i++) { 
		if (g_theUnitDB->Get(i)->GetSettleLand()) {
			settler = i;
			break;
		}
	}

	
	if (settler == -1) {
		settler = 0;
	}




	Unit id; 
	DifficultyDBRecord *drec = g_theDifficultyDB->Get(g_theProfileDB->GetDifficulty());
	sint32 humanStart = drec->m_human_start_location;
	if(humanStart > nPlayers - 1)
		humanStart = nPlayers - 1;
	while(player_start_list[humanStart].x < 0 && humanStart >= 0)
		humanStart--;
	Assert(humanStart >= 0);
	if(humanStart < 0)
		return 0;

	for (i=1; i<nPlayers; i++) 
	{
		if (g_player[i]==NULL)
			continue;

		sint32 which = i - 1;
		if(which == humanStart) 
		{
			if(i != 1) 
			{
				which = 1;
			}
		} 
		else 
			if(i == 1) 
			{
				which = humanStart;
			}

		if(player_start_list[which].x < 0)
			break;

		sint32 nUnits = 1;
		if(g_network.IsLaunchHost()) 
		{
			nUnits = s_networkSettlers[i];
			
			if (nUnits < 1 && i != PLAYER_INDEX_VANDALS)
				nUnits = 1;
		} 
		else 
		{
			if((g_startHotseatGame || g_startEmailGame) &&
			   g_hsPlayerSetup[i].isHuman) 
			{
				nUnits = 1;
			} 
			else 
				if (g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT) 
				{
					nUnits = drec->m_ai_start_units;
				}

				if (g_player_start_score[which] < sint32(drec->m_extra_settler_chance)) 

				{
					nUnits++;
				}
		}

		if (nUnits < 1)
			nUnits = 1;

		for(j = 0; j < nUnits; j++) 
		{
			id =  g_player[i]->CreateUnit(settler, player_start_list[which], Unit(0), 
										  FALSE, CAUSE_NEW_ARMY_INITIAL); 
		}

#ifdef _DEBUG
		sint32 age; 
		g_theProfileDB->SetCheatAge(g_cheat_age);
		if (g_theProfileDB->GetCheatAge(age)) {
		for(; j <9; j++) {
			id =  g_player[i]->CreateUnit(settler, player_start_list[which], Unit(0), 
			                              FALSE, CAUSE_NEW_ARMY_INITIAL);
		}
		}
#endif

	}
	return i - 1;
}

void gameinit_SpewUnits(sint32 player, MapPoint &pos)
{
	FILE *uFile = fopen("logs\\unitlist.txt", "r");
	sint32 n = g_theUnitDB->NumRecords();
	sint32 i;
	if(!uFile) {
		for (i=0; i<n; i++) { 
			if (!g_theUnitDB->Get(i)->GetHasPopAndCanBuild() &&
				!g_theUnitDB->Get(i)->GetIsTrader() 
				) { 
				
				do { 
					pos.y++;
					if (g_theWorld->GetYHeight()<= pos.y) { 
						pos.x++; 
						pos.y = 2; 
					} 
					if (g_theWorld->GetXWidth()<=pos.x) { 
						return; 
					}
				}  while(!g_theWorld->CanEnter(pos, g_theUnitDB->Get(i)->GetMovementType()));
				
				Unit id1 = g_player[player]->CreateUnit(i, pos, Unit(0), 
				                                        FALSE, CAUSE_NEW_ARMY_INITIAL); 
				id1.SetIsProfessional(TRUE); 
			}
		}
		pos.x++; 
	} else {
		fscanf(uFile, "%ld\n", &n);

		sint32 *uids = new sint32[n];
		
		for (i=0; i<n; i++) {
			fscanf(uFile, "%ld\n", &uids[i]);
		}
		fclose(uFile);
		
		for (i=0; i<n; i++) {
			sint32 uid = uids[i];
			if (!g_theUnitDB->Get(uid)->GetHasPopAndCanBuild() &&
				!g_theUnitDB->Get(uid)->GetIsTrader() 
				) { 
				
				do { 
					pos.y++;
					if (g_theWorld->GetYHeight()<= pos.y) { 
						pos.x++; 
						pos.y = 2; 
					} 
					if (g_theWorld->GetXWidth()<=pos.x) { 
						return; 
					}
				}  while(!g_theWorld->CanEnter(pos, g_theUnitDB->Get(uid)->GetMovementType()));

				Unit id1 = g_player[player]->CreateUnit(uid, pos, Unit(0), 
				                                        FALSE, CAUSE_NEW_ARMY_INITIAL); 
				id1.SetIsProfessional(TRUE); 
				
			}
		}
		delete[] uids;
	}
	if (uFile)
		fclose(uFile);
}

void gameinit_PlaceInitalUnits()
{
	sint32 j; 
	MapPoint pos; 
		
	for (j=0; j<k_MAX_PLAYERS; j++) { 
		if(!g_player[j]) continue;
		pos.x = j * 2;
		pos.y = 2;
		gameinit_SpewUnits(j, pos);
	}	
}

sint32 gameinit_InitializeGameFiles(void)
{
	int r;
	
	MBCHAR str1[_MAX_PATH];

	MBCHAR dir[_MAX_PATH]; 
	FILE *fin; 
	MBCHAR		*fn = "InitializeGameFiles";

	g_abort_parse = FALSE; 

	fin = c3files_fopen(C3DIR_GAMEDATA, "gamefile.txt", "r");

	dir[0] = 0;

	r=fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog(fn, "Missing strings file"); 
		g_abort_parse = TRUE;
		return FALSE; 
	}
	sprintf (g_stringdb_filename, "%s%s", dir, str1);

	r = fscanf (fin, "%s", str1);
	if (r == EOF) {
		c3errors_ErrorDialog  (fn, "Missing sounds file"); 
		return FALSE; 
	}
	sprintf(g_sounddb_filename, "%s%s", dir, str1);

	r = fscanf (fin, "%s", str1);	  
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing constants file"); 
		return FALSE; 
	}

	sprintf (g_constdb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1);	  
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing Age file"); 
		return FALSE; 
	}
	sprintf (g_agedb_filename, "%s%s", dir, str1); 

	
	r = fscanf( fin, "%s", str1 );
	if ( r == EOF ) {
		c3errors_ErrorDialog( fn, "Missing Throne file" );
		return FALSE;
	}
	sprintf( g_thronedb_filename, "%s%s", dir, str1 );

	
	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing TerrainIconDB file"); 
		return FALSE; 
	}
	sprintf (g_terrainicondb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing terrain file"); 
		return FALSE; 
	}
	sprintf (g_terrain_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing AdvanceIconDB file"); 
		return FALSE; 
	}
	sprintf (g_advanceicondb_filename, "%s%s", dir, str1); 

	SPLASH_STRING("Loading Advance DB...");
	r=fscanf (fin, "%s", str1); 
	if (r == EOF) {
	c3errors_ErrorDialog  (fn, "Missing advances file"); 
		return FALSE; 
	}
	sprintf (g_advancedb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing concepts file"); 
		return FALSE; 
	}
	sprintf (g_conceptdb_filename, "%s%s", dir, str1); 
	
	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing ConceptIconDB file"); 
		return FALSE; 
	}
	sprintf (g_concepticondb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing tile improvements file"); 
		return FALSE; 
	}
	sprintf (g_tileimprovementdb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing TileImprovmentIconDB file"); 
		return FALSE; 
	}
	sprintf (g_tileimprovementicondb_filename, "%s%s", dir, str1); 

	SPLASH_STRING("Loading Sprite DB...");
	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing sprite file"); 
		return FALSE; 
	}
	sprintf (g_spritestatedb_filename, "%s%s", dir, str1); 


	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing Special Effect ID file"); 
		return FALSE; 
	}
	sprintf (g_specialeffectdb_filename, "%s%s", dir, str1); 

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing Special Attack Info DB file");
		return FALSE;
	}
	sprintf(g_specialattackinfodb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if (r == EOF) {
		c3errors_ErrorDialog  (fn, "Missing GoodsSpriteID file"); 
		return FALSE; 
	}
	sprintf(g_goodsspritestatedb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if (r == EOF) {
		c3errors_ErrorDialog  (fn, "Missing GoodsSpriteID file"); 
		return FALSE; 
	}
	sprintf(g_cityspritestatedb_filename, "%s%s", dir, str1);

	
	r = fscanf( fin, "%s", str1 );
	if ( r == EOF ) {
		c3errors_ErrorDialog( fn, "Missing BranchID file" );
		return FALSE;
	}
	sprintf( g_branchdb_filename, "%s%s", dir, str1 );

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing UnitIconDB file"); 
		return FALSE; 
	}
	sprintf (g_uniticondb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing Units file"); 
		return FALSE; 
	}
	
	sprintf (g_unitdb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing wonder file"); 
		return FALSE; 
	}
	sprintf (g_wonder_filename, "%s%s", dir, str1); 
	
	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing WonderIconDB file"); 
		return FALSE; 
	}
	sprintf (g_wondericondb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing WonderMovieDB file"); 
		return FALSE; 
	}
	sprintf (g_wondermoviedb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing VictoryMovieDB file"); 
		return FALSE; 
	}
	sprintf (g_victorymoviedb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing improvement file"); 
		return FALSE; 
	}
	sprintf (g_improve_filename, "%s%s", dir, str1); 
	
	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing UnitIconDB file"); 
		return FALSE; 
	}
	sprintf (g_improveicondb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing pollution file"); 
		return FALSE; 
	}
	sprintf (g_pollution_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing global warming pollution file"); 
		return FALSE; 
	}
	sprintf (g_global_warming_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing ozone pollution file"); 
		return FALSE; 
	}
	sprintf (g_ozone_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing government file"); 
		return FALSE; 
	}
	sprintf (g_government_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing government icondb file"); 
		return FALSE; 
	}
	sprintf (g_governmenticondb_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing population file"); 
		return FALSE; 
	}
	sprintf (g_pop_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing difficulty file"); 
		return FALSE; 
	}
	sprintf (g_difficultydb_filename, "%s%s", dir, str1); 

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing installations file");
		return FALSE;
	}
	sprintf(g_installation_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing civilisations file");
		return FALSE;
	}
	sprintf(g_civilisation_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing slic file");
		return FALSE;
	}
	sprintf(g_slic_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing tutorial file");
		return FALSE;
	}
	sprintf(g_tutorial_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing goods file");
		return FALSE;
	}
	sprintf(g_goods_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing risk file");
		return FALSE;
	}
	sprintf(g_risk_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing end game database");
		return FALSE;
	}
	sprintf(g_endgame_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing end game icon database");
		return FALSE;
	}
	sprintf(g_endgameicondb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if (r==EOF) {
		c3errors_ErrorDialog(fn, "Missing Message Icon Filename database");
		return FALSE;
	}
	sprintf(g_messageiconfdb_filename, "%s%s", dir, str1);
	
	r = fscanf(fin, "%s", str1);
	if (r==EOF) {
		c3errors_ErrorDialog(fn, "Missing Goods Icon Filename database");
		return FALSE;
	}
	sprintf(g_goodsicondb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing Order DB filename");
		return FALSE;
	}
	sprintf(g_orderdb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing map database filename");
		return FALSE;
	}
	sprintf(g_mapdb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing playlist database filename");
		return FALSE;
	}
	sprintf(g_playlistdb_filename, "%s%s", dir, str1);

	
	
	
	
	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing squad classes DB filename"); 
		return FALSE; 
	}
	sprintf (g_squad_class_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing goals DB filename"); 
		return FALSE; 
	}
	sprintf (g_goal_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing wonder build lists DB filename"); 
		return FALSE; 
	}
	sprintf (g_wonder_buildlist_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing building build lists DB filename"); 
		return FALSE; 
	}
	sprintf (g_building_buildlist_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing unit build lists DB filename"); 
		return FALSE; 
	}
	sprintf (g_unit_buildlist_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing improvement lists DB filename"); 
		return FALSE; 
	}
	sprintf (g_improvement_list_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing strategies DB filename"); 
		return FALSE; 
	}
	sprintf (g_strategy_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing build list sequence DB filename"); 
		return FALSE; 
	}
	sprintf (g_buildlist_sequence_db_filename, "%s%s", dir, str1); 

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing diplomacy DB filename"); 
		return FALSE; 
	}
	sprintf (g_diplomacy_db_filename, "%s%s", dir, str1); 
	
	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing city size DB filename");
		return FALSE;
	}
	sprintf(g_citysize_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing diplomacy proposal DB filename");
		return FALSE;
	}
	sprintf(g_diplomacy_proposal_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing diplomacy threat DB filename");
		return FALSE;
	}
	sprintf(g_diplomacy_threat_filename, "%s%s", dir, str1);
	
	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing advance list DB filename");
		return FALSE;
	}
	sprintf(g_advance_list_db_filename, "%s%s", dir, str1);

	r = fscanf (fin, "%s", str1); 
	if (r == EOF) { 
		c3errors_ErrorDialog  (fn, "Missing personality DB filename"); 
		return FALSE; 
	}
	sprintf (g_personality_db_filename, "%s%s", dir, str1); 

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing feat DB filename");
		return FALSE;
	}
	sprintf(g_featdb_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing end game object DB filename");
		return FALSE;
	}
	sprintf(g_endgameobject_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing city style DB filename");
		return FALSE;
	}
	sprintf(g_city_style_db_filename, "%s%s", dir, str1);

	r = fscanf(fin, "%s", str1);
	if(r == EOF) {
		c3errors_ErrorDialog(fn, "Missing age city style DB filename");
		return FALSE;
	}
	sprintf(g_age_city_style_db_filename, "%s%s", dir, str1);

	
	c3files_fclose(fin);

	return TRUE; 
}




sint32 spriteEditor_Initialize(sint32 mWidth, sint32 mHeight)
{
	g_debugWindow->SetDebugMask(k_DBG_AI); 

	uint32 seed; 
	
	g_theProfileDB->SetNPlayers(3); // What's this?
	
	sint32 nPlayers = g_theProfileDB->GetNPlayers();

	
	g_theOrderPond	= new Pool<Order>(INITIAL_CHUNK_LIST_SIZE);
	g_theUnseenPond	= new Pool<UnseenCell>(INITIAL_CHUNK_LIST_SIZE);

	
	g_theProfileDB->SetTutorialAdvice(FALSE);





	
	
	seed = g_oldRandSeed ? g_oldRandSeed : GetTickCount();

	srand(seed);

	g_rand = new RandomGenerator(seed); 






	SPLASH_STRING("Initializing Pathing...");
	Astar_Init();





	g_theGameSettings = new GameSettings();

	SPLASH_STRING("Initializing the Map...");

	BOOL loadEverything = TRUE;
	if( 
	
	
	
		(g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS))	{		
		loadEverything = FALSE;
	}

	custommapscreen_setValues(g_theProfileDB->GetWetDry(),
	                          g_theProfileDB->GetWarmCold(),
	                          g_theProfileDB->GetOceanLand(),
	                          g_theProfileDB->GetIslandContinent(),
	                          g_theProfileDB->GetHomoDiverse(),
	                          g_theProfileDB->GetGoodCount());

	MapPoint	mapSize;
	g_theConstDB->GetMapSizeMapPoint(g_theProfileDB->GetMapSize(), mapSize);

	
	g_theWorld = new World(mapSize,
	                       g_theProfileDB->IsXWrap(),
	                       g_theProfileDB->IsYWrap());

	g_theWorld->CreateTheWorld(g_player_start_list,
	                           g_player_start_score);


	sint32 i;
	for(i = 0; i < nPlayers - 1; i++) 
		if(g_player_start_list[i].x < 0) 
		{
			nPlayers = i + 1;
			break;
		}

	g_theProfileDB->SetNPlayers(nPlayers);

	Assert(g_theWorld);

	g_turn = new TurnCount();

	g_selected_item = new SelectedItem(nPlayers); 




	
	
	

	SPLASH_STRING("Allocating Object Pools...");

	g_theUnitTree = new QuadTree<Unit>(sint16(g_theWorld->GetXWidth()), 
	                                   sint16(g_theWorld->GetYHeight()), 
	                                   g_theWorld->IsYwrap());

	g_theInstallationTree = new InstallationQuadTree(sint16(g_theWorld->GetXWidth()),
	                                                 sint16(g_theWorld->GetYHeight()),
	                                                 g_theWorld->IsYwrap());

	g_theUnitPool = new UnitPool();
	Assert(g_theUnitPool);

	g_theArmyPool = new ArmyPool();
	Assert(g_theArmyPool);

	g_theTradePool = new TradePool();

	g_theTradeOfferPool = new TradeOfferPool();
	Assert(g_theTradeOfferPool);

	g_thePollution = new Pollution() ;
	Assert(g_thePollution);

	g_theTopTen = new TopTen();
	Assert(g_theTopTen);




	SPLASH_STRING("Initializing SLIC Engine...");

	if (g_slicEngine) {
		delete g_slicEngine;
	}

	g_slicEngine = new SlicEngine();
	
	if(g_slicEngine->Load(g_slic_filename, k_NORMAL_FILE)) 
		g_slicEngine->Link();
	else
		return FALSE;

	
	g_theProfileDB->SetTutorialAdvice(FALSE);
	g_theProfileDB->SetThroneRoom(FALSE);
	g_theProfileDB->SetDifficulty(0);
	g_theProfileDB->SetRiskLevel(0);
	g_theProfileDB->SetNonRandomCivs(TRUE);
	
	Assert(g_slicEngine); // Why here?





	SPLASH_STRING("Initializing Object Pools...");

	g_theTerrainImprovementPool = new TerrainImprovementPool();
	Assert(g_theTerrainImprovementPool) ;

	g_theDiplomaticRequestPool = new DiplomaticRequestPool() ;
	Assert(g_theDiplomaticRequestPool) ;

	g_theCivilisationPool = new CivilisationPool() ;
	Assert(g_theCivilisationPool) ;

	g_theAgreementPool = new AgreementPool() ;
	Assert(g_theAgreementPool) ;

	
	
	if (g_theMessagePool) 
	{
		delete g_theMessagePool;
		g_theMessagePool = NULL;
	}

	g_theMessagePool = new MessagePool() ;
	Assert(g_theMessagePool) ;

	
	if (g_theCriticalMessagesPrefs) 
	{
		delete g_theCriticalMessagesPrefs;
		g_theCriticalMessagesPrefs = NULL;
	}

	g_theCriticalMessagesPrefs = new CriticalMessagesPrefs() ;
	Assert(g_theCriticalMessagesPrefs) ;

	g_theInstallationPool = new InstallationPool();
	Assert(g_theInstallationPool) ;

	g_theInstallationPool->RebuildQuadTree();

	g_wormhole = NULL;
	
	g_theWonderTracker = new WonderTracker();

	g_theAchievementTracker = new AchievementTracker();

	g_theTradeBids = new TradeBids;





	SPLASH_STRING("Setting Up Players...");

	g_player = new Player*[k_MAX_PLAYERS];
	g_deadPlayer = new PointerList<Player>;
	for (i=0; i<k_MAX_PLAYERS; i++) 
		g_player[i] = NULL; 

	Assert(g_player);

	sint32 diff = g_theProfileDB->GetDifficulty();

	sint32 numPlayersLoaded = 0;


	CreateBarbarians(diff);

	sint32 netIndex = 0; 
	NSPlayerInfo *nspi = NULL;
	CIV_INDEX civ = g_theProfileDB->GetCivIndex();
	
	// TODO: check if the fixed index 1 is correct here, 
	//       and whether the start/stop player have to be set.
	g_player[1] = new Player(PLAYER_INDEX(1), 
							 diff, 
							 PLAYER_TYPE_HUMAN, 
							 civ, 
							 g_theProfileDB->GetGender());

	s_networkSettlers[1] = 1;
	if ( strlen(g_theProfileDB->GetLeaderName()) > 0)
		g_player[1]->m_civilisation->AccessData()->SetLeaderName(g_theProfileDB->GetLeaderName());

	sint32 firstRobot = -1;

	for (i=2; i<nPlayers; i++)
	{
		if (g_theProfileDB->IsAIOn() && 
			((!g_network.IsNetworkLaunch() || 
			i > g_network.GetNumHumanPlayers()) ||
			(g_network.IsNetworkLaunch() && 
			g_theProfileDB->NoHumanPlayersOnHost() && 
			i==g_network.GetNumHumanPlayers()))) 
		{



			g_player[i] = new Player(PLAYER_INDEX(i), 
									 diff, 
									 PLAYER_TYPE_ROBOT, 
									 CIV_INDEX_RANDOM, 
									 GENDER_RANDOM);
			s_networkSettlers[i] = 1;
		}
		else
		{
			g_player[i] = new Player(PLAYER_INDEX(i), diff, PLAYER_TYPE_HUMAN, CIV_INDEX_RANDOM, GENDER_RANDOM);
			s_networkSettlers[i] = 1;
		}
	}


	for(; i<k_MAX_PLAYERS; i++)
		g_player[i]= NULL;





#ifdef _DEBUG
	if (g_theProfileDB->IsDiplomacyLogOn()) {
		g_theDiplomacyLog = new Diplomacy_Log;
	}
#endif





#ifdef _DEBUG
	verifyYwrap();
#endif






	SPLASH_STRING("Creating AI Interface's...");

	BOOL createRobotInterface = TRUE;

	if(createRobotInterface) 
	{
		if(g_theProfileDB->IsAIOn() || g_network.IsNetworkLaunch() ) 
		{
			
			
			
			
			PLAYER_INDEX ai_players[k_MAX_PLAYERS]; 
		
			
			sint32 next = 0;
		
			for (i=0; i< k_MAX_PLAYERS; i++) 
				if(g_player[i] && g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT) 
					ai_players[next++] = PLAYER_INDEX(i);
		
			
			

			if(!g_theProfileDB->IsAIOn() && g_network.IsNetworkLaunch()) 
				g_theProfileDB->SetAI(TRUE);
		} 
		else 
		{
			for(i = 0; i < k_MAX_PLAYERS; i++) 
				if(g_player[i])
					g_player[i]->m_playerType = PLAYER_TYPE_HUMAN;
		}
	}

	g_theTradeOfferPool->ReRegisterOffers();









	
	if ( g_selected_item && g_player[g_selected_item->GetVisiblePlayer()] ) 
	{
		
		
		
			
		
		g_selected_item->Refresh();
	}

	
	g_theWorld->A_star_heuristic->Update();

	g_theWorld->RecalculateZOC();

	
	
	
	g_player[1]->m_gold->SetLevel(1000000);
	g_player[1]->m_materialPool->AddMaterials(1000000);

	g_scenarioUsePlayerNumber = 0;








	g_turn->SetHotSeat(FALSE);
	g_turn->SetEmail(FALSE);
	
	g_startHotseatGame = FALSE;
	g_startEmailGame = FALSE;

	{
		sint32 p, c, u;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				for(c = 0; c < g_player[p]->m_all_cities->Num(); c++) {
					Unit city = g_player[p]->m_all_cities->Access(c);
					city->GetActor()->HackSetSpriteID(city.CD()->GetDesiredSpriteIndex());
				}
				for(u = 0; u < g_player[p]->m_all_units->Num(); u++) {
					Unit unit = g_player[p]->m_all_units->Access(u);
					sint32 spriteIndex =  unit.GetDBRec()->GetDefaultSprite()->GetValue();
					unit.GetActor()->GetSpriteState()->SetIndex(spriteIndex);
					unit->GetActor()->ChangeImage(unit.GetActor()->GetSpriteState(), unit.GetType(), unit);
				}
			}
		}
	}

	
	if (g_setDifficultyUponLaunch) 
	{
		g_theProfileDB->SetDifficulty(g_difficultyToSetUponLaunch);
		g_setDifficultyUponLaunch = FALSE;
	}

	if (g_setBarbarianRiskUponLaunch) 
	{
		g_theProfileDB->SetRiskLevel(g_barbarianRiskUponLaunch);
		g_setBarbarianRiskUponLaunch = FALSE;
	}


	
	
	infowin_SetMinRoundForGraphs(0);

	return 1;
}

CIV_INDEX gameinit_GetCivForSlot(sint32 slot)
{
	

	if(g_network.IsLaunchHost()) {
		
		if(slot > g_network.GetNumHumanPlayers()) {
			sint32 firstRobot = g_network.GetNumHumanPlayers() + 1;
			NSAIPlayerInfo *nsaipi = g_network.GetNSAIPlayerInfo(slot - firstRobot);
			if(nsaipi) {
				return CIV_INDEX(nsaipi->m_civ);
			}
		} else {
			
			NSPlayerInfo *nspi = g_network.GetNSPlayerInfo(slot - 1);
			if(nspi)
				return CIV_INDEX(nspi->m_civ);
		}
	}

	
	
	switch(g_startInfoType) {
		case STARTINFOTYPE_NONE:
		case STARTINFOTYPE_NOLOCS:	
		case STARTINFOTYPE_POSITIONSFIXED:
			return CIV_INDEX_RANDOM;
		case STARTINFOTYPE_CIVSFIXED:
			
			
			if (slot-1 >= g_theWorld->GetNumStartingPositions()) {
				return CIV_INDEX_RANDOM;
			} else {
				return CIV_INDEX(g_theWorld->GetStartingPointCiv(slot - 1));
			}
		default:
			return CIV_INDEX_RANDOM;
	}
}



sint32 gameinit_Initialize(sint32 mWidth, sint32 mHeight, CivArchive &archive)
{

	g_debugWindow->SetDebugMask(k_DBG_AI);

	uint32 seed;

	// Reduced min players from 4 to 2 - JJB
	if(!g_network.IsNetworkLaunch() && g_theProfileDB->GetNPlayers() < 2) {
		g_theProfileDB->SetNPlayers(2);
	}
	sint32 nPlayers = g_theProfileDB->GetNPlayers();
	Assert(2 <= nPlayers); 


	g_theOrderPond = new Pool<Order>(INITIAL_CHUNK_LIST_SIZE);
	g_theUnseenPond = new Pool<UnseenCell>(INITIAL_CHUNK_LIST_SIZE);

	if(g_network.IsActive() 
	|| g_network.IsNetworkLaunch() 
	|| g_startHotseatGame 
	|| g_startEmailGame
	){
		g_theProfileDB->SetTutorialAdvice(FALSE);
	}
	// Removed the auto-tutorial on low difficulty, since it causes
	// more problems than it solves - JJB

	if (&archive) { 
		g_rand = new RandomGenerator(archive); 
	} else { 
#ifdef _DEBUG
	FILE *fin; 

	fin = fopen ("dbgseed.txt", "r"); 

	if (fin) { 
		fscanf (fin, "%d", &seed); 
	} else { 


		seed = g_oldRandSeed ? g_oldRandSeed : GetTickCount();

		fin = fopen("logs\\oldseed.txt", "w"); 
		fprintf (fin, "%d\n", seed); 
	}

	DPRINTF(k_DBG_FIX, ("** RANDOM SEED %d\n", seed));
	fclose (fin); 
#else

	seed = g_oldRandSeed ? g_oldRandSeed : GetTickCount();
#endif

	srand(seed);

	g_rand = new RandomGenerator(seed); 
	}





	SPLASH_STRING("Initializing Pathing...");
	Astar_Init();





	if(&archive) {
		g_theGameSettings = new GameSettings(archive);
	} else {
		g_theGameSettings = new GameSettings();
	}

	SPLASH_STRING("Initializing the Map...");

	BOOL loadEverything = TRUE;
	if( 
	
	
	
		(g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS)){
		loadEverything = FALSE;
	}

	if (&archive) { 
		g_theWorld = new World(archive) ;
		if(
		

			(g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS)) {
			sint32 x, y;
			for(x = 0; x < g_theWorld->GetXWidth(); x++) {
				for(y = 0; y < g_theWorld->GetYHeight(); y++) {
					g_theWorld->GetCell(x,y)->ClearUnitsNStuff();
				}
			}
		}

		g_theWorld->NumberContinents();
	} else {
		custommapscreen_setValues(g_theProfileDB->GetWetDry(),
		                          g_theProfileDB->GetWarmCold(),
		                          g_theProfileDB->GetOceanLand(),
		                          g_theProfileDB->GetIslandContinent(),
		                          g_theProfileDB->GetHomoDiverse(),
		                          g_theProfileDB->GetGoodCount());

		MapPoint	mapSize;
		g_theConstDB->GetMapSizeMapPoint(g_theProfileDB->GetMapSize(), mapSize);

		
		g_theWorld = new World(mapSize,
		                       g_theProfileDB->IsXWrap(),
		                       g_theProfileDB->IsYWrap());

		g_theWorld->CreateTheWorld(g_player_start_list,
		                           g_player_start_score);

#ifdef _DEBUG
		if (!g_network.IsActive()) { 
			Assert(g_player_start_list[1].x >= 0);
		}
#endif

		nPlayers = g_theProfileDB->GetNPlayers();
		for(sint32 i = 0; i < nPlayers - 1; i++) {
			if(g_player_start_list[i].x < 0) {
				nPlayers = i + 1; 
				break;
			}
		}

		Assert(nPlayers<=32);
		if (32 < nPlayers) { 
			exit(0); 
		} 
		g_theProfileDB->SetNPlayers(nPlayers);

	}

	Assert(g_theWorld);

	if (&archive && loadEverything){ 
		g_turn = new TurnCount(archive);
	} else { 
		g_turn = new TurnCount(); 
		if(g_network.IsActive() || g_network.IsNetworkLaunch()) {
			sint32 startAge = g_network.GetStartingAge();
			if(startAge != 0) {
				g_turn->SkipToRound(0 );
			}
		}
	}

	if (&archive && loadEverything){ 
		g_selected_item = new SelectedItem(archive); 
	}else { 
		g_selected_item = new SelectedItem(nPlayers); 
	}




	
	
	

	SPLASH_STRING("Allocating Object Pools...");

	g_theUnitTree = new QuadTree<Unit>(sint16(g_theWorld->GetXWidth()), 
	                                   sint16(g_theWorld->GetYHeight()), 
	                                   g_theWorld->IsYwrap());

	g_theInstallationTree = new InstallationQuadTree(sint16(g_theWorld->GetXWidth()),
	                                                 sint16(g_theWorld->GetYHeight()),
	                                                 g_theWorld->IsYwrap());

	if (&archive && loadEverything)
		g_theUnitPool = new UnitPool(archive);
	else
		g_theUnitPool = new UnitPool(); 
	Assert(g_theUnitPool);
	
	if(&archive && loadEverything)
		g_theArmyPool = new ArmyPool(archive);
	else
		g_theArmyPool = new ArmyPool();
	Assert(g_theArmyPool);

	if(&archive && loadEverything)
		g_theUnitPool->RebuildQuadTree();

	if(&archive && loadEverything) {
		g_theTradePool = new TradePool(archive);

		if(g_numGoods != g_theResourceDB->NumRecords()){
			sint32 i;
			sint32 resource;
			ROUTE_TYPE routeType;
			for(i = 0; i < g_theTradePool->Num(); ++i){
				g_theTradePool->GetRouteIndex(i)->GetSourceResource(routeType, resource);
				g_theTradePool->GetRouteIndex(i)->SetSourceResource(g_newGoods[resource]);
			}
		}

		g_theTradePool->RecreateActors();
	} 
	else
		g_theTradePool = new TradePool();

	if(&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			g_theTradeOfferPool = new TradeOfferPool(archive);
		} else {
			g_theTradeOfferPool = new TradeOfferPool();
		}
	} else
		g_theTradeOfferPool = new TradeOfferPool();

	Assert(g_theTradeOfferPool);

	if (&archive && loadEverything)
		g_thePollution = new Pollution(archive);
	else
		g_thePollution = new Pollution();

	Assert(g_thePollution);

	if (&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			g_theTopTen = new TopTen(archive) ;
		} else {
			g_theTopTen = new TopTen();
		}
	} else
		g_theTopTen = new TopTen();

	Assert(g_theTopTen);




	SPLASH_STRING("Initializing SLIC Engine...");

	if (g_slicEngine) {
		delete g_slicEngine;
	}

	if(&archive) {
		g_slicEngine = new SlicEngine(archive);
		g_slicEngine->PostSerialize();
	} else {
		g_slicEngine = new SlicEngine();
		if(g_slicEngine->Load(g_slic_filename, k_NORMAL_FILE)) {
			
			
			
			g_slicEngine->Link();
		} else {
			return FALSE;
		}
		if(!g_network.IsActive() && !g_network.IsNetworkLaunch()) {
			
			g_slicEngine->SetTutorialActive(g_theProfileDB->IsTutorialAdvice());
			g_slicEngine->SetTutorialPlayer(g_theProfileDB->GetPlayerIndex());
			if(g_theProfileDB->IsTutorialAdvice()) {
				if(g_slicEngine->Load(g_tutorial_filename, k_TUTORIAL_FILE)) {
					g_slicEngine->Link();
				}
				
				g_theProfileDB->SetThroneRoom(FALSE);
				g_theProfileDB->SetDifficulty(0);
				g_theProfileDB->SetRiskLevel(0);
				g_theProfileDB->SetNonRandomCivs(TRUE);
			}
		} else {
			g_theProfileDB->SetTutorialAdvice(FALSE);
		}
	}
	Assert(g_slicEngine) ;





	SPLASH_STRING("Initializing Object Pools...");

	if(&archive && loadEverything)
		g_theTerrainImprovementPool = new TerrainImprovementPool(archive);
	else
		g_theTerrainImprovementPool = new TerrainImprovementPool();

	Assert(g_theTerrainImprovementPool) ;

	if (&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			g_theDiplomaticRequestPool = new DiplomaticRequestPool(archive) ;
		} else {
			g_theDiplomaticRequestPool = new DiplomaticRequestPool() ;
		}
	} else
		g_theDiplomaticRequestPool = new DiplomaticRequestPool() ;

	Assert(g_theDiplomaticRequestPool) ;

	if (&archive && loadEverything)
		g_theCivilisationPool = new CivilisationPool(archive) ;
	else
		g_theCivilisationPool = new CivilisationPool() ;

	Assert(g_theCivilisationPool) ;

	if (&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			g_theAgreementPool = new AgreementPool(archive) ;
		} else {
			g_theAgreementPool = new AgreementPool;
		}
	} else
		g_theAgreementPool = new AgreementPool() ;

	Assert(g_theAgreementPool) ;

	
	
	if (g_theMessagePool) {
		delete g_theMessagePool;
		g_theMessagePool = NULL;
	}

	if (&archive && loadEverything)
		g_theMessagePool = new MessagePool(archive) ;
	else
		g_theMessagePool = new MessagePool() ;

	Assert(g_theMessagePool) ;

	
	if (g_theCriticalMessagesPrefs) 
	{
		delete g_theCriticalMessagesPrefs;
		g_theCriticalMessagesPrefs = NULL;
	}
	g_theCriticalMessagesPrefs = new CriticalMessagesPrefs() ;
	Assert(g_theCriticalMessagesPrefs) ;

	if(&archive && loadEverything)
		g_theInstallationPool = new InstallationPool(archive);
	else
		g_theInstallationPool = new InstallationPool();

	Assert(g_theInstallationPool) ;

	g_theInstallationPool->RebuildQuadTree();

	
	if(&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			sint32 wormholeExists;
			archive >> wormholeExists;
			if(wormholeExists)
				g_wormhole = new Wormhole(archive);
			else
				g_wormhole = NULL;
		} else {
			g_wormhole = NULL;
		}
	} else {
		g_wormhole = NULL;
	}
	
	if(&archive && loadEverything) {
		g_theWonderTracker = new WonderTracker(archive);
	} else {
		g_theWonderTracker = new WonderTracker();
	}

	if(&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			g_theAchievementTracker = new AchievementTracker(archive);
		} else {
			g_theAchievementTracker = new AchievementTracker();
		}
	} else {
		g_theAchievementTracker = new AchievementTracker();
	}

	if(&archive) {
		if(g_exclusions)
			delete g_exclusions;
		g_exclusions = new Exclusions(archive);
	} else {
		
		
	}

	if(&archive && loadEverything) {
		g_featTracker = new FeatTracker(archive);
	} else {
		g_featTracker = new FeatTracker;
	}

	if(&archive && loadEverything) {
		if(g_saveFileVersion < 55) {
			g_theTradeBids = new TradeBids(archive);
		} else {
			g_theTradeBids = new TradeBids();
		}
	} else {
		g_theTradeBids = new TradeBids;
	}

	
	if(&archive && loadEverything)
	{
		g_eventTracker = new EventTracker(archive);
	}
	else
	{
		g_eventTracker = new EventTracker;
	}






	SPLASH_STRING("Setting Up Players...");

	sint32 i, j;
	g_player = new Player*[k_MAX_PLAYERS];
	g_deadPlayer = new PointerList<Player>;
	for (i=0; i<k_MAX_PLAYERS; i++) {
		g_player[i] = NULL; 
	}
	Assert(g_player);

	sint32 playerAlive;
	sint32 diff = g_theGameSettings->GetDifficulty();

	sint32 numPlayersLoaded = 0;

	
	if (&archive) {
		for (i=0; i<k_MAX_PLAYERS; i++) {
			archive >> playerAlive;
			if(playerAlive) { 
				g_player[i] = new Player(archive);
				numPlayersLoaded++;
			} else { 
				g_player[i] = NULL;
			}
		}

		sint32 num;
		archive >> num;
		for(i = 0; i < num; i++) {
			Player *pl = new Player(archive);
			g_deadPlayer->AddTail(pl);
		}

		
		if (g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS) 
		{
			CreateBarbarians(diff);

			switch (g_startInfoType) 
			{
			default:
			// case STARTINFOTYPE_NONE:
				break;

			case STARTINFOTYPE_CIVS:
				{
					sint32 const	positionCount	= 
						g_theWorld->GetNumStartingPositions();

					Assert(numPlayersLoaded == 0);

					Assert(g_theProfileDB->GetPlayerIndex() <= positionCount);
					if (g_theProfileDB->GetPlayerIndex() > positionCount)
					{
						// Should have been prevented in the game setup screen
						g_theProfileDB->SetPlayerIndex(positionCount);
					}
					
					// Add the human player
					sint32 const	humanIndex	= g_theProfileDB->GetPlayerIndex();
					CIV_INDEX		civ			= gameinit_GetCivForSlot(humanIndex);
					if (civ == CIV_INDEX_RANDOM)
					{
						civ = g_theProfileDB->GetCivIndex();
					}	
					
					sint32 usedPositions[k_MAX_START_POINTS];
					sint32 usedCivs = 0;
					memset(usedPositions, 0, sizeof(usedPositions));
					
					for (j = 0; j < positionCount; ++j) 
					{
						if (g_theWorld->GetStartingPointCiv(j) == civ) 
						{
							usedPositions[j] = 1;
							usedCivs++;
							break;
						}
					}
					Assert(j < positionCount);
					if (j >= positionCount) 
					{
						civ = static_cast<CIV_INDEX>(g_theWorld->GetStartingPointCiv(0));
						usedPositions[0] = 1;
						usedCivs++;
						j = 0;
					}
					
					CreateInitialHuman(diff, humanIndex, civ);
					g_player[humanIndex]->m_starting_index = j;
					
					// Add robots
					Assert(g_useScenarioCivs <= positionCount);
					for (i = 1; (i <= g_useScenarioCivs) && (i < positionCount); ++i) 
					{
						if (i != humanIndex)
						{
							civ				= gameinit_GetCivForSlot(i);
							sint32 whichCiv = (civ == CIV_INDEX_RANDOM) ? positionCount : 0;
						
							for (; whichCiv < positionCount; ++whichCiv) 
							{
								if (usedPositions[whichCiv])
									continue;
								
								if (g_theWorld->GetStartingPointCiv(whichCiv) == civ)
									break;
							}
						
							if (whichCiv >= positionCount) 
							{
								// Random, or position already assigned. 
								// Find an(other) unassigned random position.
								whichCiv = g_rand->Next(positionCount);
								while (usedPositions[whichCiv]) 
								{
									whichCiv++;
									if (whichCiv >= positionCount)
										whichCiv = 0;
								}

								// Prevent identical civ assignments
								sint32 const	locationCiv	= 
									g_theWorld->GetStartingPointCiv(whichCiv);
								civ	= (locationCiv < 0) 
									  ? CIV_INDEX_RANDOM 
									  : static_cast<CIV_INDEX>(locationCiv);
							}

							g_player[i] = new Player(PLAYER_INDEX(i), 
													 diff,
													 PLAYER_TYPE_ROBOT,
													 civ, 
													 GENDER_RANDOM
													);
							g_player[i]->m_starting_index = whichCiv;
							usedPositions[whichCiv] = 1;
							++usedCivs;
						}
					}
				}
				break;

			case STARTINFOTYPE_CIVSFIXED:
			case STARTINFOTYPE_POSITIONSFIXED:
// Added by Martin Gühmann
// No difference between STARTINFOTYPE_CIVSFIXED and STARTINFOTYPE_POSITIONSFIXED
				{
					Assert(numPlayersLoaded == 0);					
					Assert(g_useScenarioCivs <= g_theWorld->GetNumStartingPositions());
					
					Assert(g_theProfileDB->GetPlayerIndex() <= g_useScenarioCivs);
					if(g_theProfileDB->GetPlayerIndex() > g_useScenarioCivs){
						g_theProfileDB->SetPlayerIndex(g_useScenarioCivs);
					}

					// Add the human player first, to prevent civ reassignment.
					sint32 const	humanIndex	= g_theProfileDB->GetPlayerIndex();
					CIV_INDEX		civ			= gameinit_GetCivForSlot(humanIndex);
					CreateInitialHuman(diff, humanIndex, civ);

					for (i = 1; i <= g_useScenarioCivs; ++i) 
					{
						if (i != humanIndex)
						{
							civ	= gameinit_GetCivForSlot(i);
							g_player[i] = new Player(PLAYER_INDEX(i), 
													 diff, 
													 PLAYER_TYPE_ROBOT,
													 civ,
													 GENDER_RANDOM
													);
						}

						g_player[i]->m_starting_index = i;
					}
				}
				break;
			} // switch
		}
	} else {
		//
		//	Normal game code
		CreateBarbarians(diff);

		sint32 netIndex = 0; 
		NSPlayerInfo *nspi = NULL;
		CIV_INDEX civ = g_theProfileDB->GetCivIndex();
		if(g_network.IsLaunchHost()) {
			nspi = g_network.GetNSPlayerInfo(netIndex++);
			if(nspi) {
				civ = (CIV_INDEX)nspi->m_civ;
			}
		}

		sint32 const		humanIndex	= 
			(nspi) ? 1 : g_theProfileDB->GetPlayerIndex();
		CreateInitialHuman(diff, humanIndex, civ);

		if (nspi)
		{
			g_player[humanIndex]->m_gold->SetLevel(nspi->m_civpoints);
			s_networkSettlers[humanIndex] = nspi->m_settlers;
			if (strlen(nspi->m_name) > 0) 
			{
				g_player[humanIndex]->m_civilisation->AccessData()->
					SetLeaderName(nspi->m_name);
			}
		}
		else
		{
			s_networkSettlers[humanIndex] = 1;
			if ((strlen(g_theProfileDB->GetLeaderName()) > 0) &&
				!g_theProfileDB->IsTutorialAdvice()
			   ) 
			{
				g_player[humanIndex]->m_civilisation->AccessData()->
					SetLeaderName(g_theProfileDB->GetLeaderName());
			}
		}

		sint32 firstRobot = -1;

		for (i = 1; i < nPlayers; ++i) 
		{
		  if (i != humanIndex)
		  {
			if (g_theProfileDB->IsAIOn() && 
				((!g_network.IsNetworkLaunch() || i > g_network.GetNumHumanPlayers()) ||
				 (g_network.IsNetworkLaunch() && g_theProfileDB->NoHumanPlayersOnHost() && i==g_network.GetNumHumanPlayers()))) {
				
				

				NSAIPlayerInfo *nsaipi = NULL;
				if(g_network.IsLaunchHost()) {
					if(firstRobot < 0)
						firstRobot = i;

					nsaipi = g_network.GetNSAIPlayerInfo(i - firstRobot);
				}
				if(nsaipi) {
					g_player[i] = new Player(PLAYER_INDEX(i),
											 diff,
											 PLAYER_TYPE_ROBOT,
											 (CIV_INDEX)nsaipi->m_civ,
											 GENDER_RANDOM);
					g_player[i]->m_networkGroup = nsaipi->m_group;
					g_player[i]->m_gold->SetLevel(nsaipi->m_civpoints);
					s_networkSettlers[i] = nsaipi->m_settlers;
				} else {
					g_player[i] = new Player(PLAYER_INDEX(i), 
											 diff, 
											 PLAYER_TYPE_ROBOT, 
											 CIV_INDEX_RANDOM, 
											 GENDER_RANDOM);
					s_networkSettlers[i] = 1;
				}
			} else { 
				if(!g_network.IsLaunchHost()) {
					g_player[i] = new Player(PLAYER_INDEX(i), diff, PLAYER_TYPE_HUMAN, CIV_INDEX_RANDOM, GENDER_RANDOM);
					s_networkSettlers[i] = 1;
				} else {
					NSPlayerInfo *nspi = g_network.GetNSPlayerInfo(netIndex++);

					civ = nspi ? (CIV_INDEX)nspi->m_civ : CIV_INDEX_RANDOM;

					
					
					g_player[i] = new Player(PLAYER_INDEX(i),
											 diff,
											 PLAYER_TYPE_HUMAN,
											 civ,
											 GENDER_RANDOM);
					if(nspi) {
						g_player[i]->m_networkId = nspi->m_id;
						g_player[i]->m_networkGroup = nspi->m_group;
						g_player[i]->m_gold->SetLevel(nspi->m_civpoints);
						s_networkSettlers[i] = nspi->m_settlers;
						if(strlen(nspi->m_name) > 0) {
							g_player[i]->m_civilisation->AccessData()->
								SetLeaderName(nspi->m_name);
						}
					} else {
						s_networkSettlers[i] = 1;
					}
					
				}
			}
		  } // if (i != humanIndex)
		} // for


		for (; i<k_MAX_PLAYERS; i++) {
			g_player[i]= NULL;
		}

		if(g_network.IsLaunchHost() && g_network.TeamsEnabled()) {
			for(i = 1; i < k_MAX_PLAYERS; i++) {
				if(g_player[i]) {
					sint32 j;
					for(j = 1; j < k_MAX_PLAYERS; j++) {
						if(j == i)
							continue;
						if(!g_player[j])
							continue;
						if(g_player[i]->m_networkGroup == g_player[j]->m_networkGroup) {
							g_player[i]->mask_alliance |= (1 << j);
							g_player[i]->m_contactedPlayers |= (1 << j);
							g_player[i]->m_embassies |= (1 << j);
							g_player[i]->m_diplomatic_state[j] = DIPLOMATIC_STATE_ALLIED;
						}
					}
				}
			}
		}
	}





#ifdef _DEBUG
	if (g_theProfileDB->IsDiplomacyLogOn()) {
		g_theDiplomacyLog = new Diplomacy_Log;
	}
#endif





#ifdef _DEBUG
	verifyYwrap();
#endif






	BOOL createRobotInterface = TRUE;
	sint32 haveRobotInterface = FALSE;

	
	SPLASH_STRING("Initializing A-star Pathing...");
	roboinit_Initalize(archive); 

	if (&archive && loadEverything) 
	{
		SPLASH_STRING("Load AI data elements...");
		
		
		
			
			CtpAi::Load(archive);
			createRobotInterface = FALSE;
			if(!g_theProfileDB->IsAIOn()) {
				
				
				for(i = 0; i < k_MAX_PLAYERS; i++) {
					if(g_player[i])
						g_player[i]->m_playerType = PLAYER_TYPE_HUMAN;
				}
			}
		

	}
	else
	{
		SPLASH_STRING("Initialize AI data elements...");
		CtpAi::Initialize();
	}

	
	if(createRobotInterface) {
		if(g_theProfileDB->IsAIOn() || g_network.IsNetworkLaunch() ) {
			
			
			
			
			PLAYER_INDEX ai_players[k_MAX_PLAYERS]; 
		
			
			sint32 next = 0;
		
			for (i=0; i< k_MAX_PLAYERS; i++) { 
				if(g_player[i] && g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
					ai_players[next++] = PLAYER_INDEX(i);
				}
			}
		
			
			






			
			
			

			if(!g_theProfileDB->IsAIOn() && g_network.IsNetworkLaunch()) {
				g_theProfileDB->SetAI(TRUE);
			}
		} else {
			for(i = 0; i < k_MAX_PLAYERS; i++) {
				if(g_player[i])
					g_player[i]->m_playerType = PLAYER_TYPE_HUMAN;
			}
		}
	}

	g_theTradeOfferPool->ReRegisterOffers();







	if (!(&archive))
	{
		sint32 numPlaced;
#ifdef _DEBUG
		FILE *mouseFile = fopen( "__debuginit__", "r" );
		if ( mouseFile )
		{
			fclose( mouseFile );
			
			gameinit_PlaceInitalUnits();
		}
		else
		{
			numPlaced = gameinit_PlaceInitalUnits(g_theProfileDB->GetNPlayers(), g_player_start_list);
		}
#else
    
		numPlaced = gameinit_PlaceInitalUnits(g_theProfileDB->GetNPlayers(), g_player_start_list);
#endif
		if(numPlaced < g_theProfileDB->GetNPlayers() - 1) { 
			for(sint32 n = numPlaced; n < k_MAX_PLAYERS; n++) {
				if(g_player[n]) {
					
					
					delete g_player[n];
					g_player[n] = NULL;
				}
			}
		}
	} else if(

			  (g_isScenario && g_startInfoType != STARTINFOTYPE_NOLOCS)) {

		sint32 landSettler = -1;
		sint32 seaSettler = -1;
		sint32 spaceSettler = -1;
		
		sint32 n = g_theUnitDB->NumRecords();
		for (i=0; i<n; i++) { 
			if (g_theUnitDB->Get(i)->GetSettleLand() && landSettler < 0) {
				
				
				landSettler = i; 
			} else if(g_theUnitDB->Get(i)->GetSettleWater()) {
				seaSettler = i;
			} else if(g_theUnitDB->Get(i)->GetSettleSpace()) {
				spaceSettler = i;
			}
		} 
		
		if (landSettler == -1 && seaSettler == -1 && spaceSettler == -1) { 
			c3errors_FatalDialog("gameinit", "Could not find a land settler"); 
		}

		for(i = 1; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				MapPoint point = g_theWorld->GetStartingPoint(g_player[i]->m_starting_index);
				sint32 settler;
				if(g_theWorld->IsWater(point) || g_theWorld->IsShallowWater(point)) {
					settler = seaSettler;
				} else {
					settler = landSettler;
				}
				g_player[i]->CreateUnit(settler, point, Unit(0),
										FALSE, CAUSE_NEW_ARMY_INITIAL);
			}
		}

		g_turn->CountActivePlayers();

		g_director->AddCopyVision();
	}










	if(&archive) {
		g_theWorld->SetAllMoveCost();

  #if defined(USE_TEST_MP_AS_SP)
		if (g_network.IsActive() || g_network.IsNetworkLaunch() || 
			g_turn->IsEmail()	 || g_turn->IsHotSeat()
		   )
		{
			// No action: keep current setup.
		}
		else 
		{
			// Convert MP game to SP game for testing. 
			for (size_t i = 0; i < k_MAX_PLAYERS; ++i)
			{
				if (g_player[i])
				{
					if (i == g_theProfileDB->GetPlayerIndex()) 
					{
						g_player[i]->SetPlayerType(PLAYER_TYPE_HUMAN);
						g_selected_item->SetPlayerOnScreen(i);
						g_selected_item->SetCurPlayer(i);
						NewTurnCount::SetStopPlayer(i);
					}

					else
					{
						g_player[i]->SetPlayerType(PLAYER_TYPE_ROBOT);
					} 
				}
				else
				{
					if (i == g_theProfileDB->GetPlayerIndex())
					{
						// Selected a non-existing player.
						c3errors_ErrorDialog("MP to SP", "Invalid index selected");
					}
				}
			}
		}
  #endif  // USE_TEST_MP_AS_SP

		g_aPlayerIsDead = FALSE;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			
			if(g_player[i]) {
				if(g_player[i]->GetPlayerType() == PLAYER_TYPE_HUMAN) {
					g_player[i]->RecreateMessageIcons();
				}
				if(g_player[i]->m_isDead) {
					g_aPlayerIsDead = TRUE;
				}
			}

			
			if(g_player[i]) {
				g_player[i]->m_hasGlobalRadar = wonderutil_GetGlobalRadar(g_player[i]->m_builtWonders);
			}
		}
	}

	if (g_selected_item && 
		g_player[g_selected_item->GetVisiblePlayer()] &&
		!g_turn->IsHotSeat()
	   )
	{
		
			
		
		g_selected_item->Refresh();
	}

	
	g_theWorld->A_star_heuristic->Update();

	g_theWorld->RecalculateZOC();

#ifdef _DEBUG
	
	{
		sint32 x,y;
		for(x = 0; x < g_theWorld->GetXWidth(); x++) {
			for(y = 0; y < g_theWorld->GetYHeight(); y++) {
				Cell *cell = g_theWorld->GetCell(x, y);
				sint32 u;
				for(u = 0; u < cell->GetNumUnits(); u++) {
					Assert(cell->AccessUnit(u).IsValid());
				}
			}
		}
	}
#endif

	
	for(i=0; i<k_MAX_PLAYERS; i++)
	{
		int j;
		if(g_player[i])
		{
			for(j=0; j<g_player[i]->m_all_cities->Num(); j++)
			{
				g_player[i]->m_all_cities->Access(j)->GetCityData()->FindGoodDistances();
			}
		}
	}


	
	
	if(&archive && loadEverything) {
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				g_player[i]->ResetVision();


				
				
				
				
				
				if (g_player[i]->m_isDead &&
					g_player[i]->m_all_cities->Num() > 0) {
					g_player[i]->m_isDead = FALSE;
				}
			}
		}
	}
	g_director->AddCopyVision();

	
	if(!g_network.IsActive() && !g_network.IsNetworkLaunch()) {
		if(strcmp(g_theProfileDB->GetLeaderName(), "Leemur") == 0) {
		
			g_player[1]->m_gold->SetLevel(1000000);

			g_player[1]->m_materialPool->AddMaterials(1000000);
		}
	}

	if(g_isScenario && loadEverything && g_scenarioUsePlayerNumber > 0) {
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(!g_player[i])
				continue;
			
			
			
			g_player[i]->m_playerType = PLAYER_TYPE_ROBOT;
		}
		
		g_player[g_scenarioUsePlayerNumber]->m_playerType = PLAYER_TYPE_HUMAN;
		g_selected_item->SetPlayerOnScreen(g_scenarioUsePlayerNumber);

	//Added by Martin Gühmann
		//Set current player the selected player so that the 
		//game starts with the first turn and the correct player.
		g_selected_item->SetCurPlayer(g_scenarioUsePlayerNumber);
		//Make sure that the current player is kept by turning it
		//into the stop player.
		NewTurnCount::SetStopPlayer(g_scenarioUsePlayerNumber);

	} else {
		g_scenarioUsePlayerNumber = 0;
	}








	if(!(&archive) || g_isScenario) {
		if(g_startHotseatGame || g_startEmailGame) {
			
			BOOL foundFirstHuman = FALSE;

			for(i = 1; i < g_theProfileDB->GetNPlayers(); i++) {
				Assert(g_player[i]);
				if(!g_player[i])
					continue;

				
				
				if(g_hsPlayerSetup[i].isHuman && !foundFirstHuman) {
					
					NewTurnCount::SetStopPlayer(g_selected_item->GetCurPlayer());
					g_selected_item->SetPlayerOnScreen(i);
					foundFirstHuman = TRUE;
				}

				if(g_hsPlayerSetup[i].isHuman && g_player[i]->GetPlayerType() == PLAYER_TYPE_ROBOT)
					g_player[i]->m_playerType = PLAYER_TYPE_HUMAN;
				else if(!g_hsPlayerSetup[i].isHuman && g_player[i]->GetPlayerType() != PLAYER_TYPE_ROBOT)
					g_player[i]->m_playerType = PLAYER_TYPE_ROBOT;

				g_player[i]->m_civilisation->ResetCiv((CIV_INDEX)g_hsPlayerSetup[i].civ, g_player[i]->m_civilisation->GetGender());
				if(g_player[i]->GetPlayerType() == PLAYER_TYPE_HUMAN) {
					if(strlen(g_hsPlayerSetup[i].name) > 0)
						g_player[i]->m_civilisation->AccessData()->SetLeaderName(g_hsPlayerSetup[i].name);

					if(i == 1) {
						
						
						g_theProfileDB->SetLeaderName(g_hsPlayerSetup[i].name);
					}
					g_player[i]->m_email = new MBCHAR[strlen(g_hsPlayerSetup[i].email) + 1];
					strcpy(g_player[i]->m_email, g_hsPlayerSetup[i].email);
				}
			}
			if(g_startHotseatGame) {
				g_turn->SetHotSeat(TRUE);
				g_turn->SetEmail(FALSE);
			} else if(g_startEmailGame) {
				g_turn->SetEmail(TRUE);
				g_turn->SetHotSeat(FALSE);
			}
		} else {
			g_turn->SetHotSeat(FALSE);
			g_turn->SetEmail(FALSE);
		}
	} 
	
	g_startHotseatGame = FALSE;
	g_startEmailGame = FALSE;

	
	if (g_setDifficultyUponLaunch) {
		g_theProfileDB->SetDifficulty(g_difficultyToSetUponLaunch);
		g_setDifficultyUponLaunch = FALSE;
	}

	if (g_setBarbarianRiskUponLaunch) {
		g_theProfileDB->SetRiskLevel(g_barbarianRiskUponLaunch);
		g_setBarbarianRiskUponLaunch = FALSE;
	}


	
	
	if (g_isScenario) {
		infowin_SetMinRoundForGraphs(g_turn->GetRound());
	} else {
		infowin_SetMinRoundForGraphs(0);
	}

	// Clean good old -> new good table
	// Created in CityData if the good database was changed in size.
	delete[] g_newGoods;
	g_newGoods = NULL;

	return 1;
}



sint32 gameinit_CleanupMessages(void)
{
	if(!g_player)
		return 0;

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			g_player[i]->m_messages->KillList();
		}
	}
	return 0;
}

sint32 gameinit_Cleanup(void) 
{
#define CHECKDELETE(x)  if(x) { delete x; x = NULL; }
	


	CHECKDELETE(g_theInstallationPool);
	CHECKDELETE(g_theMessagePool);
	CHECKDELETE(g_theCriticalMessagesPrefs);

	if (g_player) {
		sint32 i;
	
		for (i=0; i<k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				delete g_player[i];
			}
		}

		delete [] g_player;
		g_player = NULL;
		if(g_deadPlayer) {
			g_deadPlayer->DeleteAll();
			delete g_deadPlayer;
			g_deadPlayer = NULL;
		}
	}

	CHECKDELETE(g_theAgreementPool);
	CHECKDELETE(g_theCivilisationPool);
	CHECKDELETE(g_theDiplomaticRequestPool);
	CHECKDELETE(g_theTerrainImprovementPool);
	CHECKDELETE(g_slicEngine);
	CHECKDELETE(g_theTopTen);
	CHECKDELETE(g_thePollution);
	CHECKDELETE(g_theTradePool);
	CHECKDELETE(g_theUnitPool);
	CHECKDELETE(g_theInstallationTree);
	CHECKDELETE(g_theUnitTree);
	CHECKDELETE(g_selected_item);
	CHECKDELETE(g_theTradePool); 
	CHECKDELETE(g_theTradeOfferPool); 
	CHECKDELETE(g_turn);
	CHECKDELETE(g_theWorld);
	CHECKDELETE(g_theGameSettings);
	CHECKDELETE(g_theArmyPool);
	CHECKDELETE(g_theWonderTracker);
	CHECKDELETE(g_theAchievementTracker);




	CHECKDELETE(g_theTradeBids);
	CHECKDELETE(g_eventTracker);
	CHECKDELETE(g_wormhole);
	
	CHECKDELETE(g_theOrderPond);
	CHECKDELETE(g_theUnseenPond);

	CHECKDELETE(g_featTracker);



#ifdef _DEBUG
	CHECKDELETE(g_theDiplomacyLog);
#endif
	Astar_Cleanup();

	CHECKDELETE(g_rand);
	return 0;
}

sint32 gameinit_ResetForNetwork()
{
	CHECKDELETE(g_theInstallationPool);
	g_theInstallationPool = new InstallationPool;

	CHECKDELETE(g_theAgreementPool);
	g_theAgreementPool = new AgreementPool;

	CHECKDELETE(g_theCivilisationPool);
	g_theCivilisationPool = new CivilisationPool;

	CHECKDELETE(g_theDiplomaticRequestPool);
	g_theDiplomaticRequestPool = new DiplomaticRequestPool;

	CHECKDELETE(g_theTerrainImprovementPool);
	g_theTerrainImprovementPool = new TerrainImprovementPool;

	CHECKDELETE(g_theTradePool);
	g_theTradePool = new TradePool;

	CHECKDELETE(g_theUnitPool);
	g_theUnitPool = new UnitPool;

	CHECKDELETE(g_theTradePool); 
	g_theTradePool = new TradePool;

	CHECKDELETE(g_theTradeOfferPool); 
	g_theTradeOfferPool = new TradeOfferPool;

	CHECKDELETE(g_theArmyPool);
	g_theArmyPool = new ArmyPool;

	
	CHECKDELETE(g_theOrderPond);
	g_theOrderPond = new Pool<Order>(INITIAL_CHUNK_LIST_SIZE);


	CHECKDELETE(g_theUnseenPond);
	g_theUnseenPond = new Pool<UnseenCell>(INITIAL_CHUNK_LIST_SIZE);

	return 0;
}

#include "c3ui.h"
extern C3UI *g_c3ui;
#include "radarmap.h"
#include "radarwindow.h"
#include "ctp2_Window.h"
#include "tiledmap.h"
#include "background.h"
extern Background *g_background;

void gameinit_ResetMapSize()
{
	g_c3ui->RemoveWindow(g_radarWindow->Id());
	radarwindow_Cleanup();
			
	delete g_tiledMap;
	g_tiledMap = NULL;

	MapPoint mapsize(g_theWorld->GetXWidth(),
					 g_theWorld->GetYHeight());
	g_tiledMap = new TiledMap(mapsize);
	g_tiledMap->LoadTileset();
	
	RECT rect = 
	{
		g_background->X(), 
		g_background->Y(), 
		g_background->X() + g_background->Width(),
		g_background->Y() + g_background->Height() 
	};
	
	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i]) {
			if(g_player[i]->m_vision)
				delete g_player[i]->m_vision;
			
			g_player[i]->m_vision = new Vision(i);
		}
	}
	
	g_tiledMap->Initialize(&rect);
	g_tiledMap->Refresh();
	
	radarwindow_Initialize();
	
	radarwindow_Display();
	
	g_tiledMap->PostProcessMap();
	g_tiledMap->Refresh();
	
	g_theWorld->NumberContinents();
	
	g_background->Draw();
	
	if(g_theUnitTree)
		delete g_theUnitTree;
	
	g_theUnitTree = new QuadTree<Unit>(sint16(g_theWorld->GetXWidth()), 
	                                   sint16(g_theWorld->GetYHeight()), 
	                                   g_theWorld->IsYwrap());

	if(g_theInstallationTree)
		delete g_theInstallationTree;

	g_theInstallationTree = new InstallationQuadTree((sint16)g_theWorld->GetXWidth(),
	                                                 (sint16)g_theWorld->GetYHeight(),
	                                                 g_theWorld->IsYwrap());

	
	
	CHECKDELETE(g_theInstallationPool);
	g_theInstallationPool = new InstallationPool;

	CHECKDELETE(g_theAgreementPool);
	g_theAgreementPool = new AgreementPool;

	CHECKDELETE(g_theDiplomaticRequestPool);
	g_theDiplomaticRequestPool = new DiplomaticRequestPool;

	CHECKDELETE(g_theTerrainImprovementPool);
	g_theTerrainImprovementPool = new TerrainImprovementPool;

	CHECKDELETE(g_theTradePool);
	g_theTradePool = new TradePool;

	CHECKDELETE(g_theUnitPool);
	g_theUnitPool = new UnitPool;

	CHECKDELETE(g_theTradePool); 
	g_theTradePool = new TradePool;

	CHECKDELETE(g_theArmyPool);
	g_theArmyPool = new ArmyPool;

	
	CHECKDELETE(g_theOrderPond);
	g_theOrderPond = new Pool<Order>(INITIAL_CHUNK_LIST_SIZE);


	CHECKDELETE(g_theUnseenPond);
	g_theUnseenPond = new Pool<UnseenCell>(INITIAL_CHUNK_LIST_SIZE);

	CtpAi::Initialize();
}
