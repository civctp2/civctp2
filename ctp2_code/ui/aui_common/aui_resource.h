










#ifndef __AUI_RESOURCE_H__
#define __AUI_RESOURCE_H__


#include "tech_wllist.h"


#include "c3files.h"
#include "civpaths.h"
extern CivPaths *g_civPaths;



template<class TT>
struct aui_ResourceElement
{
	aui_ResourceElement(
		MBCHAR *newName,
		MBCHAR *fullPath );
	virtual ~aui_ResourceElement();

	TT		*resource;		
	MBCHAR	*name;			
	uint32	hash;			
	uint32	pathhash;		
	sint32	refcount;		
};



template<class T>
class aui_Resource
{
public:
	
	aui_Resource();
	virtual ~aui_Resource();

	
	T			*Load( MBCHAR *name, C3DIR dir = C3DIR_DIRECT, uint32 size = 0);

	AUI_ERRCODE	Unload( T *resource );
	AUI_ERRCODE	Unload( MBCHAR *name );

	AUI_ERRCODE	AddSearchPath( MBCHAR *path );
	AUI_ERRCODE	RemoveSearchPath( MBCHAR *path );

	BOOL FindFile( MBCHAR *fullPath, MBCHAR *name );

protected:
	tech_WLList<aui_ResourceElement<T> *>	*m_resourceList;
		
		

	static tech_WLList<MBCHAR *>			*m_pathList;
		

	static sint32							m_resourceRefCount;
};








template<class TT>
aui_ResourceElement<TT>::aui_ResourceElement(
	MBCHAR *newName,
	MBCHAR *fullPath )
{
	
	Assert( newName != NULL && fullPath != NULL );
	if ( !newName || !fullPath ) return;

	
	AUI_ERRCODE errcode;
	resource = new TT( &errcode, fullPath );
	Assert( AUI_NEWOK(resource,errcode) );
	if ( !AUI_NEWOK(resource,errcode) ) return;

	errcode = resource->Load();
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) )
	{
		delete resource;
		return;
	}
	
	
	name = new MBCHAR[ strlen( newName ) + 1 ];
	Assert( name != NULL );
	if ( !name )
	{
		delete resource;
		return;
	}
	
	strcpy( name, newName );

	
	hash = aui_UI::CalculateHash( name );
	pathhash = aui_UI::CalculateHash( fullPath );

	
	refcount = 1;
}


template<class TT>
aui_ResourceElement<TT>::~aui_ResourceElement()
{
	if ( resource )
	{
		delete resource;
		resource = NULL;
	}

	if ( name )
	{
		delete[ strlen( name ) + 1 ] name;
		name = NULL;
	}
}









template<class T> tech_WLList<MBCHAR *> *aui_Resource<T>::m_pathList = NULL;
template<class T> sint32 aui_Resource<T>::m_resourceRefCount = 0;


template<class T>
aui_Resource<T>::aui_Resource()
{
	m_resourceList = new tech_WLList<aui_ResourceElement<T> *>;
	Assert( m_resourceList != NULL );

	if ( !m_resourceRefCount++ )
	{
		m_pathList = new tech_WLList<MBCHAR *>;
		Assert( m_pathList != NULL );
	}
}


template<class T>
aui_Resource<T>::~aui_Resource()
{
	if ( m_resourceList )
	{
		
		
		Assert( m_resourceList->L() == 0 );





		delete m_resourceList;
		m_resourceList = NULL;
	}

	if ( !--m_resourceRefCount )
	{
		ListPos position = m_pathList->GetHeadPosition();
		for ( sint32 i = m_pathList->L(); i; i-- )
		{
			MBCHAR *path = m_pathList->GetNext( position );
			delete[ strlen( path ) + 1 ] path;
		}
		delete m_pathList;
		m_pathList = NULL;
	}
}








template<class T>
AUI_ERRCODE aui_Resource<T>::AddSearchPath( MBCHAR *path )
{
	Assert( path != NULL );
	if ( !path ) return AUI_ERRCODE_INVALIDPARAM;

	uint32 len = strlen( path );

	Assert( len != 0 );
	if ( !len ) return AUI_ERRCODE_INVALIDPARAM;

	Assert( len <= MAX_PATH );
	if ( len > MAX_PATH ) return AUI_ERRCODE_INVALIDPARAM;

	
	
	MBCHAR *last = path + len - 1;
	Assert( *last != '\\' );
	if ( *last == '\\' ) *last = '\0';

	MBCHAR *newPath = new MBCHAR[ len + 1 ];
	Assert( newPath != NULL );
	if ( !newPath ) return AUI_ERRCODE_MEMALLOCFAILED;

	strcpy( newPath, path );

	m_pathList->AddTail( newPath );

	return AUI_ERRCODE_OK;
}


