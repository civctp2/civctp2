//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : AI and automated governor handling.
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
// CTP1_HAS_RISEN_FROM_THE_GRAVE
// - When defined, does not use the CTP2 worker utilisation style.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Consider other tile improvements when there is no fitting one for the
//   preferred type.
// - Adapted the tile improvement choice to take the CTP2 ringwise fractional
//   worker utilisation into account.
// - Ambiguous floor call removed for .NET compilation.
// - Removed .NET warnings, by Martin Gühmann.
// - Enabled AI and mayors to build undersea tunnels, by Martin Gühmann.
// - Allowed AI to build food tile improvements when city growth is low, by Martin Gühmann.
// - Allowed to link cities on different continents with undersea tunnels, by Martin Gühmann.
// - Added owner call to FindRoadPath to match the new function signature, by Martin Gühmann.
// - Added terrain boni arguments to FindBestTileImprovement to record the
//   already given terrain boni for that city, by Martin Gühmann
// - Added roads around all cities. -Calvitix
// - Disabled the last change as it takes to much roads even if there is no
//   city to bypass.
// - Added GetBestTerraformImprovement function to find the best terraform
//   improvement. - Sep. 21st 2004 Martin Gühmann
// - Modified FindBestTileImprovement function so that the AI can now
//   terraform if there is no food improvement for that terrain, and the
//   according city needs more food. - Sep. 21st 2004 Martin Gühmann
// - Disabled the utilization factor for first ring food improvements, such
//   cities benefit from food improvmements even if the ring is not filled.
//   - Oct. 6th 2004 Martin Gühmann
// - Cleaned up GetBestTerraformImprovement function. - Oct. 6th 2004 Martin Gühmann
// - Fixed an error in the should terraform logic. - Oct. 6th 2004 Martin Gühmann
// - Disabled the utilization factor for all first ring improvements, so that
//   small ciries are more likly improved. - Feb. 21st 2005 Martin Gühmann
// - Replaced c-sytle casts by by standart static_cast's.
//   - Feb. 21st 2005 Martin Gühmann
// - Fixed AssignPopulation function, at least it is now in the state as
//   supposed to be, however this is still far from perfect:
//   - Fixed population assignment if city has slaves.
//   - Allow usage of the first record in the pop database
//   - Minimum entertainers are now assigend, even if there are otherwise no
//     specialists left to assign.
//   - Minimum farmer assignment has to be done, but first necessary tools
//     in CityData have to be created.
//   - Optimization for the food, production, happiness, science and gold
//     have to be implemented, again the problem of missing tools.
//   - Mar. 1st 2005 Martin Gühmann
// - Fixed crash in debug version
// - Replaced ComputeMinimumFoodWorkers by ComputeMinimumWorkers function.
//   - April 4th 2005 Martin Gühmann
// - Redesigned AssignPopulation function:
//   - Instead calculating an assigned variable, the amount of free workers
//     is now taken to figure out whether more pops can be assigned. This
//     method is less error prone.
//   - Entertainer assignment was moved to the end and a happiness
//     recalculation is done, because happiness is dependent on the amount
//     of assigned specialists at least if pollotion is on.
//   - All pops are turned into workers before pop assignment to make
//     assigned variable superflous.
//   - April 4th 2005 Martin Gühmann
// - Cleaned a little bit functions and added experimental code for slider
//   optimization, the code is currently outcommented as it seemed that it
//   harms the AI more than it helps. - April 15th 2005 Martin Gühmann
// - Improved cleanup to reduce memory leak reports.
// - Removed debug allocator version.
// - Added copy constructor to bypass a problem concerning memory
//   allocation. - June 18th 2005 Martin Gühmann
// - Added OptimizeSliders method and updated TestSliders method for
//   better AI sliders optimisation routines. - Jul 18th 2005 Martin Gühmann
// - Added code for new city resource calculation. (Aug 12th 2005 Martin Gühmann)
// - Repaired incorrect AddEvent parameters.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Standardized code (May 21st 2006 Martin Gühmann)
// - Restored happiness calculation when testing slider settings.
// - Cleaned up (double Get-calls, unused stuff, casts).
// - GetDBUnitRec added to get government dependent unit recs. (June 5th 2006 Martin Gühmann)
// - Allow spending all unused freight
// - Improved handling when no suitable item of a category is available
//   (including settlers, to support mods).
// - Replaced old const database by new one. (05-Aug-2007 Martin Gühmann)
// - AIs now consider path between more than one city. (17-Jan-2008 Martin Gühmann)
// - The AI now builds settlers, ships, and special units, while it is
//   replacing its garrison units. (30-Jun-2008 Martin Gühmann)
// - Corrected iterator problems (detected with _HAS_ITERATOR_DEBUGGING).
// - Prepared for trade route ordering on profit per invested caravan.
// - Allowed selection of government update when it will improve the situation
//   w.r.t. the limit on the number of cities, even when it does not solve the
//   problem completely. When having 21 cities, a limit of 20 will now be
//   preferred when currently having a limit of 10.
// - Prevented crash with missing population assignment data.
// - If the AI loses its Capitol it builds a new one in its most productive
//   city. (08-Sep-2008 Martin Gühmann)
// - Changed science formula to deduct crime after the government coefficient
//   like all other resources. (22-Jul-2009 Maq)
// - The AI can now select a special build list for small cities with enough
//   garrison units, and can use a special build list for maximum size
//   increasing increasing buildings. (20-Aug-2009 Martin Gühmann)
// - Fixed AI city rank calculation. (09-Nov-2009 Martin Gühmann)
// - The AI does not build more settlers than it needs. (02-Jan-2018 Martin Gühmann)
// - The AI does not build settlers or transporters if the minimum garrison
//   strength has not been met. More to do here. (02-Jan-2018 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"                 // Pre-compiled header
#include "governor.h"           // Own declarations: consistency check

#include "AdvanceRecord.h"
#include "AgreementMatrix.h"
#include <algorithm>
#include "ArmyData.h"
#include "BuildingBuildListRecord.h"
#include "BuildingRecord.h"
#include "BuildListSequenceRecord.h"
#include "Cell.h"
#include "cellunitlist.h"
#include "CityAstar.h"
#include "citydata.h"
#include "CityInfluenceIterator.h"
#include "CitySizeRecord.h"
#include "civarchive.h"     // CivArchive
#include "ConstRecord.h"
#include "ctpai.h"
#include "Goal.h"
#include "Diplomat.h"
#include "Events.h"
#include "gaiacontroller.h"
#include "GameEventUser.h"
#include "GameSettings.h"
#include "Globals.h"
#include "GoalRecord.h"
#include "GovernmentRecord.h"
#include "gstypes.h"                    // TERRAIN_TYPES
#include "Happy.h"
#include "installationtree.h"
#include <limits>
#include "mapanalysis.h"
#include "net_action.h"
#include "network.h"
#include "newturncount.h"
#include "Path.h"
#include "player.h"
#include "PopRecord.h"
#include "profileai.h"
#include "ResourceRecord.h"
#include "Scheduler.h"
#include "SlicContext.h"    // SlicContext
#include "StrDB.h"
#include "Strengths.h"
#include "stringutils.h"
#include "StrategyRecord.h"
#include "TerrainRecord.h"
#include "terrainutil.h"
#include "TerrImprove.h"
#include "TradeRouteData.h"
#include "tradeutil.h"
#include "Unit.h"
#include "UnitBuildListRecord.h"
#include "UnitRecord.h"
#include "UnitData.h"
#include "unitutil.h"
#include "WonderBuildListRecord.h"
#include "WonderRecord.h"
#include "WonderTracker.h"
#include "wonderutil.h"
#include "World.h"

#if defined(_DEBUG) || defined(USE_LOGGING)
#include "Timer.h"
#endif

extern CityAstar    g_city_astar;

namespace
{
	sint32 const            BORDER_DISTANCE_SEARCH_MULT = 4;
	/// Value for a trade route not requiring caravans (should not occur)
	double const            VALUE_FREE_LUNCH 	        =
		std::numeric_limits<double>::max();
}

Governor::GovernorVector    Governor::s_theGovernors;
Governor::CityDistQueue     Governor::s_CityDistQueue;
Governor::CityPairList      Governor::s_CityPairList;

//----------------------------------------------------------------------------
//
// Name       : Governor::ResizeAll
//
// Description: Resize the Governor data for a (different?) number of players.
//
// Parameters : newMaxPlayerId  : number of players (highest player id)
//
// Globals    : s_theGovernors  : updated
//
// Returns    : -
//
// Remark(s)  : static function
//
//----------------------------------------------------------------------------
void Governor::ResizeAll(const PLAYER_INDEX & newMaxPlayerId)
{
	size_t const	old_size = s_theGovernors.size();

	s_theGovernors.resize(newMaxPlayerId);

	for (size_t i = old_size; i < static_cast<size_t>(newMaxPlayerId); ++i)
	{
		s_theGovernors[i].SetPlayerId(static_cast<PLAYER_INDEX>(i));
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::Cleanup
//
// Description: Release the memory of the Governor data.
//
// Parameters : -
//
// Globals    : s_theGovernors
//
// Returns    : -
//
// Remark(s)  : static function
//
//----------------------------------------------------------------------------
void Governor::Cleanup(void)
{
	GovernorVector().swap(s_theGovernors);
	CityPairList  ().swap(s_CityPairList);
	CityDistQueue ().swap(s_CityDistQueue);
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetGovernor
//
// Description: Return a reference to the Governor of a player.
//
// Parameters : playerId        : player to govern
//
// Globals    : s_theGovernors
//
// Returns    : Governor &      : governor of the player
//
// Remark(s)  : static function
//
//----------------------------------------------------------------------------
Governor & Governor::GetGovernor(PLAYER_INDEX const & playerId)
{
	Assert(playerId >= 0);
	Assert(static_cast<size_t>(playerId) < s_theGovernors.size());

	return s_theGovernors[playerId];
}

//----------------------------------------------------------------------------
//
// Name       : Governor::Governor
//
// Description: Constructor
//
// Parameters : playerId    : Player to govern
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Governor::Governor(PLAYER_INDEX const & playerId)
:
	m_maximumUnitShieldCost     (0),
	m_currentUnitShieldCost     (0),
	m_playerId                  (playerId),
	m_currentUnitCount          (),
	m_neededFreight             (0),
	m_tileImprovementGoals      (),
	m_cityTileImpGoals          (),
	m_canBuildLandSettlers      (false),
	m_canBuildSeaSettlers       (false)
{ ; }

//----------------------------------------------------------------------------
//
// Name       : Governor::Governor
//
// Description: Copy-Constructor
//
// Parameters : copyme      : Governor to copy
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Governor::Governor(Governor const & copyme)
:
	m_maximumUnitShieldCost     (copyme.m_maximumUnitShieldCost),
	m_currentUnitShieldCost     (copyme.m_currentUnitShieldCost),
	m_playerId                  (copyme.m_playerId),
	m_currentUnitCount          (),
	m_neededFreight             (copyme.m_neededFreight),
	m_tileImprovementGoals      (),
	m_cityTileImpGoals          (),
	m_canBuildLandSettlers      (copyme.m_canBuildLandSettlers),
	m_canBuildSeaSettlers       (copyme.m_canBuildSeaSettlers)
{
	std::copy(copyme.m_currentUnitCount.begin(),
	          copyme.m_currentUnitCount.end(),
	          std::back_inserter(m_currentUnitCount)
	         );
	std::copy(copyme.m_tileImprovementGoals.begin(),
	          copyme.m_tileImprovementGoals.end(),
	          std::back_inserter(m_tileImprovementGoals)
	         );
	// m_cityTileImpGoals deliberately left empty, unlike the above - it's
	// pure scratch space local to one PlaceTileImprovements call, never
	// holding meaningful state between calls.
}

//----------------------------------------------------------------------------
//
// Name       : Governor::~Governor
//
// Description: Destructor
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
Governor::~Governor()
{
	UnitCountVector().swap(m_currentUnitCount);
	TiGoalQueue().swap(m_tileImprovementGoals);
	TiGoalQueue().swap(m_cityTileImpGoals);
}

void Governor::SetPlayerId(const PLAYER_INDEX &playerId)
{
	m_playerId = playerId;
}

void Governor::Resize( const sint16 & xSize,
                       const sint16 & ySize,
                       const sint16 & resolution )
{
}

sint32 Governor::ComputeBestGovernment() const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	for (sint32 gov_index = 0; gov_index < strategy.GetNumGovernment(); gov_index++)
	{
		const GovernmentRecord * rec = strategy.GetGovernment(gov_index);

		if(!player_ptr->HasAdvance(rec->GetEnableAdvanceIndex()))
			continue;

		bool obsolete = false;
		for(sint32 i = 0; i < rec->GetNumObsoleteAdvance(); i++){
			if(player_ptr->HasAdvance(rec->GetObsoleteAdvance(i)->GetIndex())){
				obsolete = true;
				break;
			}
		}
		if (obsolete)
			continue;

		if(!g_theGameSettings->IsNoCityLimit())
		{
			sint32 const newCityLimit = rec->GetTooManyCitiesThreshold();
			if(player_ptr->GetNumCities() > newCityLimit)
			{
				sint32 const oldCityLimit =
					g_theGovernmentDB->Get(player_ptr->GetGovernmentType())->
					GetTooManyCitiesThreshold();

				if(newCityLimit > oldCityLimit)
				{
					// Do consider upgrading, to improve the situation
				}
				else
				{
					continue;
				}
			}
		}

		return rec->GetIndex();
	}

	return CTPRecord::INDEX_INVALID;
}

StringId Governor::GetGovernmentAdvice() const
{
	Player const *      player_ptr      = g_player[m_playerId];
	if (player_ptr)
	{
		Governor const &    governor        = Governor::GetGovernor(m_playerId);
		sint32 const        new_gov_index   = governor.ComputeBestGovernment();

		if (new_gov_index != CTPRecord::INDEX_INVALID)
		{
			GovernmentRecord const *    new_gov_rec     =
				g_theGovernmentDB->Get(new_gov_index);
			sint32 const                new_rank        = new_gov_rec->GetRank();
			sint32 const                current_rank    =
				g_theGovernmentDB->Get(player_ptr->GetGovernmentType())->GetRank();

			if (new_rank == current_rank)
			{
				return new_gov_rec->GetSameRankAdvice();
			}
			else if (new_rank > current_rank)
			{
				return new_gov_rec->GetHigherRankAdvice();
			}
		}
	}

	return -1;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetMaxSliderSettings
//
// Description: Finds the possible maximal slider settings.
//
// Parameters : sliders_setting: Filled with the possible maximal slider
//                               settings.
//
// Globals    : g_player: List of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void Governor::GetMaxSliderSettings(SlidersSetting & sliders_setting) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	sliders_setting.m_deltaProduction = 2 + static_cast<sint32>(player_ptr->GetWorkdayExpectation());
	sliders_setting.m_deltaGold       = 2 + static_cast<sint32>(player_ptr->GetWagesExpectation());
	sliders_setting.m_deltaFood       = 2 + static_cast<sint32>(player_ptr->GetRationsExpectation());
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ProdSliderReachedMin
//
// Description: Tests whether the production slider is at the minimum or
//              below.
//
// Parameters : sliders_setting: The slider settings to test.
//
// Globals    : g_player: List of players
//
// Returns    : Whether the production slider has reached its minimum
//              or is below.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool Governor::ProdSliderReachedMin(SlidersSetting & sliders_setting) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);
	return player_ptr->GetWorkdayExpectation() - sliders_setting.m_deltaProduction >= 2;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GoldSliderReachedMin
//
// Description: Tests whether the gold slider is at the minimum or
//              below.
//
// Parameters : sliders_setting: The slider settings to test.
//
// Globals    : g_player: List of players
//
// Returns    : Whether the gold slider has reached its minimum
//              or is below.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool Governor::GoldSliderReachedMin(SlidersSetting & sliders_setting) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);
	return player_ptr->GetWagesExpectation() - sliders_setting.m_deltaGold >= 2;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::FoodSliderReachedMin
//
// Description: Tests whether the food slider is at the minimum or
//              below.
//
// Parameters : sliders_setting: The slider settings to test.
//
// Globals    : g_player: List of players
//
// Returns    : Whether the food slider has reached its minimum
//              or is below.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool Governor::FoodSliderReachedMin(SlidersSetting & sliders_setting) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);
	return player_ptr->GetRationsExpectation() - sliders_setting.m_deltaFood >= 2;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::SetSliders
//
// Description: Gets the current slider settings.
//
// Parameters : sliders_setting: Filled with the current slider settings.
//
// Globals    : g_player: List of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void Governor::SetSliders(const SlidersSetting & sliders_setting, bool hasAllAdvances) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	player_ptr->SetWorkdayLevel(static_cast<sint32>(player_ptr->GetWorkdayExpectation()) - sliders_setting.m_deltaProduction);
	player_ptr->SetWagesLevel  (static_cast<sint32>(player_ptr->GetWagesExpectation())   - sliders_setting.m_deltaGold);
	player_ptr->SetRationsLevel(static_cast<sint32>(player_ptr->GetRationsExpectation()) - sliders_setting.m_deltaFood);

	UnitDynamicArray * city_list = player_ptr->GetAllCitiesList();
	sint32             cityCount = city_list ? city_list->Num() : 0;

	for (sint32 i = 0; i < cityCount; i++)
	{
		CityData * city = city_list->Access(i)->GetCityData();
		double old_happiness = city->GetHappiness();

		//Added by Martin Gühmann to take specialists into account.
		//Well this has an effect but the AI seems to perform worse with it.
		//Right direction but more debug work is needed.
		AssignPopulation(city, hasAllAdvances);

		city->ProcessAllResources();
	}
}

void Governor::SliderTests::CalcTests(PLAYER_INDEX playerId)
{
	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(playerId).GetCurrentStrategy();

	double deficit_spending = strategy.GetDeficitSpending();
	sint32 min_happiness = g_theConstDB->Get(0)->GetRiotLevel(); // Have a default value if strategy.GetMinimumHappiness is not present
	double max_wage_percent;

	strategy.GetMinimumHappiness(min_happiness);
	strategy.GetMaximumWagePercent(max_wage_percent);

	sint32 total_gold_cost = 0;
	sint32 gross_gold = 0;
	m_food = 0;
	m_production = 0;
	m_gold = 0;

	m_foodTest = true;
	m_happinessTest = true;

	Player * player_ptr = g_player[playerId];
	Assert(player_ptr);
	UnitDynamicArray *  city_list = player_ptr->GetAllCitiesList();
	sint32              num_cities = city_list ? city_list->Num() : 0;

	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		Unit const & city_unit = city_list->Get(city_index);

		Assert(city_unit.m_id != 0);
		Assert(city_unit->GetCityData());
		CityData * city = city_unit->GetCityData();

		double new_happiness = city->GetHappiness();
		if(new_happiness < min_happiness
		|| city->EntertainerCount() == city->PopCount()
		){
			m_happinessTest = false;
		}

		m_food += city->GetNetCityFood();
		m_production += city->GetNetCityProduction();

		total_gold_cost += city->GetWagesNeeded();
		m_gold += city->GetNetCityGold();
		m_gold += city->GetScience();
		gross_gold += city->GetGrossCityGold();

		if (city->GetNetCityFood() < 0.0)
		{
			m_foodTest = false;
		}
	}

	m_production += player_ptr->GetProductionFromFranchises();
	m_productionTest = (m_production >= player_ptr->GetReadinessCost());

	double wages_percent = static_cast<double>(total_gold_cost) / static_cast<double>(gross_gold);
	m_goldTest = (wages_percent <= max_wage_percent);

	sint32 player_gold = player_ptr->GetGold();
	if(m_gold < 0
	&& m_gold + player_gold < 0
	&& m_gold > player_gold * -deficit_spending
	){
		m_goldTest = false;
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::TestSliderSettings
//
// Description: Checks if the given slider settings are enough to support
//              the empire.
//
// Parameters : sliders_setting:  The slider settings to be tested.
//              slider_tests:     Object calculating the slider tests and
//                                holding the results.
//
// Globals    : -
//
// Returns    : bool:             Whether production_test and gold_test and
//                                food_test and happiness_test are true.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void Governor::TestSliderSettings(const SlidersSetting & sliders_setting,
                                  SliderTests &          slider_tests,
                                  bool     hasAllAdvances) const
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;

	t1.start();

	DPRINTF(k_DBG_GOVERNOR, ("\n"));
	DPRINTF(k_DBG_GOVERNOR, ("// TEST SLIDER SETTINGS -- Turn    %d\n", g_player[m_playerId]->GetCurRound()));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Player  %d\n", m_playerId));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Workday %d\n", sliders_setting.m_deltaProduction));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Wages   %d\n", sliders_setting.m_deltaGold));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Rations %d\n", sliders_setting.m_deltaFood));
#endif

	SetSliders(sliders_setting, hasAllAdvances);
	slider_tests.CalcTests(m_playerId);

#if defined(_DEBUG) || defined(USE_LOGGING)
	DPRINTF(k_DBG_GOVERNOR_DETAIL, ("// Elapsed time                        = %f ms (All cities)\n", t1.getElapsedTimeInMilliSec()));
	DPRINTF(k_DBG_GOVERNOR_DETAIL, ("\n"));
	t1.stop();
#endif
}

//----------------------------------------------------------------------------
//
// Name       : Governor::OptimizeSliders
//
// Description: Optimizes the sliders
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
void Governor::OptimizeSliders(SlidersSetting & sliders_setting) const
{
	sint32 value, valueProd, valueGold, valueFood;

	GetMaxSliderSettings(sliders_setting);
	SliderTests slider_tests;
	SliderTests prod_slider_tests;
	SliderTests gold_slider_tests;
	SliderTests food_slider_tests;
	SlidersSetting prod_sliders_setting;
	SlidersSetting gold_sliders_setting;
	SlidersSetting food_sliders_setting;

	g_player[m_playerId]->PreResourceCalculation();
	bool hasAllAdvances = g_player[m_playerId]->m_advances->HasAllAdvances();

	TestSliderSettings(sliders_setting, slider_tests, hasAllAdvances);

	while( !ProdSliderReachedMin(sliders_setting)
	||     !GoldSliderReachedMin(sliders_setting)
	||     !FoodSliderReachedMin(sliders_setting)
	){
		slider_tests.Log();
		if(slider_tests.m_happinessTest)
			break;

		value = slider_tests.GetValue();

		if(!ProdSliderReachedMin(sliders_setting))
		{
			prod_sliders_setting = sliders_setting;
			prod_sliders_setting.m_deltaProduction--;
			TestSliderSettings(prod_sliders_setting, prod_slider_tests, hasAllAdvances);
			valueProd = prod_slider_tests.GetValue();
			prod_slider_tests.Log();
			slider_tests = prod_slider_tests;
		}
		else
		{
			valueProd = std::numeric_limits<sint32>::min(); // Should be lower than everything else
		}

		if(!GoldSliderReachedMin(sliders_setting))
		{
			gold_sliders_setting = sliders_setting;
			gold_sliders_setting.m_deltaGold--;
			TestSliderSettings(gold_sliders_setting, gold_slider_tests, hasAllAdvances);
			valueGold = gold_slider_tests.GetValue();
			gold_slider_tests.Log();
			slider_tests = gold_slider_tests;
		}
		else
		{
			valueGold = std::numeric_limits<sint32>::min();
		}

		if(!FoodSliderReachedMin(sliders_setting))
		{
			food_sliders_setting = sliders_setting;
			food_sliders_setting.m_deltaFood--;
			TestSliderSettings(food_sliders_setting, food_slider_tests, hasAllAdvances);
			valueFood = food_slider_tests.GetValue();
			food_slider_tests.Log();
			slider_tests = food_slider_tests;
		}
		else
		{
			valueFood = std::numeric_limits<sint32>::min();
		}

		// If all values are good and the people are
		// happy enough, we are happy, too.
		if(value >= valueProd
		&& value >= valueGold
		&& value >= valueFood
		&& slider_tests.m_happinessTest
		){
			break;
		}
		// Real equal need a better solution
		else if(valueProd >= value
		&&      valueProd >= valueGold
		&&      valueProd >= valueFood
		&&      prod_slider_tests.m_productionTest
		){
			sliders_setting = prod_sliders_setting;
			slider_tests = prod_slider_tests;
		}
		// Real equal need a better solution
		else if(valueGold >= value
		&&      valueGold >= valueProd
		&&      valueGold >= valueFood
		&&      gold_slider_tests.m_goldTest
		){
			sliders_setting = gold_sliders_setting;
			slider_tests = gold_slider_tests;
		}
		// Real equal need a better solution
		else if(valueFood >= value
		&&      valueFood >= valueProd
		&&      valueFood >= valueGold
		&&      food_slider_tests.m_foodTest
		){
			sliders_setting = food_sliders_setting;
			slider_tests = food_slider_tests;
		}
		else{
			// the values aren't satisfying we have to select one other setting.
			// Real equal need a better solution
			if(valueProd >= value
			&& valueProd >= valueGold
			&& valueProd >= valueFood
			){
				sliders_setting = prod_sliders_setting;
				slider_tests = prod_slider_tests;
			}
			// Real equal need a better solution
			else if(valueGold >= value
			&&      valueGold >= valueProd
			&&      valueGold >= valueFood
			){
				sliders_setting = gold_sliders_setting;
				slider_tests = gold_slider_tests;
			}
			// Real equal need a better solution
			else if(valueFood >= value
			&&      valueFood >= valueProd
			&&      valueFood >= valueGold
			){
				sliders_setting = food_sliders_setting;
				slider_tests = food_slider_tests;
			}
			else{
				if(!FoodSliderReachedMin(sliders_setting)){
					sliders_setting = food_sliders_setting;
					slider_tests = food_slider_tests;
				}
				else if(!ProdSliderReachedMin(sliders_setting)){
					sliders_setting = prod_sliders_setting;
					slider_tests = prod_slider_tests;
				}
				else if(!GoldSliderReachedMin(sliders_setting)){
					sliders_setting = gold_sliders_setting;
					slider_tests = gold_slider_tests;
				}
				else{
					Assert(false);
					break;
				}
			}
		}
	}
}

