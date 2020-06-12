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

#include "c3.h"
#include "battleviewactor.h"

#include "SpriteGroupList.h"
#include "SpriteState.h"
#include "soundmanager.h"       // g_soundManager
#include "tiledmap.h"           // g_tiledMap
#include "aui_surface.h"
#include "primitives.h"

extern SpriteGroupList * g_unitSpriteGroupList;

BattleViewActor::BattleViewActor
(
	SpriteState    * spriteState,
	const Unit     & id,
	const MapPoint & pos,
	sint32           owner
)
:
	Actor             (spriteState),
	m_unitID          (id),
	m_playerNum       (owner),
	m_pos             (pos),
	m_unitSpriteGroup (NULL),
	m_facing          (k_DEFAULTSPRITEFACING),
	m_frame           (0),
	m_transparency    (NO_TRANSPARENCY),
	m_curAction       (NULL),
	m_curUnitAction   (UNITACTION_NONE),
	m_actionQueue     (k_MAX_ACTION_QUEUE_SIZE),
	m_spriteID        (0),
	m_hitPoints       (0.0),
	m_hitPointsMax    (0.0),
	m_isFortified     (false)
{
	if (spriteState) {
		m_spriteID = static_cast<uint32>(spriteState->GetIndex());
	}

	if (g_unitSpriteGroupList) {
		m_unitSpriteGroup = (UnitSpriteGroup *)
				g_unitSpriteGroupList->GetSprite(m_spriteID, GROUPTYPE_UNIT, LOADTYPE_FULL, (GAME_ACTION) 0);
	}

	AddIdle();
}

BattleViewActor::~BattleViewActor()
{
	DPRINTF(k_DBG_GAMESTATE, ("Deleting actor at %lx, unit=%lx\n", this, m_unitID.m_id));

	DumpAllActions();

	if (m_unitSpriteGroup && g_unitSpriteGroupList) {
		g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_FULL);
	}

	delete m_curAction;
}

void BattleViewActor::AddIdle()
{
	Anim * animation = CreateAnim(UNITACTION_IDLE);

	if (!animation)
	{
		animation = CreateAnim(UNITACTION_MOVE);
		Assert(animation);
	}

	delete m_curAction;
	m_curAction = Action::CreateUnitAction(UNITACTION_IDLE, animation);
	m_curUnitAction = UNITACTION_IDLE;
}

void BattleViewActor::Process()
{
	if (!m_curAction) {
		GetNextAction();
	}

	if (m_curAction)
	{
		m_curAction->Process();

		if (m_curAction->IsFinished()) {
			GetNextAction();
		}
	}

	if (m_curAction)
	{
		m_frame        = m_curAction->GetSpriteFrame();
		m_transparency = m_curAction->GetTransparency();
	}
}

void BattleViewActor::DumpAllActions()
{
	if (m_curAction)
	{
		delete m_curAction;
		m_curAction = NULL;
	}

	while (m_actionQueue.GetNumItems() > 0)
	{
		Action * otherAction;
		m_actionQueue.Dequeue(otherAction);
		delete otherAction;
	}
}

void BattleViewActor::GetNextAction()
{
	delete m_curAction;
	m_curAction = NULL;

	if (m_actionQueue.GetNumItems() > 0)
	{
		m_actionQueue.Dequeue(m_curAction);

		if (m_curAction) {
			m_curUnitAction = (UNITACTION)m_curAction->GetActionType();
		} else {
			Assert(false);
		}
	} else {
		AddIdle();
	}
}

void BattleViewActor::AddAction(Action *action)
{
	Assert(m_unitSpriteGroup && action);
	if (!m_unitSpriteGroup || !action) {
		return;
	}

	if (m_unitID.IsValid()) {
		m_playerNum = m_unitID.GetOwner();
	}

	Interrupt();

	m_actionQueue.Enqueue(action);
}

void BattleViewActor::Interrupt()
{
	if (m_curAction && m_curAction->GetActionType() == UNITACTION_IDLE) {
		delete m_curAction;
		m_curAction = NULL;
	}
}

Anim * BattleViewActor::CreateAnim(UNITACTION action)
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return NULL;
	}

	Anim * animation = m_unitSpriteGroup->GetAnim((GAME_ACTION)action);
	if (!animation && (action == UNITACTION_IDLE))
	{
		animation = m_unitSpriteGroup->GetAnim((GAME_ACTION) UNITACTION_MOVE);
		Assert(animation);
	}

	return animation ? Anim::CreateSequential(*animation) : NULL;
}

