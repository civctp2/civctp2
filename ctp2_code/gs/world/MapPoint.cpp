//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Map point handling
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
// - Added OrthogonalPoint to facilitate wrap computations.
//
//----------------------------------------------------------------------------


#include "c3.h"

#include "MapPoint.h"
#include "CivArchive.h"

#include "XY_Coordinates.h"
#include "World.h"
#include "directions.h"

#include "c3math.h"

extern World *g_theWorld; 


MapPoint g_mp_size;

#if 0

MapPoint::MapPoint() 
{ 
   x = y= 0;
}

MapPoint::MapPoint(const sint32 ix, const sint32 iy)

{
   x = ix; 
   y = iy;
}


MapPoint::MapPoint (const MapPoint &copyMe)

{
 	memcpy(this, &copyMe, sizeof(MapPoint));

}



MapPoint & MapPoint::operator= (const MapPoint &copyMe)

{
	memcpy(this, &copyMe, sizeof(MapPoint));

	return *this; 
}
#endif

const MapPoint& MapPoint::operator += (const MapPoint &rhs) 

{
    x += rhs.x; 
    y += rhs.y; 

    return *this; 
} 

void MapPoint::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
		{
		archive<<x ;
		archive<<y ;

		}
	else
		{
		archive>>x ;
		archive>>y ;
		}


}

const BOOL MapPoint::operator== (const MapPoint &test_me) const

{
    return ((x == test_me.x) && (y == test_me.y));
}

const BOOL MapPoint::operator!= (const MapPoint &test_me) const 

{
    return ((x != test_me.x) || (y != test_me.y));
}



MapPoint g_dir_offset[NOWHERE] = { 

   MapPoint(1, -2),    
   MapPoint(1, -1),    
   MapPoint(1,  0),     
   MapPoint(0, -1),    
   MapPoint(0,  1),      
   MapPoint(-1, 0),    
   MapPoint(-1, 1),    
   MapPoint(-1, 2),     
};   


































































































































































































































sint32 MapPoint::GetNeighborPosition( const WORLD_DIRECTION d, MapPoint &pos) const
{
	Assert(0 <= d); 
	Assert(d < NOWHERE); 
	sint16 sx = g_mp_size.x;
	sint16 sy = g_mp_size.y;

	Assert((x >= 0) && (x < sx));
	Assert((y >= 0) && (y < sy));
	
	sint16 from_x = x;
	sint16 from_y = y;
	pos.x = x + g_dir_offset[d].x; 
	pos.y = y + g_dir_offset[d].y;
	
    if (pos.x < 0) pos.x = sx + pos.x; 
    else if (sx <= pos.x) pos.x = pos.x - sx; 

	if (!g_theWorld->IsYwrap())
	{
	   if (pos.y < 0) { 
		   pos.y = 0; 
		   return FALSE; 
	   } else if (sy <= pos.y) { 
		   pos.y = sy - 1;
		   return FALSE; 
	   } 
	}
	else
	{
	
		Assert (2*sx == sy);
		if (pos.y < 0) pos.y += sy;
		else if (pos.y >= sy) pos.y -= sy;
	}
    
	if (!g_theWorld->IsXwrap())
	{
		Assert( 2*sx >= sy);
		
		if (from_x + (from_y/2) == sx - 1)
		{
			
			switch (d)
			{
			case EAST:
				return FALSE;
				break;
			case NORTHEAST:
			case SOUTHEAST:
				
				if (from_y & 1)
					return FALSE;
				break;
			default:
				break;
			}
		}
		else if (pos.x + pos.y/2 == sx - 1)
		{
			
			switch (d)
			{
			case WEST:
				return FALSE;
				break;
			case NORTHWEST:
			case SOUTHWEST:
				
				if (pos.y & 1)
					return FALSE;
				break;
			default:
				break;
			}
		}
	}
	Assert(0 <= pos.x); 
	Assert(pos.x < sx); 
	Assert(0 <= pos.y); 
	Assert(pos.y < sy); 

	return TRUE; 
}


