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
// - Fixed a crash in Remove_Goal. The goal is also removed from m_goals
//   before deleted. (31-Dec-2018 Martin Gühmann)
// - New goals are only added if the AI can achieve them. (08-Jan-2018 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Scheduler.h"

#include "profileai.h"

#include <algorithm>
#include <list>

#include "Goal.h"
#include "Plan.h"
#include "agent.h"

#include "c3errors.h"

#include "player.h"
#include "Diplomat.h"
#include "CTPDatabase.h"
#include "GoalRecord.h"
#include "ctpaidebug.h"
#include "AgreementMatrix.h"
#include "gfx_options.h"
#include "Army.h"
#include "ArmyData.h"
#include "citydata.h"
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

bool   Scheduler::s_needAnotherCycle          =  false;

void Scheduler::ResizeAll(const PLAYER_INDEX & newMaxPlayerId)
{
	s_theSchedulers.resize(newMaxPlayerId);

	for(sint32 i = 0; i < newMaxPlayerId; ++i)
	{
		s_theSchedulers[i].SetPlayerId(i);
	}
}

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

	Associate_Goals_With_Sub_Goals();
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
	AI_DPRINTF(k_DBG_AI, m_playerId, -1, -1, ("//       Change agent matches\n"));
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
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("Squad Class for agent %9x (%9x) change, replace matches\n", theAgent, theAgent->Get_Army().m_id));
			Remove_Matches_For_Agent(theAgent);
			Add_New_Matches_For_Agent(theAgent);
		}
		else
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1,("Remove invalid matches for agent %9x (%9x) change\n", theAgent, theAgent->Get_Army().m_id));
			Remove_Invalid_Matches_For_Agent(theAgent);
		}

		theAgent->Get_Army()->SetAICanAddOrders();

		agent_ptr_iter++;
	}

	AI_DPRINTF(k_DBG_AI, m_playerId, -1, -1, ("//       elapsed time = %d ms\n", (GetTickCount() - t1)));
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
	time_t  t1 = GetTickCount();

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t// PRIORITIZE GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));

	SetContactCache(m_playerId);
	SetIsNeutralRegardCache(m_playerId);
	SetIsAllyRegardCache(m_playerId);
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  Process goal changes:     Elapsed time = %d ms\n", (GetTickCount() - t1)));

	Raw_Prioritize_Goals(); // Computes raw priorities
	Assign_Garrison();      // For the new garrison asignment, we need the rew priorities
	Prioritize_Goals();     // Computes combined priorities
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  Prioritize goal changes:  Elapsed time = %d ms\n", (GetTickCount() - t1)));

	SetContactCache(-1);
	SetIsNeutralRegardCache(-1);
	SetIsAllyRegardCache(-1);

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t// PRUNE GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));

	Prune_Goals();
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  Prune goals:              Elapsed time = %d ms\n", (GetTickCount() - t1)));

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

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	time_t t1 = GetTickCount();

#if defined(_DEBUG)
	size_t size = m_goals.size();
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
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
		("//  %d goals sorted = %d ms, Player %d\n\n\n", m_goals.size(), (t2 - t1), m_playerId));

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

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("// REPORT SORTED GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));

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
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
				("\t[%d] goal_prioriry=%d %s\n",
					count++, value, g_theGoalDB->Get(goal_type)->GetNameText()));

			// Match and agent go inside goal log
			goal->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL);
		}
		else
		{
			GOAL_TYPE goal_type = goal->Get_Goal_Type();
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
				("\t[%d] goal_bad_prioriry=%d %s, %d\n",
					count++, value, g_theGoalDB->Get(goal_type)->GetNameText(), Goal::BAD_UTILITY));

			// Match and agent go inside goal log
			goal->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL);
		}
	}

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("// REPORT SORTED GOALS END\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));

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

namespace
{
	// Re-matching recomputes every goal's value each cycle purely from
	// which agents are still free, so two near-tied goals sharing the same
	// one or two agents can otherwise evict each other's already-committed
	// agent over noise-level score changes (observed: two
	// GOAL_ESTABLISH_EMBASSY goals endlessly swapping the same two
	// diplomats back and forth on <0.2% deltas, with neither unit ever
	// actually moving). Require a decisive lead before a competing goal is
	// allowed to bump an already-committed agent.
	double const k_MatchValueReprioritizeMargin = 0.02;
}

void Scheduler::Match_Resources(const bool move_armies)
{
	bool out_of_transports = false; // this tells us if we have run out of available transports or not
#if defined(_DEBUG) || defined(USE_LOGGING)
	sint32 count = 0;
#endif

	size_t committed_agents = 0;
	size_t total_agents     = m_agents.size();

#if defined(_DEBUG)
	int loopCount = 0;
	int loopCountBadUtility = -1;
#endif

	//It is best not to iterate using a for loop, as items may be erased as we go through
	Goal_List::iterator goal_iter = m_goals.begin();
	while(goal_iter != m_goals.end())
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
					count, g_theGoalDB->Get(goal_ptr->Get_Goal_Type())->GetNameText(), goal_ptr, pos.x, pos.y, oldMatchValue, Goal::GetTargetName(pos)));
		count++;
