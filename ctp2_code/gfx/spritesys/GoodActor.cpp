//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Good actor
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
// _ACTOR_DRAW_OPTIMIZATION
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - None
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "GoodActor.h"

#include "SpriteGroupList.h"
#include "Anim.h"
#include "tiledmap.h"               // g_tiledMap
#include "maputils.h"

extern SpriteGroupList * g_goodSpriteGroupList;

GoodActor::GoodActor(sint32 index, const MapPoint & pos)
:
	Actor             (),
	m_facing          (0),
	m_frame           (0),
	m_transparency    (NO_TRANSPARENCY),
	m_index           (index),
	m_pos             (pos),
	m_goodSpriteGroup (NULL),
	m_curAction       (NULL),
	m_curGoodAction   (GOODACTION_IDLE),
	m_actionQueue     (k_MAX_ACTION_QUEUE_SIZE),
	m_loadType        (LOADTYPE_BASIC),
	m_nextIdleAction  (0)
{
	Assert(g_goodSpriteGroupList);
	if (g_goodSpriteGroupList)
	{
		m_goodSpriteGroup = (GoodSpriteGroup *)
			g_goodSpriteGroupList->GetSprite(index, GROUPTYPE_GOOD, LOADTYPE_BASIC, (GAME_ACTION) 0);
	}

	AddIdle();
}

GoodActor::GoodActor(const GoodActor & a_Original)
:
	Actor             (a_Original),
	m_facing          (a_Original.m_facing),
	m_frame           (a_Original.m_frame),
	m_transparency    (a_Original.m_transparency),
	m_index           (a_Original.m_index),
	m_pos             (a_Original.m_pos),
	m_goodSpriteGroup (NULL),
	m_curAction       (NULL),
	m_curGoodAction   (a_Original.m_curGoodAction),
	m_actionQueue     (k_MAX_ACTION_QUEUE_SIZE),
	m_loadType        (LOADTYPE_BASIC),
	m_nextIdleAction  (a_Original.m_nextIdleAction)
{
	Assert(g_goodSpriteGroupList);

	if (a_Original.m_curAction) {
		m_curAction = new Action(*a_Original.m_curAction);
	}

	for (size_t i = 0; i < a_Original.m_actionQueue.GetNumItems(); ++i)
	{
		Action * data = NULL;
		if (a_Original.m_actionQueue.GetQueueItem(i, data)) {
			m_actionQueue.Enqueue(data ? new Action(*data) : NULL);
		}
	}

	if (g_goodSpriteGroupList)
	{
		m_loadType = a_Original.m_loadType;
		m_goodSpriteGroup = (GoodSpriteGroup *)
			g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, m_loadType, (GAME_ACTION) 0);
	}
}

GoodActor & GoodActor::operator = (const GoodActor & a_Original)
{
	if (this != &a_Original)
	{
		Assert(g_goodSpriteGroupList);

		Actor::operator  = (a_Original);
		m_facing         = a_Original.m_facing;
		m_frame          = a_Original.m_frame;
		m_transparency   = a_Original.m_transparency;
		m_index          = a_Original.m_index;
		m_pos            = a_Original.m_pos;
		m_nextIdleAction = a_Original.m_nextIdleAction;

		if (g_goodSpriteGroupList)
		{
			if (m_goodSpriteGroup) {
				g_goodSpriteGroupList->ReleaseSprite(m_index, m_loadType);
			}

			m_loadType = a_Original.m_loadType;
			m_goodSpriteGroup = (GoodSpriteGroup *)
				g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, m_loadType, (GAME_ACTION) 0);
		}
		else
		{
			m_goodSpriteGroup = NULL;
			m_loadType        = LOADTYPE_BASIC;
		}

		delete m_curAction;
		m_curAction     = a_Original.m_curAction ? new Action(*a_Original.m_curAction) : NULL;
		m_curGoodAction = a_Original.m_curGoodAction;

		while (m_actionQueue.GetNumItems() > 0)
		{
			Action * action = NULL;
			m_actionQueue.Dequeue(action);
			delete action;
		}

		for (size_t i = 0; i < a_Original.m_actionQueue.GetNumItems(); ++i)
		{
			Action * data = NULL;
			if (a_Original.m_actionQueue.GetQueueItem(i, data)) {
				m_actionQueue.Enqueue(data ? new Action(*data) : NULL);
			}
		}
	}

	return *this;
}

