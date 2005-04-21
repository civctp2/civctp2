//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Unit
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
// - Fixed number of city styles removed.
// - Prevented crashes due to uninitialised members.
// - Prevented some NULL-dereferencing crashes.
// - Exposed city walls and force field graphics to agecitystyle.txt, 
//   by Martin Gühmann.
// - Prevented crashes with invalid (i.e. killed or destroyed) units.
// - PFT 29 mar 05, show # turns until city next grows a pop.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui.h"
#include "aui_surface.h"
#include "aui_bitmapfont.h"

#include "pixelutils.h"
#include "tileutils.h"
#include "primitives.h"
#include "TileInfo.h"

#include "Unit.h"	
#include "UnitData.h"
#include "SelItem.h"
#include "XY_Coordinates.h"
#include "World.h"
#include "CivArchive.h"
#include "ScreenManager.h"
#include "Cell.h"

#include "FacedSprite.h"
#include "UnitSpriteGroup.h"
#include "SpriteState.h"
#include "Actor.h"
#include "SpriteGroupList.h"
#include "TiledMap.h"
#include "Anim.h"
#include "UnitActor.h"
#include "ActorPath.h"
#include "Action.h"
#include "Director.h"
#include "ColorSet.h"
#include "UnitPool.h"
#include "ArmyPool.h"
#include "maputils.h"
#include "TurnCnt.h"
#include "network.h"
#include "SpriteStateDB.h"
#include "AgeRecord.h"
#include "CellUnitList.h"
#include "WonderRecord.h"

#include "SoundManager.h"

#include "civilisation.h"
#include "civilisationdb.h"
#include "player.h"

#include "DebugMemory.h"
#include "UnitRecord.h"

#include "wonderutil.h"
#include "TerrainRecord.h"

#include "ArmyData.h"

#include "CityStyleRecord.h"
#include "AgeCityStyleRecord.h"

BOOL		g_showHeralds = TRUE;

#define k_doInvisible FALSE

#define k_SHIELD_ON_TIME		650
#define k_SHIELD_OFF_TIME		150

extern ColorSet			*g_colorSet;
extern SpriteGroupList	*g_unitSpriteGroupList;
extern SpriteGroupList	*g_citySpriteGroupList;
extern TiledMap			*g_tiledMap;
extern Director			*g_director;
extern SelectedItem		*g_selected_item;
extern UnitPool         *g_theUnitPool;
extern ArmyPool			*g_theArmyPool;
extern World			*g_theWorld;
extern ScreenManager	*g_screenManager;
extern SpriteStateDB	*g_theCitySpriteStateDB;
extern TurnCount		*g_turn;
extern SoundManager		*g_soundManager;

extern CivilisationDatabase	*g_theCivilisationDB;
extern Player			**g_player;

extern BOOL				g_unitCompletedAction;

#include "profiledb.h"
extern ProfileDB		*g_theProfileDB;

#ifndef _DEBUG_MEMORY
#define STOMPCHECK() if (m_curAction) { Assert(_CrtIsMemoryBlock(m_curAction, sizeof(Action),NULL,NULL,NULL));}
#else
#define STOMPCHECK() ;
#endif

namespace
{
	sint32 const	CITY_TYPE_LAND	= 0;
	sint32 const	CITY_TYPE_WATER	= 1;
};	// namespace

UnitActor::UnitActor(SpriteState *ss, Unit id, sint32 unitType, const MapPoint &pos, sint32 owner, BOOL isUnseenCellActor,
					 double visionRange, sint32 citySprite)
: Actor(ss)
{	
	sint32 spriteID;

	m_unitVisionRange = visionRange;
	m_unitVisibility = NULL;
	m_unitSaveVisibility = NULL;

	m_size = 0;
	m_nextPop = 0;//PFT 29 mar 05, show # turns until city next grows a pop
	GetIDAndType(owner, ss, id, unitType, (MapPoint)pos, &spriteID, &m_type);
	m_spriteID = (sint32)spriteID;

	m_loadType = LOADTYPE_NONE;

	
	
	if(m_spriteID < 1)
		m_spriteID = citySprite;
	Assert(m_spriteID >= 1);

	m_spriteState = ss;
	m_unitDBIndex = unitType;
	m_playerNum = owner;
	m_unitID = id;
	m_pos = pos;
	m_isUnseenCellActor = isUnseenCellActor;

	Initialize();

	
	m_facing = k_DEFAULTSPRITEFACING;
	m_lastMoveFacing = k_DEFAULTSPRITEFACING;

	m_refCount = 1;
}










UnitActor::UnitActor(CivArchive &archive)
{
	m_refCount = 1;

	m_unitVisibility = NULL;
	m_unitSaveVisibility = NULL;

	m_spriteState = NULL;

	Serialize(archive);
	Initialize();
}

void UnitActor::AddVision(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	BOOL revealedUnexplored = FALSE;
	
	if(!m_isUnseenCellActor && m_playerNum == g_selected_item->GetVisiblePlayer()) {
		g_tiledMap->GetLocalVision()->AddVisible(m_pos, m_unitVisionRange, revealedUnexplored);
	}
}

void UnitActor::RemoveVision(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	
	if(!m_isUnseenCellActor && m_playerNum == g_selected_item->GetVisiblePlayer()) {
		g_tiledMap->GetLocalVision()->RemoveVisible(m_pos,  m_unitVisionRange);
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

	
	sint32 xoff = 0,yoff = 0;
	


	SetX(pixelX - xoff);
	SetY(pixelY - yoff);
	SetPos(pos);

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
	} else 
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

	
	m_isFortifying = FALSE;
	m_isFortified = FALSE;
	m_hasCityWalls = FALSE;
	m_hasForceField = FALSE;

	m_shieldFlashOnTime = 0;
	m_shieldFlashOffTime = 0;

	m_activeListRef = 0;

	m_healthPercent = -1.0;

	m_tempStackSize = 0;

	AddIdle();
}

