
#pragma once

#ifndef __OA_PILLAGE__
#define __OA_PILLAGE__ 1

#include "OA.h"

class Opportunity_Action_Pillage : public Opportunity_Action { 

public:


    Opportunity_Action_Pillage (); 
    ~Opportunity_Action_Pillage ();

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 
}; 

#endif __OA_LOOT__
