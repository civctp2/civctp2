
#pragma once 

#ifndef __BSET_H__
#define __BSET_H__ 1

struct IC3CivArchive; 


    class CivArchive; 


enum BSET_STACK { 
    BSET_CUR_UNDONE, 
    BSET_CUR_DONE, 
    BSET_LESSER_DONE, 
    BSET_GREATER_DONE, 

}; 

class BSetID { 

    uint32 val; 

public:

    BSetID (); 
    BSetID (IC3CivArchive *);
    BSetID (const int &copy_me);
    BSetID (const uint32 &copy_me); 
    BSetID (const sint32 &copy_me); 
    BSetID (const BSetID &copy_me); 

    BSetID& operator= (const int &copy_me); 
    BSetID& operator= (const uint32 &copy_me); 
    BSetID& operator= (const sint32 &copy_me); 
    BSetID& operator= (const BSetID &copy_me); 

    BSetID& operator-- (); 
    BSetID& operator++ (); 

    BOOL operator== (const BSetID &test_me) const; 
    BOOL operator!= (const BSetID &test_me) const; 

    BOOL operator< (const BSetID &test_me) const; 
    BOOL operator<= (const BSetID &test_me) const; 
    BOOL operator> (const BSetID &test_me) const; 
    BOOL operator>= (const BSetID &test_me) const; 

    uint32 GetVal() const { return val; }
    void Castrate(); 
    void DelPointers();

    void Serialize(CivArchive &archive); 

    void Serialize(IC3CivArchive *archive); 
};

template <class T> class BSet; 

template <class T> class BSetNode { 

	BSetID m_id; 
	BSetNode *m_lesser, *m_greater; 
	T *m_data; 

public:

    BSetNode();
    ~BSetNode();    

    BSetNode(BSetID id, T *d);
    void SetId(uint32 val) { m_id = val; }
    void SetId(BSetID val) { m_id = val; }
    void SetData(T* d);
    
    friend BSet<T>; 
}; 

template <class T> BSetNode<T>::BSetNode()
{
    m_id=BSetID(0); 
	m_lesser = NULL; 
    m_greater = NULL; 
	m_data = NULL; 
}

template <class T> BSetNode<T>::~BSetNode()

{ 
    if (m_lesser) { 
        delete m_lesser; 
        m_lesser = NULL; 
    } 

    if (m_greater) { 
        delete m_greater; 
        m_greater = NULL; 
    }

    delete m_data; 
    m_data = NULL; 

}

template <class T> BSetNode<T>::BSetNode(BSetID id, T *d)

{ 
    m_id = id; 
    m_lesser = NULL; 
    m_greater = NULL; 
    m_data = d; 
} 

template <class T> void BSetNode<T>::SetData(T* d)
{
    m_data = d; 
}

template <class T> class BSet { 

    sint32 m_num;
    uint32 m_nInserted; 
	BSetNode<T> *m_root; 

    sint32 m_stack_size; 
    BSetNode<T> **m_stack; 
    BSET_STACK *m_stack_state; 
    sint32 m_stack_head; 

    void RecurseDel(BSetNode<T> *p);

    void TestStackSize(); 

    void NextLesser(BSetID &id);    
    void  NextGreater(BSetID &id);

    void Init();

public: 

    BSet(); 
    ~BSet();

    void Clear(); 
    void RecurseSerialize(IC3CivArchive *archive, BSetNode<T> *p);
    void Serialize(IC3CivArchive *archive); 

    sint32 GetNum() const { return m_num; } 
    uint32 GetCumNumInserted() { return m_nInserted; } 

    BSetID RawInsert(BSetNode<T> *p, BSetNode<T> *i);
    BSetID InsertNode(BSetNode<T> *an); 
    BSetID Insert(T *data); 

    T* First(BSetID &id); 
    BOOL Last() const;
    T *Next(BSetID &id); 

    
    T *Find(const BSetID &id);
    BOOL Del(const BSetID &id);

}; 


template <class T> BSet<T>::BSet() 

{
    Init(); 
}


template <class T> void BSet<T>::Init()
{ 
    m_num=0; 
    m_nInserted = 1; 
    m_root = NULL; 

    m_stack_size = 10; 
    m_stack = new BSetNode<T>*[m_stack_size]; 
    m_stack_state = new BSET_STACK[m_stack_size]; 

    sint32 i; 
    for (i=0; i<m_stack_size; i++) { 
        m_stack_state[i] = BSET_GREATER_DONE; 
        m_stack[i] = NULL; 
    } 

    m_stack_head = -1; 
} 