void UnitActor::GetIDAndType(sint32 owner, SpriteState *ss, Unit id, sint32 unitType, MapPoint &pos, 
								sint32 *spriteID, GROUPTYPE *groupType)
{
	BOOL		isCity;

	isCity = g_theUnitDB->Get(unitType)->GetHasPopAndCanBuild();

	if (isCity) {
		if (id.IsValid() && id.CD())
		{
			*spriteID = id.CD()->GetDesiredSpriteIndex();
		}
		else if (ss)
		{
			*spriteID = ss->GetIndex();
		}
		else
		{
			*spriteID = -1;
		}

		*groupType = GROUPTYPE_CITY;
	} else {
		*spriteID = ss->GetIndex();
		*groupType = GROUPTYPE_UNIT;
	}
}

UnitActor::~UnitActor()
{
	DumpAllActions();

	
	
	if (m_type == GROUPTYPE_UNIT) {
		if (g_unitSpriteGroupList->ReleaseSprite(m_spriteID, m_loadType))
			m_unitSpriteGroup = NULL;
	} else {
		if (g_citySpriteGroupList->ReleaseSprite(m_spriteID, m_loadType))
			m_unitSpriteGroup = NULL;
	}

	if(m_savedRevealedActors != NULL)
	{


		delete[] m_savedRevealedActors;
	}

	if(m_revealedActors != NULL)
	{


		delete[] m_revealedActors;
	}

	
	m_actionQueue.Deallocate();

	
	if (m_spriteState) delete m_spriteState;
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
	sint32			spriteID;
	GROUPTYPE		groupType;

	
	if (g_theUnitPool->IsValid(id) && id.IsCity()) {
		id.GetPop(m_size);//put the city's pop into the actor's m_size
		id.GetTurnsToNextPop(m_nextPop);//PFT, computes TurnsToNextPop and puts it into the actor's m_nextPop
	}

	DumpAllActions();

	
	
	if (m_type == GROUPTYPE_UNIT) {
		if (g_unitSpriteGroupList->ReleaseSprite(m_spriteID, m_loadType))
			m_unitSpriteGroup = NULL;
	} else {
		if (g_citySpriteGroupList->ReleaseSprite(m_spriteID, m_loadType))
			m_unitSpriteGroup = NULL;
	}

	GetIDAndType(m_playerNum, ss, id, type, m_pos, &spriteID, &groupType);

	m_type = groupType;

	if (spriteID == -1) {

#ifdef _DEBUG
		


#endif

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

	
	if(m_spriteState && m_spriteState != ss)
		delete m_spriteState;

	m_spriteState = ss;
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
			BOOL revealedUnexplored = FALSE;
			DPRINTF(k_DBG_INFO, ("Adding vision for %lx, owner %d, range %lf, center: %d,%d\n",
								 m_unitID, m_playerNum, m_unitVisionRange,
								 m_pos.x, m_pos.y));

		}
	}

	if(m_unitSpriteGroup) {
		m_directionalAttack = m_unitSpriteGroup->HasDirectional();
	} else {
		m_directionalAttack = FALSE;
	}

	
	if(g_theUnitPool->IsValid(id)) {
		id.SetSpriteState(ss);
	}

	DumpAllActions();
	AddIdle();
}

void UnitActor::AddIdle(BOOL NoIdleJustDelay)
{
	Anim		*anim;

	anim = GetAnim(UNITACTION_IDLE);
	m_frame = 0;

	
	if (anim == NULL) {
		anim = GetAnim(UNITACTION_MOVE);
		Assert(anim != NULL);
	}

	Action		*idleAction;

	if (anim && ((GetActionQueueNumItems() > 0) || NoIdleJustDelay))
	{
		anim->SetNoIdleJustDelay(TRUE);
	}


	if(NoIdleJustDelay == TRUE) {
		idleAction = new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT, 
								0, 
								TRUE);
		idleAction->SetFacing(m_facing);
	} else {
		idleAction = new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT); 
	}

	idleAction->SetAnim(anim);

	AddAction(idleAction);



	
	if (g_soundManager)
		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX, GetUnitID());
}

void UnitActor::ActionQueueUpIdle(BOOL NoIdleJustDelay)
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

	Action *	tempCurAction	= 
		new Action(UNITACTION_IDLE, ACTIONEND_INTERRUPT, 0, NoIdleJustDelay);

	tempCurAction->SetAnim(anim);

	

	
	m_actionQueue.Enqueue(tempCurAction);
}

