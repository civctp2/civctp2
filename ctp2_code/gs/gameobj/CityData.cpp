//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City data
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
// - Generate debug version when set.
//
// _SLOW_BUT_SAFE
// - Define 2 other symbols (PROJECTED_CHECK_START and PROJECTED_CHECK_END) 
//   when set. But the defined symbols are never used, so this doesn't do
//   anything at all. This makes preprocessing and compilation slower, but
//   should be safe.
//
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// USE_LOGGING
// - Enable logging when set, even when not a debug version. This is not
//   original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fix sea city sprite bug. (7.11.2003) by NelsonAndBronte. 
// - Make sure cities created by the scenario editor keep their size and 
//   style. (9.11.2003) by Martin Gühmann.
// - Enable log generation for the non-debug versions.
// - Enable reading of files created with the Activision 1.1 patch.
// - Prevent crash when settling in the Alexander scenario.
// - Can't rush buy capitalization/infrastructure
// - Made the utilisation ratio function available for the tile improvement 
//   placement governor.
//
//----------------------------------------------------------------------------


#include "c3.h"
#include "c3errors.h"

#include "DB.h"
#include "BuildingRecord.h"
#include "TerrainRecord.h"
#include "StrDB.h"
#include "AdvanceRecord.h"
#include "ConstDB.h"
#include "CivilisationDB.h"

#include "CellUnitList.h"
#include "UnitRec.h"
#include "UnitPool.h"
#include "CityData.h"
#include "XY_Coordinates.h"
#include "World.h"

#include "Player.h"
#include "RandGen.h"

#include "network.h"
#include "net_info.h"
#include "net_action.h"

#include "CivArchive.h"
#include "CheckSum.h"
#include "SelItem.h"
#include "pixelutils.h"
#include "WonderRecord.h"
#include "Pollution.h"
#include "TradeRoute.h"
#include "Gold.h"
#include "Cell.h"
#include "TileVal.h"
#include "installationtree.h"
#include "CivilisationPool.h"
#include "AICause.h"
#include "Director.h"
#include "MessagePool.h"
#include "Advances.h"
#include "SlicSegment.h"
#include "TerrainRecord.h"
#include "TopTen.h"
#include "Happy.h"
#include "HappyTracker.h"
#include "TurnCnt.h"
#include "UnoccupiedTiles.h"


#include "ProfileDB.h"
#include "TaxRate.h"

#include "ArmyPool.h"
#include "ArmyData.h"
#include "Score.h"
#include "EndGameDB.h"
#include "Exclusions.h"
#include "WonderTracker.h"
#include "MaterialPool.h"

#include "ColorSet.h"
#include "TiledMap.h"

#include "TradeOffer.h"

#include "Readiness.h"


#include "c3window.h"
#include "statswindow.h"

#include "Order.h"



#include "DiffDB.h"
extern DifficultyDB *g_theDifficultyDB;

extern ColorSet *g_colorSet;

#ifdef _DEBUG
#include "AiCause.h"
#endif

#include "UnitActor.h"

#include "SoundManager.h"
extern SoundManager		*g_soundManager;

#include "GameSettings.h"
#include "TradeOfferPool.h"

#include "Agreement.h"
#include "AgreementTypes.h"

#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"

#include "GovernmentRecord.h"
#include "UnitData.h"
#include "advanceutil.h"
#include "ResourceRecord.h"

#include "terrainutil.h"

#include "GameEventManager.h"

#include "unitutil.h"

#include "SlicEngine.h"
#include "SlicObject.h"

#include "Globals.h"

#include "CityInfluenceIterator.h"
#include "CitySizeRecord.h"

#include "PopRecord.h"

#include "buildingutil.h"
#include "wonderutil.h"

#include "FeatTracker.h"

#include "CityStyleRecord.h"
#include "AgeCityStyleRecord.h"
#include "TradePool.h"

#include "GameFile.h"

#if !defined(ACTIVISION_ORIGINAL)
//Added by Martin Gühmann to handle 
//city creation by Scenario Editor properly
#include "ScenarioEditor.h"
#include <algorithm>				// std::min
#endif

extern void player_ActivateSpaceButton(sint32 pl);

#if !defined(ACTIVISION_ORIGINAL)
static uint8 const	UNKNOWN_DEFAULT	= 0;
#endif

class HackCityArchive : public CivArchive
{
public:
	void SetStoreOn() { CivArchive::SetStore(); }
	uint8 *GetStr() { return GetStream(); }
	uint32 StreamLen() { return CivArchive::StreamLen(); }
};


#ifdef _SLOW_BUT_SAFE

#define PROJECTED_CHECK_START \
	uint32 crc1[4], crc2[4];\
	HackCityArchive * archive;\
	CheckSum *check;\
	archive = new HackCityArchive;\
	check = new CheckSum;\
	archive->SetStoreOn();\
	Serialize(*archive);\
	check->AddData(archive->GetStr(), archive->StreamLen());\
	check->Done(crc1[0], crc1[1], crc1[2], crc1[3]);\
	delete archive;\
	delete check;

#define PROJECTED_CHECK_END \
	archive = new HackCityArchive;\
	check = new CheckSum;\
	archive->SetStoreOn();\
	Serialize(*archive);\
	check->AddData(archive->GetStr(), archive->StreamLen());\
	check->Done(crc2[0], crc2[1], crc2[2], crc2[3]);\
    Assert(crc2[0] == crc1[0] &&\
		   crc2[1] == crc1[1] &&\
		   crc2[2] == crc1[2] &&\
		   crc2[3] == crc1[3]); \
	delete archive;\
	delete check;
#else 
#define PROJECTED_CHECK_START
#define PROJECTED_CHECK_END
#endif

extern TopTen *g_theTopTen ;
extern World *g_theWorld; 
extern Player **g_player; 
extern UnitPool *g_theUnitPool; 
extern StringDB *g_theStringDB;
extern RandomGenerator *g_rand; 
extern ConstDB *g_theConstDB ;
extern SelectedItem *g_selected_item;
extern Director *g_director;

extern Pollution *g_thePollution ;

extern TurnCount *g_turn;
extern ProfileDB	*g_theProfileDB ;

CityData::CityData(PLAYER_INDEX o, Unit hc, const MapPoint &center_point)
#if !defined(ACTIVISION_ORIGINAL)                       
:	m_cityStyle(CITY_STYLE_GENERIC),
	m_unknown_from_patch(UNKNOWN_DEFAULT)
#endif
{ 
	sint32 i;

	m_name[0] = 0;

    m_owner = o;
	m_founder = o;
    m_home_city = hc; 
    m_slaveBits = 0; 
	m_accumulated_food = 0;
    m_shieldstore = 0;
	m_shieldstore_at_begin_turn = 0;
	m_shields_this_turn = 0 ;
    m_trade = 0;
    m_science = 0; 
    m_luxury = 0; 
	m_isInjoined = FALSE;
	m_capturedThisTurn = FALSE ;

    m_city_attitude = CITY_ATTITUDE_CONTENT;

	m_build_queue.SetOwner(m_owner);
	m_build_queue.SetCity(m_home_city);
    m_built_improvements = 0; 
	m_builtWonders = 0;
	m_total_pollution = 0;
	m_cityPopulationPollution=0 ;
	m_cityIndustrialPollution=0 ;
	m_cityPollutionCleaner=0 ;
	m_foodVatPollution = 0 ;

	m_contribute_materials = TRUE;
    m_contribute_military = TRUE;

	m_spied_upon = 0;
	m_walls_nullified = FALSE;
	m_franchise_owner = -1;
	m_franchiseTurnsRemaining = -1;

	m_growth_rate=0;

#ifdef _DEBUG
    m_ignore_happiness = FALSE; 
#endif
	m_watchfulTurns = -1;
	m_bioInfectionTurns = -1;
	m_nanoInfectionTurns = -1;
	m_convertedTo = -1;
	m_convertedBy = CONVERTED_BY_NOTHING;
	m_terrainWasPolluted=FALSE ;									
	m_happinessAttacked=FALSE ;										
	m_terrainImprovementWasBuilt=FALSE ;							
	m_improvementWasBuilt=FALSE ;									

	m_is_rioting = FALSE;

    m_gross_trade = 0; 
    m_trade_lost_to_crime = 0 ; 
    m_shields_lost_to_crime = 0; 
    m_goldFromTradeRoutes = 0; 
	m_goldLostToPiracy = 0;
    m_collected_production_this_turn = 0; 
    m_gross_production_this_turn = 0; 
    m_food_delta = 0.0; 
    m_gross_food_this_turn = 0.0; 
    m_food_produced_this_turn = 10.0; 
    m_food_lost_to_crime = 0.0; 
    m_food_consumed_this_turn = 0.0; 
    m_convertedGold = 0; 
    m_injoinedBy = 0; 
    m_killList = NULL; 
    m_radiusNewOwner  = 0; 
    m_cityRadiusOp = RADIUS_OP_UKNOWN; 
    m_bioInfectedBy = -1; 
    m_nanoInfectedBy = -1; 

	m_happy = new Happy;
	
	g_theWorld->SetCapitolDistanceDirtyFlags(1<<o);

	m_airportLastUsed = -1;

	m_wages_paid = 0;

	m_buildInfrastructure = FALSE;
	m_buildCapitalization = FALSE;
	m_gold_from_capitalization = 0;
	m_pw_from_infrastructure = 0;

#if defined(ACTIVISION_ORIGINAL)
	m_cityStyle = g_player[m_owner]->m_civilisation->GetCityStyle();
#else
	// Set the style of the founder of the city - if any.
	if (g_player[o] && g_player[o]->GetCivilisation())
	{
		m_cityStyle = g_player[o]->GetCivilisation()->GetCityStyle();
	}
#endif

	m_doUprising = UPRISING_CAUSE_NONE;

	Assert(g_turn);
	m_turnFounded = g_turn->GetRound();

	m_productionLostToFranchise = 0;

	m_build_category_at_begin_turn = -4;
	m_lastCelebrationMsg = -1;
	m_alreadySoldABuilding = FALSE;
	m_sentInefficientMessageAlready = FALSE;


	m_population = 0;
	m_partialPopulation = 0;
	for(i = 0; i < POP_MAX; i++) {
		m_numSpecialists[i] = 0;
	}

 	
 	m_useGovernor = FALSE;
 	m_buildListSequenceIndex = 0;
 	m_garrisonOtherCities = FALSE;
 	m_garrisonComplete = FALSE;
	m_currentGarrison = 0;
	m_neededGarrison = 0;
	m_currentGarrisonStrength = 0.0;
	m_neededGarrisonStrength = 0.0;


	m_sizeIndex = 0;

	m_sellBuilding = -1;
	m_buyFront = false;
	m_paidForBuyFront = false;

	m_workerPartialUtilizationIndex = 0;
	m_workerFullUtilizationIndex = 0;

	ResetStarvationTurns();

	m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];

	m_pos = center_point;

	m_defensiveBonus= 0;
}

CityData::~CityData()
{

	
	if (!m_home_city.IsValid() || m_home_city.CD() == this)
	{
		DPRINTF(k_DBG_GAMESTATE, ("Killing City %lx\n", uint32(m_home_city)));
		sint32 i;

		for(i = 0; i < m_tradeSourceList.Num(); i++) {
			m_tradeSourceList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_DIED);
		}
		
		for(i = 0; i < m_tradeDestinationList.Num(); i++) {
			m_tradeDestinationList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_DIED);
		}
	}

	if(m_happy)
		delete m_happy;

	if(m_distanceToGood) {
		delete m_distanceToGood;
		m_distanceToGood = NULL;
	}
}












void CityData::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	if (archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_is_rioting)+sizeof(m_is_rioting));

	} else
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_is_rioting)+sizeof(m_is_rioting));

#if !defined(ACTIVISION_ORIGINAL)
	// Create and read files in the format as created with the Activision 1.1 patch.
	if (archive.IsStoring())
	{
		archive << m_unknown_from_patch;
	}
	else
	{
		if (g_saveFileVersion >= 66) 
		{
			archive >> m_unknown_from_patch;
			Assert(UNKNOWN_DEFAULT == m_unknown_from_patch);
		}
		else
		{
			m_unknown_from_patch = UNKNOWN_DEFAULT;
		}
	}
#endif
    m_home_city.Serialize(archive) ;
	m_build_queue.Serialize(archive) ;
	m_tradeSourceList.Serialize(archive);
	m_tradeDestinationList.Serialize(archive);

	// To check later: next 3 lines are invalid when CTP1_TRADE has been defined. See CityData.h.
	m_collectingResources.Serialize(archive);
	m_buyingResources.Serialize(archive);
	m_sellingResources.Serialize(archive);

    m_happy->Serialize(archive); 

	sint32 len;
	if(archive.IsStoring()) {
		len = strlen(m_name) + 1;
		archive << len;
		archive.Store((uint8*)m_name, len * sizeof(MBCHAR));
		
		archive.Store((uint8*)m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
		archive.Store((uint8*)&m_defensiveBonus, sizeof(double));
	} else {
		archive >> len;
		archive.Load((uint8*)m_name, len * sizeof(MBCHAR));
		archive.Load((uint8*)m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
		archive.Load((uint8*)&m_defensiveBonus, sizeof(double));
	}

#if defined(ACTIVISION_ORIGINAL) // Invalid values are handled by GetCityStyle.
	if(m_cityStyle < 0 || m_cityStyle >= g_theCityStyleDB->NumRecords()) {
		
		m_cityStyle = 0;
	}
#endif
}

BOOL NeedsCanalTunnel(MapPoint &center_point)

{ 
    MapPoint pos;

    if (g_theWorld->IsCanal(center_point)) { 
        return FALSE; 
    } else if (g_theWorld->IsTunnel(center_point)) { 
        return FALSE; 
    } 

	const TerrainRecord *rec = g_theTerrainDB->Get(g_theWorld->GetTerrainType(center_point));
	if(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater()) {
        return TRUE;
	} else {
        return FALSE;
	}
}

void CityData::Initialize(sint32 settlerType)
{
	MapPoint center_point;
	m_home_city.GetPos(center_point);
    if (g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.AddCreatedObject(m_home_city.AccessData());
	}


	FindBestSpecialists();	

	sint32 martialLaw;
	m_happy->CalcHappiness(*this, FALSE, martialLaw, TRUE);

	
	const UnitRecord *settlerRec;
	sint32 numPops = 1;
	if(settlerType >= 0) {
		settlerRec = g_theUnitDB->Get(settlerType);
		if(settlerRec)
			numPops = settlerRec->GetSettleSize();
		
	} else {
		settlerRec = NULL;
#if !defined(ACTIVISION_ORIGINAL)
		//Added by Martin Gühmann to make sure that also cities
		//created by the Scenario editor have a size
		if(settlerType == -2 && ScenarioEditor::PlaceCityMode() && ScenarioEditor::CitySize() > 0)
			numPops = ScenarioEditor::CitySize();
#endif
	}

	sint32 i;
#if defined(ACTIVISION_ORIGINAL)
	for(i = 0; i < numPops; i++) {
		
		if (settlerType != -2)
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
							   GEA_City, m_home_city,
							   GEA_End);
	}
#else
	//Added by Martin Gühmann to make sure that also cities created by the editor
	//have a size.
	if((settlerType != -2) || ScenarioEditor::PlaceCityMode())
	{
		for(i = 0; i < numPops; i++) {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
								   GEA_City, m_home_city,
								   GEA_End);
		}
	}
#endif
	
	if(settlerRec && settlerRec->GetNumSettleBuilding() > 0) {
		for(i = 0; i < settlerRec->GetNumSettleBuilding(); i++) {
			sint32 bi = settlerRec->GetSettleBuildingIndex(i);
			Assert(bi >= 0);
			Assert(bi < g_theBuildingDB->NumRecords());
			if(bi >= 0 && bi < g_theBuildingDB->NumRecords()) {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateBuilding,
									   GEA_City, m_home_city.m_id,
									   GEA_Int, bi,
									   GEA_End);
			}
		}
	}

	
	Cell *cell = g_theWorld->GetCell(center_point);
	if(cell->GetEnv() & k_BIT_ENV_INSTALLATION) {
		static DynamicArray<Installation> instArray;
		instArray.Clear();
		if(g_theInstallationTree->GetAt(center_point, instArray)) {
			sint32 i;
			for(i = instArray.Num() - 1; i >= 0; i--) {
				instArray[i].Kill();
			}
		}
	}
	
	
	
	sint32 good;
	if(g_theWorld->GetGood(center_point, good)) {
		m_collectingResources.AddResource(good);
	}
	
	cell->SetEnv(cell->GetEnv() & ~(k_MASK_ENV_IRRIGATION |
									k_MASK_ENV_MINE |
									k_MASK_ENV_ROAD |
									k_MASK_ENV_INSTALLATION));
	SetRoad();
	if (NeedsCanalTunnel(center_point)) { 
		g_theWorld->SetCanalTunnel(center_point, TRUE); 
	}
	
	if (g_theWorld->IsWater(center_point)) { 
		g_theWorld->NumberContinents(); 
	} else if (g_theWorld->IsNextToWater(center_point.x, center_point.y)) { 
		g_theWorld->NumberContinents();                 
	} 
	
	
	g_tiledMap->PostProcessTile(center_point, g_theWorld->GetTileInfo(center_point));
	g_tiledMap->TileChanged(center_point);
	MapPoint pos;
	for(WORLD_DIRECTION d = NORTH; d < NOWHERE; d = (WORLD_DIRECTION)((sint32)d + 1)) {
		if(center_point.GetNeighborPosition(d, pos)) {
			g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
			g_tiledMap->TileChanged(pos);
		}
	}
	g_tiledMap->RedrawTile(&center_point);
	
	g_network.Enqueue(g_theWorld->GetCell(center_point), 
					  center_point.x, center_point.y);
	

	MBCHAR s[k_MAX_NAME_LEN];
	sint32 name;
	Civilisation *civ = g_player[m_owner]->GetCivilisation() ;
	CivilisationData *civData = g_theCivilisationPool->AccessData(*civ) ;
	if (g_player[m_owner]->GetNumCities() == 0)	{ 
		name = civData->GetCapitalName() ;							


	}
	else
		name = civData->GetAnyCityName() ;							

	m_cityStyle = civ->GetCityStyle();
#if !defined(ACTIVISION_ORIGINAL)
	//Added by Martin Gühmann to make sure that cities created 
	//by the scenario editor keep their style
	if (settlerType == -2 && ScenarioEditor::PlaceCityMode())
	{
		m_cityStyle = ScenarioEditor::CityStyle();
	}
#endif

	if (name != k_CITY_NAME_UNDEFINED)	{							
		civData->GetCityName(name, s) ;								
		civData->UseCityName(name) ;								
		SetName(s) ;										
	} else {
		
		
	
		SetName(GetName()) ;							
	}

	m_probeRecoveredHere = FALSE;

	
	
	if(g_network.IsActive() && g_network.GetStartingAge() > 0) {
		sint32 i;
		for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
			if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)i)))
				continue;

			if(!CanBuildBuilding(i))
				continue;

			sint32 enable = g_theBuildingDB->Get(i)->GetEnableAdvanceIndex();
			if(g_theAdvanceDB->Get(enable)->GetAgeIndex() < g_network.GetStartingAge()) {
				m_built_improvements |= (uint64)1 << (uint64)i;
			}
		}
	}
	FindBestSpecialists();

	CalculateGrowthRate();

	FindGoodDistances();

	GenerateBorders(center_point, m_owner, g_theConstDB->GetBorderIntRadius(), g_theConstDB->GetBorderSquaredRadius());

	UpdateSprite();

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_OpenInitialCityInterface,
						   GEA_City, m_home_city,
						   GEA_End);

}

void CityData::NetworkInitialize()
{
	FindBestSpecialists();
}












CityData::CityData(CivArchive &archive)
{
	m_happy = new Happy;
	m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];

	m_sentInefficientMessageAlready = FALSE;
	Serialize(archive) ;
}

CityData::CityData(CityData *copy)
{
	m_happy = new Happy;
	m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];

	Copy(copy);
}

