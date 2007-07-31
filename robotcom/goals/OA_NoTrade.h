
#pragma once

#ifndef __OA_DONT_TRADE_WITH_ENEMY_H__
#define __OA_DONT_TRADE_WITH_ENEMY_H__ 1

#include "OA.h"

class Opportunity_Action_Dont_Trade_With_Enemy : public Opportunity_Action { 

public:

    Opportunity_Action_Dont_Trade_With_Enemy(); 
    ~Opportunity_Action_Dont_Trade_With_Enemy();

    void Serialize (CivArchive &archive); 
	void Execute(AiMain *ai); 
};

#endif __OA_DONT_TRADE_WITH_ENEMY_H__
