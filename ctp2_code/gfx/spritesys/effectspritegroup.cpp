//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  :
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Moved common SpriteGroup member handling to SpriteGroup.
// - Prevent crashes on failed file operations.
// - Repaired memory leak.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "c3errors.h"

#include "tiffutils.h"
#include "pixelutils.h"

#include "aui_directsurface.h"
#include "primitives.h"

#include "EffectSpriteGroup.h"
#include "FacedSprite.h"
#include "FacedSpriteWshadow.h"

#include "Sprite.h"
#include "CivPaths.h"
#include "c3files.h"

#include "SpriteFile.h"
#include "Anim.h"
#include "Actor.h"

#include "Token.h"

#ifndef __SPRITETEST__
	#include "tiledmap.h"

	extern CivPaths *g_civPaths;

	#ifndef __MAKESPR__
		extern TiledMap *g_tiledMap;
	#endif
#endif

#include <memory>	// std::auto_ptr

EffectSpriteGroup::EffectSpriteGroup(GROUPTYPE type)
:
SpriteGroup(type)
{
}

EffectSpriteGroup::~EffectSpriteGroup()
{
}

void EffectSpriteGroup::Draw(EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack)
{
	Assert(action > EFFECTACTION_NONE &&
			action < EFFECTACTION_MAX);

	if (m_sprites[action] == NULL) return;

	m_sprites[action]->SetCurrentFrame((uint16)frame);

	if(m_sprites[EFFECTACTION_FLASH] != NULL)
	{
#ifndef __MAKESPR__
		m_sprites[EFFECTACTION_FLASH]->SetCurrentFrame((uint16)frame);
		uint16 tempFlags = flags;
		tempFlags |= k_BIT_DRAWFLAGS_ADDITIVE;
		m_sprites[EFFECTACTION_FLASH]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, tempFlags);
#endif
	}

	if (action == EFFECTACTION_PLAY) {
		if (m_sprites[action] != NULL)
		{
			m_sprites[action]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, flags);
		}
	}
}

void EffectSpriteGroup::DrawDirect(aui_Surface *surf, EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack)
{
	Assert(action > EFFECTACTION_NONE &&
			action < EFFECTACTION_MAX);

	if (m_sprites[action] == NULL) return;

	m_sprites[action]->SetCurrentFrame((uint16)frame);

	if(m_sprites[EFFECTACTION_FLASH] != NULL)
	{
#ifndef __MAKESPR__
		m_sprites[EFFECTACTION_FLASH]->SetCurrentFrame((uint16)frame);
		uint16 tempFlags = flags;
		tempFlags |= k_BIT_DRAWFLAGS_ADDITIVE;
		m_sprites[EFFECTACTION_FLASH]->DrawDirect(surf, drawX, drawY, facing, scale, transparency, outlineColor, tempFlags);
#endif
	}

	if (action == EFFECTACTION_PLAY) {
		if (m_sprites[action] != NULL)
		{
			m_sprites[action]->DrawDirect(surf, drawX, drawY, facing, scale, transparency, outlineColor, flags);
		}
	}
}

void EffectSpriteGroup::Load(MBCHAR *filename)
{
	std::auto_ptr<SpriteFile>	file(new SpriteFile(filename));
	SPRITEFILETYPE				type;

	if (SPRITEFILEERR_OK == file->Open(&type))
	{
		file->Read(this);
		file->CloseRead();
		m_loadType = LOADTYPE_FULL;
	}
}

void EffectSpriteGroup::Save(MBCHAR *filename,unsigned version_id,unsigned compression_mode)
{
	std::auto_ptr<SpriteFile>	file(new SpriteFile(filename));

	if (SPRITEFILEERR_OK ==
			file->Create(SPRITEFILETYPE_EFFECT, version_id, compression_mode)
	   )
	{
		file->Write(this);
		file->CloseWrite();
	}
}
















