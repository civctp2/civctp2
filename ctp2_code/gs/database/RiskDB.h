#pragma once
#ifndef __RISKDB_H__
#define __RISKDB_H__
#include "DB.h"
#include "Rec.h"

class CivArchive;
class Token;
enum C3DIR;

#include "RiskRecord.h"

class RiskDatabase : public Database<RiskRecord> 
{
public:
	RiskDatabase();
	RiskDatabase(CivArchive &archive);

	BOOL Initialize(char *filename, C3DIR dir);
	void Serialize(CivArchive &archive);

private:
	BOOL m_abort_parse;

	BOOL ParseRiskDatabase(char *filename, C3DIR dir);
	BOOL ParseANumber(Token *token, sint32 &val);
	BOOL ParseAFloat(Token *token, double &val);
	BOOL ParseARisk(Token *token);
	BOOL ParseAnAttribute(Token *token, RiskRecord *rec);
};

extern RiskDatabase *g_theRiskDB;
#endif
