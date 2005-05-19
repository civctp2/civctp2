

#include "c3.h"
#include "c3errors.h"
#include "MapDB.h"
#include "Token.h"
#include "civarchive.h"
#include "StrDB.h"
#include "MapToken.h"

TokenData g_mapTokenData[TOKEN_MAP_MAX_VAL] = {
	{TOKEN_MAP_SIZE, "MAP_SIZE"},
	{TOKEN_MAP_NUM_CONTINENTS, "MAP_NUM_CONTINENTS"},
	{TOKEN_MAP_MIN_WIDTH, "MAP_MIN_WIDTH"},
	{TOKEN_MAP_MIN_HEIGHT, "MAP_MIN_HEIGHT"},
	{TOKEN_MAP_MAX_WIDTH, "MAP_MAX_WIDTH"},
	{TOKEN_MAP_MAX_HEIGHT, "MAP_MAX_HEIGHT"},
	{TOKEN_MAP_BUMP_SIZE, "MAP_BUMP_SIZE"},
	{TOKEN_MAP_MIN_SUBCONTINENTS, "MAP_MIN_SUBCONTINENTS"},
	{TOKEN_MAP_MAX_SUBCONTINENTS, "MAP_MAX_SUBCONTINENTS"},
	{TOKEN_MAP_SUBCONTINENT_SIZE, "MAP_SUBCONTINENT_SIZE"},
	{TOKEN_MAP_SUBCONTINENT_HEIGHT, "MAP_SUBCONTINENT_HEIGHT"},
	{TOKEN_MAP_BORDER_SIZE, "MAP_BORDER_SIZE"},
	{TOKEN_MAP_FAULT_LINES, "MAP_FAULT_LINES"},
	{TOKEN_MAP_FAULT_LENGTH, "MAP_FAULT_LENGTH"},
	{TOKEN_MAP_NS_BORDER_SIZE, "MAP_NS_BORDER_SIZE"},
};

MapDatabase *g_theMapDB;

MapDatabase::MapDatabase()
{
}

BOOL MapDatabase::Initialize(char *filename, C3DIR dir)
{
	Token *token = new Token(filename,
							 TOKEN_MAP_MAX_VAL - TOKEN_MAX,
							 g_mapTokenData, dir);

	BOOL done = FALSE;

	sint32 x, y;

	while(!done) {
		if(token->GetType() != TOKEN_MAP_SIZE) {
			if(token->GetType() == TOKEN_EOF) {
				delete token;
				return TRUE;
			}
			c3errors_ErrorDialog(token->ErrStr(), "Expected MAP_SIZE");
			delete token;
			return FALSE;
		}
		if(token->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(token->ErrStr(), "Expected width");
			delete token;
			return FALSE;
		}
		token->GetNumber(x);
		
		if(token->Next() != TOKEN_NUMBER) {
			c3errors_ErrorDialog(token->ErrStr(), "Expected height");
			delete token;
			return FALSE;
		}
		token->GetNumber(y);

		if(token->Next() != TOKEN_OPEN_BRACE) {
			c3errors_ErrorDialog(token->ErrStr(), "Expected open brace");
			delete token;
			return FALSE;
		}
		SetSize(m_nRec + 1);
		sint32 u;
		AddRec(0, u);
		Access(u)->SetSize(x, y);
		if(!Access(u)->Parse(token)) {
			delete token;
			return FALSE;
		}
		if(token->GetType() != TOKEN_CLOSE_BRACE) {
			c3errors_ErrorDialog(token->ErrStr(), "Expected close brace");
			delete token;
			return FALSE;
		}
		token->Next();
	}
	delete token;
	return TRUE;
}

const MapRecord *MapDatabase::FindBestMapSizeMatch(sint32 w, sint32 h)
{
	sint32 area, actualarea = w * h;
	sint32 closestArea = -1;
	sint32 closestRec = -1;

	for(sint32 i = 0; i < m_nRec; i++) {
		area = Get(i)->GetWidth() * Get(i)->GetHeight();
		if(closestArea < 0 || abs(area - actualarea) < closestArea) {
			closestArea = abs(area - actualarea);
			closestRec = i;
		}
	}
	if(closestRec < 0)
		closestRec = 0;
	return &m_rec[closestRec];
}

