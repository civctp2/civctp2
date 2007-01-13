








#pragma once
#ifndef _NET_ORDER_H_
#define _NET_ORDER_H_

#include "net_packet.h"
#include "Army.h"
#include "MapPoint.h"

class Path;
enum UNIT_ORDER_TYPE;
enum GAME_EVENT;

class NetOrder : public Packetizer
{
public:
	NetOrder(sint32 owner, const Army &army, UNIT_ORDER_TYPE o, Path *a_path,
			 MapPoint point, sint32 arg, GAME_EVENT event);
	NetOrder();
	virtual ~NetOrder();

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
private:
	sint32 m_owner;
	Army m_army;
	UNIT_ORDER_TYPE m_order;
	Path *m_path;
	MapPoint m_point;
	sint32 m_argument;
	GAME_EVENT m_event;
};

#endif
