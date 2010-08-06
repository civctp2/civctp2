//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Scheduler for AI goals
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
// - Generate debug version when set.
//
// _DEBUG_SCHEDULER
//
// USE_LOGGING
// - Enable logging when set, even when not a debug version. This is not
//   original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Do not consider invalid goals (e.g. threatened city that has been 
//   destroyed already).
// - Marked MS version specific code.
// - Added CanMatchesBeReevaluated() check -> to reevaluate the goals each turn
//   (not only first turn of war...)
// - Activated the double Goal check for all goals (not only settle goals)
// - Added Peter's Comments
// - Repaired crash when a goal type has not been defined (GOAL_SEIGE in CTC)
// - Removed double warning disable pragma.
// - Standardised list import.
// - Removed unused memory reservation.
// - New matches are now always added if a new army has been created. (30-Jun-2008 Martin Gühmann)
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
// - Simplified the design the number of committed agents and number of
//   agents are now calculated inside the Match_Resources method. (21-Aug-2008 Martin Gühmann)
// - Fixed unit garrison assignment. (23-Jan-2009 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Scheduler.h"

#include "profileai.h"

#include <algorithm>
#include <list>


#include "goal.h"
#include "Plan.h"
#include "agent.h"

#include "c3errors.h"

#include "player.h"
#include "Diplomat.h"
#include "CTPDatabase.h"
#include "GoalRecord.h"
#include "ctpaidebug.h"
#include "c3math.h"
#include "AgreementMatrix.h"
#include "gfx_options.h"
#include "Army.h"
#include "ArmyData.h"
#include "CityData.h"
#include "World.h"

namespace
{

//----------------------------------------------------------------------------
//
// Name       : IsValid
//
// Description: Determine whether a goal is valid in a given context
//
// Parameters : a_Goal          : the goal (type)
//              a_Context       : the context
//
// Globals    : -
//
// Returns    : bool            : the goal is valid in the context
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
	bool IsValid
	(
	    GOAL_TYPE const &                               a_Goal,
	    Scheduler::Sorted_Goal_List_Vector  const &     a_Context
	)
	{
		return static_cast<size_t>(a_Goal) < a_Context.size();
	};

} // namespace

sint32 Scheduler::s_max_match_list_cycles = 6;

Scheduler::Scheduler_Vector Scheduler::s_theSchedulers;

sint32 Scheduler::m_contactCachedPlayer       = -1;
uint32 Scheduler::m_contactCache              =  0;
sint32 Scheduler::m_neutralRegardCachedPlayer = -1;
uint32 Scheduler::m_neutralRegardCache        =  0;
sint32 Scheduler::m_allyRegardCachedPlayer    = -1;
uint32 Scheduler::m_allyRegardCache           =  0;

void Scheduler::ResizeAll(const PLAYER_INDEX & newMaxPlayerId)
{
	s_theSchedulers.resize(newMaxPlayerId);

	for(sint32 i = 0; i < newMaxPlayerId; ++i)
	{
		s_theSchedulers[i].SetPlayerId(i);
	}
}

#if 0
// no longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::LoadAll(CivArchive & archive)
{
	DPRINTF(k_DBG_AI, ("\n\ncalling Scheduler::LoadAll\n\n"));
	for(size_t i = 0; i < s_theSchedulers.size(); i++)
	{
		s_theSchedulers[i].Load(archive);
	}
}

// no longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::SaveAll(CivArchive & archive)
{
	for(size_t i = 0; i < s_theSchedulers.size(); i++)
	{
		s_theSchedulers[i].Save(archive);
	}
}
#endif

//////////////////////////////
//
// used mainly in ctpai to get the player's scheduler
//
// also in Governor::ComputeDesiredUnits
//         Governor::GetTacticalAdvice
//         ThreatenedCity_MotivationEvent
//
//////////////////////////////
Scheduler & Scheduler::GetScheduler(const sint32 & playerId)
{
	Assert(playerId >= 0);
	Assert(static_cast<size_t>(playerId) < s_theSchedulers.size());
	
	return s_theSchedulers[playerId]; 
}

void Scheduler::CleanupAll(void)
{
	Scheduler_Vector().swap(s_theSchedulers);
}

Scheduler::Scheduler()
:
    m_goals_of_type         (),
    m_agents                (),
    m_playerId              (-1),
    m_neededAgentStrength   (),
    m_goals                 (),
    m_generic_goals         ()
{
	m_goals_of_type.resize  (g_theGoalDB->NumRecords());
	m_generic_goals.reserve (g_theGoalDB->NumRecords());

	for(int i = 0; i < g_theGoalDB->NumRecords(); ++i)
	{
		Goal_ptr generic_goal = new Goal();

		generic_goal->Set_Type(i);

		m_generic_goals.push_back(generic_goal);
	}
}

Scheduler::Scheduler(const Scheduler &scheduler)
:
    m_goals_of_type         (), // Pointers point to objects that will be deleted, so the objects themselves must be copied
    m_agents                (), // Pointers point to objects that will be deleted, so the objects themselves must be copied
    m_playerId              (scheduler.m_playerId),
    m_neededAgentStrength   (scheduler.m_neededAgentStrength),
    m_goals                 (),
    m_generic_goals         ()
{
	m_goals_of_type.resize  (g_theGoalDB->NumRecords());
	m_generic_goals.reserve (g_theGoalDB->NumRecords());

	Agent_List::const_iterator agent_ptr_iter = scheduler.m_agents.begin();
	while(agent_ptr_iter != scheduler.m_agents.end())
	{
		m_agents.push_back(new Agent(**agent_ptr_iter));
		++agent_ptr_iter;
	}

	Goal_Vector::const_iterator generic_goals_iter = scheduler.m_generic_goals.begin();

	Sorted_Goal_Const_Iter sorted_goal_iter;

	for(size_t    goal_type = 0;
	              goal_type < scheduler.m_goals_of_type.size();
	              goal_type++
	){
		m_generic_goals.push_back(new Goal(**generic_goals_iter));
		Assert(generic_goals_iter != scheduler.m_generic_goals.end());
		++generic_goals_iter;

		sorted_goal_iter = scheduler.m_goals_of_type[goal_type].begin();

		while(sorted_goal_iter != scheduler.m_goals_of_type[goal_type].end() )
		{
			m_goals_of_type[goal_type].push_back(Sorted_Goal_ptr(Goal::BAD_UTILITY, new Goal(*sorted_goal_iter->second)));
			++sorted_goal_iter;
		}
	}
}

Scheduler::~Scheduler()
{
	Cleanup();
}

Scheduler& Scheduler::operator= (const Scheduler &scheduler)
{
	Assert(false); // Hopefully not used
	Assert(scheduler.m_playerId == -1);

	Initialize();

	return *this;
}

