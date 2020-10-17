//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Everything about a terrain cell
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
// - Added CalcTerrainFreightCost by Martin G�hmann
// - Corrected handling of tile improvements that did not have a Freight
//   modifier.
// - Standardised min/max usage.
// - Prevented some crashes.
// - Added GetFoodFromTerrain, GetShieldsFromTerrain and GetGoldFromTerrain
//   with a hypothetical terrain type argument to check whether there is a
//   a good terraforming option. - Sep. 21st 2004 Martin G�hmann
// - GetGoldProduced function now uses GetGoldFromTerrain function to avoid
//   duplicating code. - Sep. 21st 2004 Martin G�hmann
// - Moved Peter's good's fix to the according Get*FromTerrain functions.
//   - April 13th 2005 Martin G�hmann
// - Fix retrieval of good boni. - May 18th 2005 Martin G�hmann
// - Prevented crash with multiple instances of an improvement that is deleted.
// - Moved some Upgrade functionality from ArmyData. (Dec 24th 2006 Martin G�hmann)
// - Added methods to retrieve the future terrain move costs of tile
//   improvments under construction. (17-Jan-2008 Martin G�hmann)
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
#include "TradeDynArr.h"
#include "cellunitlist.h"
#include "Unit.h"
#include "UnitData.h"
#include "UnitPool.h"
#include "player.h"
#include "terrainutil.h"
#include "MoveFlags.h"
#include "Globals.h"
#include "World.h"
#include <limits>           // std::numeric_limits<sint16>::max()

extern void WhackScreen();

class CivArchive ;
extern UnitPool *g_theUnitPool;

int Cell::m_playerLandArea[k_MAX_PLAYERS+1];

int Cell::PlayerLandArea(int player)
{
	Assert(player>= -1);
	Assert(player<k_MAX_PLAYERS);

#ifdef _DEBUG
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
:   m_env                  (0),
    m_zoc                  (0),
    m_move_cost            (1),
#ifdef BATTLE_FLAGS
    m_battleFlags          (0),
#endif
    m_continent_number     (0),
    m_gf                   (0),
    m_terrain_type         (-1),
    m_city                 (),
    m_cellOwner            (-1),
    m_unit_army            (NULL),
    m_objects              (NULL),
    m_jabba                (NULL),
    m_cityHasVisibleTileImprovement(false),
    m_search_count         (0),
    m_point                (NULL),
    m_tmp_future_move_cost (std::numeric_limits<sint16>::max())
#ifdef CELL_COLOR
,   m_color                (0)
#endif
{
	m_playerLandArea[0]++;
}

Cell::~Cell()
{
	delete m_unit_army;
	delete m_objects;
	delete m_jabba;

	m_playerLandArea[m_cellOwner+1]--;  // static!
}

void Cell::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	m_search_count = 0;
	if(archive.IsStoring())
	{
		archive.StoreChunk((uint8 *)&m_env, ((uint8 *)&m_cellOwner)+sizeof(m_cellOwner));
	}
	else
	{
		m_playerLandArea[m_cellOwner+1]--;
		archive.LoadChunk((uint8 *)&m_env, ((uint8 *)&m_cellOwner)+sizeof(m_cellOwner));
		m_playerLandArea[m_cellOwner+1]++;
	}

	uint8 flags;
	if(archive.IsStoring())
	{
		flags = 0;
		flags |= (m_unit_army != NULL);
		flags |= (m_objects != NULL) << 1;
		flags |= (m_jabba != NULL) << 2;

		archive << flags;
		if(m_unit_army)
			m_unit_army->Serialize(archive);

		if(m_objects)
			m_objects->Serialize(archive);

		if(m_jabba)
			m_jabba->Serialize(archive);
	}
	else
	{
		archive >> flags;

		if(flags & 1)
		{
			m_unit_army = new CellUnitList;
			m_unit_army->Serialize(archive);
		}
		else
		{
			m_unit_army = NULL;
		}
		if(flags & 2)
		{
			m_objects = new DynamicArray<ID>;
			m_objects->Serialize(archive);
		}
		else
		{
			m_objects = NULL;
		}
		if(flags & 4)
		{
			m_jabba = new GoodyHut;
			m_jabba->Serialize(archive);
		}
		else
		{
			m_jabba = NULL;
		}

		m_tmp_future_move_cost = std::numeric_limits<sint16>::max();
	}
}