void CityData::Copy(CityData *copy)
{
	g_saveFileVersion = gamefile_CurrentVersion();

	if(g_network.IsActive()) {
		
		sint32 i;
		for(i = copy->m_tradeSourceList.Num() - 1; i >= 0; i--) {
			if(!g_theTradePool->IsValid(copy->m_tradeSourceList[i])) {
				copy->m_tradeSourceList.DelIndex(i);
			}
		}

		for(i = copy->m_tradeDestinationList.Num() - 1; i >= 0; i--) {
			if(!g_theTradePool->IsValid(copy->m_tradeDestinationList[i])) {
				copy->m_tradeDestinationList.DelIndex(i);
			}
		}
	}

	
	CivArchive archive;
	archive.SetStore();
	copy->Serialize(archive);
	archive.ResetForLoad();
	Serialize(archive);


	if(this == m_home_city.CD()) {
		if(g_network.IsHost()) {
			g_network.Enqueue(this);
		} else if(g_network.IsClient()) {
			g_network.SendCity(this);
		}
	}
}

bool CityData::IsACopy()
{
	if(!m_home_city.IsValid())
		return true;

	return this != m_home_city.CD();
}

void CityData::PrepareToRemove(const CAUSE_REMOVE_ARMY cause,
                               PLAYER_INDEX killedBy)
{
    sint32 i;
    
	
	uint64 wonders=m_builtWonders;
    for(i=0; wonders; i++) {
        if (wonders & 1) {
            g_player[m_owner]->RemoveWonder(i, TRUE);
        }
        wonders >>= 1;
    }

	
	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);
	for(it.Start(); !it.End(); it.Next()) {
		if(it.Pos() == m_home_city.RetPos()) continue;
		g_theWorld->GetCell(it.Pos())->SetCityOwner(Unit(0));
	}
}    




void CityData::SetShieldstore (sint32 s)

{
    m_shieldstore = s; 
}

void CityData::AddShields(sint32 s)
{
	m_shieldstore += s;
}











BOOL CityData::ShouldRevolt(const sint32 inciteBonus)
{
	
	
	if(g_slicEngine->GetTutorialActive() &&
	   m_owner == g_slicEngine->GetTutorialPlayer()) {
		return FALSE;
	}

	if(m_home_city.Flag(k_UDF_CANT_RIOT_OR_REVOLT))
		return FALSE;

	return (m_happy->ShouldRevolt(inciteBonus)) ;
}










void CityData::Revolt(sint32 &playerToJoin, BOOL causeIsExternal)
	{
	sint32	i,
			n ;

	PLAYER_INDEX	orgowner,
					newowner ;

	static CellUnitList army ;
	army.Clear();

	MapPoint	city_pos,
				new_city ;

    BOOL    joined_egalatarians = FALSE;

	g_player[m_owner]->m_score->AddRevolution();

	orgowner = m_owner ;
	m_home_city.GetPos(city_pos) ;
	
	if(!causeIsExternal && g_network.IsActive()) {
		g_network.Block(m_owner);
	}
	
	newowner = -1;
	if(wonderutil_GetRevoltingCitiesJoinPlayer(g_theWonderTracker->GetBuiltWonders())) {
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				if(wonderutil_GetRevoltingCitiesJoinPlayer(
					g_player[p]->m_builtWonders)) {
					newowner = p;
					
					
					
					
					
					if(newowner == m_owner) {
						newowner = -1;
                    } else {
                        joined_egalatarians = TRUE;
                    }
					break;
				}
			}
		}
	}

	if(newowner < 0) {
		if(playerToJoin < 0) {
			newowner = civilisation_NewCivilisationOrVandals(m_owner);
			playerToJoin = newowner;
		} else {
			newowner = playerToJoin;
		}
	}

    if (orgowner != newowner)
    {
		if(g_network.IsHost()) {
			g_network.Block(orgowner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_REVOLT_NOTICES,
										  orgowner, newowner,
										  m_home_city.m_id,
										  joined_egalatarians));
			g_network.Unblock(orgowner);
		}

        SlicObject *so = new SlicObject("010NewCiv");
        so->AddAllRecipients();
        so->AddCivilisation(orgowner);
        so->AddCivilisation(newowner);
        so->AddCity(m_home_city);
        g_slicEngine->Execute(so) ;

        if (joined_egalatarians) {
            so = new SlicObject("011CityJoinedYourCiv");
            so->AddRecipient(newowner);
            so->AddCivilisation(orgowner);
            so->AddCivilisation(newowner);
            so->AddCity(m_home_city);
            g_slicEngine->Execute(so) ;
        }

    }

	if(!causeIsExternal && g_network.IsActive()) {
		g_network.Unblock(m_owner);
	}
	
	static UnitDynamicArray revealedUnits;
	revealedUnits.Clear();

	m_home_city.ResetCityOwner(newowner, FALSE, CAUSE_REMOVE_CITY_HAPPINESS_REVOLT); 					

	m_build_queue.Clear() ;											

#if 0
	
	
	
	AddHappyTimer(50, 100, HAPPY_REASON_POST_REVOULTION_BLISS);
#endif

	
	g_theWorld->GetCell(city_pos)->GetArmy(army) ;
	n=army.Num() ;
    for (i=0; i<n; i++) { 
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
							   GEA_Unit, army[i].m_id,
							   GEA_Int, CAUSE_REMOVE_ARMY_UPRISING,
							   GEA_Player, newowner,
							   GEA_End);

		
		
    }

	
	
	
	
	

	
	sint32		soundID = -1, spriteID = -1;

	const SpecialAttackInfoRecord *specRec;
	specRec = unitutil_GetSpecialAttack(SPECATTACK_REVOLUTION);
	if(specRec) {
		soundID = specRec->GetSoundIDIndex();
		spriteID = specRec->GetSpriteID()->GetValue();
	}

	if (spriteID != -1 && soundID != -1) {
		g_director->AddSpecialAttack(m_home_city.GetActor()->GetUnitID(), m_home_city, SPECATTACK_REVOLUTION);
	} else {
		if (soundID != -1) {
			sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
			if ((visiblePlayer == m_owner) || 
				(m_home_city.GetVisibility() & (1 << visiblePlayer))) {

				g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID, 
											m_home_city.RetPos().x, m_home_city.RetPos().y);
			}
		}
	}
}












void CityData::TeleportUnits(const MapPoint &pos, BOOL &revealed_foreign_units, 
                             BOOL &revealed_unexplored, sint32 foreigner)
{
	
	sint32 i;
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());		
	
	
	DynamicArray<Army> armies;
	
	for(i = 0; i < cell->GetNumUnits(); i++) {
		if(armies.Find(cell->AccessUnit(i).GetArmy()) >= 0) {
			
			continue;
		}
		armies.Insert(cell->AccessUnit(i).GetArmy());
	}
			
	for(i = 0; i < armies.Num(); i++) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_GetExpelledOrder,
							   GEA_Army, armies[i],
							   GEA_MapPoint, pos,
							   GEA_Player, foreigner,
							   GEA_End);
	}
				
#if 0
	
	
	
	
	
	

	static CellUnitList	units ;
	units.Clear();

	MapPoint	city_pos ;

	sint32	i,
			n ;

	m_home_city.GetPos(city_pos) ;
	g_theWorld->GetCell(city_pos)->GetArmy(units) ;
	n = units.Num() ;

    revealed_foreign_units = FALSE; 
    revealed_unexplored = FALSE; 
	static UnitDynamicArray revealed;
	static DynamicArray<Army> moveArmies;
	moveArmies.Clear();
	revealed.Clear();

	for (i=0; i<n; i++)	{
        revealed.Clear(); 
		MapPoint oldpos;

		units[i].GetPos(oldpos);

		g_theWorld->RemoveUnitReference(oldpos, units[i]) ;
		units[i].SetPosition(pos, revealed, revealed_unexplored) ;

		
		

        if (0 < revealed.Num()) { 
            revealed_foreign_units = TRUE; 
        }
	}
	for(i = 0; i < n; i++) {
		
		
		
		units[i].GetArmy().ResetPos();
	}
#endif
}












void CityData::StopTradingWith(PLAYER_INDEX bannedRecipient)
	{
	TradeRoute	route ;

	Unit	srcCity,
			destCity ;

	sint32	i,
			n ;

	
	n = m_tradeSourceList.Num() ;
	for(i = 0; i < n; i++)
		{
		route = m_tradeSourceList[i] ;
		srcCity = route.GetSource() ;
		destCity = route.GetDestination() ;
		if ((srcCity.GetOwner() == bannedRecipient) || (destCity.GetOwner() == bannedRecipient))
			route.Kill(CAUSE_KILL_TRADE_ROUTE_DIPLOMATIC_AGREEMENT) ;											

		}

	
	n = m_tradeDestinationList.Num() ;
	for(i = 0; i < n; i++)
		{
		route = m_tradeDestinationList[i] ;
		srcCity = route.GetSource() ;
		destCity = route.GetDestination() ;
		if ((srcCity.GetOwner() == bannedRecipient) && (destCity.GetOwner() == bannedRecipient))
			route.Kill(CAUSE_KILL_TRADE_ROUTE_DIPLOMATIC_AGREEMENT) ;										

		}

	}













void CityData::CalcPollution(void)
{
	sint32 populationPolluting,productionPolluting;
	sint32 i;
	uint64 buildingCheck;
	double buildingPollution,buildingProductionPercentage,buildingPopulationPercentage,temp;


	populationPolluting = PopCount() - 
		g_theDifficultyDB->GetPollutionStartPopulationLevel(g_theGameSettings->GetDifficulty());
	if(populationPolluting < 0) 
	{
		populationPolluting = 0;
	}
	else
		populationPolluting = (sint32)(populationPolluting * 
			g_theDifficultyDB->GetPollutionPopulationRatio(g_theGameSettings->GetDifficulty()));

	populationPolluting = (sint32)(populationPolluting * g_player[m_owner]->GetPollutionCoef());

	productionPolluting = m_gross_production_this_turn -
		g_theDifficultyDB->GetPollutionStartProductionLevel(g_theGameSettings->GetDifficulty());
	if(productionPolluting < 0)
	{
		productionPolluting = 0;
	}
	else
		productionPolluting = (sint32)(productionPolluting * 
			g_theDifficultyDB->GetPollutionProductionRatio(g_theGameSettings->GetDifficulty()));

	productionPolluting = (sint32)(productionPolluting * g_player[m_owner]->GetPollutionCoef());

	buildingPollution=0.0;
	buildingProductionPercentage=0.0;
	buildingPopulationPercentage=0.0;
	for(i=0; i<g_theBuildingDB->NumRecords(); i++)
	{
		buildingCheck = (uint64)1 << (uint64)i;
		if(GetEffectiveBuildings() & buildingCheck)
		{
			if(g_theBuildingDB->Get(i)->GetPollutionAmount(temp))
			{
				buildingPollution+=temp;
			}
			if(g_theBuildingDB->Get(i)->GetProductionPollutionPercent(temp))
			{
				buildingProductionPercentage+=temp;
			}
			if(g_theBuildingDB->Get(i)->GetPopulationPollutionPercent(temp))
			{
				buildingPopulationPercentage+=temp;
			}
		}
	}

	for(i=0; i<g_theWonderDB->NumRecords(); i++)
	{
		buildingCheck = (uint64)1 << (uint64)i;
		if(GetBuiltWonders() & buildingCheck)
		{
			if(g_theWonderDB->Get(i)->GetPollutionAmount(temp))
			{
				buildingPollution+=temp;
			}
			if(g_theWonderDB->Get(i)->GetPollutionPercent(temp))
			{
				buildingProductionPercentage+=temp;
				buildingPopulationPercentage+=temp;
			}
		}
	}

	if(g_theGameSettings->GetPollution())
	{
		m_cityPopulationPollution=populationPolluting;
		m_cityIndustrialPollution=productionPolluting;

		m_total_pollution=m_cityPopulationPollution+m_cityIndustrialPollution+m_foodVatPollution;
		m_total_pollution+=(sint32)(populationPolluting*buildingPopulationPercentage);
		m_total_pollution+=(sint32)(productionPolluting*buildingProductionPercentage);
		m_total_pollution+=(sint32)buildingPollution;
	}
	else
	{
		m_cityPopulationPollution=0;
		m_cityIndustrialPollution=0;
		m_total_pollution=0;
	}
}

void CityData::DoLocalPollution()
{
	if(!g_theGameSettings->GetPollution())
		return;

	if(m_total_pollution < g_theConstDB->LocalPollutionLevel())
		return;

	double diff = double(m_total_pollution) - g_theConstDB->LocalPollutionLevel();
	double chance = diff * g_theConstDB->LocalPollutionChance();

    
    if ((chance > 0.10) &&
        (g_slicEngine->GetSegment("080CityPollutionWarning")->TestLastShown(m_owner, 10))) {
        SlicObject *so = new SlicObject("080CityPollutionWarning");
        so->AddCity(m_home_city);
        so->AddRecipient(m_owner);
		so->AddCivilisation(m_owner);
        g_slicEngine->Execute(so) ;
    }

	if(g_rand->Next(1000) < chance * 1000) {
        SlicObject *so = new SlicObject("040GrossPolluter");
        so->AddCity(m_home_city);
        so->AddRecipient(m_owner);
        g_slicEngine->Execute(so) ;

		sint32 totalTiles=0;
		MapPoint cpos;
		m_home_city.GetPos(cpos);
		CityInfluenceIterator it(cpos, m_sizeIndex);
		for(it.Start(); !it.End(); it.Next()) 
		{
			totalTiles++;
		}
		
		m_whichtile = g_rand->Next(totalTiles);
		m_tilecount = 0;
		
		m_cityRadiusOp = RADIUS_OP_KILL_TILE;
		for(it.Start(); !it.End(); it.Next()) 
		{
			CityRadiusFunc(it.Pos());
		}
	}
}


sint32 CityData::ComputeGrossProduction( double workday_per_person, sint32 collected_production, sint32 & crime_loss, sint32 & franchise_loss ) const
{

	
	
	
	

	

	
	sint32 gross_production = collected_production;

	gross_production = ceil(gross_production * workday_per_person);

	double prodBonus;
	buildingutil_GetProductionPercent(GetEffectiveBuildings(), prodBonus);
	gross_production += ceil(gross_production * prodBonus);

	sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_PRODUCTION, m_owner);
	gross_production += ceil(double(gross_production) * (double(featPercent) / 100.0));
	
	gross_production += ceil(gross_production *
						   (double(wonderutil_GetIncreaseProduction(g_player[m_owner]->m_builtWonders)) * 0.01));

	gross_production = ceil((double)gross_production * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetProductionCoef());

	if(m_specialistDBIndex[POP_LABORER] >= 0) {
		gross_production += LaborerCount() *
			g_thePopDB->Get(m_specialistDBIndex[POP_LABORER])->GetProduction();
	}

	if(m_bioInfectionTurns > 0) {
		gross_production -= ceil(double(gross_production) * g_theConstDB->GetBioInfectionProductionCoef());
	}

	crime_loss = sint32(ceil(gross_production * m_happy->GetCrime()));
	
	if (crime_loss < 0) 
		crime_loss = 0;

	sint32 net_production = gross_production - crime_loss;

	if( m_franchise_owner >= 0 ) {
		franchise_loss = ceil(double(net_production) * g_theConstDB->GetFranchiseEffect());
	}
	else
		franchise_loss = 0;

	return gross_production;
}

sint32 CityData::ProcessProduction(bool projectedOnly)
{
	
	
	m_gross_production_this_turn = 
		ComputeGrossProduction(g_player[m_owner]->GetWorkdayPerPerson(),
							   m_collected_production_this_turn,
							   m_shields_lost_to_crime,
							   m_productionLostToFranchise);
	
	m_shields_this_turn = m_gross_production_this_turn -
		(m_shields_lost_to_crime + m_productionLostToFranchise);

	if( m_franchise_owner >= 0 ) {
		if(!projectedOnly)
			g_player[m_franchise_owner]->AddProductionFromFranchise(m_productionLostToFranchise);
	}

	return m_shields_this_turn;
}

double CityData::ProjectMilitaryContribution()
{
   return (double)m_shields_this_turn; 
}


sint32 CityData::ComputeMaterialsPaid(double percent_terrain)
{
	return(m_contribute_materials ? (static_cast<sint32>(
		(static_cast<double>(m_shields_this_turn) *
		percent_terrain) + 0.000001)) : 0);
}

void CityData::PayFederalProduction (double percent_military,
									 sint32 &mil_paid, 
									 double percent_terrain, 
									 sint32 &mat_paid)

{
    

#if defined(_DEBUG) || defined(USE_LOGGING)
	sint32 origShields = m_shields_this_turn;
#endif

    if (m_contribute_military) { 
        mil_paid  = (sint32)(ceil((double)m_shields_this_turn * percent_military));
		m_shields_this_turn -= mil_paid;
    } else { 
        mil_paid = 0;
    } 

    mat_paid = ComputeMaterialsPaid(percent_terrain); 
    if (m_contribute_materials) { 
		m_shields_this_turn -= mat_paid;
    }

	DPRINTF(k_DBG_GAMESTATE, ("City %lx: S: %d, %d mil(%lf), %d mat(%lf)\n", (uint32)m_home_city, origShields, mil_paid, percent_military, mat_paid, percent_terrain));

    Assert (0 <= m_shields_this_turn);

}

void CityData::PayFederalProductionAbs (sint32 mil_paid, 
										double percent_mat, 
										sint32 &mat_paid)

{

#ifdef _DEBUG
    if(0 < mil_paid) {
        if (!m_contribute_military) { 
#pragma warning (disable : 4127)									
			Assert(0);
#pragma warning (default : 4127)									
            return; 
        } 
    } 
      
#endif

    m_shields_this_turn -= mil_paid;
    mat_paid = ComputeMaterialsPaid(percent_mat);
    m_shields_this_turn -= mat_paid;








    Assert (0 <= m_shields_this_turn);
	if (m_shields_this_turn < 0) {
		m_shields_this_turn = 0;
	}
}


void CityData::AddShieldsToBuilding()
{
    if (m_shields_this_turn < 0) { 
        m_shields_this_turn = 0; 
    }

	if(m_is_rioting) {
		m_shields_this_turn = 0;
	}

    SetShieldstore (m_shieldstore + m_shields_this_turn); 
}


#if defined(ACTIVISION_ORIGINAL)
void CityData::GetFullAndPartialRadii(sint32 &fullRadius, sint32 &partRadius)
#else
void CityData::GetFullAndPartialRadii(sint32 &fullRadius, sint32 &partRadius) const
#endif
{
	const CitySizeRecord *fullRec = NULL, *partRec = NULL;
	if(m_workerFullUtilizationIndex >= 0) {
		fullRec = g_theCitySizeDB->Get(m_workerFullUtilizationIndex);
	}

	if(m_workerPartialUtilizationIndex >= 0 && 
	   m_workerPartialUtilizationIndex < g_theCitySizeDB->NumRecords()) {
		partRec = g_theCitySizeDB->Get(m_workerPartialUtilizationIndex);
	} else {
		partRec = NULL;
	}

	if(fullRec) {
		fullRadius = fullRec->GetSquaredRadius();
	} else {
		fullRadius = 0;
	}

	if(partRec) {
		partRadius = partRec->GetSquaredRadius();
	} else {
		partRadius = 0;
	}
}

#if !defined(ACTIVISION_ORIGINAL)
//----------------------------------------------------------------------------
//
// Name       : CityData::GetUtilisationRatio
//
// Description: Get the current ratio for the worked tiles in the ring with
//              the given distance.
//
// Parameters : squaredDistance	: square of the distance
//
// Globals    : -
//
// Returns    : double			: utilisation ratio (in the range [0.0, 1.0])
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

