//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Single player new game civ selection screen
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
// USE_SELECT_BARBARIANS    : make Barbarians selectable
// USE_SHOW_BARBARIANS      : make Barbarians visible
//
//----------------------------------------------------------------------------
//
// Modifications from the original Activision code:
//
// - Memory leaks repaired.
// - Replaced old civ selection button bank by list box. (2-Jan-2008 Martin Gühmann)
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

//original tribe screen includes

#include "c3.h"                     // Precompiled header
#include "spnewgametribescreen.h"   // Own declarations: consistency check

#include "aui_radio.h"
#include "aui_switchgroup.h"
#include "aui_uniqueid.h"
#include "c3_button.h"
#include "c3_dropdown.h"
#include "c3_listitem.h"
#include "c3_popupwindow.h"
#include "c3_static.h"
#include "c3slider.h"
#include "c3textfield.h"
#include "c3ui.h"
#include "c3window.h"
#include "Civilisation.h"
#include "CivilisationRecord.h"
#include "gamefile.h"
#include "keypress.h"
#include "player.h"                 // g_player
#include "profileDB.h"              // g_theProfileDB
#include "SelItem.h"
#include "spnewgamewindow.h"
#include "StrDB.h"                  // g_theStringDB
#include "textradio.h"
#include <vector>                   // std::vector
//end includes

extern C3UI             *g_c3ui;
extern ProfileDB        *g_theProfileDB;
extern sint32               g_isCheatModeOn;

c3_PopupWindow              *g_spNewGameTribeScreen  = NULL; //added
static ctp2_ListBox         *s_CivListBox            = NULL;
//original gender buttons
static c3_Static			*s_leaderNameStatic		 = NULL;
static C3TextField			*s_leaderNameTextField	 = NULL;
static aui_SwitchGroup		*s_maleFemaleSwitchGroup = NULL;
static GENDER				s_gender;
sint32  const               INDEX_TRIBE_INVALID = -1;
aui_Radio					*s_maleRadio = NULL;
aui_Radio					*s_femaleRadio = NULL;
sint32						s_tribeIndex    = INDEX_TRIBE_INVALID;
sint32						s_skipIndex = 0;

sint32 spnewgametribescreen_getTribeIndex( void )
{
	return s_tribeIndex;
}

void spnewgametribescreen_setTribeIndex( sint32 index, MBCHAR *lname )
{

	if (index < 0)
	{
		s_tribeIndex = INDEX_TRIBE_INVALID;
		return;
	}

	//s_CivListBox->SelectItem(index);

	for(sint32 i = 0; i < s_CivListBox->NumItems(); ++i)
	{
		ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetItemByIndex(i);
		if(index == reinterpret_cast<sint32>(item->GetUserData()))
		{
			s_CivListBox->SelectItem(i);
		}
	}

	s_tribeIndex = index;

	sint32 const    playerIndex         =
	    g_selected_item ? g_selected_item->GetVisiblePlayer() : 1;
	bool const      shouldSetProfileDB  =
	    !g_isCheatModeOn ||
	     (g_player[playerIndex] &&
	      g_player[playerIndex]->IsHuman()
	     );

	if (shouldSetProfileDB)
		g_theProfileDB->SetCivIndex(index);

	StringId civString = g_theCivilisationDB->Get(index)->GetPluralCivName();

	if (shouldSetProfileDB)
		g_theProfileDB->SetCivName((MBCHAR *)g_theStringDB->GetNameStr(civString));

	s_gender = s_maleRadio->GetState() ? GENDER_MALE : GENDER_FEMALE;
	if (shouldSetProfileDB)
	{
		g_theProfileDB->SetGender(s_gender);
	}

	if ( lname )
	{
		s_leaderNameTextField->SetFieldText( lname );

		if (shouldSetProfileDB)
			g_theProfileDB->SetLeaderName( lname );

		spnewgamescreen_setPlayerName( lname );
	}
	else
	{

		StringId nameString = g_theCivilisationDB->Get(index)->GetLeaderNameMale();

		if ( s_maleRadio->GetState() )
			nameString = g_theCivilisationDB->Get(index)->GetLeaderNameMale();
		else
			nameString = g_theCivilisationDB->Get(index)->GetLeaderNameFemale();

		s_leaderNameTextField->SetFieldText(g_theStringDB->GetNameStr(nameString) );

		if (shouldSetProfileDB)
			g_theProfileDB->
				SetLeaderName((MBCHAR *)g_theStringDB->GetNameStr(nameString));

		spnewgamescreen_setPlayerName(g_theStringDB->GetNameStr(nameString));
	}
}

