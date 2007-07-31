













































































































































































































































































































































































#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"
#include "FlatPtr.h"
#include "CivArchive.h"
#include "IC3World.h"
#include "IC3Player.h"
#include "IC3UnitDB.h"


#include "AiMain.h"
#include "ArmyAgent.h"
#include "CityAgent.h"
#include "InstBidList.h"


#include "common.h"

#include "linked_list.h"
#include "sorted_array.h"

#include "Agent.h"
#include "unitflow.h"
#include "AIP.h"
#include "squad.h"

#include "dr_debug.h"

#include "AI_Match.h"

#include "AILog.h"


#include "Goal.h"
#include "ArmyGoal.h"
#include "GlAttck.h"

#include "Scheduler.h"
#include "Planner.h"
#include "Squad_Strength.h"
#include "AiMap.h"
#include "cont.h"


#include "Grassfire.h"
#include "Strategic_Map.h"

#define RALLY_SUCCESSFUL_RATIO (0.7)

#ifndef NDEBUG
static squad_ptr first_squad = NULL; 
#endif

ZEROMEM(Squad_Flat_Data); 
FLATSERIALIZE(Squad_Flat_Data); 

#define LOG_SQUADS true






Squad::Squad()
{
	
	

	
	my_agents.Init();

	
	


	
	Init();

}












void Squad::Init
(	
	Planner_ptr init_team				
)
{
	
	

	
	squad_type = DEFAULT_SQUAD;

	
	my_agents.Empty();
	my_agents.Init();

	
	my_strategic_goal = NULL;
    already_committed = FALSE; 

	last_goal_type = GOAL_TYPE_NULL;
    my_goal_id = SAVE_NO_GOAL; 
	m_moveType = 0x0;

	auxiliary_candidate = TRUE;

	
	resend = TRUE;

	
	

	m_squad_strength.Init();

	
	the_team = init_team;

	

	
	ignore_goals.Empty();

	
	if (the_team isnt NULL)
	{

		
		ai = the_team->ai;

		
		squad_ID = the_team->current_squad_ID;

		
		the_team->current_squad_ID++;

	} 

	
	else
	{
		
		ai = NULL;

		
		squad_ID = -1;

	} 

   
}












Squad::Squad
(	
	Planner_ptr init_team,				
	CivArchive &archive
)
{
	
	the_team = init_team;

	
	ai = the_team->ai;

	


    my_strategic_goal = NULL; 
    already_committed = FALSE; 

    my_goal_id = -1; 
	m_moveType = 0x0; 


	
	Serialize(archive);
}















Squad::~Squad()
{

	



	
	Clean_Up();

	LOG(("Freed Squad \n"));
}













void Squad::Clean_Up()
{
	
	Detach_Agents();

	
	Detach_Goal();
}













void Squad::Load(CivArchive &archive)
{
	
	int i;								
	Agent * the_agent= NULL;	        
	sint32 num_agents;					
	AGENT_TYPE real_type;				
	BSetID id;							
	


	
	
	

	m_squad_strength.Serialize(archive);

	
	num_agents = archive.GetSINT32() ; 

	
	for (i = 0; i < num_agents; i++)
	{
		
		id = archive.GetUINT32();

		
		real_type  = AGENT_TYPE(archive.GetSINT32());


		
		switch (real_type)
		{
		case AGENT_TYPE_ARMY:
			the_agent = the_team->ai->m_army_set->Find(id);
			break;
		case AGENT_TYPE_CITY:
			the_agent = the_team->ai->m_city_set->Find(id);
			break;
		default:
			
			INSANE(SQUAD_LOAD_UNKNOWN_AGENT);
			break;
		}

        Assert(the_agent); 
		
		Add_Agent(ai, the_agent);

	} 

    my_goal_id = archive.GetSINT32(); 
    my_strategic_goal = NULL; 
    already_committed = FALSE; 


}













void Squad::Store(CivArchive &archive)
{
	
	int i;								
	Agent * the_agent;					
	

	

	m_squad_strength.Serialize(archive);

	
	archive.PutSINT32(my_agents.count);

	
	for (i = my_agents.count - 1; i >= 0; i--)
	{
		
		the_agent = (Agent *)
			my_agents.Return_Data_By_Number( i );

		
        archive.PutUINT32(the_agent->GetID().GetVal());

        archive.PutSINT32(the_agent->GetType());


	} 

    archive.PutSINT32(my_goal_id); 
}













void Squad::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    Squad_Flat_Data::Serialize(archive); 

	
    if (archive.IsStoring())
	{
		
		Store(archive);

	} 
	else
	{
		
		Load(archive);

	} 

}













void Squad::Set_Goal 
(
  Goal_ptr new_strategic_goal
)

{
	
	if (new_strategic_goal is NULL) return;

	
	if (my_strategic_goal isnt NULL)
	{
		

		
		if (my_strategic_goal->attacking_squad is this)		{
			






			Detach_Goal();
		} 
		BOOL GOAL_DETACHED_WITHOUT_NOTIFYING_SQUAD = FALSE;
		Assert(GOAL_DETACHED_WITHOUT_NOTIFYING_SQUAD);
	} 

	
	my_strategic_goal = new_strategic_goal;
    my_goal_id = my_strategic_goal->GetID(); 
    already_committed = TRUE; 

	
    my_strategic_goal->my_squad_id = squad_ID; 
	new_strategic_goal->attacking_squad = this;

    
    *(my_strategic_goal->m_current_attacking_strength) = m_squad_strength; 

	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr,  "Squad::Set_Goal: Attaching squad %d to goal %d.\n", 
				squad_ID, new_strategic_goal->goal_ID));

		
		new_strategic_goal->Display_Goal_Type(ai);
		new_strategic_goal->Display_Goal_Details(ai);

		AILOG(( wstr,  "\n"));
	}
	#endif
	#endif DBGAILOG
}













