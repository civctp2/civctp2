//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : turncounter handles the clockwork of turn progression
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
// _DEBUG
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Propagate PW each turn update
// - Altered filename generating for PBEM saves (JJB 2004/12/30)
// - Moved needs refueling check to Unit.cpp to remove code duplication.
//   - April 24th 2005 Martin Gühmann
// - Replaced old difficulty database by new one. (April 29th 2006 Martin Gühmann)
// - Replaced old const database by new one. (5-Aug-2007 Martin Gühmann)
// - Put SendNextPlayerMessage into its own event. (14-Nov-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "TurnCnt.h"

#include "AgreementPool.h"          // g_theAgreementPool
#include "AICause.h"
#include "Barbarians.h"
#include "BuildingRecord.h"
#include "buildingutil.h"
#include "Cell.h"
#include "citydata.h"
#include "civarchive.h"
#include "CivPaths.h"               // g_civPaths
#include "colorset.h"
#include "ConstRecord.h"            // g_theConstDB
#include "DB.h"
#include "debugmemory.h"
#include "Diffcly.h"
#include "DifficultyRecord.h"
#include "Diplomacy_Log.h"
#include "DiplomaticRequestData.h"
#include "DiplomaticRequestPool.h"  // g_theDiplomaticRequestPool
#include "director.h"               // g_director
#include "dynarr.h"
#include "GameEventManager.h"
#include "gamefile.h"
#include "GameOver.h"
#include "GameSettings.h"
#include "gamesounds.h"
#include "GovernmentRecord.h"
#include "MaterialPool.h"
#include "messagemodal.h"
#include "messagewin.h"
#include "net_action.h"
#include "net_info.h"
#include "net_rand.h"
#include "net_ready.h"
#include "network.h"
#include "player.h"                 // g_player
#include "pollution.h"
#include "profileDB.h"              // g_theProfileDB
#include "radarmap.h"               // g_radarMap
#include "Readiness.h"
#include "Score.h"
#include "SelItem.h"                // g_selected_item
#include "SimpleDynArr.h"
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "soundmanager.h"           // g_soundManager
#include "StrDB.h"                  // g_theStringDB
#include "tiledmap.h"               // g_tiledMap
#include "UnitData.h"
#include "UnitDynArr.h"
#include "UnitRecord.h"
#include "World.h"                  // g_theWorld

extern Diplomacy_Log *  g_theDiplomacyLog;
extern MessageModal *   g_modalMessage;
extern Network          g_network;
extern Pollution *      g_thePollution;

sint32 g_cantEndTurn = 0;

sint32 TurnCount::sm_the_stop_player = 1;

TurnCount::TurnCount()
{
	m_sliceList = new SimpleDynamicArray<sint32>;
	Init();
}

TurnCount::TurnCount(CivArchive &archive)
{
	m_sliceList = new SimpleDynamicArray<sint32>;
	Serialize(archive);
}

TurnCount::~TurnCount()
{
	delete m_sliceList;
	m_sliceList = NULL;
}

void TurnCount::Init()

{
	m_turn = 0;
	m_round = 0;
	m_simultaneousMode = FALSE;
	m_activePlayers = g_theProfileDB->GetNPlayers();
	m_year = diffutil_GetYearFromTurn(g_theGameSettings->GetDifficulty(), m_round);
	m_lastBeginTurn = -1;
	m_isHotSeat = FALSE;
	m_isEmail = FALSE;
	ChooseHappinessPlayer();
	m_sentGameAlmostOverMessage = FALSE;
	m_sentGameOverMessage = FALSE;
}

void TurnCount::Init(CivArchive &archive)

{
	archive >> m_turn;
	archive >> m_round;
}

void TurnCount::SkipToRound(sint32 round)
{
	m_round = round;
	m_year = diffutil_GetYearFromTurn(g_theGameSettings->GetDifficulty(), m_round);
}

