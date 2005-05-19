






































































































































































































































































































































































































































#include "c3.h"
#include "globals.h"
#include "dynarr.h"
#include "IMapPointData.h"
#include "civarchive.h"


#include "AiMap.h"
#include "aicell.h"
#include "airndcnt.h"


#include "ic3GameState.h"
#include "ic3player.h"
#include "ic3world.h"
#include "ic3Rand.h"
#include "aimain.h"
#include "Foreigner.h"


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "InstBidList.h"







#include "common.h"
#include "linked_list.h"
#include "sorted_array.h"


#include "unitflow.h"


#include "aip.h"


#include "squad.h"
#include "Cont.h"


#include "goal.h"
#include "ArmyGoal.h"


#include "Plan.h"


#include "Scheduler.h"
#include "squad_strength.h"

#include "planner.h"
#include "scienceagent.h"


#include "dr_debug.h"
#include "matrix.h"
#include "grassfire.h"
#include "strategic_map.h"

#include "debugmemory.h"

#define LOG_ARMYGOAL true

#ifdef DBGAILOG
#define SUPER_INFINITE_LOOP_DEBUG
#endif 

#ifndef _DEBUG_MEMORY

#define STOMPCHECK() \
 if (m_current_needed_strength) \
 { \
	 Assert(_CrtIsMemoryBlock(m_min_needed_strength, sizeof(Squad_Strength), NULL,NULL,NULL));\
	 Assert(_CrtIsMemoryBlock(m_current_needed_strength, sizeof(Squad_Strength), NULL,NULL,NULL));\
	 Assert(_CrtIsMemoryBlock(m_current_attacking_strength, sizeof(Squad_Strength), NULL,NULL,NULL));\
  }\
	 Assert(m_pos);\
	 Assert(m_XYpos);\
	 Assert(_CrtIsMemoryBlock(m_pos, sizeof(MapPointData), NULL, NULL, NULL));\
	 Assert(_CrtIsMemoryBlock(m_XYpos, sizeof(MapPointData), NULL, NULL, NULL));\
  if (m_pos) Assert(_CrtIsMemoryBlock(m_pos, sizeof(MapPointData),NULL,NULL,NULL));
#else
#define STOMPCHECK() ;
#endif



ArmyGoal::ArmyGoal()
{
	m_pos = NULL;
	m_XYpos = NULL;
    Init(); 

} 








ArmyGoal::ArmyGoal
(
	AiMain *init_ai,					
	const double v,						
	const MapPointData &pos				
)
{
	m_pos = NULL;
	m_XYpos = NULL;

    Init(); 

	
    m_value = v; 

	

	m_pos->x = pos.x; 
 	m_pos->y = pos.y; 
	m_pos->z = pos.z; 

	
    Assert(m_XYpos); 
	init_ai->m_player->Norm2XY(pos, *m_XYpos); 
    Assert(m_XYpos); 

	
	if (init_ai->m_world->IsArmyHere(m_pos))
	{
		m_target_owner = init_ai->m_world->GetArmyOwner(m_pos);
	}
	else if (init_ai->m_world->IsCityHere(m_pos))
	{
		m_target_owner = init_ai->m_world->GetCityOwner(m_pos);
	}
	else 
	{
		m_target_owner = init_ai->m_world->GetCellOwner(m_pos);
		
		if (m_target_owner < 0)
			m_target_owner = 0;
	}

	Assert(m_target_owner >= 0);
	Assert(m_target_owner < k_MAX_PLAYERS);
}





void ArmyGoal::Init()
{ 
    m_satisfied = FALSE; 

    m_pos = new MapPointData; 
	Assert(m_pos);
    m_pos->x = -1; 
    m_pos->y = -1; 
    m_pos->z = -1; 

    m_XYpos = new MapPointData; 
    Assert(m_XYpos); 
    m_XYpos->x = -1; 
    m_XYpos->y = -1; 
    m_XYpos->z = -1; 

#ifndef _TEST
	STOMPCHECK() 
#endif

	m_value = -1;			

	
	removal_time = DONT_REMOVE;

	m_target_owner = 0;

    InitStrength(); 
}

ArmyGoal::~ArmyGoal()
{
	
	Set_Invalid();

#ifndef _TEST
	STOMPCHECK() 
#endif
	NDELETE(m_pos); 
	NDELETE(m_XYpos); 
}







 
void ArmyGoal::InitStrength()
{
#ifndef _TEST
	STOMPCHECK() 
#endif
    m_min_needed_strength->AddUnits(1); 

    *m_current_needed_strength = *m_min_needed_strength;
}

void ArmyGoal::Serialize(AiMain *ai, CivArchive &archive)
{
	
    CHECKSERIALIZE

	Goal::Serialize(ai, archive); 

    Assert(m_pos);
    Assert(m_XYpos); 

	
    if (archive.IsStoring()) { 

        archive << m_value; 

        archive.PutSINT32(m_satisfied); 

        Assert(m_pos); 
        archive << m_pos->x; 
        archive << m_pos->y; 
        archive << m_pos->z;

        Assert(m_XYpos); 
        archive << m_XYpos->x; 
        archive << m_XYpos->y; 
        archive << m_XYpos->z;

    } else { 
        archive >> m_value; 

        m_satisfied = BOOL(archive.GetSINT32());  

        Assert(m_pos); 
        archive >> m_pos->x; 
        archive >> m_pos->y; 
        archive >> m_pos->z;        

        Assert(m_XYpos); 
        archive >> m_XYpos->x; 
        archive >> m_XYpos->y; 
        archive >> m_XYpos->z;        
    }
}

BOOL ArmyGoal::Validate(AiMain *ai)
{
#ifndef _TEST
	STOMPCHECK() 
#endif

    sint32 r = Goal::Validate(ai);
    Assert(r); 

    Assert(m_pos);
	Assert(m_XYpos);

    return r;
}



