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
#include "ArmyData.h"
#include "aui_bitmapfont.h"
#include "BuildingRecord.h"
#include "cellunitlist.h"
#include "CityStyleRecord.h"
#include "citywindow.h"         // s_cityWindow
#include "Civilisation.h"
#include "colorset.h"           // g_colorset
#include "CTPRecord.h"
#include "debugmemory.h"
#include "director.h"           // g_director
#include "maputils.h"
#include "player.h"             // g_player
#include "primitives.h"
#include "profileDB.h"          // g_theProfileDB
#include "screenmanager.h"
#include "SelItem.h"            // g_selected_item
#include "SpriteGroupList.h"
#include "SpriteState.h"
#include "soundmanager.h"       // g_soundManager
#include "tiledmap.h"           // g_tiledMap
#include "UnitData.h"
#include "UnitPool.h"           // g_theUnitPool
#include "UnitRecord.h"
#include "WonderRecord.h"
#include "wonderutil.h"
#include "buildingutil.h"
#include "gamefile.h" // g_saveFileVersion

extern SpriteGroupList     *g_unitSpriteGroupList;
extern SpriteGroupList     *g_citySpriteGroupList;
extern ScreenManager       *g_screenManager;
extern PointerList<Player> *g_deadPlayer;

#define k_SHIELD_ON_TIME        650
#define k_SHIELD_OFF_TIME       150

#if !defined(_DEBUG_MEMORY) && defined(WIN32)
#define STOMPCHECK() if (m_curAction) { Assert(_CrtIsMemoryBlock(m_curAction, sizeof(Action),NULL,NULL,NULL));}
#else
#define STOMPCHECK() ;
#endif

bool                g_showHeralds   = true;

namespace
{
	sint32 const	CITY_TYPE_LAND	= 0;
	sint32 const	CITY_TYPE_WATER	= 1;

};	// namespace

UnitActor::UnitActor(SpriteState *ss, Unit id, sint32 unitType, const MapPoint &pos, sint32 owner, BOOL isUnseenCellActor,
					 double visionRange, sint32 citySprite)
:
    Actor                       (ss),
	m_refCount                  (1),
    m_pos                       (pos),
	m_savePos                   (),
    m_unitID                    (id),
    m_unitDBIndex               (unitType),
    m_playerNum                 (owner),
    m_nextPop                   (0),
    m_unitSpriteGroup           (NULL),
    m_loadType                  (LOADTYPE_NONE),
    m_facing                    (k_DEFAULTSPRITEFACING),
    m_lastMoveFacing            (k_DEFAULTSPRITEFACING),
    m_frame                     (0),
//	uint16				m_transparency;
    m_curAction                 (NULL),
    m_curUnitAction             (UNITACTION_NONE),
    m_actionQueue               (k_MAX_ACTION_QUEUE_SIZE),
//	RECT				m_heraldRect;
	m_unitVisibility            (0),
    m_unitSaveVisibility        (0),
    m_directionalAttack         (false),
    m_needsToDie                (false),
    m_needsToVictor             (false),
    m_killNow                   (false),
	m_unitVisionRange           (visionRange),
    m_newUnitVisionRange        (0.0),
    m_numRevealedActors         (0),
    m_revealedActors            (NULL),
    m_numSavedRevealedActors    (0),
    m_savedRevealedActors       (NULL),
    m_bVisSpecial               (false),
    m_moveActors                (NULL),
    m_numOActors                (0),
    m_hidden                    (false),
    m_hiddenUnderStack          (false),
    m_isTransported             (false),
//	sint32				m_holdingCurAnimPos[UNITACTION_MAX];
//	sint32				m_holdingCurAnimDelayEnd[UNITACTION_MAX];
//	sint32				m_holdingCurAnimElapsed[UNITACTION_MAX];
//	sint32				m_holdingCurAnimLastFrameTime[UNITACTION_MAX];
//	sint32				m_holdingCurAnimSpecialDelayProcess;
    m_size                      (0),
    m_isUnseenCellActor         (isUnseenCellActor),
//	GROUPTYPE			m_type;
//	sint32				m_spriteID;
    m_isFortified               (false),
    m_isFortifying              (false),
    m_hasCityWalls              (false),
    m_hasForceField             (false),
//	uint32				m_shieldFlashOnTime;
//	uint32				m_shieldFlashOffTime;
//	sint32				m_activeListRef;
//	double				m_healthPercent;
    m_tempStackSize             (0)
#ifdef _ACTOR_DRAW_OPTIMIZATION
//	sint32				m_oldFacing;
//	BOOL				m_oldIsFortified;
//	BOOL				m_oldIsFortifying;
//	BOOL				m_oldHasCityWalls;
//	BOOL				m_oldHasForceField;
//	BOOL				m_oldDrawShield;
//	BOOL				m_oldDrawSelectionBrackets;
//	uint16				m_oldFlags;
#endif
{
	sint32 spriteID = CTPRecord::INDEX_INVALID;
	GetIDAndType(owner, ss, id, unitType, pos, &spriteID, &m_type, citySprite);

	m_spriteID = (spriteID < 0) ? 0 : spriteID;
	Assert(m_spriteID >= 0);

	Initialize();
}










UnitActor::UnitActor(CivArchive &archive)
:
    Actor                       (NULL),
    m_refCount                  (1),
    m_pos                       (),
	m_savePos                   (),
    m_unitID                    (),
    m_unitDBIndex               (CTPRecord::INDEX_INVALID),
    m_playerNum                 (PLAYER_UNASSIGNED),
    m_nextPop                   (0),
    m_unitSpriteGroup           (NULL),
    m_loadType                  (LOADTYPE_NONE),
    m_facing                    (k_DEFAULTSPRITEFACING),
    m_lastMoveFacing            (k_DEFAULTSPRITEFACING),
    m_frame                     (0),
//	uint16				m_transparency;
    m_curAction                 (NULL),
    m_curUnitAction             (UNITACTION_NONE),
    m_actionQueue               (k_MAX_ACTION_QUEUE_SIZE),
//	RECT				m_heraldRect;
	m_unitVisibility            (0),
    m_unitSaveVisibility        (0),
    m_directionalAttack         (false),
    m_needsToDie                (false),
    m_needsToVictor             (false),
    m_killNow                   (false),
	m_unitVisionRange           (0.0),
    m_newUnitVisionRange        (0.0),
    m_numRevealedActors         (0),
    m_revealedActors            (NULL),
    m_numSavedRevealedActors    (0),
    m_savedRevealedActors       (NULL),
    m_bVisSpecial               (false),
    m_moveActors                (NULL),
    m_numOActors                (0),
    m_hidden                    (false),
    m_hiddenUnderStack          (false),
    m_isTransported             (false),
//	sint32				m_holdingCurAnimPos[UNITACTION_MAX];
//	sint32				m_holdingCurAnimDelayEnd[UNITACTION_MAX];
//	sint32				m_holdingCurAnimElapsed[UNITACTION_MAX];
//	sint32				m_holdingCurAnimLastFrameTime[UNITACTION_MAX];
//	sint32				m_holdingCurAnimSpecialDelayProcess;
    m_size                      (0),
    m_isUnseenCellActor         (false),
//	GROUPTYPE			m_type;
//	sint32				m_spriteID;
    m_isFortified               (false),
    m_isFortifying              (false),
    m_hasCityWalls              (false),
    m_hasForceField             (false),
//	uint32				m_shieldFlashOnTime;
//	uint32				m_shieldFlashOffTime;
//	sint32				m_activeListRef;
//	double				m_healthPercent;
    m_tempStackSize             (0)
#ifdef _ACTOR_DRAW_OPTIMIZATION
//	sint32				m_oldFacing;
//	BOOL				m_oldIsFortified;
//	BOOL				m_oldIsFortifying;
//	BOOL				m_oldHasCityWalls;
//	BOOL				m_oldHasForceField;
//	BOOL				m_oldDrawShield;
//	BOOL				m_oldDrawSelectionBrackets;
//	uint16				m_oldFlags;
#endif
{
	Serialize(archive);
	Initialize();
}

void UnitActor::Initialize(void)
{
	RECT			tmpRect = {0, 0, 10, 16};

	m_heraldRect = tmpRect;
	m_bVisSpecial = FALSE;
	m_animPos = 0;
	m_needsToDie = FALSE;
	m_needsToVictor = FALSE;
	m_killNow = FALSE;
	m_numOActors = 0;
	m_curUnitAction			= UNITACTION_NONE;
	m_transparency			= 0;
	m_numRevealedActors		= 0;

	for (sint32 i = UNITACTION_MOVE; i<UNITACTION_MAX; i++)
	{
		m_holdingCurAnimPos[i] = 0;
		m_holdingCurAnimDelayEnd[i] = 0;
		m_holdingCurAnimElapsed[i] = 0;
		m_holdingCurAnimLastFrameTime[i] = 0;
		m_holdingCurAnimSpecialDelayProcess = FALSE;
	}

	if (m_type == GROUPTYPE_UNIT)
	{
		m_unitSpriteGroup = (UnitSpriteGroup *)g_unitSpriteGroupList->GetSprite((uint32)m_spriteID, m_type, LOADTYPE_BASIC,(GAME_ACTION)0);
	}
	else
	{
		m_unitSpriteGroup = (UnitSpriteGroup *)g_citySpriteGroupList->GetSprite((uint32)m_spriteID, m_type, LOADTYPE_BASIC,(GAME_ACTION)0);
	}

	m_loadType = LOADTYPE_BASIC;

	Assert(m_unitSpriteGroup);
	if(m_unitSpriteGroup) {
		m_directionalAttack	= m_unitSpriteGroup->HasDirectional();
	} else {
		m_directionalAttack = FALSE;
	}

	m_savePos.x = m_savePos.y = 0;

	m_x = 0;
	m_y = 0;

	m_frame = 0;

	m_curAction = NULL;
	m_actionQueue.Allocate(k_MAX_ACTION_QUEUE_SIZE);


	m_numSavedRevealedActors = 0;
	m_savedRevealedActors = m_revealedActors = NULL;
	m_moveActors = NULL;
	m_hiddenUnderStack = FALSE;
	m_isTransported = FALSE;

	m_hidden = FALSE;

	m_shieldFlashOnTime = 0;
	m_shieldFlashOffTime = 0;

	m_activeListRef = 0;

	m_healthPercent = -1.0;

	m_tempStackSize = 0;

	AddIdle();
}

