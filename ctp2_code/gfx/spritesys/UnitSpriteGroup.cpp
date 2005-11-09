//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit & city sprite handling
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
// __MAKESPR__
// - Probably supposed to generate the sprite make tool.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented crashes when accessing files that failed to open/create. 
// - Prevented crashes due to uninitialised members.
// - Moved common SpriteGroup member handling to SpriteGroup.
// - Removed Assert to make mod battles less tedious to debug.
// - Cleaned up some superfluous tests.
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#include "c3.h"                 // Pre-compiled header
#include "UnitSpriteGroup.h"    // Own declarations: consistency check

#include "tiffutils.h"
#include "pixelutils.h"

#include "c3errors.h"
#include "primitives.h"

#include "FacedSprite.h"
#include "Sprite.h"
#include "screenmanager.h"

#include "CivPaths.h"           // g_civPaths
#include "c3files.h"

#include "SpriteFile.h"
#include "Anim.h"

#include "colorset.h"           // g_colorSet

#include "Token.h"

extern ScreenManager	*g_screenManager;

UnitSpriteGroup::UnitSpriteGroup(GROUPTYPE type)
:	SpriteGroup(type),
	m_numFirePointsWork(0)
{
	POINT const	thePoint	= {24, 24};
	POINT const	emptyPoint	= {0, 0};

	for (sint32 j = 0; j < k_NUM_FACINGS; j++) 
	{
		m_moveOffsets[j]			= emptyPoint;

		for (sint32 i = 0; i < UNITACTION_MAX; i++) 
		{
			m_shieldPoints[i][j]	= thePoint;
		}

		for (sint32 k = 0; k < k_NUM_FIREPOINTS; k++)
		{
			m_firePointsWork[k][j]	= emptyPoint;
		}
	}
}

UnitSpriteGroup::~UnitSpriteGroup()
{
}

void UnitSpriteGroup::DeallocateStorage(void)
{
	for (sint32 i = UNITACTION_MOVE; i < UNITACTION_MAX; i++) 
	{
		delete m_sprites[i];
		m_sprites[i] = NULL;
	}
}








void UnitSpriteGroup::DeallocateFullLoadAnims(void)
{
	for (sint32 i = UNITACTION_MOVE; i < UNITACTION_MAX; i++) 
	{
		delete m_anims[i];
		m_anims[i] = NULL;
	}
}

void UnitSpriteGroup::Draw(UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack)
{
	
	
	if (action == UNITACTION_FAKE_DEATH) 
		action = UNITACTION_MOVE;

	Assert(action >= UNITACTION_MOVE &&
			action <= UNITACTION_WORK);

	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
	
	if (specialDelayProcess 
		|| (action == UNITACTION_IDLE && m_sprites[action] == NULL)
		|| (action == UNITACTION_ATTACK && m_sprites[action] == NULL)
		|| (action == UNITACTION_MOVE && m_sprites[UNITACTION_IDLE] == NULL)
       )
	{
		if (m_sprites[UNITACTION_MOVE])
		{
			action = UNITACTION_MOVE;
			frame = 0; 
		}
	}

	if (m_sprites[action])
    {
    	if (frame >= m_sprites[action]->GetNumFrames()) 
        {
		    frame = 0;
	    }
	
    	m_sprites[action]->SetCurrentFrame(static_cast<uint16>(frame));

	
	    if (directionalAttack)
	    {
		    m_sprites[action]->DirectionalDraw
                (drawX, drawY, facing, scale, transparency, outlineColor, flags);
	    }
	    else
	    {
	        m_sprites[action]->Draw
                (drawX, drawY, facing, scale, transparency, outlineColor, flags);
	    }
    }
}

