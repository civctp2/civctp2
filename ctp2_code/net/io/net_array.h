#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _NET_ARRAY_H_
#define _NET_ARRAY_H_

class NetArray
{
public:
	NetArray(sint32 initSize = 0) :
		m_array(NULL),
		m_size(initSize)
	{
		if(initSize != 0) {
			m_array = new void* [initSize];
		}
	};

	~NetArray()
	{
		if(m_array) {
			delete [] m_array;
			m_array = NULL;
		}
	}

	sint32 Add(void* ptr);
	void Set(void* ptr, sint32 idx);
	void* Get(sint32 idx);
	sint32 GetSize() { return m_size; };
private:
	void** m_array;
	sint32 m_size;
};

inline sint32
NetArray::Add(void* ptr)
{
	if(m_size > 0) {
		void** old_array = m_array;
		m_array = new void* [++m_size];
		memcpy(m_array, old_array, sizeof(void*) * (m_size - 1));
		delete [] old_array;
		m_array[m_size - 1] = ptr;
		return m_size - 1;
	} else {
		m_size = 1;
		m_array = new void*[m_size];
		m_array[m_size - 1] = ptr;
		return m_size;
	}
}

inline void
NetArray::Set(void* ptr, sint32 idx)
{
	m_array[idx] = ptr;
}

inline void*
NetArray::Get(sint32 idx)
{
	return m_array[idx];
}

#endif