void Squad::Detach_Goal()

{
	
	Goal *tmp_goal;
	if (my_strategic_goal isnt NULL)
	{
		
		
#ifdef DBGAILOG

#ifdef LOG_SQUADS
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
		{
			AILOG((wstr, "Detaching goal #%d ", my_strategic_goal->goal_ID));
			my_strategic_goal->Display_Goal_Type(ai);
			AILOG((wstr, "\n"));
		}
#endif
#endif DBGAILOG

		

		
		tmp_goal = my_strategic_goal;
		my_strategic_goal = NULL;
        tmp_goal->Detach_Squad();

		


		
		tmp_goal->CleanUp();
	}

	
	my_strategic_goal = NULL;
    my_goal_id = SAVE_NO_GOAL; 



	last_goal_type = GOAL_TYPE_MAX;

	
	already_committed = FALSE;

}














void Squad::Detach_Agents()
{
    sint32 agent_idx, agent_num; 
	Agent *agent;

    agent_num = my_agents.count; 

    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
        agent = (Agent*) my_agents.Return_Data_By_Number(agent_idx);

		
		Remove_Agent(agent);	

    } 
	
	
	
	
	my_agents.Empty();
	squad_ID = 0xefefefef;
	already_committed = 0xefefefef;
	resend = 0xefefefef;
	squad_type = DEFAULT_SQUAD;
}













void Squad::Compute_Location()
{































}
















double Squad::Compute_Utility
(
	ArmyGoal *goal_obj
)
{
	
	int i;
    Agent *agent=NULL;				
	double utility;					
	double summed_utility = 0.0;	
	double avg_utility;				
	

	
	if (my_agents.count <= 0) 
		return BAD_UTILITY;

	sint32 found_agents = 0;
	
	for (i = 0; i < my_agents.count; i++)
	{
		
		agent = (Agent *) my_agents.Return_Data_By_Number( i );

		
		utility = agent->EstimateUtility(ai, goal_obj); 

		
		
		if (utility > BAD_UTILITY + 0.5)
		{
			summed_utility += utility;
			found_agents++;
		}

	} 

	
	if (found_agents > 0)
	{
		
		avg_utility = summed_utility / found_agents;
	}
	else
		
		return BAD_UTILITY;

	
	return avg_utility;
}














uint32 Squad::Still_Can_Act()
{
	
	int i;
    Agent *agent=NULL;				
	CityAgent *the_city;			
	ArmyAgent *the_army;			
	uint32 count;					
	

	
	if (my_agents.count <= 0) return 0;

	
	count = my_agents.count;

	
	for (i = 0; i < my_agents.count; i++)
	{
		
		agent = (Agent *) my_agents.Return_Data_By_Number( i );

		
		
		switch (agent->GetType())
		{
		case AGENT_TYPE_CITY:
			
			the_city = (CityAgent *) agent;

			
			if (the_city->GetQueueLen() >= 1)
				count--;

			break;
		case AGENT_TYPE_ARMY:
			
			the_army = (ArmyAgent *) agent;

			
			
			if ((the_army->GetMovePoints() < MINIMUM_MOVEMENT_POINTS_TO_MOVE) ||
				(the_army->IsWaitState()))
				count--;

			break;
		default:
			Assert(false);
		} 


	} 

	return count;
}













bool Squad::Contains_A_City()
{
	
	int i;
    Agent *agent=NULL;				
	

	
	if (my_agents.count <= 0) return false;

	
	for (i = 0; i < my_agents.count; i++)
	{
		
		agent = (Agent *) my_agents.Return_Data_By_Number( i );

		
		if (agent->GetType() == AGENT_TYPE_CITY)
			return true;

	} 

	
	return false;
}













void Squad::Display_Composition(AiMain *ai)
{
	
	int i;
    Agent *agent=NULL;				




	

#ifdef LOG_SQUADS
	
	if (my_agents.count <= 0)
	{
#ifdef DBGAILOG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG((wstr, "    EMPTY SQUAD!\n"));
#endif DBGAILOG
		return;
	} 

	
	for (i = 0; i < my_agents.count; i++)
	{
		
		agent = (Agent *) my_agents.Return_Data_By_Number( i );
		#ifdef DBGAILOG
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
		{
			
			MapPointData pos;
			MapPointData xy_pos;
			sint32 row, column;
			if (agent->GetType() == AGENT_TYPE_CITY)
			{
				CityAgent *city = (CityAgent *)agent;
				city->GetPos(pos);
				city->GetXYPos(ai,xy_pos);
			}
			else
			{
				ArmyAgent *army = (ArmyAgent *)agent;
				army->GetPos(ai, pos);
				army->GetXYPos(ai,xy_pos);
			}
			ai->m_planner->the_strategic_map->XY_to_MapgridRC(xy_pos.x, xy_pos.y, 
				row, column, 
				ai->m_planner->the_strategic_map->threat_map_resolution);

			AILOG((wstr, "    #%d  @ %x (%d,%d) [%d, %d]", 
				i, 
				(int) agent,
				pos.x,
				pos.y,
				row,
				column));
		}
		#endif DBGAILOG
		
		
		agent->Log_Type(ai);
		
	} 
#endif
}













