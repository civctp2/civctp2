#include "c3.h"


#include "FSFuzzy.h"
#include "CivArchive.h"


#include "IC3GameState.h"
#include "IC3Science.h"
#include "IC3Player.h"
#include "IC3World.h"
#include "IC3Government.h"
#include "IC3Wonder.h"

#include "Agreement.h"

#include "AiMain.h"
#include "AiMap.h"
#include "AiRndCnt.h"
#include "AiStats.h"
#include "Gold.h"


#include "Foreigner.h"
#include "ForeignAgent.h"
#include "ScienceAgent.h"


#include "ArmyAgent.h"
#include "CityAgent.h"


#include "GlOvertime.h"


#include "OTUtilityList.h"

#include "WallClock.h"
extern Wall_Clock *g_wall_clock;

#include "FzOut.h"
extern double fz_land_continents_full_of_friends;
extern double fz_land_continents_full;
extern double fz_closest_capitol;

extern double fz_i_agreed_to_stop_polluting ;
extern double fz_i_want_to_stop_others_who_pollute ;

extern double fz_unit_to_city_best_human_ratio;

#include "Cont.h"

#include "fliif.h"
#include "FliEngine.h"
#include "FsFuzzy.h"
#include "FliSymbol.h"
#include "AllocWGF.h"
#include "ForeignCity.h"
#include "Foreigner.h"
#include "IC3Population.h"


extern double fz_count; 



extern double fz_time; 
extern double fz_diff_level;
extern double fz_max_cities_before_penalty;
extern double fz_city_delta; 

extern double fz_num_cities; 
extern double fz_wgf_prod_setting; 
extern double fz_wgf_gold_setting; 
extern double fz_wgf_food_setting;



extern double fz_someone_built_space;
extern double fz_someone_built_wormhole;
extern double fz_someone_built_undersea;

extern double fz_unit_to_city_ratio;



extern double fz_budget_income_new_blg; 
extern double fz_budget_income_wages; 
extern double fz_budget_income_science; 
extern double fz_budget_income_savings; 
extern double fz_budget_savings_diplomacy; 
extern double fz_budget_savings_overtime; 
extern double fz_budget_savings_reserve; 

extern double fz_my_current_savings;

extern double fz_percent_wages_needed_content; 

extern double fz_income_expense_cleric; 
extern double fz_income_expense_crime;
extern double fz_income_expense_maintenance;  
extern double fz_income_expense_wages; 
extern double fz_income_expense_science; 
extern double fz_income_expense_savings; 
extern double fz_gold_and_wage_percent_cost;

extern double fz_base_gold_reserve_multiple;
extern double fz_special_action_gold_reserve_multiple;

extern double fz_save_expense_overtime; 
extern double fz_save_expense_diplomacy; 


extern double fz_sci_ave_turns_to_advance; 
extern double fz_sci_turns_to_current_advance;
extern double fz_percent_cost_readiness; 
extern double fz_desired_military_readiness; 



extern double fz_num_players_in_game;  
extern double fz_at_war_count; 
extern double fz_longest_war;
extern double fz_shortest_peace;
extern double fz_total_military_incursions;
extern double fz_lowest_unit_regard;

extern double fz_size_of_world; 

extern double fz_min_distance_to_capitol; 
extern double fz_distance_to_nearest_human_capitol;
extern double fz_my_normalized_rank;
extern double fz_normalized_rank_diff_between_first_and_second_place;

extern double fz_most_human_city_delta;

extern double fz_my_percent_pollution;
extern double fz_turns_to_disaster;

extern double fz_total_pop_size;
extern double fz_public_works;
extern double fz_current_military_readiness;

extern double fz_he_beat_me_decay;

extern double fz_highest_enemy_strength_ratio;
extern double fz_highest_human_enemy_strength_ratio;
extern double fz_total_enemy_strength_ratio;
extern double fz_allies_vs_enemies_strength_ratio;

extern double fz_most_human_adv_known_delta;
extern double fz_ratio_advance_cost_me_to_best_human;
extern double fz_ratio_advance_cost_me_to_best_other;

