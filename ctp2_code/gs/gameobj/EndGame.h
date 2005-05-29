#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ENDGAME_H__
#define __ENDGAME_H__

class CivArchive;
#include "gstypes.h"

class EndGame {
private:
	
	sint32 m_owner;
	sint32 m_currentStage;

	sint32 m_savedCurrentStage;

	sint32 m_currentStageBegan; 

	
	sint32 *m_numBuilt;

	
	
	sint32 *m_savedNumBuilt;

	friend class NetEndGame;

public:
	EndGame(PLAYER_INDEX owner);
	EndGame(CivArchive &archive);
	~EndGame();
	void Serialize(CivArchive &archive);
	void Init();

	void AddObject(sint32 type);
	void ClearAll();
	BOOL BeginSequence();
	void BeginTurn();
	void AdvanceStage();
	sint32 GetCataclysmChance();
	sint32 GetTurnsForNextStage();
	BOOL MetRequirementsForNextStage();
	BOOL HaveAllPrerequisites();
	BOOL HaveEnoughFields();
    BOOL HaveEnoughECDs();
	BOOL HaveMaxSplicers();

	void XLabCaptured();
	void Cataclysm();

	
	sint32 GetTurnsSinceStageBegan();

	
	sint32 GetStage();
	sint32 GetNumberBuilt(sint32 type);

	
	sint32 GetDisplayedStage();
	sint32 GetNumberShown(sint32 type);

	
	void UpdateDisplayState(void);

	BOOL HasLab();
};

#endif
