//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Terrain utilities
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
// - Added terrainutil_GetMinimumProductionCost to retrieve the minimum
//   costs of a tile improvement. (17-Jan-2008 Martin Gühmann)
// - Added terrainutil_GetHealRate to suppurt tileimprovement healing (24-Jul-2009 EPW)
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef TERRAIN_UTIL_H__
#define TERRAIN_UTIL_H__

#define k_MAX_GOODS_TYPES_PER_TERRAIN 4
#define k_MAX_MINE_TYPES 3

class MapPoint;
class Unit;



#include "TerrainImprovementRecord.h"

void terrainutil_Initialize();


const TerrainImprovementRecord *terrainutil_GetBestRoad(sint32 player, const MapPoint &pos);

sint32 terrainutil_GetTimeToBuild(const MapPoint &pos, sint32 fromType, sint32 toType);

sint32 terrainutil_GetProductionTime(sint32 impType, const MapPoint &pos, sint32 extraData);
sint32 terrainutil_GetProductionCost(sint32 impType, const MapPoint &pos, sint32 extraData);
sint32 terrainutil_GetBonusProductionExport(sint32 impType, const MapPoint &pos, sint32 extraData); //EMOD 4-5-2006
const TerrainImprovementRecord::Effect *terrainutil_GetTerrainEffect(const TerrainImprovementRecord *rec,
																	 const MapPoint &pos);
const TerrainImprovementRecord::Effect *terrainutil_GetTerrainEffect(const TerrainImprovementRecord *rec,
																	 sint32 terrainType);
void terrainutil_DoVision(const MapPoint &point);
bool terrainutil_PlayerHasAdvancesForTerrain(const TerrainImprovementRecord *rec, sint32 pl, sint32 terr);
bool terrainutil_PlayerHasAdvancesFor(const TerrainImprovementRecord *rec, sint32 pl);
bool terrainutil_CanPlayerBuild(const TerrainImprovementRecord *rec, sint32 pl, bool checkMaterials);
bool terrainutil_CanPlayerBuildAt(const TerrainImprovementRecord *rec, sint32 pl, const MapPoint &pos);
bool terrainutil_CanPlayerSpecialBuildAt(const TerrainImprovementRecord *rec, sint32 pl, const MapPoint &pos); //EMOD
bool terrainutil_CanPlayerSpecialBuildAt(sint32 impType, sint32 pl, const MapPoint &pos); //emod
void terrainutil_GetDefenseBonus(const MapPoint & pos, double & terrain_bonus, double & fort_bonus);
bool terrainutil_HasAirfield(const MapPoint & pos);
bool terrainutil_HasListeningPost(const MapPoint & pos);
bool terrainutil_HasFort(const MapPoint & pos);
bool terrainutil_HasRadar(const MapPoint & pos);
bool terrainutil_HasEndgame(const MapPoint & pos);
bool terrainutil_IsInstallation(const sint32 type);
bool terrainutil_HasUpgrader(const MapPoint & pos); //EMOD 5-30-2006
bool terrainutil_CanBeCaptured(const MapPoint & pos); //EMOD 5-30-2006
bool terrainutil_HasColony(const MapPoint & pos); //EMOD 5-30-2006
bool terrainutil_HasMinefield(const MapPoint & pos); //EMOD 5-30-2006
bool terrainutil_HasIrrigation(const MapPoint & pos); //EMOD 4-30-2007
bool terrainutil_HasWonder(const MapPoint & pos); //EMOD 4-30-2007
bool terrainutil_HasUrban(const MapPoint & pos); //EMOD 4-30-2007
double terrainutil_GetMaxVisionRange();
double terrainutil_GetVisionRange(const sint32 terrainType, const MapPoint & pos);

bool terrainutil_GetSomethingOwnsCell(MapPoint const &pos, sint32 owner, Unit ignoreCity);
void terrainutil_RemoveBorders(const MapPoint &center, sint32 owner, sint32 intRad, sint32 sqRad, Unit ignoreCity);

