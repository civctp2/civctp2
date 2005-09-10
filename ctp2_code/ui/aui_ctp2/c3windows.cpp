//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : The civilization 3 base window
// Id           : $Id:$
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
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
//
//----------------------------------------------------------------------------

#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "aui_ldl.h"
#include "c3ui.h"


#include "statuswindow.h"
#include "tipwindow.h"
#include "controlpanelwindow.h"
#include "debugwindow.h"
#include "c3_popupwindow.h"
#include "radarwindow.h"


#include "textbutton.h"
#include "iconbutton.h"


#include "c3thumb.h"
#include "c3slider.h"
#include "c3scroller.h"
#include "c3spinner.h"


#include "checkbox.h"
#include "textswitch.h"


#include "textradio.h"
#include "radiogroup.h"


#include "texttab.h"
#include "aui_tabgroup.h"


#include "aui_item.h"
#include "c3listbox.h"
#include "textbox.h"


#include "c3dropdown.h"


#include "c3textfield.h"


#include "tiledmap.h"

#include "c3windows.h"

#include "CivPaths.h"
#include "videoutils.h"


#include "workwin.h"
#include "statswindow.h"
#include "workwindow.h"


#include "backgroundwin.h"
#include "workwin.h"

#include "thumbnailmap.h"

#include "player.h"
#include "ID.h"
#include "SelItem.h"

#include "screenutils.h"
#include "GameSettings.h"

#include "c3_utilitydialogbox.h"



extern sint32 g_ScreenWidth;
extern sint32 g_ScreenHeight;

extern C3UI						*g_c3ui;
extern StatusWindow				*g_statusWindow;
extern ControlPanelWindow	*g_controlPanel;
extern WorkWindow			*g_workWindow;
extern C3Window				*g_turnWindow;
extern C3Window				*g_statsWindow;
extern CivPaths				*g_civPaths;

extern sint32				g_god;

extern RECT g_backgroundViewport;

DebugWindow					*g_debugWindow;
extern SelectedItem	*g_selected_item;
extern GameSettings		*g_theGameSettings;




C3Window		*g_testWindow = NULL;
C3Window		*g_standardWindow = NULL;
C3Window		*g_floatingWindow = NULL;

static TipWindow	*g_tipWindow = NULL;
static TipWindow	*g_thumbTipWindow = NULL;



static TextTab *g_happyTab = NULL;
static IconButton *g_iconButton = NULL;






void DebugExitButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_debugWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
}

void DebugApplyButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	
	TextBox *textbox;
	C3TextField *textfield;

	
	memcpy( &textbox, cookie, sizeof( TextBox * ) );
	memcpy( &textfield, (MBCHAR *)cookie + sizeof( TextBox * ), sizeof( C3TextField * ) );

	
	static MBCHAR text[ 100 ];

	
	textfield->GetFieldText( text, 100 );

	
	
	if ( strcmp( text, "clear" ) == 0 )
		textbox->SetText( "" );

	
	else
		textbox->AppendText( text );
}

void TestWindowButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( k_ID_WINDOW_TEST );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;

	auiErr = g_c3ui->RemoveWindow( k_ID_WINDOW_STANDARD );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;

	auiErr = g_c3ui->RemoveWindow( k_ID_WINDOW_FLOATING );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;

	
	
	
}






void TabCallbackSad( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
}

void TabCallbackIndifferent( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
}

void TabCallbackHappy( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
}






void CheckboxCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	aui_TabGroup *tabGroup = (aui_TabGroup *)cookie;
	if ( !tabGroup ) return;

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;



	switch ( action )
	{
	case AUI_SWITCH_ACTION_ON:
		

		if ( !g_iconButton )
		{
			sint32 controlX = 50;
			sint32 controlY = 50;

			g_iconButton = new IconButton(
				&errcode,
				k_ID_ICONBUTTON_TESTWINDOW,
				controlX,				
				controlY,				
				50, 50,
				k_PatternName,				
				k_IconName,					
				0xf2ed );				
			if ( !g_iconButton ) return;

		}

		

		if ( !g_happyTab )
		{
			
			
			g_happyTab = new TextTab(
				&errcode,
				k_ID_TAB_HAPPY,
				0,
				0,
				0,
				0,
				0,
				0,
				k_PatternName,
				"Happy",
				TabCallbackHappy );
			if ( !g_happyTab ) return;

			
			errcode = g_happyTab->AddPaneControl( g_iconButton );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK ) return;
		}

		errcode = tabGroup->AddTab( g_happyTab );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return;

		break;

	case AUI_SWITCH_ACTION_OFF:
		if ( g_happyTab )
		{
			errcode = tabGroup->RemoveTab( k_ID_TAB_HAPPY );
			Assert( errcode == AUI_ERRCODE_OK );
			if ( errcode != AUI_ERRCODE_OK ) return;














		}
		break;
	}
}

void DraggableCheckboxCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	aui_Window *window = control->GetParentWindow();
	if ( !window ) return;

	switch ( action )
	{
	case AUI_SWITCH_ACTION_ON:
		window->SetDraggable( TRUE );
		break;

	case AUI_SWITCH_ACTION_OFF:
		window->SetDraggable( FALSE );
		break;
	}
}

void TransparentCheckboxCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	aui_Window *window = control->GetParentWindow();
	if ( !window ) return;

	switch ( action )
	{
	case AUI_SWITCH_ACTION_ON:
		window->SetTransparent( TRUE );
		break;

	case AUI_SWITCH_ACTION_OFF:
		window->SetTransparent( FALSE );
		break;
	}
}






void ModalRadioCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	aui_Window *window = control->GetParentWindow();
	if ( !window ) return;

	switch ( control->Id() )
	{
	case k_ID_RADIO_WEAKLYMODAL:
		switch ( action )
		{
		case AUI_SWITCH_ACTION_ON:
			window->SetWeaklyModal( TRUE );
			break;

		case AUI_SWITCH_ACTION_OFF:
			window->SetWeaklyModal( FALSE );
			break;
		}
		break;

	case k_ID_RADIO_STRONGLYMODAL:
		switch ( action )
		{
		case AUI_SWITCH_ACTION_ON:
			window->SetStronglyModal( TRUE );
			break;

		case AUI_SWITCH_ACTION_OFF:
			window->SetStronglyModal( FALSE );
			break;
		}
		break;
	}
}








