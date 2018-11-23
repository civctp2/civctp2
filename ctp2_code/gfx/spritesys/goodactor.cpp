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

#include "aui.h"
#include "pixelutils.h"
#include "tileutils.h"

#include "Unit.h"
#include "dynarr.h"

#include "SelItem.h"

#include "FacedSprite.h"
#include "GoodSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "tiledmap.h"
#include "Anim.h"
#include "GoodActor.h"

#include "ActorPath.h"
#include "Action.h"
#include "director.h"
#include "maputils.h"
#include "civarchive.h"

extern SpriteGroupList	*g_goodSpriteGroupList;
extern TiledMap			*g_tiledMap;
extern Director			*g_director;
extern SelectedItem		*g_selected_item;

GoodActor::GoodActor(sint32 index, const MapPoint &pos)
{
	Initialize(index, pos);
}

GoodActor::GoodActor(GoodActor *copy)
{
	*this = *copy;

	m_curAction = new Action(m_curAction);

	m_goodSpriteGroup = (GoodSpriteGroup *)g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, GetLoadType(),(GAME_ACTION)0);

	m_actionQueue.CopyQueue();
}

GoodActor::GoodActor(CivArchive &archive)
{
	Serialize(archive);
}

GoodActor::~GoodActor()
{
	DumpAllActions();

	g_goodSpriteGroupList->ReleaseSprite(m_index, GetLoadType());

	m_actionQueue.Deallocate();
}

void GoodActor::Initialize(sint32 index, const MapPoint &pos)
{
	GROUPTYPE		type;

	m_curAction = NULL;
	m_animPos = 0;

	type = GROUPTYPE_GOOD;

	m_loadType = LOADTYPE_BASIC;

	Assert(g_goodSpriteGroupList);

	m_goodSpriteGroup = (GoodSpriteGroup *)g_goodSpriteGroupList->GetSprite(index, type, GetLoadType(),(GAME_ACTION)0);

	AddIdle();

	m_pos = pos;

	m_x = 0;
	m_y = 0;

	m_facing = 0;
	m_frame = 0;

	m_index = index;

	m_actionQueue.Allocate(k_MAX_ACTION_QUEUE_SIZE);

}

void GoodActor::FullLoad(void)
{ // Doesn't seem to do much
	if (!m_goodSpriteGroup) return;
	if (m_loadType == LOADTYPE_FULL) return;

	SpriteGroup *group = g_goodSpriteGroupList->GetSprite(m_index, GROUPTYPE_GOOD, LOADTYPE_FULL,(GAME_ACTION)0);

	m_loadType = LOADTYPE_FULL;

	Assert(group == m_goodSpriteGroup);

	m_frame = 0;
}

void GoodActor::DumpFullLoad(void)
{
	if (!m_goodSpriteGroup) return;
	if (m_loadType != LOADTYPE_FULL) return;

	BOOL purged = g_goodSpriteGroupList->ReleaseSprite(m_index, LOADTYPE_FULL);

	if (purged) {
		m_goodSpriteGroup = NULL;
	} else {
		m_loadType = LOADTYPE_BASIC;
	}

	m_frame = 0;
}

void GoodActor::PositionActor(MapPoint &pos)
{
	sint32 pixelX, pixelY;

	maputils_MapXY2PixelXY(pos.x, pos.y, &pixelX, &pixelY);






	SetX(pixelX);
	SetY(pixelY);

	SetPos(pos);
}

void GoodActor::AddIdle(void)
{

	if (m_curAction) return;

	m_curAction = new Action(GOODACTION_IDLE, ACTIONEND_ANIMEND);

	Anim	*anim = GetAnim(GOODACTION_IDLE);
	if (!anim) return;

	m_curAction->SetAnim(anim);
	m_curAction->SetDelay(0);

	m_curGoodAction = GOODACTION_IDLE;
}

