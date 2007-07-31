



#include "c3.h"
#include "globals.h"
#include "IC3GameState.h"
#include "AiMain.h"

#include "CityTreeQueue.h"
#include "FlatPtr.h"
#include "CivArchive.h"
#include "CityTree.h"
#include "CityVertex.h"
#include "CityAgent.h"

ZEROMEM(CityTreeQueueFlat); 
ZEROMEM(CityTreeQueuePtr); 
ZERODEL(CityTreeQueuePtr); 
FLATSERIALIZE(CityTreeQueueFlat); 
PTRSERIALIZE(CityTreeQueuePtr); 

CityTreeQueue::CityTreeQueue()
{ 
}

CityTreeQueue::CityTreeQueue(CivArchive &archive)
{ 
    Serialize(archive); 
} 

CityTreeQueue::~CityTreeQueue()
{ 
} 

void CityTreeQueuePtr::Store (CivArchive &archive)
{
}


void CityTreeQueuePtr::Load(CivArchive &archive, sint32 pcount, uint8 nmask[])
{
}

void CityTreeQueue::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE 

    CityTreeQueueFlat::Serialize(archive); 
    CityTreeQueuePtr::Serialize(archive); 
}


void CityTreeQueue::Enqueue (CityTree * t)
{
    if (m_head == NULL) { 
        Assert(m_tail == NULL); 
        m_head = m_tail = t; 
    } else { 
        Assert(m_tail != NULL); 
        m_tail->m_next = t; 
        m_tail = t; 
    } 

    m_count++; 
}

CityTree *CityTreeQueue::RemoveLargest()
{
    Assert(m_head); 

    CityTree *largest = m_head; 
    CityTree *largest_prev=NULL; 
    CityTree *p=m_head->m_next; 
    CityTree *prev=m_head; 
    while(p) {  
        if (largest->IsLarger(p)) {
            largest_prev = prev; 
            largest = p;
        } 
        prev = p; 
        p = p->m_next;
    }


    if (largest == m_head) { 
        Assert(largest_prev == NULL);
        m_head = largest->m_next; 
        if (m_head == NULL) { 
            m_tail = NULL; 
        } 
    } else if (largest == m_tail) { 
        largest_prev->m_next = NULL; 
        m_tail = largest_prev;         
    } else { 
        largest_prev->m_next = largest->m_next; 
    } 

    return largest; 
}

void CityTreeQueue::Sort()
{ 
    CityTree *the_tree=NULL; 
    CityTree *sort_head=NULL, *sort_tail=NULL; 

    
    while (m_head) { 
        the_tree = RemoveLargest(); 

        if (sort_head==NULL) { 
            sort_head = the_tree;
            sort_tail = the_tree; 
        } else { 
            sort_tail->m_next = the_tree; 
            sort_tail = the_tree; 
        }
    }

    m_head = sort_head; 
    m_tail = sort_tail; 
} 

void CityTreeQueue::Dequeue (CityTree *&t)
{

    t = m_head; 
    m_head = m_head->m_next; 
    if (m_head == NULL) { 
        m_tail = NULL; 
    } 
    m_count--;
    t->m_next = NULL; 
}

void CityTreeQueue::Remove (CityTree *t)
{
    CityTree *p=NULL; 

    Assert(m_head); 
    if (m_head == t) { 
        m_head = m_head->m_next; 
        if (m_head == NULL) { 
            m_tail = NULL; 
        } 
        t->m_next=NULL;
        m_count--;
        return;
    } else { 
        for (p=m_head; p; p = p->m_next) { 
            if (p->m_next == t) { 
                p->m_next = t->m_next; 
                if (p->m_next == NULL) { 
                    m_tail = p;
                }  
                t->m_next = NULL; 
                m_count--;
                return; 
            }
        } 
    } 
    INSANE(COULD_NOT_REMOVE_TREE); 
}

void CityTreeQueue::CleanUp()
{ 

    CityTree *t=NULL; 
    CityTree *c=NULL;

    for (t=m_head; t; t = t->m_next) { 
        for (c = m_head; c; c = c->m_next) { 
            c->ResetMinEdgeWeight(); 
        }
        t->CleanUp(); 
    }
} 


void CityTreeQueue::Dump()
{

    FILE *fout; 

    fout = fopen ("logs\\mst.txt", "a"); 
    fprintf (fout, "Queue size %d: ", m_count); 

    CityTree *t=NULL; 
    for (t = m_head; t; t = t->m_next) { 
        fprintf (fout, "(%x %d %3.1f)", t->m_city_head->m_agent->m_id.GetVal(), t->m_stage, t->m_min_edge_weight); 
    } 
    fprintf (fout, "\n"); 
    fclose(fout); 
}
