//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Collection of all setup windows.
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
// _DEBUG
// - Generate debug version when set.
//
// LOCKSETTINGSONLAUNCH
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Memory leak repaired.
// - Replaced old civilisation database by new one. (Aug 21st 2005 Martin G�hmann)
// - The ages in the summary are now displayed correctly.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Standardized code. (May 29th 2006 Martin G�hmann)
// - Replaced old civ selection button bank by list box. (2-Jan-2008 Martin G�hmann)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "allinonewindow.h"

#include <algorithm>
#include <chrono>
#include <thread>

#include "AgeRecord.h"
#include "agesscreen.h"
#include "aui_blitter.h"
#include "aui_ldl.h"
#include "aui_radio.h"
#include "aui_screen.h"
#include "aui_static.h"
#include "aui_stringtable.h"
#include "aui_tabgroup.h"
#include "aui_uniqueid.h"
#include "c3_button.h"
#include "c3_static.h"
#include "c3textfield.h"
#include "CivilisationPool.h"
#include "CivilisationRecord.h"
#include "CivPaths.h"                      // g_civPaths
#include "civscenarios.h"
#include "ctp2_dropdown.h"
#include "ctp2_spinner.h"
#include "ctp2_Static.h"
#include "ctp2_Switch.h"
#include "custommapscreen.h"
#include "dialogboxwindow.h"
#include "Exclusions.h"
#include "gamefile.h"
#include "gameselectwindow.h"
#include "loadsavewindow.h"
#include "lobbywindow.h"
#include "netshell.h"
#include "network.h"
#include "ns_chatbox.h"
#include "ns_civlistbox.h"
#include "ns_customlistbox.h"
#include "ns_item.h"
#include "ns_string.h"
#include "ns_tribes.h"
#include "passwordscreen.h"
#include "pixelutils.h"
#include "playereditwindow.h"
#include "playerselectwindow.h"
#include "primitives.h"
#include "profileDB.h"                     // g_theProfileDB
#include "spnewgamediffscreen.h"
#include "spnewgamemapsizescreen.h"
#include "spnewgamemapshapescreen.h"
#include "spnewgametribescreen.h"
#include "spnewgamewindow.h"
#include "StrDB.h"                         // g_theStringDB
#include "textradio.h"
#include "textswitch.h"
#include "texttab.h"

extern CivScenarios *g_civScenarios;
extern Exclusions *g_exclusions;
extern sint32 g_is565Format;
extern Network g_network;
extern LoadSaveWindow *g_loadsaveWindow;
extern aui_Radio *s_maleRadio;

static DialogBoxWindow *s_dbw = NULL;
AllinoneWindow *g_allinoneWindow = NULL;
DialogBoxWindow *g_rulesWindow = NULL;
DialogBoxWindow *g_exclusionsWindow = NULL;

#ifdef _DEBUG
#define DEBUG_PushChatMessage(arg) (g_netfunc->PushChatMessage("DEBUG: " arg))
#else
#define DEBUG_PushChatMessage(arg) ((void)0)
#endif



#ifdef LINUX
#include <unistd.h>
#endif

#define k_GUIDDATA_ID 2
#define k_SCENARIO_INFO_ID 3

#define LOCKSETTINGSONLAUNCH 1

void AllinoneWindow_SetupGameForLaunch( void );

namespace
{

bool IsValidTribeIndex(sint32 a_Index)
{
    Assert((a_Index >= 0) && (a_Index < k_TRIBES_MAX));
    return (a_Index >= 0) && (a_Index < k_TRIBES_MAX);
}

} // namespace

AllinoneWindow::AllinoneWindow(
	AUI_ERRCODE *retval )
	:
	ns_Window(
		retval,
		aui_UniqueId(),
		"allinonewindow",
		0,
		AUI_WINDOW_TYPE_STANDARD )
{
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = InitCommon();
	Assert( AUI_SUCCESS(*retval) );
	if ( !AUI_SUCCESS(*retval) ) return;

	*retval = CreateControls();
	Assert( AUI_SUCCESS(*retval) );
}


AUI_ERRCODE AllinoneWindow::InitCommon( void )
{
    if (!g_allinoneWindow)
    {
	    g_allinoneWindow = this;
    }
	m_numControls = CONTROL_MAX;
	m_controls = new aui_Control *[CONTROL_MAX];
	std::fill(m_controls, m_controls + CONTROL_MAX, (aui_Control *) NULL);

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	g_rulesWindow = new DialogBoxWindow(
		&errcode,
		"ruleswindow",
		NULL );

	g_exclusionsWindow = new DialogBoxWindow(
		&errcode,
		"exclusionswindow",
		NULL );

	memset( m_lname, 0, sizeof( m_lname ) );





	m_isScenarioGame = FALSE;

	m_createdExclusions = false;
	m_shouldUpdateGame = false;
	m_shouldUpdatePlayer = false;
	m_shouldUpdateAIPlayer = false;
	m_shouldAddAIPlayer = 0;
	m_joinedGame = false;

	m_tickGame = m_tickPlayer = m_tickAIPlayer = GetTickCount();

	m_aiplayerList = new tech_WLList<nf_AIPlayer *>;

	m_messageRequestDenied = new ns_String("strings.system.requestdenied");
	m_messageKicked = new ns_String("strings.system.kicked");
	m_messageGameSetup = new ns_String("strings.system.gamesetup");
	m_messageGameEnter = new ns_String("strings.system.gameenter");
	m_messageGameHost = new ns_String("strings.system.gamehost");
	m_messageGameCreate = new ns_String("strings.system.gamecreate");
	m_messageLaunched = new ns_String("strings.system.launched");

	m_receivedGuids = false;

	m_playStyleValueStrings = new aui_StringTable(
		&errcode,
		"strings.playstylevalues" );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return AUI_ERRCODE_HACK;

	m_PPTStrings = new aui_StringTable(
		&errcode,
		"strings.ppt" );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return AUI_ERRCODE_HACK;

	m_dbActionArray[ 0 ] = new DialogBoxPopDownAction;

	agesscreen_Initialize( AllinoneAgesCallback );
	spnewgamemapsizescreen_Initialize( AllinoneMapSizeCallback );
	spnewgamemapshapescreen_Initialize( AllinoneWorldShapeCallback );
	spnewgametribescreen_Initialize( AllinoneTribeCallback );

	custommapscreen_Initialize( AllinoneWorldTypeCallback );
	spnewgamediffscreen_Initialize( AllinoneDifficultyCallback );

	m_numAvailUnits = 0;
	memset( m_units, 0, sizeof( m_units ) );
	m_numAvailImprovements = 0;
	memset( m_improvements, 0, sizeof( m_improvements ) );
	m_numAvailWonders = 0;
	memset( m_wonders, 0, sizeof( m_wonders ) );

	return AUI_ERRCODE_OK;
}