WORLD_DIRECTION MapPoint::GetNeighborDirection(MapPoint neighbor) const 

{
    Assert(g_theWorld); 
	if (g_theWorld->IsYwrap()) { 
		if ((neighbor.y < 2) && ((g_mp_size.y-2) <= y)) { 
			neighbor.y += g_mp_size.y; 
			neighbor.x -= g_mp_size.y/2; 
			while (neighbor.x < 0 ) { 
				neighbor.x += g_mp_size.x; 
			}
		} else if (((g_mp_size.y-2) <= neighbor.y) && (y < 2)) { 
			neighbor.y -= g_mp_size.y;
			neighbor.x += g_mp_size.y/2; 
			while (g_mp_size.x <= neighbor.x) { 
				neighbor.x -= g_mp_size.x; 
			} 
			
		} 
	}
	
	if ((neighbor.x == 0) && (x == (g_mp_size.x-1))) { 
		neighbor.x = g_mp_size.x; 
	} else if ((neighbor.x == (g_mp_size.x-1)) && (x == 0)) { 
		neighbor.x = -1; 
	} 
	
	switch(neighbor.x - x) { 
        case 1: 
            switch ((neighbor.y - y)) {
				case -2: return NORTH; 
				case -1: return NORTHEAST; 
				case 0: return EAST; 
				default:
					Assert(0); 
					return NOWHERE; 
            }
        case 0: 
            switch ((neighbor.y - y)) {
				case -1: return NORTHWEST; 
				case 1: return SOUTHEAST; 
				default:
					Assert(0);
					return NOWHERE; 
            }
        case -1:
            switch ((neighbor.y - y)) {
				case 0: return WEST; 
				case 1: return SOUTHWEST; 
				case 2: return SOUTH; 
				default:
					Assert(0); 
					return NOWHERE; 
            } 
        default: 
            Assert(0); 
            return NOWHERE; 
	} 
}

BOOL MapPoint::IsNextTo(const MapPoint &neighbor) const 

{
	static MapPoint tmp;
    
  

    
    if(GetNeighborPosition(NORTH,tmp))
	   if (neighbor == tmp) return TRUE; 

	if(GetNeighborPosition(NORTHWEST,tmp))
		if (neighbor == tmp) return TRUE; 

    if(GetNeighborPosition(NORTHEAST,tmp))
		if (neighbor == tmp) return TRUE; 

    if(GetNeighborPosition(SOUTH,tmp))
		if (neighbor == tmp) return TRUE; 

    if(GetNeighborPosition(SOUTHWEST,tmp))
		if (neighbor == tmp) return TRUE; 

    if(GetNeighborPosition(SOUTHEAST,tmp))
		if (neighbor == tmp) return TRUE; 

    if(GetNeighborPosition(WEST,tmp))
		if (neighbor == tmp) return TRUE; 

    if(GetNeighborPosition(EAST,tmp))
		if (neighbor == tmp) return TRUE; 

    return FALSE; 
}


struct ScreenPoint
{
	ScreenPoint(const MapPoint &mp)
	{
		x = 2 * mp.x + mp.y;
		y = mp.y;
		if (x >= 2 * g_mp_size.x)
			x -= 2 * g_mp_size.x;
	}
	sint32 x, y;
};




static sint32 WrapDelta(sint32 delta, sint32 size)
{
	if (delta > 0)
	{
		if (2*delta > size)
			delta -= size;
	}
	else
	{
		if (-2*delta > size)
			delta += size;
	}
	return delta;
}

void MapPoint::NormalizedSubtract(const MapPoint &dest, MapPoint &diff) const
{
	ScreenPoint src(*this);
	ScreenPoint dst(dest);
	sint32 dx = dst.x - src.x;
	sint32 dy = dst.y - src.y;
	
	if (g_theWorld->IsYwrap())
		dy = WrapDelta(dy, g_mp_size.y);
	if (g_theWorld->IsXwrap())
		dx = WrapDelta(dx, 2 * g_mp_size.x);

	diff.x = (sint16 )dx;
	diff.y = (sint16 )dy;
}

