//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
#include "player.h"              // g_player
#include "AdvanceRecord.h"       // g_theAdvanceDB
#include "WonderRecord.h"        // g_theWonderDB
#include "profileDB.h"           // g_theProfileDB
#include "SlicEngine.h"
#include "SlicObject.h"
#include "SlicSegment.h"
#include "SlicFrame.h"
#include "GameEventManager.h"    // g_gevManager

GameEventArgument::GameEventArgument(GAME_EVENT_ARGUMENT type, va_list *vl, bool isAlwaysValid)
{
	Init(type, vl, isAlwaysValid);
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

void GameEventArgument::Init(GAME_EVENT_ARGUMENT type, va_list *vl, bool isAlwaysValid)
{
	m_IsAlwaysValid = isAlwaysValid;
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
			Assert(false);
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

bool GameEventArgument::GetCity(Unit &c) const
{
	if(m_type != GEA_City)
		return false;

	c.m_id = m_data.m_id;
	return true;
}

bool GameEventArgument::GetUnit(Unit &u) const
{
	if(m_type != GEA_Unit)
		return false;

	u.m_id = m_data.m_id;
	return true;
}

bool GameEventArgument::GetArmy(Army &a) const
{
	if(m_type != GEA_Army)
		return false;

	a.m_id = m_data.m_id;
	return true;
}

bool GameEventArgument::GetInt(sint32 &value) const
{
	switch(m_type) {
		case GEA_Gold:
		case GEA_Int:
			value = m_data.m_value;
			return true;
		default:
			return false;
	}
}

bool GameEventArgument::GetPos(MapPoint &pos) const
{
	if(m_type != GEA_MapPoint)
		return false;

	pos.x = m_data.m_pos.x;
	pos.y = m_data.m_pos.y;

	return true;
}

bool GameEventArgument::GetPath(Path *&path) const
{
	if(m_type != GEA_Path)
		return false;

	path = new Path((Path *)m_data.m_ptr);
	return true;
}

bool GameEventArgument::GetPlayer(sint32 &player) const
{
	if(m_type != GEA_Player)
		return false;

	player = m_data.m_value;

	if(player < 0 || player >= k_MAX_PLAYERS)
		return false;

	if(!g_player[player])
		return false;

	return true;
}

bool GameEventArgument::GetDirection(WORLD_DIRECTION &d) const
{
	if(m_type != GEA_Direction)
		return false;

	d = (WORLD_DIRECTION)m_data.m_value;
	return true;
}

bool GameEventArgument::GetAdvance(sint32 &a) const
{
	if(m_type != GEA_Advance)
		return false;

	a = m_data.m_value;
	return true;
}


bool GameEventArgument::GetWonder(sint32 &w) const
{
	if(m_type != GEA_Wonder)
		return false;

	w = m_data.m_value;
	return true;
}

bool GameEventArgument::GetImprovement(TerrainImprovement &imp) const
{
	if(m_type != GEA_Improvement)
		return false;

	imp.m_id = m_data.m_id;
	return true;
}

bool GameEventArgument::GetTradeRoute(TradeRoute &route) const
{
	if(m_type != GEA_TradeRoute)
		return false;

	route.m_id = m_data.m_id;
	return true;
}

bool GameEventArgument::IsValid() const
{
	if(m_IsAlwaysValid)
	{
		return true;
	}

	switch(m_type)
	{
		case GEA_Army:
		{
			Army army(m_data.m_id);
			return army.IsValid();
		}
		case GEA_Unit:
		case GEA_City:
		{
			Unit unit(m_data.m_id);
			return unit.IsValid();
		}
		case GEA_Gold:
		case GEA_Int:
			return true; // All values should be valid
		case GEA_Path:
		//	Path* path = static_cast<Path*>(m_ptr);
		//	return path->IsValid(); // Does not exist for now lets assume that all paths are valid
			return true;
		case GEA_MapPoint:
		{
			MapPoint pos(m_data.m_pos.x, m_data.m_pos.y);
			return pos.IsValid();
		}
		case GEA_Player:
		{
			sint32 player = m_data.m_value;
			return player == -1 || (player >= 0 && player < k_MAX_PLAYERS && g_player[player]);
		}
		case GEA_Direction:
		{
			sint32 direction = m_data.m_value;
			return direction >= NORTH && direction < NOWHERE;
		}
		case GEA_Wonder:
		{
			sint32 wonder = m_data.m_value;
			return wonder >= 0 && wonder < g_theWonderDB->NumRecords();
		}
		case GEA_Advance:
		{
			sint32 advance = m_data.m_value;
			return advance >= 0 && advance < g_theAdvanceDB->NumRecords();
		}
		case GEA_Improvement:
		{
			TerrainImprovement imp(m_data.m_id);
			return imp.IsValid();
		}
		case GEA_TradeRoute:
		{
			TradeRoute route(m_data.m_id);
			return route.IsValid();
		}
		default:
			Assert(false);
	}

	return false;
}

void GameEventArgument::NotifyArgIsInvalid(GAME_EVENT type, sint32 argIndex, GameEvent* event) const
{
	DPRINTF(k_DBG_GAMESTATE, ("Missing object id %lx\n", (uint32)m_data.m_id));

	if(g_theProfileDB && g_theProfileDB->IsDebugSlicEvents())
	{
		if(event != NULL
		&& event->GetContextName() != NULL)
		{
			char buf[1024];
			sprintf(buf, "Parameter #%i of type %s of event %s is invalid.\nThe event was called during execution of event %s.\nIt was called from object %s at line %i in file:\n%s\nPossible Explanation: Data became invlid during internal or slic code executation.",
			             argIndex,
			             g_gevManager->ArgToName(m_type),
			             g_gevManager->GetEventName(type),
			             g_gevManager->GetEventName(event->AddedDuring()),
			             event->GetContextName(),
			             event->GetLine(),
			             event->GetFile());

			c3errors_ErrorDialog("Slic Event Error", buf);
		}
		else if(g_slicEngine->GetContext())
		{
			char buf[1024];
			sprintf(buf, "Parameter #%i of type %s of event %s is invalid, the argument is invalid at event call.\nThe event was called during execution of event %s\nThe event was called from object %s at line %i in file:\n%s\nThe argument was already invalid at event call time.",
			             argIndex,
			             g_gevManager->ArgToName(m_type),
			             g_gevManager->GetEventName(type),
			             g_gevManager->GetEventName(g_gevManager->GetProcessingEvent()),
			             g_slicEngine->GetContext()->GetFrame()->GetSlicSegment()->GetName(),
			             g_slicEngine->GetContext()->GetFrame()->GetCurrentLine(),
			             g_slicEngine->GetContext()->GetFrame()->GetSlicSegment()->GetFilename());

			c3errors_ErrorDialog("Slic Event Error", buf);
		}
		else
		{
			char buf[1024];

			if(event != NULL)
			{
				sprintf(buf, "Parameter #%i of type %s of event %s is invalid.\nThe event was added during the event %s.\nIt was called from the executable and is a serious problem that needs to be fixed if it was not caused by slic interference.\nPossible reason for the problem: The data became invalid between event call and event execution.",
				             argIndex,
				             g_gevManager->ArgToName(m_type),
				             g_gevManager->GetEventName(type),
				             g_gevManager->GetEventName(event->AddedDuring()));
			}
			else
			{
				sprintf(buf, "Parameter #%i of type %s of event %s is invalid.The event was added during the event %s.\nIt was called from the executable and is a serious problem that needs to be fixed.\nThe data was already invalid at event call time.",
				             argIndex,
				             g_gevManager->ArgToName(m_type),
				             g_gevManager->GetEventName(type),
				             g_gevManager->GetEventName(g_gevManager->GetProcessingEvent()));
			}

			c3errors_ErrorDialog("Slic Event Source Error", buf);
		}
	}
}