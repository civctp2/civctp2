//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C/C++ header
// Description  : Game event argument
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
#ifndef __GAME_EVENT_ARGUMENT_H__
#define __GAME_EVENT_ARGUMENT_H__

#include "GameEventTypes.h"
#include "GameEvent.h"

#include "ID.h"
#include "MapPoint.h"

class Unit;
class MapPoint;
class Army;
class Path;
class TerrainImprovement;
class CivArchive;
class TradeRoute;

struct SimpleMapPoint // And we have to redefine it here?
{
	sint16 x, y, z;
};

class GameEventArgument
{
public:
	GameEventArgument(GAME_EVENT_ARGUMENT type, va_list *vl, bool isAlwaysValid = false);
	GameEventArgument(GAME_EVENT_ARGUMENT type, ...);
	GameEventArgument(CivArchive &archive);
	~GameEventArgument();
	void Serialize(CivArchive &archive);

	void Init(GAME_EVENT_ARGUMENT type, va_list *vl,  bool isAlwaysValid = false);

	GAME_EVENT_ARGUMENT GetType() const { return m_type; }

	bool GetCity(Unit &c) const;
	bool GetUnit(Unit &u) const;
	bool GetArmy(Army &a) const;

	bool GetInt(sint32 &value) const;
	bool GetPlayer(sint32 &player) const;

	bool GetPos(MapPoint &pos) const;

	bool GetPath(Path *&path) const;
	bool GetDirection(WORLD_DIRECTION &d) const;

	bool GetAdvance(sint32 &a) const;
	bool GetWonder(sint32 &w) const;

	bool GetImprovement(TerrainImprovement &imp) const;
	bool GetTradeRoute(TradeRoute &route) const;

	bool IsValid() const;
	void NotifyArgIsInvalid(GAME_EVENT type, sint32 argIndex, GameEvent* event) const;

private:
	GAME_EVENT_ARGUMENT m_type;
	union {
		uint32 m_id;
		sint32 m_value;
		SimpleMapPoint m_pos;
		void *m_ptr;
	} m_data;

	bool m_IsAlwaysValid;
};

#endif
