//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : the Goal motherclass
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Resolved ambiguous sqrt calls.
// - Added the Target Position (X,Y) to The ArmyText (the text that is
//   shown on the screen in Debug mode, with the command 'armytext') - calvitix
//   update : it now show if the Army is grouping or going to board...
// - Added an attribute for the goal subtask (used by armytext display)
// - Changed the task forces for goals (based on objectif's threat)
// - Added a Debug Log (activated with k_dbg_scheduler_all) to see the goal priority computing
//   (raw priority, plus value of each modifier)
// - Added an Ungroup condition that can be associated to goals (as it exists RallyFirst) - Calvitix
// - Added consitions for Treaspassing units : (to favorise their Retreat) - Calvitix
// - Added conditions for wounded units : IsWoundedbonus (see goals.txt) - Calvitix
// - Added conditions for territory owner and IsVisible (see goals.txt) - Calvitix
// - Correct the determination of Empire Center and foreign Empire center - Calvitix
// - Invalid goals if in territory with nontrespassing treaty - Calvitix
// - Consider a goal with max armies engaged as satisfied - the limitation of
//   army size : we cannot form a group with more armies than the max (can
//   disturb the goals with RallyFirst() - Calvitix
// - Changes the const attribute for Compute_Matching_Value (Raw_Priority will
//   be changed on wounded case) - Calvitix
// - Linux support modifications + cleanup.
// - Allow incursion permission to stealth units
// - Rollback the determination of check_dest (problem with special units that couldn't find
//   their path to goal anymore
// - Added other conditions to determine the RETREAT goals (and set max of 2 units per city
//   for that goals, hardcoded (to not depend on threat)
// - Forbid to army with settlers to perform ATTACK or SEIGE goals
// - Allow Units that are grouping to both move (if they are far enough)
// - Corrected condition to give unowned bonus in Compute_Raw_Priority, now it is
//   given if the according territory has owner -1 instead of 0
//   - Feb. 21st 2005 Martin Gühmann
// - Removed unnecessary reinitialization in Compute_Needed_Troop_Flow for trade
//   routes and tileimps. - Feb. 21st 2005 Martin Gühmann
// - Started to reimplement the refuel order. - May 7th 2005 Martin Gühmann
// - Removed .NET warnings - May 7th 2005 Martin Gühmann
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Added checks and Asserts to try to prevent crashes.
// - Improved AI debug goal report. (Jan 2nd 2007 Martin Gühmann)
// - Matching value computation now uses the distance measurement from Agent. (25-Jan-2008 Martin Gühmann)
// - Added goody hut goal bonuses, so that there is a bonus if a goody hut is
//   in vision range of the agent and whether there could Barbarians pop up from
//   the goody hut and the goody hut opeing army can defend against such Barbarians. (25-Jan-2008 Martin Gühmann)
// - Fixed Goal subtask handling. (26-Jan-2008 Martin Gühmann)
// - Improved transporter cargo loading. (30-Jan-2008 Martin Gühmann)
// - The AI does not use settlers for attack anymore. (3-Feb-2008 Martin Gühmann)
// - The AI now uses the closest transport to the unit transported, even if
//   there is only one army to be transported. (3-Feb-2008 Martin Gühmann)
// - Use more than one transporter if the goal needs more than one. (8-Feb-2008 Martin Gühmann)
// - Standartized army strength computation. (30-Apr-2008 Martin Gühmann)
// - AI force matches are now based on attack, defense, ranged, land bombard,
//   sea bombard, and air bombard. (30-Apr-2008 Martin Gühmann)
// - USE_LOGGING now works in a final version. (30-Jun-2008 Martin Gühmann)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
// - Strength calcualtion for offensive goals is now based on the average of
//   grid values and target position vales. For Barbarian targets it is
//   only the position strength. (13-Aug-2008 Martin Gühmann)
// - Slavers now go to cities with enough population. (13-Aug-2008 Martin Gühmann)
// - Rally is now complete if there is only one stack left with less than
//   twelve units. (13-Aug-2008 Martin Gühmann)
// - Added new rally algorithm. (13-Aug-2008 Martin Gühmann)
// - Slavers don't go to cities with city walls. (06-Sep-2008 Martin Gühmann)
// - Fixed unit garrison assignment. (23-Jan-2009 Martin Gühmann)
// - Merged in CTPGoal, removed virtual functions, for design and speed
//   improvement. (28-Jan-2009 Martin Gühmann)
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
// - Renamed Get_Totally_Complete to IsTotallyComplete and split it into subfunctions
//   so that we can distinquish between goals with invalid target, goals that cannot
//   be executed, goals without a diplomacy fit and goals that cannot be executed now. (08-Jan-2019 Martin Gühmann)
// - Slavey goals cannot be executed either if slavery has been abolished.(08-Jan-2019 Martin Gühmann)
// - The AI does not incite revolutions in 1 city civs. (08-Jan-2019 Martin Gühmann)
// - Execute_Task is now more explicit why a goal failed. (11-Jan-2019 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Goal.h"

#include <inttypes.h>

using namespace std;

const Utility Goal::BAD_UTILITY = -99999999;
const Utility Goal::MAX_UTILITY =  99999999;

#include "ConstRecord.h"
#include "GoalRecord.h"
#include "OrderRecord.h"
#include "StrategyRecord.h"
#include "WonderRecord.h"

#include "advanceutil.h"
#include "terrainutil.h"
#include "wonderutil.h"
#include "WonderTracker.h"

#include "squad_Strength.h"
#include "agent.h"
#include "ArmyPool.h"
#include "ArmyData.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "CityInfluenceIterator.h"
#include "ctpai.h"
#include "Diplomat.h"
#include "Barbarians.h"
#include "mapanalysis.h"
#include "settlemap.h"
#include "AgreementMatrix.h"
#include "CityAstar.h"

#include "debugassert.h"

#include "gstypes.h"
#include "gfx_options.h"
#include "World.h"

#include "ctpaidebug.h"

extern CityAstar g_city_astar;

Goal::Goal()
:
    m_current_needed_strength       (),
    m_current_attacking_strength    (),
    m_matches                       (),
    m_agents                        (),
    m_playerId                      (PLAYER_UNASSIGNED),
    m_raw_priority                  (BAD_UTILITY),
    m_combinedUtility               (0),
    m_target_pos                    (),
    m_target_city                   (),
    m_target_army                   (),
    m_sub_task                      (SUB_TASK_GOAL),
    m_goal_type                     (GOAL_TYPE_NULL),
    m_needs_sorting                 (false)
{
}

Goal::Goal(const Goal &goal)
:
    m_current_needed_strength       (goal.m_current_needed_strength),
    m_current_attacking_strength    (0),                         // Nothing since the agent list is not copied
    m_matches                       (),                          // Contains refernces that are invalid after copy
    m_agents                        (),                          // Agents are just pointers, which are changed on copy
    m_playerId                      (goal.m_playerId),
    m_raw_priority                  (goal.m_raw_priority),
    m_combinedUtility               (goal.m_combinedUtility),
    m_target_pos                    (goal.m_target_pos),
    m_target_city                   (goal.m_target_city),
    m_target_army                   (goal.m_target_army),
    m_sub_task                      (goal.m_sub_task),
    m_goal_type                     (goal.m_goal_type),
    m_needs_sorting                 (goal.m_needs_sorting)
{
}

Goal::~Goal()
{
	// Nothing to delete, references only
}

Goal& Goal::operator= (const Goal &goal)
{
	m_goal_type                  = goal.m_goal_type;
	m_playerId                   = goal.m_playerId;
	m_raw_priority               = goal.m_raw_priority;
	m_current_needed_strength    = goal.m_current_needed_strength;
	m_current_attacking_strength = goal.m_current_attacking_strength;
	m_matches                    = goal.m_matches;
	m_agents                     = goal.m_agents;
	m_combinedUtility            = goal.m_combinedUtility;
	m_needs_sorting              = goal.m_needs_sorting;
	m_target_pos                 = goal.m_target_pos;
	m_target_city                = goal.m_target_city;
	m_target_army                = goal.m_target_army;
	m_sub_task                   = goal.m_sub_task;

	Assert(false); // Hopefully not used
	return *this;
}

bool Goal::operator == (const Goal & rval) const
{
	return (m_playerId    == rval.m_playerId)       &&
	       (m_goal_type   == rval.m_goal_type)      &&
	       (m_target_army == rval.m_target_army)    &&
	       (m_target_city == rval.m_target_city)    &&
	       (m_target_pos  == rval.m_target_pos);
}

bool Goal::Is_Satisfied() const
{
	if(m_agents.size() == 0)
		return false;

	// Limitation of army size: Cannot form a group with more
	// armies than the max (without that limitation, it can
	// disturb the goals with RallyFirst() - Calvitix
//	if (m_current_attacking_strength.Get_Agent_Count() == k_MAX_ARMY_SIZE)
//		return true;

	if(g_theGoalDB->Get(m_goal_type)->GetNeverSatisfied())
	{
//		return false; // Another problem to be fixed but this has to wait, and not here
	}

//	if(m_current_needed_strength > m_current_attacking_strength)
	return m_current_attacking_strength.HasEnough(m_current_needed_strength);
}

void Goal::Commit_Agent(const Agent_ptr & agent)
{
#ifdef _DEBUG_SCHEDULER
	for
	   (
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	   )
	{
		Assert((*agent_iter) != agent);
	}
#endif

	MapPoint dest_pos = Get_Target_Pos();     // Get cheap target position first, no need for pillage checking, yet.
	MapPoint curr_pos = agent->Get_Pos();

	if(agent->IsNeededForGarrison() && dest_pos != curr_pos)
	{
		return;
	}

	Squad_Strength strength = agent->Compute_Squad_Strength();
	strength += m_current_attacking_strength;
	double oldMissingStrength = m_current_needed_strength.GetTotalMissing(m_current_attacking_strength);
	double newMissingStrength = m_current_needed_strength.GetTotalMissing(strength);

	if(
	      oldMissingStrength > newMissingStrength
	   ||
	      (
	           Needs_Transporter() // Add function
	        && strength.Get_Transport() > m_current_attacking_strength.Get_Transport()
	      )
	){
		m_current_attacking_strength.Add_Agent_Strength(agent);

		m_agents.push_back(agent);

		agent->Set_Goal(this);

		Assert(m_current_attacking_strength.Get_Agent_Count() >= static_cast<sint8>(m_agents.size()));
	}
}

void Goal::Rollback_Agent(Agent_ptr agent_ptr)
{
	if(m_agents.size() == 0)
		return;

	Agent_List::iterator next_agent_iter;
	for
	(
	      next_agent_iter  = m_agents.begin();
	      next_agent_iter != m_agents.end();
	    ++next_agent_iter
	)
	{
		if(agent_ptr == *next_agent_iter)
		{
			break;
		}
	}

	Assert(next_agent_iter != m_agents.end());

	if(next_agent_iter == m_agents.end())
	{
		return;
	}

	Rollback_Agent(next_agent_iter);
}

void Goal::Rollback_Agent(Agent_List::iterator & agent_iter)
{
	Agent_ptr agent_ptr = *agent_iter;

	Assert(agent_ptr);

	if(!agent_ptr->Get_Is_Dead()
	&&  g_player[m_playerId]
	&&  g_player[m_playerId]->IsRobot()
	){
		agent_ptr->ClearOrders();
	}

	m_current_attacking_strength.Remove_Agent_Strength(agent_ptr);

	agent_iter = m_agents.erase(agent_iter);

	Assert(m_current_attacking_strength.Get_Agent_Count() >= static_cast<sint8>(m_agents.size()));

	agent_ptr->Set_Goal(NULL);
}

bool Goal::Can_Be_Executed() const
{
	bool can_be_executed = false;

	Agent_List::const_iterator agent_iter;

	for
	(
	      agent_iter  = m_agents.begin();
	      agent_iter != m_agents.end();
	    ++agent_iter
	)
	{
		can_be_executed |= (*agent_iter)->Get_Can_Be_Executed();
	}

	return can_be_executed;
}

void Goal::Set_Can_Be_Executed(const bool & can_be_executed)
{
	Agent_List::iterator agent_iter;

	for
	(
	      agent_iter  = m_agents.begin();
	      agent_iter != m_agents.end();
	    ++agent_iter
	)
	{
		(*agent_iter)->Set_Can_Be_Executed(can_be_executed);
	}
}

void Goal::Set_Needs_Transporter(const bool needs_transporter)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		match_iter->Set_Needs_Transporter(needs_transporter);
	}
}

void Goal::Set_Needs_Transporter(Agent_ptr agent_ptr)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(agent_ptr == match_iter->Get_Agent())
			match_iter->Set_Needs_Transporter(true);
	}
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
	return m_matches.size() > 0;
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

bool Goal::Needs_Transporter() const
{
	Assert(m_current_attacking_strength.Get_Transport() >= 0);

	return m_current_needed_strength.Get_Transport() - m_current_attacking_strength.Get_Transport() > 0;
}

const Squad_Strength Goal::Get_Strength_Needed() const // Rename to missing strength
{
	Squad_Strength needed_strength  = m_current_needed_strength;
	               needed_strength -= m_current_attacking_strength;

	return needed_strength;
}

Utility Goal::Compute_Matching_Value(Plan_List & matches, const bool update)
{
	if(IsInvalid())
	{
		AI_DPRINTF
		          (
		           k_DBG_SCHEDULER_DETAIL,
		           m_playerId,
		           m_goal_type,
		           -1,
		           (
		            "\tCompute Matching Value for goal: %s, raw_match: %i (%s)\n",
		            g_theGoalDB->Get(m_goal_type)->GetNameText(),
		            m_raw_priority,
		            "Invalid"
		           )
		          );

		m_combinedUtility = Goal::BAD_UTILITY;

		return m_combinedUtility;
	}

	AI_DPRINTF
	          (
	           k_DBG_SCHEDULER_DETAIL,
	           m_playerId,
	           m_goal_type,
	           -1,
	           (
	            "\tCompute Matching Value for goal: %s, raw_match: %i (%s)\n",
	            g_theGoalDB->Get(m_goal_type)->GetNameText(),
	            m_raw_priority,
	            (g_theWorld->HasCity(Get_Target_Pos()) ? g_theWorld->GetCity(Get_Target_Pos()).GetName() : "field")
	           )
	          );

	sint32 count = 0;

	for
	(
	    Plan_List::iterator match_iter  = matches.begin();
	                        match_iter != matches.end();
	                      ++match_iter
	)
	{
		// Maybe this also needs to be handled.
		if(!match_iter->Plan_Is_Needed_And_Valid())
		{
			Assert(false);
			continue;
		}

		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\t[%3d] ", count));

		Utility matchUtility = match_iter->Compute_Matching_Value(this);

		++count;
	}

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
			("\t\tThere were %3d matches\n", count));

	Sort_Matches(matches);
	return Recompute_Matching_Value(matches, update);
}

Utility Goal::Recompute_Matching_Value(Plan_List & matches, const bool update, const bool show_strength)
{
	if(!update)
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
					("\tCompute Matching Value for goal: %x, %s, raw_match: %i\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText(), m_raw_priority));
	}

	const GoalRecord * goal_record  = g_theGoalDB->Get(m_goal_type);

	Utility combinedUtility = 0;

	sint32 count = 0;
	Squad_Strength projected_strength;
	for
	(
	    Plan_List::iterator
	            match_iter  = matches.begin();
	            match_iter != matches.end();
	          ++match_iter
	)
	{
		if(!match_iter->All_Unused_Or_Used_By_This(this))
			continue;

		if(match_iter->Get_Needs_Cargo())
			continue;

		if(!projected_strength.HasEnough(m_current_needed_strength)
//		||  goal_record->GetNeverSatisfied() // Should be considered in Commit_Agents
		){
			Utility matchUtility = match_iter->Get_Matching_Value();

			if(matchUtility > Goal::BAD_UTILITY)
			{
				projected_strength += match_iter->Get_Agent()->Get_Squad_Strength();

				combinedUtility += matchUtility;
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
				            ("\t\t[%3d] match = %d %s Army: %9x Agent: %9x (%3d, %3d)\t%20s (units %2d, cargo %2d)\n", 
				             count,
				             matchUtility,
				             g_theGoalDB->Get(m_goal_type)->GetNameText(),
				             match_iter->Get_Agent()->Get_Army(),
				             match_iter->Get_Agent(),
				             match_iter->Get_Agent()->Get_Pos().x,
				             match_iter->Get_Agent()->Get_Pos().y,
				             g_theUnitDB->GetNameStr(match_iter->Get_Agent()->Get_Army()->Get(0).GetType()),
				             match_iter->Get_Agent()->Get_Army()->Num(),
				             match_iter->Get_Agent()->Get_Army()->GetCargoNum()));
				++count;
			}
			else
			{
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
					("\t\t[%3d]First match with bad utility for goal %s, not enough, stop matching, last match in list: %i\n", count, g_theGoalDB->Get(m_goal_type)->GetNameText(), matches.rbegin()->Get_Matching_Value()));
				if(count == 0)
				{
					Log_Debug_Info(k_DBG_SCHEDULER_DETAIL);
				}
				break;
			}
		}
		else
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
						("\t\t[%3d] Enough ressources found for goal %s\n", count, g_theGoalDB->Get(m_goal_type)->GetNameText()));
			break;
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	if(CtpAiDebug::DebugLogCheck(m_playerId, -1, -1))
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1, ("\n"));
		projected_strength          .Log_Debug_Info(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, "The Projected Strength:          ");
		m_current_needed_strength   .Log_Debug_Info(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, "The Needed Strength:             ");
		Squad_Strength strength;
		strength.Set_Pos_Strength(Get_Target_Pos());
		strength                    .Log_Debug_Info(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, "The Target Pos Strength:         ");
		Squad_Strength grid_strength;
		grid_strength.Set_Enemy_Grid_Strength(Get_Target_Pos(), m_playerId);
		grid_strength               .Log_Debug_Info(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, "The Target Enemy Grid Strength:  ");
		grid_strength.Set_Allied_Grid_Strength(Get_Target_Pos(), m_playerId);
		grid_strength               .Log_Debug_Info(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, "The Target Allied Grid Strength: ");
	}
#endif

	if
	  (
	       (
	            !projected_strength.HasEnough(m_current_needed_strength)
	         && !goal_record->GetExecuteIncrementally()
	       )
	    || count == 0
	  )
	{
		combinedUtility = Goal::BAD_UTILITY;
	}
	else
	{
		combinedUtility /= count;
	}

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,("\tMatch value combined utility: %i, raw priority: %i, execute incrementally: %i\n", combinedUtility, m_raw_priority, goal_record->GetExecuteIncrementally()));
	if(update)
	{
		m_combinedUtility = combinedUtility;
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\n"));

		return Get_Matching_Value();
	}
	else
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\n"));

		return (  !g_theGoalDB->Get(m_goal_type)->GetIsGlobalGoal()
		        || combinedUtility == Goal::BAD_UTILITY) ?
		           combinedUtility : m_raw_priority;
	}
}

Utility Goal::Get_Matching_Value() const
{
	return (  !g_theGoalDB->Get(m_goal_type)->GetIsGlobalGoal()
	        || m_combinedUtility == Goal::BAD_UTILITY) ?
	           m_combinedUtility : m_raw_priority;
}

