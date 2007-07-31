

#ifndef __CityList_H__
#define __CityList_H__ 1


class AiMain;
class CivArchive; 
class CityAgent; 

template <class T> class DynamicArray; 
template <class T> class BSet; 
  
class CityVal { 
public:
    uint32 m_id; 
    CityAgent *m_city_agent; 

    int operator< (CityVal &c);
    virtual void Serialize(BSet<CityAgent> *city_set, CivArchive &a) = 0; 
    virtual double GetVal()=0; 
    virtual CityAgent *GetAgent() const { return m_city_agent; }
}; 


class CityListFlat{ 
protected:
    sint32 m_median; 
    double m_average; 
public:
	CityListFlat(); 
	void Serialize(CivArchive &archive); 
};



#define k_BIG_TIME 100000.0

class CityListPtr { 
protected:
    DynamicArray<CityVal*> *m_list; 

public:

	CityListPtr(); 
    ~CityListPtr(); 
	void Serialize(BSet<CityAgent> *city_set, CivArchive &archive);
	void Store(BSet<CityAgent> *city_set, CivArchive &archive);
    void Load(BSet<CityAgent> *city_set, CivArchive &archive);
};

class BSetID; 


class CityList : public CityListFlat , public CityListPtr { 

public:


    CityList(); 
    CityList(BSet<CityAgent> *city_set, CivArchive &archive); 
    ~CityList(); 
    void Serialize(BSet<CityAgent> *city_set, CivArchive &archive); 



    void HeapSort();

    void Clear(); 
    void Insert(CityVal *ptr); 

    sint32 GetMedian() const { return m_median; } 
    double GetAverage() const { return m_median; } 
    double GetMin() const; 
    sint32 GetNumCities() const;
    BOOL GetCityIndex(const BSetID &city_id, sint32 &the_index) const;

    CityVal* GetCity(const sint32 idx_city) const;
    void FindMedianOnlyHappy(sint32 &median, const double fuzzy_horizontal, 
                          const double fuzzy_vertical);

    void FindMedian(sint32 &median, const double fuzzy_horizontal, 
                          const double fuzzy_vertical);

    BOOL IsAboveMedian(const BSetID &m_id) const;

    void CalcAverage(); 
};

#endif __CityList_H__