BOOL ArmyGoal::PretestUnitTypes( AiMain *ai, ArmyAgent *army, GOAL_TYPE goal_type)
{
    if (goal_type != GOAL_TYPE_RETREAT) { 
        if (army->IsMigratingParatroop()) { 
            return FALSE; 
        } 
    } 

	switch (goal_type) {
			case GOAL_TYPE_TRANSPORT: 
			case GOAL_TYPE_NULL:
				
				break;
			case GOAL_TYPE_MAX:
				Assert(FALSE);
				break;
			case GOAL_TYPE_DEFENSE:
				if (army->GetDefenseStrength() < 1.0 ||
					army->GetAttackStrength() < 1.0 ||
					army->GetArmyContainsSettler() ||
					army->CanMoveWater(ai))
					return FALSE;
				
				
				if (army->CanMoveAir(ai))
					return FALSE;
				break;
			case GOAL_TYPE_HARASS_CITY:
			case GOAL_TYPE_SEIGE:
				if (army->CantCaptureCity(ai) ||
					army->GetDefenseStrength() < 1.0 ||
					army->GetAttackStrength() < 1.0 ||
					army->GetArmyContainsSettler())
					return FALSE;
			case GOAL_TYPE_PILLAGE:
			case GOAL_TYPE_PIRATE:
			case GOAL_TYPE_COUNTER_STEALTH:
			case GOAL_TYPE_GOODY_HUT:
			case GOAL_TYPE_HURT_ARMY:
			case GOAL_TYPE_SALLY:
			case GOAL_TYPE_RUSTLE:
			case GOAL_TYPE_ATTACK_REGION:
			case GOAL_TYPE_ATTACK:
			case GOAL_TYPE_REFORM_CITY:
				if (army->GetAttackStrength() < 1.0 ||
					army->GetArmyContainsSettler())
					return FALSE;
				break;
			case GOAL_TYPE_EXPEL:
				if (army->GetAttackStrength() < 1.0 &&
					!army->CanSue(ai))
					return FALSE;
				break;
			case GOAL_TYPE_PERIMETER:
			case GOAL_TYPE_CHOKEPOINT:
			case GOAL_TYPE_PATROL:
		        if (army->IsWounded() ||
					army->HasNoZOC(ai) ||
					army->GetAttackStrength() < 1.0 ||
					army->GetDefenseStrength() < 1.0 ||
					army->GetArmyContainsSettler())					
					return FALSE;
				break;
			case GOAL_TYPE_SETTLE:
				if (!army->GetArmyContainsSettler())
					return FALSE;
				break;
			case GOAL_TYPE_EXPLORE:
				if (army->CanBeRustled(ai))
					return FALSE;
				break;
			case GOAL_TYPE_CONSTRUCT_BUILDING:
			case GOAL_TYPE_CONSTRUCT_FREIGHT:
			case GOAL_TYPE_CONSTRUCT_WONDER:
			case GOAL_TYPE_BUILD_SUPPLEMENTAL:
			case GOAL_TYPE_BUILD_LIST:
				if (army->GetType() != AGENT_TYPE_CITY)
					return FALSE;
				break;
			case GOAL_TYPE_ENSLAVE:
				if (!army->CanEnslave(ai))
					return FALSE;
				break;
			case GOAL_TYPE_CONVERT_CITY:
				if (!army->CanConvertCity(ai))
					return FALSE;
				break;
            case GOAL_TYPE_BIOTERROR_CITY:
                if (!army->CanBioterrorCity(ai)) 
					return FALSE;
				break;
            case GOAL_TYPE_NANOATTACK_CITY: 
                if (!army->CanNanoattackCity(ai)) 
					return FALSE; 
				break;
			case GOAL_TYPE_BOMBARD:
				if (!army->CanBombard(ai, 0xffffffff))
					return FALSE;
				break;




			case GOAL_TYPE_BONUS_FOOD:
				if (!army->CanBonusFood(ai))
					return FALSE;
				break;
			case GOAL_TYPE_MAKE_PARK:
				if (!army->CanMakePark(ai))
					return FALSE;
				break;
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
				if (!army->CanCauseUnhappiness(ai))
					return FALSE;
				break;
			case GOAL_TYPE_ESTABLISH_EMBASSY:
				if (!army->CanEstablishEmbassy(ai))
					return FALSE;
				break;
			case GOAL_TYPE_FRANCHISING:
				if (!army->CanCreateFranchise(ai))
					return FALSE;
				break;
			case GOAL_TYPE_STEAL_TECHNOLOGY:
				if (!army->CanStealTechnology(ai))
					return FALSE;
				break;
			case GOAL_TYPE_INCITE_REVOLUTION:
				if (!army->CanInciteRevolution(ai))
					return FALSE;
				break;
			case GOAL_TYPE_INJOIN:
				if (!army->CanInjoin(ai))
					return FALSE;
				break;
			case GOAL_TYPE_ASSASINATE_RULER:
				if (!army->CanAssasinateRuler(ai))
					return FALSE;
				break;
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
				if (!army->CanUndergroundRailway(ai))
					return FALSE;
				break;
			case GOAL_TYPE_PLANT_NUKE:
			case GOAL_TYPE_NUKE_CITY:
				if (!army->IsNuclearAttack(ai))
					return FALSE;
				break;
			case GOAL_TYPE_RETREAT:
				
				break;
			case GOAL_TYPE_WANDER:
				
				break;
			case GOAL_TYPE_SUE_FRANCHISE:
				if (!army->CanSueFranchise(ai))
					return FALSE;
				break;
			case GOAL_TYPE_PROBE_WORMHOLE:
			case GOAL_TYPE_RECOVER_PROBE:
				if (!army->CanProbeWormhole(ai))
					return FALSE;
				break;
		}
	return TRUE;
}




BOOL ArmyGoal::GetPos(MapPointData &pos)
{ 
#ifndef _TEST
	STOMPCHECK() 
#endif
    pos = *m_pos; 
    return TRUE; 
} 


void ArmyGoal::SetSatisfaction (const BOOL s)
{
#ifndef _TEST
	STOMPCHECK() 
#endif
    m_satisfied = s; 
}

BOOL ArmyGoal::IsSatisfied() 
{
#ifndef _TEST
	STOMPCHECK() 
#endif
    return m_satisfied; 
}


void ArmyGoal::GetGoal(double &value, 
    GOAL_TYPE &gt, BOOL &is_sat) const
{
#ifndef _TEST
	STOMPCHECK() 
#endif
    value = m_value; 
    is_sat = m_satisfied;
    
    gt = GetType(); 
}










void ArmyGoal::Classify_Goal()
{
	
	
	
	
	
#ifndef _TEST
	STOMPCHECK() 
#endif
	what_goal = GetType();
}













void ArmyGoal::Display_Goal_Type(AiMain *ai)
{
#ifndef _TEST
	STOMPCHECK() 
#endif
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "***UNKNOWN_GOAL***"));
#endif DBGAILOG

}











