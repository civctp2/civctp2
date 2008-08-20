//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Plan object (association Goal <-> squad)
// Id           : $Id$
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
// USE_LOGGING
// - Enable logging when set, even when not a debug version. This is not
//   original Activision code.
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
// - Deferred some derefences until used (hopefully preventing crashes).
// - Restored old plan sorting and added a bonus for goals which may need many
//   units. (25-Jan-2008 Martin Gühmann)
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// @todo Check what happens when the m_the_goal or m_the_squad references
///       become invalid through delete.

#include "c3.h"
#include "Plan.h"

#include "profileai.h"
#include "debugassert.h"
#include "squad.h"
#include "goal.h"
#include "Agent.h"
#include "ArmyData.h"
#include "ctpagent.h"
#include "ctpgoal.h"
#include "Scheduler.h"
#include "squad_strength.h"
#include "ctpaidebug.h"

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
:
    m_matching_value    (Goal::BAD_UTILITY),
    m_the_squad         (NULL),
    m_the_goal          (NULL),
    m_matches           (),
    m_needs_cargo       (false)
{
}

//----------------------------------------------------------------------------
//
// Name       : Plan::Plan
//
// Description: Copy constructor
//
// Parameters : a_Original  : the object to copy the data from
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
Plan::Plan(Plan const & a_Original)
:
    m_matching_value    (a_Original.m_matching_value),
    m_the_squad         (a_Original.m_the_squad),
    m_the_goal          (a_Original.m_the_goal),
    m_matches           (a_Original.m_matches),
    m_needs_cargo       (a_Original.m_needs_cargo)
{
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
Plan & Plan::operator = (Plan const & a_Original)
{
	if(this != &a_Original)
	{
		m_the_goal          = NULL;         // temporary, until set in Set_Goal
		Set_Squad(a_Original.m_the_squad);  // sets m_the_squad, uses m_the_goal
		Set_Goal(a_Original.m_the_goal);    // sets m_the_goal, uses m_the_squad
		m_matching_value    = a_Original.m_matching_value;
		m_matches           = a_Original.m_matches;
		m_needs_cargo       = a_Original.m_needs_cargo;
	}

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
	// m_the_goal not deleted   : reference only
	// m_the_squad not deleted  : reference only
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
	// Matching value is based on the average match value per agent.
	return m_matching_value < plan.m_matching_value;
}

bool Plan::operator > (Plan const & plan) const
{
	// Matching value is based on the average match value per agent.
	return m_matching_value > plan.m_matching_value;
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
	if (m_the_goal)
	{
		return m_the_goal->Get_Goal_Type();
	}
	else
	{
		Assert(false);
		return GOAL_TYPE_NULL;
	}
}

bool Plan::Plan_Is_Needed_And_Valid() const
{
	return m_the_goal  &&
	       m_the_squad && (m_the_squad->Get_Num_Agents() > 0);
}

sint32 Plan::Get_Free_Transport_Capacity() const
{
	sint32 transports;
	sint32 max;
	sint32 empty;
	sint32 freeTransportCapacity = 0;

	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		CTPAgent_ptr agent_ptr = static_cast<CTPAgent_ptr>(match_iter->squad_index);
		agent_ptr->Get_Army()->GetCargo(transports, max, empty);
		freeTransportCapacity += empty;
	}

	return freeTransportCapacity;
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
		    Agent_Match_List::iterator match_iter  = m_matches.begin();
		                                     match_iter != m_matches.end();
		                                   ++match_iter
		)
		{
			const Agent_ptr agent_ptr = match_iter->squad_index;

			match_iter->value   = static_cast<CTPGoal_ptr>(m_the_goal)->Compute_Matching_Value(agent_ptr);

			if (match_iter->value > Goal::BAD_UTILITY)
			{
				accumulated_matching_value += match_iter->value;
				++found_agents;
			}
		}
	}

	if (found_agents > 0)
	{
		m_matching_value  = accumulated_matching_value / found_agents;
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

	if(m_the_squad)
	{
		Agent_ptr agent_ptr = m_the_squad->Get_Agent();

		m_matches.resize(1);

		Agent_Match_List::iterator match_iter       = m_matches.begin();

		match_iter->value           = Goal::BAD_UTILITY;
		match_iter->squad_index     = agent_ptr;

		match_iter->committed       = false;
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

void Plan::Commit_Agents()
{
	Assert(m_the_goal);
	Assert(m_the_squad);
	Assert(m_matches.size() == m_the_squad->Get_Num_Agents());

	if(!m_the_goal || !m_the_squad)
	{
		return;
	}

	if(Get_Matching_Value() <= Goal::BAD_UTILITY)
	{
		return;
	}

	if(m_the_goal->Is_Satisfied() || m_the_goal->Get_Totally_Complete())
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_the_goal->Get_Player_Index(), m_the_goal->Get_Goal_Type(), -1,
			("\t\tNO AGENTS COMMITTED:           (goal: %x squad: %x, id: 0%x)\n",m_the_goal, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));

		return;
	}
	else
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_the_goal->Get_Player_Index(), m_the_goal->Get_Goal_Type(), -1,
			("\t\tAGENTS CAN BE COMMITTED:       (goal: %x squad: %x, id: 0%x)\n",m_the_goal, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));
	}

	for
	(
	    Agent_Match_List::iterator match_iter  = m_matches.begin();
	                               match_iter != m_matches.end() && !m_the_goal->Is_Satisfied();
	                             ++match_iter
	)
	{
		Agent_ptr agent_ptr = match_iter->squad_index;

		if
		  (
		       match_iter->value > Goal::BAD_UTILITY
		   && !agent_ptr->Has_Goal()
		   &&  agent_ptr->Get_Can_Be_Executed()
		   && !Needs_Cargo()
		  )
		{

#ifdef _DEBUG_SCHEDULER
			CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) m_the_goal;
			if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) agent_ptr))
			{
				Assert(false);
			}