void Squad::Display_Squad_Type(AiMain *ai)
{
#ifdef DBGAILOG
	switch (squad_type)
		{
		case DEFAULT_SQUAD:
			AILOG((wstr, "DEFAULT_SQUAD"));
			break;
		case CITY_SQUAD:
			AILOG((wstr, "CITY_SQUAD"));
			break;
		case EXPLORE_SQUAD:
			AILOG((wstr, "EXPLORE_SQUAD"));
			break;
		case MILITARY_SQUAD:
			AILOG((wstr, "MILITARY_SQUAD"));
			break;
		case DEFENSE_SQUAD:
			AILOG((wstr, "DEFENSE_SQUAD"));
			break;
		case PROTECT_SQUAD:
			AILOG((wstr, "PROTECT_SQUAD"));
			break;
		case SEIGE_SQUAD:
			AILOG((wstr, "SEIGE_SQUAD"));
			break;
		case BOMBARD_SQUAD:
			AILOG((wstr, "BOMBARD_SQUAD"));
			break;
		case SLAVER_SQUAD:
			AILOG((wstr, "SLAVER_SQUAD"));
			break;
		case GOD_SQUAD:
			AILOG((wstr, "GOD_SQUAD_SQUAD"));
			break;
		case ESPIONAGE_SQUAD:
			AILOG((wstr, "EXPIONAGE_SQUAD"));
			break;
		case DEFUSE_MINES_SQUAD:
			AILOG((wstr, "DEFUSE_MINES_SQUAD"));
			break;
		case BONUS_FOOD_SQUAD:
			AILOG((wstr, "BONUS_FOOD_SQUAD"));
			break;
		case ENVIRONMENTALIST_SQUAD:
			AILOG((wstr, "ENVIRONMENTALIST_SQUAD"));
			break;
		case MEDIA_SQUAD:
			AILOG((wstr, "MEDIA_SQUAD"));
			break;
		case DIPLOMATIC_SQUAD:
			AILOG((wstr, "DIPLOMATIC_SQUAD"));
			break;
		case BUSINESS_SQUAD:
			AILOG((wstr, "BUSINESS_SQUAD"));
			break;
		case LAW_SQUAD:
			AILOG((wstr, "LAW_SQUAD"));
			break;
		case ABOLITIONIST_SQUAD:
			AILOG((wstr, "ABOLITIONIST_SQUAD"));
			break;
		case SETTLER_SQUAD:
			AILOG((wstr, "SETTLER_SQUAD"));
			break;
		case DESTROY_CITY_SQUAD:
			AILOG((wstr, "DESTROY_CITY_SQUAD"));
			break;
		case TRANSPORT_HACK_SQUAD:
			AILOG((wstr, "TRANSPORT_HACK_SQUAD"));
			break;
		case WORMHOLE_PROBE_SQUAD:
			AILOG((wstr, "WORMHOLE_PROBE_SQUAD"));
			break;
		case MAX_SQUAD_CLASSES:
			AILOG((wstr, "MAX_SQUAD_CLASSES"));
			break;
		default:
			AILOG((wstr, "unknown"));
			break;
		}
	AILOG((wstr, "\n"));
#endif DBGAILOG

}












void Squad::Recompute_Squad_Class
(
	AiMain *ai
)
{
    sint32 agent_idx, agent_num; 
    Agent *an_agent=NULL;
	squad_type = DEFAULT_SQUAD;

	agent_num = my_agents.count; 
    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
         an_agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);
		 Reclassify_Squad(ai, an_agent);
	}
}













void Squad::Reclassify_Squad
(
	AiMain *ai,
	Agent * add_me
)
{
	
	SQUAD_CLASS this_agent_squad_type;
	SQUAD_CLASS old_squad_type;
	

	
	this_agent_squad_type = add_me->Classify_Agents_Squad_Type(ai);

	
	
	
	if (ai->m_planner->the_scheduler == NULL)
		return;

	
	
	
	
	
	old_squad_type = squad_type;
	if (((old_squad_type is DEFAULT_SQUAD) ||
		(old_squad_type is CITY_SQUAD) ||
		(this_agent_squad_type isnt CITY_SQUAD)) &&
		(old_squad_type isnt this_agent_squad_type))
	{
		
		
		
		if (this_agent_squad_type > old_squad_type) 
			squad_type = this_agent_squad_type;
		else
			squad_type = old_squad_type;

		

		ai->m_planner->the_scheduler->Reclassify_Squad(this, old_squad_type);

	} 
}





	
	






void Squad::Add_Agent
(
	AiMain *ai, 
	Agent * add_me
)

{
	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY > LOG_DETAILS)
	{
		AILOG(( wstr,  "Squad::Add_Agent:\n\tArmyCount() = %d (before).\n", 
				m_squad_strength.GetArmyCount()))
		AILOG(( wstr,  "\tGetUnitCount() = %d (before).\n", 
				m_squad_strength.GetUnitCount()))
		AILOG(( wstr,  "\taddme->GetSquadStrength().GetArmyCount() = %d\n", 
				add_me->GetSquadStrength()->GetArmyCount()))
		AILOG(( wstr,  "\taddme->GetSquadStrength().GetUnitCount() = %d.\n", 
				add_me->GetSquadStrength()->GetUnitCount()))
	}
	#endif
	#endif DBGAILOG

	

    
	add_me->squads_list_cell_ptr = my_agents.Add_Data((data_ptr) add_me);
	add_me->the_squad = this;

    
    m_squad_strength.Add(add_me->GetSquadStrength()); 

	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY > LOG_DETAILS)
	{
		AILOG(( wstr,  "\tArmyCount() = %d (after).\n", 
				m_squad_strength.GetArmyCount()))
		AILOG(( wstr,  "\tGetUnitCount() = %d (after).\n", 
				m_squad_strength.GetUnitCount()))
	}
	#endif
	#endif DBGAILOG



	
    BOOL will_be_transported = FALSE; 
    MapPointData target_pos, agent_pos; 
    double move_point_cost; 
    double build_time; 
    ArmyAgent *the_army=NULL; 
    CityAgent *the_city=NULL; 
    sint32 dist; 
    if (my_strategic_goal) { 
         *(my_strategic_goal->m_current_attacking_strength) = m_squad_strength;
    

         
         
        ((ArmyGoal*)my_strategic_goal)->GetPos(target_pos); 

         if (add_me->GetType() == AGENT_TYPE_CITY) { 
             the_city = (CityAgent*) add_me; 
             the_city->GetPos(agent_pos); 
             dist = sint32(ai->m_map->WrappedDistance(agent_pos, target_pos)); 
    
         } else { 
             the_army = (ArmyAgent*) add_me; 

             if (the_army->BidDistance(ai, will_be_transported, target_pos, move_point_cost, 
                 dist, build_time) < 0.0) { 
                 dist = 100; 
             }
         }

         if (my_strategic_goal->m_farthest_agent_to_target_dist < dist) { 
             my_strategic_goal->m_farthest_agent_to_target_dist = (sint16) dist; 
         }
    }

	
	m_moveType = 0x0;

	
	Reclassify_Squad(ai, add_me);
}














