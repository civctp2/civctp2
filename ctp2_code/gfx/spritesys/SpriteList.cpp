//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite list
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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Completely inactivated. 
//
//----------------------------------------------------------------------------


#include "c3.h"	// Left in to prevent strange compiler errors.

#if defined(ACTIVISION_ORIGINAL)	// Unused code

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

#endif	// ACTIVISION_ORIGINAL)