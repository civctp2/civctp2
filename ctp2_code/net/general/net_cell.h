#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_CELL_H_
#define _NET_CELL_H_

#include "net_packet.h"

class NetCellData : public Packetizer
{
public:
	NetCellData(Cell* cell, sint32 x, sint32 y) {
		m_cell = cell;
		m_x = (uint16)x;
		m_y = (uint16)y;
	}
	NetCellData() {};

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);
	Cell* m_cell;
	uint16 m_x, m_y;
};

class NetCellList : public Packetizer
{
public:
	NetCellList(sint32 x, sint32 y)
	{
		m_x = (uint16)x;
		m_y = (uint16)y;
		m_cells = 0;
	}

	NetCellList() : m_cells(0) {};

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);
	uint16 m_x, m_y;
	uint8 m_cells;
};

class NetCellUnitOrder : public Packetizer
{
public:
	NetCellUnitOrder(sint32 x, sint32 y);
	NetCellUnitOrder() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
	uint16 m_x, m_y;
};

#else
class NetCellData;
#endif