void ArmyGoal::GetDebugName(char *str)
{
#ifndef _TEST
	STOMPCHECK() 
#endif
    switch (what_goal) { 
    case GOAL_TYPE_NULL:
        sprintf (str, "bad!?!?"); 
        break; 
    case GOAL_TYPE_DEFENSE:
        sprintf (str, "Defense"); 
        break; 
    case GOAL_TYPE_SEIGE:
        sprintf (str, "Seige"); 
        break; 
    case GOAL_TYPE_ATTACK: 
        sprintf (str, "Attack"); 
        break; 
    case GOAL_TYPE_SETTLE: 
        sprintf (str, "Settle"); 
        break; 
    case GOAL_TYPE_EXPLORE: 
        sprintf (str, "Explore"); 
        break; 
    case GOAL_TYPE_CONSTRUCT_BUILDING:
        sprintf (str, "BLG"); 
        break; 
    case GOAL_TYPE_CONSTRUCT_FREIGHT:
        sprintf (str, "FREIGHT"); 
        break; 
    case GOAL_TYPE_CONSTRUCT_WONDER:
        sprintf (str, "WONDER"); 
        break; 
    case GOAL_TYPE_TRANSPORT:    
        sprintf (str, "Transport"); 
        break; 
    case GOAL_TYPE_ENSLAVE:
        sprintf (str, "Enslave"); 
        break; 
    case GOAL_TYPE_CONVERT_CITY:
        sprintf (str, "Convert city"); 
        break; 
    case GOAL_TYPE_BIOTERROR_CITY:
        sprintf (str, "Bioterror city"); 
        break; 
    case GOAL_TYPE_NANOATTACK_CITY: 
        sprintf (str, "Nanoattack city"); 
        break; 
    case GOAL_TYPE_HARASS_CITY:
        sprintf (str, "Harass city"); 
        break; 
    case GOAL_TYPE_BOMBARD:
        sprintf (str, "Bombard"); 
        break; 
    case GOAL_TYPE_BUILD_SUPPLEMENTAL:
        sprintf (str, "Bsup"); 
        break; 
    case GOAL_TYPE_BUILD_LIST:
        sprintf (str, "Blist"); 
        break; 
    case GOAL_TYPE_PERIMETER:
        sprintf (str, "Perimeter"); 
        break; 
    case GOAL_TYPE_CHOKEPOINT:
        sprintf (str, "Chokepoint"); 
        break; 
    case GOAL_TYPE_PATROL:
        sprintf (str, "Patrol"); 
        break; 
    case GOAL_TYPE_RUSTLE:
        sprintf (str, "Rustle"); 
        break; 
	case GOAL_TYPE_EXPEL:
        sprintf (str, "Expel"); 
        break; 
	case GOAL_TYPE_SALLY:
        sprintf (str, "Sally"); 
        break; 
    case GOAL_TYPE_ATTACK_REGION:
        sprintf (str, "Attack Region"); 
        break; 
    case GOAL_TYPE_RETREAT:
        sprintf (str, "Retreat!"); 
        break; 
    case GOAL_TYPE_PROBE_WORMHOLE:
        sprintf (str, "Probe"); 
        break; 
    case GOAL_TYPE_RECOVER_PROBE:
        sprintf (str, "Recover"); 
        break; 
    default:
        INSANE(UNKNOWN_GOAL_TYPE); 
        sprintf (str, "*unknown*"); 
        break;
	}
}













void ArmyGoal::Display_Goal_Details(AiMain *ai)
{
	
}















int ArmyGoal::Is_Satisfied
(
	int *excess						
)
{
	if (attacking_squad == NULL) 
		return FALSE;

	
	if (m_current_needed_strength->IsEnough(m_current_attacking_strength, m_farthest_agent_to_target_dist))
		return TRUE;

	return FALSE;
}



















BOOL ArmyGoal::Execute_Incrementally()
{
	
	

	
	if (!attacking_squad)
		return FALSE;

	
	if (attacking_squad->GetUnitCount() <= 0)
		return FALSE;

	
	if ((what_goal == GOAL_TYPE_DEFENSE) ||
        (what_goal == GOAL_TYPE_RETREAT))
		return TRUE;

	return FALSE;
}










double ArmyGoal::Distance_From_Squad_To_Goal
(
    AiMain *ai, 
	squad_ptr the_squad				
)
{
#ifndef _TEST
	STOMPCHECK() 
#endif
	
	int i;								
    Agent *agent = NULL;				
    MapPointData agent_pos;				
	double summed_distance = 0.0;		
	double average_distance;			
	

	
	if (the_squad->my_agents.count is 0)
	{
		return A_REALLY_BIG_POSITIVE_REAL_NUMBER;

	} 

	
	for (i=0; i < attacking_squad->my_agents.count; i++)
	{
		
		agent = (Agent *) attacking_squad->my_agents.Return_Data_By_Number(i);

		
		switch (agent->GetType())
		{
		case AGENT_TYPE_ARMY:
			
			((ArmyAgent *)agent)->GetPos(ai, agent_pos ); 

			
			summed_distance += ai->m_map->WrappedDistance(*m_pos, agent_pos);

			break;
		case AGENT_TYPE_CITY:
			
			
			((CityAgent *)agent)->GetPos( agent_pos ); 

			
			summed_distance += ai->m_map->WrappedDistance(*m_pos, agent_pos);

			break;
		case AGENT_TYPE_UNKNOWN:
		default:
            INSANE(DISTANCE_TO_SQUAD_GOAL_UNKNOWN); 
			
			break;

		} 

	} 

	
	average_distance = summed_distance / attacking_squad->my_agents.count;

	
	return average_distance;
}














void ArmyGoal::Compute_Needed_Troop_Flow(AiMain *ai)
{
	
	


}















double ArmyGoal::Compute_Matching_Value
(
	AiMain *ai,							
	squad_ptr the_squad				
)
{
#ifndef _TEST
	STOMPCHECK() 
#endif

	
	double utility;					
	bool same_squad_goal_combo = false;	
	

	
	utility = the_squad->Compute_Utility( this );

	
	if (utility <= BAD_UTILITY) 
		return BAD_UTILITY;

	
	same_squad_goal_combo = (the_squad->last_goal_type is what_goal);

	
	if (same_squad_goal_combo)
	{
		
		utility += ai->m_planner->the_AIP.persistence_modifier;

	} 

	
	return ((double) utility);
}
















double ArmyGoal::Compute_Raw_Priority(AiMain *ai)
{
	
	

	
	
#ifndef _TEST
	STOMPCHECK() 
#endif
	BOOL NO_COMPUTE_RAW_PRIORITY_DEFINED = 0;
	Assert(NO_COMPUTE_RAW_PRIORITY_DEFINED);

	return 0.0;
}
















