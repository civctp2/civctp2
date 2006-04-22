//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Terrain improvement data
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
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Restored save game compatibilty. (April 22nd 2006 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _TERRIMPROVEDATA_H_
#define _TERRIMPROVEDATA_H_

#include "GameObj.h"
#include "MapPoint.h"
#include "gstypes.h"

#if 0
enum TERRAIN_IMPROVEMENT {
	TERRAIN_IMPROVEMENT_IRRIGATION_1,
	TERRAIN_IMPROVEMENT_IRRIGATION_2,
	TERRAIN_IMPROVEMENT_IRRIGATION_3,

	TERRAIN_IMPROVEMENT_ROAD_1,
	TERRAIN_IMPROVEMENT_ROAD_2,
	TERRAIN_IMPROVEMENT_ROAD_3,

	TERRAIN_IMPROVEMENT_MINE_1,
	TERRAIN_IMPROVEMENT_MINE_2,
    TERRAIN_IMPROVEMENT_MINE_3,

	TERRAIN_IMPROVEMENT_CANAL_TUNNEL,

    TERRAIN_IMPROVEMENT_TRANSFORM,
	TERRAIN_IMPROVEMENT_INSTALLATION 
  	                                

};
#else
typedef sint32 TERRAIN_IMPROVEMENT;
#endif

#include "ID.h"

class TerrainImprovementData : public GameObj
{
private:
	sint32 m_owner;
	sint32 m_type;
	MapPoint m_point;
	sint32 m_turnsToComplete;
	TERRAIN_TYPES m_transformType;
	sint32 m_materialCost;
	bool m_isComplete;
	bool m_isBuilding;
	sint32 m_materialBonus;

	friend class NetTerrainImprovement;

public:
	TerrainImprovementData(ID id,
						   sint32 owner,
						   MapPoint pnt,
						   sint32 type,
						   sint32 extraData);
	TerrainImprovementData(CivArchive &archive);
	TerrainImprovementData(ID id) : GameObj(id.m_id) {}

	sint32 GetType() const { return m_type; }

    sint32 GetOwner() const { return m_owner; }
	MapPoint GetLocation() const { return m_point; }
	sint32 GetCompletion() const { return m_turnsToComplete; }
	BOOL Complete(void);
	BOOL AddTurn(sint32 turns);
	sint32 PercentComplete() const;

	sint32 GetBonusProductionExport() const { return m_materialBonus; } //EMOD 4-5-2006
	sint32 GetMaterialCost() const { return m_materialCost; }
	bool IsBuilding() const { return m_isBuilding; }

	void StartBuilding();

	void Serialize(CivArchive &archive);
};

#endif
