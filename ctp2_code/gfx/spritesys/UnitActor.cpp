//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit actor
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
// _DEBUG
// - Generates debug information when set.
//
// _DEBUG_MEMORY
// - Generates debug information when set.
//
// _TEST
// ?
//
// _ACTOR_DRAW_OPTIMIZATION
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Fixed number of city styles removed.
// - Prevented crashes due to uninitialised members.
// - Prevented some NULL-dereferencing crashes.
// - Exposed city walls and force field graphics to agecitystyle.txt,
//   by Martin Gühmann.
// - Prevented crashes with invalid (i.e. killed or destroyed) units.
// - PFT 29 mar 05, show # turns until city next grows a pop.
// - Removed refferences to the civilisation database. (Aug 20th 2005 Martin Gühmann)
// - Removed unnecessary include files. (Aug 28th 2005 Martin Gühmann)
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Removed unused local variables. (Sep 9th 2005 Martin Gühmann)
// - Fixed memory leaks.
// - added Hidden Nationality check for units 2-21-2007
// - Added Civilization flag MAPICONS
// - Added MapIcon database (3-Mar-2007 Martin Gühmann)
// - Implemented but then outcomment DrawCityImps just didn't come out right
//   maybe revisit
// - Made DrawStackingIndicator only the stack sized moved the rest to DrawIndicators
// - Made StackingIndicator above the healthbar per Maquiladora's design
// - Move Civ flag underneath the healthbar.
// - Unit stacking indications and special indecations are placed according
//   their size. (9-Feb-2008 Martin Gühmann)
// - Made the elite icon replace the veteran icon, rather than sit below it.
//	 (11-Apr-2009 Maq)
// - Stopped the cargo icon showing for enemy transports if they're only carrying
//	 stealth units. (13-Apr-2009 Maq)
// - Changed occurances of UnitRecord::GetMaxHP to
//   UnitData::CalculateTotalHP. (Aug 3rd 2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "UnitActor.h"

#include "AgeCityStyleRecord.h"
#include "Anim.h"
#include "ArmyData.h"
#include "aui_bitmapfont.h"
#include "BuildingRecord.h"
#include "buildingutil.h"
#include "cellunitlist.h"
#include "Civilisation.h"
#include "citydata.h"
#include "CityStyleRecord.h"
#include "citywindow.h"         // s_cityWindow
#include "gamefile.h"           // g_saveFileVersion
#include "maputils.h"
#include "primitives.h"
#include "profileDB.h"          // g_theProfileDB
#include "screenmanager.h"
#include "SelItem.h"            // g_selected_item
#include "soundmanager.h"       // g_soundManager
#include "SpriteGroupList.h"
#include "SpriteState.h"
#include "tiledmap.h"           // g_tiledMap
#include "UnitData.h"
#include "UnitPool.h"           // g_theUnitPool
#include "UnitRecord.h"
#include "WonderRecord.h"
#include "wonderutil.h"

extern SpriteGroupList     * g_unitSpriteGroupList;
extern SpriteGroupList     * g_citySpriteGroupList;
extern ScreenManager       * g_screenManager;
extern PointerList<Player> * g_deadPlayer;

#if !defined(_DEBUG_MEMORY) && defined(WIN32)
#define STOMPCHECK() if (m_curAction) { Assert(_CrtIsMemoryBlock(m_curAction, sizeof(Action),NULL,NULL,NULL));}
#else
#define STOMPCHECK() ;
#endif

static const int k_SHIELD_ON_TIME  = 650;
static const int k_SHIELD_OFF_TIME = 150;

static const sint32 CITY_TYPE_LAND  = 0;
static const sint32 CITY_TYPE_WATER = 1;

bool g_showHeralds = true;

SpriteGroupList * UnitActor::GetSpriteGroupList(GROUPTYPE groupType)
{
	return (groupType == GROUPTYPE_UNIT) ? g_unitSpriteGroupList : g_citySpriteGroupList;
}

UnitActor::UnitActor(
	SpriteState    * spriteState,
	const Unit     & id,
	sint32           unitType,
	const MapPoint & pos,
	sint32           owner,
	double           visionRange,
	sint32           citySprite,
	sint32           citySize)
:
	Actor                       (spriteState),
	m_unitID                    (id),
	m_unitDBIndex               (unitType),
	m_playerNum                 (owner),
	m_pos                       (pos),
	m_hidden                    (false),
	m_type                      (GROUPTYPE_UNIT),
	m_curAction                 (NULL),
	m_curUnitAction             (UNITACTION_NONE),
	m_actionQueue               (k_MAX_ACTION_QUEUE_SIZE),
	m_unitVisibility            (0),
	m_isFortified               (false),
	m_isFortifying              (false),
	m_hasCityWalls              (false),
	m_hasForceField             (false),
	m_citySize                  (citySize),
	m_nextPop                   (0),
	m_hiddenUnderStack          (false),
	m_healthPercent             (-1.0),
	m_tempStackSize             (0),
	m_spriteID                  (-1),
	m_unitSpriteGroup           (NULL),
	m_loadType                  (LOADTYPE_NONE),
	m_directionalAttack         (false),
	m_facing                    (k_DEFAULTSPRITEFACING),
	m_frame                     (0),
	m_transparency              (NO_TRANSPARENCY),
	m_shieldFlashOnTime         (0),
	m_shieldFlashOffTime        (0),
	m_refCount                  (1)
#ifdef _ACTOR_DRAW_OPTIMIZATION
//	sint32 m_oldFacing;
//	bool   m_oldIsFortified;
//	bool   m_oldIsFortifying;
//	bool   m_oldHasCityWalls;
//	bool   m_oldHasForceField;
//	bool   m_oldDrawShield;
//	bool   m_oldDrawSelectionBrackets;
//	uint16 m_oldFlags;
#endif
{
	sint32 spriteID = CTPRecord::INDEX_INVALID;
	GetIDAndType(owner, spriteState, id, unitType, pos, citySprite, spriteID, m_type);

	m_spriteID = (spriteID < 0) ? 0 : spriteID;
	Assert(m_spriteID >= 0);

	Initialize();
}

UnitActor::UnitActor(CivArchive & archive)
:
	Actor                       (NULL),
	m_unitID                    (),
	m_unitDBIndex               (CTPRecord::INDEX_INVALID),
	m_playerNum                 (PLAYER_UNASSIGNED),
	m_pos                       (),
	m_hidden                    (false),
	m_type                      (GROUPTYPE_UNIT),
	m_curAction                 (NULL),
	m_curUnitAction             (UNITACTION_NONE),
	m_actionQueue               (k_MAX_ACTION_QUEUE_SIZE),
	m_unitVisibility            (0),
	m_isFortified               (false),
	m_isFortifying              (false),
	m_hasCityWalls              (false),
	m_hasForceField             (false),
	m_citySize                  (0),
	m_nextPop                   (0),
	m_hiddenUnderStack          (false),
	m_healthPercent             (-1.0),
	m_tempStackSize             (0),
	m_spriteID                  (-1),
	m_unitSpriteGroup           (NULL),
	m_loadType                  (LOADTYPE_NONE),
	m_directionalAttack         (false),
	m_facing                    (k_DEFAULTSPRITEFACING),
	m_frame                     (0),
	m_transparency              (NO_TRANSPARENCY),
	m_shieldFlashOnTime         (0),
	m_shieldFlashOffTime        (0),
	m_refCount                  (1)
