#include "c3.h"
#include "installation.h"
#include "installationpool.h"

#include "player.h"
#include "World.h"

#include "network.h"
#include "net_info.h"
#include "SelItem.h"
#include "director.h"
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
		// Diagnostic: every VisionRange in tileimp.txt tops out at 8, but a
		// playtest log caught Vision::DoFillCircleOp underflowing ~82 tiles
		// away from an installation killed here (TerrainImprovementData::
		// Complete -> KillInstallation -> RemoveAllReferences), which no
		// sane radius should reach. Confirm myVisionRange itself is the
		// culprit (vs. some earlier, unrelated ref-count imbalance this
		// FillCircle just happens to expose) before chasing this further.
		Assert(myVisionRange <= 20.0);
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
