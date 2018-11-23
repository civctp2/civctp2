//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Scheduler for AI goals
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
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "profileai.h"

#include <algorithm>
#include <functional>
#include <list>

#include "Scheduler.h"

#include "Goal.h"
#include "Squad.h"
#include "Plan.h"

#include "c3errors.h"

#include "player.h"
#include "Diplomat.h"
#include "CTPDatabase.h"
#include "GoalRecord.h"
#include "ctpgoal.h"
#include "ctpaidebug.h"
#include "c3math.h"
#include "ctpagent.h"
#include "AgreementMatrix.h"
#include "gfx_options.h"
#include "Army.h"
#include "ArmyData.h"

namespace
{

//----------------------------------------------------------------------------
//
// Name       : IsValid
//
// Description: Determine whether a goal is valid in a given context
//
// Parameters : a_Goal			: the goal (type)
//				a_Context		: the context
//
// Globals    : -
//
// Returns    : bool			: the goal is valid in the context
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

bool IsValid
(
	GOAL_TYPE const &								a_Goal,
	Scheduler::Sorted_Goal_List_Vector 	const &		a_Context
)
{
	return static_cast<size_t>(a_Goal) < a_Context.size();
};

} // namespace







extern CTPDatabase<GoalRecord> *g_theGoalDB;
extern CTPDatabase<StrategyRecord> *g_theStrategyDB;








sint32 Scheduler::s_max_match_list_cycles = 6;

Scheduler::Scheduler_Vector Scheduler::s_theSchedulers;


int Scheduler::m_contactCachedPlayer = -1;
uint32 Scheduler::m_contactCache = 0;
int Scheduler::m_neutralRegardCachedPlayer = -1;
uint32 Scheduler::m_neutralRegardCache = 0;
int Scheduler::m_allyRegardCachedPlayer = -1;
uint32 Scheduler::m_allyRegardCache = 0;

void Scheduler::ResizeAll(const PLAYER_INDEX & newMaxPlayerId)
{
    s_theSchedulers.resize(newMaxPlayerId + 1); // 1 extra for the Barbarians

    for (size_t i = 0; i <= (unsigned) newMaxPlayerId; ++i)
    {
		s_theSchedulers[i].SetPlayerId(i);
	}
}

// no longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::LoadAll(CivArchive & archive)
{
    DPRINTF(k_DBG_AI, ("\n\ncalling Scheduler::LoadAll\n\n"));
	for (sint32 i = 0; (unsigned) i < s_theSchedulers.size(); i++)
	{
		s_theSchedulers[i].Load(archive);
	}
}

