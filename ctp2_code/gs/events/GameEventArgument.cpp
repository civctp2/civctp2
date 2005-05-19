

#include "c3.h"
#include "GameEventArgument.h"

#include "Unit.h"
#include "MapPoint.h"
#include "Army.h"
#include "Path.h"
#include "player.h"
#include "TerrImprove.h"
#include "civarchive.h"
#include "TradeRoute.h"

GameEventArgument::GameEventArgument(GAME_EVENT_ARGUMENT type, va_list *vl)
{
	Init(type, vl);
}

GameEventArgument::GameEventArgument(GAME_EVENT_ARGUMENT type, ...)
{
	va_list vl;
	va_start(vl, type);
	Init(type, &vl);
	va_end(vl);
}

GameEventArgument::GameEventArgument(CivArchive &archive)
{
	Serialize(archive);
}

void GameEventArgument::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.PutUINT32(m_type);
		switch(m_type) {
			case GEA_Path:
				((Path *)(m_data.m_ptr))->Serialize(archive);
				break;
			default:
				archive.Store((uint8*)&m_data, sizeof(m_data));
				break;
		}
	} else {
		m_type = (GAME_EVENT_ARGUMENT)archive.GetUINT32();
		switch(m_type) {
			case GEA_Path:
				m_data.m_ptr = new Path;
				((Path *)(m_data.m_ptr))->Serialize(archive);
				break;
			default:
				archive.Load((uint8*)&m_data, sizeof(m_data));
				break;
		}
	}
}

void GameEventArgument::Init(GAME_EVENT_ARGUMENT type, va_list *vl)
{
	m_type = type;
	Assert(type >= (GAME_EVENT_ARGUMENT)0);
	Assert(type < GEA_End);
	if(type < (GAME_EVENT_ARGUMENT)0 || type >= GEA_End)
		return;

	
	static Unit unit;
	static Army army;
	static MapPoint pos;
	static TerrainImprovement imp;
	static TradeRoute route;

	sint32 value;
	Path *path;

	switch(m_type) {
		case GEA_Army:
			army = va_arg(*vl, Army);
			m_data.m_id = army.m_id;
			break;
		case GEA_Unit:
			unit = va_arg(*vl, Unit);
			m_data.m_id = unit.m_id;
			break;
		case GEA_City:
			unit = va_arg(*vl, Unit);
			m_data.m_id = unit.m_id;
			break;
		case GEA_Gold:
			value = va_arg(*vl, sint32);
			m_data.m_value = value;
			break;
		case GEA_Path:
			path = va_arg(*vl, Path *);
			m_data.m_ptr = (void *)path;
			break;
		case GEA_MapPoint:
			pos = va_arg(*vl, MapPoint);
			m_data.m_pos.x = pos.x;
			m_data.m_pos.y = pos.y;

			break;

		case GEA_Player:
			value = va_arg(*vl, sint32);
			m_data.m_value = value;
			break;

		case GEA_Int:
			value = va_arg(*vl, sint32);
			m_data.m_value = value;
			break;
		case GEA_Direction:
			value = va_arg(*vl, WORLD_DIRECTION);
			m_data.m_value = value;
			break;

		case GEA_Wonder:
			value = va_arg(*vl, sint32);
			m_data.m_value = value;
			break;
		case GEA_Advance:
			value = va_arg(*vl, sint32);
			m_data.m_value = value;
			break;
		case GEA_Improvement:
			imp = va_arg(*vl, TerrainImprovement);
			m_data.m_id = imp.m_id;
			break;
		case GEA_TradeRoute:
			route = va_arg(*vl, TradeRoute);
			m_data.m_id = route.m_id;
			break;

		default:
			Assert(FALSE);
	}
}

GameEventArgument::~GameEventArgument()
{
	Path *path;

	switch(m_type) {
		case GEA_Path:
			path = (Path *)m_data.m_ptr;
			delete path;
			break;
		default:
			break;
	}
}

BOOL GameEventArgument::GetCity(Unit &c)
{
	if(m_type != GEA_City)
		return FALSE;
	c.m_id = m_data.m_id;
	return TRUE;
}

BOOL GameEventArgument::GetUnit(Unit &u)
{
	if(m_type != GEA_Unit)
		return FALSE;

	u.m_id = m_data.m_id;
	return TRUE;
}

BOOL GameEventArgument::GetArmy(Army &a)
{
	if(m_type != GEA_Army)
		return FALSE;

	a.m_id = m_data.m_id;
	return TRUE;
}

BOOL GameEventArgument::GetInt(sint32 &value)
{
	switch(m_type) {
		case GEA_Gold:
		case GEA_Int:
			value = m_data.m_value;
			return TRUE;
		default:
			return FALSE;
	}
}

BOOL GameEventArgument::GetPos(MapPoint &pos)
{
	if(m_type != GEA_MapPoint)
		return FALSE;

	pos.x = m_data.m_pos.x;
	pos.y = m_data.m_pos.y;

	return TRUE;
}

BOOL GameEventArgument::GetPath(Path *&path)
{
	if(m_type != GEA_Path)
		return FALSE;

	path = new Path((Path *)m_data.m_ptr);
	return TRUE;
}

BOOL GameEventArgument::GetPlayer(sint32 &player)
{
	if(m_type != GEA_Player)
		return FALSE;

	player = m_data.m_value;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return FALSE;

	if(!g_player[player])
		return FALSE;

	return TRUE;
}

BOOL GameEventArgument::GetDirection(WORLD_DIRECTION &d)
{
	if(m_type != GEA_Direction)
		return FALSE;

	d = (WORLD_DIRECTION)m_data.m_value;
	return TRUE;
}

BOOL GameEventArgument::GetAdvance(sint32 &a)
{
	if(m_type != GEA_Advance)
		return FALSE;

	a = m_data.m_value;
	return TRUE;
}


BOOL GameEventArgument::GetWonder(sint32 &w)
{
	if(m_type != GEA_Wonder)
		return FALSE;

	w = m_data.m_value;
	return TRUE;
}

BOOL GameEventArgument::GetImprovement(TerrainImprovement &imp)
{
	if(m_type != GEA_Improvement)
		return FALSE;

	imp.m_id = m_data.m_id;
	return TRUE;
}

BOOL GameEventArgument::GetTradeRoute(TradeRoute &route)
{
	if(m_type != GEA_TradeRoute)
		return FALSE;

	route.m_id = m_data.m_id;
	return TRUE;
}
