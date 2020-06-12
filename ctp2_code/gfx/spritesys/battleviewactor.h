//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Battle view actor handling
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
// - Removed GetIDAndTypeMethod
//
//----------------------------------------------------------------------------

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef __BATTLEVIEWACTOR_H__
#define __BATTLEVIEWACTOR_H__

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

class BattleViewActor;

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "Actor.h"            // Actor
#include "Anim.h"             // Anim
#include "ctp2_inttypes.h"    // sint32, uint32
#include "Unit.h"
#include "UnitSpriteGroup.h"  // UnitSpriteGroup
#include "Queue.h"            // Queue
#include "tileset.h"

class Action;
class aui_Surface;
class SpriteGroup;
class SpriteState;

// GROUPTYPE, UNITACTION

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class BattleViewActor : public Actor
{
public:
	// Make position methods public
	using Actor::GetX;
	using Actor::GetY;

	BattleViewActor(SpriteState * spriteState, const Unit & id, const MapPoint & pos, sint32 owner);
	virtual ~BattleViewActor();

	Unit       GetUnitID() const { return m_unitID; }

	void       Process();
	void       AddAction(Action * action);
	UNITACTION GetCurUnitAction() const { return m_curUnitAction; }

	Anim     * CreateAnim(UNITACTION action);
	bool       HasAnim(UNITACTION action) const {
		return m_unitSpriteGroup && m_unitSpriteGroup->GetAnim((GAME_ACTION) action);
	}
	bool       HasDeath() const { return m_unitSpriteGroup->HasDeath(); }

	void       DrawDirect(aui_Surface * surf, sint32 x, sint32 y);

	uint16     GetWidth() const;
	uint16     GetHeight() const;
	void       GetBoundingRect(RECT * rect) const;

	double     GetHitPoints() const { return m_hitPoints; }
	void       SetHitPoints(double hitPoints) { m_hitPoints = hitPoints; }
	void       SetHitPointsMax(double hitPointsMax) { m_hitPointsMax = hitPointsMax; }
	void       SetFacing(sint32 facing) { m_facing = facing; }
	void       SetFortified(bool fortified) { m_isFortified = fortified; }

protected:
	static bool IsRectCompletelyOnSurface(const RECT & rect, aui_Surface * surf);

	void GetNextAction();
	void AddIdle();
	void Interrupt();
	void DumpAllActions();

	void DrawHealthBar(aui_Surface * surf) const;
	RECT DetermineIconRect(MAPICON icon) const;

	Unit              m_unitID;
	sint32            m_playerNum;
	MapPoint          m_pos;

	UnitSpriteGroup * m_unitSpriteGroup;
	sint32            m_facing;
	sint32            m_frame;
	uint16            m_transparency;

	Action          * m_curAction;
	UNITACTION        m_curUnitAction;

	Queue<Action *>   m_actionQueue;
	uint32            m_spriteID;

	double            m_hitPoints;
	double            m_hitPointsMax;

	bool              m_isFortified;
};

#endif
