
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __CITYLAYER_H__
#define __CITYLAYER_H__

class MapPoint;
class aui_Surface;
template <class T> class DynamicArray;
class MapPoint;
enum WORLD_DIRECTION;

void DrawTradeRouteSegment(aui_Surface *surf, MapPoint &pos, WORLD_DIRECTION dir,
							uint16 route, uint16 color);
void DrawReversedTradeRouteSegment(aui_Surface *surf, MapPoint &pos, WORLD_DIRECTION dir,
							uint16 route, uint16 color);
void DrawTradeRoute(aui_Surface *pSurface, DynamicArray<MapPoint> *pRoute, uint16 route, uint16 outline);

#endif 
