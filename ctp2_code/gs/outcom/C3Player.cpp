










#include "c3.h"
#include "Globals.h"
#include "C3Player.h"
#include "MapPoint.h"
#include "Player.h"
#include "XY_Coordinates.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "BuildingRecord.h"
#include "CivArchive.h"
#include "TradeRoute.h"
#include "Army.h"
#include "CellUnitList.h"
#include "TaxRate.h"
#include "Gold.h"

#include "BSet.h"
#include "CityData.h"
#include "UnitData.h"
#include "Happy.h"
#include "DB.h"
#include "WonderRecord.h"
#include "ConstDB.h"
#include "AiCause.h"



#include "TradeRoute.h"
#include "Agreement.h"
#include "Cell.h"
#include "Order.h"
#include "TradeBids.h"
#include "C3Trade.h"
#include "Regard.h"
#include "EndGame.h"

#include "network.h"

#include "Diplomacy_Log.h"
#include "UnitRecord.h"

extern Diplomacy_Log *g_theDiplomacyLog; 


#include "GovernmentRecord.h"



STDMETHODIMP C3Player::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3Player)) {
		*obj = (IC3Player*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3Player::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3Player::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}



C3Player::C3Player(sint32 idx)
{
    m_refCount = 0; 
    Assert(0 <= idx); 
    Assert(idx < k_MAX_PLAYERS); 

    m_owner = idx; 
    Assert(g_player[idx]);
    m_ptr = g_player[idx]; 
    Assert(m_ptr); 
}

C3Player::C3Player(CivArchive &archive)
{
    Serialize(archive); 
}

void C3Player::Serialize(CivArchive &archive)
{ 
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
        archive << m_owner; 
    } else  { 
        archive >> m_refCount; 
        archive >> m_owner; 
        m_ptr = g_player[m_owner]; 
    }

    CHECKSERIALIZE
}

BOOL C3Player::GetArmyPos(uint32 army_id,  BOOL *is_unknown_id, MapPointData *start)

{ 
    MapPoint p1; 

    *is_unknown_id = FALSE; 

    m_ptr->GetArmyPos(army_id, *is_unknown_id, p1); 

    if (*is_unknown_id) 
        return FALSE; 

    ((MapPoint*)start)->Iso2Norm(p1); 

    return TRUE; 

   
}

BOOL C3Player::GetArmyXYPos
(
	PLAYER_INDEX player_index,
	uint32 army_id,  
	BOOL *is_unknown_id, 
	MapPointData *start
)

{ 
    MapPoint p1; 

    *is_unknown_id = FALSE; 

	
    g_player[player_index]->GetArmyPos(army_id, *is_unknown_id, p1); 

    if (*is_unknown_id) 
        return FALSE; 

	
	g_theWorld->XY_Coords.RC_to_XY(MapPointData(p1), *start);

    return TRUE; 

   
}

BOOL C3Player::Norm2XY(const MapPointData &norm_pos, MapPointData &xy_pos)

{ 
    MapPoint ipos; 

	
    ipos.Norm2Iso(norm_pos); 

	
	g_theWorld->XY_Coords.RC_to_XY(MapPointData(ipos), xy_pos);

    return TRUE; 

   
}

BOOL C3Player::XY2Norm(const MapPointData &xy_pos, MapPointData &norm_pos)

{ 
    MapPointData ipos;
	MapPoint npos; 

	
	g_theWorld->XY_Coords.XY_to_RC(xy_pos, ipos);

	
    npos.Iso2Norm(ipos); 

	norm_pos.x = npos.x;
	norm_pos.y = npos.y;
	norm_pos.z = npos.z;

    return TRUE; 

   
}

BOOL C3Player::GetArmyCurMinMovementPoints(uint32 army_id,  BOOL *is_unknown_id, double *cur)

{
    *is_unknown_id = FALSE; 

    m_ptr->GetArmyCurMinMovementPoints(army_id, *is_unknown_id, *cur);

    return !(*is_unknown_id); 

}

BOOL C3Player::GetArmyMinMovementPoints(uint32 army_id,  BOOL *is_unknown_id, double *min_move)
{
    *is_unknown_id = FALSE; 

     m_ptr->GetArmyMinMovementPoints(army_id, *is_unknown_id, *min_move);

    return !(*is_unknown_id); 

}


BOOL C3Player::ArmySettle(uint32 army_id, BOOL *is_unknown_id) 
{ 
    MapPoint ipos; 

    *is_unknown_id = FALSE; 

    return m_ptr->ArmySettle(army_id, *is_unknown_id ); 
}

BOOL C3Player::ArmyCanEnter(uint32 army_id,  BOOL *is_unknown_id,
   MapPointData *pos,  BOOL *move_to_many_units_dest, BOOL *move_violated_zoc, BOOL *move_violated_movetype, 
   BOOL *move_out_of_fuel)

{ 

    if (
        (pos->x < 0) || (g_theWorld->GetXWidth() <= pos->x) ||
        (pos->y < 0) || (g_theWorld->GetYHeight() <= pos->y) ||
        (pos->z < 0) || (g_theWorld->GetZHeight() <= pos->z))
    {
        sint32 ArmyCanEnter_out_of_bounds=0; 
        Assert(ArmyCanEnter_out_of_bounds); 
        return FALSE; 
    } 

    MapPoint ipos; 

    *is_unknown_id = FALSE; 

    ipos.Norm2Iso(*pos); 

    return m_ptr->ArmyCanEnter(army_id, *is_unknown_id, ipos, 
        *move_to_many_units_dest, *move_violated_zoc, *move_violated_movetype, 
        *move_out_of_fuel);


}

BOOL C3Player::UnitTypeCanEnter(sint32 unit_type, MapPointData *pos)
{
    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 

    uint32 mflag = g_theUnitDB->Get(unit_type)->GetMovementType();

    return g_theWorld->CanEnter(ipos, mflag);    
}


BOOL C3Player::MoveTypeCanEnter(uint32 move_type, MapPointData *pos)
{
    MapPoint ipos; 
    ipos.Norm2Iso(*pos); 

	
    return g_theWorld->CanEnter(ipos, move_type) || g_theWorld->HasCity(ipos);
}


BOOL C3Player::ArmyMoveTo(uint32 army_id, BOOL *is_unknown_id, MapPointData *pos,  
    BOOL *did_move, BOOL *i_died, BOOL *move_violated_zoc, 
    BOOL *revealed_foreign_units, BOOL *revealed_unexplored,
       BOOL *is_transported, BOOL *out_of_fuel)
{ 
    MapPoint ipos; 

    *is_unknown_id = FALSE; 

    ipos.Norm2Iso(*pos); 

    return  m_ptr->ArmyMoveTo(army_id, *is_unknown_id, ipos, *did_move, *i_died, *move_violated_zoc, 
        *revealed_foreign_units, *revealed_unexplored, *is_transported, *out_of_fuel);
}


extern BOOL UDUnitTypeCanSettle(sint32 unit_type, const MapPoint &pos);

BOOL C3Player::UnitTypeCanSettle (uint32 unit_type,
        MapPointData *pos)
{
    MapPoint ipos; 

    ipos.Norm2Iso(*pos); 

    return UDUnitTypeCanSettle(unit_type, ipos); 
}


