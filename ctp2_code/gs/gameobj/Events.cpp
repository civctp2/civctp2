//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Event initialization and cleanup
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
// - None
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "gs/gameobj/Events.h"
#include "gs/events/GameEventUser.h"

#include "gs/gameobj/Player.h"
#include "gs/gameobj/Score.h"
#include "gs/gameobj/Army.h"
#include "robot/pathing/Path.h"
#include "gs/gameobj/Order.h"

#include "gfx/spritesys/director.h"
#include "sound/gamesounds.h"
#include "gs/outcom/AICause.h"
#include "ui/aui_ctp2/SelItem.h"

#include "gs/slic/SlicObject.h"
#include "gs/utility/RandGen.h"

#include "gs/slic/SlicEngine.h"

#include "gs/gameobj/ArmyEvent.h"
#include "gs/gameobj/CityEvent.h"
#include "gs/gameobj/PlayerEvent.h"
#include "ui/aui_ctp2/SelItemEvent.h"
#include "gs/gameobj/UnitEvent.h"
#include "gs/utility/TurnCntEvent.h"
#include "sound/soundevent.h"
#include "ui/interface/interfaceevent.h"
#include "gs/gameobj/improvementevent.h"
#include "gs/world/worldevent.h"
#include "gs/gameobj/combatevent.h"
#include "net/general/networkevent.h"
#include "gs/gameobj/tradeevent.h"

#include "ui/interface/diplomacywindow.h"

#include "gfx/spritesys/directorevent.h"

#include "gs/gameobj/Order.h"

#include "gs/world/World.h"
#include "robot/aibackdoor/bset.h"
#include "ui/interface/armymanagerwindow.h"
#include "ui/interface/trademanager.h"
#include "gs/gameobj/tradeevent.h"
#include "ui/interface/unitmanager.h"

#include "ai/ctpai.h"
#include "ui/interface/dipwizard.h"

#include "gs/gameobj/EventTracker.h"
#include "gs/gameobj/FeatTracker.h"

#include "gs/gameobj/gaiacontroller.h"

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