AUI_ERRCODE AllinoneWindow::CreateControls( void )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;




	aui_Control *control;









	control = new ns_ChatBox(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.chatbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CHATBOX ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERSSHEET ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYERSTITLESTATICTEXT ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.gamenamestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GAMENAMESTATICTEXT ] = control;

	control = new C3TextField(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.gamenametextfield" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GAMENAMETEXTFIELD ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.playstylestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYSTYLESTATICTEXT ] = control;

	control = new ctp2_DropDown(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.playstyledropdown" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYSTYLEDROPDOWN ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.playstylevaluestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYSTYLEVALUESTATICTEXT ] = control;

	control = new ctp2_Spinner(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.playstylevaluespinner" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PLAYSTYLEVALUESPINNER ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"allinonewindow",
		"reviewbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_REVIEWBUTTON ] = control;

	control = new ctp2_Switch(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.lockswitch" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_LOCKSWITCH ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"allinonewindow",
		"playerssheet.addaibutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_ADDAIBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"allinonewindow",
		"playerssheet.rulesbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_RULESBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"allinonewindow",
		"playerssheet.exclusionsbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EXCLUSIONSBUTTON ] = control;

	control = new ns_HPlayerListBox(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.playerssheet.hplayerslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_HPLAYERSLISTBOX ] = control;

	control = new ns_GPlayerListBox(
		&errcode,
		aui_UniqueId(),
		"hack.aiplayerslistbox",
		(ns_HPlayerListBox *)m_controls[ CONTROL_HPLAYERSLISTBOX ] );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GPLAYERSLISTBOX ] = control;

	control = new ns_AIPlayerListBox(
		&errcode,
		aui_UniqueId(),
		"hack.aiplayerslistbox",
		(ns_HPlayerListBox *)m_controls[ CONTROL_HPLAYERSLISTBOX ] );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_AIPLAYERSLISTBOX ] = control;

	control = new ctp2_Switch(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.publicprivateteamswitch" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PPTSWITCH ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"allinonewindow",
		"kickbutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_KICKBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"allinonewindow",
		"infobutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_INFOBUTTON ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_RULESSHEET ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulesokbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_RULESOKBUTTON ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_RULESTITLESTATICTEXT ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.titlestatictext2" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_RULESTITLESTATICTEXT2 ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"ruleswindow",
		"rulessheet.agesbutton",
		NULL);

	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_AGESBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"ruleswindow",
		"rulessheet.mapsizebutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_MAPSIZEBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"ruleswindow",
		"rulessheet.worldtypebutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_WORLDTYPEBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"ruleswindow",
		"rulessheet.worldshapebutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_WORLDSHAPEBUTTON ] = control;

	control = spNew_ctp2_Button(
		&errcode,
		"ruleswindow",
		"rulessheet.difficultybutton",
		NULL);
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_DIFFICULTYBUTTON ] = control;

	control = new aui_Switch(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.dynamicjoinswitch" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_DYNAMICJOINSWITCH ] = control;

	control = new aui_Switch(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.handicappingswitch" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_HANDICAPPINGSWITCH ] = control;

	control = new aui_Switch(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.bloodlustswitch" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_BLOODLUSTSWITCH ] = control;

	control = new aui_Switch(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.pollutionswitch" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_POLLUTIONSWITCH ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.goldstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_GOLDSTATICTEXT ] = control;

	control = new c3_EditButton(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.civpointsbutton",
		NULL,
		new CivPointsButtonAction );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CIVPOINTSBUTTON ] = control;

	control = new c3_Static(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.pwstatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PWSTATICTEXT ] = control;

	control = new c3_EditButton(
		&errcode,
		aui_UniqueId(),
		"ruleswindow.rulessheet.pwpointsbutton",
		NULL,
		new PwPointsButtonAction );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_PWPOINTSBUTTON ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EXCLUSIONSSHEET ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionsokbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EXCLUSIONSOKBUTTON ] = control;

	control = new aui_Static(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.titlestatictext" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_EXCLUSIONSTITLESTATICTEXT ] = control;

	control = new aui_TabGroup(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_SMALLNASTYTABGROUP ] = control;

	control = new aui_Tab(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup.unitstab" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_UNITSTAB ] = control;

	control = new ns_CivListBox(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup.unitstab.pane.unitslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_UNITSLISTBOX ] = control;

	control = new aui_Tab(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup.improvementstab" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_IMPROVEMENTSTAB ] = control;

	control = new ns_CivListBox(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup.improvementstab.pane.improvementslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_IMPROVEMENTSLISTBOX ] = control;

	control = new aui_Tab(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup.wonderstab" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_WONDERSTAB ] = control;

	control = new ns_CivListBox(
		&errcode,
		aui_UniqueId(),
		"exclusionswindow.exclusionssheet.smallnastytabgroup.wonderstab.pane.wonderslistbox" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_WONDERSLISTBOX ] = control;


	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.okbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_OKBUTTON ] = control;

	control = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"allinonewindow.cancelbutton" );
	Assert( AUI_NEWOK(control,errcode) );
	if ( !AUI_NEWOK(control,errcode) ) return errcode;
	m_controls[ CONTROL_CANCELBUTTON ] = control;

	aui_Ldl::SetupHeirarchyFromRoot( "allinonewindow" );
	aui_Ldl::SetupHeirarchyFromRoot( "ruleswindow" );
	aui_Ldl::SetupHeirarchyFromRoot( "exclusionswindow" );

	m_controls[ CONTROL_GAMENAMETEXTFIELD ]->SetAction(new GameNameTextFieldAction);
	m_controls[ CONTROL_PPTSWITCH ]->SetAction(new PPTSwitchAction);
	m_controls[ CONTROL_KICKBUTTON ]->SetAction(new KickButtonAction);
	m_controls[ CONTROL_INFOBUTTON ]->SetAction(new InfoButtonAction);
	m_controls[ CONTROL_OKBUTTON ]->SetAction(new OKButtonAction);
	m_controls[ CONTROL_REVIEWBUTTON ]->SetAction(new ReviewButtonAction);
	m_controls[ CONTROL_RULESBUTTON ]->SetAction(new RulesButtonAction);
	m_controls[ CONTROL_EXCLUSIONSBUTTON ]->SetAction(new ExclusionsButtonAction);
	m_controls[ CONTROL_RULESOKBUTTON ]->SetAction(new RulesOKButtonAction);
	m_controls[ CONTROL_EXCLUSIONSOKBUTTON ]->SetAction(new ExclusionsOKButtonAction);
	m_controls[ CONTROL_LOCKSWITCH ]->SetAction(new LockSwitchAction);
	m_controls[ CONTROL_ADDAIBUTTON ]->SetAction(new AddAIButtonAction);
	m_controls[ CONTROL_CANCELBUTTON ]->SetAction(new CancelButtonAction);
	m_controls[ CONTROL_HPLAYERSLISTBOX ]->SetAction(new PlayersListBoxAction);
	m_controls[ CONTROL_PLAYSTYLEDROPDOWN ]->SetAction(new PlayStyleDropDownAction);

	((ctp2_Spinner *)m_controls[CONTROL_PLAYSTYLEVALUESPINNER])->SetSpinnerCallback(PlayStyleValueSpinnerCallback, NULL);

	m_controls[ CONTROL_DYNAMICJOINSWITCH ]->SetAction(new DynamicJoinSwitchAction);
	m_controls[ CONTROL_HANDICAPPINGSWITCH ]->SetAction(new HandicappingSwitchAction);
	m_controls[ CONTROL_BLOODLUSTSWITCH ]->SetAction(new BloodlustSwitchAction);
	m_controls[ CONTROL_POLLUTIONSWITCH ]->SetAction(new PollutionSwitchAction);
	m_controls[ CONTROL_AGESBUTTON ]->SetAction(new AgesButtonAction);
	m_controls[ CONTROL_MAPSIZEBUTTON ]->SetAction(new MapSizeButtonAction);
	m_controls[ CONTROL_WORLDTYPEBUTTON ]->SetAction(new WorldTypeButtonAction);
	m_controls[ CONTROL_WORLDSHAPEBUTTON ]->SetAction(new WorldShapeButtonAction);
	m_controls[ CONTROL_DIFFICULTYBUTTON ]->SetAction(new DifficultyButtonAction);

	((aui_ListBox *)m_controls[ CONTROL_HPLAYERSLISTBOX ])->GetHeader()->
		Enable( FALSE );

	((ns_CivListBox *)m_controls[ CONTROL_UNITSLISTBOX ])->
		m_selectableList = FALSE;
	((ns_CivListBox *)m_controls[ CONTROL_IMPROVEMENTSLISTBOX ])->
		m_selectableList = FALSE;
	((ns_CivListBox *)m_controls[ CONTROL_WONDERSLISTBOX ])->
		m_selectableList = FALSE;

	((aui_ListBox *)m_controls[ CONTROL_HPLAYERSLISTBOX ])->
		SetForceSelect( TRUE );

	m_controls[ CONTROL_PLAYERSSHEET ]->SetBlindness( true );

	m_controls[ CONTROL_RULESSHEET ]->SetBlindness( true );
	m_controls[ CONTROL_EXCLUSIONSSHEET ]->SetBlindness( true );

	ctp2_DropDown *playstyleDropDown =
		(ctp2_DropDown *)m_controls[ CONTROL_PLAYSTYLEDROPDOWN ];

	aui_StringTable st( &errcode, "strings.playstyles" );
	Assert( AUI_SUCCESS(errcode) );
	if ( !AUI_SUCCESS(errcode) ) return AUI_ERRCODE_HACK;

	sint32 numStyles = st.GetNumStrings();

	for ( sint32 i = 0; i < numStyles; i++ )
	{
		ns_ListItem *item = new ns_ListItem(
			&errcode,
			st.GetString( i ),
			"listitems.playstyleitem" );
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;









		playstyleDropDown->AddItem( (ctp2_ListItem *)item );

	}

	aui_Switch *s = (aui_Switch *)m_controls[ CONTROL_PPTSWITCH ];
	s->SetState( s->GetState() );


	aui_Window *window = (aui_Window *)aui_Ldl::GetObject("ruleswindow");
	ctp2_Button *button = (ctp2_Button *)aui_Ldl::GetObject("ruleswindow", "rulesokbutton");
	if (button) {
		button->Move( window->Width() - button->Width() - 14, window->Height() - button->Height() - 17);
	}

	return AUI_ERRCODE_OK;
}


AllinoneWindow::~AllinoneWindow()
{
    allocated::clear(g_rulesWindow);
    allocated::clear(g_exclusionsWindow);

	sint32 numActions = sizeof( m_dbActionArray ) / sizeof( aui_Action *);
	sint32 i;
	for ( i = 0; i < numActions; i++ )
	{
		delete m_dbActionArray[i];
	}

	((aui_DropDown *)m_controls[ CONTROL_PLAYSTYLEDROPDOWN ])->
		GetListBox()->RemoveItems( TRUE );

	aui_ListBox * listbox =
		(aui_ListBox *)m_controls[ CONTROL_UNITSLISTBOX ];

	for ( i = listbox->NumItems(); i; i-- )
	{
		aui_Item *item = listbox->GetItemByIndex( 0 );
		listbox->RemoveItem( item->Id() );

		ListPos pos = item->ChildList()->GetHeadPosition();
		for ( sint32 j = item->ChildList()->L(); j; j-- )
		{
			aui_Item *subitem = (aui_Item *)item->ChildList()->GetNext( pos );

            delete subitem->GetAction();
			delete subitem;
		}

		delete item->GetAction();
		delete item;
	}

	listbox =
		(aui_ListBox *)m_controls[ CONTROL_IMPROVEMENTSLISTBOX ];
	for ( i = listbox->NumItems(); i; i-- )
	{
		aui_Item *item = listbox->GetItemByIndex( 0 );
		listbox->RemoveItem( item->Id() );

		ListPos pos = item->ChildList()->GetHeadPosition();
		for ( sint32 j = item->ChildList()->L(); j; j-- )
		{
			aui_Item *subitem = (aui_Item *)item->ChildList()->GetNext( pos );

			delete subitem->GetAction();
			delete subitem;
		}

		delete item->GetAction();
		delete item;
	}

	listbox =
		(aui_ListBox *)m_controls[ CONTROL_WONDERSLISTBOX ];
	for ( i = listbox->NumItems(); i; i-- )
	{
		aui_Item *item = listbox->GetItemByIndex( 0 );
		listbox->RemoveItem( item->Id() );

		ListPos pos = item->ChildList()->GetHeadPosition();
		for ( sint32 j = item->ChildList()->L(); j; j-- )
		{
			aui_Item *subitem = (aui_Item *)item->ChildList()->GetNext( pos );

			delete subitem->GetAction();
			delete subitem;
		}

		delete item->GetAction();
		delete item;
	}

	delete m_aiplayerList;
	delete m_messageKicked;
	delete m_messageGameSetup;
	delete m_messageGameEnter;
	delete m_messageGameCreate;
	delete m_messageGameHost;
	delete m_messageLaunched;
	delete m_playStyleValueStrings;
	delete m_PPTStrings;
	delete m_messageRequestDenied;

	agesscreen_Cleanup();
	spnewgamemapsizescreen_Cleanup();
	spnewgamemapshapescreen_Cleanup();
	spnewgametribescreen_Cleanup();

	custommapscreen_Cleanup();
	spnewgamediffscreen_Cleanup();

    if (this == g_allinoneWindow)
    {
	    g_allinoneWindow = NULL;
    }
}


AUI_ERRCODE AllinoneWindow::DrawThis(
	aui_Surface *surface,
	sint32 x,
	sint32 y )
{
	(void) ns_Window::DrawThis( surface, x, y );
	return AUI_ERRCODE_OK;
}


AUI_ERRCODE AllinoneWindow::CreateExclusions( void )
{
	if ( m_createdExclusions )
	{
		sint32			i;
		aui_ListBox *	listbox = (aui_ListBox *)m_controls[CONTROL_UNITSLISTBOX];

		for (i = listbox->NumItems(); i; i-- )
		{
			aui_Item *item = listbox->GetItemByIndex( 0 );
			listbox->RemoveItem( item->Id() );

			ListPos pos = item->ChildList()->GetHeadPosition();
			for ( sint32 j = item->ChildList()->L(); j; j-- )
			{
				aui_Item *subitem =
					(aui_Item *)item->ChildList()->GetNext( pos );

				delete subitem->GetAction();
				delete subitem;
			}

			delete item->GetAction();
			delete item;
		}

		listbox =
			(aui_ListBox *)m_controls[ CONTROL_IMPROVEMENTSLISTBOX ];
		for ( i = listbox->NumItems(); i; i-- )
		{
			aui_Item *item = listbox->GetItemByIndex( 0 );
			listbox->RemoveItem( item->Id() );

			ListPos pos = item->ChildList()->GetHeadPosition();
			for ( sint32 j = item->ChildList()->L(); j; j-- )
			{
				aui_Item *subitem =
					(aui_Item *)item->ChildList()->GetNext( pos );

				delete subitem->GetAction();
				delete subitem;
			}

			delete item->GetAction();
			delete item;
		}

		listbox =
			(aui_ListBox *)m_controls[ CONTROL_WONDERSLISTBOX ];
		for ( i = listbox->NumItems(); i; i-- )
		{
			aui_Item *item = listbox->GetItemByIndex( 0 );
			listbox->RemoveItem( item->Id() );

			ListPos pos = item->ChildList()->GetHeadPosition();
			for ( sint32 j = item->ChildList()->L(); j; j-- )
			{
				aui_Item *subitem =
					(aui_Item *)item->ChildList()->GetNext( pos );

				delete subitem->GetAction();
				delete subitem;
			}

			delete item->GetAction();
			delete item;
		}

		m_numAvailUnits = 0;
		memset( m_units, 0, sizeof( m_units ) );
		m_numAvailImprovements = 0;
		memset( m_improvements, 0, sizeof( m_improvements ) );
		m_numAvailWonders = 0;
		memset( m_wonders, 0, sizeof( m_wonders ) );

		m_createdExclusions = false;
	}

	aui_ListBox *listbox =
		(aui_ListBox *)m_controls[ CONTROL_UNITSLISTBOX ];
	listbox->SetAbsorbancy( FALSE );
	sint32 height = listbox->Height();

	m_numAvailUnits = g_nsUnits->GetStrings()->GetNumStrings();
	g_gamesetup.SetNumAvailUnits( m_numAvailUnits );

	aui_Switch *item = NULL;
	tech_WLList<aui_Switch *> unitList;
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	sint32 i;
	for ( i = 0; i < m_numAvailUnits; i++ )
	{
		if ( !g_nsUnits->m_noIndex[ i ] )
		{
			item = new aui_Switch(
				&errcode,
				aui_UniqueId(),
				"listitems.exclusioncheckbox" );
			Assert( AUI_NEWOK(item,errcode) );
			if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

			item->SetText( g_nsUnits->GetStrings()->GetString( i ) );
			item->SetAction(new UnitExclusionAction(i));

			m_units[ i ] = item;

			if ( (height -= item->Height()) < 0 )
				break;

			unitList.AddTail( item );
		}
	}

	ListPos pos = unitList.GetHeadPosition();

	for ( i++; i < m_numAvailUnits; i++ )
	{
		if ( !g_nsUnits->m_noIndex[ i ] )
		{
			unitList.GetNext( pos )->AddChild( item );

			item = new aui_Switch(
				&errcode,
				aui_UniqueId(),
				"listitems.exclusioncheckbox" );
			Assert( AUI_NEWOK(item,errcode) );
			if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

			item->SetText( g_nsUnits->GetStrings()->GetString( i ) );
			item->SetAction(new UnitExclusionAction(i));

			m_units[ i ] = item;

			if ( !pos )
				pos = unitList.GetHeadPosition();
		}
	}

	unitList.GetAt( pos )->AddChild( item );
	pos = unitList.GetHeadPosition();
	for ( i = unitList.L(); i; i-- )
		listbox->AddItem( (aui_Item *)unitList.GetNext( pos ) );




	listbox = (aui_ListBox *)m_controls[ CONTROL_IMPROVEMENTSLISTBOX ];
	listbox->SetAbsorbancy( FALSE );
	height = listbox->Height();

	tech_WLList<aui_Switch *> improvementList;

	m_numAvailImprovements = g_nsImprovements->GetStrings()->GetNumStrings();
	g_gamesetup.SetNumAvailImprovements( m_numAvailImprovements );
	for ( i = 0; i < m_numAvailImprovements; i++ )
	{
		item = new aui_Switch(
			&errcode,
			aui_UniqueId(),
			"listitems.exclusioncheckbox" );
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

		item->SetText( g_nsImprovements->GetStrings()->GetString( i ) );
		item->SetAction(new ImprovementExclusionAction(i));

		m_improvements[ i ] = item;

		if ( (height -= item->Height()) < 0 )
			break;

		improvementList.AddTail( item );
	}

	pos = improvementList.GetHeadPosition();

	for ( i++; i < m_numAvailImprovements; i++ )
	{
		improvementList.GetNext( pos )->AddChild( item );

		item = new aui_Switch(
			&errcode,
			aui_UniqueId(),
			"listitems.exclusioncheckbox" );
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

		item->SetText( g_nsImprovements->GetStrings()->GetString( i ) );
		item->SetAction(new ImprovementExclusionAction(i));

		m_improvements[ i ] = item;

		if ( !pos )
			pos = improvementList.GetHeadPosition();
	}

	improvementList.GetAt( pos )->AddChild( item );
	pos = improvementList.GetHeadPosition();
	for ( i = improvementList.L(); i; i-- )
		listbox->AddItem( (aui_Item *)improvementList.GetNext( pos ) );




	listbox = (aui_ListBox *)m_controls[ CONTROL_WONDERSLISTBOX ];
	listbox->SetAbsorbancy( FALSE );
	height = listbox->Height();

	tech_WLList<aui_Switch *> wonderList;

	m_numAvailWonders = g_nsWonders->GetStrings()->GetNumStrings();
	g_gamesetup.SetNumAvailWonders( m_numAvailWonders );
	for ( i = 0; i < m_numAvailWonders; i++ )
	{
		item = new aui_Switch(
			&errcode,
			aui_UniqueId(),
			"listitems.exclusioncheckbox" );
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

		item->SetText( g_nsWonders->GetStrings()->GetString( i ) );
		item->SetAction(new WonderExclusionAction(i));

		m_wonders[ i ] = item;

		if ( (height -= item->Height()) < 0 )
			break;

		wonderList.AddTail( item );
	}

	pos = wonderList.GetHeadPosition();

	for ( i++; i < m_numAvailWonders; i++ )
	{
		wonderList.GetNext( pos )->AddChild( item );

		item = new aui_Switch(
			&errcode,
			aui_UniqueId(),
			"listitems.exclusioncheckbox" );
		Assert( AUI_NEWOK(item,errcode) );
		if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

		item->SetText( g_nsWonders->GetStrings()->GetString( i ) );
		item->SetAction(new WonderExclusionAction(i));

		m_wonders[ i ] = item;

		if ( !pos )
			pos = wonderList.GetHeadPosition();
	}

	wonderList.GetAt( pos )->AddChild( item );
	pos = wonderList.GetHeadPosition();
	for ( i = wonderList.L(); i; i-- )
		listbox->AddItem( (aui_Item *)wonderList.GetNext( pos ) );

	m_createdExclusions = true;

	uint8 isScenario = m_scenInfo.isScenario;

	if ( m_mode == JOIN ||
		 m_mode == CONTINUE_CREATE ||
		 m_mode == CONTINUE_JOIN )
		SetMode( m_mode );
	m_scenInfo.isScenario = isScenario;

	return AUI_ERRCODE_OK;
}


void AllinoneWindow::SetMode( Mode m )
{

	aui_Control *pane;
	sint32 i;
	ListPos pos;













	m_scenInfo.isScenario = FALSE;

	switch( m_mode = m )
	{
	case CREATE:
		m_controls[ CONTROL_KICKBUTTON ]->Enable( !g_playersetup.IsReadyToLaunch() );;
		m_controls[ CONTROL_GAMENAMETEXTFIELD ]->Enable( !g_playersetup.IsReadyToLaunch() );
		m_controls[ CONTROL_PLAYSTYLEDROPDOWN ]->Enable( !g_playersetup.IsReadyToLaunch() );
		m_controls[ CONTROL_PLAYSTYLEVALUESPINNER ]->Enable( !g_playersetup.IsReadyToLaunch() );
		m_controls[ CONTROL_LOCKSWITCH ]->Enable( !g_playersetup.IsReadyToLaunch() );
		m_controls[ CONTROL_ADDAIBUTTON ]->Enable( !g_playersetup.IsReadyToLaunch() );

		m_controls[ CONTROL_CIVPOINTSBUTTON ]->Enable( !g_playersetup.IsReadyToLaunch() );
		m_controls[ CONTROL_PWPOINTSBUTTON ]->Enable( !g_playersetup.IsReadyToLaunch() );


		m_controls[ CONTROL_RULESSHEET ]->Enable( !g_playersetup.IsReadyToLaunch() );

		pane = ((aui_ListBox *)m_controls[ CONTROL_UNITSLISTBOX ])
			->GetPane();
		pos = pane->ChildList()->GetHeadPosition();
		for ( i = pane->ChildList()->L(); i; i-- )
			pane->ChildList()->GetNext( pos )->Enable( !g_playersetup.IsReadyToLaunch() );

		pane = ((aui_ListBox *)m_controls[ CONTROL_IMPROVEMENTSLISTBOX ])
			->GetPane();
		pos = pane->ChildList()->GetHeadPosition();
		for ( i = pane->ChildList()->L(); i; i-- )
			pane->ChildList()->GetNext( pos )->Enable( !g_playersetup.IsReadyToLaunch() );

		pane = ((aui_ListBox *)m_controls[ CONTROL_WONDERSLISTBOX ])
			->GetPane();
		pos = pane->ChildList()->GetHeadPosition();
		for ( i = pane->ChildList()->L(); i; i-- )
			pane->ChildList()->GetNext( pos )->Enable( !g_playersetup.IsReadyToLaunch() );

		break;

	case JOIN:
	case CONTINUE_JOIN:
	case CONTINUE_CREATE:
		m_controls[ CONTROL_KICKBUTTON ]->Enable( false );
		if(m_mode != CONTINUE_CREATE) {
			m_controls[ CONTROL_GAMENAMETEXTFIELD ]->Enable( false );
		} else {
			m_controls[ CONTROL_GAMENAMETEXTFIELD ]->Enable( !g_playersetup.IsReadyToLaunch() );
		}
		m_controls[ CONTROL_PLAYSTYLEDROPDOWN ]->Enable( false );
		m_controls[ CONTROL_PLAYSTYLEVALUESPINNER ]->Enable( false );
		m_controls[ CONTROL_LOCKSWITCH ]->Enable( false );
		m_controls[ CONTROL_ADDAIBUTTON ]->Enable( false );

		m_controls[ CONTROL_CIVPOINTSBUTTON ]->Enable( false );
		m_controls[ CONTROL_PWPOINTSBUTTON ]->Enable( false );


		m_controls[ CONTROL_RULESSHEET ]->Enable( false );

		if(m_mode == CONTINUE_CREATE) {

			m_controls[ CONTROL_DYNAMICJOINSWITCH ]->Enable(true);
		}

		m_controls[ CONTROL_RULESOKBUTTON ]->Enable( true );

		m_controls[ CONTROL_MAPSIZEBUTTON ]->Enable( true );
		m_controls[ CONTROL_WORLDTYPEBUTTON ]->Enable( true );
		m_controls[ CONTROL_WORLDSHAPEBUTTON ]->Enable( true );
		m_controls[ CONTROL_DIFFICULTYBUTTON ]->Enable( true );
		m_controls[ CONTROL_AGESBUTTON ]->Enable( true );

		pane = ((aui_ListBox *)m_controls[ CONTROL_UNITSLISTBOX ])
			->GetPane();
		pos = pane->ChildList()->GetHeadPosition();
		for ( i = pane->ChildList()->L(); i; i-- )
			pane->ChildList()->GetNext( pos )->Enable( false );

		pane = ((aui_ListBox *)m_controls[ CONTROL_IMPROVEMENTSLISTBOX ])
			->GetPane();
		pos = pane->ChildList()->GetHeadPosition();
		for ( i = pane->ChildList()->L(); i; i-- )
			pane->ChildList()->GetNext( pos )->Enable( false );

		pane = ((aui_ListBox *)m_controls[ CONTROL_WONDERSLISTBOX ])
			->GetPane();
		pos = pane->ChildList()->GetHeadPosition();
		for ( i = pane->ChildList()->L(); i; i-- )
			pane->ChildList()->GetNext( pos )->Enable( false );

		break;

	default:

		Assert( FALSE );
		break;
	}

	if ( m_mode == CONTINUE_CREATE )
	{
		m_controls[ CONTROL_PLAYSTYLEDROPDOWN ]->Enable( true );
		m_controls[ CONTROL_PLAYSTYLEVALUESPINNER ]->Enable( true );




	}





}


BOOL AllinoneWindow::IsMine( NETFunc::Player *player )
{
	return g_netfunc->GetPlayer()->Equals( player );
}


ns_HPlayerItem *AllinoneWindow::GetHPlayerFromId( dpid_t id )
{
	ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)
		m_controls[ CONTROL_HPLAYERSLISTBOX ];

	ListPos pos = hplistbox->GetPane()->ChildList()->GetHeadPosition();
	for ( sint32 i = hplistbox->GetPane()->ChildList()->L(); i; i-- )
	{
		ns_HPlayerItem *hplayer = (ns_HPlayerItem *)hplistbox->GetPane()->
			ChildList()->GetNext( pos );

		if ( !hplayer->IsAI() )
			if ( hplayer->GetPlayer()->GetId() == id )
				return hplayer;
	}

	return NULL;
}


NETFunc::Player *AllinoneWindow::GetPlayerFromKey( uint16 key )
{
	ns_GPlayerListBox *listbox = (ns_GPlayerListBox *)
		m_controls[ CONTROL_GPLAYERSLISTBOX ];

	ListPos pos = listbox->GetPane()->ChildList()->GetHeadPosition();
	for ( sint32 i = listbox->GetPane()->ChildList()->L(); i; i-- )
	{
		NETFunc::Player *player = (NETFunc::Player *)
			((ns_Item<NETFunc::Player, ns_Player> *)
			 listbox->GetPane()->ChildList()->GetNext( pos ))->
			GetNetShellObject()->GetNETFuncObject();

		if ( *(uint16 *)player->GetKey()->buf == key )
			return player;
	}

	return NULL;
}


nf_AIPlayer *AllinoneWindow::GetAIPlayerFromKey( uint16 key )
{
	ns_AIPlayerListBox *listbox = (ns_AIPlayerListBox *)
		m_controls[ CONTROL_AIPLAYERSLISTBOX ];

	ListPos pos = listbox->GetPane()->ChildList()->GetHeadPosition();
	for ( sint32 i = listbox->GetPane()->ChildList()->L(); i; i-- )
	{
		nf_AIPlayer *player = (nf_AIPlayer *)
			((ns_Item<nf_AIPlayer, ns_AIPlayer> *)
			 listbox->GetPane()->ChildList()->GetNext( pos ))->
			GetNetShellObject()->GetNETFuncObject();

		if ( *(uint16 *)player->GetKey()->buf == key )
			return player;
	}

	return NULL;
}


BOOL AllinoneWindow::WhoHasTribe( sint32 index, uint16 *curKey, BOOL *curIsAI, BOOL *curIsFemale )
{
	Assert( curKey != NULL );
	if ( !curKey ) return FALSE;

	*curKey = 0;

	Assert( curIsAI != NULL );
	if ( !curIsAI ) return FALSE;

	*curIsAI = FALSE;

	Assert( curIsFemale != NULL );
	if ( !curIsFemale ) return FALSE;

	*curIsFemale = FALSE;

	if (!IsValidTribeIndex(index))
	{
		return FALSE;
	}

	Assert( g_netfunc->IsHost() );
	if ( !g_netfunc->IsHost() ) return FALSE;

	if ( index == 0 ) return TRUE;

	if ( index == 1 )
	{
		*curKey = 0xffff;
		*curIsAI = TRUE;
		*curIsFemale = FALSE;
		return TRUE;
	}

	TribeSlot *tribeSlots = g_gamesetup.GetTribeSlots();
	for (int i = 0; i < k_NS_MAX_PLAYERS; i++ )
	{
		if ( tribeSlots[ i ].tribe == index )
		{
			*curKey = tribeSlots[ i ].key;
			*curIsAI = tribeSlots[ i ].isAI;
			*curIsFemale = tribeSlots[ i ].isFemale;
			break;
		}
	}

	return TRUE;
}


sint32 AllinoneWindow::FindTribe( uint16 key, BOOL isAI, BOOL *isFemale )
{

	Assert( g_netfunc->IsHost() );
	if ( !g_netfunc->IsHost() ) return 0;

	if ( key )
	{
		TribeSlot *tribeSlots = g_gamesetup.GetTribeSlots();

		for (int i = 0; i < k_NS_MAX_PLAYERS; i++ )
			if ( tribeSlots[ i ].key == key && tribeSlots[ i ].isAI == isAI )
			{
				if ( isFemale ) *isFemale = tribeSlots[ i ].isFemale;
				return tribeSlots[ i ].tribe;
			}
	}

	else
	{
		if ( m_mode == CONTINUE_CREATE || m_mode == CONTINUE_JOIN )
		{

			for ( sint32 i = 2; i < g_nsTribes->GetNumTribes(); i++ )
			{
				uint16 curKey;
				BOOL curIsAI;
				BOOL curIsFemale;
				BOOL success = WhoHasTribe( i, &curKey, &curIsAI, &curIsFemale );
				Assert( success );

				if ( curKey == 0 )
				{
					if(!m_scenInfo.isScenario)
					{
						for ( sint32 j = 0; j < k_NS_MAX_PLAYERS; j++ )
						{
							if (
								g_gamesetup.GetSavedTribeSlots()[ j ].tribe == i &&
								g_gamesetup.GetSavedTribeSlots()[ j ].isAI == isAI )
							{
								if ( isFemale ) *isFemale = g_gamesetup.GetSavedTribeSlots()[ j ].isFemale;
								return i;
							}
						}
					} else {

						if((m_scenInfo.m_startInfoType == (uint8)STARTINFOTYPE_POSITIONSFIXED) ||

						   (m_scenInfo.m_startInfoType == (uint8)STARTINFOTYPE_NOLOCS)) {

							if ( isFemale ) *isFemale = curIsFemale;
							return i;
						}

						for( sint32 j = 0; j < m_scenInfo.m_numStartPositions; j++) {
							if(m_scenInfo.m_civs[j] == i - 1) {
								if(isFemale) *isFemale = curIsFemale;
								return i;
							}
						}
					}
				}
			}
		}
		else if(m_scenInfo.isScenario && m_scenInfo.m_haveSavedGame) {
			for(sint32 i = 0; i < k_MAX_PLAYERS; i++) {
				if(m_scenInfo.m_legalCivs[i] > 0) {
					uint16 curKey;
					BOOL curIsAI;
					BOOL curIsFemale;
					BOOL success = WhoHasTribe( m_scenInfo.m_legalCivs[i] + 1,
												&curKey, &curIsAI,
												&curIsFemale );
					Assert(success);
					if(curKey == 0) {
						if(isFemale) *isFemale = curIsFemale;
						return m_scenInfo.m_legalCivs[i] + 1;
					}
				}
			}
		}
		else
		{

			for ( sint32 i = 2; i < g_nsTribes->GetNumTribes(); i++ )
			{
				uint16 curKey;
				BOOL curIsAI;
				BOOL curIsFemale;
				BOOL success = WhoHasTribe( i, &curKey, &curIsAI, &curIsFemale );
				Assert( success );

				if ( curKey == 0 )
				{
					if ( isFemale ) *isFemale = curIsFemale;
					return i;
				}
			}
		}
	}

	return 0;
}


BOOL AllinoneWindow::AssignTribe(
	sint32 index,
	uint16 key,
	BOOL isAI,
	BOOL isFemale,
	BOOL unassign )
{

	Assert( index != 1 );
	if ( index == 1 ) return FALSE;

	if (!IsValidTribeIndex(index))
	{
		return FALSE;
	}

	Assert( g_netfunc->IsHost() );
	if ( !g_netfunc->IsHost() ) return FALSE;

	uint16 curKey;
	BOOL curIsAI;
	BOOL curIsFemale;
	BOOL success = WhoHasTribe( index, &curKey, &curIsAI, &curIsFemale );
	Assert( success );

	Assert( curKey == 0 || ( curKey == key && curIsAI == isAI ) );
	if ( curKey != 0 && ( curKey != key || curIsAI != isAI ) ) return FALSE;

	TribeSlot *tribeSlots = g_gamesetup.GetTribeSlots();
	sint32 i;
	for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
	{
		if ( tribeSlots[ i ].key == key && tribeSlots[ i ].isAI == isAI )
		{
			memset( tribeSlots + i, 0, sizeof( TribeSlot ) );
			break;
		}
	}

	if ( !unassign )
	{

		PLAYER_INDEX playerIndex = i;
		if ( playerIndex == k_NS_MAX_PLAYERS )
		{
			for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
			{
				if ( tribeSlots[ i ].key == 0 )
				{
					playerIndex = i;
					break;
				}
			}
		}

		Assert( playerIndex < k_NS_MAX_PLAYERS );
		if ( playerIndex >= k_NS_MAX_PLAYERS ) return FALSE;



























		tribeSlots[ playerIndex ].isFemale  = static_cast<sint8>(isFemale);
		tribeSlots[ playerIndex ].isAI      = static_cast<sint8>(isAI);
		tribeSlots[ playerIndex ].key       = key;
		tribeSlots[ playerIndex ].tribe     = index;

		if ( !isAI )
		{

			if ( *(uint16 *)g_playersetup.GetKey()->buf == key )
			{
				g_playersetup.SetTribe( index );
				UpdatePlayerSetup();
			}
			else
			{

			}
		}
		else
		{
			nf_AIPlayer *aiplayer = GetAIPlayerFromKey( key );
			Assert( aiplayer != NULL );
			if ( !aiplayer ) return FALSE;

			Assert( index > 0 );
			if ( index > 0 )
			{
				sint32 civ = index - 1;
				StringId sid = isFemale ?
					g_theCivilisationDB->Get(civ)->GetLeaderNameFemale():
					g_theCivilisationDB->Get(civ)->GetLeaderNameMale();
				MBCHAR leader[ dp_PNAMELEN + 1 ];
				strncpy( leader, g_theStringDB->GetNameStr( sid ), dp_PNAMELEN);


				leader[dp_PNAMELEN] = 0;
				aiplayer->SetName( leader );
			}

			aiplayer->SetTribe( index );
			UpdateAIPlayerSetup( aiplayer );
		}
	}

	UpdateTribeSwitches();
	UpdateGameSetup();

	return TRUE;
}


void AllinoneWindow::RequestTribe( sint32 index )
{

	Assert( index != 1 );
	if ( index == 1 ) return;

	if (!IsValidTribeIndex(index)) return;

	Assert( !g_netfunc->IsHost() );
	if ( g_netfunc->IsHost() ) return;

	NETFunc::Message message = NETFunc::Message(
		(NETFunc::Message::Code)CUSTOMCODE_REQUESTTRIBE,
		&index,
		sizeof( index ) );

	NETFunc::STATUS err = g_netfunc->Send(
		g_netfunc->GetDP(),
		&message,
		0 );
	Assert( err == NETFunc::OK );
}























































BOOL AllinoneWindow::LoadGUIDs(SaveInfo *info)
{
	int res;
	res = dpSetPlayerData(
		g_netfunc->GetDP(),
		g_netfunc->GetPlayer()->GetId(),
		k_GUIDDATA_ID,
		&info->networkGUID[0],
		sizeof(CivGuid) * k_MAX_PLAYERS,
		0);
	Assert(res == dp_RES_OK);
	if(res != dp_RES_OK) return FALSE;
	return TRUE;
}

BOOL AllinoneWindow::SetScenarioInfo(SaveInfo *info)
{
	int res;

	m_scenInfo.isScenario = (uint8)info->isScenario;
	m_scenInfo.m_haveSavedGame = TRUE;
	strcpy(m_scenInfo.m_fileName, info->fileName);
	strcpy(m_scenInfo.m_gameName, g_scenarioName);
	if(info->isScenario && info->scenarioName) {
		strcpy(m_scenInfo.m_scenarioName, info->scenarioName);
		ScenarioPack *pack;
		Scenario *scen;
		if(!g_civScenarios->FindScenario(m_scenInfo.m_scenarioName,
										 &pack, &scen)) {
			return FALSE;
		}
	} else {
		m_scenInfo.m_scenarioName[0] = 0;
	}

	sint32 i;
	for(i = 0; i < k_MAX_START_POINTS; i++) {
		m_scenInfo.m_civs[i] = (uint8)info->positions[i].civIndex;
	}
	m_scenInfo.m_numStartPositions = (uint8)info->numPositions;
	m_scenInfo.m_startInfoType = (uint8)info->startInfoType;

	for(i = 0; i < k_MAX_PLAYERS; i++) {
		if(info->playerCivIndexList[i] != 0) {
			m_scenInfo.m_legalCivs[i] = info->playerCivIndexList[i];
		} else {

			m_scenInfo.m_legalCivs[i] = 0;
		}
	}

	res = dpSetPlayerData(
		g_netfunc->GetDP(),
		g_netfunc->GetPlayer()->GetId(),
		k_SCENARIO_INFO_ID,
		&m_scenInfo,
		sizeof(m_scenInfo),
		0);

	if(m_scenInfo.isScenario) {
		m_controls[ CONTROL_ADDAIBUTTON ]->Enable( true );

		if((m_scenInfo.m_startInfoType == (uint8)STARTINFOTYPE_CIVS) ||
		   (m_scenInfo.m_startInfoType == (uint8)STARTINFOTYPE_CIVSFIXED)) {


			if(m_scenInfo.m_numStartPositions < k_NS_MAX_HUMANS) {

				g_gamesetup.SetSize(m_scenInfo.m_numStartPositions);
				UpdateGameSetup();
			}
		}
	}
	Assert(res == dp_RES_OK);
	if(res != dp_RES_OK) return FALSE;
	return TRUE;
}

void AllinoneWindow::SetupNewScenario()
{
	m_scenInfo.isScenario = TRUE;
	strcpy(m_scenInfo.m_gameName, g_scenarioName);
	m_scenInfo.m_fileName[0] = 0;
	m_scenInfo.m_startInfoType = STARTINFOTYPE_NONE;
	m_scenInfo.m_haveSavedGame = FALSE;
	m_scenInfo.m_scenarioName[0] = 0;

	dp_result_t res = dpSetPlayerData(
		g_netfunc->GetDP(),
		g_netfunc->GetPlayer()->GetId(),
		k_SCENARIO_INFO_ID,
		&m_scenInfo,
		sizeof(m_scenInfo),
		0);
    Assert(dp_RES_OK == res);

	if(m_scenInfo.isScenario) {
		m_controls[ CONTROL_ADDAIBUTTON ]->Enable( true );

		if((m_scenInfo.m_startInfoType == (uint8)STARTINFOTYPE_CIVS) ||
		   (m_scenInfo.m_startInfoType == (uint8)STARTINFOTYPE_CIVSFIXED)) {


			if(m_scenInfo.m_numStartPositions < k_NS_MAX_HUMANS) {

				g_gamesetup.SetSize(m_scenInfo.m_numStartPositions);
				UpdateGameSetup();
			}
		}
	}
}

































AUI_ERRCODE AllinoneWindow::Idle( void )
{

	if ( m_mode == CREATE ||
		 m_mode == CONTINUE_CREATE )
	{
	bool n = true;
	NETFunc::KeyStruct gameKey;
	bool launchGame = false;
	bool joinedgame = false;

	while (n)
    {
		if (NETFunc::Message * m = g_netfunc->GetMessage())
        {
			g_netfunc->HandleMessage(m);

			switch (m->GetCode())
			{
			case NETFunc::Message::UNLAUNCH:
				EnableButtonsForUnlaunch();
				g_playersetup.SetReadyToLaunch(false);
				break;
			case NETFunc::Message::NETWORKERR:
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONERR );
				g_netshell->GotoScreen( NetShell::SCREEN_CONNECTIONSELECT );
				break;
			case dp_SESSIONLOST_PACKET_ID:
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONLOST );
				g_netfunc->Leave();

				if ( s_dbw )
				{
					DialogBoxWindow::PopDown( s_dbw );
					s_dbw = NULL;
				}
				g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
				((LobbyWindow *)g_netshell->FindWindow( NetShell::WINDOW_LOBBY ))->Update();
				break;

			default:
				break;
			}

			if ( m->GetCode() == NETFunc::Message::ADDAIPLAYER )
			{
				nf_AIPlayer aiplayer;
				aiplayer.Set( m->GetBodySize(), m->GetBody() );

				BOOL isFemale = FALSE;
				sint32 tribeToGet = FindTribe( 0, TRUE, &isFemale );

				BOOL success = AssignTribe(
					tribeToGet,
					*(uint16 *)aiplayer.GetKey()->buf,
					TRUE,
					isFemale,
					FALSE );
				Assert( success );
			}
			else if ( m->GetCode() == NETFunc::Message::DELAIPLAYER )
			{

				nf_AIPlayer aiplayer;
				aiplayer.Set( m->GetBodySize(), m->GetBody() );

				BOOL success = AssignTribe(
					0,
					*(uint16 *)aiplayer.GetKey()->buf,
					TRUE,
					FALSE,
					TRUE );
				Assert( success );
			}
			else if ( m->GetCode() == (NETFunc::Message::Code)CUSTOMCODE_REQUESTTRIBE )
			{
				ns_HPlayerItem *hplayer = GetHPlayerFromId( m->GetSender() );
				Assert( hplayer != NULL );
				if ( hplayer )
				{
					uint16 key = *(uint16 *)hplayer->GetPlayer()->GetKey()->buf;

					sint32 index = *(sint32 *)m->GetBody();

					if ( index == 0 )
					{

						AssignTribe(
							0,
							key,
							FALSE,
							FALSE,
							TRUE );

						index = FindTribe( 0, FALSE );
					}

					uint16 curKey;
					BOOL curIsAI;
					BOOL curIsFemale;
					BOOL success = WhoHasTribe( index, &curKey, &curIsAI, &curIsFemale );
					Assert( success );

					if ( curKey == 0 )
					{


						BOOL success = AssignTribe(
							index,
							key,
							FALSE,
							FALSE,
							FALSE );
						Assert( success );
					}
					else if ( curKey != key )
					{

						sint32 curTribe = FindTribe( key, FALSE );

						BOOL success = AssignTribe(
							curTribe,
							key,
							FALSE,
							FALSE,
							FALSE );
						Assert( success );

						NETFunc::Message msg = NETFunc::Message(
							(NETFunc::Message::Code)CUSTOMCODE_REQUESTDENIED );

						NETFunc::STATUS err = g_netfunc->Send(
							g_netfunc->GetDP(),
							&msg,
							m->GetSender() );
						Assert( err == NETFunc::OK );
					}
					else
					{

						Assert( curIsAI == FALSE );
					}
				}
			}
			else if(m->GetCode() == NETFunc::Message::ENTERGAME)
			{

				memcpy(
					&gameKey,
					(NETFunc::KeyStruct *)(m->GetBody()),
					sizeof(NETFunc::KeyStruct));
				m_joinedGame = joinedgame = true;

				m_scenInfo.isScenario = FALSE;
			}
			else if ( m->GetCode() == NETFunc::Message::PLAYERENTER )
			{






				m_shouldAddAIPlayer = 0;
			}
			else if(m->GetCode() == NETFunc::Message::GAMELAUNCH)
				launchGame = true;
			 else if(m->GetCode() == NETFunc::Message::PLAYERPACKET)
			 {
				 ns_HPlayerListBox *l = (ns_HPlayerListBox *)
					 FindControl( AllinoneWindow::CONTROL_HPLAYERSLISTBOX );
				 ns_HPlayerItem *item = (ns_HPlayerItem *)(l->GetSelectedItem());
				 if(item && !item->IsAI())
				 {
					 NETFunc::Player *player = item->GetPlayer();
					 PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->
						 FindWindow( NetShell::WINDOW_PLAYEREDIT );
					 *(NETFunc::PlayerSetup *)&g_rplayersetup =
						 NETFunc::PlayerSetup(player);
					 g_rplayersetup.Packet::Set(m->GetBodySize(), m->GetBody());
					 p->SetPlayerSetup(&g_rplayersetup);
					 p->SetMode(p->VIEW);
					 g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
				 }
			}
			delete m;
		} else
			n = false;
	}

	if(joinedgame && g_netfunc->GetStatus() == NETFunc::OK) {
		PlayerSelectWindow *w = (PlayerSelectWindow *)g_netshell->
			FindWindow(NetShell::WINDOW_PLAYERSELECT);
		g_playersetup = *(w->GetPlayerSetup(g_netfunc->GetPlayer()));
		g_netfunc->SetPlayerSetup(&g_playersetup);

		g_netfunc->PushChatMessage(m_messageGameCreate->GetString());

		NETFunc::EnumPlayers(false, &gameKey);

		((ns_GPlayerListBox *)FindControl( CONTROL_GPLAYERSLISTBOX ))->
			SetKey(&gameKey);

		((ns_ChatBox *)FindControl( CONTROL_CHATBOX ))->SetKey(&gameKey);

		NETFunc::EnumPlayers(true, &gameKey);

		((aui_TextField *)(FindControl(CONTROL_GAMENAMETEXTFIELD)))->
			SetFieldText(g_netfunc->GetSession()->GetName());

        delete g_nsUnits;
        g_nsUnits = new ns_Units();
        delete g_nsImprovements;
        g_nsImprovements = new ns_Improvements();
        delete g_nsWonders;
        g_nsWonders = new ns_Wonders();

		CreateExclusions();







		g_playersetup.SetKey( g_netfunc->GetPlayer()->GetKey() );

		BOOL success = FALSE;
		if ( m_mode == CONTINUE_CREATE ||
			 (IsScenarioGame()))
		{
			SaveInfo *info = NULL;
			BOOL deleteIt = FALSE;
			if(IsScenarioGame()) {
				ScenarioPack *pack;
				Scenario *scen;
				if(g_civScenarios->FindScenario(g_scenarioName,
												&pack, &scen)) {
					info = g_civScenarios->LoadSaveInfo(scen);
					if(info)
						deleteIt = TRUE;
				}
				if(!info)
					SetupNewScenario();
			} else {
				info = g_loadsaveWindow->GetSaveInfo();
			}

			if ( info )
			{
				LoadGUIDs(info);


				m_receivedGuids = true;
				memcpy(
					&m_civGuids,
					info->networkGUID,
					sizeof(CivGuid) * k_MAX_PLAYERS);

				sint32 i;
				for(i = 0; i < k_MAX_PLAYERS; i++) {
					if(m_civGuids[i].guid == *g_network.GetGuid()) {
						success = AssignTribe(
							m_civGuids[i].civIndex + 1,
							*(uint16 *)g_playersetup.GetKey()->buf,
							FALSE,
							FALSE,
							FALSE );
						Assert( success );
						break;
					}
				}

				for( i = 0; i < k_NS_MAX_PLAYERS; i++ )
				{
					if ( g_gamesetup.GetSavedTribeSlots()[ i ].isAI )
						AddAIPlayer();
				}

				BOOL haveScenario = SetScenarioInfo(info);
				if(deleteIt)
					delete info;
				if(!haveScenario) {
					if(g_netfunc->GetTransport()) {

						if ( s_dbw ) {
							DialogBoxWindow::PopDown( s_dbw );
							s_dbw = NULL;
						}

						passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_SCENARIO_NOT_FOUND );

						aui_Button *cancel = (aui_Button *)
							m_controls[ CONTROL_CANCELBUTTON ];
						cancel->GetAction()->Execute(
													 cancel,
													 AUI_BUTTON_ACTION_EXECUTE,
													 0 );
					} else {
#ifdef WIN32
						PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );
#endif
				    }
				}
			}
		}

		if ( !success )
		{
			success = AssignTribe(
				FindTribe( 0, FALSE ),
				*(uint16 *)g_playersetup.GetKey()->buf,
				FALSE,
				FALSE,
				FALSE );
			Assert( success );
		}





		UpdateDisplay();

		UpdatePlayerButtons();


		if ( s_dbw )
		{
			DialogBoxWindow::PopDown( s_dbw );
			s_dbw = NULL;
		}
	}

	if(launchGame) {
		g_netfunc->PushChatMessage(m_messageLaunched->GetString());
		*(NETFunc::Session *)&g_gamesetup = *(NETFunc::Session *)&g_netfunc->gameSetup;
		AllinoneWindow_SetupGameForLaunch();

		for ( sint32 child = g_ui->ChildList()->L(); child; child-- )
			g_ui->RemoveChild( g_ui->ChildList()->GetHead()->Id() );
		g_netshell->Leave(k_NS_FLAGS_LAUNCH | k_NS_FLAGS_DESTROYNETSHELL, TRUE);
	}

	if ( m_shouldUpdateGame )
	{
		uint32 tickNow = GetTickCount();
		if ( tickNow - m_tickGame > k_PACKET_DELAY )
		{
			ReallyUpdateGameSetup();

			m_tickGame = tickNow;
		}
	}
	if ( m_shouldAddAIPlayer )
	{

		if ( !OKToAddPlayers() )
			m_shouldAddAIPlayer = 0;
		else
			AddAIPlayer( m_shouldAddAIPlayer );
	}
	if ( m_shouldUpdateAIPlayer )
	{
		uint32 tickNow = GetTickCount();
		if ( tickNow - m_tickAIPlayer > k_PACKET_DELAY )
		{
			ReallyUpdateAIPlayerSetup();

			m_tickAIPlayer = tickNow;
		}
	}
	if ( m_joinedGame )
	{

		uint32 tickNow = GetTickCount();
		if ( tickNow - m_tickPlayer > k_PACKET_DELAY )
		{




			if ( (!g_gamesetup.GetDynamicJoin() && CurNumHumanPlayers() < 2) ||
				 (g_gamesetup.GetDynamicJoin() && ((ns_HPlayerListBox *)
			m_controls[ CONTROL_HPLAYERSLISTBOX ])->NumItems() < 2 )
				 || g_playersetup.IsReadyToLaunch() )
			{
				if ( !m_controls[ CONTROL_OKBUTTON ]->IsDisabled() )
					m_controls[ CONTROL_OKBUTTON ]->Enable( false );
			}
			else
			{
				if ( m_controls[ CONTROL_OKBUTTON ]->IsDisabled() )
					m_controls[ CONTROL_OKBUTTON ]->Enable( true );
			}


			MBCHAR temp[ dp_SNAMELEN + 1 ];
			((aui_TextField *)m_controls[ CONTROL_GAMENAMETEXTFIELD ])->
				GetFieldText( temp, dp_SNAMELEN );
			if ( strncmp( g_gamesetup.GetName(), temp, dp_SNAMELEN ) )
			{
				g_gamesetup.SetName( temp );
				UpdateGameSetup();
			}

			m_tickPlayer = tickNow;
		}
	}
	}

	else if ( m_mode == JOIN ||
			  m_mode == CONTINUE_JOIN )
	{
	bool n = true;
	NETFunc::KeyStruct gameKey;
	bool becameHost = false;
	bool launchGame = false;
	bool playerKick = false;
	bool joinedgame = false;
	bool dontHaveScenario = false;
	static bool gotpacket = false;

	while (n)
    {
		if (NETFunc::Message * m = g_netfunc->GetMessage())
        {
			g_netfunc->HandleMessage(m);
			g_gamesetup.Handle(m);

			switch ( m->GetCode() )
			{
			case NETFunc::Message::NETWORKERR:
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONERR );
				g_netshell->GotoScreen( NetShell::SCREEN_CONNECTIONSELECT );
				break;
			case dp_SESSIONLOST_PACKET_ID:
				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_CONNECTIONLOST );
				g_netfunc->Leave();

				if ( s_dbw )
				{
					DialogBoxWindow::PopDown( s_dbw );
					s_dbw = NULL;
				}
				g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
				((LobbyWindow *)g_netshell->FindWindow( NetShell::WINDOW_LOBBY ))->Update();
				break;
			case NETFunc::Message::UNLAUNCH:
				if(g_playersetup.IsReadyToLaunch())
					g_netfunc->PushChatMessage(m_messageGameSetup->GetString());
				EnableButtonsForUnlaunch();
				g_playersetup.SetReadyToLaunch(false);
				break;
			default:
				break;
			}

			if ( m->GetCode() ==
				 (NETFunc::Message::Code)CUSTOMCODE_REQUESTDENIED )
			{
				g_netfunc->PushChatMessage(m_messageRequestDenied->GetString());

				if ( g_playersetup.GetTribe() == 0 )
					RequestTribe( 0 );
			}
			else if ( m->GetCode() == dp_USER_PLAYERDATA_PACKET_ID )
			{
				dp_user_playerData_packet_t playerData =
					*(dp_user_playerData_packet_t *)m->GetBody();































				if(playerData.key == k_GUIDDATA_ID) {
					m_receivedGuids = true;
					memcpy(
						&m_civGuids,
						playerData.data,
						sizeof(CivGuid) * k_MAX_PLAYERS);

					if ( m_mode == CONTINUE_JOIN && m_joinedGame && gotpacket )
					{

						sint32 i;
						for(i = 0; i < k_MAX_PLAYERS; i++) {
							if(m_civGuids[i].guid == *g_network.GetGuid()) {
								RequestTribe( m_civGuids[ i ].civIndex + 1 );
								break;
							}
						}

						if ( i == k_MAX_PLAYERS )
							RequestTribe( 0 );
					}
				} else if(playerData.key == k_SCENARIO_INFO_ID) {
					memcpy(&m_scenInfo, playerData.data, sizeof(ns_ScenarioInfo));

					SetScenarioGame(m_scenInfo.isScenario);

					if(m_scenInfo.isScenario) {







						ScenarioPack *pack;
						Scenario *scen;
						strcpy(g_scenarioName, m_scenInfo.m_gameName);
						if(g_civScenarios->FindScenario(g_scenarioName,
														&pack, &scen)) {

							g_civPaths->SetCurScenarioPath(scen->m_path);
							g_civPaths->SetCurScenarioPackPath(pack->m_path);
						} else {
							dontHaveScenario = true;
						}

						ns_GPlayerListBox *listbox = (ns_GPlayerListBox *)
							m_controls[ CONTROL_GPLAYERSLISTBOX ];
						listbox->EnableTribeButton(g_netfunc->GetPlayer());




						UpdateTribeSwitches();

					} else if(m_scenInfo.m_scenarioName[0] != 0) {
						strcpy(g_scenarioName, m_scenInfo.m_scenarioName);
						ScenarioPack *pack;
						Scenario *scen;
						if(g_civScenarios->FindScenario(g_scenarioName,
														&pack, &scen)) {
							g_civPaths->SetCurScenarioPath(scen->m_path);
							g_civPaths->SetCurScenarioPackPath(pack->m_path);
						} else {
							dontHaveScenario = true;
						}
					} else {
						g_scenarioName[0] = 0;
					}




					dpSetPlayerData(
									g_netfunc->GetDP(),
									g_netfunc->GetPlayer()->GetId(),
									k_SCENARIO_INFO_ID,
									&m_scenInfo,
									sizeof(m_scenInfo),
									0);
				}
			}
			else if(m->GetCode() == NETFunc::Message::ENTERGAME)
			{

				memcpy(
					&gameKey,
					(NETFunc::KeyStruct *)(m->GetBody()),
					sizeof(NETFunc::KeyStruct));
				m_joinedGame = joinedgame = true;

				gotpacket = false;
			}
			else if ( m->GetCode() == NETFunc::Message::ENTERLOBBY )
			{

				if ( s_dbw )
				{
					DialogBoxWindow::PopDown( s_dbw );
					s_dbw = NULL;
				}

				passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_FULL );

				aui_Button *cancel = (aui_Button *)
					m_controls[ CONTROL_CANCELBUTTON ];
				cancel->GetAction()->Execute(
					cancel,
					AUI_BUTTON_ACTION_EXECUTE,
					0 );
			}
			else if(m->GetCode() == NETFunc::Message::GAMEHOST) {
				becameHost = true;
			} else if(m->GetCode() == NETFunc::Message::GAMELAUNCH) {
				launchGame = true;
			} else if(m->GetCode() == NETFunc::Message::PLAYERKICK) {
				playerKick = true;
				n = false;
			} else if(m->GetCode() == NETFunc::Message::GAMESESSION ) {
				((aui_TextField *)(FindControl(CONTROL_GAMENAMETEXTFIELD)))->
					SetFieldText(g_gamesetup.GetName());
			} else if(m->GetCode() == NETFunc::Message::GAMEPACKET) {
				UpdateDisplay();
				if ( !gotpacket )
				{
					gotpacket = true;
				}
			} else if(m->GetCode() == NETFunc::Message::PLAYERPACKET)
			{
				ns_HPlayerListBox *l = (ns_HPlayerListBox *)
					FindControl( AllinoneWindow::CONTROL_HPLAYERSLISTBOX );
				ns_HPlayerItem *item = (ns_HPlayerItem *)(l->GetSelectedItem());
				if(item && !item->IsAI())
				{
					NETFunc::Player *player = item->GetPlayer();
					PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->
						FindWindow( NetShell::WINDOW_PLAYEREDIT );
					*(NETFunc::PlayerSetup *)&g_rplayersetup =
						NETFunc::PlayerSetup(player);
					g_rplayersetup.Packet::Set(m->GetBodySize(), m->GetBody());
					p->SetPlayerSetup(&g_rplayersetup);
					p->SetMode(p->VIEW);
					g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
				}
			}
			delete m;
		} else
			n = false;
		if(becameHost)
			break;
	}

	if(playerKick) {
		if(g_netfunc->GetTransport()) {

			g_netfunc->PushChatMessage(m_messageKicked->GetString());
			LobbyWindow *w = (LobbyWindow *)g_netshell->
				FindWindow( NetShell::WINDOW_LOBBY );
			g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
			w->Update();
		} else {
#ifdef WIN32
			PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );
#endif
		}

	}

	if(dontHaveScenario) {
		if(g_netfunc->GetTransport()) {

			if ( s_dbw ) {
				DialogBoxWindow::PopDown( s_dbw );
				s_dbw = NULL;
			}

			passwordscreen_displayMyWindow( PASSWORDSCREEN_MODE_SCENARIO_NOT_FOUND );

			aui_Button *cancel = (aui_Button *)
				m_controls[ CONTROL_CANCELBUTTON ];
			cancel->GetAction()->Execute(
				cancel,
				AUI_BUTTON_ACTION_EXECUTE,
				0 );
		} else {
#ifdef WIN32
			PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );
#endif
	}
	}

	if(joinedgame && g_netfunc->GetStatus() == NETFunc::OK) {

		PlayerSelectWindow *w = (PlayerSelectWindow *)g_netshell->
			FindWindow(NetShell::WINDOW_PLAYERSELECT);
		g_playersetup = *(w->GetPlayerSetup(g_netfunc->GetPlayer()));
		g_netfunc->SetPlayerSetup(&g_playersetup);

		g_netfunc->PushChatMessage(m_messageGameEnter->GetString());

		NETFunc::EnumPlayers(false, &gameKey);

		((ns_GPlayerListBox *)FindControl( CONTROL_GPLAYERSLISTBOX ))->
			SetKey(&gameKey);

		((ns_ChatBox *)FindControl( CONTROL_CHATBOX ))->SetKey(&gameKey);

		NETFunc::EnumPlayers(true, &gameKey);

		((aui_TextField *)(FindControl(CONTROL_GAMENAMETEXTFIELD)))->
			SetFieldText(g_netfunc->GetSession()->GetName());
	}

	static bool didntdoyet = true;
	if ( !m_joinedGame ) didntdoyet = true;

	if ( didntdoyet && m_joinedGame && gotpacket )
	{

		didntdoyet = false;

        delete g_nsUnits;
        g_nsUnits = new ns_Units();
        delete g_nsImprovements;
        g_nsImprovements = new ns_Improvements();
        delete g_nsWonders;
        g_nsWonders = new ns_Wonders();

		CreateExclusions();




		g_playersetup.SetKey( g_netfunc->GetPlayer()->GetKey() );
		if ( m_mode == CONTINUE_JOIN )
		{

			if ( m_receivedGuids )
			{
				sint32 i;

				for (i = 0; i < k_MAX_PLAYERS; i++) {
					if(m_civGuids[i].guid == *g_network.GetGuid()) {
						RequestTribe( m_civGuids[ i ].civIndex + 1 );
						break;
					}
				}

				if ( i == k_MAX_PLAYERS )
					RequestTribe( 0 );
			}
		}
		else
		{
			RequestTribe( 0 );
		}







		UpdatePlayerButtons();


		if ( s_dbw )
		{
			DialogBoxWindow::PopDown( s_dbw );
			s_dbw = NULL;
		}
	}

	if(becameHost) {





		nf_GameSetup *newGS = new nf_GameSetup( g_gamesetup );
		if ( newGS )
		{
			ns_GameSetupListBox *gslb = (ns_GameSetupListBox *)
				((ns_Window *)g_netshell->
				 FindWindow( NetShell::WINDOW_GAMESELECT ))->
				FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX );
			gslb->InsertItem( newGS );
			gslb->SelectItem( gslb->FindItem( newGS ) );

			g_gamesetup = *gslb->FindItem( newGS )->
				GetNetShellObject()->GetNETFuncObject();

			nf_GameSetup temp = g_gamesetup;
			memset(
				temp.GetTribeSlots(),
				0,
				k_NS_MAX_PLAYERS * sizeof( TribeSlot ) );

			gslb->ChangeItem( &temp );
		}

		g_netfunc->PushChatMessage(m_messageGameHost->GetString());





		NETFunc::EnumPlayers(false, g_netfunc->GetSession()->GetKey());

		((ns_GPlayerListBox *)FindControl( CONTROL_GPLAYERSLISTBOX ))->
			SetKey(g_netfunc->GetSession()->GetKey());

		((ns_ChatBox *)FindControl( CONTROL_CHATBOX ))->
			SetKey(g_netfunc->GetSession()->GetKey());

		NETFunc::EnumPlayers(true, g_netfunc->GetSession()->GetKey());
		switch ( m_mode )
		{
		case JOIN: SetMode( CREATE ); break;
		case CONTINUE_JOIN: SetMode( CONTINUE_CREATE ); break;
		default: Assert( FALSE ); break;
		}

		if ( !FindTribe( *(uint16 *)g_playersetup.GetKey()->buf, FALSE ) )
		{
			BOOL success = AssignTribe(
				FindTribe( 0, FALSE ),
				*(uint16 *)g_playersetup.GetKey()->buf,
				FALSE,
				FALSE,
				FALSE );
			Assert( success );
		}

		UpdateDisplay();
	}

	if(!becameHost && launchGame) {
		g_netfunc->PushChatMessage(m_messageLaunched->GetString());
		*(NETFunc::Session *)&g_gamesetup = *(NETFunc::Session *)&g_netfunc->gameSetup;
		AllinoneWindow_SetupGameForLaunch();

		for ( sint32 child = g_ui->ChildList()->L(); child; child-- )
			g_ui->RemoveChild( g_ui->ChildList()->GetHead()->Id() );
		g_netshell->Leave(k_NS_FLAGS_LAUNCH | k_NS_FLAGS_DESTROYNETSHELL, TRUE);
	}
	}

	return AUI_ERRCODE_OK;
}


