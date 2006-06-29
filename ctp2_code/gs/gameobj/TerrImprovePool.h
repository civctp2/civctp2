








#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TERRIMPROVEPOOL_H_
#define _TERRIMPROVEPOOL_H_

#include "ObjPool.h"

#include "TerrImprove.h"

class CivArchive;

class TerrainImprovementPool : public ObjPool
{
private:
public:
	TerrainImprovementPool();
	TerrainImprovementPool(CivArchive &archive);

	TerrainImprovementData *AccessTerrainImprovement(const TerrainImprovement id)
	{
		return (TerrainImprovementData*)Access(id);
	}

	TerrainImprovementData *GetTerrainImprovement(const TerrainImprovement id)
	{
		return (TerrainImprovementData*)Get(id);
	}

	TerrainImprovement Create(sint32 owner,
							  MapPoint const & pnt,
							  sint32 type,
							  sint32 extraData);
	void Remove(TerrainImprovement id);
	BOOL HasImprovement(const MapPoint &point,
						TERRAIN_IMPROVEMENT type,
						sint32 extraData);
	BOOL HasAnyImprovement(const MapPoint &point) ;
	BOOL CanHaveImprovement(const MapPoint &point,
							TERRAIN_IMPROVEMENT type,
							sint32 extraData);

	void Serialize(CivArchive &archive);
};

extern TerrainImprovementPool *g_theTerrainImprovementPool;
#endif