void TurnCount::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive << m_turn;
		archive << m_round;
		archive << m_year;
		archive << static_cast<sint32>(m_simultaneousMode);
		archive << m_activePlayers;
		archive << m_lastBeginTurn;
		archive.PutUINT8(static_cast<uint8>(m_isEmail));
		archive.PutUINT8(static_cast<uint8>(m_isHotSeat));
		archive << m_happinessPlayer;

		uint8 tmp = NewTurnCount::m_sentGameOverMessage;
		archive << tmp;

		tmp = NewTurnCount::m_sentGameAlmostOverMessage;
		archive << tmp;
	} else {
		archive >> m_turn;  // Unused always -4000 or what you have in const.txt
		archive >> m_round; // Unused always 0
		archive >> m_year;  // Unused
        sint32  sim;
		archive >> sim;
		m_simultaneousMode = sim;
		archive >> m_activePlayers; // Seems to be number of players at the start
		archive >> m_lastBeginTurn; // Unused always -1
		m_isEmail = (BOOL)archive.GetUINT8();
		m_isHotSeat = (BOOL)archive.GetUINT8();
		archive >> m_happinessPlayer; // Unused always 0

		uint8 tmp;
		archive >> tmp;
		NewTurnCount::m_sentGameOverMessage = (tmp != 0);
		m_sentGameOverMessage = (tmp != 0);

		archive >> tmp;
		NewTurnCount::m_sentGameAlmostOverMessage = (tmp != 0);
		m_sentGameAlmostOverMessage = (tmp != 0);
	}
	m_sliceList->Serialize(archive);
}

void TurnCount::InformNetwork()
{
	if(g_network.IsHost()) {
		if(m_lastBeginTurn == g_selected_item->GetCurPlayer())
			return;

		m_lastBeginTurn = g_selected_item->GetCurPlayer();

		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->
			                                          GetCurPlayer()),
			                                          new NetRand());
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->
			                                          GetCurPlayer()),
			                                          new NetInfo(NET_INFO_CODE_GOLD,
			                                          g_selected_item->GetCurPlayer(),
			                                          g_player[g_selected_item->GetCurPlayer()]->m_gold->GetLevel()));
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->
			                                          GetCurPlayer()),
			                                          new NetReadiness(g_player[g_selected_item->GetCurPlayer()]->m_readiness));
			// propagate PW each turn update
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->
			                                          GetCurPlayer()),
			                                          new NetInfo(NET_INFO_CODE_MATERIALS,
			                                          g_selected_item->GetCurPlayer(),
			                                          g_player[g_selected_item->GetCurPlayer()]->m_materialPool->GetMaterials()));
		}
		g_network.BeginTurn(g_selected_item->GetCurPlayer());
		NetInfo* netInfo = new NetInfo(NET_INFO_CODE_BEGIN_TURN,
		                               g_selected_item->GetCurPlayer());
		g_network.QueuePacketToAll(netInfo);
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			g_network.SetMyTurn(FALSE);
		}
		else
		{
			g_network.SetMyTurn(TRUE);
		}
	}
}

void TurnCount::InformMessages()
{
	messagewin_BeginTurn( g_selected_item->GetVisiblePlayer() );
}

void TurnCount::SliceInformNetwork()
{
	if(g_network.IsHost()) {
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->
													  GetCurPlayer()),
								  new NetRand());
		}
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_BEGIN_SLICE,
											   g_selected_item->GetCurPlayer()));
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			g_network.SetMyTurn(FALSE);
		}
		else
		{
			g_network.SetMyTurn(TRUE);
		}
	}
}

void TurnCount::ChooseNextActivePlayer()
{
	if(m_activePlayers <= 0)
		return;

	sint32 count = 0;

	do {
		g_selected_item->NextPlayer();
		g_director->NextPlayer();
		count++;
	} while(g_player[g_selected_item->GetCurPlayer()] == NULL ||
			(g_player[g_selected_item->GetCurPlayer()]->IsTurnOver() &&
			g_player[g_selected_item->GetCurPlayer()]->GetCurRound() == m_round &&
			count <= k_MAX_PLAYERS));

	Assert(count <= k_MAX_PLAYERS);

}

void TurnCount::EndThisTurn()
{
	PLAYER_INDEX curPlayer = g_selected_item->GetCurPlayer();

#ifdef _DEBUG
	if (g_theDiplomacyLog) g_theDiplomacyLog->EndTurn();
#endif

	if(!g_player[curPlayer]->IsTurnOver()) {
		g_player[curPlayer]->EndTurn();
		m_activePlayers--;
	}

	if(g_network.IsActive()) {
		g_network.DoResetCityOwnerHack();
	}

	Assert(!g_network.IsClient());
	extern BOOL g_aPlayerIsDead;
	if(!g_network.IsClient() && g_aPlayerIsDead) {
		Player::RemoveDeadPlayers();
	}

	while((m_activePlayers > 0) &&
		  (g_player[g_selected_item->GetCurPlayer()]->IsTurnOver() &&
		   g_player[g_selected_item->GetCurPlayer()]->GetCurRound() == m_round)) {

		ChooseNextActivePlayer();

		curPlayer = g_selected_item->GetCurPlayer();

		if(m_simultaneousMode &&
		   g_player[curPlayer]->m_end_turn_soon) {
			if(g_player[curPlayer]->GetCurRound() != m_round) {
				g_player[curPlayer]->BeginTurn();
			}
			g_player[curPlayer]->EndTurn();
			m_activePlayers--;
		}
	}
}

