//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player new game rules screen
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
// - Memory leaks repaired.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
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
#include "aui_switch.h"
#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "spnewgamerulesscreen.h"

#include "keypress.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;

#define k_NUM_RULESBOXES	3 //2  //EMOD - 22 if new game settings added

static c3_PopupWindow	*s_spNewGameRulesScreen	= NULL;





static aui_Switch **	s_checkBox	= NULL;

static MBCHAR	checknames[k_NUM_RULESBOXES][50] = {
	"RuleOne",
	"RuleTwo",
	"Rule22", //emod adding city capture options, first try at adding new rules using profiledb
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
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
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
	s_checkBox[ 2 ]->SetState( g_theProfileDB->IsCityCaptureOn() ); //may have to make new bool in profiledb

/*  add new game settings here
	s_checkBox[ 3 ]->SetState( g_theProfileDB->IsGenocideRule() );
	s_checkBox[ 4 ]->SetState( g_theProfileDB->IsPollutionRule() );
	s_checkBox[ 5 ]->SetState( g_theProfileDB->GetValueByName("NoAIProductionDeficit") );		
	s_checkBox[ 6 ]->SetState( g_theProfileDB->GetValueByName("NoAIGoldDeficit") );						
	s_checkBox[ 7 ]->SetState( g_theProfileDB->GetValueByName("AICityDefenderBonus") );					
	s_checkBox[ 8 ]->SetState( g_theProfileDB->GetValueByName("BarbarianCities") );					
	s_checkBox[ 9 ]->SetState( g_theProfileDB->GetValueByName("SectarianHappiness") );
	s_checkBox[ 10 ]->SetState( g_theProfileDB->GetValueByName("RevoltCasualties") );					
	s_checkBox[ 11 ]->SetState( g_theProfileDB->GetValueByName("RevoltInsurgents") );					
	s_checkBox[ 12 ]->SetState( g_theProfileDB->GetValueByName("BarbarianCamps") );			
	s_checkBox[ 13 ]->SetState( g_theProfileDB->GetValueByName("BarbarianSpawnsBarbarian") );			
	s_checkBox[ 14 ]->SetState( g_theProfileDB->GetValueByName("AINoSinking") );							
	s_checkBox[ 15 ]->SetState( g_theProfileDB->GetValueByName("AINoCityLimit") );						
	s_checkBox[ 16 ]->SetState( g_theProfileDB->GetValueByName("GoldPerUnitSupport") );
	s_checkBox[ 17 ]->SetState( g_theProfileDB->GetValueByName("GoldPerCity") );							
	s_checkBox[ 18 ]->SetState( g_theProfileDB->GetValueByName("AINoShieldHunger") );					
	s_checkBox[ 19 ]->SetState( g_theProfileDB->GetValueByName("AINoGoldHunger") );						
	s_checkBox[ 20 ]->SetState( g_theProfileDB->GetValueByName("AIFreeUpgrade") );						
	s_checkBox[ 21 ]->SetState( g_theProfileDB->GetValueByName("AIMilitiaUnit") );

*/








	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}



//----------------------------------------------------------------------------
//
// Name       : spnewgamerulesscreen_Cleanup
//
// Description: Release the memory of the single player new game rules screen.
//
// Parameters : -
//
// Globals    : s_spNewGameRulesScreen
//				s_checkBox
//
// Returns    : AUI_ERRCODE	: always AUI_ERRCODE_OK
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------

AUI_ERRCODE spnewgamerulesscreen_Cleanup()
{
	if (s_spNewGameRulesScreen)
	{
		g_c3ui->RemoveWindow(s_spNewGameRulesScreen->Id());
		keypress_RemoveHandler(s_spNewGameRulesScreen);

		for (sint32 i = 0; i < k_NUM_RULESBOXES; i++) 
		{
			delete s_checkBox[i];
			// NULLing unnecessary: deleting the container next
		}
		delete [] s_checkBox;
		s_checkBox	= NULL;

		delete s_spNewGameRulesScreen;
		s_spNewGameRulesScreen = NULL;
	}

	return AUI_ERRCODE_OK;
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
				case RULE_CaptureCity:
					g_theProfileDB->SetCityCapture( !s_checkBox[i]->IsOn() );
					//s_cityInfluenceToggled = TRUE;  this is from graphicscreen but here there is no toggle...
					break;
/*  new rules switch
				case 	RULE_NoAIProductionDeficit		
				case 	RULE_NoAIGoldDeficit						
				case 	RULE_AICityDefenderBonus					
				case 	RULE_BarbarianCities					
				case 	RULE_SectarianHappiness
				case 	RULE_RevoltCasualties					
				case 	RULE_RevoltInsurgents					
				case 	RULE_BarbarianCamps			
				case 	RULE_BarbarianSpawnsBarbarian			
				case 	RULE_AINoSinking							
				case 	RULE_AINoCityLimit						
				case 	RULE_GoldPerUnitSupport
				case 	RULE_GoldPerCity							
				case 	RULE_AINoShieldHunger					
				case 	RULE_AINoGoldHunger						
				case 	RULE_AIFreeUpgrade						
				case 	RULE_AIMilitiaUnit
				case 	RULE_DebugAI
*/








				}
			}
		
		}
		break;
	}
}
