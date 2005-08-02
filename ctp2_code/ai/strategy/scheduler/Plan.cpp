//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Plan object (association Goal <-> squad)
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
// _DEBUG
//  Set when generating the debug version.
//
// _DEBUG_SCHEDULER
//  Perform some extra consistency checks.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Changed the > and < operator  (to change the Sort method see Sort_Matches) - Calvitix
//   instead of only looking for the matching value, the new operator first
//   compare the Goal Raw-priority. If it is equal, then compare matching 
//   value. It has the advantage to give the possibility for high priority
//   goal that demands many units to be considered. 
//   For example :  3 units available, 
//                  3 Goals : 
//                  GOAL1  priority 50000, demands 2 units to be satisfied
//                  GOAL2  priority 40000, demands 1 unit to be satisfied
//                  GOAL3  priority 30000, demands 1 unit to be satisfied
//
//      the matches are : 
//
//                  GOAL1 <-> UNIT1   52000
//                  GOAL1 <-> UNIT2   49000
//                  GOAL1 <-> UNIT3   47000
//                  GOAL2 <-> UNIT1   54000
//                  GOAL2 <-> UNIT2   53000
//                  GOAL2 <-> UNIT3   52000
//                  GOAL3 <-> UNIT1   52000
//                  GOAL3 <-> UNIT2   53000
//                  GOAL3 <-> UNIT3   55000
//
//      Original Sort_matches and agent committing : 
//
//              GOAL3  ->  UNIT3   --> OK
//              GOAL2  ->  UNIT1   --> OK     
//              GOAL1  ->  FAILED to Satisfy, and it has the greatest priority !!!!
//
//
//      with new sort_matches (implied with thos operator changes)
//
//              GOAL1  -> UNIT1
//                     -> UNIT2    --> OK
//              GOAL2  -> UNIT3    --> OK
//              GOAL3  -> FAILED to Satisfy       
//
// - Added an method that determines if the matches can be reevaluated)
//   (ie the agent can be rollbacked and eventually be used for another goal)
//   depending on NoRollback flag.
// - Prevented crashes, and cleaned up the code somewhat.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Plan.h"

#include "profileai.h"
#include "debugassert.h"
#include "Squad.h"
#include "Goal.h"
#include "agent.h"
#include "ArmyData.h"
#include "ctpagent.h"
#include "ctpgoal.h"
#include "Scheduler.h"
#include "squad_Strength.h"
#include "ctpaidebug.h"
#include "ArmyPool.h"

#include "GoalRecord.h"
extern CTPDatabase<GoalRecord> *g_theGoalDB;

//----------------------------------------------------------------------------
//
// Name       : Plan::Plan()
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Plan::Plan()
:   m_matching_value    (Goal::BAD_UTILITY),
    m_the_squad         (NULL),
    m_the_goal          (NULL),
    m_matches           ()
{ ; }

//----------------------------------------------------------------------------
//
// Name       : Plan::Plan
//
// Description: Copy constructor
//
// Parameters : plan    : the object to copy the data from
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Plan::Plan(Plan const & plan)
{
	*this = plan;
}

//----------------------------------------------------------------------------
//
// Name       : Plan::operator =
//
// Description: Assignment operator
//
// Parameters : plan    : the object to copy the data from
//
// Globals    : -
//
// Returns    : Plan &  : reference to the created object
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Plan & Plan::operator = (Plan const & plan)
{
	m_the_goal          = NULL;     // temporary, until set in Set_Goal
    Set_Squad(plan.m_the_squad);    // sets m_the_squad, uses m_the_goal
	Set_Goal(plan.m_the_goal);	    // sets m_the_goal, uses m_the_squad
	m_matching_value    = plan.m_matching_value;
	m_matches           = plan.m_matches;
	
	return *this;
}

//----------------------------------------------------------------------------
//
// Name       : Plan::~Plan
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Plan::~Plan()
{ 
    Agent_Match_List().swap(m_matches);
} 

//----------------------------------------------------------------------------
//
// Name       : Plan::operator <
//              Plan::operator >
//
// Description: Compare operators
//
// Parameters : plan    : object to compare against
//
// Globals    : -
//
// Returns    : bool    : This object has lower/higher priority than the object 
//                        to compare against.
//
// Remark(s)  : The order is determined by the priority of the goal, with 
//              matching value as tie breaker.
//
//----------------------------------------------------------------------------
bool Plan::operator < (Plan const & plan) const
{
    Utility const   myPriority      = m_the_goal 
                                      ? m_the_goal->Get_Raw_Priority() 
                                      : Goal::BAD_UTILITY;
    Utility const   otherPriority   = plan.m_the_goal 
                                      ? plan.m_the_goal->Get_Raw_Priority() 
                                      : Goal::BAD_UTILITY;

    return (myPriority < otherPriority) ||
           ((myPriority == otherPriority) && 
            (m_matching_value < plan.m_matching_value)
           );
}

