








#include "c3.h"
#include "PlayerEvent.h"
#include "Events.h"
#include "Player.h"
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
#include "SlicSegment.h"
#include "UnitData.h"
#include "Army.h"
#include "EndGame.h"
#include "TurnCnt.h"
#include "Score.h"


#include "ProfileDB.h"
#include "civapp.h"
#include "SelItem.h"

#include "controlpanelwindow.h"
#include "c3ui.h"
#include "sciencewin.h"
#include "network.h"
#include "net_info.h"
#include "unitutil.h"
#include "TerrImprove.h"
#include "ArmyData.h"
#include "CtpAi.h"
#include "MainControlPanel.h"
#include "AICause.h"
#include "Soundmanager.h"
#include "gamesounds.h"
#include "GSLogs.h"
#include "TradeRouteData.h"
#include "Diplomat.h"
#include "net_action.h"
#include "GaiaController.h"

#include "ctp2_Window.h"

extern TurnCount *g_turn;
extern CivApp *g_civApp;
extern ControlPanelWindow	*g_controlPanel;
extern C3UI						*g_c3ui;


extern sint32 g_noai_stop_player;

STDEHANDLER(ContactMadeEvent)
{
	GameEventArgument *firstPlayerArg = args->GetArg(GEA_Player, 0);
	GameEventArgument *secondPlayerArg = args->GetArg(GEA_Player, 1);

	sint32 p1, p2;

	if(!firstPlayerArg || !(firstPlayerArg->GetPlayer(p1)))
		return GEV_HD_Continue;

	if(!secondPlayerArg || !(secondPlayerArg->GetPlayer(p2)))
		return GEV_HD_Continue;

	g_player[p1]->ContactMade(p2);
	return GEV_HD_Continue;
}

STDEHANDLER(WormholeEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	if(g_wormhole)
		g_wormhole->BeginTurn(player);
	return GEV_HD_Continue;
}

STDEHANDLER(PatienceEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	
	return GEV_HD_Continue;
}

STDEHANDLER(PeaceMovementEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	Player *p = g_player[player];

	g_player[player]->m_global_happiness->CalcPeaceMovement(g_player[player],
															*g_player[player]->m_all_armies,
															*g_player[player]->m_all_cities);

	
	
	if(p->m_assasinationTimer > 0) {
		p->m_assasinationTimer--;
		if(p->m_assasinationTimer <= 0)
			p->m_assasinationModifier = 0;
	}


	return GEV_HD_Continue;
}