// no longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::SaveAll(CivArchive & archive)
{
	for (sint32 i = 0; (unsigned) i < s_theSchedulers.size(); i++)
	{
		s_theSchedulers[i].Save(archive);
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
	Assert((unsigned) playerId < s_theSchedulers.size());

	return s_theSchedulers[playerId];
}

/// not used
void Scheduler::ValidateAll()
{
	for (sint32 i = 0; (unsigned) i < s_theSchedulers.size(); i++)
	{
		s_theSchedulers[i].Validate();
	}
}

void Scheduler::CleanupAll(void)
{
    Scheduler_Vector().swap(s_theSchedulers);
}

Scheduler::Scheduler()
{

	m_playerId = -1;

	Initialize();
}

Scheduler::Scheduler(const Scheduler &scheduler)
{

	m_playerId = -1;

	*this = scheduler;
}

Scheduler::~Scheduler()
{

	Cleanup();
}

Scheduler& Scheduler::operator= (const Scheduler &scheduler)
{

	Assert(m_playerId == -1);
	Assert(scheduler.m_playerId == -1);

	Initialize();

	return *this;
}

void Scheduler::Cleanup()
{

	Squad_List::iterator squad_ptr_iter = m_new_squads.begin();
	while (squad_ptr_iter != m_new_squads.end() )
	{
		delete *squad_ptr_iter;

		squad_ptr_iter = m_new_squads.erase(squad_ptr_iter);

	}

	squad_ptr_iter = m_squads.begin();
	while (squad_ptr_iter != m_squads.end())
	{
		delete *squad_ptr_iter;

		squad_ptr_iter = m_squads.erase(squad_ptr_iter);
	}

	Goal_List::iterator goal_ptr_iter = m_new_goals.begin();
	while (goal_ptr_iter != m_new_goals.end())
	{
		delete *goal_ptr_iter;

		goal_ptr_iter = m_new_goals.erase(goal_ptr_iter);
	}

	Sorted_Goal_Iter sorted_goal_iter;
	for (GOAL_TYPE goal_type = 0; (unsigned) goal_type < m_goals_of_type.size(); goal_type++)	{

		sorted_goal_iter = m_goals_of_type[goal_type].begin();
		while (sorted_goal_iter != m_goals_of_type[goal_type].end() )
		{
			delete sorted_goal_iter->second;
			sorted_goal_iter++;
		}
		m_goals_of_type[goal_type].clear();
	}


	m_goals_of_type.clear();


	m_matches.clear();
}

/// no longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::Load(CivArchive & archive)
{

}

/// no longer used "Reason: should be able to regenerate state from game objects."
void Scheduler::Save(CivArchive & archive)
{

}

void Scheduler::Initialize()
{

	if ( g_theGoalDB == NULL )
		return;

	Cleanup();

	m_exec_goal_count.resize(g_theGoalDB->NumRecords(),0);

	m_pruned_goals_count.resize(g_theGoalDB->NumRecords(),0);

	m_goals_of_type.resize(g_theGoalDB->NumRecords());

	m_pruned_goals_of_type.resize(g_theGoalDB->NumRecords());

	m_committed_agents = 0;

	m_total_agents = 0;

	m_matches.clear();

	m_current_plan_iter = m_matches.end();

//	m_AllIsExplored = false;
}

void Scheduler::SetPlayerId(const PLAYER_INDEX &player_index)
{
	m_playerId = player_index;
}

void Scheduler::Planning_Status_Reset()
{
}

//////////////////////////////////////////////////////////////////////////
//
//  Process_Squad_Changes
//
//  When: Squads Change
//
//  Iterate: m_squads
//
//  1. Remove any empty squads and any matches that reference them.
//
//  2. Recompute squad class for all squads with changed/killed agents.
//
//  3. If squad class of squad changes, Add_New_Matches_For_Squad
//
//
//  4. Create new squads from new agents, add to squads_of_class <= where does the m_new_squads list come from
//	  list and create new matches from pruned_goals_list.
//
//  5. Count up total number of agents available to match
//

//////////////////////////////////////////////////////////////////////////

Scheduler::TIME_SLICE_STATE Scheduler::Process_Squad_Changes()
{
	SQUAD_CLASS old_class;
	SQUAD_CLASS new_class;
	Squad_List::iterator squad_ptr_iter;
	Squad_List::iterator new_squad_iter;
	sint16 committed_agents;

	m_total_agents = 0;

	squad_ptr_iter = m_squads.begin();
	while (squad_ptr_iter != m_squads.end())
	{

		committed_agents = (*squad_ptr_iter)->Remove_Dead_Agents();
		m_committed_agents -=  committed_agents;

		if ((*squad_ptr_iter)->Get_Num_Agents() <= 0) {

			Remove_Matches_For_Squad(*squad_ptr_iter);

			delete *squad_ptr_iter;

			squad_ptr_iter = m_squads.erase(squad_ptr_iter);

			continue;
		}

		m_total_agents += (*squad_ptr_iter)->Get_Num_Agents();


		old_class = (*squad_ptr_iter)->Get_Squad_Class();
		new_class = (*squad_ptr_iter)->Compute_Squad_Class();

		if (old_class != new_class)
		{

			Remove_Matches_For_Squad(*squad_ptr_iter);

			Add_New_Matches_For_Squad(squad_ptr_iter);
		}

		squad_ptr_iter++;

	}


	new_squad_iter = m_new_squads.begin();
	while (new_squad_iter != m_new_squads.end()) {

		new_class = (*new_squad_iter)->Compute_Squad_Class();


		squad_ptr_iter = m_squads.insert(m_squads.end(), *new_squad_iter);

		Add_New_Matches_For_Squad(squad_ptr_iter);

		m_total_agents += (*squad_ptr_iter)->Get_Num_Agents();

		new_squad_iter = m_new_squads.erase(new_squad_iter);
	}

	return TIME_SLICE_DONE;
}

//////////////////////////////////////////////////////////////////////////
//
//  Reset_Squad_Execution
//
//  When: After Squads Change
//
//  Iterate: m_squads
//
//  make squads available for goals
//

//////////////////////////////////////////////////////////////////////////

void Scheduler::Reset_Squad_Execution()
{
	Squad_List::iterator squad_ptr_iter = m_squads.begin();
	while (squad_ptr_iter != m_squads.end())
	{

		(*squad_ptr_iter)->Set_Can_Be_Executed(true);
        squad_ptr_iter++;
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

Scheduler::TIME_SLICE_STATE Scheduler::Process_Goal_Changes()
{

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t// PRIORITIZE GOALS\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("\t//\n\n"));











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
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t//\n\n"));

	Prune_Goals();

	m_neededSquadStrength.Init();
	m_maxUndercommittedPriority = Goal::BAD_UTILITY;

	return TIME_SLICE_DONE;
}

//////////////////////////////////////////////////////////////////////////
//
//  Sort_Matches
//
//  When: Matches Change
//
//  Iterate: m_matches Plan_List
//
//  1. For each match, compute the utility value between the
//     goal and each component agent in the squad.
//
//  2. Compute average match value from agent utility values and
//     goal raw priority.
//
//  3. Sort matches list.
//

//////////////////////////////////////////////////////////////////////////

bool Scheduler::Sort_Matches()
{
	Plan_List::iterator plan_iter = m_matches.begin();
	bool first_turn_of_war =
		Diplomat::GetDiplomat(m_playerId).FirstTurnOfWar();

	while (plan_iter != m_matches.end())
	{

		if (!plan_iter->Plan_Is_Needed_And_Valid())
		{

			Assert(0);
			continue;
		}




        if (first_turn_of_war || plan_iter->Commited_Agents_Need_Orders() || m_playerId == 0 || plan_iter->CanMatchesBeReevaluated())
			m_committed_agents -= plan_iter->Rollback_All_Agents();

		plan_iter->Compute_Matching_Value();

		plan_iter++;
	}

	sint32 t1;
	sint32 t2;

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, -1, -1, ("\n"));
	t1 = GetTickCount();

#ifdef _DEBUG
	sint32 size = m_matches.size();
#endif // _DEBUG

	m_matches.sort(std::greater<Plan>());

	Assert(m_matches.size() == (unsigned) size);

	t2 = GetTickCount();
	AI_DPRINTF(k_DBG_AI, m_playerId, -1, -1,
		("//  %d matches sorted = %d ms\n\n\n", m_matches.size(), (t2 - t1)));

#ifdef _DEBUG_SCHEDULER




	Squad_List::iterator squad_iter;
	Agent_List::const_iterator agent_iter;
	for (squad_iter = m_squads.begin(); squad_iter != m_squads.end(); squad_iter++)
	{
		const Agent_List & agent_list = (*squad_iter)->Get_Agent_List();
		for (agent_iter = agent_list.begin(); agent_iter != agent_list.end(); agent_iter++)
		{
			if ((*agent_iter)->Get_Is_Used())
			{
				bool NOT_ALL_AGENTS_ROLLED_BACK = false;
				Assert(NOT_ALL_AGENTS_ROLLED_BACK);
			}
		}
	}
#endif // _DEBUG_SCHEDULER

#ifdef _DEBUG


	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("// SORTED MATCHES\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));

	sint32 count = 0;
	for (plan_iter = m_matches.begin();
		 plan_iter != m_matches.end();
		 plan_iter++) {


		CTPGoal_ptr goal = (CTPGoal_ptr) plan_iter->Get_Goal();

		Squad_ptr squad = plan_iter->Get_Squad();
		Utility value = plan_iter->Get_Matching_Value();
		if (value > Goal::BAD_UTILITY + 0.5)
		{




			GOAL_TYPE goal_type = goal->Get_Goal_Type();
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
				("\t[%d] match=%d %s (goal: %x squad: %x)\n",
					count++, value, g_theGoalDB->Get(goal_type)->GetNameText(), goal, squad));

			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t\tGoal:\n"));
			goal->Log_Debug_Info(k_DBG_SCHEDULER_ALL);

			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t\tSquad:\n"));
			squad->Log_Debug_Info(k_DBG_SCHEDULER_ALL);
		}
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//  Match_Resources
//
//  When: Goals need to be executed (CtpAi_ProcessMatchesEvent)
//
//  Iterate: matches
//
//  1. For each match, move agents from donor squad to goal.
//
//  2. If goal is in progress, execute goal and check status.
//
//  3. If goal is completed, create a new squad from agents that
//     completed the goal.
//
//  4. If goal failed, rollback agents to donor squads.
//     Rollback marks agents in matches as invalid for the goal.
//
//  5. When all agents have been committed, check all goals on
//     pruned_goals_class list.  If under committed, rollback
//     all agents to donor squads. If over committed, rollback
//     excess agents to donor squads.
//

//////////////////////////////////////////////////////////////////////////

void Scheduler::Match_Resources(const bool move_armies)
{
	Goal_ptr goal_ptr;
	Squad_ptr squad_ptr;
	Plan_List::iterator plan_iter;
	Plan_List::iterator tmp_plan_iter;
	GOAL_TYPE goal_type;
	Sorted_Goal_Iter goal_ptr_iter;
	bool match_added;

	sint32 agent_count;

#if 0
	// This does nothing but waste time. AreAllGoalsSatisfied is never used.
	bool AreAllGoalsSatisfied = true;
	plan_iter = m_matches.begin();
	while (plan_iter != m_matches.end()) {

		goal_ptr = plan_iter->Get_Goal();
		if (!goal_ptr->Is_Satisfied())
		{
			AreAllGoalsSatisfied = false;
		}

		plan_iter++;
	}
#endif

	plan_iter = m_matches.begin();
	while (plan_iter != m_matches.end()) {

		if (m_committed_agents >= m_total_agents)
			break;


		agent_count = plan_iter->Commit_Agents();
		m_committed_agents += agent_count;

		goal_ptr = plan_iter->Get_Goal();
		squad_ptr = plan_iter->Get_Squad();

		if (goal_ptr->Get_Agent_Count() == 0)
		{

			plan_iter++;
			continue;
		}

		GOAL_RESULT result;
		if (move_armies)
			result = plan_iter->Execute_Task();
		else
			result = GOAL_IN_PROGRESS;





		switch (result) {
		case GOAL_ALREADY_MOVED:

			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_ptr->Get_Goal_Type(), -1,
				("\t\tGOAL_ALREADY_MOVED (goal: %x squad: %x)\n", goal_ptr, squad_ptr));

			plan_iter++;
			break;

		case GOAL_IN_PROGRESS:

			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_ptr->Get_Goal_Type(), -1,
				("\t\tGOAL_IN_PROGRESS (goal: %x squad: %x)\n", goal_ptr, squad_ptr));

			plan_iter++;
			break;

		case GOAL_COMPLETE:






			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
				("\t\tGOAL_COMPLETE (goal: %x squad: %x)\n", goal_ptr, squad_ptr));

			if (goal_ptr->Is_Single_Squad() == false)
			{
			}


			while (plan_iter != m_matches.end())
			{
				if (plan_iter->Get_Goal() == goal_ptr)
					plan_iter++;
				else
					break;
			}

			if (goal_ptr->Get_Removal_Time())
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
					("\t\tGOAL_COMPLETE (goal: %x squad: %x) -- Removing matches for goal.\n",
					goal_ptr, squad_ptr));


				Remove_Matches_For_Goal(goal_ptr);
			}

			else
			{

				Rollback_Matches_For_Goal(goal_ptr);
			}

			break;

		case GOAL_NEEDS_TRANSPORT:


			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_ptr->Get_Goal_Type(), -1,
				("\t\tGOAL_NEEDS_TRANSPORT (goal: %x squad: %x)\n", goal_ptr, squad_ptr));

			tmp_plan_iter = plan_iter;
			tmp_plan_iter++;

			goal_type = goal_ptr->Get_Goal_Type();
			goal_ptr_iter = m_goals_of_type[goal_type].begin();
			while ( (goal_ptr_iter != m_pruned_goals_of_type[goal_type] &&
				     goal_ptr_iter != m_goals_of_type[goal_type].end()) )
				 {
					 if (goal_ptr_iter->second == goal_ptr)
						break;

					 goal_ptr_iter++;
				 }

			Assert(goal_ptr_iter != m_pruned_goals_of_type[goal_type]);
			Assert(goal_ptr_iter != m_goals_of_type[goal_type].end());

			match_added = Add_Transport_Matches_For_Goal(goal_ptr_iter, tmp_plan_iter);

			if (match_added == false)
				{

					AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
						("\t\t **NO transports found. Failing.\n"));
				}
			else
				{

					AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
						("\t\t Transports found.\n"));

					plan_iter++;

					break;
				}

		case GOAL_FAILED:




			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
				("\t\tGOAL_FAILED (goal: %x squad: %x)\n", goal_ptr, squad_ptr));


			Rollback_Matches_For_Goal(plan_iter->Get_Goal());

			plan_iter++;

			break;
		}
	}




	Free_Undercommitted_Goal();

	m_current_plan_iter = m_matches.end();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Add_New_Goal
