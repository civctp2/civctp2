//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player new game map size selection screen
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
// - Memory leak repaired.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_radio.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamemapshapescreen.h"

#include "spnewgamewindow.h"
#include "spnewgamemapsizescreen.h"

#include "keypress.h"

#include "Globals.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;

static c3_PopupWindow	*s_spNewGameMapSizeScreen	= NULL;


static aui_SwitchGroup	*s_group		= NULL;
static aui_Radio	**s_checkBox;

static MBCHAR	checknames[k_NUM_MAPSIZEBOXES][50] = {
	"MapSizeOne",
	"MapSizeTwo",
	"MapSizeThree",
	"MapSizeFour"
};

static sint32 s_useMode = 0;

static sint32 s_mapSizeIndex = 0;
sint32 spnewgamemapsizescreen_getMapSizeIndex( void )
{
	return s_mapSizeIndex;
}






void spnewgamemapsizescreen_setMapSizeIndex( sint32 index )
{

	if ( index < 0 || index >= k_NUM_MAPSIZEBOXES )
		return;

	for (sint32 i = 0;i < k_NUM_MAPSIZEBOXES;i++ )
		s_checkBox[ i ]->SetState( 0 );
	s_checkBox[ index ]->SetState( 1 );

	switch ( s_mapSizeIndex = index ) {
	case 0:
		g_theProfileDB->SetMapSize(MAPSIZE_SMALL);

		break;
	case 1:
		g_theProfileDB->SetMapSize(MAPSIZE_MEDIUM);

		break;
	case 2:
		g_theProfileDB->SetMapSize(MAPSIZE_LARGE);

		break;
	case 3:
		g_theProfileDB->SetMapSize(MAPSIZE_GIGANTIC);

		break;
	default:

		Assert( FALSE );
		break;
	}
}





sint32	spnewgamemapsizescreen_displayMyWindow(BOOL viewMode, sint32 useMode)
{
	sint32 retval=0;
	if(!s_spNewGameMapSizeScreen) { retval = spnewgamemapsizescreen_Initialize(); }

	AUI_ERRCODE auiErr;

	for (sint32 i = 0;i < k_NUM_MAPSIZEBOXES;i++ )
		s_checkBox[ i ]->Enable( !viewMode );

	s_useMode = useMode;

	auiErr = g_c3ui->AddWindow(s_spNewGameMapSizeScreen);
	keypress_RegisterHandler(s_spNewGameMapSizeScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 spnewgamemapsizescreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	uint32 id = s_group->WhichIsSelected();

	if ( id ) {
		for ( sint32 i = 0;i < k_NUM_MAPSIZEBOXES;i ++ ) {
			if ( id == s_checkBox[i]->Id() ) {
				spnewgamemapsizescreen_setMapSizeIndex( i );
			}
		}
	}

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_spNewGameMapSizeScreen->Id() );
	keypress_RemoveHandler(s_spNewGameMapSizeScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	if ( s_useMode == 1 ) {
		spnewgamemapshapescreen_displayMyWindow( FALSE, 1 );
	}

	spnewgamescreen_update();

	return 1;
}


AUI_ERRCODE spnewgamemapsizescreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_spNewGameMapSizeScreen ) return AUI_ERRCODE_OK;

	strcpy(windowBlock, "SPNewGameMapSizeScreen");

	{
		s_spNewGameMapSizeScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGameMapSizeScreen, errcode) );

		s_spNewGameMapSizeScreen->Resize(s_spNewGameMapSizeScreen->Width(),s_spNewGameMapSizeScreen->Height());
		s_spNewGameMapSizeScreen->GrabRegion()->Resize(s_spNewGameMapSizeScreen->Width(),s_spNewGameMapSizeScreen->Height());
		s_spNewGameMapSizeScreen->SetStronglyModal(TRUE);
	}

	if ( !callback ) callback = spnewgamemapsizescreen_backPress;

	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGameMapSizeScreen->AddTitle( controlBlock );
	s_spNewGameMapSizeScreen->AddClose( callback );


	sprintf( controlBlock, "%s.%s", windowBlock, "Group" );
	s_group = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_group, errcode) );
	if ( !AUI_NEWOK(s_group, errcode) ) return errcode;

	s_checkBox = new aui_Radio*[k_NUM_MAPSIZEBOXES];

	for ( i = 0;i < k_NUM_MAPSIZEBOXES;i++ ) {
		sprintf( switchBlock, "%s.%s", controlBlock, checknames[i] );
		s_checkBox[i] = new aui_Radio( &errcode, aui_UniqueId(), switchBlock );
		Assert( AUI_NEWOK(s_checkBox[i], errcode) );
		if ( !AUI_NEWOK(s_checkBox[i], errcode) ) return errcode;
		s_group->AddSwitch( (aui_Radio *)s_checkBox[i] );

	}

	MAPSIZE		size;

	size = g_theProfileDB->GetMapSize();

	switch (size) {
	case MAPSIZE_SMALL:
		s_checkBox[0]->SetState( TRUE );
		break;
	case MAPSIZE_MEDIUM:
		s_checkBox[1]->SetState( TRUE );
		break;
	case MAPSIZE_LARGE:
		s_checkBox[2]->SetState( TRUE );
		break;
	case MAPSIZE_GIGANTIC:
		s_checkBox[3]->SetState( TRUE );
		break;
	}
















	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgamemapsizescreen_Cleanup
//
// Description: Release the memory of the screen.
//
// Parameters : -
//
// Globals    : s_spNewGameMapSizeScreen
//				s_checkBox
//				s_group
//
// Returns    : AUI_ERRCODE	: always AUI_ERRCODE_OK
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

AUI_ERRCODE spnewgamemapsizescreen_Cleanup()
{
	if (s_spNewGameMapSizeScreen)
	{
		g_c3ui->RemoveWindow(s_spNewGameMapSizeScreen->Id());
		keypress_RemoveHandler(s_spNewGameMapSizeScreen);

		for (sint32 i = 0; i < k_NUM_MAPSIZEBOXES; ++i)
		{
			delete s_checkBox[i];
			// NULLing unnecessary: deleting container next
		}
		delete [] s_checkBox;
		s_checkBox = NULL;

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; }
		mycleanup(s_group);
		mycleanup(s_spNewGameMapSizeScreen);
#undef mycleanup
	}

	return AUI_ERRCODE_OK;
}


void spnewgamemapsizescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{


	spnewgamemapsizescreen_removeMyWindow(action);
}