void UnitActor::GetNextAction(BOOL isVisible)
{
	if (m_curAction) 
	{
		delete m_curAction;
		m_curAction = NULL;
	}

	
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
			UnitActor		*tempActor;
			
			for (sint32 i=0; i<m_numSavedRevealedActors; i++) {
				tempActor = m_savedRevealedActors[i];
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
			UnitActor		*tempActor;

			for (sint32 i=0; i<m_curAction->GetNumRevealedActors(); i++) {
				tempActor = revealedActors[i];
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
		sint32		i;

		
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

			for(i = num-1; i>=0; i--) 
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

		
		g_director->HandleNextAction();
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

Action *UnitActor::WillDie(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	sint32		numItems = GetActionQueueNumItems();
	sint32		i;
	Action		*action;
	sint32		type;

	if (m_curAction != NULL) {
		type = m_curAction->m_actionType;
		if (type == UNITACTION_VICTORY) {
			if (HasDeath())
				return m_curAction;
		} else {
			if (type == UNITACTION_FAKE_DEATH) {
				return m_curAction;
			}
		}
	}
	
	for (i=0; i<numItems; i++) {
		m_actionQueue.GetQueueItem(i, action);
		if (action) {
			type = action->m_actionType;
			if (type == UNITACTION_VICTORY) {
				if (HasDeath())
					return action;
			} else
				if (type == UNITACTION_FAKE_DEATH) {
					return action;
				}
		}
	}

	return NULL;
}

Action *UnitActor::WillMorph(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	sint32		numItems = GetActionQueueNumItems();
	sint32		i;
	Action		*action;
	sint32		type;

	if (m_curAction != NULL) {
		type = m_curAction->m_actionType;
		if (type == UNITACTION_MORPH) {
			return m_curAction;
		}
	}
	
	for (i=0; i<numItems; i++) {
		m_actionQueue.GetQueueItem(i, action);
		if (action) {
			if (action->m_actionType == UNITACTION_MORPH) {
					return action;
			}
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

Anim *UnitActor::GetAnim(UNITACTION action)
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

	if(anim->GetType() == ANIMTYPE_LOOPED)
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
	
	return anim;
}

#define k_FAKE_DEATH_FRAMES			15		
#define k_FAKE_DEATH_DURATION		1500	

Anim *UnitActor::MakeFakeDeath(void)
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








#define k_FACEOFF_FRAMES			1
#define k_FACEOFF_DURATION			1000
Anim *UnitActor::MakeFaceoff(void)
{
	sint32		i;
	Anim		*anim = new Anim();

	
	uint16 *frames = new uint16[k_FACEOFF_FRAMES];
	for (i=0; i<k_FACEOFF_FRAMES; i++) {
		frames[i] = 0;
	}

	
	POINT *moveDeltas = new POINT[k_FACEOFF_FRAMES];
	POINT pt = {0,0};
	for (i=0; i<k_FACEOFF_FRAMES; i++) {
		moveDeltas[i] = pt;
	}
	
	
	uint16 *transparencies = new uint16[k_FACEOFF_FRAMES];
	for (i=0; i<k_FACEOFF_FRAMES; i++) {
		transparencies[i] = 15;
	}

	anim->SetNumFrames(k_FACEOFF_FRAMES);
	anim->SetFrames(frames);
	anim->SetPlaybackTime(k_FACEOFF_DURATION);
	anim->SetDeltas(moveDeltas);
	anim->SetTransparencies(transparencies);

	anim->SetType(ANIMTYPE_LOOPED);

	return anim;
}

void UnitActor::DrawFortified(BOOL fogged)
{
	Pixel16			*fortifiedImage = g_tiledMap->GetTileSet()->GetImprovementData(34);

	sint32	nudgeX = (sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale()), 
			nudgeY = (sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());

	
	sint32	xoff = 0,yoff = 0;



	nudgeX -= xoff;
	nudgeY -= yoff;

	
	sint32 surfWidth = g_screenManager->GetSurfWidth();
	sint32 surfHeight = g_screenManager->GetSurfHeight();

	
	if ((m_x + nudgeX) > (surfWidth - g_tiledMap->GetZoomTilePixelWidth())) 
		return;

	if ((m_y + nudgeY) > (surfHeight - g_tiledMap->GetZoomTilePixelHeight()))
		return;

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

void UnitActor::DrawFortifying(BOOL fogged)
{
	Pixel16			*fortifiedImage = g_tiledMap->GetTileSet()->GetImprovementData(34);

	sint32	x = m_x + (sint32)(double)(k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale()), 
			y = m_y + (sint32)(double)(k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	aui_BitmapFont	*font = g_tiledMap->GetFont();
	if (!font) return;

	MBCHAR		*fString = g_tiledMap->GetFortifyString();

	sint32 width = font->GetStringWidth(fString);
	sint32 height = font->GetMaxHeight();

	RECT		rect = {0, 0, width, height};
	RECT		clipRect;

	OffsetRect(&rect, x - width/2, y - height/2);

	clipRect = rect;
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

void UnitActor::DrawCityWalls(BOOL fogged)
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
				cityImage = tileSet->GetImprovementData(matchingSprite->GetWalls());
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

void UnitActor::DrawForceField(BOOL fogged)
{
	sint32 which;
#if defined(ACTIVISION_DEFAULT)
	sint32 nudgeX, nudgeY;

	if (g_theWorld->IsLand(m_pos)) {
		nudgeX = (sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale());
		nudgeY = (sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());
		which = 154;
	} else
	if (g_theWorld->IsWater(m_pos)) {
		nudgeX = (sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale());
		nudgeY = (sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());
		which = 155;
	} else {
		nudgeX = (sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale());
		nudgeY = (sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());
		which = 156;
	}
#else
	sint32 const	nudgeX	= 
		(sint32)((double)((k_ACTOR_CENTER_OFFSET_X) - 48) * g_tiledMap->GetScale());
	sint32 const	nudgeY = 
		(sint32)((double)((k_ACTOR_CENTER_OFFSET_Y) - 48) * g_tiledMap->GetScale());

	// Default
	if (g_theWorld->IsLand(m_pos)) 
	{
		which = 154;
	} 
	else if (g_theWorld->IsWater(m_pos)) 
	{
		which = 155;
	} 
	else 
	{
		which = 156;	// space?
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
#endif

	
	Pixel16 *cityImage = g_tiledMap->GetTileSet()->GetImprovementData((uint16)which);


	if (g_tiledMap->GetZoomLevel() == k_ZOOM_LARGEST) {
		
		g_tiledMap->DrawDitheredOverlayIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY, fogged);






	} else {
		
		g_tiledMap->DrawDitheredOverlayScaledIntoMix(cityImage, m_x + nudgeX, m_y + nudgeY,
														g_tiledMap->GetZoomTilePixelWidth(),
														g_tiledMap->GetZoomTileGridHeight(),
														fogged);










	}
}

BOOL UnitActor::Draw(BOOL fogged)
{
	uint16			flags;
	Pixel16			color;
	Unit			top;
	BOOL			directionAttack = FALSE;
	sint32			xoffset = 0; 
	sint32			yoffset = 0; 
	sint32			mapoffx = 0;
	sint32			mapoffy = 0;

	if (m_hidden)
		return FALSE;

	if(m_hiddenUnderStack)
		return FALSE;

	xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());

	


	xoffset-=mapoffx;
	yoffset-=mapoffy;

	flags = k_DRAWFLAGS_NORMAL;
	color = 0x0000;

	
	
	if (m_transparency < 15) {
		flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;	
	}

	if (fogged)
		flags |= k_BIT_DRAWFLAGS_FOGGED;

	BOOL isCloaked = FALSE;

	if (g_theUnitPool->IsValid(m_unitID)) {
		if (m_unitID.IsAsleep()) {
			flags |= k_BIT_DRAWFLAGS_DESATURATED;
			m_isFortified = FALSE;
			m_isFortifying = FALSE;
		}

		if (m_unitID.IsCloaked())
			isCloaked = TRUE;
	}

	if(m_directionalAttack == TRUE && m_curUnitAction == UNITACTION_ATTACK)
		directionAttack = TRUE;

	SELECT_TYPE		selectType;
	ID				selectedID;
	PLAYER_INDEX	selectedPlayer;

	g_selected_item->GetTopCurItem(selectedPlayer, selectedID, selectType);

	Unit selectedUnit;

	if(selectType == SELECT_TYPE_LOCAL_CITY) {
		selectedUnit = selectedID;
	} else if(selectType == SELECT_TYPE_LOCAL_ARMY) {
		selectedUnit = ((Army)selectedID).GetTopVisibleUnit(selectedPlayer);
	}

	BOOL	drawShield = TRUE;
	BOOL	drawSelectionBrackets = FALSE;

	if (g_theUnitPool->IsValid(selectedUnit) && selectedUnit.GetActor() == this) {
		drawSelectionBrackets = TRUE;	
		if (GetTickCount() > m_shieldFlashOffTime) {
			drawShield = FALSE;
			if (GetTickCount() > m_shieldFlashOnTime) {
				m_shieldFlashOffTime = GetTickCount() + k_SHIELD_ON_TIME;
				m_shieldFlashOnTime = m_shieldFlashOffTime + k_SHIELD_OFF_TIME;
				drawShield = TRUE;
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
		if (IsFortified())
			DrawFortified(fogged);

		if (IsFortifying()) {
			
			DrawFortifying(fogged);
		}
		
		if (HasCityWalls()) {
			DrawCityWalls(fogged);
		}

		uint16 oldTransparency;
		if (isCloaked) {
			oldTransparency = m_transparency;
			m_transparency = 8 + (rand() % 5);
			flags |= k_BIT_DRAWFLAGS_TRANSPARENCY;	
		}

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
								
		BOOL forcefieldsEverywhere = FALSE;

		if (g_player[m_playerNum]) {
			if (wonderutil_GetForcefieldEverywhere(g_player[m_playerNum]->m_builtWonders)) {
				if (g_theUnitPool->IsValid(m_unitID) && m_unitID.IsCity())
					forcefieldsEverywhere = TRUE;
			}
		}

		if (HasForceField() || forcefieldsEverywhere) {
			DrawForceField(fogged);
		}
	}

	if (drawSelectionBrackets) 
		DrawSelectionBrackets();

	if (drawShield)
		DrawHealthBar();

	return TRUE;
}

void UnitActor::DrawDirect(aui_Surface *surf, sint32 x, sint32 y, double scale)
{
	uint16			flags = k_DRAWFLAGS_NORMAL;
	Pixel16			color=0;
	BOOL			directionAttack = FALSE;
	sint32			xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * scale);
	sint32			yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * scale);

	
	if(m_unitSpriteGroup) {
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

void UnitActor::DrawHerald(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Pixel16		color;
	Unit		unit;
	POINT		*pt;
	TileSet		*tileSet;
	static CellUnitList army;
	army.Clear();

	MAPICON		icon = MAPICON_HERALD;

	if(m_hiddenUnderStack == TRUE)
		return;

	if (!g_showHeralds) return;

	unit.m_id = GetUnitID();
	if (!g_theUnitPool->IsValid(unit)) return;

	g_theWorld->GetArmy(unit.RetPos(), army);

	if (army.Num() > 1 && army.Num() < 10) {
		icon = (MAPICON) ((sint32) MAPICON_HERALD2 + army.Num() - 2);
	} else if(army.Num() >= 10) {
		icon = (MAPICON) ((sint32) MAPICON_HERALD10 + (army.Num() - 10));
	}

	color = g_colorSet->GetPlayerColor(m_playerNum);

	tileSet = g_tiledMap->GetTileSet();

	POINT iconDim = tileSet->GetMapIconDimensions(icon);

	RECT	rect = {0, 0, iconDim.x+1, iconDim.y+1};

	
	if(!m_unitSpriteGroup)
		return;

	
	
	if (m_curUnitAction == UNITACTION_IDLE && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE) == NULL) {
		pt = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
	} else {
		pt = m_unitSpriteGroup->GetShieldPoints(m_curUnitAction);
	}

	
	if (m_x < 0 || m_x > g_screenManager->GetSurfWidth()-rect.right) return;
	if (m_y < 0 || m_y > g_screenManager->GetSurfHeight()-rect.bottom) return;

	
	OffsetRect(&rect, m_x + 0  - iconDim.x/2, m_y + 0  - iconDim.y/2);

	
	g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(icon), rect.left, rect.top, color);

	g_tiledMap->AddDirtyRectToMix(rect);
}

void UnitActor::DrawStackingIndicator(sint32 x, sint32 y, sint32 stack)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	TileSet		*tileSet = g_tiledMap->GetTileSet();
	Pixel16		color = g_colorSet->GetPlayerColor(m_playerNum);
	MAPICON		icon = MAPICON_HERALD;

	if(!g_showHeralds) return;

	if (stack > 1 && stack <= 9) {
		icon = (MAPICON) ((sint32) MAPICON_HERALD2 + stack - 2);
	} else if(stack >= 10 && stack <= 12) {
		icon = (MAPICON) ((sint32) MAPICON_HERALD10 + stack - 10);
	}

	POINT	iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);

	if (x < 0) return;
	if (y < 0) return;
	if (x >= g_screenManager->GetSurfWidth() - iconDim.x) return;
	if (y >= g_screenManager->GetSurfHeight() - iconDim.y) return;

	g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(icon), x, y, color);

	sint32 x2 = x;
	sint32 y2 = y + iconDim.y;
	sint32 w = iconDim.x;
	sint32 h = iconDim.y;

	if(m_unitID.IsValid() && m_unitID->GetArmy().IsValid()) {
		
		if(m_unitID->GetArmy()->HasCargo()) {
			if(y2 < g_screenManager->GetSurfHeight() - iconDim.y) {
				g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_CARGO), x2, y2, color);
				iconDim = tileSet->GetMapIconDimensions(MAPICON_CARGO);
				y2 += iconDim.y;
				h += iconDim.y;
				w = max(w, iconDim.x);
			}
		}

		if(m_unitID->GetArmy()->Num() > 1) {
			if(y2 < g_screenManager->GetSurfHeight() - iconDim.y) {
				g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_ARMY), x2, y2, color);
				iconDim = tileSet->GetMapIconDimensions(MAPICON_ARMY);
				y2 += iconDim.y;
				h += iconDim.y;
				w = max(w, iconDim.x);
			}
		}

		if(m_unitID->GetArmy()->HasVeterans()) {
			if(y2 < g_screenManager->GetSurfHeight() - iconDim.y) {
				g_tiledMap->DrawColorizedOverlayIntoMix(tileSet->GetMapIconData(MAPICON_VETERAN), x2, y2, color);
				iconDim = tileSet->GetMapIconDimensions(MAPICON_VETERAN);
				y2 += iconDim.y;
				h += iconDim.y;
				w = max(w, iconDim.x);
			}
		}			
	}
	
	g_tiledMap->AddDirtyToMix(x, y, w, h);
}

