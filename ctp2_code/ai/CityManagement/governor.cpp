//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : AI and automated governor handling.
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Player.h"
#include "Path.h"
#include "installationtree.h"
#include "UnitRecord.h"
#include "CellUnitList.h"
#include "ArmyData.h"
#include "CityData.h"
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
#include "GaiaController.h"
#include "AgreementMatrix.h"
#include "WonderTracker.h"

#include "CtpAi.h"

#include "CityAStar.h"
extern CityAstar g_city_astar;


#include "profileAi.h"

#include <algorithm>
#include "Scheduler.h"
#include "CtpGoal.h"
#include "Governor.h"
#include "MapAnalysis.h"

#include "network.h"
#include "net_action.h"

using namespace std;

extern MapPoint g_mp_size;



Governor::GovernorVector Governor::s_theGovernors;
Governor::TiGoalQueue Governor::s_tiQueue;


void Governor::ResizeAll(const PLAYER_INDEX & newMaxPlayerId)
{
	sint32 old_size = s_theGovernors.size();

	s_theGovernors.resize(newMaxPlayerId);

	
	for (sint32 i = old_size; i < newMaxPlayerId; i++)
		{
			s_theGovernors[i].SetPlayerId(i);
		}
}


void Governor::LoadAll(CivArchive & archive)
{
	
	for (sint32 i = 0; i < s_theGovernors.size(); i++)
		{
			s_theGovernors[i].Load(archive);
		}
}


void Governor::SaveAll(CivArchive & archive)
{
	
	for (sint32 i = 0; i < s_theGovernors.size(); i++)
		{
			s_theGovernors[i].Save(archive);
		}
}


Governor & Governor::GetGovernor(const PLAYER_INDEX & playerId)
{
	Assert(playerId >= 0);
	Assert(playerId < s_theGovernors.size());
	
	return s_theGovernors[playerId]; 
}


Governor::Governor()
{
	m_playerId = -1;

	Initialize();
}


