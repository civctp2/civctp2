//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Road drawing.
// Id           : $Id:$
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Roads now use the TileSetIndex from the TerrainImprovement database. (28-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __TILEDRAWROAD_H__
#define __TILEDRAWROAD_H__

#include "directions.h"

#define k_BIT_SOUTH			(1 << SOUTH    )
#define k_BIT_SOUTHWEST		(1 << SOUTHWEST)
#define k_BIT_WEST			(1 << WEST     )
#define k_BIT_NORTHWEST		(1 << NORTHWEST)
#define k_BIT_NORTH			(1 << NORTH    )
#define k_BIT_NORTHEAST		(1 << NORTHEAST)
#define k_BIT_EAST			(1 << EAST     )
#define k_BIT_SOUTHEAST		(1 << SOUTHEAST)

#define k_T_SOUTH			(k_BIT_WEST      | k_BIT_SOUTH     | k_BIT_EAST     )
#define k_T_SOUTHWEST		(k_BIT_NORTHWEST | k_BIT_SOUTHWEST | k_BIT_SOUTHEAST)
#define k_T_WEST			(k_BIT_NORTH     | k_BIT_WEST      | k_BIT_SOUTH    )
#define k_T_NORTHWEST		(k_BIT_NORTHEAST | k_BIT_NORTHWEST | k_BIT_SOUTHWEST)
#define k_T_NORTH			(k_BIT_WEST      | k_BIT_NORTH     | k_BIT_EAST     )
#define k_T_NORTHEAST		(k_BIT_NORTHWEST | k_BIT_NORTHEAST | k_BIT_SOUTHEAST)
#define k_T_EAST			(k_BIT_NORTH     | k_BIT_EAST      | k_BIT_SOUTH    )
#define k_T_SOUTHEAST		(k_BIT_NORTHEAST | k_BIT_SOUTHEAST | k_BIT_SOUTHWEST)

#define k_T_INTERSECT		3
#define k_X_INTERSECT		4

enum ROAD 
{
	ROAD_DEFAULT,
	ROAD_SOUTH = 0,
	ROAD_SOUTHWEST,
	ROAD_WEST,
	ROAD_NORTHWEST,
	ROAD_NORTH,
	ROAD_NORTHEAST,
	ROAD_EAST,
	ROAD_SOUTHEAST,
	ROAD_T_SOUTH,
	ROAD_T_SOUTHWEST,
	ROAD_T_WEST,
	ROAD_T_NORTHWEST,
	ROAD_T_NORTH,
	ROAD_T_NORTHEAST,
	ROAD_T_EAST,
	ROAD_T_SOUTHEAST,
	ROAD_X_DIAGONAL,
	ROAD_X_STRAIGHT,
	ROAD_MAGLEVPOST
};

#endif