sint32 MapPoint::NormalizedDistance(const MapPoint &dest) const
{
	ScreenPoint src(*this);
	ScreenPoint dst(dest);
	sint32 dx = dst.x - src.x;
	sint32 dy = dst.y - src.y;
	if (dx < 0) dx = -dx;
	if (dy < 0) dy = -dy;
	
	if (g_theWorld->IsYwrap())
	{
		sint32 y_size = g_mp_size.y;
		if (2*dy > y_size)
			dy = y_size - dy;
	}
	if (g_theWorld->IsXwrap())
	{
		sint32 x_size = 2 * g_mp_size.x;
		if (2*dx > x_size)
			dx = x_size - dx;
	}
	sint32 result = (dx + dy)/2;




	
	return result;
}


static void sqmp(const MapPoint &mp, sint32 &sq_min, MapPoint &diff) 
{
    sint32 square = mp.x * mp.x + mp.y * mp.y;

    if (square < sq_min) { 
        diff = mp; 
        sq_min = square; 
    } else if (square == sq_min) { 
        if (diff.x < mp.x) { 
            diff = mp; 
        } else if (diff.y < mp.y) { 
            diff = mp; 
        } 
    }
} 




void MapPoint::OldNormalizedSubtract(const MapPoint &dest, MapPoint &diff) const
{

    
    static MapPoint s, d;

    s.x = (g_mp_size.y-1) - (x + y);
    s.y = x; 

    d.x = (g_mp_size.y-1) - (dest.x + dest.y);
    d.y = dest.x; 

    
    static MapPoint delta_inner, 
					delta_left, delta_right,
					delta_up, delta_down, 
					delta_ur, delta_ul, delta_dr, delta_dl; 

    
    delta_inner.x = d.x - s.x; 
    delta_inner.y = d.y - s.y; 
    sint32 sq_min = delta_inner.x * delta_inner.x + delta_inner.y * delta_inner.y; 
    diff = delta_inner; 

    delta_up.x = delta_inner.x + g_mp_size.x; 
    delta_up.y = -(g_mp_size.x - d.y) - s.y;
    sqmp(delta_up, sq_min, diff); 

    delta_down.x = delta_inner.x - g_mp_size.x; 
    delta_down.y = (g_mp_size.x - s.y) + d.y;
    sqmp(delta_down, sq_min, diff); 

    if (g_theWorld->IsYwrap()) { 

        delta_right.x = g_mp_size.y + d.x - s.x; 
        delta_right.y = d.y - s.y;
        sqmp(delta_right, sq_min, diff); 

        delta_left.x = -(g_mp_size.y + s.x - d.x); 
        delta_left.y = d.y - s.y;
        sqmp(delta_left, sq_min, diff); 

        delta_ur.x = delta_right.x + g_mp_size.x; 
        delta_ur.y = delta_up.y;
        sqmp(delta_ur, sq_min, diff); 

        delta_ul.x = delta_left.x + g_mp_size.x; 
        delta_ul.y = delta_up.y;
        sqmp(delta_ul, sq_min, diff); 


        delta_dr.x = delta_right.x- g_mp_size.x; 
        delta_dr.y = delta_down.y;
        sqmp(delta_dr, sq_min, diff); 

        delta_dl.x = delta_left.x - g_mp_size.x; 
        delta_dl.y = delta_down.y;
        sqmp(delta_dl, sq_min, diff); 
    }
}























































































































void MapPoint::FirstRectItt(const sint32 d, MapPoint &pos, sint32 &count)
{
	count = 0;
	pos.Set(x,y);
	sint32 i;

	for(i = 0; i < d; i++) {
		pos.GetNeighborPosition(WEST,pos);
	}
}

BOOL MapPoint::EndRectItt(const sint32 d, const sint32 count)
{ 
	return count < (4 * (d * d) + 4 * d + 1);
}

