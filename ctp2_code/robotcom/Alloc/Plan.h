#ifndef __PLAN_H__
#define __PLAN_H__ 1






#include "common.h"
#include "robotcom/planner/linked_list.h"


#include "robotcom/planner/unitflow.h"
#include "robotcom/planner/goal.h"


#include "robotcom/planner/strategic_ai_forward.h"

#include "robotcom/planner/squad_strength.h"

class CivArchive;

template <class T> class DynamicArray;








class PlanFlat{
protected:




public:




	double matching_value;






	sint8 used;

	PlanFlat();
	void Serialize(CivArchive &archive);
};



























class Plan : public PlanFlat
{

public:




	squad_ptr the_squad;

	Goal_ptr the_goal;






    Squad_Strength m_needed_strength;





public:


    Plan();

    Plan(AiMain *ai,CivArchive &archive);
    ~Plan();
    void Serialize(AiMain *ai,CivArchive &archive);

	void Init();















    void UpdateNeededStrength();

    BOOL Plan_Is_Needed_And_Valid() const;
    BOOL AccumulateEnoughStrength(AiMain *ai);
    double GetMatchingValue() { return matching_value; }

};







#endif __PLAN_H__
