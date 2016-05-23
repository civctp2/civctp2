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
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
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
// - Fixed AI city rank calculation. (9-Nov-2009 Martin Gühmann)
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
#include "c3math.h"
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
#include "World.h"

#if defined(_DEBUG) || defined(USE_LOGGING)
#include "Timer.h"
#endif

extern CityAstar    g_city_astar;

namespace
{
	sint32 const            MAX_DISTANCE                = 0x7fffffff;
	Governor const          UniqueInvalidGovernor       = Governor(PLAYER_UNASSIGNED);
	/// Value for a trade route not requiring caravans (should not occur)
	double const            VALUE_FREE_LUNCH 	        =
		std::numeric_limits<double>::max();
}

Governor const &            Governor::INVALID           = UniqueInvalidGovernor;
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
		s_theGovernors[i].SetPlayerId(i);
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::LoadAll
//
// Description: Restore the Governor data from an archived stream
//
// Parameters : archive         : stream to restore from
//
// Globals    : s_theGovernors  : updated
//
// Returns    : -
//
// Remark(s)  : static function
//              Assumption: The size of s_theGovernors has been updated
//                          (read from stream) before calling this function.
//
//----------------------------------------------------------------------------
void Governor::LoadAll(CivArchive & archive)
{
	for (size_t i = 0; i < s_theGovernors.size(); ++i)
	{
		s_theGovernors[i].Load(archive);
	}
}