void Cell::CalculateTmpFutureMoveCosts(sint32 tileImpType)
{
	const TerrainImprovementRecord *impRec = g_theTerrainImprovementDB->Get(tileImpType);
	const TerrainImprovementRecord::Effect *effect =
	    terrainutil_GetTerrainEffect(impRec, m_terrain_type);

	sint32 cost;
	if(effect && effect->GetMoveCost(cost))
	{
		m_tmp_future_move_cost = static_cast<sint16>(cost);
	}
	else
	{
		m_tmp_future_move_cost = std::numeric_limits<sint16>::max();
	}
}

void Cell::ResetTmpFutureMoveCosts()
{
	m_tmp_future_move_cost = std::numeric_limits<sint16>::max();
}

bool Cell::FutureMoveCostsAreReallyBig() const
{
	return m_tmp_future_move_cost == std::numeric_limits<sint16>::max();
}

bool Cell::IsAnyUnitInCell() const
{
	return m_unit_army != NULL;
}

bool Cell::InsertUnit(Unit id)
{
	if(!m_unit_army)
	{
		m_unit_army = new CellUnitList;
	}

	return m_unit_army->Insert(id);
}

bool Cell::RemoveUnitReference(const Unit &u)
{
	if (m_unit_army && m_unit_army->Del(u))
	{
		if(m_unit_army->Num() <= 0)
		{
			delete m_unit_army;
			m_unit_army = NULL;
		}

		return true;
	}
	else if (u == GetCity())
	{
		SetCity(Unit());
		return true;
	}
	else
	{
		return false;
	}
}

bool Cell::GetGoodsIndex(sint32 &val) const
{
	val = (m_env & k_MASK_ENV_GOOD);
	if (val == 0)
	{
		return false;
	}
	else
	{
		val >>= k_SHIFT_ENV_GOOD;
		val--;
		while(val >= 0 &&
			  (g_theTerrainDB->Get(m_terrain_type)->GetNumResources() <= val)) {
			val--;
		}
		if(val < 0)
			return false;
		return true;
	}
}

bool Cell::CanEnter(const uint32 flag) const
{
	return ((m_env & k_MASK_ENV_MOVEMENT_TYPE) & (flag << k_SHIFT_ENV_MOVEMENT_TYPE)) != 0;
}

bool Cell::GetCanDie(void) const
{
	return g_theTerrainDB->Get(m_terrain_type)->GetCanDie();
}

