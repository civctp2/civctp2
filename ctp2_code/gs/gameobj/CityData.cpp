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
// - Enable logging when set, even when not building a debug version.
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
// - Moved Peter's last modification to Cell.cpp and UnseenCell.cpp, ideally
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
// - Fixed Gold isn't added twice to income. - Aug 6th 2005 Martin  Gühmann
// - Added new code as preparation for resource calculation redesign.- Aug 6th 2005 Martin Gühmann
// - Added code for new city resource calculation. (Aug 12th 2005 Martin Gühmann)
// - Removed more unused methods. (Aug 12th 2005 Martin Gühmann)
// - NeedsCityGood added to CanBuildUnit, CanBuildBuilding, and CanBuild Wonder
//   requiring a good in the radius or if the city is buying it before a it can be
//   built. (Sept 29nd 2005 by E)
// - Added city style specific happiness bonus method. (Oct 7th 2005 Martin Gühmann)
// - Implemented EnablesGood for buildings, wonders and tile improvements now they
//   give goods to a city by E November 5th 2005
// - Goodexport Bonuses - To ProcessFood,ProcessProduction,ProcessGold, ProcessScience
//   added a check that if a city has or is buying a good than you can get a bonus.
//   EfficiencyOrCrime flag affects all four.  - added by E November 5th 2005
// - Corrected building maintenance deficit spending handling.
// - Correct GoodExport bonuses (thanks fromafar) by E 11-Jan-2006
// - Implemented 'OnePerCiv' in CanBuildBuilding allowing for Small wonders by E 2-14-2006
// - GoldPerCity in ProcessGold by E 2-14-2006
// - GoldPerUnit in ProcessGold by E 2-24-2006
// - GoldPerUnitReadiness in ProcessGold by E 2-24-2006
// - Add Civilization and Citystyle Bonuses to Gold, Food, science, production by either a
//   bonus (just a solid integer) or by a percentage. by E 2-27-2006
// - Added Increase Borders to AddImprovement by E 2-28-2006
// - GoldPerUnitSupport in ProcessGold by E 3-24-2006 uses goldhunger
// - Has EitherGood and HasNeedGood now checks selling goods by E 3-27-2006
// - Added Increase borders to AddWonder 3-28-2006
// - Added ShowOnMap to wonders and buildings (a work in progress)
// - Added IsCoastal check to canbuildwonders 3-31-2006
// - Added ObsoleteUnit so units can be obsolete by the availability of other units by E 3-31-2006
// - Added UpgradeTo so units can be obsolete by the availability of unit they upgrade to by E 3-31-2006
// - CantTrade flag for Goods now works by E 4-26-2006 (outcomment to allow for CanCollectGood)
// - CanCollectGood bool added by E to check goods for
//   CantTrade or Available and Vanish Advances 4-27-2006
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
// - Made AI deficit gold spending depending on the dificulty settings. (April 29th 2006 Martin Gühmann)
// - Made good requirements consistent with each, all requirements are now
//   the same: The city needs more goods that it buys and collects than it
//   sells. (April 30th 2006 Martin Gühmann)
// - Add AddsASlave to goods for beginturn. If you have the good then a slave
//   is added each turn you have that good. E 5-12-2006
// - CivilisationOnly added to CanBuildBuilding and CnBuildWonder E 5-12-2006
// - NeedsFeatToBuild added to CanBuildBuilding and CanBuildWonder E 5-12-2006
// - OnePerCiv added to CanBuildWonder E 5-12-2006 (doesn't work)
// - OnePerCiv fixed E 5-16-2006
// - Added Science bonuses from goods and citystyle to GetScienceFromPops
//   this should be where it is needed (E 5-17-2006)
// - AICityDefenderBonus added as an option for AI defenders by E 5-16-2006
// - Added a SectarianHappiness function by E 5-24-2006
// - IsXenephobic govts now kill off foreign populations by E 5-26-2006
// - Implemented HasGulags, IsCapitalist, IsTechnocracy, IsAgrarian, HasMindlessTelevision
//   govt flags that force specialist changes by E 5-25-2006
// - Implemented ExcludedByBuilding and CantSell by E 5-25-2006
// - Added IncreaseHP to AddImprovement by E 5-25-2006
// - ExcludedbyBuilding and ExcludedbyWonder added to Wonders
// - added AddGoodToCity function
// - ExcludedbyBuilding and ExcludedbyWonder added to Units and buildings
// - Added PrerequisiteWonder to Units and buildings
// - Added NeedsFeatToBuild to Units
// - Added Civ Bonuses to Food, Commerce, Production, Science by E Jul 3 2006
// - Added IsBonusGood to FindGoodDistances enabling non-map goods to be
//   collected by E Jul 3 2006
// - Added Advances bonus, now some advances can add science, food, Production,
//   or Gold once discovered by E July 5 2006
// - Added Miltia code; DiffDB gives AI cheapest unit if city is empty
// - Implemented Militia Building flag creates militia at start of turn if city is empty.
// - NeedsPopCountToBuild was added awhile ago it limits some buildings to only
//   being available once a certian population is reached. by E
// - Added RevoltInsurgents diffDB to beginturn. If a city is rioting then there
//   is a chance that the riot will spawn barbarians
// - added IsReligion bools 1-23-2007
// - Added Energy Ratio to production and gold. Only enebaled in profileDB also
//   units, buildings, wonders, and tileimps must have energy flags for
//   calculation - by E 2-28-2007
// - Added Insurgent Spawn slic message
// - Added BreadBasket modifier so you can have food producing areas i.e.
//   America's midwest or Rome's Egypt
// - Outcommented extra insurgent code in riot
// - Outcomment founder code in insurgents b/c it may cause same crash as
//   sectarianhappiness
// - Implemented RevoltInsurgents Rules/Profile option
// - Added profile check for AImilitia, gold deficit, gold per city,
//   gold per unit, aicitydefense
// - Added GetNumCityWonders and GetNumCityBuildings methods
// - Added check to ConstDB of MaxCityWonders and MaxCityBuildings for modders by E
// - outcommented maxcity stuff
// - Prevented barbarians from building tileimps - E 5-18-2007
// - NeedsAnyPlayerFeatToBuild implemented for Wonders and Buildings
// - MaxCityWonders and MaxCityBuildings implemented
// - Added Slic execute
// - Added RiotCasualties and InsurgentSpawn methods to clean up BeginTurn
// - Added DestroyOnePerCiv, IsReligious, HasReligionIcon 6-4-2007
// - Added code so cities with a trade route through them can collect some gold
//   this could be implemented by a wonder of feat (in the future) or should it
//   be standard? it would add value to cities. - E 6.10.2007
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Added CityExpansion and CitySlum code
// - The CityInfluenceChanged event is now valid after conquest that destroyed
//   the city. (26-Jan-2008 Martin Gühmann)
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
// - Added GetCityLandAttackBonus, GetCityAirAttackBonus and
//   GetCitySeaAttackBonus for battleview window. (07-Mar-2009 Maq)
// - Added functions to find total that each specialist type gives to a city,
//   after crime and other modifiers. (28-Mar-2009 Maq)
// - Stopped UpgradeTo obsoleting units. (30-Mar-2009 Maq)
// - Added single-player start and end age affects. (11-Apr-2009 Maq)
// - Prevented city from building gaia controller buildings unless science
//   victory race has started. (01-Jul-2009 Maq)
// - When a city is settled in foreign territory, its the national borders
//   will expand as they should be. (13-Jul-2009 Martin Gühmann)
// - Changed science formula to deduct crime after the government coefficient
//   like all other resources. (22-Jul-2009 Maq)
// - Added methods to find food and production before deductions. (22-Jul-2009 Maq)
// - Added stuff for reimplementing switch production penalty. (22-Jul-2009 Maq)
// - Loading of build queues that only contain illigal items do not make 
//   the game crash anymore. (13-Jan-2019 Martin Gühmann)
// - Reorderd the member variables and converted m_name to a pointer to reduce
//   the needed memory. Updated for that the copy and serialize methods. (13-Jan-2019 Martin Gühmann)
// - Implemented specialist increase of wonders for entertainers. (20-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "citydata.h"

#include "AdvanceRecord.h"
#include "Advances.h"
#include "advanceutil.h"
#include "AgeCityStyleRecord.h"
#include "Agreement.h"
#include "AgreementTypes.h"
#include "AICause.h"
#include <algorithm>                    // std::max, std::min
#include "ArmyData.h"
#include "ArmyPool.h"
#include "Barbarians.h"
#include "BuildingRecord.h"
#include "buildingutil.h"
#include "c3errors.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "Checksum.h"
#include "CityInfluenceIterator.h"
#include "CitySizeRecord.h"
#include "CityStyleRecord.h"
#include "civarchive.h"
#include "CivilisationPool.h"
#include "colorset.h"
#include "ConstRecord.h"                    // g_theConstDB
#include "DB.h"
#include "DifficultyRecord.h"
#include "Diplomat.h"                   // To be able to retrieve the current strategy
#include "director.h"                   // g_director
#include "EditQueue.h"
#include "Exclusions.h"
#include "FeatTracker.h"
#include "gaiacontroller.h"				// To check buildings needed for science victory
#include "GameEventManager.h"
#include "gamefile.h"
#include "GameSettings.h"
#include "Globals.h"
#include "Gold.h"
#include "GovernmentRecord.h"
#include "Happy.h"
#include "HappyTracker.h"
#include "installationtree.h"
#include "MaterialPool.h"
#include "MessagePool.h"
#include "net_action.h"
#include "net_info.h"
#include "network.h"
#include "player.h"                     // g_player
#include "pollution.h"
#include "PopRecord.h"
#include "profileDB.h"                  // g_theProfileDB
#include "RandGen.h"                    // g_rand
#include "Readiness.h"
#include "ResourceRecord.h"
#include "RiskRecord.h"  //add for barb code
#include "scenarioeditor.h"
#include "Score.h"
#include "SelItem.h"                    // g_selected_item
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "soundmanager.h"               // g_soundManager
#include "SpecialAttackInfoRecord.h"
#include "SpecialEffectRecord.h"
#include "StrategyRecord.h"             // For accessing the strategy database
#include "StrDB.h"                      // g_theStringDB
#include "TaxRate.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "TerrainImprovementRecord.h"   //EMOD
#include "TerrImprovePool.h"            //EMOD
#include "tiledmap.h"
#include "TopTen.h"
#include "TradeOffer.h"
#include "TradeOfferPool.h"
#include "TradePool.h"
#include "TradeRoute.h"
#include "TurnCnt.h"                    // g_turn
#include "UnitActor.h"
#include "UnitData.h"
#include "UnitPool.h"                   // g_theUnitPool
#include "UnitRecord.h"
#include "unitutil.h"
#include "UnseenCell.h"
#include "WonderRecord.h"
#include "WonderTracker.h"
#include "wonderutil.h"
#include "World.h"                      // g_theWorld

#if defined(_DEBUG) || defined(USE_LOGGING)
#include "Timer.h"
#endif

extern Pollution *      g_thePollution;
extern TopTen *         g_theTopTen;

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
    m_goldFromTransitRoutes             (0),
    m_science                           (0),
    m_luxury                            (0),
    m_city_attitude                     (CITY_ATTITUDE_CONTENT),
    m_collected_production_this_turn    (0),
    m_gross_production                  (0),
    m_gross_prod_before_bonuses         (0),
    m_net_production                    (0),
    m_production_lost_to_crime          (0),
    m_built_improvements                (0),
    m_builtWonders                      (0),
    m_food_delta                        (0.0),
    m_gross_food                        (0.0),
    m_gross_food_before_bonuses         (0.0),
    m_net_food                          (0.0),
    m_food_lost_to_crime                (0.0),
    m_food_consumed_this_turn           (0.0),
    m_total_pollution                   (0),
    m_cityPopulationPollution           (0),
    m_cityIndustrialPollution           (0),
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
    m_happinessAttackedBy               (PLAYER_UNASSIGNED),
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
    m_name                              (NULL),
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
    m_foodFromOnePop                    (0.0),
    m_prodFromOnePop                    (0.0),
    m_goldFromOnePop                    (0.0),
    m_scieFromOnePop                    (0.0),
    m_crimeFoodLossOfOnePop             (0.0),
    m_crimeProdLossOfOnePop             (0.0),
    m_crimeGoldLossOfOnePop             (0.0),
    m_crimeScieLossOfOnePop             (0.0),
    m_bioinfectionProdLossOfOnePop      (0.0),
    m_franchiseProdLossOfOnePop         (0.0),
    m_conversionGoldLossOfOnePop        (0.0),
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
    m_sentInefficientMessageAlready     (false),
    m_culture                           (0),      //emod
    m_secthappy                         (0),      //emod
    m_bonusFood                         (0.0),
    m_bonusFoodCoeff                    (0.0),
    m_bonusProdCoeff                    (0.0),
    m_bonusProd                         (0.0),
    m_bonusGoldCoeff                    (0.0),
    m_bonusGold                         (0.0)
{
	m_build_queue.SetOwner(m_owner);
	m_build_queue.SetCity(m_home_city);
	g_theWorld->SetCapitolDistanceDirtyFlags(1 << owner);

	// Set the style of the founder of the city - if any.
	if (g_player[owner] && g_player[owner]->GetCivilisation())
	{
		m_cityStyle = g_player[owner]->GetCivilisation()->GetCityStyle();
	} //for citystyle

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

		//// all trade routes now killed in PrepareToRemove where owner can still be determined to remove SeenByBit
	        //// left just in case PrepareToRemove was not called before
		sint32 i;
		for(i = 0; i < m_tradeSourceList.Num(); i++)
		{
			m_tradeSourceList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_DIED);
		}

		for(i = 0; i < m_tradeDestinationList.Num(); i++)
		{
			m_tradeDestinationList[i].Kill(CAUSE_KILL_TRADE_ROUTE_CITY_DIED);
		}
	}

	delete m_happy;
	delete[] m_distanceToGood;
	delete[] m_ringFood;
	delete[] m_ringProd;
	delete[] m_ringGold;
	delete[] m_ringSizes;
	delete[] m_name;

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

	if (archive.IsStoring())
	{
		sint8  empty = 0;
		sint32 tmp   = 0;

		archive.PutSINT32(m_owner);
		archive.PutUINT32(m_slaveBits);
		archive.PutSINT32(m_accumulated_food);
		archive.PutSINT32(m_shieldstore);
		archive.PutSINT32(m_shieldstore_at_begin_turn);
		archive.PutSINT32(m_build_category_at_begin_turn);
		archive.PutSINT32(m_net_gold);
		archive.PutSINT32(m_gold_lost_to_crime);
		archive.PutSINT32(m_gross_gold);
		archive.PutSINT32(m_goldFromTradeRoutes);
		archive.PutSINT32(m_goldLostToPiracy);
		archive.PutSINT32(m_science);
		archive.PutSINT32(m_luxury);
		tmp = static_cast<sint32>(m_city_attitude);
		archive.PutSINT32(tmp);
		archive.PutSINT32(m_collected_production_this_turn);
		archive.PutSINT32(m_gross_production);
		archive.PutSINT32(m_net_production);
		archive.PutSINT32(m_production_lost_to_crime);
		archive.PutSINT8(empty);
		archive.PutSINT8(empty);
		archive.PutSINT8(empty);
		archive.PutSINT8(empty);

		archive.PutUINT64(m_built_improvements);
		archive.PutUINT64(m_builtWonders);

		archive.PutDOUBLE(m_food_delta);
		archive.PutDOUBLE(m_gross_food);
		archive.PutDOUBLE(m_net_food);
		archive.PutDOUBLE(m_food_lost_to_crime);
		archive.PutDOUBLE(m_food_consumed_this_turn);

		archive.PutSINT32(m_total_pollution);
		archive.PutSINT32(m_cityPopulationPollution);
		archive.PutSINT32(m_cityIndustrialPollution);
		archive.PutSINT32(m_goldFromTransitRoutes);
		archive.PutSINT32(m_cityPollutionCleaner);
		tmp = static_cast<BOOL>(m_contribute_materials);
		archive.PutSINT32(tmp); // Was BOOL
		tmp = static_cast<BOOL>(m_contribute_military);
		archive.PutSINT32(tmp); // Was BOOL

		tmp = static_cast<BOOL>(m_capturedThisTurn);
		archive.PutSINT32(tmp); // Was BOOL

		archive.PutSINT32(m_spied_upon);
		tmp = static_cast<BOOL>(m_walls_nullified);
		archive.PutSINT32(tmp); // Was BOOL
		archive.PutSINT32(m_franchise_owner);
		archive.PutSINT32(m_franchiseTurnsRemaining);
		archive.PutSINT32(m_watchfulTurns);
		archive.PutSINT32(m_bioInfectionTurns);
		archive.PutSINT32(m_bioInfectedBy);
		archive.PutSINT32(m_nanoInfectionTurns);
		archive.PutSINT32(m_nanoInfectedBy);
		archive.PutSINT32(m_convertedTo);
		archive.PutSINT32(m_convertedGold);

		tmp = static_cast<sint32>(m_convertedBy);
		archive.PutSINT32(tmp);
		tmp = static_cast<BOOL>(m_terrainWasPolluted);
		archive.PutSINT32(tmp); // Was BOOL
		archive.PutSINT32(m_happinessAttackedBy);
		tmp = static_cast<BOOL>(m_terrainImprovementWasBuilt);
		archive.PutSINT32(tmp); // Was BOOL
		tmp = static_cast<BOOL>(m_improvementWasBuilt);
		archive.PutSINT32(tmp); // Was BOOL

		tmp = static_cast<BOOL>(m_isInjoined);
		archive.PutSINT32(tmp); // Was BOOL
		archive.PutSINT32(m_injoinedBy);

		archive.PutSINT32(m_airportLastUsed);

		archive.PutSINT32(m_founder);

		archive.PutSINT32(m_wages_paid);

		archive.PutSINT32(m_pw_from_infrastructure);
		archive.PutSINT32(m_gold_from_capitalization);
		tmp = static_cast<BOOL>(m_buildInfrastructure);
		archive.PutSINT32(tmp); // Was BOOL
		tmp = static_cast<BOOL>(m_buildCapitalization);
		archive.PutSINT32(tmp); // Was BOOL

		tmp = static_cast<BOOL>(m_paidForBuyFront);
		archive.PutSINT32(tmp); // Was BOOL

		tmp = static_cast<sint32>(m_doUprising);
		archive.PutSINT32(tmp);

		archive.PutSINT32(m_turnFounded);

		archive.PutSINT32(m_productionLostToFranchise);

		archive.PutUINT8(m_probeRecoveredHere);
		archive.PutUINT8(empty);// pad

		archive.PutSINT16(m_lastCelebrationMsg);
		archive.PutUINT8(m_alreadySoldABuilding);
		archive.PutUINT8(empty);// pad
		archive.PutUINT8(empty);// pad
		archive.PutUINT8(empty);// pad

		archive.PutSINT32(m_population);
		archive.PutSINT32(m_partialPopulation);

		// Invalid for Big Endian
		archive.StoreChunk((uint8 *)&m_numSpecialists, ((uint8 *)&m_numSpecialists) + sizeof(m_numSpecialists));
		archive.PutUINT8(empty);// pad
		archive.PutUINT8(empty);// pad

		// Invalid for Big Endian
		archive.StoreChunk((uint8 *)&m_specialistDBIndex, ((uint8 *)&m_specialistDBIndex) + sizeof(m_specialistDBIndex));

		archive.PutSINT32(m_sizeIndex);
		archive.PutSINT32(m_workerFullUtilizationIndex);
		archive.PutSINT32(m_workerPartialUtilizationIndex);

		tmp = static_cast<BOOL>(m_useGovernor);
		archive.PutSINT32(tmp); // Was BOOL
		archive.PutSINT32(m_buildListSequenceIndex);
		tmp = static_cast<BOOL>(m_garrisonOtherCities);
		archive.PutSINT32(tmp); // Was BOOL
		tmp = static_cast<BOOL>(m_garrisonComplete);
		archive.PutSINT32(tmp); // Was BOOL

		archive.PutSINT8(m_currentGarrison);
		archive.PutSINT8(m_neededGarrison);
		archive.PutUINT8(empty);// pad
		archive.PutUINT8(empty);// pad
		archive.PutDOUBLE(m_currentGarrisonStrength);
		archive.PutDOUBLE(m_neededGarrisonStrength);

		archive.PutSINT32(m_sellBuilding);
		archive.PutUINT8(m_buyFront);
		archive.PutUINT8(empty);// pad
		archive.PutUINT8(empty);// pad
		archive.PutUINT8(empty);// pad

		archive.PutSINT32(m_max_food_from_terrain);
		archive.PutSINT32(m_max_prod_from_terrain);
		archive.PutSINT32(m_max_gold_from_terrain);

		archive.PutSINT32(m_growth_rate);
		archive.PutDOUBLE(m_overcrowdingCoeff);

		archive.PutSINT32(m_starvation_turns);

		archive.PutSINT32(m_cityStyle);

		m_pos.Serialize(archive);

		tmp = static_cast<BOOL>(m_is_rioting);
		archive.PutSINT32(tmp); // Was BOOL
	}
	else
	{
		sint8 empty = 0;

		m_owner                          = archive.GetSINT32();
		m_slaveBits                      = archive.GetUINT32();
		m_accumulated_food               = archive.GetSINT32();
		m_shieldstore                    = archive.GetSINT32();
		m_shieldstore_at_begin_turn      = archive.GetSINT32();
		m_build_category_at_begin_turn   = archive.GetSINT32();
		m_net_gold                       = archive.GetSINT32();
		m_gold_lost_to_crime             = archive.GetSINT32();
		m_gross_gold                     = archive.GetSINT32();
		m_goldFromTradeRoutes            = archive.GetSINT32();
		m_goldLostToPiracy               = archive.GetSINT32();
		m_science                        = archive.GetSINT32();
		m_luxury                         = archive.GetSINT32();
		m_city_attitude                  = static_cast<CITY_ATTITUDE>(archive.GetSINT32());
		m_collected_production_this_turn = archive.GetSINT32();
		m_gross_production               = archive.GetSINT32();
		m_net_production                 = archive.GetSINT32();
		m_production_lost_to_crime       = archive.GetSINT32();
		empty                            = archive.GetSINT8();
		empty                            = archive.GetSINT8();
		empty                            = archive.GetSINT8();
		empty                            = archive.GetSINT8();
		m_built_improvements             = archive.GetUINT64();
		m_builtWonders                   = archive.GetUINT64();
		m_food_delta                     = archive.GetDOUBLE();
		m_gross_food                     = archive.GetDOUBLE();
		m_net_food                       = archive.GetDOUBLE();
		m_food_lost_to_crime             = archive.GetDOUBLE();
		m_food_consumed_this_turn        = archive.GetDOUBLE();
		m_total_pollution                = archive.GetSINT32();
		m_cityPopulationPollution        = archive.GetSINT32();
		m_cityIndustrialPollution        = archive.GetSINT32();
		m_goldFromTransitRoutes          = archive.GetSINT32();
		m_cityPollutionCleaner           = archive.GetSINT32();
		m_contribute_materials           = archive.GetSINT32() != FALSE; // Was BOOL
		m_contribute_military            = archive.GetSINT32() != FALSE; // Was BOOL
		m_capturedThisTurn               = archive.GetSINT32() != FALSE; // Was BOOL
		m_spied_upon                     = archive.GetSINT32();
		m_walls_nullified                = archive.GetSINT32() != FALSE; // Was BOOL
		m_franchise_owner                = archive.GetSINT32();
		m_franchiseTurnsRemaining        = archive.GetSINT32();
		m_watchfulTurns                  = archive.GetSINT32();
		m_bioInfectionTurns              = archive.GetSINT32();
		m_bioInfectedBy                  = archive.GetSINT32();
		m_nanoInfectionTurns             = archive.GetSINT32();
		m_nanoInfectedBy                 = archive.GetSINT32();
		m_convertedTo                    = archive.GetSINT32();
		m_convertedGold                  = archive.GetSINT32();
		m_convertedBy                    = static_cast<CONVERTED_BY>(archive.GetSINT32());
		m_terrainWasPolluted             = archive.GetSINT32() != FALSE; // Was BOOL
		m_happinessAttackedBy            = archive.GetSINT32();
		m_terrainImprovementWasBuilt     = archive.GetSINT32() != FALSE; // Was BOOL
		m_improvementWasBuilt            = archive.GetSINT32() != FALSE; // Was BOOL
		m_isInjoined                     = archive.GetSINT32() != FALSE; // Was BOOL
		m_injoinedBy                     = archive.GetSINT32();
		m_airportLastUsed                = archive.GetSINT32();
		m_founder                        = archive.GetSINT32();
		m_wages_paid                     = archive.GetSINT32();
		m_pw_from_infrastructure         = archive.GetSINT32();
		m_gold_from_capitalization       = archive.GetSINT32();
		m_buildInfrastructure            = archive.GetSINT32() != FALSE; // Was BOOL
		m_buildCapitalization            = archive.GetSINT32() != FALSE; // Was BOOL
		m_paidForBuyFront                = archive.GetSINT32() != FALSE; // Was BOOL
		m_doUprising                     = static_cast<UPRISING_CAUSE>(archive.GetSINT32());
		m_turnFounded                    = archive.GetSINT32();
		m_productionLostToFranchise      = archive.GetSINT32();
		m_probeRecoveredHere             = archive.GetUINT8();
		empty                            = archive.GetUINT8();// pad
		m_lastCelebrationMsg             = archive.GetSINT16();
		m_alreadySoldABuilding           = archive.GetUINT8();
		empty                            = archive.GetUINT8();// pad
		empty                            = archive.GetUINT8();// pad
		empty                            = archive.GetUINT8();// pad
		m_population                     = archive.GetSINT32();
		m_partialPopulation              = archive.GetSINT32();

		archive.LoadChunk((uint8 *)&m_numSpecialists, ((uint8 *)&m_numSpecialists) + sizeof(m_numSpecialists));
		empty                            = archive.GetUINT8();// pad
		empty                            = archive.GetUINT8();// pad

		archive.LoadChunk((uint8 *)&m_specialistDBIndex, ((uint8 *)&m_specialistDBIndex) + sizeof(m_specialistDBIndex));

		m_sizeIndex                      = archive.GetSINT32();
		m_workerFullUtilizationIndex     = archive.GetSINT32();
		m_workerPartialUtilizationIndex  = archive.GetSINT32();
		m_useGovernor                    = archive.GetSINT32() != FALSE; // Was BOOL
		m_buildListSequenceIndex         = archive.GetSINT32();
		m_garrisonOtherCities            = archive.GetSINT32() != FALSE; // Was BOOL
		m_garrisonComplete               = archive.GetSINT32() != FALSE; // Was BOOL
		m_currentGarrison                = archive.GetSINT8();
		m_neededGarrison                 = archive.GetSINT8();
		empty                            = archive.GetUINT8();// pad
		empty                            = archive.GetUINT8();// pad
		m_currentGarrisonStrength        = archive.GetDOUBLE();
		m_neededGarrisonStrength         = archive.GetDOUBLE();
		m_sellBuilding                   = archive.GetSINT32();
		m_buyFront                       = archive.GetUINT8();
		empty                            = archive.GetUINT8();// pad
		empty                            = archive.GetUINT8();// pad
		empty                            = archive.GetUINT8();// pad
		m_max_food_from_terrain          = archive.GetSINT32();
		m_max_prod_from_terrain          = archive.GetSINT32();
		m_max_gold_from_terrain          = archive.GetSINT32();
		m_growth_rate                    = archive.GetSINT32();
		m_overcrowdingCoeff              = archive.GetDOUBLE();
		m_starvation_turns               = archive.GetSINT32();
		m_cityStyle                      = archive.GetSINT32();

		m_pos.Serialize(archive);

		m_is_rioting                     = archive.GetSINT32() != FALSE; // Was BOOL
	}

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

	// @todo Check later: next 3 lines are invalid when CTP1_TRADE has been defined. See CityData.h.
	m_collectingResources.Serialize(archive);
	m_buyingResources.Serialize(archive);
	m_sellingResources.Serialize(archive);
	m_happy->Serialize(archive);

	sint32 len;
	if(archive.IsStoring())
	{
		len = strlen(m_name) + 1;
		archive << len;
		archive.Store((uint8*)m_name, len * sizeof(MBCHAR));

		archive.Store((uint8*)m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
		archive.Store((uint8*)&m_defensiveBonus, sizeof(double)); // Should be PutDOUBLE
	}
	else
	{
		archive >> len;
		m_name = new MBCHAR[len];
		archive.Load((uint8*)m_name, len * sizeof(MBCHAR));

		sint32 const    ressourceNum    = m_collectingResources.GetNum();

		// TODO: Clean up this mess later, and test what would happen when the good
		// database size would decrease, instead of increase.

		if(ressourceNum == g_theResourceDB->NumRecords())
		{
			m_distanceToGood = new sint32[g_theResourceDB->NumRecords()];
			archive.Load((uint8*)m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
		}
		else
		{ // Fix trade if the good database was increased in size.

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
			for(j = 0; j < ressourceNum; ++j)
			{
				for(i = 0; i < g_theResourceDB->NumRecords(); ++i)
				{
					// This does not work for the nuclear detente scenario
		//			DPRINTF(k_DBG_GAMESTATE, ("%s: GoodDistNew: %i, GoodDistOld: \t%i, \t%i, \t%i\n", m_name, tmpDistanceToGood[j], m_distanceToGood[i], j, i));
					if(tmpDistanceToGood[j] > 0 && tmpDistanceToGood[j] == m_distanceToGood[i])
					{
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

			if(g_newGoods == NULL)
			{
				// Needs only be done once per reload
				g_newGoods = new sint32[ressourceNum]; // Deleted in gameinit_Initialize
				memset(g_newGoods, 0, ressourceNum * sizeof(sint32));
				for(j = 0; j < ressourceNum; ++j)
				{
					for(i = 0; i < g_theResourceDB->NumRecords(); ++i)
					{
						if(tmpDistanceToGood[j] > 0 && tmpDistanceToGood[j] == m_distanceToGood[i])
						{
							g_newGoods[j] = i;
							break;
						}
					}
				}
			}

			delete[] tmpDistanceToGood;
		}

		archive.Load((uint8*)&m_defensiveBonus, sizeof(double)); // Should be GetDOUBLE

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
// Returns    : true if center_point is in water terrain and doesn't have a tunnel
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool NeedsCanalTunnel(MapPoint const & center_point)
{
	if (g_theWorld->IsCanal(center_point))
	{
		return false;
	}
	else if (g_theWorld->IsTunnel(center_point))
	{
		return false;
	}

	TerrainRecord const * rec =
	    g_theTerrainDB->Get(g_theWorld->GetTerrainType(center_point));

	return rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater();
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

	if (g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.AddCreatedObject(m_home_city.AccessData());
	}

	sint32 name;
	Civilisation *civ = g_player[m_owner]->GetCivilisation();
	CivilisationData *civData = g_theCivilisationPool->AccessData(*civ);
	if(g_player[m_owner]->GetNumCities() == 0)
	{
		name = civData->GetCapitalName();
	}
	else
		name = civData->GetAnyCityName();

	if(name != k_CITY_NAME_UNDEFINED)
	{
		MBCHAR s[k_MAX_NAME_LEN];
		civData->GetCityName(name, s);
		civData->UseCityName(name);
		SetName(s);
	}
	else
	{
		SetName(GetName());
	}

	FindBestSpecialists();
	//SectarianHappiness(); //emod  - does this initialize it?
	//m_secthappy = 0; //did not initialize

	sint32 martialLaw;
	m_happy->CalcHappiness(*this, false, martialLaw, true);

	const UnitRecord * settlerRec   = NULL;
	sint32 numPops = 1;
	if(settlerType >= 0)
	{
		settlerRec = g_theUnitDB->Get(settlerType, g_player[m_owner]->GetGovernmentType());
		if(settlerRec)
			numPops = settlerRec->GetSettleSize();
	}
	else
	{
		//Added by Martin  Gühmann to make sure that also cities
		//created by the Scenario editor have a size
		if(settlerType == -2 && ScenarioEditor::PlaceCityMode() && ScenarioEditor::CitySize() > 0)
			numPops = ScenarioEditor::CitySize();
	}

	//Added by Martin  Gühmann to make sure that also cities created by the editor
	//have a size.
	if((settlerType != -2) || ScenarioEditor::PlaceCityMode())
	{
		for(sint32 i = 0; i < numPops; i++)
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_MakePop,
			                       GEA_City, m_home_city,
			                       GEA_End);
		}
	}

	if(settlerRec && settlerRec->GetNumSettleBuilding() > 0)
	{
		for(sint32 i = 0; i < settlerRec->GetNumSettleBuilding(); i++)
		{
			sint32 bi = settlerRec->GetSettleBuildingIndex(i);
			Assert(bi >= 0);
			Assert(bi < g_theBuildingDB->NumRecords());
			if(bi >= 0 && bi < g_theBuildingDB->NumRecords())
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateBuilding,
				                       GEA_City, m_home_city.m_id,
				                       GEA_Int, bi,
				                       GEA_End);
			}
		}
	}

	//add 	DynamicArray<Religion> ReligionArray; //why doesn't cities use like player.cpp -> m_allInstallations = new DynamicArray<Installation>; ????

	Cell *cell = g_theWorld->GetCell(center_point);
	if(cell->GetEnv() & k_BIT_ENV_INSTALLATION)
	{
		DynamicArray<Installation> instArray;
		if(g_theInstallationTree->GetAt(center_point, instArray))
		{
			for(sint32 i = instArray.Num() - 1; i >= 0; i--)
			{
				instArray[i].Kill();
			}
		}
	}

	sint32 good;
	if(g_theWorld->GetGood(center_point, good))
	{
		if(CanCollectGood(good))
		{
			//EMOD 5-01-2006 to prevent free collection of goods
			m_collectingResources.AddResource(good);
		}
	}

	cell->SetEnv(cell->GetEnv() & ~(k_MASK_ENV_IRRIGATION |
	                                k_MASK_ENV_MINE |
	                                k_MASK_ENV_ROAD |
	                                k_MASK_ENV_INSTALLATION));
	SetRoad();

	if (NeedsCanalTunnel(center_point))
	{
		g_theWorld->SetCanalTunnel(center_point, true);
	}

	if (g_theWorld->IsWater(center_point))
	{
		g_theWorld->NumberContinents();
	}
	else if (g_theWorld->IsNextToWater(center_point.x, center_point.y))
	{
		g_theWorld->NumberContinents();
	}

	g_tiledMap->PostProcessTile(center_point, g_theWorld->GetTileInfo(center_point));
	g_tiledMap->TileChanged(center_point);
	MapPoint pos;

	for(WORLD_DIRECTION d = NORTH; d < NOWHERE; d = (WORLD_DIRECTION)((sint32)d + 1))
	{
		if(center_point.GetNeighborPosition(d, pos))
		{
			g_tiledMap->PostProcessTile(pos, g_theWorld->GetTileInfo(pos));
			g_tiledMap->TileChanged(pos);
		}
	}
	g_tiledMap->RedrawTile(&center_point);

	g_network.Enqueue(g_theWorld->GetCell(center_point),
					  center_point.x, center_point.y);

	//Added by Martin  Gühmann to make sure that cities created
	//by the scenario editor keep their style
	if((settlerType == CITY_STYLE_EDITOR) && ScenarioEditor::PlaceCityMode())
	{
		m_cityStyle = ScenarioEditor::CityStyle();
	}

	// Gives all starting age buildings to a new city.
	if(g_network.IsActive() && g_network.GetStartingAge() > 0)
	{
		for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)i, m_owner), m_owner))
				continue;

			if(!CanBuildBuilding(i))
				continue;

			sint32 enable = buildingutil_Get(i, m_owner)->GetEnableAdvanceIndex();
			if(g_theAdvanceDB->Get(enable, g_player[m_owner]->GetGovernmentType())->GetAgeIndex() < g_network.GetStartingAge())
			{
				m_built_improvements |= (uint64)1 << (uint64)i;
			}
		}
	}
	else
	{
		if (g_theProfileDB->GetSPStartingAge() > 0)
		{
			for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
			{
				if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)i, m_owner), m_owner))
					continue;

				if(!CanBuildBuilding(i))
					continue;

				sint32 enable = buildingutil_Get(i, m_owner)->GetEnableAdvanceIndex();
				if(g_theAdvanceDB->Get(enable, g_player[m_owner]->GetGovernmentType())->GetAgeIndex() < g_theProfileDB->GetSPStartingAge())
				{
					m_built_improvements |= (uint64)1 << (uint64)i;
				}
			}
		}
	}

	FindBestSpecialists();

	CalculateGrowthRate();

	FindGoodDistances();
	//this uses the ConstDB as the starting citysize. should this look to citysize instead? - E Aug 30, 2007
	GenerateBorders(center_point, m_owner, g_theConstDB->Get(0)->GetBorderIntRadius(), g_theConstDB->Get(0)->GetBorderSquaredRadius());

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

	m_sentInefficientMessageAlready = false;
	Serialize(archive) ;

	m_bonusFood      = 0.0;
	m_bonusFoodCoeff = 0.0;

	//m_secthappy = 0; //emod  - didnot initialize
}