void Squad::Remove_Agent
(
	Agent * remove_me
)
{
	
	Assert(remove_me->the_squad);
	if (!remove_me->the_squad)
		return;

	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY > LOG_DETAILS)
	{
		AILOG(( wstr,  "Squad::Remove_Agent:\n\tArmyCount() = %d (before).\n", 
				m_squad_strength.GetArmyCount()))
		AILOG(( wstr,  "\tGetUnitCount() = %d (before).\n", 
				m_squad_strength.GetUnitCount()))
		AILOG(( wstr,  "\tremoveme->GetSquadStrength()->GetArmyCount() = %d\n", 
				remove_me->GetSquadStrength()->GetArmyCount()))
		AILOG(( wstr,  "\tremoveme->GetSquadStrength()->GetUnitCount() = %d.\n", 
				remove_me->GetSquadStrength()->GetUnitCount()))

	}
	#endif
	#endif DBGAILOG

    
    m_squad_strength.Subtract(remove_me->GetSquadStrength()); 

	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY > LOG_DETAILS)
	{
		AILOG(( wstr,  "\tArmyCount() = %d (after).\n", 
				m_squad_strength.GetArmyCount()))
		AILOG(( wstr,  "\tGetUnitCount() = %d (after).\n", 
				m_squad_strength.GetUnitCount()))
	}
	#endif
	#endif DBGAILOG

	
	remove_me->the_squad = NULL;

	
    if (my_strategic_goal) { 
         *(my_strategic_goal->m_current_attacking_strength) = m_squad_strength;
    }

	
	my_agents.Remove_Cell( remove_me->squads_list_cell_ptr );
	remove_me->squads_list_cell_ptr = NULL;

	
	m_moveType = 0x0;

}













void Squad::Remove_Dead_Agents()
{
    sint32 agent_idx, agent_num; 
    Agent *an_agent=NULL;

    agent_num = my_agents.count; 

    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) { 
         an_agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);

         if (an_agent->GetType() == AGENT_TYPE_ARMY) { 
                                                        
             switch(((ArmyAgent*)an_agent)->GetState()) { 
             case AGENT_STATE_MARKED_DEAD:
            






                 Remove_Agent(an_agent); 
             default: 
                 break; 
             }
         }
    } 
}













void Squad::Take_All_Agents
(
	AiMain *ai,							
	Squad *donor_squad					
)
{
    sint32 agent_idx, agent_num; 
    Agent *donor_agent=NULL;

    agent_num = donor_squad->my_agents.count; 

    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
        donor_agent = 
			(Agent*)donor_squad->my_agents.Return_Data_By_Number(agent_idx);

		
		donor_squad->Remove_Agent(donor_agent);	

		
		Add_Agent(ai, donor_agent); 

    } 

	
	donor_squad->Recompute_Squad_Class(ai); 
}













