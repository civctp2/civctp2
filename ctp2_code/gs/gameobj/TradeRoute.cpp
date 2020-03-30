#include "c3.h"
#include "TradeRoute.h"

#include "World.h"
#include "TradePool.h"
#include "player.h"
#include "TradeRouteData.h"
#include "ResourceRecord.h"             // g_theResourceDB
#include "director.h"
#include "MapPoint.h"
#include "dynarr.h"
#include "Cell.h"
#include "UnitPool.h"
#include "SlicObject.h"
#include "SlicEngine.h"

#include "network.h"
#include "net_info.h"
#include "colorset.h"
#include "radarmap.h"
#include "trademanager.h"

extern World* g_theWorld;
extern Player** g_player;
extern Director	*g_director;
extern UnitPool *g_theUnitPool;

class TradeReport : public SlicObject
{
public:
	TradeReport(char const *what, TradeRoute route, Unit const &civ, Unit const &rcp) : SlicObject(what)
	{
		ROUTE_TYPE type;
		sint32 good;
		route.GetSourceResource(type, good);

		Unit src(route->GetSource());
		Unit dst(route->GetDestination());

		AddGood(good);
		AddCity(src);
		AddCity(dst);
		AddGold(g_theResourceDB->Get(good)->GetFood());       // Missuse to pass integer to msg
		AddGold(g_theResourceDB->Get(good)->GetProduction()); // Missuse to pass integer to msg
		AddGold(g_theResourceDB->Get(good)->GetGold());       // Missuse to pass integer to msg
		AddCivilisation(civ.GetOwner());
		AddRecipient(rcp.GetOwner());
	};
};

bool TradeRoute::IsValid() const
{
	return g_theTradePool->IsValid(m_id);
}

void TradeRoute::KillRoute(CAUSE_KILL_TRADE_ROUTE cause) // Mapped to TradeRoute::Kill in TradeRoute.h
{
	TradeRouteData* data = AccessData();
	data->Remove(cause); // Remove route from game but keep reference as long as m_seenBy is not zero
	
	Unit source(data->GetSource()), dest(data->GetDestination());

	if(g_theUnitPool->IsValid(source))
		source.DelTradeRoute(*this); // Remove route from CityData (m_tradeSourceList), essential because routes of m_tradeSourceList are expected to be active (i.e. no checks on route.IsActive())
	if(g_theUnitPool->IsValid(dest))
		dest.DelTradeRoute(*this); // Remove route from CityData (m_tradeDestinationList), essential because routes of m_tradeDestinationList are expected to be active (i.e. no checks on route.IsActive())
	
	if(g_theUnitPool->IsValid(source) && g_theUnitPool->IsValid(dest))
	{
		if(source.GetOwner() != dest.GetOwner())
		{
			switch(cause)
			{
				case CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED:
				case CAUSE_KILL_TRADE_ROUTE_PIRATED:
				case CAUSE_KILL_TRADE_ROUTE_CHANGED_DESTINATION:
				case CAUSE_KILL_TRADE_ROUTE_BETTER_OFFER:
				{
					g_slicEngine->Execute(new TradeReport("360SenderKilledTradeRoute", *this, source, dest));
					break;
				}
				case CAUSE_KILL_TRADE_ROUTE_RECIPIENT_KILLED:
				{
					g_slicEngine->Execute(new TradeReport("360SenderKilledTradeRoute", *this, dest, source));
					break;
				}
				case CAUSE_KILL_TRADE_ROUTE_CITY_CHANGED_OWNER:
				case CAUSE_KILL_TRADE_ROUTE_CITY_DIED:
				case CAUSE_KILL_TRADE_ROUTE_DIPLOMATIC_AGREEMENT:
				case CAUSE_KILL_TRADE_ROUTE_WAR:
				case CAUSE_KILL_TRADE_ROUTE_EMBARGO:
				{
					g_slicEngine->Execute(new TradeReport("360SenderKilledTradeRoute", *this, source, dest));
					g_slicEngine->Execute(new TradeReport("360SenderKilledTradeRoute", *this, dest, source));
					break;
				}
				default: // CAUSE_KILL_TRADE_ROUTE_UNKNOWN CAUSE_KILL_TRADE_ROUTE_RESET
				{
				}
			}
		}
	}

	if ((NULL != g_player)  &&
	    (NULL != g_player[GetPayingFor()]))
	{
		g_player[GetPayingFor()]->RemoveTradeRoute(*this, cause); // Brings used caravans/trade-units back - 1
	}

	TradeRoute tmp(*this);
	tmp.Deactivate();                       // Deactivate route => if a tile of the path is seen again the route will not be drawn any more
	tmp.RemoveSeenByBit(source.GetOwner()); // Owner should not see it any more instantly
	tmp.RemoveSeenByBit(dest.GetOwner());   // Receiver should not see it any more instantly
	tmp.RevealTradeRouteStateIfInVision();  // Reveal trade route state to players where route is in vision, must be after Deactivate()
}

void TradeRoute::RemoveUnseenRoute() // Mapped to TradeRoute::Kill in TradeRoute.h
{
	TradeRouteData* data = AccessData();
	if(data->SeenByBits() == 0)
	{
		TradeRoute tmp(*this);
		CAUSE_KILL_TRADE_ROUTE cause= data->IsRemoved();
		tmp.RemoveAllReferences(cause);
	}
}

