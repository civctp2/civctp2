
#pragma once

#ifndef __OA_REPAIR_INSTALLATION__
#define __OA_REPAIR_INSTALLATION__ 1

#include "OA.h"

#include "MapPoint.h"

struct MapPointData; 

class Opportunity_Action_Repair_Installation;

class Pillaged_Node
{
    MapPointData m_pos; 
    sint32 m_fix_me_by; 
    sint32 m_inst; 
    sint32 m_attempt_count; 
    Pillaged_Node *m_next; 
    
public:
    Pillaged_Node(MapPointData &pos, sint32 t, sint32 i); 
    ~Pillaged_Node(); 

    Pillaged_Node(CivArchive &archive);
    void Serialize(CivArchive &archive);

    friend Opportunity_Action_Repair_Installation; 
}; 

class Opportunity_Action_Repair_Installation : public Opportunity_Action {

    sint32 m_dirty_tile_count; 
    sint32 m_queue_len;

    Pillaged_Node *m_head; 
    Pillaged_Node *m_tail; 

public:

	Opportunity_Action_Repair_Installation();
	~Opportunity_Action_Repair_Installation(); 

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 

    void RegisterHostileAction(AiMain *ai, MapPointData &pos); 
    void RegisterPollution(AiMain *ai, MapPointData &pos);

    Pillaged_Node* Dequeue();
    void Enqueue(Pillaged_Node* add_me); 

    sint32 GetDirtyTileCount() const { return m_dirty_tile_count; }

};

#endif __OA_REPAIR_INSTALLATION__
