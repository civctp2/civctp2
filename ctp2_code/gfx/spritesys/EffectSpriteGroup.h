#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef EFFECTSPRITEGROUP_H__
#define EFFECTSPRITEGROUP_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include "windows.h"    // POINT

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class EffectSpriteGroup;

enum EFFECTACTION {
	EFFECTACTION_NONE = -1,

	EFFECTACTION_PLAY,
	EFFECTACTION_FLASH,

	EFFECTACTION_MAX
};

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "SpriteGroup.h"

class aui_Surface;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class EffectSpriteGroup : public SpriteGroup
{
public:
	EffectSpriteGroup(GROUPTYPE type) : SpriteGroup(type) {};

	void			Load(MBCHAR const * filename);
	void			Save(MBCHAR const * filename,unsigned int version_id, unsigned int compression_mode);

	void			LoadBasic(MBCHAR const * filename) { Load(filename); };
	void			LoadFull (MBCHAR const * filename) { Load(filename); };

	void			ExportScript(MBCHAR const * name);

	void			Draw(EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY, sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack = FALSE);
	void			DrawDirect(aui_Surface *surf, EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY, sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack = FALSE);

	sint32			Parse(uint16 id,GROUPTYPE group);
	POINT			GetHotPoint(EFFECTACTION action, sint32 facing);
};

#endif
