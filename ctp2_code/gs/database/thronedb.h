#ifdef HAVE_PRAGMA_ONCE
#pragma once
#endif
#ifndef _BMH_THRONE_DB_H_
#define _BMH_THRONE_DB_H_

#include "dbtypes.h"

class CivArchive;
class Token;

struct ThroneInfo {

	StringId		m_name;

	StringId		m_text;

	sint32			m_upgradeSoundID;


	MBCHAR			m_zoomedImageFilename[_MAX_PATH];

	bool			m_canZoom;


	bool			m_isCeiling;
};

class ThroneDB {
public:

	sint32		m_nThroneTypes;
	sint32		m_nThroneLevels;
	ThroneInfo	*m_throneInfo;

	ThroneDB();

	ThroneDB(CivArchive &archive);

	~ThroneDB();

	void Initialize(void);

	sint32 Init(MBCHAR *filename);

	ThroneInfo *GetThroneInfo( sint32 type, sint32 level ) const;

	void Serialize(CivArchive &archive);

protected:

	sint32 ParseNumber(Token *token, sint32 *number);

	sint32 ParseThroneDatabase(const MBCHAR *filename);

	sint32 CheckToken(Token *token, sint32 type, const MBCHAR *error);

	sint32 ParseAThrone(Token *throneToken, ThroneInfo *throneInfo);
};


extern ThroneDB *g_theThroneDB;

#endif