void AllinoneWindow::UpdateDisplay( void )
{
	((aui_TextField *)FindControl(CONTROL_GAMENAMETEXTFIELD))->
		SetFieldText(g_gamesetup.GetName());

	((aui_DropDown *)m_controls[ CONTROL_PLAYSTYLEDROPDOWN ])->
		SetSelectedItem( g_gamesetup.GetPlayStyle() );
	((ctp2_Spinner *)m_controls[ CONTROL_PLAYSTYLEVALUESPINNER ])->
		SetValue( g_gamesetup.GetPlayStyleValue(), 10 );

	((aui_Switch *)m_controls[ CONTROL_DYNAMICJOINSWITCH ])->SetState(
		g_gamesetup.GetDynamicJoin() );




	((aui_Switch *)m_controls[ CONTROL_HANDICAPPINGSWITCH ])->SetState(
		g_gamesetup.GetHandicapping() );

	((aui_Switch *)m_controls[ CONTROL_BLOODLUSTSWITCH ])->SetState(
		g_gamesetup.GetBloodlust() );






	((aui_Switch *)m_controls[ CONTROL_POLLUTIONSWITCH ])->SetState(
		g_gamesetup.GetPollution() );
	((c3_EditButton *)m_controls[ CONTROL_CIVPOINTSBUTTON ])->SetValue(
		g_gamesetup.GetCivPoints() );
	((c3_EditButton *)m_controls[ CONTROL_PWPOINTSBUTTON ])->SetValue(
		g_gamesetup.GetPwPoints() );

	((aui_Switch *)m_controls[ CONTROL_LOCKSWITCH ])->SetState(
		g_gamesetup.IsClosed() );






	agesscreen_setStartAge( g_gamesetup.GetStartAge() );
	agesscreen_setEndAge( g_gamesetup.GetEndAge() );
	spnewgamemapsizescreen_setMapSizeIndex( g_gamesetup.GetMapSize() );
	spnewgamemapshapescreen_setMapShapeIndex( g_gamesetup.GetWorldShape() );

	UpdateTribeSwitches();

	custommapscreen_setValues(
		g_gamesetup.GetWorldType1(),
		g_gamesetup.GetWorldType2(),
		g_gamesetup.GetWorldType3(),
		g_gamesetup.GetWorldType4(),
		g_gamesetup.GetWorldType5(),
		g_gamesetup.GetWorldType6() );
	spnewgamediffscreen_setDifficulty1( g_gamesetup.GetDifficulty1() );
	spnewgamediffscreen_setDifficulty2( g_gamesetup.GetDifficulty2() );

	if ( !m_createdExclusions ) CreateExclusions();

	m_numAvailUnits = g_gamesetup.GetNumAvailUnits();
	sint32 i;
	for ( i = 0; i < m_numAvailUnits; i++ )
		if ( m_units[ i ] ) m_units[ i ]->SetState( g_gamesetup.GetUnit( i ) );

	m_numAvailImprovements = g_gamesetup.GetNumAvailImprovements();
	for ( i = 0; i < m_numAvailImprovements; i++ )
		m_improvements[ i ]->SetState( g_gamesetup.GetImprovement( i ) );

	m_numAvailWonders = g_gamesetup.GetNumAvailWonders();
	for ( i = 0; i < m_numAvailWonders; i++ )
		m_wonders[ i ]->SetState( g_gamesetup.GetWonder( i ) );

	if ( !g_gamesetup.GetHandicapping() )
	{
		ns_HPlayerListBox *hplayerslistbox = (ns_HPlayerListBox *)
			m_controls[ CONTROL_HPLAYERSLISTBOX ];

		for ( sint32 i = 0; i < hplayerslistbox->NumItems(); i++ )
		{
			ns_HPlayerItem *item = (ns_HPlayerItem *)hplayerslistbox->
				GetItemByIndex( i );
			if ( item->IsAI() )
			{
				if ( g_netfunc->IsHost() )
				{
					item->GetCivpointsButton()->Enable( false );
					item->GetPwpointsButton()->Enable( false );
				}
			}
			else
			{
				if ( IsMine( item->GetPlayer() ) )
				{
					item->GetCivpointsButton()->Enable( false );
					item->GetPwpointsButton()->Enable( false );
				}
			}
		}
	}

	if ( !g_playersetup.IsReadyToLaunch() )
		m_controls[ CONTROL_OKBUTTON ]->Enable( TRUE );




}