sint32	spnewgametribescreen_displayMyWindow( void *cookie, BOOL edit )
{
	sint32 retval = g_spNewGameTribeScreen ? 0 : spnewgametribescreen_Initialize();

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(g_spNewGameTribeScreen);
	Assert(auiErr == AUI_ERRCODE_OK);

	keypress_RegisterHandler(g_spNewGameTribeScreen);

	if ( edit && !(g_isScenario && g_startInfoType == STARTINFOTYPE_NOLOCS))
		s_leaderNameTextField->Enable( TRUE );
	else
		s_leaderNameTextField->Enable( FALSE );

	g_spNewGameTribeScreen->Ok()->SetActionFuncAndCookie( g_spNewGameTribeScreen->Ok()->GetActionFunc(), cookie );

	return retval;
}

sint32 spnewgametribescreen_removeMyWindow(uint32 action, MBCHAR *lname)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetSelectedItem();
	sint32 index   = reinterpret_cast<sint32>(item->GetUserData());

	if ( lname )
		s_leaderNameTextField->GetFieldText( lname, 100 );

	spnewgametribescreen_setTribeIndex( index, lname );

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(g_spNewGameTribeScreen->Id());
	Assert( auiErr == AUI_ERRCODE_OK );

	keypress_RemoveHandler(g_spNewGameTribeScreen);
	spnewgamescreen_update();

	return 1;
}

AUI_ERRCODE spnewgametribescreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	AUI_ERRCODE errcode = AUI_ERRCODE_OK;
	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	s_tribeIndex = INDEX_TRIBE_INVALID;
	if ( g_spNewGameTribeScreen ) {
		if(callback) {
			g_spNewGameTribeScreen->Ok()->SetActionFuncAndCookie(callback, NULL);
		}
		return AUI_ERRCODE_OK;
	}

	strcpy(windowBlock, "SPNewGameTribeScreen");

	{
		g_spNewGameTribeScreen = new c3_PopupWindow( &errcode, aui_UniqueId(), windowBlock, 16, AUI_WINDOW_TYPE_FLOATING, false);
		Assert( AUI_NEWOK(g_spNewGameTribeScreen, errcode) );
		if ( !AUI_NEWOK(g_spNewGameTribeScreen, errcode) ) errcode;

		g_spNewGameTribeScreen->Resize(g_spNewGameTribeScreen->Width(),g_spNewGameTribeScreen->Height());
		g_spNewGameTribeScreen->GrabRegion()->Resize(g_spNewGameTribeScreen->Width(),g_spNewGameTribeScreen->Height());
		g_spNewGameTribeScreen->SetStronglyModal(TRUE);
	}

	if ( !callback ) callback = spnewgametribescreen_backPress;

	sprintf( controlBlock, "%s.%s", windowBlock, "Name" );
	g_spNewGameTribeScreen->AddTitle( controlBlock );
	g_spNewGameTribeScreen->AddClose( callback );

	s_CivListBox = (ctp2_ListBox *)aui_Ldl::BuildHierarchyFromRoot("SPNewGameTribeScreen.CivBox");

	Assert(s_CivListBox);

	spnewgametribescreen_addAllTribes();

	sprintf( controlBlock, "%s.%s", windowBlock, "leadernamestatic" );
	s_leaderNameStatic = new c3_Static(&errcode, aui_UniqueId(), controlBlock );

	sprintf( controlBlock, "%s.%s", windowBlock, "leadernametextfield" );
	s_leaderNameTextField = new C3TextField(&errcode, aui_UniqueId(), controlBlock );

	sprintf( controlBlock, "%s.%s", windowBlock, "malefemaleswitchgroup" );
	s_maleFemaleSwitchGroup = new aui_SwitchGroup( &errcode, aui_UniqueId(), controlBlock );

	sprintf( controlBlock, "%s.%s", windowBlock, "malefemaleswitchgroup.maleradio" );
	s_maleRadio = new aui_Radio(&errcode, aui_UniqueId(), controlBlock, spnewgametribescreen_malePress );

	sprintf( controlBlock, "%s.%s", windowBlock, "malefemaleswitchgroup.femaleradio" );
	s_femaleRadio = new aui_Radio(&errcode, aui_UniqueId(), controlBlock, spnewgametribescreen_femalePress );

	if(g_isScenario && g_startInfoType == STARTINFOTYPE_NOLOCS)
	{
		s_leaderNameTextField->Enable(FALSE);
		s_maleRadio->Enable(FALSE);
		s_femaleRadio->Enable(FALSE);
	}
	else
	{
		s_leaderNameTextField->Enable(TRUE);
		s_maleRadio->Enable(TRUE);
		s_femaleRadio->Enable(TRUE);
	}

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	s_CivListBox->SetForceSelect(TRUE);
	s_CivListBox->SetMultiSelect(FALSE);
	s_CivListBox->SetActionFuncAndCookie(spnewgametribescreen_switchPress, NULL);

	return AUI_ERRCODE_OK;
}

