

#include "c3.h"
#include "c3window.h"
#include "c3_popupwindow.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3ui.h"
#include "aui_switch.h"
#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "spnewgamerulesscreen.h"

#include "keypress.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;

#define k_NUM_RULESBOXES	2

static c3_PopupWindow	*s_spNewGameRulesScreen	= NULL;






static aui_Switch **s_checkBox;

static MBCHAR	checknames[k_NUM_RULESBOXES][50] = {
	"RuleOne",
	"RuleTwo",
};




sint32	spnewgamerulesscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_spNewGameRulesScreen) { retval = spnewgamerulesscreen_Initialize(); }

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow(s_spNewGameRulesScreen);
	keypress_RegisterHandler(s_spNewGameRulesScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}
sint32 spnewgamerulesscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_spNewGameRulesScreen->Id() );
	keypress_RemoveHandler(s_spNewGameRulesScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}



AUI_ERRCODE spnewgamerulesscreen_Initialize( void )
{
	AUI_ERRCODE errcode;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_spNewGameRulesScreen ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "SPNewGameRulesScreen");

	{ 
		s_spNewGameRulesScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGameRulesScreen, errcode) );
		if ( !AUI_NEWOK(s_spNewGameRulesScreen, errcode) ) errcode;

		
		s_spNewGameRulesScreen->Resize(s_spNewGameRulesScreen->Width(),s_spNewGameRulesScreen->Height());
		s_spNewGameRulesScreen->GrabRegion()->Resize(s_spNewGameRulesScreen->Width(),s_spNewGameRulesScreen->Height());
		s_spNewGameRulesScreen->SetStronglyModal(TRUE);
	}
	



	
	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGameRulesScreen->AddTitle( controlBlock );
	s_spNewGameRulesScreen->AddClose( spnewgamerulesscreen_backPress );



	s_checkBox = new aui_Switch*[k_NUM_RULESBOXES];

	for ( i = 0;i < k_NUM_RULESBOXES;i++ ) {
		sprintf( controlBlock, "%s.%s", windowBlock, checknames[i] );
		s_checkBox[i] = new aui_Switch( &errcode, aui_UniqueId(), controlBlock, spnewgamerulesscreen_switchPress );
		Assert( AUI_NEWOK(s_checkBox[i], errcode) );
		if ( !AUI_NEWOK(s_checkBox[i], errcode) ) return errcode;
	
	}

	s_checkBox[ 0 ]->SetState( g_theProfileDB->IsGenocideRule() );
	s_checkBox[ 1 ]->SetState( g_theProfileDB->IsPollutionRule() );










	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



AUI_ERRCODE spnewgamerulesscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_spNewGameRulesScreen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_spNewGameRulesScreen->Id() );
	keypress_RemoveHandler(s_spNewGameRulesScreen);

	for (sint32 i = 0;i < k_NUM_RULESBOXES;i++ ) {
		mycleanup( s_checkBox[i] );
	}







	delete s_spNewGameRulesScreen;
	s_spNewGameRulesScreen = NULL;

	return AUI_ERRCODE_OK;

#undef mycleanup
}




void spnewgamerulesscreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	spnewgamerulesscreen_removeMyWindow(action);

}
void spnewgamerulesscreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	spnewgamerulesscreen_removeMyWindow(action);

}
void spnewgamerulesscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	

	spnewgamerulesscreen_removeMyWindow(action);

}
void spnewgamerulesscreen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	switch ( action ) {
	case AUI_SWITCH_ACTION_ON:
	case AUI_SWITCH_ACTION_OFF:
		for ( sint32 i = 0;i < k_NUM_RULESBOXES;i++ ) {

			if ( control == s_checkBox[i] ) {
				switch ( i ) {
				case RULE_GENOCIDE:
					g_theProfileDB->SetAlienEndGame( !s_checkBox[i]->IsOn() );
					break;
				case RULE_POLLUTION:
					g_theProfileDB->SetPollutionRule( s_checkBox[i]->IsOn() );
					break;









				}
			}
		
		}
		break;
	}
}
