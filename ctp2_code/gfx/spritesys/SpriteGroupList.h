











#pragma once
#ifndef __SPRITEGROUPLIST_H__
#define __SPRITEGROUPLIST_H__

#define k_MAX_SPRITES		200

#include "SpriteList.h"
#include "SpriteGroup.h"

class SpriteGroup;

class SpriteGroupList : public SpriteList {
public:
	SpriteGroupList();
	virtual ~SpriteGroupList();

	SPRITELISTERR			LoadSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action);
	SPRITELISTERR			PurgeSprite(uint32 index);

	SpriteGroup				*GetSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action);
	BOOL					ReleaseSprite(uint32 index, LOADTYPE loadType);

	void					RefreshBasicLoads(GROUPTYPE groupType);

private:
	SpriteGroup				*m_spriteList[k_MAX_SPRITES];
};

void spritegrouplist_Initialize(void);
void spritegrouplist_Cleanup(void);

#endif