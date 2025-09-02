//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Good sprite handling
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
// - Prevent crashes on failed file operations.
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GoodSpriteGroup.h"

#include "c3errors.h"
#include <memory>         // std::unique_ptr
#include "tiffutils.h"
#include "pixelutils.h"
#include "primitives.h"
#include "FacedSprite.h"
#include "Sprite.h"
#include "screenmanager.h"
#include "CivPaths.h"
#include "c3files.h"
#include "SpriteFile.h"
#include "Anim.h"
#include "Token.h"

extern CivPaths *g_civPaths;
extern ScreenManager *g_screenManager;

void GoodSpriteGroup::Draw(GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY,
                           sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	Assert(action > GOODACTION_NONE &&
			action < GOODACTION_MAX);

	if (m_sprites[action] == NULL) return;

	if ((frame < 0) ||
	    (static_cast<size_t>(frame) >= m_sprites[action]->GetNumFrames())
	   )
	{
		frame = 0;
	}

	m_sprites[action]->SetCurrentFrame((uint16)frame);
	m_sprites[action]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, flags);
}

void GoodSpriteGroup::DrawDirect(aui_Surface *surf, GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags)
{
	Assert(action > GOODACTION_NONE &&
			action < GOODACTION_MAX);

	if (m_sprites[action] == NULL) return;

	m_sprites[action]->SetCurrentFrame((uint16)frame);
	m_sprites[action]->DrawDirect(surf, drawX, drawY, facing, scale, transparency, outlineColor, flags);
}

POINT GoodSpriteGroup::GetHotPoint(GOODACTION action)
{
	POINT nullPoint = {0,0};

	return m_sprites[action] ? m_sprites[action]->GetHotPoint() : nullPoint;
}

void GoodSpriteGroup::SetHotPoint(GOODACTION action, POINT pt)
{
	if (m_sprites[action] != NULL)
	{
		m_sprites[action]->SetHotPoint(pt.x,pt.y);
	}
}

void GoodSpriteGroup::LoadBasic(MBCHAR const * filename)
{
	std::unique_ptr<SpriteFile>	file(new SpriteFile(filename));

	SPRITEFILETYPE	type;
	if (SPRITEFILEERR_OK == file->Open(&type))
	{
		file->ReadBasic(this);
		file->CloseRead();
		m_loadType = LOADTYPE_BASIC;
	}
}

void GoodSpriteGroup::LoadFull(MBCHAR const * filename)
{
	std::unique_ptr<SpriteFile>	file(new SpriteFile(filename));

	SPRITEFILETYPE type;
	if (SPRITEFILEERR_OK == file->Open(&type))
	{
		file->ReadFull(this);
		file->CloseRead();
		m_loadType = LOADTYPE_FULL;
	}
}

void GoodSpriteGroup::Save(MBCHAR const * filename, unsigned int version_id, unsigned int compression_mode)
{
	std::unique_ptr<SpriteFile>	file(new SpriteFile(filename));

	if (SPRITEFILEERR_OK ==
	       file->Create(SPRITEFILETYPE_GOOD, version_id, compression_mode)
	   )
	{
		file->Write(this);
		file->CloseWrite();
	}
}

void GoodSpriteGroup::DeallocateStorage(void)
{
	for (int i = GOODACTION_IDLE; i < GOODACTION_MAX; i++)
	{
		delete m_sprites[i];
		m_sprites[i] = NULL;
	}
}

void GoodSpriteGroup::DeallocateFullLoadAnims(void)
{
	for (int i = GOODACTION_IDLE; i < GOODACTION_MAX; i++)
	{
		delete m_anims[i];
		m_anims[i] = NULL;
	}
}

void GoodSpriteGroup::DrawText(sint32 x, sint32 y, MBCHAR const * s)
{
	primitives_DrawText(g_screenManager->GetSurface(), x, y, s, 0, 0);
}

sint32 GoodSpriteGroup::Parse(uint16 id, GROUPTYPE group)
{
	MBCHAR			scriptName[k_MAX_NAME_LENGTH];
	char			prefixStr[80];

	sprintf(prefixStr, ".%s%03d%s", FILE_SEP, id, FILE_SEP);
	sprintf(scriptName, "GG%03d.txt", id);

	Token * theToken = new Token(scriptName, C3DIR_SPRITES);
	Assert(theToken);
	if (!theToken) return FALSE;

	if (!token_ParseKeywordNext(theToken, TOKEN_GOOD_SPRITE)) {
		delete theToken;
		return k_NOT_GOOD;
	}

	printf("Good Processing '%s'\n", scriptName);

	MBCHAR *    imageNames[k_MAX_NAMES];
	MBCHAR *    shadowNames[k_MAX_NAMES];
	size_t      i;

	for (i = 0; i < k_MAX_NAMES; i++)
	{
		imageNames[i] =  new MBCHAR[k_MAX_NAME_LENGTH];
		shadowNames[i] = new MBCHAR[k_MAX_NAME_LENGTH];
	}

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

	sint32 tmp;
	if (!token_ParseValNext(theToken, TOKEN_GOOD_SPRITE_IDLE, tmp)) return FALSE;
	if (tmp)
	{
		Sprite *idleSprite = new Sprite;
		Assert(idleSprite);
		if(!idleSprite) return FALSE;

		idleSprite->ParseFromTokens(theToken);

		printf(" [Idle");
		for(i=0; i<idleSprite->GetNumFrames(); i++)
		{
			MBCHAR			name[k_MAX_NAME_LENGTH];

			sprintf(name, "%sGG%03dS.%03zu.tif", prefixStr, id, i+idleSprite->GetFirstFrame());
			strcpy(shadowNames[i], name);

			sprintf(name, "%sGG%03dA.%03zu.tif", prefixStr, id, i+idleSprite->GetFirstFrame());
			strcpy(imageNames[i], name);
		}

		idleSprite->Import(idleSprite->GetNumFrames(), imageNames, shadowNames);
		delete m_sprites[GOODACTION_IDLE];
		m_sprites[GOODACTION_IDLE] = idleSprite;
		printf("]\n");

		Anim * idleAnim = Anim::CreateFromTokens(theToken);
		if (idleAnim) {
			delete m_anims[GOODACTION_IDLE];
			m_anims[GOODACTION_IDLE] = idleAnim;
		}
	}

	delete theToken;

	for (i = 0; i < k_MAX_NAMES; i++)
	{
		delete [] imageNames[i];
		delete [] shadowNames[i];
	}

	return TRUE;
}

void GoodSpriteGroup::ExportScript(MBCHAR const * name)
{
	extern TokenData	g_allTokens[];

	FILE * file = fopen(name, "w");
	if (!file) {
		c3errors_ErrorDialog("Sprite Export", "Could not open '%s' for writing.", name);
		return;
	}

	char timebuf[100];
	time_t ltime;
	time(&ltime);
	struct tm * now = localtime(&ltime);
	strftime(timebuf, 100, "%I:%M%p %m/%d/%Y", now);

	fprintf(file, "#\n");
	fprintf(file, "# This file was automatically generated by Sprite-Test\n");
	fprintf(file, "#\n");
	fprintf(file, "# %s\n", timebuf);
	fprintf(file, "#\n\n");

	fprintf(file, "%d # %s\n\n", 0, name);
	fprintf(file, "%s\n", g_allTokens[TOKEN_GOOD_SPRITE].keyword);
	fprintf(file, "{\n");

	ExportSpriteGroup(file,(GAME_ACTION)GOODACTION_IDLE,TOKEN_GOOD_SPRITE_IDLE,TOKEN_MAX);

	fprintf(file, "}\n");

	fclose(file);
}
