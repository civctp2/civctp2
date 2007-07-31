










 
#include "c3.h"

#include "pixelutils.h"
#include "Sprite.h"
#include "FacedSprite.h"
#include "SpriteList.h"

SpriteList::SpriteList()
{
}

SpriteList::~SpriteList()
{
}

SPRITELISTERR SpriteList::LoadSprite(uint32 index)
{
	return SPRITELISTERR_OK;
}

SPRITELISTERR SpriteList::PurgeSprite(uint32 index)
{
	return SPRITELISTERR_OK;
}

Sprite *SpriteList::GetSprite(uint32 index)
{
	return NULL;
}
