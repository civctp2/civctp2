#include "c3.h"

#include "thronedb.h"

#include "c3errors.h"
#include "Token.h"
#include "ErrMsg.h"
#include "civarchive.h"
#include "SoundRecord.h"
#include "c3files.h"
#include "StrDB.h"

extern sint32	g_abort_parse;
extern StringDB	*g_theStringDB;

enum TOKEN_THRONE
{
	TOKEN_THRONE_IS_CEILING = TOKEN_MAX + 1,
	TOKEN_THRONE_CAN_ZOOM,
	TOKEN_THRONE_UPGRADE_SOUND,
	TOKEN_THRONE_UPGRADE_TEXT,
	TOKEN_THRONE_IMAGE_ZOOMED,
	TOKEN_THRONE_MAX
};

TokenData s_ThroneDB_token_data[] = {
	{ TOKEN_THRONE_IS_CEILING,		"THRONE_IS_CEILING" },
	{ TOKEN_THRONE_CAN_ZOOM,		"THRONE_CAN_ZOOM" },
	{ TOKEN_THRONE_UPGRADE_SOUND,	"THRONE_UPGRADE_SOUND" },
	{ TOKEN_THRONE_UPGRADE_TEXT,	"THRONE_UPGRADE_TEXT" },
	{ TOKEN_THRONE_IMAGE_ZOOMED,	"THRONE_IMAGE_ZOOMED" }
};

ThroneDB::ThroneDB()
{

	Initialize();
}

ThroneDB::ThroneDB(CivArchive &archive)
{

	Initialize();

	Serialize(archive);
}

ThroneDB::~ThroneDB()
{

	if(m_throneInfo) delete [] m_throneInfo;
}

void ThroneDB::Initialize(void)
{

	m_nThroneTypes = 0;
	m_nThroneLevels = 0;
	m_throneInfo = NULL;
}

sint32 ThroneDB::Init(MBCHAR *filename)
{

	if(!ParseThroneDatabase(filename)) return(FALSE);

	return(TRUE);
}

ThroneInfo *ThroneDB::GetThroneInfo( sint32 type, sint32 level ) const
{

	sint32 index = (type * m_nThroneLevels) + level;

	return(&(m_throneInfo[index]));
}

sint32 ThroneDB::ParseNumber(Token *token, sint32 *number)
{

	sint32 n;

	if (token->GetType() != TOKEN_NUMBER) {
		c3errors_ErrorDialog(token->ErrStr(), "Missing number from ThroneDB");
		g_abort_parse = TRUE;
		return(FALSE);
	}

	token->GetNumber(n);
	token->Next();

	if (n < 0) {
		c3errors_ErrorDialog(token->ErrStr(), "Number in ThroneDB is negative");
		g_abort_parse = TRUE;
		return FALSE;
	} else if (n == 0) {
		c3errors_ErrorDialog(token->ErrStr(), "Number in ThroneDB is zero");
		g_abort_parse = TRUE;
		return TRUE;
	}

	*number = n;

	return(TRUE);
}

sint32 ThroneDB::ParseThroneDatabase(MBCHAR *filename)
{

	sint32 nThrones = 0;
	sint32 index = 0;

	Token *throneToken = new Token(filename, TOKEN_THRONE_MAX - TOKEN_MAX, s_ThroneDB_token_data, C3DIR_GAMEDATA);

	Assert(throneToken);

	if(!ParseNumber(throneToken, &m_nThroneTypes)) goto operation_failed;
	if(!ParseNumber(throneToken, &m_nThroneLevels)) goto operation_failed;

	nThrones = m_nThroneTypes * m_nThroneLevels;

	m_throneInfo = new ThroneInfo[nThrones];

	Assert(m_throneInfo);

	for(index = 0; index < nThrones; index++) {
		if(!ParseAThrone(throneToken, &m_throneInfo[index])) goto operation_failed;
	}

operation_failed:

	delete throneToken;

	if (g_abort_parse) return FALSE;

	return TRUE;
}

sint32 ThroneDB::CheckToken(Token *token, sint32 type, MBCHAR *error)
{

	if(token->Next() != type) {
		c3errors_ErrorDialog(token->ErrStr(), error);
		g_abort_parse = TRUE;
		return FALSE;
	}

	return(TRUE);
}

