#ifndef __squads_h__
#define __squads_h__


#include "strategic_ai_forward.h"

#include "squad_strength.h"







#ifndef NDEBUG

typedef struct {
  squad_ptr    next;
  squad_ptr    prev;
  const char * filename;
  int          linenumber;
} SQUAD_TRACK;
#endif










#define MAX_MOVEMENT_ATTEMPTS 3









struct MapPointData;
enum Goal_Result;






class Squad_Flat_Data
{
public:
	int squad_ID;

	int already_committed;

	int resend;

	GOAL_TYPE last_goal_type;

    BOOL auxiliary_candidate;

	SQUAD_CLASS squad_type;


	Squad_Flat_Data();

	void Serialize(CivArchive &archive);
};















class Squad : public Squad_Flat_Data
{
public:


	Planner_ptr the_team;

	AiMain *ai;




    Squad_Strength m_squad_strength;

	linked_list my_agents;




	linked_list ignore_goals;

	Goal_ptr my_strategic_goal;
    sint32 my_goal_id;

	int squad_pool_index;
	uint32 m_moveType;




#ifdef _DEBUG
	bool matched_to_any_goal;
#endif

public:






	Squad();







	void Init
	(
		Planner_ptr init_team = NULL
	);







	Squad
	(
		Planner_ptr init_team,
		CivArchive &archive
	);










	~Squad ();








	void Clean_Up();








    void Load(CivArchive &archive);








    void Store(CivArchive &archive);








    void Serialize(CivArchive &archive);








	void Set_Goal
	(
	  Goal_ptr new_strategic_goal
	);








	void Detach_Goal ();







	void Squad::Detach_Agents();







	void Compute_Location();









	double Compute_Utility
	(
		ArmyGoal *goal_obj
	);










	uint32 Still_Can_Act();








	bool Contains_A_City();








	void Display_Composition(AiMain *ai);







	void Display_Squad_Type(AiMain *ai);






	void Send_Units
	(
		strategy_map_ptr the_map
	);






	void Recompute_Squad_Class(AiMain *ai);









	void Reclassify_Squad(AiMain *ai, Agent * add_me);








	void Add_Agent
	(
        AiMain *ai,
		Agent * the_agent
	);








	void Remove_Agent
	(
		Agent * the_agent
	);

    void Remove_Dead_Agents();








	void Take_All_Agents
	(
		AiMain *ai,
		Squad *donor_squad
	);





	BOOL Recruit_Best_Strength
	(
		AiMain *ai,
		Goal *the_goal,
		Squad *donor_squad,
		bool looking_for_transports = false
	);

    int GetID() const { return squad_ID; }
    void DumpGoal(AiMain *ai, const GOAL_TYPE goal_type, const double match_value);

	BOOL Ok_To_Rally(AiMain *ai);












	int Choose_Rally_Point
	(
		AiMain *ai,
		MapPointData target,

		MapPointData &rally_pos,
		int &current_agent
	);










	Goal_Result Send_Troops_To_Rally
	(
		AiMain *ai,
		int last_agent,
		int first_agent,
		MapPointData &rally_pos,
		ArmyGoal *the_goal
	);










	Goal_Result Rally_Troops
	(
		AiMain *ai,
		MapPointData target,

		ArmyGoal *the_goal,
        BOOL &any_group_needed_to_rally
	);

	int GetUnitCount() const;
	Agent *GetUnitByNumber(const int &i);

	sint32 GetUsableTransport(AiMain *ai);
	double GetUsableAttack(AiMain *ai);
	double GetUsableDefense(AiMain *ai);

	void GroupWithArmy(AiMain *ai, ArmyAgent *army_agent);
	void UngroupAllArmies(AiMain *ai);
	double ScoreTransportUtility(AiMain *ai, ArmyAgent *transport);
	sint32 GetTransportUnitsCount(AiMain *ai);
	uint32 GetMoveTypes(AiMain *ai);
};








#endif
