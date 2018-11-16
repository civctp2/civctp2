#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ORDER_H__
#define __ORDER_H__

#include "ctp2_enums.h"
#include "GameEventDescription.h"
#include "cursormanager.h"
class GameEventArgList;
class OrderRecord;

#include "MapPoint.h"

struct OrderInfo {
	UNIT_ORDER_TYPE m_type;
	char *m_name;
	sint32 m_goldCost;
	sint32 m_moveCost;
	sint32 m_failSound;
	sint32 m_workSound;
	void *m_userData;
};

extern OrderInfo g_orderInfo[];
extern sint32 g_numOrderInfo;
extern sint32 g_orderInfoMap[UNIT_ORDER_MAX];

class Path;
class CivArchive;

class Order {
public:
	UNIT_ORDER_TYPE m_order;
	Path *m_path;
	sint32 m_round;
	MapPoint m_point;
	sint32 m_argument;
	sint32 m_index;

	GAME_EVENT m_eventType;
	GameEventArgList *m_gameEventArgs;

	Order(UNIT_ORDER_TYPE order, Path *path, const MapPoint &point,
		  sint32 argument);
	Order(CivArchive &archive);
	Order()
	{
		m_order = UNIT_ORDER_NONE;
		m_path = NULL;
		m_round = -1;
		m_argument = 0;
		m_index = -1;
		m_gameEventArgs = NULL;
	}

	~Order();

	void *operator new(size_t size);
	void operator delete (void *ptr);

	void Serialize(CivArchive &archive);

	static bool IsSpecialAttack(UNIT_ORDER_TYPE order);
	static GAME_EVENT OrderToEvent(UNIT_ORDER_TYPE order);
	static void AssociateEventsWithOrders();

	static CURSORINDEX GetCursor(OrderRecord *order);
	static CURSORINDEX GetInvalidCursor(OrderRecord *order);
};
#endif
