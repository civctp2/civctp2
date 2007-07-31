
#pragma once

#ifndef __OA_REFUEL__
#define __OA_REFUEL__ 1

#include "OA.h"

class Opportunity_Action_Refuel : public Opportunity_Action { 

public:


    Opportunity_Action_Refuel (); 
    ~Opportunity_Action_Refuel ();

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 

    void FindAircraftCarriers(AiMain *ai, ArmyAgent *current_agent, BOOL &searching, MapPointData &dest_pos);
    void FindCityAirports(AiMain *ai, ArmyAgent *current_agent, BOOL &searching, MapPointData &dest_pos);
    void FindAirfields(AiMain *ai, ArmyAgent *current_agent, BOOL &searching, MapPointData &dest_pos);


    void SendPlaneToCity(AiMain *ai, ArmyAgent *the_agent, MapPointData &dest_pos);

}; 

#endif __OA_REFUEL__
