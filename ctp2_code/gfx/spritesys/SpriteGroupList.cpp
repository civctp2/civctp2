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
// - Removed double Assert reporting when LoadSprite fails.
//
//----------------------------------------------------------------------------
 
#include "c3.h"
#include "SpriteGroupList.h"

#include "c3errors.h"
#include "c3files.h"
#include "CivPaths.h"               // g_civPaths
#include "EffectSpriteGroup.h"
#include "Globals.h"                // allocated::clear, allocated::reassign
#include "GoodSpriteGroup.h"
#include "progresswindow.h"
#include "Sprite.h"
#include "StrDB.h"                  // g_theStringDB
#include "UnitSpriteGroup.h"

extern ProgressWindow * g_theProgressWindow;


SpriteGroupList	*       g_unitSpriteGroupList   = NULL;
SpriteGroupList	*       g_effectSpriteGroupList = NULL;
SpriteGroupList	*       g_goodSpriteGroupList   = NULL;
SpriteGroupList	*       g_citySpriteGroupList   = NULL;



void spritegrouplist_Initialize(void)
{
    allocated::reassign(g_unitSpriteGroupList, new SpriteGroupList());
    allocated::reassign(g_effectSpriteGroupList, new SpriteGroupList());
    allocated::reassign(g_goodSpriteGroupList, new SpriteGroupList());
    allocated::reassign(g_citySpriteGroupList, new SpriteGroupList());
}

void spritegrouplist_Cleanup(void)
{
    allocated::clear(g_unitSpriteGroupList);
    allocated::clear(g_effectSpriteGroupList);
    allocated::clear(g_goodSpriteGroupList);
    allocated::clear(g_citySpriteGroupList);
}


SpriteGroupList::SpriteGroupList()
{
    std::fill(m_spriteList, m_spriteList + k_MAX_SPRITES, (SpriteGroup *) NULL);
}


SpriteGroupList::~SpriteGroupList()
{
	for (size_t i = 0; i < k_MAX_SPRITES; ++i) 
    {
		delete m_spriteList[i];
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

SpriteGroup *SpriteGroupList::GetSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action)
{
	Assert(index >= 0);
	Assert(index < k_MAX_SPRITES);

	if((index < 0) || (index >= k_MAX_SPRITES))
		index = 0;

	SpriteGroup *group = m_spriteList[index];

	if (group) 
	{
		if (loadType != group->GetLoadType()) 
		{
			if ((loadType==LOADTYPE_FULL)||(loadType==LOADTYPE_INDEXED)) 
			{
				group->DeallocateStorage();
                (void) LoadSprite(index, type, loadType, action);
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
	Assert(index >= 0);
	Assert(index < k_MAX_SPRITES);

	if (index < 0 || index >= k_MAX_SPRITES)
		return true; // Old behaviour

	if (m_spriteList[index] == NULL)
		return true; // Old behaviour

	LOADTYPE	groupLoadType = m_spriteList[index]->GetLoadType();

	if (loadType == LOADTYPE_FULL) 
    {
		if (groupLoadType == LOADTYPE_FULL) 
		{
			m_spriteList[index]->ReleaseFullLoad();
		}
	} 
    else 
    {
		m_spriteList[index]->Release();
	}

    if (0 == m_spriteList[index]->GetFullLoadRefCount())
    {
	    sint32	basicRefs   = m_spriteList[index]->GetRefCount(); 

        if (basicRefs == 0)
        {
		    delete m_spriteList[index];
            m_spriteList[index] = NULL;
		    return true;
	    }
        else if (basicRefs > 0)
	    {
		    if (groupLoadType == LOADTYPE_FULL) 
		    {
			    m_spriteList[index]->DeallocateStorage();
			    m_spriteList[index]->DeallocateFullLoadAnims();

			    (void) LoadSprite(index, 
                                  m_spriteList[index]->GetType(), 
                                  LOADTYPE_BASIC, 
                                  (GAME_ACTION) 0
                                 );
		    }
        }
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

		if (usg && (usg->GetLoadType() == LOADTYPE_BASIC)) 
        {
			usg->DeallocateStorage();
			(void) LoadSprite(i, groupType, LOADTYPE_BASIC,(GAME_ACTION)0);
		}

		g_theProgressWindow->StartCountingTo( i, s );
	}

	ProgressWindow::EndProgress( g_theProgressWindow );
}