int c3windows_MakeTestWindow( BOOL make )
{
	
	static TextButton	*button = NULL;
	static TextTab		*sadTab = NULL;
	static Checkbox		*checkbox = NULL;
	static Checkbox		*dragcheckbox = NULL;
	static Checkbox		*transparentcheckbox = NULL;
	static TextTab		*indifferentTab = NULL;
	static TextRadio	*stronglyModalRadio = NULL;
	static TextRadio	*weaklyModalRadio = NULL;
	static RadioGroup	*modalRadioGroup = NULL;
	static aui_TabGroup	*moodyTabGroup = NULL;
	static C3Spinner	*spinner = NULL;

	if ( make )
	{
		if ( g_testWindow ) return 0; 

		
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;

		
		
		MBCHAR windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

		{ 

			sprintf( windowBlock, "mywindow" );

			g_testWindow = new C3Window(
				&errcode,
				k_ID_WINDOW_TEST,
				windowBlock,
				16 );
			Assert( AUI_NEWOK(g_testWindow,errcode) );
			if ( !AUI_NEWOK(g_testWindow,errcode) ) return -1;


			
			
			MBCHAR controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

			{ 

				sprintf( controlBlock, "%s.%s", windowBlock, "mybutton" );

				button = new TextButton(
					&errcode,
					k_ID_BUTTON_TESTWINDOW,
					controlBlock,
					TestWindowButtonActionCallback );
				Assert( AUI_NEWOK(button,errcode) );
				if ( !AUI_NEWOK(button,errcode) ) return -1;

				
				
				

				
				c3windows_MakeTipWindow();
				
				
				button->SetTipWindow( g_tipWindow );

			} 

			{ 

				sprintf( controlBlock, "%s.%s", windowBlock, "myspinner" );

				spinner = new C3Spinner(
					&errcode,
					k_ID_SPINNER,
					controlBlock );
				Assert( AUI_NEWOK(spinner,errcode) );
				if ( !AUI_NEWOK(spinner,errcode) ) return -1;

			} 

			{ 

				sprintf( controlBlock, "%s.%s", windowBlock, "mytabgroup" );

				
				moodyTabGroup = new aui_TabGroup(
					&errcode,
					k_ID_TABGROUP_MOODY,
					controlBlock );
				Assert( AUI_NEWOK(moodyTabGroup,errcode) );
				if ( !AUI_NEWOK(moodyTabGroup,errcode) ) return -1;

				
				moodyTabGroup->SetDrawMask( k_AUI_REGION_DRAWFLAG_UPDATE );


				
				
				MBCHAR tabBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

				{ 

					sprintf( tabBlock, "%s.%s", controlBlock, "sadtab" );

					sadTab = new TextTab(
						&errcode,
						k_ID_TAB_SAD,
						tabBlock,
						TabCallbackSad );
					Assert( AUI_NEWOK(sadTab,errcode) );
					if ( !AUI_NEWOK(sadTab,errcode) ) return -1;


					
					
					MBCHAR checkboxBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

					{ 

						sprintf( checkboxBlock, "%s.pane.%s", tabBlock, "checkbox" );

						checkbox = new Checkbox(
							&errcode,
							k_ID_CHECKBOX_TESTWINDOW,
							checkboxBlock,
							CheckboxCallback,
							moodyTabGroup );
						Assert( AUI_NEWOK(checkbox,errcode) );
						if ( !AUI_NEWOK(checkbox,errcode) ) return -1;
						
					} 
						
					{ 
						
						sprintf( checkboxBlock, "%s.pane.%s", tabBlock, "dragcheckbox" );

						dragcheckbox = new Checkbox(
							&errcode,
							k_ID_DRAGCHECKBOX_TESTWINDOW,
							checkboxBlock,
							DraggableCheckboxCallback );
						Assert( AUI_NEWOK(dragcheckbox,errcode) );
						if ( !AUI_NEWOK(dragcheckbox,errcode) ) return -1;
						
					} 

					{ 
						
						sprintf( checkboxBlock, "%s.pane.%s", tabBlock, "transparentcheckbox" );

						transparentcheckbox = new Checkbox(
							&errcode,
							k_ID_TRANSPARENTCHECKBOX_TESTWINDOW,
							checkboxBlock,
							TransparentCheckboxCallback );
						Assert( AUI_NEWOK(transparentcheckbox,errcode) );
						if ( !AUI_NEWOK(transparentcheckbox,errcode) ) return -1;

					} 

				} 

				{ 

					sprintf( tabBlock, "%s.%s", controlBlock, "indifferenttab" );

					indifferentTab = new TextTab(
						&errcode,
						k_ID_TAB_INDIFFERENT,
						tabBlock,
						TabCallbackIndifferent );
					Assert( AUI_NEWOK(indifferentTab,errcode) );
					if ( !AUI_NEWOK(indifferentTab,errcode) ) return -1;


					
					
					MBCHAR radiogroupBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

					{ 

						sprintf( radiogroupBlock, "%s.pane.%s", tabBlock, "modalradiogroup" );

						modalRadioGroup = new RadioGroup(
							&errcode,
							k_ID_RADIOGROUP_MODAL,
							radiogroupBlock );
						Assert( AUI_NEWOK(modalRadioGroup,errcode) );
						if ( !AUI_NEWOK(modalRadioGroup,errcode) ) return -1;


						
						
						MBCHAR radioBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

						{ 

							sprintf( radioBlock, "%s.%s", radiogroupBlock, "stronglymodalradio" );

							stronglyModalRadio = new TextRadio(
								&errcode,
								k_ID_RADIO_STRONGLYMODAL,
								radioBlock,
								ModalRadioCallback );
							Assert( AUI_NEWOK(stronglyModalRadio,errcode) );
							if ( !AUI_NEWOK(stronglyModalRadio,errcode) ) return -1;
							
						} 

						{ 

							sprintf( radioBlock, "%s.%s", radiogroupBlock, "weaklymodalradio" );

							weaklyModalRadio = new TextRadio(
								&errcode,
								k_ID_RADIO_WEAKLYMODAL,
								radioBlock,
								ModalRadioCallback );
							Assert( AUI_NEWOK(weaklyModalRadio,errcode) );
							if ( !AUI_NEWOK(weaklyModalRadio,errcode) ) return -1;

						} 

					} 

				} 

			} 

		} 

		
		errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
		Assert( AUI_SUCCESS(errcode) );
		if ( !AUI_SUCCESS(errcode) ) return -1;
	}
	else
	{
		if ( !g_testWindow ) return 0; 

		if ( button )
		{
			delete button;
			button = NULL;
		}
		
		if ( sadTab )
		{
			delete sadTab;
			sadTab = NULL;
		}
		
		if ( checkbox )
		{
			delete checkbox;
			checkbox = NULL;
		}
		
		if ( dragcheckbox )
		{
			delete dragcheckbox;
			dragcheckbox = NULL;
		}
		
		if ( transparentcheckbox )
		{
			delete transparentcheckbox;
			transparentcheckbox = NULL;
		}
		
		if ( indifferentTab )
		{
			delete indifferentTab;
			indifferentTab = NULL;
		}
		
		if ( stronglyModalRadio )
		{
			delete stronglyModalRadio;
			stronglyModalRadio = NULL;
		}
		
		if ( weaklyModalRadio )
		{
			delete weaklyModalRadio;
			weaklyModalRadio = NULL;
		}
		
		if ( modalRadioGroup )
		{
			delete modalRadioGroup;
			modalRadioGroup = NULL;
		}
		
		if ( moodyTabGroup )
		{
			delete moodyTabGroup;
			moodyTabGroup = NULL;
		}
		
		if ( spinner )
		{
			delete spinner;
			spinner = NULL;
		}

		c3windows_MakeTipWindow( FALSE );

		if ( g_testWindow )
		{
			g_c3ui->RemoveWindow( g_testWindow->Id() );
			delete g_testWindow;
			g_testWindow = NULL;
		}
	}

	return 0;
}

