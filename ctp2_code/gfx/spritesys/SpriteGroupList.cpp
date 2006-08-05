//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite group list 
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
// - Unused spritelist.h include removed.
// - Option added to include multiple data directories.
// - Corrected crash when loading a saved game (only in release version?).
//
//----------------------------------------------------------------------------
 
#include "c3.h"

#include "c3errors.h"

#include "c3files.h"

#include "pixelutils.h"
#include "Sprite.h"
#include "FacedSprite.h"
#include "FacedSpriteWshadow.h"
#include "UnitSpriteGroup.h"

#include "EffectSpriteGroup.h"
#include "GoodSpriteGroup.h"
#include "SpriteGroupList.h"

#include "progresswindow.h"
#include "StrDB.h"

#include "CivPaths.h"
extern CivPaths			*g_civPaths;

extern ProgressWindow	*g_theProgressWindow;
extern StringDB			*g_theStringDB;

SpriteGroupList	*g_unitSpriteGroupList;

SpriteGroupList	*g_effectSpriteGroupList;
SpriteGroupList	*g_goodSpriteGroupList;
SpriteGroupList	*g_citySpriteGroupList;



void spritegrouplist_Initialize(void)
{
 	g_unitSpriteGroupList = new SpriteGroupList();
	Assert(g_unitSpriteGroupList != NULL);
	if (g_unitSpriteGroupList == NULL) {
		c3errors_FatalDialog("SpriteGroupList", "%s", "Could not create the Unit Sprite List.");
	}







	g_effectSpriteGroupList = new SpriteGroupList();
	Assert(g_effectSpriteGroupList != NULL);
	if (g_effectSpriteGroupList == NULL) {
		c3errors_FatalDialog("SpriteGroupList", "%s", "Could not create the Effect Sprite List.");
	}

	g_goodSpriteGroupList = new SpriteGroupList();
	Assert(g_goodSpriteGroupList != NULL);
	if (g_goodSpriteGroupList == NULL) {
		c3errors_FatalDialog("SpriteGroupList", "%s", "Could not create the good Sprite List.");
	}

	g_citySpriteGroupList = new SpriteGroupList();
	Assert(g_citySpriteGroupList != NULL);
	if (g_citySpriteGroupList == NULL) {
		c3errors_FatalDialog("SpriteGroupList", "%s", "Could not create the City Sprite List.");
	}
}



void spritegrouplist_Cleanup(void)
{
	delete g_unitSpriteGroupList;
	g_unitSpriteGroupList = NULL;




	delete g_effectSpriteGroupList;
	g_effectSpriteGroupList = NULL;

	delete g_goodSpriteGroupList;
	g_goodSpriteGroupList = NULL;

	delete g_citySpriteGroupList;
	g_citySpriteGroupList = NULL;
}



SpriteGroupList::SpriteGroupList()
{
	for (sint32 i=0; i<k_MAX_SPRITES; i++) {
		m_spriteList[i] = NULL;
	}
}



SpriteGroupList::~SpriteGroupList()
{
	for (sint32 i=0; i<k_MAX_SPRITES; i++) {
		PurgeSprite(i);
	}
}


#define _BASIC_SPRITE_LOAD_

SPRITELISTERR SpriteGroupList::LoadSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action)
{
	char			inFile[_MAX_PATH];
	SpriteGroup		*newSpriteGroup=m_spriteList[index];

	
	switch (type) 
	{
    case GROUPTYPE_UNIT : 
	if(newSpriteGroup==NULL)
		newSpriteGroup = new UnitSpriteGroup(type);

        // A unit sprite file may have 3 or 2 digits in the name. 
	sprintf(inFile, "GU%.3d.SPR", index);
		 
	MBCHAR fullPath[_MAX_PATH];
        if (!g_civPaths->FindFile(C3DIR_SPRITES, inFile, fullPath, TRUE, FALSE))
        {
		// No 3 digit version found: try the 2 digit version.
		sprintf(inFile, "GU%.2d.SPR", index);
	}
	break;

	case GROUPTYPE_PROJECTILE : 
		 Assert("Projectile Actors Removed From Game - CJI"==NULL);
		 return SPRITELISTERR_NOTFOUND;

	case GROUPTYPE_EFFECT : 
		 if(newSpriteGroup==NULL)
		    newSpriteGroup = new EffectSpriteGroup(type);
		 sprintf(inFile, "GX%.2d.SPR", index);
	  
		 break;
	case GROUPTYPE_CITY:
		 if(newSpriteGroup==NULL)
		    newSpriteGroup = new UnitSpriteGroup(type);
		 sprintf(inFile, "GC%.3d.SPR", index);
		 break;
	case GROUPTYPE_GOOD:
		 if(newSpriteGroup==NULL)
		    newSpriteGroup = new GoodSpriteGroup(type);
		 sprintf(inFile, "GG%.3d.SPR", index);
		 break;
	default:
		Assert(type > GROUPTYPE_GROUP && type < GROUPTYPE_MAX);
		return SPRITELISTERR_BADTYPE;
	}

	
	Assert(newSpriteGroup != NULL);

	
	if(newSpriteGroup==NULL) 
	   return SPRITELISTERR_NOTFOUND;

	
	switch(loadType)
	{
	case LOADTYPE_BASIC:
		 newSpriteGroup->LoadBasic(inFile);
		 break;
	case LOADTYPE_FULL:
		 newSpriteGroup->LoadFull(inFile);
		 break;
	case LOADTYPE_INDEXED:
		 newSpriteGroup->LoadIndexed(inFile,action);
		 break;
	default:
		 Assert(FALSE);
		 return SPRITELISTERR_NOTFOUND;
	};

	
    m_spriteList[index] = newSpriteGroup;

	return SPRITELISTERR_OK;
}



