













#pragma once
#ifndef __IC3DIPREQ_H__
#define __IC3DIPREQ_H__


DEFINE_GUID(CLSID_IC3DiplomaticRequest,
			0x6e3807a0, 
			0x310c, 
			0x11d2, 0xbd, 0x9b, 0x0, 0x60, 0x8, 0x27, 0x65, 0x9f);
#undef INTERFACE
#define INTERFACE IC3DiplomaticRequest

#include "DiplomaticTypes.h"

typedef sint32 AdvanceType;
typedef sint32 PLAYER_INDEX;

DECLARE_INTERFACE_(IC3DiplomaticRequest, IUnknown)
{
	STDMETHOD_(PLAYER_INDEX,        GetOwner) (THIS_) PURE;
	STDMETHOD_(PLAYER_INDEX,        GetRecipient) (THIS_) PURE;
	STDMETHOD_(REQUEST_TYPE,        GetRequest) (THIS_) PURE;
	STDMETHOD_(AdvanceType,         GetAdvance) (THIS_) PURE;
	STDMETHOD_(AdvanceType,         GetWantedAdvance) (THIS_) PURE;
	STDMETHOD_(uint32,              GetWantedCity) (THIS_) PURE;
	STDMETHOD_(PLAYER_INDEX,        GetThirdParty) (THIS_) PURE;
	STDMETHOD_(REQUEST_RESPONSE_TYPE, GetResponse) (THIS_) PURE;
	STDMETHOD_(uint32,              GetTarget) (THIS_) PURE;
	STDMETHOD_(sint32,              GetGold) (THIS_) PURE;
	
	STDMETHOD_(void,                Reject) (THIS_) PURE;
	STDMETHOD_(void,                Enact) (THIS_) PURE;

	STDMETHOD_(sint32,              GetTone) (THIS_) PURE;

   
};




struct C3AIDiplomaticRequest
{
	PLAYER_INDEX m_recipient;
	PLAYER_INDEX m_thirdParty;
	REQUEST_TYPE m_request;

	
	sint32 m_tone;     
	sint32 m_strength; 
	AdvanceType m_advance;
	AdvanceType m_reciprocalAdvance;
	sint32 m_gold;

	uint32 m_targetCity;
	uint32 m_reciprocalCity;
};

#endif