void TurnCount::BeginNewRound()
{
	Assert(m_activePlayers == 0);
	m_activePlayers = 0;
	sint32 i;

	m_round++;

#ifdef _DEBUG
    if (g_theDiplomacyLog) {
        g_theDiplomacyLog->BeginRound();
    }
#endif // _DEBUG

	Barbarians::BeginYear();

	ChooseHappinessPlayer();

	m_year += diffutil_GetYearIncrementFromTurn(g_theGameSettings->GetDifficulty(), m_round);

	RunNewYearMessages() ;
	if(g_network.IsHost()) {
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_YEAR, m_round, m_year));
	}
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i])
			m_activePlayers++;
	}
	g_selected_item->NextRound();
	g_director->NextPlayer();
	g_theAgreementPool->EndRound();
	g_thePollution->EndRound();
	g_slicEngine->RunYearlyTriggers();

	if(m_simultaneousMode && g_network.IsHost()) {
		for(i = 0; i < k_MAX_PLAYERS; i++) {
			if(g_player[i]) {
				g_selected_item->SetCurPlayer(i);
				BeginNewTurn(FALSE);
			}
		}
	}
	g_selected_item->SetCurPlayer(0);
	BeginNewTurn(FALSE);





}

void TurnCount::BeginNewTurn(BOOL clientVerification)
{
#ifdef _DEBUG
	sint32 age;
	sint32 player_idx;

	if ((0 == m_round) && (g_theProfileDB->GetCheatAge(age))) {
		switch(age) {
		case 1:
			m_round = 124;
			for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
				 if (g_player[player_idx]) {
					for(sint32 i = 0; i < 20; i++) {
						g_player[player_idx]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
					}
					g_player[player_idx]->m_advances->GiveAdvance(30, CAUSE_SCI_UNKNOWN);

				}
			}
			break;
		case 2:
			m_round = 249;
			for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
				 if (g_player[player_idx]) {
					for(sint32 i = 0; i < 40; i++) {
						g_player[player_idx]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
					}
				}
			}
			break;
		case 3:
			m_round = 374;
			for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
				 if (g_player[player_idx]) {
					for(sint32 i = 0; i < 60; i++) {
						g_player[player_idx]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
					}
					g_player[player_idx]->m_advances->GiveAdvance(60, CAUSE_SCI_UNKNOWN);
					g_player[player_idx]->m_advances->GiveAdvance(64, CAUSE_SCI_UNKNOWN);
				}
			}

			break;
		case 4:
			m_round = 449;
			for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
				 if (g_player[player_idx]) {
					for(sint32 i = 0; i < 80; i++) {
						g_player[player_idx]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
					}
				}
			}
			break;
		case 5:
			m_round = 524;
			for (player_idx=0; player_idx<k_MAX_PLAYERS; player_idx++) {
				 if (g_player[player_idx]) {
					for(sint32 i = 0; i < 100; i++) {
						g_player[player_idx]->m_advances->GiveAdvance(i, CAUSE_SCI_UNKNOWN);
					}
				}
			}
			break;
		}

	}
#endif // _DEBUG

	if(g_network.IsHost()) {





		if(!clientVerification) {
			InformNetwork();
		}
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			if(!clientVerification)
				return;
			DPRINTF(k_DBG_NET, ("Client %d acknowledes begin turn %d\n", g_selected_item->GetCurPlayer(),
								m_round));
		}
	}

	if(g_player[g_selected_item->GetCurPlayer()]->GetCurRound() != m_round) {
		g_gevManager->AddEvent(GEV_INSERT_Tail,
							   GEV_BeginTurn,
							   GEA_Player, g_selected_item->GetCurPlayer(),
							   GEA_Int, m_round,
							   GEA_End);
	} else {
		SliceInformNetwork();
	}




