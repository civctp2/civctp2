//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
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
// - Fixed memory leaks.
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef GOODSPRITEGROUP_H__
#define GOODSPRITEGROUP_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include "windows.h"          // POINT

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class GoodSpriteGroup;

enum GOODACTION {
	GOODACTION_NONE = -1,
	
	GOODACTION_IDLE,

	GOODACTION_MAX
};

#define k_NOT_GOOD		-1

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"    // sint32, uint16
#include "pixeltypes.h"		// Pixel16
#include "SpriteGroup.h"	// SpriteGroup, GROUPTYPE

class aui_Surface;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------
 
class GoodSpriteGroup : public SpriteGroup 
{
public:
	GoodSpriteGroup(GROUPTYPE type) : SpriteGroup(type) {};

	void			DeallocateStorage(void);
	void			DeallocateFullLoadAnims(void);

	void			LoadBasic(MBCHAR const * filename);
	void			LoadFull(MBCHAR const * filename);

	void			Save(MBCHAR const * filename, unsigned int version_id, unsigned int compression_mode);

	void			ExportScript(MBCHAR const * name);

	void			Draw(GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void			DrawDirect(aui_Surface *surf, GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags);

	void			DrawText(sint32 x, sint32 y, MBCHAR const * s);

	POINT			GetHotPoint(GOODACTION action);


 
	sint32			Parse(uint16 id,GROUPTYPE group);
};


#endif
