










#include "c3.h"
#include "aui_ui.h"

#include "aui_sound.h"



aui_Sound::aui_Sound(
	AUI_ERRCODE *retval,
	MBCHAR *filename )
	:
	aui_Base()
{
	*retval = InitCommon ( filename );
	Assert ( AUI_SUCCESS (*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}



aui_Sound::~aui_Sound()
{
	Unload();
}



AUI_ERRCODE aui_Sound::InitCommon( MBCHAR *filename )
{
	m_format = NULL;
	m_data = NULL;

	AUI_ERRCODE errcode = SetFilename( filename );
	Assert ( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Sound::SetFilename( MBCHAR *filename)
{
	Unload();

	memset ( m_filename, '\0', sizeof( m_filename ));

	if ( !filename ) 
		return AUI_ERRCODE_INVALIDPARAM;

	strncpy ( m_filename, filename, MAX_PATH );

	m_format = (aui_SoundFormat *)
		g_ui->TheMemMap()->GetFileFormat ( m_filename );
	Assert( m_format != NULL );
	if ( !m_format )
		return AUI_ERRCODE_MEMALLOCFAILED;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_Sound::Load( void ) 
{
	Assert ( m_format != NULL );
	if ( !m_format ) return AUI_ERRCODE_INVALIDPARAM;

	if ( m_data ) return AUI_ERRCODE_OK;

	return m_format->LoadSoundData( m_filename, &m_data, &m_size );
}



AUI_ERRCODE aui_Sound::Unload( void ) 
{
	if ( m_format )
	{
		g_ui->TheMemMap()->ReleaseFileFormat( m_format );
		m_format = NULL;
	}

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE aui_WavSoundFormat::LoadSoundData(
	MBCHAR *filename,
	uint8 **wavdata,
	uint32 *size)
{
	m_data = (uint8 *)g_ui->TheMemMap()->GetFileBits( filename, size );
	*wavdata = m_data;

	Assert( *wavdata != NULL );
	if ( !*wavdata ) return AUI_ERRCODE_MEMALLOCFAILED;

	TrimWavHeader( wavdata, size ); 

	return AUI_ERRCODE_OK;
}


void aui_SoundFormat::ReleaseSoundData() {
	if(m_data)
		g_ui->TheMemMap()->ReleaseFileBits(m_data);
}



void aui_WavSoundFormat::TrimWavHeader(uint8 **wavedata, uint32 *size)
{
	int i;
	uint8 *data; 
	long raw_data_size=0; 

	data = *wavedata;
	for(i=0;i<(int)*size;i++)
	{
		if(*(long *)data == *(long *)"data")
		{
		data += 4; 
		raw_data_size = *(long *)data;
		data += 4;
		break;
		}
		else
		data++;
	}

	*wavedata = data; 
	*size = raw_data_size; 
}
