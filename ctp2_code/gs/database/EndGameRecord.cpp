

#include "c3.h"
#include "EndGameDB.h"
#include "Token.h"
#include "EndGameToken.h"
#include "CivArchive.h"
#include "StrDB.h"
#include "AdvanceRecord.h"
#include "SoundRecord.h"
#include "IconRecord.h"

static BOOL s_abort_endgame_parse;
extern StringDB *g_theStringDB;

EndGameRecord::EndGameRecord()
{
	m_cost = 0;
	m_numStages = 0;
	m_soundID = -1;
	m_exactlyOneRequired = FALSE;
	m_requiredForStage = NULL;
	m_maxAllowed = 0;
	m_turnsPerStage = NULL;
	m_minRequired = 0;
	m_successBonus = 0;
	m_scoreBonusOverMinimum = 0;
	m_enable = -1;
	m_obsolete[0] = -1;
	m_controlsSpeed = FALSE;
	m_rushBuyModifer = 10;
	m_requiresProbeRecovery = FALSE;
	m_cataclysm_num = 0;
	m_cataclysm_num = 0;
	m_notify_lab_built = FALSE;
	m_starts_sequence = FALSE;
	m_requiresLab = FALSE;
}

EndGameRecord::EndGameRecord(CivArchive &archive)
{
	Serialize(archive);
}

EndGameRecord::~EndGameRecord()
{
	if(m_requiredForStage)
		delete [] m_requiredForStage;

	if(m_turnsPerStage)
		delete [] m_turnsPerStage;
}

void EndGameRecord::Serialize(CivArchive &archive)
{
	sint32 i;
	Record::Serialize(archive);
	if(archive.IsStoring()) {
		archive.StoreChunk((uint8*)&m_cost, (uint8*)&m_scoreBonusOverMinimum + sizeof(m_scoreBonusOverMinimum));
		for(i = 0; i < m_numStages; i++) {
			archive << m_requiredForStage[i];
		}
		for(i = 0; i < m_maxAllowed; i++) {
			archive << m_turnsPerStage[i];
		}
	} else {
		archive.LoadChunk((uint8*)&m_cost, (uint8*)&m_scoreBonusOverMinimum + sizeof(m_scoreBonusOverMinimum));
		if(m_numStages > 0) {
			m_requiredForStage = new sint32[m_numStages];
			for(i = 0; i < m_numStages; i++) {
				archive >> m_requiredForStage[i];
			}
		} else {
			m_requiredForStage = NULL;
		}

		if(m_maxAllowed > 0) {
			m_turnsPerStage = new sint32[m_maxAllowed];
			for(i = 0; i < m_maxAllowed; i++) {
				archive >> m_turnsPerStage[i];
			}
		} else {
			m_turnsPerStage = NULL;
		}
	}
}

BOOL EndGameRecord::ParseNumber(Token *token, sint32 &val)
{
	if(token->Next() != TOKEN_NUMBER) {
		s_abort_endgame_parse = TRUE;
		c3errors_ErrorDialog(token->ErrStr(), "Expected number");
		return FALSE;
	}
	token->GetNumber(val);
	return TRUE;
}

BOOL EndGameRecord::ParseMultipleNumbers(Token *token, sint32 *array, sint32 count)
{
	sint32 i;
	for(i = 0; i < count; i++) {
		if(token->Next() != TOKEN_NUMBER) {
			s_abort_endgame_parse = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expecting %d values", count);
			return FALSE;
		}
		token->GetNumber(array[i]);
	}
	return TRUE;
}

BOOL EndGameRecord::ParseFloat(Token *token, double &val)
{
	if(token->Next() != TOKEN_NUMBER) {
		s_abort_endgame_parse = TRUE;
		c3errors_ErrorDialog(token->ErrStr(), "Expected number");
		return FALSE;
	}
	token->GetFloat(val);
	return TRUE;
}

BOOL EndGameRecord::ParseRecord(Token *token)
{
	sint32 i;
	MBCHAR str[_MAX_PATH];
	s_abort_endgame_parse = FALSE;
	m_numStages = g_theEndGameDB->GetNumStages();
	m_requiredForStage = new sint32[m_numStages];
	m_turnsPerStage = new sint32[m_numStages];
	memset(m_requiredForStage, 0, sizeof(sint32) * m_numStages);
	memset(m_requiredForStage, 0, sizeof(sint32) * m_numStages);

	if(!token_ParseAnOpenBraceNext(token)) {
		s_abort_endgame_parse = TRUE;
		return FALSE;
	}

	if(token->Next() != TOKEN_ENDGAME_DEFAULT_ICON) {
		c3errors_ErrorDialog(token->ErrStr(), "Field ENDGAME_DEFAULT_ICON expected");
		s_abort_endgame_parse = TRUE;
		return FALSE;
	}
	if(token->Next() != TOKEN_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "ID string for ENDGAME_DEFAULT_ICON expected");
		s_abort_endgame_parse = TRUE;
		return FALSE;
	}

	
	token->GetString(str);
	i = g_theIconDB->FindTypeIndex(str);
	if (i == -1) {
		c3errors_ErrorDialog(token->ErrStr(), "Couldn't find End game icon filename string for '%s.'", str);
		s_abort_endgame_parse = TRUE;
		return FALSE;
	}
	m_iconDBIndex = i;

	while(ParseFlags(token))
		;
	if(s_abort_endgame_parse)
		return FALSE;
	return TRUE;
}

