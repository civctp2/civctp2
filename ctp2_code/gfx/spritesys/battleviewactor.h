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
#include "pixelutils.h"
#include "Queue.h"            // Queue
#include "Unit.h"
#include "UnitSpriteGroup.h"  // UnitSpriteGroup
#include "World.h"            // MapPoint

class Action;
class aui_Surface;
class SpriteGroup;
class SpriteState;

// BOOL, GROUPTYPE, UNITACTION

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class BattleViewActor : public Actor
{
public:
	BattleViewActor(SpriteState *ss, Unit id, sint32 type, const MapPoint &pos,
			  sint32 owner);

	~BattleViewActor();


	virtual void	Process(void);
	void			DumpAllActions(void);
	void			AddAction(Action *actionObj);
	void			GetNextAction(BOOL isVisible = TRUE);
	void			AddIdle(BOOL NoIdleJustDelay = FALSE);

	Anim *          CreateAnim(UNITACTION action);
	
	bool			HasThisAnim(UNITACTION action) const
    { 
        return m_unitSpriteGroup && m_unitSpriteGroup->GetAnim((GAME_ACTION) action); 
    };
	Anim			*MakeFakeDeath(void);

	
	void			Draw(BOOL fogged = FALSE);
	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y);

	void			DrawHealthBar(aui_Surface *surf);

	MapPoint		GetPos(void) const 
    { 
        return m_pos; 
    };
	void			SetPos(MapPoint pnt) { m_pos = pnt; }
    void            GetPixelPos(sint32 &x, sint32 &y) const
    {
        x = Actor::GetX(); 
        y = Actor::GetY(); 
    }; 
	void			SetPixelPos(sint32 x, sint32 y) 
    { 
        Actor::SetPos(x, y); 
    };

	sint32			GetFacing(void) const { return m_facing; }
	void			SetFacing(sint32 facing) { m_facing = facing; }

	uint16			GetWidth(void) const;
	uint16			GetHeight(void) const;
	Unit			GetUnitID(void) const
    { 
        return m_unitID; 
    };

	Action			*GetCurAction(void) const { return m_curAction; }

	Action			*LookAtNextAction(void) { return m_actionQueue.LookAtNextDeQueue(); }
	Action			*LookAtLastAction(void) { return m_actionQueue.LookAtLastDeQueue(); }
	uint32			GetActionQueueNumItems(void) const
    { 
        return m_actionQueue.GetNumItems(); 
    }

	bool			HasDeath(void) const 
    { 
        return m_unitSpriteGroup->HasDeath(); 
    };
	bool			HasDirectional(void) const
    { 
        return m_unitSpriteGroup->HasDirectional(); 
    };

	void			GetBoundingRect(RECT *rect) const;

	double			GetHitPoints(void) const { return m_hitPoints; }
	double			GetHitPointsMax(void) const{ return m_hitPointsMax; }
	void			SetHitPoints(double points) { m_hitPoints = points; }
	void			SetHitPointsMax(double points) { m_hitPointsMax = points; }


	void			SetFortified(bool fortified) { m_isFortified = fortified; }
	bool			GetFortified(void) const { return m_isFortified; }

protected:
	MapPoint			m_pos;
	Unit				m_unitID;
	sint32  			m_unitDBIndex;
	sint32				m_playerNum;

	UnitSpriteGroup		*m_unitSpriteGroup;
	sint32				m_facing;
	sint32				m_frame;
	uint16				m_transparency;

	Action				*m_curAction;
	UNITACTION			m_curUnitAction;

	Queue<Action *>		m_actionQueue;
	GROUPTYPE			m_type;			
	uint32				m_spriteID;		

	double				m_hitPoints;
	double				m_hitPointsMax;

	bool				m_isFortified;
};


#endif
