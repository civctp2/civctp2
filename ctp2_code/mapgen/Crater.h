#pragma once

#ifndef __CRATER_H__
#define __CRATER_H__

#include "IMapGen.h"

class Crater : public IMapGenerator
{
private:
	ULONG m_refCount;

public:
	Crater() { m_refCount = 0; }

	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP Generate(sint8 *map, sint32 width, sint32 height,
						 IC3Rand *randgen, 
						 const double *settings, sint32 numSettings);
};

#endif
