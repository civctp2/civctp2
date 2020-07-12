//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Effect actor
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
// - Removed some unused local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "EffectActor.h"

#include "Anim.h"
#include "tiledmap.h"
#include "SpriteGroupList.h"
#include "SpriteState.h"
#include "maputils.h"

extern SpriteGroupList * g_effectSpriteGroupList;
extern TiledMap        * g_tiledMap;

EffectActor::EffectActor(SpriteState * spriteState, const MapPoint & pos)
:
	Actor               (spriteState),
	m_pos               (pos),
	m_effectSpriteGroup (NULL),
	m_facing            (0),
	m_frame             (0),
	m_transparency      (NO_TRANSPARENCY),
	m_curAction         (NULL),
	m_curEffectAction   (EFFECTACTION_NONE)
{
	if (spriteState && g_effectSpriteGroupList)
	{
		m_effectSpriteGroup = (EffectSpriteGroup *) g_effectSpriteGroupList->GetSprite(
				spriteState->GetIndex(), GROUPTYPE_EFFECT, LOADTYPE_FULL, (GAME_ACTION) 0);
	}
}

EffectActor::~EffectActor()
{
	if (g_effectSpriteGroupList && m_spriteState) {
		g_effectSpriteGroupList->ReleaseSprite(m_spriteState->GetIndex(), LOADTYPE_FULL);
	}

	delete m_spriteState;
}

bool EffectActor::IsActionFinished() const
{
	return !m_curAction;
}

void EffectActor::Process(void)
{
	if (!m_curAction) {
		return;
	}

	if (m_curAction) {
		m_curAction->Process();

		if (m_curAction->IsFinished())
		{
			delete m_curAction;
			m_curAction = NULL;
		}
	}

	if (m_curAction)
	{
		m_facing = m_curAction->CalculateFacing(m_facing);
		m_frame = m_curAction->GetSpriteFrame();
		m_transparency = m_curAction->GetTransparency();
	}
}

void EffectActor::SetAction(Action * action)
{
	Assert(m_effectSpriteGroup && action);
	if (!m_effectSpriteGroup || !action) {
		return;
	}

	m_curAction = action;
	m_curEffectAction = (EFFECTACTION) action->GetActionType();
}

Anim * EffectActor::CreatePlayElseFlashAnim(EFFECTACTION & action) const
{
	action = EFFECTACTION_PLAY;
	Anim * animation = CreateAnim(action);
	if (!animation) {
		action = EFFECTACTION_FLASH;
		animation = CreateAnim(action);
	}
	return animation;
}

Anim * EffectActor::CreatePlayAnim() const
{
	return CreateAnim(EFFECTACTION_PLAY);
}

Anim * EffectActor::CreateAnim(EFFECTACTION action) const
{
	Assert(m_effectSpriteGroup);
	if (!m_effectSpriteGroup) {
		return NULL;
	}

	Anim * animation = m_effectSpriteGroup->GetAnim((GAME_ACTION) action);
	return animation ? Anim::CreateSequential(*animation) : NULL;
}

void EffectActor::Draw(const RECT & paintRect) const
{
	if (m_curAction)
	{
		sint32 tileX;
		maputils_MapX2TileX(m_pos.x, m_pos.y, &tileX);
		if (maputils_TilePointInTileRect(tileX, m_pos.y, paintRect))
		{
			POINT current = m_curAction->CalculatePixelXY(m_pos);
			Draw(current);

			RECT rect;
			GetBoundingRect(&rect, current);
			g_tiledMap->AddDirtyRectToMix(rect);
		}
	}
}

void EffectActor::Draw(const POINT & pos) const
{
	uint16  flags   = k_DRAWFLAGS_NORMAL;
	Pixel16 color   = 0x0000;
	sint32  xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32  yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	m_effectSpriteGroup->Draw(m_curEffectAction, m_frame, pos.x+xoffset, pos.y+yoffset,
			pos.x+xoffset, pos.y+yoffset, m_facing, g_tiledMap->GetScale(), m_transparency, color, flags);
}

void EffectActor::DrawDirectWithFlags(aui_Surface * surf, uint16 flags) const
{
	Pixel16 color = 0;
	sint32  xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32  yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
	}
	m_effectSpriteGroup->DrawDirect(surf, m_curEffectAction, m_frame, m_x + xoffset, m_y + yoffset,
			m_x + xoffset, m_y + yoffset, m_facing, g_tiledMap->GetScale(), m_transparency, color, flags);
}

void EffectActor::DrawText(sint32 x, sint32 y, MBCHAR * effectText) const
{
	m_effectSpriteGroup->DrawText(x, y, effectText);
}

uint16 EffectActor::GetWidth() const
{
	Assert(m_effectSpriteGroup);
	if (!m_effectSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_effectSpriteGroup->GetGroupSprite((GAME_ACTION)m_curEffectAction);
	if (!sprite && (m_curEffectAction != EFFECTACTION_PLAY)) {
		sprite = m_effectSpriteGroup->GetGroupSprite((GAME_ACTION)EFFECTACTION_PLAY);
	}

	return sprite ? sprite->GetWidth() : 0;
}

uint16 EffectActor::GetHeight() const
{
	Assert(m_effectSpriteGroup);
	if (!m_effectSpriteGroup) {
		return 0;
	}

	Sprite * sprite = m_effectSpriteGroup->GetGroupSprite((GAME_ACTION)m_curEffectAction);
	if (!sprite && (m_curEffectAction != EFFECTACTION_PLAY)) {
		sprite = m_effectSpriteGroup->GetGroupSprite((GAME_ACTION)EFFECTACTION_PLAY);
	}

	return sprite ? sprite->GetHeight() : 0;
}

void EffectActor::GetBoundingRect(RECT * rect, const POINT & pos) const
{
	Assert(rect);
	if (!rect) {
		return;
	}

	POINT  hotPoint = m_effectSpriteGroup->GetHotPoint(m_curEffectAction, m_facing);
	double scale = g_tiledMap->GetScale();
	sint32 xoffset = (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
	sint32 yoffset = (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left   = 0;
	rect->top    = 0;
	rect->right  = (sint32)((double)GetWidth() * scale);
	rect->bottom = (sint32)((double)GetHeight() * scale);

	OffsetRect(rect, pos.x + xoffset, pos.y + yoffset);
}