int c3windows_MakeStandardWindow( BOOL make )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static C3Slider *slider;
	static C3DropDown *dropdown;
	static TextSwitch *i1;
	static TextSwitch *i2;
	static TextSwitch *i3;
	static TextSwitch *i4;
	static TextSwitch *i5;
	static TextSwitch *i6;

	if ( make )
	{
		if ( g_standardWindow ) return 0; 
		
		
		
		
		
		sint32 windowWidth = 3 * g_ScreenWidth / 8;
		sint32 windowHeight = 2 * g_ScreenHeight / 6;
		sint32 windowX = ( g_ScreenWidth - windowWidth ) / 3;
		sint32 windowY = ( g_ScreenHeight - windowHeight ) / 3;
		
		g_standardWindow = new C3Window(
			&errcode,
			k_ID_WINDOW_STANDARD,
			windowX, windowY, windowWidth, windowHeight,
			16,
			k_PatternName );
		Assert( g_standardWindow != NULL );
		if ( !g_standardWindow ) return -1;
		
		
		
		
		
		sint32 controlWidth = 2 * windowWidth / 3;
		sint32 controlHeight = windowHeight / 4;
		sint32 controlX = windowWidth / 6;
		sint32 controlY = windowHeight / 6;
		
		slider = new C3Slider(
			&errcode,
			k_ID_SLIDER,
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			FALSE,
			k_PatternName );
		if ( !slider ) return -3;
		
		errcode = slider->SetMaximum( 3, 0 );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -3;
		
		errcode = slider->SetPage( 1, 1 );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -3;
		
		errcode = slider->UseRigidThumb( TRUE );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -3;
		
		errcode = slider->UseQuantizedDragging( TRUE );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -3;
		
		
		
		
		
		windowWidth = 200;
		windowHeight = 40;
		windowX = 0;
		windowY = 0;
		
		g_thumbTipWindow = new TipWindow(
			&errcode,
			k_ID_WINDOW_TIP,
			windowX, windowY, windowWidth, windowHeight,
			16,
			k_PatternName,
			"I am a Thumb control." );
		Assert( g_thumbTipWindow != NULL );
		if ( !g_thumbTipWindow ) return -1;
		
		
		C3Thumb *thumb = (C3Thumb *)slider->GetThumb();
		if ( thumb ) 
			thumb->SetTipWindow( g_thumbTipWindow );
		
		
		
		
		
		
		controlWidth = 2 * g_standardWindow->Width() / 3;
		controlHeight = g_standardWindow->Height() / 8;
		controlX = g_standardWindow->Width() / 6;
		controlY = g_standardWindow->Height() - 2 * controlHeight;
		
		dropdown = new C3DropDown(
			&errcode,
			aui_UniqueId(),
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			50,
			5 * ( controlHeight - 4 ) );
		if ( !dropdown ) return -3;
		
		
		
		i1 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			controlWidth - 30, controlHeight - 4,
			k_PatternName,
			"yes" );
		if ( !i1 ) return -1;
		i2 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			controlWidth - 40, controlHeight - 4,
			k_PatternName,
			"no" );
		if ( !i2 ) return -2;
		i3 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			controlWidth - 50, controlHeight - 4,
			k_PatternName,
			"i mean it" );
		if ( !i3 ) return -3;
		i4 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			controlWidth - 30, controlHeight - 4,
			k_PatternName,
			"anybody wanna peanut" );
		if ( !i4 ) return -4;
		i5 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			controlWidth - 25, controlHeight - 4,
			k_PatternName,
			"yes ma'am" );
		if ( !i5 ) return -5;
		i6 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			controlWidth - 35, controlHeight - 4,
			k_PatternName,
			"yes sir" );
		if ( !i6 ) return -6;
		dropdown->AddItem( (aui_Item *)i1 );
		dropdown->AddItem( (aui_Item *)i2 );
		dropdown->AddItem( (aui_Item *)i3 );
		dropdown->AddItem( (aui_Item *)i4 );
		dropdown->AddItem( (aui_Item *)i5 );
		dropdown->AddItem( (aui_Item *)i6 );


		
		
		
		
		errcode = g_standardWindow->AddControl( slider );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -4;
		errcode = g_standardWindow->AddControl( dropdown );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -4;
		
		g_standardWindow->SetDraggable( TRUE );
	}
	else
	{
		if ( !g_standardWindow ) return 0; 

		delete slider;
		slider = NULL;
		delete g_thumbTipWindow;
		g_thumbTipWindow = NULL;
		delete dropdown;
		dropdown = NULL;
		delete i1;
		i1 = NULL;
		delete i2;
		i2 = NULL;
		delete i3;
		i3 = NULL;
		delete i4;
		i4 = NULL;
		delete i5;
		i5 = NULL;
		delete i6;
		i6 = NULL;

		g_c3ui->RemoveWindow( g_standardWindow->Id() );
		delete g_standardWindow;
		g_standardWindow = NULL;		
	}

	return 0;
}