#ifdef _ACTOR_DRAW_OPTIMIZATION
//	sint32 m_oldFacing;
//	bool   m_oldIsFortified;
//	bool   m_oldIsFortifying;
//	bool   m_oldHasCityWalls;
//	bool   m_oldHasForceField;
//	bool   m_oldDrawShield;
//	bool   m_oldDrawSelectionBrackets;
//	uint16 m_oldFlags;
#endif
{
	Serialize(archive);
	Initialize();
}

void UnitActor::Initialize()
{
	m_loadType = LOADTYPE_BASIC;
	m_unitSpriteGroup = (UnitSpriteGroup *)
			GetSpriteGroupList(m_type)->GetSprite((uint32)m_spriteID, m_type, m_loadType,(GAME_ACTION)0);
	Assert(m_unitSpriteGroup);
	m_directionalAttack = m_unitSpriteGroup && m_unitSpriteGroup->HasDirectional();

	Actor::SetPos(0, 0);

	m_actionQueue.Allocate(k_MAX_ACTION_QUEUE_SIZE);

	AddIdle();
}

void UnitActor::PositionActor(const MapPoint & pos)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	m_pos = pos;

	sint32 pixelX, pixelY;
	maputils_MapXY2PixelXY(pos.x, pos.y, &pixelX, &pixelY);
	Actor::SetPos(pixelX, pixelY);
}

void UnitActor::GetIDAndType(
	sint32           owner,
	SpriteState    * spriteState,
	Unit             id,
	sint32           unitType,
	const MapPoint & pos,
	sint32           citySprite,
	sint32         & spriteID,
	GROUPTYPE      & groupType) const
{
	bool isCity = g_theUnitDB->Get(unitType, g_player[owner]->GetGovernmentType())->GetHasPopAndCanBuild();
	if (isCity)
	{
		if (citySprite >= 0) {
			spriteID = citySprite;
		} else if (id.IsValid() && id.CD()) {
			spriteID = id.CD()->GetDesiredSpriteIndex();
		} else if (spriteState) {
			spriteID = spriteState->GetIndex();
		} else {
			spriteID = CTPRecord::INDEX_INVALID;
		}

		groupType = GROUPTYPE_CITY;
	}
	else
	{
		spriteID  = spriteState->GetIndex();
		groupType = GROUPTYPE_UNIT;
	}
}

UnitActor::~UnitActor()
{
	DumpAllActions();

	SpriteGroupList * spriteGroupList = GetSpriteGroupList(m_type);
	spriteGroupList->ReleaseSprite(m_spriteID, m_loadType);
	if (LOADTYPE_BASIC != m_loadType) {
		spriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_BASIC);
	}

	delete m_spriteState;
}

void UnitActor::Hide()
{
	m_hidden = true;
}

void UnitActor::Show()
{
	m_hidden = false;
}

void UnitActor::ChangeImage(SpriteState * spriteState, sint32 type, const Unit & id)
{
	if (id.IsValid() && id.IsCity())
	{
		id.GetPop(m_citySize); // put the city's pop into the actor's m_size

		// PFT, computes TurnsToNextPop and puts
		// it into the actor's m_nextPop
		id.GetTurnsToNextPop(m_nextPop);
	}

	DumpAllActions();

	GetSpriteGroupList(m_type)->ReleaseSprite(m_spriteID, m_loadType);
	if (m_loadType != LOADTYPE_BASIC) {
		GetSpriteGroupList(m_type)->ReleaseSprite(m_spriteID, LOADTYPE_BASIC);
	}
	m_unitSpriteGroup = NULL;

	sint32 spriteID;
	GetIDAndType(m_playerNum, spriteState, id, type, m_pos, CTPRecord::INDEX_INVALID, spriteID, m_type);

	if (spriteID == -1)
	{
		spriteID = 1;

		const CityStyleRecord * cityStyle = g_theCityStyleDB->Get(0);
		if (cityStyle)
		{
			const AgeCityStyleRecord * ageStyle = cityStyle->GetAgeStyle(0);
			if (ageStyle && ageStyle->GetSprites(0)) {
				spriteID = ageStyle->GetSprites(0)->GetSprite();
			}
		}
	}
	m_spriteID = spriteID;

	m_loadType = LOADTYPE_BASIC;
	m_unitSpriteGroup = (UnitSpriteGroup *)
			GetSpriteGroupList(m_type)->GetSprite(spriteID, m_type, m_loadType, (GAME_ACTION)0);
	if (m_type == GROUPTYPE_CITY) {
		m_facing = k_DEFAULTSPRITEFACING;
	}

	AddIdle();
}

void UnitActor::ChangeType(SpriteState * spriteState, sint32 type, Unit id)
{
#ifndef _TEST
	STOMPCHECK();
#endif
    if (m_spriteState != spriteState)
    {
        delete m_spriteState;
        m_spriteState = spriteState;
    }
	m_unitID = id;

	ChangeImage(spriteState, type, id);

    m_directionalAttack = m_unitSpriteGroup && m_unitSpriteGroup->HasDirectional();

    if (id.IsValid()) {
        id.SetSpriteState(spriteState);
    }

	DumpAllActions();
	AddIdle();
}

void UnitActor::AddIdle()
{
	Anim * animation = CreateAnim(UNITACTION_IDLE);
	if (!animation) {
		animation = CreateAnim(UNITACTION_MOVE);
	}

	Action * idleAction = Action::CreateUnitAction(UNITACTION_IDLE, animation);
	AddAction(idleAction);
}

void UnitActor::GetNextAction()
{
	delete m_curAction;
	m_curAction = NULL;

	if (m_actionQueue.GetNumItems() > 0)
	{
		m_actionQueue.Dequeue(m_curAction);

		Assert(m_curAction);
		if (!m_curAction) {
			return;
		}

		m_facing = m_curAction->CalculateFacing(m_facing);
	} else {
		return;
	}

	if (m_curAction->GetActionType() == UNITACTION_ATTACK) {
		SetUnitVisibility(m_curAction->GetUnitsVisibility());
	}

	m_curUnitAction = (UNITACTION)m_curAction->GetActionType();
}

bool UnitActor::IsActionFinished() const
{
	return (!m_curAction
		|| m_curAction->GetActionType() == UNITACTION_IDLE || m_curAction->GetActionType() == UNITACTION_FACE_OFF);
}

void UnitActor::Process()
{
	if (!m_curAction) {
		GetNextAction();
	}

	if (!m_curAction)
	{
		DumpFullLoad();
		AddIdle();
	}

	if (m_curAction)
	{
		m_curAction->Process();

		if (m_curAction->IsFinished() && m_curAction->GetActionType() != UNITACTION_IDLE) {
			GetNextAction();
		}
	} else {
		GetNextAction();
	}

	if (!m_curAction) {
		DumpFullLoad();
	}

	if (m_curAction)
	{
		POINT current = m_curAction->CalculatePixelXY(m_pos);
		m_x = current.x;
		m_y = current.y;

		m_frame = m_curAction->GetSpriteFrame();
		m_transparency = m_curAction->GetTransparency();
	}
}

