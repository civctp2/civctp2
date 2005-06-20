//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Everything about a terrain cell
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
// - Added CalcTerrainFreightCost by Martin Gühmann
// - Corrected handling of tile improvements that did not have a Freight 
//   modifier.
// - Standardised min/max usage.
// - Prevented some crashes.
// - Added GetFoodFromTerrain, GetShieldsFromTerrain and GetGoldFromTerrain
//   with a hypothetical terrain type argument to check whether there is a 
//   a good terraforming option. - Sep. 21st 2004 Martin Gühmann 
// - GetGoldProduced function now uses GetGoldFromTerrain function to avoid
//   duplicating code. - Sep. 21st 2004 Martin Gühmann
// - Moved Peter's good's fix to the according Get*FromTerrain functions.
//   - April 13th 2005 Martin Gühmann
// - Fix retrieval of good boni. - May 18th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Cell.h"
#include "TerrainRecord.h"
#include "TerrImprove.h"
#include "TerrainImprovementRecord.h"
#include "ResourceRecord.h" // For accessing the goods on the terrain

#include "pixelutils.h"
#include "tileutils.h"
#include "TileInfo.h"
#include "GoodyHuts.h"
#include "UnitRec.h"
#include "TradeDynArr.h"
#include "cellunitlist.h"
#include "Unit.h"
#include "UnitPool.h"
#include "player.h"
#include "terrainutil.h"
#include "MoveFlags.h"
#include "Globals.h"
#include "World.h"

extern void WhackScreen(); 

class CivArchive ;
extern UnitPool *g_theUnitPool;


int Cell::m_playerLandArea[k_MAX_PLAYERS+1];


int Cell::PlayerLandArea(int player)
{
	Assert(player>= -1);
	Assert(player<k_MAX_PLAYERS);

#ifdef _DEBUG
	
	
	
	
	
	
	
	static t=0;
	int t1 = m_playerLandArea[player+1];
	int arr[k_MAX_PLAYERS+1];
	g_theWorld->WholePlayerLandArea(arr);
	int t2 = arr[player+1];
	Assert(t1==t2);
#endif

	return m_playerLandArea[player+1];
}

#if 0







void Cell::RecalcPlayerLandArea()
{
	
	
	
	g_theWorld->WholePlayerLandArea(m_playerLandArea);
}
#endif



Cell::Cell()

{
    m_env = 0;
    m_terrain_type = (sint8)-1;
    m_move_cost = 1; 
#ifdef CELL_COLOR
    m_color = 0; 
#endif
    m_search_count = 0; 
    m_point = NULL; 
#ifdef BATTLE_FLAGS
	m_battleFlags = 0;
#endif
	
	m_cellOwner = -1;
	
	m_playerLandArea[0]++;

	m_unit_army = NULL;

    m_gf = 0; 
	m_zoc = 0;

	m_objects = NULL;

	m_jabba = NULL;
}

Cell::~Cell()
{
    if(m_unit_army) { 
		delete m_unit_army;
        m_unit_army = NULL; 
    } 

	if(m_objects) {
		delete m_objects;
		m_objects = NULL;
	}

	if(m_jabba) {
		delete m_jabba;
	}
	
	m_playerLandArea[m_cellOwner+1]--;
}


void Cell::Serialize(CivArchive &archive)
	{
	
    CHECKSERIALIZE

    m_search_count = 0; 
	if (archive.IsStoring()) {
		archive.StoreChunk((uint8 *)&m_env, ((uint8 *)&m_cellOwner)+sizeof(m_cellOwner));
	} else {
		
		
		
		m_playerLandArea[m_cellOwner+1]--;
		archive.LoadChunk((uint8 *)&m_env, ((uint8 *)&m_cellOwner)+sizeof(m_cellOwner));
		m_playerLandArea[m_cellOwner+1]++;
	}

	uint8 flags;
	if(archive.IsStoring()) {
		flags = 0;
		flags |= (m_unit_army != NULL);
		flags |= (m_objects != NULL) << 1;
		flags |= (m_jabba != NULL) << 2;

		archive << flags;
		if(m_unit_army)
			m_unit_army->Serialize(archive) ;   

		if(m_objects)
			m_objects->Serialize(archive);

		if(m_jabba)
			m_jabba->Serialize(archive);
			
	} else {
		archive >> flags;

		if(flags & 1) {
			m_unit_army = new CellUnitList;
			m_unit_army->Serialize(archive);
		} else {
			m_unit_army = NULL;
		}
		if(flags & 2) {
			m_objects = new DynamicArray<ID>;
			m_objects->Serialize(archive);
		} else {
			m_objects = NULL;
		}
		if(flags & 4) {
			m_jabba = new GoodyHut;
			m_jabba->Serialize(archive);
		} else {
			m_jabba = NULL;
		}
	}
}