#endif // _DEBUG_SCHEDULER

			
			bool const committed = 
			    m_the_goal->Commit_Agent(agent_ptr);

			if(committed)
			{
				agent_ptr->Set_Goal(m_the_goal); // Move inside goals
				match_iter->committed = true;
			}

#ifdef _DEBUG_SCHEDULER
			
			m_the_goal->Validate();
#endif // _DEBUG_SCHEDULER

		}
	}

#ifdef _DEBUG_SCHEDULER
	
	if (!m_the_goal->Validate())
	{
		Assert(false);
	}
#endif // _DEBUG_SCHEDULER

#if defined(_DEBUG) || defined(USE_LOGGING)
	bool debug_plan;
	if (CtpAiDebug::IsDebugGoalTypeSet())
	{
		debug_plan = CtpAiDebug::DebugLogCheck(-1, m_the_goal->Get_Goal_Type(), -1);
	}
	else if (CtpAiDebug::IsDebugArmyIdSet())
	{
		debug_plan = false;

		CTPAgent_ptr agent_ptr = static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent());

		if(CtpAiDebug::DebugLogCheck(-1, -1, agent_ptr->Get_Army().m_id))
		{
			debug_plan = true;
		}
	}
	else
	{
		debug_plan = CtpAiDebug::DebugLogCheck(m_the_goal->Get_Player_Index(), -1, -1);
	}

	if(debug_plan && m_matches.begin()->committed)
	{
		CTPGoal_ptr goal_ptr = (CTPGoal_ptr) m_the_goal;

		sint32 mask;
		if (goal_ptr->Is_Satisfied() || goal_ptr->Is_Execute_Incrementally())
			mask = k_DBG_SCHEDULER;
		else
			mask = k_DBG_SCHEDULER_DETAIL;

		DPRINTF(mask, ("\t\tEXECUTING GOAL:                (goal: %x squad: %x, id: 0%x)\n", goal_ptr, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));

		m_the_squad->Log_Debug_Info(mask, m_the_goal);
		DPRINTF(mask, ("\t\t\t\n"));
	}
#endif // _DEBUG
}

void Plan::Commit_Transport_Agents()
{
	Assert(m_the_goal);
	Assert(m_the_squad);
	Assert(m_matches.size() == m_the_squad->Get_Num_Agents());

	if (!m_the_goal || !m_the_squad)
	{
		return;
	}

	if(!m_the_goal->Needs_Transporter() || m_the_goal->Get_Totally_Complete())
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_the_goal->Get_Player_Index(), m_the_goal->Get_Goal_Type(), -1,
			("\t\tNO TRANSPORT AGENTS COMMITTED: (goal: %x squad: %x, id: 0%x)\n",m_the_goal, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));

		return;
	}
	else
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_the_goal->Get_Player_Index(), m_the_goal->Get_Goal_Type(), -1,
			("\t\tTRANSPORT AGENTS COMMITTED:    (goal: %x squad: %x, id: 0%x)\n",m_the_goal, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));
	}

	for
	(
	    Agent_Match_List::iterator match_iter  = m_matches.begin();
	                               match_iter != m_matches.end() && m_the_goal->Needs_Transporter();
	                             ++match_iter
	)
	{
		const CTPAgent_ptr agent_ptr = static_cast<const CTPAgent_ptr>(match_iter->squad_index );

		if
		(
		       match_iter->value > Goal::BAD_UTILITY
		   && !agent_ptr->Has_Goal()
		   &&  agent_ptr->Get_Can_Be_Executed()
		   &&  agent_ptr->Get_Army()->CanTransport()
		)
		{

#ifdef _DEBUG_SCHEDULER
			CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) m_the_goal;
			if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) agent_ptr))
			{
				Assert(false);
			}
