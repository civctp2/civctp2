//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Terrain improvement
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Sound added by Martin Gühmann
// - Crash fixed when there is no sound defined (for mod).
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TerrImprovePool.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "Cell.h"
#include "TerrainRecord.h"
#include "Installationtree.h"
#include "SlicEngine.h"
#include "TiledMap.h"
#include "Player.h"
#include "Vision.h"
#include "Globals.h"
#include "terrainutil.h"
#include "TerrainImprovementRecord.h"
#if !defined(ACTIVISION_ORIGINAL)
#include "SoundRecord.h"
#include "SoundManager.h"
#include "SelItem.h"
#endif

TerrainImprovementPool::TerrainImprovementPool() 
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
{
}

TerrainImprovementPool::TerrainImprovementPool(CivArchive &archive)
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
{
	Serialize(archive);
}

#if defined(ACTIVISION_ORIGINAL)
TerrainImprovement
TerrainImprovementPool::Create(sint32 owner,
							   MapPoint &point,
							   sint32 type,
							   sint32 extraData)
{
	TerrainImprovementData *newData;

	if(!terrainutil_GetTerrainEffect(g_theTerrainImprovementDB->Get(type), point) &&
	   !g_theTerrainImprovementDB->Get(type)->GetClassTerraform()) {
		
		return TerrainImprovement(0);
	}

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] && i != owner) {
			g_player[i]->m_vision->AddUnseen(point);
		}
	}

	if(g_tiledMap) {
		
		
		
		

		
		
		g_tiledMap->GetLocalVision()->AddUnseen(point);
	}

	TerrainImprovement newImprovement(NewKey(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT));
	
	newData = new TerrainImprovementData(newImprovement,
										 owner, point,
										 type, extraData);
	Insert(newData);
	g_theWorld->InsertImprovement(newImprovement, point);
	g_tiledMap->RedrawTile(&point);

	return newImprovement;
}
#else
//----------------------------------------------------------------------------
//
// Name       : TerrainImprovementPool::Create
//
// Description: Create a new terrain improvement.
//
// Parameters : owner				: owner of the terrain improvement
//				point				: map location to improve
//				type				: type of improvement
//				extraData			: ? 
//
// Globals    : g_theTerrainImprovementDB
//				g_player
//				g_tiledMap
//				g_theWorld
//				g_soundManager
//				g_selectedItem
//
// Returns    : TerrainImprovement	: the newly created improvement
//
// Remark(s)  : - Returns a dummy TerrainImprovement of type 0 when the input
//                data is invalid.
//
//----------------------------------------------------------------------------
TerrainImprovement
TerrainImprovementPool::Create(sint32 owner,
							   MapPoint &point,
							   sint32 type,
							   sint32 extraData)
{
	TerrainImprovementRecord const *	dbTerrainImprovement	= 
		g_theTerrainImprovementDB->Get(type);

	if ((!dbTerrainImprovement) ||
		(!terrainutil_GetTerrainEffect(dbTerrainImprovement, point) &&
	     !dbTerrainImprovement->GetClassTerraform()
		) 
       )
	{
		// Invalid or inapplicable improvement 
		return TerrainImprovement(0);
	}

	// Obscure the improvement for all but the owner
	for (sint32 i = 0; i < k_MAX_PLAYERS; i++) 
	{
		if (g_player[i] && (i != owner))
		{
			g_player[i]->m_vision->AddUnseen(point);
		}
	}
	if (g_tiledMap) 
	{
		g_tiledMap->GetLocalVision()->AddUnseen(point);
	}

	// Add the new improvement to the map
	TerrainImprovement newImprovement(NewKey(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT));
	TerrainImprovementData *	newData = 
		new TerrainImprovementData(newImprovement, owner, point, type, extraData);
	Insert(newData);
	g_theWorld->InsertImprovement(newImprovement, point);
	g_tiledMap->RedrawTile(&point);

	// Added by Martin Gühmann to make a sound play when a tile improvement is built.
	SoundRecord const *	soundRecord	= dbTerrainImprovement->GetSound();
	if (soundRecord)
	{
		sint32 const	soundID		= soundRecord->GetIndex();
		if ((soundID != -1) && (owner == g_selected_item->GetVisiblePlayer()))
		{
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32) 0, soundID, point.x, point.y);
		}
	}

	return newImprovement;
}
#endif

void 
TerrainImprovementPool::Serialize(CivArchive &archive)
{
	TerrainImprovementData *data;
	sint32	i,
			count = 0 ;

#define TERRIMPROVEPOOL_MAGIC 0x11223344

    CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive.PerformMagic(TERRIMPROVEPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		for (i=0; i<k_OBJ_POOL_TABLE_SIZE; i++)
			if(m_table[i])
				count++;

		archive<<count;
		for(i = 0; i < k_OBJ_POOL_TABLE_SIZE; i++) {
			if(m_table[i])
				((TerrainImprovementData*)(m_table[i]))->Serialize(archive);
		}
	} else {
		archive.TestMagic(TERRIMPROVEPOOL_MAGIC) ;
		ObjPool::Serialize(archive);

		archive>>count;
		for (i=0; i<count; i++) {
			data = new TerrainImprovementData(archive);
			Insert(data);
		}
	}
}

BOOL TerrainImprovementPool::HasImprovement(const MapPoint &point,
											TERRAIN_IMPROVEMENT type,
											sint32 extraData)
{
	return FALSE;
}


BOOL TerrainImprovementPool::HasAnyImprovement(const MapPoint &point)
{
	return (FALSE) ;
}


BOOL TerrainImprovementPool::CanHaveImprovement(const MapPoint &point,
												TERRAIN_IMPROVEMENT type,
												sint32 extraData)
{
	return FALSE;
}
