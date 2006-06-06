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
//
//----------------------------------------------------------------------------

#include "c3.h"                 // Pre-compiled header
#include "governor.h"           // Own declarations: consistency check

#include "Diplomat.h"
#include "player.h"
#include "Path.h"
#include "installationtree.h"
#include "UnitRecord.h"
#include "cellunitlist.h"
#include "ArmyData.h"
#include "citydata.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "Globals.h"
#include "World.h"
#include "BuildListSequenceRecord.h"
#include "UnitBuildListRecord.h"
#include "WonderBuildListRecord.h"
#include "BuildingBuildListRecord.h"
#include "BuildingRecord.h"
#include "terrainutil.h"
#include "CityInfluenceIterator.h"
#include "Cell.h"
#include "c3math.h"
#include "Events.h"
#include "GameEventUser.h"
#include "WonderRecord.h"
#include "StrategyRecord.h"
#include "CitySizeRecord.h"
#include "PopRecord.h"
#include "ConstDB.h"
#include "GovernmentRecord.h"
#include "AdvanceRecord.h"
#include "ResourceRecord.h"
#include "Unit.h"
#include "tradeutil.h"
#include "TradeRouteData.h"
#include "StrDB.h"
#include "Strengths.h"
#include "stringutils.h"
#include "GoalRecord.h"
#include "gaiacontroller.h"
#include "AgreementMatrix.h"
#include "WonderTracker.h"
#include "ctpai.h"
#include "CityAstar.h"
extern CityAstar g_city_astar;
#include "gstypes.h"                    // TERRAIN_TYPES
#include "profileai.h"
#include <algorithm>
#include "Scheduler.h"
#include "ctpgoal.h"
#include "mapanalysis.h"
#include "network.h"
#include "net_action.h"
#include "TerrainRecord.h" // Use terrain database
#include "unitutil.h" // Use unit utilities

extern MapPoint g_mp_size;

namespace
{
    Governor const          UniqueInvalidGovernor       = Governor(PLAYER_UNASSIGNED);
}

Governor const &            Governor::INVALID           = UniqueInvalidGovernor;
Governor::GovernorVector    Governor::s_theGovernors;
Governor::TiGoalQueue       Governor::s_tiQueue;

