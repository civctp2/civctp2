

#include "c3.h"
#include "IC3BlgDB.h"

#include "AiBldQue.h"

#include "CivArchive.h"


#include "IC3GameState.h"
#include "AiMain.h"


#include "BldAgent.h"
#include "CityAgent.h"

void BuildingAgentFlat::Serialize (CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive.Store((uint8*) this, sizeof(*this)); 
    } else { 
        archive.Load((uint8*) this, sizeof(*this)); 
    } 
}

BuildingAgent::BuildingAgent()
{  
    m_state = BUILDING_STATE_UNBUILT;
} 

void BuildingAgent::SetType (sint16 t)
{
    m_building_type = t; 
} 

void BuildingAgent::Serialize (CivArchive &archive)
{
    CHECKSERIALIZE

    BuildingAgentFlat::Serialize(archive); 
}

BOOL BuildingAgent::CanConstructBuilding(AiMain *ai)

{
    if (m_state == BUILDING_STATE_UNBUILT) { 
        return ai->m_buildingDB->CityCanConstructBuilding(m_building_type); 
    } else { 
        return FALSE; 
    }
}

void BuildingAgent::SetState (const BUILDING_STATE s)
{
    m_state = s; 
} 

double BuildingAgent::CalcPayoff(AiMain *ai, CityAgent *ca)
{ 
    if (ai->m_buildingDB->GetIsAirport(m_building_type)) {
        return 101.0;
    } else { 

        if (m_state != BUILDING_STATE_UNBUILT) { 
            return 0.0;
        } else { 
            return 1.0 * (100 - m_building_type); 
        }
    }
        






















































} 

void BuildingAgent::RegisterCreateBuilding()

{
   

    m_state = BUILDING_STATE_DONE;
}

void BuildingAgent::RegisterLostBuilding()
{
    Assert (m_state == BUILDING_STATE_DONE);

    m_state = BUILDING_STATE_UNBUILT;
}


