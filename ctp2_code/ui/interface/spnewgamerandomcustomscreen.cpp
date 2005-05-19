
 
#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3ui.h"
#include "aui_radio.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"

#include "loadsavemapwindow.h"
#include "allinonewindow.h"

#include "spnewgamewindow.h"
#include "spnewgamerandomcustomscreen.h"

#include "keypress.h"

extern C3UI			*g_c3ui;
extern SPNewGameWindow *g_spNewGameWindow;

#define k_NUM_RANDOMCUSTOMBOXES	2

static c3_PopupWindow	*s_spNewGameRandomCustomScreen	= NULL;



static aui_SwitchGroup	*s_group		= NULL;
static aui_Radio	**s_checkBox;

static MBCHAR	checknames[k_NUM_RANDOMCUSTOMBOXES][50] = {
	"Random",
	"Custom"
};




sint32	spnewgamerandomcustomscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_spNewGameRandomCustomScreen) { retval = spnewgamerandomcustomscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_spNewGameRandomCustomScreen);
	keypress_RegisterHandler(s_spNewGameRandomCustomScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	
	bool custom = g_spNewGameWindow ?
		g_spNewGameWindow->m_useCustomMap != 0:

		false; 
	s_checkBox[ 0 ]->SetState( !custom );
	s_checkBox[ 1 ]->SetState( custom );

	return retval;
}
sint32 spnewgamerandomcustomscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_spNewGameRandomCustomScreen->Id() );
	keypress_RemoveHandler(s_spNewGameRandomCustomScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}



AUI_ERRCODE spnewgamerandomcustomscreen_Initialize(
	aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_spNewGameRandomCustomScreen ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "SPNewGameRandomCustomScreen");

	{ 
		s_spNewGameRandomCustomScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGameRandomCustomScreen, errcode) );
		if ( !AUI_NEWOK(s_spNewGameRandomCustomScreen, errcode) ) errcode;

		
		s_spNewGameRandomCustomScreen->Resize(s_spNewGameRandomCustomScreen->Width(),s_spNewGameRandomCustomScreen->Height());
		s_spNewGameRandomCustomScreen->GrabRegion()->Resize(s_spNewGameRandomCustomScreen->Width(),s_spNewGameRandomCustomScreen->Height());
		s_spNewGameRandomCustomScreen->SetStronglyModal(TRUE);
	}
	

	
	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGameRandomCustomScreen->AddTitle( controlBlock );
	s_spNewGameRandomCustomScreen->AddClose( callback ? callback : spnewgamerandomcustomscreen_backPress );



	sprintf( controlBlock, "%s.%s", windowBlock, "Group" );
	s_group = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_group, errcode) );
	if ( !AUI_NEWOK(s_group, errcode) ) return errcode;

	s_checkBox = new aui_Radio*[k_NUM_RANDOMCUSTOMBOXES];

	for ( i = 0;i < k_NUM_RANDOMCUSTOMBOXES;i++ ) {
		sprintf( switchBlock, "%s.%s", controlBlock, checknames[i] );
		s_checkBox[i] = new aui_Radio( &errcode, aui_UniqueId(), switchBlock );
		Assert( AUI_NEWOK(s_checkBox[i], errcode) );
		if ( !AUI_NEWOK(s_checkBox[i], errcode) ) return errcode;
		s_group->AddSwitch( (aui_Radio *)s_checkBox[i] );
	
	}

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE spnewgamerandomcustomscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_spNewGameRandomCustomScreen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_spNewGameRandomCustomScreen->Id() );
	keypress_RemoveHandler(s_spNewGameRandomCustomScreen);

	for (sint32 i = 0;i < k_NUM_RANDOMCUSTOMBOXES;i++ ) {
		mycleanup( s_checkBox[i] );
	}

	mycleanup( s_group );




	delete s_spNewGameRandomCustomScreen;
	s_spNewGameRandomCustomScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




void spnewgamerandomcustomscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	spnewgamerandomcustomscreen_removeMyWindow(action);

	
	if ( s_checkBox[ 1 ]->GetState() )
	{
		if ( g_spNewGameWindow )
		{
			g_spNewGameWindow->m_useCustomMap = true;
		}
		else
		{

		}

		loadsavemapscreen_displayMyWindow(0);
	}
	else
	{
		
		if ( g_spNewGameWindow )
		{
			g_spNewGameWindow->m_useCustomMap = false;
			spnewgamescreen_update();
		}
		else
		{

		}
	}
}