#endif

		if(oldMatchValue == Goal::BAD_UTILITY)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\t\tGOAL (goal: %x) -- First goal with bad utility - stop matching\n",
						goal_ptr));

			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
#if defined(_DEBUG)
			loopCountBadUtility = loopCount;
#endif
			// Assuming that the list is still sorted,
			// and the following has only Goal::BAD_UTILITY
			break;
		}

		Utility newMatchValue = goal_ptr->Compute_Matching_Value(false);

		if(newMatchValue == Goal::BAD_UTILITY)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
				("\t\tGOAL (goal: %x)(agent count: %d) -- Goal with bad utility, removing goal from queue, maybe next turn.\n",
						goal_ptr, goal_ptr->Get_Agent_Count()));

			// City garrison problem
			goal_ptr->Rollback_All_Agents(); // Just roll back but don't report to the build list

			// Actually should be checked in the next cycle, but there still seems to be something wrong.
			goal_iter = m_goals.erase(goal_iter);

			continue;
		}

		if(newMatchValue != oldMatchValue)
		{
			goal_ptr->Set_Matching_Value(newMatchValue);

			Goal_List::iterator tmp_goal_iter = goal_iter;
			++tmp_goal_iter;

			if(tmp_goal_iter != m_goals.end())
			{
				Utility nextMatchValue = static_cast<Goal_ptr>(*tmp_goal_iter)->Get_Matching_Value();
				Utility const oldMatchValueAbs = (oldMatchValue < 0) ? -oldMatchValue : oldMatchValue;
				Utility const margin =
				    static_cast<Utility>(oldMatchValueAbs * k_MatchValueReprioritizeMargin);
				if(newMatchValue < nextMatchValue - margin)
				{
					AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					    ("\t\tGOAL (goal: %x)(agent count: %d) -- Match value change (old: %d, new: %d): Rollback agents and reprioritize goal.\n",
					        goal_ptr, goal_ptr->Get_Agent_Count(),
					        oldMatchValue, newMatchValue));
					// http://www.cplusplus.com/reference/stl/list/splice.html
					//or use a decrement
					// Sort the goal list, move iterator increment herein back
					goal_ptr->Rollback_All_Agents(); // Just roll back but don't report to the build list
					goal_iter = Reprioritize_Goal(goal_iter);
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
				goal_iter++;
#if defined(_DEBUG)
				loopCount++;
#endif
				continue;
			}
		}

		committed_agents += goal_ptr->Get_Agent_Count();

		if(goal_ptr->Get_Agent_Count() == 0)
		{
			if(goal_ptr->GetSubGoalCount() == 0)
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
						("\t\tGOAL (goal: %x) -- No agents were committed, maybe next time. Continuing...\n",
							goal_ptr));
			}
			else
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
						("\t\tGOAL (goal: %x) -- Only agents for subgoal were committed, nothing more to do here. Continuing...\n",
							goal_ptr));
			}

			goal_iter++;
#if defined(_DEBUG)
			loopCount++;
