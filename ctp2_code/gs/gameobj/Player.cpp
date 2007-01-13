//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Player game object
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
// - SetResearchGoal now clears research goal if the potential goal
//   is already researched and returns now values whether the goal was
//   set, cleared or nothing happened, by Martin Gühmann.
// - Advances leading to tile improvements can now be set as goals, 
//   by Martin Gühmann.
// - Moved the autosave file generation to PlayerEvent.cpp, to prevent losing 
//   the advance that just has had its research completed.
// - #01 Recalculating military support costs after government change
//   (L. Hirth 7/2004)
// - Prevent cities from revolting twice in the same turn. By kaan.
// - Prevent instant messages showing out of turn in hotseat
//   (J Bytheway 2004/09/15)
// - Added extra checks to disable science victory in network games (bug #21)
// - Propagate PW each turn update.
// - Add new advance immediately when subtracting cost, to prevent losing
//   the advance in PBEM.
// - Changed the code which makes the science choice window appear to
//   prevent it appearing inappropriately in PBEM/hotseat games
//   (J Bytheway 2005/01/02)
// - Synchronise the turn count when adding a new civilisation.
// - Prevented memory leak reports (not actual leaks)
// - CultureOnly code added to CanBuildUnit; limits unit construction by 
//   comparing a unit's CultureOnly flag to a player's citystyle 
//   by E April 20th 2005
// - Removed .NET warnings - May 7th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3debug.h"


#include "c3.h"
#include "globals.h"
#include "c3errors.h"
#include "c3math.h"

#include "Army.h"
#include "cellunitlist.h"
#include "player.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "civarchive.h"


#include "SelItem.h"
#include "aui.h"
#include "debugwindow.h"


#include "network.h"
#include "net_action.h"
#include "net_info.h"
#include "net_army.h"
#include "net_research.h"

#include "AdvanceRecord.h"
#include "DataCheck.h"
#include "pollution.h"

#include "CivPaths.h"

#include "TradePool.h"
#include "TradeOfferPool.h"


#include "UnitData.h"
#include "citydata.h"

#include "Vision.h"

#include "TerrImprovePool.h"
#include "TerrainRecord.h"
#include "TerrainImprovementRecord.h"

#include "Readiness.h"

#include "UnseenCell.h"
#include "MaterialPool.h"
#include "ConstDB.h"

#include "installationpool.h"

#include "installation.h"
#include "installationtree.h"


#include "WonderRecord.h"

#include "radarmap.h"

#include "gold.h"
#include "Sci.h"
#include "TaxRate.h"
#include "Diffcly.h"
#include "Advances.h"
#include "PlayHap.h"
#include "DiplomaticRequestData.h"
#include "DiplomaticRequest.h"
#include "DiplomaticRequestPool.h"

#include "AgreementData.h"
#include "AgreementPool.h"
#include "Agreement.h"
#include "UnitPool.h"
#include "BuildingRecord.h"
#include "director.h"
#include "RandGen.h"
#include "Regard.h"
#include "CivilisationDB.h"
#include "CivilisationPool.h"
#include "MessagePool.h"

#include "profileDB.h"
#include "aicause.h"
#include "CreateUnit.h"
#include "SlicSegment.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "QuickSlic.h"
#include "StrDB.h"
#include "TopTen.h"
#include "Strengths.h"
#include "soundmanager.h"
#include "gamesounds.h"

#include "messagewin.h"
#include "victorymoviewin.h"
#include "GameOver.h"
#include "infowin.h"
#include "victorywin.h"

#include "civapp.h"

#include "Happy.h"
#include "AgreementDynArr.h"

#include "GovernmentRecord.h"



#include "Wormhole.h"

#include "HappyTracker.h"
#include "AchievementTracker.h"

#include "ArmyPool.h"
#include "order.h"
#include "ArmyData.h"
#include "civapp.h"

#include "Score.h"

#include "DiffDB.h"

#include "EndGame.h"

#include "AgeRecord.h"

#ifdef _DEBUG
#include "primitives.h"
#endif

#include "c3dialogs.h"
#include "TradeBids.h"

#include "controlpanelwindow.h"

#include "statswindow.h"
#include "sci_advancescreen.h"

#include "c3_hypertextbox.h"

#include "sciencewin.h"

#include "stringutils.h"
#include "screenutils.h"

#include "EndGameDB.h"
#include "EndgameWindow.h"

#include "GameSettings.h"
#include "WonderTracker.h"

#include "c3ui.h"

#include "net_strengths.h"
#include "net_endgame.h"

#include "gamefile.h"

#include "gameinit.h"

#include "GameEventManager.h"
#include "GovernmentRecord.h"
#include "UnitRecord.h"
#include "TerrainRecord.h"
#include "ResourceRecord.h"

#include "terrainutil.h"
#include "buildingutil.h"

#include "wonderutil.h"

#include "MainControlPanel.h"
#include "trademanager.h"
#include "scenarioeditor.h"

#include "FeatTracker.h"
#include "gaiacontroller.h"
#include "greatlibrary.h"
#include "EventTracker.h"

#include "ctp2_Window.h"


#include "AgreementMatrix.h"
#include "Diplomat.h"
#include "mapanalysis.h"

#include "messagewindow.h"

#include "scenarioeditor.h"

extern C3UI						*g_c3ui;
extern ControlPanelWindow		*g_controlPanel;
extern DataCheck				*g_dataCheck; 
extern CivPaths					*g_civPaths;
extern PointerList<Player>      *g_deadPlayer;
extern Pollution				*g_thePollution ;
extern TopTen					*g_theTopTen ;
extern ConstDB					*g_theConstDB ;
extern AgreementPool			*g_theAgreementPool ;
extern DiplomaticRequestPool	*g_theDiplomaticRequestPool ;
extern RandomGenerator			*g_rand ;
extern CivilisationPool			*g_theCivilisationPool ;
extern MessageWindow *g_currentMessageWindow;

extern DebugWindow				*g_debugWindow;


extern InstallationQuadTree  *g_theInstallationTree; 

extern RadarMap					*g_radarMap;
extern CivApp					*g_civApp;
extern DifficultyDB             *g_theDifficultyDB;
extern

#ifdef _DEBUG
BOOL							g_toggleAdvances ;
#endif

BOOL							g_aPlayerIsDead = FALSE;


#include "newturncount.h"

#include "TurnCnt.h"
extern TurnCount				*g_turn;
#include "tiledmap.h" 
extern TiledMap					*g_tiledMap;

extern BOOL g_powerPointsMode;


extern sint32					g_tileImprovementMode;

sint32					g_specialAttackMode = 0;

extern SoundManager				*g_soundManager;

extern sint32					g_isTileImpPadOn;

extern sint32                   g_isCheatModeOn;

extern void WhackScreen();

#ifdef _DEBUG
extern BOOL							g_ai_revolt;
#endif

#include "BFS.h"


#include "Exclusions.h"

#include "ctpai.h"








Player::Player(const PLAYER_INDEX o, sint32 d, PLAYER_TYPE pt)
{
    memset(this, 0, sizeof(*this)); 

	InitPlayer(o, d, pt) ;
}
















Player::Player(const PLAYER_INDEX o, sint32 d, PLAYER_TYPE pt, const CIV_INDEX civ, GENDER gender)
	{
    memset(this, 0, sizeof(*this)); 

	InitPlayer(o, d, pt) ;
	*m_civilisation = g_theCivilisationPool->Create(m_owner, civ, gender) ;
	}


void Player::InitPlayer(const PLAYER_INDEX o, sint32 diff, PLAYER_TYPE pt)
{


    m_owner = o;  


	g_player[o] = this;

    m_playerType = pt;

	BOOL treatAsRobot = m_playerType == PLAYER_TYPE_ROBOT;
	if(treatAsRobot && (g_startHotseatGame || g_startEmailGame) &&
	   g_hsPlayerSetup[m_owner].isHuman)
		treatAsRobot = FALSE;

    m_all_armies = new DynamicArray<Army>;

	m_all_cities = new UnitDynamicArray;

	m_all_units = new UnitDynamicArray;
	m_traderUnits = new UnitDynamicArray;
	m_capitol = new Unit;
	m_gold = new Gold(o);
	m_science = new Science;
	m_tax_rate = new TaxRate;
	m_difficulty = new Difficulty(diff,o, !treatAsRobot);
	m_advances = new Advances;
	m_tradeOffers = new DynamicArray<TradeOffer>;
	m_requests = new DynamicArray<DiplomaticRequest>;
	m_agreed = new DynamicArray<Agreement>;
	m_messages = new DynamicArray<Message>;
	m_score = new Score(o);
	

	m_regard = new Regard() ;
	m_strengths = new Strengths(o);

	m_goodSalePrices = new sint32[g_theResourceDB->NumRecords()];
	memset(m_goodSalePrices, 0, sizeof(sint32) * g_theResourceDB->NumRecords());

    m_oversea_lost_unit_count = 0; 
	m_home_lost_unit_count = 0; 

	m_global_happiness = new PlayerHappiness;
	
	m_income_Percent = 0;

    sint32 i; 
	m_diplomatic_state[0] = DIPLOMATIC_STATE_WAR;
	m_sent_requests_this_turn[0] = 0;

    for (i=1; i<k_MAX_PLAYERS; i++) { 
		if(m_owner == 0) {
			m_diplomatic_state[i] = DIPLOMATIC_STATE_WAR;
		} else {
			m_diplomatic_state[i] = DIPLOMATIC_STATE_NEUTRAL; 
		}
		m_sent_requests_this_turn[i] = 0;
    }


    *m_capitol = 0;

    Assert(o <32); 
    mask_alliance = 0x01 << o;

	m_set_government_type = 1;
    m_government_type = 1;
	m_change_government_turn = -1;
	m_changed_government_this_turn = FALSE;
	



	m_tradeTransportPoints = 0;
	m_usedTradeTransportPoints = 0;
	m_materialsTax = k_DEFAULT_MATERIALS_TAX;


	m_diplomatic_mute = 0 ;											
	memset(m_pollution_history, 0, sizeof(m_pollution_history)) ;					
	memset(m_event_pollution, 0, sizeof(m_event_pollution)) ;
	memset(m_patience, 0, sizeof(m_patience)) ;

	m_vision = new Vision(m_owner);
    if (0 == m_owner) { 
        m_vision->SetTheWholeWorldExplored(); 
    }

	m_terrainImprovements = new DynamicArray<TerrainImprovement>;
#ifdef BATTLE_FLAGS
	m_battleFlags = new DynamicArray<MapPoint>;
#endif
	m_readiness = new MilitaryReadiness(o);

	m_materialPool = new MaterialPool(o);
	m_allRadarInstallations = new DynamicArray<Installation>;
	m_allInstallations = new DynamicArray<Installation>;
	m_civilisation = new Civilisation ;

	m_throne = NULL;

	m_slic_special_city = new Unit;

	m_builtWonders = 0;
	m_wonderBuildings = 0;

	m_assasinationTimer = 0;
	m_assasinationModifier = 0;

	m_embassies = 0;
	m_productionFromFranchises = 0;

	m_unitRequestList = NULL;

	m_isDead = FALSE;
	m_first_city = TRUE;
	
	m_totalArmiesCreated = 0;
	m_hasUsedCityView = FALSE;
	m_terrainPollution = FALSE ;									

	m_deepOceanVisible = FALSE;

	m_broken_alliances_and_cease_fires = 0;

	if(!treatAsRobot) {
		m_gold->SetLevel(m_difficulty->GetStartingGold());
	} else {
		m_gold->SetLevel(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->m_ai_start_gold);
		m_materialPool->SetLevel(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->m_ai_start_public_works);
	}

	m_is_turn_over = FALSE;
	m_current_round	= NewTurnCount::GetCurrentRound();
	m_end_turn_soon = FALSE;

	m_powerPoints = 0;
	m_lastActionCost = 0;

	m_doneSettingUp = FALSE;

	m_pop_science = 0;

#ifdef _DEBUG
	m_advances->DebugDumpTree();
#endif

    m_mask_hostile = 0;
	m_hasUsedCityView = FALSE;
	m_hasUsedWorkView = FALSE;
	m_hasUsedProductionControls = FALSE;
	m_total_production = 0;
	m_broken_alliances_and_cease_fires = 0;
	m_contactedPlayers = 0;

	m_can_build_capitalization = FALSE;
	m_can_build_infrastructure = FALSE;

	m_can_use_terra_tab = FALSE;
	m_can_use_space_tab = FALSE;

	m_hasGlobalRadar = FALSE;

#ifdef _DEBUG_INCOMPATIBLE
	memset(m_attitude, 0, sizeof(m_attitude)) ;						
#endif
	m_advances->SetOwner(m_owner);

	sint32 maxAdvances;
	DifficultyDBRecord *drec = g_theDifficultyDB->Get(diff);

	if(treatAsRobot) {
		maxAdvances = drec->m_max_ai_advances;
	} else {
		maxAdvances = drec->m_max_human_advances;
	}

	sint32 startAge = 0;
	if(g_network.IsActive() || g_network.IsNetworkLaunch()) {
		startAge = g_network.GetStartingAge();
	}

	sint32 someAdvanceIHave = -1;
	if(startAge == 0) {
		sint32 granted = 0;
		PointerList<DiffAdvanceChance>::Walker walk(drec->m_advanceChances);

		while(walk.IsValid()) {
			sint32 chance;
			if(treatAsRobot) {
				chance = walk.GetObj()->m_aiChance;
			} else {
				chance = walk.GetObj()->m_humanChance;
			}

			if(chance >= 100) {
				m_advances->InitialAdvance(walk.GetObj()->m_adv);
				someAdvanceIHave = walk.GetObj()->m_adv;
				granted++;
			} else if(granted < maxAdvances) {
				if(g_rand->Next(100) < chance) {
					m_advances->InitialAdvance(walk.GetObj()->m_adv);
					granted++;
					someAdvanceIHave = walk.GetObj()->m_adv;
				}
			}

			walk.Next();
		}
	} else {
		
		sint32 i;
#if 0
		const AgeInfo *ainfo = g_theAgeDB->GetAgeInfo(startAge);
		for(i = 0; i < ainfo->ageNumAdvances; i++) {
			m_advances->GiveAdvancePlusPrerequisites(ainfo->ageAdvances[i]);
			someAdvanceIHave = ainfo->ageAdvances[i];
		}
#endif
		
		for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
			if(g_theAdvanceDB->Get(i)->GetAgeIndex() < startAge) {
				m_advances->GiveAdvance(i, CAUSE_SCI_INITIAL, FALSE);
				someAdvanceIHave = i;
			}
		}
	}		
	m_advances->ResetCanResearch(someAdvanceIHave);
	uint8 *canResearch = m_advances->CanResearch();

	for(i = 0; i < m_advances->GetNum(); i++) {
		if(canResearch[i]) {
			m_advances->SetResearching(i);
			break;
		}
	}
	delete [] canResearch;

    m_strengths->Calculate();

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		m_last_attacked[i] = -1;
	}

	if(!g_network.IsClient()) {
		
		
		
		SetWorkdayLevel((sint32)GetWorkdayExpectation());
		SetWagesLevel((sint32)GetWagesExpectation());
		SetRationsLevel((sint32)GetRationsExpectation());
	}

	memset(&m_networkGuid, 0, sizeof(m_networkGuid));
	m_networkId = 0;

	m_civRevoltingCitiesShouldJoin = -1;
	m_hasWonTheGame = FALSE;
	m_hasLostTheGame = FALSE;

    m_descrip_string[0] = 0;

	m_disableChooseResearch = FALSE;

	m_openForNetwork = FALSE;

	m_email = NULL;

	
	m_governorPwReserve = -1;

	m_gaiaController = new GaiaController(o);

	
	for(i = 1; i < g_theGovernmentDB->NumRecords(); i++) {
		if(m_advances->HasAdvance(g_theGovernmentDB->Get(i)->GetEnableAdvanceIndex())) {
			m_set_government_type = i;
			m_government_type = i;
			break;
		}
	}

	m_tax_rate->InitTaxRates(g_theGovernmentDB->Get(m_government_type)->GetMaxScienceRate(),
							 m_owner);
	m_readiness->SetLevel(m_government_type, *m_all_armies, READINESS_LEVEL_WAR);

	if(i >= g_theGovernmentDB->NumRecords()) {
		
		m_set_government_type = 0;
		m_government_type = 0;
	}

	m_age = 0;
	m_researchGoal = -1;
	m_endingTurn = FALSE;
}













Player::Player(CivArchive &archive)
{
    memset(this, 0, sizeof(*this)); 

    m_all_armies = new DynamicArray<Army>;

	m_all_cities = new UnitDynamicArray;

	m_all_units = new UnitDynamicArray;
	m_traderUnits = new UnitDynamicArray;
	m_capitol = new Unit;
	m_gold = new Gold(0);
	m_science = new Science;
	m_tax_rate = new TaxRate;
	m_difficulty = new Difficulty(0, PLAYER_INDEX(0), TRUE);
	m_advances = new Advances;
	m_tradeOffers = new DynamicArray<TradeOffer>;
	m_requests = new DynamicArray<DiplomaticRequest>;
	m_agreed = new DynamicArray<Agreement>;
	m_messages = new DynamicArray<Message>;
	m_regard = new Regard() ;
	m_strengths = new Strengths(0);
	m_global_happiness = new PlayerHappiness;
	m_allRadarInstallations = new DynamicArray<Installation>;
	m_allInstallations = new DynamicArray<Installation>;
	m_vision = new Vision(m_owner);
	m_terrainImprovements = new DynamicArray<TerrainImprovement>;
	m_readiness = new MilitaryReadiness(0);
#ifdef BATTLE_FLAGS
	m_battleFlags = new DynamicArray<MapPoint>;
#endif
	m_materialPool = new MaterialPool(0);
	m_civilisation = new Civilisation ;
	m_unitRequestList = NULL;
	m_score = new Score(PLAYER_INDEX(0));
	
	m_goodSalePrices = new sint32[g_theResourceDB->NumRecords()];
    m_descrip_string[0] = 0;

	Serialize(archive) ;
	}


Player::~Player()
{
	if(m_vision)
		delete m_vision;

	if(m_terrainImprovements)
		delete m_terrainImprovements;

#ifdef BATTLE_FLAGS
	if(m_battleFlags)
		delete m_battleFlags;
#endif
	if(m_readiness)
		delete m_readiness;

	if(m_materialPool)
		delete m_materialPool;

	if(m_allRadarInstallations)	delete m_allRadarInstallations;
	if(m_allInstallations) delete m_allInstallations;

    if(m_all_armies) delete m_all_armies;

	if(m_all_cities) delete m_all_cities;

	if(m_all_units) delete m_all_units;
	if(m_traderUnits) delete m_traderUnits;
	if(m_capitol) delete m_capitol;
	if(m_gold) delete m_gold;
	if(m_science) delete m_science;
	if(m_tax_rate) delete m_tax_rate;
	if(m_difficulty) delete m_difficulty;
	if(m_advances) delete m_advances;
	if(m_tradeOffers) delete m_tradeOffers;
	if(m_requests) delete m_requests;
	if(m_agreed) delete m_agreed;
	if(m_messages) delete m_messages;
	if(m_score) delete m_score;
	
	if(m_regard) delete m_regard;
	if(m_strengths) delete m_strengths;
	if(m_goodSalePrices) delete [] m_goodSalePrices;
	if(m_global_happiness) delete m_global_happiness;




	if(m_civilisation) {			
		if(g_theCivilisationPool->IsValid(*m_civilisation))
			m_civilisation->Kill() ;

		delete m_civilisation ;
	}

	if (m_slic_special_city)
		delete m_slic_special_city;

	if(m_email)
		delete [] m_email;

	if (m_gaiaController)
		delete m_gaiaController;
}












void Player::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

#define PLAYER_MAGIC 0x7F7E7D7C
    if (archive.IsStoring())
		{
		archive.PerformMagic(PLAYER_MAGIC) ;
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_broken_alliances_and_cease_fires)
			+ sizeof(m_broken_alliances_and_cease_fires));

		archive.Store((uint8*)m_goodSalePrices, g_theResourceDB->NumRecords() * sizeof(sint32));
#ifdef _DEBUG_INCOMPATIBLE
        {
			
            sint32 i; 
            for (i=0; i<k_MAX_PLAYERS; i++) { 
	            archive.PutUINT32(m_attitude[i]);
            }
        }
#endif
		}
    else
		{
        archive.TestMagic(PLAYER_MAGIC) ;
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_broken_alliances_and_cease_fires)
			+ sizeof(m_broken_alliances_and_cease_fires));

		archive.Load((uint8*)m_goodSalePrices, g_theResourceDB->NumRecords() * sizeof(sint32));
#ifdef _DEBUG_INCOMPATIBLE
        {
            sint32 i; 
            for (i=0; i<k_MAX_PLAYERS; i++) { 
	            m_attitude[i] = ATTITUDE_TYPE(archive.GetUINT32());
            }
        }
#endif

		}

	m_all_armies->Serialize(archive) ;

    m_all_cities->Serialize(archive) ;

    m_all_units->Serialize(archive) ;
	m_traderUnits->Serialize(archive);
    m_gold->Serialize(archive) ;
    m_science->Serialize(archive) ;
    m_tax_rate->Serialize(archive) ;
	m_difficulty->Serialize(archive) ;
	m_advances->Serialize(archive) ;
	m_tradeOffers->Serialize(archive);
    m_global_happiness->Serialize(archive); 

	uint8 hadVision;
	if(archive.IsStoring()) {
		if(m_vision) {
			hadVision = 1;
			archive << hadVision;
			m_vision->Serialize(archive);
		} else {
			hadVision = 0;
			archive << hadVision;
		}
	} else {
		archive >> hadVision;
		if(!hadVision) {
			if(m_vision) {
				delete m_vision;
				m_vision = NULL;
			}
		} else {			
			Assert(m_vision);
			m_vision->Serialize(archive);
		}
	}
	m_terrainImprovements->Serialize(archive);
	m_readiness->Serialize(archive);
#ifdef BATTTLE_FLAGS
	m_battleFlags->Serialize(archive);
#endif
	m_materialPool->Serialize(archive);
	m_capitol->Serialize(archive) ;
	m_requests->Serialize(archive) ;
	m_agreed->Serialize(archive) ;
	m_messages->Serialize(archive) ;
	m_allRadarInstallations->Serialize(archive) ;
	m_allInstallations->Serialize(archive);
	m_regard->Serialize(archive) ;

	if (!archive.IsStoring() && m_strengths) {
		delete m_strengths;
		m_strengths = new Strengths(0);
	}
	m_strengths->Serialize(archive);
	
	
	
	

	m_civilisation->Serialize(archive) ;

	if(!archive.IsStoring() && g_saveFileVersion < 54) {
		
		sint32 upgrades;
		archive >> upgrades;
		sint32 level[5];
		archive.Load((uint8 *)level, 5 * sizeof(sint32));
	}

	m_score->Serialize(archive);
	

	if(!archive.IsStoring()) {
		
		
		if(m_playerType == PLAYER_TYPE_NETWORK) {
			m_playerType = PLAYER_TYPE_HUMAN;
		}
	}

	if(archive.IsStoring()) {
		sint32 l;
		l = m_email ? strlen(m_email) : 0;
		archive << l;
		if(m_email) {
			archive.Store((uint8*)m_email, l);
		}
	} else {
		sint32 l;
		archive >> l;
		if(l > 0) {
			m_email = new MBCHAR[l + 1];
			archive.Load((uint8*)m_email, l);
			m_email[l] = 0;
		} else {
			m_email = NULL;
		}
	}

	if (m_gaiaController == NULL) {
		m_gaiaController = new GaiaController(m_owner);
	}
	m_gaiaController->Serialize(archive);

}







Unit Player::CreateUnitNoPosition(const sint32 t,
								  CellUnitList &army,
								  const MapPoint &actor_pos,
								  sint32 oldOwner)
{
	
		
		
		
	
	
    Unit u = g_theUnitPool->Create (t, m_owner, actor_pos);

	u.SetFlag(k_UDF_TEMP_SLAVE_UNIT);

	DPRINTF(k_DBG_GAMESTATE, ("Player::CreateUnitNoPosition(t=%d) : newunit=%d\n", 
					  u.GetType(), (uint32)u));
    
	if(g_network.IsClient() && g_network.IsLocalPlayer(oldOwner)) {
		g_network.AddCreatedObject(u.AccessData());
		
		
		g_network.SendAction(new NetAction(NET_ACTION_CREATED_UNIT, (uint32)u));
	} else if(g_network.IsHost()) {
		g_network.Block(oldOwner);
		g_network.Enqueue(u.AccessData());
		g_network.Unblock(oldOwner);
		g_network.AddNewUnit(oldOwner, u);
	}
	army.Insert(u);
	return u;
}








Unit Player::CreateUnit(const sint32 t,         
                        const MapPoint &pos, 
                        const Unit hc,
						BOOL tempUnit, 
                        CAUSE_NEW_ARMY cause)                        
{

	const UnitRecord *rec = g_theUnitDB->Get(t);

	if(!rec)
		return Unit(0);

	if(g_theWorld->GetCell(pos)->GetNumUnits() >= k_MAX_ARMY_SIZE &&
	   !rec->GetIsTrader()) {
		return Unit(0);
	}

	if(g_theWorld->GetCell(pos)->GetNumUnits() > 0 &&
	   g_theWorld->GetCell(pos)->AccessUnit(0).GetOwner() != m_owner) {
		return Unit(0);
	}

	if(cause == CAUSE_NEW_ARMY_CHEAT && 
	   (g_network.IsActive() || g_powerPointsMode)) {
		
		
		if(!g_network.SetupMode() && !g_powerPointsMode) {
			
			return Unit(0);
		}

		if(!g_network.IsInSetupArea(m_owner, pos)) {
			
			
			return Unit(0);
		}

		sint32 pointsNeeded = g_theUnitDB->Get(t)->GetPowerPoints();
		if(pointsNeeded > m_powerPoints) {
			
			return Unit(0);
		}

		m_powerPoints -= pointsNeeded;
		m_lastActionCost = pointsNeeded;

		if(g_network.IsClient()) {
			
			
			g_network.SendAction(new NetAction(NET_ACTION_CREATE_UNIT_CHEAT,
											   t, pos.x, pos.y,
											   (uint32)hc));
			return Unit(0);
		}
	}

	if(!g_theWorld->CanEnter(pos, rec->GetMovementType())) {
		if(!rec->GetIsTrader()) {
			if(!g_theWorld->HasCity(pos) || g_theWorld->GetCity(pos).GetOwner() != m_owner) {
				
				return Unit(0);
			}
		}
	}

	if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK && !g_network.SetupMode()) {
		if(cause != CAUSE_NEW_ARMY_INITIAL) {
			
			
			

			

			
			g_network.Block(m_owner);
		}
	}
	
    Unit u = g_theUnitPool->Create (t, m_owner, pos, hc, NULL);

	if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK &&
	   cause != CAUSE_NEW_ARMY_INITIAL && !g_network.SetupMode()) {
		g_network.Unblock(m_owner);
	}

	DPRINTF(k_DBG_GAMESTATE, ("Player::CreateUnit(t=%d, pos=(%d,%d), hc=%d) : newunit=%d\n", 
					  u.GetType(), pos.x, pos.y, hc.m_id, u.m_id));
    
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.AddCreatedObject(u.AccessData());
		g_network.SendAction(new NetAction(NET_ACTION_CREATED_UNIT, (uint32)u));
	} else if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK &&
			  cause != CAUSE_NEW_ARMY_INITIAL && !g_network.SetupMode()) {
		g_network.AddNewUnit(m_owner, u);
	}

    BOOL revealed_unexplored; 
	if(rec->GetIsTrader()) {
		m_traderUnits->Insert(u);
		AddTransportPoints((sint32)g_theUnitDB->Get(t)->GetMaxMovePoints());

		return u;
	} else {
        UnitDynamicArray	revealed;

		sint32 r = u.SetPosition(pos, revealed, revealed_unexplored); 

		
		
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(u.AccessData());
			g_network.Unblock(m_owner);
		}

		Assert(r); 

		return InsertUnitReference(u, cause, hc);
    }
}

void Player::CreateUnitSoon(sint32 t,
							const MapPoint &point,
							const Unit hc,
							BOOL tempUnit,
							CAUSE_NEW_ARMY cause)
{
	CreateUnitRequest * req = new CreateUnitRequest(t,
													point,
													hc,
													tempUnit,
													cause);
	req->m_next = m_unitRequestList;
	m_unitRequestList = req;
}

void Player::DoCreateUnits()
{
	CreateUnitRequest *req;
	while(m_unitRequestList) {
		req = m_unitRequestList;
		m_unitRequestList = req->m_next;
		CreateUnit(req->m_type, req->m_point, req->m_homeCity,
				   req->m_tempUnit, req->m_cause);
		delete req;
	}
}

void Player::InsertArmy(const MapPoint &point, const Unit &home_city,
						Army &army, CAUSE_NEW_ARMY cause)
{
#if 0
	
	sint32 i, n = army.Num();
	static UnitDynamicArray revealed;
	revealed.Clear();
    BOOL revealed_unexplored; 

	for(i = 0; i < n; i++) {
		army[i].Place(point, home_city);
		sint32 r = army[i].SetPosition(point, revealed, revealed_unexplored);
		Assert(r);
		InsertUnitReference(army[i], cause);
	}
#endif
}

Unit Player::InsertUnitReference(const Unit &u,  const CAUSE_NEW_ARMY cause,
								 const Unit &whereBuilt)
{ 

	Assert(!u.IsCity());

	if(u.IsCity())
		return Unit(0);

   	m_all_units->Insert(u); 
				







	if(cause != CAUSE_NEW_ARMY_NETWORK) {
		if(u.GetArmy().m_id == (0)) {
			Army army = GetNewArmy(cause); 
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_AddUnitToArmy, 
								   GEA_Unit, u,
								   GEA_Army, army,
								   GEA_Int, g_network.IsClient() && cause == CAUSE_NEW_ARMY_INITIAL ? CAUSE_NEW_ARMY_NETWORK : cause,
								   GEA_End);

#if 0
			if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK &&
			   cause != CAUSE_NEW_ARMY_INITIAL && !g_network.SetupMode()) {
				g_network.AddNewArmy(m_owner, army);
				g_network.Unblock(m_owner);
			} else if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK &&
					  cause == CAUSE_NEW_ARMY_INITIAL) {
				
				g_network.Enqueue(army.AccessData());
			} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner) &&
					  cause != CAUSE_NEW_ARMY_INITIAL) {
				g_network.AddCreatedObject(g_theArmyPool->AccessArmy(army));
				g_network.SendAction(new NetAction(NET_ACTION_CREATED_ARMY,
												   army));
			}
