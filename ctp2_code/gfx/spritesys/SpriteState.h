#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __SPRITE_STATE_H__
#define __SPRITE_STATE_H__

class CivArchive ;

class SpriteState {

protected:

	sint32 m_index;










public:

	SpriteState(sint32 index) { m_index = index; };
	SpriteState(CivArchive &archive) ;

	sint32		GetIndex(void) { return m_index; };
	void		SetIndex(sint32 index) { m_index = index; };

	void Serialize(CivArchive &archive) ;

};

#endif
