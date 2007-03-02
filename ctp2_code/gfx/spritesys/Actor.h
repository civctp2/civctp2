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
///
/// \file   Actor.h
/// \brief  Base actor class

#if defined(HAVE_PRAGMA_ONCE)
#pragma once
#endif

#ifndef ACTOR_H__
#define ACTOR_H__

//----------------------------------------------------------------------------
//
// Library imports
//
//----------------------------------------------------------------------------

#include "windows.h"            // POINT

//----------------------------------------------------------------------------
//
// Exported names
//
//----------------------------------------------------------------------------

class Actor;

#define k_ACTOR_CENTER_OFFSET_X		48
#define k_ACTOR_CENTER_OFFSET_Y		48

// "fix" bug #4 by kaan
#define k_MAX_ACTION_QUEUE_SIZE		12

//----------------------------------------------------------------------------
//
// Project imports
//
//----------------------------------------------------------------------------

#include "ctp2_inttypes.h"      // sint32

class Action;
class Anim;
class SpriteState;

//----------------------------------------------------------------------------
//
// Declarations
//
//----------------------------------------------------------------------------

class Actor 
{
public:
	Actor(SpriteState * ss = NULL);

	void	SetX(sint32 x) { m_x = x; }
	void	SetY(sint32 y) { m_y = y; }
	void	SetPos(sint32 x, sint32 y) { m_x = x; m_y = y; }
	void	SetPos(POINT p) { m_x = p.x; m_y = p.y; }
	void	SetMorphing(bool val) { m_morphing = val; }

	sint32	GetX(void) const { return m_x; }
	sint32	GetY(void) const { return m_y; }
	POINT	GetPos(void) const { POINT p; p.x = m_x; p.y = m_y; return p; }
	bool	GetMorphing(void) const { return m_morphing; }

	SpriteState * GetSpriteState(void) const { return m_spriteState; }
	void SetSpriteState(SpriteState *ss) { m_spriteState = ss; }

	virtual void	SetAnim(Anim *a)  {};
	virtual void	Process(void)     {};

	
	bool	IsActive () const { return m_isactive; };
	void	SetActive(bool active) { m_isactive = active; };

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

	bool			m_morphing; 

	sint32			m_animPos;

	virtual bool    ActionMove(Action *actionObj) { return false; };
};

#endif
