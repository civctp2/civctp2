#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SPRITE_STATE_DB_H__
#define __SPRITE_STATE_DB_H__ 1

class CivArchive ;
class Token;

struct SpriteNameNode {

	char m_name[_MAX_PATH];
	sint32 m_default_val;
};

class SpriteStateDB {


    sint32 m_size;







	SpriteNameNode *m_map;


public:

	SpriteStateDB ();
	SpriteStateDB(CivArchive &archive) ;
	~SpriteStateDB ();
	void SetSize(sint32 size);
	sint32 FindTypeIndex(char *str) const;
	sint32 GetDefaultVal(sint32 index) const;
    void SetName(sint32 count, char str[_MAX_PATH]);
    void SetVal(sint32 count, sint32 val);
	void Serialize(CivArchive &archive) ;

	sint32 ParseASpriteState (Token *spriteToken, sint32 count);
	sint32 Parse(char *filename);

};

#endif