BOOL C3Player::ArmyCanSettle (uint32 army_id, BOOL *is_unknown_id, 
         MapPointData *pos)
{
    MapPoint ipos; 

    ipos.Norm2Iso(*pos); 

    return m_ptr->ArmyCanSettle(army_id, *is_unknown_id, ipos); 
}

BOOL C3Player::ArmyGroup(uint32 add_me, uint32 target, BOOL *is_unknown_id)
{ 
   return m_ptr->AiArmyGroup(add_me, target, is_unknown_id);
} 

BOOL C3Player::ArmyUngroup(uint32 split_me, BOOL *is_unknown_id)

{ 
    return  m_ptr->AiArmyUngroup(split_me, *is_unknown_id);
}



BOOL C3Player::GetCityPos (uint32 city_id, BOOL *is_unknown_id, 
      MapPointData *pos)

{
    MapPoint ipos; 

    m_ptr->GetCityPos(city_id, *is_unknown_id, ipos);

   ((MapPoint*)pos)->Iso2Norm(ipos); 

    return !(*is_unknown_id);
}


BOOL C3Player::GetCityXYPos (PLAYER_INDEX owner, uint32 city_id, BOOL *is_unknown_id, 
      MapPointData *pos)

{
    MapPoint ipos; 

    Assert(0 <= owner); 
    Assert(owner < k_MAX_PLAYERS); 
    g_player[owner]->GetCityPos(city_id, *is_unknown_id, ipos);

    if (*is_unknown_id)  return FALSE; 

	
	g_theWorld->XY_Coords.RC_to_XY(MapPointData(ipos), *pos);

    return !(*is_unknown_id);
}


BOOL C3Player::CityEnqueueBuildItem (uint32 city_id, BOOL *is_unknown_id, 
        sint32 category, sint32 unit_type)
{
   
    BOOL ret =  m_ptr->CityEnqueueBuildItem (city_id, is_unknown_id, 
        category, unit_type);

    
    

    return ret; 
}

BOOL C3Player::CityChangeBuildItem (uint32 city_id, BOOL *is_unknown_id, 
        sint32 category, sint32 unit_type)
{
   
    BOOL ret = m_ptr->CityChangeCurrentlyBuildingItem(city_id, is_unknown_id,
        category, unit_type);

    
    

    return ret; 
}

BOOL C3Player::CityBuildQueueLen (uint32 city_id, BOOL *is_unknown_id)
{
   
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    Assert(is_unknown_id);

    return cd->m_build_queue.GetLen();
}


BOOL C3Player::CityBuildQueueFront (uint32 city_id, BOOL *is_unknown_id, 
									sint32 &category, sint32 &type)
{
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    Assert(is_unknown_id);
	type = -1;
	category = -1;

    if (cd->m_build_queue.GetLen())
		{
			type = cd->m_build_queue.GetHead()->m_type;
			category = cd->m_build_queue.GetHead()->m_category;
			return TRUE;
		}
	return FALSE;
}


BOOL C3Player::CityChangeCurrentlyBuildingItem (uint32 city_id, BOOL *is_unknown_id, 
        sint32 category, sint32 item_type)

{
    Assert(0); 
    return m_ptr->CityChangeCurrentlyBuildingItem (city_id, is_unknown_id, 
        category, item_type);
}

sint32  C3Player::CityGetStoredProduction (uint32 city_id, BOOL *is_unknown_id)
{
    return m_ptr->CityGetStoredProduction(city_id, is_unknown_id); 
}

sint32  C3Player::CityGetGrossProduction (uint32 city_id, BOOL *is_unknown_id)
{
    return m_ptr->CityGetGrossProduction(city_id, is_unknown_id); 
}

sint32  C3Player::CityGetNetProduction (uint32 city_id, BOOL *is_unknown_id)
{
   return m_ptr->CityGetNetProduction(city_id, is_unknown_id); 
}

double C3Player::GetTotalProduction(PLAYER_INDEX test_me)
{
    Assert(g_player[test_me]); 
    return double(g_player[test_me]->GetTotalProduction()); 
}

double C3Player::GetTotalUnitCost(PLAYER_INDEX test_me)
{
    Assert(g_player[test_me]); 
    return double(g_player[test_me]->GetTotalUnitCost()); 
}


sint32  C3Player::CityGetStoredFood (uint32 city_id, BOOL *is_unknown_id)
{
   return m_ptr->CityGetStoredFood(city_id, is_unknown_id); 
}

sint32  C3Player::CityGetGrossFood (uint32 city_id, BOOL *is_unknown_id)
{
   return m_ptr->CityGetGrossFood(city_id, is_unknown_id); 
}

sint32  C3Player::CityGetNetFood (uint32 city_id, BOOL *is_unknown_id)
{
   return m_ptr->CityGetNetFood(city_id, is_unknown_id); 
}


sint32 C3Player::GetAccumulatedGrowthFood(uint32 city_id, BOOL *is_unknown_id)
{ 
   CityData *cd = GetCityData(city_id, is_unknown_id); 
 
   if (cd == NULL) { 
       return 0; 
   } else { 
       return cd->m_accumulated_food; 
   }        
}

sint32 C3Player::GetAccumulatedGrowthThreshold(uint32 city_id, BOOL *is_unknown_id)
{ 

    CityData *cd = GetCityData(city_id, is_unknown_id); 

    if (cd == NULL) { 
       return 0; 
    }

    return (cd->PopCount() - cd->SlaveCount())  * sint32(g_theConstDB->CityGrowthCoefficient());    
}

sint32  C3Player::CityGetGrossGold (uint32 city_id, BOOL *is_unknown_id)
{
   return m_ptr->CityGetGrossGold(city_id, is_unknown_id); 
}

sint32 C3Player::CityGetGrossScience (uint32 city_id, BOOL *is_unknown_id)
{
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    Assert(cd); 
    if (*is_unknown_id) { 
        return 0; 
    } 

    return cd->m_science;
}

sint32  C3Player::CityGetNetGold (uint32 city_id, BOOL *is_unknown_id)
{
   return m_ptr->CityGetNetGold(city_id, is_unknown_id);  
}


void C3Player::SetMaterialsTax(double mt)
{   
    m_ptr->SetMaterialsTax(mt); 
}

sint32 C3Player::GetMaterialsStored ()
{ 
    return m_ptr->GetMaterialsStored(); 
} 

sint32 C3Player::GetUnusedFreight()
{
    return m_ptr->GetUnusedFreight(); 
}

sint32 C3Player::GetTotalFreight()
{ 
    return m_ptr->GetTotalFreight(); 
} 

BOOL C3Player::CreateTradeRoute(BOOL *is_unknown_id, uint32 src_city,
    sint32 type_route, sint32 scr_good,   uint32 dest_city)
{
    return m_ptr->AiCreateTradeRoute(is_unknown_id, src_city, 
	    type_route, scr_good, dest_city);
}



void C3Player::CancelTradeRoute(sint32 id)
{
    TradeRoute route(id); 
    m_ptr->CancelTradeRoute(route); 
} 

