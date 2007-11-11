//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
// Description  : Game event argument list
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
// HAVE_PRAGMA_ONCE
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Improved slic event debugging. (7-Nov-2007 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GAME_EVENT_ARG_LIST_H__
#define __GAME_EVENT_ARG_LIST_H__

#include "GameEventTypes.h"
#include "GameEvent.h"

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

class GameEventArgList
{
public:
	GameEventArgList(va_list * vl = NULL, GAME_EVENT eventType = GEV_MAX);
	GameEventArgList(CivArchive &archive);
	~GameEventArgList();
	void Serialize(CivArchive &archive);

	void Add(GameEventArgument *arg);

	GameEventArgument *GetArg(GAME_EVENT_ARGUMENT argType, sint32 index) const;
	sint32 GetArgCount(GAME_EVENT_ARGUMENT argType) const;

	bool GetCity(sint32 index, Unit &c) const;
	bool GetUnit(sint32 index, Unit &u) const;
	bool GetArmy(sint32 index, Army &a) const;

	bool GetInt(sint32 index, sint32 &value) const;
	bool GetPlayer(sint32 index, sint32 &player) const;

	bool GetPos(sint32 index, MapPoint &pos) const;

	bool GetPath(sint32 index, Path *&path) const;
	bool GetDirection(sint32 index, WORLD_DIRECTION &d) const;
	
	bool GetAdvance(sint32 index, sint32 &a) const;
	bool GetWonder(sint32 index, sint32 &w) const;

	bool GetImprovement(sint32 index, TerrainImprovement &imp) const;
	bool GetTradeRoute(sint32 index, TradeRoute &route) const;

	bool TestArgsOfType(GAME_EVENT type, GAME_EVENT_ARGUMENT argType, GameEvent* event = NULL) const;
	bool TestArgs(GAME_EVENT type, GameEvent* event = NULL) const;

private:

	PointerList<GameEventArgument> *m_argLists[GEA_End];
};

#endif
