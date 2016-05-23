#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_EVENT_ARG_LIST_H__
#define __GAME_EVENT_ARG_LIST_H__

#include "GameEventTypes.h"

template <class T> class PointerList;

class GameEventArgument;
#include "ID.h"
#include "MapPoint.h"

class Unit;
class MapPoint;
class Army;
class Path;
class TerrainImprovement;
class TradeRoute;
class CivArchive;

class GameEventArgList {
public:
	GameEventArgList(const GAME_EVENT_ARGUMENT* argTypes, const void** args);
	GameEventArgList();
	GameEventArgList(CivArchive &archive);
	~GameEventArgList();
	void Serialize(CivArchive &archive);

	void Add(GameEventArgument *arg);

	GameEventArgument *GetArg(GAME_EVENT_ARGUMENT argType, sint32 index);
	sint32 GetArgCount(GAME_EVENT_ARGUMENT argType);

	BOOL GetCity(sint32 index, Unit &c);
	BOOL GetUnit(sint32 index, Unit &u);
	BOOL GetArmy(sint32 index, Army &a);

	BOOL GetInt(sint32 index, sint32 &value);
	BOOL GetPlayer(sint32 index, sint32 &player);

	BOOL GetPos(sint32 index, MapPoint &pos);

	BOOL GetPath(sint32 index, Path *&path);
	BOOL GetDirection(sint32 index, WORLD_DIRECTION &d);

	BOOL GetAdvance(sint32 index, sint32 &a);
	BOOL GetWonder(sint32 index, sint32 &w);

	BOOL GetImprovement(sint32 index, TerrainImprovement &imp);
	BOOL GetTradeRoute(sint32 index, TradeRoute &route);

private:

	PointerList<GameEventArgument> *m_argLists[GEA_End];
};

#endif