BOOL Squad::Recruit_Best_Strength
(
	AiMain *ai,							
	Goal *the_goal,						
	Squad *donor_squad,					
	bool looking_for_transports			
)
{
    sint32 max_donor_idx, donor_idx, donor_num; 
    double max_str_score, str_score; 
    Agent *donor_agent=NULL; 
	bool anybody_found_ever = false;
	Agent *best_donor_agent=NULL;		
										

    Assert(donor_squad); 

	
	
    if (((!donor_squad->auxiliary_candidate) and (looking_for_transports)) ||
       (donor_squad->already_committed) ||
       (donor_squad->my_agents.count < 1) ) { 
         return FALSE;
    }


	
	
	
    Assert(this != donor_squad); 

	
	if (this == donor_squad)
		return FALSE;
	


	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr,  "Squad::Recruit_Best_Strength: Recruiting from squad %d into squad %d.\n", 
				donor_squad->squad_ID, squad_ID));
	}
	#endif
	#endif DBGAILOG

    
	int excess;
    do { 
        max_str_score = 0.0; 
        max_donor_idx = -1;
		best_donor_agent=NULL;
        donor_num = donor_squad->my_agents.count; 
		str_score = -1.0;

        
        for (donor_idx=0; donor_idx < donor_num; donor_idx++) 
		{ 

            donor_agent = (Agent*)donor_squad->my_agents.Return_Data_By_Number(donor_idx);

            
            the_goal->EstimateRecruitmentStrength(ai, donor_agent, str_score); 

			
			if (str_score > 0)
			{
				
				if (max_str_score < str_score) { 
					max_str_score = str_score; 
					max_donor_idx = donor_idx; 
					best_donor_agent=donor_agent;
				} 

			} 

        } 

        if (-1 == max_donor_idx) { 
			#ifdef DBGAILOG
			#ifdef LOG_SQUADS
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				if (!anybody_found_ever)
					{
						
						AILOG(( wstr,  "    No Matching Donor Agents Found...\n "));
					}
			}
			#endif
			#endif DBGAILOG

        } else {

			
			anybody_found_ever = true;

			
			donor_squad->Remove_Agent(best_donor_agent);	

			
			donor_squad->Recompute_Squad_Class(ai); 

			Add_Agent(ai, best_donor_agent); 

			#ifdef DBGAILOG
			#ifdef LOG_SQUADS
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
			{
				
				AILOG(( wstr,  "    RECRUIT: "));
				best_donor_agent->Log_Type(ai);
				AILOG(( wstr,  "\n"));
			}
			#endif
			#endif DBGAILOG

        }

    } 
	
	while (!the_goal->Is_Satisfied(&excess) && 
		   max_donor_idx != -1);  

	
	
	if (anybody_found_ever)
	{
		
		ignore_goals.Append_Whole_List(&(donor_squad->ignore_goals));

	} 

	BOOL is_enough = the_goal->Is_Satisfied(&excess);

	#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_DETAILS && anybody_found_ever)
	{
		
		char str[100];
		AILOG(( wstr,  "                 Needed       Current\n"));
		sprintf(str, "    Army Count   %5d        %5d\n", 
			the_goal->m_current_needed_strength->GetArmyCount(),
			the_goal->m_current_attacking_strength->GetArmyCount());	
		AILOG(( wstr,  str));
		
		sprintf(str, "    Unit Count   %5d        %5d\n", 
			the_goal->m_current_needed_strength->GetUnitCount(),
			the_goal->m_current_attacking_strength->GetUnitCount());	
		AILOG(( wstr,  str));
		
		sprintf(str, "    Attack       %6.0f     %6.0f\n", 
			the_goal->m_current_needed_strength->GetAttack(),
			the_goal->m_current_attacking_strength->GetAttack());	
		AILOG(( wstr,  str));
		
		sprintf(str, "    Defense      %6.0f     %6.0f\n", 
			the_goal->m_current_needed_strength->GetDefense(),
			the_goal->m_current_attacking_strength->GetDefense());	
		AILOG(( wstr,  str));

		sprintf(str, "    Eff. Ranged  %6d     %6d\n", 
			the_goal->m_current_needed_strength->EffectiveRangedStrength(),
			the_goal->m_current_attacking_strength->EffectiveRangedStrength());	
		AILOG(( wstr,  str));
		
		sprintf(str, "    Transport    %5d        %5d\n\n", 
			the_goal->m_current_needed_strength->GetTransport(),
			the_goal->m_current_attacking_strength->GetTransport());	
		AILOG(( wstr,  str));

		if (ai->AI_VERBOSITY > LOG_DETAILS)
		{
			
			AILOG((wstr, "the_team->GetUsableTransport() = %d\n", the_team->GetUsableTransport()));

			
			AILOG((wstr, "the_team->GetUsableDefense() = %d\n", the_team->GetUsableDefense()));

			
			AILOG((wstr, "the_team->GetUsableAttack() = %d\n", the_team->GetUsableAttack()));
		}
		if (is_enough)
		{
			AILOG(( wstr,  "    Squad strength is enough...\n\n"));
		}
		else 
		{
			AILOG(( wstr,  "    Squad strength still not enough...\n"));
		}

	}
	#endif
	#endif DBGAILOG

    return is_enough;
}






void Squad::DumpGoal(AiMain *ai, const GOAL_TYPE goal_type, const double matching_value)
{
    char text[255]; 

    switch (goal_type) { 
    case GOAL_TYPE_NULL:
        sprintf (text, "bad!?!?"); 
        break; 
    case GOAL_TYPE_DEFENSE:
        sprintf (text, "defense"); 
        break; 



    case GOAL_TYPE_ATTACK: 
        sprintf (text, "attack"); 
        break; 
    case GOAL_TYPE_SETTLE: 
        sprintf (text, "settle"); 
        break; 
    case GOAL_TYPE_EXPLORE: 
        sprintf (text, "explore"); 
        break; 
    case GOAL_TYPE_CONSTRUCT_BUILDING:
        sprintf (text, "BLG"); 
        break; 
    case GOAL_TYPE_CONSTRUCT_FREIGHT:
        sprintf (text, "FREIGHT"); 
        break; 
    case GOAL_TYPE_CONSTRUCT_WONDER:
        sprintf (text, "WONDER"); 
        break; 
    case GOAL_TYPE_TRANSPORT:    
        sprintf (text, "transport"); 
        break; 
    case GOAL_TYPE_ENSLAVE:
        sprintf (text, "enslave"); 
        break; 
    case GOAL_TYPE_BOMBARD:
        sprintf (text, "bombard"); 
        break; 
    case GOAL_TYPE_BUILD_SUPPLEMENTAL:
        sprintf (text, "Bsup"); 
        break; 
    case GOAL_TYPE_BUILD_LIST:
        sprintf (text, "Blist"); 
        break; 
    case GOAL_TYPE_PERIMETER:
        sprintf (text, "perimeter"); 
        break; 
    case GOAL_TYPE_CHOKEPOINT:
        sprintf (text, "chokepoint"); 
        break; 
    case GOAL_TYPE_PATROL:
        sprintf (text, "patrol"); 
        break; 
    case GOAL_TYPE_RUSTLE:
        sprintf (text, "rustle"); 
        break; 
    case GOAL_TYPE_EXPEL:
        sprintf (text, "expel"); 
        break; 
    case GOAL_TYPE_SALLY:
        sprintf (text, "sally"); 
        break; 
    case GOAL_TYPE_ATTACK_REGION:
        sprintf (text, "Attack Region"); 
        break; 
	case GOAL_TYPE_SEIGE: 
	case GOAL_TYPE_CONVERT_CITY:
    case GOAL_TYPE_BIOTERROR_CITY:
    case GOAL_TYPE_NANOATTACK_CITY:
	case GOAL_TYPE_HARASS_CITY:

	case GOAL_TYPE_PILLAGE:
	case GOAL_TYPE_PIRATE:
	case GOAL_TYPE_COUNTER_STEALTH:
	case GOAL_TYPE_BONUS_FOOD:
	case GOAL_TYPE_GOODY_HUT:
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
	case GOAL_TYPE_NUKE_CITY:
	case GOAL_TYPE_WANDER:
	case GOAL_TYPE_REFORM_CITY:
	case GOAL_TYPE_SUE_FRANCHISE:
	case GOAL_TYPE_PROBE_WORMHOLE:
	case GOAL_TYPE_RECOVER_PROBE:
        my_strategic_goal->GetDebugName(text); 
        break; 
    case GOAL_TYPE_RETREAT:
        sprintf (text, "retreat"); 
        break; 
    default:
        INSANE(UNKNOWN_GOAL_TYPE); 
        sprintf (text, "*unknown*"); 
        break;
    } 


    char valstr[100]; 
    sprintf (valstr, " %3.1f S: %d", matching_value, squad_ID); 

    strcat (text, valstr); 
    
    sint32 i; 
    Agent *the_agent; 
    for (i=0; i<my_agents.count;  i++){
		
		the_agent = (Agent *)
			my_agents.Return_Data_By_Number( i );
		
        the_agent->DumpStr(ai, text); 
	} 
}



