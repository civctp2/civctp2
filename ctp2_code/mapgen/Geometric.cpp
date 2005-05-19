/**
 * $Id$
 */
#include "ctp2_config.h"
#include "ctp2_inttypes.h"

#if !defined(USE_COM_REPLACEMENT)
#include "c3.h"
#else
#include <algorithm>

#if defined(HAVE_STRING_H)
#include <string.h>
#endif

using std::max;
#endif
#include "Geometric.h"
#include "FaultGen.h"
#include <stdlib.h>
#include "ic3Rand.h"

struct georect {
	int x, y, w, h;
};

#if defined(USE_COM_REPLACEMENT)
extern "C" IMapGenerator *CoCreateMapGenerator()
{
	IMapGenerator *gen = new Geometric();
	gen->AddRef();
	return gen;
}

Geometric::~Geometric()
{
}
#else
STDAPI CoCreateMapGenerator(IUnknown **obj)
{
	Geometric *gen = new Geometric();
	gen->AddRef();
	*obj = (IUnknown *)gen;
	return S_OK;
}

STDMETHODIMP Geometric::QueryInterface(REFIID riid, void **obj)
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
STDMETHODIMP_(ULONG) Geometric::AddRef()
#else
uint32 Geometric::AddRef()
#endif
{
	return ++m_refCount;
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP_(ULONG) Geometric::Release()
#else
uint32 Geometric::Release()
#endif
{
	if(--m_refCount)
		return m_refCount;
	delete this;
	return 0;
}

void Geometric::RaiseRectangle(sint32 sx, sint32 sy,
                               sint32 rectw, sint32 recth,
                               sint32 amount)
{
	sint32 x, y;
	for(x = sx; x < sx + rectw; x++) {
		for(y = sy; y < sy + recth; y++) {
			sint32 ax, ay;
			ax = x;
			while(ax >= m_width)
				ax -= m_width;
			ay = y;
			while(ay >= m_height)
				ay -= m_height;
			sint32 val = m_map[ay * m_width + ax];
			sint32 range = amount / 4;
			sint32 toraise;
			if(range < 1)
				toraise = 1;
			else
				toraise = m_randgen->Next(range) + (3 * amount) / 4;

			if(val + toraise > 127)
				m_map[ay * m_width + ax] = 127;
			else
				m_map[ay * m_width + ax] = sint8(val + toraise);
		}
	}
}

void Geometric::RaiseCircle(sint32 sx, sint32 sy,
                            sint32 rectw, sint32 recth,
                            sint32 amount)
{
	sint32 x, y;
	float x2;
	float halfw = (float) rectw / 2;
	float halfh = (float) recth / 2;
	sint32 cx = sx + (sint32) halfw;
	sint32 cy = sy + (sint32) halfh;
	float a_squared = halfw * halfw;
	float b_squared = halfh * halfh;
	for(x = sx; x < sx + rectw; x++) {
		x2 = (((x - cx) * (x - cx)) / a_squared);
		for(y = sy; y < sy + recth; y++) {
			
			if ((x2 + ((y-cy) * (y-cy) / b_squared)) <= 1)
			{
				sint32 ax, ay;
				ax = x;
				while(ax >= m_width)
					ax -= m_width;
				ay = y;
				while(ay >= m_height)
					ay -= m_height;
				sint32 val = m_map[ay * m_width + ax];
				sint32 range = amount / 4;
				sint32 toraise;
				if(range < 1)
					toraise = 1;
				else
					toraise = m_randgen->Next(range) + (3 * amount) / 4;

				if(val + toraise > 127)
					m_map[ay * m_width + ax] = 127;
				else
					m_map[ay * m_width + ax] = sint8(val + toraise);

			}
		}
	}
}

#define OUTM(xx, yy) outmap[yy * outwidth + xx]
#define LAND(xx,yy) (landmap[yy * outwidth + xx] != -127)

void Geometric::FixSeaFloor(sint8 *outmap, sint32 outwidth, sint32 outheight)
{
	sint8 *landmap = new sint8[outwidth * outheight];
	memcpy(landmap, outmap, outwidth * outheight);

	sint8 minContinentHeight = 127;
	sint8 maxContinentHeight = -127;
	sint32 x, y;

	
	for(y = 0; y < outheight; y++) {
		for(x = 0; x < outwidth; x++) {
			if(OUTM(x,y) < minContinentHeight &&
			   OUTM(x,y) > -127) {
				minContinentHeight = OUTM(x,y);
			}

			if(OUTM(x,y) > maxContinentHeight)
				maxContinentHeight = OUTM(x,y);
		}
	}

	
	if(maxContinentHeight < 127) {
		sint8 delta = 127 - maxContinentHeight;
		minContinentHeight += delta;
		for(y = 0; y < outheight; y++) {
			for(x = 0; x < outwidth; x++) {
				if(LAND(x,y))
					OUTM(x,y) += delta;
			}
		}
	}

	
	sint32 xl, yu;
	for(y = 0; y < outheight; y++) {
		for(x = 0; x < outwidth; x++) {
			if(!LAND(x,y)) {
				xl = x - 1;
				if(xl < 0)
					xl += outwidth;
				yu = y - 1;
				if(yu < 0)
					yu += outheight;
				sint32 h = max(OUTM(xl, y), OUTM(x, yu)) - 1;
				if(h > -126)
					h -= m_randgen->Next((h+127)/2);
				if(h > -127) {
					if(h >= minContinentHeight) 
						h = minContinentHeight - 1;
					OUTM(x, y) = (sint8)h;
				}
			}
		}
	}

	
	sint32 xr, yd;
	for(y = outheight - 1; y >= 0; y--) {
		for(x = outwidth - 1; x >= 0; x--) {
			if(!LAND(x,y)) {
				xr = x + 1;
				if(xr >= outwidth)
					xr = 0;
				yd = y + 1;
				if(yd >= outheight)
					yd = 0;
				sint32 h = max(OUTM(xr, y), OUTM(x, yd)) - 1;
				if(h > -126)
					h -= m_randgen->Next((h + 127)/2);
				if(h > -127) {
					if(h >= minContinentHeight)
						h = minContinentHeight - 1;
					if (OUTM(x, y) < h)
						OUTM(x, y) = (sint8)h;
				}
			}
		}
	}

	
	
	sint32 randrange = (minContinentHeight + 127) / 2;
	if (randrange < 1) randrange = 1;
	for(y = 0; y < outheight; y++) {
		for(x = 0; x < outwidth; x++) {
			if(!LAND(x,y) && OUTM(x,y) == -127) {
				sint32 h = m_randgen->Next(randrange) - 127;
				if(h > -127) {
					if(h >= minContinentHeight) {
						h = minContinentHeight - 1;
					}
					OUTM(x,y) = (sint8)h;
				}
			}
		}
	}

	delete [] landmap;
}

#if !defined(USE_COM_REPLACEMENT)
STDMETHODIMP Geometric::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                                 IC3Rand *randgen,
                                 const double *settings, sint32 numSettings)
