
#include "c3.h"
#include "OrderDB.h"
#include "Order.h"
#include "Token.h"
#include "SoundRecord.h"
#include "c3files.h"

static UNIT_ORDER_TYPE FindRecord(char *name, sint32 &index)
{
	sint32 i;
	for(i = 0; i < g_numOrderInfo; i++) {
		if(strcmp(g_orderInfo[i].m_name, name) == 0) {
			index = i;
			return g_orderInfo[i].m_type;
		}
	}
	index = 0;
	return UNIT_ORDER_NONE;
}

static sint32 FindRecordIndex(UNIT_ORDER_TYPE type)
{
	sint32 i;
	for(i = 0; i < g_numOrderInfo; i++) {
		if(g_orderInfo[i].m_type == type)
			return i;
	}
	return -1;
}

BOOL orderDB_Parse(char *filename)
{
	BOOL abort = FALSE;
	Token *token = new Token(filename, C3DIR_GAMEDATA);
	char str[k_MAX_NAME_LEN];
	UNIT_ORDER_TYPE order;
	sint32 index;

	sint32 i;

	for(i = 0; i < sint32(UNIT_ORDER_MAX); i++) {
		g_orderInfoMap[i] = FindRecordIndex(UNIT_ORDER_TYPE(i));
	}

	while(token->GetType() != TOKEN_EOF && !abort) {
		if(token->GetType() != TOKEN_OPEN_BRACE) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Missing open brace");
			break;
		}
		token->Next();
		if(token->GetType() != TOKEN_STRING) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected string");
			break;
		}
		token->GetString(str);
		order = FindRecord(str, index);
		if(order == UNIT_ORDER_NONE) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Order %s is unknown", str);
			break;
		}
		
		OrderInfo *oi = &g_orderInfo[order];
		if(token->Next() != TOKEN_NUMBER) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected gold cost");
			break;
		}
		token->GetNumber(oi->m_goldCost);

		if(token->Next() != TOKEN_NUMBER) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected move cost");
			break;
		}
		token->GetNumber(oi->m_moveCost);

		if(token->Next() != TOKEN_STRING) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected failure sound name");
			break;
		}
		token->GetString(str);
		if(strcmp(str, "NONE")) {
			oi->m_failSound = g_theSoundDB->FindTypeIndex(str);
			if(oi->m_failSound < 0) {
				abort = TRUE;
				c3errors_ErrorDialog(token->ErrStr(), "Sound %s not found in sound database", str);
				break;
			}
		} else {
			oi->m_failSound = -1;
		}

		if(token->Next() != TOKEN_STRING) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected success sound name");
			break;
		}
		token->GetString(str);
		if(strcmp(str, "NONE")) {
			oi->m_workSound = g_theSoundDB->FindTypeIndex(str);
			if(oi->m_workSound < 0) {
				abort = TRUE;
				c3errors_ErrorDialog(token->ErrStr(), "Sound %s not found in sound databse", str);
				break;
			}
		} else {
			oi->m_workSound = -1;
		}
		if(token->Next() != TOKEN_CLOSE_BRACE) {
			abort = TRUE;
			c3errors_ErrorDialog(token->ErrStr(), "Expected closing brace");
			break;
		}
		g_orderInfoMap[oi->m_type] = index;
		token->Next();
	}

	delete token;
	return !abort;
}