void UnitActor::Interrupt()
{
	if (m_curAction && m_curAction->GetActionType() == UNITACTION_IDLE)
	{
		delete m_curAction;
		m_curAction = NULL;
	}
}

void UnitActor::DumpAllActions()
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if (m_curAction) {
		m_facing = m_curAction->CalculateFacing(m_facing);
		delete m_curAction;
		m_curAction = NULL;
	}

	Action * otherAction = NULL;
	while (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(otherAction);
		if (otherAction)
		{
			m_facing = otherAction->CalculateFacing(m_facing);
			delete otherAction;
			otherAction = NULL;
		} else {
			Assert(false);
			return;
		}
	}
}

void UnitActor::AddAction(Action *action)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(action);
	if (!action) {
		return;
	}

	if (g_theUnitPool && g_theUnitPool->IsValid(GetUnitID())) {
		m_playerNum = Unit(GetUnitID()).GetOwner();
	}

	Interrupt();

	m_actionQueue.Enqueue(action);

	if (!m_curAction) {
		GetNextAction();
	}
}

Anim * UnitActor::CreateAnim(UNITACTION action) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return NULL;
	}

	Anim * animation = m_unitSpriteGroup->GetAnim((GAME_ACTION)action);
	return animation ? Anim::CreateSequential(*animation) : NULL;
}

void UnitActor::Draw(const RECT & paintRect)
{
	if (GetUnitVisibility() & (1 << g_selected_item->GetVisiblePlayer()))
	{
		sint32 tileX;
		maputils_MapX2TileX(m_pos.x, m_pos.y, &tileX);

		if (maputils_TilePointInTileRect(tileX, m_pos.y, paintRect)) {
			g_tiledMap->PaintUnitActor(this);
		}
	}
}

void UnitActor::DrawFortified(sint32 nudgeX, sint32 nudgeY, bool fogged) const
{
	sint32 surfWidth  = g_screenManager->GetSurfWidth();
	sint32 surfHeight = g_screenManager->GetSurfHeight();

	if ((m_x + nudgeX) > (surfWidth - g_tiledMap->GetZoomTilePixelWidth())) {
		return;
	}

	if ((m_y + nudgeY) > (surfHeight - g_tiledMap->GetZoomTilePixelHeight())) {
		return;
	}

	Pixel16 * fortifiedImage = g_tiledMap->GetTileSet()->GetImprovementData(34);
	DrawImage(fortifiedImage, nudgeX, nudgeY, fogged);
}

void UnitActor::DrawFortifying(sint32 xoffset, sint32 yoffset, bool fogged) const
{
	aui_BitmapFont * font = g_tiledMap->GetFont();
	if (!font) {
		return;
	}

	sint32 x = m_x + xoffset;
	sint32 y = m_y + yoffset;

	MBCHAR * fString = g_tiledMap->GetFortifyString();

	sint32 width  = font->GetStringWidth(fString);
	sint32 height = font->GetMaxHeight();

	RECT rect = {0, 0, width, height};
	OffsetRect(&rect, x - width/2, y - height/2);
	RECT clipRect = rect;

    if (clipRect.left < 0) {
    	clipRect.left = 0;
    }
	if (clipRect.top < 0) {
		clipRect.top = 0;
	}
	if (clipRect.right >= g_screenManager->GetSurfWidth()) {
		clipRect.right = g_screenManager->GetSurfWidth() - 1;
	}
	if (clipRect.bottom >= g_screenManager->GetSurfHeight()) {
		clipRect.bottom = g_screenManager->GetSurfHeight() - 1;
	}

	COLORREF colorRef = g_colorSet->GetColorRef(COLOR_BLACK);

	font->DrawString(g_screenManager->GetSurface(), &rect, &clipRect, fString, 0, colorRef, 0);

	if (fogged) {
		colorRef = g_colorSet->GetColorRef(COLOR_WHITE);
	} else {
		colorRef = g_colorSet->GetDarkColorRef(COLOR_WHITE);
	}

	OffsetRect(&rect, -1, -1);
	OffsetRect(&clipRect, -1, -1);

	font->DrawString(g_screenManager->GetSurface(), &rect, &clipRect, fString, 0, colorRef, 0);
}

//----------------------------------------------------------------------------
//
// Name       : UnitActor::DrawCityWalls
//
// Description: Draw city walls
//
// Parameters : fogged	: city is under fog of war
//
// Globals    : g_tiledMap
//				g_theCityStyleDB
//				g_player
//				g_theTerrainDB
//				g_theWorld
//
// Returns    : -
//
// Remark(s)  : Assumption: unit is a valid city with walls.
//              Does not draw force fields: use DrawForceField for that.
//
//----------------------------------------------------------------------------
void UnitActor::DrawCityWalls(sint32 nudgeX, sint32 nudgeY, bool fogged) const   //TODO make a draw wonders and draw buildings method
{
	const TileSet * tileSet   = g_tiledMap->GetTileSet();
	Pixel16       * cityImage = tileSet->GetImprovementData(38);	// default
	Unit unit(GetUnitID());

	// Test city style overrides.
	const CityStyleRecord * styleRec = g_theCityStyleDB->Get(unit.CD()->GetCityStyle());

	if (styleRec)
	{
		const AgeCityStyleRecord * ageStyleRec = styleRec->GetAgeStyle(g_player[unit->GetOwner()]->m_age);

		if (ageStyleRec)
		{
			bool isWater = g_theWorld->IsWater(m_pos);
			sint32 spriteCount = ageStyleRec->GetNumSprites();
			const AgeCityStyleRecord::SizeSprite * matchingSprite = NULL;

			for (sint32 i = 0; i < spriteCount; ++i)
			{
				const AgeCityStyleRecord::SizeSprite * sprite = ageStyleRec->GetSprites(i);

				if (sprite && (isWater == (CITY_TYPE_WATER == sprite->GetType())))
				{
					matchingSprite = sprite;
					// Check city size
					sint32 p;
					unit.CD()->GetPop(p);
					if ((sprite->GetMinSize() <= p) && (sprite->GetMaxSize() >= p)) {
						break;	// exact match found
					}
					// When no exact match has been found, the last (largest?)
					// of the correct type will be used.
				}
			}

			if (matchingSprite) {
				cityImage = tileSet->GetImprovementData(static_cast<uint16>(matchingSprite->GetWalls()));
			}
			// else: keep default
		}
	}
	// else: keep default

	DrawImage(cityImage, nudgeX, nudgeY, fogged);
}

