//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Merged constructors and initialised all members.
//
//----------------------------------------------------------------------------
///
/// \file   Actor.cpp
/// \brief  Base actor class

#include "ctp/c3.h"
#include "gfx/spritesys/Actor.h"

#include "gfx/spritesys/SpriteState.h"

Actor::Actor(SpriteStatePtr ss) :
#if defined(_ACTOR_DRAW_OPTIMIZATION)
    m_paintTwice    (0),
    m_oldOffsetX    (-1),
    m_oldOffsetY    (-1),
    m_oldFrame      (-1),
#endif
    m_isactive      (false),
    m_x             (0),
    m_y             (0),
    m_spriteState   (ss),
    m_morphing      (false),
    m_animPos       (0)
{ }

Actor::Actor(const Actor &rhs) {
  *this = rhs;
}

Actor &Actor::operator=(const Actor &rhs) {
  if (this != &rhs) {
#if defined(_ACTOR_DRAW_OPTIMIZATION)
    _paintTwice = rhs._paintTwice;
    m_oldOffsetX = rhs.m_oldOffsetX;
    m_oldOffsetY = rhs.m_oldOffsetY;
    m_oldFrame = rhs.m_oldFrame;
#endif
    m_isactive = rhs.m_isactive;
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    m_spriteState.reset(rhs.m_spriteState ? new SpriteState(*rhs.m_spriteState) : NULL);
    m_morphing = rhs.m_morphing;
    m_animPos = rhs.m_animPos;
    m_actionQueue = rhs.m_actionQueue; // Performs deep copy
  }

  return *this;
}

std::size_t Actor::GetActionQueueNumItems() const {
  return m_actionQueue.Size();
}