double CityData::GetUtilisationRatio(uint32 const squaredDistance) const
{
	sint32			fullSquaredRadius;
	sint32			partSquaredRadius;
	GetFullAndPartialRadii(fullSquaredRadius, partSquaredRadius);

	if (squaredDistance > partSquaredRadius)
	{
		return 0.0;	// Not in the city influence (yet).
	}
	else if (squaredDistance <= fullSquaredRadius)
	{
		return 1.0;	// Within the fully utilised ring(s).
	}

	// Compute the fraction for the partially utilised ring.

	sint32 const	dbMaxIndex			= g_theCitySizeDB->NumRecords();
	
	// # of working in the inner rings (fully employed)
	sint32			fullRingWorkers		= 0;
	if (m_workerFullUtilizationIndex >= 0)
	{
		sint32 const	dbIndex			= 
#if defined(_MSC_VER)
			min(m_workerFullUtilizationIndex, dbMaxIndex - 1);
#else
			std::min(m_workerFullUtilizationIndex, dbMaxIndex - 1);
#endif
		fullRingWorkers = g_theCitySizeDB->Get(dbIndex)->GetMaxWorkers();
	}

	// # of working when the outer ring would have been filled to the max.
	sint32          maxRingWorkers		= 0;	
	if (m_workerPartialUtilizationIndex >= 0)
	{
		sint32 const	dbIndex			= 
#if defined(_MSC_VER)
			min(m_workerPartialUtilizationIndex, dbMaxIndex - 1);
#else
			std::min(m_workerPartialUtilizationIndex, dbMaxIndex - 1);
#endif

		maxRingWorkers = g_theCitySizeDB->Get(dbIndex)->GetMaxWorkers();
	}

	// # of actually working in the partial ring.
	sint32 const		partialRingWorkers	= 	
#if defined(_MSC_VER)
			min(maxRingWorkers, WorkerCount() + SlaveCount())
#else
			std::min(maxRingWorkers, WorkerCount() + SlaveCount())
#endif
				- fullRingWorkers;


	return (partialRingWorkers <= 0)
		   ? 0.0 
		   : static_cast<double>(partialRingWorkers) / 
		     static_cast<double>(maxRingWorkers - fullRingWorkers);
}
#endif

void CityData::CollectResources()
{
	sint32 fullFoodTerrainTotal = 0;
	sint32 partFoodTerrainTotal = 0;

	sint32 fullProdTerrainTotal = 0;
	sint32 partProdTerrainTotal = 0;

	sint32 fullCommerceTerrainTotal = 0;
	sint32 partCommerceTerrainTotal = 0;

	sint32 fullSquaredRadius;
	sint32 partSquaredRadius;

	GetFullAndPartialRadii(fullSquaredRadius, partSquaredRadius);
	
	MapPoint cityPos = m_home_city.RetPos();
	CityInfluenceIterator it(cityPos, m_workerPartialUtilizationIndex);

	m_collectingResources.Clear();

	
	
	
	for(it.Start(); !it.End(); it.Next()) {
		Cell *cell = g_theWorld->GetCell(it.Pos());

		if(fullSquaredRadius >= 0 &&
		   MapPoint::GetSquaredDistance(cityPos, it.Pos()) <= fullSquaredRadius) {
			fullFoodTerrainTotal += cell->GetFoodProduced();
			fullProdTerrainTotal += cell->GetShieldsProduced();
			fullCommerceTerrainTotal += cell->GetGoldProduced();

		} else if(partSquaredRadius > 0 &&
				  MapPoint::GetSquaredDistance(cityPos, it.Pos()) <= partSquaredRadius) {
			
			partFoodTerrainTotal += cell->GetFoodProduced();
			partProdTerrainTotal += cell->GetShieldsProduced();
			partCommerceTerrainTotal += cell->GetGoldProduced();
		}
		sint32 good;
		if(g_theWorld->GetGood(it.Pos(), good)) {
			m_collectingResources.AddResource(good);
		}
	}

	
#if defined(ACTIVISION_ORIGINAL)	// moved to function GetUtilisationRatio	
	sint32 fullMaxWorkers = m_workerFullUtilizationIndex >= 0 
		                  ? g_theCitySizeDB->Get(m_workerFullUtilizationIndex)->GetMaxWorkers() 
						  : 0;

	sint32 workerDiff;
	sint32 numPartialWorkers = (WorkerCount() + SlaveCount()) - fullMaxWorkers;
	if(m_workerPartialUtilizationIndex >= 0 && 
	   m_workerPartialUtilizationIndex < g_theCitySizeDB->NumRecords()) {
		workerDiff = g_theCitySizeDB->Get(m_workerPartialUtilizationIndex)->GetMaxWorkers() -
			fullMaxWorkers;
	} else {
		numPartialWorkers = 0;
		workerDiff = 0;
	}

	
	double utilizationRatio = 0;

	if(numPartialWorkers > 0) {
		Assert(workerDiff > 0);
		if(workerDiff > 0) {
			utilizationRatio = (double)numPartialWorkers / (double)workerDiff;
		}
	}
#else
	double const	utilizationRatio	= GetUtilisationRatio(partSquaredRadius);
#endif

	m_max_food_from_terrain = fullFoodTerrainTotal + partFoodTerrainTotal;
	m_max_production_from_terrain = fullProdTerrainTotal + partProdTerrainTotal;
	m_max_trade_from_terrain = fullCommerceTerrainTotal + partCommerceTerrainTotal;

	if(m_is_rioting) {
		m_max_trade_from_terrain = 0;
		fullCommerceTerrainTotal = 0;
		partCommerceTerrainTotal = 0;
	}

	m_gross_food_this_turn = fullFoodTerrainTotal + ceil(utilizationRatio * double(partFoodTerrainTotal));
	m_collected_production_this_turn = (sint32)(fullProdTerrainTotal + ceil(utilizationRatio * double(partProdTerrainTotal)));
	m_gross_trade = (sint32)(fullCommerceTerrainTotal + ceil(utilizationRatio * double(partCommerceTerrainTotal)));

	
	m_gross_production_this_turn = m_collected_production_this_turn;
	m_shields_this_turn = m_collected_production_this_turn;
	m_food_produced_this_turn = m_gross_food_this_turn;
	m_trade = m_gross_trade;

}

sint32 CityData::ProcessFood()
{
	
	double foodBonus;
	buildingutil_GetFoodPercent(GetEffectiveBuildings(), foodBonus);
	m_gross_food_this_turn += m_food_produced_this_turn * foodBonus;

	m_gross_food_this_turn += sint32(ceil(((double)m_gross_food_this_turn * 
		double(double(wonderutil_GetIncreaseFoodAllCities(
			g_player[m_owner]->m_builtWonders)) / 100.0))));
	

	if(m_specialistDBIndex[POP_FARMER] >= 0) {
		m_gross_food_this_turn += FarmerCount() *
			g_thePopDB->Get(m_specialistDBIndex[POP_FARMER])->GetFood();
	}

	m_gross_food_this_turn = sint32(ceil((double)m_gross_food_this_turn * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetFoodCoef()));

	m_food_produced_this_turn = m_gross_food_this_turn;

	m_food_lost_to_crime = floor(m_food_produced_this_turn * m_happy->GetCrime());
	
	if (m_food_lost_to_crime < 0)
		m_food_lost_to_crime = 0;

    m_food_produced_this_turn -= m_food_lost_to_crime;

	return (sint32)m_food_produced_this_turn;
}

double CityData::GetFoodRequiredPerCitizen() const
{
	return g_player[m_owner]->GetRationsPerPerson();
}

double CityData::GetFoodRequired() const
{
	
    return (PopCount() - SlaveCount()) * GetFoodRequiredPerCitizen();
}
	
void CityData::EatFood()

{
    m_food_consumed_this_turn = GetFoodRequired();

    m_food_delta = m_food_produced_this_turn - m_food_consumed_this_turn;
}

sint32 CityData::GetBuildingOvercrowdingBonus()
{
	
	sint32 level = 0;
	buildingutil_GetRaiseOvercrowdingLevel(GetEffectiveBuildings(), level);
	return level;
}

sint32 CityData::GetBuildingMaxPopIncrease()
{
	sint32 level = 0;
	buildingutil_GetRaiseMaxPopulation(GetEffectiveBuildings(), level);
	return level;
}

void CityData::CalculateGrowthRate()
{ 
	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);

	double maxGrowthRate = (double)rec->GetGrowthRate();
	double minGrowthRate = (double)rec->GetMinGrowthRate();
	sint32 maxSurplusFood = rec->GetMaxSurplusFood();

	double minPop;
	if(m_sizeIndex < 1) {
		minPop = 0;
	} else {
		minPop = (double)g_theCitySizeDB->Get(m_sizeIndex - 1)->GetPopulation() + 1.0;
	}

	double overcrowding = rec->GetBaseOvercrowding();
	overcrowding += GetBuildingOvercrowdingBonus();
	double baseRate;
	sint32 baseMaxPop = rec->GetBaseMaxPop();
	sint32 maxPop = baseMaxPop + GetBuildingMaxPopIncrease();
	
	if(PopCount() < overcrowding) {
		baseRate = maxGrowthRate;
		m_overcrowdingCoeff = 1;
	} else {
		double popRatio = double(PopCount() - overcrowding) / double(maxPop - overcrowding);
		m_overcrowdingCoeff = 1 - (popRatio * popRatio);
		baseRate = m_overcrowdingCoeff * maxGrowthRate;
	}

	if(m_food_delta < 0) {
		
		if(m_food_delta < -maxSurplusFood)
			m_food_delta = -maxSurplusFood;

		
		
		
		double maxPop = (double)rec->GetPopulation();
		double popScale = ((double)PopCount() - minPop) / (maxPop - minPop);
		double growthRate = maxGrowthRate - popScale * (maxGrowthRate - minGrowthRate);
		
		
		m_growth_rate = sint32(double(double(m_food_delta) / (double)maxSurplusFood) * growthRate);
	} else {
		if(m_food_delta > maxSurplusFood)
			m_food_delta = maxSurplusFood;
		
		m_growth_rate = sint32(baseRate * (double(m_food_delta) / double(PopCount())));
		if(m_growth_rate > g_theConstDB->GetMaxAbsoluteGrowthRate())
			m_growth_rate = g_theConstDB->GetMaxAbsoluteGrowthRate();

		if(m_population > g_theCitySizeDB->Get(g_theCitySizeDB->NumRecords() - 1)->GetPopulation()) {
			m_growth_rate = 0;
		}

		if(m_is_rioting)
			m_growth_rate = 0;
	}
}

sint32 CityData::GrowOrStarve()
{
	CalculateGrowthRate();

	if(m_food_delta < 0) {
		if(m_growth_rate < 0) {
			if(m_starvation_turns > 0) {
				
				if (m_starvation_turns == GetStarvationProtection()) {
					SlicObject *so = new SlicObject("911CityWillStarveInitialWarning") ;
					so->AddRecipient(GetOwner()) ;
		            so->AddCity(m_home_city) ;
					so->AddPlayer(m_owner);
					g_slicEngine->Execute(so) ;
				}
				else if (m_starvation_turns == (sint32)(GetStarvationProtection()/2)) {
					SlicObject *so = new SlicObject("911CityWillStarveFoodStoresLow") ;
					so->AddRecipient(GetOwner()) ;
		            so->AddCity(m_home_city) ;
					so->AddPlayer(m_owner);
					g_slicEngine->Execute(so) ;
				}

				m_starvation_turns--;
				return TRUE;
			} 
			else m_partialPopulation -= k_PEOPLE_PER_POPULATION;

		}
		m_partialPopulation += m_growth_rate;

		if(m_partialPopulation >= k_PEOPLE_PER_POPULATION) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
								   GEA_City, m_home_city.m_id,
								   GEA_End);
			
			m_partialPopulation -= k_PEOPLE_PER_POPULATION;
			return TRUE;
		} else if(m_partialPopulation < 0) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
								   GEA_City, m_home_city.m_id,
								   GEA_End);
			
			m_partialPopulation += k_PEOPLE_PER_POPULATION;
			return TRUE;
		}
	} else {
		m_partialPopulation += sint32(m_growth_rate);

		ResetStarvationTurns();

		if(m_partialPopulation >= k_PEOPLE_PER_POPULATION) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
								   GEA_City, m_home_city.m_id,
								   GEA_End);
			
			m_partialPopulation -= k_PEOPLE_PER_POPULATION;
			return TRUE;
		}
	}
	
	return FALSE;
}

int CityData::FoodSupportTroops()

{
	return FALSE;
}

void CityData::UpdateSprite(void)
{
	
	
	
	if (PopCount() == 0) return;

	
	
	UnitActor	*actor = m_home_city.GetActor();
	SpriteState *ss = m_home_city.GetSpriteState();
	sint32	type = m_home_city.GetType();








	g_director->AddMorphUnit(actor, ss, type, m_home_city);
}

void CityData::MakeCitizen(PopDBIndex type, const MapPoint &center_pos,
						   sint32 origOwner)

{ 
	DPRINTF(k_DBG_GAMESTATE, ("MakeCitizen(%d, %d,%d)\n", type,
							  center_pos.x, center_pos.y));

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
						   GEA_City, m_home_city.m_id,
						   GEA_End);
	
}

void CityData::AddTradeResource(ROUTE_TYPE type, sint32 resource)
{
	switch(type) {
		case ROUTE_TYPE_RESOURCE:
			m_buyingResources.AddResource(resource);
			break;
		case ROUTE_TYPE_FOOD:
			Assert(FALSE); 
			
			
			break;
		default:
			Assert(FALSE);
			break;
	}
}

sint32 CityData::CalculateTradeRoutes(BOOL projectedOnly)
{
	sint32 i, n;
	ROUTE_TYPE routeType;
	sint32 routeResource;
	TradeDynamicArray deadRoutes;

	m_goldFromTradeRoutes = 0;
	m_goldLostToPiracy = 0;

	n = m_tradeSourceList.Num();
	m_sellingResources.Clear();
	for(i = 0; i < n; i++) {
		TradeRoute route = m_tradeSourceList[i];
		if(!g_theTradePool->IsValid(route)) {
			if(g_network.IsClient()) {
				g_network.RequestResync(RESYNC_BAD_TRADE_ROUTE);
			}
			continue;
		}
		if(!projectedOnly) {
			if(!route.IsActive()) {
				route.Activate();
				g_director->TradeActorCreate(route);
			}
			route.BeginTurn();
		}

		route.GetSourceResource(routeType, routeResource);

		switch(routeType) {
			case ROUTE_TYPE_RESOURCE:
				if(m_collectingResources[routeResource] <= m_sellingResources[routeResource]) {
					if(!projectedOnly) {
						deadRoutes.Insert(route);
					}
				}
				
				m_sellingResources.AddResource(routeResource);
				break;
			case ROUTE_TYPE_FOOD:
			{
				
				if(m_food_delta < routeResource && !projectedOnly) {
					deadRoutes.Insert(route);
				}
				break;
			}
			default:
				Assert(FALSE);
				break;
		}
	}

	n = m_tradeDestinationList.Num();
	BOOL killRoute;
	m_buyingResources.Clear();
	for(i = 0; i < n; i++) {
		TradeRoute route = m_tradeDestinationList[i];
		if(!g_theTradePool->IsValid(route)) {
			if(g_network.IsClient()) {
				g_network.RequestResync(RESYNC_BAD_TRADE_ROUTE);
			}
			continue;
		}

		killRoute = FALSE;
		if(!projectedOnly) {
			if(route.GetOwner() != m_owner) {
				if(g_player[m_owner]->GetGold() < route.GetGoldInReturn()) {
					deadRoutes.Insert(route);
					killRoute = TRUE;
				} else {
					g_player[m_owner]->SubGold(route.GetGoldInReturn());
					g_player[route.GetSource().GetOwner()]->AddGold(route.GetGoldInReturn());
				}
			}
			if(!route.IsActive()) {
				route.Activate();
				g_director->TradeActorCreate(route);
			}
		}
		if(route.IsActive() && !killRoute) {
			route.GetSourceResource(routeType, routeResource);
			AddTradeResource(routeType, routeResource);
		}
	}		

	if(!projectedOnly) {
		sint32 i;
		for(i = deadRoutes.Num() - 1; i >= 0; i--) {
			if(deadRoutes[i].GetSource().m_id == m_home_city.m_id) {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute,
									   GEA_TradeRoute, deadRoutes[i],
									   GEA_Int, CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED,
									   GEA_End);
			} else {
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute, 
									   GEA_TradeRoute, deadRoutes[i],
									   GEA_Int, CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED,
									   GEA_End);
			}
		}
	}

	return CalculateGoldFromResources();
}

sint32 CityData::CalculateGoldFromResources()
{
	sint32 res = 0;
	sint32 i;
	for(i = 0; i < m_tradeSourceList.Num(); i++) {
		if(!m_tradeSourceList[i]->IsBeingPirated()) {
			res += m_tradeSourceList[i]->GetValue();
		}
		else {
			m_goldLostToPiracy += m_tradeSourceList[i]->GetValue();
		}
	}

	m_goldFromTradeRoutes += res;

	sint32 wonderTradeBonus = wonderutil_GetMultiplyTradeRoutes(m_builtWonders);
	if(wonderTradeBonus > 0) {
		m_goldFromTradeRoutes += sint32(double(m_goldFromTradeRoutes) * 
										(double(wonderTradeBonus) * 0.01));
		m_goldLostToPiracy += sint32(double(m_goldLostToPiracy) * 
										(double(wonderTradeBonus) * 0.01));
	}
	return m_goldFromTradeRoutes;
}


void CityData::CollectPopScience(bool projectedOnly)
{
	Assert(FALSE);

   m_science = GetScienceFromPops();

   if(!projectedOnly)
	   g_player[m_owner]->AddPopScience(m_science);
}

sint32 CityData::SupportBuildings(bool projectedOnly)
{

	sint32 buildingUpkeep = GetSupportBuildingsCost();
	m_trade -= buildingUpkeep;
	if(m_trade < 0 && !projectedOnly) {
		
		sint32 wonderLevel = wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);

		while((-m_trade > g_player[m_owner]->GetGold()) && (m_built_improvements != 0)) {
			sint32 cheapBuilding = buildingutil_GetCheapestBuilding(m_built_improvements, wonderLevel);
			Assert(cheapBuilding >= 0);
			if(cheapBuilding < 0)
				break;

			SellBuilding(cheapBuilding, FALSE);
			SlicObject *so = new SlicObject("029NoMaint") ;
			so->AddRecipient(GetOwner()) ;
			so->AddCity(m_home_city) ;
			so->AddBuilding(cheapBuilding) ;
			g_slicEngine->Execute(so) ;
		}
		
		if(g_player[m_owner]->GetGold() < m_trade) {
			g_player[m_owner]->m_gold->SetLevel(0);
		} else {
			g_player[m_owner]->m_gold->SubIncome(-m_trade);
		}
		m_trade = 0;
	}
	if(!projectedOnly)
		g_player[m_owner]->m_gold->AddMaintenance(buildingUpkeep);

	return buildingUpkeep;
}

sint32 CityData::GetSupportBuildingsCost()
{
	
	sint32 wonderLevel = wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);

	
	sint32 buildingUpkeep = buildingutil_GetTotalUpkeep(m_built_improvements, wonderLevel);

	
	return buildingUpkeep;
}

sint32 CityData::CalcWages(sint32 wage)
{
    int n;
	sint32 totalWages = 0;

    n = PopCount(); 
	totalWages += wage * (n - SlaveCount());

	return totalWages;
}

BOOL CityData::PayWages(sint32 wage, bool projectedOnly)
{
	m_wages_paid = CalcWages(wage);
	if(!projectedOnly) {
		g_player[m_owner]->m_gold->AddWages(m_wages_paid);
	}
	m_trade -= m_wages_paid;
	if(m_trade < 0 && !projectedOnly) {
		if(-m_trade < g_player[m_owner]->GetGold()) {
			g_player[m_owner]->m_gold->SubIncome(-m_trade); 
		} else {
			g_player[m_owner]->m_gold->SubIncome(g_player[m_owner]->GetGold());
			return FALSE;
		}
	}


    return TRUE; 
}

sint32 CityData::GetWagesNeeded(const sint32 & wages_per_person) const
{
    sint32 total_pop = PopCount(); 
	sint32 totalWages = wages_per_person * (total_pop - SlaveCount());

	return totalWages;
}

sint32 CityData::GetWagesNeeded()
{
	return GetWagesNeeded((sint32)g_player[m_owner]->GetWagesPerPerson());
}

void CityData::AddTradeRoute(TradeRoute &route, BOOL fromNetwork)
{
	Assert((route.GetSource() == m_home_city) ||
		   (route.GetDestination() == m_home_city));

	if(route.GetSource() == m_home_city) {
		m_tradeSourceList.Insert(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type) {
			case ROUTE_TYPE_RESOURCE:
				if(!fromNetwork) {
					Assert(m_collectingResources[resource] > m_sellingResources[resource]);
					if(m_collectingResources[resource] > m_sellingResources[resource]) {
						m_sellingResources.AddResource(resource);
					}
				}
				break;
			default:
				break;
		}
	}

	if(route.GetDestination() == m_home_city) {
		m_tradeDestinationList.Insert(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type) {
			case ROUTE_TYPE_RESOURCE:
				if(!fromNetwork) {
					m_buyingResources.AddResource(resource);
				}
				break;
			default:
				break;
		}
	}
}