#endif
			continue;
		}

		GOAL_RESULT result = move_armies ? goal_ptr->Execute_Task() : GOAL_IN_PROGRESS;

		switch (result)
		{
			case GOAL_ALREADY_MOVED:
			{

				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\t\tGOAL_ALREADY_MOVED (goal: %x)\n", goal_ptr));

				break;
			}
			case GOAL_IN_PROGRESS:
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
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
				if(goal_ptr->IsTimeToRemove())
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
						committed_agents -= goal_ptr->Get_Agent_Count();
						goal_ptr->Rollback_All_Agents(); // No we don't want to report this to the build list
						continue;
					}
				} // if out_of_transports is true, we just fail like in the original code
			}
			case GOAL_FAILED:
			case GOAL_FAILED_TOO_EXPENSIVE:
			case GOAL_FAILED_UNGROUP:
			case GOAL_FAILED_RALLY:
			case GOAL_FAILED_NEEDS_TRANSPORT:
			{
#if defined(_DEBUG) || defined(USE_LOGGING)
				char buffer[255];
				sprintf(buffer, "\t\tGOAL_FAILED");
				switch(result)
				{
					case GOAL_FAILED:
						break;
					case GOAL_FAILED_TOO_EXPENSIVE:
						strcat(buffer, "_TOO_EXPENSIVE");
						break;
					case GOAL_FAILED_UNGROUP:
						strcat(buffer, "_UNGROUP");
						break;
					case GOAL_FAILED_RALLY:
						strcat(buffer, "_RALLY");
						break;
					case GOAL_FAILED_NEEDS_TRANSPORT:
						strcat(buffer, "_TRANSPORT");
						break;
					default:
						break;
				}

				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("%s (goal: %x)\n", buffer, goal_ptr));
#endif
				committed_agents -= goal_ptr->Get_Agent_Count();
				Rollback_Matches_For_Goal(goal_ptr);

				break;
			}
		}
		goal_iter++;
#if defined(_DEBUG)
		loopCount++;
#endif
	}

#if defined(_DEBUG)
	size_t committed_agents_test = 0;
	loopCount = 0;
	Goal_List::iterator goal_iter2 = m_goals.begin();
	for (; goal_iter2 != m_goals.end(); ++goal_iter2)
	{
		Goal_ptr goal_ptr        = static_cast<Goal_ptr>(*goal_iter2);
		committed_agents_test   += goal_ptr->Get_Agent_Count();

		loopCount++;
		if (loopCount == loopCountBadUtility)
			break;
	}

	Assert(committed_agents_test == committed_agents);

	if(committed_agents_test == committed_agents)
		return;

	size_t committed_agents_not_counted = 0;
	for (; goal_iter2 != m_goals.end(); ++goal_iter2)
	{
		Goal_ptr goal_ptr        = static_cast<Goal_ptr>(*goal_iter2);

		if(goal_ptr->Get_Agent_Count() > 0)
		{
			committed_agents_not_counted += goal_ptr->Get_Agent_Count();
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("%d agents not counted from goal: %x)\n", goal_ptr->Get_Agent_Count(), goal_ptr));
			goal_ptr->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL);
		}
	}

	Assert(committed_agents_test + committed_agents_not_counted == committed_agents);
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
void Scheduler::Add_New_Goal(const Goal & new_goal)
{
	if(new_goal.IsComplete())
	{
		return;
	}

	sint32 goal_type    = new_goal.Get_Goal_Type();

	Sorted_Goal_Iter tmp_goal_iter =
		m_goals_of_type[goal_type].begin();

	while(tmp_goal_iter != m_goals_of_type[goal_type].end())
	{
		Goal_ptr old_goal = tmp_goal_iter->second;
		if(*old_goal == new_goal)
		{
			return;
		}
		tmp_goal_iter++;
	}

	m_goals_of_type[goal_type].
		push_back(Sorted_Goal_ptr(Goal::BAD_UTILITY, new Goal(new_goal)));
}

