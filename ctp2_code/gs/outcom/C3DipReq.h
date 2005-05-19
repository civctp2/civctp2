#pragma once
#ifndef __C3DIPREQ_H__
#define __C3DIPREQ_H__

#include "ic3DipReq.h"
#include "DiplomaticRequest.h"
#include "message.h"

class CivArchive;

class C3DiplomaticRequest : public IC3DiplomaticRequest
{
private:
	ULONG m_refCount;

	DiplomaticRequest m_request;
	Message m_message;

public:
	C3DiplomaticRequest(const DiplomaticRequest &request,
						const Message &message);
	C3DiplomaticRequest(CivArchive &archive);
	~C3DiplomaticRequest();
	void Serialize(CivArchive &archive);

	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	
	STDMETHODIMP_(PLAYER_INDEX)        GetOwner();
	STDMETHODIMP_(PLAYER_INDEX)        GetRecipient();
	STDMETHODIMP_(REQUEST_TYPE)        GetRequest();

	STDMETHODIMP_(AdvanceType)         GetAdvance();
	STDMETHODIMP_(AdvanceType)         GetWantedAdvance();
	STDMETHODIMP_(uint32)              GetWantedCity();
	STDMETHODIMP_(PLAYER_INDEX)        GetThirdParty();
	STDMETHODIMP_(REQUEST_RESPONSE_TYPE) GetResponse();



	STDMETHODIMP_(uint32)              GetTarget();
	STDMETHODIMP_(sint32)              GetGold();
	
	STDMETHODIMP_(void)                Reject();
	STDMETHODIMP_(void)                Enact();
	
	STDMETHODIMP_(sint32)              GetTone();


};

#endif
