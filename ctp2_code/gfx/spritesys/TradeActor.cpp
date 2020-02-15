//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Trade actor handling
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
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented memory leaks and double deletes.
// - The good sprite index is now retrieved from the resource database
//   instead from the good sprite state database. (Aug 29th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"

#include "Unit.h"

#include "dynarr.h"

#include "SelItem.h"
#include "pixelutils.h"
#include "tileutils.h"

#include "SelItem.h"            // g_selected_item
#include "FacedSprite.h"
#include "GoodSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "tiledmap.h"
#include "Anim.h"
#include "TradeActor.h"
#include "GoodActor.h"
#include "StrDB.h"

#include "ActorPath.h"
#include "Action.h"
#include "director.h"
#include "maputils.h"
#include "Cell.h"
#include "ResourceRecord.h"

extern SpriteGroupList	*g_goodSpriteGroupList;

TradeActor::TradeActor(TradeRoute newRoute)
{
	GROUPTYPE		type;

	m_curAction = NULL;
	m_animPos = 0;

	m_routeID = newRoute;
	m_routePath = m_routeID.GetPath();
	m_routeID.GetSourceResource(m_routeType, m_routeResource);

	m_sourcePosID = 0;
	m_currentPosID = m_sourcePosID+1;
	m_destPosID = m_routePath->Num()-1;

	type = GROUPTYPE_GOOD;

	Assert(g_goodSpriteGroupList);

	sint32 index = g_theResourceDB->Get(m_routeResource)->GetSpriteID();

	m_goodSpriteGroup = (GoodSpriteGroup *)g_goodSpriteGroupList->GetSprite(index, type, LOADTYPE_FULL,(GAME_ACTION)0);
	if (m_goodSpriteGroup == NULL)
	    m_goodSpriteGroup = (GoodSpriteGroup *)g_goodSpriteGroupList->GetSprite(index, type, LOADTYPE_BASIC,(GAME_ACTION)0);

	m_currentPos = m_sourcePos = m_routePath->Get(m_currentPosID);
	m_destPos = m_routePath->Get(m_destPosID);

	m_x = 0;
	m_y = 0;

	m_facing = 0;
	m_frame = 0;

	AddIdle();

	m_actionQueue.Allocate(k_MAX_ACTION_QUEUE_SIZE);
}

TradeActor::TradeActor(TradeActor *copy)
{
	*this = *copy;
	m_curAction	= new Action(m_curAction);
}

TradeActor::~TradeActor()
{
	delete m_curAction;
}

void TradeActor::AddIdle(void)
{
	delete m_curAction;
	m_curAction = new Action(GOODACTION_IDLE, ACTIONEND_INTERRUPT);
	m_curAction->SetAnim(CreateAnim(GOODACTION_IDLE));
	m_curGoodAction = GOODACTION_IDLE;
}

MapPoint TradeActor::GetNextPos(void)
{

	if(++m_currentPosID >= m_destPosID)
		m_currentPosID = m_sourcePosID;

	return m_routePath->Get(m_currentPosID);
}

MapPoint TradeActor::LookAtNextPos(void)
{

	if(m_currentPosID+1 >= m_destPosID)
		return m_routePath->Get(m_destPosID);
	else
		return m_routePath->Get(m_currentPosID+1);
}

void TradeActor::Process(void)
{

 	if (m_curAction)
	{
		m_curAction->Process();

		if (m_curAction->Finished())
		{
			MapPoint  end;
			m_curAction->GetEndMapPoint(end);
			if (end.x != 0 || end.y != 0)
			{
				m_currentPos = end;
			}

			GetNextAction();
		}
	}

	if (m_curAction != NULL) {
		sint32 x, y;





		if(m_curAction->GetCurrentEndCondition() == ACTIONEND_INTERRUPT && 	m_curAction->LoopAnimHasCycled())
		{
			m_curAction->ResetAnimLoop();
			m_curAction->SetCurActionCounter(0);
			m_currentPos = GetNextPos();






			MapPoint tempDestPos = LookAtNextPos();
			MapPoint tempCurPos = m_currentPos;

			m_curAction->CreatePath(m_currentPos.x, m_currentPos.y, tempDestPos.x, tempDestPos.y);

		}

		maputils_MapXY2PixelXY(m_currentPos.x, m_currentPos.y, &x, &y);

		m_x = x;
		m_y = y;

		m_frame = m_curAction->GetSpriteFrame();

		m_transparency = m_curAction->GetTransparency();

		POINT curPt;

		if (m_curAction->GetPath() != NULL) {

			curPt = m_curAction->GetPosition();





			m_x = curPt.x;
			m_y = curPt.y;
		}

		m_facing = m_curAction->GetFacing();
	}
}