double ArmyGoal::Compute_Raw_Priority(AiMain *ai, 
									  sint32 threat_sign, 
									  sint32 danger_sign,  
									  sint32 value_sign,
									  sint32 power_sign,
									  sint32 distance_to_home_sign,
									  sint32 distance_to_enemy_sign,
									  sint32 chokepoint_sign)
{
	double priority = 0.0;
	Strategic_Map *strategic_map = ai->m_planner->the_strategic_map; 

	
    Assert(m_XYpos);
	Assert(threat_sign >= -1 && threat_sign <= 1);
	Assert(danger_sign >= -1 && danger_sign <= 1);
	Assert(value_sign >= -1 && value_sign <= 1);
	Assert(power_sign >= -1 && power_sign <= 1);
	Assert(chokepoint_sign >= -1 && chokepoint_sign <= 1);
	Assert(distance_to_home_sign >= -1 && distance_to_home_sign <= 1);
	Assert(distance_to_enemy_sign >= -1 && distance_to_enemy_sign <= 1);
	
	
	priority += (strategic_map->Get_Threat_Bonus(ai, *m_XYpos) * threat_sign);

	
	priority += (strategic_map->Get_Danger_Bonus(ai, *m_XYpos) * danger_sign);

	
	priority += (strategic_map->Get_Value_Bonus(ai, *m_XYpos) * value_sign);

	
	priority += (strategic_map->Get_Power_Bonus(ai, *m_XYpos) * power_sign);

	
	priority += (strategic_map->Get_Distance_To_Our_Empire(*m_XYpos) * 
				 ai->m_planner->the_AIP.distance_from_home_priority_modifier *
				 distance_to_home_sign);

	
	priority += (strategic_map->Get_Distance_To_Enemy_Empire(*m_XYpos) * 
				 ai->m_planner->the_AIP.distance_from_enemy_priority_modifier *
				 distance_to_enemy_sign);
			
	
	priority += (ai->m_world->GetIsChokePoint(*m_pos) *
				 ai->m_planner->the_AIP.max_chokepoint_raw_bonus *
				 chokepoint_sign);
 
	
	
	switch(the_class)
	{
	case SETTLER_GOAL_CLASS:
	case CITY_GOAL_CLASS:
	case EXPLORE_GOAL_CLASS:
	case DEFENSE_GOAL_CLASS:
		break;
	default:
		Assert(ai);
		Assert(ai->m_foreigner);
		
		if (m_target_owner >= 0 &&
			m_target_owner < k_MAX_PLAYERS &&
			ai->m_foreigner[m_target_owner] != 0 &&
			m_target_owner != ai->my_player_index)
		{
			Assert(ai->m_foreigner[m_target_owner]);
			if (!ai->m_foreigner[m_target_owner]->IsRobot())
				priority += ai->m_planner->the_AIP.human_target_raw_bonus;
		}
	}

	
	return priority;
}

















int ArmyGoal::Is_Unit_Appropriate
(
	AiMain *ai,							
	Agent * unit_in_question			
)
{
#ifndef _TEST
	STOMPCHECK() 
#endif

	
	AGENT_STATE agent_state;			
	AGENT_TYPE  agent_type;				
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	


	
	agent_type = unit_in_question->GetType();
	switch (agent_type)
	{
	case AGENT_TYPE_ARMY:
	{
		
		switch (GetType()) 
		{ 
		case GOAL_TYPE_CONSTRUCT_WONDER:
		case GOAL_TYPE_CONSTRUCT_BUILDING:
		case GOAL_TYPE_CONSTRUCT_FREIGHT:
			return false;
		default:
			break; 
		} 


		
		the_army = (ArmyAgent *) unit_in_question;

		
		
		if (the_army->PretestBid(ai, what_goal, *m_pos) <= BAD_UTILITY)
			return FALSE;

		if (!ArmyGoal::PretestUnitTypes(ai, the_army, GetType()))
			return FALSE;

		
		
		
		if (the_army->IsInCity(ai, the_city) &&
			ArmyGoal::PretestUnitTypes(ai, the_army, GOAL_TYPE_DEFENSE))
			{
				sint32 units_in_city = (sint32) the_city->GetGarrisonCount();
				Assert(the_city);
				MapPointData pos;
				the_city->GetPos(pos);
				
				
				if (!(pos == *m_pos))
				{
					if (units_in_city - the_army->GetNUnits() < 
						ai->m_planner->the_AIP.num_city_defenders ||
						units_in_city - the_army->GetNUnits() <
						the_city->GetMinSlaveGarrison())
					return FALSE;
				}
			}

		
		
		agent_state = the_army->GetState();
		switch (agent_state) 
		{ 
		case AGENT_STATE_MARKED_SETTLE:
		case AGENT_STATE_MARKED_DEAD:            
		case AGENT_STATE_MARKED_GARRISON:
		case AGENT_STATE_AT_GOAL:
			return false;
		default:
			return true;
		}
	} 
	break;
	case AGENT_TYPE_CITY:
	{
		
		the_city = (CityAgent *) unit_in_question;

		
		switch (GetType()) 
		{ 
		case GOAL_TYPE_DEFENSE:

		case GOAL_TYPE_ATTACK: 
		case GOAL_TYPE_SETTLE: 
		case GOAL_TYPE_EXPLORE: 
		case GOAL_TYPE_TRANSPORT:
		case GOAL_TYPE_SEIGE:
        case GOAL_TYPE_ENSLAVE:

        case GOAL_TYPE_CONSTRUCT_BUILDING:
        case GOAL_TYPE_CONSTRUCT_FREIGHT:
        case GOAL_TYPE_CONSTRUCT_WONDER:
			
			if (the_city->GetQueueLen() < 1)
				return true; 
			else return false;

		default:
		    return FALSE; 
		} 
	}
	default:
		return false;
	}
}