void MapPoint::NextRectItt(const sint32 d, MapPoint &pos, sint32 &count)
{
	count++;
	sint32 i;
	sint32 len;
	len = 2 * d + 1;
	if((count % len) == 0) {
		pos.GetNeighborPosition(SOUTHEAST, pos);
		for(i = 0; i < len - 1; i++) {
			pos.GetNeighborPosition(SOUTHWEST, pos);
		}
	} else {
		pos.GetNeighborPosition(NORTHEAST, pos);
	}
}

void MapPoint::Iso2Norm(const MapPointData &pos) 
{
    x = pos.x; 
    y = pos.y; 

   
	y += x;
    while (y >= g_mp_size.y) { 
		y -= g_mp_size.y;
    }

    







}

void MapPoint::Norm2Iso(const MapPointData &pos) 
{
    x = pos.x; 
    y = pos.y; 

	while(x < 0)
		x += g_mp_size.x;
	while(x >= g_mp_size.x)
		x -= g_mp_size.x;

	y -= x;
	while(y < 0)
		y += g_mp_size.y;

#if 0
	
	if(x >= g_mp_size.x) {
		y -= x;
		x -= g_mp_size.x;
	} else if(x < 0) {
		y -= x;
		x += g_mp_size.x;
	} else {
		y -= x;
	}

	while(y < 0) {
		y += g_mp_size.y;
		x = (x + g_mp_size.x - (((g_mp_size.y / 2) % g_mp_size.x) - 1)) % g_mp_size.x;
	}
#endif


    







}

void MapPoint::Iso2Norm(const MapPoint &pos) 

{ 
    Iso2Norm(MapPointData(pos)); 
}

void MapPoint::Norm2Iso(const MapPoint &pos) 

{
    Norm2Iso(MapPointData(pos)); 
}













































#ifdef _DEBUG
sint32 OldSquaredDistance(const MapPoint &uPos, const MapPoint &pos)
{
	sint32 dx1, dy1, dx2, dy2, dx3, dy3;
	sint16 w = sint16(g_theWorld->GetXWidth());
	sint16 h = sint16(g_theWorld->GetYHeight());

	if(!g_theWorld->IsXwrap()) {
		
		
		sint32 adjX1 = (((uPos.x + (uPos.y / 2)) % w) * 2) + (uPos.y & 1);
		sint32 adjX2 = (((pos.x + (pos.y / 2)) % w) * 2) + (pos.y & 1);

		dy1 = uPos.y - pos.y;
		dx1 = adjX1 - adjX2;
		return (dx1 * dx1 + dy1 * dy1) / 2;
	}

	
	dx1 = uPos.x - pos.x;

	if(w - ABS(dx1) < ABS(dx1)) {
		if(dx1 < 0) {
			dx1 += w;
		} else {
			dx1 -= w;
		}
	}

	dy1 = (uPos.y - pos.y) + dx1;

	if(g_theWorld->IsYwrap()) {
		sint16 magicXAdj = (h - (w * 2)) / 2;
		
		MapPoint topPos = pos;
		topPos.x += sint16(magicXAdj);
		topPos.x = topPos.x % w;
		topPos.y -= h;
		dx2 = uPos.x - topPos.x;

		if(w - ABS(dx2) < ABS(dx2)) {
			if(dx2 < 0) {
				dx2 += w;
			} else {
				dx2 -= w;
			}
		}

		dy2 = (uPos.y - topPos.y) + dx2;

		MapPoint bottomPos = pos;
		bottomPos.x -= (magicXAdj);
		while(bottomPos.x < 0)
			bottomPos.x += w;
		bottomPos.y += h;
		dx3 = uPos.x - bottomPos.x;

		if(w - ABS(dx3) < ABS(dx3)) {
			if(dx3 < 0) {
				dx3 += w;
			} else {
				dx3 -= w;
			}
		}

		dy3 = (uPos.y - bottomPos.y) + dx3;

		return min(min((dx1*dx1)+(dy1*dy1), 
					   (dx2*dx2)+(dy2*dy2)),
				   (dx3*dx3)+(dy3*dy3));
	} else {
		return dx1*dx1 + dy1*dy1;
	}
}
#endif