#else
void Geometric::Generate(sint8 *outmap, sint32 outwidth, sint32 outheight,
                         IC3Rand *randgen,
                         const double *settings, sint32 numSettings)
#endif
{
	memset(outmap, -127, outwidth * outheight);
	m_usedmap = new sint8[outwidth * outheight];
	memset(m_usedmap, 0, outwidth * outheight);
	
	sint32 numContinents;
	sint32 maxWidth, maxHeight;
	sint32 minWidth, minHeight;
	sint32 initialContinentBumpSize;

	if(numSettings >= 11) {
		numContinents = (sint32)settings[0];
		minWidth = (sint32)settings[1];
		minHeight = (sint32)settings[2];
		maxWidth = (sint32)settings[3];
		maxHeight = (sint32)settings[4];
		initialContinentBumpSize = (sint32)settings[5];
		m_minSubContinents = (sint32)settings[6];
		m_maxSubContinents = (sint32)settings[7];
		m_subContinentSizeCoeff = settings[8];
		m_subContinentHeightCoeff = settings[9];
		m_border = sint32(settings[10]);
		if(numSettings >= 14) {
			m_nsBorder = sint32(settings[13]);
		} else {
			m_nsBorder = 5;
		}
	} else {
		numContinents = 5;
		minWidth = 10;
		minHeight = 10;
		maxWidth = outwidth / 2;
		maxHeight = outheight / 2;
		initialContinentBumpSize = 64;
		m_minSubContinents = 3;
		m_maxSubContinents = 10;
		m_subContinentSizeCoeff = 0.5;
		m_subContinentHeightCoeff = 0.75;
		m_border = 4;
		m_nsBorder = 5;
	}

	sint32 x, y;
	for(y = 0; y < m_nsBorder; y++) {
		for(x = 0; x < outwidth; x++) {
			m_usedmap[y * outwidth + x] = 1;
		}
	}
	for(y = outheight - m_nsBorder; y < outheight; y++) {
		for(x = 0; x < outwidth; x++) {
			m_usedmap[y * outwidth + x] = 1;
		}
	}
	

	m_map = outmap;
	m_width = outwidth;
	m_height = outheight;
	m_randgen = randgen;

	sint32 cont;
	for(cont = 0; cont < numContinents; cont++) {
		bool locOk = false;
		sint32 loop_guard = 100;
		sint32 contx, conty, width, height;
		while(!locOk && loop_guard-- > 0) {
			contx = randgen->Next(outwidth);
			conty = randgen->Next(outheight);

			width = randgen->Next(maxWidth - minWidth) + minWidth;
			height = randgen->Next(maxHeight - minHeight) + minHeight;

			locOk = true;

			sint32 centerx = contx + width/2;
			sint32 centery = conty + height / 2;
			while(centerx >= width)
				centerx -= width;
			while(centery >= height)
				centery -= height;

			if(m_usedmap[centery * m_width + centerx]) {
				locOk = false;
			}
		}

		sint32 ux, uy;
		for(ux = contx - m_border; ux < contx + width + m_border; ux++) {
			sint32 ax = ux;
			while(ax < 0)
				ax += m_width;
			while(ax >= m_width)
				ax -= m_width;
			for(uy = conty - m_border; uy < conty + height + m_border; uy++) {
				sint32 ay = uy;
				while(ay < 0)
					ay += m_height;
				while(ay >= m_height)
					ay -= m_height;
				m_usedmap[ay * m_width + ax] = sint8(1);
			}
		}

		RaiseCircle(contx, conty, width, height, 
					   initialContinentBumpSize);
		sint32 numSub = randgen->Next(m_maxSubContinents - m_minSubContinents) + m_minSubContinents;
		sint32 sub;
		for(sub = 0; sub < numSub; sub++) {
			GenerateSubContinent(contx, conty, width, height,
								 sint32(double(initialContinentBumpSize) * m_subContinentHeightCoeff));
		}
	}
		
	FixSeaFloor(outmap, outwidth, outheight);

	if(numSettings >= 13) {
		FaultGenerator *faults = new FaultGenerator();
		faults->AddRef();
		faults->Generate(outmap, outwidth, outheight, randgen,
						 &settings[11], 2);
		faults->Release();
	}
	
	
	delete [] m_usedmap;

#if !defined(USE_COM_REPLACEMENT)
	return S_OK;
#endif
}