#ifdef _DEBUG

	if (g_theProfileDB->LogPlayerStats()) {
		LogPlayerStats();
	}
#endif
}

void TurnCount::EndThisTurnBeginNewTurn(BOOL clientRequest)
{
	if (clientRequest)
    {
		if (g_network.CurrentPlayerAckedBeginTurn())
        {
		    EndThisTurnBeginNewTurn(FALSE);
		}
	}
    else
    {
	    EndThisTurn();

	    if (m_activePlayers <= 0)
        {
		    BeginNewRound();
	    }
        else
        {
		    BeginNewTurn(FALSE);
	    }

	    if (!g_network.IsActive() ||
            g_selected_item->GetCurPlayer() == g_network.GetPlayerIndex()
           )
        {
		    g_selected_item->Refresh();
	    }
    }
}

void TurnCount::EndThisSlice()
{
	sint32 nextPlayer;
	if(!m_simultaneousMode) {
		ChooseNextActivePlayer();
	} else {
		if(m_sliceList->Num() > 0) {
			nextPlayer = m_sliceList->Access(0);
			m_sliceList->DelIndex(0);
			g_selected_item->SetCurPlayer(nextPlayer);
			g_director->NextPlayer();
		} else {
			ChooseNextActivePlayer();
		}
	}
}

BOOL TurnCount::BeginNewSlice()
{
	PLAYER_INDEX curPlayer = g_selected_item->GetCurPlayer();

	if(g_player[curPlayer]->GetCurRound() != m_round) {
		BeginNewTurn(FALSE);
	} else if(g_network.IsHost() &&
	          g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) {





		if (g_soundManager)
			g_soundManager->AddSound(SOUNDTYPE_SFX, (uint32)0,
										gamesounds_GetGameSoundID(GAMESOUNDS_NET_YOUR_TURN),
										0,
										0);
	}

	if(g_player[curPlayer]->m_end_turn_soon) {
		return FALSE;
	}

	if(m_simultaneousMode) {
		if((g_network.IsHost() && !g_player[curPlayer]->IsNetwork()) ||
		   g_network.GetPlayerIndex() == curPlayer) {
			g_player[curPlayer]->ProcessUnitOrders(TRUE);
		}
	}

	if(g_network.IsHost()) {
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			g_network.QueuePacket(g_network.IndexToId(g_selected_item->
													  GetCurPlayer()),
								  new NetRand());
		}
		g_network.QueuePacketToAll(new NetInfo(NET_INFO_CODE_BEGIN_SLICE,
											   g_selected_item->GetCurPlayer()));
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			g_network.SetMyTurn(FALSE);
		}
		else
		{
			g_network.SetMyTurn(TRUE);
		}
		if(m_sliceList->Num() > 0)
		{
			g_network.QueuePacket(g_network.IndexToId(
				g_selected_item->GetCurPlayer()),
								  new NetInfo(NET_INFO_CODE_REQUEST_SLICE));
		}

	}


	if(g_network.IsActive()) {
		g_network.UnitsMoved(-g_network.GetUnitMovesUsed());
	}
	return TRUE;
}

void TurnCount::EndThisSliceBeginNewSlice()
{
	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_END_SLICE));
		g_network.SetMyTurn(FALSE);
		return;
	}

	EndThisSlice();
	if(!BeginNewSlice()) {
		EndThisTurnBeginNewTurn();
	}
}

void TurnCount::SetSliceTo(sint32 player)
{
	if(!g_player[player] ||
	   (g_player[player]->IsTurnOver() &&
		g_player[player]->GetCurRound() == m_round)) {
		return;
	}

	g_selected_item->SetCurPlayer(player);
	g_director->NextPlayer();
	BeginNewSlice();
}

void TurnCount::QueueSliceFor(sint32 player)
{
	m_sliceList->Insert(player);
	if(g_network.IsHost()) {
		g_network.QueuePacket(g_network.IndexToId(
			g_selected_item->GetCurPlayer()),
			new NetInfo(NET_INFO_CODE_REQUEST_SLICE));
	}
}

BOOL TurnCount::SimultaneousMode() const
{
	return m_simultaneousMode;
}

void TurnCount::SetSimultaneousMode(BOOL on)
{
	m_simultaneousMode = on;
}


