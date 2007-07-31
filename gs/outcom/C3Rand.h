
#pragma once
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
	~C3Rand();

	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    C3Rand(CivArchive &archive); 
    void Serialize(CivArchive &archive);

	STDMETHODIMP_(sint32) Next(sint32 range);
};

#endif
