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
#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __GOODSPRITEGROUP_H__
#define __GOODSPRITEGROUP_H__

#include "FacedSprite.h"
#include "SpriteGroup.h"

#define k_NOT_GOOD		-1

enum GOODACTION {
	GOODACTION_NONE = -1,

	GOODACTION_IDLE,

	GOODACTION_MAX
};

class aui_Surface;

class Sprite;

class Anim;

class GoodSpriteGroup : public SpriteGroup {
public:
	GoodSpriteGroup(GROUPTYPE type);
	virtual ~GoodSpriteGroup();

	void			DeallocateStorage(void);
	void			DeallocateFullLoadAnims(void);

	void			LoadBasic(const char *filename);
	void			LoadFull(const char *filename);

	void			Save(const char *filename,unsigned version_id,unsigned compression_mode);

	void			ExportScript(const char *name);

	void			Draw(GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void			DrawDirect(aui_Surface *surf, GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags);

	void			DrawText(sint32 x, sint32 y, const char *s);

	void			RunBenchmark(aui_Surface *surf);





	POINT			GetHotPoint(GOODACTION action);


	sint32			Parse(uint16 id,GROUPTYPE group);

private:

};

#endif
