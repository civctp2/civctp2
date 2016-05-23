//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  :
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
// None
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Prevented memory leak report at the end of the program.
//
//----------------------------------------------------------------------------

#include "c3.h"

#include "civ3_main.h"
#include "civapp.h"
#include "gamefile.h"

#include "aui.h"
#include "aui_ldl.h"
#include "aui_uniqueid.h"
#include "aui_stringtable.h"
#include "aui_textfield.h"
#include "ns_gamesetup.h"

#include "c3ui.h"
#include "c3window.h"
#include "c3_static.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_listitem.h"
#include "c3_listbox.h"

#include "initialplaywindow.h"
#include "spnewgamewindow.h"
#include "loadsavemapwindow.h"

extern C3UI					*g_c3ui;
extern CivApp				*g_civApp;
extern nf_GameSetup			g_gamesetup;
extern SPNewGameWindow		*g_spNewGameWindow;

LoadSaveMapWindow				*g_loadSaveMapWindow = NULL;

static uint32 s_type = LSMS_TOTAL;
static c3_Static					*s_name					= NULL;
static aui_StringTable				*s_nameString			= NULL;

void loadsavemapscreen_setMyType(uint32 type);


sint32	loadsavemapscreen_displayMyWindow(uint32 type)
{
	sint32 retval=0;
	if(!g_loadSaveMapWindow) { retval = loadsavemapscreen_Initialize(); }


	g_loadSaveMapWindow->CleanUpSaveMapInfo();

	if(retval == AUI_ERRCODE_OK) {
		g_loadSaveMapWindow->SetType(type);

		g_c3ui->AddWindow(g_loadSaveMapWindow);
	}

	return retval;
}
sint32 loadsavemapscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( g_loadSaveMapWindow->Id() );
	Assert( auiErr == AUI_ERRCODE_OK );

	return 1;
}


AUI_ERRCODE loadsavemapscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

	if ( g_loadSaveMapWindow ) return AUI_ERRCODE_OK;

	strcpy(windowBlock, "LoadSaveMapWindow");

	g_loadSaveMapWindow= new LoadSaveMapWindow(&errcode, aui_UniqueId(), windowBlock, 16 , AUI_WINDOW_TYPE_FLOATING);
	Assert( AUI_NEWOK(g_loadSaveMapWindow, errcode) );
	if ( !AUI_NEWOK(g_loadSaveMapWindow, errcode) ) return errcode;

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	switch ( g_loadSaveMapWindow->GetType() )
	{
	case LSMS_LOAD_GAMEMAP:
		g_loadSaveMapWindow->GetOkButton()->Enable( FALSE );
		break;

	default:
		g_loadSaveMapWindow->GetOkButton()->Enable( TRUE );
		break;
	}

	g_loadSaveMapWindow->GetDeleteButton()->Enable( FALSE );




	g_loadSaveMapWindow->GetListOne()->GetHeader()->Enable( FALSE );
	g_loadSaveMapWindow->GetListTwo()->GetHeader()->Enable( FALSE );

	if ( callback )
		g_loadSaveMapWindow->GetOkButton()->SetActionFuncAndCookie(
			callback, NULL );

	return AUI_ERRCODE_OK;
}




AUI_ERRCODE loadsavemapscreen_Cleanup()
{
	if ( !g_loadSaveMapWindow  ) return AUI_ERRCODE_OK;

	g_c3ui->RemoveWindow( g_loadSaveMapWindow->Id() );

	delete g_loadSaveMapWindow;
	g_loadSaveMapWindow = NULL;

	return AUI_ERRCODE_OK;
}




void loadsavemapscreen_LoadGameMap(void)
{
	GameMapInfo	*gameMapInfo = g_loadSaveMapWindow->GetGameMapInfo();
	SaveMapInfo	*saveMapInfo = g_loadSaveMapWindow->GetSaveMapInfo();

	Assert(gameMapInfo);
	if (!gameMapInfo) return;

	Assert(saveMapInfo);
	if (!saveMapInfo) return;

	MBCHAR		path[_MAX_PATH];

	sprintf(path, "%s\\%s", gameMapInfo->path, saveMapInfo->fileName);


}



void loadsavemapscreen_SaveGameMap(void)
{
	SaveMapInfo		*saveMapInfo = g_loadSaveMapWindow->GetSaveMapInfoToSave();

	Assert( saveMapInfo != NULL );
	if ( !saveMapInfo ) return;

	if (!g_loadSaveMapWindow->GetGameMapName(saveMapInfo->gameMapName)) return;
	if (!g_loadSaveMapWindow->GetSaveMapName(saveMapInfo->fileName)) return;
	if (!g_loadSaveMapWindow->GetNote(saveMapInfo->note)) return;

	MBCHAR	path[_MAX_PATH];
	MBCHAR	fullPath[_MAX_PATH];

	if (!g_civPaths->GetSavePath(C3SAVEDIR_MAP, path)) return;

	sprintf(fullPath, "%s\\%s", path, saveMapInfo->gameMapName);

	if (!c3files_PathIsValid(fullPath)) {
		if (!c3files_CreateDirectory(fullPath)) {
			Assert(FALSE);


			return;
		}
	}

	sprintf(saveMapInfo->pathName, "%s\\%s", fullPath, saveMapInfo->fileName);

	g_loadSaveMapWindow->GetRadarMap(saveMapInfo);

	GameMapFile::SaveGameMap(saveMapInfo->pathName, saveMapInfo);
}