void UnitActor::DrawHealthBar(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	RECT		leftRect, rightRect;
	double		ratio;
	Pixel16		color;
	Unit		unit;
	TileSet		*tileSet = g_tiledMap->GetTileSet();
	sint32		top, middle;

	
	
	
	if (m_size > 0) return;

	if(!g_showHeralds) return;

	unit.m_id = GetUnitID();
	if (g_theUnitPool->IsValid(unit)) {
		if (unit.IsCity()) return;
	}

	

	
	sint32		stackSize = 1;
	static Army	army;	
	
	Cell *myCell = g_theWorld->GetCell(m_pos);

	
	
	if (m_tempStackSize != 0) {
		stackSize = m_tempStackSize;
	} else {
		if (IsActive()) {
			if (g_theUnitPool->IsValid(m_unitID)) {
			
				army = m_unitID.GetArmy();

				if (g_theArmyPool->IsValid(army)) {
					stackSize = army.Num();
				}
			}
		} else {
			CellUnitList	*unitList;

			
			

			
			
			unitList = myCell->UnitArmy();
			if (unitList) {
				stackSize = unitList->Num();
				for (sint32 i=0; i<unitList->Num(); i++) {
					static Army a;
					a.m_id = unitList->Access(i).GetArmy().m_id;
					static Unit top;
					top.m_id = 0;
					if(a.IsValid()) {
						top = a->GetTopVisibleUnit(g_selected_item->GetVisiblePlayer());
					}
					if(!top.IsValid()) {
						top.m_id = unitList->Access(i).m_id;
					}
					if (top.GetActor()) {
						if (top.GetActor()->IsActive())
							stackSize--;
					}
				}
			}
		}
	}

	if(unit.IsValid()) {
		if(stackSize > 1 && myCell->GetNumUnits()) {
			ratio = myCell->UnitArmy()->GetAverageHealthPercentage();
		} else {
			
			
			if (m_healthPercent < 0) 
				ratio = unit.GetHP() / unit.GetDBRec()->GetMaxHP();
			else
				ratio = m_healthPercent;
		}
	} else {
		if (m_healthPercent >= 0.0)
			ratio = m_healthPercent;
		else
			ratio = 0.0;
	}


	POINT	iconDim = tileSet->GetMapIconDimensions(MAPICON_HERALD);

	RECT	iconRect = {0, 0, iconDim.x, iconDim.y};

	UNITACTION		unitAction = m_curUnitAction;
	if (m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)m_curUnitAction) == NULL)
		unitAction = UNITACTION_IDLE;

	POINT *shieldPoint;

	
	
	if (unitAction == UNITACTION_IDLE && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)UNITACTION_IDLE) == NULL) {
		shieldPoint = m_unitSpriteGroup->GetShieldPoints(UNITACTION_MOVE);
		OffsetRect(&iconRect, m_x + (sint32)((double)(shieldPoint->x) * g_tiledMap->GetScale()), 
							  m_y + (sint32)((double)(shieldPoint->y) * g_tiledMap->GetScale()));
	} else {
		if (m_unitSpriteGroup && m_unitSpriteGroup->GetGroupSprite((GAME_ACTION)unitAction) != NULL) {
			shieldPoint = m_unitSpriteGroup->GetShieldPoints(unitAction);
			OffsetRect(&iconRect, m_x + (sint32)((double)(shieldPoint->x) * g_tiledMap->GetScale()), 
								  m_y + (sint32)((double)(shieldPoint->y) * g_tiledMap->GetScale()));
		} else {
			
			top = m_y;
			middle = m_x + (sint32)((k_TILE_PIXEL_WIDTH) * g_tiledMap->GetScale())/2;
			OffsetRect(&iconRect, middle - iconDim.x / 2, top - iconDim.y);
		}
	}



	


	


	if (iconRect.left < 0) return;
	if (iconRect.right >= g_screenManager->GetSurfWidth()) return;
	if (iconRect.top < 0) return;
	if (iconRect.bottom >= g_screenManager->GetSurfHeight()) return;

	DrawStackingIndicator(iconRect.left, iconRect.top, stackSize);

	RECT tempRect = iconRect;
	InflateRect(&tempRect, 2, 2);
	tempRect.top -= 8;

	g_tiledMap->AddDirtyRectToMix(tempRect);

	Pixel16		black = g_colorSet->GetColor(COLOR_BLACK);
	if (black == 0x0000)
		black = 0x0001;

	if (g_theProfileDB->GetShowEnemyHealth() || m_playerNum == g_selected_item->GetVisiblePlayer()) {
		iconRect.bottom = iconRect.top;
		iconRect.top = iconRect.bottom - 4;

		if (iconRect.left < 0) return;
		if (iconRect.right >= g_screenManager->GetSurfWidth()) return;
		if (iconRect.top < 0) return;
		if (iconRect.bottom >= g_screenManager->GetSurfHeight()) return;

		primitives_FrameRect16(g_screenManager->GetSurface(), &iconRect, black);
		
		InflateRect(&iconRect, -1, -1);

		leftRect = rightRect = iconRect;

		color = g_colorSet->GetColor(COLOR_GREEN);

		if (ratio < 1.0) {
			leftRect.right = leftRect.left + (sint32)(ratio * (double)(iconRect.right-iconRect.left));
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
	}
}

