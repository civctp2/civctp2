//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Improved structure.
//
//----------------------------------------------------------------------------
//
/// \file   net_cell.h
/// \brief  Multiplayer cell packet handling (declarations)

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef NET_CELL_H_
#define NET_CELL_H_

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class NetCellData;
class NetCellList;
class NetCellUnitOrder;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "net_packet.h" // Packetizer
class Cell;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class NetCellData : public Packetizer
{
public:
	NetCellData(Cell * cell, sint32 x, sint32 y)
	:
		Packetizer	(),
		m_cell		(cell),
		m_x			(static_cast<uint16>(x)),
		m_y			(static_cast<uint16>(y))
	{};

	NetCellData()
	:
		Packetizer	(),
		m_cell		(NULL),
		m_x			(0),
		m_y			(0)
	{};

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	Cell * m_cell;
	uint16 m_x;
	uint16 m_y;
};

class NetCellList : public Packetizer
{
public:
	NetCellList(sint32 x, sint32 y)
	:
		Packetizer	(),
		m_x			(static_cast<uint16>(x)),
		m_y			(static_cast<uint16>(y)),
		m_cells		(0)
	{};

	NetCellList()
	:
		Packetizer	(),
		m_x			(0),
		m_y			(0),
		m_cells		(0)
	{};

	void Packetize(uint8* buf, uint16& size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	uint16 m_x;
	uint16 m_y;
	uint8  m_cells;

	friend class Network;
};

class NetCellUnitOrder : public Packetizer
{
public:
	NetCellUnitOrder(sint32 x, sint32 y)
	:
		Packetizer	(),
		m_x			(static_cast<uint16>(x)),
		m_y			(static_cast<uint16>(y))
	{};

	NetCellUnitOrder()
	:
		Packetizer	(),
		m_x			(0),
		m_y			(0)
	{};

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

private:
	uint16 m_x;
	uint16 m_y;
};

#endif
