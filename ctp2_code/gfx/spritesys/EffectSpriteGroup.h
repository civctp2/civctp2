










 

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __EFFECTSPRITEGROUP_H__
#define __EFFECTSPRITEGROUP_H__

#include "FacedSpriteWshadow.h"
#include "SpriteGroup.h"

#define k_NUM_FIREPOINTS		8

enum EFFECTACTION {
	EFFECTACTION_NONE = -1,
	
	EFFECTACTION_PLAY,
	EFFECTACTION_FLASH,

	EFFECTACTION_MAX
};

class aui_Surface;
class aui_DirectSurface;

class Sprite;
class Anim;

class EffectSpriteGroup : public SpriteGroup {
public:
	EffectSpriteGroup(GROUPTYPE type);
	virtual ~EffectSpriteGroup();

	void			Load(char *filename);
	void			Save(char *filename,unsigned version_id,unsigned compression_mode);

	void			LoadBasic(char *filename){Load(filename);};
	void			LoadFull (char *filename){Load(filename);};

	void			ExportScript(MBCHAR *name);

	void			Draw(EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY, sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack = FALSE);
	void			DrawDirect(aui_Surface *surf, EFFECTACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY, sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack = FALSE);











	sint32			Parse(uint16 id,GROUPTYPE group);
	POINT			GetHotPoint(EFFECTACTION action, sint32 facing);


private:



};

#endif
