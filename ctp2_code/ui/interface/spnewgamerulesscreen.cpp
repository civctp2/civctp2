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
// THIS FILE MAPS TO SPNEWGAMEPOPUPS.LDL
// Modifications from the original Activision code:
//
// - 7 options total needed to implement a new rule
//
// - Memory leaks repaired.
// - Initialized local variables. (Sep 9th 2005 Martin Gühmann)
// - Removed new rules attempt - E 12.27.2006
// - Added citycapture as an option and mapped it to gameplayoptions
//   but appears to have no affect
// - Changed whole file to look more like gameplayoptions 3.21.2007
// - Added revoltinsurgents option 3.22.2007
// - Added NoRandomCivs option
// - Added aifreeupgrades
// - added unit gold support
// - added city gold support
// - Added no gold deficit for cities
// - Added no production deficit for cities
// - Added no gold hunger for ai
// - Added no shield hunger for ai
// - Added an upgrade option (13-Sep-2008 Martin Gühmann)
// - Added a new combat option (28-Feb-2009 Maq)
// - Added a no goody huts option (20-Mar-2009 Maq)
// - Added custom start/end "ages" button. (11-Apr-2009 Maq)
// - Removed AI specific rules from window, and added mouse-over description
//   for each existing rule. (25-Jul-2009 Maq)
// - Removed sectarian happiness rule as it isn't doing anything.(25-Jul-2009 Maq)
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
#include "aui_stringtable.h"
#include "aui_switch.h"
#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "spnewgamerulesscreen.h"
#include "agesscreen.h"

#include "keypress.h"
//missing?
#include "GameSettings.h" 
#include "screenutils.h"
#include "network.h"

extern C3UI			*g_c3ui;
extern ProfileDB	*g_theProfileDB;
extern Network				g_network;
static c3_PopupWindow	*s_spNewGameRulesScreen	= NULL;
static aui_Switch		*s_genocide			= NULL,
						*s_pollution		= NULL,
						*s_citycapture		= NULL,
						*s_onecity			= NULL,
						*s_revoltinsurgent  = NULL,
						*s_revoltcasualty   = NULL,
						*s_barbspawn		= NULL,
						*s_NonRandomCivs	= NULL,
						*s_Upgrade			= NULL,
						*s_NewCombat		= NULL,
						*s_NoGoodyHuts		= NULL,
						*s_UNITGOLD			= NULL,
						*s_CITYGOLD			= NULL,
						*s_NOCITYLIMIT		= NULL,
						*s_NULL				= NULL;
ctp2_Button				*s_ages				= NULL;
static c3_Static		*m_ruleDetails		= NULL;
static aui_StringTable	*m_ruleDetailsStrings = NULL;

enum
{
	R_GENOCIDE,
	R_POLLUTION,
	GP_CITYCAPTURE,
	R_ONECITY,
	R_INSURGENT,
	R_CASUALTY,
	R_BARBSPAWN,
	R_NonRandomCivs,
	R_UPGRADE,
	R_NEWCOMBAT,
	R_NOGOODYHUTS,
	R_UNITGOLD,
	R_CITYGOLD,
	R_NOCITYLIMIT,
	GP_TOTAL
};

static uint32 check[] =
{
	R_GENOCIDE,
	R_POLLUTION,
	GP_CITYCAPTURE,
	R_ONECITY,
	R_INSURGENT,
	R_CASUALTY,
	R_BARBSPAWN,
	R_NonRandomCivs,
	R_UPGRADE,
	R_NEWCOMBAT,
	R_NOGOODYHUTS,
	R_UNITGOLD,
	R_CITYGOLD,
	R_NOCITYLIMIT,

	GP_TOTAL
};

sint32 spnewgamerulesscreen_updateData()
{
	if(!g_theProfileDB) return -1;

	s_genocide       ->SetState( g_theProfileDB->IsGenocideRule            () );
	s_pollution      ->SetState( g_theProfileDB->IsPollutionRule           () );
	s_citycapture    ->SetState( g_theProfileDB->IsCityCaptureOptions      () );
	s_onecity        ->SetState( g_theProfileDB->IsOneCityChallenge        () );
	s_revoltinsurgent->SetState( g_theProfileDB->IsRevoltInsurgents        () );
	s_revoltcasualty ->SetState( g_theProfileDB->IsRevoltCasualties        () );
	s_barbspawn      ->SetState( g_theProfileDB->IsBarbarianSpawnsBarbarian() );
	s_NonRandomCivs  ->SetState( g_theProfileDB->IsNonRandomCivs           () );
	s_Upgrade        ->SetState( g_theProfileDB->IsUpgrade                 () );
	s_NewCombat      ->SetState( g_theProfileDB->IsNewCombat               () );
	s_NoGoodyHuts    ->SetState( g_theProfileDB->IsNoGoodyHuts             () );
	s_UNITGOLD       ->SetState( g_theProfileDB->IsGoldPerUnitSupport      () );
	s_CITYGOLD       ->SetState( g_theProfileDB->IsGoldPerCity             () );
	s_NOCITYLIMIT    ->SetState( g_theProfileDB->IsNoCityLimit             () );

	return 1;
}


