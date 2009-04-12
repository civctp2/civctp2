//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Dynamic array (of armies)
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
// 
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Standardised min/max usage.
// - Microsoft extensions marked.
//
//----------------------------------------------------------------------------
//
// Probably to be replaced with a std::vector implementation later.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef DYNAMIC_ARRAY_H__
#define DYNAMIC_ARRAY_H__

#include <algorithm>

template <class T> class DynamicArray;
#define k_FUDGE_MAX_ARMY_SIZE 9

#include "AICause.h"
#include "ctp2_inttypes.h"
#include "civarchive.h"
#include "player.h"

template <class T> class DynamicArray 
{   
private:
  	sint32  m_maxElements; 

public: /// @todo Remove public access
    sint32  m_nElements; // 1 reference left in Path::FlattenAstarList
	T *     m_array;   
	
public: 
    DynamicArray(const sint32 size = 0); 
    DynamicArray (const DynamicArray<T> &copyme); 
    virtual ~DynamicArray(); 

    void ResizeCreate(const sint32 new_size,  T *&tmp);
    void ResizeCleanup(const sint32 new_size, T *tmp);
    void Resize(const sint32 new_size);
    void ResizeFlat(const sint32 new_size);

    virtual void DelPointers()
	{ 
		m_maxElements = 0; 
		m_nElements = 0; 
		delete [] m_array;
		m_array = NULL; 
	}

    
    T & operator [] (const sint32 i) { 
		Assert(0 <= i); 
		Assert(i < m_nElements);
		return m_array[i]; } ; 

    
    const T & operator [] (const sint32 i) const { 
		Assert(0 <= i); 
		Assert(i < m_nElements);
		return m_array[i]; } ; 

    DynamicArray<T> & operator = (const DynamicArray<T> &copyme);


    
	const T & Get(const sint32 i) const { 
		Assert(0 <= i); 
		Assert(i < m_nElements);
		return m_array[i]; } ; 

	T & Access(const sint32 i) const {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i]; }

	sint32 Find(const T & me) const;

	const sint32 Num() const  { return m_nElements; }

    void Concat(const DynamicArray<T> & addme);

    void ExtendByOne();
    void ExtendByOneFlat();
    void ExtendNFlat(const sint32 n); 

    void ShortenByOne(); 

    void Insert(const T &addme);
	void Insert(T *addme);
	void InsertBefore(const T &addme, sint32 index);
	void InsertBeforeFlat(const T &addme, sint32 index);
    void InsertFlat(const T &addme);

    bool IsPresent(const T &check) const;

    
    void Clear() { m_nElements = 0; }
    void DelUpToIndex(const sint32 m_index);
    bool DelIndex(const sint32 index);
    bool Del(const T &delme);
    bool DelIndexFlat(const sint32 index);
	bool DelIndexFlatNoShrink(const sint32 index);

	void Change(const T &old, const T &newobj);
    void KillList(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy);
    void KillList();
	void FastKillList();

    
    void Serialize(CivArchive &archive) ;

};









template <class T> DynamicArray<T>::DynamicArray(const sint32 size)
:
    m_maxElements   (std::max(1, size)),
    m_nElements     (0)
{
    m_array         = new T[m_maxElements]; 
}









template <class T> DynamicArray<T>::DynamicArray 
(
    const DynamicArray<T> &copyme 
)
:
    m_maxElements   (copyme.m_maxElements),
    m_nElements     (copyme.m_nElements)
{
    Assert(copyme.m_array);
    Assert(0 < m_maxElements);
    
    m_array = new T[m_maxElements]; 
    std::copy(copyme.m_array, copyme.m_array + m_nElements, m_array);
}









template <class T> DynamicArray<T>::~DynamicArray()
{
	delete [] m_array;
}









template <class T> DynamicArray<T> & DynamicArray<T>::operator = 
(
    const DynamicArray<T> &copyme
)
{
    if (this != &copyme)
    {
        Assert(copyme.m_array); 
        Assert(0<copyme.m_maxElements);
       
        m_maxElements   = copyme.m_maxElements; 
        m_nElements     = copyme.m_nElements; 
        
        delete [] m_array;
        m_array         = new T[m_maxElements]; 
        std::copy(copyme.m_array, copyme.m_array + m_nElements, m_array);
    }

    return *this; 
}










template <class T> void DynamicArray<T>::ResizeCreate(const sint32 new_size, 
    T *&tmp)
{
    Assert(m_array!= NULL); 
    Assert(0<m_maxElements);
    Assert(0<new_size);

    tmp = new T[new_size]; 

    sint32 const  n = std::min(new_size, m_nElements);
    std::copy(m_array, m_array + n, tmp);
}