//----------------------------------------------------------------------------
//
// Name       : Governor::SaveAll
//
// Description: Save the Governor data to an archived stream
//
// Parameters : archive         : stream to save to
//
// Globals    : s_theGovernors  : input (not modified)
//
// Returns    : -
//
// Remark(s)  : static function
//              Assumption: The size of s_theGovernors has been saved
//                          (written to stream) before calling this function.
//
//----------------------------------------------------------------------------
void Governor::SaveAll(CivArchive & archive)
{
	for (size_t i = 0; i < s_theGovernors.size(); ++i)
	{
		s_theGovernors[i].Save(archive);
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
	m_neededFreight             (0.0),
	m_tileImprovementGoals      (),
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

void Governor::Load(CivArchive & archive)
{
}

void Governor::Save(CivArchive & archive) const
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

		sint32 const newCityLimit = rec->GetTooManyCitiesThreshold();
		if (player_ptr->GetNumCities() > newCityLimit)
		{
			sint32 const oldCityLimit =
				strategy.GetGovernment(player_ptr->GetGovernmentType())->
					GetTooManyCitiesThreshold();

			if (newCityLimit > oldCityLimit)
			{
				// Do consider upgrading, to improve the situation
			}
			else
			{
				continue;
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
// Name       : Governor::NormalizeSliders
//
// Description: Sets sliders back into range if there are out of range.
//
// Parameters : sliders_setting: Filled with the current slider settings.
//
// Globals    : g_player: List of players
//
// Returns    : -
//
// Remark(s)  : Strange method, why setting sliders back into range, instead
//              making sure that the sliders stay in range.
//              No more used.
//
//----------------------------------------------------------------------------
void Governor::NormalizeSliders(SlidersSetting & sliders_setting) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	//Added casts
	if(player_ptr->GetWorkdayExpectation() - sliders_setting.m_deltaProduction > 2)
		sliders_setting.m_deltaProduction = -2 + static_cast<sint32>(player_ptr->GetWorkdayExpectation());
	else if(player_ptr->GetWorkdayExpectation() - sliders_setting.m_deltaProduction < -2)
		sliders_setting.m_deltaProduction = 2 + static_cast<sint32>(player_ptr->GetWorkdayExpectation());

	if(player_ptr->GetWagesExpectation() - sliders_setting.m_deltaGold > 2)
		sliders_setting.m_deltaGold = -2 + static_cast<sint32>(player_ptr->GetWagesExpectation());
	else if(player_ptr->GetWagesExpectation() - sliders_setting.m_deltaGold < -2)
		sliders_setting.m_deltaGold = 2 + static_cast<sint32>(player_ptr->GetWagesExpectation());

	if(player_ptr->GetRationsExpectation() - sliders_setting.m_deltaFood > 2)
		sliders_setting.m_deltaFood = -2 + static_cast<sint32>(player_ptr->GetRationsExpectation());
	else if(player_ptr->GetRationsExpectation() - sliders_setting.m_deltaFood < -2)
		sliders_setting.m_deltaFood = 2 + static_cast<sint32>(player_ptr->GetRationsExpectation());
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
//              update_cities:   Whether the cities should be updated.
//
// Globals    : g_player: List of players
//
// Returns    : sint32:   The happiness delta over all cities.
//
// Remark(s)  : Happiness delta ove all cities is very odd. Actual you are
//              only interested whether the cities are happy enough.
//              Consequently this isn't used.
//
//----------------------------------------------------------------------------
sint32 Governor::SetSliders(const SlidersSetting & sliders_setting, const bool & update_cities) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	//Added casts
	player_ptr->SetWorkdayLevel(static_cast<sint32>(player_ptr->GetWorkdayExpectation()) - sliders_setting.m_deltaProduction);
	player_ptr->SetWagesLevel  (static_cast<sint32>(player_ptr->GetWagesExpectation())   - sliders_setting.m_deltaGold);
	player_ptr->SetRationsLevel(static_cast<sint32>(player_ptr->GetRationsExpectation()) - sliders_setting.m_deltaFood);

	if(!update_cities)
		return 0;

	UnitDynamicArray * city_list = player_ptr->GetAllCitiesList();
	sint32             cityCount = city_list ? city_list->Num() : 0;

	/// @todo Find out if "total delta" is a good measure
	double total_delta_happiness = 0;
	for (sint32 i = 0; i < cityCount; i++)
	{
		CityData * city = city_list->Access(i)->GetCityData();
		double old_happiness = city->GetHappiness();

		//Added by Martin Gühmann to take specialists into account.
		//Well this has an effect but the AI seems to perform worse with it.
		//Right direction but more debug work is needed.
		AssignPopulation(city);

		// Force happiness recalculation as crime losses depend on happiness.
		sint32 gold;
		city->CalcHappiness(gold, false);

		city->ProcessAllResources();

		total_delta_happiness += (city->GetHappiness() - old_happiness);
	}

	return static_cast<sint32>(total_delta_happiness);
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetSliders
//
// Description: Gets the current slider settings.
//
// Parameters : sliders_setting: Filled with the current slider settings.
//
// Globals    : g_player: List of players
//
// Returns    : -
//
// Remark(s)  : Only used by GetSlidersAdvice and this isn't used.
//
//----------------------------------------------------------------------------
void Governor::GetSliders(SlidersSetting & sliders_setting) const
{
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	sliders_setting.m_deltaProduction =
		static_cast<sint32>(player_ptr->GetWorkdayExpectation() - player_ptr->GetUnitlessWorkday());

	sliders_setting.m_deltaGold =
		static_cast<sint32>(player_ptr->GetWagesExpectation() - player_ptr->GetUnitlessWages());

	sliders_setting.m_deltaFood =
		static_cast<sint32>(player_ptr->GetRationsExpectation() - player_ptr->GetUnitlessRations());
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeMinimumSliders
//
// Description: Finds the minimum slider settings whatever this means.
//
// Parameters : sliders_setting: Filled with slider settings.
//
// Globals    : -
//
// Returns    : bool: Somewhat confusing.
//
// Remark(s)  : Removed
//
//----------------------------------------------------------------------------
bool Governor::ComputeMinimumSliders( SlidersSetting & sliders_setting ) const
{
	bool production_test;
	bool gold_test;
	bool food_test;
	bool happiness_test;
	SlidersSetting tmp_sliders_setting;
	sint32 prod, gold, food;

	bool found = TestSliderSettings(sliders_setting,
	                                production_test,
	                                gold_test,
	                                food_test,
	                                happiness_test,
	                                prod, gold, food);

	if(found)
		return false;

	SlidersSetting orig_sliders_setting = sliders_setting;

	bool changed = true;
#if defined(_DEBUG)
	sint32 loop_test = 0;
#endif
	while(found == false && changed == true)
	{
#if defined(_DEBUG)
		Assert(loop_test < 10000);
		loop_test++;
#endif
		tmp_sliders_setting = sliders_setting;

		found = TestSliderSettings(sliders_setting,
		                           production_test,
		                           gold_test,
		                           food_test,
		                           happiness_test,
		                           prod, gold, food);
		changed = false;

		if(happiness_test == false)
		{
			if(sliders_setting.m_optimizeProduction == false)
			{
				sliders_setting.m_deltaProduction--;
			}
			else if(sliders_setting.m_optimizeGold == false)
			{
				sliders_setting.m_deltaGold--;
			}
			else if( sliders_setting.m_optimizeFood == false)
			{
				sliders_setting.m_deltaFood--;
			}
			else
			{
				Assert(false);
			}
		}

		if((sliders_setting.m_deltaProduction < 0 )
		&& (production_test == false)
		){
			sliders_setting.m_deltaProduction++;
			sliders_setting.m_optimizeProduction = true;
		}
		if((sliders_setting.m_deltaGold < 0)
		&& (gold_test == false)
		){
			sliders_setting.m_deltaGold++;
			sliders_setting.m_optimizeGold = true;
		}
		if((sliders_setting.m_deltaFood < 0)
		&& (food_test == false)
		){
			sliders_setting.m_deltaFood++;
			sliders_setting.m_optimizeFood = true;
		}

		NormalizeSliders(sliders_setting);
		changed = (sliders_setting != tmp_sliders_setting );
	}

	found = TestSliderSettings(sliders_setting,
	                           production_test,
	                           gold_test,
	                           food_test,
	                           happiness_test,
	                           prod, gold, food);

	return !found || (sliders_setting != orig_sliders_setting);
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeBestSliders
//
// Description: Finds the best slider settings for the player.
//
// Parameters : sliders_setting: Filled with the best slider settings.
//
// Globals    : -
//
// Returns    : bool: Whether it was possible to find the best slider settings.
//
// Remark(s)  : Seems to do some shit, actual not worth to be understand.
//
//----------------------------------------------------------------------------
bool Governor::ComputeBestSliders(SlidersSetting & sliders_setting) const
{
	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	bool config_found = false;
	bool found;
	SlidersSetting last_sliders_setting = sliders_setting;

	for(sint32 i = 0; i < strategy.GetNumSliderElement(); i++)
	{
		// Slider elements not very usefull, too rigid.
		// Better preferences, in which order to optimize sliders.
		const StrategyRecord::SliderElement *elem = strategy.GetSliderElement(i);

		if (elem->GetProduction())
		{
			sliders_setting.m_optimizeProduction = true;
			sliders_setting.m_deltaProduction = elem->GetDelta();

			if(sliders_setting.m_deltaGold <= 0)
			{
				sliders_setting.m_deltaGold = -1 * elem->GetDelta();
			}
			if(sliders_setting.m_deltaFood <= 0)
			{
				sliders_setting.m_deltaFood = -1 * elem->GetDelta();
			}

			found = FitSlidersToCities( sliders_setting );

			sliders_setting.m_optimizeProduction = false;
		}
		else if (elem->GetGold())
		{
			sliders_setting.m_optimizeGold = true;
			sliders_setting.m_deltaGold = elem->GetDelta();

			if(sliders_setting.m_deltaProduction <= 0)
			{
				sliders_setting.m_deltaProduction = -1 * elem->GetDelta();
			}
			if(sliders_setting.m_deltaFood <= 0)
			{
				sliders_setting.m_deltaFood = -1 * elem->GetDelta();
			}

			found = FitSlidersToCities( sliders_setting );

			sliders_setting.m_optimizeGold = false;
		}
		else if (elem->GetFood())
		{
			sliders_setting.m_optimizeFood = true;
			sliders_setting.m_deltaFood = elem->GetDelta();

			if(sliders_setting.m_deltaProduction <= 0)
			{
				sliders_setting.m_deltaProduction = -1 * elem->GetDelta();
			}
			if(sliders_setting.m_deltaGold <= 0)
			{
				sliders_setting.m_deltaGold = -1 * elem->GetDelta();
			}

			found = FitSlidersToCities( sliders_setting );

			sliders_setting.m_optimizeFood = false;
		}
		else
		{
			Assert(false);
			return false;
		}

		if (found)
		{
			config_found            = true;
			last_sliders_setting    = sliders_setting;
		}
		else
		{
			sliders_setting = last_sliders_setting;
			break;
		}
	}

	return config_found;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::FitSlidersToCities
//
// Description: Modifies the given slider settings so that they fit
//
// Parameters : sliders_setting: Filled with the best acceptable slider settings.
//                               On the base of the given slider settings.
//
// Globals    : -
//
// Returns    : bool: Whether it was possible to find the best slider settings.
//
// Remark(s)  : Seems to do some shit, actual not worth to be understand.
//
//----------------------------------------------------------------------------
bool Governor::FitSlidersToCities( SlidersSetting & sliders_setting ) const
{
	Assert(m_playerId >= 0);
	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	bool production_test;
	bool gold_test;
	bool food_test;
	bool happiness_test;
	sint32 prod, gold, food;

	NormalizeSliders(sliders_setting);

	bool found = false;
	bool changed = true;

#if defined(_DEBUG)
	sint32 loop_test = 0;
#endif
	while (!found && changed)
	{
#if defined(_DEBUG)
		Assert(loop_test < 10000);
		loop_test++;
#endif

		found = TestSliderSettings(sliders_setting,
		                           production_test,
		                           gold_test,
		                           food_test,
		                           happiness_test,
		                           prod, gold, food);
		changed = false;

		if(happiness_test == false)
		{
			if(sliders_setting.m_optimizeProduction == true
			&& sliders_setting.m_deltaProduction > 0
			){
				sliders_setting.m_deltaProduction--;
				changed = true;
			}
			else if(sliders_setting.m_optimizeGold == true
			&&      sliders_setting.m_deltaGold > 0
			){
				sliders_setting.m_deltaGold--;
				changed = true;
			}
			else if(sliders_setting.m_optimizeFood == true
			&&      sliders_setting.m_deltaFood > 0
			){
				sliders_setting.m_deltaFood--;
				changed = true;
			}
			else
			{
			}
		}
		if((sliders_setting.m_deltaProduction < 0)
		&& (production_test == false )
		){
			sliders_setting.m_deltaProduction++;
			changed = true;
		}
		if((sliders_setting.m_deltaGold < 0 )
		&& (gold_test == false )
		){
			sliders_setting.m_deltaGold++;
			changed = true;
		}
		if((sliders_setting.m_deltaFood < 0)
		&& (food_test == false)
		){
			sliders_setting.m_deltaFood++;
			changed = true;
		}

		while((sliders_setting.m_deltaProduction +
		       sliders_setting.m_deltaGold +
		       sliders_setting.m_deltaFood) < 0
		){
			if(sliders_setting.m_deltaProduction < 0)
			{
				sliders_setting.m_deltaProduction++;
				changed = true;
			}
			else if(sliders_setting.m_deltaGold < 0)
			{
				sliders_setting.m_deltaGold++;
				changed = true;
			}
			else if(sliders_setting.m_deltaFood < 0)
			{
				sliders_setting.m_deltaFood++;
				changed = true;
			}
			found = false;
		}
	}

	return found;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::TestSliderSettings
//
// Description: Checks if the given slider settings are enough to support
//              the empire.
//
// Parameters : sliders_setting:  The slider settings to be tesed.
//              production_test:  Indicates after function execution whether
//                                new slider settings allow to support units.
//              gold_test:        Indicates after function execution whether
//                                new slider settings allow to pay wages and
//                                building upkeep or if building upkeep can't
//                                be paid whether deficit spending is not to high.
//              food_test:        Indicates after function execution whether
//                                new slider settings allow to feed each city.
//              happiness_test:   Indicates after function execution whether
//                                new slider settings allow each city to stay
//                                above the minimum happiness level.
//              total_production: Filled with empire production (military support,
//                                public works and production for buildings)
//              total_gold:       Filled with gold surplus
//              total_food:       Filled with food surplus
//
// Globals    : -
//
// Returns    : bool:             Whether production_test and gold_test and
//                                food_test and happiness_test are true.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool Governor::TestSliderSettings(const SlidersSetting & sliders_setting,
                                  bool   & production_test,
                                  bool   & gold_test,
                                  bool   & food_test,
                                  bool   & happiness_test,
                                  sint32 & total_production,
                                  sint32 & total_gold,
                                  sint32 & total_food) const
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
	Timer t2;

	t1.start();

	DPRINTF(k_DBG_GOVERNOR, ("\n"));
	DPRINTF(k_DBG_GOVERNOR, ("// TEST SLIDER SETTINGS -- Turn    %d\n", g_player[m_playerId]->GetCurRound()));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Player  %d\n", m_playerId));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Workday %d\n", sliders_setting.m_deltaProduction));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Wages   %d\n", sliders_setting.m_deltaGold));
	DPRINTF(k_DBG_GOVERNOR, ("//                         Rations %d\n", sliders_setting.m_deltaFood));
#endif

	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	double deficit_spending;
	sint32 min_happiness;
	double max_wage_percent;

	strategy.GetDeficitSpending(deficit_spending);
	strategy.GetMinimumHappiness(min_happiness);
	strategy.GetMaximumWagePercent(max_wage_percent);

	sint32 total_gold_cost = 0;
	sint32 gross_gold = 0;
	total_food = 0;
	total_production = 0;
	total_gold = 0;

	food_test = true;
	happiness_test = true;

	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	SetSliders(sliders_setting, false);

	UnitDynamicArray *  city_list   = player_ptr->GetAllCitiesList();
	sint32              num_cities  = city_list ? city_list->Num() : 0;

	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
#if defined(_DEBUG) || defined(USE_LOGGING)
		t2.start();
#endif
		Unit const & city_unit = city_list->Get(city_index);

		Assert(city_unit.m_id != 0);
		Assert(city_unit->GetCityData());
		CityData * city = city_unit->GetCityData();

		//Added by Martin Gühmann to take specialists into account.
		//Well this has an effect but the AI seems to perform worse with it.
		//Right direction but more debug work is needed.
		AssignPopulation(city);

#if defined(_DEBUG) || defined(USE_LOGGING)
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment = %f ms\n", t2.getElapsedTimeInMilliSec()));
#endif

		// Force happiness recalculation as crime losses depend on happiness.
		sint32 gold;
		city->CalcHappiness(gold, false);

		city->ProcessAllResources();

#if defined(_DEBUG) || defined(USE_LOGGING)
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and resources = %f ms\n", t2.getElapsedTimeInMilliSec()));
#endif

		double new_happiness = city->GetHappiness();
		if(new_happiness < min_happiness
		|| city->EntertainerCount() == city->PopCount()
		){
			happiness_test = false;
		}

		total_food += city->GetNetCityFood();
		total_production += city->GetNetCityProduction();

		total_gold_cost += city->GetWagesNeeded();
		total_gold += city->GetNetCityGold();
		gross_gold += city->GetGrossCityGold();

		if(city->GetNetCityFood() < 0)
		{
			food_test = false;
		}

#if defined(_DEBUG) || defined(USE_LOGGING)
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city = %f ms\n", t2.getElapsedTimeInMilliSec()));
		t2.stop();
#endif
	}

	total_production += player_ptr->GetProductionFromFranchises();
	production_test = (total_production >= player_ptr->GetReadinessCost());

	double wages_percent = static_cast<double>(total_gold_cost) / static_cast<double>(gross_gold);
	gold_test = (wages_percent <= max_wage_percent);

	sint32 player_gold = player_ptr->GetGold();
	if(total_gold < 0
	&& total_gold + player_gold < 0
	&& total_gold > player_gold * -deficit_spending
	){
		gold_test = false;
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time = %f ms\n", t1.getElapsedTimeInMilliSec()));
	DPRINTF(k_DBG_GOVERNOR, ("\n"));
	t1.stop();
#endif

	return (production_test && gold_test && food_test && happiness_test);
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetSlidersAdvice
//
// Description: Gets the ID of the slider message for the best slider positions.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : StringID: The ID of the string that contains the message
//                        that tells the player which slider setting to use.
//
// Remark(s)  : Unused and shouldn't be used as it modifies all the cities.
//
//----------------------------------------------------------------------------
StringId Governor::GetSlidersAdvice() const
{
	static StringId growthProductionStringId = -1;
	static StringId scienceProductionStringId = -1;
	static StringId expectationStringId = -1;
	static StringId happinessGoldAdviceId = -1;
	static StringId happinessFoodAdviceId = -1;
	static StringId happinessWorkdayAdviceId = -1;

	stringutils_SetStaticStringId(growthProductionStringId, "SLIDERS_INCREASE_GROWTH_WITH_PRODUCTION");
	stringutils_SetStaticStringId(scienceProductionStringId, "SLIDERS_INCREASE_SCIENCE_WITH_PRODUCTION");
	stringutils_SetStaticStringId(expectationStringId, "SLIDERS_MAINTAIN_EXPECTATION_ADVICE");
	stringutils_SetStaticStringId(happinessGoldAdviceId, "SLIDERS_INCREASE_HAPPINESS_WITH_GOLD_ADVICE");
	stringutils_SetStaticStringId(happinessFoodAdviceId, "SLIDERS_INCREASE_HAPPINESS_WITH_FOOD_ADVICE");
	stringutils_SetStaticStringId(happinessWorkdayAdviceId, "SLIDERS_INCREASE_HAPPINESS_WITH_WORKDAY_ADVICE");

	StringId adviceId = -1;

	SlidersSetting current_sliders_setting;
	GetSliders(current_sliders_setting);

	SlidersSetting new_sliders_setting;
	if (ComputeMinimumSliders(new_sliders_setting))
	{
		if (new_sliders_setting.m_deltaGold < 0)
		{
			adviceId = happinessGoldAdviceId;
		}
		else if (new_sliders_setting.m_deltaFood < 0)
		{
			adviceId = happinessFoodAdviceId;
		}
		else if (new_sliders_setting.m_deltaProduction < 0)
		{
			adviceId = happinessWorkdayAdviceId;
		}
	}
	else
	{
		if (Governor::GetGovernor(m_playerId).ComputeBestSliders(new_sliders_setting))
		{
			if (new_sliders_setting.m_deltaProduction < 0)
			{
				if (new_sliders_setting.m_deltaFood > 0)
				{
					adviceId = growthProductionStringId;
				}
				else if (new_sliders_setting.m_deltaGold > 0)
				{
					adviceId = scienceProductionStringId;
				}
			}
		}
		else
		{
			adviceId = expectationStringId;
		}
	}

	return adviceId;
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

	while( !ProdSliderReachedMin(sliders_setting)
	||     !GoldSliderReachedMin(sliders_setting)
	||     !FoodSliderReachedMin(sliders_setting)
	){
		TestSliderSettings(sliders_setting, slider_tests);
		value     =      slider_tests.GetValue();

		if(!ProdSliderReachedMin(sliders_setting))
		{
			prod_sliders_setting = sliders_setting;
			prod_sliders_setting.m_deltaProduction--;
			TestSliderSettings(prod_sliders_setting, prod_slider_tests);
			valueProd = prod_slider_tests.GetValue();
		}
		else
		{
			valueProd = std::numeric_limits<sint32>::min(); // Should be lower than everything else
		}

		if(!GoldSliderReachedMin(sliders_setting))
		{
			gold_sliders_setting = sliders_setting;
			gold_sliders_setting.m_deltaGold--;
			TestSliderSettings(gold_sliders_setting, gold_slider_tests);
			valueGold = gold_slider_tests.GetValue();
		}
		else
		{
			valueGold = std::numeric_limits<sint32>::min();
		}

		if(!FoodSliderReachedMin(sliders_setting))
		{
			food_sliders_setting = sliders_setting;
			food_sliders_setting.m_deltaFood--;
			TestSliderSettings(food_sliders_setting, food_slider_tests);
			valueFood = food_slider_tests.GetValue();
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
		}
		// Real equal need a better solution
		else if(valueGold >= value
		&&      valueGold >= valueProd
		&&      valueGold >= valueFood
		&&      gold_slider_tests.m_goldTest
		){
			sliders_setting = gold_sliders_setting;
		}
		// Real equal need a better solution
		else if(valueFood >= value
		&&      valueFood >= valueProd
		&&      valueFood >= valueGold
		&&      food_slider_tests.m_foodTest
		){
			sliders_setting = food_sliders_setting;
		}
		else{
			// the values aren't satisfying we have to select one other setting.
			// Real equal need a better solution
			if(valueProd >= value
			&& valueProd >= valueGold
			&& valueProd >= valueFood
			){
				sliders_setting = prod_sliders_setting;
			}
			// Real equal need a better solution
			else if(valueGold >= value
			&&      valueGold >= valueProd
			&&      valueGold >= valueFood
			){
				sliders_setting = gold_sliders_setting;
			}
			// Real equal need a better solution
			else if(valueFood >= value
			&&      valueFood >= valueProd
			&&      valueFood >= valueGold
			){
				sliders_setting = food_sliders_setting;
			}
			else{
				if(!FoodSliderReachedMin(sliders_setting)){
					sliders_setting = food_sliders_setting;
				}
				else if(!ProdSliderReachedMin(sliders_setting)){
					sliders_setting = prod_sliders_setting;
				}
				else if(!GoldSliderReachedMin(sliders_setting)){
					sliders_setting = gold_sliders_setting;
				}
				else{
					Assert(false);
					break;
				}
			}
		}
	}
}

bool Governor::AddRoadPriority(Path & path, const double & priority_delta)
{
	double bonus;
	Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy().GetRoadUtilityBonus(bonus);

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

			if(!IsInCityPairList(city_index, neighbor_index))
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
				if(AddRoadPriority(found_path, threat_rank))
					break;
			}
		}

		s_CityDistQueue.clear();
	}

	s_CityPairList.clear();
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
	ComputeRoadPriorities();    // may add to m_tileImprovementGoals

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

		sint32  bonusFood = 0;
		sint32  bonusProduction = 0;
		sint32  bonusCommerce = 0;

		for (it.Start(); !it.End(); it.Next())
		{
			if(unit.RetPos() == it.Pos())
				continue;

			Cell const * cell = g_theWorld->GetCell(it.Pos());

			if(!(cell->GetCityOwner() == unit))
				continue;

			if(cell->GetNumImprovements() > 0)
					continue;

			if(FindBestTileImprovement(it.Pos(), ti_goal, bonusFood, bonusProduction, bonusCommerce))
			{
				m_tileImprovementGoals.push_back(ti_goal);
			}
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
		TiGoalQueue::const_iterator iter = m_tileImprovementGoals.begin();
		iter != max_iter;
		++iter
	)
	{
		sint32 const needed_pw =
		       terrainutil_GetProductionCost(iter->type, iter->pos, 0);
		if (needed_pw <= avail_pw)
		{
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
// Name       : Governor::FindBestTileImprovement
//
// Description: Determines the best tile improvement for a tile.
//
// Parameters : pos:             Position of the tile on the map
//
// Globals    : g_player:        List of players in the game
//				g_theWorld:      Map information
//
// Returns    : bool:            The tile can be improved
//              goal:            Type and priority value of the tile improvement
//              bonusFood:       Added food by the selected tile improvement
//              bonusProduction: Added production(shields) by the selected tile improvement
//              bonusCommerce:   Added commerce(gold) by the selected tile improvement
//
// Remark(s)  : The information in goal is only valid when true is returned.
//
//----------------------------------------------------------------------------
bool Governor::FindBestTileImprovement(const MapPoint &pos, TiGoal &goal, sint32 &bonusFood, sint32 &bonusProduction, sint32 &bonusCommerce) const
{
	sint32 tmp_bonus;
	sint32 terrain;
	const TerrainImprovementRecord *rec;
	const TerrainImprovementRecord::Effect *effect;

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	MapAnalysis & the_map =  MapAnalysis::GetMapAnalysis();

	Unit city_owner = g_theWorld->GetCell(pos)->GetCityOwner();
	CityData* city = city_owner.GetCityData();
	Assert(city);

	double growth_rank     = the_map.GetGrowthRank    (city);
	double production_rank = the_map.GetProductionRank(city);
	double gold_rank       = the_map.GetCommerceRank  (city);
	double terr_food_rank = (g_theWorld->GetCell(pos)->GetFoodFromTerrain()) /
		(double) World::GetAvgFoodFromTerrain();
	double terr_prod_rank = (g_theWorld->GetCell(pos)->GetShieldsFromTerrain()) /
		(double) World::GetAvgShieldsFromTerrain();
	double terr_gold_rank = (g_theWorld->GetCell(pos)->GetGoldFromTerrain()) /
		(double) World::GetAvgGoldFromTerrain();
	double bonus;
	sint32 terrain_type = g_theWorld->GetCell(pos)->GetTerrainType();

	sint32 best_growth_improvement;
	sint32 best_production_improvement;
	sint32 best_gold_improvement;
	GetBestFoodProdGoldImprovement(pos,best_growth_improvement, best_production_improvement, best_gold_improvement);

	goal.pos = pos;

	goal.type = -1;

	sint32 food_ter = -1;
	sint32 prod_ter = -1;
	sint32 gold_ter = -1;

	sint32 citySize;

	if (!g_theWorld->IsGood(pos))
	{
		bool shouldTerraform = true;
		for
		(
			sint32 i = 0;
			i < g_theWorld->GetCell(pos)->GetNumDBImprovements() && shouldTerraform;
			++i
		)
		{
			rec = g_theTerrainImprovementDB->Get(g_theWorld->GetCell(pos)->GetDBImprovement(i));

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
		}
	}

	sint32 foodMissing;
	bool moreFoodNeeded = city->NeedMoreFood(bonusFood, foodMissing, true);
//	bool moreProdNeeded = city->NeedMoreProdOrGold(bonusProd, bonusGold, true);

	if((moreFoodNeeded
	|| (best_production_improvement < 0
	&&  best_gold_improvement < 0))
	&& (best_growth_improvement >= 0)
	){

		rec = g_theTerrainImprovementDB->Get(best_growth_improvement);
		effect = terrainutil_GetTerrainEffect(rec, pos);

		effect->GetBonusFood(tmp_bonus);
		bonusFood += tmp_bonus;

		effect->GetBonusProduction(tmp_bonus);
		bonusProduction += tmp_bonus;

		effect->GetBonusGold(tmp_bonus);
		bonusCommerce += tmp_bonus;

		goal.type = best_growth_improvement;

		strategy.GetImproveGrowthBonus(bonus);
		goal.utility = bonus * terr_food_rank;

		if(growth_rank < 0.2){
		    strategy.GetImproveSmallCityGrowthBonus(bonus);
			goal.utility +=  bonus * (1.0 - growth_rank);
		}

		if(g_theWorld->IsGood(pos)){
		    strategy.GetImproveGoodBonus(bonus);
			goal.utility += bonus;
		}
	}
	else if(moreFoodNeeded
	&&      food_ter >= 0
	){
		if(g_theTerrainImprovementDB->Get(food_ter)->GetTerraformTerrainIndex(terrain))
		{
			bonusFood += g_theWorld->GetCell(pos)->GetFoodFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetFoodFromTerrain();
			bonusProduction += g_theWorld->GetCell(pos)->GetShieldsFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetShieldsFromTerrain();
			bonusCommerce += g_theWorld->GetCell(pos)->GetGoldFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetGoldFromTerrain();

			goal.type = food_ter;

			if(terrain_type != terrainutil_GetDead())
			{
				strategy.GetImproveGrowthBonus(bonus);
				goal.utility = bonus * terr_food_rank;

				if(growth_rank < 0.2)
				{
				    strategy.GetImproveSmallCityGrowthBonus(bonus);
					goal.utility +=  bonus * (1.0 - growth_rank);
				}

			}
			else
			{
				goal.utility = 9999.0;
			}
		}

	}
//	else if(moreProdNeeded
//	|| best_gold_improvement < 0
	else if((terr_prod_rank > 0.2
	&&       best_production_improvement >= 0)
	||      (best_production_improvement >= 0
	&&       best_gold_improvement       <  0)
	){
		rec = g_theTerrainImprovementDB->Get(best_production_improvement);
		effect = terrainutil_GetTerrainEffect(rec, pos);

		effect->GetBonusFood(tmp_bonus);
		bonusFood += tmp_bonus;

		effect->GetBonusProduction(tmp_bonus);
		bonusProduction += tmp_bonus;

		effect->GetBonusGold(tmp_bonus);
		bonusCommerce += tmp_bonus;

		goal.type = best_production_improvement;

		strategy.GetImproveProductionBonus(bonus);
		goal.utility =  bonus * terr_prod_rank;

		if(production_rank > 0.8)
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus *	production_rank;
		}

		if(g_theWorld->IsGood(pos))
		{
			strategy.GetImproveGoodBonus(bonus);
			goal.utility += bonus;
		}
	}
	else if(//(gold_rank > 0.4)
//	&&      (best_gold_improvement >= 0)
	         best_gold_improvement >= 0
	){
		rec = g_theTerrainImprovementDB->Get(best_gold_improvement);
		effect = terrainutil_GetTerrainEffect(rec, pos);

		effect->GetBonusFood(tmp_bonus);
		bonusFood += tmp_bonus;

		effect->GetBonusProduction(tmp_bonus);
		bonusProduction += tmp_bonus;

		effect->GetBonusGold(tmp_bonus);
		bonusCommerce += tmp_bonus;

		goal.type = best_gold_improvement;

		strategy.GetImproveProductionBonus(bonus);
		goal.utility = bonus * terr_gold_rank;

		if(production_rank > 0.8)
		{
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * production_rank;
		}
		if(g_theWorld->IsGood(pos))
		{
			strategy.GetImproveGoodBonus(bonus);
			goal.utility += bonus;
		}
	}
	else if(g_theWorld->IsGood(pos) == FALSE)
	{ // Should be removed
		ERR_BUILD_INST err;
		if(terrain_type == terrainutil_GetGlacier()
		|| terrain_type == terrainutil_GetSwamp()
		|| terrain_type == terrainutil_GetTundra()
		){
			if(player_ptr->CanCreateImprovement(terrainutil_GetTerraformHillsImprovement(), pos, 0, FALSE, err))
			{
				strategy.GetImproveProductionBonus(bonus);
				goal.utility =  bonus * (1.0-production_rank);
				goal.type = terrainutil_GetTerraformHillsImprovement();
			}
		}
		else if(terrain_type == terrainutil_GetDesert())
		{
			if(player_ptr->CanCreateImprovement(terrainutil_GetTerraformGrasslandImprovement(), pos, 0, FALSE, err)){
				strategy.GetImproveGrowthBonus(bonus);
				goal.utility =  bonus * (1.0-growth_rank);
				goal.type = terrainutil_GetTerraformGrasslandImprovement();
			}
		}
	}

	// Moved to the end, so that if no terraform improvement selected
	// the dead tiles are removed.
	else if(terrain_type == terrainutil_GetDead())
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

#if defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
	// CTP1: utilisation depends on worker placement, and is either 0 or 1.
#else
	// CTP2: utilisation depends on the number of available workers and the
	// (ring) distance from the city, and may be any fraction from 0.0 to 1.0.
	if ((goal.type >= 0) && city)
	{
		city->GetPop(citySize);

		// Only decrease utility if the city has grown beyond the first ring.
		if(citySize > unitutil_GetSmallCityMaxSize())
		{
#if defined(NEW_RESOURCE_PROCESS)
			sint32 ring = city->GetRing(pos);
			goal.utility *= static_cast<double>(city->GetWorkingPeopleInRing(ring)) / static_cast<double>(city->GetRingSize(ring));
#else
			sint32 const	sqDist	= MapPoint::GetSquaredDistance(city_owner.RetPos(), pos);
			goal.utility *= city->GetUtilisationRatio(sqDist);
#endif
		}
		else
		{
			strategy.GetImproveSmallCityGrowthBonus(bonus);
			bonus        *= strategy.GetSmallCityImproveCoeff();
			goal.utility += bonus;
		}
	}
	else
	{
		goal.utility = 0.0;
	}
#endif

	return (goal.type >= 0);
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
	ERR_BUILD_INST err;
	if (player_ptr && !player_ptr->CanCreateImprovement(terr_imp_rec->GetIndex(), pos, 0, FALSE, err))
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
	sint32 tmp_bonus;
	const TerrainImprovementRecord *rec;
	const TerrainImprovementRecord::Effect *effect;

	sint32 max_bonus_food = 0;
	sint32 max_bonus_prod = 0;
	sint32 max_bonus_gold = 0;
	uint32 current_class = 0x0;
	for (sint32 index = 0; index < cell->GetNumDBImprovements(); index++)
	{
		type = cell->GetDBImprovement(index);
		rec = g_theTerrainImprovementDB->Get(type);
		effect = terrainutil_GetTerrainEffect(rec, pos);

		if (effect == NULL)
			continue;

		if (!rec->GetClassRoad() &&
			!rec->GetClassLandDetector() &&
			!rec->GetClassOceanDetector() &&
			!rec->GetClassStructure1() && !rec->GetClassStructure2())
		{
			current_class |= rec->GetClass();
		}

		if(effect->GetBonusFood(tmp_bonus)
		&& tmp_bonus > max_bonus_food
		){
			max_bonus_food = tmp_bonus;
		}

		if(effect->GetBonusProduction(tmp_bonus)
		&& tmp_bonus > max_bonus_prod
		){
			max_bonus_prod = tmp_bonus;
		}

		if(effect->GetBonusGold(tmp_bonus)
		&& tmp_bonus > max_bonus_gold
		){
			max_bonus_gold = tmp_bonus;
		}
	}

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];

	const TERRAIN_TYPES terrain_type = g_theWorld->GetTerrainType(pos);

	for (type = 0; type < g_theTerrainImprovementDB->NumRecords(); type++)
	{
		rec = g_theTerrainImprovementDB->Get(type);
		ERR_BUILD_INST err;

		if (!player_ptr->CanCreateImprovement(type, pos, 0, FALSE, err))
			continue;

		if (current_class != 0x0 && ((rec->GetClass() & current_class) == 0x0))
			continue;

		effect = terrainutil_GetTerrainEffect(rec, terrain_type);
		if(effect)
		{
			if(effect->GetBonusFood(tmp_bonus)
			&& tmp_bonus > max_bonus_food
			){
				food_imp = type;
				max_bonus_food = tmp_bonus;
			}

			if(effect->GetBonusProduction(tmp_bonus)
			&& tmp_bonus > max_bonus_prod
			){
				prod_imp = type;
				max_bonus_prod = tmp_bonus;
			}

			if(effect->GetBonusGold(tmp_bonus)
			&& tmp_bonus > max_bonus_gold
			){
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

	UnitDynamicArray * city_list = g_player[m_playerId]->GetAllCitiesList();
	for (sint32 i = 0; i < city_list->Num(); i++) // city_list->Num() returns a sint32
	{
		CityData * city = city_list->Access(i)->GetCityData();

		if (city && city->GetUseGovernor() && (city->GetSizeIndex() > 0))
		{
			AssignPopulation(city);
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

		// Test whether the element is applicable for the current rank
		double bottom_value;
		double top_value;
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
void Governor::AssignPopulation(CityData *city) const
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	Timer t1;
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
	sint32 vgs;
	city->CalcHappiness(vgs, TRUE);

	/////////////////////////////////////
	// Get maximum percent of specialists
	// Should be removed in the end.
	double specialists_percent = pop_assignment->GetSpecialists();

#if defined(_DEBUG) || defined(USE_LOGGING)
	t1.start();
#endif

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

#if defined(_DEBUG) || defined(USE_LOGGING)
	t1.stop();
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
	CityData * tmp_city = new CityData(city);
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
		tmp_city->CollectResourcesFinally();
		tmp_city->ProcessProduction(true);
		prev_result = tmp_city->GetGrossCityProduction();

		tmp_city->ChangeSpecialists(POP_LABORER, count);
		tmp_city->CollectResourcesFinally();
		tmp_city->ProcessProduction(true);

		if(tmp_city->GetGrossCityProduction() > prev_result)
		{
			city->ChangeSpecialists(POP_LABORER, count);
			city->ProcessProduction(true);
		}
		else if(tmp_city->GetGrossCityProduction() < prev_result)
		{
			delta = (-city->LaborerCount());
			city->ChangeSpecialists(POP_LABORER, delta);
		}

		delta = city->LaborerCount() - tmp_city->LaborerCount();
		tmp_city->ChangeSpecialists(POP_LABORER, delta );
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
		tmp_city->CollectResources();
		tmp_city->ProcessFood();
		prev_result = tmp_city->GetProducedFood();
//		DPRINTF(k_DBG_GAMESTATE, ("PrevResult: %f\n", prev_result));

		// Test situation with specialists
		tmp_city->ChangeSpecialists(POP_FARMER, count);
		tmp_city->CollectResourcesFinally();
		tmp_city->ProcessFood();

		if(tmp_city->GetProducedFood() > prev_result)
		{
			// The specialists are beneficial: employ in the real city.
			city->ChangeSpecialists(POP_FARMER, count);
		}
		else if(tmp_city->GetProducedFood() < prev_result)
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
		delta = city->FarmerCount() - tmp_city->FarmerCount();
		tmp_city->ChangeSpecialists(POP_FARMER, delta );
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
		tmp_city->CollectResourcesFinally();
		tmp_city->CollectOtherTrade(TRUE);
		prev_result = tmp_city->GetGrossCityGold();

		tmp_city->ChangeSpecialists(POP_MERCHANT, count);
		tmp_city->CollectResourcesFinally();
		tmp_city->CollectOtherTrade(TRUE);

		if(tmp_city->GetGrossCityGold() > prev_result)
		{
			city->ChangeSpecialists(POP_MERCHANT, count);
		}
		else if(tmp_city->GetGrossCityGold() < prev_result)
		{
			delta = (-city->MerchantCount());
			city->ChangeSpecialists(POP_MERCHANT, delta);
		}

		delta = city->MerchantCount() - tmp_city->MerchantCount();
		tmp_city->ChangeSpecialists(POP_MERCHANT, delta );
#endif
	}

#if !defined(NEW_RESOURCE_PROCESS)
	// Not used anymore
	delete tmp_city;
#endif

	best_specialist = city->GetBestSpecialist(POP_SCIENTIST);

	count = static_cast<sint32>
		(ceil(specialists * pop_assignment->GetScientistPercent()));

	count = (count <= scientists) ? count : scientists;
	if(count > city->WorkerCount())
	{
		count = city->WorkerCount();
	}

	//////////////////////////////////
	// First pop database index is 0
	if(best_specialist >= 0
	&& count > 0
	){
		city->ChangeSpecialists(POP_SCIENTIST, count);
	}

	best_specialist = city->GetBestSpecialist(POP_ENTERTAINER);

	////////////////////////////////
	// First pop database index is 0
	if(best_specialist >= 0)
	{
		/////////////////////////
		// Recalculate happiness
		city->CollectResourcesFinally();
		city->ProcessProduction(true);   // Recalcs also happiness, since production influences happiness

		////////////////////////////////////////////////////////////////////////
		// Retrieve minimum number of entertainers to keep the city from rioing.
		// Retrieve maximum number of entertainers needed for optimal happiness.
		sint32 min_entertainers;
		sint32 max_entertainers;
		ComputeMinMaxEntertainers(city, min_entertainers, max_entertainers);

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
		if(max_entertainers > city->WorkerCount())
		{
			max_entertainers = city->WorkerCount();
		}

		count = static_cast<sint32>
			(ceil(specialists * pop_assignment->GetEntertainerPercent()));

		count = (count >= min_entertainers ? count : min_entertainers);
		count = (count <= max_entertainers ? count : max_entertainers);

		count = (count <= specialists || count == min_entertainers ? count : specialists);

		city->ChangeSpecialists(POP_ENTERTAINER, count);
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
	DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time for popasign = %f ms\n", t1.getElapsedTimeInMilliSec()));
#endif
}

//----------------------------------------------------------------------------
//
// Name       : Governor::ComputeMinMaxEntertainers
//
// Description: Estimates the amount of entertainers needed to achieve
//              minimum and full happiness.
//
// Parameters : city: The city data of city for which the amount of
//                    entertainers should be estimated.
//              min:  Filled with the minimum amount of enteriners needed.
//              max:  Filled with the amount of entertainers needed for
//                    full happiness.
//
// Globals    : g_theConstDB: The const database.
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void Governor::ComputeMinMaxEntertainers(const CityData *city, sint32 & a_Min, sint32 & a_Max) const
{
	a_Min = 0;
	a_Max = 0;

	sint32 entertainer_type = city->GetBestSpecialist(POP_ENTERTAINER);

	if (entertainer_type < 0)
		return;

	sint32 per_pop_happiness = g_thePopDB->Get(entertainer_type)->GetHappiness();
	if (per_pop_happiness <= 0)
	{
		DPRINTF(k_DBG_GAMESTATE,
                ("Entertainer pop type: %i, happiness: %i\n", entertainer_type, per_pop_happiness)
               );
		Assert(0);
		return;
	}

	sint32 needed = g_theConstDB->Get(0)->GetRiotLevel();
	sint32 maximum = g_theConstDB->Get(0)->GetVeryHappyThreshold();
	sint32 current = static_cast<sint32>(city->GetHappiness());

	double min_delta = static_cast<double>(needed - current) /
	                     static_cast<double>(per_pop_happiness);
	double max_delta = static_cast<double>(maximum - current) /
	                     static_cast<double>(per_pop_happiness);

	if (min_delta < 0)
	{
		min_delta = floor(min_delta);
	}
	else
	{
		min_delta = ceil(min_delta);
	}

	if (max_delta < 0)
	{
		max_delta = floor(max_delta);
	}
	else
	{
		max_delta = ceil(max_delta);
	}

	a_Min = std::max<sint32>(0, city->EntertainerCount() + (sint32) min_delta);
	a_Max = std::max<sint32>(0, city->EntertainerCount() + (sint32) max_delta);
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
	MapPoint cityPos = city->GetHomeCity().RetPos();

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

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment ß = %f ms\n", t1.getElapsedTimeInMilliSec()));

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
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 0 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		part_radii_prod = city->ProcessProduction(true,
		                         grossProduction,
		                         part_radii_prod,
		                         crimeLoss,
		                         specialLoss, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 1 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		full_radii_prod = city->ProcessProduction(true,
		                         grossProduction,
		                         full_radii_prod,
		                         crimeLoss,
		                         specialLoss, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 2 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		grossFood = part_radii_food;
		city->ProcessFood(crimeLossFood, part_radii_food, grossFood, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 3 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		grossFood = full_radii_food;
		city->ProcessFood(crimeLossFood, full_radii_food, grossFood, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 4 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		city->CollectGold(full_radii_gold, specialLoss, crimeLoss, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 5 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		city->CollectGold(part_radii_gold, specialLoss, crimeLoss, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 6 = %f ms\n", t1.getElapsedTimeInMilliSec()));

		support = city->GetSupport();
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 7 = %f ms\n", t1.getElapsedTimeInMilliSec()));
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
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 8 = %f ms\n", t1.getElapsedTimeInMilliSec()));
		city->SplitScience(true, part_radii_scigold, part_radii_science, scieCrime, true);
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment 9 = %f ms\n", t1.getElapsedTimeInMilliSec()));

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
		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment A = %f ms\n", t1.getElapsedTimeInMilliSec()));

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// laborers to generate the same amount of production
		popResources = city->GetSpecialistsResources(POP_LABORER);
		if(popResources != 0 && part_size_pop > 0)
		{
			popProd = popResources * part_size_pop * city->GetBonusProdCoeff();
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

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment B = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// merchants to generate the same amount of gold
		popResources = city->GetSpecialistsResources(POP_MERCHANT);
		if(popResources != 0 && part_size_pop > 0)
		{
			popGold = popResources * part_size_pop * city->GetBonusGoldCoeff();
			city->CalcGoldLoss(true, popGold, specialLoss, crimeLoss);

			if(part_radii_gold <= popGold)
			{
				merchants += part_size;
				merchantsEff = static_cast<double>(popGold - part_radii_gold)/static_cast<double>(part_size_pop);
			}
			else
				merchants = 0;
		}

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment C = %f ms\n", t1.getElapsedTimeInMilliSec()));
		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// scientists to generate the same amount of science
		popResources = city->GetSpecialistsResources(POP_SCIENTIST);
		if(popResources != 0 && part_size_pop > 0)
		{
			popScience = popResources * part_size_pop * city->GetBonusScieCoeff();
			popScience -= city->CrimeLoss(popScience);

			if(part_radii_science <= popScience)
			{
				scientists += part_size;
				scientistsEff = static_cast<double>(popScience - part_radii_science)/static_cast<double>(part_size_pop);
			}
			else
				scientists = 0;
		}

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment D = %f ms\n", t1.getElapsedTimeInMilliSec()));
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

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment E = %f ms\n", t1.getElapsedTimeInMilliSec()));
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

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment F = %f ms\n", t1.getElapsedTimeInMilliSec()));
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

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment G = %f ms\n", t1.getElapsedTimeInMilliSec()));
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

		DPRINTF(k_DBG_GOVERNOR, ("//  elapsed time per city and pop asignment Z = %f ms\n", t1.getElapsedTimeInMilliSec()));
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
	sint32 needed_cities = (max_cities - num_cities);
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

	for (int list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		m_buildUnitList[list_num].m_perCityGarrison =  0;
		m_buildUnitList[list_num].m_desiredCount    =  0;
		m_buildUnitList[list_num].m_maximumCount    =  0;
		m_buildUnitList[list_num].m_bestType        = -1;

		sint32 desired_count                        =  0;
		sint32 garrison_count                       =  0;
		double unit_support_percent_by_type         =  0.0;

		build_list_rec = GetBuildListRecord(strategy, (BUILD_UNIT_LIST) list_num);

		switch (list_num)
		{
		case BUILD_UNIT_LIST_OFFENSE:
			(void) strategy.GetOffensiveGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetOffensiveUnitsPercent(unit_support_percent_by_type);
			break;

		case BUILD_UNIT_LIST_DEFENSE:
			(void) strategy.GetDefensiveGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetDefensiveUnitsPercent(unit_support_percent_by_type);
			break;

		case BUILD_UNIT_LIST_RANGED:
			(void) strategy.GetRangedGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison =
				static_cast<sint16>(garrison_count);
			m_buildUnitList[list_num].m_maximumGarrisonCount =
				static_cast<sint16>(garrison_count * player_ptr->GetNumCities());

			strategy.GetRangedUnitsPercent(unit_support_percent_by_type);
			break;

		case BUILD_UNIT_LIST_SEA:
			strategy.GetSeaUnitsPercent(unit_support_percent_by_type);
			break;

		case BUILD_UNIT_LIST_AIR:
			strategy.GetAirUnitsPercent(unit_support_percent_by_type);
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
					desired_count =
						static_cast<sint32>(ceil(m_neededFreight / freight_per_unit));
				}
			}
			break;

		default:

			Assert(false);
			break;
		}

		if(build_list_rec == NULL)
			continue;

		sint32 best_unit_type = ComputeBestUnitType(build_list_rec);
		m_buildUnitList[list_num].m_bestType = best_unit_type;

		switch ( list_num )
		{
		case BUILD_UNIT_LIST_OFFENSE:
		case BUILD_UNIT_LIST_DEFENSE:
		case BUILD_UNIT_LIST_RANGED:
		case BUILD_UNIT_LIST_SEA:
		case BUILD_UNIT_LIST_AIR:

			total_unit_support_by_type =
				static_cast<sint32>(total_unit_support * unit_support_percent_by_type);

			if (best_unit_type >= 0)
			{
				total_unit_support_by_type += total_unallocated_support;
				sint32 const bestUnitSupport = GetDBUnitRec(best_unit_type)->GetShieldHunger();

				if (bestUnitSupport > 0)
				{
					Assert(total_unit_support_by_type >= 0);
					m_buildUnitList[list_num].m_maximumCount =
						static_cast<sint16>
							(floor(static_cast<double>(total_unit_support_by_type) /
								   bestUnitSupport
								  )
							);

					total_unallocated_support = total_unit_support_by_type -
						m_buildUnitList[list_num].m_maximumCount * bestUnitSupport;
				}
				else
				{
					m_buildUnitList[list_num].m_maximumCount =
						static_cast<sint16>(total_unit_support_by_type);

					total_unallocated_support = total_unit_support_by_type;
				}
				m_buildUnitList[list_num].m_desiredCount =
					m_buildUnitList[list_num].m_maximumCount -
					m_currentUnitCount[best_unit_type];

				m_maximumUnitShieldCost +=
					m_buildUnitList[list_num].m_maximumCount * bestUnitSupport;

				m_currentUnitShieldCost +=
					m_currentUnitCount[best_unit_type] * bestUnitSupport;
			}
			else
			{
				total_unallocated_support += total_unit_support_by_type;
			}
			break;
		case BUILD_UNIT_LIST_SETTLER:
		case BUILD_UNIT_LIST_SEA_SETTLER:

			if (    (best_unit_type >= 0)
			     && (desired_count > m_currentUnitCount[best_unit_type])
			   )
			{
				m_buildUnitList[list_num].m_maximumCount =
					static_cast<sint16>(needed_cities);

				m_buildUnitList[list_num].m_desiredCount =
					m_buildUnitList[list_num].m_maximumCount -
					m_currentUnitCount[best_unit_type];
			}
			else
			{
				m_buildUnitList[list_num].m_maximumCount = 0;
				m_buildUnitList[list_num].m_desiredCount = 0;
			}

		case BUILD_UNIT_LIST_SLAVERY:
		case BUILD_UNIT_LIST_SPY:
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
				m_buildUnitList[list_num].m_maximumCount = desired_count + needed_transport;
				m_buildUnitList[list_num].m_desiredCount =
				    static_cast<sint16>(desired_count
				                        + needed_transport
				                        - m_currentUnitCount[best_unit_type]
				                       );
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

	m_buildUnitList[BUILD_UNIT_LIST_OFFENSE].m_garrisonCount = 0;
	m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_garrisonCount = 0;
	m_buildUnitList[BUILD_UNIT_LIST_RANGED].m_garrisonCount = 0;

	sint32 desired_offense;
	sint32 desired_defense;
	sint32 desired_ranged;
	MapPoint pos;

	for (city_index = 0; city_index < num_cities; city_index++)
	{
		unit = player_ptr->m_all_cities->Get(city_index);

		if (!unit.IsValid())
			continue;

		strategy.GetOffensiveGarrisonCount(desired_offense);
		strategy.GetDefensiveGarrisonCount(desired_defense);
		strategy.GetRangedGarrisonCount(desired_ranged);

		unit->GetPos(pos);
		CellUnitList *  units_ptr   = g_theWorld->GetArmyPtr(pos);
		sint32          unitCount   = units_ptr ? units_ptr->Num() : 0;
		for (sint32 unit_index = 0; unit_index < unitCount; unit_index++)
		{
			Unit    armyUnit    = units_ptr->Get(unit_index);

			if (armyUnit.IsValid())
			{
				if (armyUnit->GetType() == m_buildUnitList[BUILD_UNIT_LIST_OFFENSE].m_bestType)
					desired_offense--;

				if (armyUnit->GetType() == m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_bestType)
					desired_defense--;

				if (armyUnit->GetType() == m_buildUnitList[BUILD_UNIT_LIST_RANGED].m_bestType)
					desired_ranged--;
			}
		}

		Assert(unit->GetCityData());
		if ( (desired_offense <= 0) &&
			 (desired_defense <= 0) &&
			 (desired_ranged <= 0) )
		{
			unit->GetCityData()->SetGarrisonComplete(TRUE);
		}
		else
		{
			unit->GetCityData()->SetGarrisonComplete(FALSE);
		}

		if ( desired_offense > 0)
		{
			m_buildUnitList[BUILD_UNIT_LIST_OFFENSE].m_garrisonCount +=
				static_cast<sint16>(desired_offense);
		}

		if ( desired_defense > 0 )
		{
			m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_garrisonCount +=
				static_cast<sint16>(desired_defense);
		}

		if ( desired_ranged > 0 )
		{
			m_buildUnitList[BUILD_UNIT_LIST_RANGED].m_garrisonCount +=
				static_cast<sint16>(desired_ranged);
		}
	}
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
			else
			{
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
//			city->InsertCapitalization(); // How is Capitalization removed?
			city->BuildCapitalization();
			break;
		case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
			insert_ok = true;
//			city->InsertInfrastructure(); // How is Infrastructure removed?
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

				if (type >= 0)
				{
					Assert(city->CanBuildUnit(type));
					cat = k_GAME_OBJ_TYPE_UNIT;
					return;
				}
			}
			else if (elem->GetGarrisonUnitBuildList())
			{
				type = GetNeededGarrisonUnitType(city, list_num);
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

	DPRINTF(k_DBG_AI, ("Nothing valid found to build in city at (%d,%d), increase production allocated to units?\n"));
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

const BuildListSequenceRecord * Governor::GetMatchingSequence(const CityData *city, const bool human_city, StringId & advice, bool & noUnits) const
{
	Assert(city);
	if (city->GetUseGovernor() && human_city)
		return g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());

	Assert(g_player[m_playerId]);
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	double rank = 0.0;
	const StrategyRecord::BuildListSequenceElement *best_elem = NULL;
	sint32 best_priority = -99999;

	sint32 pollution = city->GetPollution();
	sint32 minPollution;

	sint32 minNumUnits;
	sint32 maxRawHappiness;

	SlidersSetting sliders_setting;
	sint32 cityRawHappiness = static_cast<sint32>(city->GetHappiness()) - city->GetHappinessFromPops();

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

		else if(elem->HasSmallCitiesMaxSize())
		{
			// Do nothing
		}

		else if(elem->HasBeforeMaxCitySize())
		{
			// Do nothing
		}

		else if(elem->HasMinPollution())
		{
			// Do nothing
		}

		else if(elem->GetAllCities())
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

		double top_value;
		if (elem->GetTop(top_value) &&
		    (rank >= 1.0 - top_value)
		   )
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
		}

		double bottom_value;
		if (elem->GetBottom(bottom_value) &&
		    (rank <= bottom_value)
		   )
		{
			best_priority = elem->GetPriority();
			best_elem     = elem;
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
		}
	}

	advice = -1;

	if (best_elem == NULL)
	{
		best_elem = strategy.GetBuildListSequenceElement(i-1);
		Assert(best_elem);
		if (best_elem == NULL)
			return g_theBuildListSequenceDB->Get(0);
	}

	noUnits = best_elem->GetNoUnits();

	(void) best_elem->GetAdvice(advice);

	return best_elem->GetBuildListSequence();
}

sint32 Governor::GetNeededUnitType(const CityData *city, sint32 & list_num) const
{
	Assert(g_player[m_playerId]);

	BUILD_UNIT_LIST max_list                = BUILD_UNIT_LIST_MAX;
	sint32          max_production          = 0;
	sint32          turns_to_build          = 9999;
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
	                                         build_settler_production_level;
//	                      || city->GetNeededGarrisonStrength() * build_transport_production_level <= city->GetCurrentGarrisonStrength();

	Scheduler & scheduler = Scheduler::GetScheduler(m_playerId);

	for (list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		if( !g_player[m_playerId]->IsRobot()
		&& (   static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SLAVERY
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPY
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

		turns_to_build = city->HowMuchLonger(needed_production);

		if
		  (
		       static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER
		    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
		  )
		{
			needed_production *= list_ref.m_maximumCount;
		}
		else
		{
			needed_production *= list_ref.m_desiredCount;
		}

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

#if defined(_DEBUG)
	UnitRecord const *	unit	= (type < 0) ? NULL : GetDBUnitRec(type);
	DPRINTF(k_DBG_GAMESTATE, ("Selected unit type: %s\n", unit ? unit->GetNameText() : "none"));
	DPRINTF(k_DBG_GAMESTATE, ("Player: %lx\n", m_playerId));
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

			double defence   = unitutil_GetPositionDefense(rec, true, pos, Unit());
//			double defence   = rec->GetDefense(); // Raw defense
			double firepower = static_cast<double>(rec->GetFirepower());
			double hitpoints = static_cast<double>(rec->GetMaxHP());

			garrisonDefence += rec->GetDefense() * firepower * hitpoints * static_cast<double>(list_ref.m_perCityGarrison);
		}
	}

	return garrisonDefence;
}

sint32 Governor::GetNeededGarrisonUnitType(const CityData * city, sint32 & list_num) const
{
	Assert( city );
	if (city->GetGarrisonComplete() && !city->GetGarrisonOtherCities())
		return CTPRecord::INDEX_INVALID;

	double garrisonStrength       = city->GetCurrentGarrisonStrength();
	double neededGarrisonStrength = MaxiumGarrisonDefence(city->GetHomeCity().RetPos());
	double garrisonComplte        = (neededGarrisonStrength > 0.0) ? garrisonStrength / neededGarrisonStrength : 1.0;

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

	Scheduler & scheduler = Scheduler::GetScheduler(m_playerId);

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
		else if ( city->GetGarrisonComplete() )
		{
			// @ToDo: Merge duplicated code
			double garrisonPercent = list_ref.m_maximumGarrisonCount > 0 ? static_cast<double>(list_ref.m_garrisonCount) / static_cast<double>(list_ref.m_maximumGarrisonCount) : 1.0;
			if
			  (
			   (
			         static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_TRANSPORT
			      || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA
			   )
			   && garrisonPercent > build_transport_production_level // This may be exposed extra
			   // This does not work so well yet
//			   && (PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(list_num)) > PercentUnbuilt(BUILD_UNIT_LIST_SETTLER)     || m_currentUnitCount[m_buildUnitList[BUILD_UNIT_LIST_SETTLER].m_bestType]     > 2)
//			   && (PercentUnbuilt(static_cast<BUILD_UNIT_LIST>(list_num)) > PercentUnbuilt(BUILD_UNIT_LIST_SEA_SETTLER) || m_currentUnitCount[m_buildUnitList[BUILD_UNIT_LIST_SEA_SETTLER].m_bestType] > 2)
			  )
			{
				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				sint32 turns_to_build = city->HowMuchLonger(needed_production);
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
			   && garrisonPercent > build_settler_production_level
			  )
			{
				if(!m_canBuildLandSettlers)
				{
					continue;
				}

				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				sint32 turns_to_build = city->HowMuchLonger(needed_production);
				needed_production *= list_ref.m_maximumCount;
				if(needed_production > 0)
				{
					max_list = (BUILD_UNIT_LIST) list_num;
					break;
				}
			}
			else if
			  (
			      static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER
			   && garrisonPercent > build_settler_production_level
			   && this->m_canBuildSeaSettlers
			  )
			{
				if(!m_canBuildSeaSettlers)
				{
					continue;
				}

				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				sint32 turns_to_build = city->HowMuchLonger(needed_production);
				needed_production *= list_ref.m_maximumCount;
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
			     || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPECIAL
			   )
			   && garrisonPercent > build_settler_production_level
			  )
			{
				needed_production =
					GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

				sint32 turns_to_build = city->HowMuchLonger(needed_production);
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
			// @ToDo: Cleanup duplicated code
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
				if(garrisonComplte >= 1.0) // Don't merge this part, since it may be exposed for the three types
				{
					needed_production =
						GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

					sint32 turns_to_build = city->HowMuchLonger(needed_production);
					needed_production *= list_ref.m_desiredCount;
					if(needed_production > 0)
					{
						max_list = (BUILD_UNIT_LIST) list_num;
						break;
					}
				}
			}
			else if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SETTLER)
			{
				if(!m_canBuildLandSettlers)
				{
					continue;
				}

				if(garrisonComplte >= 1.0)
				{
					needed_production =
						GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

					sint32 turns_to_build = city->HowMuchLonger(needed_production);
					needed_production *= list_ref.m_maximumCount;
					if(needed_production > 0)
					{
						max_list = (BUILD_UNIT_LIST) list_num;
						break;
					}
				}
			}
			else if(static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SEA_SETTLER)
			{
				if(!m_canBuildSeaSettlers)
				{
					continue;
				}

				if(garrisonComplte >= 1.0)
				{
					needed_production =
						GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

					sint32 turns_to_build = city->HowMuchLonger(needed_production);
					needed_production *= list_ref.m_maximumCount;
					if(needed_production > 0)
					{
						max_list = (BUILD_UNIT_LIST) list_num;
						break;
					}
				}
			}
			else if
			  (
			       static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SLAVERY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPY
			    || static_cast<BUILD_UNIT_LIST>(list_num) == BUILD_UNIT_LIST_SPECIAL
			  )
			{
				if(garrisonComplte >= 1.0)
				{
					needed_production =
						GetDBUnitRec(list_ref.m_bestType)->GetShieldCost();

					sint32 turns_to_build = city->HowMuchLonger(needed_production);
					needed_production *= list_ref.m_desiredCount;
					if(needed_production > 0)
					{
						max_list = (BUILD_UNIT_LIST) list_num;
						break;
					}
				}
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

sint32 Governor::GetNeededBuildingType(const CityData *city, const BuildingBuildListRecord *build_list_rec ) const
{
	Assert(city);

	for (sint32 i = 0; i < build_list_rec->GetNumBuilding(); i++)
	{
		sint32 const building_type = build_list_rec->GetBuildingIndex(i);

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
		m_value             (0),
		m_cost              (0.0),
		m_resource          (CTPRecord::INDEX_INVALID),
		m_sourceCity        (),
		m_destinationCity   (),
		m_valuePerCaravan	(0.0)
	{};

	bool operator < (GoodsRoute const & rval) const
	{
#if defined(USE_VALUE_PER_CARAVAN)
		return (m_valuePerCaravan < rval.m_valuePerCaravan)
		    || ((m_valuePerCaravan == rval.m_valuePerCaravan) && (m_cost > rval.m_cost));
#else
		return (m_value < rval.m_value)
		    || ((m_value == rval.m_value) && (m_cost > rval.m_cost));
#endif
	};

	/// Total yield
	sint32  m_value;
	/// Number of required caravans
	double  m_cost;
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

	double unused_freight = player_ptr->GetUnusedFreight();
	double total_freight = player_ptr->GetTotalFreight();
	GoodsRoute new_route;
	std::list<GoodsRoute> new_routes;

	m_neededFreight = 0.0;

	UnitDynamicArray * city_list = player_ptr->GetAllCitiesList();
	sint32             cityCount = city_list ? city_list->Num() : 0;
	for (sint32 i = 0; i < cityCount; i++)
	{
		Unit & city = city_list->Access(i);

		for (sint32 g = 0; g < g_theResourceDB->NumRecords(); g++)
		{
			if(city.CD()->IsLocalResource(g))
			{
				sint32 sellingPrice = -1;
				TradeRoute curDestRoute;

				if(!city.CD()->HasResource(g)
				&&  city.CD()->GetResourceTradeRoute(g, curDestRoute))
				{
					sellingPrice =
						tradeutil_GetTradeValue(m_playerId, curDestRoute->GetDestination(), g);
				}
				else
				{
					curDestRoute.m_id = 0;
				}

				Unit maxCity;
				sint32 maxPrice = 0;
				sint32 bestPrice = 0;
				double maxCost = 0.0;
				double maxNeededFreight = 0.0;
				for (sint32 op = 1; op < k_MAX_PLAYERS; op++)
				{
					if (m_playerId != op)
					{
						if (!g_player[op])
							continue;

						if (!player_ptr->HasContactWith(op))
							continue;

						if (AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, op) >= 0)
							continue;

						if (Diplomat::GetDiplomat(op).GetEmbargo(m_playerId))
							continue;
					}

					for (sint32 d = 0; d < g_player[op]->m_all_cities->Num(); d++) {
						Unit destCity = g_player[op]->m_all_cities->Access(d);

						if(!(destCity.GetVisibility() & (1 << m_playerId)))
							continue;

						if(destCity.m_id == city.m_id)
							continue;

						if (Diplomat::GetDiplomat(m_playerId).
								GetTradeRoutePiracyRisk(city, destCity))
							continue;

						if(curDestRoute.m_id != 0 &&
						    curDestRoute->GetDestination().m_id == destCity.m_id)
							continue;

						const sint32 price = tradeutil_GetTradeValue(m_playerId, destCity, g);
						const double cost = tradeutil_GetTradeDistance(city, destCity);

						if (price > bestPrice)
						{
							maxNeededFreight = cost;
							bestPrice = price;
						}

						if ((price > maxPrice) && (cost < total_freight))
						{
							maxPrice = price;
							maxCity = destCity;
							maxCost = cost;
						}
					}
				}

				m_neededFreight += maxNeededFreight;

				if (!player_ptr->IsRobot())
					continue;

				if (((sellingPrice < maxPrice) && (sellingPrice > 0) ) ||
					(curDestRoute.m_id != 0 && Diplomat::GetDiplomat(m_playerId).
					GetTradeRoutePiracyRisk(city, curDestRoute->GetDestination())))
				{
					unused_freight += curDestRoute->GetCost();

					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillTradeRoute,
						GEA_TradeRoute, curDestRoute.m_id,
						GEA_Int, CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED,
						GEA_End);
					if(g_network.IsClient()) {
						g_network.SendAction(new NetAction(NET_ACTION_CANCEL_TRADE_ROUTE,
														   (uint32)curDestRoute));
					}
				}

				if ((maxPrice > 0) && ((sellingPrice < 0) || (sellingPrice < maxPrice)))
				{
					new_route.m_sourceCity      = city;
					new_route.m_destinationCity = maxCity;
					new_route.m_cost            = maxCost;
					new_route.m_value           = maxPrice;
					new_route.m_resource        = g;
					new_route.m_valuePerCaravan	=
						(maxCost <= 0) ? VALUE_FREE_LUNCH
									   : static_cast<double>(maxPrice) / maxCost;
					new_routes.push_back(new_route);
				}
			}
		}
	}

	m_neededFreight -= total_freight;

	new_routes.sort();

	for
	(
		std::list<GoodsRoute>::iterator route_iter = new_routes.begin();
		(route_iter != new_routes.end()) && (unused_freight > 0);
		++route_iter
	)
	{
		if(route_iter->m_cost <= unused_freight)
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood,
			                       GEA_Int,         route_iter->m_resource,
			                       GEA_City,        route_iter->m_sourceCity,
			                       GEA_City,        route_iter->m_destinationCity,
			                       GEA_End
			                      );
			unused_freight -= route_iter->m_cost;
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