void Scheduler::Cleanup()
{
#if defined(_DEBUG_SCHEDULER)
	// Maybe removed again
	agent_ptr_iter = m_agents.begin();
	while(agent_ptr_iter != m_agents.end())
	{
		Agent_List::iterator agent2_ptr_iter = agent_ptr_iter;
		++agent2_ptr_iter;

		while(agent2_ptr_iter != m_agents.end())
		{
			Agent* agent1 = (*agent_ptr_iter);
			Agent* agent2 = (*agent2_ptr_iter);

			Assert(agent1 != agent2);

			++agent2_ptr_iter;
		}
		++agent_ptr_iter;
	}
#endif

	Agent_List::iterator agent_ptr_iter = m_agents.begin();
	while(agent_ptr_iter != m_agents.end())
	{
		// No need to delete agent references in goals, they will be deleted anyway.
		delete *agent_ptr_iter;
		agent_ptr_iter = m_agents.erase(agent_ptr_iter);
	}

	Sorted_Goal_Iter sorted_goal_iter;
	for(size_t    goal_type = 0;
	              goal_type < m_goals_of_type.size();
	              goal_type++
	){
		sorted_goal_iter = m_goals_of_type[goal_type].begin();
		while (sorted_goal_iter != m_goals_of_type[goal_type].end() )
		{
			delete sorted_goal_iter->second;
			sorted_goal_iter++;
		}
		m_goals_of_type[goal_type].clear();
	}

	m_goals_of_type.clear();
	m_goals.clear();

	for(size_t i = 0; i < m_generic_goals.size(); ++i)
	{
		delete m_generic_goals[i];
	}

	m_generic_goals.clear();
}

#if 0
/// No longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::Load(CivArchive & archive)
{
	// Nice reason but that could be more difficult than thought.
}

/// No longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::Save(CivArchive & archive)
{
	// Nice reason but that could be more difficult than thought.
}
#endif

void Scheduler::Initialize()
{
	if(g_theGoalDB == NULL)
		return;

	Cleanup();

	m_generic_goals.reserve(g_theGoalDB->NumRecords());

	for(int i = 0; i < g_theGoalDB->NumRecords(); ++i)
	{
		Goal_ptr generic_goal = new Goal();

		generic_goal->Set_Type(i);

		m_generic_goals.push_back(generic_goal);
	}

	m_goals_of_type.resize(g_theGoalDB->NumRecords());
}

void Scheduler::SetPlayerId(const PLAYER_INDEX &player_index)
{
	m_playerId = player_index;

	for(size_t i = 0; i < m_generic_goals.size(); ++i)
	{
		m_generic_goals[i]->Set_Player_Index(player_index);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//  Process_Agent_Changes
//
//  When: Agents Change
//
//  Iterate: m_agents
//
//  1. Remove any empty agents and any matches that reference them.
//
//  2. Recompute agent class for all agents with changed/killed agents.
//
//  3. If agent class of agent changes, Add_New_Matches_For_Agent
//
//
//  4. Create new agents from new agents, add to agents_of_class
//     list and create new matches from pruned_goals_list.
//
//  5. Count up total number of agents available to match
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Process_Agent_Changes()
{
	DPRINTF(k_DBG_AI, ("//       Change agent matches\n"));
	time_t t1 = GetTickCount();

	Agent_List::iterator agent_ptr_iter = m_agents.begin();
	while(agent_ptr_iter != m_agents.end())
	{
		Agent* theAgent = (*agent_ptr_iter);

		if(theAgent->Get_Is_Dead())
		{
			Remove_Matches_For_Agent(theAgent);
			delete theAgent;
			agent_ptr_iter = m_agents.erase(agent_ptr_iter);
			continue;
		}

		SQUAD_CLASS old_class       = theAgent->Get_Squad_Class();
		SQUAD_CLASS new_class       = theAgent->Compute_Squad_Class();

		if(old_class != new_class)
		{
			Remove_Matches_For_Agent(theAgent);
			Add_New_Matches_For_Agent(theAgent);
		}

		agent_ptr_iter++;
	}

	DPRINTF(k_DBG_AI, ("//       elapsed time = %d ms\n", (GetTickCount() - t1)));
}

//////////////////////////////////////////////////////////////////////////
//
//  Reset_Agent_Execution
//
//  When: After agents Change
//
//  Iterate: m_agents
//
//  make agents available for goals
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Reset_Agent_Execution()
{
	Agent_List::iterator agent_ptr_iter = m_agents.begin();
	while(agent_ptr_iter != m_agents.end())
	{
		(*agent_ptr_iter)->Set_Can_Be_Executed(true);
		agent_ptr_iter++;
	}

	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		(*goal_iter)->Set_Needs_Transporter(false);
	}
}

//////////////////////////////////////////////////////////////////////////
//
//  Process_Goal_Changes
//
//  When: Goals Change
//
//  Iterate: sorted_goals_of_class, pruned_goals_of_class
//
//  1. Prioritize_Goals
//
//  2. Prune_Goals
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Process_Goal_Changes()
{
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t// PRIORITIZE GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));

	SetContactCache(m_playerId);
	SetIsNeutralRegardCache(m_playerId);
	SetIsAllyRegardCache(m_playerId);

	Prioritize_Goals();

	SetContactCache(-1);
	SetIsNeutralRegardCache(-1);
	SetIsAllyRegardCache(-1);

	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t// PRUNE GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));

	Prune_Goals();

	m_neededAgentStrength       = Squad_Strength(0);
}

//////////////////////////////////////////////////////////////////////////
//
//  Sort_Goals
//
//  When: Matches Change
//
//  Iterate: m_gaols Goal_List
//
//  1. For each gaol, compute the utility value between the
//     goal and each component agent in the agent.
//
//  2. Compute average match value from agent utility values and
//     goal raw priority.
//
//  3. Sort gaols list.
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Sort_Goals()
{
	Goal_List::iterator goal_iter;

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1, ("\n"));
	time_t t1 = GetTickCount();

#if defined(_DEBUG)
	sint32 size = m_goals.size();
#endif // _DEBUG

	m_goals.sort(std::greater<Goal_ptr>());

#if defined(_DEBUG)
	Assert(m_goals.size() == size);
	if(m_goals.size() != size)
	{
		c3errors_ErrorDialog("List Sort Error", "You compiled the game on MSVC++ 6.0 with the \ndefault standart library, please used the fixed version in your include directories from \n\\ctp2_code\\compiler\\msvc6\\stlfixes");
	}
#endif // _DEBUG

	time_t t2 = GetTickCount();
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
		("//  %d goals sorted = %d ms, Player %d\n\n\n", m_goals.size(), (t2 - t1), m_playerId));
	DPRINTF(k_DBG_AI,("//  %d goals sorted = %d ms, Player %d\n\n\n", m_goals.size(), (t2 - t1), m_playerId));

#if defined(_DEBUG) || defined(USE_LOGGING)

#ifdef _DEBUG_SCHEDULER

	// Looks like something Calvitix added
	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		const Agent_List & agent_list = (*agent_iter)->Get_Agent_List();
		for
		(
		    Agent_List::const_iterator agent_iter  = agent_list.begin();
		                               agent_iter != agent_list.end();
		                             ++agent_iter
		)
		{
			if ((*agent_iter)->Get_Is_Used())
			{
				bool NOT_ALL_AGENTS_ROLLED_BACK = false;
				Assert(NOT_ALL_AGENTS_ROLLED_BACK);
			}
		}
	}