//----------------------------------------------------------------------------
//
// Name       : UnitActor::DrawForceField
//
// Description: Draw city force field
//
// Parameters : fogged	: city is under fog of war
//
// Globals    : g_tiledMap
//				g_theCityStyleDB
//				g_player
//				g_theTerrainDB
//				g_theWorld
//
// Returns    : -
//
// Remark(s)  : Assumption: unit is a valid city with a force field.
//              Does not draw walls: use DrawCityWalls for that.
//
//----------------------------------------------------------------------------
void UnitActor::DrawForceField(sint32 nudgeX, sint32 nudgeY, bool fogged) const
{
	// Default sprite index (fixed number from original code)
	sint32 which;
	if (g_theWorld->IsLand(m_pos)) {
		which = 154;
	} else if (g_theWorld->IsWater(m_pos)) {
		which = 156;
	} else {
		which = 155; // space?
	}

	Unit unit(GetUnitID());

	// Test city style overrides.
	const CityStyleRecord * styleRec = g_theCityStyleDB->Get(unit.CD()->GetCityStyle());
	if (styleRec)
	{
		const AgeCityStyleRecord * ageStyleRec = styleRec->GetAgeStyle(g_player[unit->GetOwner()]->m_age);

		if (ageStyleRec)
		{
			bool isWater = g_theWorld->IsWater(m_pos);
			sint32 spriteCount = ageStyleRec->GetNumSprites();
			const AgeCityStyleRecord::SizeSprite * matchingSprite = NULL;

			for (sint32 i = 0; i < spriteCount; ++i)
			{
				const AgeCityStyleRecord::SizeSprite * sprite = ageStyleRec->GetSprites(i);
				if (sprite && (isWater == (CITY_TYPE_WATER == sprite->GetType())))
				{
					matchingSprite = sprite;
					// Check city size
					sint32 p;
					unit.CD()->GetPop(p);
					if ((sprite->GetMinSize() <= p) && (sprite->GetMaxSize() >= p)) {
						break;	// exact match found
					}
					// When no exact match has been found, the last (largest?)
					// of the correct type will be used.
				}
			}

			if (matchingSprite) {
				which = matchingSprite->GetForceField();
			}
			// else: keep default
		}
	}
	// else: keep default

	Pixel16 * cityImage = g_tiledMap->GetTileSet()->GetImprovementData((uint16)which);
	if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST) {
		g_tiledMap->DrawDitheredOverlayIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY, fogged);
	} else {
		g_tiledMap->DrawDitheredOverlayScaledIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY,
				g_tiledMap->GetZoomTilePixelWidth(), g_tiledMap->GetZoomTileGridHeight(), fogged);
	}
}

bool UnitActor::Draw(bool fogged)
{
	if (m_hidden) {
		return false;
	}
	if (m_hiddenUnderStack) {
		return false;
	}

	uint16 flags = k_DRAWFLAGS_NORMAL;
	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
	}
	if (fogged) {
		flags |= k_BIT_DRAWFLAGS_FOGGED;
	}

	bool isCloaked = false;
	if (m_unitID.IsValid())
	{
		if (m_unitID.IsAsleep())
		{
			flags |= k_BIT_DRAWFLAGS_DESATURATED;
			m_isFortified = false;
			m_isFortifying = false;
		}

		isCloaked = m_unitID.IsCloaked();
	}

	bool directionAttack = m_directionalAttack && (m_curUnitAction == UNITACTION_ATTACK);

	SELECT_TYPE  selectType;
	ID           selectedID;
	PLAYER_INDEX selectedPlayer;
	g_selected_item->GetTopCurItem(selectedPlayer, selectedID, selectType);

	Unit selectedUnit;
	if (selectType == SELECT_TYPE_LOCAL_CITY) {
		selectedUnit = selectedID;
	} else if (selectType == SELECT_TYPE_LOCAL_ARMY)
	{
		selectedUnit = Army(selectedID).GetTopVisibleUnit(selectedPlayer);
	}

	bool drawShield            = true;
	bool drawSelectionBrackets = false;

	if (selectedUnit.IsValid() && selectedUnit.GetActor() == this)
	{
		drawSelectionBrackets = true;
		if (GetTickCount() > m_shieldFlashOffTime)
		{
			drawShield = GetTickCount() > m_shieldFlashOnTime;
			if (drawShield)
			{
				m_shieldFlashOffTime = GetTickCount() + k_SHIELD_ON_TIME;
				m_shieldFlashOnTime  = m_shieldFlashOffTime + k_SHIELD_OFF_TIME;
			}
		}
	}

#ifdef _ACTOR_DRAW_OPTIMIZATION
	if (   (m_frame == m_oldFrame)
		&& (m_facing == m_oldFacing)
		&& (m_x+xoffset == m_oldOffsetX)
		&& (m_y+yoffset == m_oldOffsetY)
		&& (flags == m_oldFlags)
		&& (m_isFortified == m_oldIsFortified)
		&& (m_isFortifying == m_oldIsFortifying)
		&& (m_hasForceField == m_oldHasForceField)
		&& (m_hasCityWalls == m_oldHasCityWalls)
		&& (drawShield == m_oldDrawShield)
		)
	{
		if ( m_paintTwice > 1 )
		{
			return false;
		}
		m_paintTwice ++;
		return false;
	}

	m_paintTwice = 0;

	m_oldFrame = m_frame;
	m_oldFacing = m_facing;
	m_oldOffsetX = m_x + xoffset;
	m_oldOffsetY = m_y + yoffset;

	m_oldFlags = flags;
	m_oldIsFortified = m_isFortified;
	m_oldIsFortifying = m_isFortifying;
	m_oldHasForceField = m_hasForceField;
	m_oldHasCityWalls = m_hasCityWalls;
	m_oldDrawShield = drawShield;
	m_oldDrawSelectionBrackets = drawSelectionBrackets;
#endif

	if (m_unitSpriteGroup)
	{
		if (m_curAction) {
			POINT current = m_curAction->CalculatePixelXY(m_pos);
			m_x = current.x;
			m_y = current.y;
		}

		sint32 xoffset = (sint32)(k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
		sint32 yoffset = (sint32)(k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

		sint32 nudgeX = (sint32)((k_ACTOR_CENTER_OFFSET_X - 48) * g_tiledMap->GetScale());
		sint32 nudgeY = (sint32)((k_ACTOR_CENTER_OFFSET_Y - 48) * g_tiledMap->GetScale());

		if (m_unitID.IsValid() && m_unitID.IsCity()) {//emod - 3-10-2007
			DrawCityImprovements(nudgeX, nudgeY, fogged);
		}
		if (m_isFortified) {
			DrawFortified(nudgeX, nudgeY, fogged);
		}
		if (m_isFortifying) {
			DrawFortifying(xoffset, yoffset, fogged);
		}
		if (m_unitID.IsValid() && m_hasCityWalls) {
			DrawCityWalls(nudgeX, nudgeY, fogged);
		}

		uint16 transparency = m_transparency;
		if (isCloaked)
		{
			transparency = static_cast<uint16>(8 + (rand() % 5));
			flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
		}

		Pixel16 color = 0x0000;
		m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing,
				g_tiledMap->GetScale(), transparency, color, flags, directionAttack);

		bool forcefieldsEverywhere = false;
		if (g_player[m_playerNum])
		{
			if (wonderutil_GetForcefieldEverywhere(g_player[m_playerNum]->m_builtWonders)) {
				forcefieldsEverywhere = m_unitID.IsValid() && m_unitID.IsCity();
			}
		}

		if (m_hasForceField || forcefieldsEverywhere) {
			DrawForceField(nudgeX, nudgeY, fogged);
		}

	// adding drawwonders and buildings using mapicons
	//mapicons will save tile file space and make it so modders don't have to add a new tilefile all the time
	// also to cut on visible wonders slic and my visible wonder code doesn't work to well
	// bool check is in the method
	//emod
	}

	if (drawSelectionBrackets) {
		DrawSelectionBrackets();
	}

	if (drawShield) {
		DrawShield();
	}

	return true;
}

