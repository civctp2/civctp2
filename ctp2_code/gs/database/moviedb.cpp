








#include "c3.h"

#include "MovieDB.h"
#include "CivArchive.h"

#include "Token.h"
#include "c3files.h"

extern sint32 g_abort_parse;

MovieDB::MovieDB ()

{
	m_map = 0;
}










MovieDB::MovieDB(CivArchive &archive)
	{
	m_map = NULL ;
	Serialize(archive) ;
	}


MovieDB::~MovieDB ()

{
	delete m_map;
}


void MovieDB::SetSize(sint32 s)

{
	m_size = s;
	m_map = new MovieNameNode[m_size];
}

sint32 MovieDB::FindTypeIndex(char *str) const

{
    sint32 i; 

    Assert(m_map); 

	for (i=0; i<m_size; i++) { 
        if (strcmp(m_map[i].m_name, str) == 0) {
			return i; 
		} 
	} 
	return -1; 
}

MBCHAR *MovieDB::GetMovieFilename(sint32 index) const
{

	Assert(0 <= index);
	Assert(index < m_size); 

	return m_map[index].m_movieFilename;
}

void MovieDB::SetName(sint32 index, char str[_MAX_PATH])

{
 	Assert(0 <= index);
	Assert(index < m_size); 
    Assert(m_map); 

    strcpy(m_map[index].m_name, str); 
}

void MovieDB::SetMovieFilename(sint32 index, MBCHAR *s)
{
 	Assert(0 <= index);
	Assert(index < m_size); 
    Assert(m_map); 

    strcpy(m_map[index].m_movieFilename, s); 
}

void MovieDB::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE;

	if (archive.IsStoring()) {
		archive<<m_size ;
    	archive.Store((uint8 *)m_map, sizeof(MovieNameNode) * m_size) ;
       
	} else {
		archive>>m_size ;
		if (m_map)
			delete m_map ;

		m_map = new MovieNameNode[m_size];
        archive.Load((uint8 *)m_map, sizeof(MovieNameNode) * m_size) ;
	}

}

sint32 MovieDB::ParseAMovie(Token *movieToken, sint32 count)

{
    char str[_MAX_PATH];

	if (movieToken->GetType() == TOKEN_EOF) { 
		return FALSE; 
	} 
	
	if (movieToken->GetType() != TOKEN_STRING) { 
		c3errors_ErrorDialog  (movieToken->ErrStr(), "Icon name expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
   		movieToken->GetString(str); 
        SetName(count, str); 
	}

    if (movieToken->Next() != TOKEN_QUOTED_STRING) { 
		c3errors_ErrorDialog  (movieToken->ErrStr(), "Movie filename expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
   		movieToken->GetString(str); 
        SetMovieFilename(count, str); 
	}

	movieToken->Next();

    return TRUE;
}

sint32 MovieDB::Parse(char *filename)

{

    Token *movieToken = new Token(filename, C3DIR_GAMEDATA); 
	Assert(movieToken); 
	
   	if (movieToken->GetType() != TOKEN_NUMBER) { 
		c3errors_ErrorDialog  (movieToken->ErrStr(), "Missing number of unit icons"); 
        g_abort_parse = TRUE;
		delete movieToken;
		return FALSE; 
	} else { 
        sint32 n;
		movieToken->GetNumber(n); 
		movieToken->Next(); 
		if (n <0) { 
			c3errors_ErrorDialog(movieToken->ErrStr(), "Number of unit icons is negative"); 
            g_abort_parse = TRUE;
			delete movieToken;
			return FALSE; 
		}
		SetSize(n); 
	}
	
    int count = 0;
    while (ParseAMovie(movieToken, count)) { 
        count++;
    }
    if (g_abort_parse) {
		delete movieToken;
		return FALSE; 
	}

	delete movieToken;

    return TRUE;
}
