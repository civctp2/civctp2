//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header file
// Description  : declarations for the SettleMap class
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
// - Removed MS version specific code.
// - Standardised <list> import.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __SETTLE_MAP_H__
#define __SETTLE_MAP_H__

#include <list>

class SettleMap;

size_t const	k_minimum_settle_city_size	= 2;
size_t const 	k_targets_per_continent 	= 25;

#include "bit_table.h"
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
		:	m_value	(0.0),
			m_pos	()
		{ ; };

		bool operator<(const SettleTarget & rval) const { return ( m_value < rval.m_value ); }
		bool operator>(const SettleTarget & rval) const { return ( m_value > rval.m_value ); }

		double	    m_value;
		MapPoint	m_pos;
	};

	typedef std::list<SettleTarget > SettleTargetList;

	static SettleMap s_settleMap;

	void Cleanup();
	void Initialize();

	void HandleCityGrowth(const Unit & city);

	void GetSettleTargets(const PLAYER_INDEX &player, 
						  const bool & settle_water,
						  SettleMap::SettleTargetList & targets) const;
	
	bool CanSettlePos(const MapPoint & rc_pos) const;

	void SetCanSettlePos(const MapPoint & rc_pos, const bool can_settle);
	
	double GetValue(const MapPoint & rc_pos) const;

private:	
    SettleMap();
	
    double ComputeSettleValue(const MapPoint & pos) const;

    MapGrid<double> m_settleValues;
    Bit_Table m_invalidCells;
};

#endif 




