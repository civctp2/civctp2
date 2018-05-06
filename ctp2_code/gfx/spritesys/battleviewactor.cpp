//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Battle view actor handling
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
// - Prevented NULL-dereferencing crash.
// - Removed unnecessary include files. (Aug 28th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "ctp/c3.h"
#include "gfx/spritesys/battleviewactor.h"

#include <algorithm>
#include "ui/aui_common/aui_surface.h"
#include "gfx/gfx_utils/colorset.h"
#include "gfx/spritesys/director.h"           // g_director
#include "ui/aui_utils/primitives.h"
#include "sound/soundmanager.h"       // g_soundManager
#include "gfx/spritesys/SpriteGroupList.h"
#include "gfx/spritesys/SpriteState.h"
#include "gfx/tilesys/tiledmap.h"           // g_tiledMap

extern SpriteGroupList *        g_unitSpriteGroupList;

BattleViewActor::BattleViewActor(
    SpriteStatePtr ss,
    Unit id,
    sint32 unitType,
    const MapPoint &pos,
    sint32 owner):
        Actor(ss),
        m_pos               (pos),
        m_unitID            (id),
        m_unitDBIndex       (unitType),
        m_playerNum         (owner),
        m_unitSpriteGroup   (NULL),
        m_facing            (k_DEFAULTSPRITEFACING),
        m_frame             (0),
        m_transparency      (15),
        m_curUnitAction     (UNITACTION_NONE),
        m_type              (GROUPTYPE_UNIT),
        m_spriteID          (0),
        m_hitPoints         (0.0),
        m_hitPointsMax      (0.0),
        m_isFortified       (false)
{
    if (ss)
    {
        m_spriteID = static_cast<uint32>(ss->GetIndex());
    }

    if (g_unitSpriteGroupList)
    {
	    m_unitSpriteGroup =
            (UnitSpriteGroup *) g_unitSpriteGroupList->GetSprite
                (m_spriteID, m_type, LOADTYPE_FULL, (GAME_ACTION) 0);
    }

	AddIdle();
}

BattleViewActor::~BattleViewActor()
{
	DPRINTF(k_DBG_GAMESTATE, ("Deleting actor at %lx, unit=%lx\n", this, m_unitID.m_id));

    DumpAllActions();

	if (m_unitSpriteGroup && g_unitSpriteGroupList)
    {
		g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_FULL);
	}
}

void BattleViewActor::AddIdle(BOOL NoIdleJustDelay)
{
	Anim * anim = CreateAnim(UNITACTION_IDLE);

	if (anim == NULL)
	{
		anim = CreateAnim(UNITACTION_MOVE);
		Assert(anim != NULL);
	}

	if (anim && (!m_actionQueue.Empty() || NoIdleJustDelay))
	{
		anim->SetNoIdleJustDelay(TRUE);
	}

	m_curAction.reset(new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT));
	m_curAction->SetAnim(anim);
	m_curUnitAction = UNITACTION_IDLE;

	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, GetUnitID());
}

void BattleViewActor::Process(void)
{
	if(!m_curAction)
		GetNextAction();

	while(m_curAction && m_curAction->GetActionType() == UNITACTION_NONE)
	{
		GetNextAction();
	}

	if (m_curAction)
	{
		if(!m_actionQueue.Empty())
			m_curAction->Process(LookAtNextAction());
		else
			m_curAction->Process();




		if (m_curAction->Finished())
		{
			GetNextAction();
		}
	}

    if (m_curAction)
    {
        m_frame        = m_curAction->GetSpriteFrame();
        m_transparency = m_curAction->GetTransparency();
    }
}

void BattleViewActor::DumpAllActions(void)
{

	if (m_curAction) {
		m_facing = m_curAction->GetFacing();
		m_curAction.reset();
	}

	ActionPtr deadAction;
	while (!m_actionQueue.Empty()) {
    deadAction = m_actionQueue.Back();
    m_actionQueue.Pop();
		if (deadAction) {
			m_facing = deadAction->GetFacing();
			deadAction.reset();
		}
	}
}

void BattleViewActor::GetNextAction(BOOL isVisible)
{
  m_curAction.reset();

	ActionPtr pendingAction = LookAtNextAction();

	while(!m_actionQueue.Empty() && pendingAction->GetActionType() == UNITACTION_NONE)
	{
    m_curAction = m_actionQueue.Back();
    m_actionQueue.Pop();
		m_curAction.reset();
		pendingAction = LookAtNextAction();
	}

	if (!m_actionQueue.Empty())
	{
    m_curAction = m_actionQueue.Back();
    m_actionQueue.Pop();

		if (m_curAction) {
			m_curUnitAction = (UNITACTION)m_curAction->GetActionType();
		} else {
			Assert(FALSE);
		}
	}
	else
	{
		AddIdle();
	}
}

