//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit actor
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
// _DEBUG
// - Generates debug information when set.
//
// _ACTOR_DRAW_OPTIMIZATION
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Unit stacking indications and special indecations are placed according
//   their size. (9-Feb-2008 Martin Gühmann)
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __UNITACTOR_H__
#define __UNITACTOR_H__

#include "Actor.h"              // Actor
#include "UnitSpriteGroup.h"    // UNITACTION
#include "Queue.h"
#include "ctp2_inttypes.h"      // sintN, uintN
#include "Unit.h"               // SPECATTACK, Unit
#include "tileset.h"            // MAPICON

class aui_Surface;
class CivArchive;
class SpriteState;
class SpriteGroupList;
class Action;
class Anim;
// POINT, RECT

class UnitActor : public Actor
{
public:
	// Make position methods public
	using Actor::GetX;
	using Actor::GetY;

	UnitActor(
		SpriteState    * spriteState,
		const Unit     & id,
		sint32           unitType,
		const MapPoint & pos,
		sint32           owner,
		double           visionRange,
		sint32           citySprite,
		sint32           citySize = 0);

	UnitActor(CivArchive & archive);

	virtual ~UnitActor();

	void             Serialize(CivArchive & archive);

	uint32           GetUnitID() const { return m_unitID.m_id; }
	sint32           GetUnitDBIndex() const { return m_unitDBIndex; }

	sint32           GetPlayerNum() const { return m_playerNum; }
	void             SetPlayerNum(sint32 playerNum) { m_playerNum = playerNum; }

	void             Process();
	void             AddAction(Action * action);
	bool             IsActionFinished() const;

	bool             Draw(bool fogged = false);
	void             Draw(const RECT & paintRect);
	void             DrawDirect(aui_Surface * surf, sint32 x, sint32 y, double scale) const;

	uint16           GetWidth() const;
	uint16           GetHeight() const;
	void             GetBoundingRect(RECT * rect) const;

	void             PositionActor(const MapPoint & pos);
	const MapPoint & GetMapPos() const { return m_pos; }
	void             Show();
	void             Hide();

	uint32           GetUnitVisibility() const { return m_unitVisibility; }
	void             SetUnitVisibility(uint32 unitVisibility) { m_unitVisibility = unitVisibility; }
	void             SetIsFortified(bool fortified) { m_isFortified = fortified; }
	void             SetIsFortifying(bool fortifying) { m_isFortifying = fortifying; }
	void             SetHasCityWalls(bool hasCityWalls) { m_hasCityWalls = hasCityWalls; }
	void             SetHasForceField(bool hasForceFields) { m_hasForceField = hasForceFields; }
	sint32           GetCitySize() const { return m_citySize; }
	sint32           GetNextPop() const { return m_nextPop;}

	void             SetHiddenUnderStack(bool hiddenUnderStack) { m_hiddenUnderStack = hiddenUnderStack; }
	void             SetHealthPercent(double healthPercent) { m_healthPercent = healthPercent;}
	void             SetTempStackSize(sint32 tempStackSize) { m_tempStackSize = tempStackSize; }

	Anim           * CreateMoveAnim();
	Anim           * CreateDeadAnim(UNITACTION & unitAction);
	Anim           * CreateAttackAnim();
	Anim           * CreateSpecialAttackAnim();
	Anim           * CreateWorkAnim();
	bool             HasAnim(UNITACTION action) const {
		return m_unitSpriteGroup && m_unitSpriteGroup->GetAnim((GAME_ACTION) action);
	}
	void             ChangeImage(SpriteState *spriteState, sint32 type, const Unit & id);
	void             ChangeType(SpriteState *spriteState, sint32 type, Unit id);

	bool             HitTest(const POINT & mousePoint) const;

	// Sprite-editor initialize
	void             HackSetSpriteID(sint32 spriteID) { m_spriteID = spriteID; }

