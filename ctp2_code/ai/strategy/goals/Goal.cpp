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
// - Consider a goal with max armies engaged as satisfied - the limitation of 
//   army size : we cannot form a group with more armies than the max (can 
//   disturb the goals with RallyFirst() - Calvitix
// - Changes the const attribute for Compute_Matching_Value (Raw_Priority will 
//   be changed on wounded case) - Calvitix
// - Linux support modifications + cleanup.
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Goal.h"
using namespace std;

const Utility Goal::BAD_UTILITY = -99999999;
const Utility Goal::MAX_UTILITY =  99999999;

#include "squad_Strength.h"
#include "agent.h"
#include "ArmyPool.h"
#include "debugassert.h"

#include "gstypes.h"
#include "gfx_options.h"
#include "ctpagent.h"
#include "World.h"
#include "Squad.h"

#ifdef _DEBUG_SCHEDULER
#include "ctpagent.h"
#include "ArmyData.h"
#include "ctpgoal.h"
#endif //_DEBUG_SCHEDULER
#include "ctpaidebug.h"
#include "GoalRecord.h"

Goal::Goal()
:
    m_goal_type                     (GOAL_TYPE_NULL),
    m_raw_priority                  (BAD_UTILITY),
    m_removal_time                  (DONT_REMOVE),
    m_is_invalid                    (false),
    m_current_needed_strength       (),
    m_current_attacking_strength    (),
    m_current_projected_strength    (),
    m_matches                       (),
    m_agents                        (),
    m_playerId                      (PLAYER_UNASSIGNED),
    m_combinedUtility               (0),
    m_needs_sorting                 (false)
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
	Assert(false);
	// Should be removed
	m_goal_type                  = goal.m_goal_type;
	m_playerId                   = goal.m_playerId;
	m_raw_priority               = goal.m_raw_priority;
	m_removal_time               = goal.m_removal_time;
	m_is_invalid                 = goal.m_is_invalid;
	m_current_needed_strength    = goal.m_current_needed_strength;
	m_current_attacking_strength = goal.m_current_attacking_strength;
	m_current_projected_strength = goal.m_current_projected_strength;
	m_matches                    = goal.m_matches;
	m_agents                     = goal.m_agents;
	m_combinedUtility            = goal.m_combinedUtility;
	m_needs_sorting              = goal.m_needs_sorting;

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
	return m_agents.size() == 1;
}

bool Goal::Commit_Agent(const Agent_ptr & agent)
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

#ifdef _DEBUG_SCHEDULER
		CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) agent;

		Assert(m_current_attacking_strength.Get_Agent_Count() >= m_agents.size());
		if (m_current_attacking_strength.Get_Agent_Count() < m_agents.size())
		{
			Assert(0);
		}
#endif // _DEBUG_SCHEDULER

		return true;
	}

	return false;
}

const Agent_List & Goal::Get_Agent_List() const
{
	return m_agents;
}
void Goal::Rollback_Agent(Agent_ptr agent_ptr)
{
#ifdef _DEBUG_SCHEDULER
//	CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr) agent_ptr;
//	if (g_theArmyPool->IsValid(ctpagent_ptr->Get_Army()))
//		Assert(ctpagent_ptr->Get_Army()->m_theGoal == this);
#endif // _DEBUG_SCHEDULER

	m_current_attacking_strength.Remove_Agent_Strength(agent_ptr);

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

	next_agent_iter = m_agents.erase(next_agent_iter);

	Assert(m_current_attacking_strength.Get_Agent_Count() >= m_agents.size());
#ifdef _DEBUG_SCHEDULER
	if (m_current_attacking_strength.Get_Agent_Count() < m_agents.size())
	{
		Assert(0);
	}
#endif // _DEBUG_SCHEDULER

}

Utility Goal::Get_Raw_Priority() const
{
	return m_raw_priority;
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
	    Agent_List::iterator agent_iter  = m_agents.begin();
	                         agent_iter != m_agents.end();
	                       ++agent_iter
	)
	{
		(*agent_iter)->Set_Needs_Transporter(needs_transporter);
	}
}

