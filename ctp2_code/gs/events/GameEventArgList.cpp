

#include "c3.h"
#include "GameEventArgList.h"
#include "GameEventArgument.h"
#include "pointerlist.h"
#include "Unit.h"
#include "Army.h"
#include "MapPoint.h"

#include "UnitPool.h"
#include "ArmyPool.h"
#include "TerrImprove.h"
#include "TerrImprovePool.h"
#include "TradeRoute.h"
#include "TradePool.h"

#include "civarchive.h"

GameEventArgList::GameEventArgList(va_list *vl)
{
	GAME_EVENT_ARGUMENT arg;
	for(arg = GEA_Null; arg < GEA_End; arg = (GAME_EVENT_ARGUMENT)(sint32(arg) + 1)) {
		m_argLists[arg] = NULL;
	}

	while(1) {
		GAME_EVENT_ARGUMENT arg = va_arg(*vl, GAME_EVENT_ARGUMENT);
		Assert(arg > GEA_Null);
		Assert(arg <= GEA_End);
		if(arg <= GEA_Null || arg > GEA_End)
			break;

		if(arg == GEA_End) {
			break;
		}

		if(!m_argLists[arg]) {
			m_argLists[arg] = new PointerList<GameEventArgument>;
		}
		m_argLists[arg]->AddTail(new GameEventArgument(arg, vl));
	}	
}

GameEventArgList::GameEventArgList()
{
	GAME_EVENT_ARGUMENT arg;
	for(arg = GEA_Null; arg < GEA_End; arg = (GAME_EVENT_ARGUMENT)(sint32(arg) + 1)) {
		m_argLists[arg] = NULL;
	}
}

GameEventArgList::GameEventArgList(CivArchive &archive)
{
	Serialize(archive);
}

GameEventArgList::~GameEventArgList()
{
	GAME_EVENT_ARGUMENT arg;
	for(arg = GEA_Null; arg < GEA_End; arg = (GAME_EVENT_ARGUMENT)(sint32(arg) + 1)) {
		if(m_argLists[arg]) {
			m_argLists[arg]->DeleteAll();
			delete m_argLists[arg];
			m_argLists[arg] = NULL;
		}
	}
}

void GameEventArgList::Serialize(CivArchive &archive)
{
	uint8 numArgs;
	if(archive.IsStoring()) {
		GAME_EVENT_ARGUMENT arg;
		for(arg = GEA_Null; arg < GEA_End; arg = (GAME_EVENT_ARGUMENT)(sint32(arg) + 1)) {
			if(!m_argLists[arg]) {
				numArgs = 0;
			} else {
				numArgs = (uint8)m_argLists[arg]->GetCount();
			}

			archive << numArgs;

			if(numArgs > 0) {
				PointerList<GameEventArgument>::Walker walk(m_argLists[arg]);
				while(walk.IsValid()) {
					walk.GetObj()->Serialize(archive);
					walk.Next();
				}
			}
		}
	} else {
		GAME_EVENT_ARGUMENT arg;
		for(arg = GEA_Null; arg < GEA_End; arg = (GAME_EVENT_ARGUMENT)(sint32(arg) + 1)) {
			archive >> numArgs;
			if(numArgs <= 0) {
				m_argLists[arg] = NULL;
			} else {
				m_argLists[arg] = new PointerList<GameEventArgument>;
				sint32 i;
				for(i = 0; i < numArgs; i++) {
					m_argLists[arg]->AddTail(new GameEventArgument(archive));
				}
			}
		}
	}				  
}

void GameEventArgList::Add(GameEventArgument *arg)
{
	Assert(arg->GetType() > GEA_Null);
	Assert(arg->GetType() < GEA_End);

	if(!m_argLists[arg->GetType()]) {
		m_argLists[arg->GetType()] = new PointerList<GameEventArgument>;
	}
	m_argLists[arg->GetType()]->AddTail(arg);
}