CityData::CityData(CityData *copy)
: m_name(NULL)
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

	//m_secthappy = 0; //emod - this set all sectarian happiness to 0 this is initializer?

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
	if(g_network.IsActive())
	{
		sint32 i;
		for(i = copy->m_tradeSourceList.Num() - 1; i >= 0; i--)
		{
			if(!g_theTradePool->IsValid(copy->m_tradeSourceList[i]))
			{
				copy->m_tradeSourceList.DelIndex(i);
			}
		}

		for(i = copy->m_tradeDestinationList.Num() - 1; i >= 0; i--)
		{
			if(!g_theTradePool->IsValid(copy->m_tradeDestinationList[i]))
			{
				copy->m_tradeDestinationList.DelIndex(i);
			}
		}
	}

	m_owner                              = copy->m_owner;
	m_slaveBits                          = copy->m_slaveBits;
	m_accumulated_food                   = copy->m_accumulated_food;
	m_shieldstore                        = copy->m_shieldstore;
	m_shieldstore_at_begin_turn          = copy->m_shieldstore_at_begin_turn;
	m_build_category_at_begin_turn       = copy->m_build_category_at_begin_turn;
	m_net_gold                           = copy->m_net_gold;
	m_gold_lost_to_crime                 = copy->m_gold_lost_to_crime;
	m_gross_gold                         = copy->m_gross_gold;
	m_goldFromTradeRoutes                = copy->m_goldFromTradeRoutes;
	m_goldLostToPiracy                   = copy->m_goldLostToPiracy;
	m_science                            = copy->m_science;
	m_luxury                             = copy->m_luxury;
	m_city_attitude                      = copy->m_city_attitude;
	m_collected_production_this_turn     = copy->m_collected_production_this_turn;
	m_gross_production                   = copy->m_gross_production;
	m_net_production                     = copy->m_net_production;
	m_production_lost_to_crime           = copy->m_production_lost_to_crime;
	m_built_improvements                 = copy->m_built_improvements;
	m_builtWonders                       = copy->m_builtWonders;
	m_food_delta                         = copy->m_food_delta;
	m_gross_food                         = copy->m_gross_food;
	m_net_food                           = copy->m_net_food;
	m_food_lost_to_crime                 = copy->m_food_lost_to_crime;
	m_food_consumed_this_turn            = copy->m_food_consumed_this_turn;
	m_total_pollution                    = copy->m_total_pollution;
	m_cityPopulationPollution            = copy->m_cityPopulationPollution;
	m_cityIndustrialPollution            = copy->m_cityIndustrialPollution;
	m_goldFromTransitRoutes              = copy->m_goldFromTransitRoutes;
	m_cityPollutionCleaner               = copy->m_cityPollutionCleaner;
	m_contribute_materials               = copy->m_contribute_materials;
	m_contribute_military                = copy->m_contribute_military;
	m_capturedThisTurn                   = copy->m_capturedThisTurn;
	m_spied_upon                         = copy->m_spied_upon;
	m_walls_nullified                    = copy->m_walls_nullified;
	m_franchise_owner                    = copy->m_franchise_owner;
	m_franchiseTurnsRemaining            = copy->m_franchiseTurnsRemaining;
	m_watchfulTurns                      = copy->m_watchfulTurns;
	m_bioInfectionTurns                  = copy->m_bioInfectionTurns;
	m_bioInfectedBy                      = copy->m_bioInfectedBy;
	m_nanoInfectionTurns                 = copy->m_nanoInfectionTurns;
	m_nanoInfectedBy                     = copy->m_nanoInfectedBy;
	m_convertedTo                        = copy->m_convertedTo;
	m_convertedGold                      = copy->m_convertedGold;
	m_convertedBy                        = copy->m_convertedBy;
	m_terrainWasPolluted                 = copy->m_terrainWasPolluted;
	m_happinessAttackedBy                = copy->m_happinessAttackedBy;
	m_terrainImprovementWasBuilt         = copy->m_terrainImprovementWasBuilt;
	m_improvementWasBuilt                = copy->m_improvementWasBuilt;
	m_isInjoined                         = copy->m_isInjoined;
	m_injoinedBy                         = copy->m_injoinedBy;
	m_airportLastUsed                    = copy->m_airportLastUsed;
	m_founder                            = copy->m_founder;
	m_wages_paid                         = copy->m_wages_paid;
	m_pw_from_infrastructure             = copy->m_pw_from_infrastructure;
	m_gold_from_capitalization           = copy->m_gold_from_capitalization;
	m_buildInfrastructure                = copy->m_buildInfrastructure;
	m_buildCapitalization                = copy->m_buildCapitalization;
	m_paidForBuyFront                    = copy->m_paidForBuyFront;
	m_doUprising                         = copy->m_doUprising;
	m_turnFounded                        = copy->m_turnFounded;
	m_productionLostToFranchise          = copy->m_productionLostToFranchise;
	m_probeRecoveredHere                 = copy->m_probeRecoveredHere;
	m_lastCelebrationMsg                 = copy->m_lastCelebrationMsg;
	m_alreadySoldABuilding               = copy->m_alreadySoldABuilding;
	m_population                         = copy->m_population;
	m_partialPopulation                  = copy->m_partialPopulation;
	m_sizeIndex                          = copy->m_sizeIndex;
	m_workerFullUtilizationIndex         = copy->m_workerFullUtilizationIndex;
	m_workerPartialUtilizationIndex      = copy->m_workerPartialUtilizationIndex;
	m_useGovernor                        = copy->m_useGovernor;
	m_buildListSequenceIndex             = copy->m_buildListSequenceIndex;
	m_garrisonOtherCities                = copy->m_garrisonOtherCities;
	m_garrisonComplete                   = copy->m_garrisonComplete;
	m_currentGarrison                    = copy->m_currentGarrison;
	m_neededGarrison                     = copy->m_neededGarrison;
	m_currentGarrisonStrength            = copy->m_currentGarrisonStrength;
	m_neededGarrisonStrength             = copy->m_neededGarrisonStrength;
	m_sellBuilding                       = copy->m_sellBuilding;
	m_buyFront                           = copy->m_buyFront;
	m_max_food_from_terrain              = copy->m_max_food_from_terrain;
	m_max_prod_from_terrain              = copy->m_max_prod_from_terrain;
	m_max_gold_from_terrain              = copy->m_max_gold_from_terrain;
	m_growth_rate                        = copy->m_growth_rate;
	m_overcrowdingCoeff                  = copy->m_overcrowdingCoeff;
	m_starvation_turns                   = copy->m_starvation_turns;
	m_cityStyle                          = copy->m_cityStyle;
	m_pos                                = copy->m_pos;
	m_is_rioting                         = copy->m_is_rioting;

	memcpy(&m_numSpecialists,    &copy->m_numSpecialists,    sizeof(copy->m_numSpecialists));
	memcpy(&m_specialistDBIndex, &copy->m_specialistDBIndex, sizeof(copy->m_specialistDBIndex));

	m_min_turns_revolt = copy->m_min_turns_revolt;

	m_home_city = copy->m_home_city;

	// Let's copy the buld queue the old way. Anyway, this CityData copying looks very dubious
	m_build_queue = copy->m_build_queue;
	m_build_queue.SetName(copy->m_build_queue.GetName());
	m_tradeSourceList = copy->m_tradeSourceList;
	m_tradeDestinationList = copy->m_tradeDestinationList;

	// To check later: next 3 lines are invalid when CTP1_TRADE has been defined. See CityData.h.
	m_collectingResources = copy->m_collectingResources;
	m_buyingResources = copy->m_buyingResources;
	m_sellingResources = copy->m_sellingResources;

	m_happy->Copy(copy->m_happy);

	SetNameLocal(copy->m_name);
	memcpy(m_distanceToGood, copy->m_distanceToGood, sizeof(sint32) * g_theResourceDB->NumRecords());
	m_defensiveBonus = copy->m_defensiveBonus;

	m_shieldstore = copy->m_shieldstore;

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

	m_secthappy = 0; //emod - didn't crash but always set to 0 at -1 it showed up but begin turn didn't work nor did it process

	if(this == m_home_city.CD())
	{
		if(g_network.IsHost())
		{
			g_network.Enqueue(this);
		}
		else if(g_network.IsClient())
		{
			g_network.SendCity(this);
		}
	}

	m_bonusFoodCoeff = copy->m_bonusFoodCoeff;
	m_bonusFood      = copy->m_bonusFood;
	m_bonusProdCoeff = copy->m_bonusProdCoeff;
	m_bonusProd      = copy->m_bonusProd;
	m_bonusGoldCoeff = copy->m_bonusGoldCoeff;
	m_bonusGold      = copy->m_bonusGold;
	m_bonusScieCoeff = copy->m_bonusScieCoeff;
	m_bonusScie      = copy->m_bonusScie;
}

bool CityData::IsACopy()
{
	if(!m_home_city.IsValid())
		return true;

	return this != m_home_city.CD();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::IsBankrupting
//
// Description: Determines whether the current city maintenance costs would
//              cause a negative amount of gold.
//
// Parameters : -
//
// Globals    : g_player    : player data
//
// Returns    : bool        : current maintenance costs can not be afforded
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool CityData::IsBankrupting(void) const
{
	return (m_net_gold < 0) && (g_player[m_owner]->GetGold() < -m_net_gold);
}

void CityData::PrepareToRemove(const CAUSE_REMOVE_ARMY cause,
                               PLAYER_INDEX killedBy)
{
	uint64 wonders = m_builtWonders;
	for (sint32 i = 0; wonders; ++i)
	{
		if (wonders & 1)
		{
			g_player[m_owner]->RemoveWonder(i, true);
		}
		wonders >>= 1;
	}

	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);
	for(it.Start(); !it.End(); it.Next())
	{
		if(it.Pos() == m_home_city.RetPos()) continue;
		g_theWorld->GetCell(it.Pos())->SetCityOwner(Unit());
	}

	KillAllTradeRoutes(); // to ensure RemoveSeenByBit is executed before city owner cannot be determined any more

	// Change influence of neighbor cities
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
// Returns    : bool true if the city should revolt, i.e.,
//              m_happiness < g_theConstDB->GetRevolutionLevel()+incite_bonus
//
// Remark(s)  : A city will not revolt
//              - when playing the tutorial
//              - when the civilisation is immune to revolts
//              - when it has just revolted
//              Otherwise, the revolt risk is determined by the happiness.
//
//----------------------------------------------------------------------------
bool CityData::ShouldRevolt(const sint32 inciteBonus)
{
	if
	  (
	       g_slicEngine->GetTutorialActive()
	    && m_owner == g_slicEngine->GetTutorialPlayer()
	  )
	{
		return false;
	}

	if(m_home_city.Flag(k_UDF_CANT_RIOT_OR_REVOLT))
		return false;

	// Modified by kaan to address bug # 12
	if (m_min_turns_revolt != 0)
		return false;

	return m_happy->ShouldRevolt(inciteBonus);
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
	if(m_min_turns_revolt > 0)
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
//              bool causeIsExternal    :
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : - causeIsExternal defaults to false in citydata.h
//            : - Sometimes a city constantly revolts needs fixing
//
//----------------------------------------------------------------------------
void CityData::Revolt(sint32 &playerToJoin, bool causeIsExternal)
{
	sint32 i,
	       n;

	PLAYER_INDEX const  orgowner    = m_owner;

	CellUnitList army;

	MapPoint     city_pos,
	             new_city;

	bool         joined_egalatarians = false;

	g_player[m_owner]->m_score->AddRevolution();

	m_home_city.GetPos(city_pos);

	if(!causeIsExternal && g_network.IsActive()) {
		g_network.Block(m_owner);
	}

	PLAYER_INDEX        newowner    = PLAYER_UNASSIGNED;

	if(wonderutil_GetRevoltingCitiesJoinPlayer(g_theWonderTracker->GetBuiltWonders()))
	{
		for(sint32 p = 0; p < k_MAX_PLAYERS; p++)
		{
			if(g_player[p])
			{
				if(wonderutil_GetRevoltingCitiesJoinPlayer(g_player[p]->m_builtWonders))
				{
					newowner = p;

					if(newowner == m_owner)
					{
						newowner = PLAYER_UNASSIGNED;
					}
					else
					{
						joined_egalatarians = true;
					}

					break;
				}
			}
		}
	}

	if(newowner < 0)
	{
		if(playerToJoin < 0)
		{
			newowner = civilisation_NewCivilisationOrVandals(m_owner);
			playerToJoin = newowner;
		}
		else
		{
			newowner = playerToJoin;
		}
	}

	if (orgowner != newowner)
	{
		if(g_network.IsHost())
		{
			g_network.Block(orgowner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_REVOLT_NOTICES,
			                              orgowner, newowner,
			                              m_home_city.m_id,
			                              joined_egalatarians));
			g_network.Unblock(orgowner);
		}

		// Need to fix script.slc and info.txt though
		// Need to make it more generic - E 9-6-2006
		if(joined_egalatarians)
		{
			SlicObject *so = new SlicObject("011CityJoinedYourCiv");
			so->AddRecipient(newowner);
			so->AddCivilisation(orgowner);
			so->AddCivilisation(newowner);
			so->AddCity(m_home_city);
			g_slicEngine->Execute(so);
		}
		else
		{
			SlicObject *so = new SlicObject("010NewCiv");
			so->AddAllRecipients();
			so->AddCivilisation(orgowner);
			so->AddCivilisation(newowner);
			so->AddCity(m_home_city);
			g_slicEngine->Execute(so);
		}
	}

	if(!causeIsExternal && g_network.IsActive()) {
		g_network.Unblock(m_owner);
	}

	m_home_city.ResetCityOwner(newowner, false, CAUSE_REMOVE_CITY_HAPPINESS_REVOLT);
	m_build_queue.Clear();

#if 0
	AddHappyTimer(50, 100, HAPPY_REASON_POST_REVOULTION_BLISS);
#endif

	g_theWorld->GetCell(city_pos)->GetArmy(army);
	n = army.Num();
	for (i=0; i<n; i++)
	{
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillUnit,
		                       GEA_Unit, army[i].m_id,
		                       GEA_Int, CAUSE_REMOVE_ARMY_UPRISING,
		                       GEA_Player, newowner,
		                       GEA_End);
	}

	SpecialAttackInfoRecord const * specRec =
	    unitutil_GetSpecialAttack(SPECATTACK_REVOLUTION);

	if (specRec)
	{
		sint32 const soundID = specRec->GetSoundIDIndex();

		if (soundID >= 0)
		{
			sint32 const spriteID = specRec->GetSpriteID()->GetValue();

			if (spriteID >= 0)
			{
				g_director->AddSpecialAttack
				    (m_home_city.GetActor()->GetUnitID(), m_home_city, SPECATTACK_REVOLUTION);
			}
			else
			{
				sint32 const visiblePlayer = g_selected_item->GetVisiblePlayer();
				if ((visiblePlayer == m_owner) ||
				    (m_home_city.GetVisibility() & (1 << visiblePlayer))
				   )
				{
					g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID,
					                         m_home_city.RetPos().x, m_home_city.RetPos().y);
				}
			}
		}
	}

	// Modified by kaan to address bug # 12
	// Prevent city from revolting twice in the same turn.
	m_min_turns_revolt = static_cast<uint8>(g_theConstDB->Get(0)->GetMinTurnsBetweenRevolts());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::TeleportUnits
