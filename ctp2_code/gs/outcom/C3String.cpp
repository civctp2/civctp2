

#include "c3.h"

#include "globals.h"
#include "C3GameState.h"
#include "C3String.h"
#include "c3errors.h"
#include "StrDB.h"
#include "civarchive.h"

extern StringDB *g_theStringDB;

STDMETHODIMP C3String::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3String)) {
		*obj = (IC3String *)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3String::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3String::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

C3String::C3String()
{
	m_refCount = 0;
}

C3String::~C3String()
{
}

const MBCHAR *C3String::GetString(const MBCHAR *stringId)
{
	return g_theStringDB->GetNameStr(stringId);
}

C3String::C3String(CivArchive &archive)
{
    Serialize(archive);
}

void C3String::Serialize(CivArchive &archive)
{
    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}
