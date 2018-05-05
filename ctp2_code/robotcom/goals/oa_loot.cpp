#include "ctp/c3.h"
#include "user-robotcom/shared/globals.h"
#include "robot/aibackdoor/bset.h"
#include "aimain.h"

#include "robotcom/Agent/ArmyAgent.h"
#include "robotcom/Agent/CityAgent.h"

#include "robotcom/goals/oa_loot.h"

#include "robotcom/Top/AiMap.h"
#include "user-robotcom/outcom/ic3BlgDB.h"
#include "user-robotcom/outcom/ic3player.h"

#include "user-robotcom/shared/order.h"







Opportunity_Action_Loot::Opportunity_Action_Loot ()
{
}







Opportunity_Action_Loot::~Opportunity_Action_Loot ()
{
}







void Opportunity_Action_Loot::Serialize (CivArchive &archive)
{
    return;
}









void Opportunity_Action_Loot::Execute(AiMain *ai)
{
	BSetID id;
    ArmyAgent *current_agent=NULL;
    CityAgent *ca;
	uint32 num_buildings;
	uint32 type;

	if (ai->m_my_player_id != PLAYER_INDEX_VANDALS)
		return;

	num_buildings = ai->m_buildingDB->GetNumBuildings();

	for (ca = ai->m_city_set->First(id);
		 ai->m_city_set->Last();
		 ca = ai->m_city_set->Next(id))
		{
			Assert(ca);
			sint32 city_id = ca->GetID().GetVal();









			for (type = 0; type< num_buildings; type++)
				ai->m_player->SellBuilding(city_id, type);
		}
}