void ArmyGoal::EstimateRecruitmentStrength
(
    AiMain *ai, 
    Agent *donor_agent, 
    double &str_score
)
{
#ifndef _TEST
	STOMPCHECK() 
#endif

    sint32 t; 



    CityAgent *the_city=NULL; 
    ArmyAgent *the_army=NULL; 
    switch (donor_agent->GetType()) { 
    case AGENT_TYPE_CITY: 
        the_city = (CityAgent *) donor_agent;
        if (the_city->GetQueueLen() >= 1) {
			str_score = -1.0;
            return; 
        } 
        break; 
    case AGENT_TYPE_ARMY:
        the_army = (ArmyAgent *) donor_agent;

        if ((the_army->GetMovePoints() < MINIMUM_MOVEMENT_POINTS_TO_MOVE) ||
            (the_army->IsWaitState())) { 
            str_score = -1.0; 
            return; 
        } 
        break; 
    default:
        INSANE(UNKNOWN_AGENT_TYPE); 
    } 

    if (0 < m_current_needed_strength->GetTransport()) { 

        sint32 needed_transport = m_current_needed_strength->GetTransport() - 
            m_current_attacking_strength->GetTransport(); 

        if (donor_agent->GetType() == AGENT_TYPE_ARMY) { 

            t = donor_agent->GetSquadStrength()->GetTransport() - needed_transport; 

            if (0 <= t) { 
                str_score = 1.0;  
            } else { 
                str_score = -1.0; 
            } 
        } else { 
            str_score = -1.0; 
        } 
    } else { 
        
        
        if (Is_Unit_Appropriate(ai, donor_agent)) { 
			if (donor_agent->GetType() == AGENT_TYPE_ARMY)
				str_score = the_army->Estimate_Army_Strength(ai); 
			else
				str_score = 1;
        } else { 
            str_score = -1.0;
        } 

    }
}














void ArmyGoal::Commit_Unit
(
	Agent * unit_in_question		
)
{
}





























































































































































































































Goal_Result ArmyGoal::Execute_Task(
    AiMain *ai, 
    Plan *the_plan
) {

#ifndef _TEST
	STOMPCHECK() 
#endif
    Assert(m_XYpos);

    Display_Goal_Type(ai); 

    Goal_Result result;

    
    if (IsExecutionHalted(ai, result)) return result;

    sint32 agent_count = attacking_squad->my_agents.count;

	
	

    MapPointData goal_pos;
    GetPos(goal_pos);   

    Agent *agent=NULL; 
    sint32 i;









	
    sint32 build_count=0;
	for (i=agent_count-1; 0<=i; i--) {

		agent = (Agent *) attacking_squad->my_agents.Return_Data_By_Number(i);





        if (!agent || agent->m_has_been_executed) continue;  

        
		if (AGENT_TYPE_CITY == agent->GetType()) {

            
            if (GOAL_INAPPROPRIATE == BuildTaskSolution(ai, (CityAgent *) agent, the_plan)) { 
                return GOAL_INAPPROPRIATE; 
            } else { 
                build_count++; 
            } 
        } else {            

            
            result = GotoTaskSolution(ai, i, (ArmyAgent *) agent, 0, NULL, goal_pos, SUB_TASK_GOAL);

			if (result != GOAL_IN_PROGRESS)
				return result;
        }

        
        if (IsExecutionHalted(ai, result)) 
			return result;
    }

    Assert(m_XYpos); 

	
	return CheckCompletion(build_count);	
}









ArmyGoal::IsExecutionHalted(AiMain *ai, Goal_Result &result)
{

#ifndef _TEST
	STOMPCHECK() 
#endif
     Assert(m_XYpos); 

     if (ai->m_i_am_dead) { 
         result = GOAL_FAILED; 
         return TRUE; 
    } else if (Get_Totally_Complete(ai)) {
		result = GOAL_COMPLETE;
        return TRUE; 
    } else if (NULL == attacking_squad){
        
        
	    

		
		result = GOAL_FAILED;
        return TRUE; 
    } else { 
        result = GOAL_IN_PROGRESS; 
        return FALSE; 
    }
}