void UnitActor::AddVision(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if(!m_isUnseenCellActor)
	{
		g_player[m_playerNum]->m_vision->AddVisible(m_pos, m_unitVisionRange);
	}
}

void UnitActor::RemoveVision(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if(!m_isUnseenCellActor)
	{
		g_player[m_playerNum]->m_vision->RemoveVisible(m_pos,  m_unitVisionRange);
	}
}

void UnitActor::PositionActor(MapPoint &pos)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	m_pos = pos;

	sint32 pixelX, pixelY;
	maputils_MapXY2PixelXY(pos.x, pos.y, &pixelX, &pixelY);

	SetX(pixelX);
	SetY(pixelY);
	SetPos(pos);
}

void UnitActor::GetIDAndType
(
    sint32              owner,
    SpriteState *       ss,
    Unit                id,
    sint32              unitType,
    MapPoint const &    pos,
    sint32 *            spriteID,
    GROUPTYPE *         groupType,
    sint32              citySprite
) const
{
	bool    isCity  = g_theUnitDB->Get(unitType, g_player[owner]->GetGovernmentType())->GetHasPopAndCanBuild();

	if (isCity)
	{
		if(citySprite >= 0)
		{
			*spriteID = citySprite;
		}
		else if(id.IsValid() && id.CD())
		{
			*spriteID = id.CD()->GetDesiredSpriteIndex();
		}
		else if(ss)
		{
			*spriteID = ss->GetIndex();
		}
		else
		{
			*spriteID = CTPRecord::INDEX_INVALID;
		}

		*groupType = GROUPTYPE_CITY;
	}
	else
	{
		*spriteID = ss->GetIndex();
		*groupType = GROUPTYPE_UNIT;
	}
}

UnitActor::~UnitActor()
{
	DumpAllActions();

	if (m_type == GROUPTYPE_UNIT)
    {
		g_unitSpriteGroupList->ReleaseSprite(m_spriteID, m_loadType);
        if (LOADTYPE_BASIC != m_loadType)
        {
            g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_BASIC);
        }
	}
    else
    {
		g_citySpriteGroupList->ReleaseSprite(m_spriteID, m_loadType);
        if (LOADTYPE_BASIC != m_loadType)
        {
		    g_citySpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_BASIC);
        }
	}

	delete [] m_savedRevealedActors;
	delete [] m_revealedActors;
	delete m_spriteState;
}

void UnitActor::Hide(void)
{
	m_hidden = TRUE;
}

void UnitActor::Show(void)
{
	m_hidden = FALSE;
}

void UnitActor::ChangeImage(SpriteState *ss, sint32 type, Unit id)
{
	if (id.IsValid() && id.IsCity()) {
		id.GetPop(m_size);//put the city's pop into the actor's m_size

		CityWindow* cityWindow = CityWindow::GetCityWindow();
		CityData* citaData = cityWindow ? cityWindow->GetCityData() : NULL;
		if(citaData != NULL && citaData->GetHomeCity() == id)
		{
			m_nextPop = citaData->TurnsToNextPop();
		}
		else
		{
			// PFT, computes TurnsToNextPop and puts
			// it into the actor's m_nextPop
			id.GetTurnsToNextPop(m_nextPop);
		}
	}

	DumpAllActions();


	if (m_type == GROUPTYPE_UNIT) {
		if (g_unitSpriteGroupList->ReleaseSprite(m_spriteID, m_loadType))
			m_unitSpriteGroup = NULL;
	} else {
		if (g_citySpriteGroupList->ReleaseSprite(m_spriteID, m_loadType))
			m_unitSpriteGroup = NULL;
	}

	GROUPTYPE		groupType;
	sint32			spriteID;
	GetIDAndType(m_playerNum, ss, id, type, m_pos, &spriteID, &groupType);

	m_type = groupType;

	if (spriteID == -1) {

		spriteID = 1;

		const CityStyleRecord *styleRec = g_theCityStyleDB->Get(0);
		if(styleRec) {
			const AgeCityStyleRecord *ageStyleRec = styleRec->GetAgeStyle(0);
			if(ageStyleRec && ageStyleRec->GetSprites(0)) {
				spriteID = ageStyleRec->GetSprites(0)->GetSprite();
			}
		}
	}

	m_spriteID = spriteID;

	if (groupType == GROUPTYPE_UNIT)
	{
		m_unitSpriteGroup = (UnitSpriteGroup *)g_unitSpriteGroupList->GetSprite(spriteID, groupType, LOADTYPE_BASIC,(GAME_ACTION)0);
	}
	else
	{

		m_unitSpriteGroup = (UnitSpriteGroup *)g_citySpriteGroupList->GetSprite(spriteID, groupType, LOADTYPE_BASIC,(GAME_ACTION)0);

		m_lastMoveFacing = 0;
		m_facing = 0;
	}

	AddIdle();

	m_loadType = LOADTYPE_BASIC;
}

void UnitActor::ChangeType(SpriteState *ss, sint32 type,  Unit id, BOOL updateVision)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, GetUnitID());

    if (m_spriteState != ss)
    {
        delete m_spriteState;
        m_spriteState = ss;
    }
	m_unitID = id;

	ChangeImage(ss, type, id);

	if (updateVision) {
		if(g_tiledMap->GetLocalVision() != NULL &&
		   m_playerNum == g_selected_item->GetVisiblePlayer() &&
		   !m_isUnseenCellActor) {
			DPRINTF(k_DBG_INFO, ("Removing vision for %lx, owner %d, range %lf, center: %d,%d\n",
								 (uint32)m_unitID, m_playerNum, m_unitVisionRange, m_pos.x, m_pos.y));

		}
	}

	m_unitVisionRange = m_newUnitVisionRange;

	if (updateVision) {
		if(g_tiledMap->GetLocalVision() != NULL &&
		   m_playerNum == g_selected_item->GetVisiblePlayer() &&
		   !m_isUnseenCellActor)
		{
			DPRINTF(k_DBG_INFO, ("Adding vision for %lx, owner %d, range %lf, center: %d,%d\n",
								 m_unitID.m_id, m_playerNum, m_unitVisionRange,
								 m_pos.x, m_pos.y));

		}
	}

    m_directionalAttack = m_unitSpriteGroup && m_unitSpriteGroup->HasDirectional();

    if (id.IsValid())
    {
        id.SetSpriteState(ss);
    }

	DumpAllActions();
	AddIdle();
}

void UnitActor::AddIdle(bool NoIdleJustDelay)
{
	Anim *  anim    = CreateAnim(UNITACTION_IDLE);
	m_frame         = 0;

	if (anim == NULL) {
		anim = CreateAnim(UNITACTION_MOVE);
	}

	if (anim && ((GetActionQueueNumItems() > 0) || NoIdleJustDelay))
	{
		anim->SetNoIdleJustDelay(TRUE);
	}

	Action * idleAction =
        new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT, 0, NoIdleJustDelay);

    if (NoIdleJustDelay)
    {
		idleAction->SetFacing(m_facing);
	}

	idleAction->SetAnim(anim);

	AddAction(idleAction);




	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, GetUnitID());
}

void UnitActor::ActionQueueUpIdle(bool NoIdleJustDelay)
{
	Anim * anim = CreateAnim(UNITACTION_IDLE);

	if (anim == NULL)
	{
		anim = CreateAnim(UNITACTION_MOVE);
		Assert(anim != NULL);
	}

	if (anim && ((GetActionQueueNumItems() > 0) || NoIdleJustDelay))
	{
		anim->SetNoIdleJustDelay(TRUE);
	}

	Action *	tempCurAction	=
		new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT, 0, NoIdleJustDelay);

	tempCurAction->SetAnim(anim);




	m_actionQueue.Enqueue(tempCurAction);
}

