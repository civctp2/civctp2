












#ifndef __AIP_h__
#define __AIP_h__



#include "Squad_And_Goal_Types.h"
#include "Iparser.h"



template <class DATA_TYPE, class KEY_TYPE> class sorted_array;
class AiMain;
class CityAgent;

typedef unsigned Flag;

typedef class AIP * AIP_ptr;


#define MAX_AIP_STRING 80
#define MAX_AIP_FILE_NAME 512
#define INITIAL_UNIT_PRIORITY_LIST_SIZE 5

#define WONDER_LIST_NAME "Wonder_List"
#define TRANSPORTATION_LIST_NAME "Transportation_List"
#define GROWTH_LIST_NAME "Growth_List"
#define HAPPINESS_LIST_NAME "Happiness_List"
#define PRODUCTION_LIST_NAME "Production_List"
#define GOLD_LIST_NAME "Gold_List"
#define SCIENCE_LIST_NAME "Science_List"
#define DEFENSE_LIST_NAME "Defense_List"
#define MISCELLANEOUS_LIST_NAME "Miscellaneous_List"
#define END_GAME_OBJECT_LIST_NAME "End_Game_Object_List"
#define CAPITALIZATION_LIST_NAME "Capitalization_List"

#define ADVANCEMENTS_LIST_NAME "Advancements_List"







class Advancement_List_Element
{
public:

	sint32 advancement_ID;	

    Advancement_List_Element(); 
    Advancement_List_Element(CivArchive &archive);
    void Serialize(CivArchive &archive);
};













class Build_List_Element
{
public:
	double units_per_city;				

	sint32 unittype_ID;	

    Build_List_Element(); 
    Build_List_Element(CivArchive &archive);
    void Serialize(CivArchive &archive);
};














class Build_List_Class
{
public:

	double priority;						
	sint32 unit_type_count;				
	semi_dynamic_array<Build_List_Element> * unit_type_list;

    Build_List_Class(); 
    Build_List_Class(CivArchive &archive); 
    void Serialize(CivArchive &archive); 
};







#define IS_NORMAL 0
#define IS_WONDER (-1)
#define IS_END_GAME_OBJECT (-2)
#define IS_CAPITALIZATION (-3)







class Building_Build_List_Element
{
public:
	double priority;					

	sint32 building_type_ID;			
	sint32 is_wonder;					
	sint32 is_end_game_object;			
	sint32 is_capitalization;			
	sint32 type;						

    Building_Build_List_Element(); 
    Building_Build_List_Element(CivArchive &archive);
    void Serialize(CivArchive &archive);
};












class AIP_Flat_Data
{
public:

	
	int AIP_ID;

	
	double build_troops_priority;
	double build_buildings_priority;
	double build_end_game_object_priority;
	double build_capitalization_priority;
	double seige_priority; 
	double explore_priority; 
	double attack_troops_priority; 
	double attack_region_priority;
	double settle_priority;
	double defend_priority;        
	double perimeter_priority;
	double patrol_priority;
	double chokepoint_priority;
	double harass_priority;
	double bombard_priority;
	double rustle_priority;
	double expel_priority;
	double sally_priority;
	double retreat_priority;
	double convert_priority;
    double bioterror_priority;
    double nanoattack_priority;
	double pillage_priority;
	double pirate_priority;
	double counter_stealth_priority;
	double bonus_food_priority;
	double goody_hut_priority;
	double defuse_mines_priority;
	double enslave_priority;
	double plant_nuke_priority;
	double create_park_priority;
	double underground_railway_priority;
	double establish_embassy_priority;
	double franchising_priority;
	double assasinate_ruler_priority;
	double steal_technology_priority;
	double injoin_priority;
	double incite_revolution_priority;
	double cause_unhappiness_priority;
	double nuke_city_priority;
	double wander_priority;
	double reform_city_priority;
	double sue_franchise_priority;
	double probe_wormhole_priority;


	
	double max_danger_raw_bonus;
	double danger_diminishment;
	double max_threat_raw_bonus; 
	double distance_from_home_priority_modifier;
	double distance_from_enemy_priority_modifier; 
	double max_chokepoint_raw_bonus; 
	double max_power_raw_bonus; 
	double max_value_raw_bonus; 
	double random_modifier;
	double per_capita_modifier;
	double threat_proximity_modifier;
	double human_target_raw_bonus;
	double defenseless_city_raw_bonus;

	
	double max_misc_bid_bonus; 

