//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : A-star pathfinding for Units
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
// - Added avoidList Param
// - Added methods to check room for army and danger along path
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __UNIT_ASTAR__
#define __UNIT_ASTAR__ 1


#include "Astar.h"
#include "Army.h"

class Cell; 
class CityData; 
class UnitAstar : public Astar

{

    uint32 m_move_union;
    uint32 m_move_intersection; 
    sint32 m_max_dir;
    uint32 m_mask_alliance;

    MapPoint m_dest; 

    BOOL m_no_bad_path;
    BOOL m_is_zero_attack; 
    PLAYER_INDEX m_owner;
   
    sint32 m_nUnits;
	
protected:
    Army m_army;
    
    float m_army_minmax_move;
    BOOL m_can_space_launch; 
    BOOL m_can_space_land; 
	BOOL m_can_be_cargo_podded;
    BOOL m_check_rail_launchers; 
	BOOL m_ignore_zoc;
    BOOL m_is_cargo_pod;
    BOOL m_cargo_pod_intersection;
	BOOL m_check_dest;
    BOOL m_check_units_in_cell;
    BOOL m_is_robot; 
    BOOL m_army_can_expel_stealth; 
	MapPoint m_start; 

public:

    UnitAstar(); 

    float ComputeValidMovCost(const MapPoint &pos, Cell *the_pos_cell);

    BOOL CheckUnexplored(const MapPoint &prev, const MapPoint &pos,                           
        Cell *the_prev_cell, Cell *the_pos_cell, 
        float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    BOOL CheckUnits(const MapPoint &prev, const MapPoint &pos,                           
        Cell *the_prev_cell, Cell *the_pos_cell, 
        float &cost, BOOL &is_zoc, BOOL &can_be_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    BOOL CheckHisCity(const MapPoint &prev, const MapPoint &pos,                           
        Cell *the_prev_cell, Cell *the_pos_cell, 
        CityData *the_pos_city, 
        float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    BOOL CheckHeight(const MapPoint &prev, const MapPoint &pos,                           
        Cell *the_prev_cell, Cell *the_pos_cell, 
        float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    BOOL CheckMyCity(const MapPoint &prev, const MapPoint &pos,                           
        Cell *the_prev_cell, Cell *the_pos_cell, 
        CityData *the_pos_city, 
        float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    BOOL CheckMoveUnion(const MapPoint &prev, const MapPoint &pos,
        Cell *the_prev_cell, Cell *the_pos_cell, 
        float &cost, BOOL &is_zoc, const BOOL can_be_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    BOOL CheckMoveIntersection(const MapPoint &prev, const MapPoint &pos,                           
        Cell *the_prev_cell, Cell *the_pos_cell, 
        float &cost, BOOL &is_zoc, const BOOL can_be_zoc, ASTAR_ENTRY_TYPE &entry, BOOL &can_enter);

    virtual sint32 EntryCost(const MapPoint &prev, const MapPoint &pos,
       float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry);        

    sint32 InitPoint(AstarPoint *parent, AstarPoint *point, const MapPoint &pos, 
        const float pc, const MapPoint &dest);
	BOOL CheckIsDangerForPos(const MapPoint & myPos, const BOOL IsCivilian);

    void RecalcEntryCost(AstarPoint *parent, AstarPoint *node, float &new_entery_cost, 
        BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &entry);

    sint32 GetMaxDir(MapPoint &pos) const;
    BOOL CanMoveIntoTransports(const MapPoint &pos);
	BOOL CanRailLaunch(const MapPoint &spos);
    BOOL CanSpaceLaunch();
    BOOL CanSpaceLand();

	float EstimateFutureCost(const MapPoint &pos, const MapPoint &dest);

    
    void InitArmy(const Army &army, sint32 &nUnints, 
    uint32 &move_intersection,  uint32 &move_union, float &army_minmax_move);


    void InitSearch( const MapPoint &start, const PLAYER_INDEX owner, 
    const MapPoint &dest, Path &good_path, sint32 &is_broken_path, Path &bad_path);

    BOOL EnterPathPoints(Path &a_path, MapPoint &old);
    BOOL FindVisionEdge(Path &a_path, MapPoint &old);

    BOOL PretestDest_Enterable(const MapPoint &start, const MapPoint &dest);
    BOOL PretestDest_HasRoom(const MapPoint &start, const MapPoint &dest);
    BOOL PretestDest_SameLandContinent(const MapPoint &start, const MapPoint &dest);
    BOOL PretestDest_SameWaterContinent(const MapPoint &start, const MapPoint &dest);
    BOOL PretestDest_ZocEnterable(const MapPoint &start, const MapPoint &dest);

    BOOL PretestDest(const MapPoint &start, const MapPoint &dest);


    sint32 FindBrokenPath(const MapPoint &start, const MapPoint &dest, Path &good_path, 
        Path &bad_path, float &total_cost);

    sint32 FindStraightPath(const MapPoint &start, const MapPoint &dest, Path &good_path, 
                           sint32 &is_broken_path, Path &bad_path, 
                           float &total_cost, BOOL no_bad_path, 
                           const sint32 cutoff, sint32 &nodes_opened);

    sint32 FindPath(Army &army, MapPoint const & start,
		  const PLAYER_INDEX owner, MapPoint const & dest, Path &new_path, 
          sint32 &is_broken_path, Path &bad_path,  
         float &total_cost);

    sint32 FindPath(Army army, sint32 nUnits, uint32 move_intersetion, 
        uint32 move_union, const MapPoint &start,
        const PLAYER_INDEX owner, const MapPoint &dest, Path &good_path, 
        sint32 &is_broken_path, Path &bad_path, 
        float &total_cost, const BOOL no_bad_path, 
        const BOOL check_rail_launcher, 
        const BOOL pretty_path, const sint32 cutoff, sint32 &nodes_opened,
		const BOOL &check_dest, const BOOL no_straight_lines, 
        const BOOL check_units_in_cell);

 
    sint32 StraightLine(const MapPoint &start, const MapPoint &dest, Path &new_path) const;

	BOOL IsBeachLanding(const MapPoint &prev,
						const MapPoint &pos,
						const uint32 &m_move_intersection);
    void ClearMem(); 
    BOOL VerifyMem() const; 
};

#endif