void Goal::Set_Matching_Value(Utility combinedUtility)
{
	m_combinedUtility = combinedUtility;
}

bool Goal::Add_Match(const Agent_ptr & agent, const bool update_match_value, const bool needsCargo)
{
#if defined(_DEBUG)
	for
	   (
	    Plan_List::iterator   plan_test_iter  = m_matches.begin();
	                          plan_test_iter != m_matches.end();
	                        ++plan_test_iter
	   )
	{
		Assert(plan_test_iter->Get_Agent() != agent);
	}
#endif

	if(!agent->Get_Is_Dead())
	{
		Plan the_match(agent, needsCargo);

		Utility matchingValue = Goal::BAD_UTILITY;

		if(update_match_value)
		{
			matchingValue = the_match.Compute_Matching_Value(this);
		}

		if(!update_match_value || matchingValue > Goal::BAD_UTILITY)
		{
			m_matches.push_back(the_match);

			m_needs_sorting = true;

			return true;
		}
	}

	return false;
}

bool Goal::CanGoalBeReevaluated() const
{
	GoalRecord const *  goalRecord      = g_theGoalDB->Get(m_goal_type);

	Assert(goalRecord);

	return goalRecord && !goalRecord->GetNoRollback();
}

bool Goal::Commited_Agents_Need_Orders() const
{
	for
	(
	    Plan_List::const_iterator   match_iter  = m_matches.begin();
	                                match_iter != m_matches.end();
	                              ++match_iter
	)
	{
		if(match_iter->Get_Agent()->Has_Goal(this))
		{
			if(match_iter->Get_Agent()->Get_Army()->NumOrders() <= 0)
				return true;
		}
	}

	return false;
}

void Goal::Rollback_All_Agents()
{
	if(m_agents.size() == 0)
		return;

	Agent_List::iterator agent_iter;
	for
	(
	      agent_iter  = m_agents.begin();
	      agent_iter != m_agents.end();
	      Rollback_Agent(agent_iter)
	)
	{
	}

	Assert(m_current_attacking_strength.NothingNeeded());
}

void Goal::Commit_Agents()
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(match_iter->Get_Matching_Value() <= Goal::BAD_UTILITY)
		{
			break;
		}
		else if(Is_Satisfied() || IsTotallyComplete())
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
				("\t\tNO MORE AGENTS NEEDED:           (goal: %x agent: %x, id: 0%x)\n", this, match_iter->Get_Agent(), match_iter->Get_Agent()->Get_Army().m_id));
			break;
		}
		else
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
				("\t\tAGENTS CAN BE COMMITTED:       (goal: %x agent: %x, id: 0%x)\n", this, match_iter->Get_Agent(), match_iter->Get_Agent()->Get_Army().m_id));

			if(!match_iter->Get_Needs_Cargo())
			{
				match_iter->Commit_Agent_Common(this);
			}
		}
	}

	if(Get_Agent_Count() > 0)
	{
		Log_Debug_Info(
		               (Is_Satisfied() || Is_Execute_Incrementally()) ?
		               k_DBG_SCHEDULER :
		               k_DBG_SCHEDULER_DETAIL
		              );
	}
}

void Goal::Commit_Transport_Agents()
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin(); // Maybe resort the matches, by agents that need a transporter and their distance
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(match_iter->Get_Matching_Value() <= Goal::BAD_UTILITY)
		{
			break;
		}
		else if(!Needs_Transporter() || IsTotallyComplete())
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1,
				("\t\tNO MORE TRANSPORT AGENTS NEEDED: (goal: %x agent: %x, id: 0%x)\n", this, match_iter->Get_Agent(), match_iter->Get_Agent()->Get_Army().m_id));
			break;
		}
		else if(match_iter->Get_Cannot_Be_Used())
		{
			break;
		}
		else if(match_iter->Get_Needs_Cargo())
		{
			if(match_iter->Get_Agent()->Get_Army()->CanTransport())
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1,
					("\t\tTRANSPORT AGENTS COMMITTED:    (goal: %x agent: %x, id: 0%x)\n", this, match_iter->Get_Agent(), match_iter->Get_Agent()->Get_Army().m_id));
				match_iter->Commit_Agent_Common(this);
			}
		}
	}
}

void Goal::Remove_Matches()
{
	Rollback_All_Agents();

	m_matches.clear();
}

void Goal::Remove_Match(const Agent_ptr & agent)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(agent == match_iter->Get_Agent())
		{
			if(agent->Has_Goal(this))
			{
				Rollback_Agent(agent);
			}

			m_matches.erase(match_iter);

			return;
		}
	}
}
void Goal::Rollback_Emptied_Transporters()
{
	if(m_agents.size() == 0)
		return;

	Agent_List::iterator agent_iter;
	for
	(
	      agent_iter  = m_agents.begin();
	      agent_iter != m_agents.end();
	    ++agent_iter
	)
	{
		Agent_ptr agent_ptr = *agent_iter;
		const MapPoint pos     = agent_ptr->Get_Target_Pos();
		MapPoint goalPos(-1,-1);

		if(Get_Target_Army().m_id == 0 || Get_Target_Army().IsValid())
		{
			goalPos = Get_Target_Pos();
		}

		if(pos == goalPos)
		{
			if(!Pretest_Bid(agent_ptr, goalPos))
			{
				AI_DPRINTF(k_DBG_SCHEDULER, agent_ptr->Get_Army()->GetOwner(), m_goal_type, -1,
					("\t\tTransporter (%s) not needed anymore, removing from goal\n", g_theUnitDB->GetNameStr(agent_ptr->Get_Army()->Get(0)->GetType())));

				Rollback_Agent(agent_iter); // increases the agent iterator
				--agent_iter;
			}
		}
	}
}

bool Goal::Can_Transport_Any_Width_Need(Agent* agent)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(match_iter->Get_Needs_Transporter() && match_iter->Get_Agent()->Get_Army()->NumUnitsCanMoveIntoThisTransport(*agent->Get_Army().GetData()))
		{
			return true;
		}
	}

	return false;
}

bool Goal::Has_Agent_And_Set_Needs_Cargo(Agent* agent)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(agent == match_iter->Get_Agent())
		{
			match_iter->Set_Needs_Cargo(true);
			return true;
		}
	}

	return false;
}

bool Goal::Needs_Cargo(Agent* agent)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(agent == match_iter->Get_Agent())
		{
			return match_iter->Get_Needs_Cargo();
		}
	}

	return false;
}

bool Goal::Cannot_Be_Used(Agent* agent)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(agent == match_iter->Get_Agent())
		{
			return match_iter->Get_Cannot_Be_Used();
		}
	}

	return false;
}

void Goal::Set_Cannot_Be_Used(Agent* agent, bool cannotBeUsed)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(agent == match_iter->Get_Agent())
		{
			match_iter->Set_Cannot_Be_Used(cannotBeUsed);
		}
	}
}

void Goal::Recompute_Current_Attacking_Strength()
{
	m_current_attacking_strength = Squad_Strength(0);

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		m_current_attacking_strength += (*agent_iter)->Get_Squad_Strength();
	}
}

Squad_Strength Goal::Compute_Current_Strength()
{
	Squad_Strength strength;

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		strength += (*agent_iter)->Get_Squad_Strength();
	}

	return strength;
}

void Goal::Sort_Matches(Plan_List & matches)
{
	matches.sort(greater< Plan >());
	m_needs_sorting = false;
}

void Goal::Sort_Matches_If_Necessary()
{
	if(m_needs_sorting)
	{
		Sort_Matches(m_matches);
	}
}

void Goal::Set_Target_Pos(const MapPoint & pos)
{
	m_target_pos = pos;
}

void Goal::Set_Target_Army(const Army & army)
{
	m_target_army = army;
}

void Goal::Set_Target_City(const Unit & city)
{
	m_target_city = city;
}

const MapPoint Goal::Get_Target_Pos(const Army & army) const
{
	const GoalRecord *rec               = g_theGoalDB->Get(m_goal_type);

	if (rec->GetTargetTypeTradeRoute())
	{
		const sint32      max_squared_dist  = 0x7fffffff;
		sint32            best_squared_dist = 0x7fffffff;
		MapPoint          best_target_pos;

		Assert( m_target_city.m_id != 0);

		const TradeDynamicArray* trade_routes =
			m_target_city.GetCityData()->GetTradeSourceList();
		for(sint32 i = 0; i < trade_routes->Num(); i++)
		{
			const DynamicArray<MapPoint>* path = (*trade_routes)[i].GetPath();

			for (sint32 j = 1; j < path->Num()-1; j++)
			{
				Cell *cell = g_theWorld->GetCell(path->Get(j));

				if (cell->HasCity())
					continue;

				if (cell->CanEnter(army->GetMovementType()))
				{
					sint32 tmp_squared_dist =
					MapPoint::GetSquaredDistance(path->Get(j), army->RetPos());
					if (tmp_squared_dist < best_squared_dist)
					{
						best_squared_dist = tmp_squared_dist;
						best_target_pos = path->Get(j);
					}
				}
			}

			if (best_squared_dist < max_squared_dist)
				return best_target_pos;
			else
			{
				bool NO_TRADE_ROUTE_TARGET_POS_FOUND = true;
				Assert(NO_TRADE_ROUTE_TARGET_POS_FOUND);
			}
		}
	}
	else if (rec->GetTargetTypeImprovement())
	{
		Assert( m_target_city.m_id != 0);

		if (m_target_city->GetCityData()->WasTerrainImprovementBuilt())
		{
			const sint32      max_squared_dist  = 0x7fffffff;
			sint32            best_squared_dist = 0x7fffffff;
			MapPoint          best_target_pos;
			CityInfluenceIterator it(m_target_city.RetPos(), m_target_city.GetCityData()->GetSizeIndex());
			for(it.Start(); !it.End(); it.Next())
			{
				Cell *cell = g_theWorld->GetCell(it.Pos());

				if (m_target_city.RetPos() == it.Pos())
					continue;

				if (!(cell->GetCityOwner() == m_target_city))
					continue;

				if (cell->GetNumDBImprovements() <= 0)
					continue;

				if (cell->CanEnter(army->GetMovementType()))
				{
					sint32 tmp_squared_dist = MapPoint::GetSquaredDistance(it.Pos(), army->RetPos());
					if (tmp_squared_dist < best_squared_dist)
					{
						best_squared_dist = tmp_squared_dist;
						best_target_pos = it.Pos();
					}
				}
			}

			if (best_squared_dist < max_squared_dist)
				return best_target_pos;
			else
			{
				bool NO_TILE_IMPROVEMENT_TARGET_POS_FOUND = true;
				Assert(NO_TILE_IMPROVEMENT_TARGET_POS_FOUND);
			}
		}
	}
	else if (rec->GetTargetTypePetrolStation())
	{
		sint32 distance_to_refuel;
		MapPoint refuel_pos;
		CtpAi::GetNearestRefuel(army, army->RetPos(), refuel_pos, distance_to_refuel);
		return refuel_pos;
	}

	return Get_Target_Pos();
}

const MapPoint & Goal::Get_Target_Pos() const
{
	static MapPoint pos;    // ugly life-time extension

	if (m_target_army != ID())
	{
		if (m_target_army.IsValid())
		{
			m_target_army->GetPos(pos);
		}
		else
		{
			pos.x = -1;
			pos.y = -1;
		}
	}
	else if (m_target_city != ID())
	{
		if (m_target_city.IsValid())
		{
			m_target_city.GetPos(pos);
		}
		else
		{
			pos.x = -1;
			pos.y = -1;
		}
	}
	else
	{
		return m_target_pos;
	}

	Assert(pos.IsValid());

	return pos;
}

const Army & Goal::Get_Target_Army() const
{
	return m_target_army;
}

void Goal::Set_Sub_Task(const SUB_TASK_TYPE & sub_task)
{
	m_sub_task = sub_task;
}

const SUB_TASK_TYPE & Goal::Get_Sub_Task() const
{
	return m_sub_task;
}

const Unit & Goal::Get_Target_City() const
{
	return m_target_city;
}

sint32 Goal::Get_Target_Value() const
{
	sint32              value   = 0;
	const GoalRecord *  rec     = g_theGoalDB->Get(m_goal_type);
	Assert(rec);

	if(rec->GetTargetTypeAttackUnit()
	|| rec->GetTargetTypeSpecialUnit()
	){
		const Army &    army = Get_Target_Army();
		sint8           tmpCount;
		float           tmp;
		float           tmpValue;
		army->ComputeStrength(tmp,tmp,tmp,tmpCount,tmpCount,tmp,tmp,tmp,tmpValue, false);
		value = static_cast<sint32>(tmpValue);
	}
	else if(rec->GetTargetTypeCity())
	{
		if(m_target_city.IsValid())
		{
			value = m_target_city->GetCityData()->GetValue();
		}
	}
	else if( rec->GetTargetTypeTradeRoute() )
	{
		const Unit & city = Get_Target_City();
		value = city->GetCityData()->GetGoldFromTradeRoutes();
	}

	return value;
}

PLAYER_INDEX Goal::Get_Target_Owner() const
{
	PLAYER_INDEX       target_owner = PLAYER_UNASSIGNED;
	GoalRecord const * goal_record  = g_theGoalDB->Get(m_goal_type);
	Assert(goal_record);

	if(
	      m_target_army.m_id != 0
	   &&
	   (
	       goal_record->GetTargetTypeAttackUnit()
	    || goal_record->GetTargetTypeSpecialUnit()
	   )
	  )
	{
		target_owner = m_target_army.GetOwner();
	}
	else if(goal_record->GetTargetTypePetrolStation())
	{
		if(m_target_city != ID() || g_theWorld->IsAirfield(Get_Target_Pos()))
		{
			target_owner = g_theWorld->GetOwner(Get_Target_Pos());
		}
		else if(m_target_army != ID())
		{
			target_owner = m_target_army.GetOwner();
		}
	}
	else
	{
		MapPoint pos(Get_Target_Pos());
		if(pos.x >= 0)
		{
			target_owner = g_theWorld->GetOwner(Get_Target_Pos());
		}
	}

	return target_owner;
}

bool Goal::Is_Execute_Incrementally() const
{
	return g_theGoalDB->Get(m_goal_type)->GetExecuteIncrementally();
}

void Goal::Compute_Needed_Troop_Flow()
{
	MapAnalysis &  mapAnalysis = MapAnalysis::GetMapAnalysis();
	const MapPoint pos         = Get_Target_Pos();
	const float   threat      = static_cast<float>(mapAnalysis.GetThreat          (m_playerId, pos));
	const float   attack      = static_cast<float>(mapAnalysis.GetEnemyAttack     (m_playerId, pos));
	const float   defense     = static_cast<float>(mapAnalysis.GetEnemyDefense    (m_playerId, pos));
	const float   ranged      = static_cast<float>(mapAnalysis.GetEnemyRanged     (m_playerId, pos));
	const float   bombardLand = static_cast<float>(mapAnalysis.GetEnemyBombardLand(m_playerId, pos));
	const float   bombardSea  = static_cast<float>(mapAnalysis.GetEnemyBombardSea (m_playerId, pos));
	const float   bombardAir  = static_cast<float>(mapAnalysis.GetEnemyBombardAir (m_playerId, pos));
	const float   value       = static_cast<float>(mapAnalysis.GetEnemyValue      (m_playerId, pos));

	m_current_needed_strength   = Squad_Strength(1);
		// why only one unit ? Why then zero units? - Martin Gühmann
		// by bringing a real army to pirate or pillage, it can be ready for seige or attack
		// a single unit is quite defenseless - Calvitix
		// A real army may not be ready for siege but a few turns later some more units would be
		// there and in the meantime the army was dissolved

	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	if(goal_record->GetTargetTypeChokePoint())
	{
		// Need also attack and ranged strength - Calvitix
		m_current_needed_strength.Set_Attack      (attack       * 0.5f);
		m_current_needed_strength.Set_Defense     (defense      * 0.5f);
		m_current_needed_strength.Set_Ranged      (ranged       * 0.5f);
		m_current_needed_strength.Set_Bombard_Land(bombardLand  * 0.5f);
		m_current_needed_strength.Set_Bombard_Sea (bombardSea   * 0.5f);
		m_current_needed_strength.Set_Bombard_Air (bombardAir   * 0.5f);
		m_current_needed_strength.Set_Value       (value        * 0.5f);
	}
	else if(goal_record->GetTargetTypeImprovement()
	     || goal_record->GetTargetTypeTradeRoute()
	     ){

		m_current_needed_strength.Set_Attack(attack  * 0.5f);
		m_current_needed_strength.Set_Ranged(defense * 0.5f);
		m_current_needed_strength.Set_Value(value);
	}
	else if(goal_record->GetTargetTypeEndgame())
	{
		if(goal_record->GetTargetOwnerSelf())
			m_current_needed_strength.Set_Defense(threat);

		else
			m_current_needed_strength.Set_Attack(threat);

		//to be sure that the global force of the army will be enough
		// (not only a wounded unit for example)
		m_current_needed_strength.Set_Value(value);

	}
	else if(goal_record->GetTargetTypeCity()
	     && goal_record->GetTargetOwnerSelf()
	     ){

		// tweak to obtain RETREAT Goal definition : TO DO - 'cleaner' method - Calvitix
		// Set to 2 to so that units with no goals will retreat to the nearest city
		// (I Prefer that method than the GARRISON troops, that are not able to leave the city)
		// cities will be better defended if there are enough units, otherwise units will be
		// affected to relevant goals
		if(g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity()
		&& g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
		&& goal_record->GetTreaspassingArmyBonus() > 0
		){
			m_current_needed_strength.Set_Agent_Count(2);
		}
		else
		{
			const StrategyRecord & strategy =
				Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

			sint32 offensive_garrison;
			sint32 defensive_garrison;
			sint32 ranged_garrison;
			strategy.GetOffensiveGarrisonCount(offensive_garrison);
			strategy.GetDefensiveGarrisonCount(defensive_garrison);
			strategy.GetRangedGarrisonCount(ranged_garrison);

			// why only defensive units ?
			// added ranged units - Calvitix
			m_current_needed_strength.Set_Defense(threat * 2 / 3);
			m_current_needed_strength.Set_Ranged(threat / 3);
			m_current_needed_strength.Set_Value(value);

			//not used for the moment (only attack or defense strength is considerated
			//(see army_strength > operator) - Calvitix
			m_current_needed_strength.Set_Defenders(static_cast<sint8>(defensive_garrison + offensive_garrison));
			m_current_needed_strength.Set_Ranged_Units(static_cast<sint8>(ranged_garrison));
		}
	}
	else if
	       (
	        (    goal_record->GetTargetTypeAttackUnit()
	          || goal_record->GetTargetTypeCity()
	        )
	        && !goal_record->GetTargetOwnerSelf()
	        && !goal_record->GetTargetTypeSpecialUnit()
	       )
	{
		// A real Attack force, depending on threat
		m_current_needed_strength.Set_Attack      (attack);
		m_current_needed_strength.Set_Defense     (defense);
	//	m_current_needed_strength.Set_Attack      (defense); // Reverse attack and defense
	//	m_current_needed_strength.Set_Defense     (attack);
		m_current_needed_strength.Set_Ranged      (ranged);
		m_current_needed_strength.Set_Bombard_Land(bombardLand);
		m_current_needed_strength.Set_Bombard_Sea (bombardSea);
		m_current_needed_strength.Set_Bombard_Air (bombardAir);
		m_current_needed_strength.Set_Value       (value);

		Squad_Strength strength;
		strength.Set_Pos_Strength(pos);
		//
		if
		  (
		       Get_Target_Owner() != 0
		    || strength.Get_Agent_Count() > 0
		  )
		{
			// Set this to zero, since how many units we need doesn't depent on the number of units at the target.
			strength.Set_Agent_Count(0);
			strength.Set_Defenders(0);
			strength.Set_Ranged_Units(0);
			m_current_needed_strength += strength;
			m_current_needed_strength.Set_Force_Matching(0.5,0.5,0.5,0.5,0.5);
		}
	}
	else if(goal_record->GetTargetTypeBorder())
	{
		// assuming threat is the global strength to use (to be coherent with other changes) - Calvitix
		m_current_needed_strength.Set_Attack      (attack       * 0.5f);
		m_current_needed_strength.Set_Defense     (defense      * 0.5f);
		m_current_needed_strength.Set_Ranged      (ranged       * 0.5f);
		m_current_needed_strength.Set_Bombard_Land(bombardLand  * 0.5f);
		m_current_needed_strength.Set_Bombard_Sea (bombardSea   * 0.5f);
		m_current_needed_strength.Set_Bombard_Air (bombardAir   * 0.5f);
		m_current_needed_strength.Set_Value       (value        * 0.5f); // Actually this stuff should go to the force matches
	}
	else if (   goal_record->GetTargetTypeSettleLand()
	        ||  goal_record->GetTargetTypeSettleSea()
	        ||  goal_record->GetTargetTypePetrolStation()
	        )
	{
		// No strength is needed
	}
	else
	{
		m_current_needed_strength.Set_Pos_Strength(pos);
	}

	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();
	const StrategyRecord::ForceMatch *force_match = NULL;

	switch (goal_record->GetForceMatch())
	{
	case k_Goal_ForceMatch_Offensive_Bit:
		force_match = strategy.GetOffensivePtr();
		break;
	case k_Goal_ForceMatch_Defensive_Bit:
		force_match = strategy.GetDefensivePtr();
		break;
	case k_Goal_ForceMatch_StealthAttack_Bit:
		force_match = strategy.GetStealthAttackPtr();
		break;
	case k_Goal_ForceMatch_Bombard_Bit:
		force_match = strategy.GetBombardPtr();
		break;
	case k_Goal_ForceMatch_Special_Bit:
		force_match = strategy.GetSpecialPtr();
		break;
	case k_Goal_ForceMatch_Harass_Bit:
		force_match = strategy.GetHarassPtr();
		break;
	default:

		Assert(false);
	}

	Assert(force_match);
	m_current_needed_strength.Set_Force_Matching(
		static_cast<float>(force_match->GetAttackMatch()),
		static_cast<float>(force_match->GetDefenseMatch()),
		static_cast<float>(force_match->GetRangedMatch()),
		static_cast<float>(force_match->GetBombardMatch()),
		static_cast<float>(force_match->GetValueMatch()));

	// Set_Pos_Strength also retrieves the transport capacity at pos
	// which of course need not be matched
	m_current_needed_strength.Set_Transport(0);
}

