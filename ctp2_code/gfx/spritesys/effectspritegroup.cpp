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
// __MAKESPR__
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
#include "EffectSpriteGroup.h"

#include "Actor.h"
#include "Anim.h"
#include <memory>	                // std::auto_ptr
#include "SpriteFile.h"
#include "Sprite.h"
#include "Token.h"


void EffectSpriteGroup::Draw(EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack)
{
	Assert(action > EFFECTACTION_NONE &&
			action < EFFECTACTION_MAX);


	if (m_sprites[action] == NULL) return;

	m_sprites[action]->SetCurrentFrame((uint16)frame);
	
#ifndef __MAKESPR__
	if(m_sprites[EFFECTACTION_FLASH] != NULL)
	{
		m_sprites[EFFECTACTION_FLASH]->SetCurrentFrame((uint16)frame);
		uint16 tempFlags = flags;
		tempFlags |= k_BIT_DRAWFLAGS_ADDITIVE;
		m_sprites[EFFECTACTION_FLASH]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, tempFlags);
	}
#endif

	
	if (action == EFFECTACTION_PLAY) 
    {
		m_sprites[action]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, flags);
	}
}

void EffectSpriteGroup::DrawDirect(aui_Surface *surf, EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack)
{
	Assert(action > EFFECTACTION_NONE &&
			action < EFFECTACTION_MAX);


	if (m_sprites[action] == NULL) return;

	m_sprites[action]->SetCurrentFrame((uint16)frame);
	
#ifndef __MAKESPR__
	if(m_sprites[EFFECTACTION_FLASH] != NULL)
	{
		m_sprites[EFFECTACTION_FLASH]->SetCurrentFrame((uint16)frame);
		uint16 tempFlags = flags;
		tempFlags |= k_BIT_DRAWFLAGS_ADDITIVE;
		m_sprites[EFFECTACTION_FLASH]->DrawDirect(surf, drawX, drawY, facing, scale, transparency, outlineColor, tempFlags);
	}
#endif

	if (action == EFFECTACTION_PLAY) 
    {
		m_sprites[action]->DrawDirect(surf, drawX, drawY, facing, scale, transparency, outlineColor, flags);
	}
}

void EffectSpriteGroup::Load(MBCHAR const * filename)
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

void EffectSpriteGroup::Save
(
    MBCHAR const *  filename,
    unsigned int    version_id,
    unsigned int    compression_mode
)
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
	MBCHAR			name[k_MAX_NAME_LENGTH];
	MBCHAR			scriptName[k_MAX_NAME_LENGTH];

	MBCHAR			*imageNames[k_MAX_NAMES];
	MBCHAR			*shadowNames[k_MAX_NAMES];

	size_t			i;

	char			prefixStr[80];

	for (i = 0; i < k_MAX_NAMES; i++) 
	{
		imageNames[i] =  new MBCHAR[k_MAX_NAME_LENGTH];
		shadowNames[i] = new MBCHAR[k_MAX_NAME_LENGTH];
	}

	sprintf(prefixStr, ".%s%d%s", FILE_SEP, id, FILE_SEP);
	sprintf(scriptName, "GX%.2d.txt", id);

    printf("Processing '%s'\n", scriptName);

	Token * theToken = new Token(scriptName, C3DIR_SPRITES); 
	Assert(theToken); 
	
	if (!theToken) return FALSE; 
	
	sint32  tmp;
    size_t  tmpNumFrames = 0; 

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

		delete m_sprites[EFFECTACTION_PLAY];
		m_sprites[EFFECTACTION_PLAY] = effectSprite;

		printf("]\n");

		Anim *effectAnim = new Anim;

		effectAnim->ParseFromTokens(theToken);
        delete m_anims[EFFECTACTION_PLAY];
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

		delete m_sprites[EFFECTACTION_FLASH];
		m_sprites[EFFECTACTION_FLASH] = flashSprite;
		printf("]\n");

		Anim *moveAnim = new Anim;

		moveAnim->ParseFromTokens(theToken);
        delete m_anims[EFFECTACTION_FLASH];
		m_anims[EFFECTACTION_FLASH] = moveAnim;

	}

	

	delete theToken;

	for (i = 0; i < k_MAX_NAMES; i++) 
	{
		delete[] imageNames[i];
		delete[] shadowNames[i];
	}

	return TRUE;
}

POINT EffectSpriteGroup::GetHotPoint(EFFECTACTION action, sint32 facing)
{
	POINT nullPoint = {0,0};

	Assert (m_sprites[action]);

	return m_sprites[action] ? m_sprites[action]->GetHotPoint() : nullPoint;
}

void EffectSpriteGroup::ExportScript(MBCHAR const * name)
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
	fprintf(file, "%s\n", g_allTokens[TOKEN_EFFECT_SPRITE].keyword);
	fprintf(file, "{\n");

	ExportSpriteGroup(file,(GAME_ACTION)EFFECTACTION_PLAY ,TOKEN_EFFECT_SPRITE_PLAY, TOKEN_MAX);
	ExportSpriteGroup(file,(GAME_ACTION)EFFECTACTION_FLASH,TOKEN_EFFECT_SPRITE_FLASH, TOKEN_MAX);

  



















	fprintf(file, "}\n");

	fclose(file);
}
