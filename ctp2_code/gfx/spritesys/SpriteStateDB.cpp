












#include "c3.h"

#include "SpriteStateDB.h"
#include "civarchive.h"

#include "Token.h"

#include "c3files.h"

#ifdef __SPRITETEST__
	#define CHECKSERIALIZE			;
#endif

extern sint32 g_abort_parse;

SpriteStateDB::SpriteStateDB ()

{
	m_map = 0;
}











SpriteStateDB::SpriteStateDB(CivArchive &archive)
	{
	m_map = NULL ;
	Serialize(archive) ;
	}


SpriteStateDB::~SpriteStateDB ()

{
	delete m_map;
}


void SpriteStateDB::SetSize(sint32 s)

{
	m_size = s;
	m_map = new SpriteNameNode[m_size];
}

sint32 SpriteStateDB::FindTypeIndex(char *str) const

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

sint32 SpriteStateDB::GetDefaultVal(sint32 index) const

{

	Assert(0 <= index);
	Assert(index < m_size); 

	if (index < 0 || index >= m_size) return -1;

	return m_map[index].m_default_val;
}

void SpriteStateDB::SetName(sint32 index, char str[_MAX_PATH])

{
 	Assert(0 <= index);
	Assert(index < m_size); 
    Assert(m_map); 

    strcpy(m_map[index].m_name, str); 
}


void SpriteStateDB::SetVal(sint32 index, sint32 val)
{
 	Assert(0 <= index);
	Assert(index < m_size); 
    Assert(m_map); 

    m_map[index].m_default_val = val; 
}













void SpriteStateDB::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	if (archive.IsStoring()) {
		archive<<m_size ;
    	archive.Store((uint8 *)m_map, sizeof(SpriteNameNode) * m_size) ;
       
	} else {
		archive>>m_size ;
		if (m_map)
			delete m_map ;

		m_map = new SpriteNameNode[m_size];
        archive.Load((uint8 *)m_map, sizeof(SpriteNameNode) * m_size) ;
	}

}


sint32 SpriteStateDB::ParseASpriteState (Token *spriteToken, sint32 count)
{
    char str[_MAX_PATH];

	if (spriteToken->GetType() == TOKEN_EOF) { 
		return FALSE; 
	} 
	
	if (spriteToken->GetType() != TOKEN_STRING) { 
		c3errors_ErrorDialog  (spriteToken->ErrStr(), "Sprite name expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
   		spriteToken->GetString(str); 
        SetName(count, str); 
	}

    sint32 val;
    if (spriteToken->Next() != TOKEN_NUMBER) { 
		c3errors_ErrorDialog  (spriteToken->ErrStr(), "Sprite default number expected"); 
        g_abort_parse = TRUE;
		return FALSE;
	} else { 
   		spriteToken->GetNumber(val); 
        SetVal(count, val); 
	}

    spriteToken->Next();

    return TRUE;
}
	

sint32 SpriteStateDB::Parse(char *filename)

{
    sint32 n;

    Token *spriteToken = new Token(filename, C3DIR_GAMEDATA); 
	Assert(spriteToken); 
	
   	if (spriteToken->GetType() != TOKEN_NUMBER) { 
		c3errors_ErrorDialog  (spriteToken->ErrStr(), "Missing number of ability"); 
        g_abort_parse = TRUE;
		delete spriteToken;
		return FALSE; 
	} else { 
		spriteToken->GetNumber(n); 
		spriteToken->Next(); 
		if (n <0) { 
			c3errors_ErrorDialog  (spriteToken->ErrStr(), "Number of sprites is negative"); 
            g_abort_parse = TRUE;
			delete spriteToken;
			return FALSE; 
		}
		SetSize(n + 1); 
	}
	
    int count = 0;

    while (ParseASpriteState(spriteToken, count)) { 
        count++;
    }
    
	Assert(count == n);
	if(count == n) {
		SetName(count, "SPRITE_MYSTERY");
		SetVal(count, 9);
	}

	if (g_abort_parse) {
		delete spriteToken;
		return FALSE; 
	}

	delete spriteToken;

    return TRUE;
}