void UnitActor::GetNextAction(bool isVisible)
{
	delete m_curAction;
	m_curAction = NULL;

	if (GetActionQueueNumItems() > 0) {

		m_actionQueue.Dequeue(m_curAction);

		Assert(m_curAction);
		if (!m_curAction)
		{
			return;
		}

		if (m_curAction->GetActionType() != m_curUnitAction)
			m_frame = 0;





	} else {
		return;
	}


	if(m_curAction->m_actionType == UNITACTION_ATTACK )
	{

		SetUnitVisibility(m_curAction->GetUnitsVisibility());
	}

















	m_curAction->SetSpecialDelayProcess(m_holdingCurAnimSpecialDelayProcess);

	MapPoint curStartMapPoint, curEndMapPoint;

	m_curAction->GetStartMapPoint(curStartMapPoint);
	m_curAction->GetEndMapPoint(curEndMapPoint);




	int i, j;
	if((j = i = m_curAction->GetNumOActors()) > 0)
	{
		i--;
		UnitActor **moveActors = m_curAction->GetMoveActors();
		for(; i>=0; i--)
		{
			if(moveActors[i] != NULL)
				moveActors[i]->SetHiddenUnderStack(TRUE);

			if(!m_isUnseenCellActor &&
				m_playerNum == g_selected_item->GetVisiblePlayer()) {
			}
		}
		m_curAction->SetNumOActors(0 - j);
	}

	if(m_playerNum == g_selected_item->GetVisiblePlayer() && m_curAction->GetActionType() == UNITACTION_MOVE)
	{

		if(!m_curAction->GetIsSpecialActionType())
		{
			if(m_isTransported == k_TRANSPORTADDONLY)
			{

				m_isTransported = FALSE;
			}
		}

		if(m_savedRevealedActors != NULL)
		{
			for (sint32 i=0; i<m_numSavedRevealedActors; i++) {
				UnitActor * tempActor = m_savedRevealedActors[i];
				Assert(tempActor != NULL);
				if (tempActor != NULL)
					tempActor->SetUnitVisibility(m_curAction->GetUnitsVisibility(), TRUE);
			}

			delete[] m_savedRevealedActors;
			m_savedRevealedActors = NULL;
			m_numSavedRevealedActors = 0;

		}





		UnitActor **revealedActors = m_curAction->GetRevealedActors();
		if (revealedActors) {
			for (sint32 i=0; i<m_curAction->GetNumRevealedActors(); i++) {
				UnitActor * tempActor = revealedActors[i];
				Assert(tempActor != NULL);
				if (tempActor != NULL)
					tempActor->SetUnitVisibility(m_curAction->GetUnitsVisibility(), TRUE);
			}

			SaveRevealedActors(revealedActors);

			m_numRevealedActors = 0;
			m_revealedActors = NULL;
			m_curAction->SetRevealedActors(NULL);
			m_curAction->SetNumRevealedActors(0);
		}
	}

	m_curUnitAction = (UNITACTION)m_curAction->GetActionType();
}

void UnitActor::Process(void)
{
	if (!m_curAction)
		GetNextAction();

	if (!m_curAction)
	{

		DumpFullLoad();

		AddIdle(m_facing != 3);




	}

	if (!m_curAction)
		return;

	m_curAction->Process();

	if (m_curAction->Finished())
	{
		if (m_curUnitAction==UNITACTION_MOVE)
		{
			MapPoint pos;

			m_curAction->GetEndMapPoint(pos);
			PositionActor(pos);
		}

		UnitActor **moveActors = m_curAction->GetMoveActors();

		if(moveActors != NULL)
		{

			sint32 num = abs(m_curAction->GetNumOActors());

			m_curAction->SetNumOActors(num);

			for (int i = num-1; i >= 0; i--)
			{
				if(moveActors[i] != NULL)
				   moveActors[i]->PositionActor(m_pos);

			}
			delete moveActors;
			m_curAction->SetMoveActors(NULL, NULL);
		}

		if (m_curAction->m_actionType != UNITACTION_IDLE &&
			m_curAction->m_actionType != UNITACTION_FACE_OFF)
			g_director->ActionFinished(m_curAction->GetSequence());

		if((m_curAction->m_actionType == UNITACTION_VICTORY && HasDeath())
			||
			m_curAction->m_actionType == UNITACTION_FAKE_DEATH) {
			SetKillNow();
			delete m_curAction;
			m_curAction = NULL;
		} else
		{

			GetNextAction();
		}
	}
	else
	{
		if (m_curAction->GetPath()!=NULL)
		{

			POINT curPt = m_curAction->GetPosition();
			m_x = curPt.x;
			m_y = curPt.y;
		}
		else
		{

			sint32 x, y;
			maputils_MapXY2PixelXY(m_pos.x, m_pos.y, &x, &y);
			m_x = x;
			m_y = y;
		}


		if(m_curAction->GetActionType() == UNITACTION_MOVE || m_curAction->GetActionType() == UNITACTION_ATTACK)
		{
			m_lastMoveFacing = m_curAction->GetFacing();
		}


		if(m_curAction->SpecialDelayProcess()
			|| (m_curUnitAction == UNITACTION_IDLE
				&& m_unitSpriteGroup
				&& m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction) == NULL)) {
			m_facing = m_lastMoveFacing;
		} else {

			m_facing = m_curAction->GetFacing();
		}

		m_frame = m_curAction->GetSpriteFrame();

		m_transparency = m_curAction->GetTransparency();
	}

}

Action *UnitActor::WillDie(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	sint32		type;

	if (m_curAction != NULL) {
		type = m_curAction->m_actionType;
		if (type == UNITACTION_VICTORY)
		{
			if (HasDeath())
				return m_curAction;
		}
		else if (type == UNITACTION_FAKE_DEATH)
		{
			return m_curAction;
		}
	}

	size_t  numItems = GetActionQueueNumItems();

	for (size_t i = 0; i < numItems; ++i)
    {
    	Action * action = NULL;
		m_actionQueue.GetQueueItem(i, action);
		if (action)
        {
			type = action->m_actionType;
			if (type == UNITACTION_VICTORY) {
				if (HasDeath())
					return action;
			}
			else if (type == UNITACTION_FAKE_DEATH)
			{
				return action;
			}
		}
	}

	return NULL;
}

Action *UnitActor::WillMorph(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (m_curAction && (UNITACTION_MORPH == m_curAction->m_actionType))
	{
		return m_curAction;
	}

	size_t numItems = GetActionQueueNumItems();
	for (size_t i = 0; i < numItems; ++i)
    {
        Action * action = NULL;
		m_actionQueue.GetQueueItem(i, action);
		if (action && (action->m_actionType == UNITACTION_MORPH))
		{
			return action;
		}
	}

	return NULL;
}

void UnitActor::DumpAllActions(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	static MapPoint pos;

	if (m_curAction != NULL) {
		m_facing = m_curAction->GetFacing();

		if (m_curAction->m_actionType == UNITACTION_MOVE) {
			m_curAction->GetEndMapPoint(pos);
			PositionActor(pos);
		}

		if (m_curAction->m_actionType != UNITACTION_IDLE &&
			m_curAction->m_actionType != UNITACTION_FACE_OFF) {
			g_director->ActionFinished(m_curAction->GetSequence());
		}
		delete m_curAction;
		m_curAction = NULL;
	}

	Action *deadAction=NULL;
	while (m_actionQueue.GetNumItems() > 0) {
		m_actionQueue.Dequeue(deadAction);
		if (deadAction != NULL) {
			m_facing = deadAction->GetFacing();

			if (deadAction->m_actionType == UNITACTION_MOVE) {
				deadAction->GetEndMapPoint(pos);
				PositionActor(pos);
			}

			if (deadAction->m_actionType != UNITACTION_IDLE &&
				deadAction->m_actionType != UNITACTION_FACE_OFF) {
				g_director->ActionFinished(deadAction->GetSequence());
			}

			delete deadAction;
			deadAction = NULL;
		} else {

			Assert(FALSE);
			return;
		}
	}
}








void UnitActor::EndTurnProcess(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	DumpAllActions();

	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, GetUnitID());

}

void UnitActor::AddAction(Action *actionObj)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return;

	Assert(actionObj != NULL);
	if (actionObj == NULL) return;






	if (g_theUnitPool) {
		if(g_theUnitPool->IsValid(GetUnitID()))
		{
			m_playerNum = Unit(GetUnitID()).GetOwner();
		}
	}

	m_actionQueue.Enqueue(actionObj);


	if (m_curAction) {
		if (m_curAction->GetCurrentEndCondition() == ACTIONEND_INTERRUPT) {
			m_curAction->SetFinished(TRUE);
		}
	} else {
		GetNextAction();
	}
}

Anim *UnitActor::CreateAnim(UNITACTION action)
{
#ifndef _TEST
	STOMPCHECK();
#endif
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
//			Assert(origAnim != NULL);
			if(origAnim == NULL)
				return NULL;
			else
				action = UNITACTION_MOVE;
		}
	}

	Anim * anim = new Anim(*origAnim);

	if (anim->GetType() == ANIMTYPE_LOOPED)
	{

		anim->SetDelayEnd(m_holdingCurAnimDelayEnd[action]);
		anim->SetElapsed(m_holdingCurAnimElapsed[action]);
		anim->SetLastFrameTime(g_director->GetMasterCurTime() - m_holdingCurAnimElapsed[action]);

		if(m_holdingCurAnimDelayEnd[action] != 0)
			anim->SetWeAreInDelay(TRUE);

	}

	if(action == UNITACTION_IDLE)
	{
		srand(anim->GetDelay() + g_director->GetMasterCurTime());
		anim->AdjustDelay(rand() % 2000);
	}

	return anim; // Has to be deleted outside.
}

#define k_FAKE_DEATH_FRAMES			15
#define k_FAKE_DEATH_DURATION		1500

Anim * UnitActor::MakeFakeDeath(void)
{
    uint16 *     frames          = new uint16[k_FAKE_DEATH_FRAMES];
    std::fill(frames, frames + k_FAKE_DEATH_FRAMES, 0);

    POINT        pt              = {0,0};
    POINT *      moveDeltas      = new POINT[k_FAKE_DEATH_FRAMES];
    std::fill(moveDeltas, moveDeltas + k_FAKE_DEATH_FRAMES, pt);

    uint16 *     transparencies  = new uint16[k_FAKE_DEATH_FRAMES];
    for (int i = 0; i < k_FAKE_DEATH_FRAMES; i++)
    {
        transparencies[i] = (uint16)(15 - i);
    }

    Anim *       anim            = new Anim();
    anim->SetNumFrames(k_FAKE_DEATH_FRAMES);
    anim->SetFrames(frames);
    anim->SetPlaybackTime(k_FAKE_DEATH_DURATION);
    anim->SetDeltas(moveDeltas);
    anim->SetTransparencies(transparencies);
    anim->SetType(ANIMTYPE_SEQUENTIAL);

    return anim;
}

#define k_FACEOFF_FRAMES			1
#define k_FACEOFF_DURATION			1000