void BattleViewActor::AddAction(ActionPtr actionObj)
{
	Assert(m_unitSpriteGroup && actionObj);
	if (!m_unitSpriteGroup || !actionObj) return;

	if (m_unitID.IsValid())
	{
		m_playerNum = m_unitID.GetOwner();
	}

	m_actionQueue.Push(actionObj);
}

Anim *BattleViewActor::CreateAnim(UNITACTION action)
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) return NULL;

	Anim *  origAnim = m_unitSpriteGroup->GetAnim((GAME_ACTION)action);

	if (!origAnim && (UNITACTION_IDLE == action))
	{
		origAnim = m_unitSpriteGroup->GetAnim((GAME_ACTION) UNITACTION_MOVE);;
		Assert(origAnim);
		action = UNITACTION_MOVE;
	}

	if (!origAnim)
		return NULL;

    Anim * anim = new Anim(*origAnim);

	if (action == UNITACTION_IDLE)
	{
		srand(anim->GetDelay() + g_director->GetMasterCurTime());
		anim->AdjustDelay(rand() % 2000);
	}

	return anim;
}

#define k_FAKE_DEATH_FRAMES			15
#define k_FAKE_DEATH_DURATION		1500

Anim * BattleViewActor::MakeFakeDeath(void)
{
    Anim *      anim        = new Anim();

    uint16 *    frames      = new uint16[k_FAKE_DEATH_FRAMES];
    std::fill(frames, frames + k_FAKE_DEATH_FRAMES, 0);
	anim->SetFrames(frames);
	anim->SetNumFrames(k_FAKE_DEATH_FRAMES);

    POINT *     moveDeltas  = new POINT[k_FAKE_DEATH_FRAMES];
    POINT       pt          = {0,0};
    std::fill(moveDeltas, moveDeltas + k_FAKE_DEATH_FRAMES, pt);
	anim->SetDeltas(moveDeltas);

    uint16 *    transparencies = new uint16[k_FAKE_DEATH_FRAMES];
    for (uint16 i = 0; i < k_FAKE_DEATH_FRAMES; ++i)
    {
        transparencies[i] = (uint16) (k_FAKE_DEATH_FRAMES - i);
    }
	anim->SetTransparencies(transparencies);

	anim->SetPlaybackTime(k_FAKE_DEATH_DURATION);
	anim->SetType(ANIMTYPE_SEQUENTIAL);

	return anim;
}

void BattleViewActor::Draw(BOOL fogged)
{
	uint16			flags           = k_DRAWFLAGS_NORMAL;
	Pixel16			color           = 0x0000;
	BOOL			directionAttack = FALSE;

	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
	}

	if (fogged)
		flags |= k_BIT_DRAWFLAGS_FOGGED;

	if (m_curAction == NULL)
	{

		m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+k_ACTOR_CENTER_OFFSET_X, m_y+k_ACTOR_CENTER_OFFSET_Y, m_facing,
			g_tiledMap->GetScale(), m_transparency, color, flags, FALSE, directionAttack);
	}
	else
	{
		m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+k_ACTOR_CENTER_OFFSET_X, m_y+k_ACTOR_CENTER_OFFSET_Y, m_facing,
			g_tiledMap->GetScale(), m_transparency, color, flags, m_curAction->SpecialDelayProcess(), directionAttack);
	}
}

void BattleViewActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y)
{
	uint16			flags           = k_DRAWFLAGS_NORMAL;
	Pixel16			color           = 0x0000;
	BOOL			directionAttack = FALSE;

	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
	}

	if (m_isFortified) {
		Pixel16			*fortifiedImage = g_tiledMap->GetTileSet()->GetImprovementData(34);
		g_tiledMap->DrawOverlay(surf, fortifiedImage, x + k_ACTOR_CENTER_OFFSET_X-48,  y+k_ACTOR_CENTER_OFFSET_Y-48);
	}

	m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame, x+k_ACTOR_CENTER_OFFSET_X, y+k_ACTOR_CENTER_OFFSET_Y, m_facing,
							1, m_transparency, color, flags, m_curUnitAction == UNITACTION_IDLE, directionAttack);

	DrawHealthBar(surf);
}

























