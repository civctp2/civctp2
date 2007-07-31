
#pragma once

#ifndef __OA_PARADROP__
#define __OA_PARADROP__ 1

#include "OA.h"
#include "DynArr.h"
#include "MapPoint.h"

class ArmyAgent; 
typedef struct { 
    MapPointData m_pos; 
    sint32 m_value; 
    sint32 m_count; 
    ArmyAgent *m_assigned[9]; 
} ParadropTarget; 

class ArmyAgent;

class Opportunity_Action_Paradrop : public Opportunity_Action { 

    DynamicArray<ParadropTarget> *m_target_list;
    DynamicArray<ArmyAgent*> *m_trooper_list; 
    
    void FindTargets(AiMain *ai);
    void AssignParatroopersToTargets(AiMain *ai);
    void DropTroopers(AiMain *ai);

public:


    Opportunity_Action_Paradrop (); 
    ~Opportunity_Action_Paradrop ();

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 

    void RegisterAddArmy (ArmyAgent *add_me);
    void RegisterMyRemoveArmy (const ArmyAgent *he_died); 
    
    void HookParatroops(AiMain *ai);

}; 

#endif __OA_PARADROP__