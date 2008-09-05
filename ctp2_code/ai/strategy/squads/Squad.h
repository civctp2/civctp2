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
// - Replaced the Agent_List by a Agent_ptr, since the Agent_List just
//   contained one pointer. (18-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef SQUAD_H__
#define SQUAD_H__

#include <list>

class Squad;

#include "scheduler_types.h"
#include "Goal.h"
#include "c3debugstl.h"
#include "Army.h"
#include "CTPAgent.h"

class Squad
{
public:

#if defined(_DEBUG)
	typedef std::list<Goal_ptr, dbgallocator<Goal_ptr> > Goal_Ref_List;
#else
	typedef std::list<Goal_ptr> Goal_Ref_List;
#endif
private:
	Squad();
public:
	explicit Squad(const Army & army);
	Squad(const Squad &squad);
	virtual ~Squad ();

	virtual Squad& operator= (const Squad &squad);

	virtual bool ContainsArmyIn(const Squad_ptr squad) const;

	void Remove_Dead_Agents();

	size_t Get_Num_Agents() const; // Replace

	CTPAgent_ptr Get_Agent() const { return m_agent; };

	SQUAD_CLASS Compute_Squad_Class();

	SQUAD_CLASS Get_Squad_Class() const;

#if defined(USE_GOAL_REF)
	void Add_Goal_Reference(const Goal_ptr goal);
	void Remove_Goal_Reference(const Goal_ptr goal);
#else
	void Add_Match_Reference(const Plan_List::iterator &plan_iter);
	void Remove_Match_Reference(const Plan_List::iterator &plan_iter);
#endif

	void Remove_Matches();

	void Set_Can_Be_Executed(const bool & can_be_executed);

	sint32 DisbandObsoleteArmies() const;

	void Log_Debug_Info(const int & log, const Goal * const goal) const;

	void Get_Strength(Squad_Strength & strength);
	void Compute_Strength(Squad_Strength & strength);
	void Set_Needs_Transporter(const bool needs_transporter);

	bool Is_Dead() const;

protected:

	CTPAgent_ptr                    m_agent;
#if defined(USE_GOAL_REF)
	Goal_Ref_List                   m_goal_references;
#else
	std::list<Plan_List::iterator>  m_match_references;
#endif
};

#endif // SQUAD_H__