template <class T> void DynamicArray<T>::ResizeCleanup(const sint32 new_size, T *tmp)
{ 
    delete [] m_array; 
    m_array         = tmp; 
    m_maxElements   = new_size;
}

template <class T> void DynamicArray<T>::Resize(const sint32 new_size)
{ 
    T * tmp = NULL; 
    ResizeCreate(new_size, tmp); 

    for (sint32 i = 0; i < m_maxElements; i++) 
    {
        m_array[i].DelPointers(); 
    }    

    ResizeCleanup(new_size, tmp); 
}

template <class T> void DynamicArray<T>::ResizeFlat(const sint32 new_size)
{ 
    T * tmp = NULL; 
    ResizeCreate(new_size, tmp); 
    ResizeCleanup(new_size, tmp); 
}


template <class T> void DynamicArray<T>::ExtendNFlat(const sint32 n)
{
    if (m_maxElements < n) { 
        ResizeFlat(n); 
    } 
    m_nElements = n;
}








template <class T> void DynamicArray<T>::Concat(const DynamicArray<T> & addme)
{ 
    Assert(m_array); 
    int sum = m_nElements + addme.m_nElements;

    
    if ((m_maxElements <= sum) && (sum < addme.m_maxElements)) { 
        Resize(addme.m_maxElements);  
    } else if ((m_maxElements <= sum) && (addme.m_maxElements <= sum)){ 
        Resize(std::max(m_maxElements, addme.m_maxElements) << 1); 
    }
    
    std::copy(addme.m_array, 
              addme.m_array + addme.m_nElements, 
              m_array + m_nElements
             );
    
    m_nElements = sum; 
}








template <class T> void DynamicArray<T>::ExtendByOne()
{
    Assert(m_array); 
    if (m_maxElements <= m_nElements) { 
        Resize(m_maxElements * 2);   
    } 
 
    m_nElements++; 
}

template <class T> void DynamicArray<T>::ExtendByOneFlat()
{
    Assert(m_array); 
    if (m_maxElements <= m_nElements) { 
        ResizeFlat(m_maxElements * 2);   
    } 
    m_nElements++; 
}


template <class T> void DynamicArray<T>::ShortenByOne()
{
    Assert(0 < m_nElements); 
    m_nElements--; 
}

template <class T> void DynamicArray<T>::Insert(const T &addme)
{
    ExtendByOne();
    m_array[m_nElements-1] = addme; 
}

template <class T> void DynamicArray<T>::Insert(T *addme)
{
	ExtendByOne();
	m_array[m_nElements-1] = *addme; 
    addme->DelPointers(); 

}

template <class T> void DynamicArray<T>::InsertBefore(const T &addme, sint32 index)
{
	ExtendByOne();

    for (sint32 i = m_nElements - 1; index < i; i--) 
    { 
        m_array[i] = m_array[i-1]; 
    } 

	m_array[index] = addme;
}

template <class T> void DynamicArray<T>::InsertBeforeFlat(const T &addme, sint32 index)
{
	ExtendByOneFlat();

    for (sint32 i= m_nElements - 1; index < i; i--) 
    { 
        m_array[i] = m_array[i-1]; 
    } 

	m_array[index] = addme;
}


template <class T> void DynamicArray<T>::InsertFlat(const T &addme)
{
    ExtendByOneFlat();
    m_array[m_nElements-1] = addme; 
}

template <class T> void DynamicArray<T>::DelUpToIndex(const sint32 index)
{
    Assert(0 <= index);
    Assert(index < m_nElements);

    sint32 new_size = m_nElements - index;

    T * tmp = new T[new_size]; 
    
    std::copy(m_array + index, (m_array + index) + new_size, tmp); 

    for (sint32 i = index; i<m_nElements; i++) 
    { 
        m_array[i].DelPointers(); 
    } 

    delete [] m_array; 
    m_array     = tmp; 
    m_nElements = m_maxElements = new_size; 
}









template <class T> bool DynamicArray<T>::DelIndex(const sint32 index)
{ 
    Assert (0 <= index);
    Assert(index < m_nElements); 
    Assert(m_array != NULL); 

    m_array[index].DelPointers(); 
    for (sint32 i = index; i < m_nElements - 1; i++) 
    { 
        m_array[i] = m_array[i+1]; 
    } 

    m_array[m_nElements-1].DelPointers(); 
   
    if (((4 * k_FUDGE_MAX_ARMY_SIZE) < m_maxElements) && (m_nElements < (m_maxElements >> 2))) { 
        Resize(m_maxElements>>1); 
    } 
    m_nElements--;

    return true; 
}

