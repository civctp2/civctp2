

#include "c3.h"
#include "network.h"
#include "net_diplomacy.h"
#include "net_packet.h"
#include "net_info.h"
#include "net_util.h"
#include "Player.h"
#include "SelItem.h"
#include "Diplomat.h"
#include "DiplomatTypes.h"
#include "GameEventManager.h"
#include "AgreementMatrix.h"

extern SelectedItem *g_selected_item;

extern Player **g_player;

void netdiplomacy_PacketizeArgument(DiplomacyArg &arg, uint8 *buf, uint16 &size)
{
	PUSHBYTE(arg.playerId);
	PUSHLONG(arg.cityId);
	PUSHLONG(arg.armyId);
	PUSHLONG(arg.agreementId);
	PUSHLONG(arg.advanceType);
	PUSHLONG(arg.unitType);
	PUSHLONG(arg.pollution);
	PUSHLONG(arg.gold);
	PUSHDOUBLE(arg.percent);
}

void netdiplomacy_UnpacketizeArgument(DiplomacyArg &arg, uint8 *buf, uint16 &pos)
{
	PULLBYTE(arg.playerId);
	PULLLONG(arg.cityId);
	PULLLONG(arg.armyId);
	PULLLONG(arg.agreementId);
	PULLLONG(arg.advanceType);
	PULLLONG(arg.unitType);
	PULLLONG(arg.pollution);
	PULLLONG(arg.gold);
	PULLDOUBLE(arg.percent);
}

void netdiplomacy_PacketizeProposalData(ProposalData &data, uint8 *buf, uint16 &size)
{
	PUSHBYTE(data.first_type);
	netdiplomacy_PacketizeArgument(data.first_arg, buf, size);
	PUSHBYTE(data.second_type);
	netdiplomacy_PacketizeArgument(data.second_arg, buf, size);
	PUSHBYTE(data.tone);
}

void netdiplomacy_UnpacketizeProposalData(ProposalData &data, uint8 *buf, uint16 &pos)
{
	PULLBYTETYPE(data.first_type, PROPOSAL_TYPE);
	netdiplomacy_UnpacketizeArgument(data.first_arg, buf, pos);
	PULLBYTETYPE(data.second_type, PROPOSAL_TYPE);
	netdiplomacy_UnpacketizeArgument(data.second_arg, buf, pos);
	PULLBYTETYPE(data.tone, DIPLOMATIC_TONE);
}

void NetDipProposal::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_DIP_PROPOSAL_ID);

	PUSHLONG(m_prop.id);
	PUSHSHORT(m_prop.priority);
	PUSHBYTE(m_prop.senderId);
	PUSHBYTE(m_prop.receiverId);
	netdiplomacy_PacketizeProposalData(m_prop.detail, buf, size);

}

void NetDipProposal::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_DIP_PROPOSAL_ID);

	PULLLONG(m_prop.id);
	PULLSHORT(m_prop.priority);
	PULLBYTE(m_prop.senderId);
	PULLBYTE(m_prop.receiverId);
	netdiplomacy_UnpacketizeProposalData(m_prop.detail, buf, pos);

	bool execute;
	NewProposal curProp = Diplomat::GetDiplomat(m_prop.senderId).GetMyLastNewProposal(m_prop.receiverId);
	if(curProp == Diplomat::s_badNewProposal) {
		DPRINTF(k_DBG_NET, ("Adding new proposal (%d,%d), current is bad, new id: 0x%lx\n", m_prop.senderId, m_prop.receiverId, m_prop.id));
		execute = true;
	} else if(curProp.id != m_prop.id) {
		DPRINTF(k_DBG_NET, ("Replacing proposal (%d,%d), old = 0x%lx, new=0x%lx\n", m_prop.senderId, m_prop.receiverId,
							curProp.id, m_prop.id));
		execute = true;
	} else {
		DPRINTF(k_DBG_NET, ("Proposal ID's match (%d,%d), id = 0x%lx\n", m_prop.senderId, m_prop.receiverId, m_prop.id));
		execute = false;
	}

	if(g_network.IsClient() &&
	   !g_network.IsLocalPlayer(m_prop.senderId) &&
	   !g_network.IsLocalPlayer(m_prop.receiverId)) {
		execute = false;
	}

	Diplomat::GetDiplomat(m_prop.senderId).SetMyLastNewProposal(m_prop.receiverId, m_prop);
	if(execute) {
		Diplomat::GetDiplomat(m_prop.senderId).ExecuteNewProposal(m_prop.receiverId);
		g_gevManager->AddEvent(GEV_INSERT_Tail, GEV_NewProposalReady,
							   GEA_Player, m_prop.senderId,
							   GEA_Player, m_prop.receiverId,
							   GEA_End);
	}
}

void netdiplomacy_PacketizeThreatData(ThreatData &threat, uint8 *buf, uint16 &size)
{
	PUSHLONG(threat.type);
	netdiplomacy_PacketizeArgument(threat.arg, buf, size);
}

void netdiplomacy_UnpacketizeThreatData(ThreatData &threat, uint8 *buf, uint16 &pos)
{
	PULLLONGTYPE(threat.type, THREAT_TYPE);
	netdiplomacy_UnpacketizeArgument(threat.arg, buf, pos);
}

