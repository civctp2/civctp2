
#ifndef __CityGrowth_H__
#define __CityGrowth_H__ 1


class AiMain;
class CivArchive; 
class CityAgent; 
template <class T> class DynamicArray; 
template <class T> class BSet; 
  
#include "CityList.h"

class GrowTime : public CityVal { 
public:
    double m_turns_total; 
    double m_turns_remaining;

    GrowTime(); 
    GrowTime(CityAgent *ca, double tt, double tr);

    GrowTime(BSet<CityAgent> *city_set, CivArchive &archive); 
    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive); 

    
    
   
    
    double GetVal(); 


}; 



class CityGrowth : public CityList { 

public:


    CityGrowth(); 
    CityGrowth(BSet<CityAgent> *city_set, CivArchive &archive); 
    ~CityGrowth(); 

    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive); 



    void InsertTime(CityAgent *ca, double turns_total, 
                          double turns_remaining);

    void EstimateGrowthTime(AiMain *ai);

    void StopUnhappyGrowth(AiMain *ai);
};

#endif __CityGrowth_H__