void AllinoneWindow::UpdateTribeSwitches( void )
{
	TribeSlot *tribeSlots = g_gamesetup.GetTribeSlots();

	if ( !g_netfunc->IsHost() )
	{
		for ( sint32 i = 0; i < k_NS_MAX_PLAYERS; i++ )
		{
			if ( !tribeSlots[ i ].isAI )
			if ( tribeSlots[ i ].key == *(uint16 *)g_playersetup.GetKey()->buf )
			{
				if ( g_playersetup.GetTribe() != tribeSlots[ i ].tribe )
				{
					g_playersetup.SetTribe( tribeSlots[ i ].tribe );
					UpdatePlayerSetup();
				}

				break;
			}
		}
	}

	spnewgametribescreen_clearTribes();
	if(IsScenarioGame())
	{
		if(m_scenInfo.m_haveSavedGame)
		{
			for(sint32 i = 0; i < k_MAX_PLAYERS; i++)
			{
				if(m_scenInfo.m_legalCivs[i] != 0)
				{
					spnewgametribescreen_addTribeNoDuplicate(m_scenInfo.m_legalCivs[i]);
				}
			}
			return;
		}
	}

	spnewgametribescreen_addAllTribes();
	sint32 i;
	for ( i = 0; i < k_NS_MAX_PLAYERS; i++ )
	{
		if ( tribeSlots[ i ].tribe != g_playersetup.GetTribe() )
			spnewgametribescreen_removeTribe( tribeSlots[ i ].tribe - 1 );
	}


	if ( m_mode == CONTINUE_CREATE || m_mode == CONTINUE_JOIN)
	{
		if(m_scenInfo.isScenario &&

		   ((m_scenInfo.m_startInfoType == uint8(STARTINFOTYPE_NOLOCS)) ||
			(m_scenInfo.m_startInfoType == uint8(STARTINFOTYPE_POSITIONSFIXED))))

			return;

		for ( i = 2; i < g_nsTribes->GetNumTribes(); i++ )
		{
			bool found = false;
			sint32 j;
			if(m_scenInfo.isScenario)
			{
				for(j = 0; j < m_scenInfo.m_numStartPositions; j++)
				{
					if(i - 1 == m_scenInfo.m_civs[j])
					{
						found = true;
						break;
					}
				}
			} else {
				for ( j = 0; j < k_NS_MAX_PLAYERS; j++ )
				{
					if ( i == g_gamesetup.GetSavedTribeSlots()[ j ].tribe )
					{
						found = true;
						break;
					}
				}
			}

			if ( !found )
				spnewgametribescreen_removeTribe( i - 1 );
		}
	}
}


