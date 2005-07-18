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
// USE_COM_REPLACEMENT
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#if !defined(USE_COM_REPLACEMENT)
#define INITGUID

#include "c3.h"
#else
#include "noCOMBase.h"
#endif

#include "PlasmaGen2.h"
#include <stdlib.h>
#include "IC3Rand.h"

#if defined(USE_COM_REPLACEMENT)
extern "C" IMapGenerator *CoCreateMapGenerator()
{
	IMapGenerator *gen = new PlasmaGenerator2();
	gen->AddRef();
	return gen;
}

PlasmaGenerator2::~PlasmaGenerator2()
{
}
#else
STDAPI CoCreateMapGenerator(IUnknown **obj)
{
	PlasmaGenerator2 *gen = new PlasmaGenerator2();
	gen->AddRef();
	*obj = (IUnknown *)gen;
	return S_OK;
}

STDMETHODIMP PlasmaGenerator2::QueryInterface(REFIID riid, void **obj)
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
STDMETHODIMP_(ULONG) PlasmaGenerator2::AddRef()
#else
uint32 PlasmaGenerator2::AddRef()
#endif
{
	return ++m_refCount;
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP_(ULONG) PlasmaGenerator2::Release()
#else
uint32 PlasmaGenerator2::Release()
#endif
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

sint8 Rand1(IC3Rand *randgen, sint16 base, sint16 delta)
{
	sint16 res = (base + static_cast<sint16>(randgen->Next(delta)) - delta/2);
	if(res > 127)
		res = 127;
	if(res < -128)
		res = -128;
	return static_cast<sint8>(res);
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP PlasmaGenerator2::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                                        IC3Rand *randgen,
                                        const double *settings, sint32 numSettings)
#else
void PlasmaGenerator2::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                                IC3Rand *randgen,
                                const double *settings, sint32 numSettings)
#endif
{
	double roughness;
	if(numSettings >= 1)
		roughness = settings[0];
	else
		roughness = 0.75;

	int i,j,i2,j2,i3,j3,h,delta,a,b,c,d,w;
	int width, height;

	for(i = 1; i < 9 ; i++) {
		if((1 << i) >= outwidth)
			break;
	}
	width = 1 << i;
	for(i = 1; i < 9; i++) {
		if((1 << i) >= outheight)
			break;
	}
	height = 1 << i;
	sint8 *map = new sint8 [width * height];

	
	
	for (i=0;i<height;i++) 
		for (j=0;j<width;j++) 
			map[i * width + j]=0;

	sint32 bigside = (width > height) ? width : height;

	h = height;
	w = width;
	delta = bigside;
		
	while(bigside > 1) {
		h = h / 2;
		w = w / 2;
		if(h < 2)
			h = 2;
		if(w < 2)
			w = 2;

		bigside = bigside / 2;
		for(i = 0; i < height; i += h) {
			for(j = 0; j < width; j += w) {

				i2 = (i + h) % height;
				i3 = (i + h/2) % height;
				j2 = (j + w) % width;
				j3 = (j + w/2) % width;
				a = map[i * width + j];
				b = map[i2 * width + j];
				c = map[i * width + j2];
				d = map[i2 * width + j2];
				map[i3 * width + j3] = Rand1(randgen, (a+b+c+d)/4, delta);
				if(i == 0)
					map[i3 * width + j] = Rand1(randgen, (a+b)/2, delta);
				if(j == 0)
					map[i * width + j3] = Rand1(randgen, (a+c)/2, delta);

				map[i2*width+j3]=Rand1(randgen, (b+d)/2,delta);

				map[i3*width+j2]=Rand1(randgen, (c+d)/2,delta);
			}
		}
		delta = int(double(delta) * roughness);
		if(delta < 1)
			delta = 1;
	}

	for(i = 0 ; i < outheight ; i++) {
		for(j = 0; j < outwidth; j++) {
			outmap[i * outwidth + j] = map[((height * i)/outheight) * width + ((width * j)/outwidth)];
		}
	}

	delete [] map;
#if !defined(USE_COM_REPLACEMENT)
	return S_OK;
#endif
}
