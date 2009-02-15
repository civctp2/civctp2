//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Scheduler
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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Marked MS version specific code.
// - Redesigned AI, so that the matching algorithm is now a greedy algorithm. (13-Aug-2008 Martin Gühmann)
// - Now the goals are used for the matching process, the goal match value
//   is the avarage match value of the matches needed for the goal.
// - Simplified the design the number of committed agents and number of
//   agents are now calculated inside the Match_Resources method. (21-Aug-2008 Martin Gühmann)
// - Fixed unit garrison assignment. (23-Jan-2009 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "scheduler_types.h"

#include "squad_Strength.h"
#include "StrategyRecord.h"


#include <vector>
#include <utility>
#include <deque>

#include "c3debugstl.h"

#include "civarchive.h"
#include "goal.h"               // Needed here to instantaite std::greater<Goal_ptr> correctly

class GoalRecord;
class Scheduler;
class Army;


class Scheduler
{

public:

#ifdef _DEBUG
	#define dbga dbgallocator

	typedef std::vector<sint16, dbgallocator<sint16> >                       Count_Vector;
	typedef std::list<GOAL_TYPE, dbgallocator<GOAL_TYPE> >                   Goal_Type_List;
	typedef std::list<Sorted_Goal_ptr, dbgallocator<Sorted_Goal_ptr> >       Sorted_Goal_List;
	typedef Sorted_Goal_List::iterator                                       Sorted_Goal_Iter;
	typedef Sorted_Goal_List::const_iterator                                 Sorted_Goal_Const_Iter;
	typedef std::list<Goal_ptr, dbgallocator<Goal_ptr> >                     Goal_List;
	typedef std::list<SQUAD_CLASS, dbgallocator<SQUAD_CLASS> >               Squad_Class_List;
	typedef std::vector<Sorted_Goal_List, dbgallocator<Sorted_Goal_List> >   Sorted_Goal_List_Vector;
	typedef std::vector<Sorted_Goal_List::iterator, dbgallocator<Sorted_Goal_List::iterator> > Sorted_Goal_List_Iter_Vector;
	typedef std::vector<Agent_List, dbgallocator<Agent_List> >               Agent_List_Vector;

	typedef std::list<Sorted_Agent_ptr, dbgallocator<Sorted_Agent_ptr> >     Sorted_Agent_List;
	typedef std::vector<Sorted_Agent_List, dbgallocator<Sorted_Agent_List> > Sorted_Agent_List_Vector;
	typedef Sorted_Agent_List::iterator                                      Sorted_Agent_Iter;

#if defined(_MSC_VER) && (_MSC_VER < 1300)	// does not compile with newer version
	typedef std::deque<Scheduler, dbga<Scheduler> >                          Scheduler_Vector;
#else
	typedef std::vector<Scheduler, dbgallocator<Scheduler> >                 Scheduler_Vector;
#endif

#else
	
	typedef std::vector<sint16>                                              Count_Vector;
	typedef std::list<GOAL_TYPE>                                             Goal_Type_List;
	
	typedef std::list<Sorted_Goal_ptr>                                       Sorted_Goal_List;
	typedef Sorted_Goal_List::iterator                                       Sorted_Goal_Iter;
	typedef Sorted_Goal_List::const_iterator                                 Sorted_Goal_Const_Iter;
	typedef std::list<Goal_ptr>                                              Goal_List;
	typedef std::list<SQUAD_CLASS>                                           Squad_Class_List;
	typedef std::vector<Sorted_Goal_List>                                    Sorted_Goal_List_Vector;
	typedef std::vector<Sorted_Goal_List::iterator>                          Sorted_Goal_List_Iter_Vector;
	typedef std::vector<Agent_List>                                          Agent_List_Vector;

	typedef std::list<Sorted_Agent_ptr>                                      Sorted_Agent_List;
	typedef std::vector<Sorted_Agent_List>                                   Sorted_Agent_List_Vector;
	typedef Sorted_Agent_List::iterator                                      Sorted_Agent_Iter;

#if defined(_MSC_VER) && (_MSC_VER < 1300)	// does not compile with newer version	
	typedef std::deque<Scheduler>                                            Scheduler_Vector;
#else
	typedef std::vector<Scheduler>                                           Scheduler_Vector;
#endif
#endif

