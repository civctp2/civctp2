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
#include "profileDB.h"

extern SpriteGroupList * g_goodSpriteGroupList;

TradeActor::TradeActor(TradeRoute newRoute)
:
	Actor               (),
	m_routeID           (newRoute),
	m_spriteID          (0),
	m_routePath         (NULL),
	m_sourcePosID       (0),
	m_currentPosID      (0),
	m_destPosID         (0),
	m_currentPos        (MapPoint(0,0)),
	m_nextPos           (MapPoint(0, 0)),
	m_goodSpriteGroup   (NULL),
	m_facing            (k_DEFAULTSPRITEFACING),
	m_idleAnim          (NULL),
	m_curAction         (NULL),
	m_curGoodAction     (GOODACTION_NONE),
	m_loadType          (LOADTYPE_NONE),
	m_nextGoodAnimation (0)
{
	ROUTE_TYPE routeType     = ROUTE_TYPE_RESOURCE;
	sint32     routeResource = 0;
	m_routeID.GetSourceResource(routeType, routeResource);

	m_spriteID = g_theResourceDB->Get(routeResource)->GetSpriteID();

	m_routePath    = m_routeID.GetPath();
	m_sourcePosID  = 0;
	m_currentPosID = m_sourcePosID + 1;
	m_destPosID    = m_routePath->Num() - 1;
	m_currentPos   = m_routePath->Get(m_currentPosID);
	m_nextPos      = LookAtNextPos();

	AddIdle();
}

TradeActor::~TradeActor()
{
	// m_routePath is reference and owned by m_routeID
	// m_idleAnim is reference and is owned by m_curAction
	delete m_curAction;
	g_goodSpriteGroupList->ReleaseSprite(m_spriteID, m_loadType);
}

void TradeActor::AddIdle(void)
{
	LoadSprites();

	// Copy of animation is made to ensure smooth animations within the movement
	//   animation is owned by action so it has to be (re-)created
	if (!m_idleAnim || (m_idleAnim->IsFinished() && GetTickCount() > m_nextGoodAnimation)) {
		m_idleAnim = CreateAnim(GOODACTION_IDLE);
	}
	else
	{
		m_idleAnim = Anim::CreateSequential(*m_idleAnim);
		if (!m_idleAnim->IsFinished()) {
			m_nextGoodAnimation = GetTickCount() + 5000 + rand() % 3000;
		}
	}
	delete m_curAction;

	m_currentPos = GetNextPos();
	m_nextPos    = LookAtNextPos();
	m_curAction = Action::CreateGoodAction(GOODACTION_IDLE, m_idleAnim, m_currentPos, m_nextPos);
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

	if (m_curAction) {
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

	return animation ? Anim::CreateSequential(*animation) : NULL;
}

void TradeActor::Draw(const RECT & paintRect) const
{
	sint32 currentTileX;
	maputils_MapX2TileX(m_currentPos.x, m_currentPos.y, &currentTileX);
	sint32 nextTileX;
	maputils_MapX2TileX(m_nextPos.x, m_nextPos.y, &nextTileX);

	if (maputils_TilePointInTileRect(currentTileX, m_currentPos.y, paintRect)
		&& maputils_TilePointInTileRect(nextTileX, m_nextPos.y, paintRect))
	{
		POINT drawPos = m_curAction->CalculatePixelXY(m_currentPos);
		double scale = g_tiledMap->GetScale();
		drawPos.x += (sint32)(k_ACTOR_CENTER_OFFSET_X * scale);
		drawPos.y += (sint32)(k_ACTOR_CENTER_OFFSET_Y * scale);

		Draw(g_tiledMap->GetLocalVision(), drawPos);
		RECT dirtyRect;
		GetBoundingRect(&dirtyRect, drawPos);
		g_tiledMap->AddDirtyRectToMix(dirtyRect);
	}
}

void TradeActor::Draw(const Vision * tileLocalVision, const POINT & drawPos) const
{
	Pixel16 color = g_colorSet->GetColor(COLOR_BLACK);

	if (m_curAction
		&& tileLocalVision->IsExplored(m_currentPos) && m_routeID.SeenBy(g_selected_item->GetVisiblePlayer()))
	{
		m_goodSpriteGroup->Draw(m_curGoodAction, m_curAction->GetSpriteFrame(), drawPos.x, drawPos.y,
			m_facing, g_tiledMap->GetScale(), m_curAction->GetTransparency(), color, k_DRAWFLAGS_NORMAL);
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

void TradeActor::GetBoundingRect(RECT * rect, const POINT & drawPos) const
{
	Assert(rect);
	if (!rect) {
		return;
	}

	double scale = g_tiledMap->GetScale();

	rect->left   = 0;
	rect->top    = 0;
	rect->right  = (sint32)(GetWidth() * scale);
	rect->bottom = (sint32)(GetHeight() * scale);

	POINT hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
	OffsetRect(rect, drawPos.x - (sint32)(hotPoint.x * scale), drawPos.y - (sint32)(hotPoint.y * scale));
}

void TradeActor::LoadSprites()
{
	LOADTYPE loadType = g_theProfileDB->IsGoodAnim() ? LOADTYPE_FULL : LOADTYPE_BASIC;
	if (loadType != m_loadType)
	{
		Assert(g_goodSpriteGroupList);

		if (m_loadType != LOADTYPE_NONE) {
			g_goodSpriteGroupList->ReleaseSprite(m_spriteID, m_loadType);
		}

		m_loadType = loadType;
		m_goodSpriteGroup = (GoodSpriteGroup *)
				g_goodSpriteGroupList->GetSprite(m_spriteID, GROUPTYPE_GOOD, m_loadType, (GAME_ACTION) 0);
		if (!m_goodSpriteGroup && loadType == LOADTYPE_FULL)
		{
			m_loadType = LOADTYPE_BASIC;
			m_goodSpriteGroup = (GoodSpriteGroup *)
					g_goodSpriteGroupList->GetSprite(m_spriteID, GROUPTYPE_GOOD, m_loadType, (GAME_ACTION) 0);
		}
	}
}