sint32	spnewgamerulesscreen_displayMyWindow()
{
	sint32 retval=0;
	if(!s_spNewGameRulesScreen) { retval = spnewgamerulesscreen_Initialize(); }

	spnewgamerulesscreen_updateData();

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

	if ( s_spNewGameRulesScreen ) return AUI_ERRCODE_OK; 

	strcpy(windowBlock, "SPNewGameRulesScreen");

	s_spNewGameRulesScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGameRulesScreen, errcode) );
		if ( !AUI_NEWOK(s_spNewGameRulesScreen, errcode) ) errcode;

		
		s_spNewGameRulesScreen->Resize(s_spNewGameRulesScreen->Width(),s_spNewGameRulesScreen->Height());
		s_spNewGameRulesScreen->GrabRegion()->Resize(s_spNewGameRulesScreen->Width(),s_spNewGameRulesScreen->Height());
		s_spNewGameRulesScreen->SetStronglyModal(TRUE);


	s_genocide			= spNew_aui_Switch(&errcode, windowBlock, "RuleOne",             spnewgamerulesscreen_checkPress, &check[R_GENOCIDE     ]);
	s_pollution			= spNew_aui_Switch(&errcode, windowBlock, "RuleTwo",             spnewgamerulesscreen_checkPress, &check[R_POLLUTION    ]);
	s_citycapture		= spNew_aui_Switch(&errcode, windowBlock, "CityCapture",         spnewgamerulesscreen_checkPress, &check[GP_CITYCAPTURE ]); //emod5
	s_onecity			= spNew_aui_Switch(&errcode, windowBlock, "OneCity",             spnewgamerulesscreen_checkPress, &check[R_ONECITY      ]); //emod5
	s_revoltinsurgent	= spNew_aui_Switch(&errcode, windowBlock, "RevoltInsurgents",    spnewgamerulesscreen_checkPress, &check[R_INSURGENT    ]); //emod5
	s_revoltcasualty	= spNew_aui_Switch(&errcode, windowBlock, "RevoltCasualties",    spnewgamerulesscreen_checkPress, &check[R_CASUALTY     ]); //emod5
	s_barbspawn			= spNew_aui_Switch(&errcode, windowBlock, "BarbSpawn",           spnewgamerulesscreen_checkPress, &check[R_BARBSPAWN    ]); //emod5
	s_NonRandomCivs		= spNew_aui_Switch(&errcode, windowBlock, "NonRandomCivs",       spnewgamerulesscreen_checkPress, &check[R_NonRandomCivs]); //emod5
	s_Upgrade			= spNew_aui_Switch(&errcode, windowBlock, "Upgrade",             spnewgamerulesscreen_checkPress, &check[R_UPGRADE      ]); //emod5
	s_NewCombat			= spNew_aui_Switch(&errcode, windowBlock, "NewCombat",           spnewgamerulesscreen_checkPress, &check[R_NEWCOMBAT    ]);
	s_NoGoodyHuts		= spNew_aui_Switch(&errcode, windowBlock, "NoGoodyHuts",         spnewgamerulesscreen_checkPress, &check[R_NOGOODYHUTS  ]);
	s_UNITGOLD			= spNew_aui_Switch(&errcode, windowBlock, "UnitGold",            spnewgamerulesscreen_checkPress, &check[R_UNITGOLD     ]); //emod5
	s_CITYGOLD			= spNew_aui_Switch(&errcode, windowBlock, "CityGold",            spnewgamerulesscreen_checkPress, &check[R_CITYGOLD     ]); //emod5
	s_NOCITYLIMIT		= spNew_aui_Switch(&errcode, windowBlock, "NoCityLimit",         spnewgamerulesscreen_checkPress, &check[R_NOCITYLIMIT  ]); //emod5

	s_ages				= spNew_ctp2_Button(&errcode, windowBlock, "AgesButton", spnewgamerulesscreen_agesPress);

	m_ruleDetails		= spNew_c3_Static(&errcode,windowBlock, "RuleDetail");

	//AUI_ERRCODE	errcode = AUI_ERRCODE_OK;
	m_ruleDetailsStrings = new aui_StringTable(&errcode, "RuleDetailsStringTable");
	//Assert(errcode == AUI_ERRCODE_OK);

	spnewgamerulesscreen_updateData();

	MBCHAR block[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( block, "%s.%s", windowBlock, "Name" );
	s_spNewGameRulesScreen->AddTitle( block );
	s_spNewGameRulesScreen->AddClose( spnewgamerulesscreen_exitPress );
	
	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}

