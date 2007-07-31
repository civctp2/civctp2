

#include "c3.h"
#include "globals.h"
#include "IC3BlgDB.h"

#include "IC3Population.h"
#include "IC3Player.h"

#include "MapPoint.h"
#include "CivArchive.h"
#include "FlatPtr.h"


#include "IC3GameState.h"
#include "IC3World.h"

#include "AiMain.h"
#include "AiCell.h"
#include "AiMap.h"
#include "AiRndCnt.h"
#include "Gold.h"


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "AiBldQue.h"
#include "BSet.h"
#include "CivArchive.h"
#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"
#include "WonderTbl.h"


#include "FzOut.h"


#include "InstBidList.h"
#include "GoalInst.h"
#include "GoodAgent.h"
#include "BldAgent.h"


#include "ArmyGoal.h"
#include "GlBlg.h"
#include "GlFreight.h"


#include "AllocWGF.h"
#include "CityFP.h"
#include "CityProd.h"



extern double fz_blg_uscale; 
extern double fz_blg_effect_silo;
extern double fz_blg_effect_capitol;
extern double fz_blg_effect_defense_bonus;

extern double fz_blg_effect_defense_bonus_no_city_walls_attacker;
    
extern double fz_blg_effect_happy_increment;
extern double fz_blg_effect_knowledge_increment;
extern double fz_blg_effect_content_aif;

extern double fz_blg_effect_double_televan;
extern double fz_blg_effect_reduce_waste;

extern double fz_blg_effect_production_type_production_increment;
extern double fz_blg_effect_production_type_food_increment;
extern double fz_blg_effect_production_type_gold_increment;
extern double fz_blg_effect_production_type_knowledge_increment;
extern double fz_blg_effect_production_type_entertainment_increment;
    
extern double fz_blg_effect_production_type_production_percent;
extern double fz_blg_effect_production_type_food_percent;
extern double fz_blg_effect_production_type_gold_percent;
extern double fz_blg_effect_production_type_knowledge_percent;
extern double fz_blg_effect_production_type_entertainment_percent;

extern double fz_blg_effect_pollution_bonus;
extern double fz_blg_effect_pollution_cleaner;

extern double fz_blg_effect_affective_pop_dec;
extern double fz_blg_effect_no_unhappy_people;
extern double fz_blg_effect_no_happiness;

extern double fz_blg_effect_lower_crime;
extern double fz_blg_effect_prevent_conver;
extern double fz_blg_effect_prevent_slavery;
extern double fz_blg_effect_lower_overcrowding;
extern double fz_blg_effect_allow_grunts;
extern double fz_blg_effect_lower_peace_move;
extern double fz_blg_effect_gold_per_citizen;
extern double fz_blg_effect_protect_from_nukes;
extern double fz_blg_effect_protect_from_bio;
extern double fz_blg_effect_protect_from_nano;
extern double fz_blg_effect_lower_pop_pollut;
extern double fz_blg_effect_television;
extern double fz_blg_effect_city_wall;
extern double fz_blg_effect_airport;
extern double fz_blg_effect_space_launch;


extern double fz_min_happiness; 
extern double fz_gold_2_production; 
extern double fz_personality_caution; 

double CityAgent::UtilityBlgSilo(AiMain *ai,  double effect[])
{
   double e = effect[BLG_EFFECT_SILO];

   if (e<= 0.0) { 
      return 0.0; 
   } 

   return m_ave_prod_tile * m_total_citizens; 
















}









double CityAgent::UtilityBlgCapitol(AiMain *ai, double effect[])
{
    sint32 city_idx; 

    switch (ai->m_capitol_state) { 
    case CAPITOL_STATE_BUILDING:
    case CAPITOL_STATE_HASIT:
        return 0.0; 
    case CAPITOL_STATE_NONE:
        
        if (0.0 < effect[BLG_EFFECT_CAPITOL]) { 
            Assert(ai->m_capitol_id == NULL); 
            
            if (m_net_production < 1.0) { 
                return 0.0; 
            } 

            if (ai->m_city_prod->GetNumCities() < 0) { 
                return 0.0;
            }

            if (!ai->m_city_prod->IsAboveMedian(m_id)) { 
                return 0.0; 
            } 

            if (!ai->m_city_prod->GetCityIndex(m_id, city_idx)){ 
                return 0.0; 
            }

            return double(city_idx) * 1000000000.0; 
        } else { 
            return 0.0; 
        }
    default:
        INSANE(CAPITOL_STATE_UNKNOWN); 
        return 0.0; 
    }
}









