










#include "c3.h"
#include "CivArchive.h"
#include "Player.h"
#include "CivilisationDB.h"
#include "CivilisationPool.h"
#include "network.h"
#include "SelItem.h"

#include "AiCause.h"
#include "profileDB.h"

#include "net_player.h"
#include "AdvanceRecord.h"
#include "xy_coordinates.h"
#include "World.h"
#include "net_vision.h"
#include "ctpai.h"

extern ProfileDB* g_theProfileDB; 
	extern	Player	**g_player ;

	extern	CivilisationPool	*g_theCivilisationtPool ;
	extern SelectedItem *g_selected_item;

extern World *g_theWorld;








void Civilisation::KillCivilisation()
	{
	Civilisation	tmp(*this) ;
	tmp.RemoveAllReferences() ;
	}









void Civilisation::RemoveAllReferences()
	{
	g_theCivilisationPool->Del(*this) ;
	}









const CivilisationData* Civilisation::GetData() const
	{
	return (g_theCivilisationPool->GetData(*this)) ;
	}









CivilisationData* Civilisation::AccessData() const
	{
	return (g_theCivilisationPool->AccessData(*this)) ;
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

	if(pi == PLAYER_INDEX_INVALID) {
		pi = PLAYER_INDEX_VANDALS;
	}
	return pi;
}

void civilisation_CreateNewPlayer(sint32 pi, sint32 old_owner)
{
	g_player[pi] = NULL ;									
	if(g_network.IsActive()) {
		g_network.AddCivilization(pi, PLAYER_TYPE_ROBOT, CIV_INDEX_RANDOM);
	}
			
	g_player[pi] = new Player(PLAYER_INDEX(pi), 0, PLAYER_TYPE_ROBOT, CIV_INDEX_RANDOM, GENDER_RANDOM) ;
			
	g_selected_item->AddPlayer(pi) ;
			
			
	if (g_theProfileDB->IsAIOn()) {
	}
			
	Assert(g_player[pi]) ;

	if(pi != PLAYER_INDEX_VANDALS && g_player[pi] && 
	   (old_owner >= 0) && g_player[old_owner]) {
		g_player[pi]->m_advances->Copy(g_player[old_owner]->m_advances);
		g_player[pi]->m_advances->SetOwner(pi);

		
		g_player[old_owner]->GiveMap(pi);

	}

	CtpAi::AddPlayer(pi);

	if(g_network.IsHost()) {
		g_network.Block(old_owner);
		g_network.QueuePacketToAll(new NetPlayer(g_player[pi]));
		
		uint16 y;
		for(y = 0; y < g_theWorld->GetYHeight(); y += k_VISION_STEP) {
			g_network.QueuePacketToAll(new NetVision(pi, y, k_VISION_STEP));
		}
		g_network.Unblock(old_owner);
	}
}

const CivilisationRecord *Civilisation::GetDBRec() const
{
	return g_theCivilisationDB->Get(GetCivilisation());
}
