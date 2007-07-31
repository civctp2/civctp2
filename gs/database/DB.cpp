











#include "c3.h"

#include "DB.h"
#include "CivArchive.h"


#include "strdb.h"
extern StringDB *g_theStringDB;

template <class T> Database<T>::Database()

{ 
    m_nRec = m_max_nRec = 0; 
    m_rec = NULL; 
	m_indexToAlpha = NULL;
	m_alphaToIndex = NULL;
}


template <class T> Database<T>::Database(CivArchive &archive)
	{
	m_nRec=m_max_nRec=0 ;
	m_rec=NULL ;
	m_indexToAlpha = NULL;
	m_alphaToIndex = NULL;
	Serialize(archive) ;
	}

template <class T> Database<T>::~Database()

{
    delete [] m_rec; 
    delete [] m_indexToAlpha; 
    delete [] m_alphaToIndex; 
}

template <class T> void  Database<T>::SetSize(const sint32 n)

{ 
    Assert (0 < n); 
	T*oldrec = NULL;
	sint32 *oldalpha = NULL;
	sint32 *oldindex = NULL;
	sint32 oldmax = m_max_nRec;
	if(m_rec) {
		oldrec = m_rec;

		Assert( m_indexToAlpha != NULL );
		oldalpha = m_indexToAlpha;
		Assert( m_alphaToIndex != NULL );
		oldindex = m_alphaToIndex;
	}
    m_max_nRec = n;
    m_rec = new T[n]; 
    m_indexToAlpha = new sint32[n]; 
    m_alphaToIndex = new sint32[n]; 
	memset(m_indexToAlpha, 0, sizeof(sint32) * n);
	memset(m_alphaToIndex, 0, sizeof(sint32) * n);
    Assert(m_rec); 
    Assert(m_indexToAlpha); 
    Assert(m_alphaToIndex); 
	if(oldrec) {
		if(oldmax < m_max_nRec)
		{
			memcpy(m_rec, oldrec, sizeof(T) * oldmax);
			memcpy(m_indexToAlpha, oldalpha, sizeof(sint32) * oldmax);
			memcpy(m_alphaToIndex, oldindex, sizeof(sint32) * oldmax);
		}
		else
		{
			memcpy(m_rec, oldrec, sizeof(T) * m_max_nRec);
			memcpy(m_indexToAlpha, oldalpha, sizeof(sint32) * m_max_nRec);
			memcpy(m_alphaToIndex, oldindex, sizeof(sint32) * m_max_nRec);
		}
	}

	
	if (oldrec)
		delete [] oldrec; 
	if (oldalpha)
		delete [] oldalpha; 
	if (oldindex)
		delete [] oldindex; 
} 

template <class T> void  Database<T>::SetSizeAll(const sint32 n)

{ 
    Assert (0 < n); 
    m_max_nRec = n;
    m_rec = new T[n]; 
    m_indexToAlpha = new sint32[n]; 
    m_alphaToIndex = new sint32[n]; 
	memset(m_indexToAlpha, 0, sizeof(sint32) * n);
	memset(m_alphaToIndex, 0, sizeof(sint32) * n);
    m_nRec = n; 
    Assert(m_rec); 
    Assert(m_indexToAlpha); 
    Assert(m_alphaToIndex); 
} 

template <class T> const T* Database<T>::Get(const sint32 i) const 

{ 
    Assert(0<=i); 
    Assert(i<m_nRec); 
	if(i < 0 || i >= m_nRec)
		return NULL;
    return &(m_rec[i]); 
}

template <class T> T* Database<T>::Access(const sint32 i) 

{ 
    Assert(0<=i); 
    Assert(i<m_nRec); 
    return &(m_rec[i]); 
}


template <class T> void Database<T>::AddRec(const StringId sid, sint32 &i) 

{ 
    Assert (m_nRec < m_max_nRec); 
    
    m_rec[m_nRec].SetName(sid); 

	
	
	{
		const MBCHAR *str = g_theStringDB->GetNameStr( sid );
		for ( sint32 a = 0; a < m_nRec; ++a )
		{
			if ( _stricoll( str, g_theStringDB->GetNameStr(
				m_rec[ m_alphaToIndex[ a ] ].GetName() ) ) < 0 )
			{
				
				memmove(
					m_alphaToIndex + a + 1,
					m_alphaToIndex + a,
					( m_nRec - a ) * sizeof(sint32) );

				
				for ( sint32 j = 0; j < m_nRec; ++j )
					if ( m_indexToAlpha[ j ] >= a )
						++m_indexToAlpha[ j ];

				break;
			}
		}

		
		m_alphaToIndex[ a ] = m_nRec;
		m_indexToAlpha[ m_nRec ] = a;
	}

    i = m_nRec; 
    m_nRec++; 
}


template <class T> void Database<T>::SetEnabling (const sint32 i, const sint32 e)

{
    Assert(0 <= i); 
    Assert(i < m_nRec); 
    
    m_rec[i].SetEnabling(e); 
}

template <class T> sint32 Database<T>::GetEnabling (const sint32 i) const
{
	Assert(0 <= i); 
    Assert(i < m_nRec); 
 
    return m_rec[i].GetEnabling(); 
}

template <class T> sint32 Database<T>::GetObsolete (const sint32 i, sint32 index) const
{
     Assert(0 <= i); 
     Assert(i < m_nRec); 
 
     return m_rec[i].GetObsolete(index); 
}

template <class T> void Database<T>::SetObsolete (const sint32 i, const sint32 o, sint32 index)

{
    Assert(0 <= i); 
    Assert(i < m_nRec); 
    
    m_rec[i].SetObsolete(o, index); 
}

template <class T> sint32 Database<T>::GetNamedItem (const StringId id, sint32 &index) const

{
    sint32 i; 
    
    for (i=0; i<m_nRec; i++) { 
        if (m_rec[i].m_name == id) { 
            index = i; 
            return TRUE; 
        } 
    } 
    return FALSE; 
}


template <class T> sint32 Database<T>::GetNamedItemID 
(
	sint32 index,
	StringId &id
) const

{
	if ((index < 0) || (index >= m_nRec))
		return FALSE;
    
    id = m_rec[index].m_name;

    return TRUE; 
}












template <class T> void Database<T>::Serialize(CivArchive &archive)
{
	int	i ;

	if (archive.IsStoring()) {
		archive<<m_nRec ;
		archive<<m_max_nRec ;
		for (i=0; i<m_nRec; i++)
		{
			m_rec[i].Serialize(archive) ;
			
			
			
			
			
			
		}

	} else {
		BOOL Database_Serialize_Doesnt_Work = FALSE;
		Assert(Database_Serialize_Doesnt_Work);
		return;
	}
}

#include "GWRecord.h"
#include "UVRecord.h"
#include "InstRec.h"
#include "CivilisationRec.h"
#include "RiskRecord.h"
#include "EndGameRecord.h"
#include "PollutionRecord.h"
#include "MapRecord.h"

template  class Database<GWRecord>;
template  class Database<PollutionRecord>;
template  class Database<CivilisationRecord>;
template  class Database<InstallationRecord>;
template  class Database<RiskRecord>;
template  class Database<EndGameRecord>;
template  class Database<MapRecord>;