void CityData::DelTradeRoute(TradeRoute route)
{
	Assert((route.GetSource() == m_home_city) ||
		   (route.GetDestination() == m_home_city));

	





	if(route.GetSource() == m_home_city) {
		m_tradeSourceList.Del(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type) {
			case ROUTE_TYPE_RESOURCE:
				Assert(m_sellingResources[resource] > 0);
				if(m_sellingResources[resource] > 0) {
					m_sellingResources.AddResource(resource, -1);
				}
				break;
			default:
				break;
		}
	}
		
	if(route.GetDestination() == m_home_city) {
		m_tradeDestinationList.Del(route);
		m_tradeSourceList.Del(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type) {
			case ROUTE_TYPE_RESOURCE:
				Assert(m_buyingResources[resource] > 0);
				if(m_buyingResources[resource] > 0) {
					m_buyingResources.AddResource(resource, -1);
				}
				break;
			default:
				break;
		}
	}
}

void CityData::CheckTopTen()
{ 
#if 0
	sint32	pos ;

	
	if (g_theTopTen->IsTopTenCity(m_home_city, TOPTENTYPE_BIGGEST_CITY, pos)) {
		m_currentlyTopTen = TRUE ;
    } else {
		m_currentlyTopTen = FALSE ;
    }
#endif
}

void CityData::CheatBuildFirstItem()
{ 
    MapPoint pos; 

    m_home_city.GetPos(pos); 

    m_shieldstore += m_build_queue.GetProductionRemaining(m_shieldstore);
	if(m_build_queue.BuildFront(m_shieldstore, this, pos, m_built_improvements, m_builtWonders, FALSE)) {
		m_shieldstore_at_begin_turn = m_shieldstore;
	}
}

void CityData::InitBeginTurnVariables()
{
	m_capturedThisTurn = FALSE;

	m_terrainWasPolluted=FALSE;				
	m_happinessAttacked=FALSE;				
	m_terrainImprovementWasBuilt=FALSE;		
	m_improvementWasBuilt=FALSE;			

	m_alreadySoldABuilding = FALSE;
	m_walls_nullified = FALSE;

}

void CityData::DoTurnCounters()
{
	if(m_spied_upon > 0)
		m_spied_upon--;

	if(m_watchfulTurns > 0)
		m_watchfulTurns--;

	if(m_bioInfectionTurns > 0) {
		m_bioInfectionTurns--;
		SpreadBioTerror();
	}

	if(m_nanoInfectionTurns > 0) {
		m_nanoInfectionTurns--;
		SpreadNanoTerror();
	}

	if(m_franchiseTurnsRemaining > 0) {
		m_franchiseTurnsRemaining--;
	} else if(m_franchiseTurnsRemaining == 0) {
		
		m_franchise_owner = -1;
		m_franchiseTurnsRemaining = -1;
	}

}

void CityData::TryToBuild()
{
	if(!m_isInjoined) {
		AddShieldsToBuilding();
		const GovernmentRecord *grec = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type);
		if(m_buildCapitalization && (m_build_queue.GetHead() && m_build_queue.GetHead()->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION)) {
			m_gold_from_capitalization = sint32(ceil(double(m_shieldstore) * grec->GetCapitalizationCoefficient()));
			m_shieldstore = 0;
			m_build_category_at_begin_turn = -1;
		} else if(m_buildInfrastructure && (m_build_queue.GetHead() && m_build_queue.GetHead()->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE)) {
			m_pw_from_infrastructure = sint32(ceil(double(m_shieldstore) * grec->GetInfrastructureCoefficient()));
			g_player[m_owner]->m_materialPool->AddMaterials(m_pw_from_infrastructure);
			m_shieldstore = 0;
			m_build_category_at_begin_turn = -2;
		} else {
			m_buildCapitalization = FALSE;
			m_gold_from_capitalization = 0;

			m_buildInfrastructure = FALSE;
			m_pw_from_infrastructure = 0;

			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
								   GEV_BuildFront,
								   GEA_City, m_home_city,
								   GEA_End);
		}
	}
	m_shieldstore_at_begin_turn = m_shieldstore;

	m_isInjoined = FALSE;
}

sint32 CityData::BeginTurn()
{
	InitBeginTurnVariables();
	DoTurnCounters();

	
    
    ImprovementHealUnitsInCity();
    ImprovementRefuelUnitsInCity();

    MapPoint pos;
    m_home_city.GetPos(pos); 

	m_pw_from_infrastructure = 0;
	m_gold_from_capitalization = 0;

	TryToBuild();

    CalcPollution(); 
	DoLocalPollution();

    
	
	
	
	
    
	ProcessFood();

	DoSupport(false);

	SplitScience(false);

	
	
    CollectOtherTrade(FALSE); 

    EatFood(); 

    if (GrowOrStarve()) {    
        if (PopCount() < 1) { 
            SlicObject *so = new SlicObject("265CityDestroyedByStarving") ;
            so->AddRecipient(GetOwner()) ;
            so->AddCity(m_home_city) ;
            g_slicEngine->Execute(so) ;

            return FALSE; 
        }
    }

    

	
	if (IsCelebratingHappiness()) {
		if(m_lastCelebrationMsg < 0 || (m_lastCelebrationMsg + 10 < g_turn->GetRound())) {
            SlicObject *so = new SlicObject("40CityIsCelebratingHappiness") ;
            so->AddCity(m_home_city) ;
            so->AddRecipient(m_owner) ;
			so->AddPlayer(m_owner);
            g_slicEngine->Execute(so) ;
			m_lastCelebrationMsg = sint16(g_turn->GetRound());
        }

		g_player[m_owner]->m_score->AddCelebration(); 
	}

	
	if (!m_build_queue.GetHead() && !m_buildCapitalization && !m_buildInfrastructure && !m_sentInefficientMessageAlready)
	{
		SlicObject *so = new SlicObject("37CityQueueIsEmpty") ;
		so->AddCity(m_home_city);
		so->AddCivilisation(m_owner);
		so->AddRecipient(m_owner) ;
		g_slicEngine->Execute(so) ;
		g_slicEngine->RunWastingWorkTriggers(m_home_city);
	}
	m_sentInefficientMessageAlready = FALSE;

	CheckForSlaveUprising();

	
	
	CityInfluenceIterator it(pos, m_sizeIndex);
	Cell *cell;
	for(it.Start(); !it.End(); it.Next()) {
		cell = g_theWorld->GetCell(it.Pos());

		
		if ((m_terrainImprovementWasBuilt == FALSE) &&
			(it.Pos() != pos) &&
			(cell->GetNumDBImprovements() > 0) &&
			(cell->GetCityOwner() == GetHomeCity()))
			m_terrainImprovementWasBuilt = TRUE;
	}

    
	

	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);

    return TRUE;
}

void CityData::EndTurn()
{
	if(m_sellBuilding >= 0) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SellBuilding,
							   GEA_City, m_home_city,
							   GEA_Int, m_sellBuilding,
							   GEA_End);
		m_sellBuilding = -1;
	}

	if(m_buyFront) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BuyFront,
							   GEA_City, m_home_city,
							   GEA_End);

		m_buyFront = false;
	}

    m_build_queue.EndTurn();
}











































































void CityData::CalcHappiness(sint32 &virtualGoldSpent, BOOL isFirstPass)

{ 
	if(isFirstPass) {
		sint32 wage = sint32(g_player[m_owner]->GetWagesPerPerson());
		int n;
		n = PopCount();
	}

    sint32 delta_martial_law; 
    m_happy->CalcHappiness(*this, FALSE, delta_martial_law, isFirstPass);
}

void CityData::CheckRiot()
{






	m_is_rioting = FALSE;

	
	if(m_home_city.Flag(k_UDF_CANT_RIOT_OR_REVOLT))
		return;

	if(g_slicEngine->GetTutorialActive() &&
	   m_owner == g_slicEngine->GetTutorialPlayer())
		return;

	if(m_happy->GetHappiness() < g_theConstDB->GetRiotLevel()) {
		if(g_rand->Next(100) < ((g_theConstDB->GetRiotLevel() - m_happy->GetHappiness()) * 
								g_player[m_owner]->GetRiotChance())) {
			m_is_rioting = TRUE;
			
			SlicObject *so = new SlicObject("100CityRioting");
			so->AddCity(m_home_city);
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
			g_player[m_owner]->m_score->AddRiot();
		}
	}
}

BOOL CityData::BuildUnit(sint32 type)
{
	
	if(!CanBuildUnit(type))
		return FALSE;

	m_buildInfrastructure = FALSE;
	m_buildCapitalization = FALSE;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD, type, m_home_city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILDING_UNIT, 
									  type, (uint32)m_home_city));
		g_network.Unblock(m_owner);
	}

	const UnitRecord* rec = g_theUnitDB->Get(type);
	Assert(rec);
    if(rec == NULL)
		return FALSE;

	if(g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex())) {
		DPRINTF(k_DBG_GAMESTATE, ("City: Building %s\n", 
								  g_theStringDB->GetNameStr(rec->m_name)));
		sint32 cost = rec->GetShieldCost();
		 return m_build_queue.InsertTail(k_GAME_OBJ_TYPE_UNIT, type, cost);
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("City: Can't build %s until %s is discovered\n",
								  g_theStringDB->GetNameStr(rec->m_name),
								  g_theAdvanceDB->GetNameStr(rec->GetEnableAdvanceIndex())));
        return FALSE; 
	}
}

BOOL CityData::BuildImprovement(sint32 type)
{
	Assert(CanBuildBuilding(type));
	if(!CanBuildBuilding(type))
		return FALSE;

	m_buildInfrastructure = FALSE;
	m_buildCapitalization = FALSE;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_IMP, type, 
										   m_home_city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_IMP, m_owner, type, 
									  m_home_city));
		g_network.Unblock(m_owner);
	}

	const BuildingRecord* rec = g_theBuildingDB->Get(type);
	Assert(rec);
	if(rec == NULL)
		return FALSE;

	if(g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex())) {
		DPRINTF(k_DBG_GAMESTATE, ("City: Building improvement %s\n",
								  g_theStringDB->GetNameStr(rec->GetName())));
		sint32 cost = rec->GetProductionCost();
		return m_build_queue.InsertTail(k_GAME_OBJ_TYPE_IMPROVEMENT, type, cost);
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("City: Can't build %s until %s is discovered\n",
								  g_theStringDB->GetNameStr(rec->GetName()),
								  g_theAdvanceDB->GetNameStr(rec->GetEnableAdvanceIndex())));
        return FALSE; 
	}
}


BOOL CityData::BuildWonder(sint32 type)
{
	Assert(CanBuildWonder(type));
	if(!CanBuildWonder(type))
	{
#ifdef _DEBUG
		char error_msg[300];

		
		sprintf(error_msg, "Cannot build wonder %d", 
			type);

		_RPT0(_CRT_WARN, error_msg);
#endif
		return FALSE;
	}

	m_buildInfrastructure = FALSE;
	m_buildCapitalization = FALSE;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_WONDER,
										   (uint32)m_home_city, type));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_WONDER,
									  (uint32)m_home_city, type));
		g_network.Unblock(m_owner);
	}

	const WonderRecord* rec = wonderutil_Get(type);
	Assert(rec);
	if(rec == NULL)
		return FALSE;

	DPRINTF(k_DBG_GAMESTATE, ("City %lx: building wonder %s\n", (uint32)m_home_city,
							  g_theStringDB->GetNameStr(rec->m_name)));

    if(g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex())) {
    	return m_build_queue.InsertTail(k_GAME_OBJ_TYPE_WONDER, type, rec->GetProductionCost());
    } else { 
        return FALSE; 
    } 

}





























void CityData::AddWonder(sint32 type)
{
	m_builtWonders |= (uint64(1) << type);
}

BOOL CityData::ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type)
{
	m_buildInfrastructure = FALSE;
	m_buildCapitalization = FALSE;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_CHANGE_BUILD,
										   (uint32)m_home_city, category,
										   item_type));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CHANGE_BUILD,
									  m_owner, (uint32)m_home_city,
									  category, item_type));
		g_network.Unblock(m_owner);
	}

    sint32 cost; 
    const UnitRecord* urec = NULL;
    const WonderRecord* wrec = NULL;
    const BuildingRecord* irec = NULL;
	const EndGameRecord *egrec = NULL;
	uint32 oldCategory;
	if(m_build_queue.GetHead())
		oldCategory = m_build_queue.GetHead()->m_category;
	else
		oldCategory = -5;

    switch(category) { 
    case k_GAME_OBJ_TYPE_UNIT:
	    urec = g_theUnitDB->Get(item_type);
    	Assert(urec);

		if(!urec)
			return FALSE;

		Assert(CanBuildUnit(item_type));
		if(!CanBuildUnit(item_type)) {
			return FALSE;
		}
		
	    if(g_player[m_owner]->HasAdvance(urec->GetEnableAdvanceIndex())) {
    		cost = urec->GetShieldCost();
		    m_build_queue.ReplaceHead(k_GAME_OBJ_TYPE_UNIT, item_type, cost);
	    } else {
            return FALSE; 
	    }
        break; 
    case k_GAME_OBJ_TYPE_WONDER:
	    wrec = wonderutil_Get(item_type);
    	Assert(wrec);

		if(!CanBuildWonder(item_type))
			return FALSE;

	    if(g_player[m_owner]->HasAdvance(wrec->GetEnableAdvanceIndex())) {
    		cost = wrec->GetProductionCost();
		    m_build_queue.ReplaceHead(k_GAME_OBJ_TYPE_WONDER, item_type, cost);
	    } else {
            return FALSE; 
	    }
        break; 

    case k_GAME_OBJ_TYPE_IMPROVEMENT:
	    irec = g_theBuildingDB->Get(item_type);
    	Assert(irec);

        
        if ((buildingutil_GetDesignatesCapitol((uint64)1 << item_type)) &&
            (g_player[m_owner]->m_capitol->m_id != (0))) {
			SlicObject *so = new SlicObject("38IACapitolWarning");
            so->AddCity(*(g_player[m_owner]->m_capitol));
			so->AddCity(m_home_city);
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
        }

	    if(CanBuildBuilding(item_type)) {
    		cost = irec->GetProductionCost();
		    m_build_queue.ReplaceHead(k_GAME_OBJ_TYPE_IMPROVEMENT, item_type, cost);
	    } else {
            return FALSE; 
	    }
        break; 
	










	case k_GAME_OBJ_TYPE_CAPITALIZATION:
		if(CanBuildCapitalization()) {
			BuildCapitalization();
			m_shieldstore = 0;
			return TRUE;
		} else {
			return FALSE;
		}
	case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
		if(CanBuildInfrastructure()) {
			BuildInfrastructure();
			m_shieldstore = 0;
			return TRUE;
		} else {
			return FALSE;
		}
    default:
#pragma warning (disable : 4127)									
        Assert(0); 
#pragma warning (default : 4127)									
        break; 
    } 

	if(category == m_build_category_at_begin_turn) {
		m_shieldstore = m_shieldstore_at_begin_turn;
	} else {
		m_shieldstore = int(double(m_shieldstore_at_begin_turn) * g_theConstDB->GetChangeCurrentlyBuildingItemPenalty()); 
	}
    return TRUE;
}


void CityData::DestroyCapitol()

{
	if(buildingutil_GetDesignatesCapitol(m_built_improvements)) {
		uint64 i;
		for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
			if(buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)i) &&
			   m_built_improvements & uint64((uint64)1 << i)) {
				m_built_improvements &= ~((uint64)1 << i);
			}
		}
	}
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);
}

void CityData::DestroyImprovement(sint32 imp)
{
	if(!(m_built_improvements & ((uint64)1 << uint64(imp))))
		return;

	
	g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_BuildingRemoved,
		GEA_City, m_home_city.m_id,
		GEA_Int, imp,
		GEA_End);

	m_built_improvements &= ~((uint64)1 << uint64(imp));

	m_build_queue.RemoveIllegalItems();

    g_player[m_owner]->RegisterLostBuilding(m_home_city, imp); 

    
	
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);
}


void CityData::NewGovernment(sint32 government_type)
{
	m_build_queue.RemoveIllegalItems(TRUE);

	if (government_type == g_player[m_owner]->m_government_type)
		return;

	

    g_player[m_owner]->RegisterNewGovernment(m_home_city, government_type); 
}


double CityData::GetDefendersBonus() const

{ 
















    return m_defensiveBonus * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetDefenseCoef();
} 

double CityData::GetDefendersBonusNoWalls() const
{
	double b;
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), b);
	return b;
}


void CityData::ImprovementHealUnitsInCity() const

{
#if 0
    static CellUnitList a; 
	a.Clear();

    MapPoint pos; 

    m_home_city.GetPos(pos); 
    g_theWorld->GetArmy(pos, a); 

    sint32 n = a.Num(); 
    sint32 i; 
    for (i=0; i<n; i++) { 
        if (buildingutil_GetMovementTypeIsHealed(GetEffectiveBuildings(), a[i])) { 
          a[i].SetHPToMax(); 
        }
    }
#endif
}

void CityData::ImprovementRefuelUnitsInCity() const
{
#if 0
	static CellUnitList a; 
	a.Clear();

    MapPoint pos; 

    m_home_city.GetPos(pos); 
    g_theWorld->GetArmy(pos, a); 

    sint32 n = a.Num(); 
    sint32 i; 
    for (i=0; i<n; i++) { 
        if (buildingutil_GetMovementTypeIsRefueled(GetEffectiveBuildings(), a[i])) { 
          a[i].SetFuelToMax(); 
        }
    }
#endif
}












sint32 CityData::ImprovementCanRefuel(const Unit &u) const 
{
   return TRUE;
}

void CityData::AddHappyTimer(sint32 turns, double adjust,
							 HAPPY_REASON reason)
{
	m_happy->AddTimer(turns, adjust, reason);
}

double CityData::GetImprovementCrimeMod() const
{
	return buildingutil_GetLowerCrime(GetEffectiveBuildings());
}

sint32 CityData::GetImprovementPeaceMod() const
{
	return sint32(buildingutil_GetLowerPeaceMovement(GetEffectiveBuildings()));
}

sint32 CityData::GetPollution() const
{
	return m_total_pollution;
}

sint32 CityData::GetPopulationPollution() const
{
	return m_cityPopulationPollution;
}

sint32 CityData::GetProductionPollution() const
{
	return m_cityIndustrialPollution;
}

void CityData::RemoveImprovements(const MapPoint &pos, sint32 owner)
{
	if(g_theWorld->GetCell(pos)->GetEnv() & k_BIT_ENV_INSTALLATION) {
		DynamicArray<Installation> instArray;
		g_theInstallationTree->GetAt(pos, instArray);
		instArray.KillList();
	}
	g_theWorld->GetCell(pos)->SetEnv(
		g_theWorld->GetCell(pos)->GetEnv() & ~(k_MASK_ENV_ROAD | 
											   k_MASK_ENV_IRRIGATION |
											   k_MASK_ENV_MINE | 
											   k_MASK_ENV_INSTALLATION |
											   k_MASK_ENV_CITY_RADIUS |
											   k_MASK_ENV_CANAL_TUNNEL));
	if(g_network.IsHost()) {
		g_network.Enqueue(g_theWorld->GetCell(pos), pos.x, pos.y);
	}
}