#endif
			
			

		}
		g_selected_item->RegisterCreatedUnit(m_owner); 
	}

    m_readiness->SupportUnit(u, m_government_type); 

    return u; 
}


Army Player::GetNewArmy(CAUSE_NEW_ARMY cause)
{
	if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK && 
	   ((cause != CAUSE_NEW_ARMY_INITIAL) && 
		(cause != CAUSE_NEW_ARMY_REMOTE_GROUPING) &&
		(cause != CAUSE_NEW_ARMY_REMOTE_UNGROUPING)) &&
	   !g_network.SetupMode()) {
		g_network.Block(m_owner);
	}

	Army army = g_theArmyPool->Create();
	army.SetOwner(m_owner);
	
	if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK &&
	   cause != CAUSE_NEW_ARMY_INITIAL && 
	   cause != CAUSE_NEW_ARMY_REMOTE_GROUPING && 
	   cause != CAUSE_NEW_ARMY_REMOTE_UNGROUPING &&
	   !g_network.SetupMode()) {
		g_network.AddNewArmy(m_owner, army);
		g_network.Unblock(m_owner);
	} else if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK &&
			  (cause == CAUSE_NEW_ARMY_INITIAL || 
			   cause == CAUSE_NEW_ARMY_REMOTE_GROUPING || 
			   cause == CAUSE_NEW_ARMY_REMOTE_UNGROUPING)) {
		
		g_network.Enqueue(army.AccessData());
	} else if(g_network.IsHost() && cause != CAUSE_NEW_ARMY_TRANSPORTED) {
		g_network.Enqueue(army.AccessData());
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner) &&
		cause != CAUSE_NEW_ARMY_INITIAL) {
		g_network.AddCreatedObject(g_theArmyPool->AccessArmy(army));
		g_network.SendAction(new NetAction(NET_ACTION_CREATED_ARMY,
			army, cause));
	}
#if 0
	if(g_network.IsHost() && m_playerType == PLAYER_TYPE_NETWORK) {
		g_network.AddNewArmy(m_owner, army);
		g_network.Unblock(m_owner);
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.AddCreatedObject(g_theArmyPool->AccessArmy(army));
		g_network.SendAction(new NetAction(NET_ACTION_CREATED_ARMY,
										   (uint32)army));
	}
#endif

	g_gevManager->Pause();
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreatedArmy,
						   GEA_Army, army,
						   GEA_End);
	g_gevManager->Resume();

	
	m_totalArmiesCreated++;
	static MBCHAR buf[40];
	sprintf(buf, "%s%d", g_theStringDB->GetNameStr("ARMY_NAME_PREFIX"), m_totalArmiesCreated);
	army->SetName(buf);

	AddArmy(army, cause, FALSE, 0);
	return army;
}

void Player::AddArmy(const Army &army,
					 const CAUSE_NEW_ARMY cause,
					 BOOL fromNetwork,
					 const Unit &whereBuilt)
{
	if(m_all_armies->IsPresent(army)) {
		Assert(g_network.IsActive());
		return;
	}

	army.IndicateAdded();

	m_all_armies->Insert(army);
	BOOL searching = TRUE;
	uint32 hc_id = 0;

	if(g_network.IsHost() && cause != CAUSE_NEW_ARMY_PARADROP && cause != CAUSE_NEW_ARMY_TRANSPORTED) {
		if(cause != CAUSE_NEW_ARMY_INITIAL)
			g_network.Block(m_owner);

		g_network.Enqueue(g_theArmyPool->AccessArmy(army));
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ADD_ARMY,
									  (uint32)m_owner, 
									  (uint32)cause, 
									  (uint32)army,
									  (uint32)whereBuilt));
		if(cause != CAUSE_NEW_ARMY_INITIAL)
			g_network.Unblock(m_owner);
	}
}

void Player::RemoveArmy(const Army &army, CAUSE_REMOVE_ARMY cause,
						PLAYER_INDEX killedBy, BOOL fromNetwork)
{
	g_selected_item->RegisterRemovedArmy(m_owner, army);
	
	sint32 dead = FindArmyIndex(army);

	
	
	

    
    
	

	m_all_armies->DelIndex(dead);

	if(!m_isDead && CheckPlayerDead()) {
		if(killedBy >= 0) {
			GameOver(GAME_OVER_LOST_CONQUERED, killedBy);
		} else {
			GameOver(GAME_OVER_LOST_INEPT, -1);
		}
	}
}

sint32 Player::FindArmyIndex(const Unit &unit_in_the_army) const
{
	return FindArmyIndex(unit_in_the_army.GetArmy());
}

sint32 Player::FindArmyIndex(const Army &army) const
{
	for(sint32 i = 0; i < m_all_armies->Num(); i++) {
		if(m_all_armies->Access(i).m_id == army.m_id)
			return i;
	}
	Assert(FALSE);
	return -1;
}

sint32 Player::FindCityIndex(const Unit &find) const
{
	for(sint32 i = 0; i < m_all_cities->Num(); i++) {
	  if(m_all_cities->Access(i) == find)
		  return i;
	}
	Assert(FALSE);
	return 0;
}



































sint32 Player::RemoveUnitReference(Unit &kill_me,  const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)

{ 
    int r = FALSE; 
    sint32 is_kill_army = FALSE; 
    sint32 dead_army = 0; 
    MapPoint pos; 
    Unit aCity; 

	DPRINTF(k_DBG_GAMESTATE, ("Player::RemoveUnitReference(%lx, %d)\n",
							  kill_me.m_id, cause));
    
	
	
	
	if (!kill_me.IsCity())
		RemoveUnitReferenceFromPlayer(kill_me, cause, killedBy);

	if (m_all_units->Del(kill_me)){
        r = TRUE; 

        m_readiness->UnsupportUnit(kill_me, m_government_type); 
        kill_me.GetPos(pos); 
    }

    if (RemoveCityReferenceFromPlayer(kill_me, CAUSE_REMOVE_CITY(cause), killedBy)) { 
        r = TRUE;
    } else {
		if(cause != CAUSE_REMOVE_ARMY_SETTLE) {
			m_score->AddUnitLost();
		}
	}

	if(m_traderUnits->Del(kill_me)) {
		RemoveTransportPoints((sint32)g_theUnitDB->Get(kill_me.GetType())->GetMaxMovePoints());
		r = TRUE;
	}

    if (*m_capitol == kill_me) { 
        m_capitol->m_id = (0); 
    }

    return r;
}

void Player::RegisterLostUnits(sint32 nUnits, const MapPoint &pos, 
   const DEATH_EFFECT_MORALE mtype)

{
   Unit aCity; 
   double dist; 

    switch (mtype) { 
    case DEATH_EFFECT_OVERSEAS: 
       m_oversea_lost_unit_count += nUnits; 
       break; 
    case DEATH_EFFECT_HOME:
       m_home_lost_unit_count += nUnits; 
       break; 
    case DEATH_EFFECT_CALC: 
       
        if (GetNearestCity(pos, aCity, dist)) { 
            if (dist <= GetAtHomeRadius()) {         
                m_home_lost_unit_count += nUnits; 
            } else { 
                m_oversea_lost_unit_count += nUnits; 
            }
        } else { 
            m_home_lost_unit_count += nUnits; 
        }
        break; 
    default:
        Assert(0); 
    }
}

void Player::RegisterInsertCargo(ID *id, const sint32 unit_type, sint32 hp)
{
	

	
	

    

    
	
            
            
            
	
    
    

    
    
}

void Player::RegisterUnloadCargo(ID *id, const sint32 unit_type, sint32 hp)
{
	

	
	

    

    
    
    
    
    
    
	

    
    
}

Unit Player::CreateCity(
                        const sint32 t,         
                        const MapPoint &pos, 
                        CAUSE_NEW_CITY cause,
						UnitActor *actor,
						sint32 settlerType)
                        
{
	if(g_theWorld->IsNextToCity(pos) || g_theWorld->IsCity(pos)) {
		
		return Unit(0);
	}

	if(cause == CAUSE_NEW_CITY_CHEAT && 
	   (g_network.IsActive() || g_powerPointsMode)) {
		
		
		if(!g_network.SetupMode() && !g_powerPointsMode) {
			
			return Unit(0);
		}

		if(!g_network.IsInSetupArea(m_owner, pos)) {
			
			
			return Unit(0);
		}

		sint32 pointsNeeded = g_theUnitDB->Get(t)->GetPowerPoints();
		if(pointsNeeded > m_powerPoints) {
			
			return Unit(0);
		}

		m_powerPoints -= pointsNeeded;
		m_lastActionCost = pointsNeeded;

		if(g_network.IsClient()) {
			
			
			g_network.SendAction(new NetAction(NET_ACTION_CREATE_CITY_CHEAT,
											   t, pos.x, pos.y));
			return Unit(0);
		}
	}

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && i != m_owner) {
			g_player[i]->m_vision->AddUnseen(pos);
		}
	}

    Unit u = g_theUnitPool->Create(t, m_owner, pos, 0, actor);

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_CREATED_CITY, (uint32)u));
	} else if(g_network.IsHost() && !g_network.IsLocalPlayer(m_owner)) {
		g_network.AddCreatedCity(m_owner, u);
	}

    sint32 r = g_theWorld->InsertCity(pos, u); 
    Assert(r); 

	
	if (cause == CAUSE_NEW_CITY_CHEAT)
		settlerType = -2; 

    u->InitializeCityData(settlerType);
    
	CityData *cityData = u.GetData()->GetCityData() ;

    AddCityReferenceToPlayer(u, cause);
    
	g_selected_item->RegisterCreatedCity(m_owner);
	sint32 virtgoldspent = 0;
	u.CalcHappiness(virtgoldspent, FALSE); 

	if(g_network.IsHost()) {
		if(cause != CAUSE_NEW_CITY_CHEAT) {
			g_network.Block(m_owner);
		}
		g_network.Enqueue(u.AccessData(), cityData);
		if(cause != CAUSE_NEW_CITY_CHEAT) {
			g_network.Unblock(m_owner);
		}
	}

    
    if (!(g_theAchievementTracker->HasAchieved(ACHIEVE_UNDERSEA_CITY)) &&
        g_theWorld->IsWater(pos)) {
        g_theAchievementTracker->AddAchievement(ACHIEVE_UNDERSEA_CITY);

        SlicObject *so;
        
        so = new SlicObject("016SeaCityBuilder") ;
        so->AddRecipient(m_owner);
        so->AddCity(u);
        g_slicEngine->Execute(so);

        
        so = new SlicObject("017SeaCityOthers") ;
        so->AddAllRecipientsBut(m_owner);
		so->AddCivilisation(g_selected_item->GetVisiblePlayer());
        so->AddCivilisation(m_owner);
        g_slicEngine->Execute(so);
    }

    
    if (!(g_theAchievementTracker->HasAchieved(ACHIEVE_SPACE_CITY)) &&
        g_theWorld->IsSpace(pos)) {
        g_theAchievementTracker->AddAchievement(ACHIEVE_SPACE_CITY);

        SlicObject *so;
        
        so = new SlicObject("014SpaceCityBuilder") ;
        so->AddRecipient(m_owner);
        so->AddCity(u);
        g_slicEngine->Execute(so);

        
        so = new SlicObject("015SpaceCityOthers") ;
        so->AddAllRecipientsBut(m_owner);
        so->AddCivilisation(m_owner);
        g_slicEngine->Execute(so);
    }

    return u; 
}

BOOL Player::AddCityReferenceToPlayer(Unit u,  CAUSE_NEW_CITY cause)

{
   m_all_cities->Insert(u); 

   sint32 virtual_goal=0;
   u.CalcHappiness(virtual_goal, TRUE);
    
   
   

   MapPoint pos; 
   u.GetPos(pos); 

   
   BOOL we_made_the_first_capitol = FALSE; 
   if(m_first_city) {
		m_all_cities->Access(0).AccessData()->GetCityData()->SetCapitol(TRUE);
		*m_capitol = m_all_cities->Access(0);
		m_first_city = FALSE;
        we_made_the_first_capitol = TRUE; 
    }

   
   if (g_theProfileDB->IsAIOn()) {
		
	    
   }

   
   
   if (we_made_the_first_capitol) { 
        sint32 idx_cap = 0; 
       	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++) {
    		if(g_theBuildingDB->Get(i)->GetCapitol()) {
                idx_cap = i; 
                break; 
            } 
        } 

       g_player[m_owner]->RegisterCreateBuilding(u, idx_cap); 
       g_player[m_owner]->RegisterNewCapitolBuilding(u); 
   }

   
   
   
   
   if (m_playerType != PLAYER_TYPE_ROBOT)
   {
		MainControlPanel::UpdateCityList();
   }

	m_maxCityCount = MAX(m_maxCityCount, m_all_cities->Num());

   return TRUE; 
}

BOOL Player::RefreshAIArmyReference(Army &the_army)
{
	bool StopCallingThisFuckingFunction = false;
	Assert(StopCallingThisFuckingFunction);

	return TRUE;
}

BOOL Player::RemoveUnitReferenceFromPlayer(Unit &killme,  CAUSE_REMOVE_ARMY cause, sint32 &killedBy)
{
	sint32 unit_type = killme.GetType();

	
	

	

	if(killme.GetTransport().m_id != (0)) {

		

		

		
		
        sint32 hp = (sint32)killme.GetHP();
		
	} else {
		
		if(!killme.GetDBRec()->GetIsTrader()) {
			
			
			
			
			
		}
	}

	return TRUE;
}


BOOL Player::RemoveCityReferenceFromPlayer(Unit &killme,  CAUSE_REMOVE_CITY cause, sint32 &killedBy)
{
    sint32 i; 
    sint32 n = m_all_cities->Num(); 
    sint32 killme_index = -1; 


    
    for (i=0; i<n; i++) { 
        if (m_all_cities->Get(i).m_id == killme.m_id) { 
            killme_index = i; 
            break;
        }
    }

    if (killme_index < 0) { 
        return FALSE; 
    } 

	if (*m_capitol == killme) { 
        m_capitol->m_id = (0); 
    }

    if (g_theProfileDB->IsAIOn()) {
        
        
    }

    m_all_cities->DelIndex(killme_index); 

	if(g_isCheatModeOn && m_all_cities->Num() < 1) {
		m_first_city = TRUE;
	}

	if(CheckPlayerDead()) {
		if(killedBy >= 0) {
			GameOver(GAME_OVER_LOST_CONQUERED, killedBy);
		} else {
			GameOver(GAME_OVER_LOST_INEPT, -1);
		}
	}

	MainControlPanel::UpdateCityList();

    return TRUE;
}


BOOL Player::RegisterCityAttack(const Unit &c, const PLAYER_INDEX &his_owner, 
								const Unit &his_unit, UNIT_ORDER_TYPE attack_type)
{
	sint32 ai_city_index = FindCityIndex(c);
    if (g_theProfileDB->IsAIOn()) 
	{
		
		
		
		
		
		
	}
	return TRUE;
}






































void Player::ResetAllMovement()
{
    
    sint32 i; 
    UnitDynamicArray   dead;

    if(g_network.IsHost()) {
		g_network.Block(m_owner);
	}
    
    for (i=0; i<m_all_units->Num(); i++) { 
        if (!m_all_units->Access(i).ResetMovement()) {
            dead.Insert(m_all_units->Get(i)); 
        }
    }
    
    
    
    
    dead.KillList(CAUSE_REMOVE_ARMY_OUTOFFUEL, -1); 
	if(g_network.IsHost()) {
		g_network.Unblock(m_owner);
	}
}

    
void Player::BeginTurnScience()
{ 
#if 0
	

    
    
	double totalScience = 0;
	if(m_gold->GetGrossIncome() > 0) {
		
		double incomePercent = double(m_gold->GetIncome()) /
			double(m_gold->GetGrossIncome());
		m_income_Percent = incomePercent;
		m_gold->SetConsiderForScience(incomePercent);
		sint32 i, n = m_all_cities->Num();
		double totalSubGold = 0;
		for(i = 0; i < n; i++) {
			double addscience, subgold;
			m_all_cities->Access(i).ContributeScience(incomePercent,
													  addscience, subgold);
            if(totalSubGold + subgold <= m_gold->GetLevel()) {
                totalSubGold += subgold;
			    totalScience += addscience;
            }
		}
		double s;
		m_tax_rate->GetScienceTaxRate(s);
		if(s > 0.9999999) {
			m_gold->SubIncome(m_gold->GetIncome());
			
		} else {
			m_gold->SubIncome(sint32(totalSubGold));
		}

        sint32 its;
        if (0.0 < totalScience) { 
             its= sint32(totalScience);
             if (its < 0) { 
                 its = INT_MAX-1; 
             } 
        } else { 
            its = 0;
        } 

		m_gold->SetScience(its);
	} else {
		m_gold->SetConsiderForScience(0);
	}

    totalScience *= GetKnowledgeCoef(); 
    double w =  0.01 * double(wonderutil_GetIncreaseKnowledgePercentage(GetBuiltWonders()));
    totalScience += sint32(totalScience * w);
	
	totalScience += m_pop_science;

	totalScience += sint32(double(totalScience) * 
										   (double(g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_SCIENCE, m_owner)) / 100.0));
#endif
	sint32 totalScience = 0;
	sint32 i;
	for(i = 0; i < m_all_cities->Num(); i++) {
		totalScience += m_all_cities->Access(i).CD()->GetScience();
	}

	
	sint32 pactScience = 
		Science::ComputeScienceFromResearchPacts(m_owner);

	totalScience += pactScience;

	DPRINTF(k_DBG_GAMESTATE, ("Adding %d science to player %d, who had %d\n", totalScience, m_owner, m_science->GetLevel()));

	
    AddScience(sint32(totalScience) ); 

	double otherCivRandomAdvanceChance = wonderutil_GetOtherCivRandomAdvanceChance(m_builtWonders);
	if(otherCivRandomAdvanceChance > 0 &&
	   g_rand->Next(100) < otherCivRandomAdvanceChance) {
		
		sint32 i, j;

		
		uint8 *mergedCanGet = new uint8[g_theAdvanceDB->NumRecords()];
		memset(mergedCanGet, 0, g_theAdvanceDB->NumRecords());
		sint32 num;
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				uint8* canGet = g_player[i]->m_advances->CanOffer(m_advances, num);
				for(j = 0; j < g_theAdvanceDB->NumRecords(); j++) {
					mergedCanGet[j] = mergedCanGet[j] || canGet[j];
				}
				delete [] canGet;
			}
		}
		
		
		num = 0;
		for(j = 0; j < g_theAdvanceDB->NumRecords(); j++) {
			if(mergedCanGet[j])
				num++;
		}

		if(num > 0) {
			
			sint32 which = g_rand->Next(num);
			sint32 count = 0;
			for(j = 0; j < g_theAdvanceDB->NumRecords(); j++) {
				if(mergedCanGet[j]) {
					if(count == which) {
						m_advances->GiveAdvance(j, CAUSE_SCI_WONDER, FALSE);

                        SlicObject *so = new SlicObject("116InternetDiscovery");
                        so->AddRecipient(m_owner);
                        so->AddAdvance(j);
                        g_slicEngine->Execute(so);
						break;
					}
					count++;
				}
			}
		}
	}
}


void Player::BeginTurnAllCities()

{
    int i, n; 
    UnitDynamicArray    dead;
	
	m_pop_science = 0;

    m_gold->SetSavings();

    
    m_readiness->BeginTurn(m_government_type); 

	
	
	if(m_capitol && g_theUnitPool->IsValid(m_capitol->m_id)) {
		MapPoint pos;
		m_capitol->GetPos(pos);
		g_theWorld->FindCityDistances(m_owner, pos);
	}
	

    n = m_all_cities->Num(); 
	sint32 virtualGoldSpent = 0;
	for(i = 0; i < n; i++) {
		m_all_cities->Access(i).CalcHappiness(virtualGoldSpent, TRUE);
		m_all_cities->Access(i).CheckRiot();
	}

    
    
    BeginTurnProduction();

    
    



    n = m_all_cities->Num(); 
    if (0 < n) { 
        UnitDynamicArray tmp_city_list(n); 
        tmp_city_list = *m_all_cities; 
        for (i=0; i<n; i++) { 
            if (g_theUnitPool->IsValid(tmp_city_list.Access(i))) { 
                tmp_city_list.Access(i).BeginTurnCity(dead); 
            }
        } 
    }
    m_gold->SetGrossIncome(); 

	
	if(m_playerType == PLAYER_TYPE_ROBOT &&
	   !(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
		sint32 age = 0; 
		sint32 desired = sint32(double(m_gold->GetGrossIncome()) * 
								g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAiGoldAdjustment(m_owner,age));
		if(desired != 0) {
			if(desired > m_gold->GetGrossIncome()) {
				m_gold->AddIncome(desired - m_gold->GetGrossIncome());
			} else {
				m_gold->SubIncome(m_gold->GetGrossIncome() - desired);
			}
			m_gold->SetGrossIncome();
		}
	}

     
    dead.KillList(CAUSE_REMOVE_ARMY_UNKNOWN, -1);

    
    
    
    n = m_all_cities->Num(); 
    for (i=0; i<n; i++) { 
		m_all_cities->Access(i).SupportBuildings();
    }

	
    sint32 w = int(GetWagesPerPerson()); 
	sint32 wagesNeeded = 0;
	sint32 oldLevel = sint32(GetUnitlessWages());
	if(oldLevel > -2) {
		do {
			for(i = 0; i < n; i++) {
				wagesNeeded += m_all_cities->Access(i).GetWagesNeeded();
			}
			
			if(wagesNeeded > m_gold->GetLevel()) {
				SetWagesLevel(sint32(GetUnitlessWages()) - 1);
				w = sint32(GetWagesPerPerson());
			}
		} while (wagesNeeded > m_gold->GetLevel() && GetUnitlessWages() > -1);
	}

	if(oldLevel != GetUnitlessWages()) {
		SlicObject *so = new SlicObject("103WageLevelReset");
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
	}

    
    
    for (i=0; i<n; i++) { 
        m_all_cities->Access(i).PayWages(w); 
    }

    
    for (i=0; i<n; i++) { 
        m_all_cities->Access(i).CalcHappiness(virtualGoldSpent, FALSE); 
		g_slicEngine->RunCityTriggers(m_all_cities->Access(i));

    }

	
	

	
	BeginTurnWonders();

    
    BeginTurnScience();

	
    for(i = m_all_cities->Num() - 1; i >= 0; i--) {
		m_gold->AddGold(m_all_cities->Access(i).GetData()->GetCityData()->GetGoldFromCapitalization());
	}
}


void Player::BeginTurnProduction()

{ 
    sint32 n = m_all_cities->Num(); 
    sint32 i;
	sint32 mil_total=0;
	sint32 mat_total=0; 
	sint32 actual_total=0;
	sint32 delta; 
	sint32 materialsFromFranchise = 0;

	for (i=0; i<n; i++) { 
        m_all_cities->Access(i).CD()->CollectResources();
		delta = m_all_cities->Access(i).CD()->ProcessProduction(false);
		mil_total += delta;
		mat_total += delta;

        actual_total += delta;
	}

	m_total_production = sint32(actual_total);

	
	
	
	
	// #01 Recalculating military support costs after government change
	// this fixes the costs for old save games 
	m_readiness->RecalcCost();
	
	
	
	if(m_productionFromFranchises > m_readiness->GetCost()) {
		materialsFromFranchise = (sint32)(m_productionFromFranchises - m_readiness->GetCost());
	}

	DPRINTF(k_DBG_GAMESTATE, ("Cost before killing units: %lf\n", m_readiness->GetCost()));

	if(!m_first_city) {
		
		
		m_readiness->KillUnitsOverBudget(m_government_type, *m_all_armies, int(mil_total) + m_productionFromFranchises);
	}
	
	
	double p;
	sint32 roundedCost = 0;
	if (mil_total < 1) { 
		p = 0;
	} else {
		roundedCost = (sint32)(m_readiness->GetCost() + 0.0000001) - m_productionFromFranchises;
		p = double(roundedCost)/double(mil_total);
	}
	
	DPRINTF(k_DBG_GAMESTATE, ("cost: %d, mil_total: %d, franchises: %d, total prod: %d\n",
							  roundedCost, mil_total, 
							  m_productionFromFranchises,
							  m_total_production));

	m_readiness->SetPecentLastTurn(p); 
	
	Assert (p <= 1.000001);
	
	sint32 mil_paid, mat_paid;
	sint32 mil_paid_total=0; 
    sint32 r; 
	
	if (0 < n) { 
		for (i=0; i<(n-1); i++) { 
			m_all_cities->Access(i).PayFederalProduction (p, mil_paid, m_materialsTax, mat_paid);
            mil_paid_total += mil_paid;
			m_materialPool->AddMaterials(mat_paid);
		}
		r = roundedCost - mil_paid_total;
		
		m_all_cities->Access(i).PayFederalProductionAbs (r, 
														 m_materialsTax, 
														 mat_paid);
		m_materialPool->AddMaterials(mat_paid);
	}
	if(materialsFromFranchise > 0) {
		m_materialPool->AddMaterials(materialsFromFranchise);
	}
	
	m_productionFromFranchises = 0;
}
void Player::BeginTurnImprovements()
{
	int i, n;
	
	n = m_terrainImprovements->Num();
	
	
	for(i = 0; i < n; i++) {
		if(!m_terrainImprovements->Access(i).IsBuilding()) {
			if(m_materialPool->GetMaterials() >= m_terrainImprovements->Access(i).GetMaterialCost())
				m_terrainImprovements->Access(i).StartBuilding();
			else
				break;
		}
	}

	
	for(i=0; i < n; i++) {
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_ImprovementAddTurn,
							   GEA_Improvement, m_terrainImprovements->Access(i),
							   GEA_End);

		
	}

	
}

void Player::BeginTurnEnemyUnits()
{
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BEGIN_TURN_ENEMY_UNITS,
									  m_owner));
		g_network.Unblock(m_owner);
	}

	sint32 i, n;
	for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
		if(p == m_owner)
			continue;
		if(!g_player[p]) continue;

		if(g_network.IsHost()) {
			g_network.Block(p);
			g_network.Block(m_owner);
		}

		n = g_player[p]->m_all_units->Num();
		for(i=0; i<n; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail,
								   GEV_UnitBeginTurnVision,
								   GEA_Unit, g_player[p]->m_all_units->Access(i),
								   GEA_Player, m_owner,
								   GEA_End);
			
		}

		n = g_player[p]->m_all_cities->Num();
		for(i=0; i<n; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail,
								   GEV_CityBeginTurnVision,
								   GEA_City, g_player[p]->m_all_cities->Access(i),
								   GEA_Player, m_owner,
								   GEA_End);
			
		}
		if(g_network.IsHost()) {
			g_network.Unblock(p);
			g_network.Unblock(m_owner);
		}

	}

	
	
	
	
	
	
	
	
}

void Player::BeginTurnUnits()
{
	sint32 i, n;
    BOOL i_died=FALSE; 
	
    BOOL is_transported=FALSE; 

	
	
	m_can_use_sea_tab = FALSE;

	
	m_cargoCapacity = 0;

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
	}
	n = m_all_units->Num();
	for(i = 0; i < n; i++) {
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnUnit,
							   GEA_Unit, m_all_units->Access(i),
							   GEA_End);
		
	}
	
	n = m_all_armies->Num();
	for(i = n - 1; i >= 0; i--) {
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnArmy,
							   GEA_Army, m_all_armies->Access(i),
							   GEA_End);
		
	}

	if(g_network.IsHost()) {
		g_network.Unblock(m_owner);
	}
}

void Player::BeginTurnWonders()
{
	m_gold->AddGold(CalcWonderGold());

	
	g_theWonderTracker->RecomputeIsBuilding(m_owner);
}

sint32 Player::CalcTotalBuildingUpkeep()
{
    sint32 i, n;
    sint32 bu = 0;
    sint32 wonderLevel = wonderutil_GetDecreaseMaintenance(m_builtWonders);

    n = m_all_cities->Num();
    for(i = 0; i < n; i++) {
        Unit *c = &(m_all_cities->Access(i));
        bu += buildingutil_GetTotalUpkeep(c->GetImprovements(),
                                             wonderLevel);
    }        

    return(bu);
}


sint32 Player::CalcWonderGold()
{
	sint32 totalWonderGold = 0;
	sint32 i, n;
	sint32 goldPerWaterRoute = wonderutil_GetGoldPerWaterTradeRoute(m_builtWonders);
	sint32 goldPerInternationalRoute = wonderutil_GetGoldPerInternationalTradeRoute(m_builtWonders);
	if(goldPerWaterRoute > 0 || goldPerInternationalRoute > 0) {
		n = g_theTradePool->GetAllRoutes().Num();
		for(i = 0; i < n; i++) {
			sint32 owner;
			owner = g_theTradePool->GetAllRoutes().Get(i).GetSource().GetOwner();
			

				if(g_theTradePool->GetAllRoutes().Get(i).CrossesWater()) {
					totalWonderGold += goldPerWaterRoute;
				}

			sint32 destOwner;
			destOwner = g_theTradePool->GetAllRoutes().Get(i).GetDestination().GetOwner();
			if(destOwner != owner)
				totalWonderGold += goldPerInternationalRoute;
		}
	}

	sint32 goldPerTelevision = wonderutil_GetGoldPerTelevision(m_builtWonders);
	if(goldPerTelevision > 0) {
		for(sint32 p = 0; p < k_MAX_PLAYERS; p++) {
			if(p == m_owner)
				continue;
			if(!g_player[p]) continue;

			n = g_player[p]->m_all_cities->Num();
			for(i = 0; i < n; i++) {
				if(buildingutil_GetTelevision(
					g_player[p]->m_all_cities->Access(i).CD()->GetEffectiveBuildings())) {
					totalWonderGold += goldPerTelevision * g_player[p]->m_all_cities->Access(i).PopCount();
				}
			}
		}
	}

	sint32 wonderBonusGold = wonderutil_GetBonusGold(m_builtWonders);
	totalWonderGold += wonderBonusGold;

	return totalWonderGold;
}








