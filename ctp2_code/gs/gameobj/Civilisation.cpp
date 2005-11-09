//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Civilisation handling.
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Recycle civilisation indices to prevent a game crash.
// - Corrected civilisation index for MP new player creation.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "Civilisation.h"

#include "civarchive.h"
#include "player.h"			    // g_player
#include "CivilisationRecord.h"
#include "CivilisationPool.h"	// g_theCivilisationPool
#include "network.h"
#include "SelItem.h"			// g_selected_item
#include "AICause.h"
#include "profileDB.h"			// g_theProfileDB
#include "net_player.h"
#include "AdvanceRecord.h"
#include "World.h"			    // g_theWorld
#include "net_vision.h"
#include "ctpai.h"



void Civilisation::KillCivilisation()
{
	Civilisation	tmp(*this);
	tmp.RemoveAllReferences();
}









void Civilisation::RemoveAllReferences()
{
	g_theCivilisationPool->Release(GetCivilisation());
	g_theCivilisationPool->Del(*this);
}









const CivilisationData* Civilisation::GetData() const
{
	return (g_theCivilisationPool->GetData(*this));
}









CivilisationData* Civilisation::AccessData() const
{
	return (g_theCivilisationPool->AccessData(*this));
}

PLAYER_INDEX civilisation_NewCivilisationOrVandals(PLAYER_INDEX old_owner)
{
	PLAYER_INDEX pi = PLAYER_INDEX_INVALID;
	sint32 i;
	sint32 maxPlayers = g_theProfileDB->GetMaxPlayers();
	if(maxPlayers <= 0) {
		maxPlayers = g_theProfileDB->GetNPlayers();
	}

	sint32 count = 0;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i])
			count++;
	}
	if(count < maxPlayers) {
		for(i = 1; i < k_MAX_PLAYERS; i++) {
			if(!g_player[i]) {
				pi = i;
				break;
			}
		}
		if(pi != PLAYER_INDEX_INVALID) {
			civilisation_CreateNewPlayer(pi, old_owner);
		}
	}

	if (pi == PLAYER_INDEX_INVALID) 
    {
        // Maximum number of players reached: add as Barbarians.
		pi = PLAYER_INDEX_VANDALS;
	}

	return pi;
}

//----------------------------------------------------------------------------
//
// Name       : civilisation_CreateNewPlayer
//
// Description: Create a new (AI) player
//
// Parameters : pi          : player index
//              old_owner   : player index of "parent" civilisation 
//
// Globals    : g_player    : player data
//              g_network   : Multiplayer data
//
// Returns    : -
//
// Remark(s)  : When a parent civilisation is provided, the new civilisation
//              inherits the advances and map of its parent.
//              Assumption  : the new player index is valid and "free".
//
//----------------------------------------------------------------------------
void civilisation_CreateNewPlayer(sint32 pi, sint32 old_owner)
{
    g_player[pi] = new Player
        (PLAYER_INDEX(pi), 0, PLAYER_TYPE_ROBOT, CIV_INDEX_RANDOM, GENDER_RANDOM);

    if (g_network.IsActive()) 
    {
        g_network.AddCivilization
            (pi, PLAYER_TYPE_ROBOT, g_player[pi]->GetCivilisation()->GetCivilisation());
    }
	
    g_selected_item->AddPlayer(pi);
	
    if (pi != PLAYER_INDEX_VANDALS && 			// Barbarians do not inherit
	    (old_owner >= 0) && g_player[old_owner]
       ) 
    {
		g_player[pi]->m_advances->Copy(g_player[old_owner]->m_advances);
		g_player[pi]->m_advances->SetOwner(pi);
		g_player[old_owner]->GiveMap(pi);
    }

    CtpAi::AddPlayer(pi);

    if (g_network.IsHost()) 
    {
		g_network.Block(old_owner);
		g_network.QueuePacketToAll(new NetPlayer(g_player[pi]));
		
		for (uint16 y = 0; y < g_theWorld->GetYHeight(); y += k_VISION_STEP)
		{
			g_network.QueuePacketToAll(new NetVision(pi, y, k_VISION_STEP));
		}

		g_network.Unblock(old_owner);
    }
}

const CivilisationRecord *Civilisation::GetDBRec() const
{
	return g_theCivilisationDB->Get(GetCivilisation());
}
