//----------------------------------------------------------------------------
//
// Project      : Call To Power 2
// File type    : C++ source
// Description  : Age Screen
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
// - Starting and ending age selection screen now uses the age names from
//   gl_str.txt, Martin G�hmann.
// - Compatibility restored.
// - Memory leak repaired.
// - Initialized local variables. (Sep 9th 2005 Martin G�hmann)
// - Added single-player start and end age. (11-Apr-2009 Maq)
//
//----------------------------------------------------------------------------

#include "c3.h"
#include "agesscreen.h"

#include "AgeRecord.h"
#include "aui_stringtable.h"
#include "c3_button.h"
#include "c3_listitem.h"
#include "c3ui.h"
#include "c3window.h"
#include "ctp2_dropdown.h"
#include "dialogboxwindow.h"
#include "Globals.h"                // allocated::...
#include "ns_gamesetup.h"
#include "ns_item.h"
#include "spnewgamewindow.h"
#include "StrDB.h"                  // g_theStringDB
#include "profileDB.h"              // g_theProfileDB

extern nf_GameSetup g_gamesetup;
extern C3UI	*       g_c3ui;

static DialogBoxWindow *s_agesScreen	= NULL;

static aui_Button		*s_back				= NULL;
static c3_Static		*s_name				= NULL;
static c3_Static		*s_start			= NULL;
static c3_Static		*s_end				= NULL;
static ctp2_DropDown	*s_startDropDown	= NULL;
static ctp2_DropDown	*s_endDropDown		= NULL;


static sint32 s_numAges = 0;
static sint32 s_startAge = 0;
static sint32 s_endAge = s_numAges;
sint32 agesscreen_getStartAge( void )
{
	return s_startAge;
}
sint32 agesscreen_getEndAge( void )
{
	return s_endAge;
}






void agesscreen_setStartAge( sint32 index )
{
	Assert( index >= 0 && index < s_numAges );
	if ( index < 0 || index >= s_numAges )
		return;

	s_startDropDown->SetSelectedItem( index );

	s_startAge = index;
	g_gamesetup.SetStartAge(static_cast<char>(index));
	g_theProfileDB->SetSPStartingAge(index);
}






void agesscreen_setEndAge( sint32 index )
{
	Assert( index >= 0 && index < s_numAges );
	if ( index < 0 || index >= s_numAges )
		return;

	s_endDropDown->SetSelectedItem( index );

	s_endAge = index;
	g_gamesetup.SetEndAge(static_cast<char>(index));
	g_theProfileDB->SetSPEndingAge(index);
}





sint32	agesscreen_displayMyWindow(bool viewMode)
{
    sint32 retval = s_agesScreen ? 0 : agesscreen_Initialize();

	s_startDropDown->Enable( !viewMode );
	s_endDropDown->Enable( !viewMode );

	AUI_ERRCODE auiErr = g_c3ui->AddWindow(s_agesScreen);
	Assert(auiErr == AUI_ERRCODE_OK);

	return retval;
}

sint32 agesscreen_removeMyWindow(uint32 action)
{
	if ( action != (uint32)AUI_BUTTON_ACTION_EXECUTE ) return 0;

	agesscreen_setStartAge( s_startDropDown->GetSelectedItem() );
	agesscreen_setEndAge( s_endDropDown->GetSelectedItem() );
	g_theProfileDB->SetSPStartingAge( s_startDropDown->GetSelectedItem() );
	g_theProfileDB->SetSPEndingAge( s_endDropDown->GetSelectedItem() );

	AUI_ERRCODE auiErr = g_c3ui->RemoveWindow(s_agesScreen->Id());
	Assert(auiErr == AUI_ERRCODE_OK);

	return 1;
}


