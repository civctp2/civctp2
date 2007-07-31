


#include "c3.h"
#include "globals.h"


#include "IC3GameState.h"
#include "IC3UnitDB.h"
#include "AiMain.h"
#include "IC3Player.h"
#include "IC3World.h"


#include "BSet.h"
#include "ForeignAgent.h"


#include "Goal.h"
#include "GlEnslave.h"
#include "GoalBombard.h"
#include "GoalRustle.h"
#include "GoalExpel.h"
#include "GoalSally.h"

#include "Squad_Strength.h"


#include "CivArchive.h"
#include "MapPoint.h"
#include "Foreigner.h"

ForeignAgent::ForeignAgent()

{ 
    Init();
} 


ForeignAgent::ForeignAgent(PLAYER_INDEX player_index, AiMain *init_ai, BSetID &id)
{
    Init(); 

	ai = init_ai;
	m_player_index = player_index;
    m_id = id; 
} 

ForeignAgent::~ForeignAgent()
{
    NDELETE (m_pos);    
    NDELETE (m_XYpos);    

	GoalEnslave *goal_obj;

	
	if (m_has_enslave_goal)
	{
		
		goal_obj = (GoalEnslave *) 
			ai->m_planner->Return_Goal_By_ID(enslave_me_goal_ID);

		
		Assert(goal_obj != NULL);

        goal_obj->Set_Totally_Complete(TRUE); 

		
		if (goal_obj->attacking_squad != NULL)
		{
			
			goal_obj->attacking_squad->Detach_Goal();

		} 

		
		ai->m_planner->the_scheduler->Remove_Goal_And_Add_To_Delete_List(goal_obj);

	} 

	
    if (attack_me_goal != NULL)
    {
        Goal *goal_ptr = attack_me_goal; 

        goal_ptr->Set_Totally_Complete(TRUE); 

        goal_ptr->Set_Invalid(); 

		









		
		ai->m_planner->the_scheduler->Remove_Goal_And_Add_To_Delete_List(goal_ptr);
    }

	
	RemoveExpelGoal(ai, m_expelMeGoalID);
	RemoveSallyGoal(ai, m_sallyMeGoalID);
	RemoveRustleGoal(ai, m_rustleMeGoalID);
	RemoveBombardGoal(ai, m_bombardMeGoalID);

#ifdef _DEBUG
    
    m_top_unit_type = 0xcdcdcdcd; 
    m_unit_num = 0xcdcdcdcd;
    m_defense_strength = double(0xcdcdcdcdcdcdcdcd); 
    m_attack_strength = double(0xcdcdcdcdcdcdcdcd); 
    m_target_value = double(0xcdcdcdcdcdcdcdcd); 
    m_has_peeked = 0xcdcdcdcd; 
	ai = (AiMain*)0xcdcdcdcd;
    m_pos = (MapPointData*)0xcdcdcdcd; 
    m_has_enslave_goal = 0xcdcdcdcd; 
	attack_me_goal = (Goal *) 0xcdcdcdcd;	
	enslave_me_goal_ID = 0xcdcdcdcd; 
#endif
} 


ForeignAgent::ForeignAgent(IC3CivArchive *archive)
{
    Init();

    Serialize(archive); 
}

void ForeignAgent::Init()
{
    m_agent_type = AGENT_TYPE_FOREIGN_ARMY;
    m_has_enslave_goal = FALSE;
    m_top_unit_type = 0; 
    m_defense_strength = 0.0; 
    m_attack_strength = 0.0; 
    m_target_value = 0.0; 
    m_has_peeked = FALSE; 
	attack_me_goal = NULL;
	ai = NULL;
    m_unit_num = 0;
    m_id = 0; 
    m_player_index = -1; 

    m_pos = new MapPointData(); 
    m_pos->x = -1;
    m_pos->y = -1;
    m_pos->z = -1;

    m_XYpos = new MapPointData(); 
    m_XYpos->x = -1; 
    m_XYpos->y = -1; 
    m_XYpos->z = -1; 

    m_has_enslave_goal = FALSE;
	enslave_me_goal_ID = UNASSIGNED_ID;
	m_bombardMeGoalID = UNASSIGNED_ID;
	m_rustleMeGoalID = UNASSIGNED_ID;
	m_expelMeGoalID = UNASSIGNED_ID;
	m_sallyMeGoalID = UNASSIGNED_ID;

    m_lastSeen = -1;

}

