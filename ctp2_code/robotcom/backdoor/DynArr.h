#pragma once
#ifndef __DYNAMIC_ARRAY_H__
#define __DYNAMIC_ARRAY_H__ 1

class CivArchive;

#include "civarchive.h"

#define k_FUDGE_MAX_ARMY_SIZE 9

enum CAUSE_REMOVE_ARMY;

template <class T> class DynamicArray {

public:

  	sint32 m_maxElements;
    sint32 m_nElements;





	T * m_array;




public:

    void ResizeCreate(const sint32 new_size,  T *&tmp);
    void ResizeCleanup(const sint32 new_size, T *tmp);
    void Resize(const sint32 new_size);
    void ResizeFlat(const sint32 new_size);

    DynamicArray();
    DynamicArray(const sint32 size);
    DynamicArray (const DynamicArray<T> &copyme);
    virtual ~DynamicArray();

    virtual void DelPointers() {
        if (m_array) {
            delete [] m_array;
			m_array = NULL;
            Castrate();
        }
    }
	virtual void Castrate()
	{
		m_maxElements = 0;
		m_nElements = 0;

		if (m_array)
			delete m_array;

		m_array = NULL;
	}

    T & operator [] (const sint32 i) {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i]; } ;

   DynamicArray<T> & operator= (const DynamicArray<T> &copyme);


	const T & Get(const sint32 i) const {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i]; } ;

	T & Access(const sint32 i) const {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i]; }

	sint32 Find(const T &me) ;

	const sint32 Num() const  { if(!this) return 0; return m_nElements; }
    const sint32 ArraySize() const { return m_maxElements; }

    void Concat(const DynamicArray<T> & addme);

    void ExtendByOne();
    void ExtendByOneFlat();
    void ExtendNFlat(const sint32 n);

    void ShortenByOne();

    void Insert(const T &addme);
	void Insert(T *addme);
	void InsertBefore(const T &addme, sint32 index);
    void InsertFlat(const T &addme);

	BOOL IsPresent(const T &check);

    void Clear() { m_nElements = 0; }
    void DelUpToIndex(const m_index);
    sint32 DelIndex(const sint32 index);
    sint32 Del(const T &delme);
    sint32 DelIndexFlat(const sint32 index);

	void Change(const T &old, const T &newobj);
    void KillList(const CAUSE_REMOVE_ARMY cause, sint32 killedBy);
    void KillList();
	void FastKillList();

    void Serialize(CivArchive &archive) ;

};









template <class T> DynamicArray<T>::DynamicArray ()

{
    m_array = new T[1];
    Assert(m_array != NULL);
    m_nElements = 0;
    m_maxElements = 1;
}









template <class T> DynamicArray<T>::DynamicArray (
     const sint32 size
  )

{

    Assert(0 < size);

    m_array = new T[size];
    Assert(m_array != NULL);
    m_nElements = 0;
    m_maxElements = size;

}









template <class T> DynamicArray<T>::DynamicArray (
    const DynamicArray<T> &copyme
)

{
    m_maxElements = copyme.m_maxElements;
    m_nElements = copyme.m_nElements;

    Assert(0 < m_maxElements);

    m_array = new T[m_maxElements];
    Assert(m_array != NULL);

    Assert(copyme.m_array);

    sint32 i;
    for(i=0; i<m_nElements; i++) {
        m_array[i] = copyme.m_array[i];
    }
}









template <class T> DynamicArray<T>::~DynamicArray()

{
    if(m_array) {
       delete[] m_array;
       m_array = NULL;
    }

    m_array=NULL;
}









template <class T> DynamicArray<T> & DynamicArray<T>::operator= (
                                      const DynamicArray<T> &copyme)