//
//
// Add a new goal to the m_new_goals Goal_List
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

	m_new_goals.push_back(new_goal);
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//    Add_New_Squad
//
//
// Add a new squad to the m_new_squads Squad_List
//
// New squads are created by AddGoalsForArmy, which is called whenever a savegame is loaded
// or an army is created.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

void Scheduler::Add_New_Squad(const Squad_ptr & new_squad)
{

#ifdef _DEBUG_SCHEDULER

	Squad_List::iterator squad_iter = m_new_squads.begin();
	while (squad_iter != m_new_squads.end() && !(*squad_iter)->ContainsArmyIn(*new_squad))
	{
		squad_iter++;
	}
	Assert(squad_iter == m_new_squads.end());

	squad_iter = m_squads.begin();
	while (squad_iter != m_squads.end() && !(*squad_iter)->ContainsArmyIn(*new_squad))
	{
		squad_iter++;
	}

	Assert(squad_iter == m_squads.end());
#endif // _DEBUG_SCHEDULER

	m_new_squads.push_back(new_squad);
}

////////////////////////////////////////////////////////////
//
//  Remove_Goal
//
//  called by Scheduler::Prioritize_Goals() when

Scheduler::Sorted_Goal_Iter Scheduler::Remove_Goal(const Scheduler::Sorted_Goal_Iter & sorted_goal_iter)
{
	GOAL_TYPE goal_type = sorted_goal_iter->second->Get_Goal_Type();

	Sorted_Goal_List & list = m_goals_of_type[goal_type];

	Remove_Matches_For_Goal(sorted_goal_iter->second);

	delete sorted_goal_iter->second;

	Scheduler::Sorted_Goal_Iter next_iter = list.erase(sorted_goal_iter);

	if (m_pruned_goals_of_type[goal_type] == sorted_goal_iter)
		m_pruned_goals_of_type[goal_type] = next_iter;

	return next_iter;


}