bool Governor::AddRoadPriority(Path & path, const double & priority_delta, const CityData * city)
{
	double bonus = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy().GetRoadUtilityBonus();

	// The element governing the originating city's current build-list
	// sequence carries its own road bonus on top of the strategy-wide one,
	// same as FindBestTileImprovement's Improve*Bonus handling. A road
	// connects two cities; this uses the one the search started from.
	const StrategyRecord::BuildListSequenceElement *elem =
	    GetMatchingSequenceElement(city);
	if(elem)
		bonus += elem->GetRoadUtilityBonus();

	TiGoal ti_goal;
	ti_goal.utility =  bonus * priority_delta;

	MapPoint old, pos;
	path.Start(old);
	path.Next(pos);

	bool addedRoadToQueue = false;

	for ( ; !path.IsEnd(); path.Next(pos))
	{
		ti_goal.type = GetBestRoadImprovement(pos);

		if(
		         ti_goal.type >= 0
		    &&  !g_theWorld->GetCell(pos)->HasTerrainImprovementOrInFuture(ti_goal.type)
		    &&   g_theWorld->GetCell(pos)->FutureMoveCostsAreReallyBig()
		  )
		{
			ti_goal.pos = pos;

			g_theWorld->GetCell(pos)->CalculateTmpFutureMoveCosts(ti_goal.type);

			m_tileImprovementGoals.push_back(ti_goal);
			addedRoadToQueue = true;
		}
	}

	return addedRoadToQueue;
}

void Governor::ComputeRoadPriorities()
{
	Player *            player_ptr  = g_player[m_playerId];
	Assert(player_ptr);
	UnitDynamicArray *  cityList    = player_ptr->GetAllCitiesList();
	sint32 const        num_cities  = cityList ? cityList->Num() : 0;

	g_theWorld->ResetAllTmpFutureMoveCosts();

	s_CityPairList.clear();

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	sint32 max_eval = strategy.GetBuildRoadsToClosestCities();
	double baseRoadPriority = strategy.GetBaseRoadPriorityVsThreatRank();

	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		Unit    city_unit   = cityList->Get(city_index);

		if (!city_unit.CD()->GetUseGovernor())
			continue;

		double threat_rank = MapAnalysis::GetMapAnalysis().GetThreatRank(city_unit.CD());
		threat_rank *= (1.0 - baseRoadPriority);
		threat_rank += baseRoadPriority;

		s_CityDistQueue.clear();

		for (sint32 neighbor_index = 0; neighbor_index < num_cities; neighbor_index++)
		{
			if (neighbor_index == city_index)
				continue;

			Unit         neighbor_unit = cityList->Get(neighbor_index);
			sint32 const neighbor_dist = MapPoint::GetSquaredDistance(neighbor_unit.RetPos(), city_unit.RetPos());

			if( neighbor_dist <= g_theConstDB->Get(0)->GetBorderSquaredRadius() * BORDER_DISTANCE_SEARCH_MULT
			&& !IsInCityPairList(city_index, neighbor_index))
			{
				s_CityDistQueue.push_back(CityDist(neighbor_unit, neighbor_dist));
				s_CityPairList.push_back(CityPair(city_index, neighbor_index));
			}
		}

		if (s_CityDistQueue.size() == 0)
			continue;

		CityDistQueue::iterator max_iter = s_CityDistQueue.begin() + std::min(static_cast<size_t>(max_eval),
																	  s_CityDistQueue.size()
																	 );

		std::partial_sort(s_CityDistQueue.begin(), max_iter, s_CityDistQueue.end(), std::less<CityDist>());

		for
		(
		    CityDistQueue::const_iterator iter = s_CityDistQueue.begin();
		    iter != max_iter;
		    ++iter
		)
		{
			float   total_cost = 0.0;
			Path    found_path;

			Unit    min_neighbor_unit = iter->m_city;

			if (g_city_astar.FindRoadPath(city_unit.RetPos(), min_neighbor_unit.RetPos(),
				m_playerId,
				found_path,
				total_cost ))
			{
				Assert(0 < found_path.Num());
				if(AddRoadPriority(found_path, threat_rank, city_unit.CD()))
					break;
			}
		}

		s_CityDistQueue.clear();
	}

	s_CityPairList.clear();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::IsBorderTile
