






























































































































































#include "c3.h"
#include "c3types.h"
#include "c3errors.h"
#include "FlatPtr.h"
#include "CivArchive.h"

#include "AiMain.h"



#include "common.h"
#include "linked_list.h"
#include "Strategic_AI_Forward.h"
#include "Unitflow.h"
#include "Squad.h"
#include "Goal.h"
#include "Squad_Strength.h"

#define LOG_GOAL true

ZEROMEM(Goal_Flat_Data); 
FLATSERIALIZE(Goal_Flat_Data); 





























void Goal::Init_Goal()
{

	
    what_goal = GOAL_TYPE_NULL; 

	
	the_class = DEFAULT_GOAL_CLASS;

	marked_for_deletion = false;		







    my_squad_id = SAVE_NO_GOAL; 
	attacking_squad = NULL;				

	dont_allocate = false;				



	raw_priority = 0.0f;				



	removal_time = DONT_REMOVE;		

	invalid_goal = false;				

	new_goal_cell_ptr = NULL;			



	single_plan_only = false;			

	



	goal_ID = UNASSIGNED_ID;			

    m_min_needed_strength = new Squad_Strength(); 
    m_current_needed_strength = new Squad_Strength();
    m_current_attacking_strength = new Squad_Strength();  

    m_is_completed = FALSE; 

    m_farthest_agent_to_target_dist = 0;
}













Goal::Goal
(	


)
{
	
	Init_Goal();

}













Goal::Goal
(	
	AiMain *ai,							
	CivArchive &archive
)
{
	
	Init_Goal();



	


	
	Serialize(ai, archive);
}













Goal::~Goal()
{
	
	
	Set_Invalid();
	








    Assert(m_min_needed_strength); 
    delete m_min_needed_strength; 
    m_min_needed_strength = NULL; 

    Assert(m_current_needed_strength); 
    delete m_current_needed_strength;
    m_current_needed_strength = NULL; 

    Assert(m_current_attacking_strength); 
    delete m_current_attacking_strength;
    m_current_attacking_strength = NULL; 

}













void Goal::Load(AiMain *ai,CivArchive &archive)
{

	


	archive >> my_squad_id;

	



		


}













void Goal::Store(AiMain *ai,CivArchive &archive)
{
#ifdef _DEBUG
    if (SAVE_NO_GOAL == my_squad_id) { 
        Assert(NULL == attacking_squad); 
    } else { 
        Assert(attacking_squad); 
    } 
#endif

    archive << my_squad_id; 
}













void Goal::Serialize(AiMain *ai,CivArchive &archive)
{
	CHECKSERIALIZE

	Goal_Flat_Data::Serialize(archive); 

	
	if (archive.IsStoring())
	{
		
		Store(ai, archive);

	} 
	else
	{
		
		Load(ai, archive);

	} 


    Assert(m_min_needed_strength); 
    m_min_needed_strength->Serialize(archive); 

    Assert(m_current_needed_strength); 
    m_current_needed_strength->Serialize(archive);

    Assert(m_current_attacking_strength); 
    m_current_attacking_strength->Serialize(archive);  

}





BOOL Goal::Validate(AiMain *ai)
{

    switch (what_goal) {
    case GOAL_TYPE_EXPLORE: 
	case GOAL_TYPE_ATTACK_REGION:
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
	case GOAL_TYPE_PATROL:
	case GOAL_TYPE_CHOKEPOINT:
	case GOAL_TYPE_HURT_ARMY: 
	case GOAL_TYPE_CONVERT_CITY:
    case GOAL_TYPE_BIOTERROR_CITY:
    case GOAL_TYPE_NANOATTACK_CITY:
	case GOAL_TYPE_HARASS_CITY:
	case GOAL_TYPE_BOMBARD:
	case GOAL_TYPE_BUILD_LIST:
	case GOAL_TYPE_RUSTLE:
	case GOAL_TYPE_EXPEL:
	case GOAL_TYPE_SALLY:

	case GOAL_TYPE_PILLAGE:
	case GOAL_TYPE_PIRATE:
	case GOAL_TYPE_COUNTER_STEALTH:
	case GOAL_TYPE_BONUS_FOOD:
	case GOAL_TYPE_GOODY_HUT:
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
	case GOAL_TYPE_RETREAT:
	case GOAL_TYPE_NUKE_CITY:
	case GOAL_TYPE_WANDER:
        break; 
    default:
        Assert(FALSE); 
        return FALSE; 
    } 

    if (goal_ID < 0) { 
        Assert(FALSE); 
        return FALSE; 
    } 

    Assert(m_min_needed_strength); 
    Assert(m_current_needed_strength);
    Assert(m_current_attacking_strength);

    








    return TRUE; 
}


void Goal::HookUp(AiMain *ai)
{
    return; 
}









int Goal::Is_Goal_Undercommitted()
{
	return !m_current_needed_strength->IsEnough(m_current_attacking_strength, m_farthest_agent_to_target_dist); 
}














void Goal::Display_Goal_Details(AiMain *ai)
{
}





void Goal::Set_Invalid()
{
	invalid_goal = true;

	
	if (attacking_squad != NULL)
	{
		
		attacking_squad->Detach_Goal();
		attacking_squad = NULL;
	} 
}


void Goal::Detach_Squad()
{
	Squad *tmp_squad;
	if (attacking_squad != NULL)
		{
			tmp_squad = attacking_squad;
			attacking_squad = NULL;
			
			tmp_squad->Detach_Goal();
			my_squad_id = SAVE_NO_GOAL; 
			
			m_current_attacking_strength->
				Subtract(m_current_attacking_strength);
		}
}

void Goal::CleanUp()
{
}


void Goal::EstimateRecruitmentStrength(AiMain *ai, Agent *donor_agent, 
                                       double &str_score)
{
	
	Assert(FALSE);
}



BOOL Goal::CanBeExecuted() const  
{
    if (GOAL_TYPE_NULL == what_goal)				
        return FALSE; 

	if (dont_allocate)					
        return FALSE; 

	if (invalid_goal)					
        return FALSE; 

    if (marked_for_deletion)			
        return FALSE;

    if (m_is_completed) 
       return FALSE;


    return TRUE;
}
