











#include "c3.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "ObjPool.h"
#include "QuadTree.h"
#include "UnitRec.h"
#include "Unit.h"
#include "Player.h"
#include "SelItem.h"
#include "installationtree.h"
#include "InstRec.h"
#include "UnitData.h"
#include "CellUnitList.h"
#include "network.h"
#include "WonderRecord.h"
#include "net_info.h"
#include "UnitPool.h"
#include "CityInfluenceIterator.h"

#include "wonderutil.h"

extern QuadTree<Unit> *g_theUnitTree;

extern BOOL player_isEnemy(PLAYER_INDEX me, PLAYER_INDEX him);
extern UnitPool *g_theUnitPool;




















sint32 World::InsertUnit (const MapPoint &pos, Unit &id, 
						  UnitDynamicArray &revealedUnits)

{ 
	Assert(!id.IsCity());
	g_theUnitTree->Insert(id);
	id.DoVision(revealedUnits);
	if(id.IsCity()) {
		GetCell(pos)->SetCity(id);
		return TRUE;
	} else {
		return GetCell(pos)->InsertUnit(id);
	}
}


sint32 World::RemoveUnitReference (const MapPoint &pos, const Unit &id)

{ 
	g_theUnitTree->Remove(id);

	
	

	return GetCell(pos)->RemoveUnitReference(id); 
}


sint32 World::GetEmptyTransports(const MapPoint pos, CellUnitList &transports)

{
   Cell *ptr = GetCell(pos); 

   sint32 i; 
   transports.Clear(); 

   sint32 n = ptr->GetNumUnits(); 
   for (i=0; i<n; i++) { 
		if (0 < ptr->AccessUnit(i).GetCargoCapacity()) { 
		   


















          transports.Insert(ptr->AccessUnit(i)); 
      } 
   }

   return transports.Num(); 
}


void World::GetArmy(const MapPoint &pos, CellUnitList &al)

{
   GetCell(pos)->GetArmy(al);
}

BOOL World::IsCellZoc(const PLAYER_INDEX &owner, const MapPoint &pos, 
                      const BOOL is_check_only_visible) 
{
    CellUnitList *a; 
    sint32 i; 

    if (is_check_only_visible) { 
        if (g_player[owner]->IsVisible(pos) == FALSE) { 
            return FALSE; 
        } 
    } 

    a = GetArmyPtr(pos); 

    if(!a)
		return FALSE;

	sint32 n = a->Num();
    if (n < 1) 
        return FALSE; 

    if (player_isEnemy(owner, a->GetOwner()) == FALSE) 
        return FALSE; 

	if(is_check_only_visible && !a->IsVisible(owner)) {
		
		return FALSE;
	}

    
    for (i=0; i<n; i++) { 
        if (!a->Get(i).IsNoZoc()) { 
			if(!is_check_only_visible || 
			   a->Get(i).GetVisibility() & (1 << owner)) {
				return TRUE;           
			}
        }
    } 
    return FALSE; 
} 