STDMETHODIMP_ (sint32) C3Player::CancelCityRoutesToPlayer(uint32 city_id, BOOL *is_unknown_id, 
     PLAYER_INDEX no_trade_for_you)
{
    CityData *the_city = GetCityData(city_id, is_unknown_id); 

    if (!the_city) return 0; 

    BOOL did_cancel = FALSE; 
    sint32 route_idx, route_num; 
    TradeRoute nth_route; 

    TradeDynamicArray* src = the_city->GetTradeSourceList();
    if (src) { 
        route_num = src->Num(); 
        for (route_idx=route_num-1; 0<=route_idx; route_idx--) { 
            nth_route = src->Access(route_idx); 
            if (nth_route.GetDestination().GetOwner() == no_trade_for_you) { 
                m_ptr->CancelTradeRoute(nth_route); 
                did_cancel = TRUE; 
            } 
        }
    }

	TradeDynamicArray* dest = the_city->GetTradeDestinationList();
    if (dest) { 
        route_num = src->Num(); 
        for (route_idx=route_num-1; 0<=route_idx; route_idx--) { 
            nth_route = dest->Access(route_idx); 
            if (nth_route.GetSource().GetOwner() == no_trade_for_you) { 
                m_ptr->CancelTradeRoute(nth_route); 
                did_cancel = TRUE; 
            } 
        }
    }

    return did_cancel; 
}

BOOL C3Player::CityBuyFront(BOOL *is_unknown_id , uint32 u_city_id)
{
    return m_ptr->CityBuyFront(is_unknown_id, u_city_id); 
}

sint32 C3Player::CityGetOvertimeCost(BOOL *is_unknown_id , uint32 u_city_id)
{
    return m_ptr->CityGetOvertimeCost(is_unknown_id, u_city_id); 
} 


void C3Player::GetGoodCount(BOOL *is_unknown_id, uint32 u_city_id, 
    sint32 type_good, sint32 *local_count, sint32 *total_count) 
{
    
    CityData *cd = GetCityData(u_city_id, is_unknown_id); 
    if (cd == NULL) return; 

    *local_count = cd->GetLocalResourceCount(type_good);
    *total_count = cd->GetResourceCount(type_good);
}

double C3Player::GetRouteCost(BOOL *is_unknown_id , 
    uint32 u_src_city, uint32 u_dest_city) 

{
    return m_ptr->GetRouteCost(is_unknown_id, u_src_city, u_dest_city); 
}


void C3Player::SetWorkdayLevel (sint32 w)
{ 
    m_ptr->SetWorkdayLevel(w); 
} 
void C3Player::SetWagesLevel (sint32 w)
{
    m_ptr->SetWagesLevel(w); 
} 
void C3Player::SetRationsLevel (sint32 w)
{
    m_ptr->SetRationsLevel(w); 
}

void C3Player::GetExpectedWGF (sint32 *w, sint32 *g, sint32 *f)
{
    *w = -1; 
    *g = 0; 
    *f = 0; 
}

void C3Player::SetWGF(sint32 w, sint32 g, sint32 f)
{
    Assert(-2 <= w); 
    Assert(w <= 2); 
    if (w < -2) { 
        return; 
    } else if (2 < w) { 
        return; 
    } 
    m_ptr->SetWorkdayLevel(w); 

    Assert(-2 <= g); 
    Assert(g <= 2); 
    if (g< -2) { 
        return; 
    } else if (2 < g) {
        return; 
    } 
    m_ptr->SetWagesLevel(g); 

    Assert(-2 <= f); 
    Assert(f <= 2); 
    if (f < -2) { 
        return; 
    } else if (2 < f) { 
        return; 
    } 
    m_ptr->SetRationsLevel(f); 
}

sint32 C3Player::GetWagesPerPerson()

{ 
    return sint32(m_ptr->GetWagesPerPerson() + 0.5);  
}

sint32 C3Player::GetTotalBuildingUpkeep()
{
    return m_ptr->GetTotalBuildingUpkeep(); 
} 

void C3Player::GetGoldLevels(sint32 *income, sint32 *lost_to_cleric,
    sint32 *lost_to_crime, sint32 *maintenance, sint32 *wages, 
    sint32 *science, sint32 *old_savings, sint32 *current_savings)
{ 
    m_ptr->m_gold->GetGoldLevels(income, lost_to_cleric, lost_to_crime, maintenance, 
         wages, science, old_savings, current_savings); 
} 



BOOL C3Player::ArmyGetNumCargo (BOOL *is_unknown_id, uint32 u_id, 
         sint32 *full_slots, sint32 *empty_slots)
{
    return m_ptr->ArmyGetNumCargo(is_unknown_id, u_id, full_slots, empty_slots);
}

sint32 C3Player::AiNumUnitsCanMoveIntoThisTranportEver(BOOL *is_unknown_id,  
        uint32 test_me, 
       uint32 transport)
{
    return m_ptr->AiNumUnitsCanMoveIntoThisTranportEver(is_unknown_id,  
        test_me, transport);
}

BOOL C3Player::AiCanMoveArmyIntoThisTranportEver(BOOL *is_unknown_id,  
        uint32 test_me, 
       uint32 transport)
{
    return m_ptr->AiCanMoveArmyIntoThisTranportEver(is_unknown_id,  
        test_me, transport);
}

BOOL C3Player::AiCanMoveArmyIntoThisTranportRightNow(BOOL *is_unknown_id,  
        uint32 move_me, uint32 transport, BOOL *enough_move, 
        BOOL *adjacent, BOOL *ever)
{
    return m_ptr->AiCanMoveArmyIntoThisTranportRightNow(is_unknown_id,  
        move_me, transport, enough_move, adjacent, ever);
} 

BOOL C3Player::AiArmyMoveIntoTranport(BOOL *is_unknown_id,  uint32 move_me, 
        uint32 transport, BOOL *is_transported)
{
    return m_ptr->AiArmyMoveIntoTranport(is_unknown_id,  move_me, 
        transport, is_transported);
} 


BOOL C3Player::AiUnloadAllTransportsInArmy(BOOL *is_unknown_id, 
        uint32 unload_me,  MapPointData *dest_pos, 
        BOOL *did_move, BOOL *i_died, 
		BOOL *revealed_foreign_units, BOOL *revealed_unexplored, 
        BOOL *zocViolation, BOOL *is_transported)
{
    MapPoint ipos; 
    ipos.Norm2Iso(*dest_pos); 

	
    return m_ptr->AiUnloadAllTransportsInArmy(is_unknown_id, 
        unload_me,  ipos, did_move, i_died, 
		revealed_foreign_units, revealed_unexplored, 
        zocViolation, is_transported);

}

#ifdef _DEBUG

#include "aui.h"
#include "aui_surface.h"
#include "primitives.h"
#include "aui_window.h"
#include "debugwindow.h"
extern DebugWindow *g_debugWindow; 
void C3Player::Dprint(char *str) 
{ 

    
    WPRINTF(k_DBG_AI, ("%s", str)); 

    return; 
} 

#endif

BOOL C3Player::GetCargoMovementPoints (BOOL *is_unknown_id, 
         uint32 u_tran_id, double* min_move_point, BOOL* first_move)
{
    return m_ptr->AiGetCargoMovementPoints (is_unknown_id, 
         u_tran_id,  min_move_point, first_move); 
}


