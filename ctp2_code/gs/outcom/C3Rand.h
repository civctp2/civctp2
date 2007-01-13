#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __C3RAND_H__
#define __C3RAND_H__

#include "IC3Rand.h"

class CivArchive; 
class RandomGenerator;

class C3Rand : public IC3Rand
{
	ULONG m_refCount;
	RandomGenerator *m_rand;
	BOOL m_ownGenerator;

public:
	C3Rand(BOOL ownGenerator = FALSE);
	virtual ~C3Rand();

#ifndef USE_COM_REPLACEMENT
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
#else
	virtual uint32 AddRef();
	virtual uint32 Release();
#endif

	C3Rand(CivArchive &archive); 
	void Serialize(CivArchive &archive);

#ifndef USE_COM_REPLACEMENT
	STDMETHODIMP_(sint32) Next(sint32 range);
#else
	virtual sint32 Next(sint32 range);
#endif
};

#endif
