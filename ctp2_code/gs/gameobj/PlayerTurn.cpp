
#include "c3.h"
#include "player.h"
#include "GameEventManager.h"
#include "controlpanelwindow.h"
#include "PlayerEvent.h"
#include "Events.h"
#include "player.h"
#include "GameEventUser.h"
#include "Wormhole.h"
#include "PlayHap.h"
#include "UnitDynArr.h"
#include "Gold.h"
#include "Readiness.h"
#include "UnitPool.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "UnitData.h"
#include "Army.h"
#include "EndGame.h"
#include "TurnCnt.h"
#include "Score.h"


#include "profileDB.h"
#include "civapp.h"
#include "SelItem.h"
#include "controlpanelwindow.h"
#include "c3ui.h"
#include "sciencewin.h"
#include "network.h"
#include "net_info.h"
#include "Strengths.h"
#include "MessagePool.h"
#include "net_strengths.h"

#include "ctp2_Window.h"
#include "debugmemory.h"

extern ControlPanelWindow	*g_controlPanel;
extern sint32					g_tileImprovementMode;
extern TurnCount *g_turn;

void Player::BeginTurn()

{
#ifdef _DEBUG
	if(m_owner == 1) {
		
	}
#endif

	
	
	
	





	
	if (g_tileImprovementMode) 
	{
		g_tileImprovementMode = 0;
		
	}

	

	
	if(g_controlPanel) 
	{
		g_controlPanel->UpdatePlayerBeginProgress(m_owner);
		g_controlPanel->GetWindow()->DrawChildren();
	}

	sint32 i;
	for(i = m_messages->Num() - 1; i >= 0; i--) {
		if(!g_theMessagePool->IsValid(m_messages->Access(i))) {
			m_messages->DelIndex(i);
			continue;
		}

		
		



	}

	
	if (g_selected_item->GetVisiblePlayer() == m_owner)
		g_controlPanel->PopulateMessageList(m_owner);


	m_is_turn_over = FALSE;
	
	m_end_turn_soon = FALSE;

	if(g_network.IsHost()) {
		g_network.Block(m_owner);
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_SET_ROUND, m_owner, m_current_round));
		g_network.Unblock(m_owner);
	}

	if(!g_network.IsActive() || g_network.IsHost() || (m_owner == g_network.GetPlayerIndex())) {
		DPRINTF(k_DBG_GAMESTATE, ("Player[%d]::BeginTurn: running\n", m_owner));

		m_civRevoltingCitiesShouldJoin = -1;

		sint32 p;
		for(p = 0; p < k_MAX_PLAYERS; p++) {
			m_sent_requests_this_turn[p] = 0;
		}

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_WormholeTurn,
							   GEA_Player, m_owner,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_PlayerPatience,
							   GEA_Player, m_owner,
							   GEA_End);

		
		
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_PeaceMovement,
							   GEA_Player, m_owner,
							   GEA_End);

		
		
		m_gold->ClearStats(); 

		
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_PollutionTurn,
							   GEA_Player, m_owner,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnAllCities,
							   GEA_Player, m_owner,
							   GEA_End);

			
		sint32 n = m_all_cities->Num();
		////////////////////////
		for(i = 0; i < n; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail,
								   GEV_CityTurnPreProduction,
								   GEA_City, m_all_cities->Access(i),
								   GEA_End);
		}
			
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnProduction,
							   GEA_Player, m_owner,
							   GEA_End);
			
			
		for(i = 0; i < n; i++) {
			g_gevManager->AddEvent(GEV_INSERT_Tail,
								   GEV_CityBeginTurn,
								   GEA_City, m_all_cities->Access(i),
								   GEA_End);
		}
		//////////////////////// Have to be merged? Or seperated?
			
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnSupport,
							   GEA_Player, m_owner,
							   GEA_End);

		
		

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnImprovements,
							   GEA_Player, m_owner,
							   GEA_End);

		BeginTurnEnemyUnits();

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnAgreements,
							   GEA_Player, m_owner,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_ResetAllMovement,
							   GEA_Player, m_owner,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_AttemptRevolt,
							   GEA_Player, m_owner,
							   GEA_End);

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnEndGame,
							   GEA_Player, m_owner,
							   GEA_End);

		
		
		
		

		BeginTurnUnits();

		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurnGovernment,
							   GEA_Player, m_owner,
							   GEA_End);

		
		m_strengths->Calculate();
		if(g_network.IsHost()) {
			g_network.Block(m_owner);
			g_network.QueuePacketToAll(new NetStrengths(m_owner));
			g_network.QueuePacketToAll(new NetScores(m_owner));
			g_network.Unblock(m_owner);
		}
	} else {
		DPRINTF(k_DBG_GAMESTATE, ("Player[%d]::BeginTurn: not running\n", m_owner));
	}


	if(!g_network.IsClient()) {
		
		
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_FinishBeginTurn,
							   GEA_Player, m_owner,
							   GEA_End);
	}
}