extern double fz_desired_farm_pw; 
extern double fz_desired_road_pw; 
extern double fz_desired_mine_pw; 
extern double fz_desired_transform_pw; 
extern double fz_desired_inst_pw; 

extern double fz_can_space_settle; 
extern double fz_can_sea_settle;

extern double fz_i_can_build_wormhole_probe;

#include "Foreigner.h"

void FSFuzzyBeginTurnPre::Serialize(CivArchive &archive)
{
	return; 
} 


#define REG(v) if(ai->m_fliEngine->GetSymbol(v)) {ai->m_fliEngine->GetSymbol(v)->RegisterInit(FLI_SECT_BEGINTURN);} else { Assert(ai->m_fliEngine->GetSymbol(v)); }



BOOL FSFuzzyBeginTurnPre::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    
    BSetID id; 
    char str[50]="null";
	char text[100];
    ArmyAgent *aa=NULL; 
    for (aa = ai->m_army_set->First(id); ai->m_army_set->Last(); aa = ai->m_army_set->Next(id)) { 
		if (aa->the_squad != NULL)
		{
			sprintf(text, "NULL S:%d", aa->the_squad->squad_ID);
			aa->DumpStr(ai, text);
		}
		else
	        aa->DumpStr(ai, str); 
    }

    fz_desired_farm_pw = ai->m_desired_farm_pw; 
    fz_desired_road_pw = ai->m_desired_road_pw; 
    fz_desired_mine_pw = ai->m_desired_mine_pw; 
    fz_desired_transform_pw = ai->m_desired_transform_pw; 
    fz_desired_inst_pw = ai->m_desired_inst_pw; 


	fz_can_space_settle = ai->m_science_agent->HasSpaceSettle() ;
	fz_can_sea_settle = ai->m_science_agent->HasWaterSettle() ;

    fz_i_can_build_wormhole_probe = double(
        ai->m_wonderDB->IsWormholeDetectorBuilt() &&
        ai->m_science_agent->CanBuildWormholeProbe());
	
	
	
	
	

	
	
	if (ai->my_player_index == 0)
	{ 
		if(fz_count < 0)
		{
			fz_count = 20;
		}
		fz_count = fz_count -1;	

	}

	

    
    fz_time = ai->m_round_count->GetRound(); 

	fz_diff_level = ai->m_gs->GetDifficulty();

	fz_num_cities = ai->m_city_set->GetNum();
	int num_cities = ai->m_world->GetNumCities(ai->my_player_index);

	int num_armies = ai->m_world->GetNumArmies(ai->my_player_index);
	
	int cities = (int) FZ_ROUND(fz_num_cities);
	if ( cities < 1 ) cities = 1;
	fz_unit_to_city_ratio =  num_armies / cities;
	
	int current_gov = ai->m_player->GetGovernmentType(); 
	fz_max_cities_before_penalty = 
		ai->m_governmentDB->GetTooManyCitiesThreshold(current_gov);

	fz_city_delta  = fz_max_cities_before_penalty - fz_num_cities;

    ai->m_alloc_WGF->GetLevel(
        fz_wgf_prod_setting, 
        fz_wgf_gold_setting, 
        fz_wgf_food_setting);

	
	fz_public_works = ai->m_player->GetMaterialsStored ();
	fz_current_military_readiness = ai->m_player->GetReadinessLevel();
    
	ai->m_gold->SetGoldLevels(ai);
    ai->m_gold->GetExpenses(
        fz_income_expense_cleric,
        fz_income_expense_crime,
        fz_income_expense_maintenance,  
        fz_income_expense_wages,
        fz_income_expense_science, 
        fz_income_expense_savings, 
        fz_save_expense_overtime,
        fz_save_expense_diplomacy 
    );
	
	
	fz_my_current_savings = ai->m_gold->GetCurrentSavings();

	fz_gold_and_wage_percent_cost = fz_income_expense_wages + fz_income_expense_maintenance;
		
    double g = ai->m_gold->GetOldScience();
    if (g < 1.0) { 
        fz_sci_turns_to_current_advance = 1000000.0; 
    } else { 
        fz_sci_turns_to_current_advance =  
            double(ai->m_science->GetCurrentScienceCost()) / g; 
    }
	
	
	double time = fz_time;

	if(time == 0) { time = 1;	}
	
	
	sint32 knownAdvances = max(ai->m_science_agent->HowManyIKnow() - 3, 0);

	fz_sci_ave_turns_to_advance = time/knownAdvances;

    MapPointData *size; 
    size = ai->m_map->GetSize(); 
    fz_size_of_world = size->x * size->y; 

    double raw_wages;
    ai->m_alloc_WGF->ContentWages(ai, raw_wages, fz_percent_wages_needed_content);

    
    sint32 total_readiness_cost; 
    ai->m_player->GetReadinessCost(&total_readiness_cost, &fz_percent_cost_readiness);

	fz_distance_to_nearest_human_capitol = 2000;
	
	int i;

	fz_i_agreed_to_stop_polluting = 0.0;
	fz_i_want_to_stop_others_who_pollute = 0.0;
	PLAYER_INDEX his_index;
	
	for (i=1; i<k_MAX_PLAYERS; i++) 
	{ 
		if (ai->m_foreigner[i]) 
		{ 
			double d = ai->m_foreigner[i]->GetDistanceToCapitol(); 
			if(d==0)
			{
				continue;
			}
			
			double his_player_type = ai->m_foreigner[i]->GetPlayerType(ai, ai->m_foreigner[i]);
			if(his_player_type != 1)
			{
				if(fz_distance_to_nearest_human_capitol > d)
				{
					fz_distance_to_nearest_human_capitol = d;
				}
			}

			his_index = ai->m_foreigner[i]->GetPlayerIndex();
			if (ai->m_player->HaveActiveAgreement(his_index, AGREEMENT_TYPE_REDUCE_POLLUTION))
			{
				fz_i_agreed_to_stop_polluting = 1.0;
			}
			if (ai->m_player->HaveActiveAgreement(his_index, AGREEMENT_TYPE_PACT_END_POLLUTION)) 
			{
				fz_i_agreed_to_stop_polluting = 1.0;
				fz_i_want_to_stop_others_who_pollute = 1.0;
			}
		}
	}
   

    
    fz_total_pop_size = 0.0;
    CityAgent *ca=NULL; 
    BOOL is_unknown_id; 
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 
         fz_total_pop_size += ai->m_population->GetCityPopCount(id.GetVal(), &is_unknown_id);
         Assert(FALSE == is_unknown_id); 
    } 

	
    ComputeLandContinentFull(ai, fz_land_continents_full, TRUE); 
	
    ComputeLandContinentFull(ai, fz_land_continents_full_of_friends, FALSE); 
	
    return TRUE; 
}

