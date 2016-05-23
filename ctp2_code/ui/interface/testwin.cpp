#include "c3.h"

#include "aui.h"
#include "aui_window.h"
#include "aui_surface.h"
#include "aui_ldl.h"
#include "aui_button.h"
#include "aui_uniqueid.h"

#include "test.h"
#include "testwin.h"
#include "testwindow.h"

#include "message.h"
#include "messageiconwindow.h"
#include "messagewin.h"

#include "c3ui.h"

extern C3UI			*g_c3ui;

UITestWindow		*g_testWindow = NULL;

int uitest_Initialize( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	g_testWindow = new UITestWindow( &errcode, aui_UniqueId(),
									100, 100, 200, 100, 16, "upba0104.tif" );
	Assert( AUI_NEWOK( g_testWindow, errcode ));
	if ( !AUI_NEWOK( g_testWindow, errcode )) return -1;

	errcode = g_c3ui->AddWindow( g_testWindow );
	Assert(errcode == AUI_ERRCODE_OK);
	if ( errcode != AUI_ERRCODE_OK ) return 11;

	messagewin_InitializeMessages();

	return 1;
}

int uitest_Cleanup( void )
{
	if ( g_testWindow ) {
		delete g_testWindow;
		g_testWindow = NULL;
	}

	return 1;
}

int AddTestWindowToUI( void )
{
	return uitest_Initialize();
}

int RemoveTestWindowToUI( void )
{
	return uitest_Cleanup();
}
