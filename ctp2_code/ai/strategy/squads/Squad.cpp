











#pragma warning(disable: 4786)

#include "c3.h"

#include "Squad.h"
#include "Agent.h"
#include "CTPAgent.h"
#include "Plan.h"

#ifdef _DEBUG
#include "Scheduler.h"
#endif

using namespace std;


#include "DebugAssert.h"
#include "CtpAiDebug.h"













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
	Agent_List::iterator agent_iter;
	Agent_ptr agent_ptr;

    
    agent_iter = m_my_agents.begin();
    while (agent_iter != m_my_agents.end()) {
         agent_ptr = (*agent_iter);
         agent_iter = m_my_agents.erase(agent_iter);
             
         
         delete agent_ptr;
   } 
}








void Squad::Init()
{
	
	m_is_committed = false;	

	
	m_squad_class = SQUAD_CLASS_DEFAULT;

	
	m_my_agents.resize(0);

	
	m_squad_changed = true;

	
	m_match_references.resize(0);
}












Squad & Squad::operator= (const Squad &squad)
{
	
	m_is_committed = squad.m_is_committed;

	
	m_squad_class = squad.m_squad_class;

	
	m_my_agents = squad.m_my_agents;

	
	m_squad_changed = squad.m_squad_changed;

	
	m_match_references = squad.m_match_references;

    return *this;
}






bool Squad::ContainsArmyIn(const Squad &squad) const
{
	Agent_List::const_iterator agent1_iter;
	Agent_List::const_iterator agent2_iter;
	bool found = false;

	
    agent1_iter = squad.Get_Agent_List().begin();
	while (agent1_iter != squad.Get_Agent_List().end() && !found) 
	{
		const CTPAgent_ptr agent1_ptr = (const CTPAgent_ptr) (*agent1_iter);
	    
		agent2_iter = m_my_agents.begin();
		while (agent2_iter != m_my_agents.end() && !found) 
		{
			const CTPAgent_ptr agent2_ptr = (const CTPAgent_ptr) (*agent2_iter);
			if (agent2_ptr->Get_Army().m_id == agent1_ptr->Get_Army().m_id)
			{
				found = true;
				break;
			}
			agent2_iter++;
		}
		agent1_iter++;
	}
	return found;
}








sint16 Squad::Remove_Dead_Agents()
{
	Agent_List::const_iterator agent_iter;
	Agent_ptr agent_ptr;
    sint16 agents_found = 0;

	
    agent_iter = m_my_agents.begin();
	while (agent_iter != m_my_agents.end()) {
		agent_ptr = (*agent_iter);

        
        
        if (agent_ptr->Get_Is_Dead()) {
			
			agent_iter = Remove_Agent(agent_iter);
			agents_found++;
		}
        else
            agent_iter++;
	}

    return agents_found;
}

















































sint16 Squad::Get_Num_Agents() const
{
	return m_my_agents.size();
} 



















































Agent_List::const_iterator Squad::Remove_Agent(const Agent_List::const_iterator & agent_iter,
                                         const bool & dealloc_agent)
{
    
    Agent_ptr the_agent = (*agent_iter);
	
    
    list<Plan_List::iterator>::iterator plan_ref_iter;
    for (plan_ref_iter = m_match_references.begin();
	plan_ref_iter != m_match_references.end();
	plan_ref_iter++) {
		bool found;
		
		
		
		found = (*plan_ref_iter)->Remove_Agent_Reference(agent_iter);
		Assert(found);

	} 
    
    
	Agent_List::iterator next_agent_iter;
	for (next_agent_iter = m_my_agents.begin();
	next_agent_iter != m_my_agents.end();
	next_agent_iter++)
	{
		if (*next_agent_iter == *agent_iter)
			break;
	}

	
	Assert(next_agent_iter != m_my_agents.end());
	
	next_agent_iter = m_my_agents.erase(next_agent_iter);
		 
    
    if (dealloc_agent) {

#ifdef _DEBUG_SCHEDULER
		
		CTPAgent_ptr ctp_agent = (CTPAgent_ptr) the_agent;
		PLAYER_INDEX playerId = ctp_agent->Get_Player_Number();

		
		Scheduler::GetScheduler(playerId).Validate();
#endif _DEBUG_SCHEDULER

        
        delete the_agent;

#ifdef _DEBUG_SCHEDULER
		
		Scheduler::GetScheduler(playerId).Validate();
#endif _DEBUG_SCHEDULER

    }
    
    
    m_squad_changed = true;

    return next_agent_iter;
}












SQUAD_CLASS Squad::Compute_Squad_Class()
{
	
	Agent_List::iterator agent_iter;
	

	
	m_squad_class = SQUAD_CLASS_DEFAULT;

	for (agent_iter = m_my_agents.begin();
		 agent_iter != m_my_agents.end();
		 agent_iter++)
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
	Agent_List::iterator agent_iter;

	
	for (agent_iter = m_my_agents.begin();
		 agent_iter != m_my_agents.end();
		 agent_iter++) {
        if (the_agent == (*agent_iter)) {
            
            Remove_Agent(agent_iter);
            break;
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












list<Plan_List::iterator> & Squad::Get_Match_References()
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
    Agent_List::iterator agent_iter;
	
	for (agent_iter = m_my_agents.begin();
		 agent_iter != m_my_agents.end();
		 agent_iter++) 
		 {
            (*agent_iter)->Set_Can_Be_Executed(can_be_executed);
		 }
}


sint32 Squad::DisbandObsoleteArmies() const
{
    Agent_List::const_iterator agent_iter;
	sint32 total_count = 0;
	sint32 count;
	CTPAgent *ctpagent_ptr;
	
	for (agent_iter = m_my_agents.begin();
		 agent_iter != m_my_agents.end();
		 agent_iter++) 
		{
			ctpagent_ptr = (CTPAgent *) (*agent_iter);
            count = ctpagent_ptr->DisbandObsoleteUnits();
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
    Agent_List::const_iterator agent_iter;
	CTPAgent *ctpagent_ptr;
	
	for (agent_iter = m_my_agents.begin();
		agent_iter != m_my_agents.end();
		agent_iter++) 
	{
		ctpagent_ptr = (CTPAgent *) (*agent_iter);
		ctpagent_ptr->Log_Debug_Info(log);
    } 
}