void AllinoneWindow::UpdateConfig( void )
{


	ns_HPlayerListBox *hplayerslistbox = (ns_HPlayerListBox *)
		m_controls[ CONTROL_HPLAYERSLISTBOX ];

	aui_Switch *hand = (aui_Switch *)m_controls[ CONTROL_HANDICAPPINGSWITCH ];
	aui_Control *cpspinner = m_controls[ CONTROL_CIVPOINTSBUTTON ];
	aui_Control *pwspinner = m_controls[ CONTROL_PWPOINTSBUTTON ];
	aui_Static *sheet = (aui_Static *)m_controls[ CONTROL_RULESSHEET ];

#ifdef LOCKSETTINGSONLAUNCH
	if ( hand->IsOn() && !g_playersetup.IsReadyToLaunch() )
#else
	if ( hand->IsOn() )
#endif
	{
		sheet->RemoveChild( m_controls[ CONTROL_GOLDSTATICTEXT ]->Id() );
		sheet->RemoveChild( cpspinner->Id() );
		sheet->RemoveChild( m_controls[ CONTROL_PWSTATICTEXT ]->Id() );
		sheet->RemoveChild( pwspinner->Id() );

		for ( sint32 i = 0; i < hplayerslistbox->NumItems(); i++ )
		{
			ns_HPlayerItem *item = (ns_HPlayerItem *)hplayerslistbox->
				GetItemByIndex( i );
			if ( item->IsAI() )
			{
				if ( g_netfunc->IsHost() )
				{
					item->GetCivpointsButton()->Enable( true );
					item->GetPwpointsButton()->Enable( true );
				}
			}
			else
			{
				if ( IsMine( item->GetPlayer() ) )
				{
					item->GetCivpointsButton()->Enable( true );
					item->GetPwpointsButton()->Enable( true );
				}
			}
		}
	}
	else
	{
		sheet->AddChild( m_controls[ CONTROL_GOLDSTATICTEXT ] );
		sheet->AddChild( cpspinner );
		sheet->AddChild( m_controls[ CONTROL_PWSTATICTEXT ] );
		sheet->AddChild( pwspinner );

		if ( !sheet->IsHidden() )
		{
			m_controls[ CONTROL_GOLDSTATICTEXT ]->Show();
			cpspinner->Show();
			m_controls[ CONTROL_PWSTATICTEXT ]->Show();
			pwspinner->Show();
		}

		for ( sint32 i = 0; i < hplayerslistbox->NumItems(); i++ )
		{
			ns_HPlayerItem *item = (ns_HPlayerItem *)hplayerslistbox->
				GetItemByIndex( i );
			if ( item->IsAI() )
			{
				if ( g_netfunc->IsHost() )
				{
					item->GetCivpointsButton()->Enable( false );
					item->GetPwpointsButton()->Enable( false );
				}
			}
			else
			{
				if ( IsMine( item->GetPlayer() ) )
				{
					item->GetCivpointsButton()->Enable( false );
					item->GetPwpointsButton()->Enable( false );
				}
			}
		}
	}

	aui_DropDown *playstyle = (aui_DropDown *)
		m_controls[ CONTROL_PLAYSTYLEDROPDOWN ];
	aui_Control *psspinner = m_controls[ CONTROL_PLAYSTYLEVALUESPINNER ];
	aui_Control *psstatic = m_controls[ CONTROL_PLAYSTYLEVALUESTATICTEXT ];

	sint32 index = playstyle->GetSelectedItem();
	Assert( index >= 0 && index < m_playStyleValueStrings->GetNumStrings());
	if ( index < 0 || index >= m_playStyleValueStrings->GetNumStrings() )
		return;

	const MBCHAR *string = m_playStyleValueStrings->GetString( index );


	if ( strlen( string ) )
	{
		m_controls[ CONTROL_PLAYERSSHEET ]->AddChild( psspinner );
		m_controls[ CONTROL_PLAYERSSHEET ]->AddChild( psstatic );
		psstatic->SetText( string );
	}
	else
	{
		m_controls[ CONTROL_PLAYERSSHEET ]->RemoveChild( psspinner->Id() );
		m_controls[ CONTROL_PLAYERSSHEET ]->RemoveChild( psstatic->Id() );
	}

	ShouldDraw();
	if ( g_rulesWindow ) g_rulesWindow->ShouldDraw();
}


void AllinoneWindow::UpdateGameSetup(bool b)
{

	if ( g_netfunc->IsHost() )
	{
		if(b)
			g_netfunc->UnLaunchAll();

		m_shouldUpdateGame = true;

		uint32 tickNow = GetTickCount();

		if ( tickNow - m_tickGame > k_PACKET_DELAY )
		{
			ReallyUpdateGameSetup();
		}

		m_tickGame = tickNow;
	}
}


void AllinoneWindow::ReallyUpdateGameSetup()
{

	g_gamesetup.Update();

	GameSelectWindow *gsw = (GameSelectWindow *)g_netshell->
		FindWindow( NetShell::WINDOW_GAMESELECT );
	ns_GameSetupListBox *gl = (ns_GameSetupListBox *)gsw->
		FindControl( GameSelectWindow::CONTROL_GAMENAMELISTBOX );

	nf_GameSetup temp = g_gamesetup;
	memset( temp.GetTribeSlots(), 0, k_NS_MAX_PLAYERS * sizeof( TribeSlot ) );
	memset( temp.GetSavedTribeSlots(), 0, k_NS_MAX_PLAYERS * sizeof( TribeSlot ) );

	gl->ChangeItem( &temp );

	m_shouldUpdateGame = false;
}


void AllinoneWindow::UpdatePlayerSetup(void)
{
	m_shouldUpdatePlayer = true;

	g_playersetup.Update();

	ns_PlayerSetupListBox *l = (ns_PlayerSetupListBox *)
		((ns_Window *)g_netshell->
		 FindWindow( NetShell::WINDOW_PLAYERSELECT ))->
		FindControl( PlayerSelectWindow::CONTROL_PLAYERNAMELISTBOX );

	if( l->FindItem( &g_playersetup ) )
		l->ChangeItem( &g_playersetup );

	m_shouldUpdatePlayer = false;
}


void AllinoneWindow::UpdateAIPlayerSetup( nf_AIPlayer *aiplayer )
{

	if ( g_netfunc->IsHost() )
	{

		if ( !m_aiplayerList->Find( aiplayer ) )
			m_aiplayerList->AddTail( aiplayer );

		m_shouldUpdateAIPlayer = true;

		uint32 tickNow = GetTickCount();
		if ( tickNow - m_tickAIPlayer > k_PACKET_DELAY )
		{
			ReallyUpdateAIPlayerSetup();
		}

		m_tickAIPlayer = tickNow;
	}
}


void AllinoneWindow::ReallyUpdateAIPlayerSetup( void )
{
	for ( sint32 i = m_aiplayerList->L(); i; i-- )
	{
		nf_AIPlayer *aiplayer = m_aiplayerList->RemoveHead();
		aiplayer->Pack();
		g_netfunc->ChangeAIPlayer( aiplayer );
	}

	m_shouldUpdateAIPlayer = false;
}


void AllinoneWindow::DeleteAIPlayer( nf_AIPlayer *player )
{
	ListPos pos = m_aiplayerList->Find( player );
	if ( pos )
	{
		m_aiplayerList->DeleteAt( pos );
		if ( !m_aiplayerList->L() ) m_shouldUpdateAIPlayer = false;
	}
}