void BattleViewActor::DrawHealthBar(aui_Surface *surf)
{
	if (!surf) return;

	RECT		leftRect, rightRect;
	sint32		top, middle;

	double		ratio = m_hitPoints / m_hitPointsMax;
	Pixel16		color = g_colorSet->GetPlayerColor(m_playerNum);
	MAPICON		icon = MAPICON_HERALD;
	TileSet		*tileSet = g_tiledMap->GetTileSet();
	sint32		surfWidth = surf->Width();
	sint32		surfHeight = surf->Height();

	POINT	iconDim = tileSet->GetMapIconDimensions(icon);

	RECT	iconRect = {0, 0, iconDim.x, iconDim.y};

	UNITACTION		unitAction = m_curUnitAction;
	if (m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction) == NULL)
		unitAction = UNITACTION_IDLE;

	POINT *shieldPoint;


	if (unitAction == UNITACTION_IDLE && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE) == NULL) {
		shieldPoint = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
		OffsetRect(&iconRect, m_x + shieldPoint->x, m_y + shieldPoint->y);
	} else {
		if (m_unitSpriteGroup && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)unitAction) != NULL) {
			shieldPoint = m_unitSpriteGroup->GetShieldPoints(unitAction);
			OffsetRect(&iconRect, m_x + shieldPoint->x, m_y + shieldPoint->y);
		} else {

			top = m_y;
			middle = m_x + (sint32)((k_TILE_PIXEL_WIDTH) * g_tiledMap->GetScale())/2;
			OffsetRect(&iconRect, middle - iconDim.x / 2, top - iconDim.y);
		}
	}

	if (iconRect.left < 0) return;
	if (iconRect.right >= surfWidth) return;
	if (iconRect.top < 0) return;
	if (iconRect.bottom >= surfHeight) return;

	g_tiledMap->DrawColorizedOverlay(tileSet->GetMapIconData(icon), surf, iconRect.left, iconRect.top, color);

	iconRect.bottom = iconRect.top;
	iconRect.top = iconRect.bottom - 4;

	if (iconRect.left < 0) return;
	if (iconRect.right >= surfWidth) return;
	if (iconRect.top < 0) return;
	if (iconRect.bottom >= surfHeight) return;

	primitives_FrameRect16(surf, &iconRect, g_colorSet->GetColor(COLOR_BLACK));

	InflateRect(&iconRect, -1, -1);

	leftRect = rightRect = iconRect;

	color = g_colorSet->GetColor(COLOR_GREEN);
	if (ratio < 1.0) {
		leftRect.right = leftRect.left + (sint32)(ratio * (double)(iconRect.right-iconRect.left));
		rightRect.left = leftRect.right;
		if (ratio >= 0.3 && ratio < 0.7) {
			color = g_colorSet->GetColor(COLOR_YELLOW);
		} else {
			if (ratio < 0.3)
				color = g_colorSet->GetColor(COLOR_RED);
		}
		primitives_PaintRect16(surf, &rightRect, g_colorSet->GetColor(COLOR_BLACK));
	}

	if(leftRect.left > leftRect.right)
	{
		RECT temprect = leftRect;
		leftRect.left = temprect.right;
		leftRect.right = temprect.left;
	}
	primitives_PaintRect16(surf, &leftRect, color);
}

uint16 BattleViewActor::GetWidth(void) const
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite * theSprite =
        m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);

	if (theSprite != NULL)
	{
		return theSprite->GetWidth();
	}
	else
	{
		if (m_curUnitAction == UNITACTION_IDLE)
		{
			theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_MOVE);
			if (theSprite != NULL)
				return theSprite->GetWidth();
		}

		return 0;
	}
}

uint16 BattleViewActor::GetHeight(void) const
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite * theSprite =
        m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) m_curUnitAction);

	if (theSprite != NULL)
	{
		return theSprite->GetHeight();
	}
	else
	{
		if (m_curUnitAction == UNITACTION_IDLE)
		{
			theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_MOVE);
			if (theSprite != NULL)
				return theSprite->GetHeight();
		}

		return 0;
	}
}

void BattleViewActor::GetBoundingRect(RECT *rect) const
{
	Assert(rect != NULL);
	if (rect == NULL) return;

	POINT	hotPoint = m_unitSpriteGroup->GetHotPoint(m_curUnitAction, m_facing);
	double	scale = g_tiledMap->GetScale();

	sint32	xoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale),
			yoff = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left = 0;
	rect->top = 0;
	rect->right = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, m_x+xoff, m_y+yoff);

rect->bottom += 36;

}