void FSFuzzyBeginTurnDiplomacy::Serialize(CivArchive &archive)
{
    return;
}

BOOL FSFuzzyBeginTurnDiplomacy::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    
    UpdateGeneralDiplomacy(ai);
    return TRUE; 
}



void FSFuzzyBeginTurnRules::Serialize(CivArchive &archive)
{
    if (archive.IsStoring()) { 
        archive.PutSINT32(m_registered_variables);
    } else  { 
        m_registered_variables = archive.GetSINT32();
        m_registered_variables = FALSE; 
    }
}

BOOL FSFuzzyBeginTurnRules::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
  	if(!m_registered_variables) {
		m_registered_variables = TRUE;

		REG("count");
		REG("time");
		REG("diff_level");

		REG("can_space_settle");
		REG("can_sea_settle");

			
		REG("num_cities");
		REG("most_human_city_delta");

		REG("wgf_prod_setting");
		REG("wgf_gold_setting");
		REG("wgf_food_setting");

        REG("income_expense_cleric");
        REG("income_expense_crime");
        REG("income_expense_maintenance");
        REG("income_expense_wages");
        REG("income_expense_science");
        REG("income_expense_savings");

        REG("gold_and_wage_percent_cost");
		REG("my_current_savings");

		REG("percent_wages_needed_content");

        REG("save_expense_overtime");
        REG("save_expense_diplomacy");
		REG("sci_ave_turns_to_advance");
		REG("size_of_world");

		REG("percent_wages_needed_content");
		REG("percent_cost_readiness");
		REG("land_continents_full");
		REG("land_continents_full_of_friends");
		REG("total_pop_size");
		REG("size_of_world");
		REG("settle_aip");
		REG("num_players_in_game");
		REG("at_war_count");
		REG("power_ratio");
		REG("total_military_incursions");
		REG("his_normalized_rank");
		REG("my_normalized_rank");
		REG("normalized_rank_diff_between_first_and_second_place");

		REG("longest_war");
		REG("shortest_peace");
		REG("distance_to_nearest_human_capitol");

		REG("city_delta");
		REG("sci_turns_to_current_advance");
		REG("total_military_incursions");

		REG("public_works");
		REG("lowest_unit_regard");
		REG("current_military_readiness");

		REG("highest_enemy_strength_ratio");
		REG("highest_human_enemy_strength_ratio");
		REG("total_enemy_strength_ratio");
		REG("allies_vs_enemies_strength_ratio");

		REG("most_human_adv_known_delta");
		REG("ratio_advance_cost_me_to_best_human");
		REG("ratio_advance_cost_me_to_best_other");

		REG("desired_farm_pw");
		REG("desired_road_pw");
		REG("desired_mine_pw");
		REG("desired_transform_pw"); 
		REG("desired_inst_pw"); 

		REG("i_agreed_to_stop_polluting");
		REG("i_want_to_stop_others_who_pollute");

		REG("someone_built_space");
		REG("someone_built_wormhole");
		REG("someone_built_undersea");

		REG("i_can_build_wormhole_probe");

		REG("unit_to_city_ratio");
		REG("unit_to_city_best_human_ratio");

	}


    ai->m_fliEngine->FireRules(FLI_SECT_BEGINTURN);
    return TRUE; 
}



