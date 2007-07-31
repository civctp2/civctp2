











#include "c3.h"
#include "SpriteState.h"
#include "Anim.h"
#include "Unit.h"
#include "Actor.h"
#include "SoundManager.h"



extern SoundManager		*g_soundManager;


Actor::Actor(SpriteState *ss)
{
	m_spriteState = ss;
	m_morphing = FALSE;
	m_isactive = false;

#ifdef _ACTOR_DRAW_OPTIMIZATION
	m_paintTwice = 0;
	m_oldFrame = -1;
	m_oldOffsetX = m_oldOffsetY = -1;
#endif
}

Actor::Actor()
{
	m_isactive = false;
}

Actor::~Actor(void)
{
}

void Actor::SetAnim(Anim *a)
{
}

void Actor::Process(void)
{	
}