bool Goal::Validate() const
{
#ifdef _DEBUG

	for
	(
	    Agent_List::const_iterator agent_iter  = m_agents.begin();
	                               agent_iter != m_agents.end();
	                             ++agent_iter
	)
	{
		uint32 *first_bytes =
			(uint32 *)&(*(*agent_iter));

		if(*first_bytes == 0xdddddddd)
		{
			bool ARMY_DELETED_WITHOUT_TELLING_GOAL = false;
			Assert(ARMY_DELETED_WITHOUT_TELLING_GOAL);
		}

		Assert((*agent_iter)->Has_Goal());

		Plan_List::const_iterator match_iter;
		for
		(
		            match_iter  = m_matches.begin();
		            match_iter != m_matches.end();
		          ++match_iter
		)
		{
			if(match_iter->Agent_Committed()) // With the simplification this doesn't make much sense anymore
			{
#ifdef _DEBUG_SCHEDULER

				Squad_ptr tmp_squad_ptr = match_iter->Get_Squad();
				Goal_ptr tmp_goal_ptr = match_iter->Get_Goal();
				CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr)(*agent_iter);

				if(!ctpagent_ptr->Get_Is_Dead())
				{
//					Assert(ctpagent_ptr->Get_Army()->m_theAgent == ctpagent_ptr);
//					Assert(ctpagent_ptr->Get_Army()->m_theGoal == tmp_goal_ptr);
				}
				else
				{
					// Do nothing army might be dead and is going to be removed
				}
#endif // _DEBUG_SCHEDULER

				break;
			}
		}

		if(match_iter == m_matches.end())
		{

#ifdef _DEBUG_SCHEDULER
//			CTPAgent_ptr ctpagent_ptr = (CTPAgent_ptr)(*agent_iter);
//			Assert(ctpagent_ptr->Get_Army().AccessData()->m_theAgent == ctpagent_ptr);
//			Assert(ctpagent_ptr->Get_Army().AccessData()->m_theGoal != NULL);
#endif // _DEBUG_SCHEDULER

			Assert(0);
		}
	}

#endif // _DEBUG

	return true;
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

#include "MapAnalysis.h"

Utility Goal::Compute_Matching_Value(const bool update)
{
	AI_DPRINTF
	          (
	           k_DBG_SCHEDULER_ALL, m_playerId,
	           -1,
	           -1,
	           (
	            "\tCompute Matching Value for goal: %s, raw_match: %i (%s)\n",
	            g_theGoalDB->Get(m_goal_type)->GetNameText(),
	            m_raw_priority,
	            (g_theWorld->HasCity(Get_Target_Pos()) ? g_theWorld->GetCity(Get_Target_Pos()).GetName() : "field")
	           )
	          );

	sint32 count = 0;

	Plan_List::iterator match_iter;

	for
	(
	            match_iter  = m_matches.begin();
	            match_iter != m_matches.end();
	          ++match_iter
	)
	{
		// Maybe this also needs to be handled.
		if(!match_iter->Plan_Is_Needed_And_Valid())
		{
			Assert(false);
			continue;
		}

		match_iter->Compute_Matching_Value();
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
					("\t\t[%3d] match = %d %s\n", count, match_iter->Get_Matching_Value(), g_theGoalDB->Get(m_goal_type)->GetNameText()));
		++count;
	}

	Sort_Matches();

	return Recompute_Matching_Value(update);
}

Utility Goal::Recompute_Matching_Value(const bool update, const bool show_strength)
{
	if(!update)
	{
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
					("\tCompute Matching Value for goal: %x, %s, raw_match: %i\n", this, g_theGoalDB->Get(m_goal_type)->GetNameText(), m_raw_priority));
	}

	const GoalRecord * goal_record  = g_theGoalDB->Get(m_goal_type);

	Utility combinedUtility = 0;

	sint32 count = 0;
	m_current_projected_strength = Squad_Strength();
	for
	(
	    Plan_List::iterator
	            match_iter  = m_matches.begin();
	            match_iter != m_matches.end();
	          ++match_iter
	)
	{
		if(!match_iter->All_Unused_Or_Used_By_This())
			continue;

		if(match_iter->Needs_Cargo())
			continue;

		if(!m_current_projected_strength.HasEnough(m_current_needed_strength)
//		||  goal_record->GetNeverSatisfied() // Should be considered in Commit_Agents
		){
			Utility matchUtility = match_iter->Get_Matching_Value();

			if(matchUtility > Goal::BAD_UTILITY)
			{
				Squad_Strength strength;
				match_iter->Get_Squad()->Get_Strength(strength);
				m_current_projected_strength += strength;

				combinedUtility += matchUtility;
				AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
							("\t\t[%3d] match = %d %s\n", count, matchUtility, g_theGoalDB->Get(m_goal_type)->GetNameText()));
				++count;
			}
			else
			{
				AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
							("\t\t[%3d]First match with bad utility for goal %s, stop matching\n", count, g_theGoalDB->Get(m_goal_type)->GetNameText()));
				if(count == 0)
				{
					Log_Debug_Info(k_DBG_SCHEDULER_ALL);
				}

				break;
			}
		}
		else
		{
			AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,
						("\t\t[%3d] Enough ressources found for goal %s\n", count, g_theGoalDB->Get(m_goal_type)->GetNameText()));
			break;
		}
	}

