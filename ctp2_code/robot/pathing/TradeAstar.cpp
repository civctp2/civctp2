//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : A-star Trade route pathing algorithm
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Globals.h"
#include "TradeAstar.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "player.h"



extern World *g_theWorld; 

TradeAstar g_theTradeAstar;
extern Player **g_player; 


bool TradeAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,                           
     float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry) 

{
	is_zoc = false;
	entry = ASTAR_CAN_ENTER;

    if (!g_player[m_owner]->IsExplored(pos)) { 
        cost = k_ASTAR_BIG; 
		entry = ASTAR_BLOCKED; 
        return false;
    }
	
	if(!g_theWorld->IsXwrap()) {
		sint16 w = (sint16)g_theWorld->GetXWidth();

		sint32 adjX1 = (prev.x + (prev.y / 2)) % w;
		sint32 adjX2 = (pos.x  + (pos.y  / 2)) % w;

		if((adjX1 == 0 && adjX2 == (w - 1)) ||
		   (adjX2 == 0 && adjX1 == (w - 1))) {
			cost = k_ASTAR_BIG;
			entry = ASTAR_BLOCKED;
			return false;
		}
	}

	cost = float(ceil(g_theWorld->CalcTerrainFreightCost(pos))); 
	return true;
}


sint32 TradeAstar::GetMaxDir(MapPoint &pos) const
{
    return SOUTH; 
}

bool TradeAstar::FindPath(const PLAYER_INDEX owner, const MapPoint &start, const MapPoint &dest, 
                      Path &a_path, float &total_cost, const bool isunit)

{
    sint32 cutoff = 2000000000; 
    sint32 nodes_opened=0;
    m_owner = owner; 

    return Astar::FindPath(start, dest, a_path, total_cost, isunit, 
            cutoff, nodes_opened);

}