void ForeignAgent::Serialize(IC3CivArchive *archive)
{

    CivArchive *a = (CivArchive *) archive; 

    Agent::Serialize(*a); 

    if (a->IsStoring()) { 

        a->PutSINT32(m_player_index); 

        a->PutSINT32(m_top_unit_type); 
        (*a) << m_defense_strength; 
        (*a) << m_attack_strength; 
        (*a) << m_target_value; 
        a->PutSINT32(m_has_peeked); 
        
        (*a) << m_pos->x; 
        (*a) << m_pos->y; 
        (*a) << m_pos->z; 
        (*a) << m_unit_num; 

        (*a) << m_XYpos->x; 
        (*a) << m_XYpos->y; 
        (*a) << m_XYpos->z; 

        a->PutSINT32(m_has_enslave_goal); 
		a->PutSINT32(enslave_me_goal_ID);
		a->PutSINT32(m_bombardMeGoalID);
		a->PutSINT32(m_rustleMeGoalID);
		a->PutSINT32(m_expelMeGoalID);
		a->PutSINT32(m_sallyMeGoalID);

        a->PutSINT32(m_lastSeen);

    } else { 
        m_player_index = PLAYER_INDEX(a->GetSINT32()); 

        m_top_unit_type = a->GetSINT32(); 
        (*a) >> m_defense_strength; 
        (*a) >> m_attack_strength; 
        (*a) >> m_target_value; 
        m_has_peeked = a->GetSINT32(); 

        (*a) >> m_pos->x; 
        (*a) >> m_pos->y; 
        (*a) >> m_pos->z; 

        (*a) >> m_unit_num; 

        (*a) >> m_XYpos->x; 
        (*a) >> m_XYpos->y; 
        (*a) >> m_XYpos->z; 

        m_has_enslave_goal = a->GetSINT32(); 
		enslave_me_goal_ID = a->GetSINT32();
		m_bombardMeGoalID = a->GetSINT32();
		m_rustleMeGoalID = a->GetSINT32();
		m_expelMeGoalID = a->GetSINT32();
		m_sallyMeGoalID = a->GetSINT32();

        m_lastSeen = a->GetSINT32(); 
    }
}

void ForeignAgent::Hook(AiMain *init_ai)
{
    ai = init_ai; 
}


void ForeignAgent::SetCurPos (const MapPointData &p)

{ 
    *m_pos = p; 
}

void ForeignAgent::GetPos(MapPointData &my_pos)
{
    my_pos = *m_pos;
}

void ForeignAgent::SetUnitNum(const sint32 unit_num)
{
    m_unit_num = unit_num; 
}

void ForeignAgent::SetCurXYPos (const MapPointData &p)

{ 
    *m_XYpos = p; 
}

void ForeignAgent::GetXYPos(MapPointData &my_pos)
{
    BOOL is_unknown_id;

	ai->m_player->GetArmyXYPos(m_player_index, m_id.GetVal(), &is_unknown_id, m_XYpos); 
    my_pos = *m_XYpos;

	
	Assert(is_unknown_id == FALSE);
}

void ForeignAgent::CalcDefense()
{

    double d, hp, fp; 
    if (m_has_peeked == FALSE) { 
        d = ai->m_unitDB->GetDefense(m_top_unit_type); 
        hp = ai->m_unitDB->GetHP(m_top_unit_type);
		
		fp = ai->m_unitDB->GetFirepower(m_top_unit_type);
        m_defense_strength = d * hp * fp;
		
		
		m_defense_strength *= GetUnitNum();
    } else { 
        m_defense_strength = 0; 
    } 
}