Goal_Result ArmyGoal::CheckCompletion(const sint32 build_count)
{
	
	
	int excess;
	if (Is_Satisfied(&excess) && 
        what_goal == GOAL_TYPE_DEFENSE)
	{
		dont_allocate = true;
		return GOAL_COMPLETE;
	}
    return GOAL_IN_PROGRESS; 
}







 
Goal_Result ArmyGoal::BuildTaskSolution(AiMain *ai, CityAgent *the_city,
    Plan *the_plan)
{ 
#ifndef _TEST
	STOMPCHECK() 
#endif

	
	
	
	
    ArmyAgent *unbuilt_army; 
	the_city->Find_Best_Utility_Agent_We_Can_Build(ai, this, unbuilt_army);

    double utility = the_plan->GetMatchingValue(); 

	
	
	
	

	if (unbuilt_army) {
        sint32 unit_idx = unbuilt_army->GetUnbuiltType(); 
		
		if (the_city->PretestContructUnit(ai, unit_idx))
		{
			the_city->EnqueueArmy(ai, unit_idx, utility);

			
			the_city->current_goal_type = what_goal;

			the_city->m_has_been_executed = TRUE;  
	        return GOAL_IN_PROGRESS;
		}
	}

	
	
	return GOAL_INAPPROPRIATE;
}









 
BOOL ArmyGoal::FindPathToTask(AiMain *ai, ArmyAgent *the_army,
     ArmyAgent *the_transport, MapPointData &goal_pos, 
     const SUB_TASK_TYPE sub_task, MapPointData &dest_pos) 
{
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Entering ArmyGoal::FindPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG

    sint32 cutoff = 200000; 
    sint32 nodes_opened; 
#ifndef _TEST
	STOMPCHECK() 
#endif
    BOOL check_rail_launcher = FALSE;
    BOOL is_unknown_id; 
    sint32 pathtime;
	BOOL check_dest;
    MapPointData start_pos; 
	
	BOOL transport_at_rendezvous;
	BOOL cargo_at_rendezvous;
    MapPointData meeting_pos; 

    the_army->GetPos(ai, start_pos); 

	if (what_goal == GOAL_TYPE_PROBE_WORMHOLE)
		{
			if (!the_army->HasProbedWormhole() &&
				ai->m_player->ArmyHasLeftMap(the_army->GetID().GetVal(), 
											 &is_unknown_id))
				
			{
				the_army->SetProbedWormhole(TRUE);
				return TRUE;
			}
			
			
			
			if (the_army->HasProbedWormhole())
				return FALSE;
		}

	
	
	switch (what_goal)
		{
			case GOAL_TYPE_ENSLAVE: 
			case GOAL_TYPE_CONVERT_CITY:
			case GOAL_TYPE_BIOTERROR_CITY: 
			case GOAL_TYPE_NANOATTACK_CITY: 
			case GOAL_TYPE_BOMBARD:
			
			case GOAL_TYPE_EXPEL:
			
			case GOAL_TYPE_PLANT_NUKE:
			case GOAL_TYPE_MAKE_PARK:
			case GOAL_TYPE_CAUSE_UNHAPPINESS:
			case GOAL_TYPE_ESTABLISH_EMBASSY:
			case GOAL_TYPE_FRANCHISING:
			case GOAL_TYPE_STEAL_TECHNOLOGY:
			case GOAL_TYPE_INCITE_REVOLUTION:
			case GOAL_TYPE_INJOIN:
			case GOAL_TYPE_ASSASINATE_RULER:
			case GOAL_TYPE_UNDERGROUND_RAILWAY:
			case GOAL_TYPE_SUE_FRANCHISE:
                { 
    			
                    check_dest = FALSE; 
                }
				break;
			default:
				
				check_dest = TRUE;
		}

    uint32 move_intersection;
    BOOL b;
    switch (sub_task) { 
    case SUB_TASK_GOAL: 
	case SUB_TASK_RALLYING: 
        if (!the_army->CanSpaceLaunch(ai) || !the_army->CanSpaceLand(ai)) { 
            if (ai->m_science_agent->CanRailLaunch()){
                if (ai->m_player->CanBeCargoPodded(&is_unknown_id, the_army->GetID().GetVal())) { 
                    check_rail_launcher = TRUE; 
                }
                Assert(!is_unknown_id); 
            }    
        }

        dest_pos = goal_pos; 

        b = the_army->FindPath(ai, goal_pos, pathtime, 
            check_rail_launcher, cutoff, nodes_opened, check_dest, what_goal); 
	
        if (b) { 
             Assert(0 < the_army->GetPathLen()); 
        }

        return b;

    case SUB_TASK_TRANSPORT_TO_BOARD:
		
		check_dest = FALSE;
        the_army->GetPos(ai, dest_pos); 
        if (start_pos.z != dest_pos.z) { 
            return FALSE; 
        } 

        move_intersection = the_transport->GetMoveIntersection(ai) |
            the_army->GetMoveIntersection(ai); 
 
        
        b = the_transport->FindPathToBoard(ai, move_intersection, dest_pos, 
            pathtime, cutoff, nodes_opened, check_dest, GOAL_TYPE_NULL);

		
		
		
		

        if (b) { 
            the_transport->RemoveLastPathPoint();
        } 
        return b;
    case SUB_TASK_CARGO_TO_BOARD:
		
        the_transport->GetPos(ai, dest_pos);
		the_army->GetPos(ai, start_pos);

		cargo_at_rendezvous = 
			(ai->m_map->WrappedDistance(start_pos, dest_pos) <= 1);

		if (the_transport->GetRemainingPathPoints(ai) >0 )
		{
			
			
			transport_at_rendezvous = 
				( the_transport->CanEnterNextPathPoint(ai) == FALSE);

			the_transport->GetNextPathPoint(meeting_pos);
		}
		else
		{
			
			transport_at_rendezvous = TRUE;
			cargo_at_rendezvous = TRUE;
		}

		
		if (transport_at_rendezvous &&
			!cargo_at_rendezvous)
		{
			
	        move_intersection = the_army->GetMoveIntersection(ai);
	        
			check_dest = TRUE;

			
		    b =  the_army->FindPathToBoard(ai, move_intersection, meeting_pos, 
			    pathtime, cutoff, nodes_opened, check_dest, GOAL_TYPE_NULL);
		}
		else
		{
			
	        move_intersection = the_transport->GetMoveIntersection(ai) |
								the_army->GetMoveIntersection(ai); 

			
			check_dest =  FALSE; 

			
		    b =  the_army->FindPathToBoard(ai, move_intersection, dest_pos, 
			    pathtime, cutoff, nodes_opened, check_dest, GOAL_TYPE_NULL);
		}
				
		return b;
    default:
        INSANE(SUB_TASK_UNKNOWN); 
        return FALSE;
    } 
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting ArmyGoal::FindPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
} 








 
Goal_Result ArmyGoal::GotoTaskSolution(AiMain *ai, const sint32 army_idx, 
     ArmyAgent *the_army, const sint32 transport_idx, ArmyAgent *the_transport, 
     MapPointData &goal_pos, 
     const SUB_TASK_TYPE sub_task)
{ 
#ifndef _TEST
	STOMPCHECK() 
#endif
   
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Entering ArmyGoal::GotoTaskSolution\n"));
#endif SUPER_INFINITE_LOOP_DEBUG

	MapPointData dest_pos; 

    if (FindPathToTask(ai, the_army, the_transport, goal_pos, sub_task, dest_pos)) {     
	
        switch (sub_task) { 
        case SUB_TASK_GOAL: 
		case SUB_TASK_RALLYING: 
			
			
			if (!attacking_squad->Ok_To_Rally(ai))
			{
				sint32 dest_cont;
				BOOL dest_is_land;
				BOOL r = ai->m_continents->GetContinent(ai,*m_pos, dest_cont, dest_is_land);
				
				if (the_army->GetMyIsLand() != dest_is_land ||
					(the_army->GetContinent() == dest_cont && 
					 the_army->GetMyIsLand() == dest_is_land))

					return GOAL_IN_PROGRESS;
				
				
				
			}

			



            if (FollowPathToTask(ai, the_army, the_transport, sub_task, dest_pos))
				return GOAL_IN_PROGRESS;
			else 
				return GOAL_CANT_REACH_TARGET;

            break; 
        case SUB_TASK_CARGO_TO_BOARD:
    	    



            if (FollowPathToTask(ai, the_army, the_transport, sub_task, dest_pos))
			{
				return GOAL_IN_PROGRESS; 
			}
			else
				return GOAL_INAPPROPRIATE;
            break; 

        case SUB_TASK_TRANSPORT_TO_BOARD: 

            



    		if (FollowPathToTask(ai, the_transport, the_army, sub_task, dest_pos))
				return GOAL_IN_PROGRESS;
			else
				return GOAL_INAPPROPRIATE;
            break; 
        }
    } else {
        switch (sub_task) { 
        case SUB_TASK_GOAL: 
		case SUB_TASK_RALLYING:
            return TryTransport(ai, army_idx, the_army, goal_pos); 
        default:
            return GOAL_INAPPROPRIATE; 
        }
    }
	return GOAL_INAPPROPRIATE;
}







