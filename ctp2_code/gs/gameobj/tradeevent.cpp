#include "c3.h"
#include "tradeevent.h"
#include "TradeRoute.h"
#include "GameEventUser.h"
#include "Events.h"
#include "Army.h"
#include "TradeRouteData.h"

STDEHANDLER(KillTradeRouteEvent)
{
	TradeRoute route;
	CAUSE_KILL_TRADE_ROUTE cause;

	if(!args->GetTradeRoute(0, route)) return GEV_HD_Continue;
	if(!args->GetInt(0, (sint32 &)cause)) return GEV_HD_Continue;

	route.Kill(cause);
	return GEV_HD_Continue;
}

STDEHANDLER(SetPiratingArmyEvent)
{
	TradeRoute route;
	Army a;

	if(!args->GetTradeRoute(0, route)) return GEV_HD_Continue;

	args->GetArmy(0, a);

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
