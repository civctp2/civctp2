#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __UNITSPRITEGROUP_H__
#define __UNITSPRITEGROUP_H__

#include "FacedSprite.h"
#include "SpriteGroup.h"
#include "Action.h"

#include "aui_directsurface.h"

enum UNITACTION
{
	UNITACTION_FACE_OFF = -4,
	UNITACTION_MORPH = -3,
	UNITACTION_FAKE_DEATH = -2,
	UNITACTION_NONE = -1,

	UNITACTION_MOVE,
	UNITACTION_ATTACK,
	UNITACTION_IDLE,
	UNITACTION_VICTORY,
	UNITACTION_WORK,

	UNITACTION_MAX
};

class Sprite;
class FacedSprite;
class Anim;

class UnitSpriteGroup : public SpriteGroup {
public:
	UnitSpriteGroup(GROUPTYPE type);
	virtual ~UnitSpriteGroup();

	void			DeallocateStorage(void);
	void			DeallocateFullLoadAnims(void);

	void			LoadBasic(const char *filename);
	void			LoadIndexed(const char *filename, GAME_ACTION index);
	void			LoadFull(const char *filename);

	bool			GetImageFileName(char *name, const char *format,...);

	void			Save(const char *filename,unsigned version_id,unsigned compression_mode);

	void			Draw(UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack);

	void			DrawText(sint32 x, sint32 y, const char *s);
	void			DrawDirect(aui_Surface *surf, UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY,
							   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags,
							   BOOL specialDelayProcess,
							   BOOL directionalAttack);

	void			RunBenchmark(aui_Surface *surf);





	POINT			*GetShieldPoints(UNITACTION action) { return m_shieldPoints[action]; }











	uint16			GetNumFirePointsWork(void) { return m_numFirePointsWork; }
	void			SetNumFirePointsWork(uint16 num) { m_numFirePointsWork = num; }

	sint32			Parse(uint16 id,GROUPTYPE type);
	void			ExportScript(const char *name);

	POINT			GetHotPoint(UNITACTION action, sint32 facing);
	void			SetHotPoint(UNITACTION action, sint32 facing,POINT pt);

	BOOL			HitTest(POINT mousePt, UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY,
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack);

private:


	uint16			m_numFirePointsWork;
	POINT			m_firePointsWork[k_NUM_FIREPOINTS][k_NUM_FACINGS];

	POINT			m_moveOffsets[k_NUM_FACINGS];

	POINT			m_shieldPoints[UNITACTION_MAX][k_NUM_FACINGS];


};

#endif
