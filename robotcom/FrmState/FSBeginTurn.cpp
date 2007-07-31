












#include "c3.h"
#include "globals.h"
#include "IMapPointData.h"
#include "IC3Population.h"


#include "FSBeginTurn.h"
#include "AiCell.h"
#include "AiMap.h"
#include "IC3GameState.h"
#include "AiMain.h"
#include "foreigner.h"
#include "Gold.h"


#include "BSet.h"
#include "ArmyAgent.h"
#include "CityAgent.h"


#include "DynArr.h"
#include "CivArchive.h"


#include "CityGrowth.h"


#include "FzOut.h"
#include "WotP.h"
#include "Cont.h"

#include "WallClock.h"
extern Wall_Clock *g_wall_clock;

#include "ScienceAgent.h"
#include "AllocWGF.h"
#include "IC3Wonder.h"


extern double fz_pop_food_max;  
extern double fz_pop_food_min;  

extern double fz_pop_production_max; 
extern double fz_pop_production_min; 

extern double fz_pop_gold; 
extern double fz_pop_science; 
extern double fz_pop_happiness; 




BOOL FSBeginTurn::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{

    CityAgent *agent; 
    BSetID id; 
    ArmyAgent *aa; 

    ai->m_wonders_available = ai->m_wonderDB->WondersAvailable(ai->m_my_player_id);

    ai->m_alloc_WGF->InitSystemVariables(ai);

#ifdef VALIDATE_ON
    ai->m_map->Validate(); 
#endif

    for (agent = ai->m_city_set->First(id); ai->m_city_set->Last(); agent = ai->m_city_set->Next(id)) { 
        agent->BeginTurn(ai); 
    }

    for (aa = ai->m_army_set->First(id); ai->m_army_set->Last(); aa = ai->m_army_set->Next(id)) { 
        aa->BeginTurn(ai); 
    }

    sint32 i; 
    for (i=0; i<k_MAX_PLAYERS; i++) { 
        if (ai->m_foreigner[i]) { 
            ai->m_foreigner[i]->BeginTurn(ai); 
        }
    } 

    ai->m_gold->StoreProjectedIncome(ai); 


    return TRUE; 
}

void FSBeginTurn::Serialize(CivArchive &archive) 
{

    CHECKSERIALIZE

    return; 
} 

FSAllocatePopulation::FSAllocatePopulation()
{

}

FSAllocatePopulation::~FSAllocatePopulation()
{

}

BOOL FSAllocatePopulation::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{

    BSetID id;
    CityAgent *ca=NULL; 
 
    sint32 idx_city; 

    if (ai->m_science_agent->IknowItAll()) { 
        fz_pop_science = 0.0;               
    } 


    sint32 nCities = ai->m_city_growth->GetNumCities(); 
    if (1 == nCities) { 
       ca = ai->m_city_growth->GetCity(0)->GetAgent(); 
      
       if (ca->GetNumCitizens() < 3) { 

            if (ca->PopNeedsChanging(ai, fz_pop_food_max, fz_pop_production_min, fz_pop_gold, 0 )) { 
               ca->PlaceAllPop(ai, fz_pop_food_max, fz_pop_production_min, 
                   fz_pop_gold, fz_pop_science, fz_pop_happiness, 0, 1) ;
            }

       } else { 
           if (ca->PopNeedsChanging(ai, fz_pop_food_min, fz_pop_production_max, fz_pop_gold, 0 )) { 
               ca->PlaceAllPop(ai, fz_pop_food_min, fz_pop_production_max,  
                    fz_pop_gold, fz_pop_science, fz_pop_happiness, 0, 1 ) ;
           }
       }

    } else if (1 < nCities) {        
        sint32 median = ai->m_city_growth->GetMedian(); 

        
		
        for (idx_city=0; idx_city<=median; idx_city++) {
            ca = ai->m_city_growth->GetCity(idx_city)->GetAgent(); 
            if (ca->PopNeedsChanging(ai, fz_pop_food_max, fz_pop_production_min, fz_pop_gold, idx_city)) { 
                ca->PlaceAllPop(ai, fz_pop_food_max, fz_pop_production_min, 
                    fz_pop_gold, fz_pop_science, fz_pop_happiness, idx_city, nCities);
            }
        }

        
      
		for (idx_city=median+1; idx_city<nCities; idx_city++) {
            ca = ai->m_city_growth->GetCity(idx_city)->GetAgent(); 

            if (ca->PopNeedsChanging(ai, fz_pop_food_min, fz_pop_production_max, fz_pop_gold, idx_city)) { 

                
                
				
                 if (ca->GetNumCitizens() < 3) { 
                     ca->PlaceAllPop(ai, fz_pop_food_max, fz_pop_production_min, 
                        fz_pop_gold, fz_pop_science, fz_pop_happiness, idx_city, nCities) ;
                 } else { 
                     ca->PlaceAllPop(ai, fz_pop_food_min, fz_pop_production_max, 
                        fz_pop_gold, fz_pop_science, fz_pop_happiness, idx_city, nCities) ;
                 }
            }
        }
    }


    return TRUE; 
}

void FSAllocatePopulation::Serialize(CivArchive &archive) 
{

    CHECKSERIALIZE

    return; 
} 








































































































































FSCleanup::FSCleanup()
{
   
    m_dead_agents = new DynamicArray<ArmyAgent*>(8);
}

FSCleanup::~FSCleanup()
{
    if (m_dead_agents) { 
		delete m_dead_agents;
        m_dead_agents = NULL;
    } 
}

void FSCleanup::Serialize(CivArchive &archive)
{ 
    CHECKSERIALIZE

    return; 
} 

BOOL FSCleanup::Execute(AiMain *ai, sint32 &branch,  FILE *fout)

{
    ArmyAgent *agent=NULL; 
    BSetID id; 

    m_dead_agents->Clear(); 

    for (agent = ai->m_army_set->First(id); ai->m_army_set->Last(); agent = ai->m_army_set->Next(id)) { 
        if (agent->GetState() == AGENT_STATE_MARKED_DEAD) { 
            m_dead_agents->InsertFlat(agent); 
        }
    }

    sint32 i, n; 
    n = m_dead_agents->Num(); 
    for (i=0; i<n; i++) { 
        ai->m_army_set->Del(m_dead_agents->Access(i)->GetID()); 
    } 

    return TRUE; 
}


void FSUpdateContinent::Serialize(class CivArchive &archive) 

{ 
    CHECKSERIALIZE

    return; 
} 

BOOL FSUpdateContinent::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    ai->m_continents->Update(ai); 


    return TRUE; 
}



BOOL FSWillOfThePeople::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{
    ai->m_will_of_the_people->PollNewGov(ai); 


    return TRUE; 
}

void FSWillOfThePeople::Serialize(CivArchive &archive)
{ 
    CHECKSERIALIZE
} 

