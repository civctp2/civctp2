//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : ???
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
// NON_STANDART_C_PLUS_PLUS
// - When defined, generates the original Activision non standart 
//   compilant code.
// - When defined, generates modified standart compilant code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Add typename in front of tech_WLList< T >::Link *tech_WLList< T >::NewLink
//   to make the latest g++ and .NET compilers happy, by Martin Gühmann.
//
//----------------------------------------------------------------------------

#ifndef __TECH_WLLIST_H__
#define __TECH_WLLIST_H__


#include "tech_memory.h"




typedef void *ListPos;




#define k_TECH_WLLIST_DEFAULT_BLOCKSIZE		20





template< class T >
class tech_WLList
{
public:
	
	tech_WLList( unsigned long blockSize = k_TECH_WLLIST_DEFAULT_BLOCKSIZE );
	virtual ~tech_WLList();

	
	unsigned long L( void ) const { return m_length; }
	int IsEmpty( void ) const { return m_length == 0; }

	
	T &GetHead( void ) { return m_pHead->element; }
	T GetHead( void ) const { return m_pHead->element; }
	T &GetTail( void ) { return m_pTail->element; }
	T GetTail( void ) const { return m_pTail->element; }

	
	T RemoveHead( void );
	T RemoveTail( void );

	
	unsigned long RemoveDuplicates( void );

	
	ListPos AddHead( const T &newElement );
	ListPos AddTail( const T &newElement );

	
	void AddHead( const tech_WLList &NewList );
	void AddTail( const tech_WLList &NewList );

	
	ListPos GetHeadPosition( void ) const { return (ListPos)m_pHead; }
	ListPos GetTailPosition( void ) const { return (ListPos)m_pTail; }

	
	T &GetNext( ListPos &position );
	T GetNext( ListPos &position ) const;

	
	T &GetPrev( ListPos &position );
	T GetPrev( ListPos &position ) const;

	
	T &GetAt( ListPos position );
	T GetAt( ListPos position ) const;
	void SetAt( ListPos position, const T &newElement );
	void DeleteAt( ListPos position );
	void DeleteAll( void );

	
	ListPos InsertBefore( ListPos position, const T &newElement );
	ListPos InsertAfter( ListPos position, const T &newElement );

	
	ListPos Find( const T &searchElement, ListPos startAt = (ListPos)0 ) const;
		
	ListPos FindIndex( unsigned long index ) const;
		
	T GetAtIndex( unsigned long index ) const;
		

protected:
	struct Link
	{
		Link *pPrev;
		Link *pNext;
		T element;
	};

	
	Link *NewLink( Link *pPrevLink, Link *pNextLink );

	
	void DeleteLink( Link *&pLink );

	unsigned long m_length;		
	Link *m_pHead;				
	Link *m_pTail;				

	tech_Memory< Link > *m_memory; 
};








template< class T >
tech_WLList< T >::tech_WLList( unsigned long blockSize )
	:
	m_length( 0 ),
	m_pHead( 0 ),
	m_pTail( 0 )
{
	m_memory = new tech_Memory< Link >(
		blockSize ? blockSize : k_TECH_WLLIST_DEFAULT_BLOCKSIZE );
}


template< class T >
tech_WLList< T >::~tech_WLList()
{
	if ( m_memory )
	{
		delete m_memory;
		m_memory = 0;
	}

	m_length = 0;
	m_pHead = m_pTail = 0;
}








template< class T >
inline T &tech_WLList< T >::GetNext( ListPos &position )
{
	Link *pLink = (Link *)position;
	position = (ListPos)pLink->pNext;
	return pLink->element;
}


template< class T >
inline T tech_WLList< T >::GetNext( ListPos &position ) const
{
	Link *pLink = (Link *)position;
	position = (ListPos)pLink->pNext;
	return pLink->element;
}


template< class T >
inline T &tech_WLList< T >::GetPrev( ListPos &position )
{
	Link *pLink = (Link *)position;
	position = (ListPos)pLink->pPrev;
	return pLink->element;
}


template< class T >
inline T tech_WLList< T >::GetPrev( ListPos &position ) const
{
	Link *pLink = (Link *)position;
	position = (ListPos)pLink->pPrev;
	return pLink->element;
}


template< class T >
inline T &tech_WLList< T >::GetAt( ListPos position )
{
	Link *pLink = (Link *)position;
	return pLink->element;
}


template< class T >
inline T tech_WLList< T >::GetAt( ListPos position ) const
{
	Link *pLink = (Link *)position;
	return pLink->element;
}


template< class T >
inline void tech_WLList< T >::SetAt( ListPos position, const T &newElement )
{
	Link *pLink = (Link *)position;
	pLink->element = newElement;
}