bool terrainutil_AdvanceEnablesImprovementForPlayer(sint32 player, sint32 advance, sint32 imp);
bool terrainutil_AdvanceEnablesImprovement(sint32 advance, sint32 imp);

double terrainutil_GetHealRate( const MapPoint & pos );

#define TERRAIN_TYPE            sint8

#define TERRAIN_FOREST terrainutil_GetForest()
#define TERRAIN_PLAINS terrainutil_GetPlains()
#define TERRAIN_TUNDRA terrainutil_GetTundra()
#define TERRAIN_GLACIER terrainutil_GetGlacier()
#define TERRAIN_GRASSLAND terrainutil_GetGrassland()
#define TERRAIN_DESERT terrainutil_GetDesert()
#define TERRAIN_SWAMP terrainutil_GetSwamp()
#define TERRAIN_JUNGLE terrainutil_GetJungle()
#define TERRAIN_MOUNTAIN terrainutil_GetMountain()
#define TERRAIN_HILL terrainutil_GetHill()
#define TERRAIN_WATER_SHALLOW terrainutil_GetWaterShallow()
#define TERRAIN_WATER_DEEP terrainutil_GetWaterDeep()
#define TERRAIN_WATER_VOLCANO terrainutil_GetWaterVolcano()
#define TERRAIN_WATER_BEACH terrainutil_GetWaterBeach()
#define TERRAIN_WATER_SHELF terrainutil_GetWaterShelf()
#define TERRAIN_WATER_TRENCH terrainutil_GetWaterTrench()
#define TERRAIN_WATER_RIFT terrainutil_GetWaterRift()
#define TERRAIN_DEAD terrainutil_GetDead()
#define TERRAIN_BROWN_HILL terrainutil_GetBrownHill()
#define TERRAIN_BROWN_MOUNTAIN terrainutil_GetBrownMountain()
#define TERRAIN_WHITE_HILL terrainutil_GetWhiteHill()
#define TERRAIN_WHITE_MOUNTAIN terrainutil_GetWhiteMountain()
#define TERRAIN_WATER_KELP terrainutil_GetWaterKelp()
#define TERRAIN_WATER_REEF terrainutil_GetWaterReef()

TERRAIN_TYPE terrainutil_GetForest();
TERRAIN_TYPE terrainutil_GetPlains();
TERRAIN_TYPE terrainutil_GetTundra();
TERRAIN_TYPE terrainutil_GetGlacier();
TERRAIN_TYPE terrainutil_GetGrassland();
TERRAIN_TYPE terrainutil_GetDesert();
TERRAIN_TYPE terrainutil_GetSwamp();
TERRAIN_TYPE terrainutil_GetJungle();
TERRAIN_TYPE terrainutil_GetMountain();
TERRAIN_TYPE terrainutil_GetHill();
TERRAIN_TYPE terrainutil_GetWaterShallow();
TERRAIN_TYPE terrainutil_GetWaterDeep();
TERRAIN_TYPE terrainutil_GetWaterVolcano();
TERRAIN_TYPE terrainutil_GetWaterBeach();
TERRAIN_TYPE terrainutil_GetWaterShelf();
TERRAIN_TYPE terrainutil_GetWaterTrench();
TERRAIN_TYPE terrainutil_GetWaterRift();
TERRAIN_TYPE terrainutil_GetDead();
TERRAIN_TYPE terrainutil_GetBrownHill();
TERRAIN_TYPE terrainutil_GetBrownMountain();
TERRAIN_TYPE terrainutil_GetWhiteHill();
TERRAIN_TYPE terrainutil_GetWhiteMountain();
TERRAIN_TYPE terrainutil_GetWaterKelp();
TERRAIN_TYPE terrainutil_GetWaterReef();


sint32 terrainutil_GetTerraformHillsImprovement();
sint32 terrainutil_GetTerraformPlainsImprovement();
sint32 terrainutil_GetTerraformForestImprovement();
sint32 terrainutil_GetTerraformGrasslandImprovement();

sint32 terrainutil_GetEndgameTileImpIndex();

sint32 terrainutil_GetMinimumProductionCost(sint32 type);
#endif