GoodActor::GoodActor(CivArchive &archive)
:
	Actor             (),
	m_facing          (0),
	m_frame           (0),
	m_transparency    (NO_TRANSPARENCY),
	m_index           (0),
	m_pos             (),
	m_goodSpriteGroup (NULL),
	m_curAction       (NULL),
	m_curGoodAction   (GOODACTION_IDLE),
	m_actionQueue     (k_MAX_ACTION_QUEUE_SIZE),
	m_loadType        (LOADTYPE_BASIC),
	m_nextIdleAction  (0)
{
	Serialize(archive);

	Assert(g_goodSpriteGroupList);
	if (g_goodSpriteGroupList) {
		m_goodSpriteGroup = (GoodSpriteGroup *)
			g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, LOADTYPE_BASIC, (GAME_ACTION) 0);
	}

	AddIdle();
}

GoodActor::~GoodActor()
{
	delete m_curAction;

	while (m_actionQueue.GetNumItems() > 0)
	{
		Action * action = NULL;
		m_actionQueue.Dequeue(action);
		delete action;
	}

	if (g_goodSpriteGroupList && m_goodSpriteGroup) {
		g_goodSpriteGroupList->ReleaseSprite(m_index, m_loadType);
	}
}

void GoodActor::Reload(LOADTYPE loadType)
{
	if (m_loadType != loadType) {
		if (loadType == LOADTYPE_FULL) {
			FullLoad();
		} else {
			DumpFullLoad();
		}
	}
}

void GoodActor::FullLoad()
{ // Doesn't seem to do much
	if (!m_goodSpriteGroup) {
		return;
	}
	if (m_loadType == LOADTYPE_FULL) {
		return;
	}

	SpriteGroup *group = g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, LOADTYPE_FULL,(GAME_ACTION)0);
	Assert(group == m_goodSpriteGroup);

	m_loadType = LOADTYPE_FULL;
	m_frame    = 0;
}

void GoodActor::DumpFullLoad()
{
	if (!m_goodSpriteGroup) {
		return;
	}
	if (m_loadType != LOADTYPE_FULL) {
		return;
	}

	bool purged = g_goodSpriteGroupList->ReleaseSprite(m_index, LOADTYPE_FULL);

	if (purged) {
		m_goodSpriteGroup = NULL;
	} else {
		m_loadType = LOADTYPE_BASIC;
	}

	m_frame = 0;
}

void GoodActor::PositionActor(const MapPoint & pos)
{
	sint32 pixelX, pixelY;
	maputils_MapXY2PixelXY(pos.x, pos.y, &pixelX, &pixelY);
	Actor::SetPos(pixelX, pixelY);
	m_pos = pos;
}

void GoodActor::AddIdle()
{
	if (m_curAction) {
		return;
	}

	if (GetTickCount() > m_nextIdleAction) {
		Anim * animation = CreateAnim(GOODACTION_IDLE);
		m_curAction = Action::CreateGoodAction(GOODACTION_IDLE, animation);
		m_curGoodAction = GOODACTION_IDLE;
	}
}

void GoodActor::Process()
{
	if (m_curAction)
    {
		m_curAction->Process();

		if (m_curAction->IsFinished())
		{
			if (m_curAction->GetActionType() == GOODACTION_IDLE) {
				m_nextIdleAction = GetTickCount() + 8000 + rand() % 5000;
			}
			GetNextAction();
		}
	} else {
		GetNextAction();
	}

	if (m_curAction)
	{
		POINT current = m_curAction->CalculatePixelXY(m_pos);
		Actor::SetPos(current.x, current.y);

		m_frame        = m_curAction->GetSpriteFrame();
		m_transparency = m_curAction->GetTransparency();
		m_facing       = m_curAction->CalculateFacing(m_facing);
	}
}

void GoodActor::GetNextAction()
{
	delete m_curAction;
	m_curAction = NULL;

	if (m_actionQueue.GetNumItems() > 0)
	{
		m_actionQueue.Dequeue(m_curAction);
		if (m_curAction) {
			m_curGoodAction = (GOODACTION)m_curAction->GetActionType();
		} else {
			Assert(false);
		}
	} else {
		AddIdle();
	}
}

void GoodActor::AddAction(Action * action)
{
	Assert(m_goodSpriteGroup && action);
	if (!m_goodSpriteGroup || !action) {
		return;
	}

	m_actionQueue.Enqueue(action);
}

Anim * GoodActor::CreateAnim(GOODACTION action) const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) {
		return NULL;
	}

	Anim * animation = m_goodSpriteGroup->GetAnim((GAME_ACTION)action);
	if (!animation)
	{
		animation = m_goodSpriteGroup->GetAnim((GAME_ACTION)GOODACTION_IDLE);
	}

	return animation ? Anim::CreateSequential(*animation) : NULL;
}

