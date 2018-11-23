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
// - Improved iterator class structure.
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

//----------------------------------------------------------------------------
// Project imports
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"  // sint32, uint32
#include "MapPoint.h"	    // MapPoint, RadiusIterator

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

class CityInfluenceIterator : public RadiusIterator
{
public:
	CityInfluenceIterator(MapPoint const & center, sint32 size);

protected:
	uint32  m_cityId;

    virtual bool    IsIncluded();
};

void GenerateCityInfluence(const MapPoint &cpos, sint32 size);
void GenerateBorders(const MapPoint &cpos, sint32 player, sint32 intRad, sint32 sqRad);

#endif