//
// Description:
//
// Parameters : MapPoint &pos                 : destination
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
void CityData::TeleportUnits(const MapPoint &pos, sint32 foreigner)
{
	sint32 i;
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());

	DynamicArray<Army> armies;

	for(i = 0; i < cell->GetNumUnits(); i++)
	{
		if(armies.Find(cell->AccessUnit(i).GetArmy()) >= 0)
		{

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

	CellUnitList	units;
	MapPoint city_pos;

	sint32   i,
	         n;

	m_home_city.GetPos(city_pos);
	g_theWorld->GetCell(city_pos)->GetArmy(units);
	n = units.Num();

	DynamicArray<Army> moveArmies;

	for (i=0; i<n; i++){
		MapPoint oldpos;

		units[i].GetPos(oldpos);

		g_theWorld->RemoveUnitReference(oldpos, units[i]);
		units[i].SetPosition(pos);

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
	if (!g_theGameSettings->GetPollution())
	{
		m_cityPopulationPollution   = 0;
		m_cityIndustrialPollution   = 0;
		m_total_pollution           = 0;
		return;
	}

	sint32 populationPolluting = PopCount() -
		g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetPollutionStartPopulationLevel();

	if (populationPolluting <= 0)
	{
		populationPolluting = 0;
	}
	else
	{
		populationPolluting = (sint32)(populationPolluting *
			g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetPollutionPopulationRatio());

		populationPolluting = (sint32)(populationPolluting * g_player[m_owner]->GetPollutionCoef());
	}

	sint32 productionPolluting = m_gross_production -
		g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetPollutionStartProductionLevel();
	if (productionPolluting <= 0)
	{
		productionPolluting = 0;
	}
	else
	{
		productionPolluting = (sint32)(productionPolluting *
			g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetPollutionProductionRatio());

		productionPolluting = (sint32)(productionPolluting * g_player[m_owner]->GetPollutionCoef());
	}

	double buildingPollution=0.0;
	double buildingProductionPercentage=0.0;
	double buildingPopulationPercentage=0.0;
	sint32 i;
	double temp;

	for(i=0; i<g_theBuildingDB->NumRecords(); i++)
	{
		if(HasEffectiveBuilding(i))
		{
			if(buildingutil_Get(i, m_owner)->GetPollutionAmount(temp))
			{
				buildingPollution+=temp;
			}
			if(buildingutil_Get(i, m_owner)->GetProductionPollutionPercent(temp))
			{
				buildingProductionPercentage+=temp;
			}
			if(buildingutil_Get(i, m_owner)->GetPopulationPollutionPercent(temp))
			{
				buildingPopulationPercentage+=temp;
			}
		}
	}

	for(i=0; i<g_theWonderDB->NumRecords(); i++)
	{
		if(HasCityWonder(i)) // This seems to be supposed to be city only.
		{
			if(wonderutil_Get(i, m_owner)->GetPollutionAmount(temp))
			{
				buildingPollution+=temp;
			}
			if(wonderutil_Get(i, m_owner)->GetPollutionPercent(temp))
			{
				buildingProductionPercentage+=temp;
				buildingPopulationPercentage+=temp;
			}
		}
	}

	m_cityPopulationPollution   = populationPolluting;
	m_cityIndustrialPollution   = productionPolluting;
	m_total_pollution           =
	            populationPolluting + productionPolluting +
	            static_cast<sint32>(populationPolluting * buildingPopulationPercentage) +
	            static_cast<sint32>(productionPolluting * buildingProductionPercentage) +
	            static_cast<sint32>(buildingPollution);
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

	if(m_total_pollution < g_theConstDB->Get(0)->GetLocalPollutionLevel())
		return;

	double diff = double(m_total_pollution) - g_theConstDB->Get(0)->GetLocalPollutionLevel();
	double chance = diff * g_theConstDB->Get(0)->GetLocalPollutionChance();

	if ((chance > 0.10) &&
		(g_slicEngine->GetSegment("080CityPollutionWarning")->TestLastShown(m_owner, 10)))
	{
		SlicObject *so = new SlicObject("080CityPollutionWarning");
		so->AddCity(m_home_city);
		so->AddRecipient(m_owner);
		so->AddCivilisation(m_owner);
		g_slicEngine->Execute(so);
	}

	if(g_rand->Next(1000) < chance * 1000)
	{
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
	if(m_bioInfectionTurns > 0)
	{
		gross_production -= static_cast<sint32>(ceil(gross_production * g_theConstDB->Get(0)->GetBioInfectionProductionCoef()));
	}

	crime_loss = CrimeLoss(gross_production);

	double net_production = gross_production - crime_loss;

	if(m_franchise_owner >= 0)
		franchise_loss = static_cast<sint32>(ceil(net_production * g_theConstDB->Get(0)->GetFranchiseEffect()));
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
sint32 CityData::ProcessProduction(bool projectedOnly, sint32 &grossProduction, sint32 &collectedProduction, sint32 &crimeLoss, sint32 &franchiseLoss, bool considerOnlyFromTerrain)// const
{
	double gross_production  = collectedProduction;
	gross_production += m_bonusProd;

	// end EMOD
	// Moved this above gov coefficient so laborers are included, so they work like all the other specialists.
	if(!considerOnlyFromTerrain)
	{
		gross_production += LaborerCount() * GetSpecialistsResources(POP_LABORER);
	}

	gross_production *= GetBonusProdCoeff();

	grossProduction = static_cast<sint32>(gross_production);
	CalcPollution();
	m_happy->RecalcPollutionHappiness(*this, g_player[m_owner]);

	grossProduction = ComputeProductionLosses(static_cast<sint32>(gross_production), crimeLoss, franchiseLoss);

	sint32 const shields			= grossProduction - (crimeLoss + franchiseLoss);

	if (m_franchise_owner >= 0)
	{
		if(!projectedOnly)
			g_player[m_franchise_owner]->AddProductionFromFranchise(franchiseLoss);
	}

	return shields;
}
#endif

// used in NewTurnCount::VerifyEndTurn
double CityData::ProjectMilitaryContribution()
{
	return static_cast<double>(m_net_production);
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

	if(m_contribute_military)
	{
		mil_paid  = (sint32)(ceil((double)m_net_production * percent_military));
		m_net_production -= mil_paid;
	}
	else
	{
		mil_paid = 0;
	}

	mat_paid = ComputeMaterialsPaid(percent_terrain);
	if(m_contribute_materials)
	{
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
			Assert(0);
			return;
		}
	}
#endif

	m_net_production -= mil_paid;

	mat_paid = ComputeMaterialsPaid(percent_mat);
	m_net_production -= mat_paid;

	Assert (0 <= m_net_production);
	if (m_net_production < 0)
	{
		m_net_production = 0;
	}
}

void CityData::AddShieldsToBuilding()
{
	if(m_net_production < 0)
	{
		m_net_production = 0;
	}

	if(m_is_rioting)
	{
		m_net_production = 0;
	}

	// Do not add shields to the store when the build-queue is empty.
	if (m_build_queue.GetHead() != NULL) {
		SetShieldstore(m_shieldstore + m_net_production);
	}
}

#if !defined(NEW_RESOURCE_PROCESS)
void CityData::GetFullAndPartialRadii(sint32 &fullRadius, sint32 &partRadius) const
{
	CitySizeRecord const *  fullRec = NULL;
	if (m_workerFullUtilizationIndex >= 0)
	{
		fullRec = g_theCitySizeDB->Get(m_workerFullUtilizationIndex);
	}

	fullRadius  = (fullRec) ? fullRec->GetSquaredRadius() : 0;

	CitySizeRecord const *  partRec = NULL;
	if ((m_workerPartialUtilizationIndex >= 0) &&
	    (m_workerPartialUtilizationIndex < g_theCitySizeDB->NumRecords())
	   )
	{
		partRec = g_theCitySizeDB->Get(m_workerPartialUtilizationIndex);
	}

	partRadius  = (partRec) ? partRec->GetSquaredRadius() : 0;
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
	size_t const    maxRing = static_cast<size_t>(g_theCitySizeDB->NumRecords());

	std::fill(m_ringFood,       m_ringFood      + maxRing,  0);
	std::fill(m_ringProd,       m_ringProd      + maxRing,  0);
	std::fill(m_ringGold,       m_ringGold      + maxRing,  0);
	std::fill(m_ringSizes,      m_ringSizes     + maxRing,  0);

#if defined(NEW_RESOURCE_PROCESS)
	std::fill(m_farmersEff,     m_farmersEff    + maxRing,  0);
	std::fill(m_laborersEff,    m_laborersEff   + maxRing,  0);
	std::fill(m_merchantsEff,   m_merchantsEff  + maxRing,  0);
	std::fill(m_scientistsEff,  m_scientistsEff + maxRing,  0);
#endif

// Add if city has building GetEnablesGood >0 then that good will be added to the city for trade

	for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++)
	{
		if(HasBuilding(b)) // Should this include buidlings given by wonders?
		{
			const BuildingRecord *rec = buildingutil_Get(b, m_owner);
	//		Check If needsGood for the building a make bonuses dependent on having that good for further bonus
			if((rec->GetNumEnablesGood() > 0) && (IsBuildingOperational(b)))
			{
				for(sint32 good = 0; good < rec->GetNumEnablesGood(); good++)
				{
					m_collectingResources.AddResource(rec->GetEnablesGoodIndex(good));
				}
			}
		}
	}

// end building enables good

	// Add if city has wonder GetEnablesGood > 0 then that good will be added to the city for trade
	for(sint32 w = 0; w < g_theWonderDB->NumRecords(); w++)
	{
		if(HasCityWonder(w)) // Check whether this should be empirewide.
		{
			const WonderRecord *wrec = wonderutil_Get(w, m_owner);
//			Check If needsGood for the building a make bonuses dependent on having that good for further bonus
			if(wrec->GetNumEnablesGood() > 0)
			{
				for(sint32 good = 0; good < wrec->GetNumEnablesGood(); good++)
				{
					m_collectingResources.AddResource(wrec->GetEnablesGoodIndex(good));
				}
			}
		}
	}

	CityInfluenceIterator it(cityPos, m_sizeIndex);
	for(it.Start(); !it.End(); it.Next())
	{
		Cell *cell = g_theWorld->GetCell(it.Pos());
		sint32 ring = GetRing(it.Pos());
		m_ringFood[ring] += cell->GetFoodProduced();
		m_ringProd[ring] += cell->GetShieldsProduced();
		m_ringGold[ring] += cell->GetGoldProduced();
		m_ringSizes[ring]++;
		sint32 good;
		if(g_theWorld->GetGood(it.Pos(), good))
		{
			//if(g_theResourceDB->Get(good)->GetCantTrade() == 0){
			if(CanCollectGood(good)){
			//EMOD 4-26-2006 to prevent free collection of goods
				m_collectingResources.AddResource(good);
			}
		}
		// Added by E (10-29-2005) - If a tileimp has enablegood then give to city
		for(sint32 i = 0; i < cell->GetNumDBImprovements(); i++)
		{
			sint32 imp = cell->GetDBImprovement(i);
			const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
			if (rec->GetNumEnablesGood() > 0)
			{
				for(good = 0; good < rec->GetNumEnablesGood(); good++)
				{
					m_collectingResources.AddResource(rec->GetEnablesGoodIndex(good));
				}
			}
		}
		//EMOD enablesgood applied to terrain::effect
		for(sint32 t = 0; t < cell->GetNumDBImprovements(); t++)
		{
			sint32 timp = cell->GetDBImprovement(t);
			const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(timp);
			const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(trec, it.Pos());
			if (effect)
			{
				for (sint32 tgood = 0; tgood < effect->GetNumEnablesGood(); tgood++)
				{
					m_collectingResources.AddResource(effect->GetEnablesGoodIndex(tgood));
				}
			}
		}
	}

	CollectResourcesFinally();
}

void CityData::CollectResourcesFinally()
{
#if !defined(NEW_RESOURCE_PROCESS)
	sint32 fullSquaredRadius;
	sint32 partSquaredRadius;

	GetFullAndPartialRadii(fullSquaredRadius, partSquaredRadius);
#endif

	sint32 fullFoodTerrainTotal = 0;
	sint32 partFoodTerrainTotal = 0;

	sint32 fullProdTerrainTotal = 0;
	sint32 partProdTerrainTotal = 0;

	sint32 fullGoldTerrainTotal = 0;
	sint32 partGoldTerrainTotal = 0;

	size_t const    maxRing = static_cast<size_t>(g_theCitySizeDB->NumRecords());

#if defined(NEW_RESOURCE_PROCESS)
	for (size_t i = 0; i < maxRing; ++i)
	{
		DPRINTF(k_DBG_GAMESTATE, ("Food: %d, Prod: %d, Gold: %d\n", m_ringFood[i], m_ringProd[i], m_ringGold[i]));
		m_max_food_from_terrain += m_ringFood[i];
		m_max_prod_from_terrain += m_ringProd[i];
		m_max_gold_from_terrain += m_ringGold[i];
	}

#else // Should go next time
	for (sint32 i = 0; i <= m_workerFullUtilizationIndex; ++i)
	{
		fullFoodTerrainTotal += m_ringFood[i];
		fullProdTerrainTotal += m_ringProd[i];
		fullGoldTerrainTotal += m_ringGold[i];
	}
	partFoodTerrainTotal = m_ringFood[m_workerPartialUtilizationIndex];
	partProdTerrainTotal = m_ringProd[m_workerPartialUtilizationIndex];

	if (m_is_rioting)
	{
		// No Gold at all.
		partGoldTerrainTotal = 0;
		fullGoldTerrainTotal = 0;
	}
	else
	{
		partGoldTerrainTotal = m_ringGold[m_workerPartialUtilizationIndex];
	}

	double const	utilizationRatio	= GetUtilisationRatio(partSquaredRadius);

	m_max_food_from_terrain = fullFoodTerrainTotal + partFoodTerrainTotal;
	m_max_prod_from_terrain = fullProdTerrainTotal + partProdTerrainTotal;
	m_max_gold_from_terrain = fullGoldTerrainTotal + partGoldTerrainTotal;

	m_gross_food = fullFoodTerrainTotal + ceil(utilizationRatio * double(partFoodTerrainTotal));
	m_gross_production = (sint32)(fullProdTerrainTotal + ceil(utilizationRatio * double(partProdTerrainTotal)));
	m_gross_gold = (sint32)(fullGoldTerrainTotal + ceil(utilizationRatio * double(partGoldTerrainTotal)));

	m_gross_food += FoodFromTrade();
	m_gross_production += ProdFromTrade();
	m_gross_gold += GoldFromTrade();

	m_collected_production_this_turn = m_gross_production;

	/* Why is gross production passed to net here?
	 This is why GetNetCityProduction sometimes returns gross production
	 if ProcessProduction has not fired afterward.
	 Put it back if it causes problems.-Maq 29-Jul-2009 */
	//m_net_production = m_gross_production;

#endif
	// Added for new Empire Manager details
	m_gross_food_before_bonuses = m_gross_food;
	m_gross_prod_before_bonuses = m_gross_production;
	//m_gross_gold_before_bonuses = m_gross_gold;// raw commerce has no bonuses
	// End - Added for new Empire manager details
	m_net_food = m_gross_food;
	m_net_gold = m_gross_gold;
}

sint32 CityData::FoodFromTrade(){
    sint32 food= 0;

    for(sint32 i = 0; i < m_tradeDestinationList.Num(); i++){
	TradeRoute route = m_tradeDestinationList[i];
	if(route.IsActive()){
	    ROUTE_TYPE routeType;
	    sint32 g;
	    route.GetSourceResource(routeType, g);
	    if(!route->IsBeingPirated()){
		food += g_theResourceDB->Get(g)->GetFood();
		}
	    }
	}

    return(food);
    }

sint32 CityData::ProdFromTrade(){
    sint32 prod= 0;

    for(sint32 i = 0; i < m_tradeDestinationList.Num(); i++){
	TradeRoute route = m_tradeDestinationList[i];
	if(route.IsActive()){
	    ROUTE_TYPE routeType;
	    sint32 g;
	    route.GetSourceResource(routeType, g);
	    if(!route->IsBeingPirated()){
		prod += g_theResourceDB->Get(g)->GetProduction();
		}
	    }
	}

    return(prod);
    }

sint32 CityData::GoldFromTrade(){
    sint32 gold= 0;

    for(sint32 i = 0; i < m_tradeDestinationList.Num(); i++){
	TradeRoute route = m_tradeDestinationList[i];
	if(route.IsActive()){
	    ROUTE_TYPE routeType;
	    sint32 g;
	    route.GetSourceResource(routeType, g);
	    if(!route->IsBeingPirated()){
		gold += g_theResourceDB->Get(g)->GetGold();
		}
	    }
	}

    return(gold);
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
	// Science Tax is set to the maximum the government allows
	// Has to be flexibilized
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
	double foodFraction = 0.0;
	double prodFraction = 0.0;
	double goldFraction = 0.0;
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

	for(sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i){
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
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double CityData::ProcessFood(sint32 food) const
{
	///////////////////////////////////////////////
	// Apply building boni
	double foodBonus;
	buildingutil_GetFoodPercent(GetEffectiveBuildings(), foodBonus, m_owner);
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
	buildingutil_GetProductionPercent(GetEffectiveBuildings(), prodBonus, m_owner);
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
	// Add(or if negative Subtract) gold per citizen
	sint32 goldPerCitizen = buildingutil_GetGoldPerCitizen(GetEffectiveBuildings(), m_owner);
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
// Remark(s)  : not used? never generated any errors no matter what I put in
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

	grossScience += buildingutil_GetIncreaseSciencePerPop(GetEffectiveBuildings(), m_owner) * static_cast<double>(PopCount() - SlaveCount());

	///////////////////////////////////////////////
	// Apply building boni
	double scienceBonus;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), scienceBonus, m_owner);
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

	///////////////////////////////////////////////
	// Science cannot be negative
	if(grossScience < 0.0)
	{
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
void CityData::ProcessFood(double &foodLostToCrime, double &producedFood, double &grossFood, bool considerOnlyFromTerrain) const
{
	grossFood += m_bonusFood;

	if(!considerOnlyFromTerrain)
	{
		grossFood += FarmerCount() * GetSpecialistsResources(POP_FARMER);
	}

	grossFood *= GetBonusFoodCoeff();

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
double CityData::ProcessFinalFood(double &foodLossToCrime, double &grossFood) const
{
	foodLossToCrime = CrimeLoss(grossFood);

	return grossFood - foodLossToCrime;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateBonusFood
//
// Description: Calculates the additional food from buildings etc.
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
void CityData::CalculateBonusFood()
{
	m_bonusFood = 0.0;

	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());

	if(styleRec)
	{
		m_bonusFood += styleRec->GetBonusFood();
	}

	m_bonusFood += g_player[m_owner]->CivFoodBonus();

	//Added by E - EXPORT BONUSES TO GOODS if has good than a commerce bonus  (27-FEB-2006)
	//Added by E - EXPORT BONUSES TO GOODS This causes a crime effect if negative and efficiency if positive
	for(sint32 i = 0; i < g_theResourceDB->NumRecords(); ++i)
	{
		if(HasNeededGood(i))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(i);
			if (rec)
			{
				m_bonusFood += rec->GetTradeFood();
			}
		}
	}

	// EMOD - Advances can add bonuses JULY 5 2006
	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++)
	{
		if(g_player[m_owner]->HasAdvance(i))
		{
			AdvanceRecord const * rec = g_theAdvanceDB->Get(i);
			if(rec)
			{
				m_bonusFood += rec->GetBonusFood();
			}
		}
	}

	//EMOD to have food colonies out side your cities i.e. America's Midwest, USSR's Ukraine, Rome's Egypt & Algeria
	if
	  (
	      g_player[m_owner]->GetBreadBasket()     > 0
	   && g_player[m_owner]->GetTotalPopulation() > 0
	  )
	{
		// Excess food spread throughout empire
		m_bonusFood += g_player[m_owner]->GetBreadBasket() * PopCount();

		//unique capitol code later?
		// if (IsCapitol()) {
		//
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateCoeffFood
//
// Description: Calculates the bonus food coefficent from buildings etc. The
//              coefficent is multiplied by the food from terrain.
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
void CityData::CalculateCoeffFood()
{
	m_bonusFoodCoeff = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetFoodCoef();

	double foodBonus;
	buildingutil_GetFoodPercent(GetEffectiveBuildings(), foodBonus, m_owner);
	m_bonusFoodCoeff += foodBonus;

	m_bonusFoodCoeff += (wonderutil_GetIncreaseFoodAllCities(g_player[m_owner]->m_builtWonders) / 100.0);

	//EMOD Civilization and Citystyle bonuses

	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());

	if(styleRec)
	{
		m_bonusFoodCoeff += styleRec->GetFoodPercent();
	}

	for(sint32 i = 0; i < g_theResourceDB->NumRecords(); ++i)
	{
		if(HasNeededGood(i))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(i);
			if(rec)
			{
				double bonus;
				if (rec->GetFoodPercent(bonus))
				{
					m_bonusFoodCoeff += bonus;
				}

				if (rec->GetEfficiencyOrCrime(bonus))
				{
					m_bonusFoodCoeff += bonus;
				}
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateBonusProd
//
// Description: Calculates the additional production from buildings etc.
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
void CityData::CalculateBonusProd()
{
	m_bonusProd  = 0.0;
	m_bonusProd += g_player[m_owner]->CivProductionBonus();

	//EMOD Citystyle bonuses
	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());

	if(styleRec)
	{
		m_bonusProd += styleRec->GetBonusProduction();
	}

	//Added by E - EXPORT BONUSES TO GOODS if has good than a production bonus	Finally Works! 1-13-2006
	//Added by E - EXPORT BONUSES TO GOODS This causes a crime effect if negative and efficiency if positive
	for(sint32 g = 0; g < g_theResourceDB->NumRecords(); ++g)
	{
		if(HasNeededGood(g))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(g);
			if(rec)
			{
				m_bonusProd += rec->GetTradeProduction();
			}
		}
	}

	// EMOD - Advances can add bonuses JULY 5 2006
	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++)
	{
		if(g_player[m_owner]->HasAdvance(i))
		{
			AdvanceRecord const * rec = g_theAdvanceDB->Get(i);
			if(rec)
			{
				m_bonusProd += rec->GetBonusProduction();
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateBonusProd
//
// Description: Calculates the bonus production coefficent from buildings etc. The
//              coefficent is multiplied by the production from terrain.
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
void CityData::CalculateCoeffProd()
{
	m_bonusProdCoeff  = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetProductionCoef() - 1.0;

	double prodBonus;
	buildingutil_GetProductionPercent(GetEffectiveBuildings(), prodBonus, m_owner);
	m_bonusProdCoeff += prodBonus;

	sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_PRODUCTION, m_owner);
	m_bonusProdCoeff += static_cast<double>(featPercent) / 100.0;
	m_bonusProdCoeff += wonderutil_GetIncreaseProduction(g_player[m_owner]->m_builtWonders) * 0.01;

	//EMOD Civilization bonuses
//	m_bonusProdCoeff += g_theCivilisationDB->Get(g_player[m_owner]->m_civilisation->GetCivilisation(), m_government_type)->GetProductionPercent();

	//EMOD Citystyle bonuses
	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());

	if(styleRec)
	{
		m_bonusProdCoeff += styleRec->GetProductionPercent();
	}

	//Added by E - EXPORT BONUSES TO GOODS if has good than a production bonus	Finally Works! 1-13-2006
	//Added by E - EXPORT BONUSES TO GOODS This causes a crime effect if negative and efficiency if positive
	for(sint32 i = 0; i < g_theResourceDB->NumRecords(); ++i)
	{
		if(HasNeededGood(i))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(i);
			if(rec)
			{
				double bonus;
				if(rec->GetProductionPercent(bonus))
				{
					m_bonusProdCoeff += bonus;
				}

				if(rec->GetEfficiencyOrCrime(bonus))
				{
					m_bonusProdCoeff += bonus;
				}
			}
		}
	}

	//emod for energy impacts
	if(g_theProfileDB->IsNRG())
	{
		double energysupply = g_player[m_owner]->GetEnergySupply();
		if ((energysupply < 1.0) && (energysupply > 0.0))
		{ //if greater than 1 no change because industrial capacity is maximized however in the processgold method you'll get extra gold
			m_bonusProdCoeff += energysupply;
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateBonusGold
//
// Description: Calculates the additional gold from buildings etc.
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
void CityData::CalculateBonusGold()
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
	t1.start();
#endif

	m_bonusGold  = static_cast<double>(CalculateGoldFromResources());
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateGoldFromResources    = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	Player* player_ptr = g_player[m_owner];
	sint32 gov = player_ptr->m_government_type;
	DPRINTF(k_DBG_GOVERNOR, ("//  GetPlayerPointer              = %f ms (%i)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	//EMOD Civilization and Citystyle bonuses
	m_bonusGold += player_ptr->CivCommerceBonus();
	DPRINTF(k_DBG_GOVERNOR, ("//  CivCommerceBonus              = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), gov);
	if(styleRec)
	{
		m_bonusGold += styleRec->GetBonusGold();
	}
	DPRINTF(k_DBG_GOVERNOR, ("//  GetCityStyleBonusGold         = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	//Added by E - EXPORT BONUSES TO GOODS This is only for adding not multiplying
	for(sint32 g = 0; g < g_theResourceDB->NumRecords(); ++g)
	{
		if(HasNeededGood(g))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(g);
			if (rec)
			{
				m_bonusGold += rec->GetTradeGold();
			}
		}
	}
	DPRINTF(k_DBG_GOVERNOR, ("//  GetTradeGold                  = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	// EMOD - Advances can add bonuses JULY 5 2006
	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++)
	{
		if(player_ptr->HasAdvance(i))
		{
			AdvanceRecord const * rec = g_theAdvanceDB->Get(i);
			if(rec)
			{
				m_bonusGold += rec->GetBonusGold();
			}
		}
	}
	//end EMOD
	DPRINTF(k_DBG_GOVERNOR, ("//  GetAdvanceBonusGold           = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	//EMOD moved to the end to avoid commercepercent multiplying flags that are likely to be used in the negative

	sint32 goldPerCitizen = buildingutil_GetGoldPerCitizen(GetEffectiveBuildings(), m_owner);
	m_bonusGold += goldPerCitizen * PopCount();
	DPRINTF(k_DBG_GOVERNOR, ("//  goldPerCitizen                = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	//////////////////////////////////
	//EMOD - GoldPerCity but now it multiplied to the max number of cities to allow for higher gold hits to humans 3-27-2006
	sint32 goldPerCity = buildingutil_GetGoldPerCity(GetEffectiveBuildings(), m_owner);
	//gold += static_cast<double>(goldPerCity * player_ptr->m_all_cities->Num());
	m_bonusGold += static_cast<double>(goldPerCity * player_ptr->m_all_cities->Num() * g_theGovernmentDB->Get(gov)->GetTooManyCitiesThreshold());
	DPRINTF(k_DBG_GOVERNOR, ("//  GetGoldPerCity                = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	///////////////////////////////////////////////
	// EMOD - Add (or if negative Subtract) gold per unit
	sint32 goldPerUnit = buildingutil_GetGoldPerUnit(GetEffectiveBuildings(), m_owner);
	m_bonusGold += static_cast<double>(goldPerUnit * player_ptr->m_all_units->Num());
	DPRINTF(k_DBG_GOVERNOR, ("//  GetGoldPerUnit                = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	///////////////////////////////////////////////
	// EMOD - Add (or if negative Subtract) gold per unit and multiplied by readiness level
	sint32 goldPerUnitReadiness = buildingutil_GetGoldPerUnitReadiness(GetEffectiveBuildings(), m_owner);
	m_bonusGold += static_cast<double>(goldPerUnitReadiness * player_ptr->m_all_units->Num()) * player_ptr->m_readiness->GetSupportModifier(gov);
	DPRINTF(k_DBG_GOVERNOR, ("//  GetGoldPerUnitReadiness       = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	///////////////////////////////////////////////
	// EMOD - Add (or if negative Subtract) gold per unit and multiplied by goldhunger * readiness * govt coefficient * wages
	sint32 goldPerUnitSupport = buildingutil_GetGoldPerUnitSupport(GetEffectiveBuildings(), m_owner);
	m_bonusGold += static_cast<double>(goldPerUnitSupport * player_ptr->m_readiness->GetGoldCost()) * player_ptr->GetWagesPerPerson() * player_ptr->m_readiness->GetSupportModifier(gov);
	DPRINTF(k_DBG_GOVERNOR, ("//  GetGoldPerUnitSupport         = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	double interest;
	buildingutil_GetTreasuryInterest(GetEffectiveBuildings(), interest, m_owner);
	m_bonusGold += static_cast<double>(player_ptr->m_gold->GetLevel()) * interest;
	DPRINTF(k_DBG_GOVERNOR, ("//  GetTreasuryInterest           = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

#if defined(_DEBUG) || defined(USE_LOGGING)
	t1.stop();
#endif
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateCoeffGold
//
// Description: Calculates the bonus gold coefficent from buildings etc. The
//              coefficent is multiplied by the gold from terrain.
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
void CityData::CalculateCoeffGold()
{
	m_bonusGoldCoeff  = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetGoldCoef();

	double bonus;
	buildingutil_GetCommercePercent(GetEffectiveBuildings(), bonus, m_owner);
	m_bonusGoldCoeff += bonus;

	sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_COMMERCE, m_owner);
	m_bonusGoldCoeff += featPercent / 100.0;

	//EMOD these three EMODs moved inside the less than 0 to prevent multiplication of percent to zero values
	//EMOD Civilization and Citystyle bonuses
//	m_bonusGoldCoeff += g_theCivilisationDB->Get(g_player[m_owner]->m_civilisation->GetCivilisation(), m_government_type)->GetCommercePercent();

	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());
	if(styleRec)
	{
		m_bonusGoldCoeff += styleRec->GetCommercePercent();
	}

	//Added by E - EXPORT BONUSES TO GOODS if has good than a commerce bonus  (11-JAN-2006)
	//Added by E - EXPORT BONUSES TO GOODS This causes a crime effect if negative and efficiency if positive
	for(sint32 i = 0; i < g_theResourceDB->NumRecords(); ++i)
	{
		if(HasNeededGood(i))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(i);
			if(rec)
			{
				if(rec->GetCommercePercent(bonus))
				{
					m_bonusGoldCoeff += bonus;
				}

				if(rec->GetEfficiencyOrCrime(bonus))
				{
					m_bonusGoldCoeff += bonus;
				}
			}
		}
	}

	//emod for energy impacts
	if(g_theProfileDB->IsNRG())
	{
		double energysupply = g_player[m_owner]->GetEnergySupply();
		if(energysupply > 0.0)
		{  //&& profile energy
			m_bonusGoldCoeff += energysupply;  // if greater than one that is additional wealth
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateBonusScie
//
// Description: Calculates the additional science from buildings etc.
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
void CityData::CalculateBonusScie()
{
	m_bonusScie  = 0.0;

	m_bonusScie += buildingutil_GetIncreaseSciencePerPop(GetEffectiveBuildings(), m_owner);
	m_bonusScie *= static_cast<double>(PopCount() - SlaveCount());

	//Added by E - EXPORT BONUSES TO GOODS This is only for adding not multiplying
	for(sint32 g = 0; g < g_theResourceDB->NumRecords(); ++g)
	{
		if(HasNeededGood(g))
		{
			ResourceRecord const * rec = g_theResourceDB->Get(g);
			if(rec)
			{
				m_bonusScie += rec->GetTradeScience();
			}
		}
	}

	// EMOD - Advances can add bonuses JULY 5 2006
	for(sint32 i = 0; i < g_theAdvanceDB->NumRecords(); i++)
	{
		if(g_player[m_owner]->HasAdvance(i))
		{
			AdvanceRecord const * rec = g_theAdvanceDB->Get(i);
			if(rec)
			{
				m_bonusScie += rec->GetBonusScience();
			}
		}
	}

	//EMOD - tileimps can add science Oct 3, 2006
	MapPoint cityPos = m_home_city.RetPos();
	CityInfluenceIterator it(cityPos, m_sizeIndex);
	for(it.Start(); !it.End(); it.Next())
	{
		Cell *cell = g_theWorld->GetCell(it.Pos());
		for(sint32 t = 0; t < cell->GetNumDBImprovements(); t++)
		{
			sint32 imp = cell->GetDBImprovement(t);
			const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(imp);
			const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(rec, it.Pos());
			if(effect)
			{
				m_bonusScie += effect->GetBonusScience();
			}
		}
	}
	//end EMOD

	//Civilization Bonus
	m_bonusScie += g_player[m_owner]->CivScienceBonus();
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateBonusScie
//
// Description: Calculates the bonus science coefficent from buildings etc. The
//              coefficent is multiplied by the science from terrain.
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
void CityData::CalculateCoeffScie()
{
	m_bonusScieCoeff  = g_player[m_owner]->GetKnowledgeCoef();

	double bonus;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), bonus, m_owner);
	m_bonusScieCoeff += bonus;

	sint32 featPercent = g_featTracker->GetAdditiveEffect(FEAT_EFFECT_INCREASE_SCIENCE, m_owner);
	m_bonusScieCoeff += static_cast<double>(featPercent) / 100.0;

	m_bonusScieCoeff += 0.01 * wonderutil_GetIncreaseKnowledgePercentage(g_player[m_owner]->GetBuiltWonders());

	//EMOD Citystyle bonuses
	const CityStyleRecord *styleRec = g_theCityStyleDB->Get(GetCityStyle(), g_player[m_owner]->GetGovernmentType());

	if(styleRec)
	{
		m_bonusScieCoeff += styleRec->GetSciencePercent();
	}

	//Added by E - EXPORT BONUSES TO GOODS if has good than a science bonus (11-JAN-2006)
	//Added by E - EXPORT BONUSES TO GOODS This causes a crime effect if negative and efficiency if positive
	for(sint32 i = 0; i < g_theResourceDB->NumRecords(); ++i)
	{
		if(HasNeededGood(i))
		{
			ResourceRecord const * rec= g_theResourceDB->Get(i);
			if(rec)
			{
				if(rec->GetSciencePercent(bonus))
				{
					m_bonusScieCoeff += bonus;
				}

				if(rec->GetEfficiencyOrCrime(bonus))
				{
					m_bonusScieCoeff += bonus;
				}
			}
		}
	}
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

	if(m_food_delta < 0)
	{
		if(m_food_delta < -maxSurplusFood)
			m_food_delta = -maxSurplusFood;
	}
	else
	{
		if(m_food_delta > maxSurplusFood)
			m_food_delta = maxSurplusFood;
	}
}

sint32 CityData::GetBuildingOvercrowdingBonus() const
{
	sint32 level = 0;
	buildingutil_GetRaiseOvercrowdingLevel(GetEffectiveBuildings(), level, m_owner);
	return level;
}

sint32 CityData::GetBuildingMaxPopIncrease() const
{
	sint32 level = 0;
	buildingutil_GetRaiseMaxPopulation(GetEffectiveBuildings(), level, m_owner);
	return level;
}

sint32 CityData::GetPossibleBuildingMaxPopIncrease() const
{
	sint32 value = 0;

	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); ++i)
	{
		sint32 raise;
		if(g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetRaiseMaxPopulation(raise) && this->CanBuildBuilding(i))
		{
			value += raise;
		}
	}

	return value;
}

sint32 CityData::GetAllBuildingMaxPopIncrease() const
{
	sint32 value = 0;

	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); ++i)
	{
		sint32 raise;
		if(g_theBuildingDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetRaiseMaxPopulation(raise))
		{
			value += raise;
		}
	}

	return value;
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
void CityData::CalculateGrowthRate()
{
	double baseRate;
	m_growth_rate = static_cast<sint32>(CalculateGrossGrowthRate(m_overcrowdingCoeff, baseRate));

	if(m_food_delta >= 0)
	{
		m_growth_rate = static_cast<sint32>(baseRate * m_growth_rate);
		if(m_growth_rate > g_theConstDB->Get(0)->GetMaxAbsoluteGrowthRate())
			m_growth_rate = g_theConstDB->Get(0)->GetMaxAbsoluteGrowthRate();

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
	if(m_sizeIndex < 1)
	{
		minPop = 0;
	}
	else
	{
		minPop = static_cast<double>(g_theCitySizeDB->Get(m_sizeIndex - 1)->GetPopulation()) + 1.0;
	}

	double overcrowding = GetOvercrowding(rec);
	sint32 maxPop = GetMaxPop(rec);

	if(PopCount() < overcrowding)
	{
		baseRate = maxGrowthRate;
		overcrowdingCoeff = 1;
	}
	else
	{
		double popRatio = static_cast<double>(PopCount() - overcrowding) / static_cast<double>(maxPop - overcrowding);
		overcrowdingCoeff = 1 - (popRatio * popRatio);
		baseRate = overcrowdingCoeff * maxGrowthRate;
	}

	if(m_food_delta + bonusFood < 0)
	{
		double maxPop = static_cast<double>(rec->GetPopulation());
		double popScale = (static_cast<double>(PopCount()) - minPop) / (maxPop - minPop);
		double growthRate = maxGrowthRate - popScale * (maxGrowthRate - minGrowthRate);

		return(((m_food_delta + bonusFood) / static_cast<double>(maxSurplusFood)) * growthRate);
	}
	else
	{
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
// Returns    : bool: true if the city is either starving or changes pop size
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
bool CityData::GrowOrStarve()
{
	CalculateGrowthRate();

	if(m_food_delta < 0)
	{
		if(m_growth_rate < 0)
		{
			if(m_starvation_turns > 0)
			{
				if (m_starvation_turns == GetStarvationProtection())
				{
					SlicObject *so = new SlicObject("911CityWillStarveInitialWarning") ;
					so->AddRecipient(GetOwner()) ;
					so->AddCity(m_home_city) ;
					so->AddPlayer(m_owner);
					g_slicEngine->Execute(so) ;
				}
				else if (m_starvation_turns == (sint32)(GetStarvationProtection()/2))
				{
					SlicObject *so = new SlicObject("911CityWillStarveFoodStoresLow") ;
					so->AddRecipient(GetOwner()) ;
					so->AddCity(m_home_city) ;
					so->AddPlayer(m_owner);
					g_slicEngine->Execute(so) ;
				}

				m_starvation_turns--;

				UpdateSprite();

				return true;
			}
			else m_partialPopulation -= k_PEOPLE_PER_POPULATION;
		}

		m_partialPopulation += m_growth_rate;

		if(m_partialPopulation >= k_PEOPLE_PER_POPULATION)
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);
			m_partialPopulation -= k_PEOPLE_PER_POPULATION;

			UpdateSprite();

			return true;

		}
		else if(m_partialPopulation < 0)
		{
			//PFT 05 apr 05: slaves starve first
			if(SlaveCount() > 0 )
				ChangeSpecialists(POP_SLAVE, -1);

			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_KillPop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);

			m_partialPopulation += k_PEOPLE_PER_POPULATION;

			UpdateSprite();

			return true;
		}
	}
	else
	{
		m_partialPopulation += sint32(m_growth_rate);

		ResetStarvationTurns();

		if(m_partialPopulation >= k_PEOPLE_PER_POPULATION)
		{
			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_MakePop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);

			m_partialPopulation -= k_PEOPLE_PER_POPULATION;

			UpdateSprite();

			return true;
		}
	}

	UpdateSprite();

	return false;
}

bool CityData::FoodSupportTroops()
{
	return false;
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
	switch(type)
	{
		case ROUTE_TYPE_RESOURCE:
			m_buyingResources.AddResource(resource);
			break;
		case ROUTE_TYPE_FOOD:
			Assert(false);

			break;
		default:
			Assert(false);
			break;
	}
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CalculateTradeRoutes
//
// Description: Removes routes that cannot be afforded any more (from CTP1)
//              Basically has no effect for CTP2 until GetGoldInReturn() or ROUTE_TYPE_FOOD is used again
//              Activates trade route in case it is not, should not happen, just in case
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
void CityData::CalculateTradeRoutes(bool projectedOnly)
{
	sint32 i, n;
	ROUTE_TYPE routeType;
	sint32 routeResource;
	TradeDynamicArray deadRoutes;

	// outgoing goods:
	n = m_tradeSourceList.Num();
	m_sellingResources.Clear();
	for(i = 0; i < n; i++)
	{
		TradeRoute route = m_tradeSourceList[i];
		if(!g_theTradePool->IsValid(route))
		{
			if(g_network.IsClient())
			{
				g_network.RequestResync(RESYNC_BAD_TRADE_ROUTE);
			}
			continue;
		}

		if(!route.IsActive()){
		    route.Activate(); // routes of m_tradeSourceList and m_tradeDestinationList are expected to be active
		    fprintf(stderr, "%s L%d: Activated deactive route, this should not happen!\n", __FILE__, __LINE__);
		    }		    
		route.GetSourceResource(routeType, routeResource);

		switch(routeType)
		{
			case ROUTE_TYPE_RESOURCE:
			    if(m_collectingResources[routeResource] <= m_sellingResources[routeResource]) // if good got lost (e.g. city shrunk or in case good collection is made dependent on adv., building, impr. etc)
				{
					if(!projectedOnly)
					{
						deadRoutes.Insert(route);
					}
				}

				m_sellingResources.AddResource(routeResource);
				break;
			case ROUTE_TYPE_FOOD:
			{
				if(m_food_delta < routeResource && !projectedOnly)
				{
					deadRoutes.Insert(route);
				}
				break;
			}
			default:
				Assert(false);
				break;
		}
	}

	// incoming goods:
	n = m_tradeDestinationList.Num();
	bool killRoute;
	m_buyingResources.Clear();
	for(i = 0; i < n; i++)
	{
		TradeRoute route = m_tradeDestinationList[i];
		if(!g_theTradePool->IsValid(route))
		{
			if(g_network.IsClient())
			{
				g_network.RequestResync(RESYNC_BAD_TRADE_ROUTE);
			}
			continue;
		}

		if(!route.IsActive()){
		    route.Activate(); // routes of m_tradeSourceList and m_tradeDestinationList are expected to be active
		    fprintf(stderr, "%s L%d: Activated deactive route, this should not happen!\n", __FILE__, __LINE__);
		    }		    

		killRoute = false;
		if(!projectedOnly)
		{
			if(route.GetOwner() != m_owner) // foreign source
			{
			//// GetGoldInReturn() seems to be a remainder form CTP1 and currently is always 0 because the only active place where it is set (0) is here: https://github.com/civctp2/civctp2/blob/ea3274c3d43176670ac71f6bc75703fac18a226e/ctp2_code/gs/gameobj/PlayerEvent.cpp#L556-L558
			//// currently the profit form trade (gold) is given here: https://github.com/civctp2/civctp2/blob/ea3274c3d43176670ac71f6bc75703fac18a226e/ctp2_code/gs/gameobj/CityData.cpp#L4510-L4538
			        Assert(route.GetGoldInReturn() == 0); // remove if GetGoldInReturn() is used again with values != 0
				if(g_player[m_owner]->GetGold() < route.GetGoldInReturn()) // kill route because of not enough gold to pay for the resource
				{
					deadRoutes.Insert(route);
					killRoute = true;
				}
				else
				{ // since GetGoldInReturn() is currently always 0, this basically has no effect
					g_player[m_owner]->SubGold(route.GetGoldInReturn()); // remove gold from receiver
					g_player[route.GetSource().GetOwner()]->AddGold(route.GetGoldInReturn()); // give gold to sender
				}
			}
		}

		if(route.IsActive() && !killRoute)
		{
			route.GetSourceResource(routeType, routeResource);
			AddTradeResource(routeType, routeResource); // add to m_buyingResources
		}
	}

	if(!projectedOnly)
	{
		for(sint32 i = deadRoutes.Num() - 1; i >= 0; i--)
		{
			if(deadRoutes[i].GetSource().m_id == m_home_city.m_id)
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute,
				                       GEA_TradeRoute, deadRoutes[i],
				                       GEA_Int, CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED,
				                       GEA_End);
			}
			else
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute,
				                       GEA_TradeRoute, deadRoutes[i],
				                       GEA_Int, CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED,
				                       GEA_End);
			}
		}
	}
}

sint32 CityData::CalculateGoldFromResources()
{
	m_goldFromTradeRoutes = 0;
	m_goldLostToPiracy    = 0;

	for(sint32 i = 0; i < m_tradeSourceList.Num(); i++)
	{
		if(!m_tradeSourceList[i]->IsBeingPirated())
		{
			m_goldFromTradeRoutes += m_tradeSourceList[i]->GetValue();
		}
		else
		{
			m_goldLostToPiracy += m_tradeSourceList[i]->GetValue();
		}
	}

	sint32 wonderTradeBonus = wonderutil_GetMultiplyTradeRoutes(m_builtWonders);

	if(wonderTradeBonus > 0)
	{
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
// Description: Pays the building and city support costs and sells buildings
//				if necessary.
//
// Parameters : projectedOnly: Whether costs should be paid or just be
//                             estimated.
//
// Globals    : g_player
//
// Returns    : sint32      : The building upkeep costs
//
// Remark(s)  : Assumption  : g_player[m_owner] is non-NULL
//
//----------------------------------------------------------------------------
sint32 CityData::SupportBuildings(bool projectedOnly)
{
	sint32     buildingUpkeep = GetSupportBuildingsCost();

	m_net_gold   -= buildingUpkeep;

	if (!projectedOnly)
	{
		if (m_net_gold < 0)
		{
			// Spending deficit
			sint32 const    wonderLevel =
			    wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);

			if(buildingUpkeep > 0)
			{
				while(IsBankrupting() && (m_built_improvements != 0))
				{
					sint32 const    cheapBuilding =
						buildingutil_GetCheapestBuilding(m_built_improvements, wonderLevel, m_owner);
					Assert(cheapBuilding >= 0);
					if(cheapBuilding < 0)
						break;

					SellBuilding(cheapBuilding, false);
					SlicObject * so = new SlicObject("029NoMaint");
					so->AddRecipient(GetOwner());
					so->AddCity(m_home_city);
					so->AddBuilding(cheapBuilding);
					g_slicEngine->Execute(so);
				}
			}

			if(IsBankrupting())
			{
#if defined(NEW_RESOURCE_PROCESS)
				m_science -= m_net_gold - g_player[m_owner]->GetGold(); // m_science originally generated from gold income, so remove it if gold isn't enough.
				if (m_science < 0)
				{
					m_science = 0;
				}
#endif
				g_player[m_owner]->m_gold->SetLevel(0);
			}
			else
			{
				g_player[m_owner]->m_gold->SubIncome(-m_net_gold);
			}

			m_net_gold = 0;
		}

		g_player[m_owner]->m_gold->AddMaintenance(buildingUpkeep);
	}

	return buildingUpkeep;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GetSupportCityCost
//
// Description: Each city costs the city's population in commerce (not gold).
//
// Parameters : -
//
// Globals    : -
//
// Returns    : Commerce to support the city.
//
// Remark(s)  : This is added to the buildings support cost in
//				CityData::SupportBuildings.
//				diffDB.txt entry takes precedence over userprofile.txt entry
//				to enable this rule.
//
//----------------------------------------------------------------------------
sint32 CityData::GetSupportCityCost() const
{
	sint32 goldPerCity = 0;

	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetGoldPerCity(goldPerCity))
	{
		return goldPerCity * PopCount();
	}

	if(g_theProfileDB->IsGoldPerCity())
	{
		return PopCount();
	}

	return 0;
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
	sint32 wonderLevel    = wonderutil_GetDecreaseMaintenance(g_player[m_owner]->m_builtWonders);
	sint32 buildingUpkeep = buildingutil_GetTotalUpkeep(m_built_improvements, wonderLevel, m_owner);

	// City maintenance cost.
	buildingUpkeep += GetSupportCityCost();

	//EMOD notadd upkeep per city
	sint32 UpkeepPerCity = buildingutil_GetUpkeepPerCity(m_built_improvements, m_owner);
	buildingUpkeep += UpkeepPerCity * g_player[m_owner]->m_all_cities->Num();

	///////////////////////////////////////////////
	// EMOD - Add upkeep per unit
	sint32 UpkeepPerUnit = buildingutil_GetUpkeepPerUnit(m_built_improvements, m_owner);
	buildingUpkeep += UpkeepPerUnit * g_player[m_owner]->m_all_units->Num();

	///////////////////////////////////////////////
	// EMOD - upkeep per unit and multiplied by readiness level
	sint32 upkeepPerUnitWagesReadiness = buildingutil_GetUpkeepPerUnitWagesReadiness(m_built_improvements, m_owner);
	buildingUpkeep += static_cast<sint32>(upkeepPerUnitWagesReadiness * g_player[m_owner]->m_all_units->Num() * g_player[m_owner]->GetWagesPerPerson() * g_player[m_owner]->m_readiness->GetSupportModifier(g_player[m_owner]->m_government_type));

	//end EMOD

	return buildingUpkeep;
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
sint32 CityData::CalcWages() const
{
	return  static_cast<sint32>(g_player[m_owner]->GetWagesPerPerson()) * (PopCount() - SlaveCount());
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ShouldRevolt
//
// Description: Pays the wages.
//
// Parameters : wage            : The amount of wages for one pop
//              projectedOnly   : Whether wages should be paid or only estimated.
//
// Globals    : g_player        : List of players
//
// Returns    : bool            : The player can afford to pay all wages
//
// Remark(s)  : Assumption      : g_player[m_owner] is non-NULL
//
//----------------------------------------------------------------------------
bool CityData::PayWages(bool projectedOnly)
{
	m_wages_paid = CalcWages();
	m_net_gold  -= m_wages_paid;

	if (!projectedOnly)
	{
		g_player[m_owner]->m_gold->AddWages(m_wages_paid);

		if (m_net_gold < 0)
		{
			if (IsBankrupting())
			{
				g_player[m_owner]->m_gold->SubIncome(g_player[m_owner]->GetGold());
				return false;
			}
			else
			{
				g_player[m_owner]->m_gold->SubIncome(-m_net_gold);
			}
		}
	}

	return true;
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
//              Activates trade route in case it is not!
//
// Parameters : TradeRoute &route       : the trade route
//            : bool       fromNetwork  :
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
void CityData::AddTradeRoute(TradeRoute &route, bool fromNetwork)
{
	Assert((route.GetSource() == m_home_city) ||
		   (route.GetDestination() == m_home_city));

	if(!route.IsActive()) // do not skip deactivated routes, calling CityData::AddTradeRoute should always only be used for trade routes to be active
	    route.Activate(); // routes of m_tradeSourceList and m_tradeDestinationList are expected to be active (i.e. no checks on route.IsActive() there)

	if(route.GetSource() == m_home_city)
	{
		m_tradeSourceList.Insert(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type)
		{
			case ROUTE_TYPE_RESOURCE:
				if(!fromNetwork)
				{
					Assert(m_collectingResources[resource] > m_sellingResources[resource]);
					if(m_collectingResources[resource] > m_sellingResources[resource])
					{
						m_sellingResources.AddResource(resource);
					}
				}
				break;
			default:
				break;
		}
	}

	if(route.GetDestination() == m_home_city)
	{
		m_tradeDestinationList.Insert(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type)
		{
			case ROUTE_TYPE_RESOURCE:
				if(!fromNetwork)
				{
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

	if(route.GetSource() == m_home_city)
	{
		m_tradeSourceList.Del(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type)
		{
			case ROUTE_TYPE_RESOURCE:
				if(m_sellingResources[resource] > 0) // can be already zero if called 2nd time from TradeRoute::RemoveAllReferences
				{
					m_sellingResources.AddResource(resource, -1);
				}
				break;
			default:
				break;
		}
	}

	if(route.GetDestination() == m_home_city)
	{
		m_tradeDestinationList.Del(route);
		ROUTE_TYPE type;
		sint32 resource;
		route.GetSourceResource(type, resource);
		switch(type)
		{
			case ROUTE_TYPE_RESOURCE:
				if(m_buyingResources[resource] > 0) // can be already zero if called 2nd time from TradeRoute::RemoveAllReferences
				{
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
	sint32	pos;

	if (g_theTopTen->IsTopTenCity(m_home_city, TOPTENTYPE_BIGGEST_CITY, pos))
	{
		m_currentlyTopTen = TRUE;
	}
	else
	{
		m_currentlyTopTen = FALSE;
	}
#endif
}

void CityData::CheatBuildFirstItem()
{
	MapPoint pos;

	m_home_city.GetPos(pos);

	m_shieldstore += m_build_queue.GetProductionRemaining(m_shieldstore);
	if(m_build_queue.BuildFront(m_shieldstore, this, pos, m_built_improvements, m_builtWonders, false))
	{
		m_shieldstore_at_begin_turn = m_shieldstore;
	}
}

void CityData::InitBeginTurnVariables()
{
	m_capturedThisTurn           = false;
	m_terrainWasPolluted         = false;
	m_happinessAttackedBy        = PLAYER_UNASSIGNED;
	m_terrainImprovementWasBuilt = false;
	m_improvementWasBuilt        = false;

	m_alreadySoldABuilding       = false;
	m_walls_nullified            = false;
	//m_secthappy = 0;
	m_goldFromTransitRoutes      = 0;
}

void CityData::DoTurnCounters()
{
	if(m_spied_upon > 0)
		m_spied_upon--;

	if(m_watchfulTurns > 0)
		m_watchfulTurns--;

	if(m_bioInfectionTurns > 0)
	{
		m_bioInfectionTurns--;
		SpreadBioTerror();
	}

	if(m_nanoInfectionTurns > 0)
	{
		m_nanoInfectionTurns--;
		SpreadNanoTerror();
	}

	if(m_franchiseTurnsRemaining > 0)
	{
		m_franchiseTurnsRemaining--;
	}
	else if(m_franchiseTurnsRemaining == 0) // only zero is meant to reset a Franchise (owner), positive numbers define turns until Franchise is removed, -1 is the default for infinit Franchise, see https://github.com/civctp2/civctp2/pull/167
	{
		RemoveFranchise();
	}
}

// deal with capitalization/infrastructure. Otherwise, build the front item in this city's buildqueue.
void CityData::TryToBuild()
{
	if(!m_isInjoined)
	{
		AddShieldsToBuilding();
		const GovernmentRecord *grec = g_theGovernmentDB->Get(g_player[m_owner]->m_government_type);

		if(IsBuildingCapitalization())
		{
			m_gold_from_capitalization = sint32(ceil(double(m_shieldstore) * grec->GetCapitalizationCoefficient()));
			m_shieldstore = 0;
			m_build_category_at_begin_turn = -1;

			if(m_build_queue.GetLen() > 1) // Remove if we have more in the list
			{
				m_build_queue.RemoveHead();
			}
		}
		else if(IsBuildingInfrastructure())
		{
			m_pw_from_infrastructure = sint32(ceil(double(m_shieldstore) * grec->GetInfrastructureCoefficient()));
			g_player[m_owner]->m_materialPool->AddMaterials(m_pw_from_infrastructure);
			m_shieldstore = 0;
			m_build_category_at_begin_turn = -2;

			if(m_build_queue.GetLen() > 1) // Remove if we have more in the list
			{
				m_build_queue.RemoveHead();
			}
		}
		else
		{
			m_gold_from_capitalization = 0;

			m_pw_from_infrastructure = 0;

			g_gevManager->AddEvent(GEV_INSERT_AfterCurrent,
			                       GEV_BuildFront,
			                       GEA_City, m_home_city,
			                       GEA_End);
		}
	}

	m_shieldstore_at_begin_turn = m_shieldstore;

	m_isInjoined = false;
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
// Returns    : bool
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
bool CityData::BeginTurn()
{
	InitBeginTurnVariables();
	DoTurnCounters(); // Decrement various turn counters, e.g., m_franchiseTurnsRemaining

	ImprovementHealUnitsInCity();   // Zeroed out. Could have, e.g., hospitals speeding up healing
	ImprovementRefuelUnitsInCity(); // Zeroed out. Could be removed.

	MapPoint pos;
	m_home_city.GetPos(pos); // See CityInfluenceIterator below

	m_pw_from_infrastructure = 0;
	m_gold_from_capitalization = 0;

	CalculateTradeRoutes(false);
	PreResourceCalculation();

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
//	DoSupport(false); // Check
#endif

	// After new resource calculation - no problem
	EatFood(); // Calculate m_food_delta = m_net_food - m_food_consumed_this_turn

	if (GrowOrStarve())
	{ // Deal with city starvation and growth/shrinkage
		if (PopCount() < 1)
		{
			SlicObject *so = new SlicObject("265CityDestroyedByStarving");
			so->AddRecipient(GetOwner());
			so->AddCity(m_home_city);
			g_slicEngine->Execute(so);

			return false;
		}
	}

	if (IsCelebratingHappiness())
	{
		if(m_lastCelebrationMsg < 0 || (m_lastCelebrationMsg + 10 < g_turn->GetRound()))
		{
			SlicObject *so = new SlicObject("40CityIsCelebratingHappiness") ;
			so->AddCity(m_home_city);
			so->AddRecipient(m_owner);
			so->AddPlayer(m_owner);
			g_slicEngine->Execute(so);
			m_lastCelebrationMsg = sint16(g_turn->GetRound());
		}

		g_player[m_owner]->m_score->AddCelebration(); // Could use something more interesting here
	}

	if(!m_build_queue.GetHead()
	&& !IsBuildingCapitalization()
	&& !IsBuildingInfrastructure()
	&& !m_sentInefficientMessageAlready
	){
		SlicObject *so = new SlicObject("37CityQueueIsEmpty");
		so->AddCity(m_home_city);
		so->AddCivilisation(m_owner);
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
		g_slicEngine->RunWastingWorkTriggers(m_home_city);
	}
	m_sentInefficientMessageAlready = false;

	CheckForSlaveUprising(); // Check that there's enough military units to guard the slaves.
	// Does the city have a TerrainImprovement in it's radius? Used for pillage goal.
	CityInfluenceIterator it(pos, m_sizeIndex);
	Cell *cell;
	for(it.Start(); !it.End(); it.Next())
	{
		cell = g_theWorld->GetCell(it.Pos());

		if(!m_terrainImprovementWasBuilt
		&& it.Pos() != pos
		&& cell->GetNumDBImprovements() > 0
		&& cell->GetCityOwner() == GetHomeCity()
		){
			m_terrainImprovementWasBuilt = true;
		}
	}

	// EMOD - AddsASlave If City is collecting good that is adds slaves
	// changepop...add slave.
	// Doing this means to add each turn slaves until the whole city is full of slaves.
	// Welcome to slave uprising.
	for (sint32 slavegood = 0; slavegood < g_theResourceDB->NumRecords(); ++slavegood)
	{
		if(g_theResourceDB->Get(slavegood)->GetAddsASlave())
		{
			if(HasNeededGood(slavegood))
			{ //&& if(!wonderutil_GetFreeSlaves(g_theWonderTracker->GetBuiltWonders()))
				ChangePopulation(+1);
				ChangeSpecialists(POP_SLAVE, +1);
			}
		}
	}

	//kills barbarian cities 5-18-2007
	//if((m_owner == PLAYER_INDEX_VANDALS) && (PopCount() > 1)) {
	//	ChangePopulation(-1);
	//}

	//EMOD diffDB so sometimes your city when it riots creates barbs 10-25-2006
	// added methods for insurgents and riot casualties
	if(m_is_rioting)
	{
		RiotCasualties();
		InsurgentSpawn();
	}

	CityGovernmentModifiers();
	Militia();

	//Cities that have trade routes pass through them should make money like along the Silkroad or mediterreanen trade
	if(CityIsOnTradeRoute())
	{
		GiveTradeRouteGold();
	}

	//added city expanision code - 31 Aug 2007   requires Urban and Slum TileImps
	AddCityExpansion();
	AddCitySlum();

	//END EMOD

	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);

	// Added this so city production turns are calculated on begin turn
	// using correct amount of current net city production.
	ProcessProduction(true);

	return true;
}

void CityData::EndTurn()
{
	if(m_sellBuilding >= 0)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SellBuilding,
		                       GEA_City, m_home_city,
		                       GEA_Int, m_sellBuilding,
		                       GEA_End);
		m_sellBuilding = -1;
	}

	if(m_buyFront)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_BuyFront,
		                       GEA_City, m_home_city,
		                       GEA_End);

		m_buyFront = false;
	}

	m_build_queue.EndTurn();

	// let city expand if adjacent city is gone or has shrunk
	GenerateCityInfluence(m_home_city.RetPos(), m_sizeIndex);
}

void CityData::CalcHappiness(sint32 &virtualGoldSpent, bool isFirstPass)

{
	sint32 delta_martial_law;
	m_happy->CalcHappiness(*this, false, delta_martial_law, isFirstPass);
}

void CityData::CheckRiot()
{
	m_is_rioting = FALSE;

	if(m_home_city.Flag(k_UDF_CANT_RIOT_OR_REVOLT))
		return;

	if(g_slicEngine->GetTutorialActive() &&
	   m_owner == g_slicEngine->GetTutorialPlayer())
		return;

	if(m_happy->GetHappiness() < g_theConstDB->Get(0)->GetRiotLevel())
	{
		if(g_rand->Next(100) < ((g_theConstDB->Get(0)->GetRiotLevel() - m_happy->GetHappiness()) *
								g_player[m_owner]->GetRiotChance()))
		{
			m_is_rioting = TRUE;

			SlicObject *so = new SlicObject("100CityRioting");
			so->AddCity(m_home_city);
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
			g_player[m_owner]->m_score->AddRiot();

// EMOD to add graphics to rioting cities //need to change and add sprite and where to put effect sprite
//			SpecialAttackInfoRecord const * specRec = unitutil_GetSpecialAttack(SPECATTACK_REVOLUTION);  //this unitutil looks like old stuff
//			if (specRec) {
//			sint32 const soundID = specRec->GetSoundIDIndex();
//
//				if (soundID >= 0)
//				{
//					sint32 const spriteID = specRec->GetSpriteID()->GetValue();
//					if (spriteID >= 0)
//					{
//						g_director->AddSpecialAttack (m_home_city.GetActor()->GetUnitID(), m_home_city, SPECATTACK_REVOLUTION);
//					}
//					else
//					{
//						sint32 const visiblePlayer = g_selected_item->GetVisiblePlayer();
//						if ((visiblePlayer == m_owner) || (m_home_city.GetVisibility() & (1 << visiblePlayer)) )
//						{
//							g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID, m_home_city.RetPos().x, m_home_city.RetPos().y);
//						}
//					}
//				}
//			}
			//end EMOD
		}
	}
}

bool CityData::BuildUnit(sint32 type)
{
	if(!CanBuildUnit(type))
		return false;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendAction(new NetAction(NET_ACTION_BUILD, type, m_home_city.m_id));
	}
	else if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILDING_UNIT,
		                  type, (uint32)m_home_city));
		g_network.Unblock(m_owner);
	}

	const UnitRecord* rec = g_theUnitDB->Get(type, g_player[m_owner]->GetGovernmentType());
	Assert(rec);
	if(rec == NULL)
		return false;

	if(g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex()))
	{
		DPRINTF(k_DBG_GAMESTATE, ("%s is buildig a unit: %s\n",
		                          m_home_city.CD()->GetName(),
		                          g_theStringDB->GetNameStr(rec->m_name)));
		return m_build_queue.InsertTail(k_GAME_OBJ_TYPE_UNIT, type, rec->GetShieldCost());
	}
	else
	{
		DPRINTF(k_DBG_GAMESTATE, ("%s can't build %s until %s is discovered\n",
		                          m_home_city.CD()->GetName(),
		                          g_theStringDB->GetNameStr(rec->m_name),
		                          g_theAdvanceDB->GetNameStr(rec->GetEnableAdvanceIndex())));
		return false;
	}
}

bool CityData::BuildImprovement(sint32 type)
{
	Assert(CanBuildBuilding(type));
	if(!CanBuildBuilding(type))
		return false;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_IMP, type, m_home_city.m_id));
	}
	else if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_IMP, m_owner, type,
		                  m_home_city));
		g_network.Unblock(m_owner);
	}

	const BuildingRecord* irec = buildingutil_Get(type, m_owner);
	Assert(irec);
	if(irec == NULL)
		return false;

	if(g_player[m_owner]->HasAdvance(irec->GetEnableAdvanceIndex()))
	{
		DPRINTF(k_DBG_GAMESTATE, ("%s is building an improvement: %s\n",
		                          m_home_city.CD()->GetName(),
		                          g_theStringDB->GetNameStr(irec->GetName())));

		//ProductionCostPopModifier m_city.CD()->PopCount()
			//EMOD ProductionCostPopModifier  10-10-2006

				sint32 cost = 0;

				if (irec->GetProductionCostPopModifier())
				{
					cost = irec->GetProductionCost() * PopCount();
				}
				else
				{
					cost = irec->GetProductionCost();
				}

		//sint32 cost = irec->GetProductionCost(); //original
		return m_build_queue.InsertTail(k_GAME_OBJ_TYPE_IMPROVEMENT, type, cost);
	}
	else
	{
		DPRINTF(k_DBG_GAMESTATE, ("%s can't build %s until %s is discovered\n",
		                          m_home_city.CD()->GetName(),
		                          g_theStringDB->GetNameStr(irec->GetName()),
		                          g_theAdvanceDB->GetNameStr(irec->GetEnableAdvanceIndex())));
		return false;
	}
}

bool CityData::BuildWonder(sint32 type)
{
	Assert(CanBuildWonder(type));
	if(!CanBuildWonder(type))
	{
#ifdef _DEBUG
		char error_msg[300];

		sprintf(error_msg, "Cannot build wonder %d",
			type);
#ifdef WIN32
		_RPT0(_CRT_WARN, error_msg);
#endif
#endif
		return false;
	}

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_WONDER,
		                     (uint32)m_home_city, type));
	}
	else if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_WONDER,
		                  (uint32)m_home_city, type));
		g_network.Unblock(m_owner);
	}

	const WonderRecord* rec = wonderutil_Get(type, m_owner);
	Assert(rec);
	if(rec == NULL)
		return false;

	DPRINTF(k_DBG_GAMESTATE, ("%s is building a wonder: %s\n", m_home_city.CD()->GetName(),
	                          g_theStringDB->GetNameStr(rec->m_name)));

	if(g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex()))
	{
		return m_build_queue.InsertTail(k_GAME_OBJ_TYPE_WONDER, type, rec->GetProductionCost());
	}
	else
	{
		return false;
	}
}

void CityData::AddWonder(sint32 type)
{
	const WonderRecord* rec = wonderutil_Get(type, m_owner); // Added by E
	MapPoint point(m_home_city.RetPos());

	m_builtWonders |= (uint64(1) << (uint64)type);

	// EMOD wonders add borders too
	sint32 intRad;
	sint32 sqRad;
	// EMOD increases city borders
	if (rec->GetIntBorderRadius(intRad) && rec->GetSquaredBorderRadius(sqRad))
	{
		GenerateBorders(point, m_owner, intRad, sqRad);
	}
// EMOD add HolyCity...need to link to religion DB (once Religion DB is done)
//	if (wonderutil_GetDesignatesHolyCity((uint64)1 << (uint64)type))
//	{
//		g_player[m_owner]->SetHolyCity(m_home_city);
//		g_player[m_owner]->RegisterNewHolyCity(m_home_city);
//	}
	// End Emod

	// EMOD Visible wonders 4-25-2006
	MapPoint SpotFound;
	CityInfluenceIterator it(point, static_cast<sint32>(GetVisionRadius()));  //m_sizeIndex

	sint32 s;
	for(s = 0; s < rec->GetNumShowOnMap(); s++)
	{
		const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(s);
		for(it.Start(); !it.End(); it.Next())
		{
			if(point == it.Pos())
				continue;

			if(terrainutil_HasUrban(it.Pos()))
				continue;

			if(terrainutil_HasWonder(it.Pos()))
				continue;

			if(terrainutil_CanPlayerSpecialBuildAt(trec, m_owner, it.Pos())
			){
				Cell *ncell = g_theWorld->GetCell(it.Pos());
				Cell *ocell = g_theWorld->GetCell(SpotFound);

				if ((!SpotFound.IsValid())
				|| ((ncell->GetNumDBImprovements() < ocell->GetNumDBImprovements())
				|| (ncell->GetGoldFromTerrain() > ocell->GetGoldFromTerrain()))
				){
					SpotFound = it.Pos();
				}
			}
		}

		g_player[m_owner]->CreateSpecialImprovement(rec->GetShowOnMapIndex(s), SpotFound, 0);
	}

		// EMOD - FU 4-1-2006 visible tileimps, but it builds
		// them all around the radius i.e. GreatWall builds Great walls
//		for(s = 0; s < rec->GetNumShowOnMapRadius(); s++)
//		{
//			const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(s);
//			if(!terrainutil_CanPlayerSpecialBuildAt(trec, m_owner, it.Pos()))
//			{
//				g_player[m_owner]->CreateSpecialImprovement(rec->GetShowOnMapRadiusIndex(s), it.Pos(), 0);
//			}
//		}
//	}
}

// Not used.
bool CityData::ChangeCurrentlyBuildingItem(sint32 category, sint32 item_type)
{
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendAction(new NetAction(NET_ACTION_CHANGE_BUILD,
										   (uint32)m_home_city, category,
										   item_type));
	}
	else if(g_network.IsHost())
	{
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
	sint32 oldCategory;
	if(m_build_queue.GetHead())
		oldCategory = m_build_queue.GetHead()->m_category;
	else
		oldCategory = -5;

	switch(category) { // see Globals.h for k_GAME_OBJ_TYPE enum
	case k_GAME_OBJ_TYPE_UNIT:
		urec = g_theUnitDB->Get(item_type, g_player[m_owner]->GetGovernmentType());
		Assert(urec);

		if(!urec)
			return false;

		Assert(CanBuildUnit(item_type));
		if(!CanBuildUnit(item_type))
		{
			return false;
		}

		if(g_player[m_owner]->HasAdvance(urec->GetEnableAdvanceIndex()))
		{
			cost = urec->GetShieldCost();
			m_build_queue.ReplaceHead(k_GAME_OBJ_TYPE_UNIT, item_type, cost);
		}
		else
		{
			return false;
		}
		break;
	case k_GAME_OBJ_TYPE_WONDER:
		wrec = wonderutil_Get(item_type, m_owner);
		Assert(wrec);

		if(!CanBuildWonder(item_type))
			return false;

		if(g_player[m_owner]->HasAdvance(wrec->GetEnableAdvanceIndex()))
		{
			cost = wrec->GetProductionCost();
			m_build_queue.ReplaceHead(k_GAME_OBJ_TYPE_WONDER, item_type, cost);
		}
		else
		{
			return false;
		}
		break;

	case k_GAME_OBJ_TYPE_IMPROVEMENT:
		irec = buildingutil_Get(item_type, m_owner);
		Assert(irec);

		if ((buildingutil_GetDesignatesCapitol((uint64)1 << item_type, m_owner)) &&
			(g_player[m_owner]->m_capitol->m_id != (0)))
		{
			SlicObject *so = new SlicObject("38IACapitolWarning");
			so->AddCity(*(g_player[m_owner]->m_capitol));
			so->AddCity(m_home_city);
			so->AddRecipient(m_owner);
			g_slicEngine->Execute(so);
		}

		if(CanBuildBuilding(item_type))
		{
			//ProductionCostPopModifier m_city.CD()->PopCount()
			//EMOD ProductionCostPopModifier  10-10-2006
			sint32 bcost = 0;
			if (irec->GetProductionCostPopModifier())
			{
				bcost = irec->GetProductionCost() * PopCount();
			}
			else
			{
				bcost = irec->GetProductionCost();
			}
			cost = bcost;  //end EMOD

			//cost = irec->GetProductionCost(); //original
			m_build_queue.ReplaceHead(k_GAME_OBJ_TYPE_IMPROVEMENT, item_type, cost);
		}
		else
		{
			return false;
		}
		break;

	case k_GAME_OBJ_TYPE_CAPITALIZATION:
		if(CanBuildCapitalization())
		{
			BuildCapitalization();
			m_shieldstore = 0;
			return true;
		}
		else
		{
			return false;
		}
	case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
		if(CanBuildInfrastructure())
		{
			BuildInfrastructure();
			m_shieldstore = 0;
			return true;
		}
		else
		{
			return false;
		}
	default:
		Assert(0);
		break;
	}

	if(category == m_build_category_at_begin_turn)
	{
		m_shieldstore = m_shieldstore_at_begin_turn;
	}
	else
	{
		double penalty = static_cast<double>(g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty());

		penalty = std::min<double>
		                  (
		                   1.0,
		                   std::max<double>(0.0, 1.0 - penalty * 0.01)
		                  );

		m_shieldstore = static_cast<sint32>(static_cast<double>(m_shieldstore_at_begin_turn) * penalty);

	}

	return true;
}

void CityData::DestroyCapitol()
{
	if(buildingutil_GetDesignatesCapitol(m_built_improvements, m_owner))
	{
		for(uint64 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			if(buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)i, m_owner) &&
			   m_built_improvements & uint64((uint64)1 << i))
			{
				m_built_improvements &= ~((uint64)1 << i);
			}
		}
	}

	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);
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

//	g_player[m_owner]->RegisterLostBuilding(m_home_city, imp); Maybe worth for reimplementation
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);
}

void CityData::NewGovernment(sint32 government_type)
{
	m_build_queue.RemoveIllegalItems(true);

	if (government_type == g_player[m_owner]->m_government_type)
		return;

	// Recalculation of resources?
}

double CityData::GetDefendersBonus() const
{
	// EMOD add population as a contributor to defense for AI, to make larger cities even tougher. It takes total population * defense coefficient * percentage of people that are happy (and most likely to resist)
	if((g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAICityDefenderBonus()
	||  g_theProfileDB->IsAICityDefenderBonus())
	&& g_player[m_owner]->IsRobot()
	){
		return m_defensiveBonus * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetDefenseCoef() + (PopCount() * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetDefenseCoef()) * (m_happy->GetHappiness() * .01);
	}
	else
	{
		return m_defensiveBonus * g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetDefenseCoef();
	}
}

double CityData::GetDefendersBonusNoWalls() const
{
	double b;
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), b, m_owner);

// EMOD - add influence or culture defese bonus here?
// EMOD - compute reductions in defense by siege units here?

	return b;
}

double CityData::GetCityLandAttackBonus() const // Improve
{
	double b = buildingutil_GetOffenseBonusLand(GetEffectiveBuildings(), m_owner);

	return b;
}

double CityData::GetCityAirAttackBonus() const
{
	double b = buildingutil_GetOffenseBonusAir(GetEffectiveBuildings(), m_owner);

	return b;
}

double CityData::GetCitySeaAttackBonus() const
{
	double b = buildingutil_GetOffenseBonusWater(GetEffectiveBuildings(), m_owner);

	return b;
}

// not used (Hospitals ?)
void CityData::ImprovementHealUnitsInCity() const
{
#if 0
	CellUnitList a;
	MapPoint pos;

	m_home_city.GetPos(pos);
	g_theWorld->GetArmy(pos, a);

	sint32 n = a.Num();
	for (sint32 i=0; i<n; i++)
	{
		if (buildingutil_GetMovementTypeIsHealed(GetEffectiveBuildings(), a[i]))
		{
			a[i].SetHPToMax();
		}
	}
#endif
}

//no function: buildingutil_GetMovementTypeIsRefueled
void CityData::ImprovementRefuelUnitsInCity() const
{
#if 0
	CellUnitList a;
	MapPoint pos;

	m_home_city.GetPos(pos);
	g_theWorld->GetArmy(pos, a);

	sint32 n = a.Num();
	for (sint32 i=0; i<n; i++)
	{
		if (buildingutil_GetMovementTypeIsRefueled(GetEffectiveBuildings(), a[i]))
		{
			a[i].SetFuelToMax();
		}
	}
#endif
}

bool CityData::ImprovementCanRefuel(const Unit &u) const
{
	return true;
}

void CityData::AddHappyTimer(sint32 turns, double adjust,
                             HAPPY_REASON reason)
{
	m_happy->AddTimer(turns, adjust, reason);
}

double CityData::GetImprovementCrimeMod() const
{
	return buildingutil_GetLowerCrime(GetEffectiveBuildings(), m_owner);
}

sint32 CityData::GetImprovementPeaceMod() const
{
	return sint32(buildingutil_GetLowerPeaceMovement(GetEffectiveBuildings(), m_owner));
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
	switch(m_cityRadiusOp)
	{
		case RADIUS_OP_REMOVE_IMPROVEMENTS:
			g_theWorld->CutImprovements(pos);
			break;
		case RADIUS_OP_KILL_UNITS:
			if(g_theWorld->GetCell(pos)->UnitArmy())
			{
				CellUnitList *units = g_theWorld->GetCell(pos)->UnitArmy();
				for(sint32 i = 0; i < units->Num(); i++)
				{
					if(!units->Access(i)->Flag(k_UDF_MAD_LAUNCHED))
						m_killList->Insert(units->Access(i));
				}
			}
			break;
		case RADIUS_OP_RESET_OWNER:
		{
#if 0
			Cell *cell = g_theWorld->GetCell(pos);
			if(cell->GetOwner() == m_owner)
			{
				MapPoint myPos;
				m_home_city.GetPos(myPos);

				if((pos == myPos) ||
				   !g_theWorld->IsInsideCityRadiusOfPlayerOtherThan(pos, m_radiusNewOwner, m_home_city))
				{
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

			if(m_tilecount == m_whichtile)
			{
				Cell *cell = g_theWorld->GetCell(pos);
				if(cell->GetCanDie())
				{
					MapPoint tmp = pos;

					cell->Kill();

					g_theWorld->CutImprovements(pos);

					cell->CalcTerrainMoveCost();
					MapPoint nonConstPos = pos;

					g_tiledMap->PostProcessTile(nonConstPos, g_theWorld->GetTileInfo(nonConstPos));
					g_tiledMap->TileChanged(nonConstPos);
					MapPoint npos;
					for(WORLD_DIRECTION d = NORTH; d < NOWHERE;
						d = (WORLD_DIRECTION)((sint32)d + 1))
					{
						if(pos.GetNeighborPosition(d, npos))
						{
							g_tiledMap->PostProcessTile(
								npos,
								g_theWorld->GetTileInfo(npos));
							g_tiledMap->TileChanged(npos);
							g_tiledMap->RedrawTile(&npos);
						}
					}
					g_tiledMap->RedrawTile(&pos);

					if(g_network.IsHost())
					{
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
			if(g_theWorld->IsGood(pos))
			{
				sint32 good;
				g_theWorld->GetGood(pos, good);
				if(!m_tempGoodAdder->HaveGoodOfType(good))
					m_tempGoodAdder->AddGood(good);
			}
			break;
		case RADIUS_OP_COUNT_GOODS:
			if(g_theWorld->IsGood(pos))
			{
				sint32 good;
				g_theWorld->GetGood(pos, good);
				if(good == m_tempGood || m_tempGood < 0)
					m_tempGoodCount++;
			}
			break;
		default:
			Assert(false);
			break;
	}
}

void CityData::CityToPark(sint32 agressor)
{
	MapPoint cpos;
	m_home_city.GetPos(cpos);

	m_cityRadiusOp = RADIUS_OP_REMOVE_IMPROVEMENTS;
	CityRadiusIterator(cpos, this);

	UnitDynamicArray killList;
	m_killList = &killList;
	m_cityRadiusOp = RADIUS_OP_KILL_UNITS;
	CityRadiusIterator(cpos, this);

	UnitDynamicArray	tempKillList;
	tempKillList.Concat(*m_killList);

	m_home_city.Kill(CAUSE_REMOVE_ARMY_PARKRANGER, agressor);

	for (sint32 i = 0; i < tempKillList.Num(); i++)
	{
		if(g_theUnitPool->IsValid(tempKillList[i]))
		{
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
	sint32 kill = sint32(double(pn) * g_theConstDB->Get(0)->GetNukePopulationPercentage());

	if(pn >= 3)
	{
		for(sint32 i = 0; i < kill; i++)
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
			                       GEA_City, m_home_city.m_id,
			                       GEA_End);
		}
	}
	else
	{
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

bool CityData::SafeFromNukes() const
{
	return buildingutil_GetProtectFromNukes(GetEffectiveBuildings(), m_owner);
}

// called by TiledMap::DrawCityNames
bool CityData::HasAirport() const
{
	return buildingutil_GetAirport(GetEffectiveBuildings(), m_owner);
}

bool CityData::HasCityWalls() const
{
	return buildingutil_GetCityWalls(GetEffectiveBuildings(), m_owner);
}

bool CityData::HasForceField() const
{
	return buildingutil_GetForceField(GetEffectiveBuildings(), m_owner);
}

void CityData::UseAirport()
{
	m_airportLastUsed = g_turn->GetRound();
}

sint32 CityData::AirportLastUsed() const
{
	return m_airportLastUsed;
}

bool CityData::HasBeenSpiedUpon() const
{
	return m_spied_upon != 0;
}

void CityData::SetSpiedUpon()
{
	m_spied_upon = g_theConstDB->Get(0)->GetSpiedUponWarinessTimer();
}

//see ORDER_RESULT UnitData::NullifyWalls
void CityData::CityNullifyWalls()
{
	m_walls_nullified = true;
}

bool CityData::IsCapitol() const
{
	return buildingutil_GetDesignatesCapitol(GetEffectiveBuildings(), m_owner);
}

void CityData::SetCapitol()
{
	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if(buildingutil_Get(i, m_owner)->GetCapitol())
		{
			m_built_improvements |= ((uint64)1 << i);

			return;
		}
	}
}

//  Added by E to add religions?
//bool CityData::IsHolyCity() const
//{
//	return Wonderutil_GetDesignatesHolyCity(g_player[m_owner]->m_builtWonders);
//}
// Holy city stuff: I should delete it after modification of SetCapitol
//	if (IsHolyCity) { //(isWatchful)
//		cityIcon = tileSet->GetMapIconData(wrec->ReligionIcon()); //MAPICON_WATCHFUL
//		Assert(cityIcon);
//		if (!cityIcon) return;
//		cityIcon = tileSet->GetMapIconData(wrec->ReligionIcon());
//		iconDim = tileSet->GetMapIconDimensions(wrec->ReligionIcon());

//		color = GetColor(COLOR_YELLOW, fog);
//		DrawColorizedOverlay(cityIcon, surf, iconRect.left, iconRect.top, color);
//		AddDirtyRectToMix(iconRect);

//		iconRect.left += iconDim.x;
//		iconRect.right += iconDim.x;

void CityData::MakeFranchise(sint32 player)
{
	if(m_franchise_owner != player){// remove foreign franchise to update city icon for former owner
	    if(m_franchise_owner > 0){// m_franchise_owner < 0 means not franchised
		SlicObject *so = new SlicObject("911SueFranchiseCompleteVictim");
		so->AddRecipient(GetFranchiseOwner());
		so->AddCity(m_home_city);
		so->AddGold(0);// misuseing AddGold for passing the remaining franchise turns
		so->AddGold(GetProductionLostToFranchise());
		g_slicEngine->Execute(so);
		}

	    RemoveFranchise();// must be after message because m_franchise_owner is set to -1
	    }

	m_franchise_owner = player;
	m_franchiseTurnsRemaining = g_theConstDB->Get(0)->GetTurnsFranchised(); // do not use SetFranchiseTurnsRemaining here, as it resets the owner for -1

	ProcessAllResources(); // updates m_productionLostToFranchise which is used for drawing loss on franchise city-icon, takes NEW_RESOURCE_PROCESS into account
}

void CityData::RemoveFranchise()
{
	if(m_franchise_owner < 0) // according to CityData::Unconvert
		return;

	//// let the owner of the franchise know about its removal, see https://github.com/civctp2/civctp2/pull/166
	MapPoint pos; // needed to get un-seen cell at this city pos
	UnseenCellCarton ucell;

	m_home_city.GetPos(pos);
	if(g_player[m_franchise_owner]->GetLastSeen(pos, ucell)) // get un-seen cell of franchise owner
	{
		ucell.m_unseenCell->SetIsFranchised(false); // remove franchise flag
	}

	m_franchise_owner = -1;
	m_franchiseTurnsRemaining = -1;
}

sint32 CityData::GetFranchiseTurnsRemaining() const
{
	return m_franchiseTurnsRemaining;
}

void CityData::SetFranchiseTurnsRemaining(sint32 turns)
{
	m_franchiseTurnsRemaining = turns;
	if(turns < 0)// < 0: remove instantly; 0: beginning of next turn, i.e. turns + 1 
	{
		RemoveFranchise();
	}
}

bool CityData::IsWatchful() const
{
	return (m_watchfulTurns > 0);
}

void CityData::SetWatchful()
{
	m_watchfulTurns = g_theConstDB->Get(0)->GetWatchfulCityTurns();
}

void CityData::ModifySpecialAttackChance(UNIT_ORDER_TYPE attack,
                                         double &chance)
{
	if(IsWatchful()) {
		chance *= g_theConstDB->Get(0)->GetWatchfulCitySuccessModifier();
	}
	switch(attack) {
		case UNIT_ORDER_BIO_INFECT:
		case UNIT_ORDER_PLAGUE:
			chance -= buildingutil_GetProtectFromBioAgents(GetEffectiveBuildings(), m_owner);
			break;
		case UNIT_ORDER_NANO_INFECT:
			chance -= buildingutil_GetProtectFromNanoVirus(GetEffectiveBuildings(), m_owner);
			break;
		case UNIT_ORDER_SLAVE_RAID:
			chance -= buildingutil_GetPreventSlavery(GetEffectiveBuildings(), m_owner);
			break;
	}
}

void CityData::RemoveOneSlave(PLAYER_INDEX p)
{
	if (SlaveCount() > 0)
	{
		ChangeSpecialists(POP_SLAVE, -1);
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
		                       GEA_City,        m_home_city.m_id,
		                       GEA_End
		                      );
	}
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
	   m_owner == g_slicEngine->GetTutorialPlayer())
	{
		return;
	}

	if (cause == UPRISING_CAUSE_INTERNAL)
		cause = m_doUprising;

	m_doUprising = UPRISING_CAUSE_NONE;

	sint32 cheapUnit = g_player[m_owner]->GetCheapestMilitaryUnit();
	Assert(cheapUnit >= 0);
	if (cheapUnit < 0)
		return;

	sint32 numSlaves = SlaveCount();
	PLAYER_INDEX si = civilisation_NewCivilisationOrVandals(m_owner);

	MapPoint cpos;
	m_home_city.GetPos(cpos);

	if(numSlaves > k_MAX_ARMY_SIZE)
		numSlaves = k_MAX_ARMY_SIZE;

	CellUnitList slaveArmy;
	sint32 i;
	for(i = 0; i < numSlaves; i++)
	{
		Unit u = g_player[si]->CreateUnitNoPosition(cheapUnit,
		                                            slaveArmy,
		                                            cpos,
		                                            m_owner);
		Assert(u.m_id != (0));
		if(u.m_id != 0)
		{
			u.SetPosAndNothingElse(cpos);
			u.SetTempSlaveUnit(true);
		}
	}

	Army sa = g_player[si]->GetNewArmy(CAUSE_NEW_ARMY_UPRISING);
	for(i = 0; i < numSlaves; i++)
	{
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

	if (g_network.IsHost())
	{
		g_network.Block(oldOwner);
		g_network.Enqueue(g_theArmyPool->AccessArmy(sa));
		g_network.Unblock(oldOwner);
	}

	bool startedBattle = false;
	if (numPossibleDefenders > 0)
	{
		CellUnitList defenders;
		for (sint32 i = 0; i < numPossibleDefenders; ++i)
		{
			if (cell->AccessUnit(i).GetDBRec()->GetMovementTypeLand())
			{
				defenders.Insert(cell->AccessUnit(i));
			}
		}

		if (defenders.Num() > 0)
		{
			sa.Fight(defenders);
			startedBattle = true;
		}
	}

	if (cause != UPRISING_CAUSE_INCITED)
	{
		SlicObject *so = new SlicObject("206CrisisSlaveRevolt");
		so->AddCity(m_home_city);
		so->AddRecipient(m_owner);
		g_slicEngine->Execute(so);
	}

	if (startedBattle)
	{
		sa->IncrementDontKillCount();
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CleanupUprising,
		                       GEA_Army, sa,
		                       GEA_City, m_home_city.m_id,
		                       GEA_End);
	}
	else
	{
		CleanupUprising(sa);
	}
}

void CityData::CleanupUprising(Army &sa)
{
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());

	sint32 oldOwner = m_owner;
	if(!g_theArmyPool->IsValid(sa) || sa.Num() < 1)
	{
		DPRINTF(k_DBG_GAMESTATE, ("The uprising was crushed\n"));

		RemoveOneSlave(GetOwner());
	}
	else
	{
		sint32 si = sa.GetOwner();
		DPRINTF(k_DBG_GAMESTATE, ("The uprising succeeded\n"));

		if(cell->UnitArmy())
		{
			cell->UnitArmy()->KillList(CAUSE_REMOVE_ARMY_SLAVE_UPRISING, GetOwner());
		}

		for(sint32 i = sa.Num() - 1; i >= 0; i--)
		{
			if(sa[i].GetHP() < 1)
				sa.DelIndex(i);
			else
			{
				sa[i].SetTempSlaveUnit(false);
				sa[i].SetPosAndNothingElse(m_home_city.RetPos());
				sa[i].AddUnitVision();

				g_theWorld->InsertUnit(m_home_city.RetPos(), sa[i]);
				g_player[sa.GetOwner()]->InsertUnitReference(sa[i],
												  CAUSE_NEW_ARMY_UPRISING,
												  m_home_city);
				if(g_network.IsHost())
				{
					g_network.Block(oldOwner);
					g_network.Enqueue(new NetInfo(NET_INFO_CODE_MAKE_UNIT_PERMANENT,
												  sa[i].m_id));
					g_network.Unblock(oldOwner);
				}
			}
		}
		sa.ResetPos();

		ChangeSpecialists(POP_SLAVE, -SlaveCount());

		m_home_city.ResetCityOwner(si, false, CAUSE_REMOVE_CITY_SLAVE_UPRISING);
	}
}

void CityData::Plague(sint32 player)
{
	sint32 pn = PopCount();
	sint32 kill = sint32(double(pn) * g_theConstDB->Get(0)->GetPlagueKillPercentage());

	if(kill < 1 && pn > 1)
	{
		kill = 1;
	}

	for(sint32 i = 0; i < kill; i++)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillPop,
		                       GEA_City, m_home_city.m_id,
		                       GEA_End);
	}
}

void CityData::BioInfect( sint32 player )
{
	m_bioInfectedBy = player;
	m_bioInfectionTurns = g_theConstDB->Get(0)->GetBioInfectionTurns();
	AddHappyTimer(g_theConstDB->Get(0)->GetBioInfectionTurns(),
	              g_theConstDB->Get(0)->GetBioInfectionUnhappiness(),
	              HAPPY_REASON_BIO_INFECTION);
}

void CityData::NanoInfect( sint32 player )
{
	m_nanoInfectedBy = player;
	m_nanoInfectionTurns = g_theConstDB->Get(0)->GetNanoInfectionTurns();
	DPRINTF(k_DBG_GAMESTATE, ("City %lx: all buildings and wonders destroyed\n", uint32(m_home_city)));

	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if(HasBuilding(i))
		{
			if(g_rand->Next(100) < (g_theConstDB->Get(0)->GetNanoBuildingKillPercentage() * 100.0))
			{
				DestroyImprovement(i);
			}
		}
	}
}

void CityData::SpreadBioTerror()
{
	Unit c;
	sint32 n = m_tradeSourceList.Num();
	for (sint32 i = 0; i < n; i++)
	{
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

		if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetBioInfectionSpreadChance()
		                              * 100.0))
		{
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
	sint32 n = m_tradeSourceList.Num();
	for (sint32 i = 0; i < n; i++)
	{
		// FIXME: See comment in CityData::SpreadBioTerror above - same applies here.
		c = m_tradeSourceList[i].GetDestination();
		if ((c.IsNanoImmune()) || (c.IsNanoInfected()))
			continue;

		if(g_rand->Next(100) < sint32(g_theConstDB->Get(0)->GetNanoInfectionSpreadChance()
		                              * 100.0))
		{
			c.NanoInfect(0);
			SlicObject *so = new SlicObject("047InfectedViaTrade");
			so->AddCity(c);
			so->AddRecipient(c.GetOwner());
			g_slicEngine->Execute(so);
		}
	}
}

bool CityData::IsBioImmune() const
{
	return wonderutil_GetProtectFromBiologicalWarfare(g_player[m_owner]->m_builtWonders);
}

bool CityData::IsNanoImmune() const
{
	return wonderutil_GetProtectFromBiologicalWarfare(g_player[m_owner]->m_builtWonders);
}

bool CityData::IsProtectedFromConversion()
{
	return wonderutil_PreventConversion(g_player[m_owner]->m_builtWonders);
}

void CityData::ConvertTo(sint32 player, CONVERTED_BY by)
{
	if(IsProtectedFromConversion())
		return;

	if(m_convertedTo != player){// remove foreign conversion to update city icon for former owner
	    if(m_convertedTo > 0){// m_convertedTo < 0 means not converted
		SlicObject *so = new SlicObject("135ReformCityVictim");
		so->AddRecipient(IsConvertedTo());
		so->AddCity(m_home_city);
		so->AddGold(GetConvertedGold());
		g_slicEngine->Execute(so);
		}

	    Unconvert(false);// do not cause unhappiness in this case, has to be after message (sets m_convertedTo to -1)
	    }
	
	m_convertedTo = player;
	m_convertedBy = by;

	ProcessAllResources(); // updates m_convertedGold which is used for drawing loss on conversion city-icon, takes NEW_RESOURCE_PROCESS into account
}

double CityData::TheologicalModifier() const
{
	return buildingutil_GetPreventConversion(GetEffectiveBuildings(), m_owner);
}

void CityData::Unconvert(bool makeUnhappy)
{
	if(m_convertedTo < 0)
		return;

	//// let the owner of the conversion know about its removal, see https://github.com/civctp2/civctp2/pull/166
	MapPoint pos; // needed to get un-seen cell at this city pos
	UnseenCellCarton ucell;

	m_home_city.GetPos(pos);
	if(g_player[m_convertedTo]->GetLastSeen(pos, ucell)) // get un-seen cell of conversion owner
	{
		ucell.m_unseenCell->SetIsConverted(false); // remove conversion flag
	}

	m_convertedTo = -1;
	m_convertedGold = 0;
	if(makeUnhappy) {
		AddHappyTimer(g_theConstDB->Get(0)->GetReformationHappinessTime(),
		              g_theConstDB->Get(0)->GetReformationHappinessAmount(),
		              HAPPY_REASON_REFORMATION);
	}
}

//this city is collecting more sint32 resource than than it is selling
bool CityData::HasResource(sint32 resource) const
{
	return m_collectingResources[resource] > m_sellingResources[resource];
}

//Added by E -- this city is collecting or buying some sint32 resource
bool CityData::HasNeededGood(sint32 resource) const
{ //3-27-2006 added selling resource impact
	return m_buyingResources[resource] + m_collectingResources[resource] - m_sellingResources[resource] > 0;
}

//EMOD - add GoodIsPirated? 3-13-2006

//this city is collecting some sint32 resource
bool CityData::IsLocalResource(sint32 resource) const
{
	return m_collectingResources[resource] > 0;
}

void CityData::AddGoodToCity(sint32 good)
{
	m_collectingResources.AddResource(good);
}

bool CityData::HasTileImpInRadius(sint32 tileimp) const
{
	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);

	for(it.Start(); !it.End(); it.Next())
	{
		Cell *cell = g_theWorld->GetCell(it.Pos());

		for(sint32 i = 0; i < cell->GetNumDBImprovements(); ++i)
		{
			if(cell->GetDBImprovement(i) == tileimp)
			{
				return true;
			}
		}
	}

	return false;
}

bool CityData::HasAnyTileImpInRadiusAndIsExploredBy(const sint32 player) const
{
	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);

	for(it.Start(); !it.End(); it.Next())
	{
		Cell *cell = g_theWorld->GetCell(it.Pos());

		if
		  (
		       cell->GetNumDBImprovements() > 0
		    && g_player[player]->IsExplored(it.Pos())
		  )
		{
			return true;
		}
	}

	return false;
}

//filters out non-resource trade
bool CityData::GetResourceTradeRoute(sint32 resource, TradeRoute & route) const
{
	for (sint32 i = 0; i < m_tradeSourceList.Num(); i++)
	{
		ROUTE_TYPE type;
		sint32 rr;
		m_tradeSourceList[i].GetSourceResource(type, rr);

		if ((type == ROUTE_TYPE_RESOURCE) && (rr == resource))
		{
			route = m_tradeSourceList[i];
			return true;
		}
	}
	return false;
}

// filters out non-resource trade
bool CityData::IsSellingResourceTo(sint32 resource, Unit & destination) const
{
	for (sint32 i = 0; i < m_tradeSourceList.Num(); i++)
	{
		ROUTE_TYPE type;
		sint32 rr;
		m_tradeSourceList[i].GetSourceResource(type, rr);

		if ((type == ROUTE_TYPE_RESOURCE) && (rr == resource))
		{
			destination = m_tradeSourceList[i].GetDestination();
			return true;
		}
	}
	destination.m_id = 0;
	return false;
}

sint32 CityData::LoadQueue(const MBCHAR *file)
{
	m_build_category_before_load_queue = GetBuildQueue()->GetHead() != NULL ? GetBuildQueue()->GetHead()->m_category : k_GAME_OBJ_TYPE_NOTHING;

	sint32 r = m_build_queue.Load(file);

	m_build_queue.SetOwner(m_owner);

	BuildNode* head = GetBuildQueue()->GetHead();
	if(head != NULL)
	{
		// Get new queue top item
		sint32 newCat = GetBuildQueue()->GetHead()->m_category;
		if(newCat != GetBuildCategoryBeforeLoadQueue()
		&& GetStoredCityProduction() > 0
		&& m_build_category_at_begin_turn != -5)// -5 is the no penalty type.
		{
			CheckSwitchProductionPenalty(newCat);
		}
	}
	else
	{
		// ToDo: Check whether the player should be informed or informed one function above
	}

	return r;
}

//----------------------------------------------------------------------------
//
// Name       : CheckSwitchProductionPenalty
//
// Description: Deduct shields stored when top production item was switched
//				to a different type.
//
// Parameters : newCat - Set the category of the new item that is the top of
//				the queue, so we can find it next time.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void CityData::CheckSwitchProductionPenalty(sint32 newCat)
{
	if (GetStoredCityProduction() > 0)
	{
		sint32 s = 0;

		double penalty = static_cast<double>(g_theConstDB->Get(0)->GetChangeCurrentlyBuildingItemPenalty());

		penalty = std::min<double>
						  (
						   1.0,
						   std::max<double>(0.0, 1.0 - penalty * 0.01)
						  );

		s = static_cast<sint32>(static_cast<double>(GetStoredCityProduction()) * penalty);
		SetShieldstore(s);
		SetBuildCategoryAtBeginTurn(newCat);
	}
	else
	{
		// Set this even for cities with no shields stored,
		// so we know the last item type the next time a switch happens.
		SetBuildCategoryAtBeginTurn(newCat);
	}
}

sint32 CityData::SaveQueue(const MBCHAR *file)
{
	sint32 r = m_build_queue.Save(file);
	m_build_queue.SetOwner(m_owner);
	return r;
}

uint32 CityData_CityData_GetVersion(void)
{
	return (k_CITYDATA_VERSION_MAJOR<<16 | k_CITYDATA_VERSION_MINOR);
}

void CityData::ResetCityOwner(sint32 owner)
{
	sint32	i, n;

	m_capturedThisTurn = true;

	m_cityRadiusOp = RADIUS_OP_RESET_OWNER;
	m_radiusNewOwner = owner;
	MapPoint pos;
	m_home_city.GetPos(pos);
	CityRadiusIterator(pos, this);

	m_doUprising = UPRISING_CAUSE_NONE;

	CityInfluenceIterator it(pos, m_sizeIndex);
	for(it.Start(); !it.End(); it.Next())
	{
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

	m_walls_nullified = false;
	RemoveFranchise(); // Franchise removed in contrast to conversion, see below
	m_watchfulTurns = 0;
	m_bioInfectionTurns = 0;
	m_bioInfectedBy = -1;
	m_nanoInfectionTurns = 0;
	m_nanoInfectedBy = -1;
	if(m_convertedTo == m_owner)
	{
		m_convertedTo = -1;
	}
	m_happinessAttackedBy = PLAYER_UNASSIGNED;
	m_isInjoined = false;

	m_happy->ClearTimedChanges();

	FindBestSpecialists();

	sint32 poptype;
	for(poptype = POP_WORKER + 1; poptype < POP_SLAVE; poptype++)
	{
		if(m_numSpecialists[poptype] > 0 && m_specialistDBIndex[poptype] < 0)
		{
			ChangeSpecialists((POP_TYPE)poptype, -m_numSpecialists[poptype]);
		}
	}

	m_lastCelebrationMsg = -1;
	//emod TODO maybe borders should be set to zero once captured and restored after conquestdistress?
	GenerateBorders(m_home_city.RetPos(), m_owner, g_theConstDB->Get(0)->GetBorderIntRadius(), g_theConstDB->Get(0)->GetBorderSquaredRadius());

	if(m_owner == m_founder) {
		g_player[m_owner]->m_score->AddCityRecaptured();
		g_featTracker->AddFeat("FEAT_CITY_RECAPTURED", m_owner);
	}
}

void CityData::SetNameLocal(const MBCHAR *name)
{
	delete[] m_name;
	m_name = new MBCHAR[strlen(name) + 1];
	strncpy(m_name, name, strlen(name) + 1);
}

void CityData::SetName(const MBCHAR *name)
{
	SetNameLocal(name);
	if(g_network.IsHost())
	{
		g_network.SendCityName(this);
	}
	else if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendCityName(this);
	}
}

sint32 CityData::GetOvertimeCost()
{
	if (m_build_queue.GetHead() == NULL)
	{
		// TODO: return 0 when nothing in queue.
		// But check usage (e.g. in ctpai.cpp) before doing so.
		return 1000000;
	}

	double type_coeff = m_build_queue.GetTypeCoeff();
	double percent_remaining = m_build_queue.GetPercentRemaining(m_shieldstore);
	sint32 production_remaining = m_build_queue.GetProductionRemaining(m_shieldstore);
	if(production_remaining < 0)
		production_remaining = 0;

	if(buildingutil_NoRushBuyPenalty(GetEffectiveBuildings(), m_owner))
	{
		percent_remaining = 0;
		type_coeff = 1;
	}

	sint32 c = sint32(type_coeff * (1 + percent_remaining) * double(production_remaining));


	if(c < 0)
		c = 0;
	return c;
}

bool CityData::BuyFront()
{
	if(!m_build_queue.GetHead())
		return false;

	// * Can't rush buy capitalization/infrastructure
	if(IsBuildingInfrastructure() || IsBuildingCapitalization())
		return false;

	//cant rush wonders?
	//g_theWonderTracker->SetBuildingWonder(m_wonderStarted, m_owner)
	// if(m_build_queue.Get

	if(m_shieldstore >= m_build_queue.GetFrontCost())
	{
		return true;
	}

	if(g_network.IsClient())
	{
		g_network.SendAction(new NetAction(NET_ACTION_BUY_FRONT,
		                     (uint32)m_home_city));
	}

	if(!m_paidForBuyFront)
	{
		if(!PayForBuyFront())
			return false;
	}

	m_shieldstore = m_build_queue.GetFrontCost();

	m_paidForBuyFront = false;

	return true;
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
	for(i = 0; i < n; i++)
	{
		enable = g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->m_enable;
		obsolete = g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0)))
		{
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
			                          g_theStringDB->GetNameStr(g_theUnitDB->Get(i)->m_name)));
		}
	}

	DPRINTF(k_DBG_GAMESTATE, ("Improvements:\n"));
	n = g_theBuildingDB->NumRecords();
	for(i = 0; i < n; i++)
	{
		enable = buildingutil_Get(i, m_owner)->m_enable;
		obsolete = buildingutil_Get(i, m_owner)->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0)))
		{
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
			                          g_theStringDB->GetNameStr(buildingutil_Get(i, m_owner)->m_name)));
		}
	}

	DPRINTF(k_DBG_GAMESTATE, ("Wonders:\n"));
	n = g_theWonderDB->NumRecords();
	for(i = 0; i < n; i++)
	{
		const WonderRecord *rec = wonderutil_Get(i);
		enable = rec->m_enable;
		obsolete = rec->m_obsolete;
		if((p->m_advances->HasAdvance(enable) || (enable < 0)) &&
		   ((!p->m_advances->HasAdvance(obsolete)) || (obsolete < 0)))
		{
			DPRINTF(k_DBG_GAMESTATE, ("  %d(%s)\n", i,
			                          g_theStringDB->GetNameStr(rec->m_name)));
		}
	}