Anim *UnitActor::MakeFaceoff(void)
{
    uint16 *     frames          = new uint16[k_FACEOFF_FRAMES];
    std::fill(frames, frames + k_FACEOFF_FRAMES, 0);

    POINT        pt              = {0,0};
    POINT *      moveDeltas      = new POINT[k_FACEOFF_FRAMES];
    std::fill(moveDeltas, moveDeltas + k_FACEOFF_FRAMES, pt);

    uint16 *     transparencies  = new uint16[k_FACEOFF_FRAMES];
    std::fill(transparencies, transparencies + k_FACEOFF_FRAMES, 15);

    Anim *       anim            = new Anim();
    anim->SetNumFrames(k_FACEOFF_FRAMES);
    anim->SetFrames(frames);
    anim->SetPlaybackTime(k_FACEOFF_DURATION);
    anim->SetDeltas(moveDeltas);
    anim->SetTransparencies(transparencies);
    anim->SetType(ANIMTYPE_LOOPED);

    return anim;
}

void UnitActor::DrawFortified(bool fogged)
{
	sint32	    nudgeX      = (sint32)((k_ACTOR_CENTER_OFFSET_X - 48) * g_tiledMap->GetScale());
	sint32      nudgeY      = (sint32)((k_ACTOR_CENTER_OFFSET_Y - 48) * g_tiledMap->GetScale());
	sint32      surfWidth   = g_screenManager->GetSurfWidth();
	sint32      surfHeight  = g_screenManager->GetSurfHeight();

	if ((m_x + nudgeX) > (surfWidth - g_tiledMap->GetZoomTilePixelWidth()))
		return;

	if ((m_y + nudgeY) > (surfHeight - g_tiledMap->GetZoomTilePixelHeight()))
		return;

	Pixel16	*   fortifiedImage = g_tiledMap->GetTileSet()->GetImprovementData(34);

	if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST) {

		if (fogged)
			g_tiledMap->DrawBlendedOverlayIntoMix(fortifiedImage, m_x + nudgeX, m_y + nudgeY, k_FOW_COLOR, k_FOW_BLEND_VALUE);
		else
			g_tiledMap->DrawColorizedOverlayIntoMix(fortifiedImage, m_x + nudgeX, m_y + nudgeY, 0x0000);
	} else {

		if (fogged)
			g_tiledMap->DrawBlendedOverlayScaledIntoMix(fortifiedImage, m_x + nudgeX, m_y + nudgeY,
												g_tiledMap->GetZoomTilePixelWidth(),
												g_tiledMap->GetZoomTileGridHeight(), k_FOW_COLOR, k_FOW_BLEND_VALUE);
		else
			g_tiledMap->DrawScaledOverlayIntoMix(fortifiedImage, m_x + nudgeX, m_y + nudgeY,
												g_tiledMap->GetZoomTilePixelWidth(),
												g_tiledMap->GetZoomTileGridHeight());
	}
}

void UnitActor::DrawFortifying(bool fogged)
{
	aui_BitmapFont	*font = g_tiledMap->GetFont();
	if (!font) return;

	sint32	x = m_x + (sint32)(double)(k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale()),
			y = m_y + (sint32)(double)(k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	MBCHAR		*fString = g_tiledMap->GetFortifyString();

	sint32 width = font->GetStringWidth(fString);
	sint32 height = font->GetMaxHeight();

	RECT		rect        = {0, 0, width, height};
	OffsetRect(&rect, x - width/2, y - height/2);
	RECT		clipRect    = rect;

    if (clipRect.left < 0) clipRect.left = 0;
	if (clipRect.top < 0) clipRect.top = 0;
	if (clipRect.right >= g_screenManager->GetSurfWidth())
		clipRect.right = g_screenManager->GetSurfWidth() - 1;
	if (clipRect.bottom >= g_screenManager->GetSurfHeight())
		clipRect.bottom = g_screenManager->GetSurfHeight() - 1;

	COLORREF colorRef = g_colorSet->GetColorRef(COLOR_BLACK);

	font->DrawString(g_screenManager->GetSurface(), &rect, &clipRect, fString,
		0,
		colorRef,
		0);

	if (fogged) {
		colorRef = g_colorSet->GetColorRef(COLOR_WHITE);
	} else {
		colorRef = g_colorSet->GetDarkColorRef(COLOR_WHITE);
	}

	OffsetRect(&rect, -1, -1);
	OffsetRect(&clipRect, -1, -1);

	font->DrawString(g_screenManager->GetSurface(), &rect, &clipRect, fString,
		0,
		colorRef,
		0);
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
void UnitActor::DrawCityWalls(bool fogged)   //TODO make a draw wonders and draw buildings method
{
	TileSet const *	tileSet		= g_tiledMap->GetTileSet();
	Pixel16 *		cityImage	= tileSet->GetImprovementData(38);	// default
	Unit			unit(GetUnitID());

	// Test city style overrides.
	CityStyleRecord const *	styleRec	=
		g_theCityStyleDB->Get(unit.CD()->GetCityStyle());

	if (styleRec)
	{
		AgeCityStyleRecord const *	ageStyleRec =
			styleRec->GetAgeStyle(g_player[unit->GetOwner()]->m_age);

		if (ageStyleRec)
		{
			bool const		isWater			= g_theWorld->IsWater(m_pos);
			sint32 const	spriteCount		= ageStyleRec->GetNumSprites();
			AgeCityStyleRecord::SizeSprite const *
							matchingSprite	= NULL;

			for (sint32 i = 0; i < spriteCount; ++i)
			{
				AgeCityStyleRecord::SizeSprite const *	spr	=
					ageStyleRec->GetSprites(i);

				if (spr && (isWater == (CITY_TYPE_WATER == spr->GetType())))
				{
					matchingSprite = spr;

					// Check city size
					sint32 p;
					unit.CD()->GetPop(p);
					if ((spr->GetMinSize() <= p) && (spr->GetMaxSize() >= p))
					{
						break;	// exact match found
					}

					// When no exact match has been found, the last (largest?)
					// of the correct type will be used.
				}
			}

			if (matchingSprite)
			{
				cityImage = tileSet->GetImprovementData(static_cast<uint16>(matchingSprite->GetWalls()));
			}
			// else: keep default
		}
	}
	// else: keep default

	sint32	nudgeX = (sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale()),
			nudgeY = (sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());

	if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST) {

		if (fogged)
			g_tiledMap->DrawBlendedOverlayIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY, k_FOW_COLOR, k_FOW_BLEND_VALUE);
		else
			g_tiledMap->DrawColorizedOverlayIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY, 0x0000);
	} else {

		if (fogged)
			g_tiledMap->DrawBlendedOverlayScaledIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY,
											g_tiledMap->GetZoomTilePixelWidth(),
											g_tiledMap->GetZoomTileGridHeight(), k_FOW_COLOR, k_FOW_BLEND_VALUE);
		else
			g_tiledMap->DrawScaledOverlayIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY,
											g_tiledMap->GetZoomTilePixelWidth(),
											g_tiledMap->GetZoomTileGridHeight());
	}
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
void UnitActor::DrawForceField(bool fogged)
{
	sint32 const	nudgeX	=
		(sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale());
	sint32 const	nudgeY =
		(sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());

	// Default sprite index (fixed number from original code)
	sint32 which;
	if (g_theWorld->IsLand(m_pos))
	{
		which = 154;
	}
	else if (g_theWorld->IsWater(m_pos))
	{
		which = 156;
	}
	else
	{
		which = 155;	// space?
	}

	Unit			unit(GetUnitID());

	// Test city style overrides.
	CityStyleRecord const *	styleRec	=
		g_theCityStyleDB->Get(unit.CD()->GetCityStyle());

	if (styleRec)
	{
		AgeCityStyleRecord const *	ageStyleRec =
			styleRec->GetAgeStyle(g_player[unit->GetOwner()]->m_age);

		if (ageStyleRec)
		{
			bool const		isWater			= g_theWorld->IsWater(m_pos);
			sint32 const	spriteCount		= ageStyleRec->GetNumSprites();
			AgeCityStyleRecord::SizeSprite const *
							matchingSprite	= NULL;

			for (sint32 i = 0; i < spriteCount; ++i)
			{
				AgeCityStyleRecord::SizeSprite const *	spr	=
					ageStyleRec->GetSprites(i);

				if (spr && (isWater == (CITY_TYPE_WATER == spr->GetType())))
				{
					matchingSprite = spr;

					// Check city size
					sint32 p;
					unit.CD()->GetPop(p);
					if ((spr->GetMinSize() <= p) && (spr->GetMaxSize() >= p))
					{
						break;	// exact match found
					}

					// When no exact match has been found, the last (largest?)
					// of the correct type will be used.
				}
			}

			if (matchingSprite)
			{
				which = matchingSprite->GetForceField();
			}
			// else: keep default
		}
	}
	// else: keep default

	Pixel16 *cityImage = g_tiledMap->GetTileSet()->GetImprovementData((uint16)which);

	if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST) {
	    g_tiledMap->DrawDitheredOverlayIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY, fogged);
	    }
	else {
	    g_tiledMap->DrawDitheredOverlayScaledIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY,
		g_tiledMap->GetZoomTilePixelWidth(),
		g_tiledMap->GetZoomTileGridHeight(),
		fogged);
	    }
}

