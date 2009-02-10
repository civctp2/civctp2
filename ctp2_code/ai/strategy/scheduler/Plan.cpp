//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Plan object (association Goal <-> agent)
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
// - Simplified the class design: m_matches replaced, since it just contained
//   pne agent pointer. (27-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------
//
/// @todo Check what happens when the m_the_goal or m_the_agent references
///       become invalid through delete.

#include "c3.h"
#include "Plan.h"

#include "profileai.h"
#include "debugassert.h"
#include "goal.h"
#include "Scheduler.h"
#include "Agent.h"
#include "ArmyData.h"
#include "agent.h"
#include "squad_strength.h"
#include "ctpaidebug.h"

#include "GoalRecord.h"

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
    m_the_agent         (NULL),
    m_the_goal          (NULL),
    m_needs_cargo       (false)
{
}

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
Plan::Plan(Agent_ptr agent, Goal_ptr goal, bool needsCargo)
:
    m_matching_value    (Goal::BAD_UTILITY),
    m_the_agent         (agent),
    m_the_goal          (goal),
    m_needs_cargo       (needsCargo)
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
    m_the_agent         (a_Original.m_the_agent),
    m_the_goal          (a_Original.m_the_goal),
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
		m_the_goal          = a_Original.m_the_goal;
		m_the_agent         = a_Original.m_the_agent;
		m_matching_value    = a_Original.m_matching_value;
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
	// m_the_goal not deleted   : reference only
	// m_the_agent not deleted  : reference only
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
// Remark(s)  : Equality is defined is having the same agent and the same 
//              goal.
//
//----------------------------------------------------------------------------
bool Plan::operator == (Plan const & plan) const
{
	return (m_the_agent == plan.m_the_agent) &&
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

sint32 Plan::Get_Free_Transport_Capacity() const
{
	sint32 transports = 0;
	sint32 max        = 0;
	sint32 empty      = 0;

	m_the_agent->Get_Army()->GetCargo(transports, max, empty);

	return empty;
}

Utility Plan::Compute_Matching_Value()
{
	Assert(m_the_goal && m_the_agent);
	if(m_the_goal && m_the_agent)
	{
		m_matching_value   = m_the_goal->Compute_Matching_Value(m_the_agent);
	}
	else
	{
		m_matching_value = Goal::BAD_UTILITY;
	}

	return m_matching_value;
}

void Plan::Commit_Agent()
{
	Assert(m_the_goal);
	Assert(m_the_agent);

	if(m_the_goal && m_the_agent)
	{
		if(!Needs_Cargo())
		{
			Commit_Agent_Common();
		}
	}
}

void Plan::Commit_Transport_Agent()
{
	Assert(m_the_goal);
	Assert(m_the_agent);

	if(m_the_goal && m_the_agent)
	{
		if(m_the_agent->Get_Army()->CanTransport())
		{
			Commit_Agent_Common();
		}
	}
}

void Plan::Commit_Agent_Common()
{
	if
	  (
	       m_matching_value > Goal::BAD_UTILITY
	   && !m_the_agent->Has_Goal()
	   &&  m_the_agent->Get_Can_Be_Executed()
	  )
	{

#ifdef _DEBUG_SCHEDULER
		if (m_the_goal->ReferencesAgent(m_the_agent))
		{
			Assert(false);
		}
#endif // _DEBUG_SCHEDULER

		m_the_goal->Commit_Agent(m_the_agent);

#ifdef _DEBUG_SCHEDULER
		
		m_the_goal->Validate();
#endif // _DEBUG_SCHEDULER

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

		if(CtpAiDebug::DebugLogCheck(-1, -1, m_the_agent->Get_Army().m_id))
		{
			debug_plan = true;
		}
	}
	else
	{
		debug_plan = CtpAiDebug::DebugLogCheck(m_the_goal->Get_Player_Index(), -1, -1);
	}

	if(debug_plan && Agent_Committed())
	{
		sint32 mask;
		if(m_the_goal->Is_Satisfied() || m_the_goal->Is_Execute_Incrementally())
			mask = k_DBG_SCHEDULER;
		else
			mask = k_DBG_SCHEDULER_DETAIL;

		DPRINTF(mask, ("\t\tEXECUTING GOAL:                (goal: %x agent: %x, id: 0%x)\n", m_the_goal, m_the_agent, m_the_agent->Get_Army().m_id));

		m_the_agent->Log_Debug_Info(mask, m_the_goal);
		DPRINTF(mask, ("\t\t\t\n"));
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
	if(Agent_Committed())
	{
		if(m_the_agent->Get_Army()->NumOrders() <= 0)
			return true;
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
	Assert(m_the_goal && m_the_agent);
	if(m_the_goal && m_the_agent)
	{
		if(Agent_Committed())
		{
			m_the_goal->Rollback_Agent(m_the_agent);
		}
	}
}

void Plan::Rollback_Emptied_Transporters()
{
	Assert(m_the_goal && m_the_agent);
	if(m_the_goal && m_the_agent)
	{
		if(Agent_Committed())
		{
			const MapPoint pos     = m_the_agent->Get_Target_Pos();
			MapPoint goalPos(-1,-1);

			if(m_the_goal->Get_Target_Army().m_id == 0 || m_the_goal->Get_Target_Army().IsValid())
			{
				goalPos = m_the_goal->Get_Target_Pos();
			}

			if(pos == goalPos)
			{
				if(!m_the_goal->Pretest_Bid(m_the_agent, goalPos))
				{
					AI_DPRINTF(k_DBG_SCHEDULER, m_the_agent->Get_Army()->GetOwner(), m_the_goal->Get_Goal_Type(), -1, 
						("\t\tTransporter not needed anymore, removing from goal\n"));

					m_the_goal->Rollback_Agent(m_the_agent);
					m_matching_value       = Goal::BAD_UTILITY;
				}
			}
		}
	}
}

bool Plan::Agent_Committed() const
{
	Assert(m_the_goal != NULL);
	return m_the_goal != NULL && m_the_agent->Get_Goal() == m_the_goal;
}

bool Plan::Has_Cargo() const
{
	return m_the_agent->Get_Army()->HasCargo();
}

void Plan::Log_Debug_Info(const int & log) const
{
	m_the_agent->Log_Debug_Info(log, m_the_goal);
}

bool Plan::All_Unused_Or_Used_By_This() const
{
	Goal_ptr goal = m_the_agent->Get_Goal();

	return goal == NULL || goal == m_the_goal;
}
