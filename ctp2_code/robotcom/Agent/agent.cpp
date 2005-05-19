




























































#include "c3.h"
#include "globals.h"
#include "civarchive.h"
#include "IMapPointData.h"
#include "ic3RobotAstar.h"
#include "ic3player.h"
#include "dynarr.h"
#include "aimain.h"

#include "common.h"

#include "linked_list.h"

#include "unitflow.h"
#include "squad.h"

#include "planner.h"

#include "dr_debug.h"

#include "ailog.h"


#include "bset.h"
#include "Agent.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "squad_strength.h"










Agent::Agent()
{ 
    m_id = 0; 

	
	the_squad = NULL;

	
	squads_list_cell_ptr = NULL; 

	
	agents_list_cell_ptr = NULL; 

	
	my_planner = NULL;

    m_agent_type = AGENT_TYPE_UNKNOWN; 

    m_agent_strength = new Squad_Strength();
    
    m_has_been_executed = FALSE; 
} 


Agent::~Agent()
{
	

	
	if (the_squad isnt NULL)
	{
		
		the_squad->Remove_Agent( this );

	} 

	
	if (my_planner != NULL)
	{
		
		if (agents_list_cell_ptr != NULL)
		{
			
			my_planner->Remove_Agent(this);

		} 

	} 

    Assert(m_agent_strength); 
    delete m_agent_strength;
    m_agent_strength = NULL; 
    
#ifdef _DEBUG
    
    m_agent_type = AGENT_TYPE(0xcdcdcdcd); 
	squads_list_cell_ptr = list_cell_ptr(0xcdcdcdcd);
	agents_list_cell_ptr = list_cell_ptr(0xcdcdcdcd);
	the_squad = squad_ptr(0xcdcdcdcd);
	my_planner= (Planner *)(0xcdcdcdcd);
#endif
}

void Agent::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) {             
       archive.PutSINT32(m_agent_type); 
       archive.PutSINT32(m_id.GetVal()); 

       archive.PutSINT32(m_has_been_executed); 

       m_agent_strength->Serialize(archive); 

    } else { 
        m_agent_type = AGENT_TYPE(archive.GetSINT32()); 
        m_id = archive.GetSINT32(); 

		
		
		
		
		
		


        m_has_been_executed = BOOL(archive.GetSINT32()); 

        m_agent_strength->Serialize(archive); 
	} 
}

BOOL Agent::Validate(AiMain *ai)
{
    switch (m_agent_type) { 
    case AGENT_TYPE_ARMY:
    case AGENT_TYPE_CITY:
        break; 
    default:
        Assert(FALSE); 
        return FALSE;
    }
    
    if (m_id.GetVal() <= uint32(0)) { 
        Assert(FALSE); 
        return FALSE; 
    } 

    Assert(m_agent_strength); 





    if (NULL == my_planner) {
        Assert(FALSE); 
        return FALSE; 
    } 
        
    return TRUE; 
}



BSetID Agent::GetID() const
{ 
    return m_id; 
}

AGENT_TYPE Agent::GetType() const
{ 
   return m_agent_type; 
} 