void Scheduler::Associate_Goals_With_Sub_Goals()
{
	for(sint32 goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		sint32 sub_goal = -1;
		if(!g_theGoalDB->Get(goal_type)->GetSubGoalIndex(sub_goal))
			continue;

		Assert(m_goals_of_type[goal_type].size() == m_goals_of_type[sub_goal].size());

		if(m_goals_of_type[goal_type].size() != m_goals_of_type[sub_goal].size())
			continue;

		Sorted_Goal_Iter tmp_goal_iter =
			m_goals_of_type[goal_type].begin();

		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
			("\tAcossiate goals of type %s with subgoals of type %s\n", g_theGoalDB->Get(goal_type)->GetNameText(), g_theGoalDB->Get(sub_goal)->GetNameText()));

		while(tmp_goal_iter != m_goals_of_type[goal_type].end())
		{
			Sorted_Goal_Iter sub_goal_iter =
				m_goals_of_type[sub_goal].begin();

			Goal_ptr the_goal = tmp_goal_iter->second;

			// Goal and subgoal may not be in the same order
			while(sub_goal_iter != m_goals_of_type[sub_goal].end())
			{
				Goal_ptr sub_goal = sub_goal_iter->second;

				MapPoint     tagetPos = the_goal->Get_Target_Pos();
				MapPoint subTaregtPos = sub_goal->Get_Target_Pos();

				if(tagetPos == subTaregtPos)
				{
					AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
						("\tAcossiate %x @ %s (%d, %d) with %x @ %s (%d, %d) \n", the_goal, the_goal->GetTargetName(), tagetPos.x, tagetPos.y, sub_goal, sub_goal->GetTargetName(), subTaregtPos.x, subTaregtPos.y));
					the_goal->SetSubGoal(sub_goal);
					break;
				}

				sub_goal_iter++;
			}

			tmp_goal_iter++;
		}
	}
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
//  Add_Agent
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
//  called by CtpAi::AddSettleTargets
//
////////////////////////////////////////////////////////////
void Scheduler::Remove_Goal(const Goal_ptr & goal)
{
	sint32 goal_type = goal->Get_Goal_Type();

	Sorted_Goal_Iter tmp_goal_iter =
		m_goals_of_type[goal_type].begin();

	while (tmp_goal_iter != m_goals_of_type[goal_type].end())
	{
		Goal_ptr old_goal = tmp_goal_iter->second;
		if (*old_goal == *goal)
		{
			Remove_Goal(tmp_goal_iter);
			return;
		}
		tmp_goal_iter++;
	}
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

	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		if (*(*goal_iter) == (*sorted_goal_iter->second))
		{
			m_goals.erase(goal_iter);
			break;
		}
	}

	delete sorted_goal_iter->second;

	return m_goals_of_type[goal_type].erase(sorted_goal_iter);
}

////////////////////////////////////////////////////////////
//
//  Remove_Goals_Type
//
//  called by CtpAi::AddSettleTargets
//
////////////////////////////////////////////////////////////
void Scheduler::Remove_Goals_Type(const GOAL_TYPE & type)
{
	Sorted_Goal_List & goalList = m_goals_of_type[type];
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

			if(goal_ptr->IsInvalid()
			|| goal_ptr->Is_Satisfied()
			|| goal_ptr->ArmiesAtGoal()
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

			if(goal_ptr->IsInvalid()                    ||
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
size_t Scheduler::CountGoalsOfType(const GOAL_TYPE & type) const
{
	return IsValid(type, m_goals_of_type)
	       ? m_goals_of_type[type].size()
	       : 0;
}

//////////////////////////////////////////////////////////////////////////
//
//  Raw_Prioritize_Goals
//
//  Called by Process_Goal_Changes
//
//
//  1. Add new_goals into appropriate goals_of_class list.
//
//  2. Remove all invalid/complete goals from goals_of_class list.
//
//  3. Remove old matches and add new matches for generic goals
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Raw_Prioritize_Goals()
{
	time_t t1 = GetTickCount();

	bool first_turn_of_war = Diplomat::GetDiplomat(m_playerId).FirstTurnOfWar();

	Sorted_Goal_Iter sorted_goal_iter;

	for(sint32 goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("// Calculate raw priority for %d goals of %s\n", m_goals_of_type[goal_type].size(), g_theGoalDB->Get(goal_type)->GetNameText()));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
			("\t %9x,\tGOAL,\t\tRAW_PRIORITY,\t\tCOORDS,\t\tINIT_VALUE,\t\tLAST_VALUE,\t\tTHREAT,\t\tENEMYVAL,\tALLIEDVAL,\tMAXPOW,\t\tHOMEDIST\t   (       ),\tENEMYDIST    (       ),\t\tSETTLE,\t\tCHOKE,\t\tUNEXPLORED,\tNOT_VISIBLE,\tIN_HOME_TER,\tIN_ENEMY_TER,\tON_NEUT_TER,\tSLAVERY_PROTECTION,\tSMALL_CITY,\tTHREATEN,\tCONNECTED,\tSMALL_EMP,\tWEAKEST,\tPLACE\n",
				this));

		sorted_goal_iter = m_goals_of_type[goal_type].begin();
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

			if(goal_ptr->IsTimeToRemove())
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\tRemoving Invalid/Completed Goal: %x,\t%s\n", goal_ptr, g_theGoalDB->Get(goal_ptr->Get_Goal_Type())->GetNameText()));

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
						AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("%x\t,%s,\tGoal::BAD_UTILITY,\tRemoving matches\n", goal_ptr, g_theGoalDB->Get(goal_ptr->Get_Goal_Type())->GetNameText()));
						Remove_Matches_For_Goal(goal_ptr);
					}
					else
					{
						AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("%x\t,%s,\tGoal::BAD_UTILITY,\tNo matches to remove\n", goal_ptr, g_theGoalDB->Get(goal_ptr->Get_Goal_Type())->GetNameText()));
					}
				}

				sorted_goal_iter++;
			}
		}
	}

	time_t t2 = GetTickCount();

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  Raw goal priorities calculated:\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  elapsed time = %d ms\n\n", (t2 - t1)));

	t1 = GetTickCount();
	Goal_Vector::iterator generic_goal_iter = m_generic_goals.begin();

	for(sint32 goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{
		Remove_Matches_For_Goal(*generic_goal_iter); // Expensive
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//  Remove generic matches:             Elapsed time = %d ms\n", (t2 - t1)));
		t2 = GetTickCount();
		Add_New_Matches_For_Goal(*generic_goal_iter, false);
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//  Add new generic matches:            Elapsed time = %d ms\n\n", (t2 - t1)));
		t2 = GetTickCount();
		++generic_goal_iter;
	}
}