void FSFuzzyBeginTurnPost::Serialize(CivArchive &archive)
{
    return;
}

BOOL FSFuzzyBeginTurnPost::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    
    ai->m_gold->CalcBudget(); 

    
    sint32 rl;
    if (fz_desired_military_readiness < 0.5) { 
        rl = 0; 
    } else if (fz_desired_military_readiness < 1.5) { 
        rl = 1; 
    } else { 
        rl = 2; 
    } 

    
    ai->m_player->SetReadinessLevel(rl); 

#ifdef DO_FZLOG
    
	ai->m_fliEngine->DumpFZInputs(ai->m_my_player_id);
#endif

    sint32 player_idx; 
    for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) { 
        if (ai->m_foreigner[player_idx]) { 
            ai->m_foreigner[player_idx]->EndTurnClear_HeBeatMeThisTurn();
        }
    }
    return TRUE;
}










 
void FSFuzzyBeginTurnPre::ComputeLandContinentFull
(
	AiMain *ai, 
	double &continents_full, 
	bool include_enemies
)
{
    sint32 cont_num = ai->m_continents->GetNumLandCont(); 
    sint32 cont_idx; 
    double *city_count = new double[cont_num]; 

    for (cont_idx=0; cont_idx<cont_num; cont_idx++) { 
        city_count[cont_idx] = 0.0; 
    } 

    BSetID id; 
    CityAgent *ca; 
    sint32 city_cont; 
    BOOL city_is_land; 

    
    MapPointData pos; 

    double total = 0.001; 
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) { 
       
        ca->GetPos(pos); 

        if (SPACE_Z == pos.z) continue; 

        if (!ai->m_continents->GetContinent(ai, pos, city_cont, city_is_land)) { 
            INSANE(CAN_NOT_FIND_CONT); 
        } 

        if (city_is_land) { 
            city_count[city_cont] += 1.0; 
            total += 1.0; 
        }
    } 

   
   ArmyAgent *aa=NULL;
   for (aa = ai->m_army_set->First(id); ai->m_army_set->Last(); aa = ai->m_army_set->Next(id)) { 
       if (aa->GetArmyContainsSettler()) { 

           
           switch(aa->GetState()) { 
           case AGENT_STATE_LOOKING_FOR_WORK:
           case AGENT_STATE_UNBUILT:
           case AGENT_STATE_BUILDING:
               break; 
           default:
               continue; 
           }

           aa->GetPos(ai, pos); 


           if (SPACE_Z == pos.z) continue; 

            if (!ai->m_continents->GetContinent(ai, pos, city_cont, city_is_land)) { 
                INSANE(CAN_NOT_FIND_CONT); 
            } 

            if (city_is_land) { 
                city_count[city_cont] += 1.0; 
                total += 1.0; 
            }
       }
   }

   
   double *foreign_city_count = new double[cont_num]; 
 
   for (cont_idx=0; cont_idx<cont_num; cont_idx++) { 
       foreign_city_count[cont_idx] = 0.0; 
   } 

   BSet<ForeignCity> *known_cities=NULL;
   ForeignCity *fc=NULL; 
	
   sint32 i; 
   for (i=0; i<k_MAX_PLAYERS; i++) { 
        if (ai->m_foreigner[i]) 
		{ 
			bool include_me = FALSE;
			
			if(!ai->m_foreigner[i]->IsAtColdWar())
			{
				include_me = TRUE;
			} 
			if(include_enemies == TRUE)
			{
				include_me = TRUE;
			}
			
			if(include_me == TRUE)
			{
				known_cities = ai->m_foreigner[i]->GetKnownCities(); 

		        if (known_cities) 
				{ 
					for (fc = known_cities->First(id); known_cities->Last(); 
						fc = known_cities->Next(id)) 
					{ 

						fc->GetPos(pos); 
						if (SPACE_Z == pos.z) continue; 
						if (!ai->m_continents->GetContinent(ai, pos, city_cont, 
							city_is_land)) 
						{ 
						    INSANE(CAN_NOT_FIND_CONT); 
						} 

						if (city_is_land) 
						{ 
						    foreign_city_count[city_cont] += 1.0; 
						} 
					} 
				} 
			} 
		} 
    } 


   continents_full = 0.0; 

   double current_cities; 
   double size;
   double available_space;
   double percent_full; 
   for (cont_idx=0; cont_idx<cont_num; cont_idx++) { 
       if (0.0 < city_count[cont_idx]) { 

           
            current_cities = city_count[cont_idx] + foreign_city_count[cont_idx]; 

            
            size = ai->m_world->GetLandContinentSize(cont_idx); 

            
            available_space = size/14.0; 

            
            percent_full = current_cities / available_space; 

            if (1.0 < percent_full) { 
                percent_full = 1.0; 
            } 

            
            continents_full += percent_full * city_count[cont_idx] / total; 
       }
   } 

   
   if (1.0 < continents_full) { 
       continents_full = 1.0; 
   }

   delete[] foreign_city_count;
   foreign_city_count = NULL; 
   delete[] city_count;
   city_count = NULL; 


}


