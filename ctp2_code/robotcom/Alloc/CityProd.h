#ifndef __CityProduction_H__
#define __CityProduction_H__ 1

class AiMain;
class CivArchive;
class CityAgent;
template <class T> class DynamicArray;
template <class T> class BSet;

#include "CityList.h"

class ProdVal : public CityVal {
public:
    double m_net_production;

    ProdVal();
    ProdVal(CityAgent *ca, double p);

    ProdVal(BSet<CityAgent> *city_set, CivArchive &archive);
    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive);


    double GetVal();

};


class CityProduction : public CityList {

public:

    CityProduction();
    CityProduction(BSet<CityAgent> *city_set, CivArchive &archive);
    ~CityProduction();

    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive);


    void InsertProduction(CityAgent *ca, const double p);

    void EstimateProduction(AiMain *ai);
};

#endif __CityProduction_H__
