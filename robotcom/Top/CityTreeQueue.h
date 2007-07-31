




#ifndef __CityTreeQueue_H__
#define __CityTreeQueue_H__ 1

class CivArchive; 


class CityTreeQueueFlat{ 
protected:
    sint32 m_count; 
public:
	CityTreeQueueFlat(); 
	void Serialize(CivArchive &archive); 
};



class CityTree; 

class CityTreeQueuePtr { 
protected:
    CityTree *m_head; 
    CityTree *m_tail; 
public:

	CityTreeQueuePtr(); 
    ~CityTreeQueuePtr(); 
	void Serialize(CivArchive &archive);
	void Store(CivArchive &archive);
    void Load(CivArchive &archive, sint32 pcount, uint8 nmask[]);
};


class CityTreeQueue : public CityTreeQueueFlat , public CityTreeQueuePtr { 

public:


    CityTreeQueue(); 
    CityTreeQueue(CivArchive &archive); 
    ~CityTreeQueue(); 
    void Serialize(CivArchive &archive); 



    void Enqueue (CityTree * t); 
    void Dequeue (CityTree *&t);
        
    CityTree *RemoveLargest();
    void Sort();

    void Remove (CityTree *t); 
    CityTree * Peek() { Assert(m_head); return m_head; } 

    sint32 Len() const { return m_count; }
    void CleanUp(); 

    void Dump(); 
};

#endif __CityTreeQueue_H__