sint32 ThroneDB::ParseAThrone(Token *throneToken, ThroneInfo *throneInfo)
{

	char str[_MAX_PATH];
	StringId str_id;
	sint32 snd_id;

	if(throneToken->GetType() == TOKEN_EOF) return(FALSE);

	if (throneToken->GetType() != TOKEN_STRING) {
		c3errors_ErrorDialog  (throneToken->ErrStr(), "Throne id expected");
        g_abort_parse = TRUE;
		return FALSE;
	}

	throneToken->GetString(str);

	if(!g_theStringDB->GetStringID(str, str_id)) {
		c3errors_ErrorDialog (throneToken->ErrStr(), "Could not find %s in string database", str);
		g_abort_parse = TRUE;
		return FALSE;
	}

	throneInfo->m_name = str_id;

	if(throneToken->Next() != TOKEN_OPEN_BRACE) {
		c3errors_ErrorDialog  (throneToken->ErrStr(), "Missing open brace");
        g_abort_parse = TRUE;
		return FALSE;
	}

	strcpy(throneInfo->m_zoomedImageFilename, "");
	throneInfo->m_text = -1;
	throneInfo->m_upgradeSoundID = -1;
	throneInfo->m_isCeiling = false;
	throneInfo->m_canZoom = false;

	while(throneToken->Next() != TOKEN_CLOSE_BRACE) {
		switch(throneToken->GetType()) {
		case TOKEN_THRONE_UPGRADE_SOUND:

			if(!CheckToken(throneToken, TOKEN_STRING, "Upgrade sound ID missing")) return(FALSE);
			throneToken->GetString(str);

			snd_id = g_theSoundDB->FindTypeIndex(str);
			if(snd_id == -1) {
				c3errors_ErrorDialog (throneToken->ErrStr(), "Could not find %s in sound database", str);
				g_abort_parse = TRUE;
				return FALSE;
			}

			throneInfo->m_upgradeSoundID = snd_id;
			break;
		case TOKEN_THRONE_UPGRADE_TEXT:

			if(!CheckToken(throneToken, TOKEN_STRING, "Upgrade sound ID missing")) return(FALSE);
			throneToken->GetString(str);

			if(!g_theStringDB->GetStringID(str, str_id)) {
				c3errors_ErrorDialog (throneToken->ErrStr(), "Could not find %s in string database", str);
				g_abort_parse = TRUE;
				return FALSE;
			}

			throneInfo->m_text = str_id;
			break;
		case TOKEN_THRONE_IMAGE_ZOOMED:

			if(!CheckToken(throneToken, TOKEN_QUOTED_STRING, "Zoomed image filename expected"))
				return(FALSE);
			throneToken->GetString( str );
			strcpy( throneInfo->m_zoomedImageFilename, str );
			break;
		case TOKEN_THRONE_IS_CEILING:
			throneInfo->m_isCeiling = true;
			break;
		case TOKEN_THRONE_CAN_ZOOM:
			throneInfo->m_canZoom = true;
			break;
		default:
			c3errors_ErrorDialog(throneToken->ErrStr(), "Unexpected token before close brace.");
		    g_abort_parse = TRUE;
			return(FALSE);
		}
	}

	throneToken->Next();

	return TRUE;
}

void ThroneDB::Serialize(CivArchive &archive)
{

	CHECKSERIALIZE;

	if (archive.IsStoring()) {

		archive << m_nThroneTypes;
		archive << m_nThroneLevels;

    	archive.Store((uint8 *)m_throneInfo, sizeof(ThroneInfo) * m_nThroneTypes * m_nThroneLevels) ;
	} else {

		archive >> m_nThroneTypes;
		archive >> m_nThroneLevels;

		if(m_throneInfo) delete m_throneInfo;

		m_throneInfo = new ThroneInfo[(m_nThroneTypes * m_nThroneLevels)];

        archive.Load((uint8 *)m_throneInfo, sizeof(ThroneInfo) * m_nThroneTypes * m_nThroneLevels) ;
	}
}
