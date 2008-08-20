//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Squad
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Replaced the Agent_List by a Agent_ptr, since the Agent_List just
//   contained one pointer. (18-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Squad.h"

#include "agent.h"
#include "ctpagent.h"
#include "ctpaidebug.h"
#include "Plan.h"
#include "ArmyData.h"

#ifdef _DEBUG
#include "Scheduler.h"
#endif

Squad::Squad()
{
	Init();
}

Squad::Squad(const Squad &squad)
{
	*this = squad;
}

Squad::~Squad()
{
	delete m_agent;
}

void Squad::Init()
{
	m_is_committed  = false;
	m_squad_changed = true;
	m_squad_class   = SQUAD_CLASS_DEFAULT;

	m_agent = NULL;

//	m_goal_references.resize(0);
	m_match_references.resize(0);
}

Squad & Squad::operator= (const Squad &squad)
{
	m_is_committed     = squad.m_is_committed;
	m_squad_class      = squad.m_squad_class;
	m_agent            = squad.m_agent;
	m_squad_changed    = squad.m_squad_changed;
	m_match_references = squad.m_match_references;
//	m_goal_references  = squad.m_goal_references;

	return *this;
}

bool Squad::ContainsArmyIn(const Squad_ptr squad) const
{
	return static_cast<CTPAgent_ptr>(m_agent)->Get_Army() == static_cast<CTPAgent_ptr>(squad->m_agent)->Get_Army();
}

/// Remove dead (or defected) agents from the squad
/// \return	Number of removed agents
void Squad::Remove_Dead_Agents()
{
	if(m_agent->Get_Is_Dead())
	{
		Remove_Agent(); // Move out
	}
}

/// Get the number of agents in the squad
/// \return	Number of agents
/// \remarks The validity (dead/defected?) of the agents is not checked.
size_t Squad::Get_Num_Agents() const
{
	return m_agent != NULL;
}

/// Remove an agent from the squad
/// \param  agent_iter      Position of the agent to remove
/// \param  dealloc_agent   Delete the agent data when removing
/// \return Position after the removed agent
/// \remarks Will update the match references and mark the squad as changed.
void Squad::Remove_Agent()
{
	for
	(
	    std::list<Plan_List::iterator>::iterator
	        plan_ref_iter  = m_match_references.begin();
	        plan_ref_iter != m_match_references.end();
	      ++plan_ref_iter
//	    Goal_Ref_List::iterator
//	        goal_ref_iter  = m_goal_references.begin();
//	        goal_ref_iter != m_goal_references.end();
//	      ++goal_ref_iter
	)
	{
//		Goal_ptr goal_ptr = static_cast<Goal_ptr>(*goal_ref_iter);
//		removed_agents += goal_ptr->Remove_Match(this);

		bool found = (*plan_ref_iter)->Remove_Agent_Reference(m_agent);
		Assert(found);
	}

#ifdef _DEBUG_SCHEDULER
	CTPAgent_ptr ctp_agent = (CTPAgent_ptr) the_agent;
	PLAYER_INDEX playerId = ctp_agent->Get_Player_Number();

	Scheduler::GetScheduler(playerId).Validate();
#endif // _DEBUG_SCHEDULER

	delete m_agent;
	m_agent = NULL;

#ifdef _DEBUG_SCHEDULER
	Scheduler::GetScheduler(playerId).Validate();
#endif // _DEBUG_SCHEDULER

	m_squad_changed = true;
}

SQUAD_CLASS Squad::Compute_Squad_Class()
{
	m_squad_class = m_agent->Compute_Squad_Class();

	return m_squad_class;
}

SQUAD_CLASS Squad::Get_Squad_Class() const
{
	return m_squad_class;
}

void Squad::Add_Agent(Agent_ptr the_agent)
{
	Assert(m_agent == NULL);

	m_agent = the_agent;
	m_squad_changed = true;
}

void Squad::Compute_Strength(Squad_Strength & strength)
{
	strength = m_agent->Compute_Squad_Strength();
}

void Squad::Get_Strength(Squad_Strength & strength)
{
	strength = m_agent->Get_Squad_Strength();
}

void Squad::Add_Match_Reference(const Plan_List::iterator &plan_iter)
//void Squad::Add_Goal_Reference(const Goal_ptr goal)
{
	m_match_references.push_back(plan_iter);
//	m_goal_references.push_back(goal);
}

void Squad::Remove_Match_Reference(const Plan_List::iterator &plan_iter)
//void Squad::Remove_Goal_Reference(const Goal_ptr goal)
{
	m_match_references.remove(plan_iter);
	//	m_goal_references.remove(goal);
}

std::list<Plan_List::iterator> & Squad::Get_Match_References()
{
	return m_match_references;
}

void Squad::Set_Can_Be_Executed(const bool & can_be_executed)
{
	m_agent->Set_Can_Be_Executed(can_be_executed);
}

void Squad::Set_Needs_Transporter(const bool needs_transporter)
{
	m_agent->Set_Needs_Transporter(needs_transporter);
}

sint32 Squad::DisbandObsoleteArmies() const
{
	sint32 count = static_cast<CTPAgent_ptr>(m_agent)->DisbandObsoleteUnits();

	if (count > 0)
	{
		AI_DPRINTF(k_DBG_SCHEDULER, static_cast<CTPAgent_ptr>(m_agent)->Get_Army()->GetOwner(), -1, -1, ("*** Disbanding Army:\n"));
		static_cast<CTPAgent_ptr>(m_agent)->Log_Debug_Info(k_DBG_SCHEDULER, static_cast<CTPAgent_ptr>(m_agent)->Get_Goal());
	}

	return count;
}

void Squad::Log_Debug_Info(const int & log, const Goal * const goal) const
{
	m_agent->Log_Debug_Info(log, goal);
}
