#include "c3.h"
#include "MapPoint.h"
#include "CityRadius.h"
#include "XY_Coordinates.h"
#include "World.h"

extern World *g_theWorld;
extern MapPoint g_mp_size;




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