bool UnitActor::Draw(bool fogged)
{
	if (m_hidden)
		return false;

	if (m_hiddenUnderStack)
		return false;

	sint32  xoffset = (sint32)(k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32  yoffset = (sint32)(k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	uint16  flags   = k_DRAWFLAGS_NORMAL;
	if (m_transparency < 15)
	{
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
	}
	if (fogged)
	{
		flags |= k_BIT_DRAWFLAGS_FOGGED;
	}

	bool isCloaked = false;

	if (m_unitID.IsValid())
	{
		if (m_unitID.IsAsleep())
		{
			flags |= k_BIT_DRAWFLAGS_DESATURATED;
			m_isFortified = FALSE;
			m_isFortifying = FALSE;
		}

		isCloaked = m_unitID.IsCloaked();
	}

	bool    directionAttack = m_directionalAttack && (m_curUnitAction == UNITACTION_ATTACK);

	SELECT_TYPE		selectType;
	ID				selectedID;
	PLAYER_INDEX	selectedPlayer;
	g_selected_item->GetTopCurItem(selectedPlayer, selectedID, selectType);

	Unit            selectedUnit;
	if (selectType == SELECT_TYPE_LOCAL_CITY)
	{
		selectedUnit = selectedID;
	}
	else if (selectType == SELECT_TYPE_LOCAL_ARMY)
	{
		selectedUnit = Army(selectedID).GetTopVisibleUnit(selectedPlayer);
	}

	bool	drawShield              = true;
	bool	drawSelectionBrackets   = false;

	if (selectedUnit.IsValid() && selectedUnit.GetActor() == this)
	{
		drawSelectionBrackets = true;
		if (GetTickCount() > m_shieldFlashOffTime)
		{
			drawShield  = GetTickCount() > m_shieldFlashOnTime;
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
		&& (IsFortified() == m_oldIsFortified)
		&& (IsFortifying() == m_oldIsFortifying)
		&& (HasForceField() == m_oldHasForceField)
		&& (HasCityWalls() == m_oldHasCityWalls)
		&& (drawShield == m_oldDrawShield)
		)
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
	m_oldFacing = m_facing;
	m_oldOffsetX = m_x + xoffset;
	m_oldOffsetY = m_y + yoffset;

	m_oldFlags = flags;
	m_oldIsFortified = IsFortified();
	m_oldIsFortifying = IsFortifying();
	m_oldHasForceField = HasForceField();
	m_oldHasCityWalls = HasCityWalls();
	m_oldDrawShield = drawShield;
	m_oldDrawSelectionBrackets = drawSelectionBrackets;

#endif

	if (m_unitSpriteGroup && m_unitID.IsValid())
	{
		if (m_unitID.IsValid() && m_unitID.IsCity()) //emod - 3-10-2007
			DrawCityImprovements(fogged);

		if (IsFortified())
			DrawFortified(fogged);

		if (IsFortifying()) {
			DrawFortifying(fogged);
		}

		if (HasCityWalls()) {
			DrawCityWalls(fogged);
		}

		uint16 oldTransparency = 0;
		if (isCloaked) {
			oldTransparency = m_transparency;
			m_transparency = static_cast<uint16>(8 + (rand() % 5));
			flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;
		}

		Pixel16 color   = 0x0000;
		if(m_curAction == NULL)
		{

			m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing,
									g_tiledMap->GetScale(), m_transparency, color, flags, FALSE, directionAttack);
		}
		else
		{
			m_unitSpriteGroup->Draw(m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing,
									g_tiledMap->GetScale(), m_transparency, color, flags, m_curAction->SpecialDelayProcess(), directionAttack);
		}

		if (isCloaked)
			m_transparency = oldTransparency;

		bool forcefieldsEverywhere = false;

		if (g_player[m_playerNum]) {
			if (wonderutil_GetForcefieldEverywhere(g_player[m_playerNum]->m_builtWonders))
			{
				forcefieldsEverywhere = m_unitID.IsValid() && m_unitID.IsCity();
			}
		}

		if (HasForceField() || forcefieldsEverywhere) {
			DrawForceField(fogged);
		}

	// adding drawwonders and buildings using mapicons
	//mapicons will save tile file space and make it so modders don't have to add a new tilefile all the time
	// also to cut on visible wonders slic and my visible wonder code doesn't work to well
	// bool check is in the method
	//emod

	}

	if (drawSelectionBrackets)
		DrawSelectionBrackets();

	if (drawShield)
		DrawHealthBar();

	return true;
}

void UnitActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale)
{
	if (m_unitSpriteGroup)
    {
	    uint16			flags   = k_DRAWFLAGS_NORMAL;
	    Pixel16			color   = 0;
	    BOOL			directionAttack = FALSE;
	    sint32			xoffset = (sint32)(k_ACTOR_CENTER_OFFSET_X * scale);
	    sint32			yoffset = (sint32)(k_ACTOR_CENTER_OFFSET_Y * scale);
		m_unitSpriteGroup->DrawDirect(surf, m_curUnitAction, m_frame, x+xoffset, y+yoffset, m_facing,
									  scale, m_transparency, color, flags, FALSE, directionAttack);
	}
}

void UnitActor::DrawText(sint32 x, sint32 y, MBCHAR *unitText)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if(m_unitSpriteGroup) {
		m_unitSpriteGroup->DrawText(x, y, unitText);
	}
}

//it doesn't look like this is used. I did create a drawstackingindicator that is used - E
void UnitActor::DrawHerald(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if (m_hiddenUnderStack)
		return;

	if (!g_showHeralds) return;

	if (!m_unitSpriteGroup)
		return;

	if (!m_unitID.IsValid()) return;

	CellUnitList army;
	g_theWorld->GetArmy(m_unitID.RetPos(), army);

	MAPICON		icon = MAPICON_HERALD;
	if (army.Num() > 1 && army.Num() < 10)
	{
		icon = (MAPICON) ((sint32) MAPICON_HERALD2 + army.Num() - 2);
	}
	else if(army.Num() >= 10)
	{
		icon = (MAPICON) ((sint32) MAPICON_HERALD10 + (army.Num() - 10));
	}

	Pixel16     color   = g_colorSet->GetPlayerColor(m_playerNum);
	TileSet *   tileSet = g_tiledMap->GetTileSet();
	POINT       iconDim = tileSet->GetMapIconDimensions(icon);
	RECT	    rect    = {0, 0, iconDim.x+1, iconDim.y+1};

	if (m_x < 0 || m_x > g_screenManager->GetSurfWidth()-rect.right) return;
	if (m_y < 0 || m_y > g_screenManager->GetSurfHeight()-rect.bottom) return;

#if 0   // Unused
	POINT		*pt;
	if (m_curUnitAction == UNITACTION_IDLE && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE) == NULL) {
		pt = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
	} else {
		pt = m_unitSpriteGroup->GetShieldPoints(m_curUnitAction);
	}
#endif

	OffsetRect(&rect, m_x + 0  - iconDim.x/2, m_y + 0  - iconDim.y/2);

	g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(icon), rect.left, rect.top, color);
	g_tiledMap->AddDirtyRectToMix(rect);
}


