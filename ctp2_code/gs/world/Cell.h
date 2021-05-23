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
// - Added CalcTerrainFreightCost by Martin Gühmann
// - Added GetFoodFromTerrain, GetShieldsFromTerrain and GetGoldFromTerrain
//   with a hypothetical terrain type argument to check whether there is a
//   a good terraforming option. - Sep. 21st 2004 Martin Gühmann
// - Moved some Upgrade functionality from ArmyData. (Dec 24th 2006 Martin Gühmann)
// - Added methods to retrieve the future terrain move costs of tile
//   improvments under construction. (17-Jan-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CELL_H__
#define __CELL_H__ 1

class Cell;





#define k_MASK_ENV_ROAD         0x00000003
#define k_SHIFT_ENV_ROAD        0

#define k_MASK_ENV_IRRIGATION   0x0000000c
#define k_SHIFT_ENV_IRRIGATION  2

#define k_MASK_ENV_MINE         0x00000030
#define k_SHIFT_ENV_MINE        4

#define k_MASK_ENV_GOOD         0x000001c0
#define k_SHIFT_ENV_GOOD        6

#define k_ENV_GOOD1             1
#define k_BIT_ENV_GOOD1  (k_ENV_GOOD1 << k_SHIFT_ENV_GOOD)
#define k_ENV_GOOD2             2
#define k_BIT_ENV_GOOD2  (k_ENV_GOOD2 << k_SHIFT_ENV_GOOD)
#define k_ENV_GOOD3             3
#define k_BIT_ENV_GOOD3  (k_ENV_GOOD3 << k_SHIFT_ENV_GOOD)
#define k_ENV_GOOD4             4
#define k_BIT_ENV_GOOD4  (k_ENV_GOOD4 << k_SHIFT_ENV_GOOD)

#define k_MASK_ENV_CANAL_TUNNEL 0x00000200
#define k_SHIFT_ENV_CANAL_TUNNEL 9

#define k_MASK_ENV_CITY         0x00000400
#define k_SHIFT_ENV_CITY 10
#define k_BIT_ENV_CITY          (1 << k_SHIFT_ENV_CITY)

#define k_MASK_ENV_RIV_CUR      0x00000800
#define k_SHIFT_ENV_RIV_CUR     11
#define k_BIT_ENV_RIV_CUR      (1 << k_SHIFT_ENV_RIV_CUR)









#define k_MASK_ENV_INSTALLATION 0x00004000
#define k_SHIFT_ENV_INSTALLATION 14
#define k_BIT_ENV_INSTALLATION  (1 << k_SHIFT_ENV_INSTALLATION)

#define k_MASK_ENV_CITY_RADIUS 0x00008000
#define k_SHIFT_ENV_CITY_RADIUS 15
#define k_BIT_ENV_CITY_RADIUS (1 << k_SHIFT_ENV_CITY_RADIUS)

#define k_SHIFT_ENV_HAS_WORMHOLE 16
#define k_MASK_ENV_HAS_WORMHOLE (1 << k_SHIFT_ENV_HAS_WORMHOLE)
#define k_BIT_ENV_HAS_WORMHOLE k_MASK_ENV_HAS_WORMHOLE

#define k_SHIFT_ENV_HAS_POP 17
#define k_MASK_ENV_HAS_POP (1 << k_SHIFT_ENV_HAS_POP)
#define k_BIT_ENV_HAS_POP k_MASK_ENV_HAS_POP

#define k_SHIFT_ENV_HAS_IMPROVEMENT 18
#define k_MASK_ENV_HAS_IMPROVEMENT (1 << k_SHIFT_ENV_HAS_IMPROVEMENT)
#define k_BIT_ENV_HAS_IMPROVEMENT k_MASK_ENV_HAS_IMPROVEMENT

#define k_CELL_VERSION_MAJOR	0
#define k_CELL_VERSION_MINOR	0

