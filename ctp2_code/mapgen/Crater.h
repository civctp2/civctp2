/**
 * $Id$
 */
#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#ifndef __CRATER_H__
#define __CRATER_H__

#include "IMapGen.h"

class Crater : public IMapGenerator
{
private:
#if defined(USE_COM_REPLACEMENT)
	uint32 m_refCount;
#else
	ULONG m_refCount;
#endif

public:
	Crater() { m_refCount = 0; }

#if !defined(USE_COM_REPLACEMENT)	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP Generate(sint8 *map, sint32 width, sint32 height,
	                      IC3Rand *randgen, const double *settings,
                              sint32 numSettings);
#else
	virtual ~Crater();
	virtual uint32 AddRef();
	virtual uint32 Release();
	virtual void Generate(sint8 *map, sint32 width, sint32 height,
	                      IC3Rand *randgen, const double *settings,
	                      sint32 numSettings);
#endif
};

#endif