void UnitActor::DrawHealthBar(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (m_size > 0) return;
	if (!g_showHeralds) return;

	if (m_unitID.IsValid() && m_unitID.IsCity())
	{
		return;
	}

	TileSet	*   tileSet = g_tiledMap->GetTileSet();
	Cell *      myCell = g_theWorld->GetCell(m_pos);

	sint32		stackSize = 1;
	if (m_tempStackSize != 0)
	{
		stackSize = m_tempStackSize;
	}
	else if (IsActive())
	{
		if (m_unitID.IsValid())
		{
			Army army = m_unitID.GetArmy();

			if (army.IsValid())
			{
				stackSize = army.Num();
			}
		}
	}
	else
	{
		CellUnitList *  unitList = myCell->UnitArmy();
		if (unitList)
		{
			stackSize = unitList->Num();
			for (sint32 i=0; i<unitList->Num(); i++)
			{
				Unit top;

				Army a      = Army(unitList->Access(i).GetArmy().m_id);
				if (a.IsValid())
				{
					top = a->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
				}

				if (!top.IsValid())
				{
					top.m_id = unitList->Access(i).m_id;
				}

				if (top.GetActor() && top.GetActor()->IsActive())
				{
					stackSize--;
				}
			}
		}
	}

	double  ratio;
	if (m_unitID.IsValid())
	{
		if (stackSize > 1 && myCell->GetNumUnits())
		{
			ratio = std::max<double>(0.0, myCell->UnitArmy()->GetAverageHealthPercentage());
		}
		else
		{
			if (m_healthPercent < 0)
			{
				ratio = std::max<double>(0.0, m_unitID.GetHP() / m_unitID->CalculateTotalHP());
			}
			else
			{
				ratio = 0.0;   //m_healthPercent;
			}
		}
	}
	else
	{
		ratio = std::max<double>(0.0, m_healthPercent);
	}

	POINT	iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);

	RECT	iconRect = {0, 0, iconDim.x, iconDim.y}; //	RECT	iconRect = {0, 0, iconDim.x, iconDim.y};  original
	//RECT	flagRect = {0, 0, iconDim.x, iconDim.y};  //added this to be the flag rect and set it at 0,0

	UNITACTION		unitAction = m_curUnitAction;
	if (m_unitSpriteGroup->GetGroupSprite((GAME_ACTION) unitAction) == NULL)
		unitAction = UNITACTION_IDLE;

	POINT *     shieldPoint;
	if (unitAction == UNITACTION_IDLE &&
	    m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE) == NULL
	   )
	{
		shieldPoint = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
		OffsetRect(&iconRect, m_x + (sint32)((double)(shieldPoint->x) * g_tiledMap->GetScale()),
							  m_y + (sint32)((double)(shieldPoint->y) * g_tiledMap->GetScale()));
	}
	else
	{
		if (m_unitSpriteGroup && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)unitAction) != NULL)
		{
			shieldPoint = m_unitSpriteGroup->GetShieldPoints(unitAction);
			OffsetRect(&iconRect, m_x + (sint32)((double)(shieldPoint->x) * g_tiledMap->GetScale()),
								  m_y + (sint32)((double)(shieldPoint->y) * g_tiledMap->GetScale()));
		}
		else
		{
			sint32 top = m_y;
			sint32 middle = m_x + (sint32)((k_TILE_PIXEL_WIDTH) * g_tiledMap->GetScale())/2;
			OffsetRect(&iconRect, middle - iconDim.x / 2, top - iconDim.y);
		}
	}

	if (iconRect.left < 0) return;
	if (iconRect.right >= g_screenManager->GetSurfWidth()) return;
	if (iconRect.top < 0) return;
	if (iconRect.bottom >= g_screenManager->GetSurfHeight()) return;

	// @ToDo: Cleanup this type mess
	sint32 x = iconRect.left;
	sint32 y = iconRect.top;
	DrawSpecialIndicators(x, y, stackSize);
	iconRect.left = x;
	iconRect.top  = y;
	RECT tempRect = iconRect;
	InflateRect(&tempRect, 2, 2);
	tempRect.top -= 8;

	g_tiledMap->AddDirtyRectToMix(tempRect);

	Pixel16		black = g_colorSet->GetColor(COLOR_BLACK);
	if (black == 0x0000)
		black = 0x0001;

	if (g_theProfileDB->GetShowEnemyHealth() ||
	    m_playerNum == g_selected_item->GetVisiblePlayer()
	   )
	{
		iconRect.bottom += 4;

		tagRECT healthBar = iconRect;

		if (healthBar.left < 0) return;
		if (healthBar.right >= g_screenManager->GetSurfWidth()) return;
		if (healthBar.top < 0) return;
		if (healthBar.bottom >= g_screenManager->GetSurfHeight()) return;

		primitives_FrameRect16(g_screenManager->GetSurface(), &healthBar, black);

		InflateRect(&healthBar, -1, -1);

		RECT    leftRect    = healthBar;
		RECT    rightRect   = healthBar;

		Pixel16 color       = g_colorSet->GetColor(COLOR_GREEN);

		if (ratio < 1.0) {

			leftRect.right = leftRect.left + (sint32)(ratio * (double)(healthBar.right-iconRect.left));
			rightRect.left = leftRect.right;

			if (ratio < 0.25) {
				color = g_colorSet->GetColor(COLOR_RED);
			} else
			if (ratio < 0.50) {
				color = g_colorSet->GetColor(COLOR_ORANGE);
			} else
			if (ratio < 0.75) {
				color = g_colorSet->GetColor(COLOR_YELLOW);
			}

			primitives_PaintRect16(g_screenManager->GetSurface(), &rightRect, black);
		}

		if(leftRect.left > leftRect.right)
		{
			RECT temprect = leftRect;
			leftRect.left = temprect.right;
			leftRect.right = temprect.left;
		}

		primitives_PaintRect16(g_screenManager->GetSurface(), &leftRect, color);

		iconRect.top = iconRect.bottom;
	}

	// @ToDo: Cleanup this type mess
	x = iconRect.left;
	y = iconRect.top;
	DrawStackingIndicator(x, y, stackSize);
	DrawIndicators(x, y, stackSize);
	iconRect.left = x;
	iconRect.top  = y;
}

//moved stacking indcators here because its actually called by the healthbar above.

void UnitActor::DrawStackingIndicator(sint32 &x, sint32 &y, sint32 stack)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (!g_showHeralds) return;
	if (x < 0) return;
	if (y < 0) return;

	TileSet	*   tileSet = g_tiledMap->GetTileSet();
	POINT	    iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);
	if (x >= g_screenManager->GetSurfWidth() - iconDim.x) return;
	if (y >= g_screenManager->GetSurfHeight() - iconDim.y) return;

	sint32      displayedOwner;
	if (    m_unitID.IsValid()
	     && m_unitID.IsHiddenNationality()
	     && (m_playerNum != g_selected_item->GetVisiblePlayer()) // You want to spot your own units
	   )
	{
		// Display unit as barbarians
		displayedOwner  = PLAYER_INDEX_VANDALS;
	}
	else
	{
		displayedOwner  = m_playerNum;
	}
	sint32 x2 = x;
	sint32 y2 = y + iconDim.y;
	sint32 w = iconDim.x;
	sint32 h = iconDim.y;
	Pixel16     displayedColor  = g_colorSet->GetPlayerColor(displayedOwner);

// Remove the next line when the scaling and centering of the text has been implemented
// properly - or you want to test its operation. Currently, the generated text looks too
// ugly to include it in a release.
#define USE_PREDEFINED_ICONS

#if defined(USE_PREDEFINED_ICONS)
	MAPICON		icon = MAPICON_HERALD; // default: plain icon

	if (stack > 1 && stack <= 9)
	{
		// single digit predefined icons
		icon = (MAPICON) ((sint32) MAPICON_HERALD2 + stack - 2);
	}
	else if (stack >= 10 && stack <= 12)
	{
		// double digits predefined icons
		icon = (MAPICON) ((sint32) MAPICON_HERALD10 + stack - 10);
	}

	g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(icon), x, y, displayedColor);
#else
	g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_HERALD), x, y, displayedColor);

	// Generate text
	MBCHAR strn[80];
	sprintf(strn, "%i", stack);

	/// @todo Scale and center text
	if (stack > 1 && stack <= 9)
	{
		// single digit
		DrawText(x + 5, y, strn);
	}
	else if (stack >= 10 && stack <= 12)
	{
		// double digits
		DrawText(x, y, strn);
	}
#endif

	g_tiledMap->AddDirtyToMix(x, y, w, h);

	// @ToDo clean the code so that the following is superflous
	x = x2;
	y = y2;
}

//emod - moved these from stacking indicators to separate indicators

void UnitActor::DrawIndicators(sint32 &x, sint32 &y, sint32 stack)
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (!g_showHeralds) return;
	if (x < 0) return;
	if (y < 0) return;

	TileSet *   tileSet = g_tiledMap->GetTileSet();
	POINT       iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);
	if (x >= g_screenManager->GetSurfWidth() - iconDim.x) return;
	if (y >= g_screenManager->GetSurfHeight() - iconDim.y) return;

	sint32      displayedOwner;
	if (    m_unitID.IsValid()
	     && m_unitID.IsHiddenNationality()
	     && (m_playerNum != g_selected_item->GetVisiblePlayer()) // You want to spot your own units
	   )
	{
		// Display unit as barbarians
		displayedOwner  = PLAYER_INDEX_VANDALS;
	}
	else
	{
		displayedOwner  = m_playerNum;
	}

	Pixel16     displayedColor  = g_colorSet->GetPlayerColor(displayedOwner);

	sint32 x2 = x;
	sint32 y2 = y;
	sint32 w  = 0;
	sint32 h  = 0;

	if(m_unitID.IsValid() && m_unitID->GetArmy().IsValid())
	{
		if(m_unitID->GetArmy()->Num() > 1)
		{
			if(y2 < g_screenManager->GetSurfHeight() - iconDim.y)
			{
				g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_ARMY), x2, y2, displayedColor);
				iconDim = tileSet->GetMapIconDimensions(MAPICON_ARMY);
				y2 += iconDim.y;
				h += iconDim.y;
				w = std::max<sint32>(w, iconDim.x);
			}
		}
		// Replace veteran icon with elite icon if an elite unit exists in army.
		if(m_unitID->GetArmy()->HasVeterans() && !m_unitID->GetArmy()->HasElite())
		{
			if(y2 < g_screenManager->GetSurfHeight() - iconDim.y)
			{
				g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_VETERAN), x2, y2, displayedColor);
				iconDim = tileSet->GetMapIconDimensions(MAPICON_VETERAN);
				y2 += iconDim.y;
				h += iconDim.y;
				w = std::max<sint32>(w, iconDim.x);
			}
		} else if(m_unitID->GetArmy()->HasElite())
		{
			if(y2 < g_screenManager->GetSurfHeight() - iconDim.y)
			{
				g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_ELITE), x2, y2, displayedColor);
				iconDim = tileSet->GetMapIconDimensions(MAPICON_ELITE);
				y2 += iconDim.y;
				h += iconDim.y;
				w = std::max<sint32>(w, iconDim.x);
			}
		}

		if(m_unitID->GetArmy()->HasCargo())
		{
			// Do not draw the cargo icon if enemy army is carrying only stealth.
			if (m_unitID->GetArmy()->HasCargoOnlyStealth()
				&& m_playerNum != g_selected_item->GetVisiblePlayer())
			{}
			// Draw it in all other cases.
			else
			{
				if(y2 < g_screenManager->GetSurfHeight() - iconDim.y)
				{
					g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_CARGO), x2, y2, displayedColor);
					iconDim = tileSet->GetMapIconDimensions(MAPICON_CARGO);
					y2 += iconDim.y;
					h += iconDim.y;
					w = std::max<sint32>(w, iconDim.x);
				}
			}
		}
	}

	g_tiledMap->AddDirtyToMix(x, y, w, h);

	// @ToDo clean the code so that the following is superflous
	x = x2;
	y = y2;
}

