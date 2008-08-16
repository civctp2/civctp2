//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef SQUAD_H__
#define SQUAD_H__

#include <list>

class Squad;

#include "Plan.h"
#include "scheduler_types.h"

class Squad
{
public:
	Squad();
	Squad(const Squad &squad);
	virtual ~Squad ();

	virtual void Init();

	virtual Squad& operator= (const Squad &squad);

	virtual bool ContainsArmyIn(const Squad &squad) const;

	size_t Remove_Dead_Agents();
	
	size_t Get_Num_Agents() const; 

	Agent_ptr Get_Agent(const Agent_List::iterator & agent_iter) const;

	Agent_List::const_iterator Remove_Agent(const Agent_List::const_iterator & agent_iter,
	                                        const bool & dealloc_agent = true);

	SQUAD_CLASS Compute_Squad_Class();

	SQUAD_CLASS Get_Squad_Class() const;

	void Add_Agent( Agent_ptr the_agent );

	void Add_Match_Reference(const Plan_List::iterator &plan_iter);

	void Remove_Match_Reference(const Plan_List::iterator &plan_iter);

	std::list<Plan_List::iterator> & Get_Match_References();

	Agent_List &        Get_Agent_List();
	const Agent_List &  Get_Agent_List() const;

	void Set_Can_Be_Executed(const bool & can_be_executed);

	sint32 DisbandObsoleteArmies() const;

	void Log_Debug_Info(const int & log, const Goal * const goal) const;

	void Get_Strength(Squad_Strength & strength);
	void Compute_Strength(Squad_Strength & strength);
	void Set_Needs_Transporter(const bool needs_transporter);

protected:

	bool                            m_is_committed;
	SQUAD_CLASS                     m_squad_class;
	Agent_List                      m_my_agents;
	bool                            m_squad_changed;    // Not used
	std::list<Plan_List::iterator>  m_match_references;

private:

};

#endif // SQUAD_H__
