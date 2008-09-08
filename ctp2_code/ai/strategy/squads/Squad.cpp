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

#include "CTPAgent.h"
#include "ctpagent.h"
#include "ctpaidebug.h"
#include "Plan.h"
#include "ArmyData.h"

#ifdef _DEBUG
#include "Scheduler.h"
#endif

Squad::Squad()
:   m_agent          (NULL)
{
	m_goal_references.resize(0);
}

Squad::Squad(const Army & army)
:
    m_agent          (new CTPAgent(army))
{
	m_goal_references.resize(0);
}

Squad::Squad(const Squad &squad)
{
//	m_agent = squad.m_agent;

//	m_goal_references.copy();

	*this = squad;
}

Squad::~Squad()
{
	Remove_Matches();
	delete m_agent;
}

Squad & Squad::operator= (const Squad &squad)
{
	m_agent            = squad.m_agent;
	m_goal_references  = squad.m_goal_references;

	return *this;
}

bool Squad::ContainsArmyIn(const Squad_ptr squad) const
{
	return m_agent->Get_Army() == squad->m_agent->Get_Army();
}

bool Squad::Is_Dead() const
{
	return m_agent->Get_Is_Dead();
}

/// Get the number of agents in the squad
/// \return	Number of agents
/// \remarks The validity (dead/defected?) of the agents is not checked.
size_t Squad::Get_Num_Agents() const
{
	return m_agent != NULL;
}

/// Remove all agents from the squad
void Squad::Remove_Matches()
{
	for
	   (
	    Goal_Ref_List::iterator
	        goal_ref_iter  = m_goal_references.begin();
	        goal_ref_iter != m_goal_references.end();
	      ++goal_ref_iter
	   )
	{
		(*goal_ref_iter)->Remove_Match(this);
	}

	m_goal_references.clear();
}

SQUAD_CLASS Squad::Compute_Squad_Class()
{
	return m_agent->Compute_Squad_Class();
}

SQUAD_CLASS Squad::Get_Squad_Class() const
{
	return m_agent->Get_Squad_Class();
}

void Squad::Compute_Strength(Squad_Strength & strength)
{
	strength = m_agent->Compute_Squad_Strength();
}

void Squad::Get_Strength(Squad_Strength & strength)
{
	strength = m_agent->Get_Squad_Strength();
}

void Squad::Add_Goal_Reference(const Goal_ptr goal)
{
	m_goal_references.push_back(goal);
}

void Squad::Remove_Goal_Reference(const Goal_ptr goal)
{
	m_goal_references.remove(goal);
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
	sint32 count = m_agent->DisbandObsoleteUnits();

	if (count > 0)
	{
		AI_DPRINTF(k_DBG_SCHEDULER, m_agent->Get_Army()->GetOwner(), -1, -1, ("*** Disbanding Army:\n"));
		m_agent->Log_Debug_Info(k_DBG_SCHEDULER, m_agent->Get_Goal());
	}

	return count;
}

void Squad::Log_Debug_Info(const int & log, const Goal * const goal) const
{
	m_agent->Log_Debug_Info(log, goal);
}
