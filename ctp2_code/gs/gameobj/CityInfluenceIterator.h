//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : City influence handling
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
// _MSC_VER		
// - When defined, allows Microsoft C++ extensions.
// - When not defined, generates standard C++.
//
// Note: For the blocks with _MSC_VER preprocessor directives, the following
//       is implied: the (_MSC_VER) preprocessor directive lines and the blocks 
//       between #else and #endif are modified Apolyton code. The blocks 
//       between #if and #else are the original Activision code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Wrap handling corrected. 
//
//----------------------------------------------------------------------------

#ifndef CITY_INFLUENCE_ITERATOR_H__
#define CITY_INFLUENCE_ITERATOR_H__

//----------------------------------------------------------------------------
// Library imports
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Exported names
//----------------------------------------------------------------------------

class CityInfluenceIterator;
class RadiusIterator;
class SquareIterator;

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "c3types.h"
#include "MapPoint.h"	// MapPoint, OrthogonalPoint

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

class CityInfluenceIterator {
protected:
	MapPoint m_center;
	MapPoint m_cur;
	MapPoint m_wrappedCur;
	sint16 m_startX, m_endX;
	sint16 m_row;
	sint16 m_intRadius;
	uint32 m_cityId;
	OrthogonalPoint	m_testXY;

public:
	
	CityInfluenceIterator(const MapPoint &center, sint32 size);
	CityInfluenceIterator();

	
	virtual void Init(const MapPoint &center, sint32 size);

	virtual void Start();
	bool End();
	virtual void Next();
	MapPoint &Pos();
};


class RadiusIterator : public CityInfluenceIterator {
protected:
	sint16 m_squaredRadius;

public:
	RadiusIterator(const MapPoint &center, sint32 size);
	RadiusIterator(const MapPoint &center, sint32 size, sint32 squaredSize);
	RadiusIterator();

	
	void Init(const MapPoint &center, sint32 size, sint32 squaredSize);
	void Start();

	
	void Next();
};


class SquareIterator : public CityInfluenceIterator {
public:
	SquareIterator(const MapPoint &center, sint32 size);
	SquareIterator();

	
	void Init(const MapPoint &center, sint32 size);
	void Start();

	
	void Next();
};

void GenerateCityInfluence(const MapPoint &cpos, sint32 size);
void GenerateBorders(const MapPoint &cpos, sint32 player, sint32 intRad, sint32 sqRad);

#endif
