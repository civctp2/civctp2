//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Network message
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
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "network.h"
#include "net_message.h"
#include "net_packet.h"
#include "net_util.h"
#include "MessageData.h"
#include "MessagePool.h"
#include "UnitDynArr.h"
#include "net_info.h"
#include "player.h"

extern Player **g_player;

void NetMessage::Packetize(uint8 *buf, uint16 &size)
{
	Assert(FALSE);

	size = 0;
	PUSHID(k_PACKET_MESSAGE_ID);
	PUSHLONG(m_data->m_id);
	PUSHBYTE((uint8)m_data->m_owner);
	PUSHBYTE((uint8)m_data->m_sender);
	PUSHBYTE((uint8)m_data->m_msgType);
	PUSHLONG(m_data->m_timestamp);
	PUSHSTRING(m_data->m_text);
	if(!m_data->m_cityList) {
		PUSHSHORT(-1);
	} else {
		PUSHSHORT((uint16)m_data->m_cityList->Num());
		for(sint32 i = 0 ; i < m_data->m_cityList->Num(); i++) {
			PUSHLONG(uint32(m_data->m_cityList->Access(i)));
		}
	}
}

void NetMessage::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	Assert(FALSE);

	uint16 packid;
	sint32 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_MESSAGE_ID);

	Message msg;
	Message realmsg;
	PULLLONGTYPE(msg, Message);

	if(g_network.IsHost()) {
		realmsg = g_theMessagePool->ServerCreate();
		m_data = realmsg.AccessData();
	} else {
		g_network.CheckReceivedObject((uint32)msg);
		if(g_theMessagePool->IsValid(msg)) {
			m_data = g_theMessagePool->AccessMessage(msg);
		} else {
			m_data = new MessageData(msg);
		}
	}

	PULLBYTETYPE(m_data->m_owner, PLAYER_INDEX);
	PULLBYTETYPE(m_data->m_sender, PLAYER_INDEX);
	PULLBYTETYPE(m_data->m_msgType, MESSAGE_TYPE);
	PULLLONG(m_data->m_timestamp);
	PULLSTRING(m_data->m_text);

	sint16 numCities;
	PULLSHORT(numCities);

	m_data->m_cityList->Clear();

	if(numCities > 0) {
		for(sint32 i = 0 ; i < numCities; i++) {
			PULLLONGTYPE(m_data->m_cityList->Access(i), Unit);
		}
	}
	if(g_network.IsHost()) {
		if(realmsg == msg) {
			g_network.QueuePacket(id, new NetInfo(NET_INFO_CODE_ACK_OBJECT,
												  (uint32)msg));
		} else {
			g_network.QueuePacket(id, new NetInfo(NET_INFO_CODE_NAK_OBJECT,
												  (uint32)msg, (uint32)realmsg));
		}
		g_player[m_data->m_owner]->AddMessage(realmsg);
		g_network.Enqueue(m_data);
	} else if(!g_theMessagePool->IsValid(msg)) {
		g_theMessagePool->HackSetKey(((uint32)msg & k_ID_KEY_MASK)+1);
		g_theMessagePool->Insert(m_data);
		g_player[m_data->m_owner]->AddMessage(msg);
	}
}

NetInfoMessage::NetInfoMessage(NET_MSG_TYPE msg, 
							   MBCHAR *playerName, 
							   sint32 index)
{
	m_msg = msg;
	m_name = playerName;
	m_player = index;
}

void NetInfoMessage::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_NET_INFO_MESSAGE_ID);
	PUSHLONG(m_msg);
	PUSHSTRING(m_name);
	PUSHLONG(m_player);
}

void NetInfoMessage::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	sint32 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_NET_INFO_MESSAGE_ID);
	
	PULLLONGTYPE(m_msg, NET_MSG_TYPE);
	MBCHAR name[1024];
	PULLSTRING(name);
	PULLLONG(m_player);

	switch(m_msg) {
		case NET_MSG_PLAYER_JOINED:
			g_network.SendJoinedMessage(name, m_player);
			break;
		case NET_MSG_PLAYER_LEFT:
			g_network.SendLeftMessage(name, m_player);
			break;
		case NET_MSG_NEW_HOST:
			g_network.SendNewHostMessage(name, m_player);
			break;
		default:
			Assert(FALSE);
			break;
	}

	Assert(pos == size);
}