Utility Goal::Compute_Agent_Matching_Value(const Agent_ptr agent_ptr) const
{
#if defined(_DEBUG)
	Player *player_ptr = g_player[ m_playerId ];
	Assert(player_ptr && agent_ptr);
#endif

	if(agent_ptr->Get_Is_Dead())
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Is dead\n"));
		return Goal::BAD_UTILITY;
	}

	MapPoint dest_pos = Get_Target_Pos();     // Get cheap target position first, no need for pillage checking, yet.
	MapPoint curr_pos = agent_ptr->Get_Pos();

	Utility bonus = 0;

	if(agent_ptr->IsNeededForGarrison())
	{
		if(dest_pos != curr_pos)
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Needed for garrison, but not in target city\n"));
			return Goal::BAD_UTILITY;
		}
		else
		{
			bonus = g_theGoalDB->Get(m_goal_type)->GetGarrisonBonus();
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)  // Add a debug report of goal computing (raw priority and all modifiers)
	double report_garrison_bonus = bonus;
#endif //_DEBUG


	// This is expensive, because of pillage, get city target first.
	dest_pos = Get_Target_Pos(agent_ptr->Get_Army());

	// This is expensive, so remove first the garrison units.
	if(!Pretest_Bid(agent_ptr, dest_pos))
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: No pretest bid\n"));
		return Goal::BAD_UTILITY;
	}

	if(
	       g_theGoalDB->Get(m_goal_type)->GetNoTransport()
	    &&
	       (
	           (
	                !g_theWorld->IsOnSameContinent(dest_pos, curr_pos)
	             && !agent_ptr->Get_Army()->GetMovementTypeAir()
	           )
	        || agent_ptr->Get_Army()->HasCargo() // Actually we do not want to have any transporters involved
	       )
	  )
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: No transport allowed\n"));
		return Goal::BAD_UTILITY;
	}

	PLAYER_INDEX target_owner = Get_Target_Owner();
	const Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);

	if( target_owner > 0 &&
		(!diplomat.IncursionPermission(target_owner)))
	{
		bool isspecial, cancapture, haszoc, canbombard;
		bool isstealth;
		sint32 maxattack, maxdefense;

		if(!agent_ptr->Get_Army()->HasCargo())
		{
			agent_ptr->Get_Army()->CharacterizeArmy( isspecial,
				isstealth,
				maxattack,
				maxdefense,
				cancapture,
				haszoc,
				canbombard);
		}
		else
		{
			agent_ptr->Get_Army()->CharacterizeCargo( isspecial,
				isstealth,
				maxattack,
				maxdefense,
				cancapture,
				haszoc,
				canbombard);
		}

		if (!isspecial || maxattack > 0 || haszoc)
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: IsNotSpecial, CanAttack, HasZOC\n"));
			return Goal::BAD_UTILITY;
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	// Maybe this is of some use later
	bool is_transporter = false;
#endif

	sint32 transports, max,empty;
	if(Needs_Transporter()
	&& agent_ptr->Get_Army()->GetCargo(transports, max, empty)
	&& empty > 0
	){
		Utility transport_utility = 0;
		Utility           utility = 0;

		sint32            count   = 0;

		for
		(
		    Plan_List::const_iterator match_iter  = m_matches.begin();
		                              match_iter != m_matches.end();
		                            ++match_iter
		)
		{
			Agent_ptr agent_trans_ptr = match_iter->Get_Agent();

			if
			  (
			      !agent_trans_ptr->Get_Is_Dead()
			   &&  match_iter->Get_Needs_Transporter()
			   &&  agent_trans_ptr->EstimateTransportUtility(agent_ptr, utility)
			  )
			{
				transport_utility += utility;

				++count;
			}
		}

		if(count > 0)
		{
			Utility match = (transport_utility / count);
			return match + Get_Raw_Priority();
		}

#if defined(_DEBUG) || defined(USE_LOGGING)
// Maybe this is of some use later
		is_transporter = true;
#endif
	}

	if(agent_ptr->Get_Army()->HasCargo() && !CanReachTargetContinent(agent_ptr))
	{
		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Cargo cannot reach target continent\n"));
		return Goal::BAD_UTILITY;
	}

	//Set if unit is wounded and it is a retreat of defense goal, add bonus
	//to goalpriority + matching

	PLAYER_INDEX PosOwner = g_theWorld->GetOwner(curr_pos);

	if(g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity()
	&& g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
	){
		if(g_theGoalDB->Get(m_goal_type)->GetNeedsRoom()
		&& agent_ptr->Get_Army()->Num() + g_theWorld->GetCell(dest_pos)->GetNumUnits() > k_MAX_ARMY_SIZE)
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Not enough room in target city\n"));
			return Goal::BAD_UTILITY;
		}

		// For Defend or Retreat goals
		if
		  (
		        agent_ptr->Get_Army()->HasCargo()
		    &&  agent_ptr->Get_Army()->IsCargoWounded()
		    && !agent_ptr->Get_Army()->IsCargoObsolete()
		    || !agent_ptr->Get_Army()->HasCargo()
		    &&  agent_ptr->Get_Army()->IsWounded()
		    && !agent_ptr->Get_Army()->IsObsolete()
		  )
		{
			bonus+= g_theGoalDB->Get(m_goal_type)->GetWoundedArmyBonus();
		}
		if(PosOwner != m_playerId
		&&!diplomat.IncursionPermission(PosOwner)
		){
			bonus+= g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus();
		}
	}
	else if((g_theGoalDB->Get(m_goal_type)->GetTargetOwnerColdEnemy()
	     ||  g_theGoalDB->Get(m_goal_type)->GetTargetOwnerHotEnemy())
	     && (g_theGoalDB->Get(m_goal_type)->GetTargetTypeAttackUnit()
	     ||  g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity())
	     )  //For Attack goals (unit or city)
	{
		if(agent_ptr->Get_Army()->CanSettle())
		{
			if
			  (
			       g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity()
			    && m_target_city.IsValid()
			    && g_theWorld->GetCell(m_target_city->GetPos())->GetNumUnits() == 0
			  )
			{
				// Opportunity action march into the city even if the army contains a settler
			}
			else
			{
				// If there is a settler in the army...
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Do not fight with a settler in the army\n"));
				return Goal::BAD_UTILITY;
			}
		}

		if(m_target_army.IsValid() && !agent_ptr->Get_Army()->CanFight(*m_target_army.AccessData()))
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Army cannot fight\n"));
			return Goal::BAD_UTILITY;
		}

		if
		  (
		        agent_ptr->Get_Army()->HasCargo()
		    &&  agent_ptr->Get_Army()->IsCargoWounded()
		    && !agent_ptr->Get_Army()->IsCargoObsolete()
		    || !agent_ptr->Get_Army()->HasCargo()
		    &&  agent_ptr->Get_Army()->IsWounded()
		    && !agent_ptr->Get_Army()->IsObsolete()
		  )
		{
			bonus+= g_theGoalDB->Get(m_goal_type)->GetWoundedArmyBonus();
		}

		if(PosOwner != m_playerId && !diplomat.IncursionPermission(PosOwner))
		{
			bonus += g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus();
		}
	}
	else if((g_theGoalDB->Get(m_goal_type)->GetTargetOwnerColdEnemy()
	     ||  g_theGoalDB->Get(m_goal_type)->GetTargetOwnerHotEnemy())
	     && (g_theGoalDB->Get(m_goal_type)->GetTargetTypeTradeRoute())
	     )  // For trade routes
	{
		if(agent_ptr->Get_Army()->CanSettle()) // CargoCanSettle
		{
			// If there is a settler in the army...
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Settler in the army\n"));
			return Goal::BAD_UTILITY;
		}
	}
#if defined(_DEBUG) || defined(USE_LOGGING)  // Add a debug report of goal computing (raw priority and all modifiers)
	double report_wounded = bonus - report_garrison_bonus;
#endif //_DEBUG

	if(agent_ptr->Get_Army()->HasCargo() ? agent_ptr->Get_Army()->IsCargoObsolete() : agent_ptr->Get_Army()->IsObsolete())
		bonus += g_theGoalDB->Get(m_goal_type)->GetObsoleteArmyBonus();

#if defined(_DEBUG) || defined(USE_LOGGING)  // Add a debug report of goal computing (raw priority and all modifiers)
	double report_obsolete = bonus - report_wounded;
#endif //_DEBUG

	MapPoint point(-1, -1);

	if(Needs_Transporter())
	{
		sint32 transports;
		sint32 max;
		sint32 empty;

		if(
		       agent_ptr->Get_Army()->GetCargo(transports, max, empty)
		    && empty > 0
		  )
		{
			point = GetClosestCargoPos(agent_ptr);       // If is transporter
		}
		else
		{
			point = GetClosestTransporterPos(agent_ptr); // If needs to be transported
		}
	}
	else
	{
		point = dest_pos;
	}

	point = (!point.IsValid()) ? dest_pos : point;

	sint32 squared_distance = 0;
	double eta = agent_ptr->GetRoundsPrecise(point, squared_distance);
	double cell_dist = sqrt(static_cast<double>(squared_distance));

	Utility raw_priority = Get_Raw_Priority();

	const StrategyRecord & strategy =
		Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	sint32 distance_modifier = 1;
	strategy.GetDistanceModifierFactor(distance_modifier);
	Utility time_term = static_cast<Utility>( (eta * distance_modifier) + cell_dist);

	if(g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus() > 0)
	{
		PLAYER_INDEX pos_owner = g_theWorld->GetCell(curr_pos)->GetOwner();

		bool incursion_permissin = Diplomat::GetDiplomat(m_playerId).IncursionPermission(pos_owner);
		if (pos_owner >= 0 && !(incursion_permissin))
		{
			bonus += g_theGoalDB->Get(m_goal_type)->GetTreaspassingArmyBonus();
		}
	}
#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers)
	double report_Treaspassing   = bonus - report_obsolete;
	double report_InVisionRange  = 0.0;
	double report_NoBarbsPresent = 0.0;
#endif //_DEBUG

	if(agent_ptr->Get_Army()->IsInVisionRangeAndCanEnter(dest_pos))
	{
		/// @ToDo: Use the actual path cost, to check whether the goody hut is really so close.
		bonus += g_theGoalDB->Get(m_goal_type)->GetInVisionRangeBonus();

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers)
		report_InVisionRange  = bonus - report_Treaspassing;
#endif //_DEBUG

		if (!Barbarians::InBarbarianPeriod()
		||  wonderutil_GetProtectFromBarbarians(g_player[agent_ptr->Get_Army()->GetOwner()]->m_builtWonders)
		){
			bonus += g_theGoalDB->Get(m_goal_type)->GetNoBarbarianBonus();
		}
		else if ((agent_ptr->Get_Squad_Class() & k_Goal_SquadClass_CanAttack_Bit) != 0x0)
		{
			bonus += g_theGoalDB->Get(m_goal_type)->GetCanAttackBonus();
		}
#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers)
		report_NoBarbsPresent = bonus - report_InVisionRange;
#endif //_DEBUG
	}

	if(!agent_ptr->Get_Army()->HasCargo())
	{
		if(!g_theWorld->IsOnSameContinent(dest_pos, curr_pos)) // Same continent problem
		{
			if((!agent_ptr->Get_Army()->GetMovementTypeAir()
			&&   g_player[m_playerId]->GetCargoCapacity() <= 0)
			|| (!g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
			&&  !g_theWorld->HasAdjacentFreeLand(dest_pos, m_playerId)
			&&   g_theWorld->GetCell(dest_pos)->GetNumUnits() > 0
			&&  !agent_ptr->Get_Army()->CanBeachAssault())
			){
				AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Army cannot beach assault\n"));
				return Goal::BAD_UTILITY;
			}
		}
	}
	else
	{
		if(!g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
		&& !g_theWorld->HasAdjacentFreeLand(dest_pos, m_playerId)
		&&  g_theWorld->GetCell(dest_pos)->GetNumUnits() > 0
		&& !agent_ptr->Get_Army()->CanSomeCargoBeachAssault())
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1, ("\t\tGoal::BAD_UTILITY: Cargo cannot beach assault\n"));
			return Goal::BAD_UTILITY;
		}
	}

	Utility match = bonus + time_term + raw_priority;

	const ArmyData* army = agent_ptr->Get_Army().GetData();

	Utility tieBreaker = 0;

	if(agent_ptr->Get_Army()->HasCargo())
	{
		sint32 cargoNum = 0;
		for(sint32 i = 0; i < army->Num(); ++i) // HasCargo, maybe has not be corrected, since we just want to make the sort order stable.
		{
			UnitDynamicArray const * cargo =
				army->Get(i).AccessData()->GetCargoList();

			if(cargo && cargo->Num() > 0)
			{
				cargoNum += cargo->Num();

				for(sint32 j = 0; j < cargo->Num(); ++j)
				{
					const UnitRecord* rec = cargo->Get(j)->GetDBRec();

					tieBreaker += static_cast<Utility>(rec->GetAttack());
					tieBreaker += static_cast<Utility>(rec->GetDefense());
					tieBreaker +=                      rec->GetZBRangeAttack();
					tieBreaker +=                      rec->GetFirepower();
					tieBreaker += static_cast<Utility>(rec->GetArmor());
					tieBreaker +=                      army->Get(i)->CalculateTotalHP();//rec->GetMaxHP();
				}
			}
		}

		// Take the number into account so that we prefer the closest one
		tieBreaker /= cargoNum;
		tieBreaker += 100; // That is still a bit hacky, it makes the AIs preffering transports, it is some sort of distance modifier
	}
	else
	{
		for(sint32 i = 0; i < army->Num(); ++i) // HasCargo, maybe has not be corrected, since we just want to make the sort order stable.
		{
			const UnitRecord* rec = army->Get(i)->GetDBRec();

			tieBreaker += static_cast<Utility>(rec->GetAttack());
			tieBreaker += static_cast<Utility>(rec->GetDefense());
			tieBreaker +=                      rec->GetZBRangeAttack();
			tieBreaker +=                      rec->GetFirepower();
			tieBreaker += static_cast<Utility>(rec->GetArmor());
			tieBreaker +=                      army->Get(i)->CalculateTotalHP();//rec->GetMaxHP();
		}

		// Take the number into account so that we prefer the closest one
		tieBreaker /= army->Num();
	}

	match += tieBreaker;

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	MapPoint target_pos = Get_Target_Pos();

	AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
	("\t\t%9x,\t %9x,\t%9x (%3d,%3d),\t%s (%3d,%3d) (%3d,%3d),\t%8d,\t%8d,\t%12f,\t%12f,\t%12d,\t%12f,\t%12f,\t%12f,\t%8d,\t%12f,\t%12f,\t%12f,\t%12d,\t%9x,\t%6d,\t%20s,\t%16s,\t%16s \n",
	this,                                          // This goal
	agent_ptr->Get_Army().m_id,                    // The army
	agent_ptr,                                     // The agent
	curr_pos.x,                                    // Agent pos.x
	curr_pos.y,                                    // Agent pos.y
	g_theGoalDB->Get(m_goal_type)->GetNameText(),  // Goal name
	target_pos.x,                                  // Target pos.x
	target_pos.y,                                  // Target pos.y
	dest_pos.x,                                    // Sub target pos.x
	dest_pos.y,                                    // Sub target pos.y
	match,                                         // Computed match value
	raw_priority,                                  // Raw match value
	cell_dist,                                     // Distance to target (Quare rooted quare distance), not identical with path distance
	eta,                                           // Rounds to target
	bonus,                                         // Total bonus
	report_wounded,                                // Whether there wounded units
	report_obsolete,                               // Whether there are obselete units
	report_Treaspassing,                           // Whether we are treaspassing
	time_term,                                     // Time needed to goal, if we would follow a bee line
	report_InVisionRange,                          // In vision range bonus
	report_NoBarbsPresent,                         // If no Barbarian are present bonus
	report_garrison_bonus,                         // If an agent is needed for garrison add a bonus so that it is also selected for garrison
	is_transporter,                                // Whether the agent is a transporter
	agent_ptr->Get_Goal(),                         // The goal to that this agent is asigned to
	tieBreaker,
	g_theUnitDB->GetNameStr(agent_ptr->Get_Army()->Get(0).GetType()),
	(g_theWorld->HasCity(curr_pos) ? g_theWorld->GetCity(curr_pos).GetName() : "field"),
	(g_theWorld->HasCity(target_pos) ? g_theWorld->GetCity(target_pos).GetName() : "field")
	));
