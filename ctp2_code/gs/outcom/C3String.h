#pragma once
#ifndef __C3STRING_H__
#define __C3STRING_H__

#include "ic3String.h"
#include "civarchive.h"

class C3String : public IC3String
{
private:
	ULONG m_refCount;

public:
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	C3String();
    C3String(CivArchive &archive); 
	~C3String();

    void Serialize(CivArchive &archive); 

	STDMETHODIMP_(const MBCHAR *) GetString(const MBCHAR *stringId);
};

#endif
