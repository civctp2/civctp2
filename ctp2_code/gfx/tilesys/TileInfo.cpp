//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : 
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Repaired memory leak.
//
//----------------------------------------------------------------------------
 
#include "c3.h"
#include "pixelutils.h"
#include "tileutils.h"
#include "TileInfo.h"
#include "GoodActor.h"
#include "CivArchive.h"
#include "Player.h"
#include "SelItem.h"
#include "TerrainRecord.h"
#include "terrainutil.h"

extern Player			**g_player;
extern SelectedItem		*g_selected_item;

#include "profiledb.h"
extern ProfileDB		*g_theProfileDB;

extern sint32			g_fog_toggle;
extern sint32			g_god;

#define k_OCEAN_BEFORE_OCEANOGRAPHY_BASE_TILE			10

TileInfo::TileInfo()
{
	m_goodActor = NULL;
	m_riverPiece = -1;
	m_tileNum = 0;
	m_terrainType = 0;
	m_transitions[0] = 0;
	m_transitions[1] = 0;
	m_transitions[2] = 0;
	m_transitions[3] = 0;
	m_megaInfo = 0;
	m_transform = rand() % 256;
}

TileInfo::TileInfo(TileInfo *copy)
{
	*this = *copy;

	if (copy->m_goodActor) 
    {
		m_goodActor = new GoodActor(copy->m_goodActor);
	}
    // else: already set in *this = *copy;
}

TileInfo::TileInfo(CivArchive &archive)
:   m_goodActor (NULL)
{
	Serialize(archive);
}

TileInfo::~TileInfo()
{
	delete m_goodActor;
}

TILEINDEX TileInfo::GetTileNum(void)
{
	return m_tileNum;

	if (g_fog_toggle)
		return m_tileNum;
	if ( g_god ) {
		return m_tileNum;
	}

	
	if (!g_player[g_selected_item->GetVisiblePlayer()]) return m_tileNum;

	if(g_player[g_selected_item->GetVisiblePlayer()]->m_hasGlobalRadar) {
		return m_tileNum;
	} else if (g_player[g_selected_item->GetVisiblePlayer()] &&
		g_player[g_selected_item->GetVisiblePlayer()]->GetDeepOceanVisible()) {
		return m_tileNum;
	} else {
		if(g_theTerrainDB->Get(m_terrainType)->GetMovementTypeSea() &&
			m_terrainType != TERRAIN_WATER_BEACH) {
			return k_OCEAN_BEFORE_OCEANOGRAPHY_BASE_TILE;
		} else {
			return m_tileNum;
		}
	}
}

sint32 TileInfo::SetGoodActor(sint32 index, MapPoint pos)
{
    delete m_goodActor;
	m_goodActor = new GoodActor(index, pos);
	
	if (g_theProfileDB->IsGoodAnim()) 
	{
		m_goodActor->FullLoad();
	}

	return TRUE;
}

void TileInfo::DeleteGoodActor(void)
{
	delete m_goodActor;
	m_goodActor = NULL;
}

void TileInfo::Serialize(CivArchive &archive)
{
	sint32 hasGoodActor;

	if(archive.IsStoring()) {
		DPRINTF(k_DBG_GRAPHICS, ("%d,%d,%d,%d, %d,%d,%d,%d\n",
								 m_riverPiece, m_megaInfo, 
								 m_terrainType, m_tileNum,
								 m_transitions[0], m_transitions[1],
								 m_transitions[2], m_transitions[3]));
		archive << m_riverPiece;
		archive << m_megaInfo;
		archive << m_terrainType;
		archive << m_tileNum;
		archive << m_transform;
		for(sint32 i = 0; i < k_NUM_TRANSITIONS; i++) {
			archive<< m_transitions[i];
		}
		hasGoodActor = (m_goodActor != NULL);
		archive << hasGoodActor;
		if(m_goodActor)
			m_goodActor->Serialize(archive);
	} else {
		archive >> m_riverPiece;
		archive >> m_megaInfo;
		archive >> m_terrainType;
		archive >> m_tileNum;
		archive >> m_transform;
		for(sint32 i = 0; i < k_NUM_TRANSITIONS; i++) {
			archive >> m_transitions[i];
		}
		archive >> hasGoodActor;

        delete m_goodActor;
		if(hasGoodActor)
			m_goodActor = new GoodActor(archive);
		else
			m_goodActor = NULL;
	}
}