BOOL EndGameRecord::ParseFlags(Token *token)
{
	sint32 tokenval = token->Next();
	char str[k_MAX_NAME_LEN];
	StringId str_id; 
	
	switch(tokenval) {
		case TOKEN_CLOSE_BRACE:
			return FALSE;
		case TOKEN_PRODUCTION_COST:
			if(!ParseNumber(token, m_cost)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENDGAME_EXACTLY_ONE:
			m_exactlyOneRequired = TRUE;
			return TRUE;
		case TOKEN_ENDGAME_MAX_ALLOWED:
			if(!ParseNumber(token, m_maxAllowed)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENDGAME_REQUIRED_FOR_STAGE:
			if(!ParseMultipleNumbers(token, m_requiredForStage, m_numStages)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENDGAME_TURNS_PER_STAGE:
			m_controlsSpeed = TRUE;
			if(m_maxAllowed <= 0) {
				c3errors_ErrorDialog(token->ErrStr(), "Need MAX_ALLOWED before turns per stage can be set");
				s_abort_endgame_parse = TRUE;
				return FALSE;
			}
			if(!ParseMultipleNumbers(token, m_turnsPerStage, m_maxAllowed + 1)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENDGAME_MIN_REQUIRED:
			if(!ParseNumber(token, m_minRequired)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENDGAME_SUCCESS_BONUS:
			if(!ParseFloat(token, m_successBonus)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENDGAME_SCORE_BONUS_OVER_MINIMUM:
			if(!ParseNumber(token, m_scoreBonusOverMinimum)) {
				return FALSE;
			}
			return TRUE;
		case TOKEN_ENABLING_ADVANCE:
			if(token->Next() == TOKEN_NULL) {
				m_enable = -1;
			} else {
				token->GetString(str);
				if(!g_theStringDB->GetStringID(str, str_id)) {
					c3errors_ErrorDialog(token->ErrStr(), "%s not in string database", str);
					s_abort_endgame_parse = TRUE;
					return FALSE;
				}
				if(!g_theAdvanceDB->GetNamedItem(str_id, m_enable)) {
					c3errors_ErrorDialog(token->ErrStr(), "%s not in Advance database", str);
					s_abort_endgame_parse = TRUE;
					return FALSE;
				}
			}
			return TRUE;
		case TOKEN_ENDGAME_REQUIRES_PROBE_RECOVERY:
			m_requiresProbeRecovery = TRUE;
			return TRUE;
		case TOKEN_ENDGAME_RUSH_BUY_MODIFIER:
			if(!ParseFloat(token, m_rushBuyModifer))
				return FALSE;
			return TRUE;
		case TOKEN_ENDGAME_CATACLYSM_CHANCE:
		{
			sint32 numbers[2];
			if(!ParseMultipleNumbers(token, numbers, 2)) {
				c3errors_ErrorDialog(token->ErrStr(), "CATACLYSM_CHANCE requires a number and a percentage");
				s_abort_endgame_parse = TRUE;
				return FALSE;
			}
			m_cataclysm_num = numbers[0];
			m_cataclysm_percent = numbers[1];
			return TRUE;
		}
		case TOKEN_ENDGAME_NOTIFY_LAB_BUILT:
			m_notify_lab_built = TRUE;
			return TRUE;
		case TOKEN_ENDGAME_STARTS_SEQUENCE:
			m_starts_sequence = TRUE;
			return TRUE;
		case TOKEN_ENDGAME_SOUND_ID:
			{
				
				if(token->Next() != TOKEN_STRING) {
					c3errors_ErrorDialog(token->ErrStr(), "Expected string after SOUND_ID"); 
					s_abort_endgame_parse = TRUE;
					return FALSE; 
				}

				
				token->GetString(str); 

				
				sint32 snd_id = g_theSoundDB->FindTypeIndex(str);
				if(snd_id == -1) { 
					c3errors_ErrorDialog(token->ErrStr(), "Could not find %s in sound database", str); 
					s_abort_endgame_parse = TRUE;
					return FALSE; 
				}

				
				m_soundID = snd_id;
			}
			return TRUE;
		case TOKEN_ENDGAME_REQUIRES_LAB:
			m_requiresLab = TRUE;
			return TRUE;
		default:
			c3errors_ErrorDialog(token->ErrStr(), "Unknown token");
			s_abort_endgame_parse = TRUE;
			return FALSE;
	}
}

sint32 EndGameRecord::GetTurnsPerStage(sint32 numBuilt) const
{
	if(!m_controlsSpeed)
		return -1;
	Assert(numBuilt <= m_maxAllowed);
	if(numBuilt <= m_maxAllowed) {
		return m_turnsPerStage[numBuilt];
	}
	return -1;
}

BOOL EndGameRecord::ExactlyOneRequired() const
{
	return m_exactlyOneRequired;
}

sint32 EndGameRecord::RequiredToAdvanceFromStage(sint32 stage) const
{
	Assert(stage >= 0);
	Assert(stage < m_numStages);
	if(stage < 0 || stage >= m_numStages) {
		return 0x7fffffff;
	}
	
	return m_requiredForStage[stage];
}