	double fzbuild_priority_modifier;
	double fzwonder_priority_modifier;
	double fzfreight_priority_modifier;
	double persistence_modifier;
	double distance_from_unit_priority_modifier;
	double better_unit_type_modifier;

	
	int wander_rounds_from_target;
	double wander_density;

	
	int captured_city_defend_turns;
	double captured_city_modifier;

	
	int patrol_one_in_n;

    
    int memory_rounds;
    int memory_rounds_stealth;

	
	double min_attack_matching_force_ratio;
	double min_defense_matching_force_ratio;
	int num_city_defenders;

	
	double Wonders_List_Threshhold;
	double Transportation_List_Threshhold;
	double Growth_List_Threshhold;
	double Happiness_List_Threshhold;
	double Production_List_Threshhold;
	double Gold_List_Threshhold;
	double Science_List_Threshhold;
	double Defense_List_Threshhold;
	double Miscellaneous_List_Threshhold;
	double End_Game_Object_List_Threshhold;
	double Capitalization_List_Threshhold;

	
	double max_eval_goals_of_type_per_city[GOAL_TYPE_MAX];
	double max_execute_goals_of_type_per_city[GOAL_TYPE_MAX];

	
	int alert_to_aip_load;

	
	int max_eval_goals_of_type[GOAL_TYPE_MAX];

	
	int max_execute_goals_of_type[GOAL_TYPE_MAX];

	
	int max_build_building_rounds;
	int max_build_unit_rounds;
	int max_build_wonder_rounds;

	
	int loaded_first_AIP;

	
	int AI_Verbosity;

	

	
	Flag AI_Verbosity_loaded_flag : 1;
	Flag build_troops_priority_loaded_flag : 1;
	Flag build_buildings_priority_loaded_flag : 1;
	Flag build_end_game_object_priority_loaded_flag : 1;
	Flag build_capitalization_priority_loaded_flag : 1;
	Flag seige_priority_loaded_flag : 1; 
	Flag explore_priority_loaded_flag : 1; 
	Flag attack_troops_priority_loaded_flag : 1; 
	Flag attack_region_priority_loaded_flag : 1;
	Flag settle_priority_loaded_flag : 1;
	Flag defend_priority_loaded_flag : 1;        
	Flag perimeter_priority_loaded_flag : 1;
	Flag patrol_priority_loaded_flag : 1;
	Flag chokepoint_priority_loaded_flag : 1;
	Flag harass_priority_loaded_flag : 1;
	Flag bombard_priority_loaded_flag : 1;
	Flag rustle_priority_loaded_flag : 1;
	Flag expel_priority_loaded_flag : 1;
	Flag sally_priority_loaded_flag : 1;
	Flag retreat_priority_loaded_flag : 1;
	Flag convert_priority_loaded_flag : 1;
    Flag bioterror_priority_loaded_flag : 1;
    Flag nanoattack_priority_loaded_flag : 1;
	Flag pillage_priority_loaded_flag : 1;
	Flag pirate_priority_loaded_flag : 1;
	Flag counter_stealth_priority_loaded_flag : 1;
	Flag bonus_food_priority_loaded_flag : 1;
	Flag goody_hut_priority_loaded_flag : 1;
	Flag defuse_mines_priority_loaded_flag : 1;
	Flag enslave_priority_loaded_flag : 1;
	Flag plant_nuke_priority_loaded_flag : 1;
	Flag create_park_priority_loaded_flag : 1;
	Flag underground_railway_priority_loaded_flag : 1;
	Flag establish_embassy_priority_loaded_flag : 1;
	Flag franchising_priority_loaded_flag : 1;
	Flag assasinate_ruler_priority_loaded_flag : 1;
	Flag steal_technology_priority_loaded_flag : 1;
	Flag injoin_priority_loaded_flag : 1;
	Flag incite_revolution_priority_loaded_flag : 1;
	Flag cause_unhappiness_priority_loaded_flag : 1;
	Flag nuke_city_priority_loaded_flag : 1;
	Flag wander_priority_loaded_flag : 1;
	Flag reform_city_priority_loaded_flag : 1;
	Flag sue_franchise_priority_loaded_flag : 1;
	Flag probe_wormhole_priority_loaded_flag : 1;


	
	Flag max_danger_raw_bonus_loaded_flag : 1;
	Flag danger_diminishment_loaded_flag : 1;
	Flag max_threat_raw_bonus_loaded_flag : 1; 
	Flag distance_from_home_priority_modifier_loaded_flag : 1;
	Flag distance_from_enemy_priority_modifier_loaded_flag : 1; 
	Flag max_chokepoint_raw_bonus_loaded_flag : 1; 
	Flag max_power_raw_bonus_loaded_flag : 1; 
	Flag max_value_raw_bonus_loaded_flag : 1; 
	Flag random_modifier_loaded_flag : 1;
	Flag per_capita_modifier_loaded_flag : 1;
	Flag threat_proximity_modifier_loaded_flag : 1;
	Flag human_target_raw_bonus_loaded_flag : 1;
	Flag defenseless_city_raw_bonus_loaded_flag : 1;

	
	Flag max_misc_bid_bonus_loaded_flag : 1; 