void FSFuzzyBeginTurnDiplomacy::UpdateGeneralDiplomacy(AiMain *ai)
{ 
    double d; 
    
	
    ForeignAgent *him; 
    BSetID h_id;
    BSet<ForeignAgent> *vu; 

	sint32 i;
	sint32 oldbest = -1;

	fz_longest_war = 0.0;
	fz_shortest_peace = 10000.0;

    ai->m_total_production = ai->m_player->GetTotalProduction(ai->m_my_player_id); 
    ai->m_total_unit_cost = ai->m_player->GetTotalUnitCost(ai->m_my_player_id); 

    ai->m_my_raw_strength = k_UNIT_STRENGTH_COEF * ai->m_total_unit_cost + 
       k_PRODUCTION_STRENGTH_COEF * ai->m_total_production; 

	fz_at_war_count = 0.0;
    fz_min_distance_to_capitol = 2000;
    fz_num_players_in_game = 1.0;  
	fz_total_military_incursions = 0;    
	fz_distance_to_nearest_human_capitol = 2000;

	fz_turns_to_disaster = ai->m_gs->GetRoundsToNextDisaster();

	
	fz_most_human_adv_known_delta = 0;
	sint32 most_human_adv_known = 0;
	sint32 best_other_total_costs = 0;
	sint32 best_human_total_costs = 0;
	sint32 my_total_costs = ai->m_science->GetCostOfWhatHeKnows(ai->my_player_index);
	sint32 my_known_adv = ai->m_science_agent->HowManyIKnow();
	
	sint32 his_num_cities = 0;
	sint32 most_human_cities = 0;

	double test_underwater = 0;
	double test_space = 0;
	double test_wormhole = 0;

	BOOL human_found = FALSE;

	PLAYER_INDEX my_index;
	my_index = ai->my_player_index;
	for (i=1; i<k_MAX_PLAYERS; i++) 
	{ 
        if (ai->m_foreigner[i]) 
		{ 
			
			ai->m_foreigner[i]->SetClosestCapitol(FALSE);

			
			fz_num_players_in_game += 1.0;

			ai->m_foreigner[i]->GetUnitAge(test_underwater, test_space, test_wormhole); 
			if(test_wormhole > 0.9) fz_someone_built_wormhole = 1.0; 
			if(test_space > 0.9) fz_someone_built_space = 1.0;
			if(test_underwater > 0.9) fz_someone_built_undersea = 1.0;

			
			
			
			
			

			
			
			
            ai->m_foreigner[i]->UpdateDiplomaticState(ai); 
			
			
			
			
			fz_total_military_incursions = 0;
			vu = ai->m_foreigner[i]->GetVisibleUnits();

			#ifdef _DEBUG
				sint32 finite_loop=0; 
			#endif

			for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id)) 
			{ 
				Assert(finite_loop++ < 1000); 

				MapPointData pos;
				him->GetPos(pos);
			
				if(ai->m_world->GetCellOwner(&pos) == ai->my_player_index) 
				{
					fz_total_military_incursions += 1;
				}
			} 

			double length_of_war_with_him = ai->m_foreigner[i]->GetLengthOfWarWithHim(ai);
			
			if(fz_longest_war < length_of_war_with_him)
			{
				fz_longest_war = length_of_war_with_him;
			}
			double length_of_peace_with_him = ai->m_foreigner[i]->GetLengthOfPeaceWithHim(ai);
			if(fz_shortest_peace > length_of_peace_with_him)
			{	
				fz_shortest_peace = length_of_peace_with_him;
			}
			
			
			if (ai->m_foreigner[i]->UpdateWar(ai, ai->my_player_index)) 
			{ 
                fz_at_war_count += 1.0; 
            }

            d = ai->m_foreigner[i]->GetDistanceToCapitol(); 
            if (d < fz_min_distance_to_capitol) 
			{ 
                fz_min_distance_to_capitol = d; 
				if(oldbest != -1)
				{
					ai->m_foreigner[oldbest]->SetClosestCapitol(FALSE);
				}
				ai->m_foreigner[i]->SetClosestCapitol(TRUE);
				oldbest = i;
            } 
			double his_player_type = ai->m_foreigner[i]->GetPlayerType(ai, ai->m_foreigner[i]);
			if(his_player_type != 1)
			{
				if(d==0)
				{
					continue;
				}
				
				if(fz_distance_to_nearest_human_capitol > d)
				{
					fz_distance_to_nearest_human_capitol = d;
				}
			}
			
			if(his_player_type !=1)
			{
				int his_adv_known =  ai->m_science->HowManyDoesHeKnow(ai->m_foreigner[i]->GetPlayerIndex());
				if (his_adv_known > most_human_adv_known)
					most_human_adv_known = his_adv_known;
				human_found = TRUE;
			}

			
			
			
			sint32 cost_adv_he_knows = ai->m_science->GetCostOfWhatHeKnows(ai->m_foreigner[i]->GetPlayerIndex());
			if(cost_adv_he_knows > best_other_total_costs)

			{
				best_other_total_costs = cost_adv_he_knows;
			}
		 
			if(his_player_type != 1)
			{
				if(cost_adv_he_knows > best_human_total_costs)
				{
					best_human_total_costs = cost_adv_he_knows;
				}

				his_num_cities = ai->m_world->GetNumCities(ai->m_foreigner[i]->GetPlayerIndex());
				if(his_num_cities > most_human_cities) 
					most_human_cities = his_num_cities;
				human_found = TRUE;
			}

		} 
		

		if (human_found)
		{
			fz_most_human_city_delta = fz_num_cities - most_human_cities;
			fz_most_human_adv_known_delta = most_human_adv_known - my_known_adv;
			
			
			
			
			fz_ratio_advance_cost_me_to_best_human = (double) my_total_costs / (double) best_human_total_costs;
			fz_ratio_advance_cost_me_to_best_other = (double) my_total_costs / (double) best_other_total_costs;
		}
		else
		{
			
			fz_most_human_city_delta = 0;
			fz_most_human_adv_known_delta = 0;
			fz_ratio_advance_cost_me_to_best_human = 1.0;
			fz_ratio_advance_cost_me_to_best_other = 1.0;
		}

    } 

    ComputeNormalizedStr(ai);
	ComputeRelativeStrongestEnemyStr(ai);

}