BOOL Squad::Ok_To_Rally(AiMain *ai)
{
    sint32 agent_idx, agent_num; 
	Agent *agent = NULL;
	ArmyAgent *army_agent = NULL;
	sint32 dest_cont;
	BOOL dest_is_land;
	MapPointData pos;
	sint32 num_in_water = 0;
	sint32 num_at_dest = 0;

    agent_num = my_agents.count; 

	ArmyGoal *army_goal = (ArmyGoal*) my_strategic_goal;
	army_goal->GetPos(pos);	

	
	

	if(pos.z == SPACE_Z) return TRUE;

	BOOL r = ai->m_continents->GetContinent(ai, pos, dest_cont, dest_is_land);
	Assert(r);

	
	if (!dest_is_land)
		return TRUE;

	
    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
		
        agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);
		if (agent->GetType() != AGENT_TYPE_ARMY)
			continue;
		
		army_agent = (ArmyAgent*) agent;
		if ((army_agent->GetNUnits() > 0) &&
			(army_agent->GetState() != AGENT_STATE_MARKED_DEAD))
		{
			
			
			if ((army_agent->GetContinent() != dest_cont)
				&& (army_agent->GetMyIsLand()))
				return FALSE;

			
			if (!army_agent->GetMyIsLand())
				num_in_water++;
			else if (army_agent->GetContinent() == dest_cont)
				num_at_dest++;
		}
	}
	
	
	return (agent_num == (num_in_water + num_at_dest));
}














int Squad::Choose_Rally_Point
(
	AiMain *ai,							
	MapPointData target,				
										
	MapPointData &rally_pos,			
	int &current_agent					
)
{
	
    Agent *agent=NULL;					
	ArmyAgent * the_army;				
	CityAgent * the_city;				
	MapPointData agent_pos;				
	MapPointData closest_pos;			
	double distance;					
	double best_distance;				
	int need_to_rally = false;			
	int units_in_segment = 0;			
	int agents_in_segment = 0;			
	int units_in_agent;					
	int found_one_distance = false;		
	int num_got_there = 0;				
	int rally_successful = false;		
	

#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "Squad::Choose_Rally_Point: Entering\n"));
	#endif
#endif DBGAILOG
	
	
	if (my_agents.count <= 0)
	{
#ifdef DBGAILOG
		if (ai->AI_VERBOSITY >= LOG_HEADERS)
			AILOG((wstr, "Squad WARNING: Rally point set for EMPTY SQUAD!\n"));
#endif DBGAILOG
		return false;
	} 

	
	

    MapPointData *size = ai->m_map->GetSize(); 

	
	
	best_distance = 2*(size->y + 2*size->x);
	
	
	do
	{
		
		agent = (Agent *) my_agents.Return_Data_By_Number( current_agent );

		
		current_agent--;

		
		switch(agent->GetType())
		{
		
		case AGENT_TYPE_CITY:
			
			the_city = (CityAgent *) agent;

			
			
			
			the_city->GetPos(agent_pos);

			
			units_in_agent = 1;
			break;

		
		case AGENT_TYPE_ARMY:
			
			the_army = (ArmyAgent *) agent;

			
			
			
			the_army->GetPos(ai, agent_pos);

			
			units_in_agent = the_army->GetNUnits();

			break;

		
		default:
			Assert(false);
			break;
		} 

		
		
		if (units_in_segment + units_in_agent > k_MAX_ARMY_SIZE)
		{



			
			continue;

		} 

		
		
		distance = ai->m_map->WrappedDistance(agent_pos, target);

		
		if (distance < best_distance)
		{
			
			if (found_one_distance)
				need_to_rally = true;

			
			best_distance = distance;

			
			closest_pos.x = agent_pos.x;
			closest_pos.y = agent_pos.y;
			closest_pos.z = agent_pos.z;

			
			found_one_distance = true;

		} 

		
		units_in_segment += units_in_agent;

		
		agents_in_segment++;

		
		if (not (
				(closest_pos.x is agent_pos.x) and
				(closest_pos.y is agent_pos.y) 
				
				))
		{
			
			need_to_rally = true;

		} 
		
		else
		{
			
			num_got_there += units_in_agent;

		} 

	} while ((units_in_segment < k_MAX_ARMY_SIZE) and (current_agent >= 0));

	
	rally_pos = closest_pos;
















	
	if (agents_in_segment > 1)
		need_to_rally = true;

#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "Squad::Choose_Rally_Point: Exiting\n"));
	#endif
#endif DBGAILOG
	
	
	return need_to_rally;
}












