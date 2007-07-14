#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __NET_DIPLOMACY_H__
#define __NET_DIPLOMACY_H__

#include "net_packet.h"
#include "diplomattypes.h"

class NetDipProposal : public Packetizer
{
public:
	NetDipProposal(NewProposal &prop) { m_prop = prop; }
	NetDipProposal() {}

	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8* buf, uint16 size);

private:
	NewProposal m_prop;
};

class NetDipResponse : public Packetizer
{
  public:
	NetDipResponse(Response &resp, sint32 exec, sint32 other) { 
		m_resp = resp; 
		m_executor = (uint8)exec;
		m_otherGuy = (uint8)other;
	}
	NetDipResponse() {}
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);

  private:
	Response m_resp;
	uint8 m_executor, m_otherGuy;
};

class NetAgreementMatrix : public Packetizer
{
  public:
	NetAgreementMatrix() {}
	void Packetize(uint8 *buf, uint16 &size);
	void Unpacketize(uint16 id, uint8 *buf, uint16 size);
};
	
#endif