void loadsavemapscreen_executePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{


	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if(loadsavemapscreen_removeMyWindow(action)) {
		switch(g_loadSaveMapWindow->GetType()) {
			case LSMS_LOAD_GAMEMAP:		loadsavemapscreen_LoadGameMap();			break;
			case LSMS_SAVE_GAMEMAP:		loadsavemapscreen_SaveGameMap();			break;
			default:
				Assert(0);
				break;
		}
	}

	spnewgamescreen_update();
}


void loadsavemapscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;





	loadsavemapscreen_removeMyWindow(action) ;

	if ( g_spNewGameWindow ) g_spNewGameWindow->m_useCustomMap = false;
	spnewgamescreen_update();
}


void loadsavemapscreen_deletePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	GameMapInfo	*gameMapInfo = g_loadSaveMapWindow->GetGameMapInfo();
	SaveMapInfo	*saveMapInfo = g_loadSaveMapWindow->GetSaveMapInfo();

	Assert(gameMapInfo);
	if (!gameMapInfo) return;

	Assert(saveMapInfo);
	if (!saveMapInfo) return;

	MBCHAR		path[_MAX_PATH];

	sprintf(path, "%s%s%s", gameMapInfo->path, FILE_SEP, saveMapInfo->fileName);

#ifdef WIN32
	if (DeleteFile( path ) )
#else
	if (!unlink(path))
#endif
	{
		sint32 one = g_loadSaveMapWindow->GetListOne()->GetSelectedItemIndex();
		sint32 two = g_loadSaveMapWindow->GetListTwo()->GetSelectedItemIndex();

		g_loadSaveMapWindow->SetType( g_loadSaveMapWindow->GetType() );

		g_loadSaveMapWindow->GetListOne()->SelectItem( one );

		if ( two && two == g_loadSaveMapWindow->GetListTwo()->NumItems() ) --two;
		g_loadSaveMapWindow->GetListTwo()->SelectItem( two );
	}
	else
	{
		Assert( "Couldn't delete file." == 0 );
	}
}


void loadsavemapscreen_ListOneHandler(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	c3_ListBox	*list = (c3_ListBox *)control;
	if (list == NULL) return;

	LSMGameMapsListItem *item = (LSMGameMapsListItem *)list->GetSelectedItem();
	if (item == NULL)
	{
		g_loadSaveMapWindow->SetGameMapInfo(NULL);

		g_loadSaveMapWindow->SetType( g_loadSaveMapWindow->GetType() );
	}
	else
	{

		g_loadSaveMapWindow->SetGameMapInfo(item->GetGameMapInfo());
	}

	if ( !g_loadSaveMapWindow->GetListTwo()->GetSelectedItem() )
	{
		switch ( g_loadSaveMapWindow->GetType() )
		{
		case LSMS_LOAD_GAMEMAP:
			g_loadSaveMapWindow->GetOkButton()->Enable( FALSE );
			break;

		default:
			g_loadSaveMapWindow->GetOkButton()->Enable( TRUE );
			break;
		}

		g_loadSaveMapWindow->GetDeleteButton()->Enable( FALSE );
	}
}


void loadsavemapscreen_ListTwoHandler(aui_Control *control, uint32 action, uint32 data, void *cookie )
{

	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	c3_ListBox	*list = (c3_ListBox *)control;
	if (list == NULL) return;

	LSMSaveMapsListItem *item = (LSMSaveMapsListItem *)list->GetSelectedItem();
	if (item == NULL)
	{
		switch ( g_loadSaveMapWindow->GetType() )
		{
		case LSMS_LOAD_GAMEMAP:
			g_loadSaveMapWindow->GetOkButton()->Enable( FALSE );
			break;

		default:
			g_loadSaveMapWindow->GetOkButton()->Enable( TRUE );
			break;
		}

		g_loadSaveMapWindow->SetSaveMapInfo(NULL);

		g_loadSaveMapWindow->SetType( g_loadSaveMapWindow->GetType() );

		g_loadSaveMapWindow->GetDeleteButton()->Enable( FALSE );
	}
	else
	{
		SaveMapInfo	*info = item->GetSaveMapInfo();
		if (info == NULL) return;

		g_loadSaveMapWindow->SetSaveMapInfo(info);

		g_loadSaveMapWindow->GetOkButton()->Enable( TRUE );
		g_loadSaveMapWindow->GetDeleteButton()->Enable( TRUE );
	}
}
