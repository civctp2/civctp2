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
//
//
//----------------------------------------------------------------------------
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __GEOMETRIC_H__
#define __GEOMETRIC_H__ 1

#include "IMapGen.h"

class Geometric : public IMapGenerator
{
private:
#if !defined(USE_COM_REPLACEMENT)
	ULONG m_refCount;
#else
	uint32 m_refCount;
#endif
	sint8 *m_map;
	sint8 *m_usedmap;
	sint32 m_width, m_height;
	sint32 m_border;
	sint32 m_nsBorder;
	IC3Rand *m_randgen;
	sint32 m_minSubContinents;
	sint32 m_maxSubContinents;
	double m_subContinentSizeCoeff;
	double m_subContinentHeightCoeff;

	void RaiseRectangle(sint32 sx, sint32 sy,
	                    sint32 rectw, sint32 recth,
	                    sint32 amount);
	void RaiseCircle(sint32 sx, sint32 sy,
	                 sint32 rectw, sint32 recth,
	                 sint32 amount);
	void FixSeaFloor(sint8 *outmap, sint32 outwidth, sint32 outheight);
	void GenerateSubContinent(sint32 contx, sint32 conty,
	                          sint32 width, sint32 height,
	                          sint32 bumpsize);
public:
	Geometric() { m_refCount = 0; }

#if !defined(USE_COM_REPLACEMENT)
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP Generate(sint8 *map, sint32 width, sint32 height,
	                      IC3Rand *randgen,
	                      const double *settings, sint32 numSettings);
#else
	virtual ~Geometric();
	virtual uint32 AddRef();
	virtual uint32 Release();
	virtual void Generate(sint8 *map, sint32 width, sint32 height,
	                      IC3Rand *randgen,
	                      const double *settings, sint32 numSettings);
#endif
};

#endif
