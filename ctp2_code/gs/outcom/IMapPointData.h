
#pragma once
#ifndef __MAP_POINT_DATA_H__
#define __MAP_POINT_DATA_H__ 1



#define _SMALL_MAPPOINTS		(1)

struct MapPointData { 

#ifdef _SMALL_MAPPOINTS
	sint16 x, y, z;
	sint16 pad;
#else
	sint32 x, y, z;
#endif

	operator==(const MapPointData &point) const {return (x==point.x && y==point.y && z==point.z);}
		operator!=(const MapPointData &point) const {return (! operator==(point));}
}; 


#define k_NUM_CITY_TILES 20 
struct TileUtility { 
    double m_utility; 
    double m_food; 
    double m_production; 
    double m_gold; 
    double m_can_be_irrigated; 
    MapPointData m_pos; 
};


#endif __MAP_POINT_DATA_H__ 

