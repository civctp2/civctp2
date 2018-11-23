//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ header
// Description  : Base actor class
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
// _MSC_VER
// - Compiler version (for the Microsoft C++ compiler only)
//
// _ACTOR_DRAW_OPTIMIZATION
//
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Animation queue increased to 12 fixing bug #4
// - Merged constructors and initialised all members.
//
//----------------------------------------------------------------------------

#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif

#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "Action.h"

#define k_ACTOR_CENTER_OFFSET_X		48
#define k_ACTOR_CENTER_OFFSET_Y		48

//fix bug #4 by kaan
#define k_MAX_ACTION_QUEUE_SIZE		12

class SpriteState;
class Anim;






class Actor
{
public:
	Actor(SpriteState * ss = NULL);
	virtual ~Actor(void);

	void	SetX(sint32 x) { m_x = x; }
	void	SetY(sint32 y) { m_y = y; }
	void	SetPos(sint32 x, sint32 y) { m_x = x; m_y = y; }
	void	SetPos(POINT p) { m_x = p.x; m_y = p.y; }
	void	SetMorphing(BOOL val) { m_morphing = val; }

	sint32	GetX(void) { return m_x; }
	sint32	GetY(void) { return m_y; }
	POINT	GetPos(void) { POINT p; p.x = m_x; p.y = m_y; return p; }
	BOOL	GetMorphing(void) { return m_morphing; }

	SpriteState *GetSpriteState(void) { return m_spriteState; }
	void SetSpriteState(SpriteState *ss) { m_spriteState = ss; }

	virtual void	SetAnim(Anim *a);
	virtual void	Process(void);

	inline bool	IsActive () { return m_isactive;};
	inline void	SetActive(bool active) { m_isactive=active;};
protected:
#ifdef _ACTOR_DRAW_OPTIMIZATION
	sint32			m_paintTwice;
	sint32			m_oldOffsetX;
	sint32			m_oldOffsetY;
	sint32			m_oldFrame;
#endif

	bool			m_isactive;

	sint32			m_x;
	sint32			m_y;
	SpriteState		*m_spriteState;

	BOOL			m_morphing;

	sint32			m_animPos;

	void			SetAnimation(GAME_ACTION action);
	void			ServeAnimation(uint32 timeslice_msecs);

	virtual bool    ActionMove(Action *actionObj){ return false;};

};

#endif
