//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : World events
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added GobalWarming and OzoneDepletion events. (29-Oct-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "World.h"
#include "Cell.h"
#include "Unit.h"
#include "directions.h"
#include "GameEventUser.h"
#include "Events.h"
#include "tiledmap.h"
#include "network.h"
#include "SlicObject.h"
#include "SlicEngine.h"
#include "cellunitlist.h"
#include "AICause.h"
#include "World.h"             // g_theWorld

STDEHANDLER(CutImprovementsEvent)
{
	MapPoint pos;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;

	MapPoint npos;

	g_theWorld->CutImprovements(pos);

	for(WORLD_DIRECTION d = NORTH; d < NOWHERE; d = (WORLD_DIRECTION)((sint32)d + 1))
	{
		if(pos.GetNeighborPosition(d, npos))
		{
			g_tiledMap->PostProcessTile(npos, g_theWorld->GetTileInfo(npos));
			g_tiledMap->TileChanged(npos);
		}
	}

	g_tiledMap->RedrawTile(&pos);

	if(g_network.IsHost())
	{
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

	for(i = units.Num() -1; i >= 0; i--)
	{
		sint32 num_killed = 0;
		if(!g_theWorld->CanEnter(pos, units[i].GetMovementType())
		&& !g_theWorld->HasCity(pos)
		){

			g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_KillUnit,
								   GEA_Unit, units[i],
								   GEA_Int, CAUSE_REMOVE_ARMY_ILLEGAL_CELL,
								   GEA_Player, -1,
								   GEA_End);

			num_killed++;
		}
		if (num_killed > 0)
		{
			SlicObject *so = new SlicObject("41IAPillageSuicide");
			so->AddRecipient(unitsOwner);
			g_slicEngine->Execute(so);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(GlobalWarmingEvent)
{
	sint32 phase;
	if(!args->GetInt(0, phase))
		return GEV_HD_Continue;

	g_theWorld->GlobalWarmingEvent(phase);

	return GEV_HD_Continue;
}

STDEHANDLER(OzoneDepletionEvent)
{
	g_theWorld->OzoneDepletionEvent();

	return GEV_HD_Continue;
}

void worldevent_Initialize()
{
	g_gevManager->AddCallback(GEV_CutImprovements, GEV_PRI_Primary, &s_CutImprovementsEvent);
	g_gevManager->AddCallback(GEV_GlobalWarming,   GEV_PRI_Primary, &s_GlobalWarmingEvent);
	g_gevManager->AddCallback(GEV_OzoneDepletion,  GEV_PRI_Primary, &s_OzoneDepletionEvent);
}

void worldevent_Cleanup()
{
}