template <class T> BSet<T>::~BSet()
{
    delete[] m_stack_state;
    m_stack_state = NULL; 
    delete[] m_stack; 
    m_stack = NULL; 

    if (m_root) { 
        delete m_root; 
        m_root = NULL; 
    } 
}

extern uint32 BitsFlip(uint32 val);

template <class T> void BSet<T>::Serialize(IC3CivArchive *archive)
{ 
    sint32 i; 
    BSetNode<T> *p; 

    if (archive->IsStoring()) { 
        archive->Store((uint8*)&m_num, sizeof(m_num));
        archive->Store((uint8*)&m_nInserted, sizeof(m_nInserted));
        
        if (m_root) { 
            RecurseSerialize(archive, m_root); 
        }
    } else { 
        archive->Load((uint8*)&m_num, sizeof(m_num));
        archive->Load((uint8*)&m_nInserted, sizeof(m_nInserted));  


        if (0 < m_num) { 
            m_root = new BSetNode<T>; 
            m_root->m_id.Serialize(archive); 
            m_root->m_data = new T(archive); 
          
    
            for (i=1; i<m_num; i++) { 
                p = new BSetNode<T>; 
                p->m_id.Serialize(archive); 
                p->m_data = new T(archive); 
              
                RawInsert(m_root, p); 
            }
        }
    }
} 

template <class T> void BSet<T>::RecurseSerialize(IC3CivArchive *archive, BSetNode<T> *p)
{
    p->m_id.Serialize(archive); 
    p->m_data->Serialize(archive); 

    if (p->m_lesser) { 
        RecurseSerialize(archive, p->m_lesser); 
    } 

    if (p->m_greater) { 
        RecurseSerialize(archive, p->m_greater); 
    } 
}

template <class T> BSetID BSet<T>::RawInsert(BSetNode<T> *p, BSetNode<T> *i)

{ 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while(1) { 
Assert(++finite_loop < 100000); 
        if (i->m_id < p->m_id) { 
            if (p->m_lesser ==NULL) { 
                p->m_lesser = i; 
                break; 
            } else {
                p = p->m_lesser;
            } 
        } else if (p->m_id < i->m_id) { 
            if (p->m_greater ==NULL) { 
                p->m_greater = i; 
                break; 
            } else {
                p = p->m_greater;
            } 
        } else { 
            Assert(0); 
            return BSetID(0); 
        } 
    } 
    return i->m_id; 
}

template <class T>  BSetID BSet<T>::InsertNode(BSetNode<T> *an)
{
    m_num++; 
    m_nInserted++; 

    if (m_root == NULL) {
        m_root = an; 
        return  an->m_id; 
    } 

    return RawInsert(m_root, an);  

}

template <class T>  BSetID BSet<T>::Insert(T *data)
{

    BSetID id; 

    id = BitsFlip(m_nInserted); 

    BSetNode<T> *i = new BSetNode<T>(id, data);     
    return InsertNode(i); 

}


template <class T> void BSet<T>::TestStackSize()

{ 
    
    BSetNode<T> **tmp_stack; 
    BSET_STACK *tmp_stack_state; 

    if ((m_stack_size-1) <= m_stack_head) {
        tmp_stack = new BSetNode<T>*[m_stack_size*2];
        memcpy (tmp_stack, m_stack, m_stack_size * sizeof(BSetNode<T>*)); 
        delete[] m_stack;
        m_stack = tmp_stack; 
   
        tmp_stack_state = new BSET_STACK[m_stack_size*2]; 
        memcpy (tmp_stack_state, m_stack_state, m_stack_size*sizeof(sint32));
        delete[] m_stack_state; 
        m_stack_state = tmp_stack_state;

        m_stack_size *= 2; 
    }
} 

template <class T> T* BSet<T>::First(BSetID &id) 
{
    if (m_root == NULL) {
        m_stack_head = -1; 
        m_stack[0] = NULL; 
        m_stack_state[0] = BSET_CUR_DONE; 

        id = 0; 
        return NULL; 
    } 

    m_stack_head = 0; 
    m_stack[m_stack_head] = m_root; 
    m_stack_state[m_stack_head] = BSET_CUR_UNDONE; 
    return Next(id); 
}

