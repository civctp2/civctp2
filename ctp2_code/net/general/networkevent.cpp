
#include "c3.h"

#include "network.h"
#include "net_diplomacy.h"
#include "net_action.h"

#include "Events.h"
#include "GameEventUser.h"
#include "net_info.h"
#include "player.h"
#include "citydata.h"
#include "UnitData.h"

#include "diplomattypes.h"
#include "Diplomat.h"

#include "ctpai.h"

#include "newturncount.h"
#include "director.h"

#include "net_city.h"
#include "net_happy.h"

STDEHANDLER(NetBeginTurnEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	
	
	

	if(g_network.IsActive()) {
		if(pl == g_network.GetPlayerIndex())
			g_network.SetMyTurn(TRUE);
		else
			g_network.SetMyTurn(FALSE);

		Diplomat::GetDiplomat(g_network.GetPlayerIndex()).ClearInitiatives();
	}

	if(g_network.IsClient() && g_network.IsLocalPlayer(pl)) {
		g_network.SendAction(new NetAction(NET_ACTION_ACK_BEGIN_TURN));
	}
	
	return GEV_HD_Continue;
}

STDEHANDLER(NetTurnSyncEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;














	return GEV_HD_Continue;
}

STDEHANDLER(NetStartMovePhaseEvent) 
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	if(g_network.IsHost()) {
		
		
		PointerList<Packetizer> cityPackets;
		PointerList<Packetizer> buildQueuePackets;
		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			
			if(!g_player[p])
				continue;

			if(p == g_network.GetPlayerIndex())
				continue;

			if(g_player[p]->m_playerType != PLAYER_TYPE_NETWORK)
				continue;

			uint16 id = g_network.IndexToId(p);
			if(id != 0xffff) {
				
				sint32 i;
				for(i = 0; i < g_player[pl]->m_all_cities->Num(); i++) {			
					UnitData *ud = g_player[pl]->m_all_cities->Access(i).AccessData();
					CityData *cd = ud->GetCityData();
					
					cityPackets.AddTail(new NetCity(ud, FALSE));
					cityPackets.AddTail(new NetCity2(cd, FALSE));
					cityPackets.AddTail(new NetHappy(g_player[pl]->m_all_cities->Access(i), cd->GetHappy(), FALSE));
					
					buildQueuePackets.AddTail(new NetCityBuildQueue(cd));
					
					
					
				}

				g_network.ChunkList(id, &cityPackets);
				if(p != pl) {
					
					g_network.ChunkList(id, &buildQueuePackets);
				}
			}
		}

		g_network.Enqueue(new NetInfo(NET_INFO_CODE_CITIES_DONE, pl));
	}

	return GEV_HD_Continue;
}

STDEHANDLER(NetAIFinishBeginTurnEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	



	if(g_network.IsHost() && !g_network.IsLocalPlayer(pl)) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_FINISH_AI_TURN, pl));
	} else if(g_network.IsClient() && g_network.IsLocalPlayer(pl) && g_player[pl]->m_playerType == PLAYER_TYPE_ROBOT) {
		
	}

	
	
	

	return GEV_HD_Continue;
}

STDEHANDLER(NetNewProposalEvent)
{
	if(!g_network.IsActive()) return GEV_HD_Continue;

	sint32 sender, receiver;
	if(!args->GetPlayer(0, sender)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, receiver)) return GEV_HD_Continue;

	NewProposal prop = Diplomat::GetDiplomat(sender).GetMyLastNewProposal(receiver);
	if(prop == Diplomat::s_badNewProposal) return GEV_HD_Continue;

	if(g_network.IsHost()) {
		g_network.Block(prop.senderId);
		g_network.QueuePacketToAll(new NetDipProposal(prop));
		g_network.Unblock(prop.senderId);
	} else if(g_network.IsLocalPlayer(sender)) {
		g_network.SendToServer(new NetDipProposal(prop));
	}

	return GEV_HD_Continue;
}

STDEHANDLER(NetResponseEvent)
{
	return GEV_HD_Continue;
}

STDEHANDLER(NetEndAIClientTurnEvent)
{
	if(!g_network.IsClient()) return GEV_HD_Stop;

	sint32 p;
	if(!args->GetPlayer(0, p)) return GEV_HD_Continue;

	Assert(g_network.IsLocalPlayer(p));
	if(!g_network.IsLocalPlayer(p))
		return GEV_HD_Continue;

	if(g_player[p]->m_playerType == PLAYER_TYPE_ROBOT) {
		DPRINTF(k_DBG_NET, ("NetEndAIClientTurnEvent, %d\n", p));
		g_director->AddEndTurn();
		
	}
	return GEV_HD_Continue;
}

STDEHANDLER(NetCreatedWonderEvent)
{
	return GEV_HD_Continue; 
#if 0   // Unreachable: CtP1 code?
	if(!g_network.IsHost()) return GEV_HD_Continue;

	Unit city;
	sint32 type;

	if(!args->GetCity(0, city)) return GEV_HD_Continue;
	if(!args->GetInt(0, type)) return GEV_HD_Continue;

	g_network.Block(city.GetOwner());
	g_network.Enqueue(new NetInfo(NET_INFO_CODE_CREATED_WONDER, city.m_id, type));
	g_network.Unblock(city.GetOwner());

	return GEV_HD_Continue;
#endif
}

	
void networkevent_Initialize()
{
	g_gevManager->AddCallback(GEV_BeginTurn, GEV_PRI_Pre, &s_NetBeginTurnEvent);
	g_gevManager->AddCallback(GEV_NetworkTurnSync, GEV_PRI_Primary, &s_NetTurnSyncEvent);

	g_gevManager->AddCallback(GEV_NewProposal, GEV_PRI_Primary, &s_NetNewProposalEvent);
	g_gevManager->AddCallback(GEV_ResponseReady, GEV_PRI_Primary, &s_NetResponseEvent);

	g_gevManager->AddCallback(GEV_StartMovePhase, GEV_PRI_Pre, &s_NetStartMovePhaseEvent);

	g_gevManager->AddCallback(GEV_AIFinishBeginTurn, GEV_PRI_Post, &s_NetAIFinishBeginTurnEvent);

	g_gevManager->AddCallback(GEV_EndAIClientTurn, GEV_PRI_Primary, &s_NetEndAIClientTurnEvent);

	
}

void networkevent_Cleanup()
{
}