BOOL C3Player::Paradrop(BOOL *is_unknown_id, uint32 u_id,  const MapPointData *dest_pos, 
    BOOL *all_dropped, BOOL *some_dropped, BOOL *all_died, 
    BOOL *revealed_foreign_units, BOOL *revealed_unexplored)
{
	return FALSE;
}

sint32 C3Player::GetParadropMaxDistance()
{ 
    return g_theConstDB->ParadropDistance();
}

BOOL C3Player::CanParadropNow(BOOL *is_unknown_id, uint32 u_id)
{
	return FALSE;
}


BOOL C3Player::CanBeCargoPodded(BOOL *is_unknown_id, 
         uint32 u_id)
{
	return FALSE;
}


BOOL C3Player::GetCurrentFuel(BOOL *is_unknown_id, uint32 u_id, sint32 *current)
{ 
    Army army; 

    if (m_ptr->AiGetArmy(*is_unknown_id, u_id, army) == FALSE) { 
        return FALSE; 
    }

    sint32 t; 
    sint32 num_army, idx_army; 
    num_army = army.Num(); 
    Assert(0 < num_army); 
    *current = 10000000; 
    sint32 f;

    for (idx_army=0; idx_army<num_army; idx_army++) { 
        t = army.Get(idx_army).GetType(); 
        if (g_theUnitDB->Get(t)->GetNoFuelThenCrash()) { 
            f = army.Access(idx_army).GetFuel(); 
            if (f < *current) { 
                *current = f; 
            } 
        } 
    }

    return *current != 10000000;
} 


sint32 C3Player::GetAllTileValue(uint32 city_id, BOOL *is_unknown_id, 
    sint32 num_tile, TileUtility *open_tile[k_NUM_CITY_TILES])	
{
    
    return m_ptr->GetAllTileValue(city_id, *is_unknown_id, 
        num_tile, open_tile);	
}


CityData * C3Player::GetCityData(uint32 city_id, BOOL *is_unknown_id)
{
    BSetID *idx = m_ptr->m_bset_cities_index->Find(city_id) ;

	
	*is_unknown_id = TRUE ;
	if (idx == NULL)
		return NULL;

	*is_unknown_id = FALSE ;

    return m_ptr->m_all_cities->Get(idx->GetVal()).GetData()->GetCityData();
}

Army C3Player::GetArmyList(uint32 army_id, BOOL *is_unknown_id)
{
    BSetID *idx; 
    
    idx = m_ptr->m_bset_armies_index->Find(army_id); 
    if (idx) { 
         *is_unknown_id = FALSE; 
    } else { 
        *is_unknown_id = TRUE; 
        return NULL; 
    }   

    return m_ptr->m_all_armies->Access(idx->GetVal());
}

void C3Player::GetCityHappiness(uint32 city_id, BOOL *is_unknown_id, 
   sint32 *old_happiness, double *old_crime)
{
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    if (cd == NULL) return; 

    *old_happiness = sint32 (cd->GetHappy()->GetHappiness());
    *old_crime = cd->GetHappy()->GetCrime();   
}

void C3Player::GetCityCrimePrevention(uint32 city_id, BOOL *is_unknown_id, 
    double *cop)
{
#if 0
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    if (cd == NULL) return; 

    double wonderCops = 1.0 - 0.01 * wonderutil_GetDecreaseCrimePercentage(
		m_ptr->GetBuiltWonders());

    double buildingCops = cd->GetImprovementCrimeMod();

    *cop = wonderCops * buildingCops;
#endif
}


long C3Player::GetCitySizePopEffect(uint32 city_id, BOOL *is_unknown_id, 
   sint32 *m_city_size_penalty)
{
    CityData *cd = GetCityData(city_id, is_unknown_id); 
	Player *p = g_player[m_owner]; 
	if (cd == NULL) return FALSE; 
    
    *m_city_size_penalty = (sint32)(cd->GetHappy()->CalcSize(*cd, p)); 
    return TRUE; 
}


void C3Player::SetScienceTax(double s)
{
    Assert(0.0<= s); 
    Assert(s <= 1.0);
    double tmp = 1.0 - s; 
    m_ptr->m_tax_rate->SetTaxRates(s, m_ptr->m_owner); 
} 

void C3Player::CityFindWhatIsBuilt (uint32 city_id, BOOL *is_unknown_id, 
         BOOL is_blg_built[64], BOOL is_wndr_built[64])
{
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    Assert(cd); 
    if (*is_unknown_id) { 
        return ; 
    } 

    uint64 b = cd->m_built_improvements;

    sint32 i;
    for (i = 0; i<64; i++) {          
        is_blg_built[i] = BOOL(b & 0x1); 
        b = b>>1; 
    } 

    b = cd->m_builtWonders; 
    for (i=0; i<64; i++) { 
        is_wndr_built[i] = BOOL(b & 0x1); 
        b = b>>1; 
    } 	
}



BOOL C3Player::SetGovernmentType(sint32 idx_gov)
{
   sint32 r; 
   r = m_ptr->SetGovernmentType(idx_gov); 
   Assert(r); 
   return r; 
} 


sint32 C3Player::GetGovernmentType ()
{
    return m_ptr->GetGovernmentType(); 
}

double C3Player::GetCityDistFromCapitol (uint32 city_id, BOOL *is_unknown_id)
{
    
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    if (cd == NULL) return 1000000.0; 

    return cd->m_happy->GetDistToCapitol();
}


void C3Player::GetPeaceMovement(double *overseas_defeat, 
    double *home_defeat, double *overseas)
{
    m_ptr->GetPeaceMovement(*overseas_defeat, *home_defeat, *overseas);
}

double C3Player::GetCityPollution (uint32 city_id, BOOL *is_unknown_id)
{
    
    CityData *cd = GetCityData(city_id, is_unknown_id); 
    if (cd == NULL) return 1000000.0; 

    return cd->m_total_pollution;
}


void C3Player::SetReadinessLevel(sint32 level) 
{

    m_ptr->SetReadinessLevel(READINESS_LEVEL(level));
}

sint32 C3Player::GetReadinessLevel()
{
    return m_ptr->GetReadinessLevel(); 
}


void C3Player::GetReadinessCost(sint32 *total_cost, double *percent_cost)
{ 
    *total_cost = m_ptr->GetReadinessCost(); 
    *percent_cost = m_ptr->GetPercentProductionToMilitary();
}

BOOL C3Player::AttemptSlaveRaid (uint32 u_id, BOOL *is_unknown_id, 
          MapPointData *target_pos)
{
    Army al = GetArmyList(u_id, is_unknown_id); 

    if (!al) return FALSE; 

    
    

    MapPoint ipos; 
    ipos.Norm2Iso(*target_pos);

    al.AddOrders(UNIT_ORDER_SLAVE_RAID, ipos);
	al.ExecuteOrders();

    

    
	
	return TRUE;
}

BOOL C3Player::IsSlaveRaidPossible(uint32 u_id, BOOL *is_unknown_id, 
          MapPointData *target_pos)
{

   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);

   double success, death; 
   sint32 timer, amount;
   sint32 uindex;
   BOOL target_is_city;
   Unit target_city;
   Unit home_city;

   return al.IsSlaveRaidPossible(
      ipos, success, death, timer, amount, uindex, 
      target_is_city, target_city, home_city);
}

