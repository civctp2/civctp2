










 

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
class aui_DirectSurface;

class Sprite;

class Anim;

class GoodSpriteGroup : public SpriteGroup {
public:
	GoodSpriteGroup(GROUPTYPE type);
	virtual ~GoodSpriteGroup();

	void			LoadBasic(char *filename);
	void			LoadFull(char *filename);

	void			Save(char *filename,unsigned version_id,unsigned compression_mode);

	void			ExportScript(MBCHAR *name);

	void			Draw(GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags);
	void			DrawDirect(aui_Surface *surf, GOODACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags);

	void			DrawText(sint32 x, sint32 y, char *s);

	void			RunBenchmark(aui_Surface *surf);










 
	POINT			GetHotPoint(GOODACTION action);


 
	
	sint32			Parse(uint16 id,GROUPTYPE group);

private:





};


#endif