STDEHANDLER(PollutionTurnEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	g_player[player]->BeginTurnPollution();
	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnAllCitiesEvent)
{
	sint32 player, i;
	SlicObject *so;
	SlicSegment *seg;

	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	Player *p = g_player[player];

    static UnitDynamicArray dead;
	dead.Clear();

	
	p->m_pop_science = 0;

    p->m_gold->SetSavings();

    
    p->m_readiness->BeginTurn(p->m_government_type); 

	if(p->m_capitol && g_theUnitPool->IsValid(p->m_capitol->m_id)) {
		MapPoint pos;
		p->m_capitol->GetPos(pos);
		g_theWorld->FindCityDistances(p->m_owner, pos);
	}

	g_player[player]->m_virtualGoldSpent = 0;

	if(g_player[player]->GetGaiaController()->CanStartCountdown()) {
		so = new SlicObject("GCReadyToActivateUs");
		seg = g_slicEngine->GetSegment("GCReadyToActivateUs");
		if(!seg->TestLastShown(player, 10000)) {
			so->AddRecipient(player);
			so->AddPlayer(player);
			g_slicEngine->Execute(so);
		
			so = new SlicObject("GCReadyToActivateThem");
			seg = g_slicEngine->GetSegment("GCReadyToActivateThem");
			for(i = 1; i < g_theProfileDB->GetMaxPlayers(); i++) {
				if(!seg->TestLastShown(i, 10000) && i != player) {
					so->AddRecipient(i);
					so->AddPlayer(player);
				}
			}
			g_slicEngine->Execute(so);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnProductionEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	
	
	g_player[player]->BeginTurnProduction();
	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnSupportEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	Player *p = g_player[player];

	
	p->BeginTurnWonders();

    
    p->BeginTurnScience();

	return GEV_HD_Continue;
}
	
STDEHANDLER(BeginTurnImprovementsEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	g_player[player]->BeginTurnImprovements();
	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnAgreementsEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	g_player[player]->BeginTurnAgreements();
	return GEV_HD_Continue;
}

STDEHANDLER(ResetAllMovementEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	g_player[player]->ResetAllMovement();

	g_player[player]->m_oversea_lost_unit_count = 0; 
	g_player[player]->m_home_lost_unit_count = 0; 

	return GEV_HD_Continue;
}

STDEHANDLER(AttemptRevoltEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	g_player[player]->AttemptRevolt();
	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnEndGameEvent)
{





	return GEV_HD_Continue;
}

STDEHANDLER(BeginTurnGovernmentEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	Player *p = g_player[player];
	Assert(p != NULL);
	if((p != NULL) && p->m_change_government_turn == p->GetCurRound()) {
		p->ActuallySetGovernment(p->m_set_government_type);
		p->m_changed_government_this_turn = TRUE;
	} else {
		p->m_changed_government_this_turn = FALSE;
	}

	
	g_slicEngine->RunPlayerTriggers(player);

	return GEV_HD_Continue;
}

STDEHANDLER(FinishBeginTurnEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player))
		return GEV_HD_Continue;

	Player *p = g_player[player];
	
	BOOL atPeace = TRUE;

	sint32 i;

	
	for(i = 1; i < k_MAX_PLAYERS; i++) {
		if(p->m_contactedPlayers & (1 << i) && g_player[i]) {
			if(p->m_diplomatic_state[i] == DIPLOMATIC_STATE_WAR) {
				atPeace = FALSE;
				break;
			}
		}
	}


	if(atPeace) {
		p->m_score->AddYearAtPeace();
	}

	if (!p->m_isDead)  {  
		
		
		
		
		
	}
	DPRINTF(k_DBG_GAMESTATE, ("It's player %d's turn - year %d.\n", p->m_owner, p->GetCurRound()));
	DPRINTF(k_DBG_GAMESTATE, ("Gold: %d\n", p->m_gold->GetLevel()));

	// JJB added the following to save in a PBEM game:
	// moved from newturncount.cpp where it was too early
	if((g_turn->IsHotSeat() || g_turn->IsEmail()) &&
	  g_player[g_selected_item->GetCurPlayer()]->GetPlayerType() !=
	  PLAYER_TYPE_ROBOT) {
		g_turn->SendNextPlayerMessage();
	}
	
	if (g_theProfileDB->IsAutoSave()) {
		
		if (p->m_playerType != PLAYER_TYPE_ROBOT) {
			g_civApp->AutoSave(p->m_owner);
		}

		
		if(g_controlPanel)
			g_controlPanel->GetWindow()->ShouldDraw(TRUE);
	}


	
	if ( p->m_owner == g_selected_item->GetVisiblePlayer() ) {



		g_c3ui->AddAction( new SW_UpdateAction );



	}

	if(g_network.IsHost()) {
		g_network.Block(p->m_owner);
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_GOLD,
											   p->m_owner, p->m_gold->GetLevel()));
		g_network.Unblock(p->m_owner);
	}

	if((p->m_playerType == PLAYER_TYPE_HUMAN ||
		p->m_playerType == PLAYER_TYPE_NETWORK && g_network.IsLocalPlayer(p->m_owner)) &&
	   p->m_owner == g_selected_item->GetVisiblePlayer() &&
	   g_theProfileDB->IsAutoSelectFirstUnit()) {
		if(g_selected_item->GetState() == SELECT_TYPE_NONE) {
			g_selected_item->NextUnmovedUnit(TRUE);
		} else if(g_selected_item->GetState() != SELECT_TYPE_LOCAL_ARMY) {
			g_selected_item->MaybeAutoEndTurn(TRUE);
		}
	}

	if(g_network.IsHost()) {
		
		
		
		g_network.SyncRand();
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_TURN_SYNC));
	}

	if(!g_network.IsClient()) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_FinishBuildPhase,
							   GEA_Player, player,
							   GEA_End);
	}

	return GEV_HD_Continue;
}

