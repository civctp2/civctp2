
#pragma once


#ifndef __GOAL_OVERTIME_H__
#define __GOAL_OVERTIME_H__ 1

class AiMain;
class BSetID; 
class CivArchive; 
class CityAgent; 
template <class T> class BSet;

class GoalOvertimeFlat { 
protected: 
    double m_utility; 
    double m_cost; 
    sint32 m_satisfied; 
public:
    GoalOvertimeFlat(); 
    void Serialize(CivArchive &archive); 
}; 

class GoalOvertimePtr { 
protected:
    	BSetID *m_city_id; 
	CityAgent *m_home_city; 
public:
    GoalOvertimePtr(); 
    void Serialize(CivArchive &archive);     
    void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
}; 

class GoalOvertime : public GoalOvertimeFlat, public GoalOvertimePtr { 
public:

	GoalOvertime(AiMain *init_ai, CityAgent *ca); 
	GoalOvertime(BSet<CityAgent> *, CivArchive &archive); 
	~GoalOvertime(); 
	void Serialize(BSet<CityAgent> *, CivArchive &archive); 

    double GetUtility() const { return m_utility; } 
    double GetCost() const { return m_cost; }
    BOOL GetIsSatisfied() const { return m_satisfied; } 

    void Spend(AiMain *ai, double &gold, double &cost); 

    CityAgent *GetHomeCity(); 

}; 

#endif __GOAL_OVERTIME_H__