#if 0


void Player::BeginTurn()

{
	
	
	
	

#ifdef _DEBUG  
    AIValidateArmyID();
#endif

	m_armies_moved_this_turn = 0;

	
	
	if (g_tileImprovementMode) 
	{
		g_tileImprovementMode = 0;
		g_debugWindow->AddText("Tile Improvement Mode - OFF");
	}

	
	if(g_controlPanel) g_controlPanel->UpdatePlayerBeginProgress(m_owner);

	for(sint32 i = m_messages->Num() - 1; i >= 0; i--) {
		if(m_messages->Access(i).GetExpiration() <= GetCurRound()) {
			m_messages->Access(i).Kill();
		}
	}

	m_is_turn_over = FALSE;
	m_current_round = NewTurnCount::GetCurrentRound();
	m_end_turn_soon = FALSE;

	if(!g_network.IsActive() || g_network.IsHost() || (m_owner == g_network.GetPlayerIndex())) {
		DPRINTF(k_DBG_GAMESTATE, ("Player[%d]::BeginTurn: running\n", m_owner));

		m_civRevoltingCitiesShouldJoin = -1;

		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			m_sent_requests_this_turn[p] = 0;
		}
		if(g_wormhole) {
			g_wormhole->BeginTurn(m_owner);
		}

		
		

		

		
		
		
       	m_global_happiness->CalcPeaceMovement(this, *m_all_armies, 
											 *m_all_cities);
		if(m_assasinationTimer > 0) {
			m_assasinationTimer--;
			if(m_assasinationTimer <= 0)
				m_assasinationModifier = 0;
		}

		
		m_gold->ClearStats(); 

		
		BeginTurnPollution();

		BeginTurnAllCities(); 

		g_thePollution->BeginTurn() ;


		BeginTurnImprovements();
		BeginTurnEnemyUnits();
		BeginTurnMonopoly() ;
		BeginTurnAgreements();

		
		
		
		ResetAllMovement();

		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue((uint8)m_owner, m_global_happiness);
			g_network.Unblock(m_owner);
		}

		m_oversea_lost_unit_count = 0; 
		m_home_lost_unit_count = 0; 

		
		#ifdef _DEBUG
		if (m_playerType != PLAYER_TYPE_ROBOT || g_ai_revolt)
		#endif
			
			AttemptRevolt() ;

		for(i = 0; i < m_all_cities->Num(); i++) {
			if(g_network.IsHost()) {
				g_network.Enqueue(m_all_cities->Access(i).AccessData(), 
								  m_all_cities->Access(i).AccessData()->GetCityData());
			}
		}

		

		
		if(g_endgameWindow && (m_owner == g_selected_item->GetVisiblePlayer()))
			g_endgameWindow->UpdateTurn(m_endGame);

		if(g_network.IsHost()) {
			g_network.SyncRand();
			g_network.Block(m_owner);
		}
		BeginTurnUnits();
		if(g_network.IsHost()) {
			g_network.Unblock(m_owner);
		}

		if(m_change_government_turn == GetCurRound()) {
			ActuallySetGovernment(m_set_government_type);
			m_changed_government_this_turn = TRUE;
		} else {
			m_changed_government_this_turn = FALSE;
		}

		g_slicEngine->RunPlayerTriggers(m_owner);

		CheckResourcesForTutorial();

		if(g_network.IsHost()) {
			
			
			
			g_network.SyncRand();
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_TURN_SYNC));

			if(m_playerType == PLAYER_TYPE_HUMAN) {
				
			}
		}		
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Player[%d]::BeginTurn: not running\n", m_owner));
	}

	if(!g_network.IsActive() || g_network.IsHost()) {
		m_strengths->Calculate();
		if(g_network.IsHost()) {
			g_network.QueuePacketToAll(new NetStrengths(m_owner));
		}
	}

	BOOL atPeace = TRUE;

	
	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(m_contactedPlayers & (1 << i) && g_player[i]) {
			if(m_diplomatic_state[i] == DIPLOMATIC_STATE_WAR) {
				atPeace = FALSE;
				break;
			}
#if 0
			if(!g_player[m_owner]->WillViolatePact(i) &&
			   !g_player[m_owner]->WillViolateCeaseFire(i)) {
				atPeace = FALSE;
				break;
			}
#endif
		}
	}


	
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CalcScores,
							   GEA_Player, m_owner,
							   GEA_End);
		
	

	if (!m_isDead)  {  
		
		
		
		
		
	}
	DPRINTF(k_DBG_GAMESTATE, ("It's player %d's turn - year %d.\n", m_owner, GetCurRound()));
	DPRINTF(k_DBG_GAMESTATE, ("Gold: %d\n", m_gold->GetLevel()));


	
	if ( m_owner == g_selected_item->GetVisiblePlayer() ) {


		if ( g_player[m_owner]->m_can_use_space_button ) {
			g_controlPanel->ShowSpaceButton();
		}

		g_c3ui->AddAction( new SW_UpdateAction );
		g_c3ui->AddAction( new USS_UpdateAction );
		g_c3ui->AddAction( new CSW_UpdateAction(TRUE) );



	}

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_GOLD,
											   m_owner, m_gold->GetLevel()));
		// propagate PW each turn update
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_MATERIALS,
											   m_owner, m_materialPool->GetMaterials()));
		g_network.Unblock(m_owner);
	}

	if((m_playerType == PLAYER_TYPE_HUMAN ||
		m_playerType == PLAYER_TYPE_NETWORK && g_network.IsLocalPlayer(m_owner)) &&
	   m_owner == g_selected_item->GetVisiblePlayer() &&
	   g_theProfileDB->IsAutoSelectFirstUnit()) {
		if(g_selected_item->GetState() == SELECT_TYPE_NONE) {
			g_selected_item->NextUnmovedUnit(TRUE);
		} else if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY) {
			g_selected_item->MaybeAutoEndTurn(TRUE);
		}
	}

#ifdef _DEBUG  
    AIValidateArmyID();
#endif

}
#endif

void Player::EndTurnSoon()
{
	m_end_turn_soon = TRUE;
}












#define _KILL_MESSAGES_EACH_TURN	1

void Player::EndTurn()

{
    int i; 
    sint32 n;





	
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_END_TURN_FOR,
						              m_owner));
	}

	
	
	m_current_round = NewTurnCount::GetCurrentRound();

	m_is_turn_over = TRUE;
	m_end_turn_soon = FALSE;

	

	
	if(g_controlPanel)
	{
		
		
		
		g_controlPanel->GetWindow()->DrawChildren();
	}

    if (m_isDead) 
        return; 

	
	if (g_tileImprovementMode) 
	{
		g_tileImprovementMode = 0;
		g_debugWindow->AddText("Tile Improvement Mode - OFF");
		
		
	}

	if ( g_specialAttackMode ) {
		g_specialAttackMode = 0;
		
	}

	
	if(g_network.IsClient() && m_owner != g_network.GetPlayerIndex())
		return;
	
	

	g_theDiplomaticRequestPool->EndTurn(m_owner) ;
	m_terrainPollution = FALSE ;
	
	n = m_all_cities->Num();
	for(i = 0; i < n; i++) {
		m_all_cities->Access(i).EndTurnCity();
	}


    
    if ((m_gold->GetLevel() < 50) && (m_gold->DeltaThisTurn() < 0) &&
        (g_slicEngine->GetSegment("027NotEnoughGold")->TestLastShown(m_owner, 10))) {
        SlicObject *so = new SlicObject("027NotEnoughGold") ;
        so->AddRecipient(m_owner) ;
		so->AddPlayer(m_owner);
        g_slicEngine->Execute(so) ;
    }

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
	}
    
    UnitDynamicArray	tmpUnits(*m_all_units);
	n = tmpUnits.Num();
	for(i = 0; i < n; i++) {
		
		
		
		
		if(g_theUnitPool->IsValid(tmpUnits[i])) {
			tmpUnits[i].EndTurn();
		}
	}

	if(g_network.IsHost()) {
		g_network.Unblock(m_owner);
	}

	
    
    
    

	
	
































	
	
	
	
	if (!g_theProfileDB->DontKillMessages()) {
		
		sint32 numMessages = m_messages->Num();
		
		if (numMessages > k_MAX_MESSAGES_KEPT) {
			for (i=0; i< (numMessages - k_MAX_MESSAGES_KEPT); i++) {
				m_messages->Access(0).Kill();
			}
		}
	}

	
	EndTurnPollution();

	
	if (!g_network.IsActive() && GetGaiaController()->TurnsToComplete() == 0)
	{
		GameOver(GAME_OVER_WON_SCIENCE, -1);
	}
}














void Player::EndTurnPollution(void)
{

















	
	Unit city;
	sint32 i;
	memmove(&m_pollution_history[1], &m_pollution_history[0], sizeof(m_pollution_history) - sizeof(m_pollution_history[0])) ;
	m_pollution_history[0] = 0 ;
	for(i = 0; i < m_all_cities->Num(); i++) 
	{
		city = m_all_cities->Access(i);
		if(g_theGameSettings->GetPollution())
		{
			m_pollution_history[0] += city.GetPollution();
		}
		else
		{
			m_pollution_history[0] = 0;
		}
	}

	
	
	
	if(GetPlayerType() == PLAYER_TYPE_ROBOT) {
		uint32 target_pollution;
		sint32 foreigner_pollution;
		const double max_difference = 0.75;
		sint32 original_pollution = m_pollution_history[0];
		ai::Agreement pollution_agreement;
		for (sint32 foreignerId = 1; foreignerId < k_MAX_PLAYERS; foreignerId++)
		{
			
			if (g_player[foreignerId] == NULL)
				continue;
			
			
			if (foreignerId == m_owner)
				continue;
			
			foreigner_pollution = g_player[foreignerId]->GetPollutionLevel();
			
			
			if (AgreementMatrix::s_agreements.HasAgreement(m_owner, foreignerId, PROPOSAL_OFFER_REDUCE_POLLUTION))
			{
				pollution_agreement = AgreementMatrix::s_agreements.GetAgreement(m_owner, foreignerId, PROPOSAL_OFFER_REDUCE_POLLUTION);
				target_pollution = 0;
				if ((pollution_agreement.senderId == m_owner && 
					 pollution_agreement.proposal.first_type == PROPOSAL_OFFER_REDUCE_POLLUTION) ||
					(pollution_agreement.receiverId == m_owner && 
					 pollution_agreement.proposal.first_type == PROPOSAL_REQUEST_REDUCE_POLLUTION))
				{
					target_pollution = pollution_agreement.proposal.first_arg.pollution;
				}
				else if ((pollution_agreement.senderId == m_owner && 
						  pollution_agreement.proposal.second_type == PROPOSAL_OFFER_REDUCE_POLLUTION) ||
						(pollution_agreement.receiverId == m_owner && 
						 pollution_agreement.proposal.second_type == PROPOSAL_REQUEST_REDUCE_POLLUTION))
				{
					target_pollution = pollution_agreement.proposal.second_arg.pollution;
				}
				
				
				if (target_pollution > original_pollution * max_difference &&
					target_pollution > foreigner_pollution * max_difference &&
					m_pollution_history[0] > target_pollution)
				{
					
					m_pollution_history[0] = target_pollution;
				}
			} 
			
			if (AgreementMatrix::s_agreements.HasAgreement(m_owner, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT))
			{
				pollution_agreement = AgreementMatrix::s_agreements.GetAgreement(m_owner, foreignerId, PROPOSAL_TREATY_POLLUTION_PACT);
				
				target_pollution = 0;
				if (pollution_agreement.proposal.first_type == PROPOSAL_TREATY_POLLUTION_PACT)
				{
					target_pollution = pollution_agreement.proposal.first_arg.pollution;
				}
				else if (pollution_agreement.proposal.second_type == PROPOSAL_TREATY_POLLUTION_PACT)
				{
					target_pollution = pollution_agreement.proposal.second_arg.pollution;
				}
				
				
				if (target_pollution > original_pollution * max_difference &&
					target_pollution > foreigner_pollution * max_difference &&
					m_pollution_history[0] > target_pollution)
				{
					
					m_pollution_history[0] = target_pollution;
				}
			} 
		} 
	}
}


void Player::ProcessUnitOrders(BOOL currentOnly)
{





    DynamicArray<Army> tmp_list = *m_all_armies; 
    sint32 army_idx, army_num = tmp_list.Num(); 

    army_num = tmp_list.Num(); 
    for (army_idx=army_num-1; 0 <= army_idx; army_idx--) { 
		if(g_theArmyPool->IsValid(tmp_list.Access(army_idx))) {
			tmp_list.Access(army_idx).ExecuteOrders();
		}
    } 
}

sint32 Player::GetAverageHappiness()
{
	if(m_all_cities->Num()<1)
	{
		return 0;
	}
	sint32 i;
	sint32 totalHappiness=0;
	for(i=0; i<m_all_cities->Num(); i++)
	{
		totalHappiness += (sint32)(m_all_cities->Get(i)->GetHappiness());
	}
	totalHappiness /= m_all_cities->Num();
	return totalHappiness;
}

void Player::RegisterProfessionalChange(BOOL on, Unit &u)
{
    if (on) { 
        m_readiness->UnsupportUnit(u, m_government_type); 
    } else { 
        m_readiness->SupportUnit(u, m_government_type); 
    }
}



sint32 Player::GetTotalUnitCost() 
{ 
    sint32 unit_num, unit_idx; 

    unit_num = m_all_units->Num(); 
    sint32 cost = 0; 
    UnitRecord *rec=NULL; 
    for (unit_idx=0; unit_idx<unit_num; unit_idx++) { 
        rec = g_theUnitDB->Access(m_all_units->Access(unit_idx).GetType());
        Assert(rec); 

        if (rec->GetHasPopAndCanBuild()) continue; 

        if (rec->GetIsTrader()) continue; 

    	cost += rec->GetShieldCost();
    }

    return cost;
}


void Player::DelTailPathOrder(sint32 index)

{
	
    m_all_armies->Access(index).ClearOrders();
}

sint32 Player::GetNearestCity(const MapPoint &pos, Unit &nearest, 
							  double &distance, BOOL butNotThisOne, const sint32 continent)
{
    sint32 j, n;
    MapPoint cpos, diff; 
    double d; 
	sint32 cont;

    
    if (g_theWorld->HasCity(pos) && !butNotThisOne) {
       nearest = g_theWorld->GetCity(pos);
       if (nearest.GetOwner() == m_owner) { 
           distance = 0; 
           return TRUE; 
       } 
    }

    nearest.m_id = (0); 
    distance = -1.0; 
    n = m_all_cities->Num(); 
    
    for (j=0; j<n; j++) { 
        m_all_cities->Get(j).GetPos(cpos); 
		if(cpos == pos && butNotThisOne)
			continue;
		
		
		if (continent != -1)
		{
			cont = g_theWorld->GetContinent(cpos);
			if (cont != continent)
				continue;
		}

		
        
        
        
		d = MapPoint::GetSquaredDistance(pos, cpos);

        if (distance == -1.0 || d < distance) 
        { 
            distance = d; 
            nearest = m_all_cities->Get(j);
        }
    }

	
	if (distance > -1.0)
		distance = sqrt(distance);

    return nearest.m_id != (0);
}

sint32 Player::GetSlaveCity(const MapPoint &pos, Unit &city)
{
	MapPoint cpos;
	sint32 d;
	sint32 i, j;

	sint32 minDistance = 0x7fffffff;
	city.m_id = (0);
	for(i = m_all_cities->Num() - 1; i >= 0; i--) {
		Unit c = m_all_cities->Access(i);

		c.GetPos(cpos);
		Cell *cell = g_theWorld->GetCell(cpos);
		sint32 numMilitaryUnits = 0;
		for(j = 0; j < cell->GetNumUnits(); j++) {
			if(cell->UnitArmy()->Access(j).GetAttack() > 0) {
				numMilitaryUnits++;
			}
		}
		if(numMilitaryUnits * g_theConstDB->SlavesPerMilitaryUnit() <=
		   c.CountSlaves()) {
			continue;
		}
		d = pos.NormalizedDistance(cpos);

		if(d < minDistance) {
			minDistance = d;
			city = m_all_cities->Access(i);
		}
	}
	if(city.m_id != (0))
		return TRUE;

	
	double distance;
	return GetNearestCity(pos, city, distance);
}

BOOL Player::GetNearestFort(const MapPoint &src, MapPoint &dest)
{
	sint32 i;
	MapPoint chkpos, diff;
	double d;
	double mindist = 1000000;
	BOOL foundOne = FALSE;

	for(i = 0; i < m_allInstallations->Num(); i++) {
		chkpos = m_allInstallations->Access(i).RetPos();
		if(terrainutil_HasFort(chkpos)) {
			d = MapPoint::GetSquaredDistance(src, chkpos);

			if(d < mindist) {
				mindist = d;
				dest = chkpos;
				foundOne = TRUE;
			}
		}
	}
	return foundOne;
}

BOOL Player::GetNearestAirfield(const MapPoint &src, MapPoint &dest, const sint32 continent)
{
	sint32 i;
	MapPoint chkpos, diff;
	double d;
	double mindist = -1.0;
	BOOL foundOne = FALSE;
	sint32 cont;

	for(i = 0; i < m_allInstallations->Num(); i++) {
		chkpos = m_allInstallations->Access(i).RetPos();
		if(terrainutil_HasAirfield(chkpos)) {
			
			if (continent != -1)
			{
				cont = g_theWorld->GetContinent(chkpos);
				if (cont != continent)
					continue;
			}

			d = MapPoint::GetSquaredDistance(src, chkpos);

			if(mindist == -1.0 || d < mindist) {
				mindist = d;
				dest = chkpos;
				foundOne = TRUE;
			}
		}
	}

	return foundOne;
}

sint32 Player::Settle(Army &settle_army)

{
	MapPoint pos;
	sint32 curID;
	
	BOOL isFromOrderQueue = FALSE; 

    sint32 searching = TRUE;
    sint32 i;

    if (m_all_armies->Num() < 1) {
		DPRINTF(k_DBG_GAMESTATE, ("No armies?!\n"));
        return FALSE; 
	}

 	settle_army.GetPos(pos);

    if (g_theWorld->HasCity(pos)) { 
		DPRINTF(k_DBG_GAMESTATE, ("Settling on top of a city\n"));
        return FALSE; 
	}

	
	
	

    sint32 n = settle_army.Num(); 
    Assert(0<n);

    for (i=0; i<n; i++) { 

        curID = settle_army[i].m_id; 
        
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		BOOL isVisible = settle_army[0].GetVisibility() & (1 << visiblePlayer);
        if (settle_army[i].Settle()) { 
            searching = FALSE;

#ifdef MOVED_TO_SOUNDEVENT_CPP
			if (g_soundManager) {
				if (visiblePlayer == m_owner ||	isVisible) {
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
											gamesounds_GetGameSoundID(GAMESOUNDS_SETTLE_CITY),
											pos.x,
											pos.y);
				}
			}

#endif

#ifdef MOVED_TO_INTERFACEEVENT_CPP
			sint32 city_idx = m_all_cities->Num()-1;
			if (0 <= city_idx) { 
				
				
				if (m_owner == g_selected_item->GetVisiblePlayer()) {
					c3_utilitydialogbox_NameCity(m_all_cities->Access(city_idx));
				}

				
				
			}
#endif

            break;
        }
    }

    if (searching) {




        return FALSE; 
	}




		                                         
		

	return TRUE;
}








#if 0

void Player::UnloadAllTransportsInArmy(const sint32 selected_army, 
    const MapPoint &pos, BOOL &did_move, BOOL &i_died, ArmyList &dead_attacker,
	BOOL &revealed_foreign_units, BOOL &revealed_unexplored, 
    BOOL &zocViolation, BOOL &is_transported, BOOL &debarker_out_of_fuel)
{
   
    static ArmyList debark;

    debark.Clear(); 

  
     m_all_armies->Access(selected_army).UnloadAllTransportsInArmy(debark, pos, 
         FALSE, did_move, i_died, dead_attacker, revealed_foreign_units, revealed_unexplored, 
        zocViolation, is_transported, debarker_out_of_fuel);

     if ((0 < debark.Num()) && !is_transported)  { 
         CopyArmyIntoPlayer(debark, CAUSE_NEW_ARMY_TRANSPORTED);
     }
}
#endif

sint32 Player::GetUnusedFreight() const 
{ 
  return  m_tradeTransportPoints - m_usedTradeTransportPoints;
}

sint32 Player::GetTotalFreight() const
{ 
  return  m_tradeTransportPoints;
}

TradeRoute Player::CreateTradeRoute(Unit sourceCity, 
									ROUTE_TYPE sourceType, 
									sint32 sourceResource,
									Unit destCity,
									PLAYER_INDEX paying_for,
									sint32 gold_in_return)
{
	if(!g_theUnitPool->IsValid(sourceCity))
		return TradeRoute(0);

	if(!g_theUnitPool->IsValid(destCity))
		return TradeRoute(0);

	Assert(sourceCity.GetOwner() == m_owner);
	if(sourceCity.GetOwner() != m_owner)
		return TradeRoute(0);

	switch(sourceType) {
		case ROUTE_TYPE_RESOURCE:
			if(!sourceCity.HasResource(sourceResource) &&
				sourceCity.CD()->IsLocalResource(sourceResource)) {
				if(sourceCity.AccessData()->GetCityData()->BreakOneSourceRoute(sourceType, sourceResource)) {
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood,
										   GEA_Int, sourceResource,
										   GEA_City, sourceCity,
										   GEA_City, destCity,
										   GEA_End);
				}
				return TradeRoute(0);
			}
			break;
		case ROUTE_TYPE_FOOD:
		case ROUTE_TYPE_GOLD:
			
			
			break;
		default:
			Assert(FALSE);
			return TradeRoute(0);
	}
#ifdef CTP1_TRADE
	
	
	if(g_player[paying_for]->m_usedTradeTransportPoints >= 
	   g_player[paying_for]->m_tradeTransportPoints &&
	   !wonderutil_GetFreeTradeRoutes(g_player[paying_for]->m_builtWonders)) {
		return TradeRoute(0);
	}
#endif

	const GovernmentRecord *grec = g_theGovernmentDB->Get(m_government_type);
	if(sourceCity.GetOutgoingTrade() >= grec->GetMaxOutgoingTrade()) {
		SlicObject *so = new SlicObject("30IATooManyTradeRoutes") ;
		so->AddRecipient(m_owner) ;
        so->AddCity(sourceCity) ;
		g_slicEngine->Execute(so) ;
		return TradeRoute(0);
    }

	if(destCity.GetIncomingTrade() >= grec->GetMaxIncomingTrade()) {
		SlicObject *so = new SlicObject("30IATooManyTradeRoutes") ;
		so->AddRecipient(m_owner) ;
        so->AddCity(destCity) ;
		g_slicEngine->Execute(so) ;
		return TradeRoute(0);
    }

    TradeRoute newRoute = g_theTradePool->Create(sourceCity, destCity, m_owner,
												 sourceType, sourceResource,
												 paying_for,
												 gold_in_return);

	if(newRoute != TradeRoute(0)) {
		return g_player[paying_for]->PayForTrade(newRoute);
	}
	return TradeRoute(0);
}

TradeRoute Player::PayForTrade(TradeRoute &newRoute)
{
	AddUsedTransportPoints((sint32)newRoute.GetCost());

	
	if((m_usedTradeTransportPoints > m_tradeTransportPoints &&
		!wonderutil_GetFreeTradeRoutes(m_builtWonders)) ||
	   !newRoute.GetData()->IsValid()) {
		if(g_network.IsClient()) {
			
			
			
			g_theTradePool->HackSetKey((uint32)newRoute & k_ID_KEY_MASK);
		}
		
		newRoute.KillRoute(CAUSE_KILL_TRADE_ROUTE_NO_INITIAL_CARAVANS);
		return TradeRoute(0);
	} else if(g_network.IsClient()) {
		ROUTE_TYPE type;
		sint32 resource;
		newRoute.GetSourceResource(type, resource);
		g_network.AddCreatedObject(newRoute.AccessData());
		g_network.SendAction(new NetAction(NET_ACTION_CREATE_TRADE_ROUTE,
										   (uint32)newRoute.GetSource(),
										   (uint32)type,
										   (uint32)resource,
										   (uint32)newRoute.GetDestination(),
										   (uint32)newRoute,
							               (uint32)m_owner,
										   (uint32)newRoute.GetGoldInReturn()));
	}
	return newRoute;
}

void Player::AddTransportPoints(sint32 delta)
{
	m_tradeTransportPoints += delta;
	TradeManager::Notify();
}

void Player::RemoveTransportPoints(sint32 delta)
{
	m_tradeTransportPoints -= delta;
	Assert(m_tradeTransportPoints >= 0);
	if(m_tradeTransportPoints < 0) {
		m_tradeTransportPoints = 0;
	}
	if(m_tradeTransportPoints - m_usedTradeTransportPoints <= 0 &&
	   !wonderutil_GetFreeTradeRoutes(m_builtWonders)) {
		g_theTradeBids->CancelBidsFrom(m_owner);
	}

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_PLAYER_TRADE_DATA,
									  m_owner, m_usedTradeTransportPoints, m_tradeTransportPoints));
	}

	TradeManager::Notify();
}

void Player::AddUsedTransportPoints(sint32 delta)
{
	m_usedTradeTransportPoints += delta;
	if(m_tradeTransportPoints - m_usedTradeTransportPoints <= 0 &&
	   !wonderutil_GetFreeTradeRoutes(m_builtWonders)) {
		g_theTradeBids->CancelBidsFrom(m_owner);
	}
	TradeManager::Notify();
}

void Player::RemoveUsedTransportPoints(sint32 delta)
{
	m_usedTradeTransportPoints -= delta;
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_PLAYER_TRADE_DATA,
									  m_owner, m_usedTradeTransportPoints, m_tradeTransportPoints));
	}
	TradeManager::Notify();
}

void Player::RemoveTradeRoute(TradeRoute route, CAUSE_KILL_TRADE_ROUTE cause)
{
	Assert(route.GetPayingFor() == m_owner);
	if(route.GetPayingFor() == m_owner && !route.AccessData()->GetDontAdjustPoints()) {
		double cost = route.GetCost();
		RemoveUsedTransportPoints((sint32)route.GetCost());

		if(cause != CAUSE_KILL_TRADE_ROUTE_NO_INITIAL_CARAVANS) {
			KillATrader();
		}

		
#if 0
		sint32 lastPoints = m_tradeTransportPoints;
		sint32 killed = 0;
		while(killed < cost && m_traderUnits->Num() > 0) {
			KillATrader();
			Assert(lastPoints > m_tradeTransportPoints);
			if(lastPoints <= m_tradeTransportPoints) {
				break;
			}
			killed += lastPoints - m_tradeTransportPoints;
			lastPoints = m_tradeTransportPoints;
		}
#endif
	}
}

void Player::CancelTradeRoute(TradeRoute route)
{
	if(g_theUnitPool->IsValid(route.GetSource()) &&
		route.GetSource().GetOwner() == m_owner) {
		route.KillRoute(CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED);
	} else {
		route.KillRoute(CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED);
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_CANCEL_TRADE_ROUTE,
										(uint32)route));
	}
}

void Player::InterceptTrade(sint32 army_index)
{
  	MapPoint pnt;

    Assert(0 < m_all_armies->Get(army_index).Num()); 
	Unit u = m_all_armies->Get(army_index).GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
    if (u.m_id == (0)) {  
        u = m_all_armies->Get(army_index).Get(0); 
    }

	u.GetPos(pnt);
	u.InterceptTrade();

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_INTERCEPT_TRADE,
										   army_index));
	}
}

void Player::KillATrader()
{
	if(wonderutil_GetFreeTradeRoutes(m_builtWonders))
		return;

	Assert(m_traderUnits->Num() > 0);
	if(m_traderUnits->Num() > 0) {
		m_traderUnits->Access(0).Kill(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
	}
}

TradeOffer Player::CreateTradeOffer(Unit fromCity,
									ROUTE_TYPE offerType, sint32 offerResource,
									ROUTE_TYPE askingType, sint32 askingResource,
									Unit toCity)
{
	TradeOffer offer = g_theTradeOfferPool->Create(fromCity, 
												   offerType, offerResource,
												   askingType, askingResource,
												   toCity);
	if(offer != TradeOffer(0)) {
		g_slicEngine->RunTradeOfferTriggers(offer);

		if(offerType == ROUTE_TYPE_RESOURCE && askingType == ROUTE_TYPE_GOLD) {
			SetLastSalePrice(offerResource, askingResource);
		}

		if(g_network.IsClient()) {
			g_network.AddCreatedObject(offer.AccessData());
			g_network.SendAction(new NetAction(NET_ACTION_CREATE_TRADE_OFFER,
											   (uint32)fromCity,
											   offerType, offerResource,
											   askingType, askingResource,
											   (uint32)toCity,
											   (uint32)offer));
		} else if(g_network.IsHost()) {
			g_network.Enqueue(offer.AccessData());
		}
	}
	return offer;
}

void Player::AddTradeOffer(TradeOffer offer)
{
	Assert(offer.GetFromCity().GetOwner() == m_owner);
	Assert(offer.GetFromCity().GetData()->GetCityData() != NULL);
	if(offer.GetOfferType() == ROUTE_TYPE_RESOURCE) {

		
		
		

		
		
	}

	m_tradeOffers->Insert(offer);
}

void Player::RemoveTradeOffer(TradeOffer offer)
{
	m_tradeOffers->Del(offer);
}

void Player::WithdrawTradeOffer(TradeOffer offer)
{
	if(!g_theTradeOfferPool->IsValid(offer)) {
		
		
		return;
	}

	Assert(offer.GetFromCity().GetOwner() == m_owner);
	offer.KillOffer();
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_WITHDRAW_TRADE_OFFER,
										   (uint32)offer));
	}
}

