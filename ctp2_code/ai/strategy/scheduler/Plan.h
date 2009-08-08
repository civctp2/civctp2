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
	Plan(Agent_ptr agent, bool needsCargo);
	Plan(const Plan &plan);

	~Plan();

	Plan& operator= (const Plan &plan);

	bool operator<  (const Plan &plan) const;
	bool operator>  (const Plan &plan) const;
	bool operator== (const Plan &plan) const;
	bool operator!= (const Plan &plan) const;


	bool Plan_Is_Needed_And_Valid() const { return m_the_agent != NULL; };

	Utility Compute_Matching_Value(Goal_ptr goal_ptr);
	Utility Get_Matching_Value() const { return m_matching_value; };

	Agent_ptr Get_Agent() const { return m_the_agent; };

	void Commit_Agent_Common(Goal_ptr goal_ptr);

	bool All_Unused_Or_Used_By_This(const Goal_ptr theGoal) const;

	sint32 Get_Free_Transport_Capacity() const;

	bool Has_Cargo            ()                        const;

	bool Get_Needs_Cargo      ()                        const { return m_needs_cargo && !Has_Cargo();   };
	void Set_Needs_Cargo      (const bool needsCargo)         { m_needs_cargo = needsCargo;             };

	bool Get_Needs_Transporter()                        const { return m_needs_transporter;             };
	void Set_Needs_Transporter(const bool needsTransporter)   { m_needs_transporter = needsTransporter; };

	bool Get_Cannot_Be_Used   ()                        const { return m_cannot_be_used;                };
	void Set_Cannot_Be_Used   (const bool cannotBeUsed)       { m_cannot_be_used = cannotBeUsed;        };

protected:

	Utility          m_matching_value;
	Agent_ptr        m_the_agent;
	bool             m_needs_cargo;
	bool             m_cannot_be_used;
	bool             m_needs_transporter;
};


#endif // PLAN_H__