BOOL World::IsMoveZOC(PLAYER_INDEX owner, const MapPoint &start, 
                      const MapPoint &dest, const BOOL is_check_only_visible)
{ 
#if 1
	Cell *toCell = m_map[dest.x][dest.y];
	CellUnitList *toUnits = toCell->UnitArmy();
	if(toUnits) {
		if(toUnits->GetOwner() == owner &&
		   toUnits->ExertsZOC()) {
			
			return FALSE;
		} else if(toUnits->GetOwner() != owner) {
			
			return FALSE;
		}
	}
	if(toCell->GetCity().m_id != (0)) {
		
		return FALSE;
	}

	uint32 fromZoc = m_map[start.x][start.y]->m_zoc;
	uint32 ownerMask = (~(1 << owner)) & (~(g_player[owner]->mask_alliance));
	if(!(fromZoc & ownerMask)) {
		
		return FALSE;
	}
	
	uint32 toZoc = m_map[dest.x][dest.y]->m_zoc;
	if(!((toZoc & fromZoc) & ownerMask)) {
		
		return FALSE;
	}

	
	if((g_player[owner]->GetPlayerType() == PLAYER_TYPE_ROBOT && 
		!(g_network.IsClient() && g_network.IsLocalPlayer(owner))) ||
	   !is_check_only_visible) {
		
		return TRUE;
	}

	
	
	static UnitDynamicArray startUnits;
	static UnitDynamicArray destUnits;
	startUnits.Clear();
	destUnits.Clear();
	GetAdjacentUnits(startUnits, start);
	sint32 i;
	BOOL canSeeStartUnit = FALSE;
	for(i = 0; i < startUnits.Num(); i++) {
		if(startUnits[i].GetOwner() != owner &&
		   (startUnits[i].GetVisibility() & (1 << owner)) &&
		   !startUnits[i].IsNoZoc()) {
			canSeeStartUnit = TRUE;
			break;
		}
	}

    UnitDynamicArray city_list;
    if (!canSeeStartUnit) { 
        city_list.Clear();
        GetAdjacentCities(city_list, start);
	    for(i = 0; i < city_list.Num(); i++) {
		    if(city_list[i].GetOwner() != owner &&
		       (city_list[i].GetVisibility() & (1 << owner))){
			    canSeeStartUnit = TRUE;
			    break;
		    }
	    }
    }

	if(!canSeeStartUnit)
		return FALSE;

	GetAdjacentUnits(destUnits, dest);
	for(i = 0; i < destUnits.Num(); i++) {
		if(destUnits[i].GetOwner() != owner &&
		   (destUnits[i].GetVisibility() & (1 << owner)) &&
		   !destUnits[i].IsNoZoc())
			return TRUE;
	}


    city_list.Clear();
    GetAdjacentCities(city_list, dest);
	for(i = 0; i < city_list.Num(); i++) {
		if(city_list[i].GetOwner() != owner &&
		   (city_list[i].GetVisibility() & (1 << owner))){
            return TRUE; 
		}
	}


	return FALSE;

#else
    if (!start.IsNextTo(dest)) { 
        return FALSE;
    }

    WORLD_DIRECTION dir = start.GetNeighborDirection(dest); 
    static MapPoint left, right; 

    CellUnitList *a = GetArmyPtr(dest); 
    if (a && 0 < a->Num()) { 
        return FALSE;   
    }

    switch (dir) { 
    case NORTH:
        if(start.GetNeighborPosition(NORTHWEST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(NORTHEAST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
    case NORTHEAST:
        if(start.GetNeighborPosition(NORTH, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(NORTHWEST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}

        if(start.GetNeighborPosition(EAST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE;
		}
        if(start.GetNeighborPosition(SOUTHEAST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE;
		}
        break; 
    case EAST:
		if(start.GetNeighborPosition(NORTHEAST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(SOUTHEAST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
    case SOUTHEAST:
        if(start.GetNeighborPosition(NORTHEAST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(EAST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}

        if(start.GetNeighborPosition(SOUTH, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE;
		}
        if(start.GetNeighborPosition(SOUTHWEST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
    case SOUTH:
        if(start.GetNeighborPosition(SOUTHEAST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(SOUTHWEST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
    case SOUTHWEST:
        if(start.GetNeighborPosition(SOUTHEAST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(SOUTH, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}

        if(start.GetNeighborPosition(WEST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE;
		}
        if(start.GetNeighborPosition(NORTHWEST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
    case WEST:
        if(start.GetNeighborPosition(SOUTHWEST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(NORTHWEST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
    case NORTHWEST:
        if(start.GetNeighborPosition(SOUTHWEST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}
        if(start.GetNeighborPosition(WEST, right)) {
			if (IsCellZoc(owner, right, is_check_only_visible))
				return TRUE; 
		}

        if(start.GetNeighborPosition(NORTH, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE;
		}
		if(start.GetNeighborPosition(NORTHEAST, left)) {
			if (IsCellZoc(owner, left, is_check_only_visible))
				return TRUE; 
		}
        break; 
	case UP:
	case DOWN:
		return FALSE;
		break;
    default:
        Assert(0); 
    }

    return FALSE;
#endif
} 



































sint32 World::GetTopVisibleUnit (const MapPoint &pos, Unit &top) const 

{
    sint32 looking_player = g_selected_item->GetVisiblePlayer(); 
    return GetTopVisibleUnit (looking_player, pos, top, TRUE); 
}

sint32 World::GetTopVisibleUnit (const sint32 looking_player, const MapPoint &pos, Unit &top, BOOL includeCities) const 

{
    Cell *c; 

    top.m_id = (0); 
    c = GetCell(pos);

	if(includeCities) {
		if (c->GetCity().IsValid()) { 
			top = c->GetCity();
			return TRUE; 
		} 
	}

    sint32 n = c->GetNumUnits();
    if (n>0) { 
		
		
		PLAYER_INDEX s_player;
		ID s_item;
		SELECT_TYPE s_state;
		Army selectedArmy;
		g_selected_item->GetTopCurItem(s_player, s_item, s_state);
		if(s_state == SELECT_TYPE_LOCAL_ARMY &&
		   s_player == looking_player ) {
			selectedArmy = s_item;
			static MapPoint spos;
			selectedArmy.GetPos(spos);
			if(spos == pos) {
				top = selectedArmy.GetTopVisibleUnit(looking_player);
				return top.IsValid();
			}
		}

        top = c->UnitArmy()->GetTopVisibleUnit(looking_player); 
        return top.IsValid(); 
    } 

    return FALSE;
}

sint32 World::GetTopVisibleUnitNotCity(const MapPoint &pos, Unit &top) const
{
	sint32 player = g_selected_item->GetVisiblePlayer();
	return GetTopVisibleUnit(player, pos, top, FALSE);
}

sint32 World::GetTopRadarUnit(const MapPoint &pos, Unit &top) const
{
	Cell *c;
	top.m_id = (0);
	c = GetCell(pos);
	BOOL hasGlobalRadar;
	if(g_player[g_selected_item->GetVisiblePlayer()]) {
		hasGlobalRadar = wonderutil_GetGlobalRadar(
			g_player[g_selected_item->GetVisiblePlayer()]->m_builtWonders);
	} else {
		hasGlobalRadar = FALSE;
	}
	uint32 playerMask = 1 << g_selected_item->GetVisiblePlayer();
	if(c->GetCity().IsValid() &&
	   ((c->GetCity().AccessData()->GetRadarVisibility() & playerMask ) ||
		(c->GetCity().AccessData()->GetEverVisible() & playerMask) ||
		hasGlobalRadar)) {
		top = c->GetCity();
		return TRUE;
	}

	sint32 i, n = c->GetNumUnits();
	for(i = 0; i < n; i++) { 
		if(!c->AccessUnit(i).IsValid()) {
			if(g_network.IsClient()) {
				g_network.RequestResync(RESYNC_INVALID_UNIT);
				return FALSE;
			}
			Assert(FALSE);
			return FALSE;
		}

		if((c->AccessUnit(i).GetRadarVisibility() & playerMask) ||
		   (c->AccessUnit(i).GetVisibility() & playerMask) ||
		   hasGlobalRadar) {
			top = c->AccessUnit(i);
			return top.m_id != (0);
		}
	}
	return FALSE;
}


sint32 World::GetSecondUnit (const MapPoint &pos, Unit &second) const 

{
    Cell *c; 

    c = GetCell(pos);

    sint32 n = c->GetNumUnits();
    if (n>1) { 
        second = c->AccessUnit(n-2);

        return TRUE; 
    } 

    return FALSE;
}

CellUnitList * World::GetArmyPtr(const MapPoint &pos)
{ 
	return (GetCell(pos)->UnitArmy()); 
}

#ifdef _DEBUG
extern sint32 g_is_debug_map_color;
#endif
void World::AddZOC(const MapPoint &pos, sint32 player)
{
	GetCell(pos)->m_zoc |= (1 << player);





}

void World::RemoveZOC(const MapPoint &pos, sint32 player)
{
	Cell *cell = GetCell(pos);
	cell->m_zoc &= ~(1 << player);









}

void World::AddOtherArmyZOC(const MapPoint &pos, sint32 player, const Army &a,
							const Unit &notThisCity)
{
	Cell *cell = GetCell(pos);
	sint32 i;
	CellUnitList *cunits = NULL;
	if(cell->GetCity().m_id != 0 &&
	   cell->GetCity().GetOwner() == player &&
	   cell->GetCity().m_id != notThisCity.m_id) {
		cell->m_zoc |= (1 << player);
		return;
	}

	
	
	if((cunits = cell->UnitArmy()) != NULL &&
	   g_theUnitPool->IsValid(cunits->Access(0))) {
		if(cunits->CanEnter(pos)) {
			if(cunits->GetOwner() == player) {
				for(i = cunits->Num() - 1; i >= 0; i--) {
					if(cunits->m_array[i].GetArmy() != a &&
					   !cunits->m_array[i].IsNoZoc()) {
						cell->m_zoc |= (1 << player);
						return;
					}
				}
			}
		}
	}

	
	if(AdjacentToZOCUnit(pos, player, a, notThisCity)) {
		cell->m_zoc |= (1 << player);
#ifdef _DEBUG



#endif
	}
}

BOOL World::AdjacentToZOCUnit(const MapPoint &cpos, sint32 player, const Army &notThisArmy, const Unit &notThisCity)
{
	static MapPoint pos;
	pos = cpos;
	static MapPoint wpos;
	pos.x += 1;
	pos.y -= 2;
	CellUnitList *units;
	for(; pos.y < cpos.y + 1; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			Unit city = m_map[wpos.x][wpos.y]->GetCity();
			if(city.m_id != (0) && city != notThisCity && 
			   city.GetOwner() == player && 
			   g_theWorld->CanEnter(cpos, city.GetMovementType())) {
				return TRUE;
			} else if(city.m_id == 0) {
				units = m_map[wpos.x][wpos.y]->m_unit_army;
				if(units && units->GetOwner() == player && units->CanEnter(cpos)) {
					sint32 i;
					for(i = units->Num() - 1; i >= 0; i--) {
						if(!units->Access(i).IsNoZoc() && units->Access(i).GetArmy() != notThisArmy)
							return TRUE;
					}
				}
			}
		}
	}

	pos.x -= 1;
 	pos.y -= 2;
	for(; pos.y < cpos.y + 2; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			Unit city = m_map[wpos.x][wpos.y]->GetCity();
			if(city.m_id != (0) && city != notThisCity && 
			   city.GetOwner() == player &&
			   g_theWorld->CanEnter(cpos, city.GetMovementType())) {
				return TRUE;
			} else if(city.m_id == 0) {
				units = m_map[wpos.x][wpos.y]->m_unit_army;
				if(units && units->GetOwner() == player && units->CanEnter(cpos)) {
					sint32 i;
					for(i = units->Num() - 1; i >= 0; i--) {
						if(!units->Access(i).IsNoZoc() && units->Access(i).GetArmy() != notThisArmy)
							return TRUE;
					}
				}
			}
		}
	}

	pos.x -= 1;
	pos.y -= 2;
	for(; pos.y < cpos.y + 3; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			Unit city = m_map[wpos.x][wpos.y]->GetCity();
			if(city.m_id != (0) && city != notThisCity && 
			   city.GetOwner() == player &&
			   g_theWorld->CanEnter(cpos, city.GetMovementType())) {
				return TRUE;
			} else if(city.m_id == 0) {
				units = m_map[wpos.x][wpos.y]->m_unit_army;
				if(units && units->GetOwner() == player && units->CanEnter(cpos)) {
					sint32 i;
					for(i = units->Num() - 1; i >= 0; i--) {
						if(!units->Access(i).IsNoZoc() && units->Access(i).GetArmy() != notThisArmy)
							return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

void World::GetAdjacentCities(UnitDynamicArray &city_list, const MapPoint &cpos)
{
	city_list.Clear();
	static MapPoint pos;
	pos = cpos;
	static MapPoint wpos;

    pos.x += 1;
	pos.y -= 2;
	Cell *cell;
    Unit nth_city; 
	for(; pos.y < cpos.y + 1; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			cell = m_map[wpos.x][wpos.y];
			nth_city = cell->GetCity();
			if(nth_city.m_id != 0) {
				city_list.Insert(nth_city);
			}
		}
	}

	pos.x -= 1;
 	pos.y -= 2;
	for(; pos.y < cpos.y + 2; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			cell = m_map[wpos.x][wpos.y];
			nth_city = cell->GetCity();
			if(nth_city.m_id != 0) {
				city_list.Insert(nth_city);
			}
		}
	}

	pos.x -= 1;
	pos.y -= 2;
	for(; pos.y < cpos.y + 3; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			cell = m_map[wpos.x][wpos.y];
			nth_city = cell->GetCity();
			if(nth_city.m_id != 0) {
				city_list.Insert(nth_city);
			}
		}
	}
}

void World::GetAdjacentUnits(UnitDynamicArray &units, const MapPoint &cpos)
{
	units.Clear();
	static MapPoint pos;
	pos = cpos;
	static MapPoint wpos;
	pos.x += 1;
	pos.y -= 2;
	Cell *cell;
	for(; pos.y < cpos.y + 1; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			cell = m_map[wpos.x][wpos.y];
			if(cell->UnitArmy()) {
				sint32 i;
				for(i = 0; i < cell->UnitArmy()->Num(); i++) {
					units.Insert(cell->UnitArmy()->Access(i));
				}
			}
		}
	}
	pos.x -= 1;
 	pos.y -= 2;
	for(; pos.y < cpos.y + 2; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			cell = m_map[wpos.x][wpos.y];
			if(cell->UnitArmy()) {
				sint32 i;
				for(i = 0; i < cell->UnitArmy()->Num(); i++) {
					units.Insert(cell->UnitArmy()->Access(i));
				}
			}
		}
	}

	pos.x -= 1;
	pos.y -= 2;
	for(; pos.y < cpos.y + 3; pos.y++) {
		if(WrapPoint(cpos, pos, wpos)) {
			cell = m_map[wpos.x][wpos.y];
			if(cell->UnitArmy()) {
				sint32 i;
				for(i = 0; i < cell->UnitArmy()->Num(); i++) {
					units.Insert(cell->UnitArmy()->Access(i));
				}
			}
		}
	}
}

void World::RecalculateZOC()
{
	
	MapPoint pos, chk;
	sint32 d;
	sint32 i;

	
	for(pos.x = 0; pos.x < m_size.x; pos.x++) {
		for(pos.y = 0; pos.y < m_size.y; pos.y++) {
			Cell *cell = m_map[pos.x][pos.y];
			cell->m_zoc = 0;
		}
	}

	for(pos.x = 0; pos.x < m_size.x; pos.x++) {
		for(pos.y = 0; pos.y < m_size.y; pos.y++) {
			Cell *cell = m_map[pos.x][pos.y];
			
			if(cell->GetCity().m_id != 0) {
				
				sint32 owner = cell->GetCity().GetOwner();
				AddZOC(pos, owner);
				for(d = 0; d < (sint32)NOWHERE; d++) {
					if(pos.GetNeighborPosition((WORLD_DIRECTION)d, chk)) {
						if(CanEnter(chk, cell->GetCity().GetMovementType())) {
							AddZOC(chk, owner);
						}
					}
				}
			} else if(cell->UnitArmy()) {
				
				for(i = 0; i < cell->UnitArmy()->Num(); i++) {
					if(!g_theUnitPool->IsValid(cell->UnitArmy()->Access(i))) {
						cell->RemoveUnitReference(cell->UnitArmy()->Access(i));
						i--;
						if(!cell->UnitArmy())
							break;
						else
							continue;
					}

					if(!cell->UnitArmy()->Access(i).IsNoZoc()) {
						
						sint32 owner = cell->UnitArmy()->GetOwner();
						AddZOC(pos, owner);
						for(d = 0; d < (sint32)NOWHERE; d++) {
							if(pos.GetNeighborPosition((WORLD_DIRECTION)d, chk) &&
							   cell->UnitArmy()->CanEnter(chk)) {
								AddZOC(chk, owner);
							}
						}
					}
				}
			}
		}
	}
}


void World::RecalculateZOC(const MapPoint &p)
{
	SquareIterator it(p, 1);
	Cell *centerCell = GetCell(p);
	for(it.Start(); !it.End(); it.Next()) {
		Cell *cell = GetCell(it.Pos());
		if(cell->UnitArmy() && 
		   cell->UnitArmy()->ExertsZOC() &&
		   cell->UnitArmy()->CanEnter(p)) {
			AddZOC(p, cell->UnitArmy()->GetOwner());
		}
		if(cell->HasCity() && 
		   cell->GetCity().IsValid() &&
		   centerCell->CanEnter(cell->GetCity().GetMovementType())) {
			AddZOC(p, cell->GetCity().GetOwner());
		}
	}
}
