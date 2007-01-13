#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SIMPLEDYNARR_H__
#define __SIMPLEDYNARR_H__

#include "civarchive.h"

template <class T> class SimpleDynamicArray {
private:
	T *m_array;
	sint32 m_nElements;
	sint32 m_arraySize;

	void Grow();
public:
	SimpleDynamicArray();
	~SimpleDynamicArray();
	void Serialize(CivArchive &archive);

	void Insert(const T &val);
	T &Access(const sint32 i) {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	T &operator [] (const sint32 i) {
		Assert(0 <= i);
		Assert(i < m_nElements);
		return m_array[i];
	}

	T GetLast() const {
		Assert(m_nElements > 0);
		return m_array[m_nElements - 1];
	}

	sint32 Num() const { return m_nElements; }
	void Clear() { m_nElements = 0; }

	void DelIndex(sint32 i);
	BOOL IsPresent(T chk);

	const T *GetArray() { return m_array; }
};

template <class T> SimpleDynamicArray<T>::SimpleDynamicArray()
{
	m_array = new T[1];
	m_nElements = 0;
	m_arraySize = 1;
}

template <class T> SimpleDynamicArray<T>::~SimpleDynamicArray()
{
	delete [] m_array;
}

template <class T> void SimpleDynamicArray<T>::Grow()
{
	T *oldarray = m_array;
	m_array = new T[m_arraySize * 2];
	memcpy(m_array, oldarray, m_arraySize*sizeof(T));
	m_arraySize *= 2;
	delete [] oldarray;
}

template <class T> void SimpleDynamicArray<T>::Insert(const T &val)
{
	if(m_nElements >= m_arraySize)
		Grow();

	m_array[m_nElements++] = val;
}

template <class T> void SimpleDynamicArray<T>::DelIndex(sint32 i)
{
	Assert(i >= 0);
	Assert(i < m_nElements);
	
	memmove(&m_array[i], &m_array[i+1], m_nElements - i - 1);
	m_nElements--;
}


template <class T> void SimpleDynamicArray<T>::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_nElements;
		archive << m_arraySize;
        if (0 < m_nElements) { 
    		archive.Store((uint8*)m_array, m_nElements * sizeof(T));
        }
	} else {
		archive >> m_nElements;
		archive >> m_arraySize;

		
		if (m_array)
			delete [] m_array;

		m_array = new T[m_arraySize];
        if (0 < m_nElements) { 
    		archive.Load((uint8*)m_array, m_nElements * sizeof(T));
        }
	}
}

template <class T> BOOL SimpleDynamicArray<T>::IsPresent(T chk)
{
	for(sint32 i = 0; i < m_nElements; i++) {
		if(m_array[i] == chk)
			return TRUE;
	}
	return FALSE;
}
#endif