bool Plan::operator > (Plan const & plan) const
{
    return plan.operator < (*this);
}

//----------------------------------------------------------------------------
//
// Name       : Plan::operator ==
//              Plan::operator !=
//
// Description: Compare operators
//
// Parameters : plan    : object to compare against
//
// Globals    : -
//
// Returns    : bool    : This object is equal to/different from the object 
//                        to compare against.
//
// Remark(s)  : Equality is defined is having the same squad and the same 
//              goal.
//
//----------------------------------------------------------------------------
bool Plan::operator == (Plan const & plan) const
{
	return (m_the_squad == plan.m_the_squad) &&
		   (m_the_goal  == plan.m_the_goal);
}

bool Plan::operator!= (Plan const & plan) const
{
	return !(operator == (plan));
}


GOAL_TYPE Plan::Get_Goal_Type() const
{
	if (m_the_goal)	{
		return m_the_goal->Get_Goal_Type();
	}
	else {
		
		Assert(false);
		return GOAL_TYPE_NULL;
	}
}


bool Plan::Plan_Is_Needed_And_Valid() const
{ 
    return m_the_goal  &&
           m_the_squad && (m_the_squad->Get_Num_Agents() > 0);
}	


Utility Plan::Compute_Matching_Value()
{
	size_t  found_agents                = 0;
	Utility accumulated_matching_value  = 0;

	Assert(m_the_goal && m_the_squad);
    if (m_the_goal && m_the_squad)
    {
	    for 
        (	
            Agent_Match_List::iterator match_iter = m_matches.begin();
		    match_iter != m_matches.end();
		    ++match_iter
        ) 
        {
	        Agent_ptr agent_ptr = *(match_iter->squad_index); 
		    match_iter->value   = m_the_goal->Compute_Matching_Value(agent_ptr);
		    
            if (match_iter->value > Goal::BAD_UTILITY) 
            {
			    accumulated_matching_value += match_iter->value;
			    ++found_agents;
		    }
	    } 
    }

	if (found_agents > 0)
	{
		m_matching_value = accumulated_matching_value / found_agents;
        m_matches.sort(std::greater<Agent_Match>());
	}
	else
    {		
        m_matching_value = Goal::BAD_UTILITY;
    }
	
	return m_matching_value;
}


Utility Plan::Get_Matching_Value() const
{
	return m_matching_value;
}


void Plan::Set_Goal(Goal_ptr goal)
{
    m_the_goal = goal;

	Assert(m_the_goal && m_the_squad);
	if (m_the_goal && m_the_squad)
	{
		for 
        ( 		
            Agent_Match_List::iterator match_iter = m_matches.begin();
            match_iter != m_matches.end();
		    ++match_iter
        ) 
	    {
            match_iter->goal_index  = m_the_goal->Get_Agent_List().end();
            match_iter->committed   = false;
		}
	}
}


Goal_ptr Plan::Get_Goal() const
{
	return m_the_goal;
}


void Plan::Set_Squad(Squad_ptr squad)
{
	m_the_squad = squad;

	if (m_the_squad) 
    {
		Agent_List & agent_list = m_the_squad->Get_Agent_List();

        m_matches.resize(agent_list.size());

		Agent_Match_List::iterator match_iter       = m_matches.begin();

		for 
        (
            Agent_List::const_iterator agent_iter   = agent_list.begin();
			agent_iter != agent_list.end();
			++agent_iter        
        ) 
        {
			match_iter->value           = Goal::BAD_UTILITY;
			match_iter->squad_index     = agent_iter;
			
			if (m_the_goal)
			{
				match_iter->goal_index  = m_the_goal->Get_Agent_List().end();
			}

			match_iter->committed       = false;
            ++match_iter;
		}
	}
    else
    {
        m_matches.clear();
    }
}


Squad_ptr Plan::Get_Squad() const
{
	return m_the_squad;
}


