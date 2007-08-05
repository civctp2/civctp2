//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The material pool
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "MaterialPool.h"
#include "civarchive.h"
#include "network.h"
#include "net_action.h"
#include "net_info.h"
#include "player.h"
#include "ConstRecord.h"

extern Player **g_player;

extern BOOL g_powerPointsMode;

void MaterialPool::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

#define MATERIALPOOL_MAGIC 0x5432FFFF
	if(archive.IsStoring()) {
		archive.PerformMagic(MATERIALPOOL_MAGIC) ;
		archive.StoreChunk((uint8 *)&m_level, ((uint8 *)&m_cap)+sizeof(m_cap));

	} else {
		archive.TestMagic(MATERIALPOOL_MAGIC) ;
		archive.LoadChunk((uint8 *)&m_level, ((uint8 *)&m_cap)+sizeof(m_cap));

	}
}

		
void MaterialPool::AddMaterials(sint32 amt)
{
	if(m_level > 0 && amt > 0 && (m_level + amt) < 0) {
		m_level = 0x7fffffff;
	} else {
		m_level += amt;
	}
}

void MaterialPool::CheatAddMaterials(sint32 amt)
{
	if((g_network.IsActive() && g_network.SetupMode()) || g_powerPointsMode) {
		sint32 pointCost = sint32(double(amt) * g_theConstDB->Get(0)->GetPowerPointsToMaterials());
		if(g_player[m_owner]->GetPoints() < pointCost)
			return;
		g_player[m_owner]->DeductPoints(pointCost);

		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_POWER_POINTS,
										  m_owner, g_player[m_owner]->GetPoints()));
		}
	}
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_CHEAT_ADD_MATERIALS,
										   amt));
	}
	AddMaterials(amt);
}

sint32 MaterialPool::CheatSubtractMaterials(sint32 amt)
{
	if((g_network.IsActive() && g_network.SetupMode()) | g_powerPointsMode) {
		sint32 pointCost = sint32(double(amt) * g_theConstDB->Get(0)->GetPowerPointsToMaterials());
		g_player[m_owner]->AddPoints(pointCost);

		if(g_network.IsHost()) {
			g_network.Enqueue(new NetInfo(NET_INFO_CODE_POWER_POINTS,
										  m_owner, g_player[m_owner]->GetPoints()));
		}
	}
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_CHEAT_SUB_MATERIALS,
										   amt));
	}
	SubtractMaterials(amt);
	return amt;
}