#endif // _DEBUG_SCHEDULER

	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("// SORTED GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));

	sint32 count = 0;
	for
	(
	      goal_iter  = m_goals.begin();
	      goal_iter != m_goals.end();
	    ++goal_iter
	)
	{
		Goal_ptr goal = static_cast<Goal_ptr>(*goal_iter);

		Utility value = goal->Get_Matching_Value();

		if (value > Goal::BAD_UTILITY)
		{
			GOAL_TYPE goal_type = goal->Get_Goal_Type();
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
				("\t[%d] goal_prioriry=%d %s\n",
					count++, value, g_theGoalDB->Get(goal_type)->GetNameText()));

			// Match and agent go inside goal log
			goal->Log_Debug_Info(k_DBG_SCHEDULER_ALL);
		}
		else
		{
			GOAL_TYPE goal_type = goal->Get_Goal_Type();
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
				("\t[%d] goal_bad_prioriry=%d %s, %d\n",
					count++, value, g_theGoalDB->Get(goal_type)->GetNameText(), Goal::BAD_UTILITY));

			// Match and agent go inside goal log
			goal->Log_Debug_Info(k_DBG_SCHEDULER_ALL);
		}
	}

	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("// SORTED GOALS END\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));

#endif
}

//////////////////////////////////////////////////////////////////////////
//
//  Match_Resources
//
//  When: Goals need to be executed (CtpAi_ProcessMatchesEvent)
//
//  Iterate: matches
//
//  1. For each match, move agents from donor agent to goal.
//
//  2. If goal is in progress, execute goal and check status.
//
//  3. If goal is completed, create a new agent from agents that
//     completed the goal.
//
//  4. If goal failed, rollback agents to donor agents.
//     Rollback marks agents in matches as invalid for the goal.
//
//  5. When all agents have been committed, check all goals on
//     pruned_goals_class list.  If under committed, rollback
//     all agents to donor agents. If over committed, rollback
//     excess agents to donor agents.
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Match_Resources(const bool move_armies)
{
	bool out_of_transports = false; // this tells us if we have run out of available transports or not
#if defined(_DEBUG) || defined(USE_LOGGING)
	sint32 count = 0;
#endif

	sint32 committed_agents = 0;
	sint32 total_agents     = m_agents.size();

	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		if(committed_agents >= total_agents)
		{
			Assert(committed_agents == total_agents);

			break;
		}

		Goal_ptr goal_ptr        = static_cast<Goal_ptr>(*goal_iter);
		Utility     oldMatchValue   = goal_ptr->Get_Matching_Value();

#if defined(_DEBUG) || defined(USE_LOGGING)
		MapPoint pos = (goal_ptr->Get_Target_Army().m_id == 0 || goal_ptr->Get_Target_Army().IsValid()) ? goal_ptr->Get_Target_Pos() : MapPoint(-1, -1);
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
				("[%d] Starting to match resources to %s: %x (x=%d,y=%d), match %d, %s\n",
					count, g_theGoalDB->Get(goal_ptr->Get_Goal_Type())->GetNameText(), goal_ptr, pos.x, pos.y, oldMatchValue, (g_theWorld->HasCity(pos) ? g_theWorld->GetCity(pos).GetName() : "field")));
		count++;
#endif

		if(oldMatchValue == Goal::BAD_UTILITY)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
					("\t\tGOAL (goal: %x) -- First goal with bad utility - stop matching\n",
						goal_ptr));

			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
			// Assuming that the list is still sorted,
			// and the following has only Goal::BAD_UTILITY
			break;
		}

		Utility newMatchValue = goal_ptr->Compute_Matching_Value(false);

		if(newMatchValue == Goal::BAD_UTILITY)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
				("\t\tGOAL (goal: %x)(agent count: %d) -- Goal with bad utility, trying agian in next cycle.\n",
						goal_ptr, goal_ptr->Get_Agent_Count()));

	//		Assert(goal_ptr->Get_Agent_Count() == 0); // Is still ok
			// City garrison problem
			goal_ptr->Rollback_All_Agents(); // Just roll back but don't report to the build list

			// Actually should be checked in the next cycle, but there still seems to be something wrong.
			Goal_List::iterator tmp_goal_iter = goal_iter;
			--goal_iter;
			m_goals.erase(tmp_goal_iter);

			/*
			// Move to the end
			m_goals.splice
			              (
			               m_goals.end(),
			               m_goals,
			               tmp_goal_iter
			              );
			*/

			continue;
		}

		if(newMatchValue != oldMatchValue)
		{
			goal_ptr->Set_Matching_Value(newMatchValue);

			Goal_List::iterator tmp_goal_iter = goal_iter;
			++tmp_goal_iter;

	//		Assert(goal_ptr->Get_Agent_Count() == 0);

			if(tmp_goal_iter != m_goals.end())
			{
				Utility nextMatchValue = static_cast<Goal_ptr>(*tmp_goal_iter)->Get_Matching_Value();
				if(newMatchValue < nextMatchValue)
				{
					// http://www.cplusplus.com/reference/stl/list/splice.html
					//or use a decrement
					// Sort the goal list, move iterator increment herein back
					tmp_goal_iter = goal_iter;
					--goal_iter;
					goal_ptr->Rollback_All_Agents(); // Just roll back but don't report to the build list
					Reprioritize_Goal(tmp_goal_iter);
					continue;
				}
			}
		}

		goal_ptr->Commit_Agents();

		// Needs to be reconsidered
		if(goal_ptr->Needs_Transporter())
		{
			// Be careful here
			sint16 transNum = goal_ptr->Get_Transporters_Num();
			goal_ptr->Commit_Transport_Agents();

			if(goal_ptr->Get_Transporters_Num() < 1
			|| transNum == goal_ptr->Get_Transporters_Num()
			){
				// Has to be modified for partial transport
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
					("\t\tGOAL_FAILED Not enough transporters (goal: %x)\n", goal_ptr));

				Rollback_Matches_For_Goal(goal_ptr);
				continue;
			}
		}

		committed_agents += goal_ptr->Get_Agent_Count();

		if(goal_ptr->Get_Agent_Count() == 0)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
					("\t\tGOAL (goal: %x) -- No agents were committed, maybe next time. Continuing...\n",
						goal_ptr));
			continue;
		}

		GOAL_RESULT result = move_armies ? goal_ptr->Execute_Task() : GOAL_IN_PROGRESS;

		switch (result)
		{
			case GOAL_ALREADY_MOVED:
			{

				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\t\tGOAL_ALREADY_MOVED (goal: %x)\n", goal_ptr));

				break;
			}
			case GOAL_IN_PROGRESS:
			{
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\t\tGOAL_IN_PROGRESS (goal: %x)\n", goal_ptr));

				break;
			}

			case GOAL_COMPLETE:
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\t\tGOAL_COMPLETE (goal: %x agent: %x)\n", goal_ptr));

				committed_agents -= goal_ptr->Get_Agent_Count();
#if 0
				if(!goal_ptr->Is_Single_Agent())
				{
				}
#endif
				if(goal_ptr->Get_Removal_Time())
				{
					AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
						("\t\tGOAL_COMPLETE (goal: %x) -- Removing matches for goal.\n",
						goal_ptr));
					Remove_Matches_For_Goal(goal_ptr);
				}
				else
				{
					Rollback_Matches_For_Goal(goal_ptr);
				}

				break;
			}
			case GOAL_NEEDS_TRANSPORT:
			{
				// Optimization: If we have previously failed to get a transport, then skip trying to get a transport now:
				if (!out_of_transports)
				{
					AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
						("\t\tGOAL_NEEDS_TRANSPORT (goal: %x)\n", goal_ptr));

					if(!Add_Transport_Matches_For_Goal(goal_ptr))
					{
						out_of_transports = true; // record the fact we could not find a transport
						AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
							("\t\t **NO transporters found. Failing.\n"));
					}
					else
					{
						AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
							("\t\t Transporters found.\n"));
						--goal_iter;
						committed_agents -= goal_ptr->Get_Agent_Count();
						goal_ptr->Rollback_All_Agents(); // No we don't want to report this to the build list
						break;
					}
				} // if out_of_transports is true, we just fail like in the original code
			}
			case GOAL_FAILED:
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
					("\t\tGOAL_FAILED (goal: %x)\n", goal_ptr));

				committed_agents -= goal_ptr->Get_Agent_Count();
				Rollback_Matches_For_Goal(goal_ptr);

				break;
			}
		}
	}