BOOL TurnCount::VerifyEndTurn(BOOL force)
{
	Player *player = g_player[g_selected_item->GetCurPlayer()];

	if (!player->IsHuman())
	{
		return(TRUE);
	}

	if (g_modalMessage && !force)

		return FALSE;

	if (g_slicEngine->GetSegment("16IAOutOfFuel")->TestLastShown(player->m_owner, 1)) {
		int i;
		int n = player->GetAllUnitList()->Num();
		for (i=0; i<n; i++) {
			Unit *unit = &(player->GetAllUnitList()->Access(i));
			if (!(unit->GetMovementTypeAir()) && !(unit->GetMovementTypeSpace()))
				continue;
			if (!(unit->GetDBRec()->GetNoFuelThenCrash()))
				continue;
			if(unit->AccessData()->CheckForRefuel())
				continue;
			if (unit->NeedsRefueling()) {
				SlicObject *so = new SlicObject("16IAOutOfFuel");
				so->AddRecipient(player->m_owner);
				so->AddCivilisation(player->m_owner);
				g_slicEngine->Execute(so);
				return(FALSE);
			}
		}
	}

	if (g_slicEngine->GetSegment("23IACityWillStarve")->TestLastShown(player->m_owner, 1)) {
		int i;
		int n = player->GetAllCitiesList()->Num();
		for (i=0; i<n; i++) {
			double tmp;
			Unit *unit = &(player->GetAllCitiesList()->Access(i));
			if (!(unit->IsCity()))
				continue;
			if (buildingutil_HaveFoodVat(unit->GetImprovements(), tmp, unit->GetOwner()))
				continue;
			CityData *city = unit->GetData()->GetCityData();
			double fudge = (double)(g_theConstDB->Get(0)->GetStarvationWarningFudgeFactor()) / 100.0;
			if ((city->GetProducedFood() < city->GetConsumedFood()) &&
				((fudge * (city->GetStoredCityFood() + city->GetProducedFood())) <
				 city->GetConsumedFood())) {
				SlicObject *so = new SlicObject("23IACityWillStarve") ;
				so->AddRecipient(player->m_owner) ;
				so->AddCity(*unit) ;
				g_slicEngine->Execute(so) ;
				return(FALSE);
			}
		}
	}

	if (g_slicEngine->GetSegment("21IACannotAffordMaintenance")->TestLastShown(player->m_owner, 1)) {
		if (player->m_gold->BankruptcyImminent() &&
			(player->CalcTotalBuildingUpkeep() > 0)) {
			SlicObject *so = new SlicObject("21IACannotAffordMaintenance");
			so->AddRecipient(player->m_owner);
			so->AddCivilisation(player->m_owner);
			g_slicEngine->Execute(so);
			return(FALSE);
		}
	}

	if (g_slicEngine->GetSegment("22IACannotAffordSupport")->TestLastShown(player->m_owner, 1)) {
		int i;
		int n = player->GetAllCitiesList()->Num();
		double prod_total = 0.0;
		double fudge = (double)(g_theConstDB->Get(0)->GetSupportWarningFudgeFactor()) / 100.0;
		for (i=0; i<n; i++) {
			Unit *unit = &(player->GetAllCitiesList()->Access(i));
			if (!(unit->IsCity()))
				continue;
			CityData *city = unit->GetData()->GetCityData();
			prod_total += city->ProjectMilitaryContribution();
		}
		prod_total *= fudge;
		if (!(player->m_first_city) &&
			(prod_total < player->m_readiness->GetCost())) {
			SlicObject *so = new SlicObject("22IACannotAffordSupport");
			so->AddRecipient(player->m_owner);
			so->AddCivilisation(player->m_owner);
			g_slicEngine->Execute(so);
			return(FALSE);
		}
	}

	return(TRUE);
}


void TurnCount::NetworkEndTurn(BOOL force)
{
	Assert(!g_network.SetupMode());
	if(g_network.SetupMode())
		return;

	if (!VerifyEndTurn(force))
		return;

	if(g_network.IsClient()) {
		g_network.SendAction(new NetAction(NET_ACTION_END_TURN));
		g_network.SetMyTurn(FALSE);
		return;
	} else if(g_network.IsHost()) {
		g_gevManager->EndTurnRequest();
		return;
	}

	return;
#if 0 // Unreachable
	{
		if(g_player[g_selected_item->GetCurPlayer()]->IsNetwork())
		{
			for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
				if(g_player[i] && !g_player[i]->IsNetwork()) {
					if(!g_player[i]->IsTurnOver() && g_player[i]->GetCurRound() == m_round) {
						g_player[i]->EndTurnSoon();
					}
				}
			}
			return;
		}
	}

    EndThisTurnBeginNewTurn(FALSE);

