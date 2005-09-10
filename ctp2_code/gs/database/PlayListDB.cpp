//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Playlist database
// Id           : $Id:$
//
//----------------------------------------------------------------------------
//
// Disclaimer
//
// THIS FILE IS NOT GENERATED OR SUPPORTED BY ACTIVISION.
//
// This material has been developed at apolyton.net by the Apolyton CtP2 
// Source Code Project. Contact the authors at ctp2source@apolyton.net.
//
//----------------------------------------------------------------------------
//
// Compiler flags
//
// - None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "Token.h"
#include "civarchive.h"

#include "PlayListDB.h"
#include "c3files.h"

enum TOKEN_PLAYLIST {
	TOKEN_PLAYLIST_NUM_SONGS = TOKEN_MAX+1,
	TOKEN_PLAYLIST_SONG_LIST,

	TOKEN_PLAYLIST_MAX
};

TokenData g_playlist_token_data [TOKEN_PLAYLIST_MAX - TOKEN_MAX] = { 
    { TOKEN_PLAYLIST_NUM_SONGS,		"NUM_SONGS"},
	{ TOKEN_PLAYLIST_SONG_LIST,		"SONG_LIST"},
};

PlayListDB::PlayListDB()
{
	m_numSongs = 0;
	m_playList = NULL;
}

PlayListDB::PlayListDB(CivArchive &archive)
{
	m_numSongs = 0;
	m_playList = NULL;

	Serialize(archive);
}

PlayListDB::~PlayListDB()
{
	if (m_playList)
		delete m_playList;
}

BOOL PlayListDB::Parse(MBCHAR *filename)
{
	Token *playListToken = new Token(filename, TOKEN_PLAYLIST_MAX - TOKEN_MAX,
									g_playlist_token_data, C3DIR_GAMEDATA);

	Assert(playListToken);
	if (!playListToken) return FALSE;

	sint32		val = 0;

	if (playListToken->GetType() == TOKEN_PLAYLIST_NUM_SONGS) { 
		if (playListToken->Next() == TOKEN_NUMBER) { 
			playListToken->GetNumber(val); 
		}
	}

	m_numSongs = val;
	m_playList = new sint32[m_numSongs];

	if (playListToken->Next() == TOKEN_PLAYLIST_SONG_LIST) { 
		for (sint32 i=0; i<m_numSongs; i++) {
			if (playListToken->Next() == TOKEN_NUMBER) { 
				playListToken->GetNumber(val); 
				m_playList[i] = val;
			} else {
				c3errors_ErrorDialog("PlayListDB", "Looking for a song number.");
				delete playListToken;
				return FALSE;
			}
		}
	} else {
		c3errors_ErrorDialog("PlayListDB", "Missing token SONG_LIST.");
		delete playListToken;
		return FALSE;
	}

	delete playListToken;

	return TRUE;
}

void PlayListDB::Serialize(CivArchive &archive)
{
	sint32		i;

	if (archive.IsStoring()) {
		archive << m_numSongs;
		for (i=0; i<m_numSongs; i++) {
			archive << m_playList[i];
		}
	} else {
		archive >> m_numSongs;
		m_playList = new sint32[m_numSongs];
		for (i=0; i<m_numSongs; i++) {
			archive >> m_playList[i];
		}
	}
}