#endif //_DEBUG

	return match;
}

Utility Goal::Get_Initial_Priority() const
{
	const StrategyRecord & strategy = Diplomat::GetDiplomat(m_playerId).GetCurrentStrategy();

	for(sint32 i = 0; i < strategy.GetNumGoalElement(); i++)
	{
		Assert(strategy.GetGoalElement(i)->GetGoalIndex() >= 0);

		if(m_goal_type == strategy.GetGoalElement(i)->GetGoalIndex())
		{
			return strategy.GetGoalElement(i)->GetPriority();
		}
	}

	return Goal::BAD_UTILITY;
}

Utility Goal::Compute_Raw_Priority()
{
	Player *    player_ptr = g_player[m_playerId];
	Assert(player_ptr);

	if (!player_ptr || IsTotallyComplete())
	{
		m_raw_priority = Goal::BAD_UTILITY;
		return m_raw_priority;
	}

	const MapPoint & target_pos = Get_Target_Pos();

	if(!player_ptr->CanUseSeaTab()
	&& (g_theWorld->IsWater(target_pos) || g_theWorld->IsShallowWater(target_pos))
	){
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1, ("\t  No sea tab\n"));
		m_raw_priority = Goal::BAD_UTILITY;
		return m_raw_priority;
	}

	const GoalRecord * goal_rec = g_theGoalDB->Get(m_goal_type);

	if
	  (
	        goal_rec->GetHasTransportersOrNoCoastalCities()
	    &&
	       (
	            !player_ptr->CanUseSeaTab()
	         ||
	            (
	                  player_ptr->HasCostalCities()
	              && !player_ptr->HasTransporters()
	            )
	       )
	  )
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1, ("\t  No sea tab\n"));
		m_raw_priority = Goal::BAD_UTILITY;
		return m_raw_priority;
	}

	const MapAnalysis & map = MapAnalysis::GetMapAnalysis();
	PLAYER_INDEX target_owner = Get_Target_Owner();

	// Allways compute a foreign center (even if the target is owned by the player
	// otherwise it compute with coords (0,0) !!
	MapPoint empire_center         = map.GetEmpireCenter(m_playerId);
	MapPoint foreign_empire_center = (target_owner > 0 && m_playerId != target_owner) ? map.GetEmpireCenter(target_owner) : map.GetNearestForeigner(m_playerId, target_pos);

	double cell_value = Get_Initial_Priority();

	if(cell_value == Goal::BAD_UTILITY)
	{
		m_raw_priority = Goal::BAD_UTILITY;
		return m_raw_priority;
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	double report_cell_initvalue         = cell_value;
	double report_cell_lastvalue         = cell_value;
	double report_cell_threat            = 0.0;
	double report_cell_EnemyValue        = 0.0;
	double report_cell_AlliedValue       = 0.0;
	double report_cell_HomeDistance      = 0.0;
	double report_cell_EnemyDistance     = 0.0;
	double report_cell_Settle            = 0.0;
	double report_cell_Chokepoint        = 0.0;
	double report_cell_MaxPower          = 0.0;
	double report_cell_Unexplored        = 0.0;
	double report_cell_NotVisible        = 0.0;
	double report_cell_NoOwnerTerritory  = 0.0;
	double report_cell_InHomeTerritory   = 0.0;
	double report_cell_InEnemyTerritory  = 0.0;
	double report_cell_SlaveryProtection = 0.0;
	double report_cell_SmallCitySize     = 0.0;
	double report_cell_CityConnected     = 0.0;
	double report_cell_SmallEmpireBonus  = 0.0;
	double report_cell_WeakestEnemyBonus = 0.0;
#endif //_DEBUG

	double maxThreat = static_cast<double>(map.GetMaxThreat(m_playerId));
	if ( maxThreat > 0.0 )
	{
		cell_value +=
		( ( static_cast<double>(map.GetThreat(m_playerId, target_pos)) /
		     maxThreat) *
		  g_theGoalDB->Get(m_goal_type)->GetThreatBonus() );

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_threat    = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG
	}

	double maxEnemyValue = static_cast<double>(map.GetMaxEnemyCityValue(m_playerId));
	if(maxEnemyValue > 0.0)
	{
		cell_value +=
		( ( static_cast<double>(map.GetEnemyCityValue( m_playerId, target_pos)) /
		    maxEnemyValue ) *
		  goal_rec->GetEnemyValueBonus() );

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_EnemyValue = cell_value - report_cell_lastvalue;
		report_cell_lastvalue  = cell_value;
#endif //_DEBUG
	}

	double maxAlliedValue = static_cast<double>(map.GetMaxAlliedCityValue(m_playerId));
	if ( maxAlliedValue > 0)
	{
		cell_value +=
		( ( static_cast<double>(map.GetAlliedCityValue(m_playerId, target_pos)) /
		    maxAlliedValue ) *
		  goal_rec->GetAlliedValueBonus() );

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_AlliedValue = cell_value - report_cell_lastvalue;
		report_cell_lastvalue   = cell_value;
#endif //_DEBUG
	}

	double maxPower = static_cast<double>(map.GetMaxPower(m_playerId)); // Get Max Power of all Allies
	if (maxPower > 0)
	{
		cell_value +=
		( ( static_cast<double>(map.GetPower( m_playerId, target_pos)) /  // Allies' Power at target_pos
		    maxPower ) *
		goal_rec->GetPowerBonus() );

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
		report_cell_MaxPower  = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG
	}

#if 0
	if(!goal_rec->GetIsGlobalGoal())
	{
		cell_value += sqrt(static_cast<double>
		(MapPoint::GetSquaredDistance(target_pos, empire_center))) * goal_rec->GetDistanceToHomeBonus();
	}
	else
	{
		if(g_theWorld->IsOnSameContinent(target_pos, empire_center))
		{
			sint32 distance;
			float costs = 0.0f;
			g_city_astar.IsLandConnected(m_playerId, target_pos, empire_center, costs, distance);

			cell_value += static_cast<double>(distance * goal_rec->GetDistanceToHomeBonus());
		}
		else
		{
			sint32 distance;
			float costs = 0.0f;
			g_city_astar.FindCityDist(m_playerId, target_pos, empire_center, costs, distance);

			cell_value += static_cast<double>(distance * goal_rec->GetDistanceToHomeBonus());
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_HomeDistance = cell_value - report_cell_lastvalue;
	report_cell_lastvalue    = cell_value;
#endif //_DEBUG

	if (foreign_empire_center.x != 0 && foreign_empire_center.y != 0)//Dangerious if the empire center has coords (0,0)
	{
		if(!goal_rec->GetIsGlobalGoal())
		{
			cell_value += sqrt(static_cast<double>
			(MapPoint::GetSquaredDistance(target_pos, foreign_empire_center))) * goal_rec->GetDistanceToEnemyBonus();
		}
		else
		{
			if(g_theWorld->IsOnSameContinent(target_pos, foreign_empire_center))
			{
				sint32 distance;
				float costs = 0.0f;
				g_city_astar.IsLandConnected(m_playerId, target_pos, foreign_empire_center, costs, distance);

				cell_value += static_cast<double>(distance * goal_rec->GetDistanceToEnemyBonus());
			}
			else
			{
				sint32 distance;
				float costs = 0.0f;
				g_city_astar.FindCityDist(m_playerId, target_pos, foreign_empire_center, costs, distance);

				cell_value += static_cast<double>(distance * goal_rec->GetDistanceToEnemyBonus());
			}
		}
	}

#else
	cell_value += sqrt(static_cast<double>
	(MapPoint::GetSquaredDistance(target_pos, empire_center))) * goal_rec->GetDistanceToHomeBonus();

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_HomeDistance = cell_value - report_cell_lastvalue;
	report_cell_lastvalue    = cell_value;
#endif //_DEBUG

//	if (foreign_empire_center.IsValid()) // Replace when default values of MapPoint are invalid
	if (foreign_empire_center.x != 0 && foreign_empire_center.y != 0)//Dangerious if the empire center has coords (0,0)
	{
		cell_value += sqrt(static_cast<double>
		(MapPoint::GetSquaredDistance(target_pos, foreign_empire_center))) * goal_rec->GetDistanceToEnemyBonus();
	}
#endif

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_EnemyDistance = cell_value - report_cell_lastvalue;
	report_cell_lastvalue     = cell_value;
#endif //_DEBUG

	if (g_theWorld->GetCell( target_pos )->GetIsChokePoint())
	{
		cell_value += goal_rec->GetChokePointBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_Chokepoint = cell_value - report_cell_lastvalue;
	report_cell_lastvalue  = cell_value;
#endif //_DEBUG

	if(!player_ptr->IsExplored(target_pos))
	{
		if
		  (
		       g_theWorld->IsCity(target_pos)
		    && g_theWorld->GetCell(target_pos)->m_cityHasVisibleTileImprovement
		  )
		{
			// Nothing to add
		}
		else
		{
			cell_value += goal_rec->GetUnexploredBonus();
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_Unexplored = cell_value - report_cell_lastvalue;
	report_cell_lastvalue  = cell_value;
#endif //_DEBUG

	if(!player_ptr->IsVisible(target_pos))
	{
		cell_value += goal_rec->GetNotVisibleBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_NotVisible = cell_value - report_cell_lastvalue;
	report_cell_lastvalue  = cell_value;
#endif //_DEBUG

	PLAYER_INDEX territoryOwner = g_theWorld->GetCell( target_pos )->GetOwner();
	if(m_playerId == territoryOwner)
	{
		cell_value += goal_rec->GetInHomeTerritoryBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_InHomeTerritory = cell_value - report_cell_lastvalue;
	report_cell_lastvalue       = cell_value;
#endif //_DEBUG

	if(m_playerId != territoryOwner && territoryOwner >= 0) // 0: Barbarian player
	{
		cell_value += goal_rec->GetInEnemyTerritoryBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_InEnemyTerritory = cell_value - report_cell_lastvalue;
	report_cell_lastvalue        = cell_value;
#endif //_DEBUG

	if(territoryOwner == PLAYER_UNASSIGNED)
	{
		cell_value += goal_rec->GetNoOwnerTerritoryBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	report_cell_NoOwnerTerritory = cell_value - report_cell_lastvalue;
	report_cell_lastvalue        = cell_value;
#endif //_DEBUG

	if ( goal_rec->GetTargetTypeSettleLand() ||
		 goal_rec->GetTargetTypeSettleSea() )
	{
		cell_value += SettleMap::s_settleMap.GetValue(target_pos);

#if defined(_DEBUG) || defined(USE_LOGGING)
		report_cell_Settle    = cell_value - report_cell_lastvalue;
		report_cell_lastvalue = cell_value;
#endif //_DEBUG

	}

	if(m_target_city.IsValid())
	{
		cell_value += goal_rec->GetSlaveryProtectionBonus() * (1.0 - m_target_city.IsProtectedFromSlavery());

#if defined(_DEBUG) || defined(USE_LOGGING)
		report_cell_SlaveryProtection    = cell_value - report_cell_lastvalue;
		report_cell_lastvalue            = cell_value;
#endif //_DEBUG

		sint32 pop;
		m_target_city->GetPop(pop);
		cell_value += (1.0 - (static_cast<double>(pop) / static_cast<double>(m_target_city.CD()->GetOverallMaxPop()))) * static_cast<double>(goal_rec->GetSmallCitySizeBonus());

#if defined(_DEBUG) || defined(USE_LOGGING)
		report_cell_SmallCitySize = cell_value - report_cell_lastvalue;
		report_cell_lastvalue     = cell_value;
#endif //_DEBUG
	}

	const sint32 doubleDistanceFactor = 4;
	sint32 distance = std::numeric_limits<sint32>::max();
	bool isLandConnected = goal_rec->HasConnectionBoni() && player_ptr->IsConnected(target_pos, doubleDistanceFactor * g_theConstDB->Get(0)->GetBorderSquaredRadius(), distance);
	bool isConnected     = goal_rec->HasConnectionBoni() && player_ptr->IsConnected(target_pos, doubleDistanceFactor * g_theConstDB->Get(0)->GetBorderSquaredRadius(), distance, false);

	const GoalRecord::ConnectionBoni* cbRec = goal_rec->GetConnectionBoniPtr();

	// A little ugly but this way I don't have to mess with the debug reports
	if(cbRec != NULL)
	{
		if(isLandConnected)
		{
			double value  =      -static_cast<double>(cbRec->GetLandToCloseCityConnectionBonus() * distance);
			       value /= sqrt( static_cast<double>(doubleDistanceFactor * doubleDistanceFactor * g_theConstDB->Get(0)->GetBorderSquaredRadius())); // Maybe this factor is not the possible maximum
			       value += static_cast<double>(cbRec->GetLandToCloseCityConnectionBonus());

			if(value > 0)
			{
				cell_value += value;
			}
		}

		if(isConnected)
		{
			double value  =      -static_cast<double>(cbRec->GetCloseCityConnectionBonus() * distance);
			       value /= sqrt( static_cast<double>(doubleDistanceFactor * doubleDistanceFactor * g_theConstDB->Get(0)->GetBorderSquaredRadius())); // Maybe this factor is not the possible maximum
			       value += static_cast<double>(cbRec->GetCloseCityConnectionBonus());

			if(value > 0)
			{
				cell_value += value;
			}
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	report_cell_CityConnected = cell_value - report_cell_lastvalue;
	report_cell_lastvalue     = cell_value;
#endif //_DEBUG

	if
	  (
	       cbRec != NULL
	    && cbRec->GetSmallTargetEmpireBonus() != 0
	    && target_owner != m_playerId
	    && target_owner > -1
	    && g_player[target_owner]->GetNumCities() < cbRec->GetSmallTargetEmpireSize()
	  )
	{
		cell_value += cbRec->GetSmallTargetEmpireBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	report_cell_SmallEmpireBonus = cell_value - report_cell_lastvalue;
	report_cell_lastvalue        = cell_value;
#endif //_DEBUG

	if
	  (
	       target_owner != m_playerId
	    && cbRec
	    && cbRec->GetWeakestEnemyBonus() != 0
	    && target_owner > -1
	    && (
	            Diplomat::GetDiplomat(m_playerId).IsBestHotwarEnemy(target_owner)
	         || target_owner == 0
	       )
	  )
	{
		cell_value += cbRec->GetWeakestEnemyBonus();
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	report_cell_WeakestEnemyBonus = cell_value - report_cell_lastvalue;
	report_cell_lastvalue         = cell_value;
#endif //_DEBUG

	sint32 threaten_bonus = GetThreatenBonus();

	m_raw_priority = (Utility) cell_value + threaten_bonus;

	Assert(m_raw_priority <  Goal::MAX_UTILITY);
	Assert(m_raw_priority >= Goal::BAD_UTILITY);
	if (m_raw_priority > Goal::MAX_UTILITY)
		m_raw_priority = Goal::MAX_UTILITY-1;
	else if (m_raw_priority < Goal::BAD_UTILITY)
		m_raw_priority = Goal::BAD_UTILITY;

#if defined(_DEBUG) || defined(USE_LOGGING) // Add a debug report of goal computing (raw priority and all modifiers) - Calvitix
	if(CtpAiDebug::DebugLogCheck(this->Get_Player_Index(), this->Get_Goal_Type(), -1))
	{
		char buff[1024];
		sprintf(buff, "\t %9" PRIXPTR ",\t%s,\t%i, \t\trc(%3d,%3d),\t%14f,\t%14f,\t%14f,\t%14f,\t%14f,\t%14f,\t%14f, rc(%3d,%3d),\t%8f, rc(%3d,%3d), \t%8f,\t%8f,\t%8f,\t%8f,\t\t%8f,\t\t%8f,\t\t%8f,",
		    (uintptr_t)this,
		        goal_rec->GetNameText(),
		        m_raw_priority,
		        target_pos.x,
		        target_pos.y,
		        report_cell_initvalue,
		        report_cell_lastvalue,
		        report_cell_threat,
		        report_cell_EnemyValue,
		        report_cell_AlliedValue,
		        report_cell_MaxPower,
		        report_cell_HomeDistance,
		        empire_center.x,
		        empire_center.y,
		        report_cell_EnemyDistance,
		        foreign_empire_center.x,
		        foreign_empire_center.y,
		        report_cell_Settle,
		        report_cell_Chokepoint,
		        report_cell_Unexplored,
		        report_cell_NotVisible,
		        report_cell_InHomeTerritory,
		        report_cell_InEnemyTerritory,
		        report_cell_NoOwnerTerritory
		       );

		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1, ("%s\t\t%8f,\t\t\t%8f,\t%i,\t\t\t%8f,\t%8f,\t%8f,\t%s\n",
		                                 buff,
		                                 report_cell_SlaveryProtection,
		                                 report_cell_SmallCitySize,
		                                 threaten_bonus,
		                                 report_cell_CityConnected,
		                                 report_cell_SmallEmpireBonus,
		                                 report_cell_WeakestEnemyBonus,
		                                 (g_theWorld->HasCity(target_pos) ? g_theWorld->GetCity(target_pos).GetName() : "field")));
	}
	// For some reason the following does not work in VC6:
/*	AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1,
	("\t %9x,\t%s,\t%i, \t\trc(%3d,%3d),\t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%8f, rc(%3d,%3d),\t%8f, rc(%3d,%3d), \t%8f,\t%8f,\t%8f,\t%8f,\t%8f,\t%s \n",
	this,
	goal_rec->GetNameText(),
	m_raw_priority,
	target_pos.x,
	target_pos.y,
	report_cell_initvalue,
	report_cell_lastvalue,
	report_cell_threat,
	report_cell_EnemyValue,
	report_cell_AlliedValue,
	report_cell_MaxPower,
	report_cell_HomeDistance,
	empire_center.x,
	empire_center.y,
	report_cell_EnemyDistance,
	foreign_empire_center.x,
	foreign_empire_center.y,
	report_cell_Settle,
	report_cell_Chokepoint,
	report_cell_Unexplored,
	report_cell_NotVisible,
	threaten_bonus,
	(g_theWorld->HasCity(target_pos) ? g_theWorld->GetCity(target_pos).GetName() : "field")
	));*/
#endif //_DEBUG

	return m_raw_priority;
}

GOAL_RESULT Goal::Execute_Task()
{
	if(IsTotallyComplete())
		return GOAL_COMPLETE;

	if(!Can_Be_Executed())
		return GOAL_ALREADY_MOVED;

	Assert(m_agents.begin() != m_agents.end());

	const GoalRecord * goal_record  = g_theGoalDB->Get(m_goal_type);

	Set_Sub_Task(SUB_TASK_GOAL);

	bool hastogowithoutgrouping = false;

	Agent_List tmpTransporters;
	for
	(
	    Agent_List::reverse_iterator agent_iter  = m_agents.rbegin();
	                                 agent_iter != m_agents.rend();
	                               ++agent_iter
	)
	{
		Agent_ptr    agent_ptr       = (Agent_ptr) *agent_iter;

		sint32 transporters = 0;
		sint32 max = 0;
		sint32 empty = 0;
		if(agent_ptr->Get_Army()->GetCargo(transporters, max, empty) && empty > 0)
		{
			m_agents.remove(agent_ptr);
			tmpTransporters.push_back(agent_ptr);
		}
	}

	m_agents.splice(m_agents.end(), tmpTransporters);

	if(goal_record->GetNeverSatisfied())
	{
		for
		(
		    Agent_List::iterator agent_iter  = m_agents.begin();
		                         agent_iter != m_agents.end();
		                       ++agent_iter
		)
		{
			Agent_ptr    agent_ptr       = (Agent_ptr) *agent_iter;

			// Add this condition to avoid that a 12 units army with SEIGE goal retreat
			// at 1 tile near the city, because it has left 1 unit and has to group with
			// another one. I Think it is better to go on an seige the city (if there is
			// more than 2/3 left, if more than 8 units).
			/// @ToDo: Reconsider NeverSatisfied
			MapPoint     target__pos     = Get_Target_Pos(agent_ptr->Get_Army());
			MapPoint     current_pos     = agent_ptr->Get_Pos();

			if
			  (
			      MapPoint::GetSquaredDistance(target__pos, current_pos) <= 1
			   && (
			           agent_ptr->Get_Army()->Num() > (2*k_MAX_ARMY_SIZE/3)
			        || agent_ptr->Get_Army()->Num() > m_current_attacking_strength.Get_Agent_Count()
			      )
			  )
			{
				hastogowithoutgrouping = true;
				break;
			}
		}
	}

	if(Is_Satisfied() || Is_Execute_Incrementally() || hastogowithoutgrouping)
	{
		if(goal_record->GetRallyFirst())
		{
			if(Goal_Too_Expensive())
				return GOAL_FAILED_TOO_EXPENSIVE;

			if(!RallyComplete())
			{
				Set_Sub_Task(SUB_TASK_RALLY);

				if(Ok_To_Rally())
				{
					if(!RallyTroops())
					{
						return GOAL_FAILED_RALLY;
					}
					else
					{
						// If hastogowithoutgrouping is true, execute the goal
						// even if the rally is not complete
						if(!hastogowithoutgrouping)
						{
							return GOAL_IN_PROGRESS;
						}
						else
						{
							Set_Sub_Task(SUB_TASK_GOAL);
						}
					}
				}
			}
			else
			{
				Set_Sub_Task(SUB_TASK_GOAL);
			}
		}

		// Added an Ungroup method (sometimes, for example to explore,
		// it is more interessant to have a lot of small units rather
		// than a huge army
		else if(goal_record->GetUnGroupFirst())
		{
			Set_Sub_Task(SUB_TASK_UNGROUP);
			if (!UnGroupTroops())
			{
				return GOAL_FAILED_UNGROUP;
			}

			if (!UnGroupComplete())
			{
				return GOAL_IN_PROGRESS;
			}
			else
			{
				Set_Sub_Task(SUB_TASK_GOAL);
			}
		}
/*		else
		{
			Set_Sub_Task(SUB_TASK_GOAL);
		}*/

		for
		(
		    Agent_List::iterator agent_iter  = m_agents.begin();
		                         agent_iter != m_agents.end();
		                       ++agent_iter
		)
		{
			Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;

			if(agent_ptr->Get_Is_Dead())
				continue;

			if(!agent_ptr->Get_Can_Be_Executed())
				continue;

			MapPoint pos = Get_Target_Pos(agent_ptr->Get_Army());

			if(!GotoGoalTaskSolution(agent_ptr, pos))
			{
				if(Needs_Transporter() && Get_Transporters_Num() < 1)
				{
					Set_Needs_Transporter(agent_ptr);
					return GOAL_NEEDS_TRANSPORT;
				}
				else
				{
					return GOAL_FAILED_NEEDS_TRANSPORT;
				}
			}
		}
	}

	return GOAL_IN_PROGRESS; // Maybe this is good as it is
}

bool Goal::IsTotallyComplete() const
{
	return IsComplete() || IsCurrentlyUnavailable();
}

bool Goal::IsComplete() const
{
	return IsInvalid() || IsTargetImmune() || IsInvalidByDiplomacy();
}

bool Goal::IsInvalidByDiplomacy() const
{
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);
	PLAYER_INDEX target_owner = Get_Target_Owner();
	MapPoint target_pos = Get_Target_Pos();

	Player *player_ptr = g_player[m_playerId];
	Assert(player_ptr != NULL);

	if(m_playerId != 0 && target_owner > 0 && target_owner != m_playerId)
	{
		bool regard_checked = false;
		bool diplomacy_match = true;

		if(player_ptr->HasContactWith(target_owner))
		{
			if(!goal_record->GetTargetOwnerHotEnemy()
			&&  player_ptr->HasWarWith(target_owner))
			{
				return true;
			}

			bool iscivilian = false;
			if(
			       m_target_army.m_id != 0
			   &&
			    (
			         goal_record->GetTargetTypeAttackUnit()
			      || goal_record->GetTargetTypeSpecialUnit()
			    )
			  )
			{
				iscivilian = m_target_army->IsCivilian();
			}
			
			/* disabling, see https://github.com/civctp2/civctp2/pull/217#issuecomment-565860162
			if(iscivilian &&
			   !goal_record->GetExecute()->GetUnitPretest_CanEnslaveSettler() &&
			   goal_record->GetTargetOwnerHotEnemy() &&
			   (diplomat.GetPersonality()->GetAlignmentGood() ||
				diplomat.GetPersonality()->GetAlignmentNeutral()))
				return true;
			*/

			if(goal_record->GetTargetOwnerNeutral())
			{
				diplomacy_match =
					diplomat.TestEffectiveRegard(target_owner, NEUTRAL_REGARD);

				regard_checked = true;
			}

			if((!regard_checked || !diplomacy_match) &&
			   goal_record->GetTargetOwnerColdEnemy())
			{
				diplomacy_match =
					diplomat.TestEffectiveRegard(target_owner, COLDWAR_REGARD);

				regard_checked = true;
			}

			if((!regard_checked || !diplomacy_match) &&
			   goal_record->GetTargetOwnerHotEnemy())
			{
				diplomacy_match =
					diplomat.TestEffectiveRegard(target_owner, HOTWAR_REGARD);

				regard_checked = true;
			}

			if((!regard_checked || !diplomacy_match) &&
			   goal_record->GetTargetOwnerAlly())
			{
				diplomacy_match =
					diplomat.TestEffectiveRegard(target_owner, ALLIED_REGARD);

				regard_checked = true;
			}
			// If the goal is not executed by stealth units, forbid to
			// execute it if there is no incursion permission
			// (depending on alignement) - Calvitix
			if((!diplomat.IncursionPermission(target_owner) &&
				(diplomat.GetPersonality()->GetAlignmentGood() ||
				 diplomat.GetPersonality()->GetAlignmentNeutral()))
			   && !goal_record->GetSquadClassStealth())
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
					("\t Player %d GOAL %x (%s) (%3d,%3d): Diplomacy match failed : No permission to enter territory of player %d\n", m_playerId, this, g_theGoalDB->Get(m_goal_type)->GetNameText(), target_pos.x, target_pos.y, target_owner));
				return true;
			}

			if(!diplomacy_match)
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
					("\t Player %d GOAL %x (%s) (%3d,%3d): Diplomacy match failed with player %d.\n", m_playerId, this, g_theGoalDB->Get(m_goal_type)->GetNameText(), target_pos.x, target_pos.y, target_owner));

				return true;
			}
		}
		else if(!goal_record->GetTargetOwnerNoContact())
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
				("\t Player %d GOAL %x (%s) (%3d,%3d): Target owner %d not contacted.\n", m_playerId, this, g_theGoalDB->Get(m_goal_type)->GetNameText(), target_pos.x, target_pos.y, target_owner));

			return true;
		}
	}

	return false;
}

bool Goal::IsTargetImmune() const
{
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	const OrderRecord * order_record = goal_record->GetExecute();
	PLAYER_INDEX target_owner = Get_Target_Owner();
	MapPoint target_pos = Get_Target_Pos();

	// Don't attack as Barbarian a target that is protected by the Great Wall
	if
	  (
	       m_playerId == PLAYER_INDEX_VANDALS
	    && wonderutil_GetProtectFromBarbarians(g_player[target_owner]->m_builtWonders)
	  )
	{
		return true;
	}

	if(m_playerId > 0 && target_owner > 0 && goal_record->HasTargetProtectionWonder())
	{
		const WonderRecord *wonder_rec = goal_record->GetTargetProtectionWonderPtr();

		if(AgreementMatrix::s_agreements.TurnsAtWar(m_playerId, target_owner) < 0
		&& g_player[target_owner]->HasWonder(wonder_rec->GetIndex()))
			return true;
	}

	if(
	      m_target_army.m_id != 0
	   &&
	   (
	       goal_record->GetTargetTypeAttackUnit()
	    || goal_record->GetTargetTypeSpecialUnit()
	   )
	  )
	{
		if(g_theWorld->GetCity(m_target_army->RetPos()).m_id != 0)
			return true;

		bool isspecial;
		bool isstealth;
		sint32 maxattack;
		sint32 maxdefense;
		bool cancapture;
		bool haszoc;
		bool canbombard;

		m_target_army->CharacterizeArmy
		                               (
		                                isspecial,
		                                isstealth,
		                                maxattack,
		                                maxdefense,
		                                cancapture,
		                                haszoc,
		                                canbombard
		                               );

		if(isspecial && !m_target_army->IsVisible(m_playerId))
			return true;

		if(!goal_record->GetTargetTypeAttackUnit()
		&&  maxattack > 0
		){
			return true;
		}
	}

	if(!MapAnalysis::GetMapAnalysis().PlayerCanEnter(m_playerId, target_pos))
		return true;

	// Will be explored once there
/*	if(goal_record->GetTargetTypeUnexplored())
	{
		Unit city = g_theWorld->GetCity(target_pos);
		CellUnitList army;
		g_theWorld->GetArmy(target_pos, army);
		if(army.Num() > 0 || city.m_id != 0x0)
			return true;
	}*/

	switch(order_record->GetTargetPretest())
	{
		case k_Order_TargetPretest_EnemyArmy_Bit:
		case k_Order_TargetPretest_EnemySpecialUnit_Bit:
		case k_Order_TargetPretest_EnemySettler_Bit:
		case k_Order_TargetPretest_EnemyTradeUnit_Bit:
		case k_Order_TargetPretest_AttackPosition_Bit:
			if((m_target_army.m_id != 0) && (m_target_army.GetOwner() == m_playerId))
			{
				Assert(false);
				return true;
			}
			break;
		case k_Order_TargetPretest_TradeRoute_Bit:

			Assert(m_target_city.m_id != 0);
			if((m_target_city.m_id == 0) ||
				(m_target_city.GetCityData()->GetTradeSourceList() == NULL) ||
			   (m_target_city.GetCityData()->GetTradeSourceList()->Num() <= 0))
				return true;
			break;
		case k_Order_TargetPretest_TerrainImprovement_Bit:

			if(goal_record->GetTargetTypeImprovement())
			{
				Assert(m_target_city.m_id != 0);
				if(m_target_city.m_id == 0 ||
				   !m_target_city->GetCityData()->WasTerrainImprovementBuilt())
					return true;
			}
			else
			{
				if(g_theWorld->GetCell(target_pos)->GetNumDBImprovements() <= 0)
					return true;
			}
			break;
	}

	if(    !order_record->GetTargetPretestTerrainImprovement()
	    && !order_record->GetTargetPretestTradeRoute()
	  )
	{
		if(!ArmyData::TargetValidForOrder(order_record, target_pos))
			return true;
	}

	// Just exchanges the player, at that size you should conquer
	// @ToDo adapt for players that have the give city wonder and are not at the city limit
	// @ToDo adapt if no new civ is created but Barbarians.
	if(order_record->GetUnitPretest_CanInciteRevolution())
	{
		if(g_player[target_owner]->GetNumCities() == 1)
			return true;
	}

	if(order_record->GetUnitPretest_CanPlantNuke())
	{
		if(!g_player[m_playerId]->CanUseNukes())
			return true;
	}

	if(order_record->GetUnitPretest_CanReformCity())
	{
		if(!m_target_city.GetCityData()->IsConverted())
			return true;
	}

	if(order_record->GetUnitPretest_CanSueFranchise())
	{
		if(!m_target_city.GetCityData()->IsFranchised())
			return true;
	}

	if( order_record->GetUnitPretest_CanSue()
	    &&  m_target_army.m_id != 0
	    && !m_target_army->CanBeSued()
	  )
	{
	    return true;
	}

	if(order_record->GetUnitPretest_EstablishEmbassy())
	{
		if(g_player[m_playerId]->HasEmbassyWith(m_target_city->GetOwner()))
			return true;
	}

	if(order_record->GetUnitPretest_CanCreateFranchise())
	{
		if(m_target_city.GetCityData()->GetFranchiseOwner() == m_playerId)
			return true;
	}

	if(order_record->GetUnitPretest_CanBioTerror())
	{
		if(m_target_city.GetCityData()->IsBioInfected())
			return true;
	}

	if(order_record->GetUnitPretest_CanNanoInfect())
	{
		if(m_target_city.GetCityData()->IsNanoInfected())
			return true;
	}

	if(order_record->GetUnitPretest_CanConvertCity())
	{
		if(m_target_city.GetCityData()->GetConvertedTo() == m_playerId)
			return true;
	}

	if(order_record->GetUnitPretest_CanInterceptTrade())
	{
		Assert(m_target_city.m_id != 0);
		if((m_target_city.m_id == 0) ||
		  ((m_target_city.GetCityData()->GetTradeSourceList()) &&
		   (m_target_city.GetCityData()->GetTradeSourceList()->Num() <= 0)))
			return true;
	}

	// Try to steal technology only if the other civ has more advances than player
	// Otherwise, spy can do anything else
	if(order_record->GetUnitPretest_CanStealTechnology())
	{
		sint32 num = 0;
		delete[] g_player[m_playerId]->m_advances->CanAskFor(g_player[target_owner]->m_advances, num);

		if(num <= 0)
			return true;
	}

	if(!wonderutil_GetProhibitSlavers(g_theWonderTracker->GetBuiltWonders()))
	{
		// Abolitionist has to go to cities with slaves
		if(    order_record->GetUnitPretest_CanInciteUprising()
		    || order_record->GetUnitPretest_CanUndergroundRailway()
		  )
		{
			if(m_target_city.GetCityData()->SlaveCount() == 0)
				return true;
		}

		if(order_record->GetUnitPretest_CanSlaveRaid())
		{
			sint32 popCount;
			m_target_city.GetCityData()->GetPop(popCount);

			// Slavers must go to cities with population to enslave, give an extra point so that the city isn't killed on conquest
			if(popCount <= 2)
				return true;
		}

		if(      order_record->GetUnitPretest_CanEnslaveSettler()
		     &&  m_target_army.m_id != 0
		     && !m_target_army->CanBeEnslaved()
		   )
		{
			return true;
		}
	}
	else
	{
		if(order_record->GetUnitPretest_CanInciteUprising()
		|| order_record->GetUnitPretest_CanUndergroundRailway()
		|| order_record->GetUnitPretest_CanSlaveRaid()
		|| order_record->GetUnitPretest_CanEnslaveSettler()
		  )
		{
			return true;
		}
	}

	return false;
}

bool Goal::IsCurrentlyUnavailable() const
{
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);
	Diplomat & diplomat = Diplomat::GetDiplomat(m_playerId);
	PLAYER_INDEX target_owner = Get_Target_Owner();
	MapPoint target_pos = Get_Target_Pos();

	const OrderRecord * order_record = g_theGoalDB->Get(m_goal_type)->GetExecute();

	if(g_player[m_playerId]->GetGold() < order_record->GetGold()) // Keep
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, 0,
		    ("GOAL %x (%s): Not enough gold to perform goal.\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText()));

		return true;
	}

	// Questionable whether it should be here, looks like we have also only one launch target, shouldn't there be more?
	if(order_record->GetUnitPretest_CanNukeCity())
	{
		if(diplomat.GetNuclearLaunchTarget() == target_owner)
			return true;
	}

	if(goal_record->GetAvoidWatchfulCity())
	{
		Assert(m_target_city.m_id != 0);
		if(m_target_city->IsWatchful())
			return true;
	}

	if(goal_record->GetTargetTypeChokePoint())
	{
		CellUnitList army;

		g_theWorld->GetArmy(target_pos, army);
		if(g_theWorld->GetCell(target_pos)->GetNumUnits() > 0
		&& army.GetOwner() != m_playerId)
			return false;

		if(Is_Satisfied() && ArmiesAtGoal())
			return true;
	}

	if(order_record->GetUnitPretest_CanInciteRevolution())
	{
		sint32 cost;
		if(ArmyData::GetInciteRevolutionCost(target_pos, cost) &&
			(cost > g_player[m_playerId]->GetGold()))
			return true;
	}

	if(order_record->GetUnitPretest_NoFuelThenCrash())
	{
		if(g_theUnitPool->IsValid(m_target_city)
		&& target_owner != m_playerId
		  )
		{
			return true;
		}
		else if(g_theArmyPool->IsValid(m_target_army)
		     && target_owner != m_playerId
		       )
		{
			return true;
		}
		else if(!g_theWorld->IsAirfield(target_pos)) // Changes if units are there
		{
			return true;
		}
	}

	return false;
}

bool Goal::IsInvalid() const
{
	if(m_playerId < 0)
	{
		Assert(false);
		return true;
	}

	bool isArmy = m_target_army.m_id != 0;

	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);

	if(   isArmy
	   &&
	      (
	          goal_record->GetTargetTypeAttackUnit()
	       || goal_record->GetTargetTypeSpecialUnit()
	      )
	  )
	{
		if(!g_theArmyPool->IsValid(m_target_army))
			return true;
		else if(m_target_army->Num() <= 0)
			return true;
		else
			return false;
	}

	if(goal_record->GetTargetTypeCity()
	|| goal_record->GetTargetTypeTradeRoute()
	|| goal_record->GetTargetTypeImprovement()
	){
		if(!g_theUnitPool->IsValid(m_target_city))
			return true;

		CityData *city = m_target_city->GetCityData();
		if(city == NULL)
		{
			Assert(false);
			return true;
		}

		if(goal_record->GetTargetOwnerSelf())
		{
			if(city->GetOwner() != m_playerId)
				return true;
		}
		else
		{
			if(city->GetOwner() == m_playerId)
				return true;
		}
	}

	if(goal_record->GetTargetTypeEndgame())
	{
		return(!terrainutil_HasEndgame( Get_Target_Pos() ));
	}

	if (goal_record->GetTargetTypeUnexplored() )
		return(g_player[m_playerId]->IsExplored(Get_Target_Pos()));

	if(goal_record->GetTargetTypeSettleLand()
	|| goal_record->GetTargetTypeSettleSea()
	){
		if(!SettleMap::s_settleMap.CanSettlePos(Get_Target_Pos()))
			return true;

		if(g_theWorld->HasCity(Get_Target_Pos()))
			return true;
	}

	if(goal_record->GetTargetTypeGoodyHut())
		return(g_theWorld->GetGoodyHut(Get_Target_Pos()) == NULL);

	// Check whether the target can refuel the given army
	if(goal_record->GetTargetTypePetrolStation())
	{
		if(g_theUnitPool->IsValid(m_target_city))
		{
			CityData *city = m_target_city->GetCityData();
			if(city == NULL){
				Assert(0);
				return true;
			}

			if(goal_record->GetTargetOwnerSelf()
			&& city->GetOwner() != m_playerId)
				return true;
			else if(!goal_record->GetTargetOwnerSelf()
				 &&  city->GetOwner() == m_playerId)
				return true;
		}
//		else if(g_theArmyPool->IsValid(m_target_army)){ // Aircraft carriers are missing
//			return Cannot carry aircrafts;
//		}
		return !g_theWorld->IsAirfield(Get_Target_Pos());
	}

	return false;
}

bool Goal::IsTimeToRemove() const
{
	if ( IsInvalid() || IsComplete() ||
		 (g_theGoalDB->Get(m_goal_type)->GetRemoveWhenComplete() &&
		  IsTotallyComplete() ) )
		 return true;
	return false;
}

bool Goal::Pretest_Bid(const Agent_ptr agent_ptr, const MapPoint & target_pos) const
{
	const Army & army = agent_ptr->Get_Army();

	if (army->GetMinFuel() != 0x7fffffff)
	{
		sint32 num_tiles_to_half;
		sint32 num_tiles_to_empty;
		army->CalcRemainingFuel(num_tiles_to_half, num_tiles_to_empty);

		num_tiles_to_empty = static_cast<sint32>(num_tiles_to_empty / k_MOVE_AIR_COST);
		num_tiles_to_half = static_cast<sint32>(num_tiles_to_half / k_MOVE_AIR_COST);

		sint32 distance_to_refuel;
		sint32 distance_to_target;
		MapPoint refuel_pos(-1, -1);
		CtpAi::GetNearestRefuel(army, target_pos, refuel_pos, distance_to_refuel);

		distance_to_target =
		    static_cast<sint32>(sqrt(static_cast<double>
		    (MapPoint::GetSquaredDistance(army->RetPos(), target_pos))
		    ));

		if (num_tiles_to_empty < distance_to_target + distance_to_refuel)
			return false;
	}

	const GoalRecord *goal_rec = g_theGoalDB->Get(m_goal_type);

	if (goal_rec->GetSquadClassCanBombard())
	{
		if(g_theWorld->GetCell(target_pos)->IsAnyUnitInCell())
		{
			CellUnitList* defenders = g_theWorld->GetCell(target_pos)->UnitArmy();
			if(!army->CanBombardTargetType(*defenders))
				return false;
		}
		else
		{
			return false;
		}
	}

	const OrderRecord *order_rec = goal_rec->GetExecute();

	sint32 transports;
	sint32 max_cargo_slots;
	sint32 empty_cargo_slots;

	army->GetCargo(transports, max_cargo_slots, empty_cargo_slots);

	if (transports > 0 )
	{
		if ((empty_cargo_slots < max_cargo_slots) || (army->HasCargo()))
		{
			ORDER_TEST cargo_test = army->CargoTestOrderHere(order_rec, target_pos);
			if ( cargo_test == ORDER_TEST_ILLEGAL ||
				 cargo_test == ORDER_TEST_INVALID_TARGET )
				return false;
			else
				return true;
		}
		else if (Needs_Transporter())
			return true;
	}

	bool needs_land_unit  = goal_rec->GetTargetTypeCity() && !m_target_city.CD()->IsCoastal();
	     needs_land_unit &= goal_rec->GetTargetOwnerSelf();
//	     needs_land_unit &= !m_target_city.CD()->IsCoastal(); // This is evaluated, even if needs_land_unit is false
	     needs_land_unit |= goal_rec->GetTargetTypeGoodyHut();

	uint32 movement_type  = army->GetMovementType();
	if (army->HasCargo())
	       movement_type |= army->GetCargoMovementType();

	bool land_movement = ((movement_type &
	        (k_Unit_MovementType_Land_Bit | k_Unit_MovementType_Mountain_Bit)) != 0x0);
	land_movement &= ((movement_type & k_Unit_MovementType_Air_Bit) == 0x0);

	if (needs_land_unit && !land_movement)
	{
		return false;
	}

	if ( army->CanNukeCity() &&
		 (g_theWorld->GetCell(target_pos)->GetOwner() != m_playerId ||
		  g_theWorld->GetCity(target_pos).m_id != 0x0) )
		return false;

	if ( army->TestOrder(order_rec) == ORDER_TEST_ILLEGAL )
		return false;

	if ( !army->CanEnter(target_pos) )
		return false;

	return true;
}

void Goal::Log_Debug_Info(const int &log) const
{
#if defined(_DEBUG) || defined(USE_LOGGING)

	const char *name = g_theGoalDB->Get(m_goal_type)->GetNameText();

	MapPoint pos  = Get_Target_Pos();

	if(m_raw_priority > BAD_UTILITY)
	{
		AI_DPRINTF
		          (
		           log,
		           m_playerId,
		           m_goal_type,
		           -1,
		           (
		                "\tGoal %9x,\t%s,\tRaw priority: %8d,\t(%3d,%3d) (%s)\n",
		                this,
		                name,
		                m_raw_priority,
		                pos.x,
		                pos.y,
		                (g_theWorld->HasCity(pos) ? g_theWorld->GetCity(pos).GetName() : "field")
		           )
		          );
	}
	else
	{
		MapPoint pos = Get_Target_Pos();

		AI_DPRINTF
		          (
		           log,
		           m_playerId,
		           m_goal_type,
		           -1,
		           (
		                "\tGoal %9x,\t%s,\tBAD_UTILITY,\t(%d,%d) (%s)\n",
		                this,
		                name,
		                pos.x,
		                pos.y,
		                (g_theWorld->HasCity(pos) ? g_theWorld->GetCity(pos).GetName() : "field")
		           )
		          );
	}

	Plan_List::const_iterator plan_iter;
	sint32 count = 0;
	for
	(
	      plan_iter  = m_matches.begin();
	      plan_iter != m_matches.end();
	    ++plan_iter
	)
	{
		Agent_ptr agent = plan_iter->Get_Agent();
		Utility value   = plan_iter->Get_Matching_Value();

		if(value > Goal::BAD_UTILITY)
		{
			SQUAD_CLASS goal_squad_class = g_theGoalDB->Get(m_goal_type)->GetSquadClass();
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, -1,
				("\t\t[%3d] match=%d %s (agent: %10x), goal class=%3x, squad class=%3x, test class=%d\t",
					count++, value, g_theGoalDB->Get(m_goal_type)->GetNameText(), agent, goal_squad_class, agent->Get_Squad_Class(), ((goal_squad_class & agent->Get_Squad_Class()) == goal_squad_class)));

			agent->Log_Debug_Info(k_DBG_SCHEDULER_DETAIL, this);
		}
		else
		{
			AI_DPRINTF
			          (
			           k_DBG_SCHEDULER_DETAIL,
			           m_playerId,
			           m_goal_type,
			           -1,
			             (
			              "\t\t[%3d] First match with bad utility: In all, there were %d matches with bad utility.\n",
			              count,
			              m_matches.size() - count
			             )
			            );

			break;
		}
	}

	bool log_army = false;
	bool log_goal = CtpAiDebug::DebugLogCheck(m_playerId, m_goal_type, -1);

	Agent_List::const_iterator agent_iter;
	for
	(
	      agent_iter  = m_agents.begin();
	      agent_iter != m_agents.end();
	    ++agent_iter
	)
	{
		Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;

		if(CtpAiDebug::DebugLogCheck(m_playerId, m_goal_type, agent_ptr->Get_Army().m_id))
		{
			log_army = true;

			pos = Get_Target_Pos(agent_ptr->Get_Army());
			break;
		}
	}

	if(!log_army || !log_goal)
	{
		return;
	}

	if (m_agents.size() > 0)
		AI_DPRINTF(log,  m_playerId, m_goal_type, -1, ("\t\t\tCommitted Agents (%d):\n", m_agents.size()));

	for( agent_iter  = m_agents.begin();
		 agent_iter != m_agents.end();
		 agent_iter++
	){
		Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;
		agent_ptr->Log_Debug_Info(log, this);
	}

#endif // _DEBUG
}

bool Goal::FollowPathToTask( Agent_ptr first_army,
                                Agent_ptr second_army,
                                MapPoint &dest_pos,
                                Path &found_path)
{
	Unit city = g_theWorld->GetCity(first_army->Get_Pos());
	if(city.m_id != 0)
	{
		if(first_army->Get_Pos() != dest_pos)
		{
			if(first_army->IsNeededForGarrison())
			{
				Assert(false);
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, first_army->Get_Army().m_id,
					("GOAL %x (%d): FollowPathToTask::Can not send army needed for garrison to destination (x=%d,y=%d):\n", this,
					m_goal_type, dest_pos.x, dest_pos.y));
				first_army->Log_Debug_Info(k_DBG_SCHEDULER, this);
				uint8 magnitude = 220;
				g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), "GARRISON", magnitude);
				return false;
			}
			else
			{
				// Do nothing, city garrisons are calculated before build phase, where it only matters.
				// Garrison units are only calculated ones, so that nothing changes between process matches.
			}
		}
	}

	const GoalRecord *goal_rec = g_theGoalDB->Get(m_goal_type);
	const OrderRecord *order_rec;
	ORDER_TEST test = ORDER_TEST_OK;

	if (m_sub_task == SUB_TASK_GOAL
	||  m_sub_task == SUB_TASK_TRANSPORT_TO_GOAL) // @ToDo clean this
	{
		order_rec = goal_rec->GetExecute();
		if (first_army->Get_Army()->HasCargo() )
		{
			test = first_army->Get_Army()->CargoTestOrderHere(order_rec, dest_pos );

			if
			  (
			       first_army->Get_Army()->GetMovementTypeAir()
			   && !first_army->Get_Army()->TestOrderAny(order_rec)
			   &&  first_army->Get_Army()->TestCargoOrderAny(order_rec)
			  )
			{
				order_rec = CtpAi::GetUnloadOrder();
				dest_pos  = found_path.SnipEndUntilCargoCanEnter(first_army->Get_Army());
			}
			else if
			  (
			   (
			        first_army->Get_Pos() == dest_pos
			     && city.m_id != 0x0
			   )
			    ||
			   (
			        g_theWorld->HasCity(dest_pos)
			     && g_theWorld->GetCity(dest_pos)->GetOwner() == first_army->Get_Army()->GetOwner()
			   )
			  )
			{
				order_rec = CtpAi::GetUnloadOrder();
			}
		}
		else
		{
			test = first_army->Get_Army()->TestOrderHere(order_rec, dest_pos );
		}
	}
