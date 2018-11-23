//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Id           : $Id$
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Automatically reserved the extra item for the overflow detection.
//
//----------------------------------------------------------------------------
//
/// \file   Queue.h
/// \brief  Queue, implemented as circular buffer

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef QUEUE_H__
#define QUEUE_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

template <class T> class Queue;

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

// None

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

/// FIFO queue, implemented as circular buffer
template <class T> class Queue
{
public:
    Queue(size_t a_Size = 0);
    Queue(Queue<T> const & a_Original);
    virtual ~Queue();

    Queue<T> & operator = (Queue<T> const & a_Original);

    void Allocate(size_t a_Size);
    void Deallocate();

    bool Enqueue(T const & object);
    bool Dequeue(T & object);

/// Peek ahead to an object in the queue
/// \return     The object at the read index
/// \remarks    The returned object may be completely bogus
    T	 LookAtNextDeQueue(void)
    {
        return m_queue[m_queueTail];
    };

/// Peek back to an object in the queue
/// \return     The object before the write index
/// \remarks    The returned object is most likely completely bogus
    T	 LookAtLastDeQueue(void)
    {
        return m_queue[(m_queueHead > 0) ? m_queueHead - 1 : m_queueSize - 1];
    };

/// Get the number of items in the queue
/// \return     The number of stored items in the queue
    size_t GetNumItems(void) const { return m_numItems; };

    void CopyQueue(void);

    bool GetQueueItem(size_t index, T & item) const;

private:
    /// Allocated size of m_queue
    size_t      m_queueSize;
    /// Item queue
    T *         m_queue;
    /// Write index
    size_t      m_queueHead;
    /// Read index
    size_t      m_queueTail;
    /// Number of stored items in the queue
    size_t      m_numItems;
};

//----------------------------------------------------------------------------
// Template class definitions
//----------------------------------------------------------------------------

/// \param a_Size   Maximum number of items in the queue
template <class T> Queue<T>::Queue(size_t a_Size)
:
    m_queueSize   (a_Size + 1), // 1 extra for overflow detection
    m_queue       (NULL),
    m_queueHead   (0),
    m_queueTail   (0),
    m_numItems    (0)
{
    m_queue = new T[m_queueSize];
}

template <class T> Queue<T>::Queue(Queue<T> const & a_Original)
:
    m_queueSize   (a_Original.m_queueSize),
    m_queue       (NULL),
    m_queueHead   (a_Original.m_queueHead),
    m_queueTail   (a_Original.m_queueTail),
    m_numItems    (a_Original.m_numItems)
{
    m_queue = new T[m_queueSize];
    std::copy(a_Original.m_queue, a_Original.m_queue + m_queueSize, m_queue);
}

template <class T> Queue<T>::~Queue()
{
    delete [] m_queue;
}

template <class T> Queue<T> & Queue<T>::operator = (Queue<T> const & a_Original)
{
    if (this != &a_Original)
    {
        m_queueSize = a_Original.m_queueSize;
        delete [] m_queue;
        m_queue     = new T[m_queueSize];

        // Just copy everything, including bogus items
        std::copy(a_Original.m_queue, a_Original.m_queue + m_queueSize, m_queue);
        m_queueHead = a_Original.m_queueHead;
        m_queueTail = a_Original.m_queueTail;
        m_numItems  = a_Original.m_numItems;
    }

    return *this;
}

/// Resize the queue
/// \param a_Size   The new size
/// \remarks All currently stored items are lost
template <class T> void Queue<T>::Allocate(size_t a_Size)
{
    m_queueSize = a_Size + 1; // 1 extra for overflow detection
    delete [] m_queue;
    m_queue     = new T[m_queueSize];
    m_queueHead = 0;
    m_queueTail = 0;
    m_numItems  = 0;
}

/// @todo Remove when no longer referenced: use the destructor to deallocate
template <class T> void Queue<T>::Deallocate()
{
    Allocate(0);
}

/// Store an object in the queue
/// \param  object  The object to store
/// \return bool    The object has been stored - i.e. the queue was not full.
template <class T> bool Queue<T>::Enqueue(T const & object)
{
    size_t  nextPosition = (m_queueHead + 1) % m_queueSize;
    Assert(nextPosition != m_queueTail);
    if (nextPosition == m_queueTail)
    {
        return false;   // The queue is full
    }

    m_queue[m_queueHead]    = object;
    m_queueHead             = nextPosition;
    ++m_numItems;
    return true;
}

/// Retrieve a previously stored item from the queue
/// \param  object  Returns the retrieved object
/// \return bool    A valid object is returned - i.e. the queue is non-empty.
/// \remarks The retrieved object will be "removed" from the queue.
template <class T> bool Queue<T>::Dequeue(T & object)
{
    Assert(m_queueTail != m_queueHead);
    if (m_queueTail == m_queueHead)
    {
        return false; // The queue is empty
    }

    object      = m_queue[m_queueTail];
    m_queueTail = (m_queueTail + 1) % m_queueSize;
    --m_numItems;
    return true;
}

/// @todo Remove when no longer referenced: use the copy constructor
///       and the assignment operator.
template <class T> void Queue<T>::CopyQueue()
{
	T * oldQueue = m_queue;
	m_queue = new T[m_queueSize];
	memcpy(m_queue, oldQueue, m_queueSize * sizeof(T));
    // The old queue is not deleted: possible leak?
}

/// Peek ahead to a previously stored object in the queue
/// \param  index   How far to peek ahead
/// \param  item    Returns the object - when available
/// \return bool    The object is available, i.e. there are at least \a index
///                 objects in the queue.
/// \remarks The retrieved object will not be removed from the queue.
template <class T> bool Queue<T>::GetQueueItem(size_t index, T & item) const
{
	if (index >= m_numItems) return false;

	item = m_queue[(m_queueTail + index) % m_queueSize];
	return true;
}

#endif