void UnitActor::DrawDirect(aui_Surface * surf, sint32 x, sint32 y, double scale) const
{
	if (m_unitSpriteGroup)
    {
		uint16  flags = k_DRAWFLAGS_NORMAL;
		Pixel16 color = 0;
		sint32 xoffset = (sint32)(k_ACTOR_CENTER_OFFSET_X * scale);
		sint32 yoffset = (sint32)(k_ACTOR_CENTER_OFFSET_Y * scale);
		m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame, x+xoffset, y+yoffset, m_facing,
				scale, m_transparency, color, flags);
	}
}

void UnitActor::DrawText(sint32 x, sint32 y, MBCHAR * unitText) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if(m_unitSpriteGroup) {
		m_unitSpriteGroup->DrawText(x, y, unitText);
	}
}

void UnitActor::DrawShield() const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if (m_citySize > 0) {
		return;
	}
	if (!g_showHeralds) {
		return;
	}
	if (m_unitID.IsValid() && m_unitID.IsCity()) {
		return;
	}

	RECT drawRect = DetermineShieldRect();
	if (!IsRectCompletelyOnScreen(drawRect)) {
		return;
	}

	sint32 stackSize = DetermineStackSize();

	drawRect = DrawSpecialIndicators(drawRect, stackSize);

	RECT dirtyRect = drawRect;
	InflateRect(&dirtyRect, 2, 2);
	dirtyRect.top -= 8;
	g_tiledMap->AddDirtyRectToMix(dirtyRect);

	RECT healthBarRect = drawRect;
	if (g_theProfileDB->GetShowEnemyHealth() || m_playerNum == g_selected_item->GetVisiblePlayer())
	{
		healthBarRect.bottom = healthBarRect.top + 4;
		if (!IsRectCompletelyOnScreen(healthBarRect)) {
			return;
		}
		drawRect.top += 3;
		drawRect.bottom += 4; // one extra to disconnect other elements
	}

	drawRect = DrawStackingIndicator(drawRect, stackSize);
	DrawHealthBar(healthBarRect, stackSize);
	DrawIndicators(drawRect);
}

//moved stacking indicators here because its actually called by the healthbar above.
RECT UnitActor::DrawStackingIndicator(const RECT & rect, sint32 stack) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	// Tested in DrawShield
	Assert(g_showHeralds);
	if (!IsRectCompletelyOnScreen(rect)) {
		return rect;
	}

// Remove the next line when the scaling and centering of the text has been implemented
// properly - or you want to test its operation. Currently, the generated text looks too
// ugly to include it in a release.
#undef USE_PREDEFINED_ICONS

#if defined(USE_PREDEFINED_ICONS)
	MAPICON icon = MAPICON_HERALD; // default: plain icon

	if (stack > 1 && stack <= 9) {
		// single digit predefined icons
		icon = (MAPICON) ((sint32) MAPICON_HERALD2 + stack - 2);
	} else if (stack >= 10 && stack <= 12) {
		// double digits predefined icons
		icon = (MAPICON) ((sint32) MAPICON_HERALD10 + stack - 10);
	}

	g_tiledMap->DrawColorizedOverlayIntoMix(
			g_tiledMap->GetTileSet()->GetMapIconData(icon), rect.left, rect.top, GetDisplayedColor());
#else
	MAPICON   icon    = MAPICON_HERALD; // default: plain icon
	TileSet	* tileSet = g_tiledMap->GetTileSet();

	Pixel16 displayedColor = GetDisplayedColor();
	g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(icon), rect.left, rect.top, displayedColor);

	if (stack > 1)
	{
		aui_BitmapFont * font = g_tiledMap->GetFont();
		if (!font) {
			return rect;
		}

		MBCHAR stackString[80];
		sprintf(stackString, "%i", stack);

		COLOR color = ColorSet::UseDarkFontColor(displayedColor) ? COLOR_BLACK : COLOR_WHITE;
		RECT textRect = { rect.left + (stack < 10 ? 5 : 1), rect.top + (stack < 10 ? 0 : -1),
					rect.left + (stack < 10 ? 12 : 14), rect.top + (stack < 10 ? 12 : 11) };

		aui_Surface * surf = g_screenManager->GetSurface();
		RECT clipRect = primitives_GetScreenAdjustedRectCopy(surf, textRect);
		font->DrawString(surf, &textRect, &clipRect, stackString, 0, g_colorSet->GetColorRef(color));
	}
#endif

	g_tiledMap->AddDirtyRectToMix(rect);

	RECT dirtyRect = rect;
	OffsetRect(&dirtyRect, 0, rect.bottom - rect.top);
	return dirtyRect;
}

//emod - moved these from stacking indicators to separate indicators
RECT UnitActor::DrawIndicators(const RECT & rect) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	// Tested in DrawShield
	Assert(g_showHeralds);

	if (!IsRectCompletelyOnScreen(rect)) {
		return rect;
	}

	TileSet * tileSet = g_tiledMap->GetTileSet();
	Pixel16 displayedColor = GetDisplayedColor();

	RECT drawRect = RECT{rect.left, rect.top, rect.left, rect.top};
	if (m_unitID.IsValid() && m_unitID->GetArmy().IsValid())
	{
		if (m_unitID->GetArmy()->Num() > 1)
		{
			drawRect = DrawIndicator(drawRect, MAPICON_ARMY, displayedColor);
		}
		// Replace veteran icon with elite icon if an elite unit exists in army.
		if (m_unitID->GetArmy()->HasElite())
		{
			drawRect = DrawIndicator(drawRect, MAPICON_ELITE, displayedColor);
		} else if (m_unitID->GetArmy()->HasVeterans())
		{
			drawRect = DrawIndicator(drawRect, MAPICON_VETERAN, displayedColor);
		}

		if (m_unitID->GetArmy()->HasCargo())
		{
			// Do not draw the cargo icon if enemy army is carrying only stealth.
			if (m_unitID->GetArmy()->HasCargoOnlyStealth() && m_playerNum != g_selected_item->GetVisiblePlayer())
			{}
			// Draw it in all other cases.
			else
			{
				drawRect = DrawIndicator(drawRect, MAPICON_CARGO, displayedColor);
			}
		}
	}

	g_tiledMap->AddDirtyToMix(rect.left, rect.top, drawRect.right - drawRect.left, drawRect.bottom - rect.top);
	return drawRect;
}

RECT UnitActor::DrawIndicator(const RECT & rect, MAPICON icon, Pixel16 displayedColor) const
{
	TileSet * tileSet = g_tiledMap->GetTileSet();
	POINT iconDimensions = tileSet->GetMapIconDimensions(icon);
	if (rect.top + iconDimensions.y < g_screenManager->GetSurfHeight())
	{
		g_tiledMap->DrawColorizedOverlayIntoMix(
				tileSet->GetMapIconData(icon), rect.left, rect.top, displayedColor);
		return RECT{
			rect.left,
			rect.top + iconDimensions.y,
			rect.left + std::max<sint32>(rect.right - rect.left, iconDimensions.x),
			rect.bottom + iconDimensions.y
		};
	}
	return rect;
}

