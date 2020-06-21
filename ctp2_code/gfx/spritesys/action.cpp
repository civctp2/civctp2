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
#include "maputils.h"
#include "profileDB.h"

static const sint32 k_MAX_UNIT_MOVEMENT_ITERATIONS = 10;

Action::Action(
	GROUPTYPE        groupType,
	sint32           actionType,
	Anim           * animation,
	const MapPoint & start,
	const MapPoint & end,
	sint32           facing)
:
	m_groupType        (groupType),
	m_actionType       (actionType),
	m_curAnim          (animation),
	m_curPath          (NULL),
	m_maxActionCounter (0),
	m_curActionCounter (0),
	m_finished         (false),
	m_startMapPoint    (start),
	m_endMapPoint      (end),
	m_facing           (facing),
	m_unitsVisibility  (0)
{
	if (m_startMapPoint != m_endMapPoint) {
		m_curPath = new ActorPath(m_startMapPoint.x, m_startMapPoint.y, m_endMapPoint.x, m_endMapPoint.y);
	}
	m_maxActionCounter = DetermineMaxActionCounter();
}

Action::Action(const Action & a_Original)
:
	m_groupType        (a_Original.m_groupType),
	m_actionType       (a_Original.m_actionType),
	m_curAnim          (NULL),
	m_curPath          (NULL),
	m_maxActionCounter (a_Original.m_maxActionCounter),
	m_curActionCounter (a_Original.m_curActionCounter),
	m_finished         (a_Original.m_finished),
	m_startMapPoint    (a_Original.m_startMapPoint),
	m_endMapPoint      (a_Original.m_endMapPoint),
	m_facing           (a_Original.m_facing),
	m_unitsVisibility  (a_Original.m_unitsVisibility)
{
	if (a_Original.m_curAnim) {
		m_curAnim = new Anim(*a_Original.m_curAnim);
	}

	/// @todo Check copying of m_curPath (NULLed in original code)
}

Action::~Action()
{
	delete m_curPath;
	delete m_curAnim;
}

void Action::Process()
{
	if (!m_curAnim) {
		m_finished = true;
		return;
	}

	m_curAnim->Process();

	if (m_curActionCounter < m_maxActionCounter) {
		m_curActionCounter++;
	} else {
		m_finished = true;
	}
}

POINT Action::CalculatePixelXY(const MapPoint & current) const
{
	POINT pos;
	if (m_curPath) {
		m_curPath->CalcPosition(0, m_maxActionCounter, m_curActionCounter, &pos);
	} else {
		maputils_MapXY2PixelXY(current.x, current.y, pos);
	}
	return pos;
}

sint32 Action::CalculateFacing(sint32 facing) const
{
	if (m_curPath) {
		return m_curPath->CalcFacing(0, m_maxActionCounter, m_curActionCounter);
	} else if (m_facing != k_MAX_FACINGS) {
		return m_facing;
	} else {
		return facing;
	}
}

uint16 Action::GetSpriteFrame() const
{
	if (m_curAnim) {
		return m_curAnim->GetCurrentFrame();
	} else {
		return 0;
	}
}

uint16 Action::GetTransparency() const
{
	if (m_curAnim) {
		return m_curAnim->GetCurrentTransparency();
	} else {
		return 15;
	}
}

sint32 Action::DetermineMaxActionCounter()
{
	sint32 result = m_curAnim ? m_curAnim->GetNumFrames() : 0;

	// To ensure that movement always has the same speed it is independent of the number of frames.
	//   exception: effects with movement are processed in parallel so they can have their own speed.
	if (m_startMapPoint != m_endMapPoint && (m_groupType != GROUPTYPE_EFFECT))
	{
		result = k_MAX_UNIT_MOVEMENT_ITERATIONS;
		// unit moves can be sped up or skipped by settings
		if (m_groupType == GROUPTYPE_UNIT && m_actionType == UNITACTION_MOVE)
		{
			if (g_theProfileDB->IsUnitAnim()) {
				result = k_MAX_UNIT_MOVEMENT_ITERATIONS - g_theProfileDB->GetUnitSpeed();
			} else {
				result = 1;
			}
		}
	}

	return result;
}
