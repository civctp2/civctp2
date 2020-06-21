#include "c3.h"
#include "workeractor.h"

#include "SpriteGroupList.h"
#include "Anim.h"

extern SpriteGroupList * g_unitSpriteGroupList;

WorkerActor::WorkerActor(sint32 index, const MapPoint & pos, sint32 x, sint32 y)
:
	Actor             (),
	m_facing          (0),
	m_frame           (0),
	m_transparency    (NO_TRANSPARENCY),
	m_index           (index),
	m_pos             (pos),
	m_unitSpriteGroup (NULL),
	m_curAction       (NULL),
	m_curUnitAction   (),
	m_actionQueue     (k_MAX_ACTION_QUEUE_SIZE)
{
	SetPos(x, y);

	Assert(g_unitSpriteGroupList);
	if (g_unitSpriteGroupList)
	{
		m_unitSpriteGroup = (UnitSpriteGroup *)
				g_unitSpriteGroupList->GetSprite(index, GROUPTYPE_UNIT, LOADTYPE_FULL,(GAME_ACTION)0);
	}

	AddIdle();
}

WorkerActor::~WorkerActor()
{
	delete m_curAction;

	while (m_actionQueue.GetNumItems() > 0) {
		Action * action;
		m_actionQueue.Dequeue(action);
		delete action;
	}
}

void WorkerActor::AddIdle()
{
	m_curAction = Action::CreateUnitAction(UNITACTION_IDLE, CreateAnim(UNITACTION_IDLE));
	m_curUnitAction = UNITACTION_IDLE;
}

void WorkerActor::Process()
{
	if (m_curAction)
	{
		m_curAction->Process();

		if (m_curAction->IsFinished() && m_curAction->GetActionType() != UNITACTION_IDLE) {
			GetNextAction();
		}
	} else {
		GetNextAction();
	}

	if (m_curAction) {
		m_frame = m_curAction->GetSpriteFrame();
		m_transparency = m_curAction->GetTransparency();
		m_facing = m_curAction->CalculateFacing(m_facing);
	}
}

void WorkerActor::GetNextAction()
{
	delete m_curAction;
	m_curAction = NULL;

	if (m_actionQueue.GetNumItems() > 0)
    {
		m_actionQueue.Dequeue(m_curAction);
		if (m_curAction) {
			m_curUnitAction = (UNITACTION) m_curAction->GetActionType();
		} else {
			Assert(false);
		}
	} else {
		AddIdle();
	}
}

void WorkerActor::AddAction(Action * action)
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return;
	}
	Assert(action);
	if (!action) {
		return;
	}

	Interrupt();

	m_actionQueue.Enqueue(action);
}

void WorkerActor::Interrupt()
{
	if (m_curAction && m_curAction->GetActionType() == UNITACTION_IDLE) {
		delete m_curAction;
		m_curAction = NULL;
	}
}

Anim * WorkerActor::CreateAnim(UNITACTION action) const
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return NULL;
	}

	Anim * animation = m_unitSpriteGroup->GetAnim((GAME_ACTION)action);
	if (!animation)
	{
		animation = m_unitSpriteGroup->GetAnim((GAME_ACTION)UNITACTION_IDLE);
		Assert(animation);
	}

	return animation ? Anim::CreateSequential(*animation) : NULL;
}

void WorkerActor::Draw() const
{
	uint16  flags = k_DRAWFLAGS_NORMAL;
	Pixel16 color = 0x0000;

	m_unitSpriteGroup->Draw(m_curUnitAction, m_frame,
			m_x + k_ACTOR_CENTER_OFFSET_X, m_y + k_ACTOR_CENTER_OFFSET_Y,
			m_facing,1, m_transparency, color, flags, false);
}

void WorkerActor::DrawDirect(aui_Surface * surf, sint32 x, sint32 y, double scale) const
{
	uint16  flags = k_DRAWFLAGS_NORMAL;
	Pixel16 color = 0x0000;

	m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame,
			sint32(x + (k_ACTOR_CENTER_OFFSET_X * scale)), sint32(y + (k_ACTOR_CENTER_OFFSET_Y * scale)),
			m_facing, scale, m_transparency, color, flags);
}

void WorkerActor::DrawText(sint32 x, sint32 y, const MBCHAR * unitText) const
{
	m_unitSpriteGroup->DrawText(x, y, unitText);
}

uint16 WorkerActor::GetWidth() const
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
	return sprite ? sprite->GetWidth() : 0;
}

uint16 WorkerActor::GetHeight() const
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
	return sprite ? sprite->GetHeight() : 0;
}