void CityData::CityRadiusFunc(const MapPoint &pos)
{
	switch(m_cityRadiusOp) {
		case RADIUS_OP_REMOVE_IMPROVEMENTS:
			g_theWorld->CutImprovements(pos);
			break;
		case RADIUS_OP_KILL_UNITS:
			if(g_theWorld->GetCell(pos)->UnitArmy()) {
				sint32 i;
				CellUnitList *units = g_theWorld->GetCell(pos)->UnitArmy();
				for(i = 0; i < units->Num(); i++) {
					
					if(!units->Access(i)->Flag(k_UDF_MAD_LAUNCHED))
						m_killList->Insert(units->Access(i));
				}
			}
			break;
		case RADIUS_OP_RESET_OWNER:
		{
#if 0
			Cell *cell = g_theWorld->GetCell(pos);
			if(cell->GetOwner() == m_owner) {
				MapPoint myPos;
				m_home_city.GetPos(myPos);
				
				if((pos == myPos) ||
				   !g_theWorld->IsInsideCityRadiusOfPlayerOtherThan(pos, m_radiusNewOwner, m_home_city)) {
					cell->SetOwner(-1);
					g_theWorld->ChangeOwner(pos, -1, m_radiusNewOwner);
				}
			}
#endif	
			break;
		}
		case RADIUS_OP_KILL_TILE:
		{
			MapPoint cpos;
			m_home_city.GetPos(cpos);
			if(cpos == pos)
				break;

			if(m_tilecount == m_whichtile) {
				Cell *cell = g_theWorld->GetCell(pos);
				if(cell->GetCanDie()) {
                    MapPoint tmp = pos; 




					cell->Kill();

					g_theWorld->CutImprovements(pos);

					

					cell->CalcTerrainMoveCost();
					MapPoint nonConstPos = pos;
					
					g_tiledMap->PostProcessTile(nonConstPos, g_theWorld->GetTileInfo(nonConstPos));
					g_tiledMap->TileChanged(nonConstPos);
					MapPoint npos;
					for(WORLD_DIRECTION d = NORTH; d < NOWHERE; 
						d = (WORLD_DIRECTION)((sint32)d + 1)) {
						if(pos.GetNeighborPosition(d, npos)) {
							g_tiledMap->PostProcessTile(
								npos, 
								g_theWorld->GetTileInfo(npos));
							g_tiledMap->TileChanged(npos);
							g_tiledMap->RedrawTile(&npos);
						}
					}
					g_tiledMap->RedrawTile(&pos);

					if(g_network.IsHost()) {
						g_network.Block(m_owner);
						g_network.Enqueue(g_theWorld->GetCell(pos), pos.x, pos.y);
						g_network.Unblock(m_owner);
					}
				}
			}
			m_tilecount++;
			break;
		}
		case RADIUS_OP_ADD_GOODS:
			if(g_theWorld->IsGood(pos)) {
				sint32 good;
				g_theWorld->GetGood(pos, good);
				if(!m_tempGoodAdder->HaveGoodOfType(good))
					m_tempGoodAdder->AddGood(good);
			}
			break;
		case RADIUS_OP_COUNT_GOODS:
			if(g_theWorld->IsGood(pos)) {
				sint32 good;
				g_theWorld->GetGood(pos, good);
				if(good == m_tempGood || m_tempGood < 0)
					m_tempGoodCount++;
			}
			break;
		default:
#pragma warning (disable : 4127)									
			Assert(FALSE);
#pragma warning (default : 4127)									
			break;
	}
}

void CityData::CityToPark(sint32 agressor)
{
	
	MapPoint cpos;
	m_home_city.GetPos(cpos);

	m_cityRadiusOp = RADIUS_OP_REMOVE_IMPROVEMENTS;
	CityRadiusIterator(cpos, this);

	static UnitDynamicArray killList;
	killList.Clear();
	m_killList = &killList;
	m_cityRadiusOp = RADIUS_OP_KILL_UNITS;
	CityRadiusIterator(cpos, this);
	
	UnitDynamicArray	tempKillList;
	tempKillList.Clear();
	tempKillList.Concat(*m_killList);

	m_home_city.Kill(CAUSE_REMOVE_ARMY_PARKRANGER, agressor);
	sint32 i;
	for(i = 0; i < tempKillList.Num(); i++) {
		
		
		
		if(g_theUnitPool->IsValid(tempKillList[i])) {
			tempKillList[i].Kill(CAUSE_REMOVE_ARMY_PARKRANGER, agressor);
		}
	}
}

void CityData::GetNuked(UnitDynamicArray &killList)
{
	MapPoint cpos;

	m_home_city.GetPos(cpos);
	m_killList = &killList;

	
	
	
	sint32 pn = PopCount();
	sint32 kill = sint32(double(pn) * g_theConstDB->NukePopulationPercentage());

	if(pn >= 3) {
		sint32 i;
		for(i = 0; i < kill; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
								   GEA_City, m_home_city.m_id,
								   GEA_End);
		}
		
	} else {
		
		killList.Insert(m_home_city);
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillTile,
							   GEA_MapPoint, m_home_city.RetPos(),
							   GEA_End);
	}

	m_cityRadiusOp = RADIUS_OP_KILL_UNITS;
	AdjacentIterator(cpos, this);

	m_cityRadiusOp = RADIUS_OP_REMOVE_IMPROVEMENTS;
	AdjacentIterator(cpos, this);

	g_thePollution->AddNukePollution(cpos);
}

BOOL CityData::SafeFromNukes() const
{
	return buildingutil_GetProtectFromNukes(GetEffectiveBuildings());
}

BOOL CityData::HasAirport() const
{
	return buildingutil_GetAirport(GetEffectiveBuildings());
}

BOOL CityData::HasCityWalls() const
{
	return buildingutil_GetCityWalls(GetEffectiveBuildings());
}

BOOL CityData::HasForceField() const
{
	return buildingutil_GetForceField(GetEffectiveBuildings());
}

void CityData::UseAirport()
{
	m_airportLastUsed = g_turn->GetRound();
}

sint32 CityData::AirportLastUsed() const
{
	return m_airportLastUsed;
}

BOOL CityData::HasBeenSpiedUpon() const
{
	return m_spied_upon != 0;
}

void CityData::SetSpiedUpon()
{
	m_spied_upon = g_theConstDB->GetSpiedUponWarinessTimer();
}

void CityData::CityNullifyWalls()
{
	m_walls_nullified = TRUE;
}

BOOL CityData::IsCapitol() const
{
	return buildingutil_GetDesignatesCapitol(GetEffectiveBuildings());
}

void CityData::SetCapitol(const BOOL delay_registration)
{
	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(g_theBuildingDB->Get(i)->GetCapitol()) {
            
			m_built_improvements |= ((uint64)1 << i);

            if (!delay_registration) { 
                g_player[m_owner]->RegisterCreateBuilding(m_home_city, i); 
                g_player[m_owner]->RegisterNewCapitolBuilding(m_home_city); 
            }
			return;
		}
	}
}

void CityData::MakeFranchise(sint32 player)
{
	m_franchise_owner = player;
	m_franchiseTurnsRemaining = -1;
}

sint32 CityData::GetFranchiseTurnsRemaining() const
{
	return m_franchiseTurnsRemaining;
}

void CityData::SetFranchiseTurnsRemaining(sint32 turns)
{
	m_franchiseTurnsRemaining = turns;
	if(turns < 1) {
		m_franchiseTurnsRemaining = -1;
		m_franchise_owner = -1;
	}
}

BOOL CityData::IsWatchful() const
{
	return (m_watchfulTurns > 0);
}

void CityData::SetWatchful()
{
	m_watchfulTurns = g_theConstDB->WatchfulCityTurns();
}

void CityData::ModifySpecialAttackChance(UNIT_ORDER_TYPE attack,
										 double &chance)
{
	if(IsWatchful()) {
		chance *= g_theConstDB->WatchfulCitySuccessModifier();
	}
	switch(attack) {
		case UNIT_ORDER_BIO_INFECT:
		case UNIT_ORDER_PLAGUE:
			chance -= buildingutil_GetProtectFromBioAgents(GetEffectiveBuildings());
			break;
		case UNIT_ORDER_NANO_INFECT:
			chance -= buildingutil_GetProtectFromNanoVirus(GetEffectiveBuildings());
			break;
		case UNIT_ORDER_SLAVE_RAID:
			chance -= buildingutil_GetPreventSlavery(GetEffectiveBuildings());
			break;
	}
}

void CityData::RemoveOneSlave(PLAYER_INDEX p)
{
	if(SlaveCount() < 1)
		return;

	ChangeSpecialists(POP_SLAVE, -1);

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
						   GEA_City, m_home_city.m_id,
						   GEA_End);
	
}

void CityData::DoUprising(UPRISING_CAUSE cause)
{
	if(SlaveCount() < 1)
		return;

	
	if(g_slicEngine->GetTutorialActive() &&
	   m_owner == g_slicEngine->GetTutorialPlayer()) {
		return; 
	}

	static CellUnitList slaveArmy;
	slaveArmy.Clear();

	sint32 oldOwner = m_owner;

	if(cause == UPRISING_CAUSE_INTERNAL)
		cause = m_doUprising;

	m_doUprising = UPRISING_CAUSE_NONE;

	sint32 cheapUnit;
	cheapUnit = g_player[m_owner]->GetCheapestMilitaryUnit();
	Assert(cheapUnit >= 0);
	if(cheapUnit < 0)
		return;
	
	sint32 numSlaves = SlaveCount();
	PLAYER_INDEX si = civilisation_NewCivilisationOrVandals(m_owner);
	
	MapPoint cpos;
	m_home_city.GetPos(cpos);

	if(numSlaves > k_MAX_ARMY_SIZE)
		numSlaves = k_MAX_ARMY_SIZE;
	for(sint32 i = 0; i < numSlaves; i++) {
		Unit u = g_player[si]->CreateUnitNoPosition(cheapUnit,
													slaveArmy,
													cpos,
													m_owner);
		Assert(u.m_id != (0));
		if(u.m_id != 0) {
			u.SetPosAndNothingElse(cpos);
			u.SetTempSlaveUnit(TRUE);
		}
	}
	
	Cell *cell = g_theWorld->GetCell(cpos);

	Army sa = g_player[si]->GetNewArmy(CAUSE_NEW_ARMY_UPRISING); 
	for(i = 0; i < numSlaves; i++) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AddUnitToArmy,
							   GEA_Unit, slaveArmy[i],
							   GEA_Army, sa,
							   GEA_Int, CAUSE_NEW_ARMY_UPRISING,
							   GEA_End);
	}

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_FinishUprising,
						   GEA_City, m_home_city,
						   GEA_Army, sa,
						   GEA_Int, cause,
						   GEA_End);
}

void CityData::FinishUprising(Army &sa, UPRISING_CAUSE cause)
{
	sint32 oldOwner = m_owner;
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());
	sint32 numPossibleDefenders = cell->GetNumUnits();

	static CellUnitList defenders;
	defenders.Clear();

	sint32 si = sa->GetOwner();

	
	
	if(g_network.IsHost()) {
		g_network.Block(oldOwner);
		g_network.Enqueue(g_theArmyPool->AccessArmy(sa));
		g_network.Unblock(oldOwner);
	}

	sint32 i;

	bool startedBattle = false;
	if(numPossibleDefenders > 0) {
		for(i = 0; i < numPossibleDefenders; i++) {
			if(cell->AccessUnit(i).GetDBRec()->GetMovementTypeLand()) {
				defenders.Insert(cell->AccessUnit(i));
			}
		}

		if(defenders.Num() > 0) {
			sa.Fight(defenders);
			startedBattle = true;
		}

	}

	if(cause != UPRISING_CAUSE_INCITED) {
		SlicObject *so = new SlicObject("206CrisisSlaveRevolt");
		so->AddCity(m_home_city);
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
	}
		
	if(!startedBattle) {
		CleanupUprising(sa);
	} else {
		sa->IncrementDontKillCount();
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CleanupUprising,
							   GEA_Army, sa,
							   GEA_City, m_home_city.m_id,
							   GEA_End);
	}
}

void CityData::CleanupUprising(Army &sa)
{
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());
	sint32 i;

	sint32 oldOwner = m_owner;
	if(!g_theArmyPool->IsValid(sa) || sa.Num() < 1) {
		DPRINTF(k_DBG_GAMESTATE, ("The uprising was crushed\n"));
		
		RemoveOneSlave(GetOwner());
		
		
		
	} else {
		sint32 si = sa.GetOwner();
		DPRINTF(k_DBG_GAMESTATE, ("The uprising succeeded\n"));
		
		
		
		if(cell->UnitArmy()) {
			cell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_SLAVE_UPRISING, GetOwner());
		}

		
		for(i = sa.Num() - 1; i >= 0; i--) {
			if(sa[i].GetHP() < 1)
				sa.DelIndex(i);
			else {
				BOOL revealed;
				sa[i].SetTempSlaveUnit(FALSE);
				sa[i].SetPosAndNothingElse(m_home_city.RetPos());
				sa[i].AddUnitVision(revealed);
				
				static UnitDynamicArray revealedUnits;
				revealedUnits.Clear();

				g_theWorld->InsertUnit(m_home_city.RetPos(), sa[i], revealedUnits);
				g_player[sa.GetOwner()]->InsertUnitReference(sa[i], 
												  CAUSE_NEW_ARMY_UPRISING,
												  m_home_city);
				if(g_network.IsHost()) {
					g_network.Block(oldOwner);
					g_network.Enqueue(new NetInfo(NET_INFO_CODE_MAKE_UNIT_PERMANENT,
												  sa[i].m_id));
					g_network.Unblock(oldOwner);
				}
			}
		}
		sa.ResetPos();
		
		ChangeSpecialists(POP_SLAVE, -SlaveCount());

		m_home_city.ResetCityOwner(si, FALSE, CAUSE_REMOVE_CITY_SLAVE_UPRISING);
	}

}

void CityData::Plague(sint32 player)
{
	sint32 pn = PopCount();
	sint32 kill = sint32(double(pn) * g_theConstDB->BioTerrorKillPercentage());

	if(kill < 1 && pn > 1) {
		kill = 1;
	}

	sint32 i;
	for(i = 0; i < kill; i++) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
							   GEA_City, m_home_city.m_id,
							   GEA_End);
		
	}

}

void CityData::BioInfect( sint32 player )
{

	m_bioInfectedBy = player;
	m_bioInfectionTurns = g_theConstDB->BioInfectionTurns();
	AddHappyTimer(g_theConstDB->BioInfectionTurns(),
				  g_theConstDB->BioInfectionUnhappiness(), 
				  HAPPY_REASON_BIO_INFECTION);
}

void CityData::NanoInfect( sint32 player )
{
	sint32 i;

	m_nanoInfectedBy = player;
	m_nanoInfectionTurns = g_theConstDB->NanoInfectionTurns();
	DPRINTF(k_DBG_GAMESTATE, ("City %lx: all buildings and wonders destroyed\n", uint32(m_home_city)));
	
	for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(m_built_improvements & ((uint64)1 << i)) {
			if(g_rand->Next(100) < (g_theConstDB->GetNanoBuildingKillPercentage() * 100.0)) {
				DestroyImprovement(i);
			}
		}
	}

	
	

    
    
    
    
    
    
    
}

void CityData::SpreadBioTerror()
{
    Unit *c;
    sint32 i, n = m_tradeSourceList.Num();
    for(i = 0; i < n; i++) {
        c = &(m_tradeSourceList[i].GetDestination());
        if ((c->IsBioImmune()) || (c->IsBioInfected()))
            continue;

        if(g_rand->Next(100) < sint32(g_theConstDB->BioInfectionSpreadChance()
                                      * 100.0)) {
            c->BioInfect(0);
            SlicObject *so = new SlicObject("047InfectedViaTrade");
            so->AddCity(*c);
            so->AddRecipient(c->GetOwner());
            g_slicEngine->Execute(so);
        }
    }
}

void CityData::SpreadNanoTerror()
{
    Unit *c;
    sint32 i, n = m_tradeSourceList.Num();
    for(i = 0; i < n; i++) {
        c = &(m_tradeSourceList[i].GetDestination());
        if ((c->IsNanoImmune()) || (c->IsNanoInfected()))
            continue;

        if(g_rand->Next(100) < sint32(g_theConstDB->NanoInfectionSpreadChance()
                                      * 100.0)) {
            c->NanoInfect(0);
            SlicObject *so = new SlicObject("047InfectedViaTrade");
            so->AddCity(*c);
            so->AddRecipient(c->GetOwner());
            g_slicEngine->Execute(so);
        }
    }
}

BOOL CityData::IsBioImmune() const
{
	return 	wonderutil_GetProtectFromBiologicalWarfare(g_player[m_owner]->m_builtWonders);
}

BOOL CityData::IsNanoImmune() const
{
	return 	wonderutil_GetProtectFromBiologicalWarfare(g_player[m_owner]->m_builtWonders);
}

bool CityData::IsProtectedFromConversion()
{
	return wonderutil_PreventConversion(g_player[m_owner]->m_builtWonders) == TRUE;
}

void CityData::ConvertTo(sint32 player, CONVERTED_BY by)
{
	if(IsProtectedFromConversion())
		return;

	m_convertedTo = player;
	m_convertedBy = by;
}

double CityData::TheologicalModifier() const
{
	return buildingutil_GetPreventConversion(GetEffectiveBuildings());
}

void CityData::Unconvert(BOOL makeUnhappy)
{
	if(m_convertedTo < 0)
		return; 

	m_convertedTo = -1;
	m_convertedGold = 0;
	if(makeUnhappy) {
		AddHappyTimer(g_theConstDB->ReformationHappinessTime(),
					  g_theConstDB->ReformationHappinessAmount(),
					  HAPPY_REASON_REFORMATION);
	}
}

BOOL CityData::HasResource(sint32 resource) const
{
	return m_collectingResources[resource] > m_sellingResources[resource];
}

BOOL CityData::IsLocalResource(sint32 resource) const
{
	return m_collectingResources[resource] > 0;
}



bool CityData::GetResourceTradeRoute(sint32 resource, TradeRoute & route) const
{
	sint32 i;
	for(i = 0; i < m_tradeSourceList.Num(); i++) {
		ROUTE_TYPE type;
		sint32 rr;
		m_tradeSourceList[i].GetSourceResource(type, rr);
		if(type != ROUTE_TYPE_RESOURCE) continue;
		if(rr != resource) continue;

		
		route = m_tradeSourceList[i];
		return true;
	}

	
	return false;
}


bool CityData::IsSellingResourceTo(sint32 resource, Unit & destination) const
{
	sint32 i;
	for(i = 0; i < m_tradeSourceList.Num(); i++) {
		ROUTE_TYPE type;
		sint32 rr;
		m_tradeSourceList[i].GetSourceResource(type, rr);
		if(type != ROUTE_TYPE_RESOURCE) continue;
		if(rr != resource) continue;

		
		destination.m_id = m_tradeSourceList[i].GetDestination().m_id;
		return true;
	}

	
	destination.m_id = 0;
	return false;
}












sint32 CityData::LoadQueue(const MBCHAR *file)
{
	sint32	r ;

	r = m_build_queue.Load(file) ;
	m_build_queue.SetOwner(m_owner) ;								

	return (r) ;
}













sint32 CityData::SaveQueue(const MBCHAR *file)
{
	sint32	r ;

	r = m_build_queue.Save(file) ;
	m_build_queue.SetOwner(m_owner) ;								

	return (r) ;
}

	












uint32 CityData_CityData_GetVersion(void)
	{
	return (k_CITYDATA_VERSION_MAJOR<<16 | k_CITYDATA_VERSION_MINOR) ;
	}


void CityData::ResetCityOwner(sint32 owner)
{
	sint32	i, n ;

	m_capturedThisTurn = TRUE;

	m_cityRadiusOp = RADIUS_OP_RESET_OWNER;
	m_radiusNewOwner = owner;
	MapPoint pos;
	m_home_city.GetPos(pos);
	CityRadiusIterator(pos, this);

	m_doUprising = UPRISING_CAUSE_NONE;

	
	
	CityInfluenceIterator it(pos, m_sizeIndex);
	for(it.Start(); !it.End(); it.Next()) {
		g_theWorld->GetCell(it.Pos())->SetCityOwner(m_home_city);
		g_theWorld->GetCell(it.Pos())->SetOwner(owner);
		g_network.Block(owner);
		g_network.Enqueue(g_theWorld->GetCell(it.Pos()), it.Pos().x, it.Pos().y);
		g_network.Unblock(owner);
	}

	
	RemoveBorders();

	if (m_owner != owner)
	{
		g_theWorld->SetCapitolDistanceDirtyFlags(1<<owner);
		m_owner = (PLAYER_INDEX)owner;
	}

	
	n = m_tradeDestinationList.Num() ;
	for(i = n-1; i >= 0; i--)
		m_tradeDestinationList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_CHANGED_OWNER) ;							

	
	n = m_tradeSourceList.Num() ;
	for(i = n-1; i >= 0; i--)
		m_tradeSourceList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_CHANGED_OWNER) ; 

    m_build_queue.ResetOwner(owner); 
                                    

    	
	NewGovernment(g_player[m_owner]->m_government_type);

	m_walls_nullified = FALSE;
	m_franchiseTurnsRemaining = 0;
	m_franchise_owner = -1;
	m_watchfulTurns = 0;
	m_bioInfectionTurns = 0;
	m_bioInfectedBy = -1;
	m_nanoInfectionTurns = 0;
	m_nanoInfectedBy = -1;
	if(m_convertedTo == m_owner) {
		m_convertedTo = -1;
	}
	m_happinessAttacked = 0;
	m_isInjoined = FALSE;

	m_happy->ClearTimedChanges();

	FindBestSpecialists();

	sint32 poptype;
	for(poptype = 0; poptype < POP_SLAVE; poptype++) {		
		if(m_numSpecialists[poptype] > 0 && m_specialistDBIndex[poptype] < 0) {
			
			
			ChangeSpecialists((POP_TYPE)poptype, -m_numSpecialists[poptype]);
		}
	}

	m_lastCelebrationMsg = -1;

	GenerateBorders(m_home_city.RetPos(), m_owner, g_theConstDB->GetBorderIntRadius(), g_theConstDB->GetBorderSquaredRadius());

	if(m_owner == m_founder) {
		g_player[m_owner]->m_score->AddCityRecaptured();
		g_featTracker->AddFeat("FEAT_CITY_RECAPTURED", m_owner);
	}
}