//----------------------------------------------------------------------------
//
// Name       : Cell::GetFoodFromTerrain
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

	if(HasCity() && rec->HasEnvCity()) {
		food += rec->GetEnvCityPtr()->GetFood();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
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
// Name       : Cell::GetFoodFromTerrain
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
// Name       : Cell::GetFoodProduced
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
// Name       : Cell::GetShieldsFromTerrain
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

	if(HasCity() && rec->HasEnvCity()) {
		shield += rec->GetEnvCityPtr()->GetShield();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
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
// Name       : Cell::GetShieldsFromTerrain
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
// Name       : Cell::GetShieldsProduced
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
// Name       : Cell::GetGoldFromTerrain
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

	if(HasCity() && rec->HasEnvCity()) {
		gold += rec->GetEnvCityPtr()->GetGold();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
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
// Name       : Cell::GetGoldFromTerrain
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
// Name       : Cell::GetGoldProduced
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

	if(HasCity() && rec->HasEnvCity()) {
		score += rec->GetEnvCityPtr()->GetScore();
	}

	if(HasRiver() && rec->HasEnvRiver()) {
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
#if 0   // unreachable
#ifdef _DEBUG
    m_color = c;
    WhackScreen();
#endif // _DEBUG
#endif // unreachable
}
#endif // CELL_COLOR

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

	for (sint32 i = GetNumObjects() - 1; i >= 0; i--)
	{
		if ((m_objects->Access(i).m_id & k_ID_TYPE_MASK) ==
		    k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE
		   )
		{
			route = m_objects->Access(i).m_id;
			if (route.GetOwner() == owner ||
				route.GetPayingFor() == owner)
				return true;
		}
	}
	return false;
}

sint32 Cell::GetNumTradeRoutes() const // returns # of trade routes not regarding route.IsActive(), this is necessary for #256 to work!
{
	if(!(m_env & k_BIT_MOVEMENT_TYPE_TRADE))
		return 0;

	sint32 c = 0;
	for (sint32 i = GetNumObjects() - 1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE)
			c++;
	}
	return c;
}

TradeRoute Cell::GetTradeRoute(sint32 index) const // returns trade route not regarding route.IsActive(), this is necessary for #256 to work!
{
	if (m_objects)
	{
		sint32 c = 0;
		for (sint32 i = 0; i < m_objects->Num(); ++i)
		{
			if ((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TRADE_ROUTE)
			{
				if (c == index)
					return TradeRoute(m_objects->Access(i).m_id);
				c++;
			}
		}
	}

	return TradeRoute();
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
		while (m_objects->Del(imp))
		{
			// Check for and delete multiple instances
		}

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
	return m_unit_army ? m_unit_army->Num() : 0;
}

sint32 Cell::GetNumFortifiedUnits() const
{
	sint32 numFortifiedUnits = 0;

	if(m_unit_army != NULL)
	{
		for(sint32 i = 0; i < m_unit_army->Num(); ++i)
		{
			if(m_unit_army->Get(i)->IsEntrenched() || m_unit_army->Get(i)->IsEntrenching())
			{
				++numFortifiedUnits;
			}
		}
	}

	return numFortifiedUnits;
}

void Cell::GetArmy(CellUnitList &al)
{
	if(m_unit_army)
		al = *m_unit_army;
	else
		al.Clear();
}

CellUnitList * Cell::UnitArmy() const
{
	return m_unit_army;
}

Unit &Cell::AccessUnit(sint32 index)
{
	static Unit invalid;
	Assert(m_unit_army);
	if(!m_unit_army) {
		return invalid;
	}

	Assert(index >= 0 && index < m_unit_army->Num());
	if(index < 0 || index >= m_unit_army->Num()) {
		return invalid;
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
	if (m_env & k_MASK_ENV_CITY)
	{
		return m_city;
	}
	else
	{
		return Unit();
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

sint32 Cell::GetNumImprovements() const
{
	if(!(m_env & k_BIT_ENV_HAS_IMPROVEMENT))
		return 0;

	sint32 c = 0;

	for (sint32 i = GetNumObjects() - 1; i >= 0; i--)
	{
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

	Assert(false);
	return TerrainImprovement();
}

void Cell::CreateGoodyHut()
{
	if (!m_jabba)
	{
		m_jabba = new GoodyHut();
	}
}

void Cell::DeleteGoodyHut()
{
	delete m_jabba;
	m_jabba = NULL;
}

bool Cell::HasWormhole() const
{
	return (m_env & k_BIT_ENV_HAS_WORMHOLE) != 0;
}

void Cell::SetWormhole(bool on)
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

	for(sint32 i = 0; i < GetNumObjects(); i++) {
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

	SetOwner(PLAYER_UNASSIGNED);
	delete m_unit_army;
	m_unit_army = NULL;

	for (sint32 i = GetNumObjects() - 1; i >= 0; i--) {
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

sint32 Cell::GetBaseMoveCosts()
{
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);
	sint32 base = 0;
	bool gotMovement = rec->GetEnvBase()->GetMovement(base);
	Assert(gotMovement);

	sint32 m;
	if(HasCity() && rec->HasEnvCity() && rec->GetEnvCityPtr()->GetMovement(m)) {
		base = std::min(base, m);
	}

	if(HasRiver() && rec->HasEnvRiver() && rec->GetEnvRiverPtr()->GetMovement(m)) {
		base = std::min(base, m);
	}

	return base;
}

void Cell::CalcTerrainMoveCost()
{
	sint32 tmp = GetBaseMoveCosts();

	for (sint32 i =  GetNumObjects() - 1 ; i >= 0; --i)
	{
		if ((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
		{
			const TerrainImprovementRecord *impRec =
			    g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			const TerrainImprovementRecord::Effect *effect =
			    terrainutil_GetTerrainEffect(impRec, m_terrain_type);

			sint32 cost;
			if (effect && effect->GetMoveCost(cost))
			{
				tmp = std::min(tmp, cost);
			}
		}
	}

	sint16 new_cost = static_cast<sint16>(tmp);
	if (new_cost != m_move_cost)
	{
		g_theWorld->SetCapitolDistanceDirtyFlags(0xffffffff);
		m_move_cost = new_cost;
	}
	Assert(m_move_cost > 0);
}

//----------------------------------------------------------------------------
//
// Name       : Cell::GetFutureTerrainMoveCost
//
// Description: Returns the terrain move cost including the tile improvements
//              that are under construction and will be finished in the future.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : The future terrain move cost.
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
double Cell::GetFutureTerrainMoveCost() const
{
	sint32 tmp = m_move_cost;

	for (sint32 i = GetNumObjects() - 1; i >= 0; i--)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
		{
			sint32 type = TerrainImprovement(m_objects->Access(i).m_id).GetType();
			const TerrainImprovementRecord *impRec = g_theTerrainImprovementDB->Get(type);
			const TerrainImprovementRecord::Effect *effect =
			    terrainutil_GetTerrainEffect(impRec, m_terrain_type);

			sint32 cost;
			if (effect && effect->GetMoveCost(cost))
			{
				tmp = std::min(tmp, cost);
			}
		}
	}

	tmp = std::min(tmp, static_cast<sint32>(m_tmp_future_move_cost));

	return static_cast<double>(tmp);
}

//----------------------------------------------------------------------------
//
// Name       : Cell::HasTerrainImprovementOrInFuture
//
// Description: Checks whether a TerrainImprovment of type is in Cell, whether
//              complete or under construction
//
// Parameters : sint32 type                 : type of TerrainImprovement that
//                                            is to checked whether it is there
//                                            or under construction
//
// Globals    : -
//
// Returns    : bool
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool Cell::HasTerrainImprovementOrInFuture(sint32 type) const
{
	for (sint32 i = GetNumObjects() - 1; i >= 0; i--)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
		{
			if(TerrainImprovement(m_objects->Access(i).m_id).GetType() == type)
				return true;
		}
		else if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
		{
			if((m_objects->Access(i).m_id & k_ID_KEY_MASK) == type)
				return true;
		}
	}

	return !FutureMoveCostsAreReallyBig();
}

//----------------------------------------------------------------------------
//
// Name       : Cell::CalcTerrainFreightCost
//
// Description: Compute the freight cost of entering this cell.
//
// Parameters : -
//
// Globals    : g_theTerrainDB              : terrain properties
//              g_theTerrainImprovementDB   : terrain improvement properties
//
// Returns    : double  : freight cost of entering this cell
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
	if (HasCity() && rec->HasEnvCity())
	{
		cost = std::min(cost, rec->GetEnvCityPtr()->GetFreight());
	}
	if (HasRiver() && rec->HasEnvRiver())
	{
		cost = std::min(cost, rec->GetEnvRiverPtr()->GetFreight());
	}

	// Modifications by tile improvements (roads, etc.)
	for (sint32 i = GetNumObjects() - 1; i >= 0; --i)
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
				cost = std::min(cost, modifiedFreight);
			}
		}
	}
	return static_cast<double>(cost);
}

GoodyHut *Cell::GetGoodyHut()
{
	return m_jabba;
}

sint32 Cell::GetNumObjects() const
{
	return m_objects ? m_objects->Num() : 0;
}

ID Cell::GetObject(sint32 index)
{
	Assert(m_objects);
	if(m_objects)
		return m_objects->Access(index);

	return ID();
}

void Cell::InsertDBImprovement(sint32 type)
{
	const TerrainImprovementRecord *rec = g_theTerrainImprovementDB->Get(type);
	if(!m_objects)
		m_objects = new DynamicArray<ID>;
	uint32 id = k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB | type;

	for (sint32 i = m_objects->Num() - 1; i >= 0; i--)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
		{
			const TerrainImprovementRecord *oldRec =
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);
			if(oldRec->GetClass() & rec->GetExcludes())
			{
				m_objects->DelIndex(i);
			}
			else if(m_objects->Access(i).m_id == id)
			{
				Assert(false);
				return;
			}
		}
	}

	m_objects->Insert(ID(id));
	if(rec->GetClassRoad())
	{
		sint32 level = rec->GetLevel();
		Assert(level > 0 && level < 4);
		if(level > 0 && level < 4)
		{
			SetEnv((m_env & ~(k_MASK_ENV_ROAD)) | (level << k_SHIFT_ENV_ROAD));
			CalcMovementType();
		}
	}

	if(rec->GetClassOceanRoad())
	{
		SetEnv(m_env | k_MASK_ENV_CANAL_TUNNEL | (k_Unit_MovementType_Land_Bit << k_SHIFT_ENV_MOVEMENT_TYPE));
	}
}

// Seems to remove all tile improvements
void Cell::RemoveDBImprovement(sint32 type)
{
	for (sint32 i = GetNumObjects() - 1; i >= 0; i--)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
			m_objects->DelIndex(i);
	}
}