#ifdef _DEBUG
	extern BOOL g_doingFastRounds;

	if (!g_doingFastRounds)
    {
		g_tiledMap->InvalidateMix();
		g_tiledMap->InvalidateMap();
		g_tiledMap->Refresh();
		g_radarMap->Update();
	}
#else
	g_tiledMap->InvalidateMix();
	g_tiledMap->InvalidateMap();
	g_tiledMap->Refresh();
	g_radarMap->Update();
#endif

#endif // Unreachable
}

void TurnCount::RunNewYearMessages(void)
{

	if (GetYear() >= g_theConstDB->Get(0)->GetEndOfGameYearEarlyWarning()) {
		if(!m_sentGameAlmostOverMessage) {
			m_sentGameAlmostOverMessage = TRUE;

			SendMsgEndOfGameEarlyWarning() ;
		}
	}

	if(GetYear() >= g_theConstDB->Get(0)->GetEndOfGameYear()) {
		if(!m_sentGameOverMessage) {
			sint32 i;
			sint32 highScore = -1;
			sint32 highPlayer = 1;

			for(i = 1; i < k_MAX_PLAYERS; i++) {
				if(g_player[i]) {
					if(g_player[i]->m_score->GetTotalScore() > highScore) {
						highScore = g_player[i]->m_score->GetTotalScore();
						highPlayer = i;
					}
				}
			}
			if(g_network.IsHost()) {
				g_network.Enqueue(new NetInfo(NET_INFO_CODE_GAME_OVER_OUT_OF_TIME,
				                              highPlayer));
			}

			for(i = 0; i < k_MAX_PLAYERS; i++) {
				if(g_player[i]) {
					if(i == highPlayer) {
						g_player[i]->GameOver(GAME_OVER_WON_OUT_OF_TIME, -1);
					} else {
						g_player[i]->GameOver(GAME_OVER_LOST_OUT_OF_TIME, -1);
					}
				}
			}
		} else {
			g_theGameSettings->SetKeepScore(FALSE);
		}
	}
}

void TurnCount::SendMsgEndOfGameEarlyWarning(void)
{
	SendMsgToAllPlayers("73EndOfGameTimeIsRunningOut") ;
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_TIMES_ALMOST_UP));
	}
}

void TurnCount::SendMsgToAllPlayers(MBCHAR *s)
	{
	sint32	i ;

	SlicObject *so = new SlicObject(s) ;

	for(i=0; i<k_MAX_PLAYERS; i++)
		{
		if ((g_player[i]) && (!g_player[i]->IsDead()))
			so->AddRecipient(i) ;

		}

	g_slicEngine->Execute(so) ;
	}

void TurnCount::CountActivePlayers()
{
	sint32 i;
	m_activePlayers = 0;
	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(g_player[i] &&
		   ((!g_player[i]->IsTurnOver()) ||
			g_player[i]->GetCurRound() != m_round)) {
			m_activePlayers++;
		}
	}
}

void TurnCount::PlayerDead(PLAYER_INDEX player)
{
	CountActivePlayers();
}

void TurnCount::RegisterNewPlayer(PLAYER_INDEX player)
{
	CountActivePlayers();
}

void TurnCount::SetHotSeat(BOOL on)
{
	m_isHotSeat = on;
}

void TurnCount::SetEmail(BOOL on)
{
	m_isEmail = on;
}

BOOL TurnCount::IsHotSeat()
{
	return m_isHotSeat;
}

BOOL TurnCount::IsEmail()
{
	return m_isEmail;
}

sint32 g_noai_stop_player = 1;

