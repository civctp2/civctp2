#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_EVENT_ARGUMENT_H__
#define __GAME_EVENT_ARGUMENT_H__

#include "GameEventTypes.h"

#include "ID.h"
#include "MapPoint.h"

class Unit;
class MapPoint;
class Army;
class Path;
class TerrainImprovement;
class CivArchive;
class TradeRoute;

struct SimpleMapPoint {
	sint16 x, y, z;
};

class GameEventArgument {
public:
	GameEventArgument(GAME_EVENT_ARGUMENT type, va_list *vl);
	GameEventArgument(GAME_EVENT_ARGUMENT type, ...);
	GameEventArgument(CivArchive &archive);
	~GameEventArgument();
	void Serialize(CivArchive &archive);

	void Init(GAME_EVENT_ARGUMENT type, va_list *vl);

	GAME_EVENT_ARGUMENT GetType() { return m_type; }

	BOOL GetCity(Unit &c);
	BOOL GetUnit(Unit &u);
	BOOL GetArmy(Army &a);

	BOOL GetInt(sint32 &value);
	BOOL GetPlayer(sint32 &player);

	BOOL GetPos(MapPoint &pos);

	BOOL GetPath(Path *&path);
	BOOL GetDirection(WORLD_DIRECTION &d);

	BOOL GetAdvance(sint32 &a);
	BOOL GetWonder(sint32 &w);

	BOOL GetImprovement(TerrainImprovement &imp);
	BOOL GetTradeRoute(TradeRoute &route);

private:
	GAME_EVENT_ARGUMENT m_type;
	union {
		uint32 m_id;         
		sint32 m_value;  
		SimpleMapPoint m_pos;  
		void *m_ptr;     
	} m_data;
};

#endif