GameEventArgument *GameEventArgList::GetArg(GAME_EVENT_ARGUMENT argType, 
											sint32 index)
{
	Assert(argType > GEA_Null);
	Assert(argType < GEA_End);

	if(argType <= GEA_Null || argType >= GEA_End)
		return NULL;

	if(!m_argLists[argType])
		return NULL;
	 
	PointerList<GameEventArgument>::Walker walk(m_argLists[argType]);
	sint32 i;
	for(i = 0; i < index && walk.IsValid(); i++) {
		walk.Next();
	}
	if(i == index && walk.IsValid())
		return walk.GetObj();

	return NULL;
}

sint32 GameEventArgList::GetArgCount(GAME_EVENT_ARGUMENT argType)
{
	Assert(argType > GEA_Null);
	Assert(argType < GEA_End);

	if(argType <= GEA_Null || argType >= GEA_End)
		return 0;

	if(!m_argLists[argType])
		return 0;

	return m_argLists[argType]->GetCount();
}


BOOL GameEventArgList::GetCity(sint32 index, Unit &c)
{
	GameEventArgument *arg;

	if(!(arg = GetArg(GEA_City, index)))
		return FALSE;

	if(!(arg->GetCity(c)) || !g_theUnitPool->IsValid(c)) {
		c.m_id = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL GameEventArgList::GetUnit(sint32 index, Unit &u)
{
	GameEventArgument *arg;

	if(!(arg = GetArg(GEA_Unit, index)))
		return FALSE;

	if(!(arg->GetUnit(u)) || !g_theUnitPool->IsValid(u)) {
		u.m_id = 0;
		return FALSE;
	}
	return TRUE;
}

BOOL GameEventArgList::GetArmy(sint32 index, Army &a)
{
	GameEventArgument *arg;
	a.m_id = 0;
	if(!(arg = GetArg(GEA_Army, index)))
		return FALSE;

	if(!(arg->GetArmy(a)) || !g_theArmyPool->IsValid(a)) {
		return FALSE;
	}
	return TRUE;
}


BOOL GameEventArgList::GetInt(sint32 index, sint32 &value)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_Int, index)))
		return FALSE;

	if(!(res = arg->GetInt(value)))
		value = 0;
	return res;
}

BOOL GameEventArgList::GetPlayer(sint32 index, sint32 &player)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_Player, index)))
		return FALSE;

	if(!(res = arg->GetPlayer(player)))
		player = -1;
	return res;
}


BOOL GameEventArgList::GetPos(sint32 index, MapPoint &pos)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_MapPoint, index)))
		return FALSE;

	if(!(res = arg->GetPos(pos)))
		pos.Set(-1,-1);
	return res;
}


BOOL GameEventArgList::GetPath(sint32 index, Path *&path)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_Path, index)))
		return FALSE;

	if(!(res = arg->GetPath(path)))
		path = NULL;
	return res;
}

BOOL GameEventArgList::GetDirection(sint32 index, WORLD_DIRECTION &d)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_Direction, index)))
		return FALSE;

	if(!(res = arg->GetDirection(d)))
		d = (WORLD_DIRECTION)-1;
	return res;
}

BOOL GameEventArgList::GetAdvance(sint32 index, sint32 &a)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_Advance, index)))
		return FALSE;

	if(!(res = arg->GetAdvance(a)))
		a = -1;
	return res;
}

BOOL GameEventArgList::GetWonder(sint32 index, sint32 &w)
{
	GameEventArgument *arg;
	BOOL res;
	if(!(arg = GetArg(GEA_Wonder, index)))
		return FALSE;

	if(!(res = arg->GetWonder(w)))
		w = -1;
	return res;
}

BOOL GameEventArgList::GetImprovement(sint32 index, TerrainImprovement &imp)
{
	GameEventArgument *arg;
	if(!(arg = GetArg(GEA_Improvement, index)))
		return FALSE;

	if(!(arg->GetImprovement(imp)) || !g_theTerrainImprovementPool->IsValid(imp.m_id)) {
		imp.m_id = 0;
		return FALSE;
	}

	return TRUE;
}

BOOL GameEventArgList::GetTradeRoute(sint32 index, TradeRoute &route)
{
	GameEventArgument *arg;
	if(!(arg = GetArg(GEA_TradeRoute, index)))
		return FALSE;
	if(!(arg->GetTradeRoute(route)) || !g_theTradePool->IsValid(route.m_id)) {
		route.m_id = 0;
		return FALSE;
	}

	return TRUE;
}
