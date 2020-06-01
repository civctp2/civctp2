//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
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
/// \file   Action.cpp
/// \brief  Action on the screen

#include "c3.h"
#include "Action.h"

#include "ActorPath.h"
#include "Anim.h"
#include "UnitActor.h"
#if 0
#include "Actor.h"
#include "debugmemory.h"
#include "director.h"       // g_director
#include "EffectActor.h"
#include "pixelutils.h"
#include "profileDB.h"      // g_theProfileDB
#include "spriteutils.h"
#include "tech_wllist.h"
#include "Unit.h"
#endif

#define STOMPCHECK()

Action::Action(sint32 actionType, ACTIONEND endCondition, sint32 startAnimPos, sint32 specialDelayProcess)
:
    m_actionType                (actionType),
    m_endCondition              (endCondition),
	m_curAnim                   (NULL),
	m_curPath                   (NULL),
	m_maxActionCounter          (0),
	m_curActionCounter          (0),
	m_animPos                   (startAnimPos),
	m_delay                     (0),
	m_finished                  (false),
	m_loopAnimFinished          (false),
	m_specialDelayProcess       (specialDelayProcess),
    m_startMapPoint             (),
	m_endMapPoint               (),
	m_facing                    (k_DEFAULTSPRITEFACING),
	m_sound_effect_id           (-1),
	m_unitsVisibility           (0),
	m_unitVisionRange           (0)
{ ; }

Action::Action(Action const & a_Original)
:
    m_actionType                (a_Original.m_actionType),
    m_endCondition              (a_Original.m_endCondition),
	m_curAnim                   (NULL),
	m_curPath                   (NULL),
	m_maxActionCounter          (a_Original.m_maxActionCounter),
	m_curActionCounter          (a_Original.m_curActionCounter),
	m_animPos                   (a_Original.m_animPos),
	m_delay                     (a_Original.m_delay),
	m_finished                  (a_Original.m_finished),
	m_loopAnimFinished          (a_Original.m_loopAnimFinished),
	m_specialDelayProcess       (a_Original.m_specialDelayProcess),
    m_startMapPoint             (a_Original.m_startMapPoint),
	m_endMapPoint               (a_Original.m_endMapPoint),
	m_facing                    (a_Original.m_facing),
	m_sound_effect_id           (a_Original.m_sound_effect_id),
	m_unitsVisibility           (a_Original.m_unitsVisibility),
	m_unitVisionRange           (a_Original.m_unitVisionRange)
{
    if (a_Original.m_curAnim)
    {
        m_curAnim = new Anim(*a_Original.m_curAnim);
    }

    /// @todo Check copying of m_curPath (NULLed in original code)
}

/// @todo Remove when no longer used
Action::Action(Action *copyme)
{
	*this = *copyme;

	if (copyme->GetAnim()) {
		m_curAnim = new Anim(*copyme->GetAnim());
	}

	m_curPath = NULL;
}

Action::~Action(void)
{
	delete m_curPath;
	delete m_curAnim;
}

void Action::Process(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (!m_curAnim)
    {
        SetFinished(true);
        return;
    }

    m_curAnim->Process();

	m_curActionCounter++;
	if (m_curActionCounter > m_maxActionCounter || m_curAnim->IsFinished())
	{
		m_curActionCounter = m_maxActionCounter;
		m_loopAnimFinished = true;

		if ( m_endCondition == ACTIONEND_ANIMEND ||
			m_endCondition == ACTIONEND_PATHEND )
		{
			SetFinished(true);
		}
	}

}

void Action::Process(Action *pendingAction)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if(pendingAction != this )
		Process();

	if (m_endCondition == ACTIONEND_INTERRUPT || Finished())
	{
		SetFinished(true);
	}

#ifndef _TEST
	STOMPCHECK();
#endif
}

void Action::SetAnim(Anim *anim)
{
#ifndef _TEST
	STOMPCHECK();
#endif

//	Assert(anim != NULL);
	if (anim == NULL) return;

	anim->Rewind();

    m_maxActionCounter = anim->GetNumFrames();

	m_curAnim = anim;

#ifndef _TEST
	STOMPCHECK();
#endif
}

void Action::CreatePath(sint32 x1, sint32 y1, sint32 x2, sint32 y2)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	delete m_curPath;
	m_curPath = new ActorPath(x1, y1, x2, y2);

	m_maxActionCounter = k_MAX_UNIT_MOVEMENT_ITERATIONS;

#ifndef _TEST
	STOMPCHECK();
#endif

}

POINT Action::GetPosition(void) const
{
	POINT pos = {0,0};

	if (m_curPath)
    {
    	m_curPath->CalcPosition(0, m_maxActionCounter, m_curActionCounter, &pos);
    }

	return pos;
}

uint16 Action::GetSpriteFrame(void) const
{
	uint16 frame;

	if (m_curAnim) {
		if (m_loopAnimFinished && m_curActionCounter == m_maxActionCounter) {
			frame = 0;
		} else {
			frame = m_curAnim->GetCurrentFrame();
		}

	} else {
		frame = 0;
	}

	return frame;
}

sint32 Action::GetFacing(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (m_curPath)
    {
		m_facing = m_curPath->CalcFacing(0, m_maxActionCounter, m_curActionCounter);
	}
    // else: No action: keep current facing

	return m_facing;
}

uint16 Action::GetTransparency(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif

	uint16 transparency = 15;

	if (m_curAnim) {
		transparency = m_curAnim->GetCurrentTransparency();
	}
	return transparency;
}
