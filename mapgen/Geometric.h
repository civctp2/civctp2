#pragma once

#ifndef __GEOMETRIC_H__
#define __GEOMETRIC_H__

#include "IMapGen.h"

class Geometric : public IMapGenerator
{
private:
	ULONG m_refCount;
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

	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP Generate(sint8 *map, sint32 width, sint32 height,
						 IC3Rand *randgen, 
						 const double *settings, sint32 numSettings);
};

#endif
