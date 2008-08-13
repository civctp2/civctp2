//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Goal handling
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
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "agent.h"

Agent::Agent()
:
    m_squad_class       (SQUAD_CLASS_DEFAULT),
    m_agent_type        (-1),
    m_goal              (NULL),
    m_squad_strength    (0),
    m_can_be_executed   (true),
    m_detached          (false),
    m_needs_transporter (false)
{
}

Agent::Agent(const Agent & an_Original)
:
    m_squad_class       (an_Original.m_squad_class),
    m_agent_type        (an_Original.m_agent_type), 
    m_goal              (an_Original.m_goal),
    m_squad_strength    (an_Original.m_squad_strength),
    m_can_be_executed   (an_Original.m_can_be_executed),
    m_detached          (an_Original.m_detached),
    m_needs_transporter (an_Original.m_needs_transporter)
{
}

Agent::~Agent()
{
    // No action: nothing to delete
}

Agent & Agent::operator = (const Agent & an_Original)
{
	if (this != &an_Original)
	{
		m_squad_class       = an_Original.m_squad_class;
		m_agent_type        = an_Original.m_agent_type;
		m_goal              = an_Original.m_goal;
		m_squad_strength    = an_Original.m_squad_strength;
		m_can_be_executed   = an_Original.m_can_be_executed;
		m_detached          = an_Original.m_detached;
		m_needs_transporter = an_Original.m_needs_transporter;
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