//	else if(m_sub_task == SUB_TASK_CARGO_TO_BOARD)
//	{
//		order_rec = CtpAi::GetBoardTransportOrder();
//	}
	else
	{
		order_rec = CtpAi::GetMoveOrder();
	}

	if(test == ORDER_TEST_OK
	|| test == ORDER_TEST_NO_MOVEMENT
	){

		//I want to see armytext even in optimized test version - Calvitix
		Utility val = Compute_Agent_Matching_Value(first_army);
		uint8 magnitude = (uint8) (((5000000 - val)* 255.0) / 5000000);
		const char * myText = goal_rec->GetNameText();
		MBCHAR * myString   = new MBCHAR[strlen(myText) + 80];
		MBCHAR * goalString = new MBCHAR[strlen(myText) + 40];
		memset(goalString, 0, strlen(myText) + 40);
		memset(myString,   0, strlen(myText) + 80);

		for (uint8 myComp = 0; myComp < strlen(myText) - 5; myComp++)
		{
			goalString[myComp] = myText[myComp + 5];
		}

		MapPoint targetPos = Get_Target_Pos(first_army->Get_Army());
		switch (m_sub_task)
		{
			case SUB_TASK_RALLY:
				sprintf(myString, "Group to (%d,%d), %s (%d,%d)", dest_pos.x, dest_pos.y, goalString, targetPos.x, targetPos.y);
				break;
			case SUB_TASK_TRANSPORT_TO_BOARD:
				sprintf(myString, "Boat to (%d,%d), %s (%d,%d)", dest_pos.x, dest_pos.y, goalString, targetPos.x, targetPos.y);
				break;
			case SUB_TASK_TRANSPORT_TO_GOAL:
				sprintf(myString, "Transp. to (%d,%d), %s (%d,%d)", dest_pos.x, dest_pos.y, goalString, targetPos.x, targetPos.y);
				break;
			case SUB_TASK_CARGO_TO_BOARD:
				sprintf(myString, "Cargo. to (%d,%d), %s (%d,%d)", dest_pos.x, dest_pos.y, goalString, targetPos.x, targetPos.y);
				break;
			case SUB_TASK_AIRLIFT:
				sprintf(myString, "Airlift to (%d,%d), %s (%d,%d)", dest_pos.x, dest_pos.y, goalString, targetPos.x, targetPos.y);
				break;
			case SUB_TASK_GOAL:
			default:
				sprintf(myString, "%s (%d,%d)", goalString, dest_pos.x, dest_pos.y);
				break;
		}

		g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), myString, magnitude, m_goal_type);
		delete[] myString;
		delete[] goalString;

		if (first_army->Get_Can_Be_Executed())
		{
			Assert(order_rec);
			if (order_rec)
			{
				if(m_sub_task == SUB_TASK_RALLY)
				{
					if(first_army->Get_Pos() != dest_pos)
					{
						first_army->PerformOrderHere(order_rec, (Path *)&found_path, GEV_INSERT_Tail);
					}
				}
				else if(m_sub_task != SUB_TASK_CARGO_TO_BOARD)
				{
					first_army->PerformOrderHere(order_rec, (Path *)&found_path);
				}
				else
				{
					first_army->PerformOrderHere(order_rec, (Path *) &found_path, GEV_INSERT_Tail);
				}
			}
		}
		else
		{
			// Nothing
		}

		return true;
	}
	else
	{
		const char * myText = goal_rec->GetNameText();
		MBCHAR * myString = new MBCHAR[strlen(myText) + 80];
		memset(myString, 0, strlen(myText) + 80);
		sprintf(myString, "%s failed at (%d, %d), order: %s", goal_rec->GetNameText(), dest_pos.x, dest_pos.y, order_rec->GetNameText());

		g_graphicsOptions->AddTextToArmy(first_army->Get_Army(), myString, 0, m_goal_type);
		delete[] myString;

		if(test != ORDER_TEST_OK && test == ORDER_TEST_NO_MOVEMENT)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, first_army->Get_Army().m_id,
				("GOAL %x (%d): FollowPathToTask:: failed TestOrderHere( %s, (%d,%d))\n", this, m_goal_type,
				order_rec->GetNameText(),dest_pos.x,dest_pos.y));
		}

		return false;
	}
}

