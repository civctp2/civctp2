













#include "c3.h"
#include "agent.h"








Agent::Agent()
:	
	m_squad_class       (SQUAD_CLASS_DEFAULT),
    m_agent_type        (-1), 
	m_is_used           (false),
	m_squad_strength    (0),
    m_can_be_executed   (true),
    m_detached          (false)
{
}


Agent::Agent(Agent const & a_Original)
:	
    m_squad_class       (a_Original.m_squad_class),
    m_agent_type        (a_Original.m_agent_type), 
	m_is_used           (a_Original.m_is_used),
	m_squad_strength    (a_Original.m_squad_strength),
    m_can_be_executed   (a_Original.m_can_be_executed),
    m_detached          (a_Original.m_detached)
{
}

Agent::~Agent()
{
    // No action: nothing to delete
}


Agent & Agent::operator = (Agent const & a_Original)
{
    if (this != &a_Original)
    {
        m_squad_class       = a_Original.m_squad_class;
	    m_agent_type        = a_Original.m_agent_type;
    	m_is_used           = a_Original.m_is_used;
	    m_squad_strength    = a_Original.m_squad_strength;
        m_can_be_executed   = a_Original.m_can_be_executed;
        m_detached          = a_Original.m_detached;
    }

    return *this;
}


sint16 Agent::Get_Type() const
{
	return m_agent_type;
} 


void Agent::Set_Type(const sint16 & type)
{
	m_agent_type = type;
}


bool Agent::Get_Is_Used() const
{
	return m_is_used;
}


void Agent::Set_Is_Used(const bool & is_used)
{
	m_is_used = is_used;
}


bool Agent::Get_Is_Dead() const
{
    return false;
}


void Agent::Set_Squad_Class(const SQUAD_CLASS & squad_class)
{
    m_squad_class = squad_class;
}


SQUAD_CLASS Agent::Get_Squad_Class() const
{
    return m_squad_class;
}


const Squad_Strength & Agent::Get_Squad_Strength() const
{
    return m_squad_strength;
}


void Agent::Set_Can_Be_Executed(const bool &can_be_executed)
{
	m_can_be_executed = can_be_executed;
}


bool Agent::Get_Can_Be_Executed() const
{
	return m_can_be_executed && !m_detached;
}


void Agent::Set_Detached(const bool detached)
{
	m_detached = detached;
}


bool Agent::Get_Detached() const
{
	return m_detached;
}
