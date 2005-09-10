//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer endgame packet handling. This file looks like
//                unused or not updated accordingly.
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed old endgame database include file. (Aug 20th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "net_endgame.h"
#include "network.h"
#include "Wormhole.h"
#include "EndGame.h" // Not part of the project
#include "net_util.h"
#include "player.h"

#include "pointerlist.h"

extern Player **g_player;

//----------------------------------------------------------------------------
//
// Name       : NetEndGame::NetEndGame
//
// Description: Constructor
//
// Parameters : sint32 owner: The owner of the according endgame.
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
NetEndGame::NetEndGame(sint32 owner)
{
	m_owner = (uint8)owner;
}

//----------------------------------------------------------------------------
//
// Name       : NetEndGame::Packetize
//
// Description: Generate an application data packet to transmit.
//
// Parameters : buf         : buffer to store the message
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetEndGame::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_ENDGAME_ID);
	EndGame *eg = g_player[m_owner]->m_endGame;

	PUSHBYTE(m_owner);



	





}

//----------------------------------------------------------------------------
//
// Name       : NetEndGame::Unpacketize
//
// Description: Retrieve the data from a received application data packet.
//
// Parameters : id          : Sender identification?
//              buf         : Buffer with received message
//              size        : Length of received message (in bytes)
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetEndGame::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_ENDGAME_ID);

	PULLBYTE(m_owner);
	if(!g_player[m_owner])
		return;






	



	





}

//----------------------------------------------------------------------------
//
// Name       : NetWormhole::Packetize
//
// Description: Generate an application data packet to transmit.
//
// Parameters : buf         : buffer to store the message
//
// Globals    : -
//
// Returns    : size        : number of bytes stored in buf
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetWormhole::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_WORMHOLE_ID);

	if(!g_wormhole) {
		PUSHBYTE(0);
		return;
	} else {
		PUSHBYTE(1);
	}

	PUSHLONG(g_wormhole->m_discoverer);
	sint32 packpos = g_network.PackedPos(g_wormhole->m_pos);
	PUSHLONG(packpos);

	PUSHBYTE(g_wormhole->m_curDir);
	PUSHLONG(g_wormhole->m_horizontalMoves);
	PUSHLONG(g_wormhole->m_topY);
	PUSHLONG(g_wormhole->m_bottomY);
	PUSHLONG(g_wormhole->m_discoveredAt);

	sint32 numEntries = g_wormhole->m_entries->GetCount();
	PUSHLONG(numEntries);
	PointerList<EntryRecord>::Walker walk(g_wormhole->m_entries);
	while(walk.IsValid()) {
		PUSHLONG(walk.GetObj()->m_unit.m_id);
		PUSHLONG(walk.GetObj()->m_round);
		walk.Next();
	}
}

//----------------------------------------------------------------------------
//
// Name       : NetWormhole::Unpacketize
//
// Description: Retrieve the data from a received application data packet.
//
// Parameters : id          : Sender identification?
//              buf         : Buffer with received message
//              size        : Length of received message (in bytes)
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void NetWormhole::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_WORMHOLE_ID);

	uint8 haveWormhole;
	PULLBYTE(haveWormhole);
	if(!haveWormhole) {
		if(g_wormhole) {
			delete g_wormhole;
			g_wormhole = NULL;
		}
		return;
	}

	sint32 discoverer;
	MapPoint wpos;

	PULLLONG(discoverer);
	sint32 packpos;
	PULLLONG(packpos);
	g_network.UnpackedPos(packpos, wpos);
	

	if(!g_wormhole) {
		g_wormhole = new Wormhole(discoverer, wpos);
	}

	g_wormhole->m_pos = wpos;
	g_wormhole->m_discoverer = discoverer;

	PULLBYTETYPE(g_wormhole->m_curDir, WORLD_DIRECTION);
	PULLLONG(g_wormhole->m_horizontalMoves);
	PULLLONG(g_wormhole->m_topY);
	PULLLONG(g_wormhole->m_bottomY);
	PULLLONG(g_wormhole->m_discoveredAt);

	sint32 numEntries;
	PULLLONG(numEntries);
	Unit unit;
	sint32 round;
	sint32 i;
	for(i = 0; i < numEntries; i++) {
		PULLLONG(unit.m_id);
		PULLLONG(round);
		g_wormhole->m_entries->AddTail(new EntryRecord(unit, round));
	}
}

