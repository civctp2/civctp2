#include "c3.h"
#include "Events.h"
#include "TurnCntEvent.h"
#include "GameEventUser.h"

#include "player.h"

#include "profileDB.h"
#include "director.h"
#include "network.h"
#include "ctpai.h"
#include "newturncount.h"

STDEHANDLER(BeginTurnEvent)
{
	sint32 player;
	sint32 round;
	if(!(args->GetPlayer(0, player))) {
		return GEV_HD_Continue;
	}

	if(!(args->GetInt(0, round))) {
		return GEV_HD_Continue;
	}

	Assert(g_player[player] != NULL);

	g_player[player]->m_current_round = round;
	g_player[player]->BeginTurn();

	g_director->NextPlayer();
	if(g_theProfileDB->IsAIOn() && (!g_network.IsClient())) {




		CtpAi::BeginTurn(player);
	}

	sint32 year = NewTurnCount::GetCurrentYear();

	return GEV_HD_Continue;
}

void turncountevent_Initialize()
{
	g_gevManager->AddCallback(GEV_BeginTurn, GEV_PRI_Primary, &s_BeginTurnEvent);
}

void turncountevent_Cleanup()
{
}
