//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : the Goal motherclass
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Consider a goal with max armies engaged as satisfied - the limitation of 
//   army size : we cannot form a group with more armies than the max (can 
//   disturb the goals with RallyFirst() - Calvitix
// - Changes the const attribute for Compute_Matching_Value (Raw_Priority will 
//   be changed on wounded case) - Calvitix
// - Linux support modifications + cleanup.
//
//----------------------------------------------------------------------------


#include "c3.h"

#include "Goal.h"
using namespace std;


const Utility Goal::BAD_UTILITY = -99999999; 
const Utility Goal::MAX_UTILITY = 99999999; 

#include "squad_Strength.h"
#include "agent.h"
#include "ArmyPool.h"
#include "debugassert.h"

#include "gstypes.h"
#include "gfx_options.h"
#include "ctpagent.h"

#ifdef _DEBUG_SCHEDULER
#include "ctpagent.h"
#include "ArmyData.h"
#include "ctpgoal.h"
#endif //_DEBUG_SCHEDULER













Goal::Goal()
:
    m_goal_type                     (GOAL_TYPE_NULL),
    m_raw_priority                  (BAD_UTILITY),
    m_removal_time                  (DONT_REMOVE),
    m_is_invalid                    (false),
    m_execute_incrementally         (false),
    m_current_needed_strength       (),
    m_current_attacking_strength    (),
    m_match_references              (),
    m_agents                        (),
    m_playerId                      (-1)
    // m_pos    not filled?
{
}


Goal::Goal(const Goal &goal)
{
	*this = goal;
}



Goal::~Goal()
{
	
}


Goal& Goal::operator= (const Goal &goal)
{
	
	m_goal_type = goal.m_goal_type;

    m_playerId = goal.m_playerId; 

	
	m_raw_priority = goal.m_raw_priority;

	
	m_removal_time = goal.m_removal_time;

	
	m_is_invalid = goal.m_is_invalid;

	
	m_execute_incrementally = goal.m_execute_incrementally;

	
    

	
    m_current_needed_strength = goal.m_current_needed_strength;

	
	m_current_attacking_strength = goal.m_current_attacking_strength;

	
	m_match_references = goal.m_match_references;

 	
	m_agents = goal.m_agents;

    return *this;
}


bool Goal::operator< (const Goal &goal) const
{
    return (m_raw_priority < goal.m_raw_priority);
}


GOAL_TYPE Goal::Get_Goal_Type() const
{
	return m_goal_type;
}


void Goal::Set_Player_Index(const PLAYER_INDEX & playerId)
{
	m_playerId = playerId;
}


PLAYER_INDEX Goal::Get_Player_Index() const
{
	return m_playerId;
}




bool Goal::Is_Satisfied() const
{
	if (m_agents.size() == 0)
		return false;

	
// limitation of army size : cannot form a group with more
// armies than the max (without that limitation, it can disturb the goals with RallyFirst() - Calvitix
    if (m_current_attacking_strength.Get_Agent_Count() == k_MAX_ARMY_SIZE)
        return true;

	if (m_current_needed_strength > m_current_attacking_strength)
		return false;

	return true;
}




bool Goal::Is_Goal_Undercommitted() const
{
	return (!Is_Satisfied() && m_agents.size() > 0);
}


sint16 Goal::Get_Agent_Count() const
{
    return m_agents.size();
}


bool Goal::Is_Single_Squad() const
{
	if (m_agents.size() <= 1)
		return true;
	
	std::list<Plan_List::iterator>::const_iterator match_iter =
		m_match_references.begin();
	bool found = false;
	Squad_ptr first_squad_ptr = NULL;
	Squad_ptr tmp_squad_ptr;
	Agent_List::const_iterator agent_iter;
	for (agent_iter = m_agents.begin(); agent_iter != m_agents.end(); agent_iter++) 
	{
		
		match_iter = m_match_references.begin();
		while (match_iter != m_match_references.end())
		{
			
			if ((*match_iter)->Agent_Committed(*agent_iter))
			{
				
				tmp_squad_ptr = (*match_iter)->Get_Squad();

				
				if (first_squad_ptr != NULL && tmp_squad_ptr != first_squad_ptr)
					return false;

				
				first_squad_ptr = tmp_squad_ptr;

				
				break;
			}
			
			match_iter++;
		}	
		
		Assert(first_squad_ptr != NULL);
	}
	return true;
}




