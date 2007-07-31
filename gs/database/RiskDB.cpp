

#include "c3.h"
#include "c3errors.h"
#include "RiskDB.h"
#include "CivArchive.h"
#include "Token.h"
#include "StrDB.h"

extern StringDB *g_theStringDB;
RiskRecord::RiskRecord()
{
	m_barbarianHutChance = 0.05;
	m_maxHutBarbarians = 1;
	m_goldChance = 0.05;
	m_minGold = 50;
	m_maxGold = 100;
	m_advanceChance = 0.05;
	m_maxAdvancePrerequisites = 1;
	m_unitChance =  0.05;
	m_maxUnitPrerequisites = 1;
	m_cityChance = 0.05 ;
	m_settlerChance = 0.05;

	
	m_barbarianChance = 0.05;
	m_barbarianUnitRankMax = 3;
	m_barbarianUnitRankMin = 1;
	m_maxSpontaneousBarbarians = 1;
	m_firstBarbarianTurn = 20;
	m_minimumBarbarianDistance = 5;
}

RiskRecord::RiskRecord(CivArchive &archive)
{
	Serialize(archive);
}

void RiskRecord::Serialize(CivArchive &archive)
{
	if(archive.IsStoring()) {
		archive.Store((uint8 *)&m_barbarianHutChance,
					 (uint8 *)&m_barbarianUnitRankMin - (uint8*)&m_barbarianHutChance + sizeof(m_barbarianUnitRankMin));
	} else {
		archive.Load((uint8 *)&m_barbarianHutChance,
					 (uint8 *)&m_barbarianUnitRankMin - (uint8*)&m_barbarianHutChance + sizeof(m_barbarianUnitRankMin));
	}
}

RiskDatabase::RiskDatabase()
{
}

RiskDatabase::RiskDatabase(CivArchive &archive)
{
	Serialize(archive);
}

void RiskDatabase::Serialize(CivArchive &archive)
{
	Database<RiskRecord>::Serialize(archive);
}

BOOL RiskDatabase::Initialize(char *filename, C3DIR dir)
{
	m_abort_parse = FALSE;
	return ParseRiskDatabase(filename, dir);
}

BOOL RiskDatabase::ParseRiskDatabase(char *filename, C3DIR dir)
{
	Token *token = new Token(filename, dir);
	sint32 n, count = 0;
	if(token->GetType() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Missing number of risks");
		return FALSE;
	}

	token->GetNumber(n);

	if(n < 0) {
		c3errors_ErrorDialog(token->ErrStr(), "Number of risks is negative");
		delete token;
		return FALSE;
	}

	SetSize(n);

	while(ParseARisk(token)) {
		count++;
	}

	if(m_abort_parse) {
		delete token;
		return FALSE;
	}

	if(count != n) {
		c3errors_ErrorDialog(token->ErrStr(), "Incorrect number of risks");
		delete token;
		return FALSE;
	}

	delete token;
	return TRUE;
}

BOOL RiskDatabase::ParseARisk(Token *token)
{
	char str[k_MAX_NAME_LEN];
	StringId str_id;

	token->Next();
	if(token->GetType() == TOKEN_EOF)
		return FALSE;

	if(token->GetType() != TOKEN_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "Risk ID expected");
		m_abort_parse = TRUE;
		return FALSE;
	}

	token->GetString(str);
	if(!g_theStringDB->GetStringID(str, str_id)) {
		c3errors_ErrorDialog(token->ErrStr(), "Couldn't find %s in string db", str);
		m_abort_parse = TRUE;
		return FALSE;
	}

	StringId u;
	AddRec(str_id, u);

	if(token->Next() != TOKEN_OPEN_BRACE) {
		c3errors_ErrorDialog(token->ErrStr(), "Missing open brace");
		m_abort_parse = TRUE;
		return FALSE;
	}

	while(ParseAnAttribute(token, Access(u)))
		;

	return !m_abort_parse;
}

BOOL RiskDatabase::ParseANumber(Token *token, sint32 &val)
{
	if(token->Next() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Value not a number");
		m_abort_parse = TRUE;
		return FALSE;
	}
	token->GetNumber(val);
	return TRUE;
}

BOOL RiskDatabase::ParseAFloat(Token *token, double &val)
{
	if(token->Next() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Value not a number");
		m_abort_parse = TRUE;
		return FALSE;
	}
	token->GetFloat(val);
	return TRUE;
}

BOOL RiskDatabase::ParseAnAttribute(Token *token, RiskRecord *rec)
{
	char str[k_MAX_NAME_LEN];
	switch(token->Next()) {
		case TOKEN_BARBARIAN_HUT_CHANCE:
			return ParseAFloat(token, rec->m_barbarianHutChance);
		case TOKEN_MAX_HUT_BARBARIANS:
			return ParseANumber(token, rec->m_maxHutBarbarians);
		case TOKEN_GOLD_HUT_CHANCE:
			return ParseAFloat(token, rec->m_goldChance);
		case TOKEN_MIN_HUT_GOLD:
			return ParseANumber(token, rec->m_minGold);
		case TOKEN_MAX_HUT_GOLD:
			return ParseANumber(token, rec->m_maxGold);
		case TOKEN_ADVANCE_HUT_CHANCE:
			return ParseAFloat(token, rec->m_advanceChance);
		case TOKEN_MAX_HUT_ADVANCE_PREREQUISITES:
			return ParseANumber(token, rec->m_maxAdvancePrerequisites);
		case TOKEN_UNIT_HUT_CHANCE:
			return ParseAFloat(token, rec->m_unitChance);
		case TOKEN_MAX_UNIT_PREREQUISITES:
			return ParseANumber(token, rec->m_maxUnitPrerequisites);
		case TOKEN_CITY_HUT_CHANCE:
			return ParseAFloat(token, rec->m_cityChance);
		case TOKEN_SETTLER_HUT_CHANCE:
			return ParseAFloat(token, rec->m_settlerChance);

		case TOKEN_BARBARIAN_CHANCE:
			return ParseAFloat(token, rec->m_barbarianChance);
		case TOKEN_BARBARIAN_RANK_MAX:
			return ParseANumber(token, rec->m_barbarianUnitRankMax);
		case TOKEN_BARBARIAN_RANK_MIN:
			return ParseANumber(token, rec->m_barbarianUnitRankMin);
		case TOKEN_MAX_SPONTANEOUS_BARBARIANS:
			return ParseANumber(token, rec->m_maxSpontaneousBarbarians);
		case TOKEN_FIRST_BARBARIAN_TURN:
			return ParseANumber(token, rec->m_firstBarbarianTurn);
		case TOKEN_LAST_BARBARIAN_TURN:
			return ParseANumber(token, rec->m_lastBarbarianTurn);
		case TOKEN_MINIMUM_BARBARIAN_DISTANCE:
			return ParseANumber(token, rec->m_minimumBarbarianDistance);
		case TOKEN_CLOSE_BRACE:
			return FALSE;
		default:
			token->GetString(str);
			c3errors_ErrorDialog(token->ErrStr(), "Unknown token %s\n", str);
			m_abort_parse = TRUE;
			return FALSE;
	}
}