BOOL UnitSpriteGroup::HitTest(POINT mousePt, UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 facing, 
							double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags,
							BOOL specialDelayProcess, BOOL directionalAttack)
{
	if (action == UNITACTION_FAKE_DEATH) 
		action = UNITACTION_MOVE;

	Assert(action >= UNITACTION_MOVE &&
			action <= UNITACTION_WORK);

	if (specialDelayProcess 
        || (action == UNITACTION_IDLE && m_sprites[action] == NULL) 
        || (action == UNITACTION_MOVE && m_sprites[UNITACTION_IDLE] == NULL)
       )
	{
		if (m_sprites[UNITACTION_MOVE])
		{
			action = UNITACTION_MOVE;
			frame = 0; 
		}
	}

	if (m_sprites[action]) 
    {
    	m_sprites[action]->SetCurrentFrame(static_cast<uint16>(frame));
	    return m_sprites[action]->HitTest
            (mousePt, drawX, drawY, facing, scale, transparency, outlineColor, flags);
    }
    return FALSE;
}

void UnitSpriteGroup::DrawDirect(aui_Surface *surf, UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, 
						   BOOL specialDelayProcess, BOOL directionalAttack)
{
	
	
	if (action == UNITACTION_FAKE_DEATH) 
		action = UNITACTION_MOVE;

	Assert(action >= UNITACTION_MOVE &&
			action <= UNITACTION_WORK);

	
	if (action < UNITACTION_MOVE || action > UNITACTION_WORK) 
		return;

	
	
	
	if (specialDelayProcess 
        || (action == UNITACTION_IDLE && m_sprites[action] == NULL)
        || (action == UNITACTION_MOVE && (m_sprites[UNITACTION_IDLE] == NULL))
       )
	{
		if (m_sprites[UNITACTION_MOVE])
		{
			action = UNITACTION_MOVE;
			frame = 0; 
		}
	}

    if (m_sprites[action] && (frame < m_sprites[action]->GetNumFrames()))
    {
    	m_sprites[action]->SetCurrentFrame(static_cast<uint16>(frame));
	
    	if (!directionalAttack)
	    {
			m_sprites[action]->DrawDirect(surf, drawX, drawY, facing, scale, transparency, outlineColor, flags);
	    }
	}
}

#define kBenchIterations		10000

void UnitSpriteGroup::RunBenchmark(aui_Surface *surf)
{
	exit(0);
}


void UnitSpriteGroup::LoadBasic(MBCHAR *filename)
{
	SpriteFile		*file = new SpriteFile(filename);
	SPRITEFILETYPE	type;
	if (SPRITEFILEERR_OK == file->Open(&type))
	{
		file->ReadBasic(this);
		file->CloseRead();
		m_loadType = LOADTYPE_BASIC;
	}

	delete file;
}




void UnitSpriteGroup::LoadIndexed(MBCHAR *filename,GAME_ACTION index)
{
	SpriteFile		*file = new SpriteFile(filename);
	SPRITEFILETYPE	type;
	if (SPRITEFILEERR_OK == file->Open(&type))
	{
		file->ReadIndexed(this, index);
		file->CloseRead();
		m_loadType = LOADTYPE_FULL;
	}

	delete file;
}



void UnitSpriteGroup::LoadFull(MBCHAR *filename)
{
	SpriteFile		*file = new SpriteFile(filename);
	SPRITEFILETYPE	type;
	if (SPRITEFILEERR_OK == file->Open(&type))
	{
		file->ReadFull(this);
		file->CloseRead();
		m_loadType = LOADTYPE_FULL;
	}

	delete file;
}

void UnitSpriteGroup::Save(MBCHAR *filename,unsigned version_id,unsigned compression_mode)
{
	SpriteFile *file = new SpriteFile(filename);
	if (SPRITEFILEERR_OK == 
			file->Create(SPRITEFILETYPE_UNIT, version_id, compression_mode)
	   )
	{
		file->Write(this);
		file->CloseWrite();
	}

	delete file;
}
 











void UnitSpriteGroup::DrawText(sint32 x, sint32 y, char *s)
{
#ifndef __MAKESPR__
	primitives_DrawText((aui_DirectSurface *)g_screenManager->GetSurface(), x+1, y+1, (MBCHAR *)s, g_colorSet->GetColorRef(COLOR_BLACK), 1);
	primitives_DrawText((aui_DirectSurface *)g_screenManager->GetSurface(), x, y, (MBCHAR *)s, g_colorSet->GetColorRef(COLOR_WHITE), 1);
#endif
}











