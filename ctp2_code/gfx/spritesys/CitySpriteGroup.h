










 

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef CITYSPRITEGROUP_H__
#define CITYSPRITEGROUP_H__

class CitySpriteGroup;

enum CITYSIZE {
	CITYSIZE_NONE = -1,
	
	CITYSIZE_1,
	CITYSIZE_2,
	CITYSIZE_3,
	CITYSIZE_4,
	CITYSIZE_5,
	CITYSIZE_6,
	CITYSIZE_7,

	CITYSIZE_MAX
};

#include "ctp2_inttypes.h"
#include "pixeltypes.h"
#include "SpriteGroup.h"

class CitySpriteGroup : public SpriteGroup 
{
public:
	CitySpriteGroup(GROUPTYPE type);

	void			Load(MBCHAR const * filename);
	void			Import(uint16 id);

	void			Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, 
					  uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void			DrawText(sint32 x, sint32 y, MBCHAR const * s);

private:
	CITYSIZE		m_currentSize;
};


#endif
