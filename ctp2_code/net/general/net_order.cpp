








#include "c3.h"
#include "globals.h"
#include "net_order.h"
#include "net_util.h"
#include "network.h"
#include "SelItem.h"
#include "TurnCnt.h"
#include "ArmyPool.h"

#include "Path.h"
#include "Player.h"
#include "GameEventManager.h"
#include "ArmyData.h"

extern Player **g_player;

extern TurnCount *g_turn;
extern SelectedItem *g_selected_item;

NetOrder::NetOrder(sint32 owner, const Army &army, 
				   UNIT_ORDER_TYPE o, Path *a_path,
				   MapPoint point, sint32 arg, GAME_EVENT event)
{
	m_owner = owner;
	m_army = army;
	m_order = o;
	if(a_path)
		m_path = new Path(a_path);
	else
		m_path = NULL;

	m_point = point;
	m_argument = arg;
	m_event = event;
}

NetOrder::NetOrder()
{
	m_path = NULL;
}

NetOrder::~NetOrder()
{
	if(m_path)
		delete m_path;
}

void NetOrder::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_UNIT_ORDER_ID);

	sint32 i;
#ifdef _DEBUG
	MapPoint armyPos;
	m_army.GetPos(armyPos);
	char pathstr[1024];
	pathstr[0] = 0;
	if(m_path) {
		sprintf(pathstr, "(%d,%d) (%d,%d), %d:%d:", m_path->m_start.x, m_path->m_start.y, m_path->m_current.x, m_path->m_current.y, m_path->m_next_dir, m_path->m_next);
		for(i = 0; i < m_path->m_step.Num(); i++) {
			switch(m_path->m_step[i].dir) {
				case NORTH: strcat(pathstr, ":N "); break;
				case SOUTH: strcat(pathstr, ":S "); break;
				case EAST: strcat(pathstr, ":E "); break;
				case WEST: strcat(pathstr, ":W "); break;
				case NORTHWEST: strcat(pathstr, ":NW"); break;
				case NORTHEAST: strcat(pathstr, ":NE"); break;
				case SOUTHWEST: strcat(pathstr, ":SW"); break;
				case SOUTHEAST: strcat(pathstr, ":SE"); break;
				default:
					Assert(FALSE);
					break;
			}
		}
	}

	DPRINTF(k_DBG_NET, ("Packetizing order: type %d, Army %lx, Loc(%d,%d) Army Loc:(%d,%d) Path:%s\n", 
						m_order, m_army.m_id, 
						(sint32)m_point.x, 
						(sint32)m_point.y, 
						(sint32)armyPos.x, 
						(sint32)armyPos.y, 
						pathstr));
#endif

	PUSHBYTE((uint8)m_owner);
	PUSHLONG((uint32)m_army);
	PUSHBYTE((uint8)m_order);

	PUSHSHORT((uint16)m_event);

	PUSHSHORT((uint16)m_point.x);
	PUSHSHORT((uint16)m_point.y);

	uint16 n = m_path ? (uint16)m_path->m_step.Num() : 0;
	if(m_path) 
		n |= 0x8000;
	PUSHSHORT(n);

	if(n & 0x8000) {
		PUSHSHORT((sint16)m_path->m_start.x);
		PUSHSHORT((sint16)m_path->m_start.y);
		PUSHSHORT((sint16)m_path->m_current.x);
		PUSHSHORT((sint16)m_path->m_current.y);
		PUSHLONG(m_path->m_next_dir);
		PUSHLONG(m_path->m_next);
		n &= ~(0x8000);
	}

	for(i = 0; i < n; i+=2) {
		uint8 twodirs = (uint8)m_path->m_step[i].dir << 4;
		if(i+1 < n) {
			twodirs |= (uint8)m_path->m_step[i+1].dir;
		}
		PUSHBYTE(twodirs);
	}

	PUSHLONG(m_argument);
}

void NetOrder::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;

	PULLID(packid);

	Assert(packid == k_PACKET_UNIT_ORDER_ID);

	PULLBYTE(m_owner);
	PULLLONGTYPE(m_army, Army);
	PULLBYTETYPE(m_order, UNIT_ORDER_TYPE);
	PULLSHORTTYPE(m_event, GAME_EVENT);

	if(!g_theArmyPool->IsValid(m_army)) {
		DPRINTF(k_DBG_NET, ("Received order for invalid army %lx\n", m_army.m_id));
		if(g_network.IsHost()) {
			g_network.Resync(g_network.IdToIndex(id));
			return;
		} else {
			g_network.RequestResync(RESYNC_INVALID_ARMY_ORDER);
			return;
		}
	}

	PULLSHORT(m_point.x);
	PULLSHORT(m_point.y);

	sint32 i;
	uint16 n;
	PULLSHORT(n);

	Path *path = NULL;
	if(n & 0x8000) {
		path = new Path;
		PULLSHORT(path->m_start.x);
		PULLSHORT(path->m_start.y);
		PULLSHORT(path->m_current.x);
		PULLSHORT(path->m_current.y);
		PULLLONG(path->m_next_dir);		
		PULLLONG(path->m_next);
		n &= ~(0x8000);
	}

	for(i = 0; i < n; i += 2) {
		uint8 twodirs;
		PULLBYTE(twodirs);
		path->m_step.Insert((WORLD_DIRECTION)(twodirs >> 4));
		if(i + 1 < n) {
			path->m_step.Insert((WORLD_DIRECTION)(twodirs & 0xf));
		}
	}
	PULLLONG(m_argument);

#ifdef _DEBUG
	MapPoint armyPos;
	m_army.GetPos(armyPos);
	char pathstr[1024];
	pathstr[0] = 0;
	if(path) {
		sprintf(pathstr, "(%d,%d) (%d,%d), %d:%d:", path->m_start.x, path->m_start.y, path->m_current.x, path->m_current.y, path->m_next_dir, path->m_next);
		for(i = 0; i < path->m_step.Num(); i++) {
			switch(path->m_step[i].dir) {
				case NORTH: strcat(pathstr, ":N "); break;
				case SOUTH: strcat(pathstr, ":S "); break;
				case EAST: strcat(pathstr, ":E "); break;
				case WEST: strcat(pathstr, ":W "); break;
				case NORTHWEST: strcat(pathstr, ":NW"); break;
				case NORTHEAST: strcat(pathstr, ":NE"); break;
				case SOUTHWEST: strcat(pathstr, ":SW"); break;
				case SOUTHEAST: strcat(pathstr, ":SE"); break;
				default:
					Assert(FALSE);
					break;
			}
		}
	}

	DPRINTF(k_DBG_NET, ("Unpacketized order: type %d, Army %lx, Loc(%d,%d) Army Loc(%d,%d) Path:%s\n", 
						m_order, m_army.m_id, 
						(sint32)m_point.x, 
						(sint32)m_point.y, 
						(sint32)armyPos.x,
						(sint32)armyPos.y,
						pathstr));
#endif

	bool needInput = g_gevManager->NeedUserInput();
	g_gevManager->SetNeedUserInput();
	m_army->AddOrders(m_order, path, m_point, m_argument, m_event);
	if(!needInput) {
		g_gevManager->GotUserInput();
	}

	
	
	

	g_network.GetSliceFor(m_owner);
	Assert(size == pos);
}
