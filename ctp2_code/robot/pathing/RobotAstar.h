












#pragma once
#ifndef __ROBOT_ASTAR_H__
#define __ROBOT_ASTAR_H__ 1


#include "UnitAstar.h"
#include "IC3RobotAstar.h"

class Player; 
class CivArchive; 

class RobotAstar :  public IC3RobotAstar, public UnitAstar

{
	ULONG m_refCount;

    Player *m_player; 
    RobotPathEval *m_callback; 

public:

    	
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

    

    RobotAstar(Player *p); 
    RobotAstar(Player *p, CivArchive &archive); 
    void Serialize(CivArchive &archive); 

    sint32 EntryCost(const MapPoint &prev, const MapPoint &pos,
      float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry);        

	void RecalcEntryCost(AstarPoint *parent, 
		AstarPoint *node, float &new_entry_cost, 
		BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry);






    STDMETHODIMP_ (BOOL) FindPath(RobotPathEval *cb, 
       uint32 army_id, PATH_ARMY_TYPE pat, uint32 army_type, 
       MapPointData *start, MapPointData *dest, sint32 *bufSize, 
	   MapPointData ** buffer, sint32 *nPoints,
	   float *total_cost, BOOL made_up_can_space_launch, 
       BOOL made_up_can_space_land, BOOL check_rail_launcher, 
       BOOL pretty_path, 
       sint32 cutoff, sint32 &nodes_opened, BOOL check_dest, 
        BOOL no_straight_lines, 
        const BOOL check_units_in_cell);

};

#endif __ROBOT_ASTAR_H__