#if defined(_DEBUG)
	sint32 committed_agents_test = 0;

	for
	(
	    Goal_List::iterator goal_iter2  = m_goals.begin();
	                        goal_iter2 != m_goals.end();
	                      ++goal_iter2
	)
	{
		Goal_ptr goal_ptr        = static_cast<Goal_ptr>(*goal_iter2);
		committed_agents_test   += goal_ptr->Get_Agent_Count();
	}

	Assert(committed_agents_test == committed_agents);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Add_New_Goal
//
//
// Add a new goal to the m_goals_of_type Goal_List
//
// New goals are primarily added in CtpAi:
//
// 1) by AddSettleTargets, AddExploreTargets, and AddMiscMapTargets.
//
// 2) by AddGoalsForArmy, which is called whenever a savegame is loaded or an army is created.
//
// 3) by AddForeignerGoalsForCity and AddOwnerGoalsForCity (when a city is created, changes hands,
//    or a savegame is loaded).
//
//    These add every goal : those that are not gamestate possible are filtered out.
//
// 4) Also called by CtpAi_KillCityEvent, CtpAi_NukeCityUnit, CtpAi_ImprovementComplete.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::Add_New_Goal(const Goal_ptr & new_goal)
{
	sint32 goal_type    = new_goal->Get_Goal_Type();

	Sorted_Goal_Iter tmp_goal_iter = 
		m_goals_of_type[goal_type].begin();

	while(tmp_goal_iter != m_goals_of_type[goal_type].end())
	{
		Goal_ptr old_goal = tmp_goal_iter->second;
		if(*old_goal == *new_goal)
		{
			delete new_goal;
			return;
		}
		tmp_goal_iter++;
	}

	m_goals_of_type[goal_type].
		push_back(Sorted_Goal_ptr(Goal::BAD_UTILITY, new_goal));
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Add_New_Agent
//
//
// Add a new agent to the m_agents Agent_List
//
// New agents are created by AddGoalsForArmy, which is called whenever a savegame is loaded
// or an army is created.
//
/////////////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::Add_New_Agent(const Agent_ptr & new_agent)
{
	if(new_agent->Get_Is_Dead())
	{
		Assert(false);
		delete new_agent;
	}
	else
	{
		new_agent->Compute_Squad_Class();
		Add_Agent(new_agent);

		Add_New_Matches_For_Agent(new_agent);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Add_Agent
//
//
// Add a new agent to the m_agents Agent_List
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////
Agent_List::iterator Scheduler::Add_Agent(const Agent_ptr & agent)
{
#ifdef _DEBUG_SCHEDULER

	Agent_List::iterator agent_iter = m_agents.begin();
	while (agent_iter != m_agents.end() && !(*agent_iter)->ContainsArmyIn(agent))
	{
		agent_iter++;
	}
	
	Assert(agent_iter == m_agents.end());
#endif // _DEBUG_SCHEDULER

	return m_agents.insert(m_agents.end(), agent);
}

////////////////////////////////////////////////////////////
//
//  Remove_Goal
//
//  called by Scheduler::Prioritize_Goals() when
//
////////////////////////////////////////////////////////////
Scheduler::Sorted_Goal_Iter Scheduler::Remove_Goal(const Scheduler::Sorted_Goal_Iter & sorted_goal_iter)
{
	GOAL_TYPE goal_type = sorted_goal_iter->second->Get_Goal_Type();

	Remove_Matches_For_Goal(sorted_goal_iter->second);

	delete sorted_goal_iter->second;

	return m_goals_of_type[goal_type].erase(sorted_goal_iter);
}

void Scheduler::Remove_Goals_Type(const GoalRecord *rec)
{
	Sorted_Goal_List & goalList = m_goals_of_type[rec->GetIndex()];
	for
	(
	    Sorted_Goal_Iter sorted_goal_iter  = goalList.begin();
	                     sorted_goal_iter != goalList.end();
	                     sorted_goal_iter = Remove_Goal(sorted_goal_iter)
	)
	{
		// Action in for
	}
}

//----------------------------------------------------------------------------
//
// Name       : Scheduler::GetValueUnsatisfiedGoals
//
// Description: Get the total value of all unsatisfied goals of a given type.
//
// Parameters : type    : type of goal
//
// Globals    : -
//
// Returns    : sint32  : sum of value of the unsatisfied goals of type type.
//
// Remark(s)  : Used in ThreatenedCity_MotivationEvent to trigger MOTIVATION_FEAR_CITY_DEFENSE
//
//----------------------------------------------------------------------------
sint32 Scheduler::GetValueUnsatisfiedGoals(const GOAL_TYPE & type) const 
{
	sint32 total_value = 0;

	if(IsValid(type, m_goals_of_type))
	{
		for
		(
		    Sorted_Goal_List::const_iterator 
		        sorted_goal_iter  = m_goals_of_type[type].begin();
		        sorted_goal_iter != m_goals_of_type[type].end();
		      ++sorted_goal_iter
		)
		{
			Goal_ptr const goal_ptr = 
				static_cast<Goal_ptr const>(sorted_goal_iter->second);

			if(goal_ptr->Get_Invalid()		||
			   goal_ptr->Is_Satisfied()	||
			   goal_ptr->ArmiesAtGoal()
			  )
			{
				// Goal has become invalid or has been satisfied: try next.
			}
			else
			{
				total_value += goal_ptr->Get_Target_Value();
			}
		}
	}

	return total_value;
}

//----------------------------------------------------------------------------
//
// Name       : Scheduler::GetHighestPriorityGoal
//
// Description: Get the highest valued goal of a given type.
//
// Parameters : type        : type of goal
//              satisfied   : consider satisfied/unsatisfied goals only
//
// Globals    : -
//
// Returns    : sint32	: sum of value of the unsatisfied goals of type type.
//
// Remark(s)  : Returns the first found goal that matches the parameters.
//              The list m_goals_of_type[type] is ordered by raw priority.
//
//              Used in governor to give advice on what city to defend
//              and in ThreatenedCity_MotivationEvent to trigger
//              MOTIVATION_FEAR_CITY_DEFENSE
//
//----------------------------------------------------------------------------
Goal_ptr Scheduler::GetHighestPriorityGoal(const GOAL_TYPE & type, const bool satisfied) const 
{
	if(IsValid(type, m_goals_of_type)) 
	{
		for
		(
		    Sorted_Goal_List::const_iterator
		        sorted_goal_iter  = m_goals_of_type[type].begin();
		        sorted_goal_iter != m_goals_of_type[type].end(); 
		      ++sorted_goal_iter
		)
		{
			Goal_ptr	const	goal_ptr = 
				static_cast<Goal_ptr const>(sorted_goal_iter->second);

			if(goal_ptr->Get_Invalid()                  ||
			    (satisfied != goal_ptr->Is_Satisfied()) ||
			    goal_ptr->ArmiesAtGoal()
			   )
			{
				// Goal does not match: try next.
			}
			else
			{
				return goal_ptr;
			}
		}
	}

	return NULL; // No matching goal available.
}

//////////////////////////////////////////////////////////////////////////
//
// CountGoalsOfType
//
// Used in ctpai when adding Explore, Settle, and MiscMap targets for goals
//
///////////////////////////////////////////////////////////////////////////
sint16 Scheduler::CountGoalsOfType(const GOAL_TYPE & type) const
{
	return IsValid(type, m_goals_of_type) 
	       ? m_goals_of_type[type].size() 
	       : 0;
}

//////////////////////////////////////////////////////////////////////////
//
//  Prioritize_Goals
//
//  Called by Process_Goal_Changes
//
//
//  1. Add new_goals into appropriate goals_of_class list.
//
//  2. Remove all invalid/complete goals from goals_of_class list.
//
//  3. Recompute raw priority of all new/changed goals from
//     sorted_goals_of_class list.
//
//  4. Sort goals_of_class lists.
//
//////////////////////////////////////////////////////////////////////////
bool Scheduler::Prioritize_Goals()
{
	time_t t1 = GetTickCount();

	bool first_turn_of_war = Diplomat::GetDiplomat(m_playerId).FirstTurnOfWar();

	Sorted_Goal_Iter sorted_goal_iter;

	sint32 goal_type;

	for(goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("// Calculate raw priority for %s (%d)\n", g_theGoalDB->Get(goal_type)->GetNameText(), m_goals_of_type[goal_type].size()));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
			("\t %9x,\tGOAL,\t\tRAW_PRIORITY,\t\tCOORDS,\t\tINIT_VALUE,\tLAST_VALUE,\tTHREAT,\t\tENEMYVAL,\tALLIEDVAL,\tMAXPOW,\t\tHOMEDIST\t(   ),\t\tENEMYDIST (    ),\t\tSETTLE,\t\tCHOKE,\t\tUNEXPLORED,\tNOT_VISIBLE,\tIN_HOME_TER,\tIN_ENEMY_TER,\tON_NEUT_TER,\tSLAVERY_PROTECTION,\tSMALL_CITY,\tTHREATEN,\tCONNECTED,\tSMALL_EMP,\tWEAKEST\n",
		this));

		      sorted_goal_iter  = m_goals_of_type[goal_type].begin();
		while(sorted_goal_iter != m_goals_of_type[goal_type].end())
		{
			Goal_ptr goal_ptr = sorted_goal_iter->second;
#if 0
			// Get_Totally_Complete also called in Compute_Raw_Priority
			if(goal_ptr->Get_Totally_Complete())
			{
				if(!goal_ptr->Is_Single_Agent())
				{
				}
			}
#endif

			if(goal_ptr->Get_Removal_Time())
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, 
					("\tRemoving Invalid/Completed Goal: %x\n", goal_ptr));

				sorted_goal_iter = Remove_Goal(sorted_goal_iter);
			}
			else
			{
				goal_ptr->Set_Can_Be_Executed(true);
				goal_ptr->Set_Needs_Transporter(false);
				sorted_goal_iter->first = goal_ptr->Compute_Raw_Priority();

				if(sorted_goal_iter->first > Goal::BAD_UTILITY)
				{
					goal_ptr->Compute_Needed_Troop_Flow();

					if
					  (
					       first_turn_of_war
					    || m_playerId == 0
					    || goal_ptr->CanGoalBeReevaluated()
					    || goal_ptr->Commited_Agents_Need_Orders()
					  )
					{
						goal_ptr->Rollback_All_Agents();
					}
				}
				else
				{
					if(goal_ptr->Get_Matches_Num() > 0)
					{
						Remove_Matches_For_Goal(goal_ptr);
					}
				}

				sorted_goal_iter++;
			}
		}
	}

	time_t t2 = GetTickCount();