STDEHANDLER(CreateUnitEvent) 
{
	MapPoint pos;
	sint32 utype;
	sint32 cause;
	static Unit homeCity;
	homeCity.m_id = 0;
	sint32 pl;

	if(!args->GetPos(0, pos)) return GEV_HD_Continue;
	args->GetCity(0, homeCity); 
	if(!args->GetInt(0, utype)) return GEV_HD_Continue;
	if(!args->GetInt(1, cause)) return GEV_HD_Continue;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	Assert(g_player[pl]);
	if(!g_player[pl])
		return GEV_HD_Continue;

	Unit u = g_player[pl]->CreateUnit(utype, pos, homeCity, false, (CAUSE_NEW_ARMY)cause);
	if(u.m_id == 0) {
		return GEV_HD_Stop;
	}

	args->Add(new GameEventArgument(GEA_Unit, u));
	return GEV_HD_Continue;
}

STDEHANDLER(SettleEvent)
{
	Army a;
	if(!args->GetArmy(0, a)) return GEV_HD_Continue;
	
	if(g_player[a.GetOwner()]) {
		if(g_player[a.GetOwner()]->Settle(a)) {
			args->Add(new GameEventArgument(GEA_Int, 1));
		}
	}

	
	return GEV_HD_Continue;
}

STDEHANDLER(CreateCityEvent)
{
	MapPoint pos;
	sint32 pl;
	sint32 cause;
	sint32 unitType;

	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;
	if(!args->GetInt(0, cause)) return GEV_HD_Continue;
	if(!args->GetInt(1, unitType)) return GEV_HD_Continue;

	if(g_player[pl]) {
		sint32 cityType = unitutil_GetCityTypeFor(pos);
		Unit city = g_player[pl]->CreateCity(cityType, pos, (CAUSE_NEW_CITY)cause, NULL, unitType);
		if(city.IsValid()) {
			args->Add(new GameEventArgument(GEA_City, city));    

			
			
			
			
			
			
			if(cause == CAUSE_NEW_CITY_GOODY_HUT) {
				SlicObject *so = new SlicObject("80RuinBecomesCity") ;
				so->AddRecipient(pl) ;
				so->AddCity(city);
				g_slicEngine->Execute(so) ;
				DPRINTF(k_DBG_GAMESTATE, ("You get a city!\n"));
				
				
				
				
				

				if (g_soundManager) {
					sint32 visiblePlayer = g_selected_item->GetVisiblePlayer();
					if (visiblePlayer == pl) {
						g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0, 
												 gamesounds_GetGameSoundID(GAMESOUNDS_GOODY_CITY),
												 pos.x,
												 pos.y);
					}
				}
			}
		} else if(cause == CAUSE_NEW_CITY_GOODY_HUT) {
			SlicObject *so = new SlicObject("93BesetByNothing");
			so->AddRecipient(pl);
			g_slicEngine->Execute(so);
		}

	}
	return GEV_HD_Continue;
}