bool Goal::GotoTransportTaskSolution(Agent_ptr the_army, Agent_ptr the_transport, MapPoint & pos)
{
	MapPoint dest_pos;
	Path found_path;
	bool check_dest;

	bool found = false;

	switch (m_sub_task)
	{

	case SUB_TASK_AIRLIFT:
	{
		MapPoint start_pos = the_army->Get_Pos();

		sint16 cargo_cont = g_theWorld->GetContinent(start_pos); // Dangerous with transport target can be closer

		Unit nearest_city;
		MapPoint nearest_airfield;
		double airfield_distance = 0.0;
		double     city_distance = 0.0;
		bool airfield_found = g_player[m_playerId]->
		                      GetNearestAirfield(start_pos, nearest_airfield, cargo_cont);
		bool     city_found = g_player[m_playerId]->
		                      GetNearestCity(start_pos, nearest_city, city_distance, false);
		if (airfield_found)
		{
			airfield_distance = MapPoint::GetSquaredDistance(start_pos, nearest_airfield);
		}

		if (airfield_found && city_found && (airfield_distance < city_distance))
			dest_pos = nearest_airfield;
		else if (city_found)
		{
			dest_pos = nearest_city.RetPos();
		}
		else
		{
			return false;
		}

		if
		  (
		       dest_pos == start_pos
		    && dest_pos == the_transport->Get_Army()->RetPos()
		  )
		{
			the_army->MoveIntoTransport();

			the_transport->Set_Can_Be_Executed(false);

			pos = dest_pos;
			return true;
		}

		check_dest = true;

		found = Agent::FindPath(the_army->Get_Army(), dest_pos, check_dest, found_path);

		if (found && FollowPathToTask(the_army, the_transport, dest_pos, found_path) )
		{
			// move_intersection = the_transport->Get_Army().GetMovementType();

			found = Agent::FindPath(the_transport->Get_Army(), dest_pos, check_dest, found_path);

			if (found && FollowPathToTask(the_transport, the_army, dest_pos, found_path) )
			{
				pos = dest_pos;
				return true;
			}
		}

		break;
	}
	case SUB_TASK_TRANSPORT_TO_BOARD:
	{
		if(!pos.IsValid())
		{
			dest_pos = the_army->Get_Pos();
			check_dest = false;
		}
		else
		{
			dest_pos = pos;
			check_dest = true;
		}

		if(!the_transport->Get_Can_Be_Executed())
		{
			pos = the_transport->Get_Target_Pos();
			Assert(pos.IsValid());
			return true;
		}

		MapPoint targetPos = Get_Target_Pos();

		Cell*  theCell = g_theWorld->GetCell(the_transport->Get_Pos());
		sint32 numUnitsInCity = the_transport->GetUnitsAtPos();
		sint32 numUnitsInArmy = the_army->Get_Army()->Num();

		if(theCell->HasCity()
		   && numUnitsInCity + numUnitsInArmy > k_MAX_ARMY_SIZE)
		{
			if(!pos.IsValid())
			{
				pos = dest_pos;
			}

			dest_pos = MoveTransportOutOfCity(the_transport, the_army);

			the_transport->Set_Target_Pos(dest_pos);
			the_transport->Set_Can_Be_Executed(false);
			return true;
		}

		if(dest_pos == the_transport->Get_Pos()
		|| the_transport->Get_Army()->CheckValidDestination(dest_pos)
		){
			if(!pos.IsValid())
			{
				pos = dest_pos;
			}

			the_transport->Set_Target_Pos(dest_pos);
			the_transport->Set_Can_Be_Executed(false);
			return true;
		}

		uint32 move_intersection =
			the_transport->Get_Army().GetMovementType() | the_army->Get_Army().GetMovementType();

		found = the_transport->FindPathToBoard(move_intersection, dest_pos, check_dest, found_path, the_army->Get_Army()->Num());

		if (!found)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_transport->Get_Army().m_id,
			        ("GOAL %x (%d):GotoTransportTaskSolution:: No path found from army to destination (x=%d,y=%d) (SUB_TASK_TRANSPORT_TO_BOARD):\n",
			        this, m_goal_type, dest_pos.x, dest_pos.y));
			the_transport->Log_Debug_Info(k_DBG_SCHEDULER, this);
			        uint8 magnitude = 220;
			        MBCHAR * myString = new MBCHAR[256];
			        sprintf(myString, "NO PATH -> BOARD (%d,%d)", dest_pos.x, dest_pos.y);
			        g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude, m_goal_type);
			        delete[] myString;
			Set_Cannot_Be_Used(the_transport, true);
		}
		else
		{
			MapPoint last = found_path.SnipEndUntilCanEnter(the_transport->Get_Army());

			if ( (found_path.GetMovesRemaining() > 0) &&
				 !FollowPathToTask(the_transport, the_army, last, found_path) )
				return false;

			if(!pos.IsValid())
			{
				pos = last;
			}

			if(found_path.GetMovesRemaining() == 0)
			{
				the_transport->Set_Can_Be_Executed(false);
				the_transport->Set_Target_Pos(pos);
			}

			return true;
		}

		break;
	}
	case SUB_TASK_CARGO_TO_BOARD:
	{
		dest_pos = pos;
		MapPoint start_pos = the_army->Get_Pos();

		Assert(pos.IsValid());

		if (dest_pos == start_pos)
		{
			the_army->Set_Can_Be_Executed(true);
			the_army->MoveIntoTransport();

			return true;
		}

		bool transport_at_rendezvous;
		if(!the_transport->Get_Army()->AtEndOfPath())
		{
			// This maybe a problem
			MapPoint next_pos;

			transport_at_rendezvous  =  the_transport->Get_Army()->GetNextPathPoint(next_pos);
			transport_at_rendezvous &= !the_transport->Get_Army()->CanEnter(next_pos);
		}
		else
		{
			transport_at_rendezvous = true;
		}

		if(transport_at_rendezvous)
		{
			uint32 move_intersection = the_army->Get_Army().GetMovementType();

			check_dest = true;

			found =  the_army->FindPathToBoard(move_intersection, dest_pos, check_dest, found_path);
		}
		else
		{
			check_dest =  false;

			found = Agent::FindPath(the_army->Get_Army(), dest_pos, check_dest, found_path);

			if(!found)
			{
				uint32 move_intersection = the_transport->Get_Army().GetMovementType() |
				                                the_army->Get_Army().GetMovementType();

				found =  the_army->FindPathToBoard(move_intersection, dest_pos, check_dest, found_path);
			}
		}

		if(!found)
		{
			AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
			        ("GOAL %x (%d): GotoTransportTaskSolution: No path found from army to destination (x=%d,y=%d) (SUB_TASK_CARGO_TO_BOARD):\n",
			        this, m_goal_type, dest_pos.x, dest_pos.y));
			the_army->Log_Debug_Info(k_DBG_SCHEDULER, this);
			        uint8 magnitude = 220;
			        MBCHAR * myString = new MBCHAR[256];
			        sprintf(myString, "NO PATH -> BOARD (%d,%d)", dest_pos.x, dest_pos.y);
			        g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude, m_goal_type);
			        delete[] myString;
		}

		if (found && FollowPathToTask(the_army, the_transport, dest_pos, found_path) )
		{
			return true;
		}
	/*	else
		{
			Set_Cannot_Be_Used(the_army, true);
		}*/

		break;
	}
	default:

		Assert(false);
	}

	return false;
}

