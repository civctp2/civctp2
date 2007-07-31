

#pragma once

#ifndef __OPPORTUNITY_ACTION_H__
#define __OPPORTUNITY_ACTION_H__ 1

class AiMain; 
class CivArchive; 

class Opportunity_Action { 

public:

    Opportunity_Action();
    ~Opportunity_Action();
    void Serialize(CivArchive &archive); 

    virtual void Execute(AiMain *ai) = 0; 

};

#endif __OPPORTUNITY_ACTION_H__