int Squad::GetUnitCount() const
{
	return my_agents.count;
}












Agent * Squad::GetUnitByNumber(const int &i)
{
	Assert(i >= 0);
	Assert(i < my_agents.count);
	return (Agent *) my_agents.Return_Data_By_Number( i );
}
















Goal_Result Squad::Send_Troops_To_Rally
(
	AiMain *ai,							
	int last_agent,						
	int first_agent,					
	MapPointData &rally_pos,			
	ArmyGoal *the_goal					
)
{
	
	sint32 army_idx;					
	double utility;						
    Agent *agent=NULL;					
	ArmyAgent * the_army;				
	AGENT_STATE army_state;				
	MapPointData agent_pos;				
	
    
    BOOL revealed_unexplored = FALSE; 
    BOOL did_move = FALSE; 
    BOOL redo_explore = FALSE; 
    BOOL use_marked=FALSE;
    BOOL revealed_forign_units = FALSE; 
	sint32 first_one_there = -1;		
										
	BOOL is_unknown_id;					
	Goal_Result result = GOAL_COMPLETE; 
	

#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "Squad::Send_Troops_To_Rally: Entering\n"));
	#endif
#endif DBGAILOG
	
	
	

	
	utility = the_goal->Compute_Raw_Priority(ai);

	
	army_idx = last_agent;

	
	
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    
    

	
	while (army_idx > first_agent)
	{
Assert(++finite_loop < 100000); 
		
		agent = (Agent *) my_agents.Return_Data_By_Number( army_idx );

		
		switch(agent->GetType())
		{
		
		case AGENT_TYPE_CITY:
			
			break;

		
		case AGENT_TYPE_ARMY:
			
			the_army = (ArmyAgent *) agent;

			
			
			
			the_army->GetPos(ai, agent_pos);

			
			army_state = the_army->GetState();

			
			if (not (
					(rally_pos.x is agent_pos.x) and
					(rally_pos.y is agent_pos.y) and
					(rally_pos.z is agent_pos.z) 
					))
			{
				
				the_goal->GotoTaskSolution(ai, 
					army_idx, the_army, 
					0, NULL, 
					rally_pos, SUB_TASK_RALLYING);
				
				
				if (ai->m_i_am_dead || the_goal->Get_Totally_Complete(ai)) 
				{ 
					
					return GOAL_COMPLETE; 
				} 



























			} 
			
			
			
			
			the_army->GetPos(ai, agent_pos);

			
			if ((
				(rally_pos.x is agent_pos.x) and
				(rally_pos.y is agent_pos.y) and
				(rally_pos.z is agent_pos.z) 
				))
			{
				
				if (first_one_there isnt -1)
				{
					
					Remove_Agent(the_army);

					
					
					
					ai->m_player->ArmyGroup(
						agent->GetID().GetVal(), 
						first_one_there, 
						&is_unknown_id
						);

					
					Assert(is_unknown_id == FALSE);

				} 
				
				else
				{
					first_one_there = agent->GetID().GetVal();
				} 

			} 
			else
				result = GOAL_IN_PROGRESS; 

			break;

		
		default:
			Assert(FALSE);
			break;
		} 

		
		army_idx--;

	} 

#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "Squad::Send_Troops_To_Rally: Exiting\n"));
	#endif
#endif DBGAILOG

    return result;
	
}


















Goal_Result Squad::Rally_Troops
(
	AiMain* ai,
	MapPointData target,				
										
	ArmyGoal *the_goal,					
    BOOL &any_group_needed_to_rally		
)
{
	
	int current_agent = 0;				
	int last_agent;						
	int agent_count;					
	int actually_rally;					
	
	MapPointData rally_point;			
	
	

#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "Squad::Rally_Troops: Entering\n"));
	#endif
#endif DBGAILOG

	
	
	

	
	agent_count = my_agents.count;

	
	if (agent_count <= 1) return GOAL_INAPPROPRIATE;

	
	any_group_needed_to_rally = false;

	
	
	current_agent = agent_count - 1;
    Goal_Result result;

	
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif
	while (current_agent >= 0)
	{
Assert(++finite_loop < 100000); 
		
		last_agent = current_agent;

		
		
		
		
		actually_rally = Choose_Rally_Point(ai, target, 
											rally_point, current_agent);

		
		any_group_needed_to_rally |= actually_rally;

		
		if (actually_rally)
		{
			
			
			result = Send_Troops_To_Rally(ai, last_agent, current_agent, rally_point,
								 the_goal);

            if (GOAL_IN_PROGRESS != result) { 
                return result; 
            } 
		} 

	} 

#ifdef DBGAILOG
	#ifdef LOG_SQUADS
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "Squad::Rally_Troops: Exiting\n"));
	#endif
#endif DBGAILOG
	
	
	return GOAL_IN_PROGRESS;
}














sint32 Squad::GetUsableTransport(AiMain *ai)
{
    sint32 agent_idx, agent_num; 
    Agent *agent = NULL;
	ArmyAgent *army_agent = NULL;
	sint32 total_empty = 0;
	sint32 empty, full;

    agent_num = my_agents.count; 

    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
         agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);

         if (agent->GetType() == AGENT_TYPE_ARMY) 
		 { 
 			 army_agent = (ArmyAgent *) agent;

			 if (army_agent->GetMovePoints() >= MINIMUM_MOVEMENT_POINTS_TO_MOVE &&
				 !army_agent->IsWaitState())
			 {
				 if (army_agent->GetNumCargo(ai, full, empty))
					 total_empty += empty;
			 }
		 }	
	}
	return total_empty;
}