// All finished tile improvements?
sint32 Cell::GetNumDBImprovements() const
{
	sint32 c = 0;

	for (sint32 i = GetNumObjects() - 1; i >= 0; i--)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB)
			c++;
	}

	return c;
}

sint32 Cell::GetDBImprovement(sint32 index) const
{
	if(!m_objects)
		return -1;

	sint32 c = 0;
	for (sint32 i = m_objects->Num() -1; i >= 0; i--) {
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			if(c == index) {
				return m_objects->Access(i).m_id & k_ID_KEY_MASK;
			}
			c++;
		}
	}
	return -1;
}

bool Cell::IsDead() const
{
	return (m_terrain_type == TERRAIN_DEAD);
}

bool Cell::IsUnitUpgradePosition(sint32 unitOwner) const
{
	// For now just stick to the city approach
	//return HasCity();

	// ToDo improve efficency
	// Add an owner check
	// -> Also a check for figureing out if on unowned terrain can be upgraded
	const TerrainRecord *rec = g_theTerrainDB->Get(m_terrain_type);

	bool canUpgrade = rec->GetEnvBase()->GetCanUpgrade();

	if(HasCity() && rec->HasEnvCity())
	{
		canUpgrade |= rec->GetEnvCityPtr()->GetCanUpgrade();
	}

	if(HasRiver() && rec->HasEnvRiver())
	{
		canUpgrade |= rec->GetEnvRiverPtr()->GetCanUpgrade();
	}

	sint32 good;
	if(g_theWorld->GetGood(this, good))
	{
		canUpgrade |= g_theResourceDB->Get(good)->GetCanUpgrade();
	}

	size_t const	count	= m_objects ? m_objects->Num() : 0;
	for(size_t i = 0; i < count; ++i)
	{
		if((m_objects->Access(i).m_id & k_ID_TYPE_MASK) == k_BIT_GAME_OBJ_TYPE_IMPROVEMENT_DB) {
			const TerrainImprovementRecord *impRec =
				g_theTerrainImprovementDB->Get(m_objects->Access(i).m_id & k_ID_KEY_MASK);

			canUpgrade |= impRec->GetCanUpgrade();

			const TerrainImprovementRecord::Effect* effect = terrainutil_GetTerrainEffect(impRec, m_terrain_type);
			if(effect != NULL)
			{
				canUpgrade |= effect->GetCanUpgrade();
			}
		}
	}

	return canUpgrade;
}
