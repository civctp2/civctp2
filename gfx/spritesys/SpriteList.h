











#pragma once
#ifndef __SPRITELIST_H__
#define __SPRITELIST_H__

#define k_MAX_SPRITES		200

enum SPRITELISTERR {
	SPRITELISTERR_OK,

	SPRITELISTERR_NOTFOUND,
	SPRITELISTERR_BADTYPE,

	SPRITELISTERR_MAX
};

class Sprite;

class SpriteList {
public:
	SpriteList();
	virtual ~SpriteList();

	virtual SPRITELISTERR	LoadSprite(uint32 index);
	virtual SPRITELISTERR	PurgeSprite(uint32 index);

	Sprite					*GetSprite(uint32 index);

private:
	Sprite					*m_spriteList[k_MAX_SPRITES];
	sint32					m_listSize;
};

#endif