STDEHANDLER(CreateImprovementEvent)
{
	MapPoint pos;
	sint32 pl;
	sint32 imptype;
	sint32 i;
	SlicObject *so;
	SlicSegment *seg;

	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	if(!args->GetPos(0, pos)) return GEV_HD_Continue;
	if(!args->GetInt(0, imptype)) return GEV_HD_Continue;

	g_player[pl]->CreateImprovement(imptype, pos, 0);
	

	if(g_player[pl] && g_player[pl]->GetGaiaController()->HasMinTowersBuilt()) {
		so = new SlicObject("GCMinObelisksReachedUs");
		seg = g_slicEngine->GetSegment("GCMinObelisksReachedUs");	
		so->AddRecipient(pl);
		if(!seg->TestLastShown(pl, 10000)) {
			g_slicEngine->Execute(so);
		
			so = new SlicObject("GCMinObelisksReachedThem");
			seg = g_slicEngine->GetSegment("GCMinObelisksReachedThem");
			for(i = 1; i < g_theProfileDB->GetMaxPlayers(); i++) {
				if(!seg->TestLastShown(i, 10000) && i != pl) {
					so->AddRecipient(i);
					so->AddPlayer(pl);
				}
			}	
			g_slicEngine->Execute(so);
		}
	}

	return GEV_HD_Continue;
}

STDEHANDLER(GrantAdvanceEvent)
{
	sint32 advance;
	sint32 pl;
	sint32 cause;

	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	if(!args->GetInt(0, advance)) return GEV_HD_Continue;
	if(!args->GetInt(0, cause)) return GEV_HD_Continue;

	g_player[pl]->m_advances->GiveAdvance(advance, (CAUSE_SCI)cause, FALSE);
	return GEV_HD_Continue;
}

STDEHANDLER(SendGoodEvent)
{
	sint32 resIndex;
	Unit sourceCity, destCity;

	if(!args->GetInt(0, resIndex)) return GEV_HD_Continue;
	if(!args->GetCity(0, sourceCity)) return GEV_HD_Continue;
	if(!args->GetCity(1, destCity)) return GEV_HD_Continue;

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_REQUEST_TRADE_ROUTE,
										   resIndex, sourceCity.m_id, destCity.m_id));
	} else {
		g_player[sourceCity.GetOwner()]->CreateTradeRoute(sourceCity, ROUTE_TYPE_RESOURCE,
														  resIndex, destCity,
														  sourceCity.GetOwner(), 0);
	}
	return GEV_HD_Continue;
}

STDEHANDLER(TradeBidEvent)
{
	sint32 player;
	sint32 resIndex;
	Unit sourceCity, destCity;

	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;
	if(!args->GetInt(0, resIndex)) return GEV_HD_Continue;
	if(!args->GetCity(0, sourceCity)) return GEV_HD_Continue;
	if(!args->GetCity(1, destCity)) return GEV_HD_Continue;

	g_player[player]->CreateTradeBid(sourceCity, resIndex, destCity);
	return GEV_HD_Continue;
}

STDEHANDLER(CreatedArmyEvent)
{
	
	
	return GEV_HD_Continue;
}

STDEHANDLER(SubGoldEvent)
{
	sint32 p, amt;
	if(!args->GetPlayer(0, p)) return GEV_HD_Continue;
	if(!args->GetInt(0, amt)) return GEV_HD_Continue;

	g_player[p]->SubGold(amt);
	return GEV_HD_Continue;
}

STDEHANDLER(AddGoldEvent)
{
	sint32 pl;
	sint32 amt;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;
	if(!args->GetInt(0, amt)) return GEV_HD_Continue;

	g_player[pl]->AddGold(amt);
	return GEV_HD_Continue;
}

STDEHANDLER(EstablishEmbassyEvent)
{
	sint32 owner, otherguy;
	if(!args->GetPlayer(0, owner)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, otherguy)) return GEV_HD_Continue;

	g_player[owner]->EstablishEmbassy(otherguy);
	return GEV_HD_Continue;
}

STDEHANDLER(ThrowPartyEvent)
{
	sint32 owner, otherguy;
	if(!args->GetPlayer(0, owner)) return GEV_HD_Continue;
	if(!args->GetPlayer(1, otherguy)) return GEV_HD_Continue;

	Diplomat::GetDiplomat(otherguy).ThrowParty(owner);
	return GEV_HD_Continue;
}