sint32 Cell::IsAnyUnitInCell() const

{ 
	return m_unit_army != NULL;
}


sint32 Cell::InsertUnit(Unit id)

{
	if(!m_unit_army) {
		m_unit_army = new CellUnitList;
	}
    sint32 r = m_unit_army->Insert(id); 
    return r; 
}


sint32 Cell::RemoveUnitReference(const Unit &u)

{

   if (m_unit_army && m_unit_army->Del(u)) {
	   if(m_unit_army->Num() <= 0) {
		   delete m_unit_army;
		   m_unit_army = NULL;
	   }

       return TRUE; 
   } else if (u == GetCity()) { 
        SetCity(Unit(0));
        return TRUE;
   } else {

	   if(!g_theUnitPool->IsValid(u) || !u.IsBeingTransported()) {
		   
		   
		   Assert(FALSE);
	   }
       return FALSE;
   }

}


sint32 Cell::GetGoodsIndex(sint32 &val) const
{
    val = (m_env & k_MASK_ENV_GOOD);
    if (val == 0) {
        return FALSE;
	} else {
		val >>= k_SHIFT_ENV_GOOD;
		val--;
		while(val >= 0 && 
			  (g_theTerrainDB->Get(m_terrain_type)->GetNumResources() <= val)) {
			val--;
		}
		if(val < 0)
			return FALSE;
		return TRUE;
    }
}

BOOL Cell::CanEnter(const uint32 flag) const
{
	return ((m_env & k_MASK_ENV_MOVEMENT_TYPE) & (flag << k_SHIFT_ENV_MOVEMENT_TYPE)) != 0;
#if 0
	
	
	
	if((m_env & k_BIT_MOVEMENT_TYPE_LAND) &&
	   (flag & k_Unit_MovementType_Land_Bit))
		return TRUE;
	else if((m_env & k_BIT_MOVEMENT_TYPE_WATER) &&
			(flag & k_Unit_MovementType_Sea_Bit))
		return TRUE;
	else if((m_env & k_BIT_MOVEMENT_TYPE_AIR) &&
			(flag & k_Unit_MovementType_Air_Bit))
		return TRUE;
	else if((m_env & k_BIT_MOVEMENT_TYPE_MOUNTAIN) &&
			(flag & k_Unit_MovementType_Mountain_Bit))
		return TRUE;
	else if((m_env & k_BIT_MOVEMENT_TYPE_SHALLOW_WATER) &&
			(flag & k_Unit_MovementType_ShallowWater_Bit))
		return TRUE;
	else if((m_env & k_MASK_ENV_ROAD) &&
			(flag & k_Unit_MovementType_Land_Bit))
		return TRUE;
	else if((!(m_env & k_BIT_MOVEMENT_TYPE_SPACE)) &&
			(flag & k_Unit_MovementType_Air_Bit))
		return TRUE;

	return FALSE; 
#endif
}

BOOL Cell::GetCanDie(void) const 
{
	
	return (m_env & k_BIT_MOVEMENT_TYPE_LAND) != 0;
}