#if defined(_DEBUG)
	time_t t = t2 - t1;
#endif
	DPRINTF(k_DBG_AI, ("//  Raw goal priorities calculated:\n"));
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n\n", (t2 - t1)  ));
	t1 = GetTickCount();

	Goal_Vector::iterator generic_goal_iter = m_generic_goals.begin();

	for(goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("// Calculate priority for %s (%d)\n", g_theGoalDB->Get(goal_type)->GetNameText(), m_goals_of_type[goal_type].size()));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n"));
//		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
//		("\t %9x,\tGOAL\t\t,\tRAW_PRIORITY,\tCOORDS\t,\tINIT_VALUE,\tLAST_VALUE,\tTHREAT,\t\tENEMYVAL,\tALLIEDVAL,\tMAXPOW,\t\tHOMEDIST\t(   )\t,\tENEMYDIST (    ),\t\tSETTLE,\t\tCHOKE,\t\tUNEXPLORED,\tNOT_VISIBLE,\tTHREATEN\n",
//		this));

		Remove_Matches_For_Goal(*generic_goal_iter);
		Add_New_Matches_For_Goal(*generic_goal_iter, false);

		for
		(
		      sorted_goal_iter  = m_goals_of_type[goal_type].begin();
		      sorted_goal_iter != m_goals_of_type[goal_type].end();
		    ++sorted_goal_iter
		)
		{
			Goal_ptr goal_ptr = sorted_goal_iter->second;

			if(sorted_goal_iter->first > Goal::BAD_UTILITY)
			{
				if(goal_ptr->Get_Matches_Num() > 0)
				{
					sorted_goal_iter->first = goal_ptr->Compute_Matching_Value();

					if(sorted_goal_iter->first <= Goal::BAD_UTILITY)
					{
						if(goal_ptr->Get_Matches_Num() > 0)
						{
							Remove_Matches_For_Goal(goal_ptr);
						}
					}
				}
				else if((*generic_goal_iter)->Get_Matches_Num() > 0)
				{
					sorted_goal_iter->first = goal_ptr->Compute_Matching_With_Generic_Matches(*generic_goal_iter);
				}
				else
				{
					// In this case there is no need to calculate anything
					sorted_goal_iter->first = Goal::BAD_UTILITY;
				}
			}
		}

		m_goals_of_type[goal_type].sort(std::greater<Sorted_Goal_ptr>());

		++generic_goal_iter;
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\n"));

		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t// SORTED GOALS of %s (%d)\n", g_theGoalDB->Get(goal_type)->GetNameText(), m_goals_of_type[goal_type].size()));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\n"));
	}

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  Goals sorted:\n"));
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n\n", (t2 - t1)  ));

