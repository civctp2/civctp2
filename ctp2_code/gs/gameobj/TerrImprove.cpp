








#include "c3.h"
#include "TerrImprove.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TerrImprovePool.h"
#include "dynarr.h"
#include "player.h"

#include "network.h"
#include "net_info.h"
#include "SelItem.h"

void TerrainImprovement::KillImprovement()
{
	TerrainImprovement tmp(*this);
	tmp.RemoveAllReferences();
}

void TerrainImprovement::RemoveAllReferences()
{
	g_player[GetOwner()]->RemoveImprovementReferences(*this);
	g_theWorld->RemoveImprovement(*this, GetLocation());
	if(g_network.IsHost()) {
		if(g_selected_item->GetCurPlayer() == GetOwner())
			g_network.Block(GetOwner());
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_IMPROVEMENT,
									  uint32(*this)));
		if(g_selected_item->GetCurPlayer() == GetOwner())
			g_network.Unblock(GetOwner());
	}

	g_theTerrainImprovementPool->Del(*this);
}

//const TerrainImprovementRecord *TerrainImprovement::GetDBRec() const //added by E to access imps 3-1-2006
//{
//	return g_theTerrainImprovementDB->Get(GetData()->GetType());
//}


const TerrainImprovementData *TerrainImprovement::GetData() const
{
	return g_theTerrainImprovementPool->GetTerrainImprovement(*this);
}

TerrainImprovementData *TerrainImprovement::AccessData() const
{
	return g_theTerrainImprovementPool->AccessTerrainImprovement(*this);
}

void TerrainImprovement::AddTurn()
{
	AccessData()->AddTurn(1);
}

bool TerrainImprovement::IsValid()
{
	return g_theTerrainImprovementPool->IsValid(*this) == TRUE;
}
