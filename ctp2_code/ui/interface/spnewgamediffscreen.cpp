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
// - Replaced old difficulty and risk level selection button banks by new
//   list boxes. (8-Jul-2007 Martin Gühmann)
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

#include "ctp2_ListBox.h"
#include "ctp2_ListItem.h"
#include "ctp2_static.h"
#include "DifficultyRecord.h"
#include "RiskRecord.h"

#include "keypress.h"

extern C3UI             *g_c3ui;
extern ProfileDB        *g_theProfileDB;

static c3_PopupWindow   *s_spNewGameDiffScreen  = NULL;
static ctp2_ListBox     *s_diffListBox          = NULL;
static ctp2_ListBox     *s_riskListBox          = NULL;

static c3_Static *       s_skillLevel           = NULL; // Skill level selection
static c3_Static *       s_barbarianThreat      = NULL; // Barbarian threat level selection

static sint32            s_difficulty1          = 0;
static sint32            s_difficulty2          = 0;

sint32 spnewgamediffscreen_getDifficulty1( void )
{
	return s_difficulty1;
}

sint32 spnewgamediffscreen_getDifficulty2( void )
{
	return s_difficulty2;
}

void spnewgamediffscreen_setDifficulty1( sint32 d )
{
	Assert( d >= 0 && d < g_theDifficultyDB->NumRecords() );
	if ( d < 0 || d >= g_theDifficultyDB->NumRecords() )
	{
		s_difficulty1 = (s_difficulty1 >= 0 && s_difficulty1 < g_theDifficultyDB->NumRecords()) ? s_difficulty1 : 0;
		s_diffListBox->SelectItem(s_difficulty1);
		return;
	}

	Assert( d >= 0 && d < g_theDifficultyDB->NumRecords() );
	s_diffListBox->SelectItem(d);

	g_theProfileDB->SetDifficulty( s_difficulty1 = d );
}

void spnewgamediffscreen_setDifficulty2( sint32 d )
{
	Assert( d >= 0 && d < g_theRiskDB->NumRecords() );
	if ( d < 0 || d >= g_theRiskDB->NumRecords() )
	{
		s_difficulty2 = (s_difficulty2 >= 0 && s_difficulty2 < g_theRiskDB->NumRecords()) ? s_difficulty2 : 0;
		s_riskListBox->SelectItem(s_difficulty2);
		return;
	}

	s_riskListBox->SelectItem(d);

	g_theProfileDB->SetRiskLevel( s_difficulty2 = d );
}

sint32	spnewgamediffscreen_displayMyWindow(BOOL viewMode,BOOL reinit)
{
	sint32 retval=0;
	if(!s_spNewGameDiffScreen) { retval = spnewgamediffscreen_Initialize(); }

	if(reinit)
	{
		s_spNewGameDiffScreen->Ok()->SetActionFuncAndCookie(spnewgamediffscreen_backPress,NULL);
	}

	AUI_ERRCODE auiErr;

	sint32 i;
	for(i = 0; i < s_diffListBox->NumItems(); i++)
		s_diffListBox->GetItemByIndex(i)->Enable(!viewMode);
	for(i = 0; i < s_riskListBox->NumItems(); i++)
		s_riskListBox->GetItemByIndex(i)->Enable(!viewMode);

	auiErr = g_c3ui->AddWindow(s_spNewGameDiffScreen);
	keypress_RegisterHandler(s_spNewGameDiffScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	return retval;
}

sint32 spnewgamediffscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	spnewgamediffscreen_setDifficulty1(s_diffListBox->GetSelectedItemIndex());
	spnewgamediffscreen_setDifficulty2(s_riskListBox->GetSelectedItemIndex());

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow( s_spNewGameDiffScreen->Id() );
	keypress_RemoveHandler(s_spNewGameDiffScreen);

	Assert( auiErr == AUI_ERRCODE_OK );

	spnewgamescreen_update();

	return 1;
}

AUI_ERRCODE spnewgamediffscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];

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
		if ( !AUI_NEWOK(s_spNewGameDiffScreen, errcode) ) errcode;

		
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

	s_diffListBox = (ctp2_ListBox *)aui_Ldl::BuildHierarchyFromRoot("SPNewGameDiffScreen.DiffBox");
	s_riskListBox = (ctp2_ListBox *)aui_Ldl::BuildHierarchyFromRoot("SPNewGameDiffScreen.RiskBox");

	Assert(s_diffListBox);
	Assert(s_riskListBox);
	s_diffListBox->SetForceSelect(TRUE);
	s_riskListBox->SetForceSelect(TRUE);

	sint32 i;
	for(i = 0; i < g_theDifficultyDB->NumRecords(); ++i){
		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DiffRiskItem");
		Assert(item);
		if(!item)
			break;
				
		item->SetUserData((void *)i);
				
		ctp2_Static *text = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(text);
		if(!text)
			break;

		text->SetText(g_theDifficultyDB->Get(i)->GetNameText());
		s_diffListBox->AddItem(item);
	}

	Assert( g_theProfileDB->GetDifficulty() >= 0 );
	if(g_theProfileDB->GetDifficulty() < 0)
		g_theProfileDB->SetDifficulty(0);

	// In the case of a mod change and the mods have different number of difficulties:
	if(g_theProfileDB->GetDifficulty() >= g_theDifficultyDB->NumRecords())
		g_theProfileDB->SetDifficulty(g_theDifficultyDB->NumRecords() - 1);

	s_diffListBox->SelectItem(g_theProfileDB->GetDifficulty());

	for(i = 0; i < g_theRiskDB->NumRecords(); ++i){
		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("DiffRiskItem");
		Assert(item);
		if(!item)
			break;
				
		item->SetUserData((void *)i);
				
		ctp2_Static *text = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(text);
		if(!text)
			break;

		text->SetText(g_theRiskDB->Get(i)->GetNameText());
		s_riskListBox->AddItem(item);
	}

	Assert( g_theProfileDB->GetRiskLevel() >= 0 );
	if(g_theProfileDB->GetRiskLevel() < 0)
		g_theProfileDB->SetRiskLevel(0);

	// In the case of a mod change and the mods have different number of difficulties:
	if(g_theProfileDB->GetRiskLevel() >= g_theRiskDB->NumRecords())
		g_theProfileDB->SetRiskLevel(g_theRiskDB->NumRecords() - 1);

	s_riskListBox->SelectItem(g_theProfileDB->GetRiskLevel());

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
//              s_barbarianThreat
//              s_riskBox
//              s_skillLevel
//              s_diffListBox
//              s_riskListBox
//
// Returns    : AUI_ERRCODE : always AUI_ERRCODE_OK
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
AUI_ERRCODE spnewgamediffscreen_Cleanup()
{
	if (s_spNewGameDiffScreen)
	{
		g_c3ui->RemoveWindow(s_spNewGameDiffScreen->Id());
		keypress_RemoveHandler(s_spNewGameDiffScreen);

#define mycleanup(mypointer) { delete mypointer; mypointer = NULL; }
		mycleanup(s_diffListBox);
		mycleanup(s_riskListBox);
		mycleanup(s_skillLevel);
		mycleanup(s_barbarianThreat);
		mycleanup(s_spNewGameDiffScreen);
#undef mycleanup
	}

	return AUI_ERRCODE_OK;
}

void spnewgamediffscreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	spnewgamediffscreen_removeMyWindow(action) ;
}
