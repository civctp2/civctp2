#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef SIMPLEDYNARR_H__
#define SIMPLEDYNARR_H__

template <class T> class SimpleDynamicArray;

#include "civarchive.h"
#include "ctp2_inttypes.h"  // sint32

template <class T> class SimpleDynamicArray
{
public:
	SimpleDynamicArray();
	~SimpleDynamicArray();
	void Serialize(CivArchive &archive);

	void Insert(const T &val);
	T &Access(const sint32 i) {
		Assert((0 <= i) && (i < m_nElements));
		return m_array[i];
	}

	T & operator [] (const sint32 i)
    {
		Assert((0 <= i) && (i < m_nElements));
		return m_array[i];
	}

	T const & operator [] (const sint32 i) const
    {
		Assert((0 <= i) && (i < m_nElements));
		return m_array[i];
	}

	T GetLast() const {
		Assert(m_nElements > 0);
		return m_array[m_nElements - 1];
	}

	sint32 Num() const { return m_nElements; }
	void Clear() { m_nElements = 0; }

	void DelIndex(sint32 i);
	bool IsPresent(T const & chk) const;

#if 0
	const T * GetArray() const { return m_array; }
#endif

private:
    /// Stored data items
    T *     m_array;
    /// Number of stored data items
    sint32  m_nElements;
    /// Allocated size for storage.
    /// Always larger than 0 and at least \a m_nElements.
    sint32  m_arraySize;

    /// Increase allocated size for storage
    void Grow();
};

template <class T> SimpleDynamicArray<T>::SimpleDynamicArray()
:
    m_array        (new T[1]),
    m_nElements    (0),
    m_arraySize    (1)
{
}

template <class T> SimpleDynamicArray<T>::~SimpleDynamicArray()
{
    delete [] m_array;
}

template <class T> void SimpleDynamicArray<T>::Grow()
{
    T * oldarray = m_array;
    m_array = new T[m_arraySize * 2];
    memcpy(m_array, oldarray, m_arraySize * sizeof(T));
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
    Assert((0 <= i) && (i < m_nElements));
    memmove(&m_array[i], &m_array[i+1], m_nElements - i - 1);
    m_nElements--;
}

template <class T> void SimpleDynamicArray<T>::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive << m_nElements;
		archive << m_arraySize; // Not very useful to store this
        if (0 < m_nElements) {
    		archive.Store((uint8*)m_array, m_nElements * sizeof(T));
        }
	} else {
		archive >> m_nElements;
		archive >> m_arraySize;

		delete [] m_array;
		m_array = new T[m_arraySize]; /// @todo Try std::max(1, m_nElements)

        if (0 < m_nElements) {
    		archive.Load((uint8*)m_array, m_nElements * sizeof(T));
        }
	}
}

template <class T> bool SimpleDynamicArray<T>::IsPresent(T const & chk) const
{
	for (int i = 0; i < m_nElements; i++)
    {
		if (m_array[i] == chk)
			return true;
	}

    return false;
}
#endif