sint32 Plan::Commit_Agents()
{
	Assert(m_the_goal);
	Assert(m_the_squad);
	Assert(m_matches.size() == (unsigned) m_the_squad->Get_Num_Agents());

    if (!m_the_goal || !m_the_squad)
    {
        return 0;
    }

    if (m_the_goal->Is_Satisfied() || m_the_goal->Get_Totally_Complete())
    {
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_the_goal->Get_Player_Index(), m_the_goal->Get_Goal_Type(), -1,
			("\t\tNO AGENTS COMMITTED: (goal: %x squad: %x)\n",m_the_goal, m_the_squad));

        
        return 0;
    }

	sint32 committed_agents = 0;
	
    for 
    (
        Agent_Match_List::iterator match_iter   = m_matches.begin();
		match_iter != m_matches.end() && !m_the_goal->Is_Satisfied();
	    ++match_iter
    ) 
    {
	    Agent_ptr agent_ptr = (* (*match_iter).squad_index );

		
		if (match_iter->value > Goal::BAD_UTILITY &&
			!agent_ptr->Get_Is_Used() &&
			agent_ptr->Get_Can_Be_Executed())
		{
				
#ifdef _DEBUG_SCHEDULER
			CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) m_the_goal;
			if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) agent_ptr))
			{
				Assert(0);
			}
#endif // _DEBUG_SCHEDULER

			
			bool const committed = 
                m_the_goal->Commit_Agent(agent_ptr, match_iter->goal_index);

			if (committed)
			{
				agent_ptr->Set_Is_Used(true);
				match_iter->committed = true;
				++committed_agents;
			}

#ifdef _DEBUG_SCHEDULER
			
			m_the_goal->Validate();
#endif // _DEBUG_SCHEDULER

		} 

	} 


#ifdef _DEBUG_SCHEDULER
	
	if (!m_the_goal->Validate())
	{
		Assert(0);
	}
#endif // _DEBUG_SCHEDULER


#ifdef _DEBUG
	CTPGoal_ptr stgoal_ptr = (CTPGoal_ptr) m_the_goal;
	Agent_List agent_list = m_the_squad->Get_Agent_List();
	Agent_List::iterator agent_iter;
	bool debug_plan;
	if (CtpAiDebug::IsDebugGoalTypeSet())
	{
		
		debug_plan = CtpAiDebug::DebugLogCheck(-1, m_the_goal->Get_Goal_Type(), -1);
	}
	else if (CtpAiDebug::IsDebugArmyIdSet())
	{
		
		debug_plan = false;
		for (agent_iter = agent_list.begin(); agent_iter != agent_list.end(); agent_iter++)
		{
			if (CtpAiDebug::DebugLogCheck(-1, -1, (((CTPAgent_ptr)(*agent_iter))->Get_Army().m_id)))
			{
				debug_plan = true;
				break;
			}
		}
	}
	else
	{
		
		debug_plan = CtpAiDebug::DebugLogCheck(m_the_goal->Get_Player_Index(), -1, -1);
	}

	int mask;
	
	if ( debug_plan && committed_agents > 0 ) 
	{
		if (stgoal_ptr->Is_Satisfied() || stgoal_ptr->Is_Execute_Incrementally())
			mask = k_DBG_SCHEDULER;
		else
			mask = k_DBG_SCHEDULER_DETAIL;
		
		
		DPRINTF(mask, ("\t\tEXECUTING GOAL: (goal: %x squad: %x)\n",stgoal_ptr, m_the_squad));
		DPRINTF(mask, ("\t\t\t"));
		stgoal_ptr->Log_Debug_Info(mask);
		
		DPRINTF(mask, ("\t\tSquad:\n"));
		m_the_squad->Log_Debug_Info(mask);
	}
#endif // _DEBUG

	return committed_agents;
}


GOAL_RESULT Plan::Execute_Task()
{
	Assert(m_the_goal);
	if (m_the_goal)	{
		return m_the_goal->Execute_Task();
	}
    return GOAL_FAILED;
}




bool Plan::Commited_Agents_Need_Orders() const
{
	Assert(m_the_goal);
	Assert(m_the_squad);

	sint16 rollback_agents = 0;
    CTPAgent_ptr agent_ptr;

	
	CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) m_the_goal;

	Agent_Match_List::const_iterator match_iter;
	for (match_iter = m_matches.begin();
		 match_iter != m_matches.end();
		 match_iter++) {

		
		agent_ptr = (CTPAgent_ptr) *(match_iter->squad_index);

		if (match_iter->committed)
			{

				if (agent_ptr->Get_Army()->NumOrders() <= 0)
					return true;
			} 

	} 

	return false;
}

bool Plan::CanMatchesBeReevaluated() const
{
    Assert(m_the_goal);
    if (m_the_goal)
    {
        GOAL_TYPE const     my_goal_type    = m_the_goal->Get_Goal_Type();
        GoalRecord const *  goalRecord      = g_theGoalDB->Get(my_goal_type);

        return goalRecord && !goalRecord->GetNoRollback();
    }

    return false;
}