//
// Description: Checks whether any neighbor of pos belongs to someone other
//              than this player (a foreign or unclaimed tile) - i.e.
//              whether pos itself faces outward at the empire's edge.
//
// Parameters : pos: Position to check.
//
// Globals    : g_theWorld
//
// Returns    : bool
//
//----------------------------------------------------------------------------
bool Governor::IsBorderTile(const MapPoint & pos) const
{
	for(sint32 dd = 0; dd < (sint32)NOWHERE; dd++)
	{
		MapPoint npos;
		if(pos.GetNeighborPosition((WORLD_DIRECTION)dd, npos)
		&& g_theWorld->GetCell(npos)->GetOwner() != m_playerId)
		{
			return true;
		}
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::WouldDestroyUnrelatedImprovement
//
// Description: Checks whether building newRec on this cell would silently
//              destroy an existing improvement that isn't itself a prior
//              tier of the same installation kind (e.g. a Farm, as opposed
//              to a Listening Post being upgraded to a Radar Station).
//
// Parameters : cell:       The cell being considered.
//              newRec:     The improvement type that would be built.
//              pos:        Position of cell (to resolve terrain effects).
//              isSameKind: terrainutil_IsAirfieldEffect/IsFortEffect/
//                          IsDetectorEffect - matching whichever finder
//                          produced newRec.
//
// Globals    : g_theTerrainImprovementDB
//
// Returns    : bool: true if something not of the same kind would be lost.
//
// Remark(s)  : Mirrors Cell::InsertBuiltImprovementType's own auto-remove-on-
//              completion logic (existingRec->GetClass() & newRec->
//              GetExcludes()) - an existing improvement whose class isn't
//              excluded by newRec would simply coexist untouched (e.g. a
//              Road), so it's never a reason to skip a tile.
//
//----------------------------------------------------------------------------
bool Governor::WouldDestroyUnrelatedImprovement(const Cell * cell, const TerrainImprovementRecord * newRec, const MapPoint & pos, InstallationEffectQualifier isSameKind) const
{
	for(sint32 index = 0; index < cell->GetNumBuiltImprovementTypes(); index++)
	{
		sint32 const                      existingType = cell->GetBuiltImprovementType(index);
		const TerrainImprovementRecord *  existingRec  = g_theTerrainImprovementDB->Get(existingType);

		if((existingRec->GetClass() & newRec->GetExcludes()) == 0)
			continue;

		if(!isSameKind(terrainutil_GetTerrainEffect(existingRec, pos)))
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::AddInstallationPriority
//
// Description: Looks for a spot to build one Airfield/Fort/detector-class
//              installation (whichever finder identifies) for this city, if
//              it doesn't already have (or isn't already building) one
//              anywhere in its territory.
//
// Parameters : city:       The city to place an installation for.
//              finder:     terrainutil_GetBestAirfield/GetBestFort/
//                          GetBestDetector - resolves the best buildable
//                          type of this kind at a given position, or NULL
//                          if none is buildable there.
//              isSameKind: terrainutil_IsAirfieldEffect/IsFortEffect/
//                          IsDetectorEffect - matching qualifier, used to
//                          tell an upgrade (previous tier of the same kind)
//                          apart from an unrelated improvement that would
//                          be destroyed by building here.
//              utility:    Priority value to give the resulting TiGoal.
//              requireBorderTile: if true (detectors, forts), a candidate
//                      tile must also face outward - see IsBorderTile. A
//                      city with no such tile anywhere (not on the
//                      empire's edge) simply never finds a candidate.
//
// Globals    : g_theWorld, g_theConstDB
//
// Returns    : bool: true if a goal was queued.
//
// Remark(s)  : Searches ring by ring outward-in, starting at the city's
//              fixed border radius and working down toward its (smaller,
//              size-dependent) collection area, so an installation lands on
//              the outskirts and doesn't intervene with worked tiles unless
//              nothing else is free. The whole territory is always scanned
//              even after a candidate is found, so an existing instance
//              anywhere (found on any ring) cancels placing a second one.
//
//----------------------------------------------------------------------------
bool Governor::AddInstallationPriority(const Unit & city, BestInstallationFinder finder, InstallationEffectQualifier isSameKind, const double & utility, bool requireBorderTile)
{
	MapPoint const cityPos      = city.RetPos();
	sint32   const owner        = city.GetOwner();
	sint32   const borderRadius = g_theConstDB->Get(0)->GetBorderIntRadius();

	bool    alreadyHasOne  = false;
	bool    foundCandidate = false;
	TiGoal  candidate;

	for(sint32 ring = borderRadius; ring >= 1 && !alreadyHasOne; ring--)
	{
		CircleIterator it(cityPos, ring, ring - 1);

		for(it.Start(); !it.End() && !alreadyHasOne; it.Next())
		{
			MapPoint const pos  = it.Pos();
			Cell *         cell = g_theWorld->GetCell(pos);

			if(cell->GetOwner() != owner)
				continue;

			if(cell->HasCity() && cell->GetCityOwner() != city)
				continue;

			const TerrainImprovementRecord * rec = finder(m_playerId, pos);
			if(!rec)
				continue;

			if(cell->HasTerrainImprovementOfType(rec->GetIndex()))
			{
				// Counts regardless of requireBorderTile - an existing
				// instance anywhere (even one not facing the border,
				// e.g. built by a human, or from before this rule
				// existed) still means this city already has one.
				alreadyHasOne = true;
				break;
			}

			if(requireBorderTile && !IsBorderTile(pos))
				continue;

			if(WouldDestroyUnrelatedImprovement(cell, rec, pos, isSameKind))
				continue;

			if(!foundCandidate)
			{
				candidate.type    = rec->GetIndex();
				candidate.pos     = pos;
				candidate.utility = utility;
				foundCandidate    = true;
			}
		}
	}

	if(alreadyHasOne || !foundCandidate)
		return false;

	m_tileImprovementGoals.push_back(candidate);
	return true;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeInstallationPriorities
//
// Description: Queues up to one Airfield, one Fort, and one detector-class
//              installation (Listening Post/Radar Station/Sonar Buoy) per
//              city, for cities the AI hasn't given one to yet.
//
// Parameters : -
//
// Globals    : g_player
//
// Returns    : -
//
// Remark(s)  : Called from PlaceTileImprovements alongside
//              ComputeRoadPriorities - feeds the same m_tileImprovementGoals
//              queue, sorted and budget-capped there like every other tile
//              improvement goal.
//
//----------------------------------------------------------------------------
void Governor::ComputeInstallationPriorities()
{
	Player *            player_ptr  = g_player[m_playerId];
	Assert(player_ptr);
	UnitDynamicArray *  cityList    = player_ptr->GetAllCitiesList();
	sint32 const        num_cities  = cityList ? cityList->Num() : 0;

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		Unit const & city = cityList->Get(city_index);

		if (!city.CD()->GetUseGovernor())
			continue;

		// The element governing this city's current build-list sequence
		// carries its own installation bonuses on top of the strategy-wide
		// ones, same as FindBestTileImprovement's Improve*Bonus handling.
		const StrategyRecord::BuildListSequenceElement *elem =
		    GetMatchingSequenceElement(city.CD());

		double airfieldUtility = strategy.GetAirfieldUtilityBonus();
		double fortUtility     = strategy.GetFortUtilityBonus();
		double detectorUtility = strategy.GetDetectorUtilityBonus();
		if(elem)
		{
			airfieldUtility += elem->GetAirfieldUtilityBonus();
			fortUtility     += elem->GetFortUtilityBonus();
			detectorUtility += elem->GetDetectorUtilityBonus();
		}

		// Unlike tile improvements/roads, these bonuses were previously
		// used as a flat, unscaled utility - which lost every competition
		// against terrain improvements whose scaled utility regularly runs
		// well above their own raw bonus (confirmed empirically: an 800-
		// turn playtest built zero Forts/Detectors/rarely an Airfield for
		// one player, while roads/farms/etc. routinely built). Scale by
		// this city's own ThreatRank instead - a factor of [1.0, 2.0], so
		// a calm city's installation still competes at exactly its old
		// baseline value (never worse than before), while a genuinely
		// threatened city's bid can double, giving it a real chance
		// against terrain improvements when the two compete for the same
		// turn's budget. Airfields aren't restricted to border tiles (they
		// support inland power projection too), but a threatened inland
		// city benefits from one just as much as a threatened border city,
		// so the same scaling applies to all three.
		double const threat_rank = MapAnalysis::GetMapAnalysis().GetThreatRank(city.CD());
		double const threat_scale = 1.0 + threat_rank;
		airfieldUtility *= threat_scale;
		fortUtility     *= threat_scale;
		detectorUtility *= threat_scale;

		AddInstallationPriority(city, terrainutil_GetBestAirfield, terrainutil_IsAirfieldEffect, airfieldUtility);
		// Forts and detectors only make sense facing outward at the
		// empire's edge - require a border tile, so purely interior
		// cities never get one.
		AddInstallationPriority(city, terrainutil_GetBestFort,     terrainutil_IsFortEffect,     fortUtility,     true);

		// No need to build installations to increase the vision range if we already see everything
		if(!wonderutil_GetGlobalRadar(player_ptr->m_builtWonders))
			AddInstallationPriority(city, terrainutil_GetBestDetector, terrainutil_IsDetectorEffect, detectorUtility, true);
	}
}

bool Governor::IsInCityPairList(sint32 city, sint32 neighborCity) const
{
	for(size_t i = 0; i < s_CityPairList.size(); ++i)
	{
		CityPair cityPair = s_CityPairList[i];

		if( (cityPair.m_city         == city
		&&   cityPair.m_neighborCity == neighborCity
		    )
		||  (cityPair.m_city         == neighborCity
		&&   cityPair.m_neighborCity == city
		    )
		){
			return true;
		}
	}

	return false;
}

void Governor::PlaceTileImprovements()
{
	Player *                player_ptr  = g_player[m_playerId];
	Assert(player_ptr);
	StrategyRecord const &  strategy    =
	    Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	sint32  reserve_pw   = 0;
	strategy.GetPublicWorksReserve(reserve_pw);

	m_tileImprovementGoals.clear();
	ComputeRoadPriorities();          // may add to m_tileImprovementGoals
	ComputeInstallationPriorities();  // may add to m_tileImprovementGoals

	TiGoal ti_goal;
	UnitDynamicArray *  cityList    = player_ptr->GetAllCitiesList();
	sint32 const        num_cities  = cityList ? cityList->Num() : 0;

	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		Unit const &    unit = cityList->Get(city_index);
		Assert(unit->GetCityData());
		CityData *      city = unit->GetCityData();

		if(!city->GetUseGovernor())
			continue;

		CityInfluenceIterator it(unit.RetPos(), city->GetSizeIndex());

		// GetRingImprovementPriority (CityData.cpp) scores a tile purely by
		// which ring it's in - identical for every city that reaches that
		// ring, every turn, with no regard for how much of that ring this
		// specific city has already improved. A city with a large or
		// terrain-rich ring 1 can keep winning every one of its own
		// candidates there turn after turn while a sibling city's equally
		// deserving but lower-terrain ring 1 never gets a look in, since
		// nothing discounts a ring as it fills up. The loop below both
		// tallies, for this city, how many of each ring's eligible tiles
		// already count as improved and gathers this turn's candidates in
		// one pass, since both share the same eligibility filter.
		sint32 const maxRing = g_theCitySizeDB->NumRecords();
		std::vector<sint32> ringTotal(maxRing, 0);
		std::vector<sint32> ringImproved(maxRing, 0);

		// m_cityTileImpGoals (a Governor member, not a local) is reused
		// city after city, turn after turn - clear() keeps its already-
		// grown heap buffer instead of freeing and reallocating it from
		// empty on every single city, which otherwise fragments the heap
		// over a long game (see tileimp_placement_priority memory).
		m_cityTileImpGoals.clear();
		// A tile counts toward ringImproved[] (but never becomes a
		// candidate) if it already has a food/prod/gold improvement under
		// construction, or carries a non-Fort/Airfield/detector-style
		// installation, or FindBestTileImprovement has nothing better to
		// offer it and it already carries a finished food/prod/gold
		// improvement. Otherwise, if FindBestTileImprovement finds a
		// candidate - whether the tile is bare or already improved, no
		// distinction is made here - it goes into m_cityTileImpGoals for
		// scoring below.
		for (it.Start(); !it.End(); it.Next())
		{
			if(unit.RetPos() == it.Pos())
				continue;

			Cell const * cell = g_theWorld->GetCell(it.Pos());

			if(cell->GetCityOwner() != unit)
				continue;

			sint32 const ring = city->GetRing(it.Pos());
			if(ring >= 0 && ring < maxRing)
			{
				ringTotal[ring]++;
			}
			else
			{
				Assert(false);
			}

			if(cell->GetNumUnfinishedImprovements() > 0)
			{
				if(HasUnfinishedFoodProdOrGoldImprovement(it.Pos()))
					ringImproved[ring]++;

				continue;
			}

			if(g_theInstallationTree->GetCount(it.Pos()) > 0
			&& g_theWorld->GetCell(it.Pos())->GetTerrainType() != terrainutil_GetDead())
			{
				ringImproved[ring]++;
				continue;
			}

			if(FindBestTileImprovement(it.Pos(), ti_goal))
			{
				m_cityTileImpGoals.push_back(ti_goal);
			}
			else if(HasFoodProdOrGoldImprovement(it.Pos()))
			{
				ringImproved[ring]++;
			}
		}

		for
		(
		    TiGoalQueue::const_iterator   iter  = m_cityTileImpGoals.begin();
		                                  iter != m_cityTileImpGoals.end();
		                                ++iter
		)
		{
			ti_goal = *iter;
			sint32 const ring         = city->GetRing(ti_goal.pos);
			sint32 const terrain_type = g_theWorld->GetCell(ti_goal.pos)->GetTerrainType();

			// Moved here from inside FindBestTileImprovement: the per-ring
			// saturation discount ScaleUtilityForCitySize now also applies
			// (on top of its own ring-distance scaling) needs this city's
			// ring tallies finalized first - see its own comment.
			ScaleUtilityForCitySize(ti_goal, city, ti_goal.pos, terrain_type, strategy,
			                         ringImproved[ring], ringTotal[ring]);

			m_tileImprovementGoals.push_back(ti_goal);
		}
	}

	sint32 max_eval = 0;
	(void) strategy.GetMaxEvalTileImprovements(max_eval);
	TiGoalQueue::iterator max_iter =
		m_tileImprovementGoals.begin() + std::min(static_cast<size_t>(max_eval),
												  m_tileImprovementGoals.size()
												 );

	std::partial_sort(m_tileImprovementGoals.begin(),
	                  max_iter,
	                  m_tileImprovementGoals.end(),
	                  std::greater<TiGoal>()
	                 );

	sint32 avail_pw = player_ptr->GetMaterialsStored() - reserve_pw;

	for
	(
	    TiGoalQueue::const_iterator   iter  = m_tileImprovementGoals.begin();
	                                  iter != max_iter;
	                                ++iter
	)
	{
		sint32 const needed_pw =
		       terrainutil_GetProductionCost(iter->type, iter->pos, 0);

		if (needed_pw <= avail_pw)
		{
			// city/ring: which city this winner belongs to, and how far
			// out - lets a playtest log be grouped by city (is the budget
			// actually spreading across an empire's cities, or is one
			// still winning almost everything - see
			// [[tileimp_placement_priority]]) and by ring (is the
			// ring-priority curve in GetRingImprovementPriority actually
			// biting). Roads can span tiles between cities that no single
			// city owns, so city/ring are only meaningful when the tile
			// does belong to one - "-"/-1 marks a road tile that doesn't.
			Unit const tile_owner = g_theWorld->GetCell(iter->pos)->GetCityOwner();
			CityData * const tile_city =
			    tile_owner.IsValid() ? tile_owner.GetCityData() : NULL;
			MBCHAR const * const tile_city_name =
			    tile_city ? tile_city->GetName() : "-";
			sint32 const tile_ring =
			    tile_city ? tile_city->GetRing(iter->pos) : -1;
			MBCHAR const * cityList = (tile_city != NULL) ? g_theBuildListSequenceDB->Get(tile_city->GetBuildListSequenceIndex())->GetNameText() : "No City";

			// Logs every tile-improvement/road/installation goal that
			// actually wins its slot in the shared, sorted
			// m_tileImprovementGoals queue - the improvement's own name
			// already tells category (road, mine, Airfield, Fort, ...)
			// apart, so a playtest log can be grouped by name to check
			// whether one category is systematically outcompeting the
			// others for the turn's public-works budget. Installation
			// utility is ThreatRank-scaled now (AddInstallationPriority,
			// 13a760c93), not flat, but still on its own formula from
			// roads/tile improvements' rank/ratio scaling - worth
			// rechecking here if a category-dominance pattern recurs.
			DPRINTF(k_DBG_GOVERNOR, ("PlaceTileImprovements: turn %d player %d city %s ring %d builds %s on %s at (%d,%d), utility %f, cost %d, BuildList: %s\n",
			        NewTurnCount::GetCurrentRound(),
			        m_playerId,
			        tile_city_name,
			        tile_ring,
			        g_theTerrainImprovementDB->Get(iter->type)->GetNameText(),
			        g_theTerrainDB->Get(g_theWorld->GetCell(iter->pos)->GetTerrainType())->GetNameText(),
			        iter->pos.x,
			        iter->pos.y,
			        iter->utility,
			        needed_pw,
			        cityList
			        ));

			g_gevManager->AddEvent(GEV_INSERT_Tail,
			                       GEV_CreateImprovement,
			                       GEA_Player,      m_playerId,
			                       GEA_MapPoint,    iter->pos,
			                       GEA_Int,         iter->type,
			                       GEA_Int,         0,
			                       GEA_End);
			avail_pw -= needed_pw;
		}
		else
			break;
	}

	m_tileImprovementGoals.clear();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreGrowthImprovement
//
// Description: Scores a growth-category tile-improvement candidate - the
//              growth-branch body of FindBestTileImprovement, extracted
//              verbatim. Picked purely by terrain capability (the caller's
//              own condition), not by how much the city actually needs
//              growth (see the session's tileimp_need_vs_terrain notes).
//
// Parameters : goal:                    The candidate goal, type/utility set in place
//              pos:                     Position of the candidate tile
//              best_growth_improvement: The chosen growth improvement type
//              terr_food_rank:          Tile's food yield relative to the map average
//              growth_rank:             The city's growth percentile
//              strategy:                The city owner's current strategy
//              elem:                    The city's matching build-list-sequence element (may be NULL)
//
// Globals    : g_theTerrainImprovementDB, g_theWorld
//
// Returns    : sint32: the tile's added food from this improvement, 0 if
//              best_growth_improvement was unavailable
//
//----------------------------------------------------------------------------
sint32 Governor::ScoreGrowthImprovement(TiGoal & goal, const MapPoint & pos, sint32 best_growth_improvement, double terr_food_rank, double growth_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem) const
{
	if (best_growth_improvement < 0)
		return 0;

	const TerrainImprovementRecord * rec = g_theTerrainImprovementDB->Get(best_growth_improvement);
	const TerrainImprovementRecord::Effect * effect = terrainutil_GetTerrainEffect(rec, pos);

	goal.type = best_growth_improvement;

	// 0.0 only silences -Wmaybe-uninitialized, see ScaleUtilityForCitySize.
	double bonus = 0.0;
	strategy.GetImproveGrowthBonus(bonus);
	if(elem)
		bonus += elem->GetImproveGrowthBonus();
	goal.utility = bonus * terr_food_rank;

	// Same top/bottom-growth-cities-element logic as
	// ScoreProductionImprovement - see its comment for the reasoning.
	// Top and bottom both apply the same SmallCityGrowthBonus formula, per
	// the user's explicit call, even though "small city" doesn't quite
	// describe a top-ranked one - simplicity/parity with production won
	// out over the field's literal name.
	double top_value    = -1.0;
	double bottom_value = -1.0;
	if(elem && elem->GetGrowthCities() && elem->GetTop(top_value))
	{
		strategy.GetImproveSmallCityGrowthBonus(bonus);
		goal.utility += bonus * growth_rank;
	}
	else if(elem && elem->GetGrowthCities() && elem->GetBottom(bottom_value))
	{
		strategy.GetImproveSmallCityGrowthBonus(bonus);
		goal.utility += bonus * (1.0 - growth_rank);
	}
	else if(growth_rank < 0.2){
		strategy.GetImproveSmallCityGrowthBonus(bonus);
		goal.utility +=  bonus * (1.0 - growth_rank);
	}

	if(g_theWorld->IsGood(pos)){
		strategy.GetImproveGoodBonus(bonus);
		goal.utility += bonus;
	}

	return effect->GetBonusFood();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreProductionImprovement
//
// Description: Scores a production-category tile-improvement candidate -
//              the production-branch body of FindBestTileImprovement,
//              extracted verbatim. Picked purely by terrain capability
//              (the caller's own condition, terr_prod_rank > 0.2 or no
//              gold alternative), not by how much the city actually needs
//              production (see the session's tileimp_need_vs_terrain notes;
//              this branch used to be gated on a now-dead moreProdNeeded
//              stub instead).
//
// Parameters : goal:                        The candidate goal, type/utility set in place
//              pos:                         Position of the candidate tile
//              best_production_improvement: The chosen production improvement type
//              terr_prod_rank:              Tile's production yield relative to the map average
//              production_rank:             The city's production percentile
//              strategy:                    The city owner's current strategy
//              elem:                        The city's matching build-list-sequence element (may be NULL)
//
// Globals    : g_theTerrainImprovementDB, g_theWorld
//
// Returns    : sint32: the tile's added production from this improvement,
//              0 if best_production_improvement was unavailable
//
//----------------------------------------------------------------------------
sint32 Governor::ScoreProductionImprovement(TiGoal & goal, const MapPoint & pos, sint32 best_production_improvement, double terr_prod_rank, double production_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem) const
{
	if (best_production_improvement < 0)
		return 0;

	const TerrainImprovementRecord * rec = g_theTerrainImprovementDB->Get(best_production_improvement);
	const TerrainImprovementRecord::Effect * effect = terrainutil_GetTerrainEffect(rec, pos);

	goal.type = best_production_improvement;

	// 0.0 only silences -Wmaybe-uninitialized, see ScaleUtilityForCitySize.
	double bonus = 0.0;
	strategy.GetImproveProductionBonus(bonus);
	if(elem)
		bonus += elem->GetImproveProductionBonus();
	goal.utility =  bonus * terr_prod_rank;

	// production_rank > 0.8 alone used to gate this unconditionally - now
	// only a fallback for cities whose matched build-list element isn't a
	// dedicated production-ranked category (e.g. Default/AllCities). When
	// the city WAS specifically matched by a "top production cities" or
	// "bottom production cities" element (GetMatchingSequenceElement's own
	// GetTop()/GetBottom() checks, gated on GetProductionCities()), the
	// element match itself is already the need signal - apply the bonus
	// unconditionally rather than re-gating on a fixed rank threshold.
	// Top and bottom currently do the same thing; kept as separate
	// branches since they're conceptually distinct and may want to
	// diverge later (e.g. scaling bottom-ranked cities differently).
	double top_value    = -1.0;
	double bottom_value = -1.0;
	if(elem && elem->GetProductionCities() && elem->GetTop(top_value))
	{
		strategy.GetImproveLargeCityProductionBonus(bonus);
		goal.utility += bonus * production_rank;
	}
	else if(elem && elem->GetProductionCities() && elem->GetBottom(bottom_value))
	{
		strategy.GetImproveLargeCityProductionBonus(bonus);
		goal.utility += bonus * (1.0 - production_rank);
	}
	else if(production_rank > 0.8)
	{
		strategy.GetImproveLargeCityProductionBonus(bonus);
		goal.utility += bonus *	production_rank;
	}

	if(g_theWorld->IsGood(pos))
	{
		strategy.GetImproveGoodBonus(bonus);
		goal.utility += bonus;
	}

	return effect->GetBonusProduction();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreGoldImprovement
//
// Description: Scores a gold-category tile-improvement candidate - the
//              gold-branch body of FindBestTileImprovement, extracted
//              verbatim. Picked purely by availability (best_gold_improvement
//              >= 0, the caller's own condition) - unlike growth/production,
//              this branch has never had a need-check at all (the
//              commented-out gold_rank/GetCommerceRank in the caller was
//              never wired up either, see tileimp_need_vs_terrain). Its
//              large-city bonus reuses ImproveLargeCityProductionBonus -
//              there's no gold-specific equivalent field in strategy.cdb -
//              but is driven by gold_rank (the call site passes its own
//              GetCommerceRank() result); an earlier version of this
//              comment mislabeled the parameter itself as production_rank,
//              which it never actually was.
//
// Parameters : goal:                  The candidate goal, type/utility set in place
//              pos:                   Position of the candidate tile
//              best_gold_improvement: The chosen gold improvement type
//              terr_gold_rank:        Tile's gold yield relative to the map average
//              gold_rank:             The city's commerce/gold percentile
//              strategy:              The city owner's current strategy
//              elem:                  The city's matching build-list-sequence element (may be NULL)
//
// Globals    : g_theTerrainImprovementDB, g_theWorld
//
// Returns    : sint32: the tile's added gold from this improvement, 0 if
//              best_gold_improvement was unavailable
//
//----------------------------------------------------------------------------
sint32 Governor::ScoreGoldImprovement(TiGoal & goal, const MapPoint & pos, sint32 best_gold_improvement, double terr_gold_rank, double gold_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem) const
{
	if (best_gold_improvement < 0)
		return 0;

	const TerrainImprovementRecord * rec = g_theTerrainImprovementDB->Get(best_gold_improvement);
	const TerrainImprovementRecord::Effect * effect = terrainutil_GetTerrainEffect(rec, pos);

	goal.type = best_gold_improvement;

	double bonus = strategy.GetImproveGoldBonus();
	if(elem)
		bonus += elem->GetImproveGoldBonus();
	goal.utility = bonus * terr_gold_rank;

	// Same top/bottom-element logic as ScoreProductionImprovement, gated on
	// GetCommerceCities() (the gold-relevant category in
	// GetMatchingSequenceElement) rather than GetProductionCities().
	double top_value    = -1.0;
	double bottom_value = -1.0;
	if(elem && elem->GetCommerceCities() && elem->GetTop(top_value))
	{
		strategy.GetImproveLargeCityProductionBonus(bonus);
		goal.utility += bonus * gold_rank;
	}
	else if(elem && elem->GetCommerceCities() && elem->GetBottom(bottom_value))
	{
		strategy.GetImproveLargeCityProductionBonus(bonus);
		goal.utility += bonus * (1.0 - gold_rank);
	}
	else if(gold_rank > 0.8)
	{
		strategy.GetImproveLargeCityProductionBonus(bonus);
		goal.utility += bonus * gold_rank;
	}
	if(g_theWorld->IsGood(pos))
	{
		strategy.GetImproveGoodBonus(bonus);
		goal.utility += bonus;
	}

	return effect->GetBonusGold();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreFoodTerraform
//
// Description: Scores a food-terraform tile-improvement candidate (the
//              tile's terrain itself changes, e.g. Desert -> Grassland) -
//              the food-terraform-branch body of FindBestTileImprovement,
//              extracted verbatim. Picked purely by whether the tile is
//              already "Good" terrain and a food terraform exists
//              (the caller's own condition, moreFoodNeeded + !IsGood +
//              food_ter >= 0) - same terrain-capability-over-need pattern
//              as the other branches (see tileimp_need_vs_terrain), though
//              this one at least reuses NeedMoreFood's real need-check.
//
// Parameters : goal:           The candidate goal, type/utility set in place
//              pos:            Position of the candidate tile
//              food_ter:       The chosen food-terraform improvement type
//              terrain_type:   The tile's current terrain type
//              terr_food_rank: Tile's food yield relative to the map average
//              growth_rank:    The city's growth percentile
//              strategy:       The city owner's current strategy
//              elem:           The city's matching build-list-sequence element (may be NULL)
//
// Globals    : g_theTerrainImprovementDB, g_theWorld
//
// Returns    : sint32: the tile's added food from terraforming, 0 if
//              food_ter was unavailable/inapplicable
//
//----------------------------------------------------------------------------
sint32 Governor::ScoreFoodTerraform(TiGoal & goal, const MapPoint & pos, sint32 food_ter, sint32 terrain_type, double terr_food_rank, double growth_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem) const
{
	if (food_ter < 0 || g_theWorld->IsGood(pos))
		return 0;

	sint32 terrain;
	if (!g_theTerrainImprovementDB->Get(food_ter)->GetTerraformTerrainIndex(terrain))
		return 0;

	goal.type = food_ter;

	if(terrain_type != terrainutil_GetDead())
	{
		// 0.0 only silences -Wmaybe-uninitialized, see ScaleUtilityForCitySize.
		double bonus = 0.0;
		strategy.GetImproveGrowthBonus(bonus);
		if(elem)
			bonus += elem->GetImproveGrowthBonus();
		goal.utility = bonus * terr_food_rank;

		// Same top/bottom-growth-cities-element logic as
		// ScoreGrowthImprovement - see its comment for the reasoning.
		double top_value    = -1.0;
		double bottom_value = -1.0;
		if(elem && elem->GetGrowthCities() && elem->GetTop(top_value))
		{
			strategy.GetImproveSmallCityGrowthBonus(bonus);
			goal.utility += bonus * growth_rank;
		}
		else if(elem && elem->GetGrowthCities() && elem->GetBottom(bottom_value))
		{
			strategy.GetImproveSmallCityGrowthBonus(bonus);
			goal.utility += bonus * (1.0 - growth_rank);
		}
		else if(growth_rank < 0.2)
		{
			strategy.GetImproveSmallCityGrowthBonus(bonus);
			goal.utility +=  bonus * (1.0 - growth_rank);
		}
	}
	else
	{
		goal.utility = 9999.0;
	}

	return g_theWorld->GetCell(pos)->GetFoodFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetFoodFromTerrain();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreGoldTerraform
//
// Description: Scores a gold-terraform tile-improvement candidate - the
//              gold-terraform-branch body of FindBestTileImprovement,
//              extracted verbatim. Picked by GetNetCityGold() <= 0 (the
//              caller's own condition) - a real need-check, unlike the
//              plain gold branch, though a coarse one (any gold deficit
//              at all, not scaled by how much).
//
// Parameters : goal:            The candidate goal, type/utility set in place
//              pos:             Position of the candidate tile
//              gold_ter:        The chosen gold-terraform improvement type
//              terrain_type:    The tile's current terrain type
//              terr_gold_rank:  Tile's gold yield relative to the map average
//              gold_rank:       The city's commerce/gold percentile
//              strategy:        The city owner's current strategy
//              elem:            The city's matching build-list-sequence element (may be NULL)
//
// Globals    : g_theTerrainImprovementDB, g_theWorld
//
// Returns    : sint32: the tile's added gold from terraforming, 0 if
//              gold_ter was unavailable/inapplicable
//
//----------------------------------------------------------------------------
sint32 Governor::ScoreGoldTerraform(TiGoal & goal, const MapPoint & pos, sint32 gold_ter, sint32 terrain_type, double terr_gold_rank, double gold_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem) const
{
	if (gold_ter < 0 || g_theWorld->IsGood(pos))
		return 0;

	sint32 terrain;
	if (!g_theTerrainImprovementDB->Get(gold_ter)->GetTerraformTerrainIndex(terrain))
		return 0;

	goal.type = gold_ter;

	if(terrain_type != terrainutil_GetDead())
	{
		double bonus = strategy.GetImproveGoldBonus();
		if(elem)
			bonus += elem->GetImproveGoldBonus();
		goal.utility = bonus * terr_gold_rank;

		// Same top/bottom-element logic as ScoreGoldImprovement - see its
		// comment for the reasoning.
		double top_value    = -1.0;
		double bottom_value = -1.0;
		if(elem && elem->GetCommerceCities() && elem->GetTop(top_value))
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * gold_rank;
		}
		else if(elem && elem->GetCommerceCities() && elem->GetBottom(bottom_value))
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * (1.0 - gold_rank);
		}
		else if(gold_rank > 0.8)
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * gold_rank;
		}
	}
	else
	{
		goal.utility = 9999.0;
	}

	return g_theWorld->GetCell(pos)->GetGoldFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetGoldFromTerrain();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreProductionTerraform
//
// Description: Scores a production-terraform tile-improvement candidate -
//              the production counterpart to ScoreFoodTerraform/
//              ScoreGoldTerraform, mirroring ScoreProductionImprovement's
//              bonus formula. NOT WIRED IN YET: unlike food_ter/gold_ter,
//              prod_ter (from GetBestTerraformImprovement) currently has
//              no dedicated branch of its own in FindBestTileImprovement -
//              it's only ever read in the Dead-tile reclamation fallback
//              (flat 9999.0, no formula). Added ahead of wiring it in as
//              part of the ongoing need-vs-terrain refactor.
//
// Parameters : goal:            The candidate goal, type/utility set in place
//              pos:             Position of the candidate tile
//              prod_ter:        The chosen production-terraform improvement type
//              terrain_type:    The tile's current terrain type
//              terr_prod_rank:  Tile's production yield relative to the map average
//              production_rank: The city's production percentile
//              strategy:        The city owner's current strategy
//              elem:            The city's matching build-list-sequence element (may be NULL)
//
// Globals    : g_theTerrainImprovementDB, g_theWorld
//
// Returns    : sint32: the tile's added production from terraforming, 0 if
//              prod_ter was unavailable/inapplicable
//
//----------------------------------------------------------------------------
sint32 Governor::ScoreProductionTerraform(TiGoal & goal, const MapPoint & pos, sint32 prod_ter, sint32 terrain_type, double terr_prod_rank, double production_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem) const
{
	if (prod_ter < 0 || g_theWorld->IsGood(pos))
		return 0;

	sint32 terrain;
	if (!g_theTerrainImprovementDB->Get(prod_ter)->GetTerraformTerrainIndex(terrain))
		return 0;

	goal.type = prod_ter;

	if(terrain_type != terrainutil_GetDead())
	{
		// 0.0 only silences -Wmaybe-uninitialized, see ScaleUtilityForCitySize.
		double bonus = 0.0;
		strategy.GetImproveProductionBonus(bonus);
		if(elem)
			bonus += elem->GetImproveProductionBonus();
		goal.utility = bonus * terr_prod_rank;

		// Same top/bottom-production-cities-element logic as
		// ScoreProductionImprovement - see its comment for the reasoning.
		double top_value    = -1.0;
		double bottom_value = -1.0;
		if(elem && elem->GetProductionCities() && elem->GetTop(top_value))
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * production_rank;
		}
		else if(elem && elem->GetProductionCities() && elem->GetBottom(bottom_value))
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * (1.0 - production_rank);
		}
		else if(production_rank > 0.8)
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * production_rank;
		}
	}
	else
	{
		goal.utility = 9999.0;
	}

	return g_theWorld->GetCell(pos)->GetShieldsFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetShieldsFromTerrain();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScoreFallbackTerraform
//
// Description: Last-resort tail of FindBestTileImprovement's terrain-driven
//              chain, extracted verbatim - only reached (goal.type < 0 at
//              the call site) when nothing else claimed the tile. Two
//              unrelated sub-cases: reclaiming a Dead tile (always wins,
//              flat 9999.0, cascading food/production/gold preference) and
//              cleaning up otherwise-unimprovable bad terrain (Glacier/
//              Swamp/Tundra -> Hills, Desert -> Grassland) that fell
//              through every other branch.
//
// Parameters : goal:            The candidate goal, type/utility set in place
//              pos:             Position of the candidate tile
//              terrain_type:    The tile's current terrain type
//              food_ter:        The chosen food-terraform improvement type (Dead-tile case)
//              prod_ter:        The chosen production-terraform improvement type (Dead-tile case)
//              gold_ter:        The chosen gold-terraform improvement type (Dead-tile case)
//              growth_rank:     The city's growth percentile
//              production_rank: The city's production percentile
//              strategy:        The city owner's current strategy
//              elem:            The city's matching build-list-sequence element (may be NULL)
//              player_ptr:      The city's owning player
//
// Globals    : g_theWorld
//
// Returns    : -
//
//----------------------------------------------------------------------------
void Governor::ScoreFallbackTerraform(TiGoal & goal, const MapPoint & pos, sint32 terrain_type, sint32 food_ter, sint32 prod_ter, sint32 gold_ter, double growth_rank, double production_rank, const StrategyRecord & strategy, const StrategyRecord::BuildListSequenceElement * elem, Player * player_ptr) const
{
	// Moved almost to the end, so that if no terraform improvement is
	// selected the dead tiles are removed.
	if(terrain_type == terrainutil_GetDead())
	{
		if(food_ter >= 0)
		{
			goal.type = food_ter;
			goal.utility = 9999.0;
		}
		else if(prod_ter >= 0)
		{
			goal.type = prod_ter;
			goal.utility = 9999.0;
		}
		else if(gold_ter >= 0)
		{
			goal.type = gold_ter;
			goal.utility = 9999.0;
		}
	}
	// Now at the end because otherwise it catches before the dead tiles
	else if(!g_theWorld->IsGood(pos))
	{ // Should be removed
		double bonus = 0.0;
		if(terrain_type == terrainutil_GetGlacier()
		|| terrain_type == terrainutil_GetSwamp()
		|| terrain_type == terrainutil_GetTundra()
		){
			if(player_ptr->CanCreateImprovement(terrainutil_GetTerraformHillsImprovement(), pos, false))
			{
				strategy.GetImproveProductionBonus(bonus);
				if(elem)
					bonus += elem->GetImproveProductionBonus();
				goal.utility =  bonus * (1.0-production_rank);
				goal.type = terrainutil_GetTerraformHillsImprovement();
			}
		}
		else if(terrain_type == terrainutil_GetDesert())
		{
			if(player_ptr->CanCreateImprovement(terrainutil_GetTerraformGrasslandImprovement(), pos, false)){
				strategy.GetImproveGrowthBonus(bonus);
				if(elem)
					bonus += elem->GetImproveGrowthBonus();
				goal.utility =  bonus * (1.0-growth_rank);
				goal.type = terrainutil_GetTerraformGrasslandImprovement();
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::FindBestTileImprovement
//
// Description: Determines the best tile improvement for a tile.
//
// Parameters : pos:             Position of the tile on the map
//
// Globals    : g_player:        List of players in the game
//              g_theWorld:      Map information
//
// Returns    : bool:            The tile can be improved
//              goal:            Type and priority value of the tile improvement
//
// Remark(s)  : The information in goal is only valid when true is returned.
//
//----------------------------------------------------------------------------
bool Governor::FindBestTileImprovement(const MapPoint &pos, TiGoal &goal) const
{
	const TerrainImprovementRecord *rec;
	const TerrainImprovementRecord::Effect *effect;

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	MapAnalysis & the_map =  MapAnalysis::GetMapAnalysis();

	Unit city_owner = g_theWorld->GetCell(pos)->GetCityOwner();
	CityData* city = city_owner.GetCityData();
	Assert(city);

	// The element governing this city's current build-list sequence (NULL
	// for a human-governed city) carries its own tile-improvement bonuses
	// on top of the strategy-wide ones below - lets a single strategy
	// fine-tune improvement choice per situation, not just per personality.
	const StrategyRecord::BuildListSequenceElement *elem = GetMatchingSequenceElement(city);
	bool hasElemPriority  = elem->GetImproveGrowthBonus()     != 0
	                     || elem->GetImproveProductionBonus() != 0
	                     || elem->GetImproveGoldBonus()       != 0;

	double growth_rank     = the_map.GetGrowthRank    (city);
	double production_rank = the_map.GetProductionRank(city);
	double gold_rank       = the_map.GetCommerceRank  (city);
	double terr_food_rank = (g_theWorld->GetCell(pos)->GetFoodFromTerrain()) /
		(double) World::GetAvgFoodFromTerrain();
	double terr_prod_rank = (g_theWorld->GetCell(pos)->GetShieldsFromTerrain()) /
		(double) World::GetAvgShieldsFromTerrain();
	double terr_gold_rank = (g_theWorld->GetCell(pos)->GetGoldFromTerrain()) /
		(double) World::GetAvgGoldFromTerrain();
	sint32 terrain_type = g_theWorld->GetCell(pos)->GetTerrainType();

	double terraform_food_rank = 0.0;
	double terraform_prod_rank = 0.0;
	double terraform_gold_rank = 0.0;

	sint32 best_growth_improvement;
	sint32 best_production_improvement;
	sint32 best_gold_improvement;
	GetBestFoodProdGoldImprovement(pos, best_growth_improvement, best_production_improvement, best_gold_improvement);

	goal.pos     = pos;
	goal.type    = -1;
	goal.utility = -1.0;

	sint32 food_ter = -1;
	sint32 prod_ter = -1;
	sint32 gold_ter = -1;

	if (!g_theWorld->IsGood(pos))
	{
		bool shouldTerraform = true;
		for
		(
			sint32 i = 0;
			i < g_theWorld->GetCell(pos)->GetNumBuiltImprovementTypes() && shouldTerraform;
			++i
		)
		{
			rec = g_theTerrainImprovementDB->Get(g_theWorld->GetCell(pos)->GetBuiltImprovementType(i));

			effect = terrainutil_GetTerrainEffect(rec, pos);
			if (effect)
			{
				shouldTerraform =  !effect->HasBonusFood()
				                && !effect->HasBonusProduction()
				                && !effect->HasBonusGold()
				                && !effect->GetEndgame();
			}
		}

		if (shouldTerraform)
		{
			GetBestTerraformImprovement(pos, food_ter, prod_ter, gold_ter, true);

			sint32 terrFood = -1;
			sint32 terrProd = -1;
			sint32 terrGold = -1;

			if(food_ter >= 0 && g_theTerrainImprovementDB->Get(food_ter)->GetTerraformTerrainIndex(terrFood))
			{
				terraform_food_rank = (g_theWorld->GetCell(pos)->GetFoodFromTerrain(terrFood)) /
					(double) World::GetAvgFoodFromTerrain();
			}
			if(prod_ter >= 0 && g_theTerrainImprovementDB->Get(prod_ter)->GetTerraformTerrainIndex(terrProd))
			{
				terraform_prod_rank = (g_theWorld->GetCell(pos)->GetShieldsFromTerrain(terrProd)) /
					(double) World::GetAvgShieldsFromTerrain();
			}
			if(gold_ter >= 0 && g_theTerrainImprovementDB->Get(gold_ter)->GetTerraformTerrainIndex(terrGold))
			{
				terraform_gold_rank = (g_theWorld->GetCell(pos)->GetGoldFromTerrain(terrGold)) /
					(double) World::GetAvgGoldFromTerrain();
			}
		}
	}

	sint32 foodMissing = 0;
	bool moreFoodNeeded = city->NeedMoreFood(0, foodMissing, true);
//	bool moreProdNeeded = city->NeedMoreProdOr(bonusCommerce, goldMissing, true);
	// No CityData equivalent of NeedMoreFood exists for gold - GetNetCityGold()
	// is the same per-city figure ScoreGoldTerraform below already gated on
	// ("<= 0 // Improve even at zero"), just named and reused here so the
	// improvement path can use the identical threshold.
	bool moreGoldNeeded = city->GetNetCityGold() <= 0;

	if(hasElemPriority)
	{
		// goal.utility was reset to -1.0 above alongside goal.type, so
		// every copy below starts as a clean "no candidate" TiGoal - a
		// category that turns out fully unavailable (both Score* calls
		// early-return without touching type/utility) can't outscore a
		// real one below with a stale value left over from a previous
		// tile (PlaceTileImprovements reuses one TiGoal across every
		// candidate tile in a city).
		TiGoal goalFood = goal;
		TiGoal goalProd = goal;
		TiGoal goalGold = goal;

		TiGoal goalFoodTerr = goal;
		sint32 bonusFoodImp = ScoreGrowthImprovement(goalFood,     pos, best_growth_improvement,      terr_food_rank, growth_rank, strategy, elem);
		sint32 bonusFoodTer = ScoreFoodTerraform    (goalFoodTerr, pos, food_ter,  terrain_type, terraform_food_rank, growth_rank, strategy, elem);
		if(bonusFoodImp < bonusFoodTer)
			goalFood = goalFoodTerr;

		// NeedMoreFood is a real need-check (unlike production/gold's dead
		// stubs, see tileimp_need_vs_terrain) - honour it as a hard
		// priority here too, same as the terrain-driven chain below: a
		// genuinely food-starved city takes the food candidate outright
		// rather than letting it get outbid by production/gold utility.
		if(moreFoodNeeded && goalFood.type >= 0)
		{
			goal = goalFood;
		}
		else
		{
			TiGoal goalProdTerr = goal;
			sint32 bonusProdImp = ScoreProductionImprovement(goalProd,     pos, best_production_improvement,      terr_prod_rank, production_rank, strategy, elem);
			sint32 bonusProdTer = ScoreProductionTerraform  (goalProdTerr, pos, prod_ter,      terrain_type, terraform_prod_rank, production_rank, strategy, elem);
			if(bonusProdImp < bonusProdTer)
				goalProd = goalProdTerr;

			TiGoal goalGoldTerr = goal;
			sint32 bonusGoldImp = ScoreGoldImprovement(goalGold,     pos, best_gold_improvement,                    terr_gold_rank, gold_rank, strategy, elem);
			sint32 bonusGoldTer = ScoreGoldTerraform  (goalGoldTerr, pos,              gold_ter, terrain_type, terraform_gold_rank, gold_rank, strategy, elem);
			if(bonusGoldImp < bonusGoldTer)
				goalGold = goalGoldTerr;

			goal = (goalFood.utility >= goalProd.utility) ? goalFood : goalProd;
			goal = (    goal.utility >= goalGold.utility) ? goal     : goalGold;
		}
	}
	else
	{
		// Each Score*() below now checks its own type's availability (e.g.
		// best_growth_improvement >= 0) internally, and is a no-op - goal.type
		// left at -1 - when unavailable. So these conditions are the need/
		// priority side only, deciding which category to *try* rather than
		// whether it's *possible*; the goal.type < 0 guards give the next
		// category a turn if the previous one turned out to have nothing
		// available, the same fallthrough the original if/else-if chain gave
		// implicitly by including the availability check in each condition
		// (see the session's tileimp_need_vs_terrain notes).
		if(moreFoodNeeded
		|| (best_production_improvement < 0
		&&  best_gold_improvement < 0)
		){
			ScoreGrowthImprovement(goal, pos, best_growth_improvement, terr_food_rank, growth_rank, strategy, elem);
		}

		if(goal.type < 0
		&& moreFoodNeeded
		){
			ScoreFoodTerraform(goal, pos, food_ter, terrain_type, terraform_food_rank, growth_rank, strategy, elem);
		}

		// Gold's own hard-priority need check, mirroring food's above -
		// tried before production gets a chance to grab the tile purely
		// on terrain rank, so a genuinely gold-starved city doesn't lose
		// the tile to production just because the terrain looks decent
		// for it. The unconditional gold attempt further below stays as
		// a fallback for cities that aren't gold-starved.
		if(goal.type < 0
		&& moreGoldNeeded
		){
			ScoreGoldImprovement(goal, pos, best_gold_improvement, terr_gold_rank, gold_rank, strategy, elem);
		}

	//	if(goal.type < 0 && (moreProdNeeded
	//	|| best_gold_improvement < 0))
		if(goal.type < 0
		&& (terr_prod_rank > 0.2
		||  best_gold_improvement < 0)
		){
			ScoreProductionImprovement(goal, pos, best_production_improvement, terr_prod_rank, production_rank, strategy, elem);
		}

		if(goal.type < 0 //(gold_rank > 0.4)
		){
			ScoreGoldImprovement(goal, pos, best_gold_improvement, terr_gold_rank, gold_rank, strategy, elem);
		}

		if(goal.type < 0
		&& moreGoldNeeded
		){
			ScoreGoldTerraform(goal, pos, gold_ter, terrain_type, terraform_gold_rank, gold_rank, strategy, elem);
		}

		if(goal.type < 0
		){
			ScoreProductionTerraform(goal, pos, prod_ter, terrain_type, terraform_prod_rank, production_rank, strategy, elem);
		}

		if(goal.type < 0)
		{
			ScoreFallbackTerraform(goal, pos, terrain_type, food_ter, prod_ter, gold_ter, growth_rank, production_rank, strategy, elem, player_ptr);
		}
	}

	return (goal.type >= 0);
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ScaleUtilityForCitySize
//
// Description: Final adjustment step for a gathered tile-improvement
//              candidate's utility, based on the owning city's size - not
//              its actual resource needs (see [[tileimp_need_vs_terrain]]
//              in the session's own notes). Two ring-related adjustments:
//              the original ring-distance preference (inner rings over
//              outer ones), and - moved in from PlaceTileImprovements, see
//              [[tileimp_placement_priority]] - a per-ring-per-city
//              saturation discount on top, of up to one ring-step's worth
//              of utility, proportional to how much of its own ring this
//              city has already improved, so one city's fully-saturated
//              ring can't keep winning a shared turn's budget over a
//              sibling city's still-empty same-numbered ring. The
//              saturation discount needs the whole city's ring tallies
//              finalized first, so - unlike before - this whole function
//              can no longer be called from inside FindBestTileImprovement
//              while that city's gathering loop is still running (the
//              tallies for the current/later rings aren't final yet);
//              PlaceTileImprovements now calls it once per gathered
//              candidate, after its own gathering loop (and so the
//              tallies) is complete.
//
// Parameters : goal:              The candidate goal, utility adjusted in place
//              city:              The tile's owning city (NULL clears utility)
//              pos:               Position of the candidate tile
//              terrain_type:      The tile's current terrain type
//              strategy:          The city owner's current strategy
//              ringImprovedCount: How many of this candidate's ring's eligible tiles this city already counts as improved
//              ringTotalCount:    How many of this candidate's ring's tiles are eligible at all
//
// Globals    : -
//
// Returns    : -
//
//----------------------------------------------------------------------------
void Governor::ScaleUtilityForCitySize(TiGoal & goal, CityData * city, const MapPoint & pos, sint32 terrain_type, const StrategyRecord & strategy, sint32 ringImprovedCount, sint32 ringTotalCount) const
{
#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
	// CTP1: utilisation depends on worker placement, and is either 0 or 1.
#else
	if ((goal.type >= 0) && city)
	{
		// Also cleanup if the city does not use the tile
		if(terrain_type != terrainutil_GetDead())
		{
#if defined(NEW_RESOURCE_PROCESS)
			sint32 ring = city->GetRing(pos);
			goal.utility *= static_cast<double>(city->GetWorkingPeopleInRing(ring)) / static_cast<double>(city->GetRingSize(ring));
#else
			double ringFactor = city->GetRingImprovementPriority(pos);

			// Soft, proportional discount on top, not a hard gate: a
			// ring that is e.g. half-improved only loses half of one
			// ring-step's worth of utility (GetRingImprovementPriorityFraction()
			// is the gap GetRingImprovementPriority() leaves between
			// two adjacent rings), so a fully-improved inner ring can
			// at most bring its own candidates down to parity with the
			// next ring out - it can never outweigh a utility
			// difference bigger than one ring-step, such as one of the
			// strategy's own Improve*Bonus additions applied earlier.
			if(ringTotalCount > 0)
			{
				double ringStillUnimproved =
				          static_cast<double>(ringImprovedCount) /
				          static_cast<double>(ringTotalCount);
				ringFactor -= ringStillUnimproved * city->GetRingImprovementPriorityFraction();
			}

			goal.utility *= ringFactor;
#endif
		}
	}
	else
	{
		goal.utility = 0.0;
	}
#endif
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetBestRoadImprovement
//
// Description: Determines the best road improvement for a tile.
//
// Parameters : pos: Position of the tile on the map
//
// Globals    : g_player:                  List of players in the game
//
// Returns    : sin32 an index into the terrain improvement database.
//
// Remark(s)  : Returns -1 if no buildable road improvement was found.
//
//----------------------------------------------------------------------------
sint32 Governor::GetBestRoadImprovement(const MapPoint & pos) const
{
	Cell const * cell = g_theWorld->GetCell(pos);
	if (cell->HasCity())
	{
		return -1;
	}

	TerrainImprovementRecord const * terr_imp_rec =
		terrainutil_GetBestRoad(m_playerId, pos);
	if (terr_imp_rec == NULL)
		return -1;

	Player *player_ptr = g_player[m_playerId];
	if (player_ptr && !player_ptr->CanCreateImprovement(terr_imp_rec->GetIndex(), pos, false))
	    return -1;

	sint32 const old_move_cost = static_cast<sint32>(cell->GetMoveCost());
	TerrainImprovementRecord::Effect const * effect =
	    terrainutil_GetTerrainEffect(terr_imp_rec, pos);

	sint32 cost;
	if (effect && effect->GetMoveCost(cost) && cost >= old_move_cost)
		return -1;

	return terr_imp_rec->GetIndex();
}

//----------------------------------------------------------------------------
//
// Name       : Governor::HasFoodProdOrGoldImprovement
//
// Description: Checks Whether the tile has a food, production, or gold
//              improvement finished or under construction.
//
// Parameters : pos: Position of the tile on the map
//
// Globals    : g_theWorld:                Map information
//              g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : Whether pos has a food, production, or gold improvement
//              finished or under construction.
//
//----------------------------------------------------------------------------
bool Governor::HasUnfinishedFoodProdOrGoldImprovement(const MapPoint & pos) const
{
	Cell const *  cell = g_theWorld->GetCell(pos);
	if (cell->HasCity()) // Do not find improvements for city tiles
		return false;

	for (sint32 index = 0; index < cell->GetNumUnfinishedImprovements(); index++)
	{
		sint32                                     type = cell->GetUnfinishedImprovement(index).GetType();
		const TerrainImprovementRecord *            rec = g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect * effect = terrainutil_GetTerrainEffect(rec, pos);

		if (effect == NULL)
			continue;

		if(effect->HasBonusFood())
			return true;

		if(effect->HasBonusProduction())
			return true;

		if(effect->HasBonusGold())
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::HasFoodProdOrGoldImprovement
//
// Description: Checks Whether the tile has a food, production, or gold
//              improvement finished or under construction.
//
// Parameters : pos: Position of the tile on the map
//
// Globals    : g_theWorld:                Map information
//              g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : Whether pos has a food, production, or gold improvement
//              finished or under construction.
//
//----------------------------------------------------------------------------
bool Governor::HasFoodProdOrGoldImprovement(const MapPoint & pos) const
{
	Cell const *  cell = g_theWorld->GetCell(pos);
	if (cell->HasCity()) // Do not find improvements for city tiles
		return false;

	if(HasUnfinishedFoodProdOrGoldImprovement(pos))
		return true;

	for (sint32 index = 0; index < cell->GetNumBuiltImprovementTypes(); index++)
	{
		sint32                                     type = cell->GetBuiltImprovementType(index);
		const TerrainImprovementRecord *            rec = g_theTerrainImprovementDB->Get(type);
		const TerrainImprovementRecord::Effect * effect = terrainutil_GetTerrainEffect(rec, pos);

		if (effect == NULL)
			continue;

		if(effect->HasBonusFood())
			return true;

		if(effect->HasBonusProduction())
			return true;

		if(effect->HasBonusGold())
			return true;
	}

	return false;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetBestFoodProdGoldImprovement
//
// Description: Determines the best food, production and gold improvement
//              for a tile.
//
// Parameters : pos: Position of the tile on the map
//
// Globals    : g_player:                  List of players in the game
//              g_theWorld:                Map information
//              g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : food_imp: Best food tile improvement
//              prod_imp: Best production tile improvement
//              gold_imp: Best gold tile improvement
//
// Remark(s)  : The return values are filled with -1 if there is no according
//              tile improvement.
//
//----------------------------------------------------------------------------
void Governor::GetBestFoodProdGoldImprovement(const MapPoint & pos, sint32 & food_imp, sint32 & prod_imp, sint32 & gold_imp) const
{
	food_imp = -1;
	prod_imp = -1;
	gold_imp = -1;

	Cell const *  cell = g_theWorld->GetCell(pos);
	if (cell->HasCity()) // Do not find improvements for city tiles
		return;

	sint32 type;
	sint32 tmp_bonus = 0;
	const TerrainImprovementRecord *rec;
	const TerrainImprovementRecord::Effect *effect;

	sint32 max_bonus_food = 0;
	sint32 max_bonus_prod = 0;
	sint32 max_bonus_gold = 0;
	uint32 current_class = 0x0;
	for (sint32 index = 0; index < cell->GetNumBuiltImprovementTypes(); index++)
	{
		type = cell->GetBuiltImprovementType(index);
		rec = g_theTerrainImprovementDB->Get(type);
		effect = terrainutil_GetTerrainEffect(rec, pos);

		if (effect == NULL)
			continue;

		if (!rec->GetClassRoad() &&
			!rec->GetClassOceanRoad() &&
			!rec->GetClassLandDetector() &&
			!rec->GetClassOceanDetector() &&
			!rec->GetClassStructure1() && !rec->GetClassStructure2())
		{
			current_class |= rec->GetClass();
		}

		tmp_bonus = effect->GetBonusFood();
		if(tmp_bonus > max_bonus_food)
		{
			max_bonus_food = tmp_bonus;
		}

		tmp_bonus = effect->GetBonusProduction();
		if(tmp_bonus > max_bonus_prod)
		{
			max_bonus_prod = tmp_bonus;
		}

		tmp_bonus = effect->GetBonusGold();
		if(tmp_bonus > max_bonus_gold)
		{
			max_bonus_gold = tmp_bonus;
		}
	}

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];

	const TERRAIN_TYPES terrain_type = g_theWorld->GetTerrainType(pos);

	for (type = 0; type < g_theTerrainImprovementDB->NumRecords(); type++)
	{
		rec = g_theTerrainImprovementDB->Get(type);

		if (!player_ptr->CanCreateImprovement(type, pos, false))
			continue;

		if (current_class != 0x0 && ((rec->GetClass() & current_class) == 0x0))
			continue;

		effect = terrainutil_GetTerrainEffect(rec, terrain_type);
		if(effect)
		{
			tmp_bonus = effect->GetBonusFood();
			if(tmp_bonus > max_bonus_food)
			{
				food_imp = type;
				max_bonus_food = tmp_bonus;
			}

			tmp_bonus = effect->GetBonusProduction();
			if(tmp_bonus > max_bonus_prod)
			{
				prod_imp = type;
				max_bonus_prod = tmp_bonus;
			}

			tmp_bonus = effect->GetBonusGold();
			if(tmp_bonus > max_bonus_gold)
			{
				gold_imp = type;
				max_bonus_gold = tmp_bonus;
			}
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetBestTerraformImprovement
//
// Description: Determines the best terraform improvement for a tile.
//
// Parameters : pos:        Position of the tile on the map
//              pwPerBonus: Whether the PW costs should be taken into account
//
// Globals    : g_player:                  List of players in the game
//              g_theWorld:                Map information
//              g_theTerrainDB:            The terrain databse
//              g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : food_imp: Best terraform food improvement
//              prod_imp: Best terraform production improvement
//              gold_imp: Best terraform gold improvement
//
// Remark(s)  : The return values are filled with -1 if there is no better
//              terrain then the current one.
//
//----------------------------------------------------------------------------
void Governor::GetBestTerraformImprovement(const MapPoint & pos, sint32 & food_imp, sint32 & prod_imp, sint32 & gold_imp, bool pwPerBonus) const
{
	food_imp = -1;
	prod_imp = -1;
	gold_imp = -1;

	Cell const * cell = g_theWorld->GetCell(pos);
	if (cell->HasCity()) // Do not terraform city tiles
		return;

	sint32 food_ter = -1;
	sint32 prod_ter = -1;
	sint32 gold_ter = -1;

	double max_bonus_food = 0.0;
	double max_bonus_prod = 0.0;
	double max_bonus_gold = 0.0;

	double tmp_bonus_food;
	double tmp_bonus_prod;
	double tmp_bonus_gold;

	const TerrainRecord *fromRec = g_theTerrainDB->Get(cell->GetTerrain());
	const TerrainRecord *toRec;

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];

	if(!player_ptr->HasAdvance(fromRec->GetRemoveAdvanceIndex()))
		return;

	double pwCosts;
	sint32 i;
	for(i = 0; i < g_theTerrainDB->NumRecords(); ++i){
		toRec = g_theTerrainDB->Get(i);

		if(!player_ptr->HasAdvance(toRec->GetAddAdvanceIndex()))
			continue;

		if(!fromRec->HasTransformRemove()
		|| !toRec->HasTransformAdd())
			continue;

		tmp_bonus_food = static_cast<double>(cell->GetFoodFromTerrain((sint8)i) - cell->GetFoodFromTerrain());
		tmp_bonus_prod = static_cast<double>(cell->GetShieldsFromTerrain((sint8)i) - cell->GetShieldsFromTerrain());
		tmp_bonus_gold = static_cast<double>(cell->GetGoldFromTerrain((sint8)i) - cell->GetGoldFromTerrain());

		if(pwPerBonus){
			pwCosts = static_cast<double>(fromRec->GetTransformRemovePtr()->GetMaterials()
			                              + toRec->GetTransformAddPtr()->GetMaterials());

			if(pwCosts == 0)pwCosts = 0.0000001; //Avoid division by zero.

			tmp_bonus_food /= pwCosts;
			tmp_bonus_prod /= pwCosts;
			tmp_bonus_gold /= pwCosts;
		}

		if(tmp_bonus_food > 0
		&& tmp_bonus_food > max_bonus_food
		){
			max_bonus_food = tmp_bonus_food;
			food_ter = i;
		}

		if(tmp_bonus_prod > 0
		&& tmp_bonus_prod > max_bonus_prod
		){
			max_bonus_prod = tmp_bonus_prod;
			prod_ter = i;
		}

		if(tmp_bonus_gold > 0
		&& tmp_bonus_gold > max_bonus_gold
		){
			max_bonus_gold = tmp_bonus_gold;
			gold_ter = i;
		}
	}

	const TerrainImprovementRecord *rec;
	sint32 ter;
	for(i = 0; i < g_theTerrainImprovementDB->NumRecords(); ++i)
	{
		rec = g_theTerrainImprovementDB->Get(i);

		if(!rec->GetTerraformTerrainIndex(ter))
			continue;

		if(food_ter == ter){
			food_imp = i;
		}
		if(prod_ter == ter){
			prod_imp = i;
		}
		if(gold_ter == ter){
			gold_imp = i;
		}
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::AssignPopulations
//
// Description: Assigns specialists in all cities
//
// Parameters : -
//
// Globals    : g_player: List of players
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void Governor::AssignPopulations()
{
	Assert(g_player[m_playerId]);

	g_player[m_playerId]->PreResourceCalculation();

	bool hasAllAdvances = g_player[m_playerId]->m_advances->HasAllAdvances();

	UnitDynamicArray * city_list = g_player[m_playerId]->GetAllCitiesList();
	for (sint32 i = 0; i < city_list->Num(); i++) // city_list->Num() returns a sint32
	{
		CityData * city = city_list->Access(i)->GetCityData();

		if (city && city->GetUseGovernor() && (city->GetSizeIndex() > 0))
		{
			AssignPopulation(city, hasAllAdvances);
		}
	}
}

const StrategyRecord::PopAssignmentElement *Governor::GetMatchingPopAssignment(const CityData *city) const
{
	Assert(city);
	Assert(g_player[m_playerId]);

	const StrategyRecord & strategy =
	           Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const StrategyRecord::PopAssignmentElement *elem = NULL;

	for (sint32 i = 0; i < strategy.GetNumPopAssignmentElement(); ++i)
	{
		elem = strategy.GetPopAssignmentElement(i);

		double rank;
		if (elem->GetProductionCities())
		{
			rank = MapAnalysis::GetMapAnalysis().GetProductionRank(city);
		}
		else if (elem->GetGrowthCities())
		{
			rank = MapAnalysis::GetMapAnalysis().GetGrowthRank(city);
		}
		else if (elem->GetGoldCities())
		{
			rank = MapAnalysis::GetMapAnalysis().GetCommerceRank(city);
		}
		else if (elem->GetDefault())
		{
			// Always OK, regardless of rank
			return elem;
		}
		else
		{
			// Try next one
			continue;
		}

		// Test whether the element is applicable for the current rank.
		// The 1.0 / 0.0 seeds only silence -Wmaybe-uninitialized (GetBottom
		// / GetTop write the out-param whenever they return true, so the
		// seed is never read); they're picked so the guarded comparison
		// comes out false, not true, if that ever stopped holding: rank is
		// in [0.0, 1.0], so bottom_value < rank and rank < 1.0 - top_value
		// are both unreachable. (GetMatchingSequenceElement uses -1.0 for
		// both, because its comparisons run the other way round.)
		double bottom_value = 1.0;
		double top_value    = 0.0;
		if (    elem->GetBottom(bottom_value) && (bottom_value < rank)
			 && elem->GetTop(top_value)       && (rank < (1.0 - top_value))
		   )
		{
			return elem;
		}
	}

	Assert(false);
	/// @todo Check return. Maybe return NULL instead of the rejected
	///       GetPopAssignmentElement(GetNumPopAssignmentElement()-1).
	return elem;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::AssignPopulation
//
// Description: Assigns the specialists
//
// Parameters : city: The city data of city for which the population should
//                    be assigned.
//
// Globals    : g_theCitySizeDB: The city size database.
//
// Returns    : -
//
// Remark(s)  : Redesigned to allow better specialist assignment however
//              this isn't finished, yet. The AI can now grow cities in
//              the deserts. But it cannot use merchants to turn a gold
//              deficit into a gold surplus. Whether more production, gold
//              or science is needed must still be added.
//
//----------------------------------------------------------------------------
void Governor::AssignPopulation(CityData *city, bool hasAllAdvances) const
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
	t1.start();
#endif

	StrategyRecord::PopAssignmentElement const * pop_assignment =
		GetMatchingPopAssignment(city);
	if (!pop_assignment)
	{
		return;
	}

	// Reset all specialists to workers
	city->ChangeSpecialists(POP_ENTERTAINER, -city->EntertainerCount());
	city->ChangeSpecialists(POP_FARMER,      -city->FarmerCount());
	city->ChangeSpecialists(POP_LABORER,     -city->LaborerCount());
	city->ChangeSpecialists(POP_MERCHANT,    -city->MerchantCount());
	city->ChangeSpecialists(POP_SCIENTIST,   -city->ScientistCount());

	//////////////////////////////////////////////////////////////////////
	// Not a pretty good idea to redo all the stuff, but to avoid this
	// the resources from the inner ring and the last ring must be
	// stored at seperated places to use them in the HowMuchMoreFoodNeeded
	// method.
#if !defined(NEW_RESOURCE_PROCESS)
	city->CollectResourcesFinally();
	city->ProcessFood();
	city->EatFood();
#endif

	//////////////////////////////////////////////////
	// Recalculate Happiness after specialists removal
	city->CalcHappiness(false);

	/////////////////////////////////////
	// Get maximum percent of specialists
	// Should be removed in the end.
	double specialists_percent = pop_assignment->GetSpecialists();

	///////////////////////////////////////////////////////////////
	// Get the amount of workers needed for base resources supply.
	sint32 farmers, laborers, merchants, scientists,
	       minFood, minProd, minGold, minScie;
	double farmersEff, laborersEff, merchantsEff, scientistsEff;
	sint32 min_workers = ComputeMinimumWorkers(city,
	                     farmers, laborers, merchants, scientists,
#if defined(NEW_RESOURCE_PROCESS)
	                     minFood, minProd, minGold, minScie);
#else
	                     minFood, minProd, minGold, minScie,
	                     farmersEff, laborersEff, merchantsEff, scientistsEff);
#endif

	sint32 size_index = city->GetSizeIndex();
//	sint32 size_index, full_index, part_index;
//	city->ComputeSizeIndexes(city->PopCount(), size_index, full_index, part_index);

	//////////////////////////////////////////////////////////////////////////
	// Get for that city the maximum number of workers that could be assigned.
#if defined(NEW_RESOURCE_PROCESS)
	sint32 minSpecialists = city->GetUnemployedPeople();
	sint32 max_workers = city->TilesForWorking();
	// Maybe fixing max_workers to the actual pop size, but the old way doesn't do it either.
#else
	sint32 max_workers = g_theCitySizeDB->Get(size_index)->GetMaxWorkers();
	sint32 minSpecialists = 0;

	if(max_workers < city->WorkerCount() + city->SlaveCount()){
		minSpecialists = (city->WorkerCount() + city->SlaveCount()) - max_workers;
	}
#endif

	min_workers -= city->SlaveCount();
	if(min_workers < 0) min_workers = 0;

	///////////////////////////////////////////////////
	// How many additional specialists should we have?
	sint32 specialists = static_cast<sint32>
		(floor(
		(city->PopCount() - (min_workers + city->SlaveCount())
		) * specialists_percent));

	///////////////////////////////////////////////////////////////
	// Cut down the number if we would assign too many specialists.
	specialists = (specialists <= max_workers) ? specialists : max_workers;
	specialists += minSpecialists;

	////////////////////////////////////////
	// Start with the specialist assignment:

	sint32 best_specialist = city->GetBestSpecialist(POP_FARMER);
	sint32 count           = 0;

#if defined(NEW_RESOURCE_PROCESS)
	sint32 foodMissing = city->HowMuchMoreFoodNeeded(0, false, true);
	if(foodMissing > 0)
	{
		sint32 i;
		sint32 workers;
		sint32 tmpCount;
		for(i = size_index; i >= 0; --i)
		{
			farmersEff = city->GetFarmersEffect(i);
			if(farmersEff)
			{
				tmpCount = static_cast<sint32>(ceil(static_cast<double>(foodMissing)/farmersEff));
				workers = city->GetWorkingPeopleInRing(i);
				if(tmpCount > workers)
				{
					count += workers;
				}
				else
				{
					count += tmpCount;
					break;
				}
			}
			else
			{
				break;
			}
		}
#else
	/////////////////////////////////////////////////////////////
	// Create a copy of city data for effect comparision.
	// Copy should be removed in the end.
	CityData tmp_city(city);
	sint32 delta;
	double prev_result;

	best_specialist = city->GetBestSpecialist(POP_LABORER);

	count = static_cast<sint32>(ceil(specialists * pop_assignment->GetLaborerPercent()));
	count = (count <= laborers) ? count : laborers;
	if(count > city->WorkerCount()){
		count = city->WorkerCount();
	}

	//////////////////////////////////
	// First pop database index is 0
	if((best_specialist >= 0)
	&& (count > 0)
	){
#if defined(NEW_RESOURCE_PROCESS)
		city->ChangeSpecialists(POP_LABORER, count);
#else
		tmp_city.CollectResourcesFinally();
		tmp_city.ProcessProduction(true);
		prev_result = tmp_city.GetGrossCityProduction();

		tmp_city.ChangeSpecialists(POP_LABORER, count);
		tmp_city.CollectResourcesFinally();
		tmp_city.ProcessProduction(true);

		if(tmp_city.GetGrossCityProduction() > prev_result)
		{
			city->ChangeSpecialists(POP_LABORER, count);
			city->ProcessProduction(true);
		}
		else if(tmp_city.GetGrossCityProduction() < prev_result)
		{
			delta = (-city->LaborerCount());
			city->ChangeSpecialists(POP_LABORER, delta);
		}

		delta = city->LaborerCount() - tmp_city.LaborerCount();
		tmp_city.ChangeSpecialists(POP_LABORER, delta );
#endif
	}

	sint32 foodMissing = city->HowMuchMoreFoodNeeded(0, false, true);
	if(foodMissing > 0
	&& farmersEff > 0
	){
		count = static_cast<sint32>(ceil(static_cast<double>(foodMissing)/farmersEff));
#endif
		// Remove debug messages when final debug is done
//		DPRINTF(k_DBG_GAMESTATE, ("foodMissing: %i\n", foodMissing));
//		DPRINTF(k_DBG_GAMESTATE, ("farmersEff: %f\n", farmersEff));
//		DPRINTF(k_DBG_GAMESTATE, ("count: %i\n", count));
	}
	else
	{
		count = static_cast<sint32> // Have to reconsider this
			(ceil(specialists * pop_assignment->GetFarmerPercent()));
	}
	if(count > city->WorkerCount())
	{
		count = city->WorkerCount();
	}
//	DPRINTF(k_DBG_GAMESTATE, ("CityName: %s\n", city->GetName()));
//	DPRINTF(k_DBG_GAMESTATE, ("PopSize: %i\n", city->PopCount()));
//	DPRINTF(k_DBG_GAMESTATE, ("count: %i\n", count));
//	DPRINTF(k_DBG_GAMESTATE, ("FoodMissing: %i\n", foodMissing));

	count = (count <= farmers) ? count : farmers;
//	DPRINTF(k_DBG_GAMESTATE, ("Farmers: %i\n", city->FarmerCount()));
//	DPRINTF(k_DBG_GAMESTATE, ("count: %i\n", count));
//	DPRINTF(k_DBG_GAMESTATE, ("specialists: %i\n", specialists));

	//////////////////////////////////
	// First pop database index is 0
	if((best_specialist >= 0)
	&& (count > 0)
	){
#if defined(NEW_RESOURCE_PROCESS)
		city->ChangeSpecialists(POP_FARMER, count);
#else
		// Test situation without specialists
		tmp_city.CollectResources();
		tmp_city.ProcessFood();
		prev_result = tmp_city.GetProducedFood();
//		DPRINTF(k_DBG_GAMESTATE, ("PrevResult: %f\n", prev_result));

		// Test situation with specialists
		tmp_city.ChangeSpecialists(POP_FARMER, count);
		tmp_city.CollectResourcesFinally();
		tmp_city.ProcessFood();

		if(tmp_city.GetProducedFood() > prev_result)
		{
			// The specialists are beneficial: employ in the real city.
			city->ChangeSpecialists(POP_FARMER, count);
		}
		else if(tmp_city.GetProducedFood() < prev_result)
		{
			// The specialists are producing less than workers: fire them all?
			// This should not do anything, because there should be none left
			// after the reset at the start.
			Assert(0 == city->FarmerCount());
			delta = (-city->FarmerCount());
			city->ChangeSpecialists(POP_FARMER, delta);
		}
//		DPRINTF(k_DBG_GAMESTATE, ("NewResult: %f\n", tmp_city->GetProducedFood()));

		// Synchronise the tmp_city with the real city.
		delta = city->FarmerCount() - tmp_city.FarmerCount();
		tmp_city.ChangeSpecialists(POP_FARMER, delta );
#endif
	}
//	DPRINTF(k_DBG_GAMESTATE, ("Farmers: %i\n", city->FarmerCount()));

	best_specialist = city->GetBestSpecialist(POP_MERCHANT);

	count = static_cast<sint32>
		(ceil(specialists * pop_assignment->GetMerchantPercent()));
	count = (count <= merchants) ? count : merchants;
	if(count > city->WorkerCount())
	{
		count = city->WorkerCount();
	}

	//////////////////////////////////
	// First pop database index is 0
	if((best_specialist >= 0)
	&& (count > 0)
	){
#if defined(NEW_RESOURCE_PROCESS)
		city->ChangeSpecialists(POP_MERCHANT, count);
#else
		tmp_city.CollectResourcesFinally();
		tmp_city.CollectOtherGold(TRUE);
		prev_result = tmp_city.GetGrossCityGold();

		tmp_city.ChangeSpecialists(POP_MERCHANT, count);
		tmp_city.CollectResourcesFinally();
		tmp_city.CollectOtherGold(TRUE);

		if(tmp_city.GetGrossCityGold() > prev_result)
		{
			city->ChangeSpecialists(POP_MERCHANT, count);
		}
		else if(tmp_city.GetGrossCityGold() < prev_result)
		{
			delta = (-city->MerchantCount());
			city->ChangeSpecialists(POP_MERCHANT, delta);
		}

		delta = city->MerchantCount() - tmp_city.MerchantCount();
		tmp_city.ChangeSpecialists(POP_MERCHANT, delta );
#endif
	}

	best_specialist = city->GetBestSpecialist(POP_SCIENTIST);

	count = static_cast<sint32>
		(ceil(specialists * pop_assignment->GetScientistPercent()));

	count = (count <= scientists) ? count : scientists;
	if(count > city->WorkerCount())
	{
		count = city->WorkerCount();
	}

	if(hasAllAdvances)
	{
		count = 0;
	}

	//////////////////////////////////
	// First pop database index is 0
	if(best_specialist >= 0
	&& count > 0
	){
		city->ChangeSpecialists(POP_SCIENTIST, count);
	}

	/////////////////////////
	// Recalculate happiness
	city->CollectResourcesFinally();
	city->ProcessProduction(true);   // Recalcs also happiness, since production influences happiness

	sint32 entertainer_delta = AssignEntertainers(city, false);
	sint32 needed            = g_theConstDB->Get(0)->GetRiotLevel();
	sint32 current           = static_cast<sint32>(city->GetHappiness());

	// Overwrite with the accepted minimum happiness from strategy.txt, if there is such
	// a value, otherwise it uses the riot level from const.txt.
	Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy().GetMinimumHappiness(needed);


	while (entertainer_delta != 0 && current > needed)
	{
		entertainer_delta = AssignEntertainers(city, true);
		current           = static_cast<sint32>(city->GetHappiness());
	}

	if (current < needed && city->WorkerCount() > 0)
	{
		// In that case we removed one entertainer to many.
		city->ChangeSpecialists(POP_ENTERTAINER, 1);
	}

	if(max_workers < city->WorkerCount() + city->SlaveCount())
	{
		minSpecialists = (city->WorkerCount() + city->SlaveCount()) - max_workers;
		best_specialist = city->GetBestSpecialist(POP_SCIENTIST);
		if(best_specialist >= 0)
		{
			city->ChangeSpecialists(POP_SCIENTIST, minSpecialists);
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	t1.stop();
	DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  Elapsed time for popasign (sub)= %f ms (%s)\n", t1.getElapsedTimeInMilliSec(), city->GetName()));
#endif
}

sint32 Governor::AssignEntertainers(CityData *city, bool decreaseByOne) const
{
	sint32 best_specialist = city->GetBestSpecialist(POP_ENTERTAINER);
	if(best_specialist < 0)
		return 0;

	sint32 count = 0;

	/////////////////////////
	// Recalculate happiness
	// Needs to be done before calling this function, either by calling this
	// explicitly or by a previous call of this function
	//city->CollectResourcesFinally();
	//city->ProcessProduction(true);   // Recalcs also happiness, since production influences happiness

	////////////////////////////////////////////////////////////////////////////
	// Get the minimum of additional entertainers to keep the city from rioing.
	// Get the maximum of additional entertainers needed for optimal happiness.
	sint32 min_entertainers = ComputeMinEntertainers(city);

	if(decreaseByOne && min_entertainers != 0)
	{
		min_entertainers = -1;
	}

	if(min_entertainers > city->WorkerCount())
	{
		count = min_entertainers - city->WorkerCount();
		count = (count <= city->LaborerCount()) ? count : city->LaborerCount();
		city->ChangeSpecialists(POP_LABORER, -count);
		// Happiness doesn't need to be recalculated, pops are used for entertainers
	}
	if(min_entertainers > city->WorkerCount())
	{
		count = min_entertainers - city->WorkerCount();
		count = (count <= city->ScientistCount()) ? count : city->ScientistCount();
		city->ChangeSpecialists(POP_SCIENTIST, -count);
	}
	if(min_entertainers > city->WorkerCount())
	{
		count = min_entertainers - city->WorkerCount();
		count = (count <= city->MerchantCount()) ? count : city->MerchantCount();
		city->ChangeSpecialists(POP_MERCHANT, -count);
	}
	if(min_entertainers > city->WorkerCount())
	{
		min_entertainers = city->WorkerCount();
	}
	if(city->SpecialistCount(POP_ENTERTAINER) + min_entertainers < 0)
	{
		min_entertainers = -city->SpecialistCount(POP_ENTERTAINER);
	}

	city->ChangeSpecialists(POP_ENTERTAINER, min_entertainers);

	/////////////////////////
	// Recalculate happiness
	city->CollectResourcesFinally();
	city->ProcessProduction(true);   // Recalcs also happiness, since production influences happiness

	return min_entertainers;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeMinEntertainers
//
// Description: Computes the additionally needed entertainers
//              to get the city happiness above riot level.
//
// Parameters : city:      The city data of city for which the amount of
//                         entertainers should be estimated.
//
// Globals    : g_theConstDB: The const database.
//
// Returns    : The additional entertainers needed if positive
//              otherwise the number of the superflous entertaners.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Governor::ComputeMinEntertainers(const CityData *city) const
{
	sint32 entertainer_type = city->GetBestSpecialist(POP_ENTERTAINER);

	if (entertainer_type < 0)
		return 0;

	sint32 per_pop_happiness = g_thePopDB->Get(entertainer_type)->GetHappiness();
	if (per_pop_happiness <= 0)
	{
		DPRINTF(k_DBG_GAMESTATE,
		        ("Entertainer pop type: %i, happiness: %i\n", entertainer_type, per_pop_happiness)
		       );
		Assert(0);
		return 0;
	}

	sint32 needed  = g_theConstDB->Get(0)->GetRiotLevel();
	sint32 current = static_cast<sint32>(city->GetHappiness());

	// Overwrite with the accepted minimum happiness from strategy.txt, if there is such
	// a value, otherwise it uses the riot level from const.txt.
	Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy().GetMinimumHappiness(needed);

	double min_delta = static_cast<double>(needed - current) /
	                   static_cast<double>(per_pop_happiness);

	// Round up
	// If we have to add entertainers, we want to add until we are happy.
	// If we have to remove entertainers, only as many before we get unhappy.
	min_delta = ceil(min_delta);

	return static_cast<sint32>(min_delta);
}

#if defined(NEW_RESOURCE_PROCESS)
//----------------------------------------------------------------------------
//
// Name       : Governor::GetMinNumOfFieldWorkers
//
// Description: Estimates the amount of needed workers to generate the amount
//              of resources (food, production or gold) given by
//              resourceFraction.
//
// Parameters : city:             The city data of city for which the amount
//                                needed workers should be calculated.
//              resourceFraction: The fraction of total resources that the
//                                city needs as minimum.
//
// Globals    : g_theCitySizeDB:  The city size database.
//
// Returns    : The amount of minimum field wokers to generate the given
//              fraction of resources.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Governor::GetMinNumOfFieldWorkers(const CityData *city, double resourceFraction) const
{
	double const totalTilesForWorking = static_cast<double>(city->TilesForWorking());
	double neededFraction = 0.0;
	sint32 minResource = -1; // First worker is free

	for (sint32 i = 0; i < g_theCitySizeDB->NumRecords(); ++i)
	{
		double const numPeople = static_cast<double>(city->GetRingSize(i));
		double const peopleFraction = totalTilesForWorking / numPeople;

		if(resourceFraction > neededFraction + peopleFraction)
		{
			neededFraction += peopleFraction;
			minResource += city->GetRingSize(i);
		}
		else if(resourceFraction == neededFraction + peopleFraction)
		{
			minResource += city->GetRingSize(i);
			break;
		}
		else if(resourceFraction < neededFraction + peopleFraction)
		{
			double ringFraction = (resourceFraction - neededFraction) / peopleFraction;
			minResource += static_cast<sint32>(ceil(city->GetRingSize(i) / ringFraction));
			break;
		}
	}

	return minResource;
}
#endif

//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeMinimumWorkers
//
// Description: Estimates the amount workers needed so that all kinds
//              of ressources (food, production, gold and science) are
//              positive. The amount of workers is estimated on the
//              assumption the according resource is not by a specialists
//              gained instead by the field worker.
//
// Parameters : city: The city data of city for which the amount of
//                    workers should be estimated.
//
// Globals    : g_theCitySizeDB: The city size database
//              farmers:         Filled with number of effective farmers.
//              laborers:        Filled with number of effective laborers.
//              merchants:       Filled with number of effective merchants.
//              scientists:      Filled with number of effective scientists.
//              minFood:         Filled minimum field workers for food.
//              minProd:         Filled minimum field workers for production.
//              minGold:         Filled minimum field workers for gold.
//              minScie:         Filled minimum field workers for science.
//              farmersEff       Additional food of a farmer after
//                               deduction of field food.
//              laborersEff      Additional production of a laborer after
//                               deduction of field production.
//              merchantsEff     Additional gold of an merchant after
//                               deduction of field gold.
//              scientistsEff    Additional science of an scientist after
//                               deduction of field food.
//
// Returns    : Amount of workers needed.
//
// Remark(s)  : Pollution unhappiness is not considered. Has to be added
//              later. Maybe additional argumend are needed as well.
//              Better ways to estimate whether there is enough science
//              and production are needed.
//
//----------------------------------------------------------------------------
sint32 Governor::ComputeMinimumWorkers(CityData *city,
                                       sint32 &farmers,
                                       sint32 &laborers,
                                       sint32 &merchants,
                                       sint32 &scientists,
                                       sint32 &minFood,
                                       sint32 &minProd,
                                       sint32 &minGold,
#if defined(NEW_RESOURCE_PROCESS)
                                       sint32 &minScie) const
#else
                                       sint32 &minScie,
                                       double &farmersEff,
                                       double &laborersEff,
                                       double &merchantsEff,
                                       double &scientistsEff) const
#endif
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
#endif

	farmers = 0;
	laborers = 0;
	merchants = 0;
	scientists = 0;

#if defined(NEW_RESOURCE_PROCESS)
	double farmersEff;
	double laborersEff;
	double merchantsEff;
	double scientistsEff;

	double food = city->GetMaxProcessFood();
	double prod = city->GetMaxProcessProd();
	double gold = city->GetMaxProcessGold();
	double scie = city->GetMaxProcessScie();

	double foodRequired = city->GetFoodRequired();
	double prodRequired = 1.0; // At least one production, better requirement needed.
	double goldRequired = static_cast<double>(city->GetSupport());
	double scieRequired = 1.0; // At least one science, better requirement needed.

	double foodFraction = foodRequired / food;
	double prodFraction = prodRequired / prod;
	double goldFraction = goldRequired / gold;
	double scieFraction = scieRequired / scie;

	minFood = GetMinNumOfFieldWorkers(city, foodFraction);
	minProd = GetMinNumOfFieldWorkers(city, prodFraction);
	minGold = GetMinNumOfFieldWorkers(city, goldFraction);
	minScie = GetMinNumOfFieldWorkers(city, scieFraction);

	bool farmersMaxReached = false;
	bool laborersMaxReached = false;
	bool merchantsMaxReached = false;
	bool scientistsMaxReached = false;

	for (sint32 i = city->GetSizeIndex(); i >= 0; --i)
	{
		city->GetSpecialistsEffect(i, farmersEff, laborersEff, merchantsEff, scientistsEff);
		sint32 workers = city->GetWorkingPeopleInRing(i);

		if( farmersEff > 0.0
		&& !farmersMaxReached
		){
			farmers += workers;
		}
		else{
			farmersMaxReached = true;
		}

		if( laborersEff > 0.0
		&& !laborersMaxReached
		){
			laborers += workers;
		}
		else{
			laborersMaxReached = true;
		}

		if( merchantsEff > 0.0
		&& !merchantsMaxReached
		){
			merchants += workers;
		}
		else{
			merchantsMaxReached = true;
		}

		if( scientistsEff > 0.0
		&& !scientistsMaxReached
		){
			scientists += workers;
		}
		else
		{
			scientistsMaxReached = true;
		}
	}

	sint32 freeCount = city->PopCount() - city->SlaveCount();
	if(freeCount < farmers) farmers = freeCount;
	if(freeCount < laborers) laborers = freeCount;
	if(freeCount < merchants) merchants = freeCount;
	if(freeCount < scientists) scientists = freeCount;

	sint32 WorkersNeeded = minFood;
	if(WorkersNeeded < minProd) WorkersNeeded = minProd;
	if(WorkersNeeded < minGold) WorkersNeeded = minGold;
	if(WorkersNeeded < minScie) WorkersNeeded = minScie;

	return WorkersNeeded;

#else
	minFood = 0;
	minProd = 0;
	minGold = 0;
	minScie = 0;
	farmersEff = 0.0;
	laborersEff = 0.0;
	merchantsEff = 0.0;
	scientistsEff = 0.0;

	double utilization_needed;
	const CitySizeRecord *part_rec;
	const CitySizeRecord *full_rec;

	double crimeLossFood;
	double grossFood;
	sint32 grossProduction;
//	sint32 grossGold;

	bool notFoodFound = true;
	bool notProdFound = true;
	bool notGoldFound = true;
	bool notScienceFound = true;
	sint32 part_size;
	sint32 part_size_pop;

	double popFood;
	sint32 popProd;
	sint32 popGold;
	sint32 popScience;

	sint32 specialLoss;
	sint32 crimeLoss;
	sint32 support;
	sint32 popResources;

	sint32 workers_needed = 0;
	sint32 WorkersNeeded = 0;

	double fullTerrainFood = city->GetFoodFromRing(0);
	double partTerrainFood = 0.0;

	sint32 fullTerrainProd = city->GetProdFromRing(0);
	sint32 partTerrainProd = 0;

	sint32 fullTerrainGold = city->GetGoldFromRing(0);
	sint32 partTerrainGold = 0;

	double full_radii_food = city->GetFoodFromRing(0);
	double part_radii_food = 0;

	sint32 full_radii_prod = city->GetProdFromRing(0);
	sint32 part_radii_prod = 0;

	sint32 full_radii_gold = city->GetGoldFromRing(0);
	sint32 part_radii_gold = 0;

	sint32 full_radii_scigold = 0;
	sint32 part_radii_scigold = 0;

	sint32 full_radii_science = 0;
	sint32 part_radii_science = 0;

	for(sint32 sizeIndex = 1; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++)
	{
		Assert(sizeIndex != 0);

#if defined(_DEBUG) || defined(USE_LOGGING)
		t1.start();
#endif

		part_rec      = g_theCitySizeDB->Get(sizeIndex);
		full_rec      = g_theCitySizeDB->Get(sizeIndex-1);

		if(part_rec->GetMaxWorkers() <= city->PopCount())
		{
			part_size = part_rec->GetMaxWorkers() - full_rec->GetMaxWorkers();
		}
		else
		{
			part_size = city->PopCount() - full_rec->GetMaxWorkers();
		}
		part_size_pop = part_rec->GetMaxWorkers() - full_rec->GetMaxWorkers();

		sint32 partSquaredRadius = part_rec->GetSquaredRadius();
		sint32 fullSquaredRadius = full_rec->GetSquaredRadius();

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment ß = %f ms\n", t1.getElapsedTimeInMilliSec()));

		fullTerrainFood += partTerrainFood;
		fullTerrainProd += partTerrainProd;
		fullTerrainGold += partTerrainGold;
		partTerrainFood  = city->GetFoodFromRing(sizeIndex);
		partTerrainProd  = city->GetProdFromRing(sizeIndex);
		partTerrainGold  = city->GetGoldFromRing(sizeIndex);

		//////////////////////////////
		// Intial resetting of values:
		full_radii_food = fullTerrainFood;
		part_radii_food = partTerrainFood;

		full_radii_prod = fullTerrainProd;
		part_radii_prod = partTerrainProd;

		full_radii_gold = fullTerrainGold;
		part_radii_gold = partTerrainGold;

		full_radii_science = 0;
		part_radii_science = 0;

		full_radii_scigold = 0;
		part_radii_scigold = 0;

		////////////////////////////////////////
		// Apply all resource boni and mali
	//	DPRINTF(k_DBG_GAMESTATE, ("sizeIndex: %i\n", sizeIndex));
	//	DPRINTF(k_DBG_GAMESTATE, ("part_size: %i\n", part_size));
	//	DPRINTF(k_DBG_GAMESTATE, ("part_size_pop: %i\n", part_size_pop));
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 0 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		part_radii_prod = city->ProcessProduction(true,
		                         grossProduction,
		                         part_radii_prod,
		                         crimeLoss,
		                         specialLoss, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 1 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		full_radii_prod = city->ProcessProduction(true,
		                         grossProduction,
		                         full_radii_prod,
		                         crimeLoss,
		                         specialLoss, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 2 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		grossFood = part_radii_food;
		city->ProcessFood(crimeLossFood, part_radii_food, grossFood, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 3 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		grossFood = full_radii_food;
		city->ProcessFood(crimeLossFood, full_radii_food, grossFood, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 4 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		city->CollectGold(full_radii_gold, specialLoss, crimeLoss, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 5 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		city->CollectGold(part_radii_gold, specialLoss, crimeLoss, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 6 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		support = city->GetSupport();
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 7 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		full_radii_scigold = full_radii_gold - support;
		part_radii_scigold = part_radii_gold;
		if(full_radii_scigold < 0)
		{
			part_radii_scigold += full_radii_scigold;
			if(part_radii_scigold < 0)
			{
				part_radii_scigold = 0;
			}
		}
		sint32 scieCrime = 0;
		city->SplitScience(true, full_radii_scigold, full_radii_science, scieCrime, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 8 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		city->SplitScience(true, part_radii_scigold, part_radii_science, scieCrime, true);
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment 9 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// farmers to generate the same amount of food
		popResources = city->GetSpecialistsResources(POP_FARMER);
		if(popResources != 0 && part_size_pop > 0)
		{
			popFood = popResources * part_size_pop * city->GetBonusFoodCoeff();
	//		DPRINTF(k_DBG_GAMESTATE, ("popFood: %f\n", popFood));
			grossFood = popFood;
			popFood = city->ProcessFinalFood(crimeLossFood, grossFood);
	//		DPRINTF(k_DBG_GAMESTATE, ("popFood: %f\n", popFood));
	//		DPRINTF(k_DBG_GAMESTATE, ("crimeLossFood: %f\n", crimeLossFood));
	//		DPRINTF(k_DBG_GAMESTATE, ("part_radii_food: %f\n", part_radii_food));

			if(part_radii_food <= popFood)
			{
				farmers += part_size;
				farmersEff = (popFood - part_radii_food)/static_cast<double>(part_size_pop);
			}
			else
				farmers = 0;
		}
		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment A = %f ms\n", t1.getElapsedTimeInMilliSec()));

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// laborers to generate the same amount of production
		popResources = city->GetSpecialistsResources(POP_LABORER);
		if(popResources != 0 && part_size_pop > 0)
		{
			popProd = static_cast<sint32>(popResources * part_size_pop * city->GetBonusProdCoeff());
			popProd = city->ComputeProductionLosses(popProd, crimeLoss, specialLoss);
			popProd -= (crimeLoss + specialLoss);
			if(part_radii_prod <= popProd)
			{
				laborers += part_size;
				laborersEff = static_cast<double>(popProd - part_radii_prod)/static_cast<double>(part_size_pop);
			}
			else
				laborers = 0;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment B = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// merchants to generate the same amount of gold
		popResources = city->GetSpecialistsResources(POP_MERCHANT);
		if(popResources != 0 && part_size_pop > 0)
		{
			popGold = static_cast<sint32>(popResources * part_size_pop * city->GetBonusGoldCoeff());
			city->CalcGoldLoss(true, popGold, specialLoss, crimeLoss);

			if(part_radii_gold <= popGold)
			{
				merchants += part_size;
				merchantsEff = static_cast<double>(popGold - part_radii_gold)/static_cast<double>(part_size_pop);
			}
			else
				merchants = 0;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment C = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// scientists to generate the same amount of science
		popResources = city->GetSpecialistsResources(POP_SCIENTIST);
		if(popResources != 0 && part_size_pop > 0)
		{
			popScience = static_cast<sint32>(popResources * part_size_pop * city->GetBonusScieCoeff());
			popScience -= city->CrimeLoss(popScience);

			if(part_radii_science <= popScience)
			{
				scientists += part_size;
				scientistsEff = static_cast<double>(popScience - part_radii_science)/static_cast<double>(part_size_pop);
			}
			else
				scientists = 0;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment D = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Checks whether minimum number of food workers
		// has been found.
		if(notFoodFound
		&& full_radii_food + part_radii_food > city->GetFoodRequired()
		&& part_radii_food > 0.0
		){
			utilization_needed =
			    (city->GetFoodRequired() - full_radii_food) / part_radii_food;

			if (fullSquaredRadius > 0)
			{
				workers_needed = full_rec->GetMaxWorkers();
			}

			workers_needed += static_cast<sint32>(ceil(utilization_needed *
									(part_rec->GetMaxWorkers() - workers_needed)));
			if(WorkersNeeded < workers_needed)
			{
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0)
			{
				minFood = workers_needed;
			}
			notFoodFound = false;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment E = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Checks whether minimum number of production workers
		// has been found.
		if(notProdFound
		&& full_radii_prod + part_radii_prod > 0 // Needs according function.
		&& part_radii_prod > 0
		){
			utilization_needed =
			    static_cast<double>(0 - full_radii_prod) / static_cast<double>(part_radii_prod);

			if (fullSquaredRadius > 0)
			{
				workers_needed = full_rec->GetMaxWorkers();
			}

			workers_needed += static_cast<sint32>(ceil(utilization_needed *
									(part_rec->GetMaxWorkers() - workers_needed)));
			if(WorkersNeeded < workers_needed)
			{
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0)
			{
				minProd = workers_needed;
			}
			notProdFound = false;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment F = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Checks whether minimum number of gold workers
		// has been found.
//		DPRINTF(k_DBG_GAMESTATE, ("wages: %f\n", g_player[city->GetOwner()]->GetWagesPerPerson()))
//		DPRINTF(k_DBG_GAMESTATE, ("support: %i\n", support));
//		DPRINTF(k_DBG_GAMESTATE, ("full_radii_gold: %i\n", full_radii_gold));
//		DPRINTF(k_DBG_GAMESTATE, ("part_radii_gold: %i\n", part_radii_gold));

		if(notGoldFound
		&& full_radii_gold + part_radii_gold > support
		&& part_radii_gold > 0
		){
			utilization_needed =
			    static_cast<double>(support - full_radii_gold) / static_cast<double>(part_radii_gold);

	//		DPRINTF(k_DBG_GAMESTATE, ("utilization_needed: %f\n", utilization_needed));

			if (fullSquaredRadius > 0)
			{
				workers_needed = full_rec->GetMaxWorkers();
			}

			workers_needed += static_cast<sint32>(ceil(utilization_needed *
									(part_rec->GetMaxWorkers() - workers_needed)));
	//		DPRINTF(k_DBG_GAMESTATE, ("workers_needed: %i\n", workers_needed));
			if(WorkersNeeded < workers_needed)
			{
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0)
			{
				minGold = workers_needed;
			}
			notGoldFound = false;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment G = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Checks whether minimum number of science workers
		// has been found.
		if(notScienceFound
		&& full_radii_science + part_radii_science > 0 // Better condition needed.
		&& part_radii_science > 0
		){
			utilization_needed =
			    static_cast<double>(0 - full_radii_science) / static_cast<double>(part_radii_science);

			if (fullSquaredRadius > 0)
			{
				workers_needed = full_rec->GetMaxWorkers();
			}

			workers_needed += static_cast<sint32>(ceil(utilization_needed *
									(part_rec->GetMaxWorkers() - workers_needed)));
			if(WorkersNeeded < workers_needed){
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0){
				minScie = workers_needed;
			}
			notScienceFound = false;
		}

		DPRINTF(k_DBG_GOVERNOR_DETAIL, ("//  elapsed time per city and pop asignment Z = %f ms\n", t1.getElapsedTimeInMilliSec()));
		if(part_rec->GetPopulation() > city->PopCount())
		{
			break;
		}
	}

	sint32 freeCount = city->PopCount() - city->SlaveCount();
	if(freeCount < farmers) farmers = freeCount;
	if(freeCount < laborers) laborers = freeCount;
	if(freeCount < merchants) merchants = freeCount;
	if(freeCount < scientists) scientists = freeCount;

//	DPRINTF(k_DBG_GAMESTATE, ("farmers: %i\n", farmers));
//	DPRINTF(k_DBG_GAMESTATE, ("laborers: %i\n", laborers));
//	DPRINTF(k_DBG_GAMESTATE, ("merchants: %i\n", merchants));
//	DPRINTF(k_DBG_GAMESTATE, ("scientists: %i\n", scientists));
//	DPRINTF(k_DBG_GAMESTATE, ("farmersEff: %f\n", farmersEff));
//	DPRINTF(k_DBG_GAMESTATE, ("laborersEff: %f\n", laborersEff));
//	DPRINTF(k_DBG_GAMESTATE, ("merchantsEff: %f\n", merchantsEff));
//	DPRINTF(k_DBG_GAMESTATE, ("scientistsEff: %f\n", scientistsEff));
//	DPRINTF(k_DBG_GAMESTATE, ("minFood: %i\n", minFood));
//	DPRINTF(k_DBG_GAMESTATE, ("minProd: %i\n", minProd));
//	DPRINTF(k_DBG_GAMESTATE, ("minGold: %i\n", minGold));
//	DPRINTF(k_DBG_GAMESTATE, ("minScie: %i\n", minScie));
//	DPRINTF(k_DBG_GAMESTATE, ("free: %i\n", freeCount));
//	DPRINTF(k_DBG_GAMESTATE, ("WokersNeeded: %i\n", WorkersNeeded));
	return WorkersNeeded;
#endif
}

void Governor::ComputeDesiredUnits()
{
	m_currentUnitCount.resize(g_theUnitDB->NumRecords());

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const UnitBuildListRecord *build_list_rec;

	sint32 total_unit_support_by_type;
	Army army;
	Unit unit;

	Assert(g_player[m_playerId]);
	Player * player_ptr = g_player[m_playerId];

	sint32 max_cities =
		g_theGovernmentDB->Get(player_ptr->GetGovernmentType())->GetTooManyCitiesThreshold();
	sint32 num_cities = player_ptr->GetNumCities();
	sint16 needed_cities = static_cast<sint16>(max_cities - num_cities);
	sint32 total_unallocated_support = 0;

	double unit_support_percent = 0.0;
	(void) strategy.GetUnitSupportPercent(unit_support_percent);

	sint32 total_unit_support = static_cast<sint32>
		(player_ptr->GetTotalProduction() * unit_support_percent);

	Assert(g_theUnitDB);
	std::fill_n(m_currentUnitCount.begin(), g_theUnitDB->NumRecords(), 0);

	sint32 num_armies = player_ptr->m_all_armies->Num();
	for (sint32 army_index = 0; army_index < num_armies; army_index++)
	{
		army = player_ptr->m_all_armies->Get(army_index);

		for (sint32 unit_index = 0; unit_index < army->Num(); unit_index++)
		{
			unit = army.Get(unit_index);

			if (unit.IsValid())
			{
				m_currentUnitCount[unit.GetType()]++;
			}
		}
	}

	sint32 city_index;
	for (city_index = 0; city_index < num_cities; city_index++)
	{
		unit = player_ptr->m_all_cities->Get(city_index);

		if (unit.IsValid())
		{
			BuildQueue * buildQueue =
			    unit->GetCityData() ? unit->GetCityData()->GetBuildQueue() : NULL;
			Assert(buildQueue);
			if (    buildQueue
			     && buildQueue->GetHead()
			     && k_GAME_OBJ_TYPE_UNIT == buildQueue->GetHead()->m_category
			   )
			{
				m_currentUnitCount[buildQueue->GetHead()->m_type]++;
			}
		}
	}

	m_maximumUnitShieldCost = 0;
	m_currentUnitShieldCost = 0;

	sint32 needed_transport = Scheduler::GetScheduler(m_playerId).
	                            GetMostNeededStrength().Get_Transport();

	// Per-category percent for the six percent-of-support categories below,
	// kept around (rather than a single scalar re-used each iteration) so
	// they can be combined after the loop when more than one category
	// shares the same best unit type.
	double list_percent[BUILD_UNIT_LIST_MAX] = { 0.0 };

	for (int list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		m_buildUnitList[list_num].m_perCityGarrison =  0;
		m_buildUnitList[list_num].m_desiredCount    =  0;
		m_buildUnitList[list_num].m_maximumCount    =  0;
		m_buildUnitList[list_num].m_bestType        = -1;

		sint32 desired_count                        =  0;
		sint32 garrison_count                       =  0;

		build_list_rec = GetBuildListRecord(strategy, (BUILD_UNIT_LIST) list_num);

		switch (list_num)
		{
		case BUILD_UNIT_LIST_OFFENSE:
			garrison_count = strategy.GetOffensiveGarrisonCount();
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetOffensiveUnitsPercent(list_percent[list_num]);
			break;

		case BUILD_UNIT_LIST_DEFENSE:
			garrison_count = strategy.GetDefensiveGarrisonCount();
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetDefensiveUnitsPercent(list_percent[list_num]);
			break;

		case BUILD_UNIT_LIST_RANGED:
			garrison_count = strategy.GetRangedGarrisonCount();
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetRangedUnitsPercent(list_percent[list_num]);
			break;

		case BUILD_UNIT_LIST_FLANKERS:
			garrison_count = strategy.GetFlankerGarrisonCount();
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetFlankerUnitsPercent(list_percent[list_num]);
			break;

		case BUILD_UNIT_LIST_SEA:
			strategy.GetSeaUnitsPercent(list_percent[list_num]);
			break;

		case BUILD_UNIT_LIST_AIR:
			strategy.GetAirUnitsPercent(list_percent[list_num]);
			break;

		case BUILD_UNIT_LIST_SETTLER:
			strategy.GetSettlerUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_SEA_SETTLER:
			strategy.GetSettlerUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_SLAVERY:
			if(!strategy.GetSlaveryUnitsCount(desired_count))
				strategy.GetSpecialUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_SPY:
			if(!strategy.GetSpyUnitsCount(desired_count))
				strategy.GetSpecialUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_DIPLOMAT:
			if(!strategy.GetDiplomatUnitsCount(desired_count))
				strategy.GetSpecialUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_MISSIONARY:
			if(!strategy.GetMissionaryUnitsCount(desired_count))
				strategy.GetSpecialUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_SPECIAL:
			strategy.GetSpecialUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_SEA_TRANSPORT:
			strategy.GetSeaTransportUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_AIR_TRANSPORT:
			strategy.GetAirTransportUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_FREIGHT:
			{
				m_buildUnitList[list_num].m_perCityGarrison = 0;

				sint32 best_unit_type = ComputeBestUnitType(build_list_rec);
				m_buildUnitList[list_num].m_bestType = best_unit_type;

				if (best_unit_type >= 0)
				{
					double freight_per_unit = GetDBUnitRec(best_unit_type)->GetMaxMovePoints();
					Assert(0.0 != freight_per_unit);
					desired_count = static_cast<sint32>(ceil((double)m_neededFreight / freight_per_unit));
				}
			}
			break;

		default:

			Assert(false);
			break;
		}

		if(build_list_rec == NULL)
		{
			DPRINTF(k_DBG_AI, ("// No build list record for list: %d\n", list_num));
			continue;
		}

		sint32 best_unit_type = ComputeBestUnitType(build_list_rec);
		m_buildUnitList[list_num].m_bestType = best_unit_type;

		switch ( list_num )
		{
		case BUILD_UNIT_LIST_OFFENSE:
		case BUILD_UNIT_LIST_DEFENSE:
		case BUILD_UNIT_LIST_RANGED:
		case BUILD_UNIT_LIST_FLANKERS:
		case BUILD_UNIT_LIST_SEA:
		case BUILD_UNIT_LIST_AIR:
			// Percent-to-count conversion for these six is deferred to the
			// combined pass right after this loop, so that categories
			// sharing the same best unit type (m_currentUnitCount is a
			// single count per unit type, not scoped per category) get
			// their percentages combined instead of each independently
			// computing a competing desired count against the same unit.
			break;
		case BUILD_UNIT_LIST_SETTLER:
		case BUILD_UNIT_LIST_SEA_SETTLER:

			if(best_unit_type >= 0)
			{
				m_buildUnitList[list_num].m_maximumCount =
					std::min<sint16>(m_buildUnitList[list_num].m_maximumCount, needed_cities);

				m_buildUnitList[list_num].m_desiredCount =
					m_buildUnitList[list_num].m_maximumCount -
					m_currentUnitCount[best_unit_type];
				DPRINTF(k_DBG_AI, ("// Best settler unit: %s needed: %d, max: %d, current: %d\n", g_theUnitDB->GetNameStr(best_unit_type), m_buildUnitList[list_num].m_desiredCount, m_buildUnitList[list_num].m_maximumCount, m_currentUnitCount[best_unit_type]));
			}
			else
			{
				m_buildUnitList[list_num].m_maximumCount = 0;
				m_buildUnitList[list_num].m_desiredCount = 0;
				DPRINTF(k_DBG_AI, ("// No settler units: needed: %d, max: %d, current: %d\n", m_buildUnitList[list_num].m_desiredCount, m_buildUnitList[list_num].m_maximumCount, 0));
			}

		case BUILD_UNIT_LIST_SLAVERY:
		case BUILD_UNIT_LIST_SPY:
		case BUILD_UNIT_LIST_DIPLOMAT:
		case BUILD_UNIT_LIST_MISSIONARY:
		case BUILD_UNIT_LIST_SPECIAL:
		case BUILD_UNIT_LIST_FREIGHT:
			if (best_unit_type >= 0)
			{
				m_buildUnitList[list_num].m_maximumCount = desired_count;
				m_buildUnitList[list_num].m_desiredCount =
				    static_cast<sint16>(desired_count
				                        - m_currentUnitCount[best_unit_type]
				                       );
			}
			break;
		case BUILD_UNIT_LIST_SEA_TRANSPORT:
			if (best_unit_type >= 0)
			{
				Assert(g_theUnitDB->Get(best_unit_type)->HasCargoData());
				if(g_theUnitDB->Get(best_unit_type)->HasCargoData())
				{
					Assert(g_theUnitDB->Get(best_unit_type)->GetCargoDataPtr()->GetMaxCargo() > 0);
					if(g_theUnitDB->Get(best_unit_type)->GetCargoDataPtr()->GetMaxCargo() > 0)
					{
						needed_transport /= g_theUnitDB->Get(best_unit_type)->GetCargoDataPtr()->GetMaxCargo();
						desired_count = std::max<sint32>(desired_count, needed_transport);

						m_buildUnitList[list_num].m_maximumCount = desired_count;
						m_buildUnitList[list_num].m_desiredCount =
						    static_cast<sint16>(desired_count
						                        - m_currentUnitCount[best_unit_type]
						                       );
					}
				}
			}
			break;

		case BUILD_UNIT_LIST_AIR_TRANSPORT:
			if (best_unit_type >= 0)
			{
				if (m_buildUnitList[BUILD_UNIT_LIST_SEA_TRANSPORT].m_desiredCount == 0)
				{
					m_buildUnitList[list_num].m_maximumCount = desired_count;
					m_buildUnitList[list_num].m_desiredCount =
					    static_cast<sint16>(desired_count
					                        - m_currentUnitCount[best_unit_type]
					                       );
				}
			}
			break;

		default:

			Assert(false);
			break;
		}
	}

	// Convert the six percent-of-support categories' budgets into unit
	// counts, combining any that currently share the same best unit type
	// (e.g. Warrior/Swordsman shared by OFFENSE and FLANKERS before
	// Feudalism unlocks Knight, or Paratrooper/Hover Infantry shared by
	// DEFENSE and OFFENSE) instead of letting each independently claim a
	// desired count against the same m_currentUnitCount entry.
	{
		BUILD_UNIT_LIST const k_percentBasedLists[] =
		{
			BUILD_UNIT_LIST_RANGED,
			BUILD_UNIT_LIST_OFFENSE,
			BUILD_UNIT_LIST_DEFENSE,
			BUILD_UNIT_LIST_FLANKERS,
			BUILD_UNIT_LIST_SEA,
			BUILD_UNIT_LIST_AIR
		};
		sint32 const numPercentBasedLists =
			sizeof(k_percentBasedLists) / sizeof(k_percentBasedLists[0]);

		bool grouped[numPercentBasedLists];
		std::fill_n(grouped, numPercentBasedLists, false);

		for (sint32 i = 0; i < numPercentBasedLists; i++)
		{
			if (grouped[i])
				continue;

			BUILD_UNIT_LIST const list_i = k_percentBasedLists[i];
			sint32 const best_unit_type = m_buildUnitList[list_i].m_bestType;

			grouped[i] = true;

			if (best_unit_type < 0)
			{
				total_unallocated_support += static_cast<sint32>
					(total_unit_support * list_percent[list_i]);
				continue;
			}

			double combined_percent = list_percent[list_i];

			for (sint32 j = i + 1; j < numPercentBasedLists; j++)
			{
				if (!grouped[j] &&
				    m_buildUnitList[k_percentBasedLists[j]].m_bestType == best_unit_type)
				{
					combined_percent += list_percent[k_percentBasedLists[j]];
					grouped[j] = true;
				}
			}

			total_unit_support_by_type =
				static_cast<sint32>(total_unit_support * combined_percent) +
				total_unallocated_support;

			sint32 const bestUnitSupport = GetDBUnitRec(best_unit_type)->GetShieldHunger();
			sint16 maximumCount;

			if (bestUnitSupport > 0)
			{
				Assert(total_unit_support_by_type >= 0);
				maximumCount = static_cast<sint16>
					(floor(static_cast<double>(total_unit_support_by_type) / bestUnitSupport));

				total_unallocated_support = total_unit_support_by_type -
					maximumCount * bestUnitSupport;
			}
			else
			{
				maximumCount = static_cast<sint16>(total_unit_support_by_type);
				total_unallocated_support = total_unit_support_by_type;
			}

			sint16 const desiredCount = static_cast<sint16>
				(maximumCount - m_currentUnitCount[best_unit_type]);

			m_maximumUnitShieldCost += maximumCount * bestUnitSupport;
			m_currentUnitShieldCost += m_currentUnitCount[best_unit_type] * bestUnitSupport;

			for (sint32 j = i; j < numPercentBasedLists; j++)
			{
				if (m_buildUnitList[k_percentBasedLists[j]].m_bestType == best_unit_type)
				{
					m_buildUnitList[k_percentBasedLists[j]].m_maximumCount = maximumCount;
					m_buildUnitList[k_percentBasedLists[j]].m_desiredCount = desiredCount;
				}
			}
		}
	}

	BUILD_UNIT_LIST const k_garrisonLists[] =
	{
		BUILD_UNIT_LIST_OFFENSE,
		BUILD_UNIT_LIST_DEFENSE,
		BUILD_UNIT_LIST_RANGED,
		BUILD_UNIT_LIST_FLANKERS
	};
	sint32 const numGarrisonLists =
		sizeof(k_garrisonLists) / sizeof(k_garrisonLists[0]);

	for (sint32 k = 0; k < numGarrisonLists; k++)
	{
		m_buildUnitList[k_garrisonLists[k]].m_garrisonCount = 0;
	}

	MapPoint pos;

	for (city_index = 0; city_index < num_cities; city_index++)
	{
		unit = player_ptr->m_all_cities->Get(city_index);

		if (!unit.IsValid())
			continue;

		sint32 desired[numGarrisonLists];
		desired[0] = strategy.GetOffensiveGarrisonCount();
		desired[1] = strategy.GetDefensiveGarrisonCount();
		desired[2] = strategy.GetRangedGarrisonCount();
		desired[3] = strategy.GetFlankerGarrisonCount();

		unit->GetPos(pos);
		CellUnitList *  units_ptr   = g_theWorld->GetArmyPtr(pos);
		sint32          unitCount   = units_ptr ? units_ptr->Num() : 0;

		// As in ComputeDesiredUnits's combined pass above, a unit type can
		// be the best pick for more than one garrison category at once -
		// without grouping, the same physical unit would satisfy each
		// category's minimum independently, letting a city report its
		// garrison complete with fewer units than the sum of the
		// configured minimums actually calls for.
		bool grouped[numGarrisonLists];
		std::fill_n(grouped, numGarrisonLists, false);

		for (sint32 k = 0; k < numGarrisonLists; k++)
		{
			if (grouped[k])
				continue;

			grouped[k] = true;

			sint32 const best_unit_type = m_buildUnitList[k_garrisonLists[k]].m_bestType;

			if (best_unit_type < 0)
				continue;

			sint32 combined_desired = desired[k];

			for (sint32 m = k + 1; m < numGarrisonLists; m++)
			{
				if (!grouped[m] &&
				    m_buildUnitList[k_garrisonLists[m]].m_bestType == best_unit_type)
				{
					combined_desired += desired[m];
					grouped[m] = true;
				}
			}

			for (sint32 unit_index = 0; unit_index < unitCount; unit_index++)
			{
				Unit armyUnit = units_ptr->Get(unit_index);

				if (armyUnit.IsValid() && armyUnit->GetType() == best_unit_type)
					combined_desired--;
			}

			for (sint32 m = 0; m < numGarrisonLists; m++)
			{
				if (m_buildUnitList[k_garrisonLists[m]].m_bestType == best_unit_type)
					desired[m] = combined_desired;
			}
		}

		Assert(unit->GetCityData());
		if ( (desired[0] <= 0) &&
			 (desired[1] <= 0) &&
			 (desired[2] <= 0) &&
			 (desired[3] <= 0) )
		{
			unit->GetCityData()->SetGarrisonComplete(TRUE);
		}
		else
		{
			unit->GetCityData()->SetGarrisonComplete(FALSE);
		}

		for (sint32 k = 0; k < numGarrisonLists; k++)
		{
			if (desired[k] > 0)
			{
				m_buildUnitList[k_garrisonLists[k]].m_garrisonCount +=
					static_cast<sint16>(desired[k]);
			}
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	DPRINTF(k_DBG_AI, ("// Desired units for player %d\n", m_playerId));
	for(int list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		sint32 type        = m_buildUnitList[list_num].m_bestType;
		sint16 needed      = m_buildUnitList[list_num].m_desiredCount;
		sint16 max         = m_buildUnitList[list_num].m_maximumCount;
		sint16 garrison    = m_buildUnitList[list_num].m_garrisonCount;
		sint16 garrisonMax = m_buildUnitList[list_num].m_maximumGarrisonCount;
		sint16 perCity     = m_buildUnitList[list_num].m_perCityGarrison;

		if(type >= 0)
		{
			DPRINTF(k_DBG_AI, ("// List %d, Best unit: %s, current: %d, needed: %d, max: %d, garrison: %d, garrison max: %d, per city: %d \n", list_num, g_theUnitDB->GetNameStr(type), m_currentUnitCount[type], needed, max, garrison, garrisonMax, perCity));
		}
		else
		{
			DPRINTF(k_DBG_AI, ("// List %d, Best unit: %s, current: %d, needed: %d, max: %d, garrison: %d, garrison max: %d, per city: %d \n", list_num, "Invalid", -1, needed, max, garrison, garrisonMax, perCity));
		}
	}
#endif
}

void Governor::FillEmptyBuildQueues(bool noWarChange)
{
	Player *    player  = g_player[m_playerId];
	Assert(player);
	if (player == NULL)
		return;

	MapAnalysis::GetMapAnalysis().RecalcCityRanks(m_playerId);
	RebuildCapitol();
	ComputeDesiredUnits();

	m_canBuildLandSettlers = SettleMap::s_settleMap.HasSettleTargets(m_playerId, false);
	m_canBuildSeaSettlers  = SettleMap::s_settleMap.HasSettleTargets(m_playerId, true );

	if (g_network.IsActive() && !g_network.IsLocalPlayer(m_playerId))
		return;

	bool first_turn_of_war = Diplomat::GetDiplomat(m_playerId).FirstTurnOfWar();

	UnitDynamicArray *  city_list = player->GetAllCitiesList();

	for (sint32 i = 0; i < city_list->Num(); i++)
	{
		Unit        cityUnit    = city_list->Access(i);
		if (!cityUnit.IsValid())
			continue;

		CityData *  city        = cityUnit->GetCityData();

		if (city->GetBuildQueue()->GetLen() > 0)
		{
			if (!noWarChange && first_turn_of_war && g_player[m_playerId]->IsRobot())
			{
				// Reconsider AI production at the start of a war
				city->GetBuildQueue()->Clear();
			}
			else if(city->GetBuildQueue()->GetHead()->m_category != k_GAME_OBJ_TYPE_CAPITALIZATION
			     && city->GetBuildQueue()->GetHead()->m_category != k_GAME_OBJ_TYPE_INFRASTRUCTURE)
			{
				if(g_player[m_playerId]->IsRobot())
				{
					StringId advice;
					bool noUnits;
					const BuildListSequenceRecord* sequence = GetMatchingSequence(city, !g_player[m_playerId]->IsRobot(), advice, noUnits);
					sint32 suggested_sequence = sequence->GetIndex();
					if (suggested_sequence >= 0)
						city->SetBuildListSequenceIndex(suggested_sequence);
				}

				// Keep using the current build queue
				continue;
			}
		}

		sint32  cat         = 0;
		sint32  type        = CTPRecord::INDEX_INVALID;
		sint32  list_num    = BUILD_UNIT_LIST_MAX;
		ComputeNextBuildItem(city, cat, type, list_num);

		if (!city->GetUseGovernor() || (CTPRecord::INDEX_INVALID == type))
			continue;

		if(city->GetBuildQueue()->GetLen() > 0)
		{
			if(city->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_CAPITALIZATION
			&&                                     cat      == k_GAME_OBJ_TYPE_CAPITALIZATION)
				continue;

			if(city->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_INFRASTRUCTURE
			&&                                     cat      == k_GAME_OBJ_TYPE_INFRASTRUCTURE)
				continue;

			if(city->GetBuildQueue()->GetLen() > 1) // There is already something after cap/inf, it is needed because, we insert new stuff before stuff was build this turn.
				continue;
		}

		bool insert_ok = false;
		switch (cat)
		{
		case k_GAME_OBJ_TYPE_UNIT:
			insert_ok = city->BuildUnit(type);
			if (insert_ok && list_num < BUILD_UNIT_LIST_MAX)
				m_buildUnitList[list_num].m_desiredCount--;
			break;
		case k_GAME_OBJ_TYPE_WONDER:
			insert_ok = city->BuildWonder(type);
			break;
		case k_GAME_OBJ_TYPE_IMPROVEMENT:
			insert_ok = city->BuildImprovement(type);
			break;
		case k_GAME_OBJ_TYPE_CAPITALIZATION:
			insert_ok = true;
			city->InsertCapitalization();
			city->BuildCapitalization();
			break;
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			insert_ok = true;
			city->InsertInfrastructure();
			city->BuildInfrastructure();
			break;
		}
		Assert(insert_ok);
	}
}

double Governor::PercentUnbuilt(const BUILD_UNIT_LIST unit_list) const
{
	if(m_buildUnitList[unit_list].m_bestType     >= 0
	&& m_buildUnitList[unit_list].m_maximumCount >  0
	){
		return m_buildUnitList[unit_list].m_desiredCount /
		       m_buildUnitList[unit_list].m_maximumCount;
	}

	return 0.0;
}

StringId Governor::GetCityBuildQueueAdvice(const CityData *city) const
{
	StringId adviceId;
	bool noUnits = false;

	GetMatchingSequence(city, false, adviceId, noUnits);

	return adviceId;
}

StringId Governor::GetUnitsAdvice(SlicContext & sc) const
{
	static StringId neededUnitAdviceId = -1;
	static StringId lowMilitaryRankAdviceId = -1;
	static StringId highMilitaryRankAdviceId = -1;

	stringutils_SetStaticStringId(neededUnitAdviceId, "NEEDED_UNIT_TYPE_ADVICE");
	stringutils_SetStaticStringId(lowMilitaryRankAdviceId, "LOW_MILITARY_RANK_ADVICE");
	stringutils_SetStaticStringId(highMilitaryRankAdviceId, "HIGH_MILITARY_RANK_ADVICE");

	double max_percent_unbuilt = 0.0;
	sint32 max_needed_list = -1;
	for (sint32 i = 0; i < BUILD_UNIT_LIST_MAX; i++)
	{
		if(static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_SETTLER
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_SEA_SETTLER
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_SLAVERY
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_SPY
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_DIPLOMAT
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_MISSIONARY
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_SPECIAL
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_SEA_TRANSPORT
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_AIR_TRANSPORT
		|| static_cast<BUILD_UNIT_LIST>(i) == BUILD_UNIT_LIST_FREIGHT
		){
				continue;
		}

		double const percent_unbuilt = PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(i));
		if (percent_unbuilt > max_percent_unbuilt)
		{
			max_percent_unbuilt = percent_unbuilt;
			max_needed_list     = i;
		}
	}

	if (max_needed_list >= 0)
	{
		sc.AddUnitRecord(m_buildUnitList[max_needed_list].m_bestType);
		return neededUnitAdviceId;
	}

	if (g_player[m_playerId]->GetRank(STRENGTH_CAT_MILITARY) < 25)
	{
		return lowMilitaryRankAdviceId;
	}
	else if (g_player[m_playerId]->GetRank(STRENGTH_CAT_MILITARY) > 90)
	{
		return highMilitaryRankAdviceId;
	}

	return -1;
}

void Governor::ComputeNextBuildItem(CityData *city, sint32 & cat, sint32 & type, sint32 & list_num) const
{
	StringId advice;
	bool noUnits = false;
	const BuildListSequenceRecord *build_list_sequence =
		GetMatchingSequence(city, !g_player[m_playerId]->IsRobot(), advice, noUnits);

	if (city->GetUseGovernor() && !g_player[m_playerId]->IsRobot())
	{
		Assert(city->GetBuildListSequenceIndex() >= 0);
		build_list_sequence = g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());
	}
	else
	{
		sint32 suggested_sequence = build_list_sequence->GetIndex();
		if (suggested_sequence >= 0)
			city->SetBuildListSequenceIndex(suggested_sequence);
	}

	bool city_full =
		(g_theWorld->GetCell(city->GetHomeCity().RetPos())->GetNumUnits() >= k_MAX_ARMY_SIZE);

	for (sint32 i = 0; i < build_list_sequence->GetNumBuildListElement(); i++)
	{
		BuildListSequenceRecord::BuildListElement const * elem =
			build_list_sequence->GetBuildListElement(i);

		if (!city_full && !noUnits)
		{
			if (elem->GetAllUnitBuildList())
			{
				type = GetNeededUnitType(city, list_num);
				DPRINTF(k_DBG_AI, ("Unit build list is: %d", list_num));

				if (type >= 0)
				{
					Assert(city->CanBuildUnit(type));
					cat = k_GAME_OBJ_TYPE_UNIT;
					return;
				}
			}
			else if (elem->GetGarrisonUnitBuildList())
			{
				type = GetNeededGarrisonUnitType(city, list_num); // Attention! GetNeededGarrisonUnitType also yiels other unit types and kind of replaces GetNeededUnitType
				if (type >= 0)
				{
					Assert(city->CanBuildUnit(type));
					cat = k_GAME_OBJ_TYPE_UNIT;
					return;
				}
			}
		}

		if (elem->HasBuildingBuildList())
		{
			type = GetNeededBuildingType(city, elem->GetBuildingBuildListPtr());
			if (type >= 0 && city->CanBuildBuilding(type))
			{
				cat = k_GAME_OBJ_TYPE_IMPROVEMENT;
				return;
			}
		}
		else if (elem->HasWonderBuildList())
		{
			type = GetNeededWonderType(city, elem->GetWonderBuildListPtr());
			if (type >= 0 && city->CanBuildWonder(type))
			{
				cat = k_GAME_OBJ_TYPE_WONDER;
				return;
			}
		}
		else if (elem->GetFreight())
		{
			type = GetNeededFreightType(list_num);
			if (type >= 0)
			{
				cat = k_GAME_OBJ_TYPE_UNIT;
				return;
			}
		}
		else if (elem->GetInfrastructure() && city->CanBuildInfrastructure())
		{
			type = 1;
			cat = k_GAME_OBJ_TYPE_INFRASTRUCTURE;
			return;
		}
		else if (elem->GetCapitalization() && city->CanBuildCapitalization())
		{
			type = 1;   // ???
			cat = k_GAME_OBJ_TYPE_CAPITALIZATION;
			return;
		}
		// else try next element
	}

	DPRINTF(k_DBG_AI, ("Nothing valid found to build in %s, increase production allocated to units?\n", city->GetName()));
	cat = k_GAME_OBJ_TYPE_UNIT;
	type = m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_bestType;
}

bool Governor::HasStopBuildings(const StrategyRecord::BuildListSequenceElement* elem, const CityData* cd) const
{
	if(elem->HasHasBuildingsThenStop())
	{
		const BuildingBuildListRecord* rec = elem->GetHasBuildingsThenStopPtr();

		if(rec == NULL) return false;

		for(sint32 j = 0; j < rec->GetNumBuilding(); ++j)
		{
			if(!cd->HasBuilding(rec->GetBuildingIndex(j)) && cd->CanBuildBuilding(rec->GetBuildingIndex(j)))
			{
				return false;
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

const StrategyRecord::BuildListSequenceElement * Governor::GetMatchingSequenceElement(const CityData *city, bool isBuildDecision) const
{
	Assert(city);

	Assert(g_player[m_playerId]);
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	double rank = 0.0;
	const StrategyRecord::BuildListSequenceElement *best_elem = NULL;
	sint32 best_priority = -99999;

	sint32 pollution = city->GetPollution();
	sint32 minPollution;

	// How far this city's population is above its own overcrowding
	// threshold (base-overcrowding-for-its-size-tier plus any building
	// bonuses); zero or negative if the city isn't overcrowded at all.
	sint32 overcrowding = city->PopCount() -
		static_cast<sint32>(city->GetOvercrowding(g_theCitySizeDB->Get(city->GetSizeIndex())));
	sint32 minOvercrowding;

	sint32 minNumUnits;
	sint32 maxRawHappiness;

	sint32 cityRawHappiness = static_cast<sint32>(city->GetHappiness()) - city->GetHappinessFromPops();

	// Diagnostic: city->GetHappiness() is a cache, last written by whichever of
	// CityTurnPreProductionEvent's CalcHappiness(true) or AssignPopulation's
	// CalcHappiness(false) ran most recently for this city - NOT necessarily this
	// turn's own CalcPollution() result, since CtpAi::BeginTurn runs
	// FillEmptyBuildQueues() (this function's caller, for a build decision) BEFORE
	// AssignPopulations() (ctpai.cpp ~1718 vs ~1732) refreshes happiness for the
	// turn. Reading the tracker's own recorded HAPPY_REASON_POLLUTION component
	// alongside the live `pollution` value lets a playtest log show directly
	// whether the cached happiness used above already reflects this turn's
	// pollution, or is one step stale - suspected after seeing "happiness w/o
	// entertainers 99" logged next to "pollution 769" for the same city/turn.
	double cachedPollutionHappiness = 0.0;
	StringId happyReasonName;
	const_cast<CityData *>(city)->GetHappy()->GetHappyTracker()->
		GetHappiness(HAPPY_REASON_POLLUTION, cachedPollutionHappiness, happyReasonName);

	bool canBuildWonders = false;
	sint32 i;
	for(i = 0; i < g_theWonderDB->NumRecords(); ++i)
	{
		if(city->CanBuildWonder(i))
		{
			canBuildWonders = true;
			break;
		}
	}

	for(i = 0; i < strategy.GetNumBuildListSequenceElement(); i++)
	{
		StrategyRecord::BuildListSequenceElement const * elem =
			strategy.GetBuildListSequenceElement(i);

		if(best_elem && elem->GetPriority() < best_priority)
			continue;

		if(HasStopBuildings(elem, city))
			continue;

		if(elem->GetMinPollution(minPollution) && pollution < minPollution)
			continue;

		if(elem->GetMinOvercrowding(minOvercrowding) && overcrowding < minOvercrowding)
			continue;

		if(elem->GetCanBuildWonders() && !canBuildWonders)
			continue;

		if(elem->GetMinNumUnits(minNumUnits))
		{
			if(g_theWorld->GetCell(city->GetHomeCity()->GetPos())->GetNumUnits() < minNumUnits)
				continue;
		}

		if(elem->GetMinNumCities(minNumUnits) && (m_canBuildLandSettlers || (m_canBuildSeaSettlers && city->IsCoastal())))
		{
			if(g_player[m_playerId]->GetNumCities() < minNumUnits)
				continue;
		}

		if(elem->GetMinTransporters(minNumUnits) && city->IsCoastal() && m_buildUnitList[BUILD_UNIT_LIST_SEA_TRANSPORT].m_desiredCount > 0)
		{
			if (minNumUnits > 0 && g_player[m_playerId]->GetTransporterNum() < minNumUnits) // minNumUnits < 1 is valid input in the strategies.txt
				continue;
		}

		if(elem->GetMaxRawHappiness(maxRawHappiness) && maxRawHappiness < cityRawHappiness)
			continue;

		if(elem->GetProductionCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetProductionRank(city);
		}

		else if(elem->GetGrowthCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetGrowthRank(city);
		}

		else if(elem->GetCommerceCities())
		{
			// A city can rank fine against other cities by commerce
			// percentile while still spending more gold than it earns in
			// absolute terms (e.g. unusually high upkeep for its own
			// size/buildings) - the normal Top/Bottom rank check below
			// wouldn't catch that. When net gold is actually negative,
			// match this element using GoldNegativePriority instead,
			// bypassing the percentile check entirely - a direct "losing
			// money right now" signal independent of how other cities compare.
			sint32 goldNegativePriority;
			if(elem->GetGoldNegativePriority(goldNegativePriority) && city->GetNetCityGold() < 0)
			{
				best_priority = goldNegativePriority;
				best_elem     = elem;
				continue;
			}

			rank = MapAnalysis::GetMapAnalysis().
			       GetCommerceRank(city);
		}

		else if(elem->GetHappyCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetHappinessRank(city);
		}

		else if(elem->GetThreatenedCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetThreatRank(city);
		}

		else if(elem->GetPowerCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetPowerRank(city);
		}

		else if(elem->GetHasDeficit())
		{
			// Replacement for the reverted GoldNegativePriority override -
			// a standalone condition rather than a CommerceCities-specific
			// override, so it needs its own branch instead of folding into
			// the rank-based matching below. Matches outright whenever the
			// city's net gold is actually negative, independent of how it
			// ranks against other cities; otherwise this element simply
			// doesn't apply - skip it the same as a failed gate earlier in
			// this function would have.
			if(city->GetNetCityGold() >= 0)
				continue;

			best_priority = elem->GetPriority();
			best_elem     = elem;
			continue;
		}

		else if(elem->HasSmallCitiesMaxSize())
		{
			// Do nothing
		}

		else if(elem->HasBeforeMaxCitySize())
		{
			// Do nothing
		}

		else if(elem->HasMinPollution() || elem->HasMinOvercrowding() || elem->GetAllCities())
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
			continue;
		}

		else if( elem->GetDefault() )
		{
			if (best_elem == NULL)
			{
				best_priority = elem->GetPriority();
				best_elem = elem;
				continue;
			}
		}
		else
		{
			continue;
		}

		// The -1.0 seeds only silence -Wmaybe-uninitialized; the Get*()
		// bit-accessors write the out-param whenever they return true, so
		// the seed is never actually read. It's picked to make the guarded
		// comparison false rather than true if that ever stopped holding:
		// rank is in [0.0, 1.0], so rank >= 1.0 - (-1.0) and rank <= -1.0
		// are both unreachable. (Note the opposite convention in
		// GetMatchingPopAssignment, where the test is bottom_value < rank
		// / rank < 1.0 - top_value, so 1.0 / 0.0 are the false-forcing
		// seeds there.)
		double top_value = -1.0;
		if (elem->GetTop(top_value) &&
		    (rank >= 1.0 - top_value)
		   )
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
			continue;
		}

		double bottom_value = -1.0;
		if (elem->GetBottom(bottom_value) &&
		    (rank <= bottom_value)
		   )
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
			continue;
		}

		sint32 cityMaxSize;
		sint32 citySize;
		city->GetPop(citySize);

		if(
		       elem->GetSmallCitiesMaxSize(cityMaxSize)
		    && cityMaxSize >= citySize
		   )
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
			continue;
		}

		sint32 value;
		if
		  (
		       elem->GetBeforeMaxCitySize(value)
		    && city->GetMaxPop() - value <= citySize
		    && city->GetPossibleBuildingMaxPopIncrease() > 0
		  )
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
			continue;
		}
	}

	if (best_elem == NULL)
	{
		best_elem = strategy.GetBuildListSequenceElement(i-1);
		Assert(best_elem);
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	// Diagnostic: which BuildListSequence category did this city match,
	// and where does it currently rank on the relevant dimension? Lets a
	// playtest log show whether cities actually rotate through categories
	// over time, or whether the same cities stay locked into the same
	// (e.g. WONDERS) category turn after turn.
	//
	// GetMatchingSequenceElement() is also called from FindBestTileImprovement/
	// AddRoadPriority/ComputeInstallationPriorities (once per candidate
	// tile/road/installation, not once per city per turn) purely to read
	// the matched element's own bonus fields - logging unconditionally
	// counted those lookups too, inflating a city's hit count ~27x above
	// its actual once-per-turn build decision. isBuildDecision is true
	// only for the real decision call, GetMatchingSequence() below.
	if (isBuildDecision)
	{
		// All six MapAnalysis ranks, not just the one the matched element
		// actually used - lets a playtest log check how close a city was
		// to a DIFFERENT category's own threshold without a second pass
		// (e.g. was this city's production rank already past Offensive's
		// Top 0.2 while Dirty Cities' MinPollution just hadn't been
		// crossed yet).
		double const productionRank = MapAnalysis::GetMapAnalysis().GetProductionRank(city);
		double const growthRank     = MapAnalysis::GetMapAnalysis().GetGrowthRank(city);
		double const commerceRank   = MapAnalysis::GetMapAnalysis().GetCommerceRank(city);
		double const happinessRank  = MapAnalysis::GetMapAnalysis().GetHappinessRank(city);
		double const threatRank     = MapAnalysis::GetMapAnalysis().GetThreatRank(city);
		double const powerRank      = MapAnalysis::GetMapAnalysis().GetPowerRank(city);

		const char * rankKind = "none";
		double       matchRank = 0.0;

		if(best_elem->GetProductionCities())
		{
			rankKind  = "Production";
			matchRank = productionRank;
		}
		else if(best_elem->GetGrowthCities())
		{
			rankKind  = "Growth";
			matchRank = growthRank;
		}
		else if(best_elem->GetCommerceCities())
		{
			rankKind  = "Commerce";
			matchRank = commerceRank;
		}
		else if(best_elem->GetHappyCities())
		{
			rankKind  = "Happiness";
			matchRank = happinessRank;
		}
		else if(best_elem->GetThreatenedCities())
		{
			rankKind  = "Threat";
			matchRank = threatRank;
		}
		else if(best_elem->GetPowerCities())
		{
			rankKind  = "Power";
			matchRank = powerRank;
		}

		const BuildListSequenceRecord * matched_seq = best_elem->GetBuildListSequence();

		// turn/player/city-count and this city's own production/food/gold/
		// pollution/happiness were added so a playtest log analysis
		// doesn't have to cross-reference this line against separate
		// "Turn N"/"Player N" header lines and a per-turn distinct-city
		// count just to answer "how many cities does this empire have
		// right now, and how does this one's own output compare" - it's
		// all here already. pollution and cityRawHappiness are the same
		// local variables HasStopBuildings/the MinPollution/MaxRawHappiness
		// gates above already computed for this city, not a second read.
		//
		// buildings is city->GetImprovements() (m_built_improvements), the
		// city's own bitmask of every building it has ever built - one bit
		// per BuildingRecord index, decodable offline against BuildingDB's
		// own record order without needing a name printed per bit here.
		// Printed as two hex halves (high 32 bits, then low 32 bits) since
		// no DPRINTF in this codebase prints a raw uint64 directly and a
		// %llx/%I64x format specifier isn't portable across this project's
		// MSVC/GCC builds - concatenate the two halves to get the full value
		// back, same convention as AchievementTracker::AddAchievement.
		uint64 const buildings = city->GetImprovements();
		DPRINTF(k_DBG_GOVERNOR, ("GetMatchingSequenceElement: turn %d player %d city %s (production %d, food %f, gold %d, pollution %d, happiness w/o entertainers %d, cached pollution happiness %f, buildings 0x%08x%08x) of %d cities -> sequence %s (priority %d, rank kind %s, rank %f) ranks: production %f growth %f commerce %f happiness %f threat %f power %f\n",
		        NewTurnCount::GetCurrentRound(),
		        m_playerId,
		        const_cast<CityData *>(city)->GetName(),
		        city->GetNetCityProduction(),
		        city->GetNetCityFood(),
		        city->GetNetCityGold(),
		        pollution,
		        cityRawHappiness,
		        cachedPollutionHappiness,
		        static_cast<uint32>(buildings >> 32),
		        static_cast<uint32>(buildings & 0xffffffffu),
		        g_player[m_playerId]->GetNumCities(),
		        matched_seq ? matched_seq->GetNameText() : "NULL",
		        best_elem->GetPriority(),
		        rankKind,
		        matchRank,
		        productionRank, growthRank, commerceRank, happinessRank, threatRank, powerRank));
	}
#endif

	return best_elem;
}

const BuildListSequenceRecord * Governor::GetMatchingSequence(const CityData *city, const bool human_city, StringId & advice, bool & noUnits) const
{
	Assert(city);
	if (city->GetUseGovernor() && human_city)
		return g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());

	const StrategyRecord::BuildListSequenceElement *best_elem = GetMatchingSequenceElement(city, true);

	advice = -1;

	if (best_elem == NULL)
		return g_theBuildListSequenceDB->Get(0);

	noUnits = best_elem->GetNoUnits();

	(void) best_elem->GetAdvice(advice);

	return best_elem->GetBuildListSequence();
}

// Attention! GetNeededGarrisonUnitType also yiels other unit types and kind of replaces GetNeededUnitType
// GetNeededGarrisonUnitType function covers GarrisonUnitBuildList of BuildListSequences.txt,
// which is called far more often than AllUnitBuildList which is covered by GetNeededUnitType
// possibly GetNeededGarrisonUnitType and GetNeededUnitType should be merged and one of them removed
// including duplicated code
sint32 Governor::GetNeededUnitType(const CityData *city, sint32 & list_num) const
{
	Assert(g_player[m_playerId]);

	BUILD_UNIT_LIST max_list                = BUILD_UNIT_LIST_MAX;
	sint32          max_production          = 0;
	sint32          needed_production;
	sint32          type                    = CTPRecord::INDEX_INVALID;
	sint32          cont;
	double          build_transport_production_level;
	double          build_settler_production_level;

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	strategy.GetBuildTransportProductionLevel(build_transport_production_level);
	strategy.GetBuildSettlerProductionLevel(build_settler_production_level);

	bool canBuildSettlers  = static_cast<double>(m_currentUnitShieldCost) /
	                         static_cast<double>(m_maximumUnitShieldCost) >
	                                      build_settler_production_level;
//	                      || city->GetNeededGarrisonStrength() * build_settler_production_level <= city->GetCurrentGarrisonStrength();

	bool canBuildTransporters = static_cast<double>(m_currentUnitShieldCost) /
	                            static_cast<double>(m_maximumUnitShieldCost) >
	                                         build_transport_production_level;
//	                      || city->GetNeededGarrisonStrength() * build_transport_production_level <= city->GetCurrentGarrisonStrength();


	for (list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		if( !g_player[m_playerId]->IsRobot()
		&& (   static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SLAVERY
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPY
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_DIPLOMAT
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_MISSIONARY
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPECIAL
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_AIR_TRANSPORT
		   )
		  )
		{
			continue;
		}

		if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
		|| static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA
		|| static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
		){
			if (!g_theWorld->GetAdjacentOcean(city->GetHomeCity().RetPos(), cont))
				continue;
		}

		if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_FREIGHT)
			continue;

		const BuildUnitList & list_ref = m_buildUnitList[list_num];

		if
		  (
		       list_ref.m_bestType < 0
		   || !city->CanBuildUnit(list_ref.m_bestType)
		  )
		{
			continue;
		}

		needed_production =
			GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

		needed_production *= list_ref.m_desiredCount;

		if ( needed_production > 0 )
		{
			if
			  (
			   (
			        static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
			     || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_AIR_TRANSPORT
			   )
			   // This does not work so well yet
//			   && (PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(list_num)) > PercentUnbuilt(BUILD_UNIT_LIST_SETTLER)     || m_currentUnitCount[m_buildUnitList[BUILD_UNIT_LIST_SETTLER].m_bestType]     > 2)
//			   && (PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(list_num)) > PercentUnbuilt(BUILD_UNIT_LIST_SEA_SETTLER) || m_currentUnitCount[m_buildUnitList[BUILD_UNIT_LIST_SEA_SETTLER].m_bestType] > 2)
			  )
			{
				if(canBuildTransporters)
				{
					max_list = static_cast<BUILD_UNIT_LIST>(list_num);
					break;
				}
			}
			else if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER)
			{
				if(canBuildSettlers && m_canBuildLandSettlers)
				{
					max_list = static_cast<BUILD_UNIT_LIST>(list_num);
					break;
				}
			}
			else if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER)
			{
				if(canBuildSettlers && m_canBuildSeaSettlers)
				{
					max_list = static_cast<BUILD_UNIT_LIST>(list_num);
					break;
				}
			}
			else if
			  (
			       static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SLAVERY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_DIPLOMAT
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_MISSIONARY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPECIAL
			  )
			{
				if(canBuildSettlers)
				{
					max_list = static_cast<BUILD_UNIT_LIST>(list_num);
					break;
				}
			}
			else if(needed_production > max_production)
			{
				max_production = needed_production;
				max_list = static_cast<BUILD_UNIT_LIST>(list_num);
			}
		}
	}

	if (max_list != BUILD_UNIT_LIST_MAX) // Redesign for best unit type
	{
		type = m_buildUnitList[max_list].m_bestType;
		list_num = max_list;

		if (!city->CanBuildUnit(type))
		{
			const UnitBuildListRecord *build_list_rec = GetBuildListRecord(strategy, max_list);
			type = ComputeBestUnitType(build_list_rec, city);
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	UnitRecord const *	unit	= (type < 0) ? NULL : GetDBUnitRec(type);
	DPRINTF(k_DBG_GAMESTATE, ("Player: %lx, Selected unit type: %s\n", m_playerId, unit ? unit->GetNameText() : "none"));
#endif

	return type;
}

const UnitBuildListRecord * Governor::GetBuildListRecord(const StrategyRecord & strategy, const BUILD_UNIT_LIST list_type) const
{
	switch (list_type)
	{
	case BUILD_UNIT_LIST_OFFENSE:
		Assert(strategy.HasOffensiveUnitList());
		return strategy.HasOffensiveUnitList() ? strategy.GetOffensiveUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_DEFENSE:
		Assert(strategy.HasDefensiveUnitList());
		return strategy.HasDefensiveUnitList() ? strategy.GetDefensiveUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_RANGED:
		Assert(strategy.HasRangedUnitList());
		return strategy.HasRangedUnitList() ? strategy.GetRangedUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_FLANKERS:
		Assert(strategy.HasFlankerUnitList());
		return strategy.HasFlankerUnitList() ? strategy.GetFlankerUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SEA:
		Assert(strategy.HasSeaUnitList());
		return strategy.HasSeaUnitList() ? strategy.GetSeaUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_AIR:
		Assert(strategy.HasAirUnitList());
		return strategy.HasAirUnitList() ? strategy.GetAirUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SETTLER:
		Assert(strategy.HasSettlerUnitList());
		return strategy.HasSettlerUnitList() ? strategy.GetSettlerUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SEA_SETTLER:
//		Assert(strategy.HasSeaSettlerUnitList());
		return strategy.HasSeaSettlerUnitList() ? strategy.GetSeaSettlerUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SLAVERY:
//		Assert(strategy.HasSlaverUnitList());
		return strategy.HasSlaverUnitList() ? strategy.GetSlaverUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SPY:
//		Assert(strategy.HasSpyUnitList());
		return strategy.HasSpyUnitList() ? strategy.GetSpyUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_DIPLOMAT:
//		Assert(strategy.HasDiplomatUnitList());
		return strategy.HasDiplomatUnitList() ? strategy.GetDiplomatUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_MISSIONARY:
		Assert(strategy.HasMissionaryUnitList());
		return strategy.HasMissionaryUnitList() ? strategy.GetMissionaryUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SPECIAL:
		Assert(strategy.HasSpecialUnitList());
		return strategy.HasSpecialUnitList() ? strategy.GetSpecialUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_SEA_TRANSPORT:
		Assert(strategy.HasSeaTransportUnitList());
		return strategy.HasSeaTransportUnitList() ? strategy.GetSeaTransportUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_AIR_TRANSPORT:
		Assert(strategy.HasAirTransportUnitList());
		return strategy.HasAirTransportUnitList() ? strategy.GetAirTransportUnitListPtr() : NULL;

	case BUILD_UNIT_LIST_FREIGHT:
		Assert(strategy.HasFreightUnitList());
		return strategy.HasFreightUnitList() ? strategy.GetFreightUnitListPtr() : NULL;

	default:
		Assert(false);
		return NULL;
	}
}

double Governor::MaxiumGarrisonDefence(const MapPoint & pos) const
{
	double garrisonDefence = 0.0;

	for (int list_num = BUILD_UNIT_LIST_RANGED; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		const BuildUnitList & list_ref = m_buildUnitList[list_num];

		if (list_ref.m_bestType >= 0)
		{
			const UnitRecord* rec = GetDBUnitRec(list_ref.m_bestType);

//			double defense   = unitutil_GetPositionDefense(rec, true, pos, Unit()); // @ToDo figure out which defense should be used
//			double defense   = rec->GetDefense(); // Raw defense
			double firepower = static_cast<double>(rec->GetFirepower());
			double hitpoints = static_cast<double>(rec->GetMaxHP());

			garrisonDefence += rec->GetDefense() * firepower * hitpoints * static_cast<double>(list_ref.m_perCityGarrison);
		}
	}

	return garrisonDefence;
}

// @ToDo Cleanup this function, duplicated, and superflous code, and fix it
// Attention! GetNeededGarrisonUnitType also yiels other unit types and kind of replaces GetNeededUnitType
// This function covers GarrisonUnitBuildList of BuildListSequences.txt, which is called far more often than
// AllUnitBuildList which is covered by GetNeededUnitType
// Possibly GetNeededGarrisonUnitType and GetNeededUnitType should be merged and one of them removed
// including duplicated code
sint32 Governor::GetNeededGarrisonUnitType(const CityData * city, sint32 & list_num) const
{
	Assert( city );
	if (city->GetGarrisonComplete() && !city->GetGarrisonOtherCities())
		return CTPRecord::INDEX_INVALID;

	double garrisonStrength       = city->GetCurrentGarrisonStrength();
	double neededGarrisonStrength = MaxiumGarrisonDefence(city->GetHomeCity().RetPos());
	double garrisonComplete       = (neededGarrisonStrength > 0.0) ? garrisonStrength / neededGarrisonStrength : 1.0;

	double          build_transport_production_level;
	double          build_settler_production_level;
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	strategy.GetBuildTransportProductionLevel(build_transport_production_level);
	strategy.GetBuildSettlerProductionLevel(build_settler_production_level);

	Assert( g_theWorld );
	BUILD_UNIT_LIST max_list = BUILD_UNIT_LIST_MAX;
	sint32 max_production = 0;
	sint32 needed_production = 0;
	CellUnitList garrison_army;
	sint32 cont;

	for (list_num = BUILD_UNIT_LIST_SEA_TRANSPORT; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		const BuildUnitList & list_ref = m_buildUnitList[list_num];

		if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
		|| static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA
		|| static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
		){
			if (!g_theWorld->GetAdjacentOcean(city->GetHomeCity().RetPos(), cont))
				continue;
		}

		if
		  (
		       list_ref.m_bestType < 0
		   || !city->CanBuildUnit(list_ref.m_bestType)
		  )
		{
			needed_production = 0;
			continue;
		}
		else if ( /*!city->GetGarrisonComplete()*/ garrisonComplete < 1.0)
		{
			// @ToDo: Merge duplicated code
	//		double garrisonPercent = list_ref.m_maximumGarrisonCount > 0 ? static_cast<double>(list_ref.m_garrisonCount) / static_cast<double>(list_ref.m_maximumGarrisonCount) : 1.0;
			if
			  (
			   (
			         static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
			      || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA
			   )
			   && garrisonComplete > build_transport_production_level // This may be exposed extra
			   // This does not work so well yet
//			   && (PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(list_num)) > PercentUnbuilt(BUILD_UNIT_LIST_SETTLER)     || m_currentUnitCount[m_buildUnitList[BUILD_UNIT_LIST_SETTLER].m_bestType]     > 2)
//			   && (PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(list_num)) > PercentUnbuilt(BUILD_UNIT_LIST_SEA_SETTLER) || m_currentUnitCount[m_buildUnitList[BUILD_UNIT_LIST_SEA_SETTLER].m_bestType] > 2)
			  )
			{
				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				needed_production *= list_ref.m_desiredCount;
				if(needed_production > 0)
				{
					max_list = (BUILD_UNIT_LIST) list_num;
					break;
				}
			}
			else if
			  (
			      static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER
			   && garrisonComplete > build_settler_production_level
			  )
			{
				if(!m_canBuildLandSettlers)
				{
					continue;
				}

				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				needed_production *= list_ref.m_desiredCount;
				if(needed_production > 0)
				{
					max_list = (BUILD_UNIT_LIST) list_num;
					break;
				}
			}
			else if
			  (
			      static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
			   && garrisonComplete > build_settler_production_level
			   && this->m_canBuildSeaSettlers
			  )
			{
				if(!m_canBuildSeaSettlers)
				{
					continue;
				}

				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				needed_production *= list_ref.m_desiredCount;
				if(needed_production > 0)
				{
					max_list = (BUILD_UNIT_LIST) list_num;
					break;
				}
			}
			else if
			  (
			   (    static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SLAVERY
			     || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPY
			     || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_DIPLOMAT
			     || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_MISSIONARY
			     || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPECIAL
			   )
			   && garrisonComplete > build_settler_production_level
			  )
			{
				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				needed_production *= list_ref.m_desiredCount;
				if(needed_production > 0)
				{
					max_list = (BUILD_UNIT_LIST) list_num;
					break;
				}
			}
			else
			{
				needed_production = (list_ref.m_garrisonCount *
									 GetDBUnitRec(list_ref.m_bestType)->GetShieldCost());
			}
		}
		else
		{
			if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER)
			{
				if(!m_canBuildLandSettlers) continue;
			}
			else if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER)
			{
				if(!m_canBuildSeaSettlers) continue;
			}

			// @ToDo: Cleanup duplicated code
			if(
			       static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_AIR_TRANSPORT
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SLAVERY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_DIPLOMAT
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_MISSIONARY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPECIAL
			  )
			{
			//	if(garrisonComplete >= 1.0) // Should be irrelevant
				{
					needed_production =
						GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

					needed_production *= list_ref.m_desiredCount;
					if(needed_production > 0)
					{
						max_list = (BUILD_UNIT_LIST) list_num;
						break;
					}
				}

			//	Assert(garrisonComplete >= 1.0);
			}
			else
			{
				needed_production = list_ref.m_perCityGarrison *
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				g_theWorld->GetArmy( city->GetHomeCity().RetPos(), garrison_army );
				for (sint32 i = 0; i < garrison_army.Num(); i++)
				{
					if ( garrison_army.Get(i).GetDBRec()->GetIndex() == list_ref.m_bestType)
					{
						needed_production -=
							GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();
					}
				}
			}
		}

		if ( needed_production > max_production )
		{
			max_production = needed_production;
			max_list = (BUILD_UNIT_LIST) list_num;
		}
	}

	sint32 type = CTPRecord::INDEX_INVALID;

	if (max_list != BUILD_UNIT_LIST_MAX)
	{
		type = m_buildUnitList[max_list].m_bestType;
		list_num = max_list;

		if (!city->CanBuildUnit(type))
		{
			const UnitBuildListRecord *build_list_rec = GetBuildListRecord(strategy, max_list);

			if(build_list_rec != NULL)
			{
				type = ComputeBestUnitType(build_list_rec, city);
			}
		}
	}

	return type;
}

sint32 Governor::GetNeededBuildingType(const CityData *city, const BuildingBuildListRecord *build_list_rec) const
{
	Assert(city);

	bool shouldNotBuildGaiaController = g_player[m_playerId]->GetNumCities() < g_player[m_playerId]->GetGaiaController()->NumMainframesRequired();

	for (sint32 i = 0; i < build_list_rec->GetNumBuilding(); i++)
	{
		sint32 const building_type = build_list_rec->GetBuildingIndex(i);

		if(GaiaController::IsSatellite(building_type)
		&&(shouldNotBuildGaiaController
		|| g_player[m_playerId]->GetGaiaController()->HasMaxSatsBuilt()))
			continue;

		if(GaiaController::IsMainframeBuilding(building_type)
		&&(shouldNotBuildGaiaController
		|| g_player[m_playerId]->GetGaiaController()->HasMinCoresBuilt()))
			continue;

		if ( city->CanBuildBuilding(building_type) )
			return building_type;
	}

	return CTPRecord::INDEX_INVALID;
}

sint32 Governor::GetNeededWonderType(const CityData *city, const WonderBuildListRecord *build_list_rec ) const
{
	Assert(g_player[m_playerId]);
	Assert(city);
	Assert(g_theWonderTracker);

	for (int i = 0; i < build_list_rec->GetNumWonder(); i++)
	{
		sint32 const wonder_type = build_list_rec->GetWonderIndex(i);

		if (g_theWonderTracker->IsBuildingWonder(wonder_type, m_playerId))
			continue;

		if ( city->CanBuildWonder(wonder_type))
			return wonder_type;
	}

	return CTPRecord::INDEX_INVALID;
}

sint32 Governor::GetNeededFreightType(sint32 & list_num) const
{
	list_num = (sint32) BUILD_UNIT_LIST_FREIGHT;

	const BuildUnitList & list_ref = m_buildUnitList[list_num];

	if (list_ref.m_bestType < 0)
		return -1;

	if (list_ref.m_desiredCount <= 0)
		return -1;

	return list_ref.m_bestType;
}

sint32 Governor::ComputeBestUnitType(const UnitBuildListRecord *build_list_rec, const CityData *city) const
{
	Assert(g_player[m_playerId]);

	for (int i = build_list_rec->GetNumUnit()-1; i >= 0; i--)
	{
		sint32 const unit_type = build_list_rec->GetUnit(i)->GetIndex();

		if (city && !city->CanBuildUnit(unit_type))
			continue;

		if ( g_player[m_playerId]->CanBuildUnit(unit_type) )
			return unit_type;
	}

	return CTPRecord::INDEX_INVALID;
}

sint32 Governor::ComputeBestMilitaryReadiness() const
{

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	sint32 new_level = 0;
	(void) strategy.GetReadinessLevel(new_level);

#if 0
// Probably intended to test whether the economy is able to support the military budget.
	Assert(g_player[m_playerId]);
	Player * player_ptr = g_player[m_playerId];

	double max_support_cost = 1.0;
	sint32 total_production = player_ptr->GetTotalProduction();

	sint32 support_percent = 0;
	if (strategy.GetMaxSupportCostPercent(support_percent))
	{
		max_support_cost = (static_cast<double>(support_percent) / 100.0);
	}

	if(total_production
	&& player_ptr->GetReadinessCost() > (max_support_cost * total_production)
	){
		// Spending too much. However, like in real life,
		// this fact is completely ignored.
	}
#endif

	return new_level;
}

StringId Governor::GetTacticalAdvice(SlicContext & sc) const
{
	static StringId defendAreaAdviceId = -1;
	static StringId atPeaceReadinessAdviceId = -1;
	static StringId alertReadinessAdviceId = -1;
	static StringId atWarReadinessAdviceId = -1;
	static StringId seigeAdviceId = -1;
	static StringId entrenchAdviceId = -1;
	static StringId nukeAdviceId = -1;

	stringutils_SetStaticStringId(defendAreaAdviceId, "TACTICAL_DEFEND_AREA_ADVICE");
	stringutils_SetStaticStringId(atPeaceReadinessAdviceId, "AT_PEACE_READINESS_ADVICE");
	stringutils_SetStaticStringId(alertReadinessAdviceId, "ALERT_READINESS_ADVICE");
	stringutils_SetStaticStringId(atWarReadinessAdviceId, "AT_WAR_READINESS_ADVICE");
	stringutils_SetStaticStringId(seigeAdviceId, "TACTICAL_SEIGE_ADVICE");
	stringutils_SetStaticStringId(entrenchAdviceId, "TACTICAL_ENTRENCH_ADVICE");
	stringutils_SetStaticStringId(nukeAdviceId, "TACTICAL_TURGIDSON_ADVICE");

	Player *player_ptr = g_player[m_playerId];
	Assert(g_player[m_playerId])

	if (player_ptr == NULL)
		return -1;

	sint32 defend_goal_type = CtpAi::GetGoalDefendIndex();

	bool is_satisfied = false;
	Goal_ptr goal_ptr =Scheduler::GetScheduler(m_playerId).GetHighestPriorityGoal((GOAL_TYPE)defend_goal_type, is_satisfied);

	if (goal_ptr != NULL)
	{
		sc.AddCity(goal_ptr->Get_Target_City());
		return defendAreaAdviceId;
	}

	sint32 desired_readiness = ComputeBestMilitaryReadiness();
	if (desired_readiness != player_ptr->GetReadinessLevel())
	{
		if (desired_readiness == 0)
		{
			return atPeaceReadinessAdviceId;
		}
		else if (desired_readiness == 1)
		{
			return alertReadinessAdviceId;
		}
		else
		{
			return atWarReadinessAdviceId;
		}
	}

	sint32 seige_goal_type = CtpAi::GetGoalSeigeIndex();

	is_satisfied = true;
	goal_ptr = Scheduler::GetScheduler(m_playerId).GetHighestPriorityGoal((GOAL_TYPE)seige_goal_type, is_satisfied);

	if (goal_ptr != NULL)
	{
		sc.AddCity(goal_ptr->Get_Target_City());
		return seigeAdviceId;
	}

	sint32 num_cities = player_ptr->m_all_cities->Num();

	CellUnitList garrison;

	for (sint32 i = 0; i < num_cities; i++)
	{
		Unit city = player_ptr->m_all_cities->Access(i);
		Assert(city.IsValid() && city->GetCityData());

		g_theWorld->GetArmy(city.RetPos(), garrison);

		sint32 j;
		for (j = 0; j < garrison.Num(); j++)
		{
			if (garrison[j].IsEntrenched() ||
				garrison[j].IsEntrenching())
				break;
		}

		if (j == garrison.Num())
		{
			sc.AddCity(city);
			return entrenchAdviceId;
		}
	}

	sint32 launch_target = Diplomat::GetDiplomat(m_playerId).GetNuclearLaunchTarget();
	if (launch_target >= 0)
	{
		sc.AddPlayer(launch_target);
		return nukeAdviceId;
	}

	return -1;
}

/// Information about a trade route
struct GoodsRoute
{
	GoodsRoute()
	:
		m_cost              (0),
		m_resource          (CTPRecord::INDEX_INVALID),
		m_sourceCity        (),
		m_destinationCity   (),
		m_valuePerCaravan	(0.0)
	{};

	bool operator < (const GoodsRoute & rval) const // similar to https://thispointer.com/c-how-to-sort-a-list-of-objects-with-custom-comparator-or-lambda-function/
	{
	// sort on higest m_valuePerCaravan (best profit in total, i.e. not wasting caravans)
		return (m_valuePerCaravan < rval.m_valuePerCaravan)
		    || ((m_valuePerCaravan == rval.m_valuePerCaravan) && (m_cost > rval.m_cost)); // if valuePerCaravan equal, compare higher cost
/* sort based on higest (gold) value (bad, since more profit in total could be made with the caravans available)
		return (m_value < rval.m_value)
		    || ((m_value == rval.m_value) && (m_cost > rval.m_cost));
*/
	};

	/// Total yield
	// sint32  m_value; // not needed with m_valuePerCaravan
	/// Number of required caravans
	sint32  m_cost;
	/// Traded good
	sint32  m_resource;
	/// Supplier city
	Unit    m_sourceCity;
	/// Buyer city
	Unit    m_destinationCity;
	/// Yield per caravan
	double  m_valuePerCaravan;
};

void Governor::ManageGoodsTradeRoutes()
{
	Assert(g_player[m_playerId] != NULL);
	Player *player_ptr = g_player[m_playerId];

	sint32 unused_freight = player_ptr->GetUnusedFreight();
	sint32 total_freight = player_ptr->GetTotalFreight();
	sint32 totalRoutes = 0; // apparently there is no GetTotalRoutes() or similar, see e.g. https://github.com/civctp2/civctp2/blob/8ed4659c7075e856b9ba72e02e21facf0329ce26/ctp2_code/ui/interface/trademanager.cpp#L539
	std::list<GoodsRoute> new_routes;

	m_neededFreight = 0;

	UnitDynamicArray * city_list = player_ptr->GetAllCitiesList();
	sint32             cityCount = city_list ? city_list->Num() : 0;
	for (sint32 i = 0; i < cityCount; i++) {
	    totalRoutes += city_list->Access(i).CD()->GetTradeSourceList()->Num(); // apparently there is no GetTotalRoutes() or similar, see e.g. https://github.com/civctp2/civctp2/blob/8ed4659c7075e856b9ba72e02e21facf0329ce26/ctp2_code/ui/interface/trademanager.cpp#L539
	    }
	for (sint32 i = 0; i < cityCount; i++)
	{
		Unit & city = city_list->Access(i);

		for (sint32 g = 0; g < g_theResourceDB->NumRecords(); g++)
		{
			if(city.CD()->IsLocalResource(g)) // only consider collected goods (not bought goods)
			{
				double sellingVPC = -1; 
				sint32 sellingCost = -1;
				TradeRoute curDestRoute;

				if(city.CD()->GetResourceTradeRoute(g, curDestRoute)) // have already a route for g
				{
					sellingCost = curDestRoute->GetCost();
					sellingVPC = static_cast<double>(tradeutil_GetTradeValue(m_playerId, curDestRoute->GetDestination(), g))
					    / sellingCost; // tradeutil_GetTradeDistance returns > 1.0
				}
				else
				{
					curDestRoute.m_id = 0;
				}

				Unit maxCity;
				double bestValuePerCaravan = 0.0;
				double maxValuePerCaravan = 0.0;
				sint32 maxCost = 0;
				sint32 maxNeededFreight = 0;
				for (sint32 op = 1; op < k_MAX_PLAYERS; op++) // determine player offering highest price for good g
				{
					if (m_playerId != op) // skip all players not awailable for trading
					{
						if (!g_player[op]) // player gone/dead 
							continue;

						if (!player_ptr->HasContactWith(op)) // player not yet known
							continue;

						if (AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, op) >= 0) // no trade if at ware
							continue;

						if (Diplomat::GetDiplomat(op).GetEmbargo(m_playerId)) // no trade if embargo enacted
							continue;
					}

					for (sint32 d = 0; d < g_player[op]->m_all_cities->Num(); d++) { // check all cities if suitabel for trading
						Unit destCity = g_player[op]->m_all_cities->Access(d);

						if(!(destCity.GetVisibility() & (1 << m_playerId))) // city not visible/known
							continue;

						if(destCity.m_id == city.m_id) // exclude current city
							continue;

						if (Diplomat::GetDiplomat(m_playerId).GetTradeRoutePiracyRisk(city, destCity)) // exclude cities whose route would have a hight risk of piracy
							continue;

						if(curDestRoute.m_id != 0 &&
						    curDestRoute->GetDestination().m_id == destCity.m_id) // exclude standing trade route destination
							continue;

						const sint32 price = tradeutil_GetTradeValue(m_playerId, destCity, g); // # of gold
						const sint32 cost = tradeutil_GetTradeDistance(city, destCity); // # of caravans
						const double valuePerCaravan = static_cast<double>(price) / cost; // cost = tradeutil_GetAccurateTradeDistance returns > 1.0

						if (valuePerCaravan > bestValuePerCaravan) // determine best offer (and its cost) that would be available, to set goal for # of caravans (m_neededFreight)
						{
							maxNeededFreight = cost;
							bestValuePerCaravan = valuePerCaravan;
						}

						if ((valuePerCaravan > maxValuePerCaravan) && (cost < total_freight - totalRoutes)) // determine best offer that can be afforded (considering all currently available (used + unused) trade-units); - 1 for each existing trade route (lost when cancelling)
						{
							maxValuePerCaravan = valuePerCaravan;
							maxCity = destCity;
							maxCost = cost; // needed for bookkeeping concerning unused_freight when creating new routes
						}
					}
				} // determined player offering highest price for good g

				m_neededFreight += maxNeededFreight;

				if (!player_ptr->IsRobot()) // exlcude human players
					continue;

				// Cancelling a route always costs a caravan (see -1 below),
				// so require the alternative to beat the existing route by
				// more than a margin, not just any amount - otherwise
				// routes churn every time some other city's offer nudges
				// ahead by a hair, only to possibly flip back next turn.
				if (((sellingVPC > 0) && !city.CD()->HasResource(g) && // kill existing routes if lower in value but only if the good is NOT available multiple times within the city influence
					(maxValuePerCaravan > sellingVPC * (1.0 + g_theConstDB->Get(0)->GetTradeRouteBetterOfferMargin()))) ||
					(curDestRoute.m_id != 0 && Diplomat::GetDiplomat(m_playerId). // or piracy risk is high, i.e. pirated too often
					GetTradeRoutePiracyRisk(city, curDestRoute->GetDestination())))
				{
					unused_freight += curDestRoute->GetCost() - 1; // - 1 because one caravan is lost when killing a route

					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute,
						GEA_TradeRoute, curDestRoute.m_id,
						GEA_Int, CAUSE_KILL_TRADE_ROUTE_BETTER_OFFER,
						GEA_End);
					if(g_network.IsClient()) {
						g_network.SendAction(new NetAction(NET_ACTION_CANCEL_TRADE_ROUTE, (uint32)curDestRoute));
					}
				}

				if ((maxValuePerCaravan > 0) && ((sellingVPC < 0) || (sellingVPC < maxValuePerCaravan))) // if there is an offer (maxValuePerCaravan > 0) && (no route for good so far (sellingVPC < 0) or better offer (sellingVPC < maxValuePerCaravan))
				{
					GoodsRoute new_route;
					new_route.m_sourceCity      = city; // needed for route creation
					new_route.m_destinationCity = maxCity; // needed for route creation
					new_route.m_cost            = maxCost; // needed for bookkeeping when creating new routes
					new_route.m_resource        = g; // needed for route creation
					new_route.m_valuePerCaravan = maxValuePerCaravan; // needed for sorting

					new_routes.push_back(new_route);
				}
			}
		}
	}

	m_neededFreight -= total_freight - totalRoutes; // cravans needed to sell all goods available (with an offer); subtract those that already exist (and would be available if existing routes got killed); - 1 for each existing trade route
	if(m_neededFreight < 0) // m_neededFreight should be >= 0
	    m_neededFreight= 0;

	new_routes.sort(); // sort routes according to m_value (or m_valuePerCaravan #if defined(USE_VALUE_PER_CARAVAN))
	new_routes.reverse(); // revert sort order to start with higest m_valuePerCaravan

	//// create new routes
	for
	(
		std::list<GoodsRoute>::iterator route_iter = new_routes.begin();
		(route_iter != std::next(new_routes.begin(), new_routes.size() / 4)) && (unused_freight > 0); // only iterate over first quater (i.e. use quantile as threshold) to not waste cravans on low VPC instead save them for next turn && if unused caravans are still available
		++route_iter
	)
	{
		if(route_iter->m_cost <= unused_freight) // create route if enough unused caravans are available
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood, // if the good is already sold and not available multiple times (i.e. city.CD()->HasResource(g) == false), this will kill a standing route of good g from the source city due to https://github.com/civctp2/civctp2/blob/45cd01bb9f54502c2dfa13e8acd40558fb2af7c0/ctp2_code/gs/gameobj/Player.cpp#L3025-L3035
			                       GEA_Int,         route_iter->m_resource,
			                       GEA_City,        route_iter->m_sourceCity,
			                       GEA_City,        route_iter->m_destinationCity,
			                       GEA_End
			                      );
			unused_freight -= route_iter->m_cost; // subtract caravans now in use
		}
	}
}

const UnitRecord * Governor::GetDBUnitRec(sint32 type) const
{
	if(Player * player = g_player[m_playerId])
	{
		return g_theUnitDB->Get(type, player->GetGovernmentType());
	}
	else
	{
		return g_theUnitDB->Get(type);
	}
}

void Governor::RebuildCapitol() const
{
	if(m_playerId == 0)
	{
		// Barbarians don't need a capitol
		return;
	}

	Player * player_ptr = g_player[m_playerId];

	MapPoint pos;
	if(player_ptr->GetCapitolPos(pos))
	{
		return; // In that case no new capitol needed.
	}

	sint32 num_cities = player_ptr->m_all_cities->Num();

	if(num_cities <= 0)
	{
		// Leave if we don't have a city
		return;
	}

	sint32 type = -1;
	for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++)
	{
		if(g_theBuildingDB->Get(b, player_ptr->GetGovernmentType())->GetCapitol())
		{
			type = b;
			break;
		}
	}

	if(type < 0)
	{
		// Cannot build new capitol
		return;
	}

	Unit newCapital(0);
	sint32 lastProdMax = 0;

	for(sint32 i = 0; i < num_cities; i++)
	{
		Unit city = player_ptr->m_all_cities->Access(i);
		Assert(city.IsValid() && city->GetCityData());

		BuildNode* node = city.CD()->GetBuildQueue()->GetHead();
		if
		  (
		       node != NULL
		    && node->m_category == k_GAME_OBJ_TYPE_IMPROVEMENT
		    && node->m_type     == type
		  )
		{
			// A city is already rebuilding the Capitol, so no action is needed.
			return;
		}

		if
		  (
		      g_theWorld->GetCell(city.RetPos())->GetNumUnits() > 0
		   && city->CanBuildBuilding(type)
		  )
		{
			sint32 prod = city->GetNetCityProduction();

			if(prod > lastProdMax)
			{
				lastProdMax = prod;
				newCapital = city;
			}
		}
	}

	if(newCapital.m_id != 0)
	{
		newCapital.CD()->GetBuildQueue()->Clear();
		newCapital->BuildImprovement(type);
	}
}