void FSFuzzyBeginTurnDiplomacy::ComputeRelativeStrongestEnemyStr(AiMain *ai)
{ 
    double my_str = ai->m_my_raw_strength; 
	double his_str = 1;
	double highest_enemy_strength = 1;
	double highest_human_enemy_strength = 1;
	double total_enemy_strength = 1;
	double allied_strength = 1;
	double his_regard = 50.0;
	double his_unit_regard = 100.0;

    sint32 i, player_type;
    
	fz_lowest_unit_regard = 100;

	fz_unit_to_city_best_human_ratio = 0;
	double his_unit_to_city_ratio = 0;
    for (i=1; i<k_MAX_PLAYERS; i++) 
	{ 
       if (ai->m_foreigner[i]) 
	   { 
			
			his_regard = ai->m_foreigner[i]->GetDiplomaticRegard();
			player_type = ai->m_foreigner[i]->GetPlayerType(ai, ai->m_foreigner[i]);

			
			
			double his_cities = ai->m_world->GetNumCities(ai->m_foreigner[i]->GetPlayerIndex());
			if ( his_cities == 0) his_cities = 1.0;

			double his_units = ai->m_world->GetNumArmies(ai->m_foreigner[i]->GetPlayerIndex());
			his_unit_to_city_ratio = his_units / his_cities;

			
			if(player_type == 0 || player_type == 2)
			{
				
				
				if ( his_unit_to_city_ratio > fz_unit_to_city_best_human_ratio)
				{
					fz_unit_to_city_best_human_ratio = his_unit_to_city_ratio;
				}
			}

			
			his_str = ai->m_foreigner[i]->GetRawStrength(); 
			
			
			
			
			his_unit_regard = ai->m_foreigner[i]->GetUnitRegard();
			if( his_unit_regard < fz_lowest_unit_regard) 
				fz_lowest_unit_regard = his_regard;

			if(!ai->m_foreigner[i]->IsAtHotWar())
			{
				
				if(ai->m_foreigner[i]->UpdateAlliance(ai, ai->my_player_index))
				{	
					allied_strength = allied_strength + his_str;
					his_str = 1.0;
				}				
				
				
				
				BOOL at_cold_war = ai->m_foreigner[i]->IsAtColdWar();
				if(at_cold_war)
				{
					his_str = his_str * (200 - his_regard)/200;
				}

				if(!at_cold_war)
				{
					his_str = his_str * 0.5;
				}
				
			}
	
			
			
			if(his_str > highest_enemy_strength)
				highest_enemy_strength = his_str; 

			
			if(player_type == 0 || player_type == 2)
			{
				
				if(his_str > highest_human_enemy_strength)
					highest_human_enemy_strength = his_str;	
			}
	
			
			total_enemy_strength = total_enemy_strength+ his_str;
		}	

	} 
	
	
	
	fz_highest_enemy_strength_ratio = my_str / highest_enemy_strength;
	if (highest_human_enemy_strength > 1)
	{
		fz_highest_human_enemy_strength_ratio = my_str / highest_human_enemy_strength;
		fz_total_enemy_strength_ratio = my_str / highest_human_enemy_strength;
	}
	else
	{
		
		fz_highest_human_enemy_strength_ratio = 1;
		fz_total_enemy_strength_ratio = 1;
	}

 fz_allies_vs_enemies_strength_ratio = (my_str + allied_strength) / total_enemy_strength;


}
			