#endif
}

// Used? - Yes.
sint32 CityData::HowMuchLonger() const
{
	if(m_build_queue.GetLen() < 1)
		return 0x7fffffff;

	sint32 prod_remaining = m_build_queue.GetProductionRemaining(m_shieldstore);

	sint32 prod = GetNetCityProduction();

	if(m_contribute_military)
	{
		sint32 mil = (sint32)ceil((double)prod * g_player[m_owner]->m_readiness->GetPecentLastTurn());
		prod -= mil;
	}

	if(m_contribute_materials)
	{
		sint32 mat = (sint32)ceil((double)prod * g_player[m_owner]->m_materialsTax);
		prod -= mat;
	}

	if (prod <= 0)
		return 0x7fffffff;

	sint32 turns_remaining = (sint32)ceil((double)prod_remaining / prod);

	if ( turns_remaining < 1 )
	{
		return 1;
	}

	if(m_buyFront && m_paidForBuyFront)
	{
		return 1;
	}

	return turns_remaining;
}

// Used? - Yes.
sint32 CityData::HowMuchLonger(sint32 productionRemaining) const
{
	sint32 prod_remaining = productionRemaining;

	sint32 prod = GetNetCityProduction();

	if(m_contribute_military)
	{
		sint32 mil = (sint32)ceil((double)prod * g_player[m_owner]->m_readiness->GetPecentLastTurn());
		prod -= mil;
	}

	if(m_contribute_materials)
	{
		sint32 mat = (sint32)ceil((double)prod * g_player[m_owner]->m_materialsTax);
		prod -= mat;
	}

	if (prod <= 0)
		return 0x7fffffff;

	sint32 turns_remaining = (sint32)ceil((double)prod_remaining / prod);

	if ( turns_remaining < 1 )
	{
		return 1;
	}

	return turns_remaining;
}