	static sint32 s_max_match_list_cycles;


	static void ResizeAll(const PLAYER_INDEX & newMaxPlayerId);

#if 0
	static void LoadAll(CivArchive & archive);
	static void SaveAll(CivArchive & archive);
#endif

	static Scheduler & GetScheduler(const sint32 & playerId);

	static void CleanupAll(void);


	static void       SetContactCache         (sint32 player);
	static bool CachedHasContactWithExceptSelf(sint32 player1, sint32 player2);
	
	static void    SetIsNeutralRegardCache(sint32 player);
	static bool CachedIsNeutralRegard     (sint32 player, sint32 opponent);
	
	
	static void    SetIsAllyRegardCache(sint32 player);
	static bool CachedIsAllyRegard     (sint32 player, sint32 ally);


	Scheduler();



	Scheduler(const Scheduler &scheduler);



	~Scheduler();

	
	Scheduler& operator= (const Scheduler &scheduler);

	
	void Cleanup();

#if 0
	void Load(CivArchive & archive);
	void Save(CivArchive & archive);
#endif

	void Initialize();

	
	void SetPlayerId(const PLAYER_INDEX &team_index);

	void Process_Agent_Changes();
	void Process_Goal_Changes();


	void Reset_Agent_Execution();

	void Sort_Goals();


	void Match_Resources(const bool move_armies);

	
	
	void Add_New_Goal(const Goal_ptr & new_goal);

	void Add_New_Agent(const Agent_ptr & new_agent);
	Agent_List::iterator Add_Agent(const Agent_ptr & agent);

	Sorted_Goal_Iter Remove_Goal(const Sorted_Goal_Iter & sorted_goal_iter);

	
	void Remove_Goals_Type(const GoalRecord *rec);

	
	
	Squad_Strength GetMostNeededStrength() const;


	sint32 GetValueUnsatisfiedGoals(const GOAL_TYPE & type) const;


	Goal_ptr GetHighestPriorityGoal(const GOAL_TYPE & type, const bool satisfied) const;


	sint16 CountGoalsOfType(const GOAL_TYPE & type) const;


	void DisbandObsoleteArmies(const sint16 max_count);

	bool Prioritize_Goals();

	bool Prune_Goals();

	
	void SetArmyDetachState(const Army & army, const bool detach);
	void Recompute_Goal_Strength();
	void Compute_Agent_Strength();
	void Rollback_Emptied_Transporters();
	void Sort_Goal_Matches_If_Necessary();
	void Assign_Garrison();

protected:

	void Add_New_Matches_For_Goal
	(
	    const Goal_ptr & goal_iter,
	    const bool       update_match_value = true
	);


	void Add_New_Matches_For_Agent(const Agent_ptr & agent);


	void Remove_Matches_For_Goal( const Goal_ptr & goal );


	void Remove_Matches_For_Agent( const Agent_ptr & agnet );


	void Rollback_Matches_For_Goal(const Goal_ptr & goal_ptr);

	void Reprioritize_Goal(Goal_List::iterator &goal_iter);

	bool Add_Transport_Matches_For_Goal(const Goal_ptr & goal_ptr);

	
	GOAL_TYPE GetMaxEvalExec(const StrategyRecord::GoalElement *goal_element_ptr, sint16 & max_eval, sint16 & max_exec);

	
	bool GetArmyDetachState(const Army & army) const;

private:

	static Scheduler_Vector      s_theSchedulers;


	Count_Vector                 m_pruned_goals_count;
	Sorted_Goal_List_Vector      m_goals_of_type;
	Sorted_Goal_List_Iter_Vector m_pruned_goals_of_type; // Not needed
	Agent_List                   m_agents;
	Goal_List                    m_new_goals;
	Agent_List                   m_new_agents;
	PLAYER_INDEX                 m_playerId;
	Squad_Strength               m_neededAgentStrength;
	Goal_List                    m_goals;

	static sint32 m_contactCachedPlayer;
	static uint32 m_contactCache;
	static sint32 m_neutralRegardCachedPlayer;
	static uint32 m_neutralRegardCache;
	static sint32 m_allyRegardCachedPlayer;
	static uint32 m_allyRegardCache;
};

#endif