sint32 AllinoneWindow::CurNumHumanPlayers( void )
{

	return g_netfunc->players.size();
}


sint32 AllinoneWindow::CurNumAiPlayers( void )
{
	ns_AIPlayerListBox *aiplistbox = (ns_AIPlayerListBox *)
		m_controls[ CONTROL_AIPLAYERSLISTBOX ];

	return aiplistbox->NumItems();
}


sint32 AllinoneWindow::CurNumPlayers( void )
{
	return CurNumHumanPlayers() + CurNumAiPlayers();
}


sint32 AllinoneWindow::OKToAddPlayers( void )
{
	sint32 curNumPlayers = CurNumHumanPlayers();


	if ( curNumPlayers == 0 ) return 1;





	sint32 numPlayers = curNumPlayers + CurNumAiPlayers();

	if ( numPlayers < 0 || numPlayers > k_NS_MAX_PLAYERS ) return 0;

	if ( numPlayers == k_NS_MAX_PLAYERS ) return 0;


	if(m_scenInfo.isScenario &&
		((m_scenInfo.m_startInfoType != (uint8)STARTINFOTYPE_NONE &&
		  m_scenInfo.m_startInfoType != (uint8)STARTINFOTYPE_NOLOCS))) {
		if(numPlayers >= m_scenInfo.m_numStartPositions)
			return 0;
	}

	ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)
		m_controls[ CONTROL_HPLAYERSLISTBOX ];
	if ( hplistbox->NumItems() >= k_NS_MAX_PLAYERS ) return 0;

	return numPlayers;
}


void AllinoneWindow::AddAIPlayer( sint32 curCount )
{


	if ( !curCount )
	{


		m_shouldAddAIPlayer++;
	}

	if ( !OKToAddPlayers() ) return;

	sint16 newMaxNumHumans = g_gamesetup.GetMaxPlayers();
	sint32 maxPlayersTotal = k_NS_MAX_PLAYERS;
	if(m_scenInfo.isScenario &&
	   (m_scenInfo.m_startInfoType != (uint8)STARTINFOTYPE_NONE &&
	    m_scenInfo.m_startInfoType != (uint8)STARTINFOTYPE_NOLOCS)) {
		if(m_scenInfo.m_numStartPositions < k_NS_MAX_PLAYERS)
			maxPlayersTotal = m_scenInfo.m_numStartPositions;
	}

	if ( maxPlayersTotal - CurNumAiPlayers() <= k_NS_MAX_HUMANS )
		--newMaxNumHumans;
	Assert( newMaxNumHumans > 0 );
	if ( !newMaxNumHumans ) newMaxNumHumans = 1;
	g_gamesetup.SetSize(newMaxNumHumans);
	UpdateGameSetup();

	static NETFunc::KeyStruct key;
	static bool firstTime = true;
	if ( firstTime )
	{
		firstTime = false;
		memset( &key, 0, sizeof( NETFunc::KeyStruct ) );
		key.len = 1;
	}

	if( key.buf[ key.len - 1 ] == (char) 255u )
		key.len++;
	key.buf[ key.len - 1 ]++;

	nf_AIPlayer *aiplayer = new nf_AIPlayer;
	aiplayer->SetKey( &key );
	aiplayer->SetName( "--" );

	if ( !g_gamesetup.GetHandicapping() )
	{
		aiplayer->SetCivPoints( g_gamesetup.GetCivPoints() );
		aiplayer->SetPwPoints( g_gamesetup.GetPwPoints() );
	}

	aiplayer->Pack();

	g_netfunc->InsertAIPlayer( aiplayer );

	m_shouldAddAIPlayer--;
}


void AllinoneWindow::Update(void)
{
	ns_GPlayerListBox *list = (ns_GPlayerListBox *)
		FindControl( AllinoneWindow::CONTROL_GPLAYERSLISTBOX );
	list->Destroy();

	((aui_TextField *)FindControl(CONTROL_GAMENAMETEXTFIELD))->SetFieldText("");


	if ( !s_dbw )
	{
		switch ( m_mode )
		{
		case CREATE:
		case CONTINUE_CREATE:
			s_dbw = DialogBoxWindow::PopUp(
				"createdialogboxwindow",
				m_dbActionArray );
			break;

		case JOIN:
		case CONTINUE_JOIN:
			s_dbw = DialogBoxWindow::PopUp(
				"joindialogboxwindow",
				m_dbActionArray );
			break;

		default:

			Assert( FALSE );
			break;
		}
	}
}


AUI_ERRCODE AllinoneWindow::SetParent( aui_Region *region )
{

	if ( region ) {
		ns_ChatBox *chatbox = (ns_ChatBox *)FindControl( CONTROL_CHATBOX );
		chatbox->SetText( "" );
		chatbox->GetInputField()->SetKeyboardFocus();


		RemoveChild( m_controls[ CONTROL_RULESSHEET ]->Id() );
		RemoveChild( m_controls[ CONTROL_EXCLUSIONSSHEET ]->Id() );
		AddChild( m_controls[ CONTROL_PLAYERSSHEET ] );

		m_joinedGame = false;
		m_receivedGuids = false;
		g_playersetup.SetTribe( 0 );

		memset( m_lname, 0, sizeof( m_lname ) );




	}

	return ns_Window::SetParent(region);
}


void AllinoneWindow::PlayersListBoxAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != AUI_LISTBOX_ACTION_SELECT ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	ns_HPlayerListBox *listbox = (ns_HPlayerListBox *)control;
	tech_WLList<sint32>	justSelectedList;
	tech_WLList<sint32>	justDeselectedList;
	listbox->WhatsChanged(justSelectedList,justDeselectedList);

	w->UpdatePlayerButtons();

	ns_ChatBox *chatbox = (ns_ChatBox *)w->
		FindControl(AllinoneWindow::CONTROL_CHATBOX);

	ListPos position = justDeselectedList.GetHeadPosition();
	sint32 i;
	for ( i = justDeselectedList.L(); i; i-- )
    {
		sint32 index = justDeselectedList.GetNext( position );
		ns_HPlayerItem *item = (ns_HPlayerItem *)listbox->GetItemByIndex(index);
		if ( !item->IsAI() )
		{
			NETFunc::Player *player = item->GetPlayer();
			if(chatbox->GetPlayer() && player->Equals(chatbox->GetPlayer()))
			{
				chatbox->SetPlayer(0);
			}
		}
	}

	position = justSelectedList.GetHeadPosition();
	for ( i = justSelectedList.L(); i; i-- )
    {
		sint32 index = justSelectedList.GetNext( position );
		ns_HPlayerItem *item = (ns_HPlayerItem *)listbox->GetItemByIndex(index);
		if (item->IsAI())
		{
			chatbox->SetPlayer(0);
		}
		else
		{
			NETFunc::Player *player = item->GetPlayer();
			chatbox->SetPlayer(player);
		}
	}

	if (!chatbox->GetPlayer() || chatbox->GetPlayer()->IsMe())
    {
		aui_Switch * s = (aui_Switch *) w->FindControl( w->CONTROL_PPTSWITCH );
		s->SetState(k_PPT_PUBLIC);
	}

	justSelectedList.DeleteAll();
	justDeselectedList.DeleteAll();
}


void AllinoneWindow::UpdatePlayerButtons( void )
{
	ns_HPlayerListBox *listbox = (ns_HPlayerListBox *)
		m_controls[ CONTROL_HPLAYERSLISTBOX ];
	ns_HPlayerItem *item = (ns_HPlayerItem *)listbox->GetSelectedItem();
	if ( item )
	{
		if ( item->IsAI() )
		{
			m_controls[ CONTROL_INFOBUTTON ]->Enable( false );
			m_controls[ CONTROL_PPTSWITCH ]->Enable( false );
		}
		else
		{
			m_controls[ CONTROL_INFOBUTTON ]->Enable( true );
			m_controls[ CONTROL_PPTSWITCH ]->Enable(!item->GetPlayer()->IsMe());
		}

		if ( g_netfunc->IsHost() )
		{
			if ( item->IsAI() )
			{
				m_controls[ CONTROL_KICKBUTTON ]->Enable( true );
			}
			else
			{
		        m_controls[ CONTROL_KICKBUTTON ]->Enable(!item->GetPlayer()->IsMe());
			}
		}
	}
	else
	{
		m_controls[ CONTROL_INFOBUTTON ]->Enable( false );
		m_controls[ CONTROL_PPTSWITCH ]->Enable( false );

		if ( g_netfunc->IsHost() )
		{
			m_controls[ CONTROL_KICKBUTTON ]->Enable( false );
		}
	}
}


void AllinoneWindow::GameNameTextFieldAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_TEXTFIELD_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	char name[ dp_SNAMELEN + 1 ];
	((aui_TextField *)g_allinoneWindow->FindControl(
		AllinoneWindow::CONTROL_GAMENAMETEXTFIELD ))->
			GetFieldText(name, dp_SNAMELEN);

	g_gamesetup.SetName(name);
	w->UpdateGameSetup();

	control->ReleaseKeyboardFocus();
}


void AllinoneWindow::PPTSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;
	ns_ChatBox *chatbox = (ns_ChatBox *)(w->FindControl(AllinoneWindow::CONTROL_CHATBOX));

	aui_Switch *s = (aui_Switch *)control;

    switch (s->GetState())
	{
	case k_PPT_PUBLIC:
		chatbox->SetWhisper(false);
		chatbox->SetGroup(false);
		break;

	case k_PPT_PRIVATE:
		if(!chatbox->GetPlayer() || chatbox->GetPlayer()->IsMe())
		{
			s->SetState(k_PPT_PUBLIC);
		}
		else
		{
			chatbox->SetWhisper(true);
		}
		chatbox->SetGroup(false);
		break;






	default:

		Assert( FALSE );
		break;
	}

	s->SetText( w->m_PPTStrings->GetString( s->GetState() ) );
}

void AllinoneWindow::KickButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	Assert( g_netfunc->IsHost() );
	if ( !g_netfunc->IsHost() ) return;

	AllinoneWindow *w = (AllinoneWindow *)control->GetParentWindow();
	ns_HPlayerListBox *listbox = (ns_HPlayerListBox *)w->
		FindControl( AllinoneWindow::CONTROL_HPLAYERSLISTBOX );
	ns_HPlayerItem *item = (ns_HPlayerItem *)listbox->GetSelectedItem();

	if( item )
	{
		if ( item->IsAI() )
		{

			sint16 newMaxNumHumans = g_gamesetup.GetMaxPlayers();
			sint32 maxPlayersTotal = k_NS_MAX_PLAYERS;
			if(w->GetScenarioInfo()->isScenario &&
			   (w->GetScenarioInfo()->m_startInfoType != (uint8)STARTINFOTYPE_NONE &&
			    w->GetScenarioInfo()->m_startInfoType != (uint8)STARTINFOTYPE_NOLOCS)) {
				if(w->GetScenarioInfo()->m_numStartPositions < k_NS_MAX_PLAYERS)
					maxPlayersTotal = w->GetScenarioInfo()->m_numStartPositions;
			}

			if ( maxPlayersTotal - w->CurNumAiPlayers() < k_NS_MAX_HUMANS )
				++newMaxNumHumans;
			Assert( newMaxNumHumans <= k_NS_MAX_HUMANS );
			if ( newMaxNumHumans > k_NS_MAX_HUMANS ) newMaxNumHumans = k_NS_MAX_HUMANS;
			g_gamesetup.SetSize(newMaxNumHumans);
			w->UpdateGameSetup();

			w->UpdatePlayerButtons();

			nf_AIPlayer *player = item->GetAIPlayer();

			w->DeleteAIPlayer( player );
			g_netfunc->DeleteAIPlayer( player );
		}
		else
		{
			NETFunc::Player *player = item->GetPlayer();
			if ( !w->IsMine( player ) )
			{
				g_netfunc->Kick(player);

				w->UpdatePlayerButtons();
			}
		}
	}
}


void AllinoneWindow::InfoButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	AllinoneWindow *w = g_allinoneWindow;
	ns_HPlayerListBox *l = (ns_HPlayerListBox *)w->
		FindControl( AllinoneWindow::CONTROL_HPLAYERSLISTBOX );

	ns_HPlayerItem *item = (ns_HPlayerItem *)(l->GetSelectedItem());
	if(item && !item->IsAI() )
	{
		NETFunc::Player *player = item->GetPlayer();

		if ( w->IsMine( player ) )
		{
			PlayerEditWindow *p = (PlayerEditWindow *)g_netshell->
				FindWindow( NetShell::WINDOW_PLAYEREDIT );
			p->SetPlayerSetup(&g_playersetup);
			p->SetMode(p->EDIT_GAMESETUP);
			g_netshell->GetCurrentScreen()->AddWindow(p, TRUE);
		}
		else
		{
			g_netfunc->GetPlayerSetupPacket(player);
		}
	}
}


void AllinoneWindow::OKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	if ( g_playersetup.IsReadyToLaunch() ) return;

#if defined(USE_SDL)
	bool legalGame = true;
#else // USE_SDL
	bool legalGame = c3files_HasLegalCD();
#endif // USE_SDL
	if (g_netfunc->IsHost() && !legalGame )
	{
		static ns_String donthavecd( "strings.system.donthavecd" );
		g_netfunc->PushChatMessage( donthavecd.GetString() );
		return;
	}

	AllinoneWindow *w = g_allinoneWindow;

	if ( g_netfunc->IsHost() && !g_gamesetup.GetDynamicJoin() )
	{

		if ( w->CurNumHumanPlayers() <= 1 ||
			 ((aui_ListBox *)w->FindControl( CONTROL_GPLAYERSLISTBOX ))->
			 NumItems() <= 1 )
		{
			if ( !w->FindControl( CONTROL_GPLAYERSLISTBOX )->IsDisabled() )
				w->FindControl( CONTROL_GPLAYERSLISTBOX )->Enable( false );

			return;
		}
	}


	char name[ dp_SNAMELEN + 1 ];
	((aui_TextField *)g_allinoneWindow->FindControl(
		AllinoneWindow::CONTROL_GAMENAMETEXTFIELD ))->
			GetFieldText(name, dp_SNAMELEN);
	g_theProfileDB->SetGameName( name );















	g_theProfileDB->SetCivName( g_playersetup.GetName() );


	if ( g_netfunc->IsHost() )
	{
		sint32 playStyleValue = g_gamesetup.GetPlayStyleValue();

		switch ( g_gamesetup.GetPlayStyle() )
		{
		default:
			Assert( FALSE );

		case 0:
			g_network.SetClassicStyle();
			break;
		case 1:
			g_network.SetSpeedStyle( TRUE, playStyleValue, FALSE, 0 );

			g_theProfileDB->SetZoomedCombatAlways( FALSE );
			g_theProfileDB->SetThroneRoom( FALSE );

			break;
		case 2:
			g_network.SetSpeedStyle(TRUE, playStyleValue, FALSE, 0);
			g_network.SetCarryoverStyle( TRUE );
			break;
		case 3:
			g_network.SetSpeedStyle(TRUE, playStyleValue, FALSE, playStyleValue);
			break;








		}
	}




	if ( g_netfunc->IsHost() )
	{
		aui_Switch *ls = (aui_Switch *)w->FindControl( w->CONTROL_LOCKSWITCH );

		if ( g_gamesetup.GetDynamicJoin() )
		{

			ls->GetAction()->Execute( ls, AUI_SWITCH_ACTION_OFF, 0 );
		}
		else
		{

			ls->GetAction()->Execute( ls, AUI_SWITCH_ACTION_ON, 0 );
		}
	}

































	agesscreen_setStartAge( g_gamesetup.GetStartAge() );
	agesscreen_setEndAge( g_gamesetup.GetEndAge() );
	spnewgamemapsizescreen_setMapSizeIndex( g_gamesetup.GetMapSize() );

	spnewgamemapshapescreen_setMapShapeIndex( g_gamesetup.GetWorldShape() );


	spnewgametribescreen_setTribeIndex(
		g_playersetup.GetTribe() - 1,
		strlen( w->m_lname ) ? w->m_lname : NULL );

	custommapscreen_setValues(
		g_gamesetup.GetWorldType1(),
		g_gamesetup.GetWorldType2(),
		g_gamesetup.GetWorldType3(),
		g_gamesetup.GetWorldType4(),
		g_gamesetup.GetWorldType5(),
		g_gamesetup.GetWorldType6() );
	spnewgamediffscreen_setDifficulty1( g_gamesetup.GetDifficulty1() );
	spnewgamediffscreen_setDifficulty2( g_gamesetup.GetDifficulty2() );




	g_theProfileDB->SetGenocideRule(
		((aui_Switch *)g_allinoneWindow->FindControl(
			AllinoneWindow::CONTROL_BLOODLUSTSWITCH ))->GetState() );

























	g_theProfileDB->SetPollutionRule(
		((aui_Switch *)g_allinoneWindow->FindControl(
			AllinoneWindow::CONTROL_POLLUTIONSWITCH ))->GetState() );







	g_theProfileDB->SetPowerPoints(
		((c3_EditButton *)g_allinoneWindow->FindControl(
			AllinoneWindow::CONTROL_CIVPOINTSBUTTON ))->GetValue() );













	NETFunc::STATUS status = g_netfunc->Launch();
    Assert(NETFunc::OK == status);


	g_playersetup.SetReadyToLaunch( true );
	control->Enable( FALSE );

