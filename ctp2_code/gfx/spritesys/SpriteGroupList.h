//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Sprite Group List
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
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Increased sprite limit from 200 to 255.
// - Marked Microsoft extensions.
// - Removed useless - and memory wasting - SpriteList dependency.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef SPRITEGROUPLIST_H__
#define SPRITEGROUPLIST_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

// None 

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class	SpriteGroupList;

enum	SPRITELISTERR 
{
		SPRITELISTERR_OK,
		SPRITELISTERR_NOTFOUND,
		SPRITELISTERR_BADTYPE,
		SPRITELISTERR_MAX
};

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "Action.h"			// GAME_ACTION
#include "c3types.h"		// uint8, uint32
#include "SpriteGroup.h"	// GROUPTYPE, LOADTYPE, SpriteGroup

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

#if USE_FORMAT_67
uint32 const    k_MAX_SPRITES   = 511;
#else
uint8  const    k_MAX_SPRITES   = 255;
// uint8 restriction: see UnitActor::Serialize. Going beyond 255 will require
// a file format update.
#endif

class SpriteGroupList
{
public:
	SpriteGroupList();
	virtual ~SpriteGroupList();

	SPRITELISTERR	LoadSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action);

	SpriteGroup *   GetSprite(uint32 index, GROUPTYPE type, LOADTYPE loadType,GAME_ACTION action);
	bool            ReleaseSprite(uint32 index, LOADTYPE loadType);

	void            RefreshBasicLoads(GROUPTYPE groupType);

private:
	SpriteGroup *   m_spriteList[k_MAX_SPRITES];
};

//----------------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------------

void spritegrouplist_Initialize(void);
void spritegrouplist_Cleanup(void);

#endif	// Multiple include guard
