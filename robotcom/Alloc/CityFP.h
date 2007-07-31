
#ifndef __CityFP_H__
#define __CityFP_H__ 1


class AiMain;
class CivArchive; 
class CityAgent; 
template <class T> class DynamicArray; 
template <class T> class BSet; 
  
#include "CityList.h"

class FPVal : public CityVal { 
public:
    double m_fp_ratio;

    FPVal(); 
    FPVal(CityAgent *ca, double p);

    FPVal(BSet<CityAgent> *city_set, CivArchive &archive); 

    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive); 

    
    
    double GetVal(); 


}; 



class CityFP : public CityList { 

public:


    CityFP(); 
    CityFP(BSet<CityAgent> *city_set, CivArchive &archive); 
    ~CityFP(); 

    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive); 



    void InsertFP(CityAgent *ca, const double p); 
    
    void EstimateFP(AiMain *ai);
};

#endif __CityFP_H__