SPRITELISTERR SpriteGroupList::PurgeSprite(uint32 index)
{
	if (m_spriteList[index] != NULL) 
	{
		delete m_spriteList[index];
		m_spriteList[index] = NULL;
	}
	
	return SPRITELISTERR_OK;
}



SpriteGroup *SpriteGroupList::GetSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action)
{
	SPRITELISTERR err;

	Assert(index >= 0);
	Assert(index < k_MAX_SPRITES);

	if((index < 0) || (index >= k_MAX_SPRITES))
		index = 0;

	SpriteGroup *group = m_spriteList[index];

	if (group) 
	{
		if (loadType != group->GetLoadType()) 
		{
			if((loadType==LOADTYPE_FULL)||(loadType==LOADTYPE_INDEXED)) 
			{
				
				group->DeallocateStorage();
				err = LoadSprite(index, type, loadType,action);
				Assert(err == SPRITELISTERR_OK);
			}
		}
	}
	else 
    {
        if (SPRITELISTERR_OK == LoadSprite(index, type, loadType, action))
        {
            // No action: m_spriteList[index] initialised with data from file
        }
        else
        {
            // Failure has been Asserted in LoadSprite
            return NULL;
        }
	} 

	switch (loadType) 
	{
	case LOADTYPE_BASIC:
		 m_spriteList[index]->AddRef();
		 break;
	case LOADTYPE_FULL:
	case LOADTYPE_INDEXED:
		 m_spriteList[index]->AddFullLoadRef();
		 break;
	default:
		 Assert(loadType == LOADTYPE_BASIC || loadType == LOADTYPE_FULL);
		 break;
	}

	return m_spriteList[index];
}




bool SpriteGroupList::ReleaseSprite(uint32 index, LOADTYPE loadType)
{
	SPRITELISTERR err;

	Assert(index >= 0);
	Assert(index < k_MAX_SPRITES);

	if (index < 0 || index >= k_MAX_SPRITES)
		return true; // Old behaviour

	if (m_spriteList[index] == NULL)
		return true; // Old behaviour

	LOADTYPE	groupLoadType = m_spriteList[index]->GetLoadType();

	
	if (loadType == LOADTYPE_FULL) {
		
		if (groupLoadType == LOADTYPE_FULL) 
		{
			
			m_spriteList[index]->ReleaseFullLoad();
		} else 
		{
			
		}
	} else {
		
		m_spriteList[index]->Release();
	}

	sint32	basicRefs = m_spriteList[index]->GetRefCount(), 
			fullRefs = m_spriteList[index]->GetFullLoadRefCount();

	
	if (basicRefs == 0 && fullRefs == 0) {
		PurgeSprite(index);
		return true;
	}

	
	if (basicRefs >= 0 && fullRefs == 0) 
	{
		
		
		if (groupLoadType == LOADTYPE_FULL) 
		{
			m_spriteList[index]->DeallocateStorage();
			m_spriteList[index]->DeallocateFullLoadAnims();

			err = LoadSprite(index, m_spriteList[index]->GetType(), LOADTYPE_BASIC,(GAME_ACTION)0); // No idea why, but without releasing the old sprite a bad idea
			Assert(err == SPRITELISTERR_OK);
		} else {
			
			
		}
		return false;
	}

	
	if (fullRefs >= 0) {
		
	}

	return false;
}












void SpriteGroupList::RefreshBasicLoads(GROUPTYPE groupType)
{

	ProgressWindow::BeginProgress(
		g_theProgressWindow,
		"InitProgressWindow",
		k_MAX_SPRITES-1 );

	MBCHAR s[_MAX_PATH];
	sprintf( s, g_theStringDB->GetNameStr("LOADING") );

	for (sint32 i=0; i<k_MAX_SPRITES; i++) 
	{
		UnitSpriteGroup *usg = (UnitSpriteGroup *)m_spriteList[i];

		if (usg != NULL) 
		{
			
			
			
			
			if (usg->GetLoadType() == LOADTYPE_BASIC) {
				usg->DeallocateStorage();

				SPRITELISTERR err = LoadSprite(i, groupType, LOADTYPE_BASIC,(GAME_ACTION)0);
				Assert(err == SPRITELISTERR_OK);
			}
		}
		g_theProgressWindow->StartCountingTo( i, s );
	}

	ProgressWindow::EndProgress( g_theProgressWindow );
}
