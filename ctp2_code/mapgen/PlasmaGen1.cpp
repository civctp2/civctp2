/**
 * $Id$
 */
#define INITGUID

#include "c3.h"
#include "PlasmaGen1.h"
#include <stdlib.h>


STDAPI CoCreateMapGenerator(IUnknown **obj)
{
	PlasmaGenerator1 *gen = new PlasmaGenerator1();
	gen->AddRef();
	*obj = (IUnknown *)gen;
	return S_OK;
}

STDMETHODIMP PlasmaGenerator1::QueryInterface(REFIID riid, void **obj)
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

STDMETHODIMP_(ULONG) PlasmaGenerator1::AddRef()
{
	return ++m_refCount;
}

STDMETHODIMP_(ULONG) PlasmaGenerator1::Release()
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

sint16 PlasmaGenerator1::AverageVals(sint8 *map, sint32 x, sint32 y,
									sint32 xstride, sint32 ystride,
									sint32 width)
{
	sint16 total = (map[(y - ystride) * width + (x - xstride)] +
		map[(y - ystride) * width + (x + xstride)] +
		map[(y + ystride) * width + (x - xstride)] +
		map[(y + ystride) * width + (x + xstride)]);
	return sint8(total/ 4);
}

#if 0
PlasmaGenerator1::Generate(sint16 *map, sint32 width, sint32 height)
{
	sint32 x, y;
	sint16 range = 1000;
	sint32 xstride = width/2;
	sint32 ystride = height/2;
	BOOL oddline = FALSE;

	while(xstride && ystride) {
		if(xstride < 1)
			xstride = 1;
		if(ystride < 1)
			ystride = 1;
		for(y = ystride; y < height - ystride; y += ystride) {
			oddline = !oddline;
			for(x = xstride; x < width - xstride; x += xstride) {
				if((oddline) && !x)
					x += xstride;
				map[y * width + x] = sint16(rand() % (range * 2)) - range;
				map[y * width + x] += AverageVals(map, x, y,
												  xstride, ystride, width);

			}
		}
		range >>= 1;
		xstride >>= 1;
		ystride >>= 1;
	}
	return TRUE;
}
#endif

sint8 Rand1(sint8 base, sint8 delta)
{
	return base + rand() % delta - delta/2;
}

STDMETHODIMP PlasmaGenerator1::Generate(sint8 *map, sint32 width, sint32 height)
{
	srand(time(0));
	int i,j,i2,j2,i3,j3,h,h2,delta,a,b,c,d;


	for (i=0;i<height;i++)
		for (j=0;j<width;j++)
			map[i * width + j]=0;

	for (h=width; h>1; h=h2) {
		h2=h/2;
		delta= (int)(3*(h/4));
		if(delta < 1)
			delta = 1;
		for (i=0; i<width; i+=h) {
			i2=((i+h)&(width-1));
            for (j=0; j<width; j+=h) {
                j2=((j+h)&(width-1));
                i3=i+h2; j3=j+h2;
                a=map[i*width+j]; b=map[i2*width+j];
                c=map[i*width+j2]; d=map[i2*width+j2];

                map[i3*width+j3]=Rand1((a+b+c+d)/4,delta);

                if (i==0)
					map[i3*width+j]=Rand1((a+b)/2,delta);

                if (j==0)
					map[i*width+j3]=Rand1((a+c)/2,delta);

                map[i2*width+j3]=Rand1((b+d)/2,delta);

                map[i3*width+j2]=Rand1((c+d)/2,delta);
			}
		}
	}
	return S_OK;
}