#ifdef LOCKSETTINGSONLAUNCH

	if ( g_netfunc->IsHost() )
	{

		uint8 isScenario = g_allinoneWindow->GetScenarioInfo()->isScenario;

		g_allinoneWindow->SetMode( g_allinoneWindow->GetMode() );
		g_allinoneWindow->GetScenarioInfo()->isScenario = isScenario;

		g_allinoneWindow->FindControl( g_allinoneWindow->CONTROL_LOCKSWITCH )->
			Enable( TRUE );
		g_allinoneWindow->UpdatePlayerButtons();
	}

	ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)
		g_allinoneWindow->FindControl( CONTROL_HPLAYERSLISTBOX );
	for ( sint32 i = 0; i < hplistbox->NumItems(); i++ )
	{
		ns_HPlayerItem *item = (ns_HPlayerItem *)hplistbox->GetItemByIndex( i );
		item->GetTribeButton()->Enable( FALSE );

		item->GetCivpointsButton()->Enable( FALSE );
		item->GetPwpointsButton()->Enable( FALSE );
	}
#endif

	static ns_String readytolaunch( "strings.system.readytolaunch" );
	g_netfunc->PushChatMessage( readytolaunch.GetString() );
}

void AllinoneWindow_SetupGameForLaunch( void )
{

	sint32 numPlayers = ((aui_ListBox *)g_allinoneWindow->FindControl(
		AllinoneWindow::CONTROL_HPLAYERSLISTBOX))->NumItems();

	g_theProfileDB->SetNPlayers(numPlayers + 1);

	aui_ListBox *HPlayersList = (aui_ListBox *)g_allinoneWindow->FindControl(AllinoneWindow::CONTROL_HPLAYERSLISTBOX);
	sint32 i;
	for(i = 0; i < numPlayers; i++) {
		ns_HPlayerItem *item = (ns_HPlayerItem *)HPlayersList->GetItemByIndex(i);
		NETFunc::Player *nfPlayer;
		if(item->IsAI()) {
			nf_AIPlayer *aiPlayer = item->GetAIPlayer();
			Assert(aiPlayer);
			if(aiPlayer) {
				int tribe = aiPlayer->GetTribe();
				if(tribe == 0)
					tribe = CIV_INDEX_RANDOM;
				else
					tribe--;

				if(g_gamesetup.GetHandicapping()) {
					g_network.SetNSAIPlayerInfo(tribe,
												aiPlayer->GetGroup(),
												aiPlayer->GetCivPoints(),
												aiPlayer->GetPwPoints());
				} else {
					g_network.SetNSAIPlayerInfo(tribe,
												aiPlayer->GetGroup(),
												g_gamesetup.GetCivPoints(),
												g_gamesetup.GetPwPoints());
				}

			}
		} else {
			nfPlayer = item->GetPlayer();
			nf_PlayerSetup playersetup(nfPlayer);

			int tribe = playersetup.GetTribe();
			if(tribe == 0)
				tribe = CIV_INDEX_RANDOM;
			else
				tribe--;

			if(g_gamesetup.GetHandicapping()) {
				g_network.SetNSPlayerInfo(nfPlayer->GetId(),
										  nfPlayer->GetName(),
										  tribe,
										  playersetup.GetGroup(),
										  playersetup.GetCivPoints(),
										  playersetup.GetPwPoints());
			} else {
				g_network.SetNSPlayerInfo(nfPlayer->GetId(),
										  nfPlayer->GetName(),
										  tribe,
										  playersetup.GetGroup(),
										  g_gamesetup.GetCivPoints(),
										  g_gamesetup.GetPwPoints());
			}
		}
	}

	g_theProfileDB->SetAI(g_netfunc->IsHost());

	g_network.SetStartingAge(agesscreen_getStartAge());
	g_network.SetEndingAge(agesscreen_getEndAge());
	g_network.SetDynamicJoin(g_gamesetup.GetDynamicJoin());
	ns_ScenarioInfo *scenInfo = g_allinoneWindow->GetScenarioInfo();

	if(scenInfo->isScenario) {
		g_useScenarioCivs = numPlayers;
	}

	if(scenInfo->isScenario) {
		g_theProfileDB->SetIsScenario(TRUE);





		ScenarioPack *pack;
		Scenario *scen;
		strcpy(g_scenarioName, scenInfo->m_gameName);
		if(g_civScenarios->FindScenario(g_scenarioName,
										&pack, &scen)) {

			g_civPaths->SetCurScenarioPath(scen->m_path);
			g_civPaths->SetCurScenarioPackPath(pack->m_path);
		}
	} else {
		if(scenInfo->m_scenarioName[0] != 0) {
			strcpy(g_scenarioName, scenInfo->m_scenarioName);
			ScenarioPack *pack;
			Scenario *scen;
			if(g_civScenarios->FindScenario(g_scenarioName,
											&pack, &scen)) {

				g_theProfileDB->SetIsScenario(TRUE);
				g_civPaths->SetCurScenarioPath(scen->m_path);
				g_civPaths->SetCurScenarioPackPath(pack->m_path);
			}
		} else {
			g_theProfileDB->SetIsScenario(FALSE);
			g_civPaths->ClearCurScenarioPath();
		}
	}
}


























void AllinoneWindow::RulesButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_ui->AddWindow( g_rulesWindow );
}


void AllinoneWindow::ExclusionsButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_ui->AddWindow( g_exclusionsWindow );
}






























void AllinoneWindow::RulesOKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_ui->RemoveWindow( g_rulesWindow->Id() );
}


void AllinoneWindow::ExclusionsOKButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_ui->RemoveWindow( g_exclusionsWindow->Id() );
}


void AllinoneWindow::CancelButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;
	if(g_netfunc->GetTransport()) {

		if ( g_netfunc->IsHost() )
		{
			AllinoneWindow *w = g_allinoneWindow;

			w->AssignTribe(
				0,
				*(uint16 *)g_netfunc->GetPlayer()->GetKey()->buf,
				FALSE,
				FALSE,
				TRUE );

			ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)
				w->FindControl( w->CONTROL_HPLAYERSLISTBOX );
			for ( sint32 i = 0; i < hplistbox->NumItems(); i++ )
			{
				ns_HPlayerItem *item = (ns_HPlayerItem *)hplistbox->
					GetItemByIndex( i );
				nf_AIPlayer *aiplayer = item->GetAIPlayer();
				if ( aiplayer )
				{
					w->AssignTribe(
						0,
						*(uint16 *)aiplayer->GetKey()->buf,
						TRUE,
						FALSE,
						TRUE );

					w->DeleteAIPlayer( aiplayer );
					g_netfunc->DeleteAIPlayer( aiplayer );
				}
			}

			g_gamesetup.SetClosed( FALSE );
			g_gamesetup.SetSize( k_NS_MAX_HUMANS );

			/// @todo Use Os::Sleep
#ifdef WIN32
			Sleep(k_PACKET_DELAY);
#else
			usleep(k_PACKET_DELAY * 1000);
#endif
//			std::this_thread::sleep_for(std::chrono::milliseconds(k_PACKET_DELAY));
			w->UpdateGameSetup();
		}
		g_netfunc->Leave();
		LobbyWindow *w = (LobbyWindow *)(g_netshell->FindWindow( NetShell::WINDOW_LOBBY ));
		g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
		w->Update();
	} else {
#ifdef WIN32
		PostMessage( g_ui->TheHWND(), WM_CLOSE, 0, 0 );
#endif
}
}

void AllinoneWindow::ReviewButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_allinoneWindow->SpitOutGameSetup();
}




void AllinoneWindow::SpitOutGameSetup( void )
{
	bool displayedSomething = false;

	const sint32 biglen = 2 << 15;
	static MBCHAR info[ biglen + 1 ];
	static MBCHAR moreinfo[ biglen + 1 ];
	static MBCHAR temp[ biglen + 1 ];


	memset( info, 0, sizeof( info ) );

	static ns_String customRules( "strings.customrules" );
	strncat( info, customRules.GetString(), biglen );
	strncat( info, "\n", biglen );



















	memset( moreinfo, 0, sizeof( moreinfo ) );
	sint32 i;
	for ( i = 0; i < m_numAvailUnits; i++ )
	{
		if ( m_units[ i ] && m_units[ i ]->GetState() )
		{
			strcat( moreinfo, m_units[ i ]->GetText() );
			strcat( moreinfo, ", " );
		}
	}
	sint32 len = strlen( moreinfo );
	if ( len > 2 )
	{
		static ns_String excludedUnitInfo( "strings.excludedunitinfo" );

		strncpy( temp, excludedUnitInfo.GetString(), biglen );

		moreinfo[ len - 2 ] = '\0';
		strcat( temp, moreinfo );

		strncat( info, temp, biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}

	memset( moreinfo, 0, sizeof( moreinfo ) );
	for ( i = 0; i < m_numAvailImprovements; i++ )
	{
		if ( m_improvements[ i ]->GetState() )
		{
			strcat( moreinfo, m_improvements[ i ]->GetText() );
			strcat( moreinfo, ", " );
		}
	}
	len = strlen( moreinfo );
	if ( len > 2 )
	{
		static ns_String excludedImprovementInfo(
			"strings.excludedimprovementinfo" );

		strncpy( temp, excludedImprovementInfo.GetString(), biglen );

		moreinfo[ len - 2 ] = '\0';
		strcat( temp, moreinfo );

		strncat( info, temp, biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}

	memset( moreinfo, 0, sizeof( moreinfo ) );
	for ( i = 0; i < m_numAvailWonders; i++ )
	{
		if ( m_wonders[ i ]->GetState() )
		{
			strcat( moreinfo, m_wonders[ i ]->GetText() );
			strcat( moreinfo, ", " );
		}
	}
	len = strlen( moreinfo );
	if ( len > 2 )
	{
		static ns_String excludedWonderInfo( "strings.excludedwonderinfo" );

		strncpy( temp, excludedWonderInfo.GetString(), biglen );

		moreinfo[ len - 2 ] = '\0';
		strcat( temp, moreinfo );

		strncat( info, temp, biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}




	aui_Switch *sw = (aui_Switch *)m_controls[ CONTROL_DYNAMICJOINSWITCH ];
	if ( sw->GetState() )
	{
		static ns_String dynamicJoin( "strings.dynamicjoin" );
		strncat( info, dynamicJoin.GetString(), biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}



















	sw = (aui_Switch *)m_controls[ CONTROL_HANDICAPPINGSWITCH ];
	if ( sw->GetState() )
	{
		static ns_String handicapping( "strings.handicapping" );
		strncat( info, handicapping.GetString(), biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}
	else
	{
		c3_EditButton *r = (c3_EditButton *)m_controls[ CONTROL_CIVPOINTSBUTTON ];
		if ( r->GetValue() > r->GetMinimum() )
		{

			{
				static ns_String goldInfo( "strings.goldinfo" );
				sprintf(
					temp,
					"%s%d",
					goldInfo.GetString(),
					r->GetValue() );
				strncat( info, temp, biglen );
				strncat( info, "\n", biglen );
			}












			displayedSomething = true;
		}
		r = (c3_EditButton *)m_controls[ CONTROL_PWPOINTSBUTTON ];
		if ( r->GetValue() > r->GetMinimum() )
		{

			{
				static ns_String pwInfo( "strings.pwinfo" );
				sprintf(
					temp,
					"%s%d",
					pwInfo.GetString(),
					r->GetValue() );
				strncat( info, temp, biglen );
				strncat( info, "\n", biglen );
			}












			displayedSomething = true;
		}
	}










	sw = (aui_Switch *)m_controls[ CONTROL_BLOODLUSTSWITCH ];
	if ( sw->GetState() )
	{
		static ns_String bloodlust( "strings.bloodlust" );
		strncat( info, bloodlust.GetString(), biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}































	sw = (aui_Switch *)m_controls[ CONTROL_POLLUTIONSWITCH ];
	if ( !sw->GetState() )
	{
		static ns_String pollution( "strings.pollution" );
		strncat( info, pollution.GetString(), biglen );
		strncat( info, "\n", biglen );
		displayedSomething = true;
	}




	{
		displayedSomething = true;
		AUI_ERRCODE errcode = AUI_ERRCODE_OK;

		static ns_String mapsize( "strings.mapsize" );
		static aui_StringTable mapsizestrings( &errcode, "strings.mapsizestrings" );
		sprintf(
			temp,
			"%s: %s\n",
			mapsize.GetString(),
			mapsizestrings.GetString( g_gamesetup.GetMapSize() ) );
		strncat( info, temp, biglen );

		static ns_String worldtype1( "strings.worldtype1" );
		sprintf(
			temp,
			"%s%d\n",
			worldtype1.GetString(),
			g_gamesetup.GetWorldType1() );
		strncat( info, temp, biglen );

		static ns_String worldtype2( "strings.worldtype2" );
		sprintf(
			temp,
			"%s%d\n",
			worldtype2.GetString(),
			g_gamesetup.GetWorldType2() );
		strncat( info, temp, biglen );

		static ns_String worldtype3( "strings.worldtype3" );
		sprintf(
			temp,
			"%s%d\n",
			worldtype3.GetString(),
			g_gamesetup.GetWorldType3() );
		strncat( info, temp, biglen );

		static ns_String worldtype4( "strings.worldtype4" );
		sprintf(
			temp,
			"%s%d\n",
			worldtype4.GetString(),
			g_gamesetup.GetWorldType4() );
		strncat( info, temp, biglen );

		static ns_String worldtype5( "strings.worldtype5" );
		sprintf(
			temp,
			"%s%d\n",
			worldtype5.GetString(),
			g_gamesetup.GetWorldType5() );
		strncat( info, temp, biglen );

		static ns_String worldtype6( "strings.worldtype6" );
		sprintf(
			temp,
			"%s%d\n",
			worldtype6.GetString(),
			g_gamesetup.GetWorldType6() );
		strncat( info, temp, biglen );

		static ns_String worldshape( "strings.worldshape" );
		static aui_StringTable worldshapestrings( &errcode, "strings.worldshapestrings" );
		sprintf(
			temp,
			"%s: %s\n",
			worldshape.GetString(),
			worldshapestrings.GetString( g_gamesetup.GetWorldShape() ) );
		strncat( info, temp, biglen );

		static ns_String difficulty1( "strings.difficulty1" );
		static aui_StringTable difficulty1strings( &errcode, "strings.difficulty1strings" );
		sprintf(
			temp,
			"%s: %s\n",
			difficulty1.GetString(),
			difficulty1strings.GetString( g_gamesetup.GetDifficulty1() ) );
		strncat( info, temp, biglen );

		static ns_String difficulty2( "strings.difficulty2" );
		static aui_StringTable difficulty2strings( &errcode, "strings.difficulty2strings" );
		sprintf(
			temp,
			"%s: %s\n",
			difficulty2.GetString(),
			difficulty2strings.GetString( g_gamesetup.GetDifficulty2() ) );
		strncat( info, temp, biglen );

		static ns_String startage( "strings.startage" );
		sprintf(
			temp,
			"%s %s\n",
			startage.GetString(),
			g_theAgeDB->Get(g_gamesetup.GetStartAge())->GetNameText());
		strncat( info, temp, biglen );

		static ns_String endage( "strings.endage" );

		sprintf(
			temp,
			"%s %s\n",
			endage.GetString(),
			g_theAgeDB->Get(g_gamesetup.GetEndAge())->GetNameText());
		strncat( info, temp, biglen );
	}

	if ( !displayedSomething )
	{
		static ns_String defaultGameSetup( "strings.defaultgamesetup" );
		strncat( info, defaultGameSetup.GetString(), biglen );
		strncat( info, "\n", biglen );
	}


	if ( len > 2 )
	{
		moreinfo[ len - 2 ] = '\0';
	}




	static aui_TextBase textStyle( "styles.system", (const MBCHAR *)NULL );
	((ns_ChatBox *)m_controls[ CONTROL_CHATBOX ])->
		AppendText( info, textStyle.GetTextColor(), FALSE, FALSE );
}


void AllinoneWindow::DialogBoxPopDownAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	g_netfunc->Leave();

	if ( s_dbw )
	{
		DialogBoxWindow::PopDown( s_dbw );
		s_dbw = NULL;
	}
	LobbyWindow *w = (LobbyWindow *)(g_netshell->FindWindow( NetShell::WINDOW_LOBBY ));
	g_netshell->GotoScreen( NetShell::SCREEN_LOBBY );
	w->Update();
}


















































void AllinoneWindow::PlayStyleDropDownAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_DROPDOWN_ACTION_SELECT ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	aui_DropDown *dropdown = (aui_DropDown *)w->
		FindControl( AllinoneWindow::CONTROL_PLAYSTYLEDROPDOWN );
	sint32 index = dropdown->GetListBox()->GetSelectedItemIndex();
	if ( index >= 0 )
	{

		w->UpdateConfig();


		if ( g_gamesetup.GetPlayStyle() != index )
			((ctp2_Spinner *)w->FindControl(
				AllinoneWindow::CONTROL_PLAYSTYLEVALUESPINNER ))->
					SetValue( 0, 0 );

		{
			g_gamesetup.SetPlayStyle( index );
			w->UpdateGameSetup(true);
		}
	}
}



























void AllinoneWindow::PlayStyleValueSpinnerCallback(aui_Control *control, uint32 action, uint32 data, void *cookie)
{
	if(action != AUI_RANGER_ACTION_VALUECHANGE)
		return;

	static bool inCallback = false;
	if(inCallback)
		return;
	inCallback = true;
	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;   /// @todo Check inCallback (this function is blocked forever)

	sint32 value = ((aui_Ranger *)control)->GetValueX();

	{
		g_gamesetup.SetPlayStyleValue( value );
		w->UpdateGameSetup(true);
	}

	inCallback = false;
}


void AllinoneWindow::DynamicJoinSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	BOOL join = ((aui_Switch *)control)->GetState();

	{
		g_gamesetup.SetDynamicJoin( (char)join );
		w->UpdateGameSetup();
	}















}


















































void AllinoneWindow::HandicappingSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	sint32 hand = ((aui_Switch *)control)->GetState();

	w->UpdateConfig();

		g_gamesetup.SetHandicapping(static_cast<char>(hand));

	if ( !g_gamesetup.GetHandicapping() )
	{
		g_playersetup.SetCivPoints( g_gamesetup.GetCivPoints() );
		g_playersetup.SetPwPoints( g_gamesetup.GetPwPoints() );
		w->UpdatePlayerSetup();

		if ( g_netfunc->IsHost() )
		{
			ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)w->
				FindControl( w->CONTROL_HPLAYERSLISTBOX );

			for ( sint32 i = 0; i < hplistbox->NumItems(); i++ )
			{
				ns_HPlayerItem *item = (ns_HPlayerItem *)hplistbox->
					GetItemByIndex( i );

				if ( item->IsAI() )
				{
					nf_AIPlayer *aiplayer = item->GetAIPlayer();
					aiplayer->SetCivPoints( g_gamesetup.GetCivPoints() );
					aiplayer->SetPwPoints( g_gamesetup.GetPwPoints() );
					w->UpdateAIPlayerSetup( aiplayer );
				}
			}
		}
	}

		w->UpdateGameSetup(true);
}

























































































void AllinoneWindow::BloodlustSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	sint32 bloodlust = ((aui_Switch *)control)->GetState();

	{
		g_gamesetup.SetBloodlust(static_cast<char>(bloodlust));
		w->UpdateGameSetup(true);
	}
}

























































































