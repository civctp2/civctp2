
#ifndef __GOAL_TRANSPORT_H__
#define __GOAL_TRANSPORT_H__ 1

#include "ArmyGoal.h"

class CivArchive; 


class GoalTransportFlat{ 
protected:

public:
	GoalTransportFlat(); 
	void Serialize(CivArchive &archive); 
};


class GoalSettle; 
class GoalTransportPtr { 
protected:
    sint32 *dummy; 
public:

	GoalTransportPtr(); 
    ~GoalTransportPtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};


class GoalTransport : public ArmyGoal, public GoalTransportFlat , public GoalTransportPtr { 
public:


    GoalTransport(); 
    GoalTransport(GoalSettle *gs); 
    GoalTransport(AiMain *ai,CivArchive &archive); 
    ~GoalTransport(); 
    void Serialize(AiMain *ai,CivArchive &archive); 
    BOOL Validate(AiMain *ai); 




    GOAL_TYPE GetType() const; 
	void Display_Goal_Type(AiMain *ai);
};

#endif __GOAL_TRANSPORT_H__


