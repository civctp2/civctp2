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
//
//----------------------------------------------------------------------------

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__


#include "Plan.h"

#include "scheduler_types.h"
#include "squad_Strength.h"
#include "StrategyRecord.h"


#include <vector>
#include <utility>
#include <deque>

#include "c3debugstl.h"

#include "civarchive.h"

class GoalRecord;
class Scheduler;
class Army;
















class Scheduler
{

public:

#ifdef _DEBUG
	#define dbga dbgallocator
	
	typedef std::vector<sint16, dbgallocator<sint16> > Count_Vector;
    typedef std::list<GOAL_TYPE, dbgallocator<GOAL_TYPE> > Goal_Type_List;
    typedef std::list<Sorted_Goal_ptr, dbgallocator<Sorted_Goal_ptr> > Sorted_Goal_List;
	typedef Sorted_Goal_List::iterator Sorted_Goal_Iter;
    typedef std::list<Goal_ptr, dbgallocator<Goal_ptr> > Goal_List;
    typedef std::list<SQUAD_CLASS, dbgallocator<SQUAD_CLASS> > Squad_Class_List;
	typedef std::vector<Sorted_Goal_List, dbgallocator<Sorted_Goal_List> > Sorted_Goal_List_Vector;
    typedef std::vector<Sorted_Goal_List::iterator, dbgallocator<Sorted_Goal_List::iterator> > Sorted_Goal_List_Iter_Vector;
	typedef std::vector<Squad_List, dbgallocator<Squad_List> > Squad_List_Vector;
#if defined(_MSC_VER) && (_MSC_VER < 1300)	// does not compile with newer version	
	typedef std::deque<Scheduler, dbga<Scheduler> > Scheduler_Vector;
#else
	typedef std::vector<Scheduler, dbgallocator<Scheduler> > Scheduler_Vector;
#endif

#else
	
	typedef std::vector<sint16> Count_Vector;
    typedef std::list<GOAL_TYPE> Goal_Type_List;
	
    typedef std::list<Sorted_Goal_ptr> Sorted_Goal_List;
	typedef Sorted_Goal_List::iterator Sorted_Goal_Iter;
    typedef std::list<Goal_ptr> Goal_List;
    typedef std::list<SQUAD_CLASS> Squad_Class_List;
	typedef std::vector<Sorted_Goal_List> Sorted_Goal_List_Vector;
    typedef std::vector<Sorted_Goal_List::iterator> Sorted_Goal_List_Iter_Vector;
	typedef std::vector<Squad_List> Squad_List_Vector;
	
#if defined(_MSC_VER) && (_MSC_VER < 1300)	// does not compile with newer version	
	typedef std::deque<Scheduler> Scheduler_Vector;
#else
	typedef std::vector<Scheduler> Scheduler_Vector;
#endif
#endif

  	
	
	
	
  	

	
	enum TIME_SLICE_STATE
	{ 
		TIME_SLICE_NOT_DONE = 0,
		TIME_SLICE_DONE = 1
	};

    
    enum 
    {
        
        MAX_DEBUG_STR = 1000
    };

	
	static sint32 s_max_match_list_cycles;

  	
  	
    
	
  	

	
	static void ResizeAll(const PLAYER_INDEX & newMaxPlayerId);

	
	static void LoadAll(CivArchive & archive);

	
	static void SaveAll(CivArchive & archive);

	
	static Scheduler & GetScheduler(const sint32 & playerId);

	
	static void ValidateAll();

	
	static void CleanupAll(void);
	
	
	
	
	
	
	
	
	
	
	
	
	static void SetContactCache(int player);
	static int CachedHasContactWithExceptSelf(int player1,int player2);
	
	static void SetIsNeutralRegardCache(int player);
	static int CachedIsNeutralRegard(int player,int opponent);
	
	
	static void SetIsAllyRegardCache(int player);
	static int CachedIsAllyRegard(int player,int ally);


  	
  	
    
	
  	
	
	
	
	


	Scheduler();


	
	Scheduler(const Scheduler &scheduler);



	virtual ~Scheduler();

	
	Scheduler& operator= (const Scheduler &scheduler);

	
	void Cleanup();

	
	void Load(CivArchive & archive);

	
	void Save(CivArchive & archive);

	
	void Initialize();

	
	void SetPlayerId(const PLAYER_INDEX &team_index);

	
	
	
	
	
    void Reclassify_Squad
	(
		Squad_ptr the_squad,					
		const SQUAD_CLASS &old_squad_class		
	); 

	
	
