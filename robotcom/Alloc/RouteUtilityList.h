
#pragma once

#ifndef __ROUTE_UTILITY_LIST_H__
#define __ROUTE_UTILITY_LIST_H__ 1


class CivArchive; 
class GoalRoute; 
class CityAgent;
template <class T> class BSet; 


class RouteUtilityList { 
    
    sint32 m_num_goals; 
    sint32 m_max_goals; 
    GoalRoute **m_goal;
    void DoubleGoals();
public:

	RouteUtilityList(); 
    RouteUtilityList(BSet<CityAgent> *bs, CivArchive &archive); 
	~RouteUtilityList(); 
	void Serialize(BSet<CityAgent> *bs, CivArchive &archive); 
	void InsertGoal(GoalRoute*); 

    GoalRoute * GetGoal(sint32 i); 
    sint32 GetNumGoals(); 
    void Clear();
}; 

#endif __ROUTE_UTILITY_LIST_H__