void FSFuzzyBeginTurnDiplomacy::ComputeNormalizedStr(AiMain *ai)
{ 
     double str_table[k_MAX_PLAYERS]; 
     sint32 fcount=1; 
     double str; 
     double min_str = ai->m_my_raw_strength; 
     double max_str = ai->m_my_raw_strength; 
	 double top = 0.001, second = 0.001;

     sint32 i,j;
     

     str_table[0] = 10000000.0; 
	 
     for (i=1; i<k_MAX_PLAYERS; i++) { 
        if (ai->m_foreigner[i]) { 
            str = ai->m_foreigner[i]->GetRawStrength(); 

            if (str<min_str) { 
                min_str =str; 
            } 

            if (max_str < str) { 
                max_str = str; 
            } 

            str_table[i] = str; 
            fcount++; 
        } else { 
            str_table[i] = 10000000.0; 
        }
     }
     str_table[ai->m_my_player_id] = ai->m_my_raw_strength; 

     
     double tmp;

	 
     for (i=1; i<(k_MAX_PLAYERS-1); i++) { 
         for (j=i+1; j<k_MAX_PLAYERS; j++) { 
             if (str_table[i] > str_table[j]) { 
                 tmp = str_table[i]; 
                 str_table[i] = str_table[j];
                 str_table[j] = tmp; 
             } 
         } 
     } 

	
	if(str_table[fcount - 1] > ai->m_my_raw_strength)
	{
		top = str_table[fcount - 1];
		if(str_table[fcount - 1] > ai->m_my_raw_strength)
		{
			second = str_table[fcount - 2];
		}else
		{
			second = ai->m_my_raw_strength;
		}
	}else
	{
		top = ai->m_my_raw_strength;
		second = str_table[fcount - 1];
	}

	
	if(top <0.001) { 
		fz_normalized_rank_diff_between_first_and_second_place = 0.0;
	}else
	{
		fz_normalized_rank_diff_between_first_and_second_place = 
			second/top;
	}

     
    double median = str_table[fcount/2]; 

     if (ai->m_my_raw_strength < median) { 
         fz_my_normalized_rank = 0.5 * (ai->m_my_raw_strength - min_str)/(median-min_str); 
     } else if (median < ai->m_my_raw_strength) { 
         fz_my_normalized_rank = 0.5 + 0.5 * (ai->m_my_raw_strength - median) / (max_str - median); 
     } else { 
         fz_my_normalized_rank = 0.5; 
     } 

     
     if (ai->my_player_index != 0){
        ai->m_foreigner[0]->SetHisRank(1.0); 
     }

	 
	 
	double his_rank;

	for (i=1; i<k_MAX_PLAYERS; i++) { 
		if (ai->m_foreigner[i]) { 
            str = ai->m_foreigner[i]->GetRawStrength(); 

            if (str < median) { 
                his_rank = 0.5 * (str - min_str)/(median-min_str); 
            } else if (median < str) { 
                his_rank = 0.5 + 0.5 * (str - median) / (max_str - median); 
            } else { 
                his_rank = 0.5; 
            } 

            ai->m_foreigner[i]->SetHisRank(his_rank); 
	
		}
	}		

}

void FSFuzzyDiplomacy::Serialize(CivArchive &archive)
{
	return; 
} 


BOOL FSFuzzyDiplomacy::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{

    return TRUE;
}




void FSFuzzyEndTurn::Serialize(CivArchive &archive)
{
	return; 
} 

BOOL FSFuzzyEndTurn::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{

    ai->m_fliEngine->FireRules(FLI_SECT_ENDTURN);

    ai->m_stats->AddTurn(ai); 

 
  


    return TRUE;
}