template<class T>
AUI_ERRCODE aui_Resource<T>::RemoveSearchPath( MBCHAR *path )
{
	

	ListPos position = m_pathList->GetHeadPosition();
	for ( sint32 i = m_pathList->L(); i; i-- )
	{
		ListPos prevPosition = position;

		MBCHAR *thisPath = m_pathList->GetNext( position );
		if ( strcmp( path, thisPath ) == 0 )
		{
			m_pathList->DeleteAt( prevPosition );
			delete[ strlen( thisPath ) + 1 ] thisPath;
			return AUI_ERRCODE_OK;
		}
	}

	return AUI_ERRCODE_OK;
}


template<class T>
T *aui_Resource<T>::Load( MBCHAR *resName, C3DIR dir, uint32 size)
{
	Assert( resName != NULL );
	if ( !resName ) return NULL;

	MBCHAR *name;
	MBCHAR tempName[MAX_PATH + 1];


	
	
	
	
	
	


	
	
	
	if (size)
	{
		sprintf(tempName, "%s%d", resName, size);
		name = tempName;
	}
	else
	{
		name = resName;
	}


	uint32 hash = aui_UI::CalculateHash( name );

	ListPos position = m_resourceList->GetHeadPosition();
	for ( sint32 i = m_resourceList->L(); i; i-- )
	{
		aui_ResourceElement<T> *re = m_resourceList->GetNext( position );
		if ( (hash == re->hash && strcmp( name, re->name ) == 0)
		||   (hash == re->pathhash && strcmp( name, re->resource->GetFilename() ) == 0) )
		{
			re->refcount++;
			return re->resource;
		}
	}


	

	if (size)
		name = resName; 

	static MBCHAR fullPath[ MAX_PATH + 1 ];
	strncpy( fullPath, name, MAX_PATH ); 

	
	
	
	
	if (dir != C3DIR_DIRECT) {
		
		
		MBCHAR path[_MAX_PATH];
		if (g_civPaths->FindFile(dir, name, path, TRUE)) {
			strcpy(fullPath, path);
		} else {
			if (dir == C3DIR_PICTURES) {
				
				if (g_civPaths->FindFile(C3DIR_PATTERNS, name, path, TRUE)) {
					strcpy(fullPath, path);
				} else {
					if (g_civPaths->FindFile(C3DIR_ICONS, name, path, TRUE)) {
						strcpy(fullPath, path);
					} else {
						FindFile( fullPath, name );
					}
				}
			} else {
				FindFile( fullPath, name );
			}
		}
	} else {
		FindFile( fullPath, name );
	}

	aui_ResourceElement<T> *re = new aui_ResourceElement<T>( name, fullPath );
	Assert( re != NULL );
	if ( re )
	{
		m_resourceList->AddTail( re );
		return re->resource;
	}

	return NULL;
}


template<class T>
BOOL aui_Resource<T>::FindFile( MBCHAR *fullPath, MBCHAR *name )
{
	BOOL found = FALSE;

	
	if ( !strchr( name, ':' ) && strncmp( name, "\\\\", 2 ) )
	{
		
		ListPos position = m_pathList->GetHeadPosition();
		if ( position )
		{
			for ( sint32 i = m_pathList->L(); i; i-- )
			{
				MBCHAR *path = m_pathList->GetNext( position );
				sprintf( fullPath, "%s\\%s", path, name );

				
				if ( GetFileAttributes( fullPath ) != 0xffffffff )
				{
					found = TRUE;
					break;
				}
			}

			
			if ( !i )
				strncpy( fullPath, name, MAX_PATH );
		}
	}

	return found;
}


template<class T>
AUI_ERRCODE aui_Resource<T>::Unload( T *resource )
{
	ListPos position = m_resourceList->GetHeadPosition();
	for ( sint32 i = m_resourceList->L(); i; i-- )
	{
		ListPos prevPosition = position;

		aui_ResourceElement<T> *re = m_resourceList->GetNext( position );
		if ( resource == re->resource && !--re->refcount )
		{
			m_resourceList->DeleteAt( prevPosition );
			delete re;
			return AUI_ERRCODE_OK;
		}
	}

	return AUI_ERRCODE_OK;
}


template<class T>
AUI_ERRCODE aui_Resource<T>::Unload( MBCHAR *name )
{
	uint32 hash = aui_UI::CalculateHash( name );

	ListPos position = m_resourceList->GetHeadPosition();
	for ( sint32 i = m_resourceList->L(); i; i-- )
	{
		ListPos prevPosition = position;

		aui_ResourceElement<T> *re = m_resourceList->GetNext( position );
		if ( (hash == re->hash && strcmp( name, re->name ) == 0)
		||   (hash == re->pathhash && strcmp( name, re->resource->GetFilename() ) == 0) )
		{
			if ( !--re->refcount )
			{
				m_resourceList->DeleteAt( prevPosition );
				delete re;
				return AUI_ERRCODE_OK;
			}
		}
	}

	return AUI_ERRCODE_OK;
}




#endif 
