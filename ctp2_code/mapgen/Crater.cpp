
#define INITGUID

#include "c3.h"
#include "Crater.h"
#include <stdlib.h>
#include "IC3Rand.h"

STDAPI CoCreateMapGenerator(IUnknown **obj)
{
	Crater *gen = new Crater();
	gen->AddRef();
	*obj = (IUnknown *)gen;
	return S_OK;
}

STDMETHODIMP Crater::QueryInterface(REFIID riid, void **obj)
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

STDMETHODIMP_(ULONG) Crater::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) Crater::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

STDMETHODIMP Crater::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
							  IC3Rand *randgen,
							  const double *settings, sint32 numSettings)
{
	sint32 numCraters, maxRadius;
	if(numSettings >= 1) {
		numCraters = sint32(settings[0]);
	} else {
		numCraters = 50;
	}

	if(numSettings >= 2) {
		maxRadius = sint32(settings[1]);
	} else {
		maxRadius = 5;
	}

	sint32 maxRadiusSq = maxRadius * maxRadius;
	for(sint32 i = 0; i < numCraters; i++) {
		sint32 radius = randgen->Next(maxRadius) + 1;

		
		sint32 cx = randgen->Next(outwidth);
		sint32 cy = randgen->Next(outheight);

		
		sint32 x, y;

		
		sint32 ax, ay;

		
		sint32 rx, ry;

		sint32 rsq = radius * radius;

		for(x = cx - radius, rx = -radius; x <= cx + radius; x++, rx++) {
			ax = x;
			if(x < 0)
				ax += outwidth;
			if(x >= outwidth)
				ax -= outwidth;
			for(y = cy - radius, ry = -radius ; y <= cy + radius; y++, ry++) {
				ay = y;
				if(y < 0)
					ay += outheight;
				if(y >= outheight)
					ay -= outheight;
				double distance = sqrt(rx*rx + ry*ry);
				if(distance <= radius) {
					
					sint32 origh = sint32(outmap[ay * outwidth + ax]);
					sint32 newh = origh - sint32(distance * distance) - maxRadiusSq;
					if(newh < -127)
						newh = -127;
					outmap[ay * outwidth + ax] = sint8(newh);
				}
			}
		}
	}
	return S_OK;
}