	Flag fzbuild_priority_modifier_loaded_flag : 1;
	Flag fzwonder_priority_modifier_loaded_flag : 1;
	Flag fzfreight_priority_modifier_loaded_flag : 1;
	Flag persistence_modifier_loaded_flag : 1;
	Flag distance_from_unit_priority_modifier_loaded_flag : 1;
	Flag better_unit_type_modifier_loaded_flag : 1;

	
	Flag wander_rounds_from_target_loaded_flag : 1;
	Flag wander_density_loaded_flag : 1;

	
	Flag captured_city_defend_turns_loaded_flag : 1;
	Flag captured_city_modifier_loaded_flag : 1;

	
	Flag patrol_one_in_n_loaded_flag : 1;

    
    Flag memory_rounds_loaded_flag : 1;
    Flag memory_rounds_stealth_loaded_flag : 1;

	
	Flag min_attack_matching_force_ratio_loaded_flag : 1;
	Flag min_defense_matching_force_ratio_loaded_flag : 1;
	Flag num_city_defenders_loaded_flag : 1;

	
	Flag Wonders_List_Threshhold_loaded_flag : 1;
	Flag Transportation_List_Threshhold_loaded_flag : 1;
	Flag Growth_List_Threshhold_loaded_flag : 1;
	Flag Happiness_List_Threshhold_loaded_flag : 1;
	Flag Production_List_Threshhold_loaded_flag : 1;
	Flag Gold_List_Threshhold_loaded_flag : 1;
	Flag Science_List_Threshhold_loaded_flag : 1;
	Flag Defense_List_Threshhold_loaded_flag : 1;
	Flag Miscellaneous_List_Threshhold_loaded_flag : 1;
	Flag End_Game_Object_List_Threshhold_loaded_flag : 1;
	Flag Capitalization_List_Threshhold_loaded_flag : 1;

	
	bool max_eval_goals_of_type_per_city_loaded_flag[GOAL_TYPE_MAX];
	bool max_execute_goals_of_type_per_city_loaded_flag[GOAL_TYPE_MAX];

	
	Flag alert_to_aip_load_loaded_flag : 1;

	
	Flag max_build_building_rounds_loaded_flag : 1;
	Flag max_build_unit_rounds_loaded_flag : 1;
	Flag max_build_wonder_rounds_loaded_flag : 1;

	
	Flag build_list_classes_loaded_flag : 1;
	Flag Wonders_List_loaded_flag : 1;
	Flag Transportation_List_loaded_flag : 1;
	Flag Growth_List_loaded_flag : 1;
	Flag Happiness_List_loaded_flag : 1;
	Flag Production_List_loaded_flag : 1;
	Flag Gold_List_loaded_flag : 1;
	Flag Science_List_loaded_flag : 1;
	Flag Defense_List_loaded_flag : 1;
	Flag Miscellaneous_List_loaded_flag : 1;
	Flag Scientific_Advancements_List_loaded_flag : 1;
	Flag End_Game_Object_List_loaded_flag : 1;
	Flag Capitalization_List_loaded_flag : 1;

public:
	AIP_Flat_Data(); 

