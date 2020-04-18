//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Music
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
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Standardised min/max usage.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "aui_music.h"


aui_Music::aui_Music() :
m_first_track(-1),
m_last_track(-1),
m_ntracks(-1),
m_ctrack(-1),
m_volume(0xff),
m_pause(FALSE)
{
}

aui_Music::~aui_Music()
{
	m_first_track = -1;
	m_last_track = -1;
	m_ntracks = -1;
	m_ctrack = -1;
	m_volume = 0xff;
	m_pause = FALSE;
}

AUI_MUSIC_ERRCODE
aui_Music::SetVolume( uint8 volume )
{
	m_volume = volume;
	return AUI_MUSIC_ERRCODE_OK;
}

#if !defined(USE_SDL)

aui_Redbook::aui_Redbook() :
m_cd_device_id(-1),
m_cd_drive_num(0),
m_cd_drive_index(0),
m_cd_ok(FALSE),
m_aux_cdrom_id(-1)
{
	return;
}

aui_Redbook::~aui_Redbook()
{
	m_cd_device_id = -1;
	m_cd_drive_num = 0;
	m_cd_drive_index = 0;
	m_aux_cdrom_id = -1;
	m_cd_ok = 0;
}

AUI_MUSIC_ERRCODE
aui_Redbook::Init()
{
	MCIERROR returnVal;
	MCI_OPEN_PARMS mciOpenParms;
	MCI_SET_PARMS mciSetParms;
	MCI_SYSINFO_PARMS mciSysInfoParms;
	MCI_STATUS_PARMS mciStatusParms;
	long numCDDrives;
	char devName[32];

	GetCDIndex();

	if( m_cd_drive_index == 0 )
		return AUI_MUSIC_ERRCODE_NOCDDRIVE;

	mciSysInfoParms.wDeviceType = MCI_DEVTYPE_CD_AUDIO;
	mciSysInfoParms.lpstrReturn = (LPSTR)&numCDDrives;
	mciSysInfoParms.dwRetSize = sizeof(numCDDrives);
	returnVal = mciSendCommand(0, MCI_SYSINFO, MCI_SYSINFO_QUANTITY, (DWORD)(&mciSysInfoParms));

	if( returnVal != 0 )
		return AUI_MUSIC_ERRCODE_SYSINFOERROR;

	if( m_cd_drive_index > numCDDrives )
		return AUI_MUSIC_ERRCODE_INVALID_DRIVE_INDEX;

	mciSysInfoParms.dwNumber = m_cd_drive_index;
	mciSysInfoParms.lpstrReturn = devName;
	mciSysInfoParms.dwRetSize = sizeof(devName);
	returnVal = mciSendCommand(0, MCI_SYSINFO, MCI_SYSINFO_NAME, (DWORD)(&mciSysInfoParms));

	if( returnVal != 0 )
	{
		mciGetErrorString(returnVal, devName, sizeof(devName));
		return AUI_MUSIC_ERRCODE_NODEVICE_NAME;
	}

	mciOpenParms.lpstrDeviceType = devName;
	returnVal = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)(&mciOpenParms));
 	if( returnVal != 0 )
		return AUI_MUSIC_ERRCODE_OPENERR;

	m_cd_device_id = mciOpenParms.wDeviceID;

	mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
	if( mciSendCommand( m_cd_device_id, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)&mciSetParms) )
	{
		CDDone();
		return AUI_MUSIC_ERRCODE_FORMATTINGERR;
	}

	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if( mciSendCommand( m_cd_device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatusParms) )
	{
		CDDone();
		return AUI_MUSIC_ERRCODE_STATUSERR;
	}

	m_ntracks = mciStatusParms.dwReturn;
	m_first_track = 1;
	m_last_track = m_ntracks;

	m_aux_cdrom_id = CDInitVolume();

	m_cd_ok = TRUE;
	return AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::Close()
{
	Stop();
	return CDDone();
}

