
#pragma once

#ifndef __OA_LOOT__
#define __OA_LOOT__ 1

#include "OA.h"

class Opportunity_Action_Loot : public Opportunity_Action { 

public:


    Opportunity_Action_Loot (); 
    ~Opportunity_Action_Loot ();

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 
}; 

#endif __OA_LOOT__