BOOL ArmyGoal::WithinRange
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
		
		









			return army_agent->AtEndOfPath(ai);
		break;
	case AGENT_TYPE_CITY:
		break; 
	case AGENT_TYPE_UNKNOWN:
	default:
		INSANE(DISTANCE_TO_SQUAD_GOAL_UNKNOWN); 
		
		break;
	} 
	return FALSE;
}












 
BOOL ArmyGoal::FollowPathToTask(AiMain *ai, ArmyAgent *first_army, 
    ArmyAgent *second_army, const SUB_TASK_TYPE sub_task, 
    MapPointData &dest_pos)
{ 
    BOOL did_move = FALSE; 

#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Entering ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    first_army->m_has_been_executed = TRUE; 

	
	CityAgent *the_city;
	if (first_army->IsInCity(ai, the_city) &&
		ArmyGoal::PretestUnitTypes(ai, first_army, GOAL_TYPE_DEFENSE))
	{
		sint32 units_in_city = (sint32) the_city->GetGarrisonCount();
		Assert(the_city);
		MapPointData city_pos;
		the_city->GetPos(city_pos);
		
		
		if (!(city_pos == dest_pos))
		{
			if (units_in_city - first_army->GetNUnits() < 
				ai->m_planner->the_AIP.num_city_defenders ||
				units_in_city - first_army->GetNUnits() <
				the_city->GetMinSlaveGarrison())
			{
				
				
				
				return FALSE; 
			}
			the_city->RemoveFromGarrison((sint16)first_army->GetNUnits());
		}
	}

    while(1) { 
 Assert(++finite_loop < 10000); 

        if (WithinRange(ai,first_army, sub_task))  { 
            ArrivedAtTask(ai, first_army, second_army, sub_task); 
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 1 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
            return TRUE; 
        }

        if (first_army->OverFuelSafety(ai)) { 
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 2 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
            return FALSE; 
        }

        if (first_army->CanEnterNextPathPoint(ai)) { 
          
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "ArmyGoal::FollowPathToTask first_army->GotoNextPathPoint\n"));
#endif SUPER_INFINITE_LOOP_DEBUG

            first_army->GotoNextPathPoint(ai, did_move); 
            	
            if (ai->m_i_am_dead) { 
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 3 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
                return FALSE; 	
            } 
            
            if (AGENT_STATE_MARKED_DEAD == first_army->GetState()){
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 4 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
                return TRUE; 
            }

            if (!did_move) {
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "ArmyGoal::FollowPathToTask CantReachTaskThisTurn\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
                CantReachTaskThisTurn(ai, first_army); 

#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 5 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
                return TRUE; 
            } 
            
        } else { 
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "ArmyGoal::FollowPathToTask CantFollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
            if (CantFollowPathToTask(ai, first_army, sub_task)) 
				; 
			else
				if (sub_task == SUB_TASK_CARGO_TO_BOARD ||
					sub_task == SUB_TASK_TRANSPORT_TO_BOARD)
				{
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 6 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
					return TRUE; 
				}
				else
                {    

                    
                    first_army->m_has_been_executed = FALSE; 
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 7 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
					return FALSE; 
                }
        }
    }

#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting 8 ArmyGoal::FollowPathToTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG

	return TRUE;
}







void ArmyGoal::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army, 
    ArmyAgent *the_transport, SUB_TASK_TYPE sub_task)
{
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Entering ArmyGoal::ArrivedAtTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG

	switch (sub_task) { 
	case SUB_TASK_GOAL: 
		Set_Totally_Complete(TRUE); 
		the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 
		break;
	case SUB_TASK_TRANSPORT_TO_BOARD: 
		break;
	case SUB_TASK_CARGO_TO_BOARD: 
		the_army->TryToBoardTransport(ai, the_transport);
		break;
	case SUB_TASK_RALLYING: 
		break;
	}
#ifdef SUPER_INFINITE_LOOP_DEBUG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "Exiting ArmyGoal::ArrivedAtTask\n"));
#endif SUPER_INFINITE_LOOP_DEBUG
}







 
void ArmyGoal::CantReachTaskThisTurn(AiMain *ai, ArmyAgent *the_army)
{
    
    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 

} 

BOOL ArmyGoal::IsBeachAssault(AiMain *ai, const MapPointData &dest_pos)
{
	MapPointData tough_pos = dest_pos;
	return ai->m_player->IsOccupiedByForeigner(&tough_pos);
}

BOOL ArmyGoal::RetargetEmptyBeach(AiMain *ai, const MapPointData &target_pos, const sint16 &continent, const uint32 &depth)
{
	
	
	sint16 x,y,z;
	MapPointData wrapped_pos;

	
	if (ai->m_world->GetContinent( target_pos ) != continent)
		return FALSE;
	
	
	MapPointData tough_pos = target_pos;
	if (!ai->m_world->IsMoveTypeLand( &tough_pos ))
		return FALSE;

	
	if (!IsBeachAssault(ai,target_pos))
	{
		m_pos->x = target_pos.x;
		m_pos->y = target_pos.y;
		m_pos->z = GROUND_Z;
		return TRUE;
	}

	
	

	
	if (depth <= 0)
		return FALSE;

	
	z = GROUND_Z;

	

	
	x = target_pos.x+1;
	y = target_pos.y;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x-1;
	y = target_pos.y;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x+1;
	y = target_pos.y+1;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x+1;
	y = target_pos.y-1;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x-1;
	y = target_pos.y+1;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x-1;
	y = target_pos.y-1;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x;
	y = target_pos.y-1;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	
	x = target_pos.x;
	y = target_pos.y+1;
    ai->m_map->Wrap(x, y, z, wrapped_pos); 
	if (RetargetEmptyBeach(ai, wrapped_pos, continent, depth-1))
		return TRUE;

	return FALSE;
}








BOOL ArmyGoal::CantFollowPathToTask(AiMain *ai, ArmyAgent *the_army, const SUB_TASK_TYPE sub_task)
{
	ArmyAgent *the_transport = NULL;
	MapPointData dest_pos; 
	MapPointData goal_pos; 

	
	GetPos(goal_pos);

	
	the_army->SetPathValid(FALSE);

    switch (sub_task) { 
    case SUB_TASK_GOAL: 
	case SUB_TASK_RALLYING: 

		
		
		if (the_army->GotCargo())
		{
			if (!the_army->CanBeachAssault(ai, goal_pos) &&
				IsBeachAssault(ai, goal_pos))
			{
				
				if (!RetargetEmptyBeach(ai, goal_pos, ai->m_world->GetContinent( goal_pos ), 5))
					return FALSE; 
				else
					GetPos(goal_pos); 
			}
		
			
			if (the_army->DebarkCargoOntoPath(ai)) 
				return FALSE; 
		}

		
		if (FindPathToTask(ai, the_army, the_transport, goal_pos, sub_task, dest_pos))
			
			if (the_army->CanEnterNextPathPoint(ai))
				
				return TRUE;

		
		return FALSE;
        break; 
    case SUB_TASK_TRANSPORT_TO_BOARD: 
        break; 
    case SUB_TASK_CARGO_TO_BOARD: 
        break; 
    default:
        INSANE(SUB_TASK_UNKNOWN); 
    }
	return FALSE; 
}