//----------------------------------------------------------------------------
//
// Name       : Governor::GetFoodFromTerrain
//
// Description: Gets the food from the given cell with the given terrain.
//
// Parameters : terrainType: The terrain type from that the statitics 
//                           should be calculated.
//
// Globals    : g_theTerrainDB: The terrain database
//
// Returns    : sint32 the ammount of food that can be produced in that cells
//              without tile improvements and the given terrain.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetFoodFromTerrain(sint8 terrainType) const
{
	const TerrainRecord *rec = g_theTerrainDB->Get(terrainType);
	
	sint32 food = rec->GetEnvBase()->GetFood();

	if(HasCity() && rec->GetEnvCity()) {
		food += rec->GetEnvCityPtr()->GetFood();
	}

	if(HasRiver() && rec->GetEnvRiver()) {
		food += rec->GetEnvRiverPtr()->GetFood();
	}

	sint32 good;
	if(g_theWorld->GetGood(this, good)) {
		food += g_theResourceDB->Get(good)->GetFood();
	}

	return food;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetFoodFromTerrain
//
// Description: Gets the food from the given cell with the current terrain.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32 the ammount of food that can be produced in that cells
//              without tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetFoodFromTerrain() const
{
	return GetFoodFromTerrain(m_terrain_type);
}


//----------------------------------------------------------------------------
//
// Name       : Governor::GetFoodProduced
//
// Description: Gets the food from the given cell with the current terrain 
//              including tile improvement.
//
// Parameters : -
//
// Globals    : g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : sint32 the ammount of food that can be produced in that cells
//              including tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetFoodProduced() const 
{
	sint32 food = GetFoodFromTerrain();
	size_t const	count	= m_objects ? m_objects->Num() : 0;
	for (size_t i = 0; i < count; ++i)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			const TerrainImprovementRecord *impRec = 
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			const TerrainImprovementRecord::Effect *effect;
			effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
			sint32 bonus;
			if(effect && effect->GetBonusFood(bonus)) {
				food += bonus;
			}
		}
	}																					
																					
    return food; 
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetShieldsFromTerrain
//
// Description: Gets the shields from the given cell with the given terrain.
//
// Parameters : terrainType: The terrain type from that the statitics 
//                           should be calculated.
//
// Globals    : g_theTerrainDB: The terrain database
//
// Returns    : sint32 the ammount of shields that can be produced in that cells
//              without tile improvements and the given terrain.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetShieldsFromTerrain(sint8 terrainType) const
{
	const TerrainRecord *rec = g_theTerrainDB->Get(terrainType);

	
    sint32 shield = rec->GetEnvBase()->GetShield();

	if(HasCity() && rec->GetEnvCity()) {
		shield += rec->GetEnvCityPtr()->GetShield();
	}

	if(HasRiver() && rec->GetEnvRiver()) {
		shield += rec->GetEnvRiverPtr()->GetShield();
	}

	sint32 good;
	if(g_theWorld->GetGood(this, good)) {
		shield += g_theResourceDB->Get(good)->GetProduction();
	}

	return shield;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetShieldsFromTerrain
//
// Description: Gets the shields from the given cell with the current terrain.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32 the ammount of shields that can be produced in that cells
//              without tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetShieldsFromTerrain() const
{
	return GetShieldsFromTerrain(m_terrain_type);
}


//----------------------------------------------------------------------------
//
// Name       : Governor::GetShieldsProduced
//
// Description: Gets the shields from the given cell with the current terrain 
//              including tile improvement.
//
// Parameters : -
//
// Globals    : g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : sint32 the ammount of shields that can be produced in that cells
//              including tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetShieldsProduced() const 
{
	sint32 shield = GetShieldsFromTerrain();

	size_t const	count	= m_objects ? m_objects->Num() : 0;
	for (size_t i = 0; i < count; ++i)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			const TerrainImprovementRecord *impRec = 
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			const TerrainImprovementRecord::Effect *effect;
			effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
			sint32 bonus;
			if(effect && effect->GetBonusProduction(bonus)) {
				shield += bonus;
			}
		}
	}																					
																					
    return shield; 
}


