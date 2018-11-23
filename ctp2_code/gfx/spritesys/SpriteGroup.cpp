//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Common sprite handling
// Id           : $Id$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Moved common SpriteGroup member handling to SpriteGroup.
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "pixelutils.h"

#include "Sprite.h"
#include "SpriteGroup.h"
#include "Token.h"

SpriteGroup::SpriteGroup(GROUPTYPE type)
{
	m_type = type;

	m_usageRefCount = 0;
	m_fullLoadRefCount = 0;

	m_width = 0;
        m_height= 0;

	m_usageRefCount=0;
	m_fullLoadRefCount=0;

	m_loadType=LOADTYPE_NONE;

	for(int i=0;i<ACTION_MAX;i++)
	{
	  m_sprites[i]=NULL;
	  m_anims  [i]=NULL;
	}

	m_hasDeath=FALSE;
	m_hasDirectional=FALSE;


}

SpriteGroup::~SpriteGroup()
{
	for (int i = 0; i < ACTION_MAX; ++i)
	{
		delete m_anims[i];
		delete m_sprites[i];
	}
}


sint32 SpriteGroup::GetNumFrames(GAME_ACTION action)
{
	if((action<0)||(action>=ACTION_MAX))
	   return 0;

	if (m_sprites[action] != NULL)
		return m_sprites[action]->GetNumFrames();

	return 0;
}

void SpriteGroup::DeallocateStorage(void)
{
	for(sint32 i = ACTION_0; i < ACTION_MAX; i++){
		delete m_anims[i];
		m_anims[i] = NULL;
	}
}

void SpriteGroup::DeallocateFullLoadAnims(void)
{
	for(sint32 i = ACTION_0; i < ACTION_MAX; i++){
		delete m_anims[i];
		m_anims[i] = NULL;
	}
}

void SpriteGroup::Draw(	 sint32 drawX,
						 sint32 drawY,
						 sint32 facing,
						 double scale,
						 uint16 transparency,
						 Pixel16 outlineColor,
						 uint16 flags)
{

}

void SpriteGroup::DrawText(sint32 x,
						   sint32 y,
						   const char *s)
{
}

void SpriteGroup::AddRef(void)
{
	m_usageRefCount++;
}

void SpriteGroup::Release(void)
{
	m_usageRefCount--;
	Assert(m_usageRefCount >= 0);
}

void SpriteGroup::AddFullLoadRef(void)
{
	m_fullLoadRefCount++;
}

void SpriteGroup::ReleaseFullLoad(void)
{
	m_fullLoadRefCount--;
	Assert(m_fullLoadRefCount >= 0);
}

void SpriteGroup::ExportSpriteGroup(FILE *file,GAME_ACTION action,TOKEN_TYPES main_token,TOKEN_TYPES sub_token,BOOL sub_value)
{
	extern TokenData	g_allTokens[];
	Sprite             *sprite;
	Anim               *anim;

	fprintf(file,"\t%s", g_allTokens[main_token].keyword);

	sprite = GetGroupSprite(action);
	anim   = GetGroupAnim(action);

	if (sprite!=NULL)
	{
		fprintf(file, "\t1\n");

		if((sub_token>=0)&&(sub_token<TOKEN_MAX))
		   fprintf(file, "\t%s\t%d\n", g_allTokens[sub_token].keyword,sub_value);

		sprite->Export(file);

		if(anim!=NULL)
		   anim->Export(file);
	}
	else
		fprintf(file, "\t0\n\n");
}
