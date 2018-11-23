//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player new game difficulty screen
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
// Modifications from the original Activision code:
//
// - Memory leaks repaired.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
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
#include "aui_radio.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"

#include "profileDB.h"

#include "spnewgamewindow.h"
#include "spnewgamediffscreen.h"

#include "keypress.h"

extern C3UI					*g_c3ui;
extern ProfileDB			*g_theProfileDB;

#define k_NUM_DIFFBOXES	6
#define k_NUM_RISKBOXES	4

static c3_PopupWindow	*s_spNewGameDiffScreen	= NULL;

// Skill level selection
static c3_Static *			s_skillLevel		= NULL;
static aui_SwitchGroup *s_group				= NULL;
static aui_Radio **	s_checkBox			= NULL;
static const MBCHAR	*checknames[k_NUM_DIFFBOXES] =
{
	"DiffOne",
	"DiffTwo",
	"DiffThree",
	"DiffFour",
	"DiffFive",
	"DiffSix"
};

// Barbarian threat selection
static c3_Static *	s_barbarianThreat	= NULL;
static aui_SwitchGroup *s_groupTwo			= NULL;
static aui_Radio **	s_riskBox			= NULL;
static const MBCHAR	*risknames[k_NUM_RISKBOXES] =
{
	"RiskOne",
	"RiskTwo",
	"RiskThree",
	"RiskFour"
};

static sint32 s_difficulty1 = 0;
static sint32 s_difficulty2 = 0;

sint32
spnewgamediffscreen_getDifficulty1(void)
{
	return s_difficulty1;
}

sint32
spnewgamediffscreen_getDifficulty2(void)
{
	return s_difficulty2;
}

void
spnewgamediffscreen_setDifficulty1(sint32 d)
{
	Assert( d >= 0 && d < k_NUM_DIFFBOXES );
	if ( d < 0 || d >= k_NUM_DIFFBOXES )
		return;

	for ( sint32 i = 0; i < k_NUM_DIFFBOXES; i++ )
		s_checkBox[ i ]->SetState( 0 );
	s_checkBox[ d ]->SetState( 1 );

	g_theProfileDB->SetDifficulty( s_difficulty1 = d );
}

void
spnewgamediffscreen_setDifficulty2(sint32 d)
{
	Assert( d >= 0 && d < k_NUM_RISKBOXES );
	if ( d < 0 || d >= k_NUM_RISKBOXES )
		return;

	for ( sint32 i = 0; i < k_NUM_RISKBOXES; i++ )
		s_riskBox[ i ]->SetState( 0 );
	s_riskBox[ d ]->SetState( 1 );

	g_theProfileDB->SetRiskLevel( s_difficulty2 = d );
}

