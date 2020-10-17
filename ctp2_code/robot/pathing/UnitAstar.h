//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star pathfinding for Units
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
// - Added avoidList Param
// - Added methods to check room for army and danger along path
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __UNIT_ASTAR__
#define __UNIT_ASTAR__ 1

#include "Astar.h"
#include "Army.h"
#include "squad_Strength.h"

class Cell;
class CityData;
class UnitAstar : public Astar
{
protected:
	bool FindPath(const Army & army,
				sint32           nUnits,
				uint32           move_intersection,
				uint32           move_union,
				const MapPoint & start,
				PLAYER_INDEX     owner,
				const MapPoint & dest,
				Path           & good_path,
				bool           & is_broken_path,
				Path           & bad_path,
				float          & total_cost,
				bool             no_bad_path,
				sint32           cutoff,
				sint32         & nodes_opened,
				const bool     & check_dest,
				bool             no_straight_lines);

	bool CheckIsDangerForPos(const MapPoint & pos);
	void InitArmy(const Army & army, sint32 & nUnits, uint32 & move_intersection, uint32 & move_union,
				float & army_minmax_move);

	Army m_army;
	bool m_army_can_expel_stealth;
	float m_army_minmax_move;

	bool m_isTransporter;
	bool m_is_robot;

	MapPoint m_start;
	MapPoint m_dest;

public:
	UnitAstar();

	virtual bool   EntryCost(const MapPoint & prev, const MapPoint & pos, float & cost, bool & is_zoc,
				ASTAR_ENTRY_TYPE & entry);
	virtual float  EstimateFutureCost(const MapPoint & pos, const MapPoint & dest);
	virtual void   RecalcEntryCost(AstarPoint * parent, AstarPoint * node, float & new_entry_cost, bool & new_is_zoc,
				ASTAR_ENTRY_TYPE & entry);
	virtual sint32 GetMaxDir(MapPoint & pos) const;

	bool FindPath(Army & army, const MapPoint & start, PLAYER_INDEX owner, const MapPoint & dest, Path & new_path,
				bool & is_broken_path, Path & bad_path, float & total_cost);

	bool StraightLine(const MapPoint & start, const MapPoint & dest, Path & new_path) const;

private:
	void ClearMem();
	bool VerifyMem() const;

	bool CheckUnexplored(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, float & cost, bool & is_zoc, ASTAR_ENTRY_TYPE & entry,
				bool & can_enter) const;
	bool CheckUnits(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, float & cost, bool & is_zoc, bool & can_be_zoc, ASTAR_ENTRY_TYPE & entry,
				bool & can_enter);
	bool CheckHeight(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, float & cost, bool & is_zoc, ASTAR_ENTRY_TYPE & entry, bool & can_enter);
	bool CheckHisCity(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, CityData * the_pos_city, float & cost, bool & is_zoc,
				ASTAR_ENTRY_TYPE & entry, bool & can_enter);
	bool CheckMyCity(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, CityData * the_pos_city, float & cost, bool & is_zoc,
				ASTAR_ENTRY_TYPE & entry, bool & can_enter);
	bool CheckMoveUnion(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, float & cost, bool & is_zoc, bool can_be_zoc,
				ASTAR_ENTRY_TYPE & entry, bool & can_enter);
	bool CheckMoveIntersection(const MapPoint & prev, const MapPoint & pos, const Cell & the_prev_cell,
				const Cell & the_pos_cell, float & cost, bool & is_zoc, bool can_be_zoc, ASTAR_ENTRY_TYPE & entry,
				bool & can_enter);

	bool PretestDest_Enterable         (const MapPoint & start, const MapPoint & dest);
	bool PretestDest_HasRoom           (const MapPoint & start, const MapPoint & dest);
	bool PretestDest_SameLandContinent (const MapPoint & start, const MapPoint & dest) const;
	bool PretestDest_SameWaterContinent(const MapPoint & start, const MapPoint & dest) const;
	bool PretestDest_ZocEnterable      (const MapPoint & start, const MapPoint & dest);
	bool PretestDest                   (const MapPoint & start, const MapPoint & dest);

	bool IsBeachLanding(const MapPoint & prev, const MapPoint & pos, const uint32 & m_move_intersection);
	bool CanSpaceLaunch();
	bool CanSpaceLand();
	bool CanMoveIntoTransports(const MapPoint & pos);

	float ComputeValidMoveCost(const MapPoint & pos, const Cell & cell) const;
	bool FindVisionEdge(Path & a_path, MapPoint & old) const;

	void InitSearch(const MapPoint & start, PLAYER_INDEX owner, const MapPoint & dest, Path & good_path,
				bool & is_broken_path, Path & bad_path);
	bool FindStraightPath(const MapPoint & start, const MapPoint & dest, Path & good_path, bool & is_broken_path,
				Path & bad_path, float & total_cost, bool no_bad_path, sint32 cutoff, sint32 & nodes_opened);
	bool FindBrokenPath(const MapPoint & start, const MapPoint & dest, Path & good_path, Path & bad_path,
				float & total_cost);
	bool EnterPathPoints(Path & a_path, MapPoint & old);

	uint32 m_move_union;
	uint32 m_move_intersection;
	sint32 m_max_dir;
	uint32 m_mask_alliance;

	bool m_no_bad_path;
	bool m_is_zero_attack;
	PLAYER_INDEX m_owner;

	sint32 m_nUnits;

	bool m_can_space_land;
	bool m_can_be_cargo_podded;
	bool m_ignore_zoc;
	bool m_is_cargo_pod;
	uint32 m_cargo_pod_intersection;
	bool m_check_dest;
	Squad_Strength m_army_strength;
};

#endif
