








#pragma once
#ifndef __CITYRADIUS_H__
#define __CITYRADIUS_H__

class MapPoint;

class CityRadiusCallback
{
public:
	virtual void CityRadiusFunc(const MapPoint &pos) = 0;
};

BOOL WrapPoint(const MapPoint &start, const MapPoint &opos, MapPoint &pos);




void CityRadiusIterator(const MapPoint &cpos,
						CityRadiusCallback *cb);
void AdjacentIterator(const MapPoint &cpos, 
					  CityRadiusCallback *cb);

#endif
