











#ifndef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ROBOT_ASTAR_H__
#define __ROBOT_ASTAR_H__ 1


#include "UnitAstar.h"
#include "IC3RobotAstar.h"

class Player; 
class CivArchive; 

class RobotAstar :  public IC3RobotAstar, public UnitAstar

{
#ifndef USE_COM_REPLACEMENT
	ULONG m_refCount;
#else
	uint32 m_refCount;
#endif

    Player *m_player; 
    RobotPathEval *m_callback; 

public:
#ifndef USE_COM_REPLACEMENT
	STDMETHODIMP QueryInterface(REFIID, void **obj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
#else
	virtual uint32 AddRef();
	virtual uint32 Release();
#endif
    

    RobotAstar(Player *p); 
    RobotAstar(Player *p, CivArchive &archive); 
    virtual ~RobotAstar();
    
    void Serialize(CivArchive &archive); 

    sint32 EntryCost(const MapPoint &prev, const MapPoint &pos,
      float &cost, BOOL &is_zoc, ASTAR_ENTRY_TYPE &entry);        

	void RecalcEntryCost(AstarPoint *parent, 
		AstarPoint *node, float &new_entry_cost, 
		BOOL &new_is_zoc, ASTAR_ENTRY_TYPE &new_entry);





#ifndef USE_COM_REPLACEMENT
    STDMETHODIMP_ (BOOL) FindPath(RobotPathEval *cb, 
#else
	virtual BOOL FindPath(RobotPathEval *cb, 
#endif
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

#endif // __ROBOT_ASTAR_H__
