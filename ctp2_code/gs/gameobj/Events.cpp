





#include "c3.h"
#include "Events.h"
#include "GameEventUser.h"

#include "player.h"
#include "Score.h"
#include "Army.h"
#include "Path.h"
#include "Order.h"

#include "director.h"
#include "gamesounds.h"
#include "AICause.h"
#include "SelItem.h"

#include "SlicObject.h"
#include "RandGen.h"
#include "ConstDB.h"

#include "SlicEngine.h"

#include "ArmyEvent.h"
#include "CityEvent.h"
#include "PlayerEvent.h"
#include "SelItemEvent.h"
#include "UnitEvent.h"
#include "TurnCntEvent.h"
#include "soundevent.h"
#include "interfaceevent.h"
#include "improvementevent.h"
#include "worldevent.h"
#include "combatevent.h"
#include "networkevent.h"
#include "tradeevent.h"

#include "diplomacywindow.h"

#include "directorevent.h"

#include "Order.h"

#include "World.h"
#include "bset.h"
#include "armymanagerwindow.h"
#include "trademanager.h"
#include "tradeevent.h"
#include "unitmanager.h"

#include "ctpai.h"
#include "dipwizard.h"

#include "EventTracker.h"
#include "FeatTracker.h"

#include "gaiacontroller.h"

STDEHANDLER(ScoreEventTest)
{
	Assert(gameEventType == GEV_CalcScores);
	GameEventArgument *arg = args->GetArg(GEA_Player, 0);
	sint32 player;
	if(arg->GetPlayer(player)) {
		g_player[player]->m_score->AddYearAtPeace();
	}
	return GEV_HD_Continue;
}

											   
void events_Initialize()
{
	armyevent_Initialize();
	cityevent_Initialize();
	playerevent_Initialize();
	selecteditemevent_Initialize();
	unitevent_Initialize();
	turncountevent_Initialize();
	soundevent_Initialize();
	interfaceevent_Initialize();
	improvementevent_Initialize();
	directorevent_Initialize();
	worldevent_Initialize();
	combatevent_Initialize();
	tradeevent_Initialize();
	
	trackerevent_Initialize();

	g_gevManager->AddCallback(GEV_CalcScores, GEV_PRI_Primary, &s_ScoreEventTest);

	Order::AssociateEventsWithOrders();

	networkevent_Initialize();

	FeatTracker::InitializeEvents();

	
	DiplomacyWindow::InitializeEvents();
	ArmyManagerWindow::InitializeEvents();
	TradeManager::InitializeEvents();
	UnitManager::InitializeEvents();
	DipWizard::InitializeEvents();

	
	CtpAi::InitializeEvents();

	GaiaController::InitializeEvents();
}

void events_Cleanup()
{
	armyevent_Cleanup();
	cityevent_Cleanup();
	playerevent_Cleanup();
	selecteditemevent_Cleanup();
	unitevent_Cleanup();
	turncountevent_Cleanup();
	soundevent_Cleanup();
	interfaceevent_Cleanup();
	improvementevent_Cleanup();
	directorevent_Cleanup();
	worldevent_Cleanup();
	combatevent_Cleanup();
	tradeevent_Cleanup();

	networkevent_Cleanup();

	FeatTracker::CleanupEvents();

	
	DiplomacyWindow::CleanupEvents();
	ArmyManagerWindow::CleanupEvents();
	TradeManager::CleanupEvents();
	UnitManager::CleanupEvents();

	CtpAi::CleanupEvents();
}