void TurnCount::NextRound(BOOL fromDirector, BOOL force)
{
	extern sint32 g_isCheatModeOn;
	if(g_isCheatModeOn)
		return;

	if(g_cantEndTurn)
		return;

	if (!g_selected_item)
		return;

	if (g_selected_item->GetCurPlayer() != g_selected_item->GetVisiblePlayer())
		return;

	if(!fromDirector) {
		g_selected_item->RegisterManualEndTurn();
	}

	if (g_theProfileDB->IsAIOn()) {

		TurnCount::SetStopPlayer(g_selected_item->GetCurPlayer());
	} else {
		g_noai_stop_player = g_selected_item->GetCurPlayer();
	}

#ifdef _DEBUG
sint32 finite_count=0;
#endif

	do
	{
		Assert(finite_count++ < 100);

		sint32 curPlayer = g_selected_item->GetCurPlayer();
		if((g_player[curPlayer]->IsHuman() ||
			(g_player[curPlayer]->IsNetwork() &&
			 g_network.IsLocalPlayer(curPlayer))) &&
		   g_selected_item->GetCurPlayer() == g_selected_item->GetVisiblePlayer()) {
			g_player[g_selected_item->GetCurPlayer()]->ProcessUnitOrders();
		}

		if(g_network.IsActive())
		{
			NetworkEndTurn();
			return;
		}

		if (!VerifyEndTurn(force))
			return;

		g_selected_item->SetPlayerOnScreen(g_selected_item->GetVisiblePlayer());
		if (g_theProfileDB->IsAIOn())
		{
			TurnCount::SetStopPlayer(g_selected_item->GetCurPlayer());
		}

		EndThisTurnBeginNewTurn();

		if(m_isHotSeat || m_isEmail)
		{
			if(!g_player[g_selected_item->GetCurPlayer()]->IsRobot())
			{
				g_selected_item->SetPlayerOnScreen(g_selected_item->GetCurPlayer());

				TurnCount::SetStopPlayer(g_selected_item->GetCurPlayer());
			}

			if(!g_player[g_selected_item->GetCurPlayer()]->IsRobot())
			{
				SendNextPlayerMessage();
			}

			g_director->NextPlayer();

			g_director->AddCopyVision();

			g_tiledMap->InvalidateMix();
			g_tiledMap->InvalidateMap();
			g_tiledMap->Refresh();
			g_radarMap->Update();
			InformMessages();
		}
	} while (0);
}

void TurnCount::ChooseHappinessPlayer()
{
	if(!g_player) {
		m_happinessPlayer = 0;
		return;
	}

	sint32 i;
	for(i = 0; i < k_MAX_PLAYERS + 1; i++) {
		m_happinessPlayer++;
		if(m_happinessPlayer >= k_MAX_PLAYERS)
			m_happinessPlayer = 0;

		if(!g_player[m_happinessPlayer])
			continue;
		if(!g_player[m_happinessPlayer]->IsRobot())
			continue;
		break;
	}

}

