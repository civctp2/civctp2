

#include "c3.h"
#include "c3errors.h"

#include "Token.h"
#include "ErrMsg.h"
#include "civarchive.h"
#include "StrDB.h"

#include "conceptdb.h"
#include "c3files.h"
#include "IconRecord.h"

extern sint32	g_abort_parse;
extern StringDB	*g_theStringDB;


ConceptDB::ConceptDB()
{
	Initialize();
}


ConceptDB::ConceptDB(CivArchive &archive)
{
	Initialize();
	Serialize(archive);
}


ConceptDB::~ConceptDB()
{
	if (m_conceptInfo)
		delete m_conceptInfo;
	if (m_indexToAlpha)
		delete m_indexToAlpha;
	if (m_alphaToIndex)
		delete m_alphaToIndex;
}


void ConceptDB::Initialize(void)
{
	m_nConcepts = 0;
	m_conceptInfo = NULL;
	m_indexToAlpha = NULL;
	m_alphaToIndex = NULL;
}


sint32 ConceptDB::Init(MBCHAR *filename)
{
	if (!ParseConceptDatabase( filename ))
		return FALSE;
	
	return TRUE;
}


sint32 ConceptDB::ParseConceptDatabase(MBCHAR *filename)
{
	sint32 n;
	Token *conceptToken = new Token(filename, C3DIR_GAMEDATA);

	Assert(conceptToken);

	if (conceptToken->GetType() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(conceptToken->ErrStr(), "Missing number of Concepts");
		g_abort_parse = TRUE;
		return FALSE;
	}
	else {
		conceptToken->GetNumber(n);
		conceptToken->Next();
		if (n < 0) {
			c3errors_ErrorDialog(conceptToken->ErrStr(), "Number of Concepts is negative");
			g_abort_parse = TRUE;
			return FALSE;
		}
		else if (n == 0) {
			c3errors_ErrorDialog(conceptToken->ErrStr(), "Number of Concepts is zero");
			g_abort_parse = TRUE;
			return TRUE;
		}

		m_nConcepts = n;
	}

	m_conceptInfo = new ConceptInfo[m_nConcepts];
    m_indexToAlpha = new sint32[m_nConcepts]; 
    m_alphaToIndex = new sint32[m_nConcepts]; 

	for (sint32 i = 0;i < m_nConcepts;i++) {
		if (!ParseAConcept(conceptToken, &m_conceptInfo[i]))
			return FALSE;

		
		m_alphaToIndex[i] = i;
		m_indexToAlpha[i] = i;

#if 0
		
		
		{
			const MBCHAR *str = g_theStringDB->GetNameStr(
				m_conceptInfo[i].m_name );
			for ( sint32 a = 0; a < i; ++a )
			{
				if ( _stricoll( str, g_theStringDB->GetNameStr(
					m_conceptInfo[ m_alphaToIndex[ a ] ].m_name ) ) < 0 )
				{
					
					memmove(
						m_alphaToIndex + a + 1,
						m_alphaToIndex + a,
						( i - a ) * sizeof(sint32) );

					
					for ( sint32 j = 0; j < i; ++j )
						if ( m_indexToAlpha[ j ] >= a )
							++m_indexToAlpha[ j ];

					break;
				}
			}

			
			m_alphaToIndex[ a ] = i;
			m_indexToAlpha[ i ] = a;
		}
#endif
	}

	if (g_abort_parse) return FALSE;

	delete conceptToken;

	return TRUE;
}