//----------------------------------------------------------------------------
//
// Name       : Governor::ResizeAll
//
// Description: Resize the Governor data for a (different?) number of players.
//
// Parameters : newMaxPlayerId	: number of players (highest player id)
//
// Globals    : s_theGovernors 	: updated
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
// Parameters : archive			: stream to restore from
//
// Globals    : s_theGovernors 	: updated
//
// Returns    : -
//
// Remark(s)  : static function
//				Assumption: The size of s_theGovernors has been updated 
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
// Parameters : archive			: stream to save to
//
// Globals    : s_theGovernors 	: input (not modified)
//
// Returns    : -
//
// Remark(s)  : static function
//				Assumption: The size of s_theGovernors has been saved 
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
    TiGoalQueue   ().swap(s_tiQueue);
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
:   m_maximumUnitShieldCost     (0),
    m_currentUnitShieldCost     (0),
    m_playerId                  (playerId),
    m_currentUnitCount          (),
    m_neededFreight             (0.0)
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
Governor::Governor(Governor const &copyme)
:   m_maximumUnitShieldCost     (copyme.m_maximumUnitShieldCost),
    m_currentUnitShieldCost     (copyme.m_currentUnitShieldCost),
    m_playerId                  (copyme.m_playerId),
    m_currentUnitCount          (),
    m_neededFreight             (copyme.m_neededFreight)
{ 
	std::copy(copyme.m_currentUnitCount.begin(),
                copyme.m_currentUnitCount.end(),
                std::back_inserter(m_currentUnitCount)
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
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	bool obsolete;
	for(sint32 gov_index = 0; gov_index < strategy.GetNumGovernment(); gov_index++){
		const GovernmentRecord *rec = strategy.GetGovernment(gov_index);
			
		if(!player_ptr->HasAdvance(rec->GetEnableAdvanceIndex()))
			continue;

		obsolete = false;
		for(sint32 i = 0; i < rec->GetNumObsoleteAdvance(); i++){
			if(player_ptr->HasAdvance(rec->GetObsoleteAdvance(i)->GetIndex())){
				obsolete = true;
				break;
			}
		}
		if (obsolete)
			continue;

		sint32 diff = rec->GetTooManyCitiesThreshold() - player_ptr->GetNumCities();
		if (diff < 0)
			continue;

		return rec->GetIndex();
	}

	return -1;
}


StringId Governor::GetGovernmentAdvice() const
{
	const Player *player_ptr = g_player[m_playerId];
	if (player_ptr == NULL)
		return -1;

	const Governor & governor = 
		Governor::GetGovernor(m_playerId);

	sint32 new_gov_index = 
		governor.ComputeBestGovernment();

	sint32 current_gov_index = 
		player_ptr->GetGovernmentType();

	
	if (new_gov_index == current_gov_index)
		return -1;

	const GovernmentRecord *new_gov_rec = g_theGovernmentDB->Get(new_gov_index);
	const GovernmentRecord *current_gov_rec = g_theGovernmentDB->Get(new_gov_index);
	sint32 new_gov_rank = new_gov_rec->GetRank();
	sint32 current_gov_rank = current_gov_rec->GetRank();

	StringId adviceId;
	if (new_gov_rank == current_gov_rank)
		
		adviceId = new_gov_rec->GetSameRankAdvice();
	else if (new_gov_rank > current_gov_rank)
		
		adviceId = new_gov_rec->GetHigherRankAdvice();
	else
		
		adviceId = -1;

	return adviceId;
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
	player_ptr->SetWagesLevel(static_cast<sint32>(player_ptr->GetWagesExpectation()) - sliders_setting.m_deltaGold);
	player_ptr->SetRationsLevel(static_cast<sint32>(player_ptr->GetRationsExpectation()) - sliders_setting.m_deltaFood );

	if (update_cities == false)
		return 0;


	UnitDynamicArray *city_list = player_ptr->GetAllCitiesList();
	CityData *city;
	sint32 gold;
	double new_happiness;
	double old_happiness;
	double delta_happiness;
	double total_delta_happiness = 0;
	for (sint32 i = 0; i < city_list->Num(); i++) // sint32 to allow more then 256 cities
	{
		city = city_list->Access(i)->GetCityData();
		old_happiness = city->GetHappiness();
	
		//Added by Martin Gühmann to take specialists into account.
		//Well this has an effect but the AI seems to perform worse with it.
		//Right direction but more debug work is needed.
		AssignPopulation(city);
		// Force happiness recalculation as crime losses depend on happiness.
		city->CalcHappiness(gold, FALSE);
#if defined(NEW_RESOURCE_PROCESS)
		city->ProcessResources();
		city->CalculateResources();
		city->CalcPollution();
		city->DoSupport(true);
#else
		city->CollectResources();
		city->ProcessProduction(true);
		city->DoSupport(true); // Deduct wages and building costs
		city->SplitScience(true); // Deduct science costs
		city->CollectOtherTrade(true, false);
		city->ProcessFood();
		city->CalcPollution();
#endif

		// Production has an effect on pollution and polltion has an effect on happiness
		// Of course better would be only one recalculation
		city->CalcHappiness(gold, FALSE);
		city->EatFood();
		city->CalculateGrowthRate();
		new_happiness = city->GetHappiness();
		delta_happiness = new_happiness - old_happiness;
		total_delta_happiness += delta_happiness; // Total delta is nonsense, half over the limit other half under the limit and we are in plus.
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

	if(!found)
		return true;

	changed = (sliders_setting != orig_sliders_setting);
	return changed;
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

		config_found |= found;
		if(found)
		{
			last_sliders_setting = sliders_setting;
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
	while(found == false && changed == true)
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

	production_test = true;
	gold_test = true;
	food_test = true;
	happiness_test = true;
	
	Player * player_ptr = g_player[m_playerId];
	Assert(player_ptr);
	Unit city_unit;
	CityData * city;
	sint32 gold;

	double new_happiness;

	
	SetSliders(sliders_setting, false);

	
	sint32 num_cities = player_ptr->m_all_cities->Num();
	for(sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		city_unit = player_ptr->m_all_cities->Get(city_index);

		Assert(city_unit.m_id != 0);
		Assert(city_unit->GetCityData());
		city = city_unit->GetCityData();

			
		//Added by Martin Gühmann to take specialists into account.
		//Well this has an effect but the AI seems to perform worse with it.
		//Right direction but more debug work is needed.
		AssignPopulation(city);
		// Force happiness recalculation as crime losses depend on happiness.
		city->CalcHappiness(gold, FALSE);
        /// @todo Handle riot and revolt risk. Too many AI cities are revolting.

#if defined(NEW_RESOURCE_PROCESS)
		city->ProcessResources();
		city->CalculateResources();
		city->CalcPollution();
		city->DoSupport(true);
#else
		city->CollectResources();
		city->ProcessProduction(true);
		city->DoSupport(true); // Deduct wages and building costs
		city->SplitScience(true); // Deduct science costs
		city->CollectOtherTrade(true, false);
		city->ProcessFood();
		city->CalcPollution();
#endif
		city->CalcHappiness(gold, FALSE);
		city->EatFood();
		city->CalculateGrowthRate();

		new_happiness = city->GetHappiness();
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
	}

	
	total_production += player_ptr->GetProductionFromFranchises();
	if(total_production < player_ptr->GetReadinessCost())
	{
		production_test = false;
	}

	
	double wages_percent = static_cast<double>(total_gold_cost) / static_cast<double>(gross_gold);
	if(wages_percent > max_wage_percent)
	{
		gold_test = false;
	}

	
	sint32 player_gold = player_ptr->GetGold();
	if(total_gold < 0
	&& total_gold + player_gold < 0
	&& total_gold > player_gold * -deficit_spending
	){
		gold_test = false;
	}

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

	SlidersSetting current_sliders_setting;
	SlidersSetting new_sliders_setting;
	StringId adviceId = -1;
	std::string motivation_name;
	bool found;

	GetSliders(current_sliders_setting);

	
	if(ComputeMinimumSliders(new_sliders_setting) == false)
	{
		found = Governor::GetGovernor(m_playerId).ComputeBestSliders(new_sliders_setting);

		if(found)
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
	else
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


	while( !ProdSliderReachedMin(sliders_setting)
	||     !GoldSliderReachedMin(sliders_setting)
	||     !FoodSliderReachedMin(sliders_setting)
	){

		TestSliderSettings(     sliders_setting, slider_tests);
		value     =      slider_tests.GetValue();

		if(!ProdSliderReachedMin(sliders_setting)){
			prod_sliders_setting = sliders_setting;
			prod_sliders_setting.m_deltaProduction--;
			TestSliderSettings(prod_sliders_setting, prod_slider_tests);
			valueProd = prod_slider_tests.GetValue();
		}
		else{
			valueProd = -1; // Should be lower than everything else
		}

		if(!GoldSliderReachedMin(sliders_setting)){
			gold_sliders_setting = sliders_setting;
			gold_sliders_setting.m_deltaGold--;
			TestSliderSettings(gold_sliders_setting, gold_slider_tests);
			valueGold = gold_slider_tests.GetValue();
		}
		else{
			valueGold = -1;
		}

		if(!FoodSliderReachedMin(sliders_setting)){
			food_sliders_setting = sliders_setting;
			food_sliders_setting.m_deltaFood--;
			TestSliderSettings(food_sliders_setting, food_slider_tests);
			valueFood = food_slider_tests.GetValue();
		}
		else{
			valueFood = -1;
		}

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
			// Real equal need a better solution
/*			if(valueProd >= value
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
			}*/
			break;
		}

	}
}

void Governor::AddRoadPriority(Path & path, const double & priority_delta)
{

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	
	double bonus;
	TiGoal ti_goal;

	MapPoint old, pos;
	path.Start(old);
	path.Next(pos);

	for ( ; !path.IsEnd(); path.Next(pos))
	{
		
		ti_goal.type = GetBestRoadImprovement(pos);

		
		if ( ti_goal.type >= 0)
		{
			
			ti_goal.pos = pos;

			
			strategy.GetRoadUtilityBonus(bonus);
			ti_goal.utility =  bonus * priority_delta;
			s_tiQueue.push_back(ti_goal);
		}
	}
}


void Governor::ComputeRoadPriorities()
{
	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	sint32 num_cities = player_ptr->m_all_cities->Num();

	Unit city_unit;
	Unit neighbor_unit;
	sint32 neighbor_dist;
	sint32 min_dist;
	sint32 threat_rank;

	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		city_unit = player_ptr->m_all_cities->Get(city_index);


		if (!city_unit.CD()->GetUseGovernor())
			continue;

		threat_rank = 
			static_cast<sint32>(MapAnalysis::GetMapAnalysis().GetThreatRank(city_unit.CD()));
		min_dist = g_mp_size.x * g_mp_size.y; 
		Unit min_neighbor_unit;

			
		for (sint32 neighbor_index = 0; neighbor_index < num_cities; neighbor_index++)
		{
			neighbor_unit = player_ptr->m_all_cities->Get(neighbor_index);

				
			if (neighbor_unit.m_id == city_unit.m_id)
				continue;
				
			neighbor_dist = MapPoint::GetSquaredDistance(neighbor_unit.RetPos(), city_unit.RetPos());
			if (neighbor_dist < (min_dist * min_dist))  // TODO: check, doesn't look right: min_dist already stored as squared?
			{
				min_dist            = neighbor_dist;
				min_neighbor_unit   = neighbor_unit;
			}
		} 

			
		if (min_neighbor_unit == Unit())
			continue;

			
			
			

		float total_cost = 0.0;
		Path found_path;
			
		if (g_city_astar.FindRoadPath(city_unit.RetPos(), min_neighbor_unit.RetPos(),
			m_playerId,
			found_path,
			total_cost ))
		{
			Assert(0 < found_path.Num());

				
				
			AddRoadPriority(found_path, threat_rank);
		}
	} 
}


void Governor::PlaceTileImprovements()
{
	Player *player_ptr;
	CityData *city;
	Unit unit;
	Cell *cell;
	TiGoal ti_goal;
	MapPoint pos;

	Assert(g_player[m_playerId]);
	player_ptr = g_player[m_playerId];

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	
	s_tiQueue.clear();

	
	sint32 reserve_pw=0;
	
	
	
	
	
	
	
	
	
		
		strategy.GetPublicWorksReserve(reserve_pw);

	
	ComputeRoadPriorities();

	sint32 bonusFood, bonusProduction, bonusCommerce;
	
	sint32 num_cities = player_ptr->m_all_cities->Num();
	for (sint32 city_index = 0; city_index < num_cities; city_index++)
	{
		unit = player_ptr->m_all_cities->Get(city_index);
		Assert(unit->GetCityData());
		city = unit->GetCityData();


		if(!city->GetUseGovernor())
			continue;

		CityInfluenceIterator it(unit.RetPos(), city->GetSizeIndex());

//Added by Martin Gühmann to store in memory the added boni
		bonusFood = 0;
		bonusProduction = 0;
		bonusCommerce = 0;
			
		for (it.Start(); !it.End(); it.Next()) 
		{

			if(unit.RetPos() == it.Pos())
				continue;

			cell = g_theWorld->GetCell(it.Pos());

			if(!(cell->GetCityOwner() == unit))
				continue;

			if(cell->GetNumImprovements() > 0)
					continue;

			if(FindBestTileImprovement(it.Pos(), ti_goal, bonusFood, bonusProduction, bonusCommerce))
			{
				s_tiQueue.push_back(ti_goal);
			}
		} 
	} 

	sint32 max_eval = 0;
	(void) strategy.GetMaxEvalTileImprovements(max_eval);
	TiGoalQueue::iterator max_iter = s_tiQueue.begin() + std::min(static_cast<size_t>(max_eval),
																  s_tiQueue.size()
																 );
	
	std::partial_sort(s_tiQueue.begin(), max_iter, s_tiQueue.end(), std::greater<TiGoal>());

	sint32 avail_pw = player_ptr->GetMaterialsStored() - reserve_pw;

	for
	(
		TiGoalQueue::const_iterator iter = s_tiQueue.begin(); 
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

	s_tiQueue.clear();
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
bool Governor::FindBestTileImprovement(const MapPoint &pos, TiGoal &goal, sint32 &bonusFood, sint32 &bonusProduction, sint32 &bonusCommerce) const{

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
	
	double growth_rank = the_map.GetGrowthRank(city, false);
	double production_rank = the_map.GetProductionRank(city, false);
	double gold_rank = the_map.GetCommerceRank(city, false);
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

			Assert(effect);
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


		if(g_theTerrainImprovementDB->Get(food_ter)->GetTerraformTerrainIndex(terrain)){
			bonusFood += g_theWorld->GetCell(pos)->GetFoodFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetFoodFromTerrain();
			bonusProduction += g_theWorld->GetCell(pos)->GetShieldsFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetShieldsFromTerrain();
			bonusCommerce += g_theWorld->GetCell(pos)->GetGoldFromTerrain((sint8)terrain) - g_theWorld->GetCell(pos)->GetGoldFromTerrain();
	
			goal.type = food_ter;
	
			if(terrain_type != terrainutil_GetDead()){
				strategy.GetImproveGrowthBonus(bonus);
				goal.utility = bonus * terr_food_rank;

				if(growth_rank < 0.2){
				    strategy.GetImproveSmallCityGrowthBonus(bonus);
					goal.utility +=  bonus * (1.0 - growth_rank);
				}
		
			}
			else{
				goal.utility = 9999.0;
			}
		}

	}
//	else if(moreProdNeeded
//	|| best_gold_improvement < 0
	else if((terr_prod_rank > 0.2) 
	&&      (best_production_improvement >= 0)
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
			
			
		if(production_rank > 0.8){
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus *	production_rank;
		}
			
		if(g_theWorld->IsGood(pos)){
			strategy.GetImproveGoodBonus(bonus);
			goal.utility += bonus;
		}
	}
	else if((gold_rank > 0.4)
	&&      (best_gold_improvement >= 0)
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
			
		if(production_rank > 0.8){
			strategy.GetImproveLargeCityProductionBonus(bonus);
			goal.utility += bonus * production_rank;
		}
		if(g_theWorld->IsGood(pos)){
			strategy.GetImproveGoodBonus(bonus);
			goal.utility += bonus;
		}
	}
	else if(g_theWorld->IsGood(pos) == FALSE){ // Should be removed
		ERR_BUILD_INST err;
		if(terrain_type == terrainutil_GetGlacier() 
		|| terrain_type == terrainutil_GetSwamp() 
		|| terrain_type == terrainutil_GetTundra()
		){
			if(player_ptr->CanCreateImprovement(terrainutil_GetTerraformHillsImprovement(), pos, 0, FALSE, err)){
				strategy.GetImproveProductionBonus(bonus);
				goal.utility =  bonus * (1.0-production_rank);
				goal.type = terrainutil_GetTerraformHillsImprovement();
			}
		}
		else if(terrain_type == terrainutil_GetDesert()){
			if(player_ptr->CanCreateImprovement(terrainutil_GetTerraformGrasslandImprovement(), pos, 0, FALSE, err)){
				strategy.GetImproveGrowthBonus(bonus);
				goal.utility =  bonus * (1.0-growth_rank);
				goal.type = terrainutil_GetTerraformGrasslandImprovement();
			}
		}
	}

	// Moved to the end, so that if no terraform improvement selected
	// the dead tiles are removed.
	else if(terrain_type == terrainutil_GetDead()){
		if(food_ter >= 0){
			goal.type = food_ter;
			goal.utility = 9999.0;
		}
		else if(prod_ter >= 0){
			goal.type = prod_ter;
			goal.utility = 9999.0;
		}
		else if(gold_ter >= 0){
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
		if(citySize > unitutil_GetSmallCityMaxSize()){
#if defined(NEW_RESOURCE_PROCESS)
			sint32 ring = city->GetRing(pos);
			goal.utility *= static_cast<double>(city->GetWorkingPeopleInRing(ring)) / static_cast<double>(city->GetRingSize(ring));
#else
			sint32 const	sqDist	= MapPoint::GetSquaredDistance(city_owner.RetPos(), pos);
			goal.utility *= city->GetUtilisationRatio(sqDist);
#endif
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
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 old_move_cost = static_cast<sint32>(cell->GetMoveCost());
	
	Unit city = cell->GetCity();
	if (city.m_id != 0x0)
		return -1;

	const TerrainImprovementRecord *terr_imp_rec = 
		terrainutil_GetBestRoad(m_playerId, pos);
	if (terr_imp_rec == NULL)
		return -1;

	
	Player *player_ptr = g_player[m_playerId];
	ERR_BUILD_INST err;
	if (player_ptr && !player_ptr->CanCreateImprovement(terr_imp_rec->GetIndex(), pos, 0, FALSE, err))
		return -1;

	
	const TerrainImprovementRecord::Effect *effect;
	effect = terrainutil_GetTerrainEffect(terr_imp_rec, pos);
	sint32 cost;
	if(effect && effect->GetMoveCost(cost) && cost >= old_move_cost)
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
	sint32 type;
	sint32 tmp_bonus;
	const TerrainImprovementRecord *rec;
	const TerrainImprovementRecord::Effect *effect;

	Cell *cell;
	cell = g_theWorld->GetCell(pos);

	
	food_imp = -1;
	prod_imp = -1;
	gold_imp = -1;

	
	Unit city = cell->GetCity();
	if (city.m_id != 0x0)
		return;
	
	
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

		if (effect->GetBonusFood(tmp_bonus) &&
			(tmp_bonus > max_bonus_food)
           )
		{
			max_bonus_food = tmp_bonus;
		}
		
		if (effect->GetBonusProduction(tmp_bonus) &&
			(tmp_bonus > max_bonus_prod)
           )
		{
			max_bonus_prod = tmp_bonus;
		}
		
		if (effect->GetBonusGold(tmp_bonus) &&
			(tmp_bonus > max_bonus_gold)
           )
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
		ERR_BUILD_INST err;
		
		
		if (!player_ptr->CanCreateImprovement(type, pos, 0, FALSE, err))
			continue;
		
		
		if (current_class != 0x0 && ((rec->GetClass() & current_class) == 0x0))
			continue;

		
		effect = terrainutil_GetTerrainEffect(rec, terrain_type);
		if (effect) 
		{
			if (effect->GetBonusFood(tmp_bonus) &&
				(tmp_bonus > max_bonus_food)
               )
			{
				food_imp = type;
				max_bonus_food = tmp_bonus;
			}
			
			if (effect->GetBonusProduction(tmp_bonus) &&
				(tmp_bonus > max_bonus_prod)
               )
			{
				prod_imp = type;
				max_bonus_prod = tmp_bonus;
			}

			if (effect->GetBonusGold(tmp_bonus) &&
				(tmp_bonus > max_bonus_gold)
               )
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
//				g_theWorld:                Map information
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

	sint32 food_ter = -1;
	sint32 prod_ter = -1;
	sint32 gold_ter = -1;

	double max_bonus_food = 0.0;
	double max_bonus_prod = 0.0;
	double max_bonus_gold = 0.0;

	double tmp_bonus_food;
	double tmp_bonus_prod;
	double tmp_bonus_gold;


	Cell *  cell    = g_theWorld->GetCell(pos);
	Unit    city    = cell->GetCity();
	if (city.m_id != 0x0)
		return;

	const TerrainRecord *fromRec = g_theTerrainDB->Get(cell->GetTerrain());
	const TerrainRecord *toRec;

	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];

	if(!player_ptr->HasAdvance(fromRec->GetRemoveAdvanceIndex()))
		return;


	double pwCosts;
	sint32 index;
	for(index = 0; index < g_theTerrainDB->NumRecords(); ++index){
		toRec = g_theTerrainDB->Get(index);
	
		if(!player_ptr->HasAdvance(toRec->GetAddAdvanceIndex()))
			continue;

		if(!fromRec->HasTransformRemove()
		|| !toRec->HasTransformAdd())
			continue;

		tmp_bonus_food = static_cast<double>(cell->GetFoodFromTerrain((sint8)index) - cell->GetFoodFromTerrain());
		tmp_bonus_prod = static_cast<double>(cell->GetShieldsFromTerrain((sint8)index) - cell->GetShieldsFromTerrain());
		tmp_bonus_gold = static_cast<double>(cell->GetGoldFromTerrain((sint8)index) - cell->GetGoldFromTerrain());

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
			food_ter = index;
		}

		if(tmp_bonus_prod > 0
		&& tmp_bonus_prod > max_bonus_prod
		){
			max_bonus_prod = tmp_bonus_prod;
			prod_ter = index;
		}

		if(tmp_bonus_gold > 0
		&& tmp_bonus_gold > max_bonus_gold
		){
			max_bonus_gold = tmp_bonus_gold;
			gold_ter = index;
		}
	}
	
	const TerrainImprovementRecord *rec;
	sint32 ter;
	for(index = 0; index < g_theTerrainImprovementDB->NumRecords(); ++index){
		rec = g_theTerrainImprovementDB->Get(index);

		if(!rec->GetTerraformTerrainIndex(ter))
			continue;

		if(food_ter == ter){
			food_imp = index;
		}
		if(prod_ter == ter){
			prod_imp = index;
		}
		if(gold_ter == ter){
			gold_imp = index;
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

	
	UnitDynamicArray *city_list = g_player[m_playerId]->GetAllCitiesList();
	CityData *city;
	for(sint32 i = 0; i < city_list->Num(); i++) // city_list->Num() returns a sint32
	{
		city = city_list->Access(i)->GetCityData();

		
		if(!city->GetUseGovernor()) // Robot check already done in GetUseGovernor
			continue;

		
		if(city->GetSizeIndex() > 0)
			AssignPopulation(city);
	}

}


const StrategyRecord::PopAssignmentElement *Governor::GetMatchingPopAssignment(const CityData *city) const
{
	Assert(city);
	Assert(g_player[m_playerId]);

	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const StrategyRecord::PopAssignmentElement *elem = NULL;

	double top_value;
	double bottom_value;
	double rank; 
	bool found = false;

	for (sint16 i = 0; !found && i < strategy.GetNumPopAssignmentElement(); i++)
	{
		found = true;
		elem = strategy.GetPopAssignmentElement(i);
		
		if ( elem->GetProductionCities() ) 
		{
			rank = MapAnalysis::GetMapAnalysis().GetProductionRank(city, false);
		}
		else if ( elem->GetGrowthCities() ) 
		{
			rank = MapAnalysis::GetMapAnalysis().GetGrowthRank(city, false);
		}
		else if ( elem->GetGoldCities() ) 
		{
			rank = MapAnalysis::GetMapAnalysis().GetCommerceRank(city, false);
		}
		else if ( elem->GetDefault() )
		{
			break;
		}
		else 
		{
			continue;   // try next
		}

		if (elem->GetTop(top_value)) 
		{
			found &= (rank >= 1.0 - top_value);
		}
			
		if (elem->GetBottom(bottom_value))
		{
			found &= (rank <= bottom_value);
		}
	}

	Assert(found);
	Assert(elem);

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
void Governor::AssignPopulation(CityData *city) const {
    // Reset all specialists to workers
	city->ChangeSpecialists(POP_ENTERTAINER, -1 * city->EntertainerCount());
	city->ChangeSpecialists(POP_FARMER, -1 * city->FarmerCount());
	city->ChangeSpecialists(POP_LABORER, -1 * city->LaborerCount());
	city->ChangeSpecialists(POP_MERCHANT, -1 * city->MerchantCount());
	city->ChangeSpecialists(POP_SCIENTIST, -1 * city->ScientistCount());

	//////////////////////////////////////////////////////////////////////
	// Not a pretty good idea to redo all the stuff, but to avoid this 
	// the resources from the inner ring and the last ring must be 
	// stored at seperated places to use them in the HowMuchMoreFoodNeeded
	// method.
#if !defined(NEW_RESOURCE_PROCESS)
	city->CollectResources();
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
	const StrategyRecord::PopAssignmentElement *pop_assignment = GetMatchingPopAssignment(city);
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
	if(foodMissing > 0){
		sint32 i;
		sint32 workers;
		sint32 tmpCount;
		for(i = size_index; i >= 0; --i){
			farmersEff = city->GetFarmersEffect(i);
			if(farmersEff){
				tmpCount = static_cast<sint32>(ceil(static_cast<double>(foodMissing)/farmersEff));
				workers = city->GetWorkingPeopleInRing(i);
				if(tmpCount > workers){
					count += workers;
				}
				else{
					count += tmpCount;
					break;
				}
			}
			else{
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
	else{
		count = static_cast<sint32> // Have to reconsider this
			(ceil(specialists * pop_assignment->GetFarmerPercent()));
	}
	if(count > city->WorkerCount()){
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
		tmp_city->CollectResources();
		tmp_city->ProcessFood();
		
		if(tmp_city->GetProducedFood() > prev_result){
            // The specialists are beneficial: employ in the real city.
			city->ChangeSpecialists(POP_FARMER, count);
		}			
		else if(tmp_city->GetProducedFood() < prev_result){
            // The specialists are producing less than workers: fire them all?
            // This should not do anything, because there should be none left 
		// after the reset at the start.
			Assert(0 == city->FarmerCount());
			delta = (-1 * city->FarmerCount());
			city->ChangeSpecialists(POP_FARMER, delta);
		}
//		DPRINTF(k_DBG_GAMESTATE, ("NewResult: %f\n", tmp_city->GetProducedFood()));
		
        // Synchronise the tmp_city with the real city.
		delta = city->FarmerCount() - tmp_city->FarmerCount();
		tmp_city->ChangeSpecialists(POP_FARMER, delta );
#endif
	}
//	DPRINTF(k_DBG_GAMESTATE, ("Farmers: %i\n", city->FarmerCount()));
	
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
		tmp_city->CollectResources();
		tmp_city->ProcessProduction(true);
		prev_result = tmp_city->GetGrossCityProduction();

		tmp_city->ChangeSpecialists(POP_LABORER, count);
		tmp_city->CollectResources();
		tmp_city->ProcessProduction(true);

			
		if(tmp_city->GetGrossCityProduction() > prev_result){
			city->ChangeSpecialists(POP_LABORER, count);
		}			
		else if(tmp_city->GetGrossCityProduction() < prev_result){
			delta = (-1 * city->LaborerCount());
			city->ChangeSpecialists(POP_LABORER, delta);
		}
			
		delta = city->LaborerCount() - tmp_city->LaborerCount();
		tmp_city->ChangeSpecialists(POP_LABORER, delta );
#endif
	}

	
	best_specialist = city->GetBestSpecialist(POP_MERCHANT);

	count = static_cast<sint32>
		(ceil(specialists * pop_assignment->GetMerchantPercent()));
	count = (count <= merchants) ? count : merchants;
	if(count > city->WorkerCount()){
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
		tmp_city->CollectResources();
		tmp_city->CollectOtherTrade(TRUE, FALSE);
		prev_result = tmp_city->GetGrossCityGold();

		tmp_city->ChangeSpecialists(POP_MERCHANT, count);
		tmp_city->CollectResources();
		tmp_city->CollectOtherTrade(TRUE, FALSE);
		
		if(tmp_city->GetGrossCityGold() > prev_result){
			city->ChangeSpecialists(POP_MERCHANT, count);
		}
		else if(tmp_city->GetGrossCityGold() < prev_result){
			delta = (-1 * city->MerchantCount());
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
	if(count > city->WorkerCount()){
		count = city->WorkerCount();
	}

	//////////////////////////////////
	// First pop database index is 0
	if((best_specialist >= 0) 
	&& (count > 0)
	){
		city->ChangeSpecialists(POP_SCIENTIST, count);
	}

	best_specialist = city->GetBestSpecialist(POP_ENTERTAINER);

	////////////////////////////////
	// First pop database index is 0
	if(best_specialist >= 0){

		/////////////////////////
		// Recalculate happiness
		city->CalcHappiness(vgs,TRUE);

		////////////////////////////////////////////////////////////////////////
		// Retrieve minimum number of entertainers to keep the city from rioing.
		// Retrieve maximum number of entertainers needed for optimal happiness.
		sint32 min_entertainers;
		sint32 max_entertainers;
		ComputeMinMaxEntertainers(city, min_entertainers, max_entertainers);

		if(min_entertainers > city->WorkerCount()){
			count = min_entertainers - city->WorkerCount();
			count = (count <= city->LaborerCount()) ? count : city->LaborerCount();
			city->ChangeSpecialists(POP_LABORER, -count);
			// Happiness doesn't need to be recalculated, pops are used for entertainers
		}
		if(min_entertainers > city->WorkerCount()){
			count = min_entertainers - city->WorkerCount();
			count = (count <= city->ScientistCount()) ? count : city->ScientistCount();
			city->ChangeSpecialists(POP_SCIENTIST, -count);
		}
		if(min_entertainers > city->WorkerCount()){
			count = min_entertainers - city->WorkerCount();
			count = (count <= city->MerchantCount()) ? count : city->MerchantCount();
			city->ChangeSpecialists(POP_MERCHANT, -count);
		}
		if(min_entertainers > city->WorkerCount()){
			min_entertainers = city->WorkerCount();
		}
		if(max_entertainers > city->WorkerCount()){
			max_entertainers = city->WorkerCount();
		}

		count = static_cast<sint32>
			(ceil(specialists * pop_assignment->GetEntertainerPercent()));
		
		count = (count >= min_entertainers ? count : min_entertainers);
		count = (count <= max_entertainers ? count : max_entertainers);

		count = (count <= specialists || count == min_entertainers ? count : specialists);

		city->ChangeSpecialists(POP_ENTERTAINER, count);
	}

	if(max_workers < city->WorkerCount() + city->SlaveCount()){
		minSpecialists = (city->WorkerCount() + city->SlaveCount()) - max_workers;
		best_specialist = city->GetBestSpecialist(POP_SCIENTIST);
		if(best_specialist >= 0){
			city->ChangeSpecialists(POP_SCIENTIST, minSpecialists);
		}
	}
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
void Governor::ComputeMinMaxEntertainers(const CityData *city, sint32 & min, sint32 & max) const
{
	min = 0;
	max = 0;

	sint32 entertainer_type = city->GetBestSpecialist(POP_ENTERTAINER);

	if (entertainer_type < 0)
		return;

	sint32 per_pop_happiness = g_thePopDB->Get(entertainer_type)->GetHappiness();
    if (per_pop_happiness <= 0)
    {
		Assert(0);
		return;
    }

	sint32 needed = g_theConstDB->GetRiotLevel();
	sint32 maximum = g_theConstDB->GetVeryHappyThreshold();
	sint32 current = static_cast<sint32>(city->GetHappiness());


	double min_delta = static_cast<double>(needed - current) / 
                         static_cast<double>(per_pop_happiness);
	double max_delta = static_cast<double>(maximum - current) / 
                         static_cast<double>(per_pop_happiness);	
	
	if (min_delta < 0) {
		min_delta = floor(min_delta);
	}
	else {
		min_delta = ceil(min_delta);
	}

	if (max_delta < 0) {
		max_delta = floor(max_delta);
	}
	else {
		max_delta = ceil(max_delta);
	}

	min = (city->EntertainerCount() + (sint32) min_delta);
	max = (city->EntertainerCount() + (sint32) max_delta);
	
	min = (min < 0 ? 0 : min);
	max = (max < 0 ? 0 : max);

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

		if(resourceFraction > neededFraction + peopleFraction){
			neededFraction += peopleFraction;
			minResource += city->GetRingSize(i);
		}
		else if(resourceFraction == neededFraction + peopleFraction){
			minResource += city->GetRingSize(i);
			break;
		}
		else if(resourceFraction < neededFraction + peopleFraction){
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
sint32 Governor::ComputeMinimumWorkers(const CityData *city, 
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

	for (sint32 i = city->GetSizeIndex(); i >= 0; --i){
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
		else{
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

	double full_radii_food;
	double part_radii_food;

	sint32 full_radii_prod;
	sint32 part_radii_prod;

	sint32 full_radii_gold;
	sint32 part_radii_gold;

	sint32 full_radii_scigold;
	sint32 part_radii_scigold;

	sint32 full_radii_science;
	sint32 part_radii_science;
	
	sint32 partSquaredRadius; 
	sint32 fullSquaredRadius; 
	double utilization_needed;
	const CitySizeRecord *part_rec;
	const CitySizeRecord *full_rec;
	MapPoint cityPos = city->GetHomeCity().RetPos();

	double crimeLossFood;
	double grossFood;
	sint32 grossProduction;

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
	sint32 bestSpecialist;

	sint32 workers_needed = 0;
	sint32 WorkersNeeded = 0;
	
	for(sint32 sizeIndex = 1; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++){
		part_rec = g_theCitySizeDB->Get(sizeIndex);

		if(sizeIndex > 0){
			full_rec = g_theCitySizeDB->Get(sizeIndex-1);
			if(part_rec->GetMaxWorkers() <= city->PopCount()){
				part_size = part_rec->GetMaxWorkers() - full_rec->GetMaxWorkers();
			}
			else{
				part_size = city->PopCount() - full_rec->GetMaxWorkers();
			}
			part_size_pop = part_rec->GetMaxWorkers() - full_rec->GetMaxWorkers();
		}
		else{
			full_rec = g_theCitySizeDB->Get(0);
			part_size = part_rec->GetMaxWorkers();
			part_size_pop = part_rec->GetMaxWorkers();
		}

		partSquaredRadius = part_rec->GetSquaredRadius();
		fullSquaredRadius = full_rec->GetSquaredRadius();

		//////////////////////////////
		// Intial resetting of values:
		full_radii_food = 0;
		part_radii_food = 0;
		
		full_radii_prod = 0;
		part_radii_prod = 0;

		full_radii_gold = 0;
		part_radii_gold = 0;
		
		full_radii_science = 0;
		part_radii_science = 0;

		full_radii_scigold = 0;
		part_radii_scigold = 0;

		///////////////////////////////////////////
		// Collect resourses
		CityInfluenceIterator it(cityPos, sizeIndex);
		for(it.Start(); !it.End(); it.Next()){
			Cell *cell = g_theWorld->GetCell(it.Pos());

			if(fullSquaredRadius >= 0 
			&& MapPoint::GetSquaredDistance(cityPos, it.Pos()) <= fullSquaredRadius
			){
				full_radii_food += cell->GetFoodProduced();
				full_radii_prod += cell->GetShieldsProduced();
				full_radii_gold += cell->GetGoldProduced();
			} 
			else if(partSquaredRadius > 0 
			&& MapPoint::GetSquaredDistance(cityPos, it.Pos()) <= partSquaredRadius
			){
				part_radii_food += cell->GetFoodProduced();
				part_radii_prod += cell->GetShieldsProduced();
				part_radii_gold += cell->GetGoldProduced();
			}
		}

		////////////////////////////////////////
		// Apply all resource boni and mali
	//	DPRINTF(k_DBG_GAMESTATE, ("sizeIndex: %i\n", sizeIndex));
	//	DPRINTF(k_DBG_GAMESTATE, ("part_size: %i\n", part_size));
	//	DPRINTF(k_DBG_GAMESTATE, ("part_size_pop: %i\n", part_size_pop));
		grossFood = part_radii_food;
		city->ProcessFood(crimeLossFood, part_radii_food, grossFood, true);
		grossFood = full_radii_food;
		city->ProcessFood(crimeLossFood, full_radii_food, grossFood, true);

		part_radii_prod = city->ProcessProduction(true, 
		                         grossProduction, 
		                         part_radii_prod, 
		                         crimeLoss, 
		                         specialLoss, true);
		full_radii_prod = city->ProcessProduction(true, 
		                         grossProduction, 
		                         full_radii_prod, 
		                         crimeLoss, 
		                         specialLoss, true);
		city->CollectGold(full_radii_gold, specialLoss, crimeLoss, true);
		city->CollectGold(part_radii_gold, specialLoss, crimeLoss, true);

		support = city->GetSupport();
		full_radii_scigold = full_radii_gold - support;
		part_radii_scigold = part_radii_gold;
		if(full_radii_scigold < 0){
			part_radii_scigold += full_radii_scigold;
			if(part_radii_scigold < 0){
				part_radii_scigold = 0;
			}
		}

		city->SplitScience(true, full_radii_scigold, full_radii_science, true);
		city->SplitScience(true, part_radii_scigold, part_radii_science, true);

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// farmers to generate the same amount of food
		bestSpecialist = city->GetBestSpecialist(POP_FARMER);
		if(bestSpecialist >= 0 && part_size_pop > 0){
	

			popFood = g_thePopDB->Get(bestSpecialist)->GetFood()*part_size_pop;
	//		DPRINTF(k_DBG_GAMESTATE, ("popFood: %f\n", popFood));
			grossFood = popFood;
			popFood = city->ProcessFinalFood(crimeLossFood, grossFood);
	//		DPRINTF(k_DBG_GAMESTATE, ("popFood: %f\n", popFood));
	//		DPRINTF(k_DBG_GAMESTATE, ("crimeLossFood: %f\n", crimeLossFood));
	//		DPRINTF(k_DBG_GAMESTATE, ("part_radii_food: %f\n", part_radii_food));

			if(part_radii_food <= popFood){
				farmers += part_size;
				farmersEff = (popFood - part_radii_food)/static_cast<double>(part_size_pop);
			}
			else
				farmers = 0;
		}

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// laborers to generate the same amount of production
		bestSpecialist = city->GetBestSpecialist(POP_LABORER);
		if(bestSpecialist >= 0 && part_size_pop > 0){
			popProd = g_thePopDB->Get(bestSpecialist)->GetProduction()*part_size_pop;
			popProd = city->ComputeProductionLosses(popProd, crimeLoss, specialLoss);
			popProd -= (crimeLoss + specialLoss);
			if(part_radii_prod <= popProd){
				laborers += part_size;
				laborersEff = static_cast<double>(popProd - part_radii_prod)/static_cast<double>(part_size_pop);
			}
			else
				laborers = 0;
		}

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// merchants to generate the same amount of gold
		bestSpecialist = city->GetBestSpecialist(POP_MERCHANT);
		if(bestSpecialist >= 0 && part_size_pop > 0){
			popGold = g_thePopDB->Get(bestSpecialist)->GetCommerce()*part_size_pop;
			city->ApplyGoldCoeff(popGold);
			city->CalcGoldLoss(true, popGold, specialLoss, crimeLoss);

			if(part_radii_gold <= popGold){
				merchants += part_size;
				merchantsEff = static_cast<double>(popGold - part_radii_gold)/static_cast<double>(part_size_pop);
			}
			else
				merchants = 0;
		}

		///////////////////////////////////////////////////
		// Check whether it might be better to use
		// scientists to generate the same amount of science
		bestSpecialist = city->GetBestSpecialist(POP_SCIENTIST);
		if(bestSpecialist >= 0 && part_size_pop > 0){
			popScience = city->GetScienceFromPops(true);
			popScience += g_thePopDB->Get(bestSpecialist)->GetScience()*part_size_pop;
			popScience -= city->CrimeLoss(popScience);
			popScience = static_cast<sint32>(ceil(popScience * g_player[city->GetOwner()]->GetKnowledgeCoef()));

			if(part_radii_science <= popScience){
				scientists += part_size;
				scientistsEff = static_cast<double>(popScience - part_radii_science)/static_cast<double>(part_size_pop);
			}
			else
				scientists = 0;
		}

		///////////////////////////////////////////////////
		// Checks whether minimum number of food workers
		// has been found.
		if(notFoodFound
		&& full_radii_food + part_radii_food > city->GetFoodRequired()
		&& part_radii_food > 0.0
		){
	
			utilization_needed = 
			    (city->GetFoodRequired() - full_radii_food) / part_radii_food;

			if (fullSquaredRadius > 0) {
				workers_needed = full_rec->GetMaxWorkers();
			}
	
			workers_needed += static_cast<sint32>(ceil(utilization_needed * 
									(part_rec->GetMaxWorkers() - workers_needed)));
			if(WorkersNeeded < workers_needed){
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0){
				minFood = workers_needed;
			}
			notFoodFound = false;
		}

		///////////////////////////////////////////////////
		// Checks whether minimum number of production workers
		// has been found.
		if(notProdFound
		&& full_radii_prod + part_radii_prod > 0 // Needs according function.
		&& part_radii_prod > 0
		){
	
			utilization_needed = 
			    static_cast<double>(0 - full_radii_prod) / static_cast<double>(part_radii_prod);

			if (fullSquaredRadius > 0) {
				workers_needed = full_rec->GetMaxWorkers();
			}
	
			workers_needed += static_cast<sint32>(ceil(utilization_needed * 
									(part_rec->GetMaxWorkers() - workers_needed)));
			if(WorkersNeeded < workers_needed){
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0){
				minProd = workers_needed;
			}
			notProdFound = false;
		}

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

			if (fullSquaredRadius > 0) {
				workers_needed = full_rec->GetMaxWorkers();
			}
	
			workers_needed += static_cast<sint32>(ceil(utilization_needed * 
									(part_rec->GetMaxWorkers() - workers_needed)));
	//		DPRINTF(k_DBG_GAMESTATE, ("workers_needed: %i\n", workers_needed));
			if(WorkersNeeded < workers_needed){
				WorkersNeeded = workers_needed;
			}
			if(workers_needed > 0){
				minGold = workers_needed;
			}
			notGoldFound = false;
		}
		
		///////////////////////////////////////////////////
		// Checks whether minimum number of science workers
		// has been found.
		if(notScienceFound
		&& full_radii_science + part_radii_science > 0 // Better condition needed.
		&& part_radii_science > 0
		){
	
			utilization_needed = 
			    static_cast<double>(0 - full_radii_science) / static_cast<double>(part_radii_science);

			if (fullSquaredRadius > 0) {
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

		if(part_rec->GetPopulation() > city->PopCount()){
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

	sint16 list_num;
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const UnitBuildListRecord *build_list_rec;
	sint32 best_unit_type;

	sint32 total_unit_support_by_type;
    sint32 city_index;
	Army army;
	Unit unit;
	double freight_per_unit;

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
	
	
	for (best_unit_type=0; best_unit_type < g_theUnitDB->NumRecords(); best_unit_type++)
		m_currentUnitCount[best_unit_type]=0;
	
	
	sint32 num_armies = player_ptr->m_all_armies->Num();
	for (sint32 army_index = 0; army_index < num_armies; army_index++)
	{
		army = player_ptr->m_all_armies->Get(army_index);

		for (sint32 unit_index = 0; unit_index < army->Num(); unit_index++)
		{
			unit = army.Get(unit_index);

			
			if (!g_theUnitPool->IsValid(unit))
				continue;

			
			m_currentUnitCount[unit.GetType()]++;
		}
	} 
	
	
	for (city_index = 0; city_index < num_cities; city_index++)
	{
		unit = player_ptr->m_all_cities->Get(city_index);

		
		if (!g_theUnitPool->IsValid(unit))
			continue;

		Assert(unit->GetCityData());
		Assert(unit->GetCityData()->GetBuildQueue());
		if( unit->GetCityData()->GetBuildQueue()->GetHead() &&
			(unit->GetCityData()->GetBuildQueue()->GetHead()->m_category == k_GAME_OBJ_TYPE_UNIT) )
		{
			
			m_currentUnitCount[unit->GetCityData()->GetBuildQueue()->GetHead()->m_type]++;
		}
	} 

	m_maximumUnitShieldCost = 0;
	m_currentUnitShieldCost = 0;
	
	sint16 needed_transport = Scheduler::GetScheduler(m_playerId).
		GetMostNeededStrength().Get_Transport();

	
	

	
	for (list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		m_buildUnitList[list_num].m_perCityGarrison = 0;
		m_buildUnitList[list_num].m_desiredCount    = 0;
		m_buildUnitList[list_num].m_maximumCount    = 0;

		build_list_rec = GetBuildListRecord(strategy, (BUILD_UNIT_LIST) list_num);

        sint32 desired_count                        = 0;
	    sint32 garrison_count                       = 0;
	    double unit_support_percent_by_type         = 0.0;
		
		switch (list_num)
		{
		case BUILD_UNIT_LIST_OFFENSE:
			(void) strategy.GetOffensiveGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison = 
				static_cast<sint16>(garrison_count);
			
			strategy.GetOffensiveUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_DEFENSE:
			(void) strategy.GetDefensiveGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison = 
				static_cast<sint16>(garrison_count);
			
			strategy.GetDefensiveUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_RANGED:
			(void) strategy.GetRangedGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison = 
				static_cast<sint16>(garrison_count);
			
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
			
			m_buildUnitList[list_num].m_perCityGarrison = 0;
			
			
			best_unit_type = ComputeBestUnitType(build_list_rec);
			m_buildUnitList[list_num].m_bestType = best_unit_type;
			
			if (best_unit_type >= 0)
			{
				
				freight_per_unit = g_theUnitDB->Get(best_unit_type)->GetMaxMovePoints();
				desired_count = 
					static_cast<sint32>(ceil(m_neededFreight / freight_per_unit));
			}
			break;
			
		default:
			
			Assert(false);
			break;
		}
		
		
		best_unit_type = ComputeBestUnitType(build_list_rec);
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
				
				
				
				sint32 const bestUnitSupport	= 
					g_theUnitDB->Get(best_unit_type)->GetShieldHunger();

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
			
			
			if (desired_count - m_currentUnitCount[best_unit_type] > 0)
			{
				m_buildUnitList[list_num].m_maximumCount = 	
					static_cast<sint16>(needed_cities);
			}
			else
			{
				m_buildUnitList[list_num].m_maximumCount = 	0;
			}
			
		case BUILD_UNIT_LIST_SPECIAL:
		case BUILD_UNIT_LIST_FREIGHT:
			if (best_unit_type >= 0)
			{
				
				
				
				m_buildUnitList[list_num].m_desiredCount = 
					static_cast<sint16>(desired_count - m_currentUnitCount[best_unit_type]);
			}
			break;
		case BUILD_UNIT_LIST_SEA_TRANSPORT:
			if (best_unit_type >= 0)
				{

					m_buildUnitList[list_num].m_desiredCount = 
						static_cast<sint16>(desired_count);
					
					m_buildUnitList[list_num].m_desiredCount -= 
						(m_currentUnitCount[best_unit_type] - needed_transport);
				}
			break;

		case BUILD_UNIT_LIST_AIR_TRANSPORT:
			if (best_unit_type >= 0)
				{
					
					if (m_buildUnitList[BUILD_UNIT_LIST_SEA_TRANSPORT].m_desiredCount == 0) 
					{
						m_buildUnitList[list_num].m_desiredCount = 
							static_cast<sint16>(desired_count);

						m_buildUnitList[list_num].m_desiredCount -= m_currentUnitCount[best_unit_type];
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
	CellUnitList *units_ptr;
	for (city_index = 0; city_index < num_cities; city_index++)
		{
			strategy.GetOffensiveGarrisonCount(desired_offense);
			strategy.GetDefensiveGarrisonCount(desired_defense);
			strategy.GetRangedGarrisonCount(desired_ranged);

			unit = player_ptr->m_all_cities->Get(city_index);

			
			if (!g_theUnitPool->IsValid(unit))
				continue;

			unit->GetPos(pos);
			units_ptr = g_theWorld->GetArmyPtr(pos);
			for (sint32 unit_index = 0; unit_index < units_ptr->Num(); unit_index++)
				{
					
					if (!g_theUnitPool->IsValid(units_ptr->Get(unit_index)))
						continue;

					
					if ( units_ptr->Get(unit_index)->GetType() == 
						 m_buildUnitList[BUILD_UNIT_LIST_OFFENSE].m_bestType )
						desired_offense--;

					
					if ( units_ptr->Get(unit_index)->GetType() == 
						 m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_bestType )
						desired_defense--;

					
					if ( units_ptr->Get(unit_index)->GetType() == 
						 m_buildUnitList[BUILD_UNIT_LIST_RANGED].m_bestType )
						desired_ranged--;
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


void Governor::FillEmptyBuildQueues()
{
	Assert(g_player[m_playerId]);
	if (g_player[m_playerId] == NULL)
		return;

	
	ComputeDesiredUnits();

	
	
	if(g_network.IsActive() && !g_network.IsLocalPlayer(m_playerId))
		return;

	bool first_turn_of_war = Diplomat::GetDiplomat(m_playerId).FirstTurnOfWar();

	
	UnitDynamicArray *city_list = g_player[m_playerId]->GetAllCitiesList();
	sint32 cat,type;

	
	sint32 list_num = BUILD_UNIT_LIST_MAX;

	for (sint32 i = 0; i < city_list->Num(); i++)
	{
		
		if (!g_theUnitPool->IsValid(city_list->Access(i)))
			continue;

		CityData *city = city_list->Access(i)->GetCityData();

		
		if (city->GetBuildQueue()->GetLen() > 0)
		{
			
			
			if (!first_turn_of_war || (g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT))
				continue;
			else 
				city->GetBuildQueue()->Clear();
		}

		
		ComputeNextBuildItem(city, cat, type, list_num);

		
		if (!city->GetUseGovernor())
			continue;

		
		BOOL insert_ok = false;
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
//				city->InsertCapitalization(); // How is Capitalization removed?
				city->BuildCapitalization();
				break;
			case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
				insert_ok = true;
//				city->InsertInfrastructure(); // How is Infrastructure removed?
				city->BuildInfrastructure();
				break;
			}
		Assert(insert_ok);
	}
}


double Governor::PercentUnbuilt(const BUILD_UNIT_LIST unit_list) const
{
	if (m_buildUnitList[unit_list].m_bestType >= 0 && 
		m_buildUnitList[unit_list].m_maximumCount > 0)
		{
			return ( m_buildUnitList[unit_list].m_desiredCount /
				m_buildUnitList[unit_list].m_maximumCount );
		}

	return 0.0;
}


StringId Governor::GetCityBuildQueueAdvice(const CityData *city) const
{
	StringId adviceId;

	GetMatchingSequence(city, false, adviceId);

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
	for (sint32 list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
		{
			
			if ( ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SETTLER ) ||
				 ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SPECIAL) ||
				 ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SEA_TRANSPORT) ||
				 ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_AIR_TRANSPORT ))
				continue;

			
			if ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_FREIGHT ) 
				continue;

			double const percent_unbuilt = PercentUnbuilt((BUILD_UNIT_LIST) list_num);
			if (percent_unbuilt > max_percent_unbuilt)
				{
					max_percent_unbuilt = percent_unbuilt;
					max_needed_list = list_num;
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
	const BuildListSequenceRecord *build_list_sequence = 
		GetMatchingSequence(city, (g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT), advice);

	
	if((g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT))
	{
		if(city->GetUseGovernor())
		{
			Assert(city->GetBuildListSequenceIndex() >= 0);
			build_list_sequence = g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());
		}
	}

	
	
	if(!city->GetUseGovernor() || g_player[m_playerId]->GetPlayerType() == PLAYER_TYPE_ROBOT)
	{
		sint32 suggested_sequence = build_list_sequence->GetIndex();
		if (suggested_sequence >= 0)
			city->SetBuildListSequenceIndex(suggested_sequence);
	}

	
	
	
	const BuildListSequenceRecord::BuildListElement *elem;

	bool city_full = 
		(g_theWorld->GetCell(city->GetHomeCity().RetPos())->GetNumUnits() >= k_MAX_ARMY_SIZE);
	
	
	int elem_num;
	for( elem_num = 0; elem_num < build_list_sequence->GetNumBuildListElement(); elem_num++)
	{
		elem = build_list_sequence->GetBuildListElement(elem_num);

		
		if(elem->GetAllUnitBuildList() && !city_full)
		{
			type = GetNeededUnitType(city, list_num);
			cat = k_GAME_OBJ_TYPE_UNIT;

			
			if(type > -1)
			{
				Assert(city->CanBuildUnit(type));
				break;
			}
		}

		if(elem->GetGarrisonUnitBuildList() && !city_full)
		{
			type = GetNeededGarrisonUnitType(city, list_num);
			cat = k_GAME_OBJ_TYPE_UNIT;

			if(type > -1)
			{
				Assert(city->CanBuildUnit(type));
				break;
			}
		}
			
		else if(elem->HasBuildingBuildList())
		{
			type = GetNeededBuildingType(city, elem->GetBuildingBuildListPtr());
			cat = k_GAME_OBJ_TYPE_IMPROVEMENT;

			if(type > -1 && city->CanBuildBuilding(type))
				break;

		}
			
		else if(elem->HasWonderBuildList())
		{
			type = GetNeededWonderType(city, elem->GetWonderBuildListPtr());
			cat = k_GAME_OBJ_TYPE_WONDER;

			if(type > -1 && city->CanBuildWonder(type))
				break;

		}
		else if(elem->GetFreight())
		{
			type = GetNeededFreightType(list_num);
			cat = k_GAME_OBJ_TYPE_UNIT;

			if (type > -1)
				break;

		}
		else if(elem->GetInfrastructure())
		{
			if(city->CanBuildInfrastructure())
			{
				type = 1;
				cat = k_GAME_OBJ_TYPE_INFRASTRUCTURE;
				break;
			}
		}
		else if(elem->GetCapitalization())
		{
			if ( city->CanBuildCapitalization() )
			{
				type = 1;
				cat = k_GAME_OBJ_TYPE_CAPITALIZATION;
				break;
			}
		}
		else
		{
				
				
		}
	}

	if (elem_num >= build_list_sequence->GetNumBuildListElement() )
	{
		DPRINTF(k_DBG_AI, ("Nothing valid found to build in city at (%d,%d), increase production allocated to units?\n"));
		cat = k_GAME_OBJ_TYPE_UNIT;
		type = m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_bestType;
	}
#ifdef _DEBUG
	bool RICHARD_WANTS_TO_SEE_THIS_ASSERT = (type > -1);
	Assert(RICHARD_WANTS_TO_SEE_THIS_ASSERT);
#endif
}


const BuildListSequenceRecord * Governor::GetMatchingSequence(const CityData *city, const bool human_city, StringId & advice) const
{
	Assert(city);
	Assert(g_player[m_playerId]);

	
	if(city->GetUseGovernor() && human_city)
		return g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());

	
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	
	double rank = 0.0; 
	const StrategyRecord::BuildListSequenceElement *elem = NULL;
	const StrategyRecord::BuildListSequenceElement *best_elem = NULL;
	sint32 best_priority = -99999;

	sint32 elem_num;
	for(elem_num = 0; elem_num < strategy.GetNumBuildListSequenceElement(); elem_num++)
	{
		elem = strategy.GetBuildListSequenceElement(elem_num);

		if(best_elem && elem->GetPriority() < best_priority)
			continue;

			
		if(elem->GetProductionCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetProductionRank(city, false);
		}

		else if(elem->GetGrowthCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetGrowthRank(city, false);
		}

		else if(elem->GetCommerceCities())
		{
			rank = MapAnalysis::GetMapAnalysis().
			       GetCommerceRank(city, false);
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
			best_elem = elem;
		}

		double bottom_value;
		if (elem->GetBottom(bottom_value) &&
            (rank <= bottom_value)
           )
		{
			best_priority = elem->GetPriority();
			best_elem = elem;
		}
	}


	advice = -1;
	
	if (best_elem == NULL)
	{
		best_elem = strategy.GetBuildListSequenceElement(elem_num-1);
	    Assert(best_elem);
		if (best_elem == NULL)
			return g_theBuildListSequenceDB->Get(0);
	}

	(void) best_elem->GetAdvice(advice);

	return best_elem->GetBuildListSequence();
}


sint32 Governor::GetNeededUnitType(const CityData *city, sint32 & list_num) const
{
	Assert(g_player[m_playerId]);

	BUILD_UNIT_LIST max_list = BUILD_UNIT_LIST_MAX;
	sint32 max_production = 0;
	sint32 turns_to_build = 9999;
	sint32 needed_production;
	sint32 type = -1; 
	sint32 cont;
	double build_transport_production_level;
	double build_settler_production_level;

	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	
	
	strategy.GetBuildTransportProductionLevel(build_transport_production_level);
	strategy.GetBuildSettlerProductionLevel(build_settler_production_level);

	for (list_num = 0; list_num < BUILD_UNIT_LIST_MAX; list_num++)
	{
		
		if ( ( g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT ) &&
			( ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SETTLER ) ||
			( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SPECIAL) ||
			( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SEA_TRANSPORT ) ||
			( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_AIR_TRANSPORT ) ) ) 
			continue;
		
		
		if ((BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SEA_TRANSPORT ||
			(BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SEA )
		{
			if (!g_theWorld->GetAdjacentOcean(city->GetHomeCity().RetPos(), cont))
				continue;
		}
		
		
		if ( (BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_FREIGHT ) 
			continue;
		
		
		const BuildUnitList & list_ref = m_buildUnitList[list_num];
		
		
		
		if (list_ref.m_bestType < 0)
			continue;
		
		
		needed_production = 
			g_theUnitDB->Get(list_ref.m_bestType)->GetShieldCost();
		
		
		turns_to_build = city->HowMuchLonger(needed_production);
		
		if ((BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SETTLER)
		{
			
			
			needed_production *= list_ref.m_maximumCount;
		}
		else
		{
			
			
			needed_production *= list_ref.m_desiredCount;
		}
		
		
		if ( needed_production > 0 )
		{
			
			if (((BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SEA_TRANSPORT) ||
				((BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_AIR_TRANSPORT))
			{
				
				
				if ( ((double) m_currentUnitShieldCost / m_maximumUnitShieldCost) > build_transport_production_level )
				{
					max_list = (BUILD_UNIT_LIST) list_num;
					
					
					break;
				}
			}
			else if ((BUILD_UNIT_LIST) list_num == BUILD_UNIT_LIST_SETTLER)
			{
				
				
				if ( ((double) m_currentUnitShieldCost / m_maximumUnitShieldCost) > build_settler_production_level )
				{
					
					max_list = (BUILD_UNIT_LIST) list_num;
					
					
					break;
				}
			}
			
			else if	( needed_production > max_production ) 
			{
				
				max_production = needed_production;
				max_list = (BUILD_UNIT_LIST) list_num;
			}
		}
	}

	
	
	if (max_list != BUILD_UNIT_LIST_MAX)
	{
		type = m_buildUnitList[max_list].m_bestType;
		list_num = max_list;
		
		
		if (!city->CanBuildUnit(type))
		{
			
			const UnitBuildListRecord *build_list_rec = GetBuildListRecord(strategy, max_list);
			type = ComputeBestUnitType(build_list_rec, city);
		} 
	}	

	UnitRecord const *	unit	= g_theUnitDB->Get(type);
	DPRINTF(k_DBG_GAMESTATE, ("Selected unit type: %s\n", unit ? unit->GetNameText() : "none"));
				DPRINTF(k_DBG_GAMESTATE, ("Player: %lx\n", m_playerId));

	return type;
}


const UnitBuildListRecord * Governor::GetBuildListRecord(const StrategyRecord & strategy, const BUILD_UNIT_LIST list_type) const
{
	const UnitBuildListRecord * build_list_rec;
	switch ( list_type )
	{
	case BUILD_UNIT_LIST_OFFENSE:
		
		Assert(strategy.HasOffensiveUnitList());
		build_list_rec = strategy.GetOffensiveUnitListPtr();
		break;
	case BUILD_UNIT_LIST_DEFENSE:
		
		Assert(strategy.HasDefensiveUnitList());
		build_list_rec = strategy.GetDefensiveUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_RANGED:
		
		Assert(strategy.HasRangedUnitList());
		build_list_rec = strategy.GetRangedUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SEA:
		
		Assert(strategy.HasSeaUnitList());
		build_list_rec = strategy.GetSeaUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_AIR:
		
		Assert(strategy.HasAirUnitList());
		build_list_rec = strategy.GetAirUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SETTLER:
		
		Assert(strategy.HasSettlerUnitList());
		build_list_rec = strategy.GetSettlerUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SPECIAL:
		
		Assert(strategy.HasSpecialUnitList());
		build_list_rec = strategy.GetSpecialUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SEA_TRANSPORT:
		
		Assert(strategy.HasSeaTransportUnitList());
		build_list_rec = strategy.GetSeaTransportUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_AIR_TRANSPORT:
		
		Assert(strategy.HasAirTransportUnitList());
		build_list_rec = strategy.GetAirTransportUnitListPtr();
		break;

	case BUILD_UNIT_LIST_FREIGHT:
		
		Assert(strategy.HasFreightUnitList());
		build_list_rec = strategy.GetFreightUnitListPtr();
		break;
	default:
		build_list_rec = NULL;
		Assert(false);
		break;
	}
	return build_list_rec;
}	



sint32 Governor::GetNeededGarrisonUnitType(const CityData * city, sint32 & list_num) const
{
	Assert( g_player[m_playerId] );
	Assert( city );
	Assert( g_theWorld );

	if ( city->GetGarrisonComplete() && 
		 city->GetGarrisonOtherCities() == FALSE)
		return -1;

	BUILD_UNIT_LIST max_list = BUILD_UNIT_LIST_MAX;
	sint32 max_production = 0;
	sint32 needed_production = 0;
	CellUnitList garrison_army;
	
	for (list_num = BUILD_UNIT_LIST_RANGED; list_num < BUILD_UNIT_LIST_MAX; list_num++)
		{
			
			const BuildUnitList & list_ref = m_buildUnitList[list_num];

			if (list_ref.m_bestType < 0)
				{
					
					needed_production = 0;
				}
			
			else if ( city->GetGarrisonComplete() )
				{
					
					needed_production = (list_ref.m_garrisonCount * 
										 g_theUnitDB->Get(list_ref.m_bestType)->GetShieldCost());
				}
			
			else
				{
					
					needed_production = list_ref.m_perCityGarrison * 
						g_theUnitDB->Get(list_ref.m_bestType)->GetShieldCost();
							
					
					g_theWorld->GetArmy( city->GetHomeCity().RetPos(), garrison_army );
					for (int i = 0; i < garrison_army.Num(); i++)
						{
							if ( garrison_army.Get(i).GetDBRec()->GetIndex() == list_ref.m_bestType)
								{
									needed_production -= 
										g_theUnitDB->Get(list_ref.m_bestType)->GetShieldCost();
								}
						}
				}
					
			if ( needed_production > max_production ) 
				{
					
					max_production = needed_production;
					max_list = (BUILD_UNIT_LIST) list_num;
				}
		}

	
	sint32 type = -1; 

	if (max_list != BUILD_UNIT_LIST_MAX )
		{
			type = m_buildUnitList[max_list].m_bestType;
			list_num = max_list;

			
			if (!city->CanBuildUnit(type))
			{
				
				const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
				const UnitBuildListRecord *build_list_rec = GetBuildListRecord(strategy, max_list);
				type = ComputeBestUnitType(build_list_rec, city);
			} 
		}

	return type;
}


sint32 Governor::GetNeededBuildingType(const CityData *city, const BuildingBuildListRecord *build_list_rec ) const
{
	Assert( g_player[m_playerId] );
	Assert( city );
	Assert( g_theBuildingDB );
	
	for (int i = 0; i < build_list_rec->GetNumBuilding(); i++)
		{
			sint32 const building_type = build_list_rec->GetBuildingIndex(i);

			
			if ( city->CanBuildBuilding(building_type) )
				return building_type;
		}

	return (-1);
}


sint32 Governor::GetNeededWonderType(const CityData *city, const WonderBuildListRecord *build_list_rec ) const
{
	Assert( g_player[m_playerId] );
	Assert( city );
	Assert( g_theWonderDB );

	for (int i = 0; i < build_list_rec->GetNumWonder(); i++)
		{
			sint32 const wonder_type = build_list_rec->GetWonderIndex(i);

			
			if (g_theWonderTracker->IsBuildingWonder(wonder_type, m_playerId))
				continue;

			
			if ( city->CanBuildWonder(wonder_type))
				return wonder_type;
		}

	return (-1);
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

	
	return -1;
}






sint32 Governor::ComputeBestMilitaryReadiness() const
{
	
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	sint32 new_level = 0;
	(void) strategy.GetReadinessLevel(new_level);

	Assert(g_player[m_playerId]);
	Player * player_ptr = g_player[m_playerId];

	double max_support_cost = 1.0;
	sint32 total_production = player_ptr->GetTotalProduction();
	sint32 support_percent;
	if (strategy.GetMaxSupportCostPercent(support_percent))
	{
		max_support_cost = ((double)support_percent / 100);
	}

	
	if (total_production &&
		player_ptr->GetReadinessCost() > (max_support_cost * total_production))
		{
			
			

			
			
			
			

			
			

			
		}

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
	CTPGoal_ptr ctp_goal_ptr = 
		(CTPGoal_ptr) Scheduler::GetScheduler(m_playerId).GetHighestPriorityGoal((GOAL_TYPE)defend_goal_type, is_satisfied);
	if (ctp_goal_ptr != NULL)
	{
		sc.AddCity(ctp_goal_ptr->Get_Target_City());
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
	ctp_goal_ptr = 
		(CTPGoal_ptr) Scheduler::GetScheduler(m_playerId).GetHighestPriorityGoal((GOAL_TYPE)seige_goal_type, is_satisfied);
	if (ctp_goal_ptr != NULL)
	{
		sc.AddCity(ctp_goal_ptr->Get_Target_City());
		return seigeAdviceId;
	}

	sint32 num_cities = player_ptr->m_all_cities->Num();

	CellUnitList garrison;
	Unit city;
	sint32 j;
	for (sint32 i = 0; i < num_cities; i++)
	{
		city = player_ptr->m_all_cities->Access(i);
		Assert( g_theUnitPool->IsValid(city) );
		Assert( city->GetCityData() != NULL );

		g_theWorld->GetArmy(city.RetPos(), garrison);
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





struct GoodsRoute {
	GoodsRoute() : m_value(-1) {}
	bool operator<(const GoodsRoute & rval) const
	{
		
		return (m_value < rval.m_value ||
			    (m_value == rval.m_value &&
				 m_cost > rval.m_cost ));
	}
	sint32 m_value;
	double m_cost;
	sint32 m_resource;
	
	Unit m_sourceCity;
	Unit m_destinationCity;
};

typedef std::list<GoodsRoute> GoodsRouteList;

void Governor::ManageGoodsTradeRoutes()
{
	Assert(g_player[m_playerId] != NULL);
	Player *player_ptr = g_player[m_playerId];

	Unit city;
	UnitDynamicArray *city_list = player_ptr->GetAllCitiesList();
	double unused_freight = player_ptr->GetUnusedFreight();
	double total_freight = player_ptr->GetTotalFreight();
	GoodsRoute new_route;
	GoodsRouteList new_routes;

	
	m_neededFreight = 0.0;

	
	for (sint32 i = 0; i < city_list->Num(); i++) {
		city = city_list->Access(i);

		for (sint32 g = 0; g < g_theResourceDB->NumRecords(); g++) {
			if(city.CD()->IsLocalResource(g)) {
				
				Unit maxCity;
				sint32 maxPrice = 0;
				sint32 bestPrice = 0;
				double maxCost = 0.0;
				double maxNeededFreight = 0.0;
				sint32 sellingPrice = -1;
				TradeRoute curDestRoute;

				
				if(	!city.CD()->HasResource(g) &&	
					city.CD()->GetResourceTradeRoute(g, curDestRoute)) 
				{
					sellingPrice = 
						tradeutil_GetTradeValue(m_playerId, curDestRoute->GetDestination(), g);
				}
				else 
				{
					curDestRoute.m_id = 0;
				}

				
				for (sint32 op = 1; op < k_MAX_PLAYERS; op++) {

					if(!g_player[op]) continue;
					if(m_playerId != op && !player_ptr->HasContactWith(op)) continue;

					
					if(AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, op) >= 0) 
						continue;

					
					if(Diplomat::GetDiplomat(op).GetEmbargo(m_playerId))
						continue;

					
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
						
						
						if(price > maxPrice) 
						{
							
							if ( cost < total_freight)
							{
								maxPrice = price;
								maxCity = destCity;
								maxCost = cost;
							}

						}

					} 
				} 
	
				
				m_neededFreight += maxNeededFreight;

				
				
				if (player_ptr->m_playerType != PLAYER_TYPE_ROBOT)
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

				
				if((maxPrice > 0) && ((sellingPrice < 0) || (sellingPrice < maxPrice))) 
				{
					
					
					new_route.m_sourceCity = city;
					new_route.m_destinationCity = maxCity;
					new_route.m_cost = maxCost;
					new_route.m_value = maxPrice;
					new_route.m_resource = g;
					new_routes.push_back(new_route);
				}
				
			} 
		} 
	} 

	
	m_neededFreight -= total_freight;

	
	new_routes.sort();

    for 
    (
		GoodsRouteList::iterator route_iter = new_routes.begin();
	    (route_iter != new_routes.end()) && (unused_freight > 0);
        ++route_iter
    )
	{
		if (route_iter->m_cost < unused_freight) 
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