int c3windows_MakeTipWindow( BOOL make )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	if ( make )
	{
		if ( g_tipWindow ) return 0; 
		
		
		
		
		
		sint32 windowWidth = 200;
		sint32 windowHeight = 30;
		sint32 windowX = 0;
		sint32 windowY = 0;
		
		g_tipWindow = new TipWindow(
			&errcode,
			k_ID_WINDOW_TIP,
			windowX, windowY, windowWidth, windowHeight,
			16,
			k_PatternName,
			"Press this button" );
		Assert( g_tipWindow != NULL );
		if ( !g_tipWindow ) return -1;
	}
	else
	{
		if ( !g_tipWindow ) return 0; 

		delete g_tipWindow;
		g_tipWindow = NULL;
	}

	return 0;
}


int c3windows_MakeFloatingWindow( BOOL make )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static C3ListBox *listbox;
	static TextSwitch *items[ 14 ];
	static TextSwitch *subItems[ 14 ][ 5 ];
	static TextSwitch *h1;
	static TextSwitch *h2;
	static TextSwitch *h3;
	static TextSwitch *h4;
	static TextSwitch *h5;
	static TextSwitch *h6;

	if ( make )
	{
		if ( g_floatingWindow ) return 0; 
		
		
		
		
		
		sint32 windowWidth = 600;
		sint32 windowHeight = 300;
		sint32 windowX = 10;
		sint32 windowY = 10;
		
		g_floatingWindow = new C3Window(
			&errcode,
			k_ID_WINDOW_FLOATING,
			windowX, windowY, windowWidth, windowHeight,
			16,
			k_PatternName,
			AUI_WINDOW_TYPE_FLOATING );
		Assert( g_floatingWindow != NULL );
		if ( !g_floatingWindow ) return -1;
		
		g_floatingWindow->SetDraggable( TRUE );
		
		
		
		
		
		
		sint32 controlWidth = 2 * windowWidth / 3;
		sint32 controlHeight = 2 * windowHeight / 3;
		sint32 controlX = windowWidth / 6;
		sint32 controlY = windowHeight / 6;
		
		listbox = new C3ListBox(
			&errcode,
			k_ID_LISTBOX,
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName );
		if ( !listbox ) return -3;
		

		
		
		sint32 itemHeight = 30 + 2 * (rand()%5);
		
		for ( sint32 i = 0; i < 14; i++ )
		{
			static char s[ 50 ];
			sprintf( s, "row=%d col=%d", i, 0 );
			
			items[ i ] = new TextSwitch(
				&errcode,
				aui_UniqueId(),
				0, 0,
				140 + 4 * (rand()%10), itemHeight,
				k_PatternName,
				s );
			if ( !items[ i ] ) return -i * 100;
			
			for ( sint32 j = 0; j < 5; j++ )
			{
				sprintf( s, "row=%d col=%d", i, j+1 );
				
				subItems[ i ][ j ] = new TextSwitch(
					&errcode,
					aui_UniqueId(),
					0, 0,
					140 + 4 * (rand()%10), itemHeight,
					k_PatternName,
					s );
				if ( !subItems[ i ][ j ] ) return -j * 1000;
				
				items[ i ]->AddChild( subItems[ i ][ j ] );
			}
			
			listbox->AddItem( (aui_Item *)items[ i ] );
		}
		
		
		
		
		h1 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			50, 25,
			k_PatternName,
			"name" );
		if ( !h1 ) return -1;
		h2 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			50, 25,
			k_PatternName,
			"date" );
		if ( !h2 ) return -2;
		h3 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			50, 25,
			k_PatternName,
			"stuff" );
		if ( !h3 ) return -3;
		h4 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			50, 25,
			k_PatternName,
			"thing" );
		if ( !h4 ) return -4;
		h5 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			50, 25,
			k_PatternName,
			"rate" );
		if ( !h5 ) return -5;
		h6 = new TextSwitch(
			&errcode,
			aui_UniqueId(),
			0, 0,
			50, 25,
			k_PatternName,
			"creamescense" );
		if ( !h6 ) return -6;
		listbox->AddHeaderSwitch( h1 );
		listbox->AddHeaderSwitch( h2 );
		listbox->AddHeaderSwitch( h3 );
		listbox->AddHeaderSwitch( h4 );
		listbox->AddHeaderSwitch( h5 );
		listbox->AddHeaderSwitch( h6 );
		
		
		
		
		
		
		errcode = g_floatingWindow->AddControl( listbox );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -4;
	
	}
	else
	{
		if ( !g_floatingWindow ) return 0; 

		delete listbox;
		listbox = NULL;
		for ( sint32 i = 0; i < 14; i++ )
		{
			for ( sint32 j = 0; j < 5; j++ )
			{
				delete subItems[ i ][ j ];
				subItems[ i ][ j ] = NULL;
			}
			delete items[ i ];
			items[ i ] = NULL;
		}
		
		














		g_c3ui->RemoveWindow( g_floatingWindow->Id() );
		delete g_floatingWindow;
		g_floatingWindow = NULL;
	}

	return 0;
}

void ControlWindowButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	int err;
	AUI_ERRCODE auiErr;

	err = c3windows_MakeTestWindow();
	Assert( err == 0 );
	if ( err != 0 ) return;

	err = c3windows_MakeStandardWindow();
	Assert( err == 0 );
	if ( err != 0 ) return;

	err = c3windows_MakeFloatingWindow();
	Assert( err == 0 );
	if ( err != 0 ) return;

	
	auiErr = g_c3ui->AddWindow( g_floatingWindow );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
	auiErr = g_c3ui->AddWindow( g_testWindow );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
	auiErr = g_c3ui->AddWindow( g_standardWindow );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
}

void KnowledgeButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	c3_ExpelPopup *popup = new c3_ExpelPopup(NULL);
	popup->DisplayWindow();
}

void DebugButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->AddWindow( g_debugWindow );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
}

static aui_Window		*s_thumbWindow = NULL;
static ThumbnailMap		*s_thumbnail = NULL;


void ResourceButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;






















}

void CheatButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	

	g_theGameSettings->SetKeepScore( FALSE );


}

void DiplomacyButtonActionCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

}

void CityViewButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	bool This_Code_Is_Really_Really_Old = false;
	Assert(This_Code_Is_Really_Really_Old);

#if 0
	int err;
	AUI_ERRCODE auiErr;

	PLAYER_INDEX	player ;

	ID	item ;

	SELECT_TYPE	state ;

	MapPoint pos;

	g_selected_item->GetTopCurItem(player, item, state) ;
	if (state != SELECT_TYPE_LOCAL_CITY)
		return ;


	err = cityviewwin_Initialize();
	Assert( err == 0 );
	if ( err != 0 ) return;

	
	CityViewWindow_Update();

	auiErr = g_c3ui->AddWindow( g_cityViewWindow );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;