// identifier for religious unit or national flag
RECT UnitActor::DrawSpecialIndicators(const RECT & rect, sint32 stack) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	// Tested in DrawShield
	Assert(g_showHeralds);

	Pixel16 displayedColor = GetDisplayedColor();

	//If religious unit it shows the religion icon else it shows the national flag - E Aug 27 2007
	sint32 religionIcon = 0;
	if (m_unitID.IsValid() && m_unitID.GetDBRec()->GetHasReligionIconIndex(religionIcon))
	{
		g_tiledMap->DrawColorizedOverlayIntoMix(
				g_tiledMap->GetTileSet()->GetMapIconData(religionIcon), rect.left, rect.top, displayedColor);
	}
	else if (g_theProfileDB->IsCivFlags())
	{
		sint32 displayedOwner = GetDisplayedOwner();
		sint32 civilisation = -1;
		// Add civilization flags here - moved flags here and edited the
		// heralds to put numbers on national flags emod 2-21-2007
		if (g_player[displayedOwner]) {
			civilisation = g_player[displayedOwner]->GetCivilisation()->GetCivilisation();
		}
		else
		{
			for (PointerList<Player>::Walker walk(g_deadPlayer); walk.IsValid(); walk.Next())
			{
				Player * player = walk.GetObj();
				if (player)
				{
					Civilisation * playerCivilisation = player->GetCivilisation();
					if (playerCivilisation && g_theCivilisationPool->IsValid(*playerCivilisation))
					{
						if (playerCivilisation->GetOwner() == displayedOwner) {
							civilisation = playerCivilisation->GetCivilisation();
						}
					}
				}
			}
		}

		sint32 civilisationIcon = 0;
		if (g_theCivilisationDB->Get(civilisation)->GetNationUnitFlagIndex(civilisationIcon) && civilisation > -1)
		{
			g_tiledMap->DrawColorizedOverlayIntoMix(
					g_tiledMap->GetTileSet()->GetMapIconData(civilisationIcon), rect.left, rect.top, displayedColor);
		}
	}

	g_tiledMap->AddDirtyRectToMix(rect);

	RECT dirtyRect = rect;
	OffsetRect(&dirtyRect, 0, rect.bottom - rect.top);
	return  dirtyRect;
}

//end emod

void UnitActor::DrawSelectionBrackets() const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if(!m_unitSpriteGroup) {
		return;
	}

	RECT rect;
	SetRect(&rect, 0, 0, 1, 1);

	OffsetRect(&rect,
			m_x + (sint32)(k_TILE_PIXEL_WIDTH * g_tiledMap->GetScale())/2,
			m_y + (sint32)(k_TILE_GRID_HEIGHT * g_tiledMap->GetScale())/2);

	InflateRect(&rect, 25, 25);

	g_tiledMap->AddDirtyRectToMix(rect);

	TileSet * tileSet = g_tiledMap->GetTileSet();
	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_BRACKET1);

	rect.right -= (iconDim.x+1);
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
	Pixel16 * botRight = tileSet->GetMapIconData(MAPICON_BRACKET3);
	Assert(botRight);
	if (!botRight) {
		return;
	}
	Pixel16 * botLeft = tileSet->GetMapIconData(MAPICON_BRACKET4);
	Assert(botLeft);
	if (!botLeft) {
		return;
	}

	COLOR color = COLOR_YELLOW;
	if (m_unitID.IsValid())
    {
		if (m_unitID.GetArmy().IsValid() && m_unitID.GetArmy().CanMove()) {
			color = COLOR_GREEN;
		} else if (m_unitID.IsCity()) {
			color = COLOR_RED;
		}
	}
	Pixel16	pixelColor = g_colorSet->GetColor(color);

	g_tiledMap->DrawColorizedOverlayIntoMix(topLeft, rect.left, rect.top, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(topRight, rect.right, rect.top, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(botRight, rect.right, rect.bottom, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(botLeft, rect.left, rect.bottom, pixelColor);
}

uint16 UnitActor::GetWidth() const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Sprite * sprite = GetSprite();
	return sprite ? sprite->GetWidth() : 0;
}

uint16 UnitActor::GetHeight() const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Sprite * sprite = GetSprite();
	return sprite ? sprite->GetHeight() : 0;
}

Sprite * UnitActor::GetSprite() const
{
	Assert(m_unitSpriteGroup);
	if (!m_unitSpriteGroup) {
		return NULL;
	}

	Sprite * sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);
	if (!sprite && (m_curUnitAction == UNITACTION_IDLE)) {
		sprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_MOVE);
	}
	return sprite;
}