void NetDipResponse::Packetize(uint8 *buf, uint16 &size)
{
	size = 0;
	PUSHID(k_PACKET_DIP_RESPONSE_ID);

	PUSHLONG(m_resp.id);
	PUSHBYTE(m_executor);
	PUSHBYTE(m_otherGuy);
	PUSHSHORT(m_resp.priority);
	PUSHLONG(m_resp.senderId);
	PUSHLONG(m_resp.receiverId);
	PUSHLONG(m_resp.type);
	netdiplomacy_PacketizeProposalData(m_resp.counter, buf, size);
	netdiplomacy_PacketizeThreatData(m_resp.threat, buf, size);	
}

void NetDipResponse::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_DIP_RESPONSE_ID);

	PULLLONG(m_resp.id);
	PULLBYTE(m_executor);
	PULLBYTE(m_otherGuy);
	PULLSHORT(m_resp.priority);
	PULLLONG(m_resp.senderId);
	PULLLONG(m_resp.receiverId);
	PULLLONGTYPE(m_resp.type, RESPONSE_TYPE);
	netdiplomacy_UnpacketizeProposalData(m_resp.counter, buf, pos);
	netdiplomacy_UnpacketizeThreatData(m_resp.threat, buf, pos);

	bool execute;
	Response curResp = Diplomat::GetDiplomat(m_resp.receiverId).GetMyLastResponse(m_resp.senderId);
	if(curResp == Diplomat::s_badResponse) {
		DPRINTF(k_DBG_NET, ("Adding new response (%d,%d), id: 0x%lx\n", m_resp.receiverId, m_resp.senderId, m_resp.id));
		execute = true;
	} else if(curResp.id != m_resp.id) {
		DPRINTF(k_DBG_NET, ("Replacing response (%d,%d), old id: 0x%lx, new id: 0x%lx\n",
							m_resp.receiverId, m_resp.senderId, curResp.id, m_resp.id));
		execute = true;
	} else {
		DPRINTF(k_DBG_NET, ("Response IDs match (%d,%d), ID: 0x%lx\n", m_resp.receiverId, m_resp.senderId,
							m_resp.id));
		execute = false;
	}

	if(g_network.IsClient() && 
	   !g_network.IsLocalPlayer(m_resp.senderId) &&
	   !g_network.IsLocalPlayer(m_resp.receiverId)) {
		execute = false;
	}

	Diplomat::GetDiplomat(m_executor).SetMyLastResponse(m_executor == m_resp.receiverId ? m_resp.senderId : m_resp.receiverId, m_resp);
	if(execute) {
		g_network.Block(m_executor);
		g_gevManager->Pause();
		Diplomat::GetDiplomat(m_executor).ExecuteResponse(m_resp);
		g_gevManager->Resume();
		g_network.Unblock(m_executor);











	}
	
}

void NetAgreementMatrix::Packetize(uint8 *buf, uint16 &size)
{
	sint32 p1, p2;
	sint32 prop;
	size = 0;

	PUSHID(k_PACKET_DIP_AGREEMENT_MATRIX_ID);

	PUSHSHORT(AgreementMatrix::s_agreements.GetMaxPlayers());

	for(p1 = 0; p1 < k_MAX_PLAYERS; p1++) {
		if(!g_player[p1])
			continue;
		for(p2 = 0; p2 < k_MAX_PLAYERS; p2++) {
			if(!g_player[p2])
				continue;
			if(p2 == p1)
				continue;
			for(prop = sint32(PROPOSAL_NONE) + 1; prop < sint32(PROPOSAL_MAX); prop++) {
				ai::Agreement ag = AgreementMatrix::s_agreements.GetAgreement(p1, p2, (PROPOSAL_TYPE)prop);
				if(ag != AgreementMatrix::s_badAgreement) {
					PUSHBYTE(p1);
					PUSHBYTE(p2);
					PUSHBYTE(prop);
					
					PUSHLONG(ag.id);
					PUSHBYTE(ag.senderId);
					PUSHBYTE(ag.receiverId);
					PUSHSHORT(ag.start);
					PUSHSHORT(ag.end);
					netdiplomacy_PacketizeProposalData(ag.proposal, buf, size);
					PUSHLONG(ag.explainStrId);
					PUSHLONG(ag.newsStrId);
				}
			}
		}
	}
	
	PUSHBYTE(0xff);
}

void NetAgreementMatrix::Unpacketize(uint16 id, uint8 *buf, uint16 size)
{
	uint16 packid;
	uint16 pos = 0;
	PULLID(packid);
	Assert(packid == k_PACKET_DIP_AGREEMENT_MATRIX_ID);

	sint32 p1, p2, prop;
	sint16 maxPlayers;
	PULLSHORT(maxPlayers);
	if(maxPlayers != AgreementMatrix::s_agreements.GetMaxPlayers()) {
		AgreementMatrix::s_agreements.Resize(maxPlayers);
	}

	do {
		PULLBYTE(p1);
		if(p1 != 0xff) {
			PULLBYTE(p2);
			PULLBYTE(prop);

			ai::Agreement ag;
			PULLLONG(ag.id);
			PULLBYTE(ag.senderId);
			PULLBYTE(ag.receiverId);
			PULLSHORT(ag.start);
			PULLSHORT(ag.end);
			netdiplomacy_UnpacketizeProposalData(ag.proposal, buf, pos);
			PULLLONG(ag.explainStrId);
			PULLLONG(ag.newsStrId);

			sint32 agIndex = ((p1 * AgreementMatrix::s_agreements.GetMaxPlayers() * PROPOSAL_MAX) +
							  (p2 * PROPOSAL_MAX) +
							  prop);
			AgreementMatrix::s_agreements.SetAgreementFast(agIndex, ag);
		}
	} while(p1 != 0xff);
}

	
