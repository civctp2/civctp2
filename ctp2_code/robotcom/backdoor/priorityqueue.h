
#pragma once

#ifndef __DA_PRIORITY_QUEUE__
#define __DA_PRIORITY_QUEUE__ 1

#include "dynarr.h"

template <class T> class DAPriorityQueue { 
	DynamicArray<T*> m_queue; 

    sint32 GetParent(const sint32 idx) const;
    sint32 GetLeftChild(const sint32 idx) const { 
        return idx<<1;
    }

    sint32 GetRightChild(const sint32 idx) const { 
        return (idx<<1) + 1;
    }

    void ShiftUp(const sint32 start_idx);
    void ShiftDown(const sint32 start_idx);

    void Swap(const sint32 a, const sint32 b);

public:
   
    DAPriorityQueue(); 
    ~DAPriorityQueue(); 

    void Insert(T *add_me);
    T* Remove(sint32 del_idx);

    sint32 Len();

    void LookForDups(const sint32 g);

    void Clear();

};


template <class T> DAPriorityQueue<T>::DAPriorityQueue()
{
    m_queue.ResizeFlat(1024); 
    m_queue.InsertFlat((T*)NULL); 
}

template <class T> DAPriorityQueue<T>::~DAPriorityQueue()
{
    
}

template <class T> sint32 DAPriorityQueue<T>::GetParent(const sint32 idx) const
{ 
    return idx>>1;
}












template <class T> void DAPriorityQueue<T>::ShiftUp(const sint32 start_idx)
{
    Assert(0 < start_idx); 

    sint32 current_idx = start_idx;
    
    sint32 parent_idx; 

#ifdef _DEBUG
    sint32 finite_count=0; 
#endif
    while (1) { 
Assert(finite_count++ < 100); 

        if (current_idx == 1) return; 

        parent_idx = GetParent(current_idx); 

        if ( *(m_queue[current_idx])  < *(m_queue[parent_idx])) { 
            Swap (parent_idx, current_idx); 
            current_idx = parent_idx; 
        } else { 
            return; 
        }
    } 
}

template <class T> void DAPriorityQueue<T>::ShiftDown(const sint32 start_idx)
{
    Assert(0 < start_idx); 

    register sint32 current_idx = start_idx;
    register sint32 right_idx; 
    register sint32 swap_me;

    T* swap_me_ptr=NULL; 
    T* right_ptr=NULL; 
    T* current_ptr=NULL; 

#ifdef _DEBUG
    sint32 finite_count=0; 
#endif

    sint32 N = m_queue.Num();
    while (1) { 

Assert(finite_count++ < 100); 

        if (N <= current_idx) return; 

        swap_me = GetLeftChild(current_idx); 
        right_idx = swap_me+1; 

        if (N <= swap_me) return; 

        swap_me_ptr = m_queue[swap_me]; 
        if (right_idx < N) {             
            right_ptr = m_queue[right_idx];
            if (*right_ptr < *swap_me_ptr) { 
                swap_me = right_idx; 
                swap_me_ptr = right_ptr; 
            }
       }

        current_ptr = m_queue[current_idx]; 
        if (*swap_me_ptr < *current_ptr) { 
            m_queue[current_idx] = swap_me_ptr;
            swap_me_ptr->SetPriorityQueueIndex(current_idx); 

            m_queue[swap_me] = current_ptr; 
            current_ptr->SetPriorityQueueIndex(swap_me); 

            current_idx = swap_me;
            current_ptr = swap_me_ptr; 
        } else { 
            return; 
        } 
    } 
}




















































template <class T> void DAPriorityQueue<T>::Swap(const sint32 a, const sint32 b)
{
    T* tmp_ptr; 

    tmp_ptr = m_queue[a]; 
    m_queue[a] = m_queue[b];
    m_queue[b] = tmp_ptr; 

    m_queue[a]->SetPriorityQueueIndex(a); 
    m_queue[b]->SetPriorityQueueIndex(b); 
}



template <class T> void DAPriorityQueue<T>::Insert(T *add_me)
{
    sint32 new_idx = m_queue.Num(); 
    
	
	
	

    Assert(add_me->GetPriorityQueueIndex() < 0); 
    m_queue.InsertFlat(add_me); 
    add_me->SetPriorityQueueIndex(new_idx); 

    ShiftUp(new_idx); 
}

template <class T> T* DAPriorityQueue<T>::Remove(sint32 del_idx)

{
    if (del_idx < 1)  { 
        Assert(0); 
        return NULL; 
    } else if (m_queue.Num() <= del_idx) { 
        Assert(0); 
        return NULL; 
    } 

    T* ret = m_queue[del_idx];

    if (NULL == ret) {
        Assert(0); 
        return NULL; 
    } 

    sint32 n = m_queue.Num()-1; 

    if (n == del_idx) { 
        
        m_queue[n]->SetPriorityQueueIndex(-1); 
Assert(ret->GetPriorityQueueIndex() < 0); 
        m_queue[n] = NULL; 
        m_queue.ShortenByOne();
Assert(ret->GetPriorityQueueIndex() < 0); 

    } else { 

        ret->SetPriorityQueueIndex(-1); 
Assert(ret->GetPriorityQueueIndex() < 0); 
Assert(m_queue[del_idx] != m_queue[n]);
        m_queue[del_idx] = m_queue[n]; 
        m_queue[del_idx]->SetPriorityQueueIndex(del_idx); 
        m_queue[n] = NULL; 
Assert(ret->GetPriorityQueueIndex() < 0); 
        m_queue.ShortenByOne(); 
Assert(ret->GetPriorityQueueIndex() < 0); 

        ShiftDown(del_idx); 
Assert(ret->GetPriorityQueueIndex() < 0); 
    }

Assert(ret->GetPriorityQueueIndex() < 0); 
    return ret; 
} 

template <class T> sint32 DAPriorityQueue<T>::Len()
{
    return m_queue.Num() - 1 ; 
} 

template <class T> void DAPriorityQueue<T>::LookForDups(const sint32 g)
{
    sint32 i, j, n; 

    n = m_queue.Num(); 
    for (i=1; i<(n-1); i++) { 
        for (j=i+1; j<n; j++) { 
            Assert(i == m_queue[i]->GetPriorityQueueIndex()); 
            Assert(m_queue[i] != m_queue[j]); 
        } 
    } 

    MapPoint pos; 
    for (i=1; i<n; i++) { 
        m_queue[i]->GetPos(pos); 
        Assert(g == sint32(g_theWorld->GetCell(pos)->m_search_count)); 
    } 
} 

template <class T> void DAPriorityQueue<T>::Clear()
{ 
    m_queue.Clear(); 
    m_queue.InsertFlat((T*) NULL); 
} 
#endif __DA_PRIORITY_QUEUE__
