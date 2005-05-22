










#ifdef HAVE_PRAGMA_ONCE 
#pragma once
#endif
#ifndef __QUEUE_H__
#define __QUEUE_H__

template <class T> class Queue {

public:
  	sint32		m_queueSize;	
	T			*m_queue;		
    sint32		m_queueHead;	
	sint32		m_queueTail;	
	sint32		m_numItems;

public: 
	Queue();
	Queue(const sint32 size);
    virtual ~Queue(); 

	void Allocate(const sint32 size);
	void Deallocate();

	void Enqueue(T &object);
	void Dequeue(T &object);

	T	 LookAtNextDeQueue(void) { return m_queue[m_queueTail]; } 
	T	 LookAtLastDeQueue(void) { return m_queue[ (m_queueHead-1 < 0) ? m_queueSize-1 : m_queueHead-1]; }

	sint32 GetNumItems(void) { return m_numItems; }

	
	
	void CopyQueue(void);

	BOOL	GetQueueItem(sint32 index, T &item);
};

template <class T> Queue<T>::Queue()
{ 
	m_queueSize = 0;
	m_queueHead = 0;
	m_queueTail = 0;
	m_numItems = 0;
	m_queue = NULL;
} 

template <class T> Queue<T>::Queue(const sint32 size)
{
	Assert(size > 0);
	if (size == 0) return;

	m_queue = new T[size];
	Assert(m_queue != NULL);

    m_queueSize = size;

	m_queueHead = 0;
	m_queueTail = 0;
	m_numItems = 0;
}

template <class T> Queue<T>::~Queue()

{
    if(m_queue) { 
       delete[] m_queue;    
       m_queue = NULL;
    }

    m_queue=NULL;
	m_queueHead = 0;
	m_queueTail = 0;
	m_numItems = 0;
}

template <class T> void Queue<T>::Allocate(const sint32 size)
{
	if (m_queue != NULL) {
		delete[] m_queue;
		m_queueSize = 0;
	}

	m_queue = new T[size];
	Assert(m_queue != NULL);
	m_queueSize = size;
}

template <class T> void Queue<T>::Deallocate()
{
	if (m_queue != NULL) {
		delete[] m_queue;
		m_queue = NULL;
		m_queueSize = 0;
	}
}

template <class T> void Queue<T>::Enqueue(T &object)

{
	Assert(m_queue != NULL);
	if (m_queue == NULL) return;	

	Assert((m_queueHead + 1) % m_queueSize != m_queueTail);
	if ((m_queueHead + 1) % m_queueSize == m_queueTail) {
		
		return;
	}

    m_queue[m_queueHead] = object;
	sint32 newHead = (m_queueHead + 1) % m_queueSize;
	m_queueHead = newHead;

	m_numItems++;

}


template <class T> void Queue<T>::Dequeue(T &object)

{
	Assert(m_queue != NULL);
	if (m_queue == NULL) return;
	
	Assert(m_queueTail != m_queueHead);
	if (m_queueTail != m_queueHead) {
		object=m_queue[m_queueTail];
	} else {
		
		return;
	}
	
	m_queueTail = (m_queueTail + 1) % m_queueSize;

	m_numItems--;
}


template <class T> void Queue<T>::CopyQueue()
{
	T *oldQueue = m_queue;
	m_queue = new T[m_queueSize];
	memcpy(m_queue, oldQueue, m_queueSize * sizeof(T));
}

template <class T> BOOL Queue<T>::GetQueueItem(sint32 index, T &item) 
{
	if (index >= (sint32)m_numItems) return FALSE;
	
	sint32 i;

	i = (m_queueTail + index) % m_queueSize;

	item = m_queue[i];

	return TRUE;
}

#endif
