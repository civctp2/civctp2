


#include "c3.h"
#include "globals.h"
#include "DynArr.h"
#include "IMapPointData.h"
#include "IC3Player.h"
#include "IC3World.h"

#include "CivArchive.h"
#include "FlatPtr.h"
#include "BSet.h"

#include "AIMain.h"
#include "AiMap.h"
#include "GlEnslave.h"


#include "dr_debug.h"
#include "matrix.h"
#include "Grassfire.h"
#include "Strategic_Map.h"

#include "ForeignAgent.h"
#include "ForeignCity.h"
#include "Foreigner.h"

#include "ArmyAgent.h"

ZEROMEM(GoalEnslaveFlat); 
ZEROMEM(GoalEnslavePtr); 
FLATSERIALIZE(GoalEnslaveFlat); 

GoalEnslave::GoalEnslave()
{
    Init(); 
	m_ai = NULL; 
}

GoalEnslave::GoalEnslave(AiMain *init_ai,
                         BSetID &id, 
                         ForeignAgent *him)
{
    Init(); 

    m_his_agent_id = id;
    foreign_player = him->GetPlayerIndex(); 

	MapPointData pos;

	him->GetPos(pos);

	*m_pos = pos;

	
    Assert(m_XYpos); 
	init_ai->m_player->Norm2XY(pos, *m_XYpos); 
    Assert(m_XYpos); 

	m_ai = init_ai; 
}

GoalEnslave::GoalEnslave(AiMain *init_ai,
	BSetID &id,	
   ForeignCity *his_city)
{
    Init(); 
 
    m_his_city_id = id; 
    foreign_player = his_city->GetPlayerIndex(); 

	MapPointData pos;

	his_city->GetPos(pos);

	*m_pos = pos;

	
    Assert(m_XYpos); 
	init_ai->m_player->Norm2XY(pos, *m_XYpos); 
    Assert(m_XYpos); 

	m_ai = init_ai; 
}

GoalEnslave::GoalEnslave(AiMain *ai, CivArchive &archive)
{ 
    Init(); 
	m_ai = ai; 

    Serialize(ai, archive); 
} 

void GoalEnslave::Init()
{
	
	the_class = SLAVER_GOAL_CLASS;

    
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_ENSLAVE;

    foreign_player = -1; 

	
    
    
    
    

    
    
    
    
    

	m_value = -1;			
}

GoalEnslavePtr::~GoalEnslavePtr()
{ 
} 

void GoalEnslavePtr::Store(CivArchive &archive)
{
    Assert(0 <= foreign_player); 
    Assert(foreign_player < k_MAX_PLAYERS); 

    archive << foreign_player; 

    if (m_his_agent_id.GetVal()) { 
        archive.PutSINT32(2); 
        archive.PutUINT32(m_his_agent_id.GetVal()); 
    } else if (m_his_city_id.GetVal()) { 
        archive.PutSINT32(1); 
        archive.PutUINT32(m_his_city_id.GetVal()); 
    } else { 
        INSANE(GOAL_ENSLAVE_HAS_NO_DATA); 
        archive.PutSINT32(0); 
    } 
}

void GoalEnslavePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
    

    archive >> foreign_player; 

    Assert(0 <= foreign_player); 
    Assert(foreign_player < k_MAX_PLAYERS); 


    sint32 target_type; 
    target_type = archive.GetSINT32(); 
    uint32 val; 
    switch(target_type) { 
    case 2:
        val = archive.GetUINT32(); 
        m_his_agent_id = val;
        break; 
    case 1:
        val = archive.GetUINT32(); 
        m_his_city_id = val;
        break; 
    default: 
        INSANE(GOAL_ENSLAVE_HAS_NO_DATA); 
        break;
    } 
}

void GoalEnslavePtr::Serialize(CivArchive &archive)
{ 
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        Store(archive); 
    } else { 
        sint32 pcount=0;
        uint8 nmask[10];

        Load(archive, pcount, nmask);
    }
}

void GoalEnslave::Serialize(AiMain *ai, CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
    GoalEnslaveFlat::Serialize(archive); 
    GoalEnslavePtr::Serialize(archive); 
}


BOOL GoalEnslave::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}


void GoalEnslave::HookUp(AiMain *ai)
{ 
    Assert(0 <= foreign_player); 
    Assert(foreign_player < k_MAX_PLAYERS); 

    Assert(ai->m_foreigner[foreign_player]); 

}


GOAL_TYPE GoalEnslave::GetType () const
{
	return GOAL_TYPE_ENSLAVE; 
}







void GoalEnslave::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_ENSLAVE"));
#endif DBGAILOG
}