#endif // _DEBUG_SCHEDULER

			
			bool const committed = 
			    m_the_goal->Commit_Agent(agent_ptr);

			if(committed)
			{
				agent_ptr->Set_Goal(m_the_goal);
				match_iter->committed = true;
			}

#ifdef _DEBUG_SCHEDULER
			
			m_the_goal->Validate();
#endif // _DEBUG_SCHEDULER

		}
	}

#ifdef _DEBUG_SCHEDULER
	
	if (!m_the_goal->Validate())
	{
		Assert(false);
	}
#endif // _DEBUG_SCHEDULER

#ifdef _DEBUG
	bool debug_plan;
	if (CtpAiDebug::IsDebugGoalTypeSet())
	{
		debug_plan = CtpAiDebug::DebugLogCheck(-1, m_the_goal->Get_Goal_Type(), -1);
	}
	else if(CtpAiDebug::IsDebugArmyIdSet())
	{
		debug_plan = false;

		CTPAgent_ptr agent_ptr = static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent());

		if(CtpAiDebug::DebugLogCheck(-1, -1, agent_ptr->Get_Army().m_id))
		{
			debug_plan = true;
		}
	}
	else
	{
		debug_plan = CtpAiDebug::DebugLogCheck(m_the_goal->Get_Player_Index(), -1, -1);
	}

	if(debug_plan && m_matches.begin()->committed)
	{
		CTPGoal_ptr goal_ptr = (CTPGoal_ptr) m_the_goal;

		sint32 mask;
		if (goal_ptr->Is_Satisfied() || goal_ptr->Is_Execute_Incrementally())
			mask = k_DBG_SCHEDULER;
		else
			mask = k_DBG_SCHEDULER_DETAIL;
		
		
		DPRINTF(mask, ("\t\tEXECUTING GOAL:                (goal: %x squad: %x, id: 0%x)\n", goal_ptr, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));
		DPRINTF(mask, ("\t\t\t\n"));
		goal_ptr->Log_Debug_Info(mask);
		
		DPRINTF(mask, ("\t\tSquad:\n"));
		m_the_squad->Log_Debug_Info(mask, m_the_goal);
	}
#endif // _DEBUG
}

GOAL_RESULT Plan::Execute_Task()
{
	Assert(m_the_goal);
	return m_the_goal ? m_the_goal->Execute_Task() : GOAL_FAILED;
}

bool Plan::Commited_Agents_Need_Orders() const
{
	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		if(match_iter->committed)
		{
			CTPAgent_ptr agent_ptr = static_cast<CTPAgent_ptr>(match_iter->squad_index);

			if (agent_ptr->Get_Army()->NumOrders() <= 0)
				return true;
		}
	}

	return false;
}

bool Plan::CanMatchesBeReevaluated() const
{
	Assert(m_the_goal);
	if(m_the_goal)
	{
		GOAL_TYPE const     my_goal_type    = m_the_goal->Get_Goal_Type();
		GoalRecord const *  goalRecord      = g_theGoalDB->Get(my_goal_type);

		return goalRecord && !goalRecord->GetNoRollback();
	}

	return false;
}

void Plan::Rollback_All_Agents()
{
	Assert(m_the_goal && m_the_squad);
	if(m_the_goal && m_the_squad)
	{
		CTPGoal_ptr ctpgoal_ptr = static_cast<CTPGoal_ptr>(m_the_goal);

		for
		(
		    Agent_Match_List::iterator match_iter  = m_matches.begin();
		                               match_iter != m_matches.end();
		                             ++match_iter
		)
		{
			if(match_iter->committed)
			{
				CTPAgent_ptr agent_ptr = static_cast<CTPAgent_ptr>(match_iter->squad_index);
				ctpgoal_ptr->Rollback_Agent(match_iter->squad_index);
				match_iter->committed   = false;
		//		match_iter->value       = Goal::BAD_UTILITY;
				agent_ptr->Set_Goal(NULL);
			}
		}
	}
}