#if defined(_DEBUG) || defined(USE_LOGGING)
	if(update && show_strength)
	{
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\n"));
		m_current_projected_strength.Log_Debug_Info(k_DBG_SCHEDULER_ALL, m_playerId, "The Projected Strength:  ");
		m_current_needed_strength   .Log_Debug_Info(k_DBG_SCHEDULER_ALL, m_playerId, "The Needed Strength:     ");
		Squad_Strength strength;
		strength.Set_Pos_Strength(Get_Target_Pos());
		strength                    .Log_Debug_Info(k_DBG_SCHEDULER_ALL, m_playerId, "The Target Pos Strength: ");
		Squad_Strength grid_strength;
		grid_strength.Set_Enemy_Grid_Strength(Get_Target_Pos(), m_playerId);
		grid_strength               .Log_Debug_Info(k_DBG_SCHEDULER_ALL, m_playerId, "The Target Grid Strength:");
	}
#endif

	if
	  (
	       (
	            !m_current_projected_strength.HasEnough(m_current_needed_strength)
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

	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1,("\tMatch value: %i\n", combinedUtility));
	if(update)
	{
		m_combinedUtility = combinedUtility;
	}

	AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, -1, -1, ("\t\n"));

	return combinedUtility;
}

Utility Goal::Get_Matching_Value() const
{
	return m_combinedUtility;
}

void Goal::Set_Matching_Value(Utility combinedUtility)
{
	m_combinedUtility = combinedUtility;
}

bool Goal::Add_Match(const Squad_ptr & squad, const bool update_match_value, const bool needsCargo)
{
	for
	   (
	    Plan_List::iterator   plan_test_iter  = m_matches.begin();
	                          plan_test_iter != m_matches.end();
	                        ++plan_test_iter
	   )
	{
		Assert(plan_test_iter->Get_Squad() != squad);
	}

	Plan the_match(squad, this, needsCargo);

	Assert(!Get_Invalid())
	Assert(squad->Get_Num_Agents() > 0)

	if(the_match.Can_Add_To_Goal())
	{
		if(update_match_value)
		{
			Utility matching_value = the_match.Compute_Matching_Value();
		}

#if defined(USE_GOAL_REF)
		m_matches.push_back(the_match);
		squad->Add_Goal_Reference(this);
#else
		Plan_List::iterator & plan_iter = m_matches.insert(m_matches.end(), the_match);
		squad->Add_Match_Reference(plan_iter);
#endif
		m_needs_sorting = true;

		/*
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, m_playerId, this->Get_Goal_Type(), -1, 
				("\tAdded match for goal: %x squad: %x  value = %d\n",
				 this,
				 (*squad_iter),
				 matching_value));
		*/

		return true;
	}
	else
	{
	/*
		AI_DPRINTF(k_DBG_SCHEDULER_ALL, m_playerId, this->Get_Goal_Type(), -1, 
		("\tMatch for goal: %x (%d) squad: %x has BAD_UTILITY.\n",
			 this,
		goal_iter->second->Get_Goal_Type(),
			 (*squad_iter)));
	*/
		return false;
	}
}

bool Goal::CanGoalBeReevaluated() const
{
	GoalRecord const *  goalRecord      = g_theGoalDB->Get(m_goal_type);

	Assert(goalRecord);

	return goalRecord && !goalRecord->GetNoRollback();
}

bool Goal::Commited_Agents_Need_Orders() const
{
	bool needOrders = false;

	Plan_List::const_iterator match_iter;

	for
	(
	      match_iter  = m_matches.begin();
	      match_iter != m_matches.end();
	    ++match_iter
	)
	{
		needOrders |= match_iter->Commited_Agents_Need_Orders();
	}

	return needOrders;
}