void UnitActor::DrawSpecialIndicators(sint32 &x, sint32 &y, sint32 stack) //identifier for religious unit or national flag
{
#ifndef _TEST
	STOMPCHECK();
#endif

	if (!g_showHeralds) return;
	if (x < 0) return;
	if (y < 0) return;

	TileSet	*   tileSet = g_tiledMap->GetTileSet();
	POINT	    iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);
	if (x >= g_screenManager->GetSurfWidth() - iconDim.x) return;
	if (y >= g_screenManager->GetSurfHeight() - iconDim.y) return;

	sint32      displayedOwner;
	if (    m_unitID.IsValid()
	     && m_unitID.IsHiddenNationality()
	     && (m_playerNum != g_selected_item->GetVisiblePlayer()) // You want to spot your own units
	   )
	{
		// Display unit as barbarians
		displayedOwner  = PLAYER_INDEX_VANDALS;
	}
	else
	{
		displayedOwner  = m_playerNum;
	}

	Pixel16     displayedColor  = g_colorSet->GetPlayerColor(displayedOwner);
	sint32 x2 = x;
	sint32 y2 = y + iconDim.y;
	sint32 w = iconDim.x;
	sint32 h = iconDim.y;

	//If religious unit it shows the religion icon else it shows the national flag - E Aug 27 2007

	sint32  religionicon = 0;
	if(m_unitID.IsValid()
	&& m_unitID.GetDBRec()->GetHasReligionIconIndex(religionicon)
	){
		sint32 xf = x; // + iconDim.x;
		g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(religionicon), xf, y, displayedColor);
	}
	else if(g_theProfileDB->IsCivFlags())
	{
		sint32 civ = -1;
		// Add civilization flags here - moved flags here and edited the
		// heralds to put numbers on national flags emod 2-21-2007
		if(g_player[displayedOwner] != NULL)
		{
			civ     = g_player[displayedOwner]->GetCivilisation()->GetCivilisation();
		}
		else
		{
			for
			(
			    PointerList<Player>::Walker walk(g_deadPlayer);
			    walk.IsValid();
			    walk.Next()
			)
			{
				Player * p = walk.GetObj();

				if(p)
				{
					Civilisation *civP = p->GetCivilisation();
					if(civP != NULL && g_theCivilisationPool->IsValid(*civP))
					{
						if(civP->GetOwner() == displayedOwner)
						{
							civ = civP->GetCivilisation();
						}
					}
				}
			}
		}

		sint32  civicon = 0;

		if (g_theCivilisationDB->Get(civ)->GetNationUnitFlagIndex(civicon) && civ > -1)
		{
			sint32 xf = x; // + iconDim.x;
			g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(civicon), xf, y, displayedColor);
		}
	}

	g_tiledMap->AddDirtyToMix(x, y, w, h);

	// @ToDo clean the code so that the following is superflous
	x = x2;
	y = y2;
}

//end emod

void UnitActor::DrawSelectionBrackets(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if(!m_unitSpriteGroup)
		return;

	RECT		rect;
	SetRect(&rect, 0, 0, 1, 1);

 	OffsetRect(&rect,	m_x + (sint32)(k_TILE_PIXEL_WIDTH * g_tiledMap->GetScale())/2,
						m_y + (sint32)(k_TILE_GRID_HEIGHT * g_tiledMap->GetScale())/2);

	InflateRect(&rect, 25, 25);

	g_tiledMap->AddDirtyRectToMix(rect);

	TileSet *   tileSet = g_tiledMap->GetTileSet();
	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_BRACKET1);

	rect.right -= (iconDim.x+1);
	rect.bottom -= (iconDim.y+1);

	Pixel16 * topLeft = tileSet->GetMapIconData(MAPICON_BRACKET1);
	Assert(topLeft); if (!topLeft) return;
	Pixel16 * topRight = tileSet->GetMapIconData(MAPICON_BRACKET2);
	Assert(topRight); if (!topRight) return;
	Pixel16 * botRight = tileSet->GetMapIconData(MAPICON_BRACKET3);
	Assert(botRight); if (!botRight) return;
	Pixel16 * botLeft = tileSet->GetMapIconData(MAPICON_BRACKET4);
	Assert(botLeft); if (!botLeft) return;

	COLOR color = COLOR_YELLOW;
	if (m_unitID.IsValid())
    {
		if (m_unitID.GetArmy().IsValid() && m_unitID.GetArmy().CanMove())
		{
			color = COLOR_GREEN;
		}
		else if (m_unitID.IsCity())
        {
			color = COLOR_RED;
		}
	}
	Pixel16	pixelColor = g_colorSet->GetColor(color);

	g_tiledMap->DrawColorizedOverlayIntoMix(topLeft, rect.left, rect.top, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(topRight, rect.right, rect.top, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(botRight, rect.right, rect.bottom, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(botLeft, rect.left, rect.bottom, pixelColor);
}

bool UnitActor::IsAnimating(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
    return m_curAction && (m_curAction->GetActionType() != UNITACTION_IDLE);
}

uint16 UnitActor::GetWidth(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite *    theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);

	if (!theSprite && (m_curUnitAction == UNITACTION_IDLE))
	{
		theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_MOVE);
	}

    return theSprite ? theSprite->GetWidth() : 0;
}

uint16 UnitActor::GetHeight(void) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(m_unitSpriteGroup != NULL);
	if (m_unitSpriteGroup == NULL) return 0;

	Sprite * theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction);

    if (!theSprite && (m_curUnitAction == UNITACTION_IDLE))
	{
		theSprite = m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_MOVE);
	}

    return theSprite ? theSprite->GetHeight() : 0;
}

void UnitActor::GetBoundingRect(RECT *rect) const
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(rect != NULL);
	if (rect == NULL) return;

	if(!m_unitSpriteGroup)
		return;

	POINT	hotPoint = m_unitSpriteGroup->GetHotPoint(m_curUnitAction, m_facing);
	double	scale = g_tiledMap->GetScale();

	sint32 x = m_x;
	if (m_facing >= 5) {
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
		EndTurnProcess();

		if (m_unitID.IsValid() && m_unitID.IsCity())
		{
			m_unitID.GetPop(m_size);
		}

		archive << m_facing;
		archive.PutUINT8((uint8)m_isFortified); // was not saved before #244 and is only saved here to fill the fromer 4 bytes, i.e. slot could be reused
		archive.PutUINT8((uint8)m_isFortifying); // was not saved before #244 and is only saved here to fill the fromer 4 bytes, i.e. slot could be reused
		archive.PutUINT8((uint8)m_hasCityWalls); // without saving, known city walls are not drawn after a game reload (bad for sending armies, slavers)
		archive.PutUINT8((uint8)m_hasForceField); // without saving, known force fields are not drawn after a game reload (bad for sending armies)
		archive << m_size;
		archive.PutUINT8((uint8)m_isUnseenCellActor);

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
		archive << m_unitVisionRange;
		archive << m_unitVisibility;

		m_pos.Serialize(archive);
		m_spriteState->Serialize(archive);
	}
	else
	{
		archive >> m_facing;
		m_isFortified = (BOOL)archive.GetUINT8();
		m_isFortifying = (BOOL)archive.GetUINT8();
		m_hasCityWalls = (BOOL)archive.GetUINT8();
		m_hasForceField = (BOOL)archive.GetUINT8();
		archive >> m_size;

		m_isUnseenCellActor = (BOOL)archive.GetUINT8();

		m_type = (GROUPTYPE)archive.GetUINT8();

		if (g_saveFileVersion >= 67)
		{
			archive >> m_spriteID;
		}
		else
		{
			m_spriteID = (sint32)archive.GetUINT8();
		}

		m_playerNum = (sint32)archive.GetUINT8();
		m_unitID    = Unit(archive.GetUINT32());

		if (g_saveFileVersion >= 67)
		{
			archive >> m_unitDBIndex;
		}
		else
		{
			m_unitDBIndex = (sint32)archive.GetUINT8();
		}

		archive >> m_unitVisionRange;
		archive >> m_unitVisibility;

		m_pos.Serialize(archive);
		delete m_spriteState;
		m_spriteState = new SpriteState(archive);
	}
}

LOADTYPE UnitActor::GetLoadType(void) const
{
    return (m_unitSpriteGroup) ? m_loadType : LOADTYPE_NONE;
}




















void UnitActor::FullLoad(UNITACTION action)
{

	if (!g_theProfileDB->IsUnitAnim())
		return;

	if (!m_unitSpriteGroup)
		return;
	if (m_loadType == LOADTYPE_FULL)
		return;







	SpriteGroup *group = g_unitSpriteGroupList->GetSprite(m_spriteID, m_unitSpriteGroup->GetType(), LOADTYPE_FULL,(GAME_ACTION)action);

	m_loadType = LOADTYPE_FULL;

	Assert(group == m_unitSpriteGroup);
}




bool UnitActor::ActionMove(Action *actionObj)
{
	Assert(actionObj != NULL);

	if (actionObj == NULL)
		return false;

	if(GetNeedsToDie())
	   return false;

	SetIsFortifying(FALSE);
	SetIsFortified (FALSE);

	actionObj->SetActionType(UNITACTION_MOVE);
	actionObj->SetAnimPos(GetHoldingCurAnimPos(UNITACTION_MOVE));
	actionObj->SetSpecialDelayProcess(GetHoldingCurAnimSpecialDelayProcess(UNITACTION_MOVE));
	actionObj->SetCurrentEndCondition(ACTIONEND_PATHEND);

	if (GetLoadType()!=LOADTYPE_FULL)
	 	FullLoad(UNITACTION_MOVE);

	Anim * anim	= CreateAnim(UNITACTION_MOVE);
	Assert(anim != NULL);
   	if (anim == NULL)
   		return false;

	actionObj->SetAnim(anim);
	actionObj->SetUnitsVisibility(GetUnitVisibility());
	actionObj->SetUnitVisionRange(GetUnitVisionRange());
	actionObj->SetMaxActionCounter
        (k_MAX_UNIT_MOVEMENT_ITERATIONS - g_theProfileDB->GetUnitSpeed());
	actionObj->SetCurActionCounter(0);

	AddAction(actionObj);

	if (GetIsTransported() == k_TRANSPORTREMOVEONLY)
    {
		TerminateLoopingSound(SOUNDTYPE_SFX);
    }
	else
	{
	    sint32 const visiblePlayer = g_selected_item->GetVisiblePlayer();

		if ((visiblePlayer == GetPlayerNum()) ||
			(GetUnitVisibility() & (1 << visiblePlayer)))
        {
			AddLoopingSound(SOUNDTYPE_SFX,actionObj->GetSoundEffect());
        }
   	}

	return true;
}


