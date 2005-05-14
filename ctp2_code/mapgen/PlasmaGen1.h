/**
 * $Id$
 */
#pragma once

#ifndef __PLASMA_GEN_1_H__
#define __PLASMA_GEN_1_H__

#include "IMapGen.h"

class PlasmaGenerator1 : public IMapGenerator
{
private:
	ULONG m_refCount;

	sint16 AverageVals(sint8 *map, sint32 x, sint32 y,
					   sint32 xstride, sint32 ystride,
					   sint32 width);
public:
	PlasmaGenerator1() { m_refCount = 0; }

	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHOD(Generate) (sint8 *map, sint32 width, sint32 height);
};

#endif