sint32
spnewgamediffscreen_displayMyWindow(BOOL viewMode,BOOL reinit)
{
	sint32 retval=0;
	if(!s_spNewGameDiffScreen) { retval = spnewgamediffscreen_Initialize(); }

	if(reinit)
	{
		s_spNewGameDiffScreen->Ok()->SetActionFuncAndCookie(spnewgamediffscreen_backPress,NULL);
	}

	AUI_ERRCODE auiErr;

	sint32 i;
	for ( i = 0;i < k_NUM_DIFFBOXES;i++ )
		s_checkBox[ i ]->Enable( !viewMode );
	for ( i = 0;i < k_NUM_RISKBOXES;i++ )
		s_riskBox[ i ]->Enable( !viewMode );

	auiErr = g_c3ui->AddWindow(s_spNewGameDiffScreen);
	keypress_RegisterHandler(s_spNewGameDiffScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}

sint32
spnewgamediffscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	uint32 id = s_group->WhichIsSelected();

	if ( id ) {
		for ( uint32 i = 0;i < k_NUM_DIFFBOXES;i ++ ) {
			if ( id == s_checkBox[i]->Id() ) {
				spnewgamediffscreen_setDifficulty1( i );
			}
		}
	}

	id = s_groupTwo->WhichIsSelected();

	if ( id ) {
		for ( uint32 i = 0;i < k_NUM_RISKBOXES;i ++ ) {
			if ( id == s_riskBox[i]->Id() ) {
				spnewgamediffscreen_setDifficulty2( i );
			}
		}
	}

	AUI_ERRCODE auiErr;

	auiErr = g_c3ui->RemoveWindow( s_spNewGameDiffScreen->Id() );
	keypress_RemoveHandler(s_spNewGameDiffScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}

AUI_ERRCODE
spnewgamediffscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		switchBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sint32 i;

	if ( s_spNewGameDiffScreen ) {
		if(callback) {
			s_spNewGameDiffScreen->Ok()->SetActionFuncAndCookie(callback, NULL);
		}
		return AUI_ERRCODE_OK;
	}

	strcpy(windowBlock, "SPNewGameDiffScreen");

	{
		s_spNewGameDiffScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(s_spNewGameDiffScreen, errcode) );

		s_spNewGameDiffScreen->Resize(s_spNewGameDiffScreen->Width(),s_spNewGameDiffScreen->Height());
		s_spNewGameDiffScreen->GrabRegion()->Resize(s_spNewGameDiffScreen->Width(),s_spNewGameDiffScreen->Height());
		s_spNewGameDiffScreen->SetStronglyModal(TRUE);
	}

	if ( !callback ) callback = spnewgamediffscreen_backPress;

	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	s_spNewGameDiffScreen->AddTitle( controlBlock );
	s_spNewGameDiffScreen->AddClose( callback );


	s_skillLevel = spNew_c3_Static(&errcode,windowBlock,"SkillLevel");
	s_barbarianThreat = spNew_c3_Static(&errcode,windowBlock,"BarbarianThreat");

	sprintf( controlBlock, "%s.%s", windowBlock, "Group" );
	s_group = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_group, errcode) );
	if ( !AUI_NEWOK(s_group, errcode) ) return errcode;

	s_checkBox = new aui_Radio*[k_NUM_DIFFBOXES];

	for ( i = 0;i < k_NUM_DIFFBOXES;i++ ) {
		sprintf( switchBlock, "%s.%s", controlBlock, checknames[i] );
		s_checkBox[i] = new aui_Radio( &errcode, aui_UniqueId(), switchBlock );
		Assert( AUI_NEWOK(s_checkBox[i], errcode) );
		if ( !AUI_NEWOK(s_checkBox[i], errcode) ) return errcode;
		s_group->AddSwitch( (aui_Radio *)s_checkBox[i] );

	}

	Assert( g_theProfileDB->GetDifficulty() >= 0 );
	Assert( g_theProfileDB->GetDifficulty() < k_NUM_DIFFBOXES );
	s_checkBox[g_theProfileDB->GetDifficulty()]->SetState( 1 );

	sprintf( controlBlock, "%s.%s", windowBlock, "GroupTwo" );
	s_groupTwo = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );
	Assert( AUI_NEWOK(s_groupTwo, errcode) );
	if ( !AUI_NEWOK(s_groupTwo, errcode) ) return errcode;

	s_riskBox = new aui_Radio*[k_NUM_RISKBOXES];

	for ( i = 0;i < k_NUM_RISKBOXES;i++ ) {
		sprintf( switchBlock, "%s.%s", controlBlock, risknames[i] );
		s_riskBox[i] = new aui_Radio( &errcode, aui_UniqueId(), switchBlock );
		Assert( AUI_NEWOK(s_riskBox[i], errcode) );
		if ( !AUI_NEWOK(s_riskBox[i], errcode) ) return errcode;
		s_groupTwo->AddSwitch( (aui_Radio *)s_riskBox[i] );

	}

	s_riskBox[g_theProfileDB->GetRiskLevel()]->SetState( 1 );

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgamediffscreen_Cleanup
//
// Description: Release the memory of the screen.
//
// Parameters : -
//
// Globals    : s_spNewGameDiffScreen
//				s_barbarianThreat
//				s_checkBox
//				s_riskBox
//				s_skillLevel
//				s_group
//				s_groupTwo
//
// Returns    : AUI_ERRCODE	: always AUI_ERRCODE_OK
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
AUI_ERRCODE
spnewgamediffscreen_Cleanup()
{
	if (s_spNewGameDiffScreen)
	{
		g_c3ui->RemoveWindow(s_spNewGameDiffScreen->Id());
		keypress_RemoveHandler(s_spNewGameDiffScreen);

		sint32 i;
		for (i = 0; i < k_NUM_DIFFBOXES; i++)
		{
			delete s_checkBox[i];
			// NULLing not necessary: deleting container next
		}
		delete [] s_checkBox;
		s_checkBox	= NULL;

		for (i = 0; i < k_NUM_RISKBOXES; ++i)
		{
			delete s_riskBox[i];
			// NULLing not necessary: deleting container next
		}
		delete [] s_riskBox;
		s_riskBox	= NULL;

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; }
		mycleanup(s_group);
		mycleanup(s_groupTwo);
		mycleanup(s_skillLevel);
		mycleanup(s_barbarianThreat);
		mycleanup(s_spNewGameDiffScreen);
#undef mycleanup
	}

	return AUI_ERRCODE_OK;
}

void
spnewgamediffscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{


	spnewgamediffscreen_removeMyWindow(action) ;
}
