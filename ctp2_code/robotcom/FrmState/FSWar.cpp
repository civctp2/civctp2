#include "c3.h"
#include "globals.h"
#include "IMapPointData.h"

#include "FSWar.h"
#include "aicell.h"
#include "AiMap.h"
#include "ic3GameState.h"
#include "aimain.h"
#include "Foreigner.h"


#include "ArmyAgent.h"
#include "CityAgent.h"

#include "Wallclock.h"
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
