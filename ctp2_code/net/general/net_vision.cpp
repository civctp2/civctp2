//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer vision packet handling.
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
// BATTLE_FLAGS
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "net_vision.h"
#include "net_util.h"
#include "Vision.h"
#include "player.h"         // g_player
#include "tiledmap.h"       // g_tiledMap
#include "radarmap.h"       // g_radarMap
#include "UnseenCell.h"
#include "TileInfo.h"
#include "Vision.h"
#include "TerrImprove.h"
#include "pointerlist.h"
#include "UnitActor.h"
#include "SpriteState.h"
#include "UnitRecord.h"

extern Player **g_player;
extern TiledMap *g_tiledMap;
extern RadarMap *g_radarMap;

//----------------------------------------------------------------------------
//
// Name       : NetVision::NetVision
//
// Description: Constructor
//
// Parameters : sint32 owner:  First database to check
//              uint16 row:    
//              uint8 numRows: 
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
NetVision::NetVision(sint32 owner, uint16 row, uint8 numRows)
{
	m_owner = (uint8)owner;
	m_row = row;
	m_numRows = numRows;
}

//----------------------------------------------------------------------------
//
// Name       : NetVision::Packetize
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
void NetVision::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_VISION_ID);
	PUSHBYTE(m_owner);
	PUSHSHORT(m_row);
	PUSHBYTE(m_numRows);

	uint8 *ptr = NULL;
	sint32 x, y;
	uint8 bitPos = 0;
	Vision *vision = g_player[m_owner]->m_vision;
	sint32 w = vision->m_width;
	sint32 bottom = m_row + m_numRows;
	if(bottom > vision->m_height)
		bottom = vision->m_height;

	for(y = m_row; y < bottom; y++) {
		bitPos = 0;
		ptr = &buf[size] + ((y - m_row) * ((w+7) / 8));
		*ptr = 0;
		for(x = 0; x < vision->m_width; x++) {
			uint16 vis = vision->m_array[x][y];
			if(vis & 0x8000) {
				*ptr |= 1 << bitPos;
			}
			bitPos++;
			if(bitPos >= 8) {
				bitPos = 0;
				ptr++;
				*ptr = 0;
			}
		}
	}
	if(bitPos != 0) {
		
		ptr++;
	}
	size = ptr - buf + 1;
}

//----------------------------------------------------------------------------
//
// Name       : NetVision::Unpacketize
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
void NetVision::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_VISION_ID);

	PULLBYTE(m_owner);
	PULLSHORT(m_row);
	PULLBYTE(m_numRows);

	uint8 *ptr = NULL;
	sint32 x, y;
	uint8 bitPos = 0;
	Vision *vision = g_player[m_owner]->m_vision;
	sint32 w = vision->m_width;
	sint32 bottom = m_row + m_numRows;
	if(bottom > vision->m_height)
		bottom = vision->m_height;

	for(y = m_row; y < bottom; y++) {
		bitPos = 0;
		ptr = &buf[pos] + ((y - m_row) * ((w+7) / 8));
		for(x = 0; x < vision->m_width; x++) {
			if(*ptr & (1 << bitPos)) {
				vision->m_array[x][y] |= 0x8000;
			} else {
				vision->m_array[x][y] &= 0x7fff;
			}
			bitPos++;
			if(bitPos >= 8) {
				bitPos = 0;
				ptr++;
			}
		}
	}
	if(bitPos != 0) {
		
		ptr++;
	}
	Assert(size == ptr - buf + 1);

}

//----------------------------------------------------------------------------
//
// Name       : NetUnseenCell::NetUnseenCell
//
// Description: Constructor
//
// Parameters : UnseenCell *ucell: The unseen cell from/to the network
//              uint8 owner:       
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
NetUnseenCell::NetUnseenCell(UnseenCell *ucell, uint8 owner)
{
	m_ucell = ucell;
	m_owner = owner;
}

#define k_BIO 1
#define k_NANO 2
#define k_CONVERTED 4
#define k_FRANCHISED 8
#define k_INJOINED 0x10
#define k_HAPPINESS 0x20
#define k_HUT 0x40