void GoodActor::DrawSelectionBrackets() const
{
	TileSet * tileSet = g_tiledMap->GetTileSet();

	RECT rect;
	SetRect(&rect, 0, 0, 1, 1);

	OffsetRect(&rect,
			m_x + (sint32)(k_TILE_PIXEL_WIDTH * g_tiledMap->GetScale())/2,
			m_y + (sint32)(k_TILE_GRID_HEIGHT * g_tiledMap->GetScale())/2);

	InflateRect(&rect, 25, 25);

	g_tiledMap->AddDirtyRectToMix(rect);

	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_BRACKET1);

	rect.right  -= (iconDim.x+1);
	rect.bottom -= (iconDim.y+1);

	Pixel16 * topLeft = tileSet->GetMapIconData(MAPICON_BRACKET1);
	Assert(topLeft);
	if (!topLeft) {
		return;
	}
	Pixel16 * topRight = tileSet->GetMapIconData(MAPICON_BRACKET2);
	Assert(topRight);
	if (!topRight) {
		return;
	}
	Pixel16 * bottomRight = tileSet->GetMapIconData(MAPICON_BRACKET3);
	Assert(bottomRight);
	if (!bottomRight) {
		return;
	}
	Pixel16 * bottomLeft = tileSet->GetMapIconData(MAPICON_BRACKET4);
	Assert(bottomLeft);
	if (!bottomLeft) {
		return;
	}

	g_tiledMap->DrawColorizedOverlayIntoMix(topLeft, rect.left, rect.top);
	g_tiledMap->DrawColorizedOverlayIntoMix(topRight, rect.right, rect.top);
	g_tiledMap->DrawColorizedOverlayIntoMix(bottomRight, rect.right, rect.bottom);
	g_tiledMap->DrawColorizedOverlayIntoMix(bottomLeft, rect.left, rect.bottom);
}

bool GoodActor::Draw(bool fogged) const
{
	uint16  flags = k_DRAWFLAGS_NORMAL;
	Pixel16 color = 0x0000;

	sint32  xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32  yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

#ifdef _ACTOR_DRAW_OPTIMIZATION
	if ((m_frame == m_oldFrame) && (m_x + xoffset == m_oldOffsetX) && (m_y + yoffset == m_oldOffsetY)) {
		if (m_paintTwice < 2) {
			m_paintTwice++;
		}
		return false;
	}

	m_paintTwice = 0;

	m_oldFrame   = m_frame;
	m_oldOffsetX = m_x + xoffset;
	m_oldOffsetY = m_y + yoffset;
#endif

	if (fogged) {
		flags |= k_BIT_DRAWFLAGS_FOGGED;
	}

	m_goodSpriteGroup->Draw(m_curGoodAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing,
							g_tiledMap->GetScale(), m_transparency, color, flags);

	if (g_selected_item->GetState() == SELECT_TYPE_GOOD)
	{
		if (m_pos == g_selected_item->GetCurSelectPos()) {
			DrawSelectionBrackets();
		}
	}

	return true;
}

void GoodActor::DrawDirect(aui_Surface * surf, sint32 x, sint32 y, double scale) const
{
	uint16  flags = k_DRAWFLAGS_NORMAL;
	Pixel16 color = 0x0000;

	sint32  xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * scale);
	sint32  yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * scale);

	m_goodSpriteGroup->DrawDirect(surf, GOODACTION_IDLE, m_frame, x+xoffset, y+yoffset, m_facing, scale,
							m_transparency, color, flags);
}

void GoodActor::DrawText(sint32 x, sint32 y, const MBCHAR * goodText) const
{
	m_goodSpriteGroup->DrawText(x, y, goodText);
}

POINT GoodActor::GetHotpoint() const
{
	POINT point = {0, 0};

	if (!m_goodSpriteGroup) {
		return point;
	}
	if (!m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)GOODACTION_IDLE)) {
		return point;
	}

	return m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)GOODACTION_IDLE)->GetHotPoint();
}

uint16 GoodActor::GetWidth() const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
	return sprite ? sprite->GetWidth() : 0;
}

uint16 GoodActor::GetHeight() const
{
	Assert(m_goodSpriteGroup);
	if (!m_goodSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)m_curGoodAction);
	return sprite ? sprite->GetHeight() : 0;
}

void GoodActor::GetBoundingRect(RECT * rect) const
{
	Assert(m_goodSpriteGroup && rect);
	if (!m_goodSpriteGroup || !rect) {
		return;
	}

	POINT  hotPoint = m_goodSpriteGroup->GetHotPoint(m_curGoodAction);
	double scale = g_tiledMap->GetScale();
	sint32 xoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
	sint32 yoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left   = 0;
	rect->top    = 0;
	rect->right  = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, m_x+xoff, m_y+yoff);
}

void GoodActor::Serialize(CivArchive & archive)
{
	CHECKSERIALIZE

	if (archive.IsStoring())
	{
		archive << m_index;
		m_pos.Serialize(archive);
	}
	else
		{
		archive >> m_index;
		m_pos.Serialize(archive);
	}
}
