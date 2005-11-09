//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City data
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
// - Generate debug version when set.
//
// _SLOW_BUT_SAFE
// - Define 2 other symbols (PROJECTED_CHECK_START and PROJECTED_CHECK_END) 
//   when set. But the defined symbols are never used, so this doesn't do
//   anything at all. This makes preprocessing and compilation slower, but
//   should be safe.
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
// - #01 Fixed sometimes not correct filled m_net_production, that
//   leads to wrong sorting sequence for number of turns until production
//   is finished in NationalManager status window (maybe also to
//   other wrong behaviours). (L. Hirth 7/2004) 
// - Prevented cities from revolting twice in the same turn. By kaan.
// - Standardised min/max usage.
// - Addition of new NeedMoreFood function to figure out wheather a city 
//   needs more food, so that the AI can build new food tile improvements, 
//   by Martin Gühmann.
// - Possible solution for bug #14 by Klaus Kaan
// - Make city growth/starvation work for PBEM.
// - Modified the bug #14 solution: use a new - debugger confirmed - message.
// - Recompute the defense bonus when selling a building.
// - Splitted and cleaned various functions to have better access to resource
//   estimation without the need to recalculate everything when adjusting 
//   population assignment for instnce. - April 4th 2005 Martin Gühmann
// - Updated NeedMoreFood function for better estimation. 
//   - April 4th 2005 Martin Gühmann
// - Changed CollectResources to add in the food, production, and gold from the resource. 
// - Moved Peter's last modification to Cell.cpp and UnseenCell.cpp, idially 
//   such code should only be put at one place. - April 12th 2005 Martin Gühmann 
// - Track city growth with updated TurnsToNextPop method - PFT 29 mar 05
// - Improved running time of TurnsToNextPop method and removed superflous call
//   of TurnsToNextPop methods, as private member m_turnsNextPop was never 
//   accessed by a get method or was used in any other method.
//   - April 23rd 2005 Martin Gühmann
// - CityStyleOnly code added to CanBuildUnit, CanBuildBuilding, CanBuildWonder; 
//   Checks if the style of city matches the citystyle flag for the unit, 
//   building, or wonder - by E April 20th 2005
// - CultureOnly added to CanBuildBuilding and CanBuildWonder; Checks if the 
//   player's citystyle matches the citystyle flag for the building, or wonder 
//   - by E April 20th 2005
// - Governmenttype added to CanBuildWonder and CanBuildBuilding; Checks if
//   the player's government matches the flag for the wonder or building - by E 
//   April 20th 2005 
// - PrerequisiteBuilding added to CanBuildUnit and CanBuildWonder; Checks if a 
//   city has a building required to build the unit or wonder. - by E 
//   April 30th 2005
// - Moved m_distanceToGood reinitialization to serialize method, to handle
//   modified Ressource database on reload. - May 19th 2005 Martin Gühmann
// - Fixed reload of savegames with different number of goods in their 
//   database than in the games database. - June 5th 2005 Martin Gühmann
// - Fixed the large memory leaks - when opening the city window - that were
//   introduced by Martin's fix.
// - Prevent crashes when killing a city twice.
// - Redesigned Copy method so that it doesn't abuse the serialize method
//   anymore. Replaces Fromafar's large memory fix, for some reason it made
//   crash the game anyway. - Jul. 5th 2005 Martin Gühmann
// - If capitalisation or infrastructure first item in the build queue, an
//   item can now rush bought that is inserted before. - Jul. 23rd 2005 Martin Gühmann
// - Replaced some member names for clarity. - Aug 6th 2005 Martin Gühmann
// - Removed a bunch of unused and incomplete methods. - Aug 6th 2005 Martin Gühmann
// - Fixed Gold isn't added twice to income. - Aug 6th 2005 Martin Gühmann
// - Added new code as preparation for resource calculation redesign.- Aug 6th 2005 Martin Gühmann
// - Added code for new city resource calculation. (Aug 12th 2005 Martin Gühmann)
// - Removed more unused methods. (Aug 12th 2005 Martin Gühmann)
// - NeedsCityGood added to CanBuildUnit, CanBuildBuilding, and CanBuild Wonder
//   requiring a good in the radius or if the city is buying it before a it can be 
//   built. (Sept 29nd 2005 by E)
// - Added city style specific happiness bonus method. (Oct 7th 2005 Martin Gühmann)
// - Implemented EnablesGood for buildings, wonders and tile improvements now they 
//   give goods to a city by E November 5th 2005
// - To ProcessFood,ProcessProduction,ProcessGold, ProcessScience added a check that 
//   if a city has or is buying a good than you can get a bonus. EfficiencyOrCrime 
//   flag affects all four.  - added by E November 5th 2005
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

#include "cellunitlist.h"
#include "UnitRecord.h"
#include "UnitPool.h"
#include "citydata.h"
#include "XY_Coordinates.h"
#include "World.h"

#include "player.h"
#include "RandGen.h"

#include "network.h"
#include "net_info.h"
#include "net_action.h"

#include "civarchive.h"
#include "Checksum.h"
#include "SelItem.h"
#include "WonderRecord.h"
#include "pollution.h"
#include "TradeRoute.h"
#include "Gold.h"
#include "Cell.h"
#include "installationtree.h"
#include "CivilisationPool.h"
#include "AICause.h"
#include "director.h"
#include "MessagePool.h"
#include "Advances.h"
#include "SlicSegment.h"
#include "TerrainRecord.h"
#include "TopTen.h"
#include "Happy.h"
#include "HappyTracker.h"
#include "TurnCnt.h"


#include "profileDB.h"
#include "TaxRate.h"

#include "ArmyPool.h"
#include "ArmyData.h"
#include "Score.h"
#include "Exclusions.h"
#include "WonderTracker.h"
#include "MaterialPool.h"

#include "colorset.h"
#include "tiledmap.h"

#include "TradeOffer.h"

#include "Readiness.h"


#include "DiffDB.h"
extern DifficultyDB *g_theDifficultyDB;

#ifdef _DEBUG
#include "aicause.h"
#endif

#include "UnitActor.h"

#include "soundmanager.h"
extern SoundManager *g_soundManager;

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

#include "gamefile.h"

//Added by Martin Gühmann to handle 
//city creation by Scenario Editor properly
#include "scenarioeditor.h"
#include "StrategyRecord.h"         // For accessing the strategy database
#include "Diplomat.h"               // To be able to retrieve the current strategy
#include <algorithm>                // std::min

extern void player_ActivateSpaceButton(sint32 pl);


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

extern TopTen            *g_theTopTen;
extern World             *g_theWorld; 
extern Player            **g_player; 
extern UnitPool          *g_theUnitPool; 
extern StringDB          *g_theStringDB;
extern RandomGenerator   *g_rand; 
extern ConstDB           *g_theConstDB;
extern SelectedItem      *g_selected_item;
extern Director          *g_director;

extern Pollution         *g_thePollution;

extern TurnCount         *g_turn;
extern ProfileDB	     *g_theProfileDB;

//----------------------------------------------------------------------------
//
// Name       : CityData::CityData
//
// Description: Constructor
//
// Parameters : PLAYER_INDEX o             : the city's owner
//              Unit         hc            : cities are units
//              MapPoint     &center_point : the city's map location
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
CityData::CityData(PLAYER_INDEX owner, Unit hc, const MapPoint &center_point)
:
    m_owner                             (owner),
	m_slaveBits                         (0),
	m_accumulated_food                  (0),
	m_shieldstore                       (0),
	m_shieldstore_at_begin_turn         (0),
	m_build_category_at_begin_turn      (-4),
	m_net_gold                          (0),
	m_gold_lost_to_crime                (0),
	m_gross_gold                        (0),
	m_goldFromTradeRoutes               (0),
	m_goldLostToPiracy                  (0),
	m_science                           (0),
	m_luxury                            (0),
	m_city_attitude                     (CITY_ATTITUDE_CONTENT),
	m_collected_production_this_turn    (0),
	m_gross_production                  (0),
	m_net_production                    (0),
	m_production_lost_to_crime          (0),
	m_built_improvements                (0),
	m_builtWonders                      (0),
	m_food_delta                        (0.0),
	m_gross_food                        (0.0),
	m_net_food                          (0.0),
	m_food_lost_to_crime                (0.0),
	m_food_consumed_this_turn           (0.0),
	m_total_pollution                   (0),
	m_cityPopulationPollution           (0),
	m_cityIndustrialPollution           (0),
	m_foodVatPollution                  (0),
	m_cityPollutionCleaner              (0),
	m_contribute_materials              (true),
	m_contribute_military               (true),
	m_capturedThisTurn                  (false),
	m_spied_upon                        (0),
	m_walls_nullified                   (false),
	m_franchise_owner                   (PLAYER_UNASSIGNED),
	m_franchiseTurnsRemaining           (-1),
	m_watchfulTurns                     (-1),
	m_bioInfectionTurns                 (-1),
	m_bioInfectedBy                     (PLAYER_UNASSIGNED),
	m_nanoInfectionTurns                (-1),
	m_nanoInfectedBy                    (PLAYER_UNASSIGNED),
	m_convertedTo                       (PLAYER_UNASSIGNED),
	m_convertedGold                     (0),
	m_convertedBy                       (CONVERTED_BY_NOTHING),
	m_terrainWasPolluted                (false),
	m_happinessAttacked                 (false),
	m_terrainImprovementWasBuilt        (false),
	m_improvementWasBuilt               (false),
	m_isInjoined                        (false),
	m_injoinedBy                        (PLAYER_UNASSIGNED),
	m_airportLastUsed                   (-1),
	m_founder                           (owner),
	m_wages_paid                        (0),
	m_pw_from_infrastructure            (0),
	m_gold_from_capitalization          (0),
	m_buildInfrastructure               (false),
	m_buildCapitalization               (false),
	m_paidForBuyFront                   (false),
	m_doUprising                        (UPRISING_CAUSE_NONE),
	m_turnFounded                       (g_turn ? g_turn->GetRound() : 0),
	m_productionLostToFranchise         (0),
	m_probeRecoveredHere                (false),
	m_lastCelebrationMsg                (-1),
	m_alreadySoldABuilding              (false),
	m_population                        (0),
	m_partialPopulation                 (0),
//	sint16 m_numSpecialists[POP_MAX];
//	sint32 m_specialistDBIndex[POP_MAX];
	m_sizeIndex                         (0),
	m_workerFullUtilizationIndex        (0),
	m_workerPartialUtilizationIndex     (0),
	m_useGovernor                       (false),
	m_buildListSequenceIndex            (0),
	m_garrisonOtherCities               (false),
	m_garrisonComplete                  (false),
	m_currentGarrison                   (0),
	m_neededGarrison                    (0),
	m_currentGarrisonStrength           (0.0),
	m_neededGarrisonStrength            (0.0),
	m_sellBuilding                      (-1),
	m_buyFront                          (false),
	m_max_food_from_terrain             (0),
	m_max_prod_from_terrain             (0),
	m_max_gold_from_terrain             (0),
	m_growth_rate                       (0),
	m_overcrowdingCoeff                 (0.0),
	m_starvation_turns                  (0),
	m_cityStyle                         (CITY_STYLE_GENERIC),
	m_pos                               (center_point),
	m_is_rioting                        (false),
	m_home_city                         (hc),
	m_min_turns_revolt                  (0),
	m_build_queue                       (),
	m_tradeSourceList                   (),
	m_tradeDestinationList              (),
#ifdef CTP1_TRADE
	m_resources                         (),
	m_localResources                    (),
#else
	m_collectingResources               (),
	m_sellingResources                  (),
	m_buyingResources                   (),
#endif
	m_happy                             (new Happy()),
//	MBCHAR    m_name[k_MAX_NAME_LEN];
//	sint32    *m_distanceToGood;
	m_defensiveBonus                    (0.0),
//	sint32    *m_ringFood;
//	sint32    *m_ringProd;
//	sint32    *m_ringGold;
//	sint32    *m_ringSizes;
#if defined(NEW_RESOURCE_PROCESS)
//	double    *m_farmersEff;
//	double    *m_laborersEff;
//	double    *m_merchantsEff;
//	double    *m_scientistsEff;
	m_max_processed_terrain_food        (0.0),
	m_max_processed_terrain_prod        (0.0),
	m_max_processed_terrain_gold        (0.0),
	m_max_processed_terrain_scie        (0.0),
	m_grossFoodCrimeLoss                (0.0),
	m_grossProdCrimeLoss                (0.0),
	m_grossGoldCrimeLoss                (0.0),
	m_grossScieCrimeLoss                (0.0),
	m_grossProdBioinfectionLoss         (0.0),
	m_grossProdFranchiseLoss            (0.0),
	m_grossGoldConversionLoss           (0.0),
//	double    m_foodFromOnePop;
//	double    m_prodFromOnePop;
//	double    m_goldFromOnePop;
//	double    m_scieFromOnePop;
//	double    m_crimeFoodLossOfOnePop;
//	double    m_crimeProdLossOfOnePop;
//	double    m_crimeGoldLossOfOnePop;
//	double    m_crimeScieLossOfOnePop;
//	double    m_bioinfectionProdLossOfOnePop;
//	double    m_franchiseProdLossOfOnePop;
//	double    m_conversionGoldLossOfOnePop;
	m_productionLostToBioinfection      (0),
	m_max_scie_from_terrain             (0),
	m_gross_science                     (0.0),
	m_science_lost_to_crime             (0.0),
#endif	
	m_cityRadiusOp                      (RADIUS_OP_UKNOWN),
	m_killList                          (NULL),
	m_radiusNewOwner                    (0),
    m_tilecount                         (0), 
//            m_whichtile;
    m_tempGoodAdder                     (NULL),
    m_tempGood                          (-1), 
    m_tempGoodCount                     (0),
	m_sentInefficientMessageAlready     (false)
#ifdef _DEBUG
  , m_ignore_happiness                  (false) 
#endif
{
	m_name[0] = 0;
	m_build_queue.SetOwner(m_owner);
	m_build_queue.SetCity(m_home_city);
	g_theWorld->SetCapitolDistanceDirtyFlags(1 << owner);

	// Set the style of the founder of the city - if any.
	if (g_player[owner] && g_player[owner]->GetCivilisation())
	{
		m_cityStyle = g_player[owner]->GetCivilisation()->GetCityStyle();
	}

	for (size_t i = 0; i < POP_MAX; ++i) 
    {
		m_numSpecialists[i]     = 0;
        m_specialistDBIndex[i]  = -1;
	}

	ResetStarvationTurns();

	m_distanceToGood    = new sint32[g_theResourceDB->NumRecords()];
	m_ringFood          = new sint32[g_theCitySizeDB->NumRecords()];
	m_ringProd          = new sint32[g_theCitySizeDB->NumRecords()];
	m_ringGold          = new sint32[g_theCitySizeDB->NumRecords()];
	m_ringSizes         = new sint32[g_theCitySizeDB->NumRecords()];

#if defined(NEW_RESOURCE_PROCESS)
	m_farmersEff        = new double[g_theCitySizeDB->NumRecords()];
	m_laborersEff       = new double[g_theCitySizeDB->NumRecords()];
	m_merchantsEff      = new double[g_theCitySizeDB->NumRecords()];
	m_scientistsEff     = new double[g_theCitySizeDB->NumRecords()];
#endif
}

CityData::~CityData()
{
	if (!m_home_city.IsValid() || m_home_city.CD() == this)
	{
		//DPRINTF(k_DBG_GAMESTATE, ("Killing City %lx\n", uint32(m_home_city)));
		sint32 i;

		for(i = 0; i < m_tradeSourceList.Num(); i++) {
			m_tradeSourceList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_DIED);
		}
		
		for(i = 0; i < m_tradeDestinationList.Num(); i++) {
			m_tradeDestinationList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_DIED);
		}
	}

	delete m_happy;
	delete m_distanceToGood;
	delete m_ringFood;
	delete m_ringProd;
	delete m_ringGold;
	delete m_ringSizes;
	
#if defined(NEW_RESOURCE_PROCESS)
	delete m_farmersEff;
	delete m_laborersEff;
	delete m_merchantsEff;
	delete m_scientistsEff;
#endif
}

// Global to fix trade routes
sint32 *g_newGoods = NULL;

//----------------------------------------------------------------------------
//
// Name       : CityData::Serialize
//
// Description: Store/Load CityData
//
// Parameters : CivArchive &archive       :
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	if (archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_owner, ((uint8 *)&m_is_rioting)+sizeof(m_is_rioting));

	} else
		archive.LoadChunk((uint8 *)&m_owner, ((uint8 *)&m_is_rioting)+sizeof(m_is_rioting));

	// Create and read files in the format as created with the Activision 1.1 patch.
	// Used to prevent cities revolting twice in the same turn.
	if (archive.IsStoring())
	{
		archive << m_min_turns_revolt;
	}
	else
	{
		if (g_saveFileVersion >= 66) 
		{
			archive >> m_min_turns_revolt;
		}
		else
		{
			m_min_turns_revolt = 0;
		}
	}
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

		sint32 const    ressourceNum    = m_collectingResources.GetNum();

		// TODO: Clean up this mess later, and test what would happen when the good 
		// database size would decrease, instead of increase. Also consider the 
		// global variable that is never reinitialised. What happens when loading a 
		// game (mod/scenario) with a different number of goods? It is deleted on the 
		// end of the load so no problem for the next save game.

		if(ressourceNum == g_theResourceDB->NumRecords()){
			m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];
			archive.Load((uint8*)m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
		}
		else{ // Fix trade if the good database was increased in size.
			sint32 *tmpDistanceToGood = new sint32[ressourceNum];
			archive.Load((uint8*)tmpDistanceToGood, sizeof(sint32) * ressourceNum);

			m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];

			// To check later: next 3 lines are invalid when CTP1_TRADE has been defined. See CityData.h.
			m_collectingResources.Resize(g_theResourceDB->NumRecords());
			m_buyingResources.Resize(g_theResourceDB->NumRecords());
			m_sellingResources.Resize(g_theResourceDB->NumRecords());

			FindGoodDistances(); // Recalc good distances

			Resources copyCollectingResources(m_collectingResources);
			Resources copyBuyingResources(m_buyingResources);
			Resources copySellingResources(m_sellingResources);
			sint32 i, j;
			for(j = 0; j < ressourceNum; ++j){
				for(i = 0; i < g_theResourceDB->NumRecords(); ++i){
					if(tmpDistanceToGood[j] > 0 && tmpDistanceToGood[j] == m_distanceToGood[i]){
						m_collectingResources.AddResource(i, +copyCollectingResources[j]);
						m_collectingResources.AddResource(j, -copyCollectingResources[j]);
						m_buyingResources.AddResource(i, +copyBuyingResources[j]);
						m_buyingResources.AddResource(j, -copyBuyingResources[j]);
						m_sellingResources.AddResource(i, +copySellingResources[j]);
						m_sellingResources.AddResource(j, -copySellingResources[j]);
						break;
					}
				}
			}

			if(g_newGoods == NULL){
				// Needs only be done once per reload
				g_newGoods = new sint32[ressourceNum]; // Deleted in gameinit_Initialize
				memset(g_newGoods, 0, ressourceNum * sizeof(sint32));
				for(j = 0; j < ressourceNum; ++j){
					for(i = 0; i < g_theResourceDB->NumRecords(); ++i){
						if(tmpDistanceToGood[j] > 0 && tmpDistanceToGood[j] == m_distanceToGood[i]){
							g_newGoods[j] = i;
							break;
						}
					}
				}
			}

			delete[] tmpDistanceToGood;
		}

		archive.Load((uint8*)&m_defensiveBonus, sizeof(double));

		m_ringFood  = new sint32[g_theCitySizeDB->NumRecords()];
		m_ringProd  = new sint32[g_theCitySizeDB->NumRecords()];
		m_ringGold  = new sint32[g_theCitySizeDB->NumRecords()];
		m_ringSizes = new sint32[g_theCitySizeDB->NumRecords()];