bool UnitActor::ActionAttack(Action *actionObj,sint32 facing)
{
	Assert(actionObj != NULL);

	if (actionObj == NULL)
		return false;

	if(GetNeedsToDie())
	   return false;

	actionObj->SetCurrentEndCondition(ACTIONEND_ANIMEND);

   	if(!TryAnimation(actionObj,UNITACTION_ATTACK))
	   if(!TryAnimation(actionObj,UNITACTION_IDLE))
	 	 return false;

	actionObj->SetActionType(UNITACTION_ATTACK);
	actionObj->SetFacing(facing);
	actionObj->SetUnitsVisibility(GetUnitVisibility());
	actionObj->SetUnitVisionRange(GetUnitVisionRange());

	AddAction(actionObj);

	TerminateLoopingSound(SOUNDTYPE_SFX);

	sint32 const    visiblePlayer = g_selected_item->GetVisiblePlayer();

    if ((visiblePlayer == GetPlayerNum()) || (GetUnitVisibility() & (1 << visiblePlayer)))
		  AddSound(SOUNDTYPE_SFX,actionObj->GetSoundEffect());

	return true;
}

bool UnitActor::ActionSpecialAttack(Action *actionObj,sint32 facing)
{
	Assert(actionObj != NULL);

	if (actionObj==NULL)
		return false;

	if(GetNeedsToDie())
	   return false;

	actionObj->SetCurrentEndCondition(ACTIONEND_ANIMEND);

	if(!TryAnimation(actionObj,UNITACTION_WORK))
	   if(!TryAnimation(actionObj,UNITACTION_ATTACK))
		  if(!TryAnimation(actionObj,UNITACTION_IDLE))
			 return false;

	actionObj->SetActionType(UNITACTION_ATTACK);
	actionObj->SetFacing(facing);
	actionObj->SetUnitsVisibility(GetUnitVisibility());
	actionObj->SetUnitVisionRange(GetUnitVisionRange());

	AddAction(actionObj);

	TerminateLoopingSound(SOUNDTYPE_SFX);

	sint32 const    visiblePlayer = g_selected_item->GetVisiblePlayer();

	if ((visiblePlayer == GetPlayerNum()) || (GetUnitVisibility() & (1 << visiblePlayer)))
		AddSound(SOUNDTYPE_SFX, actionObj->GetSoundEffect());

	return true;
}

bool UnitActor::TryAnimation(Action *actionObj,UNITACTION action)
{
	FullLoad(action);

	Anim * theAnim = CreateAnim(action); // theAnim must be deleted
	if (theAnim)
	{
        actionObj->SetAnimPos(GetHoldingCurAnimPos(action));
        actionObj->SetSpecialDelayProcess(GetHoldingCurAnimSpecialDelayProcess(action));
        actionObj->SetAnim(theAnim);
        return true;
	}

	return false;
}

void UnitActor::DumpFullLoad(void)
{
	if (!m_unitSpriteGroup) return;
	if (m_loadType != LOADTYPE_FULL) return;

	bool purged = g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_FULL);

	if (purged) {
		m_unitSpriteGroup = NULL;
	} else {
		m_loadType = LOADTYPE_BASIC;
	}
}

BOOL UnitActor::HitTest(POINT mousePt)
{
	bool    isDirectionAttack   = m_directionalAttack && (m_curUnitAction == UNITACTION_ATTACK);
	sint32	xoffset             = (sint32)(k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32	yoffset             = (sint32)(k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());
    Pixel16 color               = COLOR_WHITE;
	uint16	flags               = 0;
    bool    isSpecialDelay      = m_curAction && m_curAction->SpecialDelayProcess();

	return m_unitSpriteGroup->HitTest
                (mousePt, m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset,
                 m_facing, g_tiledMap->GetScale(), m_transparency, color, flags,
                 isSpecialDelay, isDirectionAttack
                );
}





void UnitActor::AddLoopingSound(uint32 sound_type, sint32 sound_id)
{
	if ((g_soundManager)&&(sound_id>=0))
	   	g_soundManager->AddLoopingSound((SOUNDTYPE)sound_type,
										(uint32)GetUnitID(),sound_id,
										GetPos().x, GetPos().y);

}

void UnitActor::AddSound(uint32 sound_type, sint32 sound_id)
{
	if ((g_soundManager)&&(sound_id>=0))
	   	g_soundManager->AddSound((SOUNDTYPE)sound_type,
								 (uint32)GetUnitID(),sound_id,
								 GetPos().x, GetPos().y);
}

void UnitActor::TerminateLoopingSound(uint32 sound_type)
{
	if (g_soundManager)
 		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,GetUnitID());
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
void UnitActor::DrawCityImprovements(bool fogged)
{
	TileSet	*   tileSet = g_tiledMap->GetTileSet();

	sint32	nudgeX = (sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale());
	sint32 	nudgeY = (sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());

	POINT	    iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);
	if ((m_x + nudgeX) >= g_screenManager->GetSurfWidth() - iconDim.x) return;
	if ((m_y + nudgeY) >= g_screenManager->GetSurfHeight() - iconDim.y) return;

	Unit	unit(m_unitID);
	sint32  cityIcon = 0;
	if (unit.IsValid() && unit.IsCity()) {
	for(sint32 b = 0; b < g_theBuildingDB->NumRecords(); b++)
	{
		if(buildingutil_Get(b, m_playerNum)->GetShowCityIconBottomIndex(cityIcon))
		{
			if(unit.CD()->GetImprovements() & ((uint64)1 << b))
			{
				if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST)
				{
					if (fogged)
						g_tiledMap->DrawBlendedOverlayIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY, k_FOW_COLOR, k_FOW_BLEND_VALUE);
					else
							g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY, 0x0000);
				}
				else
				{
					if (fogged)
						g_tiledMap->DrawBlendedOverlayScaledIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY,
								g_tiledMap->GetZoomTilePixelWidth(),
								g_tiledMap->GetZoomTileGridHeight(), k_FOW_COLOR, k_FOW_BLEND_VALUE);
					else
						g_tiledMap->DrawScaledOverlayIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY,
								g_tiledMap->GetZoomTilePixelWidth(),
								g_tiledMap->GetZoomTileGridHeight());
				}
				nudgeX += 5;
			}
		}
	}

	for(sint32 i=0; i<g_theWonderDB->NumRecords(); i++)
	{
		if(wonderutil_Get(i, m_playerNum)->GetShowCityIconBottomIndex(cityIcon))
		{
			if(unit.CD()->GetBuiltWonders() & (uint64)1 << (uint64)i)
			{
				if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST)
				{
					if (fogged)
						g_tiledMap->DrawBlendedOverlayIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY, k_FOW_COLOR, k_FOW_BLEND_VALUE);
					else
							g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY, 0x0000);
				}
				else
				{
					if (fogged)
						g_tiledMap->DrawBlendedOverlayScaledIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY,
								g_tiledMap->GetZoomTilePixelWidth(),
								g_tiledMap->GetZoomTileGridHeight(), k_FOW_COLOR, k_FOW_BLEND_VALUE);
					else
						g_tiledMap->DrawScaledOverlayIntoMix(tileSet->GetMapIconData(cityIcon), m_x + nudgeX, m_y + nudgeY,
								g_tiledMap->GetZoomTilePixelWidth(),
								g_tiledMap->GetZoomTileGridHeight());
				}
				nudgeX += 5;
			}
		}
	}
	}
}

#ifdef _DEBUG
void UnitActor::DumpActor(void)
{
	DPRINTF(k_DBG_UI, ("Actor %#.8lx\n", this));
	DPRINTF(k_DBG_UI, ("  m_unitID           :%#.8lx\n", m_unitID.m_id));
	DPRINTF(k_DBG_UI, ("  m_unitDBIndex      :%d\n", m_unitDBIndex));
	DPRINTF(k_DBG_UI, ("  m_curAction        :%#.8lx\n", m_curAction));

	if (m_curAction) {
		DPRINTF(k_DBG_UI, ("  m_curAction.m_actionType     :%ld\n", m_curAction->m_actionType));
		DPRINTF(k_DBG_UI, ("  m_curAction.m_finished       :%ld\n", m_curAction->Finished()));

		DPRINTF(k_DBG_UI, ("  m_curAction.m_sequence       :%#.8lx\n", m_curAction->GetSequence()));
		if (m_curAction->GetSequence()) {
			DPRINTF(k_DBG_UI, ("Actor %#.8lx m_curAction:\n", this));
			g_director->DumpSequence(m_curAction->GetSequence());
		}
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));

	DPRINTF(k_DBG_UI, ("  m_actionQueue         :%d\n", m_actionQueue.GetNumItems()));
	if (m_actionQueue.GetNumItems() > 0) {
		for (size_t i = 0; i < m_actionQueue.GetNumItems(); i++)
        {
		    Action * action = NULL;
			m_actionQueue.GetQueueItem(i, action);
			DPRINTF(k_DBG_UI, ("  m_actionQueue Item      :%u\n", i));

			if (action) {
				DPRINTF(k_DBG_UI, ("  action.m_actionType     :%ld\n", action->m_actionType));
				DPRINTF(k_DBG_UI, ("  action.m_finished       :%ld\n", action->Finished()));
				DPRINTF(k_DBG_UI, ("  action.m_sequence       :%#.8lx\n", action->GetSequence()));
				if (action->GetSequence()) {
					g_director->DumpSequence(action->GetSequence());
				}
			}
		}
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));
}
#endif