bool Goal::Commit_Agent(const Agent_ptr & agent, Agent_List::const_iterator & agent_list_iter)
{
	
	if ( Satisfied_By( agent->Compute_Squad_Strength() ) )
	{
		
		m_current_attacking_strength.Add_Agent_Strength(agent);

		
		agent_list_iter = m_agents.insert(m_agents.end(), agent);
		
#ifdef _DEBUG_SCHEDULER
		Assert(m_match_references.size() > 0);
		CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) agent;
		Assert(ctpagent_ptr->Get_Army().GetData()->m_theGoal == NULL);
		ctpagent_ptr->Get_Army()->m_theGoal = (CTPGoal_ptr) this;

		
		Assert(m_current_attacking_strength.Get_Agent_Count() >= m_agents.size());
		if (m_current_attacking_strength.Get_Agent_Count() < m_agents.size())
		{
			
			Assert(0);
		}
#endif // _DEBUG_SCHEDULER

		return true;
	}

	
	agent_list_iter = m_agents.end();

	return false;
}


const Agent_List & Goal::Get_Agent_List() const
{
	return m_agents;
}



Agent_ptr Goal::Rollback_Agent(Agent_List::const_iterator & agent_iter)
{
	Assert(agent_iter != m_agents.end());

	Agent_ptr agent_ptr = (*agent_iter);

#ifdef _DEBUG_SCHEDULER
	CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) agent_ptr;
	if (g_theArmyPool->IsValid(ctpagent_ptr->Get_Army()))
		Assert(ctpagent_ptr->Get_Army()->m_theGoal == this);
#endif // _DEBUG_SCHEDULER

	

	

	
	m_current_attacking_strength.Remove_Agent_Strength(agent_ptr);

    
	Agent_List::iterator next_agent_iter;
	for (next_agent_iter = m_agents.begin();
	next_agent_iter != m_agents.end();
	next_agent_iter++)
	{
		if (*agent_iter == *next_agent_iter)
		{
			
			Assert(agent_iter == next_agent_iter);
			break;
		}
	}

	
	Assert(next_agent_iter != m_agents.end());
	
	
	next_agent_iter = m_agents.erase(next_agent_iter);
	agent_iter = m_agents.end();

	
	
#ifdef _DEBUG_SCHEDULER
	if (g_theArmyPool->IsValid(ctpagent_ptr->Get_Army()))
		ctpagent_ptr->Get_Army()->m_theGoal = NULL;

	Assert(m_current_attacking_strength.Get_Agent_Count() >= m_agents.size());
	if (m_current_attacking_strength.Get_Agent_Count() < m_agents.size())
	{
		
		Assert(0);
	}
#endif // _DEBUG_SCHEDULER

	return agent_ptr;
}




bool Goal::Is_Execute_Incrementally() const
{
	return m_execute_incrementally;
}



void Goal::Compute_Needed_Troop_Flow()
{

    #ifdef TEST_DRIVER
    m_current_needed_strength.Set_Attack(300.0);
    m_current_needed_strength.Set_Defense(100.0);
    return;
    #endif

	
	Assert(false);
}



Utility Goal::Compute_Matching_Value( const Agent_ptr agent ) const
{
    #ifdef TEST_DRIVER
    return 100;
    #endif
	
	Assert(false);
	return 0;
}


Utility Goal::Get_Raw_Priority() const
{
	return m_raw_priority;
}


GOAL_RESULT Goal::Execute_Task()
{
	

    
    if (Is_Satisfied() || Is_Execute_Incrementally())
    {
        return GOAL_IN_PROGRESS;	
    }

    return GOAL_IN_PROGRESS;	
}


bool Goal::Get_Totally_Complete() const
{
	return false;
}


void Goal::Set_Invalid(const bool &is_invalid)
{
	m_is_invalid = is_invalid;
}


bool Goal::Get_Invalid() const
{
	return m_is_invalid;
}


bool Goal::Get_Removal_Time() const
{
	return false;
} 


void Goal::Set_Removal_Time(const REMOVAL_TIME & removal_time)
{
    m_removal_time = removal_time;
}


bool Goal::Can_Be_Executed() const  
{
	

	bool can_be_executed = false;
	Agent_List::const_iterator agent_iter;
	for (agent_iter = m_agents.begin(); agent_iter != m_agents.end();agent_iter++) 
	{
		can_be_executed |= (*agent_iter)->Get_Can_Be_Executed();
	}

    return can_be_executed;
}


void Goal::Set_Can_Be_Executed(const bool & can_be_executed)
{
	
	Agent_List::iterator agent_iter;
	for (agent_iter = m_agents.begin(); agent_iter != m_agents.end();agent_iter++) 
	{
		(*agent_iter)->Set_Can_Be_Executed(can_be_executed);
	}

}