//////////////////////////////////////////////////////////////////////////
//
//  Prioritize_Goals
//
//  Called by Process_Goal_Changes
//
//  1. Recompute raw priority of all new/changed goals from
//     sorted_goals_of_class list.
//
//  2. Sort goals_of_class lists.
//
//////////////////////////////////////////////////////////////////////////
void Scheduler::Prioritize_Goals()
{
	Sorted_Goal_Iter sorted_goal_iter;
	time_t t1 = GetTickCount();
	time_t t2 = t1;

	Goal_Vector::iterator generic_goal_iter = m_generic_goals.begin();

	for(sint32 goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++)
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

		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//  Before recalculation:       Elapsed time = %d ms\n", (t2 - t1)));
		t2 = GetTickCount();
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//  Recalculate match value:    Elapsed time = %d ms\n", (t2 - t1)));

		m_goals_of_type[goal_type].sort(std::greater<Sorted_Goal_ptr>());
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("//  Sorted goals:               Elapsed time = %d ms\n", (t2 - t1)));
		t2 = GetTickCount();

		++generic_goal_iter;
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));

		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\t// SORTED GOALS of %s (%d)\n", g_theGoalDB->Get(goal_type)->GetNameText(), m_goals_of_type[goal_type].size()));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
	}

	t2 = GetTickCount();
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  Goals sorted:\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  elapsed time = %d ms\n\n", (t2 - t1)  ));
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
//  4. Otherwise, if no matches exist for this goal, add them.
//
//////////////////////////////////////////////////////////////////////////
bool Scheduler::Prune_Goals()
{
	time_t t1 = GetTickCount();

	sint16 max_eval;
	sint16 max_exec;

	const StrategyRecord &strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	m_goals.resize(0); // Empty this earlier or after all the matches, so that the memory is not wasted over the turns

	for(sint32 i = 0; i < strategy.GetNumGoalElement(); i++)
	{
		const StrategyRecord::GoalElement* goal_element_ptr = strategy.GetGoalElement(i);

		GOAL_TYPE goal_type = GetMaxEvalExec(goal_element_ptr, max_eval, max_exec);

		Sorted_Goal_Iter pruned_goal_iter = m_goals_of_type[goal_type].end();
		Sorted_Goal_Iter goal_ptr_iter    = m_goals_of_type[goal_type].begin();

#if defined(_DEBUG) || defined(USE_LOGGING)
		sint32 count = 0;

		char buffer[255];
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\t// PRUNED GOALS: %s (type %d, original num %d)\n",
			g_theGoalDB->Get(goal_type)->GetNameText(),
			goal_type,
			m_goals_of_type[goal_type].size()));

		sprintf(buffer, "\t// max_eval = %3.2f, max_exec = %3.2f max_proc_eval = %i, max_proc_exec = %i ", goal_element_ptr->GetMaxEval(), goal_element_ptr->GetMaxExec(), max_eval, max_exec);

		if(goal_element_ptr->GetExecPerCity())
			strcat(buffer, " (ExecPerCity)");
		if(goal_element_ptr->GetEvalPerCity())
			strcat(buffer, " (EvalPerCity)");
		if(goal_element_ptr->GetPerCity())
			strcat(buffer, " (PerCity)");
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("%s\n", buffer));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1, ("\n"));
#endif

		sint32 goals_evaluated = 0;

		while(goal_ptr_iter != pruned_goal_iter &&
		      goal_ptr_iter != m_goals_of_type[goal_type].end()
		){
			Goal_ptr goal_ptr     = goal_ptr_iter->second;
			Utility  raw_priority = goal_ptr_iter->first;

			bool ok_to_match_goal = (goals_evaluated < max_eval);

			ok_to_match_goal &= (raw_priority != Goal::BAD_UTILITY);

			if(ok_to_match_goal)
			{
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1,
					("\t%3d: [%x] of %s is being evaluated.\n", goals_evaluated, goal_ptr, g_theGoalDB->GetNameStr(goal_type)));
				if(goal_ptr->Get_Matches_Num() == 0)
				{
					Add_New_Matches_For_Goal(goal_ptr); // This consumes the most time, but it seems this cannot be improved.
					goal_ptr->Sort_Matches_If_Necessary();
					goal_ptr->Recompute_Matching_Value();
				}

				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1,
					("\t%3d: [%x] of %s has been evaluated.\n", goals_evaluated, goal_ptr, g_theGoalDB->GetNameStr(goal_type)));
				goal_ptr->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL);
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1, ("\n"));

				goals_evaluated++;
				goal_ptr_iter++;
			}
			else
			{
				if(goal_ptr->Get_Matches_Num() > 0)
				{
					Remove_Matches_For_Goal(goal_ptr);
				}

#if defined(_DEBUG) || defined(USE_LOGGING)
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1,
					("\t%3d: [%x] Is removed", count, goal_ptr_iter->second));
				goal_ptr_iter->second->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL);

				count++;
