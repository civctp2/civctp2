/**
 * $Id$
 */
#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif

#ifndef __PLASMA_GEN_1_H__
#define __PLASMA_GEN_1_H__

#include "IMapGen.h"

class PlasmaGenerator2 : public IMapGenerator
{
private:
#if !defined(USE_COM_REPLACEMENT)
	ULONG m_refCount;
#else
	uint32 m_refCount;
#endif

	sint16 AverageVals(sint8 *map, sint32 x, sint32 y,
					   sint32 xstride, sint32 ystride,
					   sint32 width);
public:
	PlasmaGenerator2() { m_refCount = 0; }

#if !defined(USE_COM_REPLACEMENT)	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHOD(Generate) (sint8 *map, sint32 width, sint32 height,
	                     IC3Rand *randgen, 
	                     const double *settings, sint32 numSettings);
#else
	virtual ~PlasmaGenerator2();
	virtual uint32 AddRef();
	virtual uint32 Release();
	virtual void Generate(sint8 *map, sint32 width, sint32 height,
	                      IC3Rand *randgen,
	                      const double *settings, sint32 numSettings);
#endif
};

#endif