void Governor::Initialize()
{
	
	
	

	
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

	bool config_found = false;
	bool obsolete;
	sint32 government_index = -1;
	for (sint32 gov_index = 0; government_index == -1 && gov_index < strategy.GetNumGovernment(); gov_index++)
		{
			const GovernmentRecord *rec = strategy.GetGovernment(gov_index);
			
			
			if (player_ptr->HasAdvance(rec->GetEnableAdvanceIndex()) == false)
				continue;

			
			obsolete = false;
			for (sint8 i=0; i < rec->GetNumObsoleteAdvance() && !obsolete; i++)
				{
					
					if (player_ptr->HasAdvance(rec->GetObsoleteAdvance(i)->GetIndex())) 
						{
							obsolete = true;
						}
				}
			if (obsolete)
				continue;

			
			sint32 diff = rec->GetTooManyCitiesThreshold() - player_ptr->GetNumCities();
			if (diff < 0)
				continue;

			
			
			

			
			government_index = rec->GetIndex();
		}
	
	
	return government_index;
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
	





void Governor::NormalizeSliders(SlidersSetting & sliders_setting) const
{
		Player * player_ptr = g_player[m_playerId];
		Assert(player_ptr != NULL);

		
		if (player_ptr->GetWorkdayExpectation() - sliders_setting.m_deltaProduction > 2)
			sliders_setting.m_deltaProduction = -2 + player_ptr->GetWorkdayExpectation();
		else if (player_ptr->GetWorkdayExpectation() - sliders_setting.m_deltaProduction < -2)
			sliders_setting.m_deltaProduction = 2 + player_ptr->GetWorkdayExpectation();

		
		if (player_ptr->GetWagesExpectation() - sliders_setting.m_deltaGold > 2)
			sliders_setting.m_deltaGold = -2 + player_ptr->GetWagesExpectation();
		else if (player_ptr->GetWagesExpectation() - sliders_setting.m_deltaGold < -2)
			sliders_setting.m_deltaGold = 2 + player_ptr->GetWagesExpectation();

		
		if (player_ptr->GetRationsExpectation() - sliders_setting.m_deltaFood > 2)
			sliders_setting.m_deltaFood = -2 + player_ptr->GetRationsExpectation();
		else if (player_ptr->GetRationsExpectation() - sliders_setting.m_deltaFood < -2)
			sliders_setting.m_deltaFood = 2 + player_ptr->GetRationsExpectation();
}


sint32 Governor::SetSliders(const SlidersSetting & sliders_setting, const bool & update_cities) const
{
		Player * player_ptr = g_player[m_playerId];
		Assert(player_ptr != NULL);

		
		

		
		player_ptr->SetWorkdayLevel(player_ptr->GetWorkdayExpectation() - sliders_setting.m_deltaProduction);

		
		player_ptr->SetWagesLevel(player_ptr->GetWagesExpectation() - sliders_setting.m_deltaGold);

		
		player_ptr->SetRationsLevel(player_ptr->GetRationsExpectation() - sliders_setting.m_deltaFood );

		
		if (update_cities == false)
			return 0;

		
		UnitDynamicArray *city_list = player_ptr->GetAllCitiesList();
		CityData *city;
		sint32 gold;
		double new_happiness;
		double old_happiness;
		double delta_happiness;
		double total_delta_happiness = 0;
		for (sint16 i = 0; i < city_list->Num(); i++)
		{
			city = city_list->Access(i)->GetCityData();
			old_happiness = city->GetHappiness();
			city->CollectResources();
			
			city->DoSupport(true);
			city->SplitScience(true);
			city->ProcessFood();
			city->CollectOtherTrade(TRUE, FALSE);
			city->ProcessProduction(true);
			
			
			city->CalcHappiness(gold, FALSE);
			city->EatFood();
			city->CalculateGrowthRate();
			new_happiness = city->GetHappiness();
			delta_happiness = new_happiness - old_happiness;
			total_delta_happiness += delta_happiness;
		}
	return (sint32) total_delta_happiness;
}


void Governor::GetSliders(SlidersSetting & sliders_setting) const
{
		Player * player_ptr = g_player[m_playerId];
		Assert(player_ptr != NULL);

		
		sliders_setting.m_deltaProduction = 
			(sint32) (player_ptr->GetWorkdayExpectation() - (sint32) player_ptr->GetUnitlessWorkday());

		
		sliders_setting.m_deltaGold = 
			(sint32) (player_ptr->GetWagesExpectation() - (sint32) player_ptr->GetUnitlessWages());

		
		sliders_setting.m_deltaFood =
			(sint32) (player_ptr->GetRationsExpectation() - (sint32) player_ptr->GetUnitlessRations());
}



bool Governor::ComputeMinimumSliders( SlidersSetting & sliders_setting ) const
{
	bool production_test;
	bool gold_test;
	bool food_test;
	bool happiness_test;
	SlidersSetting tmp_sliders_setting;

	
	bool found = TestSliderSettings( sliders_setting, 
									 production_test,
									 gold_test,
									 food_test,
									 happiness_test);
	
	if (found)
		return false;

	
	SlidersSetting orig_sliders_setting = sliders_setting;

	bool changed = true;
	bool error = false;
	sint32 loop_test = 0;
	while ( found == false && changed == true)
		{
			Assert(loop_test < 10000);
			loop_test++;
			tmp_sliders_setting = sliders_setting;

			
			found = TestSliderSettings( sliders_setting, 
										production_test,
										gold_test,
										food_test,
										happiness_test);
			changed = false;

			
			if (happiness_test == false)
				{
					
					

					
					if ( sliders_setting.m_optimizeProduction == false )
						{
							
							sliders_setting.m_deltaProduction--;
						}
					
					else if ( sliders_setting.m_optimizeGold == false )
						{
							
							sliders_setting.m_deltaGold--;
						}

					
					else if ( sliders_setting.m_optimizeFood == false )
						{
							
							sliders_setting.m_deltaFood--;
						}
					else
						{
							
							Assert(false);
						}
				}

			
			if ( ( sliders_setting.m_deltaProduction < 0 ) &&
				 ( production_test == false ) )
				{
					
					sliders_setting.m_deltaProduction++;
					sliders_setting.m_optimizeProduction = true;
				}

			
			if ( ( sliders_setting.m_deltaGold < 0 ) &&
				 ( gold_test == false ) )
				{
					
					sliders_setting.m_deltaGold++;
					sliders_setting.m_optimizeGold = true;
				}

			
			if ( ( sliders_setting.m_deltaFood < 0 ) &&
				 ( food_test == false ) )
				{
					
					sliders_setting.m_deltaFood++;
					sliders_setting.m_optimizeFood = true;
				}

			
			NormalizeSliders(sliders_setting);
			changed = (sliders_setting != tmp_sliders_setting );
		}

	
	found = TestSliderSettings( sliders_setting, 
								production_test,
								gold_test,
								food_test,
								happiness_test);

	
	if (!found)
		return true;

	changed = (sliders_setting != orig_sliders_setting);
	return changed;
}


bool Governor::ComputeBestSliders( SlidersSetting & sliders_setting ) const
{
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	bool config_found = false;
	bool found;
	SlidersSetting last_sliders_setting = sliders_setting;

	for (sint32 i = 0; i < strategy.GetNumSliderElement(); i++)
		{
			const StrategyRecord::SliderElement *elem = strategy.GetSliderElement(i);

			
			if (elem->GetProduction() == true)
				{
					sliders_setting.m_optimizeProduction = true;
					sliders_setting.m_deltaProduction = elem->GetDelta();

					
					if (sliders_setting.m_deltaGold <= 0)
						{
							sliders_setting.m_deltaGold = -1 * elem->GetDelta();
						}
					if (sliders_setting.m_deltaFood <= 0)
						{
							sliders_setting.m_deltaFood = -1 * elem->GetDelta();
						}

					
					found = FitSlidersToCities( sliders_setting );

					
					sliders_setting.m_optimizeProduction = false;
				}
			else if (elem->GetGold() == true)
				{
					sliders_setting.m_optimizeGold = true;
					sliders_setting.m_deltaGold = elem->GetDelta();

					
					if (sliders_setting.m_deltaProduction <= 0)
						{
							sliders_setting.m_deltaProduction = -1 * elem->GetDelta();
						}
					if (sliders_setting.m_deltaFood <= 0)
						{
							sliders_setting.m_deltaFood = -1 * elem->GetDelta();
						}

					
					found = FitSlidersToCities( sliders_setting );

					
					sliders_setting.m_optimizeGold = false;
				}
			else if (elem->GetFood() == true)
				{
					sliders_setting.m_optimizeFood = true;
					sliders_setting.m_deltaFood = elem->GetDelta();

					
					if (sliders_setting.m_deltaProduction <= 0)
						{
							sliders_setting.m_deltaProduction = -1 * elem->GetDelta();
						}
					if (sliders_setting.m_deltaGold <= 0)
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
			if (found)
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


bool Governor::FitSlidersToCities( SlidersSetting & sliders_setting ) const
{
	

	

	Assert(m_playerId >= 0);
	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	Unit city_unit;
	sint16 num_cities = player_ptr->m_all_cities->Num();
	CityData *city = NULL;
	CityData *unhappy_city = NULL;
	bool production_test;
	bool gold_test;
	bool food_test;
	bool happiness_test;

	
	NormalizeSliders(sliders_setting);

	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	
	bool found = false;
	bool changed = true;

	sint32 loop_test = 0;
	while ( found == false && changed == true )
		{
			Assert(loop_test < 10000);
			loop_test++;

			
			found = TestSliderSettings( sliders_setting, 
										production_test,
										gold_test,
										food_test,
										happiness_test);
			changed = false;
			
			
			if (happiness_test == false)
				{
					
					if ( sliders_setting.m_optimizeProduction == true &&
						  sliders_setting.m_deltaProduction > 0)
						{
							
							sliders_setting.m_deltaProduction--;
							changed = true;
						}
					
					else if ( sliders_setting.m_optimizeGold == true &&
							  sliders_setting.m_deltaGold > 0)
						{
							
							sliders_setting.m_deltaGold--;
							changed = true;
						}

					
					else if ( sliders_setting.m_optimizeFood == true  &&
						      sliders_setting.m_deltaFood > 0)
						{
							
							sliders_setting.m_deltaFood--;
							changed = true;
						}
					else
						{
							
							
							
						}
				}

			
			if ( ( sliders_setting.m_deltaProduction < 0 ) &&
				 ( production_test == false ) )
				{
					
					sliders_setting.m_deltaProduction++;
					changed = true;
				}

			
			if ( ( sliders_setting.m_deltaGold < 0 ) &&
				 ( gold_test == false ) )
				{
					
					sliders_setting.m_deltaGold++;
					changed = true;
				}

			
			if ( ( sliders_setting.m_deltaFood < 0 ) &&
				 ( food_test == false ) )
				{
					
					sliders_setting.m_deltaFood++;
					changed = true;
				}

			
			
			while ( (sliders_setting.m_deltaProduction + 
					 sliders_setting.m_deltaGold + 
					 sliders_setting.m_deltaFood) < 0 )
				{
					
					if ( sliders_setting.m_deltaProduction < 0 ) 
						{
							sliders_setting.m_deltaProduction++;
							changed = true;
						}
					else if ( sliders_setting.m_deltaGold < 0 )
						{
							sliders_setting.m_deltaGold++;
							changed = true;
						}
					else if ( sliders_setting.m_deltaFood < 0 )
						{
							sliders_setting.m_deltaFood++;
							changed = true;
						}
					found = false;
				}
		}

	return found;
}


bool Governor::TestSliderSettings( const SlidersSetting & sliders_setting,
								   bool & production_test,
								   bool & gold_test,
								   bool & food_test,
								   bool & happiness_test) const
{
	const StrategyRecord & strategy = 
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	double deficit_spending;
	sint32 min_happiness;
	double max_wage_percent;

	strategy.GetDeficitSpending(deficit_spending);
	strategy.GetMinimumHappiness(min_happiness);
	strategy.GetMaximumWagePercent(max_wage_percent);

	sint32 total_production = 0;
	sint32 total_gold_cost = 0;
	sint32 gross_gold = 0;
	sint32 total_gold = 0;

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

	
	SetSliders( sliders_setting, false );

	
	sint32 num_cities = player_ptr->m_all_cities->Num();
	for (sint16 city_index = 0; city_index < num_cities; city_index++)
		{
			city_unit = player_ptr->m_all_cities->Get(city_index);

			Assert(city_unit.m_id != 0);
			Assert(city_unit->GetCityData());
			city = city_unit->GetCityData();

			
			
			city->ProcessFood();
			city->CollectOtherTrade(TRUE, FALSE);
			city->ProcessProduction(true);
			
			city->CalcHappiness(gold, FALSE);
			city->EatFood();
			

			
			new_happiness = city->GetHappiness();
			if ( new_happiness < min_happiness )
				{
					happiness_test = false;
				}

			
			total_production += city->GetNetCityProduction();

			
			total_gold_cost += city->GetWagesNeeded();
			total_gold += city->GetNetCityGold();
			gross_gold += city->GetGrossCityGold();
			
			
			if ( city->GetNetCityFood() < 0)
				{
					food_test = false;
				}
		}

	
	total_production += player_ptr->GetProductionFromFranchises();
	if (total_production < player_ptr->GetReadinessCost() )
		{
			production_test = false;
		}

	
	double wages_percent = (double) total_gold_cost / gross_gold;
	if ( wages_percent > max_wage_percent)
		{
			gold_test = false;
		}

	
	
	total_gold_cost += player_ptr->CalcTotalBuildingUpkeep();
	total_gold += player_ptr->GetGold();
	sint32 surplus = gross_gold - total_gold_cost;
	if ( (surplus < 0) && 
		 ((total_gold + surplus) > 0) &&
		 (surplus > (player_ptr->GetGold() * deficit_spending * -1)) )
		{
			gold_test = false;
		}

	return (production_test && gold_test && food_test && happiness_test);
}


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

	
	if (ComputeMinimumSliders(new_sliders_setting) == false)
		{
			
			found = Governor::GetGovernor(m_playerId).ComputeBestSliders(new_sliders_setting);

			if (found)
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

	sint16 num_cities = player_ptr->m_all_cities->Num();
	Unit city_unit;
	Unit neighbor_unit;
	Unit min_neighbor_unit;
	sint32 neighbor_dist;
	sint32 min_dist;
	sint32 city_cont;
	sint32 threat_rank;

	
	for (sint16 city_index = 0; city_index < num_cities; city_index++)
		{
			city_unit = player_ptr->m_all_cities->Get(city_index);

			
			if (city_unit.CD()->GetUseGovernor() == false)
				continue;

			threat_rank = 
				MapAnalysis::GetMapAnalysis().GetThreatRank(city_unit.CD());

			min_dist = g_mp_size.x * g_mp_size.y; 
			min_neighbor_unit = Unit(0);
			city_cont = g_theWorld->GetContinent( city_unit.RetPos() );

			
			for (sint16 neighbor_index = 0; neighbor_index < num_cities; neighbor_index++)
			{
				neighbor_unit = player_ptr->m_all_cities->Get(neighbor_index);

				
				if (neighbor_unit.m_id == city_unit.m_id)
					continue;

				
				
				
				if ( city_cont != g_theWorld->GetContinent( neighbor_unit.RetPos() ) )
					continue;

				
				neighbor_dist = MapPoint::GetSquaredDistance(neighbor_unit.RetPos(), city_unit.RetPos());
				if (neighbor_dist < (min_dist * min_dist))
				{
					min_dist = neighbor_dist;
					min_neighbor_unit = neighbor_unit;
				}
			} 

			
			if (min_neighbor_unit == Unit(0))
				continue;

			
			
			

		    float total_cost = 0.0; 
			Path found_path;
			double trans_max_r = 0.8;
			
			
			
			
 
			if (g_city_astar.FindRoadPath(city_unit.RetPos(), min_neighbor_unit.RetPos(),
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

	
	sint16 num_cities = player_ptr->m_all_cities->Num();
	for (sint16 city_index = 0; city_index < num_cities; city_index++)
		{
			unit = player_ptr->m_all_cities->Get(city_index);
			Assert(unit->GetCityData());
			city = unit->GetCityData();

			
			if (city->GetUseGovernor() == false)
				continue;

			CityInfluenceIterator it(unit.RetPos(), city->GetSizeIndex());
			
			for(it.Start(); !it.End(); it.Next()) 
				{
					
					if (unit.RetPos() == it.Pos())
						continue;

					cell = g_theWorld->GetCell(it.Pos());

					
					if (!(cell->GetCityOwner() == unit))
						continue;

					
					if (cell->GetNumImprovements() > 0)
						continue;

					
					if ( FindBestTileImprovement(it.Pos(), ti_goal) )
						{
							
							s_tiQueue.push_back(ti_goal);
						}
				} 
		} 

	
	
	sint32 max_eval;

	strategy.GetMaxEvalTileImprovements(max_eval);
	max_eval = MIN(max_eval, s_tiQueue.size());
	TiGoalQueue::iterator max_iter = TiGoalQueue::iterator(&s_tiQueue[max_eval]);
	
	std::partial_sort(s_tiQueue.begin(), max_iter, s_tiQueue.end(), greater<TiGoal>());

#if defined(ACTIVISION_ORIGINAL)	// spent_pw is not needed
	sint32 spent_pw = 0;
	sint32 needed_pw;
#endif
	sint32 avail_pw = player_ptr->GetMaterialsStored() - reserve_pw;

	
	TiGoalQueue::const_iterator iter;
	for (iter = s_tiQueue.begin(); iter != max_iter; iter++)
		{
#if defined(ACTIVISION_ORIGINAL)	// Will always leave 1 PW unspent
			needed_pw = terrainutil_GetProductionCost( iter->type, iter->pos, 0 );
			
			
			
			if (spent_pw + needed_pw < avail_pw)
				{
					
					g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_CreateImprovement,
										   GEA_Player, m_playerId,
										   GEA_MapPoint, iter->pos,
										   GEA_Int, iter->type,
										   GEA_Int, 0, 
										   GEA_End);
					spent_pw += needed_pw;
				}
#else
			sint32 const	needed_pw	= 
				terrainutil_GetProductionCost(iter->type, iter->pos, 0);
			if (needed_pw <= avail_pw)
			{
				g_gevManager->AddEvent(GEV_INSERT_Tail, 
									   GEV_CreateImprovement,
									   GEA_Player,		m_playerId,
									   GEA_MapPoint,	iter->pos,
									   GEA_Int,			iter->type,
									   GEA_Int,			0, 
									   GEA_End);
				avail_pw -= needed_pw;
			}
#endif
			else
				
				break;
		}

	
	s_tiQueue.clear();
}
				
//----------------------------------------------------------------------------
//
// Name       : Governor::FindBestTileImprovement
//
// Description: Determine the best tile improvement for a tile.
//
// Parameters : pos			: position of the tile on the map
//
// Globals    : g_player	: list of players in the game
//				g_theWorld	: map information
//
// Returns    : bool		: the tile can be improved
//              goal		: type and priority value of the tile improvement 
//
// Remark(s)  : The information in goal is only valid when true is returned.
//
//----------------------------------------------------------------------------

bool Governor::FindBestTileImprovement(const MapPoint &pos, TiGoal & goal) const
{					
	Assert(g_player[m_playerId]);
	Player *player_ptr = g_player[m_playerId];
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	Unit city_owner = g_theWorld->GetCell(pos)->GetCityOwner();
	MapAnalysis & the_map =  MapAnalysis::GetMapAnalysis();
	double growth_rank = the_map.GetGrowthRank(city_owner.GetCityData(), false);
	double production_rank = the_map.GetProductionRank(city_owner.GetCityData(), false);
	double gold_rank = the_map.GetCommerceRank(city_owner.GetCityData(), false);
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

	
	if (terrain_type == terrainutil_GetDead())
	{
		goal.type = terrainutil_GetTerraformPlainsImprovement();
		goal.utility = 9999.0;
	}
#if defined(ACTIVISION_ORIGINAL)	
	else if ( terr_food_rank > 0.2 &&
		 terr_prod_rank < 1.0 ) 
		{
			
			if (best_growth_improvement < 0)
				return false;
#else
	// Forest (10 food, only 5 production) appears to trigger the condition,
	// but does not have a growth improvement. Instead of just returning false,
	// continue checking other improvement types. This should enable the AI to 
	// build trading posts and other gold improvements on forest tiles.
	else if ((terr_food_rank > 0.2) &&
		     (terr_prod_rank < 1.0) &&
			 (best_growth_improvement >= 0)
            )
		{
#endif
		
			goal.type = best_growth_improvement;
			
			
			strategy.GetImproveGrowthBonus(bonus);
			goal.utility = bonus * terr_food_rank;

			
			if ( growth_rank < 0.2 )
				{
					
				    strategy.GetImproveSmallCityGrowthBonus(bonus);
					goal.utility +=  bonus * (1.0 - growth_rank);
				}

			
			if ( g_theWorld->IsGood(pos) )
				{
				    strategy.GetImproveGoodBonus(bonus);
					goal.utility += bonus;
				}
		}
#if defined(ACTIVISION_ORIGINAL)	
	else if ( terr_prod_rank > 0.2 ) 
		{
			
			if (best_production_improvement < 0)
				return false;
#else
	else if ((terr_prod_rank > 0.2) && 
		     (best_production_improvement >= 0)
            )
		{
#endif
			
			goal.type = best_production_improvement;
			
			
			strategy.GetImproveProductionBonus(bonus);
			goal.utility =  bonus * terr_prod_rank;
			
			
			if ( production_rank > 0.8 )
				{
					
					strategy.GetImproveLargeCityProductionBonus(bonus);
					goal.utility += bonus *	production_rank;
				}

			
			if ( g_theWorld->IsGood(pos) )
				{
					strategy.GetImproveGoodBonus(bonus);
					goal.utility += bonus;
				}
		}
#if defined(ACTIVISION_ORIGINAL)	
	else if ( gold_rank > 0.4 ) 
		{
			
			if (best_gold_improvement < 0)
				return false;
#else
	else if ((gold_rank > 0.4) &&
		     (best_gold_improvement >= 0)
            )
		{
#endif

			goal.type = best_gold_improvement;
			
			
			strategy.GetImproveProductionBonus(bonus);
			goal.utility =  bonus * terr_gold_rank;
			
			if ( production_rank > 0.8 )
				{
					
					strategy.GetImproveLargeCityProductionBonus(bonus);
					goal.utility += bonus *	production_rank;
				}

			
			if ( g_theWorld->IsGood(pos) )
				{
					strategy.GetImproveGoodBonus(bonus);
					goal.utility += bonus;
				}
		}
	
	else if ( g_theWorld->IsGood(pos) == FALSE ) 
		{
			ERR_BUILD_INST err;
			
			if (terrain_type == terrainutil_GetGlacier() ||
				terrain_type == terrainutil_GetSwamp() || 
				terrain_type == terrainutil_GetTundra())
				{
					if (player_ptr->CanCreateImprovement(terrainutil_GetTerraformHillsImprovement(), pos, 0, FALSE, err))
					{
						strategy.GetImproveProductionBonus(bonus);
						
						goal.utility =  bonus * (1.0-production_rank);
						goal.type = terrainutil_GetTerraformHillsImprovement();
					}
				}

			
			else if (terrain_type == terrainutil_GetDesert())
				{
					if (player_ptr->CanCreateImprovement(terrainutil_GetTerraformGrasslandImprovement(), pos, 0, FALSE, err))
					{
						strategy.GetImproveGrowthBonus(bonus);
						
						goal.utility =  bonus * (1.0-growth_rank);
						goal.type = terrainutil_GetTerraformGrasslandImprovement();
					}
				}
		}

#if defined(ACTIVISION_ORIGINAL) || defined(CTP1_HAS_RISEN_FROM_THE_GRAVE)
	// CTP1: utilisation depends on worker placement, and is either 0 or 1.
#else
	// CTP2: utilisation depends on the number of available workers and the 
	// (ring) distance from the city, and may be any fraction from 0.0 to 1.0.
	if ((goal.type >= 0) && city_owner.GetCityData())
	{
		sint32 const	sqDist	= MapPoint::GetSquaredDistance(city_owner.RetPos(), pos);
		goal.utility *= city_owner.GetCityData()->GetUtilisationRatio(sqDist);
	}
	else
	{
		goal.utility = 0.0;
	}
#endif

	return (goal.type >= 0);
}


sint32 Governor::GetBestRoadImprovement(const MapPoint & pos) const
{
	Cell *cell = g_theWorld->GetCell(pos);
	sint32 old_move_cost = cell->GetMoveCost();

	
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

		if (effect->GetBonusFood() &&
			effect->GetBonusFood(tmp_bonus) &&
			(tmp_bonus > max_bonus_food))
		{
			max_bonus_food = tmp_bonus;
		}
		
		if (effect->GetBonusProduction() &&
			effect->GetBonusProduction(tmp_bonus) &&
			(tmp_bonus > max_bonus_prod))
		{
			max_bonus_prod = tmp_bonus;
		}
		
		if (effect->GetBonusGold() &&
			effect->GetBonusGold(tmp_bonus) &&
			(tmp_bonus > max_bonus_gold))
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
		
		
		if (player_ptr->CanCreateImprovement(type, pos, 0, FALSE, err) == FALSE)
			continue;
		
		
		if (current_class != 0x0 && ((rec->GetClass() & current_class) == 0x0))
			continue;

		
		effect = terrainutil_GetTerrainEffect(rec, terrain_type);
		if(effect) 
		{
			if (effect->GetBonusFood() &&
				effect->GetBonusFood(tmp_bonus) &&
				(tmp_bonus > max_bonus_food))
			{
				
				food_imp = type ;
				max_bonus_food = tmp_bonus;
			}
			
			if (effect->GetBonusProduction() &&
				effect->GetBonusProduction(tmp_bonus) &&
				(tmp_bonus > max_bonus_prod))
			{
				
				prod_imp = type;
				max_bonus_prod = tmp_bonus;
			}

			if (effect->GetBonusGold() &&
				effect->GetBonusGold(tmp_bonus) &&
				(tmp_bonus > max_bonus_gold))
			{
				
				gold_imp = type;
				max_bonus_gold = tmp_bonus;
			}

		}
	}
}






void Governor::AssignPopulations()
{
	Assert(g_player[m_playerId]);

	
	UnitDynamicArray *city_list = g_player[m_playerId]->GetAllCitiesList();
	CityData *city;
	for (sint16 i = 0; i < city_list->Num(); i++)
	{
		city = city_list->Access(i)->GetCityData();

		
		if (city->GetUseGovernor() == FALSE &&
			g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT)
			continue;

		
		if (city->GetSizeIndex() > 0)
			
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
					rank = MapAnalysis::GetMapAnalysis().
						GetProductionRank(city, false);
				}

			
			else if ( elem->GetGrowthCities() ) 
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetGrowthRank(city, false);
				}
			
			else if ( elem->GetGoldCities() ) 
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetCommerceRank(city, false);
				}
			
			
			else if ( elem->GetDefault() )
				{
					
					break;
				}
			else 
				{
					
					Assert(false);
					break;
				}

			
			if ( elem->GetTop() ) 
				{
					elem->GetTop(top_value);
					found &= (rank >= 1.0 - top_value);
				}
			
			
			if ( elem->GetBottom() )
				{
					elem->GetBottom(bottom_value);
					found &= (rank <= bottom_value);
				}
		}

	
	Assert(found);
	Assert(elem);

	return elem;
}


void Governor::AssignPopulation(CityData *city)
{
	
	CityData *tmp_city = new CityData(city);

	
	tmp_city->ChangeSpecialists(POP_ENTERTAINER, -1 * city->EntertainerCount());
	tmp_city->ChangeSpecialists(POP_FARMER, -1 * tmp_city->FarmerCount());
	tmp_city->ChangeSpecialists(POP_LABORER, -1 * tmp_city->LaborerCount());
	tmp_city->ChangeSpecialists(POP_MERCHANT, -1 * tmp_city->MerchantCount());
	tmp_city->ChangeSpecialists(POP_SCIENTIST, -1 * tmp_city->ScientistCount());
	
	
    const StrategyRecord::PopAssignmentElement *pop_assignment = GetMatchingPopAssignment(city);

	
	sint32 min_entertainers;
	sint32 max_entertainers;
	ComputeMinMaxEntertainers(city, min_entertainers, max_entertainers);

	
	sint32 min_workers = ComputeMinimumFoodWorkers(city);

	
	double specialists_percent = pop_assignment->GetSpecialists();

	

	
	
	sint32 size_index, full_index, part_index;
	city->ComputeSizeIndexes(city->PopCount(), size_index, full_index, part_index);

	
	sint32 max_workers = g_theCitySizeDB->Get(size_index)->GetMaxWorkers();
	if (max_workers > city->PopCount())
		max_workers = city->PopCount();

	
	sint32 specialists = floor((city->PopCount() - (min_workers + min_entertainers)) * specialists_percent);

	sint32 non_specialists = (city->PopCount() - (specialists + min_entertainers));

	
	
	if (non_specialists > max_workers)
		{
			specialists += non_specialists - max_workers;
			non_specialists = (city->PopCount() - (specialists + min_entertainers));
		}

	
	sint32 assigned = 0;
	sint32 count;
	sint32 delta;
	double prev_result;

	
	sint32 best_specialist = city->GetBestSpecialist(POP_ENTERTAINER);

	if ((best_specialist > 0) && (assigned < specialists))
	{
		count = ceil(specialists * pop_assignment->GetEntertainerPercent());
		
		
		count = (count >= min_entertainers ? count : min_entertainers);
		
		
		count = (count <= max_entertainers ? count : max_entertainers);

		delta = (count - city->EntertainerCount());
		city->ChangeSpecialists(POP_ENTERTAINER, delta);
		tmp_city->ChangeSpecialists(POP_ENTERTAINER, delta);
		assigned += count;
	}

	
	
	

	
	

	
	best_specialist = city->GetBestSpecialist(POP_FARMER);

	
	count = ceil(specialists * pop_assignment->GetFarmerPercent());
	count = (count + assigned <= specialists ? count : (specialists - assigned));

	if ((best_specialist > 0) && (assigned < specialists) && (count > 0))
		{
			
			tmp_city->CollectResources();
			tmp_city->ProcessFood();
			prev_result = tmp_city->GetProducedFood();

			tmp_city->ChangeSpecialists(POP_FARMER, count);
			tmp_city->CollectResources();
			tmp_city->ProcessFood();

			
			if (tmp_city->GetProducedFood() > prev_result)
			{
				
				delta = (count - city->FarmerCount());
				city->ChangeSpecialists(POP_FARMER, delta);
				assigned += count;
			}
			
			else if (tmp_city->GetProducedFood() < prev_result)
			{
				delta = (-1 * city->FarmerCount());
				city->ChangeSpecialists(POP_FARMER, delta);
			}

			
			delta = city->FarmerCount() - tmp_city->FarmerCount();
			tmp_city->ChangeSpecialists(POP_FARMER, delta );
		}

	
	best_specialist = city->GetBestSpecialist(POP_LABORER);

	
	count = ceil(specialists * pop_assignment->GetLaborerPercent());
	count = (count + assigned <= specialists ? count : (specialists - assigned));

	if ((best_specialist > 0) && (assigned < specialists) && (count > 0))
		{
			
			tmp_city->CollectResources();
			tmp_city->ProcessProduction(true);
			prev_result = tmp_city->GetGrossCityProduction();

			tmp_city->ChangeSpecialists(POP_LABORER, count);
			tmp_city->CollectResources();
			tmp_city->ProcessProduction(true);

			
			if (tmp_city->GetGrossCityProduction() > prev_result)
			{
				delta = (count - city->LaborerCount());
				city->ChangeSpecialists(POP_LABORER, delta);
				assigned += count;
			}
			
			else if (tmp_city->GetGrossCityProduction() < prev_result)
			{
				delta = (-1 * city->LaborerCount());
				city->ChangeSpecialists(POP_LABORER, delta);
			}

			
			delta = city->LaborerCount() - tmp_city->LaborerCount();
			tmp_city->ChangeSpecialists(POP_LABORER, delta );

		}

	
	best_specialist = city->GetBestSpecialist(POP_MERCHANT);

	
	count = ceil(specialists * pop_assignment->GetMerchantPercent());
	count = (count + assigned <= specialists ? count : (specialists - assigned));

	if ((best_specialist > 0) && (assigned < specialists) && (count > 0))
		{
			
			tmp_city->CollectResources();
			tmp_city->CollectOtherTrade(TRUE, FALSE);
			prev_result = tmp_city->GetGrossCityGold();

			tmp_city->ChangeSpecialists(POP_MERCHANT, count);
			tmp_city->CollectResources();
			tmp_city->CollectOtherTrade(TRUE, FALSE);

			
			if (tmp_city->GetGrossCityGold() > prev_result)
			{
				delta = (count - city->MerchantCount());
				city->ChangeSpecialists(POP_MERCHANT, delta);
				assigned += count;
			}
			
			else if (tmp_city->GetGrossCityGold() < prev_result)
			{
				delta = (-1 * city->MerchantCount());
				city->ChangeSpecialists(POP_MERCHANT, delta);
			}

			
			delta = city->MerchantCount() - tmp_city->MerchantCount();
			tmp_city->ChangeSpecialists(POP_MERCHANT, delta );
		}

	
	best_specialist = city->GetBestSpecialist(POP_SCIENTIST);

	
	count = ceil(specialists * pop_assignment->GetScientistPercent());
	count = (count + assigned <= specialists ? count : (specialists - assigned));

	if ((best_specialist > 0) && (assigned < specialists) && (count > 0))
		{
			
			
			

			delta = (count - city->ScientistCount());
			city->ChangeSpecialists(POP_SCIENTIST, delta);
			assigned += count;
		}


	
	

	
	delete tmp_city;
}


void Governor::ComputeMinMaxEntertainers(const CityData *city, sint32 & min, sint32 & max)
{
	sint32 size_index, full_index, partial_index;
	sint32 workers = city->WorkerCount();
	sint32 entertainer_delta = 0;
	city->ComputeSizeIndexes(workers, size_index, full_index, partial_index);

	
	min = 0;
	max = 0;

	sint32 entertainer_type = city->GetBestSpecialist(POP_ENTERTAINER);

	
	if (entertainer_type < 0)
		return;

	sint32 per_pop_happiness = g_thePopDB->Get(entertainer_type)->GetHappiness();

	
	sint32 needed = g_theConstDB->GetRiotLevel();
	sint32 maximum = g_theConstDB->GetVeryHappyThreshold();
	sint32 current = city->GetHappiness();
	double min_delta;
	double max_delta;

	
	if (per_pop_happiness <= 0)
		{
			
			Assert(0);
			return;
		}
	
	
	min_delta = (double)(needed - current) / per_pop_happiness;
	max_delta = (double)(maximum - current) / per_pop_happiness;

	
	
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


sint32 Governor::ComputeMinimumFoodWorkers(const CityData *city)
{
	
	
	sint32 full_radii_food;
	sint32 part_radii_food;
	sint32 full_radii;
	double utilization_needed;
	const CitySizeRecord *part_rec;
	const CitySizeRecord *full_rec;
	MapPoint center = city->GetHomeCity().RetPos();
	sint32 workers_needed = 0;

	
	for(sint32 sizeIndex = 0; sizeIndex < g_theCitySizeDB->NumRecords(); sizeIndex++) {
		part_rec = g_theCitySizeDB->Get(sizeIndex);

		if (sizeIndex > 0) {
			full_rec = g_theCitySizeDB->Get(sizeIndex-1);
			full_radii = full_rec->GetSquaredRadius();
		}
		else {
			full_radii = 0;
		}

		
		full_radii_food = 0;
		part_radii_food = 0;
		CityInfluenceIterator it(center, sizeIndex);
		for(it.Start(); !it.End(); it.Next()) {
			Cell *cell = g_theWorld->GetCell(it.Pos());

			if (MapPoint::GetSquaredDistance(center, it.Pos()) < full_radii) {
				full_radii_food += cell->GetFoodProduced();
			} 
			else {
				part_radii_food += cell->GetFoodProduced();
			}
		}

		
		if (full_radii_food + part_radii_food > city->GetFoodRequired()) {
			break;
		}

		
		if(part_rec->GetPopulation() > city->PopCount() ) {
			break;
		}
	}
	
	
	utilization_needed = 
		(city->GetFoodRequired() - full_radii_food) / part_radii_food;

	
	if (full_radii > 0) {
		workers_needed = full_rec->GetMaxWorkers();
	}
	
	workers_needed += (sint32) ceil(utilization_needed * 
									(part_rec->GetMaxWorkers() - workers_needed));

	return workers_needed;
}






void Governor::ComputeDesiredUnits()
{
	
	m_currentUnitCount.resize(g_theUnitDB->NumRecords());

	sint16 list_num;
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const UnitBuildListRecord *build_list_rec;
	sint32 best_unit_type;

	double unit_support_percent; 
	sint32 total_unit_support;
	double unit_support_percent_by_type;
	sint32 total_unit_support_by_type;
	sint32 total_unallocated_support;
	sint32 garrison_count;
	sint32 army_index;
	sint32 city_index;
	sint32 unit_index;
	Player *player_ptr;
	Army army;
	Unit unit;
	sint32 desired_count;
	double freight_per_unit;

	sint32 max_cities = 
		g_theGovernmentDB->Get(g_player[m_playerId]->GetGovernmentType())->GetTooManyCitiesThreshold();
	sint32 num_cities = g_player[m_playerId]->GetNumCities();
	sint32 needed_cities = (max_cities - num_cities);

	
	
	total_unallocated_support = 0;
	
	
	Assert(strategy.GetUnitSupportPercent());
	strategy.GetUnitSupportPercent(unit_support_percent);
	
	Assert(g_player[m_playerId]);
	player_ptr = g_player[m_playerId];
	total_unit_support = player_ptr->GetTotalProduction() * unit_support_percent;
	
	Assert(g_theUnitDB);
	
	
	for (best_unit_type=0; best_unit_type < g_theUnitDB->NumRecords(); best_unit_type++)
		m_currentUnitCount[best_unit_type]=0;
	
	
	sint32 num_armies = player_ptr->m_all_armies->Num();
	for (army_index = 0; army_index < num_armies; army_index++)
	{
		army = player_ptr->m_all_armies->Get(army_index);

		for (unit_index = 0; unit_index < army->Num(); unit_index++)
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
		
		
		desired_count = 0;
		
		
		m_buildUnitList[list_num].m_desiredCount = 0;
		m_buildUnitList[list_num].m_maximumCount = 0;

		build_list_rec = GetBuildListRecord(strategy, (BUILD_UNIT_LIST) list_num);
		
		
		switch ( list_num )
		{
		case BUILD_UNIT_LIST_OFFENSE:
			
			Assert(strategy.GetOffensiveGarrisonCount());
			strategy.GetOffensiveGarrisonCount(garrison_count);	
			m_buildUnitList[list_num].m_perCityGarrison = garrison_count;
			
			
			Assert(strategy.GetOffensiveUnitsPercent());
			strategy.GetOffensiveUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_DEFENSE:
			
			Assert(strategy.GetDefensiveGarrisonCount());
			strategy.GetDefensiveGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison = garrison_count;
			
			
			Assert(strategy.GetDefensiveUnitsPercent());
			strategy.GetDefensiveUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_RANGED:
			
			Assert(strategy.GetRangedGarrisonCount());
			strategy.GetRangedGarrisonCount(garrison_count);
			m_buildUnitList[list_num].m_perCityGarrison = garrison_count;
			
			
			Assert(strategy.GetRangedUnitsPercent());
			strategy.GetRangedUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_SEA:
			
			Assert(strategy.GetSeaUnitsPercent());
			strategy.GetSeaUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_AIR:
			
			Assert(strategy.GetAirUnitsPercent());
			strategy.GetAirUnitsPercent(unit_support_percent_by_type);
			break;
			
		case BUILD_UNIT_LIST_SETTLER:
			
			Assert( strategy.GetSettlerUnitsCount() );
			strategy.GetSettlerUnitsCount(desired_count);
			break;
			
		case BUILD_UNIT_LIST_SPECIAL:
			
			Assert( strategy.GetSpecialUnitsCount() );
			strategy.GetSpecialUnitsCount(desired_count);
			break;
			
		case BUILD_UNIT_LIST_SEA_TRANSPORT:
			
			Assert( strategy.GetSeaTransportUnitsCount() );
			strategy.GetSeaTransportUnitsCount(desired_count);
			break;

		case BUILD_UNIT_LIST_AIR_TRANSPORT:
			
			Assert( strategy.GetAirTransportUnitsCount() );
			strategy.GetAirTransportUnitsCount(desired_count);
			break;
			
		case BUILD_UNIT_LIST_FREIGHT:
			
			m_buildUnitList[list_num].m_perCityGarrison = 0;
			
			
			best_unit_type = ComputeBestUnitType(build_list_rec);
			m_buildUnitList[list_num].m_bestType = best_unit_type;
			
			if (best_unit_type >= 0)
			{
				
				freight_per_unit = g_theUnitDB->Get(best_unit_type)->GetMaxMovePoints();
				desired_count = ceil(m_neededFreight / freight_per_unit);
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
			
			total_unit_support_by_type = total_unit_support * 
				unit_support_percent_by_type;
			
			
			if (best_unit_type >= 0)
			{
				
				total_unit_support_by_type += total_unallocated_support;
				
				
				
				
				if (g_theUnitDB->Get(best_unit_type)->GetShieldHunger() > 0)
				{
					m_buildUnitList[list_num].m_maximumCount = floor(total_unit_support_by_type / 
						g_theUnitDB->Get(best_unit_type)->GetShieldHunger());
					
					
					total_unallocated_support = total_unit_support_by_type - 
						m_buildUnitList[list_num].m_maximumCount *
						g_theUnitDB->Get(best_unit_type)->GetShieldHunger();
				}
				else 
				{
					
					m_buildUnitList[list_num].m_maximumCount = total_unit_support_by_type;
					
					
					total_unallocated_support = total_unit_support_by_type;
				}
				m_buildUnitList[list_num].m_desiredCount = 
					m_buildUnitList[list_num].m_maximumCount - 
					m_currentUnitCount[best_unit_type];

				
				m_maximumUnitShieldCost += 
					(m_buildUnitList[list_num].m_maximumCount *
					 g_theUnitDB->Get(best_unit_type)->GetShieldCost());
				
				
				m_currentUnitShieldCost += 
					(m_currentUnitCount[best_unit_type] *
					 g_theUnitDB->Get(best_unit_type)->GetShieldCost());
			}
			else 
			{
				
				total_unallocated_support += total_unit_support_by_type;
			}
			break;
		case BUILD_UNIT_LIST_SETTLER:
			
			
			if (desired_count - m_currentUnitCount[best_unit_type] > 0)
			{
				m_buildUnitList[list_num].m_maximumCount = 	needed_cities;
			}
			else
			{
				m_buildUnitList[list_num].m_maximumCount = 	0;
			}
			
		case BUILD_UNIT_LIST_SPECIAL:
		case BUILD_UNIT_LIST_FREIGHT:
			if (best_unit_type >= 0)
			{
				
				
				
				m_buildUnitList[list_num].m_desiredCount = desired_count - m_currentUnitCount[best_unit_type];
			}
			break;
		case BUILD_UNIT_LIST_SEA_TRANSPORT:
			if (best_unit_type >= 0)
				{
					
					m_buildUnitList[list_num].m_desiredCount = desired_count;

					
					m_buildUnitList[list_num].m_desiredCount -= 
						(m_currentUnitCount[best_unit_type] - needed_transport);
				}
			break;

		case BUILD_UNIT_LIST_AIR_TRANSPORT:
			if (best_unit_type >= 0)
				{
					
					if (m_buildUnitList[BUILD_UNIT_LIST_SEA_TRANSPORT].m_desiredCount == 0) 
					{
						
						m_buildUnitList[list_num].m_desiredCount = desired_count;

						
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
			for (unit_index = 0; unit_index < units_ptr->Num(); unit_index++)
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
						desired_offense;
				}

			
			if ( desired_defense > 0 )
				{
					m_buildUnitList[BUILD_UNIT_LIST_DEFENSE].m_garrisonCount += 
						desired_defense;
				}

			
			if ( desired_ranged > 0 )
				{
					m_buildUnitList[BUILD_UNIT_LIST_RANGED].m_garrisonCount +=
						desired_ranged;
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

	for (sint16 i = 0; i < city_list->Num(); i++)
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

		
		if (city->GetUseGovernor() == FALSE &&
			g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT)
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
				city->BuildCapitalization();
				break;
			case k_GAME_OBJ_TYPE_INFRASTRUCTURE:
				insert_ok = true;
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

	
	return (0.0);
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

	
	double percent_unbuilt;
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

			percent_unbuilt = PercentUnbuilt((BUILD_UNIT_LIST) list_num);
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

	
	if ( (g_player[m_playerId]->GetPlayerType() != PLAYER_TYPE_ROBOT) )
		{
			if (city->GetUseGovernor())
			{
				Assert(city->GetBuildListSequenceIndex() >= 0);
				build_list_sequence = g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());
			}
		}

	
	
	if (city->GetUseGovernor() == false || g_player[m_playerId]->GetPlayerType() == PLAYER_TYPE_ROBOT)
	{
		
		sint32 suggested_sequence = build_list_sequence->GetIndex();
		if (suggested_sequence >= 0)
			city->SetBuildListSequenceIndex(suggested_sequence);
	}

	
	
	
	const BuildListSequenceRecord::BuildListElement *elem;

	bool city_full = 
		(g_theWorld->GetCell(city->GetHomeCity().RetPos())->GetNumUnits() >= k_MAX_ARMY_SIZE);
	
	
	
	for ( int elem_num = 0; elem_num < build_list_sequence->GetNumBuildListElement(); elem_num++)
		{
			elem = build_list_sequence->GetBuildListElement(elem_num);

			
			if ( elem->GetAllUnitBuildList() && !city_full) 
				{
					type = GetNeededUnitType(city, list_num);
					cat = k_GAME_OBJ_TYPE_UNIT;

					
					if (type > -1)
					{
						
						Assert(city->CanBuildUnit(type));

						break;	
					}
					
				}
			
			if ( elem->GetGarrisonUnitBuildList() && !city_full) 
				{
					type = GetNeededGarrisonUnitType(city, list_num);
					cat = k_GAME_OBJ_TYPE_UNIT;

					
					if (type > -1)
					{
						
						Assert(city->CanBuildUnit(type));

						break;
					}
					
				}
			
			else if ( elem->GetBuildingBuildList() )
				{
					type = GetNeededBuildingType(city, elem->GetBuildingBuildListPtr());
					cat = k_GAME_OBJ_TYPE_IMPROVEMENT;

					
					if (type > -1 && city->CanBuildBuilding(type))
						break;
					
				}
			
			else if ( elem->GetWonderBuildList() )
				{
					type = GetNeededWonderType(city, elem->GetWonderBuildListPtr());
					cat = k_GAME_OBJ_TYPE_WONDER;

					
					if (type > -1 && city->CanBuildWonder(type))
						break;
					
				}
			
			else if ( elem->GetFreight() )
				{
					type = GetNeededFreightType(list_num);
					cat = k_GAME_OBJ_TYPE_UNIT;
				
					if (type > -1)
						break;
					
				}
			
			else if ( elem->GetInfrastructure() )
				{
					
					if ( city->CanBuildInfrastructure() )
						{
							type = 1;
							cat = k_GAME_OBJ_TYPE_INFRASTRUCTURE;
							
							break;
						}
					
				}
			
			else if ( elem->GetCapitalization() )
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

	
	if ( city->GetUseGovernor() && human_city)
		return g_theBuildListSequenceDB->Get(city->GetBuildListSequenceIndex());

	
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	
	double top_value;
	double bottom_value;
	double rank; 
	const StrategyRecord::BuildListSequenceElement *elem = NULL;
	const StrategyRecord::BuildListSequenceElement *best_elem = NULL;
	sint32 best_priority = -99999;

	for (int elem_num = 0; elem_num < strategy.GetNumBuildListSequenceElement(); elem_num++)
		{
			
			elem = strategy.GetBuildListSequenceElement(elem_num);

			
			if (best_elem && elem->GetPriority() < best_priority)
				continue;

			
			if ( elem->GetProductionCities() ) 
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetProductionRank(city, false);
				}

			
			else if ( elem->GetGrowthCities() ) 
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetGrowthRank(city, false);
				}
			
			else if ( elem->GetCommerceCities() ) 
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetCommerceRank(city, false);
				}
			
			else if (elem->GetHappyCities())
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetHappinessRank(city);
				}
			
			else if (elem->GetThreatenedCities())
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetThreatRank(city);
				}
			
			else if (elem->GetPowerCities())
				{
					rank = MapAnalysis::GetMapAnalysis().
						GetPowerRank(city);
				}
			
			else if ( elem->GetDefault() )
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
					
					Assert(false);
					continue;
				}

			
			if ( elem->GetTop() ) 
				{
					elem->GetTop(top_value);
					if (rank >= 1.0 - top_value)
					{
						best_priority = elem->GetPriority();
						best_elem = elem;
					}
				}
			
			
			if ( elem->GetBottom() )
				{
					elem->GetBottom(bottom_value);
					if (rank <= bottom_value)
					{
						best_priority = elem->GetPriority();
						best_elem = elem;
					}
				}
		}


	
	Assert(best_elem);

	
	if (best_elem == NULL)
	{
		advice = -1;
		best_elem = strategy.GetBuildListSequenceElement(elem_num-1);
		if (best_elem == NULL)
			return g_theBuildListSequenceDB->Get(0);
	}

	
	if (best_elem->GetAdvice())
		best_elem->GetAdvice(advice);
	else
		advice = -1;

	return best_elem->GetBuildListSequence();
}


sint32 Governor::GetNeededUnitType(const CityData *city, sint32 & list_num) const
{
	Assert( g_player[m_playerId] );

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

	bool can_build_settler = false;

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
	return type;
}


const UnitBuildListRecord * Governor::GetBuildListRecord(const StrategyRecord & strategy, const BUILD_UNIT_LIST list_type) const
{
	const UnitBuildListRecord * build_list_rec;
	switch ( list_type )
	{
	case BUILD_UNIT_LIST_OFFENSE:
		
		Assert(strategy.GetOffensiveUnitList());
		build_list_rec = strategy.GetOffensiveUnitListPtr();
		break;
	case BUILD_UNIT_LIST_DEFENSE:
		
		Assert(strategy.GetDefensiveUnitList());
		build_list_rec = strategy.GetDefensiveUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_RANGED:
		
		Assert(strategy.GetRangedUnitList());
		build_list_rec = strategy.GetRangedUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SEA:
		
		Assert(strategy.GetSeaUnitList());
		build_list_rec = strategy.GetSeaUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_AIR:
		
		Assert(strategy.GetAirUnitList());
		build_list_rec = strategy.GetAirUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SETTLER:
		
		Assert(strategy.GetSettlerUnitList());
		build_list_rec = strategy.GetSettlerUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SPECIAL:
		
		Assert(strategy.GetSpecialUnitList());
		build_list_rec = strategy.GetSpecialUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_SEA_TRANSPORT:
		
		Assert(strategy.GetSeaTransportUnitList());
		build_list_rec = strategy.GetSeaTransportUnitListPtr();
		break;
		
	case BUILD_UNIT_LIST_AIR_TRANSPORT:
		
		Assert(strategy.GetAirTransportUnitList());
		build_list_rec = strategy.GetAirTransportUnitListPtr();
		break;

	case BUILD_UNIT_LIST_FREIGHT:
		
		Assert(strategy.GetFreightUnitList());
		build_list_rec = strategy.GetFreightUnitListPtr();
		break;
	default:
		
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

	BUILD_UNIT_LIST max_list = BUILD_UNIT_LIST_MAX;
	sint32 max_production = 0;
	sint32 needed_production = 0;
	sint32 type = -1; 
	CellUnitList garrison_army;

	if ( city->GetGarrisonComplete() && 
		 city->GetGarrisonOtherCities() == FALSE)
		
		return -1;

	
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

	sint32 building_type;

	
	for (int i = 0; i < build_list_rec->GetNumBuilding(); i++)
		{
			building_type = build_list_rec->GetBuildingIndex(i);

			
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

	sint32 wonder_type;

	
	for (int i = 0; i < build_list_rec->GetNumWonder(); i++)
		{
			wonder_type = build_list_rec->GetWonderIndex(i);

			
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

	sint32 unit_type;

	
	for (int i = build_list_rec->GetNumUnit()-1; i >= 0; i--)
		{
			unit_type = build_list_rec->GetUnit(i)->GetIndex();

			
			if (city != NULL && !city->CanBuildUnit(unit_type))
				continue;

			
			if ( g_player[m_playerId]->CanBuildUnit(unit_type) )
				return unit_type;
		}

	
	return -1;
}






sint32 Governor::ComputeBestMilitaryReadiness() const
{
	
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	sint32 new_level;
	if (strategy.GetReadinessLevel())
		strategy.GetReadinessLevel(new_level);

	Assert(g_player[m_playerId]);
	Player * player_ptr = g_player[m_playerId];

	double max_support_cost = 1.0;
	sint32 total_production = player_ptr->GetTotalProduction();
	if (strategy.GetMaxSupportCostPercent())
		{
			sint32 support_percent;
			strategy.GetMaxSupportCostPercent(support_percent);
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

	
	if (player_ptr == NULL)
		return -1;

	
	sint32 defend_goal_type = CtpAi::GetGoalDefendIndex();

	bool is_satisfied = false;
	CTPGoal_ptr ctp_goal_ptr = 
		(CTPGoal_ptr) Scheduler::GetScheduler(m_playerId).GetHighestPriorityGoal(defend_goal_type, is_satisfied);
	if (ctp_goal_ptr != NULL)
	{
		sc.AddCity(ctp_goal_ptr->Get_Target_City());
		return defendAreaAdviceId;
	}

	
	sint32 desired_readiness = ComputeBestMilitaryReadiness();
	Assert(g_player[m_playerId])
	if (g_player[m_playerId] &&
		desired_readiness != g_player[m_playerId]->GetReadinessLevel())
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
		(CTPGoal_ptr) Scheduler::GetScheduler(m_playerId).GetHighestPriorityGoal(seige_goal_type, is_satisfied);
	if (ctp_goal_ptr != NULL)
	{
		sc.AddCity(ctp_goal_ptr->Get_Target_City());
		return seigeAdviceId;
	}

	sint32 num_cities = player_ptr->m_all_cities->Num();

	CellUnitList garrison;
	Unit city;
	sint32 i,j;
	for (i = 0; i < num_cities; i++)
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
	if ( launch_target != -1)
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

#ifdef _DEBUG
	
	typedef list<GoodsRoute, dbgallocator<GoodsRoute> > GoodsRouteList;
#else
	typedef list<GoodsRoute> GoodsRouteList;
#endif


void Governor::ManageGoodsTradeRoutes()
{
	Assert(g_player[m_playerId] != NULL);
	Player *player_ptr = g_player[m_playerId];

	sint32 cur_round = player_ptr->GetCurRound();

	Unit city;
	sint16 i,g,d;;
	UnitDynamicArray *city_list = player_ptr->GetAllCitiesList();
	double unused_freight = player_ptr->GetUnusedFreight();
	double total_freight = player_ptr->GetTotalFreight();
	GoodsRoute new_route;
	GoodsRouteList new_routes;

	
	m_neededFreight = 0.0;

	
	for (i = 0; i < city_list->Num(); i++) {
		city = city_list->Access(i);

		
		for(g = 0; g < g_theResourceDB->NumRecords(); g++) {
			if(city.CD()->IsLocalResource(g)) {
				
				sint32 op;
				Unit maxCity;
				sint32 maxPrice = 0;
				sint32 bestPrice = 0;
				double maxCost = 0.0;
				double maxNeededFreight = 0.0;
				sint32 sellingPrice = -1;
				TradeRoute curDestRoute;

				
				if(	city.CD()->HasResource(g) == FALSE &&	
					city.CD()->GetResourceTradeRoute(g, curDestRoute)) 
				{
					sellingPrice = 
						tradeutil_GetTradeValue(m_playerId, curDestRoute->GetDestination(), g);
				}
				else 
				{
					curDestRoute.m_id = 0;
					sellingPrice = -1;
				}

				
				for(op = 1; op < k_MAX_PLAYERS; op++) {

					if(!g_player[op]) continue;
					if(m_playerId != op && !player_ptr->HasContactWith(op)) continue;

					
					if(AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, op) >= 0) 
						continue;

					
					if(Diplomat::GetDiplomat(op).GetEmbargo(m_playerId))
						continue;

					
					for(d = 0; d < g_player[op]->m_all_cities->Num(); d++) {
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

	
	GoodsRouteList::iterator route_iter = new_routes.begin();
	while (route_iter != new_routes.end() && unused_freight > 0)
	{
		if (route_iter->m_cost < unused_freight) 
		{
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendGood,
				GEA_Int, route_iter->m_resource,
				GEA_City, route_iter->m_sourceCity,
				GEA_City, route_iter->m_destinationCity,
				GEA_End);
			unused_freight -= route_iter->m_cost;
		}
	
		
		route_iter++;
	}
}
