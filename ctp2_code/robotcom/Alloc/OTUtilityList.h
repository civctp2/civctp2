
#pragma once

#ifndef __OVERTIME_UTILITY_H__
#define __OVERTIME_UTILITY_H__ 1

#include "GlOvertime.h"

class CivArchive; 
class CityAgent; 
template <class T> class BSet; 
class GoalOvertime; 

class OvertimeUtilityList { 

	sint32 m_num_goals; 
    sint32 m_max_goals; 
	GoalOvertime **m_goal; 

public:
	OvertimeUtilityList(); 
	OvertimeUtilityList(BSet<CityAgent>*bs, CivArchive &archive); 
	~OvertimeUtilityList(); 

	void Serialize(BSet<CityAgent>*bs, CivArchive &archive);
    void DoubleGoal(); 
    void Insert(GoalOvertime *goal_obj);
    void GetMax(GoalOvertime *&max_goal, double &max_utility, double &min_cost);
    void Clear(); 

    void RegisterMyRemoveCity(const uint32 del_me); 
}; 

#endif __OVERTIME_UTILITY_H__