	void Serialize(CivArchive &archive); 
};






class AIP_Ptr_Data { 
protected:
    
	sint32 build_list_classes_count;		
	sint32 dont_delete_ptr_fields;		

	
	
	sint32 unit_type_count;

	
	semi_dynamic_array <Build_List_Class> * build_list_classes;

	
	semi_dynamic_array <double> * desired_units;

	

	semi_dynamic_array <sorted_array<double, double> *> * desired_unit_priority;

	
	semi_dynamic_array <int> * actual_units;

	
	
	Flag units_build_list_from_saved_game : 1;

	Flag Wonders_List_from_saved_game : 1;
	Flag Transportation_List_from_saved_game : 1;
	Flag Growth_List_from_saved_game : 1;
	Flag Happiness_List_from_saved_game : 1;
	Flag Production_List_from_saved_game : 1;
	Flag Gold_List_from_saved_game : 1;
	Flag Science_List_from_saved_game : 1;
	Flag Defense_List_from_saved_game : 1;
	Flag Miscellaneous_List_from_saved_game : 1;
	Flag End_Game_Object_List_from_saved_game : 1;
	Flag Capitalization_List_from_saved_game : 1;
	Flag Scientific_Advancements_List_from_saved_game : 1;


	
	semi_dynamic_array<Building_Build_List_Element> *Wonders_List;
	semi_dynamic_array<Building_Build_List_Element> *Transportation_List;
	semi_dynamic_array<Building_Build_List_Element> *Growth_List;
	semi_dynamic_array<Building_Build_List_Element> *Happiness_List;
	semi_dynamic_array<Building_Build_List_Element> *Production_List;
	semi_dynamic_array<Building_Build_List_Element> *Gold_List;
	semi_dynamic_array<Building_Build_List_Element> *Science_List;
	semi_dynamic_array<Building_Build_List_Element> *Defense_List;
	semi_dynamic_array<Building_Build_List_Element> *Miscellaneous_List;
	semi_dynamic_array<Building_Build_List_Element> *End_Game_Object_List;
	semi_dynamic_array<Building_Build_List_Element> *Capitalization_List;

	
	semi_dynamic_array<Advancement_List_Element> *Scientific_Advancements_List;

	AiMain *ai;							

public:


	char name[MAX_AIP_FILE_NAME];		


    AIP_Ptr_Data(); 
    ~AIP_Ptr_Data(); 

	void Delete_Build_Lists();

	void Load_Building_List(CivArchive &archive, 
		semi_dynamic_array<Building_Build_List_Element> **the_list);

	void Save_Building_List(CivArchive &archive, 
		semi_dynamic_array<Building_Build_List_Element> *the_list);

	void Load_Science_List(CivArchive &archive, 
		semi_dynamic_array<Advancement_List_Element> **the_list);

	void Save_Science_List(CivArchive &archive, 
		semi_dynamic_array<Advancement_List_Element> *the_list);

    void Serialize(CivArchive &archive);
};







class AIP : public AIP_Flat_Data, public AIP_Ptr_Data
{

private:

	
	
	
	
	
	
	void AIP::Load_Build_List_Class
	(
		iparser * AIP_parameters,			
		char * class_name,					
		int class_num						
	);



	
	
	
	
	
	
	void AIP::Load_Supplemental_Build_List_Classes
	(
		iparser * AIP_parameters				
	);



	
	
	
	
	
	
	
	