void Player::AcceptTradeOffer(TradeOffer offer, Unit &sourceCity, Unit &destCity)
{
	
	
	

	sint32 sender = offer.GetFromCity().GetOwner();
	if(!g_player[sender])
		return;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_TAKE_TRADE_OFFER,
										   (uint32)offer, 
										   (uint32)sourceCity,
										   (uint32)destCity));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_TAKE_TRADE_OFFER,
									  m_owner,
									  (uint32)offer,
									  (uint32)sourceCity,
									  (uint32)destCity));
		g_network.Unblock(m_owner);
	}

	if(g_player[sender]->GetPlayerType() == PLAYER_TYPE_ROBOT) {
		
		
	} else {
		SlicObject *so;
		if(offer.GetOfferType() == ROUTE_TYPE_RESOURCE) {
			so = new SlicObject("90AcceptTradeOffer");
			so->AddGood(offer.GetOfferResource());
			so->AddGood(offer.GetAskingResource());
			so->AddGold(offer.GetAskingResource());
		} else if(offer.GetOfferType() == ROUTE_TYPE_SLAVE) {
			so = new SlicObject("90bAcceptSlaveTradeOffer");
			so->AddGold(offer.GetAskingResource());
		}
		so->AddRecipient(offer.GetFromCity().GetOwner());
		so->AddTradeOffer(offer);
		so->AddCity(offer.GetFromCity());
		so->AddCity(offer.GetToCity());
		so->AddCity(sourceCity);
		so->AddCity(destCity);
		so->AddCivilisation(m_owner);
		
		g_slicEngine->Execute(so);
	}

#if 0
	if(offer.Accept(m_owner, sourceCity, destCity)) {
		SlicObject *so = new SlicObject("22OfferAccepted") ;
		so->AddRecipient(sourceCity.GetOwner()) ;
		so->AddGood(offer.GetOfferResource()) ;
		so->AddCity(sourceCity) ;
		so->AddCity(destCity) ;
		so->AddCivilisation(destCity.GetOwner()) ;
		g_slicEngine->Execute(so) ;
		offer.KillOffer();
	}
#endif
}
	
void Player::CreateTradeBid(Unit &fromCity, sint32 resource, Unit &toCity)
{
	Assert(fromCity.GetOwner() != m_owner);
	if(fromCity.GetOwner() == m_owner)
		return;
	Assert(toCity.GetOwner() == m_owner);
	if(toCity.GetOwner() != m_owner)
		return;

	if(m_usedTradeTransportPoints >= m_tradeTransportPoints &&
	   !wonderutil_GetFreeTradeRoutes(m_builtWonders)) {
		SlicObject *so = new SlicObject("NoTraders");
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
		return;
	}











	
	c3dialogs_PostForeignTradeBidDialog(m_owner, fromCity, toCity, resource);

}

void Player::SendTradeBid(Unit &fromCity, sint32 resource, Unit &toCity,
						  sint32 price)
{
	Assert(fromCity.GetOwner() != m_owner);
	if(fromCity.GetOwner() == m_owner)
		return;
	Assert(toCity.GetOwner() == m_owner);
	if(toCity.GetOwner() != m_owner)
		return;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_SEND_TRADE_BID,
										   m_owner,
										   fromCity, resource, toCity,
										   price));
	} else if(g_network.IsHost()) {
		g_network.Block(toCity.GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SEND_TRADE_BID,
									  m_owner,
									  fromCity, resource, toCity, price));
		g_network.Unblock(toCity.GetOwner());
	}

	uint32 bidId = g_theTradeBids->AddBid(m_owner,
										  fromCity,
										  resource,
										  toCity,
										  price);
	
	
	
	
	
	
	
	
	

	SlicObject *so = new SlicObject("TradeBid");
	so->AddRecipient(fromCity.GetOwner());
	so->AddCivilisation(m_owner);
	so->AddGood(resource);
	so->AddCity(fromCity);
	so->AddCity(toCity);
	so->AddGold(price);
	so->AddTradeBid(bidId);
	g_slicEngine->Execute(so);

}

void Player::AcceptTradeBid(Unit &fromCity, sint32 resource, Unit &toCity,
							sint32 price)
{
	SlicObject *so = new SlicObject("TradeBidAccepted");
	so->AddRecipient(toCity.GetOwner());
	so->AddCivilisation(m_owner);
	so->AddGood(resource);
	so->AddCity(fromCity);
	so->AddCity(toCity);
	so->AddGold(price);
	g_slicEngine->Execute(so);

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_ACCEPT_TRADE_BID,
										   m_owner,
										   fromCity, resource, toCity,
										   price));
	}
	TradeRoute route = CreateTradeRoute(fromCity, 
					 ROUTE_TYPE_RESOURCE, resource, 
					 toCity, 
					 toCity.GetOwner(), 
					 price); 
}

void Player::RejectTradeBid(Unit &fromCity, sint32 resource, Unit &toCity,
							sint32 price)
{
	SlicObject *so = new SlicObject("TradeBidRejected");
	so->AddRecipient(toCity.GetOwner());
	so->AddCivilisation(m_owner);
	so->AddGood(resource);
	so->AddCity(fromCity);
	so->AddCity(toCity);
	so->AddGold(price);
	g_slicEngine->Execute(so);

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_REJECT_TRADE_BID,
										   m_owner,
										   fromCity, resource, toCity,
										   price));
	}
}

void Player::AddTrader(Unit uid)
{
	m_traderUnits->Insert(uid);
	AddTransportPoints((sint32)g_theUnitDB->Get(uid.GetType())->GetMaxMovePoints());
}

void Player::GamestateDebug()
{ 
    
    
    int i, n; 
    
    n = m_all_units->Num(); 
    for (i=0; i<n; i++) { 
        m_all_units->Access(i).GamestateDebug();
    }
    n = m_all_cities->Num(); 
    for (i=0; i<n; i++) { 
        m_all_cities->Access(i).GamestateDebug();
    }
}

void Player::BuildUnit(sint32 type, Unit &city)
{
	
	
	
	Assert(city.GetOwner() == m_owner);
	if(city.GetOwner() != m_owner)
		return;

	Assert(!g_theUnitDB->Get(type)->GetCantBuild());
	if(g_theUnitDB->Get(type)->GetCantBuild())
		return;

#if 0
	
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD, type, city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILDING_UNIT, 
									  type, (uint32)city));
		g_network.Unblock(m_owner);
	}
#endif
	city.BuildUnit(type);
}

void Player::BuildImprovement(sint32 type, Unit &city)
{
	
	
	
	Assert(city.GetOwner() == m_owner);
	if(city.GetOwner() != m_owner)
		return;

#if 0
	
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_IMP, type, city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_IMP, m_owner, type, 
									  city));
		g_network.Unblock(m_owner);
	}
#endif
	city.BuildImprovement(type);
}

BOOL Player::ChangeCurrentlyBuildingItem(Unit &city, sint32 category, sint32 item_type)

{
	Assert(city.GetOwner() == m_owner);

   
	return city.ChangeCurrentlyBuildingItem(category, item_type);
}

void Player::SetTaxes(double s)
{
    DPRINTF( k_DBG_GAMESTATE, ("Player::SetTaxes(%lf)\n", s));
    
    Assert(0.0 <= s); 
    Assert(s <= 1.0);
	
	if(s > g_theGovernmentDB->Get(m_government_type)->GetMaxScienceRate())
		s = g_theGovernmentDB->Get(m_government_type)->GetMaxScienceRate();

#if 0
    if(g_network.IsActive() && g_network.IsClient()) {
        g_network.SendAction(new NetAction(NET_ACTION_TAX_RATES, 
            (sint32)(s * 100000.),
            0,
            0));
    }
#endif
	m_tax_rate->SetTaxRates(s, m_owner);
#if 0
	if(g_network.IsActive() && g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_TAX_RATE,
									  m_owner, 
									  (sint32)(s * 100000.),
                                      0, 0));
	}
#endif
}

void Player::SetMaterialsTax(double m)
{
	Assert(0 <= m);
	Assert(m <= 1);
	if(m < 0 || m > 1)
		return;

	sint32 rounded = sint32(m * 100.0);

	m_materialsTax = double(rounded) / 100.0;
	
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_MATERIALS_TAX,
									  m_owner, (sint32)((m_materialsTax + 0.001) * 100.)));
		g_network.Unblock(m_owner);
	} else if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_SET_MATERIALS_TAX,
										   (uint32)((m_materialsTax + 0.001) * 100.)));
	}

	g_slicEngine->RunPublicWorksTaxTriggers(m_owner);
}


sint32 Player::GetCurrentScienceCost()
{ return m_advances->GetCost(); 
} 


sint32 Player::GetCurrentScienceLevel()
{ 
    return m_science->GetLevel(); 
} 


void Player::AddPopScience(const sint32 d)
{
	m_pop_science += d;
}














void Player::AddScience(const sint32 delta)
{
    sint32 wonderAdvanceChance;
    sint32 gotadvance;
    BOOL gotRandomAdvance;

	m_science->AddScience(delta);
	m_gold->SetScience(delta);

    
    

    
    wonderAdvanceChance = wonderutil_GetRandomAdvanceChance(m_builtWonders);
    gotRandomAdvance = ((wonderAdvanceChance > 0) && 
                        (g_rand->Next(100) < wonderAdvanceChance));

	DPRINTF(k_DBG_GAMESTATE, ("Advance cost: %d, have: %d\n", m_advances->GetCost(), m_science->GetLevel()));

    
    if (gotRandomAdvance ||
        (m_science->GetLevel() >= m_advances->GetCost())) {
		m_science->SetLevel(m_science->GetLevel() - m_advances->GetCost());
		if(m_science->GetLevel() < 0) {
			m_science->SetLevel(0);
		}
        gotadvance = m_advances->GetResearching();
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_GrantAdvance,
							   GEA_Player, m_owner,
							   GEA_Int, gotadvance,
							   GEA_Int, CAUSE_SCI_RESEARCH,
							   GEA_End);





		
    }

    
    if (gotRandomAdvance) {
        SlicObject *so = new SlicObject("115EdisonDiscovery");
        so->AddRecipient(m_owner);
        so->AddAdvance(gotadvance);
        g_slicEngine->Execute(so);
    }        
}

void Player::SpecialDiscoveryNotices(AdvanceType advance)
	{
#if 0
	SlicObject	*so ;
	
	
	if (g_theAdvanceDB->GetSpecialDiscovery(m_advances->GetResearching()))
		{
		so = new SlicObject("29SpecialDiscovery") ;
		so->AddRecipient(GetOwner()) ;
		so->AddAdvance(m_advances->GetResearching()) ;
		so->AddAction(g_theAdvanceDB->GetDiscoveryHoopla(m_advances->GetResearching())) ;
		g_slicEngine->Execute(so) ;
		
		so = new SlicObject("30SpecialDiscovery") ;
		so->AddAdvance(m_advances->GetResearching()) ;
		so->AddAction(g_theAdvanceDB->GetDiscoveryHoopla(m_advances->GetResearching())) ;
		for(i = 1; i < k_MAX_PLAYERS; i++)
			if ((g_player[i]) && (i != GetOwner()))
				so->AddRecipient(i) ;
		
		g_slicEngine->Execute(so) ;
		
		}
#endif

	g_slicEngine->RunDiscoveryTriggers(advance, m_owner);
	}


void Player::GovernmentDiscoveryNotices(AdvanceType advance)
{
	SlicObject	*so ;
	
	sint32	i,
			numGovs ;

	numGovs = g_theGovernmentDB->NumRecords() ;
	for (i=0; i<numGovs; i++) {
	    if (g_theGovernmentDB->Get(i)->GetEnableAdvanceIndex()==advance) {
			so = new SlicObject("57PlayerDiscoversNewGovernment") ;
			so->AddRecipient(GetOwner()) ;
			so->AddGovernment(i);
			g_slicEngine->Execute(so) ;
			break ;
		}
	}
}

void Player::ObsoleteNotices(AdvanceType advance)
{
    SlicObject  *so;
    const UnitRecord *rec;
    sint32  i, j;
    sint32 num_found=0;

    
    
    so = new SlicObject("114UnitObsoleteCivwide");
    for (i=0; i < g_theUnitDB->NumRecords(); i++) {
        rec = g_theUnitDB->Get(i);
        for(j = 0; j < rec->GetNumObsoleteAdvance(); j++) {
            if (rec->GetObsoleteAdvanceIndex(j) == advance) {
                so->AddAction(g_theStringDB->GetNameStr(rec->m_name));
                num_found++;
                break;
            }
        }
    }

    
    if (num_found == 0) {
        delete so;
        return;
    }

    so->AddUnit(0);
    so->AddAdvance(advance);
    so->AddRecipient(m_owner);
    g_slicEngine->Execute(so);
}

void Player::BuildResearchDialog(AdvanceType advance)
{
	SlicContext sc;
	const MBCHAR *dstring;
	MBCHAR text[1024];

    
	if(m_playerType == PLAYER_TYPE_ROBOT)
		return;
  
	if (g_network.IsActive() && !(g_network.IsLocalPlayer(m_owner)))
		return;

	if(m_disableChooseResearch)
		return;

  dstring = g_theStringDB->GetNameStr("PICK_NEW_DISCOVERY");
  
	if (dstring) {
    sc.AddAdvance(advance);

		MBCHAR	tempStr[1024], 
				messageStr[1024]; 
		
		
		strcpy(messageStr, dstring);

		
		sprintf(tempStr, "%#.3d", (sint32)advance);

		
		MBCHAR *p = strstr(messageStr, "000>");
		if(p) {
			
			for (sint32 i=0; i<3; i++) {
				p[i] = tempStr[i];
			}
		}

		stringutils_Interpret(messageStr, sc, text);

		
		if((g_turn->IsHotSeat() || g_turn->IsEmail())
				// ... and replaced it with this, which makes much more sense,
				// but doesn't work because BlankScreen() is called too late
				//&& (g_slicEngine->ShouldScreenBeBlank())
				// So, I've left it without a further condition,
				// which means the window (probably) won't appear in PBEM/hotseat
				// games when the advance currently
				// being researched is obtained via other means (e.g. ruins).
				// I have been unable to test this as yet.
				// 2005/01/02
				// (FIXME)
				) {
			g_slicEngine->AddResearchOnUnblank(m_owner, text);
		} else {
			g_director->AddInvokeResearchAdvance(text);
		}
	} else {
		g_director->AddInvokeResearchAdvance(NULL);
	}
}













void Player::StartResearching(sint32 adv)
    {
	SetResearching(adv);
#if 0
	m_advances->SetResearching(adv);						
	if(g_network.IsClient() && m_owner == g_network.GetPlayerIndex())
		{
		g_network.SendAction(new NetAction(NET_ACTION_RESEARCH, adv));
		}
	else if(g_network.IsHost())
		{
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_RESEARCH, m_owner, adv));
		}
#endif
	}


void Player::SetResearching(AdvanceType advance)
{
	uint8 * canResearch = m_advances->CanResearch();

	
	
	if(!g_network.IsActive() || g_network.IsHost() || g_network.IsLocalPlayer(m_owner)) {
		if(m_playerType == PLAYER_TYPE_HUMAN || m_playerType == PLAYER_TYPE_NETWORK ||
			(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))) {
			
			
			
			if(m_playerType != PLAYER_TYPE_ROBOT) {
				Assert(canResearch[advance]);
			}
			if(!canResearch[advance]) {
				delete [] canResearch;
				return;
			}
		}
	}
	m_advances->SetResearching(advance);
	if(g_network.IsClient() && m_owner == g_network.GetPlayerIndex()) {
		g_network.SendAction(new NetAction(NET_ACTION_RESEARCH, advance));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_RESEARCH, m_owner, advance));
		g_network.Unblock(m_owner);
	}
	delete [] canResearch;
}

void Player::AddUnitVision(const MapPoint &pnt, double range,
						   BOOL &revealed_unexplored)
{
	m_vision->AddVisible(pnt, range, revealed_unexplored);
}

void Player::RemoveUnitVision(const MapPoint &pnt, double range)
{
	m_vision->RemoveVisible(pnt, range);
}

void Player::OwnExploredArea()
{
	sint32 x,y;
	for(x = 0; x < g_theWorld->GetXWidth(); x++) {
		for(y = 0; y < g_theWorld->GetYHeight(); y++) {
			if(IsExplored(x,y)) {
				g_theWorld->GetCell(x,y)->SetOwner((sint8)m_owner);
			}
		}
	}
}

BOOL Player::IsExplored(const MapPoint &pos) const 
{
	return m_vision->IsExplored(pos);
}

BOOL Player::IsVisible(const MapPoint &pos) const 
{
	return m_vision->IsVisible(pos);
}

BOOL Player::IsExplored(sint32 x, sint32 y) const 
{
	MapPoint pnt(x,y);
	return m_vision->IsExplored(pnt);
}

BOOL Player::IsVisible(sint32 x, sint32 y) const 
{
	MapPoint pnt(x,y);
	return m_vision->IsVisible(pnt);
}

BOOL Player::GetLastSeen(const MapPoint &point, UnseenCellCarton &ucell)
{ 
	return m_vision->GetLastSeen(point, ucell);
}












uint32 Player::GetAverageEventPollution(void)
	{
	sint32	i ;

	uint32	average ;

	average = 0 ;
	for (i=0; i<g_theConstDB->AveragePollutionTurns(); i++)
		average += m_event_pollution[i] ;

	average /= g_theConstDB->AveragePollutionTurns() ;

	return (average) ;
	}













void Player::AttemptRevolt(void)
{
	BOOL	*revolution ;											

	sint32	i, j,
			cityNum,
			inciteBonus = 0 ;

	Unit	u ;

	CityData	*cityData ;
		
	MapPoint	cityPos,
				neighbourPos ;

	m_num_revolted = 0;		
	cityNum = m_all_cities->Num() ;

    if (cityNum < 1) 
        return; 

	revolution = new BOOL[cityNum] ;
	memset(revolution, FALSE, sizeof(BOOL) * cityNum) ;

	
	for (i=0; i<cityNum; i++)
		{
		u = m_all_cities->Get(i) ;
		cityData = u.GetData()->GetCityData() ;
		u.GetPos(cityPos) ;
		if (!revolution[i])											
			{
			if (cityData->ShouldRevolt(0))							
				{
				revolution[i] = TRUE ;

				
				for (j=0; j<cityNum; j++)
					{
					if (!revolution[j])
						{
						u = m_all_cities->Get(j) ;
						u.GetPos(neighbourPos) ;
						cityData = u.GetData()->GetCityData() ;
						inciteBonus = g_theConstDB->GetRevoltInfluenceDistance() - MAX(abs(cityPos.x - neighbourPos.x), abs(cityPos.y - neighbourPos.y)) ;
						if (inciteBonus >= 0)						
							{
							if (cityData->ShouldRevolt(inciteBonus))
								revolution[i] = TRUE ;

							}

						}

					}

				}

			}

		}

	for (i=cityNum - 1; i>= 0; i--)
	{
		// Modified by kaan to address bug # 12
		u = m_all_cities->Get(i);
		cityData = u.GetData()->GetCityData();

		if (revolution[i])
		{
			cityData->Revolt(m_civRevoltingCitiesShouldJoin);
			CtpAi::AddOwnerGoalsForCity(u, u.GetOwner());
			CtpAi::AddForeignerGoalsForCity(u, m_owner);
			m_num_revolted++;
		}
		else 
		{
			cityData->NoRevoltCountdown();
		}
	}

	delete [] revolution ;

	
	
	
	
	for(i = m_all_cities->Num() - 1; i >= 0; i--) {
		cityData = m_all_cities->Get(i).GetData()->GetCityData();
		if(cityData->NeedToDoUprising()) {
			cityData->DoUprising(UPRISING_CAUSE_INTERNAL);
		}
	}
}













void Player::Revolt(const sint32 idx)
	{
	Unit city = m_all_cities->Get(idx) ;




	CityData	*cityData = city.AccessData()->GetCityData() ;
	cityData->GetHappy()->ForceRevolt() ;
	AttemptRevolt() ;
	}

	












void Player::GiveAdvance(PLAYER_INDEX recipient, AdvanceType adv, CAUSE_SCI cause)
	{
    if (HasAdvance(adv)) { 
		g_player[recipient]->m_advances->GiveAdvance(adv, cause) ;
    } else { 
        Assert(0); 
        return; 
    } 
	}














void Player::GiveUnit(const PLAYER_INDEX other_player, const sint32 unit_idx)
	{
    UnitDynamicArray    revealed;
	MapPoint	p ;
    BOOL revealed_unexplored; 
	Unit	u = m_all_units->Get(unit_idx).m_id ;

	GetCapitolPos(p) ;
	u.ResetUnitOwner(other_player, CAUSE_REMOVE_ARMY_DIPLOMACY) ;
	u.SetPosition(p, revealed, revealed_unexplored) ;
	}













void Player::StopTradingWith(PLAYER_INDEX bannedRecipient)
	{
	sint32	c,
			cityNum ;

	CityData	*cityData ;

	cityNum = m_all_cities->Num() ;
	for (c=0; c<cityNum; c++)
		{
		cityData = m_all_cities->Get(c).GetData()->GetCityData() ;
		cityData->StopTradingWith(bannedRecipient) ;
		}

	}

	











void Player::FormAlliance(PLAYER_INDEX ally)
	{
	g_player[ally]->SetAlliance(m_owner) ;
	g_player[m_owner]->SetAlliance(ally) ;
	}












void Player::SetAlliance(PLAYER_INDEX ally)
	{
	Assert(ally!=m_owner) ;
	if (ally == m_owner)											
		return ;

	mask_alliance |= (0x01<<ally) ;									

    
    SetDiplomaticState(ally, DIPLOMATIC_STATE_ALLIED);
	
	g_player[ally]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_ALLIED);
}












void Player::ClearAlliance(PLAYER_INDEX ally)
	{
	Assert(ally!=m_owner) ;
	if (ally == m_owner)											
		return ;

	mask_alliance &= ~(0x01<<ally) ;								

    SetDiplomaticState(ally, DIPLOMATIC_STATE_NEUTRAL);
	
	g_player[ally]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_NEUTRAL);
}













void Player::BreakAlliance(PLAYER_INDEX ally)
{
	if(!(mask_alliance & (1 << ally)) &&
	   !(g_player[ally]->mask_alliance & (1 << m_owner))) {
		
		return;
	}

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BREAK_ALLIANCE,
										   m_owner, ally));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BREAK_ALLIANCE,
									  m_owner, ally));
		g_network.Unblock(m_owner);
	}

	g_player[ally]->ClearAlliance(m_owner) ;
	g_player[m_owner]->ClearAlliance(ally) ;
	m_broken_alliances_and_cease_fires++;

	SlicObject *so = new SlicObject("106YouBrokeAlliance");
	so->AddRecipient(m_owner);
	so->AddCivilisation(ally);
	g_slicEngine->Execute(so);

	so = new SlicObject("107AllianceBroken");
	so->AddRecipient(ally);
	so->AddCivilisation(m_owner);
	g_slicEngine->Execute(so);



}













void Player::ExchangeMap(PLAYER_INDEX recipient)
{
	GiveMap(recipient) ;											
	g_player[recipient]->GiveMap(m_owner) ;							
	
	
	
	}












void Player::GiveMap(PLAYER_INDEX recipient)
	{
	g_player[recipient]->m_vision->MergeMap(m_vision) ;
	if(g_selected_item->GetVisiblePlayer() == recipient) {
		g_director->AddCopyVision();
	}
	}











BOOL Player::IsPollutionReduced(void)
	{

#define k_DIPLOMATIC_REDUCE_POLLUTION_THRESHOLD		10

	if (GetPollutionLevel() > k_DIPLOMATIC_REDUCE_POLLUTION_THRESHOLD)
		{
		DPRINTF(k_DBG_INFO, ("Player %d pollution is above threshold\n", m_owner)) ;

		return (FALSE) ;
		}

	return (TRUE) ;
	}












BOOL Player::IsViolatingPeace(PLAYER_INDEX player)
	{
	if (mask_alliance & (0x01 << player))
		{
		DPRINTF(k_DBG_INFO, ("Player %d is violating Player %d\n", m_owner, player)) ;

		return (TRUE) ;
		}

	DPRINTF(k_DBG_INFO, ("Player %d is not violating Player %d\n", m_owner, player)) ;

	return (FALSE) ;
	}













Agreement Player::FindAgreement(const AGREEMENT_TYPE agreement, const PLAYER_INDEX otherParty) const
	{
	sint32	i,
			agreedNum ;

	agreedNum = m_agreed->Num() ;
	for (i=0; i<agreedNum; i++)
		{
		if ((m_agreed->Get(i).GetAgreement() == agreement))
			if ((m_agreed->Get(i).GetRecipient() == otherParty) || (m_agreed->Get(i).GetOwner() == otherParty))
				return (m_agreed->Get(i)) ;

		}

	return (Agreement(0)) ;
	}













Agreement Player::FindAgreement(const AGREEMENT_TYPE agreement) const
	{
	sint32	i,
			agreedNum ;

	agreedNum = m_agreed->Num() ;
	for (i=0; i<agreedNum; i++)
		{
		if ((m_agreed->Get(i).GetAgreement() == agreement))
			return (m_agreed->Get(i)) ;

		}

	return (Agreement(0)) ;
	}

	











Agreement Player::FindAgreement(const PLAYER_INDEX otherParty) const
	{
	sint32	i,
			agreedNum ;

	agreedNum = m_agreed->Num() ;
	for (i=0; i<agreedNum; i++)
		{
		if ((m_agreed->Get(i).GetRecipient() == otherParty) || (m_agreed->Get(i).GetOwner() == otherParty))
			return (m_agreed->Get(i)) ;

		}

	return (Agreement(0)) ;
	}

	














BOOL Player::FulfillCaptureCityAgreement(Unit city)
	{

return TRUE; 
	AgreementDynamicArray	killList ;

	sint32	i,
			agreedNum ;
	
	Agreement	agree ;

	SlicObject *so ;
	
	agreedNum = m_agreed->Num() ;
	for (i=0; i<agreedNum; i++)
		{
		agree = m_agreed->Get(i) ;
		if ((agree.GetAgreement() == AGREEMENT_TYPE_PACT_CAPTURE_CITY) && (agree.GetTarget() == city))
			{
			
			so = new SlicObject("85PactFulfilledCityCapturedFirstParty") ;
			so->AddRecipient(m_owner) ;
			so->AddCity(city) ;
			so->AddCivilisation(m_owner) ;
			if (m_owner != agree.GetOwner())
				so->AddCivilisation(agree.GetOwner()) ;
			else
				so->AddCivilisation(agree.GetRecipient()) ;

			g_slicEngine->Execute(so) ;

			so = new SlicObject("86PactFulfilledCityCapturedSecondParty") ;
			if (m_owner != agree.GetOwner())
				so->AddRecipient(agree.GetOwner()) ;
			else
				so->AddRecipient(agree.GetRecipient()) ;

			so->AddRecipient(agree.GetRecipient()) ;
			so->AddCity(city) ;
			so->AddCivilisation(m_owner) ;
			if (m_owner != agree.GetOwner())
				so->AddCivilisation(agree.GetOwner()) ;
			else
				so->AddCivilisation(agree.GetRecipient()) ;

			g_slicEngine->Execute(so) ;
			killList.Insert(agree) ;								
			}
		else
			{
			
			Assert(FALSE) ;
			}







		}

	if (killList.Num())
		{
	    killList.KillList() ;		

		return (TRUE) ;												
		}

	return (FALSE) ;												
	}




















































































Agreement Player::MakeEndPollutionPact(PLAYER_INDEX player)
	{
	Agreement a = g_theAgreementPool->Create(m_owner, player, AGREEMENT_TYPE_PACT_END_POLLUTION) ;
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;

	a.SetExpires(g_theConstDB->GetPactEndPollutionExpires()) ;
	DPRINTF(k_DBG_INFO, ("Player #%d agrees with Player #%d to end pollution as part of a pact\n", m_owner, player)) ;

	return (a) ;
	}














Agreement Player::MakeLeaveOurLands(PLAYER_INDEX player)
{
	Agreement a = g_theAgreementPool->Create(m_owner, player, AGREEMENT_TYPE_DEMAND_LEAVE_OUR_LANDS) ;	
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;


    
    
    

	a.SetExpires(g_theConstDB->GetLeaveOurLandsExpires()) ;
	DPRINTF(k_DBG_INFO, ("Player #%d agrees to leave the lands of Player #%d\n", player, m_owner)) ;

	if(g_player[player]) {
		DynamicArray<Army> *armies = g_player[player]->m_all_armies;
		sint32 i, n = armies->Num();
        sint32 num_moved = 0;
		bool atLeastOneCouldntBeExpelled = false;
		for(i = 0; i < n; i++) {
			MapPoint pos;
			armies->Access(i).GetPos(pos);
			if(g_theWorld->GetCell(pos)->GetOwner() == m_owner) {
				MapPoint cpos;
				BOOL foundCity = 
					armies->Access(i)[0].NearestFriendlyCityWithRoom(cpos, armies->Access(i).Num(), &armies->Access(i));
				if(foundCity) {
					armies->Access(i).AutoAddOrdersWrongTurn(UNIT_ORDER_EXPEL_TO, NULL, cpos, 0);
                    num_moved++;
				} else {
					atLeastOneCouldntBeExpelled = true;
				}
			}
		}

        if (num_moved > 0 || atLeastOneCouldntBeExpelled) {
			SlicObject *so;
			if(atLeastOneCouldntBeExpelled) {
				so = new SlicObject("40IALeftLands") ;
			} else {
				so = new SlicObject("40IALeftLandsButNotAll");
			}
            so->AddRecipient(player) ;
            g_slicEngine->Execute(so) ;
        }

		if(atLeastOneCouldntBeExpelled) {
			SlicObject *so = new SlicObject("364AtLeastOneArmyCouldntBeExpelled");
			so->AddRecipient(m_owner);
			so->AddCivilisation(player);
			g_slicEngine->Execute(so);
		}
	}

	return (a) ;
}














Agreement Player::MakeReducePollution(PLAYER_INDEX player)
	{
	Agreement a = g_theAgreementPool->Create(m_owner, player, AGREEMENT_TYPE_REDUCE_POLLUTION) ;	
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;

	a.SetExpires(g_theConstDB->GetReducePollutionExpires()) ;
	DPRINTF(k_DBG_INFO, ("Player #%d agrees to leave the lands of Player #%d\n", player, m_owner)) ;

	return (a) ;
	}














