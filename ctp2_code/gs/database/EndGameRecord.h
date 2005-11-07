#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ENDGAME_RECORD_H__
#define __ENDGAME_RECORD_H__

#include "Rec.h"
class Token;

class EndGameRecord : public Record
{
private:
	
	sint32 m_cost;
	sint32 m_numStages;	
	sint32 m_soundID;			
	BOOL m_exactlyOneRequired;
	sint32 m_maxAllowed;
	sint32 m_minRequired;
	double m_successBonus;
	BOOL m_requiresProbeRecovery;
	double m_rushBuyModifer;
	BOOL m_controlsSpeed;
	sint32 m_cataclysm_num;
	sint32 m_cataclysm_percent;
	sint32 m_notify_lab_built;
	sint32 m_starts_sequence;
	sint32 m_requiresLab;
public:
	sint32 m_iconDBIndex;
private:
	sint32 m_scoreBonusOverMinimum;
	
	
	sint32 *m_requiredForStage;
	sint32 *m_turnsPerStage;

public:
	EndGameRecord();
	EndGameRecord(CivArchive &archive);
	~EndGameRecord();
	void Serialize(CivArchive &archive);

	BOOL ParseNumber(Token *token, sint32 &val);
	BOOL ParseMultipleNumbers(Token *token, sint32 *array, sint32 count);
	BOOL ParseFloat(Token *token, double &val);
	BOOL ParseFlags(Token *token);
	BOOL ParseRecord(Token *token);

	BOOL RequiresProbeRecovery() const { return m_requiresProbeRecovery; }
	double RushBuyModifier() const { return m_rushBuyModifer; }
	BOOL ControlsSpeed() const { return m_controlsSpeed; }
	sint32 GetTurnsPerStage(sint32 numBuilt) const;
	BOOL ExactlyOneRequired() const;
	sint32 RequiredForStage(sint32 stage) const;
	sint32 GetCataclysmNum() const { return m_cataclysm_num; }
	sint32 GetCataclysmPercent() const { return m_cataclysm_percent; }
	sint32 NotifyLabBuilt() const { return m_notify_lab_built; }
	sint32 GetSoundID() const { return m_soundID; }	
	sint32 StartsSequence() const { return m_starts_sequence; }
	sint32 RequiresLab() const { return m_requiresLab; }
	sint32 GetMaxAllowed() const { return m_maxAllowed; }

	sint32 GetCost() const { return m_cost; }
	sint32 RequiredToAdvanceFromStage(sint32 stage) const;
};

#endif
