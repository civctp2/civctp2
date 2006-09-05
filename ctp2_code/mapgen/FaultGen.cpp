/**
 * $Id$
 */
#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#if !defined(USE_COM_REPLACEMENT)
#define INITGUID
#else
#include "noCOMBase.h"
#endif

#if defined(WIN32)
#include "c3.h"
#endif

#include <algorithm>
#include "FaultGen.h"
#include <stdlib.h>
#include "IC3Rand.h"

#ifndef GEOMETRIC
#if !defined(USE_COM_REPLACEMENT)
STDAPI CoCreateMapGenerator(IUnknown **obj)
{
	FaultGenerator *gen = new FaultGenerator();
	gen->AddRef();
	*obj = (IUnknown *)gen;
	return S_OK;
}

#else
extern "C" IMapGenerator *CoCreateMapGenerator()
{
	IMapGenerator *gen = new FaultGenerator();
	gen->AddRef();
	return gen;
}

FaultGenerator::~FaultGenerator()
{
}
#endif
#endif

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP FaultGenerator::QueryInterface(REFIID riid, void **obj)
{
	*obj = NULL;

	if(IsEqualIID(riid, IID_IUnknown)) {
		*obj = (IUnknown *)this;
		AddRef();
		return S_OK;
	} else if(IsEqualIID(riid, CLSID_MapGenerator)) {
		*obj = (IMapGenerator*)this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) FaultGenerator::AddRef()
#else
uint32 FaultGenerator::AddRef()
#endif
{
	return ++m_refCount;
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP_(ULONG) FaultGenerator::Release()
#else
uint32 FaultGenerator::Release()
#endif
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

#if 0
void Shift(sint8 *map, sint32 x, sint32 y,
		   sint32 width, sint32 height,
		   sint32 xstep, sint32 ystep,
		   sint32 count, sint32 raiselower)
{
	sint32 s;
	for(s = 0; s < count; s++) {
		sint8 old = map[y * width + x];
		if(raiselower > 0) {
			if(old < 127) {
				map[y * width + x]++;
			}
		} else if(raiselower < 0) {
			if(old > -127) {
				map[y * width + x]--;
			}
		}
		x += xstep;
		if(x >= width) {
			x = 0;
		} else if(x < 0) {
			x = width - 1;
		}
		y += ystep;
		if(y >= height) {
			y = 0;
		} else if(y < 0) {
			y = height - 1;
		}
	}
}
#endif
void Shift(sint8 *map, sint32 sx, sint32 sy,
           sint32 width, sint32 height,
           bool   horiz, sint32 delta, double slope,
           sint32 count, sint32 raiselower)
{
	sint32 s;
	sint32 x = sx, y = sy;
	sint32 ax = x, ay = y;
	for(s = 0; s < count; s++) {
		sint8 old = map[y * width + x];
		if(raiselower > 0) {
			if(old < 127) {
				map[y * width + x]++;
			}
		} else if(raiselower < 0) {
			if(old > -127) {
				map[y * width + x]--;
			}
		}
		if(horiz) {
			ax += delta;
			x += delta;
			while(x >= width) x -= width;
			while(x < 0) x += width;
			y = sint32(sy + (double(ax - sx) * slope));
			while(y >= height) y -= height;
			while(y < 0) y += height;
		} else {
			ay += delta;
			y += delta;
			while(y >= height) y -= height;
			while(y < 0) y += height;

			x = sint32(sx + (double(ay - sy) * slope));
			while(x >= width) x -= width;
			while(x < 0) x += width;
		}
	}
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP FaultGenerator::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                                      IC3Rand *randgen,
                                      const double *settings, sint32 numSettings)
#else
void FaultGenerator::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                              IC3Rand *randgen,
                              const double *settings, sint32 numSettings)
#endif
{
	sint32 faults, f;
	if(numSettings >= 1) {
		faults = sint32(settings[0]);
	} else {
		faults = 500;
	}
	double lenCoeff;
	if(numSettings >= 2) {
		lenCoeff = settings[1];
	} else {
		lenCoeff = 1.0;
	}

    sint32 length = static_cast<sint32>(std::min(outwidth, outheight) * lenCoeff);
#define PI 3.1415926535
#define ANGLERANGE (((PI / 4.0) * 2.0) - (PI / 4.0))

	for(f = 0; f < faults; f++) {
		sint32 sx = randgen->Next(outwidth),
			sy = randgen->Next(outheight);
		sint32 x, y;
		sint32 horiz = randgen->Next(100) < 50; 
		sint32 dir = randgen->Next(100) < 50;    
		if(horiz) {
			double yslope = (double)(randgen->Next(1800) - 900) / 1000.0;
			
			x = sx;
			for(sint32 w = 0; w < length; w++) {
				y = sint32(sy + ((double)w * yslope));
				while(y >= outheight)
					y -= outheight;
				while(y < 0)
					y += outheight;

				Shift(outmap, x, y, outwidth, outheight, 
				      false, 1, yslope, length / 2, dir ? -1 : 1);
				Shift(outmap, x, y, outwidth, outheight, 
				      false, -1, 0 - yslope,  length / 2, dir ? 1 : -1);
				x++;
				if(x >= outwidth)
					x = 0;
			}
		} else {
			double xslope = (double)(randgen->Next(1800) - 900) / 1000.0;
			
			y = sy;
			for(sint32 h = 0; h < length; h++) {
				x = sint32(sx + ((double)h * xslope));
				while(x >= outwidth)
					x -= outwidth;
				while(x < 0)
					x += outwidth;

				Shift(outmap, x, y, outwidth, outheight, 
				      true, 1, xslope, length / 2, dir ? -1 : 1);
				Shift(outmap, x, y, outwidth, outheight, 
				      true, -1, 0 - xslope, length / 2, dir ? 1 : -1);
				y++;
				if(y >= outheight)
					y = 0;
			}
		}
	}
#if !defined(USE_COM_REPLACEMENT)
	return S_OK;
#endif
}
