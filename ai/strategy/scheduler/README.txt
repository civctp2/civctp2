
Planning System
----------------

Modifications:

Cyclic Dependencies
-------------------

		Broke dependency of Squad on Planner (the_team); Squads keep
track of what team number they belong to only.  Squads now lookup
component agents directly from gamestate.  This way we can load squads
from a file before we load teams with out having to use a method to
later hook them up.

Removed Variables
-----------------

		Squad::resend -- not used

		Squad::auxiliary_candidate -- always true

		Goal::m_farthest_agent_to_target_dist -- CTP specific

		Goal::new_goal_cell_ptr -- used to remove goal from new_goals
		linked list in scheduler.  Goal should not need to keep this info.

		Goal::m_current_attacking_strength -- Available from
        Goal::m_attacking_squad; better to not keep this information
        in two places that need to be constantly synced up.

		Goal::m_my_squad_id -- Used only for loading and saving.

        Plan::used -- Not used (commented out) of Scheduler::Clean_up.
        All plans are removed, not just used ones.

		Plan::m_needed_strength -- Not used (saved and loaded, but not used).

		Scheduler::precompute_squad_number -- Not Used.

		Scheduler::max_priority -- Not Used (only set).

		Scheduler::min_priority -- Not Used (only set).

		Scheduler::m_last_best_goal_of_type -- Not Used (only set)

		Squad::ignore_goals	-- new system makes this unnecessary

		Squad::my_strategic_goal -- new system makes this unnecessary

		Squad::my_goal_id -- no serialization

		Squad::squad_pool_index -- STL makes this unnecessary

		Squad::m_squad_strength -- removed so that squads don't cache agent data that might change



Removed Methods
---------------

		virtual void Goal::HookUp(AiMain *ai) -- noop 

		Goal::Commit_Unit( Agent * unit_in_question ) -- Not used in
        code; must be obsolete and not intregral to design.

		Squad::Send_Units() -- Method not used.

		Squad::Clean_Up() -- New system makes this unnecessary

		Squad::Serialize(CivArchive &archive) -- Serialization removed 

		Squad::Set_Goal( Goal_ptr new_strategic_goal ) -- New system
		makes this unnecessary.

		Squad::Detach_Goal () -- New System make this unnecessary.

		Squad::Detach_Agents() -- New System makes this unnecessary.

		Squad::Contains_A_City() -- Specific to CTP

		Scheduler::Clean_Up() -- Not needed because cyclic
		dependencies do not exist.

		Scheduler::Prepare_Cities() -- CTP specific.

		Scheduler::Init_Match_Lists() -- STL makes this unnecessary.

		Scheduler::Generate_Matches_And_Valid_Goal_List() -- New
		system makes this unnecessary.

		Scheduler::Allocate_Troops_And_Execute_Tasks() -- This method
		has been broken out and its constituent steps have been made public.

		Scheduler::Allocate_Remaining_Troops() -- Not Implemented.

		Scheduler::Return_Goal_By_Number() -- Used by serialization.

		Scheduler::Update_Goals() -- Not neeeded because goals are now
		detached as they are completed.

		Scheduler::Add_Single_Plan() -- CTP specific

		Scheduler::Remove_Goal_And_Add_To_Delete_List() -- Goals are
		removed during Match_Resources if they are completed or fail.

		Goal::Detach_Squad() -- New system makes this unnecessary.

		Goal::Clean_Up() -- New system makes this unnecessary.

		Goal::Classify_Goal() -- Redundant and performed no function.

		Goal::Distance_From_Squad_To_Goal() -- Implemented but never called.

		Goal::EstimateRecruitmentStrength() -- Obsolete architectually (replaced by Compute_Matching_Value)

		Squad::Compute_Location() -- commented out and not used
		
		Squad::Compute_Utility() -- Obsolete architectually

		Squad::Still_Can_Act() -- Not relevant in non-turn based games

		Squad::Recruit_Best_Strength() - Obsolete architectually

		Squad::GetID() -- ID's not used

		Squad::Ok_To_Rally() -- Rally code should be moved to goal class

		Squad::Choose_Rally_Point() -- Rally code should be moved to goal class

		Squad::Send_Troops_To_Rally -- Rally code should be moved to goal class

		Squad::Rally_Troops	-- Rally code should be moved to goal class

		Squad::GetUnitCount() -- replaced with Squad::Get_Num_Agents()

		Squad::GetUnitByNumber() -- replaced with Squad::Get_Agent()

		Squad::Take_All_Agents() -- Architectually obsolete

Removed CTP specifics
---------------------
		
		Removed references to AiMain in Squad class; functions now
performed by externals.

Renamed Methods
---------------

		Scheduler::Update_Squads to Process_Squad_Changes -- Conotation
		has changed; we call this method once before matching phase.
		Removing empty squads should not be done in the middle of plan
		execution because there is no way to inform existing matches
		that a squad has been removed.

Renamed Variables
-----------------

