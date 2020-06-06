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
#include "TradeActor.h"

#include "dynarr.h"
#include "Anim.h"
#include "SpriteGroupList.h"
#include "ResourceRecord.h"
#include "maputils.h"
#include "tiledmap.h"

extern SpriteGroupList * g_goodSpriteGroupList;

TradeActor::TradeActor(TradeRoute newRoute)
:
	Actor             (),
	m_routeID         (newRoute),
	m_routeType       (ROUTE_TYPE_RESOURCE),
	m_routeResource   (0),
	m_routePath       (NULL),
	m_sourcePosID     (0),
	m_currentPosID    (0),
	m_destPosID       (0),
	m_currentPos      (MapPoint(0,0)),
	m_goodSpriteGroup (NULL),
	m_facing          (k_DEFAULTSPRITEFACING),
	m_frame           (0),
	m_transparency    (NO_TRANSPARENCY),
	m_idleAnim        (NULL),
	m_curAction       (NULL),
	m_curGoodAction   (GOODACTION_NONE)
{
	m_routeID.GetSourceResource(m_routeType, m_routeResource);

	m_routePath    = m_routeID.GetPath();
	m_sourcePosID  = 0;
	m_currentPosID = m_sourcePosID + 1;
	m_destPosID    = m_routePath->Num() - 1;
	m_currentPos   = m_routePath->Get(m_currentPosID);

	Assert(g_goodSpriteGroupList);

	sint32 index = g_theResourceDB->Get(m_routeResource)->GetSpriteID();
	m_goodSpriteGroup = (GoodSpriteGroup *)
			g_goodSpriteGroupList->GetSprite(index, GROUPTYPE_GOOD, LOADTYPE_FULL,(GAME_ACTION)0);
	if (!m_goodSpriteGroup) {
		m_goodSpriteGroup = (GoodSpriteGroup *)
			g_goodSpriteGroupList->GetSprite(index, GROUPTYPE_GOOD, LOADTYPE_BASIC, (GAME_ACTION) 0);
	}

	AddIdle();
}

TradeActor::~TradeActor()
{
	// m_routePath is reference and owned by m_routeID
	// m_idleAnim is reference and is owned by m_curAction
	delete m_curAction;
}

void TradeActor::AddIdle(void)
{
	// Copy of animation is made to ensure smooth animations within the movement
	//   animation is owned by action so it has to be (re-)created
	m_idleAnim = m_idleAnim ? Anim::CreateLooping(*m_idleAnim) : CreateAnim(GOODACTION_IDLE);
	delete m_curAction;

	m_currentPos = GetNextPos();
	m_curAction = Action::CreateGoodAction(GOODACTION_IDLE, m_idleAnim, m_currentPos, LookAtNextPos());
	m_curGoodAction = GOODACTION_IDLE;
}

MapPoint TradeActor::GetNextPos()
{
	if (++m_currentPosID >= m_destPosID) {
		m_currentPosID = m_sourcePosID;
	}

	return m_routePath->Get(m_currentPosID);
}

MapPoint TradeActor::LookAtNextPos() const
{
	if (m_currentPosID + 1 >= m_destPosID) {
		return m_routePath->Get(m_destPosID);
	} else {
		return m_routePath->Get(m_currentPosID + 1);
	}
}

void TradeActor::Process()
{
	if (m_curAction)
	{
		m_curAction->Process();

		if (m_curAction->IsFinished()) {
			AddIdle();
		}
	} else {
		AddIdle();
	}

	if (m_curAction)
	{
		POINT current = m_curAction->CalculatePixelXY(m_currentPos);
		m_x = current.x;
		m_y = current.y;

		m_frame = m_curAction->GetSpriteFrame();
		m_transparency = m_curAction->GetTransparency();
		m_facing = m_curAction->CalculateFacing(m_facing);
	}
}

Anim * TradeActor::CreateAnim(GOODACTION action) const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) {
		return NULL;
	}

	Anim * animation = m_goodSpriteGroup->GetAnim((GAME_ACTION)action);
	if (!animation)
	{
		animation = m_goodSpriteGroup->GetAnim((GAME_ACTION)GOODACTION_IDLE);
		Assert(animation);
	}

	return animation ? Anim::CreateLooping(*animation) : NULL;
}

void TradeActor::Draw(RECT * paintRect) const
{
	sint32 tileX;
	maputils_MapX2TileX(m_currentPos.x, m_currentPos.y, &tileX);

	if (maputils_TilePointInTileRect(tileX, m_currentPos.y, paintRect))
	{
		Draw(g_tiledMap->GetLocalVision());
		RECT dirtyRect;
		GetBoundingRect(&dirtyRect);
		g_tiledMap->AddDirtyRectToMix(dirtyRect);
	}
}

void TradeActor::Draw(const Vision * tileLocalVision) const
{
	uint16  flags = k_DRAWFLAGS_NORMAL;
	Pixel16 color = 0x0000;

	if (tileLocalVision->IsExplored(m_currentPos) && m_routeID.SeenBy(g_selected_item->GetVisiblePlayer()))
	{
		// POINT  hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
		// double scale  = g_tiledMap->GetScale();
		// sint32 xoffset = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
		// sint32 yoffset = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

		sint32 xoffset = k_ACTOR_CENTER_OFFSET_X;
		sint32 yoffset = k_ACTOR_CENTER_OFFSET_Y;

		m_goodSpriteGroup->Draw(m_curGoodAction, m_frame, m_x + xoffset, m_y + yoffset, m_facing,
								g_tiledMap->GetScale(), m_transparency, color, flags);
	}
}

void TradeActor::DrawText(sint32 x, sint32 y, MBCHAR * goodText) const
{
	m_goodSpriteGroup->DrawText(x, y, goodText);
}

uint16 TradeActor::GetWidth() const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
	if (sprite) {
		return sprite->GetWidth();
	} else {
		return 0;
	}
}

uint16 TradeActor::GetHeight() const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
	if (sprite) {
		return sprite->GetHeight();
	} else {
		return 0;
	}
}

void TradeActor::GetBoundingRect(RECT * rect) const
{
	Assert(rect);
	if (!rect) {
		return;
	}

	POINT  hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
	double scale    = g_tiledMap->GetScale();
	sint32 xoffset  = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
	sint32 yoffset  = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left   = 0;
	rect->top    = 0;
	rect->right  = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, m_x + xoffset, m_y + yoffset);
}
