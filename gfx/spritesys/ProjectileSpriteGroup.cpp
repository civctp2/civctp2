










 


#include "c3.h"

#include "tiffutils.h"
#include "pixelutils.h"

#include "aui_directsurface.h"
#include "primitives.h"
#include "ScreenManager.h"

#include "ProjectileSpriteGroup.h"
#include "FacedSpriteWshadow.h"
#include "Sprite.h"

#include "CivPaths.h"
#include "c3files.h"

#include "SpriteFile.h"
#include "Anim.h"

#include "Token.h"

extern CivPaths *g_civPaths;
extern ScreenManager *g_screenManager;

ProjectileSpriteGroup::ProjectileSpriteGroup(GROUPTYPE type)
:SpriteGroup(type)
{
	POINT		thePoint = {24,24};
	POINT		emptyPoint = {0,0};
	sint32		i,j;

	for (i = PROJECTILEACTION_MOVE; i<PROJECTILEACTION_MAX; i++) 
	{
		m_sprites[i] = NULL;
		m_anims[i] = NULL;

	}

	for (i=0; i<k_NUM_FIREPOINTS; i++) 
	{
		for (j=0; j<k_NUM_FACINGS; j++) 
		{
			m_firePoints[i][j] = emptyPoint;
		}
	}
	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		m_moveOffsets[j] = emptyPoint;
	}

	m_width = 0;
	m_height = 0;

	m_hasDeath = FALSE;
	m_hasDirectional = FALSE;

	m_numFirePoints = 0;
}

ProjectileSpriteGroup::~ProjectileSpriteGroup()
{
	for (int i = PROJECTILEACTION_NONE+1; i<PROJECTILEACTION_MAX; i++) 
	{
		if (m_sprites[i]) 
		{
			delete m_sprites[i];
			m_sprites[i] = NULL;
		}
	}
}

void ProjectileSpriteGroup::Draw(PROJECTILEACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack)
{
	Assert(action > PROJECTILEACTION_NONE &&
			action < PROJECTILEACTION_MAX);


	if (m_sprites[action] == NULL) return;

	m_sprites[action]->SetCurrentFrame((uint16)frame);

	
	if(directionalAttack == FALSE)
	{
		if (m_sprites[action] != NULL)
		{
			m_sprites[action]->Draw(drawX, drawY, facing, scale, transparency, outlineColor, flags);
			m_sprites[action]->DrawShadow(SdrawX, SdrawY, facing, scale, transparency, outlineColor, flags);
		}
	}
	else
	{
		if (m_sprites[action] != NULL)
		{
			m_sprites[action]->DirectionalDraw(drawX, drawY, facing, scale, transparency, outlineColor, flags);
			m_sprites[action]->DirectionalDrawShadow(SdrawX, SdrawY, facing, scale, transparency, outlineColor, flags);
		}
	}
}

#define kBenchIterations		10000

void ProjectileSpriteGroup::RunBenchmark(aui_Surface *surf)
{








































































































	exit(0);
}


void ProjectileSpriteGroup::Load(MBCHAR *filename)
{
	SpriteFile		*file = new SpriteFile(filename);
	SPRITEFILETYPE	type;

	file->Open(&type);
	file->Read(this);
	file->CloseRead();

	m_loadType = LOADTYPE_FULL;
}

void ProjectileSpriteGroup::Save(MBCHAR *filename)
{
	SpriteFile *file = new SpriteFile(filename);

	file->Create(SPRITEFILETYPE_PROJECTILE);
	file->Write(this);
	file->CloseWrite();
}


sint32 ProjectileSpriteGroup::GetNumFrames(PROJECTILEACTION action)
{
	if (m_sprites[action] != NULL)
	{
		return m_sprites[action]->GetNumFrames();
	}
	else  
	{

		return 0;
	}
}

void ProjectileSpriteGroup::DrawText(sint32 x, sint32 y, char *s)
{
	primitives_DrawText((aui_DirectSurface *)g_screenManager->GetSurface(), x, y, (MBCHAR *)s, 0, 0);
}




