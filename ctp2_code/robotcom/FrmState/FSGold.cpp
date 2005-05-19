#include "c3.h"


#include "FSGold.h"
#include "civarchive.h"


#include "ic3GameState.h"
#include "ic3player.h"
#include "ic3Population.h"
#include "ic3TerrDB.h"

#include "aimain.h"
#include "gold.h"


#include "CityAgent.h"
#include "scienceagent.h"


#include "GlOvertime.h"


#include "OTUtilityList.h"

#include "Wallclock.h"
extern Wall_Clock *g_wall_clock;


#include "FzOut.h"

FSAllocScience::FSAllocScience()
{ 
    return; 
} 

void FSAllocScience::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	return; 
} 

BOOL FSAllocScience::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    if (ai->m_science_agent->MustLearnMoreScience()) { 
    	ai->m_science_agent->ResearchScience(ai) ;
    }

    if (ai->m_science_agent->IknowItAll()) {
        ai->m_player->SetScienceTax(0.0); 
    } else { 

        ai->m_player->SetScienceTax(1.0); 
    }


    return TRUE;
}


FSCompeteForGold::FSCompeteForGold ()
{
	return; 
} 

void FSCompeteForGold::Serialize(CivArchive &archive)
{ 
	return; 
} 


BOOL FSCompeteForGold::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    MakeUtilityList(ai); 
    SpendGold(ai); 


    return TRUE; 
}


double FSCompeteForGold::EstimateExpense(AiMain *ai)
{ 

    sint32 p, s; 
    double sum = 0.0; 
    CityAgent *ca = NULL; 
    BSetID id;
    BOOL is_unknown_id; 

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {             
    	p = ai->m_population->GetCityPopCount(ca->GetID().GetVal(), &is_unknown_id);
        s = ai->m_population->GetCitySlaveCount(ca->GetID().GetVal(), &is_unknown_id);
        sum += p - s;
    }
    
    return ai->m_player->GetTotalBuildingUpkeep() + 
        ai->m_player->GetWagesPerPerson() * sum;
    
} 





















void FSCompeteForGold::MakeUtilityList(AiMain *ai)
{
    
    CityAgent *ca=NULL; 
    GoalOvertime *goal_obj=NULL; 
    BSetID id;

    ai->m_overtime_utility_list->Clear(); 
    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {            
        if (PretestOvertime(ai, ca)) { 
            goal_obj = new GoalOvertime(ai, ca); 
            ai->m_overtime_utility_list->Insert(goal_obj); 
        }
    }    
} 

BOOL FSCompeteForGold::PretestOvertime(AiMain *ai, CityAgent *ca)
{ 
    if (ca->GetQueueLen() < 1) { 
        return FALSE; 
    } else { 

        BOOL is_unknown_id = FALSE; 
        uint32 city_id = ca->GetID().GetVal(); 

        double cost =  ai->m_player->CityGetOvertimeCost(&is_unknown_id, city_id); 
        Assert(FALSE == is_unknown_id); 

        if (cost <= 1.0) { 
            return FALSE; 
        } 

        sint32 turns = ai->m_player->CityTurnsToBuild(city_id, &is_unknown_id); 

        if (turns < 3) { 
            return FALSE; 
        }

        return TRUE; 
    } 
}

void FSCompeteForGold::SpendGold(AiMain *ai)
{
    GoalOvertime *max_goal=NULL; 
    double max_utility; 
    double min_cost; 
    double cost=0.0; 
    
    double available = ai->m_gold->GetPercentSaveOvertime() * 
        ai->m_gold->GetCurrentSavings(); 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (0.0 < available) { 
Assert(++finite_loop < 10000); 
        ai->m_overtime_utility_list->GetMax(max_goal, max_utility, min_cost); 
        
        if (max_goal) {
            max_goal->Spend(ai, available, cost); 
        } else { 
            break; 
        }
    } 
}