void Geometric::GenerateSubContinent(sint32 contx, sint32 conty,
                                     sint32 cwidth, sint32 cheight,
                                     sint32 bumpSize)
{
	sint32 cx, cy;
	sint32 x, y;
#ifdef EDGE_SUB_CONTINENTS
	sint32 center = m_randgen->Next(cwidth * 2 + cheight * 2);

	
	if(center < cwidth) {
		cx = center + contx;
		cy = conty;
	} else if(center - cwidth < cheight) {
		cx = contx + cwidth - 1;
		cy = center - cwidth + conty;
	} else if(center - (cwidth + cheight) < cwidth) {
		cx = center - (cwidth + cheight);
		cy = conty + cheight - 1;
	} else {
		cx = contx;
		cy = center - ((cwidth * 2) + cheight) + conty;
	}

	
	sint32 w = sint32(double(cwidth) * m_subContinentSizeCoeff);
	sint32 h = sint32(double(cheight) * m_subContinentSizeCoeff);
#else
	cx = contx + m_randgen->Next(cwidth);
	cy = conty + m_randgen->Next(cheight);
	sint32 w = sint32(double(cwidth) * m_subContinentSizeCoeff);
	sint32 h = sint32(double(cheight) * m_subContinentSizeCoeff);	
#endif
	if(w < 1 || h < 1)
		return;
	x = cx - w / 2;
	if(x < 0)
		x += m_width;
	y = cy - h / 2;
	if(y < 0)
		y += m_height;

	if(w > 1 && h > 1 && bumpSize > 1) {
		RaiseCircle(x, y, w, h, bumpSize);
		sint32 numSub = m_randgen->Next(m_maxSubContinents - m_minSubContinents) + m_minSubContinents;
		sint32 sub;
		for(sub = 0; sub < numSub; sub++) {
			GenerateSubContinent(x, y, w, h, 
								 sint32(double(bumpSize) * m_subContinentHeightCoeff));
		}
	}
}

