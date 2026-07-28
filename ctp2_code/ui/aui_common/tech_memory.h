//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  :
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
// _MSC_VER
// - Seems that this should be removed
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Variable scope corrected
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - New()/Delete() used to find a free slot by scanning every block's
//   used-bitmask from the start of the pool on every single allocation,
//   which is O(number of blocks the pool has ever grown to) per call and
//   never improves even once elements are freed, since blocks are never
//   released back. Replaced with an O(1) intrusive free list.
//
//----------------------------------------------------------------------------

#ifndef __TECH_MEMORY_H__
#define __TECH_MEMORY_H__

#define k_TECH_MEMORY_DEFAULT_BLOCKSIZE		20

template< class T >
class tech_Memory
{
public:
	tech_Memory(size_t blockSize = k_TECH_MEMORY_DEFAULT_BLOCKSIZE );
	virtual ~tech_Memory();

	T		*New( void );
	void	Delete( T *t );

protected:
	struct Block
	{
		Block(size_t blockSize)
		:
			pNext (0),
			data  (new T[blockSize])
		{};

		virtual ~Block()
		{
			delete[] data;
		};

		Block * pNext;
		T *     data;
	};

	// A free slot's own storage holds the pointer to the next free slot,
	// instead of a separate used/free bitmask. This is safe because a
	// slot only ever sits on the free list between calls to Delete() and
	// New(): New() always hands the slot back to a caller that fully
	// repopulates its fields before use, exactly as callers already had
	// to do when this pool tracked free slots via a bitmask instead.
	static T *&NextFree( T *slot ) { return *reinterpret_cast<T **>(slot); }

	size_t m_blockSize;
	Block * m_pFirst;
	Block * m_pLast;
	T *     m_pFreeList;
};




template< class T >
tech_Memory< T >::tech_Memory( size_t blockSize )
	:
	m_blockSize( blockSize ? blockSize : k_TECH_MEMORY_DEFAULT_BLOCKSIZE ),
	m_pFirst( 0 ),
	m_pLast( 0 ),
	m_pFreeList( 0 )
{
	// The free list needs room to store a T* inside a free T slot.
	static_assert(sizeof(T) >= sizeof(T *),
	              "tech_Memory<T> requires sizeof(T) >= sizeof(T*)");
}


template< class T >
tech_Memory< T >::~tech_Memory()
{
	Block *pNextBlock;
	for ( Block *pBlock = m_pFirst; pBlock; pBlock = pNextBlock )
	{
		pNextBlock = pBlock->pNext;
		delete pBlock;
	}

	m_pFirst = m_pLast = 0;
	m_pFreeList = 0;
}


template< class T >
T *tech_Memory< T >::New( void )
{
	if ( !m_pFreeList )
	{
		Block *newBlock = new Block(m_blockSize);

		if ( m_pLast )
		{
			m_pLast->pNext = newBlock;
		}
		else
		{
			m_pFirst = newBlock;
		}
		m_pLast = newBlock;

		// Chain every slot in the new block onto the free list.
		for ( size_t i = 0; i < m_blockSize; i++ )
		{
			T *slot = &newBlock->data[i];
			NextFree(slot) = m_pFreeList;
			m_pFreeList = slot;
		}
	}

	T *t = m_pFreeList;
	m_pFreeList = NextFree(t);
	return t;
}


template< class T >
void tech_Memory< T >::Delete( T *t )
{
	if ( !t ) return;

	NextFree(t) = m_pFreeList;
	m_pFreeList = t;
}


#endif
