










 

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef UNITSPRITEGROUP_H__
#define UNITSPRITEGROUP_H__

//----------------------------------------------------------------------------
// Library dependencies
//----------------------------------------------------------------------------

#include "windows.h"          // BOOL, POINT

//----------------------------------------------------------------------------
// Export overview
//----------------------------------------------------------------------------

class UnitSpriteGroup;

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

//----------------------------------------------------------------------------
// Project dependencies
//----------------------------------------------------------------------------

#include "Action.h"         // GAME_ACTION
#include "ctp2_inttypes.h"  // sint32, uint16
#include "pixeltypes.h"		// Pixel16
#include "SpriteGroup.h"	// SpriteGroup, GROUPTYPE

class aui_Surface;

//----------------------------------------------------------------------------
// Class declarations
//----------------------------------------------------------------------------

class UnitSpriteGroup : public SpriteGroup 
{
public:
	UnitSpriteGroup(GROUPTYPE type);

	void			DeallocateStorage(void);
	void			DeallocateFullLoadAnims(void);

	void			LoadBasic(MBCHAR const * filename);
	void			LoadIndexed(MBCHAR const * filename, GAME_ACTION index);
	void			LoadFull(MBCHAR const * filename);

	bool			GetImageFileName(MBCHAR * name, char * format, ...);

	void			Save(MBCHAR const * filename, unsigned int version_id, unsigned int compression_mode);

	void			Draw(UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
						   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, BOOL specialDelayProcess, BOOL directionalAttack);

	void			DrawText(sint32 x, sint32 y, MBCHAR const * s);
	void			DrawDirect(aui_Surface *surf, UNITACTION action, sint32 frame, sint32 drawX, sint32 drawY, 
							   sint32 facing, double scale, uint16 transparency, Pixel16 outlineColor, uint16 flags, 
							   BOOL specialDelayProcess, 
							   BOOL directionalAttack);


	POINT *         GetShieldPoints(UNITACTION action) { return m_shieldPoints[action]; }











	uint16			GetNumFirePointsWork(void) { return m_numFirePointsWork; }
	void			SetNumFirePointsWork(uint16 num) { m_numFirePointsWork = num; }

	sint32			Parse(uint16 id,GROUPTYPE type);
	void			ExportScript(MBCHAR const * name);

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
