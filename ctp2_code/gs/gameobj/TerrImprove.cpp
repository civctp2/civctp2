#include "ctp/c3.h"
#include "gs/gameobj/TerrImprove.h"

#include "net/general/net_info.h"             // NetInfo
#include "net/general/network.h"              // g_network
#include "gs/gameobj/Player.h"               // g_player
#include "ui/aui_ctp2/SelItem.h"              // g_selected_item
#include "gs/gameobj/TerrImprovePool.h"      // g_theTerrainImprovementPool
#include "gs/world/World.h"                // g_theWorld

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
	return g_theTerrainImprovementPool->IsValid(*this);
}