void CityData::SellBuilding(sint32 which, bool byChoice)
{
	if(HasBuilding(which))
	{
		if(byChoice)
		{
			if(m_alreadySoldABuilding)
				return;

			//EMOD to make unsellable buildings
			if(buildingutil_Get(which, m_owner)->GetCantSell())
				return;

			if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
			{
				g_network.SendAction(new NetAction(NET_ACTION_SELL_BUILDING,
					(uint32)m_home_city,
					which));
			}
			else if(g_network.IsHost())
			{
				g_network.Block(m_owner);
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_SOLD_BUILDING,
											  (uint32)m_home_city, which));
			}

			m_alreadySoldABuilding = true;
		}
		sint32 gold = sint32(double(buildingutil_Get(which, m_owner)->GetProductionCost()) *
			g_theConstDB->Get(0)->GetBuildingProductionToValueModifier());
		if(byChoice)
			g_player[m_owner]->m_gold->AddGold(gold);
		else
			m_net_gold += gold;
		m_built_improvements &= ~((uint64)1 << uint64(which));

//		g_player[m_owner]->RegisterLostBuilding(m_home_city, which); Maybe worth of reimplementation
		m_build_queue.RemoveIllegalItems(true);
		if(buildingutil_GetDesignatesCapitol(((uint64)1 << (uint64)which), m_owner))
		{
			Assert(g_player[m_owner]->m_capitol->m_id == m_home_city.m_id);
			if(g_player[m_owner]->m_capitol->m_id == m_home_city.m_id)
			{
				g_player[m_owner]->m_capitol->m_id = 0;
			}
		}

		// Selling a building may impact the defensive bonus
		buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);

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

	for (sint32 i = 0; i < size - PopCount(); i++)
	{
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

	for (sint32 i = 0; i < m_tradeSourceList.Num(); i++)
	{
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

	for(sint32 i = 0; i < cell->GetNumUnits(); i++)
	{
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
//            : Added PrerequisiteBuilding check to see if a city has a building
//              needed to build a unit.
//            : Added NeedsCityGood checks unit good and sees if the city
//              has the either good in its radius or is buying the good from trade.
//            : Added NeedsCityGoodAll checks unit good and sees if the city
//              has all of the goods in its radius or is buying the good from trade.
//
//----------------------------------------------------------------------------
bool CityData::CanBuildUnit(sint32 type) const
{
	if(!g_player[m_owner]->CanBuildUnit(type))
		return false;

	const UnitRecord *rec = g_theUnitDB->Get(type, g_player[m_owner]->GetGovernmentType());
	if(!rec)
		return false;

	MapPoint pos;
	m_home_city.GetPos(pos);

	// Added by E - units can be obsolete by the availability of other units
	if(rec->GetNumObsoleteUnit() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumObsoleteUnit(); i++)
		{
			if(CanBuildUnit(rec->GetObsoleteUnitIndex(i)))
				return false;
		}
	}

	// Added by E - checks if a city has a building required to build the unit
	if(rec->GetNumPrerequisiteBuilding() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumPrerequisiteBuilding(); i++)
		{
			if(!HasEffectiveBuilding(rec->GetPrerequisiteBuildingIndex(i)))
				return false;
		}
	}

	// EMOD reverse of prereq - this building prevents other buildings
	// good for state religion etc
	if(rec->GetNumExcludedByBuilding() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumExcludedByBuilding(); i++)
		{
			if(HasEffectiveBuilding(rec->GetExcludedByBuildingIndex(i)))
			{
				return false;
			}
		}
	}

	// Added by E - checks if a city has a wonder required to build the unit
	if(rec->GetNumPrerequisiteWonder() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumPrerequisiteWonder(); i++)
		{
			if(!HasCityWonder(rec->GetPrerequisiteWonderIndex(i)))
				return false;
		}
	}

	// EMOD this wonder is prevented by other wonders
	// to be built. (good for state religion etc)
	if(rec->GetNumExcludedByWonder() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumExcludedByWonder(); i++)
		{
			if(HasCityWonder(rec->GetExcludedByWonderIndex(i)))
			{
				return false;
			}
		}
	}

	// Added by E - Compares Unit CityStyle to the CityStyle of the City
	if(rec->GetNumCityStyleOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCityStyleOnly(); i++)
		{
			if(rec->GetCityStyleOnlyIndex(i) == m_cityStyle)
			{
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

	// Start Resources section - more to add later
	// Added by E - Compares Unit NeedsCityGood to the resources collected
	// our bought by the city, can be either/or
	if(rec->GetNumNeedsCityGood() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsCityGood(); i++)
		{
			if(HasNeededGood(rec->GetNeedsCityGoodIndex(i)))
			{
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

	// Added by E - Compares Unit NeedsCityGoodAll to the resources collected
	// our bought by the city, must be all listed
	if(rec->GetNumNeedsCityGoodAll() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumNeedsCityGoodAll(); i++)
		{
			if(!HasNeededGood(rec->GetNeedsCityGoodAllIndex(i)))
				return false;
		}
	}

	//End Resources Code
	// Added by E - Compares NeedsFeatToBuild to the FeatTracker	6-27-2006
	if(rec->GetNumNeedsFeatToBuild() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsFeatToBuild(); i++)
		{
			if(g_featTracker->HasFeat(rec->GetNeedsFeatToBuildIndex(i)))
			{
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

	if(!g_slicEngine->CallMod(mod_CanCityBuildUnit, true, m_home_city.m_id, rec->GetIndex()))
		return false;

	if(!rec->GetMovementTypeLand() && !rec->GetMovementTypeTrade() && !rec->GetIsTrader())
	{
		if(g_theWorld->IsWater(pos.x, pos.y) && rec->GetSeaCityCanBuild())
		{
			return true;
		}

		if((g_theWorld->IsLand(pos.x, pos.y) ||
			g_theWorld->IsMountain(pos.x, pos.y)) &&
			rec->GetLandCityCanBuild()) {

			return true;
		}

		if(rec->GetMovementTypeSea() || rec->GetMovementTypeShallowWater())
		{
			if(g_theWorld->IsNextToWater(pos.x, pos.y))
			{
				return true;
			}

			return false;
		}
		else if(rec->GetMovementTypeAir())
		{
			return true;
		}
		return false;
	}

	return true;
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
// Remark(s)  : - CityStyleOnly added by E. Limits certain buildings to be built
//                only at certain cities of certain styles.
//              - GovernmentType flag for Buidings limits Buildings to govt type.
//              - CultureOnly flag added by E. It allows only civilizations with
//                the same CityStyle as CultureOnly's style to build that building.
//              - Added NeedsCityGood checks building good and sees if the city
//                has the either good in its radius or is buying the good from trade.
//              - Added NeedsCityGoodAll checks building good and sees if the city
//                has all of the goods in its radius or is buying the good from trade.
//              - Added OnePerCiv checks if any civ already built building
//              - Added Buildingfeat checks if any civ has built num or percent of buildings
//
//----------------------------------------------------------------------------
bool CityData::CanBuildBuilding(sint32 type) const
{
	if(HasBuilding(type)) // This does not excludes buildings given by wonders, maybe change this
	{
		return false;
	}

	if(g_exclusions->IsBuildingExcluded(type))
		return false;

	const BuildingRecord* rec = buildingutil_Get(type, m_owner);

	Assert(rec != NULL);
	if (!rec)
		return false;

	if(!g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex())
	&&  rec->GetEnableAdvanceIndex() >= 0
	){
		return false;
	}

	{ // ToDo: Check whether something is missing here.
		for(sint32 i = 0; i < rec->GetNumObsoleteAdvance(); i++)
		{
			if(g_player[m_owner]->HasAdvance(rec->GetObsoleteAdvanceIndex(i)))
				return false;
		}
	}

	MapPoint pos;
	m_home_city.GetPos(pos);
	if(g_theWorld->IsWater(pos))
	{
		if(rec->GetCantBuildInSea())
			return false;
	}
	else
	{
		if(rec->GetCantBuildOnLand())
			return false;
	}

	if (rec->GetCoastalBuilding())
	{
		if(!g_theWorld->IsNextToWater(pos.x, pos.y))
			return false;
	}

	if(rec->GetNuclearPlant()
	&& wonderutil_GetNukesEliminated(g_theWonderTracker->GetBuiltWonders())
	){
		return false;
	}

	if(rec->GetNumPrerequisiteBuilding() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumPrerequisiteBuilding(); i++)
		{
			if(!HasEffectiveBuilding(rec->GetPrerequisiteBuildingIndex(i)))
				return false;
		}
	}

	// EMOD reverse of prereq - this building prevents other buildings
	// to be built. (good for state religion etc)
	if(rec->GetNumExcludedByBuilding() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumExcludedByBuilding(); i++)
		{
			if(HasEffectiveBuilding(rec->GetExcludedByBuildingIndex(i)))
			{
				return false;
			}
		}
	}

	// Added by E - checks if a city has a wonder required to build the building
	if(rec->GetNumPrerequisiteWonder() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumPrerequisiteWonder(); i++)
		{
			if(!HasCityWonder(rec->GetPrerequisiteWonderIndex(i))) // Only city specific, but not empire specific
				return false;
		}
	}

	// Added by Maq - fix so science victory buildings need gaia controller race to be started first,
	//                in vanilla this is triggered by anyone building the "Solaris Project" wonder
	if(rec->GetIndex() == GaiaController::GetMainframeBuildingIndex()
	&& !wonderutil_GetStartGaiaController(g_theWonderTracker->GetBuiltWonders())
	){
		return false;
	}

	if(rec->GetIndex() == GaiaController::GetSatelliteBuildingIndex()
	&& !wonderutil_GetStartGaiaController(g_theWonderTracker->GetBuiltWonders())
	){
		return false;
	}

	// EMOD this wonder is prevented by other wonders
	// to be built. (good for state religion etc)
	if(rec->GetNumExcludedByWonder() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumExcludedByWonder(); i++)
		{
			if(HasCityWonder(rec->GetExcludedByWonderIndex(i))) // Only city specific, but not empire specific
			{
				return false;
			}
		}
	}

	// EMOD OnePerCiv allows for buildings to be Small Wonders
	if (rec->GetOnePerCiv())
	{
		for (sint32 i = 0; i < g_player[m_owner]->m_all_cities->Num(); ++i)
		{
			Unit aCity = g_player[m_owner]->m_all_cities->Access(i);
			if(aCity.CD()->HasBuilding(type))
			{
				return false;
			}
		}
	}

	// EMOD from feats but needs a number of builds to build, goes
	// with small wonders 2-24-2006
	const BuildingRecord::BuildingFeat *bf;

	if(rec->GetBuildingFeat(bf))
	{
		if(bf->GetBuildingIndex())
		{
			sint32 numCities = 0;
			for(sint32 i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++)
			{
				Unit aCity = g_player[m_owner]->m_all_cities->Access(i);
				if(aCity.CD()->HasBuilding(bf->GetBuildingIndex()))
					numCities++;
			}

			sint32 num;
			sint32 percent;
			bool enoughNum     = false;
			bool enoughPercent = false;

			if(bf->GetNum(num))
			{
				if(numCities >= num)
				{
					enoughNum = true;
				}
			}

			if(bf->GetPercentCities(percent))
			{
				sint32 havePercent = (numCities * 100) / g_player[m_owner]->m_all_cities->Num();
				if(havePercent >= percent)
				{
					enoughPercent = true;
				}
			}

			if(!enoughNum && !enoughPercent)
				return false;
		}
	}

	// Added GovernmentType flag from Units to use for Buildings
	if(rec->GetNumGovernmentType() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumGovernmentType(); i++)
		{
			if(rec->GetGovernmentTypeIndex(i) == g_player[m_owner]->GetGovernmentType())
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares Building CityStyle to the CityStyle of the City
	if(rec->GetNumCityStyleOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCityStyleOnly(); i++)
		{
			if(rec->GetCityStyleOnlyIndex(i) == m_cityStyle)
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares Building CultureOnly to the Player's CityStyle
	if(rec->GetNumCultureOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCultureOnly(); i++)
		{
			if(rec->GetCultureOnlyIndex(i) == g_player[m_owner]->GetCivilisation()->GetCityStyle())
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares CivilisationOnly to the Player's Civilisation	5-11-2006
	if(rec->GetNumCivilisationOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCivilisationOnly(); i++)
		{
			if(rec->GetCivilisationOnlyIndex(i) == g_player[m_owner]->m_civilisation->GetCivilisation())
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Start Resources section - more to add later
	// Added by E - Compares Building NeedsCityGood to the resources collected our bought by the city, can be either/or
	if(rec->GetNumNeedsCityGood() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsCityGood(); i++)
		{
			if(HasNeededGood(rec->GetNeedsCityGoodIndex(i)))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares Building NeedsCityGoodAll to the resources collected our bought by the city, must be all listed
	if(rec->GetNumNeedsCityGoodAll() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumNeedsCityGoodAll(); i++)
		{
			if(!HasNeededGood(rec->GetNeedsCityGoodAllIndex(i)))
				return false;
		}
	}

	if(rec->GetNumNeedsCityGoodAnyCity())
	{
		bool goodavail = false;

		for(sint32 i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++)
		{
			for(sint32 g = 0; g < rec->GetNumNeedsCityGoodAnyCity(); g++)
			{
				if(g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->HasNeededGood(rec->GetNeedsCityGoodAnyCityIndex(g)))
				{
					goodavail = true;
					break;
				}
			}

			if(goodavail)
			{
				break;
			}
		}

		if(!goodavail)
			return false;
	}

	//End Resources Code

	// Added by E - Compares Unit NeedsFeatToBuild to the FeatTracker	5-11-2006
	if(rec->GetNumNeedsFeatToBuild() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsFeatToBuild(); i++)
		{
			if(g_featTracker->PlayerHasFeat(rec->GetNeedsFeatToBuildIndex(i), m_owner))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	//Any player must have feat to build
	if(rec->GetNumNeedsAnyPlayerFeatToBuild() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsAnyPlayerFeatToBuild(); i++)
		{
			if(g_featTracker->HasFeat(rec->GetNeedsAnyPlayerFeatToBuildIndex(i)))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// added by E - some buildings can only be built once city reaches certain size
	sint32 pop;
	if(rec->GetNeedsPopCountToBuild(pop))
	{
		if(PopCount() <= pop)
		{
			return false;
		}
	}

	if(rec->GetPopCountBuildLimit(pop))
	{
		if(PopCount() >= pop)
		{
			return false;
		}
	}

	//emod to let modders limit the number of buildings in a city
	//if (GetNumCityBuildings() >= g_theConstDB->GetMaxCityBuildings()) {
	//	return false;
	//}

	///END CONDITIONS
	return g_slicEngine->CallMod(mod_CanCityBuildBuilding, TRUE, m_home_city.m_id, rec->GetIndex()) != FALSE;
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
// Remark(s)  : - CityStyleOnly added by E. Limits certain wonders to be built
//                only at certain cities of certain styles.
//              - GovernmentType flag for wonders limits wonders to govt type.
//              - CultureOnly flag added by E. It allows only civilizations with
//                the same CityStyle as CultureOnly's style to build that wonder.
//              - PrerequisiteBuilding checks if a city has a building in order
//                to build a wonder. Added by E.
//              - Added NeedsCityGood checks wonder good and sees if the city
//                has the either good in its radius or is buying the good from trade.
//              - Added NeedsCityGoodAll checks wonder good and sees if the city
//                has all of the goods in its radius or is buying the good from trade.
//              - Added Buildingfeat checks if any civ has built num or percent of buildings
//
//----------------------------------------------------------------------------
bool CityData::CanBuildWonder(sint32 type) const
{
	if(!wonderutil_IsAvailable(type, m_owner))
		return false;

	if(g_exclusions->IsWonderExcluded(type))
		return false;

	// Added Wonder database
	const WonderRecord* rec = wonderutil_Get(type, m_owner);

	// Added PrerequisiteBuilding checks if city has building to build wonder
	if(rec->GetNumPrerequisiteBuilding() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumPrerequisiteBuilding(); i++)
		{
			if(!HasEffectiveBuilding(rec->GetPrerequisiteBuildingIndex(i)))
				return false;
		}
	}

	// EMOD reverse of prereq - this wonder prevented by buildings
	// from being be built. (good for state religion etc)
	if(rec->GetNumExcludedByBuilding() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumExcludedByBuilding(); i++)
		{
			if(HasEffectiveBuilding(rec->GetExcludedByBuildingIndex(i)))
			{
				return false;
			}
		}
	}

	// EMOD this wonder is prevented by other wonders
	// to be built. (good for state religion etc)
	if(rec->GetNumExcludedByWonder() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumExcludedByWonder(); i++)
		{
			if(HasCityWonder(rec->GetExcludedByWonderIndex(i)))
			{
				return false;
			}
		}
	}

	// Added GovernmentType flag from Units to use for Wonders
	if(rec->GetNumGovernmentType() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumGovernmentType(); i++)
		{
			if(rec->GetGovernmentTypeIndex(i) == g_player[m_owner]->GetGovernmentType())
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// EMOD - Added Coastal Buildings to wonders
	if (rec->GetCoastalBuilding())
	{
		MapPoint pos;
		m_home_city.GetPos(pos);

		if(!g_theWorld->IsNextToWater(pos.x, pos.y))
			return false;
	}

	// EMOD from feats but needs a number of builds to build, goes with wonders 2-24-2006
	const WonderRecord::BuildingFeat *bf;

	if(rec->GetBuildingFeat(bf))
	{
		if(bf->GetBuildingIndex())
		{
			sint32 numCities = 0;
			for(sint32 i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++)
			{
				Unit aCity = g_player[m_owner]->m_all_cities->Access(i);
				if(aCity.CD()->HasBuilding(bf->GetBuildingIndex()))
					numCities++;
			}

			sint32 num, percent;

			if(bf->GetNum(num))
			{
				if(numCities >= num)
				{
					return true;
				}
				return false;
			}
			else if(bf->GetPercentCities(percent))
			{
				sint32 havePercent = (numCities * 100) / g_player[m_owner]->m_all_cities->Num();
				if(havePercent >= percent)
				{
					return true;
				}

				return false;
			}
		}
	}

	// Added by E - Compares Wonder CityStyle to the CityStyle of the City
	if(rec->GetNumCityStyleOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCityStyleOnly(); i++)
		{
			if(rec->GetCityStyleOnlyIndex(i) == m_cityStyle)
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares Wonder CultureOnly to the Player's CityStyle
	if(rec->GetNumCultureOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCultureOnly(); i++)
		{
			if(rec->GetCultureOnlyIndex(i) == g_player[m_owner]->GetCivilisation()->GetCityStyle())
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares CivilisationOnly to the Player's Civilisation	5-11-2006
	if(rec->GetNumCivilisationOnly() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumCivilisationOnly(); i++)
		{
			if(rec->GetCivilisationOnlyIndex(i) == g_player[m_owner]->m_civilisation->GetCivilisation())
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	if(rec->GetNumNeedsCityGoodAnyCity())
	{
		bool goodavail = false;
		for(sint32 i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++)
		{
			for(sint32 g = 0; g < rec->GetNumNeedsCityGoodAnyCity(); g++)
			{
				if(g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->HasNeededGood(rec->GetNeedsCityGoodAnyCityIndex(g)))
				{
					goodavail = true;
					break;
				}
			}

			if(goodavail)
			{
				break;
			}
		}

		if(!goodavail)
			return false;
	}

	// Start Resources section - more to add later
	// Added by E - Compares Unit NeedsCityGood to the resources collected or bought by the city, can be either/or
	if(rec->GetNumNeedsCityGood() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsCityGood(); i++)
		{
			if(HasNeededGood(rec->GetNeedsCityGoodIndex(i)))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares Wonder NeedsCityGoodAll to the resources collected or bought by the city, must be all listed
	if(rec->GetNumNeedsCityGoodAll() > 0)
	{
		for(sint32 i = 0; i < rec->GetNumNeedsCityGoodAll(); i++)
		{
			if(!HasNeededGood(rec->GetNeedsCityGoodAllIndex(i)))
				return false;
		}
	}
	//End Resources Code

	// Added by E - Compares Unit NeedsFeatToBuild to the FeatTracker	5-11-2006
	// changed so player has to have achieved feat
	if(rec->GetNumNeedsFeatToBuild() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsFeatToBuild(); i++)
		{
			if(g_featTracker->PlayerHasFeat(rec->GetNeedsFeatToBuildIndex(i), m_owner))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Any player must have feat to build 5-18-2007
	if(rec->GetNumNeedsAnyPlayerFeatToBuild() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumNeedsAnyPlayerFeatToBuild(); i++)
		{
			if(g_featTracker->HasFeat(rec->GetNeedsAnyPlayerFeatToBuildIndex(i)))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	////////////////////////////////////////////////////////////
	// EMOD TODO: count up the number of wonders in a city and
	// compare it to the difficultydb wondercitylimit and add
	// to a buildingflag additional wonder to city and a govt
	// modifier?
	////////////////////////////////////////////////////////////
	if(GetNumCityWonders() >= g_theConstDB->Get(0)->GetMaxCityWonders())
	{
		return false;
	}

	return g_slicEngine->CallMod(mod_CanCityBuildWonder, TRUE, m_home_city.m_id, type) != FALSE;
}

void CityData::Injoin(sint32 player)
{
	m_isInjoined = true;
	m_injoinedBy = player;
}

bool CityData::IsInjoined() const
{
	return m_isInjoined;
}

// Why aren't those inlined?
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
bool CityData::IsCelebratingHappiness(void) const { return (m_happy->IsVeryHappy()); }

void CityData::HappinessAttackedBy(sint32 player)
{
	Assert(player < k_MAX_PLAYERS);
	m_happinessAttackedBy = player;
}

bool CityData::WasHappinessAttacked(void) const
{
	if(m_happinessAttackedBy >= 0)
		return true;

	double val;
	StringId name;
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
	for(i = 0; i < m_tradeSourceList.Num(); i++)
	{
		if(m_tradeSourceList.Get(i).GetDestination().GetOwner() == player)
			num++;
	}

	for(i = 0; i < m_tradeDestinationList.Num(); i++)
	{
		if(m_tradeDestinationList.Get(i).GetOwner() == player)
			num++;
	}

	return num;
}

double CityData::IsProtectedFromSlavery() const
{
	return buildingutil_GetPreventSlavery(GetEffectiveBuildings(), m_owner);
}

void CityData::NotifyAdvance(AdvanceType advance)
{
	for (sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if (m_built_improvements & ((uint64)1 << i))
		{
			BuildingRecord const * irec =
			    buildingutil_Get(i, m_owner);

			for (sint32 o = 0; o < irec->GetNumObsoleteAdvance(); o++)
			{
				if (irec->GetObsoleteAdvanceIndex(o) == advance)
				{
					SellBuilding(i, false);
					break;
				}
			}
		}
	}

	m_build_queue.RemoveIllegalItems(true);
	FindBestSpecialists();
}

//not used; see Player::BeginTurnScience()
void CityData::ContributeScience(double incomePercent,
                                 double &addscience,
                                 double &subgold)
{
	Assert(false);

	double scienceRate;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(scienceRate);

	subgold = m_net_gold * incomePercent * scienceRate;

	double s;
	buildingutil_GetSciencePercent(GetEffectiveBuildings(), s, m_owner);

	addscience = subgold + (subgold * s);

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

	return static_cast<sint32>
	    (ceil(static_cast<double>(k_PEOPLE_PER_POPULATION - m_partialPopulation)
	            / static_cast<double>(m_growth_rate)
	         )
	    );
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

// Worth to be implemented
bool CityData::SendSlaveTo(Unit &dest)
{
	return false;
}

void CityData::SetFullHappinessTurns(sint32 turns)
{
	m_happy->SetFullHappinessTurns(turns);
}

sint32 CityData::GetHappinessFromPops() const
{
	return (m_specialistDBIndex[POP_ENTERTAINER] < 0)
	       ? 0
	       : EntertainerCount() * (g_thePopDB->Get
	                                (m_specialistDBIndex[POP_ENTERTAINER],
	                                 g_player[m_owner]->GetGovernmentType()
	                                )->GetHappiness() + wonderutil_GetIncreaseSpecialists(g_player[m_owner]->m_builtWonders));
}

sint32 CityData::GetNumPop() const
{
	return std::max<sint32>(PopCount(), 1);
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
	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
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
	                            true))
	{
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

	for(sint32 i = 0; i < k_MAX_BUILDINGS; i++)
	{
		if(HasBuilding(i))
		{
			if(!buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)i, m_owner))
				buildings[count++] = i;
		}
	}

	if(count > 0)
	{
		sint32 which = g_rand->Next(count);

		m_built_improvements &= ~((uint64)1 << uint64(buildings[which]));
//		g_player[m_owner]->RegisterLostBuilding(m_home_city, buildings[which]); //  Maybe worth of reimplementation
		m_build_queue.RemoveIllegalItems(true);
	}

	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);
}

void CityData::AddConversionUnhappiness(sint32 who)
{
	if(m_convertedTo == who)
	{
		m_happy->RemoveTimerReason(HAPPY_REASON_ATTACKED_CONVERTER);

		m_happy->AddTimer(g_theConstDB->Get(0)->GetAttackConverterUnhappinessTurns(),
		                  g_theConstDB->Get(0)->GetAttackConverterUnhappinessAmount(),
		                  HAPPY_REASON_ATTACKED_CONVERTER);
	}
}

bool CityData::CanBuildInfrastructure() const
{
	return g_player[m_owner]->CanBuildInfrastructure();
}

void CityData::InsertInfrastructure()
{
	Assert(CanBuildInfrastructure());
	if (CanBuildInfrastructure())
	{
		m_build_queue.InsertTail(k_GAME_OBJ_TYPE_INFRASTRUCTURE, 0, 0);
	}
}

void CityData::InsertCapitalization()
{
	Assert(CanBuildCapitalization());
	if (CanBuildCapitalization())
	{
		m_build_queue.InsertTail(k_GAME_OBJ_TYPE_CAPITALIZATION, 0, 0);
	}
}

void CityData::BuildInfrastructure()
{
	Assert(CanBuildInfrastructure());
	if(!CanBuildInfrastructure())
		return;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_INFRASTRUCTURE, m_home_city.m_id));
	}
	else if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_INFRASTRUCTURE, m_home_city));
		g_network.Unblock(m_owner);
	}
}

bool CityData::CanBuildCapitalization() const
{
	return g_player[m_owner]->CanBuildCapitalization();
}

void CityData::BuildCapitalization()
{
	Assert(CanBuildCapitalization());
	if(!CanBuildCapitalization())
		return;

	if(g_network.IsClient() && g_network.IsLocalPlayer(m_owner))
	{
		g_network.SendAction(new NetAction(NET_ACTION_BUILD_CAPITALIZATION, m_home_city.m_id));
	}
	else if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_BUILD_CAPITALIZATION, m_home_city));
		g_network.Unblock(m_owner);
	}
}