sint32 Plan::Rollback_All_Agents()
{
	sint32 rollback_agents = 0;

	Assert(m_the_goal && m_the_squad);
    if (m_the_goal && m_the_squad)
    {
	    CTPGoal_ptr ctpgoal_ptr = static_cast<CTPGoal_ptr>(m_the_goal);
	    
	    for 
        (
            Agent_Match_List::iterator match_iter = m_matches.begin();
		    match_iter != m_matches.end();
		    ++match_iter
        ) 
        {
            CTPAgent_ptr agent_ptr = (CTPAgent_ptr) *(match_iter->squad_index);

		    if (match_iter->committed)
		    {
			    ctpgoal_ptr->Rollback_Agent(match_iter->goal_index);
			    match_iter->committed   = false;
			    match_iter->value       = Goal::BAD_UTILITY;
			    agent_ptr->Set_Is_Used(false);
			    ++rollback_agents;
		    } 
		    else
		    {
			    Assert(ctpgoal_ptr->Get_Agent_List().end() == match_iter->goal_index);
		    }
        }
	} 
	
	return rollback_agents;
}


void Plan::Move_All_Agents(Squad_ptr new_squad)
{
	Assert(m_the_goal);
	Assert(m_the_squad);

	Agent_ptr agent_ptr;
    Agent_List::const_iterator agent_iter;

	
	CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) m_the_goal;

	Agent_Match_List::iterator match_iter;
    match_iter = m_matches.begin();
	while (match_iter != m_matches.end()) {

        
		if (match_iter->committed)
			{
#ifdef _DEBUG
				
				CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) (*((*match_iter).goal_index));
				if (!g_theArmyPool->IsValid(ctpagent_ptr->Get_Army()))
				{
					bool SHOW_RICHARD_THIS_ASSERT_082900 = false;
					Assert(SHOW_RICHARD_THIS_ASSERT_082900);
				}
#endif // _DEBUG
				agent_ptr = ctpgoal_ptr->Rollback_Agent((*match_iter).goal_index);

                

				
				new_squad->Add_Agent(agent_ptr);

				
				match_iter->committed = false;

				
				agent_ptr->Set_Is_Used(false);

                
                agent_iter = match_iter->squad_index;

                
                
                match_iter++; 

                
				m_the_squad->Remove_Agent(agent_iter, false);
			} 
        else
		{
			
			Assert(ctpgoal_ptr->Get_Agent_List().end() == match_iter->goal_index);

            
            match_iter++; 
		}
	} 

#ifdef _DEBUG_SCHEDULER
	
	ctpgoal_ptr->Validate();
#endif // _DEBUG_SCHEDULER
}


bool Plan::Remove_Agent_Reference(const Agent_List::const_iterator & agent_iter)
{
	Assert(m_the_goal);
	Assert(m_the_squad);
	
	CTPGoal_ptr ctpgoal_ptr	= static_cast<CTPGoal_ptr>(m_the_goal);

	for 
    (
        Agent_Match_List::iterator  match_iter  = m_matches.begin();
        match_iter != m_matches.end();
        ++match_iter
    ) 
	{
        
        if (match_iter->squad_index == agent_iter) 
		{
            if (match_iter->committed) 
            {
                if (ctpgoal_ptr)
                {
				    ctpgoal_ptr->Rollback_Agent(match_iter->goal_index);
                }

				
				(*agent_iter)->Set_Is_Used(false);
	
				
#ifdef _DEBUG_SCHEDULER
				if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) *agent_iter))
				{
					Assert(0);
				}
#endif // _DEBUG_SCHEDULER
            }
			else 
			{
				Assert(ctpgoal_ptr->Get_Agent_List().end() == match_iter->goal_index);

				
#ifdef _DEBUG_SCHEDULER
				if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) *agent_iter))
				{
					Assert(0);
				}
#endif // _DEBUG_SCHEDULER
			}

            match_iter = m_matches.erase(match_iter);

            return true;
        } 
	} 

	return false;
}



bool Plan::Agent_Committed(const Agent_ptr agent_ptr) const
{
    for  
    (
	    Agent_Match_List::const_iterator match_iter = m_matches.begin();
        match_iter != m_matches.end();
        ++match_iter
    ) 
    {
		Agent_List::const_iterator goal_index = match_iter->goal_index;

		Assert(!match_iter->committed || goal_index != m_the_goal->Get_Agent_List().end());

        
        if (match_iter->committed && *goal_index == agent_ptr)
		{
            return true;
		}
	}

	return false;
}
