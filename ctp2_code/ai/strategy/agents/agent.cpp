













#include "c3.h"

#include "Agent.h"


#include "DebugAssert.h"






Agent::Agent()
{
	Init();
}


Agent::Agent(const Agent &agent)
{
	*this = agent;
}

Agent::~Agent()
{
	
}



void Agent::Init()
{
	
	m_agent_type = -1; 

	
	m_is_used = false;
	m_can_be_executed = true;
	m_detached = false;
}


Agent & Agent::operator= (const Agent &agent)
{
	
	m_agent_type = agent.m_agent_type;

	
	m_is_used = agent.m_is_used;

	
	m_squad_strength = agent.m_squad_strength;

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