void CityData::SetName(const MBCHAR *name)
{ 
	Assert(strlen(name)<k_MAX_NAME_LEN) ; 
	strncpy(m_name, name, k_MAX_NAME_LEN) ; 
	if(g_network.IsHost()) {
		g_network.SendCityName(this);
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendCityName(this);
	}
}


sint32 CityData::GetOvertimeCost() 
{
    
    if (m_build_queue.GetHead() == NULL) { 
		// TODO: return 0 when nothing in queue. 
		// But check usage (e.g. in ctpai.cpp) before doing so.
        return 1000000;
    }

	double type_coeff = m_build_queue.GetTypeCoeff();
	double percent_remaining = m_build_queue.GetPercentRemaining(m_shieldstore);
	sint32 production_remaining = m_build_queue.GetProductionRemaining(m_shieldstore);
	if(production_remaining < 0)
		production_remaining = 0;

	if(buildingutil_NoRushBuyPenalty(GetEffectiveBuildings())) {
		percent_remaining = 0;
		type_coeff = 1;
	}

	sint32 c = sint32(type_coeff * (1 + percent_remaining) * double(production_remaining));
	
	
    
	if(c < 0)
		c = 0;
    return c; 
}

BOOL CityData::BuyFront()
{
	if(!m_build_queue.GetHead())
		return FALSE;

#if !defined(ACTIVISION_ORIGINAL)
    // * Can't rush buy capitalization/infrastructure
	if(m_buildInfrastructure || m_buildCapitalization )
        return FALSE;
#endif

	if(m_shieldstore >= m_build_queue.GetFrontCost()) {
		
		return TRUE;
	}

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_BUY_FRONT,
										   (uint32)m_home_city));
	}

	if(!m_paidForBuyFront) {
		if(!PayForBuyFront())
			return FALSE;
	}

	
	m_shieldstore = m_build_queue.GetFrontCost();

	m_paidForBuyFront = false;

	return TRUE;
}

void CityData::RemoveFront()
{
	m_build_queue.RemoveHead();
}

void CityData::BuildWhat() const
{
#if 0
	
	
	sint32 i, n;
	Player *p = g_player[m_owner];
	sint32 enable, obsolete;

	n = g_theUnitDB->NumRecords();

	DPRINTF(k_DBG_GAMESTATE, ("Units:\n"));
	for(i = 0; i < n; i++) {
		enable = g_theUnitDB->Get(i)->m_enable;
		obsolete = g_theUnitDB->Get(i)->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0))) {
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
									  g_theStringDB->GetNameStr(g_theUnitDB->Get(i)->m_name)));
		}
	}
	
	DPRINTF(k_DBG_GAMESTATE, ("Improvements:\n"));
	n = g_theBuildingDB->NumRecords();
	for(i = 0; i < n; i++) {
		enable = g_theBuildingDB->Get(i)->m_enable;
		obsolete = g_theBuildingDB->Get(i)->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0))) {
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
									  g_theStringDB->GetNameStr(g_theBuildingDB->Get(i)->m_name)));
		}
	}

	DPRINTF(k_DBG_GAMESTATE, ("Wonders:\n"));
	n = g_theWonderDB->NumRecords();
	for(i = 0; i < n; i++) {
		const WonderRecord *rec = wonderutil_Get(i);
		enable = rec->m_enable;
		obsolete = rec->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0))) {
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
									  g_theStringDB->GetNameStr(rec->m_name)));
		}
	}
#endif
}

sint32 CityData::HowMuchLonger() const
{
	if(m_build_queue.GetLen() < 1)
		return 0x7fffffff;

	sint32 prod_remaining = m_build_queue.GetProductionRemaining(m_shieldstore);



	sint32 prod, prodBeforeCrime;
	GetDetailedProjectedProduction( prod, prodBeforeCrime );

	if(m_contribute_military) {
		sint32 mil = (sint32)ceil((double)prod * g_player[m_owner]->m_readiness->GetPecentLastTurn());
		prod -= mil;
	}

	if(m_contribute_materials) {
		sint32 mat = (sint32)ceil((double)prod * g_player[m_owner]->m_materialsTax);
		prod -= mat;
	}

	if (prod <= 0)
		return 0x7fffffff; 

	sint32 turns_remaining = (sint32)ceil((double)prod_remaining / prod);

	if ( turns_remaining < 1 ) {
		return 1;	
	}

	if(m_buyFront && m_paidForBuyFront) {
		return 1;
	}

	return turns_remaining;
}

sint32 CityData::HowMuchLonger( sint32 productionRemaining ) const
{
	sint32 prod_remaining = productionRemaining;



	sint32 prod, prodBeforeCrime;
	GetDetailedProjectedProduction( prod, prodBeforeCrime );

	if(m_contribute_military) {
		sint32 mil = (sint32)ceil((double)prod * g_player[m_owner]->m_readiness->GetPecentLastTurn());
		prod -= mil;
	}

	if(m_contribute_materials) {
		sint32 mat = (sint32)ceil((double)prod * g_player[m_owner]->m_materialsTax);
		prod -= mat;
	}

	if (prod <= 0)
		return 0x7fffffff; 

	sint32 turns_remaining = (sint32)ceil((double)prod_remaining / prod);

	if ( turns_remaining < 1 ) {
		return 1;	
	}

	return turns_remaining;
}

void CityData::SellBuilding(sint32 which, BOOL byChoice)
{







	if((m_built_improvements & ((uint64)1 << uint64(which)))) {
		if(byChoice) {
			if(m_alreadySoldABuilding)
				return;

			if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
				g_network.SendAction(new NetAction(NET_ACTION_SELL_BUILDING,
					(uint32)m_home_city, 
					which));
			} else if(g_network.IsHost()) {
				g_network.Block(m_owner);
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_SOLD_BUILDING,
											  (uint32)m_home_city, which));
			}

			m_alreadySoldABuilding = TRUE;
		}
		sint32 gold = sint32(double(g_theBuildingDB->Get(which)->GetProductionCost()) * 
			g_theConstDB->BuildingProductionToValueModifier());
		if(byChoice)
			g_player[m_owner]->m_gold->AddGold(gold);
		else
			m_trade += gold;
		m_built_improvements &= ~((uint64)1 << uint64(which));

        g_player[m_owner]->RegisterLostBuilding(m_home_city, which); 
        m_build_queue.RemoveIllegalItems(TRUE);
		if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)which))) {
			Assert(g_player[m_owner]->m_capitol->m_id == m_home_city.m_id);
			if(g_player[m_owner]->m_capitol->m_id == m_home_city.m_id) {
				g_player[m_owner]->m_capitol->m_id = 0;
			}
		}

		if(byChoice && g_network.IsHost()) {
			g_network.Unblock(m_owner);
		}
	}
    
	
}

void CityData::SetRoad() const
{
	MapPoint pos;
	m_home_city.GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);

	const TerrainImprovementRecord *rec = terrainutil_GetBestRoad(m_owner, pos);
	if(rec) {
		cell->InsertDBImprovement(rec->GetIndex());
	}
#if 0
	uint32 oenv = cell->GetEnv();
	cell->SetEnv(cell->GetEnv() & ~(k_MASK_ENV_ROAD));

	uint32 roadLevel;
	roadLevel = terrainutil_GetBestRoad(m_owner, pos);
	cell->SetEnv(cell->GetEnv() | (roadLevel << k_SHIFT_ENV_ROAD));

	if(cell->GetEnv() != oenv) {
		if(g_network.IsHost()) {
			g_network.Enqueue(cell, pos.x, pos.y);
		}
	}
#endif
}



void CityData::SetSize(sint32 size)
{
	if(size == PopCount())
		return;

	sint32 i;
	for(i = 0; i < size - PopCount(); i++) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
							   GEA_City, m_home_city.m_id,
							   GEA_End);
	}
	
}

sint32 CityData::GetNumTradeRoutes() const
{
	return m_tradeSourceList.Num() + m_tradeDestinationList.Num();
}

void CityData::AddTradedResources(Resources &resources)
{
	ROUTE_TYPE type;
	sint32 res;

	for(sint32 i = 0; i < m_tradeSourceList.Num(); i++) {
		m_tradeSourceList[i].GetSourceResource(type, res);
		if(type == ROUTE_TYPE_RESOURCE) {
			resources.AddResource(res);
		}
	}
}

sint32 CityData::GetCombatUnits() const
{
	MapPoint pos;
	m_home_city.GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 count = 0;

	for(sint32 i = 0; i < cell->GetNumUnits(); i++) {
		if(cell->AccessUnit(i).GetDBRec()->GetAttack() > 0)
			count++;
	}
	return count;
}

		
void CityData::GetProjectedProductionDeltas(sint32 &s, 
											sint32 &t, 
											sint32 &f,
											sint32 &science)
{
	
	
	s = m_shields_this_turn;
	t = m_trade;
	f = (sint32)m_gross_food_this_turn;
	science = 0;
}








void CityData::GetProjectedGrossProduction(sint32 &prod, sint32 &prod_crime)
{
}


void CityData::GetProjectedGrossFood(sint32 &food, sint32 &food_crime)
{
}


void CityData::GetProjectedGrossGold(sint32 &gold, sint32 &crime_gold)
{
}










void CityData::GetProjectedFood(sint32 &food)
{
}

void CityData::GetDetailedProjectedFood(sint32 &food, 
										sint32 &foodBeforeCrime,
										sint32 &foodBeforeCrimeAndEaten)
{
}
	








void CityData::GetProjectedProduction(sint32 &production)
{
}


void CityData::GetDetailedProjectedProduction(sint32 &production,
											  sint32 &productionBeforeCrime) const
{
	production = m_shields_this_turn;
	productionBeforeCrime = 0;
}









void CityData::GetProjectedTrade(sint32 &projected_gross_gold, sint32 &projected_net_gold)
{
}

sint32 CityData::GetDetailedProjectedTradeDeltaAdjustResource(sint32 resource, sint32 adjust)
{
	return 0;
}

void CityData::GetDetailedProjectedTradeScience(sint32 &projected_gross_gold, 
												sint32 &projected_net_gold,
												sint32 &science,
												sint32 &popScience,
												sint32 &projected_gold_before_crime,
												sint32 &projected_wages)
{
	projected_gross_gold = m_gross_trade;
	projected_net_gold = m_trade;
	popScience = GetScienceFromPops();

	double scienceRate;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(scienceRate);

	science = sint32(popScience + ((g_player[m_owner]->m_gold->GetConsiderForScience() * (double)m_trade) * scienceRate));
}









void CityData::GetProjectedTradeFromCell(MapPoint &pos, sint32 &trade)
{
}


void CityData::GetDetailedProjectedHappinessCrime(double &hap, double &crime)
{
}

void CityData::GetProjectedHappinessCrime(double &hap, double &crime, 
     sint32 &delta_martial_law)
{
}

BOOL CityData::CanHavePopType(sint32 type) const
{
	return TRUE;
}

BOOL CityData::CanHaveUIPopType(sint32 uitype) const
{
	return TRUE;
}


BOOL CityData::CanBuildUnit(sint32 type) const
{

	const UnitRecord *rec = g_theUnitDB->Get(type);
	if(!rec)
		return FALSE;

	MapPoint pos;
	m_home_city.GetPos(pos);

    if (!g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex()))
        return FALSE;

	sint32 o;
	for(o = rec->GetNumObsoleteAdvance() - 1; o >= 0; o--) {
		if(g_player[m_owner]->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
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
			if(rec->GetGovernmentTypeIndex(i) == g_player[m_owner]->m_government_type) {
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
		sint32 i, n = g_player[m_owner]->m_all_units->Num();
		for(i = 0; i < n; i++) {
			if(g_player[m_owner]->m_all_units->Access(i).GetDBRec()->GetNoSlaves())
				return FALSE;
		}
	}

	if(rec->GetNoSlaves()) {
		
		sint32 i, n = g_player[m_owner]->m_all_cities->Num();
		for(i = 0; i < n; i++) {
			if(g_player[m_owner]->m_all_cities->Access(i).CD()->SlaveCount() > 0)
				return FALSE;
		}

		n = g_player[m_owner]->m_all_units->Num();
		for(i = 0; i < n; i++) {
			if(g_player[m_owner]->m_all_units->Access(i).GetDBRec()->GetSlaveRaids())
				return FALSE;
		}
	}

	if(rec->GetCreateParks() && 
	   !wonderutil_GetParkRangersEnabled(g_theWonderTracker->GetBuiltWonders())) {
		return FALSE;
	}

	if(!g_slicEngine->CallMod(mod_CanCityBuildUnit, TRUE, m_home_city.m_id, rec->GetIndex()))
		return FALSE;

	if(!rec->GetMovementTypeLand() && !rec->GetMovementTypeTrade() && !rec->GetIsTrader()) {
		if(g_theWorld->IsWater(pos.x, pos.y) && rec->GetSeaCityCanBuild()) {
			
			
			return TRUE;
		}
		
		if((g_theWorld->IsLand(pos.x, pos.y) || 
			g_theWorld->IsMountain(pos.x, pos.y)) &&
			rec->GetLandCityCanBuild()) {
			
			return TRUE;
		}
		
		
		if(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater()) {
			
			if(g_theWorld->IsNextToWater(pos.x, pos.y)) {
				return TRUE;
			}
			
			return FALSE;
		} else if(rec->GetMovementTypeAir()) {
			return TRUE;
		}
		return FALSE;
	}

	
	return TRUE;
}

BOOL CityData::CanBuildBuilding(sint32 type) const
{
	if(g_exclusions->IsBuildingExcluded(type))
		return FALSE;

    const BuildingRecord* irec = g_theBuildingDB->Get(type);
	
	
	Assert(irec != NULL);
	if (!irec) 
		return FALSE;

	if(!g_player[m_owner]->HasAdvance(irec->GetEnableAdvanceIndex()) && irec->GetEnableAdvanceIndex() >= 0) {
		return FALSE;
	}

	sint32 o;
	for(o = 0; o < irec->GetNumObsoleteAdvance(); o++) {
		if(g_player[m_owner]->HasAdvance(irec->GetObsoleteAdvanceIndex(o)))
			return FALSE;
	}
	
	MapPoint pos;
	m_home_city.GetPos(pos);
	if(g_theWorld->IsWater(pos)) {
		if(irec->GetCantBuildInSea())
			return FALSE;
	} else {
		if(irec->GetCantBuildOnLand())
			return FALSE;
	}
	
	
	if (irec->GetCoastalBuilding()) {
		if(!g_theWorld->IsNextToWater(pos.x, pos.y))
			return FALSE;
	}

	if(m_built_improvements & uint64((uint64)1 << (uint64)type)) {
		
		return FALSE;
	}

	if((irec->GetNuclearPlant() &&
	   wonderutil_GetNukesEliminated(g_theWonderTracker->GetBuiltWonders()))) {
		return FALSE;
	}

	
	if(irec->GetNumPrerequisiteBuilding() > 0) {
		for(o = 0; o < irec->GetNumPrerequisiteBuilding(); o++) {
			sint32 b = irec->GetPrerequisiteBuildingIndex(o);
			if(!(GetEffectiveBuildings() & ((uint64)1 << (uint64)b)))
				return FALSE;
		}
	}


	return g_slicEngine->CallMod(mod_CanCityBuildBuilding, TRUE, m_home_city.m_id, irec->GetIndex());
}











































BOOL CityData::CanBuildWonder(sint32 type) const
{
	if(g_exclusions->IsWonderExcluded(type))
		return FALSE;

	if(!wonderutil_IsAvailable(type, m_owner))
		return FALSE;

	return g_slicEngine->CallMod(mod_CanCityBuildWonder, TRUE, m_home_city.m_id, type);
}

void CityData::RemoveWonderFromQueue(sint32 type)
{
	BOOL OopsSomethingUsesThisFunctionIGuess = FALSE;
	Assert(OopsSomethingUsesThisFunctionIGuess);

	m_build_queue.RemoveObjectsOfType(k_GAME_OBJ_TYPE_WONDER, type, CAUSE_REMOVE_BUILD_ITEM_ILLEGAL);
}

void CityData::Injoin(sint32 player)
{
	m_isInjoined = TRUE;
	m_injoinedBy = player;
}

BOOL CityData::IsInjoined() const
{
	return m_isInjoined;
}


BOOL CityData::HaveImprovement(const sint32 type) const
{ 
    return (GetImprovements() & (uint64(1) << type)) != 0;
}

sint32 CityData::GetPreferedPopType() const

{ 
	return 0;
}

void CityData::ResetConquestDistress(double new_distress) { m_happy->ResetConquestDistress(new_distress); } 
double CityData::GetHappiness() const { return m_happy->GetHappiness(); } 
double CityData::GetHappySize() const { return m_happy->GetSize(); }
double CityData::GetHappyPollution() const { return m_happy->GetPollution();}
double CityData::GetHappyConquestDistress() const { return m_happy->GetConquestDistress(); }
double CityData::GetHappyEmpireDist() const { return m_happy->GetEmpireDist(); }
double CityData::GetHappyEnemyAction() const { return m_happy->GetEnemyAction();  }
double CityData::GetHappyPeace() const { return m_happy->GetPeace(); }
double CityData::GetHappyWorkday() const { return m_happy->GetWorkday(); }
double CityData::GetHappyWages() const { return m_happy->GetWages(); }
double CityData::GetHappyRations() const { return m_happy->GetRations(); }
double CityData::GetHappyMartialLaw() const { return m_happy->GetMartialLaw(); } 
double CityData::GetHappyPopEntertainment() const { return m_happy->GetPopEntertainment(); }
double CityData::GetHappyImprovement() const { return m_happy->GetImprovement(); }
double CityData::GetHappyWonders() const { return m_happy->GetWonders(); }
double CityData::GetHappyCrime() const { return m_happy->GetCrime(); }  
BOOL CityData::IsCelebratingHappiness(void) const { 	return (m_happy->IsVeryHappy()) ; }

BOOL CityData::WasHappinessAttacked(void) const
{
	if(m_happinessAttacked)
		return TRUE;

	double val;
	StringID name;
	m_happy->GetHappyTracker()->GetHappiness(HAPPY_REASON_HAPPINESS_ATTACK,
											 val, name);
	return val != 0.0;
}













































































	
void CityData::RecalculateResources()
{
}

sint32 CityData::CountTradeWith(PLAYER_INDEX player) const
{
	sint32 num = 0;
	sint32 i;
	for(i = 0; i < m_tradeSourceList.Num(); i++) {
		if(m_tradeSourceList.Get(i).GetDestination().GetOwner() == player)
			num++;
	}

	for(i = 0; i < m_tradeDestinationList.Num(); i++) {
		if(m_tradeDestinationList.Get(i).GetOwner() == player)
			num++;
	}
	return num;
}

double CityData::IsProtectedFromSlavery() const
{
	return buildingutil_GetPreventSlavery(GetEffectiveBuildings());
}

void CityData::NotifyAdvance(AdvanceType advance)
{
	sint32 i;
	for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
		if(m_built_improvements & ((uint64)1 << i)) {
			const BuildingRecord *irec = g_theBuildingDB->Get(i);
			sint32 o;

			for(o = 0; o < irec->GetNumObsoleteAdvance(); o++) {
				if(irec->GetObsoleteAdvanceIndex(o) == advance) {
					SellBuilding(i, FALSE);
					break;
				}
			}
		}
	}

	m_build_queue.RemoveIllegalItems(TRUE);
	FindBestSpecialists();
}

