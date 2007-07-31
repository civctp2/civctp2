








#include "c3.h"
#include "installation.h"
#include "installationpool.h"

#include "Player.h"
#include "XY_Coordinates.h"
#include "World.h"

#include "network.h"
#include "net_info.h"
#include "SelItem.h"
#include "Director.h"
#include "terrainutil.h"

extern Player **g_player;
extern World *g_theWorld;
extern SelectedItem *g_selected_item;
extern Director *g_director;

void
Installation::KillInstallation()
{
	Installation tmp(*this);
	tmp.RemoveAllReferences();
}

void
Installation::RemoveAllReferences()
{
	MapPoint pos;
	GetPos(pos);
	
	if(GetOwner() >= 0 && g_player[GetOwner()]) {
		g_player[GetOwner()]->RemoveInstallationReferences(*this);
	}
	g_theWorld->RemoveInstallation(*this, pos);
	if(GetOwner() >= 0 && g_player[GetOwner()]) {
		
		double myVisionRange = terrainutil_GetVisionRange(GetType(), RetPos());
		if(myVisionRange > 0) {
			g_player[GetOwner()]->RemoveUnitVision(pos, myVisionRange);
			if(GetOwner() == g_selected_item->GetVisiblePlayer()) {
				g_director->AddCopyVision();
			}
		}
	}

	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_INSTALLATION,
									  uint32(*this)));
	}
	g_theInstallationPool->Del(*this);
}

const InstallationData *Installation::GetData() const
{
	return g_theInstallationPool->GetInstallation(*this);
}

InstallationData *Installation::AccessData()
{
	return g_theInstallationPool->AccessInstallation(*this);
}

const TerrainImprovementRecord *Installation::GetDBRec() const
{
	return g_theTerrainImprovementDB->Get(GetData()->GetType());
}

void Installation::UseAirfield()
{
	AccessData()->UseAirfield();
}

sint32 Installation::AirfieldLastUsed() const
{
	return GetData()->AirfieldLastUsed();
}

void Installation::ChangeOwner(sint32 toOwner)
{
	AccessData()->ChangeOwner(toOwner);
}