template <class T> BOOL BSet<T>::Last() const
{   return 0 <= m_stack_head;
}

template <class T> void BSet<T>::NextLesser(BSetID &id) 
{ 
   TestStackSize(); 
   m_stack_state[m_stack_head] = BSET_LESSER_DONE;
   m_stack[m_stack_head+1] = m_stack[m_stack_head]->m_lesser;
   m_stack_head++; 
   m_stack_state[m_stack_head] =  BSET_CUR_UNDONE; 
   id = m_stack[m_stack_head]->m_id;
}

template <class T> void BSet<T>::NextGreater(BSetID &id) 
{ 
   TestStackSize(); 
   m_stack_state[m_stack_head] = BSET_GREATER_DONE;
   m_stack[m_stack_head+1] = m_stack[m_stack_head]->m_greater;
   m_stack_head++; 
   m_stack_state[m_stack_head] =  BSET_CUR_UNDONE; 
   id = m_stack[m_stack_head]->m_id; 
}

template <class T> T *BSet<T>::Next(BSetID &id)
{

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (0 <= m_stack_head) { 
Assert(++finite_loop < 100000); 
        switch (m_stack_state[m_stack_head]) { 
        case BSET_CUR_UNDONE: 
            if (m_stack[m_stack_head]->m_lesser) {          
                NextLesser(id);  
                break; 
            } 
            
        case BSET_LESSER_DONE: 
            m_stack_state[m_stack_head] = BSET_CUR_DONE;           
            id = m_stack[m_stack_head]->m_id; 
            return m_stack[m_stack_head]->m_data;

        case BSET_CUR_DONE: 
            if (m_stack[m_stack_head]->m_greater) { 
                NextGreater(id);          
                break; 
            } 
            
        case BSET_GREATER_DONE: 
            m_stack_head--; 
            break; 
        default: 
            Assert(0); 
        } 
    } 
    return NULL;
}

template <class T> T *BSet<T>::Find(const BSetID &id)

{
    if (m_root == NULL) 
        return NULL; 

    BSetNode<T> *p = m_root; 
#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (TRUE) { 
Assert(++finite_loop < 100000); 
        if (p->m_id == id) { 
            return p->m_data; 
        } else if (id < p->m_id) { 
            if (p->m_lesser) { 
                p = p->m_lesser; 
            } else { 
                return NULL; 
            } 
        } else { 
            if (p->m_greater) { 
                p = p->m_greater; 
            } else { 
                return NULL; 
            } 
        }
    }
    Assert(0); 
}

template <class T> BOOL BSet<T>::Del(const BSetID &id)

{
    if (m_root == NULL) 
        return FALSE; 

    BSetNode<T> **p = &m_root; 
    BSetNode<T> *del_me; 

#ifdef _DEBUG
    sint32 finite_loop=0; 
#endif

    while (TRUE) { 
Assert(++finite_loop < 100000); 
        if ((*p)->m_id == id) { 
            del_me = *p; 
            if ((*p)->m_lesser) { 
               if ((*p)->m_greater) { 
                    RawInsert((*p)->m_lesser, (*p)->m_greater); 
               }
               *p = (*p)->m_lesser; 
            } else if ((*p)->m_greater) { 
                *p = (*p)->m_greater; 
            } else { 
                *p = NULL; 
            }

            del_me->m_lesser = NULL;  
            del_me->m_greater = NULL; 

            delete del_me; 
            del_me = NULL; 
            m_num--; 
            return TRUE; 
        } else if (id < (*p)->m_id) { 
            if ((*p)->m_lesser) { 
                p = &((*p)->m_lesser); 
            } else { 
                return FALSE; 
            } 
        } else { 
            if ((*p)->m_greater) { 
                p = &((*p)->m_greater); 
            } else { 
                return FALSE; 
            } 
        }
    }
    Assert(0); 
    return FALSE; 
}

template <class T> void BSet<T>::Clear()
{

    m_nInserted = 0; 
    m_num = 0; 
    m_stack_head = 0; 
    if (m_root) { 
        delete m_root; 
        m_root = NULL; 
    } 
}

#endif __BSET_H__
