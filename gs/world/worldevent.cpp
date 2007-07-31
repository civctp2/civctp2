
#include "c3.h"
#include "World.h"
#include "Cell.h"
#include "Unit.h"
#include "directions.h"
#include "GameEventUser.h"
#include "Events.h"
#include "TiledMap.h"
#include "network.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "CellUnitList.h"
#include "AICause.h"

STDEHANDLER(CutImprovementsEvent)
{
	MapPoint pos;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	MapPoint npos;

	g_theWorld->CutImprovements(pos);

	for(WORLD_DIRECTION d = NORTH; d < NOWHERE; d = (WORLD_DIRECTION)((sint32)d + 1)) {
		if(pos.GetNeighborPosition(d, npos)) {
			g_tiledMap->PostProcessTile(npos, g_theWorld->GetTileInfo(npos));
			g_tiledMap->TileChanged(npos);
		}
	}
	g_tiledMap->RedrawTile(&pos);

	if(g_network.IsHost()) {
		Cell *cell = g_theWorld->GetCell(pos);
		g_network.Enqueue(cell, pos.x, pos.y);
	}

	static CellUnitList units;
	units.Clear();
	g_theWorld->GetArmy(pos, units);

	sint32 i;
	sint32 unitsOwner = 0;
	if(units.Num() > 0) {
		unitsOwner = units.GetOwner();
	}

	for(i = units.Num() -1; i >= 0; i--) {
        sint32 num_killed = 0;
		if(!g_theWorld->CanEnter(pos, units[i].GetMovementType()) &&
		   g_theWorld->GetCity(pos).m_id == 0) {
			
			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillUnit,
								   GEA_Unit, units[i],
								   GEA_Int, CAUSE_REMOVE_ARMY_ILLEGAL_CELL,
								   GEA_Player, -1,
								   GEA_End);
			
            num_killed++;
		}
        if (num_killed > 0) {			
            SlicObject *so = new SlicObject("41IAPillageSuicide") ;
            so->AddRecipient(unitsOwner) ;
            g_slicEngine->Execute(so) ;
        }
	}

	return GEV_HD_Continue;
}

void worldevent_Initialize()
{
	g_gevManager->AddCallback(GEV_CutImprovements, GEV_PRI_Primary, &s_CutImprovementsEvent);
}

void worldevent_Cleanup()
{
}