//----------------------------------------------------------------------------
//
// Name       : Governor::GetGoldFromTerrain
//
// Description: Gets the gold from the given cell with the given terrain.
//
// Parameters : terrainType: The terrain type from that the statitics 
//                           should be calculated.
//
// Globals    : g_theTerrainDB: The terrain database
//
// Returns    : sint32 the ammount of gold that can be produced in that cells
//              without tile improvements and the given terrain.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetGoldFromTerrain(sint8 terrainType) const
{
	const TerrainRecord *rec = g_theTerrainDB->Get(terrainType);

	
    sint32 gold = rec->GetEnvBase()->GetGold();

	if(HasCity() && rec->GetEnvCity()) {
		gold += rec->GetEnvCityPtr()->GetGold();
	}

	if(HasRiver() && rec->GetEnvRiver()) {
		gold += rec->GetEnvRiverPtr()->GetGold();
	}

	sint32 good;
	if(g_theWorld->GetGood(this, good)) {
		gold += g_theResourceDB->Get(good)->GetGold();
	}

	return gold;
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetGoldFromTerrain
//
// Description: Gets the gold from the given cell with the current terrain.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : sint32 the ammount of gold that can be produced in that cells
//              without tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetGoldFromTerrain() const
{
	return GetGoldFromTerrain(m_terrain_type);
}

//----------------------------------------------------------------------------
//
// Name       : Governor::GetGoldProduced
//
// Description: Gets the gold from the given cell with the current terrain 
//              including tile improvement.
//
// Parameters : -
//
// Globals    : g_theTerrainImprovementDB: The tile improvement database
//
// Returns    : sint32 the ammount of gold that can be produced in that cells
//              including tile improvements.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
sint32 Cell::GetGoldProduced() const 
{
	sint32 gold = GetGoldFromTerrain(); // Like in GetShieldProduced and in GetFoodProduced functions

	size_t const	count	= m_objects ? m_objects->Num() : 0;
	for (size_t i = 0; i < count; ++i)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			const TerrainImprovementRecord *impRec = 
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			const TerrainImprovementRecord::Effect *effect;
			effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
			sint32 bonus;
			if(effect && effect->GetBonusGold(bonus)) {
				gold += bonus;
			}
		}
	}																					
																					
    return gold; 
}

sint32 Cell::GetScore() const 

{
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);
	
    sint32 score = rec->GetEnvBase()->GetScore();

	if(HasCity() && rec->GetEnvCity()) {
		score += rec->GetEnvCityPtr()->GetScore();
	}

	if(HasRiver() && rec->GetEnvRiver()) {
		score += rec->GetEnvRiverPtr()->GetScore();
	}

	
	if ((m_env & k_MASK_ENV_GOOD) != 0x0) {
		// Should be moved to Goods.txt
		
		score += rec->GetEnvBase()->GetScore();
	}

    return score; 
}

#ifdef CELL_COLOR
void Cell::SetColor(sint32 c)

{
	return;

#ifdef _DEBUG
    m_color = c; 
    WhackScreen(); 
#endif
}
#endif

void Cell::AddTradeRoute(TradeRoute route)
{
	if(!m_objects) {
		m_objects = new DynamicArray<ID>;
	}
	if(!m_objects->IsPresent(route)) {
		m_objects->Insert(route);
	}
	m_env |= k_BIT_MOVEMENT_TYPE_TRADE;
}

void Cell::DelTradeRoute(TradeRoute route)
{
	if (m_objects)
	{
		m_objects->Del(route);
		size_t const	count	= m_objects->Num();
		
		for (size_t i = 0; i < count; ++i)
		{
			if ((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == 
					k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE
			   )
			{
				return;	// Some other trade route still passes through this cell.
			}
		}

		if (count <= 0)
		{
			delete m_objects;
			m_objects = NULL;
		}
	}

	// Mark cell free of trade.
	m_env &= ~(k_BIT_MOVEMENT_TYPE_TRADE);	
}

bool Cell::OwnsTradeRoute(const PLAYER_INDEX &owner) const
{
	if(!(m_env & k_BIT_MOVEMENT_TYPE_TRADE))
		return false;

	TradeRoute route;

	sint32 i;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == 
		   k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE) 
			{
				route = m_objects->Access(i).m_id;
				if (route.GetOwner() == owner ||
					route.GetPayingFor() == owner)
					return true;
			}
	}
	return false;
}

sint32 Cell::GetNumTradeRoutes() const
{
	if(!(m_env & k_BIT_MOVEMENT_TYPE_TRADE))
		return 0;


	sint32 i, c = 0;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE)
			c++;
	}
	return c;
}

TradeRoute Cell::GetTradeRoute(sint32 index) const
{
	static TradeRoute r(0);
	if(!m_objects)
		return r;

	sint32 i, c = 0;
	for(i = 0; i < m_objects->Num(); i++) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE) {
			if(c == index)
				return TradeRoute(m_objects->Access(i).m_id);
			c++;
		}
	}
	return r;
}

