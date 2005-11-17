









#include "c3.h"
#include "network.h"
#include "net_agreement.h"
#include "net_util.h"
#include "net_gameobj.h"
#include "net_info.h"

#include "Agreement.h"
#include "AgreementPool.h"
#include "player.h"
#include "AgreementDynArr.h"

extern AgreementPool *g_theAgreementPool;
extern Player **g_player;

NetAgreement::NetAgreement(AgreementData *data)
{
	m_data = data;
}

void NetAgreement::PacketizeData(AgreementData *data, uint8 *buf, uint16 &size)
{
	PUSHBYTE((uint8)data->m_owner);
	PUSHBYTE((uint8)data->m_recipient);
	PUSHBYTE((uint8)data->m_thirdParty);

	Assert(AGREEMENT_TYPE_MAX < 256);
	PUSHBYTE(data->m_agreement);

	PUSHLONG(data->m_round);
	PUSHLONG(data->m_expires);
	PUSHLONG(data->m_ownerPollution);
	PUSHLONG(data->m_recipientPollution);

	PUSHLONG(uint32(data->m_targetCity));
}

void NetAgreement::UnpacketizeData(AgreementData *data,
								   uint8 *buf, sint32 &pos, uint16 size)
{
	PULLBYTETYPE(data->m_owner, PLAYER_INDEX);
	PULLBYTETYPE(data->m_recipient, PLAYER_INDEX);
	PULLBYTETYPE(data->m_thirdParty, PLAYER_INDEX);

	PULLBYTETYPE(data->m_agreement, AGREEMENT_TYPE);

	PULLLONG(data->m_round);
	PULLLONG(data->m_expires);
	PULLLONG(data->m_ownerPollution);
	PULLLONG(data->m_recipientPollution);

	PULLLONGTYPE(data->m_targetCity, Unit);
	
	sint32 goldLevel;
	PULLLONG(goldLevel);
}

void NetAgreement::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_AGREEMENT_ID);

	PUSHLONG(m_data->m_id);

	PacketizeData(m_data, buf, size);
}

void NetAgreement::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_AGREEMENT_ID);
	
	Agreement ag;

	PULLLONGTYPE(ag, Agreement);
	
	g_network.CheckReceivedObject((uint32)ag);
	if(!g_theAgreementPool->IsValid(ag)) {
		m_data = new AgreementData(ag);
	} else {
		m_data = g_theAgreementPool->AccessAgreement(ag);
	}

	UnpacketizeData(m_data, buf, pos, size);

	if(!g_theAgreementPool->IsValid(ag)) {
		g_theAgreementPool->Insert(m_data);

		g_player[m_data->m_owner]->m_agreed->Insert(ag);
		g_player[m_data->m_recipient]->m_agreed->Insert(ag);
	}
}

NetClientAgreement::NetClientAgreement(AgreementData *data)
{
	m_data = data;
}

void NetClientAgreement::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_CLIENT_AGREEMENT_ID);

	PUSHLONG(m_data->m_id);

	NetAgreement::PacketizeData(m_data, buf, size);
}

void NetClientAgreement::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	sint32 pos = 0;
	uint16 packid;
	PULLID(packid);
	Assert(packid == k_PACKET_CLIENT_AGREEMENT_ID);

	Agreement ag;
	PULLLONGTYPE(ag, Agreement);

	m_data = new AgreementData(ID());

	NetAgreement::UnpacketizeData(m_data, buf, pos, size);

	g_network.Block(g_network.IdToIndex(id));

	Agreement createdAgreement =
		g_theAgreementPool->Create(m_data->m_owner, m_data->m_recipient,
								   m_data->m_agreement);
	g_network.Unblock(g_network.IdToIndex(id));

	if(createdAgreement == ag) {
		g_network.QueuePacket(id, new NetInfo(NET_INFO_CODE_ACK_OBJECT,
													  uint32(ag)));
	} else {
		g_network.QueuePacket(id, new NetInfo(NET_INFO_CODE_NAK_OBJECT,
											  uint32(ag), uint32(createdAgreement)));
	}


	AgreementData *cdata = createdAgreement.AccessData();
	cdata->m_owner = m_data->m_owner;
	cdata->m_recipient = m_data->m_recipient;
	cdata->m_thirdParty = m_data->m_thirdParty;
	cdata->m_agreement = m_data->m_agreement;

	cdata->m_round = m_data->m_round;
	cdata->m_expires = m_data->m_expires;


	cdata->m_targetCity = m_data->m_targetCity;



	g_network.Enqueue(cdata);
	delete m_data;
}