template< class T >
#if defined(NON_STANDART_C_PLUS_PLUS)
//Removed by Martin Gühmann
tech_WLList< T >::Link *tech_WLList< T >::NewLink(
#else
//Added by Martin Gühmann to allow compiling on compilers
//that require standart C++ code.
typename tech_WLList< T >::Link *tech_WLList< T >::NewLink(
#endif
	Link *pPrevLink,
	Link *pNextLink )
{
	Link *pNewLink = m_memory->New();
	if ( pNewLink )
	{
		pNewLink->pPrev = pPrevLink;
		pNewLink->pNext = pNextLink;
		m_length++;
	}

	return pNewLink;
}


template< class T >
void tech_WLList< T >::DeleteLink( Link *&pLink )
{
	if ( pLink )
	{
		m_memory->Delete( pLink );
		pLink = 0;
		m_length--;
	}
}








template< class T >
ListPos tech_WLList< T >::AddHead( const T &newElement )
{
	Link *pNewLink = NewLink( 0, m_pHead );
	if ( pNewLink )
	{
		pNewLink->element = newElement;

		if ( m_pHead )
			m_pHead->pPrev = pNewLink;
		else
			m_pTail = pNewLink;

		m_pHead = pNewLink;
	}

	return (ListPos)pNewLink;
}


template< class T >
ListPos tech_WLList< T >::AddTail( const T &newElement )
{
	Link *pNewLink = NewLink( m_pTail, 0 );
	if ( pNewLink )
	{
		pNewLink->element = newElement;

		if ( m_pTail )
			m_pTail->pNext = pNewLink;
		else
			m_pHead = pNewLink;

		m_pTail = pNewLink;
	}

	return (ListPos)pNewLink;
}


template< class T >
void tech_WLList< T >::AddHead( const tech_WLList &NewList )
{
	ListPos position = NewList.GetTailPosition();
	while ( position )
		AddHead( NewList.GetPrev( position ) );
}


template< class T >
void tech_WLList< T >::AddTail( const tech_WLList &NewList )
{
	ListPos position = NewList.GetHeadPosition();
	while ( position )
		AddTail( NewList.GetNext( position ) );
}


template< class T >
T tech_WLList< T >::RemoveHead( void )
{
	Link *pOldLink = m_pHead;
	T theElement = pOldLink->element;

	if ( (m_pHead = pOldLink->pNext) )
		m_pHead->pPrev = 0;
	else
		m_pTail = 0;

	DeleteLink( pOldLink );

	return theElement;
}


template< class T >
T tech_WLList< T >::RemoveTail( void )
{
	Link *pOldLink = m_pTail;
	T theElement = pOldLink->element;

	if ( (m_pTail = pOldLink->pPrev) )
		m_pTail->pNext = 0;
	else
		m_pHead = 0;

	DeleteLink( pOldLink );

	return theElement;
}


template< class T >
ListPos tech_WLList< T >::InsertBefore( ListPos position, const T &newElement )
{
	if ( !position )
		return AddHead( newElement );

	Link *pOldLink = (Link *)position;
	Link *pNewLink = NewLink( pOldLink->pPrev, pOldLink );
	pNewLink->element = newElement;

	if ( pOldLink->pPrev )
		pOldLink->pPrev->pNext = pNewLink;
	else
		m_pHead = pNewLink;

	pOldLink->pPrev = pNewLink;

	return (ListPos)pNewLink;
}


template< class T >
ListPos tech_WLList< T >::InsertAfter( ListPos position, const T &newElement )
{
	if ( !position )
		return AddTail( newElement );

	Link *pOldLink = (Link *)position;
	Link *pNewLink = NewLink( pOldLink, pOldLink->pNext );
	pNewLink->element = newElement;

	if ( pOldLink->pNext )
		pOldLink->pNext->pPrev = pNewLink;
	else
		m_pTail = pNewLink;

	pOldLink->pNext = pNewLink;

	return (ListPos)pNewLink;
}


template< class T >
void tech_WLList< T >::DeleteAt( ListPos position )
{
	
	if (!position) return;

	Link *pOldLink = (Link *)position;

	if ( pOldLink == m_pHead )
	{
		if ( m_pHead = pOldLink->pNext )
			m_pHead->pPrev = NULL;
	}
	else
		pOldLink->pPrev->pNext = pOldLink->pNext;

	if ( pOldLink == m_pTail )
	{
		if ( m_pTail = pOldLink->pPrev )
			m_pTail->pNext = NULL;
	}
	else
		pOldLink->pNext->pPrev = pOldLink->pPrev;

	DeleteLink( pOldLink );
}


template< class T >
void tech_WLList< T >::DeleteAll( void )
{
	Link *pNextLink;
	for ( Link *pLink = m_pHead; pLink; pLink = pNextLink )
	{
		pNextLink = pLink->pNext;
		DeleteLink( pLink );
	}

	m_length = 0;
	m_pHead = m_pTail = 0;
}


template< class T >
ListPos tech_WLList< T >::FindIndex( unsigned long index ) const
{
	if ( index >= m_length )
		return (ListPos)0;

	Link *pLink = m_pHead;
	while ( index-- )
		pLink = pLink->pNext;

	return (ListPos)pLink;
}


template< class T >
T tech_WLList< T >::GetAtIndex( unsigned long index ) const
{
	



	Link *pLink = m_pHead;
	while ( index-- )
		pLink = pLink->pNext;

	return GetAt( pLink );
}


template< class T >
ListPos tech_WLList< T >::Find( const T &searchElement, ListPos startAt ) const
{
	Link *pLink = (Link *)startAt;
	if ( !pLink )
		pLink = m_pHead;

	for ( ; pLink; pLink = pLink->pNext )
		if ( pLink->element == searchElement )
			return (ListPos)pLink;

	return (ListPos)0;
}


template< class T >
unsigned long tech_WLList< T >::RemoveDuplicates( void )
{
	unsigned long numDupsFound = 0; 

	
	ListPos outerPosition = m_pHead;



	for ( sint32 i = m_length - 1; i >= 0; i-- )	
													
	{
		T element = GetNext( outerPosition );

		
		ListPos innerPosition = outerPosition;
		for ( sint32 j = i; j; j-- )
		{
			ListPos position = innerPosition;

			if ( GetNext( innerPosition ) == element )
			{
				DeleteAt( position );
				numDupsFound++;

				
				if ( outerPosition == position )
					outerPosition = innerPosition;
				i--;
			}
		}
	}

	return numDupsFound;
}




#endif 