Agreement Player::MakeCaptureCityPact(PLAYER_INDEX player, Unit &city)
	{
	Agreement a = g_theAgreementPool->Create(m_owner, player, AGREEMENT_TYPE_PACT_CAPTURE_CITY) ;
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;

	a.SetExpires(g_theConstDB->GetPactCaptureCityExpires()) ;
	a.SetTarget(city) ;
	DPRINTF(k_DBG_INFO, ("Player #%d agrees with Player #%d to capture city id %d as part of a pact\n", m_owner, player, city)) ;

	return (a) ;
	}






































































































































































	





























	












BOOL Player::WillViolatePact(PLAYER_INDEX otherParty)
	{
	sint32	i,
			agreedNum ;

	agreedNum = m_agreed->Num() ;
	for (i=0; i<agreedNum; i++)
		{
		Agreement	a = m_agreed->Get(i) ;
		AGREEMENT_TYPE	pact = a.GetAgreement() ;
		if ((pact == AGREEMENT_TYPE_PACT_CAPTURE_CITY) || (pact == AGREEMENT_TYPE_PACT_END_POLLUTION))
			if ((m_agreed->Get(i).GetRecipient() == otherParty) || (m_agreed->Get(i).GetOwner() == otherParty))
				{
				DPRINTF(k_DBG_INFO, ("Player #%d will violate pact with Player #%d\n", m_owner, otherParty)) ;

				return (TRUE) ;
				}

		}

	DPRINTF(k_DBG_INFO, ("Player %d has no pacts with Player %d\n", m_owner, otherParty)) ;

	return (FALSE) ;
	}














BOOL Player::WillViolateCeaseFire(PLAYER_INDEX other_player)
	{
	if (FindAgreement(AGREEMENT_TYPE_CEASE_FIRE, other_player) == Agreement(0))
		{
		DPRINTF(k_DBG_INFO, ("Player #%d does not have a cease fire agreement with Player #%d\n", m_owner, other_player)) ;

		return (FALSE) ;
		}

	DPRINTF(k_DBG_INFO, ("Player %d is violating cease fire agreement of Player %d\n", m_owner, other_player)) ;

	return (TRUE) ;
	}

BOOL Player::HaveNoPiracyAgreement(PLAYER_INDEX other_player)
{
	return FindAgreement(AGREEMENT_TYPE_NO_PIRACY, other_player) != Agreement(0);
}













void Player::MakeNoPiracyPact(PLAYER_INDEX other_player)
	{
	
	if (HaveNoPiracyAgreement(other_player))
		{
		DPRINTF(k_DBG_INFO, ("Player #%d already has a \"No Piracy\" agreement with Player #%d\n", m_owner, other_player)) ;

		return ;													
		}

	
	Agreement a = g_theAgreementPool->Create(m_owner, other_player, AGREEMENT_TYPE_NO_PIRACY) ; 
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;

	a.SetExpires(g_theConstDB->GetNoPiracyExpires()) ;
	DPRINTF(k_DBG_INFO, ("Player #%d establishes a \"No Piracy\" agreement with Player #%d\n", m_owner, other_player)) ;
	}

	












void Player::MakeShortCeaseFire(PLAYER_INDEX other_player, AGREEMENT_TYPE agreement,
								PLAYER_INDEX third_party)
	{
	







	
	Agreement a = g_theAgreementPool->Create(m_owner, other_player, agreement) ; 
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;

	if(agreement == AGREEMENT_TYPE_DEMAND_STOP_TRADE) {
		a.SetThirdParty(third_party);
		if(g_network.IsHost()) {
			g_network.Enqueue(a.AccessData());
		}
	}

	a.SetExpires(g_theConstDB->GetShortCeaseFireExpires()) ;
	DPRINTF(k_DBG_INFO, ("Player #%d establishes a \"Cease Fire\" with Player #%d\n", m_owner, other_player)) ;
	
	if(GetDiplomaticState(other_player) != DIPLOMATIC_STATE_ALLIED)
		
		SetDiplomaticState(other_player, DIPLOMATIC_STATE_CEASEFIRE);
	
	Assert(g_player[other_player]);
	if (g_player[other_player] && g_player[other_player]->GetDiplomaticState(m_owner) != DIPLOMATIC_STATE_ALLIED) {
		
		g_player[other_player]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_CEASEFIRE);
	}		
}














void Player::MakeCeaseFire(PLAYER_INDEX other_player)
	{
	
	if (FindAgreement(AGREEMENT_TYPE_CEASE_FIRE, other_player) != Agreement(0))
		{
		DPRINTF(k_DBG_INFO, ("Player #%d already has a \"Cease Fire\" agreement with Player #%d\n", m_owner, other_player)) ;

		return ;													
		}

	
	Agreement a = g_theAgreementPool->Create(m_owner, other_player, AGREEMENT_TYPE_CEASE_FIRE) ; 
	if (a==Agreement(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_AGREEMENT_ID") ;

	a.SetExpires(g_theConstDB->GetCeaseFireExpires()) ;
	DPRINTF(k_DBG_INFO, ("Player #%d establishes a \"Cease Fire\" with Player #%d\n", m_owner, other_player)) ;

	if(GetDiplomaticState(other_player) != DIPLOMATIC_STATE_ALLIED)
		SetDiplomaticState(other_player, DIPLOMATIC_STATE_CEASEFIRE);     

	Assert(g_player[other_player]);
	if (g_player[other_player] && g_player[other_player]->GetDiplomaticState(m_owner) != DIPLOMATIC_STATE_ALLIED) {
		
		g_player[other_player]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_CEASEFIRE);
	}
}














void Player::BreakCeaseFire(PLAYER_INDEX other_player, BOOL sendMessages)
	{
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BREAK_CEASE_FIRE,
										   m_owner, other_player, sendMessages));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BREAK_CEASE_FIRE,
									  m_owner, other_player, sendMessages));
		g_network.Unblock(m_owner);
	}

	Agreement	a ;

	
	while ((a = FindAgreement(other_player)) != Agreement(0))
		{
		DPRINTF(k_DBG_INFO, ("Player #%d breaks an agreement with Player #%d\n", m_owner, other_player)) ;
		a.Break();
		g_slicEngine->RunTreatyBrokenTriggers(m_owner, other_player, a);
		a.KillAgreement() ;										
		m_broken_alliances_and_cease_fires++;

		if(sendMessages) {
			sendMessages = FALSE;
			SlicObject *so = new SlicObject("108YouBrokeCeaseFire");
			so->AddRecipient(m_owner);
			so->AddCivilisation(other_player);
			g_slicEngine->Execute(so);

			so = new SlicObject("109CeaseFireBroken");
			so->AddRecipient(other_player);
			so->AddCivilisation(m_owner);
			g_slicEngine->Execute(so);
		}

        SetDiplomaticState(other_player, DIPLOMATIC_STATE_WAR); 
		Assert(g_player[other_player]);
		if (g_player[other_player]) {
			
			g_player[other_player]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_WAR);
		}

		
		

		}



	}

void Player::BreakAllTreaties(PLAYER_INDEX with)
{
	sint32 oldAlliance = m_broken_alliances_and_cease_fires;
	BreakAlliance(with);
	BreakCeaseFire(with, oldAlliance == m_broken_alliances_and_cease_fires);
}

void Player::BeginTurnAgreements()
{
	sint32 i, n = m_agreed->Num();
    DynamicArray<Agreement>   agreed(*m_agreed);

    for(i = n - 1; i >= 0; i--) {
		if(!g_theAgreementPool->IsValid(agreed[i]))
			continue;
		Agreement ag = agreed.Access(i);

		if(ag.GetOwner() == m_owner) {
			ag.BeginTurnOwner();
		} else if(ag.GetRecipient() == m_owner) {
			ag.BeginTurnRecipient();
		} else {
			Assert(FALSE);
		}

		
		
		if(g_theAgreementPool->IsValid(ag) &&
		   ag.GetTurns() == 2) {
			SlicObject *so = new SlicObject("001TreatyToExpire");
			if(ag.GetRecipient() == m_owner) {
				so->AddRecipient(ag.GetOwner());
				so->AddCivilisation(ag.GetRecipient());
			} else {
				so->AddRecipient(ag.GetRecipient());
				so->AddCivilisation(ag.GetOwner());
			}
			g_slicEngine->Execute(so);
		}
	}
}











void Player::RemoveAgreementReferences(Agreement a)
	{

    if (a.GetAgreement() == AGREEMENT_TYPE_CEASE_FIRE) { 
        m_diplomatic_state[a.GetRecipient()] = DIPLOMATIC_STATE_NEUTRAL; 
		sint32 other_player = a.GetRecipient();

		
		Assert(g_player[other_player]);
		if (g_player[other_player]) {
			g_player[other_player]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_NEUTRAL);
		}

	}

	m_agreed->Del(a) ;
	}












void Player::AddAgreement(Agreement a)
{
	sint32 i;

	
	for(i = m_agreed->Num() - 1; i >= 0; i--) {
		Agreement kill = m_agreed->Access(i);
		if(kill.GetOwner() == a.GetOwner() &&
		   kill.GetRecipient() == a.GetRecipient() &&
		   kill.GetAgreement() == a.GetAgreement()) {
			kill.Kill(); 
		}
	}
	m_agreed->Insert(a) ;
}












void Player::RemoveDiplomaticReferences(DiplomaticRequest a)
{
	Assert(false);
}












void Player::AddDiplomaticRequest(DiplomaticRequest &request)
	{
	m_requests->Insert(request) ;
	}












void Player::RequestGreeting(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_GREETING) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
	so->AddCivilisation(recipient) ;

    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestDemandAdvance(const PLAYER_INDEX recipient, AdvanceType advance)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_ADVANCE) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetAdvance(advance) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddAdvance(advance) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestDemandCity(const PLAYER_INDEX recipient, Unit &city)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_CITY) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetTarget(city) ;												
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddCity(city) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}












void Player::RequestDemandMap(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_MAP) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
	so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestDemandGold(const PLAYER_INDEX recipient, Gold &amount)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_GOLD) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetGold(amount) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddGold(amount.GetLevel()) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}














void Player::RequestDemandStopTrade(const PLAYER_INDEX recipient, const PLAYER_INDEX thirdParty)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_STOP_TRADE) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetThirdParty(thirdParty) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
	so->AddCivilisation(recipient) ;
	so->AddCivilisation(thirdParty) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}












void Player::RequestDemandNoPiracy(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_NO_PIRACY) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete() ;
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
	so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestDemandAttackEnemy(const PLAYER_INDEX recipient, const PLAYER_INDEX thirdParty)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_ATTACK_ENEMY) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetThirdParty(thirdParty) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
	so->AddCivilisation(recipient) ;
	so->AddCivilisation(thirdParty) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}












void Player::RequestDemandLeaveOurLands(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestDemandReducePollution(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_DEMAND_REDUCE_POLLUTION) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}















void Player::RequestOfferAdvance(const PLAYER_INDEX recipient, AdvanceType &advance)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_ADVANCE) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetAdvance(advance) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddAdvance(advance) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}














void Player::RequestOfferCity(const PLAYER_INDEX recipient, Unit &city)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_CITY) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetTarget(city) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddCity(city) ;

    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestOfferMap(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_MAP) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}














void Player::RequestOfferGold(const PLAYER_INDEX recipient, const Gold &amount)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_GOLD) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetGold(amount) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddGold(amount.GetLevel()) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestOfferCeaseFire(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_CEASE_FIRE) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	g_slicEngine->RunSentCeaseFireTriggers(m_owner, recipient);
	}












void Player::RequestOfferPermanentAlliance(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}














void Player::RequestOfferPactCaptureCity(const PLAYER_INDEX recipient, Unit &city)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetThirdParty(city.GetOwner()) ;
	r.SetTarget(city) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddCity(city) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestOfferPactEndPollution(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_OFFER_PACT_END_POLLUTION) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}




































































































































































































































void Player::RequestExchangeAdvance(const PLAYER_INDEX recipient, AdvanceType &advance, AdvanceType &desired_advance)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_EXCHANGE_ADVANCE) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetAdvance(advance) ;
	r.SetWanted(desired_advance) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddAdvance(advance) ;
	so->AddAdvance(desired_advance) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}















void Player::RequestExchangeCity(const PLAYER_INDEX recipient, Unit &offerCity, Unit &wantCity)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_EXCHANGE_CITY) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.SetTarget(offerCity) ;
	r.SetWanted(wantCity) ;
	r.Complete();
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
	so->AddCity(offerCity) ;
	so->AddCity(wantCity) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;
	}













void Player::RequestExchangeMap(const PLAYER_INDEX recipient)
	{
	
	DiplomaticRequest	r = g_theDiplomaticRequestPool->Create(m_owner, recipient, REQUEST_TYPE_EXCHANGE_MAP) ;
	if (r==DiplomaticRequest(0))
		c3errors_FatalDialogFromDB("DIPLOMACY_ERROR", "DIPLOMACY_INVALID_REQUEST_ID") ;

	r.Complete();
	
	SlicObject *so = new SlicObject(r.GetRequestString()) ;
	so->AddRecipient(recipient) ;
	so->AddCivilisation(m_owner) ;
    so->AddCivilisation(recipient) ;
    Assert(g_player[recipient]); 
    so->AddAttitude(GetAttitude(recipient));
	g_slicEngine->Execute(so) ;

	}











void Player::DumpAgreements(void)
	{
	sint32	i,
			n ;

	DPRINTF(k_DBG_INFO, ("Diplomatic Agreements for P%d\n", m_owner)) ;
	n = m_agreed->Num() ;
	for (i=0; i<n; i++)
		m_agreed->Access(i).Dump(i) ;

	}











void Player::DumpRequests(void)
	{
	sint32	i,
			n ;

	DPRINTF(k_DBG_INFO, ("Dumping Diplomatic Requests for Player #%d\n", m_owner)) ;
	n = m_requests->Num() ;
	for (i=0; i<n; i++)
		{
		m_requests->Access(i).Dump(i) ;
		}

	}













DiplomaticRequest Player::RequestIndexToRequest(sint32 requestIndex)
	{
	
	return (m_requests->Get(requestIndex)) ;
	}













void Player::DiplomaticMute(PLAYER_INDEX player, BOOL enable)
	{
	Assert((player>=0) && (player<k_MAX_PLAYERS)) ;
	if ((player<=0) || (player>k_MAX_PLAYERS))
		{
		c3errors_FatalDialog("Diplomacy Error", "Player number out of bounds") ;
		return ;
		}


	if (enable)
		m_diplomatic_mute |= (1<<player) ;
	else
		m_diplomatic_mute &= ~(1<<player) ;

	}














BOOL Player::IsMuted(PLAYER_INDEX player)
	{
	return (m_diplomatic_mute & (1<<player)) ;
	}













BOOL Player::IsViolatingBorders(PLAYER_INDEX player)
	{
	PLAYER_INDEX	o ;

	MapPoint	armyPos ;

	sint32	i,
			armyNum ;
	
	armyNum = m_all_armies->Num() ;
	for (i=0; i<armyNum; i++)										
		{
		m_all_armies->Get(i).GetPos(armyPos) ;						
		o = (PLAYER_INDEX)g_theWorld->GetCell(armyPos)->GetOwner() ;				
		if (o == player)											
			{
			DPRINTF(k_DBG_INFO, ("Player %d is violating the borders of Player %d\n", m_owner, player)) ;

			return (TRUE) ;											
			}

		}

	DPRINTF(k_DBG_INFO, ("Player %d is not violating the borders of Player %d\n", m_owner, player)) ;

	return (FALSE) ;												
	}
















BOOL Player::GiveGold(const PLAYER_INDEX other_player, const Gold &amount)
	{
	if (!m_gold->GiveGold(amount))
		{
		DPRINTF(k_DBG_GAMESTATE, ("Player %d does not have enough to give %d gold to Player %d\n", m_owner, amount.GetLevel(), other_player)) ;

		return (FALSE) ;
		}

    Assert(0 <= other_player); 
    Assert(other_player < k_MAX_PLAYERS);
    Assert(g_player[other_player]); 
	g_player[other_player]->m_gold->AddGold(amount) ;
	DPRINTF(k_DBG_GAMESTATE, ("Player %d gave %d gold to Player %d\n", m_owner, amount.GetLevel(), other_player)) ;

	return (TRUE) ;
	}
















void Player::BequeathGold(const Gold &amount)
	{
	m_gold->AddGold(amount) ;
	DPRINTF(k_DBG_GAMESTATE, ("Player has been given %d gold\n", amount.GetLevel())) ;

	}















void Player::GiveCity(const PLAYER_INDEX player, const sint32 c)
{
	Assert(FALSE);
#if 0
	Unit	u = m_all_cities->Get(c).m_id ;

	CityData	*cityData = m_all_cities->Get(c).GetData()->GetCityData() ;

	MapPoint newPos ;
	double dist;
	Unit city;
	MapPoint oldPos;
	u.GetPos(oldPos);

	GetNearestCity(oldPos, city, dist, TRUE);
	city.GetPos(newPos);

    BOOL revealed_foreign_units; 
    BOOL revealed_unexplored; 


	cityData->TeleportUnits(newPos, revealed_foreign_units, revealed_unexplored) ;

	u.ResetCityOwner(player, FALSE, CAUSE_REMOVE_CITY_DIPLOMACY) ;
#endif
}











void Player::GiveCity(const PLAYER_INDEX recipient, Unit city)
{

	if(!g_theUnitPool->IsValid(city)) {
		return;
	}

	CityData	*cityData = city.GetData()->GetCityData() ;
	MapPoint pos ;
	
	MapPoint newPos ;
	double dist;
	Unit c;
	MapPoint oldPos;
	city.GetPos(oldPos);

	if(!GetNearestCity(oldPos, c, dist, TRUE)) {
		sint32 i;
		Cell *cell = g_theWorld->GetCell(oldPos);
		CellUnitList *units = cell->UnitArmy();
		if(units) {
			for(i = units->Num() - 1; i >= 0; i--) {
				g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
									   GEA_Unit, units->Access(i),
									   GEA_Int, CAUSE_REMOVE_ARMY_DIPLOMACY,
									   GEA_Player, recipient,
									   GEA_End);
				
			}
		}
	}  else {
		c.GetPos(newPos);
		BOOL revealed_foreign_units; 
		BOOL revealed_unexplored;

		cityData->TeleportUnits(newPos, revealed_foreign_units, revealed_unexplored, recipient) ;
	}

	city.ResetCityOwner(recipient, FALSE, CAUSE_REMOVE_CITY_DIPLOMACY) ;

}












void Player::ExchangeCity(const PLAYER_INDEX recipient, const sint32 cityA, const sint32 cityB)
	{
	g_player[m_owner]->GiveCity(recipient, cityA) ;
	g_player[recipient]->GiveCity(m_owner, cityB) ;
	}












void Player::ExchangeCity(const PLAYER_INDEX recipient, const Unit &cityA, const Unit &cityB)
	{
	g_player[m_owner]->GiveCity(recipient, cityA) ;
	g_player[recipient]->GiveCity(m_owner, cityB) ;
	}












#ifdef _DEBUG
extern sint32 g_robotMessages;
#endif

void Player::AddMessage(Message &msg)
	{
	if(!g_network.IsActive() || 
	   g_network.GetPlayerIndex() == m_owner ||
	   m_playerType != PLAYER_TYPE_NETWORK )
	{
		m_messages->Insert(msg) ;
#ifdef _DEBUG
		{
			FILE *mfile;
			static int openedFile = 0;
			if(openedFile) {
				mfile = fopen("logs\\messages.txt", "a");
			} else {
				mfile = fopen("logs\\messages.txt", "w");
				openedFile = 1;
			}
			if(mfile) {
				if(msg.AccessData()->GetSlicSegment() &&
				   msg.AccessData()->GetSlicSegment()->GetName()) {
					fprintf(mfile, "%s\n", msg.AccessData()->GetSlicSegment()->GetName());
				} else {
					fprintf(mfile, "bug\n");
				}
				fclose(mfile);
			}
		}
#endif
		if(m_playerType != PLAYER_TYPE_ROBOT
#ifdef _DEBUG
		   || g_robotMessages
#endif
			) 
			{
				if(msg.UseDirector() && 
				   ((!g_currentMessageWindow) ||
					(!g_currentMessageWindow->GetMessage()) ||
					(!g_theMessagePool->IsValid(*g_currentMessageWindow->GetMessage())))) {
					g_director->AddMessage(msg);
				} else if(msg.IsAlertBox()) {
					if(!messagewin_IsModalMessageDisplayed()) {
						messagewin_CreateModalMessage(msg);
					}
				} else {
					messagewin_CreateMessage( msg );
					if(msg.IsInstantMessage() &&
						// JJB added this to prevent instant messages showing
						// out of turn in hotseat games.
						// With the existing behaviour they would show immediately
						// which would often mean that they show on the wrong players
						// turn.
						 g_selected_item->GetVisiblePlayer() == m_owner &&
					   ((!g_currentMessageWindow) ||
					    (!g_currentMessageWindow->GetMessage()) ||
					    (!g_theMessagePool->IsValid(*g_currentMessageWindow->GetMessage())))) {
						msg.Show();
					}
				}
			}

		}
	}


void Player::NotifyModalMessageDestroyed()
{
	sint32 i;
	for(i = 0; i < m_messages->Num(); i++) {
		if(!g_theMessagePool->IsValid(m_messages->Access(i))) {
			m_messages->DelIndex(i);
			i--;
		} else {
			if(m_messages->Access(i).IsAlertBox()) {
				messagewin_CreateModalMessage(m_messages->Access(i));
				break;
			}
		}
	}
}

void Player::RegisterDiplomaticMessage(const Message &msg)
{
	
	

	
	
	
	
	
	
}

void Player::RegisterDiplomaticResponse(const DiplomaticRequest &req)
{
	
	
	
	
	
	
}











void Player::RemoveMessageReferences(Message msg)
	{
	m_messages->Del(msg) ;
	}











void Player::DumpMessages(void)
	{
	sint32	i,
			n ;

	n = m_messages->Num() ;
	if (n==0)
		{
		DPRINTF(k_DBG_INFO, ("Player #%d has no messages\n", m_owner)) ;
		}
	else
		{
		DPRINTF(k_DBG_INFO, ("Dumping Messages for Player #%d\n", m_owner)) ;
		for (i=0; i<n; i++)
			m_messages->Access(i).Dump(i) ;

		}

	}


void Player::SendTestMessage(void)
	{
	SlicObject *so = new SlicObject("pact fulfilled city captured") ;
	so->AddCity(m_all_cities->Get(0)) ;
	so->AddCivilisation(m_owner) ;
	so->AddCivilisation(m_owner+1) ;
	}

	










MBCHAR *Player::GetLeaderName(void)
{
    if (!g_network.IsActive() && !g_slicEngine->GetTutorialActive() &&
		g_theProfileDB->GetPlayerIndex() == m_owner) {
        return (g_theProfileDB->GetLeaderName());
    } else {
        return (m_civilisation->GetLeaderName());
    }
}











void Player::GetPluralCivName(MBCHAR *s)
	{
	m_civilisation->GetPluralCivName(s) ;
	}

	










void Player::GetSingularCivName(MBCHAR *s)
	{
	m_civilisation->GetSingularCivName(s) ;
	}

	









void Player::DumpAllies(void)
	{
	sint32	i ;

	MBCHAR	s[_MAX_PATH] ;

	DPRINTF(k_DBG_INFO, ("Dumping alliances for Player #%d", m_owner)) ;
	s[0] = NULL ;
	for (i=0; i<k_MAX_PLAYERS; i++)
		{
		if ((mask_alliance & (0x01<<i)) && (i != m_owner))
			sprintf(s, "%s P%d, ", s, i) ;

		}

	if (strlen(s))
		{
		s[strlen(s)-2] = NULL ;
		DPRINTF(k_DBG_INFO, ("%s", s)) ;
		}
	else
		{
		DPRINTF(k_DBG_INFO, ("No alliances formed")) ;
		}

	}


sint32 Player::GetCityIndex(const Unit &c, sint32 &idx)

{
    sint32 n; 

    n = m_all_cities->Num(); 
    for (idx=0; idx<n; idx++) { 
        if (m_all_cities->Get(idx) == c) { 
            return TRUE;
        } 
    } 
    return FALSE; 
}

Unit Player::GetCityFromIndex(sint32 unit_idx)
{ 
	return m_all_cities->Get(unit_idx).m_id; 
}

sint32 Player::GetArmyIndex(const Unit &u, sint32 &idx)

{
    sint32 n, m, j; 

    n=m_all_units->Num(); 
    for (idx=0; idx<n; idx++) { 
        m = m_all_armies->Access(idx).Num(); 
        for (j=0; j<m; j++) { 
            if (m_all_armies->Access(idx)[j] == u) { 
                return TRUE;
            }
        }
    } 
    return FALSE; 
        
}

void Player::AdjustPollution(const sint32 amount)
	{
    if (g_theGameSettings->GetPollution())
		m_pollution_history[0] += amount ;

	}


void Player::AdjustEventPollution(const sint32 amount)
	{
    if (g_theGameSettings->GetPollution())
		m_event_pollution[0] += amount ;

	}











void Player::BeginTurnPollution(void)
{



	
	memmove(&m_event_pollution[1], &m_event_pollution[0], sizeof(m_event_pollution) - sizeof(m_event_pollution[0])) ;
	m_event_pollution[0] = 0 ;
}

void Player::GetArmyPos(sint32 index, MapPoint &army_pos)

{
    m_all_armies->Access(index).GetPos(army_pos); 
}


BOOL Player::CanCreateImprovement(sint32 type,
								  const MapPoint &point, sint32 extraData, 
								  const BOOL check_materials, ERR_BUILD_INST &err)

{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
	Cell *cell = g_theWorld->GetCell(point);
	
	Assert(rec != NULL);
	if(!rec)
		return FALSE;

	sint32 i;
	for(i = 0; i < cell->GetNumDBImprovements(); i++) {
		if(cell->GetDBImprovement(i) == type) {
			
			return FALSE;
		}
	}

	for(i = 0; i < cell->GetNumImprovements(); i++) {
		if(cell->AccessImprovement(i).GetType() == type) {
			
			return FALSE;
		}
	}

	if(!terrainutil_CanPlayerBuildAt(rec, m_owner, point)) {
		return FALSE;
	}	

	if(!rec->GetClassTerraform()) {
		const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, point);
		if(!effect)
			return FALSE;
	}

	
	if(check_materials) {		
		if(m_materialPool->GetMaterials() < terrainutil_GetProductionCost(rec->GetIndex(), point, 0)) {
			return FALSE;
		}
	}

	return TRUE;
}

TerrainImprovement Player::CreateImprovement(sint32 dbIndex,
											 MapPoint &point,
											 sint32 extraData)
{
	TerrainImprovement theImprovement(0);
	ERR_BUILD_INST err;

	if(!CanCreateImprovement(dbIndex, point, extraData, true, err))
		return theImprovement;

	theImprovement = g_theTerrainImprovementPool->Create(m_owner,
														 point,
														 dbIndex,
														 extraData);

	if(g_theTerrainImprovementPool->IsValid(theImprovement.m_id)) {
		if(g_network.IsClient()) {
			g_network.AddCreatedObject(theImprovement.AccessData());
			g_network.SendAction(new NetAction(NET_ACTION_TERRAIN_IMPROVEMENT, 
											   dbIndex, 
											   (sint32)point.x, (sint32)point.y,
											   extraData,
											   theImprovement.m_id));											   
		}
		m_terrainImprovements->Insert(theImprovement);
		if(theImprovement.GetMaterialCost() <= m_materialPool->GetMaterials()) {
			theImprovement.StartBuilding();
		}
	}

	return theImprovement;
}

sint32 Player::GetMaterialsStored() const
{ 
   return m_materialPool->GetMaterials(); 
} 


void Player::AddImprovement(TerrainImprovement imp)
{
	m_terrainImprovements->Insert(imp);
}

void Player::RemoveImprovementReferences(TerrainImprovement imp)
{
	m_terrainImprovements->Del(imp);
}

Installation Player::CreateInstallation(sint32 type,
										MapPoint &point)
{
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
	}
	Installation theInst = g_theInstallationPool->Create(m_owner,
														 point,
														 type);
	if(theInst != Installation(0)) {
		if(g_network.IsClient()) {
			g_network.AddCreatedObject(theInst.AccessData());
			g_network.SendAction(new NetAction(NET_ACTION_CREATE_INSTALLATION,
											   (sint32)type,
											   point.x, point.y,
											   (uint32)theInst));
		}
	}
	if(g_network.IsHost()) {
		g_network.Unblock(m_owner);
	}

	return theInst;
}

void Player::AddInstallation(Installation &inst)
{
	m_allInstallations->Insert(inst);

	MapPoint pos;
	inst.GetPos(pos); 

	double visionRange = terrainutil_GetVisionRange(inst.GetType(),pos);
	if( visionRange > 0) {
		m_allRadarInstallations->Insert(inst);
	}

	
	

	if(visionRange > 0) {
		if(g_selected_item->GetVisiblePlayer() == m_owner) {
			BOOL revealedUnexplored = FALSE;
			g_tiledMap->GetLocalVision()->AddVisible(pos, visionRange, revealedUnexplored);
		}
	}

    
    
    
	
    
	
	
}

void Player::RemoveInstallationReferences(Installation &inst)
{
	m_allRadarInstallations->Del(inst);
	m_allInstallations->Del(inst);

    MapPoint pos;
	inst.GetPos(pos); 

    
    
    
	
	
	
	
}

#ifdef BATTLE_FLAGS
void Player::AddBattleFlag(const MapPoint &point)
{
	m_battleFlags->Insert(point);
	g_theWorld->GetCell(point);
}

void Player::BeginTurnBattleFlags()
{
	sint32 i, n, d = 0;
	n = m_battleFlags->Num();
	sint32 *dead_index = new sint32[n];

	for(i = 0; i < n; i++) {
		if(!g_theWorld->GetCell(m_battleFlags->Get(i))->DecayBattleFlag()) {
			dead_index[d++] = i;
		}
	}

	for(i = d-1; i >= 0; i--) {
		m_battleFlags->DelIndex(dead_index[i]);
	}

	delete [] dead_index;
}
#endif

sint32 Player::GetReadinessLevel() const
{
    return m_readiness->GetLevel(); 
}