template <class T> bool DynamicArray<T>::DelIndexFlat(const sint32 index)
{ 
    Assert (0 <= index);
    Assert(index < m_nElements); 
    Assert(m_array != NULL); 

	const sint32 limit = m_nElements-1;
    for (sint32 i = index; i < limit; i++) 
    { 
        m_array[i] = m_array[i+1]; 
    } 
   
    if ((m_nElements < (m_maxElements >> 2)) && 
        ((4 * k_FUDGE_MAX_ARMY_SIZE) < m_maxElements)
       ) 
    { 
        ResizeFlat(m_maxElements>>1); 
    } 
    m_nElements--;
    return true; 
}


template <class T> bool DynamicArray<T>::DelIndexFlatNoShrink(const sint32 index)
{ 
    Assert (0 <= index);
    Assert(index < m_nElements); 
    Assert(m_array != NULL); 

	const sint32 limit = m_nElements-1;
    for (sint32 i = index; i < limit; i++) 
    { 
        m_array[i] = m_array[i+1]; 
    } 
    m_nElements--;
    return true; 
}














template <class T> bool DynamicArray<T>::Del(const T &del_me)
{
    Assert(m_array); 
    T * last    = m_array + m_nElements; 
    
    for (T * ptr = m_array; ptr != last; ++ptr)
    {
        if (*ptr == del_me)
        {
            return DelIndex(int(ptr - m_array));
        }
    }

    return false; 
}







template <class T> void DynamicArray<T>::Change(const T &old, const T &newobj)
{
    Assert(m_array);
    T * last    = m_array + m_nElements; 
    
    for (T * ptr = m_array; ptr != last; ++ptr)
    {
        if (*ptr == old)
        {
            *ptr = newobj;
            return;
        }
    }
}
	
template <class T> bool DynamicArray<T>::IsPresent(const T &check) const
{
	Assert(m_array != NULL);
    T * last    = m_array + m_nElements;
	for (T * ptr = m_array; ptr != last; ++ptr)
    {
        if (*ptr == check)
        {
            return true;
        }
    }

    return false;
}	









template <class T> sint32 DynamicArray<T>::Find(const T &me) const
{
    T * ptr = m_array;

    for (sint32 i = 0; i < m_nElements; ++i)
    {
        if (me == *ptr++)
        {
            return i;
        }
    }

	return -1;
}









template <class T> void DynamicArray<T>::KillList(const CAUSE_REMOVE_ARMY cause, PLAYER_INDEX killedBy)

{ 
    if (m_nElements <= 0)
        return; 

    T * killList = new T[m_nElements];
    std::copy(m_array, m_array + m_nElements, killList);

    int n = m_nElements;
    int i;
    for (i = 0; i < n; i++) 
    {
        killList[i].Kill(cause, killedBy); 
    } 

    for (i = 0; i < m_maxElements; i++) 
    { 
        killList[i].DelPointers(); 
    }

    delete [] killList;
    m_nElements = 0; 
}


template <class T> void DynamicArray<T>::KillList()
{ 
    if (m_nElements <= 0) 
        return; 

    T * killList = new T[m_nElements];
    std::copy(m_array, m_array + m_nElements, killList);

    int n = m_nElements;
    int i;
    for (i = 0; i < n; i++) 
    {
        killList[i].Kill(); 
    } 
    for (i=0; i<m_maxElements; i++) 
    { 
        killList[i].DelPointers(); 
    }
    delete [] killList;
    m_nElements = 0; 
}




template <class T> void DynamicArray<T>::FastKillList()
{ 
	int n = m_nElements;
    for (int i = 0; i < n; i++) 
    {
        m_array[i].FastKill();
    } 

    m_nElements = 0; 
}


template <class T> void DynamicArray<T>::Serialize(CivArchive &archive)
{
    if (archive.IsStoring()) 
    {
		archive << m_maxElements;
		archive << m_nElements;
    } 
    else 
    {
		archive >> m_maxElements;
		m_maxElements = std::max<sint32>(1, m_maxElements);
		archive >> m_nElements;
		delete [] m_array;    
		m_array = new T[m_maxElements];
    }

    for (int i = 0; i < m_nElements; ++i)
    {
		m_array[i].Serialize(archive);
    }
}

#endif