sint32 ProjectileSpriteGroup::Parse(uint16 id)
{
	Token			*theToken=NULL; 
	MBCHAR			name[k_MAX_NAME_LENGTH];
	MBCHAR			scriptName[k_MAX_NAME_LENGTH];

	MBCHAR			*facedImageNames[k_NUM_FACINGS][k_MAX_NAMES];
	MBCHAR			*facedShadowNames[k_NUM_FACINGS][k_MAX_NAMES];

	MBCHAR			*imageNames[k_MAX_NAMES];
	MBCHAR			*shadowNames[k_MAX_NAMES];

	sint32			i,j;

	char			prefixStr[80];

	
	for (j=0; j<5; j++) 
	{
		for (i=0; i<k_MAX_NAMES; i++) 
		{
			facedImageNames[j][i] = (char *)malloc(k_MAX_NAME_LENGTH);
			facedShadowNames[j][i] = (char *)malloc(k_MAX_NAME_LENGTH);
		}
	}

	for (i=0; i<k_MAX_NAMES; i++) 
	{
		imageNames[i] = (char *)malloc(k_MAX_NAME_LENGTH);
		shadowNames[i] = (char *)malloc(k_MAX_NAME_LENGTH);
	}





	sprintf(prefixStr, ".\\%d\\", id);
	sprintf(scriptName, "GP%#.2d.txt", id);

printf("Processing '%s'\n", scriptName);

	theToken = new Token(scriptName, C3DIR_SPRITES); 
	Assert(theToken); 
	
	if (!theToken) return FALSE; 
	
	sint32 tmp; 

	if (!token_ParseKeywordNext(theToken, TOKEN_PROJECTILE_SPRITE)) return FALSE; 

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE; 

	if (!token_ParseValNext(theToken, TOKEN_PROJECTILE_SPRITE_MOVE, tmp)) return FALSE;    
	if (tmp) 
	{
		
		FacedSpriteWshadow *moveSprite = new FacedSpriteWshadow;
		
		
		moveSprite->ParseFromTokens(theToken);

		printf(" [Move");
		for (j=0; j<k_NUM_FACINGS; j++) 
		{
			for(i=0; i<moveSprite->GetNumFrames(); i++) 
			{

				sprintf(name, "%sGP%#.2dMS%d.%d.tif", prefixStr,  id, j+1, i+moveSprite->GetFirstFrame());
				strcpy(facedShadowNames[j][i], name);

				sprintf(name, "%sGP%#.2dMA%d.%d.tif", prefixStr, id,  j+1, i+moveSprite->GetFirstFrame());
				strcpy(facedImageNames[j][i], name);
			}
		}

		
		moveSprite->Import(moveSprite->GetNumFrames(), facedImageNames, facedShadowNames);

		
		m_sprites[PROJECTILEACTION_MOVE] = (FacedSpriteWshadow *)moveSprite;
		printf("]\n");

		Anim *moveAnim = new Anim;

		moveAnim->ParseFromTokens(theToken);
		m_anims[PROJECTILEACTION_MOVE] = moveAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_PROJECTILE_SPRITE_FIREPOINTS, tmp)) return FALSE;    
	if (tmp) 
	{
		
		m_numFirePoints = (uint16)tmp;

		for (sint32 j=0; j<tmp; j++) 
		{
			if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;
			for (i=0; i<k_NUM_FACINGS; i++) 
			{
				m_firePoints[j][i] = token_ParsePoint(theToken);
			}
			if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
		}
	}

	if (!token_ParseValNext(theToken, TOKEN_PROJECTILE_SPRITE_MOVEOFFSETS, tmp)) return FALSE;    
	if (tmp) 
	{
		if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;
		for (i=0; i<k_NUM_FACINGS; i++) 
		{
			m_moveOffsets[i] = token_ParsePoint(theToken);
		}
		if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
	}

	

	delete theToken;

	for (j=0; j<5; j++) 
	{
		for (i=0; i<k_MAX_NAMES; i++) 
		{
			free(facedImageNames[j][i]);
			free(facedShadowNames[j][i]);
		}
	}

	for (i=0; i<k_MAX_NAMES; i++) 
	{
		free(imageNames[i]);
		free(shadowNames[i]);
	}

	return TRUE;
}

POINT ProjectileSpriteGroup::GetHotPoint(PROJECTILEACTION action, sint32 facing)
{
	POINT nullPoint = {0,0};

	action = PROJECTILEACTION_MOVE;

	if (m_sprites[action] != NULL) 
	{
		if (m_sprites[action]->GetType() == SPRITETYPE_FACEDWSHADOW) 
		{
			if (facing >= k_NUM_FACINGS) facing = k_MAX_FACINGS - facing;
			return ((FacedSpriteWshadow *)m_sprites[action])->GetHotPoint((uint16)facing);
		} 
		else
		{
			return nullPoint;
		}
	}
	return nullPoint;
}
