











#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3ui.h"
#include "aui_action.h"
#include "aui_control.h"

#include "c3_popupwindow.h"
#include "optionswindow.h"
#include "keypress.h"
#include "ctp2_button.h"
#include "c3_static.h"
#include "UIUtils.h"
#include "c3_hypertextbox.h"
#include "ctp2_hypertextbox.h"

#include "scenarioeditor.h"

#include "scorewarn.h"
#include "StrDB.h"
#include "profileDB.h"
#include "SelItem.h"
#include "GameSettings.h"


extern C3UI				*g_c3ui;

c3_PopupWindow	*g_scorewarn = NULL;


static c3_Static	*s_message;





void scorewarn_OkButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	disclaimer_Initialize(scorewarn_AcceptWarningCallback);
}



void scorewarn_AcceptWarningCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if(optionsscreen_removeMyWindow(action)) {
		AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( g_scorewarn->Id() );
		keypress_RemoveHandler(g_scorewarn);
		Assert( auiErr == AUI_ERRCODE_OK );
		if ( auiErr != AUI_ERRCODE_OK ) return;

		if ( g_selected_item ) {
			g_selected_item->Deselect( g_selected_item->GetVisiblePlayer() );
		}

		ScenarioEditor::Display();

		
		g_theGameSettings->SetKeepScore( FALSE );
	}
}

void scorewarn_CancelButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( g_scorewarn->Id() );
	keypress_RemoveHandler(g_scorewarn);
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;

	
	g_theGameSettings->SetKeepScore( TRUE );
}

sint32 scorewarn_Initialize( void )
{
	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (g_scorewarn) return 0;	

	strcpy(windowBlock, "Scorewarn");

	g_scorewarn = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING);
	Assert(AUI_NEWOK(g_scorewarn, errcode));
	if (!AUI_NEWOK(g_scorewarn, errcode)) return -1;

	g_scorewarn->SetStronglyModal( TRUE );

	
	g_scorewarn->AddOk( scorewarn_OkButtonActionCallback, NULL, "c3_PopupOk" );
	g_scorewarn->AddCancel( scorewarn_CancelButtonActionCallback );

	sprintf(buttonBlock, "%s.%s", windowBlock, "Message");
	s_message = new c3_Static(
		&errcode,
		aui_UniqueId(),
		buttonBlock );
	Assert(AUI_NEWOK(s_message, errcode));
	if (!AUI_NEWOK(s_message, errcode)) return -1;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	return 0;
}

sint32 scorewarn_Cleanup( void )
{
	if (!g_scorewarn) return 0;	

	g_c3ui->RemoveWindow( g_scorewarn->Id() );
	keypress_RemoveHandler(g_scorewarn);

	RemoveControl ( s_message );

	RemoveControl ( g_scorewarn );

	return 0;
}






static c3_PopupWindow	*s_disclaimerWindow = NULL;
static c3_Static		*s_disclaimerLabel = NULL;
static ctp2_Button		*s_disclaimerAcceptButton = NULL;
static ctp2_Button		*s_disclaimerDeclineButton = NULL;
static aui_Control::ControlActionCallback *s_disclaimerCallback = NULL;
static ctp2_HyperTextBox	*s_disclaimerTextBox;

extern BOOL g_launchIntoCheatMode;

void DisclaimerCloseAction::Execute(aui_Control *control, uint32 action, uint32 data)
{
	disclaimer_Cleanup();
}

void disclaimer_AcceptButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) return;

	g_c3ui->AddAction(new DisclaimerCloseAction);
	
	if (s_disclaimerCallback)
		s_disclaimerCallback(control, action, data, cookie);
}

void disclaimer_DeclineButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if (action != AUI_BUTTON_ACTION_EXECUTE) return;

	
	
	
	g_launchIntoCheatMode = FALSE;
	

	g_c3ui->AddAction(new DisclaimerCloseAction);
}

