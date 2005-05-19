//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Battle view actor handling
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented NULL-dereferencing crash.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_surface.h"

#include "TileInfo.h"

#include "Unit.h"	
#include "UnitData.h"
#include "screenmanager.h"

#include "FacedSprite.h"
#include "UnitSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "tiledmap.h"
#include "tileset.h"
#include "Anim.h"
#include "battleviewactor.h"
#include "ActorPath.h"
#include "Action.h"

#include "director.h"
#include "UnitPool.h"
#include "SpriteStateDB.h"
#include "primitives.h"

#include "soundmanager.h"
#include "colorset.h"

extern SpriteGroupList	*g_unitSpriteGroupList;
extern TiledMap			*g_tiledMap;
extern Director			*g_director;
extern UnitPool         *g_theUnitPool;
extern World			*g_theWorld;
extern ScreenManager	*g_screenManager;
extern SpriteStateDB	*g_theCitySpriteStateDB;
extern SoundManager		*g_soundManager;
extern ColorSet			*g_colorSet;

BattleViewActor::BattleViewActor(SpriteState *ss, Unit id, sint32 unitType, const MapPoint &pos, sint32 owner)
: Actor(ss)
{	
	uint32 spriteID;

	GetIDAndType(owner, ss, id, unitType, (MapPoint)pos, &spriteID, &m_type);
	m_spriteID = (sint32)spriteID;

	m_spriteState = ss;
	m_unitDBIndex = unitType;
	m_playerNum = owner;
	m_unitID = id;
	m_pos = pos;

	Initialize();
	
	m_facing = k_DEFAULTSPRITEFACING;
}

void BattleViewActor::Initialize(void)
{
	
	m_curAction = NULL;
	m_animPos = 0;

	m_unitSpriteGroup = (UnitSpriteGroup *)g_unitSpriteGroupList->GetSprite((uint32)m_spriteID, m_type, LOADTYPE_FULL,(GAME_ACTION)0);

	m_x = 0;
	m_y = 0;

	m_frame = 0;

	m_transparency = 15;

	m_hitPoints = 0;
	m_hitPointsMax = 0;

	m_isFortified = FALSE;

	m_actionQueue.Allocate(k_MAX_ACTION_QUEUE_SIZE);

	AddIdle();
}

void BattleViewActor::GetIDAndType(sint32 owner, SpriteState *ss, Unit id, sint32 unitType, MapPoint &pos, 
								uint32 *spriteID, GROUPTYPE *groupType)
{
	*spriteID = ss->GetIndex();
	*groupType = GROUPTYPE_UNIT;
}

BattleViewActor::~BattleViewActor()
{
	DPRINTF(k_DBG_GAMESTATE, ("Deleting actor at %lx, unit=%lx\n", this, m_unitID.m_id));

	
	DumpAllActions();

	
	if (m_unitSpriteGroup != NULL) {
		g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_FULL);
	}

	m_actionQueue.Deallocate();

	


	
	if (m_curAction) delete m_curAction;
}


void BattleViewActor::AddIdle(BOOL NoIdleJustDelay)
{
	Anim		*anim;

	anim = GetAnim(UNITACTION_IDLE);

	
	if (anim == NULL) 
	{
		anim = GetAnim(UNITACTION_MOVE);
		Assert(anim != NULL);
	}

	if (anim && ((GetActionQueueNumItems() > 0) || NoIdleJustDelay))
	{
		anim->SetNoIdleJustDelay(TRUE);
	}

	delete m_curAction;

	m_curAction = new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT);
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
		if(GetActionQueueNumItems() > 0)
			m_curAction->Process(LookAtNextAction());
		else
			m_curAction->Process();

		
		
		
		if (m_curAction->Finished()) 
		{
			GetNextAction();
		}
	}
	
	if (m_curAction != NULL) {
		
		m_frame = m_curAction->GetSpriteFrame();

		
		m_transparency = m_curAction->GetTransparency();
	}
}


void BattleViewActor::DumpAllActions(void)
{
	
	if (m_curAction != NULL) {
		m_facing = m_curAction->GetFacing();
		delete m_curAction;
		m_curAction = NULL;
	}

	
	Action *deadAction=NULL;
	while (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(deadAction);
		if (deadAction != NULL) {
			m_facing = deadAction->GetFacing();
			delete deadAction;
			deadAction = NULL;
		}
	}
}