void Cell::InsertImprovement(const TerrainImprovement &imp)
{
	if(!m_objects) {
		m_objects = new DynamicArray<ID>;
	}

	m_env |= k_BIT_ENV_HAS_IMPROVEMENT;
	m_objects->Insert(imp);
}

void Cell::RemoveImprovement(const TerrainImprovement &imp)
{
	if (m_objects)
	{
		m_objects->Del(imp);
		size_t const	count	= m_objects->Num();
		
		for (size_t i = 0; i < count; ++i)
		{
			if ((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == 
					k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT
			   )
			{
				return;	// Some other improvement still present in this cell.
			}
		}

		if (count <= 0)
		{
			delete m_objects;
			m_objects = NULL;
		}
	}

	// Mark cell free of trade.
	m_env &= ~(k_BIT_ENV_HAS_IMPROVEMENT);	

}

void Cell::SetOwner(sint32 owner)
{
	
	m_playerLandArea[m_cellOwner+1]--;
	m_cellOwner = (sint8)owner;
	m_playerLandArea[m_cellOwner+1]++;
}

#ifdef BATTLE_FLAGS
void Cell::AddBattleFlag(sint32 victor)
{
	m_battleFlags |= (victor << k_BATTLE_FLAG_VICTOR_SHIFT);
	m_battleFlags |= k_BATTLE_FLAG_DECAY_TIME;
}

BOOL Cell::DecayBattleFlag()
{
	if(m_battleFlags & (k_MASK_DECAY_TIME)) {
		m_battleFlags--;
		if((m_battleFlags & k_MASK_DECAY_TIME) == 0) {
			return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}
#endif

double Cell::GetTerrainDefenseBonus()
{
    const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type); 
	return rec->GetEnvBase()->GetDefense();
}













uint32 Cell_CELL_GetVersion(void)
	{
	return (k_CELL_VERSION_MAJOR<<16 | k_CELL_VERSION_MINOR) ;
	}

void Cell::Kill(void)
	{
	m_terrain_type = (sint8)TERRAIN_DEAD ;
	}


sint32 Cell::GetNumUnits() const
{
	if(!m_unit_army)
		return 0;
	return m_unit_army->Num();
}


void Cell::GetArmy(CellUnitList &al)
{
    if(m_unit_army) 
		al = *m_unit_army;
	else
		al.Clear();
}

CellUnitList *Cell::UnitArmy()
{
	return m_unit_army;
}

Unit &Cell::AccessUnit(sint32 index)
{
	static Unit zero(0);
	Assert(m_unit_army);
	if(!m_unit_army) {
		return zero;
	}

	Assert(index >= 0 && index < m_unit_army->Num());
	if(index < 0 || index >= m_unit_army->Num()) {
		return zero;
	}

	return m_unit_army->Access(index);
}

void Cell::SetCity(const Unit &c)
{
	
	
	m_city.m_id = c.m_id;
	if(c.m_id != 0) {
		m_env |= (k_BIT_ENV_CITY | k_BIT_ENV_CITY_RADIUS);
	} else {
		m_env &= ~(k_BIT_ENV_CITY | k_BIT_ENV_CITY_RADIUS);
	}
}

Unit Cell::GetCity() const
{
	static Unit u(0);
	if(m_env & k_MASK_ENV_CITY) {
		
		
		
		return m_city;
	} else {
		
		
		
		return u;
	}
}

Unit Cell::GetCityOwner() const
{
	return m_city;
}

void Cell::SetCityOwner(const Unit &c)
{
	Assert(!(m_env & k_MASK_ENV_CITY) || (c.m_id == m_city.m_id) || (c.m_id == 0) );

	m_city.m_id = c.m_id;
	if(c.m_id != 0) {
		m_env |= k_BIT_ENV_CITY_RADIUS;
	} else {
		m_env &= ~(k_BIT_ENV_CITY_RADIUS);
	}
}


sint32 Cell::GetNumImprovements() 
{
	if(!(m_env & k_BIT_ENV_HAS_IMPROVEMENT))
		return 0;

	sint32 i, c = 0;

	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
			c++;
	}
	return c;
}

TerrainImprovement Cell::AccessImprovement(sint32 index)
{
	Assert(m_objects);
	Assert(m_env & k_BIT_ENV_HAS_IMPROVEMENT);
	if(m_objects && (m_env & k_BIT_ENV_HAS_IMPROVEMENT)) {
		sint32 i, c = 0;
		for(i = 0; i < m_objects->Num(); i++) {
			if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT) {
				if(c == index)
					return TerrainImprovement(m_objects->Access(i).m_id);
				c++;
			}
		}
	}
	Assert(FALSE);
	static TerrainImprovement t(0);
	return t;
}