AUI_ERRCODE agesscreen_Initialize( aui_Control::ControlActionCallback *callback )
{
	if ( s_agesScreen ) return AUI_ERRCODE_OK;

	MBCHAR		windowBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	strcpy(windowBlock, "agesscreen");

	AUI_ERRCODE errcode = AUI_ERRCODE_OK;

	s_agesScreen = new DialogBoxWindow(
		&errcode,
		windowBlock,
		NULL );
	Assert( AUI_NEWOK(s_agesScreen, errcode) );
	if ( !AUI_NEWOK(s_agesScreen, errcode) ) errcode;

	s_back = new aui_Button(
		&errcode,
		aui_UniqueId(),
		"agesscreen.closebutton" );
	Assert( AUI_NEWOK(s_back,errcode) );
	if ( !AUI_NEWOK(s_back,errcode) ) return errcode;

	if ( !callback ) callback = agesscreen_backPress;
	s_back->SetActionFuncAndCookie(callback, NULL);

	s_name = spNew_c3_Static(&errcode,windowBlock,"NameStatic");
	s_start = spNew_c3_Static(&errcode,windowBlock,"StartStatic");
	s_end = spNew_c3_Static(&errcode,windowBlock,"EndStatic");

	MBCHAR		controlBlock[ k_AUI_LDL_MAXBLOCK + 1 ];
	sprintf( controlBlock, "%s.%s", windowBlock, "StartDropDown" );

	s_startDropDown = new ctp2_DropDown(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		agesscreen_startDropDownCallback,
		NULL );
	Assert( AUI_NEWOK(s_startDropDown, errcode) );
	if ( !AUI_NEWOK(s_startDropDown, errcode) ) return errcode;

	sprintf( controlBlock, "%s.%s", windowBlock, "EndDropDown" );

	s_endDropDown = new ctp2_DropDown(
		&errcode,
		aui_UniqueId(),
		controlBlock,
		agesscreen_endDropDownCallback,
		NULL );
	Assert( AUI_NEWOK(s_endDropDown, errcode) );
	if ( !AUI_NEWOK(s_endDropDown, errcode) ) return errcode;

	aui_StringTable	startagestrings(&errcode, "strings.startagestrings");
	s_numAges = g_theAgeDB->NumRecords();
	bool const		isLdlUsable = s_numAges == startagestrings.GetNumStrings();
	for (sint32 i = 0; i < s_numAges; i++)
	{
//Added by Martin G�hmann so that no *.ldl needs to be edited
//anymore when new ages are added.
		MBCHAR const *	ageId	= g_theAgeDB->GetNameStr(i);
		MBCHAR const *	name	= g_theAgeDB->Get(i)->GetNameText();

		if (isLdlUsable && (0 == strcmp(ageId, name)))
		{
			// Age name not defined in gl_str.txt: use the ldl file text.
			name = startagestrings.GetString(i);
		}
		else if (!name)
		{
			name = ageId;
		}






		{
			ns_ListItem *item = new ns_ListItem(
				&errcode,
				name,
				"listitems.ageitem" );
			Assert( AUI_NEWOK(item,errcode) );
			if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

			s_startDropDown->AddItem( (ctp2_ListItem *)item );
		}







		{

			ns_ListItem *item = new ns_ListItem(
				&errcode,
				name,
				"listitems.ageitem" );
			Assert( AUI_NEWOK(item,errcode) );
			if ( !AUI_NEWOK(item,errcode) ) return AUI_ERRCODE_MEMALLOCFAILED;

			s_endDropDown->AddItem( (ctp2_ListItem *)item );
		}
	}

	agesscreen_setStartAge(0);
	agesscreen_setEndAge(s_numAges - 1);

	errcode = aui_Ldl::SetupHeirarchyFromRoot( windowBlock );
	Assert( AUI_SUCCESS(errcode) );

	if (s_back) {
		s_back->Move( s_agesScreen->Width() - s_back->Width() - 14, s_agesScreen->Height() - s_back->Height() - 17);
	}

	return AUI_ERRCODE_OK;
}


//----------------------------------------------------------------------------
//
// Name       : agesscreen_Cleanup
//
// Description: Release the memory of all static data from this file.
//
// Parameters : -
//
// Globals    : All static variables.
//
// Returns    : -
//
// Remark(s)  : -
//
//----------------------------------------------------------------------------
void agesscreen_Cleanup()
{
	if (s_startDropDown)
	{
		s_startDropDown->Clear();
        allocated::clear(s_startDropDown);
	}
	if (s_endDropDown)
	{
		s_endDropDown->Clear();
        allocated::clear(s_endDropDown);
	}

    allocated::clear(s_name);
	allocated::clear(s_start);
	allocated::clear(s_end);
	allocated::clear(s_back);

	if (s_agesScreen)
	{
		g_c3ui->RemoveWindow(s_agesScreen->Id());
		allocated::clear(s_agesScreen);
	}
}





void agesscreen_startDropDownCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	Cookie cookie )
{
	if ( action != (uint32)AUI_DROPDOWN_ACTION_SELECT ) return;




#if 0

	if ( s_startDropDown->GetSelectedItem() == s_numAges - 1 )
	{
		s_startDropDown->SetSelectedItem( s_numAges - 2 );
	}
	else
	{
		if ( s_startDropDown->GetSelectedItem() >=
			 s_endDropDown->GetSelectedItem() )
			s_endDropDown->SetSelectedItem(
				s_startDropDown->GetSelectedItem() + 1 );
	}
#else
	if ( s_startDropDown->GetSelectedItem() >
		 s_endDropDown->GetSelectedItem() )
		s_endDropDown->SetSelectedItem(
			s_startDropDown->GetSelectedItem() );
#endif

}
void agesscreen_endDropDownCallback(
	aui_Control *control,
	uint32 action,
	uint32 data,
	Cookie cookie )
{
	if ( action != (uint32)AUI_DROPDOWN_ACTION_SELECT ) return;




#if 0

	if ( s_endDropDown->GetSelectedItem() == 0 )
	{
		s_endDropDown->SetSelectedItem( 1 );
	}
	else
	{
		if ( s_endDropDown->GetSelectedItem() <=
			 s_startDropDown->GetSelectedItem() )
			s_startDropDown->SetSelectedItem(
				s_endDropDown->GetSelectedItem() - 1 );
	}
#else
	if ( s_endDropDown->GetSelectedItem() <
		 s_startDropDown->GetSelectedItem() )
		s_startDropDown->SetSelectedItem(
			s_endDropDown->GetSelectedItem() );
#endif

}
void agesscreen_backPress(aui_Control *control, uint32 action, uint32 data, Cookie cookie )
{


	agesscreen_removeMyWindow(action);
}
