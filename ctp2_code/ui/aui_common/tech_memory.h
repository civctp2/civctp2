










#ifndef __TECH_MEMORY_H__
#define __TECH_MEMORY_H__





#define k_TECH_MEMORY_DEFAULT_BLOCKSIZE		20
#define k_TECH_MEMORY_BITSPERDWORD			(sizeof(unsigned)<<3)



template< class T >
class tech_Memory
{
public:
	
	tech_Memory( unsigned long blockSize = k_TECH_MEMORY_DEFAULT_BLOCKSIZE );
	virtual ~tech_Memory();

	T		*New( void );
	void	Delete( T *t );

protected:
	struct Block
	{
		Block( unsigned long blockSize )
			:
			pNext( 0 ),
			dataSize( blockSize )
		{
			
			usedSize = dataSize / k_TECH_MEMORY_BITSPERDWORD;
			unsigned long remainder = dataSize % k_TECH_MEMORY_BITSPERDWORD;
			if ( remainder ) usedSize++;

			if ( used = new unsigned[ usedSize ] )
			{
				
				memset( used, 0, usedSize * sizeof( unsigned ) );

				
				if ( remainder )
					used[ usedSize - 1 ] = ~( ( 1 << remainder ) - 1 );
			}
			
			data = new T[ dataSize ];
		}
		virtual ~Block()
		{
			if ( used )
			{
				delete[ usedSize ] used;
				used = 0;
			}

			if ( data )
			{
				delete[ dataSize ] data;
				data = 0;
			}
		}

		Block *pNext;			
		unsigned long usedSize; 
		unsigned *used;			
		unsigned long dataSize; 
		T *data;				
	};

	
	T *UseFreeElement( void );

	
	void UnuseElement( T *t );

	unsigned long m_blockSize;
	Block *m_pFirst;
	Block *m_pLast;
};





template< class T >
tech_Memory< T >::tech_Memory( unsigned long blockSize )
	:
	m_blockSize( blockSize ? blockSize : k_TECH_MEMORY_DEFAULT_BLOCKSIZE ),
	m_pFirst( 0 ),
	m_pLast( 0 )
{
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
}



template< class T >
T *tech_Memory< T >::New( void )
{
	T *t;

	if ( m_pLast )
	{
		if ( !(t = UseFreeElement()) )
		{
			if ( m_pLast->pNext = new Block( m_blockSize ) )
			{
				m_pLast = m_pLast->pNext;

				*(m_pLast->used) |= 1;
				t = m_pLast->data;
			}
		}
	}
	else
	{
		if ( m_pLast = m_pFirst = new Block( m_blockSize ) )
		{
			*(m_pLast->used) |= 1;
			t = m_pLast->data;
		}
	}

	return t;
}



template< class T >
void tech_Memory< T >::Delete( T *t )
{
	UnuseElement( t );
}



template< class T >
T *tech_Memory< T >::UseFreeElement( void )
{
	
	for ( Block *pBlock = m_pFirst; pBlock; pBlock = pBlock->pNext )
	{
		T *t = pBlock->data;
		T *stopT = t + m_blockSize;

		
		unsigned *pUsed = pBlock->used;
		unsigned *pStop = pUsed + pBlock->usedSize;
		for ( ; pUsed != pStop; pUsed++ )
		{
			
			unsigned freeSlots = ~(*pUsed);
			if ( freeSlots )
			{
				unsigned freeSlot = 1;
				while ( !(freeSlots & 1) )
				{
					if ( ++t == stopT ) return 0;
					freeSlot <<= 1;
					freeSlots >>= 1;
				}

				*pUsed |= freeSlot;
				return t;
			}

			t += k_TECH_MEMORY_BITSPERDWORD;
		}
	}

	return 0;
}



template< class T >
void tech_Memory< T >::UnuseElement( T *t )
{
	
	if ( !t ) return;

	
	unsigned long offset;
	for ( Block *pBlock = m_pFirst; pBlock; pBlock = pBlock->pNext )
	{
		offset = t - pBlock->data;
		if ( offset < m_blockSize )
			break;
	}

	
	if ( !pBlock ) return;

	
	pBlock->used[ offset / k_TECH_MEMORY_BITSPERDWORD ] &=
		~( 1 << ( offset % k_TECH_MEMORY_BITSPERDWORD ) );
}




#endif 
