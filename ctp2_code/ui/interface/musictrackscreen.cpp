//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Music track selection window
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
// - Cleanup improved.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_listbox.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3_switch.h"
#include "c3_checkbox.h"
#include "c3ui.h"
#include "profileDB.h"
#include "spnewgamewindow.h"
#include "musicscreen.h"
#include "musictrackscreen.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "soundmanager.h"

extern SoundManager		*g_soundManager;

extern C3UI				*g_c3ui;
extern ProfileDB		*g_theProfileDB;

static c3_PopupWindow	*s_musicTrackScreen	= NULL;
static c3_Button		*s_accept			= NULL;
static c3_ListBox		*s_trackList		= NULL;
static aui_StringTable	*s_trackNames		= NULL;
static sint32			s_trackNum			= -1;

BOOL					g_musicTrackChosen = FALSE;




sint32	musictrackscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_musicTrackScreen) { retval = musictrackscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_musicTrackScreen);
	Assert( auiErr == AUI_ERRCODE_OK );

	aui_Item *item = s_trackList->GetSelectedItem();
	if (item) {
		s_trackList->DeselectItem(item);
	}

	g_musicTrackChosen = FALSE;

	s_trackNum = -1;

	return retval;
}
sint32 musictrackscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_musicTrackScreen->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}


void MusicTrackListCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	MusicTrackListItem *item = (MusicTrackListItem *) s_trackList->GetSelectedItem();
	if (!item)
		return;

	s_trackNum = item->GetTrackNum();

}





AUI_ERRCODE musictrackscreen_Initialize( void )
{
	if (s_musicTrackScreen) return AUI_ERRCODE_OK;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	MBCHAR		windowBlock[k_AUI_LDL_MAXBLOCK + 1];
	strcpy(windowBlock, "MusicTrackScreen");
	{
		s_musicTrackScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false );
		Assert( AUI_NEWOK(s_musicTrackScreen, errcode) );
		if ( !AUI_NEWOK(s_musicTrackScreen, errcode) ) errcode;

		s_musicTrackScreen->Resize(s_musicTrackScreen->Width(),s_musicTrackScreen->Height());
		s_musicTrackScreen->GrabRegion()->Resize(s_musicTrackScreen->Width(),s_musicTrackScreen->Height());
		s_musicTrackScreen->SetStronglyModal(TRUE);
	}

	s_musicTrackScreen->AddTitle( "MusicTrackScreen.Name" );
	s_musicTrackScreen->AddClose( musictrackscreen_acceptPress );

	MBCHAR		controlBlock[k_AUI_LDL_MAXBLOCK + 1];
	sprintf( controlBlock, "%s.%s", windowBlock, "TrackList" );
	s_trackList = new c3_ListBox( &errcode, aui_UniqueId(), controlBlock, MusicTrackListCallback, NULL);
	Assert( AUI_NEWOK(s_trackList, errcode) );
	if ( !AUI_NEWOK(s_trackList, errcode) ) return errcode;
	s_trackList->SetForceSelect(FALSE);

	sprintf(controlBlock, "%s.%s", windowBlock, "TrackNames");
	s_trackNames = new aui_StringTable(&errcode, controlBlock);
	Assert(AUI_NEWOK(s_trackNames, errcode));
	if (!AUI_NEWOK(s_trackNames, errcode)) return errcode;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	sprintf(controlBlock, "%s.%s", windowBlock, "MusicTrackListItem");
	for (sint32 i = 0; i < s_trackNames->GetNumStrings(); ++i)
    {
		s_trackList->AddItem
            (new MusicTrackListItem
                (&errcode, i, s_trackNames->GetString(i), controlBlock)
            );
	}

	return AUI_ERRCODE_OK;
}




void musictrackscreen_Cleanup()
{
	if (s_musicTrackScreen)
	{
		g_c3ui->RemoveWindow(s_musicTrackScreen->Id());
	}
    allocated::clear(s_trackNames);
    allocated::clear(s_trackList);
    allocated::clear(s_musicTrackScreen);
}




void musictrackscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( musictrackscreen_removeMyWindow(action) ) {

		if (g_soundManager) {
			if (s_trackNum != -1) {
				g_soundManager->SetMusicStyle(MUSICSTYLE_USER);
				g_soundManager->SetUserTrack(s_trackNum);
				g_musicTrackChosen = TRUE;
			}
		}

		musicscreen_displayMyWindow();
	}
}





MusicTrackListItem::MusicTrackListItem(AUI_ERRCODE *retval, sint32 trackNum, MBCHAR *name, MBCHAR *ldlBlock)
	:
	aui_ImageBase(ldlBlock),
	aui_TextBase(ldlBlock, (MBCHAR *)NULL),
	c3_ListItem( retval, ldlBlock)
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommonLdl(trackNum, name, ldlBlock);
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;
}

AUI_ERRCODE MusicTrackListItem::InitCommonLdl(sint32 trackNum, MBCHAR *name, MBCHAR *ldlBlock)
{
	MBCHAR			block[ k_AUI_LDL_MAXBLOCK + 1 ];
	AUI_ERRCODE		retval;

	c3_Static		*subItem;

	sprintf(block, "%s.%s", ldlBlock, "TrackName");
	subItem = new c3_Static(&retval, aui_UniqueId(), block);
	subItem->SetText(name);

	AddChild(subItem);

	m_trackNum = trackNum;

	return AUI_ERRCODE_OK;
}
