
#pragma once

#ifndef __OA_ENTRENCH__
#define __OA_ENTRENCH__ 1

#include "OA.h"

class Opportunity_Action_Entrench : public Opportunity_Action { 

public:


    Opportunity_Action_Entrench (); 
    ~Opportunity_Action_Entrench ();

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 
}; 

#endif __OA_ENTRENCH__
