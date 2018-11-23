#include "c3.h"
#include "globals.h"
#include "bset.h"
#include "aimain.h"

#include "ArmyAgent.h"
#include "CityAgent.h"

#include "OA_Entrench.h"

#include "AiMap.h"

#include "order.h"







Opportunity_Action_Entrench::Opportunity_Action_Entrench ()
{
}







Opportunity_Action_Entrench::~Opportunity_Action_Entrench ()
{
}







void Opportunity_Action_Entrench::Serialize (CivArchive &archive)
{
    return;
}









void Opportunity_Action_Entrench::Execute(AiMain *ai)
{

    BSetID id;

    ArmyAgent *current_agent=NULL;

    sint32 army_num = ai->m_army_set->GetNum();
    sint32 army_idx=0;

    if (army_num < 1) return;

	for (current_agent = ai->m_army_set->First(id);
		 ai->m_army_set->Last();
		 current_agent = ai->m_army_set->Next(id))
		{
			Assert(current_agent);








			switch (current_agent->GetState())
				{
					case AGENT_STATE_MARKED_DEAD:
					case AGENT_STATE_UNBUILT:
					case AGENT_STATE_BUILDING:
						continue;
					case AGENT_STATE_AT_GOAL:
						current_agent->SetState(AGENT_STATE_LOOKING_FOR_WORK);
					default:
						current_agent->ActionTryToEntrench(ai);
				}
		}
}