void Scheduler::Remove_Goals_Type(const GoalRecord *rec)
{
	Sorted_Goal_List &	goalList	= m_goals_of_type[rec->GetIndex()];
	for
	(
		Sorted_Goal_Iter	sorted_goal_iter	= goalList.begin();
		sorted_goal_iter != goalList.end();
		sorted_goal_iter = Remove_Goal(sorted_goal_iter)
	)
	{
		// Action in for
	}
}

bool Scheduler::Validate() const
{
#ifdef _DEBUG_SCHEDULER
	Sorted_Goal_List::const_iterator sorted_goal_iter;

	for (sint32 goal_type = 0;	goal_type < g_theGoalDB->NumRecords(); goal_type++)
	{

		sorted_goal_iter = m_goals_of_type[goal_type].begin();
		while (sorted_goal_iter != m_goals_of_type[goal_type].end())
		{

			if (!sorted_goal_iter->second->Validate())
			{
				bool GOAL_VALIDATION_FAILED = false;
				Assert(GOAL_VALIDATION_FAILED);
			}
			sorted_goal_iter++;
		}
	}
#endif // _DEBUG_SCHEDULER

	return true;
}

Scheduler::Sorted_Goal_List Scheduler::Get_Top_Goals(const int &number) const
{
	Sorted_Goal_List output_goals;

	return output_goals;
}

//----------------------------------------------------------------------------
//
// Name       : Scheduler::GetValueUnsatisfiedGoals
//
// Description: Get the total value of all unsatisfied goals of a given type.
//
// Parameters : type	: type of goal
//
// Globals    : -
//
// Returns    : sint32	: sum of value of the unsatisfied goals of type type.
//
// Remark(s)  : Used in ThreatenedCity_MotivationEvent to trigger MOTIVATION_FEAR_CITY_DEFENSE
//
//----------------------------------------------------------------------------