AUI_MUSIC_ERRCODE
aui_Redbook::Pause()
{
	MCI_GENERIC_PARMS mciPauseParms;

	sint32 mci_retval = mciSendCommand( m_cd_device_id, MCI_PAUSE, 0, (DWORD)&mciPauseParms );
	return mci_retval ? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::Resume()
{
	MCI_GENERIC_PARMS mciResumeParms;

	sint32 mci_retval = mciSendCommand( m_cd_device_id, MCI_RESUME, 0, (DWORD)&mciResumeParms );
	return mci_retval ? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::Stop()
{
	MCI_GENERIC_PARMS mciStopParms;

	sint32 mci_retval = mciSendCommand( m_cd_device_id, MCI_STOP, 0, (DWORD)&mciStopParms );
	return mci_retval ? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::Play( sint32 itrack )
{
	if( (itrack < m_first_track) || (itrack > m_last_track) )
		return AUI_MUSIC_ERRCODE_INVALID_TRACKNUMBER;

	int playFlags = MCI_FROM;
	MCI_PLAY_PARMS mciPlayParms;
	mciPlayParms.dwFrom = (ULONG)itrack;

	sint32 mci_retval = mciSendCommand( m_cd_device_id, MCI_PLAY, playFlags, (DWORD)&mciPlayParms );
	if( mci_retval == MMSYSERR_NOERROR)
		m_ctrack = itrack;

	return mci_retval ? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::Play()
{
	MCI_PLAY_PARMS mciPlayParms;

	sint32 mci_retval = mciSendCommand(m_cd_device_id, MCI_PLAY, 0, (DWORD)&mciPlayParms);

	return mci_retval ? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::SetVolume( uint8 volume )
{
	m_volume = volume;

	if (m_aux_cdrom_id == -1)
	{
		return AUI_MUSIC_AUXCDROMID_INVALID;
	}

	DWORD CDvolume = (DWORD)( ( ( m_volume + 1 ) << 8 ) - 1 );
	CDvolume |= ( CDvolume << 16 );

	sint32 mci_retval = auxSetVolume(m_aux_cdrom_id, CDvolume);
	return mci_retval ? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}

sint32
aui_Redbook::CDInitVolume()
{
	int     CDRomID = -1;
	int     HowManyAuxDevices = auxGetNumDevs();

	if (HowManyAuxDevices > 0)
	{
		BOOL    FoundCDRom = FALSE;
		int     i;
		AUXCAPS AuxDeviceCapabilities;

		for( i=0; (i<HowManyAuxDevices) && (FoundCDRom == FALSE); i++ )
		{
			if( auxGetDevCaps(i,
					&AuxDeviceCapabilities,
					sizeof(AUXCAPS) ) )
			{
				return( -1 );
			}

			if( AuxDeviceCapabilities.wTechnology == AUXCAPS_CDAUDIO )
			{
				FoundCDRom = TRUE;
				CDRomID = i;
			}
		}

		if( FoundCDRom == FALSE )
		{
			for( i=0; (i<HowManyAuxDevices) && (FoundCDRom == FALSE); i++ )
			{

				if( auxGetDevCaps(i,
						&AuxDeviceCapabilities,
						sizeof(AUXCAPS) ) )
				{
					return( -1 );
				}

				if( AuxDeviceCapabilities.wTechnology == AUXCAPS_AUXIN )
				{
					FoundCDRom = TRUE;
					CDRomID = i;
				}
			}
		}
	}

	return( CDRomID );
}

AUI_MUSIC_ERRCODE
aui_Redbook::GetCDIndex()
{
	LPSTR   driveMap    = (LPSTR) calloc(105, sizeof(char));
	GetLogicalDriveStrings(105, driveMap);

	LPSTR   driveName   = driveMap;
	m_cd_drive_index = 0;

	while ( *driveName )
	{
		if ( GetDriveType(driveName) ==	DRIVE_CDROM )
		{
			m_cd_drive_index++;
			break ;
		}
		driveName += 4;
	}

	if ( *driveName )
		m_cd_drive_num = driveName[0];
	else
		m_cd_drive_index = 0;

	free(driveMap);

	return AUI_MUSIC_ERRCODE_OK;
}

AUI_MUSIC_ERRCODE
aui_Redbook::CDDone()
{
	MCI_GENERIC_PARMS mciCloseParms;

	sint32 mci_retval = mciSendCommand( m_cd_device_id, MCI_CLOSE, 0, (DWORD)(&mciCloseParms) );
	m_cd_device_id = -1;

	return mci_retval? AUI_MUSIC_ERRCODE_MCIERR : AUI_MUSIC_ERRCODE_OK;
}


AUI_MUSIC_CODE
aui_Redbook::Status()
{
	if (m_cd_ok)
	{
		MCI_STATUS_PARMS mciStatusParms;
		mciStatusParms.dwItem = MCI_STATUS_MODE;

		sint32 mci_retval = mciSendCommand( m_cd_device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatusParms );

		if (mci_retval)
		{
			CDDone();
			m_cd_ok = 0;
		}
		else
		{
			switch (mciStatusParms.dwReturn)
			{
			default:
//			case MCI_MODE_RECORD:
//			case MCI_MODE_SEEK:
//			case MCI_MODE_NOT_READY:
				break;

			case MCI_MODE_PAUSE:
				return AUI_MUSIC_CODE_PAUSE;
			case MCI_MODE_PLAY:
				return AUI_MUSIC_CODE_PLAY;
			case MCI_MODE_STOP:
				return AUI_MUSIC_CODE_STOP;
			case MCI_MODE_OPEN:
				return AUI_MUSIC_CODE_EMPTY;
			}
		}
	}

	return AUI_MUSIC_CODE_UNKNOWN;
}

#endif // USE_SDL
