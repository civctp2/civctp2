










#include "c3.h"
#include "aui_ui.h"
#include "aui_ldl.h"
#include "aui_sound.h"

#include "aui_soundbase.h"

#include "soundmanager.h"
#include "gamesounds.h"

extern SoundManager		*g_soundManager;


MBCHAR *aui_SoundBase::m_soundLdlKeywords[ AUI_SOUNDBASE_SOUND_LAST ] =
{
	"activatesound",
	"deactivatesound",
	"engagesound",
	"disengagesound",
	"executesound",
	"tipsound"
};



aui_SoundBase::aui_SoundBase( MBCHAR *ldlBlock )
{
	InitCommonLdl( ldlBlock );
}



aui_SoundBase::aui_SoundBase( MBCHAR **soundNames )
{
	InitCommon( soundNames );
}



AUI_ERRCODE aui_SoundBase::InitCommonLdl( MBCHAR *ldlBlock )
{
	aui_Ldl *theLdl = g_ui->GetLdl();

	
	BOOL valid = theLdl->IsValid( ldlBlock );
	Assert( valid );
	if ( !valid ) return AUI_ERRCODE_HACK;

	
	ldl_datablock *block = theLdl->GetLdl()->FindDataBlock( ldlBlock );
	Assert( block != NULL );
	if ( !block ) return AUI_ERRCODE_LDLFINDDATABLOCKFAILED;

	MBCHAR *soundNames[ AUI_SOUNDBASE_SOUND_LAST ];
	for ( sint32 i = 0; i < AUI_SOUNDBASE_SOUND_LAST; i++ )
		soundNames[ i ] = block->GetString( m_soundLdlKeywords[ i ] );

	AUI_ERRCODE errcode = InitCommon( soundNames );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return errcode;

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE aui_SoundBase::InitCommon( MBCHAR **soundNames )
{
	memset( m_sounds, 0, sizeof( m_sounds ) );

	for ( sint32 i = 0; i < AUI_SOUNDBASE_SOUND_LAST; i++ )
		SetSound( (AUI_SOUNDBASE_SOUND)i, soundNames ? soundNames[ i ] : NULL );

	return AUI_ERRCODE_OK;
}



aui_SoundBase::~aui_SoundBase()
{
	for ( sint32 i = 0; i < AUI_SOUNDBASE_SOUND_LAST; i++ )
	{
		if ( m_sounds[ i ] )
		{
			g_ui->UnloadSound( m_sounds[ i ] );
			m_sounds[ i ] = NULL;
		}
	}
}



aui_Sound *aui_SoundBase::GetSound( AUI_SOUNDBASE_SOUND sound ) const
{
	if ( sound < 0 || sound >= AUI_SOUNDBASE_SOUND_LAST )
		return NULL;

	return m_sounds[ sound ];
}



aui_Sound *aui_SoundBase::SetSound(
	AUI_SOUNDBASE_SOUND sound,
	MBCHAR *soundName )
{
	aui_Sound *prevSound = GetSound( sound );

	if ( soundName && g_ui->TheAudioManager()->UsingAudio() )
	{
		m_sounds[ sound ] = g_ui->LoadSound( soundName );
		Assert( m_sounds[ sound ] != NULL );
		if ( !m_sounds[ sound ] )
		{
			m_sounds[ sound ] = prevSound;
			return NULL;
		}
	}
	else
		m_sounds[ sound ] = NULL;

	if ( prevSound ) g_ui->UnloadSound( prevSound );

	return prevSound;
}



AUI_ERRCODE aui_SoundBase::PlaySound( AUI_SOUNDBASE_SOUND sound )
{






	switch (sound) {



	case AUI_SOUNDBASE_SOUND_EXECUTE:
		g_soundManager->AddSound(SOUNDTYPE_SFX, 0, 
				gamesounds_GetGameSoundID(GAMESOUNDS_BUTTONCLICK), 0, 0);
		break;
	case AUI_SOUNDBASE_SOUND_ENGAGE:
	case AUI_SOUNDBASE_SOUND_TIP:


		break;
	}

	return AUI_ERRCODE_OK;
}
