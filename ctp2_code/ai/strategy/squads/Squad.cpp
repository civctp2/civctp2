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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Squad.h"

#include "agent.h"
#include "ctpagent.h"
#include "ctpaidebug.h"
#include "Plan.h"

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

Squad::~Squad ()
{
	Agent_List::iterator agent_iter = m_my_agents.begin();
	while (agent_iter != m_my_agents.end())
	{
		Agent_ptr agent_ptr = (*agent_iter);
		agent_iter = m_my_agents.erase(agent_iter);
		
		delete agent_ptr;
	}
}

void Squad::Init()
{
	m_is_committed  = false;
	m_squad_changed = true;
	m_squad_class   = SQUAD_CLASS_DEFAULT;

	m_my_agents.resize(0);

	m_match_references.resize(0);
}

Squad & Squad::operator= (const Squad &squad)
{
	m_is_committed     = squad.m_is_committed;
	m_squad_class      = squad.m_squad_class;
	m_my_agents        = squad.m_my_agents;
	m_squad_changed    = squad.m_squad_changed;
	m_match_references = squad.m_match_references;

	return *this;
}

bool Squad::ContainsArmyIn(const Squad &squad) const
{
	bool found = false;

	for
	(
	    Agent_List::const_iterator agent1_iter  = squad.Get_Agent_List().begin();
	                               agent1_iter != squad.Get_Agent_List().end() && !found;
	                             ++agent1_iter
	)
	{
		const CTPAgent_ptr agent1_ptr = (CTPAgent_ptr) (*agent1_iter);

		for
		(
		    Agent_List::const_iterator agent2_iter = m_my_agents.begin();
		    agent2_iter != m_my_agents.end() && !found;
		    ++agent2_iter
		)
		{
			const CTPAgent_ptr agent2_ptr = (CTPAgent_ptr) (*agent2_iter);
			found = 
			    (agent2_ptr->Get_Army() == agent1_ptr->Get_Army());
		}
	}

	return found;
}

/// Remove dead (or defected) agents from the squad
/// \return	Number of removed agents
size_t Squad::Remove_Dead_Agents()
{
	size_t agents_found = 0;
	
	for
	(
	    Agent_List::const_iterator  agent_iter  = m_my_agents.begin();
	                                agent_iter != m_my_agents.end();
	                             // agent_iter updated in loop
	)
	{
		CTPAgent_ptr agent_ptr = (CTPAgent_ptr) *agent_iter;

		if (agent_ptr->Get_Is_Dead())
		{
			++agents_found;
			agent_iter = Remove_Agent(agent_iter);
		}
		else
		{
			++agent_iter;
		}
	}

	return agents_found;
}

/// Get the number of agents in the squad
/// \return	Number of agents
/// \remarks The validity (dead/defected?) of the agents is not checked.
size_t Squad::Get_Num_Agents() const
{
	return m_my_agents.size();
}

/// Remove an agent from the squad
/// \param  agent_iter      Position of the agent to remove
/// \param  dealloc_agent   Delete the agent data when removing
/// \return Position after the removed agent
/// \remarks Will update the match references and mark the squad as changed.
Agent_List::const_iterator Squad::Remove_Agent
(
    const Agent_List::const_iterator &  agent_iter,
    const bool &                        dealloc_agent   // = true
)
{
	Agent_ptr the_agent = *agent_iter;
	
	for
	(
	    std::list<Plan_List::iterator>::iterator
	        plan_ref_iter  = m_match_references.begin();
	        plan_ref_iter != m_match_references.end();
	      ++plan_ref_iter
	)
	{
		bool found = (*plan_ref_iter)->Remove_Agent_Reference(agent_iter);
		Assert(found);
	}

	Agent_List::iterator next_agent_iter = 
	    std::find(m_my_agents.begin(), m_my_agents.end(), the_agent);
	Assert(next_agent_iter != m_my_agents.end());

	next_agent_iter = m_my_agents.erase(next_agent_iter);

	if (dealloc_agent)
	{
#ifdef _DEBUG_SCHEDULER
		CTPAgent_ptr ctp_agent = (CTPAgent_ptr) the_agent;
		PLAYER_INDEX playerId = ctp_agent->Get_Player_Number();
		
		Scheduler::GetScheduler(playerId).Validate();
#endif // _DEBUG_SCHEDULER

		delete the_agent;

#ifdef _DEBUG_SCHEDULER
		Scheduler::GetScheduler(playerId).Validate();
#endif // _DEBUG_SCHEDULER
	}

	m_squad_changed = true;

	return next_agent_iter;
}

SQUAD_CLASS Squad::Compute_Squad_Class()
{
	m_squad_class = SQUAD_CLASS_DEFAULT;
	
	for
	(
	    Agent_List::iterator agent_iter  = m_my_agents.begin();
	                         agent_iter != m_my_agents.end();
	                       ++agent_iter
	)
	{
		m_squad_class |= (*agent_iter)->Compute_Squad_Class();
	}

	return m_squad_class;
}

SQUAD_CLASS Squad::Get_Squad_Class() const
{
	return m_squad_class;
}

void Squad::Add_Agent( Agent_ptr the_agent )
{
	m_my_agents.push_back(the_agent);
	m_squad_changed = true;
}

void Squad::Remove_Agent( Agent_ptr the_agent )
{
	for
	(
	    Agent_List::iterator agent_iter  = m_my_agents.begin();
	                         agent_iter != m_my_agents.end();
	                       ++agent_iter
	)
	{
		if (the_agent == (*agent_iter))
		{
			Remove_Agent(agent_iter);
			return;
		}
	}
}

void Squad::Add_Match_Reference(const Plan_List::iterator &plan_iter)
{
	m_match_references.push_back(plan_iter);
}

void Squad::Remove_Match_Reference(const Plan_List::iterator &plan_iter)
{
	m_match_references.remove(plan_iter);
}

std::list<Plan_List::iterator> & Squad::Get_Match_References()
{
	return m_match_references;
}

Agent_List & Squad::Get_Agent_List()
{
	return m_my_agents;
}
const Agent_List & Squad::Get_Agent_List() const
{
	return m_my_agents;
}

void Squad::Set_Can_Be_Executed(const bool & can_be_executed)
{
	for
	(
	    Agent_List::iterator agent_iter  = m_my_agents.begin();
	                         agent_iter != m_my_agents.end();
	                       ++agent_iter
	)
	{
		(*agent_iter)->Set_Can_Be_Executed(can_be_executed);
	}
}

sint32 Squad::DisbandObsoleteArmies() const
{
	sint32 total_count = 0;
	
	for
	(
	    Agent_List::const_iterator agent_iter  = m_my_agents.begin();
	                               agent_iter != m_my_agents.end();
	                             ++agent_iter
	)
	{
		CTPAgent * ctpagent_ptr = (CTPAgent *) (*agent_iter);
		sint32 count = ctpagent_ptr->DisbandObsoleteUnits();
		
		if (count > 0)
		{
			DPRINTF(k_DBG_SCHEDULER, ("*** Disbanding Army:"));
			ctpagent_ptr->Log_Debug_Info(k_DBG_SCHEDULER);
		}

		total_count += count;
	}

	return total_count;
}

void Squad::Log_Debug_Info(const int & log) const
{
	for
	(
	    Agent_List::const_iterator agent_iter  = m_my_agents.begin();
	                               agent_iter != m_my_agents.end();
	                             ++agent_iter
	)
	{
		CTPAgent * ctpagent_ptr = (CTPAgent *) (*agent_iter);
		ctpagent_ptr->Log_Debug_Info(log);
	}
}