double CityAgent::UtilityBlgHappy(AiMain *ai, 
     
   double effect[] 
   )
{
   double e = effect[BLG_EFFECT_HAPPY_INCREMENT]; 
   if ( e <= 0.0) {
       return 0.0; 
   } 

   sint32 rev_hap = ai->m_gs->GetConstRevolutionLevel();
   sint32 riot_hap = ai->m_gs->GetConstRiotLevel(); 

    
    
   double min_happy = max(fz_min_happiness,  max(rev_hap, riot_hap)); 
   e -= m_old_happiness - min_happy;

   
   if (e <= 0.0) { 
       return 0.0; 
   } 

   double musician_happiness = -1;
   

   Assert(0.0 < musician_happiness); 
   double m; 
   if (musician_happiness <= 0.0) { 
       m = m_num_musicians; 
   } else { 

       
       m = e / musician_happiness; 
   }

   
   double food = m_ave_food_tile * m;    
   double r = ai->m_alloc_WGF->GetCurrentRation(); 
   double new_pop;

  
   if (r <= 0.0000001) { 
       new_pop = 1.0; 
   } else { 
      new_pop = food / r; 
   }

   
   
   
   return m_ave_prod_tile * (m + new_pop); 
}

double CityAgent::UtilityBlgWaste(AiMain *ai, 
     
   double effect[] 
   )
{
    return 0.0; 
    
}

double CityAgent::UtilityBlgCrime(AiMain *ai, 
     
   double effect[] 
   )
{
    return 0.0; 
    
}


double CityAgent::UtilityBlgProductionInc(AiMain *ai, double effect[])
{ 
    
    return effect[BLG_EFFECT_PRODUCTION_TYPE_PRODUCTION_INCREMENT];
}

double CityAgent::UtilityBlgFoodInc(AiMain *ai, double effect[])
{

    double e = effect[BLG_EFFECT_PRODUCTION_TYPE_FOOD_INCREMENT];

    if (e <= 0) { 
        return 0.0; 
    } 

    double current_rations = ai->m_alloc_WGF->GetCurrentRation(); 

    Assert(0.0 < current_rations); 
    if (current_rations <= 0.0) { 
        return 0.0; 
    } 

    
    
    return m_ave_prod_tile * e/current_rations; 
}

double CityAgent::UtilityGoldInc(AiMain *ai, double effect[])
{
    double e = effect[BLG_EFFECT_PRODUCTION_TYPE_GOLD_INCREMENT];

    if (e <= 0.0) { 
        return 0.0; 
    } 

    return e * fz_gold_2_production;
}

double CityAgent::UtilityKnowledgeInc(AiMain *ai, double effect[])
{
    
    double e = effect[BLG_EFFECT_PRODUCTION_TYPE_KNOWLEDGE_INCREMENT];

    if (e <= 0.0) { 
        return 0.0; 
    } 

    if (ai->m_city_fp->IsAboveMedian(m_id)) { 
        return 10 *  e;
    } else { 
        return 0.0; 
    } 
}

double CityAgent::UtilityProdEntertainInc(AiMain *ai, double effect[])
{
    
   double e = effect[BLG_EFFECT_PRODUCTION_TYPE_ENTERTAINMENT_INCREMENT]; 
   Assert(e == 0.0); 
   return e; 
}

double CityAgent::UtilityProdPercent(AiMain *ai, double effect[])
{
    double delta = m_gross_production * effect[BLG_EFFECT_PRODUCTION_TYPE_PRODUCTION_PERCENT];
    return delta; 
}

double CityAgent::UtilityFoodPercent(AiMain *ai, double effect[])
{
    double e = effect[BLG_EFFECT_PRODUCTION_TYPE_FOOD_PERCENT];

    if (e <= 0.0) { 
        return 0.0; 
    } 

    double delta = m_gross_food * e;
    double pr = ai->m_alloc_WGF->GetCurrentRation(); 

    if (pr <= 0.0) { 
        return 0.0; 
    } 

    
    
    return m_ave_prod_tile * delta/pr; 
}