bool Goal::Validate() const
{

#ifdef _DEBUG

	
	Agent_List::const_iterator agent_iter;
	for (agent_iter = m_agents.begin(); agent_iter != m_agents.end();agent_iter++) 
	{
		uint32 *first_bytes =
			(uint32 *)&(*(*agent_iter));

		if ( *first_bytes == 0xdddddddd)
		{
			bool ARMY_DELETED_WITHOUT_TELLING_GOAL = false;
			Assert(ARMY_DELETED_WITHOUT_TELLING_GOAL);
		}

		
		std::list<Plan_List::iterator>::const_iterator match_iter =
			m_match_references.begin();
		
		Assert((*agent_iter)->Get_Is_Used() == true);

		
		match_iter = m_match_references.begin();
		while (match_iter != m_match_references.end())
		{
			
			if ((*match_iter)->Agent_Committed(*agent_iter))
			{

#ifdef _DEBUG_SCHEDULER
		
				
				Squad_ptr tmp_squad_ptr = (*match_iter)->Get_Squad();
				Goal_ptr tmp_goal_ptr = (*match_iter)->Get_Goal();
				CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr)(*agent_iter);

				if (g_theArmyPool->IsValid(ctpagent_ptr->Get_Army()))
				{
					Assert(ctpagent_ptr->Get_Army()->m_theAgent == ctpagent_ptr);
					Assert(ctpagent_ptr->Get_Army()->m_theGoal == tmp_goal_ptr);
				}
				else
				{
					
					Assert(0);
				}
#endif _DEBUG_SCHEDULER

				
				break;
			}
			
			match_iter++;
		}
		if (match_iter == m_match_references.end())
		{

#ifdef _DEBUG_SCHEDULER
			CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr)(*agent_iter);
			Assert(ctpagent_ptr->Get_Army().AccessData()->m_theAgent == ctpagent_ptr);
			Assert(ctpagent_ptr->Get_Army().AccessData()->m_theGoal != NULL);
#endif // _DEBUG_SCHEDULER

			
			Assert(0);
		}
	}

#endif // _DEBUG
    
    return true;
} 


void Goal::Log_Debug_Info(const int & log) const 
{

}







void Goal::Add_Match_Reference(const Plan_List::iterator &plan_iter)
{
	m_match_references.push_back(plan_iter);

#ifdef _DEBUG_SCHEDULER
		
		Validate();
#endif // _DEBUG_SCHEDULER

}


void Goal::Remove_Match_Reference(const Plan_List::iterator &plan_iter)
{
	m_match_references.remove(plan_iter);

#ifdef _DEBUG_SCHEDULER
		Assert(m_match_references.size() > 0 || m_agents.size()  == 0);

		
		Validate();
#endif // _DEBUG_SCHEDULER

}


list<Plan_List::iterator> & Goal::Get_Match_References()
{
	return m_match_references;
}


void Goal::Set_Type(const GOAL_TYPE & type)
{
	m_goal_type = type;
}



void Goal::Set_Raw_Priority(const Utility & priority)
{
    m_raw_priority = priority;
}


bool Goal::Get_Is_Appropriate() const
{
    return m_match_references.size() > 0;
}


bool Goal::Satisfied_By(const Squad_Strength & army_strength) const
{
	Squad_Strength needed_strength = m_current_needed_strength;
	needed_strength -= m_current_attacking_strength;

	
	if ( army_strength.Get_Transport() > 0)
	{
		
		if ( needed_strength.Get_Transport() > 0)
		  return true;

		

		
		
		
		
		
	}

	
	//Check if the army has too much units to fit in one tile - Calvitix
	if (m_current_attacking_strength.Get_Agent_Count() + 
		army_strength.Get_Agent_Count() > k_MAX_ARMY_SIZE)
	return false;
	
	

	
	if ((needed_strength.Get_Agent_Count() > 0) && 
		(army_strength.Get_Agent_Count() > 0)) 
		return true;

	
	if ((needed_strength.Get_Attack() > 0) &&
 		(army_strength.Get_Attack() > 0))
		return true;

	
	if ((needed_strength.Get_Defense() > 0) && 
		(army_strength.Get_Defense() > 0) )
		return true;

	
	if ((needed_strength.Get_Defenders() > 0) &&
		(army_strength.Get_Defenders() > 0))
		return true;

	
	if ((needed_strength.Get_Ranged() > 0) &&
		(army_strength.Get_Ranged() > 0))
		return true;

	
	if ((needed_strength.Get_Ranged_Units() > 0) && 
		(army_strength.Get_Ranged_Units() > 0))
		return true;

	return false;
}


bool Goal::Needs_Transport() const
{
	Squad_Strength needed_strength = m_current_needed_strength;
	needed_strength -= m_current_attacking_strength;
	
	return needed_strength.Get_Transport() > 0;
}



const Squad_Strength Goal::Get_Strength_Needed() const
{
	Squad_Strength needed_strength = m_current_needed_strength;
	needed_strength -= m_current_attacking_strength;

	return needed_strength;
}