#if defined(NEW_RESOURCE_PROCESS)
		m_farmersEff    = new double[g_theCitySizeDB->NumRecords()];
		m_laborersEff   = new double[g_theCitySizeDB->NumRecords()];
		m_merchantsEff  = new double[g_theCitySizeDB->NumRecords()];
		m_scientistsEff = new double[g_theCitySizeDB->NumRecords()];

		memset(&m_max_processed_terrain_food, 0, (uint32)&m_science_lost_to_crime + sizeof(m_science_lost_to_crime) - (uint32)&m_max_processed_terrain_food);
#endif
	}

}

//----------------------------------------------------------------------------
//
// Name       : CityData::NeedsCanalTunnel
//
// Description: Test if a city built at MapPoint &center_point would need a CanalTunnel
//
// Parameters : MapPoint &center_point     : The center of the city's radius
//
// Globals    : g_theWorld
//
// Returns    : TRUE if center_point is in water terrain and doesn't have a tunnel
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
BOOL NeedsCanalTunnel(MapPoint &center_point)
{
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

//----------------------------------------------------------------------------
//
// Name       : CityData::Initialize
//
// Description: Create a new city
//
// Parameters : sint32 settlerType    : The type of settler that's founding the city
//
// Globals    : g_network
//            : g_theUnitDB
//            : g_gevManager
//            : g_theBuildingDB
//            : g_theInstallationTree
//            : g_theWorld
//            : g_tiledMap
//            : g_theCivilisationPool
//            : g_theAdvanceDB
//            : g_theConstDB
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::Initialize(sint32 settlerType)
{
	MapPoint center_point(m_home_city.RetPos());

	if (g_network.IsClient() && g_network.IsLocalPlayer(m_owner)) {
		g_network.AddCreatedObject(m_home_city.AccessData());
	}

	FindBestSpecialists();

	sint32 martialLaw;
	m_happy->CalcHappiness(*this, FALSE, martialLaw, TRUE);

	const UnitRecord * settlerRec   = NULL;
	sint32 numPops = 1;
	if(settlerType >= 0) {
		settlerRec = g_theUnitDB->Get(settlerType, g_player[m_owner]->GetGovernmentType());
		if(settlerRec)
			numPops = settlerRec->GetSettleSize();
		
	} else {
		//Added by Martin Gühmann to make sure that also cities
		//created by the Scenario editor have a size
		if(settlerType == -2 && ScenarioEditor::PlaceCityMode() && ScenarioEditor::CitySize() > 0)
			numPops = ScenarioEditor::CitySize();
	}

	sint32 i;
	//Added by Martin Gühmann to make sure that also cities created by the editor
	//have a size.
	if((settlerType != -2) || ScenarioEditor::PlaceCityMode()){
		for(i = 0; i < numPops; i++){
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
			                       GEA_City, m_home_city,
			                       GEA_End);
		}
	}
	
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
		DynamicArray<Installation> instArray;
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
	Civilisation *civ = g_player[m_owner]->GetCivilisation();
	CivilisationData *civData = g_theCivilisationPool->AccessData(*civ);
	if (g_player[m_owner]->GetNumCities() == 0){
		name = civData->GetCapitalName();


	}
	else
		name = civData->GetAnyCityName();

	//Added by Martin Gühmann to make sure that cities created 
	//by the scenario editor keep their style
	if ((settlerType == CITY_STYLE_EDITOR) && ScenarioEditor::PlaceCityMode())
	{
		m_cityStyle = ScenarioEditor::CityStyle();
	}

	if (name != k_CITY_NAME_UNDEFINED){
		civData->GetCityName(name, s);
		civData->UseCityName(name);
		SetName(s);
	} else {
		
		
	
		SetName(GetName());
	}

	if(g_network.IsActive() && g_network.GetStartingAge() > 0) {
		sint32 i;
		for(i = 0; i < g_theBuildingDB->NumRecords(); i++) {
			if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)i)))
				continue;

			if(!CanBuildBuilding(i))
				continue;

			sint32 enable = g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetEnableAdvanceIndex();
			if(g_theAdvanceDB->Get(enable, g_player[m_owner]->GetGovernmentType())->GetAgeIndex() < g_network.GetStartingAge()) {
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

// called only by NetUnit::Unpacketize
void CityData::NetworkInitialize()
{
	FindBestSpecialists();
}












CityData::CityData(CivArchive &archive)
{
	m_happy = new Happy;

	m_sentInefficientMessageAlready = FALSE;
	Serialize(archive) ;
}

CityData::CityData(CityData *copy)
{
	m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];
	m_happy = new Happy;

	m_ringFood  = new sint32[g_theCitySizeDB->NumRecords()];
	m_ringProd  = new sint32[g_theCitySizeDB->NumRecords()];
	m_ringGold  = new sint32[g_theCitySizeDB->NumRecords()];
	m_ringSizes = new sint32[g_theCitySizeDB->NumRecords()];

#if defined(NEW_RESOURCE_PROCESS)
	m_farmersEff    = new double[g_theCitySizeDB->NumRecords()];
	m_laborersEff   = new double[g_theCitySizeDB->NumRecords()];
	m_merchantsEff  = new double[g_theCitySizeDB->NumRecords()];
	m_scientistsEff = new double[g_theCitySizeDB->NumRecords()];
#endif

	Copy(copy);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::Copy
//
// Description: Copies the content from a CityData into this CityData. 
//
// Parameters : CityData *copy    : The instance from that data should be
//                                  copied into this instance.
//
// Globals    : g_network
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::Copy(CityData *copy)
{

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

	Assert((uint32)&copy->m_is_rioting + sizeof(copy->m_is_rioting) - (uint32)&copy->m_owner == (uint32)&m_is_rioting + sizeof(m_is_rioting) - (uint32)&m_owner);
	memcpy(&m_owner, &copy->m_owner, (uint32)&copy->m_is_rioting + sizeof(copy->m_is_rioting) - (uint32)&copy->m_owner);

	m_min_turns_revolt = copy->m_min_turns_revolt;

	m_home_city = copy->m_home_city;

	m_build_queue = copy->m_build_queue;
	m_tradeSourceList = copy->m_tradeSourceList;
	m_tradeDestinationList = copy->m_tradeDestinationList;

	// To check later: next 3 lines are invalid when CTP1_TRADE has been defined. See CityData.h.
	m_collectingResources = copy->m_collectingResources;
	m_buyingResources = copy->m_buyingResources;
	m_sellingResources = copy->m_sellingResources;

	m_happy->Copy(copy->m_happy);

	memcpy(m_name, copy->m_name, (strlen(copy->m_name) + 1) * sizeof(MBCHAR));
	memcpy(m_distanceToGood, copy->m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
	m_defensiveBonus = copy->m_defensiveBonus;

	memcpy(m_ringFood,  copy->m_ringFood,  sizeof(sint32) * g_theCitySizeDB->NumRecords());
	memcpy(m_ringProd,  copy->m_ringProd,  sizeof(sint32) * g_theCitySizeDB->NumRecords());
	memcpy(m_ringGold,  copy->m_ringGold,  sizeof(sint32) * g_theCitySizeDB->NumRecords());
	memcpy(m_ringSizes, copy->m_ringSizes, sizeof(sint32) * g_theCitySizeDB->NumRecords());
#if defined(NEW_RESOURCE_PROCESS)
	memcpy(m_farmersEff,    copy->m_farmersEff,    sizeof(double) * g_theCitySizeDB->NumRecords());
	memcpy(m_laborersEff,   copy->m_laborersEff,   sizeof(double) * g_theCitySizeDB->NumRecords());
	memcpy(m_merchantsEff,  copy->m_merchantsEff,  sizeof(double) * g_theCitySizeDB->NumRecords());
	memcpy(m_scientistsEff, copy->m_scientistsEff, sizeof(double) * g_theCitySizeDB->NumRecords());

	memcpy(&m_max_processed_terrain_food, &copy->m_max_processed_terrain_food, (uint32)&copy->m_science_lost_to_crime + sizeof(copy->m_science_lost_to_crime) - (uint32)&copy->m_max_processed_terrain_food);
#endif

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

//----------------------------------------------------------------------------
//
// Name       : CityData::SetShieldstore
//
// Description: Set the city's stored shields
//
// Parameters : sint32 s    : The ammount of shields to set
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::SetShieldstore (sint32 s)
{
	m_shieldstore = s;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::AddShields
//
// Description: Add shields to the city's store of them
//
// Parameters : sint32 s    : The number of shields to add
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::AddShields(sint32 s)
{
	m_shieldstore += s;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ShouldRevolt
//
// Description: Determine whether a city should revolt.
//
// Parameters : inciteBonus     : extra revolt risk over the normal happiness
//                                (e.g. from neighbouring revolting cities)
//
// Globals    : g_slicEngine	
//
// Returns    : BOOL TRUE if the city should revolt, i.e.,
//              m_happiness < g_theConstDB->GetRevolutionLevel()+incite_bonus
//
// Remark(s)  : A city will not revolt
//              - when playing the tutorial
//              - when the civilisation is immune to revolts
//              - when it has just revolted
//              Otherwise, the revolt risk is determined by the happiness.
//
//----------------------------------------------------------------------------
BOOL CityData::ShouldRevolt(const sint32 inciteBonus)
{
	
	
	if(g_slicEngine->GetTutorialActive() &&
	   m_owner == g_slicEngine->GetTutorialPlayer()) {
		return FALSE;
	}

	if(m_home_city.Flag(k_UDF_CANT_RIOT_OR_REVOLT))
		return FALSE;

	// Modified by kaan to address bug # 12
	if (m_min_turns_revolt != 0) 
		return FALSE;

	return (m_happy->ShouldRevolt(inciteBonus)) ;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::NoRevoltCountdown
//
// Description: Decrease the number of turns that the city is free from revolt
//              risk.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Modified by kaan to address bug # 12
//
//----------------------------------------------------------------------------
void CityData::NoRevoltCountdown() 
{
	if (m_min_turns_revolt > 0) 
	{
		m_min_turns_revolt--;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::Revolt
//
// Description: Make a city revolt
//
// Parameters : sint32 &playerToJoin    : The city's new owner
//              BOOL causeIsExternal    :
//              
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : - causeIsExternal defaults to FALSE in citydata.h
//
//----------------------------------------------------------------------------
void CityData::Revolt(sint32 &playerToJoin, BOOL causeIsExternal)
{
	sint32 i,
	       n;

	PLAYER_INDEX const  orgowner    = m_owner;

	CellUnitList army;

	MapPoint     city_pos,
	             new_city;

	BOOL         joined_egalatarians = FALSE;

	g_player[m_owner]->m_score->AddRevolution();

	m_home_city.GetPos(city_pos) ;
	
	if(!causeIsExternal && g_network.IsActive()) {
		g_network.Block(m_owner);
	}
	
	PLAYER_INDEX        newowner    = PLAYER_UNASSIGNED;

	if(wonderutil_GetRevoltingCitiesJoinPlayer(g_theWonderTracker->GetBuiltWonders())) {
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			if(g_player[p]) {
				if(wonderutil_GetRevoltingCitiesJoinPlayer(
					g_player[p]->m_builtWonders)) {
					newowner = p;
					
					
					
					
					
					if(newowner == m_owner) {
						newowner = PLAYER_UNASSIGNED;
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
	
	m_home_city.ResetCityOwner(newowner, FALSE, CAUSE_REMOVE_CITY_HAPPINESS_REVOLT);
	m_build_queue.Clear();

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

	sint32       soundID = -1, spriteID = -1;

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

	// Modified by kaan to address bug # 12
	// Prevent city from revolting twice in the same turn.
	m_min_turns_revolt = (uint8)g_theConstDB->GetMinTurnsBetweenRevolt(); 
}

//----------------------------------------------------------------------------
//
// Name       : CityData::TeleportUnits
//
// Description: 
//
// Parameters : MapPoint &pos                 : destination
//              BOOL &revealed_foreign_units  : zeroed out
//              BOOL &revealed_unexplored     : zeroed out
//              sint32 foreigner              : recipient
//              
// Globals    : g_theWorld
//            : g_gevManager
//
// Returns    : -
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
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
	
	
	
	
	
	

	static CellUnitList	units;
	units.Clear();

	MapPoint city_pos;

	sint32   i,
	         n;

	m_home_city.GetPos(city_pos);
	g_theWorld->GetCell(city_pos)->GetArmy(units);
	n = units.Num();

	revealed_foreign_units = FALSE;
	revealed_unexplored = FALSE;
	static UnitDynamicArray revealed;
	static DynamicArray<Army> moveArmies;
	moveArmies.Clear();
	revealed.Clear();

	for (i=0; i<n; i++){
		revealed.Clear();
		MapPoint oldpos;

		units[i].GetPos(oldpos);

		g_theWorld->RemoveUnitReference(oldpos, units[i]);
		units[i].SetPosition(pos, revealed, revealed_unexplored);




		if (0 < revealed.Num()) {
			revealed_foreign_units = TRUE;
		}
	}
	for(i = 0; i < n; i++) {
		
		
		
		units[i].GetArmy().ResetPos();
	}
#endif
}

//----------------------------------------------------------------------------
//
// Name       : CityData::StopTradingWith
//
// Description: Stop all trade between this city and the player bannedRecipient
//
// Parameters : PLAYER_INDEX bannedRecipient:
//              
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
void CityData::StopTradingWith(PLAYER_INDEX bannedRecipient)
{
	TradeRoute route;

	Unit       srcCity,
	           destCity;

	sint32      i;
	sint32      n = m_tradeSourceList.Num();

	for(i = 0; i < n; i++)
	{
		route = m_tradeSourceList[i];
		srcCity = route.GetSource();
		destCity = route.GetDestination();
		if ((srcCity.GetOwner() == bannedRecipient) || (destCity.GetOwner() == bannedRecipient))
			route.Kill(CAUSE_KILL_TRADE_ROUTE_DIPLOMATIC_AGREEMENT);

	}

	
	n = m_tradeDestinationList.Num();
	for(i = 0; i < n; i++)
	{
		route = m_tradeDestinationList[i];
		srcCity = route.GetSource();
		destCity = route.GetDestination();
		if ((srcCity.GetOwner() == bannedRecipient) && (destCity.GetOwner() == bannedRecipient))
			route.Kill(CAUSE_KILL_TRADE_ROUTE_DIPLOMATIC_AGREEMENT);

	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalcPollution
//
// Description: Calculate the pollution produced by this city
//
// Parameters : -
//              
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : fills in
//                         m_cityPopulationPollution
//                         m_cityIndustrialPollution
//                         m_total_pollution
//
//----------------------------------------------------------------------------
void CityData::CalcPollution(void)
{
	sint32 populationPolluting = PopCount() - 
		g_theDifficultyDB->GetPollutionStartPopulationLevel(g_theGameSettings->GetDifficulty());
	if(populationPolluting < 0) 
	{
		populationPolluting = 0;
	}
	else
		populationPolluting = (sint32)(populationPolluting * 
			g_theDifficultyDB->GetPollutionPopulationRatio(g_theGameSettings->GetDifficulty()));

	populationPolluting = (sint32)(populationPolluting * g_player[m_owner]->GetPollutionCoef());

	sint32 productionPolluting = m_gross_production -
		g_theDifficultyDB->GetPollutionStartProductionLevel(g_theGameSettings->GetDifficulty());
	if(productionPolluting < 0)
	{
		productionPolluting = 0;
	}
	else
		productionPolluting = (sint32)(productionPolluting * 
			g_theDifficultyDB->GetPollutionProductionRatio(g_theGameSettings->GetDifficulty()));

	productionPolluting = (sint32)(productionPolluting * g_player[m_owner]->GetPollutionCoef());

	double buildingPollution=0.0;
	double buildingProductionPercentage=0.0;
	double buildingPopulationPercentage=0.0;
	sint32 i;
	uint64 buildingCheck;
	double temp;

	for(i=0; i<g_theBuildingDB->NumRecords(); i++)
	{
		buildingCheck = (uint64)1 << (uint64)i;
		if(GetEffectiveBuildings() & buildingCheck)
		{
			if(g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetPollutionAmount(temp))
			{
				buildingPollution+=temp;
			}
			if(g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetProductionPollutionPercent(temp))
			{
				buildingProductionPercentage+=temp;
			}
			if(g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetPopulationPollutionPercent(temp))
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
			if(g_theWonderDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetPollutionAmount(temp))
			{
				buildingPollution+=temp;
			}
			if(g_theWonderDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetPollutionPercent(temp))
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

//----------------------------------------------------------------------------
//
// Name       : CityData::DoLocalPollution
//
// Description: Add dead tiles near polluting cities
//
// Parameters : -
//              
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Could be changed so that unhappy citizens emigrate, PFT 
//
//----------------------------------------------------------------------------
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
		g_slicEngine->Execute(so);
	}

	if(g_rand->Next(1000) < chance * 1000) {
		SlicObject *so = new SlicObject("040GrossPolluter");
		so->AddCity(m_home_city);
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);

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
#if !defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : CityData::ComputeGrossProduction
//
// Description: Calculates the production of the city for this turn and the 
//              losses to crime and franchise dependent on workday.
//
// Parameters : workday_per_person:      The workday length (g_player[m_owner]->GetWorkdayPerPerson() )
//              collectedProduction:     Filled with the net production (m_collected_production_this_turn).
//              crime_loss:              Filled with the loss due to crime (m_production_lost_to_crime).
//              franchise_loss:          Filled with the loss to franchise (m_productionLostToFranchise).
//              considerOnlyFromTerrain: Whether labors should be considered.
//
// Globals    : g_player:                The list of players
//              g_theGovernmentDB:       The government database
//              g_thePopDB:              The pop database
//              g_theConstDB:            The constant database
//
// Returns    : sint32: The gross production including losses due bioinfection
//
// Remark(s)  : Use this method for estimation.
//
//----------------------------------------------------------------------------
sint32 CityData::ComputeGrossProduction(double workday_per_person, sint32 collected_production, sint32 & crime_loss, sint32 & franchise_loss, bool considerOnlyFromTerrain) const
{
	double gross_production = collected_production;

	gross_production = ceil(gross_production * workday_per_person);

	double prodBonus;
	buildingutil_GetProductionPercent(GetEffectiveBuildings(), prodBonus);
	gross_production += ceil(gross_production * prodBonus);

	sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_PRODUCTION, m_owner);
	gross_production += ceil(gross_production * (static_cast<double>(featPercent) / 100.0));
	
	gross_production += ceil(gross_production *
	                        (wonderutil_GetIncreaseProduction(g_player[m_owner]->m_builtWonders) * 0.01));

	gross_production = ceil(gross_production * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetProductionCoef());

	if(!considerOnlyFromTerrain && m_specialistDBIndex[POP_LABORER] >= 0){
		gross_production += LaborerCount() *
			g_thePopDB->Get(m_specialistDBIndex[POP_LABORER], g_player[m_owner]->GetGovernmentType())->GetProduction();
	}

	return ComputeProductionLosses(static_cast<sint32>(gross_production), crime_loss, franchise_loss);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ComputeProductionLosses
//
// Description: Calculates the production of the city for this turn and the 
//              losses to crime and franchise dependent on workday.
//
// Parameters : gross_production:        The gross production
//              crime_loss:              Filled with the loss due to crime (m_production_lost_to_crime).
//              franchise_loss:          Filled with the loss to franchise (m_productionLostToFranchise).
//
// Globals    : g_theConstDB:            The constant database
//
// Returns    : sint32: The gross production including losses due bioinfection
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::ComputeProductionLosses(sint32 gross_production, sint32 &crime_loss, sint32 &franchise_loss) const
{

	if(m_bioInfectionTurns > 0){
		gross_production -= static_cast<sint32>(ceil(gross_production * g_theConstDB->GetBioInfectionProductionCoef()));
	}

	crime_loss = CrimeLoss(static_cast<sint32>(gross_production));
	
	double net_production = gross_production - crime_loss;

	if(m_franchise_owner >= 0)
		franchise_loss = static_cast<sint32>(ceil(net_production * g_theConstDB->GetFranchiseEffect()));
	else
		franchise_loss = 0;

	return gross_production;

}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessProduction
//
// Description: Calculates the production of the city for this turn and the 
//              losses to crime and franchise.
//
// Parameters : projectedOnly: Do not add production to franchise owner and
//                             produced items.
//
// Globals    : -
//
// Returns    : sint32: The net production of this turn.
//
// Remark(s)  : Use this method to recalculate production.
//
//----------------------------------------------------------------------------
sint32 CityData::ProcessProduction(bool projectedOnly)
{
	m_net_production = ProcessProduction(projectedOnly, 
	                                        m_gross_production, 
	                                        m_collected_production_this_turn, 
	                                        m_production_lost_to_crime, 
	                                        m_productionLostToFranchise);
	return m_net_production;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessProduction
//
// Description: Calculates the production of the city for this turn and the 
//              losses to crime and franchise.
//
// Parameters : projectedOnly:           Do not add production to franchise 
//                                       owner and produced items.
//              grossProduction:         The production without losses
//              collectedProduction:     Filled with the net production.
//              crimeLoss:               Filled with the loss due to crime
//              franchiseLoss:           Filled with the loss to franchise
//              considerOnlyFromTerrain: Whether labors should be considered.
//
// Globals    : g_player[m_franchise_owner] : If the city is franchised, this is who did it
//
// Returns    : sint32: The net production
//
// Remark(s)  : grossProduction is modified.
//              Use this method to estimate the production from grossProdction.
//
//----------------------------------------------------------------------------
sint32 CityData::ProcessProduction(bool projectedOnly, sint32 &grossProduction, sint32 &collectedProduction, sint32 &crimeLoss, sint32 &franchiseLoss, bool considerOnlyFromTerrain) const
{

	grossProduction = 
	    ComputeGrossProduction(g_player[m_owner]->GetWorkdayPerPerson(),
	                          collectedProduction,
	                          crimeLoss,
	                          franchiseLoss,
	                          considerOnlyFromTerrain);
	
	sint32 shields = grossProduction -
	    (crimeLoss + franchiseLoss);

	if( m_franchise_owner >= 0 ) {
		if(!projectedOnly)
			g_player[m_franchise_owner]->AddProductionFromFranchise(franchiseLoss);
	}

//Added by E - EXPORT BONUSES TO GOODS if has good than a production bonus	
	sint32 good;
	for(good = 0; good < g_theResourceDB->NumRecords(); good++) {
		if((m_buyingResources[good] + m_collectingResources[good] - m_sellingResources[good]) > 0){
			grossProduction += ceil(grossProduction * (g_theResourceDB->Get(good))->GetProductionPercent());		
		}
	}

//Added by E - EXPORT BONUSES TO GOODS This causes a crime effect if negative and efficiency if positive
	sint32 g;
	for(g = 0; g < g_theResourceDB->NumRecords(); g++) {
		if((m_buyingResources[g] + m_collectingResources[g] - m_sellingResources[g]) > 0){
			grossProduction += ceil(grossProduction * (g_theResourceDB->Get(g))->GetEfficiencyOrCrime());		
		}
	}

	return shields;
}
#endif

// used in NewTurnCount::VerifyEndTurn
double CityData::ProjectMilitaryContribution()
{
	return (double)m_net_production; 
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ComputeMaterialsPaid
//
// Description: Test whether this city should save some shields.
//
// Parameters : double percent_terrain  :  actually, player->m_materialsTax 
//       
// Globals    : -
//
// Returns    : sint32 (1 or 0)
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
sint32 CityData::ComputeMaterialsPaid(double percent_terrain)
{
	return(m_contribute_materials ? (static_cast<sint32>(
		(static_cast<double>(m_net_production) *
		percent_terrain) + 0.000001)) : 0);
}

void CityData::PayFederalProduction (double percent_military,
                                     sint32 &mil_paid, 
                                     double percent_terrain, 
                                     sint32 &mat_paid)

{


#if defined(_DEBUG) || defined(USE_LOGGING)
	sint32 origShields = m_net_production;
#endif

	if (m_contribute_military) {
		mil_paid  = (sint32)(ceil((double)m_net_production * percent_military));
		m_net_production -= mil_paid;
	} else {
		mil_paid = 0;
	}

	mat_paid = ComputeMaterialsPaid(percent_terrain);
	if (m_contribute_materials) {
		m_net_production -= mat_paid;
	}

	DPRINTF(k_DBG_GAMESTATE, ("City %lx: S: %d, %d mil(%lf), %d mat(%lf)\n", (uint32)m_home_city, origShields, mil_paid, percent_military, mat_paid, percent_terrain));

	Assert (0 <= m_net_production);

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

	m_net_production -= mil_paid;

	mat_paid = ComputeMaterialsPaid(percent_mat);
	m_net_production -= mat_paid;








	Assert (0 <= m_net_production);
	if (m_net_production < 0) {
		m_net_production = 0;
	}
}


void CityData::AddShieldsToBuilding()
{
	if (m_net_production < 0) { 
		m_net_production = 0; 
	}

	if(m_is_rioting) {
		m_net_production = 0;
	}

	SetShieldstore (m_shieldstore + m_net_production); 
}

#if !defined(NEW_RESOURCE_PROCESS)
void CityData::GetFullAndPartialRadii(sint32 &fullRadius, sint32 &partRadius) const
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

	if (squaredDistance > (uint32)partSquaredRadius)
	{
		return 0.0;	// Not in the city influence (yet).
	}
	else if (squaredDistance <= (uint32)fullSquaredRadius)
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
			std::min<sint32>(m_workerFullUtilizationIndex, dbMaxIndex - 1);

		fullRingWorkers = g_theCitySizeDB->Get(dbIndex)->GetMaxWorkers();
	}

	// # of working when the outer ring would have been filled to the max.
	sint32          maxRingWorkers		= 0;	
	if (m_workerPartialUtilizationIndex >= 0)
	{
		sint32 const	dbIndex			= 
			std::min<sint32>(m_workerPartialUtilizationIndex, dbMaxIndex - 1);

		maxRingWorkers = g_theCitySizeDB->Get(dbIndex)->GetMaxWorkers();
	}

	// # of actually working in the partial ring.
	sint32 const		partialRingWorkers	= 	
			std::min(maxRingWorkers, WorkerCount() + SlaveCount())
				- fullRingWorkers;


	return (partialRingWorkers <= 0)
		   ? 0.0 
		   : static_cast<double>(partialRingWorkers) / 
		     static_cast<double>(maxRingWorkers - fullRingWorkers);
}
#endif

//----------------------------------------------------------------------------
//
// Name       : CityData::CollectResources
//
// Description: Collects the resources food, production and gold from the field.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Added EnablesGood check for tile improvemnts.
//
//----------------------------------------------------------------------------
void CityData::CollectResources() 
{
	sint32 fullFoodTerrainTotal = 0;
	sint32 partFoodTerrainTotal = 0;

	sint32 fullProdTerrainTotal = 0;
	sint32 partProdTerrainTotal = 0;

	sint32 fullGoldTerrainTotal = 0;
	sint32 partGoldTerrainTotal = 0;

#if !defined(NEW_RESOURCE_PROCESS)
	sint32 fullSquaredRadius;
	sint32 partSquaredRadius;

	GetFullAndPartialRadii(fullSquaredRadius, partSquaredRadius);
#endif
	
	MapPoint cityPos = m_home_city.RetPos();

	m_collectingResources.Clear();
	memset(m_ringFood,  0, sizeof(sint32) * g_theCitySizeDB->NumRecords());
	memset(m_ringProd,  0, sizeof(sint32) * g_theCitySizeDB->NumRecords());
	memset(m_ringGold,  0, sizeof(sint32) * g_theCitySizeDB->NumRecords());
	memset(m_ringSizes, 0, sizeof(sint32) * g_theCitySizeDB->NumRecords());

#if defined(NEW_RESOURCE_PROCESS)
	memset(m_farmersEff,    0, sizeof(double) * g_theCitySizeDB->NumRecords());
	memset(m_laborersEff,   0, sizeof(double) * g_theCitySizeDB->NumRecords());
	memset(m_merchantsEff,  0, sizeof(double) * g_theCitySizeDB->NumRecords());
	memset(m_scientistsEff, 0, sizeof(double) * g_theCitySizeDB->NumRecords());
#endif


// Add if city has building GetEnablesGood >0 then that good will be dded to the city for trade
    sint32 good; 
	for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++) {
		if(m_built_improvements & ((uint64)1 << b)) {
			const BuildingRecord *rec = g_theBuildingDB->Get(b, g_player[m_owner]->GetGovernmentType());
//      Check If needsGood for the building a make bonuses dependent on having that good for further bonus
			if (rec->GetNumEnablesGood() > 0){
				for(good = 0; good < rec->GetNumEnablesGood(); good++) {
  						m_collectingResources.AddResource(rec->GetEnablesGoodIndex(good));
				}
			}
		}
	}

// end building enables good

// Add if city has wonder GetEnablesGood >0 then that good will be dded to the city for trade
    sint32 wgood; 
	for(sint32 w = 0; w < g_theWonderDB->NumRecords(); w++) {
		if(m_builtWonders & ((uint64)1 << w)) {
			const WonderRecord *rec = wonderutil_Get(w);
//      Check If needsGood for the building a make bonuses dependent on having that good for further bonus
			if (rec->GetNumEnablesGood() > 0){
				for(wgood = 0; wgood < rec->GetNumEnablesGood(); wgood++) {
  						m_collectingResources.AddResource(rec->GetEnablesGoodIndex(wgood));
				}
			}
		}
	}
// end wonder enables good

	CityInfluenceIterator it(cityPos, m_sizeIndex);
	for(it.Start(); !it.End(); it.Next()) {
		Cell *cell = g_theWorld->GetCell(it.Pos());
		sint32 ring = GetRing(it.Pos());
		m_ringFood[ring] += cell->GetFoodProduced();
		m_ringProd[ring] += cell->GetShieldsProduced();
		m_ringGold[ring] += cell->GetGoldProduced();
		m_ringSizes[ring]++;
		sint32 good;
		if(g_theWorld->GetGood(it.Pos(), good)
#if !defined(NEW_RESOURCE_PROCESS)
		&& MapPoint::GetSquaredDistance(cityPos, it.Pos()) <= partSquaredRadius
#endif
		){
			m_collectingResources.AddResource(good);
		}


// Added by E (10-29-2005) - If a tileimp

		for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++) {
		sint32 imp = cell->GetDBImprovement(i);
		const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
//      Check If needsGood for the building a make bonuses dependent on having that good for further bonus
			if (rec->GetNumEnablesGood() > 0){
				for(good = 0; good < rec->GetNumEnablesGood(); good++) {
  						m_collectingResources.AddResource(rec->GetEnablesGoodIndex(good));
				}
			}
		}

	}

#if defined(NEW_RESOURCE_PROCESS)

	sint32 i;
	for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
		m_max_food_from_terrain += m_ringFood[i];
		m_max_prod_from_terrain += m_ringProd[i];
		m_max_gold_from_terrain += m_ringGold[i];
	}

#else // Should go next time
	sint32 i;
	for(i = 0; i <= m_workerFullUtilizationIndex; ++i){
		fullFoodTerrainTotal += m_ringFood[i];
		fullProdTerrainTotal += m_ringProd[i];
		fullGoldTerrainTotal += m_ringGold[i];
	}
	partFoodTerrainTotal = m_ringFood[m_workerPartialUtilizationIndex];
	partProdTerrainTotal = m_ringProd[m_workerPartialUtilizationIndex];
	partGoldTerrainTotal = m_ringGold[m_workerPartialUtilizationIndex];

	double const	utilizationRatio	= GetUtilisationRatio(partSquaredRadius);

	m_max_food_from_terrain = fullFoodTerrainTotal + partFoodTerrainTotal;
	m_max_prod_from_terrain = fullProdTerrainTotal + partProdTerrainTotal;
	m_max_gold_from_terrain = fullGoldTerrainTotal + partGoldTerrainTotal;

	if(m_is_rioting) {
		m_max_gold_from_terrain = 0;
		fullGoldTerrainTotal = 0;
		partGoldTerrainTotal = 0;
	}

	m_gross_food = fullFoodTerrainTotal + ceil(utilizationRatio * double(partFoodTerrainTotal));
	m_gross_production = (sint32)(fullProdTerrainTotal + ceil(utilizationRatio * double(partProdTerrainTotal)));
	m_gross_gold = (sint32)(fullGoldTerrainTotal + ceil(utilizationRatio * double(partGoldTerrainTotal)));

	
	m_collected_production_this_turn = m_gross_production;
	m_net_production = m_gross_production;
	m_net_food = m_gross_food;
	m_net_gold = m_gross_gold;
#endif
}

#if defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessResources
//
// Description: Processes all the resources of food, production, gold and 
//              science from the field to get the maximal values possible
//              if all fields have a worker. And computes all the losses.
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
void CityData::ProcessResources()
{
	///////////////////////////////////////////////
	// Split science from gold
	double s;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(s);
	m_max_scie_from_terrain = static_cast<sint32>(ceil(m_max_gold_from_terrain * s));
	m_max_gold_from_terrain -= m_max_scie_from_terrain;

	///////////////////////////////////////////////
	// Process resources: Building, feat, wonder and boni from
	// citizens
	m_max_processed_terrain_food = ProcessFood(m_max_food_from_terrain);
	m_max_processed_terrain_prod = ProcessProd(m_max_prod_from_terrain);
	m_max_processed_terrain_gold = ProcessGold(m_max_gold_from_terrain);
	m_max_processed_terrain_scie = ProcessScie(m_max_scie_from_terrain);

	///////////////////////////////////////////////
	// Apply government coefficents:
	ApplyFoodCoeff(m_max_processed_terrain_food);
	ApplyProdCoeff(m_max_processed_terrain_prod);
	ApplyGoldCoeff(m_max_processed_terrain_gold);
	ApplyKnowledgeCoeff(m_max_processed_terrain_scie);

	///////////////////////////////////////////////
	// Apply slider modifers:
	// Food modified by food eaten
	m_max_processed_terrain_prod *= g_player[m_owner]->GetWorkdayPerPerson();
	// Gold modified by wages height
	// No sliders for science


	///////////////////////////////////////////////
	// Calculate losses

	///////////////////////////////////////////////
	// Calculate food losses:
	// No speacal attack loss
	m_grossFoodCrimeLoss = CrimeLoss(m_max_processed_terrain_food);
	// No specal attack loss to other players

	///////////////////////////////////////////////
	// Calculate production losses:
	m_grossProdBioinfectionLoss = BioinfectionLoss(m_max_processed_terrain_prod);
	m_grossProdCrimeLoss = CrimeLoss(m_max_processed_terrain_prod - m_grossProdBioinfectionLoss);
	m_grossProdFranchiseLoss = FranchiseLoss(m_max_processed_terrain_prod - m_grossProdBioinfectionLoss - m_grossProdCrimeLoss);

	///////////////////////////////////////////////
	// Calculate gold losses:
	// No speacal attack loss
	m_grossGoldCrimeLoss = CrimeLoss(m_max_processed_terrain_gold);
	m_grossGoldConversionLoss = ConversionLoss(m_max_processed_terrain_gold - m_grossGoldCrimeLoss);

	///////////////////////////////////////////////
	// Calculate science losses:
	// No speacal attack loss
	m_grossScieCrimeLoss = CrimeLoss(m_max_processed_terrain_scie);
	// No specal attack loss to other players


	///////////////////////////////////////////////
	// Calculate resources from specialists
	m_foodFromOnePop = FoodFromOnePop(m_crimeFoodLossOfOnePop);
	m_prodFromOnePop = ProdFromOnePop(m_crimeProdLossOfOnePop, m_bioinfectionProdLossOfOnePop, m_franchiseProdLossOfOnePop);
	m_goldFromOnePop = GoldFromOnePop(m_crimeGoldLossOfOnePop, m_conversionGoldLossOfOnePop);
	m_scieFromOnePop = ScieFromOnePop(m_crimeScieLossOfOnePop);

}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateResources
//
// Description: Caculates the gross and net resources from the maximal 
//              processed resources from terrain by fractionising them from
//              the actual worker tile allocation.
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
void CityData::CalculateResources()
{
	///////////////////////////////////////////////
	// Get utilization fractions
	double foodFraction = 0;
	double prodFraction = 0;
	double goldFraction = 0;
	ResourceFractions(foodFraction, prodFraction, goldFraction, WorkerCount() + SlaveCount());

	///////////////////////////////////////////////
	// Calculate gross resources
	m_gross_food       = ceil(m_max_processed_terrain_food * foodFraction);
	m_gross_production = static_cast<sint32>(ceil(m_max_processed_terrain_prod * prodFraction * g_player[m_owner]->GetWorkdayPerPerson()));
	m_gross_gold       = static_cast<sint32>(ceil(m_max_processed_terrain_gold * goldFraction));
	m_gross_science    = ceil(m_max_processed_terrain_scie * goldFraction);

	///////////////////////////////////////////////
	// Calculate losses due to crime
	m_food_lost_to_crime           = ceil(m_grossFoodCrimeLoss * foodFraction);
	m_production_lost_to_crime     = static_cast<sint32>(ceil(m_grossProdCrimeLoss * prodFraction * g_player[m_owner]->GetWorkdayPerPerson()));
	m_gold_lost_to_crime           = static_cast<sint32>(ceil(m_grossGoldCrimeLoss * goldFraction));
	m_science_lost_to_crime        = ceil(m_grossScieCrimeLoss * goldFraction);

	///////////////////////////////////////////////
	// Calculate losses due to special attacks
	m_productionLostToFranchise    = static_cast<sint32>(ceil(m_grossProdFranchiseLoss * prodFraction * g_player[m_owner]->GetWorkdayPerPerson()));
	m_productionLostToBioinfection = static_cast<sint32>(ceil(m_grossProdBioinfectionLoss * prodFraction * g_player[m_owner]->GetWorkdayPerPerson()));
	m_convertedGold                = static_cast<sint32>(ceil(m_grossGoldConversionLoss * goldFraction));

	if(m_is_rioting) {
		m_gross_gold         = 0;
		m_gold_lost_to_crime = 0;
		m_convertedGold      = 0;
	}

	///////////////////////////////////////////////
	// Calculate net resources without specialists' constribution
	m_net_food       = m_gross_food - m_food_lost_to_crime;
	m_net_production = m_gross_production - m_production_lost_to_crime - m_productionLostToFranchise - m_productionLostToBioinfection;
	m_net_gold       = m_gross_gold - m_gold_lost_to_crime - m_convertedGold;
	m_science        = static_cast<sint32>(m_gross_science - m_science_lost_to_crime);

	///////////////////////////////////////////////
	// Add gross resources from specialists
	m_gross_food       += ceil(m_foodFromOnePop * FarmerCount());
	m_gross_production += static_cast<sint32>(ceil(m_prodFromOnePop * LaborerCount()));
	m_gross_gold       += static_cast<sint32>(ceil(m_goldFromOnePop * MerchantCount()));
	m_gross_science    += ceil(m_scieFromOnePop * ScientistCount());

	///////////////////////////////////////////////
	// Add net resources from specialists
	m_net_food       += ceil((m_foodFromOnePop - m_crimeFoodLossOfOnePop) * FarmerCount());
	m_net_production += static_cast<sint32>(ceil((m_prodFromOnePop - m_crimeProdLossOfOnePop - m_bioinfectionProdLossOfOnePop - m_franchiseProdLossOfOnePop) * LaborerCount()));
	m_net_gold       += static_cast<sint32>(ceil((m_goldFromOnePop - m_crimeGoldLossOfOnePop - m_conversionGoldLossOfOnePop) * MerchantCount()));
	m_science        += static_cast<sint32>(ceil((m_scieFromOnePop - m_crimeScieLossOfOnePop) * ScientistCount()));

	ComputeSpecialistsEffects();

	///////////////////////////////////////////////
	// Add gross gold from trade routes to gross gold
	sint32 gold = m_goldFromTradeRoutes;
	ApplyGoldCoeff(gold);
	m_gross_gold += gold;

	///////////////////////////////////////////////
	// Add net gold from trade routes to net gold
	sint32 crimeLossGold = CrimeLoss(gold);
	double conversionLossGold = ConversionLoss(static_cast<double>(gold - crimeLossGold));
	m_gold_lost_to_crime += crimeLossGold;
	m_convertedGold += static_cast<sint32>(conversionLossGold);
	m_net_gold += gold - crimeLossGold - static_cast<sint32>(conversionLossGold);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::PayResources
//
// Description: Adds the earned gold of this turn to the player's income.
//              Pays the losses of franchise and conversion to the
//              according owners.
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
void CityData::PayResources()
{
	g_player[m_owner]->m_gold->AddIncome(m_net_gold);

	if(m_franchise_owner >= 0
	&& g_player[m_franchise_owner]
	){
		g_player[m_franchise_owner]->AddProductionFromFranchise(m_productionLostToFranchise);
	}
	
	if(m_convertedTo >= 0
	&& g_player[m_convertedTo]
	){
		g_player[m_convertedTo]->AddGold(m_convertedGold);
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::AddCapitalizationAndTryToBuild
//
// Description: Handles build item construction and infrastructure and 
//              capitalization generation, and adds capitalisation to losses
//              and income.
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
void CityData::AddCapitalizationAndTryToBuild()
{
	///////////////////////////////////////////////
	// Generate m_gold_from_capitalization for the following step
	// And try to build something
	TryToBuild();

	///////////////////////////////////////////////
	// Add gross gold from capitalization to gross gold
	m_gross_gold += m_gold_from_capitalization;

	///////////////////////////////////////////////
	// Add net gold from capitalization to net gold
	sint32 crimeLossGold = CrimeLoss(m_gold_from_capitalization);
	double conversionLossGold = ConversionLoss(static_cast<double>(m_gold_from_capitalization - crimeLossGold));
	m_gold_lost_to_crime += crimeLossGold;
	m_convertedGold += static_cast<sint32>(conversionLossGold);
	m_net_gold += m_gold_from_capitalization - crimeLossGold - static_cast<sint32>(conversionLossGold);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ComputeSpecialistsEffects
//
// Description: Computes the amount of resoucres one specialist adds to the 
//              amount of resources to the total amount.
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
void CityData::ComputeSpecialistsEffects()
{
	sint32 totalFood = GetFoodFromRing(-1);
	sint32 totalProd = GetProdFromRing(-1);
	sint32 totalGold = GetGoldFromRing(-1);

	sint32 i;
	for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
		m_farmersEff[i]     = (m_foodFromOnePop - m_crimeFoodLossOfOnePop) - m_net_food * (static_cast<double>(m_ringFood[i]) / static_cast<double>(totalFood));
		m_laborersEff[i]    = (m_prodFromOnePop - m_crimeProdLossOfOnePop - m_bioinfectionProdLossOfOnePop - m_franchiseProdLossOfOnePop) - m_net_production * (static_cast<double>(m_ringProd[i]) / static_cast<double>(totalProd));
		m_merchantsEff[i]   = (m_goldFromOnePop - m_crimeGoldLossOfOnePop - m_conversionGoldLossOfOnePop) - m_net_gold * (static_cast<double>(m_ringGold[i]) / static_cast<double>(totalGold));
		m_scientistsEff[i]  = (m_scieFromOnePop - m_crimeScieLossOfOnePop) - m_science * (static_cast<double>(m_ringGold[i]) / static_cast<double>(totalGold));
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessFood
//
// Description: Calculates the gross food of the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Returns the amount of gross food this turn.
//
// Remark(s)  : By E - this function appears not to be used
//
//----------------------------------------------------------------------------
double CityData::ProcessFood(sint32 food) const
{
	///////////////////////////////////////////////
	// Apply building boni
	double foodBonus;
	buildingutil_GetFoodPercent(GetEffectiveBuildings(), foodBonus);
	double grossFood = static_cast<double>(food) * foodBonus;

	///////////////////////////////////////////////
	// Apply feat boni
	// No feat boni. Something to add here
	
	///////////////////////////////////////////////
	// Apply wonder boni
	grossFood += grossFood * (wonderutil_GetIncreaseFoodAllCities(
	                   g_player[m_owner]->m_builtWonders) / 100.0);

	///////////////////////////////////////////////
	// Add food from citizen
	// No food from citizen. Maybe something to add.

	///////////////////////////////////////////////

		

	
	return grossFood;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessProd
//
// Description: Calculates the gross production of the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Returns the amount of gross production this turn.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ProcessProd(sint32 prod) const
{
	///////////////////////////////////////////////
	// Apply building boni
	double prodBonus;
	buildingutil_GetProductionPercent(GetEffectiveBuildings(), prodBonus);
	double grossProd = static_cast<double>(prod) * prodBonus;

	///////////////////////////////////////////////
	// Apply feat boni
	sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_PRODUCTION, m_owner);
	grossProd += grossProd * (static_cast<double>(featPercent) / 100.0);
	
	///////////////////////////////////////////////
	// Apply wonder boni
	grossProd += grossProd * (wonderutil_GetIncreaseProduction(
	                   g_player[m_owner]->m_builtWonders) * 0.01);

	///////////////////////////////////////////////
	// Add prod from citizen
	// No prod from citizen. Maybe something to add.

	return grossProd;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessGold
//
// Description: Calculates the gross gold of the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Returns the amount of gross gold this turn.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ProcessGold(sint32 gold) const
{
	double grossGold = 0.0;
	if(gold > 0) {
		
		///////////////////////////////////////////////
		// Apply building boni
		double goldBonus;
		buildingutil_GetCommercePercent(GetEffectiveBuildings(), goldBonus, m_owner);
		grossGold += static_cast<double>(gold) * goldBonus;

		///////////////////////////////////////////////
		// Apply feat boni
		sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_COMMERCE, m_owner);
		grossGold += grossGold * (featPercent / 100.0);
		
		///////////////////////////////////////////////
		// Apply wonder boni
		// No wonder boni. Something to add here
	}

	///////////////////////////////////////////////
	// Add gold from citizen
	sint32 goldPerCitizen = buildingutil_GetGoldPerCitizen(GetEffectiveBuildings());
	grossGold += static_cast<double>(goldPerCitizen * PopCount());

	return grossGold;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessScie
//
// Description: Calculates the gross science of the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Returns the amount of gross science this turn.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ProcessScie(sint32 science) const
{
	double grossScience = static_cast<double>(science);

	double popWonderModifier = static_cast<double>(wonderutil_GetIncreaseScientists(m_builtWonders));
	popWonderModifier += static_cast<double>(wonderutil_GetIncreaseSpecialists(g_player[m_owner]->m_builtWonders));
	
	if(popWonderModifier && m_specialistDBIndex[POP_SCIENTIST] >= 0 
	&& m_specialistDBIndex[POP_SCIENTIST] < g_thePopDB->NumRecords()
	){
		grossScience += popWonderModifier * 
		             g_thePopDB->Get(m_specialistDBIndex[POP_SCIENTIST], g_player[m_owner]->GetGovernmentType())->GetScience();
	}

	grossScience += buildingutil_GetIncreaseSciencePerPop(GetEffectiveBuildings()) * static_cast<double>(PopCount() - SlaveCount());

	///////////////////////////////////////////////
	// Apply building boni
	double scienceBonus;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), scienceBonus);
	grossScience += static_cast<double>(grossScience) * scienceBonus;

	///////////////////////////////////////////////
	// Apply feat boni
	// No feat boni. Something to add here

	///////////////////////////////////////////////
	// Apply wonder boni
	grossScience += grossScience * (wonderutil_GetIncreaseKnowledgePercentage(
	                     g_player[m_owner]->GetBuiltWonders()) * 0.01);

	///////////////////////////////////////////////
	// Add science from citizen
	// No science from citizen. Maybe something to add.

	if(grossScience < 0.0) {
		grossScience = 0.0;
	}

	return grossScience;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ApplyFoodCoeff
//
// Description: Modifies the given amount of food by the player's government
//              food coeffiecient.
//
// Parameters : double &food:        Amount of gross food.
//
// Globals    : g_player:            List of players
//              g_theGovernmentDB:   The government databse
//
// Returns    : -
//
// Remark(s)  : food is modified by this method.
//
//----------------------------------------------------------------------------
void CityData::ApplyFoodCoeff(double &food) const
{
	food *= g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetFoodCoef();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ApplyProdCoeff
//
// Description: Modifies the given amount of prod by the player's government
//              production coeffiecient.
//
// Parameters : double &prod:        Amount of gross production.
//
// Globals    : g_player:            List of players
//              g_theGovernmentDB:   The government databse
//
// Returns    : -
//
// Remark(s)  : prod is modified by this method.
//
//----------------------------------------------------------------------------
void CityData::ApplyProdCoeff(double &prod) const
{
	prod *= g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetProductionCoef();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ApplyGoldCoeff
//
// Description: Modifys the given amount of gold by the player's government
//              gold coeffiecient.
//
// Parameters : sint32 &gold:        Amount of gross gold.
//
// Globals    : g_player:            List of players
//              g_theGovernmentDB:   The government databse
//
// Returns    : -
//
// Remark(s)  : gold is modified by this method.
//
//----------------------------------------------------------------------------
void CityData::ApplyGoldCoeff(sint32 &gold) const
{
	gold = static_cast<sint32>(gold * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetGoldCoef());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ApplyGoldCoeff
//
// Description: Modifies the given amount of gold by the player's government
//              gold coeffiecient.
//
// Parameters : double &gold:        Amount of gross gold.
//
// Globals    : g_player:            List of players
//              g_theGovernmentDB:   The government databse
//
// Returns    : -
//
// Remark(s)  : gold is modified by this method.
//
//----------------------------------------------------------------------------
void CityData::ApplyGoldCoeff(double &gold) const
{
	gold *= g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetGoldCoef();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ApplyKnowledgeCoeff
//
// Description: Modifies the given amount of science by the player's government
//              knowledge coeffiecient.
//
// Parameters : double &science:     Amount of gross science.
//
// Globals    : g_player:            List of players
//
// Returns    : -
//
// Remark(s)  : science is modified by this method.
//
//----------------------------------------------------------------------------
void CityData::ApplyKnowledgeCoeff(double &science) const
{
	science *= g_player[m_owner]->GetKnowledgeCoef();
}

#else
//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessFood
//
// Description: Calculates the food production of the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Returns the amount of food produced this turn.
//
// Remark(s)  : Use this function if the growth rate should be recalculated.
//
//----------------------------------------------------------------------------
sint32 CityData::ProcessFood()
{
	ProcessFood(m_food_lost_to_crime, 
	            m_net_food, 
	            m_gross_food);

	return static_cast<sint32>(m_net_food);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessFood
//
// Description: Calculates the food production of a city.
//
// Parameters : foodLostToCrime:         Filled with the amount of food lost
//                                       to crime.
//              producedFood:            Filled with the amount of food 
//                                       produced actually.
//              grossFood:               Hypothetical food production without
//                                       crime lost
//              considerOnlyFromTerrain: Consider only food from terrain 
//                                       not from farmers
//
// Globals    : g_thePlayer:  List of players
//              g_thePopDB:   The population database
//
// Returns    : -
//
// Remark(s)  : producedFood is the input and output parameter at the same time.
//              Use this method for estimations.
//
//----------------------------------------------------------------------------
void CityData::ProcessFood(double &foodLostToCrime, double &producedFood, double &grossFood, bool considerOnlyFromTerrain) const{

	double foodBonus;
	buildingutil_GetFoodPercent(GetEffectiveBuildings(), foodBonus);
	grossFood += producedFood * foodBonus;
	
	grossFood += ceil(grossFood * 
		static_cast<double>(wonderutil_GetIncreaseFoodAllCities(
			g_player[m_owner]->m_builtWonders) / 100.0));

	if(!considerOnlyFromTerrain && m_specialistDBIndex[POP_FARMER] >= 0) {
		grossFood += FarmerCount() *
			g_thePopDB->Get(m_specialistDBIndex[POP_FARMER], g_player[m_owner]->GetGovernmentType())->GetFood();
	}

//Added by E - EXPORT BONUSES TO GOODS This gives a bonus of food	
	sint32 good;
	for(good = 0; good < g_theResourceDB->NumRecords(); good++) {
		if((m_buyingResources[good] + m_collectingResources[good] - m_sellingResources[good]) > 0){
			grossFood += ceil(grossFood * (g_theResourceDB->Get(good))->GetFoodPercent());		
		}
	}

//Added by E - EXPORT BONUSES TO GOODS This Causes a crime effect if negative and efficiency if positive	
	sint32 g;
	for(g = 0; g < g_theResourceDB->NumRecords(); g++) {
		if((m_buyingResources[g] + m_collectingResources[g] - m_sellingResources[g]) > 0){
			grossFood += ceil(grossFood * (g_theResourceDB->Get(g))->GetEfficiencyOrCrime());		
		}
	}


	producedFood = ProcessFinalFood(foodLostToCrime, grossFood);

}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessFinalFood
//
// Description: Calculates the food production that is common for terrain food
//              and pop food..
//
// Parameters : foodLossToCrime: Crime loss
//              grossFood:       Food before crime deduction
//
// Globals    : -
//
// Returns    : Returns the amount of food produced this turn on the base of
//              grossFood.
//
// Remark(s)  : foodLossToCrime and grossFood are modified
//
//----------------------------------------------------------------------------
double CityData::ProcessFinalFood(double &foodLossToCrime, double &grossFood) const{

	grossFood = ceil(grossFood * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetFoodCoef());

	double producedFood = grossFood;

	foodLossToCrime = CrimeLoss(producedFood);
	
	producedFood -= foodLossToCrime;

	return producedFood;
}
#endif

//----------------------------------------------------------------------------
//
// Name       : CityData::GetFoodRequiredPerCitizen
//
// Description: Gets the current rations for each person.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : double: The rations amount for each person.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::GetFoodRequiredPerCitizen() const
{
	return g_player[m_owner]->GetRationsPerPerson();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetFoodRequired
//
// Description: Gets the raw amount of food required for the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : double: The amount of food needed in this turn
//
// Remark(s)  : Slaves don't need rations.
//
//----------------------------------------------------------------------------
double CityData::GetFoodRequired() const
{
	return GetFoodRequired(PopCount() - SlaveCount());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetFoodRequired
//
// Description: Gets the raw amount of food required for the city.
//
// Parameters : popCount: Amount of pops for that the needed food 
//                        should be calculated.
//
// Globals    : -
//
// Returns    : double: The amount of food needed in this turn
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::GetFoodRequired(sint32 popCount) const
{
	return popCount * GetFoodRequiredPerCitizen();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::EatFood
//
// Description: Calculates the amount of food that is left and is consumed
//              this turn.
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
void CityData::EatFood()
{
	m_food_consumed_this_turn = GetFoodRequired();

	m_food_delta = m_net_food - m_food_consumed_this_turn;

	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);
	sint32 maxSurplusFood = rec->GetMaxSurplusFood();

	if(m_food_delta < 0) {
		if(m_food_delta < -maxSurplusFood)
			m_food_delta = -maxSurplusFood;
	}
	else{
		if(m_food_delta > maxSurplusFood)
			m_food_delta = maxSurplusFood;
	}

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

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateGrowthRate
//
// Description: Calculates the growth rate of the city.
//
// Parameters : -
//
// Globals    : g_theConstDB:   The constant database
//
// Returns    : -
//
// Remark(s)  : Use this function if the growth rate should be recalculated.
//
//              Food produced above maxSurplusFood is ignored; could be saved like pw, PFT
//
//----------------------------------------------------------------------------
void CityData::CalculateGrowthRate(){

	double baseRate;
	m_growth_rate = static_cast<sint32>(CalculateGrossGrowthRate(m_overcrowdingCoeff, baseRate));

	if(m_food_delta >= 0) {
		m_growth_rate = static_cast<sint32>(baseRate * m_growth_rate);
		if(m_growth_rate > g_theConstDB->GetMaxAbsoluteGrowthRate())
			m_growth_rate = g_theConstDB->GetMaxAbsoluteGrowthRate();

		if(m_population > g_theCitySizeDB->Get(g_theCitySizeDB->NumRecords() - 1)->GetPopulation())
			m_growth_rate = 0;

		if(m_is_rioting)
			m_growth_rate = 0;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateGrossGrowthRate
//
// Description: Calculates the growth rate of the city.
//
// Parameters : overcrowdingCoeff: Filled with overcrowding coefficient
//              baseRate:          Filled with the maximal growth rate
//              bonusFood:         Additional food for the calculation
//
// Globals    : g_theCitySizeDB:   The city size database
//
// Returns    : Returns the amount of fractional population added with the 
//              given food amount each turn.
//
// Remark(s)  : Use this method to estimate effects.
//              Effects like rioting, max. city size, abs. growth rate 
//              are not considered.
//
//----------------------------------------------------------------------------
double CityData::CalculateGrossGrowthRate(double &overcrowdingCoeff, double &baseRate, sint32 bonusFood)
{
	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);

	double maxGrowthRate = static_cast<double>(rec->GetGrowthRate());
	double minGrowthRate = static_cast<double>(rec->GetMinGrowthRate());
	sint32 maxSurplusFood = rec->GetMaxSurplusFood();

	double minPop;
	if(m_sizeIndex < 1){
		minPop = 0;
	}
	else{
		minPop = static_cast<double>(g_theCitySizeDB->Get(m_sizeIndex - 1)->GetPopulation()) + 1.0;
	}

	double overcrowding = GetOvercrowding(rec);
	sint32 maxPop = GetMaxPop(rec);
	
	if(PopCount() < overcrowding){
		baseRate = maxGrowthRate;
		overcrowdingCoeff = 1;
	} 
	else{
		double popRatio = static_cast<double>(PopCount() - overcrowding) / static_cast<double>(maxPop - overcrowding);
		overcrowdingCoeff = 1 - (popRatio * popRatio);
		baseRate = overcrowdingCoeff * maxGrowthRate;
	}

	if(m_food_delta + bonusFood < 0){

		double maxPop = static_cast<double>(rec->GetPopulation());
		double popScale = (static_cast<double>(PopCount()) - minPop) / (maxPop - minPop);
		double growthRate = maxGrowthRate - popScale * (maxGrowthRate - minGrowthRate);

		return(((m_food_delta + bonusFood) / static_cast<double>(maxSurplusFood)) * growthRate);

	}
	else{
		return((m_food_delta + bonusFood) / static_cast<double>(PopCount()));
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GrowOrStarve
//
// Description: Deal with city starvation and growth/shrinkage
//
// Parameters : -
//
// Globals    : g_gevManager
//
// Returns    : sint32 (TRUE or FALSE) TRUE if the city is either starving or changes pop size
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
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
				UpdateSprite();
				return TRUE;
			} 
			else m_partialPopulation -= k_PEOPLE_PER_POPULATION;

		}
		m_partialPopulation += m_growth_rate;

		if(m_partialPopulation >= k_PEOPLE_PER_POPULATION) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);
			m_partialPopulation -= k_PEOPLE_PER_POPULATION;
			UpdateSprite();
			return TRUE;
		} else if(m_partialPopulation < 0) {
			//PFT 05 apr 05: slaves starve first
			if(SlaveCount() > 0 )
				ChangeSpecialists(POP_SLAVE, -1);

			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillPop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);
			
			m_partialPopulation += k_PEOPLE_PER_POPULATION;
			UpdateSprite();
			return TRUE;
		}
	} else {
		m_partialPopulation += sint32(m_growth_rate);

		ResetStarvationTurns();

		if(m_partialPopulation >= k_PEOPLE_PER_POPULATION) {
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);
			
			m_partialPopulation -= k_PEOPLE_PER_POPULATION;
			UpdateSprite();
			return TRUE;
		}
	}
	UpdateSprite();
	return FALSE;
}

int CityData::FoodSupportTroops()
{
	return FALSE;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::UpdateSprite
//
// Description: Update this city's sprite
//
// Parameters : -
//
// Globals    : g_director
//
// Returns    : -
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateTradeRoutes
//
// Description: 
//
// Parameters : bool projectedOnly
//
// Globals    : 
//
// Returns    : sint32 m_goldFromTradeRoutes (via CalculateGoldFromResources() )
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
sint32 CityData::CalculateTradeRoutes(bool projectedOnly)
{
	sint32 i, n;
	ROUTE_TYPE routeType;
	sint32 routeResource;
	TradeDynamicArray deadRoutes;

	m_goldFromTradeRoutes = 0;
	m_goldLostToPiracy = 0;
	// outgoing goods:
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
	// incoming goods:
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

//----------------------------------------------------------------------------
//
// Name       : CityData::SupportBuildings
//
// Description: Pays the building support costs and sells buildings if
//              necessary.
//
// Parameters : projectedOnly: Whether costs should be paid or just be
//                             estimated.
//
// Globals    : -
//
// Returns    : The building upkeep costs
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::SupportBuildings(bool projectedOnly)
{

	sint32 buildingUpkeep = GetSupportBuildingsCost();
	m_net_gold -= buildingUpkeep;
	if(m_net_gold < 0 && !projectedOnly) {
		
		sint32 wonderLevel = wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);

		while((-m_net_gold > g_player[m_owner]->GetGold()) && (m_built_improvements != 0)) {
			sint32 cheapBuilding = buildingutil_GetCheapestBuilding(m_built_improvements, wonderLevel);
			Assert(cheapBuilding >= 0);
			if(cheapBuilding < 0)
				break;

			SellBuilding(cheapBuilding, FALSE);
			SlicObject *so = new SlicObject("029NoMaint");
			so->AddRecipient(GetOwner());
			so->AddCity(m_home_city);
			so->AddBuilding(cheapBuilding);
			g_slicEngine->Execute(so);
		}
		
		if(g_player[m_owner]->GetGold() < m_net_gold) {
#if defined(NEW_RESOURCE_PROCESS)
			m_science -= m_net_gold - g_player[m_owner]->GetGold(); // m_science originally generated from gold income, so remove it if gold isn't enough.
			if(m_science < 0) m_science = 0;
#endif
			g_player[m_owner]->m_gold->SetLevel(0);
		} else {
			g_player[m_owner]->m_gold->SubIncome(-m_net_gold);
		}
		m_net_gold = 0;
	}
	if(!projectedOnly)
		g_player[m_owner]->m_gold->AddMaintenance(buildingUpkeep);

	return buildingUpkeep;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetSupportBuildingsCost
//
// Description: Determine whether a city should revolt.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : The amount of gold to pay in this turn
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetSupportBuildingsCost() const
{
	sint32 wonderLevel = wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);
	
	return buildingutil_GetTotalUpkeep(m_built_improvements, wonderLevel);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalcWages
//
// Description: Calculates the wages needed for this turn.
//
// Parameters : wage: The amount of wages for one pop
//
// Globals    : -	
//
// Returns    : The amount of wages needed for this turn
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::CalcWages(sint32 wage) const
{
	return wage * (PopCount() - SlaveCount());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ShouldRevolt
//
// Description: Pays the wages.
//
// Parameters : wage:          The amount of wages for one pop
//              projectedOnly: Whether wages should be paid or only estimated.
//
// Globals    : g_player:      List of players	
//
// Returns    : Whether the player can afford to pay wages
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
BOOL CityData::PayWages(sint32 wage, bool projectedOnly)
{
	m_wages_paid = CalcWages(wage);
	if(!projectedOnly) {
		g_player[m_owner]->m_gold->AddWages(m_wages_paid);
	}
	m_net_gold -= m_wages_paid;
	if(m_net_gold < 0 && !projectedOnly) {
		if(-m_net_gold < g_player[m_owner]->GetGold()) {
			g_player[m_owner]->m_gold->SubIncome(-m_net_gold); 
		} else {
			g_player[m_owner]->m_gold->SubIncome(g_player[m_owner]->GetGold());
			return FALSE;
		}
	}


	return TRUE; 
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetWagesNeeded
//
// Description: Calculates the wages needed for this turn.
//
// Parameters : wages_per_person: The amount of wages for one pop
//
// Globals    : -	
//
// Returns    : The amount of wages needed for this turn
//
// Remark(s)  : see next method
//
//----------------------------------------------------------------------------
sint32 CityData::GetWagesNeeded(const sint32 & wages_per_person) const
{
	return wages_per_person * (PopCount() - SlaveCount());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetWagesNeeded
//
// Description: Calculates the wages needed for this turn.
//
// Parameters : -
//
// Globals    : g_player: List of players	
//
// Returns    : The amount of wages needed for this turn
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetWagesNeeded()
{
	return GetWagesNeeded(static_cast<sint32>(g_player[m_owner]->GetWagesPerPerson()));
}

//----------------------------------------------------------------------------
//
// Name       : AddTradeRoute
//
// Description: Add a trade route to this city's m_tradeSourceList or tradeDestinationList
//
// Parameters : TradeRoute &route       : the trade route
//            : BOOL       fromNetwork  :
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  :  enum ROUTE_TYPE {
//	                       ROUTE_TYPE_RESOURCE,   <= only this one implemented
//	                       ROUTE_TYPE_FOOD,
//	                       ROUTE_TYPE_GOLD,
//	                       ROUTE_TYPE_SLAVE, 
//                };
//
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
//
// Name       : DelTradeRoute
//
// Description: Delete a trade route from this city's m_tradeSourceList or tradeDestinationList
//
// Parameters : TradeRoute &route       : the trade route
//            : BOOL       fromNetwork  :
//
// Globals    : -
//
// Returns    : sint32
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
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

// CTP1
void CityData::CheckTopTen()
{ 
#if 0
	sint32	pos ;

	
	if (g_theTopTen->IsTopTenCity(m_home_city, TOPTENTYPE_BIGGEST_CITY, pos)) {
		m_currentlyTopTen = TRUE;
	} else {
		m_currentlyTopTen = FALSE;
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

// deal with capitalization/infrastructure. Otherwise, build the front item in this city's buildqueue.
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

//----------------------------------------------------------------------------
//
// Name       : CityData::BeginTurn
//
// Description: Begin this city's turn.
//
// Parameters : 
//
// Globals    : 
//
// Returns    : sint32 (TRUE or FALSE)
//
// Remark(s)  : 
//
//----------------------------------------------------------------------------
sint32 CityData::BeginTurn()
{
	InitBeginTurnVariables();
	DoTurnCounters(); // Decrement various turn counters, e.g., m_franchiseTurnsRemaining

	ImprovementHealUnitsInCity();   // Zeroed out. Could have, e.g., hospitals speeding up healing
	ImprovementRefuelUnitsInCity(); // Zeroed out. Could be removed.

	MapPoint pos;
	m_home_city.GetPos(pos); // See CityInfluenceIterator below

	m_pw_from_infrastructure = 0;
	m_gold_from_capitalization = 0;

#if defined(NEW_RESOURCE_PROCESS)
	PayResources();
	CalcPollution(); // Calculate the pollution produced by this city
	DoLocalPollution(); // Add dead tiles near polluting cities
	AddCapitalizationAndTryToBuild();
	DoSupport(false);
#else

	TryToBuild(); // Deal with capitalization/infrastructure. Otherwise, build the front item in this city's buildqueue.
	//TryToBuild must before capitalisation computation and after production computation

	CalcPollution(); // Calculate the pollution produced by this city
	//CalcPollution must be done after calculation of m_gross_production - No problem in the new system.
	DoLocalPollution(); // Add dead tiles near polluting cities

	ProcessFood(); // Modify m_gross_food by any applicable bonus and subtract
	               // m_food_lost_to_crime to get m_net_food

	DoSupport(false); // Why before split science and collect other trade?

	SplitScience(false);

	CollectOtherTrade(false);
//	DoSupport(false);
#endif

	// After new resource calculation - no problem
	EatFood(); // Calculate m_food_delta = m_net_food - m_food_consumed_this_turn 

	if (GrowOrStarve()) { // Deal with city starvation and growth/shrinkage
		if (PopCount() < 1) {
			SlicObject *so = new SlicObject("265CityDestroyedByStarving");
			so->AddRecipient(GetOwner());
			so->AddCity(m_home_city);
			g_slicEngine->Execute(so);

			return FALSE; 
		}
	}
	
	if (IsCelebratingHappiness()) {
		if(m_lastCelebrationMsg < 0 || (m_lastCelebrationMsg + 10 < g_turn->GetRound())) {
			SlicObject *so = new SlicObject("40CityIsCelebratingHappiness") ;
			so->AddCity(m_home_city);
			so->AddRecipient(m_owner);
			so->AddPlayer(m_owner);
			g_slicEngine->Execute(so);
			m_lastCelebrationMsg = sint16(g_turn->GetRound());
		}

		g_player[m_owner]->m_score->AddCelebration(); // Could use something more interesting here 
	}

	if (!m_build_queue.GetHead() && !m_buildCapitalization && !m_buildInfrastructure && !m_sentInefficientMessageAlready)
	{
		SlicObject *so = new SlicObject("37CityQueueIsEmpty");
		so->AddCity(m_home_city);
		so->AddCivilisation(m_owner);
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
		g_slicEngine->RunWastingWorkTriggers(m_home_city);
	}
	m_sentInefficientMessageAlready = FALSE;

	CheckForSlaveUprising();//Check that there's enough military units to guard the slaves.
	//Does the city have a terrainImprovement in it's radius? Used for pillage goal.
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
	/* ???
	if(isFirstPass) {
		sint32 wage = sint32(g_player[m_owner]->GetWagesPerPerson());
		int n;
		n = PopCount();
	}*/

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

	const UnitRecord* rec = g_theUnitDB->Get(type, g_player[m_owner]->GetGovernmentType());
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

	const BuildingRecord* rec = g_theBuildingDB->Get(type, g_player[m_owner]->GetGovernmentType());
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
//	const EndGameRecord *egrec = NULL; // Maybe usefull later
	uint32 oldCategory;
	if(m_build_queue.GetHead())
		oldCategory = m_build_queue.GetHead()->m_category;
	else
		oldCategory = -5;

	switch(category) { // see Globals.h for k_GAME_OBJ_TYPE enum
	case k_GAME_OBJ_TYPE_UNIT:
		urec = g_theUnitDB->Get(item_type, g_player[m_owner]->GetGovernmentType());
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
		irec = g_theBuildingDB->Get(item_type, g_player[m_owner]->GetGovernmentType());
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
#ifdef WIN32
#pragma warning (disable : 4127)
#endif
		Assert(0);
#ifdef WIN32
#pragma warning (default : 4127)
#endif
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

	// Recalculation of resources?

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

// not used (Hospitals ?)
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

//no function: buildingutil_GetMovementTypeIsRefueled
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
#ifdef WIN32
#pragma warning (disable : 4127)
#endif
			Assert(FALSE);
#ifdef WIN32
#pragma warning (default : 4127)
#endif
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

// called by TiledMap::DrawCityNames
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

//see ORDER_RESULT UnitData::NullifyWalls
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
		if(g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetCapitol()) {
			
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

//----------------------------------------------------------------------------
//
// Name       : CityData::DoUprising
//
// Description: Do a slave uprising in this city
//
// Parameters : UPRISING_CAUSE cause
//
// Globals    : 
//
// Returns    : 
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
void CityData::DoUprising(UPRISING_CAUSE cause)
{
	if(SlaveCount() < 1)
		return;

	
	if(g_slicEngine->GetTutorialActive() &&
	   m_owner == g_slicEngine->GetTutorialPlayer()) {
		return; 
	}

	CellUnitList slaveArmy;
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
	sint32 i;
	for(i = 0; i < numSlaves; i++) {
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

	CellUnitList defenders;

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
				
				UnitDynamicArray revealedUnits;
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
	Unit c;
	sint32 i, n = m_tradeSourceList.Num();
	for(i = 0; i < n; i++) {
		// FIXME: I believe that this is not having the intended effect because c reaches here by
		// value rather than by reference.  The previous code took the address of 
		// m_tradeSourceList[i].GetDestination() and used that, but since that generated a
		// "taking address of temporary" warning, I'm fairly sure it was the wrong thing to do,
		// and investigating the implmentation of m_tradeSourceList[i].GetDestination() increased my
		// worries.  I've made these changes as a temporary solution, but this needs to be further
		// investigated. - JJB 2005/07/02
		c = m_tradeSourceList[i].GetDestination();
		if ((c.IsBioImmune()) || (c.IsBioInfected()))
			continue;

		if(g_rand->Next(100) < sint32(g_theConstDB->BioInfectionSpreadChance()
			                          * 100.0)) {
			c.BioInfect(0);
			SlicObject *so = new SlicObject("047InfectedViaTrade");
			so->AddCity(c);
			so->AddRecipient(c.GetOwner());
			g_slicEngine->Execute(so);
		}
	}
}

void CityData::SpreadNanoTerror()
{
	Unit c;
	sint32 i, n = m_tradeSourceList.Num();
	for(i = 0; i < n; i++) {
		// FIXME: See comment in CityData::SpreadBioTerror above - same applies here.
		c = m_tradeSourceList[i].GetDestination();
		if ((c.IsNanoImmune()) || (c.IsNanoInfected()))
			continue;

		if(g_rand->Next(100) < sint32(g_theConstDB->NanoInfectionSpreadChance()
			                          * 100.0)) {
			c.NanoInfect(0);
			SlicObject *so = new SlicObject("047InfectedViaTrade");
			so->AddCity(c);
			so->AddRecipient(c.GetOwner());
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

//this city is collecting more sint32 resource than than it is selling
BOOL CityData::HasResource(sint32 resource) const
{
	return m_collectingResources[resource] > m_sellingResources[resource];
}

//Added by E -- this city is collecting or buying some sint32 resource 
BOOL CityData::HasNeededGood(sint32 resource) const
{
	return m_buyingResources[resource] + m_collectingResources[resource] == 0;
}

BOOL CityData::HasEitherGood(sint32 resource) const
{
	return m_buyingResources[resource] + m_collectingResources[resource] > 0;
}

//this city is collecting some sint32 resource
BOOL CityData::IsLocalResource(sint32 resource) const
{
	return m_collectingResources[resource] > 0;
}


//filters out non-resource trade
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

// filters out non-resource trade
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
	sint32 r;

	r = m_build_queue.Load(file);
	m_build_queue.SetOwner(m_owner);

	return (r) ;
}

sint32 CityData::SaveQueue(const MBCHAR *file)
{
	sint32 r;

	r = m_build_queue.Save(file);
	m_build_queue.SetOwner(m_owner);

	return (r) ;
}

uint32 CityData_CityData_GetVersion(void)
{
	return (k_CITYDATA_VERSION_MAJOR<<16 | k_CITYDATA_VERSION_MINOR);
}


void CityData::ResetCityOwner(sint32 owner)
{
	sint32	i, n;

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

	
	n = m_tradeDestinationList.Num();
	for(i = n-1; i >= 0; i--)
		m_tradeDestinationList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_CHANGED_OWNER);

	
	n = m_tradeSourceList.Num();
	for(i = n-1; i >= 0; i--)
		m_tradeSourceList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_CHANGED_OWNER);

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
	for(poptype = POP_WORKER + 1; poptype < POP_SLAVE; poptype++) {
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
	Assert(strlen(name)<k_MAX_NAME_LEN);
	strncpy(m_name, name, k_MAX_NAME_LEN);
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

	// * Can't rush buy capitalization/infrastructure
	if(m_buildInfrastructure || m_buildCapitalization)
		return FALSE;

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
		enable = g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->m_enable;
		obsolete = g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0))) {
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
			                          g_theStringDB->GetNameStr(g_theUnitDB->Get(i)->m_name)));
		}
	}
	
	DPRINTF(k_DBG_GAMESTATE, ("Improvements:\n"));
	n = g_theBuildingDB->NumRecords();
	for(i = 0; i < n; i++) {
		enable = g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->m_enable;
		obsolete = g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->m_obsolete;
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

// Used?
sint32 CityData::HowMuchLonger() const
{
	if(m_build_queue.GetLen() < 1)
		return 0x7fffffff;

	sint32 prod_remaining = m_build_queue.GetProductionRemaining(m_shieldstore);

	sint32 prod = GetNetCityProduction();

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

// Used?
sint32 CityData::HowMuchLonger(sint32 productionRemaining) const
{
	sint32 prod_remaining = productionRemaining;



	sint32 prod = GetNetCityProduction();

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
		sint32 gold = sint32(double(g_theBuildingDB->Get(which, g_player[m_owner]->GetGovernmentType())->GetProductionCost()) * 
			g_theConstDB->BuildingProductionToValueModifier());
		if(byChoice)
			g_player[m_owner]->m_gold->AddGold(gold);
		else
			m_net_gold += gold;
		m_built_improvements &= ~((uint64)1 << uint64(which));

		g_player[m_owner]->RegisterLostBuilding(m_home_city, which);
		m_build_queue.RemoveIllegalItems(TRUE);
		if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)which))) {
			Assert(g_player[m_owner]->m_capitol->m_id == m_home_city.m_id);
			if(g_player[m_owner]->m_capitol->m_id == m_home_city.m_id) {
				g_player[m_owner]->m_capitol->m_id = 0;
			}
		}

		// Selling a building may impact the defensive bonus 
		buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus);

		if(byChoice && g_network.IsHost()) {
			g_network.Unblock(m_owner);
		}
	}


}

void CityData::SetRoad() const
{
	MapPoint    pos     (m_home_city.RetPos());
	Cell *      cell    = g_theWorld->GetCell(pos);

	TerrainImprovementRecord const *
                rec     = terrainutil_GetBestRoad(m_owner, pos);
	if (rec) 
    {
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

// CTP1 trade: see Player::HasSameGoodAsTraded 
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

// not used
// how many combat (Attack > 0) units are there in m_home_city
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

//----------------------------------------------------------------------------
//
// Name       : CityData::CanBuildUnit
//
// Description: Checks whether the city can build the unit specified by type.
//
// Parameters : type: The unit type for that is checked whether the city can 
//              build it.
//
// Globals    : g_player:     The list of players
//              g_theUnitDB:  The unit database
//              g_slicEngine: The slic engine
//              g_theWorld:   The world properties
//
// Returns    : Whether the city can build the unit specified by type.
//
// Remark(s)  : Added CityStyleOnly check to build units in specific
//              CityStyle types.
//		      : Added PrerequisiteBuilding check to see if a city has a building
//              needed to build a unit.
//		      : Added NeedsCityGood checks unit good and sees if the city 
//			    has the either good in its radius or is buying the good from trade.
//		      : Added NeedsCityGoodAll checks unit good and sees if the city 
//			    has all of the goods in its radius or is buying the good from trade.
//
//----------------------------------------------------------------------------
BOOL CityData::CanBuildUnit(sint32 type) const
{
// Added by Martin Gühmann
	if(!g_player[m_owner]->CanBuildUnit(type))
		return FALSE;

	const UnitRecord *rec = g_theUnitDB->Get(type, g_player[m_owner]->GetGovernmentType());
	if(!rec)
		return FALSE;

	MapPoint pos;
	m_home_city.GetPos(pos);

// Added by E - checks if a city has a building required to build the unit
	if(rec->GetNumPrerequisiteBuilding() > 0) {
		sint32 o;
		for(o = 0; o < rec->GetNumPrerequisiteBuilding(); o++) {
			sint32 b = rec->GetPrerequisiteBuildingIndex(o);
			if(!(GetEffectiveBuildings() & ((uint64)1 << (uint64)b)))
				return FALSE;
		}
	}
// Added by E - Compares Unit CityStyle to the CityStyle of the City
	if(rec->GetNumCityStyleOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCityStyleOnly(); s++) {
			if(rec->GetCityStyleOnlyIndex(s) == m_cityStyle) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

//  Start Resources section - more to add later 
//  Added by E - Compares Unit NeedsCityGood to the resources collected our bought by the city, can be either/or

	if(rec->GetNumNeedsCityGood() > 0) {
		sint32 g;
		bool found = false;
		for(g = 0; g < rec->GetNumNeedsCityGood(); g++) {
			if((m_buyingResources[rec->GetNeedsCityGoodIndex(g)] + m_collectingResources[rec->GetNeedsCityGoodIndex(g)]) > 0){
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}
//  Added by E - Compares Unit NeedsCityGoodAll to the resources collected our bought by the city, must be all listed


	if(rec->GetNumNeedsCityGoodAll() > 0) {
		sint32 g;
		for(g = 0; g < rec->GetNumNeedsCityGoodAll(); g++) {
			if((m_buyingResources[rec->GetNeedsCityGoodAllIndex(g)] + m_collectingResources[rec->GetNeedsCityGoodAllIndex(g)]) == 0)
			return FALSE;
		}
	}

//End Resources Code


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

//----------------------------------------------------------------------------
//
// Name       : CityData::CanBuildBuilding
//
// Description: Checks whether the city can build the building specified by 
//              type.
//
// Parameters : type: The building type for that is checked whether the city  
//              can build it.
//
// Globals    : g_player:        The list of players
//              g_theBuildingDB: The building database
//              g_slicEngine:    The slic engine
//              g_theWorld:      The world properties
//
// Returns    : Whether the city can build the building specified by type.
//
// Remark(s)  : CityStyleOnly added by E. Limits certain buildings to be built  
//              only at certain cities of certain styles.
//              GovernmentType flag for Buidings limits Buildings to govt type.
//              CultureOnly flag added by E. It allows only civilizations with 
//              the same CityStyle as CultureOnly's style to build that building.
//		      : Added NeedsCityGood checks building good and sees if the city 
//			    has the either good in its radius or is buying the good from trade.
//		      : Added NeedsCityGoodAll checks building good and sees if the city 
//			    has all of the goods in its radius or is buying the good from trade.
//
//----------------------------------------------------------------------------
BOOL CityData::CanBuildBuilding(sint32 type) const
{
	if(g_exclusions->IsBuildingExcluded(type))
		return FALSE;

	const BuildingRecord* rec = g_theBuildingDB->Get(type, g_player[m_owner]->GetGovernmentType());
	
	
	Assert(rec != NULL);
	if (!rec) 
		return FALSE;

	if(!g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex()) && rec->GetEnableAdvanceIndex() >= 0) {
		return FALSE;
	}

	sint32 o;
	for(o = 0; o < rec->GetNumObsoleteAdvance(); o++) {
		if(g_player[m_owner]->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
			return FALSE;
	}
	
	MapPoint pos;
	m_home_city.GetPos(pos);
	if(g_theWorld->IsWater(pos)) {
		if(rec->GetCantBuildInSea())
			return FALSE;
	} else {
		if(rec->GetCantBuildOnLand())
			return FALSE;
	}
	
	
	if (rec->GetCoastalBuilding()) {
		if(!g_theWorld->IsNextToWater(pos.x, pos.y))
			return FALSE;
	}

	if(m_built_improvements & uint64((uint64)1 << (uint64)type)) {
		
		return FALSE;
	}

	if((rec->GetNuclearPlant() &&
	   wonderutil_GetNukesEliminated(g_theWonderTracker->GetBuiltWonders()))) {
		return FALSE;
	}

	
	if(rec->GetNumPrerequisiteBuilding() > 0) {
		for(o = 0; o < rec->GetNumPrerequisiteBuilding(); o++) {
			sint32 b = rec->GetPrerequisiteBuildingIndex(o);
			if(!(GetEffectiveBuildings() & ((uint64)1 << (uint64)b)))
				return FALSE;
		}
	}

// Added GovernmentType flag from Units to use for Buildings
	if(rec->GetNumGovernmentType() > 0) {
		sint32 i;
		bool found = false;
		for(i = 0; i < rec->GetNumGovernmentType(); i++) {
			if(rec->GetGovernmentTypeIndex(i) == g_player[m_owner]->GetGovernmentType()) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

// Added by E - Compares Building CityStyle to the CityStyle of the City
	if(rec->GetNumCityStyleOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCityStyleOnly(); s++) {
			if(rec->GetCityStyleOnlyIndex(s) == m_cityStyle) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

// Added by E - Compares Building CultureOnly to the Player's CityStyle
	if(rec->GetNumCultureOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCultureOnly(); s++) {
			if(rec->GetCultureOnlyIndex(s) == g_player[m_owner]->GetCivilisation()->GetCityStyle()) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}
//  Start Resources section - more to add later 
//  Added by E - Compares Building NeedsCityGood to the resources collected our bought by the city, can be either/or

	if(rec->GetNumNeedsCityGood() > 0) {
		sint32 g;
		bool found = false;
		for(g = 0; g < rec->GetNumNeedsCityGood(); g++) {
			if((m_buyingResources[rec->GetNeedsCityGoodIndex(g)] + m_collectingResources[rec->GetNeedsCityGoodIndex(g)]) > 0){
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

//  Added by E - Compares Building NeedsCityGoodAll to the resources collected our bought by the city, must be all listed

	if(rec->GetNumNeedsCityGoodAll() > 0) {
		sint32 g;
		for(g = 0; g < rec->GetNumNeedsCityGoodAll(); g++) {
			if((m_buyingResources[rec->GetNeedsCityGoodAllIndex(g)] + m_collectingResources[rec->GetNeedsCityGoodAllIndex(g)]) == 0)
			return FALSE;
		}
	}

//End Resources Code


	return g_slicEngine->CallMod(mod_CanCityBuildBuilding, TRUE, m_home_city.m_id, rec->GetIndex());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CanBuildWonder
//
// Description: Checks whether the city can build the Wonder specified by 
//              type.
//
// Parameters : type: The Wonder type for that is checked whether the city  
//              can build it.
//
// Globals    : g_player:        The list of players
//              g_theWonderDB:   The building database
//              g_slicEngine:    The slic engine
//              g_theWorld:      The world properties
//
// Returns    : Whether the city can build the wonder specified by type.
//
// Remark(s)  : CityStyleOnly added by E. Limits certain wonders to be built  
//              only at certain cities of certain styles.
//            : GovernmentType flag for wonders limits wonders to govt type.
//            : CultureOnly flag added by E. It allows only civilizations with 
//              the same CityStyle as CultureOnly's style to build that wonder.
//            : PrerequisiteBuilding checks if a city has a building in order
//              to build a wonder. Added by E.
//		      : Added NeedsCityGood checks wonder good and sees if the city 
//			    has the either good in its radius or is buying the good from trade.
//		      : Added NeedsCityGoodAll checks wonder good and sees if the city 
//			    has all of the goods in its radius or is buying the good from trade.
//
//----------------------------------------------------------------------------
BOOL CityData::CanBuildWonder(sint32 type) const
{
	if(g_exclusions->IsWonderExcluded(type))
		return FALSE;

	if(!wonderutil_IsAvailable(type, m_owner))
		return FALSE;


// Added Wonder database 
	const WonderRecord* rec = wonderutil_Get(type);

// Added PrerequisiteBuilding checks if city has building to build wonder 
	if(rec->GetNumPrerequisiteBuilding() > 0) {
		sint32 o;
		for(o = 0; o < rec->GetNumPrerequisiteBuilding(); o++) {
			sint32 b = rec->GetPrerequisiteBuildingIndex(o);
			if(!(GetEffectiveBuildings() & ((uint64)1 << (uint64)b)))
				return FALSE;
		}
	}
	
// Added GovernmentType flag from Units to use for Wonders
	if(rec->GetNumGovernmentType() > 0) {
		sint32 i;
		bool found = false;
		for(i = 0; i < rec->GetNumGovernmentType(); i++) {
			if(rec->GetGovernmentTypeIndex(i) == g_player[m_owner]->GetGovernmentType()) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

// Added by E - Compares Wonder CityStyle to the CityStyle of the City
	if(rec->GetNumCityStyleOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCityStyleOnly(); s++) {
			if(rec->GetCityStyleOnlyIndex(s) == m_cityStyle) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

// Added by E - Compares Wonder CultureOnly to the Player's CityStyle
	if(rec->GetNumCultureOnly() > 0) {
		sint32 s;
		bool found = false;
		for(s = 0; s < rec->GetNumCultureOnly(); s++) {
			if(rec->GetCultureOnlyIndex(s) == g_player[m_owner]->GetCivilisation()->GetCityStyle()) {
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}
//  Start Resources section - more to add later 
//  Added by E - Compares Unit NeedsCityGood to the resources collected our bought by the city, can be either/or

	if(rec->GetNumNeedsCityGood() > 0) {
		sint32 g;
		bool found = false;
		for(g = 0; g < rec->GetNumNeedsCityGood(); g++) {
			if((m_buyingResources[rec->GetNeedsCityGoodIndex(g)] + m_collectingResources[rec->GetNeedsCityGoodIndex(g)]) > 0){
				found = true;
				break;
			}
		}
		if(!found)
			return FALSE;
	}

//  Added by E - Compares Wonder NeedsCityGoodAll to the resources collected our bought by the city, must be all listed

	if(rec->GetNumNeedsCityGoodAll() > 0) {
		sint32 g;
		for(g = 0; g < rec->GetNumNeedsCityGoodAll(); g++) {
			if((m_buyingResources[rec->GetNeedsCityGoodAllIndex(g)] + m_collectingResources[rec->GetNeedsCityGoodAllIndex(g)]) == 0)
			return FALSE;
		}
	}

//End Resources Code

	return g_slicEngine->CallMod(mod_CanCityBuildWonder, TRUE, m_home_city.m_id, type);
}
// no longer used
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

/*sint32 CityData::GetPreferedPopType() const
{
	return 0;
}*/

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
			const BuildingRecord *irec = g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType());
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

//not used; see Player::BeginTurnScience()
void CityData::ContributeScience(double incomePercent,
                                 double &addscience,
                                 double &subgold)
{
	Assert(FALSE);

	double goldToConsider = m_net_gold * incomePercent;
	Player *p = g_player[m_owner];
	double scienceRate;
	p->m_tax_rate->GetScienceTaxRate(scienceRate);
	
	addscience = goldToConsider * scienceRate;
	subgold = addscience;

	double s;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), s);
	addscience += addscience * s;
	
}

//----------------------------------------------------------------------------
//
// Name       : CityData:TurnsToNextPop
//
// Description: Calculates the number of turns until this city adds a pop
//
// Parameters : -
//
// Globals    : -
//
// Returns    : the number of turns until this city adds a pop
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::TurnsToNextPop( void )
{

	if(m_food_delta <= 0
	|| m_growth_rate <= 0
	|| m_is_rioting
	){
		return 999;
	}

	return static_cast<sint32>(ceil((static_cast<double>(k_PEOPLE_PER_POPULATION - m_partialPopulation)) / static_cast<double>(m_growth_rate)));
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

sint32 CityData::GetHappinessFromPops()
{
	sint32 happy = 0;

	if(m_specialistDBIndex[POP_ENTERTAINER] >= 0) {
		happy += sint32(EntertainerCount() *
		                g_thePopDB->Get(m_specialistDBIndex[POP_ENTERTAINER], g_player[m_owner]->GetGovernmentType())->GetHappiness());
	}
	return happy;
}

#if !defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : CityData::GetScienceFromPops
//
// Description: Calculates the amount of science that is generated from 
//              specialists.
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
sint32 CityData::GetScienceFromPops(bool considerOnlyFromTerrain) const
{
	double sci = 0.0;

	double popWonderModifier = static_cast<double>(wonderutil_GetIncreaseScientists(m_builtWonders));
	popWonderModifier += static_cast<double>(wonderutil_GetIncreaseSpecialists(g_player[m_owner]->m_builtWonders));
	
	if(popWonderModifier && m_specialistDBIndex[POP_SCIENTIST] >= 0 
	&& m_specialistDBIndex[POP_SCIENTIST] < g_thePopDB->NumRecords()
	){
		sci += popWonderModifier * 
		             g_thePopDB->Get(m_specialistDBIndex[POP_SCIENTIST], g_player[m_owner]->GetGovernmentType())->GetScience();
	}

	sci += buildingutil_GetIncreaseSciencePerPop(GetEffectiveBuildings()) * static_cast<double>(PopCount() - SlaveCount());

	double p;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), p);
	sci += sci * p;
	sci = sci * g_player[m_home_city.GetOwner()]->GetKnowledgeCoef();
	sci += sci * static_cast<double>(wonderutil_GetIncreaseKnowledgePercentage(g_player[m_home_city.GetOwner()]->GetBuiltWonders())) / 100.0;
	
//Added by E - EXPORT BONUSES TO GOODS This gives a bonus of food	
	sint32 good;
	for(good = 0; good < g_theResourceDB->NumRecords(); good++) {
		if((m_buyingResources[good] + m_collectingResources[good] - m_sellingResources[good]) > 0){
			sci += ceil(sci * (g_theResourceDB->Get(good))->GetSciencePercent());		
		}
	}

//Added by E - EXPORT BONUSES TO GOODS This Causes a crime effect if negative and efficiency if positive	
	sint32 g;
	for(g = 0; g < g_theResourceDB->NumRecords(); g++) {
		if((m_buyingResources[g] + m_collectingResources[g] - m_sellingResources[g]) > 0){
			sci += ceil(sci * (g_theResourceDB->Get(g))->GetEfficiencyOrCrime());		
		}
	}
	
	
	if(!considerOnlyFromTerrain
	&& m_specialistDBIndex[POP_SCIENTIST] >= 0 
	&& m_specialistDBIndex[POP_SCIENTIST] < g_thePopDB->NumRecords()) {
		sci += ScientistCount() * 
					  g_thePopDB->Get(m_specialistDBIndex[POP_SCIENTIST], g_player[m_owner]->GetGovernmentType())->GetScience();
	}

	return static_cast<sint32>(sci);
}
#endif

sint32 CityData::GetNumPop() const
{
	sint32 n = PopCount();

	if (n < 1) {
		return 1;
	} else {
		return n;
	}
}

//how many trade routes come into this city
sint32 CityData::GetIncomingTrade() const
{
	return m_tradeDestinationList.Num();
}

//how many trade routes go out of this city
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
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_INFRASTRUCTURE, m_home_city.m_id));
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
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_CAPITALIZATION, m_home_city.m_id));
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
		if(g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetSettleLand() && 
		   (g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos)) &&
		   !g_theWorld->IsWater(pos) && !g_theWorld->IsShallowWater(pos)) {
			settler = i;
			break;
		} else if(g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetSettleWater() && g_theWorld->IsWater(pos)) {
			settler = i;
			break;
		} else if(g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetSettleSpace() && g_theWorld->IsSpace(pos)) {
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
			//possible solution for bug #14
			if (g_network.IsHost()) 
			{
				g_network.Block(s.GetOwner());
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_DISBANDED_CITY_SETTLER, s.m_id));
				g_network.Unblock(s.GetOwner());
			}
		}
	}

	ChangePopulation(-PopCount());
	// The ChangePopulation(everyone) call will trigger a KillCity event.
	// m_home_city.Kill(CAUSE_REMOVE_ARMY_DISBANDED, -1) will make the city invalid and may
	// crash the KillCity execution.
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

	
	production += m_net_production;

	
	
	production += m_net_gold;

	
	
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

#if defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeSizeIndexes
//
// Description: Adjusts the city size indices and makes the city radius grow
//              or shrink if necessary.
//
// Parameters : -
//
// Globals    : g_theCitySizeDB: Computes the city size index.
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::ComputeSizeIndexes(const sint32 & workers, sint32 & size_index) const
{
	size_index = -1;

	sint32 sizeIndex;
	for(sizeIndex = 0; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++) {
		const CitySizeRecord *rec = g_theCitySizeDB->Get(sizeIndex);
		if(size_index < 0 && PopCount() <= rec->GetPopulation()) {
			size_index = sizeIndex;
		}
	}

	if(size_index < 0) {
		size_index = g_theCitySizeDB->NumRecords() - 1;
	}
}
#else

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
#endif

sint32 CityData::GetBestSpecialist(const POP_TYPE & type) const
{
	Assert(type > 0);
	Assert(type < POP_SLAVE); 
	return m_specialistDBIndex[type];
}

//----------------------------------------------------------------------------
//
// Name       : Governor::AdjustSizeIndices
//
// Description: Adjusts the city size indices and makes the city radius grow
//              or shrink if necessary.
//
// Parameters : -
//
// Globals    : g_gevManager: The slic game event manager.
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::AdjustSizeIndices()
{
	double oldVision = -1;

	if(m_home_city->Flag(k_UDF_VISION_ADDED)) {
		Assert(m_sizeIndex >= 0);
		oldVision = GetVisionRadius();
	}

	sint32 oldSizeIndex = m_sizeIndex;
#if defined(NEW_RESOURCE_PROCESS)
	ComputeSizeIndexes(WorkerCount(), 
	                   m_sizeIndex);
#else
	ComputeSizeIndexes(WorkerCount(), 
	                   m_sizeIndex,
	                   m_workerFullUtilizationIndex,
	                   m_workerPartialUtilizationIndex);
#endif
		
	
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
		const PopRecord *rec = g_thePopDB->Get(dbindex, g_player[m_owner]->GetGovernmentType());

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
	if(index == 0 
	&& category != k_GAME_OBJ_TYPE_CAPITALIZATION
	&& category != k_GAME_OBJ_TYPE_INFRASTRUCTURE
	){
		StopInfrastructureCapitalization();
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
	Player *		player	= g_player[m_owner];
	sint32 const	cost	= GetOvertimeCost();

	if ((cost <= 0) || (player->GetGold() < cost))
	{
		return false;
	}

	player->SubGold(cost);
	player->m_gold->AddLostToRushBuy(cost);

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

//	g_theWorld->FindDistances(m_owner, m_home_city.RetPos(), goodsToFind,
//	                          FindGoodDistancesCallback, this);
	g_theWorld->FindDistances(m_owner, m_pos, goodsToFind,
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

	// Added DWT
	// We want to retreive the underlying terrain type
	// not the terrain type as modified by improvements
	// as a sea city on a tunnel will turn into a land city
	const TerrainRecord *rec = g_theTerrainDB->Get(g_theWorld->GetTerrainType(m_pos));
	bool isLand = justTryLand || !(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater());

	// 
	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());

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

sint32 CityData::GetSupport() const
{
	Assert(g_player[m_owner]);
	if (g_player[m_owner] == NULL)
		return 0;

	sint32 support = CalcWages(static_cast<sint32>(g_player[m_owner]->GetWagesPerPerson()));
	support += GetSupportBuildingsCost();

	return support;
}

#if !defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : CityData::SplitScience
//
// Description: Calculates the player's amount of generated science.
//
// Parameters : projectedOnly:           Has no effect here.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Use this function for science recalculation.
//
//----------------------------------------------------------------------------
void CityData::SplitScience(bool projectedOnly)
{
	SplitScience(projectedOnly, m_net_gold, m_science);
}

//----------------------------------------------------------------------------
//
// Name       : CityData::SplitScience
//
// Description: Calculates the player's amount of generated science.
//
// Parameters : projectedOnly:           Has no effect here.
//              gold:                    Amount of gross gold.
//              science:                 Filled with the amount of science
//                                       generated here.
//              considerOnlyFromTerrain: Whether scientists should be considered.
//
// Globals    : g_player:            List of players
//
// Returns    : -
//
// Remark(s)  : gold is modified by this method.
//              Use this function for estimations.
//
//----------------------------------------------------------------------------
void CityData::SplitScience(bool projectedOnly, sint32 &gold, sint32 &science, bool considerOnlyFromTerrain) const
{
	if (g_player[m_owner]==NULL)
		return;

	sint32 baseGold = gold;
	if(baseGold <= 0) {
		baseGold = 0;
	}

	double s, ws;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(s);
	science = static_cast<sint32>(ceil(baseGold * s));
	gold -= science;

	
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), s);
	science += static_cast<sint32>(ceil(science * s));

	ws = 0.01 * wonderutil_GetIncreaseKnowledgePercentage(g_player[m_owner]->GetBuiltWonders());
	science += static_cast<sint32>(ceil(science * ws));

	science += GetScienceFromPops(considerOnlyFromTerrain);
	
	science -= CrimeLoss(science);
	science = static_cast<sint32>(ceil(science * g_player[m_owner]->GetKnowledgeCoef()));

//	Just clutters debug log, if you need this kind of information uncomment this.
//	DPRINTF(k_DBG_GAMESTATE, ("SplitScience: %lx: %d, %lf, %lf, %d, %lf, %lf\n", m_home_city.m_id, science,
//	                          g_player[m_owner]->GetKnowledgeCoef(),
//	                          m_happy->GetCrime(),
//	                          GetScienceFromPops(considerOnlyFromTerrain),
//	                          ws, s));

}

//----------------------------------------------------------------------------
//
// Name       : CityData::CollectOtherTrade
//
// Description: Calculate this city's m_net_gold and maybe transfer it to the owner's gold reserve.
//
// Parameters : BOOL projectedOnly     : if FALSE then actually transfer gold
//            : BOOL changeResources   : if TRUE (default) then redo trade routes, else just calculate gold
//
// Globals    : g_player:            List of players
//
// Returns    : -
//
// Remark(s)  : Use this method if you want to change the gold production.
//
//----------------------------------------------------------------------------
void CityData::CollectOtherTrade(const bool projectedOnly, bool changeResources)
{
	ProcessGold(m_net_gold);
	
	if(changeResources) {
		m_net_gold += CalculateTradeRoutes(projectedOnly);
	} else {
		m_goldFromTradeRoutes = 0;
		m_goldLostToPiracy = 0;
		m_net_gold += CalculateGoldFromResources();
	}

	ApplyGoldCoeff(m_net_gold);

	m_net_gold += m_gold_from_capitalization;

	CalcGoldLoss(projectedOnly, m_net_gold, m_convertedGold, m_gold_lost_to_crime);

	if(!projectedOnly) {
		g_player[m_owner]->m_gold->AddIncome(m_net_gold);
	}	

}

//----------------------------------------------------------------------------
//
// Name       : CityData::CollectGold
//
// Description: Applys the commerce boni of buildings and feats. 
//
// Parameters : gold:                   Amount of gross gold.
//              convertedGold:           Filled with the amount of gold lost 
//                                       to conversion.
//              crimeLost:               Filled with the amount of gold lost 
//                                       to crime.
//              considerOnlyFromTerrain: Whether merchants should be considered.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : gold is modified.
//              Use this method if you want to estimate the amount of gold
//              that is produced.
//
//----------------------------------------------------------------------------
void CityData::CollectGold(sint32 &gold, sint32 &convertedGold, sint32 &crimeLost, bool considerOnlyFromTerrain) const
{
	ProcessGold(gold, considerOnlyFromTerrain);
	ApplyGoldCoeff(gold);
	CalcGoldLoss(true, gold, convertedGold, crimeLost);

}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessGold
//
// Description: Applys the commerce boni of buildings and feats. 
//
// Parameters : gold:                   Amount of gross gold.
//              considerOnlyFromTerrain: Whether merchants should be considered.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : gold is modified.
//
//----------------------------------------------------------------------------
void CityData::ProcessGold(sint32 &gold, bool considerOnlyFromTerrain) const
{
	if(gold > 0) {
		
		double goldBonus;
		buildingutil_GetCommercePercent(GetEffectiveBuildings(), goldBonus, m_owner);
		gold += static_cast<sint32>(gold * goldBonus);

		sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_COMMERCE, m_owner);
		gold += static_cast<sint32>(gold * featPercent / 100.0);
	}

	sint32 goldPerCitizen = buildingutil_GetGoldPerCitizen(GetEffectiveBuildings());
	gold += goldPerCitizen * PopCount();

	if(!considerOnlyFromTerrain && m_specialistDBIndex[POP_MERCHANT] >= 0) {
		gold += MerchantCount() *
			g_thePopDB->Get(m_specialistDBIndex[POP_MERCHANT], g_player[m_owner]->GetGovernmentType())->GetCommerce();
	}
//Added by E - EXPORT BONUSES TO GOODS This gives a bonus of gold	
	sint32 good;
	for(good = 0; good < g_theResourceDB->NumRecords(); good++) {
		if((m_buyingResources[good] + m_collectingResources[good] - m_sellingResources[good]) > 0){
			gold += ceil(gold * (g_theResourceDB->Get(good))->GetCommercePercent());		
		}
	}

//Added by E - EXPORT BONUSES TO GOODS This Causes a crime effect if negative and efficiency if positive	
	sint32 g;
	for(g = 0; g < g_theResourceDB->NumRecords(); g++) {
		if((m_buyingResources[g] + m_collectingResources[g] - m_sellingResources[g]) > 0){
			gold += ceil(gold * (g_theResourceDB->Get(g))->GetEfficiencyOrCrime());		
		}
	}


}

//----------------------------------------------------------------------------
//
// Name       : CityData::ApplyGoldCoeff
//
// Description: Modifys the given amount of gold by the player's government
//              gold coeffiecient.
//
// Parameters : gold:         Amount of gross gold.
//
// Globals    : g_player:            List of players
//              g_theGovernmentDB:   The government databse
//
// Returns    : -
//
// Remark(s)  : gold is modified by this method.
//
//----------------------------------------------------------------------------
void CityData::ApplyGoldCoeff(sint32 &gold) const
{
	if(gold > 0) {
		gold = static_cast<sint32>(gold * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetGoldCoef());
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalcGoldLoss
//
// Description: Gets the player's index of the founder of the city.
//
// Parameters : projectedOnly: Whether the conversion owner should be paid.
//              gold:         Amount of gross gold.
//              convertedGold: Filled with the amount of gold lost to conversion.
//              crimeLost:     Filled with the amount of gold lost to crime.
//
// Globals    : g_player:            List of players
//              g_theConstDB:        The constant databse
//
// Returns    : -
//
// Remark(s)  : Amount of loss is deducted from gold. 
//              Use projectedOnly if you just want to check the values.
//
//----------------------------------------------------------------------------
void CityData::CalcGoldLoss(const bool projectedOnly, sint32 &gold, sint32 &convertedGold, sint32 &crimeLost) const
{
	convertedGold = 0;

	if(m_convertedTo >= 0) {
		if(m_convertedBy == CONVERTED_BY_CLERIC) {
			convertedGold = static_cast<sint32>(gold * g_theConstDB->ClericConversionFactor());
		} else if(m_convertedBy == CONVERTED_BY_TELEVANGELIST) {
			sint32 doubleTelevangelists;
			buildingutil_GetDoubleTelevangelism(GetEffectiveBuildings(), doubleTelevangelists);
			if(doubleTelevangelists) {
				convertedGold = static_cast<sint32>(gold * g_theConstDB->TelevangelistConversionFactor());
			} else {
				convertedGold = static_cast<sint32>(gold * g_theConstDB->ClericConversionFactor());
			}
		} else {
			Assert(FALSE);
		}
		if(convertedGold < 1)
			convertedGold = 1;

		if (g_player[m_convertedTo]) {
			sint32 wonderIncrease = wonderutil_GetIncreaseConvertedCitiesFeePercentage(g_player[m_convertedTo]->m_builtWonders);
			if(wonderIncrease > 0) {
				convertedGold += (convertedGold * wonderIncrease) / 100;
			}
		}

		gold -= convertedGold;

		if(!projectedOnly) {
			DPRINTF(k_DBG_GAMESTATE, ("Sending %d gold to player %d due to conversion\n",
			                          convertedGold, m_convertedTo));
			if (g_player[m_convertedTo]) {
				g_player[m_convertedTo]->AddGold(convertedGold);
			}
		}
	}
	crimeLost = CrimeLoss(gold);
	gold -= crimeLost;

}
#endif

//----------------------------------------------------------------------------
//
// Name       : CityData::GetProjectedScience
//
// Description: Estimates the amount of science that is created next turn.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Amount of science that might be creted next turn.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetProjectedScience()
{
#if defined(NEW_RESOURCE_PROCESS)
	///////////////////////////////////////////////
	// Unsplit science and gold
	double scienceReturn = static_cast<double>(m_max_gold_from_terrain + m_max_scie_from_terrain);

	///////////////////////////////////////////////
	// Resplit science from gold
	double s;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(s);
	scienceReturn *= s;

	///////////////////////////////////////////////
	// Process science
	scienceReturn = ProcessScie(static_cast<sint32>(scienceReturn));
	ApplyKnowledgeCoeff(m_max_processed_terrain_scie);
	scienceReturn -= CrimeLoss(scienceReturn);
	m_scieFromOnePop; // Recalc on government change.

	///////////////////////////////////////////////
	// Get utilization fractions
	double foodFraction = 0;
	double prodFraction = 0;
	double goldFraction = 0;
	ResourceFractions(foodFraction, prodFraction, goldFraction, WorkerCount() + SlaveCount());
	scienceReturn += ceil((m_scieFromOnePop - m_crimeScieLossOfOnePop) * ScientistCount());

	return static_cast<sint32>(scienceReturn);
#else
	// Bad design needs to be redesigned
	double grossFood = m_gross_food;
	sint32 collectedProduction = m_collected_production_this_turn;
	sint32 grossGold = m_gross_gold;
	double foodThisTurn = m_net_food;
	sint32 gold = m_net_gold;
	sint32 science = m_science;
	sint32 wagesPaid = m_wages_paid;

	CollectResources();
	DoSupport(true);
	SplitScience(true);

	sint32 scienceReturn = m_science;

	
	m_gross_food = grossFood;
	m_collected_production_this_turn = collectedProduction;
	m_gross_gold = grossGold;
	m_net_food = foodThisTurn;
	m_net_gold = gold;
	m_science = science;
	m_wages_paid = wagesPaid;

	return scienceReturn;
#endif
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetFounder
//
// Description: Gets the index of the player who founded the city.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : The index of the player who founded the city
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetFounder() const
{
	return m_founder;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::NeedMoreFood
//
// Description: Checks whether a new food terrain improvement should be 
//              built in the area of the according city.
//
// Parameters : bonusFood:               Additional food that should be 
//                                       taken into the consideration
//              foodMissing:             Filled with the amount of 
//                                       missing food during calculation
//              considerOnlyFromTerrain: Whether farmers should be considered.
//              
//
// Globals    : g_theCitySizeDB          The city size database
//              
//
// Returns    : Whether a new food terrain improvement should be built 
//              in the area of the city.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool CityData::NeedMoreFood(sint32 bonusFood, sint32 &foodMissing, bool considerOnlyFromTerrain){

	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);
	sint32 maxPop = GetMaxPop(rec);

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

	sint32 popDistance;
	strategy.GetStopBuildingFoodBeforePopMax(popDistance);

	foodMissing = HowMuchMoreFoodNeeded(bonusFood, true, considerOnlyFromTerrain);

	return(foodMissing > 0
	&&    (PopCount() < maxPop - popDistance
	||     m_food_delta + bonusFood < 0));

}

//----------------------------------------------------------------------------
//
// Name       : CityData::HowMuchMoreFoodNeeded
//
// Description: Gets the aoumt of food that is in addition needed to make 
//              the city grow, without consideration of overcrowding.
//
// Parameters : bonusFood:               Additional food that should be 
//                                       taken into the consideration
//              considerOnlyFromTerrain: Whether farmers should be considered.
//              
// Globals    : g_theCitySizeDB:         The city size database
//              
// Returns    : The amount of food that is missing for optimal growth.
//
// Remark(s)  : Has to be reworked: GetProducedFood() and m_food_delta are 
//              dependent on specialist assignment. These should be function
//              parameters.
//
//----------------------------------------------------------------------------
sint32 CityData::HowMuchMoreFoodNeeded(sint32 bonusFood, bool onlyGrwoth, bool considerOnlyFromTerrain){

#if defined(NEW_RESOURCE_PROCESS)
	double maxFoodFromTerrain = ProcessFood(m_max_food_from_terrain + bonusFood);
	ApplyFoodCoeff(maxFoodFromTerrain);
	maxFoodFromTerrain -= CrimeLoss(maxFoodFromTerrain);

	///////////////////////////////////////////////
	// The utilization fractions
	double foodFraction = 0;
	double prodFraction = 0;
	double goldFraction = 0;

	if(considerOnlyFromTerrain){
		ResourceFractions(foodFraction, prodFraction, goldFraction, PopCount());
		maxFoodFromTerrain = ceil(maxFoodFromTerrain * foodFraction);
	}
	else{
		ResourceFractions(foodFraction, prodFraction, goldFraction, WorkerCount() + SlaveCount());
		maxFoodFromTerrain = ceil(maxFoodFromTerrain * foodFraction);

		maxFoodFromTerrain += ceil((m_foodFromOnePop - m_crimeFoodLossOfOnePop) * FarmerCount());;
	}

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

	sint32 timePerPop;
	strategy.GetTurnsAcceptedForOnePop(timePerPop);
	
	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);
	double food = ceil(static_cast<double>(PopCount()*k_PEOPLE_PER_POPULATION)/static_cast<double>(timePerPop*rec->GetGrowthRate()));

	food -= maxFoodFromTerrain;
	if(onlyGrwoth){
		const CitySizeRecord *nextRec = NULL;
		if(m_sizeIndex >= 0 
		&& m_sizeIndex < g_theCitySizeDB->NumRecords()){
			nextRec = g_theCitySizeDB->Get(m_sizeIndex);
		} 
		else{
			nextRec = g_theCitySizeDB->Get(g_theCitySizeDB->NumRecords()-1);
		}

		double foodForNextRing;
		if(nextRec){
	//		foodForNextRing = GetFoodRequired(nextRec->GetPopulation() + 1 - SlaveCount());
			foodForNextRing = GetFoodRequired(nextRec->GetPopulation() - SlaveCount());
		}
		else{
			foodForNextRing = GetFoodRequired(PopCount() - SlaveCount());
		}

		double foodForNextPop = GetFoodRequired(PopCount() + 1 - SlaveCount());
	
		if(food < foodForNextRing - maxFoodFromTerrain){
			food = foodForNextRing - maxFoodFromTerrain;
		}

		if(food < foodForNextPop - maxFoodFromTerrain){
			food = foodForNextPop - maxFoodFromTerrain;
		}
	}

	return static_cast<sint32>(ceil(food));

#else
//	Function not final yet. Therefore DPRINTFs are still in outcommented state in.
//	DPRINTF(k_DBG_GAMESTATE, ("CityName: %s\n", GetName()));
	double maxFoodFromTerrain = GetMaxFoodFromTerrain() + bonusFood;

	double crimeLost;
	double grossFood = maxFoodFromTerrain;
	ProcessFood(crimeLost, maxFoodFromTerrain, grossFood, considerOnlyFromTerrain);

	double processedBonusFood = bonusFood;
	grossFood = bonusFood;
	ProcessFood(crimeLost, processedBonusFood, grossFood, considerOnlyFromTerrain);

	/////////////////////////////////////
	// A better solution has to be found.
	double currentFood;
	double foodDelta = m_food_delta;
	sint32 bestSpecialist = GetBestSpecialist(POP_FARMER);
	if(bestSpecialist >= 0 && considerOnlyFromTerrain){
		double crimeLossFood;
		double currentFoodPerPop = GetProducedFood() / static_cast<double>(WorkerCount());
		double popFood = static_cast<double>(g_thePopDB->Get(bestSpecialist, g_player[m_owner]->GetGovernmentType())->GetFood()*FarmerCount());
		grossFood = popFood;
		popFood   = ProcessFinalFood(crimeLossFood, grossFood);

		currentFood = GetProducedFood() + processedBonusFood - popFood + currentFoodPerPop*FarmerCount();
		foodDelta  -= popFood;
		foodDelta  += currentFoodPerPop*FarmerCount();
	}
	else{
		currentFood = GetProducedFood() + processedBonusFood;
	}
	/////////////////////////////////////

	const CitySizeRecord *nextRec = NULL;
	if(m_sizeIndex >= 0 
	&& m_sizeIndex < g_theCitySizeDB->NumRecords()){
		nextRec = g_theCitySizeDB->Get(m_sizeIndex);
	} 
	else{
		nextRec = g_theCitySizeDB->Get(g_theCitySizeDB->NumRecords()-1);
	}

	double foodForNextRing;
	if(nextRec){
//		foodForNextRing = GetFoodRequired(nextRec->GetPopulation() + 1 - SlaveCount());
		foodForNextRing = GetFoodRequired(nextRec->GetPopulation() - SlaveCount());
	}
	else{
		foodForNextRing = GetFoodRequired(PopCount() - SlaveCount());
	}

	double foodForNextPop = GetFoodRequired(PopCount() + 1 - SlaveCount());

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

	sint32 timePerPop;
	strategy.GetTurnsAcceptedForOnePop(timePerPop);

	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);
//	DPRINTF(k_DBG_GAMESTATE, ("GrowthRate: %i\n", rec->GetGrowthRate()));
	double food = ceil(static_cast<double>(PopCount()*k_PEOPLE_PER_POPULATION)/static_cast<double>(timePerPop*rec->GetGrowthRate())) - static_cast<double>(bonusFood);
//	DPRINTF(k_DBG_GAMESTATE, ("Food: %i\n", static_cast<sint32>(food)));
//	DPRINTF(k_DBG_GAMESTATE, ("m_food_delta: %f\n", m_food_delta));
	food -= foodDelta;
//	DPRINTF(k_DBG_GAMESTATE, ("Food: %i\n", static_cast<sint32>(food)));
//	DPRINTF(k_DBG_GAMESTATE, ("foodForNextRing: %i\n", static_cast<sint32>(foodForNextRing)));
//	DPRINTF(k_DBG_GAMESTATE, ("maxFoodFromTerrain: %i\n", static_cast<sint32>(maxFoodFromTerrain)));

	if(food < foodForNextRing - maxFoodFromTerrain){
		food = foodForNextRing - maxFoodFromTerrain;
	}
//	DPRINTF(k_DBG_GAMESTATE, ("FoodNextRing: %i\n", static_cast<sint32>(food)));

//	DPRINTF(k_DBG_GAMESTATE, ("Food: %i\n", static_cast<sint32>(food)));
//	DPRINTF(k_DBG_GAMESTATE, ("foodForNextPop: %i\n", static_cast<sint32>(foodForNextPop)));
//	DPRINTF(k_DBG_GAMESTATE, ("currentFood: %i\n", static_cast<sint32>(currentFood)));

	if(food < foodForNextPop - currentFood){
		food = foodForNextPop - currentFood;
	}
//	DPRINTF(k_DBG_GAMESTATE, ("FoodNextPop: %i\n", static_cast<sint32>(foodForNextPop)));
//	DPRINTF(k_DBG_GAMESTATE, ("FoodNextPop: %i\n", static_cast<sint32>(food)));

	return static_cast<sint32>(ceil(food));
#endif
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CrimeLoss
//
// Description: Calculates the crime loss of a factor like food, 
//              production or gold.
//
// Parameters : sint32 gross: The gross value
//
// Globals    : -
//
// Returns    : Crime loss
//
// Remark(s)  : Standardize fatcors.
//
//----------------------------------------------------------------------------
sint32 CityData::CrimeLoss(sint32 gross) const
{
	sint32 crime_loss = static_cast<sint32>(ceil(gross * m_happy->GetCrime()));

	
	if (crime_loss < 0) 
		crime_loss = 0;

	return crime_loss;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CrimeLoss
//
// Description: Calculates the crime loss of a factor like food, 
//              production or gold.
//
// Parameters : double gross: The gross value
//
// Globals    : -
//
// Returns    : Crime Loss as double
//
// Remark(s)  : Standardize fatcors.
//
//----------------------------------------------------------------------------
double CityData::CrimeLoss(double gross) const
{
	double crime_loss = ceil(gross * m_happy->GetCrime()); // Remove ceil
	
	if (crime_loss < 0) 
		crime_loss = 0;

	return crime_loss;
}

#if defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : CityData::BioinfectionLoss
//
// Description: Calculates the loss of production due to bioinfection
//
// Parameters : double prod: Production before loss
//
// Globals    : g_theConstDB: The const database
//
// Returns    : Bioinfection Loss as double
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::BioinfectionLoss(double prod) const
{
	if(m_bioInfectionTurns > 0){
		return prod * g_theConstDB->GetBioInfectionProductionCoef();
	}
	else{
		return 0.0;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::FranchiseLoss
//
// Description: Calculates the loss of production due to franchising
//
// Parameters : double prod: Production before loss
//
// Globals    : g_theConstDB: The const database
//
// Returns    : Franchise Loss as double
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::FranchiseLoss(double prod) const
{
	if(m_franchise_owner >= 0){
		return prod * g_theConstDB->GetFranchiseEffect();
	}
	else{
		return 0.0;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ConversionLoss
//
// Description: Calculates the loss of gold due to conversion
//
// Parameters : double prod: Gold before loss
//
// Globals    : g_theConstDB: The const database
//
// Returns    : Conversion Loss as double
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ConversionLoss(double gold) const
{
	double convertedGold = 0.0;

	if(m_convertedTo >= 0) {
		if(m_convertedBy == CONVERTED_BY_CLERIC) {
			convertedGold = gold * g_theConstDB->ClericConversionFactor();
		} else if(m_convertedBy == CONVERTED_BY_TELEVANGELIST) {
			sint32 doubleTelevangelists;
			buildingutil_GetDoubleTelevangelism(GetEffectiveBuildings(), doubleTelevangelists);
			if(doubleTelevangelists) {
				convertedGold = gold * g_theConstDB->TelevangelistConversionFactor();
			} else {
				convertedGold = gold * g_theConstDB->ClericConversionFactor();
			}
		} else {
			Assert(FALSE);
		}
		if(convertedGold < 1)
			convertedGold = 1;

		if (g_player[m_convertedTo]) {
			sint32 wonderIncrease = wonderutil_GetIncreaseConvertedCitiesFeePercentage(g_player[m_convertedTo]->m_builtWonders);
			if(wonderIncrease > 0) {
				convertedGold += (convertedGold * static_cast<double>(wonderIncrease)) / 100.0;
			}
		}
	}

	return convertedGold;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::FoodFromOnePop
//
// Description: Calculates the amount of food generated by one farmer.
//
// Parameters : double &crimeLoss: Filled with amount of food generated by 
//                                 one farmer lost to crime.
//
// Globals    : g_thePopDB: The population database
//
// Returns    : Amount of food generated by one farmer.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::FoodFromOnePop(double &crimeLoss) const
{
	///////////////////////////////////////////////
	// Get food from specialist
	double food = 0.0;
	if(m_specialistDBIndex[POP_FARMER] >= 0
	&& m_specialistDBIndex[POP_FARMER] < g_thePopDB->NumRecords()
	){
		food = g_thePopDB->Get(m_specialistDBIndex[POP_FARMER], g_player[m_owner]->GetGovernmentType())->GetFood();
	}
	
	///////////////////////////////////////////////
	// Apply Governmental coefficent
	ApplyFoodCoeff(food);

	///////////////////////////////////////////////
	// Calculate losses
	crimeLoss = CrimeLoss(food);

	///////////////////////////////////////////////
	// Return gross food from one pop
	return food;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProdFromOnePop
//
// Description: Calculates the amount of production generated by one laborer.
//
// Parameters : double &crimeLoss: Filled with amount of production generated 
//                                 by one laborer lost to crime.
//
// Globals    : g_thePopDB: The population database
//
// Returns    : Amount of production generated by one laborer.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ProdFromOnePop(double &crimeLoss, double &bioinfectionLoss, double &franchiseLoss) const
{
	///////////////////////////////////////////////
	// Get production from specialist
	double prod = 0.0;
	if(m_specialistDBIndex[POP_LABORER] >= 0
	&& m_specialistDBIndex[POP_LABORER] < g_thePopDB->NumRecords()
	){
		prod = g_thePopDB->Get(m_specialistDBIndex[POP_LABORER], g_player[m_owner]->GetGovernmentType())->GetProduction();
	}

	///////////////////////////////////////////////
	// Apply Governmental coefficent
	ApplyProdCoeff(prod);

	///////////////////////////////////////////////
	// Calculate losses
	bioinfectionLoss = BioinfectionLoss(prod);
	crimeLoss = CrimeLoss(prod - bioinfectionLoss);
	franchiseLoss = FranchiseLoss(prod - bioinfectionLoss - crimeLoss);

	///////////////////////////////////////////////
	// Return gross production from one pop
	return prod;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GoldFromOnePop
//
// Description: Calculates the amount of gold generated by one merchant.
//
// Parameters : double &crimeLoss: Filled with amount of gold generated by 
//                                 one merchant lost to crime.
//
// Globals    : g_thePopDB: The population database
//
// Returns    : Amount of gold generated by one merchant.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::GoldFromOnePop(double &crimeLoss, double &conversionLoss) const
{
	///////////////////////////////////////////////
	// Get gold from specialist
	double gold = 0.0;
	if(m_specialistDBIndex[POP_MERCHANT] >= 0
	&& m_specialistDBIndex[POP_MERCHANT] < g_thePopDB->NumRecords()
	){
		gold = g_thePopDB->Get(m_specialistDBIndex[POP_MERCHANT], g_player[m_owner]->GetGovernmentType())->GetCommerce();
	}

	///////////////////////////////////////////////
	// Apply Governmental coefficent
	ApplyGoldCoeff(gold);

	///////////////////////////////////////////////
	// Calculate losses
	crimeLoss = CrimeLoss(gold);
	conversionLoss = ConversionLoss(gold - crimeLoss);

	///////////////////////////////////////////////
	// Return gross gold from one pop
	return gold;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ScieFromOnePop
//
// Description: Calculates the amount of science generated by one scientist.
//
// Parameters : double &crimeLoss: Filled with amount of science generated by 
//                                 one scientists lost to crime.
//
// Globals    : g_thePopDB: The population database
//
// Returns    : Amount of science generated by one scientist.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ScieFromOnePop(double &crimeLoss) const
{
	///////////////////////////////////////////////
	// Get science from specialist
	double science = 0.0;
	if(m_specialistDBIndex[POP_SCIENTIST] >= 0 
	&& m_specialistDBIndex[POP_SCIENTIST] < g_thePopDB->NumRecords()
	){
		science = g_thePopDB->Get(m_specialistDBIndex[POP_SCIENTIST], g_player[m_owner]->GetGovernmentType())->GetScience();
	}

	///////////////////////////////////////////////
	// Apply Governmental coefficent
	ApplyKnowledgeCoeff(science);

	///////////////////////////////////////////////
	// Calculate losses
	crimeLoss = CrimeLoss(science);

	///////////////////////////////////////////////
	// Return gross scince from one pop
	return science;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetWorkingPeopleInRing
//
// Description: Gets the amount of people that are working in the given ring.
//
// Parameters : sint32 ring:     The given ring.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : Amount of people that are working in the given ring.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetWorkingPeopleInRing(sint32 ring) const
{
	if(ring < 0
	|| ring >= g_theCitySizeDB->NumRecords() - 1
	){
		ring = g_theCitySizeDB->NumRecords() - 1;
		Assert(false);
	}

	sint32 workingPeople = WorkerCount() + SlaveCount() + 1; // Center is free

	sint32 i;
	for(i = 0; i < ring; ++i){
		workingPeople -= m_ringSizes[i];
	}

	if(workingPeople >= m_ringSizes[i]){
		return m_ringSizes[i];
	}
	else if(workingPeople <= 0){
		return 0;
	}
	else{
		return workingPeople;
	}

}

//----------------------------------------------------------------------------
//
// Name       : CityData::ResourceFractions
//
// Description: Calculates the amount of science generated by one scientist.
//
// Parameters : double &foodFraction: Filled fraction of food actual collected.
//              double &prodFraction: Filled fraction of production actual collected.
//              double &goldFraction: Filled fraction of gold actual collected.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::ResourceFractions(double &foodFraction, double &prodFraction, double goldFraction, sint32 workingPeople) const
{
	sint32 i;
	workingPeople++; // Center is free

	///////////////////////////////////////////////
	// Calcuete the nominators of the fractions
	for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
		if(m_ringSizes[i] == 0){
			break;
		}
		else if(m_ringSizes[i] <= workingPeople){
			foodFraction += static_cast<double>(m_ringFood[i]);
			prodFraction += static_cast<double>(m_ringProd[i]);
			goldFraction += static_cast<double>(m_ringGold[i]);
			workingPeople -= m_ringSizes[i];
		}
		else{
			double utilization = static_cast<double>(workingPeople) / static_cast<double>(m_ringSizes[i]);
			foodFraction += static_cast<double>(m_ringFood[i]) * utilization;
			prodFraction += static_cast<double>(m_ringProd[i]) * utilization;
			goldFraction += static_cast<double>(m_ringGold[i]) * utilization;
			break; // Leave loop
		}
	}

	if(workingPeople == 0){
		///////////////////////////////////////////////
		// Calcuete the denominators of the fractions
		double foodTotal = 0.0;
		double prodTotal = 0.0;
		double goldTotal = 0.0;
		for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
			foodTotal += static_cast<double>(m_ringFood[i]);
			prodTotal += static_cast<double>(m_ringProd[i]);
			goldTotal += static_cast<double>(m_ringGold[i]);
		}

		///////////////////////////////////////////////
		// Calculate the the fractions
		foodFraction /= foodTotal;
		prodFraction /= prodTotal;
		goldFraction /= goldTotal;
	}
	else{
		foodFraction = 1.0;
		prodFraction = 1.0;
		goldFraction = 1.0;
	}

	// Maybe calculate how many people need a job
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetUnemployedPeople
//
// Description: Gets the amount of people that have to become specialists,
//              because the are not enough tiles in the field left.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : The number of people needed to be made specialists.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetUnemployedPeople() const
{
	return TilesForWorking() - PopCount();
}
#endif

//----------------------------------------------------------------------------
//
// Name       : CityData::GetCityStyle
//
// Description: Get the style of the city.
//
// Parameters : -
//
// Globals    : g_player:     List of players
//
// Returns    : The style of the city
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

//----------------------------------------------------------------------------
//
// Name       : CityData::SetCityStyle
//
// Description: Set the style of the city and updates its sprite.
//
// Parameters : sint32 style: The city style to set.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::SetCityStyle(sint32 style)
{

	m_cityStyle = style;
	UpdateSprite();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetRing
//
// Description: Gets the ring number in which the given tiles lies.
//
// Parameters : MapPoint pos:    The location for which the ring position
//                               should be found.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : The number of the ring of the city in which the given 
//              position can be found.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::GetRing(MapPoint pos) const
{
	MapPoint cityPos = m_home_city.RetPos();

	sint32 i, squaredRadius, squaredDistance;
	for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
		squaredRadius = g_theCitySizeDB->Get(i)->GetSquaredRadius();
		squaredDistance = MapPoint::GetSquaredDistance(cityPos, pos);
		if(squaredDistance <= squaredRadius){
			return i;
		}
	}

	Assert(false);
	return -1;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetFoodFromRing
//
// Description: Gets the amount of food that can be collected in the 
//              given ring.
//
// Parameters : sint32 ring:     The ring for that the amount of collected
//                               food should be returned.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : The amount of food that can be collected in the given ring.
//
// Remark(s)  : If -1 is passed to the function then the food of all rings
//              is returned.
//
//----------------------------------------------------------------------------
sint32 CityData::GetFoodFromRing(sint32 ring) const
{
	if(ring >= 0
	&& ring < g_theCitySizeDB->NumRecords()
	){
		return m_ringFood[ring];
	}
	else{
		sint32 i; 
		sint32 maxFood = 0;
		for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
			maxFood += m_ringFood[i];
		}
		return maxFood;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetProdFromRing
//
// Description: Gets the amount of production that can be collected in the 
//              given ring.
//
// Parameters : sint32 ring:     The ring for that the amount of collected
//                               production should be returned.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : The amount of production that can be collected in the given ring.
//
// Remark(s)  : If -1 is passed to the function then the production of all rings
//              is returned.
//
//----------------------------------------------------------------------------
sint32 CityData::GetProdFromRing(sint32 ring) const
{
	if(ring >= 0
	&& ring < g_theCitySizeDB->NumRecords()
	){
		return m_ringProd[ring];
	}
	else{
		sint32 i; 
		sint32 maxProd = 0;
		for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
			maxProd += m_ringProd[i];
		}
		return maxProd;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetGoldFromRing
//
// Description: Gets the amount of gold that can be collected in the 
//              given ring.
//
// Parameters : sint32 ring:     The ring for that the amount of collected
//                               gold should be returned.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : The amount of gold that can be collected in the given ring.
//
// Remark(s)  : If -1 is passed to the function then the gold of all rings
//              is returned.
//
//----------------------------------------------------------------------------
sint32 CityData::GetGoldFromRing(sint32 ring) const
{
	if(ring >= 0
	&& ring < g_theCitySizeDB->NumRecords()
	){
		return m_ringGold[ring];
	}
	else{
		sint32 i; 
		sint32 maxGold = 0;
		for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
			maxGold += m_ringGold[i];
		}
		return maxGold;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetRingSize
//
// Description: Gets the number of tiles that are in the given ring.
//
// Parameters : sint32 ring:     The ring for that the number of tiles 
//                               should be returned.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : The amount of tiles in the given ring.
//
// Remark(s)  : If -1 is passed to the function then the number of all tiles
//              that the city owns is returned.
//
//----------------------------------------------------------------------------
sint32 CityData::GetRingSize(sint32 ring) const
{
	if(ring >= 0
	&& ring < g_theCitySizeDB->NumRecords()
	){
		return m_ringSizes[ring];
	}
	else{
		sint32 i; 
		sint32 numTiles = 0;
		for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
			numTiles += m_ringSizes[i];
		}
		return numTiles;
	}
}

#if defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : CityData::GetSpecialistsEffect
//
// Description: Gets the additional resources that are produced by a specialist.
//
// Parameters : sint32 ring:           The ring for that the resource effect should be
//                                     got.
//              double &farmersEff:    Filled with the additional amount of food
//                                     that a laborer can produce in the given ring.
//              double &laborersEff:   Filled with the additional amount of production
//                                     that a merchant can produce in the given ring.
//              double &merchantsEff:  Filled with the additional amount of gold
//                                     that a scientists can produce in the given ring.
//              double &scientistsEff: Filled with the additional amount of science
//                                     that a farmer can produce in the given ring.
//
// Globals    : g_theCitySizeDB: The city size database
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::GetSpecialistsEffect(sint32 ring, double &farmersEff, double &laborersEff, double &merchantsEff, double &scientistsEff) const
{
	assert(ring >= 0 && ring < g_theCitySizeDB->NumRecords());
	if(ring < 0 || ring >= g_theCitySizeDB->NumRecords()) ring = 0;

	farmersEff = m_farmersEff[ring];
	laborersEff = m_laborersEff[ring];
	merchantsEff = m_merchantsEff[ring];
	scientistsEff = m_scientistsEff[ring];
}
#endif

//----------------------------------------------------------------------------
//
// Name       : CityData::StyleHappinessIncr
//
// Description: Gets the amount of happiness increase associated to the 
//              city's city style.
//
// Parameters : -
//
// Globals    : g_theCityStyleDB: The city style database
//              g_player:         The list of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 CityData::StyleHappinessIncr() const
{
	return g_theCityStyleDB->Get(m_cityStyle, g_player[m_owner]->GetGovernmentType())->GetHappyInc();
}
