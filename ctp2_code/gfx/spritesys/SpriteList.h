//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Sprite List
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
// _DEBUG
// - Generate debug version when set.
//
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Increased sprite limit from 200 to 500 by Martin Gühmann
//
//----------------------------------------------------------------------------

#pragma once
#ifndef __SPRITELIST_H__
#define __SPRITELIST_H__

#if defined(ACTIVISION_ORIGINAL)
//Removed by Martin Gühmann
#define k_MAX_SPRITES		200
#else
//Added by Martin Gühmann
#define k_MAX_SPRITES		500
#endif


enum SPRITELISTERR {
	SPRITELISTERR_OK,

	SPRITELISTERR_NOTFOUND,
	SPRITELISTERR_BADTYPE,

	SPRITELISTERR_MAX
};

class Sprite;

class SpriteList {
public:
	SpriteList();
	virtual ~SpriteList();

	virtual SPRITELISTERR	LoadSprite(uint32 index);
	virtual SPRITELISTERR	PurgeSprite(uint32 index);

	Sprite					*GetSprite(uint32 index);

private:
	Sprite					*m_spriteList[k_MAX_SPRITES];
	sint32					m_listSize;
};

#endif