#endif

				goal_ptr_iter++;
			}
		}

		m_goals_of_type[goal_type].sort(std::greater<Sorted_Goal_ptr>());

		sint32 goals_added = 0;
		goal_ptr_iter = m_goals_of_type[goal_type].begin();
		while(goal_ptr_iter != pruned_goal_iter)
		{
			if(goals_added < max_exec)
			{
				Goal_ptr goal_ptr     = goal_ptr_iter->second;
				m_goals.push_back(goal_ptr);
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1,
					("\t%3d: [%x] of %s has been added.\n", goals_added, goal_ptr, g_theGoalDB->GetNameStr(goal_type)));
//				goal_ptr->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL); // Enable if you really need this
				goal_ptr_iter++;
				goals_added++;

#if defined(_DEBUG) || defined(USE_LOGGING)
				if(goal_ptr_iter == pruned_goal_iter)
				{
					AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1,
						("\t%3d goals of %s have been added, that were all goals.\n", goals_added, g_theGoalDB->GetNameStr(goal_type)));
				}
#endif
			}
			else
			{
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_type, -1,
					("\t%3d goals of %s have been added, the maximum number of goals executed.\n", goals_added, g_theGoalDB->GetNameStr(goal_type)));
				break;
			}
		}
	}

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  PRUNE goals based on max eval and max exec:\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("//  elapsed time = %d ms\n\n", GetTickCount() - t1 ));

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
	GOAL_TYPE   type             = goal_ptr->Get_Goal_Type();
	SQUAD_CLASS goal_squad_class = g_theGoalDB->Get(type)->GetSquadClass();
	bool hasInField = g_theGoalDB->Get(type)->GetInField();

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

		if(!agent->Get_Army()->TestOrderAny(g_theGoalDB->Get(type)->GetExecute())
		&& !agent->Get_Army()->TestCargoOrderAny(g_theGoalDB->Get(type)->GetExecute()))
			continue;

		if(hasInField && g_theWorld->HasCity(agent->Get_Army()->RetPos()))
			continue;

		if(g_theGoalDB->Get(type)->GetOnlyFortifiable() && !agent->Get_Army()->CanEntrench())
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
	SQUAD_CLASS squad_class = agent->Get_Squad_Class();

	for(sint32 i = 0; i < g_theGoalDB->NumRecords(); i++)
	{
		Sorted_Goal_List & goal_list = m_goals_of_type[i];

		SQUAD_CLASS goal_squad_class =
				  g_theGoalDB->Get(i)->GetSquadClass();

		if((goal_squad_class & squad_class) != goal_squad_class)
			continue;

		if(g_theGoalDB->Get(i)->GetInField() && g_theWorld->HasCity(agent->Get_Army()->RetPos()))
			continue;

		if(g_theGoalDB->Get(i)->GetOnlyFortifiable() && !agent->Get_Army()->CanEntrench())
			continue;

		for
		(
		    Sorted_Goal_Iter goal_iter  = goal_list.begin();
		                     goal_iter != goal_list.end();
		                   ++goal_iter
		){
			if(goal_iter->second->Get_Matches_Num() == 0)
				continue;

			if(goal_iter->second->IsInvalid())
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

void Scheduler::Remove_Matches_For_Agent(const Agent_ptr & agent)
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

void Scheduler::Remove_Invalid_Matches_For_Agent
(
    const Agent_ptr & agent
)
{
	for(sint32 i = 0; i < g_theGoalDB->NumRecords(); i++)
	{
		// For retraet goals, once we are in a city, no more retreat is necessary
		if(g_theGoalDB->Get(i)->GetInField() && g_theWorld->HasCity(agent->Get_Army()->RetPos()))
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
}

void Scheduler::Rollback_Matches_For_Goal
(
    const Goal_ptr & goal
)
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	const int log = k_DBG_SCHEDULER;
	GOAL_TYPE goalType = goal->Get_Goal_Type();
	AI_DPRINTF(log, m_playerId, goalType, -1, ("ROLLBACK_MATCHES_FOR_GOAL\n"));
	AI_DPRINTF(log, m_playerId, goalType, -1, ("\t"));
	goal->Log_Debug_Info(log);
	AI_DPRINTF(log, m_playerId, goalType, -1, ("\n"));
#endif

	Squad_Strength needed_strength =
		goal->Get_Strength_Needed();

#if defined(_DEBUG) || defined(USE_LOGGING)
	AI_DPRINTF(log, m_playerId, goalType, -1,
		("\t\tMissing attack = %3.0f\n",needed_strength.Get_Attack()));
	AI_DPRINTF(log, m_playerId, goalType, -1,
		("\t\tMissing transport = %d\n",needed_strength.Get_Transport()));
	AI_DPRINTF(log, m_playerId, goalType, -1,
		("\t\tMissing defense = %3.0f\n",needed_strength.Get_Defense()));
#endif

	m_neededAgentStrength.Set_To_The_Maximum(needed_strength);

#if defined(_DEBUG) || defined(USE_LOGGING)
	AI_DPRINTF(log,  m_playerId, goalType, -1, ("\t%d agents to roll back for Goal %x, %s.\n", goal->Get_Agent_Count(), goal, g_theGoalDB->Get(goalType)->GetNameText()));
#endif

	goal->Rollback_All_Agents();
}

bool Scheduler::Add_Transport_Matches_For_Goal
(
    const Goal_ptr & goal_ptr
)
{
	Assert(goal_ptr->Needs_Transporter());

	bool match_added = false;

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1, ("\t\tNow %d matches, add transport matches.\n", goal_ptr->Get_Matches_Num()));

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

		// Can Transport
		if(!goal_ptr->Can_Transport_Any_Width_Need(agent))
			continue;

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
		&& max == empty // Must be empty otherwise the reansporter would go somewhere else or is already in the agent list
		&& goal_ptr->CanReachTargetContinent(agent)
		&& goal_ptr->Add_Transport_Match(agent)
		){
			match_added = true;
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	if(match_added)
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1, ("\t\tNow %d matches. Transport matches were added.\n", goal_ptr->Get_Matches_Num()));
	}
	else
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1, ("\t\tNow %d matches. No transport matches were added.\n", goal_ptr->Get_Matches_Num()));
	}

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1, ("\t\tRecomputing matches for transport\n"));
#endif

	goal_ptr->Compute_Matching_Value(false);

	return match_added;
}

