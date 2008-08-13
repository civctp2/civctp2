//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : declarations for the scheduler_types class
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
// - Removed MSVC specific code.
// - Standardised imports.
//
//----------------------------------------------------------------------------

#ifndef __SCHEDULER_TYPES_H__
#define __SCHEDULER_TYPES_H__ 1

#include <list>       // std::list
#include <utility>    // std::pair

#include "Goal_And_Squad_Types.h"


enum GOAL_RESULT
{
    GOAL_FAILED,
    GOAL_NEEDS_TRANSPORT,
    GOAL_ALREADY_MOVED,
    GOAL_IN_PROGRESS,
    GOAL_COMPLETE
};

typedef sint32				Utility;
typedef sint32				PLAYER_INDEX;

class Agent;
class Squad_Strength;
class Goal;
class Squad;
class Plan;

typedef Agent*			Agent_ptr;
typedef Goal*			Goal_ptr;
typedef Squad*			Squad_ptr;
typedef Plan*			Plan_ptr;

typedef std::pair<Utility, Goal_ptr> Sorted_Goal_ptr; 


template<class _T1, class _T2>
bool operator <
(
    const Sorted_Goal_ptr& _X,
    const Sorted_Goal_ptr& _Y
)
{
	return (_X.first < _Y.first);
};

typedef std::list<Agent_ptr> Agent_List;
typedef std::list<Squad_ptr> Squad_List;
typedef std::list<Plan>      Plan_List;

#endif //__SCHEDULER_TYPES_H__
