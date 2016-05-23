#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __PQUEUE_H__
#define __PQUEUE_H__

template <class T> class PQueue {
private:
	T* m_array;
	sint32 m_nElements;
	sint32 m_maxSize;

	void Grow() {
		T* oldArray = m_array;
		m_array = new T[m_maxSize * 2];
		memcpy(m_array, oldArray, sizeof(T) * m_maxSize);
		delete [] oldArray;
		m_maxSize *= 2;
	}

	void Shift(sint32 index) {
		if(m_nElements >= m_maxSize)
			Grow();

		if(m_nElements - index > 0) {
			memmove(&m_array[index + 1], &m_array[index],
					sizeof(T) * (m_nElements - index));
		}
		m_nElements++;
	}

public:
	PQueue(sint32 startSize) {
		m_maxSize = startSize;
		m_nElements = 0;
		m_array = new T[m_maxSize];
	}
	~PQueue() {
		if(m_array) {
			delete [] m_array;
			m_array = NULL;
		}
	}

	void Insert(const T &obj);
	bool RemoveTop(T &obj);
	sint32 Num() { return m_nElements; }
	void Clear() { m_nElements = 0; }
};

template <class T> void PQueue<T>::Insert(const T &obj)
{
	if(m_nElements >= m_maxSize) {
		Grow();
	}
	if(m_nElements == 0) {
		m_array[0] = obj;
		m_nElements++;
		return;
	}

	sint32 index = m_nElements >> 1;
	sint32 step = m_nElements >> 2;
	if(step < 1)
		step = 1;

	while(1) {
		if(obj.m_value > m_array[index].m_value) {
			if(index == 0 || obj.m_value <= m_array[index - 1].m_value) {
				Shift(index);
				m_array[index] = obj;
				return;
			}
			index -= step;
		} else if(obj.m_value < m_array[index].m_value) {
			if(index >= m_nElements - 1 ||
			   obj.m_value >= m_array[index + 1].m_value) {
				Shift(index + 1);
				m_array[index + 1] = obj;
				return;
			}
			index += step;
		} else {
			Shift(index);
			m_array[index] = obj;
			return;
		}
		if(step > 1)
			step >>= 1;
	}
}

template <class T> bool PQueue<T>::RemoveTop(T &obj)
{
	if(m_nElements <= 0)
		return FALSE;

	obj = m_array[m_nElements - 1];
	m_nElements--;
	return true;
}

#endif