sint32 EffectSpriteGroup::Parse(uint16 id,GROUPTYPE group)
{
	Token			*theToken=NULL;
	MBCHAR			name[k_MAX_NAME_LENGTH];
	MBCHAR			scriptName[k_MAX_NAME_LENGTH];

	MBCHAR			*imageNames[k_MAX_NAMES];
	MBCHAR			*shadowNames[k_MAX_NAMES];

	sint32			i;

	char			prefixStr[80];

	for (i=0; i<k_MAX_NAMES; i++)
	{
		imageNames[i] = (char *)malloc(k_MAX_NAME_LENGTH);
		shadowNames[i] = (char *)malloc(k_MAX_NAME_LENGTH);
	}

	sprintf(prefixStr, ".%s%d%s", FILE_SEP, id, FILE_SEP);
	sprintf(scriptName, "GX%.2d.txt", id);

printf("Processing '%s'\n", scriptName);

	theToken = new Token(scriptName, C3DIR_SPRITES);
	Assert(theToken);

	if (!theToken) return FALSE;

	sint32 tmp, tmpNumFrames = 0;

	if (!token_ParseKeywordNext(theToken, TOKEN_EFFECT_SPRITE)) return FALSE;

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

	if (!token_ParseValNext(theToken, TOKEN_EFFECT_SPRITE_PLAY, tmp)) return FALSE;
	if (tmp)
	{

		Sprite *effectSprite = new Sprite;

		effectSprite->ParseFromTokens(theToken);

		printf(" [Effect");
		tmpNumFrames = effectSprite->GetNumFrames();
		for(i=0; i<effectSprite->GetNumFrames(); i++)
		{

			sprintf(name, "%sGX%.2dES.%d.tif", prefixStr,  id, i+effectSprite->GetFirstFrame());
			strcpy(shadowNames[i], name);

			sprintf(name, "%sGX%.2dEA.%d.tif", prefixStr, id, i+effectSprite->GetFirstFrame());
			strcpy(imageNames[i], name);
		}

		effectSprite->Import(effectSprite->GetNumFrames(), imageNames, shadowNames);

		m_sprites[EFFECTACTION_PLAY] = effectSprite;

		printf("]\n");

		Anim *effectAnim = new Anim;

		effectAnim->ParseFromTokens(theToken);
		m_anims[EFFECTACTION_PLAY] = effectAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_EFFECT_SPRITE_FLASH, tmp)) return FALSE;
	if (tmp)
	{

		Sprite *flashSprite = new Sprite;

		flashSprite->ParseFromTokens(theToken);

		printf(" [Flash");

		if(tmpNumFrames != 0 && tmpNumFrames != flashSprite->GetNumFrames() )
		{
			printf("\n Must have same number of special frames as effect frames ");
			return FALSE;
		}

		for(i=0; i<flashSprite->GetNumFrames(); i++)
		{
			sprintf(name, "%sGX%.2dFA.%d.tif", prefixStr, id, i+flashSprite->GetFirstFrame());
			strcpy(imageNames[i], name);

			strcpy(shadowNames[i], "");
		}

		flashSprite->Import(flashSprite->GetNumFrames(), imageNames, shadowNames);

		m_sprites[EFFECTACTION_FLASH] = flashSprite;
		printf("]\n");

		Anim *moveAnim = new Anim;

		moveAnim->ParseFromTokens(theToken);
		m_anims[EFFECTACTION_FLASH] = moveAnim;

	}


	delete theToken;

	for (i=0; i<k_MAX_NAMES; i++)
	{
		free(imageNames[i]);
		free(shadowNames[i]);
	}

	return TRUE;
}

POINT EffectSpriteGroup::GetHotPoint(EFFECTACTION action, sint32 facing)
{
	POINT nullPoint = {0,0};

	action = EFFECTACTION_PLAY;

	if (m_sprites[action] != NULL)
	{

			return m_sprites[action]->GetHotPoint();





	}
	return nullPoint;
}

void EffectSpriteGroup::ExportScript(MBCHAR *name)
{
	FILE				*file;
	extern TokenData	g_allTokens[];

	file = fopen(name, "w");
	if (!file) {
		c3errors_ErrorDialog("Sprite Export", "Could not open '%s' for writing.", name);
		return;
	}

	char timebuf[100];
	time_t ltime;
	struct tm *now;

	time(&ltime);
	now = localtime(&ltime);
	strftime(timebuf, 100, "%I:%M%p %m/%d/%Y", now);

	fprintf(file, "#\n");
	fprintf(file, "# This file was automatically generated by Sprite-Test\n");
	fprintf(file, "#\n");
	fprintf(file, "# %s\n", timebuf);
	fprintf(file, "#\n\n");

	fprintf(file, "%d # %s\n\n", 0, name);
	fprintf(file, "%s\n", g_allTokens[TOKEN_EFFECT_SPRITE].keyword);
	fprintf(file, "{\n");

	ExportSpriteGroup(file,(GAME_ACTION)EFFECTACTION_PLAY ,TOKEN_EFFECT_SPRITE_PLAY, TOKEN_MAX);
	ExportSpriteGroup(file,(GAME_ACTION)EFFECTACTION_FLASH,TOKEN_EFFECT_SPRITE_FLASH, TOKEN_MAX);





















	fprintf(file, "}\n");

	fclose(file);
}
