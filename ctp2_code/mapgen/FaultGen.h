//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
// - corrected wrong defined name for multiple include prevention
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __FAULTGEN_H__
#define __FAULTGEN_H__ 1

#include "IMapGen.h"

class FaultGenerator : public IMapGenerator
{
private:
#if !defined(USE_COM_REPLACEMENT)
	ULONG m_refCount;
#else
	uint32 m_refCount;
#endif

public:
	FaultGenerator() { m_refCount = 0; }

#if !defined(USE_COM_REPLACEMENT)
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHOD(Generate) (sint8 *map, sint32 width, sint32 height,
	                     IC3Rand *randgen,
	                     const double *settings, sint32 numSettings);
#else
	virtual ~FaultGenerator();
	virtual uint32 AddRef();
	virtual uint32 Release();
	virtual void Generate(sint8 *map, sint32 width, sint32 height,
	                      IC3Rand *randgen, const double *settings,
	                      sint32 numSettings);
#endif
};

#endif