Scheduler::Goal_List::iterator Scheduler::Reprioritize_Goal(Goal_List::iterator &goal_iter)
{
	Goal_List::iterator goal_ptr_iter = goal_iter, return_iter;
	Utility matchValue = static_cast<Goal_ptr>(*goal_ptr_iter)->Get_Matching_Value();

	++goal_ptr_iter;
	return_iter = goal_ptr_iter;

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
			if (goal_ptr_iter == m_goals.end())
			{
				m_goals.splice(goal_ptr_iter, tmp_list);
				break;
			}
		}
	}

	return return_iter;
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

bool Scheduler::HasAgentToExecute(const GOAL_TYPE & type) const
{
	const OrderRecord *order = g_theGoalDB->Get(type)->GetExecute();

	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		Agent* agent = (*agent_iter);

		if(agent->Get_Is_Dead()) // We haven't removed those, yet.
			continue;

		if(agent->Get_Army()->TestOrderAny(order) || agent->Get_Army()->TestCargoOrderAny(order))
		{
			return true;
		}
	}

	return false;
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

void Scheduler::ResetTransport()
{
	for
	(
	    Goal_List::iterator goal_iter  = m_goals.begin();
	                        goal_iter != m_goals.end();
	                      ++goal_iter
	)
	{
		(*goal_iter)->ResetNeededTransport();
	}
}