STDEHANDLER(FinishBuildPhaseEvent)
{
	sint32 player;
	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;

	
		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_StartMovePhase,
							   GEA_Player, player,
							   GEA_End);
	

	if(g_network.IsActive()) {
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NetworkTurnSync,
							   GEA_Player, player,
							   GEA_End);
	}


	return GEV_HD_Continue;
}

STDEHANDLER(StartMovePhaseEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	if(g_player[pl]->m_playerType == PLAYER_TYPE_ROBOT &&
	   (!g_network.IsActive() || g_network.IsHost())) {
		
		
		
		
			
		
		
		
		
		
	}

	if(!g_network.IsClient()) {
		

		
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_AIFinishBeginTurn,
							   GEA_Player, pl,
							   GEA_End);
	}

	

#ifndef _BFR_
	gslog_LogPlayerStats(g_selected_item->GetCurPlayer());

	
	Diplomat::GetDiplomat(pl).LogDebugStatus(1);
#endif

	return GEV_HD_Continue;
}

STDEHANDLER(ProcessUnitOrdersEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	g_player[pl]->ProcessUnitOrders();
	return GEV_HD_Continue;
}

STDEHANDLER(AIFinishBeginTurnEvent)
{
	sint32 pl;
	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	CtpAi::FinishBeginTurn(pl);

	return GEV_HD_Continue;
}


STDEHANDLER(GiveMapEvent)
{
	sint32 from_player;
	sint32 to_player;
	if(!args->GetPlayer(0, from_player)) 
		return GEV_HD_Continue;	
	if(!args->GetPlayer(1, to_player)) 
		return GEV_HD_Continue;	

	Assert(g_player[from_player] != NULL);
	g_player[from_player]->GiveMap(to_player);

	return GEV_HD_Continue;	
}



STDEHANDLER(GiveCityEvent)
{
	Unit giftCity;
	PLAYER_INDEX player;

	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;
	if(!args->GetCity(0, giftCity)) return GEV_HD_Continue;

	Assert(g_player[player] != NULL);
	g_player[giftCity->GetOwner()]->GiveCity(player, giftCity);

	return GEV_HD_Continue;	
}

STDEHANDLER(EnterAgeEvent)
{
	PLAYER_INDEX player;
	sint32 age;

	if(!args->GetPlayer(0, player)) return GEV_HD_Continue;
	if(!args->GetInt(0, age)) return GEV_HD_Continue;

	Assert(g_player[player] != NULL);
	g_player[player]->EnterNewAge(age);
	return GEV_HD_Continue;
}

STDEHANDLER(EndTurnEvent)
{
	PLAYER_INDEX pl;

	if(!args->GetPlayer(0, pl)) return GEV_HD_Continue;

	Assert(pl == g_selected_item->GetCurPlayer());

	if(pl == g_selected_item->GetCurPlayer()) {
		NewTurnCount::StartNextPlayer(false);
	}

	return GEV_HD_Continue;
}