//----------------------------------------------------------------------------
//
// Name       : spnewgametribescreen_Cleanup
//
// Description: Release the memory of the screen.
//
// Parameters : -
//
// Globals    : g_spNewGameTribeScreen
//              s_barbarianThreat
//              s_riskBox
//              s_skillLevel
//              s_CivListBox
//              s_riskListBox
//
// Returns    : AUI_ERRCODE : always AUI_ERRCODE_OK
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
AUI_ERRCODE spnewgametribescreen_Cleanup()
{
	if (g_spNewGameTribeScreen)
	{
		g_c3ui->RemoveWindow(g_spNewGameTribeScreen->Id());
		keypress_RemoveHandler(g_spNewGameTribeScreen);

		allocated::clear(s_CivListBox);
		allocated::clear(s_leaderNameStatic);
		allocated::clear(s_leaderNameTextField);
		allocated::clear(s_maleFemaleSwitchGroup);
		allocated::clear(s_maleRadio);
		allocated::clear(s_femaleRadio);
		allocated::clear(g_spNewGameTribeScreen);
	}

	return AUI_ERRCODE_OK;
}

//original code for gender, etc

void spnewgametribescreen_getLeaderName(MBCHAR *lname)
{
	s_leaderNameTextField->GetFieldText(lname, 100);
}

GENDER spnewgametribescreen_getGender(void)
{
	return s_gender;
}

//action buttons
void spnewgametribescreen_acceptPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	spnewgametribescreen_removeMyWindow(action);
}

void spnewgametribescreen_cancelPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	spnewgametribescreen_removeMyWindow(action);
}

void spnewgametribescreen_backPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	const sint32 size = 100;
	MBCHAR lname[ size + 1 ];
	spnewgametribescreen_getLeaderName( lname );

	spnewgametribescreen_removeMyWindow(action, lname);
}

void spnewgametribescreen_switchPress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	//this is for the text boxes and updated once selected
	if ( action != (uint32)AUI_LISTBOX_ACTION_SELECT ) return;

	ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetSelectedItem();
	sint32 civ = reinterpret_cast<sint32>(item->GetUserData());
	StringId nameString;

	if ( s_maleRadio->GetState() )
		nameString = g_theCivilisationDB->Get(civ)->GetLeaderNameMale();
	else
		nameString = g_theCivilisationDB->Get(civ)->GetLeaderNameFemale();

	s_leaderNameTextField->SetFieldText( g_theStringDB->GetNameStr(nameString) );
}