void Agent::Log_Type(AiMain *ai)
{
#ifdef DBGAILOG

    
	AGENT_STATE agent_state;			
	AGENT_TYPE  agent_type;				
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	int Q_length;						
	

	

    if (ai->AI_VERBOSITY < LOG_MEDIUM) return;

	
	switch (agent_type = GetType())
	{
	case AGENT_TYPE_ARMY:
		AILOG((wstr, "    AGENT_TYPE_ARMY"));

		
		the_army = (ArmyAgent *) this;

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_BUILDING: 
			AILOG((wstr, "    BUILDING \n"));
			break;
		case AGENT_STATE_UNBUILT: 
			AILOG((wstr, "    UNBUILT \n"));
			break;
		case AGENT_STATE_MARKED_SETTLE:
			AILOG((wstr, "    MARKED_SETTLE \n"));
			break;
		case AGENT_STATE_MARKED_DEAD:            
			AILOG((wstr, "    MARKED_DEAD \n"));
			break;
		case AGENT_STATE_MARKED_GARRISON:
			AILOG((wstr, "    MARKED_GARRISON \n"));
			break;
		case AGENT_STATE_AT_GOAL:
			AILOG((wstr, "    AT_GOAL \n"));
			break;
		default:
			AILOG((wstr, "    \n"));
			break;
		}

		
		the_army->Display_Army(ai);

		break;
	case AGENT_TYPE_CITY:
		
		the_city = (CityAgent *) this;

		
		Q_length = the_city->GetQueueLen();
		
		
		AILOG((wstr, "    AGENT_TYPE_CITY  Queue Length: %d\n", Q_length));

		break;

	default:
		AILOG((wstr, "    UNKNOWN TYPE \n"));
		break;
	} 
#endif DBGAILOG

}

double Agent::EstimateUtility(AiMain *ai, ArmyGoal *goal_obj)
{
    INSANE(AGENT_NO_UTILITY_FUNCTION); 
    return BAD_UTILITY; 
} 

void Agent::DumpStr(AiMain *ai, char *str)

{
    INSANE(AGENT_NO_STR); 
    return;
} 








SQUAD_CLASS Agent::Classify_Agents_Squad_Type(AiMain *ai)
{
    
	ArmyAgent * the_army;				
	SQUAD_CLASS squad_type;
	double defense;
	double attack;
	
	
	
	squad_type = DEFAULT_SQUAD;

	
	if (GetType() == AGENT_TYPE_CITY)
		return CITY_SQUAD;
	else
		the_army = (ArmyAgent *) this;

	
	
	
	

	
	if (the_army->CanProbeWormhole(ai))
		return WORMHOLE_PROBE_SQUAD;

	
	if (the_army->GetArmyContainsSettler())
		return SETTLER_SQUAD;

	
	defense = the_army->GetDefenseStrength();
	attack = the_army->GetAttackStrength();

	
	
	
	
	squad_type = EXPLORE_SQUAD;

	
	if (the_army->CanBombard(ai, 0xffffffff))
		return BOMBARD_SQUAD;

	
	else if ( attack > 0.0 )
		{
			
			squad_type = MILITARY_SQUAD;

			
			if (defense > 0.0)
				{
					
					squad_type = DEFENSE_SQUAD;

					
					if (!the_army->HasNoZOC(ai))
						squad_type = PROTECT_SQUAD;

					
					if (!the_army->CantCaptureCity(ai))
						squad_type = SEIGE_SQUAD;
				}
		}

	
	
	

	if (the_army->CanEnslave(ai))
		return SLAVER_SQUAD;

	if (the_army->CanConvertCity(ai))
		return GOD_SQUAD;

	if (the_army->CanStealTechnology(ai) ||
		the_army->CanAssasinateRuler(ai) ||
		the_army->CanInciteRevolution(ai) ||
		the_army->CanPlantNuke(ai) ||
        the_army->CanBioterrorCity(ai) || 
        the_army->CanNanoattackCity(ai))
		return ESPIONAGE_SQUAD;




	if (the_army->CanBonusFood(ai))
		return BONUS_FOOD_SQUAD;

	if (the_army->CanCauseUnhappiness(ai))
		return MEDIA_SQUAD;

	if (the_army->CanEstablishEmbassy(ai))
		return DIPLOMATIC_SQUAD;

	if (the_army->CanCreateFranchise(ai))
		return BUSINESS_SQUAD;

	if (the_army->CanInjoin(ai) ||
		the_army->CanSueFranchise(ai))
		return LAW_SQUAD;

	if (the_army->CanUndergroundRailway(ai))
		return ABOLITIONIST_SQUAD;


	if (the_army->CanMakePark(ai) ||
		the_army->IsNuclearAttack(ai))
		return DESTROY_CITY_SQUAD;

	
	return squad_type;
}




