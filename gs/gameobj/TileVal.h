

#pragma once
#ifndef TILE_VALUE_H
#define TILE_VALUE_H 1

#include "MapPoint.h"

struct TileValue

{
    sint32 production;												
    sint32 food;													
	sint32 resource;												
	sint32 gold ;													
    double value;
    MapPoint pos;
};

#endif
