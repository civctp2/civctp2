















































































































































































#include "c3.h"
#include "dynarr.h"
#include "civarchive.h"
#include "IMapPointData.h"
#include "bset.h"


#include "ic3GameState.h"
#include "ic3world.h"
#include "ic3UnitDB.h"

#include "aimain.h"
#include "AiMap.h"


#include "unitflow.h"
#include "common.h"
#include "linked_list.h"
#include "semi_dynamic_array.h"
#include "sorted_array.h"
#include "aip.h"
#include "planner.h"


#include "dr_debug.h"
#include "matrix.h"
#include "grassfire.h"
#include "strategic_map.h"


#include "FzOut.h"
extern double fz_min_force_matching_percent;
extern double fz_max_force_matching_percent;


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "ForeignAgent.h"
#include "Foreigner.h"


#include "GlAttck.h"

#include "ailog.h"
#include "squad_strength.h"

#define LOG_GOAL_ATTACK true


GoalAttack::GoalAttack()
{
    Init(); 
}


GoalAttack::GoalAttack ( AiMain *ai,
						ForeignAgent * init_the_bastard,	
						const MapPointData &pos
						) : ArmyGoal(ai, 1000000, pos)
{
	
    Init(); 
	
	

	

	removal_time = REMOVE_WHEN_SORTED;

	
    Assert(init_the_bastard); 
	the_bastard = init_the_bastard;
    the_bastard_id = new BSetID(the_bastard->GetID().GetVal()); 
    m_foreign_player = the_bastard->GetPlayerIndex(); 

	
	
 	
	

	
	rallying = true;

	
	
 	
	

	m_value = 2000; 

	return; 
} 	

GoalAttack::GoalAttack(
	AiMain *ai,
	CivArchive &archive
) {

    Init(); 
    Serialize(ai, archive); 
}

void GoalAttack::Init() 
{

    removal_time = REMOVE_WHEN_SORTED;
    
	what_goal = GOAL_TYPE_ATTACK;

	
	the_class = MILITARY_GOAL_CLASS;

    the_bastard = NULL; 
    the_bastard_id = NULL;
    m_foreign_player=-1; 
    rallying = -1;
}

GoalAttack::~GoalAttack()
{ 
	delete the_bastard_id; 
    the_bastard_id = NULL; 
} 

void GoalAttack::Serialize(AiMain *ai, CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 

    uint32 v;
    if (archive.IsStoring()) { 
        archive.PutSINT32(the_bastard_id->GetVal()); 

        Assert(0 <= m_foreign_player); 
        archive << m_foreign_player; 
        archive.PutSINT32(rallying); 
    } else { 
        v = archive.GetSINT32(); 
        the_bastard_id = new BSetID(v); 

        archive >> m_foreign_player; 
        Assert(0 <= m_foreign_player); 
        rallying = archive.GetSINT32(); 
    } 
}


void GoalAttack::HookUp(AiMain *ai)
{
    Assert(0 <= m_foreign_player); 
    Assert(m_foreign_player < k_MAX_PLAYERS); 

    Assert(ai->m_foreigner[m_foreign_player]); 

    BSet<ForeignAgent> *vu = ai->m_foreigner[m_foreign_player]->GetVisibleUnits();
    BSetID h_id; 
    ForeignAgent *him=NULL; 
    for (him = vu->First(h_id); vu->Last(); him =  vu->Next(h_id)) { 
        if (him->GetID() == *the_bastard_id) {  
            the_bastard = him; 
            Assert(NULL == him->attack_me_goal); 
            him->attack_me_goal = this; 
            return; 
        }
    }

    {
        BOOL COULD_NOT_FIND_ATTACK_GOAL_TARGET=0;
        Assert(COULD_NOT_FIND_ATTACK_GOAL_TARGET);
    }
}


BOOL GoalAttack::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}




GOAL_TYPE GoalAttack::GetType () const
{
	return GOAL_TYPE_ATTACK; 
}







void GoalAttack::Display_Goal_Type(AiMain* ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_ATTACK"));
#endif DBGAILOG
}













void GoalAttack::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
		AILOG((wstr, " AGENT %x Target: (%d, %d, %d)", the_bastard,
			m_pos->x, m_pos->y, m_pos->z));
#endif DBGAILOG
}













Goal_Result GoalAttack::Execute_Task(AiMain *ai, Plan *the_plan)
{
#ifdef DBGAILOG
	#ifdef LOG_GOAL_ATTACK
	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "GoalAttack::Execute_Task: Entering\n"));
	#endif
