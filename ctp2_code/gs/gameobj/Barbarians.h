#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __BARBARIANS_H__
#define __BARBARIANS_H__



class MapPoint;

typedef sint32 PLAYER_INDEX;

class Barbarians {
public:
	static sint32 ChooseUnitType();
	static BOOL AddBarbarians(const MapPoint &point, PLAYER_INDEX meat,
							  BOOL fromGoodyHut);
	static sint32 ChooseSeaUnitType();
	static BOOL AddPirates(const MapPoint &point, PLAYER_INDEX meat,
							  BOOL fromGoodyHut);
	static void BeginYear();
};
	
#endif