bool Goal::GotoGoalTaskSolution(Agent_ptr the_army, MapPoint & goal_pos)
{
	if(the_army->Get_Army()->CheckValidDestination(goal_pos)) // If we are already moving along a path
		return true;

	Path found_path;

	sint32 range = 0;
	(void) g_theGoalDB->Get(m_goal_type)->GetExecute()->GetRange(range);

	bool check_dest;
	const GoalRecord * goal_rec = g_theGoalDB->Get(m_goal_type);
	if (range > 0  ||
		goal_rec->GetExecute()->GetTargetPretestAttackPosition() ||
		(goal_rec->GetTargetTypeCity() && goal_rec->GetTargetOwnerSelf()))
		check_dest = false;
	else
		check_dest = true;

	bool     waiting_for_buddies = !Ok_To_Rally()
	                            && m_sub_task == SUB_TASK_RALLY
	                            && g_theWorld->IsOnSameContinent(goal_pos, the_army->Get_Pos())
	                            && g_theWorld->IsLand(goal_pos)
	                            && g_theWorld->IsLand(the_army->Get_Pos());

	bool     found               = false;

	if(the_army->Get_Army()->HasCargo()
	&& the_army->Get_Army()->GetMovementTypeAir()
	&& the_army->Get_Army()->CanSpaceLaunch()
	){
		sint16 target_cont = g_theWorld->GetContinent(goal_pos);

		Unit   nearest_city;
		double city_distance = 0.0;
		bool   city_found    = g_player[m_playerId]->
		                       GetNearestCity(goal_pos, nearest_city, city_distance, false, target_cont);

		if (city_found)
		{
			found = Agent::FindPath(the_army->Get_Army(), nearest_city.RetPos(), true, found_path);
			if (found) Set_Sub_Task(SUB_TASK_AIRLIFT);

			if (the_army->Get_Pos() == nearest_city.RetPos())
			{
				the_army->PerformOrderHere(CtpAi::GetUnloadOrder(), (Path *) &found_path);
			}
			else if(!the_army->Get_Army()->HasLeftMap())
			{
				the_army->PerformOrderHere(CtpAi::GetSpaceLaunchOrder(), (Path *) &found_path);
			}

			return true;
		}
	}
	else if ( the_army->Get_Army()->HasCargo()
	     &&  !the_army->Get_Army()->GetMovementTypeAir())
	{
		// Check if is single squad
		// Return true if we are a transporter and we need transporters
		// SUB_TASK_TRANSPORT_TO_GOAL
		uint32 move_intersection =
		        the_army->Get_Army()->GetMovementType() | the_army->Get_Army()->GetCargoMovementType();

		found = the_army->FindPathToBoard(move_intersection, goal_pos, false, found_path);

		Assert(found); // Problem

		if (found)
		{
			if(Get_Target_Pos(the_army->Get_Army()) == goal_pos)
			{
				Set_Sub_Task(SUB_TASK_TRANSPORT_TO_GOAL);
			}
			else
			{
				Set_Sub_Task(SUB_TASK_RALLY);
			}
		}
		else
		{
			bool test = the_army->CanReachTargetContinent(goal_pos);
			Assert(test);
		}
	}
	else if ( the_army->Get_Army()->CanTransport()
	     &&  !the_army->Get_Army()->GetMovementTypeAir()
	     &&  !the_army->Get_Army()->CanEnter(goal_pos) )
	{
		// There is still a problem with air transporters
		return true;
	}
	else if ( the_army->Get_Army()->HasCargo()
	     &&   the_army->Get_Army()->GetMovementTypeAir()
	     &&   the_army->Get_Army()->RetPos().IsNextTo(goal_pos)
	     &&   goal_rec->GetTargetTypeCity()
	     &&   goal_rec->GetTargetOwnerSelf()
	     &&   g_theWorld->IsOnSameContinent(goal_pos, the_army->Get_Pos())
	     &&   the_army->Get_Army()->Num() + g_theWorld->GetCell(goal_pos)->GetNumUnits() > k_MAX_ARMY_SIZE)
	{
		the_army->UnloadCargo();
	}
	else
	{
		if (!waiting_for_buddies)
		{
			found = Agent::FindPath(the_army->Get_Army(), goal_pos, check_dest, found_path);
		}
	}

	switch (m_sub_task) {
		case SUB_TASK_GOAL:
			if (!found)
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
				           ("GOAL %x (%s): GotoGoalTaskSolution: No path found from army (x=%d,y=%d) to goal (x=%d,y=%d) (SUB_TASK_GOAL):\n",
				            this, g_theGoalDB->Get(m_goal_type)->GetNameText(), the_army->Get_Pos().x, the_army->Get_Pos().y, goal_pos.x, goal_pos.y));
				the_army->Log_Debug_Info(k_DBG_SCHEDULER, this);
				uint8 magnitude = 220;
				MBCHAR * myString = new MBCHAR[256];
				sprintf(myString, "NO PATH to (%d,%d) - %s", goal_pos.x, goal_pos.y, g_theGoalDB->Get(m_goal_type)->GetNameText());
				g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude, m_goal_type);

				delete[] myString;
			}

			break;

		case SUB_TASK_RALLY:

			if (waiting_for_buddies)
			{
				Utility val = Compute_Agent_Matching_Value(the_army);
				uint8 magnitude = (uint8)(((5000000 - val) * 255.0) / 5000000);
				MBCHAR * myString = new MBCHAR[256];
				sprintf(myString, "Waiting GROUP to GO (%d,%d)\n", goal_pos.x, goal_pos.y);
				g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude, m_goal_type);
				delete[] myString;

				return true;
			}

			if (!found)
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
				           ("GOAL %x (%d):GotoGoalTaskSolution: No path found from army (x=%d,y=%d) to goal (x=%d,y=%d) (SUB_TASK_RALLY):\n",
				           this, m_goal_type, the_army->Get_Pos().x, the_army->Get_Pos().y, goal_pos.x, goal_pos.y));
				the_army->Log_Debug_Info(k_DBG_SCHEDULER, this);
				uint8 magnitude = 220;
				MBCHAR * myString = new MBCHAR[256];
				sprintf(myString, "NO PATH (GROUP)(%d,%d)", goal_pos.x, goal_pos.y);
				g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude, m_goal_type);
				delete[] myString;
			}

			break;

		default:
		{
			if (!found)
			{
				AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, the_army->Get_Army().m_id,
				           ("GOAL %x (%d):GotoGoalTaskSolution: No path found from army (x=%d,y=%d) to goal (x=%d,y=%d) (SUB_TASK_TRANSPORT):\n",
				           this, m_goal_type, the_army->Get_Pos().x, the_army->Get_Pos().y, goal_pos.x, goal_pos.y));
				the_army->Log_Debug_Info(k_DBG_SCHEDULER, this);
				uint8 magnitude = 220;
				MBCHAR * myString = new MBCHAR[256];
				sprintf(myString, "NO PATH (TRANSP.)(%d,%d)", goal_pos.x, goal_pos.y);
				g_graphicsOptions->AddTextToArmy(the_army->Get_Army(), myString, magnitude, m_goal_type);
				delete[] myString;
			}
		}
	}

	bool move_success = false;
	if ( found )
	{
		move_success = FollowPathToTask(the_army, NULL, goal_pos, found_path);
	}
	else
	{
		move_success = TryTransport(the_army, goal_pos);
	}

	if (move_success)
	{
		the_army->Set_Can_Be_Executed(false);
	}

	return move_success;
}

bool Goal::Ok_To_Rally() const
{
	if (m_agents.size() == 1)
		return false;

	if (!g_theWorld->IsLand(Get_Target_Pos())) // Problematic if we want to attack a stack of Destroyers
		return true;

	sint32          num_in_water    = 0;
	sint32          num_at_dest     = 0;
	sint32          count           = 0;
	MapPoint        targetPos       = Get_Target_Pos();

	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		const Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;
		Assert(agent_ptr);

		if(!agent_ptr->Get_Is_Dead())
		{
			MapPoint const	army_pos		= agent_ptr->Get_Pos();

			if (g_theWorld->IsLand(army_pos))
			{
				if(g_theWorld->IsOnSameContinent(army_pos, targetPos))
				{
					++num_at_dest;
				}
				// Would have been removed for the outcommented stuff below
				else if(agent_ptr->Get_Army()->HasCargo())
				{
					++num_in_water; // Actually is beeing transported
				}
				else
				{
					return false;
				}
			}
			else
			{
				++num_in_water;
			}

			++count;
		}
	}
/*	//So far this isn't good it makes transporters to take part in the rally
	if(m_current_needed_strength.Get_Transport() > 0)
	{
		if(!Needs_Transporter())
		{
			return true;
		}
	}
*/
	return (num_in_water + num_at_dest) == count;
}

bool Goal::RallyComplete() const
{
	if (m_agents.size() == 1)
		return true;

	bool incompleteStackFound   = false;

	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		Agent_ptr agent_ptr   = (Agent_ptr) *agent_iter;
		Assert(agent_ptr);

		if (agent_ptr->Get_Is_Dead())
			continue;

		if(!agent_ptr->IsArmyPosFilled() || !agent_ptr->IsOneArmyAtPos())
		{
			if(incompleteStackFound)
			{
				return false;
			}

			incompleteStackFound = true;
		}
	}

	return true;
}

void Goal::GroupTroops()
{
	// Maybe sort the agent list by id
	// so that we use the lower ids.

	for
	(
	    Agent_List::iterator agent1_iter  = m_agents.begin();
	                         agent1_iter != m_agents.end();
	                       ++agent1_iter
	)
	{
		Agent_ptr agent1_ptr = (Agent_ptr) *agent1_iter;

		if
		  (
		        agent1_ptr->GetUnitsAtPos() == agent1_ptr->Get_Army()->Num() // Nothing to group here
		    ||  agent1_ptr->Get_Is_Dead()
		    || !agent1_ptr->Get_Can_Be_Executed()
		  )
		{
			continue;
		}

		for
		(
		    Agent_List::iterator agent2_iter  = m_agents.begin();
		                         agent2_iter != m_agents.end();
		                       ++agent2_iter
		)
		{
			Agent_ptr agent2_ptr = (Agent_ptr) *agent2_iter;

			if( agent2_ptr->Get_Is_Dead()
			|| !agent2_ptr->Get_Can_Be_Executed()
			||  agent1_ptr == agent2_ptr
			){
				continue;
			}

			if(agent1_ptr->Get_Pos() == agent2_ptr->Get_Pos())
			{
				if(agent1_ptr->CargoCanEnter())
				{
					agent1_ptr->UnloadCargo();
				}
				else if(agent2_ptr->CargoCanEnter())
				{
					agent2_ptr->UnloadCargo();
				}
				else
				{
					agent1_ptr->Group_With(agent2_ptr);
				}
			}
		}
	}
}

MapPoint Goal::MoveToTarget(Agent_ptr rallyAgent)
{
	bool check_dest;
	const GoalRecord * goal_rec = g_theGoalDB->Get(m_goal_type);
	sint32 range = 0;
	(void) goal_rec->GetExecute()->GetRange(range);
	if (range > 0  ||
		goal_rec->GetExecute()->GetTargetPretestAttackPosition() ||
		(goal_rec->GetTargetTypeCity() && goal_rec->GetTargetOwnerSelf()))
		check_dest = false;
	else
		check_dest = true;

	Path found_path;
	bool found = Agent::FindPath(rallyAgent->Get_Army(), Get_Target_Pos(rallyAgent->Get_Army()), check_dest, found_path);

	Assert(found);

	if(!found)
	{
		return rallyAgent->Get_Pos();
	}

//	MapPoint rallyPos = found_path.SnipEndUntilCannotEnter(rallyAgent->Get_Army());
	MapPoint rallyPos = found_path.SnipEndUntilCargoCanEnter(rallyAgent->Get_Army());

	FollowPathToTask(rallyAgent, NULL, rallyPos, found_path);

	return rallyPos;
}

MapPoint Goal::MoveAwayFromTargetCity(Agent_ptr rallyAgent)
{
	MapPoint  rallyPos = rallyAgent->Get_Pos();
	MapPoint targetPos = Get_Target_Pos();

	if(m_target_city.IsValid() && rallyPos.IsNextTo(targetPos))
	{
		MapPoint tempPos;
		for(sint32 i = 0; i < NOWHERE; i++)
		{
			if(rallyPos.GetNeighborPosition(WORLD_DIRECTION(i), tempPos))
			{
				if(targetPos.IsNextTo(tempPos))
					continue;

				if(!g_theWorld->IsOnSameContinent(targetPos, tempPos))
					continue;

				if(!g_theWorld->GetArmyPtr(tempPos)
				   && (    ( rallyAgent->Get_Army()->HasCargo()
				        &&   rallyAgent->Get_Army()->CargoCanEnter(tempPos))
				        || (!rallyAgent->Get_Army()->HasCargo()
				        &&   rallyAgent->Get_Army()->CanEnter(tempPos))
				      )
				  )
				{
					// Search for a cell without an army
					rallyPos = tempPos;
					if(!GotoGoalTaskSolution(rallyAgent, rallyPos))
						Assert(false);

					break;
				}
			}
		}
	}

	return rallyPos;
}

MapPoint Goal::MoveOutOfCity(Agent_ptr rallyAgent)
{
	MapPoint rallyPos = rallyAgent->Get_Pos();
	if(g_theWorld->GetCity(rallyPos).IsValid())
	{
		MapPoint tempPos;
		for(sint32 i = 0 ; i < NOWHERE; i++)
		{
			if(rallyPos.GetNeighborPosition(WORLD_DIRECTION(i), tempPos))
			{
				if(!g_theWorld->GetArmyPtr(tempPos)
				&& rallyAgent->Get_Army()->CanEnter(tempPos)
				){
					// Search for a cell without an army
					rallyPos = tempPos;
					break;
				}
			}
		}

		if(!GotoGoalTaskSolution(rallyAgent, rallyPos))
			Assert(false);
	}

	return rallyPos;
}

MapPoint Goal::MoveTransportOutOfCity(Agent_ptr transport, Agent_ptr cargo)
{
	sint32 minDistance = 0x7fffffff;

	MapPoint transportPos = transport->Get_Pos();
	if(g_theWorld->GetCity(transportPos).IsValid())
	{
		MapPoint tempPos;
		for(sint32 i = 0; i < NOWHERE; i++)
		{
			if(transportPos.GetNeighborPosition(WORLD_DIRECTION(i), tempPos))
			{
				if(!g_theWorld->GetArmyPtr(tempPos)
				   && transport->Get_Army()->CanEnter(tempPos)
				   )
				{
					sint32 distance = MapPoint::GetSquaredDistance(transport->Get_Pos(), tempPos);

					if(distance < minDistance)
					{
						minDistance = distance;
						transportPos = tempPos;
					}
				}
			}
		}

		if(!GotoGoalTaskSolution(transport, transportPos))
			Assert(false);
	}

	return transportPos;
}

Agent_ptr Goal::GetRallyAgent() const
{
	MapPoint targetPos              = Get_Target_Pos();
	Agent_ptr rallyAgent            = NULL;
	sint32 minDistance              = 0x7fffffff;

	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;

		if(agent_ptr->GetUnitsAtPos() >= k_MAX_ARMY_SIZE)
		{
			if(rallyAgent == NULL)
			{
				// If every possible agent is in a full city
				rallyAgent = agent_ptr;
			}
			continue;
		}

		sint32 distance = MapPoint::GetSquaredDistance(agent_ptr->Get_Pos(), targetPos);

		if(distance < minDistance)
		{
			minDistance = distance;
			rallyAgent  = agent_ptr;
		}
	}

	Assert(rallyAgent != NULL);

	return rallyAgent;
}