{

    Assert(this != &copyme);

    Assert(copyme.m_array!= NULL);

    if (m_array)
       delete[] m_array;
    m_array = NULL;
    Assert(0<copyme.m_maxElements);

    m_maxElements = copyme.m_maxElements;
    m_nElements = copyme.m_nElements;

    m_array = new T[m_maxElements];
    Assert(m_array!= NULL);


    sint32 i;
    for(i=0; i<m_nElements; i++) {
        m_array[i] = copyme.m_array[i];
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
    Assert(tmp);

    sint32 i;
    sint32 n = min(new_size, m_nElements);
    for (i=0; i<n; i++) {
       tmp[i] = m_array[i];
    }
}

template <class T> void DynamicArray<T>::ResizeCleanup(const sint32 new_size, T *tmp)
{
    delete[] m_array;
    m_array = NULL;

    m_array = tmp;
    m_maxElements = new_size;
}

template <class T> void DynamicArray<T>::Resize(const sint32 new_size)
{
    T *tmp=NULL;

    ResizeCreate(new_size, tmp);

    int i;
    for (i=0; i<m_maxElements; i++) {
        m_array[i].DelPointers();

    }

    ResizeCleanup(new_size, tmp);
}

template <class T> void DynamicArray<T>::ResizeFlat(const sint32 new_size)
{
    T *tmp=NULL;

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
    Assert(m_array != NULL);
    int sum = m_nElements + addme.m_nElements;

    if ((m_maxElements <= sum) && (sum < addme.m_maxElements)) {
        Resize(addme.m_maxElements);
    } else if ((m_maxElements <= sum) && (addme.m_maxElements <= sum)){
        Resize(max(m_maxElements, addme.m_maxElements) << 1);
    }


    int i, j;
    for (i=m_nElements, j=0; j<addme.m_nElements; i++, j++) {
        m_array[i]  = addme.m_array[j];
    }

    m_nElements = sum;
}








template <class T> void DynamicArray<T>::ExtendByOne()

{
    Assert(m_array != NULL);
    if (m_maxElements <= m_nElements) {
        Resize(m_maxElements * 2);
    }

    m_nElements++;
}

template <class T> void DynamicArray<T>::ExtendByOneFlat()
{
    Assert(m_array != NULL);
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
    sint32 i;
    for (i=(m_nElements-1); index <i; i--) {
        m_array[i] = m_array[i-1];
    }

	m_array[index] = addme;
}

template <class T> void DynamicArray<T>::InsertFlat(const T &addme)

{
    ExtendByOneFlat();
    m_array[m_nElements-1] = addme;
}

template <class T> void DynamicArray<T>::DelUpToIndex(const index)

{
    Assert(0 <= index);
    Assert(index < m_nElements);


    sint32 new_size = m_nElements - index;

    T *tmp = new T[new_size];

    sint32 i;
    for (i=0; i<new_size; i++) {
        tmp[i] = m_array[index + i];
    }

    for (i = index; i<m_nElements; i++) {
        m_array[i].DelPointers();

    }

    delete[] m_array;

    m_array = tmp;
    m_nElements = m_maxElements = new_size;
}









template <class T> sint32 DynamicArray<T>::DelIndex(const sint32 index)
{
    Assert (0 <= index);
    Assert(index < m_nElements);
    Assert(m_array != NULL);




    m_array[index].DelPointers();
    sint32 i;
    for (i=index; i<(m_nElements-1); i++) {
        m_array[i] = m_array[i+1];
    }

    m_array[m_nElements-1].DelPointers();


    if (((4 * k_FUDGE_MAX_ARMY_SIZE) < m_maxElements) && (m_nElements < (m_maxElements >> 2))) {
        Resize(m_maxElements>>1);
    }
    m_nElements--;
    return TRUE;
}

template <class T> sint32 DynamicArray<T>::DelIndexFlat(const sint32 index)
{
    Assert (0 <= index);
    Assert(index < m_nElements);
    Assert(m_array != NULL);




    sint32 i;
    for (i=index; i<(m_nElements-1); i++) {
        m_array[i] = m_array[i+1];
    }




    if (((4 * k_FUDGE_MAX_ARMY_SIZE) < m_maxElements) && (m_nElements < (m_maxElements >> 2))) {
        ResizeFlat(m_maxElements>>1);
    }
    m_nElements--;
    return TRUE;
}










template <class T> sint32 DynamicArray<T>::Del(const T &del_me)

{
    Assert(m_array != NULL);
    T *ptr=NULL, *last=NULL;

    for (ptr=m_array, last=m_array+m_nElements; (ptr != last) && (*ptr!=del_me); ptr++);

    if (ptr == last) {
        return FALSE;
    } else {
        return DelIndex(int(ptr - m_array));
    }
}







template <class T> void DynamicArray<T>::Change(const T &old, const T &newobj)
{
	Assert(m_array != NULL);
    T *ptr=NULL, *last=NULL;

    for (ptr=m_array, last=m_array+m_nElements; (ptr != last) && (*ptr!=old); ptr++)
		;
	Assert(ptr != last);
	if(ptr == last)
		return;
	*ptr = newobj;
}

template <class T> BOOL DynamicArray<T>::IsPresent(const T &check)
{
	Assert(m_array != NULL);
	T *ptr, *last;
	for(ptr = m_array, last = m_array + m_nElements; (ptr != last) && (*ptr != check); ptr++)
		;
	return ptr != last;
}









template <class T> sint32 DynamicArray<T>::Find(const T &me)
	{
	sint32	i ;

	T	*ptr,
		*last ;

	Assert(m_array != NULL) ;

	for(i=0, ptr = m_array, last = m_array + m_nElements; (ptr != last) && (*ptr != me); ptr++, i++) ;

	if (ptr != last)
		return (i) ;

	return (-1) ;
	}









template <class T> void DynamicArray<T>::KillList(const CAUSE_REMOVE_ARMY cause, sint32 killedBy)

{
    Assert(m_array!= NULL);

    int i;
    int n;




    if (m_nElements <1)
        return;

    Assert(0<m_nElements);
    T* killList = new T[m_nElements];

    for (i=0; i<m_nElements; i++) {
        killList[i] = m_array[i];
    }

    n = m_nElements;

    for (i=0; i<n; i++) {
        killList[i].Kill(cause, killedBy);
    }

    for (i=0; i<m_maxElements; i++) {
        killList[i].DelPointers();

    }
    delete [] killList;
    m_nElements = 0;
}

template <class T> void DynamicArray<T>::KillList()

{
    Assert(m_array!= NULL);

    int i;
    int n;




    if (m_nElements <1)
        return;

    Assert(0<m_nElements);
    T* killList = new T[m_nElements];

    for (i=0; i<m_nElements; i++) {
       killList[i] = m_array[i];
    }

    n = m_nElements;

    for (i=0; i<n; i++) {
        killList[i].Kill();
    }
    for (i=0; i<m_maxElements; i++) {
        killList[i].DelPointers();

    }
    delete [] killList;
    m_nElements = 0;
}




template <class T> void DynamicArray<T>::FastKillList()

{
    Assert(m_array!= NULL);

    int i;
    int n;

	n = m_nElements;
    for (i=0; i<n; i++) {
        m_array[i].FastKill();
    }
    m_nElements = 0;
}

template <class T> void DynamicArray<T>::Serialize(CivArchive &archive)
{
    Assert(m_array != NULL);

    int	i ;

    if (archive.IsStoring()) {
		archive<<m_maxElements ;
		archive<<m_nElements ;

        for (i=0; i<m_nElements; i++)
			m_array[i].Serialize(archive) ;

    } else {
		archive>>m_maxElements ;
		archive>>m_nElements ;
		if (m_array != NULL)
			delete[] m_array;

		m_array = new T[m_maxElements] ;

        for (i=0; i<m_nElements; i++)
  			m_array[i].Serialize(archive) ;

    }

}
#endif
