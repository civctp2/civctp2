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
// ACTIVISION_ORIGINAL		
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
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
	return;
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


aui_Redbook::aui_Redbook() :
m_cd_device_id(-1),
m_cd_drive_letter(0),
m_cd_drive_index(0),
m_aux_cdrom_id(-1),
m_cd_ok(FALSE)
{
	return;
}


aui_Redbook::~aui_Redbook()
{
	m_cd_device_id = -1;
	m_cd_drive_letter = 0;
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
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	
	retval = Stop();
	retval = CDDone();

	return retval;
}


AUI_MUSIC_ERRCODE
aui_Redbook::Pause()
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	MCI_GENERIC_PARMS mciPauseParms;

	mci_retval = mciSendCommand( m_cd_device_id, MCI_PAUSE, 0, (DWORD)&mciPauseParms );
	retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
	return retval;
}


AUI_MUSIC_ERRCODE
aui_Redbook::Resume()
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	MCI_GENERIC_PARMS mciResumeParms;

	mci_retval = mciSendCommand( m_cd_device_id, MCI_RESUME, 0, (DWORD)&mciResumeParms );
	retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
	return retval;

}


AUI_MUSIC_ERRCODE
aui_Redbook::Stop()
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	MCI_GENERIC_PARMS mciStopParms;

	mci_retval = mciSendCommand( m_cd_device_id, MCI_STOP, 0, (DWORD)&mciStopParms );
	retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
	return retval;
}


AUI_MUSIC_ERRCODE
aui_Redbook::Play( sint32 itrack )
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	int playFlags = MCI_FROM;
	MCI_PLAY_PARMS mciPlayParms;

	
	if( (itrack < m_first_track) || (itrack > m_last_track) )
		return AUI_MUSIC_ERRCODE_INVALID_TRACKNUMBER;

	mciPlayParms.dwFrom = (ULONG)itrack;

	mci_retval = mciSendCommand( m_cd_device_id, MCI_PLAY, playFlags, (DWORD)&mciPlayParms );
	if( mci_retval == MMSYSERR_NOERROR)
		m_ctrack = itrack;

	retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
	return retval;
}


AUI_MUSIC_ERRCODE
aui_Redbook::Play()
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	MCI_PLAY_PARMS mciPlayParms;

	mci_retval = mciSendCommand( m_cd_device_id, MCI_PLAY, 0, (DWORD)&mciPlayParms ); 
	retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
	return retval;
}


AUI_MUSIC_ERRCODE
aui_Redbook::SetVolume( uint8 volume )
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	DWORD CDvolume = 0;
	m_volume = std::min(volume, (unsigned char) 0xff);
	
	CDvolume = (DWORD)( ( ( m_volume + 1 ) << 8 ) - 1 );	
	CDvolume |= ( CDvolume << 16 );								

	if( m_aux_cdrom_id != -1 )
	{
		mci_retval = auxSetVolume( m_aux_cdrom_id, CDvolume );
		retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
	}
	else
	{
		retval = AUI_MUSIC_AUXCDROMID_INVALID;
	}

	return retval;
}



sint32
aui_Redbook::CDInitVolume()
{
	int     HowManyAuxDevices;
   BOOL    FoundCDRom = FALSE;
   int     i;
   AUXCAPS AuxDeviceCapabilities;
   int     CDRomID = -1;

   
   
   
	
	
	HowManyAuxDevices = auxGetNumDevs();

	
	if(HowManyAuxDevices > 0)
	{
		
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
	LPSTR driveMap;
	LPSTR driveName;
	
	
	
	driveMap = (LPSTR)calloc(105, sizeof(char));	

	GetLogicalDriveStrings(105, driveMap);

	driveName = driveMap;
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
		m_cd_drive_letter = driveName[0];
	else
		m_cd_drive_index = 0;

	free(driveMap); 

	return AUI_MUSIC_ERRCODE_OK;
}


AUI_MUSIC_ERRCODE
aui_Redbook::CDDone()
{
	AUI_MUSIC_ERRCODE retval = AUI_MUSIC_ERRCODE_OK;
	sint32 mci_retval = 0;
	MCI_GENERIC_PARMS mciCloseParms;

	mci_retval = mciSendCommand( m_cd_device_id, MCI_CLOSE, 0, (DWORD)(&mciCloseParms) );
	m_cd_device_id = -1;

	retval = mci_retval? AUI_MUSIC_ERRCODE_MCIERR:AUI_MUSIC_ERRCODE_OK;
  	return retval;
}



AUI_MUSIC_CODE
aui_Redbook::Status()
{
	sint32 mci_retval = 0;
	MCI_STATUS_PARMS mciStatusParms;

	if (m_cd_ok)
	{
		mciStatusParms.dwItem = MCI_STATUS_MODE;
		mci_retval = mciSendCommand( m_cd_device_id, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)&mciStatusParms );
	   if( mci_retval )
	   {
			CDDone();
			m_cd_ok = 0;
	      return( AUI_MUSIC_CODE_UNKNOWN );
	   }

	   switch ( mciStatusParms.dwReturn )
		{
			case MCI_MODE_PAUSE:		
				return( AUI_MUSIC_CODE_PAUSE ); 
			case MCI_MODE_PLAY:			
				return( AUI_MUSIC_CODE_PLAY );
			case MCI_MODE_STOP:			
				return( AUI_MUSIC_CODE_STOP );
			case MCI_MODE_OPEN:			
				return( AUI_MUSIC_CODE_EMPTY );

			case MCI_MODE_RECORD:
			case MCI_MODE_SEEK:
			case MCI_MODE_NOT_READY:
			default:					
				return( AUI_MUSIC_CODE_UNKNOWN );
		}
	}
	else
		return( AUI_MUSIC_CODE_UNKNOWN );
}
