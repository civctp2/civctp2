//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : Declarations for the SettleMap class
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
// - Removed MS version specific code.
// - Standardised <list> import.
// - Moved settle_water argument inside SettleMap::GetSettleTargets. (May 20th 2006 Martin Gühmann)
// - Removed m_invalidCells, cells not suited for settling have a value of
//   zero. (03-Jan-2018 Martin Gühmann)
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __SETTLE_MAP_H__
#define __SETTLE_MAP_H__

#include <list>

class SettleMap;

sint32 const    k_minimum_settle_city_size  = 2;
sint16 const    k_targets_per_continent     = 25;

#include "mapgrid.h"
#include "MapPoint.h"
#include "player.h"
#include "Unit.h"

class SettleMap
{
public:
	struct SettleTarget
	{
		SettleTarget()
		:   m_value (0.0),
		    m_pos   ()
		{ ; };

		bool operator<(const SettleTarget & rval) const { return ( m_value < rval.m_value ); }
		bool operator>(const SettleTarget & rval) const { return ( m_value > rval.m_value ); }

		double      m_value;
		MapPoint    m_pos;
	};

	typedef std::list<SettleTarget > SettleTargetList;

	static SettleMap s_settleMap;

	void Cleanup();
	void Initialize();

	void HandleCityGrowth(const Unit & city, sint32 oldSizeIndex);

	void GetSettleTargets(const PLAYER_INDEX &player,
	                      SettleMap::SettleTargetList & targets) const;

	bool HasSettleTargets(const PLAYER_INDEX &player, bool isWater) const;

	bool CanSettlePos(const MapPoint & rc_pos) const;

	double GetValue(const MapPoint & rc_pos) const;

private:
	SettleMap();

	double ComputeSettleValue(const MapPoint & pos, const Unit &city) const;
	void CanUnitRecordSettle(const UnitRecord* rec, bool* settleTerrainTypes, bool &noSettleUnits) const;
	bool HasCityInSettleDistance(const MapPoint & pos, sint32 min_settle_distance) const;

	MapGrid<double> m_settleValues;
};

#endif
