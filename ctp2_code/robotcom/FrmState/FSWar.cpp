#include "ctp/c3.h"
#include "user-robotcom/shared/globals.h"
#include "gs/outcom/IMapPointData.h"

#include "robotcom/FrmState/FSWar.h"
#include "robotcom/Top/aicell.h"
#include "robotcom/Top/AiMap.h"
#include "user-robotcom/outcom/ic3GameState.h"
#include "aimain.h"
#include "Foreigner.h"


#include "robotcom/Agent/ArmyAgent.h"
#include "robotcom/Agent/CityAgent.h"

#include "robotcom/backdoor/Wallclock.h"
extern Wall_Clock *g_wall_clock;


void FSBeginWar::Serialize(CivArchive &archive)
{
    return;
}

BOOL FSBeginWar::Execute(AiMain *ai, sint32 &branch, FILE *fout)
{

    return TRUE;
}


void FSUpdateThreat::Serialize(CivArchive &archive)
{
    return;
}

BOOL FSUpdateThreat::Execute(AiMain *ai, sint32 &branch,  FILE *fout)
{

    ai->m_map->GetVisibleForeigners(ai);


    CityAgent *ca;
    BSetID id;

    for (ca = ai->m_city_set->First(id); ai->m_city_set->Last(); ca = ai->m_city_set->Next(id)) {
        ca->UpdateThreat(ai);
    }

    return TRUE;
}
