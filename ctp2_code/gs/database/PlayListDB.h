
#ifndef __PLAYLISTDB_H__
#define __PLAYLISTDB_H__

class CivArchive;

class PlayListDB 
{
public:
	PlayListDB();
	PlayListDB(CivArchive &archive);
	~PlayListDB();

	BOOL Parse(MBCHAR *filename);

	void Serialize(CivArchive &archive);

	sint32		GetNumSongs(void) { return m_numSongs; }
	sint32		GetSong(sint32 songNum) { return m_playList[songNum]; }

private:
	sint32		m_numSongs;
	sint32		*m_playList;
};

#endif
