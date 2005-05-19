

























































































































































































































































#include "c3.h"
#include "globals.h"
#include "c3types.h"
#include "c3errors.h"
#include "FlatPtr.h"
#include "civarchive.h"
#include "ic3world.h"

#include "dr_debug.h"
#include "aimain.h"


#include "ArmyAgent.h"
#include "CityAgent.h"
#include "InstBidList.h"


#include "common.h"


#include "linked_list.h"		
#include "semi_dynamic_array.h"
#include "sorted_array.h"
#include "list_array.h"
#include "matrix.h"
#include "grassfire.h"
#include "pool.h"
#include "pointer_pool.h"

#include "unitflow.h"
#include "aip.h"
#include "squad.h"


#include "goal.h"
#include "ArmyGoal.h"
#include "GoalDefense.h"

#include "GlSettl.h"
#include "GlExpl.h"
#include "GlAttck.h"
#include "GlEnslave.h"
#include "glblg.h"
#include "glfreight.h"

#include "GlTrans.h"
#include "goal_build_supplemental.h"
#include "Goal_Perimeter.h"
#include "Goal_Patrol.h"
#include "goalbombard.h"
#include "Goal_Build_List.h"
#include "goalrustle.h"
#include "GoalExpel.h"
#include "GoalSally.h"
#include "GoalRetreat.h"
#include "GoalMapTarget.h"
#include "GoalCityTarget.h"
#include "Goal_Attack_Region.h"

#include "Scheduler.h"


#include "strategic_map.h"
#include "planner.h"


extern Pool<Squad> * g_squad_pool; 

#include "ailog.h"

#define LOG_PLANNER true



ZEROMEM(Planner_Flat_Data); 
FLATSERIALIZE(Planner_Flat_Data);  






















Planner::Planner
(
	AiMain *init_ai,					
	int i_team_ID						
)  : the_AIP(init_ai, true), my_goals(INITAL_GOAL_LIST_SIZE)
{
	
	team_ID = i_team_ID;

	
	my_squads = new list_array<Squad *> (SQUAD_ARRAY_BASE_SIZE);

	
	
	ai = init_ai;






	
	the_scheduler = new Scheduler(init_ai, this);

	
	the_strategic_map = new Strategic_Map(init_ai);

	
	
	current_squad_ID = 0;

	
	aicontrol = true;

	
	m_usableTransport = 0;

	
	Init();

#ifdef LOG_PLANNER
#ifdef DBGAILOG

	
	
	
	
	int *test_ptrs[100];
	int *test_me;
	int where_am_i;
	int int_index[100];
	int z;
	Pool<int> int_pool(5, -1);

	for (z = 0; z < 100; z++)
	{
		test_ptrs[z] = int_pool.Get_Next_Pointer(int_index[z]);

		
		test_me = test_ptrs[z];

		*(test_ptrs[z]) = 100-z;
	}
	
	
	int_pool.Release_Pointer(3);

	
	test_me = int_pool.Get_Next_Pointer(where_am_i);

	*test_me = 999;

	
	test_me = int_pool.Get_Next_Pointer(where_am_i);

	*test_me = 333;

	
	test_me = int_pool.Get_Next_Pointer(where_am_i);

	*test_me = 777;

	
	
	
	Pointer_Pool<int> int_pool2(5, -1);

	for (z = 0; z < 100; z++)
	{
		test_ptrs[z] = int_pool2.Get_Next_Pointer();

		
		test_me = test_ptrs[z];

		*(test_ptrs[z]) = 100-z;
	}
	
	
	int_pool2.Release_Pointer(test_ptrs[3]);

	
	test_me = int_pool2.Get_Next_Pointer();

	*test_me = 999;

	
	test_me = int_pool2.Get_Next_Pointer();

	*test_me = 333;

	
	test_me = int_pool2.Get_Next_Pointer();

	*test_me = 777;

	
	
	
	matrix<int> a_matrix(5, 5);
	matrix<int> grassfire_output(5, 5);
	Grassfire<int> dist_to_spot(5, 5);

	int r, c;

	for (r = 0; r < 5; r++) for (c = 0; c < 5; c++)
		a_matrix(r,c) = 0;

	a_matrix(1,3) = 3;

	dist_to_spot.Xform(&a_matrix, &grassfire_output, 3, 		
		DISTANCE_OUTSIDE, 
		BORDER_IS_NOT_TARGET,
		EIGHT_CONNECTED,
		ai->m_world->IsXwrap(), ai->m_world->IsYwrap()
		);

	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "\n\nGRASSFIRE TEST 1: 8 connected, No Border\n\n"));

	for (r = 0; r < 5; r++) 
	{
		for (c = 0; c < 5; c++)
		{
			if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
				AILOG((wstr, " %d ", grassfire_output(r, c) ));
		}
		if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			AILOG((wstr, "\n"));
	}

	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "\n\nGRASSFIRE TEST 2: 8 connected, Border\n\n"));

	dist_to_spot.Xform(&a_matrix, &grassfire_output, 3, 		
		DISTANCE_OUTSIDE, 
		BORDER_IS_TARGET,
		EIGHT_CONNECTED, false, false
		);

	for (r = 0; r < 5; r++) 
	{
		for (c = 0; c < 5; c++)
		{
			if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
				AILOG((wstr, " %d ", grassfire_output(r, c) ));
		}
		if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			AILOG((wstr, "\n"));
	}

	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "\n\nGRASSFIRE TEST 2: 4 connected, no border\n\n"));

	dist_to_spot.Xform(&a_matrix, &grassfire_output, 3, 		
		DISTANCE_OUTSIDE, 
		BORDER_IS_NOT_TARGET,
		FOUR_CONNECTED, 
		ai->m_world->IsXwrap(), ai->m_world->IsYwrap()
		);

	for (r = 0; r < 5; r++) 
	{
		for (c = 0; c < 5; c++)
		{
			if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
				AILOG((wstr, " %d ", grassfire_output(r, c) ));
		}
		if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
			AILOG((wstr, "\n"));
	}

	if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
		AILOG((wstr, "\n\n"));