#endif
}

#define k_STATUS_WINDOW_HEIGHT		30

int c3windows_MakeStatusWindow( BOOL make )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static C3TextField *textfield;
	static C3TextField *textfield2;
	static TextButton	*button, *debugButton, *resourceButton, *cheatButton, *cityViewButton, *knowledgeButton, *diplomacyButton;

	if ( make )
	{
		if ( g_statusWindow ) return 0; 

		
		sint32 windowWidth = g_ScreenWidth;
		sint32 windowHeight = k_STATUS_WINDOW_HEIGHT;
		sint32 windowX = 0;
		sint32 windowY = 27;

		
		g_statusWindow = new StatusWindow(
			&errcode,
			k_ID_WINDOW_STATUS,
			windowX, windowY, windowWidth, windowHeight,
			16,
			k_PatternName );
		Assert( g_statusWindow != NULL );
		if ( !g_statusWindow ) return -1;
		g_statusWindow->SetDraggable( TRUE );

		
		sint32 controlWidth = 60;
		sint32 controlHeight = 20;
		sint32 controlX = g_statusWindow->Width() / 2;
		sint32 controlY = 5;
		
		knowledgeButton = new TextButton(
			&errcode,
			aui_UniqueId(),
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			"Science",
			KnowledgeButtonActionCallback );
		if ( !knowledgeButton ) return -16;
		
		controlX += controlWidth + 6;

		button = new TextButton(
			&errcode,
			k_ID_BUTTON_CONTROLWINDOW,
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			"UI",
			ControlWindowButtonActionCallback );
		if ( !button ) return -16;
		
		controlX += controlWidth + 6;

		debugButton = new TextButton(
			&errcode,
			aui_UniqueId(),
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			"Dbg",
			DebugButtonActionCallback );
		if ( !debugButton ) return -16;
		
			controlX += controlWidth + 6;

		resourceButton = new TextButton(
			&errcode,
			aui_UniqueId(),
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			"FLI",
			ResourceButtonActionCallback );
		if ( !resourceButton ) return -16;	

		controlX += controlWidth + 6;

		cheatButton = new TextButton(
			&errcode,
			aui_UniqueId(),
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			"Cheat",
			CheatButtonActionCallback );
		if ( !cheatButton ) return -16;	
		
		controlX += controlWidth + 6;













		
		controlX += controlWidth + 6;
		diplomacyButton = new TextButton(
			&errcode,
			aui_UniqueId(),
			controlX,
			controlY,
			controlWidth,
			controlHeight,
			k_PatternName,
			"Throne",
			DiplomacyButtonActionCallback );
		if ( !diplomacyButton ) return -16;	
		
		controlX += controlWidth + 6;
		
		
		
		
		errcode = g_statusWindow->AddControl( button );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -15;
		errcode = g_statusWindow->AddControl( debugButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -15;
		errcode = g_statusWindow->AddControl( resourceButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -15;
		errcode = g_statusWindow->AddControl( cheatButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -15;



		errcode = g_statusWindow->AddControl( diplomacyButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -15;
		errcode = g_statusWindow->AddControl( knowledgeButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -15;































	}
	else
	{
		if ( !g_statusWindow ) return 0; 

		g_c3ui->RemoveWindow( g_statusWindow->Id() );

		delete button;
		button = NULL;
		delete debugButton;
		debugButton = NULL;
		delete resourceButton;
		resourceButton = NULL;
		delete cheatButton;
		cheatButton = NULL;


		delete diplomacyButton;
		diplomacyButton = NULL ;
		delete knowledgeButton;
		knowledgeButton = NULL;

		if (s_thumbWindow) delete s_thumbWindow;
		if (s_thumbnail) delete s_thumbnail;

		delete g_statusWindow;
		g_statusWindow = NULL;
	}

	return 0;
}

int c3windows_MakeDebugWindow( BOOL make )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	static TextBox *textbox;
	static TextButton *exitButton;
	static C3TextField *textfield;

	if ( make )
	{
		if ( g_debugWindow ) return 0; 

		
		sint32 windowWidth = g_ScreenWidth / 3;
		sint32 windowHeight = g_ScreenHeight / 2;
		sint32 windowX = 0;
		sint32 windowY = g_ScreenHeight - windowHeight;

		
		g_debugWindow = new DebugWindow(
			&errcode,
			aui_UniqueId(),
			windowX, windowY, windowWidth, windowHeight,
			16,
			k_PatternName,
			AUI_WINDOW_TYPE_FLOATING );
		Assert( g_debugWindow != NULL );
		if ( !g_debugWindow ) return -1;

		g_debugWindow->SetDynamic(FALSE);

		g_debugWindow->GrabRegion()->Move( 0, 0 );
		g_debugWindow->GrabRegion()->Resize( windowWidth, 20 );
		g_debugWindow->SetDraggable( TRUE );

		textbox = new TextBox(
			&errcode,
			aui_UniqueId(),
			2,
			12,
			windowWidth - 30,
			windowHeight - 24,
			k_PatternName, NULL, NULL, NULL);
		if ( !textbox ) return -3;



		exitButton = new TextButton(
			&errcode,
			aui_UniqueId(),
			windowWidth - 12,
			2,
			10,
			10,
			k_PatternName,
			"X",
			DebugExitButtonActionCallback,
			textbox );
		if ( !exitButton ) return -3;


		errcode = g_debugWindow->AddControl( textbox );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -4;

		errcode = g_debugWindow->AddControl( exitButton );
		Assert( errcode == AUI_ERRCODE_OK );
		if ( errcode != AUI_ERRCODE_OK ) return -4;


 
		g_debugWindow->SetTextBox(textbox);

	}
	else
	{
		if ( !g_debugWindow ) return 0; 

		g_c3ui->RemoveWindow( g_debugWindow->Id() );

		delete textbox;
		textbox = NULL;

		delete exitButton;
		exitButton = NULL;

		delete g_debugWindow;
		g_debugWindow = NULL;
	}

	return 0;
}

void c3windows_Cleanup( void )
{
	c3windows_MakeDebugWindow( FALSE );
	workwin_Cleanup();
	
	c3windows_MakeTestWindow( FALSE );
	c3windows_MakeFloatingWindow( FALSE );
	c3windows_MakeStandardWindow( FALSE );
	c3windows_MakeStatusWindow( FALSE );

	controlpanelwindow_Cleanup();



	radarwindow_Cleanup();
	backgroundWin_Cleanup();

	
	if ( g_iconButton )
	{
		delete g_iconButton;
		g_iconButton = NULL;
	}
	if ( g_happyTab )
	{
		delete g_happyTab;
		g_happyTab = NULL;
	}

	g_c3ui->UnloadIcon( k_IconName );
}