#ifdef _DEBUG
void TurnCount::LogPlayerStats(void)
{
	PLAYER_INDEX    playerNum   = g_selected_item->GetCurPlayer();
	MBCHAR          filename[80];
	sprintf(filename, "Playerlog%#.2d.txt", playerNum);
	FILE *  logfile = fopen(filename, "rt");

	if (logfile)
    {
        // Created before
		fclose(logfile);
		logfile = fopen(filename, "at");
       	if (!logfile) return;
    }
    else
    {
        // First creation
		logfile = fopen(filename, "wt");
		if (!logfile) return;

		fprintf(logfile, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t\n",
		        "Turn #",
		        "# Cities",
		        "Total Prod/Turn",
		        "Total Food/Turn",
		        "Total Gold/Turn",
		        "Total Sci/Turn",
		        "# Rioting",
		        "# Revolting",
		        "Gov Type",
		        "Workday",
		        "Wages",
		        "Rations",
		        "Science Setting",
		        "PW Setting",
		        "# of units",
		        "Unit Support",
		        "Total Pop",
		        "Largest City",
		        "Pollution",
		        "Income Percent",
		        "# advances known",
		        "AIP File");
	}

	UnitDynamicArray *  cityList    = g_player[playerNum]->GetAllCitiesList();
	sint32              citySize,
	                    maxCitySize = -1;
	sint32              numCitiesRioting = 0;
	sint32              totalPop = 0;
	sint32              totalFood = 0;
	sint32              totalProduction = 0;
	sint32              totalGold = 0;

	CityData            *cityData;
	Unit                city;

	for (int i = 0; i < cityList->Num(); i++) {
		city = cityList->Access(i);
		cityData = city.AccessData()->GetCityData();

		cityData->GetPop(citySize);

		totalPop += citySize;

		if (citySize > maxCitySize)
			maxCitySize = citySize;

		if (cityData->GetIsRioting())
			numCitiesRioting++;

		totalFood       += cityData->GetNetCityFood();
		totalProduction += cityData->GetNetCityProduction();
		totalGold       += cityData->GetNetCityGold();

	}

	fprintf(logfile, "%d\t", m_round);
	fprintf(logfile, "%d\t", g_player[playerNum]->GetNumCities());
	fprintf(logfile, "%d\t", totalProduction);
	fprintf(logfile, "%d\t", totalFood);
	fprintf(logfile, "%d\t", totalGold);
	fprintf(logfile, "%d\t", g_player[playerNum]->m_gold->GetScience());
	fprintf(logfile, "%d\t", numCitiesRioting);
	fprintf(logfile, "%d\t", g_player[playerNum]->GetNumRevolted());
	fprintf(logfile, "%d\t", g_player[playerNum]->GetGovernmentType());
	fprintf(logfile, "%#.3f\t", g_player[playerNum]->GetWorkdayPerPerson());
	fprintf(logfile, "%#.3f\t", g_player[playerNum]->GetWagesPerPerson());
	fprintf(logfile, "%#.3f\t", g_player[playerNum]->GetRationsPerPerson());

	double taxRate;
	g_player[playerNum]->GetScienceTaxRate(taxRate);
	fprintf(logfile, "%#.2f\t", taxRate);

	fprintf(logfile, "%#.2f\t", g_player[playerNum]->m_materialsTax);
	fprintf(logfile, "%d\t", g_player[playerNum]->GetAllUnitList()->Num());
	fprintf(logfile, "%d\t", g_player[playerNum]->GetReadinessCost());
	fprintf(logfile, "%d\t", totalPop);
	fprintf(logfile, "%d\t", maxCitySize);
	fprintf(logfile, "%d\t", g_player[playerNum]->GetCurrentPollution());
	fprintf(logfile, "%#.3f\t", g_player[playerNum]->GetIncomePercent());

	sint32 numAdvances = 0;
	for (sint32 adv=0; adv<g_player[playerNum]->NumAdvances(); adv++)
		if (g_player[playerNum]->HasAdvance(adv)) {
			numAdvances++;
		}
	fprintf(logfile, "%d\t", numAdvances);

    fprintf(logfile, "\n");
	fclose(logfile);
}

#endif

void TurnCount::NotifyBecameHost()
{
	CountActivePlayers();
}

void TurnCount::SendNextPlayerMessage()
{
	g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_SendEmailAndHotSeatMessage,
	                       GEA_End);

}

// Only called from the event:
void TurnCount::SendNextPlayerMessageEvent()
{
	if(!m_isHotSeat && !m_isEmail)
		return;

	sint32 player = g_selected_item->GetCurPlayer();

	if(!g_player[player])
		return;

	if(m_isEmail) {

		g_isScenario = FALSE;

		MBCHAR fullPath[_MAX_PATH], *c, *startc, *fc;
		strcpy(fullPath, g_civPaths->GetDesktopPath());
		// JJB changed this from CTP to CTP2 to avoid confusion between the two games
		strcat(fullPath, "\\CTP2 Email To ");

		startc = g_player[player]->m_email;
		c = startc;
		fc = &fullPath[strlen(fullPath)];
		while(*c && ((c - startc) < (_MAX_PATH - 50))) {
			if((*c >= 'a' && *c <= 'z') ||
			   (*c >= 'A' && *c <= 'Z') ||
			   (*c >= '0' && *c <= '9') ||
			   *c == '-' || *c == '_' || *c == '@' ||
			   *c == '.') {
				*fc = *c;
				fc++;
				*fc = 0;
			}
			c++;
		}
		MBCHAR turnString[_MAX_PATH];
		// JJB changed this from m_round to GetRound()
		// since m_round seems to always be zero
		sprintf(turnString, " (Turn %d)", GetRound()); // New turn is changed with the BeginTurn event, which still has to be executed.
		strcat(fullPath, turnString);
		strcat(fullPath, ".c2g");
		GameFile::SaveGame(fullPath, NULL);
	}

	SlicObject * so =
	    new SlicObject(m_isHotSeat ? "104NextHotSeatPlayer" : "105NextEmailPlayer");
	so->AddRecipient(player);
	so->AddCivilisation(player);
	if(m_isEmail)
		so->AddAction(g_player[player]->m_email);

	g_slicEngine->Execute(so);

}