void TradeRoute::RemoveAllReferences(CAUSE_KILL_TRADE_ROUTE cause)
{
	g_director->TradeActorDestroy(*this);  // Good animation
	TradeRouteData* data = AccessData();

	Unit source(data->GetSource()), dest(data->GetDestination());
	if(g_theUnitPool->IsValid(source))
		source.DelTradeRoute(*this); // Remove route from CityData (m_tradeSourceList), essential because routes of m_tradeSourceList are expected to be active (i.e. no checks on route.IsActive())
	if(g_theUnitPool->IsValid(dest))
		dest.DelTradeRoute(*this);   // Remove route from CityData (m_tradeDestinationList), essential because routes of m_tradeDestinationList are expected to be active (i.e. no checks on route.IsActive())
	data->RemoveFromCells();

#ifdef RECIPROCAL_ROUTES

	if (GetRecip().IsValid())
	{
		AccessRecip().SetRecip(TradeRoute());
		AccessRecip().KillRoute();
	}
#endif

	if(g_network.IsHost())
	{
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_TRADE_ROUTE, (uint32)*this, (uint32)cause));
	}
	else if(g_network.IsClient())
	{
		g_network.AddDeadUnit(m_id);
	}

	g_theTradePool->Remove(*this);

	TradeManager::Notify();
}

Unit TradeRoute::GetSource() const
{
	return GetData()->GetSource();
}

PLAYER_INDEX TradeRoute::GetOwner() const
{
	return GetData()->GetOwner();
}

PLAYER_INDEX TradeRoute::GetPayingFor() const
{
	return GetData()->GetPayingFor();
}

const TradeRouteData* TradeRoute::GetData() const
{
	Assert(g_theTradePool);
	return g_theTradePool->GetTradeRoute(*this);
}

TradeRouteData* TradeRoute::AccessData() const
{
	Assert(g_theTradePool);
	return g_theTradePool->AccessTradeRoute(*this);
}

Unit TradeRoute::GetDestination() const
{
	return GetData()->GetDestination();
}

sint32 TradeRoute::GetCost() const
{
	return GetData()->GetCost();
}

const DynamicArray<MapPoint>* TradeRoute::GetPath() const
{
	return GetData()->GetPath();
}

const DynamicArray<MapPoint>* TradeRoute::GetSelectedPath() const
{
	return GetData()->GetSelectedPath();
}

void TradeRoute::GetSourceResource(ROUTE_TYPE &type, sint32 &resource) const
{
	GetData()->GetSourceResource(type, resource);
}

BOOL TradeRoute::PassesThrough(sint32 player) const
{
	return GetData()->PassesThrough(player);
}

BOOL TradeRoute::CrossesWater() const
{
	return GetData()->CrossesWater();
}

BOOL TradeRoute::IsActive() const
{
	return GetData()->IsActive();
}

void TradeRoute::Activate()
{
	AccessData()->Activate();
}

void TradeRoute::Deactivate()
{
	AccessData()->Deactivate();
}

StringId TradeRoute::GetResourceName() const
{
	return GetData()->GetResourceName();
}

uint32 TradeRoute::GetColor() const
{
	return GetData()->GetColor();
}

void TradeRoute::SetColor( uint32 color )
{
	AccessData()->SetColor(color);
}

void TradeRoute::AddSeenByBit( sint32 player )
{
	AccessData()->AddSeenByBit(player);
}

void TradeRoute::RemoveSeenByBit( sint32 player )
{
	AccessData()->RemoveSeenByBit(player);
}

bool TradeRoute::SeenBy( sint32 player ) const
{
	return GetData()->SeenBy(player);
}

void TradeRoute::RedrawRadarMapAlongRoute()
{
	AccessData()->RedrawRadarMapAlongRoute();
}

void TradeRoute::RevealTradeRouteStateIfInVision()
{
	AccessData()->RevealTradeRouteStateIfInVision();
}

void TradeRoute::ReturnPath(const PLAYER_INDEX owner, DynamicArray<MapPoint> &waypoints,
                            DynamicArray<MapPoint> &fullpath,
                            double &cost)
{
	AccessData()->ReturnPath(owner, waypoints, fullpath, cost);
}

/* unused
void TradeRoute::SetPath(DynamicArray<MapPoint> &fullpath, DynamicArray<MapPoint> &waypoints)
{
	AccessData()->SetPath(fullpath, waypoints);
}
*/

void TradeRoute::GenerateSelectedPath(const MapPoint &pos)
{
	AccessData()->GenerateSelectedPath(pos);
}

sint32 TradeRoute::AddSelectedWayPoint(const MapPoint &pos)
{
	return AccessData()->AddSelectedWayPoint(pos);
}

BOOL TradeRoute::IsSelectedPathSame()
{
	return AccessData()->IsSelectedPathSame();
}

BOOL TradeRoute::IsPosInSelectedPath(const MapPoint &pos)
{
	return AccessData()->IsPosInSelectedPath(pos);
}

BOOL TradeRoute::IsPosInPath(const MapPoint &pos)
{
	return AccessData()->IsPosInPath(pos);
}

void TradeRoute::UpdateSelectedCellData(TradeRoute route)
{
	AccessData()->UpdateSelectedCellData(route);
}

void TradeRoute::ClearSelectedCellData(TradeRoute route)
{
	AccessData()->ClearSelectedCellData(route);
}

BOOL TradeRoute::InitSelectedData()
{
	return AccessData()->InitSelectedData();
}

void TradeRoute::ClearSelectedPath()
{
	AccessData()->ClearSelectedPath();
}

sint32 TradeRoute::GetPathSelectionState() const
{
	return GetData()->GetPathSelectionState();
}

void TradeRoute::SetPathSelectionState( sint32 state )
{
	AccessData()->SetPathSelectionState(state);
}

/* unused
void TradeRoute::BeginTurn()
{
	AccessData()->BeginTurn();
}
*/

sint32 TradeRoute::GetGoldInReturn() const
{
	return GetData()->GetGoldInReturn();
}

void TradeRoute::DontAdjustPointsWhenKilled()
{
	AccessData()->DontAdjustPointsWhenKilled();
}