sint32 MapPoint::GetSquaredDistance(const MapPoint &from, const MapPoint &to)
{
	ScreenPoint src(from);
	ScreenPoint dst(to);
	sint32 dx = dst.x - src.x;
	sint32 dy = dst.y - src.y;
	
	if (g_theWorld->IsYwrap())
		dy = WrapDelta(dy, g_mp_size.y);
	if (g_theWorld->IsXwrap())
		dx = WrapDelta(dx, 2 * g_mp_size.x);
	sint32 retval = (dx * dx + dy * dy) / 2;
#if defined(ACTIVISION_ORIGINAL)	// Useless error pop-up in debug mode
	Assert(retval <= OldSquaredDistance(from, to)); 
#endif
	return retval;
}













uint32 MapPoint_MapPoint_GetVersion(void)
	{
	return (k_MAPPOINT_VERSION_MAJOR<<16 | k_MAPPOINT_VERSION_MINOR) ;
	}

void verifyYwrap()
{
    MapPoint pos, tmp1, tmp2; 




































































































   for (pos.x=0; pos.x<g_mp_size.x; pos.x++) { 
      for (pos.y=0; pos.y<g_mp_size.y; pos.y++) {    
         tmp1.Norm2Iso(pos); 
         tmp2.Iso2Norm(tmp1); 
         Assert(pos == tmp2); 
      } 
   } 
}


void MapPoint::xy2rc(const MapPoint & xy_pos, const MapPoint & map_size)
{
#if 0
	sint16 c1 = ((xy_pos.x - xy_pos.y) / 2) % map_size.x;
	
	if (c1 < 0)
	{
		x = (sint16)(map_size.x + c1);
		y = xy_pos.y;
	}
	else
	{
		x = c1;
		y = xy_pos.y;
	}
#endif

	
	
	
	y = xy_pos.y;	
	x = (xy_pos.x - xy_pos.y)>>1;
	if (x<0) {
		do {
			x+= map_size.x;
		} while (x<0);
	}
	else while (x>=map_size.x) x-=map_size.x;
}


void MapPoint::rc2xy(const MapPoint & rc_pos, const MapPoint & map_size )
{
	
	
	
	
	
	
	y = rc_pos.y;	
	x = (2*rc_pos.x) + rc_pos.y;

	const w = map_size.x*2;
	while (x>=w) x -= w;
}

#if !defined(ACTIVISION_ORIGINAL)

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::OrthogonalPoint
//
// Description: Convert RC coordinates to orthogonal (XY) coordinates.
//
// Parameters : -
//
// Globals    : g_mp_size	: map size
//
// Returns    : -
//
// Remark(s)  : Constructor. Does not do any checking.
//
//----------------------------------------------------------------------------

OrthogonalPoint::OrthogonalPoint(MapPoint const & rc)
#if defined(_SMALL_MAPPOINTS)
:	m_point((2 * rc.x + rc.y) % (2 * g_mp_size.x), rc.y)
#else
:	m_point((2 * rc.x + rc.y) % (2 * g_mp_size.x), rc.y, rc.z)
#endif
{ }

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::GetRC
//
// Description: Convert to RC coordinates.
//
// Parameters : -
//
// Globals    : -
//
// Returns    : MapPoint	: Point with RC coordinates.
//
// Remark(s)  : The returned value is only valid when the current point is 
//              a valid map point. 
//
//----------------------------------------------------------------------------