#endif DBGAILOG

        
    if (ai->m_i_am_dead) { 
        return GOAL_FAILED; 
    } else if (Get_Totally_Complete(ai)) {
		return GOAL_COMPLETE;
    } else if (NULL == attacking_squad){
		return GOAL_FAILED; 
	} 

	
	
	sint32 agent_count = attacking_squad->my_agents.count;

	
	rallying = (agent_count > 1);

	
    Goal_Result result;

	BOOL ok_to_rally = attacking_squad->Ok_To_Rally(ai);
    
	if (rallying && ok_to_rally)
	{
		
        MapPointData dest_pos; 
		the_bastard->GetPos(dest_pos); 

		
		result = attacking_squad->Rally_Troops(ai, dest_pos, this, rallying);

        if (GOAL_IN_PROGRESS != result &&
			GOAL_COMPLETE != result) { 
            return result;
        }

		if (GOAL_COMPLETE == result)
			rallying = FALSE;

	} 

	
	if (rallying && ok_to_rally)
	{
#ifdef DBGAILOG
		#ifdef LOG_GOAL_ATTACK
		if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			AILOG((wstr, "GoalAttack::still rallying-Exiting\n"));
		#endif
#endif DBGAILOG

		return GOAL_IN_PROGRESS;
	} 
	
	else if (ok_to_rally)
		{
#ifdef DBGAILOG
			#ifdef LOG_GOAL_ATTACK
			if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
				AILOG((wstr, "GoalAttack::rallying complete-Exiting\n"));
			#endif
#endif DBGAILOG
			; 
		}
		
    return ArmyGoal::Execute_Task(ai,the_plan); 
}



























































































































































void GoalAttack::Set_Invalid() 
{
	invalid_goal = true;

	
	if (the_bastard != NULL)
	{
		
        Assert(this == the_bastard->attack_me_goal);
		the_bastard->attack_me_goal = NULL;

		
		the_bastard = NULL;

	} 
		
	Goal::Set_Invalid();
}













void GoalAttack::Compute_Needed_Troop_Flow(AiMain *ai)
{

	



    float attack; 
    float defense; 
    sint32 defend_unit_count; 
    float ranged; 
    sint32 ranged_unit_count;

    BOOL ret = ComputeEnemyStrengthAt(ai, *m_pos, attack, defend_unit_count, defense, 
        ranged_unit_count, ranged);

    Assert(ret); 

	
	

	
	double match_ratio = 
		AddMinMaxRandomRange(ai, 
							 ai->m_planner->the_AIP.min_defense_matching_force_ratio,
							 fz_min_force_matching_percent,
							 fz_max_force_matching_percent);
	float  min_required_strength = float(attack * match_ratio);

	m_min_needed_strength->SetDefense(min_required_strength);
	m_current_needed_strength->SetDefense(min_required_strength);

	
	

	
	match_ratio = 
		AddMinMaxRandomRange(ai, 
							 ai->m_planner->the_AIP.min_attack_matching_force_ratio,
							 fz_min_force_matching_percent,
							 fz_max_force_matching_percent);
	
	min_required_strength = float(defense * match_ratio);
	m_min_needed_strength->SetAttack(min_required_strength);
	m_current_needed_strength->SetAttack(min_required_strength);

    m_min_needed_strength->SetUnitCount(1); 
    m_current_needed_strength->SetUnitCount(1);
    
    m_min_needed_strength->SetRangeUnitCount(ranged_unit_count); 
    m_current_needed_strength->SetRangeUnitCount(ranged_unit_count); 

    m_min_needed_strength->SetRangeStrength(ranged); 
    m_current_needed_strength->SetRangeStrength(ranged); 

}
















double GoalAttack::Compute_Raw_Priority(AiMain *ai)
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

	

	raw_priority = ai->m_planner->the_AIP.attack_troops_priority;
	raw_priority += ArmyGoal::Compute_Raw_Priority(ai,
		 1, 
		-1, 
		 1, 
		 1, 
		-1, 
		-1, 
		 1); 

	
	return raw_priority;
}






void GoalAttack::ArrivedAtTask(AiMain *ai, ArmyAgent *the_army)
{
    the_army->SetState(AGENT_STATE_LOOKING_FOR_WORK); 

}

double GoalAttack::GetDefenseStrength()
{
    return m_min_needed_strength->GetDefense(); 
}

double GoalAttack::GetAttackStrength()
{
    return m_min_needed_strength->GetAttack(); 
}