BOOL GoalEnslave::GetPos(MapPointData &pos)
{
	ForeignAgent *his_agent = GetAgentTarget(m_ai);
	ForeignCity *his_city = GetCityTarget(m_ai);

    if (his_agent) { 
        his_agent->GetPos(pos); 
        return TRUE; 
	}
	else if (his_city) {
		his_city->GetPos(pos); 
		return TRUE; 
	} 

    Assert(FALSE); 
	return FALSE;
}

BOOL GoalEnslave::IsCity()
{
    return GetCityTarget(m_ai) != NULL; 
}

void GoalEnslave::CleanUp()
{
	ForeignAgent *his_agent = GetAgentTarget(m_ai);
	ForeignCity *his_city = GetCityTarget(m_ai);
	
    if (his_agent) { 
        his_agent->RemoveEnslaveGoal();
    } else { 
		if (his_city)
			his_city->RemoveEnslaveGoal();  
    } 
}









void GoalEnslave::Set_Invalid() 
{
	invalid_goal = true;

	CleanUp();
	
	Goal::Set_Invalid();
}
















double GoalEnslave::Compute_Raw_Priority(AiMain *ai)
{
#ifdef _DEBUG
    MapPointData *size; 
    size = ai->m_map->GetSize(); 
    Assert(m_pos); 
    Assert(m_XYpos); 
    Assert(0 <= m_XYpos->x); 
    Assert(m_XYpos->x < size->x*2); 
    Assert(0 <= m_XYpos->y); 
    Assert(m_XYpos->y < size->y); 
    Assert(0 <= m_XYpos->z); 
    Assert(m_XYpos->z < size->z); 

    Assert(0 <= m_pos->x); 
    Assert(m_pos->x < size->x); 
    Assert(0 <= m_pos->y); 
    Assert(m_pos->y < size->y); 
    Assert(0 <= m_pos->z); 
    Assert(m_pos->z < size->z); 
#endif
	
	raw_priority = ai->m_planner->the_AIP.enslave_priority;
	
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		 0, 
		-1, 
		 1, 
		 1, 
		-1, 
		-1, 
		 1); 

    if (ai->m_world->IsWatchfulCityHere(m_pos)) raw_priority -= 500.0; 

	
	return raw_priority;
}
























void GoalEnslave::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
 	if (SUB_TASK_CARGO_TO_BOARD == sub_task)
	{
		the_army->TryToBoardTransport(ai, the_transport);
		return;
	}

	Set_Totally_Complete(TRUE); 
    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 

	
	if (sub_task != SUB_TASK_GOAL)
		return;

    MapPointData dummy_pos; 
    the_army->ActionTryToEnslave(ai, dummy_pos, this);  
}







ForeignAgent *GoalEnslave::GetAgentTarget(AiMain *ai) 
{ 
    if (m_his_agent_id.GetVal()) { 
		
		if (!ai->m_foreigner) return NULL;
		if (!ai->m_foreigner[foreign_player]) return NULL;

        BSet<ForeignAgent> *vua = ai->m_foreigner[foreign_player]->GetVisibleUnits();
        BSetID h_id; 
        ForeignAgent *his_agent=NULL; 
        for (his_agent = vua->First(h_id); vua->Last(); his_agent =  vua->Next(h_id)) { 
            if (his_agent->GetID() == m_his_agent_id) {  
                return his_agent;
            }
		}
	}

	return NULL;
} 













ForeignCity *GoalEnslave::GetCityTarget(AiMain *ai) 
{ 
    if (m_his_city_id.GetVal()) { 
		
		if (!ai->m_foreigner) return NULL;
		if (!ai->m_foreigner[foreign_player]) return NULL;

        BSet<ForeignCity> *vua = ai->m_foreigner[foreign_player]->GetKnownCities();
        BSetID h_id; 
        ForeignCity *his_city=NULL; 
        for (his_city = vua->First(h_id); vua->Last(); his_city =  vua->Next(h_id)) { 
            if (his_city->GetID() == m_his_city_id) {  
                return his_city;
            }
		}
	}

	return NULL;
} 













BOOL GoalEnslave::WithinRange
(
    AiMain *ai, 
	Agent *agent,
    const SUB_TASK_TYPE & sub_task
)
{
	ArmyAgent *army_agent = (ArmyAgent *) agent;

	
	switch (agent->GetType())
	{
	case AGENT_TYPE_ARMY:
		
		if (sub_task == SUB_TASK_CARGO_TO_BOARD ||
			sub_task == SUB_TASK_TRANSPORT_TO_BOARD)
			return army_agent->AtEndOfPath(ai);

		
		return (army_agent->GetRemainingPathPoints(ai) <= 1);
		break;
	case AGENT_TYPE_CITY:
		break; 
	case AGENT_TYPE_UNKNOWN:
	default:
		INSANE(DISTANCE_TO_SQUAD_GOAL_UNKNOWN); 
		
		break;
	} 
	return false;
}




