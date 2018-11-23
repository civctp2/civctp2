#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3ERRORREPORT_H__
#define __C3ERRORREPORT_H__

#include "ic3ErrorReport.h"

class CivArchive;

class C3ErrorReport : public IC3ErrorReport
{
private:
	ULONG m_refCount;

public:
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	C3ErrorReport();
    C3ErrorReport(CivArchive &archive);
	~C3ErrorReport();

    void Serialize(CivArchive &archive);

	STDMETHODIMP_(void) ReportDBError(const MBCHAR *stringId);
	STDMETHODIMP_(void) ReportError(const MBCHAR *text);
};

#endif