bool			
UnitSpriteGroup::GetImageFileName(char *name,char *format,...)
{
   	va_list          v_args;
	char			 fname[512];
	
	
    va_start(v_args, format);    
    vsprintf(name,format,v_args);
    va_end( v_args );            

	
	sprintf(fname,"%s.%s",name,"TGA");

	if (c3files_PathIsValid(fname))
	{	
		strcpy(name,fname);
		return true;
	}

	
	sprintf(fname,"%s.%s",name,"TIF");

	if (c3files_PathIsValid(fname))
	{	
		strcpy(name,fname);
		return true;
	}

	return false;
}

sint32 UnitSpriteGroup::Parse(uint16 id, GROUPTYPE type)
{
	Token			*theToken=NULL; 
	MBCHAR			scriptName[k_MAX_NAME_LENGTH];

	MBCHAR			*facedImageNames[k_NUM_FACINGS][k_MAX_NAMES];
	MBCHAR			*facedShadowNames[k_NUM_FACINGS][k_MAX_NAMES];

	MBCHAR			*imageNames[k_MAX_NAMES];
	MBCHAR			*shadowNames[k_MAX_NAMES];

	sint32			i,j;

	char			prefixStr[80];

	
	for (j=0; j<k_NUM_FACINGS; j++) 
	{
		for (i=0; i<k_MAX_NAMES; i++) 
		{
			facedImageNames[j][i] = (char *)malloc(k_MAX_NAME_LENGTH<<1);
			facedShadowNames[j][i] = (char *)malloc(k_MAX_NAME_LENGTH<<1);
		}
	}

	for (i=0; i<k_MAX_NAMES; i++) 
	{
		imageNames[i] = (char *)malloc(k_MAX_NAME_LENGTH<<1);
		shadowNames[i] = (char *)malloc(k_MAX_NAME_LENGTH<<1);
	}

	sprintf(prefixStr, ".%s%d%s", FILE_SEP, id, FILE_SEP);

	if (type == GROUPTYPE_UNIT)
	{
		sprintf(scriptName, "GU%.3d.txt", id);

		
		if (!c3files_PathIsValid(scriptName))
			sprintf(scriptName, "GU%.2d.txt", id);
	}
	else 
		sprintf(scriptName, "GC%.3d.txt", id);

	printf("Processing '%s'\n", scriptName);

	theToken = new Token(scriptName, C3DIR_SPRITES); 
	Assert(theToken); 
	
	if (!theToken) return FALSE; 
	
	sint32 tmp; 

	if (!token_ParseKeywordNext(theToken, TOKEN_UNIT_SPRITE)) return FALSE; 

	if (!token_ParseAnOpenBraceNext(theToken)) return FALSE; 

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_MOVE, tmp)) return FALSE;    
	
	if (tmp) 
	{
		Assert(type == GROUPTYPE_UNIT);
		
		if (type != GROUPTYPE_UNIT) 
		{
			printf("\n Illegal unit action (Move) for a city sprite.\n");
			return FALSE;
		}

		
		FacedSprite *moveSprite = new FacedSprite;
		
		
		moveSprite->ParseFromTokens(theToken);

		printf(" [Move");
		for (j=0; j<k_NUM_FACINGS; j++) 
		{
			for(i=0; i<moveSprite->GetNumFrames(); i++) 
			{
				
				if (!GetImageFileName(facedShadowNames[j][i],"%sGU%#.3dMS%d.%d", prefixStr,  id, j+1, i+moveSprite->GetFirstFrame()))
					GetImageFileName(facedShadowNames[j][i] ,"%sGU%#.2dMS%d.%d", prefixStr,  id, j+1, i+moveSprite->GetFirstFrame());
				
				if (!GetImageFileName(facedImageNames[j][i], "%sGU%#.3dMA%d.%d", prefixStr, id,  j+1, i+moveSprite->GetFirstFrame()))
					GetImageFileName(facedImageNames[j][i] , "%sGU%#.2dMA%d.%d", prefixStr, id,  j+1, i+moveSprite->GetFirstFrame());
			}
		}

		
		moveSprite->Import(moveSprite->GetNumFrames(), facedImageNames, facedShadowNames);

		
		m_sprites[UNITACTION_MOVE] = (Sprite *)moveSprite;
		printf("]\n");

		Anim *moveAnim = new Anim;

		moveAnim->ParseFromTokens(theToken);
		m_anims[UNITACTION_MOVE] = moveAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_ATTACK, tmp)) return FALSE;    
	if (tmp) 
	{
		Assert(type == GROUPTYPE_UNIT);
		
		if (type != GROUPTYPE_UNIT) 
		{
			printf("\n Illegal unit action (Attack) for a city sprite.\n");
			return FALSE;
		}
		
		if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_ATTACK_IS_DIRECTIONAL, tmp)) return FALSE;
		if (tmp) 
			m_hasDirectional = TRUE;
		else 
			m_hasDirectional = FALSE;

		
		FacedSprite *attackSprite = new FacedSprite;
		
		
		attackSprite->ParseFromTokens(theToken);

		printf(" [Attack");
		for (j=0; j<k_NUM_FACINGS; j++) 
		{
			sint32 camera = j + 1;
			for(i=0; i<attackSprite->GetNumFrames(); i++) 
			{
				if (!GetImageFileName(facedShadowNames[j][i],"%sGU%#.3dAS%d.%d", prefixStr, id, j+1, i+attackSprite->GetFirstFrame()))
					GetImageFileName (facedShadowNames[j][i],"%sGU%#.2dAS%d.%d", prefixStr, id, j+1, i+attackSprite->GetFirstFrame());
				
				if (!GetImageFileName(facedImageNames [j][i],"%sGU%#.3dAA%d.%d", prefixStr, id, j+1, i+attackSprite->GetFirstFrame()))
					GetImageFileName (facedImageNames [j][i],"%sGU%#.2dAA%d.%d", prefixStr, id, j+1, i+attackSprite->GetFirstFrame());
			}
		}

		
		attackSprite->Import(attackSprite->GetNumFrames(), facedImageNames, facedShadowNames);

		
		m_sprites[UNITACTION_ATTACK] = (Sprite *)attackSprite;
		printf("]\n");

		Anim *attackAnim = new Anim;

		attackAnim->ParseFromTokens(theToken);
		m_anims[UNITACTION_ATTACK] = attackAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_IDLE, tmp)) return FALSE;    
	if (tmp) 
	{
		Sprite *idleSprite = new Sprite;
		idleSprite->ParseFromTokens(theToken);

		if (type == GROUPTYPE_UNIT) 
		{
			printf(" [Idle");
		  
			for(i=0; i<idleSprite->GetNumFrames(); i++) 
			{
				
				if (!GetImageFileName(imageNames[i] ,"%sGU%#.3dIA%d.%d", prefixStr, id,  4,i+idleSprite->GetFirstFrame()))
					GetImageFileName (imageNames[i] ,"%sGU%#.2dIA%d.%d", prefixStr, id,  4,i+idleSprite->GetFirstFrame());
				if (!GetImageFileName(shadowNames[i],"%sGU%#.3dIS%d.%d", prefixStr, id,  4,i+idleSprite->GetFirstFrame()))
					GetImageFileName (shadowNames[i],"%sGU%#.2dIS%d.%d", prefixStr, id,  4,i+idleSprite->GetFirstFrame());
			}
		} 
		else 
		{
			if (type == GROUPTYPE_CITY) 
			{
				printf(" [City");

				for(i=0; i<idleSprite->GetNumFrames(); i++) 
				{
					GetImageFileName(shadowNames[i], "%sGC%#.3dS.%d", prefixStr, id,i+idleSprite->GetFirstFrame());
					GetImageFileName(imageNames[i] , "%sGC%#.3dA.%d", prefixStr, id,i+idleSprite->GetFirstFrame());
				}
			} 
			else 
			{
				Assert(FALSE);
			}
		}

		idleSprite->Import(idleSprite->GetNumFrames(), imageNames, shadowNames);
		m_sprites[UNITACTION_IDLE] = idleSprite;
		printf("]\n");

		Anim *idleAnim = new Anim;

		idleAnim->ParseFromTokens(theToken);
		m_anims[UNITACTION_IDLE] = idleAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_VICTORY, tmp)) return FALSE;    
	if (tmp) 
	{
		Assert(type == GROUPTYPE_UNIT);
		
		if (type != GROUPTYPE_UNIT) 
		{
			printf("\n Illegal unit action (Victory) for a city sprite.\n");
			return FALSE;
		}

		Sprite *victorySprite = new Sprite;

		if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_IS_DEATH, tmp)) return FALSE;
		if (tmp) 
			m_hasDeath = TRUE;
		else 
			m_hasDeath = FALSE;

		victorySprite->ParseFromTokens(theToken);

		printf(" [Victory");
		for(i=0; i<victorySprite->GetNumFrames(); i++) 
		{			
			
			if (!GetImageFileName(shadowNames[i],"%sGU%#.3dVS%d.%d", prefixStr, id,  4,i+victorySprite->GetFirstFrame()))
				GetImageFileName (shadowNames[i],"%sGU%#.2dVS%d.%d", prefixStr, id,  4,i+victorySprite->GetFirstFrame());
			if (!GetImageFileName(imageNames[i], "%sGU%#.3dVA%d.%d", prefixStr, id,  4,i+victorySprite->GetFirstFrame()))
				GetImageFileName (imageNames[i], "%sGU%#.2dVA%d.%d", prefixStr, id,  4,i+victorySprite->GetFirstFrame());
		}

		victorySprite->Import(victorySprite->GetNumFrames(), imageNames, shadowNames);
		m_sprites[UNITACTION_VICTORY] = victorySprite;
		printf("]\n");

		Anim *victoryAnim = new Anim;

		victoryAnim->ParseFromTokens(theToken);
		m_anims[UNITACTION_VICTORY] = victoryAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_WORK, tmp)) return FALSE;    
	if (tmp) 
	{
		Assert(type == GROUPTYPE_UNIT);
		if (type != GROUPTYPE_UNIT) 
		{
			printf("\n Illegal unit action (Work) for a city sprite.\n");
			return FALSE;
		}

		
		FacedSprite *workSprite = new FacedSprite;
		
		
		workSprite->ParseFromTokens(theToken);

		printf(" [Work/A2");
		for (j=0; j<k_NUM_FACINGS; j++) 
		{
			sint32 camera = j + 1;
			for(i=0; i<workSprite->GetNumFrames(); i++) 
			{
				
				if (!GetImageFileName(facedShadowNames[j][i],"%sGU%#.3dWS%d.%d", prefixStr, id, j+1, i+workSprite->GetFirstFrame()))
					GetImageFileName (facedShadowNames[j][i],"%sGU%#.2dWS%d.%d", prefixStr, id, j+1, i+workSprite->GetFirstFrame());
				if (!GetImageFileName(facedImageNames[j][i] ,"%sGU%#.3dWA%d.%d", prefixStr, id, j+1, i+workSprite->GetFirstFrame()))
					GetImageFileName (facedImageNames[j][i] ,"%sGU%#.2dWA%d.%d", prefixStr, id, j+1, i+workSprite->GetFirstFrame());
			}
		}


		
		workSprite->Import(workSprite->GetNumFrames(), facedImageNames, facedShadowNames);

		
		m_sprites[UNITACTION_WORK] = (Sprite *)workSprite;
		printf("]\n");

		Anim *workAnim = new Anim;
		workAnim->ParseFromTokens(theToken);
		m_anims[UNITACTION_WORK] = workAnim;
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_FIREPOINTS, tmp)) return FALSE;    
	
	if (tmp) 
	{
		
	

		for (sint32 j=0; j<tmp; j++) {
			if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;
			for (i=0; i<k_NUM_FACINGS; i++) {
			    token_ParsePoint(theToken);
			}
			if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
		}
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_FIREPOINTS_WORK, tmp)) return FALSE;    
	if (tmp) {
		
		m_numFirePointsWork = (uint16)tmp;

		for (sint32 j=0; j<tmp; j++) {
			if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;
			for (i=0; i<k_NUM_FACINGS; i++) {
			   token_ParsePoint(theToken);
			}
			if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
		}
	}


	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_MOVEOFFSETS, tmp)) return FALSE;    
	if (tmp) {
		if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;
		for (i=0; i<k_NUM_FACINGS; i++) {
			m_moveOffsets[i] = token_ParsePoint(theToken);
		}
		if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
	}

	if (!token_ParseValNext(theToken, TOKEN_UNIT_SPRITE_SHIELDPOINTS, tmp)) return FALSE;    
	if (tmp) {
		
		if (!token_ParseAnOpenBraceNext(theToken)) return FALSE;

		if (!token_ParseKeywordNext(theToken, TOKEN_UNIT_SPRITE_SHIELDPOINTS_MOVE)) return FALSE;    
		if (tmp) {
			for (i=0; i<k_NUM_FACINGS; i++) {
				m_shieldPoints[UNITACTION_MOVE][i] = token_ParsePoint(theToken);
			}
		}
		if (!token_ParseKeywordNext(theToken, TOKEN_UNIT_SPRITE_SHIELDPOINTS_ATTACK)) return FALSE;    
		if (tmp) {
			for (i=0; i<k_NUM_FACINGS; i++) {
				m_shieldPoints[UNITACTION_ATTACK][i] = token_ParsePoint(theToken);
			}
		}
		if (!token_ParseKeywordNext(theToken, TOKEN_UNIT_SPRITE_SHIELDPOINTS_IDLE)) return FALSE;    
		if (tmp) {
			for (i=0; i<k_NUM_FACINGS; i++) {
				m_shieldPoints[UNITACTION_IDLE][i] = token_ParsePoint(theToken);
			}
		}
		if (!token_ParseKeywordNext(theToken, TOKEN_UNIT_SPRITE_SHIELDPOINTS_VICTORY)) return FALSE;    
		if (tmp) {
			for (i=0; i<k_NUM_FACINGS; i++) {
				m_shieldPoints[UNITACTION_VICTORY][i] = token_ParsePoint(theToken);
			}
		}
		if (!token_ParseKeywordNext(theToken, TOKEN_UNIT_SPRITE_SHIELDPOINTS_WORK)) return FALSE;    
		if (tmp) {
			for (i=0; i<k_NUM_FACINGS; i++) {
				m_shieldPoints[UNITACTION_WORK][i] = token_ParsePoint(theToken);
			}
		}
		
		if (!token_ParseAnCloseBraceNext(theToken)) return FALSE;
	}

	

	

	delete theToken;

	for (j=0; j<k_NUM_FACINGS; j++) {
		for (i=0; i<k_MAX_NAMES; i++) 
		{
			free(facedImageNames[j][i]);
			free(facedShadowNames[j][i]);
		}
	}

	for (i=0; i<k_MAX_NAMES; i++) {
		free(imageNames[i]);
		free(shadowNames[i]);
	}

	return TRUE;
}

