
#pragma once
#ifndef __ORDER_H__
#define __ORDER_H__

class GameEventArgList;
enum GAME_EVENT;
enum CURSORINDEX;
class OrderRecord;

enum UNIT_ORDER_TYPE {
	
	UNIT_ORDER_NONE,
	UNIT_ORDER_MOVE,
	UNIT_ORDER_PATROL,
	UNIT_ORDER_CIRCULAR_PATROL,
	UNIT_ORDER_ENTRENCH,
	UNIT_ORDER_SLEEP,
	UNIT_ORDER_DETRENCH,
	UNIT_ORDER_UNLOAD,
	UNIT_ORDER_MOVE_TO,
	UNIT_ORDER_TELEPORT_TO,

	
	UNIT_ORDER_EXPEL_TO,
	UNIT_ORDER_GROUP,
	UNIT_ORDER_UNGROUP,
	UNIT_ORDER_INVESTIGATE_CITY,
	UNIT_ORDER_NULLIFY_WALLS,
	UNIT_ORDER_STEAL_TECHNOLOGY,
	UNIT_ORDER_INCITE_REVOLUTION,
	UNIT_ORDER_ASSASSINATE,
	UNIT_ORDER_INVESTIGATE_READINESS,
	UNIT_ORDER_BOMBARD,

	
	UNIT_ORDER_SUE,
	UNIT_ORDER_FRANCHISE,
	UNIT_ORDER_SUE_FRANCHISE,
	UNIT_ORDER_EXPEL,
	UNIT_ORDER_ESTABLISH_EMBASSY,
	UNIT_ORDER_THROW_PARTY,
	UNIT_ORDER_CAUSE_UNHAPPINESS,
	UNIT_ORDER_PLANT_NUKE,
	UNIT_ORDER_SLAVE_RAID,
	UNIT_ORDER_ENSLAVE_SETTLER,

	
	UNIT_ORDER_UNDERGROUND_RAILWAY,
	UNIT_ORDER_INCITE_UPRISING,
	UNIT_ORDER_BIO_INFECT,
	UNIT_ORDER_NANO_INFECT,
	UNIT_ORDER_CONVERT,
	UNIT_ORDER_REFORM,
	UNIT_ORDER_INDULGENCE,
	UNIT_ORDER_SOOTHSAY,
	UNIT_ORDER_CREATE_PARK,
	UNIT_ORDER_PILLAGE,

	
	UNIT_ORDER_INJOIN,
	UNIT_ORDER_INTERCEPT_TRADE,
	UNIT_ORDER_PARADROP_MOVE,
	UNIT_ORDER_SET_UNLOAD_MOVEMENT_POINTS,
	UNIT_ORDER_GROUP_UNIT,
	UNIT_ORDER_DISBAND,
	UNIT_ORDER_FINISH_ATTACK,
	UNIT_ORDER_UNLOAD_ONE_UNIT,
	UNIT_ORDER_BOARD_TRANSPORT,
	UNIT_ORDER_WAKE_UP,

	
	UNIT_ORDER_PILLAGE_UNCONDITIONALLY,
	UNIT_ORDER_MOVE_THEN_UNLOAD,
	UNIT_ORDER_ADVERTISE,
    UNIT_ORDER_INFORM_AI_CAPTURE_CITY, 
	UNIT_ORDER_UNLOAD_SELECTED_STACK,
	UNIT_ORDER_ADD_EVENT,
	UNIT_ORDER_SETTLE, 
	UNIT_ORDER_LAUNCH,
	UNIT_ORDER_TARGET,
	UNIT_ORDER_CLEAR_TARGET,

	
	UNIT_ORDER_PLAGUE,
	UNIT_ORDER_VICTORY_MOVE,

	UNIT_ORDER_MAX
};

enum ORDER_RESULT {
	ORDER_RESULT_ILLEGAL,
	ORDER_RESULT_FAILED,
	ORDER_RESULT_SUCCEEDED,
	ORDER_RESULT_INCOMPLETE,
	ORDER_RESULT_SUCCEEDED_INCOMPLETE,
};

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