#endif DBGAILOG

#endif

}













Planner::Planner
(
	AiMain *init_ai,				
	CivArchive &archive				
) : the_AIP(init_ai, true), my_goals(INITAL_GOAL_LIST_SIZE)
{

    ZeroInit(); 

	
	my_squads = new list_array<Squad *> (SQUAD_ARRAY_BASE_SIZE);


	
	ai = init_ai;

	
	
	ai->m_planner = this;

	
    Serialize(init_ai, archive); 

	
	Init();
}













void Planner::Init()
{
	
	

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFAULT_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SETTLER_SQUAD, SETTLER_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SETTLER_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SETTLER_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, CITY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, BOMBARD_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, SEIGE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, PROTECT_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, DEFENSE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, SETTLER_GOAL_CLASS);
	
	

	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, CITY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(CITY_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(EXPLORE_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(EXPLORE_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, BOMBARD_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, SEIGE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, PROTECT_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, DEFENSE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BOMBARD_SQUAD, DEFAULT_GOAL_CLASS);	

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SEIGE_SQUAD, SEIGE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SEIGE_SQUAD, PROTECT_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SEIGE_SQUAD, DEFENSE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SEIGE_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SEIGE_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SEIGE_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(PROTECT_SQUAD, PROTECT_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(PROTECT_SQUAD, DEFENSE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(PROTECT_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(PROTECT_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(PROTECT_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFENSE_SQUAD, DEFENSE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFENSE_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFENSE_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFENSE_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(MILITARY_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(MILITARY_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(MILITARY_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SLAVER_SQUAD, SLAVER_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SLAVER_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(SLAVER_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(GOD_SQUAD, GOD_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(GOD_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(GOD_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(ESPIONAGE_SQUAD, ESPIONAGE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(ESPIONAGE_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(ESPIONAGE_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFUSE_MINES_SQUAD, DEFUSE_MINES_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFUSE_MINES_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BONUS_FOOD_SQUAD, BONUS_FOOD_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BONUS_FOOD_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(MEDIA_SQUAD, MEDIA_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(MEDIA_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(MEDIA_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DIPLOMATIC_SQUAD, DIPLOMATIC_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DIPLOMATIC_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DIPLOMATIC_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BUSINESS_SQUAD, BUSINESS_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BUSINESS_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(BUSINESS_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(LAW_SQUAD, MILITARY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(LAW_SQUAD, LAW_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(LAW_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(LAW_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(ABOLITIONIST_SQUAD, ABOLITIONIST_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(ABOLITIONIST_SQUAD, EXPLORE_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(ABOLITIONIST_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DESTROY_CITY_SQUAD, DESTROY_CITY_GOAL_CLASS);
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DESTROY_CITY_SQUAD, DEFAULT_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(TRANSPORT_HACK_SQUAD, TRANSPORT_HACK_GOAL_CLASS);

	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(WORMHOLE_PROBE_SQUAD, WORMHOLE_PROBE_GOAL_CLASS);
	
	
	the_scheduler->Associate_Squad_Class_With_Goal_Class(DEFAULT_SQUAD, DEFAULT_GOAL_CLASS);
}









void Planner::ZeroInit()
{
	ai = NULL;	

	the_scheduler = NULL;			
	the_strategic_map = NULL;
}







void Planner::Clean_Up()
{
	
	int i;								
	squad_ptr the_squad;				
	Goal *the_goal;						
	

	
	the_scheduler->Clean_Up();

	
	
	my_agents.Empty();

	
	
	for (i = my_squads->count - 1; i >= 0; i--)
	{
		
		the_squad = (squad_ptr)
			my_squads->Return_Data_By_Number( i );

		
		Assert(the_squad);

		
	    the_squad->my_strategic_goal = NULL;


		
		the_squad->Clean_Up();

		
		g_squad_pool->Release_Pointer(the_squad->squad_pool_index);

	} 

	
	my_squads->Empty();

	delete my_squads;
	my_squads = NULL;

	
	
	for (i = my_goals.count - 1; i >= 0; i--)
	{
		
		the_goal = (Goal *)
			my_goals.Return_Data_By_Number( i );

		
		Assert(the_goal);

		
        the_goal->attacking_squad = NULL; 
		delete the_goal;
        the_goal = NULL; 

	} 

	
	my_goals.Empty();

}













Planner::~Planner()
{
	
	Clean_Up();

	


	
	delete the_scheduler;
    the_scheduler = NULL; 

	
	delete the_strategic_map;
    the_strategic_map = NULL; 

}









void Planner::LoadMyAgents(AiMain *init_ai, CivArchive &archive)
{
    	
    sint32 i; 
    sint32 num_agents; 
    uint32 id; 
    AGENT_TYPE real_type;
    Agent *the_agent; 

	num_agents = archive.GetSINT32(); 
	
	for (i = 0; i < num_agents; i++)
	{
		
		id = archive.GetUINT32();

		real_type = AGENT_TYPE(archive.GetSINT32());
		
		switch (real_type){
		case AGENT_TYPE_ARMY: the_agent = init_ai->m_army_set->Find(id); break;
		case AGENT_TYPE_CITY: the_agent = init_ai->m_city_set->Find(id); break;
		default:
			
			INSANE(LOAD_AGENT_UNKOWN);
			break;
        }

        Assert(the_agent); 

		
		the_agent->agents_list_cell_ptr = my_agents.Add_Data((data_ptr) the_agent);

        the_agent->my_planner = this;
	} 


}

void Planner::StoreMyAgents(CivArchive &archive)
{
    sint32 i; 
    Agent* the_agent; 

    
	archive.PutSINT32(my_agents.count);
	

	for (i = my_agents.count - 1; i >= 0; i--)
	{
		
		the_agent = (Agent *)
			my_agents.Return_Data_By_Number( i );

		
		archive.PutUINT32(the_agent->GetID().GetVal());

        archive.PutSINT32(the_agent->GetType()); 
	} 
}


void Planner::LoadMyGoals(CivArchive &archive) 
{ 
    sint32 i; 
    sint32 num_goals;
    GOAL_TYPE the_goal_type;
    ArmyGoal *the_goal=NULL; 

#ifdef VALIDATE_ON
    my_goals.Validate(); 
#endif VALIDATE_ON

	
	archive >> num_goals; 

	for (i = 0; i < num_goals; i++)	{	
	
		the_goal_type = GOAL_TYPE(archive.GetSINT32());
        
		
		switch (the_goal_type) {
		case GOAL_TYPE_EXPLORE: the_goal = new GoalExplore(ai, archive); break;
		case GOAL_TYPE_ATTACK: the_goal = new GoalAttack(ai, archive); break;
        case GOAL_TYPE_DEFENSE: the_goal = new GoalDefense(ai, archive); break; 
        case GOAL_TYPE_SETTLE: the_goal = new GoalSettle(ai, archive); break;
        case GOAL_TYPE_ENSLAVE: the_goal = new GoalEnslave(ai, archive); break; 
		
        case GOAL_TYPE_CONSTRUCT_BUILDING: the_goal = new GoalBuilding(ai, archive); break;
        case GOAL_TYPE_CONSTRUCT_FREIGHT: the_goal = new GoalFreight(ai, archive); break; 
        
        
        case GOAL_TYPE_BUILD_SUPPLEMENTAL: the_goal = new Goal_Build_Supplemental(ai, archive); break; 
		case GOAL_TYPE_PERIMETER:the_goal = new Goal_Perimeter(ai, archive); break; 
		case GOAL_TYPE_ATTACK_REGION:the_goal = new Goal_Attack_Region(ai, archive); break; 
		case GOAL_TYPE_CHOKEPOINT:the_goal = new GoalMapTarget(ai, archive); break; 
		case GOAL_TYPE_PATROL:the_goal = new Goal_Patrol(ai, archive); break; 
		case GOAL_TYPE_BOMBARD: 
			the_goal = new GoalBombard(ai,archive); 
			break;
		case GOAL_TYPE_BUILD_LIST:
			the_goal = new Goal_Build_List(ai, archive); 
			break;
		case GOAL_TYPE_RUSTLE: 
			the_goal = new GoalRustle(ai,archive); 
			break;			
		case GOAL_TYPE_EXPEL: 
			the_goal = new GoalExpel(ai,archive); 
			break;			
		case GOAL_TYPE_SALLY: 
			the_goal = new GoalSally(ai,archive); 
			break;			

		case GOAL_TYPE_PILLAGE:
		case GOAL_TYPE_PIRATE:
		case GOAL_TYPE_COUNTER_STEALTH:
		case GOAL_TYPE_BONUS_FOOD:
		case GOAL_TYPE_GOODY_HUT:
		case GOAL_TYPE_WANDER:
		case GOAL_TYPE_TRANSPORT:
			the_goal = new GoalMapTarget(ai,archive); 
			break;			
		case GOAL_TYPE_RETREAT: 
			the_goal = new GoalRetreat(ai,archive); 
			break;			
		case GOAL_TYPE_SEIGE: 
		case GOAL_TYPE_CONVERT_CITY:
        case GOAL_TYPE_BIOTERROR_CITY:
        case GOAL_TYPE_NANOATTACK_CITY:
		case GOAL_TYPE_HARASS_CITY:
		case GOAL_TYPE_PLANT_NUKE:
		case GOAL_TYPE_MAKE_PARK:
		case GOAL_TYPE_CAUSE_UNHAPPINESS:
		case GOAL_TYPE_ESTABLISH_EMBASSY:
		case GOAL_TYPE_FRANCHISING:
		case GOAL_TYPE_STEAL_TECHNOLOGY:
		case GOAL_TYPE_INCITE_REVOLUTION:
		case GOAL_TYPE_INJOIN:
		case GOAL_TYPE_ASSASINATE_RULER:
		case GOAL_TYPE_UNDERGROUND_RAILWAY:
		case GOAL_TYPE_NUKE_CITY:
			the_goal = new GoalCityTarget(ai,archive); 
			break;	
		default:
			
			Assert(false);
			break;
		}

        Assert(the_goal); 
        Assert(the_goal->m_pos); 
        Assert(the_goal->m_XYpos); 
        
		

		my_goals.Add_Data( the_goal, the_goal->goal_ID);

	} 

#ifdef VALIDATE_ON
    my_goals.Validate(); 
#endif VALIDATE_ON

}

void Planner::StoreMyGoals(CivArchive &archive)
{ 
    sint32 i;
    ArmyGoal *the_goal=NULL; 

#ifdef VALIDATE_ON
    my_goals.Validate(); 
#endif VALIDATE_ON

	
	archive << (sint32) my_goals.count; 
	
	for (i = my_goals.count - 1; i >= 0; i--)
	{
		
		the_goal = (ArmyGoal *)
			my_goals.Return_Data_By_Number( i );

		
		
		
		archive.PutSINT32(the_goal->GetType());

		
		the_goal->Serialize(ai, archive);

	} 

#ifdef VALIDATE_ON
    my_goals.Validate(); 
#endif VALIDATE_ON

}


void Planner::LoadMySquads(CivArchive &archive)
{
	
    sint32 i;
	int squad_pool_index;				
    sint32 num_squads; 
    Squad *the_squad = NULL;
	
    num_squads = archive.GetSINT32(); 

	
	for (i = 0; i < num_squads; i++)
	{
		

		
		the_squad = g_squad_pool->Get_Next_Pointer(squad_pool_index);
		the_squad->Init(this);
		the_squad->squad_pool_index = squad_pool_index;

		
		the_squad->Serialize(archive);

        Assert(the_squad); 

		

		my_squads->Append_Data( the_squad);

	} 
}

void Planner::StoreMySquads(CivArchive &archive)
{ 
    sint32 i;
    Squad *the_squad=NULL; 

    	
	archive.PutSINT32(my_squads->count); 

	
	for (i = my_squads->count - 1; i >= 0; i--)	{
		
		the_squad = (squad_ptr)
			my_squads->Return_Data_By_Number( i );

		
		the_squad->Serialize(archive);

	} 
}


void Planner::HookSquadGoals()
{

    sint32 i; 

    
    Squad* the_squad=NULL; 
    for (i=0; i<my_squads->count; i++)  {
		
		the_squad = (Squad *)my_squads->Return_Data_By_Number( i );
        Assert(the_squad); 

        if (UNASSIGNED_ID < the_squad->my_goal_id) {  
            the_squad->my_strategic_goal = Look_Up_Goal_In_Load_Table(the_squad->my_goal_id);
            Assert(the_squad->my_strategic_goal);
        }        
    }


    
    Goal* the_goal=NULL; 
    for (i=0; i<my_goals.count; i++)  {
		
		the_goal = (Goal *)my_goals.Return_Data_By_Number( i );
        Assert(the_goal); 

        if (-1 < the_goal->my_squad_id) {  
            the_goal->attacking_squad = Look_Up_Squad_In_Load_Table(the_goal->my_squad_id);
            Assert(the_goal->attacking_squad); 
        }        
    }
}







void Planner::Serialize(AiMain *init_ai, CivArchive &archive)
{
    CHECKSERIALIZE
        
	
    Planner_Flat_Data::Serialize(archive); 

	
	the_AIP.Serialize(init_ai, archive);

	
    if (archive.IsStoring()){
        
        Assert(the_strategic_map); 

	    the_strategic_map->Serialize(archive);

        StoreMyAgents(archive); 
        StoreMySquads(archive); 
        StoreMyGoals(archive); 

		


        
        Assert(the_scheduler); 
	    the_scheduler->Serialize(archive);


	} 
	else
	{
        	    
	    the_strategic_map = new Strategic_Map(init_ai, archive);

        LoadMyAgents(init_ai, archive); 
        LoadMySquads(archive); 
        LoadMyGoals(archive); 




        
     
        
	    the_scheduler = new Scheduler(init_ai, this, archive);


        HookSquadGoals();

	} 
}





void Planner::HookUp(AiMain *ai)
{
    sint32 goal_idx; 
    Goal* the_goal=NULL; 

    for (goal_idx=0; goal_idx<my_goals.count; goal_idx++) { 
        the_goal = (Goal *) my_goals.Return_Data_By_Number( goal_idx );
        Assert(the_goal); 
        the_goal->HookUp(ai);
    } 
}








void Planner::Insert_Agent
(
	Agent *agent
)
{
	
	squad_ptr the_squad;				
	int squad_pool_index;				
    


	






	
	
	
	
	if (agent->agents_list_cell_ptr is NULL)
	{

#ifdef VALIDATE_ON
#ifdef _DEBUG 
		Agent *old_agent=NULL; 
		sint32 agent_idx; 

		for (agent_idx=0; agent_idx<my_agents.count; agent_idx++) 
		{ 
			old_agent = (Agent *) my_agents.Return_Data_By_Number(agent_idx);
			Assert(old_agent != agent); 
		}

#endif
#endif
		


		
		agent->agents_list_cell_ptr = my_agents.Add_Data((data_ptr) agent);

		
		agent->my_planner = this;

#ifdef DBGAILOG

#ifdef LOG_PLANNER
		
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG(( wstr, "PLANNER: Added agent %x\n", (int) agent));
#endif
#endif DBGAILOG
		
		if (agent->the_squad is NULL)
		{
			

			
			the_squad = g_squad_pool->Get_Next_Pointer(squad_pool_index);
			the_squad->Init(this);
			the_squad->squad_pool_index = squad_pool_index;

			
#ifdef DBGAILOG
#ifdef LOG_PLANNER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
				AILOG(( wstr, "Made new Squad %d\n", (int) the_squad));
#endif
#endif DBGAILOG
			
			the_squad->Add_Agent( ai, agent );

			
			Add_Squad(the_squad);

			
			
			if ((the_scheduler->subscheduling_inited) 
				and (not the_scheduler->troop_allocation_done))
			{
				
				the_scheduler->Add_New_Matches_For_Squad(the_squad);

			} 

		} 

	} 

}













void Planner::Remove_Agent
(
	Agent *agent
)
{
	
	if (agent->agents_list_cell_ptr isnt NULL)
	{
#ifdef DBGAILOG

#ifdef LOG_PLANNER
		
		if (ai->AI_VERBOSITY >= LOG_DETAILS)
			AILOG(( wstr, "\nPLANNER: Removed agent %x\n", (int) agent));
#endif
#endif DBGAILOG

		
		my_agents.Remove_Cell(agent->agents_list_cell_ptr);

		
		agent->agents_list_cell_ptr = NULL;

	} 

}













void Planner::Update_Map(AiMain *ai)
{
	
	the_strategic_map->Update_Death_Differential();

	
	the_strategic_map->Update_Our_Empire(ai);

	
	the_strategic_map->Update_Threats();

	
	the_strategic_map->Update_Enemy_Empire(ai);

	
	the_strategic_map->Update_Explored_Territory(ai);

	
	the_strategic_map->Update_Power_Map();

	
	the_strategic_map->Update_Value_Map();
}












void Planner::Update_Squads(AiMain *ai)
{
	
	int i;								
	Agent *an_agent;					
	ArmyAgent * the_army;				
	AGENT_TYPE  agent_type;				
	AGENT_STATE agent_state;			
	int dont_add_to_squad = false;		
	squad_ptr the_squad;				
	int squad_pool_index;				
	

	
	
	
	
	for (i=0; i < my_agents.count; i++)
	{
		
		dont_add_to_squad = false;

		
		an_agent = (Agent *) my_agents.Return_Data_By_Number(i);

		
		agent_type = an_agent->GetType();

		
        if (agent_type is AGENT_TYPE_ARMY)
		{
			
			the_army = (ArmyAgent *) an_agent;

			
			
			agent_state = the_army->GetState();

			
			switch (agent_state)
			{
			case AGENT_STATE_UNBUILT:
			case AGENT_STATE_BUILDING:
			case AGENT_STATE_MARKED_DEAD:
#ifdef DBGAILOG

#ifdef LOG_PLANNER
				if (ai->AI_VERBOSITY >= LOG_PAINFUL_DETAILS)
				{
					AILOG(( wstr, "Removing Unit\n"));
				}
#endif
#endif DBGAILOG

				
				if (an_agent->the_squad isnt NULL)
				{
					
					an_agent->the_squad->Remove_Agent( an_agent );

				} 

				
				dont_add_to_squad = true;
				break;
			default: break;
			} 
		
		} 

		
		
		if ((an_agent->the_squad is NULL) and (not dont_add_to_squad))
		{
			

			
			the_squad = g_squad_pool->Get_Next_Pointer(squad_pool_index);
			the_squad->Init(this);
			the_squad->squad_pool_index = squad_pool_index;

			
#ifdef DBGAILOG
#ifdef LOG_PLANNER
			if (ai->AI_VERBOSITY >= LOG_DETAILS)
				AILOG(( wstr, "Made new Squad %d\n", (int) the_squad));
#endif
#endif DBGAILOG
			
			the_squad->Add_Agent(ai, an_agent );

			
			Add_Squad(the_squad);

		} 

	} 

	
    for (sint32 squad_idx=0; squad_idx<my_squads->count; squad_idx++) 
	{ 
       the_squad = (Squad *) my_squads->Return_Data_By_Number(squad_idx);
	   the_squad->Recompute_Squad_Class(ai);
	}

}













void Planner::Update_Goals()
{
	

	

	
	the_scheduler->Update_Goals();

}













void Planner::Adjust_Death_Differential
(
	double adjustment,					
	MapPointData query_pos				
)
{
	
	if (the_strategic_map) {
		the_strategic_map->Adjust_Death_Differential (adjustment, query_pos);
	}
}













void Planner::Add_Squad(squad_ptr new_squad)
{
	ASSERT(new_squad);

#ifdef DBGAILOG
#ifdef LOG_PLANNER
	if (ai->AI_VERBOSITY >= LOG_DETAILS)
	{
		AILOG(( wstr, "PLANNER: Added Squad #%d\n", new_squad->squad_ID));
	}
#endif
#endif DBGAILOG

#ifdef VALIDATE_ON
#ifdef _DEBUG 
    Squad *old_squad=NULL; 
    sint32 squad_idx; 

    for (squad_idx=0; squad_idx<my_squads->count; squad_idx++) { 
       old_squad = (Squad *) my_squads->Return_Data_By_Number(squad_idx);
       Assert(old_squad != new_squad); 
    } 
#endif
#endif

    is_there_new_squads = TRUE;

	

	my_squads->Append_Data( new_squad);






}














squad_ptr Planner::Look_Up_Squad_In_Load_Table(int the_ID)
{
   Squad* the_squad=NULL;
   sint32 i; 

   
   

	for (i=0; i<my_squads->count; i++)	{
		the_squad = (squad_ptr)my_squads->Return_Data_By_Number(i);

        if (the_squad->GetID() == the_ID) { 
           return the_squad; 
        }          
    } 

    INSANE(LOAD_COULD_NOT_FIND_SQUAD); 
    return NULL; 
}














Goal_ptr Planner::Look_Up_Goal_In_Load_Table(int the_ID)
{

    Goal *the_goal; 
    sint32 i; 

   
   
	for (i = 0; i < my_goals.count; i++)	{
		the_goal = (Goal *) my_goals.Return_Data_By_Number( i );

		Assert(the_goal); 
        if (the_goal->GetID() == the_ID) { 
#ifdef _DEBUG
    switch (the_goal->what_goal) {
	case GOAL_TYPE_EXPLORE: 
	case GOAL_TYPE_ATTACK: 
    case GOAL_TYPE_DEFENSE: 
    case GOAL_TYPE_SETTLE: 
    case GOAL_TYPE_ENSLAVE: 

    case GOAL_TYPE_SEIGE: 
    case GOAL_TYPE_CONSTRUCT_BUILDING: 
    case GOAL_TYPE_CONSTRUCT_FREIGHT: 
    case GOAL_TYPE_CONSTRUCT_WONDER: 
    case GOAL_TYPE_TRANSPORT: 
    case GOAL_TYPE_BUILD_SUPPLEMENTAL: 
	case GOAL_TYPE_PERIMETER:
	case GOAL_TYPE_ATTACK_REGION:
	case GOAL_TYPE_CHOKEPOINT:
	case GOAL_TYPE_PATROL:
	case GOAL_TYPE_CONVERT_CITY:
    case GOAL_TYPE_BIOTERROR_CITY:
    case GOAL_TYPE_NANOATTACK_CITY:
	case GOAL_TYPE_HARASS_CITY:
	case GOAL_TYPE_BOMBARD:
	case GOAL_TYPE_BUILD_LIST:
	case GOAL_TYPE_RUSTLE:
	case GOAL_TYPE_EXPEL:
	case GOAL_TYPE_SALLY:
	case GOAL_TYPE_RETREAT:
	case GOAL_TYPE_WANDER:
		break; 
	default:
        INSANE(LOAD_GOAL_UNKNOWN); 
        break;
    } 
#endif
           return the_goal; 
        }          
    } 

    Assert(0);
    return NULL; 
}






void Planner::Add_Goal(Goal* new_goal)
{
    
Assert(new_goal);

#ifdef VALIDATE_ON
#ifdef _DEBUG 
    Goal *old_goal=NULL; 
    sint32 goal_idx; 

    for (goal_idx=0; goal_idx<my_goals.count; goal_idx++) { 
       old_goal = (Goal_ptr) my_goals.Return_Data_By_Number(goal_idx);
       Assert(old_goal != new_goal); 
    } 
#endif
#endif

    is_there_new_goals = TRUE; 
 
    the_scheduler->Add_New_Goal(new_goal); 
    new_goal->Classify_Goal(); 


	my_goals.Add_Data( new_goal, new_goal->goal_ID);

#ifdef VALIDATE_ON
    my_goals.Validate(); 
#endif VALIDATE_ON
}







void Planner::Add_Single_Plan_Goal(Goal* new_goal)
{
    
#ifdef VALIDATE_ON
#ifdef _DEBUG 
    Goal *old_goal=NULL; 
    sint32 goal_idx; 

    for (goal_idx=0; goal_idx<my_goals.count; goal_idx++) { 
       old_goal = (Goal_ptr) my_goals.Return_Data_By_Number(goal_idx);
       Assert(old_goal != new_goal); 
    } 
#endif
#endif

	new_goal->single_plan_only = true;

    the_scheduler->Assign_Unique_ID(new_goal); 
    new_goal->Classify_Goal(); 


	my_goals.Add_Data( new_goal, new_goal->goal_ID);

}














Goal_ptr Planner::Return_Goal_By_ID(int the_ID)
{
	int found = DATA_NOT_FOUND;
	Goal_ptr the_goal = NULL;

	
	found = my_goals.Return_Data_By_Unique_Key(the_goal, the_ID);

	
	Assert(found is DATA_FOUND);

	
	return the_goal;




















}





BOOL Planner::Validate(AiMain *ai)
{
    BOOL r = TRUE; 
#ifdef VALIDATE_ON
    sint32 i;
    Agent *the_agent=NULL;

	for (i = 0; i < my_agents.count; i++)
	{
		the_agent = (Agent*) my_agents.Return_Data_By_Number(i);
        r = the_agent->Validate(ai); 
        Assert(r); 
        if (!r) 
            return FALSE; 
	} 


    Goal *the_goal=NULL;

	for (i = 0; i < my_goals.count; i++)
	{
		the_goal = (Goal_ptr) my_goals.Return_Data_By_Number(i);
		Assert(the_goal);
        r = the_goal->Validate(ai); 
        Assert(r); 
        if (!r) 
            return FALSE; 
	} 

    Assert(the_scheduler); 
    r = the_scheduler->Validate(ai); 
#endif VALIDATE_ON
    return r; 
}

sint32 Planner::GetUsableTransport()
{
	return m_usableTransport;
}

void Planner::SetUsableTransport(const sint32 &count)
{
	m_usableTransport = count;
}


double Planner::GetUsableAttack()
{
	return m_usableAttack;
}

void Planner::SetUsableAttack(const double &attack)
{
	m_usableAttack = attack;
}

double Planner::GetUsableDefense()
{
	return m_usableDefense;
}

void Planner::SetUsableDefense(const double &defense)
{
	m_usableDefense = defense;
}