MapPoint OrthogonalPoint::GetRC(void)
{
	MapPoint	remap(m_point);

	if (IsValid())
	{
		// Apply coordinate transformation
		remap.xy2rc(m_point, g_mp_size);
	}
	else
	{
		// Invalid data
		Assert(false);
	}

	return remap;
}

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::IsValid
//
// Description: Test whether the point is valid (i.e. on the map).
//
// Parameters : -
//
// Globals    : g_mp_size
//
// Returns    : bool	: point is valid
//
// Remark(s)  : Apply wrapping first. 
//
//----------------------------------------------------------------------------

bool OrthogonalPoint::IsValid(void)
{
	Normalise();

	return (m_point.x >= 0) && (m_point.x < (2 * g_mp_size.x))	&&
		   (m_point.y >= 0) && (m_point.y < g_mp_size.y)		&&
#if !defined(_SMALL_MAPPOINTS)
		   (m_point.z >= 0) && (m_point.z < g_mp_size.z)		&&
#endif
		   (((m_point.x + m_point.y) & 1) == 0);
}

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::Move
//
// Description: Move the position of a point.
//
// ParametersA: delta			: movement vector
//
// ParametersB: direction		: direction to move in
//				count			: number of steps to move
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : The move is always applied - even when invalid.
//
//----------------------------------------------------------------------------

void OrthogonalPoint::Move(MapPointData const & delta)
{
	m_point += MapPoint(delta);
}

//----------------------------------------------------------------------------

void OrthogonalPoint::Move
(
	WORLD_DIRECTION const	direction,
	size_t const			count
)
{
	MapPointData	delta	= Step(direction);
	delta.x	*= count;
	delta.y	*= count;
#if !defined(_SMALL_MAPPOINTS)
	delta.z	*= count;
#endif

	m_point	+= MapPoint(delta);
}

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::Normalise
//
// Description: Apply wrapping (when allowed) to bring the point coordinates
//              in the "usual" range.
//
// Parameters : -
//
// Globals    : g_theWorld
//				g_mp_size
//
// Returns    : -
//
// Remark(s)  : "Usual" range: 0 <= x < 2 * map width, 0 <= y < map height 
//
//----------------------------------------------------------------------------

void OrthogonalPoint::Normalise(void)
{
	if (g_theWorld->IsXwrap())
	{
		while (m_point.x < 0)
		{
			m_point.x += 2 * g_mp_size.x;
		}
		while (m_point.x >= 2 * g_mp_size.x)
		{
			m_point.x -= 2 * g_mp_size.x;
		}
	}

	if (g_theWorld->IsYwrap())
	{
		while (m_point.y < 0)
		{
			m_point.y += g_mp_size.y;
		}
		while (m_point.y >= g_mp_size.y)
		{
			m_point.y -= g_mp_size.y;
		}
	}

	// There is no IsZwrap function, not even when !defined(_SMALL_MAPPOINTS).
}

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::Step
//
// Description: Convert a direction to an XY coordinate delta.
//
// Parameters : direction		: direction to move
//
// Globals    : -
//
// Returns    : MapPointData	: XY coordinate delta
//
// Remark(s)  : When the argument is not a valid direction, the returned delta
//              is all 0.
//			
//----------------------------------------------------------------------------

MapPointData OrthogonalPoint::Step
(
	WORLD_DIRECTION const	direction
)
{
	switch (direction)
	{
	case NORTH:	
		return MapPointData(0, -2);
	case NORTHEAST:
		return MapPointData(1, -1);
	case EAST:
		return MapPointData(2 , 0);
	case SOUTHEAST:        
		return MapPointData(1, 1);
	case SOUTH:
		return MapPointData(0, 2);
	case SOUTHWEST:
		return MapPointData(-1, 1);
	case WEST:
		return MapPointData(-2, 0);
	case NORTHWEST:
		return MapPointData(-1, -1);
#if 0	// Not in WORLD_DIRECTION now, but could be used for space launches.
	case UP:
		return MapPointData(0, 0, 1);
	case DOWN:
		return MapPointData(0, 0, -1);
#endif
	default:
		return MapPointData(0, 0);
	}
}

#endif	// ACTIVISION_ORIGINAL
