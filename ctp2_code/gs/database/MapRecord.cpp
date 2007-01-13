#pragma once

#include "c3.h"
#include "MapRecord.h"
#include "Token.h"
#include "MapToken.h"

extern TokenData g_mapTokenData[TOKEN_MAP_MAX_VAL];

BOOL MapRecord::Parse(Token *token)
{
	m_numLevels =0;
	while(m_numLevels < k_MAP_LEVELS) {
		if(token->Next() != TOKEN_OPEN_BRACE) {
			if(token->GetType() == TOKEN_CLOSE_BRACE)
				return TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected open brace");
			return FALSE;
		}
		sint32 t;
		for(t = sint32(TOKEN_MAP_NUM_CONTINENTS); 
			t < sint32(TOKEN_MAP_MAX_VAL); t++) {
			if(!token_ParseFloatNext(token, t, m_settings[m_numLevels][t - sint32(TOKEN_MAP_NUM_CONTINENTS)])) {
				c3errors_ErrorDialog(token->ErrStr(), "Expected %s", g_mapTokenData[t].keyword);
				return FALSE;
			}
		}
		if(token->Next() != TOKEN_CLOSE_BRACE) {
			c3errors_ErrorDialog(token->ErrStr(), "Expected close brace");
			return FALSE;
		}
		m_numLevels++;
	}
	token->Next();
	return TRUE;
}

const double *MapRecord::GetSettings(sint32 level, sint32 &numSettings) const
{
	if(level < 0) {
		level = 0;
	} else if(level >= m_numLevels) {
		level = m_numLevels;
	}
	
	numSettings = k_NUM_MAP_VARS;

	return &m_settings[level][0];
}