AUI_ERRCODE spnewgamerulesscreen_Cleanup()
{
#define mycleanup(mypointer) if(mypointer) { delete mypointer; mypointer = NULL; };

	if ( !s_spNewGameRulesScreen  ) return AUI_ERRCODE_OK; 

	g_c3ui->RemoveWindow( s_spNewGameRulesScreen->Id() );
	keypress_RemoveHandler(s_spNewGameRulesScreen);

	mycleanup(s_genocide);
	mycleanup(s_pollution);
	mycleanup(s_citycapture);
	mycleanup(s_onecity);
	mycleanup(s_revoltinsurgent);
	mycleanup(s_revoltcasualty);
	mycleanup(s_barbspawn);
	mycleanup(s_NonRandomCivs);
	mycleanup(s_Upgrade);
	mycleanup(s_NewCombat);
	mycleanup(s_NoGoodyHuts);
	mycleanup(s_UNITGOLD);
	mycleanup(s_CITYGOLD);
	mycleanup(s_NOCITYLIMIT);
	mycleanup(s_ages);
	mycleanup(m_ruleDetails);

    delete m_ruleDetailsStrings;
	delete s_spNewGameRulesScreen;
    m_ruleDetailsStrings = NULL;
	s_spNewGameRulesScreen = NULL;

	return AUI_ERRCODE_OK;
#undef mycleanup
}

void spnewgamerulesscreen_agesPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if (m_ruleDetails && m_ruleDetailsStrings)
	{
		m_ruleDetails->SetText("", NULL);
		m_ruleDetails->Hide();
		m_ruleDetails->Show();

		char buf[1024];
		sprintf(buf, "%s", m_ruleDetailsStrings->GetString(14));

		m_ruleDetails->SetText(buf);
	}

	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return;

	agesscreen_displayMyWindow(0);
}

void spnewgamerulesscreen_checkPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	uint32 checkbox = *((uint32*)cookie);

	sint32 rule = -1;

	if (m_ruleDetails && m_ruleDetailsStrings)
	{
		m_ruleDetails->SetText("", NULL);
		m_ruleDetails->Hide();

		switch(checkbox)
		{
			case R_GENOCIDE     : rule = 0; break;
			case R_POLLUTION    : rule = 1; break;
			case GP_CITYCAPTURE : rule = 2; break;
			case R_ONECITY      : rule = 3; break;
			case R_INSURGENT    : rule = 4; break;
			case R_CASUALTY     : rule = 5; break;
			case R_BARBSPAWN    : rule = 6; break;
			case R_NonRandomCivs: rule = 7; break;
			case R_UPGRADE      : rule = 8; break;
			case R_NEWCOMBAT    : rule = 9; break;
			case R_NOGOODYHUTS  : rule = 10; break;
			case R_UNITGOLD     : rule = 11; break;
			case R_CITYGOLD     : rule = 12; break;
			case R_NOCITYLIMIT  : rule = 13; break;
			default             : Assert(0); break;
		};

		if (rule >= 0)
		{
			m_ruleDetails->Show();
			m_ruleDetails->SetText("", NULL);

			char buf[1024];
			sprintf(buf, "%s", m_ruleDetailsStrings->GetString(rule));
			m_ruleDetails->SetText(buf);
		}
		else
		{
			m_ruleDetails->SetText("", NULL);
			m_ruleDetails->Hide();
		}
	}


	if ( action != (uint32)AUI_SWITCH_ACTION_PRESS ) return;

	void (ProfileDB::*func)(BOOL) = 0;
	uint32 state = data;

	switch(checkbox)
	{
		case R_GENOCIDE     : func = &ProfileDB::SetGenocideRule            ; break;
		case R_POLLUTION    : func = &ProfileDB::SetPollutionRule           ; break;
		case GP_CITYCAPTURE : func = &ProfileDB::SetCityCaptureOptions      ; break;
		case R_ONECITY      : func = &ProfileDB::SetOneCity                 ; break;
		case R_INSURGENT    : func = &ProfileDB::SetRevoltInsurgents        ; break;
		case R_CASUALTY     : func = &ProfileDB::SetRevoltCasualties        ; break;
		case R_BARBSPAWN    : func = &ProfileDB::SetBarbarianSpawnsBarbarian; break;
		case R_NonRandomCivs: func = &ProfileDB::SetNonRandomCivs           ; break;
		case R_UPGRADE      : func = &ProfileDB::SetUpgrade                 ; break;
		case R_NEWCOMBAT    : func = &ProfileDB::SetNewCombat               ; break;
		case R_NOGOODYHUTS  : func = &ProfileDB::SetNoGoodyHuts             ; break;
		case R_UNITGOLD     : func = &ProfileDB::SetGoldPerUnitSupport      ; break;
		case R_CITYGOLD     : func = &ProfileDB::SetGoldPerCity             ; break;
		case R_NOCITYLIMIT  : func = &ProfileDB::SetNoCityLimit             ; break;
		default             : Assert(0)                                     ; break;
	};

	if(func)
		(g_theProfileDB->*func)(state ? FALSE : TRUE);
}


void spnewgamerulesscreen_exitPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	g_theProfileDB->Save();

	spnewgamerulesscreen_removeMyWindow(action);
}
