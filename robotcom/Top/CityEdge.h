


#ifndef __CityEdge_H__
#define __CityEdge_H__ 1

class CivArchive; 


class CityEdgeFlat{ 
protected:
    double m_weight; 
public:
	CityEdgeFlat(); 
	void Serialize(CivArchive &archive); 
};


class CityEdge; 
class CityVertex; 

class CityEdgePtr { 
protected:
    CityVertex *m_start; 
    CityVertex *m_dest; 

    CityEdge *m_next; 
public:

	CityEdgePtr(); 
    ~CityEdgePtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);

};

#define k_MAX_EDGE 1000000.0


class CityAgent; 
class CityEdge : public CityEdgeFlat , public CityEdgePtr { 

    friend class CityVertex; 
    friend class FS_MinSpanTree; 
    friend class CityTree; 

public:


    CityEdge(); 
    CityEdge(CivArchive &archive); 
    CityEdge(const double w, CityVertex *start_v, CityVertex *dest_v);
    ~CityEdge(); 
    void Serialize(CivArchive &archive); 

    void* operator new(size_t); 
    void operator delete(void *);



    CityVertex * GetDest() const { return m_dest; } 
    double GetWeight() const { return m_weight; } 
    
    CityAgent *GetStartCityAgent(); 
    CityAgent *GetDestCityAgent(); 

    void Dump(); 

};

#endif __CityEdge_H__



