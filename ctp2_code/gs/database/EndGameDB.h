#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ENDGAME_DB_H__
#define __ENDGAME_DB_H__

#include "Rec.h"
#include "DB.h"
#include "EndGameRecord.h"
#include "c3files.h"
#include "Token.h"

class EndGameDatabase : public Database<EndGameRecord> {
private:
	BOOL m_abort_parse;

public:
	sint32 m_numStages;

	EndGameDatabase();
	EndGameDatabase(CivArchive &archive);
	void Serialize(CivArchive &archive);

	BOOL Initialize(char *filename, C3DIR dir);
	BOOL ParseAnEndGameObject(Token *token, sint32 index);

	sint32 GetNumStages() const { return m_numStages; }

};

extern EndGameDatabase *g_theEndGameDB;

#endif