#if defined(_DEBUG)
	t = t2 - t1;
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//  Prune_Goals
//
//  Called by Process_Goals
//
//  IV. pruned_goals_of_class
//
//  1. Iterate over all (sorted) goals in each goal class
//
//  2. Count out maximum number of each goal type.
//
//  3. If pruned_goals_count for a particular goal exceedes allowed count,
//     or the goal is complete, then remove matches for that goal
//     and splice onto end of list.
//
//  4. Otherwise, if not matches exist for this goal, add them.
//
//////////////////////////////////////////////////////////////////////////
bool Scheduler::Prune_Goals()
{
	time_t t1 = GetTickCount();

	sint16 max_eval;
	sint16 max_exec;

	const StrategyRecord &strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	m_goals.resize(0);

	Goal_Vector::iterator generic_goal_iter = m_generic_goals.begin();

	for(sint32 i = 0; i < strategy.GetNumGoalElement(); i++)
	{
		time_t t1 = GetTickCount();

		const StrategyRecord::GoalElement* goal_element_ptr = strategy.GetGoalElement(i);

		GOAL_TYPE goal_type = GetMaxEvalExec(goal_element_ptr, max_eval, max_exec);

//		max_eval = m_goals_of_type[goal_type].size(); // Temporary, maybe permanetly

		Sorted_Goal_Iter pruned_goal_iter = m_goals_of_type[goal_type].end();
		Sorted_Goal_Iter goal_ptr_iter    = m_goals_of_type[goal_type].begin();

#if defined(_DEBUG) || defined(USE_LOGGING)
		sint32 count = 0;

		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\n\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t// PRUNED GOALS: %s (type %d)\n",
			g_theGoalDB->Get(goal_type)->GetNameText(),
			goal_type));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
			("\t// max eval = %3.2f, max_exec = %3.2f",
			goal_element_ptr->GetMaxEval(),
			goal_element_ptr->GetMaxExec()));
		if(goal_element_ptr->GetExecPerCity())
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
				(" (ExecPerCity)"));
		if(goal_element_ptr->GetEvalPerCity())
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
				(" (EvalPerCity)"));
		if(goal_element_ptr->GetPerCity())
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
				(" (PerCity)"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
			("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
			("\t//\n\n"));
#endif

		sint32 goals_added = 0;

		while(goal_ptr_iter != pruned_goal_iter &&
		      goal_ptr_iter != m_goals_of_type[goal_type].end()
		){
			Goal_ptr goal_ptr     = goal_ptr_iter->second;
			Utility  raw_priority = goal_ptr_iter->first;

			bool ok_to_match_goal = (goals_added < max_eval);

			ok_to_match_goal &= (raw_priority != Goal::BAD_UTILITY);

			if(ok_to_match_goal)
			{
				if(goal_ptr->Get_Matches_Num() == 0)
				{
					goal_ptr->Copy_Insert_Matches(m_generic_goals[goal_type]);
				}

				m_goals.push_back(goal_ptr);

				goals_added++;
				goal_ptr_iter++;
			}
			else
			{
				if(goal_ptr->Get_Matches_Num() > 0)
				{
					Remove_Matches_For_Goal(goal_ptr);
				}

#if defined(_DEBUG) || defined(USE_LOGGING)
				AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
					("\t%3d: [%x]", count, goal_ptr_iter->second));
				goal_ptr_iter->second->Log_Debug_Info(k_DBG_SCHEDULER_ALL);

				count++;
#endif

				goal_ptr_iter++;
			}
		}
	}

	DPRINTF(k_DBG_AI, ("//  PRUNE goals based on max eval and max exec:\n"));
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n\n", GetTickCount() - t1 ));

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//   Add_New_Matches_For_Goal
//
//   called by Scheduler::Prune_Goals() when iterating through the Sorted_Goal (by raw priority)
//   list m_goals_of_type[goal_type]
//
//   If it's ok to match the goal (because so far there's less than maxeval of them, and
//   it's raw_priority != Goal::BAD_UTILITY), it iterates through the Agent_List m_agents
//   and calls Add_New_Match_For_Goal_And_Agent to try and add a 3-tuple <goal,agent,matching value>,
//   for each agent that qualifies, to the plan
//
///////////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::Add_New_Matches_For_Goal
(
    const Goal_ptr & goal_ptr,
    const bool       update_match_value
)
{
	sint32      count            = 0;
	GOAL_TYPE   type             = goal_ptr->Get_Goal_Type();
	SQUAD_CLASS goal_squad_class = g_theGoalDB->Get(type)->GetSquadClass();

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		Agent* agent = (*agent_iter);
		if((goal_squad_class & agent->Get_Squad_Class()) != goal_squad_class)
			continue;

		goal_ptr->Add_Match(agent, update_match_value);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//   Add_New_Matches_For_Agent
//
//   called by Process_Agent_Changes
//
//////////////////////////////////////////////////////////////////////////////////////////////
void Scheduler::Add_New_Matches_For_Agent
(
    const Agent_ptr & agent
)
{
	sint32 count = 0;

	SQUAD_CLASS squad_class = agent->Get_Squad_Class();

	for(sint32 i = 0; i < g_theGoalDB->NumRecords(); i++)
	{
		Sorted_Goal_List & goal_list = m_goals_of_type[i];

		SQUAD_CLASS goal_squad_class = 
				  g_theGoalDB->Get(i)->GetSquadClass();

		if((goal_squad_class & squad_class) != goal_squad_class)
			continue;

		for
		(
		    Sorted_Goal_Iter goal_iter  = goal_list.begin();
		                     goal_iter != goal_list.end();
		                   ++goal_iter
		){
			if(goal_iter->second->Get_Matches_Num() == 0)
				continue;

			if(goal_iter->second->Get_Invalid())
				continue;

			goal_iter->second->Add_Match(agent);
		}
	}
}

void Scheduler::Remove_Matches_For_Goal
(
    const Goal_ptr & goal_ptr
)
{
	goal_ptr->Remove_Matches();
}


void Scheduler::Remove_Matches_For_Agent
(
    const Agent_ptr & agent
)
{
	for(sint32 i = 0; i < g_theGoalDB->NumRecords(); i++)
	{
		Sorted_Goal_List & goal_list = m_goals_of_type[i];

		for
		(
		    Sorted_Goal_Iter goal_iter  = goal_list.begin();
		                     goal_iter != goal_list.end();
		                   ++goal_iter
		){

			goal_iter->second->Remove_Match(agent);
		}
	}
}

void Scheduler::Rollback_Matches_For_Goal
(
    const Goal_ptr & goal
)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("ROLLBACK_MATCHES_FOR_GOAL\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t"));
	goal->Log_Debug_Info(k_DBG_SCHEDULER);
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
#endif

	Squad_Strength needed_strength = 
		goal->Get_Strength_Needed();

