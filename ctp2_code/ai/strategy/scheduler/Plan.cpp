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
// - Simplified the class design: m_matches replaced, since it just contained
//   pne agent pointer. (27-Aug-2008 Martin Gühmann)
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
#include "Scheduler.h"
#include "Agent.h"
#include "ArmyData.h"
#include "ctpagent.h"
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
    m_the_squad         (NULL),
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
Plan::Plan(Squad_ptr squad, Goal_ptr goal, bool needsCargo)
:
    m_matching_value    (Goal::BAD_UTILITY),
    m_the_squad         (squad),
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
    m_the_squad         (a_Original.m_the_squad),
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
		m_the_squad         = a_Original.m_the_squad;
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
	sint32 transports = 0;
	sint32 max        = 0;
	sint32 empty      = 0;

	m_the_squad->Get_Agent()->Get_Army()->GetCargo(transports, max, empty);

	return empty;
}

Utility Plan::Compute_Matching_Value()
{
	Assert(m_the_goal && m_the_squad);
	if(m_the_goal && m_the_squad)
	{
		m_matching_value   = m_the_goal->Compute_Matching_Value(static_cast<Agent_ptr>(m_the_squad->Get_Agent()));
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

Goal_ptr Plan::Get_Goal() const
{
	return m_the_goal;
}

Squad_ptr Plan::Get_Squad() const
{
	return m_the_squad;
}

void Plan::Commit_Agent()
{
	Assert(m_the_goal);
	Assert(m_the_squad);

	if(m_the_goal && m_the_squad)
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
	Assert(m_the_squad);

	if(m_the_goal && m_the_squad)
	{
		if(m_the_squad->Get_Agent()->Get_Army()->CanTransport())
		{
			Commit_Agent_Common();
		}
	}
}

void Plan::Commit_Agent_Common()
{
	Agent_ptr agent_ptr = m_the_squad->Get_Agent();

	if
	  (
	       m_matching_value > Goal::BAD_UTILITY
	   && !agent_ptr->Has_Goal()
	   &&  agent_ptr->Get_Can_Be_Executed()
	  )
	{

#ifdef _DEBUG_SCHEDULER
		CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) m_the_goal;
		if (ctpgoal_ptr->ReferencesAgent((const CTPAgent_ptr) agent_ptr))
		{
			Assert(false);
		}
#endif // _DEBUG_SCHEDULER

		if(m_the_goal->Commit_Agent(agent_ptr))
		{
			agent_ptr->Set_Goal(m_the_goal); // Move inside goals
		}

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

	if(debug_plan && Agent_Committed())
	{
		sint32 mask;
		if(m_the_goal->Is_Satisfied() || m_the_goal->Is_Execute_Incrementally())
			mask = k_DBG_SCHEDULER;
		else
			mask = k_DBG_SCHEDULER_DETAIL;

		DPRINTF(mask, ("\t\tEXECUTING GOAL:                (goal: %x squad: %x, id: 0%x)\n", m_the_goal, m_the_squad, static_cast<CTPAgent_ptr>(m_the_squad->Get_Agent())->Get_Army().m_id));

		m_the_squad->Log_Debug_Info(mask, m_the_goal);
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
		if(m_the_squad->Get_Agent()->Get_Army()->NumOrders() <= 0)
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
	Assert(m_the_goal && m_the_squad);
	if(m_the_goal && m_the_squad)
	{
		if(Agent_Committed())
		{
			m_the_goal->Rollback_Agent(m_the_squad->Get_Agent());
			m_the_squad->Get_Agent()->Set_Goal(NULL);
		}
	}
}

void Plan::Rollback_Emptied_Transporters()
{
	Assert(m_the_goal && m_the_squad);
	if(m_the_goal && m_the_squad)
	{
		if(Agent_Committed())
		{
			CTPAgent_ptr agent_ptr = m_the_squad->Get_Agent();

			const MapPoint pos     = agent_ptr->Get_Target_Pos();
			MapPoint goalPos(-1,-1);

			if(m_the_goal->Get_Target_Army().m_id == 0 || m_the_goal->Get_Target_Army().IsValid())
			{
				goalPos = m_the_goal->Get_Target_Pos();
			}

			if(pos == goalPos)
			{
				if(!m_the_goal->Pretest_Bid(agent_ptr, goalPos))
				{
					AI_DPRINTF(k_DBG_SCHEDULER, agent_ptr->Get_Army()->GetOwner(), m_the_goal->Get_Goal_Type(), -1, 
						("\t\tTransporter not needed anymore, removing from goal\n"));

					m_the_goal->Rollback_Agent(m_the_squad->Get_Agent());
					m_matching_value       = Goal::BAD_UTILITY;
					agent_ptr->Set_Goal(NULL);
				}
			}
		}
	}
}

bool Plan::Has_Cargo() const
{
	return m_the_squad->Get_Agent()->Get_Army()->HasCargo();
}

bool Plan::Agent_Committed(const Agent_ptr agent_ptr) const
{
	return agent_ptr == m_the_squad->Get_Agent() && Agent_Committed();
}

bool Plan::Agent_Committed() const
{
	Assert(m_the_goal != NULL);
	return m_the_goal != NULL && m_the_squad->Get_Agent()->Get_Goal() == m_the_goal;
}

void Plan::Log_Debug_Info(const int & log) const
{
	m_the_squad->Get_Agent()->Log_Debug_Info(log, m_the_goal);
}

bool Plan::All_Unused_Or_Used_By_This() const
{
	Goal_ptr goal = m_the_squad->Get_Agent()->Get_Goal();

	return goal == NULL || goal == m_the_goal;
}

bool Plan::Needs_Cargo() const
{
	return m_needs_cargo && !Has_Cargo();
}

bool Plan::Can_Add_To_Goal() const
{
	Assert(m_the_goal && m_the_squad);

	if(m_the_goal && m_the_squad)
	{
		if(!m_the_goal->Can_Add_To_Goal(m_the_squad->Get_Agent()))
		{
			return false;
		}
	}

	return true;
}