MapPoint Goal::GetFreeNeighborPos(MapPoint pos) const
{
	MapPoint nextPos(-1, -1);
	MapPoint tempPos;
	for(sint32 i = 0 ; i < NOWHERE; i++)
	{
		bool result = pos.GetNeighborPosition(WORLD_DIRECTION(i), tempPos);
		if(result)
		{
			CellUnitList *the_army = g_theWorld->GetArmyPtr(tempPos);
			if(!the_army)
			{	//search for cell without army
				nextPos = tempPos;
				break;
			}
		}
	}

	// Could happen in that case we have to find something else
	// Assert(nextPos.IsValid());

	return nextPos;
}

bool Goal::RallyTroops()
{
	Set_Sub_Task(SUB_TASK_RALLY);

// A new rally algorithm, but actually the old one is better in principle
// but unfortunately less complete
#if 1
	// Group armies first
	GroupTroops();
	Agent_ptr rallyAgent = GetRallyAgent();

	if(rallyAgent == NULL)
	{
		return true;
	}

	MapPoint     rallyPos = rallyAgent->Get_Army()->HasCargo() ? MoveToTarget(rallyAgent) : MoveOutOfCity(rallyAgent);
	             rallyPos = MoveAwayFromTargetCity(rallyAgent);

	if(rallyPos == rallyAgent->Get_Pos()
	&& rallyAgent->Get_Army()->HasCargo()
	&& rallyAgent->CargoCanEnter()
	){
		rallyAgent->UnloadCargo();
	}

	rallyAgent->WaitHere(Get_Target_Pos(rallyAgent->Get_Army()));

	Squad_Strength strength = m_current_attacking_strength;

//	Assert(rallyPos != Get_Target_Pos(rallyAgent->Get_Army())); // Actually a valid situation, for choke points

	sint32 unitsAtRallyPos = (rallyPos == rallyAgent->Get_Pos()) ? rallyAgent->GetUnitsAtPos() : rallyAgent->Get_Army()->Num();

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;
		Assert(agent_ptr);

		if( agent_ptr->Get_Is_Dead()
		|| !agent_ptr->Get_Can_Be_Executed()
		){
			continue;
		}

		if(!agent_ptr->CanMove()
		&& !agent_ptr->Get_Army()->HasCargo()
		||  agent_ptr == rallyAgent
		){
			agent_ptr->Set_Can_Be_Executed(false);
			continue;
		}

		if
		  (
		       agent_ptr->Get_Army()->Num() >= k_MAX_ARMY_SIZE
		    && rallyPos.IsNextTo(agent_ptr->Get_Pos())
		  )
		{
			agent_ptr->WaitHere(Get_Target_Pos(agent_ptr->Get_Army()));
			continue;
		}

		// Send units to rally position
		if(!GotoGoalTaskSolution(agent_ptr, rallyPos))
		{
			// @ToDo: Check for transports, too.
			strength -= agent_ptr->Get_Squad_Strength();
			if(strength.HasEnough(m_current_needed_strength))
			{
				continue;
			}
			else
			{
				return false;
			}
		}

		if(unitsAtRallyPos + agent_ptr->Get_Army()->Num() > k_MAX_ARMY_SIZE
		&& agent_ptr->Get_Pos().IsNextTo(rallyPos))
		{

			// Change target
			if(agent_ptr->Get_Army()->Num() < k_MAX_ARMY_SIZE)
			{
				rallyPos = agent_ptr->Get_Pos();

				// Outgroup the units over the limit
				// Outgrouping is done before the army is send to their target
				agent_ptr->Get_Army()->Split(agent_ptr->Get_Army()->Num() - unitsAtRallyPos);
				agent_ptr->Set_Can_Be_Executed(false);

				unitsAtRallyPos = agent_ptr->Get_Army()->Num() - unitsAtRallyPos;

				uint8 magnitude = 220;
				MBCHAR * myString = new MBCHAR[256];
				MapPoint goal_pos = Get_Target_Pos(agent_ptr->Get_Army());
				MapPoint curr_pos = agent_ptr->Get_Pos();
				sprintf(myString, "Split at (%d,%d) to GO (%d,%d)", curr_pos.x, curr_pos.y, goal_pos.x, goal_pos.y);
				g_graphicsOptions->AddTextToArmy(agent_ptr->Get_Army(), myString, magnitude);
				delete[] myString;
			}
		}
	}
#else
	Agent_List tmp_agents            = m_agents;
	Agent_List::iterator agent1_iter = tmp_agents.begin();

	/// @ToDo: Check whether this sqaure design is necessary
	while (agent1_iter != tmp_agents.end())
	{
		Agent_ptr agent1_ptr = (Agent_ptr) *agent1_iter;
		Assert(agent1_ptr);

		if( agent1_ptr->Get_Is_Dead()
		|| !agent1_ptr->Get_Can_Be_Executed()
		){
			++agent1_iter;
			continue;
		}

		bool agent1_is_partial = (agent1_ptr->Get_Army().Num() < k_MAX_ARMY_SIZE );

		sint32 min_distance = (g_mp_size.x + g_mp_size.y);
		min_distance *= min_distance;
		Agent_List::iterator closest_agent_iter = tmp_agents.end();
		bool partiality_found = false;

		for
		(
		    Agent_List::iterator agent2_iter  = tmp_agents.begin();
		                         agent2_iter != tmp_agents.end();
		                       ++agent2_iter
		)
		{
			Agent_ptr agent2_ptr = (Agent_ptr) *agent2_iter;

			if(agent1_ptr == agent2_ptr)
				continue;

			if(agent2_ptr->Get_Is_Dead())
				continue;
			bool agent2_is_partial = (agent2_ptr->Get_Army().Num() < k_MAX_ARMY_SIZE);
			if( (partiality_found) &&
				(agent2_is_partial != agent1_is_partial) )
				continue;

			sint32 const distance =
			    MapPoint::GetSquaredDistance(agent1_ptr->Get_Pos(), agent2_ptr->Get_Pos());

			if(distance < min_distance)
			{
				min_distance        = distance;
				closest_agent_iter  = agent2_iter;
			}

			if( agent1_is_partial == agent2_is_partial &&
				!partiality_found)
			{
				partiality_found = true;
				min_distance = distance;
				closest_agent_iter = agent2_iter;
			}
		}

		if(min_distance < 1)
		{
			agent1_ptr->Group_With((Agent_ptr) *closest_agent_iter);
		}
		else if( closest_agent_iter != tmp_agents.end() )
		{
			MapPoint closest_agent_pos;

			Agent_ptr closest_agent_ptr = (Agent_ptr) *closest_agent_iter;

			MapPoint closest_agent_pos;
			if(closest_agent_ptr->Get_Can_Be_Executed())
			{
				closest_agent_pos = closest_agent_ptr->Get_Pos();
			}
			else
			{
				closest_agent_pos = closest_agent_ptr->Get_Target_Pos();
			}

			// To avoid Groups to be blocked when an unit is in a city
			// (problem with garrison -> not enough room)
			sint32 cells;
			if(!g_theWorld->GetCity(closest_agent_pos).IsValid()
			||  agent1_ptr->GetRounds(closest_agent_pos, cells) > 2
			){
				// Should be superflous
				Set_Sub_Task(SUB_TASK_RALLY);
				if (!GotoGoalTaskSolution(agent1_ptr, closest_agent_pos))
					return false;
			}
			else
			{
				uint8 magnitude = 220;
				MBCHAR * myString = new MBCHAR[256];
				MapPoint goal_pos;
				goal_pos = Get_Target_Pos(agent1_ptr->Get_Army());
				sprintf(myString, "Waiting GROUP to GO (%d,%d)", goal_pos.x, goal_pos.y);
				g_graphicsOptions->AddTextToArmy(agent1_ptr->Get_Army(), myString, magnitude);
				delete[] myString;
			}

			MapPoint agent1_pos = agent1_ptr->Get_Pos();
			if( g_theWorld->GetCity(closest_agent_pos).IsValid() || closest_agent_ptr->GetRounds(agent1_pos, cells) > 2)
			{
				if (g_theWorld->GetCity(agent1_pos).IsValid() && g_theWorld->GetCity(closest_agent_pos).IsValid()) //two units are in another town
				{
					MapPoint tempPos;
					for(sint32 i = 0 ; i < NOWHERE; i++)
					{
						if(closest_agent_pos.GetNeighborPosition(WORLD_DIRECTION(i),tempPos))
						{
							CellUnitList *the_army = g_theWorld->GetArmyPtr(tempPos);
							if(!the_army)
							{	//search for cell without army
								agent1_pos = tempPos;
								break;
							}
						}
					}
				}

				// Should be superflous
				Set_Sub_Task(SUB_TASK_RALLY);
				if(!GotoGoalTaskSolution(closest_agent_ptr, agent1_pos))
					return false;
			}
			else
			{
				CellUnitList *the_army = g_theWorld->GetArmyPtr(closest_agent_pos);
				if(static_cast<uint32>(the_army->Num()) >= m_agents.size() && m_agents.size() > k_MAX_ARMY_SIZE/2)
				{
					MapPoint tempPos;
					for(sint32 i = 0 ; i < NOWHERE; i++)
					{
						if(closest_agent_pos.GetNeighborPosition(WORLD_DIRECTION(i),tempPos))
						{
							CellUnitList *the_army = g_theWorld->GetArmyPtr(tempPos);
							if(!the_army)
							{	//search for cell without army
								// Should be superflous
								Set_Sub_Task(SUB_TASK_RALLY);
								if(!GotoGoalTaskSolution(closest_agent_ptr, tempPos))
									return false;
								break;
							}
						}
					}
				}
			}

			/// @ToDo: Check whether this is necessary, and what does it do
			tmp_agents.insert(tmp_agents.begin(), *closest_agent_iter);
			tmp_agents.erase(closest_agent_iter);
		}
		else
		{
			Assert(false);
		}

		++agent1_iter;
	}
#endif

	return true;
}

bool Goal::UnGroupTroops()
{
	bool            breturn     = false;

	for
	(
	    Agent_List::iterator agent1_iter  = m_agents.begin();
	                         agent1_iter != m_agents.end();
	                       ++agent1_iter
	)
	{
		Agent_ptr    agent1_ptr   = (Agent_ptr) *agent1_iter;
		Assert(agent1_ptr);

		if (!agent1_ptr->Get_Is_Dead())
		{
			if (agent1_ptr->Get_Army().Num() > 1)
			{
				agent1_ptr->Ungroup_Order();
			}

			breturn = true;
		}
	}

	return breturn;
}

bool Goal::UnGroupComplete() const
{
	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;
		Assert(agent_ptr);

		if (!agent_ptr->Get_Is_Dead() &&
		    (agent_ptr->Get_Army().Num() > 1)
		   )
		{
			return false;
		}
	}

	return true;
}

bool Goal::TryTransport(Agent_ptr agent_ptr, const MapPoint & goal_pos)
{
	if (g_theGoalDB->Get(m_goal_type)->GetNoTransport())
		return false;

	return LoadTransporters(agent_ptr);
}

bool Goal::FindTransporters(const Agent_ptr & agent_ptr, std::list< std::pair<Utility, Agent_ptr> > & transporter_list)
{
	std::pair<Utility, Agent_ptr> transporter;

	for
	(
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		Agent_ptr    possible_transport  = (Agent_ptr) *agent_iter;

		sint32          transports          = 0;
		sint32          max_slots           = 0;
		sint32          empty_slots         = 0;
		possible_transport->Get_Army()->GetCargo(transports, max_slots, empty_slots);

		AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, m_goal_type, possible_transport->Get_Army().m_id,
			("GOAL %x Agent %x (%d): transports: %d, max_slots: %d, empty_slots %d\n",
			 this, possible_transport, m_goal_type, transports, max_slots, empty_slots));

		if(max_slots <= 0)
			continue;

		if(empty_slots <= 0)
			continue;

		if(possible_transport == agent_ptr)
			continue;

		SQUAD_CLASS goal_squad_class = g_theGoalDB->Get(m_goal_type)->GetSquadClass();

		if(max_slots != empty_slots && ((goal_squad_class & possible_transport->Get_Squad_Class()) != goal_squad_class))
			continue;

		if(!possible_transport->CanReachTargetContinent(Get_Target_Pos()))
		{
			continue;
		}

		Utility  utility         = Goal::BAD_UTILITY;
		if(agent_ptr->EstimateTransportUtility(possible_transport, utility))
		{
			transporter.first  = utility;
			transporter.second = possible_transport;

			transporter_list.push_back(transporter);
		}
	}

	// Probably more stuff needs to be done here
	m_current_needed_strength.Set_Transport(static_cast<sint8>(agent_ptr->Get_Army()->Num()));

	transporter_list.sort(std::greater<std::pair<Utility,class Agent *> >());
	return transporter_list.size() > 0;
}

bool Goal::LoadTransporters(Agent_ptr agent_ptr)
{
	std::list< std::pair<Utility, Agent_ptr> > transporter_list;

	if(!FindTransporters(agent_ptr, transporter_list))
		return false;

	bool success = false;
	sint32 foundSlots = 0;

	MapPoint pos(-1,-1);

	/// @ToDo: Add transport escorts
	for
	(
	    std::list< std::pair<Utility, Agent_ptr> >::iterator
	        transporter  = transporter_list.begin();
	        transporter != transporter_list.end();
	      ++transporter
	)
	{
		Agent_ptr transport_ptr = transporter->second;

		transport_ptr->Log_Debug_Info(k_DBG_SCHEDULER, this);

		if(transport_ptr->Get_Army()->GetMovementTypeAir()
		&& transport_ptr->Get_Army()->CanSpaceLaunch()
		){
			// @ToDo: Check whether this really works
			Set_Sub_Task(SUB_TASK_AIRLIFT);
		}
		else
		{
			Set_Sub_Task(SUB_TASK_TRANSPORT_TO_BOARD);
		}

		if(!GotoTransportTaskSolution(agent_ptr, transport_ptr, pos))
			continue;

		success = true;

		Assert(pos.IsValid());

		sint32 transporters = 0;
		sint32 max          = 0;
		sint32 empty        = 0;

		transport_ptr->Get_Army()->GetCargo(transporters, max, empty);

		foundSlots += empty;

		if(agent_ptr->Get_Army()->Num() <= foundSlots)
			break;
	}

	if (success)
	{
		Assert(foundSlots != 0);

		if(foundSlots == 0)
		{
			return false;
		}

		Agent_ptr transport_ptr = transporter_list.begin()->second;
		Set_Sub_Task(SUB_TASK_CARGO_TO_BOARD);
		success = GotoTransportTaskSolution(agent_ptr, transport_ptr, pos);

		g_player[m_playerId]->
			AddCargoCapacity(static_cast<sint16>(-1 * agent_ptr->Get_Army().Num()));
	}
	else
	{
		Set_Sub_Task(SUB_TASK_GOAL);
	}

	return success;
}

bool Goal::CanReachTargetContinent(Agent_ptr agent_ptr) const
{
	return agent_ptr->CanReachTargetContinent(Get_Target_Pos());
}

bool Goal::ArmiesAtGoal() const
{
	MapPoint pos = Get_Target_Pos();

	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		Agent_ptr agent_ptr = (Agent_ptr) *agent_iter;

		if (agent_ptr->Get_Pos() != pos)
			return false;
	}

	return true;
}

sint32 Goal::GetThreatenBonus() const
{
	const GoalRecord *goal_record = g_theGoalDB->Get(m_goal_type);

	if (goal_record->GetThreatenTypeNone())
		return 0;

	switch (goal_record->GetThreatenType())
	{
		case k_Goal_ThreatenType_DestroyCity_Bit:
		{
			Threat           tmp_threat;
			const Diplomat & diplomat    = Diplomat::GetDiplomat(m_playerId);

			if (diplomat.HasThreat(Get_Target_Owner(), THREAT_DESTROY_CITY, tmp_threat))
			{
				if (tmp_threat.detail.arg.cityId == m_target_city.m_id)
					return goal_record->GetThreatenBonus();
			}
			break;
		}
	}

	return 0;
}

bool Goal::Goal_Too_Expensive() const
{
#if defined(_DEBUG) || defined(USE_LOGGING)
	if(    (g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
	    &&  g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity()
	    &&  m_current_attacking_strength.Get_Agent_Count() > k_MAX_ARMY_SIZE)
	    && (m_current_attacking_strength.Get_Value() >
	            m_current_needed_strength.Get_Value() * 3
	       )
	  )
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_playerId, m_goal_type, -1,
			("\t\tGOAL (goal: %x of %s) too expensive, agent count %d, current value %f, needed value %f\n",
			 this, g_theGoalDB->Get(m_goal_type)->GetNameText(), m_current_attacking_strength.Get_Agent_Count(), m_current_attacking_strength.Get_Value(), m_current_needed_strength.Get_Value()));
	}
#endif

	return (g_theGoalDB->Get(m_goal_type)->GetTargetOwnerSelf()
	    &&  g_theGoalDB->Get(m_goal_type)->GetTargetTypeCity()
	    &&  m_current_attacking_strength.Get_Agent_Count() > k_MAX_ARMY_SIZE)
	    && (m_current_attacking_strength.Get_Value() >
	            m_current_needed_strength.Get_Value() * 3
	       );
}

MapPoint Goal::GetClosestTransporterPos(const Agent_ptr agent_ptr) const
{
	sint32            best_squared_dist = 0x7fffffff;
	MapPoint          best_target_pos   = MapPoint(-1, -1);

	for
	(
	    Plan_List::const_iterator match_iter  = m_matches.begin();
	                              match_iter != m_matches.end();
	                            ++match_iter
	)
	{
		if(!match_iter->Get_Agent()->Get_Is_Dead() && match_iter->Get_Needs_Cargo())
		{
			sint32 tmp_squared_dist = MapPoint::GetSquaredDistance(match_iter->Get_Agent()->Get_Pos(), agent_ptr->Get_Pos());
			if (tmp_squared_dist < best_squared_dist)
			{
				best_squared_dist = tmp_squared_dist;
				best_target_pos = match_iter->Get_Agent()->Get_Pos();
			}
		}
	}

	return best_target_pos;
}

MapPoint Goal::GetClosestCargoPos(const Agent_ptr agent_ptr) const
{
	sint32            best_squared_dist = 0x7fffffff;
	MapPoint          best_target_pos   = MapPoint(-1, -1);

	for
	(
	    Plan_List::const_iterator match_iter  = m_matches.begin();
	                              match_iter != m_matches.end();
	                            ++match_iter
	)
	{
		if(!match_iter->Get_Agent()->Get_Is_Dead() && match_iter->Get_Needs_Transporter())
		{
			sint32 tmp_squared_dist = MapPoint::GetSquaredDistance(match_iter->Get_Agent()->Get_Pos(), agent_ptr->Get_Pos());
			if (tmp_squared_dist < best_squared_dist)
			{
				best_squared_dist = tmp_squared_dist;
				best_target_pos = match_iter->Get_Agent()->Get_Pos();
			}
		}
	}

	return best_target_pos;
}

void Goal::ResetNeededTransport()
{
	m_current_needed_strength.Set_Transport(0);
}
