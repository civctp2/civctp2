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

#if defined(ACTIVISION_ORIGINAL)	// incorrect, functionality to MapPoint
BOOL WrapPoint(const MapPoint &center, const MapPoint &opos, MapPoint &pos)
{
	pos = opos;
	sint16 sx = g_mp_size.x; 
	sint16 sy = g_mp_size.y; 
	
	if (g_theWorld->IsYwrap()) { 
		
		if (pos.x <0) pos.x = sx + pos.x; 
		else if (sx <= pos.x) pos.x = pos.x - sx; 
		
		if (pos.y < 0) {
			pos.y = sy + pos.y;
			pos.x -= (g_theWorld->GetHeight() - (2 * g_theWorld->GetWidth())) / 2;
		} else if (sy <= pos.y) {
			pos.y = pos.y - sy; 
			pos.x += (g_theWorld->GetHeight() - (2 * g_theWorld->GetWidth())) / 2;
		}
	} else { 
		
		if (pos.x <0) pos.x = sx + pos.x; 
		else if (sx <= pos.x) pos.x = pos.x - sx; 
		
		
		if (pos.y <0) { 
			pos.y = 0; 
			return FALSE;
		} else if (sy <= pos.y) { 
			pos.y = sy -1;
			return FALSE;
		} 

		if(!g_theWorld->IsXwrap()) {
			sint16 centerLeftEdge = (sx - (center.y / 2)) % sx;
			sint16 oposLeftEdge = (sx - (pos.y / 2)) % sx;
			if(center.x >= centerLeftEdge && opos.x < oposLeftEdge)
				return FALSE;

			sint16 centerRightEdge = (centerLeftEdge + sx) % sx;
			sint16 oposRightEdge = (oposLeftEdge + sx) % sx;
			if(center.x < centerRightEdge && opos.x >= oposRightEdge)
				return FALSE;
		}
	}
	
	return TRUE;
}

void CityRadiusIterator(const MapPoint &cpos, 
						CityRadiusCallback *cb)
{
	MapPoint pos = cpos;
	MapPoint wpos;
	pos.y -= 3;
	pos.x += 2;
	
	for(; pos.y < cpos.y; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}
	
	pos.x--; pos.y -= 3;
	for(; pos.y < cpos.y + 2; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}
	
	pos.x--; pos.y -= 4;
	for(; pos.y < cpos.y + 3; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}
	
	pos.x--; pos.y -= 4;
	for(; pos.y < cpos.y + 4; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}
	
	pos.x--; pos.y -= 3;
	for(; pos.y < cpos.y + 4; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}
}

void AdjacentIterator(const MapPoint &cpos, CityRadiusCallback *cb)
{
	MapPoint pos = cpos;
	MapPoint wpos;
	pos.x += 1;
	pos.y -= 2;
	for(; pos.y < cpos.y + 1; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}

	pos.x -= 1;
 	pos.y -= 2;
	for(; pos.y < cpos.y + 2; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}

	pos.x -= 1;
	pos.y -= 2;
	for(; pos.y < cpos.y + 3; pos.y++) {
		if(WrapPoint(cpos, pos, wpos))
			cb->CityRadiusFunc(wpos);
	}
}

#else	// ACTIVISION_ORIGINAL

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
#endif
