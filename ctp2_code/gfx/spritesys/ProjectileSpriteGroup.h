










 


#pragma once
#ifndef __PROJECTILESPRITEGROUP_H__
#define __PROJECTILESPRITEGROUP_H__

#include "FacedSpriteWshadow.h"
#include "SpriteGroup.h"

#define k_NUM_FIREPOINTS		8

enum PROJECTILEACTION {
	PROJECTILEACTION_NONE = -1,
	
	PROJECTILEACTION_MOVE,

	PROJECTILEACTION_MAX
};

class aui_Surface;
class aui_DirectSurface;

class Sprite;
class FacedSpriteWshadow;

class Anim;

class ProjectileSpriteGroup : public SpriteGroup {
public:
	ProjectileSpriteGroup(GROUPTYPE type);
	virtual ~ProjectileSpriteGroup();

	void			Load(char *filename);
	void			Save(char *filename);

	void			Draw(PROJECTILEACTION action, sint32 frame, sint32 drawX, sint32 drawY, sint32 SdrawX, sint32 SdrawY, sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack = FALSE);

	void			DrawText(sint32 x, sint32 y, char *s);

	void			RunBenchmark(aui_Surface *surf);

	sint32			GetWidth(void) { return m_width; };
	sint32			GetHeight(void) { return m_height; };

	sint32			GetNumFrames(PROJECTILEACTION action);

	FacedSpriteWshadow	*GetGroupSprite(PROJECTILEACTION action) { return m_sprites[action]; }
	void			SetGroupSprite(PROJECTILEACTION action, FacedSpriteWshadow *sprite) { m_sprites[action] = sprite; }

	Anim			*GetGroupAnim(PROJECTILEACTION action) { return m_anims[action]; }
	void			SetGroupAnim(PROJECTILEACTION action, Anim *anim) { m_anims[action] = anim; }


	POINT			*GetFirePoints(uint16 which) { return m_firePoints[which]; }

	POINT			*GetMoveOffsets(void) { return m_moveOffsets; }

	Anim			*GetAnim(PROJECTILEACTION action) { return m_anims[action]; }

	BOOL			HasDeath(void) { return m_hasDeath; }
	void			SetHasDeath(BOOL val) { m_hasDeath = val; }

	BOOL			HasDirectional(void) { return m_hasDirectional; }
	void			SetHasDirectional(BOOL val) { m_hasDirectional = val; }
	
	uint16			GetNumFirePoints(void) { return m_numFirePoints; }
	void			SetNumFirePoints(uint16 num) { m_numFirePoints = num; }

	sint32			Parse(uint16 id);

	POINT			GetHotPoint(PROJECTILEACTION action, sint32 facing);

private:
	sint32				m_width, m_height;

	FacedSpriteWshadow	*m_sprites[PROJECTILEACTION_MAX];
	Anim				*m_anims[PROJECTILEACTION_MAX];

	uint16				m_numFirePoints;
	POINT				m_firePoints[k_NUM_FIREPOINTS][k_NUM_FACINGS];
	
	POINT				m_moveOffsets[k_NUM_FACINGS];
	
	BOOL				m_hasDeath;
	BOOL				m_hasDirectional;
};


#endif