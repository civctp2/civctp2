#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __EFFECTACTOR_H__
#define __EFFECTACTOR_H__

#include "Actor.h"
#include "EffectSpriteGroup.h"
#include "pixelutils.h"
#include "Queue.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "tech_wllist.h"

class SpriteState;
class SpriteGroup;
class aui_Surface;
class ActorPath;
class Action;

typedef sint32 UnitDBIndex;

class EffectActor : public Actor
{
public:
	EffectActor(SpriteState *ss, const MapPoint &pos);
	virtual ~EffectActor();

	void ChangeType(SpriteState *ss, sint32 type, Unit id);

	virtual void	Process(void);
	void			EndTurnProcess(void);

	void			AddAction(Action *actionObj);
	void			GetNextAction(BOOL isVisible = TRUE);

	Anim *          CreateAnim(EFFECTACTION action);

	void			Draw(void);
	void			DrawDirect(aui_Surface *surf, sint32 x, sint32 y);
	void			DrawDirectWithFlags(aui_Surface *surf, sint32 x, sint32 y, uint16 flags);

	void			DrawText(sint32 x, sint32 y, MBCHAR *EffectText);

	BOOL			IsAnimating(void) const;

	MapPoint		GetPos(void) const { return m_pos; }
	void			SetPos(MapPoint pnt) { m_pos = pnt; }
	MapPoint		GetSavedPos(void) const { return m_savePos; }
	void			SetSavedPos(MapPoint pnt) { m_savePos = pnt; }
    void            GetPixelPos(sint32 &x, sint32 &y) const { x = m_x; y = m_y; }

	uint16			GetWidth(void) const;
	uint16			GetHeight(void) const;

	void			SetPlayerNum(sint32 playerNum) { m_playerNum = playerNum; }

	Action			*GetCurAction(void) const { return m_curAction; }
	Action			*LookAtNextAction(void) { return m_actionQueue.LookAtNextDeQueue(); }
	size_t			GetActionQueueNumItems(void) const { return m_actionQueue.GetNumItems(); }

	void			SetGenerateDeath(BOOL val) { m_generateDeath = val; }
	BOOL			GetGenerateDeath(void) const { return m_generateDeath; }

	void			SetDieAtTick(uint32 val) { m_dieAtTick = val; }
	uint32			GetDieAtTick(void) const { return m_dieAtTick; }

	void			SetEffectVisibility(uint32 val) { m_effectVisibility = val; }
	void			SetEffectVisibility(uint32 val, BOOL bval) { m_effectSaveVisibility = m_effectVisibility; m_effectVisibility = val; m_bVisSpecial = TRUE; }

	void			SetEffectVisibility() { m_bVisSpecial = FALSE; }
	uint32			GetEffectVisibility(void) const { return m_effectVisibility; }
	BOOL			GetVisSpecial(void) const { return m_bVisSpecial; }

	void			SetNeedsToDie(BOOL val) { m_needsToDie = val; }
	BOOL			GetNeedsToDie(void) const { return m_needsToDie; }

	void			SetKillNow(void) { m_killNow = TRUE; }
	BOOL			GetKillNow(void) const { return m_killNow; }

	void			GetBoundingRect(RECT *rect) const;
	EFFECTACTION	GetEffectAction(void) const { return m_curEffectAction; }

protected:
	MapPoint					m_pos;
	MapPoint					m_savePos;

	sint32						m_shX;
	sint32						m_shY;

	EffectSpriteGroup			*m_effectSpriteGroup;
	sint32						m_facing;
	sint32						m_lastMoveFacing;
	sint32						m_frame;
	uint16						m_transparency;

	Action						*m_curAction;
	EFFECTACTION				m_curEffectAction;

	Queue<Action *>				m_actionQueue;

	sint32						m_playerNum;
	uint32						m_effectVisibility;
	uint32						m_effectSaveVisibility;

	uint32						m_dieAtTick;

	BOOL						m_directionalAttack;
 	BOOL						m_needsToDie;
	BOOL						m_killNow;
	BOOL						m_generateDeath;

	BOOL						m_bVisSpecial;
};

#endif