void CityData::ContributeScience(double incomePercent,
								 double &addscience,
								 double &subgold)
{
	Assert(FALSE);

	double tradeToConsider = m_trade * incomePercent;
	Player *p = g_player[m_owner];
	double scienceRate;
	p->m_tax_rate->GetScienceTaxRate(scienceRate);
	
	addscience = tradeToConsider * scienceRate;
	subgold = addscience;

	double s;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), s);
	addscience += addscience * s;
	
}

sint32 CityData::TurnsToNextPop( void )
{
	sint32 food = 0, netFood;
	sint32 tempAccum = m_accumulated_food;

	sint32 f2, f3;
	GetDetailedProjectedFood(food, f2, f3);

	netFood = food;

	if ( netFood <= 0 ) return -1;
	if(m_is_rioting)
		return -1;

	sint32 turns = 0;
	sint32 foodNeeded = (PopCount() - SlaveCount())  * 
		CityGrowthCoefficient();

	if(g_player[m_owner]->GetPlayerType() != PLAYER_TYPE_ROBOT ||
	   g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		foodNeeded += sint32(ceil(double(foodNeeded) * 
								  g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->m_human_food_bonus));
	}

    sint32 is_silo = 0;
    sint32 silo_size = 0; 
    

	if(IsCelebratingHappiness()) {
		if(is_silo) {
			foodNeeded = (3 * foodNeeded) / 4;
		} else {
			foodNeeded = foodNeeded / 2;
		}
	}

	while ( tempAccum < foodNeeded ) {
		tempAccum += netFood;
		turns++;
	}

	return turns;
}

sint32 CityData::FreeSlaves()
{
	sint32 num = SlaveCount();
	ChangeSpecialists(POP_SLAVE, -num);
	return num;
}

void CityData::AddEndGameObject(sint32 type)
{
	bool wtf = false;
	Assert(wtf);
}

BOOL CityData::SendSlaveTo(Unit &dest)
{
	return FALSE;
}


void CityData::SetFullHappinessTurns(sint32 turns)
{
	m_happy->SetFullHappinessTurns(turns);
}

void CityData::AiStartMovingPops()
{
}

void CityData::AiDoneMovingPops()
{
}

sint32 CityData::GetHappinessFromPops()
{
	sint32 happy = 0;

	if(m_specialistDBIndex[POP_ENTERTAINER] >= 0) {
		happy += sint32(EntertainerCount() *
						g_thePopDB->Get(m_specialistDBIndex[POP_ENTERTAINER])->GetHappiness());
	}
   return happy;
}

sint32 CityData::GetScienceFromPops()
{
	sint32 sci = 0;

	double popWonderModifier = double(wonderutil_GetIncreaseScientists(m_builtWonders));
	popWonderModifier += double(wonderutil_GetIncreaseSpecialists(g_player[m_owner]->m_builtWonders));
	
	if(popWonderModifier && m_specialistDBIndex[POP_SCIENTIST] >= 0 && 
	   m_specialistDBIndex[POP_SCIENTIST] < g_thePopDB->NumRecords()) {
		sci += sint32(popWonderModifier * 
					  g_thePopDB->Get(m_specialistDBIndex[POP_SCIENTIST])->GetScience());
	}

	sci += sint32(buildingutil_GetIncreaseSciencePerPop(GetEffectiveBuildings()) * double(PopCount() - SlaveCount()));

	
	double p;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), p);
	sci += sint32(sci * p);
	sci = sint32(double(sci) * g_player[m_home_city.GetOwner()]->GetKnowledgeCoef());
	sci += sint32((double)sci * double((double)wonderutil_GetIncreaseKnowledgePercentage(g_player[m_home_city.GetOwner()]->GetBuiltWonders()) / 100.0));
	
	if(m_specialistDBIndex[POP_SCIENTIST] >= 0 &&
		m_specialistDBIndex[POP_SCIENTIST] < g_thePopDB->NumRecords()) {
		sci += sint32(ScientistCount() * 
					  g_thePopDB->Get(m_specialistDBIndex[POP_SCIENTIST])->GetScience());
	}

	return sci;
}

sint32 CityData::GetNumPop() const
{ 
    sint32 n = PopCount(); 

    if (n < 1) {
        return 1; 
    } else {
        return n; 
    } 
} 

sint32 CityData::GetGoldFromPops()
{
	sint32 gold = 0;
	return gold;
}

sint32 CityData::GetProductionFromPops()
{
	sint32 prod = 0;
	
	return prod;
}

sint32 CityData::GetIncomingTrade() const
{
	return m_tradeDestinationList.Num();
}

sint32 CityData::GetOutgoingTrade() const
{
	return m_tradeSourceList.Num();
}

void CityData::FinishBuilding()
{
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_FINISH_BUILDING,
										   m_home_city.m_id));
	}

	MapPoint pos;
	m_home_city.GetPos(pos);
	if(m_build_queue.BuildFront(m_shieldstore_at_begin_turn, 
								this, 
								pos, 
								m_built_improvements, 
								m_builtWonders, 
								TRUE)) {
		m_shieldstore_at_begin_turn = 0;
		m_shieldstore = 0;
		if(PopCount() < 1) {
			m_home_city.Kill(CAUSE_REMOVE_ARMY_UNKNOWN, -1);
		}
	}
}

void CityData::DestroyRandomBuilding()
{
#define k_MAX_BUILDINGS 64
#define k_MAX_WONDERS 64
	sint32 buildings[k_MAX_BUILDINGS + k_MAX_WONDERS];
	sint32 count = 0;
	sint32 i;
	for(i = 0; i < k_MAX_BUILDINGS; i++) {
		if(m_built_improvements & ((uint64)1 << (uint64)i)) {
			
			if(!buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)i))
				buildings[count++] = i;
		}
	}
	

	
	
	
	
	
	
	

	if(count > 0) {
		sint32 which = g_rand->Next(count);
		
		m_built_improvements &= ~((uint64)1 << uint64(buildings[which]));
		g_player[m_owner]->RegisterLostBuilding(m_home_city, buildings[which]);
		m_build_queue.RemoveIllegalItems(TRUE);
		
		
		
		
	}
    
	
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);
}

void CityData::AddConversionUnhappiness(sint32 who)
{
	if(m_convertedTo == who) {
		
		m_happy->RemoveTimerReason(HAPPY_REASON_ATTACKED_CONVERTER);

		m_happy->AddTimer(g_theConstDB->AttackConverterUnhappinessTurns(),
						  g_theConstDB->AttackConverterUnhappinessAmount(),
						  HAPPY_REASON_ATTACKED_CONVERTER);
	}
}

BOOL CityData::CanBuildInfrastructure() const
{
	return g_player[m_owner]->CanBuildInfrastructure();
}

void CityData::StopInfrastructureCapitalization()
{
	m_buildInfrastructure = FALSE;
	m_buildCapitalization = FALSE;
}

void CityData::InsertInfrastructure()
{
	Assert(CanBuildInfrastructure());
	if(!CanBuildInfrastructure())
		return;

	m_build_queue.InsertTail(k_GAME_OBJ_TYPE_INFRASTRUCTURE, 0, 0);	
}

void CityData::InsertCapitalization()
{
	Assert(CanBuildCapitalization());
	if(!CanBuildCapitalization())
		return;

	m_build_queue.InsertTail(k_GAME_OBJ_TYPE_CAPITALIZATION, 0, 0);
}

void CityData::BuildInfrastructure()
{
	Assert(CanBuildInfrastructure());
	if(!CanBuildInfrastructure())
		return;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_INFRASTRUCTURE, m_home_city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_INFRASTRUCTURE, m_home_city));
		g_network.Unblock(m_owner);
	}

	m_buildInfrastructure = TRUE;
	m_buildCapitalization = FALSE;
	
}

BOOL CityData::CanBuildCapitalization() const
{
	return g_player[m_owner]->CanBuildCapitalization();
}

void CityData::BuildCapitalization()
{
	Assert(CanBuildCapitalization());
	if(!CanBuildCapitalization())
		return;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_CAPITALIZATION, m_home_city));
	} else if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_CAPITALIZATION, m_home_city));
		g_network.Unblock(m_owner);
	}
	m_buildInfrastructure = FALSE;
	m_buildCapitalization = TRUE;
	
}

		
void CityData::EliminateNukes()
{
	if(buildingutil_IsNuclearPlant(m_built_improvements)) {
		sint32 i;
		for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
			if(buildingutil_IsNuclearPlant((uint64)1 << i)) {
				DestroyImprovement(i);
			}
		}
	}

	m_build_queue.RemoveIllegalItems();
}

bool CityData::BreakOneSourceRoute(ROUTE_TYPE type, sint32 resource)
{
	sint32 i;
	for(i = 0; i < m_tradeSourceList.Num(); i++) {
		ROUTE_TYPE t;
		sint32 r;
		m_tradeSourceList[i].GetSourceResource(t, r);
		if(t == type && r == resource) {
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute, 
								   GEA_TradeRoute, m_tradeSourceList[i],
								   GEA_Int, (sint32)CAUSE_KILL_TRADE_ROUTE_CHANGED_DESTINATION,
								   GEA_End);
			return true;
		}
	}
	return false;
}

void CityData::KillAllTradeRoutes()
{
	sint32 i;
	for(i = m_tradeSourceList.Num() - 1; i >= 0; i--) {
		m_tradeSourceList.Access(i).Kill(CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED);
	}

	for(i = m_tradeDestinationList.Num() - 1; i >= 0; i--) {
		m_tradeDestinationList.Access(i).Kill(CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED);
	}
}

void CityData::RemoveEndGameObjects()
{
}

void CityData::CheckForSlaveUprising()
{
	sint32 numSlaves = SlaveCount();
	if(numSlaves < 1)
		return;
	sint32 numMilitaryUnits = 0;
	MapPoint pos;
	m_home_city.GetPos(pos);
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 i;
	for(i = 0; i < cell->GetNumUnits(); i++) {
		if(cell->UnitArmy()->Access(i).GetAttack() > 0) {
			numMilitaryUnits++;
		}
	}
	if(double(numMilitaryUnits) >= 
	   double(double(numSlaves) / double(g_theConstDB->SlavesPerMilitaryUnit())))
		return;
	sint32 over = numSlaves - (numMilitaryUnits * g_theConstDB->SlavesPerMilitaryUnit());

	
	sint32 chance = over * g_theConstDB->UprisingChancePerUnguardedSlave();

	if(g_rand->Next(100) < chance) {
		
		m_doUprising = UPRISING_CAUSE_UNGUARDED_SLAVES;
	}
}

BOOL CityData::NeedToDoUprising() const
{ 
	return m_doUprising != UPRISING_CAUSE_NONE; 
}

void CityData::Disband()
{
	if(PopCount() > g_theConstDB->MaxDisbandSize()) {
		
		return;
	}
	
	
	if(g_player[m_owner]->m_all_cities->Num() < 2)
		return;

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_DISBAND_CITY, 
										   (uint32)m_home_city));
	}

	MapPoint pos;
	m_home_city.GetPos(pos);
	sint32 i;
	sint32 settler = -1;
	for(i = 0; i < g_theUnitDB->NumRecords(); i++) {
		if(g_theUnitDB->Get(i)->GetSettleLand() && 
		   (g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos)) &&
		   !g_theWorld->IsWater(pos) && !g_theWorld->IsShallowWater(pos)) {
			settler = i;
			break;
		} else if(g_theUnitDB->Get(i)->GetSettleWater() && g_theWorld->IsWater(pos)) {
			settler = i;
			break;
		} else if(g_theUnitDB->Get(i)->GetSettleSpace() && g_theWorld->IsSpace(pos)) {
			settler = i;
			break;
		}
	}
	Assert(settler >= 0);
	if(settler >= 0) {
		Unit s = g_player[m_owner]->CreateUnit(settler, pos,
											   m_home_city, FALSE,
											   CAUSE_NEW_ARMY_DISBANDED_CITY);
		if(g_theUnitPool->IsValid(s)) {
			s.ClearFlag(k_UDF_FIRST_MOVE);
			s.SetMovementPoints(0);
		}
	}

	ChangePopulation(-PopCount());
	m_home_city.Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1);
}

void CityData::AddSlaveBit(sint32 player)
{
	if(player >= 0 && player < k_MAX_PLAYERS) {
		m_slaveBits |= (1 << player);
	}
}

uint32 CityData::GetSlaveBits()
{
	if(m_numSpecialists[POP_SLAVE] <= 0)
		return 0;

	return m_slaveBits;
}

void CityData::AddGoods(SlicObject *obj)
{
	m_tempGoodAdder = obj;
	m_cityRadiusOp = RADIUS_OP_ADD_GOODS;
	MapPoint pos;
	m_home_city.GetPos(pos);
	CityRadiusIterator(pos, this);
}

sint32 CityData::GetGoodCountInRadius(sint32 good)
{
	m_cityRadiusOp = RADIUS_OP_COUNT_GOODS;
	m_tempGood = good;
	m_tempGoodCount = 0;
	MapPoint pos;
	m_home_city.GetPos(pos);
	CityRadiusIterator(pos, this);
	return m_tempGoodCount;
}

sint32 CityData::SubtractAccumulatedFood(sint32 amount)
{
	m_accumulated_food -= amount;
	if(m_accumulated_food < 0)
		m_accumulated_food = 0;
	return m_accumulated_food;
}

void CityData::SetProbeRecoveredHere(BOOL recovered)
{
	m_probeRecoveredHere = (uint8)recovered;
}





BOOL CityData::HasSleepingUnits(void)
{
	BOOL		hasSleepingOrFortified = FALSE;

	CellUnitList *units = g_theWorld->GetArmyPtr(m_home_city.RetPos());

	if (!units) return FALSE;

	for (sint32 i=0; i<units->Num(); i++) {
		if (units->Access(i).IsAsleep() || units->Access(i).IsEntrenched()) {
			hasSleepingOrFortified = TRUE;
			break;
		}
	}
	
	return hasSleepingOrFortified;
}

sint32 CityData::CityGrowthCoefficient()
{
	return sint32(g_theConstDB->CityGrowthCoefficient() * 
		          m_home_city.GetDBRec()->GetCityGrowthCoefficient());
}

void CityData::DestroyWonder(sint32 which)
{
	
	
	m_builtWonders &= ~((uint64)1 << (uint64)which);
	g_player[m_owner]->RemoveWonder(which, TRUE);

	
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_WonderRemoved,
		GEA_City, m_home_city.m_id,
		GEA_Int, which,
		GEA_End);
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);
}


void CityData::BuildFront()
{
	MapPoint pos = m_home_city.RetPos();

	m_shieldstore_at_begin_turn = m_shieldstore;
	if(m_build_queue.BuildFront(m_shieldstore, this, pos, m_built_improvements, m_builtWonders, FALSE)) {
		m_shieldstore_at_begin_turn = m_shieldstore;
	}
	if(!m_build_queue.GetHead()) {
		m_build_category_at_begin_turn = -3;
	} else {
		m_build_category_at_begin_turn = m_build_queue.GetHead()->m_category;
	}
	if(!g_theUnitPool->IsValid(m_home_city)) {
		
		
		
		
		return;
	}
	
}



sint32 CityData::GetValue() const
{
	sint32 production = 0;

	
	sint32 i;
	uint64 wonders = m_builtWonders&(((uint64)1<<(uint64)g_theWonderDB->NumRecords())-1);
	for(i=0;wonders!=0; i++,wonders>>=1)
	{
		
		if ((wonders&0xFF) == 0) {
			wonders>>=8;
			i+=8;
		}

		if ((wonders&1) && !wonderutil_IsObsolete(i))
		{
			production += wonderutil_GetProductionCost(i);		
		}
	}


	
	uint64 buildings = GetEffectiveBuildings()&(((uint64)1<<(uint64)g_theBuildingDB->NumRecords())-1);
	for(i=0;buildings!=0; i++,buildings>>=1)
	{
		
		if ((buildings&0xFF) == 0) {
			buildings>>=8;
			i+=8;
		}

		if (buildings&1)
		{
			production += buildingutil_GetProductionCost(i);
		}
	}

	
	production += m_shields_this_turn;

	
	
	production += m_trade;

	
	
	production += m_goldFromTradeRoutes;

	
	

	return production;
}

sint32 CityData::PopCount() const
{
	return m_population;
}

sint32 CityData::SlaveCount() const
{
	return m_numSpecialists[POP_SLAVE];
}

sint32 CityData::WorkerCount() const
{
	return m_numSpecialists[POP_WORKER];
}

sint32 CityData::ScientistCount() const
{
	return m_numSpecialists[POP_SCIENTIST];
}

sint32 CityData::EntertainerCount() const
{
	return m_numSpecialists[POP_ENTERTAINER];
}

sint32 CityData::FarmerCount() const
{
	return m_numSpecialists[POP_FARMER];
}

sint32 CityData::MerchantCount() const
{
	return m_numSpecialists[POP_MERCHANT];
}

sint32 CityData::LaborerCount() const
{
	return m_numSpecialists[POP_LABORER];
}

sint32 CityData::SpecialistCount(POP_TYPE type) const
{
	return m_numSpecialists[type];
}

void CityData::ChangeSpecialists(POP_TYPE type, sint32 delta)
{
	Assert(type != POP_WORKER); 
	if(type == POP_WORKER)
		return;

	if(delta > 0) {
		Assert(WorkerCount() >= delta);
		if(WorkerCount() < delta)
			return;
	} else if(delta < 0) {
		Assert(SpecialistCount(type) + delta >= 0);
		if(SpecialistCount(type) + delta < 0)
			return;
	}

	m_numSpecialists[POP_WORKER] -= (sint16)delta;
	m_numSpecialists[type] += (sint16)delta;

	if(type == POP_SLAVE) {
		if(m_numSpecialists[POP_SLAVE] <= 0)
			m_slaveBits = 0;
	}

	AdjustSizeIndices();

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(this);
		g_network.Unblock(m_owner);
	} else if(g_network.IsClient()) {

		if(this == m_home_city.CD()) {
			g_network.SendAction(new NetAction(NET_ACTION_SET_SPECIALISTS, m_home_city.m_id, type, m_numSpecialists[type]));
		}
	}
}


void CityData::ComputeSizeIndexes(const sint32 & workers, sint32 & size_index, sint32 & full_index, sint32 & partial_index) const
{
	full_index = -1;
	partial_index = 0;
	size_index = -1;

	sint32 sizeIndex;
	for(sizeIndex = 0; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++) {
		const CitySizeRecord *rec = g_theCitySizeDB->Get(sizeIndex);

		if(workers >= rec->GetMaxWorkers()) {
			full_index = sizeIndex;
			partial_index = sizeIndex + 1;
		}

		if(size_index < 0 && PopCount() <= rec->GetPopulation()) {
			size_index = sizeIndex;
		}
	}

	if(size_index < 0) {
		
		
		size_index = g_theCitySizeDB->NumRecords() - 1;
	}
}


sint32 CityData::GetBestSpecialist(const POP_TYPE & type) const
{
	Assert(type > 0);
	Assert(type < POP_SLAVE); 
	return m_specialistDBIndex[type];
}

void CityData::AdjustSizeIndices()
{
	double oldVision = -1;

	if(m_home_city->Flag(k_UDF_VISION_ADDED)) {
		Assert(m_sizeIndex >= 0);
		oldVision = GetVisionRadius();
	}

	sint32 oldSizeIndex = m_sizeIndex;
	ComputeSizeIndexes( WorkerCount(), 
					    m_sizeIndex,
					    m_workerFullUtilizationIndex,
					    m_workerPartialUtilizationIndex);

		
	
	if (m_sizeIndex == 0 || m_sizeIndex != oldSizeIndex)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CityInfluenceChanged,
								   GEA_City, m_home_city.m_id,
								   GEA_Int, (oldSizeIndex - m_sizeIndex),
								   GEA_End);

		if(oldVision != GetVisionRadius()) {
			if(oldVision >= 0) {
				m_home_city->RemoveOldUnitVision(oldVision);
			}
			BOOL revealed;
			m_home_city->AddUnitVision(revealed);
		}
		if(m_sizeIndex < oldSizeIndex) {
			
			
			CityInfluenceIterator it(m_home_city.RetPos(), oldSizeIndex);
			for(it.Start(); !it.End(); it.Next()) {
				if(it.Pos() != m_home_city.RetPos())
					g_theWorld->GetCell(it.Pos())->SetCityOwner(0);
			}
		}
		GenerateCityInfluence(m_home_city.RetPos(), m_sizeIndex);
	}

	m_collectingResources.Clear();
	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);
	for(it.Start(); !it.End(); it.Next()) {
		sint32 good;
		if(g_theWorld->GetGood(it.Pos(), good)) {
			m_collectingResources.AddResource(good);
		}
	}

	if(oldSizeIndex != m_sizeIndex) {
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(m_home_city.AccessData(), this);
			g_network.Unblock(m_owner);
		}
	}
}

