












#include "c3.h"
#include "Globals.h"
#include "TradeAstar.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Player.h"



extern World *g_theWorld; 

TradeAstar g_theTradeAstar;
extern Player **g_player; 


sint32 TradeAstar::EntryCost(const MapPoint &prev, const MapPoint &pos,                           
     float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry) 

{
	is_zoc = FALSE;
	entry = ASTAR_CAN_ENTER;

    if (g_player[m_owner]->IsExplored(pos) == FALSE) { 
        cost = k_ASTAR_BIG; 
		entry = ASTAR_BLOCKED; 
        return FALSE;
    }
	
	if(!g_theWorld->IsXwrap()) {
		sint16 w = (sint16)g_theWorld->GetXWidth();

		sint32 adjX1 = (prev.x + (prev.y / 2)) % w;
		sint32 adjX2 = (pos.x  + (pos.y  / 2)) % w;

		if((adjX1 == 0 && adjX2 == (w - 1)) ||
		   (adjX2 == 0 && adjX1 == (w - 1))) {
			cost = k_ASTAR_BIG;
			entry = ASTAR_BLOCKED;
			return FALSE;
		}
	}

	cost = float(ceil(g_theWorld->CalcTerrainFreightCost(pos))); 
	return TRUE; 
}


sint32 TradeAstar::GetMaxDir(MapPoint &pos) const
{
    return SOUTH; 
}

sint32 TradeAstar::FindPath(const PLAYER_INDEX owner, const MapPoint &start, const MapPoint &dest, 
                      Path &a_path, float &total_cost, const sint32 isunit)

{
    sint32 cutoff = 2000000000; 
    sint32 nodes_opened=0;
    m_owner = owner; 

    return Astar::FindPath(start, dest, a_path, total_cost, isunit, 
            cutoff, nodes_opened);

}

