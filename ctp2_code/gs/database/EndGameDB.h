#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef __ENDGAME_DB_H__
#define __ENDGAME_DB_H__

#include "gs/database/Rec.h"
#include "gs/database/DB.h"
#include "gs/database/EndGameRecord.h"
#include "ctp/ctp2_utils/c3files.h"
#include "gs/fileio/Token.h"

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