#define k_BATTLE_FLAG_VICTOR_SHIFT 8
#define k_BATTLE_FLAG_DECAY_TIME 5
#define k_MASK_DECAY_TIME 0xff


template <class T> class DynamicArray;
class TradeRoute;
class TerrainImprovement;
class TileInfo;
class TradeDynamicArray;
class CivArchive;
class AstarPoint;
class GoodyHut;
class Unit;
class CellUnitList;
class ID;

class MapPoint;

#define CELL_COLOR 1

#include "gstypes.h"    // TERRAIN_TYPES
#include "player.h"     // PLAYER_INDEX
#include "Unit.h"


class Cell {

private:

//----------------------------------------------------------------------------
// Do not change anything in the types or order of the following variable
// declarations. Doing so will break reading in of save files.
// See the Serialize implementation for more details.
//----------------------------------------------------------------------------

	uint32 m_env;
	uint32 m_zoc;
	sint16 m_move_cost;

#ifdef BATTLE_FLAGS
	uint16 m_battleFlags;
#endif
	sint16 m_continent_number;
	sint8  m_gf;
	sint8  m_terrain_type;
	Unit   m_city;
	sint8  m_cellOwner;

//----------------------------------------------------------------------------
// Changing the order below this line should not break anything.
//----------------------------------------------------------------------------

	CellUnitList *m_unit_army;
	DynamicArray<ID> *m_objects;

#if 0
	Pop m_aPop;
	TradeDynamicArray *m_tradeRoutes;
	DynamicArray<TerrainImprovement> *m_improvements;

#endif
	GoodyHut *m_jabba;

public:

	bool m_cityHasVisibleTileImprovement;
	sint32 m_search_count;
	AstarPoint *m_point;
	sint16 m_tmp_future_move_cost;

	friend class World;
	friend class NetCellData;
	friend class NetCellList;

#ifdef CELL_COLOR
	int m_color;
#endif
	Cell();
	~Cell();

	double GetTmpFutureMoveCosts      ()  { return static_cast<double>(m_tmp_future_move_cost); };
	void   CalculateTmpFutureMoveCosts(sint32 tileImpType);
	void   ResetTmpFutureMoveCosts    ();
	bool   FutureMoveCostsAreReallyBig() const;

	bool IsAnyUnitInCell() const;
	bool InsertUnit(const Unit id);
	bool RemoveUnitReference(const Unit &id);

	sint64 IsZoc (sint32 UnitFlags, uint64 maskAlliance);
	uint32 GetRawZoc() { return m_zoc; }

	sint32 GetFoodFromTerrain() const;
	sint32 GetFoodProduced() const;
	sint32 GetShieldsFromTerrain() const;
	sint32 GetShieldsProduced() const;
	sint32 GetGoldFromTerrain() const;
	sint32 GetGoldProduced() const;
	sint32 GetScore() const;

	sint32 GetFoodFromTerrain(sint8 terrainType) const;
	sint32 GetShieldsFromTerrain(sint8 terrainType) const;
	sint32 GetGoldFromTerrain(sint8 terrainType) const;

	TERRAIN_TYPES GetTerrainType() const { return TERRAIN_TYPES(m_terrain_type); }

	sint32 GetGoodIndex() const;
	sint32 GetRoadIndex() const;
	sint32 GetIrrIndex() const;
	sint32 GetMineIndex() const;
	sint32 GetRivCurIndex() const;
	sint32 GetCanalTunnelIndex() const;
	bool GetCanDie(void) const ;

	void SetIrrigation(sint32 level);
	void SetMine(sint32 level);
	void SetRoad(sint32 level);
	void SetCanalTunnel(sint32 level);

	sint32 GetNumUnits() const;
	sint32 GetNumFortifiedUnits() const;
	void GetArmy(CellUnitList &al);
	CellUnitList * UnitArmy() const;
	Unit &AccessUnit(sint32 index);

