//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Language settings screen
// Id           : $Id$
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
// - Created file. (04-May-2025 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "LanguageScreen.h"

#include "c3ui.h"
#include "Globals.h"            // allocated::clear
#include "keypress.h"

#include "spnewgamewindow.h"

#include "aui_uniqueid.h"
#include "ctp2_listbox.h"
#include "ctp2_button.h"
#include "ctp2_hypertextbox.h"
#include "ctp2_Static.h"

#include "LanguageRecord.h"

LanguageScreen* LanguageScreen::s_languageScreen = NULL;
extern C3UI *   g_c3ui;

LanguageScreen::LanguageScreen(AUI_ERRCODE *errcode, sint32 bpp)
:   c3_PopupWindow(errcode, aui_UniqueId(), "LanguageScreen", bpp, AUI_WINDOW_TYPE_FLOATING, false)
{
	AddTitle("LanguageScreen.Name");
	AddClose(LanguageScreen::AcceptPress);

	m_LanguageListBox = (ctp2_ListBox *)aui_Ldl::BuildHierarchyFromRoot("LanguageScreen.LanguageBox");

	Assert(m_LanguageListBox);

	m_LanguageListBox->SortByColumn(0, true);

	for(sint32 i = 0; i < g_theLanguageDB->NumRecords(); ++i)
	{
		const LanguageRecord* lanRec = g_theLanguageDB->Get(i);

		if(lanRec->GetHidden())
			continue;

		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("LanguageItem");
		Assert(item);
		if(!item)
			return;

		item->SetUserData((void *)i);

		ctp2_Static *text = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(text);
		if(!text)
			return;

		text->SetText(lanRec->GetNameText());
		item->SetCompareCallback(LanguageScreen::CompareItems);
		m_LanguageListBox->AddItem(item);
	}

	m_languageDescription = new ctp2_HyperTextBox(errcode, aui_UniqueId(), "LanguageScreen.LanguageDescription");

	m_getLanguageFromOS = spNew_ctp2_Button(errcode, "LanguageScreen", "SelectTrackButton", LanguageScreen::GetLanguageFromOS);

	*errcode = aui_Ldl::SetupHeirarchyFromRoot("LanguageScreen");
}

LanguageScreen::~LanguageScreen()
{
	delete m_LanguageListBox;
	delete m_languageDescription;
	delete m_getLanguageFromOS;
}

void LanguageScreen::Init()
{
	if(s_languageScreen == NULL)
	{
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;
		s_languageScreen = new LanguageScreen(&errcode, 16);
	}
}

void LanguageScreen::Cleanup()
{
	if (s_languageScreen != NULL)
	{
		g_c3ui->RemoveWindow(s_languageScreen->Id());
		keypress_RemoveHandler(s_languageScreen);
	}

	allocated::clear(s_languageScreen);
}

void LanguageScreen::DisplayWindow()
{
	Init();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(s_languageScreen);
	Assert(auiErr == AUI_ERRCODE_OK);
	keypress_RegisterHandler(s_languageScreen);
}

void LanguageScreen::RemoveWindow(uint32 action)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(s_languageScreen->Id());
	Assert(auiErr == AUI_ERRCODE_OK);
	keypress_RemoveHandler(s_languageScreen);
}

void LanguageScreen::AcceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != (uint32)AUI_BUTTON_ACTION_EXECUTE) return;

	LanguageScreen::RemoveWindow(action);
}

void LanguageScreen::GetLanguageFromOS(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
}

sint32 LanguageScreen::CompareItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	ctp2_Static *text1 = (ctp2_Static *)item1->GetChildByIndex(0);
	ctp2_Static *text2 = (ctp2_Static *)item2->GetChildByIndex(0);

	return _stricoll(text1->GetText(), text2->GetText());
}

