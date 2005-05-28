//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Handling of tile improvements
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - GetShield calls replaced with GetFood calls in the food functions.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "installationtree.h"
#include "TerrainRecord.h"

void
World::InsertImprovement(const TerrainImprovement &imp, MapPoint &pnt)
{
	GetCell(pnt)->InsertImprovement(imp);
}

void
World::RemoveImprovement(const TerrainImprovement &imp, const MapPoint &pnt)
{
	GetCell(pnt)->RemoveImprovement(imp);
}

void
World::InsertInstallation(Installation &inst, MapPoint &pnt)
{
	GetCell(pnt)->m_env |= k_BIT_ENV_INSTALLATION;
	g_theInstallationTree->Insert(inst);
}

void 
World::RemoveInstallation(Installation &inst, MapPoint &pnt)
{
	g_theInstallationTree->Remove(inst);
	if(g_theInstallationTree->GetCount(pnt) <= 0) {
		GetCell(pnt)->m_env &= ~(k_BIT_ENV_INSTALLATION);
	}
}

sint32 World::CountImprovements(const MapPoint & pos)
{
  sint32 count = 0;
  Cell* cell = GetCell(pos);
  if(cell->m_env & k_MASK_ENV_ROAD)
	count++;
  if(cell->m_env & k_MASK_ENV_IRRIGATION)
	count++;
  if(cell->m_env & k_MASK_ENV_MINE)
	count++;

  static DynamicArray<Installation> array;
  array.Clear();
  g_theInstallationTree->GetAt(pos, array);
  count += array.Num();

  return count;

}


sint32 World::GetMaxFoodFromTerrain()
{
	sint32 food;
	const TerrainRecord *rec;
	static sint32 max_food = 0;

	if (max_food != 0)
		return max_food;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		rec = g_theTerrainDB->Get(i);
		food = rec->GetEnvBase()->GetFood();
		if (rec->GetEnvRiver())
			food += rec->GetEnvRiverPtr()->GetFood();

		if (food > max_food)
			max_food = food;
	}
	return max_food;
}


sint32 World::GetMaxShieldsFromTerrain()
{
	sint32 prod;
	const TerrainRecord *rec;
	static sint32 max_prod = 0;

	if (max_prod != 0)
		return max_prod;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		rec = g_theTerrainDB->Get(i);
		prod = rec->GetEnvBase()->GetShield();
		if (rec->GetEnvRiver())
			prod += rec->GetEnvRiverPtr()->GetShield();
		if (prod > max_prod)
			max_prod = prod;
	}
	return max_prod;
}


sint32 World::GetAvgFoodFromTerrain()
{
	sint32 food;
	const TerrainRecord *rec;
	static sint32 avg_food = 0;

	if (avg_food != 0)
		return avg_food;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		rec = g_theTerrainDB->Get(i);
		food = rec->GetEnvBase()->GetFood();
		if (rec->GetEnvRiver())
			food += rec->GetEnvRiverPtr()->GetFood();

		avg_food += food;
	}
	avg_food /= g_theTerrainDB->NumRecords();

	return avg_food;
}


sint32 World::GetAvgShieldsFromTerrain()
{
	sint32 prod;
	const TerrainRecord *rec;
	static sint32 avg_prod = 0;

	if (avg_prod != 0)
		return avg_prod;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		rec = g_theTerrainDB->Get(i);
		prod = rec->GetEnvBase()->GetShield();
		if (rec->GetEnvRiver())
			prod += rec->GetEnvRiverPtr()->GetShield();
		avg_prod += prod;
	}
	avg_prod /= g_theTerrainDB->NumRecords();

	return avg_prod;
}


sint32 World::GetAvgGoldFromTerrain()
{
	sint32 gold;
	const TerrainRecord *rec;
	static sint32 avg_gold = 0;

	if (avg_gold != 0)
		return avg_gold;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) {
		rec = g_theTerrainDB->Get(i);
		gold = rec->GetEnvBase()->GetGold();
		if (rec->GetEnvRiver())
			gold += rec->GetEnvRiverPtr()->GetGold();
		avg_gold += gold;
	}
	avg_gold /= g_theTerrainDB->NumRecords();

	return avg_gold;
}