POINT UnitSpriteGroup::GetHotPoint(UNITACTION action, sint32 facing)
{
	POINT nullPoint = {0,0};

	
	if (action == UNITACTION_IDLE && m_sprites[action] == NULL)
		action = UNITACTION_MOVE;

	if (m_sprites[action] != NULL) {
		if (m_sprites[action]->GetType() == SPRITETYPE_FACED) {
			if (facing >= k_NUM_FACINGS) facing = k_MAX_FACINGS - facing;
			return ((FacedSprite *)m_sprites[action])->GetHotPoint((uint16)facing);
		} else {
			return m_sprites[action]->GetHotPoint();
		}
	}
	return nullPoint;
}




void
UnitSpriteGroup::SetHotPoint(UNITACTION action, sint32 facing,POINT pt)
{
	
	if (action == UNITACTION_IDLE && m_sprites[action] == NULL)
		action = UNITACTION_MOVE;

	if (m_sprites[action] != NULL) 
	{
		if (m_sprites[action]->GetType() == SPRITETYPE_FACED) 
		{
			if (facing >= k_NUM_FACINGS) 
				facing = k_MAX_FACINGS - facing;
			
			((FacedSprite *)m_sprites[action])->SetHotPoint((uint16)facing,pt.x,pt.y);
		} 
		else 
			m_sprites[action]->SetHotPoint(pt.x,pt.y);
	}
}