sint32 C3Player::GetSlaveCount(uint32 city_id, BOOL *is_unknown_id)
{ 

    CityData *cd = GetCityData(city_id, is_unknown_id); 

    if (cd == NULL) { 
       return -1; 
    }

    return (cd->SlaveCount());
}

BOOL C3Player::FindNearestCity(MapPointData *start_pos, uint32 *nearest_city)
{
    MapPoint ipos;
    ipos.Norm2Iso(*start_pos); 

	double distance;
    Unit home_city; 
	sint32 r = m_ptr->GetNearestCity(ipos, home_city, distance);

    if (!r) { 
        return FALSE; 
    } 

    sint32 city_idx, city_num; 
    city_num = m_ptr->m_all_cities->Num(); 
    for (city_idx=0; city_idx<city_num; city_idx++) { 
        if (m_ptr->m_all_cities->Get(city_idx) == home_city) { 
            *nearest_city = m_ptr->m_all_cities_id->Access(city_idx).GetVal(); 
            return TRUE; 
        }
    } 

    Assert(0); 
    return FALSE; 
}

void C3Player::SetArmyDBGString (uint32 u_id, BOOL *is_unknown_id, 
        char *str)
{
#ifdef _DEBUG
   Army the_army = GetArmyList(u_id, is_unknown_id); 
   if (!the_army) return; 

   sint32 unit_idx, unit_num; 
   unit_num = the_army.Num(); 
   for (unit_idx=0; unit_idx<unit_num; unit_idx++) {
        the_army[unit_idx].SetText(str); 
   } 
#endif
}

void  C3Player::SetCityDBGString (uint32 u_id, BOOL *is_unknown_id, 
        char *str)
{
#ifdef _DEBUG
    BSetID *idx = m_ptr->m_bset_cities_index->Find(u_id) ;

	
	*is_unknown_id = TRUE ;
	if (idx == NULL)
		return;

	*is_unknown_id = FALSE ;

    m_ptr->m_all_cities->Access(idx->GetVal()).SetText(str);
#endif
}

STDMETHODIMP_(double) C3Player::GetPollutionLevel()
{
	return m_ptr->GetPollutionLevel();
}

STDMETHODIMP_(sint32) C3Player::NearTradeRoute(MapPointData *pos)
{
	Cell *cell;
    
	MapPoint ipos; 
	ipos.Norm2Iso(*pos); 
	
	cell = g_theWorld->GetCell(ipos);
	if((cell->GetNumTradeRoutes()) > 0) {
		for(sint32 i = cell->GetNumTradeRoutes() -1; i>= 0; i--) {
			if(cell->GetTradeRoute(i).GetSource().GetOwner() == m_owner ||
			   cell->GetTradeRoute(i).GetDestination().GetOwner() == m_owner)
				return 1;
		}
	}
	
	return 0;
}

STDMETHODIMP_(sint32) C3Player::GetNumTradeRoutes()
{
	Assert(g_player[m_owner]);
	if(g_player[m_owner]) {
		return g_player[m_owner]->GetNumTradeRoutes();
	}
	return 0;
}

STDMETHODIMP_(sint32) C3Player::GetTradeWith(PLAYER_INDEX second)
{
	Assert(g_player[m_owner]);
	Assert(g_player[second]);
	if(g_player[m_owner] && g_player[second]) {
		return g_player[m_owner]->GetTradeWith(second);
	}
	return 0;
}

