









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
//Added by Martin Gühmann
#include "SoundRecord.h"
#include "SoundManager.h"
#include "SelItem.h"

TerrainImprovementPool::TerrainImprovementPool() 
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
{
}

TerrainImprovementPool::TerrainImprovementPool(CivArchive &archive)
	: ObjPool(k_BIT_GAME_OBJ_TYPE_TERRAIN_IMPROVEMENT)
{
	Serialize(archive);
}

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

	//Added by Martin Gühmann to make a sound play when a tile improvement is build.
	sint32 soundID = g_theTerrainImprovementDB->Get(newData->GetType())->GetSound()->GetIndex();
	if (soundID != -1) {
		sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
		if ((visiblePlayer == owner)/* || 
			(m_visibility & (1 << visiblePlayer))*/) {

			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 	soundID, point.x, point.y);
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