void UnitSpriteGroup::ExportScript(MBCHAR *name)
{
	sint32				i;
	extern TokenData	g_allTokens[];
	
	FILE * file = fopen(name, "w");
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
	fprintf(file, "%s\n", g_allTokens[TOKEN_UNIT_SPRITE].keyword);
	fprintf(file, "{\n");

	fprintf(file, "\t%s", g_allTokens[TOKEN_UNIT_SPRITE_MOVE].keyword);
	
	
	ExportSpriteGroup(file,(GAME_ACTION)UNITACTION_MOVE,TOKEN_UNIT_SPRITE_MOVE, TOKEN_MAX);








	ExportSpriteGroup(file,(GAME_ACTION)UNITACTION_MOVE,TOKEN_UNIT_SPRITE_ATTACK,TOKEN_UNIT_SPRITE_ATTACK_IS_DIRECTIONAL,HasDirectional());











	ExportSpriteGroup(file,(GAME_ACTION)UNITACTION_IDLE,TOKEN_UNIT_SPRITE_IDLE, TOKEN_MAX);









	ExportSpriteGroup(file,(GAME_ACTION)UNITACTION_VICTORY,TOKEN_UNIT_SPRITE_VICTORY,TOKEN_UNIT_SPRITE_IS_DEATH,HasDeath());










	ExportSpriteGroup(file,(GAME_ACTION)UNITACTION_WORK,TOKEN_UNIT_SPRITE_WORK, TOKEN_MAX);










	
 
 
 
 
 
 
 
	  
 
 
 
 

	
 
 
 
 
 
 
 
 
 
 

	
 
 
 
 
 
 

	
	fprintf(file, "\t%s\t1\n", g_allTokens[TOKEN_UNIT_SPRITE_SHIELDPOINTS].keyword);
	fprintf(file, "\t{\t\n");

	fprintf(file, "\t\t%s\n", g_allTokens[TOKEN_UNIT_SPRITE_SHIELDPOINTS_MOVE].keyword);
	for (i=0; i<k_NUM_FACINGS; i++) {
		fprintf(file, "\t\t\t%d %d\n", m_shieldPoints[UNITACTION_MOVE][i].x, m_shieldPoints[UNITACTION_MOVE][i].y);
	}
	fprintf(file, "\t\t%s\n", g_allTokens[TOKEN_UNIT_SPRITE_SHIELDPOINTS_ATTACK].keyword);
	for (i=0; i<k_NUM_FACINGS; i++) {
		fprintf(file, "\t\t\t%d %d\n", m_shieldPoints[UNITACTION_ATTACK][i].x, m_shieldPoints[UNITACTION_ATTACK][i].y);
	}
	fprintf(file, "\t\t%s\n", g_allTokens[TOKEN_UNIT_SPRITE_SHIELDPOINTS_IDLE].keyword);
	for (i=0; i<k_NUM_FACINGS; i++) {
		fprintf(file, "\t\t\t%d %d\n", m_shieldPoints[UNITACTION_IDLE][i].x, m_shieldPoints[UNITACTION_IDLE][i].y);
	}
	fprintf(file, "\t\t%s\n", g_allTokens[TOKEN_UNIT_SPRITE_SHIELDPOINTS_VICTORY].keyword);
	for (i=0; i<k_NUM_FACINGS; i++) {
		fprintf(file, "\t\t\t%d %d\n", m_shieldPoints[UNITACTION_VICTORY][i].x, m_shieldPoints[UNITACTION_VICTORY][i].y);
	}
	fprintf(file, "\t\t%s\n", g_allTokens[TOKEN_UNIT_SPRITE_SHIELDPOINTS_WORK].keyword);
	for (i=0; i<k_NUM_FACINGS; i++) {
		fprintf(file, "\t\t\t%d %d\n", m_shieldPoints[UNITACTION_WORK][i].x, m_shieldPoints[UNITACTION_WORK][i].y);
	}
	
	fprintf(file, "\t}\n\n");

	fprintf(file, "}\n");

	fclose(file);
}