//----------------------------------------------------------------------------
//
// Name       : NetUnseenCell::Packetize
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
void NetUnseenCell::Packetize(uint8 *buf, uint16 &size)
{
	PUSHID(k_PACKET_UNSEEN_CELL_ID);
	PUSHBYTE(m_owner);

	PUSHLONG(m_ucell->m_env);
	PUSHBYTE(uint8(m_ucell->m_terrain_type));
	
	PUSHSHORT((uint16)m_ucell->m_point.x);
	PUSHSHORT((uint16)m_ucell->m_point.y);
	PUSHSHORT(m_ucell->m_move_cost);

	
	
	PUSHSHORT((uint16)m_ucell->m_flags);













	uint8 citySize;
	if(!m_ucell->m_actor) {
		citySize = 0;
	} else {
		citySize = (uint8)m_ucell->m_citySize;
	}
	PUSHBYTE(citySize);

	if(citySize > 0) {
		PUSHBYTE(m_ucell->m_bioInfectedOwner);
		PUSHBYTE(m_ucell->m_nanoInfectedOwner);
		PUSHBYTE(m_ucell->m_convertedOwner);
		PUSHBYTE(m_ucell->m_franchiseOwner);
		PUSHBYTE(m_ucell->m_injoinedOwner);
		PUSHBYTE(m_ucell->m_happinessAttackOwner);










		PUSHSHORT((uint16)m_ucell->m_cityOwner);
		PUSHSHORT((uint16)m_ucell->m_citySpriteIndex);

		PUSHSTRING(m_ucell->m_cityName);

		
		
		PUSHSHORT((uint16)m_ucell->m_actor->GetUnitDBIndex());
	}

#ifdef BATTLE_FLAGS
	PUSHSHORT(m_ucell->m_battleFlags);
#endif
	uint8 c = (uint8)m_ucell->m_improvements->GetCount();
	PUSHBYTE(c);
	PointerList<UnseenImprovementInfo>::Walker impWalk(m_ucell->m_improvements);
	for(; impWalk.IsValid(); impWalk.Next()) {
		PUSHBYTE((uint8)impWalk.GetObj()->m_type);
		PUSHBYTE((uint8)impWalk.GetObj()->m_percentComplete);
	}

	c = (uint8)m_ucell->m_installations->GetCount();
	PUSHBYTE(c);
	PointerList<UnseenInstallationInfo>::Walker instWalk(m_ucell->m_installations);
	for(; instWalk.IsValid(); instWalk.Next()) {
		PUSHBYTE((uint8)instWalk.GetObj()->m_type);
		PUSHLONG(instWalk.GetObj()->m_visibility);
	}

	PUSHBYTE(m_ucell->m_tileInfo->m_riverPiece);
	PUSHBYTE(m_ucell->m_tileInfo->m_megaInfo);
	PUSHSHORT(m_ucell->m_tileInfo->m_terrainType);
	PUSHSHORT(m_ucell->m_tileInfo->m_tileNum);
	for(c = 0; c < k_NUM_TRANSITIONS; c++) {
		PUSHBYTE(m_ucell->m_tileInfo->m_transitions[c]);
	}
}

//----------------------------------------------------------------------------
//
// Name       : NetUnseenCell::Unpacketize
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
void NetUnseenCell::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_UNSEEN_CELL_ID);
	PULLBYTE(m_owner);

	m_ucell = new UnseenCell;

	PULLLONG(m_ucell->m_env);
	PULLBYTETYPE(m_ucell->m_terrain_type, TERRAIN_TYPES);
	
	PULLSHORT(m_ucell->m_point.x);
	PULLSHORT(m_ucell->m_point.y);
	PULLSHORT(m_ucell->m_move_cost);

	
	
	
	PULLSHORT(m_ucell->m_flags);













	uint8 citySize;
	PULLBYTE(citySize);

	if(citySize > 0) {
		PULLBYTE(m_ucell->m_bioInfectedOwner);
		PULLBYTE(m_ucell->m_nanoInfectedOwner);
		PULLBYTE(m_ucell->m_convertedOwner);
		PULLBYTE(m_ucell->m_franchiseOwner);
		PULLBYTE(m_ucell->m_injoinedOwner);
		PULLBYTE(m_ucell->m_happinessAttackOwner);









		PULLSHORT(m_ucell->m_cityOwner);
		PULLSHORT(m_ucell->m_citySpriteIndex);

		PULLNEWSTRING(m_ucell->m_cityName);

		
		

		
		
		
		SpriteState *ss = new SpriteState(0);
		Unit		unitID = Unit(0);
		
		
		uint16 dbIndex;
		PULLSHORT(dbIndex);

		
		double		visionRange = g_theUnitDB->Get(dbIndex)->GetVisionRange();

		
		
		m_ucell->m_actor = new UnitActor(ss, 
											unitID, 
											(sint32)dbIndex, 
											m_ucell->m_point, 
											m_ucell->m_cityOwner, 
											TRUE, 
											visionRange,
											m_ucell->m_citySpriteIndex);
		m_ucell->m_actor->SetUnitVisibility(1 << m_owner);

		
		m_ucell->m_actor->SetSize(citySize);

		
		m_ucell->m_actor->ChangeImage(ss, dbIndex, unitID);
	}
	m_ucell->m_citySize = citySize;

#ifdef BATTLE_FLAGS
	PULLSHORT(m_ucell->m_battleFlags);
#endif
	uint8 c, i;
	PULLBYTE(c);
	for(i = 0; i < c; i++) {
		uint8 type;
		uint8 percent;
		PULLBYTE(type);
		PULLBYTE(percent);
		m_ucell->m_improvements->AddTail(
			new UnseenImprovementInfo((TERRAIN_IMPROVEMENT)type, 
									  (sint32)percent));
	}

	PULLBYTE(c);
	for(i = 0; i < c; i++) {
		uint8 type;
		uint32 vis;
		PULLBYTE(type);
		PULLLONG(vis);
		m_ucell->m_installations->AddTail(new UnseenInstallationInfo((sint32)type, vis));
	}

	m_ucell->m_tileInfo = new TileInfo;

	PULLBYTE(m_ucell->m_tileInfo->m_riverPiece);
	PULLBYTE(m_ucell->m_tileInfo->m_megaInfo);
	PULLSHORT(m_ucell->m_tileInfo->m_terrainType);
	PULLSHORT(m_ucell->m_tileInfo->m_tileNum);
	for(c = 0; c < k_NUM_TRANSITIONS; c++) {
		PULLBYTE(m_ucell->m_tileInfo->m_transitions[c]);
	}

	g_player[m_owner]->m_vision->AddUnseen(m_ucell);
}
