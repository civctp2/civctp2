#pragma once
#ifndef __MAPGEN_H__
#define __MAPGEN_H__

#include <OBJBASE.H>
interface IC3Rand;

typedef HRESULT (STDAPICALLTYPE *CreateMapGenerator)(IUnknown **u);


DEFINE_GUID(CLSID_MapGenerator,
			0x47b9bd03, 
			0xdf1, 
			0x11d2, 
			0x99, 0xee, 0x0, 0xe0, 0x98, 0x1, 0xd0, 0xf0);

#undef INTERFACE
#define INTERFACE IMapGenerator;

DECLARE_INTERFACE_(IMapGenerator, IUnknown)
{
	STDMETHOD(Generate) (THIS_ sint8 *map, sint32 width, sint32 height,
						 IC3Rand *randgen, 
						 const double *settings, sint32 numSettings) PURE;
};

#endif