void Cell::CreateGoodyHut()
{
	if ( !m_jabba ) {
		m_jabba = new GoodyHut();
	}
}

void Cell::DeleteGoodyHut()
{
	if (m_jabba != NULL) {
		delete m_jabba;
		m_jabba = NULL;
	}
}

BOOL Cell::HasWormhole() const
{
	return (m_env & k_BIT_ENV_HAS_WORMHOLE) != 0;
}

void Cell::SetWormhole(BOOL on)
{
	if(on)
		m_env |= k_BIT_ENV_HAS_WORMHOLE;
	else
		m_env &= ~(k_BIT_ENV_HAS_WORMHOLE);
}

void Cell::SetTerrain(sint32 terrain)
{
	m_terrain_type = (sint8)terrain;
	CalcMovementType();
	CalcTerrainMoveCost();
}

void Cell::CalcMovementType()
{
	m_env = (m_env & (~(k_MASK_ENV_MOVEMENT_TYPE))) | 
		(g_theTerrainDB->Get(m_terrain_type)->GetMovementType() << k_SHIFT_ENV_MOVEMENT_TYPE);
	if(m_env & (k_MASK_ENV_ROAD)) {
		m_env |= (k_Unit_MovementType_Land_Bit << k_SHIFT_ENV_MOVEMENT_TYPE);
	}

	for(sint32 i = 0; i < m_objects->Num(); i++) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE) {
			m_env |= (k_Unit_MovementType_Trade_Bit << k_SHIFT_ENV_MOVEMENT_TYPE);
			break;
		}
	}

	if(m_env & k_MASK_ENV_CANAL_TUNNEL) {
		m_env |= (k_Unit_MovementType_Land_Bit << k_SHIFT_ENV_MOVEMENT_TYPE);
	}
}

void Cell::ClearUnitsNStuff()
{
	
#ifdef BATTLE_FLAGS
	m_battleFlags = 0;
#endif
	
	
	SetOwner(-1);
	if(m_unit_army)
		delete m_unit_army;
	m_unit_army = NULL;

	sint32 i;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) != k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
			m_objects->DelIndex(i);
	}

	m_env &= ~(k_MASK_ENV_INSTALLATION | 
			   k_MASK_ENV_CITY |
			   k_MASK_ENV_ROAD |
			   k_MASK_ENV_IRRIGATION |
			   k_MASK_ENV_MINE |
			   k_MASK_ENV_CANAL_TUNNEL |
			   k_MASK_ENV_CITY_RADIUS);
}


sint16 Cell::GF() const
{
    return m_gf; 
} 

void Cell::SetGF(const sint16 v)
{
    m_gf = (sint8)v; 
}

void Cell::SetEnv(uint32 env)
{ 
	m_env = env; 
	CalcTerrainMoveCost();
}

void Cell::CalcTerrainMoveCost()
{
	double tmp;
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);
	sint32 base;
	bool gotMovement = rec->GetEnvBase()->GetMovement(base);
	Assert(gotMovement);
	tmp = base;

	sint32 m;
	if(HasCity() && rec->GetEnvCity() && rec->GetEnvCityPtr()->GetMovement(m)) {
		tmp = std::min(tmp, static_cast<double>(m));
	}

	if(HasRiver() && rec->GetEnvRiver() && rec->GetEnvRiverPtr()->GetMovement(m)) {
		tmp = std::min(tmp, static_cast<double>(m));
	}

	sint32 i;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			const TerrainImprovementRecord *impRec = 
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			const TerrainImprovementRecord::Effect *effect;
			effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
			sint32 cost;
			if(effect && effect->GetMoveCost(cost)) {
				tmp = std::min(tmp, static_cast<double>(cost));
			}
		}
	}																					
	
	sint16 new_cost = (sint16 )tmp;
	if (new_cost != m_move_cost)
	{
		g_theWorld->SetCapitolDistanceDirtyFlags(0xffffffff);
		m_move_cost = new_cost;
	}
	Assert(m_move_cost > 0);
}

