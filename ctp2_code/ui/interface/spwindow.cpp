//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player screen
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
// ACTIVISION_ORIGINAL
// - When defined, generates the original Activision code.
// - When not defined, generates the modified Apolyton code.
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Removed the content of this file completely since it's no longer needed
//   (JJB)
//
//----------------------------------------------------------------------------
	
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
#include "c3window.h"
#include "ctp2_Button.h"
#include "ctp2_Button.h"
#include "c3_static.h"
#include "c3_listitem.h"
#include "c3_dropdown.h"
#include "StrDB.h"
#include "CivilisationDB.h"
#include "civilisationpool.h"

#include "spnewgamewindow.h" 
#include "spwindow.h"

#if defined(ACTIVISION_ORIGINAL)

extern StringDB						*g_theStringDB;
extern CivilisationDatabase			*g_theCivilisationDB;

SPWindow::SPWindow(AUI_ERRCODE *retval, uint32 id,
		MBCHAR *ldlBlock, sint32 bpp, AUI_WINDOW_TYPE type, bool bevel)
		: C3Window(retval,id,ldlBlock,bpp,type,bevel),
		m_tutorial(NULL), 
		m_newgame(NULL), 
		m_loadgame(NULL),
		m_return(NULL), 
		m_quit(NULL), 
		m_options(NULL), 
		m_background(NULL)
{
	m_newgame = spNew_ctp2_Button(retval,ldlBlock,"NewGameButton",spscreen_newgamePress);
	m_tutorial = spNew_ctp2_Button(retval,ldlBlock,"TutorialButton",spscreen_tutorialPress);

	m_tutorial->Enable(FALSE);

	m_loadgame = spNew_ctp2_Button(retval,ldlBlock,"LoadGameButton",spscreen_loadgamePress);
	m_return = spNew_ctp2_Button(retval,ldlBlock,"ReturnButton",spscreen_returnPress);
	m_quit = spNew_ctp2_Button(retval,ldlBlock,"QuitButton",spscreen_quitPress);
	m_options = spNew_ctp2_Button( retval, ldlBlock, "OptionsButton", spscreen_optionsPress);
	m_background = spNew_c3_Static(retval,ldlBlock,"Background");
}


SPWindow::~SPWindow()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };
	mycleanup(m_tutorial);
	mycleanup(m_newgame);
	mycleanup(m_loadgame);
	mycleanup(m_return);
	mycleanup(m_quit);
	mycleanup( m_options );
	mycleanup(m_background);
#undef mycleanup
}
#endif // defined(ACTIVISION_ORIGINAL)