sint32 ConceptDB::ParseAConcept(Token *conceptToken, ConceptInfo *conceptInfo)
{
	char str[k_MAX_NAME_LEN];
	StringId str_id; 

	conceptInfo->m_noIndex = FALSE;
	
	if (conceptToken->GetType() == TOKEN_EOF) { 
		return FALSE; 
	} 
	
	if (conceptToken->GetType() != TOKEN_STRING) { 
		c3errors_ErrorDialog  (conceptToken->ErrStr(), "Concept id expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
		conceptToken->GetString(str); 
		if (!g_theStringDB->GetStringID(str, str_id)) { 
			c3errors_ErrorDialog (conceptToken->ErrStr(), "Could not find %s in string database", str); 
            g_abort_parse = TRUE;
			return FALSE; 
		}
	}

	conceptInfo->m_name = str_id;
	
	if (conceptToken->Next() != TOKEN_OPEN_BRACE) { 
		c3errors_ErrorDialog  (conceptToken->ErrStr(), "Missing open brace"); 
        g_abort_parse = TRUE;
		return FALSE;
	} 
	
	if(conceptToken->Next() != TOKEN_CONCEPT_DEFAULT_ICON) {
		c3errors_ErrorDialog(conceptToken->ErrStr(), "Field CONCEPT_DEFAULT_ICON expected");
		g_abort_parse = TRUE;
		return FALSE;
	}
	if(conceptToken->Next() != TOKEN_STRING) {
		c3errors_ErrorDialog(conceptToken->ErrStr(), "ID string for CONCEPT_DEFAULT_ICON expected");
		g_abort_parse = TRUE;
		return FALSE;
	}

	
	conceptToken->GetString(str);
	sint32 i = g_theIconDB->FindTypeIndex(str);
	if (i == -1) {
		c3errors_ErrorDialog(conceptToken->ErrStr(), "Couldn't find unit Concept filename string for '%s.'", str);
		g_abort_parse = TRUE;
		return FALSE;
	}

	conceptInfo->m_iconDBIndex = i;

	if (g_abort_parse) return FALSE; 

	conceptToken->Next();

	while ( ParseOptional(conceptToken, conceptInfo) );

	if (conceptToken->GetType() != TOKEN_CLOSE_BRACE) { 
		c3errors_ErrorDialog  (conceptToken->ErrStr(), "Missing close brace"); 
        g_abort_parse = TRUE;
		return FALSE; 
	} 

	conceptToken->Next();     
	return TRUE; 
}


BOOL ConceptDB::ParseOptional( Token *conceptToken, ConceptInfo *info )
{
	switch ( conceptToken->GetType() ) {
	case TOKEN_NO_INDEX:
		info->m_noIndex = TRUE;
		break;

	case TOKEN_CLOSE_BRACE:
		return FALSE;
		break;
	}
	conceptToken->Next();
	return TRUE;
}



const MBCHAR *ConceptDB::GetNameStr (sint32 at) const 

{ 
   Assert(0 <= at); 
   Assert(at < m_nConcepts);

   return g_theStringDB->GetNameStr(m_conceptInfo[at].m_name); 
}


void ConceptDB::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE;

	if (archive.IsStoring()) {
		archive<<m_nConcepts ;
    	archive.Store((uint8 *)m_conceptInfo, sizeof(ConceptInfo) * m_nConcepts) ;
    	archive.Store((uint8 *)m_indexToAlpha, sizeof(sint32) * m_nConcepts) ;
		
		
       
	} else {
		archive>>m_nConcepts ;
		if (m_conceptInfo)
			delete m_conceptInfo ;
		if (m_indexToAlpha)
			delete m_indexToAlpha ;
		if (m_alphaToIndex)
			delete m_alphaToIndex ;

		m_conceptInfo = new ConceptInfo[m_nConcepts];
		m_indexToAlpha = new sint32[m_nConcepts];
		m_alphaToIndex = new sint32[m_nConcepts];
        archive.Load((uint8 *)m_conceptInfo, sizeof(ConceptInfo) * m_nConcepts) ;
        archive.Load((uint8 *)m_indexToAlpha, sizeof(sint32) * m_nConcepts) ;
		for ( sint32 i = 0; i < m_nConcepts; ++i )
			m_alphaToIndex[ m_indexToAlpha[ i ] ] = i;
	}
}
