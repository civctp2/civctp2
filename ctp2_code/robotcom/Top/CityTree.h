


#ifndef __CityTree_H__
#define __CityTree_H__ 1

class CivArchive; 


class CityTreeFlat{ 
protected:
    sint32 m_stage; 
    double m_min_edge_weight;
    sint32 m_external_edge_count; 
public:
	CityTreeFlat(); 
	void Serialize(CivArchive &archive); 
};
class CityTree; 

class CityVertex; 
class CityEdge;
class CityTreePtr { 
protected:
    CityVertex *m_city_head; 
    CityEdge *m_edge_internal; 
    CityEdge *m_edge_external; 

    
    CityTree *m_next;
public:

	CityTreePtr(); 
    ~CityTreePtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};


class CityAgent;
class CityTree : public CityTreeFlat , public CityTreePtr { 

public:


    CityTree(); 
    CityTree(CityVertex *v); 
    CityTree(CivArchive &archive); 
    ~CityTree(); 
    void Del(); 

    void Serialize(CivArchive &archive); 


    void* operator new(size_t); 
    void operator delete(void *);


    friend class FS_MinSpanTree; 
    friend class CityTreeQueue; 
    sint32 GetStage() const { return m_stage; }

    void CleanUp(); 
    void GetMinEdge(CityEdge *&e, CityTree *&connect_me);
    void Merge(CityEdge *e, CityTree *connect_me);

    double GetMinEdgeWeight();
    void SetMinEdgeWeight(CityEdge *e); 
    void ResetMinEdgeWeight();
    CityEdge* TakeCheapestEdge();

    void RemoveCityEdge(CityVertex *remove_me);
    void RemoveCityReferance(CityAgent *remove_me); 
    
    BOOL IsLarger(CityTree *b);


    void Dump(); 
};

#endif __CityTree_H__


