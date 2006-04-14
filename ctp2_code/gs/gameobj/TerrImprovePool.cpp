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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Sound added by Martin Gühmann
// - Crash fixed when there is no sound defined (for mod).
// - Moved network handling from TerrainImprovementData constructor to prevent 
//   reporting the temporary when completing the tile improvement.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TerrImprovePool.h"
#include "World.h"
#include "Cell.h"
#include "TerrainRecord.h"
#include "installationtree.h"
#include "SlicEngine.h"
#include "tiledmap.h"
#include "player.h"
#include "Vision.h"
#include "Globals.h"
#include "terrainutil.h"
#include "TerrainImprovementRecord.h"
#include "SoundRecord.h"
#include "soundmanager.h"
#include "SelItem.h"
#include "Network.h"                    // g_network

TerrainImprovementPool::TerrainImprovementPool() 
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
{
}

TerrainImprovementPool::TerrainImprovementPool(CivArchive &archive)
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
{
	Serialize(archive);
}

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
		return TerrainImprovement();
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

    if (g_network.IsActive() && g_network.IsHost()) 
    {
        g_network.Enqueue(newData); 
    }

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

//not implemented??? E 4-7-2006
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
