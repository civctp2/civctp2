//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : City radius handler
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - pragma commented out
// - incorrect function removed
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __CITYRADIUS_H__
#define __CITYRADIUS_H__

class MapPoint;

class CityRadiusCallback
{
public:
	virtual void CityRadiusFunc(const MapPoint &pos) = 0;
};




void CityRadiusIterator(const MapPoint &cpos,
						CityRadiusCallback *cb);
void AdjacentIterator(const MapPoint &cpos, 
					  CityRadiusCallback *cb);

#endif