void BattleViewActor::DrawDirect(aui_Surface * surf, sint32 x, sint32 y)
{
	uint16  flags           = k_DRAWFLAGS_NORMAL;
	Pixel16 color           = COLOR_BLACK;

	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
	}

	if (m_isFortified) {
		Pixel16 * fortifiedImage = g_tiledMap->GetTileSet()->GetImprovementData(34);
		g_tiledMap->DrawOverlay(surf, fortifiedImage, x + k_ACTOR_CENTER_OFFSET_X-48,  y+k_ACTOR_CENTER_OFFSET_Y-48);
	}

	m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame, x+k_ACTOR_CENTER_OFFSET_X, y+k_ACTOR_CENTER_OFFSET_Y,
			m_facing, 1, m_transparency, color, flags);

	DrawHealthBar(surf);
}

void BattleViewActor::DrawHealthBar(aui_Surface * surf) const
{
	if (!surf) {
		return;
	}

	static const MAPICON icon = MAPICON_HERALD;

	RECT iconRect = DetermineIconRect(icon);
	if (!IsRectCompletelyOnSurface(iconRect, surf)) {
		return;
	}

	g_tiledMap->DrawColorizedOverlay(g_tiledMap->GetTileSet()->GetMapIconData(icon), surf, iconRect.left, iconRect.top,
			g_colorSet->GetPlayerColor(m_playerNum));

	iconRect.bottom = iconRect.top;
	iconRect.top    = iconRect.bottom - 3;
	if (!IsRectCompletelyOnSurface(iconRect, surf)) {
		return;
	}

	// Draw background
	Pixel16 black = g_colorSet->GetColor(COLOR_BLACK);
	primitives_FrameRect16(surf, &iconRect, black);

	InflateRect(&iconRect, -1, -1);

	double healthPercentage = std::max<double>(m_hitPoints / m_hitPointsMax, 0.0);
	sint32 healthBarLength = iconRect.right - iconRect.left;
	if (healthPercentage < 1.0)
	{
		healthBarLength = (sint32) (healthPercentage * healthBarLength);
		RECT blackRect = iconRect;
		blackRect.left += healthBarLength;
		primitives_PaintRect16(surf, &blackRect, black);
	}

	RECT colorRect  = iconRect;
	colorRect.right = colorRect.left + healthBarLength;
	primitives_PaintRect16(surf, &colorRect, GetHealthBarColor(healthPercentage));
}

uint16 BattleViewActor::GetWidth() const
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) m_curUnitAction);
	if (!sprite && (m_curUnitAction == UNITACTION_IDLE)) {
		sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) UNITACTION_MOVE);
	}
	return sprite ? sprite->GetWidth() : 0;
}

uint16 BattleViewActor::GetHeight() const
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) m_curUnitAction);
	if (!sprite && (m_curUnitAction == UNITACTION_IDLE)) {
		sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) UNITACTION_MOVE);
	}
	return sprite ? sprite->GetHeight() : 0;
}

void BattleViewActor::GetBoundingRect(RECT * rect) const
{
	Assert(rect);
	if (!rect) {
		return;
	}

	POINT  hotPoint = m_unitSpriteGroup->GetHotPoint(m_curUnitAction, m_facing);
	double scale    = g_tiledMap->GetScale();

	sint32 xoffset = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
	sint32 yoffset = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left   = 0;
	rect->top    = 0;
	rect->right  = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, m_x + xoffset, m_y + yoffset);

	rect->bottom += 36;
}

RECT BattleViewActor::DetermineIconRect(MAPICON icon) const
{
	POINT iconDim = g_tiledMap->GetTileSet()->GetMapIconDimensions(icon);
	RECT iconRect = {0, 0, iconDim.x, iconDim.y};

	UNITACTION unitAction = m_curUnitAction;
	if (!m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction)) {
		unitAction = UNITACTION_IDLE;
	}

	if (unitAction == UNITACTION_IDLE && !m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) UNITACTION_IDLE))
	{
		POINT * shieldPoint = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
		OffsetRect(&iconRect, m_x + shieldPoint->x, m_y + shieldPoint->y);
	}
	else
	{
		if (m_unitSpriteGroup && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) unitAction))
		{
			POINT * shieldPoint = m_unitSpriteGroup->GetShieldPoints(unitAction);
			OffsetRect(&iconRect, m_x + shieldPoint->x, m_y + shieldPoint->y);
		}
		else
		{
			sint32 top = m_y;
			sint32 middle = m_x + (sint32)((k_TILE_PIXEL_WIDTH) * g_tiledMap->GetScale()) / 2;
			OffsetRect(&iconRect, middle - iconDim.x / 2, top - iconDim.y);
		}
	}
	return iconRect;
}

bool BattleViewActor::IsRectCompletelyOnSurface(const RECT & rect, aui_Surface * surf)
{
	return rect.left >= 0 && rect.right < surf->Width()
	       && rect.top >= 0 && rect.bottom < surf->Height();
}
