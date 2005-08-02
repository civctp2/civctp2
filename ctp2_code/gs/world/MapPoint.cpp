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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Added OrthogonalPoint to facilitate wrap computations.
// - Corrected wrap computation for city influence and borders.
// - Fixed first RadiusIterator constructor so that it does the same as
//   in the original version. The defect caused a significant AI 
//   performance loss. July 16th 2005 Martin Gühmann
//
//----------------------------------------------------------------------------

#include "c3.h"                 // pre-compiled header
#include "MapPoint.h"           // own declarations: consistency check

#include "civarchive.h"
#include "XY_Coordinates.h"
#include "World.h"              // g_theWorld
#include "directions.h"
#include "c3math.h"

MapPoint g_mp_size;

#define k_MAPPOINT_VERSION_MAJOR    0
#define k_MAPPOINT_VERSION_MINOR    0

namespace
{
MapPoint const  MAP_POINT_UNKNOWN   = MapPoint(-1, -1);

void sqmp(const MapPoint &mp, sint32 &sq_min, MapPoint &diff)
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

sint32 WrapDelta(sint32 delta, sint32 size)
{
	if (delta > 0)
	{
		while (2 * delta > size)
		{
			delta -= size;
		}
	}
	else
	{
		while (-2 * delta > size)
		{
			delta += size;
		}
	}

	return delta;
}

}	// namespace


MapPoint& MapPoint::operator += (const MapPoint &rhs)
{
	x += rhs.x;
	y += rhs.y;
#if !defined(_SMALL_MAPPOINTS)
	z += rhs.z;
#endif
	return *this;
}

MapPoint & MapPoint::operator -= (const MapPoint &rhs)
{
	x -= rhs.x;
	y -= rhs.y;
#if !defined(_SMALL_MAPPOINTS)
	z -= rhs.z;
#endif
    return *this;
}

void MapPoint::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
		{
		archive<<x;
		archive<<y;
#if !defined(_SMALL_MAPPOINTS)
		archive << z;
#endif
		}
	else
		{
		archive>>x;
		archive>>y;
#if !defined(_SMALL_MAPPOINTS)
		archive >> z;
#endif
		}
}

bool MapPoint::operator == (const MapPoint &test_me) const
{
	return ((x == test_me.x) && (y == test_me.y));
}

bool MapPoint::operator!= (const MapPoint &test_me) const
{
	return ((x != test_me.x) || (y != test_me.y));
}

//----------------------------------------------------------------------------
//
// Name       : MapPoint::GetNeighborPosition
//
// Description: Get a neighbour of this point.
//
// Parameters : direction       : the direction to get the neighbour for
//
// Globals    : -
//
// Returns    : bool            : the neighbour is valid (i.e. on the map)
//              pos             : the neighbour
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool MapPoint::GetNeighborPosition
(
	WORLD_DIRECTION const & direction,
	MapPoint &              pos
) const
{
	OrthogonalPoint	point(*this);
	Assert(point.IsValid());

	point.Move(direction);
	bool const	isAtMap	= point.IsValid();
	if (isAtMap)
	{
		pos = point.GetRC();
	}

	return isAtMap;
}

//----------------------------------------------------------------------------
//
// Name       : MapPoint::GetNeighborDirection
//
// Description: Get the direction of a neighbour.
//
// Parameters : neighbor        : the neighbour to get the direction for
//
// Globals    : -
//
// Returns    : WORLD_DIRECTION : the direction of the neighbour
//
// Remark(s)  : When neighbor is not actually a - direct - neighbour,
//              NOWHERE will be returned.
//
//----------------------------------------------------------------------------
WORLD_DIRECTION MapPoint::GetNeighborDirection(MapPoint const & neighbor) const
{
	MapPointData diff = NormalizedSubtract(neighbor);

	switch (diff.x)
	{
	default:
		break;

	case -2:
		if ( 0 == diff.y)       return WEST;
		break;

	case -1:
		if (-1 == diff.y)       return NORTHWEST;
		if ( 1 == diff.y)       return SOUTHWEST;
		break;

	case  0:
		if (-2 == diff.y)       return NORTH;
		if ( 2 == diff.y)       return SOUTH;
#if !defined(_SMALL_MAPPOINTS)
		if ( 0 == diff.y) 
		{
			if (-1 == diff.z)   return DOWN;
			if ( 1 == diff.z)   return UP;
		}
#endif
		break;

	case 1:
		if (-1 == diff.y)       return NORTHEAST;
		if ( 1 == diff.y)       return SOUTHEAST;
		break;

	case 2:
		if ( 0 == diff.y)       return EAST;
		break;
	}

	return NOWHERE;
}