	// UnseenCell
	void             IncreaseReferenceCount() { m_refCount++; }
	bool             DecreaseReferenceCount() { return --m_refCount <= 0; }
	void             CopyFlags(const UnitActor & other);

#ifdef _DEBUG
	void   DumpActor();
#endif

protected:
	static SpriteGroupList * GetSpriteGroupList(GROUPTYPE groupType);
	static bool              IsRectCompletelyOnScreen(const RECT & rect);

	void   Initialize();

	void     GetIDAndType(
		sint32           owner,
		SpriteState    * spriteState,
		Unit             id,
		sint32           unitType,
		const MapPoint & pos,
		sint32           citySprite,
		sint32         & spriteID,
		GROUPTYPE      & groupType) const;

	void     GetNextAction();
	void     AddIdle();
	void     Interrupt();
	void     DumpAllActions();

	Anim   * TryAnimation(UNITACTION action);
	Anim   * CreateAnim(UNITACTION action) const;
	void     FullLoad(UNITACTION action);
	void     DumpFullLoad();

	void     DrawFortifying(sint32 xoffset, sint32 yoffset, bool fogged) const;
	void     DrawFortified(sint32 nudgeX, sint32 nudgeY, bool fogged) const;
	void     DrawCityWalls(sint32 nudgeX, sint32 nudgeY, bool fogged) const;
	void     DrawForceField(sint32 nudgeX, sint32 nudgeY, bool fogged) const;
	void     DrawCityImprovements(sint32 nudgeX, sint32 nudgeY, bool fogged) const; //emod
	void     DrawSelectionBrackets() const;
	void     DrawShield() const;
	RECT     DrawStackingIndicator(const RECT & rect, sint32 stackSize) const;
	RECT     DrawIndicators(const RECT & rect) const;
	RECT     DrawIndicator(const RECT & rect, MAPICON icon, Pixel16 displayedColor) const;
	RECT     DrawSpecialIndicators(const RECT & rect, sint32 stackSize) const;
	void     DrawText(sint32 x, sint32 y, MBCHAR * unitText) const;
	void     DrawImage(Pixel16 * image, sint32 nudgeX, sint32 nudgeY, bool fogged) const;
	Sprite * GetSprite() const;
	RECT     DetermineShieldRect() const;
	sint32   DetermineStackSize() const;
	double   CalculateHealthPercentage(sint32 stackSize) const;
	void     DrawHealthBar(const RECT & rect, sint32 stackSize) const;
	sint32   GetDisplayedOwner() const;
	Pixel16  GetDisplayedColor() const;

	Unit              m_unitID;
	sint32            m_unitDBIndex;
	sint32            m_playerNum;
	MapPoint          m_pos;
	bool              m_hidden;

	GROUPTYPE         m_type;
	Action          * m_curAction;
	UNITACTION        m_curUnitAction;
	Queue<Action *>   m_actionQueue;

	uint32            m_unitVisibility;
	bool              m_isFortified;
	bool              m_isFortifying;
	bool              m_hasCityWalls;
	bool              m_hasForceField;
	sint32            m_citySize;
	sint32            m_nextPop; //PFT 29 mar 05, show # turns until city next grows a pop

	bool              m_hiddenUnderStack;
	double            m_healthPercent;
	sint32            m_tempStackSize;

	sint32            m_spriteID;
	UnitSpriteGroup * m_unitSpriteGroup;
	LOADTYPE          m_loadType;
	bool              m_directionalAttack;

	sint32            m_facing;
	sint32            m_frame;
	uint16            m_transparency;
	uint32            m_shieldFlashOnTime;
	uint32            m_shieldFlashOffTime;

	sint32            m_refCount;

#ifdef _ACTOR_DRAW_OPTIMIZATION
	sint32            m_oldFacing;
	bool              m_oldIsFortified;
	bool              m_oldIsFortifying;
	bool              m_oldHasCityWalls;
	bool              m_oldHasForceField;
	bool              m_oldDrawShield;
	bool              m_oldDrawSelectionBrackets;
	uint16            m_oldFlags;
#endif
};

#endif
