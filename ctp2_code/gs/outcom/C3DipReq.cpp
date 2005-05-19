

#include "c3.h"
#include "C3DipReq.h"
#include "civarchive.h"
#include "globals.h"

#include "player.h"
extern Player **g_player;


C3DiplomaticRequest::C3DiplomaticRequest(const DiplomaticRequest &request,
										 const Message &message)
{
	m_refCount = 0;
	m_request = request;
	m_message = message;
}

C3DiplomaticRequest::C3DiplomaticRequest(CivArchive &archive)
{
	m_refCount = 0;
	Serialize(archive);
}

C3DiplomaticRequest::~C3DiplomaticRequest()
{
}

void C3DiplomaticRequest::Serialize(CivArchive &archive)
{
	m_request.Serialize(archive);
	m_message.Serialize(archive);

	if(archive.IsStoring()) {
		archive << m_refCount;
	} else {
		archive >> m_refCount;
		Assert(m_refCount == 1);
		m_refCount = 1;
	}
}


STDMETHODIMP C3DiplomaticRequest::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;
	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3DiplomaticRequest)) {
		*obj = (IC3DiplomaticRequest *)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3DiplomaticRequest::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3DiplomaticRequest::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}


STDMETHODIMP_(PLAYER_INDEX)        C3DiplomaticRequest::GetOwner()
{
	return m_request.GetOwner();
}

STDMETHODIMP_(PLAYER_INDEX)        C3DiplomaticRequest::GetRecipient()
{
	return m_request.GetRecipient();
}

STDMETHODIMP_(REQUEST_TYPE)        C3DiplomaticRequest::GetRequest()
{
	return m_request.GetRequest();
}







STDMETHODIMP_(AdvanceType)         C3DiplomaticRequest::GetAdvance()
{
	return m_request.GetAdvance();
}

STDMETHODIMP_(AdvanceType)         C3DiplomaticRequest::GetWantedAdvance()
{
	return m_request.GetWantedAdvance();
}

STDMETHODIMP_(uint32)              C3DiplomaticRequest::GetWantedCity()
{
	return g_player[m_request.GetWantedCity().GetOwner()]->GetCityId(m_request.GetWantedCity());
}

STDMETHODIMP_(PLAYER_INDEX)        C3DiplomaticRequest::GetThirdParty()
{
	return m_request.GetThirdParty();
}

STDMETHODIMP_(REQUEST_RESPONSE_TYPE) C3DiplomaticRequest::GetResponse()
{
	return m_request.GetResponse();
}



















STDMETHODIMP_(uint32)              C3DiplomaticRequest::GetTarget()
{
	return g_player[m_request.GetTarget().GetOwner()]->GetCityId(m_request.GetTarget());
}

STDMETHODIMP_(sint32)              C3DiplomaticRequest::GetGold()
{
	return m_request.GetGold().GetLevel();
}


STDMETHODIMP_(void) C3DiplomaticRequest::Reject()
{
	if(m_message != Message(0)) {
		m_message.Reject();
		m_request = DiplomaticRequest(0);
		m_message = Message(0);

	}
}

STDMETHODIMP_(void) C3DiplomaticRequest::Enact()
{
	if(m_message != Message(0)) {
		m_message.Accept();
		m_request = DiplomaticRequest(0);
		m_message = Message(0);

	}
}

STDMETHODIMP_(sint32) C3DiplomaticRequest::GetTone()
{
	return m_request.GetTone();
}