void CityData::EliminateNukes()
{
	if(buildingutil_IsNuclearPlant(m_built_improvements, m_owner))
	{
		for (sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
		{
			if(buildingutil_IsNuclearPlant((uint64)1 << i, m_owner))
			{
				DestroyImprovement(i);
			}
		}
	}

	m_build_queue.RemoveIllegalItems();
}

bool CityData::BreakOneSourceRoute(ROUTE_TYPE type, sint32 resource)
{
	for (sint32 i = 0; i < m_tradeSourceList.Num(); i++)
	{
		ROUTE_TYPE t;
		sint32 r;
		m_tradeSourceList[i].GetSourceResource(t, r);

		if(t == type && r == resource)
		{
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
	for(i = m_tradeSourceList.Num() - 1; i >= 0; i--)
	{
		m_tradeSourceList.Access(i).Kill(CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED);
	}

	for(i = m_tradeDestinationList.Num() - 1; i >= 0; i--)
	{
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
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());

	for (sint32 i = 0; i < cell->GetNumUnits(); i++)
	{
		if(cell->UnitArmy()->Access(i).GetAttack() > 0)
		{
			numMilitaryUnits++;
		}
	}

	if (double(numMilitaryUnits) >=
	        (double(numSlaves) / double(g_theConstDB->Get(0)->GetSlavesPerMilitaryUnit()))
	   )
		return;

	sint32 over = numSlaves - (numMilitaryUnits * g_theConstDB->Get(0)->GetSlavesPerMilitaryUnit());

	sint32 chance = over * g_theConstDB->Get(0)->GetUprisingChancePerUnguardedSlave();

	if(g_rand->Next(100) < chance)
	{
		m_doUprising = UPRISING_CAUSE_UNGUARDED_SLAVES;
	}
}

bool CityData::NeedToDoUprising() const
{
	return m_doUprising != UPRISING_CAUSE_NONE;
}

void CityData::Disband()
{
	if(PopCount() > g_theConstDB->Get(0)->GetMaxDisbandSize())
	{
		return;
	}

	if(g_player[m_owner]->m_all_cities->Num() < 2)
		return;

	if(g_network.IsClient())
	{
		g_network.SendAction(new NetAction(NET_ACTION_DISBAND_CITY,
										   (uint32)m_home_city));
	}

	MapPoint pos;
	m_home_city.GetPos(pos);
	sint32 settler = -1;

	for(sint32 i = 0; i < g_theUnitDB->NumRecords(); i++)
	{
		if(g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetSettleLand() &&
		   (g_theWorld->IsLand(pos) || g_theWorld->IsMountain(pos)) &&
		   !g_theWorld->IsWater(pos) && !g_theWorld->IsShallowWater(pos))
		{
			settler = i;
			break;
		}
		else if(g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetSettleWater() && g_theWorld->IsWater(pos))
		{
			settler = i;
			break;
		}
		else if(g_theUnitDB->Get(i, g_player[m_owner]->GetGovernmentType())->GetSettleSpace() && g_theWorld->IsSpace(pos))
		{
			settler = i;
			break;
		}
	}

	Assert(settler >= 0);
	if(settler >= 0)
	{
		Unit s = g_player[m_owner]->CreateUnit(settler, pos,
		                                       m_home_city, false,
		                                       CAUSE_NEW_ARMY_DISBANDED_CITY);
		if(g_theUnitPool->IsValid(s))
		{
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
	return (m_numSpecialists[POP_SLAVE] > 0) ? m_slaveBits : 0;
}

void CityData::AddGoods(SlicObject *obj)
{
	m_tempGoodAdder = obj;
	m_cityRadiusOp = RADIUS_OP_ADD_GOODS;
	CityRadiusIterator(m_home_city.RetPos(), this);
}

sint32 CityData::GetGoodCountInRadius(sint32 good)
{
	m_cityRadiusOp = RADIUS_OP_COUNT_GOODS;
	m_tempGood = good;
	m_tempGoodCount = 0;
	CityRadiusIterator(m_home_city.RetPos(), this);
	return m_tempGoodCount;
}

sint32 CityData::SubtractAccumulatedFood(sint32 amount)
{
	m_accumulated_food = std::max<sint32>(m_accumulated_food - amount, 0);
	return m_accumulated_food;
}

bool CityData::HasSleepingUnits(void) const
{
	CellUnitList * units = g_theWorld->GetArmyPtr(m_home_city.RetPos());

	if (units)
	{
		for (sint32 i = 0; i <units->Num(); ++i)
		{
			Unit    u = units->Access(i);
			if (u.IsValid() && (u.IsAsleep() || u.IsEntrenched()))
			{
				return true;
			}
		}
	}

	return false;
}

sint32 CityData::CityGrowthCoefficient()
{
	double value = 0.0;
	m_home_city.GetDBRec()->GetCityGrowthCoefficient(value);

	return static_cast<sint32>(g_theConstDB->Get(0)->GetCityGrowthCoefficient() * value);
}

void CityData::DestroyWonder(sint32 which)
{
	m_builtWonders &= ~((uint64)1 << (uint64)which);
	g_player[m_owner]->RemoveWonder(which, true);

	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_WonderRemoved,
	                       GEA_City, m_home_city.m_id,
	                       GEA_Int, which,
	                       GEA_End);
	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);
}

void CityData::BuildFront()
{
	m_shieldstore_at_begin_turn = m_shieldstore;
	if (m_build_queue.BuildFront
	        (m_shieldstore,
	         this,
	         m_home_city.RetPos(),
	         m_built_improvements,
	         m_builtWonders,
	         false
	        )
	   )
	{
		m_shieldstore_at_begin_turn = m_shieldstore;
	}

	if (m_build_queue.GetHead())
	{
		m_build_category_at_begin_turn = m_build_queue.GetHead()->m_category;
	}
	// If no item in build queue at begin turn, then use last item that was built
	// as the type, NOT the empty type -3.
	// This is so a switch penalty will not apply between items of the same type
	// if the queue was empty between completing one item and adding a new one.
//	else
//	{
	//	m_build_category_at_begin_turn = -3;
	//}
}

sint32 CityData::GetValue() const
{
	sint32 value = 0;

	sint32 i;
	uint64 wonders = m_builtWonders&(((uint64)1<<(uint64)g_theWonderDB->NumRecords())-1);
	for(i=0;wonders!=0; i++,wonders>>=1)
	{
		if ((wonders&0xFF) == 0)
		{
			wonders>>=8;
			i+=8;
		}

		if ((wonders&1) && !wonderutil_IsObsolete(i))
		{
			value += wonderutil_GetProductionCost(i);
		}
	}

	uint64 buildings = GetEffectiveBuildings()&(((uint64)1<<(uint64)g_theBuildingDB->NumRecords())-1);
	for(i=0;buildings!=0; i++,buildings>>=1)
	{
		if ((buildings&0xFF) == 0)
		{
			buildings>>=8;
			i+=8;
		}

		if (buildings&1)
		{
			value += buildingutil_GetProductionCost(i, m_owner);
		}
	}

	value += m_net_production;
	value += m_net_gold;
	value += m_goldFromTradeRoutes;

	return value;
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

	if(delta > 0)
	{
		Assert(WorkerCount() >= delta);
		if(WorkerCount() < delta)
			return;
	}
	else if(delta < 0)
	{
		Assert(SpecialistCount(type) + delta >= 0);
		if(SpecialistCount(type) + delta < 0)
			return;
	}

	m_numSpecialists[POP_WORKER] -= (sint16)delta;
	m_numSpecialists[type] += (sint16)delta;

	if(type == POP_SLAVE)
	{
		if(m_numSpecialists[POP_SLAVE] <= 0)
			m_slaveBits = 0;
	}

	AdjustSizeIndices();

	if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(this);
		g_network.Unblock(m_owner);
	}
	else if(g_network.IsClient())
	{
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
	for(sizeIndex = 0; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++)
	{
		const CitySizeRecord *rec = g_theCitySizeDB->Get(sizeIndex);
		if(size_index < 0 && PopCount() <= rec->GetPopulation())
		{
			size_index = sizeIndex;
		}
	}

	if(size_index < 0)
	{
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
	for(sizeIndex = 0; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++)
	{
		const CitySizeRecord *rec = g_theCitySizeDB->Get(sizeIndex);

		if(workers >= rec->GetMaxWorkers())
		{
			full_index = sizeIndex;
			partial_index = sizeIndex + 1;
		}

		if(size_index < 0 && PopCount() <= rec->GetPopulation())
		{
			size_index = sizeIndex;
		}
	}

	if(size_index < 0)
	{
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
// Name       : CityData::AdjustSizeIndices
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

	if(m_home_city->Flag(k_UDF_VISION_ADDED))
	{
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
		g_gevManager->AddEvent(GEV_INSERT_AfterCurrent, GEV_CityInfluenceChanged,
								   GEA_City, m_home_city.m_id,
								   GEA_Int, oldSizeIndex,
								   GEA_End);

		if(oldVision != GetVisionRadius())
		{
			if(oldVision >= 0)
			{
				m_home_city->RemoveOldUnitVision(oldVision);
			}
			m_home_city->AddUnitVision();
		}

		if(m_sizeIndex < oldSizeIndex)
		{
			CityInfluenceIterator it(m_home_city.RetPos(), oldSizeIndex);
			for(it.Start(); !it.End(); it.Next())
			{
				if(it.Pos() != m_home_city.RetPos())
					g_theWorld->GetCell(it.Pos())->SetCityOwner(Unit());
			}
		}

		GenerateCityInfluence(m_home_city.RetPos(), m_sizeIndex);
		GenerateBorders(m_home_city.RetPos(), m_owner, g_theConstDB->Get(0)->GetBorderIntRadius(), g_theConstDB->Get(0)->GetBorderSquaredRadius());
	}

	m_collectingResources.Clear();
	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);
	for(it.Start(); !it.End(); it.Next())
	{
		sint32 good;
		if(g_theWorld->GetGood(it.Pos(), good))
		{
			if(CanCollectGood(good)) //EMOD to prevent Free Collection of a good 5-1-2006
			{
				m_collectingResources.AddResource(good);
			}
		}
	}

	if(oldSizeIndex != m_sizeIndex)
	{
		if(g_network.IsHost())
		{
			g_network.Block(m_owner);
			g_network.Enqueue(m_home_city.AccessData(), this);
			g_network.Unblock(m_owner);
		}
	}
}

void CityData::ChangePopulation(sint32 delta)
{
	m_population                 += delta;
	m_numSpecialists[POP_WORKER] += (sint16) delta;

	for (int i = 0; (i < POP_MAX) && (m_numSpecialists[POP_WORKER] < 0); ++i)
	{
		if (m_numSpecialists[i] > 0)
		{
			sint16 convertCount = std::min<sint16>
			                          (m_numSpecialists[i], -m_numSpecialists[POP_WORKER]);
			m_numSpecialists[i]          -= convertCount;
			m_numSpecialists[POP_WORKER] += convertCount;
		}
	}

	Assert(m_numSpecialists[POP_WORKER] >= 0);

	AdjustSizeIndices();

	// Recalculate resources for turns to next pop
	// and city production figure.
	ProcessAllResources();
	//CalculateGrowthRate();
	//ProcessProduction(true);

	UpdateSprite();

	if(g_network.IsHost())
	{
		g_network.Block(m_owner);
		g_network.Enqueue(m_home_city.AccessData(), this);
		g_network.Unblock(m_owner);
	}

	if(m_population <= 0)
	{
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillCity,
		                       GEA_City, m_home_city.m_id,
		                       GEA_Int, CAUSE_REMOVE_CITY_NO_PEOPLE,
		                       GEA_Player, PLAYER_UNASSIGNED,
		                       GEA_End);
	}
}

void CityData::FindBestSpecialists()
{
	sint32 maxValue[POP_MAX];

	for (sint32 popCategory = (sint32)POP_WORKER + 1;
	     popCategory < (sint32)POP_SLAVE;
	     popCategory++)
	{

		maxValue[popCategory] = -1;
		m_specialistDBIndex[popCategory] = -1;
	}

	for (sint32 dbindex = 0; dbindex < g_thePopDB->NumRecords(); dbindex++)
	{
		const PopRecord *rec = g_thePopDB->Get(dbindex, g_player[m_owner]->GetGovernmentType());

		if(!g_player[m_owner]->HasAdvance(rec->GetEnableAdvanceIndex()))
			continue;

//		if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetSpecialistCap()
//		{
////////////////////////SCIENTISTS
//			sint32 EnabledScientists = buildingutil_GetEnablesScientists(GetEffectiveBuildings());
//			scientists += EnabledScientists;
//			if (ScientistCount() => scientists)
//				continue;

////////////////////////MERCHANTS
//			sint32 EnabledMerchants = buildingutil_GetEnablesMerchants(GetEffectiveBuildings());
//			Merchants += EnabledMerchants;
//			if (MerchantCount() => Merchants)
//				continue;

////////////////////////ENTERTAINERS
//			sint32 EnabledEntertainers = buildingutil_GetEnablesEntertainers(GetEffectiveBuildings());
//			Entertainers += EnabledEntertainers;
//			if (EntertainerCount() => Entertainers)
//				continue;

///////////////////////LABORERS
//			sint32 EnabledLaborer = buildingutil_GetEnablesLaborers(GetEffectiveBuildings());
//			Laborers += EnabledLaborers;
//			if (LaborerCount() => Laborers)
//				continue;

///////////////////////FARMERS
//			sint32 EnabledFarmers = buildingutil_GetEnablesLaborers(GetEffectiveBuildings());
//			Farmers += EnabledFarmers;
//			if (FarmerCount() => Farmers)
//				continue;
///////////////////////
//	}
//////////////////////

		if( rec->GetScience() > 0 &&
			rec->GetScience() > maxValue[POP_SCIENTIST])
		{
			maxValue[POP_SCIENTIST] = rec->GetScience();
			m_specialistDBIndex[POP_SCIENTIST] = dbindex;
		}

		if( rec->GetHappiness() > 0 &&
			rec->GetHappiness() > maxValue[POP_ENTERTAINER])
		{
			maxValue[POP_ENTERTAINER] = rec->GetHappiness();
			m_specialistDBIndex[POP_ENTERTAINER] = dbindex;
		}

		if( rec->GetFood() > 0 &&
			rec->GetFood() > maxValue[POP_FARMER])
		{
			maxValue[POP_FARMER] = rec->GetFood();
			m_specialistDBIndex[POP_FARMER] = dbindex;
		}

		if( rec->GetProduction() > 0 &&
			rec->GetProduction() > maxValue[POP_LABORER])
		{
			maxValue[POP_LABORER] = rec->GetProduction();
			m_specialistDBIndex[POP_LABORER] = dbindex;
		}

		if( rec->GetCommerce() > 0 &&
			rec->GetCommerce() > maxValue[POP_MERCHANT])
		{
			maxValue[POP_MERCHANT] = rec->GetCommerce();
			m_specialistDBIndex[POP_MERCHANT] = dbindex;
		}
	}
}

bool CityData::GetUseGovernor() const
{
	return m_useGovernor || (g_player[m_owner]->IsRobot());
}

void CityData::SetUseGovernor(const bool &value)
{
	m_useGovernor = value;
	if(!IsACopy())
	{
		if(g_network.IsHost())
		{
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
			g_network.Unblock(m_owner);
		}
		else if(g_network.IsClient())
		{
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
	if(!IsACopy())
	{
		if(g_network.IsHost())
		{
			g_network.Block(m_owner);
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
			g_network.Unblock(m_owner);
		}
		else if(g_network.IsClient())
		{
			if(g_network.IsLocalPlayer(m_owner))
				g_network.SendAction(new NetAction(NET_ACTION_SET_MAYOR, m_home_city.m_id, m_buildListSequenceIndex, m_useGovernor));
		}
	}
}

bool CityData::GetGarrisonOtherCities() const
{
	return m_garrisonOtherCities;
}

void CityData::SetGarrisonOtherCities(const bool &value)
{
	m_garrisonOtherCities = value;
}

bool CityData::GetGarrisonComplete() const
{
	return m_garrisonComplete;
}

void CityData::SetGarrisonComplete(const bool &value)
{
	m_garrisonComplete = value;
}

sint8 CityData::GetCurrentGarrison() const
{
	return m_currentGarrison;
}

void CityData::SetCurrentGarrison(const sint8 & value)
{
	Assert(value >= 0);
	Assert(value <= k_MAX_ARMY_SIZE);
	m_currentGarrison = value;
}

sint8 CityData::GetNeededGarrison() const
{
	return m_neededGarrison;
}

void CityData::SetNeededGarrison(const sint8 & value)
{
	Assert(value <= k_MAX_ARMY_SIZE);
	m_neededGarrison = value;
}

double CityData::GetCurrentGarrisonStrength() const
{
	return m_currentGarrisonStrength;
}

void CityData::SetCurrentGarrisonStrength(const double & value)
{
	Assert(value == 0.0 || value >= 0.5);
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
	if(index < 0)
	{
		switch(category)
		{
			case k_GAME_OBJ_TYPE_UNIT:           BuildUnit           (type); return;
			case k_GAME_OBJ_TYPE_IMPROVEMENT:    BuildImprovement    (type); return;
			case k_GAME_OBJ_TYPE_WONDER:         BuildWonder         (type); return;
			case k_GAME_OBJ_TYPE_CAPITALIZATION: InsertCapitalization();     return;
			case k_GAME_OBJ_TYPE_INFRASTRUCTURE: InsertInfrastructure();     return;
			default:
				Assert(false);
				return;
		}
	}

	BuildNode *node = m_build_queue.GetNodeByIndex(index);
	if(!node)
	{
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
	if(PayForBuyFront())
	{
		m_buyFront = true;
	}
}

void CityData::AddImprovement(sint32 type)
{
	MapPoint point(m_home_city.RetPos()); //EMOD add for borders
	const BuildingRecord *rec = buildingutil_Get(type, m_owner); //EMOD add for borders

	SetImprovements(m_built_improvements | ((uint64)1 << (uint64)type));
	IndicateImprovementBuilt();

	if (buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)type, m_owner))
	{
		g_player[m_owner]->SetCapitol(m_home_city);
	}

	sint32 intRad;
	sint32 sqRad;
	//EMOD increases city borders
	if (rec->GetIntBorderRadius(intRad) && rec->GetSquaredBorderRadius(sqRad))
	{
		GenerateBorders(point, m_owner, intRad, sqRad);
	}

	//EMOD creates a unit once built, allows for militia unit
//	sint32 unit;
//	for(unit = 0; unit < rec->GetNumCreatesUnit(); unit++)
//	{
//		if (rec->GetCreatesUnitIndex(unit) > 0)
//	{
//			g_player[m_owner]->CreateUnit(rec->GetCreatesUnitIndex(unit), point, m_home_city, false, CAUSE_NEW_ARMY);
//		}
//	}

	for(sint32 s = 0; s < rec->GetNumShowOnMap(); s++)
	{
		const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(s);

		if(!HasTileImpInRadius(rec->GetShowOnMapIndex(s)))
		{
			MapPoint spot(-1,-1);
			bool found = false;
			CityInfluenceIterator it(point, m_sizeIndex);

			for(it.Start(); !it.End(); it.Next())
			{
				if(point == it.Pos())
					continue;
				if(terrainutil_HasUrban(it.Pos()))
					continue;
				if(terrainutil_HasWonder(it.Pos()))
					continue;

				if(terrainutil_CanPlayerSpecialBuildAt(trec, m_owner, it.Pos()))
				{
					if(found)
					{
						if
						  (
						       g_theWorld->GetCell(spot)->HasTerrainImprovementOrInFuture(rec->GetShowOnMapIndex(s))     // ToDo: check whether the new improvement would remove any of the old ones, if the new one doesn't remove any of the old ones no new location is needed.
						   && !g_theWorld->GetCell(it.Pos())->HasTerrainImprovementOrInFuture(rec->GetShowOnMapIndex(s))
						  )
						{
							spot = it.Pos();
							break;
						}
					}
					else
					{
						spot = it.Pos();
						found = true;

						if(!g_theWorld->GetCell(it.Pos())->HasTerrainImprovementOrInFuture(rec->GetShowOnMapIndex(s)))
						{
							break;
						}
					}
				}
			}

			if(found)
			{
				g_player[m_owner]->CreateSpecialImprovement(rec->GetShowOnMapIndex(s), spot, 0);
			}
		}
	}

	//EMOD Visible Buildings 4-1-2006

	// This fills the city influence radius with the given terrain improvement
	CityInfluenceIterator it(point, m_sizeIndex);
	for(it.Start(); !it.End(); it.Next())
	{
		if(point == it.Pos())
			continue;
		if(terrainutil_HasUrban(it.Pos()))
			continue;
		if(terrainutil_HasWonder(it.Pos()))
			continue;

		//EMOD - FU 4-1-2006 visible tileimps, but it builds them all around the radius and should cost PW
		for(sint32 s = 0; s < rec->GetNumShowOnMapRadius(); s++)
		{
			const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(s);
			if(!terrainutil_CanPlayerSpecialBuildAt(trec, m_owner, it.Pos()))
			{
				g_player[m_owner]->CreateSpecialImprovement(rec->GetShowOnMapRadiusIndex(s), it.Pos(), 0);
			}
		}
	}

	//EMOD for Buildings to increase HP
	sint32 hpBonus = buildingutil_GetIncreaseHP((uint64)1 << type, m_owner);
	if(hpBonus > 0)
	{
		sint32 n = g_player[m_owner]->m_all_units->Num();
		for(sint32 i = 0; i < n; i++)
		{
			g_player[m_owner]->m_all_units->Access(i).AddWonderHPBonus(hpBonus);
		}
	}

	//Add disband city imp here
	//popcount
	//disbandsettlernum
	//if popcount > dsn
	//   set = dsn
	//else
	//   set = popcount
	//for i = 0 i < set; i++)
	// createunit(settler#)

	//EMOD - Add Holy City here?

	buildingutil_GetDefendersBonus(GetEffectiveBuildings(), m_defensiveBonus, m_owner);
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
	sint32 goodsToFind = 0;
	sint32 specialdistance = 0;		//EMOD
	for(sint32 i = 0; i < g_theResourceDB->NumRecords(); i++)
	{
		if(g_theWorld->GetGoodValue(i) <= g_theConstDB->Get(0)->GetMaxGoodValue())
		{
			goodsToFind++;
		}
//EMOD for special goods
		if(g_theResourceDB->Get(i)->GetIsBonusGood())
		{
			specialdistance = g_theResourceDB->Get(i)->GetGold();
		}
//EMOD
		m_distanceToGood[i] = 0 + specialdistance;
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
	                          g_theConstDB->Get(0)->GetBorderIntRadius(),
	                          g_theConstDB->Get(0)->GetBorderSquaredRadius(),
	                          m_home_city);
}

void CityData::ResetStarvationTurns()
{
	m_starvation_turns = g_theConstDB->Get(0)->GetBaseStarvationProtection();
	m_starvation_turns += buildingutil_GetStarvationProtection(GetEffectiveBuildings(), m_owner);
}

sint32 CityData::GetStarvationProtection()
{
	sint32 turns  = g_theConstDB->Get(0)->GetBaseStarvationProtection();
	       turns += buildingutil_GetStarvationProtection(GetEffectiveBuildings(), m_owner);

	return turns;
}

double CityData::GetOffenseBonus(const Unit &defender)
{
	const UnitRecord *defRec = defender.GetDBRec();
	double bonus = 0;
	if(defRec->GetMovementTypeLand() || defRec->GetMovementTypeMountain())
		bonus += buildingutil_GetOffenseBonusLand(GetEffectiveBuildings(), m_owner);

	if(defRec->GetMovementTypeSea() || defRec->GetMovementTypeShallowWater())
		bonus += buildingutil_GetOffenseBonusWater(GetEffectiveBuildings(), m_owner);

	if(defRec->GetMovementTypeAir())
		bonus += buildingutil_GetOffenseBonusAir(GetEffectiveBuildings(), m_owner);

	return bonus;
}

uint64 CityData::GetEffectiveBuildings() const
{
	return m_built_improvements | g_player[m_owner]->GetWonderBuildings();
}

sint32 CityData::GetDesiredSpriteIndex(bool justTryLand)
{
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

	for(sint32 i = 0; i < ageStyleRec->GetNumSprites(); i++)
	{
		spr = ageStyleRec->GetSprites(i);
		if (spr)
		{
			if((isLand && spr->GetType() == 0) ||
			   (!isLand && spr->GetType() != 0))
			{
				lastTypeSpr = spr;
				if(spr->GetMinSize() <= m_population &&
				   spr->GetMaxSize() >= m_population)
				{
					return spr->GetSprite();
				}
			}
		}
	}

	if(!justTryLand && !isLand)
	{
		return GetDesiredSpriteIndex(true);
	}

	if(lastTypeSpr)
	{
		return lastTypeSpr->GetSprite();
	}

	if(spr)
	{
		return spr->GetSprite();
	}

	return 0;
}

void CityData::DoSupport(bool projectedOnly)
{
	Assert(g_player[m_owner]);
	if (g_player[m_owner])
	{
		(void) PayWages(projectedOnly);
		// City maintenance is included in this already:
		(void) SupportBuildings(projectedOnly);
	}
}

sint32 CityData::GetSupport() const
{
	Assert(g_player[m_owner]);
	if (g_player[m_owner] == NULL)
		return 0;

	return CalcWages() + GetSupportBuildingsCost();
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
	SplitScience(projectedOnly, m_net_gold, m_science, m_scie_lost_to_crime);
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
//              scieCrime                Filled with science lost to crime.
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
void CityData::SplitScience(bool projectedOnly, sint32 &gold, sint32 &science, sint32 &scieCrime, bool considerOnlyFromTerrain) const
{
	if (g_player[m_owner]==NULL)
		return;

	sint32 baseGold = gold;
	if(baseGold <= 0)
	{
		baseGold = 0;
	}

	double s;
	g_player[m_owner]->m_tax_rate->GetScienceTaxRate(s);
	science = static_cast<sint32>(ceil(baseGold * s));
	gold -= science;

	science += static_cast<sint32>(m_bonusScie);

	if(!considerOnlyFromTerrain)
	{
		science += ScientistCount() * GetSpecialistsResources(POP_SCIENTIST);
	}

	science  = static_cast<sint32>(science * GetBonusScieCoeff());

	scieCrime = CrimeLoss(science);
	science -= scieCrime;

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
// Parameters : bool projectedOnly     : if false then actually transfer gold
//            : bool changeResources   : if true (default) then redo trade routes, else just calculate gold
//
// Globals    : g_player:            List of players
//
// Returns    : -
//
// Remark(s)  : Use this method if you want to change the gold production.
//
//----------------------------------------------------------------------------
void CityData::CollectOtherTrade(const bool projectedOnly)
{
	ProcessGold(m_net_gold);
	CalcGoldLoss(projectedOnly, m_net_gold, m_convertedGold, m_gold_lost_to_crime);

	if(!projectedOnly)
	{
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
	gold += static_cast<sint32>(m_bonusGold);
	gold += m_gold_from_capitalization;

	//EMOD to assist AI
	if(gold < 0)
	{
		if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetNoAIGoldDeficit()
			&& g_player[m_owner]->IsRobot())
		{
			gold = 0;
		}
	}

	if(!considerOnlyFromTerrain)
	{
		gold += MerchantCount() * GetSpecialistsResources(POP_MERCHANT);
	}

	gold = static_cast<sint32>(gold * GetBonusGoldCoeff());
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
		if(m_convertedBy == CONVERTED_BY_CLERIC)
		{
			convertedGold = static_cast<sint32>(gold * g_theConstDB->Get(0)->GetClericConversionFactor());
		}
		else if(m_convertedBy == CONVERTED_BY_TELEVANGELIST)
		{
			if(buildingutil_GetDoubleTelevangelism(GetEffectiveBuildings(), m_owner))
			{
				convertedGold = static_cast<sint32>(gold * g_theConstDB->Get(0)->GetTelevangelistConversionFactor());
			}
			else
			{
				convertedGold = static_cast<sint32>(gold * g_theConstDB->Get(0)->GetClericConversionFactor());
			}
		}
		else
		{
			Assert(false);
		}

		if(convertedGold < 1)
			convertedGold = 1;

		if (g_player[m_convertedTo])
		{
			sint32 wonderIncrease = wonderutil_GetIncreaseConvertedCitiesFeePercentage(g_player[m_convertedTo]->m_builtWonders);
			if(wonderIncrease > 0)
			{
				convertedGold += (convertedGold * wonderIncrease) / 100;
			}
		}

		gold -= convertedGold;

		if(!projectedOnly)
		{
			DPRINTF(k_DBG_GAMESTATE, ("Sending %d gold to player %d due to conversion\n",
			                          convertedGold, m_convertedTo));
			if (g_player[m_convertedTo])
			{
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
	double foodFraction = 0.0;
	double prodFraction = 0.0;
	double goldFraction = 0.0;
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
bool CityData::NeedMoreFood(sint32 bonusFood, sint32 &foodMissing, bool considerOnlyFromTerrain)
{
	sint32 maxPop = GetMaxPop(g_theCitySizeDB->Get(m_sizeIndex));

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

	sint32 popDistance = strategy.GetStopBuildingFoodBeforePopMax();

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
sint32 CityData::HowMuchMoreFoodNeeded(sint32 bonusFood, bool onlyGrwoth, bool considerOnlyFromTerrain)
{
#if defined(NEW_RESOURCE_PROCESS)
	double maxFoodFromTerrain = ProcessFood(m_max_food_from_terrain + bonusFood);
	ApplyFoodCoeff(maxFoodFromTerrain);
	maxFoodFromTerrain -= CrimeLoss(maxFoodFromTerrain);

	///////////////////////////////////////////////
	// The utilization fractions
	double foodFraction = 0.0;
	double prodFraction = 0.0;
	double goldFraction = 0.0;

	if(considerOnlyFromTerrain)
	{
		ResourceFractions(foodFraction, prodFraction, goldFraction, PopCount());
		maxFoodFromTerrain = ceil(maxFoodFromTerrain * foodFraction);
	}
	else
	{
		ResourceFractions(foodFraction, prodFraction, goldFraction, WorkerCount() + SlaveCount());
		maxFoodFromTerrain = ceil(maxFoodFromTerrain * foodFraction);

		maxFoodFromTerrain += ceil((m_foodFromOnePop - m_crimeFoodLossOfOnePop) * FarmerCount());;
	}

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

	sint32 timePerPop = strategy.GetTurnsAcceptedForOnePop();

	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);
	double food = ceil(static_cast<double>(PopCount()*k_PEOPLE_PER_POPULATION)/static_cast<double>(timePerPop*rec->GetGrowthRate()));

	food -= maxFoodFromTerrain;
	if(onlyGrwoth)
	{
		const CitySizeRecord *nextRec = NULL;
		if(m_sizeIndex >= 0
		&& m_sizeIndex < g_theCitySizeDB->NumRecords())
		{
			nextRec = g_theCitySizeDB->Get(m_sizeIndex);
		}
		else{
			nextRec = g_theCitySizeDB->Get(g_theCitySizeDB->NumRecords()-1);
		}

		double foodForNextRing;
		if(nextRec)
		{
	//		foodForNextRing = GetFoodRequired(nextRec->GetPopulation() + 1 - SlaveCount());
			foodForNextRing = GetFoodRequired(nextRec->GetPopulation() - SlaveCount());
		}
		else
		{
			foodForNextRing = GetFoodRequired(PopCount() - SlaveCount());
		}

		double foodForNextPop = GetFoodRequired(PopCount() + 1 - SlaveCount());

		if(food < foodForNextRing - maxFoodFromTerrain)
		{
			food = foodForNextRing - maxFoodFromTerrain;
		}

		if(food < foodForNextPop - maxFoodFromTerrain)
		{
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
	sint32 specalistFood = GetSpecialistsResources(POP_FARMER);
	if(specalistFood != 0 && considerOnlyFromTerrain)
	{
		double crimeLossFood;
		double currentFoodPerPop = GetProducedFood() / static_cast<double>(WorkerCount());
		double popFood = static_cast<double>(specalistFood*FarmerCount());
		grossFood = popFood;
		popFood   = ProcessFinalFood(crimeLossFood, grossFood);

		currentFood = GetProducedFood() + processedBonusFood - popFood + currentFoodPerPop*FarmerCount();
		foodDelta  -= popFood;
		foodDelta  += currentFoodPerPop*FarmerCount();
	}
	else
	{
		currentFood = GetProducedFood() + processedBonusFood;
	}
	/////////////////////////////////////

	const CitySizeRecord *nextRec = NULL;
	if(m_sizeIndex >= 0
		&& m_sizeIndex < g_theCitySizeDB->NumRecords())
	{
		nextRec = g_theCitySizeDB->Get(m_sizeIndex);
	}
	else
	{
		nextRec = g_theCitySizeDB->Get(g_theCitySizeDB->NumRecords()-1);
	}

	double foodForNextRing;
	if(nextRec)
	{
//		foodForNextRing = GetFoodRequired(nextRec->GetPopulation() + 1 - SlaveCount());
		foodForNextRing = GetFoodRequired(nextRec->GetPopulation() - SlaveCount());
	}
	else
	{
		foodForNextRing = GetFoodRequired(PopCount() - SlaveCount());
	}

	double foodForNextPop = GetFoodRequired(PopCount() + 1 - SlaveCount());

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_owner).GetCurrentStrategy();

	sint32 timePerPop = strategy.GetTurnsAcceptedForOnePop();

	const CitySizeRecord *rec = g_theCitySizeDB->Get(m_sizeIndex);
//	DPRINTF(k_DBG_GAMESTATE, ("GrowthRate: %i\n", rec->GetGrowthRate()));
	double food = ceil(static_cast<double>(PopCount()*k_PEOPLE_PER_POPULATION)/static_cast<double>(timePerPop*rec->GetGrowthRate())) - static_cast<double>(bonusFood);
//	DPRINTF(k_DBG_GAMESTATE, ("Food: %i\n", static_cast<sint32>(food)));
//	DPRINTF(k_DBG_GAMESTATE, ("m_food_delta: %f\n", m_food_delta));
	food -= foodDelta;
//	DPRINTF(k_DBG_GAMESTATE, ("Food: %i\n", static_cast<sint32>(food)));
//	DPRINTF(k_DBG_GAMESTATE, ("foodForNextRing: %i\n", static_cast<sint32>(foodForNextRing)));
//	DPRINTF(k_DBG_GAMESTATE, ("maxFoodFromTerrain: %i\n", static_cast<sint32>(maxFoodFromTerrain)));

	if(food < foodForNextRing - maxFoodFromTerrain)
	{
		food = foodForNextRing - maxFoodFromTerrain;
	}
//	DPRINTF(k_DBG_GAMESTATE, ("FoodNextRing: %i\n", static_cast<sint32>(food)));

//	DPRINTF(k_DBG_GAMESTATE, ("Food: %i\n", static_cast<sint32>(food)));
//	DPRINTF(k_DBG_GAMESTATE, ("foodForNextPop: %i\n", static_cast<sint32>(foodForNextPop)));
//	DPRINTF(k_DBG_GAMESTATE, ("currentFood: %i\n", static_cast<sint32>(currentFood)));

	if(food < foodForNextPop - currentFood)
	{
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
	return std::max<sint32>(0, crime_loss);
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
	return std::max<double>(0.0, crime_loss);
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
		return prod * g_theConstDB->Get(0)->GetBioInfectionProductionCoef();
	}
	else
	{
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
	if(m_franchise_owner >= 0)
	{
		return prod * g_theConstDB->Get(0)->GetFranchiseEffect();
	}
	else
	{
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

	if(m_convertedTo >= 0)
	{
		if(m_convertedBy == CONVERTED_BY_CLERIC)
		{
			convertedGold = gold * g_theConstDB->Get(0)->GetClericConversionFactor();
		}
		else if(m_convertedBy == CONVERTED_BY_TELEVANGELIST)
		{
			if(buildingutil_GetDoubleTelevangelism(GetEffectiveBuildings(), m_owner))
			{
				convertedGold = gold * g_theConstDB->Get(0)->GetTelevangelistConversionFactor();
			}
			else
			{
				convertedGold = gold * g_theConstDB->Get(0)->GetClericConversionFactor();
			}
		}
		else
		{
			Assert(false);
		}

		if(convertedGold < 1)
			convertedGold = 1;

		if (g_player[m_convertedTo])
		{
			sint32 wonderIncrease = wonderutil_GetIncreaseConvertedCitiesFeePercentage(g_player[m_convertedTo]->m_builtWonders);
			if(wonderIncrease > 0)
			{
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

	for(sint32 i = 0; i < ring; ++i)
	{
		workingPeople -= m_ringSizes[i];
	}

	if(workingPeople >= m_ringSizes[i])
	{
		return m_ringSizes[i];
	}
	else if(workingPeople <= 0)
	{
		return 0;
	}
	else
	{
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
	workingPeople++; // Center is free

	///////////////////////////////////////////////
	// Calculete the nominators of the fractions
	for(sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
	{
		if(m_ringSizes[i] == 0)
		{
			break;
		}
		else if(m_ringSizes[i] <= workingPeople)
		{
			foodFraction += static_cast<double>(m_ringFood[i]);
			prodFraction += static_cast<double>(m_ringProd[i]);
			goldFraction += static_cast<double>(m_ringGold[i]);
			workingPeople -= m_ringSizes[i];
		}
		else
		{
			double utilization = static_cast<double>(workingPeople) / static_cast<double>(m_ringSizes[i]);
			foodFraction += static_cast<double>(m_ringFood[i]) * utilization;
			prodFraction += static_cast<double>(m_ringProd[i]) * utilization;
			goldFraction += static_cast<double>(m_ringGold[i]) * utilization;
			break; // Leave loop
		}
	}

	if(workingPeople == 0){
		///////////////////////////////////////////////
		// Calculate the denominators of the fractions
		double foodTotal = 0.0;
		double prodTotal = 0.0;
		double goldTotal = 0.0;
		for(i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
		{
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
	else
	{
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

	for (sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
	{
		sint32 squaredRadius   = g_theCitySizeDB->Get(i)->GetSquaredRadius();
		sint32 squaredDistance = MapPoint::GetSquaredDistance(cityPos, pos);

		if (squaredDistance <= squaredRadius)
		{
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
	if (ring >= 0 && ring < g_theCitySizeDB->NumRecords())
	{
		return m_ringFood[ring];
	}
	else
	{
		sint32 maxFood = 0;
		for (sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
		{
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
	if (ring >= 0 && ring < g_theCitySizeDB->NumRecords())
	{
		return m_ringProd[ring];
	}
	else
	{
		sint32 maxProd = 0;
		for (sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
		{
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
	if (ring >= 0 && ring < g_theCitySizeDB->NumRecords())
	{
		return m_ringGold[ring];
	}
	else
	{
		sint32 maxGold = 0;
		for (sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
		{
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
	if (ring >= 0 && ring < g_theCitySizeDB->NumRecords())
	{
		return m_ringSizes[ring];
	}
	else
	{
		sint32 numTiles = 0;
		for (sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
		{
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
	Assert(ring >= 0 && ring < g_theCitySizeDB->NumRecords());
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
	CityStyleRecord const * style   =
	    g_theCityStyleDB->Get(GetCityStyle(),
	    g_player[m_owner]->GetGovernmentType());

	return (style) ? style->GetHappyInc() : 0;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::GoodHappinessIncr
//
// Description: Returns how much the goods in the city radius increase the
//              city's happiness
//
// Parameters : -
//
// Globals    : g_theResourceDB: The resource database
//
// Returns    : sint32 the increasement of happiness of all goods in the
//              city radius
//
// Remark(s)  : Maybe this should only be the happiness increase of the good
//              with maximum increase.
//
//----------------------------------------------------------------------------
sint32 CityData::GoodHappinessIncr() const
{
	sint32 totalHappinessInc = 0;
	for (sint32 i = 0; i < g_theResourceDB->NumRecords(); ++i)
	{
		if (HasNeededGood(i))
		{
			totalHappinessInc += g_theResourceDB->Get(i)->GetHappyInc();
		}
	}

	return totalHappinessInc;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::CanCollectGood
//
// Description: Returns true if a city can collect the good
//
// Parameters : -
//
// Globals    : g_theResourceDB: The resource database
//
// Returns    : bool if the prerequisites are met to collect a good
//
// Remark(s)  : Additional checks may be added
//
//----------------------------------------------------------------------------
bool CityData::CanCollectGood(sint32 good) const
//EMOD to check Good flags to see if a player can collect it. Modelled on CanBuildBuilding. 4-27-2006
{
	const ResourceRecord *rec = g_theResourceDB->Get(good);
	if(!rec)
		return false;

	if(rec->GetCantTrade())
	{
		return false;
	}

	if(!g_player[m_owner]->HasAdvance(rec->GetAvailableAdvanceIndex())
	&& rec->GetAvailableAdvanceIndex() >= 0) // HasAdvance returns true if the passed parameter is negative
	{
		return false;
	}

	if(g_player[m_owner]->HasAdvance(rec->GetVanishAdvanceIndex())
	&& rec->GetVanishAdvanceIndex() >= 0)
	{
		return false;
	}
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::IsBuildingOperational
//
// Description: Returns true if the buildings prereqs are still available
//
// Parameters : -
//
// Globals    : g_theBuildingDB: The building database
//
// Returns    : bool if the prerequisites are met for the buildings flags to
//              operate
//
// Remark(s)  : Additional checks may be added
//              EMOD to check if a building can operate. Modelled on
//              CanBuildBuilding. 4-27-2006
//
//----------------------------------------------------------------------------
bool CityData::IsBuildingOperational(sint32 type) const
{
	const BuildingRecord* rec = buildingutil_Get(type, m_owner);

	Assert(rec != NULL);
	if (!rec)
		return false;

	sint32 o;
	for(o = 0; o < rec->GetNumObsoleteAdvance(); o++)
	{
		if(g_player[m_owner]->HasAdvance(rec->GetObsoleteAdvanceIndex(o)))
			return false;
	}

	if(rec->GetNuclearPlant() &&
	   wonderutil_GetNukesEliminated(g_theWonderTracker->GetBuiltWonders()))
	{
		return false;
	}

	if(rec->GetNumPrerequisiteBuilding() > 0)
	{
		for(o = 0; o < rec->GetNumPrerequisiteBuilding(); o++)
		{
			if(!HasEffectiveBuilding(rec->GetPrerequisiteBuildingIndex(o)))
				return false;
		}
	}

	// Added GovernmentType flag from Units to use for Buildings
	if(rec->GetNumGovernmentType() > 0)
	{
		bool found = false;
		for(sint32 i = 0; i < rec->GetNumGovernmentType(); i++)
		{
			if(rec->GetGovernmentTypeIndex(i) == g_player[m_owner]->GetGovernmentType())
			{
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

	// Start Resources section - more to add later
	// Added by E - Compares Building NeedsCityGood to the resources
	// collected our bought by the city, can be either/or
	if(rec->GetNumNeedsCityGood() > 0)
	{
		bool found = false;
		for(sint32 g = 0; g < rec->GetNumNeedsCityGood(); g++)
		{
			if(HasNeededGood(rec->GetNeedsCityGoodIndex(g)))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	// Added by E - Compares Building NeedsCityGoodAll to the resources
	// collected our bought by the city, must be all listed
	if(rec->GetNumNeedsCityGoodAll() > 0)
	{
		for(sint32 g = 0; g < rec->GetNumNeedsCityGoodAll(); g++)
		{
			if(!HasNeededGood(rec->GetNeedsCityGoodAllIndex(g)))
				return false;
		}
	}

	if(rec->GetNumNeedsCityGoodAnyCity())
	{
		bool goodavail = false;

		for(sint32 i = 0; i < g_player[m_owner]->m_all_cities->Num(); i++)
		{
			for(sint32 g = 0; g < rec->GetNumNeedsCityGoodAnyCity(); g++)
			{
				if(g_player[m_owner]->m_all_cities->Access(i).AccessData()->GetCityData()->HasNeededGood(rec->GetNeedsCityGoodAnyCityIndex(g))){
					goodavail = true;
					break;
				}
			}

			if(goodavail)
			{
				break;
			}
		}
		if(!goodavail)
			return false;
	}

	//End Resources Code

	return true;
}

sint32 CityData::SectarianHappiness() const
{
	if(
	  (g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetSectarianHappiness())
	||(g_theProfileDB->IsSectarianHappiness())
	) {
		ProcessSectarianHappiness(m_secthappy, m_owner, m_cityStyle);
	}

	return m_secthappy;
}

sint32 CityData::ProcessSectarianHappiness(sint32 newsecthappy, sint32 owner, sint32 citystyle) const
//void CityData::SectarianHappiness() const  //EMOD
// This code creates a happiness modifier based off of the factors of:
// utilizes ProcessSectarianHappiness(m_secthappy);  so doesn't randomize eash turn
{
	//secthappy = 0;
	//sint32 third = (PopCount() / 3) * -1;
	//sint32 newsecthappy = secthappy;

	// Checks if a city has a buildng that conflicts with
	// another (mosques, churches, synagogues, etc)
	// randomized to add unpredictable realism

	for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++)
	{
		if(HasBuilding(b))
		{
			const BuildingRecord *rec = buildingutil_Get(b, owner);

			// Checks if a city has a buildng that conflicts with
			// another (mosques, churches, synagogues, etc)
			for(sint32 i = 0; i < rec->GetNumConflictsWithBuilding(); i++)
			{
				if((rec->GetNumConflictsWithBuilding()) && HasBuilding(rec->GetConflictsWithBuildingIndex(i)))
				{
					newsecthappy -= g_rand->Next(PopCount() / 3);
				}

				// Checks if ANY city has a building that conflicts
				// with another (mosques, churches, synagogues, etc);
				// For State Religion Building
				sint32 c;
				for(c = 0; c < g_player[m_owner]->m_all_cities->Num(); c++)
				{
					Unit aCity = g_player[m_owner]->m_all_cities->Access(c);
					if(aCity.CD()->HasBuilding(rec->GetConflictsWithBuildingIndex(i)))
					{
						newsecthappy -= g_rand->Next(PopCount() / 3);
					}
				}
			}

			//checks if govt conflicts prereqgovt
			for(sint32 g = 0; g < rec->GetNumGovernmentType(); g++)
			{
				if(rec->GetGovernmentTypeIndex(g) != g_player[owner]->GetGovernmentType())
				{
					newsecthappy -= g_rand->Next(PopCount() / 3);
				}
			}

			//checks if cultureonly conflicts
			for(sint32 u = 0; u < rec->GetNumCultureOnly(); u++)
			{
				if(rec->GetCultureOnlyIndex(u) != g_player[owner]->GetCivilisation()->GetCityStyle())
				{
					newsecthappy -= g_rand->Next(PopCount() / 3);
				}
			}
			//end Buildings
		}
	}

		// Checks if the citystle of the city is different than the person that owns it for cultural/ethnic strife
	//if( (g_player[owner] != NULL) && (g_player[owner] > 0) && (g_player[owner]->GetCivilisation() != NULL) && (g_player[owner]->GetCivilisation() > 0)
	//	&& (g_player[owner]->GetCivilisation()->GetCityStyle() > 0) && (owner != PLAYER_UNASSIGNED) && (citystyle > 0) && (citystyle != NULL)
	//){ //this STILL didn't fix it
	//	if(citystyle != g_player[owner]->GetCivilisation()->GetCityStyle())
	//	{ //TODO this line causes the crash
	//		secthappy -= 2; //g_rand->Next(PopCount() / 3);
	//	}
	//}

	// @TODO: This doesn't do anything?
	if ((m_cityStyle >= 0) && (m_cityStyle < g_theCityStyleDB->NumRecords())
		&& (g_player[m_owner] && g_player[m_owner]->GetCivilisation()))
	{
		newsecthappy -= 2;
	}
		 //if ((secthappy < PopCount()) || (secthappy > PopCount()) ) { //added for a cap because it was still randomizing and at values over a million since rev710
		//	secthappy = PopCount();
		//}

		// Checks if the original owner of the city has a different govt than the occupier for political strife
		//if(g_player[m_founder]->GetGovernmentType() == g_player[m_owner]->GetGovernmentType()) {
		//	secthappy -= g_rand->Next(PopCount() / 3);
		//}
	//m_secthappy += newsecthappy;

	return newsecthappy;
//	return SectarianHappiness; //no longer a sint32
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProcessAllResources
//
// Description: Recalculates all the resource stuff from the beginning to
//              the end.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Replaces duplicated code code in:
//              - CauseAndEffectTab::UpdateCities
//              - CityWindow::Project
//              - Governor::SetSliders
//              - Governor::TestSliderSettings
//
//----------------------------------------------------------------------------
void CityData::ProcessAllResources()
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
	t1.start();
#endif
	DPRINTF(k_DBG_GOVERNOR, ("//  City time for nothing         = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	PreResourceCalculation();

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for precalculation  = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	CollectResources();

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for collection      = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

#if defined(NEW_RESOURCE_PROCESS)
	ProcessResources();
	CalculateResources();
	CalcPollution();
	DoSupport(true);
#else
	ProcessProduction(true);

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for production      = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	DoSupport(true);                // Deduct wages and building costs

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for support         = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	SplitScience(true);             // Deduct science costs

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for paying scince   = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	CollectOtherTrade(true);

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for other trade     = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	ProcessFood();

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for food            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	CalcPollution();

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for pollution       = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
#endif
	sint32 gold = 0;
	// Production has an effect on pollution and polltion has an effect
	// on happiness. Of course better would be only one recalculation.
	CalcHappiness(gold, false);

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for happiness       = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	EatFood();

	DPRINTF(k_DBG_GOVERNOR, ("//  City time for eating          = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

	CalculateGrowthRate();

	DPRINTF(k_DBG_GOVERNOR, ("//  City total time for resources = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

#if defined(_DEBUG) || defined(USE_LOGGING)
	t1.stop();
#endif
}

//----------------------------------------------------------------------------
//
// Name       : CityData::TileImpHappinessIncr
//
// Description: Returns how much the tileimps in the city radius increase the
//              city's happiness
//
// Parameters : -
//
// Globals    : g_theResourceDB: The resource database
//
// Returns    : sint32 the increasement of happiness of all tileimps in the
//              city radius
//
// Remark(s)  : Maybe this should only be the happiness increase of the tileimp
//              with maximum increase.
//
//----------------------------------------------------------------------------
sint32 CityData::TileImpHappinessIncr() const
{
	sint32 totalHappinessInc = 0;

	CityInfluenceIterator it(m_home_city.RetPos(), m_sizeIndex);
	for (it.Start(); !it.End(); it.Next())
	{
		Cell *cell = g_theWorld->GetCell(it.Pos());
		for (sint32 t = 0; t < cell->GetNumDBImprovements(); t++)
		{
			sint32 timp = cell->GetDBImprovement(t);
			const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(timp);
			const TerrainImprovementRecord::Effect *effect = terrainutil_GetTerrainEffect(trec, it.Pos());

			if (effect)
			{
				totalHappinessInc += effect->GetHappyInc();
			}
		}
	}

	return totalHappinessInc;
}

//----------------------------------------------------------------------------
//
// Name       : CityData::ProduceEnergy
//
// Description: calculates total city energy
//
// Parameters : -
//
// Globals    : -
//
// Returns    : energy produced
//
// Remark(s)  :
//
//----------------------------------------------------------------------------
sint32 CityData::ProduceEnergy()
{
	sint32 energy = 0;

	//Added by E - EXPORT BONUSES TO GOODS This is only for adding not multiplying
	for (sint32 tgood = 0; tgood < g_theResourceDB->NumRecords(); ++tgood)
	{
		if(HasNeededGood(tgood))
		{
			//sint32 e;
			ResourceRecord const * tgoodData = g_theResourceDB->Get(tgood);
			if (tgoodData)
			{
				energy += tgoodData->GetProducesEnergy();
			}
		}
	}

	//Bit(Int)   EnergyHunger
	//Bit(Int)   EnergyHungerPerPop
	//Bit(Int)   ProducesEnergy
	//Bit(Int)   ProducesEnergyPerPop

	//energy produced per person
	sint32 energyPerCitizen = buildingutil_GetProducesEnergyPerPop(GetEffectiveBuildings(), m_owner);
	energy += energyPerCitizen * PopCount();

	//energy produced per person
	sint32 energyperbldg = buildingutil_GetProducesEnergy(GetEffectiveBuildings(), m_owner);
	energy += energyperbldg;

	//energy produced per person
	sint32 WenergyPerCitizen = wonderutil_GetProducesEnergyPerPop(GetEffectiveBuildings());
	energy += WenergyPerCitizen * PopCount();

	//energy produced per person
	sint32 Wenergyperbldg = wonderutil_GetProducesEnergy(GetEffectiveBuildings());
	energy += Wenergyperbldg;

	return energy;
}

sint32 CityData::ConsumeEnergy()
{
	sint32 energy = 0;
	//Added by E - EXPORT BONUSES TO GOODS This is only for adding not multiplying
	/// @todo Use standart use standard index variable names
	//        i usually denotes a variable used as index, you can use j
	//        if you already have an i. k would be the next in the alphabet.
	//        But don not use l (el) since it looks like 1 (one).
	for (sint32 tgood = 0; tgood < g_theResourceDB->NumRecords(); ++tgood)
	{
		if(HasNeededGood(tgood))
		{
			ResourceRecord const * tgoodData = g_theResourceDB->Get(tgood);
			if (tgoodData)
			{
				energy += tgoodData->GetEnergyHunger();
			}
		}
	}

	// Energy produced per person
	sint32 energyPerCitizen = buildingutil_GetEnergyHungerPerPop(GetEffectiveBuildings(), m_owner);
	energy += energyPerCitizen * PopCount();

	// Energy produced per person
	sint32 energyperbldg = buildingutil_GetEnergyHunger(GetEffectiveBuildings(), m_owner);
	energy += energyperbldg;

	// Energy produced per person
	sint32 WenergyPerCitizen = wonderutil_GetEnergyHungerPerPop(GetEffectiveBuildings());
	energy += WenergyPerCitizen * PopCount();

	// Energy produced per person
	/// @todo Use better identifiers: I don't even know how to pronounce
	//        Wenergyperbldg
	/// @todo Use standart camel casing, local variables start with a
	//        lower case letter, each new word starts with an uper case letter,
	//        no hyphes or underscores
	sint32 Wenergyperbldg = wonderutil_GetEnergyHunger(GetEffectiveBuildings());
	energy += Wenergyperbldg;

	return energy;
}

sint32 CityData::GetNumCityWonders() const
{
	sint32 citywon = 0;
	// - ok I tested this by: setting limit to 2, after giving age of reason a city having no wonders can build any wonders
	// I gave a city one wonder and it can still build; I gave it two and it couldn't build any so I assume it works - now 6-3-2007
	for(sint32 i = 0; i < g_theWonderDB->NumRecords(); i++)
	{
		if(HasCityWonder(i))
		{
			citywon++;
		}
	}

	return citywon;
}

sint32 CityData::GetNumCityBuildings() const
{
	sint32 citybld = 0;
	for(sint32 i = 0; i < g_theBuildingDB->NumRecords(); i++)
	{
		if(HasEffectiveBuilding(i))
		{
			citybld++;
		}
	}

	return citybld;
}

void CityData::InsurgentSpawn()
{
	MapPoint cpos = m_home_city.RetPos();

	//EMOD diffDB so sometimes your city when it riots creates barbs 10-25-2006
	const RiskRecord *risk = g_theRiskDB->Get(g_theGameSettings->GetRisk());

	if(g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetRevoltInsurgents()
	|| g_theProfileDB->IsRevoltInsurgents()
	){
		double barbchance   = risk->GetBarbarianChance();
		double notFounder   = 0.0;
		double notCityStyle = 0.0;

		// If the city has a diffferent culture more likely to have insurgents
		if(m_cityStyle != g_player[m_owner]->GetCivilisation()->GetCityStyle())
		{
			notCityStyle = barbchance * 3.0;
		}
		// If the revolting city is because of an occupation more likely to revolt
		//if(g_player[m_founder]->GetGovernmentType() == g_player[m_owner]->GetGovernmentType()) {
		//	notFounder = barbchance * 2.0;
		//}  //this may cause a crash if founder is already dead

		//TODO: Technology modifier from the founder that increases insurgents
		//TODO: Technology modifier that allows for more suppression
		//TODO: Govt modifier that allows for more suppression
			barbchance += notFounder;
			barbchance += notCityStyle;

			if(g_rand->Next(10000) < static_cast<sint32>(barbchance * 10000.0))
			{
				// Add some Barbarians nearby cpos.
				Barbarians::AddBarbarians(cpos, m_owner, false);
				SlicObject *so = new SlicObject("999InsurgentSpawn");
				so->AddRecipient(m_owner);
				so->AddCity(m_home_city);
				g_slicEngine->Execute(so);  //forgot this?
			}
		}
}

void CityData::RiotCasualties()
{
	MapPoint pos;
	m_home_city.GetPos(pos); // See CityInfluenceIterator below

	//EMOD diffDB so sometimes your city when it riots creates barbs 10-25-2006
	//EMOD to cut population after a revolt (adds realism and minimizes repeat revolts/ feral cities)
	if(
	   (      g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetRevoltCasualties()
	      ||  g_theProfileDB->IsRevoltCasualties()
	   )
	   &&     PopCount() >= 10
	  )
	{
		sint32 casualties = (g_rand->Next(PopCount() / 10));

		if (casualties > 1)
		{
			casualties *= -1 ; //random number of caualties
			ChangePopulation(casualties);
			SlicObject *so = new SlicObject("999RiotCasulaties");
			so->AddRecipient(m_owner);
			so->AddCity(m_home_city);
			so->AddGold(casualties);
			g_slicEngine->Execute(so);
		}
	}

}

void CityData::CityGovernmentModifiers()
{
	//EMOD Fascist governments now kill off alien populations
	// What an idea to kill the whole city at least in the long run.
	if(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetIsXenophobic()) {
		if((g_player[m_owner]->GetCivilisation()->GetCityStyle() != m_cityStyle) && (PopCount() > 1)) {
			ChangePopulation(-1);
		}
	}
	//end EMOD
	// Doing this means to add each turn slaves until the whole city is full of slaves.
	// Welcome to slave uprising.
	if(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetHasGulags())
	{
		if(PopCount() > SlaveCount() * 3)
		{
			ChangeSpecialists(POP_SLAVE, +1);
		}
	}

	if(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetIsCapitalist())
	{
		if(PopCount() > (MerchantCount() * 2))
		{
			ChangeSpecialists(POP_MERCHANT, +1);
		}
	}

	if(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetIsTechnocracy())
	{
		if(PopCount() > (ScientistCount() * 3))
		{
			ChangeSpecialists(POP_SCIENTIST, +1);
		}
	}

	if(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetIsAgrarian())
	{
		if(PopCount() > FarmerCount())
		{
			ChangeSpecialists(POP_FARMER, +1);
		}
	}

	if(g_theGovernmentDB->Get(g_player[m_owner]->m_government_type)->GetHasMindlessTelevision())
	{
		if(PopCount() > EntertainerCount() * 2)
		{
			ChangeSpecialists(POP_ENTERTAINER, +1);
		}
	}

	//EMOD if Player PrereqBuilding is different than the government than destroy it
	for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++)
	{
		if(HasBuilding(b))
		{
			const BuildingRecord *rec = buildingutil_Get(b, m_owner);
			for(sint32 i = 0; i < rec->GetNumGovernmentType(); i++)
			{
				if(rec->GetExcludedByGovernmentTypeIndex(i) != g_player[m_owner]->GetGovernmentType())
				{
					DestroyImprovement(b);
				}
			}
		}
	}
	//end
}

void CityData::Militia()
{
	//EMOD Militia code diffdb and building
	MapPoint cpos = m_home_city.RetPos();

	if(g_theWorld->GetCell(cpos)->GetNumUnits() <= 0)
	{
		sint32 cheapUnit = g_player[m_owner]->GetCheapestMilitaryUnit();

		// If DiffDB AI gets a free unit when city ungarrisoned then give cheapest unit
		if((g_theDifficultyDB->Get(g_theGameSettings->GetDifficulty())->GetAIMilitiaUnit()
		|| 	g_theProfileDB->IsAIMilitiaUnit())
		&& g_player[m_owner]->IsRobot()
		){
			g_player[m_owner]->CreateUnit(cheapUnit, cpos, m_home_city, false, CAUSE_NEW_ARMY_CHEAT);
		}

		// If city has a building that gives it a militia then if
		// empty creates cheapest unit could be human exploit though.
		for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++)
		{
			if(HasBuilding(b)) // Should this include buildings given by wonders?
			{
				const BuildingRecord *rec = buildingutil_Get(b, m_owner);

				if(rec->GetCreatesMiltiaUnit())
				{
					g_player[m_owner]->CreateUnit(cheapUnit, cpos, m_home_city, false, CAUSE_NEW_ARMY_CHEAT);
				}
			}
		}
	}
	//end
}

void CityData::DestroyOnePerCiv()
{
	if(buildingutil_GetDesignatesOnePerCiv(m_built_improvements, m_owner))
	{
		for(uint64 i = 0; i < g_theBuildingDB->NumRecords(); i++) // Can this be simplified?
		{
			if(buildingutil_GetDesignatesCapitol((uint64)1 << (uint64)i, m_owner) &&
			   m_built_improvements & uint64((uint64)1 << i))
			{
				m_built_improvements &= ~((uint64)1 << i);
			}
		}
	}
}

bool CityData::HasReligionIcon() const
{
	if (buildingutil_GetHasReligionIcon(GetEffectiveBuildings(), m_owner))
	{
		return true;
	}

	if (wonderutil_GetHasReligionIcon(GetBuiltWonders()))
	{
		return true;
	}

	return false;
}

bool CityData::IsReligious() const
{
	return buildingutil_GetIsReligious(GetEffectiveBuildings(), m_owner);
}

bool CityData::CityIsOnTradeRoute()
{
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());
	Assert(cell);
	if(cell)
	{
		for(sint32 i = 0; i < cell->GetNumTradeRoutes(); i++)
		{
			if(cell->GetTradeRoute(i).IsActive() && cell->GetTradeRoute(i).IsValid()) // only count active (and valid) routes
			{
				return true;
			}
		}
	}

	return false;
}

void CityData::GiveTradeRouteGold()
{
	//if SilkRoad wonder?
	//if Trade feat?
	Cell *cell = g_theWorld->GetCell(m_home_city.RetPos());
	Assert(cell);
	if(cell)
	{
		for(sint32 i = 0; i < cell->GetNumTradeRoutes(); i++)
		{
			if(cell->GetTradeRoute(i).IsValid())
			{
				TradeRoute route = cell->GetTradeRoute(i);
				if(!route.IsActive()) // skip deactivated routes (only exist for drawing until revisited, see #256)
				    continue;

				if((route.GetSource().GetOwner() != m_owner)
				&&(route.GetDestination().GetOwner() != m_owner)
				){
					sint32 tgold = static_cast<sint32>(route->GetValue() * g_theConstDB->Get(0)->GetCityOnTradeRouteCoeff());

					Unit fromCity = route.GetSource();
					Unit toCity = route.GetDestination();

					ROUTE_TYPE type;
					sint32 good;
					route.GetSourceResource(type, good);

					if(!route->IsBeingPirated())
					{
						g_player[m_owner]->AddGold(tgold);
						m_goldFromTransitRoutes+= tgold;
					
						/*
						SlicObject * so = new SlicObject("359TradePassing");
						so->AddRecipient(GetOwner());
						so->AddGold(tgold) ;
						so->AddCity(m_home_city); // m_home_city should equal fromCity
						so->AddGood(good);
						so->AddCity(fromCity);
						so->AddCity(toCity);
						so->AddCivilisation(fromCity.GetOwner());
						g_slicEngine->Execute(so);
						*/
					}
					else
					{ // message that tgold is not earned from transit due to pirating
						SlicObject * so = new SlicObject("358TradePassingPirated");
						so->AddRecipient(GetOwner());
						so->AddGold(tgold) ;
						so->AddCity(m_home_city); // m_home_city should equal fromCity
						so->AddGood(good);
						so->AddCity(fromCity);
						so->AddCity(toCity);
						so->AddCivilisation(fromCity.GetOwner());
						so->AddCivilisation(route->GetPiratingArmy().GetOwner());
						g_slicEngine->Execute(so);
					}
				}
			}
		}
	}
}

bool CityData::HasSpecialIcon() const
{
	//may add more to this bool so I can reuse the unseencell stuff and then break down the icon calls
	if (buildingutil_GetShowCityIconTop(GetEffectiveBuildings(), m_owner)) {
		return true;
	}
	if (wonderutil_GetShowCityIconTop(GetBuiltWonders())) {
		return true;
	}

	return false;
}

void CityData::AddCityExpansion()
{
	if (g_theConstDB->Get(0)->GetCityExpansionDenominator() > 0) //this checks if its specified in constDB
	{
		sint32 UrbanTile = PopCount()/(g_theConstDB->Get(0)->GetCityExpansionDenominator()); // No good idea to call something a denominator and give it the default value zero
		if(GetNumUrbanTile(m_home_city.RetPos()) >= UrbanTile)
			return;

		sint32 UrbanImp = GetUrbanTileAvailable(m_home_city.RetPos());

		MapPoint point(m_home_city.RetPos());
		MapPoint SpotFound(m_home_city.RetPos());
		CityInfluenceIterator it(point, static_cast<sint32>(GetVisionRadius()));
		bool found = false;

		for(it.Start(); !it.End(); it.Next())
		{
			Cell *ncell = g_theWorld->GetCell(it.Pos());
			Cell *ocell = g_theWorld->GetCell(SpotFound);
			UrbanImp = GetUrbanTileAvailable(it.Pos()); //includes advance check
			if (UrbanImp < 0)
				continue;      // On the next tile an urban tilimp could be available

			if(point == it.Pos())
				continue;

			if(terrainutil_HasUrban(it.Pos()))
				continue;

			if(terrainutil_HasWonder(it.Pos()))
				continue;

			if(terrainutil_CanPlayerSpecialBuildAt(UrbanImp, m_owner, it.Pos())
			){
				if(ncell->GetNumDBImprovements() < ocell->GetNumDBImprovements()
				|| ncell->GetGoldFromTerrain()   > ocell->GetGoldFromTerrain()
				  )
				{
					found = true;
					SpotFound = it.Pos();
				}
			}
		}

		if(found)
		{
			g_player[m_owner]->CreateSpecialImprovement(UrbanImp, SpotFound, 0);
		}
	}
}

sint32 CityData::GetNumUrbanTile(const MapPoint pos) const
{
	CityInfluenceIterator it(pos, static_cast<sint32>(GetVisionRadius()));
	sint32 UrbanImp = 0;
	for(it.Start(); !it.End(); it.Next())
	{
		if(terrainutil_HasUrban(it.Pos()))
		{
			UrbanImp++;
		}
	}
	return UrbanImp;
}

sint32 CityData::GetUrbanTileAvailable(const MapPoint pos) const
{
	sint32 UrbanImp = -1;
	for (sint32 imp = 0; imp < g_theTerrainImprovementDB->NumRecords(); imp++)
	{
		const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(imp);
		const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(trec, pos);
		if( (eff && eff->GetIsUrban()) && (terrainutil_CanPlayerBuild(trec, m_owner, true))
		){
			UrbanImp = imp;
		}
	}
	return UrbanImp;
}

sint32 CityData::GetSlumTileAvailable(const MapPoint pos) const
{
	sint32 UrbanImp = -1;
	for (sint32 imp = 0; imp < g_theTerrainImprovementDB->NumRecords(); imp++)
	{
		const TerrainImprovementRecord *trec = g_theTerrainImprovementDB->Get(imp);
		const TerrainImprovementRecord::Effect *eff = terrainutil_GetTerrainEffect(trec, pos);
		if( (eff && eff->GetSlum()) && (terrainutil_CanPlayerBuild(trec, m_owner, true)) //<-but this makes it availbale in game
		){
			UrbanImp = imp;
		}
	}
	return UrbanImp;
}

void CityData::AddCitySlum()
{
	MapPoint point(m_home_city.RetPos());
	MapPoint SpotFound;
	CityInfluenceIterator it(point, static_cast<sint32>(GetVisionRadius()));

	if (g_theConstDB->Get(0)->GetCityExpansionDenominator() > 0) //this checks if its specified in constDB
	{
		sint32 UrbanTile = PopCount()/(g_theConstDB->Get(0)->GetCityExpansionDenominator());

		if(GetNumUrbanTile(m_home_city.RetPos()) < UrbanTile)
			return;

		for(it.Start(); !it.End(); it.Next())
		{
			sint32 UrbanImp = GetSlumTileAvailable(it.Pos());
			if (UrbanImp < 0)
				return;

			if(point == it.Pos())
				continue;

			if(terrainutil_HasWonder(it.Pos()))
				continue;

			if(terrainutil_CanPlayerSpecialBuildAt(UrbanImp, m_owner, it.Pos())
			){
				if (
				   (!SpotFound.IsValid())
				&& (terrainutil_HasUrban(it.Pos()))
				){
					SpotFound = it.Pos();
				}
			}

			if(GetNumUrbanTile(m_home_city.RetPos()) > UrbanTile)
			{
				g_player[m_owner]->CreateSpecialImprovement(UrbanImp, SpotFound, 0);
			}
		}
	}
}

bool CityData::IsBuildingCapitalization() const
{
	return m_build_queue.GetHead() != NULL && m_build_queue.GetHead()->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION;
}

bool CityData::IsBuildingInfrastructure() const
{
	return m_build_queue.GetHead() != NULL && m_build_queue.GetHead()->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE;
}

bool CityData::IsCoastal() const
{
	MapPoint pos = m_home_city.RetPos();
	return g_theWorld->IsNextToWater(pos.x, pos.y);
}

void CityData::PreResourceCalculation()
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
	t1.start();
#endif

	CalculateBonusFood();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateBonusFood            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateCoeffFood();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateCoeffFood            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateBonusProd();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateBonusProd            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateCoeffProd();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateCoeffProd            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateBonusGold();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateBonusGold            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateCoeffGold();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateCoeffGold            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateBonusScie();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateBonusScie            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));
	CalculateCoeffScie();
	DPRINTF(k_DBG_GOVERNOR, ("//  CalculateCoeffScie            = %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), GetName()));

#if defined(_DEBUG) || defined(USE_LOGGING)
	t1.stop();
#endif
}

sint32 CityData::GetSpecialistsResources(POP_TYPE pop) const
{
	sint32 resource = 0;

	if(m_specialistDBIndex[pop] >= 0
	&& m_specialistDBIndex[pop] < g_thePopDB->NumRecords())
	{
		resource += wonderutil_GetIncreaseSpecialists(g_player[m_owner]->m_builtWonders);

		switch(pop)
		{
			case POP_SCIENTIST:
			{
				resource += wonderutil_GetIncreaseScientists(m_builtWonders);
				resource += g_thePopDB->Get(m_specialistDBIndex[pop], g_player[m_owner]->GetGovernmentType())->GetScience();
				break;
			}
			case POP_FARMER:
			{
				resource += g_thePopDB->Get(m_specialistDBIndex[pop], g_player[m_owner]->GetGovernmentType())->GetFood();
				break;
			}
			case POP_LABORER:
			{
				resource += g_thePopDB->Get(m_specialistDBIndex[pop], g_player[m_owner]->GetGovernmentType())->GetProduction();
				break;
			}
			case POP_MERCHANT:
			{
				resource += g_thePopDB->Get(m_specialistDBIndex[pop], g_player[m_owner]->GetGovernmentType())->GetCommerce();
				break;
			}
			default:
			{
				Assert(false);
				break;
			}
		}
	}

	return resource;
}