	bool AIP::Load_Single_Building_Build_List
	(
		iparser * AIP_parameters,			
		char *list_name,					
		semi_dynamic_array<Building_Build_List_Element> **the_list 
	);


	
	
	
	
	
	
	
	void AIP::Load_Building_Build_Lists
	(
		iparser * AIP_parameters				
	);


	
	
	
	
	
	
	
	void AIP::Load_Scientific_Advancement_List
	(
		iparser * AIP_parameters				
	);


	
	
	
	
	
	
	
	
	
	bool AIP::Load_Double_If_There
	(
		iparser * AIP_parameters,				
		char *parameter_name,					
		double &my_variable					
	);

	
	
	
	
	
	
	
	
	
	bool AIP::Load_Int_If_There
	(
		iparser * AIP_parameters,				
		char *parameter_name,					
		int &my_variable,						
		bool dont_warn = true					
	);

	
	
	
	
	
	
	
	void AIP::Load_Simple_AIP_Priorities
	(
		iparser * AIP_parameters				
	);

	
	
	
	
	
	
	
	void AIP::Switch_Simple_AIP_Priorities
	(
		AIP_ptr new_aip
	);



public:


	AIP(AiMain *init_ai, bool i_dont_delete_ptr_fields = false);
	AIP(AiMain *init_ai, CivArchive &archive);
	void Init(AiMain *init_ai);
	~AIP();

	enum BUILD_LIST_TYPE{
	BUILD_LIST_WONDER,
	BUILD_LIST_TRANSPORTATION,
	BUILD_LIST_GROWTH,
	BUILD_LIST_HAPPINESS,
	BUILD_LIST_PRODUCTION,
	BUILD_LIST_GOLD,
	BUILD_LIST_SCIENCE,
	BUILD_LIST_DEFENSE,
	BUILD_LIST_MISCELLANEOUS,
	BUILD_LIST_END_GAME_OBJECT,
	BUILD_LIST_CAPITALIZATION,
	BUILD_LIST_MAX
	};

	
	
	
	
	
	AIP_ptr Load_AIP_File
	(
		char * AIP_name,
		bool force_reload = false
	);

	
	
	
	
	
	
	
	void Switch_AIP
	(
		AIP_ptr new_aip
	);

	
	
	
	
	
	
    void Serialize(AiMain *init_ai, CivArchive &archive); 

	
	
	
	
	
	
	
	void Update_Max_Goals();


	
	
	
	
	
	
	void Update_Actual_Units();


	
	
	
	
	
	
	sint32 Get_Build_List_Classes_Count();


	
	
	
	
	
	
	sint32 Get_Best_Unit_From_List(const sint32 &build_list_class);


	
	
	
	
	
	
	double Choose_Unit_From_List
	(
		
		semi_dynamic_array<Build_List_Element> * unit_type_list,
		sint32 & build_this_unit		
	);


	
	
	
	
	
	
	
	void Update_Desired_Units();


	
	
	
	
	
	
	
	void Add_Supplemental_Unit_Build_Goals();

    
    
    BOOL FindMaxBuildGoal(sint32 &max_unit_type, double &max_build_priority, 
                           double *current_units );


	
	
	
	
	
	
	
	void Update_City_Distributions
	(
		sorted_array<CityAgent *, sint32> *production_rank,
		sorted_array<CityAgent *, sint32> *growth_rank,
		sorted_array<CityAgent *, sint32> *gold_rank,
		sorted_array<CityAgent *, sint32> *science_rank,
		sorted_array<CityAgent *, sint32> *happiness_rank,
		int num_cities						
	);


	
	
	
	
	
	
	
	void Update_City_Needs
	(
		sorted_array<CityAgent *, sint32> *production_rank,
		sorted_array<CityAgent *, sint32> *growth_rank,
		sorted_array<CityAgent *, sint32> *gold_rank,
		sorted_array<CityAgent *, sint32> *science_rank,
		sorted_array<CityAgent *, sint32> *happiness_rank,
		int num_cities						

	);


	
	
	
	
	
	
	
	void Add_Supplemental_Building_Build_Goal
	(
		CityAgent *the_city,
		semi_dynamic_array<Building_Build_List_Element> *the_list,
		double bid_coefficient,
		BUILD_LIST_TYPE which_list
	);


	
	
	
	
	
	
	void Update_Building_Build_Lists();



	
	
	
	
	
	
	void Update();


	
	
	
	
	
	
	
	
	sint32 Choose_Next_Science();

    void GetCurrentAIPname(char *str); 


};








#endif  