double Squad::GetUsableAttack(AiMain *ai)
{
    sint32 agent_idx, agent_num; 
    Agent *agent = NULL;
	ArmyAgent *army_agent = NULL;
	double total_attack = 0.0;

    agent_num = my_agents.count; 

    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
         agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);

         if (agent->GetType() == AGENT_TYPE_ARMY) 
		 { 
 			 army_agent = (ArmyAgent *) agent;

			 if (army_agent->GetMovePoints() >= MINIMUM_MOVEMENT_POINTS_TO_MOVE &&
				 !army_agent->IsWaitState())
				 total_attack += army_agent->GetAttackStrength();
		 }	
	}
	return total_attack;
}










double Squad::GetUsableDefense(AiMain *ai)
{
    sint32 agent_idx, agent_num; 
    Agent *agent = NULL;
	ArmyAgent *army_agent = NULL;
	double total_defense = 0.0;

    agent_num = my_agents.count; 

    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
         agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);

         if (agent->GetType() == AGENT_TYPE_ARMY) 
		 { 
 			 army_agent = (ArmyAgent *) agent;

			 if (army_agent->GetMovePoints() >= MINIMUM_MOVEMENT_POINTS_TO_MOVE &&
				 !army_agent->IsWaitState())
				 total_defense += army_agent->GetDefenseStrength();
		 }	
	}
	return total_defense;
}














void Squad::GroupWithArmy(AiMain *ai, ArmyAgent *army_agent)
{
    sint32 agent_idx, agent_num; 
    Agent *agent = NULL;
	ArmyAgent *grouped_agent = NULL;
	BOOL is_unknown_id;
	MapPointData army_pos;
	MapPointData grouped_pos;
    agent_num = my_agents.count; 
	army_agent->GetPos(ai,army_pos);

	
	Assert(army_agent->GetState() != AGENT_STATE_MARKED_DEAD);
	if (army_agent->GetState() == AGENT_STATE_MARKED_DEAD)
		return;

	
    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
         agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);

		 
         if (agent->GetType() == AGENT_TYPE_ARMY) 
		 { 
 			 grouped_agent = (ArmyAgent *) agent;

			 
			 Assert(grouped_agent->GetState() != AGENT_STATE_MARKED_DEAD);
			 if (grouped_agent->GetState() == AGENT_STATE_MARKED_DEAD)
				 continue; 

			 
			 grouped_agent->GetPos(ai,grouped_pos);
			 if (grouped_agent->GetID().GetVal() != army_agent->GetID().GetVal() &&
				 grouped_pos.x == army_pos.x &&
				 grouped_pos.y == army_pos.y &&
				 grouped_pos.z == army_pos.z)
				 {

					 
					 Remove_Agent(grouped_agent);

					

					 
					 ai->m_player->ArmyGroup(grouped_agent->GetID().GetVal(), 
											 army_agent->GetID().GetVal(),
											 &is_unknown_id);
					 Assert(!is_unknown_id);
					 if (!is_unknown_id)
						 {
							 
							 grouped_agent->m_has_been_executed = TRUE;
							 grouped_agent->SetState(AGENT_STATE_MARKED_DEAD);
						 }
				 }
		 }	
	}
}














void Squad::UngroupAllArmies(AiMain *ai)
{
    sint32 agent_idx, agent_num; 
	Agent *agent = NULL;
	ArmyAgent *army_agent = NULL;
	BOOL is_unknown_id;

    agent_num = my_agents.count; 

	
    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
		
        agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);
		if (agent->GetType() != AGENT_TYPE_ARMY)
			continue;
		
		army_agent = (ArmyAgent*) agent;
		if ((army_agent->GetNUnits() > 1) &&
			(army_agent->GetState() != AGENT_STATE_MARKED_DEAD))
		{
			Assert(army_agent->GetID().GetVal() != 0);
			
			ai->m_player->ArmyUngroup(army_agent->GetID().GetVal(),
				&is_unknown_id);
			Assert(!is_unknown_id);	
		}	
	 }
}

double Squad::ScoreTransportUtility(AiMain *ai, ArmyAgent *transport)
{
	double max_score = BAD_UTILITY;
	double tmp_score;
    sint32 agent_idx, agent_num; 
	Agent *agent = NULL;
	ArmyAgent *army_agent = NULL;

    agent_num = my_agents.count; 

	
    for (agent_idx = agent_num-1; 0 <= agent_idx; agent_idx--) 
	{ 
		
        agent = (Agent*)my_agents.Return_Data_By_Number(agent_idx);
		if (agent->GetType() != AGENT_TYPE_ARMY)
			continue;
		
		army_agent = (ArmyAgent*) agent;
		tmp_score = army_agent->EstimateTransportUtility(ai, transport);
		if (tmp_score > max_score)
			max_score = tmp_score;
	}
	return max_score;
}

sint32 Squad::GetTransportUnitsCount(AiMain *ai)
{
	
	sint32 my_transports = 0;
	Agent *agent;
	for (sint32 i = 0; i < my_agents.count; i++)
	{
		
		agent = (Agent *) my_agents.Return_Data_By_Number( i );
		if (agent->GetType() != AGENT_TYPE_CITY)
			my_transports += ((ArmyAgent*) agent)->GetTransports(ai);
	}
	return my_transports;
}


uint32 Squad::GetMoveTypes(AiMain *ai)
{
	
	if (m_moveType)
		return m_moveType;

	
	Agent *agent;
	ArmyAgent *army_agent;

	
	for (sint32 i = 0; i < my_agents.count; i++)
	{
		agent = (Agent *) my_agents.Return_Data_By_Number( i );
		if (agent->GetType() != AGENT_TYPE_CITY)
			{
				army_agent = (ArmyAgent *)agent;
				
				m_moveType |= army_agent->GetMoveIntersection(ai);
			}
	}
	return m_moveType;
}
