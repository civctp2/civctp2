#include "c3.h"
#include "Order.h"
#include "Path.h"
#include "TurnCnt.h"
#include "civarchive.h"
#include "pool.h"
#include "GameEventArgList.h"

#include <vector>
#include "c3debugstl.h"
#include "OrderRecord.h"
#include "GameEventManager.h"

extern TurnCount *g_turn;
extern Pool<Order> *g_theOrderPond;

OrderInfo g_orderInfo[] = {
	{UNIT_ORDER_NONE, "ORDER_NONE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_MOVE, "ORDER_MOVE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_PATROL, "ORDER_PATROL", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_CIRCULAR_PATROL, "ORDER_CIRCULAR_PATROL", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_ENTRENCH, "ORDER_ENTRENCH", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_SLEEP, "ORDER_SLEEP", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_DETRENCH, "ORDER_DETRENCH", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_UNLOAD, "ORDER_UNLOAD", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_MOVE_TO, "ORDER_MOVE_TO", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_TELEPORT_TO, "ORDER_TELEPORT_TO", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_EXPEL_TO, "ORDER_EXPEL_TO", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_GROUP, "ORDER_GROUP", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_UNGROUP, "ORDER_UNGROUP", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INVESTIGATE_CITY, "ORDER_INVESTIGATE_CITY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_NULLIFY_WALLS, "ORDER_NULLIFY_WALLS", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_STEAL_TECHNOLOGY, "ORDER_STEAL_TECHNOLOGY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INCITE_REVOLUTION, "ORDER_INCITE_REVOLUTION", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_ASSASSINATE, "ORDER_ASSASSINATE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INVESTIGATE_READINESS, "ORDER_INVESTIGATE_READINESS", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_BOMBARD, "ORDER_BOMBARD", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_SUE, "ORDER_SUE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_FRANCHISE, "ORDER_FRANCHISE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_SUE_FRANCHISE, "ORDER_SUE_FRANCHISE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_EXPEL, "ORDER_EXPEL", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_ESTABLISH_EMBASSY, "ORDER_ESTABLISH_EMBASSY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_THROW_PARTY, "ORDER_THROW_PARTY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_CAUSE_UNHAPPINESS, "ORDER_CAUSE_UNHAPPINESS", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_PLANT_NUKE, "ORDER_PLANT_NUKE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_SLAVE_RAID, "ORDER_SLAVE_RAID", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_ENSLAVE_SETTLER, "ORDER_ENSLAVE_SETTLER", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_UNDERGROUND_RAILWAY, "ORDER_UNDERGROUND_RAILWAY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INCITE_UPRISING, "ORDER_INCITE_UPRISING", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_BIO_INFECT, "ORDER_BIO_INFECT", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_NANO_INFECT, "ORDER_NANO_INFECT", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_CONVERT, "ORDER_CONVERT", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_REFORM, "ORDER_REFORM", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INDULGENCE, "ORDER_INDULGENCE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_SOOTHSAY, "ORDER_SOOTHSAY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_CREATE_PARK, "ORDER_CREATE_PARK", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_PILLAGE, "ORDER_PILLAGE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INJOIN, "ORDER_INJOIN", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_INTERCEPT_TRADE, "ORDER_INTERCEPT_TRADE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_PARADROP_MOVE, "ORDER_PARADROP_MOVE", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_SET_UNLOAD_MOVEMENT_POINTS, "ORDER_SET_UNLOAD_MOVEMENT_POINTS", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_GROUP_UNIT, "ORDER_GROUP_UNIT", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_DISBAND, "ORDER_DISBAND_ARMY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_FINISH_ATTACK, "ORDER_FINISH_ATTACK", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_UNLOAD_ONE_UNIT, "ORDER_UNLOAD_ONE_UNIT", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_BOARD_TRANSPORT, "ORDER_BOARD_TRANSPORT", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_WAKE_UP, "ORDER_WAKE_UP", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_PILLAGE_UNCONDITIONALLY, "ORDER_PILLAGE_UNCONDITIONALLY", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_MOVE_THEN_UNLOAD, "ORDER_MOVE_THEN_UNLOAD", 0, 0, 0, 0, NULL},
	{UNIT_ORDER_ADVERTISE, "ORDER_ADVERTISE", 0, 0, 0, 0, NULL},
    {UNIT_ORDER_SETTLE, "ORDER_SETTLE", 0, 0, 0, 0, NULL},
    {UNIT_ORDER_LAUNCH, "ORDER_LAUNCH", 0, 0, 0, 0, NULL},
    {UNIT_ORDER_TARGET, "ORDER_TARGET", 0, 0, 0, 0, NULL},
    {UNIT_ORDER_CLEAR_TARGET, "ORDER_CLEAR_TARGET", 0, 0, 0, 0, NULL},
    {UNIT_ORDER_PLAGUE, "ORDER_PLAGUE", 0, 0, 0, 0, NULL},
    {UNIT_ORDER_VICTORY_MOVE, "ORDER_VICTORY_MOVE", 0, 0, 0, 0, NULL},
};
sint32 g_numOrderInfo = sizeof(g_orderInfo) / sizeof(OrderInfo);
sint32 g_orderInfoMap[UNIT_ORDER_MAX];

Order::Order(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point, sint32 arg)
{
	m_order = order;
	m_path = path;
	m_round = g_turn->GetRound();
	m_point = point;
	m_argument = arg;
	m_gameEventArgs = NULL;
	m_eventType = GEV_MAX;
}

Order::Order(CivArchive &archive)
{
	Serialize(archive);
}

Order::~Order()
{
	if(m_path) {
		delete m_path;
		m_path = NULL;
	}

	if(m_gameEventArgs) {
		delete m_gameEventArgs;
		m_gameEventArgs = NULL;
	}
}

void Order::Serialize(CivArchive &archive)
{
	uint8 hasPath;
	uint8 hasArgs;

	m_point.Serialize(archive);

	if(archive.IsStoring()) {
		archive.PutSINT32(m_order);
		archive.PutSINT32(m_round);
		archive.PutSINT32(m_argument);
		archive.PutSINT32(m_eventType);

		hasPath = m_path != NULL;
		archive << hasPath;
		if(hasPath)
			m_path->Serialize(archive);

		hasArgs = m_gameEventArgs != NULL;
		archive << hasArgs;
		if(hasArgs)
			m_gameEventArgs->Serialize(archive);

	} else {
		m_order = (UNIT_ORDER_TYPE)archive.GetSINT32();
		m_round = archive.GetSINT32();
		m_argument = archive.GetSINT32();
		m_eventType = (GAME_EVENT)archive.GetSINT32();

		archive >> hasPath;
		if(hasPath) {
			m_path = new Path();
			m_path->Serialize(archive);
		} else
			m_path = NULL;

		archive >> hasArgs;
		if(hasArgs) {
			m_gameEventArgs = new GameEventArgList(archive);
		} else {
			m_gameEventArgs = NULL;
		}
	}
}

void *Order::operator new(size_t size)
{
	int index;
	Order *order = g_theOrderPond->Get_Next_Pointer(index);
	order->m_index = index;
	return order;
}

void Order::operator delete(void *ptr)
{
	Order *order = (Order *)ptr;

	g_theOrderPond->Release_Pointer(order->m_index);
}

bool Order::IsSpecialAttack(UNIT_ORDER_TYPE order)
{
	switch(order) {
		case UNIT_ORDER_INVESTIGATE_CITY:
		case UNIT_ORDER_NULLIFY_WALLS:
		case UNIT_ORDER_STEAL_TECHNOLOGY:
		case UNIT_ORDER_INCITE_REVOLUTION:
		case UNIT_ORDER_ASSASSINATE:
		case UNIT_ORDER_INVESTIGATE_READINESS:
		case UNIT_ORDER_BOMBARD:
		case UNIT_ORDER_SUE:
		case UNIT_ORDER_FRANCHISE:
		case UNIT_ORDER_SUE_FRANCHISE:
		case UNIT_ORDER_EXPEL:
		case UNIT_ORDER_ESTABLISH_EMBASSY:
		case UNIT_ORDER_THROW_PARTY:
		case UNIT_ORDER_CAUSE_UNHAPPINESS:
		case UNIT_ORDER_PLANT_NUKE:
		case UNIT_ORDER_SLAVE_RAID:
		case UNIT_ORDER_ENSLAVE_SETTLER:
		case UNIT_ORDER_UNDERGROUND_RAILWAY:
		case UNIT_ORDER_INCITE_UPRISING:
		case UNIT_ORDER_BIO_INFECT:
		case UNIT_ORDER_NANO_INFECT:
		case UNIT_ORDER_CONVERT:
		case UNIT_ORDER_REFORM:
		case UNIT_ORDER_INDULGENCE:
		case UNIT_ORDER_SOOTHSAY:
		case UNIT_ORDER_CREATE_PARK:
		case UNIT_ORDER_PILLAGE:
		case UNIT_ORDER_INJOIN:
		case UNIT_ORDER_INTERCEPT_TRADE:
		case UNIT_ORDER_ADVERTISE:
		case UNIT_ORDER_SETTLE:
		case UNIT_ORDER_DISBAND:
		case UNIT_ORDER_UNLOAD:
		case UNIT_ORDER_LAUNCH:
		case UNIT_ORDER_TARGET:
		case UNIT_ORDER_CLEAR_TARGET:
			return true;
		default:
			return false;
	}
}

static GAME_EVENT s_orderToEventMap[UNIT_ORDER_MAX];

GAME_EVENT Order::OrderToEvent(UNIT_ORDER_TYPE order)
{
	return s_orderToEventMap[order];
}


void Order::AssociateEventsWithOrders()
{

	const char *event_name;
	for(sint32 i = 0; i < UNIT_ORDER_MAX; i++) {
		s_orderToEventMap[i] = GEV_MAX;
	}

	for (sint32 order_index = 0; order_index < g_numOrderInfo; order_index++)
	{
		sint32 dbIndex;
		if(g_theOrderDB->GetNamedItem(g_orderInfo[order_index].m_name, dbIndex)) {
			event_name = g_theOrderDB->Get(dbIndex)->GetEventName();
			if (strlen(event_name) > 0)
				s_orderToEventMap[g_orderInfo[order_index].m_type] = g_gevManager->GetEventIndex(event_name);
			else {
				s_orderToEventMap[g_orderInfo[order_index].m_type] = GEV_MAX;
			}

		}
	}
}

CURSORINDEX Order::GetCursor(OrderRecord *order)
{
	return (CURSORINDEX)order->GetCursor();
}

CURSORINDEX Order::GetInvalidCursor(OrderRecord *order)
{
	return (CURSORINDEX)order->GetInvalidCursor();
}