void UnitActor::GetBoundingRect(RECT *rect) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(rect);
	if (!rect) {
		return;
	}
	if (!m_unitSpriteGroup) {
		return;
	}

	POINT  hotPoint = m_unitSpriteGroup->GetHotPoint(m_curUnitAction, m_facing);
	double scale = g_tiledMap->GetScale();

	sint32 x = m_x;
	if (Sprite::IsReversedFacing(m_facing)) {
		x += (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - (GetWidth()-hotPoint.x)) * scale);
	} else {
		x += (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
	}
	sint32 y = m_y + (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left = x;
	rect->top = y;
	rect->right = x + (sint32)((double)GetWidth() * scale);
	rect->bottom = y + (sint32)((double)GetHeight() * scale);
}

void UnitActor::Serialize(CivArchive &archive)
{
	if (archive.IsStoring())
	{
		DumpAllActions();

		if (m_unitID.IsValid() && m_unitID.IsCity())
		{
			m_unitID.GetPop(m_citySize);
		}

		archive << m_facing;
		archive.PutUINT8((uint8)m_isFortified); // was not saved before #244 and is only saved here to fill the fromer 4 bytes, i.e. slot could be reused
		archive.PutUINT8((uint8)m_isFortifying); // was not saved before #244 and is only saved here to fill the fromer 4 bytes, i.e. slot could be reused
		archive.PutUINT8((uint8)m_hasCityWalls); // without saving, known city walls are not drawn after a game reload (bad for sending armies, slavers)
		archive.PutUINT8((uint8)m_hasForceField); // without saving, known force fields are not drawn after a game reload (bad for sending armies)
		archive << m_citySize;
		{
			// previously m_isUnseenCellActor was stored here; it is unused and therefor deprecated
			uint8 unused = 0;
			archive.PutUINT8(unused);
		}

		archive.PutUINT8((uint8)m_type);
#if USE_FORMAT_67
		archive << m_spriteID;
#else
		archive.PutUINT8((uint8)m_spriteID);
#endif
		archive.PutUINT8((uint8)m_playerNum);
		archive.PutUINT32((uint32)m_unitID);
#if USE_FORMAT_67
		archive << m_unitDBIndex;
#else
		archive.PutUINT8((uint8)m_unitDBIndex);
#endif
		{
			// previously m_unitVisionRange was stored here; it is unused and therefor deprecated
			double unused = 0.0;
			archive << unused;
		}
		archive << m_unitVisibility;

		m_pos.Serialize(archive);
		m_spriteState->Serialize(archive);
	}
	else
	{
		archive >> m_facing;
		m_isFortified = (bool)archive.GetUINT8();
		m_isFortifying = (bool)archive.GetUINT8();
		m_hasCityWalls = (bool)archive.GetUINT8();
		m_hasForceField = (bool)archive.GetUINT8();
		archive >> m_citySize;
		{
			// previously m_isUnseenCellActor was read here; it is unused and therefor deprecated
			archive.GetUINT8();
		}
		m_type = (GROUPTYPE)archive.GetUINT8();

		if (g_saveFileVersion >= 67) {
			archive >> m_spriteID;
		} else {
			m_spriteID = (sint32)archive.GetUINT8();
		}

		m_playerNum = (sint32)archive.GetUINT8();
		m_unitID    = Unit(archive.GetUINT32());

		if (g_saveFileVersion >= 67) {
			archive >> m_unitDBIndex;
		} else {
			m_unitDBIndex = (sint32)archive.GetUINT8();
		}

		{
			// previously m_unitVisionRange was read here; it is unused and therefor deprecated
			double unused;
			archive >> unused;
		}

		archive >> m_unitVisibility;

		m_pos.Serialize(archive);
		delete m_spriteState;
		m_spriteState = new SpriteState(archive);
	}
}

void UnitActor::FullLoad(UNITACTION action)
{
	if (!g_theProfileDB->IsUnitAnim()) {
		return;
	}
	if (!m_unitSpriteGroup) {
		return;
	}
	if (m_type != GROUPTYPE_UNIT) {
		return;
	}
	if (m_loadType == LOADTYPE_FULL) {
		return;
	}

	m_loadType = LOADTYPE_FULL;
	SpriteGroup * group = g_unitSpriteGroupList->GetSprite(
			m_spriteID, m_unitSpriteGroup->GetType(), m_loadType,(GAME_ACTION)action);
	Assert(group == m_unitSpriteGroup);
}

Anim * UnitActor::CreateMoveAnim()
{
	return TryAnimation(UNITACTION_MOVE);
}

Anim * UnitActor::CreateDeadAnim(UNITACTION & unitAction)
{
	Anim * animation = NULL;
	if (m_unitSpriteGroup->HasDeath())
	{
		unitAction = UNITACTION_VICTORY;
		animation = TryAnimation(unitAction);
	}
	if(!animation)
	{
		unitAction = UNITACTION_FAKE_DEATH;
		animation = Anim::MakeFakeDeath();
	}
	return animation;
}

Anim * UnitActor::CreateAttackAnim()
{
	Anim * animation = TryAnimation(UNITACTION_ATTACK);
	if (!animation) {
		animation = TryAnimation(UNITACTION_IDLE);
	}
	return animation;
}

Anim * UnitActor::CreateSpecialAttackAnim()
{
	Anim * animation = TryAnimation(UNITACTION_WORK);
	if (!animation) {
		animation = TryAnimation(UNITACTION_ATTACK);
		if (!animation) {
			animation = TryAnimation(UNITACTION_IDLE);
		}
	}
	return animation;
}

Anim * UnitActor::CreateWorkAnim()
{
	Anim * animation = TryAnimation(UNITACTION_WORK);
	if (!animation)
	{
		animation = TryAnimation(UNITACTION_MOVE);
	}
	return animation;
}

Anim * UnitActor::TryAnimation(UNITACTION action)
{
	FullLoad(action);

	return CreateAnim(action); // theAnim must be deleted
}

void UnitActor::DumpFullLoad()
{
	if (!m_unitSpriteGroup) {
		return;
	}
	if (m_type != GROUPTYPE_UNIT) {
		return;
	}
	if (m_loadType != LOADTYPE_FULL) {
		return;
	}

	bool purged = g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_FULL);
	if (purged) {
		m_unitSpriteGroup = NULL;
	} else {
		m_loadType = LOADTYPE_BASIC;
	}
}

bool UnitActor::HitTest(const POINT & mousePoint) const
{
	bool    isDirectionAttack = m_directionalAttack && (m_curUnitAction == UNITACTION_ATTACK);
	sint32  xoffset           = (sint32)(k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32  yoffset           = (sint32)(k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());
	Pixel16 color             = COLOR_WHITE;
	uint16  flags             = 0;

	return m_unitSpriteGroup->HitTest(mousePoint, m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset,
			m_facing, g_tiledMap->GetScale(), m_transparency, color, flags,isDirectionAttack);
}

//----------------------------------------------------------------------------
//
// Name       : UnitActor::DrawCityImprovements
//
// Description: Draw wonders and buildings on city graphics
//
// Parameters : fogged  : city is under fog of war
//
// Globals    : g_tiledMap
//              g_theCityStyleDB
//              g_player
//              g_theTerrainDB
//              g_theWorld
//
// Returns    : -
//
// Remark(s)  : Assumption: checks for icons
//              Replaced dithered with overlay to make it work better
//              Places wonder "behind" the city and city name
//              Looks a lot nicer
//----------------------------------------------------------------------------
void UnitActor::DrawCityImprovements(sint32 nudgeX, sint32 nudgeY, bool fogged) const
{
	TileSet * tileSet = g_tiledMap->GetTileSet();

	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);
	if ((m_x + nudgeX) >= g_screenManager->GetSurfWidth() - iconDim.x) {
		return;
	}
	if ((m_y + nudgeY) >= g_screenManager->GetSurfHeight() - iconDim.y) {
		return;
	}

	Unit unit(m_unitID);
	sint32 cityIcon = 0;
	if (unit.IsValid() && unit.IsCity())
	{
		for (sint32 building = 0; building < g_theBuildingDB->NumRecords(); building++)
		{
			if (buildingutil_Get(building, m_playerNum)->GetShowCityIconBottomIndex(cityIcon))
			{
				if (unit.CD()->GetImprovements() & ((uint64)1 << building))
				{
					DrawImage(tileSet->GetMapIconData(cityIcon), nudgeX, nudgeY, fogged);
					nudgeX += 5;
				}
			}
		}

		for (sint32 i = 0; i < g_theWonderDB->NumRecords(); i++)
		{
			if (wonderutil_Get(i, m_playerNum)->GetShowCityIconBottomIndex(cityIcon))
			{
				if (unit.CD()->GetBuiltWonders() & (uint64)1 << (uint64)i)
				{
					DrawImage(tileSet->GetMapIconData(cityIcon), nudgeX, nudgeY, fogged);
					nudgeX += 5;
				}
			}
		}
	}
}

void UnitActor::DrawImage(Pixel16 * image, sint32 nudgeX, sint32 nudgeY, bool fogged) const
{
	if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST)
	{
		if (fogged) {
			g_tiledMap->DrawBlendedOverlayIntoMix(image, m_x + nudgeX, m_y + nudgeY, k_FOW_COLOR, k_FOW_BLEND_VALUE);
		} else {
			g_tiledMap->DrawColorizedOverlayIntoMix(image, m_x + nudgeX, m_y + nudgeY, 0x0000);
		}
	}
	else
	{
		if (fogged) {
			g_tiledMap->DrawBlendedOverlayScaledIntoMix(image, m_x + nudgeX, m_y + nudgeY,
					g_tiledMap->GetZoomTilePixelWidth(), g_tiledMap->GetZoomTileGridHeight(), k_FOW_COLOR,
					k_FOW_BLEND_VALUE);
		} else {
			g_tiledMap->DrawScaledOverlayIntoMix(image, m_x + nudgeX, m_y + nudgeY,
					g_tiledMap->GetZoomTilePixelWidth(), g_tiledMap->GetZoomTileGridHeight());
		}
	}
}

