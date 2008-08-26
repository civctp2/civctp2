//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __AGENT_H__
#define __AGENT_H__ 1

class Agent;

#include "c3.h"

#include "scheduler_types.h"
#include "squad_Strength.h"

class Agent
{
protected:
	Agent();
public:
	Agent(const Agent & an_Original);
	virtual ~Agent();

	virtual Agent& operator= (const Agent & an_Original);

	sint16 Get_Type() const;

	void Set_Type(const sint16 & type);

	SQUAD_CLASS Get_Squad_Class() const;

	void     Set_Goal(Goal_ptr goal){ m_goal = goal; };
	Goal_ptr Get_Goal(){ return m_goal; };
	bool     Has_Goal(){ return m_goal != NULL; };

	virtual bool Get_Is_Dead() const = 0;

	void Set_Squad_Class(const SQUAD_CLASS & squad_class);

	virtual SQUAD_CLASS Compute_Squad_Class() { return m_squad_class; };

	virtual const Squad_Strength & Get_Squad_Strength() const;

	virtual const Squad_Strength & Compute_Squad_Strength() { return m_squad_strength; };

	virtual void Set_Can_Be_Executed(const bool &can_be_executed);

	bool Get_Can_Be_Executed() const;

	void Set_Detached(const bool detached);

	bool Get_Detached() const;

	virtual void Log_Debug_Info(const int & log, const Goal * const goal) const = 0;

	void Set_Needs_Transporter(const bool needs_transporter) { m_needs_transporter = needs_transporter; };
	bool Get_Needs_Transporter() const { return m_needs_transporter; };

protected:

	SQUAD_CLASS    m_squad_class;
	sint16         m_agent_type;
	Goal_ptr       m_goal;
	Squad_Strength m_squad_strength;
	bool           m_can_be_executed;
	bool           m_detached;
	bool           m_needs_transporter;
};

#endif // __AGENT_H__
