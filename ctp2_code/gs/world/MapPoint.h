











#pragma once
#ifndef MAPPOINT_H
#define MAPPOINT_H 1

enum WORLD_DIRECTION;





#define _SMALL_MAPPOINTS		(1)

struct MapPointData { 

	sint16 x, y;

	operator==(const MapPointData &point) const {return (x==point.x && y==point.y);}
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

class CivArchive; 

#define k_MAPPOINT_VERSION_MAJOR	0								
#define k_MAPPOINT_VERSION_MINOR	0								

class MapPoint : public MapPointData { 

public:

    
	
	
	

	
	

	MapPoint() {x=y=0;}



#ifdef _SMALL_MAPPOINTS
	MapPoint(const sint32 ix, const sint32 iy) {
		x = (sint16)ix; y = (sint16)iy;
	}
#else
	MapPoint(const sint32 ix, const sint32 iy, const sint32 iz) {
		x = ix; y = iy; z = iz;
	}
#endif

	
	
	

    const BOOL operator == (const MapPoint &test_me) const; 
    const BOOL operator != (const MapPoint &test_me) const; 
    const MapPoint& operator += (const MapPoint &rhs); 

    const MapPoint& operator-= (const MapPoint &rhs) { 
        x -= rhs.x; 
        y -= rhs.y; 
        return *this;
    } 

    const double EuclidianLength () { 
        double sum = x *x + y * y; 
        return sqrt(sum); 
    }

#ifdef _SMALL_MAPPOINTS
    void Set(sint32 ix, sint32 iy) { x = (sint16)ix; y = (sint16)iy; }
#else
    void Set(sint32 ix, sint32 iy, sint32 iz) { x = ix; y = iy; z = iz; }
#endif

	
	

    sint32 GetNeighborPosition( const WORLD_DIRECTION d, MapPoint &pos) const; 
    WORLD_DIRECTION GetNeighborDirection(MapPoint neighbor)const; 
    void NormalizedSubtract(const MapPoint &dest, MapPoint &diff) const;
	sint32 NormalizedDistance(const MapPoint &dest) const;
    void OldNormalizedSubtract(const MapPoint &dest, MapPoint &diff) const;
    BOOL IsNextTo(const MapPoint &neighbor) const;

	
	void Castrate() {}
	void DelPointers() {}

    void FirstRectItt(const sint32 d, MapPoint &pos, sint32 &count); 
    BOOL EndRectItt(const sint32 d, const sint32 count);
    void NextRectItt(const sint32 d, MapPoint &pos, sint32 &count);

    void Iso2Norm(const MapPointData &pos);
    void Norm2Iso(const MapPointData &pos);     
    void Iso2Norm(const MapPoint &pos);
    void Norm2Iso(const MapPoint &pos); 
    
	void Serialize(CivArchive &archive) ;

	
	static sint32 GetSquaredDistance(const MapPoint &uPos, const MapPoint &pos);

	
	void xy2rc(const MapPoint & xy_pos, const MapPoint & map_size);
	
	
	void rc2xy(const MapPoint & rc_pos, const MapPoint & map_size );

	
	inline bool operator<(const MapPoint & rval) const 
		{return ((x< rval.x) && (y < rval.y)); }

};

uint32 MapPoint_MapPoint_GetVersion(void) ;
#endif
