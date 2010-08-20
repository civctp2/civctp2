//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Map point handling
// Id           : $Id$
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
// - #pragma once commented out
// - Import structure modified to allow mingw compilation.
// - bool added to == and != operators.
// - Added OrthogonalPoint to facilitate wrap computations.
// - Corrected wrap computation for city influence and borders.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef MAPPOINT_H
#define MAPPOINT_H 1

//----------------------------------------------------------------------------
// Compiler flags
//----------------------------------------------------------------------------

#define _SMALL_MAPPOINTS	// Use 2 dimensions (no space layer) when defined

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class	MapPoint;						// x = diagonal,  y = latitude (RC)
struct	MapPointData;
class	OrthogonalPoint;				// x = longitude, y = latitude (XY)
class   RadiusIterator;
class   SquareIterator;
struct	TileUtility;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "civarchive.h"		// CivArchive
#include "directions.h"		// WORLD_DIRECTION

//----------------------------------------------------------------------------
// General declarations
//----------------------------------------------------------------------------

uint32 MapPoint_MapPoint_GetVersion(void);

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

struct MapPointData
{
#if defined(_SMALL_MAPPOINTS)
	MapPointData(sint16 const a_X = 0, sint16 const a_Y = 0)
	:	x(a_X),
		y(a_Y)
	{ };
#else
	MapPointData(sint16 const a_X = 0, sint16 const a_Y = 0, sint16 const a_Z = 0)
	:	x(a_X),
		y(a_Y),
		z(a_Z)
	{ };

	sint16			z;
#endif

	// Coordinates
	sint16			x;
	sint16			y;

	bool operator == (MapPointData const & point) const
	{
		return ((x == point.x) && (y == point.y));
	};

	bool operator != (MapPointData const & point) const
	{
		return (! operator ==(point));
	};
};

struct TileUtility
{
    double			m_utility;
    double			m_food;
    double			m_production;
    double			m_gold;
    double			m_can_be_irrigated;
    MapPointData	m_pos;
};

class MapPoint : public MapPointData
{
public:
	MapPoint()
	:	MapPointData()
	{ };

	explicit MapPoint(MapPointData const & a_Data)
	: MapPointData(a_Data)
	{ };


#ifdef _SMALL_MAPPOINTS
	MapPoint(const sint32 ix, const sint32 iy) {
		x = (sint16)ix; y = (sint16)iy;
	}
#else
	MapPoint(const sint32 ix, const sint32 iy, const sint32 iz) {
		x = ix; y = iy; z = iz;
	}
#endif





    bool operator == (const MapPoint &test_me) const;
    bool operator != (const MapPoint &test_me) const;
    MapPoint & operator += (const MapPoint &rhs);
    MapPoint & operator -= (const MapPoint &rhs);
    MapPoint & operator += (const MapPointData &rhs);

    double EuclidianLength () const
    {
#ifdef _SMALL_MAPPOINTS
        double sum = x * x + y * y;
#else
        double sum = x * x + y * y + z * z;
#endif
        return sqrt(sum);
    }

#ifdef _SMALL_MAPPOINTS
    void Set(sint32 ix, sint32 iy) { x = (sint16)ix; y = (sint16)iy; }
#else
    void Set(sint32 ix, sint32 iy, sint32 iz) { x = ix; y = iy; z = iz; }
#endif

	bool HasUnexploredNeighbor(sint32 player) const;
	bool GetNeighborPosition
	(
		WORLD_DIRECTION const &	direction,
		MapPoint & 				pos
	) const;

	static WORLD_DIRECTION GetOppsositeDirection(WORLD_DIRECTION const & direction);

    WORLD_DIRECTION GetNeighborDirection(MapPoint const & neighbor) const;
    MapPointData    NormalizedSubtract(MapPoint const & dest) const;
	sint32 NormalizedDistance(const MapPoint &dest) const;
    void OldNormalizedSubtract(const MapPoint &dest, MapPoint &diff) const;
    bool IsNextTo(const MapPoint &neighbor) const;
	bool IsValid(void) const;

	void DelPointers() {}

    void Iso2Norm(const MapPointData &pos);
    void Norm2Iso(const MapPointData &pos);
    void Iso2Norm(const MapPoint &pos);
    void Norm2Iso(const MapPoint &pos);

	void Serialize(CivArchive &archive) ;


	static sint32 GetSquaredDistance(const MapPoint &uPos, const MapPoint &pos);


	void xy2rc(const MapPoint & xy_pos, const MapPoint & map_size);


	void rc2xy(const MapPoint & rc_pos, const MapPoint & map_size);


	bool operator < (const MapPoint & rval) const
		{return ((x< rval.x) && (y < rval.y)); }

};

class OrthogonalPoint
{
public:
	explicit OrthogonalPoint(MapPoint const & point);
    OrthogonalPoint(OrthogonalPoint const & copy);
    OrthogonalPoint & operator = (OrthogonalPoint const & copy);

	MapPoint			GetRC(void);
	bool				IsValid(void);
	void				Move
	(
		MapPointData const &	delta
	);
	void				Move
	(
		WORLD_DIRECTION const	direction,
		size_t const			count		= 1
	);

    sint16 const &      x;
    sint16 const &      y;
#if !defined(_SMALL_MAPPOINTS)
    sint16 const &      z;
#endif

private:

	void				Normalise(void);

	static MapPointData	Step(WORLD_DIRECTION const direction);

	MapPoint			m_point;	// point in XY coordinates
};


class SquareIterator
{
public:
	SquareIterator(MapPoint const & center, sint32 size);

	void            Start();
	void            Next();
	bool            End() const;
	const MapPoint& Pos() const;

protected:
	MapPoint        m_center;
	MapPoint        m_cur;
	MapPoint        m_wrappedCur;
	sint16          m_startX;
	sint16          m_endX;
	sint16          m_row;
	sint16          m_intRadius;
	OrthogonalPoint m_testXY;

	virtual bool    IsIncluded();
};

class RadiusIterator : public SquareIterator 
{
public:
	RadiusIterator(MapPoint const & center, sint32 size);
	RadiusIterator(MapPoint const & center, sint32 size, double squaredSize);
	
protected:
	double          m_squaredRadius;

	virtual bool    IsIncluded();
};

class CircleIterator : public RadiusIterator 
{
public:
	CircleIterator(MapPoint const & center, sint32 outerSize, sint32 innerSize);
	CircleIterator(MapPoint const & center, sint32 outerSize, double squaredOuterSize, sint32 innerSize);
	
protected:
	double          m_innerSquaredRadius;

	virtual bool    IsIncluded();
};

#endif
