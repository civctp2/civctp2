//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : City radius handling
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Corrected map wrapping.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "MapPoint.h"
#include "CityRadius.h"
#include "XY_Coordinates.h"
#include "World.h"

extern World *g_theWorld;
extern MapPoint g_mp_size;


#include "directions.h"	// NOWHERE

//----------------------------------------------------------------------------
//
// Name       : CityRadiusIterator
//
// Description: Perform an action on all tiles in the radius of a city.
//
// Parameters : cpos	: the location of the city
//              cb		: the action (callback) to perform
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The action is performed on the location itself as well.
//
//----------------------------------------------------------------------------

void CityRadiusIterator(const MapPoint &cpos, 
						CityRadiusCallback *cb)
{
	// Visit the outer ring, skipping the N, E, S, and W points
	OrthogonalPoint	testXY(cpos);
	testXY.Move(NORTH, 2);	// north
	for (int se = 0; se < 3; ++se)
	{
		testXY.Move(SOUTHEAST);
		if (testXY.IsValid())
		{
			cb->CityRadiusFunc(testXY.GetRC());
		}
	}
	testXY.Move(SOUTHEAST);	// east
	for (int sw = 0; sw < 3; ++sw)
	{
		testXY.Move(SOUTHWEST);
		if (testXY.IsValid())
		{
			cb->CityRadiusFunc(testXY.GetRC());
		}
	}
	testXY.Move(SOUTHWEST);	//  south
	for (int nw = 0; nw < 3; ++nw)
	{
		testXY.Move(NORTHWEST);
		if (testXY.IsValid())
		{
			cb->CityRadiusFunc(testXY.GetRC());
		}
	}
	testXY.Move(NORTHWEST);	// west
	for (int ne = 0; ne < 3; ++ne)
	{
		testXY.Move(NORTHEAST);
		if (testXY.IsValid())
		{
			cb->CityRadiusFunc(testXY.GetRC());
		}
	}

	// Visit the inner ring
	AdjacentIterator(cpos, cb);
}

//----------------------------------------------------------------------------
//
// Name       : AdjacentIterator
//
// Description: Perform an action on all adjacent tiles of a location.
//
// Parameters : cpos	: the location
//              cb		: the action (callback) to perform
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The action is performed on the location itself as well.
//
//----------------------------------------------------------------------------

void AdjacentIterator(const MapPoint &cpos, CityRadiusCallback *cb)
{
	for (int dir = 0; dir <= NOWHERE; ++dir)
	{
		OrthogonalPoint	testXY(cpos);
		testXY.Move(WORLD_DIRECTION(dir));
		if (testXY.IsValid())
		{
			cb->CityRadiusFunc(testXY.GetRC());
		}
	}
}