void UnitActor::CopyFlags(const UnitActor & other)
{
	m_isFortified   = other.m_isFortified;
	m_isFortifying  = other.m_isFortifying;
	m_hasCityWalls  = other.m_hasCityWalls;
	m_hasForceField = other.m_hasForceField;
}

RECT UnitActor::DetermineShieldRect() const
{
	POINT iconDimensions = g_tiledMap->GetTileSet()->GetMapIconDimensions(MAPICON_HERALD);
	RECT iconRect = {0, 0, iconDimensions.x, iconDimensions.y};

	UNITACTION unitAction = m_curUnitAction;
	if (!m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) unitAction)) {
		unitAction = UNITACTION_IDLE;
	}

	if (unitAction == UNITACTION_IDLE && !m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE))
	{
		POINT * shieldPoint = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
		OffsetRect(&iconRect,
				   m_x + (sint32)((double)(shieldPoint->x) * g_tiledMap->GetScale()),
				   m_y + (sint32)((double)(shieldPoint->y) * g_tiledMap->GetScale()));
	}
	else
	{
		if (m_unitSpriteGroup && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)unitAction))
		{
			POINT * shieldPoint = m_unitSpriteGroup->GetShieldPoints(unitAction);
			OffsetRect(&iconRect,
					   m_x + (sint32)((double)(shieldPoint->x) * g_tiledMap->GetScale()),
					   m_y + (sint32)((double)(shieldPoint->y) * g_tiledMap->GetScale()));
		}
		else
		{
			sint32 top = m_y;
			sint32 middle = m_x + (sint32)((k_TILE_PIXEL_WIDTH) * g_tiledMap->GetScale())/2;
			OffsetRect(&iconRect, middle - iconDimensions.x / 2, top - iconDimensions.y);
		}
	}
	return iconRect;
}

sint32 UnitActor::DetermineStackSize() const
{
	sint32 stackSize = 1;
	if (m_tempStackSize != 0)
	{
		stackSize = m_tempStackSize;
	}
	else if (IsActive())
	{
		if (m_unitID.IsValid())
		{
			Army army = m_unitID.GetArmy();
			if (army.IsValid()) {
				stackSize = army.Num();
			}
		}
	}
	else
	{
		CellUnitList * unitList = g_theWorld->GetCell(m_pos)->UnitArmy();
		if (unitList)
		{
			stackSize = unitList->Num();
			for (sint32 i = 0; i < unitList->Num(); i++)
			{
				Unit top;

				Army army = Army(unitList->Access(i).GetArmy().m_id);
				if (army.IsValid()) {
					top = army->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
				}

				if (!top.IsValid()) {
					top.m_id = unitList->Access(i).m_id;
				}

				if (top.GetActor() && top.GetActor()->IsActive()) {
					stackSize--;
				}
			}
		}
	}
	return stackSize;
}

double UnitActor::CalculateHealthPercentage(sint32 stackSize) const
{
	double healthPercentage = std::max<double>(0.0, m_healthPercent);
	if (m_unitID.IsValid())
	{
		Cell * cell = g_theWorld->GetCell(m_pos);
		if (stackSize > 1 && cell->GetNumUnits()) {
			healthPercentage = std::max<double>(0.0, cell->UnitArmy()->GetAverageHealthPercentage());
		}
		else if (m_healthPercent < 0) {
			healthPercentage = std::max<double>(0.0, m_unitID.GetHP() / m_unitID->CalculateTotalHP());
		}
		else {
			healthPercentage = 0.0;
		}
	}
	return healthPercentage;
}

sint32 UnitActor::GetDisplayedOwner() const
{
	sint32 displayedOwner = m_playerNum;
	if (m_unitID.IsValid() && m_unitID.IsHiddenNationality() && (m_playerNum != g_selected_item->GetVisiblePlayer())) {
		// Display unit as barbarians
		displayedOwner = PLAYER_INDEX_VANDALS;
	}
	return displayedOwner;
}

Pixel16 UnitActor::GetDisplayedColor() const
{
	return g_colorSet->GetPlayerColor(GetDisplayedOwner());
}

void UnitActor::DrawHealthBar(const RECT & rect, sint32 stackSize) const
{
	if (!g_theProfileDB->GetShowEnemyHealth() && m_playerNum != g_selected_item->GetVisiblePlayer()) {
		return;
	}

	RECT healthBarRect = rect;

	Pixel16 black = g_colorSet->GetColor(COLOR_BLACK);
	if (black == 0x0000) {
		black = 0x0001;
	}

	// Draw background
	primitives_FrameRect16(g_screenManager->GetSurface(), &healthBarRect, black);

	InflateRect(&healthBarRect, -1, -1);

	double healthPercentage = CalculateHealthPercentage(stackSize);
	sint32 healthBarLength = healthBarRect.right - healthBarRect.left;
	if (healthPercentage < 1.0)
	{
		healthBarLength = (sint32) (healthPercentage * healthBarLength);
		RECT blackRect = healthBarRect;
		blackRect.left += healthBarLength;
		primitives_PaintRect16(g_screenManager->GetSurface(), &blackRect, black);
	}

	Pixel16 color = GetHealthBarColor(healthPercentage);
	RECT colorRect  = healthBarRect;
	colorRect.right = colorRect.left + healthBarLength;
	primitives_PaintRect16(g_screenManager->GetSurface(), &colorRect, color);
}

bool UnitActor::IsRectCompletelyOnScreen(const RECT & rect)
{
	return rect.left >= 0 && rect.right < g_screenManager->GetSurfWidth()
		&& rect.top >= 0 && rect.bottom < g_screenManager->GetSurfHeight();
}

#ifdef _DEBUG
void UnitActor::DumpActor()
{
	DPRINTF(k_DBG_UI, ("Actor %#.8lx\n", this));
	DPRINTF(k_DBG_UI, ("  m_unitID           :%#.8lx\n", m_unitID.m_id));
	DPRINTF(k_DBG_UI, ("  m_unitDBIndex      :%d\n", m_unitDBIndex));
	DPRINTF(k_DBG_UI, ("  m_curAction        :%#.8lx\n", m_curAction));

	if (m_curAction)
	{
		DPRINTF(k_DBG_UI, ("  m_curAction.m_actionType     :%ld\n", m_curAction->GetActionType()));
		DPRINTF(k_DBG_UI, ("  m_curAction.m_finished       :%ld\n", m_curAction->IsFinished()));
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));

	DPRINTF(k_DBG_UI, ("  m_actionQueue         :%d\n", m_actionQueue.GetNumItems()));
	if (m_actionQueue.GetNumItems() > 0)
	{
		for (size_t i = 0; i < m_actionQueue.GetNumItems(); i++)
		{
			Action * action = NULL;
			m_actionQueue.GetQueueItem(i, action);
			DPRINTF(k_DBG_UI, ("  m_actionQueue Item      :%u\n", i));

			if (action)
			{
				DPRINTF(k_DBG_UI, ("  action.m_actionType     :%ld\n", action->GetActionType()));
				DPRINTF(k_DBG_UI, ("  action.m_finished       :%ld\n", action->IsFinished()));
			}
		}
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));
}
#endif
