








#include "c3.h"

#include "filenamedb.h"
#include "civarchive.h"

#include "Token.h"
#include "c3files.h"

extern sint32 g_abort_parse;

FilenameDB::FilenameDB ()

{
	m_map = 0;
}










FilenameDB::FilenameDB(CivArchive &archive)
	{
	m_map = NULL ;
	Serialize(archive) ;
	}


FilenameDB::~FilenameDB ()

{
	delete m_map;
}


void FilenameDB::SetSize(sint32 s)

{
	m_size = s;
	m_map = new FilenameNode[m_size];
}

sint32 FilenameDB::FindTypeIndex(const char *str) const

{
    sint32 i; 

    Assert(m_map); 

	for (i=0; i<m_size; i++) { 
        if (stricmp(m_map[i].m_name, str) == 0) {
			return i; 
		} 
	} 
	return -1; 
}

MBCHAR *FilenameDB::GetFilename(sint32 index) const
{

	Assert(0 <= index);
	Assert(index < m_size); 

	if(index < 0 || index >= m_size)
		return NULL;
	return m_map[index].m_filename;
}

void FilenameDB::SetName(sint32 index, char str[_MAX_PATH])

{
 	Assert(0 <= index);
	Assert(index < m_size); 
    Assert(m_map); 

    strcpy(m_map[index].m_name, str); 
}

void FilenameDB::SetFilename(sint32 index, MBCHAR *s)
{
 	Assert(0 <= index);
	Assert(index < m_size); 
    Assert(m_map); 

    strcpy(m_map[index].m_filename, s); 
}

void FilenameDB::Serialize(CivArchive &archive)
{
	CHECKSERIALIZE;

	if (archive.IsStoring()) {
		archive<<m_size ;
    	archive.Store((uint8 *)m_map, sizeof(FilenameNode) * m_size) ;
       
	} else {
		archive>>m_size ;
		if (m_map)
			delete m_map ;

		m_map = new FilenameNode[m_size];
        archive.Load((uint8 *)m_map, sizeof(FilenameNode) * m_size) ;
	}

}

sint32 FilenameDB::ParseAFilename(Token *token, sint32 count)

{
    char str[_MAX_PATH];

	if (token->GetType() == TOKEN_EOF) { 
		return FALSE; 
	} 
	
	if (token->GetType() != TOKEN_STRING) { 
		c3errors_ErrorDialog  (token->ErrStr(), "Icon name expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
   		token->GetString(str); 
        SetName(count, str); 
	}

    if (token->Next() != TOKEN_QUOTED_STRING) { 
		c3errors_ErrorDialog  (token->ErrStr(), "Filename expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
   		token->GetString(str); 
        SetFilename(count, str); 
	}

	token->Next();

    return TRUE;
}

sint32 FilenameDB::Parse(char *filename)

{

    Token *token = new Token(filename, C3DIR_GAMEDATA); 
	Assert(token); 
	
   	if (token->GetType() != TOKEN_NUMBER) { 
		c3errors_ErrorDialog  (token->ErrStr(), "Missing number of filenames"); 
        g_abort_parse = TRUE;
		delete token;
		return FALSE; 
	} else { 
        sint32 n;
		token->GetNumber(n); 
		token->Next(); 
		if (n <0) { 
			c3errors_ErrorDialog(token->ErrStr(), "Number of filename is negative"); 
            g_abort_parse = TRUE;
			delete token;
			return FALSE; 
		}
		SetSize(n); 
	}
	
    int count = 0;
    while (ParseAFilename(token, count)) { 
        count++;
    }
    if (g_abort_parse) {
		delete token;
		return FALSE; 
	}

	delete token;

    return TRUE;
}