sint32 Player::GetReadinessCost() const
{
    return sint32(m_readiness->GetCost()); 
} 


void Player::SetReadinessLevel(READINESS_LEVEL level, BOOL immediate)
{
	m_readiness->SetLevel(m_government_type, *m_all_armies, level, immediate);
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_SET_READINESS, (sint32)level, (BOOL)immediate));
	} 
	





}

double Player::GetHPModifier()
{
	return m_readiness->GetHPModifier();
}

double Player::GetSupportModifier()
{
	return m_readiness->GetSupportModifier(m_government_type);
}

void Player::BuildWonder(sint32 wonder, Unit &city)
{
	Assert(wonderutil_IsAvailable(wonder, m_owner));
	if(!wonderutil_IsAvailable(wonder, m_owner))
		return;

#if 0
	
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_WONDER,
										   (uint32)city, wonder));
	}
#endif
	BOOL b = city.BuildWonder(wonder);

	
    
}

void Player::BuildEndGame(sint32 type, Unit &city)
{
	BOOL b = city.BuildEndGame(type);
	Assert(b);
}
void Player::AddWonder(sint32 wonder, Unit &city)
{
	DPRINTF(k_DBG_GAMESTATE, ("Player %d built wonder %d\n", m_owner, wonder));
	m_builtWonders |= ((uint64)1 << wonder);
	

	if(wonderutil_Get(wonder)->GetPollutersToParks()) {

		
		
		
		if(!g_network.IsClient() || g_network.IsLocalPlayer(m_owner)) {
			
			sint32 polluters;
			wonderutil_Get(wonder)->GetPollutersToParks(polluters);
			Unit *ua = new Unit[polluters];
			for(sint32 i = 0; i < polluters; i++) {
				ua[i].m_id = (0);
			}
			
			sint32 pl, c, po, mv;
			
			for(pl = 0; pl < k_MAX_PLAYERS; pl++) {
				if(!g_player[pl]) continue;
				int numCities=g_player[pl]->m_all_cities->Num() ;
				for(c = 0; c <numCities; c++) {
					Unit city(g_player[pl]->m_all_cities->Access(c));

					
					if(city.AccessData()->GetCityData()->GetBuiltWonders() & (uint64(1) << wonder))
						continue;

					
					if(city.GetPollution() < 1)
						continue;

					for(po = 0; po < polluters; po++) {
						if(ua[po].m_id == (0)) {
							ua[po] = city;
							break;
						} else if(city.GetPollution() > ua[po].GetPollution()) {
							for(mv = polluters - 1; mv > po; mv--) {
								ua[mv] = ua[mv -1];
							}
							ua[po] = city;
							break;
						}
					}
				}
			}

			for(po = 0; po < polluters; po++) {
				if(ua[po].m_id != (0)) {
					ua[po].CityToPark(m_owner);
				}
			}
			delete [] ua;
		}
	}

	if(wonderutil_GetFreeSlaves((uint64)1 << wonder)) {
		sint32 i, n = m_all_cities->Num();
		for(i = 0; i < n; i++) {
			m_all_cities->Access(i).FreeSlaves();
		}

		for(i = m_all_units->Num() - 1; i >= 0; i--) {
			const UnitRecord *rec = m_all_units->Access(i).GetDBRec();
			if(rec->GetSlaveRaids() || rec->GetSettlerSlaveRaids() ||
			   rec->GetSlaveUprising()) {
				m_all_units->Access(i).Kill(CAUSE_REMOVE_ARMY_EMANCIPATION, -1);
			}
		}
			
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i] && i != m_owner) {
				g_player[i]->Emancipate();
			}
		}
	}

	sint32 hpBonus = wonderutil_GetIncreaseHP((uint64)1 << wonder);
	if(hpBonus > 0) {
		sint32 i, n = m_all_units->Num();
		for(i = 0; i < n; i++) {
			m_all_units->Access(i).AddWonderHPBonus(hpBonus);
		}
	}

	sint32 fullHappinessTurns = wonderutil_GetTemporaryFullHappiness(
		((uint64)1 << wonder));
	if(fullHappinessTurns > 0) {
		sint32 i, n = m_all_cities->Num();
		for(i = 0; i < n; i++) {
			m_all_cities->Access(i).SetFullHappinessTurns(fullHappinessTurns);
		}
	}

	
	
	
	
	sint32 readinessReduction = wonderutil_GetReadinessCostReduction((uint64)1 << wonder);
	if(readinessReduction > 0) {
		m_readiness->RecalcCost();
	}
		
	if(wonderutil_GetNukesEliminated((uint64) 1 << wonder)) {
		sint32 i, p;
		SlicObject *so = new SlicObject("251NaniteDefuseEliminatesNukes");
		so->AddWonder(wonder);
		so->AddCivilisation(m_owner);
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				so->AddRecipient(p);
				
				for(i = 0; i < g_player[p]->m_all_cities->Num(); i++) {
					g_player[p]->m_all_cities->Access(i).AccessData()->GetCityData()->EliminateNukes();
				}
				
				for(i = g_player[p]->m_all_units->Num() - 1; i >= 0; i--) {
					if(g_player[p]->m_all_units->Access(i).GetDBRec()->GetNuclearAttack()) {
						g_player[p]->m_all_units->Access(i).Kill(CAUSE_REMOVE_ARMY_NUKES_ELIMINATED, m_owner);
					}
				}
			}
		}
		g_slicEngine->Execute(so);
	}

	if(wonderutil_GetCloseEmbassies((uint64)1 << wonder)) {
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				g_player[p]->CloseEmbassy(m_owner);
				g_player[p]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_NEUTRAL);
				SetDiplomaticState(p, DIPLOMATIC_STATE_NEUTRAL);
			}
		}
	}

	if(wonderutil_GetEmbassiesEverywhereEvenAtWar((uint64)1 << wonder)) {
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				g_player[p]->SetDiplomaticState(m_owner, DIPLOMATIC_STATE_NEUTRAL);
				SetDiplomaticState(p, DIPLOMATIC_STATE_NEUTRAL);
			}
		}
	}

	if(wonderutil_Get(wonder)->GetPreventConversion()) {
		sint32 i;
		for(i = 0; i < m_all_cities->Num(); i++) {
			m_all_cities->Access(i).GetData()->GetCityData()->Unconvert(FALSE);
		}
	}

	
	if(wonderutil_Get(wonder)->GetGlobalRadar()) {
		m_hasGlobalRadar = TRUE;
		g_theWonderTracker->SetGlobeSatFlags(g_theWonderTracker->GlobeSatFlags() | (1 << m_owner));

		
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			sint32 u;
			if(!g_player[p] || m_owner == p)
				continue;

			
			
			ContactMade(p);

			for(u = 0; u < g_player[p]->m_all_units->Num(); u++) {
				if(g_player[p]->m_all_units->Access(u).GetDBRec()->GetVisionClassStandard()) {
					g_player[p]->m_all_units->Access(u).SetVisible(m_owner);
				}
			}

			sint32 c;
			for(c = 0; c < g_player[p]->m_all_cities->Num(); c++) {
				g_player[p]->m_all_cities->Access(c).SetVisible(m_owner);
			}
		}

		
		
		m_vision->SetTheWholeWorldExplored();
		m_vision->ClearUnseen();

		if(m_owner == g_selected_item->GetVisiblePlayer()) {
			
			
			
			g_director->AddCopyVision();
		}
	}

	sint32 i;
	for(i = 0; i < m_all_cities->Num(); i++) {
		m_all_cities->Access(i).GetData()->GetCityData()->GetBuildQueue()->RemoveIllegalItems();
	}

	
	
	sint32 pl, c;
	for(pl = 0; pl < k_MAX_PLAYERS; pl++) {
		if(pl == m_owner || 
		   !g_player[pl]) {
			continue;
		}
		int numCities=g_player[pl]->m_all_cities->Num() ;
		for(c = 0; c <numCities; c++) {
			g_player[pl]->m_all_cities->Access(c).GetData()->GetCityData()->GetBuildQueue()->
				RemoveIllegalItems(TRUE);
		}
	}

	const WonderRecord *wrec = g_theWonderDB->Get(wonder);
	sint32 buildingIndex;
	if(wrec->GetBuildingEverywhereIndex(buildingIndex)) {
		m_wonderBuildings |= ((uint64)1 << buildingIndex);
	}
		
}

void Player::RemoveWonder(sint32 which, BOOL destroyed)
{
	m_builtWonders &= ~((uint64)1 << which);

    if (!wonderutil_IsObsolete(which)) { 
	    sint32 increaseRegard = wonderutil_GetIncreaseRegard((uint64)1 << which);
		
		
    }

	if(destroyed) {
		
		
        sint32 j;
		Unit c;
		g_theWonderTracker->GetCityWithWonder(which, c);
		SlicObject *so = new SlicObject("097WonderDestroyed") ;
        for (j=1; j<k_MAX_PLAYERS; j++)
            if (g_player[j] && !g_player[j]->m_isDead)
                so->AddRecipient(j) ;
        so->AddWonder(which);
		so->AddCity(c);
        g_slicEngine->Execute(so) ;
    }

	sint32 buildingIndex;
	if(g_theWonderDB->Get(which)->GetBuildingEverywhereIndex(buildingIndex)) {
		m_wonderBuildings &= ~((uint64)1 << buildingIndex);
	}

	if(g_theWonderDB->Get(which)->GetGlobalRadar()) {
		m_hasGlobalRadar = FALSE;
		g_theWonderTracker->SetGlobeSatFlags(g_theWonderTracker->GlobeSatFlags() & ~(1 << m_owner));
	}
}

uint64 Player::GetBuiltWonders()
{
	return m_builtWonders;
}

uint64 Player::GetWonderBuildings()
{
	return m_wonderBuildings;
}

void Player::Entrench(sint32 army_idx)
{
	Assert(army_idx < m_all_armies->Num());
	if(army_idx >= m_all_armies->Num())
		return;

	m_all_armies->Access(army_idx).AddOrders(UNIT_ORDER_ENTRENCH);
	
	
	
	

	m_all_armies->Access(army_idx).ClearOrders();


	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_ENTRENCH, (uint32)army_idx));
	}
}

void Player::Detrench(sint32 army_idx)
{
	Assert(army_idx < m_all_armies->Num());
	if(army_idx >= m_all_armies->Num())
		return;

	m_all_armies->Access(army_idx).AddOrders(UNIT_ORDER_DETRENCH);
}

void Player::Sleep(sint32 army_idx)
{
	Assert(army_idx < m_all_armies->Num());
	if(army_idx >= m_all_armies->Num())
		return;

	m_all_armies->Access(army_idx).AddOrders(UNIT_ORDER_SLEEP);
}

void Player::WakeUp(sint32 army_idx)
{
	Assert(army_idx < m_all_armies->Num());
	if(army_idx >= m_all_armies->Num())
		return;

	m_all_armies->Access(army_idx).AddOrders(UNIT_ORDER_WAKE_UP);
}

Army Player::GetArmy(sint32 s_index)
{ 
	return m_all_armies->Access(s_index);
} 

Unit Player::GetTopSelectedArmy(const sint32 selected_army)
{ 
	if (m_all_armies->Num() < 1) { 
		return Unit(0); 
	} else { 
		sint32 n = m_all_armies->Access(selected_army).Num();
		Assert(0 <n);
		return m_all_armies->Access(selected_army)[n-1]; 
	}
}

sint32 Player::GetNumCities() const 
{
	return m_all_cities->Num(); 
}

sint32 Player::GetMaxCityCount() const 
{
	return m_maxCityCount; 
}













sint32 Player::GetPollutionSizeModifier(void) const
	{
	return (m_advances->GetPollutionSizeModifier()) ;
	}













sint32 Player::GetPollutionProductionModifier(void) const
	{
	return (m_advances->GetPollutionProductionModifier()) ;
	}

	
BOOL Player::HasAdvance(AdvanceType adv) const
{ 
	if (!m_advances) return FALSE;

	return m_advances->HasAdvance(adv); 
}

sint32 Player::NumAdvances()
{ 
	if (!m_advances) return 0;
	return (m_advances->GetNum()); 
}

uint32 Player::RoadAdvanceLevel() const
{
#if 0
	
	uint32 enable_maglev = g_theAdvanceDB->EnableImprovement(TERRAIN_IMPROVEMENT_ROAD_3);
	if(enable_maglev >= 0 && m_advances->HasAdvance(enable_maglev)) {
		return 3;
	}

	uint32 enable_railroad = g_theAdvanceDB->EnableImprovement(TERRAIN_IMPROVEMENT_ROAD_2);
	if(enable_railroad >= 0 && m_advances->HasAdvance(enable_railroad)) {
		return 2;
	}

	uint32 enable_road = g_theAdvanceDB->EnableImprovement(TERRAIN_IMPROVEMENT_ROAD_1);
	if(enable_road >= 0 && m_advances->HasAdvance(enable_road)) {
		return 1;
	}
#endif
	return 1;
}

void Player::SetCityRoads()
{
	if(!m_all_cities)
		return;

	for(sint32 i = 0; i < m_all_cities->Num(); i++) {
		m_all_cities->Access(i).AccessData()->GetCityData()->SetRoad();
	}
}

#if 0
UnitOrderQueue* Player::GetOrderQueue(sint32 i)
{
	return &m_army_orders->Access(i); 
}
#endif

void Player::GiveOrders(sint32 army, UNIT_ORDER_TYPE order)
{
	
	Assert(army >= 0 && army < m_all_armies->Num());
	if(army < 0 || army >= m_all_armies->Num())
		return;

	if(order == UNIT_ORDER_NONE) {
		m_all_armies->Access(army).ClearOrders();
	} else {
		m_all_armies->Access(army).AddOrders(order);
	}
}

void Player::AddGold(const sint32 d)
{ 
	m_gold->AddGold(d);
}

void Player::SubGold(const sint32 d)
{ 
	m_gold->SubGold(d);
}

void Player::AddGold(const Gold &amount)
{ 
	m_gold->AddGold(amount);
}

void Player::SubGold(const Gold &amount)
{ 
	m_gold->SubGold(amount);
}

sint32 Player::GetGold()
{ 
	return m_gold->GetLevel();
}

void Player::SetGold(const sint32 d)
{
	m_gold->SetLevel(d);
}

void Player::GetScienceTaxRate(double &s){ m_tax_rate->GetScienceTaxRate(s); };

void Player::GetPeaceMovement(double &overseas_defeat, double &home_defeat, 
							  double &overseas)
{  
	m_global_happiness->GetPeaceMovement(overseas_defeat, 
										 home_defeat, overseas);
}

void Player::SetWorkdayLevel (sint32 w) 
{ 
	m_global_happiness->SetWorkdayLevel (w); 

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_WORKDAY_LEVEL,
										   w));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_WORKDAY_LEVEL,
											   m_owner, w));
		g_network.Unblock(m_owner);
	}
}

double Player::GetWorkdayPerPerson () 
{ 
	
	if (m_global_happiness)
		return m_global_happiness->GetWorkdayPerPerson (); 

	return 0.0;
}

double Player::GetUnitlessWorkday () 
{ 
	
	if (m_global_happiness)
		return m_global_happiness->GetUnitlessWorkday ();

	return 0.0;
}

void Player::SetWagesLevel (sint32 w) 
{ 
	m_global_happiness->SetWagesLevel (w);
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_WAGES_LEVEL,
										   w));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_WAGES_LEVEL,
											   m_owner, w));
		g_network.Unblock(m_owner);
	}
}



double Player::GetWagesPerPerson () { return m_global_happiness->GetWagesPerPerson (); }
double Player::GetUnitlessWages () { return m_global_happiness->GetUnitlessWages (); }

void Player::SetRationsLevel (sint32 w) 
{ 
	m_global_happiness->SetRationsLevel (w); 
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_RATIONS_LEVEL,
										   w));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_RATIONS_LEVEL,
											   m_owner, w));
		g_network.Unblock(m_owner);
	}
}
double Player::GetRationsPerPerson () const { return m_global_happiness->GetRationsPerPerson (); }
double Player::GetUnitlessRations () { return m_global_happiness->GetUnitlessRations (); }

sint32 Player::GetBaseContentment() const { return m_difficulty->GetBaseContentment(); }; 

double Player::GetBigCityScale() const { return m_difficulty->GetBigCityScale(); };
double Player::GetBigCityOffset() const { return m_difficulty->GetBigCityOffset(); };


double Player::GetPollutionDifficultyMultiplier() const
{
    return (m_difficulty->GetPollutionMultiplier()) ;
}

double Player::GetRiotChance() const
{
	return m_difficulty->GetRiotChance();
}

double Player::GetStarvationEffect() const
{
	return m_difficulty->GetStarvationEffect();
}


double Player::GetPositiveWorkdayCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetPositiveWorkdayCoef();
}

double Player::GetNegativeWorkdayCoef() const
{
	return g_theGovernmentDB->Get(m_government_type)->GetNegativeWorkdayCoef();
}

double Player::GetWorkdayExpectation() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetWorkdayExpectation();
}

double Player::GetPositiveWagesCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetPositiveWagesCoef();
}
 
double Player::GetNegativeWagesCoef() const
{
	return g_theGovernmentDB->Get(m_government_type)->GetNegativeWagesCoef();
}

double Player::GetWagesExpectation() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetWagesExpectation();
}
 
double Player::GetPositiveRationsCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetPositiveRationsCoef();
}
 
double Player::GetNegativeRationsCoef() const
{
	return g_theGovernmentDB->Get(m_government_type)->GetNegativeRationsCoef();
}

double Player::GetRationsExpectation() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetRationsExpectation();
}

double Player::GetRationsLevel() const
{
	return m_global_happiness->GetUnitlessRations();
}

double Player::GetKnowledgeCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetKnowledgeCoef(); 
}

double Player::GetPollutionCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetPollutionCoef();
}

double Player::GetPollutionUnhappyCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetPollutionUnhappyCoef();
}


double Player::GetConquestDistress() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetConquestDistress();
}

double Player::GetConquestDistressDecay() const
{
	return g_theGovernmentDB->Get(m_government_type)->GetConquestDistressDecay();
}

double Player::GetEmpireDistanceScale() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetEmpireDistanceScale();
}

double Player::GetMaxEmpireDistance() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetMaxEmpireDistance();
}


sint32 Player::GetMaxMartialLawUnits() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetMaxMartialLawUnits();
}

double Player::GetMartialLawEffect() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetMartialLawEffect(); 
}

sint32 Player::GetMartialLawThreshold() const 
{
    return g_theGovernmentDB->Get(m_government_type)->GetMartialLawThreshold(); 
}

double Player::GetAtHomeRadius() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetAtHomeRadius();
}
         
double Player::GetOverseasCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetOverseasCoef();
}
     

double Player::GetOverseasDefeatDecay() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetOverseasDefeatDecay();
}

double Player::GetOverseasDefeatCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetOverseasDefeatCoef();
}

double Player::GetHomeDefeatDecay() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetHomeDefeatDecay();
}

double Player::GetHomeDefeatCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetHomeDefeatCoef();
}


double Player::GetCrimeCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetCrimeCoef();
}
    
double Player::GetCrimeOffset () const
{
    return g_theGovernmentDB->Get(m_government_type)->GetCrimeOffset();
}


sint32 Player::GetProfessionalUnits() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetProfessionalUnits();
}


double Player::GetReadyPeaceCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetReadyPeaceCoef(); 
}

double Player::GetReadyPeaceHP() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetReadyPeaceHP();
}


double Player::GetReadyAlertCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetReadyAlertCoef(); 
}

double Player::GetReadyAlertHP() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetReadyAlertHP(); 
}


double Player::GetReadyWarCoef() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetReadyWarCoef();
}

double Player::GetReadyWarHP() const
{
    return g_theGovernmentDB->Get(m_government_type)->GetReadyWarHP();
}

sint32 Player::GetScienceHandicap()
{
	return m_difficulty->GetScienceHandicap();
}

void Player::SetCapitol(const Unit &c)
{
	
	if (m_capitol->m_id != (0)) {
		m_capitol->DestroyCapitol(); 
	} 
	*m_capitol = c; 
}

sint32 Player::GetCapitolPos(MapPoint &pos) const
{ 
    if (m_capitol->m_id == (0)) { 
		return FALSE; 
	} else { 
		if(!g_network.IsActive() || g_network.ReadyToStart()) {
			Assert(g_theUnitPool->IsValid(*m_capitol));
		}
        if(!g_theUnitPool->IsValid(*m_capitol)) {
			if(!g_network.IsActive() || g_network.ReadyToStart()) {
				m_capitol->m_id = (0);
			}
            return FALSE;
        }
        m_capitol->GetPos(pos); 
		return TRUE; 
	} 
}

Unit Player::CityIndexToUnit(sint32 index)
{ 
	return m_all_cities->Get(index); 
}

BOOL Player::SetGovernmentType(sint32 type)
{
	DPRINTF(k_DBG_GAMESTATE, ("Player %d set gov to %d\n", m_owner, type));
	if(type == m_government_type)
		
		return FALSE;

	Assert(type >= 0 && type < g_theGovernmentDB->NumRecords());
	if(type < 0 || type > g_theGovernmentDB->NumRecords())
		return FALSE;

    sint32 sci_id = g_theGovernmentDB->Get(type)->GetEnableAdvanceIndex();
    if (!HasAdvance(sci_id)) { 
        
        return FALSE; 
    } 

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_SET_GOVERNMENT, type));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_GOVERNMENT, m_owner, type));
		g_network.Unblock(m_owner);
	}
										   
	if(m_government_type == 0) {
		m_set_government_type = type;
		if(m_change_government_turn <= GetCurRound()) {
			return ActuallySetGovernment(type);
		}
		return TRUE;
	} else if(!m_changed_government_this_turn) {
		m_set_government_type = type;
		sint32 turns = g_rand->Next(g_theConstDB->MaxGovernmentChangeTurns()) + 1;
		m_change_government_turn = GetCurRound() + turns;
		return ActuallySetGovernment(0);
	} else {
		return ActuallySetGovernment(type);
	}
}

BOOL Player::ActuallySetGovernment(sint32 type)
{
	if(m_government_type == type) {
		return TRUE;
	}

	SlicObject *so;
    if (g_civApp->IsGameLoaded() && (type != 0) && !ScenarioEditor::IsShown()) {
        if (GetCurRound() > 50) {
            so = new SlicObject("012CivNewGov") ;
            so->AddAllRecipientsBut(m_owner);
            so->AddCivilisation(m_owner);
            so->AddGovernment(type);
            g_slicEngine->Execute(so) ;
        }

        so = new SlicObject("012NewGovEnacted") ;
        so->AddRecipient(m_owner);
        so->AddGovernment(type);
        g_slicEngine->Execute(so) ;
		if(m_owner == g_selected_item->GetVisiblePlayer() && type != 0) {
			g_director->AddGameSound(GAMESOUNDS_CHANGE_GOV);
		}
    }

	DPRINTF(k_DBG_GAMESTATE, ("Player[%d]::ActuallySetGovernment to %d.\n", m_owner, type));

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_ACTUALLY_SET_GOVERNMENT, type));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ACTUALLY_SET_GOVERNMENT, m_owner, type));
		g_network.Unblock(m_owner);
	}
										   
	sint32 i, n = m_all_cities->Num();

	for(i = 0; i < n; i++) {
		(*m_all_cities)[i].NewGovernment(type);
	}

	m_government_type = type;

	BOOL unit_disbanded = FALSE;
	for(i = m_all_units->Num() - 1; i >= 0; i--) {
		const UnitRecord *rec = m_all_units->Access(i).GetDBRec();
		if(rec->GetNumGovernmentType() < 1)
			continue;

		sint32 j;
		bool found = false;
		for(j = 0; j < rec->GetNumGovernmentType(); j++) {
			if(rec->GetGovernmentTypeIndex(j) == type) {
				found = true;
				break;
			}
		}
		if(!found) {
			m_all_units->Access(i).KillUnit(CAUSE_REMOVE_ARMY_GOVERNMENT_CHANGE, -1);
			unit_disbanded = TRUE;
		}
	}

	if (unit_disbanded) {
		so = new SlicObject("41UnitDisbandedOnGovChange");
		so->AddRecipient(m_owner);
		so->AddCivilisation(m_owner);
		so->AddGovernment(type);
		g_slicEngine->Execute(so) ;
	}		 

	sint32 p;
	for(p = 0; p < k_MAX_PLAYERS; p++) {
		if(!g_player[p])
			continue;
		for(i = 0; i < g_player[p]->m_all_cities->Num(); i++) {
			if(g_player[p]->m_all_cities->Access(i).IsConvertedTo() == m_owner) {
				g_player[p]->m_all_cities->Access(i).GetData()->GetCityData()->Unconvert(FALSE);
			}
		}
	}

	SetWorkdayLevel((sint32)GetWorkdayExpectation());
	SetWagesLevel((sint32)GetWagesExpectation());
	SetRationsLevel((sint32)GetRationsExpectation());
    
     
	 
    

	double s;

	if(m_government_type == 0) {
		m_tax_rate->GetScienceTaxRate(s);
		m_tax_rate->SetTaxRateBeforeAnarchy(s, m_owner);
	} else {
		
		s = g_theGovernmentDB->Get(m_government_type)->GetMaxScienceRate();
	}

	if(s > g_theGovernmentDB->Get(m_government_type)->GetMaxScienceRate()) {
		s = g_theGovernmentDB->Get(m_government_type)->GetMaxScienceRate();
	}
	m_tax_rate->SetTaxRates(s, m_owner);


	// recalc the military support costs under the new government
	m_readiness->RecalcCost();


	g_slicEngine->RunGovernmentChangedTriggers(m_owner);

	return TRUE;
}

void Player::GroupArmy(Army &army)
{
	Assert(FALSE);
	
}

#if 0
void Player::UngroupArmy(Army &army)
{
	sint32 i;
	for(i = army.Num() - 1; i > 0; i--) {
		
		
		
		Army newArmy = GetNewArmy(CAUSE_NEW_ARMY_UNGROUPING);
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AddUnitToArmy,
							   GEA_Unit, army[i],
							   GEA_Army, newArmy,
							   GEA_End);

		
		

		
		

		
		
		
		
	}
}
#endif

void Player::RegisterYourArmyWasMoved(const Army &i_moved, const MapPoint &new_pos)
{

    
	for(sint32 i = 0; i < m_all_armies->Num(); i++) {
        if(m_all_armies->Access(i).m_id == i_moved.m_id) {          
            
            
            return; 
        }	
	}
 
    return; 
}

void Player::AssasinateRuler()
{
	m_assasinationModifier = g_theConstDB->AssasinationHappinessEffect();
	m_assasinationTimer = g_theConstDB->AssasinationHappinessEffectTimer();
}

sint32 Player::GetTimedHappiness()
{
	return m_assasinationModifier;
}

void Player::EstablishEmbassy(sint32 player)
{
	m_embassies |= (1 << player);
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_EMBASSIES, m_owner, m_embassies));
	}
}

void Player::CloseEmbassy(sint32 player)
{
	m_embassies &= ~(1 << player);
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_EMBASSIES, m_owner, m_embassies));
	}
}





BOOL Player::HasEmbassyWith(sint32 player)
{

	
	
	
	if(!g_player[player])
		return FALSE;

	
	
	
	if(wonderutil_GetCloseEmbassies(g_player[player]->m_builtWonders))
		return FALSE;

	
	
	if(wonderutil_GetEmbassiesEverywhereEvenAtWar(m_builtWonders))
		return TRUE;

	
	if(wonderutil_GetEmbassy(m_builtWonders, g_player[player]->m_builtWonders)) {
		
		
		
		if (!AgreementMatrix::s_agreements.HasAgreement(player, m_owner, PROPOSAL_TREATY_DECLARE_WAR)) {
			return TRUE;
		}

	}

	return (m_embassies & (1 << player)) != 0;
}

sint32 Player::GetProductionFromFranchises()
{
	return m_productionFromFranchises;
}

void Player::AddProductionFromFranchise(sint32 amt)
{
	m_productionFromFranchises += amt;
	if(g_network.IsHost()) {
		
		
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_FRANCHISE_PRODUCTION,
									  m_owner, m_productionFromFranchises));
	}
}

#ifdef _DEBUG









void Player::DisplayAdvances()
	{
	MBCHAR	s[512] ;

	AdvanceType	adv ;

	sprintf(s, "Advances, Player %ld:", m_owner);
	g_debugWindow->AddText(s);

		for (adv=0; adv<m_advances->GetNum(); adv++)
			if (HasAdvance(adv))
				{
				sprintf(s, " %s", g_theAdvanceDB->GetNameStr(adv)) ;
				g_debugWindow->AddText(s);
				}

	}
#endif

void Player::DisplayWWR()
{
	g_debugWindow->AddText("WWR:");
    m_global_happiness->DisplayWWR(); 

    char str[80];
    sprintf(str, "  readiness level %d cost %d  %%  %3.1f",  m_readiness->GetLevel(),
        m_readiness->GetCost(), 
        100.0 * m_readiness->GetPecentLastTurn());
	g_debugWindow->AddText(str);
    sprintf(str, "  materials %% %3.1f materials current %d", 100.0 * m_materialsTax, m_materialPool->GetMaterials()); 
	g_debugWindow->AddText(str);
    double tmp; 
    m_tax_rate->GetScienceTaxRate(tmp);
    sprintf(str, "  science %% %3.1f science current %d", tmp, 100.0 * m_science->GetLevel()); 
	g_debugWindow->AddText(str);
    sprintf(str, "  gold %d", m_gold->GetLevel()); 
	g_debugWindow->AddText(str);

























} 














uint32 Player_Player_GetVersion(void)
	{
	return (k_PLAYER_VERSION_MAJOR<<16 | k_PLAYER_VERSION_MINOR) ;
	}


BOOL player_isAlly(PLAYER_INDEX me, PLAYER_INDEX him)
{
    return (g_player[me]->GetMaskAlliance() &  (0x01 << him)); 
}

BOOL player_isEnemy(PLAYER_INDEX me, PLAYER_INDEX him)
{
    
	return !AgreementMatrix::s_agreements.HasAgreement(me, him, PROPOSAL_TREATY_ALLIANCE);
}






