
#pragma once
#include "c3.h"
#include "c3errors.h"
#include "EndGameDB.h"
#include "Token.h"
#include "civarchive.h"
#include "EndGameToken.h"
#include "StrDB.h"

extern StringDB *g_theStringDB;

static TokenData s_endgameTokenData[TOKEN_ENDGAME_MAX_VAL] = {
	{TOKEN_ENDGAME_EXACTLY_ONE, "EXACTLY_ONE"},
	{TOKEN_ENDGAME_NUM_STAGES, "NUM_STAGES"},
	{TOKEN_ENDGAME_MAX_ALLOWED, "MAX_ALLOWED"},
	{TOKEN_ENDGAME_TURNS_PER_STAGE, "TURNS_PER_STAGE"},
	{TOKEN_ENDGAME_REQUIRED_FOR_STAGE, "REQUIRED_FOR_STAGE"},
	{TOKEN_ENDGAME_MIN_REQUIRED, "MIN_REQUIRED"},
	{TOKEN_ENDGAME_SUCCESS_BONUS, "SUCCESS_BONUS"},
	{TOKEN_ENDGAME_SCORE_BONUS_OVER_MINIMUM, "SCORE_BONUS_OVER_MINIMUM"},
	{TOKEN_ENDGAME_REQUIRES_PROBE_RECOVERY, "REQUIRES_PROBE_RECOVERY"},
	{TOKEN_ENDGAME_RUSH_BUY_MODIFIER, "RUSH_BUY_MODIFIER"},
	{TOKEN_ENDGAME_CATACLYSM_CHANCE, "CATACLYSM_CHANCE"},
	{TOKEN_ENDGAME_NOTIFY_LAB_BUILT, "NOTIFY_LAB_BUILT"},
	{TOKEN_ENDGAME_STARTS_SEQUENCE, "STARTS_SEQUENCE"},
	{TOKEN_ENDGAME_REQUIRES_LAB, "REQUIRES_LAB"},
	{TOKEN_ENDGAME_SOUND_ID, "SOUND_ID"},
};

EndGameDatabase *g_theEndGameDB = NULL;

EndGameDatabase::EndGameDatabase()
{
	m_numStages = 0;
}

EndGameDatabase::EndGameDatabase(CivArchive &archive)
{
	Serialize(archive);
}

void EndGameDatabase::Serialize(CivArchive &archive)
{
	Database<EndGameRecord>::Serialize(archive);

	if(archive.IsStoring()) {
		archive << m_numStages;
	} else {
		archive >> m_numStages;
	}
}

BOOL EndGameDatabase::Initialize(char *filename, C3DIR dir)
{
	m_abort_parse = FALSE;
	Token *token = new Token(filename, 
							 TOKEN_ENDGAME_MAX_VAL - TOKEN_MAX,
							 s_endgameTokenData, dir);

	if(token->GetType() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Expected number of endgame objects");
		return FALSE;
	}

	sint32 n;
	sint32 count = 0;
	token->GetNumber(n);
	SetSize(n);

	if(!token_ParseValNext(token, TOKEN_ENDGAME_NUM_STAGES, m_numStages)) {
		c3errors_ErrorDialog(token->ErrStr(), "Expected number of stages");
		return FALSE;
	}

	while(ParseAnEndGameObject(token, count)) {
		count++;
	}

	delete token;
	if(m_abort_parse)
		return FALSE;
	return TRUE;
}

BOOL EndGameDatabase::ParseAnEndGameObject(Token *token, sint32 index)
{
	char str[k_MAX_NAME_LEN];
	StringId str_id;
	if(token->Next() == TOKEN_EOF) {
		return FALSE;
	}

	if(token->GetType() != TOKEN_STRING) {
		c3errors_ErrorDialog(token->ErrStr(), "Expected end game object id");
		m_abort_parse = TRUE;
		return FALSE;
	}

	token->GetString(str);
	if(!g_theStringDB->GetStringID(str, str_id)) {
		c3errors_ErrorDialog(token->ErrStr(), "%s is not in the string database", str);
		m_abort_parse = TRUE;
		return FALSE;
	}

	sint32 u;
	AddRec(str_id, u);
	if(!m_rec[index].ParseRecord(token)) {
		c3errors_ErrorDialog(token->ErrStr(), "Error parsing object %s", str);
		m_abort_parse = TRUE;
		return FALSE;
	}
	return TRUE;
}