void CityData::ChangePopulation(sint32 delta)
{
	m_population += delta;
	

	if(delta > 0) {
		
		m_numSpecialists[POP_WORKER] += (sint16)delta;
	} else {
		
		
		m_numSpecialists[POP_WORKER] += (sint16)delta;
		if(m_numSpecialists[POP_WORKER] < 0) {
			sint32 needToRemove = -m_numSpecialists[POP_WORKER];
			m_numSpecialists[POP_WORKER] = 0;

			sint32 i;
			for(i = 0; i < POP_MAX && needToRemove > 0; i++) {
				if(m_numSpecialists[i] >= (sint16)needToRemove) {
					m_numSpecialists[i] -= (sint16)needToRemove;
					needToRemove = 0;
				} else if(m_numSpecialists[i] >= 0) {
					needToRemove -= m_numSpecialists[i];
					m_numSpecialists[i] = 0;
				}
			}
		}
	}

	AdjustSizeIndices();
	UpdateSprite();

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(m_home_city.AccessData(), this);
		g_network.Unblock(m_owner);
	}

	if(m_population <= 0) {
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillCity,
							  GEA_City, m_home_city.m_id,
							  GEA_Int, CAUSE_REMOVE_CITY_UNKNOWN,
							  GEA_Player, -1,
							  GEA_End);
	}
}

void CityData::FindBestSpecialists()
{
	sint32 dbindex;
	sint32 maxValue[POP_MAX];

	sint32 popCategory;
	for (popCategory = (sint32)POP_WORKER + 1; 
		 popCategory < (sint32)POP_SLAVE; 
		 popCategory++) {
		
		maxValue[popCategory] = -1;
		m_specialistDBIndex[popCategory] = -1;
	}

	for(dbindex = 0; dbindex < g_thePopDB->NumRecords(); dbindex++) {
		const PopRecord *rec = g_thePopDB->Get(dbindex);

		if(!g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex()))
			continue;

		if( rec->GetScience() > 0 &&
			rec->GetScience() > maxValue[POP_SCIENTIST]) {
			maxValue[POP_SCIENTIST] = rec->GetScience();
			m_specialistDBIndex[POP_SCIENTIST] = dbindex;
		}

		if( rec->GetHappiness() > 0 &&
			rec->GetHappiness() > maxValue[POP_ENTERTAINER]) {
			maxValue[POP_ENTERTAINER] = rec->GetHappiness();
			m_specialistDBIndex[POP_ENTERTAINER] = dbindex;
		}

		if( rec->GetFood() > 0 &&
			rec->GetFood() > maxValue[POP_FARMER]) {
			maxValue[POP_FARMER] = rec->GetFood();
			m_specialistDBIndex[POP_FARMER] = dbindex;
		}

		if( rec->GetProduction() > 0 &&
			rec->GetProduction() > maxValue[POP_LABORER]) {
			maxValue[POP_LABORER] = rec->GetProduction();
			m_specialistDBIndex[POP_LABORER] = dbindex;
		}

		if( rec->GetCommerce() > 0 &&
			rec->GetCommerce() > maxValue[POP_MERCHANT]) {
			maxValue[POP_MERCHANT] = rec->GetCommerce();
			m_specialistDBIndex[POP_MERCHANT] = dbindex;
		}
	}
}


BOOL CityData::GetUseGovernor() const
{
 	return m_useGovernor || (g_player[m_owner]->GetPlayerType() == PLAYER_TYPE_ROBOT);
}
 
void CityData::SetUseGovernor(const bool &value)
{
 	m_useGovernor = value;
	if(!IsACopy()) {
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
			g_network.Unblock(m_owner);
		} else if(g_network.IsClient()) {
			if(g_network.IsLocalPlayer(m_owner))
				g_network.SendAction(new NetAction(NET_ACTION_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
		}
	}
}
 
sint32 CityData::GetBuildListSequenceIndex() const
{
 	return m_buildListSequenceIndex;
}
 
void CityData::SetBuildListSequenceIndex(const sint32 &value)
{
 	m_buildListSequenceIndex = value;
	if(!IsACopy()) {
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
			g_network.Unblock(m_owner);
		} else if(g_network.IsClient()) {
			if(g_network.IsLocalPlayer(m_owner))
				g_network.SendAction(new NetAction(NET_ACTION_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
		}
	}
}
 
sint32 CityData::GetGarrisonOtherCities() const
{
 	return m_garrisonOtherCities;
}
 
void CityData::SetGarrisonOtherCities(const sint32 &value)
{
 	m_garrisonOtherCities = value;
}
 
sint32 CityData::GetGarrisonComplete() const
{
	return m_garrisonComplete;
}
 
void CityData::SetGarrisonComplete(const sint32 &value)
{
 	m_garrisonComplete = value;
}


sint8 CityData::GetCurrentGarrison() const
{
	return m_currentGarrison;
}


void CityData::SetCurrentGarrison(const sint8 & value)
{
	m_currentGarrison = value;
}


sint8 CityData::GetNeededGarrison() const
{
	return m_neededGarrison;
}


void CityData::SetNeededGarrison(const sint8 & value)
{
	m_neededGarrison = value;
}


double CityData::GetCurrentGarrisonStrength() const
{
	return m_currentGarrisonStrength;
}


void CityData::SetCurrentGarrisonStrength(const double & value)
{
	m_currentGarrisonStrength = value;
}


double CityData::GetNeededGarrisonStrength() const
{
	return m_neededGarrisonStrength;
}


void CityData::SetNeededGarrisonStrength(const double & value)
{
	m_neededGarrisonStrength = value;
}

double CityData::GetVisionRadius() const
{
	Assert(m_sizeIndex >= 0);
	return g_theCitySizeDB->Get(m_sizeIndex)->GetVisionRadius();
}

void CityData::InsertBuildItem(sint32 index, uint32 category, sint32 type)
{
	if(index < 0) {
		switch(category) {
			case k_GAME_OBJ_TYPE_UNIT: BuildUnit(type); return;
			case k_GAME_OBJ_TYPE_IMPROVEMENT: BuildImprovement(type); return;
			case k_GAME_OBJ_TYPE_WONDER: BuildWonder(type); return;
			case k_GAME_OBJ_TYPE_CAPITALIZATION: InsertCapitalization(); return;
			case k_GAME_OBJ_TYPE_INFRASTRUCTURE: InsertInfrastructure(); return;
			default:
				Assert(FALSE);
				return;
		}
	}

	BuildNode *node = m_build_queue.GetNodeByIndex(index);
	if(!node) {
		InsertBuildItem(-1, category, type);
		return;
	}

	m_build_queue.InsertBefore(node, category, type);
}

void CityData::AddSellBuilding(sint32 building)
{
	m_sellBuilding = building;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::PayForBuyFront
//
// Description: Attempt to pay for rush production.
//
// Parameters : -
//
// Globals    : g_player	: list of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool CityData::PayForBuyFront()
{
#if defined(ACTIVISION_ORIGINAL)
	sint32 cost = GetOvertimeCost();

	if(cost < 0)
		return false;

	if(g_player[m_owner]->GetGold() < cost) {
		return false;
	}

	g_player[m_owner]->SubGold(cost);
    g_player[m_owner]->m_gold->AddLostToRushBuy(cost);
#else
	Player *		player	= g_player[m_owner];
	sint32 const	cost	= GetOvertimeCost();

	if ((cost <= 0) || (player->GetGold() < cost))
	{
		return false;
	}

	player->SubGold(cost);
	player->m_gold->AddLostToRushBuy(cost);
#endif

	m_paidForBuyFront = true;

	return true;
}

void CityData::AddBuyFront()
{
	if(PayForBuyFront()) {
		m_buyFront = true;
	}
}

void CityData::AddImprovement(sint32 type)
{
	SetImprovements(m_built_improvements | ((uint64)1 << (uint64)type));
	IndicateImprovementBuilt();
	g_player[m_owner]->RegisterCreateBuilding(m_home_city, type);

	if (buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)type)) {
		g_player[m_owner]->SetCapitol(m_home_city); 
		g_player[m_owner]->RegisterNewCapitolBuilding(m_home_city); 
	}

	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);

}

bool CityData::FindGoodDistancesCallback(const MapPoint &pos, Cell *cell, void *cookie)
{
	sint32 good;
	if(!g_theWorld->GetGood(pos, good))
		return false;

	CityData *cd = (CityData *)cookie;

	if(cd->m_distanceToGood[good] > 0)
		return false;

	cd->m_distanceToGood[good] = cell->GetScratch();
	return true;
}

void CityData::FindGoodDistances()
{
	sint32 i;
	sint32 goodsToFind = 0;
	for(i = 0; i < g_theResourceDB->NumRecords(); i++) {
		if(g_theWorld->GetGoodValue(i) <= g_theConstDB->GetMaxGoodValue()) {
			goodsToFind++;
		}
		m_distanceToGood[i] = 0;
	}

	g_theWorld->FindDistances(m_owner, m_home_city.RetPos(), goodsToFind,
							  FindGoodDistancesCallback, this);
}

sint32 CityData::GetDistanceToGood(sint32 good)
{
	Assert(good >= 0);
	Assert(good < g_theResourceDB->NumRecords());
	if(good < 0 || good >= g_theResourceDB->NumRecords())
		return 0;

	return m_distanceToGood[good];
}

void CityData::RemoveBorders()
{
	terrainutil_RemoveBorders(m_home_city.RetPos(), m_owner,
							   g_theConstDB->GetBorderIntRadius(),
							   g_theConstDB->GetBorderSquaredRadius(),
							   m_home_city);
}

void CityData::ResetStarvationTurns()
{
	m_starvation_turns = g_theConstDB->GetBaseStarvationProtection();
	m_starvation_turns += buildingutil_GetStarvationProtection(GetEffectiveBuildings());	
}

sint32 CityData::GetStarvationProtection()
{
	sint32 turns;

	turns = g_theConstDB->GetBaseStarvationProtection();
	turns += buildingutil_GetStarvationProtection(GetEffectiveBuildings());	

	return turns;
}

double CityData::GetOffenseBonus(const Unit &defender)
{
	const UnitRecord *defRec = defender.GetDBRec();
	double bonus = 0;
	if(defRec->GetMovementTypeLand() || defRec->GetMovementTypeMountain())
		bonus += buildingutil_GetOffenseBonusLand(GetEffectiveBuildings());
	
	if(defRec->GetMovementTypeSea() || defRec->GetMovementTypeShallowWater())
		bonus += buildingutil_GetOffenseBonusWater(GetEffectiveBuildings());

	if(defRec->GetMovementTypeAir())
		bonus += buildingutil_GetOffenseBonusAir(GetEffectiveBuildings());

	return bonus;
}

uint64 CityData::GetEffectiveBuildings() const
{
	return m_built_improvements | g_player[m_owner]->GetWonderBuildings();
}


sint32 CityData::GetDesiredSpriteIndex(bool justTryLand)
{
	sint32 i;

#if defined(ACTIVISION_ORIGINAL)
	bool isLand = justTryLand || g_theWorld->IsLand(m_pos);

	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(m_cityStyle);
#else
	// Added DWT
	// We want to retreive the underlying terrain type
	// not the terrain type as modified by improvements
	// as a sea city on a tunnel will turn into a land city
	const TerrainRecord *rec = g_theTerrainDB->Get(g_theWorld->GetTerrainType(m_pos));
	bool isLand = justTryLand || !(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater());

	// 
	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle());
#endif

	if(!styleRec) return -1;

	const AgeCityStyleRecord *ageStyleRec = styleRec->GetAgeStyle(g_player[m_owner]->m_age);
	if(!ageStyleRec) return -1;

	const AgeCityStyleRecord::SizeSprite *spr = NULL;
	const AgeCityStyleRecord::SizeSprite *lastTypeSpr = NULL;

	for(i = 0; i < ageStyleRec->GetNumSprites(); i++) {
		if(spr = ageStyleRec->GetSprites(i)) {
			if((isLand && spr->GetType() == 0) ||
			   (!isLand && spr->GetType() != 0)) {
				lastTypeSpr = spr;
				if(spr->GetMinSize() <= m_population &&
				   spr->GetMaxSize() >= m_population) {
					return spr->GetSprite();
				}
			}
		}
	}
	
	if(!justTryLand && !isLand) {
		
		return GetDesiredSpriteIndex(true);
	}

	if(lastTypeSpr) {
		
		return lastTypeSpr->GetSprite();
	}

	if(spr) {
		
		return spr->GetSprite();
	}
	return 0;
}

void CityData::DoSupport(bool projectedOnly)
{
	Assert(g_player[m_owner]);
	if (g_player[m_owner] == NULL)
		return;

	
	PayWages((sint32)g_player[m_owner]->GetWagesPerPerson(), projectedOnly);

	SupportBuildings(projectedOnly);
}

void CityData::SplitScience(bool projectedOnly)
{
	sint32 baseTrade = m_trade;
	if(baseTrade <= 0) {
		baseTrade = 0;
	}
	if (g_player[m_owner]==NULL)
		return;

	double s, ws;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(s);
	m_science = (sint32)(ceil(double(baseTrade) * s));
	m_trade -= m_science;

	
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), s);
	m_science += sint32(ceil(double(m_science) * s));

	ws = 0.01 * double(wonderutil_GetIncreaseKnowledgePercentage(g_player[m_owner]->GetBuiltWonders()));
	m_science += (sint32)ceil(double(m_science) * ws);

	m_science += GetScienceFromPops();
	
	m_science = m_science - sint32(ceil(double(m_science) * m_happy->GetCrime()));
	m_science = sint32(ceil(double(m_science) * g_player[m_owner]->GetKnowledgeCoef()));

	DPRINTF(k_DBG_GAMESTATE, ("SplitScience: %lx: %d, %lf, %lf, %d, %lf, %lf\n", m_home_city.m_id, m_science,
							  g_player[m_owner]->GetKnowledgeCoef(),
							  m_happy->GetCrime(),
							  GetScienceFromPops(),
							  ws, s));

	
}

void CityData::CollectOtherTrade(const BOOL projectedOnly, BOOL changeResources)
{
	
	if (m_trade > 0) {  
		
		double tradeBonus;
		buildingutil_GetCommercePercent(GetEffectiveBuildings(), tradeBonus, m_owner);
		m_trade += sint32(m_trade * tradeBonus);

		
		
		

		sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_COMMERCE, m_owner);
		m_trade += sint32(double(m_trade) * (double(featPercent) / 100.0));
	}

	
	sint32 goldPerCitizen = buildingutil_GetGoldPerCitizen(GetEffectiveBuildings());
	m_trade += goldPerCitizen * PopCount();

	
	if(m_specialistDBIndex[POP_MERCHANT] >= 0) {
		m_trade += MerchantCount() *
			g_thePopDB->Get(m_specialistDBIndex[POP_MERCHANT])->GetCommerce();
	}

	
	if(changeResources) {
		m_trade += CalculateTradeRoutes(projectedOnly);
	} else {
		m_goldFromTradeRoutes = 0;
		m_goldLostToPiracy = 0;
		m_trade += CalculateGoldFromResources();
	}

	
	if (m_trade > 0) { 
		
		m_trade = sint32(double(m_trade) * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetGoldCoef());
	}
	
	
	m_trade += m_gold_from_capitalization;

	
    m_convertedGold = 0;
	if(m_convertedTo >= 0) {
		if(m_convertedBy == CONVERTED_BY_CLERIC) {
			m_convertedGold = sint32(double(m_trade) * g_theConstDB->ClericConversionFactor());
		} else if(m_convertedBy == CONVERTED_BY_TELEVANGELIST) {
			sint32 doubleTelevangelists;
			buildingutil_GetDoubleTelevangelism(GetEffectiveBuildings(), doubleTelevangelists);
			if(doubleTelevangelists) {
				m_convertedGold = sint32(double(m_trade) * g_theConstDB->TelevangelistConversionFactor());
			} else {
				m_convertedGold = sint32(double(m_trade) * g_theConstDB->ClericConversionFactor());
			}
		} else {
#pragma warning (disable : 4127)									
			Assert(FALSE);
#pragma warning (default : 4127)									
		}
		if(m_convertedGold < 1)  
			m_convertedGold = 1;

		if (g_player[m_convertedTo]) {
			sint32 wonderIncrease = wonderutil_GetIncreaseConvertedCitiesFeePercentage(g_player[m_convertedTo]->m_builtWonders);
			if(wonderIncrease > 0) {
				m_convertedGold += (m_convertedGold * wonderIncrease) / 100;
			}
		}

		m_trade -= m_convertedGold;

		if(!projectedOnly) {
			DPRINTF(k_DBG_GAMESTATE, ("Sending %d gold to player %d due to conversion\n",
									  m_convertedGold, m_convertedTo));
			if (g_player[m_convertedTo]) {
				g_player[m_convertedTo]->AddGold(m_convertedGold);
			}
		}
	}


	
	m_trade_lost_to_crime = int(m_trade * m_happy->GetCrime());

	
	if (m_trade_lost_to_crime < 0)
		m_trade_lost_to_crime = 0;

    m_trade -= m_trade_lost_to_crime;


	
	if(!projectedOnly) {
		g_player[m_owner]->m_gold->AddIncome(m_trade);
	}	
}

sint32 CityData::GetProjectedScience()
{
	
	sint32 grossFood = m_gross_food_this_turn;
	sint32 collectedProduction = m_collected_production_this_turn;
	sint32 grossTrade = m_gross_trade;
	sint32 shieldsThisTurn = m_shields_this_turn;
	sint32 foodThisTurn = m_food_produced_this_turn;
	sint32 trade = m_trade;
	sint32 science = m_science;
	sint32 wagesPaid = m_wages_paid;

	CollectResources();
	DoSupport(true);
	SplitScience(true);

	sint32 scienceReturn = m_science;

	
	m_gross_food_this_turn = grossFood;
	m_collected_production_this_turn = collectedProduction;
	m_gross_trade = grossTrade;
	m_shields_this_turn = shieldsThisTurn;
	m_food_produced_this_turn = foodThisTurn;
	m_trade = trade;
	m_science = science;
	m_wages_paid = wagesPaid;
	
	return scienceReturn;
}

sint32 CityData::GetFounder() const
{
	return m_founder;
}

#if !defined(ACTIVISION_ORIGINAL)
//----------------------------------------------------------------------------
//
// Name       : CityData::GetCityStyle
//
// Description: Get the style of the city.
//
// Parameters : -
//
// Globals    : g_player	: list of players
//
// Returns    : -
//
// Remark(s)  : Order of checking:
//              1. The value of m_cityStyle for the individual city.
//              2. The style of the founder.
//              3. The style of the owner.
//
//----------------------------------------------------------------------------

sint32 CityData::GetCityStyle() const
{
	if ((m_cityStyle >= 0) && (m_cityStyle < g_theCityStyleDB->NumRecords()))
	{
		return m_cityStyle;
	}
	else if (g_player[m_founder] && g_player[m_founder]->GetCivilisation()) 
	{
		return g_player[m_founder]->GetCivilisation()->GetCityStyle();
	}
	else if (g_player[m_owner] && g_player[m_owner]->GetCivilisation())
	{
		return g_player[m_owner]->GetCivilisation()->GetCityStyle();
	}
	else
	{
		return CITY_STYLE_DEFAULT;
	}
}
#endif

void CityData::SetCityStyle(sint32 style)
{

	m_cityStyle = style;
#if defined(ACTIVISION_ORIGINAL) // Invalid values are handled by GetCityStyle.
	Assert(m_cityStyle >= 0);
	Assert(m_cityStyle < g_theCityStyleDB->NumRecords());
	if(m_cityStyle < 0 || m_cityStyle >= g_theCityStyleDB->NumRecords()) {
		m_cityStyle = 0;
	}
#endif
	UpdateSprite();
}
