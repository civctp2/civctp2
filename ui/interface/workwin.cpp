
#include "c3.h"


#include "aui.h"
#include "aui_uniqueid.h"
#include "c3ui.h"
#include "aui_ldl.h"
#include "aui_UniqueId.h"


#include "c3_Button.h"


#include "c3windows.h"
#include "workwindow.h"



#include "SelItem.h"

#include "resourcemap.h"
#include "workmap.h"

#include "workwin.h"


extern sint32		g_ScreenWidth;
extern sint32		g_ScreenHeight;
extern C3UI			*g_c3ui;


WorkWindow			*g_workWindow = NULL;
ResourceMap				*g_resourceMap = NULL;


WorkMap *g_workMap = NULL;


static c3_Button			*s_exitButton;



extern SelectedItem			*g_selected_item;


void WorkWinCleanupAction::Execute(aui_Control *control,
									uint32 action,
									uint32 data )
{
	
	workwin_Cleanup();
}

void WorkWinUpdateAction::Execute(aui_Control *control,
									uint32 action,
									uint32 data )
{
	if (!g_workWindow) return;

	if ( g_c3ui->GetWindow(g_workWindow->Id()) ) {
		Unit city;
		if ( g_selected_item->GetSelectedCity(city) ) {
			if ( g_resourceMap ) {
				g_resourceMap->SetUnit( city );
				g_resourceMap->DrawSurface();
			}
		}
	}
}

void WorkExitButtonActionCallback( aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if (!g_workWindow) return;

	
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_workWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );
	if ( auiErr != AUI_ERRCODE_OK ) return;

	WorkWinCleanupAction *tempAction = new WorkWinCleanupAction;
	g_c3ui->AddAction( tempAction );
}


sint32 workwin_Initialize( void )
{
	AUI_ERRCODE		errcode;
	MBCHAR			windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR			controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_workWindow ) {
		if ( g_resourceMap ) {
			g_resourceMap->DrawSurface();
		}
		return 0; 
	}






















	


	strcpy(windowBlock, "WorkWindow");

	g_workWindow = new WorkWindow(&errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING);
	Assert( AUI_NEWOK(g_workWindow, errcode) );
	if ( !AUI_NEWOK(g_workWindow, errcode) ) return -1;

	g_workWindow->GrabRegion()->Move( 0, 0 );
	g_workWindow->GrabRegion()->Resize( g_workWindow->Width(), 20 );
	g_workWindow->SetDraggable( TRUE );

	sprintf( controlBlock, "%s.%s", windowBlock, "ResourceMap" );
	g_resourceMap = new ResourceMap( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(g_resourceMap, errcode) );
	if ( !AUI_NEWOK(g_resourceMap, errcode) ) return -3;

	
	
	
	
	
	



	sprintf( controlBlock, "%s.%s", windowBlock, "ExitButton" );
	s_exitButton = new c3_Button( &errcode, aui_UniqueId(), controlBlock, WorkExitButtonActionCallback );
	Assert( AUI_NEWOK(s_exitButton, errcode) );
	if ( !AUI_NEWOK(s_exitButton, errcode) ) return -5;

	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return -1;

	return 0;
}

sint32 workwin_Cleanup( void )
{
	if ( !g_workWindow ) return 0; 
	
	g_c3ui->RemoveWindow( g_workWindow->Id() );

	delete s_exitButton;
	s_exitButton = NULL;
	
	delete g_workWindow;
	g_workWindow = NULL;

	if (g_resourceMap) {
		delete g_resourceMap;
		g_resourceMap = NULL;
	}

	return 0;
}

sint32 workwin_Update( void )
{
	if ( g_workWindow ) {
		if ( g_c3ui->GetWindow(g_workWindow->Id()) ) {
			if ( g_resourceMap ) {
				g_resourceMap->DrawSurface();
			}
		}
	}

	return 1;
}
