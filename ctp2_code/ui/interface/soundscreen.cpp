

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_uniqueid.h"

#include "spnewgamewindow.h"
#include "soundscreen.h"
#include "ProfileDB.h"

#include "keypress.h"

extern C3UI					*g_c3ui;
extern ProfileDB			*g_theProfileDB;


static c3_PopupWindow *s_soundWindow	= NULL;
static C3Slider		*s_sfx				= NULL,
					*s_music			= NULL,
					*s_voice			= NULL;

static c3_Static	*s_sfxN				= NULL,
					*s_musicN			= NULL,
					*s_voiceN			= NULL;
static c3_Static	*s_sfxmin			= NULL,
					*s_sfxmax			= NULL,
					*s_musicmin			= NULL,
					*s_musicmax			= NULL,
					*s_voicemin			= NULL,
					*s_voicemax			= NULL;





sint32	soundscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_soundWindow) { retval = soundscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_soundWindow);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(s_soundWindow);

	return retval;
}
sint32 soundscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	
	sint32		sfx, voice, music;

	soundscreen_getValues(sfx, music, voice);

	g_theProfileDB->SetSFXVolume(sfx);
	g_theProfileDB->SetVoiceVolume(voice);
	g_theProfileDB->SetMusicVolume(music);

	auiErr = g_c3ui->RemoveWindow( s_soundWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(s_soundWindow);

	return 1;
}



AUI_ERRCODE soundscreen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( s_soundWindow ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "SoundWindow");
	s_soundWindow = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING,
		false );
	Assert( AUI_NEWOK(s_soundWindow, errcode) );
	if ( !AUI_NEWOK(s_soundWindow, errcode) ) errcode;

	s_soundWindow->SetStronglyModal(TRUE);

	
	s_sfx				= spNew_C3Slider(&errcode,windowBlock,"SFXSlider",soundscreen_sfxSlide);
	s_sfxN				= spNew_c3_Static(&errcode,windowBlock,"SFXName");
	s_music				= spNew_C3Slider(&errcode,windowBlock,"MusicSlider",soundscreen_musicSlide);
	s_musicN			= spNew_c3_Static(&errcode,windowBlock,"MusicName");
	s_voice				= spNew_C3Slider(&errcode,windowBlock,"VoiceSlider",soundscreen_voiceSlide);
	s_voiceN			= spNew_c3_Static(&errcode,windowBlock,"VoiceName");

	s_sfxmin			= spNew_c3_Static(&errcode,windowBlock,"SFXMin");
	s_sfxmax			= spNew_c3_Static(&errcode,windowBlock,"SFXMax");
	s_musicmin			= spNew_c3_Static(&errcode,windowBlock,"MusicMin");
	s_musicmax			= spNew_c3_Static(&errcode,windowBlock,"MusicMax");
	s_voicemin			= spNew_c3_Static(&errcode,windowBlock,"VoiceMin");
	s_voicemax			= spNew_c3_Static(&errcode,windowBlock,"VoiceMax");

	
	s_sfx->SetValue(g_theProfileDB->GetSFXVolume(), 0);
	s_voice->SetValue(g_theProfileDB->GetVoiceVolume(), 0);
	s_music->SetValue(g_theProfileDB->GetMusicVolume(), 0);






	MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.%s", windowBlock, "Name" );
	s_soundWindow->AddTitle( block );
	s_soundWindow->AddClose( soundscreen_exitPress );

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE soundscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_soundWindow  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_soundWindow->Id() );
	keypress_RemoveHandler(s_soundWindow);



	mycleanup(s_sfx);
	mycleanup(s_sfxN);
	mycleanup(s_music);
	mycleanup(s_musicN);
	mycleanup(s_voice);
	mycleanup(s_voiceN);



	mycleanup(s_sfxmin);
	mycleanup(s_sfxmax);
	mycleanup(s_musicmin);
	mycleanup(s_musicmax);
	mycleanup(s_voicemin);
	mycleanup(s_voicemax);

	delete s_soundWindow;
	s_soundWindow = NULL;

	return AUI_ERRCODE_OK;
#undef mycleanup
}




void soundscreen_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	g_theProfileDB->Save();
	
	soundscreen_removeMyWindow(action);
}









void soundscreen_sfxSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
	g_theProfileDB->SetSFXVolume(s_sfx->GetValueX());

}
void soundscreen_musicSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
	g_theProfileDB->SetMusicVolume(s_music->GetValueX());

}
void soundscreen_voiceSlide(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != AUI_RANGER_ACTION_VALUECHANGE ) return;
	g_theProfileDB->SetVoiceVolume(s_voice->GetValueX());

}






void soundscreen_getValues(sint32 &sfx, sint32 &music, sint32 &voice)
{
	
	sfx				= s_sfx->GetValueX();
	music			= s_music->GetValueX();
	voice			= s_voice->GetValueX();

}
