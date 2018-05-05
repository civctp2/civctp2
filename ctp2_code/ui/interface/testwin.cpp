#include "ctp/c3.h"

#include "ui/aui_common/aui.h"
#include "ui/aui_common/aui_window.h"
#include "ui/aui_common/aui_surface.h"
#include "ui/aui_common/aui_ldl.h"
#include "ui/aui_common/aui_button.h"
#include "ui/aui_common/aui_uniqueid.h"

#include "ui/interface/test.h"
#include "ui/interface/testwin.h"
#include "ui/interface/testwindow.h"

#include "gs/gameobj/message.h"
#include "ui/interface/messageiconwindow.h"
#include "ui/interface/messagewin.h"

#include "ui/aui_ctp2/c3ui.h"

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
