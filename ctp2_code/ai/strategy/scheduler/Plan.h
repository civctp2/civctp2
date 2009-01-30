//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : declarations for the Plan class
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
// - Marked MS version specific code.
// - Add the CanMatchesBeReevaluated (implemented for the moment always at true)
//   so, the matches are rollbacked and changed every turn... - Calvitix
// - Standardised <list> import.
// - Cleaned up the code somewhat (removed sint16 conversions, removed Init)
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Simplified the class design: m_matches replaced, since it just contained
//   pne agent pointer. (27-Aug-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------
 
#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef PLAN_H__
#define PLAN_H__ 1

#include <list>

class Plan;

#include "scheduler_types.h"

class Plan
{
private:
	Plan();
public:
	Plan(Agent_ptr agent, Goal_ptr goal, bool needsCargo);
	Plan(const Plan &plan);

	~Plan();

	Plan& operator= (const Plan &plan);

	bool operator<  (const Plan &plan) const;
	bool operator>  (const Plan &plan) const;
	bool operator== (const Plan &plan) const;
	bool operator!= (const Plan &plan) const;


	GOAL_TYPE Get_Goal_Type() const;


	bool Plan_Is_Needed_And_Valid() const { return m_the_goal && m_the_agent; };

	Utility Compute_Matching_Value();
	Utility Get_Matching_Value() const { return m_matching_value; };

	Goal_ptr   Get_Goal() const { return m_the_goal;  };
	Agent_ptr Get_Agent() const { return m_the_agent; };


	void Commit_Agent();
	void Commit_Agent_Common();
	void Commit_Transport_Agent();


	GOAL_RESULT Execute_Task();


	void Rollback_All_Agents();
	void Rollback_Emptied_Transporters();


	bool Commited_Agents_Need_Orders() const;
	bool All_Unused_Or_Used_By_This() const;

	///
	/// Check if the agents can be rollbacked to eventually receive other priority 
	/// (for example : not if they are too close to their goals and grouping)
	///
	bool CanMatchesBeReevaluated() const;

	bool Agent_Committed(const Agent_ptr agent_ptr) const { return agent_ptr == m_the_agent && Agent_Committed(); };
	bool Agent_Committed() const;
	sint32 Get_Free_Transport_Capacity() const;

	bool Has_Cargo      ()                const;

	void Log_Debug_Info (const int & log) const;
	bool Needs_Cargo    ()                const { return m_needs_cargo && !Has_Cargo();         };
	void Set_Needs_Cargo(const bool needsCargo) { m_needs_cargo = needsCargo;                   };

	bool Can_Add_To_Goal() const;

protected:

	Utility          m_matching_value;
	Agent_ptr        m_the_agent;
	Goal_ptr         m_the_goal;
	bool             m_needs_cargo;
};


#endif // PLAN_H__