double CityAgent::UtilityGoldPercent(AiMain *ai, double effect[])
{
    double e = effect[BLG_EFFECT_PRODUCTION_TYPE_GOLD_PERCENT];
    if (e <= 0.0) { 
        return 0.0;
    } 
    double delta = m_gross_gold * e;
    return delta * fz_gold_2_production; 
}

double CityAgent::UtilityKnowledgePercent(AiMain *ai, double effect[])
{
    double delta = m_gross_science * effect[BLG_EFFECT_PRODUCTION_TYPE_KNOWLEDGE_PERCENT];
    return 10.0 * delta; 
}

double CityAgent::UtilityEntertainPercent(AiMain *ai, double effect[])
{
    double e = effect[BLG_EFFECT_PRODUCTION_TYPE_ENTERTAINMENT_PERCENT];
    Assert(e == 0.0); 
    return e; 
}

double CityAgent::UtilityBlgDefense(AiMain *ai, double effect[])
{ 
    double e = effect[BLG_EFFECT_DEFENSE_BONUS]; 
    if (e<=0.0) { 
        return 0.0; 
    } 

    sint32 types_in_army[k_MAX_ARMY_SIZE];
    sint32 num_in_army;
    ai->m_world->Get_Army_Types(m_pos, types_in_army, num_in_army);
    if (num_in_army < 1) { 
        return 0.0; 
    } 

    
    double t = m_threat_strength - m_threat_strength * (fz_personality_caution - 0.25); 
    double ds = m_defenders_defense_strength; 

    if ((t > 10.0)  && 
        (t > m_defenders_defense_strength)) { 
    
        double max_u = 2 + 6.0 * (1.0 - fz_personality_caution);
        if ((max_u < m_defenders_nUnits) ||
            (t <= (ds + ds * e))) { 
            return m_net_production; 
        } 
    } 
    return 0.0;
}

double CityAgent::UtilityBlgWall(AiMain *ai, double effect[])
{
    double e = effect[BLG_EFFECT_CITY_WALL]; 

    return 0.0; 
}

double CityAgent::UtilityBlgSpaceLauncher(AiMain*ai, double effect[])
{ 
    if (1 == m_pos->z) { 
        return 0.0; 
    } else { 
        if (0.0 < effect[BLG_EFFECT_SPACE_LAUNCH]) { 
            return fz_blg_effect_space_launch * effect[BLG_EFFECT_SPACE_LAUNCH];
        } else { 
            return 0.0; 
        } 
    }
}

double CityAgent::ActionBidBuild(AiMain *ai,  GoalBuilding *goal_obj)
{
    if (goal_obj->GetHomeId() != m_id.GetVal()) { 
        return BAD_UTILITY; 
    } else { 
        sint32 blg_type = goal_obj->GetBuildingType(); 
        sint32 upkeep = ai->m_buildingDB->GetGoldUpkeep(blg_type); 

		if (!Can_Build_Building(ai,blg_type))
			return BAD_UTILITY;
        
		
        
        
        if (ai->m_gold->GetAllowedExpenseNewBlg() < upkeep) { 
            return BAD_UTILITY; 
        } 

        if (GetProjectedNetProduction() < 1) { 
            return BAD_UTILITY;                  
        }
        double effect[BLG_EFFECT_MAX]; 

        ai->m_buildingDB->GetAllEffects(blg_type, effect); 

        double estimate_one_turn = 
           
            fz_blg_effect_capitol *  UtilityBlgCapitol(ai, effect);
            
























































        double production_cost = ai->m_buildingDB->GetProductionCost(blg_type); 
        double turns_to_complete; 
        if (m_net_production < 5) { 
            turns_to_complete  = production_cost / 5; 
                                                     
                                                    
        } else { 
            turns_to_complete = production_cost / m_net_production; 
        }

        
        
        double u =   WeightTimeToBuild(ai, estimate_one_turn, turns_to_complete); 

		
		if (u <= 0.01) return BAD_UTILITY;

        return u; 
    }
}

void CityAgent::SetHasSpaceLauncher(const BOOL val)
{
    m_has_space_launcher = val; 
}

BOOL CityAgent::HasSpaceLauncher() const
{
    return m_has_space_launcher;
}

BOOL CityAgent::HasAirport() const
{ 
    return m_has_airport; 
} 

BOOL CityAgent::HasGranary() const
{ 
    return m_has_granary; 
} 

void CityAgent::ResetBuildingGoal(const sint32 blg_idx)
{

}