Squad/Goal/Plan Lookup Mechanism
--------------------------------

		Planner::Look_Up_Squad_In_Load_Table(int the_ID) -->
				Squad::GetSquadFromId(const uint32 &the_ID) (static, public)

		Planner::my_squads --> 
				Squad::s_loadedSquads (static, private)
				
		Planner::Goal_ptr Look_Up_Goal_In_Load_Table(int the_ID) -->
				Goal::GetGoalFromId (static, public)

		Scheduler::unique_goal_ID -->
				Goal::s_nextUniqueGoalId (static, private)

		Planner::unique_plan_ID -->
				Plan::s_nextUniquePlanId (static, private)

typedef Goal::Utility to sint32 (was double)
--------------------------------------------
Use sint32 values rather than double for match utility and priorities.
This change was made to reduce rounding errors that occurr when
comparing floating point values.  Also makes it possible to use
counting based sort algorithms (rather than compare based algorithms).
Why must utilities be unique anyway?

Move Plan::AccumulateEnoughStrength to Scheduler.cpp
----------------------------------------------------
A plan should not refer to members of the scheduler, instead the scheduler
should refer to members of a plan.  Eliminates a cyclic redundancy.

Goals do not have pointers to squads and vice versus
----------------------------------------------------
Why? Removing these pointer references eliminates the need for error
prone mechanisms that synch goals and squads when one or the other is
removed.

Miscellaneous Comment
---------------------
Function names and variables with underbars (_) slow down emacs
editing.  When you M-f (next word) you have to do it multiple times
per statement.  This makes macros harder to write too.  BUT it does
make the code easier to read..

=============================================================================

TBD:

Use Allocators
--------------

Rather than using Red-Black tree and pools, create an object allocator
from which references to objects can be created.  These references
point to objects that are pulled off of a vector that grows when
objects are needed, and keeps a queue of references that are no longer
needed and can be reused.  Once we have this efficient allocator, we
can use multisets for our matches.


Agent, ArmyAgent (CraftAgent) parred down
-----------------------------------------
Should wrap game object handle and provide direct access to game
objects.  Perhaps add semantics, like 'protected army' or 'escorting
army'.  This purpose here is to elliminate the bookeeping associated with
keeping AI agents and game state agents in synch.

When to Recompute squad strength
----------------------------------------------------------- 
Before matches are made, recompute army and squad strengths based on
most up-to-date info.  Goals should look directly at game objects in
squads to compute their utility.

Removed All Serialization code
-------------------------------
Reason: should be able to regenerate state from game objects.

Squad::Reclassify_Squad 
-----------------------
Originally the method Squad::Reclassify_Squad called back up to
scheduler when ever an army was added (but not removed) from the
squad.  But, the squad classification is a concept used only in the
scheduler.  The scheduler can reclassify squads itself anytime that
calling Plan::Commit_Squad or Plan::Rollback returns true (ie. armies
were transfered.  The squad can be gotten via Plan::Get_Squad().
Reclassification might also be defered by the scheduler until before
beginning a match cycle.

Changed so that Reclassify_Squad returns SQUAD_CLASS and does not
call back to Scheduler::Reclassify_Squad.


=============================================================================

Steps in Scheduler:

"Squads Changed"

I. squads_of_class

		  1. Remove any empty squads and any matches that reference them.

          2. Recompute squad class for all squads with changed/killed agents.
		  
		  3. If squad class of squad changes, move to new
             squads_of_class list.

	      4. Create new squads from new agents, add to squads_of_class
             list and create new matches from pruned_goals_list.

"Goals Changed"

II. sorted_goals_of_class
	
		  1. Remove all invalid/complete goals from sorted_goals list.

		  2. Recompute raw priority of all new/changed goals from
             sorted_goals_of_class list.

		  3. Sort sorted_goals_of_class lists.

III. pruned_goals_of_class

		  1. Remove all invalid/complete goals from pruned_goals_of_class lists
             and remove corresponding matches.

		  2. Recompute raw priority of all changed goals from
			 pruned_goals_of_class lists.

		  3. Sort pruned_goals_of_class lists.

IV. pruned_goals_of_class

		  1. Iterate from highest priority goals on
             pruned_list_of_class and compare to highest priority on
             sorted_list_of_class.

		  2. Count out maximum number of each goal type.

	      3. If goal from sorted_list_of_class > pruned_list_of_class,
             move it to the pruned_goals list and create new matches.

	      4. If goal from pruned_list_of_class < sorted_list_of_class,
			 move it from the pruned_goals_of_class list to the
			 sorted_goals_of_class list and rollback/remove
			 corresponding matches.

"Matches Changed"

V. matches

		  1. For each match, compute the utility value between the
             goal and each component agent in the squad.

	      2. Compute average match value from agent utility values and
             goal raw priority.

		  3. Sort matches list.
		  
"Match Cycle"

VI. matches

	  Iterate: matches

		  1. For each match, move agents from donor squad to goal.

		  2. If goal is in progress, execute goal and check status.

		  3. If goal is completed, create a new squad and match it to
		  pruned goals list.

		  4. If goal failed, rollback agents to donor squads.
		  Rollback marks agents in matches as invalid for the goal.

		  5. When all agents have been commited, check all goals on
		  pruned_goals_class list.  If under commited, rollback agents
		  to donor squads.


Perform steps I. to IV. periodically, and step VI. often.