/*
sint32	musicscreen_displayMyWindow()
{
	sint32 retval = (s_musicScreen) ? 0 : musicscreen_Initialize();

	s_autoRepeat->SetState(s_useAutoRepeat);
	s_randomOrder->SetState(s_useRandomOrder);
	s_musicOn->SetState(s_useMusicOn);

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(s_musicScreen);
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RegisterHandler(s_musicScreen);

	return retval;
}

sint32 musicscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(s_musicScreen->Id());
	Assert( auiErr == AUI_ERRCODE_OK );
	keypress_RemoveHandler(s_musicScreen);

	return 1;
}


AUI_ERRCODE musicscreen_Initialize( void )
{
	if ( s_musicScreen ) return AUI_ERRCODE_OK;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	strcpy(windowBlock, "MusicScreen");
	{
		s_musicScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false );
		Assert( AUI_NEWOK(s_musicScreen, errcode) );
		if ( !AUI_NEWOK(s_musicScreen, errcode) )
		{
			return errcode;
		}

		s_musicScreen->Resize(s_musicScreen->Width(),s_musicScreen->Height());
		s_musicScreen->GrabRegion()->Resize(s_musicScreen->Width(),s_musicScreen->Height());
		s_musicScreen->SetStronglyModal(TRUE);
	}


	s_musicScreen->AddTitle( "MusicScreen.Name" );
	s_musicScreen->AddClose( musicscreen_acceptPress );

	s_selectTrack = spNew_ctp2_Button( &errcode, windowBlock, "SelectTrackButton", musicscreen_selectTrackPress );

	s_musicString = spNewStringTable( &errcode, "MS_Strings" );

	s_autoRepeat	= spNew_c3_Switch(&errcode,windowBlock,"AutoRepeatSwitch",musicscreen_checkPress );
	s_autoRepeat->SetText( s_musicString->GetString(MS_STRING_AUTO_OFF) );
	s_randomOrder	= spNew_c3_Switch(&errcode,windowBlock,"RandomOrderSwitch",musicscreen_checkPress );
	s_randomOrder->SetText( s_musicString->GetString(MS_STRING_RANDOM_OFF) );
	s_musicOn		= spNew_c3_Switch(&errcode,windowBlock,"MusicOnSwitch",musicscreen_checkPress );

	s_useAutoRepeat		= g_soundManager->IsAutoRepeat();
	s_useRandomOrder	= (MUSICSTYLE_RANDOM == g_soundManager->GetMusicStyle());
	s_useMusicOn		= g_theProfileDB->IsUseRedbookAudio();

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}


void musicscreen_Cleanup()
{
	musictrackscreen_Cleanup();

	if (s_musicScreen)
	{
		g_c3ui->RemoveWindow(s_musicScreen->Id());
		keypress_RemoveHandler(s_musicScreen);
	}

	allocated::clear(s_musicString);
	allocated::clear(s_selectTrack);
	allocated::clear(s_autoRepeat);
	allocated::clear(s_randomOrder);
	allocated::clear(s_musicOn);
	allocated::clear(s_musicScreen);
}

void musicscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	c3_Switch *musicSwitch = (c3_Switch *)control;

	switch ( action ) {
	case AUI_SWITCH_ACTION_ON:
		if ( musicSwitch == s_autoRepeat ) {
			s_autoRepeat->SetText( s_musicString->GetString(MS_STRING_AUTO_ON) );
			s_useAutoRepeat = TRUE;
		}
		else if ( musicSwitch == s_randomOrder ) {
			s_randomOrder->SetText( s_musicString->GetString(MS_STRING_RANDOM_ON) );
			s_useRandomOrder = TRUE;
		}
		else if ( musicSwitch == s_musicOn ) {
			s_useMusicOn = TRUE;
		}
		break;

	case AUI_SWITCH_ACTION_OFF:
		if ( musicSwitch == s_autoRepeat ) {
			s_autoRepeat->SetText( s_musicString->GetString(MS_STRING_AUTO_OFF) );
			s_useAutoRepeat = FALSE;
		}
		else if ( musicSwitch == s_randomOrder ) {
			s_randomOrder->SetText( s_musicString->GetString(MS_STRING_RANDOM_OFF) );
			s_useRandomOrder = FALSE;
		}
		else if ( musicSwitch == s_musicOn ) {
			s_useMusicOn = FALSE;
		}
		break;
	}
}
void musicscreen_selectTrackPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( musicscreen_removeMyWindow(action) ) {
		musictrackscreen_displayMyWindow();
	}
}
void musicscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if (g_soundManager) {
		g_soundManager->SetAutoRepeat(s_useAutoRepeat);

		if (g_musicTrackChosen) {
			g_soundManager->SetMusicStyle(MUSICSTYLE_USER);
			g_soundManager->SetLastTrack(0);
		} else {
			if (s_useRandomOrder)
				g_soundManager->SetMusicStyle(MUSICSTYLE_RANDOM);
			else
				g_soundManager->SetMusicStyle(MUSICSTYLE_PLAYLIST);
		}

		if ( s_useMusicOn ) {
			g_soundManager->EnableMusic();
			g_soundManager->InitRedbook();
		} else {
			g_soundManager->DisableMusic();
			g_soundManager->CleanupRedbook();
		}

		g_theProfileDB->SetUseRedbookAudio(s_useMusicOn);

		g_soundManager->PickNextTrack();
		g_soundManager->StartMusic();
	}

	musicscreen_removeMyWindow(action);

	g_musicTrackChosen = FALSE;
}*/
