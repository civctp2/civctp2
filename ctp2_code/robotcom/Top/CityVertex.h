


#ifndef __CityVertex_H__
#define __CityVertex_H__ 1

class CivArchive; 


class CityVertexFlat{ 
protected:
    sint32 m_edge_count; 

public:
	CityVertexFlat(); 
	void Serialize(CivArchive &archive); 
};


class CityEdge; 
class CityTree; 
class CityVertex; 
class Point2d;

class CityAgent;

class CityVertexPtr { 
protected:
    CityAgent *m_agent; 
    CityEdge *m_edge_head; 
    CityTree *m_tree; 

    CityVertex *m_next; 
    Point2d *m_point; 

public:

    friend CityEdge; 

	CityVertexPtr(); 
    ~CityVertexPtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};
struct MapPointData; 


class CityVertex : public CityVertexFlat , public CityVertexPtr { 

public:


    CityVertex(); 
    CityVertex(MapPointData &pos); 
    CityVertex(CityAgent *ca); 
    CityVertex(const double sx, const double sy); 

    CityVertex(CivArchive &archive); 
    ~CityVertex(); 
    void Serialize(CivArchive &archive); 


    void* operator new(size_t); 
    void operator delete(void *);


    friend class CityTree; 
    friend class CityTreeQueue; 
    friend class FS_MinSpanTree; 

    void SetTree(CityTree *t) { Assert(t); m_tree = t; } 
    CityTree * GetTree() const { return m_tree; } 
    void AddEdge(CityEdge *e); 

    void GetPos(double &px, double &py);
    CityAgent *GetAgent(); 

    void Dump();
};

#endif __CityVertex_H__