//Added by Martin Gühmann

//----------------------------------------------------------------------------
//
// Name       : Cell::CalcTerrainFreightCost
//
// Description: Compute the freight cost of entering this cell.
//
// Parameters : -
//
// Globals    : g_theTerrainDB				: terrain properties
//              g_theTerrainImprovementDB	: terrain improvement properties
//
// Returns    : double	: freight cost of entering this cell
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

double Cell::CalcTerrainFreightCost()
{
	// Base terrain cost
	TerrainRecord const *	rec		= g_theTerrainDB->Get(m_terrain_type);
	sint32					cost	= rec->GetEnvBase()->GetFreight();

	// Modifications by special situations (city, river)
	if (HasCity() && rec->GetEnvCity()) 
	{
		cost = min(cost, rec->GetEnvCityPtr()->GetFreight());
	}
	if (HasRiver() && rec->GetEnvRiver()) 
	{
		cost = min(cost, rec->GetEnvRiverPtr()->GetFreight());
	}

	// Modifications by tile improvements (roads, etc.)
	for (sint32 i = m_objects->Num() - 1; i >= 0; --i) 
	{
		ID const &	object	= m_objects->Access(i);
		if (k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB == (object.m_id & k_ID_TYPE_MASK)) 
		{
			TerrainImprovementRecord const *			impRec = 
				g_theTerrainImprovementDB->Get(object.m_id & k_ID_KEY_MASK);
			TerrainImprovementRecord::Effect const *	effect =
				terrainutil_GetTerrainEffect(impRec, m_terrain_type);

			sint32	modifiedFreight;
			if (effect && effect->GetFreight(modifiedFreight)) 
			{
				cost = min(cost, modifiedFreight);
			}
		}
	}																					
	
	return static_cast<double>(cost);
}

GoodyHut *Cell::GetGoodyHut()
{
	return m_jabba;
}

sint32 Cell::GetNumObjects()
{
	return m_objects->Num();
}

ID Cell::GetObject(sint32 index)
{
	Assert(m_objects);
	if(m_objects)	  
		return m_objects->Access(index);

	static ID id(0);
	return id;
}

void Cell::InsertDBImprovement(sint32 type)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
	if(!m_objects)
		m_objects = new DynamicArray<ID>;
	uint32 id = k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB | type;

	sint32 i;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			const TerrainImprovementRecord *oldRec = 
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			if(oldRec->GetClass() & rec->GetExcludes()) {
				
				m_objects->DelIndex(i);
			} else if(m_objects->Access(i).m_id == id) {
				return;
			}
		}
	}

	m_objects->Insert(ID(id));
	if(rec->GetClassRoad()) {
		sint32 level = rec->GetLevel();
		Assert(level > 0 && level < 4);
		if(level > 0 && level < 4) {
			SetEnv((m_env & ~(k_MASK_ENV_ROAD)) | (level << k_SHIFT_ENV_ROAD));
			CalcMovementType();
		}
	}

	if(rec->GetClassOceanRoad()) {
		SetEnv(m_env | k_MASK_ENV_CANAL_TUNNEL | (k_Unit_MovementType_Land_Bit << k_SHIFT_ENV_MOVEMENT_TYPE));
	}
}

void Cell::RemoveDBImprovement(sint32 type)
{
	if(!m_objects)
		return;

	sint32 i;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
			m_objects->DelIndex(i);
	}
}

sint32 Cell::GetNumDBImprovements() const
{
	if(!m_objects)
		return 0;

	sint32 i, c = 0;
	for(i = m_objects->Num() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
			c++;
	}
	return c;
}

sint32 Cell::GetDBImprovement(sint32 index) const
{
	if(!m_objects)
		return -1;

	sint32 i, c = 0;
	for(i = m_objects->Num() -1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			if(c == index) {
				return m_objects->Access(i).m_id & k_ID_KEY_MASK;
			}
			c++;
		}
	}
	return -1;
}

sint32 Cell::IsDead() const
{ 
	return ((m_terrain_type == TERRAIN_DEAD)) ; 
}