sint32 disclaimer_Initialize(aui_Control::ControlActionCallback *callback)
{
	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			buttonBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if (s_disclaimerWindow) {
		errcode = g_c3ui->AddWindow( s_disclaimerWindow );

		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -1;

		s_disclaimerCallback = callback;

		return 0;	
	}
	
	strcpy(windowBlock, "DisclaimerScreen");

	s_disclaimerWindow = new c3_PopupWindow(
		&errcode,
		aui_UniqueId(),
		windowBlock,
		16,
		AUI_WINDOW_TYPE_FLOATING);
	Assert(AUI_NEWOK(s_disclaimerWindow, errcode));
	if (!AUI_NEWOK(s_disclaimerWindow, errcode)) return -1;

	
	s_disclaimerWindow->SetStronglyModal(TRUE);




	
	sprintf(buttonBlock, "%s.%s", windowBlock, "TitleLabel");
	s_disclaimerLabel = new c3_Static(&errcode, aui_UniqueId(), buttonBlock);
	Assert(AUI_NEWOK(s_disclaimerLabel, errcode));
	if (!AUI_NEWOK(s_disclaimerLabel, errcode)) return -1;

	
	
	
	sprintf( buttonBlock, "%s.%s", windowBlock, "AgreeButton" );
	s_disclaimerAcceptButton = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock, 
		disclaimer_AcceptButtonActionCallback);
	Assert( AUI_NEWOK(s_disclaimerAcceptButton, errcode) );
	if ( !AUI_NEWOK(s_disclaimerAcceptButton, errcode) ) return -1;
	

	
	
	
	sprintf( buttonBlock, "%s.%s", windowBlock, "DisagreeButton" );
	s_disclaimerDeclineButton = new ctp2_Button( &errcode, aui_UniqueId(), buttonBlock, 
		disclaimer_DeclineButtonActionCallback);
	Assert( AUI_NEWOK(s_disclaimerDeclineButton, errcode) );
	if ( !AUI_NEWOK(s_disclaimerDeclineButton, errcode) ) return -1;

	
	
	
	sprintf(buttonBlock, "%s.%s", windowBlock, "DisclaimerText");
	s_disclaimerTextBox = new ctp2_HyperTextBox(&errcode, aui_UniqueId(), buttonBlock, NULL, NULL);
	Assert( AUI_NEWOK(s_disclaimerTextBox, errcode) );
	if ( !AUI_NEWOK(s_disclaimerTextBox, errcode) ) return -1;

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	
	
	
	MBCHAR		*message;
	sint32		filesize = 0;

	FILE *f = fopen("disclaimer.txt", "rb");

	if (!f)
		goto Error;

	if (fseek(f, 0, SEEK_END) == 0) {
		filesize = ftell(f);
	} else {
		goto Error;
	}
 	
	fclose(f);

	message = new MBCHAR[filesize+1];
	memset(message, 0, filesize+1);

	f = fopen("disclaimer.txt", "rb");
	if (!f) 
		goto Error;















	c3files_fread( message, 1, filesize, f );

	fclose(f);

	s_disclaimerTextBox->SetHyperText(message);

	delete message;
	message = NULL;

	s_disclaimerCallback = callback;

	
	errcode = g_c3ui->AddWindow( s_disclaimerWindow );

	Assert( errcode == AUI_ERRCODE_OK );
	if ( errcode != AUI_ERRCODE_OK ) return -1;

	return 0;

Error:
	if (s_disclaimerWindow) {
		disclaimer_Cleanup();
	}

	return -1;
}



void disclaimer_Cleanup()
{
	if (!s_disclaimerWindow) return;

	g_c3ui->RemoveWindow( s_disclaimerWindow->Id() );

	RemoveControl(s_disclaimerLabel);
	RemoveControl(s_disclaimerAcceptButton);
	RemoveControl(s_disclaimerDeclineButton);
	RemoveControl(s_disclaimerTextBox);

	RemoveControl(s_disclaimerWindow);
}