void playerevent_Initialize()
{
	g_gevManager->AddCallback(GEV_ContactMade, GEV_PRI_Primary, &s_ContactMadeEvent);

	
	g_gevManager->AddCallback(GEV_WormholeTurn, GEV_PRI_Primary, &s_WormholeEvent);
	g_gevManager->AddCallback(GEV_PlayerPatience, GEV_PRI_Primary, &s_PatienceEvent);
	g_gevManager->AddCallback(GEV_PeaceMovement, GEV_PRI_Primary, &s_PeaceMovementEvent);
	g_gevManager->AddCallback(GEV_PollutionTurn, GEV_PRI_Primary, &s_PollutionTurnEvent);
	g_gevManager->AddCallback(GEV_BeginTurnAllCities, GEV_PRI_Primary, &s_BeginTurnAllCitiesEvent);
	g_gevManager->AddCallback(GEV_BeginTurnProduction, GEV_PRI_Primary, &s_BeginTurnProductionEvent);
	g_gevManager->AddCallback(GEV_BeginTurnSupport, GEV_PRI_Primary, &s_BeginTurnSupportEvent);
	g_gevManager->AddCallback(GEV_BeginTurnImprovements, GEV_PRI_Primary, &s_BeginTurnImprovementsEvent);
	g_gevManager->AddCallback(GEV_BeginTurnAgreements, GEV_PRI_Primary, &s_BeginTurnAgreementsEvent);
	g_gevManager->AddCallback(GEV_ResetAllMovement, GEV_PRI_Primary, &s_ResetAllMovementEvent);
	g_gevManager->AddCallback(GEV_AttemptRevolt, GEV_PRI_Primary, &s_AttemptRevoltEvent);
	g_gevManager->AddCallback(GEV_BeginTurnEndGame, GEV_PRI_Primary, &s_BeginTurnEndGameEvent);
	g_gevManager->AddCallback(GEV_BeginTurnGovernment, GEV_PRI_Primary, &s_BeginTurnGovernmentEvent);
	g_gevManager->AddCallback(GEV_FinishBeginTurn, GEV_PRI_Primary, &s_FinishBeginTurnEvent);
	

	g_gevManager->AddCallback(GEV_CreateUnit, GEV_PRI_Primary, &s_CreateUnitEvent);

	g_gevManager->AddCallback(GEV_Settle, GEV_PRI_Primary, &s_SettleEvent);
	g_gevManager->AddCallback(GEV_CreateCity, GEV_PRI_Primary, &s_CreateCityEvent);

	g_gevManager->AddCallback(GEV_CreateImprovement, GEV_PRI_Primary, &s_CreateImprovementEvent);

	g_gevManager->AddCallback(GEV_GrantAdvance, GEV_PRI_Primary, &s_GrantAdvanceEvent);

	g_gevManager->AddCallback(GEV_SendGood, GEV_PRI_Primary, &s_SendGoodEvent);
	g_gevManager->AddCallback(GEV_TradeBid, GEV_PRI_Primary, &s_TradeBidEvent);

	g_gevManager->AddCallback(GEV_CreatedArmy, GEV_PRI_Primary, &s_CreatedArmyEvent);
	g_gevManager->AddCallback(GEV_SubGold, GEV_PRI_Primary, &s_SubGoldEvent);
	g_gevManager->AddCallback(GEV_AddGold, GEV_PRI_Primary, &s_AddGoldEvent);

	g_gevManager->AddCallback(GEV_EstablishEmbassy, GEV_PRI_Primary, &s_EstablishEmbassyEvent);
	g_gevManager->AddCallback(GEV_ThrowParty, GEV_PRI_Primary, &s_ThrowPartyEvent);

	g_gevManager->AddCallback(GEV_FinishBuildPhase, GEV_PRI_Primary, &s_FinishBuildPhaseEvent);
	g_gevManager->AddCallback(GEV_StartMovePhase, GEV_PRI_Primary, &s_StartMovePhaseEvent);

	g_gevManager->AddCallback(GEV_ProcessUnitOrders, GEV_PRI_Primary, &s_ProcessUnitOrdersEvent);
	g_gevManager->AddCallback(GEV_AIFinishBeginTurn, GEV_PRI_Primary, &s_AIFinishBeginTurnEvent);
	g_gevManager->AddCallback(GEV_GiveMap, GEV_PRI_Primary, &s_GiveMapEvent);
	g_gevManager->AddCallback(GEV_GiveCity, GEV_PRI_Primary, &s_GiveCityEvent);

	g_gevManager->AddCallback(GEV_EnterAge, GEV_PRI_Primary, &s_EnterAgeEvent);
	g_gevManager->AddCallback(GEV_EndTurn, GEV_PRI_Primary, &s_EndTurnEvent);
}

void playerevent_Cleanup()
{
}