void AllinoneWindow::PollutionSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	sint32 poll = ((aui_Switch *)control)->GetState();

	{
		g_gamesetup.SetPollution(static_cast<char>(poll));
		w->UpdateGameSetup(true);
	}
}


void AllinoneWindow::CivPointsButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	sint32 value = ((c3_EditButton *)control)->GetValue();

		g_gamesetup.SetCivPoints( value );

	if ( !g_gamesetup.GetHandicapping() )
	{
		g_playersetup.SetCivPoints( g_gamesetup.GetCivPoints() );
		w->UpdatePlayerSetup();

		if ( g_netfunc->IsHost() )
		{
			ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)w->
				FindControl( w->CONTROL_HPLAYERSLISTBOX );

			for ( sint32 i = 0; i < hplistbox->NumItems(); i++ )
			{
				ns_HPlayerItem *item = (ns_HPlayerItem *)hplistbox->
					GetItemByIndex( i );

				if ( item->IsAI() )
				{
					nf_AIPlayer *aiplayer = item->GetAIPlayer();
					aiplayer->SetCivPoints( g_gamesetup.GetCivPoints() );
					w->UpdateAIPlayerSetup( aiplayer );
				}
			}
		}
	}

		w->UpdateGameSetup(true);
}

void AllinoneWindow::PwPointsButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	sint32 value = ((c3_EditButton *)control)->GetValue();

		g_gamesetup.SetPwPoints( value );

	if ( !g_gamesetup.GetHandicapping() )
	{
		g_playersetup.SetPwPoints( g_gamesetup.GetPwPoints() );
		w->UpdatePlayerSetup();

		if ( g_netfunc->IsHost() )
		{
			ns_HPlayerListBox *hplistbox = (ns_HPlayerListBox *)w->
				FindControl( w->CONTROL_HPLAYERSLISTBOX );

			for ( sint32 i = 0; i < hplistbox->NumItems(); i++ )
			{
				ns_HPlayerItem *item = (ns_HPlayerItem *)hplistbox->
					GetItemByIndex( i );

				if ( item->IsAI() )
				{
					nf_AIPlayer *aiplayer = item->GetAIPlayer();
					aiplayer->SetPwPoints( g_gamesetup.GetPwPoints() );
					w->UpdateAIPlayerSetup( aiplayer );
				}
			}
		}
	}

		w->UpdateGameSetup(true);
}





































































void AllinoneWindow::AgesButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;

	agesscreen_displayMyWindow(
		w->GetMode() == JOIN ||
		w->GetMode() == CONTINUE_CREATE ||
		w->GetMode() == CONTINUE_JOIN ||
		( g_netfunc->IsHost() && g_playersetup.IsReadyToLaunch() ) );
}


void AllinoneAgesCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	agesscreen_removeMyWindow( action );


	if ( w->GetMode() == w->CREATE )
	{
		g_gamesetup.SetStartAge(static_cast<char>(agesscreen_getStartAge()));
		g_gamesetup.SetEndAge(static_cast<char>(agesscreen_getEndAge()));
		w->UpdateGameSetup(true);
	}
}


void AllinoneWindow::MapSizeButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;

	spnewgamemapsizescreen_displayMyWindow(
		w->GetMode() == JOIN ||
		w->GetMode() == CONTINUE_CREATE ||
		w->GetMode() == CONTINUE_JOIN ||
		( g_netfunc->IsHost() && g_playersetup.IsReadyToLaunch() ),
		0 );
}


void AllinoneWindow::WorldShapeButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;


	spnewgamemapshapescreen_displayMyWindow(
		w->GetMode() == JOIN ||
		w->GetMode() == CONTINUE_CREATE ||
		w->GetMode() == CONTINUE_JOIN ||
		( g_netfunc->IsHost() && g_playersetup.IsReadyToLaunch() ) );

}


void AllinoneMapSizeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	spnewgamemapsizescreen_removeMyWindow( action );


	if ( w->GetMode() == w->CREATE )
	{
		g_gamesetup.SetMapSize(static_cast<char>(spnewgamemapsizescreen_getMapSizeIndex()));
		w->UpdateGameSetup(true);
	}
}


void AllinoneWorldShapeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	spnewgamemapshapescreen_removeMyWindow( action );


	if ( w->GetMode() == w->CREATE )
	{
		g_gamesetup.SetWorldShape
            (static_cast<char>(spnewgamemapshapescreen_getMapShapeIndex()));
		w->UpdateGameSetup(true);
	}
}


void AllinoneTribeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	ns_HPlayerItem *item = (ns_HPlayerItem *)cookie;

	spnewgametribescreen_removeMyWindow(
		action,
		item->IsAI() ? NULL : w->m_lname );




	sint32 index = spnewgametribescreen_getTribeIndex() + 1;
	if ( index < 0 ) index = 0;

	if ( !item->GetPlayer() && !item->GetAIPlayer() ) return;

	uint16 key = item->IsAI() ?
		key = *(uint16 *)item->GetAIPlayer()->GetKey()->buf :
		key = *(uint16 *)item->GetPlayer()->GetKey()->buf;


	if ( g_netfunc->IsHost() && index == 0 )
	{
		w->AssignTribe(
			0,
			key,
			item->IsAI(),
			FALSE,
			TRUE );

		index = w->FindTribe( 0, item->IsAI() );
	}

	if ( item->IsAI() )
	{
		if ( g_netfunc->IsHost() )
		{
			uint16 curKey;
			BOOL curIsAI;
			BOOL curIsFemale;
			BOOL success = w->WhoHasTribe( index, &curKey, &curIsAI, &curIsFemale );
			Assert( success );

			if ( curKey == 0 || curKey == key )
			{

				BOOL success = w->AssignTribe(
					index,
					key,
					TRUE,
					s_maleRadio ? !s_maleRadio->GetState() : FALSE,
					FALSE );
				Assert( success );
			}
			else
			{

			}








			spnewgametribescreen_setTribeIndex(
				g_playersetup.GetTribe() - 1,
				strlen( w->m_lname ) ? w->m_lname : NULL );
		}
	}
	else
	{
		ns_GPlayerListBox *gplistbox = (ns_GPlayerListBox *)
			w->FindControl( AllinoneWindow::CONTROL_GPLAYERSLISTBOX );

		if ( gplistbox->NumItems() && w->IsMine( item->GetPlayer() ) )
		{
			if ( !g_netfunc->IsHost() )
			{

				w->RequestTribe( index );
			}
			else
			{
				uint16 curKey;
				BOOL curIsAI;
				BOOL curIsFemale;
				BOOL success = w->WhoHasTribe( index, &curKey, &curIsAI, &curIsFemale );
				Assert( success );

				if ( curKey == 0 )
				{

					BOOL success = w->AssignTribe(
						index,
						key,
						FALSE,
						FALSE,
						FALSE );
					Assert( success );
				}
				else if ( curKey != key )
				{

				}
				else
				{

					Assert( curIsAI == FALSE );
				}
			}
		}
	}
}


void AllinoneWindow::WorldTypeButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;

	custommapscreen_displayMyWindow(
		w->GetMode() == JOIN ||
		w->GetMode() == CONTINUE_CREATE ||
		w->GetMode() == CONTINUE_JOIN ||
		( g_netfunc->IsHost() && g_playersetup.IsReadyToLaunch() ) );
}


void AllinoneWorldTypeCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	custommapscreen_removeMyWindow( action );


	if ( w->GetMode() == w->CREATE )
	{
		sint32 val1, val2, val3, val4, val5, val6;
		custommapscreen_getValues( val1, val2, val3, val4, val5, val6 );
		g_gamesetup.SetWorldType1(static_cast<sint8>(val1));
		g_gamesetup.SetWorldType2(static_cast<sint8>(val2));
		g_gamesetup.SetWorldType3(static_cast<sint8>(val3));
		g_gamesetup.SetWorldType4(static_cast<sint8>(val4));
		g_gamesetup.SetWorldType5(static_cast<sint8>(val5));
		g_gamesetup.SetWorldType6(static_cast<sint8>(val6));
		w->UpdateGameSetup(true);
	}
}


void AllinoneWindow::DifficultyButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;

	spnewgamediffscreen_displayMyWindow(
		w->GetMode() == JOIN ||
		w->GetMode() == CONTINUE_CREATE ||
		w->GetMode() == CONTINUE_JOIN ||
		( g_netfunc->IsHost() && g_playersetup.IsReadyToLaunch() ) );
}


void AllinoneDifficultyCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void* cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	spnewgamediffscreen_removeMyWindow( action );


	if ( w->GetMode() == w->CREATE )
	{
		g_gamesetup.SetDifficulty1(static_cast<char>(spnewgamediffscreen_getDifficulty1()));
		g_gamesetup.SetDifficulty2(static_cast<char>(spnewgamediffscreen_getDifficulty2()));
		w->UpdateGameSetup(true);
	}
}


void AllinoneWindow::UnitExclusionAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	if(!g_netfunc)
		return;

	if ( g_netfunc->IsHost() )
	{
		AllinoneWindow *w = g_allinoneWindow;
		if ( !w ) return;

		aui_Switch *sw = (aui_Switch *)control;

		if(!sw) return;


		if(g_exclusions)
			g_exclusions->ExcludeUnit( m_index, sw->GetState() );

		{
			g_gamesetup.SetUnit( (char)sw->GetState(), m_index );
			w->UpdateGameSetup(true);
		}
	}
}


void AllinoneWindow::ImprovementExclusionAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	if ( g_netfunc->IsHost() )
	{
		AllinoneWindow *w = g_allinoneWindow;
		if ( !w ) return;

		aui_Switch *sw = (aui_Switch *)control;

		g_exclusions->ExcludeBuilding( m_index, sw->GetState() );

		{
			g_gamesetup.SetImprovement( (char)sw->GetState(), m_index );
			w->UpdateGameSetup(true);
		}
	}
}


void AllinoneWindow::WonderExclusionAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	if ( g_netfunc->IsHost() )
	{
		AllinoneWindow *w = g_allinoneWindow;
		if ( !w ) return;

		aui_Switch *sw = (aui_Switch *)control;

		g_exclusions->ExcludeWonder( m_index, sw->GetState() );

		{
			g_gamesetup.SetWonder( (sint8)sw->GetState(), m_index );
			w->UpdateGameSetup(true);
		}
	}
}


void AllinoneWindow::LockSwitchAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON
	&&   action != (uint32)AUI_SWITCH_ACTION_OFF ) return;


	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	bool lock = ( action == (uint32)AUI_SWITCH_ACTION_ON ) ? true : false;

	if ( lock )
	{
		if ( !g_gamesetup.IsClosed() )
		{

			g_gamesetup.SetSize(static_cast<char>(w->CurNumHumanPlayers()));
			dpEnableNewPlayers(g_netfunc->GetDP(), 0);
			g_gamesetup.SetClosed( true );
			w->UpdateGameSetup();
		}
	}
	else
	{
		if ( g_gamesetup.IsClosed() )
		{

			sint32 newMaxNumHumans = k_NS_MAX_PLAYERS - w->CurNumAiPlayers();
			if ( newMaxNumHumans > k_NS_MAX_HUMANS )
				newMaxNumHumans = k_NS_MAX_HUMANS;
			g_gamesetup.SetSize(static_cast<char>(newMaxNumHumans));
			dpEnableNewPlayers(g_netfunc->GetDP(), 1);
			g_gamesetup.SetClosed( false );
			w->UpdateGameSetup();
		}
	}
}


void AllinoneWindow::AddAIButtonAction::Execute(
	aui_Control *control,
	uint32 action,
	uint32 data )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;

	w->AddAIPlayer();
}

void TribesButtonCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;

	ns_HPlayerItem *item = (ns_HPlayerItem *)cookie;

	if ( item->IsAI() )
	{
		spnewgametribescreen_removeTribe( g_playersetup.GetTribe() - 1 );
		spnewgametribescreen_addTribeNoDuplicate( item->GetAIPlayer()->GetTribe() - 1 );
		spnewgametribescreen_setTribeIndex(item->GetAIPlayer()->GetTribe() - 1);
	}
	else
	{
		g_allinoneWindow->UpdateTribeSwitches();
		spnewgametribescreen_setTribeIndex(
			g_playersetup.GetTribe() - 1,
			strlen( w->m_lname ) ? w->m_lname : NULL );
	}

	spnewgametribescreen_displayMyWindow(
		cookie,
		!item->IsAI() );
}








































































































void CivPointsButtonCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{


	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;


	c3_EditButton *button = (c3_EditButton *)control;
	sint32 value = button->GetValue();

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	ns_HPlayerItem *item = (ns_HPlayerItem *)cookie;

	if ( item->IsAI() )
	{
		if ( g_netfunc->IsHost() )
		{
			nf_AIPlayer *aiplayer = item->GetAIPlayer();
			if ( value != aiplayer->GetCivPoints() )
			{
				aiplayer->SetCivPoints( value );
				w->UpdateAIPlayerSetup( aiplayer );
			}
		}
	}
	else
	{
		ns_GPlayerListBox *gplistbox = (ns_GPlayerListBox *)
			w->FindControl( AllinoneWindow::CONTROL_GPLAYERSLISTBOX );

		if ( gplistbox->NumItems() && w->IsMine( item->GetPlayer() ) )
		{
			if ( value != g_playersetup.GetCivPoints() )
			{
				g_playersetup.SetCivPoints( value );
				w->UpdatePlayerSetup();
			}
		}
	}
}


void PwPointsButtonCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	void *cookie )
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	AllinoneWindow *w = g_allinoneWindow;
	if ( !w ) return;

	c3_EditButton *button = (c3_EditButton *)control;
	sint32 value = button->GetValue();

	ns_HPlayerItem *item = (ns_HPlayerItem *)cookie;

	if ( item->IsAI() )
	{
		if ( g_netfunc->IsHost() )
		{
			nf_AIPlayer *aiplayer = item->GetAIPlayer();
			if ( value != aiplayer->GetPwPoints() )
			{
				aiplayer->SetPwPoints( value );
				w->UpdateAIPlayerSetup( aiplayer );
			}
		}
	}
	else
	{
		ns_GPlayerListBox *gplistbox = (ns_GPlayerListBox *)
			w->FindControl( AllinoneWindow::CONTROL_GPLAYERSLISTBOX );

		if ( gplistbox->NumItems() && w->IsMine( item->GetPlayer() ) )
		{
			if ( value != g_playersetup.GetPwPoints() )
			{
				g_playersetup.SetPwPoints( value );
				w->UpdatePlayerSetup();
			}
		}
	}
}

void AllinoneWindow::EnableButtonsForUnlaunch()
{
	ns_HPlayerListBox *lb = ((ns_HPlayerListBox *)m_controls[CONTROL_HPLAYERSLISTBOX]);
	sint32 i;
	for(i = 0; i < lb->NumItems(); i++) {
		ns_HPlayerItem *item = (ns_HPlayerItem *)lb->GetItemByIndex(i);
		if((item->IsAI() && g_netfunc->IsHost()) || (item->GetPlayer() && IsMine(item->GetPlayer()))) {
			if(g_netfunc->IsHost()) {
				item->GetTribeButton()->Enable(TRUE);
				if(g_gamesetup.GetHandicapping()) {
					item->GetCivpointsButton()->Enable(true);
					item->GetPwpointsButton()->Enable(true);
				}
			}
		}
	}
}