//----------------------------------------------------------------------------
//
// Name       : MapPoint::IsNextTo
//
// Description: Check whether neighbor is actually a neighbour.
//
// Parameters : neighbor        : the neighbour to check
//
// Globals    : -
//
// Returns    : bool            : neighbor is a direct neighbour
//
// Remark(s)  : When neighbor is not a valid map location, false will be
//              returned. The validity of this location is not checked.
//
//----------------------------------------------------------------------------
bool MapPoint::IsNextTo(MapPoint const & neighbor) const
{
	WORLD_DIRECTION	const	direction	= GetNeighborDirection(neighbor);

	if (NOWHERE == direction)
	{
		return false;
	}

	MapPoint				tmp;
	return GetNeighborPosition(direction, tmp) && (neighbor == tmp);
}

//----------------------------------------------------------------------------
//
// Name       : MapPoint::IsValid
//
// Description: Test whether the point is valid (i.e. on the map).
//
// Parameters : -
//
// Globals    : g_mp_size
//              g_theWorld
//
// Returns    : bool	: point is valid
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
bool MapPoint::IsValid(void) const
{
	return (g_theWorld->IsXwrap() || ((x >= 0) && (x < g_mp_size.x)))
	    && (g_theWorld->IsYwrap() || ((y >= 0) && (y < g_mp_size.y)))
#if !defined(_SMALL_MAPPOINTS)
	    && (z >= 0) && (z < g_mp_size.z)
#endif
	    ;
}

//----------------------------------------------------------------------------
//
// Name       : MapPoint::NormalizedSubtract
//
// Description: Return the (shortest) difference between dest and this point.
//
// Parameters : dest            : the point to go to from here
//
// Globals    : g_mp_size
//              g_theWorld
//
// Returns    : MapPointData     : the difference, in XY coordinates
//
// Remark(s)  : Not valid for z component yet.
//
//----------------------------------------------------------------------------
MapPointData MapPoint::NormalizedSubtract(MapPoint const & dest) const
{
	OrthogonalPoint here    (*this);
	OrthogonalPoint to      (dest);
	to.Move(MapPointData(-here.x, -here.y));

	MapPointData    diff    (to.x, to.y);    

	if (g_theWorld->IsXwrap())
	{
		diff.x = WrapDelta(diff.x, 2 * g_mp_size.x);
	}
	if (g_theWorld->IsYwrap())
	{
		diff.y = WrapDelta(diff.y, g_mp_size.y);
	}

	return diff;
}

//----------------------------------------------------------------------------
//
// Name       : MapPoint::NormalizedDistance
//
// Description: Return the (shortest) difference between dest and this point.
//
// Parameters : dest    : the point to go to from here
//
// Globals    : -
//
// Returns    : sint32  : the distance
//
// Remark(s)  : Not valid for z component yet.
//
//----------------------------------------------------------------------------
sint32 MapPoint::NormalizedDistance(MapPoint const & dest) const
{
	MapPointData	diff    = NormalizedSubtract(dest);

	return (abs(diff.x) + abs(diff.y)) / 2;
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


//----------------------------------------------------------------------------
//
// Name       : MapPoint::GetSquaredDistance
//
// Description: Return the (shortest) difference between dest and this point.
//
// Parameters : dest    : the point to go to from here
//
// Globals    : -
//
// Returns    : sint32  : the squared distance
//
// Remark(s)  : Not valid for z component yet.
//
//----------------------------------------------------------------------------
sint32 MapPoint::GetSquaredDistance(MapPoint const & from, MapPoint const & to)
{
	MapPointData diff = from.NormalizedSubtract(to);

	return ((diff.x * diff.x) + (diff.y * diff.y)) / 2;
	// Check return value when defining !_SMALL_MAPPOINTS
}













uint32 MapPoint_MapPoint_GetVersion(void)
	{
	return (k_MAPPOINT_VERSION_MAJOR<<16 | k_MAPPOINT_VERSION_MINOR) ;
	}

void verifyYwrap()
{
	MapPoint pos, tmp1, tmp2;

	for (pos.x=0; pos.x<g_mp_size.x; pos.x++)
	{
		for (pos.y=0; pos.y<g_mp_size.y; pos.y++)
		{
			tmp1.Norm2Iso(pos);
			tmp2.Iso2Norm(tmp1);
			Assert(pos == tmp2);
		}
	}
}


void MapPoint::xy2rc(const MapPoint & xy_pos, const MapPoint & map_size)
{
	y = xy_pos.y;
	x = (xy_pos.x - xy_pos.y) >> 1;

	sint16 const	w = map_size.x;

	while (x < 0)
	{
		x += w;
	}

	while (x >= w)
	{
		x -= w;
	}
}


void MapPoint::rc2xy(const MapPoint & rc_pos, const MapPoint & map_size )
{
	y = rc_pos.y;
	x = (2 * rc_pos.x) + rc_pos.y;

	sint16 const	w	= map_size.x * 2;
	while (x < 0)
	{
		x += w;
	}

	while (x >=w)
	{
		x -= w;
	}
}


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
:
	x       (m_point.x),
	y       (m_point.y),
	m_point ((2 * rc.x + rc.y) % (2 * g_mp_size.x), rc.y)
#else
:	m_point((2 * rc.x + rc.y) % (2 * g_mp_size.x), rc.y, rc.z),
	x       (m_point.x),
	y       (m_point.y),
	z       (m_point.z)
#endif
{ }

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::OrthogonalPoint
//
// Description: Copy constructor
//
// Parameters : copy    : data to copy from
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : Constructor. Does not do any checking.
//
//----------------------------------------------------------------------------
OrthogonalPoint::OrthogonalPoint(OrthogonalPoint const & copy)
:
	x       (m_point.x),
	y       (m_point.y),
#if !defined(_SMALL_MAPPOINTS)
	z       (m_point.z),
#endif
	m_point (copy.m_point)
{ }

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::operator =
//
// Description: Assignment operator
//
// Parameters : copy    : data to assign from
//
// Globals    : -
//
// Returns    : OrthogonalPoint &   : this object, after modification
//
// Remark(s)  : Does not do any checking.
//
//----------------------------------------------------------------------------
OrthogonalPoint & OrthogonalPoint::operator = (OrthogonalPoint const & copy)
{
	m_point = copy.m_point;
	return *this;
}

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
// Returns    : bool    : point is valid
//
// Remark(s)  : Apply wrapping first.
//
//----------------------------------------------------------------------------
bool OrthogonalPoint::IsValid(void) 
{
	Normalise();

	return (m_point.x >= 0) && (m_point.x < (2 * g_mp_size.x))  &&
	       (m_point.y >= 0) && (m_point.y < g_mp_size.y)        &&
#if !defined(_SMALL_MAPPOINTS)
	       (m_point.z >= 0) && (m_point.z < g_mp_size.z)        &&
#endif
	       (((m_point.x + m_point.y) & 1) == 0);
}

//----------------------------------------------------------------------------
//
// Name       : OrthogonalPoint::Move
//
// Description: Move the position of a point.
//
// ParametersA: delta           : movement vector
//
// ParametersB: direction       : direction to move in
//              count           : number of steps to move
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
	WORLD_DIRECTION const   direction,
	size_t const            count
)
{
	MapPointData	delta   = Step(direction);
	delta.x *= count;
	delta.y *= count;
#if !defined(_SMALL_MAPPOINTS)
	delta.z *= count;
#endif

	m_point += MapPoint(delta);
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
//              g_mp_size
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
#if !defined(_SMALL_MAPPOINTS)
	case UP:
		return MapPointData(0, 0, 1);
	case DOWN:
		return MapPointData(0, 0, -1);
#endif
	default:
		return MapPointData(0, 0);
	}
}