sint32 Scheduler::GetValueUnsatisfiedGoals(const GOAL_TYPE & type) const
{
	sint32	total_value	= 0;

	if (IsValid(type, m_goals_of_type))
	{
		for
		(
			Sorted_Goal_List::const_iterator sorted_goal_iter =
				m_goals_of_type[type].begin();
			sorted_goal_iter != m_goals_of_type[type].end();
			++sorted_goal_iter
		)
		{
			CTPGoal_ptr	const	ctp_goal_ptr =
				static_cast<CTPGoal_ptr const>(sorted_goal_iter->second);

			if (ctp_goal_ptr->Get_Invalid()		||
				ctp_goal_ptr->Is_Satisfied()	||
				ctp_goal_ptr->ArmiesAtGoal()
			   )
			{
				// Goal has become invalid or has been satisfied: try next.
			}
			else
			{
				total_value += ctp_goal_ptr->Get_Target_Value();
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
// Parameters : type		: type of goal
//				satisfied	: consider satisfied/unsatisfied goals only
//
// Globals    : -
//
// Returns    : sint32	: sum of value of the unsatisfied goals of type type.
//
// Remark(s)  : Returns the first found goal that matches the parameters. The list m_goals_of_type[type]
//              is ordered by raw priority.
//
//              Used in governor to give advice on what city to defend
//              and in ThreatenedCity_MotivationEvent to trigger MOTIVATION_FEAR_CITY_DEFENSE
//
//----------------------------------------------------------------------------

Goal_ptr Scheduler::GetHighestPriorityGoal(const GOAL_TYPE & type, const bool satisfied) const
{
	if (IsValid(type, m_goals_of_type))
	{
		for
		(
			Sorted_Goal_List::const_iterator sorted_goal_iter =
				m_goals_of_type[type].begin();
			sorted_goal_iter != m_goals_of_type[type].end();
			++sorted_goal_iter
		)
		{
			CTPGoal_ptr	const	ctp_goal_ptr =
				static_cast<CTPGoal_ptr const>(sorted_goal_iter->second);

			if (ctp_goal_ptr->Get_Invalid()					||
				(satisfied != ctp_goal_ptr->Is_Satisfied())	||
				ctp_goal_ptr->ArmiesAtGoal()
			   )
			{
				// Goal does not match: try next.
			}
			else
			{
				return ctp_goal_ptr;
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
	Goal_List::iterator goal_ptr_iter;
	Sorted_Goal_Iter sorted_goal_iter;
	GOAL_TYPE goal_type = 0;

	time_t t1;
	time_t t2;

	t1 = GetTickCount();

	goal_ptr_iter = m_new_goals.begin();

	for (; goal_ptr_iter != m_new_goals.end(); goal_ptr_iter++) {

		goal_type = (*goal_ptr_iter)->Get_Goal_Type();

		CTPGoal_ptr goal2 = (CTPGoal_ptr) *goal_ptr_iter;

		Sorted_Goal_Iter tmp_goal_iter =
			m_goals_of_type[goal_type].begin();

		{
			while (tmp_goal_iter != m_goals_of_type[goal_type].end())
			{
				CTPGoal_ptr goal1 = (CTPGoal_ptr) tmp_goal_iter->second;
				if ( *goal1 == *goal2)
				{

					break;
				}
				tmp_goal_iter++;
			}




			if (tmp_goal_iter != m_goals_of_type[goal_type].end())
			{

				delete goal2;

				continue;
			}
		}

		m_goals_of_type[goal_type].
			push_back(Sorted_Goal_ptr(Goal::BAD_UTILITY, *goal_ptr_iter));

	/*	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
			("\tAdded New Goal: %x %d(%s) - (%3d,%3d)\n",
			*goal_ptr_iter,
			goal_type,
			g_theGoalDB->Get(goal_type)->GetNameText(),
			(*goal_ptr_iter)->GetTargetPos().x,
			(*goal_ptr_iter)->GetTargetPos().y
			));*/
	}

	m_new_goals.clear();

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  Add new goals to list:\n"));
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n\n", (t2 - t1)  ));
	t1 = GetTickCount();

	sint16 committed_agents = 0;

	for (goal_type = 0;	goal_type < g_theGoalDB->NumRecords(); goal_type++) {

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("\n\n "));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("// \n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("// %s \n",g_theGoalDB->Get(goal_type)->GetNameText()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,("// \n\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal_type, -1,
    ("\t %9x,\tGOAL\t\t,\tCOORDS\t\t,\tINIT_VALUE,\t\tLAST_VALUE,\t\tTHREAT,\t\tENEMYVAL,\t\tALLIEDVAL,\t\tMAXPOW,\t\tHOMEDIST \t(   )\t,\t\tENEMYDIST (    ),\t\tSETTLE,\t\tCHOKE,\t\tUNEXPLORED,\t\tTHREATEN, \n",
    this));

		sorted_goal_iter = m_goals_of_type[goal_type].begin();
		while (sorted_goal_iter != m_goals_of_type[goal_type].end()) {


			if ( sorted_goal_iter->second->Get_Totally_Complete()) {






				if (sorted_goal_iter->second->Is_Single_Squad() == false) {













				}
			}




			if ( sorted_goal_iter->second->Get_Removal_Time() ) {

				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, sorted_goal_iter->second->Get_Goal_Type(), -1,
					("\tRemoving Invalid/Completed Goal: %x\n", sorted_goal_iter->second));

				sorted_goal_iter = Remove_Goal(sorted_goal_iter);

				continue;
			}

			sorted_goal_iter->second->Set_Can_Be_Executed(true);

			sorted_goal_iter->first =
				sorted_goal_iter->second->Compute_Raw_Priority();

			sorted_goal_iter->second->Compute_Needed_Troop_Flow();

			sorted_goal_iter++;

		}

		m_goals_of_type[goal_type].sort(std::greater<Sorted_Goal_ptr>());

		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t// SORTED GOALS: (type %d)\n",	goal_type));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t//\n"));
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\n"));

#ifdef _DEBUG
		int count=0;
#endif
		for (sorted_goal_iter = m_goals_of_type[goal_type].begin();
		    sorted_goal_iter != m_goals_of_type[goal_type].end();
		    sorted_goal_iter++) {
#ifdef _DEBUG

			if (sorted_goal_iter->first > Goal::BAD_UTILITY + 0.5)
			{

				AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
					("\t%3d: ", count++));
				sorted_goal_iter->second->Log_Debug_Info(k_DBG_SCHEDULER_ALL);
			}
#endif

			committed_agents += sorted_goal_iter->second->Get_Agent_Count();
		}
	}

	t2 = GetTickCount();
	DPRINTF(k_DBG_AI, ("//  Compute raw priorities and sort goal lists:\n"));
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n\n", (t2 - t1)  ));

	if (committed_agents != m_committed_agents) {
		DPRINTF(k_DBG_AI, ("m_committed_agents out of sync (%d != %d)\n",
			 committed_agents, m_committed_agents));
		m_committed_agents = committed_agents;
	}

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

	m_pruned_goals_count.assign(g_theGoalDB->NumRecords(), 0);

	Sorted_Goal_Iter goal_ptr_iter;
	Sorted_Goal_Iter pruned_goal_iter;
 	Sorted_Goal_Iter tmp_goal_iter;

	GOAL_TYPE goal_type;
	bool ok_to_match_goal;
	sint16 max_eval;
	sint16 max_exec;

	const StrategyRecord &strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const StrategyRecord::GoalElement *goal_element_ptr;

	time_t t1;
	time_t t2;
	sint32 prune_time = 0;

	for (sint16 goal_element = 0; goal_element < strategy.GetNumGoalElement(); goal_element++) {

		t1 = GetTickCount();

		goal_element_ptr = strategy.GetGoalElement(goal_element);

		Goal_ptr goal_ptr;
		Utility raw_priority;


		goal_element_ptr = strategy.GetGoalElement(goal_element);
		goal_type = GetMaxEvalExec(goal_element_ptr, max_eval, max_exec);

		pruned_goal_iter = m_goals_of_type[goal_type].end();

		goal_ptr_iter = m_goals_of_type[goal_type].begin();
		while (goal_ptr_iter != pruned_goal_iter &&
			goal_ptr_iter != m_goals_of_type[goal_type].end()) {

			goal_ptr = goal_ptr_iter->second;
			raw_priority = goal_ptr_iter->first;

			ok_to_match_goal =
				(m_pruned_goals_count[goal_type] < max_eval);

			ok_to_match_goal = ok_to_match_goal &&
				(raw_priority != Goal::BAD_UTILITY);

			if (ok_to_match_goal) {

				if (goal_ptr->Get_Match_References().size() == 0) {
					Add_New_Matches_For_Goal(goal_ptr_iter);
				}

				m_pruned_goals_count[goal_type]++;

				goal_ptr_iter++;
			}
			else {

				if (goal_ptr->Get_Match_References().size() > 0) {
					Remove_Matches_For_Goal(goal_ptr);
				}

 				tmp_goal_iter = goal_ptr_iter;

 				goal_ptr_iter++;

				m_goals_of_type[goal_type].splice(m_goals_of_type[goal_type].end(),
								  m_goals_of_type[goal_type],
								  tmp_goal_iter);

				if (pruned_goal_iter == m_goals_of_type[goal_type].end())
				{

					pruned_goal_iter--;
				}
			}
		}


		m_pruned_goals_of_type[goal_type] = pruned_goal_iter;

		t2 = GetTickCount();
 		prune_time += t2 - t1;

#ifdef _DEBUG
		sint32 count = 0;
//		CTPGoal_ptr ctpgoal_ptr;
		for (goal_ptr_iter = m_pruned_goals_of_type[goal_type];
			 goal_ptr_iter != m_goals_of_type[goal_type].end();
			 goal_ptr_iter++) {

				 if (count == 0)
				 {
					 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\n\n"));
					 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t//\n"));
					 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1, ("\t// PRUNED GOALS: %s (type %d)\n",
						 g_theGoalDB->Get(goal_type)->GetNameText(),
						 goal_type));
					 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						 ("\t// max eval = %3.2f, max_exec = %3.2f",
						 goal_element_ptr->GetMaxEval(),
						 goal_element_ptr->GetMaxExec()));
					 if (goal_element_ptr->GetExecPerCity())
						 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						 (" (ExecPerCity)"));
					 if (goal_element_ptr->GetEvalPerCity())
						 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						 (" (EvalPerCity)"));
					 if (goal_element_ptr->GetPerCity())
						 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						 (" (PerCity)"));
					 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						 ("\n"));

					 AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						 ("\t//\n\n"));
				 }

				 if (goal_ptr_iter->first > Goal::BAD_UTILITY + 0.5)
				 {
			/*		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_type, -1,
						("\t%3d: [%x]", count, goal_ptr_iter->second));
					ctpgoal_ptr = (CTPGoal_ptr) goal_ptr_iter->second;
					ctpgoal_ptr->Log_Debug_Info(k_DBG_SCHEDULER_ALL);*/
				 }
				 count++;
		}