void ForeignAgent::CalcThreat()
{
    double a, hp, fp; 
    if (m_has_peeked == FALSE) { 
        a = ai->m_unitDB->GetAttack(m_top_unit_type); 
        hp = ai->m_unitDB->GetHP(m_top_unit_type);
		
		fp = ai->m_unitDB->GetFirepower(m_top_unit_type);
        m_attack_strength = a * hp * fp;

		
		m_attack_strength *= GetUnitNum();
    } else { 
        m_attack_strength = 0; 
    } 
}

void ForeignAgent::CalcTarget()

{ 
    double cost = ai->m_unitDB->GetProductionCost(m_top_unit_type); 
    double maxcost = 4500; 
    double target_product_cost_scale = 20.0; 

    m_target_value =  target_product_cost_scale * cost / maxcost; 

    if (ai->m_unitDB->IsSettler(m_top_unit_type)) { 
        m_target_value += 10.0; 
    } 

	
	m_target_value *= GetUnitNum();
        
} 
void ForeignAgent::SetTopUnit(sint32 tp)
{
    m_top_unit_type = tp;

    CalcDefense(); 
    CalcThreat(); 
    CalcTarget(); 
}

double ForeignAgent::Get_Threat_Value()
{
	
	return (FOREIGNAGENT_DEFAULT_THREAT); 
}


double ForeignAgent::GetMaxMovePoints()
{
    return ai->m_unitDB->UnitTypeMovementPoints(m_top_unit_type); 
}

double ForeignAgent::GetTargetValue() const
{ 
    return m_target_value; 
} 

double ForeignAgent::PretestEnslave()
{
    if (m_has_enslave_goal) { 
        return BAD_UTILITY; 
    } else { 
        return 1.0; 
    } 
}


void ForeignAgent::SetHasEnslaveGoal(const BOOL b)
{ 
    m_has_enslave_goal = b; 
} 

void ForeignAgent::SetEnslaveGoalID(sint32 goal_ID)
{ 
    enslave_me_goal_ID = goal_ID; 
} 


void ForeignAgent::RemoveEnslaveGoal()
{
	GoalEnslave *goal_obj;

	
	if (m_has_enslave_goal)
	{
		
		goal_obj = (GoalEnslave *) 
			ai->m_planner->Return_Goal_By_ID(enslave_me_goal_ID);

		
		Assert(goal_obj != NULL);

        goal_obj->Set_Totally_Complete(TRUE); 

		
		if (goal_obj->attacking_squad != NULL)
		{
			
			goal_obj->attacking_squad->Detach_Goal();

		} 

	} 

    m_has_enslave_goal = FALSE; 
	enslave_me_goal_ID = UNASSIGNED_ID;
}


sint32 ForeignAgent::GetUnitNum()
{
    return m_unit_num;
}

sint32 ForeignAgent::GetTopUnitType()
{
    return m_top_unit_type;
}

BOOL ForeignAgent::IsLoneSettler(AiMain *ai)
{
    return (1 == GetUnitNum()) && ai->m_unitDB->IsSettler(m_top_unit_type); 
} 

BOOL ForeignAgent::IsStealthy(AiMain *ai)
{
    return (GetUnitNum() > 0) && ai->m_unitDB->IsStealthy(m_top_unit_type); 
} 

BOOL ForeignAgent::IsSpaceAttack(AiMain *ai)
{
    return (GetUnitNum() > 0) && ai->m_unitDB->IsSpaceAttack(m_top_unit_type); 
} 

BOOL ForeignAgent::IsSpaceLand(AiMain *ai)
{
    return (GetUnitNum() > 0) && ai->m_unitDB->CanSpaceLand(m_top_unit_type);
} 