//----------------------------------------------------------------------------
//
// Name       : SquareIterator
//
// Description: -
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

SquareIterator::SquareIterator(MapPoint const & center, sint32 size)
:	m_center        (center),
	m_cur           (center),
	m_wrappedCur    (center),
	m_startX        (center.x),
	m_endX          (center.x),
	m_row           (size),
	m_intRadius     (static_cast<sint16>(size)),
	m_testXY        (OrthogonalPoint(m_center))
{
}

void SquareIterator::Start()
{
	m_startX    = m_center.x - m_intRadius;
	m_endX      = m_center.x + m_intRadius;
	m_cur.x     = m_startX;
	m_row       = 0;
	m_cur.y     = m_center.y + m_row;
	
	m_testXY    = OrthogonalPoint(m_center);
	m_testXY.Move(WEST, m_intRadius);
	
	if (IsIncluded())
	{
		// Point is on the map
		m_wrappedCur = m_testXY.GetRC();
	}
	else
	{
		Next();
	}
}

void SquareIterator::Next()
{
	bool	isValid	= false;
	
	while (!isValid && !End())  
	{
		m_cur.x++;
		m_cur.y--;
		m_testXY.Move(NORTHEAST);

		if (m_cur.x > m_endX) 
		{
			m_row++;
			m_cur.x = m_startX;
			m_cur.y = m_center.y + m_row;
			m_testXY.Move(SOUTH);
			m_testXY.Move(SOUTHWEST, m_endX - m_startX);
		}

		isValid	= IsIncluded();
	} 
		
	if (isValid)
	{
		m_wrappedCur = m_testXY.GetRC();
	}
}

bool SquareIterator::End() const
{
	return m_row > (2 * m_intRadius);
}

bool SquareIterator::IsIncluded()
{
	return m_testXY.IsValid();
}

MapPoint SquareIterator::Pos() const
{
	return m_wrappedCur;
}

//----------------------------------------------------------------------------
//
// Name       : RadiusIterator
//
// Description: -
//
// Parameters : -
//
// Globals    : -
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

RadiusIterator::RadiusIterator(MapPoint const & center, sint32 size)
:	SquareIterator  (center, size),
	m_squaredRadius (static_cast<double>(size * size))
{
}

RadiusIterator::RadiusIterator(MapPoint const & center, sint32 size, double squaredSize)
:	SquareIterator  (center, size),
	m_squaredRadius (squaredSize)
{
}

bool RadiusIterator::IsIncluded()
{
	return m_testXY.IsValid() &&
	       (MapPoint::GetSquaredDistance(m_cur, m_center) <= m_squaredRadius);
}
