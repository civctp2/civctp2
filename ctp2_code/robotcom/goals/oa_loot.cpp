#include "c3.h"
#include "globals.h"
#include "bset.h"
#include "aimain.h"

#include "ArmyAgent.h"
#include "CityAgent.h"

#include "oa_loot.h"

#include "AiMap.h"
#include "ic3BlgDB.h"
#include "ic3player.h"

#include "order.h"







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
