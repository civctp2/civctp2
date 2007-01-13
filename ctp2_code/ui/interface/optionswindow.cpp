
#include "c3.h"


#include "aui.h"
#include "aui_ldl.h"
#include "aui_surface.h"
#include "aui_uniqueid.h"
#include "aui_imagebase.h"
#include "aui_textbase.h"
#include "aui_textfield.h"
#include "aui_stringtable.h"
#include "c3ui.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "StrDB.h"
#include "CivilisationDB.h"
#include "CivilisationPool.h"
#include "network.h"

#include "spnewgamewindow.h" 
#include "optionswindow.h"

extern StringDB						*g_theStringDB;
extern CivilisationDatabase			*g_theCivilisationDB;
extern Network						g_network;









OptionsWindow::OptionsWindow( AUI_ERRCODE *retval, uint32 id, MBCHAR *ldlBlock, sint32 bpp,
							 AUI_WINDOW_TYPE type, bool bevel) :
c3_PopupWindow(retval,id,ldlBlock,bpp,type,bevel)
{
	m_graphics = m_sound = m_music = m_newgame = m_savegame = m_loadgame =
		m_restart = m_gameplay = m_mapeditor = m_keyboard =
		m_quittoshell = NULL;

	m_configHeader = m_gameHeader = NULL;

	m_graphics = spNew_ctp2_Button(retval,ldlBlock,"GraphicsButton",optionsscreen_graphicsPress);
	m_sound = spNew_ctp2_Button(retval,ldlBlock,"SoundButton",optionsscreen_soundPress);
	m_music = spNew_ctp2_Button(retval,ldlBlock,"MusicButton",optionsscreen_musicPress);
	m_newgame = spNew_ctp2_Button(retval,ldlBlock,"NewGameButton",optionsscreen_quitToShellPress);
	m_savegame = spNew_ctp2_Button(retval,ldlBlock,"SaveGameButton",optionsscreen_savegamePress);
	m_loadgame = spNew_ctp2_Button(retval,ldlBlock,"LoadGameButton",optionsscreen_loadgamePress);
	m_restart = spNew_ctp2_Button(retval,ldlBlock,"RestartButton",optionsscreen_restartPress);
	m_gameplay = spNew_ctp2_Button(retval,ldlBlock,"GamePlayButton",optionsscreen_gameplayPress);
	m_mapeditor = spNew_ctp2_Button(retval,ldlBlock,"MapEditorButton",optionsscreen_mapeditorPress);
	m_keyboard = spNew_ctp2_Button( retval, ldlBlock, "KeyboardButton", optionsscreen_keyboardPress );
	m_quittoshell = spNew_ctp2_Button( retval, ldlBlock, "QuitToShellButton", optionsscreen_quitPress );

	
	m_configHeader = spNew_c3_Static(retval,ldlBlock,"ConfigHeader");
	m_gameHeader = spNew_c3_Static(retval,ldlBlock,"GameHeader");



	



	AddClose( optionsscreen_returnPress );

}


OptionsWindow::~OptionsWindow()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	mycleanup( m_quittoshell );
	mycleanup( m_keyboard );
	mycleanup(m_mapeditor);
	mycleanup(m_gameplay);


	mycleanup(m_restart);
	mycleanup(m_loadgame);
	mycleanup(m_savegame);


	mycleanup(m_newgame);
	mycleanup(m_music);
	mycleanup(m_sound);
	mycleanup(m_graphics);

	mycleanup(m_configHeader);
	mycleanup(m_gameHeader);


	



#undef mycleanup
}


sint32 OptionsWindow::EnableButtons( void ) 
{
	
	
	m_newgame->Enable( TRUE );
	m_savegame->Enable( TRUE );


	m_restart->Enable( TRUE );
	m_mapeditor->Enable( TRUE );
	m_quittoshell->Enable( TRUE );

	return 0;
}


sint32 OptionsWindow::DisableButtons( void )
{
	
	
	m_newgame->Enable( FALSE );
	m_savegame->Enable( FALSE );


	m_restart->Enable( FALSE );
	m_mapeditor->Enable( FALSE );
	m_quittoshell->Enable( FALSE );

	return 0;
}


void OptionsWindow::RemoveQuitToWindowsButton( void )
{
	if ( m_quittoshell && GetChild( m_quittoshell->Id() ) )
		RemoveChild( m_quittoshell->Id() );
}


void OptionsWindow::AddQuitToWindowsButton( void )
{
	if ( m_quittoshell && !GetChild( m_quittoshell->Id() ) )
		AddChild( m_quittoshell );
}