void spnewgametribescreen_malePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetSelectedItem();

	if(item)
	{
		sint32 civ = reinterpret_cast<sint32>(item->GetUserData());

		StringId const  nameString  = g_theCivilisationDB->Get(civ)->GetLeaderNameMale();

		s_leaderNameTextField->SetFieldText(g_theStringDB->GetNameStr(nameString));
	}
}

void spnewgametribescreen_femalePress(aui_Control *control, uint32 action, uint32 data, void *cookie )
{
	if ( action != (uint32)AUI_SWITCH_ACTION_ON ) return;

	ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetSelectedItem();

	if(item)
	{
		sint32 civ = reinterpret_cast<sint32>(item->GetUserData());

		StringId const  nameString  = g_theCivilisationDB->Get(civ)->GetLeaderNameFemale();

		s_leaderNameTextField->SetFieldText(g_theStringDB->GetNameStr(nameString));
	}
}

void spnewgametribescreen_clearTribes()
{
	s_CivListBox->Clear();
}

void spnewgametribescreen_addTribeNoDuplicate(sint32 tribe)
{
	for(sint32 i = 0; i < s_CivListBox->NumItems(); i++)
	{
		ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetItemByIndex(i);
		if(tribe == reinterpret_cast<sint32>(item->GetUserData()))
		{
			return;
		}
	}

	spnewgametribescreen_addTribe(tribe);

	s_CivListBox->SortByColumn(0, true);
}

void spnewgametribescreen_addTribe(sint32 tribe)
{
#if defined(USE_SHOW_BARBARIANS) || defined(USE_SELECT_BARBARIANS)
	// Always create a list item
#else
	if (PLAYER_INDEX_VANDALS == tribe) // PLAYER_INDEX_VANDALS obsolete
	{
		s_skipIndex = tribe;
	}
	else
#endif
	{
		ctp2_ListItem *item = (ctp2_ListItem *)aui_Ldl::BuildHierarchyFromRoot("CivItem");
		Assert(item);
		if(!item)
			return;

		item->SetUserData((void *)tribe);

		ctp2_Static *text = (ctp2_Static *)item->GetChildByIndex(0);
		Assert(text);
		if(!text)
			return;

		text->SetText(g_theStringDB->GetNameStr(g_theCivilisationDB->Get(tribe)->GetPluralCivName()));  //singular? // Maybe change to civ name and then we can change it to Roman Empire, instead of having the boring name Romans
		item->SetCompareCallback(spnewgametribescreen_CompareItems);
		s_CivListBox->AddItem(item);
	}
}

void spnewgametribescreen_removeTribe(sint32 tribe)
{
	for(sint32 i = s_CivListBox->NumItems() - 1; i >= 0; i--)
	{
		ctp2_ListItem *item = (ctp2_ListItem *)s_CivListBox->GetItemByIndex(i);
		if(tribe == reinterpret_cast<sint32>(item->GetUserData()))
		{
			s_CivListBox->RemoveItemByIndex(i);
			delete item;
			return;
		}
	}
}

void spnewgametribescreen_addAllTribes()
{
	for(sint32 i = 0; i < g_theCivilisationDB->NumRecords(); ++i)
	{
		spnewgametribescreen_addTribe(i);
	}

	s_CivListBox->SortByColumn(0, true);
}

sint32 spnewgametribescreen_CompareItems(ctp2_ListItem *item1, ctp2_ListItem *item2, sint32 column)
{
	ctp2_Static *text1 = (ctp2_Static *)item1->GetChildByIndex(0);
	ctp2_Static *text2 = (ctp2_Static *)item2->GetChildByIndex(0);

	return _stricoll(text1->GetText(), text2->GetText());
}
