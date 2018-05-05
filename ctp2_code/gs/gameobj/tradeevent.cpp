#include "ctp/c3.h"
#include "gs/gameobj/tradeevent.h"

#include "gs/gameobj/TradeRoute.h"
#include "gs/events/GameEventUser.h"
#include "gs/gameobj/Events.h"
#include "gs/gameobj/Army.h"
#include "gs/gameobj/TradeRouteData.h"

STDEHANDLER(KillTradeRouteEvent)
{
	TradeRoute  route;
	sint32      cause;

	if(!args->GetTradeRoute(0, route)) return GEV_HD_Continue;
	if(!args->GetInt(0, cause)) return GEV_HD_Continue;

	route.Kill(static_cast<CAUSE_KILL_TRADE_ROUTE>(cause));
	return GEV_HD_Continue;
}

STDEHANDLER(SetPiratingArmyEvent)
{
	TradeRoute route;
	Army a;

	if(!args->GetTradeRoute(0, route)) return GEV_HD_Continue;
	args->GetArmy(0, a);    // invalid army = stop pirating

	route->SetPiratingArmy(a);

	return GEV_HD_Continue;
}

void tradeevent_Initialize()
{
	g_gevManager->AddCallback(GEV_KillTradeRoute, GEV_PRI_Primary, &s_KillTradeRouteEvent);
	g_gevManager->AddCallback(GEV_SetPiratingArmy, GEV_PRI_Primary, &s_SetPiratingArmyEvent);
}

void tradeevent_Cleanup()
{
}
