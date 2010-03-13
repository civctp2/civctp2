









#include "c3.h"
#include "TradeRoute.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "TradePool.h"
#include "player.h"
#include "TradeRouteData.h"
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

bool TradeRoute::IsValid() const
{
	return g_theTradePool->IsValid(m_id);
}

void TradeRoute::KillRoute(CAUSE_KILL_TRADE_ROUTE cause)
{
	TradeRoute tmp(*this);
	tmp.RemoveAllReferences(cause);
}

void TradeRoute::RemoveAllReferences(CAUSE_KILL_TRADE_ROUTE cause)
{
	g_director->TradeActorDestroy(*this);
	TradeRouteData* data = AccessData();

	
	
	
	

	Unit source(data->GetSource()), dest(data->GetDestination());

	
	
	if(g_theUnitPool->IsValid(source))
		source.DelTradeRoute(*this);
	if(g_theUnitPool->IsValid(dest))
		dest.DelTradeRoute(*this);

	if(g_theUnitPool->IsValid(source) && g_theUnitPool->IsValid(dest)) {
		if(source.GetOwner() != dest.GetOwner()) {
			if(cause == CAUSE_KILL_TRADE_ROUTE_SENDER_KILLED) {
				SlicObject *so = new SlicObject("360SenderKilledTradeRoute");
				ROUTE_TYPE type;
				sint32 good;
				GetSourceResource(type, good);
				so->AddGood(good);
				so->AddCity(source);
				so->AddCity(dest);
				so->AddCivilisation(source.GetOwner());
				so->AddRecipient(dest.GetOwner());
				g_slicEngine->Execute(so);
			}
		}
	}

    if ((NULL != g_player)  &&
        (NULL != g_player[GetPayingFor()])) {
    	g_player[GetPayingFor()]->RemoveTradeRoute(*this, cause);
    }

	data->RemoveFromCells();

#ifdef RECIPROCAL_ROUTES
	
	if (GetRecip().IsValid()) 
	{
		AccessRecip().SetRecip(TradeRoute());
		AccessRecip().KillRoute();
	}
#endif
	
	
	
	
	if(g_network.IsHost()) {
		g_network.Enqueue(new NetInfo(NET_INFO_CODE_KILL_TRADE_ROUTE, (uint32)*this, (uint32)cause));
	} else if(g_network.IsClient()) {
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

double TradeRoute::GetCost() const
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

TradeRoute TradeRoute::GetRecip() const
{
	return GetData()->GetRecip();
}

TradeRoute TradeRoute::AccessRecip()
{
	return AccessData()->AccessRecip();
}

void TradeRoute::SetRecip(TradeRoute route)
{
	AccessData()->SetRecip(route);
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

uint32 TradeRoute::GetOutlineColor() const
{
	return GetData()->GetOutlineColor();
}

void TradeRoute::SetColor( uint32 color )
{
	 AccessData()->SetColor(color);
}

void TradeRoute::SetOutlineColor( uint32 color )
{
	 AccessData()->SetOutlineColor(color);
}


void TradeRoute::ReturnPath(const PLAYER_INDEX owner, DynamicArray<MapPoint> &waypoints,
							DynamicArray<MapPoint> &fullpath,
							double &cost)
{
	AccessData()->ReturnPath(owner, waypoints, fullpath, cost);
}

void TradeRoute::SetPath(DynamicArray<MapPoint> &fullpath,
						 DynamicArray<MapPoint> &waypoints)
{
	AccessData()->SetPath(fullpath, waypoints);
}

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

void TradeRoute::BeginTurn()
{
	AccessData()->BeginTurn();
}

sint32 TradeRoute::GetGoldInReturn() const
{
	return GetData()->GetGoldInReturn();
}

void TradeRoute::DontAdjustPointsWhenKilled()
{
	AccessData()->DontAdjustPointsWhenKilled();
}