#if defined(_DEBUG) || defined(USE_LOGGING)
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, 
		("\t\tMissing attack = %3.0f\n",needed_strength.Get_Attack()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, 
		("\t\tMissing transport = %d\n",needed_strength.Get_Transport()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, 
		("\t\tMissing defense = %3.0f\n",needed_strength.Get_Defense()));

#endif

	m_neededAgentStrength.Set_To_The_Maximum(needed_strength);

	AI_DPRINTF(k_DBG_SCHEDULER,  m_playerId, -1, -1, ("\t%d agents to roll back for Goal %x, %s.\n", goal->Get_Agent_Count(), goal, g_theGoalDB->Get(goal->Get_Goal_Type())->GetNameText()));

	goal->Rollback_All_Agents();
}

bool Scheduler::Add_Transport_Matches_For_Goal
(
    const Goal_ptr & goal_ptr
)
{
	Assert(goal_ptr->Needs_Transporter());

	bool match_added = false;

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		Agent* agent = (*agent_iter);
		if ( (k_Goal_SquadClass_CanTransport_Bit & agent->Get_Squad_Class()) !=
			  k_Goal_SquadClass_CanTransport_Bit )
			  continue;

		if(goal_ptr->Cannot_Be_Used(agent)) // Failed transporters
		{
			continue;
		}

		bool hasMatch  = goal_ptr->Has_Agent_And_Set_Needs_Cargo(agent);
		match_added   |= hasMatch;

		if(hasMatch)
		{
			continue;
		}

		sint32 transports = 0;
		sint32 max        = 0;
		sint32 empty      = 0;

		if(!agent->Has_Any_Goal())
		{
			agent->Get_Army()->GetCargo(transports, max, empty);
		}

		if(empty > 0
		&& max == empty // must be empty otherwise the reansporter would go somewhere else or is alread in the agent list
		&& goal_ptr->CanReachTargetContinent(agent)
		&& goal_ptr->Add_Transport_Match(agent)
		){
			match_added = true;
		}
	}

	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t\tRecomputing matches for transport\n"));
	goal_ptr->Compute_Matching_Value(false);

	return match_added;
}

void Scheduler::Reprioritize_Goal(Goal_List::iterator &goal_iter)
{
	Goal_List::iterator goal_ptr_iter = goal_iter;
	Utility matchValue = static_cast<Goal_ptr>(*goal_ptr_iter)->Get_Matching_Value();

	++goal_ptr_iter;

	Goal_List tmp_list;
	tmp_list.splice(tmp_list.begin(), m_goals, goal_iter);


	while(goal_ptr_iter != m_goals.end())
	{
		Utility tmpValue = static_cast<Goal_ptr>(*goal_ptr_iter)->Get_Matching_Value();
		if(matchValue >= tmpValue)
		{
			m_goals.splice(goal_ptr_iter, tmp_list);
			break;
		}
		else
		{
			++goal_ptr_iter;
		}
	}
}

GOAL_TYPE Scheduler::GetMaxEvalExec(const StrategyRecord::GoalElement *goal_element_ptr, sint16 & max_eval, sint16 & max_exec)
{
	GOAL_TYPE goal_type = goal_element_ptr->GetGoalIndex();
	Assert(goal_type >= 0);
	Assert(goal_type < g_theGoalDB->NumRecords());

	
	double tmp_eval = goal_element_ptr->GetMaxEval();
	double tmp_exec = goal_element_ptr->GetMaxExec();
	
	if (goal_element_ptr->GetEvalPerCity() ||
		goal_element_ptr->GetPerCity())
		tmp_eval *= g_player[m_playerId]->GetNumCities();

	
	if (goal_element_ptr->GetExecPerCity() ||
		goal_element_ptr->GetPerCity())
		tmp_exec *= g_player[m_playerId]->GetNumCities();

	max_eval = (sint16) floor(tmp_eval);
	max_exec = (sint16) floor(tmp_exec);

	return goal_type;
}


void Scheduler::DisbandObsoleteArmies(const sint16 max_count)
{
	sint32 count = 0;

	Agent_List::iterator agent_ptr_iter = m_agents.begin();
	while (agent_ptr_iter != m_agents.end() && count < max_count)
	{
		if ((*agent_ptr_iter)->DisbandObsoleteArmies() > 0)
			count++;
		agent_ptr_iter++;
	}
}

Squad_Strength Scheduler::GetMostNeededStrength() const
{
	return m_neededAgentStrength;
}

void Scheduler::SetArmyDetachState(const Army & army, const bool detach)
{
	Agent_List::iterator agent_ptr_iter = m_agents.begin();
	bool found = false;
	while(agent_ptr_iter != m_agents.end() && !found)
	{
		Agent_ptr ctp_agent = (*agent_ptr_iter);
		if(ctp_agent->Get_Army().m_id == army.m_id)
		{
			ctp_agent->Set_Detached(detach);
			found = true;
			break;
		}

		agent_ptr_iter++;
	}
}

bool Scheduler::GetArmyDetachState(const Army & army) const
{
	Agent_List::const_iterator agent_ptr_iter = m_agents.begin();

	while(agent_ptr_iter != m_agents.end())
	{
		const Agent_ptr ctp_agent = (*agent_ptr_iter);
		if (ctp_agent->Get_Army().m_id == army.m_id)
		{
			return ctp_agent->Get_Detached();
		}

		agent_ptr_iter++;
	}
	return false;
}

void Scheduler::SetContactCache(sint32 player)
{
	m_contactCachedPlayer = player;

	if (player < 0) return;

	m_contactCache        = 0;

	for(sint32 i = 0; i < k_MAX_PLAYERS; ++i)
	{
		if(i==player) continue;

		if(g_player[player]->HasContactWith(i))
		{
			m_contactCache |= (1<<i);
		}
	}
}

bool Scheduler::CachedHasContactWithExceptSelf(sint32 player1, sint32 player2)
{
	if (player1 == m_contactCachedPlayer)
	{

#ifdef _DEBUG

	//	int test1 = g_player[player1]->HasContactWith(player2) && (player1!=player2);
	//	int test2 = ((m_contactCache>>player2) & 1) != 0;
	//	Assert(test1==test2);
#endif

		return ((m_contactCache>>player2)&1) != 0;
	}

	if (g_player[player1] == NULL)
		return false;

	if (player1==player2) return false;
	return g_player[player1]->HasContactWith(player2);
}

