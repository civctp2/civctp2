
#ifndef __GOAL_GOOD_H__
#define __GOAL_GOOD_H__ 1


class CivArchive; 

class GoalRouteFlat {
protected:
    
    sint32 m_good_type; 
    double m_utility; 
    double m_cost; 
    sint32 m_is_satisfied;
public:
    GoalRouteFlat(); 
    ~GoalRouteFlat(); 
    void Serialize(CivArchive &archive); 
};

class AiMain;
class CityAgent; 

class BSetID; 
template <class T> class BSet; 

class GoalRoutePtr { 
protected:
    BSetID *m_city_id; 
    BSetID *m_dest_id; 
    CityAgent *m_city_agent; 
    CityAgent *m_dest_city; 
    

public:
    GoalRoutePtr(); 
    ~GoalRoutePtr(); 
    void Serialize(CivArchive &archive); 
    BOOL Validate(AiMain *ai); 


    void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
}; 

class GoalRoute : public GoalRouteFlat, public GoalRoutePtr{ 

public:

    GoalRoute(CityAgent *ca, sint32 good_type); 
    ~GoalRoute(); 
    GoalRoute (BSet<CityAgent> *bs, CivArchive &archive); 

    void Serialize(BSet<CityAgent> *bs, CivArchive &archive); 

    void EstimateRouteBenifit(AiMain *ai, CityAgent *ca, double &utility, double &cost);
    void SetDest(CityAgent *dest_city, double max_utility, double max_cost);
    void ConnectRoute(AiMain *ai, sint32 &freight); 
    double GetUtility() const { return m_utility; } 
    void AddToUtility(double a) { m_utility += a; } 
    void SetIsSatisfied(sint32 s); 
    BOOL IsSatisfied() const; 
    double GetCost() const; 
    sint32 GetGoodType() const; 
    CityAgent *GetCityAgent() { return m_city_agent; }
    CityAgent *GetDestAgent() { return m_dest_city; } 
}; 

#endif __GOAL_GOOD_H__