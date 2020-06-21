//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Action on the screen
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
//----------------------------------------------------------------------------
///
/// \file   Action.h
/// \brief  Action on the screen

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef ACTION_H__
#define ACTION_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

enum GAME_ACTION
{
	ACTION_0=0,
	ACTION_1,
	ACTION_2,
	ACTION_3,
	ACTION_4,
	ACTION_5,
	ACTION_6,
	ACTION_7,
	ACTION_8,
	ACTION_9,
	ACTION_10,
	ACTION_11,
	ACTION_12,
	ACTION_13,
	ACTION_14,
	ACTION_15,
	ACTION_MAX
};

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"      // sintNN, uintNN
#include "MapPoint.h"           // MapPoint
#include "Sprite.h"
#include "SpriteGroup.h"

class ActorPath;
class Anim;
class UnitActor;
// POINT

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class Action
{
public:
	static Action * CreateEffectAction(sint32 actionType, Anim * anim) {
		return CreateEffectAction(actionType, anim, MapPoint(0,0), MapPoint(0,0));
	}
	static Action * CreateEffectAction(sint32 actionType, Anim * anim, const MapPoint & start, const MapPoint & end) {
		return new Action(GROUPTYPE_EFFECT, actionType, anim, start, end, k_MAX_FACINGS);
	}
	static Action * CreateGoodAction(sint32 actionType, Anim * anim) {
		return CreateGoodAction(actionType, anim, MapPoint(0,0), MapPoint(0,0));
	}
	static Action * CreateGoodAction(sint32 actionType, Anim * anim, const MapPoint & start, const MapPoint & end) {
		return new Action(GROUPTYPE_GOOD, actionType, anim, start, end, k_MAX_FACINGS);
	}
	static Action * CreateUnitAction(sint32 actionType, Anim * anim) {
		return CreateUnitAction(actionType, anim, MapPoint(0,0), MapPoint(0,0));
	}
	static Action * CreateUnitAction(
		sint32           actionType,
		Anim           * anim,
		const MapPoint & current,
		sint32           facing = k_MAX_FACINGS)
	{
		return CreateUnitAction(actionType, anim, current, current, facing);
	}
	static Action * CreateUnitAction(
		sint32           actionType,
		Anim           * anim,
		const MapPoint & start,
		const MapPoint & end,
		sint32           facing = k_MAX_FACINGS)
	{
		return new Action(GROUPTYPE_UNIT, actionType, anim, start, end, facing);
	}

	Action(Action const & a_Original);

	~Action();

	sint32 GetActionType() const { return (m_actionType > 0 ) ? m_actionType : 0; }

	void   Process();
	bool   IsFinished() const { return m_finished; }

	POINT  CalculatePixelXY(const MapPoint & current) const;
	sint32 CalculateFacing(sint32 facing) const;
	uint16 GetSpriteFrame() const;
	uint16 GetTransparency() const;

	// Only used to store unit's visibility in case of action-type is UNITACTION_ATTACK
	void   PutUnitsVisibility(uint32 unitsVisibility) { m_unitsVisibility = unitsVisibility; }
	uint32 GetUnitsVisibility(void) const { return m_unitsVisibility; }

protected:
	Action(
		GROUPTYPE        groupType,
		sint32           actionType,
		Anim           * animation,
		const MapPoint & start,
		const MapPoint & end,
		sint32           facing
	);

	sint32 DetermineMaxActionCounter();

	GROUPTYPE   m_groupType;
	sint32      m_actionType;
	bool        m_finished;

	Anim      * m_curAnim;
	ActorPath * m_curPath;

	sint32      m_maxActionCounter;
	sint32      m_curActionCounter;

	MapPoint    m_startMapPoint;
	MapPoint    m_endMapPoint;

	sint32      m_facing;
	uint32      m_unitsVisibility;
};

#endif