	void Planning_Status_Reset();

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	TIME_SLICE_STATE Process_Squad_Changes();

	
	
	
	
	
	
	
	
	TIME_SLICE_STATE Process_Goal_Changes();

	
	void Reset_Squad_Execution();

	
	
	
	
	
	
	
	
	
	
	
	
	bool Sort_Matches();

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	void Scheduler::Match_Resources(const bool move_armies);

	
	
	void Add_New_Goal(const Goal_ptr & new_goal);

	
	
	void Add_New_Squad(const Squad_ptr & new_squad);

    
	Sorted_Goal_Iter Remove_Goal(const Sorted_Goal_Iter & sorted_goal_iter);

	
	void Remove_Goals_Type(const GoalRecord *rec);

	
	
	Squad_Strength GetMostNeededStrength() const;

	
	
	

	
    bool Validate() const;

	
	Sorted_Goal_List Get_Top_Goals(const int &number) const;

	
	sint32 GetValueUnsatisfiedGoals(const GOAL_TYPE & type) const;

	
	Goal_ptr GetHighestPriorityGoal(const GOAL_TYPE & type, const bool satisfied) const;

	
	sint16 CountGoalsOfType(const GOAL_TYPE & type) const;

	
	void DisbandObsoleteArmies(const sint16 max_count);

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	bool Prioritize_Goals();

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	bool Prune_Goals();

	
	void SetArmyDetachState(const Army & army, const bool detach);

protected:

	
	
	
	
	

	
	
	bool Add_New_Match_For_Goal_And_Squad
	(
	 const Goal_ptr & goal_ptr,
	 const Squad_List::iterator & squad_iter,
	 Plan_List::iterator & plan_iter
	);

	
	
	sint32 Add_New_Matches_For_Goal
	(
	 const Goal_ptr & goal_iter
	);

	
	
	sint32 Add_New_Matches_For_Squad
	(
	 const Squad_List::iterator & squad_iter
	);

    
    
    
	bool Free_Undercommitted_Goal();

	
	void Remove_Matches_For_Goal( const Goal_ptr & goal );

	
	void Remove_Matches_For_Squad( const Squad_ptr & squad );

	
	void Remove_Match(Plan_List::iterator &plan_iter);

	
	
	sint32 Scheduler::Rollback_Matches_For_Goal
	(
	 const Goal_ptr & goal_ptr 
	);

	
	
	
	Squad_ptr Scheduler::Form_Squad_From_Goal
	(
	 const Goal_ptr & goal_ptr 
	);

	bool Add_Transport_Matches_For_Goal
	(
	 const Goal_ptr & goal_ptr,
	 Plan_List::iterator & plan_iter
	);

	
	GOAL_TYPE GetMaxEvalExec(const StrategyRecord::GoalElement *goal_element_ptr, sint16 & max_eval, sint16 & max_exec);

	
	bool GetArmyDetachState(const Army & army) const;

private:

    static char s_debug_str[MAX_DEBUG_STR];

	static Scheduler_Vector s_theSchedulers;

	
	
	
	
	

	
	Count_Vector m_exec_goal_count; 

	
	Count_Vector m_pruned_goals_count; 

	
    
    
    
	Sorted_Goal_List_Vector m_goals_of_type; 

	
    Sorted_Goal_List_Iter_Vector m_pruned_goals_of_type; 

	
	Squad_List m_squads;

    
	
	
	Goal_List m_new_goals; 

    
	
	
	Squad_List m_new_squads;

	
	PLAYER_INDEX m_playerId;

	
	Plan_List m_matches;

	
	sint32 m_committed_agents;

	
	sint32 m_total_agents;

	
	Plan_List::iterator m_current_plan_iter;

	
	
	Squad_Strength m_neededSquadStrength;
	Utility m_maxUndercommittedPriority;

	
	
    static sint32 m_contactCachedPlayer;
	static uint32 m_contactCache;
	static sint32 m_neutralRegardCachedPlayer;
	static uint32 m_neutralRegardCache;
	static sint32 m_allyRegardCachedPlayer;
	static uint32 m_allyRegardCache;
};

#endif