	bool HasRiver() const { return (m_env & k_MASK_ENV_RIV_CUR) != 0; }
	void SetCity(const Unit &c);
	void SetCityOwner(const Unit &c);
	bool HasCity() const { return (m_env & k_MASK_ENV_CITY) != 0; }
	Unit GetCity() const;
	Unit GetCityOwner() const;

	uint32 GetEnv() { return m_env; }
	void SetEnv(uint32 env);

	void SetEnvFast(uint32 env) { m_env = env; }

	bool CanEnter(const uint32 flag) const;

	void SetTerrain(sint32 terrain);
	sint32 GetTerrain() const { return static_cast<sint32>(m_terrain_type); }
	void SetMoveCost(double cost) { m_move_cost = (sint16)cost; }
	double GetMoveCost() const { return double(m_move_cost); }
	double GetFutureTerrainMoveCost() const;

	bool GetIsChokePoint() const { return m_gf != 0; }
	sint16 GetContinent() const { return m_continent_number; }
	void SetContinent(sint16 val) { m_continent_number = val; }

	sint32 GetNumImprovements() const;
	TerrainImprovement AccessImprovement(sint32 index);

	void Serialize(CivArchive &archive) ;


	bool IsDead(void) const;
	void Kill(void);

	sint32 GetScratch(void) const { return (m_search_count) ; }
	void SetScratch(sint32 s) { m_search_count = s; }




	void AddTradeRoute(TradeRoute route);
	void DelTradeRoute(TradeRoute route);
	bool OwnsTradeRoute(const PLAYER_INDEX &owner) const;
	sint32 GetNumTradeRoutes() const;
	TradeRoute GetTradeRoute(sint32 index) const;


	sint32 GetNumObjects() const;
	ID GetObject(sint32 index);

#ifdef CELL_COLOR
	void SetColor(sint32 c);
#endif
	bool GetGoodsIndex (sint32 &val) const;

	void InsertImprovement(const TerrainImprovement &imp);
	void RemoveImprovement(const TerrainImprovement &imp);

	void InsertDBImprovement(sint32 dbType);
	void RemoveDBImprovement(sint32 dbType);
	sint32 GetNumDBImprovements() const;
	sint32 GetDBImprovement(sint32 index) const;
	bool HasTerrainImprovementOrInFuture(sint32 type) const;

#ifdef BATTLE_FLAGS
	void AddBattleFlag(sint32 player);
	bool DecayBattleFlag();
	uint16 GetBattleFlags() { return m_battleFlags; }
	uint16 GetBattleVictor() { return ((m_battleFlags >> k_BATTLE_FLAG_VICTOR_SHIFT) & 0xFF) ; }
	uint16 GetBattleDecay() { return (m_battleFlags & 0xFF) ; }
#endif

	void SetOwner(sint32 o);

	inline sint32 GetOwner(void) const {
		return ((sint32)(m_cellOwner)) ;
	}

#if 0
	TileInfo	*GetTileInfo(void) const;
	void		SetTileInfo(TileInfo *tileInfo) { m_tileInfo = tileInfo; }
#endif

	GoodyHut	*GetGoodyHut(void);
	void DeleteGoodyHut();

	void CreateGoodyHut();

	double GetTerrainDefenseBonus();

	bool HasWormhole() const;
	void SetWormhole(bool on);

	void ClearUnitsNStuff();
#ifdef CELL_COLOR
	void DebugUpdatePopColor();
#endif

	sint16 GF() const;
	void SetGF(const sint16 v);

	void CalcTerrainMoveCost();
	void CalcMovementType();

	double CalcTerrainFreightCost();
	sint32 GetBaseMoveCosts();

	bool IsUnitUpgradePosition(sint32 unitOwner) const;





private:

	static int m_playerLandArea[k_MAX_PLAYERS+1];
public:

	static int PlayerLandArea(int player);




	static void RecalcPlayerLandArea();
};

uint32 Cell_CELL_GetVersion(void) ;

#endif
