


#include "c3.h"
#include "civarchive.h"
#include "IMapPointData.h"


#include "ic3GameState.h"
#include "ic3BlgDB.h"
#include "aimain.h"


#include "Agent.h"
#include "CityAgent.h"


#include "glblg.h"

#include "ailog.h"

GoalBuildingFlat::~GoalBuildingFlat()
{ 
    return; 
} 

GoalBuilding::GoalBuilding()
{
	
	the_class = CITY_GOAL_CLASS;

    return; 
}

GoalBuilding::GoalBuilding
(
	
	const double v,		
	const sint32 bt, 
	const uint32 id
)
{
	
	the_class = CITY_GOAL_CLASS;

    
	removal_time = REMOVE_WHEN_SORTED;
	what_goal = GOAL_TYPE_CONSTRUCT_BUILDING;
	
    m_value = v; 
    m_building_type = bt; 
    m_home_id = id; 
}

GoalBuilding::GoalBuilding(AiMain *ai, CivArchive &archive)
{
	
	the_class = CITY_GOAL_CLASS;

    Serialize(ai, archive); 
}

GoalBuilding::~GoalBuilding() 
{
    return; 
}

void GoalBuildingFlat::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive.Store((uint8*) this, sizeof(*this)); 
    } else { 
        archive.Load((uint8*) this, sizeof(*this)); 
    } 
}

void GoalBuilding::Serialize(AiMain *ai, CivArchive &archive)
{
    CHECKSERIALIZE

    ArmyGoal::Serialize(ai, archive); 
    GoalBuildingFlat::Serialize(archive); 
    
}

BOOL GoalBuilding::Validate(AiMain *ai)
{
    return ArmyGoal::Validate(ai); 
}

GOAL_TYPE GoalBuilding::GetType () const
{
    return GOAL_TYPE_CONSTRUCT_BUILDING;
}







void GoalBuilding::Display_Goal_Type(AiMain *ai)
{
#ifdef DBGAILOG
	
	if (ai->AI_VERBOSITY >= LOG_MEDIUM)
		AILOG(( wstr,  "GOAL_TYPE_CONSTRUCT_BUILDING"));
#endif DBGAILOG
}













void GoalBuilding::Display_Goal_Details(AiMain *ai)
{
#ifdef DBGAILOG
	
	char * to_build_type_name;				
	

	if (ai->AI_VERBOSITY < LOG_DETAILS) return;

	
	to_build_type_name = ai->m_buildingDB->Improvementtype_Int_To_String(m_building_type);

	if (to_build_type_name != NULL)
		AILOG((wstr, "   %s ", to_build_type_name));
#endif DBGAILOG
}







sint32 GoalBuilding::GetBuildingType() const
{
    return m_building_type; 
}











int GoalBuilding::Is_Unit_Appropriate
(
	AiMain *ai,							
	Agent * unit_in_question			
)
{

	
	AGENT_TYPE  agent_type;				
	CityAgent * the_city;				
	

	
	agent_type = unit_in_question->GetType();

	
	if (agent_type == AGENT_TYPE_CITY)
	{
		
		the_city = (CityAgent *) unit_in_question;

		
		if (the_city->PretestContructBuilding(ai, m_building_type))
		{
			
			if (the_city->Already_Has_One(m_building_type))
				return false;
				
			
			return true; 
		} 

	} 

	
	return false;
}








Goal_Result GoalBuilding::BuildTaskSolution(AiMain *ai, CityAgent *the_city, Plan* the_plan)
{
 
    Assert(the_city);
    Assert(the_plan); 
    double utility; 

    if (ai->m_buildingDB->IsCapitolBuilding(m_building_type)) { 
        switch (ai->m_capitol_state) { 
        case CAPITOL_STATE_BUILDING: 
        case CAPITOL_STATE_HASIT:
            return GOAL_INAPPROPRIATE; 

        case CAPITOL_STATE_NONE: 
            Assert(NULL == ai->m_capitol_id); 
            utility = the_plan->GetMatchingValue(); 
            the_city->EnqueueBuildBuilding(ai, m_building_type, utility);

			
			the_city->current_goal_type = what_goal;

            ai->m_capitol_state = CAPITOL_STATE_BUILDING;
            ai->m_capitol_id = new BSetID();
            *ai->m_capitol_id = the_city->GetID(); 
            return GOAL_IN_PROGRESS; 
        default:
            INSANE(CAPITOL_STATE_UNKNOWN);
            return GOAL_INAPPROPRIATE;
        } 
    } else { 

        utility = the_plan->GetMatchingValue(); 
        the_city->EnqueueBuildBuilding(ai, m_building_type, utility);

		
		the_city->current_goal_type = what_goal;

        return GOAL_IN_PROGRESS; 
    }

}

Goal_Result GoalBuilding::CheckCompletion(const sint32 build_count)
{
    if (0 < build_count) {
        return GOAL_COMPLETE; 
    } else { 
        return GOAL_FAILED; 
    } 
}











double GoalBuilding::Compute_Raw_Priority(AiMain *ai)
{
	raw_priority = ai->m_planner->the_AIP.fzbuild_priority_modifier
		* (double) m_value;

	
	return raw_priority;
}