STDMETHODIMP_(BOOL) C3Player::ReformCity(uint32 u_id, BOOL *is_unknown_id, 
										 MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CanReformCity(uint32 u_id, BOOL *is_unknown_id, 
											MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 index;

   return al.CanReformCity(index, ipos);
}

STDMETHODIMP_(BOOL) C3Player::SueFranchise(uint32 u_id, BOOL *is_unknown_id, 
									  MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CanSueFranchise(uint32 u_id, 
											  BOOL *is_unknown_id, 
											  MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);

   sint32 index;

   
   return al.CanSue(index);
}


STDMETHODIMP_(BOOL) C3Player::Bombard(uint32 u_id, 
									  BOOL *is_unknown_id, 
									  MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CanBombard(uint32 u_id, 
										 BOOL *is_unknown_id, 
										 MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);

   return al.CanBombard(ipos);
}

STDMETHODIMP_ (BOOL) C3Player::ConvertCity (uint32 u_id, 
											BOOL *is_unknown_id, 
											MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_ (BOOL) C3Player::CanConvertCity(uint32 u_id, 
											  BOOL *is_unknown_id, 
											  MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);

   return al.CanConvertCity(ipos);
}

STDMETHODIMP_ (BOOL) C3Player::Rustle(uint32 u_id, 
									  BOOL *is_unknown_id, 
									  MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_ (BOOL) C3Player::CanRustle(uint32 u_id, 
										 BOOL *is_unknown_id, 
										 MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_ (BOOL) C3Player::Pillage(uint32 u_id, 
									   BOOL *is_unknown_id, 
									   MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_ (BOOL) C3Player::CanPillage(uint32 u_id, 
										  BOOL *is_unknown_id, 
										  MapPointData *target_pos)
{
	uint32 uindex;
	Army al = GetArmyList(u_id, is_unknown_id); 
	if (!al) return FALSE; 

  


	
	
	MapPoint army_pos;
	al.GetPos(army_pos);
	MapPoint ipos; 
	ipos.Norm2Iso(*target_pos);

    if (ipos == army_pos) { 

        Cell *the_cell = g_theWorld->AccessCell(ipos); 
        if (m_ptr->m_owner == the_cell->GetOwner()) return FALSE; 

		return al.CanPillage(uindex);
    } 
    else { 	
		return false;
    }
}

STDMETHODIMP_ (BOOL) C3Player::Pirate(uint32 u_id, 
									  BOOL *is_unknown_id, 
									  MapPointData *target_pos)
{
	return FALSE;
}

STDMETHODIMP_ (BOOL) C3Player::CanPirate(uint32 u_id, 
										 BOOL *is_unknown_id, 
										 MapPointData *target_pos)
{
	uint32 uindex;
	Army al = GetArmyList(u_id, is_unknown_id); 
	if (!al) return FALSE; 

	
	
	MapPoint army_pos;
	al.GetPos(army_pos);
	MapPoint ipos; 
	ipos.Norm2Iso(*target_pos);
	if (ipos == army_pos)
		return al.CanInterceptTrade(uindex);
	else
		return false;
}

STDMETHODIMP_ (BOOL) C3Player::IsEnemy(const PLAYER_INDEX & second)
{
	Assert(g_player[m_owner]);
	Assert(g_player[second]);

    uint32 a, b;

	if(g_player[m_owner] && g_player[second]) {
		a = ~(g_player[m_owner]->GetMaskAlliance()); 
		b = (0x00000001 << second); 

		return a & b;
	}
	return false;
}

STDMETHODIMP_ (BOOL) C3Player::HaveActiveAgreement(const PLAYER_INDEX & second, const AGREEMENT_TYPE &type)
{
	Agreement agreement;
	Assert(second >= 0);
	Assert(g_player[m_owner]);
	Assert(g_player[second]);
	
	agreement = g_player[m_owner]->FindAgreement(type, second);

	
	if (agreement == Agreement(0))
		return false;

	return (!agreement.IsExpired()&& !agreement.IsBroken());
}

STDMETHODIMP_ (BOOL) C3Player::ExecuteAIOrder(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos, uint32 order)
{
	return FALSE;
}

STDMETHODIMP_ (BOOL) C3Player::AbleToPlantNukeTarget(uint32 u_id, 
											  BOOL *is_unknown_id, 
											  MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToPlantNukeTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToMakeParkTarget(uint32 u_id, 
													BOOL *is_unknown_id, 
													MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToMakeParkTarget(ipos, uindex);
}


STDMETHODIMP_ (BOOL) C3Player::AbleToUndergroundRailwayTarget(uint32 u_id, 
															  BOOL *is_unknown_id, 
															  MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToUndergroundRailwayTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToConvertTarget(uint32 u_id,
												   BOOL *is_unknown_id,
												   MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToConvertTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToEstablishEmbassyTarget(uint32 u_id, 
															BOOL *is_unknown_id, 
															MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToEstablishEmbassyTarget(ipos, uindex);
}


STDMETHODIMP_ (BOOL) C3Player::AbleToCreateFranchiseTarget(uint32 u_id, 
														   BOOL *is_unknown_id, 
														   MapPointData *target_pos)
{
	Army al = GetArmyList(u_id, is_unknown_id); 
	if (!al) return FALSE; 

	MapPoint ipos; 
	ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToCreateFranchiseTarget(ipos, uindex);
}


STDMETHODIMP_ (BOOL) C3Player::AbleToAssasinateRulerTarget(uint32 u_id, 
														   BOOL *is_unknown_id, 
														   MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToAssasinateRulerTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToStealTechnologyTarget(uint32 u_id, 
														   BOOL *is_unknown_id, 
														   MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToStealTechnologyTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToInjoinTarget(uint32 u_id, 
												  BOOL *is_unknown_id, 
												  MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToInjoinTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToInciteRevolutionTarget(uint32 u_id, 
															BOOL *is_unknown_id, 
															MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToInciteRevolutionTarget(ipos,uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToCauseUnhappinessTarget(uint32 u_id, 
															BOOL *is_unknown_id, 
															MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToCauseUnhappinessTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToExpelTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos)
{
   Army al = GetArmyList(u_id, is_unknown_id); 
   if (!al) return FALSE; 

   MapPoint ipos; 
   ipos.Norm2Iso(*target_pos);
   sint32 uindex;
   return al.AbleToExpelTarget(ipos, uindex);
}

STDMETHODIMP_ (BOOL) C3Player::AbleToSueTarget(uint32 u_id, BOOL *is_unknown_id, MapPointData *target_pos)
{
    Army army; 
    if (m_ptr->AiGetArmy(*is_unknown_id, u_id, army) == FALSE) { 
        return FALSE; 
    }
    sint32 type; 
    sint32 num_army, idx_army; 
    num_army = army.Num(); 
    Assert(0 < num_army); 
	BOOL can_sue = FALSE;

	
    for (idx_army=0; idx_army<num_army; idx_army++) { 
        type = army.Get(idx_army).GetType(); 
        if (g_theUnitDB->Get(type)->GetCanSue()) { 
			can_sue = TRUE;
			break;
        } 
    }

	
	BOOL target_can_be_sued = FALSE;
	MapPoint ipos; 
	ipos.Norm2Iso(*target_pos);
    CellUnitList *a=NULL; 
   
    a = g_theWorld->GetArmyPtr(ipos); 
    Unit u=Unit(0); 
    if (!a || a->Num() < 1) 
		{ 
			return FALSE; 
        } 
	else 
		{ 
			u = a->GetTopVisibleUnit(m_owner); 
			if(u.m_id == (0))
				return FALSE; 
		}

	target_can_be_sued = g_theUnitDB->Get(u.GetType())->GetCanBeSued();

	return can_sue && target_can_be_sued;
}

STDMETHODIMP_(BOOL) C3Player::HasEmbassyWith(const PLAYER_INDEX &second)
{
	Assert(g_player[second]);
	Assert(second >= 0);
	return g_player[m_owner]->HasEmbassyWith(second);
}

STDMETHODIMP_ (void) C3Player::RejectTradeBid(uint32 bidId)
{
	g_theTradeBids->Reject(bidId);
}

STDMETHODIMP_(void) C3Player::AcceptTradeBid(uint32 bidId)
{
	g_theTradeBids->Accept(bidId);
}
	
BOOL C3Player::HasBuildingType(uint32 city_id, sint32 type)
{
   BOOL is_unknown_id;
   CityData *cd = GetCityData(city_id, &is_unknown_id); 

   Assert(type < g_theBuildingDB->NumRecords());
   Assert(type >= 0);

   return (cd->m_built_improvements & ((uint64)1 << type)) != 0; 
}

void C3Player::SellBuilding(uint32 city_id, sint32 type)
{
   BOOL is_unknown_id;
   CityData *cd = GetCityData(city_id, &is_unknown_id); 

   Assert(type < g_theBuildingDB->NumRecords());
   Assert(type >= 0);

   cd->SellBuilding(type);
}


sint32 C3Player::CityTurnsToBuild(uint32 city_id, BOOL *is_unknown_id)
{
   CityData *cd = GetCityData(city_id, is_unknown_id); 
 
   if (cd == NULL) { 
       return 0; 
   } else { 
       return cd->HowMuchLonger(); 
   }   
}


STDMETHODIMP_(void) C3Player::CancelTradeOffer(IC3TradeOffer *offer)
{
	Assert(m_owner == offer->GetOwner());
	if(m_owner == offer->GetOwner()) {
		TradeOffer offer(offer->GetID());
		g_player[m_owner]->WithdrawTradeOffer(offer);
	}
}

STDMETHODIMP_(void) C3Player::MakeTradeOffer(uint32 city, sint32 resource,
											 sint32 gold)
{
	m_ptr->AiMakeTradeOffer(city, resource, gold);
}

STDMETHODIMP_(BOOL) C3Player::PlayerCanSeeCity(uint32 cityId, 
											   sint32 otherPlayer)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	return (city.GetVisibility() & (1 << otherPlayer)) != 0;
}

STDMETHODIMP_(BOOL) C3Player::CanSeeHisCity(uint32 cityId,
											sint32 cityOwner)
{
	BOOL isUnknown;
	Unit city;
	Assert(g_player[cityOwner]);
	if(g_player[cityOwner]) {
		g_player[cityOwner]->AiGetCity(isUnknown, cityId, city);
		Assert(!isUnknown);
		if(!isUnknown) {
			return (city.GetVisibility() & (1 << m_owner)) != 0;
		}
	}
	return FALSE;
}

STDMETHODIMP_(sint32) C3Player::UnusedTradePoints()
{
	return m_ptr->m_tradeTransportPoints - m_ptr->m_usedTradeTransportPoints;
}


STDMETHODIMP_ (sint32) C3Player::GetCityPopulation (sint32 player, uint32 city_id,
                                                    BOOL *is_unknown_id)
{
    Unit city;
    Assert(g_player[player]);
    g_player[player]->AiGetCity(*is_unknown_id, city_id, city);

    if (*is_unknown_id) { 
        Assert(0); 
        return 0; 
    } 
    return city.PopCount();
}


STDMETHODIMP_ (sint32) C3Player::GetCityProduction (sint32 player, uint32 city_id,
                                                    BOOL *is_unknown_id)
{
    Unit city;
    Assert(g_player[player]);
    g_player[player]->AiGetCity(*is_unknown_id, city_id, city);

    if (*is_unknown_id) { 
        Assert(0); 
        return 0; 
    } 

    sint32 s, t, f; 
    city.GetProductionStats(s, t, f);
    return s;
}


STDMETHODIMP_ (sint32) C3Player::GetCityFood (sint32 player, uint32 city_id,
                                              BOOL *is_unknown_id)
{
    Unit city;
    Assert(g_player[player]);
    g_player[player]->AiGetCity(*is_unknown_id, city_id, city);

    
    if (*is_unknown_id) { 
        Assert(0); 
        return 0; 
    } 

    sint32 s, t, f; 
    city.GetProductionStats(s, t, f);
    return f;

}

STDMETHODIMP_ (sint32) C3Player::GetCityGold (sint32 player, uint32 city_id,
                                              BOOL *is_unknown_id)
{
    Unit city;
    Assert(g_player[player]);
    g_player[player]->AiGetCity(*is_unknown_id, city_id, city);

    
    if (*is_unknown_id) { 
        Assert(0); 
        return 0; 
    } 

    sint32 s, t, f; 
    city.GetProductionStats(s, t, f);
    return t;

}


STDMETHODIMP_(BOOL) C3Player::CityCanBuildUnit(THIS_ uint32 cityId, sint32 unitType)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		return city.CanBuildUnit(unitType);
	}
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CityCanBuildBuilding(THIS_ uint32 cityId, sint32 buildingType)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		return city.CanBuildBuilding(buildingType);
	}
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CityCanBuildWonder(THIS_ uint32 cityId, sint32 wonderType)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		return city.CanBuildWonder(wonderType);
	}
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CityCanBuildEndGameObject(THIS_ uint32 cityId, sint32 endGameType)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		return city.CanBuildEndGameObject(endGameType);
	}
	return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::CityCanBuildCapitalization(THIS_ uint32 cityId)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		return city.CanBuildCapitalization();
	}
	return FALSE;
}

STDMETHODIMP_(void) C3Player::AiStartMovingPops(THIS_ uint32 cityId)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		city.AccessData()->GetCityData()->AiStartMovingPops();
	}
}

STDMETHODIMP_(void) C3Player::AiDoneMovingPops(THIS_ uint32 cityId)
{
	BOOL isUnknown;
	Unit city;
	m_ptr->AiGetCity(isUnknown, cityId, city);
	Assert(!isUnknown);
	if(!isUnknown) {
		city.AccessData()->GetCityData()->AiDoneMovingPops();
	}
}

BOOL C3Player::IsOccupiedByForeigner(MapPointData *target_pos)
{
	CellUnitList*	a = NULL; 
	MapPoint ipos; 
	ipos.Norm2Iso(*target_pos);
	
	
	a = g_theWorld->GetArmyPtr( ipos );

	
	if ( a && a->Num() >= 1 )
		{
			
			if ( a->GetOwner() != m_owner )
				return TRUE;
		}
   return FALSE;
}

STDMETHODIMP_(BOOL) C3Player::GetCapitolPos(sint32 him, MapPointData *cap_pos)
{

    if (NULL == g_player[him]) { 
        return FALSE; 
    } 

    if (NULL == g_player[him]->m_capitol) { 
        return FALSE; 
    }

    if ((0) == g_player[him]->m_capitol->m_id) { 
        return FALSE; 
    }

    MapPoint ipos; 
    MapPoint npos; 
    g_player[him]->m_capitol->GetPos(ipos); 

    npos.Iso2Norm(ipos); 

    cap_pos->x = npos.x; 
    cap_pos->y = npos.y; 
    cap_pos->z = npos.z; 

    return TRUE; 
}


sint32 C3Player::GetNumAtWarWith(PLAYER_INDEX test_me)
{ 

    Assert(g_player[test_me]);

    sint32 player_idx; 
    sint32 count = 0; 

    for (player_idx=1; player_idx<k_MAX_PLAYERS; player_idx++) { 
        if ((test_me != player_idx) && g_player[player_idx]) { 
            if (DIPLOMATIC_STATE_WAR == g_player[test_me]->m_diplomatic_state[player_idx]) { 
                count++; 
            } 
        }
    }

    return count; 
}

double C3Player::GetCeaseFire(PLAYER_INDEX test_me, sint32 player_idx)
{ 
    Assert(g_player[test_me]);
	double test = (g_player[test_me]->m_diplomatic_state[player_idx]); 
	 
    if(g_player[test_me]->m_diplomatic_state[player_idx] == DIPLOMATIC_STATE_CEASEFIRE) 
	{ 
		return TRUE; 
    } 
    return FALSE; 
}


double C3Player::GetAlliance(PLAYER_INDEX test_me, sint32 player_idx)
{ 
    Assert(g_player[test_me]);

    if (DIPLOMATIC_STATE_ALLIED == g_player[test_me]->m_diplomatic_state[player_idx]) 
	{ 
		return TRUE; 
    } 
    return FALSE; 
}


double C3Player::GetWar(
	PLAYER_INDEX this_player, 
	sint32 test_player_one, 
	sint32 test_player_two
)
{ 
    Assert(g_player[test_player_one]);

    if (DIPLOMATIC_STATE_WAR == g_player[test_player_one]->m_diplomatic_state[test_player_two]) 
	{ 
		return TRUE; 
    } 
    return FALSE; 
}


double C3Player::GetNeutrality(PLAYER_INDEX test_me, sint32 player_idx)
{ 
    Assert(g_player[test_me]);

    if (DIPLOMATIC_STATE_NEUTRAL == g_player[test_me]->m_diplomatic_state[player_idx]) 
	{ 
		return TRUE; 
    } 
    return FALSE; 
}

double C3Player::GetPlayerType(PLAYER_INDEX test_me)
{ 
    Assert(g_player[test_me]);

	return g_player[test_me]->m_playerType;
	 
}




BOOL C3Player::IsRobotPlayer(PLAYER_INDEX test_me)
{
	Assert(g_player[test_me]);

	if (g_player[test_me] && 
		g_player[test_me]->m_playerType == PLAYER_TYPE_ROBOT)
		return TRUE;

	return FALSE;
}

uint32 C3Player::CreateUnit(sint32 type, MapPointData *pos, uint32 home_city)
{
	Unit new_unit;
	MapPoint isoPos;
	
	isoPos.Norm2Iso(*pos);

	
	if(g_network.IsClient())
		return 0;

	new_unit = g_player[m_owner]->CreateUnit(type,
		isoPos,  Unit(home_city), FALSE, CAUSE_NEW_ARMY_AI_TRANSPORT_CHEAT);

	if (new_unit.m_id != (0))
		return g_player[m_owner]->GetArmyId(new_unit);
	else
		return 0;
}

void C3Player::GetBestProfessionals(
    uint32 cityId, 
    BOOL *isUnknown, 
    sint32 *pop_best_scientist,     sint32 *pop_best_science, 
    sint32 *pop_best_grunt,     sint32 *pop_best_production, 
    sint32 *pop_best_musician,     sint32 *pop_best_luxury,
    sint32 *pop_best_banker,    sint32 *pop_best_gold
) 

{
    const CityData *cd = GetCityData(cityId, isUnknown); 
 
   if (cd == NULL) { 
       return; 
   } else { 
	}
}

sint32 C3Player::CountMyTradeToForeignCity(PLAYER_INDEX test_him, uint32 city_id, BOOL *is_unknown_id)
{

    Assert(g_player[test_him]); 
    if (NULL == g_player[test_him]) return 0; 

    BSetID *idx = g_player[test_him]->m_bset_cities_index->Find(city_id) ;

	
	*is_unknown_id = TRUE ;
	if (idx == NULL)
		return 0;

	*is_unknown_id = FALSE ;

    return g_player[test_him]->m_all_cities->Get(idx->GetVal()).GetData()->GetCityData()->CountTradeWith(m_ptr->m_owner);
}

STDMETHODIMP_(void) C3Player::SetRegard(PLAYER_INDEX who, sint32 level)
{
	REGARD_TYPE set;

        if (level <= k_REGARD_INSANE_HATRED) { 
            set = REGARD_TYPE_INSANE_HATRED;
        } else if ( level <= k_REGARD_HOTWAR) 
		    set = REGARD_TYPE_HOTWAR;
	    else if(level <= k_REGARD_COLDWAR)
		    set = REGARD_TYPE_COLDWAR;
	    else if ( level >= k_REGARD_LOVE)
		    set = REGARD_TYPE_LOVE;
	    else if(level >= k_REGARD_FRIENDLY)
	        set = REGARD_TYPE_FRIENDLY;
	    else
		    set = REGARD_TYPE_NEUTRAL; 


	m_ptr->m_regard->SetForPlayer(who, set);
}

STDMETHODIMP_(BOOL) C3Player::HeAgreed(PLAYER_INDEX who, AGREEMENT_TYPE agreement)
{
	DynamicArray<Agreement> *agreements = m_ptr->GetAgreements();
	sint32 i, n = agreements->Num();
	for(i = 0; i < n; i++) {
		if(agreements->Access(i).GetOwner() == m_owner &&
		   agreements->Access(i).GetRecipient() == who &&
		   agreements->Access(i).GetAgreement() == agreement) {
			return TRUE;
		}
	}
	return FALSE;
}


STDMETHODIMP_ (void) C3Player::GetArmyCurrentHP(uint32 u_id, 
    BOOL *is_unknown_id, sint32 *n, sint32 unit_type[100], 
    sint32 unit_hp[100])
{

    Army al = GetArmyList(u_id, is_unknown_id); 

    al.GetCurrentHP(*n, unit_type, unit_hp); 

}

STDMETHODIMP_ (BOOL) C3Player::ValidateArmyID(uint32 u_id, sint32 unit_num)
{
    BOOL is_unknown_id; 
    Army al = GetArmyList(u_id, &is_unknown_id); 
    Assert(al.Num() == unit_num); 

    return !is_unknown_id; 
}




STDMETHODIMP_ (void) C3Player::GetCityNumTradeRoutes(PLAYER_INDEX owner, uint32 u_id, 
    BOOL *is_unknown_id, sint32 *in_num, sint32 *in_max, sint32 *out_num, sint32 *out_max)
{ 
    BSetID *idx = g_player[owner]->m_bset_cities_index->Find(u_id) ;

	
	*is_unknown_id = TRUE ;
    if (idx == NULL) { 
        Assert(0); 
		return;
    }

	*is_unknown_id = FALSE ;

    CityData *cd = g_player[owner]->m_all_cities->Get(idx->GetVal()).GetData()->GetCityData();
    
    if (cd == NULL) { 
        Assert(0); 
        return; 
    } 

	const GovernmentRecord *grec = g_theGovernmentDB->Get(m_ptr->GetGovernmentType());

    *in_num = cd->GetIncomingTrade(); 
    *in_max = grec->GetMaxIncomingTrade();

    *out_num = cd->GetOutgoingTrade();
    *out_max = grec->GetMaxOutgoingTrade();
}

STDMETHODIMP_ (void) C3Player::GetAllRegard (sint32  player_idx, double i_like[k_MAX_PLAYERS])
{ 

} 


STDMETHODIMP_ (sint32) C3Player::GetSlavesPerMilitaryUnit()
{
	return g_theConstDB->SlavesPerMilitaryUnit();
}
STDMETHODIMP_ (BOOL) C3Player::BuildCapitalization(uint32 city_id, BOOL *is_unknown_id)
{
   CityData *cd = GetCityData(city_id, is_unknown_id); 
 
   if (cd == NULL) { 
       return FALSE; 
   } else { 
	   cd->BuildCapitalization();
       return TRUE;
   }        
}


BOOL C3Player::ArmyHasLeftMap(uint32 army_id, BOOL *is_unknown_id)
{ 
    MapPoint ipos; 

    *is_unknown_id = FALSE; 
	Army army;

	m_ptr->AiGetArmy(*is_unknown_id, army_id, army);

    return army.HasLeftMap();
}

sint32 C3Player::GetStage()
{
	if (m_ptr->m_endGame)
		{
			return m_ptr->m_endGame->GetStage();
		}
	return -1;
}

sint32 C3Player::GetNumberBuilt(sint32 type)
{
	if (m_ptr->m_endGame)
		{
			return m_ptr->m_endGame->GetNumberBuilt(type);
		}
	return -1;
}

BOOL C3Player::HasLab()
{
	if (m_ptr->m_endGame)
		{
			return m_ptr->m_endGame->HasLab();
		}
	return FALSE;
}

STDMETHODIMP_ (void)  C3Player::DipLogStr(sint32 player_idx, char *str)
{
    if (g_theDiplomacyLog) 
        g_theDiplomacyLog->LogStr(player_idx, str); 
}

STDMETHODIMP_ (BOOL) C3Player::IsActuallyVisible(sint32 player, 
												 uint32 army_id)
{
	if(player < 0 || player >= k_MAX_PLAYERS)
		return FALSE;
	if(!g_player[player])
		return FALSE;
	
	Army army;
	BOOL is_unknown_id;
	g_player[player]->AiGetArmy(is_unknown_id, army_id, army);
	if(is_unknown_id)
		return FALSE;
	sint32 i;
	for(i = 0; i <army.Num(); i++) {
		if(army[i].GetVisibility() & (1 << m_ptr->m_owner))
			return TRUE;
	}
	return FALSE;
}


double C3Player::GetProductionPercentBonus(uint32 city_id, BOOL &is_unknown_id,
										   const PRODUCTION_TYPE &type)
{
    CityData *cd = GetCityData(city_id, &is_unknown_id);

	if (cd == NULL)
		return 0.0;

	return 0;
}