void Goal::Rollback_All_Agents()
{
#if defined(_DEBUG)
	Squad_Strength strength  = m_current_attacking_strength;
	Squad_Strength strength1 = Compute_Current_Strength();

	Squad_Strength strength2 = strength1;
	strength2 -= strength;
	Assert(strength2.NothingNeeded());

	Squad_Strength strength3 = strength;
	strength3 -= strength1;
	Assert(strength3.NothingNeeded());
#endif

	for
	(
	    Plan_List::iterator
	      match_iter  = m_matches.begin();
	      match_iter != m_matches.end();
	    ++match_iter
	)
	{
		match_iter->Rollback_All_Agents();
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
		else if(Is_Satisfied() || Get_Totally_Complete())
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, Get_Player_Index(), Get_Goal_Type(), -1,
				("\t\tNO AGENTS COMMITTED:           (goal: %x squad: %x, id: 0%x)\n", this, match_iter->Get_Squad(), match_iter->Get_Squad()->Get_Agent()->Get_Army().m_id));
			break;
		}
		else
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, Get_Player_Index(), Get_Goal_Type(), -1,
				("\t\tAGENTS CAN BE COMMITTED:       (goal: %x squad: %x, id: 0%x)\n", this, match_iter->Get_Squad(), match_iter->Get_Squad()->Get_Agent()->Get_Army().m_id));

			match_iter->Commit_Agent();
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
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(match_iter->Get_Matching_Value() <= Goal::BAD_UTILITY)
		{
			break;
		}
		else if(!Needs_Transporter() || Get_Totally_Complete())
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, Get_Player_Index(), Get_Goal_Type(), -1,
				("\t\tNO TRANSPORT AGENTS COMMITTED: (goal: %x squad: %x, id: 0%x)\n", this, match_iter->Get_Squad(), match_iter->Get_Squad()->Get_Agent()->Get_Army().m_id));
			break;
		}
		else if(match_iter->Needs_Cargo())
		{
			AI_DPRINTF(k_DBG_SCHEDULER_DETAIL, Get_Player_Index(), Get_Goal_Type(), -1,
				("\t\tTRANSPORT AGENTS COMMITTED:    (goal: %x squad: %x, id: 0%x)\n", this, match_iter->Get_Squad(), match_iter->Get_Squad()->Get_Agent()->Get_Army().m_id));

			match_iter->Commit_Transport_Agent();
		}
	}
}

void Goal::Remove_Matches()
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		match_iter->Rollback_All_Agents();

		Squad_ptr squad_ptr = match_iter->Get_Squad();

		Assert(squad_ptr);
		if(squad_ptr)
#if defined(USE_GOAL_REF)
			squad_ptr->Remove_Goal_Reference(this);
#else
			squad_ptr->Remove_Match_Reference(match_iter);
#endif
	}

	m_matches.clear();
}

#if defined(USE_GOAL_REF)
void Goal::Remove_Match(const Squad_ptr & squad)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(squad == match_iter->Get_Squad())
		{
			match_iter->Rollback_All_Agents();
			m_matches.erase(match_iter);

			return;
		}
	}
}
#else
void Goal::Remove_Match(Plan_List::iterator match)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(match == match_iter)
		{
			match_iter->Rollback_All_Agents();
			m_matches.erase(match_iter);

			return;
		}
	}
}
#endif

bool Goal::Has_Squad_And_Set_Needs_Cargo(Squad* squad)
{
	for
	(
	    Plan_List::iterator match_iter  = m_matches.begin();
	                        match_iter != m_matches.end();
	                      ++match_iter
	)
	{
		if(squad == match_iter->Get_Squad())
		{
			match_iter->Set_Needs_Cargo(true);
			return true;
		}
	}

	return false;
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
		CTPAgent* ctpAgent = static_cast<CTPAgent*>(*agent_iter);
		m_current_attacking_strength += ctpAgent->Get_Squad_Strength();
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
		CTPAgent* ctpAgent = static_cast<CTPAgent*>(*agent_iter);
		strength += ctpAgent->Get_Squad_Strength();
	}

	return strength;
}

void Goal::Rollback_Emptied_Transporters()
{
	for
	(
	    Plan_List::iterator
	      match_iter  = m_matches.begin();
	      match_iter != m_matches.end();
	    ++match_iter
	)
	{
		match_iter->Rollback_Emptied_Transporters();
	}
}

void Goal::Sort_Matches()
{
	m_matches.sort(greater< Plan >());
	m_needs_sorting = false;
}

void Goal::Sort_Matches_If_Necessary()
{
	if(m_needs_sorting)
	{
		Sort_Matches();
	}
}