double ForeignAgent::Estimate_Army_Strength(AiMain *ai)
{
	
	sint32 types_in_army[k_MAX_ARMY_SIZE];	
	sint32 i;							
	sint32 num_in_army;					
	double total_strength = 0;			
	double my_strength;					
	double type_hitpoints;				
	double attack_value;				
	double defense_value;				
	double firepower;					
	

	
	ai->m_world->Get_Army_Types(m_pos, types_in_army, num_in_army);

	
	for (i = 0; i < num_in_army; i++)
	{
		
        attack_value = ai->m_unitDB->GetAttack(types_in_army[i]); 
        defense_value = ai->m_unitDB->GetDefense(types_in_army[i]); 
        type_hitpoints = ai->m_unitDB->GetHP(types_in_army[i]);
		
		firepower = ai->m_unitDB->GetFirepower(types_in_army[i]);
		
		
		my_strength = (attack_value + defense_value) * type_hitpoints * firepower;

		
		total_strength += my_strength;

	} 

	
	if (total_strength < 1.0) total_strength = 1.0;

	
	return total_strength;
}













double ForeignAgent::Estimate_Army_Attack_Strength(AiMain *ai)
{
	
	sint32 types_in_army[k_MAX_ARMY_SIZE];	
	sint32 i;							
	sint32 num_in_army;					
	double total_strength = 0;			
	double my_strength;					
	double type_hitpoints;				
	double attack_value;				
	double firepower;					
	

	
	ai->m_world->Get_Army_Types(m_pos, types_in_army, num_in_army);

	
	for (i = 0; i < num_in_army; i++)
	{
		
        attack_value = ai->m_unitDB->GetAttack(types_in_army[i]); 
        type_hitpoints = ai->m_unitDB->GetHP(types_in_army[i]);
		
		
        firepower = ai->m_unitDB->GetFirepower(types_in_army[i]);

		
		
		my_strength = (attack_value * firepower) * type_hitpoints;

		
		total_strength += my_strength;

	} 

	
	if (total_strength < 1.0) total_strength = 1.0;

	
	return total_strength;
}













void ForeignAgent::Get_Army_Attack_And_Defense_Strengths
(
	AiMain* ai, 
	float& attack, 
	float& defend
)
{
	
	sint32 types_in_army[k_MAX_ARMY_SIZE];	
	sint32 i;							
	sint32 num_in_army;					
	float total_strength = 0;			
	float type_hitpoints;				
	float attack_value;				
	float firepower;					
	float my_defense;					
	Squad_Strength my_strength;			
	

	
	ai->m_world->Get_Army_Types(m_pos, types_in_army, num_in_army);

	
	for (i = 0; i < num_in_army; i++)
	{
		
        attack_value = float(ai->m_unitDB->GetAttack(types_in_army[i])); 
        type_hitpoints = float(ai->m_unitDB->GetHP(types_in_army[i]));
		firepower = float(ai->m_unitDB->GetFirepower(types_in_army[i]));
		my_defense = float(ai->m_unitDB->GetDefense(types_in_army[i]));

		
		my_strength.AddAttack(attack_value, firepower, type_hitpoints); 
		my_strength.AddDefense(my_defense, firepower, type_hitpoints); 

	} 

    attack = my_strength.GetAttack(); 
    defend = my_strength.GetDefense(); 

}













void ForeignAgent::SetupSpecialGoals(AiMain *ai)
{
	
	

	
	if (!ai->m_foreigner[m_player_index])
		return;

	if (m_bombardMeGoalID == UNASSIGNED_ID) {

        if (ai->m_foreigner[m_player_index]->IsAtHotWar()) { 
		    GoalBombard *bombardMeGoal = new GoalBombard(ai, this, *m_pos);

		    
		    ai->m_planner->Add_Goal(bombardMeGoal);

		    
		    m_bombardMeGoalID = bombardMeGoal->GetID();
        }
	}

	
	












	
	if (m_expelMeGoalID == UNASSIGNED_ID && GoalExpel::PretestConstruct(ai, this))
	{
		GoalExpel *expelMeGoal = new GoalExpel(ai, this, *m_pos);

		
		ai->m_planner->Add_Goal(expelMeGoal);

		
		m_expelMeGoalID = expelMeGoal->GetID();
	}

}