sint32 Player::GetCheapestMilitaryUnit()
{
	sint32 i;
	sint32 cheapindex = -1;
	sint32 cheapcost = 0x7fffffff;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		const UnitRecord *rec = g_theUnitDB->Get(i);
		BOOL isObsolete = FALSE;
		sint32 o;
		for(o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
			if(HasAdvance(rec->GetObsoleteAdvanceIndex(o))) {
				isObsolete = TRUE;
				break;
			}
		}
		if(isObsolete && cheapindex >= 0)
			continue;
		if((rec->GetEnableAdvanceIndex() < 0 || HasAdvance(rec->GetEnableAdvanceIndex())) &&
		   rec->GetShieldCost() < cheapcost &&
		   rec->GetAttack() > 0) {
			cheapcost = rec->GetShieldCost();
			cheapindex = i;
		}
	}
	Assert(cheapindex >= 0);
	return cheapindex;
}


MBCHAR *Player::GenerateDescriptionString(int is_winner)
{
    char *ptag = NULL;
	sint32 curScore = infowin_GetCivScore(m_owner);

    if (is_winner) {
        if (curScore <= 20)        ptag = "PLAYER_DESCRIPTION_WINNER_RANK_1";
        else if (curScore <= 40)   ptag = "PLAYER_DESCRIPTION_WINNER_RANK_2";
        else if (curScore <= 60)   ptag = "PLAYER_DESCRIPTION_WINNER_RANK_3";
        else if (curScore <= 80)   ptag = "PLAYER_DESCRIPTION_WINNER_RANK_4";
        else if (curScore <= 100)  ptag = "PLAYER_DESCRIPTION_WINNER_RANK_5";
        else                       ptag = "PLAYER_DESCRIPTION_WINNER_RANK_6";
    } else {
        if (curScore <= 5)         ptag = "PLAYER_DESCRIPTION_LOSER_RANK_1";
        else if (curScore <= 10)   ptag = "PLAYER_DESCRIPTION_LOSER_RANK_2";
        else if (curScore <= 15)   ptag = "PLAYER_DESCRIPTION_LOSER_RANK_3";
        else if (curScore <= 20)   ptag = "PLAYER_DESCRIPTION_LOSER_RANK_4";
        else if (curScore <= 25)   ptag = "PLAYER_DESCRIPTION_LOSER_RANK_5";
        else                       ptag = "PLAYER_DESCRIPTION_LOSER_RANK_6";
    }

    SlicContext sc;
    sc.AddCivilisation(m_owner);
    stringutils_Interpret(g_theStringDB->GetNameStr(ptag), sc, m_descrip_string);

    return(m_descrip_string);
}

MBCHAR *Player::GetDescriptionString()
{
    if (m_descrip_string[0]) {
        
        return(m_descrip_string);
    } else {
        
        return(GenerateDescriptionString(TRUE));
    }
}

void Player::GameOver(GAME_OVER reason, sint32 data)
{
	if (PLAYER_INDEX_VANDALS == m_owner) { 
		return; 
	}

	BOOL previouslyWon = m_hasWonTheGame;
	BOOL previouslyLost = m_hasLostTheGame;

	sint32 i, aPlayer;
	sint32 count = 0;

	
	switch(reason) {
		case GAME_OVER_WON_CONQUERED_WORLD:
		case GAME_OVER_WON_SCENARIO:
		case GAME_OVER_WON_OUT_OF_TIME:
            GenerateDescriptionString(TRUE);
			m_hasWonTheGame = TRUE;
			break;
		case GAME_OVER_WON_DIPLOMACY:
            GenerateDescriptionString(TRUE);
			m_hasWonTheGame = TRUE;
			for(i = 1; i < k_MAX_PLAYERS; i++) {
				if(g_player[i] && !g_player[i]->m_isDead && i != m_owner) {
					
					
					
				    
				    
					if(!g_network.IsClient()) {
						
						g_player[i]->GameOver(GAME_OVER_LOST_DIPLOMACY, -1);
					}
				}
			}
			break;
		
		case GAME_OVER_WON_SCIENCE:
			if (!g_network.IsActive()) { 
				GenerateDescriptionString(TRUE);
				m_hasWonTheGame = TRUE;
				for(i = 1; i < k_MAX_PLAYERS; i++) {
					if(g_player[i] && !g_player[i]->m_isDead && i != m_owner) {
						
						
						
						
						
						if(!g_network.IsClient()) {
							
							g_player[i]->GameOver(GAME_OVER_LOST_SCIENCE, -1);
						}
					}
				}
			} 
			break;

		case GAME_OVER_LOST_CONQUERED:
		case GAME_OVER_LOST_INEPT:
		case GAME_OVER_LOST_SCENARIO:
            GenerateDescriptionString(FALSE);
			StartDeath(reason, data);
			for(i = 1; i < k_MAX_PLAYERS; i++) {
				if(g_player[i] && !g_player[i]->m_isDead && i != m_owner) {
					
					
					
					
					
					count++;
					aPlayer = i;
				}
			}
			if(count == 1) {
				if(!g_network.IsClient()) {
					
					g_player[aPlayer]->GameOver(GAME_OVER_WON_CONQUERED_WORLD, -1);
				}
			}
			m_hasLostTheGame = TRUE;
			break;
		case GAME_OVER_LOST_OUT_OF_TIME:
		case GAME_OVER_LOST_SCIENCE:
		case GAME_OVER_LOST_DIPLOMACY:
            GenerateDescriptionString(FALSE);
			m_hasLostTheGame = TRUE;
			break;

        default:
            Assert(false); 
            break;
	}

	if(g_network.IsHost()) {
		if(reason == GAME_OVER_WON_OUT_OF_TIME || reason == GAME_OVER_LOST_OUT_OF_TIME) {
			
		} else {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_GAME_OVER, reason, m_owner, data));
		}
	}

	
	if (m_playerType != PLAYER_TYPE_ROBOT && m_owner == g_selected_item->GetVisiblePlayer() &&
		(!g_slicEngine->GetTutorialActive() || g_slicEngine->GetTutorialPlayer() == m_owner)) 
	{
		close_AllScreens();

		
		
		if (reason == GAME_OVER_LOST_SCIENCE ||
			reason == GAME_OVER_LOST_DIPLOMACY) {
			
			infowin_Initialize();
			victorywin_Initialize(k_VICWIN_DEFEAT);
			victorywin_DisplayWindow(k_VICWIN_DEFEAT);
		} else {
			
			g_director->CatchUp();
			g_director->AddPlayVictoryMovie(reason, previouslyWon, previouslyLost);
		}


	}
}

BOOL Player::CheckPlayerDead()
{
	if(g_isCheatModeOn || ScenarioEditor::IsShown())
		return FALSE;

	if(m_all_cities->Num() <= 0 && (!m_first_city || m_all_units->Num () < 1)) {
		return TRUE;
	}
	return FALSE;
}

extern sint32 g_noai_stop_player;

void Player::StartDeath(GAME_OVER reason, sint32 data)
{
	if(m_isDead) {
		
		return;
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPlayer,
						   GEA_Player, m_owner,
						   GEA_Int, reason,
						   GEA_Player, data,
						   GEA_End);

	if(g_network.IsActive()) {
		g_network.KillPlayer(m_owner, reason, data);
	}

    
    SlicObject *so = new SlicObject("77YouLose") ;
    so->AddRecipient(m_owner) ;
	so->AddPlayer(m_owner);
    g_slicEngine->Execute(so) ;

    
    if (reason == GAME_OVER_LOST_CONQUERED && data != m_owner) {
        so = new SlicObject("76PlayerDefeatedBy") ;
        so->AddCivilisation(m_owner) ;
        so->AddCivilisation(data) ;
        so->AddAllRecipientsBut(m_owner);
        g_slicEngine->Execute(so) ;
		if(g_player[data]) {
			g_player[data]->m_score->AddOpponentConquered();
		}
    } else {
        so = new SlicObject("75PlayerDefeated") ;
        so->AddCivilisation(m_owner) ;
        so->AddAllRecipientsBut(m_owner);
        g_slicEngine->Execute(so) ;
    }

	m_isDead = TRUE; 
    g_aPlayerIsDead = TRUE; 

    
    if (g_theProfileDB->IsAIOn()) { 
        
        if (m_owner == NewTurnCount::GetStopPlayer()) { 
            NewTurnCount::SetStopPlayer(g_selected_item->GetPlayerAfterThis(m_owner));
        }
		
        
    } else {  
        g_noai_stop_player = g_selected_item->GetPlayerAfterThis(m_owner);
    } 

	
	
	if(m_terrainImprovements) {
		sint32 i, n = m_terrainImprovements->Num();
		for(i = n - 1; i >= 0; i--) {
			m_terrainImprovements->Access(i).Kill();
		}
	}

	if(m_agreed) {
		while(m_agreed->Num() > 0) {
			m_agreed->Access(0).Kill();
		}
	}

	g_featTracker->CheckConquerFeat(m_owner,data);



    
}



void Player::RemoveDeadPlayers()
{
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_REMOVE_DEAD_PLAYERS));
	}

	sint32 i, playersInGame = 0;
	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && g_player[i]->m_isDead) {

			
			

			
			
			
			g_theWorld->RegisterPlayerDead(i);

			sint32 j, k;
			
			for(j = 0; j < k_MAX_PLAYERS; j++) {
				if(!g_player[j] || j == i)
					continue;
				for(k = g_player[j]->m_all_cities->Num() - 1; k >= 0; k--) {
					CityData *cd = g_player[j]->m_all_cities->Access(k).GetData()->GetCityData();
					if(cd->GetFranchiseOwner() == i) {
						cd->SetFranchiseTurnsRemaining(0);
					}

					if(cd->IsConvertedTo() == i) {
						cd->Unconvert();
					}
				}
			}

			for(j = g_player[i]->m_all_armies->Num() - 1; j >= 0; j--) {
				for(k = g_player[i]->m_all_armies->Access(j).Num() - 1; k >= 0; k--) {
					g_player[i]->m_all_armies->Access(j)[k].Kill(CAUSE_REMOVE_ARMY_NO_MAT_SUPPORT, -1);
				}
			}	

#ifdef _DEBUG
			for(j = 0; j < g_player[i]->m_all_units->Num(); j++) {
				DPRINTF(k_DBG_GAMESTATE, ("Unit %lx still alive\n",
										  g_player[i]->m_all_units->Access(j)));
			}

			Assert(g_player[i]->m_all_units->Num() == 0);
			Assert(g_player[i]->m_all_armies->Num() == 0);
			if(g_player[i]->m_all_units->Num() != 0 ||
			   g_player[i]->m_all_armies->Num() != 0) {
				BOOL HeyYou_DontJustHitIGoTellJoe = FALSE;
				Assert(HeyYou_DontJustHitIGoTellJoe);
			}
#endif

			
			if(!g_deadPlayer) {
				g_deadPlayer = new PointerList<Player>;
			}
			g_deadPlayer->AddHead(g_player[i]);
			g_player[i]->m_score->SetFinalScore(g_player[i]->m_score->GetTotalScore());
			
			delete g_player[i]->m_vision;
			g_player[i]->m_vision = NULL;

			g_player[i] = NULL;
			g_selected_item->RemovePlayer((PLAYER_INDEX)i);
			g_turn->PlayerDead(i);

			
			CtpAi::RemovePlayer(i);
		}
	}
	g_aPlayerIsDead = FALSE;

	for (i=1; i < k_MAX_PLAYERS; i++) {
		if (g_player[i] && !g_player[i]->m_isDead)
			playersInGame++ ;
	}
}

Player *Player::GetDeadPlayer(sint32 index)
{
	PointerList<Player>::Walker walk(g_deadPlayer);
	while(walk.IsValid()) {
		if(walk.GetObj()->m_owner == index) {
			return walk.GetObj();
		}
		walk.Next();
	}
	return NULL;
}













sint32 Player::LoadBuildQueue(const sint32 city, const MBCHAR *file)
	{
	Unit	c ;

	CityData	*cityData ;

	Assert(city>=0) ;
	Assert(city<m_all_cities->Num()) ;
	Assert(file != NULL) ;
	Assert(file[0] != 0) ;

	c = CityIndexToUnit(city) ;
	cityData = c.GetData()->GetCityData() ;

	return (cityData->LoadQueue(file)) ;
	}














sint32 Player::SaveBuildQueue(const sint32 city, const MBCHAR *file)
	{
	Unit	c ;

	CityData	*cityData ;

	Assert(city>=0) ;
	Assert(city<m_all_cities->Num()) ;
	Assert(file != NULL) ;
	Assert(file[0] != 0) ;

	c = CityIndexToUnit(city) ;
	cityData = c.GetData()->GetCityData() ;

	return (cityData->SaveQueue(file)) ;
	}



sint32 Player::GetTotalResources()
{
	sint32 res = 0;
#ifdef CTP1_TRADE
	for(sint32 i = 0; i < m_all_cities->Num(); i++) {
		res += m_all_cities->Access(i).AccessData()->GetCityData()->GetResourceCount();
	}
#else
	static bool justAssertsOnce = false;
	Assert(justAssertsOnce);
	justAssertsOnce = true;
#endif
	return res;
}












void Player::BeginTurnMonopoly(void)
	{
#ifdef CTP1_TRADE
	SlicObject	*so ;
	
	sint32	p, i, j,
			cities,
			resourceCount = 0 ;

	cities = m_all_cities->Num() ;
	for (i=0; i<g_theResourceDB->NumRecords(); i++)
		{
		
		resourceCount = 0;	
		for (j=0; j<cities; j++) {
			resourceCount += m_all_cities->Access(j).AccessData()->GetCityData()->GetResourceCount(i) ;

			if (resourceCount > g_theConstDB->GetMonopolyThreshold())
				{
				so = new SlicObject("26MonopolyDetected") ;
				for (p=0; p<k_MAX_PLAYERS; p++)
					if(g_player[i] && (p != m_owner) && !g_player[i]->m_isDead)
						so->AddRecipient(p) ;

				so->AddCivilisation(m_owner) ;
				so->AddGood(i) ;
				so->AddCity(m_all_cities->Get(j)) ;
				g_slicEngine->Execute(so) ;
				
				break;
				}
			}
		}
#endif 
	}


BOOL Player::ContinentShared() const
{
	if(m_capitol->m_id == (0))
		return FALSE;
	MapPoint capitolPos;
	m_capitol->GetPos(capitolPos);

	return g_theWorld->IsContinentSharedWithOthers(capitolPos,
												   m_owner,
												   NULL);
}

void Player::CheckResourcesForTutorial() const
{
#ifdef CTP1_TRADE
	if(!g_slicEngine->SpecialSameGoodEnabled())
		return;

	sint32 i, j;
	for(i = 0; i < m_all_cities->Num(); i++) {
		for(j = i+1; j < m_all_cities->Num(); j++) {
			if(m_all_cities->Access(i).GetResources()->FindMatch(
				m_all_cities->Access(j).GetResources()) >= 0) {
				g_slicEngine->RunSameGoodTriggers(m_all_cities->Access(i),
												  m_all_cities->Access(j));
				return;
			}
		}
	}
#endif
}

sint32 Player::GetNumTradeRoutes() const
{
	sint32 routes = 0;

	for(sint32 i = 0; i < m_all_cities->Num(); i++) {
		routes += m_all_cities->Access(i).GetNumTradeRoutes();
	}
	return routes;
}

BOOL Player::HasSameGoodAsTraded() const
{
#ifdef CTP1_TRADE
	sint32 i, j, k;
	Resources tradeResources;
	Resources localResources;

	for(i = 0; i < m_all_cities->Num(); i++) {
		m_all_cities->Access(i).AddTradedResources(tradeResources);
		for(j = i+1; j < m_all_cities->Num(); j++) {
			m_all_cities->Access(j).LocalResources(localResources);
			for(k = 0; k < g_theResourceDB->NumRecords(); k++) {
				if(localResources.GetResourceCount(k) > 0 &&
				   tradeResources.GetResourceCount(k) > 0) {
					*m_slic_special_city = m_all_cities->Access(j);
					return TRUE;
				}
			}
		}
	}
#endif
	return FALSE;
}



sint32 Player::GetTradeWith(PLAYER_INDEX third_party)
{
	sint32 i;
	sint32 amount = 0;
	for(i = 0; i < m_all_cities->Num(); i++) {
		amount += m_all_cities->Access(i).CountTradeWith(third_party);
	}
	return amount;
}

double Player::GetPercentProductionToMilitary() const
{
	if(m_total_production == 0)
		return 0.0;

	return m_readiness->GetCost() / m_total_production;
}


void Player::SetAttitude(PLAYER_INDEX player, ATTITUDE_TYPE attitude)
	{
	Assert((player>=0) && (player<k_MAX_PLAYERS)) ;
	Assert((attitude>=ATTITUDE_TYPE_MIN) & (attitude<=ATTITUDE_TYPE_MAX)) ;
	
#ifdef _DEBUG_INCOMPATIBLE
	m_attitude[player] = attitude ;
#endif
	}


DIPLOMATIC_STRENGTH Player::GetRelativeStrength(PLAYER_INDEX him) const 
{
	
	if(!g_player[him]) {
		return DIPLOMATIC_STRENGTH_VERY_STRONG;
	}

    double my_str=0.0; 
    sint32 turn = GetCurRound();

    my_str += m_strengths->GetTurnStrength(STRENGTH_CAT_UNITS,turn);
	my_str += m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD,turn);
	my_str += m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS,turn);
	my_str += m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS,turn);
	my_str += m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION,turn);

    double his_str=0.0; 
    his_str += g_player[him]->m_strengths->GetTurnStrength(STRENGTH_CAT_UNITS,turn);
	his_str += g_player[him]->m_strengths->GetTurnStrength(STRENGTH_CAT_GOLD,turn);
	his_str += g_player[him]->m_strengths->GetTurnStrength(STRENGTH_CAT_BUILDINGS,turn);
	his_str += g_player[him]->m_strengths->GetTurnStrength(STRENGTH_CAT_WONDERS,turn);
	his_str += g_player[him]->m_strengths->GetTurnStrength(STRENGTH_CAT_PRODUCTION,turn);

    if ((2.0 * my_str) < his_str) {
        return DIPLOMATIC_STRENGTH_VERY_WEAK;
    } else if ((1.5 * my_str) < his_str) {
        return DIPLOMATIC_STRENGTH_WEAK;
    } else if (my_str > (2.0 * his_str)) {
        return DIPLOMATIC_STRENGTH_VERY_STRONG;
    } else if (my_str > (1.5 * his_str)) {
        return DIPLOMATIC_STRENGTH_STRONG;
    } else {
        return DIPLOMATIC_STRENGTH_AVERAGE;
    }
}

ATTITUDE_TYPE Player::GetAttitude(PLAYER_INDEX him) const
{

    Assert(m_owner != him); 
    if (m_owner == him) return ATTITUDE_TYPE_NEUTRAL;

    Assert(0 <= him); 
    Assert(him < k_MAX_PLAYERS); 
    Assert(g_player[him]); 

    BOOL is_weak = FALSE;
    BOOL is_strong = FALSE; 

    
    switch (GetRelativeStrength(him)) { 
    case DIPLOMATIC_STRENGTH_VERY_WEAK:
    case DIPLOMATIC_STRENGTH_WEAK: 
        is_weak = TRUE; 
        break; 
    case DIPLOMATIC_STRENGTH_STRONG:
    case DIPLOMATIC_STRENGTH_VERY_STRONG:
        is_strong = TRUE; 
        break; 
    default:
        break;
    } 
   
	if(m_playerType != PLAYER_TYPE_ROBOT || g_network.IsClient()) {
		switch(m_diplomatic_state[him]) {
			case DIPLOMATIC_STATE_WAR:
				if(is_weak) {
					return ATTITUDE_TYPE_WEAK_HOSTILE;
				} else {
					return ATTITUDE_TYPE_STRONG_HOSTILE;
				}
			case DIPLOMATIC_STATE_ALLIED:
				if(is_weak) {
					return ATTITUDE_TYPE_WEAK_FRIENDLY;
				} else {
					return ATTITUDE_TYPE_STRONG_FRIENDLY;
				}
			default:
				return ATTITUDE_TYPE_NEUTRAL;
		}
	}










  
    switch (m_regard->GetUpdatedRegard(m_owner, him)) { 
    case REGARD_TYPE_LOVE: 
        if (is_weak) {
            return(ATTITUDE_TYPE_WEAK_FRIENDLY);
        } else { 
            return(ATTITUDE_TYPE_STRONG_FRIENDLY);
        }
    case REGARD_TYPE_FRIENDLY:
        if (is_weak) {
            return(ATTITUDE_TYPE_WEAK_FRIENDLY);
        } else if (is_strong) {   
            return(ATTITUDE_TYPE_STRONG_FRIENDLY);
        } else { 
            return(ATTITUDE_TYPE_NEUTRAL);
        }
    case REGARD_TYPE_NEUTRAL:    
        return(ATTITUDE_TYPE_NEUTRAL);

    case REGARD_TYPE_COLDWAR:
        if (is_weak) {
            return(ATTITUDE_TYPE_WEAK_HOSTILE);
        } else if (is_strong) {             
            return(ATTITUDE_TYPE_STRONG_HOSTILE);
        } else { 
            return(ATTITUDE_TYPE_NEUTRAL);
        }
    case REGARD_TYPE_HOTWAR: 
    case REGARD_TYPE_INSANE_HATRED: 
        if (is_weak) {
            return(ATTITUDE_TYPE_WEAK_HOSTILE);
        } else {             
            return(ATTITUDE_TYPE_STRONG_HOSTILE);
        }
    default:
        Assert(0); 
        return(ATTITUDE_TYPE_NEUTRAL);
    }
}

sint32 Player::GetKnowledgeStrength() const
{
	
	return m_advances->GetCostOfWhatHeKnows();
}

sint32 Player::GetMilitaryStrength() const
{
	sint32 totalCost = 0;
	for(sint32 i = 0; i < m_all_units->Num(); i++) {
		if(m_all_units->Access(i).GetAttack() > 0)
			totalCost += m_all_units->Access(i).GetShieldCost();
	}
	return totalCost / 2;
}

sint32 Player::GetTradeStrength() const
{
	
	

	return MapAnalysis::GetMapAnalysis().GetTotalTrade(m_owner);
}

sint32 Player::GetTotalPopulation() const
{
	sint32 totalPop = 0;
	for(sint32 i = 0; i < m_all_cities->Num(); i++) {
		totalPop += m_all_cities->Access(i).PopCount();
	}
	return totalPop;
}

sint32 Player::GetPartialPopulation() const
{
	sint32 partialPop = 0;
	for(sint32 i = 0; i < m_all_cities->Num(); i++) {
		partialPop += m_all_cities->Access(i).GetCityData()->GetPartialPopulation();
	}
	return partialPop;
}


sint32 Player::GetLandArea() const
{
	return Cell::PlayerLandArea(m_owner);
}

sint32 Player::GetSpaceStrength() const
{
	return 0;
}

sint32 Player::GetUnderseaStrength() const
{
	return 0;
}

sint32 Player::GetRank(STRENGTH_CAT category) const
{
	sint16 rank = 1;
	sint16 total = 0;
	sint32 maxStrength = 0;
	sint32 myStrength = m_strengths->GetStrength(category);
	sint32 minStrength = myStrength;
	sint32 hisStrength;
	for(sint32 p = 1; p < k_MAX_PLAYERS; p++) {
		if (g_player[p]) {
			total++;
			hisStrength = g_player[p]->m_strengths->GetStrength(category);
			if (hisStrength > myStrength) {
				rank++;
			}
			if (hisStrength > maxStrength)
				maxStrength = hisStrength;
			if (hisStrength < minStrength)
				minStrength = hisStrength;
		}
	}

	
	if (maxStrength <= minStrength)
		return 100;

	float relativeRank = ((float)myStrength - minStrength) /
		((float)maxStrength - minStrength);
	return (sint32) (relativeRank * 100.0);
}

void Player::MakeCaptureCityPriority(const Unit &city)
{
	
}

void Player::MakeCaptureRegionPriority(const struct PSlicComplexRegion *region)
{
}

void Player::MakeLeaveRegionPriority(const struct PSlicComplexRegion *region)
{
}

void Player::Surrender()
{
}

sint32 Player::GetCurRound() const
{
	return m_current_round;
}

BOOL Player::IsTurnOver() const
{
	return m_is_turn_over;
}

void Player::BuildDiplomaticSlicMessage(DiplomaticRequest &r)
{
	SlicObject *so = new SlicObject(r.GetRequestString());
	so->AddRecipient(r.GetRecipient());
	so->AddCivilisation(r.GetOwner());
	so->AddCivilisation(r.GetRecipient());
	so->AddAttitude(g_player[r.GetOwner()]->GetAttitude(r.GetRecipient()));

	switch(r.GetRequest()) {
		case REQUEST_TYPE_GREETING: 
			break;
		case REQUEST_TYPE_DEMAND_ADVANCE: 
			so->AddAdvance(r.GetAdvance());
			break;
		case REQUEST_TYPE_DEMAND_CITY:
			so->AddCity(r.GetTarget());
			break;
		case REQUEST_TYPE_DEMAND_MAP: 
			break;
		case REQUEST_TYPE_DEMAND_GOLD:
			so->AddGold(r.GetGold().GetLevel());
			break;
		case REQUEST_TYPE_DEMAND_STOP_TRADE:
			so->AddCivilisation(r.GetThirdParty());
			break;
		case REQUEST_TYPE_DEMAND_ATTACK_ENEMY:
			so->AddCivilisation(r.GetThirdParty());
			break;
		case REQUEST_TYPE_DEMAND_LEAVE_OUR_LANDS:
			break;
		case REQUEST_TYPE_DEMAND_REDUCE_POLLUTION:
			break;
		case REQUEST_TYPE_OFFER_ADVANCE:
			so->AddAdvance(r.GetAdvance());
			break;
		case REQUEST_TYPE_OFFER_CITY:
			so->AddCity(r.GetTarget());
			break;
		case REQUEST_TYPE_OFFER_MAP:
			break;
		case REQUEST_TYPE_OFFER_GOLD:
			so->AddGold(r.GetGold().GetLevel());
			break;
		case REQUEST_TYPE_OFFER_CEASE_FIRE:
			break;
		case REQUEST_TYPE_OFFER_PERMANENT_ALLIANCE:
			break;
		case REQUEST_TYPE_OFFER_PACT_CAPTURE_CITY:
			so->AddCity(r.GetTarget());
			so->AddCivilisation(r.GetTarget().GetOwner());
			break;
		case REQUEST_TYPE_OFFER_PACT_END_POLLUTION :
			break;
		case REQUEST_TYPE_EXCHANGE_ADVANCE:
			so->AddAdvance(r.GetAdvance());
			so->AddAdvance(r.GetWantedAdvance());
			break;
		case REQUEST_TYPE_EXCHANGE_CITY:
			so->AddCity(r.GetTarget());
			so->AddCity(r.GetWantedCity());
			break;
		case REQUEST_TYPE_EXCHANGE_MAP:
			break;
		case REQUEST_TYPE_DEMAND_NO_PIRACY:
			break;
		default:
			Assert(FALSE);
			break;
	}
	g_slicEngine->Execute(so);

}

sint32 Player::GetPoints() const
{
	return m_powerPoints;
}

sint32 Player::DeductPoints(sint32 p)
{
	Assert(m_powerPoints >= p);
	if(m_powerPoints >= p)
		m_powerPoints -= p;
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_POWER_POINTS,
									  m_owner, m_powerPoints));
	}
	return m_powerPoints;
}

sint32 Player::AddPoints(sint32 p)
{
	m_powerPoints += p;
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_POWER_POINTS,
									  m_owner, m_powerPoints));
	}
	return m_powerPoints;
}

sint32 Player::SetPoints(sint32 p)
{
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_POWER_POINTS,
									  m_owner, m_powerPoints));
	}
	return(m_powerPoints = p);
}

void Player::TradeUnitsForPoints(const MapPoint &pnt)
{
	if(!(g_network.IsActive() && g_network.SetupMode()) &&
	   !g_powerPointsMode)
		return;
	   
	if(m_all_armies->Num() + m_all_cities->Num() < 2)
		
		return;

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_SELL_UNITS,
										   pnt.x, pnt.y));
		return;
	}

	Cell *cell = g_theWorld->GetCell(pnt);
	if(cell->GetNumUnits() <= 0) {
		if(cell->GetCity().m_id != (0)) {
			TradeCityForPoints(cell->GetCity());
		}
		return;
	}

	if(cell->AccessUnit(0).GetOwner() != m_owner)
		return;

	sint32 i;
	for(i = cell->GetNumUnits() - 1; i >= 0; i--) {
		TradeUnitForPoints(cell->AccessUnit(i));
	}
}

void Player::TradeUnitForPoints(Unit &unit)
{
	if(!(g_network.IsActive() && g_network.SetupMode()) &&
	   !g_powerPointsMode)
		return;
	   
	if(m_all_armies->Num() + m_all_cities->Num() < 2)
		return;

	Assert(unit.GetOwner() == m_owner);
	if(unit.GetOwner() != m_owner)
		return;

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_SELL_ONE_UNIT,
										   (uint32)unit));
		return;
	}

	sint32 points = unit.GetDBRec()->GetPowerPoints();
	AddPoints(points);
	unit.Kill(CAUSE_REMOVE_ARMY_CHEAT, -1);
}

void Player::TradeCityForPoints(Unit &city)
{
	if(!(g_network.IsActive() && g_network.SetupMode()) &&
	   !g_powerPointsMode)
		return;
	   
	if(m_all_armies->Num() + m_all_cities->Num() < 2)
		return;

	
	TradeUnitForPoints(city);
}

void Player::TradeImprovementsForPoints(const MapPoint &pnt)
{
	if(!(g_network.IsActive() && g_network.SetupMode()) &&
	   !g_powerPointsMode)
		return;
	   
	Cell *cell = g_theWorld->GetCell(pnt);
	if(!cell->GetNumImprovements())
		return;

	if(cell->AccessImprovement(0).GetOwner() != m_owner)
		return;

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_SELL_IMPROVEMENTS,
										   pnt.x, pnt.y));
		return;
	}

	sint32 i;

	for(i = cell->GetNumImprovements() - 1; i >= 0; i--) {
		sint32 materialCost = cell->AccessImprovement(i).GetMaterialCost();
		sint32 pointsBack = sint32(double(materialCost) * g_theConstDB->PowerPointsToMaterialsConversion());
		AddPoints(pointsBack);
		cell->AccessImprovement(i).Kill();
	}
}


