#ifndef __AUI_MUSIC_H__
#define __AUI_MUSIC_H__

#include "aui_base.h"

enum AUI_MUSIC_CODE
{
	AUI_MUSIC_CODE_UNKNOWN = -1,
	AUI_MUSIC_CODE_EMPTY,
	AUI_MUSIC_CODE_PAUSE,
	AUI_MUSIC_CODE_PLAY,
	AUI_MUSIC_CODE_STOP,

	AUI_MUSIC_CODE_COUNT
};

enum AUI_MUSIC_ERRCODE
{
	AUI_MUSIC_ERRCODE_UNKNOWN = -1,
	AUI_MUSIC_ERRCODE_OK = 0,

	AUI_MUSIC_ERRCODE_NOCDDRIVE,
	AUI_MUSIC_ERRCODE_SYSINFOERROR,
	AUI_MUSIC_ERRCODE_INVALID_DRIVE_INDEX,
	AUI_MUSIC_ERRCODE_NODEVICE_NAME,
	AUI_MUSIC_ERRCODE_OPENERR,
	AUI_MUSIC_ERRCODE_FORMATTINGERR,
	AUI_MUSIC_ERRCODE_STATUSERR,
	AUI_MUSIC_ERRCODE_INVALID_TRACKNUMBER,
	AUI_MUSIC_AUXCDROMID_INVALID,
	AUI_MUSIC_ERRCODE_MCIERR,

	AUI_MUSIC_ERRCODE_COUNT
};

class aui_Music : public aui_Base
{
public:
	aui_Music();
	~aui_Music();

	virtual AUI_MUSIC_ERRCODE Init() { return AUI_MUSIC_ERRCODE_OK; }
	virtual AUI_MUSIC_ERRCODE Close() { return AUI_MUSIC_ERRCODE_OK; }
	virtual AUI_MUSIC_ERRCODE Pause() { return AUI_MUSIC_ERRCODE_OK; }
	virtual AUI_MUSIC_ERRCODE Resume() { return AUI_MUSIC_ERRCODE_OK; }
	virtual AUI_MUSIC_ERRCODE Stop() { return AUI_MUSIC_ERRCODE_OK; }
	virtual AUI_MUSIC_ERRCODE Play() { return AUI_MUSIC_ERRCODE_OK; }
	virtual AUI_MUSIC_ERRCODE Play( sint32 itrack ) { return AUI_MUSIC_ERRCODE_OK; }

	virtual AUI_MUSIC_ERRCODE SetVolume( uint8 volume );
	virtual uint8 GetVolume() { return m_volume; }

	sint32 GetCurrentTrack() { return m_ctrack; }
	sint32 GetNumberOfTracks() { return m_ntracks; }
	BOOL IsPause() { return m_pause; }

	virtual AUI_MUSIC_CODE Status() { return AUI_MUSIC_CODE_UNKNOWN; }

protected:
	sint32 m_first_track;
	sint32 m_last_track;
	sint32 m_ntracks;
	sint32 m_ctrack;
	uint8 m_volume;
	BOOL m_pause;

};

#if !defined(USE_SDL)

class aui_Redbook : public aui_Music
{
public:
	aui_Redbook();
	virtual ~aui_Redbook();


	virtual AUI_MUSIC_ERRCODE Init();
	virtual AUI_MUSIC_ERRCODE Close();
	virtual AUI_MUSIC_ERRCODE Pause();
	virtual AUI_MUSIC_ERRCODE Resume();
	virtual AUI_MUSIC_ERRCODE Stop();
	virtual AUI_MUSIC_ERRCODE Play();
	virtual AUI_MUSIC_ERRCODE Play( sint32 itrack );
	virtual AUI_MUSIC_ERRCODE SetVolume( uint8 volume );
	virtual AUI_MUSIC_CODE Status();

protected:

	sint32 CDInitVolume();
	AUI_MUSIC_ERRCODE GetCDIndex();
	AUI_MUSIC_ERRCODE CDDone();

protected:

	sint32 m_cd_device_id;
	int    m_cd_drive_num;
	sint32 m_cd_drive_index;
	BOOL   m_cd_ok;
	sint32 m_aux_cdrom_id;

};

#endif // USE_SDL

#endif
