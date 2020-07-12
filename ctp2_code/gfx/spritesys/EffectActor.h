#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __EFFECTACTOR_H__
#define __EFFECTACTOR_H__

#include "Actor.h"
#include "EffectSpriteGroup.h"

class SpriteState;
class SpriteGroup;
class aui_Surface;
class ActorPath;
class Action;

class EffectActor : public Actor
{
public:
	EffectActor(SpriteState * spriteState, const MapPoint & pos);
	virtual ~EffectActor();

	void Process();
	void SetAction(Action * action);
	bool IsActionFinished() const;

	Anim * CreatePlayElseFlashAnim(EFFECTACTION & action) const;
	Anim * CreatePlayAnim() const;

	void Draw(const RECT & paintRect) const;
	void DrawDirectWithFlags(aui_Surface * surf, uint16 flags) const;

	const MapPoint & GetMapPos() const { return m_pos; }

	uint16 GetWidth() const;
	uint16 GetHeight() const;

protected:
	Anim * CreateAnim(EFFECTACTION action) const;

	void Draw(const POINT & pos) const;
	void DrawText(sint32 x, sint32 y, MBCHAR * EffectText) const;
	void GetBoundingRect(RECT * rect, const POINT & pos) const;

	MapPoint            m_pos;

	EffectSpriteGroup * m_effectSpriteGroup;
	sint32              m_facing;
	sint32              m_frame;
	uint16              m_transparency;

	Action            * m_curAction;
	EFFECTACTION        m_curEffectAction;
};

#endif
