#pragma once

#ifndef __IC3TerrainB_H__
#define __IC3TerrainDB_H__ 1

#include <OBJBASE.H>

DEFINE_GUID(CLSID_IC3TerrainDB,
    0x7b481060,
    0x011f,
    0x11d2,
    0x85, 0xa7, 0x00, 0x60, 0x08, 0x3a, 0x40, 0x82
);


#undef INTERFACE
#define INTERFACE IC3TerrainDB

DECLARE_INTERFACE_(IC3TerrainDB, IUnknown)
{

    STDMETHOD_ (sint32, GetGoodGoldValue) (THIS_
        sint32 type_good) PURE;

	STDMETHOD_ (sint32, GetNumTerrainTypes)(THIS_) PURE ;
    STDMETHOD_ (sint32, GetNumTypeGoods) (THIS_) PURE;
	STDMETHOD_ (double, GetMoveCost) (THIS_ const sint8 terrain_type) PURE;
};

#endif __IC3TerrainDB_H__