void TradeActor::GetNextAction(void)
{
	if (m_curAction) {
		delete m_curAction;
		m_curAction = NULL;
	}

	if (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(m_curAction);
		if (m_curAction) {
			m_curGoodAction = (GOODACTION)m_curAction->GetActionType();
		} else {
			Assert(FALSE);
		}
	} else {

		AddIdle();
	}
}

void TradeActor::AddAction(Action *actionObj)
{
	Assert(m_goodSpriteGroup != NULL);
	if (m_goodSpriteGroup == NULL) return;

	Assert(actionObj != NULL);
	if (actionObj == NULL) return;


	m_actionQueue.Enqueue(actionObj);

	if (m_curAction) {
		if (m_curAction->GetAnim()->GetType() == ANIMTYPE_LOOPED) {
			m_curAction->SetFinished(TRUE);
		}
	}
}

Anim *TradeActor::CreateAnim(GOODACTION action)
{
	Assert(m_goodSpriteGroup != NULL);
	if (m_goodSpriteGroup == NULL) return NULL;

	Anim	*origAnim = m_goodSpriteGroup->GetAnim((GAME_ACTION)action);
	if (origAnim == NULL)
	{

		origAnim = m_goodSpriteGroup->GetAnim((GAME_ACTION)GOODACTION_IDLE);
		Assert(origAnim != NULL);
		return NULL;
	}

	return new Anim(*origAnim);
}

void TradeActor::Draw(const Vision *tileLocalVision)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;

	if(tileLocalVision->IsExplored(m_currentPos) && m_routeID.SeenBy(g_selected_item->GetVisiblePlayer()))
	{
		POINT	hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
		double	scale = g_tiledMap->GetScale();
		sint32	xoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale),
				yoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

		xoff = k_ACTOR_CENTER_OFFSET_X;
		yoff = k_ACTOR_CENTER_OFFSET_Y;

		m_goodSpriteGroup->Draw(m_curGoodAction, m_frame, m_x+xoff, m_y+yoff, m_facing,
									g_tiledMap->GetScale(), m_transparency, color, flags);
	}
}

void TradeActor::DrawText(sint32 x, sint32 y, MBCHAR *goodText)
{
	m_goodSpriteGroup->DrawText(x, y, goodText);
}

BOOL TradeActor::IsAnimating(void)
{

	return FALSE;
}

uint16 TradeActor::GetWidth(void)
{
	Assert(m_goodSpriteGroup != NULL);
	if (m_goodSpriteGroup == NULL) return 0;

	Sprite	*theSprite;

	theSprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
	if (theSprite != NULL) {
		return theSprite->GetWidth();
	} else {
		return 0;
	}
}

uint16 TradeActor::GetHeight(void)
{
	Assert(m_goodSpriteGroup != NULL);
	if (m_goodSpriteGroup == NULL) return 0;

	Sprite	*theSprite;

	theSprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
	if (theSprite != NULL) {
		return theSprite->GetHeight();
	} else {
		return 0;
	}
}

void TradeActor::GetBoundingRect(RECT *rect)
{
	Assert(rect != NULL);
	if (rect == NULL) return;

	POINT	hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
	double	scale = g_tiledMap->GetScale();
	sint32	xoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale),
			yoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left = 0;
	rect->top = 0;
	rect->right = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, m_x+xoff, m_y+yoff);
}