void BattleViewActor::GetNextAction(BOOL isVisible)
{
	uint32 numItems = GetActionQueueNumItems();

	if (m_curAction) 
	{
		delete m_curAction;
		m_curAction = NULL;
	}

	Action *pendingAction = LookAtNextAction();
	
	while(numItems > 0 && pendingAction->GetActionType() == UNITACTION_NONE)
	{
		m_actionQueue.Dequeue(m_curAction);
		delete m_curAction;
		m_curAction = NULL;
		pendingAction = LookAtNextAction();
	}
		
		
	if (numItems > 0) 
	{

		
		m_actionQueue.Dequeue(m_curAction);

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


void BattleViewActor::AddAction(Action *actionObj)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return;
	
	Assert(actionObj != NULL);
	if (actionObj == NULL) return;


	
	
	
	
	if(g_theUnitPool->IsValid(GetUnitID())) 
	{
		m_playerNum = GetUnitID().GetOwner();
	}

	
	
	

	m_actionQueue.Enqueue(actionObj);

}

Anim *BattleViewActor::GetAnim(UNITACTION action)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return NULL;

	
	Anim	*origAnim = m_unitSpriteGroup->GetAnim((GAME_ACTION)action);

	if (origAnim == NULL) 
	{
		if(action != UNITACTION_IDLE)
		{
			return NULL;
		}
		else
		{
			
			origAnim = m_unitSpriteGroup->GetAnim((GAME_ACTION)UNITACTION_MOVE);;
			Assert(origAnim != NULL);
			if(origAnim == NULL)
				return NULL;
			else
				action = UNITACTION_MOVE;
		}
	}

	Anim	*anim = new Anim();
	*anim = *origAnim;
	anim->SetSpecialCopyDelete(ANIMXEROX_COPY);

	if(action == UNITACTION_IDLE)
	{
		srand(anim->GetDelay() + g_director->GetMasterCurTime());
		anim->AdjustDelay(rand() % 2000);
	}
	
	return anim;
}

#define k_FAKE_DEATH_FRAMES			15		
#define k_FAKE_DEATH_DURATION		1500	

Anim *BattleViewActor::MakeFakeDeath(void)
{
	sint32		i;
	Anim		*anim = new Anim();

	
	uint16 *frames = new uint16[k_FAKE_DEATH_FRAMES];
	for (i=0; i<k_FAKE_DEATH_FRAMES; i++) {
		frames[i] = 0;
	}

	
	POINT *moveDeltas = new POINT[k_FAKE_DEATH_FRAMES];
	POINT pt = {0,0};
	for (i=0; i<k_FAKE_DEATH_FRAMES; i++) {
		moveDeltas[i] = pt;
	}
	
	
	uint16 *transparencies = new uint16[k_FAKE_DEATH_FRAMES];
	for (i=0; i<k_FAKE_DEATH_FRAMES; i++) {
		transparencies[i] = (uint16)(15 - i);
	}

	anim->SetNumFrames(k_FAKE_DEATH_FRAMES);
	anim->SetFrames(frames);
	anim->SetPlaybackTime(k_FAKE_DEATH_DURATION);
	anim->SetDeltas(moveDeltas);
	anim->SetTransparencies(transparencies);

	anim->SetType(ANIMTYPE_SEQUENTIAL);

	return anim;
}

void BattleViewActor::Draw(BOOL fogged)
{
	uint16			flags;
	Pixel16			color;
	Unit			top;
	BOOL			directionAttack = FALSE;
	
	flags = k_DRAWFLAGS_NORMAL;
	color = 0x0000;

	
	
	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;	
	}

	if (fogged)
		flags |= k_BIT_DRAWFLAGS_FOGGED;

	if(m_curAction == NULL)
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
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color=0;
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

uint16 BattleViewActor::GetWidth(void)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite	*theSprite;

	theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
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

uint16 BattleViewActor::GetHeight(void)
{
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite	*theSprite;

	theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
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

void BattleViewActor::GetBoundingRect(RECT *rect)
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

