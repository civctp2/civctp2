







#ifndef __planner_h__
#define __planner_h__



class AIP;
class Strategic_Map;
template <class DATA_TYPE> class list_array;
template <class DATA_TYPE, class KEY_TYPE> class sorted_array;

#include "aip.h"
#include "strategic_ai_forward.h"


#define NO_SQUAD -1

#define RBD_EXPIRATION_TIME ((float) 120.0)
#define RBD_TRIGGER_TIME ((float) 2.0)

#define SQUAD_ARRAY_BASE_SIZE 400
#define INITAL_GOAL_LIST_SIZE 400









class Planner_Flat_Data
{


private:



public:
	int current_squad_ID;				

	int team_ID;						

	int aicontrol;						



	int mapgrid_added;					

	int max_cell_priority;				

	int priorities_changed;				

    BOOL is_there_new_goals;            
    BOOL is_there_new_squads;




	Planner_Flat_Data(); 
	void Serialize(CivArchive &archive); 
};















class Planner : public Planner_Flat_Data
{


private:



public:

	AIP the_AIP;						

	AiMain *ai;							

	Scheduler *the_scheduler;			





	


	
	sorted_array<Goal_ptr,int> my_goals;
	linked_list my_agents;				
	list_array<Squad *> * my_squads;	

	Strategic_Map *the_strategic_map;	
	sint32 m_usableTransport;			
	double m_usableAttack;				
	double m_usableDefense;				



public:

	
	
	
	
	
	
	Planner
	(
		AiMain *init_ai,				
		int i_team_ID = -1				
	);


	
	
	
	
	
	
	Planner
	(
		AiMain *init_ai,				
		CivArchive &archive				
	);


	
	
	
	
	
	
	void Init();


    
    void ZeroInit();


	
	
	
	
	
	
	void Clean_Up();


	
	
	
	
	
	
	~Planner();


	
	
	
	
	
	
    void Load(CivArchive &archive); 


	
	
	
	
	
	
    void Store(CivArchive &archive); 


	
	
	
	
	
	
    void Serialize(AiMain *ai, CivArchive &archive); 
    void HookUp(AiMain *ai); 

	
	
	
	
	
	
	void Insert_Agent
	(
		Agent *agent
	);


	
	
	
	
	
	
	void Remove_Agent
	(
		Agent *agent
	);


	
	
	
	
	
	
	void Update_Map(AiMain *ai);


	
	
	
	
	
	
	void Update_Squads(AiMain *ai);


	
	
	
	
	
	
	void Update_Goals();


	
	
	
	
	
	
	void Adjust_Death_Differential
	(
		double adjustment,					
		MapPointData query_pos				
	);


	
	
	
	
	
	
	void Add_Squad(squad_ptr new_squad);


	
	
	
	
	
	
	
	squad_ptr Look_Up_Squad_In_Load_Table(int the_ID);


	
	
	
	
	
	
	
	Goal_ptr Look_Up_Goal_In_Load_Table(int the_ID);

    void LoadMyAgents(AiMain *init_ai, CivArchive &archive); 
    void StoreMyAgents(CivArchive &archive); 
    void LoadMyGoals(CivArchive &archive); 
    void StoreMyGoals(CivArchive &archive); 
    void LoadMySquads(CivArchive &archive); 
    void StoreMySquads(CivArchive &archive); 
    void HookSquadGoals(); 
    void Add_Goal(Goal* new_goal); 
    void Add_Single_Plan_Goal(Goal* new_goal); 
    void StoreGoodPlans(AiMain *init_ai, CivArchive &archive); 
    void LoadGoodPlans(AiMain *init_ai, CivArchive &archive); 


	
	
	
	
	
	
	
	Goal_ptr Return_Goal_By_ID(int the_ID);

    BOOL Validate(AiMain *ai); 


	sint32 GetUsableTransport();
	void SetUsableTransport(const sint32 &count);

	double GetUsableAttack();
	void SetUsableAttack(const double &attack);

	double GetUsableDefense();
	void SetUsableDefense(const double &defense);
};



























































































































#endif 