double ArmyGoal::Action_Bid(AiMain *ai, Agent *agent)
{
	BOOL MUST_BE_DEFINED_FOR_ALL_NEW_SUBGOALS = 0;
    Assert(MUST_BE_DEFINED_FOR_ALL_NEW_SUBGOALS);
	return 0.0;
}







Goal_Result ArmyGoal::TryTransport(AiMain *ai, const sint32 army_idx, 
    ArmyAgent *the_army,  MapPointData &goal_pos) 
{
    sint32 best_transport_idx; 
    ArmyAgent *the_transport=NULL; 

    
    sint32 full, empty;
    if (the_army->GetNumCargo(ai, full, empty)) { 
        if (0 < full) { 
            return GOAL_INAPPROPRIATE; 
        } 
    }

	
    switch (what_goal) 
		{ 
		case GOAL_TYPE_PERIMETER:
		case GOAL_TYPE_CHOKEPOINT:
		case GOAL_TYPE_PATROL:
		case GOAL_TYPE_COUNTER_STEALTH:
		case GOAL_TYPE_SALLY:
		case GOAL_TYPE_PROBE_WORMHOLE:
		case GOAL_TYPE_RECOVER_PROBE:
		case GOAL_TYPE_GOODY_HUT:
		case GOAL_TYPE_PILLAGE:
		case GOAL_TYPE_PIRATE:
		case GOAL_TYPE_DEFENSE:
		#ifdef DBGAILOG
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
				AILOG(( wstr,  "TryTransport: not valid for this goal type\n"));
		#endif DBGAILOG
			return GOAL_INAPPROPRIATE; 
			break;  
		default:
			;
		}

	
	if (ai->m_world->GetContinent( goal_pos ) == 
		the_army->GetContinent())
		return GOAL_CANT_REACH_TARGET;
	
	
	the_transport = (ArmyAgent *)
		FindTranportInSquad(ai, army_idx, the_army, goal_pos, best_transport_idx);

    
    if (the_transport != NULL) { 

        Assert(the_transport); 

		#ifdef DBGAILOG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG(( wstr,  "TryTransport: loading transports in squad.\n"));
		#endif DBGAILOG

        
        return LoadTransport(ai, army_idx, the_army, best_transport_idx, 
            the_transport, goal_pos);        

    } else { 
    
        if (what_goal == GOAL_TYPE_RETREAT)  { 
            return GOAL_INAPPROPRIATE;
        } else  { 
			
            sint32 t = the_army->GetUnitCount() - the_army->GetTransports(ai); 
            m_current_needed_strength->AddTransports(t); 
			
			if (t > 0)
				{
					#ifdef DBGAILOG
					if (ai->AI_VERBOSITY >= LOG_DETAILS)
						AILOG(( wstr,  "TryTransport: not enough transport in squad.\n"));
					#endif DBGAILOG
					return GOAL_CANT_REACH_TARGET; 
				}
			else
				{
					#ifdef DBGAILOG
					if (ai->AI_VERBOSITY >= LOG_DETAILS)
						AILOG(( wstr,  "TryTransport: squad has enough transport.\n"));
					#endif DBGAILOG					
					
					
					return GOAL_CANT_REACH_TARGET;
				}
        }
    } 
}








Agent * ArmyGoal::FindTranportInSquad(AiMain *ai, const sint32 dont_check, 
     ArmyAgent *the_army, MapPointData &goal_pos, sint32 &best_transport_idx) 
{
    sint32 agent_idx; 
    sint32 agent_num = attacking_squad->my_agents.count;

    Agent *current_agent=NULL; 
    ArmyAgent *possible_transport;
    double max_utility = -1.0; 
    double utility; 
	Agent *best_transport = NULL;

    
    for (agent_idx=0; agent_idx < agent_num; agent_idx++) { 

        if (dont_check == agent_idx) continue; 

        current_agent = (Agent*)attacking_squad->my_agents.Return_Data_By_Number(agent_idx); 
        if (current_agent->GetType() == AGENT_TYPE_ARMY) {            
           possible_transport = (ArmyAgent *)current_agent; 

           
           

		   
		   utility = the_army->EstimateTransportUtility(ai, possible_transport); 

           if (max_utility < utility) { 
                max_utility = utility; 
                best_transport_idx = agent_idx;
				best_transport = current_agent;
           } 
        } 
    } 

    return best_transport; 
}








Goal_Result ArmyGoal::LoadTransport(AiMain *ai,
        const sint32 army_idx, ArmyAgent *the_army, 
        const sint32 transport_idx, ArmyAgent *the_transport, 
        MapPointData &dest_pos)
{
    Goal_Result result; 

    
    result = GotoTaskSolution(ai, army_idx, the_army, transport_idx, the_transport, 
        dest_pos, SUB_TASK_TRANSPORT_TO_BOARD);

	#ifdef DBGAILOG
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		if (result != GOAL_IN_PROGRESS)
			AILOG(( wstr,  "LoadTransport: Transport can not reach cargo.\n"));
	#endif DBGAILOG

    
    if (GOAL_IN_PROGRESS == result) { 
        result = GotoTaskSolution(ai, army_idx, the_army, transport_idx, the_transport, 
            dest_pos, SUB_TASK_CARGO_TO_BOARD);
	#ifdef DBGAILOG
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		if (result != GOAL_IN_PROGRESS)
			AILOG(( wstr,  "LoadTransport: Cargo can not reach transport.\n"));
	#endif DBGAILOG

    }


    return result; 
}

double ArmyGoal::GetDefenseStrength()
{
    return 0.0; 
}

double ArmyGoal::GetAttackStrength()
{
    return 0.0; 
}

double ArmyGoal::AddMinMaxRandomRange(AiMain *ai,
									  const double &value,
									  const double &min_percent,
									  const double &max_percent)
{
	Assert(min_percent <= 1.0);
	Assert(max_percent >= 1.0);
	double low = value * min_percent;
	double high = value * max_percent;
	
	if (high-low <= 0.0)
		return value;
	sint32 range = (sint32) ceil((high-low)*1000.0);
	double rand_val = (double) ai->m_rand->Next(range) / 1000.0;
	Assert(low+rand_val >= low);
	Assert(low+rand_val <= high);

	return low+rand_val;
}