void Plan::Rollback_Emptied_Transporters()
{
	Assert(m_the_goal && m_the_squad);
	if(m_the_goal && m_the_squad)
	{
		CTPGoal_ptr ctpgoal_ptr = static_cast<CTPGoal_ptr>(m_the_goal);

		for
		(
		    Agent_Match_List::iterator match_iter  = m_matches.begin();
		                               match_iter != m_matches.end();
		                             ++match_iter
		)
		{
			if(match_iter->committed)
			{
				CTPAgent_ptr agent_ptr = static_cast<CTPAgent_ptr>(match_iter->squad_index);

				const MapPoint pos     = agent_ptr->Get_Target_Pos();
				const MapPoint goalPos = ctpgoal_ptr->Get_Target_Pos();

				if(pos == goalPos)
				{
					if(!ctpgoal_ptr->Pretest_Bid(agent_ptr, goalPos))
					{
						AI_DPRINTF(k_DBG_SCHEDULER, agent_ptr->Get_Army()->GetOwner(), m_the_goal->Get_Goal_Type(), -1, 
							("\t\tTransporter not needed anymore, removing from goal\n"));

						ctpgoal_ptr->Rollback_Agent(match_iter->squad_index);
						match_iter->committed   = false;
						match_iter->value       = Goal::BAD_UTILITY;
						agent_ptr->Set_Goal(NULL);
					}
				}
			}
		}
	}
}

bool Plan::Remove_Agent_Reference(const Agent_ptr agent_ptr)
{
	Assert(m_the_goal && m_the_squad);
	
	CTPGoal_ptr ctpgoal_ptr	= static_cast<CTPGoal_ptr>(m_the_goal);

	for
	(
	    Agent_Match_List::iterator match_iter  = m_matches.begin();
	                               match_iter != m_matches.end();
	                             ++match_iter
	)
	{
		if(match_iter->squad_index == agent_ptr)
		{
			if (match_iter->committed)
			{
				if (ctpgoal_ptr)
				{
					ctpgoal_ptr->Rollback_Agent(match_iter->squad_index);
				}

				agent_ptr->Set_Goal(NULL);
			}

#ifdef _DEBUG_SCHEDULER
			if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) *agent_iter))
			{
				Assert(0);
			}
#endif // _DEBUG_SCHEDULER

			match_iter = m_matches.erase(match_iter);
			return true;
		}
	}

	return false;
}

bool Plan::Has_Cargo() const
{
	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		const CTPAgent_ptr agent_ptr = static_cast<const CTPAgent_ptr>(match_iter->squad_index );
		if(agent_ptr->Get_Army()->HasCargo())
			return true;
	}

	return false;
}

bool Plan::Has_Space_For_Cargo() const
{
	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		const CTPAgent_ptr agent_ptr = static_cast<const CTPAgent_ptr>(match_iter->squad_index);
		sint32 transports = 0;
		sint32 max        = 0;
		sint32 empty      = 0;
		agent_ptr->Get_Army()->GetCargo(transports, max, empty);

		if(max > 0)
			return true;
	}

	return false;
}

bool Plan::Agent_Committed(const Agent_ptr agent_ptr) const
{
	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		if (match_iter->committed)
		{
			return true;
		}
	}

	return false;
}

void Plan::Log_Debug_Info(const int & log) const
{
	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		CTPAgent_ptr ctpagent_ptr = static_cast<CTPAgent_ptr>(match_iter->squad_index);
		ctpagent_ptr->Log_Debug_Info(log, m_the_goal);
	}
}

bool Plan::All_Unused_Or_Used_By_This() const
{
	for
	(
	    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
	                                     match_iter != m_matches.end();
	                                   ++match_iter
	)
	{
		CTPAgent_ptr ctpagent_ptr = static_cast<CTPAgent_ptr>(match_iter->squad_index);

		Goal_ptr goal = ctpagent_ptr->Get_Goal();

		if(goal != NULL
		&& goal != m_the_goal
		){
			return false;
		}
	}

	return true;
}

bool Plan::Needs_Cargo() const
{
	return m_needs_cargo && !Has_Cargo();
}

bool Plan::Can_Add_To_Goal() const
{
	size_t  found_agents                = 0;
	Utility accumulated_matching_value  = 0;

	Assert(m_the_goal && m_the_squad);

	if(m_the_goal && m_the_squad)
	{
		for
		(
		    Agent_Match_List::const_iterator match_iter  = m_matches.begin();
		                                     match_iter != m_matches.end();
		                                   ++match_iter
		)
		{
			Agent_ptr agent_ptr = match_iter->squad_index;

			Assert(agent_ptr);

			if(!m_the_goal->Can_Add_To_Goal(agent_ptr))
			{
				return false;
			}
		}
	}

	return true;
}
