

#include "c3.h"

#include "globals.h"
#include "C3GameState.h"
#include "C3ErrorReport.h"
#include "c3debug.h"
#include "c3errors.h"
#include "civarchive.h"

STDMETHODIMP C3ErrorReport::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_IC3ErrorReport)) {
		*obj = (IC3ErrorReport *)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) C3ErrorReport::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) C3ErrorReport::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

C3ErrorReport::C3ErrorReport()
{
	m_refCount = 0;
}

C3ErrorReport::~C3ErrorReport()
{
}

void C3ErrorReport::ReportDBError(const MBCHAR *stringId)
{
	c3errors_ErrorDialogFromDB("AIDLL", stringId);
}

void C3ErrorReport::ReportError(const MBCHAR *text)
{
	c3errors_ErrorDialog("AI DLL", text);
}

C3ErrorReport::C3ErrorReport(CivArchive &archive)
{
    Serialize(archive);
}

void C3ErrorReport::Serialize(CivArchive &archive)
{
    if (archive.IsStoring()) { 
        archive << m_refCount; 
    } else { 
        archive >> m_refCount; 
    } 
}