void UnitActor::DrawSelectionBrackets(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	RECT		rect;
	TileSet		*tileSet = g_tiledMap->GetTileSet();

	SetRect(&rect, 0, 0, 1, 1);

	if(!m_unitSpriteGroup)
		return;

	

	long mapoffx = 0,mapoffy = 0;

	


	
 	OffsetRect(&rect,	m_x + (sint32)(k_TILE_PIXEL_WIDTH * g_tiledMap->GetScale())/2 - mapoffx, 
						m_y + (sint32)(k_TILE_GRID_HEIGHT * g_tiledMap->GetScale())/2 - mapoffy);

	
	InflateRect(&rect, 25, 25);

	g_tiledMap->AddDirtyRectToMix(rect);

	POINT iconDim = tileSet->GetMapIconDimensions(MAPICON_BRACKET1);

	rect.right -= (iconDim.x+1);
	rect.bottom -= (iconDim.y+1);

	
	Pixel16			*topLeft, *topRight, *botLeft, *botRight;

	topLeft = g_tiledMap->GetTileSet()->GetMapIconData(MAPICON_BRACKET1);
	Assert(topLeft); if (!topLeft) return;
	topRight = g_tiledMap->GetTileSet()->GetMapIconData(MAPICON_BRACKET2);
	Assert(topRight); if (!topRight) return;
	botRight = g_tiledMap->GetTileSet()->GetMapIconData(MAPICON_BRACKET3);
	Assert(botRight); if (!botRight) return;
	botLeft = g_tiledMap->GetTileSet()->GetMapIconData(MAPICON_BRACKET4);
	Assert(botLeft); if (!botLeft) return;

	COLOR color = COLOR_YELLOW;

	if (g_theUnitPool->IsValid(m_unitID)) {
		if(m_unitID.IsCity()) {
			color = COLOR_RED;
		}
		if (m_unitID.GetArmy().m_id != 0 && g_theArmyPool->IsValid(m_unitID.GetArmy())) {
			if (m_unitID.GetArmy().CanMove())
				color = COLOR_GREEN;
		}
	}
	Pixel16	pixelColor = g_colorSet->GetColor(color);

	g_tiledMap->DrawColorizedOverlayIntoMix(topLeft, rect.left, rect.top, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(topRight, rect.right, rect.top, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(botRight, rect.right, rect.bottom, pixelColor);
	g_tiledMap->DrawColorizedOverlayIntoMix(botLeft, rect.left, rect.bottom, pixelColor);
}

BOOL UnitActor::IsAnimating(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	if (m_curAction) 
	{
		if (m_curAction->GetActionType() != UNITACTION_IDLE) 
			return TRUE;
		else 
			return FALSE;
	} else 
		return FALSE;
}

uint16 UnitActor::GetWidth(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
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

uint16 UnitActor::GetHeight(void)
{
#ifndef _TEST
	STOMPCHECK();
#endif
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

void UnitActor::GetBoundingRect(RECT *rect)
{
#ifndef _TEST
	STOMPCHECK();
#endif
	Assert(rect != NULL);
	if (rect == NULL) return;

	sint32 x = m_x;
	sint32 y = m_y;

	if(!m_unitSpriteGroup)
		return;

	POINT	hotPoint = m_unitSpriteGroup->GetHotPoint(m_curUnitAction, m_facing);
	double	scale = g_tiledMap->GetScale();
	
	if (m_facing >= 5) {
		x += (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - (GetWidth()-hotPoint.x)) * scale);
	} else {
		x += (sint32)((double)(k_ACTOR_CENTER_OFFSET_X - hotPoint.x) * scale);
	}
	y += (sint32)((double)(k_ACTOR_CENTER_OFFSET_Y - hotPoint.y) * scale);

	rect->left = x;
	rect->top = y;
	rect->right = x + (sint32)((double)GetWidth() * scale);
	rect->bottom = y + (sint32)((double)GetHeight() * scale);













}
void UnitActor::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		
		EndTurnProcess();

		
		if (g_theUnitPool->IsValid(m_unitID) && m_unitID.IsCity()) {
			m_unitID.GetPop(m_size);
		}
		archive << m_facing;
		archive << m_lastMoveFacing;
		archive << m_size;
		archive.PutUINT8((uint8)m_isUnseenCellActor);

		archive.PutUINT8((uint8)m_type);
		archive.PutUINT8((uint8)m_spriteID);
		archive.PutUINT8((uint8)m_playerNum);
		archive.PutUINT32((uint32)m_unitID);
		archive.PutUINT8((uint8)m_unitDBIndex);
		archive << m_unitVisionRange;
		archive << m_unitVisibility;
		
		m_pos.Serialize(archive);
		m_spriteState->Serialize(archive);
	} else {
		archive >> m_facing;
		archive >> m_lastMoveFacing;
		archive >> m_size;
		
		m_isUnseenCellActor = (BOOL)archive.GetUINT8();

		m_type = (GROUPTYPE)archive.GetUINT8();
		m_spriteID = archive.GetUINT8();
		m_playerNum = (sint32)archive.GetUINT8();

		m_unitID = Unit(archive.GetUINT32());

		m_unitDBIndex = (sint32)archive.GetUINT8();
		archive >> m_unitVisionRange;
		archive >> m_unitVisibility;

		m_pos.Serialize(archive);
		if (m_spriteState) delete m_spriteState;
		m_spriteState = new SpriteState(archive);
	}
}

LOADTYPE UnitActor::GetLoadType(void)
{
	if (!m_unitSpriteGroup) return LOADTYPE_NONE;

	return m_loadType;
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




bool 
UnitActor::ActionMove(Action *actionObj)	
{
	Assert(actionObj != NULL);
	
	if (actionObj == NULL) 
		return false;

	Anim	 *anim		=	NULL;
	sint32    visiblePlayer;
	
	
	sint32	  speed				= g_theProfileDB->GetUnitSpeed();
	sint32	  maxActionCounter	= k_MAX_UNIT_MOVEMENT_ITERATIONS - speed;

	
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

	
	anim = GetAnim(UNITACTION_MOVE);
	
	Assert(anim != NULL);
   
	
	if (anim == NULL) 
   		return false;

	
	actionObj->SetAnim(anim);

	actionObj->SetUnitsVisibility(GetUnitVisibility());
	actionObj->SetUnitVisionRange(GetUnitVisionRange());

	actionObj->SetMaxActionCounter(maxActionCounter);
	actionObj->SetCurActionCounter(0);

	AddAction(actionObj);

	
	
	
	if (GetIsTransported()==k_TRANSPORTREMOVEONLY) 
		TerminateLoopingSound(SOUNDTYPE_SFX);
	else 
	{
		visiblePlayer = g_selected_item->GetVisiblePlayer();

		if ((visiblePlayer == GetPlayerNum()) ||
			(GetUnitVisibility() & (1 << visiblePlayer))) 
			AddLoopingSound(SOUNDTYPE_SFX,actionObj->GetSoundEffect());
   	}

	
	return true;
}



bool 
UnitActor::ActionAttack(Action *actionObj,sint32 facing)	
{
	Assert(actionObj != NULL);
	
	if (actionObj == NULL) 
		return false;

	if(GetNeedsToDie()) 
	   return false;

	sint32    visiblePlayer = g_selected_item->GetVisiblePlayer();
	
	
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

	
	  if ((visiblePlayer == GetPlayerNum()) || (GetUnitVisibility() & (1 << visiblePlayer))) 
		  AddSound(SOUNDTYPE_SFX,actionObj->GetSoundEffect());

	
	return true;
}		   


bool 
UnitActor::ActionSpecialAttack(Action *actionObj,sint32 facing)	
{
	Assert(actionObj != NULL);
	
	if (actionObj==NULL) 
		return false;

	if(GetNeedsToDie()) 
	   return false;

	sint32    visiblePlayer = g_selected_item->GetVisiblePlayer();
	
	
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

	
	  if ((visiblePlayer == GetPlayerNum()) || (GetUnitVisibility() & (1 << visiblePlayer))) 
		  AddSound(SOUNDTYPE_SFX,actionObj->GetSoundEffect());

	
	return true;
}


bool 
UnitActor::TryAnimation(Action *actionObj,UNITACTION action)
{
	
	FullLoad(action);

	
	if(GetAnim(action)!=NULL)
	{ 
	  actionObj->SetAnimPos(GetHoldingCurAnimPos(action));
	  actionObj->SetSpecialDelayProcess(GetHoldingCurAnimSpecialDelayProcess(action));
	  actionObj->SetAnim(GetAnim(action));
	  return true;
	}
	
	return false;
}


void UnitActor::DumpFullLoad(void)
{
	if (!m_unitSpriteGroup) return;
	if (m_loadType != LOADTYPE_FULL) return;

	BOOL purged = g_unitSpriteGroupList->ReleaseSprite(m_spriteID, LOADTYPE_FULL);

	if (purged) {
		m_unitSpriteGroup = NULL;
	} else {
		m_loadType = LOADTYPE_BASIC;
	}
}

BOOL UnitActor::HitTest(POINT mousePt)
{
	BOOL			directionAttack = FALSE;
	sint32			xoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_X * g_tiledMap->GetScale());
	sint32			yoffset = (sint32)((double)k_ACTOR_CENTER_OFFSET_Y * g_tiledMap->GetScale());
	COLOR			color = COLOR_WHITE;
	uint16			flags = 0;

	if(m_directionalAttack == TRUE && m_curUnitAction == UNITACTION_ATTACK)
		directionAttack = TRUE;

	if(m_curAction == NULL)
	{
		
		return m_unitSpriteGroup->HitTest(mousePt, m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing, 
								g_tiledMap->GetScale(), m_transparency, color, flags, FALSE, directionAttack);
	}
	else
	{
		return m_unitSpriteGroup->HitTest(mousePt, m_curUnitAction, m_frame, m_x+xoffset, m_y+yoffset, m_facing, 
								g_tiledMap->GetScale(), m_transparency, color, flags, m_curAction->SpecialDelayProcess(), directionAttack);
	}
}





void			
UnitActor::AddLoopingSound(uint32 sound_type, sint32 sound_id)
{
	if ((g_soundManager)&&(sound_id>=0))
	   	g_soundManager->AddLoopingSound((SOUNDTYPE)sound_type,
										(uint32)GetUnitID(),sound_id,
										GetPos().x, GetPos().y);

}


void			
UnitActor::AddSound(uint32 sound_type, sint32 sound_id)
{
	if ((g_soundManager)&&(sound_id>=0))
	   	g_soundManager->AddSound((SOUNDTYPE)sound_type,
								 (uint32)GetUnitID(),sound_id,
								 GetPos().x, GetPos().y);
}


void			
UnitActor::TerminateLoopingSound(uint32 sound_type) 
{
	if (g_soundManager)
 		g_soundManager->TerminateLoopingSound(SOUNDTYPE_SFX,GetUnitID());
}



#ifdef _DEBUG
void UnitActor::DumpActor(void)
{
	sint32 i;

	DPRINTF(k_DBG_UI, ("Actor %#.8lx\n", this));
	DPRINTF(k_DBG_UI, ("  m_unitID           :%#.8lx\n", m_unitID));
	DPRINTF(k_DBG_UI, ("  m_unitDBIndex      :%d\n", m_unitDBIndex));
	DPRINTF(k_DBG_UI, ("  m_curAction        :%#.8lx\n", m_curAction));

	if (m_curAction) {
		DPRINTF(k_DBG_UI, ("  m_curAction.m_actionType     :%ld\n", m_curAction->m_actionType));
		DPRINTF(k_DBG_UI, ("  m_curAction.m_finished       :%ld\n", m_curAction->Finished()));

		DPRINTF(k_DBG_UI, ("  m_curAction.m_sequence       :%#.8lx\n", m_curAction->GetSequence()));
		if (m_curAction->GetSequence()) {
			DPRINTF(k_DBG_UI, ("Actor %#.8lx m_curAction:\n", this));
			DPRINTF(k_DBG_UI, ("  m_curAction.m_sequence->m_sequenceID     :%ld\n", 
							m_curAction->GetSequence()->GetSequenceID()));
			DQItem		*item = m_curAction->GetSequence()->GetItem();
			g_director->DumpItem(item);
		}
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));		

	DPRINTF(k_DBG_UI, ("  m_actionQueue         :%d\n", m_actionQueue.GetNumItems()));
	if (m_actionQueue.GetNumItems() > 0) {
		Action		*action;
		for (i=0; i<m_actionQueue.GetNumItems(); i++) {
			m_actionQueue.GetQueueItem(i, action);
			DPRINTF(k_DBG_UI, ("  m_actionQueue Item      :%d\n", i));

			if (action) {
				DPRINTF(k_DBG_UI, ("  action.m_actionType     :%ld\n", action->m_actionType));
				DPRINTF(k_DBG_UI, ("  action.m_finished       :%ld\n", action->Finished()));
				DPRINTF(k_DBG_UI, ("  action.m_sequence       :%#.8lx\n", action->GetSequence()));
				if (action->GetSequence()) {
					DPRINTF(k_DBG_UI, ("  action.m_sequence->m_sequenceID:%ld\n", 
									action->GetSequence()->GetSequenceID()));
					DQItem		*item = action->GetSequence()->GetItem();
					g_director->DumpItem(item);
				}
			}
		}
	}
	DPRINTF(k_DBG_UI, (" ------------------\n"));		
}
#endif
