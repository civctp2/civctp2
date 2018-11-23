//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
// - Prevented an unnecessary call to the overloaded sqrt function to support
//   VC++ .NET compilation.
//
//----------------------------------------------------------------------------
#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#if !defined(USE_COM_REPLACEMENT)
#define INITGUID

#include "c3.h"
#endif

#include "Crater.h"
#include <stdlib.h>
#include "IC3Rand.h"

#if defined(USE_COM_REPLACEMENT)
extern "C" IMapGenerator *CoCreateMapGenerator()
{
	IMapGenerator *gen = new Crater();
	gen->AddRef();
	return gen;
}

Crater::~Crater()
{
}
#else
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
#endif

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP_(ULONG) Crater::AddRef()
#else
uint32 Crater::AddRef()
#endif
{
	return ++m_refCount;
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP_(ULONG) Crater::Release()
#else
uint32 Crater::Release()
#endif
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP Crater::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                              IC3Rand *randgen,
                              const double *settings, sint32 numSettings)
#else
void Crater::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                      IC3Rand *randgen,
                      const double *settings, sint32 numSettings)
#endif
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
				sint32 const	dist_square	= (rx * rx) + (ry * ry);
				if (dist_square <= rsq)
				{
					sint32 const	origh	= sint32(outmap[ay * outwidth + ax]);
					sint32			newh	= origh - dist_square - maxRadiusSq;
					if (newh < -127)
					{
						newh = -127;
					}
					outmap[ay * outwidth + ax]	= sint8(newh);

				}
			}
		}
	}
#if !defined(USE_COM_REPLACEMENT)
	return S_OK;
#endif
}
