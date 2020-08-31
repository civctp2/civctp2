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
	bool FindPath(Army army,
				sint32 nUnits,
				uint32 move_intersetion,
				uint32 move_union,
				const MapPoint &start,
				const PLAYER_INDEX owner,
				const MapPoint &dest,
				Path &good_path,
				bool  &is_broken_path,
				Path &bad_path,
				float &total_cost,
				const bool no_bad_path,
				const sint32 cutoff,
				sint32 &nodes_opened,
				const bool &check_dest,
				const bool no_straight_lines);

    uint32 m_move_union;
    uint32 m_move_intersection;
    sint32 m_max_dir;
    uint32 m_mask_alliance;

    MapPoint m_dest;

    bool m_no_bad_path;
    bool m_is_zero_attack;
    PLAYER_INDEX m_owner;

    sint32 m_nUnits;

    Army m_army;

    float m_army_minmax_move;
    BOOL m_can_space_launch;
    BOOL m_can_space_land;
    BOOL m_can_be_cargo_podded;
    bool m_ignore_zoc;
    bool m_is_cargo_pod;
    uint32 m_cargo_pod_intersection;
    bool m_check_dest;
    bool m_is_robot;
    bool m_army_can_expel_stealth;
    MapPoint m_start;
    bool m_isTransporter;
    Squad_Strength m_army_strength;

public:

    UnitAstar();

    float ComputeValidMovCost(const MapPoint &pos, Cell *the_pos_cell);

    bool CheckUnexplored(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    bool CheckUnits(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        float &cost, bool &is_zoc, bool &can_be_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    bool CheckHisCity(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        CityData *the_pos_city,
        float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    bool CheckHeight(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    bool CheckMyCity(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        CityData *the_pos_city,
        float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    bool CheckMoveUnion(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        float &cost, bool &is_zoc, const bool can_be_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    bool CheckMoveIntersection(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell,
        float &cost, bool &is_zoc, const bool can_be_zoc, ASTAR_ENTRY_TYPE &entry, bool &can_enter);

    virtual bool EntryCost(const MapPoint &prev, const MapPoint &pos,
       float &cost, bool &is_zoc, ASTAR_ENTRY_TYPE &entry);

    bool CheckIsDangerForPos(const MapPoint & myPos);

    void RecalcEntryCost(AstarPoint *parent, AstarPoint *node, float &new_entery_cost,
        bool &new_is_zoc, ASTAR_ENTRY_TYPE &entry);

    sint32 GetMaxDir(MapPoint &pos) const;
    bool CanMoveIntoTransports(const MapPoint &pos);
    bool CanRailLaunch(const MapPoint &spos);
    bool CanSpaceLaunch();
    bool CanSpaceLand();

    float EstimateFutureCost(const MapPoint &pos, const MapPoint &dest);

    void InitArmy(const Army &army, sint32 &nUnints,
    uint32 &move_intersection,  uint32 &move_union, float &army_minmax_move);

    void InitSearch( const MapPoint &start, const PLAYER_INDEX owner,
    const MapPoint &dest, Path &good_path, bool &is_broken_path, Path &bad_path);

    bool EnterPathPoints(Path &a_path, MapPoint &old);
    bool FindVisionEdge (Path &a_path, MapPoint &old);

    bool PretestDest_Enterable         (const MapPoint &start, const MapPoint &dest);
    bool PretestDest_HasRoom           (const MapPoint &start, const MapPoint &dest);
    bool PretestDest_SameLandContinent (const MapPoint &start, const MapPoint &dest);
    bool PretestDest_SameWaterContinent(const MapPoint &start, const MapPoint &dest);
    bool PretestDest_ZocEnterable      (const MapPoint &start, const MapPoint &dest);

    bool PretestDest(const MapPoint &start, const MapPoint &dest);

    bool FindBrokenPath(const MapPoint &start, const MapPoint &dest, Path &good_path,
                        Path &bad_path, float &total_cost);

    bool FindStraightPath(const MapPoint &start, const MapPoint &dest, Path &good_path,
                          bool &is_broken_path, Path &bad_path,
                          float &total_cost, bool no_bad_path,
                          const sint32 cutoff, sint32 &nodes_opened);

    bool FindPath(Army &army, MapPoint const & start,
                  const PLAYER_INDEX owner, MapPoint const & dest, Path &new_path,
                  bool &is_broken_path, Path &bad_path,
                  float &total_cost);

    bool StraightLine(const MapPoint &start, const MapPoint &dest, Path &new_path) const;

    bool IsBeachLanding(const MapPoint &prev,
                        const MapPoint &pos,
                        const uint32   &m_move_intersection);
    void ClearMem();
    bool VerifyMem() const;
};

#endif
