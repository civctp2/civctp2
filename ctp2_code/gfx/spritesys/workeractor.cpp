











#include "c3.h"

#include "aui.h"
#include "pixelutils.h"
#include "tileutils.h"

#include "Unit.h"
#include "dynarr.h"

#include "SelItem.h"

#include "FacedSprite.h"
#include "UnitSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "tiledmap.h"
#include "Anim.h"
#include "workeractor.h"


#include "ActorPath.h"
#include "Action.h"
#include "director.h"
#include "maputils.h"

extern SpriteGroupList	*g_unitSpriteGroupList;
extern TiledMap			*g_tiledMap;
extern Director			*g_director;
extern SelectedItem		*g_selected_item;

WorkerActor::WorkerActor(sint32 index, const MapPoint &pos)
{
	Initialize(index, pos);
}

WorkerActor::WorkerActor(sint32 index, const MapPoint &pos, sint32 x, sint32 y)
{
	Initialize(index, pos);
	m_x = x;
	m_y = y;
}

WorkerActor::WorkerActor(WorkerActor *copy)
{
	*this = *copy;
	m_actionQueue.CopyQueue();
}


WorkerActor::~WorkerActor()
{
	
	if (m_unitSpriteGroup != NULL) m_unitSpriteGroup = NULL;
}

void WorkerActor::Initialize(sint32 index, const MapPoint &pos)
{
	GROUPTYPE		type;

	m_curAction = NULL;
	m_animPos = 0;

	type = GROUPTYPE_UNIT;

	Assert(g_unitSpriteGroupList);

	m_unitSpriteGroup = (UnitSpriteGroup *)g_unitSpriteGroupList->GetSprite(index, type, LOADTYPE_FULL,(GAME_ACTION)0);

	AddIdle();
	m_pos = pos;

	m_x = 0;
	m_y = 0;

	m_facing = 0;
	m_frame = 0;

	m_index = index;

	m_actionQueue.Allocate(k_MAX_ACTION_QUEUE_SIZE);
	
}

void WorkerActor::AddIdle(void)
{

	m_curAction = new Action(UNITACTION_IDLE, ACTIONEND_ANIMEND);
	m_curAction->SetAnim(GetAnim(UNITACTION_IDLE));
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

		POINT curPt;

		
		if (m_curAction->GetPath() != NULL) {
			
			curPt = m_curAction->GetPosition();


		}
		
		m_facing = m_curAction->GetFacing();
	}
}

void WorkerActor::GetNextAction(void)
{
	if (m_curAction) {
		delete m_curAction;
		m_curAction = NULL;
	}

	if (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(m_curAction);
		if (m_curAction) {
			m_curUnitAction = (UNITACTION)m_curAction->GetActionType();
		} else {
			Assert(FALSE);
		}
	} else {
		
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

	if (m_curAction) {
		if (m_curAction->GetAnim()->GetType() == ANIMTYPE_LOOPED) {
			m_curAction->SetFinished(TRUE);
		}
	}
}

Anim *WorkerActor::GetAnim(UNITACTION action)
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
	Anim	*anim = new Anim();
	*anim = *origAnim;
	anim->SetSpecialCopyDelete(ANIMXEROX_COPY);

	return anim;

}

void WorkerActor::Draw(void)
{


	Unit			selectedGood;

	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;
		


	m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+k_ACTOR_CENTER_OFFSET_X, m_y+k_ACTOR_CENTER_OFFSET_Y, m_facing, 
							1, m_transparency, color, flags, NULL, NULL);
}

void WorkerActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale)
{


	Unit			selectedGood;

	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;
		


	m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame, sint32(x+(k_ACTOR_CENTER_OFFSET_X*scale)), sint32(y+(k_ACTOR_CENTER_OFFSET_Y*scale)), m_facing, 
							scale, m_transparency, color, flags, NULL, NULL);
}

void WorkerActor::DrawText(sint32 x, sint32 y, MBCHAR *unitText)
{
	m_unitSpriteGroup->DrawText(x, y, unitText);
}

BOOL WorkerActor::IsAnimating(void)
{
	
	return FALSE;
}

uint16 WorkerActor::GetWidth(void)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite	*theSprite;

	theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
	if (theSprite != NULL) {
		return theSprite->GetWidth();
	} else {
		return 0;
	}
}

uint16 WorkerActor::GetHeight(void)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite	*theSprite;

	theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
	if (theSprite != NULL) {
		return theSprite->GetHeight();
	} else {
		return 0;
	}
}