void Scheduler::Assign_Garrison()
{
	sint32 garrisonIndex = -1;
	sint32 forceGarrison = -1;
	sint32 forceDefense = -1;
	for(sint32 i = 0; i < g_theGoalDB->NumRecords(); ++i)
	{
		const GoalRecord* rec = g_theGoalDB->Get(i);

		if(!rec->GetSquadClassCanDefend())
			continue;

		if(!rec->GetTargetOwnerSelf())
			continue;

		if(!rec->GetTargetTypeCity())
			continue;

		if(rec->GetForceMatchDefensive())
		{
			forceDefense = i;
		}
		else if(rec->GetForceMatchGarrison())
		{
			forceGarrison = i;
		}

		if(rec->GetIsGarrison())
		{
			garrisonIndex = i;
			break;
		}
	}

	Assert(garrisonIndex >= 0 || forceGarrison >= 0 || forceDefense >= 0);

	if(garrisonIndex == -1 && forceGarrison >= 0)
	{
		garrisonIndex = forceGarrison;
	}

	if(garrisonIndex == -1 && forceDefense >= 0)
	{
		garrisonIndex = forceDefense;
	}

	// It simply should find a matching goal
	// Of course, you can remove this from goals.txt, but that should also screw up a few other things.
	if(garrisonIndex == -1)
	{
		Assert(garrisonIndex != -1);
		return;
	}

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,("// Find armies for garrison\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,("//\n"));

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		Agent_ptr agent = (*agent_iter);

		agent->SetIsNeededForGarrison(false);
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	sint32 count = 0;
#endif
	Goal* generic_goal_ptr = m_generic_goals[garrisonIndex];

	for
	(
	    Sorted_Goal_List::const_iterator
	        sorted_goal_iter  = m_goals_of_type[garrisonIndex].begin();
	        sorted_goal_iter != m_goals_of_type[garrisonIndex].end();
	      ++sorted_goal_iter
	)
	{
		Goal* goal_ptr = sorted_goal_iter->second;

#if defined(_DEBUG) || defined(USE_LOGGING)
		MapPoint pos = (goal_ptr->Get_Target_Army().m_id == 0 || goal_ptr->Get_Target_Army().IsValid()) ? goal_ptr->Get_Target_Pos() : MapPoint(-1, -1);
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
			("[%d] Starting to match garrison resources with %s: %x (x=%d,y=%d), match %d, %s\n",
				count, g_theGoalDB->Get(goal_ptr->Get_Goal_Type())->GetNameText(), goal_ptr, pos.x, pos.y, goal_ptr->Get_Matching_Value(), Goal::GetTargetName(pos)));
		count++;
#endif
		if (goal_ptr->Get_Matches_Num() > 0)
		{
			goal_ptr->Compute_Matching_Value(true, true);
		}
		else if (generic_goal_ptr->Get_Matches_Num() > 0)
		{
			goal_ptr->Compute_Matching_With_Generic_Matches(generic_goal_ptr, true);
		}
	}
}

#if defined(_DEBUG) || defined(USE_LOGGING)
void Scheduler::PrintAllGoals() const
{
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
	                           ("Number of goals in goal database: %d\n", g_theGoalDB->NumRecords()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
	                           ("Current goals for player %d: %d\n", m_playerId, m_goals.size()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));

	Sorted_Goal_Iter sorted_goal_iter;
	for(size_t    goal_type = 0;
	              goal_type < m_goals_of_type.size();
	              goal_type++
	   )
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
		                           ("%s: %zu\n", g_theGoalDB->GetNameStr(goal_type), m_goals_of_type[goal_type].size()));
	}

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\n"));
}
#endif