void Player::RegisterAttack(PLAYER_INDEX against)
{
	m_last_attacked[against] = GetCurRound();

#if 0
	sint32 i;
	DiplomaticRequest req;
	for(i = m_messages->Num() - 1; i >= 0; i--) {		
		req = m_messages->Access(i).GetDiplomaticRequest();
		if(g_theDiplomaticRequestPool->IsValid(req)) {
			if(req.GetOwner() == against) {
				m_messages->Access(i).Reject();
			}
		}
	}

	for(i = g_player[against]->m_messages->Num() - 1; i >= 0; i--) {
		req = g_player[against]->m_messages->Access(i).GetDiplomaticRequest();
		if(g_theDiplomaticRequestPool->IsValid(req)) {
			if(req.GetOwner() == m_owner) {
				g_player[against]->m_messages->Access(i).Reject();
			}
		}
	}
#endif

	
	
	
	
}

void Player::ContactMade(PLAYER_INDEX with)
{
	if(with == m_owner)
		return;

	if(!(m_contactedPlayers & (1 << with))) {
		m_contactedPlayers |= (1 << with);
		Assert(g_player[with]);
		if(g_player[with]) {
			
			
			if(g_player[with]->m_contactedPlayers & (1 << m_owner)) {

				
				if (with != 0 && m_owner != 0)
				{
					
					if (g_selected_item->GetVisiblePlayer() == m_owner &&
						m_playerType != PLAYER_TYPE_ROBOT) {
						Diplomat::GetDiplomat(with).SendGreeting(m_owner);
					}
					else if (g_selected_item->GetVisiblePlayer() == with &&
						g_player[with]->GetPlayerType() != PLAYER_TYPE_ROBOT) {
						Diplomat::GetDiplomat(m_owner).SendGreeting(with);
					}
				}

				
				
				
				


			}
		}

		if(g_selected_item && g_radarMap && m_owner == g_selected_item->GetVisiblePlayer()) {
			g_radarMap->Update();
		}
	}
}







BOOL Player::HasContactWith(PLAYER_INDEX pl)
{
	Assert(pl >= 0);
	Assert(pl < k_MAX_PLAYERS);
	
	
	
	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return FALSE;

	
	
	
	if(!g_player[pl])
		return FALSE;

	
	
	if(g_player[pl] && g_player[pl]->IsDead())
		return FALSE;

    
	
	if(pl == m_owner) {
		return TRUE;
	}

	
	
	
	
	
	if(HasEmbassyWith(pl)) {
		return TRUE;
	}

	
	
	
	return ((m_contactedPlayers & (1 << pl)) != 0) &&
		((g_player[pl]->m_contactedPlayers & (1 << m_owner)) != 0);
}


BOOL Player::HasSeen(PLAYER_INDEX pl)
{
	Assert(pl >= 0);
	Assert(pl < k_MAX_PLAYERS);
	if(pl < 0 || pl >= k_MAX_PLAYERS)
		return FALSE;

	if(!g_player[pl])
		return FALSE;

	if(pl == m_owner) {
		return TRUE;
	}

	
	

	
	
		
		
	
	
	return (m_contactedPlayers & (1 << pl)) != 0;
}
	
void Player::CheckWonderObsoletions(AdvanceType advance)
{
    sint32 i, o;
    sint32 player_idx; 
    SlicObject *so;

    
    for(i = 0; i < g_theWonderDB->NumRecords(); i++) {
        const WonderRecord *wrec = wonderutil_Get(i);

        
        BOOL is_obsolete = FALSE;
        for(o = 0; o < wrec->GetNumObsoleteAdvance(); o++) {
            if(wrec->GetObsoleteAdvanceIndex(o) == advance) {
                is_obsolete = TRUE;
            }
        }
        if (!is_obsolete)
            continue;

        // Wonder is obsolete, check if someone is building this wonder
		// if the wonder is not already build
        if(!wonderutil_IsBuilt(i)) {
            for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) { 
                if (g_player[player_idx] == NULL) continue;
                if (player_idx == m_owner) continue; 
                
                if (g_theWonderTracker->IsBuildingWonder(i, player_idx) &&
                    (GetCurRound() > 1)) {

                    // Send the player a message, that the wonder he builds is obsolete
                    so = new SlicObject("097aWonderObsolete");
                    so->AddRecipient(player_idx);
                    so->AddAdvance(advance);
                    so->AddWonder(i);
                    g_slicEngine->Execute(so);
                }
            } 

            continue;
        }

        // check if the wonder owner is valid
        sint32 wowner = wonderutil_GetOwner(i);
        if ((wowner < 0) || (wowner >= k_MAX_PLAYERS) ||
            (g_player[wowner] == NULL) ||
            (g_player[wowner]->m_isDead)) {
            continue;
        }

        // recalculate values that could be changed by the
		// now obsolete wonder

		// Trade Route Costs
        if(wrec->GetFreeTradeRoutes()) {
            g_player[wowner]->ReconsiderCostOfTrade();
        }
		
        if(wrec->GetReduceReadinessCost()) {
            g_player[wowner]->m_readiness->RecalcCost();
        }

		
        
        
		
        
        

		if(wrec->GetGlobalRadar()) {
			g_player[wowner]->m_hasGlobalRadar = FALSE;
			g_theWonderTracker->SetGlobeSatFlags(g_theWonderTracker->GlobeSatFlags() & ~(1 << m_owner));
			m_vision->ClearUnseen();
			if(wowner == g_selected_item->GetVisiblePlayer()) {
				g_director->AddCopyVision();
			}
		}

        
        if (GetCurRound() > 1) {

            if(g_network.IsHost() && !g_network.IsLocalPlayer(wowner)) {
                g_network.QueuePacket(g_network.IndexToId(wowner),
                                      new NetInfo(NET_INFO_CODE_WONDER_OBSOLETE,
                                                  advance, i));
            }

            SlicObject *so = new SlicObject("097aWonderObsolete");
            so->AddRecipient(wowner);
            so->AddAdvance(advance);
            so->AddWonder(i);
            g_slicEngine->Execute(so);
        }
    }
}    

void Player::SetHasAdvance(AdvanceType advance)
{
	sint32 i;
    BOOL im_the_first = TRUE;

    
	for(i=1; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && (!g_player[i]->m_isDead) && (i != m_owner) &&
           (g_player[i]->HasAdvance(advance))) {
            im_the_first = FALSE;
            break;
        }
	}

	sint32 gaiaWonder = wonderutil_GetGaiaIndex();
	sint32 foo;
	if(gaiaWonder < 0 || gaiaWonder >= g_theWonderDB->NumRecords()) {
		foo = -1;
	} else {
		foo = g_theWonderDB->Get(gaiaWonder)->GetEnableAdvanceIndex();
	}

	if(advance == foo) {
		
		
		SlicObject *so = new SlicObject("GCDiscoveredSolarisProjectUs");
		so->AddRecipient(m_owner);
		so->AddPlayer(m_owner);
		g_slicEngine->Execute(so);
		for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i] && i != m_owner) {
				SlicObject *so = new SlicObject("GCDiscoveredSolarisProjectThem");
				so->AddRecipient(i);
				so->AddPlayer(m_owner);
				g_slicEngine->Execute(so);
			}
		}
	}
	
	const AdvanceRecord *advRec = g_theAdvanceDB->Get(advance);

    
    if (im_the_first && !ScenarioEditor::IsGivingAdvances()) {
        CheckWonderObsoletions(advance);

		sint32 feat;
		if(!g_network.IsNetworkLaunch() && g_player[g_selected_item->GetCurPlayer()] && advRec->GetTriggerFeatIndex(feat)) {
			if(!g_network.IsActive() || g_network.ReadyToStart()) {
				g_featTracker->AddFeat(feat, m_owner);
			}
		}
    }

	
	for(i = 0; i < m_all_cities->Num(); i++) {
		m_all_cities->Access(i).NotifyAdvance(advance);
	}

	if (g_controlPanel!=NULL && m_owner == g_selected_item->GetVisiblePlayer())
		g_controlPanel->TileImpPanelRedisplay();

    
	sint32 player_idx; 
	sint32 city_idx, city_num; 
	for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) { 
		if (g_player[player_idx] == NULL) continue;
		if (player_idx == m_owner) continue; 

		city_num = g_player[player_idx]->m_all_cities->Num(); 
		for (city_idx=0; city_idx<city_num; city_idx++) { 
			g_player[player_idx]->m_all_cities->Access(city_idx).GetData()->GetCityData()->GetBuildQueue()->RemoveIllegalItems(TRUE);
		} 
	} 

	if(g_civApp->IsGameLoaded() && !ScenarioEditor::IsGivingAdvances()) {
		
		if (GetCurRound() > 1) {
			if(!g_network.IsClient() || g_network.ReadyToStart()) {
				SpecialDiscoveryNotices(advance);
				GovernmentDiscoveryNotices(advance);
				
				
			}
		}

		if(advance == m_researchGoal) {
			m_researchGoal = -1;
		}

		if(advance == m_advances->GetResearching()) {
			m_advances->ResetCanResearch(advance);
			sint32 i;
			if(m_researchGoal >= 0) {
				StartResearchingAdvanceForGoal(m_researchGoal);
			} else {
				for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
					if(m_advances->CanResearch(i)) {
						m_advances->SetResearching(i);
						break;
					}
				}
			}
			if(g_network.IsHost() && !g_network.IsLocalPlayer(m_owner)) {
				g_network.QueuePacketToAll(new NetResearch(m_advances));
				g_network.QueuePacket(g_network.IndexToId(m_owner),
									  new NetInfo(NET_INFO_CODE_CHOOSE_RESEARCH, advance));
			}
			if(!g_network.IsActive() || (g_network.IsHost() && g_network.IsLocalPlayer(m_owner))) {
				BuildResearchDialog(advance);
			}
		}
		if(sci_advancescreen_isOnScreen() &&
		   m_owner == g_selected_item->GetVisiblePlayer()) {
			sci_advancescreen_loadList();
		}
	}

	if(advRec->GetAgeIndex() > m_age) {
		
		
		sint32 count = 0;
		for(i = 0; i < g_theAdvanceDB->NumRecords(); i++) {
			if(g_theAdvanceDB->Get(i)->GetAgeIndex() == advRec->GetAgeIndex() &&
			   m_advances->HasAdvance(i)) {
				count++;
			}
		}
		if(count >= advRec->GetAge()->GetNeedAdvances()) {
			EnterNewAge(advRec->GetAgeIndex());
		}
	}

	SetCityRoads();
}


void Player::GiveArmyCommand(Army &army,
							 UNIT_COMMAND command)
{
	Assert(g_theArmyPool->IsValid(army));
	if(!g_theArmyPool->IsValid(army))
		return;

	Assert(army.GetOwner() == m_owner);
	if(army.GetOwner() != m_owner)
		return;

	Unit aUnit = army.Access(0);
	MapPoint apos;
	army.GetPos(apos);
	Unit aCity;
	MapPoint cpos;
	double dist;

	switch(command) {
		case UNIT_COMMAND_NEAREST_CITY:
			GetNearestCity(apos, aCity, dist);
			if(aCity.m_id != (0)) {
				aCity.GetPos(cpos);
				g_selected_item->EnterMovePath(m_owner, army,
											   apos, cpos);
			}
			break;
		case UNIT_COMMAND_NEAREST_FORT:
			if(GetNearestFort(apos, cpos)) {
				g_selected_item->EnterMovePath(m_owner, army,
											   apos, cpos);
			}
			break;
		case UNIT_COMMAND_NEAREST_AIRFIELD:
			if(GetNearestAirfield(apos, cpos)) {
				g_selected_item->EnterMovePath(m_owner, army,
											   apos, cpos);
			}
			break;
		case UNIT_COMMAND_ENTRENCH:
			
			army.AddOrders(UNIT_ORDER_ENTRENCH);
			break;
		case UNIT_COMMAND_SLEEP:
			
			army.AddOrders(UNIT_ORDER_SLEEP);
			break;
	}
}

void Player::Emancipate()
{
	sint32 i, n = m_all_cities->Num();
	sint32 numFreed = 0;
	for(i = 0; i < n; i++) {
		numFreed = m_all_cities->Access(i).FreeSlaves();
		m_all_cities->Access(i).GetData()->GetCityData()->
			GetBuildQueue()->RemoveIllegalItems();
		if(numFreed > 0) {
			m_all_cities->Access(i).AccessData()->GetCityData()->GetHappy()->AddTimer(
				g_theConstDB->EmancipationUnhappinessTurns(),
				g_theConstDB->EmancipationUnhappinessAmount(),
				HAPPY_REASON_EMANCIPATION);
		}
	}

	for(i = m_all_units->Num() - 1; i >= 0; i--) {
		const UnitRecord *rec = m_all_units->Access(i).GetDBRec();
		if(rec->GetSlaveRaids() || rec->GetSettlerSlaveRaids() ||
		   rec->GetSlaveUprising()) {
			m_all_units->Access(i).Kill(CAUSE_REMOVE_ARMY_EMANCIPATION, -1);
		}
	}
}

void Player::AddEndGameObject(Unit &city, sint32 type)
{
	
	

	
	
	

	
	
	
	

}

void Player::MakeConvertedCitiesUnhappy(sint32 convertedTo)
{
	sint32 i, n = m_all_cities->Num();
	for(i = 0; i < n; i++) {
		m_all_cities->Access(i).AddConversionUnhappiness(convertedTo);
	}
}

BOOL Player::CanBuildInfrastructure() const { return m_can_build_infrastructure; }
BOOL Player::CanBuildCapitalization() const { return m_can_build_capitalization; }

//----------------------------------------------------------------------------
//
// Name       : Player::CanBuildUnit
//
// Description: Checks whether the player can build the unit specified by type.
//
// Parameters : type: The unit type for that is checked whether the city can 
//              build it.
//
// Globals    : g_player:     The list of players
//              g_theUnitDB:  The unit database
//              g_slicEngine: The slic engine
//              G_TheWonderTracker: The list of wonders
//
// Returns    : Whether the player can build the unit specified by type.
//
// Remark(s)  : CultureOnly added by E; checks if a player can build a unit 
//              by comparing the unit's CultureOnly flag to the player's 
//              citystyle.
//
//-----------------------------------------------------------------------------
BOOL Player::CanBuildUnit(const sint32 type) const
{
	const UnitRecord *rec = g_theUnitDB->Get(type);

	if (!HasAdvance(rec->GetEnableAdvanceIndex()))
		return FALSE;

	sint32 o;
	for(o = rec->GetNumObsoleteAdvance() - 1; o >= 0; o--) {
		if(HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
			return FALSE;
	}

	if(g_exclusions->IsUnitExcluded(type))
		return FALSE;

	if(rec->GetCantBuild()) {
		
		return FALSE;
	}

	if(rec->GetNumGovernmentType() > 0) {
		sint32 i;
		bool found = false;
		for(i = 0; i < rec->GetNumGovernmentType(); i++) {
			if(rec->GetGovernmentTypeIndex(i) == m_government_type) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

// Added by E - Compares Unit CultureOnly to the Player's CityStyle
	if(rec->GetNumCultureOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCultureOnly(); s++) {
			if(rec->GetCultureOnlyIndex(s) == GetCivilisation()->GetCityStyle()) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}


	if(rec->GetNuclearAttack() &&
	   wonderutil_GetNukesEliminated(g_theWonderTracker->GetBuiltWonders())) {
		return FALSE;
	}

	if(rec->GetSlaveRaids() || rec->GetSettlerSlaveRaids() ||
	   rec->GetSlaveUprising()) {
		if(wonderutil_GetFreeSlaves(g_theWonderTracker->GetBuiltWonders())) {
			return FALSE;
		}
	}

	if(rec->GetSlaveRaids() || rec->GetSettlerSlaveRaids()) {
		sint32 i, n = m_all_units->Num();
		for(i = 0; i < n; i++) {
			if(m_all_units->Access(i).GetDBRec()->GetNoSlaves())
				return FALSE;
		}
	}

	if(rec->GetNoSlaves()) {
		
		sint32 i, n = m_all_cities->Num();
		for(i = 0; i < n; i++) {
			if(m_all_cities->Access(i).CountSlaves() > 0)
				return FALSE;
		}

		n = g_player[m_owner]->m_all_units->Num();
		for(i = 0; i < n; i++) {
			if(m_all_units->Access(i).GetDBRec()->GetSlaveRaids())
				return FALSE;
		}
	}

	if(rec->GetCreateParks() && 
	   !wonderutil_GetParkRangersEnabled(g_theWonderTracker->GetBuiltWonders())) {
		return FALSE;
	}

	
	return TRUE;
}

void Player::RemoveEmptyCities(CAUSE_REMOVE_ARMY cause)
{
	sint32 i;
	for(i = m_all_cities->Num() - 1; i >= 0; i--) {
		if(m_all_cities->Access(i).PopCount() < 1) {
			Unit *city = &(m_all_cities->Access(i));

			if(!g_theUnitPool->IsValid(*city))
				continue;

			if (cause == CAUSE_REMOVE_ARMY_FLOOD) {
				SlicObject *so = new SlicObject("04CitiesKilledByCalamity");
				so->AddRecipient(m_owner);
				so->AddCity(*city);
				g_slicEngine->Execute(so);
			}

			city->Kill(cause, -1);
		}
	}
}

sint32 Player::GetLastSalePrice(sint32 good)
{
	Assert(good >= 0);
	Assert(good < g_theResourceDB->NumRecords());
	if(good >= 0 && good < g_theResourceDB->NumRecords())
		return m_goodSalePrices[good];
	return 0;
}

void Player::SetLastSalePrice(sint32 good, sint32 price)
{
	Assert(good >= 0);
	Assert(good < g_theResourceDB->NumRecords());
	if(good >= 0 && good < g_theResourceDB->NumRecords())
		m_goodSalePrices[good] = price;
}

sint32 Player::GetLastAttacked(PLAYER_INDEX player)
{
	return m_last_attacked[player];
}

BOOL Player::CanStillSendRequestsTo(PLAYER_INDEX otherPlayer)
{
	return m_sent_requests_this_turn[otherPlayer] < g_theConstDB->MaxRequestsPerPlayerPerTurn();
}

void Player::IncrementSentRequests(PLAYER_INDEX otherPlayer)
{
	m_sent_requests_this_turn[otherPlayer]++;
}


void Player::ReconsiderCostOfTrade()
{
	const TradeDynamicArray *allRoutes = g_theTradePool->AccessAllRoutes();
	sint32 i;
	
	m_usedTradeTransportPoints = 0;
	for(i = allRoutes->Num() - 1; i >= 0; i--) {
		if(allRoutes->Access(i).GetPayingFor() == m_owner) {
			if(m_tradeTransportPoints - m_usedTradeTransportPoints <= 0) {
				allRoutes->Access(i).DontAdjustPointsWhenKilled();
				allRoutes->Access(i).Kill(CAUSE_KILL_TRADE_ROUTE_NO_MORE_CARAVANS);
			} else {
				m_usedTradeTransportPoints++;
			}
		}
	}
}

void Player::RecoveredProbe(const Unit &city)
{
	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_RECOVERED_PROBE,
									  m_owner, city.m_id));
		g_network.Unblock(m_owner);
	}

	
	m_advances->AddAlienLifeAdvance();
	
	

	
	SlicObject *so = new SlicObject("306EndGameRecoveredProbe");
	so->AddRecipient(m_owner);
	
	g_slicEngine->Execute(so);
	
}

void Player::RecreateMessageIcons()
{
	DynamicArray<Message> messages = *m_messages;
	sint32 m;
	for(m = 0; m < messages.Num(); m++) {
		Message msg = messages.Access(m);
		
		if(g_theMessagePool->IsValid(msg)) {
			if(m_playerType != PLAYER_TYPE_ROBOT) {				
				if(messages.Access(m).IsAlertBox()) {
					
					
					
					if(!messagewin_IsModalMessageDisplayed()) {
						messagewin_CreateModalMessage(messages.Access(m));
					}
				} else {
					messagewin_CreateMessage(messages.Access(m));
					
					
					
				}
			}
		} else {
			m_messages->Del(msg);
		}
	}

	if(g_controlPanel && m_owner == g_selected_item->GetVisiblePlayer()) {
		g_controlPanel->PopulateMessageList(m_owner);
	}
}

void Player::SetDiplomaticState(const PLAYER_INDEX p, const DIPLOMATIC_STATE s)
{ 
	Assert(0 <= p); 
	Assert(p < k_MAX_PLAYERS); 
	if(p < 0 || p >= k_MAX_PLAYERS)
		return;

	DIPLOMATIC_STATE realState = s;
	if(p == 0 || m_owner == 0) {
		
		realState = DIPLOMATIC_STATE_WAR;
	} else if(s != DIPLOMATIC_STATE_WAR &&
			  g_player[p]->GetDiplomaticState(m_owner) == DIPLOMATIC_STATE_WAR &&
			  GetDiplomaticState(p) == DIPLOMATIC_STATE_WAR) {
		SlicObject *so = new SlicObject("401WarOver");
		so->AddCivilisation(m_owner);
		so->AddCivilisation(p);
		so->AddAllRecipients();
		g_slicEngine->Execute(so);
	}
	
	Assert(g_player[p]);
	if(g_player[p]) {
		m_diplomatic_state[p] = realState;
		if(s == DIPLOMATIC_STATE_WAR) {
			m_embassies &= ~(1 << p);
		}
	}

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_DIP_STATE,
									  m_owner, p, realState));
		g_network.Unblock(m_owner);
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_SET_DIP_STATE,
										   p, realState));
	}
} 
void Player::ThisMeansWAR(PLAYER_INDEX defense_owner)
{
	Assert(0 <= defense_owner); 
    Assert(defense_owner < k_MAX_PLAYERS);
    Assert(m_owner != defense_owner); 
    Assert(g_player[m_owner]); 
    Assert(g_player[defense_owner]); 

    PLAYER_INDEX attack_owner = m_owner;

   
    
    
    
    
    

	if ((attack_owner != 0) && (defense_owner != 0) && 
		!AgreementMatrix::s_agreements.HasAgreement(attack_owner, defense_owner, PROPOSAL_TREATY_DECLARE_WAR)) {
        SlicObject *so = new SlicObject("128CivStartedWar");
        so->AddCivilisation(attack_owner);
        so->AddCivilisation(defense_owner);
        so->AddAllRecipientsBut(attack_owner);
        g_slicEngine->Execute(so);
    }        

    
	sint32 oldBrokenAlliances = m_broken_alliances_and_cease_fires;
	
	
	Diplomat::GetDiplomat(m_owner).DeclareWar(defense_owner);

	
    
    
    
    
    
    

    
    g_player[m_owner]->RegisterAttack(defense_owner);
    g_player[defense_owner]->RegisterAttack(m_owner);

	
	
	
	


}

void player_ActivateSpaceButton(sint32 owner)
{
	g_player[owner]->m_can_use_space_button = TRUE;

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_ACTIVATE_SPACE_BUTTON,
									  owner));
	}
}


void Player::ResetVision()
{
	
	m_vision->SetTheWholeWorldUnseen();
	sint32 j;

	for(j = 0; j < m_all_units->Num(); j++) {
		if(m_all_units->Access(j).Flag(k_UDF_VISION_ADDED)) {
			BOOL revealed;
			m_all_units->Access(j).ClearFlag(k_UDF_VISION_ADDED);
			m_all_units->Access(j).AddUnitVision(revealed);
		}
	}
	
	for(j = 0; j < m_all_cities->Num(); j++) {
		if(m_all_cities->Access(j).Flag(k_UDF_VISION_ADDED)) {
			BOOL revealed;
			m_all_cities->Access(j).ClearFlag(k_UDF_VISION_ADDED);
			m_all_cities->Access(j).AddUnitVision(revealed);
			m_all_cities->Access(j).CD()->AdjustSizeIndices();
		}
	}

	for(j = 0; j < m_allInstallations->Num(); j++) {
		MapPoint pos;
		Installation inst = m_allInstallations->Access(j);
		if(!g_theInstallationPool->IsValid(inst.m_id)) {
			Assert(FALSE);
			continue;
		}
		
		inst.GetPos(pos);
		double vision_range = 
			terrainutil_GetVisionRange(inst.GetType(), pos);
		if(vision_range <= 0)
			continue;
		BOOL revealed;
		AddUnitVision(pos, vision_range, revealed);
	}
}

sint32 Player::CountCityHappiness(sint32 &rioting, sint32 &content, sint32 &happy)
{
	sint32 i;
	rioting = content = happy = 0;
	for(i = 0; i < m_all_cities->Num(); i++) {
		if(m_all_cities->Access(i).CD()->GetIsRioting()) {
			rioting++;
		} else if(m_all_cities->Access(i).CD()->IsCelebratingHappiness()) {
			happy++;
		} else {
			content++;
		}
	}
	return m_all_cities->Num();
}



sint16 Player::GetCargoCapacity() const
{
	return m_cargoCapacity;
}


void Player::AddCargoCapacity(const sint16 delta_cargo_slots)
{
	m_cargoCapacity += delta_cargo_slots;
	
}

	
GaiaController *Player::GetGaiaController()
{
	return m_gaiaController;
}

void Player::EnterNewAge(sint32 age)
{
	m_age = age;
	const AgeRecord *rec = g_theAgeDB->Get(age);
	if(!rec) return;

	if(!g_network.IsNetworkLaunch() && g_player[g_selected_item->GetCurPlayer()]) {
		if(!g_network.IsActive() || g_network.ReadyToStart()) {
			
			SlicObject *so = new SlicObject((char *)rec->GetSlicObject());
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
		}
	}
	sint32 i;
	for(i = 0; i < m_all_cities->Num(); i++) {
		m_all_cities->Access(i).CD()->UpdateSprite();
	}
	if(!g_network.IsNetworkLaunch() && g_player[g_selected_item->GetCurPlayer()]) {
		g_eventTracker->AddEvent(EVENT_TYPE_AGES,m_owner,NewTurnCount::GetCurrentRound(),age);
	}
}

//Added by Martin Gühmann

//----------------------------------------------------------------------------
//
// Name       : SetResearchGoal
//
// Description: Sets the research goal of a player given the item has an 
//              enabling advance and that advance is not researched, yet.
//
// Parameters : enum DATABASE db - An database enum
//              sint32 index - An index in the according database
//
// Globals    : -
//
// Returns    : Whether the research goal was set, cleared or the item
//              has an enabling advance that can be researched.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Player::SetResearchGoal(enum DATABASE db, sint32 index)
{
	//Added by Martin Gühmann
	//Clears reseach goal if index is smaller 0:
	if(index < 0){
		m_researchGoal = -1;
		return 2;
	}

	sint32 i;
	sint32 tmpCosts;
	sint32 tmpAdvance;
	sint32 advance;

	switch(db) {
		case DATABASE_DEFAULT:
		case DATABASE_SEARCH:
		case DATABASE_ORDERS:
		case DATABASE_RESOURCE:
		case DATABASE_CONCEPTS:
		case DATABASE_TERRAIN:
			
			return 2;
		case DATABASE_UNITS:
			advance = g_theUnitDB->Get(index)->GetEnableAdvanceIndex();
			break;
		case DATABASE_BUILDINGS:
			advance = g_theBuildingDB->Get(index)->GetEnableAdvanceIndex();
			break;
		case DATABASE_WONDERS:
			advance = g_theWonderDB->Get(index)->GetEnableAdvanceIndex();
			break;
		case DATABASE_ADVANCES:
			advance = index; 
			break;
		case DATABASE_GOVERNMENTS:
			advance = g_theGovernmentDB->Get(index)->GetEnableAdvanceIndex();
			break;
		case DATABASE_TILE_IMPROVEMENTS:
			advance = g_theTerrainImprovementDB->Get(index)->GetTerrainEffect(0)->GetEnableAdvanceIndex();
			tmpCosts = m_advances->GetCost(advance);
			for(i = 1; i < g_theTerrainImprovementDB->Get(index)->GetNumTerrainEffect(); i++){
				tmpAdvance = g_theTerrainImprovementDB->Get(index)->GetTerrainEffect(i)->GetEnableAdvanceIndex();
				if(!m_advances->HasAdvance(tmpAdvance)){
					if(tmpCosts > m_advances->GetCost(tmpAdvance)){
						tmpCosts = m_advances->GetCost(tmpAdvance);
						advance = tmpAdvance;
					}
				}
			}
			break;
		default:
			Assert(FALSE);
			return 2;
	}

	if(m_advances->HasAdvance(advance)){
		//Clears research goal if the advance is already known.
		m_researchGoal = -1;
		return 0;
	}

	StartResearchingAdvanceForGoal(advance);

	return 1;
}

void Player::StartResearchingAdvanceForGoal(sint32 goal)
{
	m_researchGoal = goal;

	if(m_advances->CanResearch(goal)) {
		
		StartResearching(goal);
		return;
	}

	RecursivelyStartResearching(goal);
}

bool Player::RecursivelyStartResearching(sint32 advance)
{
	

	const AdvanceRecord *rec = g_theAdvanceDB->Get(advance);
	
	if(m_advances->CanResearch(advance)) {
		StartResearching(advance);
		return true;
	}

	sint32 i;
	sint32 a_prereq = -1;
	for(i = 0; i < rec->GetNumPrerequisites(); i++) {
		if(rec->GetPrerequisitesIndex(i) == rec->GetIndex())
			continue;

		if(m_advances->HasAdvance(rec->GetPrerequisitesIndex(i)))
			continue;
		if(m_advances->CanResearch(rec->GetPrerequisitesIndex(i))) {
			StartResearching(rec->GetPrerequisitesIndex(i));
			return true;
		}
		a_prereq = rec->GetPrerequisitesIndex(i);
	}

	
	if(a_prereq >= 0) {
		return RecursivelyStartResearching(a_prereq);
	}

	
	return false;
}

void Player::SetPlayerType(PLAYER_TYPE pt)
{ 
	m_playerType = pt; 
	if(g_network.IsHost()) {
		if(pt == PLAYER_TYPE_ROBOT) {
			if(m_owner != g_network.GetPlayerIndex()) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_ATTACH_ROBOT, m_owner));
			} else {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_DETACH_ROBOT, m_owner));
			}
		} else {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_DETACH_ROBOT, m_owner));
		}
	}

}
