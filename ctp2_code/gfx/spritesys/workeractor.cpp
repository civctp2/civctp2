#include "c3.h"
#include "workeractor.h"

#include "aui.h"
#include "pixelutils.h"
#include "SelItem.h"                // g_selected_item
#include "tileutils.h"
#include "Unit.h"

#include "FacedSprite.h"
#include "UnitSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "tiledmap.h"               // g_tiledMap
#include "Anim.h"

#include "ActorPath.h"
#include "Action.h"
#include "director.h"               // g_director
#include "maputils.h"

extern SpriteGroupList	*g_unitSpriteGroupList;

namespace
{
    /// Default transparency value
    /// @todo Move to better location
    uint16 const        TRANSPARENCY_DEFAULT    = 15;
}

WorkerActor::WorkerActor(sint32 index, const MapPoint &pos, sint32 x, sint32 y)
:
    Actor               (),
	m_facing            (0),
	m_frame             (0),
	m_transparency      (TRANSPARENCY_DEFAULT),
	m_index             (index),
    m_pos               (pos),
    m_unitSpriteGroup   (NULL),
    m_curAction         (NULL),
    m_curUnitAction     (),
    m_actionQueue       (k_MAX_ACTION_QUEUE_SIZE)
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
}

void WorkerActor::AddIdle(void)
{
	m_curAction = new Action(UNITACTION_IDLE, ACTIONEND_ANIMEND);
	m_curAction->SetAnim(CreateAnim(UNITACTION_IDLE));
	m_curUnitAction = UNITACTION_IDLE;
}

void WorkerActor::Process(void)
{

	if (m_curAction) {
		m_curAction->Process();

		if (m_curAction->Finished()) {
			MapPoint  end;
			m_curAction->GetEndMapPoint(end);
			if (end.x != 0 || end.y != 0) {
				m_pos = end;
			}

			GetNextAction();
		}
	}

	if (m_curAction != NULL) {








		m_frame = m_curAction->GetSpriteFrame();

		m_transparency = m_curAction->GetTransparency();


		if (m_curAction->GetPath())
        {
		    (void) m_curAction->GetPosition();
		}

		m_facing = m_curAction->GetFacing();
	}
}

void WorkerActor::GetNextAction(void)
{
	delete m_curAction;
	m_curAction = NULL;

	if (m_actionQueue.GetNumItems() > 0)
    {
		m_actionQueue.Dequeue(m_curAction);
		if (m_curAction)
        {
			m_curUnitAction = (UNITACTION) m_curAction->GetActionType();
		}
        else
        {
			Assert(FALSE);
		}
	}
    else
    {
		AddIdle();
	}
}

void WorkerActor::AddAction(Action *actionObj)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return;

	Assert(actionObj != NULL);
	if (actionObj == NULL) return;


	m_actionQueue.Enqueue(actionObj);

	if (m_curAction)
    {
        if (!m_curAction->GetAnim() ||
       	    (m_curAction->GetAnim()->GetType() == ANIMTYPE_LOOPED)
           )
        {
		    m_curAction->SetFinished(TRUE);
        }
    }
}

Anim *WorkerActor::CreateAnim(UNITACTION action)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return NULL;

	Anim	*origAnim = m_unitSpriteGroup->GetAnim((GAME_ACTION)action);
	if (origAnim == NULL)
	{

		origAnim = m_unitSpriteGroup->GetAnim((GAME_ACTION)UNITACTION_IDLE);
		Assert(origAnim != NULL);
		return NULL;
	}

	return new Anim(*origAnim);
}

void WorkerActor::Draw(void)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;

	m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+k_ACTOR_CENTER_OFFSET_X, m_y+k_ACTOR_CENTER_OFFSET_Y, m_facing,
							1, m_transparency, color, flags, NULL, NULL);
}

void WorkerActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;

	m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame, sint32(x+(k_ACTOR_CENTER_OFFSET_X*scale)), sint32(y+(k_ACTOR_CENTER_OFFSET_Y*scale)), m_facing,
							scale, m_transparency, color, flags, NULL, NULL);
}

void WorkerActor::DrawText(sint32 x, sint32 y, MBCHAR const * unitText)
{
	m_unitSpriteGroup->DrawText(x, y, unitText);
}

bool WorkerActor::IsAnimating(void) const
{
	return false;
}

uint16 WorkerActor::GetWidth(void) const
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite *    theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
    return (theSprite) ? theSprite->GetWidth() : 0;
}

uint16 WorkerActor::GetHeight(void) const
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite *    theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
    return (theSprite) ? theSprite->GetHeight() : 0;
}