void ForeignAgent::SetupSallyGoal(AiMain *ai)
{
	
	if (m_sallyMeGoalID == UNASSIGNED_ID)
	{
		GoalSally *sallyMeGoal = new GoalSally(ai, this, *m_pos);

		
		ai->m_planner->Add_Goal(sallyMeGoal);

		
		m_sallyMeGoalID = sallyMeGoal->GetID();
	}
}













void ForeignAgent::RemoveBombardGoal(AiMain *ai, const sint32 & goalID)
{
	Assert(m_bombardMeGoalID == goalID);

	
	if (m_bombardMeGoalID != UNASSIGNED_ID)
	{
		
		Goal *bombardMeGoal = ai->m_planner->Return_Goal_By_ID(m_bombardMeGoalID);

        
		if (bombardMeGoal)
		{
			
			if (bombardMeGoal->attacking_squad != NULL)
			{
				bombardMeGoal->Set_Totally_Complete(TRUE); 
				
				
				bombardMeGoal->attacking_squad->Detach_Goal();
				
			} 
			
			
			ai->m_planner->the_scheduler->
				Remove_Goal_And_Add_To_Delete_List(bombardMeGoal);
		}
		m_bombardMeGoalID = UNASSIGNED_ID;
	} 
}













void ForeignAgent::RemoveRustleGoal(AiMain *ai, const sint32 & goalID)
{

	if (m_rustleMeGoalID != UNASSIGNED_ID)
	{
		Assert(m_rustleMeGoalID == goalID);

		
		Goal *rustleMeGoal = ai->m_planner->Return_Goal_By_ID(m_rustleMeGoalID);

        
		if (rustleMeGoal)
		{
			
			if (rustleMeGoal->attacking_squad != NULL)
			{
				rustleMeGoal->Set_Totally_Complete(TRUE); 
				
				
				rustleMeGoal->attacking_squad->Detach_Goal();
				
			} 
			
			
			ai->m_planner->the_scheduler->
				Remove_Goal_And_Add_To_Delete_List(rustleMeGoal);
		}
		m_rustleMeGoalID = UNASSIGNED_ID;
	} 

}













void ForeignAgent::RemoveExpelGoal(AiMain *ai, const sint32 & goalID)
{
	if (m_expelMeGoalID != UNASSIGNED_ID)
	{
		Assert(m_expelMeGoalID == goalID);

		
		Goal *expelMeGoal = ai->m_planner->Return_Goal_By_ID(m_expelMeGoalID);

        
		if (expelMeGoal)
		{
			
			if (expelMeGoal->attacking_squad != NULL)
			{
				expelMeGoal->Set_Totally_Complete(TRUE); 
				
				
				expelMeGoal->attacking_squad->Detach_Goal();
				
			} 
			
			
			ai->m_planner->the_scheduler->
				Remove_Goal_And_Add_To_Delete_List(expelMeGoal);
		}
		m_expelMeGoalID = UNASSIGNED_ID;
	} 

}













void ForeignAgent::RemoveSallyGoal(AiMain *ai, const sint32 & goalID)
{
	if (m_sallyMeGoalID != UNASSIGNED_ID)
	{
		Assert(m_sallyMeGoalID == goalID);

		
		Goal *sallyMeGoal = ai->m_planner->Return_Goal_By_ID(m_sallyMeGoalID);

        
		if (sallyMeGoal)
		{
			
			if (sallyMeGoal->attacking_squad != NULL)
			{
				sallyMeGoal->Set_Totally_Complete(TRUE); 
				
				
				sallyMeGoal->attacking_squad->Detach_Goal();
				
			} 
			
			
			ai->m_planner->the_scheduler->
				Remove_Goal_And_Add_To_Delete_List(sallyMeGoal);
		}
		m_sallyMeGoalID = UNASSIGNED_ID;
	} 

}







ARMY_TARGET_TYPE ForeignAgent::GetTargetType()
{
    return ARMY_TARGET_TYPE_MILITARY;
}

sint32 ForeignAgent::GetLastSeen()
{
    return m_lastSeen;
}

void ForeignAgent::SetLastSeen(const sint32 &round)
{
    m_lastSeen = round;
}
