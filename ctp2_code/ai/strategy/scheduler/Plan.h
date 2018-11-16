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
public:

    struct Agent_Match {
        Utility value;
        Agent_List::const_iterator squad_index;
        Agent_List::const_iterator goal_index;
        bool committed;
        bool operator<(const Agent_Match &match) const {
            return value < match.value;
        }
        bool operator>(const Agent_Match &match) const {
            return value > match.value;
        }
    };

    typedef std::list<Agent_Match> Agent_Match_List;

    Plan();
	Plan(const Plan &plan);

    virtual ~Plan();

	Plan& operator= (const Plan &plan);

	bool operator< (const Plan &plan) const;
	bool operator> (const Plan &plan) const;
	bool operator== (const Plan &plan) const;
	bool operator!= (const Plan &plan) const;

	GOAL_TYPE Get_Goal_Type() const;

    bool Plan_Is_Needed_And_Valid() const;

    Utility Compute_Matching_Value();

    Utility Get_Matching_Value() const;

    void Set_Goal(Goal_ptr goal);

    Goal_ptr Get_Goal() const;

    void Set_Squad(Squad_ptr squad);

    Squad_ptr Get_Squad() const;

	sint32 Commit_Agents();

    GOAL_RESULT Execute_Task();

	sint32 Rollback_All_Agents();

 	bool Commited_Agents_Need_Orders() const;

    ///
    /// Check if the agents can be rollbacked to eventually receive other priority
    /// (for example : not if they are too close to their goals and grouping)
    ///
    bool CanMatchesBeReevaluated() const;
	void Move_All_Agents(Squad_ptr new_squad);

    bool Remove_Agent_Reference(const Agent_List::const_iterator & agent_iter);

	bool Agent_Committed(const Agent_ptr agent_ptr) const;

protected:

	Utility m_matching_value;

	Squad_ptr m_the_squad;

	Goal_ptr m_the_goal;

	Agent_Match_List m_matches;

};

#endif // PLAN_H__