#endif

	}

	DPRINTF(k_DBG_AI, ("//  PRUNE goals based on max eval and max exec:\n"));
	DPRINTF(k_DBG_AI, ("//  elapsed time = %d ms\n\n", prune_time  ));

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//   Add_New_Match_For_Goal_And_Squad
//
//   called by: Add_New_Matches_For_Goal
//              Add_New_Matches_For_Squad
//              Add_Transport_Matches_For_Goal
//
//

bool Scheduler::Add_New_Match_For_Goal_And_Squad
(
 const Sorted_Goal_Iter & goal_iter,
 const Squad_List::iterator & squad_iter,
 Plan_List::iterator & plan_iter
 )
{

	Utility matching_value;
	Plan the_match;

	the_match.Set_Squad(*squad_iter);
	the_match.Set_Goal(goal_iter->second);

	Assert(!goal_iter->second->Get_Invalid())
	Assert((*squad_iter)->Get_Num_Agents() > 0)

	matching_value = the_match.Compute_Matching_Value();

	if (matching_value > (Goal::BAD_UTILITY + 0.5)) {






		while (plan_iter != m_matches.end() &&
			   *plan_iter > the_match)
			   plan_iter++;

		plan_iter = m_matches.insert(plan_iter, the_match);

		goal_iter->second->Add_Match_Reference(plan_iter);
		(*squad_iter)->Add_Match_Reference(plan_iter);

	/*	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, goal_iter->second->Get_Goal_Type(), -1,
			("\tAdded match for goal: %x squad: %x  value = %d\n",
			 goal_iter->second,
			 (*squad_iter),
			 matching_value));

	*/
		return true;

	}
	else {


	/*	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, goal_iter->second->Get_Goal_Type(), -1,
        ("\tMatch for goal: %x (%d) squad: %x has BAD_UTILITY.\n",
			 goal_iter->second,
        goal_iter->second->Get_Goal_Type(),
			 (*squad_iter)));
*/
		return false;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
//   Add_New_Matches_For_Goal
//
//   called by Scheduler::Prune_Goals() when iterating through the Sorted_Goal (by raw priority)
//   list m_goals_of_type[goal_type]
//
//   If it's ok to match the goal (because so far there's less than maxeval of them, and
//   it's raw_priority != Goal::BAD_UTILITY), it iterates through the Squad_List m_squads
//   and calls Add_New_Match_For_Goal_And_Squad to try and add a 3-tuple <goal,squad,matching value>,
//   for each squad that qualifies, to the plan
//
///////////////////////////////////////////////////////////////////////////////////////////////

sint32 Scheduler::Add_New_Matches_For_Goal
(
 const Sorted_Goal_Iter & goal_iter
 )
{
	Squad_Class_List::iterator squad_class_iter;
	Squad_List::iterator squad_iter;
	sint32 count = 0;

	if (goal_iter->second->Get_Invalid())
		return 0;

	Plan_List::iterator plan_iter;

	GOAL_TYPE type = goal_iter->second->Get_Goal_Type();
    SQUAD_CLASS goal_squad_class =
			  g_theGoalDB->Get(type)->GetSquadClass();

	for ( squad_iter = m_squads.begin();
		  squad_iter != m_squads.end();
		  squad_iter++)
		  {

			if (( goal_squad_class & (*squad_iter)->Get_Squad_Class() ) != goal_squad_class )
				continue;

			plan_iter = m_matches.end();
			if ( Add_New_Match_For_Goal_And_Squad(goal_iter,
												  squad_iter,
												  plan_iter) )
			{
				  count++;
			}
		  }

	return count;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//
//   Add_New_Matches_For_Squad
//
//   called by Process_Squad_Changes
//

sint32 Scheduler::Add_New_Matches_For_Squad
(
 const Squad_List::iterator & squad_iter
 )
{
	Goal_Type_List::iterator goal_type_iter;
	Sorted_Goal_Iter goal_iter;
	sint32 count = 0;

	if ((*squad_iter)->Get_Num_Agents() <= 0)
		return 0;

	for (int type = 0; type < g_theGoalDB->NumRecords(); type++)
	{

		Sorted_Goal_List & goal_list = m_goals_of_type[type];

		Plan_List::iterator plan_iter;

		SQUAD_CLASS goal_squad_class =
				  g_theGoalDB->Get(type)->GetSquadClass();

		for (goal_iter = goal_list.begin();
			 goal_iter != goal_list.end() &&
			 goal_iter != m_pruned_goals_of_type[type];
			 goal_iter++)
		{

			if (goal_iter->second->Get_Invalid())
				continue;


			if (( goal_squad_class & (*squad_iter)->Get_Squad_Class() ) != goal_squad_class )
				continue;

			plan_iter = m_matches.end();
			if ( Add_New_Match_For_Goal_And_Squad(goal_iter,
												  squad_iter,
												  plan_iter) )
				count++;
			else

				break;
		}
	}

	return count;
}

//////////////////////////////////////////////////////////////////////////
//
//  Free_Undercommitted_Goal
//
//  called by Match_Resources
//
//  Find the undercommitted goal with the lowest raw priority, and
//  Free all agents for this goal.  Remove the goal as a potential
//  target for the freed agents in this phase.
//
//////////////////////////////////////////////////////////////////////////

bool Scheduler::Free_Undercommitted_Goal()
{
	bool undercommittment_found = false;
	Sorted_Goal_Iter highest_goal_iter;
	Utility highest_goal_priority = Goal::BAD_UTILITY;

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,	("\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,	("\t//\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,	("\t// FREE_UNDERCOMMITTED_GOAL\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,	("\t//\n\n"));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,	("\t//\n"));

	GOAL_TYPE goal_type;
	Sorted_Goal_Iter goal_iter;
	for (goal_type = 0; goal_type < g_theGoalDB->NumRecords(); goal_type++) {

		Sorted_Goal_List & goal_list = m_goals_of_type[goal_type];

		for (goal_iter = goal_list.begin();
			 goal_iter != goal_list.end() &&
			 goal_iter != m_pruned_goals_of_type[goal_type];
			 goal_iter++) {

			if ( goal_iter->second->Is_Execute_Incrementally() == true )
				continue;

			if ( goal_iter->second->Is_Goal_Undercommitted() == false )
				continue;

			if ( goal_iter->second->Get_Raw_Priority() > highest_goal_priority ) {

				highest_goal_priority = goal_iter->second->Get_Raw_Priority();
				highest_goal_iter = goal_iter;
			}
		}
	}

	if (highest_goal_priority > Goal::BAD_UTILITY) {
		Rollback_Matches_For_Goal(highest_goal_iter->second);

		undercommittment_found = true;
	}
	return undercommittment_found;
}

void Scheduler::Remove_Matches_For_Goal(const Goal_ptr & goal_ptr)
{
    std::list<Plan_List::iterator> & match_refs = goal_ptr->Get_Match_References();
    std::list<Plan_List::iterator>::iterator plan_ref_iter;
	Squad_ptr squad_ptr;

	sint32 rolled_back_agents = 0;

	for (plan_ref_iter = match_refs.begin();
		 plan_ref_iter != match_refs.end();
		 plan_ref_iter++) {
		Assert((*plan_ref_iter)->Get_Goal() == goal_ptr);

		rolled_back_agents += (*plan_ref_iter)->Rollback_All_Agents();

		squad_ptr = (*plan_ref_iter)->Get_Squad();
		squad_ptr->Remove_Match_Reference(*plan_ref_iter);

		Remove_Match(*plan_ref_iter);
	}

	m_committed_agents -= rolled_back_agents;

	match_refs.clear();
}

void Scheduler::Remove_Matches_For_Squad
(
 const Squad_ptr & squad
 )
{
    std::list<Plan_List::iterator> & match_refs = squad->Get_Match_References();
    std::list<Plan_List::iterator>::iterator plan_ref_iter;
	Goal_ptr goal_ptr;

	for (plan_ref_iter = match_refs.begin();
		 plan_ref_iter != match_refs.end();
		 plan_ref_iter++) {
		Assert((*plan_ref_iter)->Get_Squad() == squad);

		m_committed_agents -= (*plan_ref_iter)->Rollback_All_Agents();

		goal_ptr = (*plan_ref_iter)->Get_Goal();
		goal_ptr->Remove_Match_Reference(*plan_ref_iter);

		Remove_Match(*plan_ref_iter);
	}

	match_refs.clear();
}












void Scheduler::Remove_Match(Plan_List::iterator & plan_iter)
{

	m_current_plan_iter = m_matches.erase(plan_iter);
}












sint32 Scheduler::Rollback_Matches_For_Goal
(
 const Goal_ptr & goal
 )
{
    std::list<Plan_List::iterator> & match_refs = goal->Get_Match_References();
    std::list<Plan_List::iterator>::iterator plan_ref_iter;

#ifdef _DEBUG
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1, ("ROLLBACK_MATCHES_FOR_GOAL\n"));
#endif


	CTPGoal_ptr ctpgoal_ptr = (CTPGoal_ptr) goal;

#ifdef _DEBUG
	AI_DPRINTF(k_DBG_SCHEDULER,  m_playerId, -1, -1, ("\t"));
	ctpgoal_ptr->Log_Debug_Info(k_DBG_SCHEDULER);
	AI_DPRINTF(k_DBG_SCHEDULER,  m_playerId, -1, -1, ("\n"));

#endif

	Squad_Strength needed_strength =
		ctpgoal_ptr->Get_Strength_Needed();
#ifdef _DEBUG
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
		("\t\tMissing attack = %3.0f\n",needed_strength.Get_Attack()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
		("\t\tMissing transport = %d\n",needed_strength.Get_Transport()));
	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, -1, -1,
		("\t\tMissing defense = %3.0f\n",needed_strength.Get_Defense()));

#endif


	if (goal->Get_Raw_Priority() > m_maxUndercommittedPriority)
	{
		m_neededSquadStrength = needed_strength;
		m_maxUndercommittedPriority	= goal->Get_Raw_Priority();
	}

	sint32 count=0;

	for (plan_ref_iter = match_refs.begin();
		 plan_ref_iter != match_refs.end();
		 plan_ref_iter++) {

		count += (*plan_ref_iter)->Rollback_All_Agents();
	}
	m_committed_agents -= count;

    DPRINTF(k_DBG_SCHEDULER, ("\t%d agents rolled back for Goal %x, %d.\n", count, goal, goal->Get_Goal_Type()));

	return count;
}













Squad_ptr Scheduler::Form_Squad_From_Goal
(
 const Goal_ptr & goal
 )
{

	Assert(goal->Get_Totally_Complete());

    std::list<Plan_List::iterator> & match_refs = goal->Get_Match_References();
    std::list<Plan_List::iterator>::iterator plan_ref_iter;

	Squad_ptr new_squad = new Squad();

	for (plan_ref_iter = match_refs.begin();
		 plan_ref_iter != match_refs.end();
		 plan_ref_iter++) {

		(*plan_ref_iter)->Move_All_Agents(new_squad);
	}

	m_committed_agents -= new_squad->Get_Num_Agents();

	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, goal->Get_Goal_Type(), -1,
		("FORM_SQUAD_FROM_GOAL squad: %x	goal: %x\n",
		 new_squad,
		 goal));

	return new_squad;
}








bool Scheduler::Add_Transport_Matches_For_Goal(
	 const Sorted_Goal_Iter & goal_iter,
	 Plan_List::iterator & plan_iter
	)
{
	Squad_List::iterator squad_iter;


    std::list<Plan_List::iterator> & matches_to_goal = goal_iter->second->Get_Match_References();
    std::list<Plan_List::iterator>::iterator matches_to_goal_iter;

	for ( matches_to_goal_iter = matches_to_goal.begin();
		  matches_to_goal_iter != matches_to_goal.end();
		  matches_to_goal_iter++ )
		  {

			  if ( (k_Goal_SquadClass_CanTransport_Bit & (*matches_to_goal_iter)->Get_Squad()->Get_Squad_Class()) !=
				  k_Goal_SquadClass_CanTransport_Bit )
				  continue;


			  (*matches_to_goal_iter)->Compute_Matching_Value();
		  }

	Plan_List tmp_list;

	tmp_list.splice(tmp_list.begin(), m_matches, plan_iter, m_matches.end());

	tmp_list.sort(std::greater<Plan>());

	m_matches.splice(m_matches.end(), tmp_list);

	bool match_added = false;


	for ( squad_iter = m_squads.begin(); squad_iter != m_squads.end(); squad_iter++)
	{

		if ( (k_Goal_SquadClass_CanTransport_Bit & (*squad_iter)->Get_Squad_Class()) !=
			  k_Goal_SquadClass_CanTransport_Bit )
			  continue;

		match_added = false;

		for ( matches_to_goal_iter = matches_to_goal.begin();
			  matches_to_goal_iter != matches_to_goal.end();
			  matches_to_goal_iter++ )
		{
			if (*squad_iter == (*matches_to_goal_iter)->Get_Squad())
			{

				match_added = true;
				break;
			}
		}


		if (match_added)
			continue;

		match_added |= Add_New_Match_For_Goal_And_Squad(
			goal_iter,
			squad_iter,
			plan_iter);

	}
	return match_added;
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

	Squad_List::iterator squad_ptr_iter = m_squads.begin();
	while (squad_ptr_iter != m_squads.end() && count < max_count)
	{
		if ((*squad_ptr_iter)->DisbandObsoleteArmies() > 0)
			count++;
		squad_ptr_iter++;
	}
}


Squad_Strength Scheduler::GetMostNeededStrength() const
{
	return m_neededSquadStrength;
}

void Scheduler::SetArmyDetachState(const Army & army, const bool detach)
{

	Squad_List::iterator squad_ptr_iter = m_squads.begin();
	bool found = false;
	while (squad_ptr_iter != m_squads.end() && !found)
	{
		Agent_List & agent_list = (*squad_ptr_iter)->Get_Agent_List();

		Agent_List::iterator agent_iter;
		for (agent_iter = agent_list.begin(); agent_iter != agent_list.end(); agent_iter++)
		{
			CTPAgent_ptr ctp_agent = (CTPAgent_ptr)(*agent_iter);
			if (ctp_agent->Get_Army().m_id == army.m_id)
			{

				ctp_agent->Set_Detached(detach);
				found = true;
				break;
			}
		}

		squad_ptr_iter++;
	}
}

bool Scheduler::GetArmyDetachState(const Army & army) const
{

	Squad_List::const_iterator squad_ptr_iter = m_squads.begin();
	bool found = false;
	while (squad_ptr_iter != m_squads.end() && !found)
	{
		const Agent_List & agent_list = (*squad_ptr_iter)->Get_Agent_List();

		Agent_List::const_iterator agent_iter;
		for (agent_iter = agent_list.begin(); agent_iter != agent_list.end(); agent_iter++)
		{
			const CTPAgent_ptr ctp_agent = (CTPAgent_ptr)(*agent_iter);
			if (ctp_agent->Get_Army().m_id == army.m_id)
			{
				return ctp_agent->Get_Detached();
			}
		}

		squad_ptr_iter++;
	}
	return false;
}













void Scheduler::SetContactCache(int player)
{
	m_contactCachedPlayer = player;

	if (player < 0) return;
	m_contactCache = 0;
	sint32 opponent;
	for (opponent=0;opponent<k_MAX_PLAYERS;++opponent) {

		if (opponent==player) continue;
		if (g_player[player]->HasContactWith(opponent)) {
			m_contactCache |= (1<<opponent);
		}

	}
}

int Scheduler::CachedHasContactWithExceptSelf(int player1,int player2)
{
	if (player1 == m_contactCachedPlayer) {

#ifdef _DEBUG







		int test1 = g_player[player1]->HasContactWith(player2) && (player1!=player2);
		int test2 = ((m_contactCache>>player2) & 1) != 0;
		Assert(test1==test2);
#endif

		return ((m_contactCache>>player2)&1) != 0;
	}

	if (g_player[player1] == NULL)
		return FALSE;


	if (player1==player2) return FALSE;
	return g_player[player1]->HasContactWith(player2);
}


void Scheduler::SetIsNeutralRegardCache(int player)
{
	m_neutralRegardCachedPlayer = player;

	if (player < 0) return;
	m_neutralRegardCache = 0;
	sint32 opponent;
	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	for (opponent=0;opponent<AgreementMatrix::s_agreements.GetMaxPlayers();++opponent) {

		if ( diplomat.TestEffectiveRegard(opponent, NEUTRAL_REGARD))
		{
			m_neutralRegardCache |= (1<<opponent);
		}
	}
}

int Scheduler::CachedIsNeutralRegard(int player,int opponent)
{
	if (player == m_neutralRegardCachedPlayer) {

#ifdef _DEBUG







		Diplomat & diplomat = Diplomat::GetDiplomat(player);
		int test1 = diplomat.TestEffectiveRegard(opponent, NEUTRAL_REGARD);
		int test2 = ((m_neutralRegardCache>>opponent) & 1) != 0;
		Assert(test1==test2);
#endif
		return ((m_neutralRegardCache>>opponent)&1) != 0;
	}


	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	return diplomat.TestEffectiveRegard(opponent, NEUTRAL_REGARD);
}


void Scheduler::SetIsAllyRegardCache(int player)
{
	m_allyRegardCachedPlayer = player;

	if (player < 0) return;
	m_allyRegardCache = 0;
	sint32 ally;
	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	for (ally=0;ally<AgreementMatrix::s_agreements.GetMaxPlayers();++ally) {

		if ( diplomat.TestAlliedRegard(ally))
		{
			m_allyRegardCache |= (1<<ally);
		}
	}
}

int Scheduler::CachedIsAllyRegard(int player,int ally)
{
	if (player == m_allyRegardCachedPlayer) {

#ifdef _DEBUG







		Diplomat & diplomat = Diplomat::GetDiplomat(player);
		int test1 = diplomat.TestAlliedRegard(ally);
		int test2 = ((m_allyRegardCache>>ally) & 1) != 0;
		Assert(test1==test2);
#endif
		return ((m_allyRegardCache>>ally)&1) != 0;
	}


	Diplomat & diplomat = Diplomat::GetDiplomat(player);
	return diplomat.TestAlliedRegard(ally);
}
