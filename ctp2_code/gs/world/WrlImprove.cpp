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
#include "World.h"

#include "Cell.h"
#include "installationtree.h"
#include "TerrainRecord.h"

sint32 Terrain::GetFood() const
{
    sint32  food    = m_Terrain->GetEnvBase()->GetFood();
    if (m_Terrain->HasEnvRiver())
    {
        food += m_Terrain->GetEnvRiverPtr()->GetFood();
    }

    return food;
};

sint32 Terrain::GetShield() const
{
    sint32  prod    = m_Terrain->GetEnvBase()->GetShield();
    if (m_Terrain->HasEnvRiver())
    {
        prod += m_Terrain->GetEnvRiverPtr()->GetShield();
    }

    return prod;
};

sint32 Terrain::GetGold() const
{
    sint32  gold    = m_Terrain->GetEnvBase()->GetGold();
    if (m_Terrain->HasEnvRiver())
    {
        gold += m_Terrain->GetEnvRiverPtr()->GetGold();
    }

    return gold;
}

void
World::InsertImprovement(const TerrainImprovement &imp, MapPoint const & pnt)
{
	GetCell(pnt)->InsertImprovement(imp);
}

void
World::RemoveImprovement(const TerrainImprovement &imp, const MapPoint &pnt)
{
	GetCell(pnt)->RemoveImprovement(imp);
}

void
World::InsertInstallation(Installation &inst, MapPoint const & pnt)
{
	GetCell(pnt)->m_env |= k_BIT_ENV_INSTALLATION;
	g_theInstallationTree->Insert(inst);
}

void 
World::RemoveInstallation(Installation &inst, MapPoint const & pnt)
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

  DynamicArray<Installation> l_array;
  g_theInstallationTree->GetAt(pos, l_array);

  return count + l_array.Num();
}


sint32 World::GetMaxFoodFromTerrain()
{
	static sint32 max_food = 0;

	if (max_food != 0)
		return max_food;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) 
    {
        max_food = std::max(max_food, Terrain(g_theTerrainDB->Get(i)).GetFood());
	}

	return max_food;
}


sint32 World::GetMaxShieldsFromTerrain()
{
	static sint32 max_prod = 0;

	if (max_prod != 0)
		return max_prod;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) 
    {
        max_prod = std::max(max_prod, Terrain(g_theTerrainDB->Get(i)).GetShield());
	}

	return max_prod;
}


sint32 World::GetAvgFoodFromTerrain()
{
	static sint32 avg_food = 0;

	if (avg_food != 0)
		return avg_food;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) 
    {
		avg_food += Terrain(g_theTerrainDB->Get(i)).GetFood();
	}
	avg_food /= g_theTerrainDB->NumRecords();

	return avg_food;
}


sint32 World::GetAvgShieldsFromTerrain()
{
	static sint32 avg_prod = 0;

	if (avg_prod != 0)
		return avg_prod;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) 
    {
		avg_prod += Terrain(g_theTerrainDB->Get(i)).GetShield();
	}
	avg_prod /= g_theTerrainDB->NumRecords();

	return avg_prod;
}


sint32 World::GetAvgGoldFromTerrain()
{
	static sint32 avg_gold = 0;

	if (avg_gold != 0)
		return avg_gold;

	for (sint32 i = 0; i < g_theTerrainDB->NumRecords(); i++) 
    {
		avg_gold += Terrain(g_theTerrainDB->Get(i)).GetGold();
	}
	avg_gold /= g_theTerrainDB->NumRecords();

	return avg_gold;
}