void Scheduler::SetIsNeutralRegardCache(sint32 player)
{
	m_neutralRegardCachedPlayer = player;

	if (player < 0) return;

	m_neutralRegardCache        = 0;

	Diplomat & diplomat = Diplomat::GetDiplomat(player);

	for(sint32 i = 0; i < AgreementMatrix::s_agreements.GetMaxPlayers(); ++i)
	{
		if(diplomat.TestEffectiveRegard(i, NEUTRAL_REGARD))
		{
			m_neutralRegardCache |= (1<<i);
		}
	}
}

bool Scheduler::CachedIsNeutralRegard(sint32 player, sint32 opponent)
{
	if (player == m_neutralRegardCachedPlayer)
	{

#ifdef _DEBUG

	//	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	//	int test1 = diplomat.TestEffectiveRegard(opponent, NEUTRAL_REGARD);
	//	int test2 = ((m_neutralRegardCache>>opponent) & 1) != 0;
	//	Assert(test1==test2);
#endif
		return ((m_neutralRegardCache>>opponent)&1) != 0;
	}

	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	return diplomat.TestEffectiveRegard(opponent, NEUTRAL_REGARD);
}

void Scheduler::SetIsAllyRegardCache(sint32 player)
{
	m_allyRegardCachedPlayer = player;

	if (player < 0) return;

	m_allyRegardCache        = 0;

	Diplomat & diplomat = Diplomat::GetDiplomat(player);

	for(sint32 i = 0; i < AgreementMatrix::s_agreements.GetMaxPlayers(); ++i)
	{
		if(diplomat.TestAlliedRegard(i))
		{
			m_allyRegardCache |= (1<<i);
		}
	}
}

bool Scheduler::CachedIsAllyRegard(sint32 player, sint32 ally)
{
	if (player == m_allyRegardCachedPlayer)
	{

#ifdef _DEBUG
		
	//	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	//	int test1 = diplomat.TestAlliedRegard(ally);
	//	int test2 = ((m_allyRegardCache>>ally) & 1) != 0;
	//	Assert(test1==test2);
#endif
		return ((m_allyRegardCache>>ally)&1) != 0;
	}

	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	return diplomat.TestAlliedRegard(ally);
}

void Scheduler::Recompute_Goal_Strength()
{
	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		Goal_ptr theGoal = static_cast<Goal_ptr>(*goal_iter);

		theGoal->Recompute_Current_Attacking_Strength();
	}
}

void Scheduler::Compute_Agent_Strength()
{
	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		(*agent_iter)->Compute_Squad_Strength();
	}
}

void Scheduler::Rollback_Emptied_Transporters()
{
	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		(*goal_iter)->Rollback_Emptied_Transporters();
	}
}

void Scheduler::Sort_Goal_Matches_If_Necessary()
{
	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		(*goal_iter)->Sort_Matches_If_Necessary();
	}
}

void Scheduler::Assign_Garrison()
{
	sint32 cityNum = g_player[m_playerId]->GetNumCities();
	Sorted_Agent_List_Vector garrisonAgents;

	garrisonAgents.resize(cityNum);

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		Agent_ptr agent = (*agent_iter);

		agent->SetIsNeededForGarrison(false);

		Army army = agent->Get_Army();

		if(!army.IsValid())
			continue;

		if(army->NumOrders() > 0)
			continue;

		if((army->GetMovementType() &
		     (k_Unit_MovementType_Land_Bit | k_Unit_MovementType_Mountain_Bit)) == 0)
		{
			continue;
		}

		MapPoint pos = army->RetPos();
		Unit city = g_theWorld->GetCity(pos);
		if(city.m_id == 0)
			continue;

		sint32 transports,max,empty;
		if(army->GetCargo(transports, max, empty))
			continue;

		if(
		   ((agent->Get_Squad_Class() & k_Goal_SquadClass_CanDefend_Bit   ) != 0x0 ) &&
		   ((agent->Get_Squad_Class() & k_Goal_SquadClass_Special_Bit     ) == 0x0 ) &&
		   ((agent->Get_Squad_Class() & k_Goal_SquadClass_CanTransport_Bit) == 0x0 )
		  )
		{

			sint16 defense_count;
			sint16 tmp_count;
			float  tmp;
			float  defense_strength;
			army->ComputeStrength(tmp,
			                      defense_strength,
			                      tmp,
			                      defense_count,
			                      tmp_count,
			                      tmp,
			                      tmp,
			                      tmp,
			                      tmp,
			                      false
			                     );

			defense_strength += city.GetDefendersBonus() * static_cast<double>(defense_count);

			sint32 idx = -1;
			if(g_player[m_playerId]->GetCityIndex(city, idx))
			{
				garrisonAgents[idx].push_back(Sorted_Agent_ptr(defense_strength, agent));
			}
		}
	}

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("Find armies for garrison\n"));

	for(sint32 i = 0; i < cityNum; ++i)
	{
		Unit city = g_player[m_playerId]->GetCityFromIndex(i);

		garrisonAgents[i].sort();

		sint8  needed_garrison           = city.CD()->GetNeededGarrison();
		double needed_garrison_strength  = city.CD()->GetNeededGarrisonStrength();

		sint8  current_garrison          = 0;
		double current_garrison_strength = 0.0;

		for
		(
		    Sorted_Agent_Iter agent_iter  = garrisonAgents[i].begin();
		                      agent_iter != garrisonAgents[i].end();
		                    ++agent_iter
		)
		{
			if
			  (
			      current_garrison_strength < needed_garrison_strength
			   || current_garrison          < needed_garrison
			  )
			{
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("%9x\t %9x\t %s\n", agent_iter->second, agent_iter->second->Get_Army(), city.GetName()));

				current_garrison_strength += agent_iter->first;
				current_garrison          += agent_iter->second->Get_Army()->Num();

				agent_iter->second->SetIsNeededForGarrison(true);

				if(agent_iter->second->Has_Any_Goal())
				{
					Goal_ptr goal_ptr = agent_iter->second->Get_Goal();

					MapPoint dest_pos = goal_ptr->Get_Target_Pos();     // Get cheap target position first, no need for pillage checking, yet.
					MapPoint curr_pos = agent_iter->second->Get_Pos();

					if(dest_pos != curr_pos) // Agents with other goals have to be freed
					{
						goal_ptr->Rollback_Agent(agent_iter->second);
					}
				}
			}
			else
			{
				if(agent_iter->second->Has_Any_Goal())
				{
					Goal_ptr goal_ptr = agent_iter->second->Get_Goal();

					MapPoint dest_pos = goal_ptr->Get_Target_Pos();     // Get cheap target position first, no need for pillage checking, yet.
					MapPoint curr_pos = agent_iter->second->Get_Pos();

					if(dest_pos == curr_pos) // Agents with goals here have to be freed
					{
						goal_ptr->Rollback_Agent(agent_iter->second);
					}
				}

				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("%9x\t %9x\t %s (Not needed for city garrison)\n", agent_iter->second, agent_iter->second->Get_Army(), city.GetName()));
			}
		}
	}

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("\n"));
}
