










 


#pragma once
#ifndef __CITYSPRITEGROUP_H__
#define __CITYSPRITEGROUP_H__

#include "SpriteGroup.h"

class aui_Surface;
class aui_DirectSurface;

class Sprite;
class FacedSprite;

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

class CitySpriteGroup : public SpriteGroup {
public:
	CitySpriteGroup(GROUPTYPE type);
	virtual ~CitySpriteGroup();

	void			Load(char *filename);
	void			Import(uint16 id);

	void			Draw(sint32 drawX, sint32 drawY, sint32 facing, double scale, 
					  uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void			DrawText(sint32 x, sint32 y, char *s);

private:
 

	CITYSIZE		m_currentSize;
};


#endif