void GoodActor::Process(void)
{

	sint32		tickCount = GetTickCount();

	if (m_curAction) {
		m_curAction->Process();

		if (m_curAction->Finished()) {
			if (m_curAction->GetDelay() > 0 &&
				tickCount > m_curAction->GetDelay()) {
				MapPoint  end;
				m_curAction->GetEndMapPoint(end);
				if (end.x != 0 || end.y != 0) {
					m_pos = end;
				}

				GetNextAction();
			} else {
				if (m_curAction->GetDelay() == 0) {
					m_curAction->SetDelay(tickCount + 8000 + rand() % 5000);
				} else {
					return;
				}
			}
		}
	}

	if (m_curAction != NULL) {
		sint32 x, y;

		maputils_MapXY2PixelXY(m_pos.x, m_pos.y, &x, &y);
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

void GoodActor::GetNextAction(void)
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

void GoodActor::AddAction(Action *actionObj)
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

void GoodActor::DumpAllActions(void)
{
	if (m_curAction) {
		delete m_curAction;
		m_curAction = NULL;
	}

	Action *action;
	while (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(action);
		if (action) {
			delete action;
		}
	}

}

Anim *GoodActor::GetAnim(GOODACTION action)
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

	Anim	*anim = new Anim();
	*anim = *origAnim;
	anim->SetSpecialCopyDelete(ANIMXEROX_COPY);

	return anim;

}

void GoodActor::DrawSelectionBrackets(void)
{
	RECT		rect;

	TileSet		*tileSet = g_tiledMap->GetTileSet();

	SetRect(&rect, 0, 0, 1, 1);


 	OffsetRect(&rect,	m_x + (sint32)(k_TILE_PIXEL_WIDTH * g_tiledMap->GetScale())/2,
						m_y + (sint32)(k_TILE_GRID_HEIGHT * g_tiledMap->GetScale())/2);

	InflateRect(&rect, 25, 25);

	g_tiledMap->AddDirtyRectToMix(rect);

	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_BRACKET1);

	rect.right -= (iconDim.x+1);
	rect.bottom -= (iconDim.y+1);

	Pixel16			*topLeft, *topRight, *botLeft, *botRight;

	topLeft = tileSet->GetMapIconData(MAPICON_BRACKET1);
	Assert(topLeft); if (!topLeft) return;
	topRight = tileSet->GetMapIconData(MAPICON_BRACKET2);
	Assert(topRight); if (!topRight) return;
	botRight = tileSet->GetMapIconData(MAPICON_BRACKET3);
	Assert(botRight); if (!botRight) return;
	botLeft = tileSet->GetMapIconData(MAPICON_BRACKET4);
	Assert(botLeft); if (!botLeft) return;

	g_tiledMap->DrawColorizedOverlayIntoMix(topLeft, rect.left, rect.top);
	g_tiledMap->DrawColorizedOverlayIntoMix(topRight, rect.right, rect.top);
	g_tiledMap->DrawColorizedOverlayIntoMix(botRight, rect.right, rect.bottom);
	g_tiledMap->DrawColorizedOverlayIntoMix(botLeft, rect.left, rect.bottom);
}

BOOL GoodActor::Draw(BOOL fogged)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;
	sint32			xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32			yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());










#ifdef _ACTOR_DRAW_OPTIMIZATION

	if ( ( m_frame == m_oldFrame ) &&
		( m_x+xoffset == m_oldOffsetX ) && ( m_y+yoffset == m_oldOffsetY ) )
	{
		if ( m_paintTwice > 1 )
		{
			return ( FALSE );
		}
		m_paintTwice ++;
		return FALSE;
	}

	m_paintTwice = 0;

	m_oldFrame = m_frame;
	m_oldOffsetX = m_x + xoffset;
	m_oldOffsetY = m_y + yoffset;
#endif

	if (fogged)
		flags |= k_BIT_DRAWFLAGS_FOGGED;

	m_goodSpriteGroup->Draw(m_curGoodAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing,
							g_tiledMap->GetScale(), m_transparency, color, flags);

	if (g_selected_item->GetState() == SELECT_TYPE_GOOD) {
		if (m_pos == g_selected_item->GetCurSelectPos()) {
			DrawSelectionBrackets();
		}
	}

	return TRUE;
}

void GoodActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color = 0x0000;

	sint32			xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * scale);
	sint32			yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * scale);

	m_goodSpriteGroup->DrawDirect(surf, GOODACTION_IDLE, m_frame, x+xoffset, y+yoffset, m_facing, scale,
							m_transparency, color, flags);
}

void GoodActor::DrawText(sint32 x, sint32 y, MBCHAR *goodText)
{
	m_goodSpriteGroup->DrawText(x, y, goodText);
}

POINT GoodActor::GetHotpoint(void)
{
	POINT pt = {0,0};

	if (!m_goodSpriteGroup) return pt;
	if (!m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)GOODACTION_IDLE)) return pt;

	return m_goodSpriteGroup->GetGroupSprite((GAME_ACTION)GOODACTION_IDLE)->GetHotPoint();
}

BOOL GoodActor::IsAnimating(void)
{

	return FALSE;
}

uint16 GoodActor::GetWidth(void)
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

uint16 GoodActor::GetHeight(void)
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

void GoodActor::GetBoundingRect(RECT *rect)
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

void GoodActor::Serialize(CivArchive &archive)
{
    CHECKSERIALIZE

	if(archive.IsStoring()) {
		archive << m_index;
		m_pos.Serialize(archive);
	} else {
		archive >> m_index;
		m_pos.Serialize(archive);

		Initialize(m_index, m_pos);
	}
}
