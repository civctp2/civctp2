//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Multiplayer feat packet handler.
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
// None yet.
//
//----------------------------------------------------------------------------
//
// This file is not part of the original Activision code release.
// Its current status is incomplete. Much more debugging is required to make
// it work.
//
//----------------------------------------------------------------------------

#include "c3.h"				// general settings
#include "net_feat.h"		// own declarations: consistency check

#include "FeatTracker.h"	// g_featTracker
#include "net_packet.h"		// k_PACKET_FEAT_TRACKER_ID
#include "net_util.h"		// PULL..., PUSH...

namespace
{

	unsigned char const			FIXED_BLOCK[6]	=
	{
		0x40, 0x42, 0x42, 0x42, 0x43, 0x43
	};

} // namespace

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::NetFeatTracker
//
// Description: Constructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

NetFeatTracker::NetFeatTracker()
:	Packetizer()
{
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::~NetFeatTracker
//
// Description: Destructor
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

NetFeatTracker::~NetFeatTracker()
{
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::Packetize
//
// Description: Generate an application message to transmit.
//
// Parameters : buf			: buffer to store the message
//
// Globals    : -
//
// Returns    : size		: number of bytes stored in buf
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

void NetFeatTracker::Packetize(uint8 * buf, uint16 & size)
{
	size = 0;
	PUSHID(k_PACKET_FEAT_TRACKER_ID);

	PUSHBYTE(0);

	uint8 unknown	= 0;
	PUSHBYTE(unknown);
	PUSHBYTE(unknown);

	for (size_t i = 0; i < 6; ++i)
	{
		PUSHBYTE(FIXED_BLOCK[i] + unknown);
	}

	PUSHLONG(g_featTracker->m_activeList->GetCount());
	PointerList<Feat>::Walker walk(g_featTracker->m_activeList);
	while (walk.IsValid()) 
	{
		Feat *	feat	= walk.GetObj();
		PUSHLONG(feat->GetType());
		PUSHLONG(feat->GetPlayer());
		PUSHLONG(feat->GetRound());

		walk.Next();
	}
}

//----------------------------------------------------------------------------
//
// Name       : NetFeatTracker::Unpacketize
//
// Description: Retrieve the data from a received application data packet.
//
// Parameters : id			: TODO (sender identification?)
//				buf			: buffer with received message
//				size		: length of received message (in bytes)
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : TODO: make useful code
//
//----------------------------------------------------------------------------

void NetFeatTracker::Unpacketize(uint16 id, uint8 * buf, uint16 size)
{
	sint32 pos = 0;	// required for the PULL macros

	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_FEAT_TRACKER_ID);

	// TODO: confirm or reject the completely blind guesses here

	uint8	unknownCount;
	PULLBYTE(unknownCount);
	Assert(unknownCount == 0);

	uint8	offset;
	uint8	achievedCount;
	PULLBYTE(offset);
	PULLBYTE(achievedCount);
	Assert(offset == achievedCount);

	uint8	unknownBlock[6];
	for (size_t i = 0; i < 6; ++i)
	{
		PULLBYTE(unknownBlock[i]);
		Assert(unknownBlock[i] == (FIXED_BLOCK[i] + offset));
	}

	uint32 activeCount;
	PULLLONG(activeCount);
	for (size_t j = 0; j < activeCount; ++j)
	{
		sint32	featIndex;
		sint32	player;
		sint32	turn;
		
		PULLLONG(featIndex);
		PULLLONG(player);
		PULLLONG(turn);

		g_featTracker->m_activeList->AddTail(new Feat(featIndex, player, turn));
	}

	Assert(pos == size);
}
