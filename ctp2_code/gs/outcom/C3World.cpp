











#include "c3.h"
#include "Globals.h"

#include "CityData.h"
#include "C3World.h"
#include "Cell.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TerrImprove.h"
#include "Wormhole.h"
#include "UnitRecord.h"

#include "Player.h"

#include "CivArchive.h"
#include "TerrImproveData.h"
#include "CellUnitList.h"
#include "InstallationTree.h"
#include "InstDB.h"
#include "MoveFlags.h"

#include "UnitData.h"

extern InstallationQuadTree  *g_theInstallationTree;





static MapPoint ipos;


STDMETHODIMP C3World::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3World)) {
		*obj = (IC3World*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3World::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3World::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}



C3World::C3World(Player *p)
{
    m_refCount = 0; 
    m_ptr = g_theWorld;
    m_player = p; 
    Assert(m_ptr); 
}

C3World::C3World(Player *p, CivArchive &archive)
{
    m_ptr = g_theWorld; 
    m_player = p; 
    Serialize(archive); 
}

void C3World::Serialize(CivArchive &archive) 
{
    CHECKSERIALIZE

    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 

    CHECKSERIALIZE
}

sint16 C3World::GetXWidth() 

{ 
    return sint16(g_theWorld->GetXWidth());
}

sint16 C3World::GetYHeight()  
{ 
    return sint16(g_theWorld->GetYHeight()); 
} 

sint16 C3World::GetZHeight() 
{
    return sint16(g_theWorld->GetZHeight()); 
}

double C3World::GetMoveCost(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 

    return g_theWorld->GetMoveCost(ipos); 
}

BOOL C3World::GetTileType(MapPointData *pos, TERRAIN_TYPES *t)

{

    ipos.Norm2Iso(*pos); 

    *t = g_theWorld->GetTerrainType(ipos); 
    return TRUE; 
}


extern uint16 myRGB(sint32 r,  sint32 g, sint32 b);



#ifdef _DEBUG

#endif

#ifdef CELL_COLOR
BOOL C3World::SetColor (MapPointData *pos, sint32 r, sint32 g, sint32 b)
{

    ipos.Norm2Iso(*pos); 

    uint16 c; 

    c = myRGB(r, g, b); 

    g_theWorld->SetColor(ipos, c);
    return TRUE; 
}


BOOL C3World::SetColor (MapPointData *pos, sint32 c)
{ 

    ipos.Norm2Iso(*pos); 
   
    g_theWorld->SetColor(ipos, uint16(c));
    return TRUE; 
} 

BOOL C3World::ClearColor()
{
    static MapPoint pos; 
   sint32 mx = g_theWorld->GetXWidth();
   sint32 my = g_theWorld->GetYHeight();
   sint32 mz = g_theWorld->GetZHeight(); 

   uint16 c = myRGB(0, 0, 0); 

   for (pos.z=0; pos.z < mz; pos.z++) { 
        for (pos.x=0; pos.x < mx; pos.x++) { 
            for (pos.y=0; pos.y<my; pos.y++) { 
                    g_theWorld->SetColor(pos, c);
            }
        }
   }
   return TRUE;
}

#endif

BOOL C3World::IsVisible(MapPointData *pos)
{


    ipos.Norm2Iso(*pos); 

    return m_player->IsVisible(ipos); 
}


BOOL C3World::IsExplored(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 


    return m_player->IsExplored(ipos); 
}

BOOL C3World::IOwnThis(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 


    return g_theWorld->GetOwner(ipos) == m_player->GetOwner(); 
}

BOOL C3World::IsRiver(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 

    return g_theWorld->IsRiver(ipos);
}

BOOL C3World::CanBeIrrigated(MapPointData *pos)
{
    ipos.Norm2Iso(*pos); 

    return g_theWorld->CanBeIrrigated(ipos);
}

BOOL C3World::IsTradeGood(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    
    return g_theWorld->IsGood(ipos);
}

BOOL C3World::IsWater (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    
    return g_theWorld->IsWater(ipos);
}

double C3World::GetDefenseBonus(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    
    return g_theWorld->GetDefenseBonus(ipos);
}

double C3World::GetTerrainDefenseBonus(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    
    return g_theWorld->GetCell(ipos)->GetTerrainDefenseBonus();
}

BOOL C3World::IsVisibleEnemyArmyThere (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 

    if (m_player->IsVisible(ipos) == FALSE) {  
        return FALSE;
    } 

    CellUnitList *a=NULL; 
   
    a = g_theWorld->GetArmyPtr(ipos); 

    if (a == NULL) { 
        return FALSE; 
    } else if (a->Num() < 1) { 
        return FALSE; 
    } else { 
        if (m_player->GetOwner() == a->GetOwner()) 
            return FALSE; 

        uint32 mask = 0x01 << m_player->GetOwner(); 

        sint32 i, n = a->Num(); 
        for (i=0; i<n; i++) { 
            if (a->Get(i).GetVisibility() & mask) 
                return TRUE;
        }


        
        return FALSE; 
    }
}

BOOL C3World::IsMyArmyThere (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    CellUnitList *a=NULL; 
   
    a = g_theWorld->GetArmyPtr(ipos); 

    if (!a || a->Num() < 1) { 
        return FALSE; 
    } else { 
        return m_player->GetOwner() == a->GetOwner(); 
    }
}


BOOL C3World::IsCellOwned (MapPointData *pos)
{


    ipos.Norm2Iso(*pos); 

    return (g_theWorld->GetOwner(ipos) != PLAYER_INDEX_INVALID) &&
        (g_theWorld->GetOwner(ipos) != -1); 
}

PLAYER_INDEX C3World::GetCellOwner (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 

    return g_theWorld->GetOwner(ipos); 
}

sint32 ic_count; 

BOOL C3World::IsCityHere (MapPointData *pos)
{
    ic_count++;

    ipos.Norm2Iso(*pos); 

    return g_theWorld->HasCity(ipos);
}

PLAYER_INDEX C3World::GetCityOwner (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    
    Unit c; 
    
    c = g_theWorld->GetCity(ipos);
	Assert(c);
	if (!c)
		return 0; 

    return c.GetOwner(); 
}


PLAYER_INDEX C3World::GetArmyOwner (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
  
    CellUnitList *ul =  g_theWorld->GetArmyPtr(ipos); 
    
    if (!ul) return -1; 

    return ul->GetOwner(); 

}

BOOL C3World::IsArmyHere(MapPointData *pos)

{

    ipos.Norm2Iso(*pos); 
  
    CellUnitList *ul = g_theWorld->GetArmyPtr(ipos); 

    if (ul == NULL) { 
        return FALSE; 
    } else { 
        return 0 < ul->Num(); 
    }
}


BOOL C3World::GetArmyId (MapPointData *pos, sint32 *player_id, 
     uint32 *a_id, sint32 *top_unit_type, sint32 *unit_num)
{

    ipos.Norm2Iso(*pos); 









    CellUnitList *a=NULL; 
   
    a = g_theWorld->GetArmyPtr(ipos); 
    Unit u=Unit(0); 
    if (!a || a->Num() < 1) { 
        *a_id = 0; 
        *top_unit_type = 0; 
        return FALSE; 
    } else if (g_theWorld->HasCity(ipos)) {
        Unit c = g_theWorld->GetCity(ipos); 

        if (c.GetOwner() != m_player->GetOwner()) { 
            *a_id = 0; 
            *top_unit_type = 0; 
            *unit_num = 0; 
            return FALSE;
        } else { 
             u = a->GetTopVisibleUnit(m_player->GetOwner()); 
			 if(u.m_id != (0)) {
				 *top_unit_type = u.GetType(); 
				 *player_id = u.GetOwner();
				 *a_id = g_player[*player_id]->GetArmyId(u); 
			 } else {
				 *a_id = 0;
				 *top_unit_type = 0;
				 *unit_num = 0;
			 }
			 *unit_num = a->Num(); 
             return TRUE; 
        } 
    } else { 
        
        g_theWorld->GetTopVisibleUnit(m_player->GetOwner(), ipos, u); 

        if (u.m_id == (0)) { 
            *a_id = 0; 
            *top_unit_type = 0; 
            *unit_num = 0; 
            return FALSE;
        } else { 
            *player_id = a->GetOwner();
            *top_unit_type = u.GetType(); 
            *a_id = g_player[*player_id]->GetArmyId(u); 
            if (0 == *a_id) { 
                return FALSE; 
            } 
            *unit_num = a->Num(); 
            return TRUE; 
        }
    }
}


BOOL C3World::GetCityId (MapPointData *pos, sint32 *player_id, uint32 *c_id)
{
   

    ipos.Norm2Iso(*pos); 









    Unit c; 
   
    c = g_theWorld->GetCity(ipos);
    if (c.m_id == (0)) { 
        *c_id = 0; 
        return FALSE; 
    } else {
        *player_id = c.GetOwner(); 
        *c_id = g_player[*player_id]->GetCityId(c); 
		Assert(*c_id);
        return TRUE;
    } 
}

BOOL C3World::IsWatchfulCityHere(MapPointData *pos)
{
    ipos.Norm2Iso(*pos); 

    Unit c; 
   
    c = g_theWorld->GetCity(ipos);
    if (c.m_id == (0)) { 
        return FALSE; 
    } else {
        return c.IsWatchful(); 
    } 
}



void C3World::GetCityAndArmyId( MapPointData* pos, sint32* player_id, 
							   uint32* a_id, sint32* top_unit_type, 
							   sint32* unit_num, uint32* c_id,
							   BOOL* unitVis, BOOL* cityVis, BOOL* mineVis )
{
	static MapPoint		ipos; 
    ipos.Norm2Iso( *pos );
	
	*unitVis = *cityVis = FALSE;
	
	*player_id = -1;
    *a_id = *top_unit_type = *unit_num = *c_id = 0;
	
	PLAYER_INDEX owner = m_player->GetOwner();
	
    if ( m_player->IsVisible( ipos ) )
	{
		
		Unit c = g_theWorld->GetCity( ipos );
		if ( c.m_id != 0)
		{
			*player_id = c.GetOwner(); 
			*c_id = g_player[ *player_id ]->GetCityId( c ); 
			Assert( *c_id );
			*cityVis = TRUE;
		}else { 
            *c_id = 0; 
        } 
		
		
		CellUnitList*	a = NULL; 
		
		a = g_theWorld->GetArmyPtr( ipos );
		
		if ( a && a->Num() >= 1 ) {
			*player_id = a->GetOwner();
			*unit_num = a->Num(); 

            *a_id = g_player[*player_id]->GetArmyId(a->Get(0)); 

			Unit u(0);
			g_theWorld->GetTopVisibleUnit(owner, ipos, u ); 
			
			if ( u.m_id != 0 ) { 
				*top_unit_type = u.GetType(); 				
				*unitVis = TRUE; 					
			}
		}
	}
	else
	{
		*top_unit_type = -1;
	} 
	

	*mineVis = false;
}

void C3World::GetCellContents( MapPointData* pos, sint32* player_id, 
							   uint32* a_id, sint32* top_unit_type, 
							   sint32* unit_num, uint32* c_id,
							   BOOL* unitVis, BOOL* cityVis, BOOL* mineVis, 
                               BOOL *can_be_expelled )
{
	static MapPoint		ipos; 
    ipos.Norm2Iso( *pos );
	
	*unitVis = *cityVis = FALSE;
	
	*player_id = -1;
    *a_id = *top_unit_type = *unit_num = *c_id = 0;
    *can_be_expelled= FALSE;  
	
	PLAYER_INDEX owner = m_player->GetOwner();
	
		
		Unit c = g_theWorld->GetCity( ipos );
		if ( c.m_id != 0){

			*player_id = c.GetOwner(); 
			*c_id = g_player[ *player_id ]->GetCityId( c ); 
			Assert( *c_id );
			*cityVis = TRUE;
        }

		
		CellUnitList*	a = NULL; 
		
		a = g_theWorld->GetArmyPtr( ipos );
		
		if ( a && a->Num() >= 1 ) {

            *can_be_expelled = a->CanBeExpelled(); 
			*player_id = a->GetOwner();
			*unit_num = a->Num(); 
            *a_id = g_player[*player_id]->GetArmyId(a->Get(0)); 

			Unit u(0);
			g_theWorld->GetTopVisibleUnit(owner,ipos, u ); 			
			if ( u.m_id != 0 ) { 
				*top_unit_type = u.GetType(); 				
				*unitVis = TRUE; 					
			}
        }

        

	*mineVis = false;
}

BOOL C3World::GetUnitData(MapPointData *pos, PLAYER_INDEX *owner, 
        sint32 *unit_num, sint32 unit_type[k_MAX_ARMY_SIZE], 
        sint32 unit_hp[k_MAX_ARMY_SIZE], 
        BOOL *is_entrenched)
{


	ipos.Norm2Iso(*pos); 

    CellUnitList *ul = g_theWorld->GetArmyPtr(ipos); 

    if (ul == NULL) { 
        *owner = -1; 
        return FALSE; 
    }

    sint32 i, n; 
    n = ul->Num(); 
    *unit_num = n;

    *is_entrenched = FALSE; 
    if (n < 1)  { 
        *owner = -1; 
        return FALSE; 
    } else {  
        *owner = ul->Get(0).GetOwner(); 
        for (i=0; i<n; i++) { 
            unit_type[i] = ul->Get(i).GetType(); 
            unit_hp[i]= sint32(ul->Get(i).GetHP()); 

            if (ul->Get(i).IsEntrenched()) { 
                *is_entrenched = TRUE; 
            } 
        } 

        
        for ( ; i<k_MAX_ARMY_SIZE; i++) { 
            unit_type[i]= -1;
            unit_hp[i] = -1; 
        } 

        return TRUE; 
    }
}

void C3World::GetTerrainCombatBonus (MapPointData *pos,
        double *terrain_bonus, double *fort_bonus, double *wall_bonus)
{


	ipos.Norm2Iso(*pos); 

    Cell *cell = g_theWorld->GetCell(ipos);
    *terrain_bonus = cell->GetTerrainDefenseBonus();

    *fort_bonus = g_theWorld->GetDefenseBonus(ipos); 

    *wall_bonus = 0.0; 
    if(cell->GetCity().m_id != (0)) {
		CityData *cityData = cell->GetCity().GetData()->GetCityData();
		Assert(cityData);
    	*wall_bonus += cityData->GetDefendersBonus();
	}
}


BOOL C3World::GetMoveType(MapPointData *p, uint32 *moveType)
{        




			ipos.Norm2Iso(*p); 

			*moveType = g_theWorld->GetMovementType(ipos);
			return TRUE;



} 

BOOL C3World::IsMoveTypeLand(MapPointData *p)
{        



        ipos.Norm2Iso(*p); 

        return 0 < (g_theWorld->GetMovementType(ipos) & k_BIT_MOVEMENT_TYPE_LAND);



} 
BOOL C3World::IsMoveTypeMountain(MapPointData *p)
{ 



        ipos.Norm2Iso(*p); 

        return 0 < (g_theWorld->GetMovementType(ipos) &  k_BIT_MOVEMENT_TYPE_MOUNTAIN);



} 

BOOL C3World::IsMoveTypeWater(MapPointData *p)
{     



        ipos.Norm2Iso(*p); 

        return 0 < (g_theWorld->GetMovementType(ipos) &  k_BIT_MOVEMENT_TYPE_WATER);



} 
BOOL C3World::IsMoveTypeShallowWater(MapPointData *p)
{        



        ipos.Norm2Iso(*p); 

        return 0 < (g_theWorld->GetMovementType(ipos) &  k_BIT_MOVEMENT_TYPE_SHALLOW_WATER);



} 

BOOL C3World::IsMoveTypeSpace(MapPointData *p)
{ 



        ipos.Norm2Iso(*p); 

        return 0 < (g_theWorld->GetMovementType(ipos) &  k_BIT_MOVEMENT_TYPE_SPACE);



} 


BOOL C3World::IsMoveTypeTrade(MapPointData *p)
{ 



        ipos.Norm2Iso(*p); 

        return 0 < (g_theWorld->GetMovementType(ipos) &  k_BIT_MOVEMENT_TYPE_TRADE);



} 

BOOL C3World::GetTerrainType (MapPointData *p, sint32 *type_terrain) 
{



        ipos.Norm2Iso(*p); 

        *type_terrain = g_theWorld->GetTerrainType(ipos);
        return TRUE; 




}


BOOL C3World::GetGood (MapPointData *p, sint32 *type_good)
{



        ipos.Norm2Iso(*p); 

		return g_theWorld->GetGood(ipos, *type_good);
#if 0
        if (g_theWorld->IsGood(ipos.x, ipos.y, ipos.z)) {  
            *type_good = g_theWorld->GetGood(ipos);
            *type_good -= k_BASE_TERRAIN_TYPES;
            return TRUE; 
        } else { 
            return FALSE; 
        } 
#endif





}

BOOL C3World::GetFood(MapPointData *pos, sint32 *food)
{ 



        ipos.Norm2Iso(*pos); 

        *food = g_theWorld->GetFoodProduced(ipos); 
        return TRUE; 




}

BOOL C3World::GetProd(MapPointData *pos, sint32 *prod)
{ 



        ipos.Norm2Iso(*pos); 

        *prod = g_theWorld->GetShieldsProduced(ipos); 
        return TRUE; 




}


BOOL C3World::IsMyPopThere(MapPointData *pos)
{    
        return FALSE;
}


void C3World::GetOpenTileValuesRow(sint32 player, sint32 len, MapPoint &pos, sint32 &tiles, MapPointData *openList)

{
    sint32 i; 
	sint32 r;

    for (i=0; i<len; i++, r = pos.GetNeighborPosition(EAST, pos)) { 

		if(!r) 
			break;
        
	    ((MapPoint*)(&openList[tiles]))->Iso2Norm(pos) ;
        tiles++; 
    } 

}


void C3World::GetOpenTileValues(sint32 player, MapPointData *p, sint32 *tiles, MapPointData *openList)
{
	static MapPoint	center,
				pos,
				tmp ;

	if (!IsExplored(p))
		{
		*tiles = 0 ;

		return ;
		}

	center.Norm2Iso(*p) ;
    *tiles=0; 

    
    do {
        if (!center.GetNeighborPosition(NORTHWEST, pos)) break;  
        if (!pos.GetNeighborPosition(NORTH, pos)) break; 
        
        GetOpenTileValuesRow(player, 2, pos, *tiles, openList); 
		
    } while(0); 

    do {
        if (!center.GetNeighborPosition(NORTHWEST, pos)) break;  
        if (!pos.GetNeighborPosition(NORTHWEST, pos)) break; 
        
        GetOpenTileValuesRow(player, 3, pos, *tiles, openList); 
    } while(0); 

    do {
        if (!center.GetNeighborPosition(NORTHWEST, pos)) break;  
        if (!pos.GetNeighborPosition(WEST, pos)) break; 
        
        GetOpenTileValuesRow(player, 4, pos, *tiles, openList); 
    } while(0); 

    do {
        if (!center.GetNeighborPosition(WEST, pos)) break;  
        
        GetOpenTileValuesRow(player, 1, pos, *tiles, openList); 

        if (!center.GetNeighborPosition(EAST, pos)) break;  
        
        GetOpenTileValuesRow(player, 1, pos, *tiles, openList); 

    } while(0); 

    
    do {
        if (!center.GetNeighborPosition(SOUTHWEST, pos)) return;  
        if (!pos.GetNeighborPosition(WEST, pos)) return; 
        
        GetOpenTileValuesRow(player, 4, pos, *tiles, openList); 
    } while(0); 

    do {
        if (!center.GetNeighborPosition(SOUTHWEST, pos)) return;  
        if (!pos.GetNeighborPosition(SOUTHWEST, pos)) return; 
        
        GetOpenTileValuesRow(player, 3, pos, *tiles, openList); 
    } while(0); 

    do {
        if (!center.GetNeighborPosition(SOUTHWEST, pos)) return;  
        if (!pos.GetNeighborPosition(SOUTH, pos)) return; 
        
        GetOpenTileValuesRow(player, 2, pos, *tiles, openList); 
    } while(0); 
}


BOOL C3World::ProvidesTrade(MapPointData *p)
{






		ipos.Norm2Iso(*p) ;

		return (g_theWorld->IsSupplyingTrade(ipos) > 0) ;



}

BOOL C3World::IsNextTo (MapPointData *a, MapPointData *b)
{
    static MapPoint iposa; 
    static MapPoint iposb;
    
    iposa.Norm2Iso(*a); 
    iposb.Norm2Iso(*b); 

    return iposa.IsNextTo(iposb); 
}

BOOL C3World::GetRoad (MapPointData *pos, sint32 *road_type)
{

    




    ipos.Norm2Iso(*pos); 

    uint32 env =  (g_theWorld->GetCell(ipos)->GetEnv() & k_MASK_ENV_ROAD);
    
    if (env) { 
        *road_type = ((env) >> k_SHIFT_ENV_ROAD) + 2;

        return TRUE; 
    } else { 
        return FALSE; 
    } 
}


BOOL C3World::GetConstructingRoad(MapPointData *pos, sint32 *road_type)
{ 

    ipos.Norm2Iso(*pos); 

    Cell *c = g_theWorld->GetCell(ipos);
    Assert(c); 

     
    sint32 n = c->GetNumImprovements();
    sint32 i;
    TERRAIN_IMPROVEMENT cell_imp; 

    if (0 < n) {
        TerrainImprovement imp;
        TerrainImprovementData *d=NULL;
        for (i=0; i<n; i++) { 
            imp = c->AccessImprovement(i);  
            
            cell_imp = imp.GetType(); 
            switch (cell_imp) { 
			
            
            
            
            
            
            
            default:
                break;
            } 
        }
    }
    return FALSE; 
}




BOOL C3World::IsConnectedToACity(MapPointData *pos, sint32 player_id)
{ 
	
	return TRUE;


    ipos.Norm2Iso(*pos); 

    return g_theWorld->IsConnectedToCity(ipos, player_id);
}












sint32 C3World::Get_XY_Travel_Distance
(
	const MapPointData &A,
	const MapPointData &B
)
{ 
    return g_theWorld->XY_Coords.Get_XY_Travel_Distance(A,B);
}














double C3World::Get_XY_Euclidean_Distance_Squared
(
	const MapPointData &A,
	const MapPointData &B
)
{ 
    return g_theWorld->XY_Coords.Get_XY_Euclidean_Distance_Squared(A,B);
}














void C3World::Get_Army_Types
(
	MapPointData *pos, 
	sint32 types_in_army[k_MAX_ARMY_SIZE],	
	sint32 &num_in_army					
)

{
	

	Cell *c;							
	sint32 i;							
	Unit a_unit;						
	

	
	ipos.Norm2Iso(*pos); 

	
    c = g_theWorld->GetCell(ipos);

	
    num_in_army = c->GetNumUnits();

	
	for (i = 0; i < num_in_army; i++)
	{
		
        a_unit = c->AccessUnit(i);

        
		types_in_army[i] = a_unit.GetType();

    }  

}














BOOL C3World::HasBonusFoodUnit(MapPointData *pos)
{
	

	Cell *c;							
	sint32 i;							
	sint32 unit_type;					
	sint32 num_in_army;					
	

	
	ipos.Norm2Iso(*pos); 

	
    c = g_theWorld->GetCell(ipos);

	
    num_in_army = c->GetNumUnits();

	
	for (i = 0; i < num_in_army; i++)
	{
		
        unit_type = c->AccessUnit(i).GetType();

        
		if (g_theUnitDB->Get(unit_type)->GetBonusFood())
			return TRUE;
    }  

	return FALSE;
}













sint32 C3World::GetConvertedTo(uint32 u_id, 
							   BOOL *is_unknown_id, 
							   MapPointData *target_pos)
{

    ipos.Norm2Iso(*target_pos); 
    Unit c; 

    if (!g_theWorld->HasCity(ipos))
		return -1;
    
    c = g_theWorld->GetCity(ipos);

	return c.IsConvertedTo();
}














BOOL C3World::GetSafeFromNukes(MapPointData *target_pos)
{

    ipos.Norm2Iso(*target_pos); 
    Unit c; 

    if (!g_theWorld->HasCity(ipos))
		return FALSE;
    
    c = g_theWorld->GetCity(ipos);

	return c.SafeFromNukes();
}














BOOL C3World::GetHasCityAirport(MapPointData *target_pos)
{

    ipos.Norm2Iso(*target_pos); 
    Unit c; 

    if (!g_theWorld->HasCity(ipos))
		return FALSE;
    
    c = g_theWorld->GetCity(ipos);

	return c.HasAirport();
}














BOOL C3World::GetHasForceField(MapPointData *target_pos)
{

    ipos.Norm2Iso(*target_pos); 
    Unit c; 

    if (!g_theWorld->HasCity(ipos))
		return FALSE;

    c = g_theWorld->GetCity(ipos);

	return c.HasForceField();
}














BOOL C3World::GetHasCityWalls(MapPointData *target_pos)
{

    ipos.Norm2Iso(*target_pos); 
    Unit c; 

    if (!g_theWorld->HasCity(ipos))
		return FALSE;

    c = g_theWorld->GetCity(ipos);

	return c.HasCityWalls();
}














sint32 C3World::GetFranchiseOwner(uint32 u_id, 
								  BOOL *is_unknown_id, 
								  MapPointData *target_pos)
{

    ipos.Norm2Iso(*target_pos); 
    Unit c; 

    if (!g_theWorld->HasCity(ipos)) { 
        *is_unknown_id = TRUE; 
		return -1;
    } 
    
    c = g_theWorld->GetCity(ipos);

    sint32 city_owner = c.GetOwner(); 
    if (g_player[city_owner]->AiGetCityID(c) != u_id) { 
       *is_unknown_id = TRUE; 
       return -1; 
    } else { 
       *is_unknown_id = FALSE; 
    }

	return c.GetFranchiseOwner();
}














uint32 C3World::GetNumImprovements (const MapPointData & pos)
{

	ipos.Norm2Iso(pos); 

	
    return g_theWorld->CountImprovements(ipos);
}













TERRAIN_IMPROVEMENT C3World::GetImprovementType (const MapPointData & pos, const uint32 & index)
{

    Cell *cell;
	ipos.Norm2Iso(pos); 

	
    cell = g_theWorld->GetCell(ipos);
	return cell->AccessImprovement(index).GetType();
}














PLAYER_INDEX C3World::GetImprovementOwner (const MapPointData & pos, const uint32 & index)
{

    Cell *cell;
	ipos.Norm2Iso(pos); 

	
    cell = g_theWorld->GetCell(ipos);
	sint32 o =  cell->AccessImprovement(index).GetOwner();
    Assert(g_player[o]); 
    o =  cell->AccessImprovement(index).GetOwner();
    return o; 
}





















































BOOL C3World::IsSafeFromNukes (const MapPointData & pos)
{

	ipos.Norm2Iso(pos); 

    return g_theWorld->IsSafeFromNukes(ipos); 
}













uint32 C3World::GetNumTradeRoutes (const MapPointData & pos)
{

	ipos.Norm2Iso(pos); 

	return g_theWorld->GetCell(ipos)->GetNumTradeRoutes();
}













PLAYER_INDEX C3World::GetTradeRouteSource (const MapPointData & pos, const uint32 index)
{

	ipos.Norm2Iso(pos); 

	Cell *cell = g_theWorld->GetCell(ipos);

	Assert(index >= 0);
	Assert(index < (uint32)cell->GetNumTradeRoutes());

	if(cell->GetNumTradeRoutes() > 0)
		{
			return cell->GetTradeRoute(index).GetSource().GetOwner();
		}
	return -1;
}













PLAYER_INDEX C3World::GetTradeRouteDestination(const MapPointData & pos, const uint32 index)
{
	Cell *cell;    

	ipos.Norm2Iso(pos); 

	cell = g_theWorld->GetCell(ipos);
	
	if(cell->GetNumTradeRoutes() < 1) {
		return -1;
	}

	Assert(index >= 0);
	Assert(index < (uint32)cell->GetNumTradeRoutes());

	if(cell->GetNumTradeRoutes() > 0) 
		{
			return cell->GetTradeRoute(index).GetDestination().GetOwner();
		}
	return -1;
}












sint32 C3World::GetTradeRouteGoldInReturn (const MapPointData & pos, const uint32 index)
{

	ipos.Norm2Iso(pos); 
	sint32 value=0;

	Cell *cell = g_theWorld->GetCell(ipos);

	Assert(index >= 0);
	Assert(index < (uint32)cell->GetNumTradeRoutes());

	if(cell->GetNumTradeRoutes() > 0) 
		{
			value = cell->GetTradeRoute(index).GetGoldInReturn();
		}
	return value;
}













BOOL C3World::IsGoodyHut (const MapPointData & pos)
{

	ipos.Norm2Iso(pos); 

	if(g_theWorld->GetCell(ipos)->GetGoodyHut() != NULL) 
		return TRUE;
	return FALSE;
}





BOOL C3World::GetIsChokePoint(const MapPointData &pos)
{

	ipos.Norm2Iso(pos); 

    return g_theWorld->GetIsChokePoint(ipos); 
}

sint16 C3World::GetContinent(const MapPointData &pos) 
{

    ipos.Norm2Iso(pos); 

    return g_theWorld->GetContinent(ipos); 
}

sint16 C3World::GetMinWaterContinent()
{
    return g_theWorld->GetMinWaterContinent();  
}

sint16 C3World::GetMaxWaterContinent() 
{
    return g_theWorld->GetMaxWaterContinent();
}

sint16 C3World::GetMinLandContinent() 
{
    return g_theWorld->GetMinLandContinent();
}

sint16 C3World::GetMaxLandContinent() 
{
    return g_theWorld->GetMaxLandContinent();
}








sint32 C3World::IsXwrap()
{
	return g_theWorld->IsXwrap();
}












sint32 C3World::IsYwrap()
{
	return g_theWorld->IsYwrap();
}







BOOL C3World::CanBuildUnderseaTunnel(MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    
    switch (g_theWorld->GetTerrainType(ipos)) { 
    case TERRAIN_WATER_SHALLOW:
    case TERRAIN_WATER_DEEP:
    case TERRAIN_WATER_VOLCANO:
    case TERRAIN_WATER_BEACH:
    case TERRAIN_WATER_SHELF:
    case TERRAIN_WATER_RIFT:
        return TRUE; 
    default:
        return FALSE; 
    } 
}
 

BOOL C3World::HasUnderseaTunnel(MapPointData  *pos)
{

    ipos.Norm2Iso(*pos); 

    return g_theWorld->IsTunnel(ipos);
}


sint32  C3World::GetLandContinentSize(sint32 cont_num) 
{
    return g_theWorld->GetLandContinentSize(cont_num);
}

sint32 C3World::GetWaterContinentSize(sint32 cont_num) 
{
    return g_theWorld->GetWaterContinentSize(cont_num);
}


BOOL C3World::IsWaterNextTooLand(const sint32 waterc, const sint32 landc)
{
    return g_theWorld->IsWaterNextTooLand(waterc, landc);
}

BOOL C3World::IsLandNextTooWater(const sint32 landc, const sint32 waterc) 
{
    return g_theWorld->IsLandNextTooWater(landc, waterc); 
}

MapValueStruct*** C3World::GetMapValue ()
{

}

sint16 C3World::GetSettleValue(sint16 x, sint16 y, sint16 z)
{

}


sint32 C3World::GetCitySlaveCount (MapPointData *pos)
{

    ipos.Norm2Iso(*pos); 
    Unit c; 
	PLAYER_INDEX player_id;
	sint32 slave_count = 0;
	uint32 c_id;
	BOOL is_unknown_id;
   
    c = g_theWorld->GetCity(ipos);
    if (c.m_id == (0)) 
        return -1; 

	player_id = c.GetOwner(); 
	c_id = g_player[player_id]->GetCityId(c);
	slave_count = 
		g_player[player_id]->GetCitySlaveCount(c_id, is_unknown_id); 
	Assert(!is_unknown_id);
    
	return slave_count;
}

BOOL C3World::IsWormholeVisible(const sint32 &player_id)
{
	if (!g_wormhole)
		return FALSE;
	return g_wormhole->IsVisible(player_id);
}

void C3World::GetWormholePos(MapPointData &pos)
{
	if (!g_wormhole)
		return;


	MapPoint npos;
	ipos = g_wormhole->GetPos();
	npos.Iso2Norm(ipos);
	pos = npos;
}


BOOL C3World::HasWormholeProbeEntered(const MapPointData &pos)
{
	if (!g_wormhole)
		return FALSE;

	
	return FALSE;
}


BOOL C3World::HasWormholeProbeReturned(const MapPointData &pos)
{
	if (!g_wormhole)
		return FALSE;

	
	return FALSE;
}

sint32 C3World::GetNumCities(const PLAYER_INDEX &player_index)
{
	if (g_player[player_index])
		{
			Assert(g_player[player_index]->m_all_cities);
			return g_player[player_index]->m_all_cities->Num();
		}
	return 0;
}

sint32 C3World::GetNumArmies(const PLAYER_INDEX &player_index)
{
	if (g_player[player_index])
		{
			Assert(g_player[player_index]->m_all_armies);
			return g_player[player_index]->m_all_armies->Num();
		}
	return 0;
}
	

